/**
 * @file ginn/geisgesturesource.cpp
 * @brief Implementation of the GEIS gesture source.
 */

/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ginn/geisgesturesource.h"

#include <geis/geis.h>
#include "ginn/configuration.h"
#include <glib.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>


namespace Ginn
{

struct GeisGestureEvent
: public GestureEvent
{
  GeisGestureEvent(GeisEvent geis_event)
  {
    GeisAttr attr = geis_event_attr_by_name(geis_event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
    GeisGroupSet groupset = static_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));
    for (GeisSize i= 0; i < geis_groupset_group_count(groupset); ++i)
    {
      GeisGroup group = geis_groupset_group(groupset, i);
      for (GeisSize j=0; j < geis_group_frame_count(group); ++j)
      {
        GeisFrame frame = geis_group_frame(group, j);
        attr = geis_frame_attr_by_name(frame, GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID);
        if (attr)
        {
          Window::Id id = geis_attr_value_to_integer(attr);
          frames_[id] = frame;
        }
      }
    }
  }

  bool
  matches(Window const* window, Wish::Ptr const& wish) const
  {
    if (frames_.find(window->id_) != frames_.end())
    {
      GeisFrame frame = frames_.at(window->id_);
      GeisAttr attr = geis_frame_attr_by_name(frame, wish->property().c_str());
      if (attr)
      {
          float fval = geis_attr_value_to_float(attr);
          return wish->min() <= fval && fval <= wish->max();
      }
    }
    return false;
  }

  std::map<Window::Id, GeisFrame> frames_;
};


struct GeisGestureSubscription
: public GestureSubscription
{
  GeisGestureSubscription(GeisSubscription geis_sub)
  : geis_sub_(geis_sub)
  { }

  ~GeisGestureSubscription()
  { }

  GeisSubscription geis_sub_;
};


struct GeisGestureSource::Impl
{
  Impl(Configuration const& config);
  ~Impl();

  Configuration                            config_;
  ::Geis                                   geis_;
  GestureSource::EventReceivedCallback     event_received_callback_;
  GestureSource::InitializedCallback       initialized_callback_;
  GIOChannel*                              iochannel_;
  std::map<std::string, GeisGestureClass>  class_map_;
};


/**
 * GIO event handler callback, passes GEIS events on to GEIS.
 */
static gboolean
geis_gio_event_ready(GIOChannel*, GIOCondition, gpointer pdata)
{
  ::Geis geis = (::Geis)pdata;
  while (GEIS_STATUS_CONTINUE == geis_dispatch_events(geis))
    ;
  return TRUE;
}


/**
 * GEIS gesture event callback: handles gesture events asynchronously.
 */
static void
geis_gesture_event_ready(::Geis, GeisEvent geis_event, void* context)
{
  GeisGestureSource::Impl* impl = static_cast<GeisGestureSource::Impl*>(context);
  
  switch (geis_event_type(geis_event))
  {
    case GEIS_EVENT_INIT_COMPLETE:
      if (impl->initialized_callback_)
        impl->initialized_callback_();
      break;

    case GEIS_EVENT_ERROR:
      std::cerr << "failed to initialize gesture interface\n";
      break;

    case GEIS_EVENT_CLASS_AVAILABLE:
    {
      GeisAttr attr = geis_event_attr_by_name(geis_event, GEIS_EVENT_ATTRIBUTE_CLASS);
      GeisGestureClass gesture_class =
          static_cast<GeisGestureClass>(geis_attr_value_to_pointer(attr));
      char const* class_name = geis_gesture_class_name(gesture_class);
      impl->class_map_[class_name] = gesture_class;
      break;
    }

    case GEIS_EVENT_DEVICE_AVAILABLE:
    case GEIS_EVENT_DEVICE_UNAVAILABLE:
      break;

    case GEIS_EVENT_GESTURE_BEGIN:
    case GEIS_EVENT_GESTURE_UPDATE:
    case GEIS_EVENT_GESTURE_END:
    {
      GeisGestureEvent gesture_event(geis_event);
      if (impl->event_received_callback_)
        impl->event_received_callback_(gesture_event);
      break;
    }

    default:
      break;
  }
  geis_event_delete(geis_event);
}


GeisGestureSource::Impl::
Impl(Configuration const& config)
: config_(config)
, geis_(geis_new(GEIS_INIT_TRACK_DEVICES, GEIS_INIT_TRACK_GESTURE_CLASSES, NULL))
{
  if (!geis_)
    throw std::runtime_error("could not create GEIS instance");

  geis_register_device_callback(geis_, geis_gesture_event_ready, this);
  geis_register_class_callback(geis_, geis_gesture_event_ready, this);
  geis_register_event_callback(geis_, geis_gesture_event_ready, this);

  int fd = -1;
  geis_get_configuration(geis_, GEIS_CONFIGURATION_FD, &fd);
  iochannel_ = g_io_channel_unix_new(fd);
  g_io_add_watch(iochannel_, G_IO_IN, geis_gio_event_ready, geis_);
}


GeisGestureSource::Impl::
~Impl()
{
  g_io_channel_shutdown(iochannel_, FALSE, NULL);
  g_io_channel_unref(iochannel_);
  geis_delete(geis_);
}


GeisGestureSource::
GeisGestureSource(Configuration const& config)
: impl_(new Impl(config))
{
  if (impl_->config_.is_verbose_mode())
    std::cout << __FUNCTION__ << " created\n";
}


GeisGestureSource::
~GeisGestureSource()
{ }


void GeisGestureSource::
set_initialized_callback(InitializedCallback const& initialized_callback)
{
  impl_->initialized_callback_ = initialized_callback;
}


void GeisGestureSource::
set_event_callback(EventReceivedCallback const& event_callback)
{
  impl_->event_received_callback_ = event_callback;
}


GestureSubscription::Ptr GeisGestureSource::
subscribe(Window::Id window_id, Wish::Ptr const& wish)
{
  GeisSubscription geis_sub = geis_subscription_new(impl_->geis_,
                                                    wish->name().c_str(),
                                                    GEIS_SUBSCRIPTION_CONT);
  std::string filter_name = wish->name();
  GeisFilter filter = geis_filter_new(impl_->geis_, filter_name.c_str());
  geis_filter_add_term(filter, GEIS_FILTER_REGION,
           GEIS_REGION_ATTRIBUTE_WINDOWID, GEIS_FILTER_OP_EQ, window_id,
           NULL);
  geis_filter_add_term(filter, GEIS_FILTER_CLASS,
           GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, wish->gesture().c_str(),
           GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, wish->touches(),
           NULL);
  geis_subscription_add_filter(geis_sub, filter);
  geis_subscription_activate(geis_sub);

  return GestureSubscription::Ptr(new GeisGestureSubscription(geis_sub));
}


} // namespace Ginn

