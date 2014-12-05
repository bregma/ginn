/**
 * @file ginn/main.cpp
 * @brief Ginn program entry point.
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

#include "ginn/bamfapplicationsource.h"
#include "ginn/configuration.h"
#include "ginn/geisgesturesource.h"
#include "ginn/ginn.h"
#include "ginn/wishsource.h"
#include "ginn/x11actionsink.h"
#include "ginn/x11keymap.h"
#include <iostream>
#include <stdexcept>
#include <utility>


int
main(int argc, char* argv[])
{
  using namespace Ginn;
  using namespace std;

  try
  {
    Configuration config(argc, argv);
    if (config.is_verbose_mode())
      cout << __FUNCTION__ << ": creating components\n";

    WishSource::Ptr wish_source = WishSource::factory(&config);
    BamfApplicationSource app_source(config);
    GeisGestureSource gesture_source(config);
    X11Keymap x11_keymap(config);
    X11ActionSink action_sink(config);

    if (config.is_verbose_mode())
      cout << __FUNCTION__ << ": creating Ginn\n";
    Ginn::Ginn ginn(config,
                    wish_source.get(),
                    &app_source,
                    &x11_keymap,
                    &gesture_source,
                    &action_sink);

    if (config.is_verbose_mode())
      cout << __FUNCTION__ << ": starting main loop\n";
    ginn.run();

    if (config.is_verbose_mode())
      cout << __FUNCTION__ << ": shutting down cleanly\n";
  }
  catch (exception& ex)
  {
    cerr << "exception caught: " << ex.what() << "\n";
    return 1;
  }
  catch (...)
  {
    cerr << "unknown exception caught\n";
    return 1;
  }
  return 0;
}
