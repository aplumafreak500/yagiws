/* Yet another Genshin Impact wish simulator */
/* ©2023 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

/*
Release History:
	* v1.0 (2023-10-20): Initial release
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/random.h>
#include <math.h>
#include <errno.h>
#include <getopt.h>

// Change the 4 variables below to configure the banners.
// The three rate-up 4-star characters on both character banners.
static const unsigned short _4StarChrUp[3] = {1068, 1036, 1050};

// The two rate-up 5-star characters, one per character banner.
static const unsigned short _5StarChrUp[2] = {1086, 1022};

// The five rate-up 4-star weapons on the weapon banner.
static const unsigned short _4StarWpnUp[5] = {15427, 13427, 11401, 12405, 14409};

// The two rate-up 5-star weapons on the weapon banner.
static const unsigned short _5StarWpnUp[2] = {14513, 15503};

// The list of 3-star weapons in the pool.
static const unsigned short _3Star[13] = {11301, 11302, 11306, 12301, 12302, 12305, 13303, 14301, 14302, 15304, 15301, 15302, 15304};

// The list of 4-star characters in the pool, along with the rate-up drops.
static const unsigned short _4StarChr[33] = {
	// v1.0 standard pool: Barbara, Razor, Xiangling, Beidou, Xingqiu, Ningguang, Fischl, Bennett, Noelle, Chongyun, Sucrose
	1014, 1020, 1023, 1024, 1025, 1027, 1031, 1032, 1036, 1043,
	// Noelle (listed last from 1.0 chars due to novice banner)
	1034,
	// v1.2: Diona, Xinyan
	1039, 1044,
	// v1.5: Rosaria
	1045,
	// v1.6: Yanfei
	1048,
	// v2.1: Sayu
	1053,
	// v2.2: Kujou Sara
	1056,
	// v2.3: Thoma
	1050,
	// v2.4: Gorou
	1055,
	// v2.5: Yun Jin
	1064,
	// v2.8: Kuki Shinobu
	1065,
	// v3.0: Shikanoin Heizou
	1059,
	// v3.1: Collei, Dori
	1067, 1068,
	// v3.2: Candace
	1072,
	// v3.3: Layla
	1074,
	// v3.4: Faruzan
	1076,
	// v3.5: Yaoyao
	1077,
	// v3.6: Mika
	1080,
	// v3.7: Kaveh
	1081,
	// v3.8: Kiara
	1061,
	// v4.1: Lynette, Freminet
	1083, 1085
};

// The list of 4-star weapons in the pool, along with the rate-up drops.
static const unsigned short _4StarWpn[18] = {11401, 11402, 11403, 11405, 12401, 12402, 12403, 12405, 13401, 13407, 14401, 14402, 14403, 14409, 15401, 15402, 15403, 15405};

// The list of 5-star characters in the pool, along with the rate-up drops.
static const unsigned short _5StarChr[7] = {
	// v1.0 standard pool: Jean, Diluc, Qiqi, Mona, Keqing
	1003, 1016, 1035, 1041, 1042,
	// v3.1: Tighnari
	1069,
	// v3.6: Deyha
	1079
};

// The list of 5-star weapons in the pool, along with the rate-up drops (only on the weapon and standard banners)
static const unsigned short _5StarWpn[10] = {11501, 11502, 12501, 12502, 13502, 13505, 14501, 14502, 15501, 15502};

// Max indexes into _4StarChr per version (for old banners)
static const unsigned char _4StarMaxIndex[28] = {
	/* Novice */ 10,
	/* v1.x */ 11, 11, 13, 13, 13, 14, 15,
	/* v2.x */ 15, 16, 17, 18, 19, 20, 20, 20, 21,
	/* v3.x */ 22, 24, 25, 26, 27, 28, 29, 30, 31,
	/* v4.x */ 31, 33
};

// Max indexes into _5StarChr per version (for old banners)
static const unsigned char _5StarMaxIndex[28] = {
	/* Novice */ 5,
	/* v1.x */ 5, 5, 5, 5, 5, 5, 5,
	/* v2.x */ 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* v3.x */ 5, 6, 6, 6, 6, 6, 7, 7, 7,
	/* v4.x */ 7, 7
};

enum {
	STD_CHR = 0,
	STD_WPN, // non-vanilla
	CHAR1,
	CHAR2,
	WPN,
	NOVICE,
	WISH_CNT
};

#define MAX_CHARS 88

static const char* const chrList[MAX_CHARS] = {
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
	[87] = "Neuvillette", // TODO verify
};

enum {
	SWORD = 1,
	CLAYMORE = 2,
	POLEARM = 3,
	CATALYST = 4,
	BOW = 5,
};

// Weapon data is present, even for 1-star and 2-star weapons, for the sake of completeness.

static const char* const _1StarSwords[1] = {"Dull Blade"};
static const char* const _1StarClaymores[1] = {"Waster Greatsword"};
static const char* const _1StarPolearms[1] = {"Beginner's Protector"};
static const char* const _1StarCatalysts[1] = {"Apprentice's Notes"};
static const char* const _1StarBows[1] = {"Hunter's Bow"};

static const char* const* const _1StarWeapons[6] = {
	[SWORD] = _1StarSwords,
	[CLAYMORE] = _1StarClaymores,
	[POLEARM] = _1StarPolearms,
	[CATALYST] = _1StarCatalysts,
	[BOW] = _1StarBows,
};

static const char* const _2StarSwords[1] = {"Silver Sword"};
static const char* const _2StarClaymores[1] = {"Old Merc's Pal"};
static const char* const _2StarPolearms[1] = {"Iron Point"};
static const char* const _2StarCatalysts[1] = {"Pocket Grimoire"};
static const char* const _2StarBows[1] = {"Seasoned Hunter's Bow"};

static const char* const* const _2StarWeapons[6] = {
	NULL,
	_2StarSwords,
	_2StarClaymores,
	_2StarPolearms,
	_2StarCatalysts,
	_2StarBows,
};

static const char* const _3StarSwords[6] = {
	"Cool Steel",
	"Harbinger of Dawn",
	"Traveler's Handy Sword",
	"Dark Iron Sword",
	"Fillet Blade",
	"Skyrider Sword"
};
static const char* const _3StarClaymores[6] = {
	"Ferrous Shadow",
	"Bloodtainted Greatsword",
	"White Iron Greatsword",
	"Quartz (beta?)",
	"Debate Club",
	"Skyrider Greatsword"
};
static const char* const _3StarPolearms[4] = {
	"White Tassel",
	"Halberd",
	"Black Tassel",
	"Flagstaff"
};
static const char* const _3StarCatalysts[6] = {
	"Magic Guide",
	"Thrilling Tales of Dragon Slayers",
	"Otherworldly Story",
	"Emerald Orb",
	"Twin Nephrite",
	"Amber Bead (beta?)"
};
static const char* const _3StarBows[6] = {
	"Raven Bow",
	"Sharpshooter's Oath",
	"Recurve Bow",
	"Slingshot",
	"Messenger",
	"Ebony Bow (beta?)"
};

static const char* const* const _3StarWeapons[6] = {
	NULL,
	_3StarSwords,
	_3StarClaymores,
	_3StarPolearms,
	_3StarCatalysts,
	_3StarBows,
};

// TODO 4.1 new weapons

static const char* const _4StarSwords[26] = {
	"Favonius Sword",
	"The Flute",
	"Sacrificial Sword",
	"Royal Longsword",
	"Lion's Roar",
	"Prototype Rancour",
	"Iron Sting",
	"Blackcliff Longsword",
	"Black Sword",
	"Alley Flash",
	NULL,
	"Sword of Descension",
	"Festering Desire",
	"Amenoma Kageuchi",
	"Cinnibar Spindle",
	"Kagotsurube Isshin",
	"Sapwood Blade",
	"Xiphos' Moonlight",
	"Prized Isshin Blade (1)",
	"Prized Isshin Blade (2)",
	"Prized Isshin Blade (3)",
	"Toukabou Shigure",
	"Wolf-Fang",
	"Finale of the Deep",
	"Fleuve Cendre Ferryman"
};

static const char* const _4StarClaymores[25] = {
	"Favonius Greatsword",
	"The Bell",
	"Sacrificial Greatsword",
	"Royal Greatsword",
	"Rainslasher",
	"Prototype Archaic",
	"Whiteblind",
	"Blackcliff Slasher",
	"Serpent Spine",
	"Lithic Blade",
	"Snow-Tombed Starsilver",
	"Luxurious Sea-Lord",
	"Katsuragikiri Nagamasa",
	"Makhaira Aquamarine",
	"Akuoumaru",
	"Forest Regalia",
	"Mailed Flower",
	[23] = "Talking Stick",
	[24] = "Tidal Shadow"
};

static const char* const _4StarPolearms[25] = {
	"Dragon's Bane",
	"Prototype Starglitter",
	"Crescent Pike",
	"Blackcliff Pole",
	"Deathmatch",
	"Lithic Spear",
	"Favonius Lance",
	"Royal Spear",
	"Dragonspine Spear",
	[13] = "Kitain Cross Spear",
	[14] = "\"The Catch\"",
	[15] = "Wavebreaker's Fin",
	[16] = "Moonpiercer",
	[18] = "Missive Windspear",
	[23] = "Ballad of the Fjords",
	[24] = "Rightful Rewards"
};

static const char* const _4StarCatalysts[25] = {
	"Favonius Codex",
	"Widsith",
	"Sacrificial Fragments",
	"Royal Grimoire",
	"Prototype Amber",
	"Mappa Mare",
	"Blackcliff Agate",
	"Eye of Perception",
	"Wine and Song",
	NULL,
	"Frostbearer",
	"Dodoco Tales",
	"Hakushin Ring",
	"Oathsworn Eye",
	"Wandering Evenstar",
	"Fruit of Fulfillment",
	[23] = "Sacrificial Jade",
	[24] = "Flowing Purity"
};

static const char* const _4StarBows[25] = {
	"Favonius Warbow",
	"The Stringless",
	"Sacrificial Bow",
	"Royal Bow",
	"Rust",
	"Prototype Crescent",
	"Compound Bow",
	"Blackcliff Warbow",
	"Viridescent Hunt",
	"Alley Hunter",
	"Fading Twilight",
	"Mitternachts Waltz",
	"Windblume Ode",
	"Hamayumi",
	"Predator",
	"Mouun's Moon",
	"King's Squire",
	"End of the Line",
	"Ibis Piercer",
	[23] = "Scion of the Blazing Sun",
	[24] = "Song of Stillness"
};

static const char* const* const _4StarWeapons[6] = {
	NULL,
	_4StarSwords,
	_4StarClaymores,
	_4StarPolearms,
	_4StarCatalysts,
	_4StarBows,
};

static const char* const _5StarSwords[12] = {
	"Aquila Favonia",
	"Skyward Blade",
	"Freedom-Sworn",
	"Summit Shaper",
	"Primordial Jade Cutter (1)",
	"Primordial Jade Cutter (2)",
	"One Side",
	NULL,
	"Mistsplitter Reforged",
	"Haran Geppaku Futsu",
	"Key of Khaj-Nisut",
	"Light of Foliar Incision"
};

static const char* const _5StarClaymores[11] = {
	"Skyward Pride",
	"Wolf's Gravestone",
	"Song of Broken Pines",
	"Unforged",
	"Primordial Jade Greatsword",
	"Other Side",
	NULL,
	NULL,
	NULL,
	"Redhorn Stonethresher",
	"Beacon of the Reed Sea"
};

static const char* const _5StarPolearms[11] = {
	"Staff of Homa",
	"Skyward Spine",
	NULL,
	"Vortex Vanquisher",
	"Primordial Jade Winged-Spear",
	"Deicide",
	"Calamity Queller",
	NULL,
	"Engulfing Lightning",
	NULL,
	"Staff of the Scarlet Fields"
};

static const char* const _5StarCatalysts[12] = {
	"Skyward Atlas",
	"Lost Prayer to the Sacred Winds",
	"Lost Ballade",
	"Memory of Dust",
	"Jadefall's Splendor",
	"Everlasting Moonglow",
	NULL,
	NULL,
	"Kagura's Verity",
	NULL,
	"A Thousand Floating Dreams",
	"Tulaytullah's Rememberance"
};

static const char* const _5StarBows[12] = {
	"Skyward Harp",
	"Amos' Bow",
	"Elegy for the End",
	"Kunwu's Wyrmbane",
	"Primordial Jade Vista",
	"Mirror Breaker",
	"Polar Star",
	"Aqua Simulacara",
	"Thundering Pulse",
	NULL,
	"Hunter's Path",
	"The First Great Magic"
};

static const char* const* const _5StarWeapons[6] = {
	NULL,
	_5StarSwords,
	_5StarClaymores,
	_5StarPolearms,
	_5StarCatalysts,
	_5StarBows,
};

static const char* const* const* const Weapons[6] = {
	NULL,
	_1StarWeapons,
	_2StarWeapons,
	_3StarWeapons,
	_4StarWeapons,
	_5StarWeapons,
};

static const char* getWeapon(unsigned int _id) {
	unsigned int type = (_id / 1000) % 10;
	unsigned int stars = (_id / 100) % 10;
	unsigned int id = _id % 100;
	if (id < 1) return NULL;
	id--;
	unsigned int maxId = 0;
	switch ((type * 10) + stars) {
	default:
		return NULL;
	case 11:
	case 12:
	case 21:
	case 22:
	case 31:
	case 32:
	case 41:
	case 42:
	case 51:
	case 52:
		maxId = 1;
		break;
	case 13:
	case 23:
	case 43:
	case 53:
		maxId = 6;
		break;
	case 33:
		maxId = 4;
		break;
	case 14:
		maxId = 26;
		break;
	case 24:
	case 34:
	case 44:
	case 54:
		maxId = 25;
		break;
	case 15:
	case 45:
	case 55:
		maxId = 12;
		break;
	case 25:
	case 35:
		maxId = 11;
		break;
	}
	if (id >= maxId) return NULL;
	return Weapons[stars][type][id];
}

static unsigned char pity[2];
static unsigned char pityS[4];
static unsigned char getRateUp[2];
static unsigned char fatePoints;
static unsigned short epitomizedPath;
static int doSmooth[2] = {1, 1};
static int doPity[2] = {1, 1};
static char do5050 = 1;

// TODO: There is a different linear rise for standard prior to reaching soft pity. Figure out what it is.
static long double getWeight5(unsigned int _pity) {
	if (_pity <= 73 || !doPity[1]) return 0.006l;
	return 0.006l + 0.06l * (long double) (_pity - 73);
}

static long double getWeight4(unsigned int _pity) {
	if (_pity <= 8 || !doPity[0]) return 0.051l;
	return 0.051l + 0.51l * (long double) (_pity - 8);
}

static long double getWeight5W(unsigned int _pity) {
	if (_pity <= 62 || !doPity[1]) return 0.007l;
	else if (_pity <= 73) return 0.007l + 0.07l * (long double) (_pity - 62);
	return 0.777l + 0.035l * (long double) (_pity - 73);
}

static long double getWeight4W(unsigned int _pity) {
	if (_pity <= 7 || !doPity[0]) return 0.06l;
	else if (_pity == 8) return 0.66l;
	return 0.66l + 0.3l * (long double) (_pity - 8);
}

static long double getWeight(unsigned int _pity, unsigned int rare) {
	switch (rare) {
	default:
		return 0.0f;
	case 3:
		return 0.943l;
	case 4:
		return getWeight4(_pity);
	case 5:
		return getWeight5(_pity);
	}
}

static long double getWeightW(unsigned int _pity, unsigned int rare) {
	switch (rare) {
	default:
		return 0.0f;
	case 3:
		return 0.933l;
	case 4:
		return getWeight4W(_pity);
	case 5:
		return getWeight5W(_pity);
	}
}

// "Smoothening" function.
// If it's disabled, it still needs to be called, since character vs weapon still needs to be decided.
// 5-star variant, only on standard banner
static long double getWeight5S(unsigned int _pity) {
	if (!doSmooth[1]) return 0.5f;
	if (_pity <= 146) return 0.003l;
	return 0.003l + 0.03l * (long double) (_pity - 146);
}

// 4-star character/standard banner variant
static long double getWeight4S(unsigned int _pity) {
	if (!doSmooth[0]) return 0.5f;
	if (_pity <= 17) return 0.0255l;
	return 0.0255l + 0.255l * (long double) (_pity - 17);
}

// 4-star weapon banner variant
static long double getWeight4SW(unsigned int _pity) {
	if (_pity <= 14) return 0.03l;
	return 0.03l + 0.3l * (long double) (_pity - 14);
}

static long double rndFloat() {
	static long long rndBuf;
	getrandom(&rndBuf, sizeof(long long), 0);
	return fabsl((long double) rndBuf / (long double) LLONG_MAX);
}

static unsigned int doAPull(unsigned int banner, unsigned int stdPoolVersion, unsigned int* rare, unsigned int* isRateUp) {
	unsigned long long rnd;
	long double rndF;
	if (banner >= WISH_CNT) return -1;
	if (rare == NULL) return -1;
	if (isRateUp == NULL) return -1;
	long double (*_getWeight)(unsigned int, unsigned int) = getWeight;
	if (banner == WPN || banner == STD_WPN) _getWeight = getWeightW;
	if (banner != NOVICE) {
		if (stdPoolVersion > 0x41) stdPoolVersion = 0x41;
		if (stdPoolVersion >= 0x40) {
			stdPoolVersion -= 0x7;
		}
		if (stdPoolVersion >= 0x30) {
			stdPoolVersion -= 0x7;
		}
		if (stdPoolVersion >= 0x20) {
			stdPoolVersion -= 0x9;
		}
		stdPoolVersion -= 0xf;
		if ((int) stdPoolVersion < 1) stdPoolVersion = 1;
	}
	else stdPoolVersion = 0;
	if (doPity[0]) pity[0]++;
	if (doPity[1]) pity[1]++;
	if (doSmooth[0]) {
		pityS[0]++;
		pityS[1]++;
	}
	if (doSmooth[1]) {
		pityS[2]++;
		pityS[3]++;
	}
	if (!do5050) {
		getRateUp[0] = 0;
		getRateUp[1] = 0;
	}
	rndF = rndFloat();
	if (rndF <= _getWeight(pity[1], 5)) {
		*rare = 5;
		pity[1] = 0;
		switch (banner) {
		case CHAR1:
		case CHAR2:
			// Character banners don't use the stable function for 5-stars
			pityS[2] = 0;
			pityS[3] = 0;
			if (!getRateUp[1] || !do5050) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 2 == 0) {
				*isRateUp = 1;
				getRateUp[1] = 0;
				// Character banners don't use Fate Points, but best to reset them anyway
				fatePoints = 0;
				return _5StarChrUp[banner - CHAR1];
			}
			*isRateUp = 0;
			getRateUp[1] = 1;
			// Character banners don't use Fate Points, but best to set it anyway
			fatePoints++;
			getrandom(&rnd, sizeof(long long), 0);
			return _5StarChr[rnd % _5StarMaxIndex[stdPoolVersion]];
		case WPN:
			// Weapon banner does not use the stable function for 5-stars
			pityS[2] = 0;
			pityS[3] = 0;
			if (fatePoints < 2 && (!getRateUp[1] || !do5050)) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 4 < 3) {
				*isRateUp = 1;
				getRateUp[1] = 0;
				if (fatePoints >= 2) {
					fatePoints = 0;
					return epitomizedPath;
				}
				else {
					getrandom(&rnd, sizeof(long long), 0);
					if (epitomizedPath) {
						if (_5StarWpnUp[rnd % 2] == epitomizedPath) {
							fatePoints = 0;
						}
						else {
							fatePoints++;
						}
					}
					return _5StarWpnUp[rnd % 2];
				}
			}
			else {
				*isRateUp = 0;
				getRateUp[1] = 1;
				fatePoints++;
				getrandom(&rnd, sizeof(long long), 0);
				return _5StarWpn[rnd % 10];
			}
		case NOVICE:
			// Novice banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			// Novice banner does not use Fate Points
			fatePoints = 0;
			// Novice banner does not use the stable function
			pityS[2] = 0;
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _5StarChr[rnd % _5StarMaxIndex[stdPoolVersion]];
		case STD_WPN:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			// Standard banner does not use Fate Points
			fatePoints = 0;
			// There's no point to use the stable function here, because then the banner's drop rates would be nearly identical to the vanilla standard banner
			pityS[2] = 0;
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _5StarWpn[rnd % 10];
		case STD_CHR:
		default:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			// Standard banner does not use Fate Points
			fatePoints = 0;
			rndF = rndFloat();
			if (pityS[2] <= pityS[3]) {
				if (rndF <= getWeight5S(pityS[3])) {
					pityS[3] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return _5StarWpn[rnd % 10];
				}
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _5StarChr[rnd % _5StarMaxIndex[stdPoolVersion]];
			}
			if (rndF <= getWeight5S(pityS[2])) {
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _5StarChr[rnd % _5StarMaxIndex[stdPoolVersion]];
			}
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _5StarWpn[rnd % 10];
		}
	}
	else if (rndF <= _getWeight(pity[0], 4)) {
		*rare = 4;
		pity[0] = 0;
		switch (banner) {
		case CHAR1:
		case CHAR2:
			if (!getRateUp[0] || !do5050) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 2 == 0) {
				*isRateUp = 1;
				getRateUp[0] = 0;
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChrUp[rnd % 3];
			}
			*isRateUp = 0;
			getRateUp[0] = 1;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4S(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return _4StarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _4StarWpn[rnd % 18];
		case WPN:
			if (!getRateUp[0] || !do5050) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 4 < 3) {
				*isRateUp = 1;
				getRateUp[0] = 0;
				pityS[1] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarWpnUp[rnd % 5];
			}
			*isRateUp = 0;
			getRateUp[0] = 1;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4SW(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return _4StarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _4StarWpn[rnd % 18];
		case NOVICE:
			*isRateUp = 0;
			// Novice banner does not use the rate-up function
			getRateUp[0] = 0;
			// Novice banner does not use the stable function
			pityS[0] = 0;
			pityS[1] = 0;
			rndF = rndFloat();
			getrandom(&rnd, sizeof(long long), 0);
			return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
		case STD_CHR:
		default:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4S(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return _4StarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _4StarWpn[rnd % 18];
		case STD_WPN:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4SW(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return _4StarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return _4StarChr[rnd % _4StarMaxIndex[stdPoolVersion]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return _4StarWpn[rnd % 18];
		}
	}
	else {
		*isRateUp = 0;
		*rare = 3;
		getrandom(&rnd, sizeof(long long), 0);
		return _3Star[rnd % 13];
	}
}

static int shouldBold(unsigned int rare, unsigned int banner, unsigned int rateUp) {
	if (rare <= 3) return 0;
	if (banner == STD_CHR) return 1;
	if (banner == STD_WPN) return 1;
	if (banner == NOVICE) return 1;
	return rateUp ? 1 : 0;
}

static const char* const banners[WISH_CNT][2] = {
	[CHAR1] = {"char1", "Character Event Wish"},
	[CHAR2] = {"char2", "Character Event Wish-2"},
	[WPN] = {"weapon", "Weapon Event Wish"},
	[STD_CHR] = {"std", "Wanderlust Invocation"},
	[STD_WPN] = {"std_weapon", "Wanderlust Incocation (Weapons)"},
	[NOVICE] = {"novice", "Beginners' Wish"}
};

static void ver() {
		printf("Yet Another Genshin Impact Gacha Simulator v1.0\n"
		"\n©2023 Alex Pensinger (ArcticLuma113).\nThis program is released under the terms of the MPLv2, which can be viewed at:\nhttps://mozilla.org/MPL/2.0/.\n");
}

static void usage() {
	unsigned int i;
	ver();
	printf(
		"\nUsage:\n"
		"\t-b,--banner            Choose a banner type. Valid banners:\n"
		"\t                       \t"
	);
	for (i = 0; i < WISH_CNT; i++) {
		printf("%s%s", i != 0 ? ", " : "", banners[i][0]);
	}
	printf("\n"
		"\t                       \t(Required argument)\n"
		"\t-d, --details          Shows the pool of avaliable items and then exits.\n"
		"\t-p, --pulls            Specify the number of pulls to perform at once.\n"
		"\t-4, --pity4            Specify initial 4★ pity.\n"
		"\t-5, --pity5            Specify initial 5★ pity.\n"
		"\t-l, --lostRateUp4      Specify that the next 4★ is guaranteed to be the\n"
		"\t                       \trate-up item. Only relevant on the Character and\n"
		"\t                       \tWeapon Event banners.\n"
		"\t-L, --lostRateUp5      Specify that the next 5★ is guaranteed to be the\n"
		"\t                       \trate-up item. Only relevant on the Character and\n"
		"\t                       \tWeapon Event banners.\n"
		"\t-f, --fate_points      Specify the accumulated Fate Points. Only\n"
		"\t                       \trelevant on the Weapon Event banner.\n"
		"\t-e, --epitomized_path  Specify the course to chart for the Epitomized\n"
		"\t                       \tPath. Only relevant on the Weapon Event banner.\n"
		"\t-c, --noviceCnt        Specify how many pulls have been made previously.\n"
		"\t                       \tOnly relevant on the Beginners' banner and is\n"
		"\t                       \tcapped at 8.\n"
		"\t-h, --help, --usage    Show this text and exit.\n"
		"\t-v, --version          Show application version and exit.\n"
		"\nAdvanced Usage:\n"
		"\t-n, --noPity4          Disable 4★ pity.\n"
		"\t-N, --noPity5          Disable 5★ pity.\n"
		"\t-g, --noGuarantee      Disable the rate-up mechanism. Only relevant on\n"
		"\t                       \tthe Character and Weapon Event banners.\n"
		"\t                       \t(note that Epitomized Path will still work.)\n"
		"\t-V, --poolVersion      Specify the version from which the standard pool\n"
		"\t                       \tof items will be drawn from. Not relevant on the\n"
		"\t                       \tBeginners' banner, which uses a fixed pool.\n"
		"\nTuning the \"Stable Pity\" Mechanism:\n"
		"(the mechanic that prevents too many character or weapon drops in a row)\n"
		"\t--smooth4c             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ character.\n"
		"\t--smooth4w             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ weapon.\n"
		"\t--smooth5c             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ character. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t--smooth5w             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ weapon. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t-s, --noSmooth4        Disable the \"smooth\" pity mechanism for 4★\n"
		"\t                       \titems.\n"
		"\t-S, --noSmooth5        Disable the \"smooth\" pity mechanism for 5★\n"
		"\t                       \titems.\n"
		"\nDisclaimer:\n"
		"This project is not affiliated with miHoYo/Hoyoverse/Cogonosphere or any of\ntheir subsidiaries. It is designed for entertainment purposes only, and gacha\npulls made with this program can not and will not be reflected in your in-game\naccount.\n"
	);
}

typedef struct option opt_t;

static const opt_t long_opts[] = {
	{"banner", required_argument, 0, 'b'},
	{"pulls", required_argument, 0, 'p'},
	{"fate_points", required_argument, 0, 'f'},
	{"epitomized_path", required_argument, 0, 'e'},
	{"pity4", required_argument, 0, '4'},
	{"pity5", required_argument, 0, '5'},
	{"smooth4c", required_argument, 0, 1},
	{"smooth4w", required_argument, 0, 2},
	{"smooth5c", required_argument, 0, 3},
	{"smooth5w", required_argument, 0, 4},
	{"lostRateUp4", no_argument, 0, 'l'},
	{"lostRateUp5", no_argument, 0, 'L'},
	{"noSmooth5", no_argument, 0, 'S'},
	{"noSmooth4", no_argument, 0, 's'},
	{"noPity5", no_argument, 0, 'N'},
	{"noPity4", no_argument, 0, 'n'},
	{"noGuarantee", no_argument, 0, 'g'},
	{"noviceCnt", required_argument, 0, 'c'},
	{"pool_version", required_argument, 0, 'V'},
	{"details", no_argument, 0, 'd'},
	{"version", no_argument, 0, 'v'},
	{"help", no_argument, 0, 'h'},
	{"usage", no_argument, 0, 5},
	{NULL, 0, 0, 0},
};

int main(int argc, char** argv) {
	unsigned int i;
	static char buf[1024];
	int item = 11301;
	unsigned int rare = 3;
	unsigned int color = 0;
	unsigned int isChar = 0;
	unsigned int won5050 = 0;
	int banner = -1;
	unsigned int pulls = 10;
	unsigned int noviceCnt = 0;
	unsigned int detailsRequested = 0;
	int c = 0;
	long long n = 0;
	int v[4] = {-1, -1, 0, 0x41};
	char* p = NULL;
	while (1) {
		c = getopt_long(argc, argv, "4:5:LNSV:b:c:de:f:ghlnsp:v", long_opts, NULL);
		if (c == -1) break;
		switch (c) {
		case '4':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "4★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 11) {
				fprintf(stderr, "4★ pity cannot be more than 11.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "4★ pity must be numeric.\n");
				return -1;
			}
			pity[0] = n;
			break;
		case '5':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "5★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 90) {
				fprintf(stderr, "5★ pity cannot be more than 90.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "5★ pity must be numeric.\n");
				return -1;
			}
			pity[1] = n;
			break;
		case 'L':
			getRateUp[1] = 1;
			break;
		case 'N':
			doPity[1] = 0;
			break;
		case 'S':
			doSmooth[1] = 0;
			break;
		case 'V':
			n = sscanf(optarg, "%i.%i", &v[0], &v[1]);
			if (n == EOF || n == 0) {
				fprintf(stderr, "Unable to parse pool version, using %d.%d\n", v[3] >> 4, v[3] & 15);
			}
			v[2] = 1;
			if (n == 1) {
				fprintf(stderr, "Only got major pool version, using %d.0\n", v[0]);
				v[1] = 0;
			}
			break;
		case 'b':
			for (n = 0; n < WISH_CNT; n++) {
				if (strcasecmp(optarg, banners[n][0]) == 0) {
					banner = n;
					break;
				}
			}
			if (n >= WISH_CNT) {
				fprintf(stderr, "Invalid banner type \"%s\". Valid banner types:\n", optarg);
				for (n = 0; n < WISH_CNT; n++) {
					fprintf(stderr, "\t%s: %s\n", banners[n][0], banners[n][1]);
				}
				return -1;
			}
			break;
		case 'c':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "Wish count cannot be negative.\n");
				return -1;
			}
			if (n > 8) {
				n = 8;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Wish count must be numeric.\n");
				return -1;
			}
			noviceCnt = n;
			break;
		case 'd':
			detailsRequested = 1;
			break;
		case 'e':
			n = strtoull(optarg, &p, 0);
			if (n < 1 && n > 2) {
				fprintf(stderr, "Epitomized Path index is invalid.\n)");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Epitomized Path index must be numeric.\n");
				return -1;
			}
			epitomizedPath = _5StarWpnUp[n];
			break;
		case 'f':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "Fate Points cannot be negative.\n");
				return -1;
			}
			if (n > 2) {
				fprintf(stderr, "Fate Points cannot be more than 2.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Fate Points must be numeric.\n");
				return -1;
			}
			fatePoints = n;
			break;
		case 'g':
			do5050 = 0;
			break;
		case 'l':
			getRateUp[0] = 1;
			break;
		case 'n':
			doPity[0] = 0;
			break;
		case 's':
			doSmooth[0] = 0;
			break;
		case 'p':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "Pull count cannot be negative.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Pull count must be numeric.\n");
				return -1;
			}
			pulls = n;
			break;
		case 1 ... 4:
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "Stable pity cannot be negative.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Stable pity must be numeric.\n");
				return -1;
			}
			// TODO minor sanity checks, similar to main pity
			pityS[c - 1] = n;
			break;
		case 'v':
			ver();
			return 0;
		case 'h':
		case 5:
			usage();
			return 0;
		case '?':
			fprintf(stderr, "Try '%s --help' for more information.\n", program_invocation_name);
			return -1;
		default:
			break;
		}
	}
	if (optind < argc) {
		fprintf(stderr, "%s: unrecognized option '%s'\n", program_invocation_name, argv[optind]);
		fprintf(stderr, "Try '%s --help' for more information.\n", program_invocation_name);
		return -1;
	}
	if (optind <= 1) {
		usage();
		return 0;
	}
	if (banner < 0) {
		fprintf(stderr, "We need a banner to pull from!\nValid banner indexes:\n");
		for (n = 0; n < WISH_CNT; n++) {
			fprintf(stderr, "\t%s: %s\n", banners[n][0], banners[n][1]);
		}
		fprintf(stderr, "Pick one with the -b option.\n");
		return -1;
	}
	if (banner == WPN || banner == STD_WPN) {
		if (pity[0] >= 10) {
			fprintf(stderr, "4★ pity cannot be more than 10 for weapon banners.\n");
			return -1;
		}
		if (pity[1] >= 80) {
			fprintf(stderr, "5★ pity cannot be more than 80 for weapon banners.\n");
			return -1;
		}
	}
	if (detailsRequested) {
		printf("Details for the %s banner:", banners[banner][1]);
		if (banner != NOVICE && v[2]) {
			v[3] = ((v[0] & 15) << 4 | (v[1] & 15));
			printf(" (v%d.%d standard pool)", v[0] & 15, v[1] & 15);
		}
		printf("\n\n");
		if (banner != NOVICE) {
			if (v[3] > 0x41) v[3] = 0x41;
			if (v[3] >= 0x40) {
				v[3] -= 0x7;
			}
			if (v[3] >= 0x30) {
				v[3] -= 0x7;
			}
			if (v[3] >= 0x20) {
				v[3] -= 0x9;
			}
			v[3] -= 0xf;
			if ((int) v[3] < 1) v[3] = 1;
		}
		else v[3] = 0;
		if (banner == CHAR1 || banner == CHAR2) {
			item = _5StarChrUp[banner - CHAR1];
			if (chrList[item - 1000] != NULL) {
				snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
			}
			else {
				snprintf(buf, 1024, "id \e[33;1m%u\e[39;0m", item);
			}
			printf("Rate-Up 5★ Character:\n\t%s\n\n", buf);
			printf("Rate-Up 4★ Characters:\n");
			for (n = 0; n < 3; n++) {
				item = _4StarChrUp[n];
				if (chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
				}
				else {
					snprintf(buf, 1024, "id \e[35;1m%u\e[39;0m", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		else if (banner == WPN) {
			printf("Rate-Up 5★ Weapons:\n");
			for (n = 0; n < 2; n++) {
				item = _5StarWpnUp[n];
				if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33;1m%u\e[39;0m", item);
				}
				printf("\t%d: %s\n", (int) n + 1, buf);
			}
			printf("(Chart a course by passing -e x, where x is the desired index listed above.)\n\n");
			printf("Rate-Up 4★ Weapons:\n");
			for (n = 0; n < 5; n++) {
				item = _4StarWpnUp[n];
				if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35;1m%u\e[39;0m", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != WPN && banner != STD_WPN) {
			printf("5★ Character Pool:\n");
			for (n = 0; n < _5StarMaxIndex[v[3]]; n++) {
				item = _5StarChr[n];
				if (chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
				}
				else {
					snprintf(buf, 1024, "with id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != CHAR1 && banner != CHAR2 && banner != NOVICE) {
			printf("5★ Weapon Pool:\n");
			for (n = 0; n < 10; n++) {
				item = _5StarWpn[n];
				if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "with id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		printf("4★ Character Pool:\n");
		for (n = 0; n < _4StarMaxIndex[v[3]]; n++) {
			item = _4StarChr[n];
			if (chrList[item - 1000] != NULL) {
				snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
			}
			else {
				snprintf(buf, 1024, "with id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
			}
			printf("\t%s\n", buf);
		}
		printf("\n");
		if (banner != NOVICE) {
			printf("4★ Weapon Pool:\n");
			for (n = 0; n < 18; n++) {
				item = _4StarWpn[n];
				if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "with id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		printf("3★ Weapon Pool:\n");
		for (n = 0; n < 13; n++) {
			item = _3Star[n];
			if (getWeapon(item) != NULL) {
				snprintf(buf, 1024, "\e[34;22m%s\e[39;0m (id %u)", getWeapon(item), item);
			}
			else {
				snprintf(buf, 1024, "with id \e[34;22m%u\e[39;0m", item);
			}
			printf("\t%s\n", buf);
		}
		return 0;
	}
	fprintf(stderr, "Making %u wishes on the %s banner", pulls, banners[banner][1]);
	if (banner != NOVICE && v[2]) {
		v[3] = ((v[0] & 15) << 4 | (v[1] & 15));
		fprintf(stderr, " (v%d.%d standard pool)", v[0] & 15, v[1] & 15);
	}
	fprintf(stderr, "\n\n");
	for (i = 0; i < pulls; i++) {
		if (banner == NOVICE && i == (7 - noviceCnt)) { // 8th wish is always Noelle on novice banner
			rare = 4;
			item = 1034;
			won5050 = 0;
			getRateUp[0] = 0;
			pity[0] = 0;
			pity[1]++;
			pityS[0] = 0;
			pityS[1] = 0;
		}
		else item = doAPull(banner, v[3], &rare, &won5050);
		if (item < 0) {
			fprintf(stderr, "Pull #%u failed (retcode = %d)\n", i + 1, item);
			break;
		}
		switch (rare) {
		case 0:
		case 1:
		default:
			color = 39;
			break;
		case 2:
			color = 32;
			break;
		case 3:
			color = 34;
			break;
		case 4:
			color = 35;
			break;
		case 5:
			color = 33;
			break;
		}
		if (item <= 1000 + MAX_CHARS && item >= 1000) {
			isChar = 1;
			if (chrList[item - 1000] != NULL) {
				snprintf(buf, 1024, "\e[%u%sm%s\e[39;0m (id %u)", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", chrList[item - 1000], item);
			}
			else {
				snprintf(buf, 1024, "with id \e[%u%sm%u\e[39;0m", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		else {
			isChar = 0;
			if (getWeapon(item) != NULL) {
				snprintf(buf, 1024, "\e[%u%sm%s\e[39;0m (id %u)", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", getWeapon(item), item);
			}
			else {
				snprintf(buf, 1024, "with id \e[%u%sm%u\e[39;0m", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		printf("Pull %u: %u★ %s %s\n", i + 1, rare, isChar ? "Character" : "Weapon", buf);
	}
	printf("\nResults after last pull:\n");
	if (doPity[0]) {
		printf("\n4★ pity: %u", pity[0]);
	}
	if (doPity[1]) {
		printf("\n5★ pity: %u\n", pity[1]);
	}
	else printf("\n");
	if (do5050 && (banner == CHAR1 || banner == CHAR2 || banner == WPN)) {
		printf("\n4★ guaranteed: %u\n", getRateUp[0] ? 1 : 0);
		printf("5★ guaranteed: %u\n", getRateUp[1] ? 1 : 0);
	}
	if (banner == WPN && epitomizedPath) {
		printf("Fate Points: %u\n", fatePoints);
	}
	if (doSmooth[0] && banner != NOVICE) {
		printf("\n4★ stable val (characters): %u\n", pityS[0]);
		printf("4★ stable val (weapons): %u", pityS[1]);
	}
	if (doSmooth[1] && (banner == STD_CHR || banner == STD_WPN)) {
		printf("\n5★ stable val (characters): %u\n", pityS[2]);
		printf("5★ stable val (weapons): %u\n", pityS[3]);
	}
	else printf("\n");
	return 0;
}
