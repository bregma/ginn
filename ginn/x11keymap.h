/**
 * @file ginn/x11keymap.h
 * @brief Interface to the Ginn X11 keymap module.
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
#ifndef GINN_X11KEYMAP_H_
#define GINN_X11KEYMAP_H_

#include "ginn/keymap.h"
#include <memory>


namespace Ginn
{
class Configuration;

class X11Keymap
: public Keymap
{
public:
  struct Impl;

public:
  X11Keymap(Configuration const& config);

  ~X11Keymap();

  void
  set_initialized_callback(InitializedCallback const& initialized_callback);

  Keycode
  to_keycode(std::string const& keysym_name) const;

private:
  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_X11KEYMAP_H_

