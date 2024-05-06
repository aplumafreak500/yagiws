/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <limits.h>
#include "config.h"
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_SYS_RANDOM_H
#include <sys/random.h>
#endif
#include "util.h"

long double rndFloat() {
	static long long rndBuf;
	getrandom(&rndBuf, sizeof(long long), 0);
	return fabsl((long double) rndBuf / (long double) LLONG_MAX);
}
