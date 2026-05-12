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

struct resource {
	const char *name;
	int type;

	const struct limit_controller *lim_controllers[_NLIMIT_CONTROLLERS];

	struct lslimits_verdict verdict;
};

struct resource_def {
	const char *name;
	int type;
	void (*init)(struct process *proc);
	void (*free_resource)(struct resource *res);
};

static struct resource_def res_def[_NRESOURCE_TYPES] = {
	[RESOURCE_TYPE_CPU] = {
		.name = "CPU resource",
		.type = RESOURCE_TYPE_CPU,
		.init = NULL,
	},
	[RESOURCE_TYPE_IO] = {
		.name = "IO resource",
		.type = RESOURCE_TYPE_IO,
		.init = NULL,
	},
	[RESOURCE_TYPE_MEM] = {
		.name = "memory resource",
		.type = RESOURCE_TYPE_MEM,
		.init = init_memory_resource,
		.free_resource = deinit_memory_resource,
	 },
	[RESOURCE_TYPE_FILE] = {
		.name = "file resource",
		.type = RESOURCE_TYPE_FILE,
		.init = NULL,
	 },
	[RESOURCE_TYPE_PID] = {
		.name = "PID resource",
		.type = RESOURCE_TYPE_PID,
		.init = NULL,
	},
};
