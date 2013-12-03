/**
 * @file ginn/x11actionsink.cpp
 * @brief Implementation of the Ginn X11 ActionSink module.
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
#include "ginn/x11actionsink.h"

#include "ginn/action.h"
#include <iostream>


namespace Ginn
{

X11ActionSink::
X11ActionSink(InitializedCallback initialized_callback)
{
  initialized_callback();
}


X11ActionSink::
~X11ActionSink()
{
}


void X11ActionSink::
perform(Action const& action)
{
  std::cerr << action << "\n";
}


} // namespace Ginn

