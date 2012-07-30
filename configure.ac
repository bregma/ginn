# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([Ginn],
        [0.2.6],
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

PKG_CHECK_MODULES([GEIS], [libgeis >= 1.0.10])
PKG_CHECK_MODULES([X11],  [x11 >= 1.3.3])
PKG_CHECK_MODULES([XML2], [libxml-2.0 >= 2.7.7])
PKG_CHECK_MODULES([XTST], [xtst >= 1.1.0])
PKG_CHECK_MODULES([BAMF], [libbamf >= 0.2.53])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 etc/Makefile
                 ginn.pc])
AC_OUTPUT
