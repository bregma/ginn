/**
 * @file ginn/geisgesturesource.h
 * @brief Interface for the GEIS gesture source.
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
#ifndef GINN_GEISGESTURESOURCE_H_
#define GINN_GEISGESTURESOURCE_H_

#include "ginn/gesturesource.h"
#include <memory>


namespace Ginn
{
class Configuration;

/**
 * An concrete source of gesture events built on GEIS.
 */
class GeisGestureSource
: public GestureSource
{
public:
  struct Impl;

public:
  GeisGestureSource(Configuration const& config);
  ~GeisGestureSource();

  void
  set_initialized_callback(InitializedCallback const& initialized_callback);

  void
  set_event_callback(EventReceivedCallback const& event_callback);

  GestureSubscription::Ptr
  subscribe(Window::Id window_id, Wish::Ptr const& wish);

private:
  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_GEISGESTURESOURCE_H_

