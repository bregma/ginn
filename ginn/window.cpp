/**
 * @file ginn/window.cpp
 * @brief Definitions of the Ginn Application Window class.
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
#include "ginn/window.h"

#include "ginn/application.h"
#include <iomanip>
#include <iostream>


namespace Ginn
{

std::ostream&
operator<<(std::ostream& ostr, Window const& window)
{
  return ostr << "window_id="
              << std::hex << std::setw(8) << std::setfill('0') << std::showbase
              << window.id_
              << std::dec
              << " application=\"" << window.application_->name() << "\""
              << " monitor=" << window.monitor_
              << " title=\"" << window.title_ << "\"";
}


} // namespace Ginn

