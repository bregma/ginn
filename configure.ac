# This file is part of Ginn, the general-purpose multi-touch gesture utility.
# Copyright 2013 Canonical Ltd.
# 
# Ginn is free software: you can redistribute it and/or modify it under the terms
# of the GNU General Public License version 3, as published by the Free
# Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranties of MERCHANTABILITY, SATISFACTORY
# QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.60])
AC_INIT([Ginn],
        [1.0.0],
        [],
        [ginn])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_COPYRIGHT([Copyright 2013 Canonical Ltd.])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for external dependencies.
PKG_CHECK_MODULES([GLIB2_0], [glib-2.0])
PKG_CHECK_MODULES([GEIS],    [libgeis >= 1.0.10])
PKG_CHECK_MODULES([GIO],     [gio-unix-2.0])
PKG_CHECK_MODULES([XML2],    [libxml-2.0 >= 2.7.7])
PKG_CHECK_MODULES([XTEST],   [xcb-xtest >= 1.9.0])
PKG_CHECK_MODULES([BAMF],    [libbamf3 >= 0.2.53])

AC_ARG_ENABLE([tests],
              [AC_HELP_STRING([--enable-tests=@<:@no/yes@:>@],
                              [Enable unit tests @<:@default=yes@:>@])],
              [],
              [enable_tests=yes])
AS_IF([test "x$enable_tests" = "xyes"],[CHECK_GTEST])
AM_CONDITIONAL(BUILD_TESTS, [test "x$enable_tests" = "xyes"])

AC_SUBST(AM_CXXFLAGS, "-Wall -Wextra -pedantic -std=c++11")

AC_CONFIG_FILES([Makefile
                 ginn/Makefile
                 etc/Makefile
                 test/Makefile
                 doc/Makefile
                 ginn.pc])
AC_OUTPUT
