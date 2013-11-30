/**
 * @file ginn/bamfapplicationsource.cpp
 * @brief Definitions of the Ginn BAMF Application Source class.
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
#include "ginn/bamfapplicationsource.h"

#include "ginn/application.h"
#include "ginn/applicationbuilder.h"
#include <gio/gdesktopappinfo.h>
#include <glib.h>
#include <libbamf/bamf-matcher.h>


typedef std::unique_ptr<BamfMatcher, void(*)(gpointer)> bamf_matcher_t;


namespace Ginn
{

static Window
build_window(BamfMatcher* matcher, BamfWindow* bamf_window)
{
  char const* title = bamf_view_get_name(BAMF_VIEW(bamf_window));

  const gchar* application_id = nullptr;
  BamfApplication* bamf_app = bamf_matcher_get_application_for_window(matcher,
                                                                      bamf_window);
  if (bamf_app)
    application_id = bamf_application_get_desktop_file(bamf_app);

  return Window{ bamf_window_get_xid(bamf_window),
                 (title?title:"???"),
                 application_id ? application_id : "(none)",
                 (bool)bamf_view_is_active(BAMF_VIEW(bamf_window)),
                 (bool)bamf_view_is_user_visible(BAMF_VIEW(bamf_window)),
                 bamf_window_get_monitor(bamf_window) };
}


struct BamfApplicationBuilder
: public ApplicationBuilder
{
  BamfApplicationBuilder(BamfMatcher* matcher, BamfApplication* app)
  : matcher_(matcher)
  , app_(app)
  { }

  ~BamfApplicationBuilder()
  { }

  Application::Id
  application_id() const
  {
    const gchar* desktop_file = bamf_application_get_desktop_file(app_);
    return desktop_file ? desktop_file : "";
  }

  std::string
  name() const
  {
    char const* name = bamf_view_get_name(BAMF_VIEW(app_));
    return name ? name : "";
  }

  std::string
  generic_name() const
  {
    GDesktopAppInfo* app_info = g_desktop_app_info_new_from_filename(application_id().c_str());
    if (app_info)
    {
      if (g_desktop_app_info_has_key(app_info,
                                     G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME))
      {
        gchar const* generic_name = g_desktop_app_info_get_string(app_info,
                                        G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME);
        return generic_name ? generic_name : "";
      }
    }
    return "";
  }

  Application::Windows
  windows() const
  {
    Application::Windows windows;

    GList* window_list = bamf_application_get_windows(app_);
    for (GList* window = window_list; window; window = window->next)
    {
      if (BAMF_IS_WINDOW(window->data))
      {
        auto bamf_window = static_cast<BamfWindow*>(window->data);
        windows.push_back(std::move(build_window(matcher_, bamf_window)));
      }
    }

    return windows;
  }

  BamfMatcher*     matcher_;
  BamfApplication* app_;
};


void
on_view_opened(BamfMatcher* matcher, BamfView* view, gpointer data)
{
  ApplicationObserver* observer = static_cast<ApplicationObserver*>(data);
  if (BAMF_IS_APPLICATION(view))
  {
    BamfApplication* bamf_app = BAMF_APPLICATION(view);
    auto a = std::make_shared<Application>(BamfApplicationBuilder(matcher,
                                                                  bamf_app));
    observer->application_added(a);
  }
  else if (BAMF_IS_WINDOW(view))
  {
    BamfWindow* bamf_window = BAMF_WINDOW(view);
    observer->window_added(build_window(matcher, bamf_window));
  }
}

void
on_view_closed(BamfMatcher* matcher, BamfView* view, gpointer data)
{
  ApplicationObserver* observer = static_cast<ApplicationObserver*>(data);
  if (BAMF_IS_APPLICATION(view))
  {
    BamfApplication* bamf_app = BAMF_APPLICATION(view);
    auto a = std::make_shared<Application>(BamfApplicationBuilder(matcher,
                                                                  bamf_app));
    observer->application_removed(a->application_id());
  }
  else if (BAMF_IS_WINDOW(view))
  {
    BamfWindow* bamf_window = BAMF_WINDOW(view);
    Window::Id window_id = bamf_window_get_xid(bamf_window);
    observer->window_removed(window_id);
  }
}

struct BamfApplicationSource::Impl
{
  Impl(ApplicationObserver* observer)
  : matcher_(bamf_matcher_get_default(), g_object_unref)
  , observer_(observer)
  {
    g_signal_connect(G_OBJECT(matcher_.get()), "view-opened", (GCallback)on_view_opened, observer_);
    g_signal_connect(G_OBJECT(matcher_.get()), "view_closed", (GCallback)on_view_closed, observer_);
  }

  ~Impl()
  { }

  bamf_matcher_t       matcher_;
  ApplicationObserver* observer_;
};


BamfApplicationSource::
BamfApplicationSource(ApplicationObserver* observer)
: impl_(new Impl(observer))
{
}


BamfApplicationSource::
~BamfApplicationSource()
{
}


Application::List BamfApplicationSource::
get_applications()
{
  Application::List apps;
  GList* app_list = bamf_matcher_get_running_applications(impl_->matcher_.get());
  for (GList* app = app_list; app; app = app->next)
  {
    if (!BAMF_IS_APPLICATION(app->data))
      continue;

    auto bamf_app = static_cast<BamfApplication*>(app->data);
    auto a = std::make_shared<Application>(BamfApplicationBuilder(impl_->matcher_.get(),
                                                                  bamf_app));
    apps[a->name()] = a;
  }
  return apps;
}


} // namespace Ginn

