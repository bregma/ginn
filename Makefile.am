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

AM_MAKEFLAGS = --no-print-directory

SUBDIRS = ginn data test doc

dist-hook:
	@test -d "$(top_srcdir)/.bzr" && \
	  $(top_srcdir)/config-aux/missing --run \
	    bzr log --gnu-changelog -n 1 $(top_srcdir) > ChangeLog \
	    || true

