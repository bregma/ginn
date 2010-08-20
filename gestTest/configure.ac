#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([utouch-gesturetest], 1.0.4)
AM_INIT_AUTOMAKE([foreign dist-bzip2])

AM_PATH_PYTHON([2.5])

AC_PROG_LIBTOOL
AC_PROG_CC

AC_PATH_PROG(XSLTPROC, xsltproc, no)
if test "$XSLTPROC" = "no"; then
   AC_MSG_ERROR([XCB requires xsltproc.])
fi

# Checks for pkg-config packages
PKG_CHECK_MODULES(XCBPROTO, xcb-proto >= 1.6)
NEEDED="xcb xcb-icccm"
PKG_CHECK_MODULES(NEEDED, $NEEDED)

AC_SUBST(NEEDED)

# Find the xcb-proto protocol descriptions
AC_MSG_CHECKING(XCBPROTO_XCBINCLUDEDIR)
XCBPROTO_XCBINCLUDEDIR=`$PKG_CONFIG --variable=xcbincludedir xcb-proto`
AC_MSG_RESULT($XCBPROTO_XCBINCLUDEDIR)
AC_SUBST(XCBPROTO_XCBINCLUDEDIR)

AC_MSG_CHECKING(XCBINCLUDEDIR)
XCBINCLUDEDIR=`$PKG_CONFIG --variable=includedir xcb`
AC_MSG_RESULT($XCBINCLUDEDIR)
AC_SUBST(XCBINCLUDEDIR)


# Find the xcb-proto version
XCBPROTO_VERSION=`$PKG_CONFIG --modversion xcb-proto`
AC_SUBST(XCBPROTO_VERSION)

# Find the xcbgen Python package
AC_MSG_CHECKING(XCBPROTO_XCBPYTHONDIR)
XCBPROTO_XCBPYTHONDIR=`$PKG_CONFIG --variable=pythondir xcb-proto`
AC_MSG_RESULT($XCBPROTO_XCBPYTHONDIR)
AC_SUBST(XCBPROTO_XCBPYTHONDIR)

AC_HEADER_STDC

xcbincludedir='${includedir}/xcb'
AC_SUBST(xcbincludedir)

if  test "x$GCC" = xyes ; then
    CWARNFLAGS="-Wall -pedantic -Wpointer-arith \
		-Wstrict-prototypes -Wmissing-declarations -Wnested-externs"
else
    AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"], [SUNCC="no"])
    if test "x$SUNCC" = "xyes"; then
	CWARNFLAGS="-v"
    fi
fi
AC_SUBST(CWARNFLAGS)

XCB_CHECK_VISIBILITY()

case $host_os in
	# darwin has poll() but can't be used to poll character devices (atleast through SnowLeopard)
	darwin*) ;;
	*)
		AC_CHECK_FUNC(poll, [AC_DEFINE(USE_POLL, 1, [poll() function is available])], )
	;;
esac

AC_CONFIG_FILES([
Makefile
src/Makefile
src/gesture.xml
])

AC_OUTPUT

dnl Configuration output

echo ""
echo "  Package: ${PACKAGE_NAME} ${PACKAGE_VERSION}"
echo ""
echo "  Used CFLAGS:"
echo "    CPPFLAGS............: ${CPPFLAGS}"
echo "    CFLAGS..............: ${CFLAGS}"
echo "    Warning CFLAGS......: ${CWARNFLAGS}"
echo ""
echo "  Installation:"
echo "    Prefix..............: ${prefix}"
echo ""
