/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
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
unsigned char fatePoints;
unsigned short epitomizedPath;

int doSmooth[2] = {1, 1};
int doPity[2] = {1, 1};
int do5050 = 1;

const char* const banners[WISH_CNT][2] = {
	[CHAR1] = {"char1", "Character Event Wish"},
	[CHAR2] = {"char2", "Character Event Wish-2"},
	[WPN] = {"weapon", "Weapon Event Wish"},
	[STD_CHR] = {"std", "Wanderlust Invocation"},
	[STD_WPN] = {"std_weapon", "Wanderlust Incocation (Weapons)"},
	[NOVICE] = {"novice", "Beginners' Wish"},
	[CHRONICLED] = {"chronicle", "Chronicled Wish"}
};

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

/*
TODO: It is currently possible to lose the event-rate chance, but get the rate-up item anyways. This is most prominently apparent in:
	* 5-stars in the Chronicled Wish;
	* 4-stars in the Character and Weapon Event Wishes; and
	* 5-stars in the Character and Weapon Event Wishes, if the rate-up item is also	in the standard pool.
In the future, this should be handled by either:
	1) treating this as actually a event-rate win, or
	2) rerolling.
*/
#ifndef DEBUG
unsigned int doAPull(unsigned int banner, unsigned int stdPoolIndex, unsigned int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#else
unsigned int doAPull(unsigned int banner, int stdPoolIndex, int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#endif
	unsigned long long rnd;
	long double rndF;
	unsigned int maxIdx;
	unsigned int minIdx;
	const unsigned short* pool;
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
			if (fatePoints < 2 && !getRateUp[1]) {
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
		case CHRONICLED:
			// 60 = v4.4 phase 1
			if (ChroniclePool[bannerIndex - 60] == NULL) {
				return -1;
			}
			pool = ChroniclePool[bannerIndex - 60]->FiveStarPool;
			minIdx = 0;
			maxIdx = ChroniclePool[bannerIndex - 60]->FiveStarWeaponCount + ChroniclePool[bannerIndex - 60]->FiveStarCharCount;

			if (epitomizedPath) {
				// If Chronicled Path is set, behave like a character event banner.
				// No point to use the stable function in this case.
				pityS[2] = 0;
				pityS[3] = 0;
				if (epitomizedPath >= 10000) {
					minIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;
				}
				else {
					maxIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;
				}
				if (!getRateUp[1]) {
					getrandom(&rnd, sizeof(long long), 0);
				}
				else rnd = 0;
				if (rnd % 2 == 0) {
					*isRateUp = 1;
					getRateUp[1] = 0;
					fatePoints = 0;
					return epitomizedPath;
				}
				*isRateUp = 0;
				getRateUp[1] = 1;
				fatePoints++;
				getrandom(&rnd, sizeof(long long), 0);
				return pool[(rnd % (maxIdx - minIdx)) + minIdx];
			}
			else {
				// Else, behave like the standard banner.
				// Signal that all pulls are rate-up in this case.
				*isRateUp = 1;
				getRateUp[1] = 0;
				fatePoints = 0;
				rndF = rndFloat();
				if (pityS[2] <= pityS[3]) {
					if (rndF <= getWeight5S(pityS[3])) {
						pityS[3] = 0;
						minIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;

					}
					else {
						pityS[2] = 0;
						maxIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;
					}
				}
				else {
					if (rndF <= getWeight5S(pityS[2])) {
						pityS[2] = 0;
						maxIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;
					}
					else {
						pityS[3] = 0;
						minIdx = ChroniclePool[bannerIndex - 60]->FiveStarCharCount;
					}
				}
			}
			getrandom(&rnd, sizeof(long long), 0);
			return pool[(rnd % (maxIdx - minIdx)) + minIdx];
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
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[(rnd % FourStarMaxIndex[stdPoolIndex]) + 3];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[(rnd % FourStarMaxIndex[stdPoolIndex]) + 3];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case WPN:
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
					return FourStarWpn[rnd % 18];
				}
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[(rnd % FourStarMaxIndex[stdPoolIndex]) + 3];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[(rnd % FourStarMaxIndex[stdPoolIndex]) + 3];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case CHRONICLED:
			// 60 = v4.4 phase 1
			if (ChroniclePool[bannerIndex - 60] == NULL) {
				return -1;
			}
			*isRateUp = 1;
			getRateUp[0] = 0;
			pool = ChroniclePool[bannerIndex - 60]->FourStarPool;
			minIdx = 0;
			maxIdx = ChroniclePool[bannerIndex - 60]->FourStarWeaponCount + ChroniclePool[bannerIndex - 60]->FourStarCharCount;

			rndF = rndFloat();
			if (pityS[0] <= pityS[1]) {
				if (rndF <= getWeight4S(pityS[1])) {
					pityS[1] = 0;
					minIdx = ChroniclePool[bannerIndex - 60]->FourStarCharCount;

				}
				else {
					pityS[0] = 0;
					maxIdx = ChroniclePool[bannerIndex - 60]->FourStarCharCount;
				}
			}
			else {
				if (rndF <= getWeight4S(pityS[0])) {
					pityS[0] = 0;
					maxIdx = ChroniclePool[bannerIndex - 60]->FourStarCharCount;
				}
				else {
					pityS[1] = 0;
					minIdx = ChroniclePool[bannerIndex - 60]->FourStarCharCount;
				}
			}
			getrandom(&rnd, sizeof(long long), 0);
			return pool[(rnd % (maxIdx - minIdx)) + minIdx];
		case NOVICE:
			*isRateUp = 0;
			// Novice banner does not use the rate-up function
			getRateUp[0] = 0;
			// Novice banner does not use the stable function
			pityS[0] = 0;
			pityS[1] = 0;
			rndF = rndFloat();
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarChr[(rnd % FourStarMaxIndex[stdPoolIndex]) + 3];
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
				return FourStarChr[rnd % (FourStarMaxIndex[stdPoolIndex] + 3)];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % (FourStarMaxIndex[stdPoolIndex] + 3)];
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
				return FourStarChr[rnd % (FourStarMaxIndex[stdPoolIndex] + 3)];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % (FourStarMaxIndex[stdPoolIndex] + 3)];
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
