# This file is part of Ginn, the general-purpose multi-touch gesture utility.
#
# Copyright 2013-2014 Canonical Ltd.
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

bin_PROGRAMS = ginn
noinst_LIBRARIES = libginn.a

libginn_a_SOURCES = \
	action.h                 action.cpp \
	actionbuilder.h          actionbuilder.cpp \
	actionsink.h             actionsink.cpp \
	activewishes.h           activewishes.cpp \
	application.h            application.cpp \
	applicationbuilder.h     applicationbuilder.cpp \
	applicationsource.h      applicationsource.cpp \
	bamfapplicationsource.h  bamfapplicationsource.cpp \
	configuration.h          configuration.cpp \
	geisgesturesource.h      geisgesturesource.cpp \
	gesturesource.h          gesturesource.cpp \
	ginn.h                   ginn.cpp \
	ginnconfig.h             ginnconfig.cpp \
	keymap.h                 keymap.cpp \
	window.h                 window.cpp \
	wish.h                   wish.cpp \
	wishbuilder.h            wishbuilder.cpp \
	wishsource.h             wishsource.cpp \
	wishsourceconfig.h       wishsourceconfig.cpp \
	x11actionsink.h          x11actionsink.cpp \
	x11keymap.h              x11keymap.cpp \
	xmlwishsource.h          xmlwishsource.cpp

libginn_a_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	$(BAMF_CFLAGS) \
	$(GEIS_CFLAGS) \
	$(GIO_CFLAGS) \
	$(XML2_CFLAGS) \
	$(XTEST_CFLAGS)

ginn_SOURCES = \
	main.cpp

ginn_CPPFLAGS = \
	-I$(top_srcdir) \
	$(GLIB2_0_CFLAGS)

ginn_LDADD = \
	libginn.a \
	$(BAMF_LIBS) \
	$(GEIS_LIBS) \
	$(GIO_LIBS) \
	$(GLIB2_0_LIBS) \
	$(XML2_LIBS) \
	$(XTEST_LIBS)

