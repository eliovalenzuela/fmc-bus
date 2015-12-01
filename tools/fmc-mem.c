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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static char git_version[] = "git version: " GIT_VERSION;

static void help(char *prgname)
{
	fprintf(stderr, "%s: use "
		"\"%s -V <device> <addr> [<value>] [+<nbytes>]\"\n"
		"     <device> is a file name, all others are hex numbers\n"
		"     bursts of \"nbytes\" use stdin/stdout (value ignored)\n"
		"     -V version\n",
		prgname, prgname);
	exit(1);
}


static void w_loop(int fd, char **argv, int nbytes)
{
	uint32_t reg;

	while (nbytes >= sizeof(reg)) {
		if (read(0, &reg, sizeof(reg)) != sizeof(reg)) {
			fprintf(stderr, "%s: <stdin>: short read\n", argv[0]);
			exit(1);
		}
		if (write(fd, &reg, sizeof(reg)) != sizeof(reg)) {
			fprintf(stderr, "%s: write(%s): %s\n", argv[0],
				argv[1], strerror(errno));
			exit(1);
		}
		nbytes -= sizeof(reg);
	}
	exit(0);
}

static void r_loop(int fd, char **argv, int nbytes)
{
	uint32_t reg;

	while (nbytes >= sizeof(reg)) {
		if (read(fd, &reg, sizeof(reg)) != sizeof(reg)) {
			fprintf(stderr, "%s: read(%s): %s\n", argv[0],
				argv[1], strerror(errno));
			exit(1);
		}
		if (write(1, &reg, sizeof(reg)) != sizeof(reg)) {
			fprintf(stderr, "%s: <stdout>: %s\n", argv[0],
				strerror(errno));
			exit(1);
		}
		nbytes -= sizeof(reg);
	}
	exit(0);
}

static void print_version(char *pname)
{
	printf("%s %s\n", pname, git_version);
}

int main(int argc, char **argv)
{
	int fd;
	unsigned long addr, value, nbytes = 0;
	uint32_t reg;
	int dowrite = 0, doloop = 0;
	char c;

	if ((argc == 2) && (!strcmp(argv[1], "-V"))) {
		print_version(argv[0]);
		exit(0);
	}

	if (argc < 3 || argv[1][0] == '-') /* -h, --help, whatever */
		help(argv[0]);

	/* boring scanning of arguments */
	if (sscanf(argv[2], "%lx%c", &addr, &c) != 1) {
		fprintf(stderr, "%s: Not an hex address \"%s\"\n",
			argv[0], argv[2]);
		exit(1);
	}
	if (argv[argc - 1][0] == '+') {
		if (sscanf(argv[argc - 1], "+%lx%c", &nbytes, &c) != 1) {
			fprintf(stderr, "%s: Not +<hex-number> \"%s\"\n",
				argv[0], argv[argc - 1]);
			exit(1);
		}
		doloop = 1;
		argc--;
	}
	if (argc > 4)
		help(argv[0]);
	if (argc == 4) {
		if (sscanf(argv[3], "%lx%c", &value, &c) != 1) {
			fprintf(stderr, "%s: Not an hex value \"%s\"\n",
				argv[0], argv[3]);
			exit(1);
		}
		dowrite = 1;
	}

	if (0) { /* want to verify? */
		if (dowrite)
			printf("write %lx to %s:%lx (loop %i %lx)\n",
			       value, argv[1], addr, doloop, nbytes);
		else
			printf("read from %s:%lx (loop %i %lx)\n",
			       argv[1], addr, doloop, nbytes);
		exit(0);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1],
			strerror(errno));
		exit(1);
	}

	if (lseek(fd, addr, SEEK_SET) < 0) {
		fprintf(stderr, "%s: %s: lseek: %s\n", argv[0], argv[1],
			strerror(errno));
		exit(1);
	}

	/* each loop function here exits when it's done */
	if (doloop && dowrite)
		w_loop(fd, argv, nbytes);
	if (doloop)
		r_loop(fd, argv, nbytes);

	/* one write only */
	if (dowrite) {
		reg = value;
		if (write(fd, &reg, sizeof(reg)) != sizeof(reg)) {
			fprintf(stderr, "%s: write(): %s\n", argv[0],
				strerror(errno));
			exit(1);
		}
		exit(0);
	}

	/* one read only */
	if (read(fd, &reg, sizeof(reg)) != sizeof(reg)) {
		fprintf(stderr, "%s: read(): %s\n", argv[0],
			strerror(errno));
		exit(1);
	}
	printf("%08x\n", reg);
	exit(0);
}
