/**
 * @file test/gtest_fake_application_source.cpp
 * @brief Unit tests of the fake action sink.
 * Makes sure the fake applicaton source works, otherwise you can't depend on
 * tests that use it.
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
#include "fakeapplicationsource.h"
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace Ginn;
using namespace testing;

using std::bind;


class MockObserver
{
public:
  ~MockObserver() {}
  MOCK_METHOD1(window_opened, void(Window const* window));
  MOCK_METHOD1(window_closed, void(Window const* window));
};

class FakeApplicationSourceTest
: public testing::Test
{
public:
  FakeApplicationSourceTest()
  {
    app_source_.set_initialized_callback(bind(&FakeApplicationSourceTest::app_source_initialized, this));
  }

  void
  app_source_initialized()
  { app_source_.report_windows(); }

protected:
  MockObserver          mock_app_observer_;
  FakeApplicationSource app_source_;
};


TEST_F(FakeApplicationSourceTest, addWindowBeforeInit)
{
  app_source_.set_window_opened_callback(bind(&MockObserver::window_opened,
                                              &mock_app_observer_,
                                              std::placeholders::_1));
  app_source_.add_application("id", "name", "generic_name");
  app_source_.add_window("id", 1000);

  EXPECT_CALL(mock_app_observer_, window_opened(_)).Times(1);
  app_source_.complete_initialization();
}


TEST_F(FakeApplicationSourceTest, addWindowAfterInit)
{
  EXPECT_CALL(mock_app_observer_, window_opened(_)).Times(1);

  app_source_.set_window_opened_callback(bind(&MockObserver::window_opened,
                                              &mock_app_observer_,
                                              std::placeholders::_1));
  app_source_.complete_initialization();
  app_source_.add_application("id", "name", "generic_name");
  app_source_.add_window("id", 1000);
}


TEST_F(FakeApplicationSourceTest, removeWindow)
{
  EXPECT_CALL(mock_app_observer_, window_closed(_)).Times(1);

  app_source_.add_application("id", "name", "generic_name");
  app_source_.add_window("id", 1000);
  app_source_.complete_initialization();
  app_source_.set_window_closed_callback(bind(&MockObserver::window_closed,
                                              &mock_app_observer_,
                                              std::placeholders::_1));
  app_source_.remove_window(1000);
}


TEST_F(FakeApplicationSourceTest, removeApplication)
{
  EXPECT_CALL(mock_app_observer_, window_closed(_)).Times(1);

  app_source_.add_application("id", "name", "generic_name");
  app_source_.add_window("id", 1000);
  app_source_.complete_initialization();
  app_source_.set_window_closed_callback(bind(&MockObserver::window_closed,
                                              &mock_app_observer_,
                                              std::placeholders::_1));
  app_source_.remove_application("id");
}


