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

#ifndef UTIL_LINUX_LSLIMITS_RESOURCE_H
#define UTIL_LINUX_LSLIMITS_RESOURCE_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

#include "xalloc.h"
#include "lslimits.h"

struct resource {
	const char *name;
	int 	type;
	int	unit;	/* LSLIMITS_UNIT_* */

	const struct limit_controller *lim_controllers[_NLIMIT_CONTROLLERS];

	bool (*is_more_restrictive)();
	void (*deinitialize)(struct resource *res);

	struct lslimits_verdict verdict;
};

struct resource_desc {
	const char *name;
	int unit;
	void (*init)(struct process *proc);
};

extern struct resource_desc resource_desc[_NRESOURCE_TYPES];

extern int make_verdict(struct resource *res, struct process *proc);
extern bool default_is_more_restrictive(uint64_t curr_lim, uint64_t absolute_lim);

#endif /* UTIL_LINUX_LSLIMITS_RESOURCE_H */
