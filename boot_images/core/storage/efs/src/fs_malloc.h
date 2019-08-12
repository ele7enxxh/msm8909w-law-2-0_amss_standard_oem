/***********************************************************************
 * fs_malloc.h
 *
 * Memory allocation routines.
 * Copyright (C) 2010-2011 Qualcomm Technologies, Inc.
 *
 * EFS has some memory space reserved for many of its internal variables
 * and buffers. In this module we implement memory allocation of the
 * reserved space.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_malloc.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-07-22   vm    Eliminate the compiler warnings in AMSS
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-07-19   wek   Create.

===========================================================================*/

#ifndef __FS_MALLOC_H__
#define __FS_MALLOC_H__

#include "fs_sys_types.h"

/** Initialize Memory allocation library */
void fs_malloc_init (void);

/** Disallow all future mallocs, called at the end of initialization */
void fs_disable_malloc (void);

/** Allocate memory aligned for Data Mover.
 *
 * Safe version of malloc, it will never return a NULL pointer, it will
 * ERR_FATAL instead.
 *
 * @param size number of bytes to allocate.
 *
 * @return pointer to allocated memory.
 *
 * @see fs_unsafe_aligned_malloc, fs_malloc
 */
void *fs_align_malloc (fs_size_t size);

/** Allocate memory aligned for Data Mover.
 *
 * @param size number of bytes to allocate.
 *
 * @return Pointer to allocate memory. When no space is available for given
 *         size returns NULL.
 *
 * @see fs_align_malloc, fs_malloc
 */
void *fs_unsafe_align_malloc (fs_size_t size);

/** Allocate memory safely.
 *
 * Allocates memory with no specific alignment. It will ERR_FATAL when it
 * can't allocate memory.
 *
 * @param size number of bytes to allocate.
 *
 * @return pointer to allocated memory.
 *
 * @see fs_align_malloc, fs_unsafe_aligned_malloc
 */
void *fs_malloc (fs_size_t size);

#endif /* not __FS_MALLOC_H__ */
