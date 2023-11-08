/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Genshin Impact Wish Simulator */
/* ©2023 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#ifndef GACHA_H
#define GACHA_H
enum {
	STD_CHR = 0,
	STD_WPN, // non-vanilla
	CHAR1,
	CHAR2,
	WPN,
	NOVICE,
	WISH_CNT
};

// Banner data
extern const unsigned short FourStarChrUp[58][3];
extern const unsigned short FiveStarChrUp[58][2];
extern const unsigned short FourStarWpnUp[58][5];
extern const unsigned short FiveStarWpnUp[58][2];
extern const unsigned short FourStarChr[37];
extern const unsigned short FiveStarChr[7];
extern const unsigned short ThreeStar[13];
extern const unsigned short FourStarWpn[18];
extern const unsigned short FiveStarWpn[10];
extern const unsigned char FourStarMaxIndex[30];
extern const unsigned char FiveStarMaxIndex[30];

// Configuration variables
extern unsigned char pity[2];
extern unsigned char pityS[4];
extern unsigned char getRateUp[2];
extern unsigned char fatePoints;
extern unsigned short epitomizedPath;
extern int doSmooth[2];
extern int doPity[2];
extern int do5050;

extern const char* const banners[WISH_CNT][2];

#ifndef DEBUG
unsigned int doAPull(unsigned int, unsigned int, unsigned int, unsigned int*, unsigned int*);
#else
unsigned int doAPull(unsigned int, int, int, unsigned int*, unsigned int*);
#endif
#endif
