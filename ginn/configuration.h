/**
 * @file ginn/configuration.h
 * @brief Declarations of the Ginn Confiuration module.
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
#ifndef GINN_CONFIGURATION_H_
#define GINN_CONFIGURATION_H_

#include "ginn/applicationsource.h"
#include "ginn/wishsourceconfig.h"
#include <memory>
#include <string>


namespace Ginn
{

/**
 * Consolidated configuration values.
 */
class Configuration
: public WishSourceConfig
{
public:
  Configuration(int argc, char* argv[]);

  ~Configuration();

  /** Indicates the program should be verbose about what its doing.  */
  bool
  is_verbose_mode() const;

  /** Gets the wish file format to use. */
  Format
  wish_source_format() const override;

  /** Gets a list of names of files containing wish definitions. */
  SourceNameList const&
  wish_sources() const override;

  /** Gets the name of the wish schema file. */
  std::string const&
  wish_schema_file_name() const override;

private:
  struct Impl;

  std::shared_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_CONFIGURATION_H_

