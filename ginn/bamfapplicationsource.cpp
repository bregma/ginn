/**
 * @file ginn/bamfapplicationsource.cpp
 * @brief Definitions of the Ginn BAMF Application Source class.
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
#include "ginn/bamfapplicationsource.h"

#include <algorithm>
#include <cassert>
#include "ginn/application.h"
#include "ginn/applicationbuilder.h"
#include "ginn/configuration.h"
#include <gio/gdesktopappinfo.h>
#include <glib.h>
#include <iostream>
#include <libbamf/bamf-matcher.h>


using bamf_matcher_t = std::unique_ptr<BamfMatcher, void(*)(gpointer)>;


namespace Ginn
{

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
    std::string name = "";
    GDesktopAppInfo* app_info = g_desktop_app_info_new_from_filename(application_id().c_str());
    if (app_info)
    {
      if (g_desktop_app_info_has_key(app_info,
                                     G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME))
      {
        gchar* generic_name = g_desktop_app_info_get_string(app_info,
                                        G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME);
        name = generic_name ? generic_name : "";
        g_free(generic_name);
      }
      g_clear_object(&app_info);
    }
    return name;
  }

  BamfMatcher*     matcher_;
  BamfApplication* app_;
};


using AppPtr = std::unique_ptr<Application>;


struct BamfApplicationSource::Impl
{
  Impl(Configuration const& config);

  ~Impl()
  { }

  Application*
  get_application(BamfApplication* bamf_app);

  Application*
  add_application(BamfApplication* bamf_app);

  void
  remove_application(BamfApplication* bamf_app);

  void
  add_window(BamfWindow* bamf_window);

  void
  remove_window(BamfWindow* bamf_window);

  static gboolean
  do_initialization(gpointer data);

  Configuration         config_;
  bamf_matcher_t        matcher_;
  std::vector<AppPtr>   applications_;
  InitializedCallback   initialized_callback_;
  WindowOpenedCallback  window_opened_callback_;
  WindowClosedCallback  window_closed_callback_;
};


void
on_view_opened(BamfMatcher*, BamfView* view, gpointer data)
{
  BamfApplicationSource::Impl* impl = static_cast<BamfApplicationSource::Impl*>(data);
  if (BAMF_IS_APPLICATION(view))
  {
    if (!impl->get_application(BAMF_APPLICATION(view)))
      impl->add_application(BAMF_APPLICATION(view));
  }
  else if (BAMF_IS_WINDOW(view))
  {
    impl->add_window(BAMF_WINDOW(view));
  }
}


void
on_view_closed(BamfMatcher*, BamfView* view, gpointer data)
{
  BamfApplicationSource::Impl* impl = static_cast<BamfApplicationSource::Impl*>(data);
  if (BAMF_IS_APPLICATION(view))
  {
    impl->remove_application(BAMF_APPLICATION(view));
  }
  else if (BAMF_IS_WINDOW(view))
  {
    impl->remove_window(BAMF_WINDOW(view));
  }
}


BamfApplicationSource::Impl::
Impl(Configuration const& config)
: config_(config)
, matcher_(bamf_matcher_get_default(), g_object_unref)
{
  g_signal_connect(G_OBJECT(matcher_.get()),
                   "view-opened",
                   (GCallback)on_view_opened,
                   this);
  g_signal_connect(G_OBJECT(matcher_.get()),
                   "view_closed",
                   (GCallback)on_view_closed,
                   this);
  g_idle_add(do_initialization, this);
}


Application* BamfApplicationSource::Impl::
get_application(BamfApplication* bamf_app)
{
  Application* app = nullptr;
  const gchar* application_id = bamf_application_get_desktop_file(bamf_app);
  if (nullptr == application_id)
  {
    if (config_.is_verbose_mode())
      std::cerr << "warning: no desktop file for application\n";
  }
  else
  {
    auto it = std::find_if(std::begin(applications_),
                           std::end(applications_),
                           [&application_id](AppPtr const& app) -> bool
                             { return app->application_id() == application_id; });
    if (it != std::end(applications_))
      app = it->get();
  }
  return app;
}


Application* BamfApplicationSource::Impl::
add_application(BamfApplication* bamf_app)
{
  AppPtr a(new Application(BamfApplicationBuilder(matcher_.get(), bamf_app)));
  if (config_.is_verbose_mode())
    std::cout << __FUNCTION__ << ": " << *a;
  applications_.push_back(std::move(a));
  return applications_.back().get();
}


void BamfApplicationSource::Impl::
remove_application(BamfApplication* bamf_app)
{
  const gchar* application_id = bamf_application_get_desktop_file(bamf_app);
  auto it = std::find_if(std::begin(applications_),
                         std::end(applications_),
                         [&application_id](AppPtr const& app) -> bool
                           { return app->application_id() == application_id; });
  if (it != std::end(applications_))
  {
    if (config_.is_verbose_mode())
      std::cout << __FUNCTION__ << ": \"" << (*it)->name() << "\" exited\n";
    applications_.erase(it);
  }
}


void BamfApplicationSource::Impl::
add_window(BamfWindow* bamf_window)
{
  auto bamf_app = bamf_matcher_get_application_for_window(matcher_.get(), bamf_window);
  auto app = get_application(bamf_app);
  if (!app)
  {
    app = add_application(bamf_app);
  }

  char const* title = bamf_view_get_name(BAMF_VIEW(bamf_window));
  Window* w = new Window {bamf_window_get_xid(bamf_window),
                          (title?title:"???"),
                          app,
                          (bool)bamf_view_is_active(BAMF_VIEW(bamf_window)),
                          (bool)bamf_view_is_user_visible(BAMF_VIEW(bamf_window)),
                          bamf_window_get_monitor(bamf_window) };
  app->add_window(std::unique_ptr<Window>(w));

  if (window_opened_callback_)
    window_opened_callback_(w);
}


void BamfApplicationSource::Impl::
remove_window(BamfWindow* bamf_window)
{
  Window::Id window_id = bamf_window_get_xid(bamf_window);
  for (auto const& app: applications_)
  {
    Window const* w = app->window(window_id);
    if (w)
    {
      if (window_closed_callback_)
      {
        Window const* w = app->window(window_id);
        window_closed_callback_(w);
      }
      app->remove_window(window_id);
      break;
    }
  }
}


gboolean BamfApplicationSource::Impl::
do_initialization(gpointer data)
{
  BamfApplicationSource::Impl* impl = static_cast<BamfApplicationSource::Impl*>(data);
  GList* app_list = bamf_matcher_get_running_applications(impl->matcher_.get());
  for (GList* app = app_list; app; app = app->next)
  {
    if (!BAMF_IS_APPLICATION(app->data))
      continue;

    auto a = impl->get_application(BAMF_APPLICATION(app->data));
    if (!a)
      a = impl->add_application(BAMF_APPLICATION(app->data));

    GList* window_list = bamf_application_get_windows(BAMF_APPLICATION(app->data));
    for (GList* window = window_list; window; window = window->next)
    {
      if (BAMF_IS_WINDOW(window->data))
      {
        auto bamf_window = static_cast<BamfWindow*>(window->data);
        impl->add_window(bamf_window);
      }
    }
    g_list_free(window_list);
  }
  g_list_free(app_list);

  if (impl->initialized_callback_)
    impl->initialized_callback_();
  return false;
}


BamfApplicationSource::
BamfApplicationSource(Configuration const& config)
: impl_(new Impl(config))
{
  if (impl_->config_.is_verbose_mode())
    std::cout << __FUNCTION__ << " created\n";
}


BamfApplicationSource::
~BamfApplicationSource()
{ }


void BamfApplicationSource::
set_initialized_callback(InitializedCallback const& callback)
{
  impl_->initialized_callback_ = callback;
}


void BamfApplicationSource::
set_window_opened_callback(WindowOpenedCallback const& callback)
{
  impl_->window_opened_callback_ = callback;
}


void BamfApplicationSource::
set_window_closed_callback(WindowClosedCallback const& callback)
{
  impl_->window_closed_callback_ = callback;
}


void BamfApplicationSource::
report_windows()
{
  if (impl_->window_opened_callback_)
  {
    for (auto const& app: impl_->applications_)
    {
      app->for_all_windows([this](Window const* w)
          { impl_->window_opened_callback_(w); });
    }
  }
}

} // namespace Ginn

