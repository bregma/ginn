/**
 * @file ginn/action.cpp
 * @brief Implementation of the Ginn Action module.
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
#include "ginn/action.h"


namespace Ginn
{

Action::
Action(ActionBuilder const& builder)
{
}


Action::
~Action()
{
}


Action::EventList::const_iterator Action::
begin() const
{
  return events_.cbegin();
}


Action::EventList::const_iterator Action::
end() const
{
  return events_.cend();
}

} // namespace Ginn

