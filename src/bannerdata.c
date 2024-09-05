/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include "gacha.h"

const unsigned short FourStarChrUp[IDX_MAX*2][3] = {
	// v1.0 - Launch
	{1105, 1109, 1106}, // Natasha, Hook, Pela
	{1206, 1001, 1202}, // Sushang, March 7th, Tingyun
	// v1.1
	{1002, 1009, 1103}, // Dan Heng, Asta, Serval
	{1207, 1201, 1106}, // *Yukong*, Qingque
	// v1.2
	{1008, 1206, 1105}, // Arlan
	{1111, 1108, 1103}, // *Luka*, Sampo
	// v1.3
	{1207, 1009, 1001},
	{1110, 1109, 1106}, // Lynx
	// v1.4
	{1202, 1201, 1108},
	{1210, 1111, 1206}, // Guinaifen
	// v1.5
	{1002, 1008, 1103},
	{1215, 1110, 1009}, // Hanya
	// v1.6
	{1214, 1001, 1202}, // Xueyi
	{1206, 1105, 1109},
	// v2.0
	{1312, 1202, 1210}, // Misha
	{1108, 1201, 1215},
	// v2.1
	{1301, 1106, 1002}, // Gallagher
	{1110, 1111, 1103},
	// v2.2
	{1001, 1215, 1214},
	{1106, 1111, 1109},
	// v2.3
	{1301, 1312, 1214},
	{1103, 1105, 1009},
	// v2.4
	{1215, 1207, 1110},
	{1008, 1109, 1210},
	// v2.5 (Indelible Coterie differing 4 stars? TODO)
	{1223, 1009, 1111}, // Moze
	{1005, 1210, 1312},
	// v2.6 TODO
	{8001, 8002, 8003},
	{8004, 8005, 8006},
};

// 0xfffe: Other three banners are read from (TODO)
// 0xffff: No second banner
const unsigned short FiveStarChrUp[IDX_MAX*2][2] = {
	// v1.0	- Launch
	{1102, 0xffff}, // Seele
	{1204, 0xffff}, // Jing Yuan
	// v1.1
	{1006, 0xffff}, // Silver Wolf
	{1203, 0xffff}, // Luocha
	// v1.2
	{1205, 0xffff}, // Blade
	{1005, 0xffff}, // Kafka
	// v1.3
	{1213, 0xffff}, // Dan Heng • Imbibitor Lunae
	{1208, 0xffff}, // Fu Xuan
	// v1.4
	{1212, 0xffff}, // Jingliu
	{1112, 1102}, // Topaz and Numby
	// v1.5
	{1217, 0xffff}, // Huohuo
	{1302, 1006}, // Argenti
	// v1.6
	{1303, 1205}, // Ruan Mei
	{1305, 1005}, // Dr. Ratio
	// v2.0
	{1307, 1213}, // Black Swan
	{1306, 1204}, // Sparkle
	// v2.1
	{1308, 1203}, // Acheron
	{1304, 1212}, // Aventurine
	// v2.2
	{1309, 1112}, // Robin
	{1315, 1208}, // Boothill
	// v2.3
	{1310, 1303}, // Firefly
	{1314, 1302}, // Jade
	// v2.4
	{1321, 1217}, // Yunli
	{1318, 1306}, // Jiaoqiu
	// v2.5 (Indelible Coterie TODO)
	{1220, 0}, // Feixiao
	{1222, 1112}, // Lingsha
	// v2.6 TODO
	{8001, 8002},
	{8003, 8004},
};

const unsigned short FourStarWpnUp[IDX_MAX*2][3] = {
	// v1.0 - Launch
	{21000, 21001, 21005}, // Post-Op Conversation, Good Night and Sleep Well, The Moles Welcome You
	{21002, 21003, 21011}, // Day One of My New Life, Only Silence Remains, Planetary Rendezvous
	// v1.1
	{21004, 21013, 21017}, // Memories of the Past, Make the World Clamor, Subscribe for More
	{21018, 21020, 21001}, // Geniuses Repose, Dance Dance Dance
	// v1.2
	{21007, 21010, 21012}, // Shared Feeling, Swordplay, A Secret Vow
	{21006, 21008, 21015}, // Birth of the Self, Eyes of the Prey, Revolution Shines as Pearls of Sweat
	// v1.3
	{21009, 21011, 21018}, // Landau's Choice
	{21014, 21016, 21019}, // Perfect Timing, Trend of the Universal Market, Under the Blue Sky
	// v1.4
	{21004, 21008, 21013},
	{21003, 21005, 21015},
	// v1.5
	{21007, 21016, 21017},
	{21000, 21006, 21019},
	// v1.6
	{21002, 21011, 21012},
	{21003, 21009, 21014},
	// v2.0
	{21042, 21015, 21018}, // Indelible Promise
	{21011, 21019, 21020},
	// v2.1
	{21000, 21001, 21017},
	{21043, 21007, 21013}, // Concert for Two
	// v2.2
	{21044, 21010, 21014}, // Boundless Choreo
	{21009, 21012, 21020},
	// v2.3
	{21004, 21002, 21008},
	{21045, 21007, 21016}, // After the Charmony Fall
	// v2.4
	{21005, 21006, 21014},
	{21046, 21008, 21012}, // Poised to Bloom
	// v2.5 (Coalesced Truths different 4 stars? TODO)
	{21006, 21010, 21015},
	{21047, 21007, 21011}, // Shadowed by Night
	// v2.6 TODO
	{21000, 21001, 21002},
	{21000, 21001, 21002},
};

// 0xffff: No second banner
const unsigned short FiveStarWpnUp[IDX_MAX*2][2] = {
	// v1.0 - Launch
	{23001, 0xffff}, // In the Night
	{23010, 0xffff}, // Before Dawn
	// v1.1
	{23007, 0xffff}, // Incessant Rain
	{23008, 0xffff}, // Echoes of the Coffin
	// v1.2
	{23009, 0xffff}, // The Unreachable Side
	{23006, 0xffff}, // Patience is All You Need
	// v1.3
	{23015, 0xffff}, // Brighter than the Sun
	{23011, 0xffff}, // She Already Shut Her Eyes
	// v1.4
	{23014, 0xffff}, // I Shall Be My Own Sword
	{23016, 23001}, // Worrisome, Blissful
	// v1.5
	{23017, 0xffff}, // Night of Fright
	{23018, 23007}, // An Instant Before a Gaze
	// v1.6
	{23019, 23009}, // Past Self in Mirror
	{23020, 23006}, // Baptism of Pure Thought
	// v2.0
	{23022, 23015}, // Reforged Rememberance
	{23021, 23010}, // Earthly Escapade
	// v2.1
	{23024, 23008}, // Along the Passing Shore
	{23023, 23014}, // Inherently Unjust Destiny
	// v2.2
	{23026, 23016}, // Flowing Nightglow
	{23027, 23011}, // Sailing Towards a Second Life
	// v2.3
	{23025, 23019}, // Whereabouts Should Dreams Rest
	{23028, 23011}, // Yet Hope Is Priceless
	// v2.4
	{23031, 23017}, // Dance at Sunset
	{23030, 23021}, // Those Many Springs
	// v2.5 (Indelible Coterie TODO)
	{23032, 0}, // I Venture Forth to Hunt
	{23033, 23016}, // Scent Alone Stays True
	// v2.6 TODO
	{23000, 23001},
	{23000, 23001},
};

// Max indexes into FourStarChr per version (for old banners)
const unsigned char FourStarChrMaxIndex[IDX_MAX] = {
	/* v1.x */ 13, 13, 14, 15, 16, 17, 18,
	/* v2.x */ 19, 20, 21, 21, 21, 21, 21
};

const unsigned char FourStarWpnMaxIndex[IDX_MAX] = {
	/* v1.x */ 21, 21, 21, 21, 21, 21, 21,
	/* v2.x */ 21, 22, 23, 24, 25, 26, 27
};

// The list of 4-star characters in the pool, used together with the rate-up drops.
const unsigned short FourStarChr[22] = {
	// v1.0 standard pool
	1001, 1002, 1008, 1009, 1013, 1103, 1105, 1106, 1108, 1109, 1201, 1202, 1206,
	// v1.2: Yukong
	1207,
	// v1.3: Luka
	1111,
	// v1.4: Lynx
	1110,
	// v1.5: Guinaifen
	1210,
	// v1.6: Hanya
	1215,
	// v2.0: Xueyi
	1214,
	// v2.1: Misha
	1312,
	// v2.2: Gallagher
	1301,
};

// The list of 4-star lightcones in the pool, used together with the rate-up drops.
const unsigned short FourStarWpn[27] = {
	// v1.x standard pool
	21000, 21001, 21002, 21003, 21004, 21005, 21006, 21007, 21008, 21009, 21010, 21011, 21012, 21013, 21014, 21015, 21016, 21017, 21018, 21019, 21020,
	// v2.1: Indelible Promise
	21042,
	// v2.2: Concert for Two
	21043,
	// v2.3: Boundless Choreo
	21044,
	// v2.4: After the Charmony Fall
	21045,
	// v2.5: Poised to Bloom
	21046,
	// v2.6: Shadowed by Night
	21047,
};

// The list of 5-star characters in the pool, used together with the rate-up drops.
const unsigned short FiveStarChr[7] = {1003, 1004, 1101, 1104, 1107, 1209, 1211};

// The list of 5-star lightcones in the pool, used together with the rate-up drops (only on the lightcone and standard banners)
const unsigned short FiveStarWpn[7] = {23000, 23002, 23003, 23004, 23005, 23012, 23013};

