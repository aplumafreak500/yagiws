/* Yet another Genshin Impact wish simulator */
/* ©2023 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

/*
Release History:
	* v1.0 (2023-10-20):
		* Initial release
	* v1.0.1 (2023-11-04):
		* Add 4.1 weapons and initial support for 4.2
		* Fix getWeight4SW using smooth weight even when smooth function is disabled
	* v1.1 (2023-11-05):
		* Add support for multiple banners. Choose one with the -B switch.
		* Debug mode that disables most sanity checks. (Beware of segfaults!)
		* Extra sanity checks for displaying characters in Wish Details
		* Check for a valid weapon ID, even on character banners (and vice versa) in Wish Details
		* Fix getWeapon not returning a valid weapon if its ID is not 1xxxx
	* v1.1.1 (2023-11-06):
		* 4.2 Phase 1 Banners and two new swords
		* Add missing Solar Pearl 4-star catalyst, thus fixing an off-by-one error
		* Add most test/placeholder weapons (data from https://genshin.honeyhunterworld.com/?lang=EN)
		* Fix typo on Charlotte's name
	* v1.2 (202?-??-??):
		* Split into different files
		* Consolidate item/weapon funcs into one call
		* Add -r flag to display or drop the rate-up items only. (The opposite effect is not implemented because that's effectively just the standard banner)
		* Clarified the meaning of the -g flag.
*/

#define _GNU_SOURCE

#include <sys/random.h>
#include "gacha.h"
#include "util.h"

// CLI frontend exclusive includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "item.h"

// TODO: Determine if any/all of these are worth being made non-static.
static unsigned char pity[2];
static unsigned char pityS[4];
static unsigned char getRateUp[2];
static unsigned char fatePoints;
static unsigned short epitomizedPath;
static int doSmooth[2] = {1, 1};
static int doPity[2] = {1, 1};
static int do5050 = 1;

static const char* const banners[WISH_CNT][2] = {
	[CHAR1] = {"char1", "Character Event Wish"},
	[CHAR2] = {"char2", "Character Event Wish-2"},
	[WPN] = {"weapon", "Weapon Event Wish"},
	[STD_CHR] = {"std", "Wanderlust Invocation"},
	[STD_WPN] = {"std_weapon", "Wanderlust Incocation (Weapons)"},
	[NOVICE] = {"novice", "Beginners' Wish"}
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

#ifndef DEBUG
static unsigned int doAPull(unsigned int banner, unsigned int stdPoolIndex, unsigned int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#else
static unsigned int doAPull(unsigned int banner, int stdPoolIndex, int bannerIndex, unsigned int* rare, unsigned int* isRateUp) {
#endif
	unsigned long long rnd;
	long double rndF;
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
			if (!getRateUp[1] || do5050 > 0) {
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
			if (fatePoints < 2 && (!getRateUp[1] || do5050 > 0)) {
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
			if (!getRateUp[0] || do5050 > 0) {
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
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case WPN:
			if (!getRateUp[0] || do5050 > 0) {
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
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			pityS[1] = 0;
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarWpn[rnd % 18];
		case NOVICE:
			*isRateUp = 0;
			// Novice banner does not use the rate-up function
			getRateUp[0] = 0;
			// Novice banner does not use the stable function
			pityS[0] = 0;
			pityS[1] = 0;
			rndF = rndFloat();
			getrandom(&rnd, sizeof(long long), 0);
			return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
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
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4S(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
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
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
			}
			if (rndF <= getWeight4SW(pityS[0])) {
				pityS[0] = 0;
				getrandom(&rnd, sizeof(long long), 0);
				return FourStarChr[rnd % FourStarMaxIndex[stdPoolIndex]];
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

// NOTE: Everything from here down is not a candidate for being moved to a different file, since it is unique to the CLI frontend.

static int shouldBold(unsigned int rare, unsigned int banner, unsigned int rateUp) {
	if (rare <= 3) return 0;
	if (banner == STD_CHR) return 1;
	if (banner == STD_WPN) return 1;
	if (banner == NOVICE) return 1;
	return rateUp ? 1 : 0;
}

static void ver() {
		printf("Yet Another Genshin Impact Gacha Simulator v1.1\n"
		"\n©2023 Alex Pensinger (ArcticLuma113).\nThis program is released under the terms of the MPLv2, which can be viewed at:\nhttps://mozilla.org/MPL/2.0/.\n");
}

static void usage() {
	unsigned int i;
	ver();
	printf(
		"\nUsage:\n"
		"\t-b, --banner           Choose a banner type. Valid banners:\n"
		"\t                       \t"
	);
	for (i = 0; i < WISH_CNT; i++) {
		printf("%s%s", i != 0 ? ", " : "", banners[i][0]);
	}
	printf("\n"
		"\t                       \t(Required argument)\n"
		"\t-B, --banner_version   Choose a specific banner according to the version\n"
		"\t                       \tit appeared in. Format is as follows:\n"
		"\t                       \t<major>.<minor>.<phase>\n"
		"\t-d, --details          Shows the pool of avaliable items and then exits.\n"
		"\t-p, --pulls            Specify the number of pulls to perform at once.\n"
		"\t-4, --pity4            Specify initial 4★ pity.\n"
		"\t-5, --pity5            Specify initial 5★ pity.\n"
		"\t-l, --lostRateUp4      Specify that the next 4★ is guaranteed to be the\n"
		"\t                       \trate-up item. Only relevant on the Character and\n"
		"\t                       \tWeapon Event banners.\n"
		"\t-L, --lostRateUp5      Specify that the next 5★ is guaranteed to be the\n"
		"\t                       \trate-up item. Only relevant on the Character and\n"
		"\t                       \tWeapon Event banners.\n"
		"\t-f, --fate_points      Specify the accumulated Fate Points. Only\n"
		"\t                       \trelevant on the Weapon Event banner.\n"
		"\t-e, --epitomized_path  Specify the course to chart for the Epitomized\n"
		"\t                       \tPath. Only relevant on the Weapon Event banner.\n"
		"\t-c, --noviceCnt        Specify how many pulls have been made previously.\n"
		"\t                       \tOnly relevant on the Beginners' banner and is\n"
		"\t                       \tcapped at 8.\n"
		"\t-h, --help, --usage    Show this text and exit.\n"
		"\t-v, --version          Show application version and exit.\n"
		"\nAdvanced Usage:\n"
		"\t-n, --noPity4          Disable 4★ pity.\n"
		"\t-N, --noPity5          Disable 5★ pity.\n"
		"\t-g, --noGuarantee      Disable the rate-up guarantee mechanism. Only relevant on the Character and Weapon Event banners.\n"
		"\t                       \t(note that Epitomized Path will still work if it is set.)\n"
		"\t-r, --rateUpOnly       Display (with -d) or drop only rate-up characters. Only relevant on the Character and Weapon Event banners. Implies -g."
		"\t-V, --pool_version     Specify the version from which the standard pool\n"
		"\t                       \tof items will be drawn from."
		"\t                       \tNot relevant on the Beginners' banner, which uses a fixed pool.\n"
		"\nTuning the \"Stable Pity\" Mechanism:\n"
		"(the mechanic that prevents too many character or weapon drops in a row)\n"
		"\t--smooth4c             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ character.\n"
		"\t--smooth4w             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ weapon.\n"
		"\t--smooth5c             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ character. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t--smooth5w             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ weapon. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t-s, --noSmooth4        Disable the \"smooth\" pity mechanism for 4★\n"
		"\t                       \titems.\n"
		"\t-S, --noSmooth5        Disable the \"smooth\" pity mechanism for 5★\n"
		"\t                       \titems.\n"
		"\nDisclaimer:\n"
		"This project is not affiliated with miHoYo/Hoyoverse/Cogonosphere or any of\ntheir subsidiaries. It is designed for entertainment purposes only, and gacha\npulls made with this program can not and will not be reflected in your in-game\naccount.\n"
	);
}

typedef struct option opt_t;

static const opt_t long_opts[] = {
	{"banner", required_argument, 0, 'b'},
	{"pulls", required_argument, 0, 'p'},
	{"fate_points", required_argument, 0, 'f'},
	{"epitomized_path", required_argument, 0, 'e'},
	{"pity4", required_argument, 0, '4'},
	{"pity5", required_argument, 0, '5'},
	{"smooth4c", required_argument, 0, 1},
	{"smooth4w", required_argument, 0, 2},
	{"smooth5c", required_argument, 0, 3},
	{"smooth5w", required_argument, 0, 4},
	{"lostRateUp4", no_argument, 0, 'l'},
	{"lostRateUp5", no_argument, 0, 'L'},
	{"noSmooth5", no_argument, 0, 'S'},
	{"noSmooth4", no_argument, 0, 's'},
	{"noPity5", no_argument, 0, 'N'},
	{"noPity4", no_argument, 0, 'n'},
	{"noGuarantee", no_argument, 0, 'g'},
	{"noviceCnt", required_argument, 0, 'c'},
	{"pool_version", required_argument, 0, 'V'},
	{"details", no_argument, 0, 'd'},
	{"version", no_argument, 0, 'v'},
	{"help", no_argument, 0, 'h'},
	{"usage", no_argument, 0, 5},
	{"banner_version", required_argument, 0, 'B'},
	{"rateUpOnly", no_argument, 0, 'r'},
	{NULL, 0, 0, 0},
};

int main(int argc, char** argv) {
	unsigned int i;
	static char buf[1024];
	int item = 11301;
	unsigned int rare = 3;
	unsigned int color = 0;
	unsigned int isChar = 0;
	unsigned int won5050 = 0;
	int banner = -1;
	unsigned int pulls = 10;
	unsigned int noviceCnt = 0;
	unsigned int detailsRequested = 0;
	int epitomizedPathIndex = -1;
	int c = 0;
	long long n = 0;
	int v[4] = {-1, -1, 0, 0x42};
	int b[5] = {-1, -1, -1, 0, 0x421};
	char* p = NULL;
	while (1) {
		c = getopt_long(argc, argv, "4:5:B:LNSV:b:c:de:f:ghlnrsp:v", long_opts, NULL);
		if (c == -1) break;
		switch (c) {
		case '4':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "4★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 11) {
				fprintf(stderr, "4★ pity cannot be more than 11.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "4★ pity must be numeric.\n");
				return -1;
			}
			pity[0] = n;
			break;
		case '5':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "5★ pity cannot be negative.\n");
				return -1;
			}
			if (n > 90) {
				fprintf(stderr, "5★ pity cannot be more than 90.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "5★ pity must be numeric.\n");
				return -1;
			}
			pity[1] = n;
			break;
		case 'B':
			n = sscanf(optarg, "%i.%i.%i", &b[0], &b[1], &b[2]);
			if (n == EOF || n == 0) {
				fprintf(stderr, "Unable to parse banner version, using %d.%d.%d\n", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
			}
			b[3] = 1;
			if (n == 2) {
				if (b[0] == (b[4] >> 8) && b[1] == ((b[4] >> 4) & 0xf)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, "Did not get banner phase, using %d.%d.%d\n", b[0], b[1], b[2]);
			}
			else if (n == 1) {
				b[1] = (b[4] >> 4) & 0xf;
				if (b[0] == (b[4] >> 8)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, "Only got banner major version, using %d.0.%d\n", b[0], b[2]);
			}
#ifndef DEBUG
			if (b[2] > ((b[0] == 1 && b[1] == 3) ? 4 : 2) || b[2] < 1) {
				fprintf(stderr, "Invalid banner phase %d (only 1%s accepted)\n", b[2], (b[0] == 1 && b[1] == 3) ? "-4" : " or 2");
				return -1;
			}
#endif
			break;
		case 'L':
			getRateUp[1] = 1;
			break;
		case 'N':
			doPity[1] = 0;
			break;
		case 'S':
			doSmooth[1] = 0;
			break;
		case 'V':
			n = sscanf(optarg, "%i.%i", &v[0], &v[1]);
			if (n == EOF || n == 0) {
				fprintf(stderr, "Unable to parse pool version, using %d.%d\n", v[3] >> 4, v[3] & 0xf);
			}
			v[2] = 1;
			if (n == 1) {
				fprintf(stderr, "Only got major pool version, using %d.0\n", v[0]);
				v[1] = 0;
			}
			break;
		case 'b':
			for (n = 0; n < WISH_CNT; n++) {
				if (strcasecmp(optarg, banners[n][0]) == 0) {
					banner = n;
					break;
				}
			}
			if (n >= WISH_CNT) {
				fprintf(stderr, "Invalid banner type \"%s\". Valid banner types:\n", optarg);
				for (n = 0; n < WISH_CNT; n++) {
					fprintf(stderr, "\t%s: %s\n", banners[n][0], banners[n][1]);
				}
				return -1;
			}
			break;
		case 'c':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Wish count cannot be negative.\n");
				return -1;
			}
			if (n > 8) {
				n = 8;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Wish count must be numeric.\n");
				return -1;
			}
			noviceCnt = n;
			break;
		case 'd':
			detailsRequested = 1;
			break;
		case 'e':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 1 && n > 2) {
				fprintf(stderr, "Epitomized Path index is invalid.\n)");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Epitomized Path index must be numeric.\n");
				return -1;
			}
			epitomizedPathIndex = n;
			break;
		case 'f':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Fate Points cannot be negative.\n");
				return -1;
			}
			if (n > 2) {
				fprintf(stderr, "Fate Points cannot be more than 2.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Fate Points must be numeric.\n");
				return -1;
			}
			fatePoints = n;
			break;
		case 'g':
			do5050 = 0;
			break;
		case 'l':
			getRateUp[0] = 1;
			break;
		case 'n':
			doPity[0] = 0;
			break;
		case 'r':
			do5050 = -1;
			break;
		case 's':
			doSmooth[0] = 0;
			break;
		case 'p':
			n = strtoull(optarg, &p, 0);
			if (n < 0) {
				fprintf(stderr, "Pull count cannot be negative.\n");
				return -1;
			}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Pull count must be numeric.\n");
				return -1;
			}
			pulls = n;
			break;
		case 1 ... 4:
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, "Stable pity cannot be negative.\n");
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, "Stable pity must be numeric.\n");
				return -1;
			}
			// TODO minor sanity checks, similar to main pity
			pityS[c - 1] = n;
			break;
		case 'v':
			ver();
			return 0;
		case 'h':
		case 5:
			usage();
			return 0;
		case '?':
			fprintf(stderr, "Try '%s --help' for more information.\n", program_invocation_name);
			return -1;
		default:
			break;
		}
	}
	if (optind < argc) {
		fprintf(stderr, "%s: unrecognized option '%s'\n", program_invocation_name, argv[optind]);
		fprintf(stderr, "Try '%s --help' for more information.\n", program_invocation_name);
		return -1;
	}
	if (optind <= 1) {
		usage();
		return 0;
	}
	if (banner < 0) {
		fprintf(stderr, "We need a banner to pull from!\nValid banner indexes:\n");
		for (n = 0; n < WISH_CNT; n++) {
			fprintf(stderr, "\t%s: %s\n", banners[n][0], banners[n][1]);
		}
		fprintf(stderr, "Pick one with the -b option.\n");
		return -1;
	}
#ifndef DEBUG
	if (banner == WPN || banner == STD_WPN) {
		if (pity[0] >= 10) {
			fprintf(stderr, "4★ pity cannot be more than 10 for weapon banners.\n");
			return -1;
		}
		if (pity[1] >= 80) {
			fprintf(stderr, "5★ pity cannot be more than 80 for weapon banners.\n");
			return -1;
		}
	}
#endif
	if (b[3]) {
#ifndef DEBUG
		b[4] = (((b[0] & 0xf) << 8) | ((b[1] & 0xf) << 4) | (b[2] & 0xf));
#else
		b[4] = ((b[0] << 8) | (b[1] << 4) | b[2]);
#endif
	}
	b[0] = b[4] & 0xf;
	b[0]--;
	b[1] = b[4] >> 4;
#ifndef DEBUG
	if (b[1] > 0x42) b[1] = 0x42;
#endif
	if (b[1] >= 0x40) {
		b[1] -= 0x7;
	}
	if (b[1] >= 0x30) {
		b[1] -= 0x7;
	}
	if (b[1] >= 0x20) {
		b[1] -= 0x9;
	}
	if (b[1] >= 0x14) {
		b[1] += 0x1;
	}
	b[1] -= 0x10;
#ifndef DEBUG
	if (b[1] > 28) {
		b[1] = 28;
	}
#endif
	b[0] += (b[1] << 1);
	if (banner == WPN && epitomizedPathIndex > 0) {
		epitomizedPathIndex--;
#ifndef DEBUG
		epitomizedPathIndex &= 1;
#endif
		epitomizedPath = FiveStarWpnUp[b[0]][epitomizedPathIndex];
	}
	if (!(banner == STD_CHR || banner == STD_WPN) && do5050 < 0) {
		v[2] = 0;
		v[3] = 0;
	}
	if (v[2]) {
#ifndef DEBUG
		v[3] = ((v[0] & 0xf) << 4 | (v[1] & 0xf));
#else
		v[3] = (v[0] << 4 | v[1]);
#endif
	}
	else {
		v[3] = b[4] >> 4;
	}
	v[0] = v[3];
#ifndef DEBUG
	if (banner == NOVICE) {
#else
	if (0) {
#endif
		v[0] = 0;
	}
	else {
#ifndef DEBUG
		if (v[0] > 0x42) v[0] = 0x42;
#endif
		if (v[0] >= 0x40) {
			v[0] -= 0x7;
		}
		if (v[0] >= 0x30) {
			v[0] -= 0x7;
		}
		if (v[0] >= 0x20) {
			v[0] -= 0x9;
		}
		v[0] -= 0xf;
#ifndef DEBUG
		if ((int) v[0] < 1) v[0] = 1;
		if (v[0] > 29) v[0] = 29;
#endif
	}
#ifndef DEBUG
	if (FiveStarChrUp[b[0]][1] == 0xffff && banner == CHAR2) {
		fprintf(stderr, "Warning: Character Event Banner-2 didn't run during version %d.%d phase %d, switching to main Character Event Banner\n", (b[4] >> 8 & 0xf), (b[4] >> 4) & 0xf, b[4] & 0xf);
		banner = CHAR1;
	}
#endif
	if (detailsRequested) {
		printf("Details for the %s banner", banners[banner][1]);
		if ((banner == CHAR1 || banner == CHAR2 || banner == WPN) && b[3]) {
			printf(" from v%d.%d phase %d", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
		}
		printf(":");
		if (banner != NOVICE && v[2]) {
			printf(" (v%d.%d standard pool)", v[3] >> 4, v[3] & 0xf);
		}
		printf("\n\n");
		if (banner == CHAR1 || banner == CHAR2) {
			item = FiveStarChrUp[b[0]][banner - CHAR1];
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", getItem(item), item);
			}
			else {
				snprintf(buf, 1024, "id \e[33;1m%u\e[39;0m", item);
			}
			printf("Rate-Up 5★ Character:\n\t%s\n\n", buf);
			printf("Rate-Up 4★ Characters:\n");
			for (n = 0; n < 3; n++) {
				item = FourStarChrUp[b[0]][n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35;1m%u\e[39;0m", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		else if (banner == WPN) {
			printf("Rate-Up 5★ Weapons:\n");
			for (n = 0; n < 2; n++) {
				item = FiveStarWpnUp[b[0]][n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[33;1m%s\e[39;0m (id %u)", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33;1m%u\e[39;0m", item);
				}
				printf("\t%d: %s\n", (int) n + 1, buf);
			}
			printf("(Chart a course by passing -e x, where x is the desired index listed above.)\n\n");
			printf("Rate-Up 4★ Weapons:\n");
			for (n = 0; n < 5; n++) {
				item = FourStarWpnUp[b[0]][n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[35;1m%s\e[39;0m (id %u)", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35;1m%u\e[39;0m", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != WPN && banner != STD_WPN && do5050 >= 0) {
			printf("5★ Character Pool:\n");
			for (n = 0; n < FiveStarMaxIndex[v[0]]; n++) {
				item = FiveStarChr[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != CHAR1 && banner != CHAR2 && banner != NOVICE  && do5050 >= 0) {
			printf("5★ Weapon Pool:\n");
			for (n = 0; n < 10; n++) {
				item = FiveStarWpn[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[33%sm%s\e[39;0m (id %u)", shouldBold(5, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[33%sm%u\e[39;0m", shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (do5050 >= 0) {
			printf("4★ Character Pool:\n");
			for (n = 0; n < FourStarMaxIndex[v[0]]; n++) {
				item = FourStarChr[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != NOVICE && do5050 >= 0) {
			printf("4★ Weapon Pool:\n");
			for (n = 0; n < 18; n++) {
				item = FourStarWpn[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[35%sm%s\e[39;0m (id %u)", shouldBold(4, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[35%sm%u\e[39;0m", shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (do5050 >= 0) {
			printf("3★ Weapon Pool:\n");
			for (n = 0; n < 13; n++) {
				item = ThreeStar[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, "\e[34;22m%s\e[39;0m (id %u)", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, "id \e[34;22m%u\e[39;0m", item);
				}
				printf("\t%s\n", buf);
			}
		}
		return 0;
	}
	fprintf(stderr, "Making %u wishes on the %s banner", pulls, banners[banner][1]);
	if ((banner == CHAR1 || banner == CHAR2 || banner == WPN) && b[3]) {
		fprintf(stderr, " from v%d.%d phase %d", b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
	}
	if (banner != NOVICE && v[2]) {
		fprintf(stderr, " (v%d.%d standard pool)", v[3] >> 4, v[3] & 0xf);
	}
	fprintf(stderr, "\n\n");
	for (i = 0; i < pulls; i++) {
		if (banner == NOVICE && i == (7 - noviceCnt)) { // 8th wish is always Noelle on novice banner
			rare = 4;
			item = 1034;
			won5050 = 0;
			getRateUp[0] = 0;
			pity[0] = 0;
			pity[1]++;
			pityS[0] = 0;
			pityS[1] = 0;
		}
		// TODO Implement logic for character vs. item pool instead of checking the ID to determine that
		else item = doAPull(banner, v[0], b[0], &rare, &won5050);
		if (item < 0) {
			fprintf(stderr, "Pull #%u failed (retcode = %d)\n", i + 1, item);
			break;
		}
		switch (rare) {
		case 0:
		case 1:
		default:
			color = 39;
			break;
		case 2:
			color = 32;
			break;
		case 3:
			color = 34;
			break;
		case 4:
			color = 35;
			break;
		case 5:
			color = 33;
			break;
		}
		if (item <= 1000 + MAX_CHARS && item >= 1000) {
			isChar = 1;
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, "\e[%u%sm%s\e[39;0m (id %u)", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", getItem(item), item);
			}
			else {
				snprintf(buf, 1024, "with id \e[%u%sm%u\e[39;0m", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		else {
			isChar = 0;
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, "\e[%u%sm%s\e[39;0m (id %u)", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", getItem(item), item);
			}
			else {
				snprintf(buf, 1024, "with id \e[%u%sm%u\e[39;0m", color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		printf("Pull %u: %u★ %s %s\n", i + 1, rare, isChar ? "Character" : "Weapon", buf);
	}
	printf("\nResults after last pull:\n");
	if (doPity[0]) {
		printf("\n4★ pity: %u", pity[0]);
	}
	if (doPity[1]) {
		printf("\n5★ pity: %u\n", pity[1]);
	}
	else printf("\n");
	if (do5050 > 0 && (banner == CHAR1 || banner == CHAR2 || banner == WPN)) {
		printf("\n4★ guaranteed: %u\n", getRateUp[0] ? 1 : 0);
		printf("5★ guaranteed: %u\n", getRateUp[1] ? 1 : 0);
	}
	if (banner == WPN && epitomizedPath) {
		printf("Fate Points: %u\n", fatePoints);
	}
	if (do5050 >= 0 && doSmooth[0] && banner != NOVICE) {
		printf("\n4★ stable val (characters): %u\n", pityS[0]);
		printf("4★ stable val (weapons): %u", pityS[1]);
	}
	if (do5050 >= 0 && doSmooth[1] && (banner == STD_CHR || banner == STD_WPN)) {
		printf("\n5★ stable val (characters): %u\n", pityS[2]);
		printf("5★ stable val (weapons): %u\n", pityS[3]);
	}
	else printf("\n");
	return 0;
}
