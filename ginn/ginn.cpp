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

#include "ginn/applicationloader.h"
#include "ginn/applicationobserver.h"
#include "ginn/configuration.h"
#include "ginn/geis.h"
#include "ginn/geisobserver.h"
#include "ginn/gesturewatch.h"
#include "ginn/wish.h"
#include "ginn/wishloader.h"
#include <glib.h>
#include <glib-unix.h>
#include <iomanip>
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
  for (auto const& wish_file_name: config.wish_file_names())
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


static void
dump_window(Ginn::Window const& window)
{
  std::cout << "window_id="
            << std::hex << std::setw(8) << std::setfill('0') << std::showbase
            << window.window_id
            << std::dec
            << " application_id=\"" << window.application_id << "\""
            << " monitor=" << window.monitor
            << " title=\"" << window.title << "\"\n";
}


static void
dump_application(Ginn::Application::Ptr const& application)
{
  std::cout << "application_id=\"" << application->application_id() << "\""
            << " name=\"" << application->name() << "\""
            << " generic_name=\"" << application->generic_name() << "\""
            << "\n";
  for (auto const& window: application->windows())
  {
    std::cout << "    ";
    dump_window(window);
  }
}


static void
dump_applications(Ginn::Application::List const& apps)
{
  for (auto const& application: apps)
  {
    dump_application(application.second);
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
  run()
  { g_main_loop_run(main_loop_.get()); }

private:
  Configuration                            config_;
  main_loop_t                              main_loop_;
  WishLoader::Ptr                          wish_loader_;
  Wish::Table                              wish_table_;
  ApplicationLoader::Ptr                   app_loader_;
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
, wish_loader_(WishLoader::wish_loader_factory(config_.wish_file_format(),
                                               config_.wish_schema_file_name()))
, app_loader_(ApplicationLoader::application_loader_factory("bamf", this))
, geis_(this)
{ 
  if (config_.is_verbose_mode())
    dump_configuration(config_);

  if (config_.wish_file_names().empty())
    throw std::runtime_error("no wish files found in configuration");

  g_unix_signal_add(SIGTERM, quit_cb, main_loop_.get());
  g_unix_signal_add(SIGINT,  quit_cb, main_loop_.get());
}


/**
 * Loads the wishes from all the configured sources.
 */
void Ginn::Impl::
load_wishes()
{
  wish_table_ = std::move(wish_loader_->get_wishes(config_.wish_file_names()));
  if (config_.is_verbose_mode())
    std::cout << wish_table_.size() << " wishes loaded\n";
}


/**
 * Loads the cuurent active applications from the confiured source.
 */
void Ginn::Impl::
load_applications()
{
  apps_ = std::move(app_loader_->get_applications());
  if (config_.is_verbose_mode())
  {
    std::cout << apps_.size() << " applications recognized:\n";
    dump_applications(apps_);
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
  {
    std::cout << "application added: ";
    dump_application(application);
  }
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
    {
      std::cout << "removing application: ";
      dump_application(it->second);
    }
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
    {
      std::cout << "window added: ";
      dump_window(window);
    }
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
      {
        std::cout << "window removed: ";
        dump_window(*window);
      }
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
            gesture_map_[window.window_id] = std::move(
                GestureWatch::Ptr(new GestureWatch(window.window_id,
                                                   app.second,
                                                   w.second,
                                                   geis_)));
          }
        }
      }
    }
  }

  for (auto const& watch: gesture_map_)
  {
    watch.second->dump();
  }
}


void Ginn::Impl::
geis_new_class(GeisGestureClass gesture_class)
{
  char const* class_name = geis_gesture_class_name(gesture_class);
  class_map_[class_name] = gesture_class;
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

