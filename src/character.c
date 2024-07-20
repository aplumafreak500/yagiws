/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stdio.h>
#include "item.h"
#include "util.h"

static const char* const chrList[325] = {
	[0] = _N("None"),
	[1] = _N("March 7th"),
	[2] = _N("Dan Heng"),
	[3] = _N("Himeko"),
	[4] = _N("Welt"),
	[5] = _N("Kafka"),
	[6] = _N("Silver Wolf"),
	[8] = _N("Arlan"),
	[9] = _N("Asta"),
	[13] = _N("Herta"),
	[101] = _N("Bronya"),
	[102] = _N("Seele"),
	[103] = _N("Serval"),
	[104] = _N("Gepard"),
	[105] = _N("Natasha"),
	[106] = _N("Pela"),
	[107] = _N("Clara"),
	[108] = _N("Sampo"),
	[109] = _N("Hook"),
	[110] = _N("Lynx"),
	[111] = _N("Luka"),
	[112] = _N("Topaz & Numby"),
	[201] = _N("Qingque"),
	[202] = _N("Tingyun"),
	[203] = _N("Luocha"),
	[204] = _N("Jing Yuan"),
	[205] = _N("Blade"),
	[206] = _N("Sushang"),
	[207] = _N("Yukong"),
	[208] = _N("Fu Xuan"),
	[209] = _N("Yanqing"),
	[210] = _N("Guinaifen"),
	[211] = _N("Bailu"),
	[212] = _N("Jingliu"),
	[213] = _N("Dan Heng • Imbibitor Lunae"),
	[214] = _N("Xueyi"),
	[215] = _N("Hanya"),
	[217] = _N("Huohuo"),
	[301] = _N("Gallagher"),
	[302] = _N("Argenti"),
	[303] = _N("Ruan Mei"),
	[304] = _N("Aventurine"),
	[305] = _N("Dr. Ratio"),
	[306] = _N("Sparkle"),
	[307] = _N("Black Swan"),
	[308] = _N("Acheron"),
	[309] = _N("Robin"),
	[310] = _N("Firefly"),
	[312] = _N("Misha"),
	[314] = _N("Jade"),
	[315] = _N("Boothill"),
	[318] = _N("Jiaoqiu"),
	[321] = _N("Yunli"),
	[324] = _N("March 7th"), // The Hunt variant
};

const char* getCharacter(unsigned int id) {
	static char strBuf[1024];
	if (id < 1000) return NULL;
	if (id >= 1000 && id <= 1315) {
		return chrList[id - 1000];
	}
	// Special logic for the Trailblazer
	static const char* const paths[3] = {
		_N("Destruction"),
		_N("Preservation"),
		_N("Harmony"),
	};
	static const char* const names[2] = {_N("Caelus"), _N("Stelle")};
	unsigned int path = (id - 8001) / 2;
	if (id >= 8001 && id <= 8006) {
		snprintf(strBuf, 1024, _("%s (%s)"), gettext(names[id & 1]), gettext(paths[path]));
		return strBuf;
	}
	// Special logic for Eidlons
	if (id >= 11000 && id <= 11312) {
		if (chrList[id - 11000] == NULL) {
			return NULL;
		}
		snprintf(strBuf, 1024, _("%s's Eidolon"), gettext(chrList[id - 11000]));
		return strBuf;
	}
	// Special logic for the Trailblazer's Eidlons
	if (id >= 18001 && id <= 18006) {
		snprintf(strBuf, 1024, _("Shadow of %s"), gettext(paths[path]));
		return strBuf;
	}
	return NULL;
}
