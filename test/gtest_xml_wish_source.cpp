/**
 * @file test/gtest_xml_wish_source.cpp
 * @brief Unit tests of teh XML wish source module.
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
#include "ginn/wishsource.h"
#include <gtest/gtest.h>


TEST(TestXMLWishSource, construct)
{
  Ginn::WishSource::Ptr ptr = Ginn::WishSource::factory(Ginn::WishSource::Format::XML, "");
  ASSERT_NE(ptr, nullptr);
}

