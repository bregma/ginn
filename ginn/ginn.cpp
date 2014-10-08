/**
 * @file ginn/ginn.cpp
 * @brief Definitions of the Ginn module.
 */

/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include "config.h"
#include "ginn/ginn.h"

#include <algorithm>
#include <cassert>
#include "ginn/actionsink.h"
#include "ginn/applicationsource.h"
#include "ginn/configuration.h"
#include "ginn/gesturesource.h"
#include "ginn/gesturewatch.h"
#include "ginn/keymap.h"
#include "ginn/wish.h"
#include "ginn/wishsource.h"
#include <glib.h>
#include <glib-unix.h>
#include <iostream>
#include <stdexcept>
#include <utility>


/** C++ wrapper for GMainLoop */
typedef std::unique_ptr<GMainLoop, void(*)(GMainLoop*)>  main_loop_t;

/**
 * Signal handler for INT and TERM signals
 *
 * Shuts the daemon down gracefully.
 */
static gboolean
quit_cb(gpointer loop)
{
  if (loop)
    g_idle_add((GSourceFunc)g_main_loop_quit, loop);
  else
    exit(0);
  return FALSE;
}


namespace Ginn
{

/**
 * The actual implemntation of the Ginn object.
 *
 * This is hidden behind a compile-time firewall to (a) hide the nasty business
 * of using a glib main loop, which is forced on us by the use of GObject-based
 * BAMF client interface, and (2) to make development and maintenance a little
 * easier, in that internals don't leak and everything is kept as
 * self-contained as possible.
 */
struct Ginn::Impl
{
  Impl(Configuration const&  config,
       WishSource*           wish_source,
       ApplicationSource*    app_source,
       Keymap*               keymap,
       GestureSource*        gesture_source,
       ActionSink*           action_sink);

  void
  load_raw_wishes();

  void
  app_source_initialized();

  void
  window_opened(Window const* window);

  void
  window_closed(Window const* window);

  void
  keymap_initialized();

  void
  action_sink_initialized();

  void
  gesture_source_initialized();

  void
  gesture_event(GestureEvent const& event);

  void
  create_watches();

  /** Indicates if all the asynch Ginn init has completed. */
  bool
  is_initialized() const
  {
    return keymap_is_initialized_
        && app_source_is_initialized_
        && gesture_source_is_initialized
        && action_sink_is_initialized_;
  }

  void
  run()
  { g_main_loop_run(main_loop_.get()); }

  static gboolean
  on_ginn_initialized(gpointer data);

private:
  Configuration                            config_;
  WishSource*                              wish_source_;
  Wish::Table                              wish_table_;
  bool                                     app_source_is_initialized_;
  ApplicationSource*                       app_source_;
  bool                                     keymap_is_initialized_;
  Keymap*                                  keymap_;
  bool                                     gesture_source_is_initialized;
  GestureSource*                           gesture_source_;
  GestureWatch::Map                        gesture_map_;
  bool                                     action_sink_is_initialized_;
  ActionSink*                              action_sink_;
  main_loop_t                              main_loop_;
};


/**
 * GLib callback for polling Ginn initialization state from an idle callback.
 * @param[in] data A disguised pointer to the Ginn object.
 *
 * This callback function determines if the Ginn is completely ready for action,
 * and if it is, creates the initial watches and starts the action.
 *
 * @returns true if the Ginn object is fully initialzed (and the idle callback
 * should be removed, false otherwise (and the callback should be re-queued).
 */
gboolean Ginn::Impl::
on_ginn_initialized(gpointer data)
{
  Ginn::Impl* ginn = (Ginn::Impl*)data;
  if (ginn->is_initialized())
  {
    ginn->create_watches();
    return false;
  }
  return true;
}


/**
 * Constructs the internal Ginn implementation.
 */
Ginn::Impl::
Impl(Configuration const&  config,
     WishSource*           wish_source,
     ApplicationSource*    app_source,
     Keymap*               keymap,
     GestureSource*        gesture_source,
     ActionSink*           action_sink)
: config_(config)
, wish_source_(wish_source)
, app_source_is_initialized_(false)
, app_source_(app_source)
, keymap_is_initialized_(false)
, keymap_(keymap)
, gesture_source_is_initialized(false)
, gesture_source_(gesture_source)
, action_sink_is_initialized_(false)
, action_sink_(action_sink)
, main_loop_(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
{ 
  using std::bind;
  using std::placeholders::_1;

  if (config_.wish_sources().empty())
    throw std::runtime_error("no wish sources found");

  g_unix_signal_add(SIGTERM, quit_cb, main_loop_.get());
  g_unix_signal_add(SIGINT,  quit_cb, main_loop_.get());

  g_idle_add(on_ginn_initialized, this);

  app_source_->set_initialized_callback(bind(&Ginn::Impl::app_source_initialized, this));
  app_source_->set_window_opened_callback(bind(&Impl::window_opened, this, _1));
  app_source_->set_window_closed_callback(bind(&Impl::window_closed, this, _1));
  action_sink_->set_initialized_callback(bind(&Impl::action_sink_initialized, this));
  keymap_->set_initialized_callback(bind(&Ginn::Impl::keymap_initialized, this));
  gesture_source_->set_initialized_callback(bind(&Ginn::Impl::gesture_source_initialized, this));
  gesture_source_->set_event_callback(bind(&Ginn::Impl::gesture_event, this, _1));
}


/**
 * Loads the wishes from all the configured sources.
 */
void Ginn::Impl::
load_raw_wishes()
{
  WishSource::RawSourceList raw_sources = WishSource::read_raw_sources(config_.wish_sources());
  wish_table_ = wish_source_->get_wishes(raw_sources, keymap_);
  if (config_.is_verbose_mode())
    std::cout << wish_table_.size() << " raw wishes loaded\n";
}


void Ginn::Impl::
app_source_initialized()
{
  app_source_is_initialized_ = true;
  if (config_.is_verbose_mode())
    std::cout << "application source is initialized\n";
}


/**
 * Reacts to an application window being opened.
 * @param[in] window  The new application window being opened.
 *
 * If the window's application is not known, the window is ignored and will
 * probably be added later when the new-application notification comes in.
 */
void Ginn::Impl::
window_opened(Window const* window)
{
  if (!window)
    assert("logic error: NULL window");

  Application const* app = window->application_;
  if (!app)
    assert("logic error: NULL window application");

  auto wish_it = wish_table_.find(app->name());
  if (wish_it != std::end(wish_table_))
  {
    for (auto const& w: wish_it->second)
    {
#if 0
      GestureWatch::Ptr watch {
        new GestureWatch(window->id_,
                         app.second,
                         w.second,
                         gesture_source_->subscribe(window->id_,
                                                    w.second))
      };
      gesture_map_[window->id_].push_back(std::move(watch));
#else
      if (config_.is_verbose_mode())
        std::cout << __FUNCTION__ << " adding wish for '" << window->application_->name() << "'\n";
#endif
    }
  }
}


/**
 * Reacts to an application window being closed.
 * @param[in]  window  The application window being closed.
 */
void Ginn::Impl::
window_closed(Window const*)
{
#if 0
  for (auto const& it: apps_)
  {
    Window const* window = it.second->window(window_id);
    if (window)
    {
      if (config_.is_verbose_mode())
        std::cout << "window removed: " << *window << "\n";;
      it.second->remove_window(window_id);
      break;
    }
  }
#endif
}


/**
 * Reacts to the Geis being initialized.
 *
 * The action sink can be initialized asynchronously.  No Ginn is completely
 * initialized without a fully initialized action sink.
 */
void Ginn::Impl::
action_sink_initialized()
{
  action_sink_is_initialized_ = true;
  if (config_.is_verbose_mode())
    std::cout << "action sink is initialized\n";
}


/**
 * Reacts to the keymap being initialized.
 *
 * The keymap can be initialized asynchronously, and it needs to be fully
 * initialized before the wishes get loaded because the wish loader needs the
 * keymap to map the keysyms found in a wish definition to the keycodes used in
 * the action sink.
 */
void Ginn::Impl::
keymap_initialized()
{
  keymap_is_initialized_ = true;
  if (config_.is_verbose_mode())
    std::cout << "keymap is initialized\n";
  load_raw_wishes();
}


/**
 * Reacts to the Geis being initialized.
 *
 * The Geis can be initialized asynchronously.  No Ginn is completely
 * initialized without a fully initialized Geis.
 */
void Ginn::Impl::
gesture_source_initialized()
{
  gesture_source_is_initialized = true;
  if (config_.is_verbose_mode())
    std::cout << "gesture recognizer is initialized\n";
}


/**
 * Reacts to a new gesture event coming from the gesture source.
 * @param[in]  event  The gesture event being reported.
 */
void Ginn::Impl::
gesture_event(GestureEvent const& event)
{
  for (auto const& watchlist: gesture_map_)
  {
    for (auto const& watch: watchlist.second)
    {
      if (event.matches(*watch))
      {
        action_sink_->perform(watch->wish()->action());
        std::cerr << "gesture event handled for window " << watch->window_id() << "\n";
        break;
      }
    }
  }
}


/**
 * Creates the initial gesture watches.
 *
 * Requires a fully-initialized Ginn (wishes loaded, Geis initialized, action
 * sink ready).
 */
void Ginn::Impl::
create_watches()
{
  if (config_.is_verbose_mode())
    std::cout << "creating watches...\n";

  if (config_.is_verbose_mode())
  {
    for (auto const& watchlist: gesture_map_)
    {
      for (auto const& watch: watchlist.second)
      {
        std::cout << *watch << "\n";;
      }
    }
    std::cout << "Ginn initialization complete\n";
  }
}


/**
 * Constructs the Ginn by creating its internal implementation, hooking it up
 * to signal handlers, then loading the data.
 */
Ginn::
Ginn(Configuration const&  config,
     WishSource*           wish_source,
     ApplicationSource*    app_source,
     Keymap*               keymap,
     GestureSource*        gesture_source,
     ActionSink*           action_sink)
: impl_(new Impl(config, wish_source, app_source, keymap, gesture_source, action_sink))
{ }


/**
 * Tears down the Ginn.
 */
Ginn::
~Ginn()
{ }


/**
 * Runs the main event loop until a signal to shut down is received.
 */
void Ginn::
run()
{ impl_->run(); }


} // namespace Ginn

