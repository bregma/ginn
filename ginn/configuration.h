/**
 * @file ginn/configuration.h
 * @brief Declarations of the Ginn Confiuration module.
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
#ifndef GINN_CONFIGURATION_H_
#define GINN_CONFIGURATION_H_

#include <memory>
#include <string>
#include <vector>


namespace Ginn
{

/** A collection of file names. */
typedef std::vector<std::string> FileNameList;

/** Types of wish file formats supported. @todo support more than one format. */
enum class WishFileFormat
{
  XML,
};


/**
 * Consolidated configuration values.
 */
class Configuration
{
public:
  Configuration(int argc, char* argv[]);

  ~Configuration();

  /** Indicates the program should be verbose about what its doing.  */
  bool
  is_verbose_mode() const;

  /** Gets the wish file format to use. */
  WishFileFormat
  wish_file_format() const;

  /** Gets a list of names of files containing wish definitions. */
  FileNameList const&
  wish_file_names() const;

  /** Special schema file name indicating 'do not validate'. */
  static const std::string WISH_NO_VALIDATE;

  /** Gets the name of the wish schema file (empty if no validation) */
  std::string const&
  wish_schema_file_name() const;

private:
  struct Impl;

  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_CONFIGURATION_H_
