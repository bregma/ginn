/**
 * @file ginn/geisgesturesource.cpp
 * @brief Implementation of the GEIS gesture source.
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
#include "ginn/geisgesturesource.h"

#include "ginn/configuration.h"
#include <iostream>


namespace Ginn
{

struct GeisGestureSource::Impl
{
  Impl(Configuration const& config);

  Configuration config_;
};

GeisGestureSource::Impl::
Impl(Configuration const& config)
: config_(config)
{
}


GeisGestureSource::
GeisGestureSource(Configuration const& config)
: impl_(new Impl(config))
{
  if (impl_->config_.is_verbose_mode())
    std::cout << __FUNCTION__ << " created\n";
}


GeisGestureSource::
~GeisGestureSource()
{
}


void GeisGestureSource::
set_initialized_callback(InitializedCallback const&)
{
}


void GeisGestureSource::
set_event_callback(EventReceivedCallback const&)
{
}


GestureSubscription::Ptr GeisGestureSource::
subscribe(Window::Id, Wish::Ptr const&)
{
  GestureSubscription::Ptr p;
  return p;
}


} // namespace Ginn

