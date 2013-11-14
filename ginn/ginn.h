/**
 * @file ginn/ginn.h
 * @brief Declarations of the Ginn module.
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
#ifndef GINN_GINN_H_
#define GINN_GINN_H_

#include <memory>


namespace Ginn
{

/**
 * The Ginn module encapsulates everything necessary for the daemon to grant
 * wishes to the lucky uncpator.
 */
class Ginn
{
public:
  /** Lets the Ginn out of te bottle. */
  Ginn(int argc, char* argv[]);

  /** Puts the Ginn back in the bottle. */
  ~Ginn();

  /** Lets the Ginn go wild. */
  void
  run();

private:
  struct Impl;

  std::unique_ptr<Impl> impl_;
};

} // namespace Ginn

#endif // GINN_GINN_H_

