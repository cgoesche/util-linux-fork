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

struct resource_desc resource_desc[_NRESOURCE_TYPES] = {
	[RESOURCE_TYPE_CPU] = {
		.name = "CPU time",
		.unit = LSLIMITS_UNIT_SECS,
		.init = NULL,
	},
	[RESOURCE_TYPE_IO] = {
		.name = "IO resource",
		.unit = LSLIMITS_UNIT_SECS,
		.init = NULL,
	},
	[RESOURCE_TYPE_MEM] = {
		.name = "Memory",
		.unit = LSLIMITS_UNIT_BYTES,
		.init = init_memory_resource,
	 },
	[RESOURCE_TYPE_FILE] = {
		.name = "file handle count",
		.unit = LSLIMITS_UNIT_COUNT,
		.init = NULL,
	 },
	[RESOURCE_TYPE_PID] = {
		.name = "process count",
		.unit = LSLIMITS_UNIT_COUNT,
		.init = NULL,
	},
};

const char *get_resource_name(struct resource *res)
{
	return res->name ? res->name : _("unknown");
}

uint64_t get_verdict_limit_value(struct resource *res)
{
	return res->verdict.abs_limit.value;
}

const char *get_verdict_controller(struct resource *res)
{
	assert(res);
	return res->verdict.controller ? res->verdict.controller : _("unknown") ;
}

const char *get_verdict_source(struct resource *res)
{
	assert(res);
	return res->verdict.abs_limit.source ? res->verdict.abs_limit.source : _("unknown") ;
}

bool default_is_more_restrictive(uint64_t curr_lim, uint64_t absolute_lim)
{
	return curr_lim < absolute_lim;
}

int make_verdict(struct resource *res, struct process *proc)
{
	int rc = 0;
	bool first_seen = true;

	struct absolute_limit absolute_lim = { .value = LSLIMITS_UNLIMITED };
	const struct limit_controller *winner_ctrl = NULL;

	assert(res);
	assert(proc);

	for (size_t i = 0; i < ARRAY_SIZE(res->lim_controllers); i++) {
		if (res->lim_controllers[i] == NULL)
			continue;

		struct absolute_limit lim = {};
		const struct limit_controller *ctrl = res->lim_controllers[i];

		rc = ctrl->get_absolute_limit(ctrl, proc, &lim);
		if (rc) {
			warnx(_("failed to get absolute limit from %s"), ctrl->name);
			return -1;
		}

		/* The more restrictive limit wins, and the policy provided for it
		 * is defined per-resource type in the is_more_restrictive() callback.
		 */
		if (first_seen || res->is_more_restrictive()) {
			absolute_lim = lim;
			winner_ctrl = ctrl;
			first_seen = false;
		}
	}

	res->verdict.abs_limit = absolute_lim;
	res->verdict.controller = winner_ctrl->name;

	return rc;
}
