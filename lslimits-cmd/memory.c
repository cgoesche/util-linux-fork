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
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

#include "c.h"
#include "xalloc.h"
#include "lslimits.h"
#include "resource.h"

/* RLIMIT controller */

static int rlimit_get_abs(struct limit_controller *ctrl, struct process *proc,
				uint64_t *abs_val)
{
	int rc = 0;

	assert(proc);
	assert(ctrl);
	assert(abs_val);

	int target = ctrl->target.id;
	struct rlimit *r = &proc->rlimits[target].rlim;

	if (r->rlim_cur == RLIM_INFINITY)
		*abs_val = MAX_OF_UINT_TYPE(uint64_t);
	else
		*abs_val = (uint64_t)r->rlim_cur;

	return rc;
}

const struct limit_controller rlimit_controller = {
	.name = "RLIMIT",
	.type = LIMIT_CONTROLLER_RLIMIT,
	.target.id = _RLIMIT_AS,
	.get_absolute_limit = rlimit_get_abs,
};

static int init_limit_controllers(struct resource *res)
{
	int rc = 0;

	assert(res);

	res->lim_controllers[LIMIT_CONTROLLER_RLIMIT] = &rlimit_controller;

	return rc;
}

void init_memory_resource(struct process *proc)
{
	assert(proc);

	int type = RESOURCE_TYPE_MEM;

	struct resource *res = xmalloc(sizeof(struct resource));
	res->name = res_def[type].name;
	res->type = type;

	init_limit_controllers(res);

	proc->resources[type] = res;
}

void deinit_memory_resource(struct resource *res)
{
	free(res);
	return;
}


