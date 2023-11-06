#include <stdio.h>
#include "item.h"

/*static*/ const char* const chrList[MAX_CHARS] = {
	[0] = "None",
	[1] = "Kate",
	[2] = "Kamisato Ayaka",
	[3] = "Jean",
	[5] = "Aether",
	[6] = "Lisa",
	[7] = "Lumine",
	[14] = "Barbara",
	[15] = "Kayea",
	[16] = "Diluc",
	[20] = "Razor",
	[21] = "Amber",
	[22] = "Venti",
	[23] = "Xiangling",
	[24] = "Beidou",
	[25] = "Xingqiu",
	[26] = "Xiao",
	[27] = "Ningguang",
	[29] = "Klee",
	[30] = "Zhongli",
	[31] = "Fishcl",
	[32] = "Bennett",
	[33] = "Tartaglia",
	[34] = "Noelle",
	[35] = "Qiqi",
	[36] = "Chongyun",
	[37] = "Ganyu",
	[38] = "Albedo",
	[39] = "Diona",
	[41] = "Mona",
	[42] = "Keqing",
	[43] = "Sucrose",
	[44] = "Xinyan",
	[45] = "Rosaria",
	[46] = "Hu Tao",
	[47] = "Kadehara Kazuha",
	[48] = "Yanfei",
	[49] = "Yoimiya",
	[50] = "Thoma",
	[51] = "Eula",
	[52] = "Raiden Shogun",
	[53] = "Sayu",
	[54] = "Sangonomiya Kokomi",
	[55] = "Gorou",
	[56] = "Kujou Sara",
	[57] = "Arataki Itto",
	[58] = "Yae Miko",
	[59] = "Shikanoin Heizou",
	[60] = "Yelan",
	[61] = "Kiara",
	[62] = "Aloy",
	[63] = "Shenhe",
	[64] = "Yun Jin",
	[65] = "Kuki Shinobu",
	[66] = "Kamisato Ayato",
	[67] = "Collei",
	[68] = "Dori",
	[69] = "Tighnari",
	[70] = "Nilou",
	[71] = "Cyno",
	[72] = "Candace",
	[73] = "Nahida",
	[74] = "Layla",
	[75] = "Wanderer",
	[76] = "Faruzan",
	[77] = "Yaoyao",
	[78] = "Alhatham",
	[79] = "Dehya",
	[80] = "Mika",
	[81] = "Kaveh",
	[82] = "Baizhu",
	[83] = "Lynette",
	[84] = "Lyney",
	[85] = "Freminet",
	[86] = "Wriothesley",
	[87] = "Neuvillette",
	[88] = "Charlotte",
	[89] = "Furina",
};

const char* getCharacter(unsigned int id) {
	static char stellaBuf[1024];
	if (id < 1000) return NULL;
	if (id < 1000 + MAX_CHARS) {
		return chrList[id - 1000];
	}
	if (id >= 1100 && id < 1100 + MAX_CHARS) {
		if (chrList[id - 1100] == NULL) {
			return NULL;
		}
		snprintf(stellaBuf, 1024, "%s's Stella Fortuna", chrList[id - 1100]);
		return stellaBuf;
	}
	// TODO handle avatar IDs (> 10000000)
	return NULL;
}
