/**
 * @file ginn/keymap.cpp
 * @brief Implementation of the the Ginn keymap module.
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
#include "ginn/x11keymap.h"

#include "ginn/configuration.h"
#include <glib.h>
#include <iostream>
#include "keymap.h"
#include <map>
#include <stdexcept>


namespace Ginn
{

struct X11Keymap::Impl
{
  Impl(Configuration const& config)
  : config_(config)
  , pid_(-1)
  , out_fd_(-1)
  , err_fd_(-1)
  { }
  
  Configuration                  config_;
  InitializedCallback            initialized_callback_;
  GPid                           pid_;
  gint                           out_fd_;
  gint                           err_fd_;
  std::map<std::string, Keycode> keymap_;
};


static gboolean
cb_out_watch(GIOChannel* channel, GIOCondition cond, gpointer data)
{
  gchar *line_in;
  gsize  size;
  X11Keymap::Impl* impl = reinterpret_cast<X11Keymap::Impl*>(data);

  if (cond == G_IO_HUP)
  {
    g_io_channel_unref(channel);
    impl->initialized_callback_();
    return(FALSE);
  }

  g_io_channel_read_line(channel, &line_in, &size, NULL, NULL);
  std::string s = line_in;
  g_free(line_in);

  std::string::size_type blank = s.find(' ');
  if (blank != std::string::npos)
  {
    uint8_t keycode = static_cast<uint8_t>(std::stoi(s.substr(blank+1)));

    std::string::size_type eq = s.find('=', blank+1);
    std::string::size_type start = s.find_first_not_of("= \t\n", eq);
    while (start != std::string::npos)
    {
      std::string::size_type end = s.find_first_of(" \t\n", start);
      std::string keysym = s.substr(start, end-start);
      if (keysym == "ancel")
      {
        std::cerr << "==smw> keysym='" << keysym << "' s='" << s << "'\n";
      }
      if (keysym != "NoSymbol")
      {
        impl->keymap_[keysym] = keycode;
      }
      start = s.find_first_not_of(" \t\n", end);
    }
  }

  return(TRUE);
}

static gboolean
cb_err_watch(GIOChannel* channel, GIOCondition cond, gpointer)
{
  gchar *line_in;
  gsize  size;

  if (cond == G_IO_HUP)
  {
      g_io_channel_unref(channel);
      return( FALSE );
  }

  g_io_channel_read_line(channel, &line_in, &size, NULL, NULL);
  std::cerr << __PRETTY_FUNCTION__ << ": '" << line_in << "'\n";
  g_free(line_in);

  return(TRUE);
}


X11Keymap::
X11Keymap(Configuration const& config)
: impl_(new Impl(config))
{
  char const* argv[] = { "/usr/bin/xmodmap", "xmodmap", "-pke", NULL };
  gboolean bstat = g_spawn_async_with_pipes(NULL,
                                            (gchar**)argv,
                                            NULL,
                                            G_SPAWN_FILE_AND_ARGV_ZERO,
                                            NULL,
                                            NULL,
                                            &impl_->pid_,
                                            NULL,
                                            &impl_->out_fd_,
                                            &impl_->err_fd_,
                                            NULL);
  if (!bstat)
  {
    throw std::runtime_error("spawn failed");
  }

  GIOChannel* out_ch = g_io_channel_unix_new(impl_->out_fd_);
  GIOChannel* err_ch = g_io_channel_unix_new(impl_->err_fd_);
  g_io_add_watch(out_ch, GIOCondition(G_IO_IN | G_IO_HUP), cb_out_watch, impl_.get());
  g_io_add_watch(err_ch, GIOCondition(G_IO_IN | G_IO_HUP), (GIOFunc)cb_err_watch, impl_.get());

  if (impl_->config_.is_verbose_mode())
    std::cout << __FUNCTION__ << " created\n";
}


X11Keymap::
~X11Keymap()
{
}


void X11Keymap::
set_initialized_callback(InitializedCallback const& initialized_callback)
{
  impl_->initialized_callback_ = initialized_callback;
}


Keymap::Keycode X11Keymap::
to_keycode(std::string const& keysym_name) const
{
  auto it = impl_->keymap_.find(keysym_name);
  if (it != impl_->keymap_.end())
  {
    return it->second;
  }
  return 0;
}

} // namespace Ginn

