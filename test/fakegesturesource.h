/**
 * @file test/fakegesturesource.h
 * @brief A fake GestureSource for testing.
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
#ifndef GINN_FAKEGESTURESOURCE_H_
#define GINN_FAKEGESTURESOURCE_H_

#include "ginn/gesturesource.h"


namespace Ginn
{

/**
 * A place where actions end up getting sent.
 */
class FakeGestureSource
: public GestureSource
{
public:
  FakeGestureSource();
  ~FakeGestureSource();

  void
  set_initialized_callback(InitializedCallback const& callback);

  void
  set_event_callback(EventReceivedCallback const& event_callback);

  virtual GestureSubscription::Ptr
  subscribe(Window::Id window_id, Wish::Ptr const& wish);
};

} // namespace Ginn

#endif // GINN_FAKEGESTURESOURCE_H_

