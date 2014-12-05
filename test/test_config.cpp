/**
 * @file test/test_configuration.cpp
 * @brief Unit tests of the Ginn Configuration module.
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
#include "ginn/configuration.h"

#include <cstdlib>
#include <gtest/gtest.h>
#include <string.h>
#include <vector>


/**
 * Fixture for setting up and tearing down command-line arguments.
 *
 * ISO/IEC 9899 [5.1.2.2.1](2)(e) says argv and its content shall be writable.
 */
class Configuration
: public ::testing::Test
{
public:
  Configuration()
  : argc_(1)
  , argv_(argc_, nullptr)
  {
    add_argument("verify_ginn");
  }

  ~Configuration()
  {
    for (int i = 0; i < argc_; ++i)
      std::free(argv_[i]);
  }

  void
  add_argument(char const* const arg)
  {
    argv_.back() = strdup(arg);
    argv_.push_back(nullptr);
    argc_ = argv_.size() - 1;
  }

protected:
  int                argc_;
  std::vector<char*> argv_;
};


TEST_F(Configuration, Ginn_default_values)
{
  Ginn::Configuration config(argc_, &argv_[0]);

  EXPECT_EQ(config.is_verbose_mode(), false);
}

TEST_F(Configuration, Ginn_verbose_mode)
{
  add_argument("--verbose");
  Ginn::Configuration config(argc_, &argv_[0]);

  EXPECT_EQ(config.is_verbose_mode(), true);
}

TEST_F(Configuration, WishSource_default_values)
{
  Ginn::Configuration config(argc_, &argv_[0]);

  EXPECT_EQ(config.wish_source_format(), Ginn::WishSourceConfig::Format::XML);
  EXPECT_EQ(config.wish_schema_file_name(), Ginn::WishSourceConfig::WISH_NO_VALIDATE);

  Ginn::WishSourceConfig::SourceNameList source_name_list = config.wish_sources();
  EXPECT_GT(source_name_list.size(), 0);
}

