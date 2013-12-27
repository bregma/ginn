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

#include <fstream>
#include "ginn/configuration.h"
#include "ginn/xmlwishsource.h"
#include <iostream>
#include <utility>


namespace Ginn
{


WishSource::
~WishSource()
{ }


/**
 * Gets the identified wish source.
 * @param[in] format           Identifies te format of the wish source.
 * @param[in] schema_file_name Names the wish schema file.
 *
 * @returns the identified wish source object.
 */
WishSource::Ptr WishSource::
factory(Configuration const& configuration)
{
  Format format = configuration.wish_source_format();
  Ptr source;
  if (format == Format::XML)
    source.reset(new XmlWishSource(configuration));
  return source;
}


/**
 * Reads the raw sources from the named files into in-memory buffers.
 * @param[in] wish_file_names a collection of wish files to read
 *
 * @returns a collection of loaded raw wish sources.
 */
WishSource::RawSourceList WishSource::
read_raw_sources(NameList const& wish_file_names)
{
  RawSourceList raw_source_list;
  for (auto const& file_name: wish_file_names)
  {
    std::ifstream ifs(file_name);
    if (ifs)
    {
      std::string contents;
      ifs.seekg(0, std::ios::end);
      contents.resize(ifs.tellg());
      ifs.seekg(0, std::ios::beg);
      ifs.read(&contents[0], contents.size());
      raw_source_list.push_back({file_name, contents});
    }
  }

  return raw_source_list;
}

} // namespace Ginn


