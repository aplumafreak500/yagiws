/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <stddef.h>
#include <sys/random.h>
#include "gacha.h"
#include "util.h"

unsigned char pity[2];
unsigned char pityS[4];
unsigned char getRateUp[2];
unsigned char fatePoints;
unsigned short epitomizedPath;

int doRadiance = 0;
int doEpitomized = -1;
int doSmooth[2] = {1, 1};
int doPity[2] = {1, 1};
int do5050 = 1;

const char* const banners[WISH_CNT][2] = {
	[CHAR1] = {"char1", _N("Character Event Wish")},
	[CHAR2] = {"char2", _N("Character Event Wish-2")},
	[WPN] = {"weapon", _N("Weapon Event Wish")},
	[STD_CHR] = {"std", _N("Wanderlust Invocation")},
	[STD_WPN] = {"std_weapon", _N("Wanderlust Incocation (Weapons)")},
	[STD_ONLY_CHR] = {"std_char", _N("Wanderlust Incocation (Characters Only)")},
	[NOVICE] = {"novice", _N("Beginners' Wish")},
	[CHRONICLED] = {"chronicle", _N("Chronicled Wish")}
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
	* 4-stars in the Character and Weapon Event Wishes; and
	* 5-stars in the Character and Weapon Event Wishes, if the rate-up item is also	in the standard pool.
In the future, this should be handled by either:
	1) treating this as actually a event-rate win, or
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
	const unsigned short* pool;
	const ChroniclePool_t* ChroniclePool = getChroniclePool(bannerIndex);
	if (banner >= WISH_CNT) return -1;
	if (rare == NULL) return -1;
	if (isRateUp == NULL) return -1;
	long double (*_getWeight)(unsigned int, unsigned int) = getWeight;
	if (banner == WPN || banner == STD_WPN) _getWeight = getWeightW;
	if (doPity[0]) pity[0]++;
	if (doPity[1]) pity[1]++;
	if (doSmooth[0] > 0) {
		pityS[0]++;
		pityS[1]++;
	}
	if (doSmooth[1] > 0) {
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
	if (doRadiance < 0) {
		// TODO: Check banner version index
		doRadiance = 0;
	}
	if (doEpitomized < 0) {
		// TODO: Check banner version index
		doRadiance = banner == CHRONICLED ? 1 : 2;
	}
	if (doEpitomized == 0) {
		fatePoints = 0;
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
			if (doRadiance) {
				getrandom(&rnd, sizeof(long long), 0);
				if (rnd % 10 == 0) {
					*isRateUp = 2;
					getRateUp[1] = 0;
					// Character banners don't use Fate Points, but best to reset them anyway
					fatePoints = 0;
					return FiveStarChrUp[bannerIndex][banner - CHAR1];
				}
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
			if (fatePoints < doEpitomized && !getRateUp[1]) {
				getrandom(&rnd, sizeof(long long), 0);
			}
			else rnd = 0;
			if (rnd % 4 < 3) {
				*isRateUp = 1;
				getRateUp[1] = 0;
				if (fatePoints >= doEpitomized) {
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
			if (ChroniclePool == NULL) {
				return -1;
			}
			pool = ChroniclePool->FiveStarPool;
			minIdx = 0;
			maxIdx = ChroniclePool->FiveStarWeaponCount + ChroniclePool->FiveStarCharCount;

			if (epitomizedPath && doEpitomized) {
				// If Chronicled Path is set, behave like a weapon event banner.
				// No point to use the stable function in this case.
				pityS[2] = 0;
				pityS[3] = 0;
				if (epitomizedPath >= 10000) {
					minIdx = ChroniclePool->FiveStarCharCount;
				}
				else {
					maxIdx = ChroniclePool->FiveStarCharCount;
				}
				if (fatePoints < doEpitomized && !getRateUp[1]) {
					getrandom(&rnd, sizeof(long long), 0);
				}
				else rnd = 0;
				if (rnd % 2 == 0) {
					*isRateUp = 1;
					getRateUp[1] = 0;
					fatePoints = 0;
					return epitomizedPath;
				}
				else {
					getrandom(&rnd, sizeof(long long), 0);
					if (pool[(rnd % (maxIdx - minIdx)) + minIdx] == epitomizedPath) {
						*isRateUp = 1;
						getRateUp[1] = 0;
						fatePoints = 0;
					}
					else {
						*isRateUp = 0;
						getRateUp[1] = 1;
						fatePoints++;
					}
					return pool[(rnd % (maxIdx - minIdx)) + minIdx];
				}
			}
			else {
				// Else, behave like the standard banner.
				// Signal that all pulls are rate-up in this case.
				*isRateUp = 1;
				getRateUp[1] = 0;
				fatePoints = 0;
				rndF = rndFloat();
				if (doSmooth[1] >= 0) {
					if (pityS[2] <= pityS[3]) {
						if (rndF <= getWeight5S(pityS[3])) {
							pityS[3] = 0;
							minIdx = ChroniclePool->FiveStarCharCount;

						}
						else {
							pityS[2] = 0;
							maxIdx = ChroniclePool->FiveStarCharCount;
						}
					}
					else {
						if (rndF <= getWeight5S(pityS[2])) {
							pityS[2] = 0;
							maxIdx = ChroniclePool->FiveStarCharCount;
						}
						else {
							pityS[3] = 0;
							minIdx = ChroniclePool->FiveStarCharCount;
						}
					}
				}
			}
			getrandom(&rnd, sizeof(long long), 0);
			return pool[(rnd % (maxIdx - minIdx)) + minIdx];
		case NOVICE:
		case STD_ONLY_CHR: // Same drops for 5-stars in this case
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
			if (doSmooth[1] < 0) {
				minIdx = FiveStarMaxIndex[stdPoolIndex];
				maxIdx = minIdx + 10;
				getrandom(&rnd, sizeof(long long), 0);
				if ((rnd % maxIdx) < minIdx) {
					return FiveStarChr[rnd % minIdx];
				}
				else {
					minIdx = (rnd % maxIdx) - minIdx;
					return FiveStarWpn[minIdx];
				}
			}
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
			if (doSmooth[0] < 0) {
				minIdx = FourStarMaxIndex[stdPoolIndex];
				maxIdx = minIdx + 18;
				getrandom(&rnd, sizeof(long long), 0);
				if ((rnd % maxIdx) < minIdx) {
					return FourStarChr[(rnd % minIdx) + 3];
				}
				else {
					minIdx = (rnd % maxIdx) - minIdx;
					return FourStarWpn[minIdx];
				}
			}
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
			if (doSmooth[0] < 0) {
				minIdx = FourStarMaxIndex[stdPoolIndex];
				maxIdx = minIdx + 18;
				getrandom(&rnd, sizeof(long long), 0);
				if ((rnd % maxIdx) < minIdx) {
					return FourStarChr[(rnd % minIdx) + 3];
				}
				else {
					minIdx = (rnd % maxIdx) - minIdx;
					return FourStarWpn[minIdx];
				}
			}
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
			if (ChroniclePool == NULL) {
				return -1;
			}
			*isRateUp = 1;
			getRateUp[0] = 0;
			pool = ChroniclePool->FourStarPool;
			minIdx = 0;
			maxIdx = ChroniclePool->FourStarWeaponCount + ChroniclePool->FourStarCharCount;

			rndF = rndFloat();
			if (doSmooth[0] >= 0) {
				if (pityS[0] <= pityS[1]) {
					if (rndF <= getWeight4S(pityS[1])) {
						pityS[1] = 0;
						minIdx = ChroniclePool->FourStarCharCount;

					}
					else {
						pityS[0] = 0;
						maxIdx = ChroniclePool->FourStarCharCount;
					}
				}
				else {
					if (rndF <= getWeight4S(pityS[0])) {
						pityS[0] = 0;
						maxIdx = ChroniclePool->FourStarCharCount;
					}
					else {
						pityS[1] = 0;
						minIdx = ChroniclePool->FourStarCharCount;
					}
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
		case STD_ONLY_CHR:
		default:
			// Standard banner does not use the rate-up function
			*isRateUp = 0;
			getRateUp[0] = 0;
			rndF = rndFloat();
			if (doSmooth[0] < 0) {
				minIdx = FourStarMaxIndex[stdPoolIndex] + 3;
				maxIdx = minIdx + 18;
				getrandom(&rnd, sizeof(long long), 0);
				if ((rnd % maxIdx) < minIdx) {
					return FourStarChr[rnd % minIdx];
				}
				else {
					minIdx = (rnd % maxIdx) - minIdx;
					return FourStarWpn[minIdx];
				}
			}
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
			if (doSmooth[0] < 0) {
				minIdx = FourStarMaxIndex[stdPoolIndex] + 3;
				maxIdx = minIdx + 18;
				getrandom(&rnd, sizeof(long long), 0);
				if ((rnd % maxIdx) < minIdx) {
					return FourStarChr[rnd % minIdx];
				}
				else {
					minIdx = (rnd % maxIdx) - minIdx;
					return FourStarWpn[minIdx];
				}
			}
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
