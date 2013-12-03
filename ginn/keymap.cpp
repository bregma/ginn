/**
 * @file ginn/keymap.cpp
 * @brief Implementation of the the Ginn keymap module.
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
#include "ginn/keymap.h"


namespace Ginn
{

struct Keymap::Impl
{
  Impl(InitializedCallback initialized_callback)
  : initialized_callback_(initialized_callback)
  { }

  InitializedCallback initialized_callback_;
};


Keymap::
Keymap(InitializedCallback const& initialized_callback)
: impl_(new Impl(initialized_callback))
{
  impl_->initialized_callback_();
}


Keymap::
~Keymap()
{
}


Keymap::Keycode Keymap::
to_keycode(std::string const& keysym_name)
{
  return 0;
}

} // namespace Ginn

