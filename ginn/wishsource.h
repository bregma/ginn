/**
 * @file ginn/wishsource.h
 * @brief Declarations of the Ginn Wish Source module interface.
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
#ifndef GINN_WISHSOURCE_H_
#define GINN_WISHSOURCE_H_

#include "ginn/configuration.h"
#include "ginn/wish.h"
#include <memory>
#include <string>


namespace Ginn
{

/**
 * An interface for Wish sources.
 *
 * Comes complete with a handy factory function to create Wish souces by name.
 */
class WishSource
{
public:
  typedef std::shared_ptr<WishSource> Ptr;

public:
  virtual ~WishSource() = 0;

  /** Creates a concrete WishSource. */
  static Ptr
  wish_source_factory(WishFileFormat format, std::string const& schema_file_name);

  /** Gest wishes from the source. */
  virtual Wish::Table
  get_wishes(FileNameList const& wish_file_names) = 0;
};

}

#endif // GINN_WISHSOURCE_H_

