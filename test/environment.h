/**
 * @file test/environment.h
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
#ifndef GINN_TEST_ENVIRONMENT_H_
#define GINN_TEST_ENVIRONMENT_H_

#include "ginn/configuration.h"
#include <gtest/gtest.h>
#include <memory>


namespace Ginn
{
namespace Test
{

/**
 * The Ginn test environment contains a Ginn configuration so that manual test
 * runs can be toyed with in unsuspecting ways for enhanced problem analysis.
 */
class Environment
: public ::testing::Environment
{
public:
  static Configuration const&
  config();

  friend void
  create_test_environment(int argc, char** argv);

private:
  /** Constructs the Ginn testing environment from the command line. */
  Environment(int argc, char** argv);

  /** The global environment instance */
  static Environment* ptr_;

  /** The global configuration. */
  Configuration config_;
};

/** Creates and registers the Ginn test environment. */
void
create_test_environment(int argc, char** argv);

} // namespace Test
} // namespace Ginn

#endif // GINN_TEST_ENVIRONMENT_H_

