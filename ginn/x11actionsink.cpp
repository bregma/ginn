/**
 * @file ginn/x11actionsink.cpp
 * @brief Implementation of the Ginn X11 ActionSink module.
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
#include "ginn/x11actionsink.h"

#include "ginn/action.h"
#include "ginn/configuration.h"
#include <glib.h>
#include <iostream>
#include <map>
#include <queue>
#include <xcb/xtest.h>
#include <xcb/xcb.h>


namespace Ginn
{

using Callback = std::function<void()>;
using CallbackQueue = std::queue<Callback>;

struct X11ActionSink::Impl
{
public:
  Impl(Configuration const& config)
  : config_(config)
  , connection_(xcb_connect(NULL, NULL))
  {
    if (!connection_)
    {
      throw std::runtime_error("connecting to X server");
    }
    int fd = xcb_get_file_descriptor(connection_);
    iochannel_ = g_io_channel_unix_new(fd);
    g_io_add_watch(iochannel_,
                   GIOCondition(G_IO_IN | G_IO_ERR | G_IO_HUP),
                   xcb_gio_event_ready,
                   this);

    xcb_test_get_version_cookie_t cookie = xcb_test_get_version(connection_, 2, 1);
    callback_queue_.push(std::bind(&Impl::verify_version, this, cookie));
    xcb_flush(connection_);
  }

  ~Impl()
  {
    g_io_channel_shutdown(iochannel_, FALSE, NULL);
    g_io_channel_unref(iochannel_);
    xcb_disconnect(connection_);
  }


  static gboolean
  xcb_gio_event_ready(GIOChannel* channel, GIOCondition cond, gpointer pdata)
  {
    X11ActionSink::Impl* impl = (X11ActionSink::Impl*)pdata;
    if (cond == G_IO_HUP)
    {
      g_io_channel_unref(channel);
      if (impl->initialized_callback_)
        impl->initialized_callback_();
      return FALSE;
    }

    if (!impl->callback_queue_.empty())
    {
      impl->callback_queue_.front()();
      impl->callback_queue_.pop();
    }
    return TRUE;
  }


  void
  verify_version(xcb_test_get_version_cookie_t cookie)
  {
    xcb_generic_error_t* e = NULL;
    xcb_test_get_version_reply_t* reply = xcb_test_get_version_reply(connection_,
                                                                     cookie, &e);
    if (reply)
    {
      std::cout << "XTest version "
                << (int)reply->major_version << "." << (int)reply->minor_version
                << "\n";
      free(reply);
      if (initialized_callback_)
        initialized_callback_();
    }
    if (e)
    {
      std::cerr << "XTest version error: " << e->error_code << "\n";
      free(e);
    }
  }

  void
  check_fake_input(xcb_void_cookie_t)
  {
#if 0
    xcb_generic_error_t* e = NULL;
    xcb_test_fake_input_reply_t* reply = xcb_test_fake_input_reply(connection_,
                                                                   cookie, &e);
    if (reply)
    {
      free(reply);
    }
    if (e)
    {
      std::cerr << "XTest version error: " << e->error_code << "\n";
      free(e);
    }
#else
    std::cerr << "==smw> " << __PRETTY_FUNCTION__ << "\n";
#endif
  }

  Configuration       config_;
  xcb_connection_t*   connection_;
  GIOChannel*         iochannel_;
  InitializedCallback initialized_callback_;
  CallbackQueue       callback_queue_;
};


X11ActionSink::
X11ActionSink(Configuration const& config)
: impl_(new Impl(config))
{
  if (impl_->config_.is_verbose_mode())
    std::cerr << __FUNCTION__ << " created\n";
}


X11ActionSink::
~X11ActionSink()
{
}


void X11ActionSink::
set_initialized_callback(InitializedCallback const& callback)
{
  impl_->initialized_callback_ = callback;
}


void X11ActionSink::
perform(Action const& action)
{
  if (impl_->config_.is_verbose_mode())
    std::cout << __PRETTY_FUNCTION__ << " " << action << "\n";
  static const xcb_window_t none = { XCB_NONE };
  static const std::map<Action::EventType, uint8_t> type_map = {
    { Action::EventType::key_press,      XCB_KEY_PRESS      },
    { Action::EventType::key_release,    XCB_KEY_RELEASE    },
    { Action::EventType::button_press,   XCB_BUTTON_PRESS   },
    { Action::EventType::button_release, XCB_BUTTON_RELEASE }
  };

  for (auto const& event: action)
  {
    xcb_void_cookie_t cookie = xcb_test_fake_input(impl_->connection_,
                                                   type_map.at(event.type),
                                                   event.code,
                                                   0,
                                                   none,
                                                   0, 0, 0);
    impl_->callback_queue_.push(std::bind(&Impl::check_fake_input,
                                          impl_.get(),
                                          cookie));
  }
}


} // namespace Ginn

