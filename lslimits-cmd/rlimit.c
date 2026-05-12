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


int get_curr_rlimit_resource(pid_t pid, int resource, struct rlimit *rlim)
{
	return prlimit(pid, resource, NULL, rlim);
}

int collect_rlimit_data(struct process *proc)
{
	int rc = 0;

	assert(proc);

	for (size_t id = 0; id < _NRLIMIT_RESOURCES; id++) {
		struct rlimit_description *desc = &rlimit_desc[id];
		struct proc_rlimit *rlimit = &proc->rlimits[id];

		rlimit->desc = desc;

		rc = get_curr_rlimit_resource(proc->pid, rlimit->desc->resource, &rlimit->rlim);
		if (rc) {
			warn("failed to get %s limit for PID %d", rlimit->desc->name, proc->pid);
			return rc;
		}
	}
	return rc;
}
