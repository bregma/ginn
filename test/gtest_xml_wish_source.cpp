/**
 * @file test/gtest_xml_wish_source.cpp
 * @brief Unit tests of teh XML wish source module.
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
#include "ginn/configuration.h"
#include "ginn/keymap.h"
#include "ginn/wishsource.h"
#include <gtest/gtest.h>

class TestXMLWishSource
: public testing::Test
{
public:
  TestXMLWishSource()
  : config_(0, nullptr)
  , keymap_(config_)
  { }

  virtual void
  SetUp()
  {
    source_ = Ginn::WishSource::factory(Ginn::WishSource::Format::XML, config_);
    ASSERT_NE(source_, nullptr);
  };

protected:
  Ginn::Configuration   config_;
  Ginn::WishSource::Ptr source_;
  Ginn::Keymap          keymap_;
};


TEST_F(TestXMLWishSource, invalidXML)
{
  Ginn::WishSource::RawSourceList raws = {
    { "invalid xml", "invalid xml" }
  };

  Ginn::Wish::Table table = source_->get_wishes(raws, &keymap_);
  ASSERT_TRUE(table.size() == 0);
}


TEST_F(TestXMLWishSource, basic)
{
  Ginn::WishSource::RawSourceList raws = {
    { "basic",
      "<ginn>"
        "<applications>"
          "<application name=\"dummy\">"
            "<wish gesture=\"Pinch\" fingers=\"2\">"
              "<action name=\"stud\" when=\"update\">"
                "<trigger prop=\"radius delta\" min=\"20\" max=\"80\"/>"
                "<key modifier1=\"Control_L\">Up</key>"
              "</action>"
            "</wish>"
          "</application>"
        "</applications>"
      "</ginn>" }
  };

  Ginn::Wish::Table table = source_->get_wishes(raws, &keymap_);
  ASSERT_TRUE(table.size() == 1);
}



