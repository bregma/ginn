/**
 * @file test/fakekeymap.h
 * @brief A fake Keymap for testing.
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
#ifndef GINN_FAKEKEYMAP_H_
#define GINN_FAKEKEYMAP_H_

#include "ginn/keymap.h"


namespace Ginn
{

/**
 * A place where actions end up getting sent.
 */
class FakeKeymap
: public Keymap
{
public:
  FakeKeymap();
  ~FakeKeymap();

  void
  set_initialized_callback(InitializedCallback const& callback);

  Keycode
  to_keycode(std::string const& keysym_name) const;
};

} // namespace Ginn

#endif // GINN_FAKEKEYMAP_H_

