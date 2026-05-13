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


static bool _memory_is_more_restrictive(struct absolute_limit *winner_lim, struct limit_controller *winner_ctrl,
				struct absolute_limit *curr_lim, struct limit_controller *curr_ctrl)
{

	/* the more restrictive limit wins; always adopt the first result */
	if (default_is_more_restrictive(curr_lim->value, winner_lim->value))
		return true;

	if (curr_lim->value == winner_lim->value
				&& curr_ctrl->type == LIMIT_CONTROLLER_RLIMIT
				&& winner_ctrl && winner_ctrl->type != LIMIT_CONTROLLER_RLIMIT)
		return true;

	return false;
}

/* RLIMIT controller */
static int rlimit_get_absolute_limit(const struct limit_controller *ctrl, struct process *proc,
				struct absolute_limit *abs_lim)
{
	int rc = 0;

	assert(proc);
	assert(ctrl);
	assert(abs_lim);

	int target = ctrl->target.id;
	struct rlimit *r = &proc->rlimits[target].rlim;

	if (r->rlim_cur == RLIM_INFINITY)
		abs_lim->value = r->rlim_max == RLIM_INFINITY ? MAX_OF_UINT_TYPE(uint64_t) : (uint64_t)r->rlim_max;
	else
		abs_lim->value = (uint64_t)r->rlim_cur;

	abs_lim->source = rlimit_id_to_string(target);

	return rc;
}

static const struct limit_controller rlimit_controller = {
	.name = "RLIMIT",
	.type = LIMIT_CONTROLLER_RLIMIT,
	.target.id = _RLIMIT_AS,
	.get_absolute_limit = rlimit_get_absolute_limit,
};

static int init_limit_controllers(struct resource *res)
{
	int rc = 0;

	assert(res);

	res->lim_controllers[LIMIT_CONTROLLER_RLIMIT] = &rlimit_controller;

	return rc;
}

static void deinitialize_memory_resource(struct resource *res)
{
	free(res);
	return;
}

void init_memory_resource(struct process *proc)
{
	int type = RESOURCE_TYPE_MEM;

	assert(proc);
	assert(proc->resources[type] == NULL);

	struct resource *res = xmalloc(sizeof(struct resource));
	res->name = resource_desc[type].name;
	res->unit = resource_desc[type].unit;
	res->is_more_restrictive = _memory_is_more_restrictive;	/* this is the deciding policy */
	res->deinitialize = deinitialize_memory_resource;

	init_limit_controllers(res);

	proc->resources[type] = res;
}


