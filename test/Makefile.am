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
if BUILD_TESTS

check_LIBRARIES = libgtest.a
check_PROGRAMS = gtest_ginn
TESTS = $(check_PROGRAMS)

nodist_libgtest_a_SOURCES = \
 $(GTEST_PREFIX)/src/gtest-all.cc

libgtest_a_CPPFLAGS = \
	$(GTEST_CPPFLAGS)

nodist_gtest_ginn_SOURCES = \
 $(GTEST_PREFIX)/src/gtest_main.cc

gtest_ginn_SOURCES = \
 gtest_xml_wish_source.cpp

gtest_ginn_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

gtest_ginn_LDADD = \
	$(top_builddir)/ginn/libginn.a \
	$(BAMF_LIBS) \
	$(GEIS_LIBS) \
	$(GIO_LIBS) \
	$(GLIB2_0_LIBS) \
	$(XML2_LIBS) \
	$(XTEST_LIBS) \
	libgtest.a \
	-lpthread

endif
