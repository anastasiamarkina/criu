#ifndef __CR_IMAGE_DEFS_H__
#define __CR_IMAGE_DEFS_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "asm/int.h"

struct cr_options;

/* file descriptors template */
struct cr_fd_desc_tmpl {
	const char	*fmt;			/* format for the name */
	u32		magic;			/* magic in the header */
	void		(*show)(int fd, struct cr_options *o);
};

#define CR_FD_PERM	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

#define O_DUMP		(O_RDWR | O_CREAT | O_EXCL)
#define O_SHOW		(O_RDONLY)
#define O_RSTR		(O_RDONLY)

enum {
	CR_FD_INVENTORY,
	/*
	 * Task entries
	 */

	_CR_FD_TASK_FROM,
	CR_FD_FILE_LOCKS,
	CR_FD_CORE,
	CR_FD_IDS,
	CR_FD_MM,
	CR_FD_VMAS,
	CR_FD_SIGACT,
	CR_FD_ITIMERS,
	CR_FD_CREDS,
	CR_FD_FS,
	CR_FD_RLIMIT,
	CR_FD_SIGNAL,
	_CR_FD_TASK_TO,

	CR_FD_PAGEMAP,

	/*
	 * NS entries
	 */

	_CR_FD_NS_FROM,
	CR_FD_UTSNS,
	CR_FD_IPCNS_VAR,
	CR_FD_IPCNS_SHM,
	CR_FD_IPCNS_MSG,
	CR_FD_IPCNS_SEM,
	CR_FD_MOUNTPOINTS,
	CR_FD_NETDEV,
	CR_FD_IFADDR,
	CR_FD_ROUTE,
	_CR_FD_NS_TO,

	CR_FD_PSTREE,
	CR_FD_SHMEM_PAGEMAP,
	CR_FD_GHOST_FILE,
	CR_FD_TCP_STREAM,
	CR_FD_FDINFO,

	_CR_FD_GLOB_FROM,
	CR_FD_SK_QUEUES,
	CR_FD_REG_FILES,
	CR_FD_INETSK,
	CR_FD_UNIXSK,
	CR_FD_PACKETSK,
	CR_FD_NETLINKSK,
	CR_FD_PIPES,
	CR_FD_PIPES_DATA,
	CR_FD_FIFO,
	CR_FD_FIFO_DATA,
	CR_FD_TTY,
	CR_FD_TTY_INFO,
	CR_FD_REMAP_FPATH,
	CR_FD_EVENTFD,
	CR_FD_EVENTPOLL,
	CR_FD_EVENTPOLL_TFD,
	CR_FD_SIGNALFD,
	CR_FD_INOTIFY,
	CR_FD_INOTIFY_WD,
	CR_FD_FANOTIFY,
	CR_FD_FANOTIFY_MARK,
	_CR_FD_GLOB_TO,

	CR_FD_TMPFS,
	CR_FD_PAGES,
	CR_FD_PSIGNAL,

	CR_FD_PAGES_OLD,
	CR_FD_SHM_PAGES_OLD,

	CR_FD_MAX
};

extern struct cr_fd_desc_tmpl fdset_template[CR_FD_MAX];

#endif /* __CR_IMAGE_DEFS_H__ */
