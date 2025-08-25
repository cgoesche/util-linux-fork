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


#include "statfs_magic.h"
#include "strv.h"
#include "strutils.h"
#include "nls.h"
#include "pid-utils.h"

/*
 * Parses the provided @pidstr and assumes the following format
 * "pid:pidfd_inode". pid_num and pidfd_inode are stored in 
 * @pid and @pidfd_ino, respectively.
 *
 */
int ul_parse_pid_str(char *pidstr, pid_t *pid_num, uint64_t *pfd_ino)
{
        int l, rc;
	char **all;
	char *p;
        char *end = NULL;

   	if (!pidstr || !*pidstr || !pid_num) {
		return -(errno = EINVAL);
	}
	
	all = ul_strv_split(pidstr, ":");
	p = (all && *all != NULL) ? *all : pidstr;

        if (pfd_ino) {
                l = ul_strv_length(all);
                if (l == 2 && all++ != NULL) {
                        rc = ul_strtou64(*all, pfd_ino, 10);
		        if (rc != 0)
                                return -(errno = ERANGE);
	        }
	        else if (l > 2){
		        return -(errno = EINVAL);
	        }
        }

        *pid_num = strtol(p, &end, 10);
		if (errno != 0 || ((end && *end != '\0') || p >= end))
                        return -(errno = EINVAL);
	return 0;
}