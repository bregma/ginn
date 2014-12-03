/**
 * @file ginn/wishsourceconfig.h
 * @brief Declarations of the Ginn Wish Source Confiuration interface.
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
#ifndef GINN_WISHSOURCECONFIG_H_
#define GINN_WISHSOURCECONFIG_H_

#include <string>
#include <vector>


namespace Ginn
{

/**
 * Consolidated configuration values.
 */
class WishSourceConfig
{
public:
  /**
   * Supported formats for wish sources.
   * @todo support additional wish file formats
   */
  enum class Format
  {
    XML,
  };

  /** A collection of named wish sources. */
  using SourceNameList = std::vector<std::string>;

public:
  virtual
  ~WishSourceConfig() = 0;

  /** Gets the wish file format to use. */
  virtual Format
  wish_source_format() const = 0;

  /** Gets a list of names of files containing wish definitions. */
  virtual SourceNameList const&
  wish_sources() const = 0;

  /** Special schema file name indicating 'do not validate'. */
  static const std::string WISH_NO_VALIDATE;

  /** Gets the name of the wish schema file. */
  virtual std::string const&
  wish_schema_file_name() const = 0;
};

} // namespace Ginn

#endif // GINN_WISHSOURCECONFIG_H_

