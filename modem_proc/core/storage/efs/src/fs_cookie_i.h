/***********************************************************************
 * fs_cookie_i.h
 *
 * EFS Cookie header
 * Copyright (C) 2012 QUALCOMM Technologies, Inc.
 *
 * EFS cookie utility functions that help storing and retrieving cookies
 * from RAM.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_cookie_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-08-09   dks   Rename fs_cookie.h to fs_cookie_i.h.
2012-07-23   dks   Fix bugs for in cookie id validity and payload size check.
2012-06-12   dks   Create

===========================================================================*/

#ifndef __FS_COOKIE_I_H__
#define __FS_COOKIE_I_H__

#include "comdef.h"

/** EFS cookie ID types. Cookie ID is a uint8 type, so we can have only 255
    unique cookie IDs. Increase the cookie ID field size if more cookie IDs
    are required.*/
enum fs_cookie_id
{
  EFS_COOKIE_ID_INVALID,
  EFS_COOKIE_ID_BACKUP,
  EFS_COOKIE_ID_MAX          /* Can be a max of 255. */
};

/** EFS cookie data structure.
 */
PACKED struct fs_cookie
{
  uint8   cookie_id;
  uint16  payload_size;
  uint8   payload[1];    /* Variable size payload. */
}PACKED_POST;


/** Iterator structure to get next cookie data.
 */
struct fs_cookie_iter;

/**
 * Initialize the efs cookie module. To be done only once for the phone boot
 * time. Will purge all data.
 */
void fs_cookie_init (void);


/**
 * Obtains a cookie list manager handle which will manage the supplied cookie
 * ram.
 *
 * @param ram_addr
 *   Address that will be treated as the cookie list start.
 *
 * @param ram_size
 *   Size of the RAM that will function as cookie list.
 *
 * @return
 *   Returns -1 on error and valid handle >= 0 on success.
 */
 int fs_cookie_get_handle_for_ram (void *ram_addr, uint32 ram_size);


/**
 * Adds a new cookie to the cookie list.
 *
 * @param handle
 *   Valid handle returned from fs_cookie_get_handle_for_ram.
 *
 * @param cookie_id
 *   Cookie ID to be added.
 *
 * @param payload
 *   Buffer containing cookie payload data. Cannot be NULL if payload_size > 0.
 *
 * @param payload_size
 *   Size of payload data.
 *
 * @return
 *   0 on success and -1 on failure.
 */
int fs_cookie_add (int handle, uint8 cookie_id, uint8 *payload,
                   uint16 payload_size);


/**
 * Opens a iterator to iterate through all the cookies in the cookie list.
 *
 * @param handle
 *   Valid handle returned by fs_cookie_get_handle_for_ram
 *
 * @return
 *   NULL on failure or a NON-NULL iterator on success.
 */
struct fs_cookie_iter* fs_cookie_iter_open (int handle);


/**
 * Return pointer to next cookie object.
 *
 * @param i_hdl
 *   Valid handle returned by fs_cookie_iter_open.
 *
 * @return
 *   NULL if there are no more cookies or pointer to the next cookie.
 */
struct fs_cookie* fs_cookie_iter_get_next (struct fs_cookie_iter *i_hdl);

/**
 * Close the previously opened iterator.
 *
 * @param iter_hdl
 *   Valid handle returned by fs_cookie_iter_open.
 *
 * @return
 *   0 on success and -1 on failure.
 */
int fs_cookie_iter_close (struct fs_cookie_iter* iter_hdl);


/**
 * Remove all cookies in the list and release all handles.
 *
 * @param handle
 *   Valid handle returned by fs_cookie_get_handle_for_ram.
 *
 * @return
 *   0 on success and -1 on failure.
 */
int fs_cookie_remove_all_cookies (int handle);


/**
 * Release handle obtained from cookie module to manage a cookie RAM.
 * Releases all resources assigned for it.
 *
 * @param handle
 *   Valid handle returned by fs_cookie_get_handle_for_ram.
 *
 * @return
 *   0 on success and -1 on failure.
 */
int fs_cookie_release_handle (int handle);

#endif /* not __FS_COOKIE_I_H__ */
