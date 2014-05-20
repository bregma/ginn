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

#include "ginn/applicationbuilder.h"


namespace Ginn
{

class FakeApplicationBuilder
: public ApplicationBuilder
{
public:
  FakeApplicationBuilder(Application::Id const& id,
                         std::string const&     name,
                         std::string const&     generic_name)
  : id_(id), name_(name), generic_name_(generic_name)
  { }

  ~FakeApplicationBuilder()
  { }

  virtual Application::Id
  application_id() const
  { return id_; }

  virtual std::string
  name() const
  { return name_; }

  virtual std::string
  generic_name() const
  { return generic_name_; }

private:
  Application::Id id_;
  std::string     name_;
  std::string     generic_name_;
};


FakeApplicationSource::
~FakeApplicationSource()
{ }


void FakeApplicationSource::
set_initialized_callback(InitializedCallback const& callback)
{
  initialized_callback_ = callback;
}


void FakeApplicationSource::
set_window_opened_callback(WindowOpenedCallback const& callback)
{
  window_opened_callback_ = callback;
}


void FakeApplicationSource::
set_window_closed_callback(WindowClosedCallback const& callback)
{
  window_closed_callback_ = callback;
}


void FakeApplicationSource::
add_application(Application::Id const& id,
                std::string const&     name,
                std::string const&     generic_name)
{
  apps_[id] = std::make_shared<Application>(FakeApplicationBuilder(id,
                                                                   name,
                                                                   generic_name));
}


void FakeApplicationSource::
remove_application(Application::Id const& id)
{
  auto const& app_it = apps_.find(id);
  if (app_it != std::end(apps_))
  {
    app_it->second->for_all_windows([this](Window const* window)
    {
      if (window_closed_callback_)
        window_closed_callback_(window);
    });
  }
}


void FakeApplicationSource::
add_window(Application::Id const& app_id,
           Window::Id const&      window_id)
{
  auto const& app_it = apps_.find(app_id);
  if (app_it != std::end(apps_))
  {
    std::unique_ptr<Window> window(new Window{ window_id, "A Window", app_it->second.get(), true, true, 0 });
    if (window_opened_callback_)
      window_opened_callback_(window.get());
    app_it->second->add_window(std::move(window));
  }
}


void FakeApplicationSource::
remove_window(Window::Id window_id)
{
  for (auto app: apps_)
  {
    if (Window const* window = app.second->window(window_id))
    {
      if (window_closed_callback_)
        window_closed_callback_(window);
    }
  }
}


void FakeApplicationSource::
complete_initialization()
{
  for (auto const& app: apps_)
  {
    app.second->for_all_windows([this](Window const* window)
    {
      if (window_opened_callback_)
        window_opened_callback_(window);
    });
  }
  if (initialized_callback_)
    initialized_callback_();
}


} // namespace Ginn


