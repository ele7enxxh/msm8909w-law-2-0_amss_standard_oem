/***********************************************************************
 * fs_malloc.c
 *
 * Memory allocation routines.
 * Copyright (C) 2010-2012 Qualcomm Technologies, Inc.
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_malloc.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-05-02   rp    Fix malloc to return NULL for max uint32 values.
2011-11-15   nr    Fix Klocwork errors seen in MPSS.1.1
2011-09-15   wek   Fix copyright year.
2011-09-07   rp    Fix compiler warnings seen in 8960 target build.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-07-19   wek   Create.

===========================================================================*/

#include "assert.h"
#include "fs_err.h"
#include "fs_heap.h"
#include "fs_sys_types.h"
#include "fs_protected_memory.h"


/* This structure holds the state of the buffer used for memory allocation. */
struct fs_malloc_buf_info
{
  uint8 allow_malloc;  /* Bool value, 1 allow malloc, 0 ERR_FATAL on malloc */
  fs_size_t offset; /* Indicates next offset to allocate. Same as space used */
  fs_size_t size;   /* Total size of buffer. */
  uint8 *buffer;    /* Memory address of buffer, constant. */
};

static FS_ALIGN uint8 efs_memory_buffer[FS_MALLOC_BUFFER_SIZE];
static struct fs_malloc_buf_info fs_malloc_info_inst = {0, 0, 0, NULL};

void
fs_malloc_init (void)
{
  fs_malloc_info_inst.allow_malloc = 1;
  fs_malloc_info_inst.offset = 0;
  fs_malloc_info_inst.size = sizeof (efs_memory_buffer);
  fs_malloc_info_inst.buffer = efs_memory_buffer;
}

void
fs_disable_malloc (void)
{
  fs_malloc_info_inst.allow_malloc = 0;
}

/** Basic function to allocate data.
 *
 * The original buffer is align to FS_ALIGN, that is FS_DM_ALIGN_BYTES
 * alignment. Therefore we cannot guarantee alignment above DM_ALIGNMENT.
 * Also, to make the algorithm easier we only align to power of 2.
 *
 * @param size  number of bytes to allocate.
 * @param align Desired alignment. For no alignment use align = 0.
 *
 * @return Pointer to allocate memory. When no space is available for given
 *         size returns NULL.
 */
static void*
fs_malloc_helper (fs_size_t size, fs_size_t align)
{
  struct fs_malloc_buf_info *buf_info = &fs_malloc_info_inst;
  fs_size_t offset;
  void *pointer = NULL;
  fs_size_t avail_space;

  if (buf_info->allow_malloc == 0)
  {
    FS_ERR_FATAL ("Tried to allocate memory after it was prohibited",0,0,0);
  }

  /* Make sure align is power of 2. */
  ASSERT ((align & (align - 1)) == 0);

  offset = buf_info->offset;
  if (align != 0 && (offset & (align - 1)) != 0)
  {
    /* Round up the offset to the nearest alignment */
    offset += (align - 1);
    offset &= ~(align - 1);
  }

  if (offset < buf_info->size)
  {
    avail_space = buf_info->size - offset;
    if (avail_space >= size)
    {
      pointer = buf_info->buffer + offset;
      buf_info->offset = offset + size;
    }
  }

  return pointer;
}

void*
fs_align_malloc (fs_size_t size)
{
  void *pointer;
  pointer = fs_malloc_helper (size, FS_DM_ALIGN_BYTES);
  if (pointer == NULL)
  {
    FS_ERR_FATAL ("Ran out of space to allocate",0,0,0);
  }
  return pointer;
}

void*
fs_unsafe_align_malloc (fs_size_t size)
{
  return fs_malloc_helper (size, FS_DM_ALIGN_BYTES);
}

void*
fs_malloc (fs_size_t size)
{
  void *pointer;
  pointer = fs_malloc_helper (size, 0);
  if (pointer == NULL)
  {
    FS_ERR_FATAL ("Ran out of space to allocate",0,0,0);
  }
  return pointer;
}

