/**
 * @file ginn/action.h
 * @brief Interface to the Ginn Action module.
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
#ifndef GINN_ACTION_H_
#define GINN_ACTION_H_

#include <memory>
#include <string>
#include <vector>


namespace Ginn
{

class ActionBuilder;


/**
 * Encapsulates an action to be performed when a wish is fulfilled.
 *
 * Generally, an action is either a (mouse) button press or a keyboard key
 * press, with zero or more modifier keys pressed at the same time.  that
 * description can be a little misleading, since things like mouse scroll wheel
 * actions are often sent as mouse button presses.
 *
 * The difference between a button and a key is that a button has a cardinal
 * value (uh, that'd be a number, Bob) whereas a key usually has a name (called
 * a keysym in X11 parlance), which needs to get mapped to a cardinal (called a
 * keycode in X11).
 *
 * An Action consists of a sequence of one or more events.  For example, a
 * "control-alt-T" action consists of 3 key-down events followed by 2 key-up
 * events, for a total of 6 events.
 *
 * An Action needs to be constructed by an ActionBuilder and used by an
 * ActionSink.
 */
class Action
{
public:
  /** The types of action events that can be defined. */
  enum class EventType
  {
    key_press,
    key_release,
    button_press,
    button_release
  };

  /** An actual action event. */
  struct Event
  {
    EventType   type;         ///< the type of event
    uint8_t     code;         ///< event detail
  };

  /** A collection of action events that make up an actipon. */
  typedef std::vector<Event> EventList;

public:
  /** Constructs an Action with no data. */
  Action();

  /** Constructs an Action with data. */
  Action(ActionBuilder const& builder);

  /** Destroys an action. */
  ~Action();

  /** Gets an iterator initialized to the beginning of the action event list. */
  EventList::const_iterator
  begin() const;

  /** Gets an iterator initialized to one-past-the-end of the event list. */
  EventList::const_iterator
  end() const;

private:
  EventList events_;
};

} // namespace Ginn

#endif // GINN_ACTION_H_

