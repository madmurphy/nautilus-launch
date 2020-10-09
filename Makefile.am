# Process this file with automake to produce Makefile.in


ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	src

dist_doc_DATA = \
	AUTHORS \
	COPYING \
	ChangeLog \
	NEWS \
	README

INTLTOOL_FILES = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	ChangeLog.md \
	INSTALL \
	NEWS \
	README.md \
	bootstrap \
	package.json \
	$(INTLTOOL_FILES)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


if NLS_ENABLED

SUBDIRS += \
	po

else !NLS_ENABLED

EXTRA_DIST += \
	po

endif !NLS_ENABLED


# Remove doc directory on uninstall
uninstall-local:
	-rm -r '$(DESTDIR)$(docdir)'


# EOF

