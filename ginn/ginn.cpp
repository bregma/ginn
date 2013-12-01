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

#include "ginn/applicationsource.h"
#include "ginn/applicationobserver.h"
#include "ginn/configuration.h"
#include "ginn/geis.h"
#include "ginn/geisobserver.h"
#include "ginn/gesturewatch.h"
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


/**
 * Dumps the current confiuration values.
 */
static void
dump_configuration(Ginn::Configuration const& config)
{
  std::cout << PACKAGE_STRING << "\n";
  std::cout << "Copyright 2013 Canonical Ltd.\n\n";

  std::cout << "Loading wishes from the following files:\n";
  for (auto const& wish_file_name: config.wish_sources())
  {
    std::cout << "    " << wish_file_name << "\n";
  }
  if (config.wish_schema_file_name() == Ginn::Configuration::WISH_NO_VALIDATE)
  {
    std::cout << "wish validation is disabled.\n";
  }
  else
  {
    std::cout << "wish validation schema: " << config.wish_schema_file_name()
              << "\n";
  }
}


namespace Ginn
{

/**
 * The actual implemntation of the Ginn object.
 *
 * This is hidden behind a compile-time firewall to (a) hide the nasty business
 * of using a glib main loop, which is forced on us by the use of GObject-based
 * BAMF client interface, and (2) to make development and maintenance a little
 * easier, in that internals don;t leaks and everything is kept as
 * self-contained as possible.
 */
struct Ginn::Impl
: public ApplicationObserver
, public GeisObserver
{
  Impl(int argc, char* argv[]);

  void
  load_wishes();

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
  geis_initialized();

  void
  geis_new_class(GeisGestureClass gesture_class);

  void
  geis_gesture_event(GeisEvent event);

  void
  run()
  { g_main_loop_run(main_loop_.get()); }

private:
  Configuration                            config_;
  main_loop_t                              main_loop_;
  WishSource::Ptr                          wish_source_;
  Wish::Table                              wish_table_;
  ApplicationSource::Ptr                   app_source_;
  Application::List                        apps_;
  Geis                                     geis_;
  std::map<std::string, GeisGestureClass>  class_map_;
  GestureWatch::Map                        gesture_map_;
};


/**
 * Constructs the internal Ginn implementation.
 */
Ginn::Impl::
Impl(int argc, char* argv[])
: config_(argc, argv)
, main_loop_(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
, wish_source_(WishSource::wish_source_factory(config_.wish_file_format(),
                                               config_.wish_schema_file_name()))
, app_source_(ApplicationSource::application_source_factory("bamf", this))
, geis_(this)
{ 
  if (config_.is_verbose_mode())
    dump_configuration(config_);

  if (config_.wish_sources().empty())
    throw std::runtime_error("no wish sources found");

  g_unix_signal_add(SIGTERM, quit_cb, main_loop_.get());
  g_unix_signal_add(SIGINT,  quit_cb, main_loop_.get());
}


/**
 * Loads the wishes from all the configured sources.
 */
void Ginn::Impl::
load_wishes()
{
  wish_table_ = std::move(wish_source_->get_wishes(config_.wish_sources()));
  if (config_.is_verbose_mode())
    std::cout << wish_table_.size() << " wishes loaded\n";
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


void Ginn::Impl::
geis_initialized()
{
  // Create the initial gesture watches.
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


void Ginn::Impl::
geis_new_class(GeisGestureClass gesture_class)
{
  char const* class_name = geis_gesture_class_name(gesture_class);
  class_map_[class_name] = gesture_class;
}


void Ginn::Impl::
geis_gesture_event(GeisEvent event)
{
  for (auto const& watchlist: gesture_map_)
  {
    for (auto const& watch: watchlist.second)
    {
      if (watch->matches(event))
      {
        std::cerr << "gesture event for window " << watchlist.first << "\n";
      }
    }
  }
}


/**
 * Constructs the Ginn by creating its internal implementation, hooking it up
 * to signal handlers, then loading the data.
 */
Ginn::
Ginn(int argc, char* argv[])
: impl_(new Impl(argc, argv))
{
  impl_->load_wishes();
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

