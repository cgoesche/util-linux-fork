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

#ifndef UTIL_LINUX_LSLIMITS_H
#define UTIL_LINUX_LSLIMITS_H

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/statvfs.h>


#include "c.h"
#include "list.h"
#include "debug.h"


UL_DEBUG_DECLARE_MASK(lslimits);

#define LSLIMITS_DEBUG_INIT		(1 << 1)
#define LSLIMITS_DEBUG_RESOURCE		(1 << 2)

#define DBG(m, x)	__UL_DBG(lslimits, LSLIMITS_DEBUG_, m, x)

/* resource type flags */
#define FL_RESOURCE_TYPE_CPU	(1 << 1)
#define FL_RESOURCE_TYPE_FILE	(1 << 2)
#define FL_RESOURCE_TYPE_IO	(1 << 3)
#define FL_RESOURCE_TYPE_MEM	(1 << 4)
#define FL_RESOURCE_TYPE_NET	(1 << 5)
#define FL_RESOURCE_TYPE_PID	(1 << 6)

/* resource type ids */
enum {
	RESOURCE_TYPE_CPU,
	RESOURCE_TYPE_IO,
	RESOURCE_TYPE_MEM,
	RESOURCE_TYPE_FILE,
	RESOURCE_TYPE_PID,

	_NRESOURCE_TYPES
};

/* data format type */
enum {
	LSLIMITS_BYTES,
	LSLIMITS_COUNT,
	LSLIMITS_MSECS,
};

/* limit controller types */
enum {
	LIMIT_CONTROLLER_CGROUP2,
	LIMIT_CONTROLLER_RLIMIT,

	_NLIMIT_CONTROLLERS
};

/* RLIMIT specific stuff */

/* resource type array ids */
enum {
	_RLIMIT_AS,
	_RLIMIT_CORE,
	_RLIMIT_CPU,
	_RLIMIT_DATA,
	_RLIMIT_FSIZE,
	_RLIMIT_LOCKS,
	_RLIMIT_MEMLOCK,
	_RLIMIT_MSGQUEUE,
	_RLIMIT_NICE,
	_RLIMIT_NOFILE,
	_RLIMIT_NPROC,
	_RLIMIT_RSS,
	_RLIMIT_RTPRIO,
	_RLIMIT_RTTIME,
	_RLIMIT_SIGPENDING,
	_RLIMIT_STACK,

	_NRLIMIT_RESOURCES
};

struct rlimit_description {
	const char *name;
	int resource;
};

static struct rlimit_description rlimit_desc[_NRLIMIT_RESOURCES] = {
	[_RLIMIT_AS] = { .name = "RLIMIT_AS", .resource = RLIMIT_AS },
	[_RLIMIT_CORE] = { .name = "RLIMIT_CORE", .resource = RLIMIT_CORE },
	[_RLIMIT_CPU] = { .name = "RLIMIT_CPU", .resource = RLIMIT_CPU },
	[_RLIMIT_DATA] = { .name = "RLIMIT_DATA", .resource = RLIMIT_DATA },
	[_RLIMIT_FSIZE] = { .name = "RLIMIT_FSIZE", .resource = RLIMIT_FSIZE },
	[_RLIMIT_LOCKS] = { .name = "RLIMIT_LOCKS", .resource = RLIMIT_LOCKS },
	[_RLIMIT_MEMLOCK] = { .name = "RLIMIT_MEMLOCK", .resource = RLIMIT_MEMLOCK },
	[_RLIMIT_MSGQUEUE] = { .name = "RLIMIT_MSGQUEUE", .resource = RLIMIT_MSGQUEUE },
	[_RLIMIT_NICE] = { .name = "RLIMIT_NICE", .resource = RLIMIT_NICE },
	[_RLIMIT_NOFILE] = { .name = "RLIMIT_NOFILE", .resource = RLIMIT_NOFILE },
	[_RLIMIT_NPROC] = { .name = "RLIMIT_NPROC", .resource = RLIMIT_NPROC },
	[_RLIMIT_RSS] = { .name = "RLIMIT_RSS", .resource = RLIMIT_RSS },
	[_RLIMIT_RTPRIO] = { .name = "RLIMIT_RTPRIO", .resource = RLIMIT_RTPRIO },
	[_RLIMIT_RTTIME] = { .name = "RLIMIT_RTTIME", .resource = RLIMIT_RTTIME },
	[_RLIMIT_SIGPENDING] = { .name = "RLIMIT_SIGPENDING", .resource = RLIMIT_SIGPENDING },
	[_RLIMIT_STACK] = { .name = "RLIMIT_STACK", .resource = RLIMIT_STACK },
};


struct proc_rlimit {
	struct rlimit_description *desc;
	struct rlimit rlim;
	uint64_t normalized_data;
	int format_hint;
	int type;
};



/* cgroupv2 specific stuff */
struct proc_cgroup {
	struct list_head cgroup_hiera;

	/* payload */
};

struct limit_source {
	union {
		int rlimit_id;
		struct proc_cgroup *cg;
	} origin;

	enum {
		LIMIT_SOURCE_UNSET = 0,
		LIMIT_SOURCE_RLIMIT,
		LIMIT_SOURCE_CGROUP
	} limit_source;
};

struct lslimits_verdict {
	uint64_t absolute_limit;
	struct limit_source source;
};

struct process {
	pid_t	pid;

	struct proc_rlimit rlimits[_NRLIMIT_RESOURCES];

	struct resource *resources[_NRESOURCE_TYPES];
/*	struct list_head cgroupv2_hiera; */
};

struct limit_controller {
	const char *name;	/* "RLIMIT", "CGROUPv2",... */
	int type;		/*  LIMIT_CONTROLLER_* */

	union {
		int id;
		const char *name;
	} target;

	int (*get_absolute_limit)(struct limit_controller *ctrl,
					struct process *proc,
					uint64_t *abs_val);
};

struct lslimits_ctx {
	unsigned int 	table_flags,
			res_types;	/* resource type flags FL_RESOURCE_* */

	struct process *proc;

	bool	noheadings : 1,
		raw  : 1,
		json : 1,
		annotate_col_headers : 1;
};

/* context.c */
extern void init_resource(struct process *proc, int type);

extern void lslimits_free_context(struct lslimits_ctx *ctx);
extern void free_resource(struct resource *l);
extern void free_proc_rlimit(struct proc_rlimit *prlimit);

/* process.c */
extern void init_proc_resources(struct process *proc, int flags);

/* rlimit.c */
extern int _init_rlimit(struct list_head *lims, int type);
extern int collect_rlimit_data(struct process *proc);
extern int get_curr_rlimit_resource(pid_t pid, int resource, struct rlimit *rlim);

/* cpu.c */
extern void init_cpu_resource(struct resource **r);

/* memory.c */
extern void init_memory_resource(struct process *proc);
extern void deinit_memory_resource(struct resource *res);

/* io.c */
extern void init_io_resource(struct resource **r);

/* file.c */
extern void init_file_resource(struct resource **r);

/* pid.c */
extern void init_pid_resource(struct resource **r);


/* utils.c */
extern int res_flag_to_res_id(int flag);

#endif /* UTIL_LINUX_LSLIMITS_H */
