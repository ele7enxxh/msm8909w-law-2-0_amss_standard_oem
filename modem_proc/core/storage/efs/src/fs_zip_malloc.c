/***********************************************************************
 * fs_zip_malloc.c
 *
 * FS ZLIB Malloc Module.
 * Copyright (C) 2011-2012, 2014 QUALCOMM Technologies, Inc.
 *
 * This module provides API to manage dynamic memory allocation for ZLIB.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_zip_malloc.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-30   mj    Changed malloc functions to return 4-byte aligned address.
2012-05-24   nr    Fix bugs in fs_zip and fs_tar initialization.
2011-06-28   nr    Create

===========================================================================*/
#include "fs_zip_malloc.h"
#include "assert.h"

#include "fs_util.h"
#include "fs_err.h"

#ifdef FEATURE_EFS_ZIP

#define FS_MALLOC_ZIP_ALIGN 4

/* This library assume frees are called in reverse order of mallocs.
 * Pattern malloc(a) malloc(b) free(a) free(b) is not allowed. */


struct fs_zip_malloc_heap_state
{
  uint8* heap_start; /* Mark the start of the heap */
  uint32 heap_size; /* The size of the heap (must be non zero */
  uint8* heap_curr; /* Current pointer to return if there is enough memory */
  uint32 total_allocated; /* Total size  allocated */
  uint8 in_use;
};

static struct fs_zip_malloc_heap_state
fs_zip_malloc_struct =
{
  NULL, /* start of heap */
  0, /* Size of heap */
  NULL,/*current pointer */
  0,/* total allocated */
  0, /* not in use */
};

void
fs_zip_malloc_init (void)
{
  memset (&fs_zip_malloc_struct, 0, sizeof(fs_zip_malloc_struct));
}

fs_zip_malloc_handle
fs_zip_malloc_open (void *heap_start, uint32 heap_size)
{
  uint32 align;
  uint32 align_increase = 0;
  fs_zip_malloc_handle malloc_handle = &fs_zip_malloc_struct;

  if (malloc_handle->in_use != 0)
  {
    return NULL;
  }

  malloc_handle->in_use = 1;
  malloc_handle->heap_start = heap_start;

  align = FS_MALLOC_ZIP_ALIGN;

  if (align != 0 && ((uint32)heap_start & (align - 1)) != 0)
  {
    /* Round up the offset to the nearest alignment */
    heap_start = (void*)((uint8*)heap_start + (align - 1));
    heap_start = (void*)((uint32)heap_start & ~(align - 1));
    align_increase = (uint32)heap_start - (uint32)(malloc_handle->heap_start);
    malloc_handle->heap_start = heap_start;

  }

  malloc_handle->heap_curr = heap_start;
  malloc_handle->heap_size = heap_size - align_increase;

  if (malloc_handle->heap_size == 0)
  {
    /* Detect a zero size heap and fatal as this would be bad */
     FS_ERR_FATAL ("ZERO SIZE HEAP ALLOCATED \n", 0, 0, 0);
  }

  return malloc_handle;
}


void*
fs_zip_malloc_alloc (void *handle, unsigned items, unsigned size)
{
  void *address = NULL;
  fs_zip_malloc_handle malloc_handle = (fs_zip_malloc_handle) handle;
  uint32 align;
  uint32 align_increase = 0;
  uint32 alloc_request_size = items * size;

  ASSERT (malloc_handle->in_use == 1);

  /* The C standard for malloc is as follows :
   * If the size of the space requested is zero, the behavior is
   * implementation-defined: either a null pointer is returned, or the
   * behavior is as if the size were some nonzero value, except that the
   * returned pointer shall not be used to access an object.
   *
   * We choose to return NULL
   */
  if (alloc_request_size == 0)
  {
    return NULL;
  }

  align = FS_MALLOC_ZIP_ALIGN;
  address = (void*)malloc_handle->heap_curr;

  if (align != 0 && ((uint32)address & (align - 1)) != 0)
  {
    /* Round up the offset to the nearest alignment */
    address = (void*)((uint8*)address + (align - 1));
    address = (void*)((uint32)address & ~(align - 1));
    align_increase = (uint32)address - (uint32)(malloc_handle->heap_curr);

  }

  alloc_request_size += align_increase;
  malloc_handle->heap_curr += alloc_request_size;
  malloc_handle->total_allocated += alloc_request_size;

  if (malloc_handle->total_allocated > malloc_handle->heap_size)
  {
    /* The memory required exceeds the alllocated heap give more heap */
    FS_ERR_FATAL ("[%d > %d] OUT OF MEMORY IN HEAP",
          malloc_handle->total_allocated, malloc_handle->heap_size, 0);
  }

  ASSERT(!((uint32)address & (align - 1)));

  return (void*) address;
}

void fs_zip_malloc_free (void *handle, void *addr)
{
  fs_zip_malloc_handle malloc_handle = (fs_zip_malloc_handle) handle;
  uint32 freed_space = 0;

  ASSERT (malloc_handle->in_use == 1);
  if((malloc_handle->heap_curr <= (uint8*) addr) ||
          (malloc_handle->heap_start > (uint8*) addr))
  {
    /* The free was performed in the wrong order i.e. not as follows
     * as A = malloc() B = malloc() free (B) free(A)   */
    FS_ERR_FATAL ("%x address freed :ILLEGAL MEMORY ACCESS",
                  (uint32) addr, 0, 0);
  }
  freed_space = (uint32)(malloc_handle->heap_curr - (uint8*) addr);
  malloc_handle->heap_curr = addr;

  malloc_handle->total_allocated -= freed_space;
}

void
fs_zip_malloc_close(fs_zip_malloc_handle malloc_handle)
{

  ASSERT (malloc_handle->in_use == 1);
  if((malloc_handle->heap_curr != malloc_handle->heap_start))
  {
    /* Somer of the heap is not yet freed close called too early or
     * free not called correctly */
    FS_ERR_FATAL ("[%x != %x] %d bytes not freed: Memory Leak Detected \n",
                (uint32) malloc_handle->heap_curr,
                (uint32) malloc_handle->heap_start,
                malloc_handle->total_allocated);
  }
  malloc_handle->in_use = 0;
  malloc_handle->heap_start = NULL;
  malloc_handle->heap_curr = NULL;
  malloc_handle->heap_size = 0;
}

#endif
