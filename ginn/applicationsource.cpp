/**
 * @file applicationsource.cpp
 * @brief DEfinition of the Ginn ApplicationSource interface class.
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
#include "ginn/applicationsource.h"

#include "ginn/bamfapplicationsource.h"
#include "ginn/configuration.h"


namespace Ginn
{

ApplicationSource::
~ApplicationSource()
{ }


ApplicationSource::Ptr ApplicationSource::
factory(Type type, Configuration const& config)
{
  Ptr source;
  if (type == Type::BAMF)
    source.reset(new BamfApplicationSource(config));
  return source;
}


} // namespace Ginn


