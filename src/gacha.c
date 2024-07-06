/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include <stddef.h>
#include "config.h"
#ifdef HAVE_SYS_RANDOM_H
#include <sys/random.h>
#endif
#include "gacha.h"
#include "util.h"

unsigned char pity[2];
unsigned char pityS[4];
unsigned char getRateUp[2];

int doSmooth[2] = {1, 1};
int doPity[2] = {1, 1};
int do5050 = 1;

const char* const banners[WARP_CNT][2] = {
	[CHAR1] = {"char1", "Character Event Warp"},
	[CHAR2] = {"char2", "Character Event Warp-2"},
	[WPN1] = {"lc1", "Light Cone Event Warp"},
	[WPN2] = {"lc2", "Light Cone Event Warp-2"},
	[STD_CHR] = {"std", "Stellar Warp"},
	[STD_WPN] = {"std_lc", "Stellar Warp (Light Cones)"},
	[STD_ONLY_CHR] = {"std_char", "Stellar Warp (Characters Only)"},
	[NOVICE] = {"novice", "Departure Warp"},
};

// TODO: The below pity curves are derived from Genshin Impact's and are unconfirmed, particularly the 5-star lightcone banner's pity curve.
// TODO: There is a different linear rise for standard prior to reaching soft pity. Figure out what it is or if it even exists.
static long double getWeight5(unsigned int _pity) {
	if (_pity <= 73 || !doPity[1]) return 0.006l;
	return 0.006l + 0.06l * (long double) (_pity - 73);
}

static long double getWeight4(unsigned int _pity) {
	if (_pity <= 8 || !doPity[0]) return 0.051l;
	return 0.051l + 0.51l * (long double) (_pity - 8);
}

static long double getWeight5W(unsigned int _pity) {
	if (_pity <= 62 || !doPity[1]) return 0.008l;
	else if (_pity <= 73) return 0.008l + 0.08l * (long double) (_pity - 62);
	return 0.888l + 0.08l * (long double) (_pity - 73);
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
		return 0.926l;
	case 4:
		return getWeight4W(_pity);
	case 5:
		return getWeight5W(_pity);
	}
}

// "Smoothening" function.
// If it's disabled, it still needs to be called, since character vs weapon still needs to be decided.
// TODO: Find out if Star Rail still has this.
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

// 4-star lightcone banner variant
static long double getWeight4SW(unsigned int _pity) {
	if (!doSmooth[0]) return 0.5f;
	if (_pity <= 14) return 0.03l;
	return 0.03l + 0.3l * (long double) (_pity - 14);
}

/*
TODO: It is currently possible to lose the event-rate chance, but get the rate-up item anyways. This is most prominently apparent in:
	* 4-stars in the Character and Light Cone Event Warps; and
	* 5-stars in the Character and Light Cone Event Warps, if the rate-up item is also in the standard pool.
In the future, this should be handled by either:
	1) treating this as actually an event-rate win, or
	2) rerolling.
*/
// TODO Ignoring getrandom's return value should not really be done.
#ifndef DEBUG
unsigned int doAPull(unsigned int banner, unsigned int stdPoolIndex, unsigned int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#else
unsigned int doAPull(unsigned int banner, int stdPoolIndex, int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#endif
	unsigned long long rnd;
	long double rndF;
	unsigned int maxIdx;
	unsigned int minIdx;
	if (banner >= WARP_CNT) return -1;
	if (rare == NULL) return -1;
	if (isRateUp == NULL) return -1;
	long double (*_getWeight)(unsigned int, unsigned int) = getWeight;
	if (banner == WPN1 || banner == WPN2 || banner == STD_WPN) _getWeight = getWeightW;
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
	if (do5050 == 0) {
		getRateUp[0] = 0;
		getRateUp[1] = 0;
	}
	else if (do5050 < 0) {
		getRateUp[0] = 1;
		getRateUp[1] = 1;
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
			if (!getRateUp[1]) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 2 == 0) {
				*isRateUp = 1;
				getRateUp[1] = 0;
				return FiveStarChrUp[bannerIndex][banner - CHAR1];
			}
			*isRateUp = 0;
			getRateUp[1] = 1;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarChr[rnd % 7];
		case WPN1:
		case WPN2:
			// Lightcone banners don't not use the stable function for 5-stars
			pityS[2] = 0;
			pityS[3] = 0;
			if (!getRateUp[1]) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 4 < 3) {
				*isRateUp = 1;
				getRateUp[1] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarWpnUp[bannerIndex][banner - WPN1];
			}
			else {
				*isRateUp = 0;
				getRateUp[1] = 1;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarWpn[rnd % 7];
			}
		case NOVICE:
		case STD_ONLY_CHR: // Same drops for 5-stars in this case
			// Novice banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			// Novice banner does not use the stable function
			pityS[2] = 0;
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarChr[rnd % 7];
		case STD_WPN:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			// There's no point to use the stable function here, because then the banner's drop rates would be nearly identical to the vanilla standard banner
			pityS[2] = 0;
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarWpn[rnd % 7];
		case STD_CHR:
		default:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[1] = 0;
			rndF = rndFloat();
			if (pityS[2] <= pityS[3]) {
				if (rndF <= getWeight5S(pityS[3])) {
					pityS[3] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FiveStarWpn[rnd % 7];
				}
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarChr[rnd % 7];
			}
			if (rndF <= getWeight5S(pityS[2])) {
				pityS[2] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FiveStarChr[rnd % 7];
			}
			pityS[3] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FiveStarWpn[rnd % 7];
		}
	}
	else if (rndF <= _getWeight(pity[0], 4)) {
		*rare = 4;
		pity[0] = 0;
		switch (banner) {
		case CHAR1:
		case CHAR2:
			if (!getRateUp[0]) {
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
					return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
		case WPN1:
		case WPN2:
			if (!getRateUp[0]) {
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
					return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
		case NOVICE:
			*isRateUp = 0;
			// Novice banner does not use the rate-up function
			getRateUp[0] = 0;
			// Novice banner does not use the stable function
			pityS[0] = 0;
			pityS[1] = 0;
			rndF = rndFloat();
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
		case STD_CHR:
		case STD_ONLY_CHR:
		default:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4S(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
		case STD_WPN:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4SW(pityS[1])) {
					pityS[1] = 0;
					getrandom(&rnd, sizeof(long long), 0);
					return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarChrMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % FourStarWpnMaxIndex[stdPoolIndex]];;
		}
	}
	else {
		*isRateUp = 0;
		*rare = 3;
		getrandom(&rnd, sizeof(long long), 0);
		return (rnd % 21) + 20000;
	}
}
