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

#include "ginn/configuration.h"
#include <glib.h>
#include <glib-unix.h>
#include <iostream>
#include <stdexcept>


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
  if (config.wish_schema_file_name().empty())
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

struct Ginn::Impl
{
  Impl(int argc, char* argv[]);

  Configuration  config_;
  main_loop_t    main_loop_;
};


Ginn::Impl::
Impl(int argc, char* argv[])
: config_(argc, argv)
, main_loop_(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
{ 
}


Ginn::
Ginn(int argc, char* argv[])
: impl_(new Impl(argc, argv))
{
  if (impl_->config_.is_verbose_mode())
    dump_configuration(impl_->config_);

  if (impl_->config_.wish_file_names().empty())
    throw std::runtime_error("no wish files found in configuration");

  g_unix_signal_add(SIGTERM, quit_cb, impl_->main_loop_.get());
  g_unix_signal_add(SIGINT, quit_cb, impl_->main_loop_.get());
}


Ginn::
~Ginn()
{ }


void Ginn::
run()
{ g_main_loop_run(impl_->main_loop_.get()); }


} // namespace Ginn

