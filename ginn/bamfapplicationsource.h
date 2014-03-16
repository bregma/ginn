/**
 * @file ginn/bamfapplicationsource.h
 * @brief Declarations of the Ginn BAMF Application Source class.
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
#ifndef GINN_BAMFAPPLICATIONSOURCE_H_
#define GINN_BAMFAPPLICATIONSOURCE_H_

#include "ginn/applicationsource.h"
#include <memory>


namespace Ginn
{

class Configuration;

/**
 * A factory class to load Applications through BAMF.
 */
class BamfApplicationSource
: public ApplicationSource
{
public:
  BamfApplicationSource(Configuration const& config);
  ~BamfApplicationSource();

void
set_window_opened_callback(WindowOpenedCallback const& callback) override;

void
set_window_closed_callback(WindowClosedCallback const& callback) override;

Application::List
get_applications();

private:
  struct Impl;

  std::unique_ptr<Impl> impl_;
};

}

#endif // GINN_BAMFAPPLICATIONSOURCE_H_

