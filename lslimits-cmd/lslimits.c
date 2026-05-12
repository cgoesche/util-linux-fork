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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "c.h"
#include "cctype.h"
#include "list.h"
#include "nls.h"
#include "xalloc.h"
#include "closestream.h"
#include "column-list-table.h"
#include "pidutils.h"
#include "pidfd-utils.h"

#include "lslimits.h"
#include "resource.h"

enum {
	COL_PID,
	COL_MEM_MIN,
	COL_MEM_MAX,
	LSLIMITS_NCOLS
};

/*  */
enum {
	LSLIMITS_ASCII =	(1 << 0),
	LSLIMITS_RAW =		(1 << 1),
	LSLIMITS_NOHEADINGS =	(1 << 2),
	LSLIMITS_EXPORT =	(1 << 3),
	LSLIMITS_TREE =		(1 << 4),
	LSLIMITS_JSON =		(1 << 5),
	LSLIMITS_SHELLVAR =	(1 << 6)
};


/* Types used for qsort() and JSON */
enum {
	COLTYPE_STR	= 0,	/* default */
	COLTYPE_NUM	= 1,	/* always u64 number */
	COLTYPE_SORTNUM = 2,	/* string on output, u64 for qsort() */
	COLTYPE_SIZE	= 3,	/* srring by default, number when --bytes */
	COLTYPE_BOOL	= 4	/* 0 or 1 */
};

/* column names */
struct colinfo {
	const char	* const name;	/* header */
	double		whint;		/* width hint (N < 1 is in percent of termwidth) */
	int		flags;		/* SCOLS_FL_* */
	const char	*help;
	int		type;		/* COLTYPE_* */
};

/* columns descriptions */
static const struct colinfo infos[] = {
	[COL_PID] = { "PID", 6, SCOLS_FL_RIGHT, N_("process ID"), COLTYPE_NUM },
	[COL_MEM_MIN] = { "MEMORY-MIN", 6, SCOLS_FL_RIGHT, N_("minimum memory usage"), COLTYPE_STR },
	[COL_MEM_MAX] = { "MEMORY-MAX", 1, SCOLS_FL_RIGHT, N_("maximum memory usage"), COLTYPE_STR },
};

static const int default_columns[] = {
	COL_PID,
	COL_MEM_MIN,
};

static int columns[ARRAY_SIZE(infos) * 2];
static size_t ncolumns;

static inline void add_column(int id)
{
	if (ncolumns >= ARRAY_SIZE(columns))
		errx(EXIT_FAILURE, _("too many columns specified, "
				     "the limit is %zu columns"),
				ARRAY_SIZE(columns) - 1);
	columns[ ncolumns++ ] =  id;
}

/* Converts column ID (COL_*) to column sequential number */
static int column_id_to_number(int id)
{
	size_t i = -1;

	for (i = 0; i < ncolumns; i++)
		if (columns[i] == id)
			return i;
	return i;
}

/* Converts column sequential number to column ID (COL_*) */
static int get_column_id(int num)
{
	assert(num >= 0);
	assert((size_t) num < ncolumns);
	assert(columns[num] < (int) ARRAY_SIZE(infos));
	return columns[num];
}

/* Returns column description for the column sequential number */
static const struct colinfo *get_column_info(int num)
{
	return &infos[ get_column_id(num) ];
}

/* Converts column name (as defined in the infos[] to the column ID */
static int column_name_to_id(const char *name, size_t namesz)
{
	size_t i;

	/* name as displayed for users */
	for (i = 0; i < ARRAY_SIZE(infos); i++) {
		const char *cn = infos[i].name;

		if (!c_strncasecmp(name, cn, namesz) && !*(cn + namesz))
			return i;
	}

	/* name as used in expressions, JSON output etc. */
	if (strnchr(name, namesz, '_')) {
		char *buf = NULL;
		size_t bufsz = 0;

		for (i = 0; i < ARRAY_SIZE(infos); i++) {
			if (scols_shellvar_name(infos[i].name, &buf, &bufsz) != 0)
				continue;
			if (!c_strncasecmp(name, buf, namesz) && !*(buf + namesz)) {
				free(buf);
				return i;
			}
		}
		free(buf);
	}

	warnx(_("unknown column: %s"), name);
	return -1;
}

static inline void add_uniq_column(int id)
{
	if (column_id_to_number(id) < 0)
		add_column(id);
}


static void __attribute__((__noreturn__)) list_columns(struct lslimits_ctx *ctx)
{
	size_t i;
	struct libscols_table *tb = xcolumn_list_table_new("lslimits-columns", stdout,
						ctx->table_flags & LSLIMITS_RAW,
						ctx->table_flags & LSLIMITS_JSON);

	for (i = 0; i < ARRAY_SIZE(infos); i++) {
		const struct colinfo *ci = &infos[i];

		xcolumn_list_table_append_line(tb, ci->name,
			ci->type == COLTYPE_SIZE ? -1 :
			ci->type == COLTYPE_NUM  ? SCOLS_JSON_NUMBER :
			ci->type == COLTYPE_BOOL ? SCOLS_JSON_BOOLEAN :
			ci->flags & SCOLS_FL_WRAP ? SCOLS_JSON_ARRAY_STRING :
				SCOLS_JSON_STRING,
			ci->type == COLTYPE_SIZE ? "<string|number>" : NULL,
			_(ci->help));
	}

	scols_print_table(tb);
	scols_unref_table(tb);

	exit(EXIT_SUCCESS);
}

static void __attribute__((__noreturn__)) usage(void)
{
	fputs(USAGE_HEADER, stdout);
	fprintf(stdout, _(" %s [options] PID[:inode]...\n"), program_invocation_short_name);

	fputs(USAGE_SEPARATOR, stdout);
	fputsln(_("List various resource limits for a given PID."), stdout);

	fputs(USAGE_OPTIONS, stdout);
	fputsln(_(" -c, --cpu            show CPU limits"), stdout);
	fputsln(_(" -f, --file           show file limits"), stdout);
	fputsln(_(" -d, --io             show IO limits"), stdout);
	fputsln(_(" -m, --memory         show memory limits"), stdout);
	fputsln(_(" -n, --network        show network limits"), stdout);
	fputsln(_(" -p, --pid            show process creation limits"), stdout);

	fputs(USAGE_SEPARATOR, stdout);
	fprintf(stdout, USAGE_LIST_COLUMNS_OPTION(22));
	fprintf(stdout, USAGE_HELP_OPTIONS(22)); /* char offset to align option descriptions */
	fprintf(stdout, USAGE_MAN_TAIL("lslimits(1)"));
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int c, pfd = 0, rc;
	uint64_t pidfd_ino = 0;
	bool output_all, collist = false;
	char *annotate_opt_arg = NULL;

	struct process *_proc = xmalloc(sizeof(struct process));
	struct lslimits_ctx _ctx = {
		.table_flags = LSLIMITS_ASCII,
		.proc = _proc,
	};
	struct lslimits_ctx *ctx = &_ctx;

	enum {
		OPT_OUTPUT_ALL = CHAR_MAX + 1,
		OPT_ANNOTATE,
	};
	static const struct option longopts[] = {
		{ "cpu",       no_argument,       NULL, 'c'            },
		{ "file",       no_argument,       NULL, 'f'            },
		{ "io",       no_argument,       NULL, 'd'            },
		{ "memory",       no_argument,       NULL, 'm'            },
		{ "network",       no_argument,       NULL, 'n'            },
		{ "pid",       no_argument,       NULL, 'p'            },
		{ "output-all",   no_argument,       NULL, OPT_OUTPUT_ALL },
		{ "annotate",   optional_argument,       NULL, OPT_ANNOTATE },
		{ "list-columns", no_argument,     NULL, 'H'              },
		{ "version",      no_argument,       NULL, 'V'            },
		{ "help",         no_argument,       NULL, 'h'            },
		{ NULL, 0, NULL, 0 }
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	close_stdout_atexit();

	while ((c = getopt_long(argc, argv, "cdfHmnpVh", longopts, NULL)) != -1) {
		switch (c) {
		case 'c':
			ctx->res_types |= FL_RESOURCE_TYPE_CPU;
			break;
		case 'd':
			ctx->res_types |= FL_RESOURCE_TYPE_IO;
			break;
		case 'f':
			ctx->res_types |= FL_RESOURCE_TYPE_FILE;
			break;
		case 'm':
			ctx->res_types |= FL_RESOURCE_TYPE_MEM;
			break;
		case 'n':
			ctx->res_types |= FL_RESOURCE_TYPE_NET;
			break;
		case 'p':
			ctx->res_types |= FL_RESOURCE_TYPE_PID;
			break;
		case 'H':
			collist = true;
			break;
		case OPT_OUTPUT_ALL:
			output_all = true;
			break;
		case OPT_ANNOTATE:
			annotate_opt_arg = optarg;
			break;
		case 'V':
			print_version(EXIT_SUCCESS);
		case 'h':
			usage();
		default:
			errtryhelp(EXIT_FAILURE);
		}
	}

	if (argc - optind < 1) {
		warnx(_("missing PID argument"));
		errtryhelp(EXIT_FAILURE);
	}

	if (collist)
		list_columns(ctx);        /* print end exit */

	if (annotationwanted(annotate_opt_arg))
		ctx->annotate_col_headers = true;

	ul_parse_pid_str_or_err(argv[optind], &ctx->proc->pid, &pidfd_ino, 0);
	if (pidfd_ino > 0)
			pfd = ul_get_valid_pidfd_or_err(ctx->proc->pid, pidfd_ino);

	rc = collect_rlimit_data(ctx->proc);
	if (rc)
		goto done;

	init_proc_resources(ctx->proc, ctx->res_types);

	for (size_t i = 0; i < ARRAY_SIZE(ctx->proc->rlimits); i++) {
		uint64_t val = ctx->proc->rlimits[i].rlim.rlim_cur;
		const char *name = ctx->proc->rlimits[i].desc->name;
		printf("%20s\t%zu\n", name, val);
	}

done:
	if (pfd)
		close(pfd);

	lslimits_free_context(ctx);
	return EXIT_SUCCESS;
}
