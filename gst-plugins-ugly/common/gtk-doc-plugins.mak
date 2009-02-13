# This is an include file specifically tuned for building documentation
# for GStreamer plug-ins

help:
	@echo
	@echo "If you are a doc maintainer, run 'make update' to update"
	@echo "the documentation files maintained in CVS"
	@echo
	@echo Other useful make targets:
	@echo
	@echo  check-inspected-versions: make sure the inspected plugin info
	@echo                            is up to date before a release
	@echo

# update the stuff maintained by doc maintainers
update:
	$(MAKE) inspect-update
	$(MAKE) scanobj-update

# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH, when it comes to
# whether a source that is a target of one rule is then
# searched for in VPATH/GPATH.
#
GPATH = $(srcdir)

# thomas: make docs parallel installable
TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@

EXTRA_DIST = 				\
	scanobj-build.stamp		\
	$(srcdir)/inspect/*.xml		\
	inspect.stamp			\
	inspect-build.stamp		\
	$(SCANOBJ_FILES)		\
	$(content_files)		\
	$(extra_files)			\
	$(HTML_IMAGES)			\
	$(DOC_MAIN_SGML_FILE)	\
	$(DOC_OVERRIDES)		\
	$(DOC_MODULE)-sections.txt

MAINTAINER_DOC_STAMPS =			\
	scanobj-build.stamp		\
	inspect-build.stamp		\
	inspect.stamp

# we don't add inspect-build.stamp and scanobj-build.stamp here since they are
# built manually by docs maintainers and result is commited to CVS
DOC_STAMPS =				\
	scan-build.stamp		\
	tmpl-build.stamp		\
	sgml-build.stamp		\
	html-build.stamp		\
	scan.stamp			\
	tmpl.stamp			\
	sgml.stamp			\
	html.stamp

# files generated/updated by gtkdoc-scangobj
SCANOBJ_FILES =				\
	$(DOC_MODULE).signals           \
        $(DOC_MODULE).hierarchy         \
        $(DOC_MODULE).interfaces        \
        $(DOC_MODULE).prerequisites     \
	$(DOC_MODULE).types		\
        $(DOC_MODULE).args

SCANOBJ_FILES_O =			\
	.libs/$(DOC_MODULE)-scan.o

# files generated/updated by gtkdoc-scan
SCAN_FILES =				\
	$(DOC_MODULE)-sections.txt	\
	$(DOC_MODULE)-overrides.txt	\
	$(DOC_MODULE)-undocumented.txt	\
	$(DOC_MODULE)-decl.txt		\
	$(DOC_MODULE)-decl-list.txt


REPORT_FILES = \
	$(DOC_MODULE)-undocumented.txt \
	$(DOC_MODULE)-undeclared.txt \
	$(DOC_MODULE)-unused.txt

# FC3 seems to need -scan.c to be part of CLEANFILES for distcheck
# no idea why FC4 can do without
CLEANFILES = \
	$(SCANOBJ_FILES_O) \
	$(DOC_MODULE)-scan.c \
	$(REPORT_FILES) \
	$(DOC_STAMPS) \
	inspect-registry.xml


if ENABLE_GTK_DOC
all-local: html-build.stamp

#### scan gobjects; done by documentation maintainer ####
scanobj-update:
	-rm scanobj-build.stamp
	$(MAKE) scanobj-build.stamp

# in the case of non-srcdir builds, the built gst directory gets added
# to gtk-doc scanning; but only then, to avoid duplicates
# FIXME: since we don't have the scan step as part of the build anymore,
# we could remove that
# TODO: finish elite script that updates the output files of this step
# instead of rewriting them, so that multiple maintainers can generate
# a collective set of args and signals
scanobj-build.stamp: $(SCANOBJ_DEPS) $(basefiles)
	@echo '*** Scanning GObjects ***'
	if test x"$(srcdir)" != x. ; then				\
	    for f in $(SCANOBJ_FILES);					\
	    do								\
	        cp $(srcdir)/$$f . ;					\
	    done;							\
	else								\
	    $(INSPECT_ENVIRONMENT) 					\
	    CC="$(GTKDOC_CC)" LD="$(GTKDOC_LD)"				\
	    CFLAGS="$(GTKDOC_CFLAGS) $(CFLAGS)"				\
	    LDFLAGS="$(GTKDOC_LIBS) $(LDFLAGS)"				\
	    $(GST_DOC_SCANOBJ) --type-init-func="gst_init(NULL,NULL)"	\
	        --module=$(DOC_MODULE) --source=$(PACKAGE) &&		\
		$(PYTHON)						\
		$(top_srcdir)/common/scangobj-merge.py $(DOC_MODULE);	\
	fi
	touch scanobj-build.stamp

$(DOC_MODULE)-decl.txt $(SCANOBJ_FILES) $(SCANOBJ_FILES_O): scan-build.stamp
	@true

### inspect GStreamer plug-ins; done by documentation maintainer ###

# only look at the plugins in this module when building inspect .xml stuff
INSPECT_REGISTRY=$(top_builddir)/docs/plugins/inspect-registry.xml
INSPECT_ENVIRONMENT=\
        GST_PLUGIN_SYSTEM_PATH= \
        GST_PLUGIN_PATH=$(top_builddir)/gst:$(top_builddir)/sys:$(top_builddir)/ext:$(top_builddir)/plugins:$(top_builddir)/src:$(top_builddir)/gnl \
        GST_REGISTRY=$(INSPECT_REGISTRY)

# update the element and plugin XML descriptions; store in inspect/
inspect:
	mkdir inspect

inspect-update: inspect
	-rm -f $(INSPECT_REGISTRY) inspect-build.stamp
	$(MAKE) inspect-build.stamp

# FIXME: inspect.stamp should be written to by gst-xmlinspect.py
# IF the output changed; see gtkdoc-mktmpl
inspect-build.stamp:
	@echo '*** Rebuilding plugin inspection files ***'
	if test x"$(srcdir)" != x. ; then \
	    cp $(srcdir)/inspect.stamp . ; \
	    cp $(srcdir)/inspect-build.stamp . ; \
	else \
	    $(INSPECT_ENVIRONMENT) $(PYTHON) \
	        $(top_srcdir)/common/gst-xmlinspect.py $(PACKAGE) inspect && \
	    echo -n "timestamp" > inspect.stamp && \
	    touch inspect-build.stamp; \
        fi

### scan headers; done on every build ###
scan-build.stamp: $(HFILE_GLOB) $(EXTRA_HFILES) $(basefiles) scanobj-build.stamp inspect-build.stamp
	if test "x$(top_srcdir)" != "x$(top_builddir)" &&		\
	   test -d "$(top_builddir)/gst";				\
        then								\
            export BUILT_OPTIONS="--source-dir=$(top_builddir)/gst";	\
        fi;								\
	gtkdoc-scan							\
	    $(SCAN_OPTIONS) $(EXTRA_HFILES)				\
	    --module=$(DOC_MODULE)					\
	    $$BUILT_OPTIONS						\
	    --ignore-headers="$(IGNORE_HFILES)";			\
	touch scan-build.stamp

#### update templates; done on every build ####

### FIXME: make this error out again when docs are fixed for 0.9
# in a non-srcdir build, we need to copy files from the previous step
# and the files from previous runs of this step
tmpl-build.stamp: $(DOC_MODULE)-decl.txt $(SCANOBJ_FILES) $(DOC_MODULE)-sections.txt $(DOC_OVERRIDES)
	@echo '*** Rebuilding template files ***'
	if test x"$(srcdir)" != x. ; then				\
	    for f in $(SCANOBJ_FILES) $(SCAN_FILES);			\
	    do								\
	        if test -e $(srcdir)/$$f; then cp $(srcdir)/$$f . ; fi; \
	    done;							\
	fi
	gtkdoc-mktmpl --module=$(DOC_MODULE) | tee tmpl-build.log
	$(PYTHON) \
		$(top_srcdir)/common/mangle-tmpl.py $(srcdir)/inspect tmpl
	@cat $(DOC_MODULE)-unused.txt
	rm -f tmpl-build.log
	touch tmpl-build.stamp

tmpl.stamp: tmpl-build.stamp
	@true

#### build xml; done on every build ####

### FIXME: make this error out again when docs are fixed for 0.9
sgml-build.stamp: tmpl.stamp inspect.stamp $(CFILE_GLOB) $(top_srcdir)/common/plugins.xsl
	@echo '*** Building XML ***'
	@-mkdir -p xml
	@for a in $(srcdir)/inspect/*.xml; do \
            xsltproc --stringparam module $(MODULE) \
		$(top_srcdir)/common/plugins.xsl $$a > xml/`basename $$a`; done
	@for f in $(EXAMPLE_CFILES); do \
		$(PYTHON) $(top_srcdir)/common/c-to-xml.py $$f > xml/element-`basename $$f .c`.xml; done
	gtkdoc-mkdb \
		--module=$(DOC_MODULE) \
		--source-dir=$(DOC_SOURCE_DIR) \
		--main-sgml-file=$(srcdir)/$(DOC_MAIN_SGML_FILE) \
		--output-format=xml \
		--ignore-files="$(IGNORE_HFILES) $(IGNORE_CFILES)" \
		$(MKDB_OPTIONS) \
		| tee sgml-build.log
	@if grep "WARNING:" sgml-build.log > /dev/null; then true; fi # exit 1; fi
	cp ../version.entities xml
	rm sgml-build.log
	touch sgml-build.stamp

sgml.stamp: sgml-build.stamp
	@true

#### build html; done on every step ####

html-build.stamp: sgml.stamp $(DOC_MAIN_SGML_FILE) $(content_files)
	@echo '*** Building HTML ***'
	if test -d html; then rm -rf html; fi
	mkdir html
	cp $(srcdir)/$(DOC_MAIN_SGML_FILE) html
	@for f in $(content_files); do cp $(srcdir)/$$f html; done
	cp -pr xml html
	cp ../version.entities html
	cd html && gtkdoc-mkhtml $(DOC_MODULE) $(DOC_MAIN_SGML_FILE) \
	    2>&1 | tee ../html-build.log
	@if grep "warning:" html-build.log > /dev/null; then \
		echo "ERROR"; grep "warning:" html-build.log; exit 1; fi
	@rm html-build.log
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
	    rm -rf $(SCANOBJ_FILES) $(SCAN_FILES); \
	fi
else
all-local:
clean-local-gtkdoc:
endif

clean-local: clean-local-gtkdoc
	rm -f *~ *.bak
	rm -rf .libs

distclean-local: clean
	rm -rf tmpl/*.sgml.bak
	rm -rf *.o

MAINTAINERCLEANFILES = $(MAINTAINER_DOC_STAMPS)

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
	  pngfiles=`echo ./html/*.png`; \
	  if test "$$pngfiles" != './html/*.png'; then \
	    for i in $$pngfiles; do \
	      echo '-- Installing '$$i ; \
	      $(INSTALL_DATA) $$i $(DESTDIR)$(TARGET_DIR); \
	    done; \
	  fi; \
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
	(installfiles=`echo ./html/*.html`; \
	if test "$$installfiles" = './html/*.html'; \
	then echo '-- Nothing to uninstall' ; \
	else \
	  for i in $$installfiles; do \
	    rmfile=`basename $$i` ; \
	    echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/'$$rmfile ; \
	    rm -f $(DESTDIR)$(TARGET_DIR)/$$rmfile; \
	  done; \
	  pngfiles=`echo ./html/*.png`; \
	  if test "$$pngfiles" != './html/*.png'; then \
	    for i in $$pngfiles; do \
	      rmfile=`basename $$i` ; \
	      echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/'$$rmfile ; \
	      rm -f $(DESTDIR)$(TARGET_DIR)/$$rmfile; \
	    done; \
	  fi; \
	  echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE).devhelp' ; \
	  rm -f $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@.devhelp; \
	  if test -e $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@.devhelp2; then \
	    rm -f $(DESTDIR)$(TARGET_DIR)/$(DOC_MODULE)-@GST_MAJORMINOR@.devhelp2; \
	  fi; \
	  echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/index.sgml' ; \
	  rm -f $(DESTDIR)$(TARGET_DIR)/index.sgml; \
		if test -e $(DESTDIR)$(TARGET_DIR)/style.css; then \
			echo '-- Uninstalling $(DESTDIR)$(TARGET_DIR)/style.css' ; \
			rm -f $(DESTDIR)$(TARGET_DIR)/style.css; \
		fi; \
	fi) 
	if test -d $(DESTDIR)$(TARGET_DIR); then rmdir -p --ignore-fail-on-non-empty $(DESTDIR)$(TARGET_DIR) 2>/dev/null; fi; true

#
# Checks
#
check-hierarchy: $(DOC_MODULE).hierarchy
	@if grep '	' $(DOC_MODULE).hierarchy; then \
	    echo "$(DOC_MODULE).hierarchy contains tabs, please fix"; \
	    /bin/false; \
	fi

check: check-hierarchy

# wildcard is apparently not portable to other makes, hence the use of find
inspect_files = $(shell find $(srcdir)/inspect -name '*.xml')

check-inspected-versions:
	@echo Checking plugin versions of inspected plugin data ...; \
	fail=0 ; \
	for each in $(inspect_files) ; do \
	  if (grep -H '<version>' $$each | grep -v '<version>$(VERSION)'); then \
	    echo $$each should be fixed to say version $(VERSION) or be removed ; \
	    echo "sed -i -e 's/<version.*version>/<version>$(VERSION)<\/version>/'" $$each; \
	    echo ; \
	    fail=1; \
	  fi ; \
	done ; \
	exit $$fail

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

# FIXME: decide whether we want to dist generated html or not
dist-hook: dist-check-gtkdoc dist-hook-local
	mkdir $(distdir)/html
	cp $(srcdir)/html/* $(distdir)/html
	-cp $(srcdir)/$(DOC_MODULE).types $(distdir)/
	-cp $(srcdir)/$(DOC_MODULE)-sections.txt $(distdir)/
	cd $(distdir) && rm -f $(DISTCLEANFILES)
        -gtkdoc-rebase --online --relative --html-dir=$(distdir)/html

.PHONY : dist-hook-local docs

