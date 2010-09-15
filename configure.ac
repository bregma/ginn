# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([Ginn],
        [1.0.0],
        [],
        [ginn])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config-aux])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

LIB_VERSION=1:0:0
AC_SUBST([LIB_VERSION])

# Initialize libtool
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

PKG_CHECK_MODULES([GEIS], [libutouch-geis >= 1.0.10])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 ginn.pc])
AC_OUTPUT
