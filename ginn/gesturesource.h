/**
 * @file ginn/gesturesource.h
 * @brief Interface for gesture sources.
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
#ifndef GINN_GESTURESOURCE_H_
#define GINN_GESTURESOURCE_H_

#include <functional>
#include "ginn/application.h"
#include "ginn/wish.h"
#include <memory>


namespace Ginn
{
class Window;

/**
 * An abstract class wrapping gesture events.
 */
class GestureEvent
{
public:
  virtual
  ~GestureEvent() = 0;

  virtual bool
  matches(Window const* window, Wish::Ptr const& wish) const = 0;
};


/**
 * An abstract class representing a single gesture subscription.
 */
class GestureSubscription
{
public:
  using Ptr = std::unique_ptr<GestureSubscription>;

public:
  virtual
  ~GestureSubscription() = 0;
};


/**
 * An abstract source of gesture events.
 */
class GestureSource
{
public:
  /** Signal indicating a new gesture event has been received. */
  using EventReceivedCallback = std::function<void(GestureEvent const&)>;

  /** Signal for when the gesture source has completed its asynch initi. */
  using InitializedCallback = std::function<void()>;

public:
  virtual
  ~GestureSource() = 0;

  virtual void
  set_initialized_callback(InitializedCallback const& initialized_callback) = 0;

  virtual void
  set_event_callback(EventReceivedCallback const& event_callback) = 0;

  virtual GestureSubscription::Ptr
  subscribe(Window::Id window_id, Wish::Ptr const& wish) = 0;
};

} // namespace Ginn

#endif // GINN_GESTURESOURCE_H_

