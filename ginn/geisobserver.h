/**
 * @file ginn/geisobserver.h
 * @brief Declarations of the Ginn Geis Observer class.
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
#ifndef GINN_GEISOBSERVER_H_
#define GINN_GEISOBSERVER_H_

#include <geis/geis.h>


namespace Ginn
{

/**
 * A mixin interface for things that observe geis actions.
 */
class GeisObserver
{
public:
  virtual void
  geis_initialized() = 0;

  virtual void
  geis_new_class(GeisGestureClass gesture_class) = 0;
};

}

#endif // GINN_GEISOBSERVER_H_

