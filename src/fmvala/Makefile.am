NULL=

AM_CPPFLAGS = \
	-I$(srcdir) \
	-I$(srcdir)../ \
	-DPACKAGE_LIB_DIR=\""$(libdir)/libfmcore"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

noinst_LTLIBRARIES = libfmvala.la

libfmvala_la_SOURCES = \
	fm-config.vala \
	$(NULL)

libfmvala_la_CFLAGS = \
	$(GTK_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)

VALAFLAGS = \
	--pkg gtk+-2.0 \
    --header fm-config.h \
	$(NULL)

libfmvalaincludedir = $(includedir)/libfmcore
libfmvalainclude_HEADERS = \
	fm-config.h \
	$(NULL)

libfmvala_la_LDFLAGS = \
	$(NULL)

libfmvala_la_LIBADD = \
	$(INTLLIBS) \
	$(NULL)
