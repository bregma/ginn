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

#include "ginn/x11actionsink.h"
#include "ginn/applicationsource.h"
#include "ginn/configuration.h"
#include "ginn/ginn.h"
#include "ginn/wishsource.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>


int
main(int argc, char* argv[])
{
  using namespace Ginn;

  try
  {
    Configuration config(argc, argv);
    if (config.is_verbose_mode())
      std::cout << __FUNCTION__ << ": creating components\n";

    WishSource::Ptr wish_source = WishSource::factory(config.wish_source_format(),
                                                      config);
    ApplicationSource::Ptr app_source 
          = ApplicationSource::factory(config.application_source_type(), config);
    std::unique_ptr<ActionSink> action_sink(new X11ActionSink(config));

    if (config.is_verbose_mode())
      std::cout << __FUNCTION__ << ": creating Ginn\n";
    Ginn::Ginn ginn(config,
                    std::move(wish_source),
                    std::move(app_source),
                    action_sink.get());

    if (config.is_verbose_mode())
      std::cout << __FUNCTION__ << ": starting main loop\n";
    ginn.run();

    if (config.is_verbose_mode())
      std::cout << __FUNCTION__ << ": shutting down cleanly\n";
  }
  catch (std::exception& ex)
  {
    std::cerr << "exception caught: " << ex.what() << "\n";
    return 1;
  }
  catch (...)
  {
    std::cerr << "unknown exception caught\n";
    return 1;
  }
  return 0;
}
