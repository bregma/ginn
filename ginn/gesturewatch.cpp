/**
 * @file ginn/gesturewatch.cpp
 * @brief Declarations of the Ginn GestureWatch class.
 */

/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include "ginn/gesturewatch.h"

#include <iomanip>
#include <iostream>
#include <utility>


namespace Ginn
{

GestureWatch::
GestureWatch(Window::Id                window_id,
             Application::Ptr          app,
             Wish::Ptr                 wish,
             GestureSubscription::Ptr  subscription)
: window_id_(window_id)
, application_(app)
, wish_(wish)
, subscription_(std::move(subscription))
{
}


GestureWatch::
~GestureWatch()
{
}


std::ostream&
operator<<(std::ostream& ostr, GestureWatch const& watch)
{
  Window const* const w = watch.application_->window(watch.window_id_);
  return ostr << "app '" << watch.application_->name()
              << "' window "
              << std::hex << std::setw(8) << std::setfill('0') << std::showbase
              << watch.window_id_ << std::dec << " '" << w->title_ << "'";
}


} // namespace Ginn


