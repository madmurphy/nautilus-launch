# Process this file with automake to produce Makefile.in


ACLOCAL_AMFLAGS = -I m4

AM_DISTCHECK_CONFIGURE_FLAGS = --without-nautilus-libdir

SUBDIRS = \
	src

dist_doc_DATA = \
	AUTHORS \
	COPYING \
	ChangeLog \
	NEWS \
	README

EXTRA_DIST = \
	ChangeLog.md \
	INSTALL \
	NEWS \
	README.md \
	bootstrap \
	package.json \
	.editorconfig


if NLS_ENABLED

SUBDIRS += \
	po

else !NLS_ENABLED

EXTRA_DIST += \
	po

endif !NLS_ENABLED


# Shell expansion is supported here
_distclean_dirs_ = \
	'po/backups'


# Remove `$(docdir)` directory on uninstall
uninstall-local:
	-rm -r '$(DESTDIR)$(docdir)';


distclean-local:
	-rm -rf $(_distclean_dirs_);


# Update translations
.PHONY: i18n-update
i18n-update:
	$(MKDIR_P) 'po/backups' && $(GREP) -o '^\s*\w\S\+' 'po/LINGUAS' | \
	while read _i_; do cp "po/$${_i_}.po" "po/backups/$${_i_}.po.bak" && \
	$(MAKE) -C 'po' "$${_i_}.po" && sed -i \
	's/"Project-Id-Version:.*$$/"Project-Id-Version: @PACKAGE_TARNAME@ @PACKAGE_VERSION@\\n"/' \
	"po/$${_i_}.po"; done;


# EOF

