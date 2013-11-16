/**
 * @file ginn/xmlwishloader.h
 * @brief Declarations of the Ginn XML Wish Loader class.
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
#ifndef GINN_XMLWISHLOADER_H_
#define GINN_XMLWISHLOADER_H_

#include "ginn/wishloader.h"
#include <memory>
#include <string>


namespace Ginn
{

/**
 * A factory class to load wishes from an XML file.
 */
class XmlWishLoader
: public WishLoader
{
public:
  XmlWishLoader(std::string const& schema_file_name);
  ~XmlWishLoader();

  Wish::Table
  get_wishes(FileNameList const& wish_file_names);

private:
  struct Impl;

  std::unique_ptr<Impl> impl_;
};

}

#endif // GINN_XMLWISHLOADER_H_
