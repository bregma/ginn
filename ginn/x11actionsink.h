/**
 * @file ginn/x11actionsink.h
 * @brief Interface to the Ginn X11 ActionSink module.
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
#ifndef GINN_X11ACTIONSINK_H_
#define GINN_X11ACTIONSINK_H_

#include "ginn/actionsink.h"
#include <memory>


namespace Ginn
{
class Configuration;

/**
 * A concrete action sink that injects events into the X11 server for an
 * effected wish.
 */
class X11ActionSink
: public ActionSink
{
public:

  /** Internal implementation of this class. */
  struct Impl;

public:
  X11ActionSink(Configuration const& config);

  ~X11ActionSink();

  void
  set_initialized_callback(InitializedCallback const& callback);

  void
  perform(Action const& action);

private:
  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_X11ACTIONSINK_H_

