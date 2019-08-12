/**********************************************************************
 * fs_stdio.c
 * EFS Stdio implementation.
 *
 * Note 1: The functions in this module are not thread-safe.
 * Note 2: Only one stream at a time can be marked for asynchronous
 * writes.
 *
 * Standard IO library for EFS.
 * Copyright (C) 2004-2007, 2009-2011, 2013 QUALCOMM Technologies, Inc.
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_stdio.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when        who  what, where, why
 * ---------   ---  -------------------------------------------------------
 * 2015-12-27   mj  Remove function calls with ASSERT macro.
 * 2013-10-09   nr  Replace banned memory functions with safer versions.
 * 2011-11-08  wek  Featurize autodog enable call to build different targets.
 * 2011-11-03  wek  Remove dereference to task TCB.
 * 2010-11-03  wek  Make stdio independent of page size macro.
 * 2010-09-16  yog  Perform bitwise AND operation with DEFAULT_UMASK field.
 * 2010-02-08   rp  CMI compliance, remove #ifdef from API files.
 * 2009-06-12  ebb  Moved rex_xxx calls to fs_osal.h abstraction.
 * 2009-05-18   rp  Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
 * 2009-01-28  wek  Modified rollback calls.
 * 2007-08-19   rp  Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
 * 2007-03-01   rp  Removed efs_fget_rbk_file_nbr().
 * 2007-01-31   rp  Added rollback support for existing files.
 * 2007-01-19  umr  Add Support for efs_fread() after a efs_fwrite().
 * 2006-12-05  dlb  Max transaction size configurable.
 * 2006-11-14   sh  Cleanup, warning fixes.
 * 2006-11-02   rp  Added file-rollback support of new-files.
 * 2006-10-06   sh  Add efs_fpending().
 * 2006-06-09  dlb  Add efs_fdopen().
 * 2006-05-30  dlb  Add efs_fileno().
 * 2006-05-16  dlb  Convert back to efs2 API.
 * 2006-04-13   sh  Removed incorrect "Lint cleanup" comment.
 * 2006-02-06   sh  Avoid unnecessary flush during unnecessary seek.
 * 2005-10-30   sh  Lint cleanup.
 * 2005-12-08  nrs  Added error check on flushing while closing
 * 2005-06-14   cs  Added autodogkick.
 * 2005-05-26   sd  Compilation fixes for L4.
 * 2005-05-16   ck  Fixed an issue where fs_stdio_async_stream flag was not
 *                  set to NULL when the stream was closed after an error,
 *                  that resulted in data aborts.
 *                  after an async stream is closed.
 * 2005-05-12  nrs  Added a check to handle full file system cases in
 *                  flush_write_buffer( )
 * 2005-04-29   ck  Fixed another bug that could cause an illegel ptr ref
 *                  after an async stream is closed.
 * 2005-04-26  dlb  Add 2K page support.
 * 2005-02-01   gr  Fixed a bug that could cause an illegal pointer reference
 *                  after an asynchronous stream is closed.
 * 2004-10-07  dlb  Whitespace cleanup.
 * 2004-06-05   gr  Set efs_errno when needed, and handle stream errors better.
 * 2004-04-30   gr  Fixed a bug that could result in a infinite loop while
 *                  blocking waiting for a write to complete.
 * 2004-03-23   gr  Fixed a bug in efs_fseek.
 * 2004-02-20   gr  Modified to use EFS1 API. This allows the user to write
 *                  to MMC/SD cards besides flash.
 * 2004-02-10   gr  Modified to support asynchronous writes.
 * 2003-01-25  dlb  Implement with basic write buffering.
 *
 ==========================================================================*/

/* This is a non-conforming partial implementation of some parts of the ANSI
 * C stdio library. */

#include "fs_stdio.h"
#include "fs_public.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_errno.h"
#include "fs_fcntl.h"
#include "assert.h"
#include "fs_osal.h"

#include <stdlib.h>
#include <string.h>

#include "fs_rollback.h"
#include "fs_efs2.h"
#include "fs_global_lock.h"

/* The cluster and buffer sizes must all be powers of two. There
 * is code in this module that makes this assumption.
 */

/* EFS_MAX_BUF_SIZE: maximum buffer size                       */
#define EFS_MAX_BUF_SIZE   1048576

#define STREAM_MAGIC 0x279b3f1a

struct _efs_file_io {
  uint32        stream_magic; /* used to check for an initialized stream. */
  int           fd;           /* file descriptor.                         */
  unsigned      state;        /* bitmask with markers for various states. */
  int           error;        /* set if there is an error on the stream.  */
  fs_off_t      pos;          /* current file position.                   */
  fs_os_tcb_type *caller_tcb; /* TCB to signal after buffer flush. */
  unsigned long caller_sigs;  /* signals to set after buffer flush.       */
  fs_os_tcb_type *writer_tcb; /* TCB to signal to flush buffer.    */
  unsigned long writer_sigs;  /* signals to set to flush buffer.          */
  unsigned      head;         /* buffer index for writing data.           */
  unsigned      tail;         /* buffer index for draining data.          */
  unsigned int  bufsize;      /* buffer size.                             */
  unsigned int  clustsize;    /* File system cluster size.                */
  char         *buffer;       /* holds the data to be written out.        */
  struct efs_rbk_file_info rbk_info; /* Rollback file information         */
};

/* The state field has markers for various states.
 * XXX: Describe MODE and OK.
 */
#define STATE_MODE_READ         0x02
#define STATE_MODE_WRITE        0x04
#define STATE_MODE_MASK         (STATE_MODE_READ | STATE_MODE_WRITE)
#define STATE_OK_READ           0x08
#define STATE_OK_WRITE          0x10
#define STATE_OK_MASK           (STATE_OK_READ | STATE_OK_WRITE)
#define MODE_TO_OK_SHIFT     2

/* Reset a state to allow whatever kind of read/write is permitted by the
 * original open. */
#define RESET_OK(state) \
   (((state) & ~STATE_OK_MASK) |                        \
    (((state) & STATE_MODE_MASK) << MODE_TO_OK_SHIFT))

static int buf_avail (int head, int tail, int bufsize);
static void flush_write_buffer (EFS_FILE *fp);
static void block_for_write_completion (EFS_FILE *fp);
static int async_write_enabled (EFS_FILE *fp);

/* Note: this macro has a return statement in it. */
#define STREAM_CHECK(stream, badval) \
  do { \
    if ((stream) == NULL || (stream)->stream_magic != STREAM_MAGIC) { \
      efs_errno = EINVAL; \
      return (badval); \
    } \
  } while (0)

/* EFS doesn't yet have a UMASK, so define a default that will be used by
 * these calls. */
#define DEFAULT_UMASK 0022

/* The head and tail for each file buffer are mod 2n pointers. Taking the
 * mod of these quantities with the buffer size will yield values that can be
 * used as indexes into the buffer.
 */

#define EFS_BUF_EMPTY(head, tail) \
  ((head) == (tail))

#define EFS_BUF_FULL(head, tail, bufsize) \
  ((head) == (tail) + (bufsize) || (tail) == (head) + (bufsize))

/* Increment a mod 2n pointer.
 */
#define EFS_BUF_INC(ptr, inc, bufsize) \
  do { \
    ptr += (inc); \
    if ((ptr) >= ((bufsize) << 1)) \
      ptr -= ((bufsize) << 1); \
  } while (0)

/* Copy data into a buffer, accounting for wrapping.
 */
#define EFS_BUF_COPY(buf, data, start, count, bufsize) \
  do { \
    FS_ASSERT ((count) <= (bufsize)); \
    if ((start) + (count) > (bufsize)) { \
      fs_memscpy ((buf) + (start), (bufsize) - (start), (char *) (data), \
                  (bufsize) - (start)); \
      fs_memscpy ((buf), (count) - ((bufsize) - (start)), \
                  (char *) (data) + ((bufsize) - (start)), \
                  (count) - ((bufsize) - (start))); \
    } else { \
      fs_memscpy ((buf) + (start), (bufsize) - (start), (char *) (data), \
                  (count)); \
    } \
  } while (0)

/* Pointer to the stream that the async task should process. Note that we
 * only support one async stream currently.
 */
static EFS_FILE *fs_stdio_async_stream;

static uint32 stdio_cluster_size;

void
efs_stdio_init (void)
{
  struct fs_statvfs efs_info;

  if (efs_statvfs ("/", &efs_info))
    FS_ERR_FATAL ("efs_stdio_init failed, efs_errno=%d.", efs_errno, 0, 0);

  stdio_cluster_size = efs_info.f_bsize;
}

/* Parse the user passed mode flag.  Returns '0' on success, or '-1' on
 * error.  Sets efs_errno to an appropriate value on error.  Upon
 * successful return '*oflag' will be set to the appropriate flag to pass
 * to efs_open, and '*state' will be set the proper state to initialize the
 * descriptor. */
static int
efs_parse_mode_flag (const char *mode,
    int *oflag, unsigned *state)
{
  int has_plus = 0;
  const char *p;
  int loflag;
  unsigned lstate;

  if (mode == NULL || *mode == '\0') {
    efs_errno = EINVAL;
    return -1;
  }

  /* Parse the mode string.  ANSI requires the first character to be one of
   * 'r', 'w', or 'a', and can contain the plus string.  Since other
   * characters are value after the initial character, just scan for the
   * '+' character, ignoring others. */
  for (p = mode + 1; *p != '\0'; p++) {
    if (*p == '+')
      has_plus = 1;
  }

  lstate = 0;
  loflag = 0;
  switch (*mode) {
    case 'r':
      lstate = STATE_MODE_READ;
      if (has_plus)
        loflag = O_RDWR;
      else
        loflag = O_RDONLY;
      break;

    case 'w':
      lstate = STATE_MODE_WRITE;
      if (has_plus)
        loflag = O_RDWR | O_CREAT | O_TRUNC;
      else
        loflag = O_WRONLY | O_CREAT | O_TRUNC;
      break;

    case 'a':
      /* We do not support append mode for now.
      */
      efs_errno = EINVAL;
      return -1;

    default:
      efs_errno = EINVAL;
      return -1;
  }

  if (has_plus)
    lstate = STATE_MODE_READ | STATE_MODE_WRITE;

  lstate = RESET_OK (lstate);

  *state = lstate;
  *oflag = loflag;
  return 0;
}

/* Returns TRUE if there is an 'u' or 'U' in the given mode string
 * else FALSE */
static boolean
efs_parse_mode_for_rollback_flag (const char *mode)
{
  const char *p;

  if (mode == NULL || *mode == '\0')
    return FALSE;

  /* Skip the first character (r,w,a) of mode string */
  for (p = mode + 1; *p != '\0'; p++) {
    if (*p == 'u' || *p == 'U')
        return TRUE;
  }

  return FALSE;
}

/* Construct a stream from a given initial 'state' and an initial file
 * handle.  Returns NULL if there was a problem allocating the descriptor.
 */
static EFS_FILE *
efs_alloc_descriptor (int filedes, unsigned state)
{
  EFS_FILE *fp;

  fp = malloc (sizeof (struct _efs_file_io));
  if (fp == NULL) {
    efs_errno = ENOMEM;
    return NULL;
  }

  fp->buffer = malloc ((FS_MAX_TRANSACTION_DATA_PAGES * stdio_cluster_size));
  if (fp->buffer == NULL) {
    efs_errno = ENOMEM;
    free (fp);
    return NULL;
  }

  fp->pos = 0;
  fp->stream_magic = STREAM_MAGIC;
  fp->fd = filedes;
  fp->state = state;
  fp->error = 0;
  fp->caller_tcb = NULL;
  fp->caller_sigs = 0;
  fp->writer_tcb = NULL;
  fp->writer_sigs = 0;
  fp->head = fp->tail = 0;
  fp->bufsize = (FS_MAX_TRANSACTION_DATA_PAGES * stdio_cluster_size);
  fp->clustsize = stdio_cluster_size;

  /* Rollback members */
  fp->rbk_info.rbk_file_nbr = 0; /* 0 :-> 0 means No Rollback */
  fp->rbk_info.rbk_fd = -1;

  return fp;
}

EFS_FILE *
efs_fopen (const char *path, const char *mode)
{
  int fd_result = -1;
  unsigned state;
  EFS_FILE *fp;
  int oflag;
  struct efs_rbk_file_info rbk_info = {0, -1}; /* 0 :-> 0 means No Rollback */

  FS_GLOBAL_LOCK ();

  if (efs_parse_mode_flag (mode, &oflag, &state) != 0)
    goto Error;

  if (efs_parse_mode_for_rollback_flag (mode) == TRUE) {
    if (efs_rollback_create_rbk_file (path, &rbk_info) == -1) {
      /* Could not create rollback file, so we shall bail */
      goto Error;
    }
  }

  fd_result = efs_open (path, oflag, 0777 & ~DEFAULT_UMASK);
  if (fd_result < 0) {
    /* open failed, so we shall bail */
    goto Error;
  }

  fp = efs_alloc_descriptor (fd_result, state);
  if (fp == NULL) {
    /* failed to allocate a stream, so we shall bail */
    goto Error;
  }

  /* copy the rbk_info onto the stream */
  if (rbk_info.rbk_file_nbr != 0) {
    fp->rbk_info = rbk_info;
  }

  FS_GLOBAL_UNLOCK ();
  return fp;

Error:
  if (rbk_info.rbk_file_nbr != 0) {
    efs_rollback_remove_rbk_file (&rbk_info);
  }
  if (fd_result >= 0) {
    (void) efs_close (fd_result);
  }

  FS_GLOBAL_UNLOCK ();
  return NULL;
}

EFS_FILE *
efs_fdopen (int filedes, const char *mode)
{
  unsigned state;
  int oflag;

  if (efs_parse_mode_flag (mode, &oflag, &state) != 0)
    return NULL;

  return efs_alloc_descriptor (filedes, state);
}

/* Posix leaves the behavior on partial writes and reads undefined.  The
 * best I can come up with doing is to just return the number of full items
 * that were read or written.  The file position might not be in the
 * desired location.  My recommendation is that 'size' always be given as
 * 1. */
fs_size_t
efs_fread (void *ptr, fs_size_t size, fs_size_t nitems, EFS_FILE *stream)
{
  int result;
  fs_size_t  full_size = size * nitems;
  fs_size_t  pos = 0;

  STREAM_CHECK (stream, 0);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return 0;
  }

  if ((stream->state & STATE_OK_READ) == 0) {
    /* XXX: Is this the right return code? */
    efs_errno = EBADF;
    return 0;
  }

  if (!EFS_BUF_EMPTY(stream->head, stream->tail))
  {
    if (async_write_enabled (stream))
      block_for_write_completion (stream);
    else
      flush_write_buffer (stream);
  }

  while (pos < full_size) {
    result = efs_read (stream->fd,
        (void *) ((uint8 *) ptr + pos),
        full_size - pos);
    if (result > 0) {
      if (result == 0)
        break;
      pos += (fs_size_t) result;
      stream->pos += (fs_size_t) result;
    } else {
      /* Error, leave efs errno. */
      /* XXX: Needs to set stream state. */
      break;
    }
  }

  /* Best we can do is return the floor of the result. */
  return pos / size;
}

/* Returns the available space in a buffer. head and tail are mod 2n
 * pointers.
 */
static int
buf_avail (int head, int tail, int bufsize)
{
  if (EFS_BUF_FULL (head, tail, bufsize))
    return 0;

  if (EFS_BUF_EMPTY (head, tail))
    return bufsize;

  return (tail + bufsize - head) & (bufsize - 1);
}

fs_size_t
efs_fwrite (void *ptr, fs_size_t size, fs_size_t nitems, EFS_FILE *stream)
{
  fs_size_t full_size = size * nitems;
  fs_size_t count;
  fs_size_t offset = 0;
  unsigned tail;
  int is_async_stream;

  STREAM_CHECK (stream, 0);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return 0;
  }

  if ((stream->state & STATE_OK_WRITE) == 0) {
    /* XXX: Is this the right return code? */
    efs_errno = EBADF;
    return 0;
  }

  is_async_stream = async_write_enabled (stream);

  if (is_async_stream && stream->caller_tcb != fs_os_self ()) {
    /* XXX: Is this the right return code? */
    efs_errno = EPERM;
    return 0;
  }

  /* Check if this file needs rollback */
  if (stream->rbk_info.rbk_file_nbr != 0) {
    /* Rollback : Backup original file contents before overwrite */
    if (efs_rollback_write_req_to_orig_file (stream->rbk_info.rbk_fd,
                                             stream->fd, full_size,
                                             stream->pos) == -1) {
      efs_errno = EROLLBACK;
      return 0;
    }
  }

  if (is_async_stream)
    fs_os_clr_sigs (stream->caller_tcb, stream->caller_sigs);

  while (offset < full_size) {

    /* If the buffer is full, do one of two things:
     * (a) if this stream is marked for async writes, block until the
     * writer task has written something out and signalled back.
     * (b) if this stream is marked for synchronous writing, flush the
     * buffer.
     */
    tail = stream->tail;
    if (EFS_BUF_FULL (stream->head, tail, stream->bufsize)) {
      if (is_async_stream) {
        fs_os_set_sigs (stream->writer_tcb, stream->writer_sigs);
        while (EFS_BUF_FULL (stream->head, tail, stream->bufsize)) {
          fs_os_wait (stream->caller_sigs);
          fs_os_clr_sigs (stream->caller_tcb, stream->caller_sigs);
          tail = stream->tail;
          if (stream->error)
            break;
        }
      } else {
        flush_write_buffer (stream);
      }
    }

    if (stream->error)
      return 0;

    tail = stream->tail;

    /* Compute the number of bytes we will add to the buffer. This number
     * is the minimum of two quantities:
     * 1) the number of bytes available in the buffer
     * 2) the number of bytes left in the request
     */
    count = (fs_size_t) buf_avail (stream->head, tail, stream->bufsize);

    ASSERT (count > 0);

    if (count > full_size - offset)
      count = full_size - offset;

    EFS_BUF_COPY (stream->buffer, (char *) ptr + offset,
      stream->head & (stream->bufsize-1), count, stream->bufsize);

    EFS_BUF_INC (stream->head, count, stream->bufsize);

    if (is_async_stream)
      fs_os_set_sigs (stream->writer_tcb, stream->writer_sigs);

    offset += count;
    stream->pos += count;
  }

  /* Best we can do is return the floor of the result. */
  return offset / size;
}

int
efs_fclose (EFS_FILE *stream)
{
  int result;
  int flush_error = 0;

  STREAM_CHECK (stream, -1);

  if (stream->error == 0) {
    if (async_write_enabled (stream))
      block_for_write_completion (stream);
    else
      flush_write_buffer (stream);
    if (stream->error != 0)
      flush_error = 1;
  }

  if (async_write_enabled (stream))
    fs_stdio_async_stream = NULL;

  result = efs_close (stream->fd);

  if (stream->rbk_info.rbk_file_nbr != 0) {
    /* Rollback operation is completed for this file, so we shall remove the
       rollback-file that we created to do rollback on this file. */
    efs_rollback_remove_rbk_file (&stream->rbk_info);
  }

  stream->stream_magic = 0;
  free (stream->buffer);
  free (stream);

  if (flush_error)
  {
    return -1;
  }
  else if (result < 0) {
    return -1;
  } else {
    return 0;
  }
}

int
efs_fseek (EFS_FILE *stream, long offset, int whence)
{
  fs_off_t result;

  STREAM_CHECK (stream, -1);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return -1;
  }

  if (async_write_enabled (stream))
    block_for_write_completion (stream);
  else
    flush_write_buffer (stream);

  result = efs_lseek (stream->fd, offset, whence);
  if (result == (fs_off_t) -1) {
    stream->state &= ~STATE_OK_MASK;
    stream->error = 1;
    return -1;
  } else {
    stream->state = RESET_OK (stream->state);
    stream->pos = result;
    stream->head = stream->tail = stream->pos & (stream->clustsize - 1);
    return 0;
  }
}

long
efs_ftell (EFS_FILE *stream)
{
  STREAM_CHECK (stream, -1);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return -1;
  } else
    return stream->pos;
}

fs_size_t
efs_fpending (EFS_FILE *stream)
{
  return (stream->bufsize
          - buf_avail (stream->head, stream->tail, stream->bufsize));
}

/* Flush the write buffer, if necessary.  The fd itself must already be
 * positioned correctly. */
static void
flush_write_buffer (EFS_FILE *stream)
{
  int result;
  unsigned head;
  unsigned tail;
  unsigned real_tail;
  unsigned count;
  int is_async_stream = async_write_enabled (stream);
  int write_failed = 0;

  head = stream->head;
  tail = stream->tail;
  while (head != tail) {
    count = stream->bufsize - buf_avail(head, tail, stream->bufsize);
    if (count > (FS_MAX_TRANSACTION_DATA_PAGES * stream->clustsize))
      count = (FS_MAX_TRANSACTION_DATA_PAGES * stream->clustsize);
    if (count > 0) {
      real_tail = tail & (stream->bufsize-1);
      if (count + real_tail > stream->bufsize)
        count = stream->bufsize - real_tail;
      result = efs_write (stream->fd,
          (void *) (stream->buffer + real_tail),
          count);
      if (result <= 0)
      {
        stream->error = 1;
        write_failed = 1;
        break;
      } else if (result == 0)
      {
        /* XXX: errno should already be set? */
        efs_errno = ENOSPC;
        stream->error = 1;
        write_failed = 1;
        break;
      } else
      {
        EFS_BUF_INC (stream->tail, result, stream->bufsize);
        tail = stream->tail;
      }
    }
    if (is_async_stream)
      fs_os_set_sigs (stream->caller_tcb, stream->caller_sigs);
  }

  if (is_async_stream && write_failed)
    fs_os_set_sigs (stream->caller_tcb, stream->caller_sigs);
}

/* Set buffer attributes for a stream. Returns 0 on success and -1 on error.
 */
int
efs_setvbuf (EFS_FILE *stream, char *buf, int mode, fs_size_t size)
{
  char *buffer;

  STREAM_CHECK (stream, -1);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return -1;
  }

  /* We support only full buffering.
   */
  if (mode != EFS_IOFBF) {
    efs_errno = EINVAL;
    return -1;
  }

  /* We do not yet support user-allocated buffers.
   */
  if (buf != NULL) {
    efs_errno = EINVAL;
    return -1;
  }

  /* Ensure that the size passed in is in the correct range and is a power
   * of two.
   */
  if ((size > EFS_MAX_BUF_SIZE) || (size < stream->clustsize) ||
      ((size & (size - 1)) != 0)) {
    efs_errno = EINVAL;
    return -1;
  }

  /* Allocate a new buffer.
   */
  buffer = malloc (size);
  if (buffer == NULL) {
    efs_errno = ENOMEM;
    return -1;
  }

  if (async_write_enabled (stream))
    block_for_write_completion (stream);
  else
    flush_write_buffer (stream);

  /* Free the existing buffer and set up the new one.
   */
  free (stream->buffer);

  stream->buffer = buffer;
  stream->bufsize = size;
  stream->head = stream->tail = stream->pos & (stream->clustsize - 1);

  return 0;
}

/* Returns 1 if the specified stream is enabled for asynchronous
 * writing and 0 if not.
 */
static int
async_write_enabled (EFS_FILE *stream)
{
  if (stream->caller_tcb != NULL)
    return 1;
  else
    return 0;
}

/* Mark a stream for asynchronous writing by setting all the parameters
 * required. Returns 0 on success and -1 on failure.
 */
int
efs_set_async_params (EFS_FILE *stream, void *caller_tcb,
  unsigned long caller_sigs, void *writer_tcb,
  unsigned long writer_sigs)
{
  STREAM_CHECK (stream, -1);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return -1;
  }

  /* If another stream is already marked for asynchronous writing, return
   * failure.
   */
  if (fs_stdio_async_stream != NULL && fs_stdio_async_stream != stream)
    return -1;

  if (async_write_enabled (stream))
    block_for_write_completion (stream);
  else
    flush_write_buffer (stream);

  stream->caller_tcb  = caller_tcb;
  stream->caller_sigs = caller_sigs;
  stream->writer_tcb  = writer_tcb;
  stream->writer_sigs = writer_sigs;

  fs_stdio_async_stream = stream;

  return 0;
}

/* Mark a stream for synchronous writing.
 */
int
efs_set_sync (EFS_FILE *stream)
{
  STREAM_CHECK (stream, -1);

  if (stream->error != 0) {
    efs_errno = EBADF;
    return -1;
  }

  if (async_write_enabled (stream)) {
    block_for_write_completion (stream);

    stream->caller_tcb  = NULL;
    stream->caller_sigs = 0;
    stream->writer_tcb  = NULL;
    stream->writer_sigs = 0;

    fs_stdio_async_stream = NULL;
  }
  return 0;
}

/* Queue the buffer for writing and block until all the data in it
 * has been written out.  The fd itself must already be positioned
 * correctly. */
static void
block_for_write_completion (EFS_FILE *stream)
{
  uint32 dog_autokick;
  int result;
  result = async_write_enabled (stream);
  ASSERT (result != 0);

  if (stream->head != stream->tail)
    fs_os_set_sigs (stream->writer_tcb, stream->writer_sigs);

#ifdef FEATURE_REX_OPAQUE_TCB_APIS
  dog_autokick = fs_os_autodog_enable_self ();
#else
  dog_autokick = fs_os_autodog_enable (fs_os_self()->dog_report_val);
#endif

  while ((stream->head != stream->tail) && (stream->error == 0)) {
    fs_os_wait (stream->caller_sigs);
    fs_os_clr_sigs (stream->caller_tcb, stream->caller_sigs);
  }

  if (dog_autokick == 0) {
    fs_os_autodog_disable();
  }
}

/* Function to be called by a task that handles asynchronous write
 * requests.
 */
void
efs_process_async_writes (void)
{
  if (fs_stdio_async_stream != NULL)
    flush_write_buffer (fs_stdio_async_stream);
}

/* Return the underlying filehandle to make raw EFS calls.  It is up to the
 * caller to handle flushing of buffers properly. */
int
efs_fileno (EFS_FILE *stream)
{
  STREAM_CHECK (stream, -1);

  return stream->fd;
}

#ifdef FS_UNIT_TEST
int
efs_fget_rbk_file_fd (EFS_FILE *stream)
{
  STREAM_CHECK (stream, -1);

  return stream->rbk_info.rbk_fd;
}
#endif /* FS_UNIT_TEST */
