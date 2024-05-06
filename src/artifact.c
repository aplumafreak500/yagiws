/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <stddef.h>
#include "config.h"
#include "item.h"

static const char* const Artifacts[100][6] = {
	[20] = {"Deepwood Memories", "Lamp of the Lost", "Scholar of Vines", "Laurel Coronet", "Labyrinth Wayfarer", "A Time of Insight"},
	[21] = {"Gilded Dreams", "Honeyed Final Feast", "Feather of Judgment", "Shadow of the Sand King", "Dreaming Steelbloom", "The Sunken Years"},
	[27] = {"Desert Pavilion Chronicle", "Defender of the Enchanting Dream", "End of the Golden Realm", "Legacy of the Desert High-Born", "The First Days of the City of Kings", "Timepiece of the Lost Path"},
	[28] = {"Flower of Paradise Lost", "Secret-Keeper's Magic Bottle", "Wilting Feast", "Amethyst Crown", "Ay-Khanoum's Myriad", "A Moment Congealed"},
	[29] = {"Nymph's Dream", "Heroes' Tea Party", "Wicked Mage's Plumule", "Fell Dragon's Monocle", "Odyssean Flower", "Nymph's Constancy"},
	[30] = {"Vourukasha's Glow", "Feast of Boundless Joy", "Vibrant Pinion", "Heart of Khvarena's Brilliance", "Stamen of Khvarena's Origin", "Ancient Abscission"},
	[31] = {"Marechaussee Hunter", "Forgotten Vessel", "Masterpiece's Overture", "Veteran's Visage", "Hunter's Brooch", "Moment of Judgment"},
	[32] = {"Golden Troupe", "Golden Night's Bustle", "Golden Bird's Shedding", "Golden Troupe's Reward", "Golden Song's Variation", "Golden Era's Prelude"},
	[33] = {"Song of Days Past", "Promised Dream of Days Past", "Recollection of Days Past", "Poem Passed Down from Days Past", "Forgotten Oath of Days Past", "Echoing Sound from Days Past"},
	[34] = {"Nighttime Whispers in the Echoing Woods", "Magnanimous Ink Bottle", "Honest Quill", "Compassionate Ladies' Hat", "Selfless Floral Accessory", "Faithful Hourglass"},
	[35] = {"Fragment of Harmonic Whimsy", "Ichor Shower Rhapsody", "Ancient Sea's Nocturnal Musing", "Whimsical Dance of the Withered", "Harmonious Symphony Prelude", "The Grand Jape of the Turning of Fate"},
	[36] = {"Unfinished Reverie", "The Wine-Flask Over Which the Plan Was Hatched", "Faded Emerald Tail", "Crownless Crown", "Dark Fruit of Bright Flowers", "Moment of Attainment"},
	[51] = {"Resolution of Sojourner", "Goblet of the Sojourner", "Feather of Homecoming", "Crown of Parting", "Heart of Comradeship", "Sundial of the Sojourner"},
	[52] = {"Brave Heart", "Outset of the Brave", "Prospect of the Brave", "Crown of the Brave", "Medal of the Brave", "Fortitude of the Brave"},
	[53] = {"Defender's Will", "Guardian's Vessel", "Guardian's Sigil", "Guardian's Band", "Guardian's Flower", "Guardian's Clock"},
	[54] = {"Tiny Miracle", "Tiny Miracle's Goblet", "Tiny Miracle's Feather", "Tiny Miracle's Earrings", "Tiny Miracle's Flower", "Tiny Miracle's Hourglass"},
	[55] = {"Berserker", "Berserker's Bone Goblet", "Berserker's Indigo Feather", "Berserker's Battle Mask", "Berserker's Rose", "Berserker's Timepiece"},
	[56] = {"Martial Artist", "Martial Artist's Wine Cup", "Martial Artist's Feather Accessory", "Martial Artist's Bandana", "Martial Artist's Red Flower", "Martial Artist's Water Hourglass"},
	[57] = {"Instructor", "Instructor's Tea Cup", "Instructor's Feather Accessory", "Instructor's Cap", "Instructor's Brooch", "Instructor's Pocket Watch"},
	[58] = {"Gambler", "Gambler's Dice Cup", "Gambler's Feather Accessory", "Gambler's Earrings", "Gambler's Brooch", "Gambler's Pocket Watch"},
	[59] = {"The Exile", "Exile's Goblet", "Exile's Feather", "Exile's Circlet", "Exile's Flower", "Exile's Pocket Watch"},
	[60] = {"Adventurer", "Adventurer's Golden Goblet", "Adventurer's Tail Feather", "Adventurer's Bandana", "Adventurer's Flower", "Adventurer's Pocket Watch"},
	[61] = {"Lucky Dog", "Lucky Dog's Goblet", "Lucky Dog's Eagle Feather", "Lucky Dog's Silver Circlet", "Lucky Dog's Clover", "Lucky Dog's Hourglass"},
	[62] = {"Scholar", "Scholar's Ink Cup", "Scholar's Quill Pen", "Scholar's Lens", "Scholar's Bookmark", "Scholar's Clock"},
	[63] = {"Traveling Doctor", "Traveling Doctor's Medicine Pot", "Traveling Doctor's Owl Feather", "Traveling Doctor's Handkerchief", "Traveling Doctor's Silver Lotus", "Traveling Doctor's Pocket Watch"},
	[71] = {"Blizzard Strayer", "Frost-Weaved Dignity", "Icebreaker's Resolve", "Broken Rime's Echo", "Snowswept Memory", "Frozen Homeland's Demise"},
	[72] = {"Thundersoother", "Thundersoother's Goblet", "Thundersoother's Plume", "Thundersoother's Diadem", "Thundersoother's Heart", "Hour of Soothing Thunder"},
	[73] = {"Lavawalker", "Lavawalker's Epiphany", "Lavawalker's Salvation", "Lavawalker's Wisdom", "Lavawalker's Resolution", "Lavawalker's Torment"},
	[74] = {"Maiden Beloved", "Maiden's Fleeting Leisure", "Maiden's Heart-stricken Infatuation", "Maiden's Fading Beauty", "Maiden's Distant Love", "Maiden's Passing Youth"},
	[75] = {"Gladiator's Finale", "Gladiator's Intoxication", "Gladiator's Destiny", "Gladiator's Triumphus", "Gladiator's Nostalgia", "Gladiator's Longing"},
	[76] = {"Viridescent Venerer", "Viridescent Venerer's Vessel", "Viridescent Arrow Feather", "Viridescent Venerer's Diadem", "In Remembrance of Viridescent Fields", "Viridescent Venerer's Determination"},
	[77] = {"Wanderer's Troupe", "Wanderer's String-Kettle", "Bard's Arrow Feather", "Conductor's Top Hat", "Troupe's Dawnlight", "Concert's Final Hour"},
	[78] = {"Glacier and Snowfield", "Goblet of Aquilo", "Plume of Snow", "Crown of Glacier", "Heart of Frost", "Glacial Sand"},
	[79] = {"Thundering Fury", "Omen of Thunderstorm", "Survivor of Catastrophe", "Thunder Summoner's Crown", "Thunderbird's Mercy", "Hourglass of Thunder"},
	[80] = {"Crimson Witch of Flames", "Witch's Heart Flames", "Witch's Ever-Burning Plume", "Witch's Scorching Hat", "Witch's Flower of Blaze", "Witch's End Time"},
	[81] = {"Noblesse Oblige", "Royal Silver Urn", "Royal Plume", "Royal Masque", "Royal Flora", "Royal Pocket Watch"},
	[82] = {"Bloodstained Chivalry", "Bloodstained Chevalier's Goblet", "Bloodstained Black Plume", "Bloodstained Iron Mask", "Bloodstained Flower of Iron", "Bloodstained Final Hour"},
	[83] = {"Prayers for Illumination", NULL, NULL, "Tiara of Flame", NULL, NULL},
	[84] = {"Prayers for Destiny", NULL, NULL, "Tiara of Torrents", NULL, NULL},
	[85] = {"Prayers for Wisdom", NULL, NULL, "Tiara of Thunder", NULL, NULL},
	[86] = {"Prayers to the Firmament", NULL, NULL, "Tiara of Gales", NULL, NULL},
	[87] = {"Prayers to Springtime", NULL, NULL, "Tiara of Frost", NULL, NULL},
	[88] = {"Archaic Petra", "Goblet of Chiseled Crag", "Feather of Jagged Peaks", "Mask of Solitude Basalt", "Flower of Creviced Cliff", "Sundial of Enduring Jade"},
	[89] = {"Retracing Bolide", "Summer Night's Waterballoon", "Summer Night's Finale", "Summer Night's Mask", "Summer Night's Bloom", "Summer Night's Moment"},
	[90] = {"Heart of Depth", "Goblet of Thundering Deep", "Gust of Nostalgia", "Wine-Stained Tricorne", "Gilded Corsage", "Copper Compass"},
	[91] = {"Tenacity of the Millelith", "Noble's Pledging Vessel", "Ceremonial War-Plume", "General's Ancient Helm", "Flower of Accolades", "Orichalceous Time-Dial"},
	[92] = {"Pale Flame", "Surpassing Cup", "Wise Doctor's Pinion", "Mocking Mask", "Stainless Bloom", "Moment of Cessation"},
	[93] = {"Shimenawa's Reminiscence", "Hopeful Heart", "Shaft of Remembrance", "Capricious Visage", "Entangling Bloom", "Morning Dew's Moment"},
	[94] = {"Emblem of Severed Fate", "Scarlet Vessel", "Sundered Feather", "Ornate Kabuto", "Magnificent Tsuba", "Storm Cage"},
	[95] = {"Husk of Opulent Dreams", "Calabash of Awakening", "Plume of Luxury", "Skeletal Hat", "Bloom Times", "Song of Life"},
	[96] = {"Ocean-Hued Clam", "Pearl Cage", "Deep Palace's Plume", "Crown of Watatsumi", "Sea-Dyed Blossom", "Cowry of Parting"},
	[97] = {"Vermillion Hereafter", "Moment of the Pact", "Feather of Nascent Light", "Thundering Poise", "Flowering Life", "Solar Relic"},
	[98] = {"Echoes of an Offering", "Chalice of the Font", "Jade Leaf", "Flowing Rings", "Soulscent Bloom", "Symbol of Felicitation"},
	[99] = {"Lord of the Winds", "Lord of Wind over Firmament's Cup", "Lord of Wind over Firmament's Feather", "Lord of Wind over Firmament's Crown", "Lord of Wind over Firmament's Flower", "Lord of Wind over Firmament's Hourglass"},
};

const char* getArtifact(unsigned int id) {
	unsigned int set = id / 1000;
	unsigned int rarity = (id % 1000) / 100;
	unsigned int piece = (id % 100) / 10;
	unsigned int start_stats = id % 10;
	// Artifacts start with a max of 4 stats.
	if (start_stats > 4) return NULL;
	// Artifact piece must be between 1 and 5. (Note that a piece ID of 0 indicates the set itself and is thus allowed here.)
	if (piece > 5) return NULL;
	// Artifact rarity must be between 1 and 5.
	if (rarity < 1) return NULL;
	if (rarity > 5) return NULL;
	// As of v4.1, artifact IDs must be between 20 and 99.
	if (set < 20) return NULL;
	if (set > 99) return NULL;
	return Artifacts[set][piece];
}
