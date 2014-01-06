/**
 * @file test/fakegesturesource.cpp
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
#include "fakegesturesource.h"


namespace Ginn
{

FakeGestureSource::
FakeGestureSource()
{ }


FakeGestureSource::
~FakeGestureSource()
{ }


void FakeGestureSource::
set_initialized_callback(InitializedCallback const&)
{ }


void FakeGestureSource::
set_event_callback(EventReceivedCallback const&)
{ }


GestureSubscription::Ptr FakeGestureSource::
subscribe(Window::Id, Wish::Ptr const&)
{
  GestureSubscription::Ptr p;
  return p;
}

} // namespace Ginn

