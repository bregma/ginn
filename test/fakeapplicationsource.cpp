/**
 * @file test/fakeapplicationsource.hcpp
 * @brief A fake FakeApplicationSource for testing.
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
#include "fakeapplicationsource.h"


namespace Ginn
{

FakeApplicationSource::
~FakeApplicationSource()
{ }


void FakeApplicationSource::
set_window_opened_callback(WindowOpenedCallback const&)
{
}


void FakeApplicationSource::
set_window_closed_callback(WindowClosedCallback const&)
{
}


Application::List FakeApplicationSource::
get_applications()
{
  Application::List app_list;
  return app_list;
}


} // namespace Ginn


