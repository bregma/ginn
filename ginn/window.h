/**
 * @file ginn/window.h
 * @brief Declarations of the Ginn Application Window class.
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
#ifndef GINN_WINDOW_H_
#define GINN_WINDOW_H_

#include <string>
#include <vector>


namespace Ginn
{
class Application;

/**
 * An application window.
 */
struct Window
{
  /** A unique identifier for an application window. */
  typedef unsigned long       Id;
  /** A collection of windows */
  typedef std::vector<Window> List;

  Id            id_;
  std::string   title_;
  Application*  application_;
  bool          is_active_;
  bool          is_visible_;
  int           monitor_;
};

std::ostream&
operator<<(std::ostream& ostr, Window const& window);


} // namespace Ginn

#endif // GINN_WINDOW_H_

