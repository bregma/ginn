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

#include "ginn/configuration.h"
#include "ginn/ginn.h"
#include <iostream>
#include <stdexcept>


int
main(int argc, char* argv[])
{
  try
  {
    Ginn::Configuration configuration(argc, argv);
    Ginn::Ginn ginn(configuration);
    ginn.run();
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
