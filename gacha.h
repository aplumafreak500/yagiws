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
#endif
