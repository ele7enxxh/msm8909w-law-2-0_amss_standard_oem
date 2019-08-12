/***********************************************************************
 * fs_zip.c
 *
 * FS ZIP Module.
 * Copyright (C) 2011-2013, 2015 QUALCOMM Technologies, Inc.
 *
 * This module provides API to compress and decompress byte streams or
 * files.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_zip.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-07-03   vm    Remove extra parenthesis to fix compiler warning.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-02-28   nr    Update to the new zlib version (1.2.7) from services team.
2012-05-24   nr    Fix bugs in fs_zip and fs_tar initialization.
2011-11-02   nr    Klocwork error fixes in Modem code.
2011-10-24   nr    Minor Cleanup and Fixes.
2011-06-23   nr    Create

===========================================================================*/

#include "fs_zip.h"

#ifdef FEATURE_EFS_ZIP

#include <stdio.h>
#include "fs_zip_malloc.h"
#include "zlib.h"

#include "assert.h"
#include "fs_err.h"
#include "fs_util.h"


/*****************************************************************************
 * STATE TRANSITION DIAGRAM FOR FS_ZIP_MODULE.
******************************************************************************

                            +----------------+
                            |  FS_ZIP_UNUSED |
                            +-------+--------+
                                    |
                                    |
  +------------------>--------------+
  |                                 |
  |                                 |
  |                                 v
  |                     +--------------------------+
  |                     | FS_ZIP_WAITING_FOR_INPUT |
  |                     +-----------+--------------+
  |                                 |
  |   +------------------------<----+---->-----------------------------+
  |   |                             |                                  |
  |   |                             |                                  |
  |   |                             v                                  |
  |   |                  +-----------------------+                     |
  |   v                  | FS_ZIP_PROCESS_HEADER |                     v
  |   |                  +----------+------------+                     |
  |   |                             |                                  |
  ^   |                             |                                  |
  |   |                    +--------+------------------+---------------|---+
  |   |                    |                           |               |   |
  |   |                    |                           v               |   |
  |   +------------------->+            +----------------------------+ |   |
  |   |                    |            | FS_ZIP_HEADER_CHECK_FAILED | |   |
  |   |                    |            +----------------------------+ |   v
  |   |                    |                                           |   |
  |   v                    v                                           |   |
  |   |     +-------------------------------+                          |   |
  +-<-------+ FS_ZIP_COMPRESS_CONSUME_INPUT |                          |   |
  |   |     +-------------------------------+                          |   |
  |   |                                                                |   |
  |   +-----------+                                                    |   |
  |               |             +-----------<--------+------------<----+   |
  ^               |             |                    |                     |
  |               v             |                    +------------<--------+
  |  +-----------------------+  |                    |
  |  | FS_ZIP_COMPRESS_FLUSH |  |                    |
  |  +-----------------------+  |                    |
  |                             |                    v
  |                             | +------------------------------------------+
  +----------<--------------------+ FS_ZIP_COMPRESS_DECOMPRESS_CONSUME_INPUT |
  |                             | +------------------------------------------+
  |                             |
  ^                             |
  |                             v
  |                  +------------------------+
  +------<-----------+ FS_ZIP_PROCESS_TRAILER |
                     +----------+-------------+
                                |
                                |
                                v
           +--------------------+------------+
           |                                 |
           v                                 |
 +-------------------+                       |
 | FS_ZIP_CRC_FAILED |                       v
 +-------------------+               +-------------+
                                     | FS_ZIP_DONE |
                                     +-------------+

*****************************************************************************/



/************************************************************************
 *
 *  The GZIP HEADER IS DECRIBED IN THE GZIP FILE FORMAT RFC
 *
 ************************************************************************/
/* gzip magic header */
#define FS_GZ_MAGIC_HEADER_1 0x1f /* Magic gzip buferr part 1. */
#define FS_GZ_MAGIC_HEADER_2 0x8b /* Magic gzip header part 2. */

/* gzip flag byte */
#define FS_GZ_ASCII_FLAG_MASK   0x01 /* bit 0 set: file probably ascii text */
#define FS_GZ_HEAD_CRC_MASK     0x02 /* bit 1 set: header CRC present */
#define FS_GZ_EXTRA_FIELD_MASK  0x04 /* bit 2 set: extra field present */
#define FS_GZ_ORIG_FNAME_MASK   0x08 /* bit 3 set: original file name
                                        present */
#define FS_GZ_HEAD_COMMENT_MASK 0x10 /* bit 4 set: file comment present */
#define FS_GZ_RESERVED_MASK     0xE0 /* bits 5..7: reserved */

#ifndef FS_GZ_HEADER_OS_CODE
  #define FS_GZ_HEADER_OS_CODE  0xff  /* Unknown OS */
#endif

#define FS_ZIP_GZIP_TRAILER_SIZE 8 /*uint32 crc and uint32 input file length */
/************************************************************************
 *
 *  The GZIP HEADER IS DECRIBED IN THE GZIP FILE FORMAT RFC
 *
 ************************************************************************/
PACKED struct PACKED_POST fs_zip_header{
  uint8 magic_header1; /* The first byte of magic header */
  uint8 magic_header2; /* the second byte of the magic header */
  uint8 compression_method; /* the method of compression  */
  uint8 flags; /* gzip flags (bit wise) */
  uint32 time_stamp; /* time stamp in unix/posix format */
  uint8 xflags; /* flags for use by specific compression methods */
  uint8 os_code; /* The os code system on which compression took place */
}; /* Fordetailed explanation please look at the header file */

#define FS_ZIP_MINIMUM_GZIP_HEADER_SIZE (sizeof (struct fs_zip_header))

enum fs_zip_state
{
  FS_ZIP_UNUSED, /* Indicates the handle is not currently in used. */
  FS_ZIP_PROCESS_HEADER, /* To process the header. */
  FS_ZIP_WAITING_FOR_INPUT, /* Input buffer is empty. */
  FS_ZIP_COMPRESS_CONSUME_INPUT, /* Consuming input buffer and Compress. */
  FS_ZIP_DECOMPRESS_CONSUME_INPUT, /* Consuming input buffer and Decompres. */
  FS_ZIP_COMPRESS_FLUSH, /* End of input so flushing to finish compression. */
  FS_ZIP_PROCESS_TRAILER, /* Produce/check  the crc and other end of file
                           operations. */
  FS_ZIP_DONE, /* File successfully processed. */
  FS_ZIP_HEADER_CHECK_FAILED, /* Header check failed */
  FS_ZIP_CRC_FAILED ,/* Crc check failed */
};


enum fs_zip_header_state
{
  FS_ZIP_HEADER_CHECK_PASSED = 0, /* The header sucessfully checked */
  FS_ZIP_HEADER_BEGIN = -1, /* The start condition :Begin the header check */
  FS_ZIP_BAD_HEADER= -2 /* The check failed */
};

enum fs_zip_file_errors
{
  FS_ZIP_FILE_IS_FINE = 0,
  FS_ZIP_FILE_END_OF_FILE = 1,
  FS_ZIP_FILE_HEADER_SPLIT_ACROSS_FRAMES = 2,
};

/* Structure for zlib operations
 * Important members for ZLIB.  */
struct fs_zlib_stream
{
  z_stream stream; /* Handle returned by zlib. */
  int z_err;   /* error code for last stream operation */
  enum fs_zip_file_errors  zip_file_errors;   /* File parsing errors */
  uLong generated_crc;     /* crc32 of uncompressed data */
  z_off_t total_input_consumed;      /* bytes into deflate or inflate */
  z_off_t total_output_produced;     /* bytes out of deflate or inflate */
};

struct fs_zip_handle_structure
{
  enum fs_zip_state state; /* The state of the compression/decompression*/
  enum fs_zip_return_status message; /* return messages to the client */
  struct fs_zlib_stream fs_zlib_stream; /* Important members for ZLIB.  */
  uint8 *input_buffer; /* passed by user in the set_input function  */
  uint32 input_buffer_size; /* size of input buffer passed by user */
  uint8 *output_buffer; /* Set in the compress or decompress functions */
  uint32 output_buff_size; /* size of output buffer */
  enum fs_zip_mode mode; /* mode of the zip ie compress or decompress*/
  int32 input_not_consumed; /* input still remaining */
  uint32 header_length; /* length of the header */
  enum fs_zip_header_state header_state; /* state of header processing */
  uint32 file_crc; /* CRC of the file */
  uint32 trailer_bytes_to_read; /* length of crc and or file_length left */
  uint8 gzip_trailer[FS_ZIP_GZIP_TRAILER_SIZE]; /* generated trailer  */
  fs_zip_malloc_handle malloc_handle; /* Handle from zip_malloc to zlib */
}; /* holds the data for processing */


#define FS_ZIP_MAX_HANDLE_COUNT 1 /* Only one concurrent handle */
static struct fs_zip_handle_structure
              fs_gzhandle_struct[FS_ZIP_MAX_HANDLE_COUNT];

/* The simple gzip header used for gzip creation */
static const struct fs_zip_header simple_gzip_header =
{
  FS_GZ_MAGIC_HEADER_1, /* Magic header 1. */
  FS_GZ_MAGIC_HEADER_2, /* Magic header 2. */
  Z_DEFLATED, /* Z_DEFLATED = 8 ; 1 to 7 are reserved. */
  0, /* No Flags */
  0, /* If necessary populate time stamp later. */
  0, /* Don't need xflags */
  FS_GZ_HEADER_OS_CODE, /* The OS code = 0xFF i.e unknown OS. */
};

/*
 * Function to ensure correct state transitions.
 * We assert on invalid state transitions.
 */
static void
fs_zip_set_state (fs_zip_handle handle, enum fs_zip_state new_state)
{
  switch (new_state)
  {
    case FS_ZIP_UNUSED :
    {
      ASSERT (handle->state != FS_ZIP_UNUSED);
      break;
    }

    case FS_ZIP_PROCESS_HEADER :
    {
      ASSERT (handle->state == FS_ZIP_WAITING_FOR_INPUT);
      break;
    }

    case FS_ZIP_WAITING_FOR_INPUT :
    {
      ASSERT ((handle->state == FS_ZIP_UNUSED) ||
              (handle->state == FS_ZIP_COMPRESS_CONSUME_INPUT) ||
              (handle->state == FS_ZIP_DECOMPRESS_CONSUME_INPUT) ||
              (handle->state == FS_ZIP_PROCESS_TRAILER));
      break;
    }

    case FS_ZIP_COMPRESS_CONSUME_INPUT :
    {
      ASSERT ((handle->state == FS_ZIP_WAITING_FOR_INPUT) ||
              (handle->state == FS_ZIP_PROCESS_HEADER));
      break;
    }

    case FS_ZIP_DECOMPRESS_CONSUME_INPUT :
    {
      ASSERT ((handle->state == FS_ZIP_WAITING_FOR_INPUT) ||
              (handle->state == FS_ZIP_PROCESS_HEADER));
      break;
    }

    case FS_ZIP_COMPRESS_FLUSH :
    {
      ASSERT (handle->state == FS_ZIP_WAITING_FOR_INPUT);
      break;
    }

    case FS_ZIP_PROCESS_TRAILER :
    {
      ASSERT ((handle->state == FS_ZIP_COMPRESS_FLUSH) ||
              (handle->state == FS_ZIP_DECOMPRESS_CONSUME_INPUT)||
              (handle->state == FS_ZIP_WAITING_FOR_INPUT));
      break;
    }

    case FS_ZIP_DONE :
    {
      ASSERT (handle->state == FS_ZIP_PROCESS_TRAILER);
      break;
    }

    case FS_ZIP_HEADER_CHECK_FAILED :
    {
      ASSERT (handle->state == FS_ZIP_PROCESS_HEADER);
      break;
    }

    case FS_ZIP_CRC_FAILED :
    {
      ASSERT (handle->state == FS_ZIP_PROCESS_TRAILER);
      break;
    }

    default :
    {
      FS_ERR_FATAL ("[%d<-%d] Invalid State", new_state, handle->state, 0);
    }
  }
  handle->state = new_state;
}


/* Cleanup then free the given fs_zlib_stream. Return a zlib error code.
 * Try freeing in the reverse order of allocations.
 */
static int
fs_zip_destroy (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  int err = Z_OK;

  if (s->stream.state != NULL)
  {
    if (handle->mode == FS_ZIP_MODE_COMPRESSION)
    {
      err = deflateEnd (&(s->stream));
    }
    else if (handle->mode == FS_ZIP_MODE_DECOMPRESSION)
    {
      err = inflateEnd (&(s->stream));
    }
  }
  if(s->z_err < 0)
  {
    err = s->z_err;
  }
  handle->input_buffer = NULL;
  handle->output_buffer = NULL;
  return err;
}

/* Function to initialize the stream mode is FS_ZIP_MODE_COMPRESSION or
 * FS_ZIP_MODE_DECOMPRESSION for deflate and inflate
 * respectively.
 */
static int
fs_zip_open_zlib_stream (fs_zip_handle handle, int level)
{
  int err;
  int compression_level = Z_DEFAULT_COMPRESSION; /* Assume default compression
                                                    level */
  int strategy = Z_DEFAULT_STRATEGY; /* set compression strategy to DEFLATE */
  struct fs_zlib_stream *s =  &handle->fs_zlib_stream;

  s->stream.zalloc = (alloc_func) (fs_zip_malloc_alloc);
  s->stream.zfree = (free_func)(fs_zip_malloc_free);
  s->stream.opaque = (voidpf) (handle->malloc_handle);
  s->stream.next_in = Z_NULL;
  s->stream.next_out = Z_NULL;
  s->stream.avail_in = s->stream.avail_out = 0;
  s->z_err = Z_OK;
  s->zip_file_errors = FS_ZIP_FILE_IS_FINE;
  s->total_input_consumed = 0;
  s->total_output_produced = 0;
  s->generated_crc = crc32(0L, Z_NULL, 0);
  if ((level >= 0) && (level <= 9) )
  {
    compression_level = level;
  }
  if (handle->mode != FS_ZIP_MODE_COMPRESSION &&
      handle->mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return -1;
  }
  if (handle->mode == FS_ZIP_MODE_COMPRESSION)
  {
    err = deflateInit2(&(s->stream), compression_level,
                Z_DEFLATED, FS_ZIP_WINDOW_SIZE, FS_ZIP_MEM_LEVEL, strategy);
        /* windowBits is passed < 0 to suppress zlib header and trailer */
   if (err != Z_OK )
    {
      return -1;
    }
  }
  else /* if Decompression */
  {
    /* windowBits is passed < 0 to tell that there is no zlib header.
     * Note that in this case inflate *requires* an extra "dummy" byte
     * after the compressed stream in order to complete decompression and
     * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
     * present after the compressed stream.
     */
    err = inflateInit2(&(s->stream), -MAX_WBITS);

    if (err != Z_OK)
    {
      return -1;
    }
  }
  return 0;
}

/* Writes a simple gz header to the output buffer and
 * returns the header size */
static int
fs_zip_get_gzip_header (fs_zip_handle handle)
{
  struct fs_zip_header *new_header = (struct fs_zip_header*)
                                            (handle->output_buffer);

  if (handle->mode == FS_ZIP_MODE_COMPRESSION)
  {
    /* Write a very simple .gz header  */
    fs_memscpy (new_header, handle->output_buff_size, &simple_gzip_header,
                sizeof (struct fs_zip_header));
    new_header->time_stamp = fs_time ();
  }
  else
  {
    return FS_ZIP_STATUS_MODE_ERROR;
  }
  fs_zip_set_state (handle, FS_ZIP_COMPRESS_CONSUME_INPUT);
  handle->message = FS_ZIP_STATUS_SUCCESS;
  handle->header_state = FS_ZIP_HEADER_CHECK_PASSED;
  return (sizeof (struct fs_zip_header));
}

/* Compute crc should be called after all input buffer is consumed before
 * giving any new input.
 */
static int
fs_zip_compute_crc (voidpc input, struct fs_zlib_stream *s, uint32 length)
{
  s->generated_crc = crc32(s->generated_crc, (const Bytef *)input, length);

  return Z_OK;
}

/* Should be called to set every input but only after all previos input is
 * consumed.
 */
static int
fs_zip_deflate_set_input (fs_zip_handle handle)
{
  voidpc input = handle->input_buffer;
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 length = handle->input_buffer_size;

  if (handle->mode != FS_ZIP_MODE_COMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  s->stream.next_in = (Bytef*)input;
  s->stream.avail_in = length;
  fs_zip_compute_crc (input, s, length);
  return Z_OK;
}

static int
fs_zip_inflate_set_input (fs_zip_handle handle)
{
  voidpc input = handle->input_buffer;
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 length = handle->input_buffer_size;

  if (handle->mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  s->stream.next_in = (Bytef*)input;
  s->stream.avail_in = length;
  return Z_OK;
}

/* Writes data to compress stream and returns size of input not yet consumed
 * -1 as return indicates error.
 */
static int32
fs_zip_deflate_consume_input (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 available_input, available_output;

  if (handle->mode != FS_ZIP_MODE_COMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  if (s->stream.avail_in != 0)
  {
    available_input = s->stream.avail_in;
    available_output = s->stream.avail_out;

    s->z_err = deflate (&(s->stream), Z_NO_FLUSH);

    s->total_input_consumed += available_input - s->stream.avail_in;
    s->total_output_produced += available_output - s->stream.avail_out;
    if (s->z_err != Z_OK)
    {
      return Z_STREAM_ERROR;
    }
  }
  return (int32)(s->stream.avail_in);
}

/* Writes data to compress stream and returns size of input not yet consumed
 * -1 as return indicates error.
 */
static int32
fs_zip_inflate_consume_input (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 available_input, available_output;

  if (handle->mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  if (s->stream.avail_in != 0)
  {
    available_input = s->stream.avail_in;
    available_output = s->stream.avail_out;

    s->z_err = inflate (&(s->stream), Z_NO_FLUSH);

    s->total_input_consumed += available_input - s->stream.avail_in;
    s->total_output_produced += available_output - s->stream.avail_out;
    if ((s->z_err != Z_OK) && (s->z_err != Z_STREAM_END ))
    {
      return Z_STREAM_ERROR;
    }
  }
  return (int32)(s->stream.avail_in);
}


/* Returns length of output available. */
static int32
fs_zip_deflate_get_output_length (fs_zip_handle handle)
{
  uint32 written_length = 0;
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;

  if (handle->mode != FS_ZIP_MODE_COMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  written_length = handle->output_buff_size - s->stream.avail_out;
  if (written_length>0)
  {
    s->stream.next_out = (Byte*) handle->output_buffer;
    s->stream.avail_out = handle->output_buff_size;
  }
  return written_length;
}

/* Returns length of output available. */
static int32
fs_zip_inflate_write_output (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 written_length = 0;

  if (handle->mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  written_length = handle->output_buff_size - s->stream.avail_out;
  if (written_length > 0)
  {
    s->stream.next_out = (Byte*) handle->output_buffer;
    s->stream.avail_out = handle->output_buff_size;
    fs_zip_compute_crc (handle->output_buffer, s, written_length);
  }

  return written_length;
}

static int
fs_zip_inflate_is_done (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  int ret_val = 0;

  if (s->z_err == Z_STREAM_END)
  {
    ret_val = 1;
  }
  return ret_val;
}

/* Call flush in loop after all input is read returns true when the last output
 * is avaialble. Call write one last time after true.
 */
static int
fs_zip_flush (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  int done;
  int len = 0;
  int flush = Z_FINISH;

  if (handle->mode != FS_ZIP_MODE_COMPRESSION)
  {
    return Z_STREAM_ERROR;
  }
  s->stream.avail_in = 0; /* should be zero already anyway */
  len = handle->output_buff_size - s->stream.avail_out;
  s->total_output_produced += s->stream.avail_out;
  s->z_err = deflate (&(s->stream), flush);
  s->total_output_produced -= s->stream.avail_out;
  /* Ignore the second of two consecutive flushes
   * in case no more room in output */
  if (len == 0 && s->z_err == Z_BUF_ERROR)
  {
    s->z_err = Z_OK;
  }

  /* deflate has finished flushing only when it hasn't used up
   * all the available space in the output buffer:
   */
  done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);
  return done;
}

/* Closes the compression and returns the output buff with the required size of
 * last buffer to be written.
 */
static void
fs_zip_generate_trailer (fs_zip_handle handle)
{
  uint32 *trailer = (uint32*) handle->gzip_trailer;
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;

  *trailer = s->generated_crc;
  trailer++;
  *trailer = s->total_input_consumed;
  handle->trailer_bytes_to_read =  sizeof (handle->gzip_trailer);
}

static uint32
fs_zip_write_trailer (fs_zip_handle handle)
{
  uint8 *output = handle->output_buffer;
  uint8 *trailer = handle->gzip_trailer;
  uint32 size = handle->output_buff_size;

  if (handle->trailer_bytes_to_read == 0)
  {
    fs_zip_generate_trailer (handle);
  }
  if (size > handle->trailer_bytes_to_read)
  {
    size = handle->trailer_bytes_to_read;
  }
  trailer += sizeof (handle->gzip_trailer) - handle->trailer_bytes_to_read;
  fs_memscpy (output, handle->output_buff_size, trailer, size);
  handle->trailer_bytes_to_read -= size;
  if (handle->trailer_bytes_to_read == 0)
  {
    fs_zip_set_state (handle, FS_ZIP_DONE);
  }
  handle->message = FS_ZIP_STATUS_SUCCESS;
  return size;
}


static int
fs_zip_get_byte (fs_zip_handle handle, uint32 *curr_byte)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  int ch = 0;

  if ((*curr_byte) >= (uint32)handle->input_buffer_size)
  {
    s->zip_file_errors =  FS_ZIP_FILE_HEADER_SPLIT_ACROSS_FRAMES;
    ch = EOF;
    return ch;
  }

  ch = s->stream.next_in[(*curr_byte)++] ;

  if (ch == EOF)
  {
    s->zip_file_errors =  FS_ZIP_FILE_END_OF_FILE;
  }
  else
  {
    handle->header_length ++;
  }
  return ch;
}

static void
fs_zip_get_crc (fs_zip_handle handle)
{
  uint32 current  = 0;
  uint32 * curr = &current;
  uint32 shift = 0;
  uint32 *count = &(handle->trailer_bytes_to_read);

  while ((*count))
  {
    if (handle->input_buffer_size < (current + 1) )
    {
      fs_zip_set_state (handle, FS_ZIP_WAITING_FOR_INPUT);
      handle->message = FS_ZIP_STATUS_NEED_MORE_INPUT;
      return;
    }
    shift = (4 - (*count)) << 3;
    handle->file_crc += ((uint32)fs_zip_get_byte (handle, curr)) << (shift);
    (*count)-- ;
  }

  handle->message = FS_ZIP_STATUS_SUCCESS;
}


/* Check the gzip header of a fs_zlib_stream opened for reading. Set the stream
 * mode to transparent if the gzip magic header is not present; set s->err
 * to Z_DATA_ERROR if the magic header is present but the rest of the header
 * is incorrect.
 * The stream s has already been created sucessfully; s->stream.avail_in is
 *  the size of buffer filled. Does not support concatenated gz files.
 */
static int
fs_zip_check_gz_header (fs_zip_handle handle)
{
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;
  uint32 buffer_consumed = 0;
  uint32 *current_byte = &buffer_consumed;
  uint32 count = 0;
  int8 ch;
  int ret_value = FS_ZIP_STATUS_HEADER_ERROR;
  struct fs_zip_header *file_header = (struct fs_zip_header*)
                                                      (s->stream.next_in);
  handle->header_length = 0;

  /* Verify that buffer is atleast the minimum size of header */
  if (handle->input_buffer_size >= FS_ZIP_MINIMUM_GZIP_HEADER_SIZE)
  {
    *current_byte = FS_ZIP_MINIMUM_GZIP_HEADER_SIZE;
    handle->header_length = FS_ZIP_MINIMUM_GZIP_HEADER_SIZE;
  }
  else
  {
    return FS_ZIP_STATUS_HEADER_ERROR;
  }

  /* Peek ahead to check the gzip magic header */
  if (file_header->magic_header1 != FS_GZ_MAGIC_HEADER_1 ||
      file_header->magic_header2 != FS_GZ_MAGIC_HEADER_2)
  {
    return FS_ZIP_STATUS_HEADER_ERROR;
  }
  /* Check the rest of the gzip header */
  if (file_header->compression_method != Z_DEFLATED ||
        (file_header->flags & FS_GZ_RESERVED_MASK) != 0)
  {
    s->z_err = Z_DATA_ERROR;
    return FS_ZIP_STATUS_HEADER_ERROR;
  }

  /* The basic ten byte header is verified  */

  if ((file_header->flags & FS_GZ_EXTRA_FIELD_MASK) != 0)
  { /* skip the extra field */
    uint16 *extra_count  =  (uint16 *) ((s->stream.next_in) + *current_byte);
    /* *extra_count is garbage if EOF but the loop below will quit anyway */
    while (*extra_count-- != 0)
    {
      if (fs_zip_get_byte (handle, current_byte) != EOF)
      {
        break;
      }
    }
  }
  if ((file_header->flags & FS_GZ_ORIG_FNAME_MASK) != 0)
  { /* skip the original file name */
    while ((ch = fs_zip_get_byte (handle, current_byte)) != '\0')
    {
      if (ch != EOF)
      {
        break;
      }
    }
  }
  if ((file_header->flags & FS_GZ_HEAD_COMMENT_MASK) != 0)
  {   /* skip the .gz file comment */
    while ((ch = fs_zip_get_byte (handle, current_byte)) != '\0')
    {
      if (ch != EOF)
      {
        break;
      }
    }
  }
  if ((file_header->flags & FS_GZ_HEAD_CRC_MASK) != 0)
  {  /* skip the header crc */
    for (count = 0; count < 2; count++)
    {
      (void)fs_zip_get_byte (handle, current_byte);
    }
  }
  s->z_err = (s->zip_file_errors != FS_ZIP_FILE_IS_FINE) ? Z_DATA_ERROR : Z_OK;
  if (s->z_err == Z_OK)
  {
    ret_value = FS_ZIP_HEADER_CHECK_PASSED;
    handle->header_state = FS_ZIP_HEADER_CHECK_PASSED;
    handle->message = FS_ZIP_STATUS_SUCCESS;
    fs_zip_set_state (handle, FS_ZIP_DECOMPRESS_CONSUME_INPUT);
    s->stream.avail_in -= buffer_consumed;
    s->stream.next_in += buffer_consumed;
  }
  else
  {
    handle->message = FS_ZIP_STATUS_HEADER_ERROR;
    handle->header_state = FS_ZIP_BAD_HEADER;
    fs_zip_set_state (handle, FS_ZIP_HEADER_CHECK_FAILED);
  }
  return ret_value;
}


/* Flush input till output ends*/
static int
fs_zip_compress_flush (fs_zip_handle handle)
{
  int32 output_avaialble = 0;

  while (!fs_zip_flush (handle))
  {
    output_avaialble = fs_zip_deflate_get_output_length (handle);
    if (output_avaialble > 0)
    {
      handle->message = FS_ZIP_STATUS_SUCCESS;
      return output_avaialble;
    }
  }
  output_avaialble = fs_zip_deflate_get_output_length (handle);
  fs_zip_set_state (handle, FS_ZIP_PROCESS_TRAILER);
  handle->message = FS_ZIP_STATUS_SUCCESS;
  return output_avaialble;
}

/* Consume input till atleast one output put output in buffer and return bytes
 * in output buffer.
 */
static int
fs_zip_compress_read_input (fs_zip_handle handle)
{
  int32 output_available = 0;

  if (handle->input_buffer_size != 0)
  {
    do
    {
      handle->input_not_consumed = fs_zip_deflate_consume_input (handle);
      output_available = fs_zip_deflate_get_output_length (handle);
      if (output_available > 0)
      {
        handle->message = FS_ZIP_STATUS_SUCCESS;
        return output_available;
      }
    } while (handle->input_not_consumed > 0);

  }
  fs_zip_set_state (handle, FS_ZIP_WAITING_FOR_INPUT);
  handle->message = FS_ZIP_STATUS_NEED_MORE_INPUT;
  return output_available;
}

/* Consume input till atleast one output put output in buffer and return bytes
 * in output buffer.
 */
static int
fs_zip_decompress_consume_input (fs_zip_handle handle)
{
  int32 output_available = 0;
  struct fs_zlib_stream *s = &handle->fs_zlib_stream;

  if (handle->input_buffer_size > 0)
  {
    do
    {
      handle->input_not_consumed = fs_zip_inflate_consume_input (handle);
      output_available = fs_zip_inflate_write_output (handle);
      if (fs_zip_inflate_is_done (handle))
      {
        fs_zip_set_state (handle, FS_ZIP_PROCESS_TRAILER);
        handle->trailer_bytes_to_read = sizeof (s->generated_crc);
        handle->file_crc = 0;
        handle->input_buffer_size = handle->input_not_consumed;
        handle->message = FS_ZIP_STATUS_SUCCESS;
      }

      if (output_available > 0)
      {
        handle->message = FS_ZIP_STATUS_SUCCESS;
        return output_available;
      }

      if (fs_zip_inflate_is_done (handle))
      {
        return 0;
      }
    }while (handle->input_not_consumed > 0);

  }
  fs_zip_set_state (handle, FS_ZIP_WAITING_FOR_INPUT);
  handle->message = FS_ZIP_STATUS_NEED_MORE_INPUT;
  return output_available;
}

static void
fs_zip_check_crc (fs_zip_handle handle)
{
  fs_zip_get_crc (handle);
  if (handle->message == FS_ZIP_STATUS_NEED_MORE_INPUT)
  {
    return;
  }
  else if (handle->file_crc == handle->fs_zlib_stream.generated_crc)
  {
    fs_zip_set_state (handle, FS_ZIP_DONE);
    handle->message = FS_ZIP_STATUS_FINISHED;
  }
  else
  {
    fs_zip_set_state (handle, FS_ZIP_CRC_FAILED);
    handle->message = FS_ZIP_STATUS_CRC_ERROR;
  }
}


/* Initilize the zip module structures */
void fs_zip_init (void)
{
  int i;

  fs_zip_malloc_init ();
  memset (fs_gzhandle_struct, 0, sizeof(fs_gzhandle_struct));

  for (i = 0; i < FS_ZIP_MAX_HANDLE_COUNT ; i++)
  {
    fs_gzhandle_struct[i].state = FS_ZIP_UNUSED;
  }

  (void) simple_gzip_header.xflags;
  (void) simple_gzip_header.os_code;
}

/* Opens a gz stream and initializes state. Returns the handle if not in use
 * or a NULL if it is in use. Mode is either FS_ZIP_MODE_DECOMPRESSION  or
 * FS_ZIP_MODE_COMPRESSION for decompression
 * and compression respectively.
 */
fs_zip_handle
fs_zip_open (enum fs_zip_mode mode, void *heap_start, uint32 heap_size)
{
  fs_zip_handle handle = NULL;
  int i;
  int result = -1;

  if (mode != FS_ZIP_MODE_COMPRESSION && mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return NULL;
  }

  for (i = 0; i < FS_ZIP_MAX_HANDLE_COUNT ; i++)
  {
    handle = &fs_gzhandle_struct[i];
    if (handle->state == FS_ZIP_UNUSED)
    {
      /* initialize malloc */
      handle->malloc_handle = fs_zip_malloc_open (heap_start, heap_size);

      if (handle->malloc_handle == NULL)
      {
        return NULL;
      }

      fs_zip_set_state (handle, FS_ZIP_WAITING_FOR_INPUT);
      handle->mode = mode;
      handle->input_not_consumed = 0;
      handle->input_buffer_size = 0;
      handle->header_state = FS_ZIP_HEADER_BEGIN;
      handle->trailer_bytes_to_read = 0;
      handle->file_crc = 0;
      result = fs_zip_open_zlib_stream (handle, FS_ZIP_COMPRESSION_LEVEL);

      if (result != 0)
      {
       fs_zip_destroy (handle);
       fs_zip_malloc_close (handle->malloc_handle);
       handle->malloc_handle = NULL;
       fs_zip_set_state (handle, FS_ZIP_UNUSED);
       handle = NULL;
      }
      break;
    }
  }
  return handle;
}


enum fs_zip_return_status
fs_zip_set_input (fs_zip_handle handle, void *input, uint32 size)
{
  enum fs_zip_mode mode;
  struct fs_zlib_stream *s;

  if (handle == NULL)
  {
    return FS_ZIP_STATUS_UNKNOWN_ERROR;
  }

  if (((input == NULL) && (size > 0)))
  {
    return FS_ZIP_STATUS_UNKNOWN_ERROR;
  }

  mode = handle->mode;
  s = &handle->fs_zlib_stream;
  handle->message = FS_ZIP_STATUS_SUCCESS;

  if (handle->state == FS_ZIP_WAITING_FOR_INPUT)
  {
    if (size != 0)
    {
      handle->input_buffer = (uint8*) input;
      s->stream.next_in = (Byte*) input;
      handle->input_buffer_size = size;
      if (mode == FS_ZIP_MODE_COMPRESSION)
      {
        fs_zip_deflate_set_input (handle);

        if (handle->header_state != FS_ZIP_HEADER_CHECK_PASSED)
        {
          fs_zip_set_state (handle, FS_ZIP_PROCESS_HEADER);
        }
        else
        {
          fs_zip_set_state (handle, FS_ZIP_COMPRESS_CONSUME_INPUT);
        }
      }
      else if (mode == FS_ZIP_MODE_DECOMPRESSION)
      {
        fs_zip_inflate_set_input (handle);

        if (handle->header_state != FS_ZIP_HEADER_CHECK_PASSED)
        {
          fs_zip_set_state (handle, FS_ZIP_PROCESS_HEADER);
        }
        else if (handle->trailer_bytes_to_read > 0)
        {
          fs_zip_set_state (handle, FS_ZIP_PROCESS_TRAILER);
        }
        else
        {
          fs_zip_set_state (handle, FS_ZIP_DECOMPRESS_CONSUME_INPUT);
        }
      }
      else
      {
        handle->message = FS_ZIP_STATUS_MODE_ERROR;
      }

    }
    else /* if size == 0 */
    {
      fs_zip_set_state (handle, FS_ZIP_COMPRESS_FLUSH);
    }
  }
  else
  {
    handle->message = FS_ZIP_STATUS_STATE_ERROR;
  }
  return handle->message;
}

/* Produce the compresed output in the output buffer. and puts the length of
 * output in bytes in the variable size and returns the status.
 */
enum fs_zip_return_status
fs_zip_compress (fs_zip_handle handle, void *output, uint32 output_buff_size,
                 uint32 *available_output)
{
  struct fs_zlib_stream *s;

  if ((handle == NULL) || (output == NULL) || (output_buff_size == 0))
  {
    return FS_ZIP_STATUS_UNKNOWN_ERROR;
  }
  s = &handle->fs_zlib_stream;
  handle->message = FS_ZIP_STATUS_UNKNOWN_ERROR;
  if (handle->mode != FS_ZIP_MODE_COMPRESSION)
  {
    return FS_ZIP_STATUS_MODE_ERROR;
  }
  if (handle->state == FS_ZIP_UNUSED)
  {
    handle->message = FS_ZIP_STATUS_UNUSED_STREAM;
  }
  else
  {
    s->stream.next_out = (Byte*) output;
    handle->output_buffer = (uint8*) output;
    handle->output_buff_size = output_buff_size;
    s->stream.avail_out = output_buff_size;
    switch (handle->state)
    {
      case FS_ZIP_PROCESS_HEADER :
      {
        *available_output = fs_zip_get_gzip_header (handle);
        break;
      }

      case FS_ZIP_COMPRESS_CONSUME_INPUT :
      {
        *available_output = fs_zip_compress_read_input (handle);
        break;
      }

      case FS_ZIP_COMPRESS_FLUSH :
      {
       *available_output = fs_zip_compress_flush (handle);
        break;
      }

      case FS_ZIP_PROCESS_TRAILER :
      {
        *available_output = fs_zip_write_trailer (handle);
        break;
      }

      case FS_ZIP_DONE :
      {
        *available_output = 0;
        handle->message = FS_ZIP_STATUS_FINISHED;
        break;
      }

      default :
      {
        *available_output = 0;
        handle->message = FS_ZIP_STATUS_UNKNOWN_ERROR;
      }
    }

  }
  return handle->message;
}

enum fs_zip_return_status
fs_zip_decompress (fs_zip_handle handle, void *output, uint32 output_buff_size,
                    uint32 *available_output)
{
  struct fs_zlib_stream *s;

  if ((handle == NULL) || (output == NULL) || (output_buff_size == 0))
  {
    return FS_ZIP_STATUS_UNKNOWN_ERROR;
  }
  s = &handle->fs_zlib_stream;
  handle->message = FS_ZIP_STATUS_UNKNOWN_ERROR;

  if (handle->mode != FS_ZIP_MODE_DECOMPRESSION)
  {
    return FS_ZIP_STATUS_MODE_ERROR;
  }
  if (handle->state == FS_ZIP_UNUSED)
  {
    handle->message = FS_ZIP_STATUS_UNUSED_STREAM;
  }
  else
  {
    s->stream.next_out = (Byte*) output;
    handle->output_buffer = output;
    handle->output_buff_size = output_buff_size;
    s->stream.avail_out = output_buff_size;
    switch (handle->state)
    {
      case  FS_ZIP_HEADER_CHECK_FAILED :
      {
        *available_output = 0;
        handle->message = FS_ZIP_STATUS_HEADER_ERROR;
        break;
      }

      case FS_ZIP_PROCESS_HEADER :
      {
        fs_zip_check_gz_header (handle);
        if (handle->message == FS_ZIP_STATUS_HEADER_ERROR)
        {
          *available_output = 0;
          break;
        }
        /* else fall through */
      }
      /*lint -fallthrough */
      case FS_ZIP_DECOMPRESS_CONSUME_INPUT :
      {
        *available_output = fs_zip_decompress_consume_input (handle);
        break;
      }

      case FS_ZIP_PROCESS_TRAILER :
      {
        fs_zip_check_crc (handle);
        if (handle->message != FS_ZIP_STATUS_FINISHED )
        {
          *available_output = 0;
          break;
        }
        /* else fall through */
      }

      /*lint -fallthrough */
      case FS_ZIP_DONE :
      {
        *available_output = 0;
        handle->message = FS_ZIP_STATUS_FINISHED;
        break;
      }

      default :
      {
        *available_output = 0;
        handle->message = FS_ZIP_STATUS_UNKNOWN_ERROR;
      }
    }
  }
  return handle->message;
}

enum fs_zip_return_status
fs_zip_close (fs_zip_handle handle)
{
  if (handle == NULL)
  {
    return FS_ZIP_STATUS_UNKNOWN_ERROR;
  }
  if (handle->state == FS_ZIP_UNUSED)
  {
    return FS_ZIP_STATUS_STATE_ERROR;
  }
  fs_zip_destroy (handle);
  fs_zip_malloc_close (handle->malloc_handle);
  handle->malloc_handle = NULL;
  fs_zip_set_state (handle, FS_ZIP_UNUSED);
  return FS_ZIP_STATUS_SUCCESS;
}

int
fs_zip_is_this_a_gzip_file (void *buffer, uint32 buffer_size)
{
  struct fs_zip_header *file_header = (struct fs_zip_header*) buffer;

  if ((buffer_size < FS_ZIP_MINIMUM_GZIP_SIZE) || (buffer == NULL))
  {
    return 0;
  }
  if ((file_header->magic_header1 != FS_GZ_MAGIC_HEADER_1) ||
      (file_header->magic_header2 != FS_GZ_MAGIC_HEADER_2) ||
      (file_header->compression_method != Z_DEFLATED) ||
        ((file_header->flags & FS_GZ_RESERVED_MASK) != 0))
  {
    return 0;
  }
  return 1;
}
#endif /* end FEATURE_EFS_ZIP*/

