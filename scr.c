#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

extern char *__progname;

void
usage() {
	fprintf(stderr, "usage: %s [-hrnls] [-b base] [infile [outfile]]\n", __progname);
	exit(1);
}

void
dashes (FILE *ofp, int sn)
{
	int i, n;
	n = 64 - 3 - 1 - 6 - 1; /* Three dashes, one space, SCREEN, one space. */
	if (n < 10)
		n--;
	else if (n < 100)
		n -= 2;
	else if (n < 1000)
		n -= 3;
	else /* Infinite number! */
		n -= 4;
	for(i = 0; i < n; i++)
		fputc('-', ofp);
	fputc('\n', ofp);
}

int
main(int argc, char *argv[]) {
	int ropt;
	ropt = 0;
	int nopt;
	nopt = 0;
	int lopt;
	lopt = 0;
	int sopt;
	sopt = 0;
	int bopt;
	bopt = 0;
	int boptarg;
	boptarg = 0;
	char *ifn;
	ifn = NULL;
	FILE *ifp;
	ifp = stdin;
	char *ofn;
	ofn = NULL;
	FILE *ofp;
	ofp = stdout;

	char ch;
	while ((ch = getopt(argc, argv, "hrnlsb:")) != -1) {
		switch (ch) {
			case 'r':
				if (ropt != 0)
					usage();
				ropt = 1;
				break;
			case 'n':
				if (nopt != 0)
					usage();
				nopt = 1;
				break;
			case 'l':
				if (lopt != 0)
					usage();
				lopt = 3;
				break;
			case 's':
				if (sopt != 0)
					usage();
				sopt = 1;
				break;
			case 'b':
				if (bopt != 0)
					usage();
				bopt = 1;
				boptarg = atoi(optarg);
				break;
			case 'h':
			default:
				usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 2) {
		usage();
	}

	if (argc >= 1) {
		if (strcmp(argv[0], "-") != 0) {
			ifn = argv[0];
			ifp = fopen(ifn, "r");
			if (ifp == NULL)
				err(1, "%s", ifn);
		}
	}
	if (argc >= 2) {
		if (strcmp(argv[1], "-") != 0) {
			ofn = argv[1];
			ofp = fopen(ofn, "w");
			if (ofp == NULL)
				err(1, "%s", ofn);
		}
	}

#ifdef __OpenBSD__
	if (pledge("stdio", NULL) == -1)
		err(1, "pledge");
#endif

	const char ln[16][3] = {
		" 0 ",
		" 1 ",
		" 2 ",
		" 3 ",
		" 4 ",
		" 5 ",
		" 6 ",
		" 7 ",
		" 8 ",
		" 9 ",
		"10 ",
		"11 ",
		"12 ",
		"13 ",
		"14 ",
		"15 ",
	};

	int ls;
	ls = 0 + lopt;
	int le;
	le = 64 + lopt;

	int sn;
	sn = boptarg;

	if (!ropt) {
		int cc;
		cc = 0;
		int q;
		q = 0;
		while (!q) {
			int i, j;
			char s[16][68];
			int d;
			d = 0;
			for (i = 0; i < 16; i++) {
				for (j = 0; j < ls; j++)
					s[i][j] = ln[i][j];
				int k;
				k = ls;
				for (j = ls; j < le; j++) {
					int c;
					c = fgetc(ifp);
					if (c == EOF && feof(ifp)) {
						q = 1;
						break;
					}
					cc++;
					if (nopt && (char)c == '\n')
						c = ' ';
					if ((char)c != ' ')
						k = j + 1;
					s[i][j] = (char)c;
					d = 1;
					if ((char)c < 32 || (char)c > 126) {
						if (ifn != NULL)
							fprintf(stderr, "%s:", ifn);
						fprintf(stderr, "#%d: invalid character\n", cc);
						exit(2);
					}
				}
				s[i][k] = '\0';
			}
			if (q && d) {
				if (ifn != NULL)
					fprintf(stderr, "%s:", ifn);
				fprintf(stderr, "#%d: screen truncated\n", cc);
				exit(2);
			}
			if (!q || d) {
				if (sopt) {
					fprintf(ofp, "   --- SCREEN %d ", sn);
					dashes (ofp, sn);
					sn++;
				}
				for (i = 0; i < 16; i++) {
					fputs(s[i], ofp);
					fputc('\n', ofp);
				}
			}
		}
	} else {
		int lc;
		lc = 1;
		int q;
		q = 0;
		while (!q) {
			int i, j;
			char s[17][67];
			int d;
			d = 0;
			for (i = 0; i < 16 + sopt; i++) {
				for (j = 0; j < le; j++)
					s[i][j] = ' ';
				for (j = 0; j <= le; j++) {
					int c;
					c = fgetc(ifp);
					if (c == EOF && feof(ifp)) {
						q = 1;
						break;
					}
					if (!nopt && (char)c == '\n')
						break;
					if (j == le) {
						if (ifn != NULL)
							fprintf(stderr, "%s:", ifn);
						fprintf(stderr, "%d: line is too long\n", lc);
						exit(2);
					}
					s[i][j] = (char)c;
					d = 1;
					if ((char)c == '\n')
						break;
					if ((char)c < 32 || (char)c > 126) {
						if (ifn != NULL)
							fprintf(stderr, "%s:", ifn);
						fprintf(stderr, "%d: invalid character\n", lc);
						exit(2);
					}
				}
				lc++;
			}
			if (!q || d)
				for (i = sopt; i < 16 + sopt; i++)
					for (j = ls; j < le; j++)
						fputc(s[i][j], ofp);
		}
	}
}
