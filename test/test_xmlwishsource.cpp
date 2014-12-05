/**
 * @file test/test_xmlwishsource.cpp
 * @brief Unit tests of teh XML wish source module.
 */

/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include "fakekeymap.h"
#include "ginn/wishsource.h"
#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include "test/mockwishsourceconfig.h"


using namespace ::testing;


class MockKeymap
: public Ginn::FakeKeymap
{
public:
  MOCK_CONST_METHOD1(to_keycode, Keymap::Keycode(std::string const& keysym_name));
};


class TestXMLWishSource
: public testing::Test
{
public:
  TestXMLWishSource()
  : keymap_()
  {
    EXPECT_CALL(config_, wish_source_format()).WillOnce(Return(Ginn::WishSourceConfig::Format::XML));
    EXPECT_CALL(config_, wish_schema_file_name()).WillOnce(ReturnRef(Ginn::WishSourceConfig::WISH_NO_VALIDATE));
  }

  virtual void
  SetUp()
  {
    source_ = Ginn::WishSource::factory(&config_);
    ASSERT_NE(source_, nullptr);
  };

protected:
  NiceMock<MockWishSourceConfig>  config_;
  Ginn::WishSource::Ptr           source_;
  MockKeymap                      keymap_;
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

  EXPECT_CALL(keymap_, to_keycode(_)).Times(AtLeast(2));
  Ginn::Wish::Table table = source_->get_wishes(raws, &keymap_);
  ASSERT_TRUE(table.size() == 1);
}



