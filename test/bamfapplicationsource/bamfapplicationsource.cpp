/**
 * @file test/bamfapplicationsource/bamfapplicationsource.cpp
 * @brief Test driver for the BAMF application source.
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
#include "ginn/bamfapplicationsource.h"
#include "ginn/configuration.h"
#include "ginn/window.h"
#include <glib.h>
#include <iostream>
#include <memory>


typedef std::unique_ptr<GMainLoop, void(*)(GMainLoop*)>  main_loop_t;


void
window_opened(Ginn::Window const* window)
{
  std::cerr << __FUNCTION__ << ": " << *window << "\n";
}


void
window_closed(Ginn::Window const* window)
{
  std::cerr << __FUNCTION__ << ": " << *window << "\n";
}


int
main(int argc, char* argv[])
{
  Ginn::Configuration config(argc, argv);
  Ginn::BamfApplicationSource app_source(config);
  app_source.set_window_opened_callback(window_opened);
  app_source.set_window_closed_callback(window_closed);
  app_source.get_applications();

  main_loop_t main_loop(g_main_loop_new(NULL, FALSE), g_main_loop_unref);
  g_main_loop_run(main_loop.get());
}

