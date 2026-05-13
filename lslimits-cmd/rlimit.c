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

#include <stdbool.h>
#include <sys/resource.h>
#include <unistd.h>

#include "list.h"
#include "xalloc.h"
#include "lslimits.h"

struct rlimit_description rlimit_desc[_NRLIMIT_RESOURCES] = {
	[_RLIMIT_AS] = { .name = "RLIMIT_AS", .resource = RLIMIT_AS },
	[_RLIMIT_CORE] = { .name = "RLIMIT_CORE", .resource = RLIMIT_CORE },
	[_RLIMIT_CPU] = { .name = "RLIMIT_CPU", .resource = RLIMIT_CPU },
	[_RLIMIT_DATA] = { .name = "RLIMIT_DATA", .resource = RLIMIT_DATA },
	[_RLIMIT_FSIZE] = { .name = "RLIMIT_FSIZE", .resource = RLIMIT_FSIZE },
	[_RLIMIT_LOCKS] = { .name = "RLIMIT_LOCKS", .resource = RLIMIT_LOCKS },
	[_RLIMIT_MEMLOCK] = { .name = "RLIMIT_MEMLOCK", .resource = RLIMIT_MEMLOCK },
	[_RLIMIT_MSGQUEUE] = { .name = "RLIMIT_MSGQUEUE", .resource = RLIMIT_MSGQUEUE },
	[_RLIMIT_NICE] = { .name = "RLIMIT_NICE", .resource = RLIMIT_NICE },
	[_RLIMIT_NOFILE] = { .name = "RLIMIT_NOFILE", .resource = RLIMIT_NOFILE },
	[_RLIMIT_NPROC] = { .name = "RLIMIT_NPROC", .resource = RLIMIT_NPROC },
	[_RLIMIT_RSS] = { .name = "RLIMIT_RSS", .resource = RLIMIT_RSS },	/* no-op ever since Linux kernel version 2.4+ */
	[_RLIMIT_RTPRIO] = { .name = "RLIMIT_RTPRIO", .resource = RLIMIT_RTPRIO },
	[_RLIMIT_RTTIME] = { .name = "RLIMIT_RTTIME", .resource = RLIMIT_RTTIME },
	[_RLIMIT_SIGPENDING] = { .name = "RLIMIT_SIGPENDING", .resource = RLIMIT_SIGPENDING },
	[_RLIMIT_STACK] = { .name = "RLIMIT_STACK", .resource = RLIMIT_STACK },
};

int get_curr_rlimit_resource(pid_t pid, int resource, struct rlimit *rlim)
{
	return prlimit(pid, resource, NULL, rlim);
}
