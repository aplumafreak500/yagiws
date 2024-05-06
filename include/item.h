/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#ifndef ITEM_H
#define ITEM_H
// Characters
#define MAX_CHARS 99

const char* getCharacter(unsigned int);

// Artifacts
const char* getArtifact(unsigned int);

// Weapons
enum {
	SWORD = 1,
	CLAYMORE = 2,
	POLEARM = 3,
	CATALYST = 4,
	BOW = 5,
};

const char* getWeapon(unsigned int);

// Other Items TODO
const char* getItem(unsigned int);
#endif
