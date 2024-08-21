/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include "item.h"
#include "util.h"

static const char* const OneStarSwords[1] = {_N("Dull Blade")};
static const char* const OneStarClaymores[1] = {_N("Waster Greatsword")};
static const char* const OneStarPolearms[1] = {_N("Beginner's Protector")};
static const char* const OneStarCatalysts[1] = {_N("Apprentice's Notes")};
static const char* const OneStarBows[1] = {_N("Hunter's Bow")};

static const char* const* const OneStarWeapons[6] = {
	[SWORD] = OneStarSwords,
	[CLAYMORE] = OneStarClaymores,
	[POLEARM] = OneStarPolearms,
	[CATALYST] = OneStarCatalysts,
	[BOW] = OneStarBows,
};

static const char* const TwoStarSwords[1] = {_N("Silver Sword")};
static const char* const TwoStarClaymores[1] = {_N("Old Merc's Pal")};
static const char* const TwoStarPolearms[1] = {_N("Iron Point")};
static const char* const TwoStarCatalysts[1] = {_N("Pocket Grimoire")};
static const char* const TwoStarBows[1] = {_N("Seasoned Hunter's Bow")};

static const char* const* const TwoStarWeapons[6] = {
	NULL,
	TwoStarSwords,
	TwoStarClaymores,
	TwoStarPolearms,
	TwoStarCatalysts,
	TwoStarBows,
};

static const char* const ThreeStarSwords[6] = {
	_N("Cool Steel"),
	_N("Harbinger of Dawn"),
	_N("Traveler's Handy Sword"),
	_N("Dark Iron Sword"),
	_N("Fillet Blade"),
	_N("Skyrider Sword"),
};
static const char* const ThreeStarClaymores[6] = {
	_N("Ferrous Shadow"),
	_N("Bloodtainted Greatsword"),
	_N("White Iron Greatsword"),
	_N("(Beta) Quartz"),
	_N("Debate Club"),
	_N("Skyrider Greatsword"),
};
static const char* const ThreeStarPolearms[4] = {
	_N("White Tassel"),
	_N("Halberd"),
	_N("Black Tassel"),
	_N("(Test) The Flagstaff"),
};
static const char* const ThreeStarCatalysts[6] = {
	_N("Magic Guide"),
	_N("Thrilling Tales of Dragon Slayers"),
	_N("Otherworldly Story"),
	_N("Emerald Orb"),
	_N("Twin Nephrite"),
	_N("(Beta) Amber Bead"),
};
static const char* const ThreeStarBows[6] = {
	_N("Raven Bow"),
	_N("Sharpshooter's Oath"),
	_N("Recurve Bow"),
	_N("Slingshot"),
	_N("Messenger"),
	_N("(Beta) Ebony Bow"),
};

static const char* const* const ThreeStarWeapons[6] = {
	NULL,
	ThreeStarSwords,
	ThreeStarClaymores,
	ThreeStarPolearms,
	ThreeStarCatalysts,
	ThreeStarBows,
};

static const char* const FourStarSwords[31] = {
	_N("Favonius Sword"),
	_N("The Flute"),
	_N("Sacrificial Sword"),
	_N("Royal Longsword"),
	_N("Lion's Roar"),
	_N("Prototype Rancour"),
	_N("Iron Sting"),
	_N("Blackcliff Longsword"),
	_N("The Black Sword"),
	_N("The Alley Flash"),
	_N("(Test) (Dull Blade icon)"),
	_N("Sword of Descension"),
	_N("Festering Desire"),
	_N("Amenoma Kageuchi"),
	_N("Cinnibar Spindle"),
	_N("Kagotsurube Isshin"),
	_N("Sapwood Blade"),
	_N("Xiphos' Moonlight"),
	_N("Prized Isshin Blade"), // Final Stage
	_N("Prized Isshin Blade (2)"), // First Stage
	_N("Prized Isshin Blade (3)"), // Second Stage
	_N("Toukabou Shigure"),
	_N("Wolf-Fang"),
	_N("Finale of the Deep"),
	_N("Fleuve Cendre Ferryman"),
	[26] = _N("The Dockhand's Assistant"),
	[28] = _N("Sword of Narzissenkreuz"), // Final Stage
	[29] = _N("Sword of Narzissenkreuz (2)"), // First Stage
	[30] = _N("Flute of Ezpitzal"),
};

static const char* const FourStarClaymores[31] = {
	_N("Favonius Greatsword"),
	_N("The Bell"),
	_N("Sacrificial Greatsword"),
	_N("Royal Greatsword"),
	_N("Rainslasher"),
	_N("Prototype Archaic"),
	_N("Whiteblind"),
	_N("Blackcliff Slasher"),
	_N("Serpent Spine"),
	_N("Lithic Blade"),
	_N("Snow-Tombed Starsilver"),
	_N("Luxurious Sea-Lord"),
	NULL,
	_N("Katsuragikiri Nagamasa"),
	_N("Makhaira Aquamarine"),
	_N("Akuoumaru"),
	_N("Forest Regalia"),
	_N("Mailed Flower"),
	[23] = _N("Talking Stick"),
	[24] = _N("Tidal Shadow"),
	[25] = _N("\"Ultimate Overlord's Mega Magic Sword\""),
	[26] = _N("Portable Power Saw"),
	[30] = _N("Earth Shaker"),
};

static const char* const FourStarPolearms[31] = {
	_N("Dragon's Bane"),
	_N("Prototype Starglitter"),
	_N("Crescent Pike"),
	_N("Blackcliff Pole"),
	_N("Deathmatch"),
	_N("Lithic Spear"),
	_N("Favonius Lance"),
	_N("Royal Spear"),
	_N("Dragonspine Spear"),
	[13] = _N("Kitain Cross Spear"),
	[14] = _N("\"The Catch\""),
	[15] = _N("Wavebreaker's Fin"),
	[16] = _N("Moonpiercer"),
	[18] = _N("Missive Windspear"),
	[23] = _N("Ballad of the Fjords"),
	[24] = _N("Rightful Reward"),
	[26] = _N("Prospector's Drill"),
	[27] = _N("Dialogues of the Desert Sages"),
	[30] = _N("Footprint of the Rainbow"),
};

static const char* const FourStarCatalysts[31] = {
	_N("Favonius Codex"),
	_N("The Widsith"),
	_N("Sacrificial Fragments"),
	_N("Royal Grimoire"),
	_N("Solar Pearl"),
	_N("Prototype Amber"),
	_N("Mappa Mare"),
	_N("Blackcliff Agate"),
	_N("Eye of Perception"),
	_N("Wine and Song"),
	_N("(Test) (Apprentice's Notes icon)"),
	_N("Frostbearer"),
	_N("Dodoco Tales"),
	_N("Hakushin Ring"),
	_N("Oathsworn Eye"),
	_N("Wandering Evenstar"),
	_N("Fruit of Fulfillment"),
	[23] = _N("Sacrificial Jade"),
	[24] = _N("Flowing Purity"),
	[25] = _N("Ballad of the Boundless Blue"),
	[26] = _N("Ash-Graven Drinking Horn"),
	[30] = _N("Ring of Yaxche"),
};

static const char* const FourStarBows[31] = {
	_N("Favonius Warbow"),
	_N("The Stringless"),
	_N("Sacrificial Bow"),
	_N("Royal Bow"),
	_N("Rust"),
	_N("Prototype Crescent"),
	_N("Compound Bow"),
	_N("Blackcliff Warbow"),
	_N("The Viridescent Hunt"),
	_N("Alley Hunter"),
	_N("Fading Twilight"),
	_N("Mitternachts Waltz"),
	_N("Windblume Ode"),
	_N("Hamayumi"),
	_N("Predator"),
	_N("Mouun's Moon"),
	_N("King's Squire"),
	_N("End of the Line"),
	_N("Ibis Piercer"),
	[23] = _N("Scion of the Blazing Sun"),
	[24] = _N("Song of Stillness"),
	[25] = _N("Cloudweaver"),
	[26] = _N("Range Gauge"),
	[30] = _N("Chain Breaker"),
};

static const char* const* const FourStarWeapons[6] = {
	NULL,
	FourStarSwords,
	FourStarClaymores,
	FourStarPolearms,
	FourStarCatalysts,
	FourStarBows,
};

static const char* const FiveStarSwords[15] = {
	_N("Aquila Favonia"),
	_N("Skyward Blade"),
	_N("Freedom-Sworn"),
	_N("Summit Shaper"),
	_N("Primordial Jade Cutter"),
	_N("(Test) Primordial Jade Cutter"),
	_N("(Test) One Side"),
	_N("(Test) (Dull Blade icon)"),
	_N("Mistsplitter Reforged"),
	_N("Haran Geppaku Futsu"),
	_N("Key of Khaj-Nisut"),
	_N("Light of Foliar Incision"),
	_N("Splendor of Tranquil Waters"),
	_N("Uraku Misugiri"),
	_N("Absolution"),
};

static const char* const FiveStarClaymores[13] = {
	_N("Skyward Pride"),
	_N("Wolf's Gravestone"),
	_N("Song of Broken Pines"),
	_N("The Unforged"),
	_N("(Test) Primordial Jade Greatsword"),
	_N("(Test) The Other Side"),
	NULL,
	_N("(Test) (Waster Greatsword icon)"),
	_N("(Test) (null name and icon)"),
	_N("Redhorn Stonethresher"),
	_N("Beacon of the Reed Sea"),
	_N("Verdict"),
	_N("Fang of the Mountain King"),
};

static const char* const FiveStarPolearms[13] = {
	_N("Staff of Homa"),
	_N("Skyward Spine"),
	_N("(Test) (Beginner's Protector icon)"),
	_N("Vortex Vanquisher"),
	_N("Primordial Jade Winged-Spear"),
	_N("(Test) Deicide"),
	_N("Calamity Queller"),
	NULL,
	_N("Engulfing Lightning"),
	NULL,
	_N("Staff of the Scarlet Fields"),
	_N("Crimson Moon's Semblance"),
	_N("Lumidouce Elegy"),
};

static const char* const FiveStarCatalysts[16] = {
	_N("Skyward Atlas"),
	_N("Lost Prayer to the Sacred Winds"),
	_N("(Test) Lost Ballade"),
	_N("Memory of Dust"),
	_N("Jadefall's Splendor"),
	_N("Everlasting Moonglow"),
	NULL,
	_N("(Test) (Apprentice's Notes icon)"),
	_N("Kagura's Verity"),
	NULL,
	_N("A Thousand Floating Dreams"),
	_N("Tulaytullah's Rememberance"),
	_N("Cashflow Supervision"),
	_N("Tome of the Eternal Flow"),
	_N("Crane's Echoing Call"),
	_N("Surf's Up"),
};

static const char* const FiveStarBows[13] = {
	_N("Skyward Harp"),
	_N("Amos' Bow"),
	_N("Elegy for the End"),
	_N("(Test) Kunwu's Wyrmbane"),
	_N("(Test) Primordial Jade Vista"),
	_N("(Test) Mirror Breaker"),
	_N("Polar Star"),
	_N("Aqua Simulacara"),
	_N("Thundering Pulse"),
	NULL,
	_N("Hunter's Path"),
	_N("The First Great Magic"),
	_N("Silvershower Heartstrings"),
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
	case 24:
	case 34:
	case 44:
	case 54:
		maxId = 31;
		break;
	case 15:
		maxId = 15;
		break;
	case 25:
	case 35:
	case 55:
		maxId = 13;
		break;
	case 45:
		maxId = 16;
		break;
	}
	if (id >= maxId) return NULL;
	return gettext(Weapons[stars][type][id]);
}
