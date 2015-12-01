/*
 * Copyright (C) 2012 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 *
 * Released according to the GNU GPL, version 2 or any later version.
 *
 * This work is part of the White Rabbit project, a research effort led
 * by CERN, the European Institute for Nuclear Research.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ipmi-fru.h>

#include "../kernel/fru-parse.c" /* Aaaargh!!!!! horrible hack... */

static char git_version[] = "git version: " GIT_VERSION;

void *fru_alloc(size_t size)
{
	return malloc(size);
}

static void print_version(char *pname)
{
	printf("%s %s\n", pname, git_version);
}

#define EEPROM_SIZE 8192

int main(int argc, char **argv)
{
	struct fru_board_info_area *bia;
	struct fru_common_header *h;
	struct stat stbuf;
	void *eeprom;
	char *fname;
	FILE *f = NULL;
	int i, err = 0;

	if ((argc == 2) && (!strcmp(argv[1], "-V"))) {
		print_version(argv[0]);
		exit(0);
	}

	if (argc < 2) {
		fprintf(stderr, "%s: Use \"%s [-V] <fru-image> [...]\"\n",
			argv[0], argv[0]);
		exit(1);
	}

	eeprom = malloc(EEPROM_SIZE);
	if (!eeprom) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		exit(1);
	}
	h = eeprom;

	for (i = 1; i < argc; i++) {
		fname = argv[i];
		if (f) /* second or later loop */
			fclose(f);
		f = fopen(fname, "r");
		memset(eeprom, 0, EEPROM_SIZE);
		if (!f) {
			fprintf(stderr, "%s: %s: %s\n", argv[0], fname,
				strerror(errno));
			err++;
			continue;
		}
		if (fstat(fileno(f), &stbuf) < 0) { /* never, I hope */
			fprintf(stderr, "%s: %s: %s\n", argv[0], fname,
				strerror(errno));
			err++;
			continue;
		}
		if (stbuf.st_size > EEPROM_SIZE)
			stbuf.st_size = EEPROM_SIZE;
		if (fread(eeprom, 1, stbuf.st_size, f) != stbuf.st_size) {
			fprintf(stderr, "%s: %s: read error\n", argv[0],
				fname);
			err++;
			continue;
		}

		/* some boring check */
		if (h->format != 1) {
			fprintf(stderr, "%s: not a FRU file\n", fname);
			continue;
		}
		if (!fru_header_cksum_ok(h)) {
			fprintf(stderr, "%s: wrong header checksum\n", fname);
		}
		bia = fru_get_board_area(h);
		if (!fru_bia_cksum_ok(bia)) {
			fprintf(stderr, "%s: wrong board area checksum\n",
				fname);
		}

		/* FIXME: dump the stupid date -- which expires in 2027... */

		/* The following may have nulls and segfault: who cares... */
		printf("%s: manufacturer: %s\n", fname,
			fru_get_board_manufacturer(h));
		printf("%s: product-name: %s\n", fname,
			fru_get_product_name(h));
		printf("%s: serial-number: %s\n", fname,
			fru_get_serial_number(h));
		printf("%s: part-number: %s\n", fname,
			fru_get_part_number(h));
	}
	return err != 0;
}
