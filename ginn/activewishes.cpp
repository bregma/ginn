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

#include <cassert>
#include "ginn/applicationsource.h"
#include "ginn/configuration.h"
#include "ginn/gesturesource.h"
#include <iostream>
#include <vector>


namespace Ginn
{

/**
 * A tuple relating a Wish, an Application Window, and a Gesture Subscription.
 */
struct WishWindowSub
{
  Wish::Ptr                wish_;
  Window const*            window_;
  GestureSubscription::Ptr subscription_;
};

using WishSubs = std::vector<WishWindowSub>;


struct ActiveWishes::Impl
{
  Impl(Configuration const& config, GestureSource* gesture_source);

  void
  window_opened(Window const* window);

  void
  window_closed(Window const* window);

  void
  grant_wishes(Wish::Table const& wishes, Window const* window);

  Configuration      config_;
  GestureSource*     gesture_source_;
  WishSubs           wish_subs_;
  Callback           wish_granted_callback_;
  Callback           wish_revoked_callback_;
};


ActiveWishes::Impl::
Impl(Configuration const& config, GestureSource* gesture_source)
: config_(config)
, gesture_source_(gesture_source)
{ }


ActiveWishes::
ActiveWishes(Configuration const& config, GestureSource* gesture_source)
: impl_(new Impl(config, gesture_source))
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


void ActiveWishes::
grant_wishes_for_window(Wish::Table const& wishes, Window const* window)
{
  assert(window != nullptr);

  Application const* app = window->application_;
  assert(app != nullptr);

  auto wish_table_it = wishes.find(app->application_id());
  if (wish_table_it == std::end(wishes))
  {
    wish_table_it = wishes.find(app->name());
  }
  if (wish_table_it != std::end(wishes))
  {
    for (auto const& wish: wish_table_it->second)
    {
      if (impl_->config_.is_verbose_mode())
        std::cout << __PRETTY_FUNCTION__ << " granting wish '" << wish.second->name() << "'for window: " << *window << "\n";

      /** @todo: actually grant wish */
      impl_->wish_subs_.push_back(WishWindowSub{wish.second,
                                                window,
                                                impl_->gesture_source_->subscribe(window->id_, wish.second)});

      if (impl_->wish_granted_callback_)
        impl_->wish_granted_callback_(*wish.second, *window);
    }
  }
}


void ActiveWishes::
revoke_wishes_for_window(Window const* window)
{
  assert(window != nullptr);

  auto it = std::begin(impl_->wish_subs_);
  while (it != std::end(impl_->wish_subs_))
  {
    if (it->window_ == window)
    {
      if (impl_->wish_revoked_callback_)
      {
        impl_->wish_revoked_callback_(*it->wish_, *window);
      }
      if (impl_->config_.is_verbose_mode())
        std::cout << __PRETTY_FUNCTION__ << " wish " << *it->wish_
                  << " revoked for window " << *window << "\n";;
      it = impl_->wish_subs_.erase(it);
    }
    else
    {
      ++it;
    }
  }
  if (impl_->config_.is_verbose_mode())
    std::cout << __PRETTY_FUNCTION__ << " window removed: " << *window << "\n";;
}


} // namespace Ginn

