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

#include <iostream>
#include <stdexcept>


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


int
main(int argc, char* argv[])
{
  try
  {
    Ginn::Configuration configuration(argc, argv);
    if (configuration.is_verbose_mode())
    {
      dump_configuration(configuration);
    }

    if (configuration.wish_file_names().empty())
    {
      std::cerr << "no wish files found in configuration, exiting...\n";
      return 2;
    }
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
