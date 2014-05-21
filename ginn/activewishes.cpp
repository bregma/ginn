/**
 * @file ginn/activewishes.cpp
 * @brief Definitions of the Ginn ActiveWishes module.
 */

/*
 * Copyright 2014 Canonical Ltd.
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
#include "ginn/activewishes.h"

#include "ginn/applicationsource.h"
#include "ginn/configuration.h"
#include <iostream>


namespace Ginn
{

struct ActiveWishes::Impl
{
  Impl(Configuration const& config,
       Wish::Table const&   wishes,
       ApplicationSource*   app_source);

  void
  window_opened(Window const* window);

  void
  window_closed(Window const* window);

  Configuration      config_;
  Wish::Table        wishes_;
  ApplicationSource* app_source_;
  Callback           wish_granted_callback_;
  Callback           wish_revoked_callback_;
};


ActiveWishes::Impl::
Impl(Configuration const& config,
     Wish::Table const&   wishes,
     ApplicationSource*   app_source)
: config_(config)
, wishes_(wishes)
, app_source_(app_source)
{
  using std::bind;
  using std::placeholders::_1;

  app_source_->set_window_opened_callback(bind(&ActiveWishes::Impl::window_opened, this, _1));
  app_source_->set_window_closed_callback(bind(&ActiveWishes::Impl::window_closed, this, _1));
}


void ActiveWishes::Impl::
window_opened(Window const* window)
{
  if (config_.is_verbose_mode())
    std::cout << __PRETTY_FUNCTION__ << " window added: " << *window << "\n";
#if 0
  auto app = apps_.find(window.application_id);
  if (app != apps_.end())
  {
    if (config_.is_verbose_mode())
      std::cout << "window added: " << *window << "\n";
    app->second->add_window(window);

    if (wish_granted_callback_)
    {
      for (auto const& appwish: wishes_)
      {
        if (app->first == appwish.first)
        {
          for (auto const& wish: appwish.second)
          {
            wish_granted_callback_(*wish.second, *window);
          }
        }
      }
    }
  }
#endif
}


void ActiveWishes::Impl::
window_closed(Window const* window)
{
  if (window)
  {
    if (config_.is_verbose_mode())
      std::cout << __PRETTY_FUNCTION__ << " window removed: " << *window << "\n";;
#if 0
    if (wish_revoked_callback_)
    {
      for (auto const& appwish: wishes_)
      {
        if (app.first == appwish.first)
        {
          for (auto const& wish: appwish.second)
          {
            wish_revoked_callback_(*wish.second, *window);
          }
        }
      }
    }
    app.second->remove_window(window_id);
    break;
#endif
  }
}


ActiveWishes::
ActiveWishes(Configuration const& config,
             Wish::Table const&   wishes,
             ApplicationSource*   app_source)
: impl_(new Impl(config, wishes, app_source))
{
}


ActiveWishes::
ActiveWishes()
{ }


ActiveWishes::
~ActiveWishes()
{ }


void ActiveWishes::
set_wish_granted_callback(Callback const& wish_granted_callback)
{
  impl_->wish_granted_callback_ = wish_granted_callback;
}


void ActiveWishes::
set_wish_revoked_callback(Callback const& wish_revoked_callback)
{
  impl_->wish_revoked_callback_ = wish_revoked_callback;
}


} // namespace Ginn

