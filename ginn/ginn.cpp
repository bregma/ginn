/**
 * @file ginn/ginn.cpp
 * @brief Definitions of the Ginn module.
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
#include "config.h"
#include "ginn/ginn.h"

#include "ginn/actionsink.h"
#include "ginn/applicationsource.h"
#include "ginn/applicationobserver.h"
#include "ginn/configuration.h"
#include "ginn/geis.h"
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
: public ApplicationObserver
{
  Impl(Configuration const&  config,
       WishSource*           wish_source,
       ApplicationSource*    app_source,
       Keymap*               keymap,
       ActionSink*           action_sink);

  void
  load_raw_wishes();

  void
  load_applications();

  void
  application_added(Application::Ptr const& application);

  void
  application_removed(Application::Id const& application_id);

  void
  window_added(Window const& window);

  void
  window_removed(Window::Id window_id);

  void
  keymap_initialized();

  void
  action_sink_initialized();

  void
  geis_initialized();

  void
  geis_gesture_event(GeisEvent event);

  void
  create_watches();

  /** Indicates if all the asynch Ginn init has completed. */
  bool
  is_initialized() const
  {
    return keymap_is_initialized_
        && geis_is_initialized_
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
  ApplicationSource*                       app_source_;
  Application::List                        apps_;
  bool                                     keymap_is_initialized_;
  Keymap*                                  keymap_;
  bool                                     geis_is_initialized_;
  Geis                                     geis_;
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
     ActionSink*           action_sink)
: config_(config)
, wish_source_(wish_source)
, app_source_(app_source)
, keymap_is_initialized_(false)
, keymap_(keymap)
, geis_is_initialized_(false)
, geis_(config_)
, action_sink_is_initialized_(false)
, action_sink_(action_sink)
, main_loop_(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
{ 
  if (config_.wish_sources().empty())
    throw std::runtime_error("no wish sources found");

  g_unix_signal_add(SIGTERM, quit_cb, main_loop_.get());
  g_unix_signal_add(SIGINT,  quit_cb, main_loop_.get());

  g_idle_add(on_ginn_initialized, this);

  app_source_->set_observer(this);
  action_sink_->set_initialized_callback(std::bind(&Impl::action_sink_initialized,
                                                   this));
  keymap_->set_initialized_callback(std::bind(&Ginn::Impl::keymap_initialized,
                                              this));
  geis_.set_initialized_callback(std::bind(&Ginn::Impl::geis_initialized,
                                           this));
  geis_.set_event_callback(std::bind(&Ginn::Impl::geis_gesture_event,
                                      this,
                                      std::placeholders::_1));
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


/**
 * Loads the cuurent active applications from the confiured source.
 */
void Ginn::Impl::
load_applications()
{
  apps_ = std::move(app_source_->get_applications());
  if (config_.is_verbose_mode())
  {
    std::cout << apps_.size() << " applications recognized:\n";
    for (auto const& application: apps_)
      std::cout << *application.second << "\n";
  }
}


/**
 * Reacts to a new active application being added.
 * @param[in] application  The new application being added.
 */
void Ginn::Impl::
application_added(Application::Ptr const& application)
{
  apps_[application->application_id()] = application;
  if (config_.is_verbose_mode())
    std::cout << "application added: " << *application << "\n";
}


/**
 * Reacts to an active application being removed.
 * @param[in] application_id  Identifies the application being removed.
 */
void Ginn::Impl::
application_removed(Application::Id const& application_id)
{
  auto it = apps_.find(application_id);
  if (it != apps_.end())
  {
    if (config_.is_verbose_mode())
      std::cout << "removing application: " << it->second << "\n";;
    apps_.erase(it);
  }
}


/**
 * Reacts to an application window being added.
 * @param[in] window  The new application window being added.
 *
 * If the window's application is not known, the window is ignored and will
 * probably be added later when the new-application notification comes in.
 */
void Ginn::Impl::
window_added(Window const& window)
{
  auto it = apps_.find(window.application_id);
  if (it != apps_.end())
  {
    if (config_.is_verbose_mode())
      std::cout << "window added: " << window << "\n";
    it->second->add_window(window);
  }
}


/**
 * Reacts to an application window being removed.
 * @param[in]  window_id  Identifies the application window being removed.
 */
void Ginn::Impl::
window_removed(Window::Id window_id)
{
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
geis_initialized()
{
  geis_is_initialized_ = true;
  if (config_.is_verbose_mode())
    std::cout << "gesture recognizer is initialized\n";
}


/**
 * Reacts to a new gesture event coming from the Geis.
 * @param[in]  event  The Geis event being reported.
 */
void Ginn::Impl::
geis_gesture_event(GeisEvent event)
{
  for (auto const& watchlist: gesture_map_)
  {
    for (auto const& watch: watchlist.second)
    {
      if (watch->matches(event, action_sink_))
      {
        std::cerr << "gesture event handled for window " << watchlist.first << "\n";
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

  for (auto const& app: apps_)
  {
    for (auto const& wish: wish_table_)
    {
      if (app.first == wish.first)
      {
        for (auto const& window: app.second->windows())
        {
          for (auto const& w: wish.second)
          {
            GestureWatch::Ptr watch { new GestureWatch(window.window_id,
                                                       app.second,
                                                       w.second,
                                                       geis_) };
            gesture_map_[window.window_id].push_back(std::move(watch));
          }
        }
      }
    }
  }

  if (config_.is_verbose_mode())
  {
    for (auto const& watchlist: gesture_map_)
    {
      for (auto const& watch: watchlist.second)
      {
        std::cout << *watch << "\n";;
      }
    }
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
     ActionSink*           action_sink)
: impl_(new Impl(config, wish_source, app_source, keymap, action_sink))
{
  impl_->load_applications();
}


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

