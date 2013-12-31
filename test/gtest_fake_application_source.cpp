/**
 * @file test/gtest_fake_application_source.cpp
 * @brief Unit tests of the fake action sink.
 * Makes sure the fake action sink works, otherwise you can;t depend on tests
 * that use it.
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
#include "fakeapplicationsource.h"
#include <gtest/gtest.h>
#include <memory>


TEST(FakeApplicationSource, construct)
{
  std::unique_ptr<Ginn::ApplicationSource> app_source(Ginn::FakeApplicationSource());
  ASSERT_TRUE(app_source);
}
