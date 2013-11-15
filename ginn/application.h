/**
 * @file ginn/application.h
 * @brief Declarations of the Ginn Application class.
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

#include <map>
#include <memory>
#include <string>
#include <vector>


namespace Ginn
{
class ApplicationBuilder;


/**
 * Information on an X11 window owned by the app
 *
 * @todo make this a first-class object
 */
struct Window
{
  /** A unique identifier for an application window. */
  typedef unsigned long Id;

  Id            window_id;
  std::string   title;
  std::string   application_id;
  bool          is_active;
  bool          is_visible;
  int           monitor;
};


/**
 * A proxy for an application targetted to receive gestural input.
 */
class Application
{
public:
  /** A unique identifier for an application. */
  typedef std::string                  Id;
  /** A (shared) pointer to an Applciation. */
  typedef std::shared_ptr<Application> Ptr;
  /** A collection of Applications keyed by application_id. */
  typedef std::map<std::string, Ptr>   List;
  /** A collection of windows associated with an application. */
  typedef std::vector<Window>          Windows;

public:
  /** builds an Application */
  Application(const ApplicationBuilder& builder);

  Id const&
  application_id() const;

  std::string const&
  name() const;

  std::string const&
  generic_name() const;

  /** Gets all current windows for the application. */
  Windows const&
  windows() const;

  /** Gets a particular cuurent window by window_id. */
  Window const*
  window(Window::Id window_id) const;

  /** Adds a new tracked window. */
  void
  add_window(Window const& window);

  /** Removes a particular cuurent window by window_id. */
  void
  remove_window(Window::Id window_id);

private:
  Id          application_id_;  ///< name of the desktop file
  std::string name_;            ///< Name key in the desktop file
  std::string generic_name_;    ///< GenericName key in the desktop file
  Windows     windows_;
};

} // namespace Ginn

#endif // GINN_APPLICATION_H_

