/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (c) 2026 Christian Goeschel Ndjomouo <cgoesc2@wgu.edu>
 * Written by Christian Goeschel Ndjomouo <cgoesc2@wgu.edu>
 *
 * lslimits - list resource limits for a given PID
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "c.h"
#include "closestream.h"
#include "nls.h"
#include "pidfd-utils.h"
#include "strutils.h"
#include "optutils.h"
#include "pidutils.h"

static void __attribute__((__noreturn__)) usage(void)
{
	fputs(USAGE_HEADER, stdout);
	fprintf(stdout, _(" %s [options] PID[:inode]...\n"), program_invocation_short_name);

	fputs(USAGE_SEPARATOR, stdout);
	fputsln(_("Print the unique inode number of a process's file descriptor or namespace."), stdout);

	fputs(USAGE_OPTIONS, stdout);
	fputsln(_(" -p, --print-pid     enable PID:inode format printing"), stdout);
	fputsln(_("     --cgroupns      act on the cgroup namespace"), stdout);
	fputsln(_("     --ipcns         act on the ipc namespace"), stdout);
	fputsln(_("     --mntns         act on the mount namespace"), stdout);
	fputsln(_("     --netns         act on the net namespace"), stdout);
	fputsln(_("     --pidfs         act on the pidfs file descriptor (default)"), stdout);
	fputsln(_("     --pidns         act on the pid namespace"), stdout);
	fputsln(_("     --timens        act on the time namespace"), stdout);
	fputsln(_("     --userns        act on the user namespace"), stdout);
	fputsln(_("     --utsns         act on the uts namespace"), stdout);
	fputs(USAGE_SEPARATOR, stdout);
	fprintf(stdout, USAGE_HELP_OPTIONS(21)); /* char offset to align option descriptions */
	fprintf(stdout, USAGE_MAN_TAIL("getino(1)"));
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int c;

	enum {
		OPT_XYZZY = CHAR_MAX + 1,
		OPT_OPTIONAL	/* see howto-man-page.txt about short option */
	};
	static const struct option longopts[] = {
		{ "no-argument",                no_argument,       NULL, 'n'          },
		{ "optional",                   optional_argument, NULL, OPT_OPTIONAL },
		{ "required",                   required_argument, NULL, 'r'          },
		{ "extremely-long-long-option", no_argument,       NULL, 'e'          },
		{ "xyzzy",                      no_argument,       NULL, OPT_XYZZY    },
		{ "long-explanation",           no_argument,       NULL, 'l'          },
		{ "foobar",                     no_argument,       NULL, 'f'          },
		{ "version",                    no_argument,       NULL, 'V'          },
		{ "help",                       no_argument,       NULL, 'h'          },
		{ NULL, 0, NULL, 0 }
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	close_stdout_atexit();

	while ((c = getopt_long(argc, argv, "nr:zelfVh", longopts, NULL)) != -1)
		switch (c) {
		case 'n':
		case OPT_OPTIONAL:
		case 'r':
		case 'z':
		case OPT_XYZZY:
		case 'e':
		case 'l':
		case 'f':
			break;
		case 'V':
			print_version(EXIT_SUCCESS);
		case 'h':
			usage();
		default:
			errtryhelp(EXIT_FAILURE);
		}

	return EXIT_SUCCESS;
}
