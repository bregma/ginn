# This file is part of Ginn, the general-purpose multi-touch gesture utility.
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

SUBDIRS = bamfapplicationsource

if BUILD_TESTS

check_LIBRARIES = libgmock.a
check_PROGRAMS = gmock_ginn
TESTS = $(check_PROGRAMS)

nodist_libgmock_a_SOURCES = \
  $(GMOCK_PREFIX)/src/gmock-all.cc \
  $(GTEST_PREFIX)/src/gtest-all.cc

libgmock_a_CPPFLAGS = \
  -Wno-missing-field-initializers \
  $(GMOCK_CPPFLAGS) \
  $(GTEST_CPPFLAGS)

nodist_gmock_ginn_SOURCES = \
  $(GMOCK_PREFIX)/src/gmock_main.cc

gmock_ginn_SOURCES = \
  fakeactionsink.h          fakeactionsink.cpp \
  fakeapplicationsource.h   fakeapplicationsource.cpp \
  fakegesturesource.h       fakegesturesource.cpp \
  fakekeymap.h              fakekeymap.cpp \
  gtest_fake_action_sink.cpp \
  gtest_fake_application_source.cpp \
  gtest_fake_gesture_source.cpp \
  gtest_xml_wish_source.cpp

gmock_ginn_CPPFLAGS = \
  -I$(top_srcdir) \
  -I$(top_builddir) \
  $(GMOCK_CPPFLAGS) \
  $(GTEST_CPPFLAGS)

gmock_ginn_LDADD = \
  $(top_builddir)/ginn/libginn.a \
  $(BAMF_LIBS) \
  $(GEIS_LIBS) \
  $(GIO_LIBS) \
  $(GLIB2_0_LIBS) \
  $(XML2_LIBS) \
  $(XTEST_LIBS) \
  libgmock.a \
  -lpthread

endif
