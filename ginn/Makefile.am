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

bin_PROGRAMS = ginn
noinst_LIBRARIES = libginn.a

libginn_a_SOURCES = \
	action.h                 action.cpp \
	actionsink.h             actionsink.cpp \
	application.h            application.cpp \
	applicationbuilder.h     applicationbuilder.cpp \
	applicationsource.h      applicationsource.cpp \
	applicationobserver.h    \
	bamfapplicationsource.h  bamfapplicationsource.cpp \
	configuration.h          configuration.cpp \
	geis.h                   geis.cpp \
	geisobserver.h           \
	gesturewatch.h           gesturewatch.cpp \
	wish.h                   wish.cpp \
	wishbuilder.h            wishbuilder.cpp \
	wishsource.h             wishsource.cpp \
	xmlwishsource.h          xmlwishsource.cpp

libginn_a_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	$(BAMF_CFLAGS) \
	$(GEIS_CFLAGS) \
	$(GIO_CFLAGS) \
	$(XML2_CFLAGS)

ginn_SOURCES = \
	ginn.h ginn.cpp \
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
	$(XML2_LIBS)

