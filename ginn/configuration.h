/**
 * @file ginn/configuration.h
 * @brief Declarations of the Ginn Confiuration module.
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
#ifndef GINN_APPLICATION_H_
#define GINN_APPLICATION_H_

namespace Ginn
{

/**
 * Consolidated configuration values.
 */
class Configuration
{
public:
  Configuration(int argc, char* argv[]);
};

} // namespace Ginn

#endif // GINN_APPLICATION_H_

