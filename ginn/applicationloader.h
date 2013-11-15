/**
 * @file ginn/applicationloader.h
 * @brief Declarations of the Ginn Application Loader interface.
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
#ifndef GINN_APPLICATIONLOADER_H_
#define GINN_APPLICATIONLOADER_H_

#include "ginn/application.h"
#include "ginn/applicationobserver.h"
#include <memory>
#include <string>


namespace Ginn
{

  /**
   * A factory class to load Applications through BAMF.
   */
  class ApplicationLoader
  {
  public:
    typedef std::shared_ptr<ApplicationLoader> Ptr;

  public:
    virtual ~ApplicationLoader() = 0;

    static Ptr
    application_loader_factory(std::string const&   name,
                               ApplicationObserver* observer);

    virtual Application::List
    get_applications() = 0;
  };
}

#endif // GINN_APPLICATIONLOADER_H_

