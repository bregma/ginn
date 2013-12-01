/**
 * @file ginn/actionsink.cpp
 * @brief Implementation of the Ginn ActionSink interface.
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
#include "ginn/actionsink.h"

#include "ginn/x11actionsink.h"


namespace Ginn
{

ActionSink::
~ActionSink()
{
}


ActionSink::Ptr ActionSink::
action_sink_factory(Type type)
{
  Ptr sink;
  if (type == Type::X11)
    sink.reset(new X11ActionSink);
  return sink;
}

} // namespace Ginn


