/**
 * @file test/gtest_active_wishes.cpp
 * @brief Unit tests of the active wishes module.
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
#include "fakeapplicationsource.h"
#include "fakegesturesource.h"
#include "fakekeymap.h"
#include <functional>
#include "ginn/activewishes.h"
#include "ginn/configuration.h"
#include "ginn/wish.h"
#include "ginn/wishsource.h"
#include <gtest/gtest.h>
#include "test/environment.h"

using namespace Ginn;
using Ginn::Test::Environment;
using std::placeholders::_1;
using std::placeholders::_2;


static WishSource::RawSourceList one_wish_app = {
  { "one_wish_app",
      "<ginn>"
        "<applications>"
          "<application name=\"test-app-id\">"
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


class ActiveWishesTest
: public testing::Test
{
public:
  ActiveWishesTest()
  : wish_source_(WishSource::factory(WishSource::Format::XML,
                                     Environment::config()))
  { }

  void
  SetUp()
  { callback_count_ = 0; }

  void
  callback_counter(Wish const&, Window const&)
  { ++callback_count_; }

protected:
  WishSource::Ptr        wish_source_;
  FakeKeymap             fake_keymap_;
  FakeApplicationSource  app_source_;
  FakeGestureSource      gesture_source_;
  Wish::Table            wish_table_;
  int                    callback_count_;
};


TEST_F(ActiveWishesTest, empty_wishes)
{
  WishSource::RawSourceList raws = {
    { "empty_wishes", "<ginn></ginn>" }
  };
  wish_table_ = wish_source_->get_wishes(raws, &fake_keymap_);

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_granted_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  EXPECT_EQ(callback_count_, 0);
}


TEST_F(ActiveWishesTest, empty_applications)
{
  wish_table_ = wish_source_->get_wishes(one_wish_app, &fake_keymap_);

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_granted_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  EXPECT_EQ(callback_count_, 0);
}


TEST_F(ActiveWishesTest, single_match_before_init)
{
  wish_table_ = wish_source_->get_wishes(one_wish_app, &fake_keymap_);

  app_source_.add_application("test-app-id", "app-name", "generic_name");
  app_source_.add_window("test-app-id", 1000);

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_granted_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  EXPECT_EQ(callback_count_, 1);
}


TEST_F(ActiveWishesTest, multiple_match_before_init)
{
  wish_table_ = wish_source_->get_wishes(one_wish_app, &fake_keymap_);

  app_source_.add_application("test-app-id", "app-name", "dummy");
  app_source_.add_window("test-app-id", 0x1001);
  app_source_.add_window("test-app-id", 0x1002);

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_granted_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  EXPECT_EQ(callback_count_, 2);
}


TEST_F(ActiveWishesTest, single_match_after_init)
{
  wish_table_ = wish_source_->get_wishes(one_wish_app, &fake_keymap_);

  app_source_.add_application("test-app-id", "app-name", "dummy");

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_granted_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  app_source_.add_window("test-app-id", 0x1001);

  EXPECT_EQ(callback_count_, 1);
}


TEST_F(ActiveWishesTest, remove_window)
{
  wish_table_ = wish_source_->get_wishes(one_wish_app, &fake_keymap_);
  app_source_.add_application("test-app-id", "dummy", "dummy");
  app_source_.add_window("test-app-id", 0x1001);
  app_source_.add_window("test-app-id", 0x1002);

  ActiveWishes active_wishes(Environment::config(), wish_table_, &app_source_, &gesture_source_);
  active_wishes.set_wish_revoked_callback(std::bind(&ActiveWishesTest::callback_counter, this, _1, _2));
  app_source_.complete_initialization();

  app_source_.remove_window(0x1001);

  EXPECT_EQ(callback_count_, 1);
}



