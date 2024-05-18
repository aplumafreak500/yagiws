# SPDX-License-Identifier: MPL-2.0
# This file is part of Yet Another Genshin Impact Wish Simulator
# ©2024 Alex Pensinger (ArcticLuma113)
# Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/

AC_INIT([yagiws], m4_esyscmd([./git-version-gen .tarball-version]))
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-xz no-dist-gzip])
AM_MAINTAINER_MODE([disable])
AM_SILENT_RULES([yes])
AC_ARG_ENABLE(
	[debug-mode],
	AS_HELP_STRING(
		[--enable-debug-mode],
		[Enable a debug mode that disables most sanity checks. Expect lots of segfaults, so use with caution!]
	)
)
AC_PROG_CC
gl_EARLY
gl_INIT
AS_IF([test "x$enable_debug_mode" = "xyes"], [
	AC_DEFINE([DEBUG], [1], [If debug mode is enabled.])
])
AC_C_CONST
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.21)
AC_CHECK_DECL(program_invocation_name, [], [AC_MSG_ERROR([required symbol program_invocation_name is not defined])], [[#include <errno.h>]])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in gnulib/Makefile])
AC_OUTPUT
