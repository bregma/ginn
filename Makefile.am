AM_MAKEFLAGS = --no-print-directory

SUBDIRS = src etc test doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ginn.pc

INCLUDES = $(top_srcdir)/include/

dist-hook:
	@test -d "$(top_srcdir)/.bzr" && \
	  $(top_srcdir)/config-aux/missing --run \
	    bzr log --gnu-changelog -n 1 $(top_srcdir) > ChangeLog \
	    || true

