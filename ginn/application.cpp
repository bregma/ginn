/**
 * @file ginn/application.cpp
 * @brief Definitions of the Ginn Application class.
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
#include "ginn/application.h"

#include <algorithm>
#include "ginn/applicationbuilder.h"
#include <iostream>
#include <utility>


namespace Ginn
{

Application::
Application(const ApplicationBuilder& builder)
: application_id_(std::move(builder.application_id()))
, name_(std::move(builder.name()))
, generic_name_(std::move(builder.generic_name()))
, windows_(std::move(builder.windows()))
{
}


Application::Id const& Application::
application_id() const
{
  return application_id_;
}


std::string const& Application::
name() const
{
  return name_;
}


std::string const& Application::
generic_name() const
{
  return generic_name_;
}

Window::List const& Application::
windows() const
{
  return windows_;
}


Window const* Application::
window(Window::Id window_id) const
{
  auto it = std::find_if(windows_.begin(), windows_.end(),
                         [&window_id](Window const& w) -> bool
                         { return window_id == w.id_; });
  if (it == windows_.end())
    return nullptr;
  return &*it;
}


void Application::
add_window(Window const& window)
{
  windows_.push_back(window);
}


void Application::
remove_window(Window::Id window_id)
{
  auto it = std::find_if(windows_.begin(), windows_.end(),
                         [&window_id](Window const& w) -> bool
                         { return window_id == w.id_; });
  if (it != windows_.end())
    windows_.erase(it);
}


std::ostream& Application::
dump(std::ostream& ostr) const
{
  ostr << "application_id=\"" << application_id() << "\""
       << " name=\"" << name() << "\""
       << " generic_name=\"" << generic_name() << "\""
       << "\n";
  for (auto const& window: windows())
  {
    std::cout << "    " << window << "\n";;
  }
  return ostr;
}


}

