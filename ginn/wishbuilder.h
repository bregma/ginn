/**
 * @file ginn/wishbuilder.h
 * @brief Declarations of the Ginn WishBuilder interface.
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
#ifndef GINN_WISHBUILDER_H_
#define GINN_WISHBUILDER_H_

#include <string>


namespace Ginn
{

  /**
   * Interface for building an Wish object.
   */
  class WishBuilder
  {
  public:
    virtual ~WishBuilder() = 0;

    virtual std::string
    name() const = 0;

    virtual std::string
    gesture() const = 0;

    virtual int
    touches() const = 0;

    virtual std::string
    when() const = 0;

    virtual std::string
    property() const = 0;

    virtual float
    min() const = 0;

    virtual float
    max() const = 0;

    virtual std::string
    action() const = 0;

    virtual std::string
    modifier1() const = 0;

    virtual std::string
    modifier2() const = 0;

    virtual std::string
    modifier3() const = 0;
  };
}

#endif // GINN_WISHBUILDER_H_

