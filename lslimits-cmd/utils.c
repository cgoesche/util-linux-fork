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
#include <unistd.h>


#include "lslimits.h"


int res_flag_to_res_id(int flag)
{
	int res_id = -1;

	switch (flag) {
	case FL_RESOURCE_TYPE_CPU:
		res_id = RESOURCE_TYPE_CPU;
	case FL_RESOURCE_TYPE_MEM:
		res_id = RESOURCE_TYPE_MEM;
	case FL_RESOURCE_TYPE_FILE:
		res_id = RESOURCE_TYPE_FILE;
	case FL_RESOURCE_TYPE_PID:
		res_id = RESOURCE_TYPE_PID;
	default:
		break;
	}
	return res_id;
}
