/**
 * @file wishsource.cpp
 * @brief DEfinition of the Ginn WishSource interface class.
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
#include "ginn/wishsource.h"

#include "ginn/xmlwishsource.h"


namespace Ginn
{


WishSource::
~WishSource()
{ }


WishSource::Ptr WishSource::
wish_source_factory(Format format, std::string const& schema_file_name)
{
  Ptr source;
  if (format == Format::XML)
    source.reset(new XmlWishSource(schema_file_name));
  return source;
}


} // namespace Ginn


