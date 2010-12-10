SUBDIRS = src etc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ginn.pc

INCLUDES = $(top_srcdir)/include/

.PHONY: ChangeLog INSTALL
INSTALL:
	$(INSTALL_CMD)
ChangeLog:
	bzr log > ChangeLog

dist-hook: ChangeLog INSTALL

DISTCLEANFILES = ChangeLog

