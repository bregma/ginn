/**
 * @file ginn/wish.h
 * @brief Declarations of the Ginn Wish class.
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
#ifndef GINN_WISH_H_
#define GINN_WISH_H_

#include "ginn/action.h"
#include <map>
#include <memory>
#include <string>


namespace Ginn
{
class WishBuilder;


/**
 * A description of a mapping between a multi-touch gesture and an action.
 *
 * A Wish has two parts:  a description of the the gesture and a description
 * of the action to take when the gesture occurs.  generally, the action
 * consists of sending a keyboard or mouse click event back through the X
 * server.
 *
 * Wishes are grouped into collections and the collections associated with
 * applications.
 *
 * @todo Refine the internals of this class to maybe hide stuff better.
 *
 * @todo Break the Table key into a separate class so apps can be matched by
 * name, generic_name, or desktop_name.
 *
 * @todo Redesign wishes to allow for more complex (continuation) gestures,
 * like tap-and-hold, with timeouts etc.
 */
class Wish
{
public:
  /** A (shared) pointer to a Wish. */
  using Ptr = std::shared_ptr<Wish>;
  /** A collection of wishes. */
  using List = std::map<std::string, Ptr>;
  /** A collection of wishes grouped by application name. */
  using Table = std::map<std::string, Wish::List>;

public:
  Wish(const WishBuilder& builder);

  /** Gets the name of the wish. this should be unique within a Wish::List. */
  std::string const&
  name() const
  { return name_; }

  std::string const&
  gesture() const
  { return gesture_; }

  int
  touches() const
  { return touches_; }

  std::string const&
  property() const
  { return property_; }

  float
  min() const
  { return min_; }

  float
  max() const
  { return max_; }

  Action const&
  action() const
  { return action_; }

private:
  std::string name_;
  std::string gesture_;
  int         touches_;
  std::string when_;
  std::string property_;
  float       min_;
  float       max_;
  Action      action_;
};

std::ostream&
operator<<(std::ostream& ostr, Wish const& wish);

}

#endif // GINN_WISH_H_

