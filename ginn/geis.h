/**
 * @file ginn/geis.h
 * @brief Interface to the Ginn GEIS module.
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
#ifndef GINN_GEIS_H_
#define GINN_GEIS_H_

#include "geis/geis.h"
#include "ginn/application.h"
#include "ginn/wish.h"
#include <memory>


namespace Ginn
{
class GeisObserver;


class Geis
{
public:
  class Subscription
  {
  public:
    Subscription(GeisSubscription subscription);
    Subscription(Subscription&& rhs);
    ~Subscription();

  private:
    GeisSubscription subscription_;
  };

public:
  Geis(GeisObserver* observer);

  ~Geis();

  Subscription
  subscribe(Window::Id window_id, Wish::Ptr const& wish);

private:
  struct Impl;

  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_GEIS_H_

