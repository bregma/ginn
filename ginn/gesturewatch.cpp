/**
 * @file ginn/gesturewatch.cpp
 * @brief Declarations of the Ginn GestureWatch class.
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
#include "ginn/gesturewatch.h"

#include "ginn/geis.h"
#include <iomanip>
#include <iostream>


namespace Ginn
{

GestureWatch::
GestureWatch(Window::Id        window_id,
             Application::Ptr  app,
             Wish::Ptr         wish,
             Geis&             geis)
: window_id_(window_id)
, application_(app)
, wish_(wish)
, subscription_(geis.subscribe(window_id, wish))
{
}


GestureWatch::
~GestureWatch()
{
}


static bool
frame_matches_window_id(GeisFrame frame, Window::Id window_id)
{
  GeisAttr attr = geis_frame_attr_by_name(frame,
                                          GEIS_GESTURE_ATTRIBUTE_EVENT_WINDOW_ID);
  if (attr)
  {
    Window::Id id = geis_attr_value_to_integer(attr);
    return id == window_id;
  }
  return false;
}


bool GestureWatch::
matches(GeisEvent event, ActionSink::Ptr const& action_sink)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet groupset = (GeisGroupSet)geis_attr_value_to_pointer(attr);
  for (GeisSize i= 0; i < geis_groupset_group_count(groupset); ++i)
  {
    GeisGroup group = geis_groupset_group(groupset, i);
    for (GeisSize j=0; j < geis_group_frame_count(group); ++j)
    {
      GeisFrame frame = geis_group_frame(group, j);
      if (frame_matches_window_id(frame, window_id_))
      {
        GeisAttr attr = geis_frame_attr_by_name(frame, wish_->property().c_str());
        if (attr)
        {
            float fval = geis_attr_value_to_float(attr);
            std::cerr << "gesture attr '" << wish_->property() << "' "
                      << "value " << fval << " "
                      << " (min=" << wish_->min() << ", max=" << wish_->max() << ")";
            if (wish_->min() <= fval && fval <= wish_->max())
            {
              action_sink->perform(wish_->action());
              return true;
            }
            std::cerr << "\n";
            return false;
        }
      }
    }
  }
  return false;
}


std::ostream&
operator<<(std::ostream& ostr, GestureWatch const& watch)
{
  Window const* const w = watch.application_->window(watch.window_id_);
  return ostr << "app '" << watch.application_->name()
              << "' window "
              << std::hex << std::setw(8) << std::setfill('0') << std::showbase
              << watch.window_id_ << std::dec << " '" << w->title << "'";
}


} // namespace Ginn


