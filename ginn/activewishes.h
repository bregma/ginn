/**
 * @file ginn/activewishes.h
 * @brief Declarations of the Ginn ActiveWishes module.
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
#ifndef GINN_ACTIVEWISHES_H_
#define GINN_ACTIVEWISHES_H_

#include <functional>
#include "ginn/wish.h"
#include <memory>


namespace Ginn
{
  class ActionSink;
  class Configuration;
  class GestureEvent;
  class GestureSource;
  class Window;
  class WishSource;

/**
 * The ActiveWishes class encapsulates the active wishes currently being juggled
 * by the Ginn.
 *
 * An active wish is an application window that matches a rule defined in a
 * wish.
 *
 * The active wishes collection is a dynamic entity, because application windows
 * may come and go as they please.
 */
class ActiveWishes
{
public:
  using Callback = std::function<void(Wish const&, Window const&)>;

  struct Impl;

public:
  ActiveWishes(Configuration const& config, GestureSource* gesture_source);

  ~ActiveWishes();

  void
  set_wish_granted_callback(Callback const& wish_granted_callback);

  void
  set_wish_revoked_callback(Callback const& wish_revoked_callback);

  void
  grant_wishes_for_window(Wish::Table const& wishes, Window const* window);

  void
  revoke_wishes_for_window(Window const* window);

  void
  process_gesture_event(GestureEvent const& gesture_event,
                        ActionSink*         action_sink);

private:
  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_ACTIVEWISHES_H_

