/**
 * @file ginn/keymap.h
 * @brief Interface to the Ginn keymap module.
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
#ifndef GINN_KEYMAP_H_
#define GINN_KEYMAP_H_

#include <cstdint>
#include <functional>
#include <string>


namespace Ginn
{

class Keymap
{
public:
  /** The encoded key (may have to change for non-X11 support). */
  using Keycode = std::uint8_t;

  /** Signal for when Keymap has completed its asynchronous initialization. */
  using InitializedCallback = std::function<void()>;

public:
  virtual
  ~Keymap() = 0;

  virtual void
  set_initialized_callback(InitializedCallback const& initialized_callback) = 0;

  virtual Keycode
  to_keycode(std::string const& keysym_name) const = 0;
};

} // namespace Ginn

#endif // GINN_KEYMAP_H_

