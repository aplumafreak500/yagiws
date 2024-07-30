/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stdio.h>
#include "item.h"
#include "util.h"

static const char* const chrList[103] = {
	[0] = _N("None"),
	[1] = _N("Kate"),
	[2] = _N("Kamisato Ayaka"),
	[3] = _N("Jean"),
	[5] = _N("Aether"),
	[6] = _N("Lisa"),
	[7] = _N("Lumine"),
	[14] = _N("Barbara"),
	[15] = _N("Kayea"),
	[16] = _N("Diluc"),
	[20] = _N("Razor"),
	[21] = _N("Amber"),
	[22] = _N("Venti"),
	[23] = _N("Xiangling"),
	[24] = _N("Beidou"),
	[25] = _N("Xingqiu"),
	[26] = _N("Xiao"),
	[27] = _N("Ningguang"),
	[29] = _N("Klee"),
	[30] = _N("Zhongli"),
	[31] = _N("Fishcl"),
	[32] = _N("Bennett"),
	[33] = _N("Tartaglia"),
	[34] = _N("Noelle"),
	[35] = _N("Qiqi"),
	[36] = _N("Chongyun"),
	[37] = _N("Ganyu"),
	[38] = _N("Albedo"),
	[39] = _N("Diona"),
	[41] = _N("Mona"),
	[42] = _N("Keqing"),
	[43] = _N("Sucrose"),
	[44] = _N("Xinyan"),
	[45] = _N("Rosaria"),
	[46] = _N("Hu Tao"),
	[47] = _N("Kadehara Kazuha"),
	[48] = _N("Yanfei"),
	[49] = _N("Yoimiya"),
	[50] = _N("Thoma"),
	[51] = _N("Eula"),
	[52] = _N("Raiden Shogun"),
	[53] = _N("Sayu"),
	[54] = _N("Sangonomiya Kokomi"),
	[55] = _N("Gorou"),
	[56] = _N("Kujou Sara"),
	[57] = _N("Arataki Itto"),
	[58] = _N("Yae Miko"),
	[59] = _N("Shikanoin Heizou"),
	[60] = _N("Yelan"),
	[61] = _N("Kiara"),
	[62] = _N("Aloy"),
	[63] = _N("Shenhe"),
	[64] = _N("Yun Jin"),
	[65] = _N("Kuki Shinobu"),
	[66] = _N("Kamisato Ayato"),
	[67] = _N("Collei"),
	[68] = _N("Dori"),
	[69] = _N("Tighnari"),
	[70] = _N("Nilou"),
	[71] = _N("Cyno"),
	[72] = _N("Candace"),
	[73] = _N("Nahida"),
	[74] = _N("Layla"),
	[75] = _N("Wanderer"),
	[76] = _N("Faruzan"),
	[77] = _N("Yaoyao"),
	[78] = _N("Alhatham"),
	[79] = _N("Dehya"),
	[80] = _N("Mika"),
	[81] = _N("Kaveh"),
	[82] = _N("Baizhu"),
	[83] = _N("Lynette"),
	[84] = _N("Lyney"),
	[85] = _N("Freminet"),
	[86] = _N("Wriothesley"),
	[87] = _N("Neuvillette"),
	[88] = _N("Charlotte"),
	[89] = _N("Furina"),
	[90] = _N("Chevreuse"),
	[91] = _N("Navia"),
	[92] = _N("Gaming"),
	[93] = _N("Xianyun"),
	[94] = _N("Chiori"),
	[95] = _N("Sigewinne"),
	[96] = _N("Arlecchino"),
	[97] = _N("Sethos"),
	[98] = _N("Clorinde"),
	[99] = _N("Emilie"),
	[100] = _N("Kachina"),
	[101] = _N("Kinich"),
	[102] = _N("Mualani"),
};

const char* getCharacter(unsigned int id) {
	static char stellaBuf[1024];
	if (id < 1000) return NULL;
	if (id < 1100) {
		return chrList[id - 1000];
	}
	if (id < 1200) {
		if (chrList[id - 1100] == NULL) {
			return NULL;
		}
		snprintf(stellaBuf, 1024, _("%s's Stella Fortuna"), gettext(chrList[id - 1100]));
		return stellaBuf;
	}
	if (id < 4100 && id > 4102) {
		return chrList[id - 4100];
	}
	if (id < 5100 && id > 5102) {
		if (chrList[id - 5100] == NULL) {
			return NULL;
		}
		snprintf(stellaBuf, 1024, _("%s's Stella Fortuna"), gettext(chrList[id - 5100]));
		return stellaBuf;
	}
	// TODO handle avatar IDs (> 10000000)
	return NULL;
}
