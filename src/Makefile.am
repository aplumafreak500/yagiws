# SPDX-License-Identifier: MPL-2.0
# This file is part of Yet Another Genshin Impact Wish Simulator
# ©2024 Alex Pensinger (ArcticLuma113)
# Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/gnulib -DLOCALEDIR=\"$(datadir)/locale\"
bin_PROGRAMS = yagiws
yagiws_SOURCES = yagiws.c bannerdata.c gacha.c item.c character.c weapon.c artifact.c util.c
yagiws_LDADD = $(top_builddir)/gnulib/libgnu.a $(GETRANDOM_LIB) $(FABSL_LIBM) $(FABS_LIBM) $(LIBINTL) $(SETLOCALE_LIB) $(SETLOCALE_NULL_LIB) $(HARD_LOCALE_LIB) @INTL_MACOSX_LIBS@ $(LIBUNISTRING) $(MBRTOWC_LIB) $(LIBC32CONV) $(LIBTHREAD)
