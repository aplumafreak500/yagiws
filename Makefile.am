# SPDX-License-Identifier: MPL-2.0
# This file is part of Yet Another Genshin Impact Wish Simulator
# ©2024 Alex Pensinger (ArcticLuma113)
# Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/

SUBDIRS = po src
dist_doc_DATA = Readme.md Changelog License.txt
EXTRA_DIST = include $(top_srcdir)/.version
BUILT_SOURCES = $(top_srcdir)/.version

$(top_srcdir)/.version:
	$(AM_V_GEN)echo '$(VERSION)' > $@-t && mv $@-t $@

dist-hook:
	$(AM_V_GEN)echo '$(VERSION)' > $(distdir)/.tarball-version
