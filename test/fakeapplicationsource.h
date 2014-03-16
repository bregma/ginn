/**
 * @file test/fakeapplicationsource.h
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
#ifndef GINN_FAKEAPPLICATIONSOURCE_H_
#define GINN_FAKEAPPLICATIONSOURCE_H_

#include "ginn/applicationsource.h"


namespace Ginn
{

class FakeApplicationSource
: public ApplicationSource
{
public:
  ~FakeApplicationSource();

  void
  set_observer(ApplicationObserver* observer);

  virtual void
  set_window_opened_callback(WindowOpenedCallback const& callback) override;

  virtual void
  set_window_closed_callback(WindowClosedCallback const& callback) override;

  Application::List
  get_applications();
};

} // namespace Ginn

#endif // GINN_FAKEAPPLICATIONSOURCE_H_

