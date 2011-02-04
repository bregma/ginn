bin_PROGRAMS = ginn
man1_MANS = ginn.1

ginn_SOURCES = ginn.c config.h config.c xt.c bamf.c
ginn_CFLAGS = \
	-DGINN_CONFIG_DIR=\"$(sysconfdir)/ginn\" \
	$(GEIS_CFLAGS) $(XTST_CFLAGS) $(X11_CFLAGS) $(XML2_CFLAGS) $(BAMF_CFLAGS)
ginn_LDFLAGS = $(GEIS_LIBS) $(XTST_LIBS) $(X11_LIBS) $(XML2_LIBS) $(BAMF_LIBS)

EXTRA_DIST = ginn.pod

%.1: %.pod
	pod2man -c '' -d '' -r '' $< > $@

DISTCLEANFILES = $(man1_MANS)

