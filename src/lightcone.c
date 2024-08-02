/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include "item.h"
#include "util.h"

static const char* const ThreeStarLightCones[21] = {
	_N("Arrows"),
	_N("Cornucopia"),
	_N("Collapsing Sky"),
	_N("Amber"),
	_N("Void"),
	_N("Chorus"),
	_N("Data Bank"),
	_N("Darting Arrow"),
	_N("Fine Fruit"),
	_N("Shattered Home"),
	_N("Defense"),
	_N("Loop"),
	_N("Meshing Cogs"),
	_N("Passkey"),
	_N("Adversarial"),
	_N("Multiplication"),
	_N("Mutual Demise"),
	_N("Pioneering"),
	_N("Hidden Shadow"),
	_N("Mediation"),
	_N("Sagacity"),
};

static const char* const FourStarLightCones[48] = {
	_N("Post-Op Conversation"),
	_N("Good Night and Sleep Well"),
	_N("Day One of My New Life"),
	_N("Only Silence Remains"),
	_N("Memories of the Past"),
	_N("The Moles Welcome You"),
	_N("The Birth of the Self"),
	_N("Shared Feeling"),
	_N("Eyes of the Prey"),
	_N("Landau's Choice"),
	_N("Swordplay"),
	_N("Planetary Rendezvous"),
	_N("A Secret Vow"),
	_N("Make the World Clamor"),
	_N("Perfect Timing"),
	_N("Resolution Shines As Pearls of Sweat"),
	_N("Trend of the Universal Market"),
	_N("Subscribe for More!"),
	_N("Dance! Dance! Dance!"),
	_N("Under the Blue Sky"),
	_N("Geniuses' Repose"),
	_N("Quid Pro Quo"),
	_N("Fermata"),
	_N("We Are Wildfire"),
	_N("River Flows in Spring"),
	_N("Past and Future"),
	_N("Woof! Walk Time!"),
	_N("The Seriousness of Breakfast"),
	_N("Warmth Shortens Cold Nights"),
	_N("We Will Meet Again"),
	_N("This Is Me!"),
	_N("Return to Darkness"),
	_N("Carve the Moon, Weave the Clouds"),
	_N("Nowhere to Run"),
	_N("Today Is Another Peaceful Day"),
	_N("What Is Real?"),
	_N("Dreamville Adventure"),
	_N("Final Victor"),
	_N("Flames Afar"),
	_N("Destiny's Threads Forewoven"),
	_N("The Day The Cosmos Fell"),
	_N("It's Showtime"),
	_N("Indelible Promise"),
	_N("Concert for Two"),
	_N("Boundless Choreo"),
	_N("After the Charmony Fall"),
	_N("Poised to Bloom"),
	_N("Shadowed by Night"),
};

static const char* const FourStarLightCones_Series2[3] = {
	_N("Before the Tutorial Mission Starts"),
	_N("Hey, Over Here"),
	_N("For Tomorrow's Journey"),
};

static const char* const FiveStarLightCones[33] = {
	_N("Night on the Milky Way"),
	_N("In the Night"),
	_N("Something Irreplaceable"),
	_N("But the Battle Isn't Over"),
	_N("In the Name of the World"),
	_N("Moment of Victory"),
	_N("Patience Is All You Need"),
	_N("Incessant Rain"),
	_N("Echoes of the Coffin"),
	_N("The Unreachable Side"),
	_N("Before Dawn"),
	_N("She Already Shut Her Eyes"),
	_N("Sleep Like the Dead"),
	_N("Time Waits for No One"),
	_N("I Shall Be My Own Sword"),
	_N("Brighter Than the Sun"),
	_N("Worrisome, Blissful"),
	_N("Night of Fright"),
	_N("An Instant Before A Gaze"),
	_N("Past Self in Mirror"),
	_N("Baptism of Pure Thought"),
	_N("Earthly Escapade"),
	_N("Reforged Remembrance"),
	_N("Inherently Unjust Destiny"),
	_N("Along the Passing Shore"),
	_N("Whereabouts Should Dreams Rest"),
	_N("Flowing Nightglow"),
	_N("Sailing Towards A Second Life"),
	_N("Yet Hope Is Priceless"),
	_N("Those Many Springs"),
	_N("Dance at Sunset"),
	_N("I Venture Forth to Hunt"),
	_N("Scent Alone Stays True"),
};

static const char* const FiveStarLightCones_Series2[5] = {
	_N("On the Fall of an Aeon"),
	_N("Cruising in the Stellar Sea"),
	_N("Texture of Memories"),
	_N("Solitary Healing"),
	_N("Eternal Calculus"),
};

const char* getLightcone(unsigned int id) {
	if (id < 20000) return NULL;
	if (id >= 20000 && id <= 20021) return gettext(ThreeStarLightCones[id - 20000]);
	if (id >= 21000 && id <= 21046) return gettext(FourStarLightCones[id - 21000]);
	if (id >= 22000 && id <= 22002) return gettext(FourStarLightCones_Series2[id - 22000]);
	if (id >= 23000 && id <= 23032) return gettext(FiveStarLightCones[id - 23000]);
	if (id >= 24000 && id <= 24004) return gettext(FiveStarLightCones_Series2[id - 24000]);
	return NULL;
}
