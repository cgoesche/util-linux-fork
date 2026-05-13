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

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>


#include "xalloc.h"
#include "lslimits.h"
#include "resource.h"


int proc_collect_rlimit_data(struct process *proc)
{
	int rc = 0;

	assert(proc);

	for (size_t id = 0; id < _NRLIMIT_RESOURCES; id++) {
		struct proc_rlimit *rlimit = &proc->rlimits[id];

		rlimit->name = rlimit_desc[id].name;
		rlimit->resource = rlimit_desc[id].resource;

		rc = get_curr_rlimit_resource(proc->pid, rlimit->resource, &rlimit->rlim);
		if (rc) {
			warn(_("failed to get %s limits for PID %d"), rlimit->name, proc->pid);
			return rc;
		}
	}
	return rc;
}

void proc_make_verdicts(struct process *proc)
{
	int rc = 0;

	assert(proc);

	for (size_t i = 0; i < _NRESOURCE_TYPES; i++) {
		struct resource *res = proc->resources[i];

		if (!res)
			continue;

		rc = make_verdict(res, proc);
		if (rc)
			warn(_("failed to make verdict for '%s' resource"), res->name);
	}
}

void init_proc_resources(struct process *proc, int flags)
{
	assert(proc);

	if (flags & FL_RESOURCE_TYPE_MEM)
		resource_desc[RESOURCE_TYPE_MEM].init(proc);

	return;
}
