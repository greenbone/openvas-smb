/*
 * SPDX-FileCopyrightText: Andrew Tridgell              1999-2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

 /* 
   Unix SMB/CIFS implementation.

   trivial database library

   Copyright (C) Andrew Tridgell              1999-2005
   Copyright (C) Paul `Rusty' Russell		   2000
   Copyright (C) Jeremy Allison			   2000-2003
   
     ** NOTE! The following LGPL license applies to the tdb
     ** library. This does NOT imply that all of Samba is released
     ** under the LGPL
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "tdb_private.h"

/* all contexts, to ensure no double-opens (fcntl locks don't nest!) */
static struct tdb_context *tdbs = NULL;


/* This is based on the hash algorithm from gdbm */
static unsigned int default_tdb_hash(TDB_DATA *key)
{
	u32 value;	/* Used to compute the hash value.  */
	u32   i;	/* Used to cycle through random values. */

	/* Set the initial value from the key size. */
	for (value = 0x238F13AF * key->dsize, i=0; i < key->dsize; i++)
		value = (value + (key->dptr[i] << (i*5 % 24)));

	return (1103515243 * value + 12345);  
}


/* initialise a new database with a specified hash size */
static int tdb_new_database(struct tdb_context *tdb, int hash_size)
{
	struct tdb_header *newdb;
	int size, ret = -1;

	/* We make it up in memory, then write it out if not internal */
	size = sizeof(struct tdb_header) + (hash_size+1)*sizeof(tdb_off_t);
	if (!(newdb = (struct tdb_header *)calloc(size, 1)))
		return TDB_ERRCODE(TDB_ERR_OOM, -1);

	/* Fill in the header */
	newdb->version = TDB_VERSION;
	newdb->hash_size = hash_size;
	if (tdb->flags & TDB_INTERNAL) {
		tdb->map_size = size;
		tdb->map_ptr = (char *)newdb;
		memcpy(&tdb->header, newdb, sizeof(tdb->header));
		/* Convert the `ondisk' version if asked. */
		CONVERT(*newdb);
		return 0;
	}
	if (lseek(tdb->fd, 0, SEEK_SET) == -1)
		goto fail;

	if (ftruncate(tdb->fd, 0) == -1)
		goto fail;

	/* This creates an endian-converted header, as if read from disk */
	CONVERT(*newdb);
	memcpy(&tdb->header, newdb, sizeof(tdb->header));
	/* Don't endian-convert the magic food! */
	memcpy(newdb->magic_food, TDB_MAGIC_FOOD, strlen(TDB_MAGIC_FOOD)+1);
	if (write(tdb->fd, newdb, size) != size) {
		ret = -1;
	} else {
		ret = 0;
	}

  fail:
	SAFE_FREE(newdb);
	return ret;
}



static int tdb_already_open(dev_t device,
			    ino_t ino)
{
	struct tdb_context *i;
	
	for (i = tdbs; i; i = i->next) {
		if (i->device == device && i->inode == ino) {
			return 1;
		}
	}

	return 0;
}

/* open the database, creating it if necessary 

   The open_flags and mode are passed straight to the open call on the
   database file. A flags value of O_WRONLY is invalid. The hash size
   is advisory, use zero for a default value.

   Return is NULL on error, in which case errno is also set.  Don't 
   try to call tdb_error or tdb_errname, just do strerror(errno).

   @param name may be NULL for internal databases. */
struct tdb_context *tdb_open(const char *name, int hash_size, int tdb_flags,
		      int open_flags, mode_t mode)
{
	return tdb_open_ex(name, hash_size, tdb_flags, open_flags, mode, NULL, NULL);
}

/* a default logging function */
static void null_log_fn(struct tdb_context *tdb, enum tdb_debug_level level, const char *fmt, ...) PRINTF_ATTRIBUTE(3, 4);
static void null_log_fn(struct tdb_context *tdb, enum tdb_debug_level level, const char *fmt, ...)
{
}


struct tdb_context *tdb_open_ex(const char *name, int hash_size, int tdb_flags,
				int open_flags, mode_t mode,
				const struct tdb_logging_context *log_ctx,
				tdb_hash_func hash_fn)
{
	struct tdb_context *tdb;
	struct stat st;
	int rev = 0, locked = 0;
	unsigned char *vp;
	u32 vertest;

	if (!(tdb = (struct tdb_context *)calloc(1, sizeof *tdb))) {
		/* Can't log this */
		errno = ENOMEM;
		goto fail;
	}
	tdb_io_init(tdb);
	tdb->fd = -1;
	tdb->name = NULL;
	tdb->map_ptr = NULL;
	tdb->flags = tdb_flags;
	tdb->open_flags = open_flags;
	if (log_ctx) {
		tdb->log = *log_ctx;
	} else {
		tdb->log.log_fn = null_log_fn;
		tdb->log.log_private = NULL;
	}
	tdb->hash_fn = hash_fn ? hash_fn : default_tdb_hash;

	/* cache the page size */
	tdb->page_size = getpagesize();
	if (tdb->page_size <= 0) {
		tdb->page_size = 0x2000;
	}

	if ((open_flags & O_ACCMODE) == O_WRONLY) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: can't open tdb %s write-only\n",
			 name));
		errno = EINVAL;
		goto fail;
	}
	
	if (hash_size == 0)
		hash_size = DEFAULT_HASH_SIZE;
	if ((open_flags & O_ACCMODE) == O_RDONLY) {
		tdb->read_only = 1;
		/* read only databases don't do locking or clear if first */
		tdb->flags |= TDB_NOLOCK;
		tdb->flags &= ~TDB_CLEAR_IF_FIRST;
	}

	/* internal databases don't mmap or lock, and start off cleared */
	if (tdb->flags & TDB_INTERNAL) {
		tdb->flags |= (TDB_NOLOCK | TDB_NOMMAP);
		tdb->flags &= ~TDB_CLEAR_IF_FIRST;
		if (tdb_new_database(tdb, hash_size) != 0) {
			TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: tdb_new_database failed!"));
			goto fail;
		}
		goto internal;
	}

	if ((tdb->fd = open(name, open_flags, mode)) == -1) {
		TDB_LOG((tdb, TDB_DEBUG_WARNING, "tdb_open_ex: could not open file %s: %s\n",
			 name, strerror(errno)));
		goto fail;	/* errno set by open(2) */
	}

	/* ensure there is only one process initialising at once */
	if (tdb->methods->tdb_brlock(tdb, GLOBAL_LOCK, F_WRLCK, F_SETLKW, 0, 1) == -1) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: failed to get global lock on %s: %s\n",
			 name, strerror(errno)));
		goto fail;	/* errno set by tdb_brlock */
	}

	/* we need to zero database if we are the only one with it open */
	if ((tdb_flags & TDB_CLEAR_IF_FIRST) &&
	    (locked = (tdb->methods->tdb_brlock(tdb, ACTIVE_LOCK, F_WRLCK, F_SETLK, 0, 1) == 0))) {
		open_flags |= O_CREAT;
		if (ftruncate(tdb->fd, 0) == -1) {
			TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_open_ex: "
				 "failed to truncate %s: %s\n",
				 name, strerror(errno)));
			goto fail; /* errno set by ftruncate */
		}
	}

	if (read(tdb->fd, &tdb->header, sizeof(tdb->header)) != sizeof(tdb->header)
	    || strcmp(tdb->header.magic_food, TDB_MAGIC_FOOD) != 0
	    || (tdb->header.version != TDB_VERSION
		&& !(rev = (tdb->header.version==TDB_BYTEREV(TDB_VERSION))))) {
		/* its not a valid database - possibly initialise it */
		if (!(open_flags & O_CREAT) || tdb_new_database(tdb, hash_size) == -1) {
			errno = EIO; /* ie bad format or something */
			goto fail;
		}
		rev = (tdb->flags & TDB_CONVERT);
	}
	vp = (unsigned char *)&tdb->header.version;
	vertest = (((u32)vp[0]) << 24) | (((u32)vp[1]) << 16) |
		  (((u32)vp[2]) << 8) | (u32)vp[3];
	tdb->flags |= (vertest==TDB_VERSION) ? TDB_BIGENDIAN : 0;
	if (!rev)
		tdb->flags &= ~TDB_CONVERT;
	else {
		tdb->flags |= TDB_CONVERT;
		tdb_convert(&tdb->header, sizeof(tdb->header));
	}
	if (fstat(tdb->fd, &st) == -1)
		goto fail;

	if (tdb->header.rwlocks != 0) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: spinlocks no longer supported\n"));
		goto fail;
	}

	/* Is it already in the open list?  If so, fail. */
	if (tdb_already_open(st.st_dev, st.st_ino)) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: "
			 "%s (%d,%d) is already open in this process\n",
			 name, (int)st.st_dev, (int)st.st_ino));
		errno = EBUSY;
		goto fail;
	}

	if (!(tdb->name = (char *)strdup(name))) {
		errno = ENOMEM;
		goto fail;
	}

	tdb->map_size = st.st_size;
	tdb->device = st.st_dev;
	tdb->inode = st.st_ino;
	tdb->locked = (struct tdb_lock_type *)calloc(tdb->header.hash_size+1,
						     sizeof(tdb->locked[0]));
	if (!tdb->locked) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: "
			 "failed to allocate lock structure for %s\n",
			 name));
		errno = ENOMEM;
		goto fail;
	}
	tdb_mmap(tdb);
	if (locked) {
		if (tdb->methods->tdb_brlock(tdb, ACTIVE_LOCK, F_UNLCK, F_SETLK, 0, 1) == -1) {
			TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: "
				 "failed to take ACTIVE_LOCK on %s: %s\n",
				 name, strerror(errno)));
			goto fail;
		}

	}

	/* We always need to do this if the CLEAR_IF_FIRST flag is set, even if
	   we didn't get the initial exclusive lock as we need to let all other
	   users know we're using it. */

	if (tdb_flags & TDB_CLEAR_IF_FIRST) {
		/* leave this lock in place to indicate it's in use */
		if (tdb->methods->tdb_brlock(tdb, ACTIVE_LOCK, F_RDLCK, F_SETLKW, 0, 1) == -1)
			goto fail;
	}

	/* if needed, run recovery */
	if (tdb_transaction_recover(tdb) == -1) {
		goto fail;
	}

 internal:
	/* Internal (memory-only) databases skip all the code above to
	 * do with disk files, and resume here by releasing their
	 * global lock and hooking into the active list. */
	if (tdb->methods->tdb_brlock(tdb, GLOBAL_LOCK, F_UNLCK, F_SETLKW, 0, 1) == -1)
		goto fail;
	tdb->next = tdbs;
	tdbs = tdb;
	return tdb;

 fail:
	{ int save_errno = errno;

	if (!tdb)
		return NULL;
	
	if (tdb->map_ptr) {
		if (tdb->flags & TDB_INTERNAL)
			SAFE_FREE(tdb->map_ptr);
		else
			tdb_munmap(tdb);
	}
	SAFE_FREE(tdb->name);
	if (tdb->fd != -1)
		if (close(tdb->fd) != 0)
			TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_open_ex: failed to close tdb->fd on error!\n"));
	SAFE_FREE(tdb->locked);
	SAFE_FREE(tdb);
	errno = save_errno;
	return NULL;
	}
}

/**
 * Close a database.
 *
 * @returns -1 for error; 0 for success.
 **/
int tdb_close(struct tdb_context *tdb)
{
	struct tdb_context **i;
	int ret = 0;

	if (tdb->transaction) {
		tdb_transaction_cancel(tdb);
	}

	if (tdb->map_ptr) {
		if (tdb->flags & TDB_INTERNAL)
			SAFE_FREE(tdb->map_ptr);
		else
			tdb_munmap(tdb);
	}
	SAFE_FREE(tdb->name);
	if (tdb->fd != -1)
		ret = close(tdb->fd);
	SAFE_FREE(tdb->locked);

	/* Remove from contexts list */
	for (i = &tdbs; *i; i = &(*i)->next) {
		if (*i == tdb) {
			*i = tdb->next;
			break;
		}
	}

	memset(tdb, 0, sizeof(*tdb));
	SAFE_FREE(tdb);

	return ret;
}

/* register a loging function */
void tdb_set_logging_function(struct tdb_context *tdb,
                              const struct tdb_logging_context *log)
{
        tdb->log = *log;
}

void *tdb_get_logging_private(struct tdb_context *tdb)
{
	return tdb->log.log_private;
}

/* reopen a tdb - this can be used after a fork to ensure that we have an independent
   seek pointer from our parent and to re-establish locks */
int tdb_reopen(struct tdb_context *tdb)
{
	struct stat st;

	if (tdb->flags & TDB_INTERNAL) {
		return 0; /* Nothing to do. */
	}

	if (tdb->num_locks != 0 || tdb->global_lock.count) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_reopen: reopen not allowed with locks held\n"));
		goto fail;
	}

	if (tdb->transaction != 0) {
		TDB_LOG((tdb, TDB_DEBUG_ERROR, "tdb_reopen: reopen not allowed inside a transaction\n"));
		goto fail;
	}

	if (tdb_munmap(tdb) != 0) {
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: munmap failed (%s)\n", strerror(errno)));
		goto fail;
	}
	if (close(tdb->fd) != 0)
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: WARNING closing tdb->fd failed!\n"));
	tdb->fd = open(tdb->name, tdb->open_flags & ~(O_CREAT|O_TRUNC), 0);
	if (tdb->fd == -1) {
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: open failed (%s)\n", strerror(errno)));
		goto fail;
	}
	if ((tdb->flags & TDB_CLEAR_IF_FIRST) && 
	    (tdb->methods->tdb_brlock(tdb, ACTIVE_LOCK, F_RDLCK, F_SETLKW, 0, 1) == -1)) {
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: failed to obtain active lock\n"));
		goto fail;
	}
	if (fstat(tdb->fd, &st) != 0) {
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: fstat failed (%s)\n", strerror(errno)));
		goto fail;
	}
	if (st.st_ino != tdb->inode || st.st_dev != tdb->device) {
		TDB_LOG((tdb, TDB_DEBUG_FATAL, "tdb_reopen: file dev/inode has changed!\n"));
		goto fail;
	}
	tdb_mmap(tdb);

	return 0;

fail:
	tdb_close(tdb);
	return -1;
}

/* reopen all tdb's */
int tdb_reopen_all(int parent_longlived)
{
	struct tdb_context *tdb;

	for (tdb=tdbs; tdb; tdb = tdb->next) {
		/*
		 * If the parent is longlived (ie. a
		 * parent daemon architecture), we know
		 * it will keep it's active lock on a
		 * tdb opened with CLEAR_IF_FIRST. Thus
		 * for child processes we don't have to
		 * add an active lock. This is essential
		 * to improve performance on systems that
		 * keep POSIX locks as a non-scalable data
		 * structure in the kernel.
		 */
		if (parent_longlived) {
			/* Ensure no clear-if-first. */
			tdb->flags &= ~TDB_CLEAR_IF_FIRST;
		}

		if (tdb_reopen(tdb) != 0)
			return -1;
	}

	return 0;
}
