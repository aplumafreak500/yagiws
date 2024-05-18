/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#ifndef UTIL_H
#define UTIL_H
// Gettext
#include "gettext.h"
#define _(x) gettext(x)
#define _N(x) gettext_noop(x)

// RNG
long double rndFloat();
#endif
