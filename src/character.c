/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#include "item.h"

static const char* const chrList[316] = {
	[0] = "None",
	[1] = "March 7th",
	[2] = "Dan Heng",
	[3] = "Himeko",
	[4] = "Welt",
	[5] = "Kafka",
	[6] = "Silver Wolf",
	[8] = "Arlan",
	[9] = "Asta",
	[13] = "Herta",
	[101] = "Bronya",
	[102] = "Seele",
	[103] = "Serval",
	[104] = "Gepard",
	[105] = "Natasha",
	[106] = "Pela",
	[107] = "Clara",
	[108] = "Sampo",
	[109] = "Hook",
	[110] = "Lynx",
	[111] = "Luka",
	[112] = "Topaz & Numby",
	[201] = "Qingque",
	[202] = "Tingyun",
	[203] = "Luocha",
	[204] = "Jing Yuan",
	[205] = "Blade",
	[206] = "Sushang",
	[207] = "Yukong",
	[208] = "Fu Xuan",
	[209] = "Yanqing",
	[210] = "Guinaifen",
	[211] = "Bailu",
	[212] = "Jingliu",
	[213] = "Dan Heng • Imbibitor Lunae",
	[214] = "Xueyi",
	[215] = "Hanya",
	[217] = "Huohuo",
	[301] = "Gallagher",
	[302] = "Argenti",
	[303] = "Ruan Mei",
	[304] = "Aventurine",
	[305] = "Dr. Ratio",
	[306] = "Sparkle",
	[307] = "Black Swan",
	[308] = "Acheron",
	[309] = "Robin",
	[312] = "Misha",
	[315] = "Boothill",
};

const char* getCharacter(unsigned int id) {
	static char strBuf[1024];
	if (id < 1000) return NULL;
	if (id >= 1000 && id <= 1315) {
		return chrList[id - 1000];
	}
	// Special logic for the Trailblazer
	static const char* const paths[3] = {
		"Destruction",
		"Preservation",
		"Harmony",
	};
	static const char* const names[2] = {"Caelus", "Stelle"};
	unsigned int path = (id - 8001) / 2;
	if (id >= 8001 && id <= 8006) {
		snprintf(strBuf, 1024, "%s (%s)", names[id & 1], paths[path]);
		return strBuf;
	}
	// Special logic for Eidlons
	if (id >= 11000 && id <= 11312) {
		if (chrList[id - 11000] == NULL) {
			return NULL;
		}
		snprintf(strBuf, 1024, "%s's Eidolon", chrList[id - 11000]);
		return strBuf;
	}
	// Special logic for the Trailblazer's Eidlons
	if (id >= 18001 && id <= 18006) {
		snprintf(strBuf, 1024, "Shadow of %s", paths[path]);
		return strBuf;
	}
	return NULL;
}
