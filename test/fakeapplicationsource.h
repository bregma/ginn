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
  FakeApplicationSource();
  ~FakeApplicationSource();

  void
  set_initialized_callback(InitializedCallback const& callback) override;

  virtual void
  set_window_opened_callback(WindowOpenedCallback const& callback) override;

  virtual void
  set_window_closed_callback(WindowClosedCallback const& callback) override;

  void
  add_application(Application::Id const& id,
                  std::string const&     name,
                  std::string const&     generic_name);

  void
  add_window(Application::Id const& app_id,
             Window::Id const&      window_id);

  void
  remove_window(Window::Id window_id);

  void
  remove_application(Application::Id const& id);

  void
  complete_initialization();

  void
  report_windows() override;

private:
  bool                 is_initialized_;
  InitializedCallback  initialized_callback_;
  WindowOpenedCallback window_opened_callback_;
  WindowClosedCallback window_closed_callback_;
  Application::List    apps_;
};

} // namespace Ginn

#endif // GINN_FAKEAPPLICATIONSOURCE_H_

