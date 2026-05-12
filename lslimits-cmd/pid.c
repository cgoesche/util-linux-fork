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

#include "list.h"
#include "xalloc.h"
#include "lslimits.h"


