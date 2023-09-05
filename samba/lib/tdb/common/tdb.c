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

TDB_DATA tdb_null;

/*
  increment the tdb sequence number if the tdb has been opened using
  the TDB_SEQNUM flag
*/
static void tdb_increment_seqnum(struct tdb_context *tdb)
{
	tdb_off_t seqnum=0;
	
	if (!(tdb->flags & TDB_SEQNUM)) {
		return;
	}

	if (tdb_brlock(tdb, TDB_SEQNUM_OFS, F_WRLCK, F_SETLKW, 1, 1) != 0) {
		return;
	}

	/* we ignore errors from this, as we have no sane way of
	   dealing with them.
	*/
	tdb_ofs_read(tdb, TDB_SEQNUM_OFS, &seqnum);
	seqnum++;
	tdb_ofs_write(tdb, TDB_SEQNUM_OFS, &seqnum);

	tdb_brlock(tdb, TDB_SEQNUM_OFS, F_UNLCK, F_SETLKW, 1, 1);
}


/* Returns 0 on fail.  On success, return offset of record, and fills
   in rec */
static tdb_off_t tdb_find(struct tdb_context *tdb, TDB_DATA key, u32 hash,
			struct list_struct *r)
{
	tdb_off_t rec_ptr;
	
	/* read in the hash top */
	if (tdb_ofs_read(tdb, TDB_HASH_TOP(hash), &rec_ptr) == -1)
		return 0;

	/* keep looking until we find the right record */
	while (rec_ptr) {
		if (tdb_rec_read(tdb, rec_ptr, r) == -1)
			return 0;

		if (!TDB_DEAD(r) && hash==r->full_hash && key.dsize==r->key_len) {
			unsigned char *k;
			/* a very likely hit - read the key */
			k = tdb_alloc_read(tdb, rec_ptr + sizeof(*r), 
					   r->key_len);
			if (!k)
				return 0;

			if (memcmp(key.dptr, k, key.dsize) == 0) {
				SAFE_FREE(k);
				return rec_ptr;
			}
			SAFE_FREE(k);
		}
		rec_ptr = r->next;
	}
	return TDB_ERRCODE(TDB_ERR_NOEXIST, 0);
}

/* As tdb_find, but if you succeed, keep the lock */
tdb_off_t tdb_find_lock_hash(struct tdb_context *tdb, TDB_DATA key, u32 hash, int locktype,
			   struct list_struct *rec)
{
	u32 rec_ptr;

	if (tdb_lock(tdb, BUCKET(hash), locktype) == -1)
		return 0;
	if (!(rec_ptr = tdb_find(tdb, key, hash, rec)))
		tdb_unlock(tdb, BUCKET(hash), locktype);
	return rec_ptr;
}


/* update an entry in place - this only works if the new data size
   is <= the old data size and the key exists.
   on failure return -1.
*/
static int tdb_update_hash(struct tdb_context *tdb, TDB_DATA key, u32 hash, TDB_DATA dbuf)
{
	struct list_struct rec;
	tdb_off_t rec_ptr;

	/* find entry */
	if (!(rec_ptr = tdb_find(tdb, key, hash, &rec)))
		return -1;

	/* must be long enough key, data and tailer */
	if (rec.rec_len < key.dsize + dbuf.dsize + sizeof(tdb_off_t)) {
		tdb->ecode = TDB_SUCCESS; /* Not really an error */
		return -1;
	}

	if (tdb->methods->tdb_write(tdb, rec_ptr + sizeof(rec) + rec.key_len,
		      dbuf.dptr, dbuf.dsize) == -1)
		return -1;

	if (dbuf.dsize != rec.data_len) {
		/* update size */
		rec.data_len = dbuf.dsize;
		return tdb_rec_write(tdb, rec_ptr, &rec);
	}
 
	return 0;
}

/* find an entry in the database given a key */
/* If an entry doesn't exist tdb_err will be set to
 * TDB_ERR_NOEXIST. If a key has no data attached
 * then the TDB_DATA will have zero length but
 * a non-zero pointer
 */
TDB_DATA tdb_fetch(struct tdb_context *tdb, TDB_DATA key)
{
	tdb_off_t rec_ptr;
	struct list_struct rec;
	TDB_DATA ret;
	u32 hash;

	/* find which hash bucket it is in */
	hash = tdb->hash_fn(&key);
	if (!(rec_ptr = tdb_find_lock_hash(tdb,key,hash,F_RDLCK,&rec)))
		return tdb_null;

	ret.dptr = tdb_alloc_read(tdb, rec_ptr + sizeof(rec) + rec.key_len,
				  rec.data_len);
	ret.dsize = rec.data_len;
	tdb_unlock(tdb, BUCKET(rec.full_hash), F_RDLCK);
	return ret;
}

/* check if an entry in the database exists 

   note that 1 is returned if the key is found and 0 is returned if not found
   this doesn't match the conventions in the rest of this module, but is
   compatible with gdbm
*/
static int tdb_exists_hash(struct tdb_context *tdb, TDB_DATA key, u32 hash)
{
	struct list_struct rec;
	
	if (tdb_find_lock_hash(tdb, key, hash, F_RDLCK, &rec) == 0)
		return 0;
	tdb_unlock(tdb, BUCKET(rec.full_hash), F_RDLCK);
	return 1;
}

int tdb_exists(struct tdb_context *tdb, TDB_DATA key)
{
	u32 hash = tdb->hash_fn(&key);
	return tdb_exists_hash(tdb, key, hash);
}

/* actually delete an entry in the database given the offset */
int tdb_do_delete(struct tdb_context *tdb, tdb_off_t rec_ptr, struct list_struct*rec)
{
	tdb_off_t last_ptr, i;
	struct list_struct lastrec;

	if (tdb->read_only || tdb->traverse_read) return -1;

	if (tdb_write_lock_record(tdb, rec_ptr) == -1) {
		/* Someone traversing here: mark it as dead */
		rec->magic = TDB_DEAD_MAGIC;
		return tdb_rec_write(tdb, rec_ptr, rec);
	}
	if (tdb_write_unlock_record(tdb, rec_ptr) != 0)
		return -1;

	/* find previous record in hash chain */
	if (tdb_ofs_read(tdb, TDB_HASH_TOP(rec->full_hash), &i) == -1)
		return -1;
	for (last_ptr = 0; i != rec_ptr; last_ptr = i, i = lastrec.next)
		if (tdb_rec_read(tdb, i, &lastrec) == -1)
			return -1;

	/* unlink it: next ptr is at start of record. */
	if (last_ptr == 0)
		last_ptr = TDB_HASH_TOP(rec->full_hash);
	if (tdb_ofs_write(tdb, last_ptr, &rec->next) == -1)
		return -1;

	/* recover the space */
	if (tdb_free(tdb, rec_ptr, rec) == -1)
		return -1;
	return 0;
}

/* delete an entry in the database given a key */
static int tdb_delete_hash(struct tdb_context *tdb, TDB_DATA key, u32 hash)
{
	tdb_off_t rec_ptr;
	struct list_struct rec;
	int ret;

	if (!(rec_ptr = tdb_find_lock_hash(tdb, key, hash, F_WRLCK, &rec)))
		return -1;
	ret = tdb_do_delete(tdb, rec_ptr, &rec);

	if (ret == 0) {
		tdb_increment_seqnum(tdb);
	}

	if (tdb_unlock(tdb, BUCKET(rec.full_hash), F_WRLCK) != 0)
		TDB_LOG((tdb, TDB_DEBUG_WARNING, "tdb_delete: WARNING tdb_unlock failed!\n"));
	return ret;
}

int tdb_delete(struct tdb_context *tdb, TDB_DATA key)
{
	u32 hash = tdb->hash_fn(&key);
	return tdb_delete_hash(tdb, key, hash);
}

/* store an element in the database, replacing any existing element
   with the same key 

   return 0 on success, -1 on failure
*/
int tdb_store(struct tdb_context *tdb, TDB_DATA key, TDB_DATA dbuf, int flag)
{
	struct list_struct rec;
	u32 hash;
	tdb_off_t rec_ptr;
	char *p = NULL;
	int ret = 0;

	if (tdb->read_only || tdb->traverse_read) {
		tdb->ecode = TDB_ERR_RDONLY;
		return -1;
	}

	/* find which hash bucket it is in */
	hash = tdb->hash_fn(&key);
	if (tdb_lock(tdb, BUCKET(hash), F_WRLCK) == -1)
		return -1;

	/* check for it existing, on insert. */
	if (flag == TDB_INSERT) {
		if (tdb_exists_hash(tdb, key, hash)) {
			tdb->ecode = TDB_ERR_EXISTS;
			goto fail;
		}
	} else {
		/* first try in-place update, on modify or replace. */
		if (tdb_update_hash(tdb, key, hash, dbuf) == 0)
			goto out;
		if (tdb->ecode == TDB_ERR_NOEXIST &&
		    flag == TDB_MODIFY) {
			/* if the record doesn't exist and we are in TDB_MODIFY mode then
			 we should fail the store */
			goto fail;
		}
	}
	/* reset the error code potentially set by the tdb_update() */
	tdb->ecode = TDB_SUCCESS;

	/* delete any existing record - if it doesn't exist we don't
           care.  Doing this first reduces fragmentation, and avoids
           coalescing with `allocated' block before it's updated. */
	if (flag != TDB_INSERT)
		tdb_delete_hash(tdb, key, hash);

	/* Copy key+value *before* allocating free space in case malloc
	   fails and we are left with a dead spot in the tdb. */

	if (!(p = (char *)malloc(key.dsize + dbuf.dsize))) {
		tdb->ecode = TDB_ERR_OOM;
		goto fail;
	}

	memcpy(p, key.dptr, key.dsize);
	if (dbuf.dsize)
		memcpy(p+key.dsize, dbuf.dptr, dbuf.dsize);

	/* we have to allocate some space */
	if (!(rec_ptr = tdb_allocate(tdb, key.dsize + dbuf.dsize, &rec)))
		goto fail;

	/* Read hash top into next ptr */
	if (tdb_ofs_read(tdb, TDB_HASH_TOP(hash), &rec.next) == -1)
		goto fail;

	rec.key_len = key.dsize;
	rec.data_len = dbuf.dsize;
	rec.full_hash = hash;
	rec.magic = TDB_MAGIC;

	/* write out and point the top of the hash chain at it */
	if (tdb_rec_write(tdb, rec_ptr, &rec) == -1
	    || tdb->methods->tdb_write(tdb, rec_ptr+sizeof(rec), p, key.dsize+dbuf.dsize)==-1
	    || tdb_ofs_write(tdb, TDB_HASH_TOP(hash), &rec_ptr) == -1) {
		/* Need to tdb_unallocate() here */
		goto fail;
	}

	tdb_increment_seqnum(tdb);

 out:
	SAFE_FREE(p); 
	tdb_unlock(tdb, BUCKET(hash), F_WRLCK);
	return ret;
fail:
	ret = -1;
	goto out;
}


/* Append to an entry. Create if not exist. */
int tdb_append(struct tdb_context *tdb, TDB_DATA key, TDB_DATA new_dbuf)
{
	u32 hash;
	TDB_DATA dbuf;
	int ret = -1;

	/* find which hash bucket it is in */
	hash = tdb->hash_fn(&key);
	if (tdb_lock(tdb, BUCKET(hash), F_WRLCK) == -1)
		return -1;

	dbuf = tdb_fetch(tdb, key);

	if (dbuf.dptr == NULL) {
		dbuf.dptr = malloc(new_dbuf.dsize);
	} else {
		dbuf.dptr = realloc(dbuf.dptr, dbuf.dsize + new_dbuf.dsize);
	}

	if (dbuf.dptr == NULL) {
		tdb->ecode = TDB_ERR_OOM;
		goto failed;
	}

	memcpy(dbuf.dptr + dbuf.dsize, new_dbuf.dptr, new_dbuf.dsize);
	dbuf.dsize += new_dbuf.dsize;

	ret = tdb_store(tdb, key, dbuf, 0);
	
failed:
	tdb_unlock(tdb, BUCKET(hash), F_WRLCK);
	SAFE_FREE(dbuf.dptr);
	return ret;
}


/*
  return the name of the current tdb file
  useful for external logging functions
*/
const char *tdb_name(struct tdb_context *tdb)
{
	return tdb->name;
}

/*
  return the underlying file descriptor being used by tdb, or -1
  useful for external routines that want to check the device/inode
  of the fd
*/
int tdb_fd(struct tdb_context *tdb)
{
	return tdb->fd;
}

/*
  return the current logging function
  useful for external tdb routines that wish to log tdb errors
*/
tdb_log_func tdb_log_fn(struct tdb_context *tdb)
{
	return tdb->log.log_fn;
}


/*
  get the tdb sequence number. Only makes sense if the writers opened
  with TDB_SEQNUM set. Note that this sequence number will wrap quite
  quickly, so it should only be used for a 'has something changed'
  test, not for code that relies on the count of the number of changes
  made. If you want a counter then use a tdb record.

  The aim of this sequence number is to allow for a very lightweight
  test of a possible tdb change.
*/
int tdb_get_seqnum(struct tdb_context *tdb)
{
	tdb_off_t seqnum=0;

	tdb_ofs_read(tdb, TDB_SEQNUM_OFS, &seqnum);
	return seqnum;
}

int tdb_hash_size(struct tdb_context *tdb)
{
	return tdb->header.hash_size;
}

size_t tdb_map_size(struct tdb_context *tdb)
{
	return tdb->map_size;
}

int tdb_get_flags(struct tdb_context *tdb)
{
	return tdb->flags;
}

