/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <stddef.h>
#include "config.h"
#include "item.h"
#include "util.h"

static const char* const Artifacts[100][6] = {
	[20] = {_N("Deepwood Memories"), _N("Lamp of the Lost"), _N("Scholar of Vines"), _N("Laurel Coronet"), _N("Labyrinth Wayfarer"), _N("A Time of Insight")},
	[21] = {_N("Gilded Dreams"), _N("Honeyed Final Feast"), _N("Feather of Judgment"), _N("Shadow of the Sand King"), _N("Dreaming Steelbloom"), _N("The Sunken Years")},
	[27] = {_N("Desert Pavilion Chronicle"), _N("Defender of the Enchanting Dream"), _N("End of the Golden Realm"), _N("Legacy of the Desert High-Born"), _N("The First Days of the City of Kings"), _N("Timepiece of the Lost Path")},
	[28] = {_N("Flower of Paradise Lost"), _N("Secret-Keeper's Magic Bottle"), _N("Wilting Feast"), _N("Amethyst Crown"), _N("Ay-Khanoum's Myriad"), _N("A Moment Congealed")},
	[29] = {_N("Nymph's Dream"), _N("Heroes' Tea Party"), _N("Wicked Mage's Plumule"), _N("Fell Dragon's Monocle"), _N("Odyssean Flower"), _N("Nymph's Constancy")},
	[30] = {_N("Vourukasha's Glow"), _N("Feast of Boundless Joy"), _N("Vibrant Pinion"), _N("Heart of Khvarena's Brilliance"), _N("Stamen of Khvarena's Origin"), _N("Ancient Abscission")},
	[31] = {_N("Marechaussee Hunter"), _N("Forgotten Vessel"), _N("Masterpiece's Overture"), _N("Veteran's Visage"), _N("Hunter's Brooch"), _N("Moment of Judgment")},
	[32] = {_N("Golden Troupe"), _N("Golden Night's Bustle"), _N("Golden Bird's Shedding"), _N("Golden Troupe's Reward"), _N("Golden Song's Variation"), _N("Golden Era's Prelude")},
	[33] = {_N("Song of Days Past"), _N("Promised Dream of Days Past"), _N("Recollection of Days Past"), _N("Poem Passed Down from Days Past"), _N("Forgotten Oath of Days Past"), _N("Echoing Sound from Days Past")},
	[34] = {_N("Nighttime Whispers in the Echoing Woods"), _N("Magnanimous Ink Bottle"), _N("Honest Quill"), _N("Compassionate Ladies' Hat"), _N("Selfless Floral Accessory"), _N("Faithful Hourglass")},
	[35] = {_N("Fragment of Harmonic Whimsy"), _N("Ichor Shower Rhapsody"), _N("Ancient Sea's Nocturnal Musing"), _N("Whimsical Dance of the Withered"), _N("Harmonious Symphony Prelude"), _N("The Grand Jape of the Turning of Fate")},
	[36] = {_N("Unfinished Reverie"), _N("The Wine-Flask Over Which the Plan Was Hatched"), _N("Faded Emerald Tail"), _N("Crownless Crown"), _N("Dark Fruit of Bright Flowers"), _N("Moment of Attainment")},
	[51] = {_N("Resolution of Sojourner"), _N("Goblet of the Sojourner"), _N("Feather of Homecoming"), _N("Crown of Parting"), _N("Heart of Comradeship"), _N("Sundial of the Sojourner")},
	[52] = {_N("Brave Heart"), _N("Outset of the Brave"), _N("Prospect of the Brave"), _N("Crown of the Brave"), _N("Medal of the Brave"), _N("Fortitude of the Brave")},
	[53] = {_N("Defender's Will"), _N("Guardian's Vessel"), _N("Guardian's Sigil"), _N("Guardian's Band"), _N("Guardian's Flower"), _N("Guardian's Clock")},
	[54] = {_N("Tiny Miracle"), _N("Tiny Miracle's Goblet"), _N("Tiny Miracle's Feather"), _N("Tiny Miracle's Earrings"), _N("Tiny Miracle's Flower"), _N("Tiny Miracle's Hourglass")},
	[55] = {_N("Berserker"), _N("Berserker's Bone Goblet"), _N("Berserker's Indigo Feather"), _N("Berserker's Battle Mask"), _N("Berserker's Rose"), _N("Berserker's Timepiece")},
	[56] = {_N("Martial Artist"), _N("Martial Artist's Wine Cup"), _N("Martial Artist's Feather Accessory"), _N("Martial Artist's Bandana"), _N("Martial Artist's Red Flower"), _N("Martial Artist's Water Hourglass")},
	[57] = {_N("Instructor"), _N("Instructor's Tea Cup"), _N("Instructor's Feather Accessory"), _N("Instructor's Cap"), _N("Instructor's Brooch"), _N("Instructor's Pocket Watch")},
	[58] = {_N("Gambler"), _N("Gambler's Dice Cup"), _N("Gambler's Feather Accessory"), _N("Gambler's Earrings"), _N("Gambler's Brooch"), _N("Gambler's Pocket Watch")},
	[59] = {_N("The Exile"), _N("Exile's Goblet"), _N("Exile's Feather"), _N("Exile's Circlet"), _N("Exile's Flower"), _N("Exile's Pocket Watch")},
	[60] = {_N("Adventurer"), _N("Adventurer's Golden Goblet"), _N("Adventurer's Tail Feather"), _N("Adventurer's Bandana"), _N("Adventurer's Flower"), _N("Adventurer's Pocket Watch")},
	[61] = {_N("Lucky Dog"), _N("Lucky Dog's Goblet"), _N("Lucky Dog's Eagle Feather"), _N("Lucky Dog's Silver Circlet"), _N("Lucky Dog's Clover"), _N("Lucky Dog's Hourglass")},
	[62] = {_N("Scholar"), _N("Scholar's Ink Cup"), _N("Scholar's Quill Pen"), _N("Scholar's Lens"), _N("Scholar's Bookmark"), _N("Scholar's Clock")},
	[63] = {_N("Traveling Doctor"), _N("Traveling Doctor's Medicine Pot"), _N("Traveling Doctor's Owl Feather"), _N("Traveling Doctor's Handkerchief"), _N("Traveling Doctor's Silver Lotus"), _N("Traveling Doctor's Pocket Watch")},
	[71] = {_N("Blizzard Strayer"), _N("Frost-Weaved Dignity"), _N("Icebreaker's Resolve"), _N("Broken Rime's Echo"), _N("Snowswept Memory"), _N("Frozen Homeland's Demise")},
	[72] = {_N("Thundersoother"), _N("Thundersoother's Goblet"), _N("Thundersoother's Plume"), _N("Thundersoother's Diadem"), _N("Thundersoother's Heart"), _N("Hour of Soothing Thunder")},
	[73] = {_N("Lavawalker"), _N("Lavawalker's Epiphany"), _N("Lavawalker's Salvation"), _N("Lavawalker's Wisdom"), _N("Lavawalker's Resolution"), _N("Lavawalker's Torment")},
	[74] = {_N("Maiden Beloved"), _N("Maiden's Fleeting Leisure"), _N("Maiden's Heart-stricken Infatuation"), _N("Maiden's Fading Beauty"), _N("Maiden's Distant Love"), _N("Maiden's Passing Youth")},
	[75] = {_N("Gladiator's Finale"), _N("Gladiator's Intoxication"), _N("Gladiator's Destiny"), _N("Gladiator's Triumphus"), _N("Gladiator's Nostalgia"), _N("Gladiator's Longing")},
	[76] = {_N("Viridescent Venerer"), _N("Viridescent Venerer's Vessel"), _N("Viridescent Arrow Feather"), _N("Viridescent Venerer's Diadem"), _N("In Remembrance of Viridescent Fields"), _N("Viridescent Venerer's Determination")},
	[77] = {_N("Wanderer's Troupe"), _N("Wanderer's String-Kettle"), _N("Bard's Arrow Feather"), _N("Conductor's Top Hat"), _N("Troupe's Dawnlight"), _N("Concert's Final Hour")},
	[78] = {_N("Glacier and Snowfield"), _N("Goblet of Aquilo"), _N("Plume of Snow"), _N("Crown of Glacier"), _N("Heart of Frost"), _N("Glacial Sand")},
	[79] = {_N("Thundering Fury"), _N("Omen of Thunderstorm"), _N("Survivor of Catastrophe"), _N("Thunder Summoner's Crown"), _N("Thunderbird's Mercy"), _N("Hourglass of Thunder")},
	[80] = {_N("Crimson Witch of Flames"), _N("Witch's Heart Flames"), _N("Witch's Ever-Burning Plume"), _N("Witch's Scorching Hat"), _N("Witch's Flower of Blaze"), _N("Witch's End Time")},
	[81] = {_N("Noblesse Oblige"), _N("Royal Silver Urn"), _N("Royal Plume"), _N("Royal Masque"), _N("Royal Flora"), _N("Royal Pocket Watch")},
	[82] = {_N("Bloodstained Chivalry"), _N("Bloodstained Chevalier's Goblet"), _N("Bloodstained Black Plume"), _N("Bloodstained Iron Mask"), _N("Bloodstained Flower of Iron"), _N("Bloodstained Final Hour")},
	[83] = {_N("Prayers for Illumination"), NULL, NULL, _N("Tiara of Flame"), NULL, NULL},
	[84] = {_N("Prayers for Destiny"), NULL, NULL, _N("Tiara of Torrents"), NULL, NULL},
	[85] = {_N("Prayers for Wisdom"), NULL, NULL, _N("Tiara of Thunder"), NULL, NULL},
	[86] = {_N("Prayers to the Firmament"), NULL, NULL, _N("Tiara of Gales"), NULL, NULL},
	[87] = {_N("Prayers to Springtime"), NULL, NULL, _N("Tiara of Frost"), NULL, NULL},
	[88] = {_N("Archaic Petra"), _N("Goblet of Chiseled Crag"), _N("Feather of Jagged Peaks"), _N("Mask of Solitude Basalt"), _N("Flower of Creviced Cliff"), _N("Sundial of Enduring Jade")},
	[89] = {_N("Retracing Bolide"), _N("Summer Night's Waterballoon"), _N("Summer Night's Finale"), _N("Summer Night's Mask"), _N("Summer Night's Bloom"), _N("Summer Night's Moment")},
	[90] = {_N("Heart of Depth"), _N("Goblet of Thundering Deep"), _N("Gust of Nostalgia"), _N("Wine-Stained Tricorne"), _N("Gilded Corsage"), _N("Copper Compass")},
	[91] = {_N("Tenacity of the Millelith"), _N("Noble's Pledging Vessel"), _N("Ceremonial War-Plume"), _N("General's Ancient Helm"), _N("Flower of Accolades"), _N("Orichalceous Time-Dial")},
	[92] = {_N("Pale Flame"), _N("Surpassing Cup"), _N("Wise Doctor's Pinion"), _N("Mocking Mask"), _N("Stainless Bloom"), _N("Moment of Cessation")},
	[93] = {_N("Shimenawa's Reminiscence"), _N("Hopeful Heart"), _N("Shaft of Remembrance"), _N("Capricious Visage"), _N("Entangling Bloom"), _N("Morning Dew's Moment")},
	[94] = {_N("Emblem of Severed Fate"), _N("Scarlet Vessel"), _N("Sundered Feather"), _N("Ornate Kabuto"), _N("Magnificent Tsuba"), _N("Storm Cage")},
	[95] = {_N("Husk of Opulent Dreams"), _N("Calabash of Awakening"), _N("Plume of Luxury"), _N("Skeletal Hat"), _N("Bloom Times"), _N("Song of Life")},
	[96] = {_N("Ocean-Hued Clam"), _N("Pearl Cage"), _N("Deep Palace's Plume"), _N("Crown of Watatsumi"), _N("Sea-Dyed Blossom"), _N("Cowry of Parting")},
	[97] = {_N("Vermillion Hereafter"), _N("Moment of the Pact"), _N("Feather of Nascent Light"), _N("Thundering Poise"), _N("Flowering Life"), _N("Solar Relic")},
	[98] = {_N("Echoes of an Offering"), _N("Chalice of the Font"), _N("Jade Leaf"), _N("Flowing Rings"), _N("Soulscent Bloom"), _N("Symbol of Felicitation")},
	[99] = {_N("Lord of the Winds"), _N("Lord of Wind over Firmament's Cup"), _N("Lord of Wind over Firmament's Feather"), _N("Lord of Wind over Firmament's Crown"), _N("Lord of Wind over Firmament's Flower"), _N("Lord of Wind over Firmament's Hourglass")},
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
	return gettext(Artifacts[set][piece]);
}
