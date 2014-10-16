/**
 * @file ginn/applicationsource.h
 * @brief Declarations of the Ginn Application Source interface.
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
#ifndef GINN_APPLICATIONSOURCE_H_
#define GINN_APPLICATIONSOURCE_H_

#include <functional>
#include "ginn/application.h"
#include <string>


namespace Ginn
{


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
  /** Signal for when the gesture source has completed its asynch init. */
  using InitializedCallback = std::function<void()>;

  /** Signal indicating a new application window has been opened. */
  using WindowOpenedCallback = std::function<void(Window const*)>;

  /** Signal indicating an application window has been closed. */
  using WindowClosedCallback = std::function<void(Window const*)>;

public:
  virtual ~ApplicationSource() = 0;

  /**
   * Sets a callback to be invoked when the app source has completed its
   * asynchronous initialization.
   */
  virtual void
  set_initialized_callback(InitializedCallback const& initialized_callback) = 0;

  /**
   * Sets a callback to be invoked when a new window has been opened.
   */
  virtual void
  set_window_opened_callback(WindowOpenedCallback const& callback) = 0;

  /**
   * Sets a callback to be invoked when a window has been closed.
   */
  virtual void
  set_window_closed_callback(WindowClosedCallback const& callback) = 0;

  /**
   * Reports all currently known windows.
   *
   * Should be used after asynchronous initialization has been signalled and the
   * caller is ready to report any windows discovered during initialization.
   */
  virtual void
  report_windows() = 0;
};

} // namespace Ginn

#endif // GINN_APPLICATIONSOURCE_H_

