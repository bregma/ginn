/**
 * @file test/environment.cpp
 * @brief The Ginn test environment.
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
#include "test/environment.h"

#include <gtest/gtest.h>


namespace Ginn
{
namespace Test
{

Environment* Environment::ptr_;


Environment::
Environment(int argc, char** argv)
: config_(argc, argv)
{ }


Configuration const& Environment::
config()
{
  return ptr_->config_;
}


/**
 * Creates a Ginn test environment.
 *
 * This environment allows individual tests to be manually run with additional
 * command-line parameters.  not all testing is regression or refactor testing:
 * sometimes you need to analyse and fix bugs, too.
 *
 * This is a very roundabout and unintuitive way to setting up the test
 * environment because the Google Test toolkit deletes the environment, so while
 * it's not documented that way, you are required to new your environments and
 * follow Google's standard C-with-classes methodologies.
 */
void
create_test_environment(int argc, char** argv)
{
  Environment::ptr_ = new Environment(argc, argv);
  ::testing::AddGlobalTestEnvironment(Environment::ptr_);
}


} // namespace Test
} // namespace Ginn

