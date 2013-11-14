/**
 * @file wishloader.cpp
 * @brief DEfinition of the Ginn WishLoader interface class.
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
#include "ginn/wishloader.h"

#include "ginn/xmlwishloader.h"


namespace Ginn
{


WishLoader::
~WishLoader()
{ }


WishLoader::Ptr WishLoader::
wish_loader_factory(WishFileFormat format, std::string const& schema_file_name)
{
  Ptr loader;
  if (format == WishFileFormat::XML)
    loader.reset(new XmlWishLoader(schema_file_name));
  return loader;
}


} // namespace Ginn


