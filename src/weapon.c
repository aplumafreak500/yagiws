/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2023 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <stddef.h>
#include "config.h"
#include "item.h"

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
	"Skyrider Sword",
};
static const char* const ThreeStarClaymores[6] = {
	"Ferrous Shadow",
	"Bloodtainted Greatsword",
	"White Iron Greatsword",
	"(Beta) Quartz",
	"Debate Club",
	"Skyrider Greatsword",
};
static const char* const ThreeStarPolearms[4] = {
	"White Tassel",
	"Halberd",
	"Black Tassel",
	"(Test) The Flagstaff",
};
static const char* const ThreeStarCatalysts[6] = {
	"Magic Guide",
	"Thrilling Tales of Dragon Slayers",
	"Otherworldly Story",
	"Emerald Orb",
	"Twin Nephrite",
	"(Beta) Amber Bead",
};
static const char* const ThreeStarBows[6] = {
	"Raven Bow",
	"Sharpshooter's Oath",
	"Recurve Bow",
	"Slingshot",
	"Messenger",
	"(Beta) Ebony Bow",
};

static const char* const* const ThreeStarWeapons[6] = {
	NULL,
	ThreeStarSwords,
	ThreeStarClaymores,
	ThreeStarPolearms,
	ThreeStarCatalysts,
	ThreeStarBows,
};

static const char* const FourStarSwords[30] = {
	"Favonius Sword",
	"The Flute",
	"Sacrificial Sword",
	"Royal Longsword",
	"Lion's Roar",
	"Prototype Rancour",
	"Iron Sting",
	"Blackcliff Longsword",
	"The Black Sword",
	"The Alley Flash",
	"(Test) (Dull Blade icon)",
	"Sword of Descension",
	"Festering Desire",
	"Amenoma Kageuchi",
	"Cinnibar Spindle",
	"Kagotsurube Isshin",
	"Sapwood Blade",
	"Xiphos' Moonlight",
	"Prized Isshin Blade", // Final Stage
	"Prized Isshin Blade (2)", // First Stage
	"Prized Isshin Blade (3)", // Second Stage
	"Toukabou Shigure",
	"Wolf-Fang",
	"Finale of the Deep",
	"Fleuve Cendre Ferryman",
	[26] = "The Dockhand's Assistant",
	[28] = "Sword of Narzissenkreuz", // Final Stage
	[29] = "Sword of Narzissenkreuz (2)", // First Stage
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
	"The Widsith",
	"Sacrificial Fragments",
	"Royal Grimoire",
	"Solar Pearl",
	"Prototype Amber",
	"Mappa Mare",
	"Blackcliff Agate",
	"Eye of Perception",
	"Wine and Song",
	"(Test) (Apprentice's Notes icon)",
	"Frostbearer",
	"Dodoco Tales",
	"Hakushin Ring",
	"Oathsworn Eye",
	"Wandering Evenstar",
	"Fruit of Fulfillment",
	[23] = "Sacrificial Jade",
	[24] = "Flowing Purity",
	[26] = "Ballad of the Boundless Blue",
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
	"The Viridescent Hunt",
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
	"(Test) Primordial Jade Cutter",
	"(Test) One Side",
	"(Test) (Dull Blade icon)",
	"Mistsplitter Reforged",
	"Haran Geppaku Futsu",
	"Key of Khaj-Nisut",
	"Light of Foliar Incision",
	"Splendor of Tranquil Waters",
};

static const char* const FiveStarClaymores[11] = {
	"Skyward Pride",
	"Wolf's Gravestone",
	"Song of Broken Pines",
	"The Unforged",
	"(Test) Primordial Jade Greatsword",
	"(Test) The Other Side",
	NULL,
	"(Test) (Waster Greatsword icon)",
	"(Test) (null name and icon)",
	"Redhorn Stonethresher",
	"Beacon of the Reed Sea",
};

static const char* const FiveStarPolearms[11] = {
	"Staff of Homa",
	"Skyward Spine",
	"(Test) (Beginner's Protector icon)",
	"Vortex Vanquisher",
	"Primordial Jade Winged-Spear",
	"(Test) Deicide",
	"Calamity Queller",
	NULL,
	"Engulfing Lightning",
	NULL,
	"Staff of the Scarlet Fields",
};

static const char* const FiveStarCatalysts[14] = {
	"Skyward Atlas",
	"Lost Prayer to the Sacred Winds",
	"(Test) Lost Ballade",
	"Memory of Dust",
	"Jadefall's Splendor",
	"Everlasting Moonglow",
	NULL,
	"(Test) (Apprentice's Notes icon)",
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
	"(Test) Kunwu's Wyrmbane",
	"(Test) Primordial Jade Vista",
	"(Test) Mirror Breaker",
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

const char* getWeapon(unsigned int _id) {
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
		maxId = 29;
		break;
	case 24:
	case 34:
	case 44:
	case 54:
		maxId = 27;
		break;
	case 15:
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
