/**
 * @file ginn/geis.cpp
 * @brief Implementation of the Ginn GEIS module.
 */

/*
 * Copyright 2013 Canonical Ltd.
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
#include "ginn/geis.h"

#include <geis/geis.h>
#include <glib.h>
#include <iostream>
#include <map>
#include <stdexcept>


namespace Ginn
{

Geis::Subscription::
Subscription(GeisSubscription subscription)
: subscription_(subscription)
{ }


Geis::Subscription::
Subscription(Subscription&& rhs)
: subscription_(rhs.subscription_)
{ rhs.subscription_ = 0; }


Geis::Subscription::
~Subscription()
{
  if (subscription_)
  {
    geis_subscription_delete(subscription_);
  }
}

 
/**
 * The internal implementation of the Ginn GEIS module.
 */
struct Geis::Impl
{
  Impl(Geis::EventReceivedCallback const& event_received_callback,
       Geis::InitializedCallback const&   initialized_callback);
  ~Impl();

  ::Geis                                  geis_;
  Geis::EventReceivedCallback             event_received_callback_;
  Geis::InitializedCallback               initialized_callback_;
  GIOChannel*                             iochannel_;
  int                                     iochannel_watch_;
  std::map<std::string, GeisGestureClass> class_map_;
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
void
geis_gesture_event_ready(::Geis, GeisEvent event, void* context)
{
  Geis::Impl* impl = static_cast<Geis::Impl*>(context);
  
  switch (geis_event_type(event))
  {
    case GEIS_EVENT_INIT_COMPLETE:
      impl->initialized_callback_();
      break;

    case GEIS_EVENT_ERROR:
      std::cerr << "failed to initialize gesture interface\n";
      break;

    case GEIS_EVENT_DEVICE_AVAILABLE:
    case GEIS_EVENT_DEVICE_UNAVAILABLE:
      break;

    case GEIS_EVENT_GESTURE_BEGIN:
    case GEIS_EVENT_GESTURE_UPDATE:
    case GEIS_EVENT_GESTURE_END:
      impl->event_received_callback_(event);
      break;

    default:
      break;
  }
  geis_event_delete(event);
}


/**
 * GEIS class event callback: handles class events asynchronously.
 */
void
geis_class_event_ready(::Geis, GeisEvent event, void* context)
{
  Geis::Impl* impl = static_cast<Geis::Impl*>(context);
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
  GeisGestureClass gesture_class =
      static_cast<GeisGestureClass>(geis_attr_value_to_pointer(attr));
  switch (geis_event_type(event))
  {
    case GEIS_EVENT_CLASS_AVAILABLE:
    {
      char const* class_name = geis_gesture_class_name(gesture_class);
      impl->class_map_[class_name] = gesture_class;
      break;
    }

    default:
      break;
  }
  geis_event_delete(event);
}


/**
 * Sets up the GEIS dispatch mechanism and lets it go wild.
 */
Geis::Impl::
Impl(Geis::EventReceivedCallback const& event_received_callback,
     Geis::InitializedCallback const&   initialized_callback)
: geis_(geis_new(GEIS_INIT_TRACK_DEVICES, GEIS_INIT_TRACK_GESTURE_CLASSES, NULL))
, event_received_callback_(event_received_callback)
, initialized_callback_(initialized_callback)
, iochannel_(NULL)
{
  if (!geis_)
    throw std::runtime_error("could not create GEIS instance");

  geis_register_device_callback(geis_, geis_gesture_event_ready, this);
  geis_register_class_callback(geis_, geis_class_event_ready, this);
  geis_register_event_callback(geis_, geis_gesture_event_ready, this);

  int fd = -1;
  geis_get_configuration(geis_, GEIS_CONFIGURATION_FD, &fd);
  iochannel_ = g_io_channel_unix_new(fd);
  iochannel_watch_ = g_io_add_watch(iochannel_,
                                    G_IO_IN,
                                    geis_gio_event_ready,
                                    geis_);
}


/**
 * Tears down the GEIS dispatch mech.
 */
Geis::Impl::
~Impl()
{
  g_io_channel_shutdown(iochannel_, FALSE, NULL);
  g_io_channel_unref(iochannel_);
  geis_delete(geis_);
}



Geis::
Geis(EventReceivedCallback const& event_received_callback,
     InitializedCallback const&   initialized_calback)
: impl_(new Impl(event_received_callback,
                 initialized_calback))
{
}


Geis::
~Geis()
{
}

Geis::Subscription Geis::
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
  Geis::Subscription subscription(geis_sub);

  return std::move(subscription);
}


} // namespace Ginn


