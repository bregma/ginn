/**
 * @file test/mockwishsourceconfig.h
 * @brief a mock Ginn Wish Source Config
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
#ifndef TEST_MOCK_WISHSOURCECONFIG_H_
#define TEST_MOCK_WISHSOURCECONFIG_H_

#include "ginn/wishsourceconfig.h"
#include <gmock/gmock.h>


class MockWishSourceConfig
: public Ginn::WishSourceConfig
{
public:
  ~MockWishSourceConfig() { }

  MOCK_CONST_METHOD0(is_verbose_mode, bool());
  MOCK_CONST_METHOD0(wish_source_format, Format());
  MOCK_CONST_METHOD0(wish_sources, SourceNameList const&());
  MOCK_CONST_METHOD0(wish_schema_file_name, std::string const&());
};

#endif // TEST_MOCK_WISHSOURCECONFIG_H_

