/***********************************************************************
 * fs_zip_tar.c
 *
 * FS ZIPPED TAR MODULE
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This provides api to create zipped tar files.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/src/fs_zip_tar.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-10-24   nr    Minor Cleanup and Fixes.
2011-09-21   nr    Create

===========================================================================*/
#include "fs_zip_tar_i.h"

#ifdef FEATURE_FS_ZIP_TAR

#include "fs_zip.h"
#include "fs_tar_i.h"
#include "fs_priv_funcs.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_public.h"

enum fs_zip_tar_state
{
  FS_ZIP_TAR_STATE_UNUSED, /* Indicates that handle is currently not in use */
  FS_ZIP_TAR_STATE_NEED_INPUT, /* Indicates that input needs to be set */
  FS_ZIP_TAR_STATE_READ, /* Read and populate the output buffer */
  FS_ZIP_TAR_STATE_FINISHED, /* End of image creation */
  FS_ZIP_TAR_CLOSE, /* Close the handle */
};

struct fs_zip_tar_handle
{
  enum fs_zip_tar_state state;
  void *zip_handle;
  void *tar_handle;
  void *data_buffer;
  uint32 data_buffer_size;
  enum fs_zip_return_status error_code;
};
typedef struct fs_zip_tar_handle* fs_zip_tar_handle;

#define FS_ZIP_TAR_MAX_HANDLE_COUNT 1 /* Only one concurrent handle */
static
struct fs_zip_tar_handle fs_zip_tar_handle_struct[FS_ZIP_TAR_MAX_HANDLE_COUNT];


void fs_zip_tar_init (void)
{
  int i;
  for (i = 0; i < FS_ZIP_TAR_MAX_HANDLE_COUNT; i++)
  {
    fs_zip_tar_handle_struct[i].state = FS_ZIP_TAR_STATE_UNUSED;
  }
}

void *
fs_zip_tar_open (const char *path)
{
  fs_zip_tar_handle handle = NULL;
  uint8 *zip_heap_start = NULL;
  uint32 zip_heap_size = 0;
  enum fs_zip_return_status status;
  enum fs_zip_mode mode = FS_ZIP_MODE_COMPRESSION;
  int i,result;
  for (i = 0; i < FS_ZIP_TAR_MAX_HANDLE_COUNT; i++)
  {
    if (fs_zip_tar_handle_struct[i].state == FS_ZIP_TAR_STATE_UNUSED)
    {
      handle = &fs_zip_tar_handle_struct[i];
      break;
    }
  }
  if (handle == NULL)
  {
    efs_errno = EBUSY;
    return NULL;
  }

  result  = efs_private_borrow_ram ((void**)&zip_heap_start, &zip_heap_size);
  if ((result != 0) || (zip_heap_size < FS_ZIP_TAR_HEAP_SIZE))
  {
    efs_errno = ENOMEM;
    return NULL;
  }
  ASSERT (zip_heap_start != NULL);

  handle->data_buffer_size = FS_TAR_HDR_SIZE;
  handle->data_buffer = (void*) (zip_heap_start);
  zip_heap_start += handle->data_buffer_size;
  zip_heap_size -= handle->data_buffer_size;
  handle->zip_handle = fs_zip_open (mode, zip_heap_start, zip_heap_size);
  if (handle->zip_handle == NULL)
  {
    handle->data_buffer_size = 0;
    handle->data_buffer = NULL;
    efs_errno = EBUSY;
    return NULL;
  }
  handle->tar_handle = fs_tar_create_open (path);
  if (handle->tar_handle == NULL)
  {
    status = fs_zip_close(handle->zip_handle);
    ASSERT (status == FS_ZIP_STATUS_SUCCESS);
    handle->data_buffer_size = 0;
    handle->data_buffer = NULL;
    return NULL;
  }
  handle->state = FS_ZIP_TAR_STATE_NEED_INPUT;
  return handle;
}

int32
fs_zip_tar_create_read (void *handle_ptr, uint8 *output,
                                uint32 out_buff_size)
{
  int input_size = 0;
  fs_zip_tar_handle handle = (fs_zip_tar_handle) handle_ptr;
  enum fs_zip_return_status status = FS_ZIP_STATUS_SUCCESS;
  uint32 output_available = 0;
  uint32 total_output = 0;
  uint32 expected_out = out_buff_size;
  uint32 input_buf_size;
  void *tar_handle, *zip_handle, *input_buff;

  if ((handle == NULL) ||(out_buff_size == 0) || (output == NULL))
  {
    efs_errno = EINVAL;
    return -1;
  }

  input_buff = handle->data_buffer;
  input_buf_size = handle->data_buffer_size;
  tar_handle = handle->tar_handle;
  zip_handle = handle->zip_handle;

  if (handle->state == FS_ZIP_TAR_STATE_NEED_INPUT)
  {
    input_size = fs_tar_create_read (tar_handle, input_buff, input_buf_size);
    status = fs_zip_set_input (zip_handle, input_buff, input_size);
    handle->state = FS_ZIP_TAR_STATE_READ;
  }
  if (handle->state == FS_ZIP_TAR_STATE_FINISHED)
  {
    handle->state = FS_ZIP_TAR_CLOSE;
    return 0;
  }

  if (handle->state != FS_ZIP_TAR_STATE_READ)
  {
    efs_errno = ESTALE;
    return -1;
  }

  while (status == FS_ZIP_STATUS_SUCCESS)
  {
    status = fs_zip_compress (zip_handle, output, expected_out,
                                &output_available);
    if (status == FS_ZIP_STATUS_SUCCESS)
    {
      total_output +=  output_available;
      if(output_available < expected_out)
      {
        expected_out -= output_available;
        output += output_available;
        continue;
      }
      break;
    }
    else if (status == FS_ZIP_STATUS_NEED_MORE_INPUT)
    {
      input_size = fs_tar_create_read (tar_handle, input_buff, input_buf_size);
      status = fs_zip_set_input (zip_handle, input_buff, input_size);
    }
  }

  if(status == FS_ZIP_STATUS_FINISHED)
  {
    handle->state = FS_ZIP_TAR_STATE_FINISHED;
  }
  else if(status != FS_ZIP_STATUS_SUCCESS)
  {
    handle->error_code = status;
    efs_errno = EEOF;
    return -1;
  }
  return total_output;

}

int
fs_zip_tar_close (void *handle_ptr)
{
  int result;
  fs_zip_tar_handle handle = (fs_zip_tar_handle) handle_ptr;
  enum fs_zip_return_status status;

  if (handle == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }
  if (handle->state == FS_ZIP_TAR_STATE_UNUSED)
  {
    efs_errno = ESTALE;
    return -1;
  }

  handle->state = FS_ZIP_TAR_STATE_UNUSED;
  status = fs_zip_close (handle->zip_handle);
  result = fs_tar_create_close (handle->tar_handle);
  ASSERT (status == FS_ZIP_STATUS_SUCCESS);
  return result;
}

#endif /* FEATURE_FS_ZIP_TAR */
