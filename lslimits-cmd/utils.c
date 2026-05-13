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
#include <stdint.h>
#include <unistd.h>


#include "lslimits.h"


const char *rlimit_id_to_string(int id)
{
	return rlimit_desc[id].name;
}

bool is_unlimited(uint64_t num)
{
	return num == LSLIMITS_UNLIMITED;
}

const char *lslimits_unit_names[_LSLIMITS_NUNITS] = {
	[LSLIMITS_UNIT_BYTES] = "bytes",
	[LSLIMITS_UNIT_COUNT] = "count",
	[LSLIMITS_UNIT_SECS] = "seconds",
};

const char *unit_id_to_name(int id)
{
	return lslimits_unit_names[id];
}
