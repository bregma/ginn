bin_PROGRAMS = gesturetest
man1_MANS = gesturetest.1

AM_CFLAGS = -I$(xcbincludedir) $(NEEDED_LIBS) -I$(XCBINCLUDEDIR)/xcb

EXTSOURCES = gesture.c
EXTHEADERS = $(EXTSOURCES:.c=.h)

BUILT_SOURCES = $(EXTSOURCES) $(EXTHEADERS)
nodist_gesturetest_SOURCES = $(EXTSOURCES) $(EXTHEADERS)
gesturetest_SOURCES = main.c

CLEANFILES = $(EXTSOURCES) $(EXTHEADERS)
EXTRA_DIST = c_client.py c-client.xsl gesturetest.pod

$(EXTSOURCES): c_client.py
	$(PYTHON) $(srcdir)/c_client.py -p $(XCBPROTO_XCBPYTHONDIR) $(builddir)/$(@:.c=.xml)

%.1: %.pod
	pod2man -c '' -d '' -r '' $< > $@
