/**
 * @file ginn/gesturewatch.h
 * @brief Declarations of the Ginn GestureWatch class.
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
#ifndef GINN_GESTUREWATCH_H_
#define GINN_GESTUREWATCH_H_

#include "ginn/actionsink.h"
#include "ginn/application.h"
#include "ginn/gesturesource.h"
#include "ginn/wish.h"
#include <iosfwd>
#include <map>


namespace Ginn
{

/**
 * Something that joins wishes and application windows through a gesure
 * subscription.
 */
class GestureWatch
{
public:
  /** A pointer to a watch. */
  typedef std::unique_ptr<GestureWatch> Ptr;
  /** A list of gesture watches. */
  typedef std::vector<GestureWatch::Ptr> List;
  /** Collection mapping a windowid to gesture watches. */
  typedef std::map<Window::Id, GestureWatch::List> Map;

public:
  GestureWatch(Window::Id               window_id,
               Application::Ptr         app,
               Wish::Ptr                wish,
               GestureSubscription::Ptr subscription);

  ~GestureWatch();

  Window::Id
  window_id() const
  { return window_id_; }

  Wish::Ptr
  wish() const
  { return wish_; }

  friend std::ostream&
  operator<<(std::ostream& ostr, GestureWatch const& watch);

private:
  Window::Id               window_id_;
  Application::Ptr         application_;
  Wish::Ptr                wish_;
  GestureSubscription::Ptr subscription_;
};


} // namespace Ginn

#endif // GINN_GESTUREWATCH_H_

