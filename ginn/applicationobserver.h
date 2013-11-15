/**
 * @file ginn/applicationobserver.h
 * @brief Declarations of the Ginn Application Observer class.
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
#ifndef GINN_APPLICATIONOBSERVER_H_
#define GINN_APPLICATIONOBSERVER_H_

#include "ginn/application.h"


namespace Ginn
{

  /**
   * A mixin interface for things that observe application actions.
   */
  class ApplicationObserver
  {
  public:
    virtual void
    application_added(Application::Ptr const& application) = 0;

    virtual void
    application_removed(Application::Id const& application_id) = 0;

    virtual void
    window_added(Window const& window) = 0;

    virtual void
    window_removed(Window::Id window_id) = 0;
  };
}

#endif // GINN_APPLICATIONOBSERVER_H_

