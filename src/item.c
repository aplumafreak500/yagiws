/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <stddef.h>
#include "config.h"
#include "item.h"

const char* getItem(unsigned int id) {
	const char* ret;
	// TODO There's gotta be a better way to do this, right?
	ret = getCharacter(id);
	if (ret != NULL) return ret;
	ret = getLightcone(id);
	if (ret != NULL) return ret;
	// TODO Other items
	return NULL;
}
