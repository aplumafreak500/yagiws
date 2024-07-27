/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include "gacha.h"

const unsigned short FourStarChrUp[IDX_MAX*2][3] = {
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
	// v4.2 - Masquerade of the Guilty & Erinnyes Forest
	{1067, 1024, 1088}, // Charlotte
	{1061, 1065, 1023},
	// v4.3 - Roses & Muskets
	{1043, 1045, 1072},
	{1090, 1056, 1032}, // Chevreuse
	// v4.4 - Lantern Rite 4 & Chenyu Vale
	{1092, 1034, 1076}, // Gaming
	{1077, 1044, 1027},
	// v4.5 - Alchemical Ascension
	{1055, 1068, 1064},
	{1014, 1025, 1048},
	// v4.6 - Sea of Bygone Eras
	{1083, 1085, 1023},
	{1074, 1076, 1024},
	// v4.7 - Imaginarium Theater
	{1097, 1032, 1050}, // Sethos
	{1034, 1045, 1092},
	// v4.8 - Summertide Scales and Tales (phase 2 TODO)
	{1061, 1081, 1092},
	{1006, 1015, 1021},
	// v5.0 - Natlan TODO
	{1006, 1015, 1021},
	{1006, 1015, 1021},
};

// 0xffff: No second banner
const unsigned short FiveStarChrUp[IDX_MAX*2][2] = {
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
	// v4.2 - Masquerade of the Guilty & Erinnyes Forest
	{1089, 1082}, // Furina
	{1071, 1066},
	// v4.3 - Roses & Muskets
	{1091, 1002}, // Navia
	{1052, 1049},
	// v4.4 - Lantern Rite 4 & Chenyu Vale
	{1093, 1073}, // Xianyun
	{1026, 1058},
	// v4.5 - Alchemical Ascension
	{1094, 1057}, // Chiori
	{1087, 1047},
	// v4.6 - Sea of Bygone Eras
	{1096, 1084}, // Arlecchino
	{1075, 1082},
	// v4.7 - Imaginarium Theater
	{1098, 1078}, // Clorinde
	{1095, 1089}, // Sigewinne
	// v4.8 - Summertide Scales and Tales
	{1091, 1070},
	{1099, 1060}, // Emile
	// v5.0 - Natlan TODO
	{1005, 1007},
	{1005, 1007},
};

const unsigned short FourStarWpnUp[IDX_MAX*2][5] = {
	// v1.0 - Launch
	{13407, 11402, 12402, 15402, 14402}, // Favonius Lance, The Flute, The Bell, The Stringless, The Widsith
	{13401, 11403, 12403, 14403, 15403}, // Dragon's Bane, Sacrificial Sword, Sacrificial Greatsword, Sacrificial Fragments, Sacrificial Bow
	// v1.1 - Unreconciled Stars & Foul Legacy
	{15405, 11402, 14410, 13407, 12405}, // Rust, Wine and Song, Rainslasher
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
	{15410, 11401, 13407, 14401, 12403}, // Alley Hunter, Favonius Sword
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
	// v4.2 - Masquerade of the Guilty & Erinnyes Forest
	{11403, 12402, 13401, 14403, 15402},
	{11405, 12401, 13407, 14402, 15401},
	// v4.3 - Roses & Muskets
	{11402, 12416, 13401, 14401, 15416},
	{11401, 12405, 13416, 14409, 15405},
	// v4.4 - Lantern Rite 4 & Chenyu Vale
	{11403, 12403, 13406, 14403, 15403},
	{11405, 12410, 13407, 14402, 15402},
	// v4.5 - Alchemical Ascension
	{11410, 12402, 13401, 14401, 15410},
	{11402, 12401, 13407, 14410, 15412},
	// v4.6 - Sea of Bygone Eras
	{11427, 12427, 13401, 14409, 15401},
	{11401, 12405, 13427, 14403, 15427},
	// v4.7 - Imaginarium Theater
	{11403, 12403, 13406, 14402, 15402},
	{11405, 12410, 13407, 14401, 15403},
	// v4.8 - Summertide Scales and Tales (phase 2 TODO)
	{11418, 12402, 13401, 14409, 15405},
	{11400, 12400, 13400, 14400, 15400},
	// v5.0 - Natlan TODO
	{11400, 12400, 13400, 14400, 15400},
	{11400, 12400, 13400, 14400, 15400},
};

const unsigned short FiveStarWpnUp[IDX_MAX*2][2] = {
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
	// v4.2 - Masquerade of the Guilty & Erinnyes Forest
	{11513, 14505}, // Splendor of Tranquil Waters
	{13511, 11510},
	// v4.3 - Roses & Muskets
	{12512, 11509}, // Verdict
	{15509, 13509},
	// v4.4 - Lantern Rite 4 & Chenyu Vale
	{14515, 14511}, // Crane's Echoing Call
	{13505, 14509},
	// v4.5 - Alchemical Ascension
	{11514, 12510}, // Uraku Misugiri
	{14514, 11503},
	// v4.6 - Sea of Bygone Eras
	{13512, 15512}, // Crimson Moon's Semblance
	{14512, 14505},
	// v4.7 - Imaginarium Theater
	{11515, 11512}, // Absolution
	{15513, 11513}, // Silvershower Heartstrings
	// v4.8 - Summertide Scales and Tales
	{12512, 11511},
	{13513, 15508}, // Lumidouce Elegy
	// v5.0 - Natlan TODO
	{10500, 10501},
	{10500, 10501},
};

static const unsigned short ChroniclePool5_441[17] = {
	// Characters
	1051, 1041, 1038, 1029, 1016, 1003,
	// Weapons
	11502, 11501, 12501, 12502, 12503, 12511, 13502, 14502, 14501, 15501, 15511
};

static const unsigned short ChroniclePool4_441[33] = {
	// Characters
	1006, 1014, 1015, 1020, 1021, 1031, 1032, 1034, 1039, 1043, 1045, 1080,
	// Weapons
	11401, 11402, 11403, 11405, 12401, 12402, 12403, 12405, 13401, 13407, 14401, 14402, 14403, 14409, 14410, 15401, 15402, 15403, 15405, 15410, 15412
};

static const ChroniclePool_t ChroniclePool_441 = {
	ChroniclePool5_441, 6, 11,
	ChroniclePool4_441, 12, 21,
};

const ChroniclePool_t* getChroniclePool(unsigned short v) {
	switch (v) {
	default:
		return NULL;
	case 30: // v4.4 - Lantern Rite 4 & Chenyu Vale (phase 1)
		return &ChroniclePool_441;
	}
}

// The list of 4-star characters in the pool, used together with the rate-up drops.
const unsigned short FourStarChr[40] = {
	// v1.0 standard pool: Lisa, Kayea, Amber (standard banner only)
	1006, 1015, 1021,
	// v1.0 standard pool: Barbara, Razor, Xiangling, Beidou, Xingqiu, Ningguang, Fischl, Bennett, Noelle, Chongyun, Sucrose
	1014, 1020, 1023, 1024, 1025, 1027, 1031, 1032, 1036, 1043,
	// Noelle (listed as last of 1.0 chars due to novice banner)
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
	// v4.3: Charlotte
	1088,
	// v4.4: Chevreuse
	1090,
	// v4.5: Gaming
	1092,
	// v4.8: Sethos
	1097,
};

// The list of 5-star characters in the pool, used together with the rate-up drops.
const unsigned short FiveStarChr[7] = {
	// v1.0 standard pool: Jean, Diluc, Qiqi, Mona, Keqing
	1003, 1016, 1035, 1041, 1042,
	// v3.1: Tighnari
	1069,
	// v3.6: Deyha
	1079,
};

// Max indexes into FourStarChr per version (for old banners)
const unsigned char FourStarMaxIndex[IDX_MAX] = {
	/* Novice */ 10,
	/* v1.x */ 11, 11, 13, 13, 13, 14, 15,
	/* v2.x */ 15, 16, 17, 18, 19, 20, 20, 20, 21,
	/* v3.x */ 22, 24, 25, 26, 27, 28, 29, 30, 31,
	/* v4.x */ 31, 33, 33, 34, 35, 36, 36, 36, 37,
	/* v5.x */ 37,
};

// Max indexes into FiveStarChr per version (for old banners)
const unsigned char FiveStarMaxIndex[IDX_MAX] = {
	/* Novice */ 5,
	/* v1.x */ 5, 5, 5, 5, 5, 5, 5,
	/* v2.x */ 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* v3.x */ 5, 6, 6, 6, 6, 6, 7, 7, 7,
	/* v4.x */ 7, 7, 7, 7, 7, 7, 7, 7, 7,
	/* v5.x */ 7,
};

// The list of 3-star weapons in the pool.
const unsigned short ThreeStar[13] = {11301, 11302, 11306, 12301, 12302, 12305, 13303, 14301, 14302, 15304, 15301, 15302, 15304};

// The list of 4-star weapons in the pool, used together with the rate-up drops.
const unsigned short FourStarWpn[18] = {11401, 11402, 11403, 11405, 12401, 12402, 12403, 12405, 13401, 13407, 14401, 14402, 14403, 14409, 15401, 15402, 15403, 15405};

// The list of 5-star weapons in the pool, used together with the rate-up drops (only on the weapon and standard banners)
const unsigned short FiveStarWpn[10] = {11501, 11502, 12501, 12502, 13502, 13505, 14501, 14502, 15501, 15502};
