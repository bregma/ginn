/**
 * @file ginn/applicationbuilder.h
 * @brief Declarations of the Ginn ApplicationBuilder interface.
 */

/*
 * Copyright 2013-2014 Canonical Ltd.
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
#ifndef GINN_APPLICATIONBUILDER_H_
#define GINN_APPLICATIONBUILDER_H_

#include "ginn/application.h"
#include <memory>
#include <string>


namespace Ginn
{

/**
 * Interface for building an Application object.
 */
class ApplicationBuilder
{
public:
  virtual ~ApplicationBuilder() = 0;

  virtual Application::Id
  application_id() const = 0;

  virtual std::string
  name() const = 0;

  virtual std::string
  generic_name() const = 0;
};

} // namespace Ginn

#endif // GINN_APPLICATIONBUILDER_H_

