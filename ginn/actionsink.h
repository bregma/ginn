/**
 * @file ginn/actionsink.h
 * @brief Interface to the Ginn ActionSink interface.
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
#ifndef GINN_ACTIONSINK_H_
#define GINN_ACTIONSINK_H_

#include <functional>


namespace Ginn
{
class Action;


/**
 * A place where actions end up getting sent.
 *
 * This is an interface to the place where actions get performed when a gesture
 * is effected over a window according to a wish.
 */
class ActionSink
{
public:
  /** Signal for when the action sink has completed its initialization. */
  using InitializedCallback = std::function<void()>;

public:
  virtual ~ActionSink() = 0;

  virtual void
  set_initialized_callback(InitializedCallback const& callback) = 0;

  virtual void
  perform(Action const& action) = 0;
};

} // namespace Ginn

#endif // GINN_ACTIONSINK_H_

