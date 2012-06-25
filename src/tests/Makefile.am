##include $(top_srcdir)/Makefile.decl

NULL=
TEST_PROGS =


AM_CPPFLAGS = \
	-I../ \
	-I../base \
	-I../fm-vala \
	-I../gtk \
	-I../gtk-dlg \
	-I../jobs \
	-I../jobs/FmJobs \
	-I../utils \
	$(GTK_CFLAGS) \
	$(GIO_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(MENU_CACHE_CFLAGS) \
	$(DBUS_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)


VALAFLAGS = \
	--vapidir=$(top_srcdir)/vapi \
	--pkg vconfig \
	--pkg $(GTK_PKG) \
    --pkg debug \
    --pkg libfmcore \
    --pkg libfmcore-private \
    --pkg posix \
	$(NULL)


noinst_PROGRAMS = $(TEST_PROGS)



TEST_PROGS += automount

automount_SOURCES = automount.c

automount_LDADD= \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	../libfmcore-@LIBFMCORE_API_VERSION@.la \
	$(NULL)
	


TEST_PROGS += mount

mount_SOURCES = mount.c

mount_LDADD= \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	../libfmcore-@LIBFMCORE_API_VERSION@.la \
	$(NULL)
	


TEST_PROGS += main

main_SOURCES = main.c

main_LDADD= \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	../libfmcore-@LIBFMCORE_API_VERSION@.la \
	$(NULL)
	
	


TEST_PROGS += filemanager

filemanager_SOURCES = \
	FileManager/Application.vala \
	FileManager/ManagerWindow.vala \
	$(NULL)
	

filemanager_LDADD= \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	../libfmcore-@LIBFMCORE_API_VERSION@.la \
	$(NULL)


