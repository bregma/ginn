bin_PROGRAMS = ginn

ginn_SOURCES = ginn.c config.c xt.c bamf.c
ginn_CFLAGS = \
	-DGINN_CONFIG_DIR=\"$(pkgdatadir)\" \
	$(GEIS_CFLAGS) $(XTST_CFLAGS) $(X11_CFLAGS) $(XML2_CFLAGS) $(BAMF_CFLAGS)
ginn_LDFLAGS = $(GEIS_LIBS) $(XTST_LIBS) $(X11_LIBS) $(XML2_LIBS) $(BAMF_LIBS)
