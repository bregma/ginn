/**
 * @file ginn/configuration.cpp
 * @brief Definitions of the Ginn Configuration module.
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
#include "config.h"
#include "ginn/configuration.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>


typedef std::vector<std::string> ConfigPath;

/**
 * Creates a list of directories following the XDG basedir spec (see
 * http://http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html)
 *
 * The Ginn wishes will be found in ginn/wishes.xml or *.xml in ginn/wishes.d in
 * one of these paths if it's not specified on the command line or in an
 * environment variable (or it's an error).
 */
ConfigPath
config_search_path()
{
  std::vector<std::string> directory_list;

  // Rule 1: the $XDG_CONFIG_HOME (default; $HOME/.config)
  // should be searched firs.
  char const* xdg_data_home = std::getenv("XDG_CONFIG_HOME");
  if (xdg_data_home && *xdg_data_home)
  {
    directory_list.push_back(xdg_data_home);
  }
  else
  {
    char const* home = std::getenv("HOME");
    if (home && *home)
    {
      directory_list.push_back(std::string(home) + "/.config");
    }
  }

  // Rule 2:  the $XDG_CONFIG_DIRS path, if any, should be search in order.
  // Default is '/etc/ginn/'.
  char const* xdg_config_dirs = std::getenv("XDG_CONFIG_DIRS");
  if (0 == xdg_config_dirs || 0 == *xdg_config_dirs)
  {
    xdg_config_dirs = PKGDATADIR "/xdg";
  }

  std::string path = xdg_config_dirs;
  std::string::size_type p = 0;
  std::string::size_type n = path.find(':');
  while (n != std::string::npos)
  {
    directory_list.push_back(path.substr(p, n-p));
    p = n+1, n = path.find(':', p);
  }
  directory_list.push_back(path.substr(p));

  std::reverse(std::begin(directory_list), std::end(directory_list));
  return directory_list;
}


/**
 * Creates an ordered list of wish definition files.
 */
static Ginn::FileNameList
find_wish_source_files(std::string const& arg_wish_file_name,
                       ConfigPath const&  path)
{
  Ginn::FileNameList file_list;

  if (!arg_wish_file_name.empty())
  {
    file_list.push_back(arg_wish_file_name);
    return file_list;
  }

  for (auto const& d: path)
  {
    std::string refdir= d + "/" PACKAGE;
    DIR* dir = opendir(refdir.c_str());
    if (dir)
    {
      for (struct dirent* de = readdir(dir); de; de = readdir(dir))
      {
        if (0 == std::strcmp(de->d_name, "wishes.xml"))
        {
          std::string file_name = refdir + "/" + de->d_name;
          file_list.push_back(file_name);
        }
      }

      rewinddir(dir);
      for (struct dirent* de = readdir(dir); de; de = readdir(dir))
      {
        if (0 == std::strcmp(de->d_name, "wishes.d"))
        {
          std::string dir = refdir + "/" + de->d_name;
          struct stat f_stat;
          int sstat = stat(dir.c_str(), &f_stat);
          if (0 == sstat && S_ISDIR(f_stat.st_mode))
          {
            DIR* sdir = opendir(dir.c_str());
            if (sdir)
            {
              for (struct dirent* sde = readdir(sdir); sde; sde = readdir(sdir))
              {
                std::string file_name = sde->d_name;
                if (file_name.rfind(".xml") == file_name.length() - 4)
                {
                  file_name = dir + "/" + file_name;
                  file_list.push_back(file_name);
                }
              }
              closedir(sdir);
            }
          }
        }
      }
      closedir(dir);
    }
  }

  return file_list;
}


/**
 * Creates an ordered list of wish definition schema files.
 */
std::string
find_wish_schema_file(std::string const& arg_wish_schema_file_name,
                      ConfigPath const&  path)
{
  if (!arg_wish_schema_file_name.empty())
  {
    return arg_wish_schema_file_name;
  }

  for (auto const& d: path)
  {
    std::string refdir= d + "/" PACKAGE;
    DIR* dir = opendir(refdir.c_str());
    if (dir)
    {
      for (struct dirent* de = readdir(dir); de; de = readdir(dir))
      {
        if (0 == std::strcmp(de->d_name, "ginn.rng"))
        {
          return refdir + "/" + de->d_name;
        }
      }
    }
  }

  return "";
}


namespace Ginn
{

struct Configuration::Impl
{
  Impl();

  bool         is_verbose_mode;
  ConfigPath   config_path;
  std::string  wish_schema_file_name;
  FileNameList wish_file_names;
};


Configuration::Impl::
Impl()
: is_verbose_mode(false)
, config_path(config_search_path())
{
}


static void
print_help()
{
  std::cerr
  << "usage:\n"
  << "  ginn [OPTION...] - convert multi-touch input events to key/mouse events\n"
  << "\n"
  << "Applicationo Options:\n"
  << "  -h, --help                       Print this message and exit.\n"
  << "  -n, --novalidate                 Do not validate the wishes file.\n"
  << "  -v, --verbose                    Keep a running commentary on stdout.\n"
  << "  -f, --wishes-file=FILE           Name the (single) wish file to load.\n"
  << "  -s, --wishes-schema-file=FILE    Name the wish schema file to load.\n"
  << "\n";
}


/**
 * Determines the current run-time Ginn configuration.
 * @param[in] argc  Count of the number of command-line arguments.
 * @param[in] argv  Vector of command-line arguments.
 */
Configuration::
Configuration(int argc, char* argv[])
: impl_(new Impl)
{
  std::string arg_wish_file_name;
  char const* env_wish_file = std::getenv("GINN_WISH_FILE");
  if (env_wish_file && env_wish_file[0])
    arg_wish_file_name = env_wish_file;

  std::string arg_wish_schema_file_name;

  while (1)
  {
    int option_index = 0;
    static struct option long_options[] = {
      { "help",                no_argument,       NULL, 'h' },
      { "novalidate",          no_argument,       NULL, 'n' },
      { "wishes-schema-file",  required_argument, NULL, 's' },
      { "verbose",             no_argument,       NULL, 'v' },
      { "wishes-file",         required_argument, NULL, 'f' },
      { 0,                     no_argument,       NULL,  0  }
    };

    int c = getopt_long(argc, argv, "f:hr:v", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 'f':
        arg_wish_file_name = optarg;
        break;
      case 's':
        arg_wish_schema_file_name = optarg;
        break;
      case 'v':
        impl_->is_verbose_mode = true;
        break;
      case 'h':
      case '?':
        print_help();
        exit(-1);
        break;
      default:
        break;
    }
  }

  impl_->wish_file_names = find_wish_source_files(arg_wish_file_name,
                                                  impl_->config_path);
  impl_->wish_schema_file_name = find_wish_schema_file(arg_wish_schema_file_name,
                                                       impl_->config_path);

}


/**
 * Destroys the Ginn runtime configuration object.
 */
Configuration::
~Configuration()
{ }


bool Configuration::
is_verbose_mode() const
{
  return impl_->is_verbose_mode;
}


FileNameList const& Configuration::
wish_file_names() const
{
  return impl_->wish_file_names;
}


std::string const& Configuration::
wish_schema_file_name() const
{
  return impl_->wish_schema_file_name;
}

} // namespace Ginn


