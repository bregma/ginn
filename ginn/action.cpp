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

#include "ginn/actionbuilder.h"
#include <iostream>
#include <map>
#include <string>
#include <utility>


namespace Ginn
{

Action::
Action()
{ }


Action::
Action(ActionBuilder const& builder)
: events_(std::move(builder.events()))
{ }


Action::
~Action()
{ }


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


std::ostream&
operator<<(std::ostream& ostr, Action const& action)
{
  for (auto const& event: action)
  {
    ostr << "(" << event << ") ";
  }
  return ostr;
}


std::ostream&
operator<<(std::ostream& ostr, Action::EventType const& event_type)
{
  static const std::map<Action::EventType, std::string> event_type_names {
    { Action::EventType::key_press,      "key press"      },
    { Action::EventType::key_release,    "key release"    },
    { Action::EventType::button_press,   "button press"   },
    { Action::EventType::button_release, "button release" }
  };
  return ostr << event_type_names.at(event_type);
}


std::ostream&
operator<<(std::ostream& ostr, Action::Event const& event)
{
  return ostr << event.type << " " << (static_cast<int>(event.code) & 0xff);
}

} // namespace Ginn

