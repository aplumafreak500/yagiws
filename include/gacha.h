/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#ifndef GACHA_H
#define GACHA_H
// Banner types
enum {
	STD_CHR = 0,
	STD_WPN, // non-vanilla
	CHAR1,
	CHAR2,
	WPN,
	NOVICE,
	CHRONICLED,
	STD_ONLY_CHR,
	WISH_CNT
};
extern const char* const banners[WISH_CNT][2];

// Chronicled Wish
typedef struct {
	const unsigned short* FiveStarPool;
	unsigned int FiveStarCharCount;
	unsigned int FiveStarWeaponCount;
	const unsigned short* FourStarPool;
	unsigned int FourStarCharCount;
	unsigned int FourStarWeaponCount;
} ChroniclePool_t;
const ChroniclePool_t* getChroniclePool(unsigned short);

// Banner data
#define IDX_MAX 36
extern const unsigned short FourStarChrUp[IDX_MAX*2][3];
extern const unsigned short FiveStarChrUp[IDX_MAX*2][2];
extern const unsigned short FourStarWpnUp[IDX_MAX*2][5];
extern const unsigned short FiveStarWpnUp[IDX_MAX*2][2];
extern const unsigned short FourStarChr[41];
extern const unsigned short FiveStarChr[7];
extern const unsigned short ThreeStar[13];
extern const unsigned short FourStarWpn[18];
extern const unsigned short FiveStarWpn[10];
extern const unsigned char FourStarMaxIndex[IDX_MAX];
extern const unsigned char FiveStarMaxIndex[IDX_MAX];

// Configuration variables
extern unsigned char pity[2];
extern unsigned char pityS[4];
extern unsigned char getRateUp[2];
extern unsigned char fatePoints;
extern unsigned short epitomizedPath;
extern int doSmooth[2];
extern int doPity[2];
extern int do5050;
extern int doEpitomized;
extern int doRadiance;

// Main gacha function
#ifndef DEBUG
unsigned int doAPull(unsigned int, unsigned int, unsigned int, unsigned int*, unsigned int*);
#else
unsigned int doAPull(unsigned int, int, int, unsigned int*, unsigned int*);
#endif
#endif
