# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([Ginn],
        [1.0.0],
        [],
        [ginn])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config-aux])
AC_CONFIG_MACRO_DIR([m4])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for external dependencies.
PKG_CHECK_MODULES([GEIS], [libgeis >= 1.0.10])
PKG_CHECK_MODULES([X11],  [x11 >= 1.3.3])
PKG_CHECK_MODULES([XML2], [libxml-2.0 >= 2.7.7])
PKG_CHECK_MODULES([XTST], [xtst >= 1.1.0])
PKG_CHECK_MODULES([BAMF], [libbamf3 >= 0.2.53])

AC_ARG_ENABLE([tests],
              [AC_HELP_STRING([--enable-tests=@<:@no/yes@:>@],
                              [Enable unit tests @<:@default=yes@:>@])],
              [],
              [enable_tests=yes])
AS_IF([test "x$enable_tests" = "xyes"],[CHECK_GTEST])
AM_CONDITIONAL(BUILD_TESTS, [test "x$enable_tests" = "xyes"])

AC_SUBST(AM_CXXFLAGS, "-Wall -Wextra -pedantic -std=c++11 -Weffc++")

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 ginn/Makefile
                 etc/Makefile
                 test/Makefile
                 doc/Makefile
                 ginn.pc])
AC_OUTPUT
