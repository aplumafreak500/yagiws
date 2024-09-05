/* SPDX-License-Identifier: MPL-2.0 */
/* This file is part of Yet Another Star Rail Warp Simulator */
/* ©2024 Alex Pensinger (ArcticLuma113) */
/* Released under the terms of the MPLv2, which can be viewed at https://mozilla.org/MPL/2.0/ */

#include "config.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif
#include "gacha.h"
#include "item.h"
#include "util.h"

static int shouldBold(unsigned int rare, unsigned int banner, unsigned int rateUp) {
	if (rare <= 3) return 0;
	switch (banner) {
	case STD_CHR:
	case STD_WPN:
	case STD_ONLY_CHR:
	case NOVICE:
		return 1;
	default:
		break;
	}
	return rateUp ? 1 : 0;
}

static void ver() {
	printf(_(
		"Yet Another Star Rail Warp Simulator v%s\n"
		"\n©2024 Alex Pensinger (ArcticLuma113)"
		"\nThis program is released under the terms of the MPLv2, which can be viewed at:"
		"\nhttps://mozilla.org/MPL/2.0/\n"
	), PACKAGE_VERSION);
}

static void usage() {
	unsigned int i;
	ver();
	printf(_(
		"\nUsage:\n"
		"\t-b, --banner           Choose a banner type. Valid banners:\n"
		"\t                       \t"
	));
	const char* sep;
	for (i = 0; i < WARP_CNT; i++) {
		if (i == 0) sep = "";
		else if (i == STD_ONLY_CHR) sep = ",\t\n\t                       \t";
		else sep = ", ";
		printf("%s%s", sep, gettext(banners[i][0]));
	}
	printf(_("\n"
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
		"\t                       \tLight Cone Event banners.\n"
		"\t-L, --lostRateUp5      Specify that the next 5★ is guaranteed to be the\n"
		"\t                       \trate-up item. Only relevant on the Character and\n"
		"\t                       \tLight Cone Event banners.\n"
		"\t-c, --noviceCnt        Specify how many pulls have been made previously.\n"
		"\t                       \tOnly relevant on the Departure banner and is\n"
		"\t                       \tcapped at 50.\n"
		"\t                       \tAlso used on the Stellar Warp; when this is zero, the very first pull is forced to be Asta.\n"
		"\t-h, --help, --usage    Show this text and exit.\n"
		"\t-v, --version          Show application version and exit.\n"
		"\nAdvanced Usage:\n"
		"\t-n, --noPity4          Disable 4★ pity.\n"
		"\t-N, --noPity5          Disable 5★ pity.\n"
		"\t-g, --noGuarantee      Disable the rate-up guarantee mechanism.\n"
		"\t                       \tOnly relevant on the Character and Light Cone\n"
		"\t                       \tEvent banners.\n"
		"\t-r, --rateUpOnly       Display (with -d) or drop only rate-up items.\n"
		"\t                       \tOnly relevant on the Character and Light Cone\n"
		"\t                       \tEvent banners.\n"
		"\t                       \tImplies -g.\n"
		"\t-V, --pool_version     Specify the version from which the standard pool\n"
		"\t                       \tof items will be drawn from.\n"
		"\t                       \tNot relevant on the Departure banner, which uses\n"
		"\t                       \ta fixed pool.\n"
		"\nTuning the \"Stable Pity\" Mechanism:\n"
		"(the mechanic that prevents too many character or Light Cone drops in a row)\n"
		"\t--smooth4c             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ character.\n"
		"\t--smooth4w             Specify the number of pulls since receiving a\n"
		"\t                       \t4★ Light Cone.\n"
		"\t--smooth5c             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ character. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t--smooth5w             Specify the number of pulls since receiving a\n"
		"\t                       \t5★ Light Cone. Only relevant on the standard\n"
		"\t                       \tbanner.\n"
		"\t-s, --noSmooth4        Disable the \"smooth\" pity mechanism for 4★\n"
		"\t                       \titems.\n"
		"\t-S, --noSmooth5        Disable the \"smooth\" pity mechanism for 5★\n"
		"\t                       \titems.\n"
		"\t-C, --forceSmoothChar  Forces the \"smooth\" pity mechanism into dropping\n"
		"\t                       \tonly characters.\n"
		"\t-W, --forceSmoothWpn   Forces the \"smooth\" pity mechanism into dropping\n"
		"\t                       \tonly Light Cones.\n"
		"\t                       \t(Note: If both -C and -W are given, neither\n"
		"\t                       \toption will take effect.)\n"
		"\t--noSmoothOld          With -s and/or -S, specify that the >v1.1\n"
		"\t                       \tbehavior should be used that forces \"smooth\"\n"
		"\t                       \tpity to be 50/50 between characters and Light\n"
		"\t                       \tCones.\n"
		"\nDisclaimer:\n"
		"This project is not affiliated with miHoYo/Hoyoverse/Cogonosphere or any of\n"
		"their subsidiaries. It is designed for entertainment purposes only, and gacha\n"
		"pulls made with this program can not and will not be reflected in your in-game\n"
		"account.\n"
	));
}

typedef struct option opt_t;

static const opt_t long_opts[] = {
	{"banner", required_argument, 0, 'b'},
	{"pulls", required_argument, 0, 'p'},
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
	{"forceSmoothChar", no_argument, 0, 'C'},
	{"forceSmoothWpn", no_argument, 0, 'W'},
	{"noSmoothOld", no_argument, 0, 6},
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
	unsigned int oldSmooth = 0;
	int c = 0;
	long long n = 0;
	int v[4] = {-1, -1, 0, 0x24};
	int b[5] = {-1, -1, -1, 0, 0x242};
	unsigned int forceSmooth = 0;
	char* p = NULL;
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif
	while (1) {
		c = getopt_long(argc, argv, "4:5:B:CLNSV:Wb:c:dghlnrsp:v", long_opts, NULL);
		if (c == -1) break;
		switch (c) {
		case '4':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, _("4★ pity cannot be negative.\n"));
				return -1;
			}
			if (n > 11) {
				fprintf(stderr, _("4★ pity cannot be more than 11.\n"));
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, _("4★ pity must be numeric.\n"));
				return -1;
			}
			pity[0] = n;
			break;
		case '5':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, _("5★ pity cannot be negative.\n"));
				return -1;
			}
			if (n > 90) {
				fprintf(stderr, _("5★ pity cannot be more than 90.\n"));
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, _("5★ pity must be numeric.\n"));
				return -1;
			}
			pity[1] = n;
			break;
		case 'B':
			n = sscanf(optarg, "%i.%i.%i", &b[0], &b[1], &b[2]);
			if (n == EOF || n == 0) {
				fprintf(stderr, _("Unable to parse banner version, using %d.%d.%d\n"), b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
			}
			b[3] = 1;
			if (n == 2) {
				if (b[0] == (b[4] >> 8) && b[1] == ((b[4] >> 4) & 0xf)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, _("Did not get banner phase, using %d.%d.%d\n"), b[0], b[1], b[2]);
			}
			else if (n == 1) {
				b[1] = (b[4] >> 4) & 0xf;
				if (b[0] == (b[4] >> 8)) {
					b[2] = b[4] & 0xf;
				}
				else b[2] = 1;
				fprintf(stderr, _("Only got banner major version, using %d.0.%d\n"), b[0], b[2]);
			}
#ifndef DEBUG
			if ((b[2] > 2) || (b[2] < 1)) {
				fprintf(stderr, _("Invalid banner phase %d (only 1 or 2 accepted)\n"), b[2]);
				return -1;
			}
#endif
			break;
		case 'C':
			forceSmooth |= 1;
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
				fprintf(stderr, _("Unable to parse pool version, using %d.%d\n"), v[3] >> 4, v[3] & 0xf);
			}
			v[2] = 1;
			if (n == 1) {
				fprintf(stderr, _("Only got major pool version, using %d.0\n"), v[0]);
				v[1] = 0;
			}
			break;
		case 'W':
			forceSmooth |= 2;
			break;
		case 'b':
			for (n = 0; n < WARP_CNT; n++) {
				if (strcasecmp(optarg, banners[n][0]) == 0) {
					banner = n;
					break;
				}
			}
			if (n >= WARP_CNT) {
				fprintf(stderr, _("Invalid banner type \"%s\". Valid banner types:\n"), optarg);
				for (n = 0; n < WARP_CNT; n++) {
					fprintf(stderr, _("\t%s: %s\n"), banners[n][0], gettext(banners[n][1]));
				}
				return -1;
			}
			break;
		case 'c':
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, _("Warp count cannot be negative.\n"));
				return -1;
			}
			if (n > 50) {
				n = 50;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, _("Warp count must be numeric.\n"));
				return -1;
			}
			noviceCnt = n;
			break;
		case 'd':
			detailsRequested = 1;
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
				fprintf(stderr, _("Pull count cannot be negative.\n"));
				return -1;
		}
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, _("Pull count must be numeric.\n"));
				return -1;
			}
			pulls = n;
			break;
		case 1 ... 4:
			n = strtoull(optarg, &p, 0);
#ifndef DEBUG
			if (n < 0) {
				fprintf(stderr, _("Stable pity cannot be negative.\n"));
				return -1;
			}
#endif
			if ((unsigned long) optarg == (unsigned long) p) {
				fprintf(stderr, _("Stable pity must be numeric.\n"));
				return -1;
			}
			// TODO minor sanity checks, similar to main pity
			pityS[c - 1] = n;
			break;
		case 6:
			oldSmooth = 1;
			break;
		case 'v':
			ver();
			return 0;
		case 'h':
		case 5:
			usage();
			return 0;
		case '?':
			fprintf(stderr, _("Try '%s --help' for more information.\n"), program_invocation_name);
			return -1;
		default:
			break;
		}
	}
	if (optind < argc) {
		fprintf(stderr, _("%s: unrecognized option '%s'\n"), program_invocation_name, argv[optind]);
		fprintf(stderr, _("Try '%s --help' for more information.\n"), program_invocation_name);
		return -1;
	}
	if (optind <= 1) {
		usage();
		return 0;
	}
	if (banner < 0) {
		fprintf(stderr, _("We need a banner to pull from!\nValid banner indexes:\n"));
		for (n = 0; n < WARP_CNT; n++) {
			fprintf(stderr, _("\t%s: %s\n"), banners[n][0], gettext(banners[n][1]));
		}
		fprintf(stderr, _("Pick one with the -b option.\n"));
		return -1;
	}
#ifndef DEBUG
	if (banner == WPN1 || banner == WPN2 || banner == STD_WPN) {
		if (pity[0] >= 10) {
			fprintf(stderr, _("4★ pity cannot be more than 10 for Light Cone banners.\n"));
			return -1;
		}
		if (pity[1] >= 80) {
			fprintf(stderr, _("5★ pity cannot be more than 80 for Light Cone banners.\n"));
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
	if (b[1] > 0x26) b[1] = 0x26;
#endif

	if (b[1] >= 0x20) {
		b[1] -= 0x9;
	}
	b[1] -= 0x10;
#ifndef DEBUG
	if (b[1] > IDX_MAX - 1) {
		b[1] = IDX_MAX - 1;
	}
#endif
	b[0] += (b[1] << 1);
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
		if (v[0] > 0x26) v[0] = 0x26;
#endif
		if (v[0] >= 0x20) {
			v[0] -= 0x9;
		}
		v[0] -= 0x10;
#ifndef DEBUG
		if (v[0] > IDX_MAX) v[0] = IDX_MAX;
#endif
	}
#ifndef DEBUG
	if (FiveStarChrUp[b[0]][1] == 0xffff && banner == CHAR2) {
		fprintf(stderr, _("Warning: Character Event Warp-2 banner didn't run during version %d.%d phase %d, switching to main Character Event Warp banner\n"), (b[4] >> 8 & 0xf), (b[4] >> 4) & 0xf, b[4] & 0xf);
		banner = CHAR1;
	}
	if (FiveStarWpnUp[b[0]][1] == 0xffff && banner == WPN2) {
		fprintf(stderr, _("Warning: Light Cone Event Warp-2 banner didn't run during version %d.%d phase %d, switching to main Light Cone Event Warp banner\n"), (b[4] >> 8 & 0xf), (b[4] >> 4) & 0xf, b[4] & 0xf);
		banner = WPN1;
	}
#endif
	if ((forceSmooth & 3) == 3) {
		fprintf(stderr, _("Both characters and Light Cones specified as forced. Reverting to normal behavior.\n"));
		forceSmooth = 0;
	}
	if (oldSmooth) {
		if (doSmooth[0] == 0) {
			doSmooth[0] = -1;
		}
		if (doSmooth[1] == 0) {
			doSmooth[1] = -1;
		}
	}
	if (detailsRequested) {
		if ((banner == CHAR1 || banner == CHAR2 || banner == WPN1 || banner == WPN2) && b[3]) {
			printf(_("Details for the %s banner from v%d.%d phase %d:"), gettext(banners[banner][1]), b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
		}
		else {
			printf(_("Details for the %s banner:"), gettext(banners[banner][1]));
		}
		if ((banner != NOVICE) && v[2]) {
			printf(_(" (v%d.%d standard pool)"), v[3] >> 4, v[3] & 0xf);
		}
		printf("\n\n");
		if (banner == CHAR1 || banner == CHAR2) {
			item = FiveStarChrUp[b[0]][banner - CHAR1];
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, _("\e[33;1m%s\e[39;0m (id %u)"), getItem(item), item);
			}
			else {
				snprintf(buf, 1024, _("id \e[33;1m%u\e[39;0m"), item);
			}
			printf(_("Rate-Up 5★ Character:\n\t%s\n\n"), buf);
			printf(_("Rate-Up 4★ Characters:\n"));
			for (n = 0; n < 3; n++) {
				item = FourStarChrUp[b[0]][n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[35;1m%s\e[39;0m (id %u)"), getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[35;1m%u\e[39;0m"), item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		else if (banner == WPN1 || banner == WPN2) {
			item = FiveStarWpnUp[b[0]][banner - WPN1];
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, _("\e[33;1m%s\e[39;0m (id %u)"), getItem(item), item);
			}
			else {
				snprintf(buf, 1024, _("id \e[33;1m%u\e[39;0m"), item);
			}
			printf(_("Rate-Up 5★ Light Cone:\n\t%s\n\n"), buf);
			printf(_("Rate-Up 4★ Light Cones:\n"));
			for (n = 0; n < 3; n++) {
				item = FourStarWpnUp[b[0]][n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[35;1m%s\e[39;0m (id %u)"), getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[35;1m%u\e[39;0m"), item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != WPN1 && banner != WPN2 && banner != STD_WPN && do5050 >= 0) {
			printf(_("5★ Character Pool:\n"));
			for (n = 0; n < 7; n++) {
				item = FiveStarChr[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[33%sm%s\e[39;0m (id %u)"), shouldBold(5, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[33%sm%u\e[39;0m"), shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != CHAR1 && banner != CHAR2 && banner != NOVICE && banner != STD_ONLY_CHR && do5050 >= 0) {
			printf(_("5★ Light Cone Pool:\n"));
			for (n = 0; n < 7; n++) {
				item = FiveStarWpn[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[33%sm%s\e[39;0m (id %u)"), shouldBold(5, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[33%sm%u\e[39;0m"), shouldBold(5, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (do5050 >= 0) {
			printf(_("4★ Character Pool:\n"));
			for (n = 0; n < FourStarChrMaxIndex[v[0]]; n++) {
				item = FourStarChr[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[35%sm%s\e[39;0m (id %u)"), shouldBold(4, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[35%sm%u\e[39;0m"), shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (banner != NOVICE && do5050 >= 0) {
			printf(_("4★ Light Cone Pool:\n"));
			for (n = 0; n < FourStarWpnMaxIndex[v[0]]; n++) {
				item = FourStarWpn[n];
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[35%sm%s\e[39;0m (id %u)"), shouldBold(4, banner, 0) ? ";1" : ";22", getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[35%sm%u\e[39;0m"), shouldBold(4, banner, 0) ? ";1" : ";22", item);
				}
				printf("\t%s\n", buf);
			}
			printf("\n");
		}
		if (do5050 >= 0) {
			printf(_("3★ Light Cone Pool:\n"));
			for (n = 0; n < 21; n++) {
				item = 20000 + n;
				if (getItem(item) != NULL) {
					snprintf(buf, 1024, _("\e[34;22m%s\e[39;0m (id %u)"), getItem(item), item);
				}
				else {
					snprintf(buf, 1024, _("id \e[34;22m%u\e[39;0m"), item);
				}
				printf("\t%s\n", buf);
			}
		}
		return 0;
	}
	if ((banner == CHAR1 || banner == CHAR2 || banner == WPN1 || banner == WPN2) && b[3]) {
		fprintf(stderr, _("Performing %u warps - %s banner from v%d.%d phase %d"), pulls, gettext(banners[banner][1]), b[4] >> 8, (b[4] >> 4) & 0xf, b[4] & 0xf);
	}
	else {
		fprintf(stderr, _("Performing %u warps - %s banner"), pulls, gettext(banners[banner][1]));
	}
	if ((banner != NOVICE) && v[2]) {
		fprintf(stderr, _(" (v%d.%d standard pool)"), v[3] >> 4, v[3] & 0xf);
	}
	fprintf(stderr, "\n\n");
	for (i = 0; i < pulls; i++) {
		if (banner == NOVICE && i == (49 - noviceCnt)) {
			// If 5-star pity is 50 or more...
			if (pity[1] + 1 >= 50) {
				// Then max it out before making a pull. This (should) force a 5-star character no matter what.
				pity[1] = ~1;
			}
		}
		else if ((banner == STD_CHR || banner == STD_ONLY_CHR) && i == noviceCnt && noviceCnt == 0) {
			// Very first wish on standard banner is always Asta
			rare = 4;
			item = 1009;
			won5050 = 0;
			getRateUp[0] = 0;
			pity[0] = 0;
			pity[1]++;
			pityS[0] = 0;
			pityS[1] = 0;
		}
		// TODO Implement logic for character vs. item pool instead of checking the ID to determine that
		if (forceSmooth & 1) {
			pityS[0] = ~1;
			pityS[2] = ~1;
			pityS[1] = -1;
			pityS[3] = -1;
		}
		if (forceSmooth & 2) {
			pityS[1] = ~1;
			pityS[3] = ~1;
			pityS[0] = -1;
			pityS[2] = -1;
		}
		item = doAPull(banner, v[0], b[0], &rare, &won5050);
		if (item < 0) {
			fprintf(stderr, _("Pull #%u failed (retcode = %d)\n"), i + 1, item);
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
		if (item <= 1324 && item >= 1000) {
			isChar = 1;
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, _("\e[%u%sm%s\e[39;0m (id %u)"), color, shouldBold(rare, banner, won5050) ? ";1" : ";22", getItem(item), item);
			}
			else {
				snprintf(buf, 1024, _("with id \e[%u%sm%u\e[39;0m"), color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		else {
			isChar = 0;
			if (getItem(item) != NULL) {
				snprintf(buf, 1024, _("\e[%u%sm%s\e[39;0m (id %u)"), color, shouldBold(rare, banner, won5050) ? ";1" : ";22", getItem(item), item);
			}
			else {
				snprintf(buf, 1024, _("with id \e[%u%sm%u\e[39;0m"), color, shouldBold(rare, banner, won5050) ? ";1" : ";22", item);
			}
		}
		printf(_("Pull %u: %u★ %s %s\n"), i + 1, rare, isChar ? _("Character") : _("Light Cone"), buf);
	}
	printf(_("\nResults after last pull:\n"));
	if (doPity[0]) {
		printf(_("\n4★ pity: %u"), pity[0]);
	}
	if (doPity[1]) {
		printf(_("\n5★ pity: %u\n"), pity[1]);
	}
	else printf("\n");
	if (do5050 > 0 && (banner == CHAR1 || banner == CHAR2 || banner == WPN1 || banner == WPN2)) {
		printf(_("\n4★ guaranteed: %u\n"), getRateUp[0] ? 1 : 0);
		printf(_("5★ guaranteed: %u\n"), getRateUp[1] ? 1 : 0);
	}
	if (do5050 >= 0 && doSmooth[0] && banner != NOVICE) {
		printf(_("\n4★ stable val (characters): %u\n"), pityS[0]);
		printf(_("4★ stable val (Light Cones): %u"), pityS[1]);
	}
	if (do5050 >= 0 && doSmooth[1] && (banner == STD_CHR || banner == STD_WPN || banner == STD_ONLY_CHR)) {
		printf(_("\n5★ stable val (characters): %u\n"), pityS[2]);
		printf(_("5★ stable val (Light Cones): %u\n"), pityS[3]);
	}
	else printf("\n");
	return 0;
}
