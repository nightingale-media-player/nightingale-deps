###########################################################################
# Everything below here is generic and you shouldn't need to change it.
###########################################################################
# thomas: except of course that we did

# thomas: copied from glib-2
# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH, when it comes to
# whether a source that is a target of one rule is then
# searched for in VPATH/GPATH.
#
GPATH = $(srcdir)

# thomas: make docs parallel installable
TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@

EXTRA_DIST = 				\
	$(content_files)		\
	$(extra_files)			\
	$(HTML_IMAGES)			\
	$(DOC_MAIN_SGML_FILE)		\
	$(DOC_MODULE).types		\
	$(DOC_OVERRIDES)		\
	$(DOC_MODULE)-sections.txt

DOC_STAMPS =				\
	scan-build.stamp		\
	tmpl-build.stamp		\
	sgml-build.stamp		\
	html-build.stamp		\
	$(srcdir)/tmpl.stamp		\
	$(srcdir)/sgml.stamp		\
	$(srcdir)/html.stamp

SCANOBJ_FILES =				\
	$(DOC_MODULE).args		\
	$(DOC_MODULE).hierarchy		\
	$(DOC_MODULE).interfaces		\
	$(DOC_MODULE).prerequisites	\
	$(DOC_MODULE).signals		\
	.libs/$(DOC_MODULE)-scan.o

REPORT_FILES = \
	$(DOC_MODULE)-undocumented.txt \
	$(DOC_MODULE)-undeclared.txt \
	$(DOC_MODULE)-unused.txt

CLEANFILES = $(SCANOBJ_FILES) $(REPORT_FILES) $(DOC_STAMPS) doc-registry.xml

if ENABLE_GTK_DOC
all-local: html-build.stamp

#### scan ####

# in the case of non-srcdir builds, the built gst directory gets added
# to gtk-doc scanning; but only then, to avoid duplicates
scan-build.stamp: $(HFILE_GLOB) $(SCANOBJ_DEPS) $(basefiles)
	@echo '*** Scanning header files ***'
	if grep -l '^..*$$' $(srcdir)/$(DOC_MODULE).types > /dev/null;	\
	then								\
	    if test x"$(srcdir)" != x. ; then				\
	        cp $(srcdir)/$(DOC_MODULE).types . ;			\
	        chmod u+w $(DOC_MODULE).types ;				\
	    fi ;							\
	    GST_PLUGIN_SYSTEM_PATH=`cd $(top_builddir) && pwd`		\
	    GST_PLUGIN_PATH=						\
	    GST_REGISTRY=doc-registry.xml				\
	    CC="$(GTKDOC_CC)" LD="$(GTKDOC_LD)"				\
	    CFLAGS="$(GTKDOC_CFLAGS) $(CFLAGS)"				\
	    LDFLAGS="$(GTKDOC_LIBS) $(LDFLAGS)"				\
	    gtkdoc-scangobj --type-init-func="gst_init(NULL,NULL)"	\
	        --module=$(DOC_MODULE) ;				\
	else								\
	    cd $(srcdir) ;						\
	    for i in $(SCANOBJ_FILES) ; do				\
               test -f $$i || touch $$i ;				\
	    done							\
	fi
	if test "x$(top_srcdir)" != "x$(top_builddir)";			\
        then								\
          export BUILT_OPTIONS="--source-dir=$(DOC_BUILD_DIR)";		\
        fi;								\
	gtkdoc-scan							\
		$(SCAN_OPTIONS) $(EXTRA_HFILES)				\
		--module=$(DOC_MODULE)					\
		--source-dir=$(DOC_SOURCE_DIR)				\
		$$BUILT_OPTIONS						\
		--ignore-headers="$(IGNORE_HFILES)"
	touch scan-build.stamp

$(DOC_MODULE)-decl.txt $(SCANOBJ_FILES): scan-build.stamp
	@true

#### templates ####

tmpl-build.stamp: $(DOC_MODULE)-decl.txt $(SCANOBJ_FILES) $(DOC_MODULE)-sections.txt $(DOC_OVERRIDES)
	@echo '*** Rebuilding template files ***'
	if test x"$(srcdir)" != x. ; then \
	    cp $(srcdir)/$(DOC_MODULE)-sections.txt . ; \
	    touch $(DOC_MODULE)-decl.txt ; \
	fi
	gtkdoc-mktmpl --module=$(DOC_MODULE) | tee tmpl-build.log
	@if test -s $(DOC_MODULE)-unused.txt; then \
	    exit $(if $(DOCS_ARE_INCOMPLETE_PLEASE_FIXME),0,1); fi
	rm -f tmpl-build.log
	touch tmpl-build.stamp

tmpl.stamp: tmpl-build.stamp
	@true

#### xml ####

### FIXME: make this error out again when docs are complete
sgml-build.stamp: tmpl.stamp $(CFILE_GLOB)
	@echo '*** Building XML ***'
	gtkdoc-mkdb --module=$(DOC_MODULE) --source-dir=$(DOC_SOURCE_DIR) --main-sgml-file=$(srcdir)/$(DOC_MAIN_SGML_FILE) --output-format=xml $(MKDB_OPTIONS) | tee sgml-build.log
	@if grep "WARNING:" sgml-build.log > /dev/null; then true; fi # exit 1; fi
	cp ../version.entities xml
	rm sgml-build.log
	touch sgml-build.stamp

sgml.stamp: sgml-build.stamp
	@true

#### html ####

html-build.stamp: sgml.stamp $(DOC_MAIN_SGML_FILE) $(content_files)
	@echo '*** Building HTML ***'
	if test -d html; then rm -rf html; fi
	mkdir html
	cp $(srcdir)/$(DOC_MAIN_SGML_FILE) html
	@for f in $(content_files); do cp $(srcdir)/$$f html; done
	cp -pr xml html
	cp ../version.entities html
	cd html && gtkdoc-mkhtml $(DOC_MODULE) $(DOC_MAIN_SGML_FILE)
	mv html/index.sgml html/index.sgml.bak
	$(SED) "s/ href=\"$(DOC_MODULE)\// href=\"$(DOC_MODULE)-@GST_MAJORMINOR@\//g" html/index.sgml.bak >html/index.sgml
	rm -f html/index.sgml.bak
	rm -f html/$(DOC_MAIN_SGML_FILE)
	rm -rf html/xml
	rm -f html/version.entities
	test "x$(HTML_IMAGES)" = "x" || for i in "" $(HTML_IMAGES) ; do \
	    if test "$$i" != ""; then cp $(srcdir)/$$i html ; fi; done
	@echo '-- Fixing Crossreferences' 
	gtkdoc-fixxref --module-dir=html --html-dir=$(HTML_DIR) $(FIXXREF_OPTIONS)
	touch html-build.stamp

clean-local-gtkdoc:
	rm -rf xml tmpl html
# clean files copied for nonsrcdir templates build
	if test x"$(srcdir)" != x. ; then \
	        rm -rf $(DOC_MODULE).types; \
	fi
else
all-local:
clean-local-gtkdoc:
endif

clean-local: clean-local-gtkdoc
	rm -f *~ *.bak
	rm -rf .libs

# company: don't delete .sgml and -sections.txt as they're in CVS
# FIXME : thomas added all sgml files and some other things to make
# make distcheck work
distclean-local: clean
	rm -f $(REPORT_FILES) \
                $(DOC_MODULE)-decl-list.txt $(DOC_MODULE)-decl.txt
	rm -rf tmpl/*.sgml.bak
	rm -f $(DOC_MODULE).hierarchy
	rm -f *.stamp || true
	if test x"$(srcdir)" != x. ; then \
	    rm -f $(DOC_MODULE)-docs.sgml ; \
	    rm -f $(DOC_MODULE).types ; \
	    rm -f $(DOC_MODULE).interfaces ; \
            rm -f $(DOC_MODULE)-overrides.txt ; \
	    rm -f $(DOC_MODULE).prerequisites ; \
	    rm -f $(DOC_MODULE)-sections.txt ; \
	    rm -rf tmpl/*.sgml ; \
	fi
	rm -rf *.o

maintainer-clean-local: clean
	cd $(srcdir) && rm -rf html \
                xml $(DOC_MODULE)-decl-list.txt $(DOC_MODULE)-decl.txt

# thomas: make docs parallel installable; devhelp requires majorminor too
install-data-local:
	(installfiles=`echo $(srcdir)/html/*.sgml $(srcdir)/html/*.html $(srcdir)/html/*.png $(srcdir)/html/*.css`; \
	if test "$$installfiles" = '$(srcdir)/html/*.sgml $(srcdir)/html/*.html $(srcdir)/html/*.png $(srcdir)/html/*.css'; \
	then echo '-- Nothing to install' ; \
	else \
	  $(mkinstalldirs) $(DESTDIR)$(TARGET_DIR); \
	  for i in $$installfiles; do \
	    echo '-- Installing '$$i ; \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
	  done; \
	  echo '-- Installing $(srcdir)/html/$(DOC_MODULE).devhelp' ; \
	  $(INSTALL_DATA) $(srcdir)/html/$(DOC_MODULE).devhelp \
	    $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@.devhelp; \
	  if test -e $(srcdir)/html/$(DOC_MODULE).devhelp2; then \
        	    $(INSTALL_DATA) $(srcdir)/html/$(DOC_MODULE).devhelp2 \
	           $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@.devhelp2; \
	  fi; \
	  (which gtkdoc-rebase >/dev/null && \
	    gtkdoc-rebase --relative --dest-dir=$(DESTDIR) --html-dir=$(DESTDIR)$(TARGET_DIR)) || true ; \
	fi) 
uninstall-local:
	if test -d $(DESTDIR)$(TARGET_DIR); then \
	  rm -rf $(DESTDIR)$(TARGET_DIR)/*; \
	  rmdir -p $(DESTDIR)$(TARGET_DIR) 2>/dev/null || true; \
	else \
	  echo '-- Nothing to uninstall' ; \
	fi;


#
# Require gtk-doc when making dist
#
if ENABLE_GTK_DOC
dist-check-gtkdoc:
else
dist-check-gtkdoc:
	@echo "*** gtk-doc must be installed and enabled in order to make dist"
	@false
endif

dist-hook: dist-check-gtkdoc dist-hook-local
	mkdir $(distdir)/html
	cp $(srcdir)/html/* $(distdir)/html
	-cp $(srcdir)/$(DOC_MODULE).types $(distdir)/
	-cp $(srcdir)/$(DOC_MODULE)-sections.txt $(distdir)/
	cd $(distdir) && rm -f $(DISTCLEANFILES)
        -gtkdoc-rebase --online --relative --html-dir=$(distdir)/html

.PHONY : dist-hook-local docs
