/***********************************************************************
 * fs_cookie.c
 *
 * EFS cookie implementation.
 *
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file implements the functions to set and get EFS cookies in the
 * cookie RAM.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_cookie.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-08-09   dks   Rename fs_cookie.h to fs_cookie_i.h.
2012-07-23   dks   Fix bugs in cookie id validity and payload size check.
2012-06-12   dks   Create

===========================================================================*/

#include "fs_cookie_i.h"
#include "fs_err.h"
#include "fs_util.h"

#include "assert.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

/* Will change when list or cookie structure or anything inside the cookie
   logic changes. Upgrade logic needs to be implemented. */
#define EFS_COOKIE_VERSION     0x1

#define FS_COOKIE_MAX_HANDLES  1
#define FS_COOKIE_MAX_ITERS    1

#define EFS_COOKIE_MAGIC1      0x4f435346   /* FSCO */
#define EFS_COOKIE_MAGIC2      0x45494b4f   /* OKIE */


/**************** Cookie List layout in cookie partition ****************//**

      |-----------------------|
      |  Cookie List Header   |
      |  -------------------  |
      |   1st Cookie          |
      |  -------------------- |
      |   2nd Cookie          |
      |  -------------------  |
      |   3rd Cookie          |
      |  -------------------  |
      |   4th Cookie          |
      |  -------------------- |<-- empty_cookie pointer for insertion

Cookie List Header

      |-----------------------|
      |   EFS_COOKIE_VERSION  | 4 bytes.
      |-----------------------|
      |   EFS_COOKIE_MAGIC1   | 4 bytes.
      |-----------------------|
      |   EFS_COOKIE_MAGIC2   | 4 bytes.
      |-----------------------|

Cookie structure
      |-----------------------|
      |   fs_cookie_id        | 1 byte.
      |  -------------------  |
      |   payload size        | 2 bytes.
      |  -------------------  |
      |   payload data        | Variable size. (Can be NULL)
      |-----------------------|

****************************************************************************/

struct fs_cookie_list_manager;


/** Header preceding the cookie list. */
struct fs_cookie_list_hdr
{
  uint32 version; /**< Version of cookie layout and structure. Should be the
                       first field in structure to perform proper upgrades
                       when layout or structure of cookies change. */
  uint32 magic1;  /**< Helps detect accidental changes to cookie data on
                       partition. */
  uint32 magic2;
};

/** The cookies list layout in RAM. Cookies are stored in a list, starting with
    a header to help easily identify the beginning of the list, and a
    contiguous list of cookies afterwards which can grow as long as the cookie
    ram size. */
struct fs_cookie_list
{
  struct fs_cookie_list_hdr list_hdr;
  struct fs_cookie cookies[1];          /**< Growing list of cookies */
};


/** Cookie iterator to iterate through cookies in the list */
struct fs_cookie_iter
{
  uint8 in_use;
  struct fs_cookie_list_manager *cookie_list_manager; /**< Parent object whose
                          cookie list this iter is used to iterate through */
  struct fs_cookie *prev_cookie; /**< On a call to get next, fetch cookie
                          following this cookie. If NULL, return the first
                          cookie in list. */
};


/** Cookie list manager to manage the cookie list for supplied RAM. */
struct fs_cookie_list_manager
{
  int in_use;
  struct fs_cookie_list *cookie_list;
  uint8 *empty_cookie_pos;   /**< Next free cookie position in the list. */
  uint8 *list_end_addr; /**< End of the list. No more additions after this. */
  struct fs_cookie_iter cookie_iters[FS_COOKIE_MAX_ITERS];
};
struct fs_cookie_list_manager
      fs_cookie_list_manager_objs[FS_COOKIE_MAX_HANDLES];

/* Convenience macro to calculate payload offset. */
#define FS_COOKIE_PAYLOAD_OFFSET (offsetof (struct fs_cookie, payload))

/* Convenience macro to calculate cookie size. */
#define FS_COOKIE_SIZE(x) (FS_COOKIE_PAYLOAD_OFFSET + x->payload_size)

/* Initialize the EFS cookie module. Should be called only once, or will purge
   data in all cookie list manager instances. */
void fs_cookie_init (void)
{
  if (EFS_COOKIE_ID_MAX > 255)
  {
    FS_ERR_FATAL ("%d,cookie ID > 255 defined. Cookie id is a uint8. Increase \
                   size of cookie ID field\n", EFS_COOKIE_ID_MAX, 0, 0);
  }
  memset (fs_cookie_list_manager_objs, 0x0,
          sizeof (fs_cookie_list_manager_objs));
}

static int
fs_cookie_is_valid (struct fs_cookie *cookie, uint8 *cookie_pos,
                    struct fs_cookie_list_manager *cookie_list_manager)
{
  int cookie_valid = 0;

  ASSERT (cookie != NULL);
  ASSERT (cookie_list_manager != NULL);
  ASSERT (cookie_pos!= NULL);

  if (cookie->cookie_id > EFS_COOKIE_ID_INVALID &&
      cookie->cookie_id < EFS_COOKIE_ID_MAX)
  {
    uint32 cookie_size = FS_COOKIE_SIZE (cookie);
    if (cookie_pos + cookie_size <= cookie_list_manager->list_end_addr)
    {
      cookie_valid = 1;
    }
  }

  return cookie_valid;
}

static int
fs_cookie_list_init (struct fs_cookie_list_manager *cookie_list_manager)
{
  struct fs_cookie_list *cookie_list;
  uint32 list_size_bytes;

  if (cookie_list_manager == NULL)
  {
    return -1;
  }

  /* Based on whether the RAM has existing cookie list or not,
     initialize the empty_cookie pointer. */
  cookie_list = cookie_list_manager->cookie_list;
  ASSERT (cookie_list != NULL);

  list_size_bytes = cookie_list_manager->list_end_addr - (uint8 *)cookie_list;

  /* Initialize empty pos to start of the list. */
  cookie_list_manager->empty_cookie_pos = (uint8 *) cookie_list->cookies;

  if (cookie_list->list_hdr.version != EFS_COOKIE_VERSION ||
      cookie_list->list_hdr.magic1  != EFS_COOKIE_MAGIC1  ||
      cookie_list->list_hdr.magic2  != EFS_COOKIE_MAGIC2)/* No Cookies in RAM*/
  {
    /* Clear out rest of the memory. */
    memset (cookie_list, EFS_COOKIE_ID_INVALID, list_size_bytes);
    cookie_list->list_hdr.version = EFS_COOKIE_VERSION;
    cookie_list->list_hdr.magic1 = EFS_COOKIE_MAGIC1;
    cookie_list->list_hdr.magic2 = EFS_COOKIE_MAGIC2;
  }
  else /* Cookies already present in RAM, goto end of the cookies list. */
  {
    uint8 *next_cookie_pos;
    struct fs_cookie *cookie_ptr;
    uint32 free_ram_size;

    cookie_ptr = cookie_list->cookies;
    next_cookie_pos = (uint8 *)cookie_ptr;

    while (fs_cookie_is_valid (cookie_ptr, next_cookie_pos,
                               cookie_list_manager) == 1)
    {
      next_cookie_pos += FS_COOKIE_SIZE (cookie_ptr);
      cookie_ptr = (struct fs_cookie *) next_cookie_pos;
      cookie_list_manager->empty_cookie_pos = next_cookie_pos;
    }

    /* Empty position is now in a place where there are no more valid cookies.
       Clear out rest of the cookie RAM. */
    ASSERT (cookie_list_manager->empty_cookie_pos <=
            cookie_list_manager->list_end_addr);

    free_ram_size = cookie_list_manager->list_end_addr -
                    cookie_list_manager->empty_cookie_pos;
    memset (cookie_list_manager->empty_cookie_pos, EFS_COOKIE_ID_INVALID,
            free_ram_size);
  }

  return 0;
}


/* Initialize the RAM for cookie to work with. */
int
fs_cookie_get_handle_for_ram (void *ram_addr, uint32 ram_size)
{
  uint32 num_lists, i;
  int result = -1;
  struct fs_cookie_list_manager *cookie_list_manager;

  if (ram_addr == NULL || ram_size == 0 ||
      ram_size < sizeof (struct fs_cookie_list))
  {
    return -1;
  }

  num_lists = sizeof (fs_cookie_list_manager_objs) /
                      sizeof (fs_cookie_list_manager_objs[0]);

  for (i = 0; i < num_lists; i++)
  {
    cookie_list_manager = &fs_cookie_list_manager_objs[i];
    if (cookie_list_manager->in_use == 0)
    {
      cookie_list_manager->in_use = 1;
      break;
    }
  }

  if (i < num_lists)
  {
    cookie_list_manager->cookie_list = (struct fs_cookie_list *) ram_addr;
    cookie_list_manager->list_end_addr = ((uint8 *)ram_addr + ram_size);
    if (fs_cookie_list_init (cookie_list_manager) != 0)
    {
      result = -1;
    }
    else
    {
      result = i;
    }
  }

  return result;
}


/* Add new cookie data to cookie list pointed to by handle. */
int
fs_cookie_add (int handle, uint8 cookie_id, uint8 *payload,
               uint16 payload_size)
{
  struct fs_cookie_list_manager *cookie_list_manager;
  struct fs_cookie *cookie_ptr;
  uint32 cookie_size;

  if ((handle < 0) || (handle >= FS_COOKIE_MAX_HANDLES))
  {
    return -1;
  }

  if ((cookie_id == EFS_COOKIE_ID_INVALID) ||
      (cookie_id >= EFS_COOKIE_ID_MAX))
  {
    return -1;
  }

  if ((payload_size > 0) && (payload == NULL))
  {
    return -1;
  }

  cookie_list_manager = &fs_cookie_list_manager_objs[handle];
  ASSERT (cookie_list_manager->in_use == 1);

  cookie_size = FS_COOKIE_PAYLOAD_OFFSET + payload_size;
  if ((cookie_list_manager->empty_cookie_pos + cookie_size) >
       cookie_list_manager->list_end_addr)
  {
    return -1;
  }

  cookie_ptr = (struct fs_cookie*) cookie_list_manager->empty_cookie_pos;
  cookie_ptr->cookie_id = cookie_id;
  cookie_ptr->payload_size = payload_size;
  if (payload_size > 0)
  {
    fs_memscpy (cookie_ptr->payload, payload_size, payload, payload_size);
  }

  cookie_list_manager->empty_cookie_pos += cookie_size;

  return 0;
}


/* Open a iterator to iterate through the cookies in the cookie list */
struct fs_cookie_iter*
fs_cookie_iter_open (int handle)
{
  struct fs_cookie_list_manager *cookie_list_manager;
  struct fs_cookie_iter *iter = NULL;
  uint32 i;

  if ((handle < 0) || (handle >= FS_COOKIE_MAX_HANDLES))
  {
    return NULL;
  }

  cookie_list_manager = &fs_cookie_list_manager_objs[handle];
  ASSERT (cookie_list_manager->in_use == 1);

  for (i = 0; i < FS_COOKIE_MAX_ITERS; i++)
  {
    iter = &cookie_list_manager->cookie_iters[i];
    if (iter->in_use == 0)
    {
      iter->in_use = 1;
      iter->cookie_list_manager = cookie_list_manager;
      iter->prev_cookie = NULL;
      break;
    }
  }

  if (i == FS_COOKIE_MAX_ITERS)
  {
    iter = NULL;
  }

  return iter;
}


/* Return pointer to next cookie in the list if one present, NULL otherwise. */
struct fs_cookie*
fs_cookie_iter_get_next (struct fs_cookie_iter *iter)
{
  struct fs_cookie *cookie;
  uint8 *cookie_byte_addr;

  if (iter == NULL)
  {
    return NULL;
  }

  if (iter->in_use == 0)
  {
    return NULL;
  }

  /* If new iterator, assign to first cookie address */
  if (iter->prev_cookie == NULL)
  {
    cookie = iter->cookie_list_manager->cookie_list->cookies;
  }
  else /* Advance to next cookie position */
  {
    cookie_byte_addr = (uint8 *)iter->prev_cookie;
    cookie_byte_addr += FS_COOKIE_SIZE (iter->prev_cookie);
    cookie = (struct fs_cookie *)cookie_byte_addr;
  }

  /* If the new cookie position is beyond end if list, return NULL */
  if ((uint8 *)cookie >= iter->cookie_list_manager->empty_cookie_pos)
  {
    return NULL;
  }

  /* Update iter's position to next cookie position */
  iter->prev_cookie = cookie;

  return cookie;
}


/* Close the previously opened iterator. */
int
fs_cookie_iter_close (struct fs_cookie_iter* iter)
{
  if (iter == NULL)
  {
   return -1;
  }

  memset (iter, 0x0, sizeof (struct fs_cookie_iter));

  return 0;
}


/* Remove all cookies in the list and release all iter handles */
int
fs_cookie_remove_all_cookies (int handle)
{
  struct fs_cookie_list_manager *cookie_list_manager;
  uint32 cookie_list_size_bytes;
  uint8 *cookie_list_byte_addr;

  if ((handle < 0) || (handle >= FS_COOKIE_MAX_HANDLES))
  {
    return -1;
  }

  cookie_list_manager = &fs_cookie_list_manager_objs[handle];
  ASSERT (cookie_list_manager->in_use == 1);

  /* Remove only cookie data and retain the header. */
  cookie_list_byte_addr = (uint8 *)cookie_list_manager->cookie_list->cookies;
  cookie_list_size_bytes = cookie_list_manager->list_end_addr -
                           cookie_list_byte_addr;
  memset (cookie_list_byte_addr, EFS_COOKIE_ID_INVALID,cookie_list_size_bytes);
  cookie_list_manager->empty_cookie_pos = (uint8 *) cookie_list_byte_addr;

  /* Free all the iterators */
  memset (cookie_list_manager->cookie_iters, 0x0,
          sizeof (cookie_list_manager->cookie_iters));

  return 0;
}


/* Release handle obtained from cookie module to manage cookie list for RAM. */
int
fs_cookie_release_handle (int handle)
{
  struct fs_cookie_list_manager *cookie_list_manager;

  if (handle < 0)
  {
   return -1;
  }

  cookie_list_manager = &fs_cookie_list_manager_objs[handle];
  ASSERT (cookie_list_manager->in_use == 1);

  memset (cookie_list_manager, 0x0, sizeof (struct fs_cookie_list_manager));

  return 0;
}

