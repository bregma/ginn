/**
 * @file ginn/wishsource.h
 * @brief Declarations of the Ginn Wish Source module interface.
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
#ifndef GINN_WISHSOURCE_H_
#define GINN_WISHSOURCE_H_

#include "ginn/wish.h"
#include "ginn/wishsourceconfig.h"
#include <memory>
#include <string>
#include <vector>


namespace Ginn
{
class Configuration;
class Keymap;

/**
 * An interface for Wish sources.
 *
 * Comes complete with a handy factory function to create Wish souces by name.
 */
class WishSource
{
public:
  using Ptr = std::unique_ptr<WishSource>;

  /** A memory image of a raw wish source. */
  struct RawSource
  {
    std::string name;    ///< name of the source
    std::string source;  ///< the raw source itself
  };

  /** A collection of raw sources */
  using RawSourceList = std::vector<RawSource>;

public:
  virtual ~WishSource() = 0;

  /** Creates a concrete WishSource. */
  static Ptr
  factory(WishSourceConfig::Format format, Configuration const& configration);

  /** Reads the raw wishes into a buffer. */
  static RawSourceList
  read_raw_sources(WishSourceConfig::SourceNameList const& wish_file_names);

  /** Gets wishes from the source. */
  virtual Wish::Table
  get_wishes(RawSourceList const& raw_wishes, Keymap* keymap) = 0;
};

}

#endif // GINN_WISHSOURCE_H_

