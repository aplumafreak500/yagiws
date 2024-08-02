/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include "item.h"

static const char* const ThreeStarLightCones[21] = {
	"Arrows",
	"Cornucopia",
	"Collapsing Sky",
	"Amber",
	"Void",
	"Chorus",
	"Data Bank",
	"Darting Arrow",
	"Fine Fruit",
	"Shattered Home",
	"Defense",
	"Loop",
	"Meshing Cogs",
	"Passkey",
	"Adversarial",
	"Multiplication",
	"Mutual Demise",
	"Pioneering",
	"Hidden Shadow",
	"Mediation",
	"Sagacity",
};

static const char* const FourStarLightCones[48] = {
	"Post-Op Conversation",
	"Good Night and Sleep Well",
	"Day One of My New Life",
	"Only Silence Remains",
	"Memories of the Past",
	"The Moles Welcome You",
	"The Birth of the Self",
	"Shared Feeling",
	"Eyes of the Prey",
	"Landau's Choice",
	"Swordplay",
	"Planetary Rendezvous",
	"A Secret Vow",
	"Make the World Clamor",
	"Perfect Timing",
	"Resolution Shines As Pearls of Sweat",
	"Trend of the Universal Market",
	"Subscribe for More!",
	"Dance! Dance! Dance!",
	"Under the Blue Sky",
	"Geniuses' Repose",
	"Quid Pro Quo",
	"Fermata",
	"We Are Wildfire",
	"River Flows in Spring",
	"Past and Future",
	"Woof! Walk Time!",
	"The Seriousness of Breakfast",
	"Warmth Shortens Cold Nights",
	"We Will Meet Again",
	"This Is Me!",
	"Return to Darkness",
	"Carve the Moon, Weave the Clouds",
	"Nowhere to Run",
	"Today Is Another Peaceful Day",
	"What Is Real?"
	"Dreamville Adventure",
	"Final Victor",
	"Flames Afar",
	"Destiny's Threads Forewoven",
	"The Day The Cosmos Fell",
	"It's Showtime",
	"Indelible Promise",
	"Concert for Two",
	"Boundless Choreo",
	"After the Charmony Fall",
	"Poised to Bloom",
	"Shadowed by Night",
};

static const char* const FourStarLightCones_Series2[3] = {
	"Before the Tutorial Mission Starts",
	"Hey, Over Here",
	"For Tomorrow's Journey",
};

static const char* const FiveStarLightCones[33] = {
	"Night on the Milky Way",
	"In the Night",
	"Something Irreplaceable",
	"But the Battle Isn't Over",
	"In the Name of the World",
	"Moment of Victory",
	"Patience Is All You Need",
	"Incessant Rain",
	"Echoes of the Coffin",
	"The Unreachable Side",
	"Before Dawn",
	"She Already Shut Her Eyes",
	"Sleep Like the Dead",
	"Time Waits for No One",
	"I Shall Be My Own Sword",
	"Brighter Than the Sun",
	"Worrisome, Blissful",
	"Night of Fright",
	"An Instant Before A Gaze",
	"Past Self in Mirror",
	"Baptism of Pure Thought",
	"Earthly Escapade",
	"Reforged Remembrance",
	"Inherently Unjust Destiny",
	"Along the Passing Shore",
	"Whereabouts Should Dreams Rest",
	"Flowing Nightglow",
	"Sailing Towards A Second Life",
	"Yet Hope Is Priceless",
	"Those Many Springs",
	"Dance at Sunset",
	"I Venture Forth to Hunt",
	"Scent Alone Stays True",
};

static const char* const FiveStarLightCones_Series2[5] = {
	"On the Fall of an Aeon",
	"Cruising in the Stellar Sea",
	"Texture of Memories",
	"Solitary Healing",
	"Eternal Calculus",
};

const char* getLightcone(unsigned int id) {
	if (id < 20000) return NULL;
	if (id >= 20000 && id <= 20021) return ThreeStarLightCones[id - 20000];
	if (id >= 21000 && id <= 21046) return FourStarLightCones[id - 21000];
	if (id >= 22000 && id <= 22002) return FourStarLightCones_Series2[id - 22000];
	if (id >= 23000 && id <= 23032) return FiveStarLightCones[id - 23000];
	if (id >= 24000 && id <= 24004) return FiveStarLightCones_Series2[id - 24000];
	return NULL;
}
