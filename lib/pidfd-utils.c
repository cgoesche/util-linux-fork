/*
 * No copyright is claimed.  This code is in the public domain; do with
 * it what you wish.
 *
 * Authors: Christian Goeschel Ndjomouo <cgoesc2@wgu.edu> [2025]
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <errno.h>

#include "pidfd-utils.h"
#include "statfs_magic.h"

int ul_pfd_validate_pfd_ino(int pfd, size_t pfd_ino)
{
	struct stat f;
	struct statfs vfs;

	if(fstatfs(pfd, &vfs) != 0) {
		return -errno;
	}

	if(!F_TYPE_EQUAL(vfs.f_type, STATFS_PID_FS_MAGIC)) {
		return -(errno = ENOTSUP);
	}

	fstat(pfd, &f);
	if(errno != 0) {
		return -1;
	}

	if (!(f.st_ino == (ino_t) pfd_ino))
		return -1;
	return 0;
}
