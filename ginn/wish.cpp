/**
 * @file ginn/wish.cpp
 * @brief Definitions of the Ginn Wish class.
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
#include "ginn/wish.h"

#include "ginn/wishbuilder.h"
#include <iostream>
#include <utility>


namespace Ginn
{

Wish::
Wish(const WishBuilder& builder)
: name_(std::move(builder.name()))
, gesture_(std::move(builder.gesture()))
, touches_(std::move(builder.touches()))
, when_(std::move(builder.when()))
, property_(std::move(builder.property()))
, min_(builder.min())
, max_(builder.max())
, action_(std::move(builder.action()))
{
}

std::ostream&
operator<<(std::ostream& ostr, Wish const& wish)
{
  ostr << wish.name() << " {" << wish.gesture() << wish.touches() << "}";
  return ostr;
}


}

