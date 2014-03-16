/**
 * @file ginn/application.h
 * @brief Declarations of the Ginn Application class.
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
#ifndef GINN_APPLICATION_H_
#define GINN_APPLICATION_H_

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ginn/window.h"


namespace Ginn
{
class ApplicationBuilder;


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
  Window::List const&
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

  /** Dump the application information to an output stream. */
  std::ostream&
  dump(std::ostream& ostr) const;

private:
  Id            application_id_;  ///< name of the desktop file
  std::string   name_;            ///< Name key in the desktop file
  std::string   generic_name_;    ///< GenericName key in the desktop file
  Window::List  windows_;         ///< list of open windows
};


inline std::ostream&
operator<<(std::ostream& ostr, Application const& app)
{ return app.dump(ostr); }

} // namespace Ginn

#endif // GINN_APPLICATION_H_

