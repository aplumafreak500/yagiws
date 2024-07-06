# SPDX-License-Identifier: MPL-2.0
# This file is part of Yet Another Genshin Impact Wish Simulator
# ©2024 Alex Pensinger (ArcticLuma113)
# Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/

AC_INIT([yagiws], [1.4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-xz no-dist-gzip])
AM_MAINTAINER_MODE([disable])
AC_ARG_ENABLE(
	[debug-mode],
	AS_HELP_STRING(
		[--enable-debug-mode],
		[Enable a debug mode that disables most sanity checks. Expect lots of segfaults, so use with caution!]
	)
)
AC_USE_SYSTEM_EXTENSIONS([_GNU_SOURCE])
AC_PROG_CC
AS_IF([test "x$enable_debug_mode" = "xyes"], [
	AC_DEFINE([DEBUG], [1], [If debug mode is enabled.])
])
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(sys/random.h)
AC_CHECK_HEADERS(math.h)
# TODO: Replacement functions/macros, rather than erroring out
AC_CHECK_FUNC(getrandom, [], [AC_MSG_ERROR([required function getrandom() is not defined])])
AC_CHECK_FUNC(getopt_long, [], [AC_MSG_ERROR([required function getopt_long() is not defined])])
AC_CHECK_FUNC(fabsl, [], [
	AC_CHECK_LIB(m, fabsl, [],
		[AC_MSG_ERROR([required function fabsl() is not defined])]
	, [])
])
AC_CHECK_FUNC(strtoull, [], [AC_MSG_ERROR([required function strtoull() is not defined])])
AC_CHECK_FUNC(strcasecmp, [], [AC_MSG_ERROR([required function strcasecmp() is not defined])])
AC_CHECK_FUNC(sscanf, [], [AC_MSG_ERROR([required function sscanf() is not defined])])
AC_CHECK_FUNC(printf, [], [AC_MSG_ERROR([required function printf() is not defined])])
AC_CHECK_FUNC(fprintf, [], [AC_MSG_ERROR([required function fprintf() is not defined])])
AC_CHECK_FUNC(snprintf, [], [AC_MSG_ERROR([required function snprintf() is not defined])])
AC_CHECK_DECL(program_invocation_name, [], [AC_MSG_ERROR([required symbol program_invocation_name is not defined])], [[#include <errno.h>]])
AC_CHECK_DECL(stderr, [], [AC_MSG_ERROR([required symbol stderr is not defined])], [[#include <stdio.h>]])
AC_CHECK_DECL(optarg, [], [AC_MSG_ERROR([required symbol optarg is not defined])], [[#include <getopt.h>]])
AC_CHECK_DECL(optind, [], [AC_MSG_ERROR([required symbol optind is not defined])], [[#include <getopt.h>]])
AC_CHECK_DECL(LLONG_MAX, [], [AC_MSG_ERROR([required symbol LLONG_MAX is not defined])], [[#include <limits.h>]])
AC_CHECK_DECL(NULL, [], [AC_MSG_ERROR([required symbol NULL is not defined])], [[#include <stddef.h>]])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
