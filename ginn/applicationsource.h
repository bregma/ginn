/**
 * @file ginn/applicationsource.h
 * @brief Declarations of the Ginn Application Source interface.
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
#ifndef GINN_APPLICATIONSOURCE_H_
#define GINN_APPLICATIONSOURCE_H_

#include "ginn/application.h"
#include <string>


namespace Ginn
{
class ApplicationObserver;
class Configuration;

/**
 * An abstract interface for Application sources.
 *
 * Folks, they gotta come from somewhere.  Not only does every source of
 * Applications look like this-here interface, but it provides a factory
 * function for the sole purpose of creating such a source just given its
 * name.
 */
class ApplicationSource
{
public:

  /**
   * Supported types of application sources.
   * @todo support additional types of application sources.
   */
  enum class Type
  {
    BAMF,
  };

public:
  virtual ~ApplicationSource() = 0;

  /** Sets the observer. */
  virtual void
  set_observer(ApplicationObserver* observer) = 0;

  /** Sources Applications. */
  virtual Application::List
  get_applications() = 0;
};
}

#endif // GINN_APPLICATIONSOURCE_H_

