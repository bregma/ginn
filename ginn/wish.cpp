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

#include <utility>
#include "ginn/wishbuilder.h"


namespace Ginn
{

Wish::
Wish(const WishBuilder& builder)
: name_(std::move(builder.name()))
, when_(std::move(builder.when()))
, property_(std::move(builder.property()))
, min_(builder.min())
, max_(builder.max())
, action_(std::move(builder.action()))
, modifier1_(std::move(builder.modifier1()))
, modifier2_(std::move(builder.modifier2()))
, modifier3_(std::move(builder.modifier3()))
{
}


}

