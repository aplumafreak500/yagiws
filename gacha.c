/* Yet another Genshin Impact wish simulator */
/* ©2023 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

/*
Release History:
	* v1.0 (2023-10-20):
		* Initial release
	* v1.0.1 (2023-11-04):
		* Add 4.1 weapons and initial support for 4.2
		* Fix getWeight4SW using smooth weight even when smooth function is disabled
	* v1.1 (2023-11-05):
		* Add support for multiple banners. Choose one with the -B switch.
		* Debug mode that disables most sanity checks. (Beware of segfaults!)
		* Extra sanity checks for displaying characters in Wish Details
		* Check for a valid weapon ID, even on character banners (and vice versa) in Wish Details
		* Fix getWeapon not returning a valid weapon if its ID is not 1xxxx
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

// 0xffff: No second banner
static const unsigned short FiveStarChrUp[58][2] = {
	// v1.0 - Launch
	{1022, 0xffff}, // Venti
	{1029, 0xffff}, // Klee
	// v1.1 - Unreconciled Stars & Foul Legacy
	{1033, 0xffff}, // Tartaglia
	{1030, 0xffff}, // Zhongli
	// v1.2 - Dragonspine
	{1038, 0xffff}, // Albedo
	{1037, 0xffff}, // Ganyu
	// v1.3 - Lantern Rite - Special case, ran three character banners with no second banners, but only ran two weapon banners. Treat as a 4-phased banner cycle
	{1026, 0xffff}, // Xiao
	{1042, 0xffff}, // Keqing (first half)
	{1042, 0xffff}, // Keqing (second half)
	{1046, 0xffff}, // Hu Tao
	// v1.4 - Windblume
	{1022, 0xffff},
	{1033, 0xffff},
	// v1.5 - Serenitea Pot
	{1030, 0xffff},
	{1051, 0xffff}, // Eula
	// v1.6 - Golden Apple Archipelago
	{1029, 0xffff},
	{1047, 0xffff}, // Kazuha
	// v2.0 - Inazuma
	{1002, 0xffff}, // Ayaka
	{1049, 0xffff}, // Yoimiya
	// v2.1 - Plane of Euthymia & Watatsumi
	{1052, 0xffff}, // Raiden
	{1054, 0xffff}, // Kokomi
	// v2.2 - Tsurumi
	{1033, 0xffff},
	{1046, 0xffff},
	// v2.3 - Dragonspine 2 - Secondary banners start here
	{1038, 1051},
	{1057, 0xffff}, // Itto
	// v2.4 - Enkanomiya & Lantern Rite 2
	{1063, 1026}, // Shenhe
	{1030, 1037},
	// v2.5 - Three Realms Gateway Offering
	{1058, 0xffff}, // Miko
	{1052, 1054},
	// v2.6 - Chasm
	{1066, 1022}, // Ayato
	{1002, 0xffff},
	// v2.7 - Chasm 2
	{1060, 1026}, // Yelan
	{1057, 0xffff},
	// v2.8	- Golden Apple Archipelago 2
	{1029, 1047},
	{1049, 0xffff},
	// v3.0 - Sumeru
	{1069, 1030}, // Tighnari
	{1037, 1054},
	// v3.1 - Sumeru Desert
	{1071, 1022}, // Cyno
	{1070, 1038}, // Nilou
	// v3.2 - Akasha Pulses
	{1073, 1049}, // Nahida
	{1058, 1033},
	// v3.3 - Genius Invokation TCG
	{1075, 1057}, // Wanderer
	{1052, 1066},
	// v3.4 - Lantern Rite 3
	{1078, 1026}, // Alhathiam
	{1046, 1060},
	// v3.5 - Windblume 2
	{1079, 1071}, // Deyha
	{1063, 1002},
	// v3.6 - Parade of Providence & Girdle of the Sands
	{1073, 1070},
	{1082, 1037}, // Baizhu
	// v3.7 - The Summoners' Summit
	{1049, 1058},
	{1078, 1047},
	// v3.8 - Secret Summer Paradise
	{1051, 1029},
	{1054, 1075},
	// v4.0 - Fontaine
	{1084, 1060}, // Lyney
	{1030, 1033},
	// v4.1 - Fortreess of Meropide & Waterborne Poetry
	{1087, 1046}, // Neuvillette
	{1086, 1022}, // Wriothesley
	// v4.2 - Masquerade of the Guilty (TODO verify Furina's ID)
	{1088, 1082}, // Furina
	{1071, 1066},
};

static const unsigned short FourStarChrUp[58][3] = {
	// v1.0 - Launch
	{1023, 1031, 1014}, // Xiangling, Fischl, Barbara
	{1025, 1034, 1043}, // Xingqiu, Noelle, Sucrose
	// v1.1 - Unreconciled Stars & Foul Legacy
	{1027, 1024, 1039}, // Ningguang, Beidou, *Diona*
	{1044, 1036, 1020}, // *Xinyan*, Chongyun, Razor
	// v1.2 - Dragonspine
	{1031, 1043, 1032}, // Bennett
	{1023, 1025, 1034},
	// v1.3 - Lantern Rite - Special case, ran three character banners with no second banners, but only ran two weapon banners. Treat as a 4-phased banner cycle
	{1039, 1024, 1044},
	{1027, 1032, 1014}, // first half
	{1027, 1032, 1014}, // second half
	{1025, 1036, 1023},
	// v1.4 - Windblume
	{1043, 1020, 1034},
	{1014, 1031, 1045}, // Rosaria
	// v1.5 - Serenitea Pot
	{1048, 1034, 1039}, // Yanfei
	{1044, 1025, 1024},
	// v1.6 - Golden Apple Archipelago
	{1014, 1043, 1031},
	{1045, 1032, 1020},
	// v2.0 - Inazuma
	{1027, 1036, 1048},
	{1053, 1039, 1044}, // Sayu
	// v2.1 - Plane of Euthymia & Watatsumi
	{1056, 1023, 1043}, // Kujou Sara
	{1045, 1024, 1025},
	// v2.2 - Tsurumi
	{1027, 1036, 1048},
	{1050, 1039, 1053}, // Thoma
	// v2.3 - Dragonspine 2
	{1032, 1034, 1045},
	{1055, 1014, 1023}, // Gorou
	// v2.4 - Enkanomiya & Lantern Rite 2
	{1064, 1027, 1036}, // Yun Jin
	{1025, 1024, 1048},
	// v2.5 - Three Realms Gateway Offering
	{1031, 1039, 1050},
	{1044, 1056, 1032},
	// v2.6 - Chasm
	{1023, 1043, 1064},
	{1045, 1053, 1020},
	// v2.7 - Chasm 2
	{1048, 1014, 1034},
	{1036, 1055, 1065}, // Kuki Shinobu
	// v2.8	- Golden Apple Archipelago 2
	{1027, 1059, 1050},
	{1064, 1044, 1032},
	// v3.0 - Sumeru
	{1067, 1039, 1031}, // Collei
	{1025, 1043, 1068}, // Dori
	// v3.1 - Sumeru Desert
	{1065, 1053, 1072}, // Candace
	{1024, 1014, 1023},
	// v3.2 - Akasha Pulses
	{1032, 1020, 1034},
	{1074, 1059, 1050}, // Layla
	// v3.3 - Genius Invokation TCG
	{1076, 1055, 1048}, // Faruzan
	{1045, 1053, 1056},
	// v3.4 - Lantern Rite 3 & Desert of Hadramaveth
	{1044, 1064, 1077}, // Yaoyao
	{1025, 1027, 1024},
	// v3.5 - Windblume 2
	{1067, 1014, 1032},
	{1080, 1043, 1039}, // Mika
	// v3.6 - Parade of Providence & Girdle of the Sands
	{1065, 1068, 1074},
	{1081, 1031, 1072}, // Kaveh
	// v3.7 - The Summoners' Summit
	{1036, 1064, 1061}, // Kiara
	{1077, 1023, 1059},
	// v3.8 - Secret Summer Paradise
	{1080, 1020, 1050},
	{1076, 1045, 1048},
	// v4.0 - Fontaine
	{1083, 1032, 1014}, // Lynette
	{1053, 1034, 1085}, // Freminet
	// v4.1 - Fortreess of Meropide & Waterborne Poetry
	{1025, 1031, 1039},
	{1068, 1036, 1050},
	// v4.2 - Masquerade of the Guilty (TODO verify Charolette's ID and the full list of featured characters)
	{1006, 1015, 1089}, // Charolette
	{1006, 1015, 1021},
};

static const unsigned short FiveStarWpnUp[58][2] = {
	// v1.0 - Launch
	{15502, 15501}, // Amos' Bow/Aquila Favonia
	{14502, 12502}, // Lost Prayer to the Sacred Winds/Wolf's Gravestone
	// v1.1 - Unreconciled Stars & Foul Legacy
	{15501, 14504}, // Skyward Harp/Memory of Dust
	{13504, 12504}, // Vortex Vanquisher/The Unforged
	// v1.2 - Dragonspine
	{11504, 14501}, // Summit Shaper/Skyward Atlas
	{15502, 12501}, // Skyward Pride
	// v1.3 - Lantern Rite - Special case, ran three character banners with no second banners, but only ran two weapon banners. Treat as a 4-phased banner cycle
	{11505, 13505}, // Primordial Jade Cutter/Primordial Jade-Winged Spear (1st half)
	{11505, 13505}, // Primordial Jade Cutter/Primordial Jade-Winged Spear (2nd half)
	{13501, 12502}, // Staff of Homa (1st half)
	{13501, 12502}, // Staff of Homa (2nd half)
	// v1.4 - Windblume
	{15503, 11502}, // Elegy for the End/Skyward Blade
	{15501, 14502},
	// v1.5 - Serenitea Pot
	{11504, 14504},
	{12503, 11501}, // Song of Broken Pines
	// v1.6 - Golden Apple Archipelago
	{14502, 12501},
	{11503, 14501}, // Freedom-Sworn
	// v2.0 - Inazuma - Epitomized Path starts here
	{11509, 13502}, // Mistsplitter Reforged
	{15509, 11502}, // Thundering Pulse
	// v2.1 - Plane of Euthymia & Watatsumi
	{13509, 12504}, // Engulfing Lightning
	{11505, 14506}, // Everlasting Moonglow
	// v2.2 - Tsurumi
	{14504, 15507}, // Polar Star
	{13501, 15503},
	// v2.3 - Dragonspine 2
	{11503, 12503},
	{12510, 15501}, // Redhorn Stonethresher
	// v2.4 - Enkanomiya & Lantern Rite 2
	{13507, 13505}, // Calamity Queller
	{13504, 15502},
	// v2.5 - Three Realms Gateway Offering
	{14509, 11505}, // Kagura's Verity
	{13509, 14506},
	// v2.6 - Chasm
	{11510, 15503}, // Haran Geppaku Futsu
	{11509, 12504},
	// v2.7 - Chasm 2
	{15508, 13505}, // Aqua Simulacara
	{12510, 14504},
	// v2.8	- Golden Apple Archipelago 2
	{11503, 14502},
	{15509, 11504},
	// v3.0 - Sumeru
	{15511, 13504}, // Hunter's Path
	{15502, 14506},
	// v3.1 - Sumeru Desert
	{13511, 15503}, // Staff of the Scarlet Sands
	{11511, 11505}, // Key of Khaj-Nisut
	// v3.2 - Akasha Pulses
	{14511, 15509}, // A Thousand Floating Dreams
	{14509, 15507},
	// v3.3 - Genius Invokation TCG
	{14512, 12510}, // Tulaytullah's Rememberance
	{13509, 11510}, // Haran Geppaku Futsu
	// v3.4 - Lantern Rite 3
	{11512, 13505}, // Light of Foliar Incision
	{13501, 15508},
	// v3.5 - Windblume 2
	{12511, 13511}, // Beacon of the Reed Sea
	{13501, 15508},
	// v3.6 - Parade of Providence & Girdle of the Sands
	{11511, 14511},
	{14505, 15502}, // Jadefall's Splendor
	// v3.7 - The Summoners' Summit
	{15509, 14509},
	{11503, 11512},
	// v3.8 - Secret Summer Paradise
	{12503, 14502},
	{14512, 14506},
	// v4.0 - Fontaine
	{15512, 15508}, // The First Great Magic
	{13504, 15507},
	// v4.1 - Fortreess of Meropide & Waterborne Poetry
	{14514, 13501}, // Tome of the Eternal Flow
	{14513, 15503}, // Cashflow Supervision
	// v4.2 - Masquerade of the Guilty (TODO verify Splendor of Tranquil Waters' ID and the actual rate-up weapons)
	{11513, 14505}, // Splendor of Tranquil Waters
	{13511, 11510},
};

static const unsigned short FourStarWpnUp[58][5] = {
	// v1.0 - Launch
	{13407, 11402, 12402, 15402, 14402}, // Favonius Lance, The Flute, The Bell, The Stringless, The Widsith
	{13401, 11403, 12403, 14403, 15403}, // Dragon's Bane, Sacrificial Sword, Sacrificial Greatsword, Sacrificial Fragments, Sacrificial Bow
	// v1.1 - Unreconciled Stars & Foul Legacy
	{15405, 11402, 14409, 13407, 12405}, // Rust, Wine and Song, Rainslasher
	{12402, 11405, 13401, 14401, 15401}, // Lion's Roar, Favonius Codex, Favonius Warbow
	// v1.2 - Dragonspine
	{12401, 13407, 11405, 15402, 14402}, // Favonius Greatsword
	{13401, 14409, 12402, 11403, 15401}, // Eye of Perception
	// v1.3 - Lantern Rite - Special case, ran three character banners with no second banners, but only ran two weapon banners. Treat as a 4-phased banner cycle
	{14409, 13407, 11402, 15405, 12403}, // first half
	{14409, 13407, 11402, 15405, 12403}, // second half
	{11405, 12410, 13406, 14402, 15403}, // Lithic Blade, Lithic Spear (first half)
	{11405, 12410, 13406, 14402, 15403}, // Lithic Blade, Lithic Spear (second half)
	// v1.4 - Windblume
	{13401, 12401, 15401, 11410, 14410}, // Alley Flash
	{15410, 11401, 13407, 14401, 12403}, // Alley Hunter
	// v1.5 - Serenitea Pot
	{12410, 13406, 15403, 11402, 14409},
	{13401, 14403, 11403, 15405, 12405},
	// v1.6 - Golden Apple Archipelago
	{13407, 11405, 15412, 12402, 14402}, // Mitternachts Waltz
	{15410, 13401, 12401, 11410, 14410},
	// v2.0 - Inazuma
	{14401, 11401, 13407, 15402, 12403},
	{13401, 15401, 12405, 11403, 14403},
	// v2.1 - Plane of Euthymia & Watatsumi
	{12402, 11405, 14402, 15403, 13407},
	{13401, 14401, 12401, 11402, 15402},
	// v2.2 - Tsurumi
	{13407, 14409, 15405, 11401, 12416}, // Akuoumaru
	{11403, 12405, 13416, 14402, 15416}, // Wavebreaker's Fin
	// v2.3 - Dragonspine 2
	{11405, 12403, 13401, 14410, 15410},
	{11410, 12402, 13407, 14403, 15412},
	// v2.4 - Enkanomiya & Lantern Rite 2
	{11402, 12401, 13406, 14402, 15401},
	{11401, 12410, 13401, 14401, 15403},
	// v2.5 - Three Realms Gateway Offering
	{11403, 12405, 13416, 14409, 15402},
	{12416, 15416, 11405, 13407, 14403},
	// v2.6 - Chasm
	{15405, 11402, 14402, 13401, 12403},
	{11401, 12402, 13407, 14401, 15401},
	// v2.7 - Chasm 2
	{13406, 11403, 12401, 14409, 15403},
	{12410, 11405, 13401, 14403, 15402},
	// v2.8	- Golden Apple Archipelago 2
	{11410, 12405, 13407, 14402, 15412},
	{14410, 15410, 11402, 12403, 13401},
	// v3.0 - Sumeru
	{11401, 12402, 13407, 14401, 15402},
	{11403, 12401, 13401, 14409, 15405},
	// v3.1 - Sumeru Desert
	{11405, 12415, 13407, 14403, 15401}, // Makhaira Aquamarine
	{11418, 12405, 13401, 14416, 15403}, // Xiphos' Moonlight, Wandering Evenstar
	// v3.2 - Akasha Pulses
	{11402, 12403, 13407, 14402, 15405},
	{11401, 12402, 13401, 14401, 15402},
	// v3.3 - Genius Invokation TCG
	{11403, 12401, 13416, 14409, 15401},
	{12416, 15416, 11405, 13407, 14403}, // Mouun's Moon
	// v3.4 - Lantern Rite 3
	{13406, 11402, 12405, 14402, 15403},
	{12410, 11401, 13401, 14401, 15405},
	// v3.5 - Windblume 2
	{11410, 15410, 12403, 13401, 14409},
	{14410, 11403, 12402, 13407, 15401},
	// v3.6 - Parade of Providence & Girdle of the Sands
	{11418, 12401, 13401, 14403, 15402},
	{12415, 14416, 11405, 13407, 15403},
	// v3.7 - The Summoners' Summit
	{12416, 11402, 13401, 14402, 15405},
	{13416, 15416, 11401, 12403, 14401},
	// v3.8 - Secret Summer Paradise
	{11410, 15410, 12405, 13407, 14409},
	{11405, 12402, 14410, 13401, 15401},
	// v4.0 - Fontaine
	{11403, 12410, 15403, 13407, 14403},
	{11402, 12413, 13401, 14402, 15405},
	// v4.1 - Fortreess of Meropide & Waterborne Poetry
	{11427, 12427, 15412, 13407, 14401}, // The Dockhand's Assistant, Portable Power Saw
	{15427, 13427, 11401, 12405, 14409}, // Range Gauge, Prospector's Drill
	// v4.2 - Masquerade of the Guilty (TODO get the full list of rate-up weapons)
	{11401, 12401, 13401, 14401, 15501},
	{11401, 12401, 13401, 14401, 15501},
};

// The list of 3-star weapons in the pool.
static const unsigned short ThreeStar[13] = {11301, 11302, 11306, 12301, 12302, 12305, 13303, 14301, 14302, 15304, 15301, 15302, 15304};

// The list of 4-star characters in the pool, along with the rate-up drops.
static const unsigned short FourStarChr[34] = {
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
	1083, 1085,
	// v4.3: Charolette
	1089, // TODO: verify once 4.2 releases
};

// The list of 4-star weapons in the pool, along with the rate-up drops.
static const unsigned short FourStarWpn[18] = {11401, 11402, 11403, 11405, 12401, 12402, 12403, 12405, 13401, 13407, 14401, 14402, 14403, 14409, 15401, 15402, 15403, 15405};

// The list of 5-star characters in the pool, along with the rate-up drops.
static const unsigned short FiveStarChr[7] = {
	// v1.0 standard pool: Jean, Diluc, Qiqi, Mona, Keqing
	1003, 1016, 1035, 1041, 1042,
	// v3.1: Tighnari
	1069,
	// v3.6: Deyha
	1079,
};

// The list of 5-star weapons in the pool, along with the rate-up drops (only on the weapon and standard banners)
static const unsigned short FiveStarWpn[10] = {11501, 11502, 12501, 12502, 13502, 13505, 14501, 14502, 15501, 15502};

// Max indexes into FourStarChr per version (for old banners)
static const unsigned char FourStarMaxIndex[30] = {
	/* Novice */ 10,
	/* v1.x */ 11, 11, 13, 13, 13, 14, 15,
	/* v2.x */ 15, 16, 17, 18, 19, 20, 20, 20, 21,
	/* v3.x */ 22, 24, 25, 26, 27, 28, 29, 30, 31,
	/* v4.x */ 31, 33, 33, 34,
};

// Max indexes into FiveStarChr per version (for old banners)
static const unsigned char FiveStarMaxIndex[30] = {
	/* Novice */ 5,
	/* v1.x */ 5, 5, 5, 5, 5, 5, 5,
	/* v2.x */ 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* v3.x */ 5, 6, 6, 6, 6, 6, 7, 7, 7,
	/* v4.x */ 7, 7, 7, 7,
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

#define MAX_CHARS 90

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
	[87] = "Neuvillette",
	[88] = "Furina", // TODO verify once 4.2 releases
	[89] = "Charolette", // TODO verify once 4.2 releases
};

enum {
	SWORD = 1,
	CLAYMORE = 2,
	POLEARM = 3,
	CATALYST = 4,
	BOW = 5,
};

// Weapon data is present, even for 1-star and 2-star weapons, for the sake of completeness.

static const char* const OneStarSwords[1] = {"Dull Blade"};
static const char* const OneStarClaymores[1] = {"Waster Greatsword"};
static const char* const OneStarPolearms[1] = {"Beginner's Protector"};
static const char* const OneStarCatalysts[1] = {"Apprentice's Notes"};
static const char* const OneStarBows[1] = {"Hunter's Bow"};

static const char* const* const OneStarWeapons[6] = {
	[SWORD] = OneStarSwords,
	[CLAYMORE] = OneStarClaymores,
	[POLEARM] = OneStarPolearms,
	[CATALYST] = OneStarCatalysts,
	[BOW] = OneStarBows,
};

static const char* const TwoStarSwords[1] = {"Silver Sword"};
static const char* const TwoStarClaymores[1] = {"Old Merc's Pal"};
static const char* const TwoStarPolearms[1] = {"Iron Point"};
static const char* const TwoStarCatalysts[1] = {"Pocket Grimoire"};
static const char* const TwoStarBows[1] = {"Seasoned Hunter's Bow"};

static const char* const* const TwoStarWeapons[6] = {
	NULL,
	TwoStarSwords,
	TwoStarClaymores,
	TwoStarPolearms,
	TwoStarCatalysts,
	TwoStarBows,
};

static const char* const ThreeStarSwords[6] = {
	"Cool Steel",
	"Harbinger of Dawn",
	"Traveler's Handy Sword",
	"Dark Iron Sword",
	"Fillet Blade",
	"Skyrider Sword"
};
static const char* const ThreeStarClaymores[6] = {
	"Ferrous Shadow",
	"Bloodtainted Greatsword",
	"White Iron Greatsword",
	"Quartz (beta?)",
	"Debate Club",
	"Skyrider Greatsword"
};
static const char* const ThreeStarPolearms[4] = {
	"White Tassel",
	"Halberd",
	"Black Tassel",
	"Flagstaff"
};
static const char* const ThreeStarCatalysts[6] = {
	"Magic Guide",
	"Thrilling Tales of Dragon Slayers",
	"Otherworldly Story",
	"Emerald Orb",
	"Twin Nephrite",
	"Amber Bead (beta?)"
};
static const char* const ThreeStarBows[6] = {
	"Raven Bow",
	"Sharpshooter's Oath",
	"Recurve Bow",
	"Slingshot",
	"Messenger",
	"Ebony Bow (beta?)"
};

static const char* const* const ThreeStarWeapons[6] = {
	NULL,
	ThreeStarSwords,
	ThreeStarClaymores,
	ThreeStarPolearms,
	ThreeStarCatalysts,
	ThreeStarBows,
};

static const char* const FourStarSwords[27] = {
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
	"Fleuve Cendre Ferryman",
	[26] = "The Dockhand's Assistant",
};

static const char* const FourStarClaymores[27] = {
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
	[24] = "Tidal Shadow",
	[26] = "Portable Power Saw",
};

static const char* const FourStarPolearms[27] = {
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
	[24] = "Rightful Rewards",
	[26] = "Prospector's Drill",
};

static const char* const FourStarCatalysts[27] = {
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
	[24] = "Flowing Purity",
	[26] = "Ballad of the Boundless Blue", // TODO verify the ID
};

static const char* const FourStarBows[27] = {
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
	[24] = "Song of Stillness",
	[26] = "Range Gauge",
};

static const char* const* const FourStarWeapons[6] = {
	NULL,
	FourStarSwords,
	FourStarClaymores,
	FourStarPolearms,
	FourStarCatalysts,
	FourStarBows,
};

static const char* const FiveStarSwords[13] = {
	"Aquila Favonia",
	"Skyward Blade",
	"Freedom-Sworn",
	"Summit Shaper",
	"Primordial Jade Cutter",
	"Primordial Jade Cutter (2)",
	"One Side",
	NULL,
	"Mistsplitter Reforged",
	"Haran Geppaku Futsu",
	"Key of Khaj-Nisut",
	"Light of Foliar Incision",
	"Splendor of Tranquil Waters", // TODO verify once 4.2 releases
};

static const char* const FiveStarClaymores[11] = {
	"Skyward Pride",
	"Wolf's Gravestone",
	"Song of Broken Pines",
	"The Unforged",
	"Primordial Jade Greatsword",
	"Other Side",
	NULL,
	NULL,
	NULL,
	"Redhorn Stonethresher",
	"Beacon of the Reed Sea",
};

static const char* const FiveStarPolearms[11] = {
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
	"Staff of the Scarlet Fields",
};

static const char* const FiveStarCatalysts[14] = {
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
	"Tulaytullah's Rememberance",
	"Cashflow Supervision",
	"Tome of the Eternal Flow",
};

static const char* const FiveStarBows[12] = {
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
	"The First Great Magic",
};

static const char* const* const FiveStarWeapons[6] = {
	NULL,
	FiveStarSwords,
	FiveStarClaymores,
	FiveStarPolearms,
	FiveStarCatalysts,
	FiveStarBows,
};

static const char* const* const* const Weapons[6] = {
	NULL,
	OneStarWeapons,
	TwoStarWeapons,
	ThreeStarWeapons,
	FourStarWeapons,
	FiveStarWeapons,
};

static const char* getWeapon(unsigned int _id) {
	if (_id / 10000 != 1) return NULL;
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
	case 21:
	case 31:
	case 41:
	case 51:
	case 12:
	case 22:
	case 32:
	case 42:
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
	case 24:
	case 34:
	case 44: // TODO verify max catalyst id
	case 54:
		maxId = 27;
		break;
	case 15: // TODO verify once 4.2 releases
		maxId = 13;
		break;
	case 25:
	case 35:
		maxId = 11;
		break;
	case 45:
		maxId = 14;
		break;
	case 55:
		maxId = 12;
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
	if (!doSmooth[0]) return 0.5f;
	if (_pity <= 14) return 0.03l;
	return 0.03l + 0.3l * (long double) (_pity - 14);
}

static long double rndFloat() {
	static long long rndBuf;
	getrandom(&rndBuf, sizeof(long long), 0);
	return fabsl((long double) rndBuf / (long double) LLONG_MAX);
}

#ifndef DEBUG
static unsigned int doAPull(unsigned int banner, unsigned int stdPoolIndex, unsigned int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#else
static unsigned int doAPull(unsigned int banner, int stdPoolIndex, int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#endif
	unsigned long long rnd;
	long double rndF;
	if (banner >= WISH_CNT) return -1;
	if (rare == NULL) return -1;
	if (isRateUp == NULL) return -1;
	long double (*_getWeight)(unsigned int, unsigned int) = getWeight;
	if (banner == WPN || banner == STD_WPN) _getWeight = getWeightW;
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
				return FiveStarChrUp[bannerIndex][banner - CHAR1];
			}
			*isRateUp = 0;
			getRateUp[1] = 1;
			// Character banners don't use Fate Points, but best to set it anyway
			fatePoints++;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarChr[rnd % FiveStarMaxIndex[stdPoolIndex]];
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
						if (FiveStarWpnUp[bannerIndex][rnd % 2] == epitomizedPath) {
							fatePoints = 0;
						}
						else {
							fatePoints++;
						}
					}
					return FiveStarWpnUp[bannerIndex][rnd % 2];
				}
			}
			else {
				*isRateUp = 0;
				getRateUp[1] = 1;
				fatePoints++;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarWpn[rnd % 10];
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
			return FiveStarChr[rnd % FiveStarMaxIndex[stdPoolIndex]];
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
			return FiveStarWpn[rnd % 10];
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
					return FiveStarWpn[rnd % 10];
				}
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarChr[rnd % FiveStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight5S(pityS[2])) {
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarChr[rnd % FiveStarMaxIndex[stdPoolIndex]];
			}
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarWpn[rnd % 10];
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
				return FourStarChrUp[bannerIndex][rnd % 3];
			}
			*isRateUp = 0;
			getRateUp[0] = 1;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4S(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
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
				return FourStarWpnUp[bannerIndex][rnd % 5];
			}
			*isRateUp = 0;
			getRateUp[0] = 1;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4SW(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case NOVICE:
			*isRateUp = 0;
			// Novice banner does not use the rate-up function
			getRateUp[0] = 0;
			// Novice banner does not use the stable function
			pityS[0] = 0;
			pityS[1] = 0;
			rndF = rndFloat();
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
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
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case STD_WPN:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4SW(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		}
	}
	else {
		*isRateUp = 0;
		*rare = 3;
		getrandom(&rnd, sizeof(long long), 0);
		return ThreeStar[rnd % 13];
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
		printf("Yet Another Genshin Impact Gacha Simulator v1.1\n"
		"\n©2023 Alex Pensinger (ArcticLuma113).\nThis program is released under the terms of the MPLv2, which can be viewed at:\nhttps://mozilla.org/MPL/2.0/.\n");
}

static void usage() {
	unsigned int i;
	ver();
	printf(
		"\nUsage:\n"
		"\t-b, --banner           Choose a banner type. Valid banners:\n"
		"\t                       \t"
	);
	for (i = 0; i < WISH_CNT; i++) {
		printf("%s%s", i != 0 ? ", " : "", banners[i][0]);
	}
	printf("\n"
		"\t                       \t(Required argument)\n"
		"\t-B, --banner_version   Choose a specific banner according to the version\n"
		"\t                       \tit appeared in. Format is as follows:\n"
		"\t                       \t<major>.<minor>.<phase>\n"
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
	{"banner_version", required_argument, 0, 'B'},
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
	int epitomizedPathIndex = -1;
	int c = 0;
	long long n = 0;
	int v[4] = {-1, -1, 0, 0x42};
	int b[5] = {-1, -1, -1, 0, 0x411};
	char* p = NULL;
	while (1) {
		c = getopt_long(argc, argv, "4:5:B:LNSV:b:c:de:f:ghlnsp:v", long_opts, NULL);
		if (c == -1) break;
		switch (c) {
		case '4':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "4★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 11) {
				fprintf(stderr, "4★ pity cannot be more than 11.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "4★ pity must be numeric.\n");
				return -1;
			}
			pity[0] = n;
			break;
		case '5':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "5★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 90) {
				fprintf(stderr, "5★ pity cannot be more than 90.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "5★ pity must be numeric.\n");
				return -1;
			}
			pity[1] = n;
			break;
		case 'B':
			n = sscanf(optarg, "%i.%i.%i", &b[0], &b[1], &b[2]);
			if (n == EOF || n == 0) {
				fprintf(stderr, "Unable to parse banner version, using %d.%d.%d\n", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
			}
			b[3] = 1;
			if (n == 2) {
				if (b[0] == (b[4] >> 8) && b[1] == ((b[4] >> 4) & 0xf)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, "Did not get banner phase, using %d.%d.%d\n", b[0], b[1], b[2]);
			}
			else if (n == 1) {
				b[1] = (b[4] >> 4) & 0xf;
				if (b[0] == (b[4] >> 8)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, "Only got banner major version, using %d.0.%d\n", b[0], b[2]);
			}
#ifndef DEBUG
			if (b[2] > ((b[0] == 1 && b[1] == 3) ? 4 : 2) || b[2] < 1) {
				fprintf(stderr, "Invalid banner phase %d (only 1%s accepted)\n", b[2], (b[0] == 1 && b[1] == 3) ? "-4" : " or 2");
				return -1;
			}
#endif
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
				fprintf(stderr, "Unable to parse pool version, using %d.%d\n", v[3] >> 4, v[3] & 0xf);
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
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Wish count cannot be negative.\n");
				return -1;
			}
			if (n > 8) {
				n = 8;
			}
#endif
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
#ifndef DEBUG
			if (n < 1 && n > 2) {
				fprintf(stderr, "Epitomized Path index is invalid.\n)");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Epitomized Path index must be numeric.\n");
				return -1;
			}
			epitomizedPathIndex = n;
			break;
		case 'f':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Fate Points cannot be negative.\n");
				return -1;
			}
			if (n > 2) {
				fprintf(stderr, "Fate Points cannot be more than 2.\n");
				return -1;
			}
#endif
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
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Stable pity cannot be negative.\n");
				return -1;
			}
#endif
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
#ifndef DEBUG
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
#endif
	if (b[3]) {
#ifndef DEBUG
		b[4] = (((b[0] & 0xf) << 8) | ((b[1] & 0xf) << 4) | (b[2] & 0xf));
#else
		b[4] = ((b[0] << 8) | (b[1] << 4) | b[2]);
#endif
	}
	b[0] = b[4] & 0xf;
	b[0]--;
	b[1] = b[4] >> 4;
#ifndef DEBUG
	if (b[1] > 0x42) b[1] = 0x42;
#endif
	if (b[1] >= 0x40) {
		b[1] -= 0x7;
	}
	if (b[1] >= 0x30) {
		b[1] -= 0x7;
	}
	if (b[1] >= 0x20) {
		b[1] -= 0x9;
	}
	if (b[1] >= 0x14) {
		b[1] += 0x1;
	}
	b[1] -= 0x10;
#ifndef DEBUG
	if (b[1] > 28) {
		b[1] = 28;
	}
#endif
	b[0] += (b[1] << 1);
	if (banner == WPN && epitomizedPathIndex > 0) {
		epitomizedPathIndex--;
#ifndef DEBUG
		epitomizedPathIndex &= 1;
#endif
		epitomizedPath = FiveStarWpnUp[b[0]][epitomizedPathIndex];
	}
	if (v[2]) {
#ifndef DEBUG
		v[3] = ((v[0] & 0xf) << 4 | (v[1] & 0xf));
#else
		v[3] = (v[0] << 4 | v[1]);
#endif
	}
	else {
		v[3] = b[4] >> 4;
	}
	v[0] = v[3];
#ifndef DEBUG
	if (banner != NOVICE) {
#else
	if (1) {
#endif
#ifndef DEBUG
		if (v[0] > 0x42) v[0] = 0x42;
#endif
		if (v[0] >= 0x40) {
			v[0] -= 0x7;
		}
		if (v[0] >= 0x30) {
			v[0] -= 0x7;
		}
		if (v[0] >= 0x20) {
			v[0] -= 0x9;
		}
		v[0] -= 0xf;
#ifndef DEBUG
		if ((int) v[0] < 1) v[0] = 1;
		if (v[0] > 29) v[0] = 29;
#endif
	}
	else v[0] = 0;
#ifndef DEBUG
	if (FiveStarChrUp[b[0]][1] == 0xffff && banner == CHAR2) {
		fprintf(stderr, "Warning: Character Event Banner-2 didn't run during version %d.%d phase %d, switching to main Character Event Banner\n", (b[4] >> 8 & 0xf), (b[4] >> 4) & 0xf, b[4] & 0xf);
		banner = CHAR1;
	}
#endif
	if (detailsRequested) {
		printf("Details for the %s banner", banners[banner][1]);
		if ((banner == CHAR1 || banner == CHAR2 || banner == WPN) && b[3]) {
			printf(" from v%d.%d phase %d", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
		}
		printf(":");
		if (banner != NOVICE && v[2]) {
			printf(" (v%d.%d standard pool)", v[3] >> 4, v[3] & 0xf);
		}
		printf("\n\n");
		if (banner == CHAR1 || banner == CHAR2) {
			item = FiveStarChrUp[b[0]][banner - CHAR1];
			if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
				snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
			}
			else if (getWeapon(item) != NULL) {
				snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", getWeapon(item), item);
			}
			else {
				snprintf(buf, 1024, "id \e[33;1m%u\e[39;0m", item);
			}
			printf("Rate-Up 5★ Character:\n\t%s\n\n", buf);
			printf("Rate-Up 4★ Characters:\n");
			for (n = 0; n < 3; n++) {
				item = FourStarChrUp[b[0]][n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", getWeapon(item), item);
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
				item = FiveStarWpnUp[b[0]][n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
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
				item = FourStarWpnUp[b[0]][n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
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
			for (n = 0; n < FiveStarMaxIndex[v[0]]; n++) {
				item = FiveStarChr[n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != CHAR1 && banner != CHAR2 && banner != NOVICE) {
			printf("5★ Weapon Pool:\n");
			for (n = 0; n < 10; n++) {
				item = FiveStarWpn[n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		printf("4★ Character Pool:\n");
		for (n = 0; n < FourStarMaxIndex[v[0]]; n++) {
			item = FourStarChr[n];
			if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
				snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
			}
			else if (getWeapon(item) != NULL) {
				snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", getWeapon(item), item);
			}
			else {
				snprintf(buf, 1024, "id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
			}
			printf("\t%s\n", buf);
		}
		printf("\n");
		if (banner != NOVICE) {
			printf("4★ Weapon Pool:\n");
			for (n = 0; n < 18; n++) {
				item = FourStarWpn[n];
				if (item <= 1000 + MAX_CHARS && item >= 1000 && chrList[item - 1000] != NULL) {
					snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", chrList[item - 1000], item);
				}
				else if (getWeapon(item) != NULL) {
					snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", getWeapon(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		printf("3★ Weapon Pool:\n");
		for (n = 0; n < 13; n++) {
			item = ThreeStar[n];
			if (getWeapon(item) != NULL) {
				snprintf(buf, 1024, "\e[34;22m%s\e[39;0m (id %u)", getWeapon(item), item);
			}
			else {
				snprintf(buf, 1024, "id \e[34;22m%u\e[39;0m", item);
			}
			printf("\t%s\n", buf);
		}
		return 0;
	}
	fprintf(stderr, "Making %u wishes on the %s banner", pulls, banners[banner][1]);
	if ((banner == CHAR1 || banner == CHAR2 || banner == WPN) && b[3]) {
		fprintf(stderr, " from v%d.%d phase %d", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
	}
	if (banner != NOVICE && v[2]) {
		fprintf(stderr, " (v%d.%d standard pool)", v[3] >> 4, v[3] & 0xf);
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
		else item = doAPull(banner, v[0], b[0], &rare, &won5050);
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
