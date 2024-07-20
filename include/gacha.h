/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
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
	WPN1,
	WPN2,
	NOVICE,
	STD_ONLY_CHR,
	WARP_CNT
};

extern const char* const banners[WARP_CNT][2];

// Banner data
#define IDX_MAX 12
extern const unsigned short FourStarChrUp[IDX_MAX*2][3];
extern const unsigned short FiveStarChrUp[IDX_MAX*2][2];
extern const unsigned short FourStarWpnUp[IDX_MAX*2][3];
extern const unsigned short FiveStarWpnUp[IDX_MAX*2][2];
extern const unsigned char FourStarChrMaxIndex[IDX_MAX];
extern const unsigned char FourStarWpnMaxIndex[IDX_MAX];
extern const unsigned short FourStarChr[22];
extern const unsigned short FourStarWpn[24];
extern const unsigned short FiveStarChr[7];
extern const unsigned short FiveStarWpn[7];

// Configuration variables
extern unsigned char pity[2];
extern unsigned char pityS[4];
extern unsigned char getRateUp[2];
extern int doSmooth[2];
extern int doPity[2];
extern int do5050;

// Main gacha function
#ifndef DEBUG
unsigned int doAPull(unsigned int, unsigned int, unsigned int, unsigned int*, unsigned int*);
#else
unsigned int doAPull(unsigned int, int, int, unsigned int*, unsigned int*);
#endif
#endif
