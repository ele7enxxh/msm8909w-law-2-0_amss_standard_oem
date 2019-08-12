/**********************************************************************
 * fs_estdio.c
 * EFS Stdio implementation.
 *
 * Standard File IO library for EFS.
 * Copyright (C) 2009-2010, 2013 QUALCOMM Technologies, Inc.
 *
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_estdio.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when        who  what, where, why
 * ----------  ---  -------------------------------------------------------
 * 2013-10-09  nr   Replace banned memory functions with safer versions.
 * 2010-04-23  yog  Removed compiler warnings.
 * 2010-04-21  rp   Fix the prototype of efs_efseek64() API.
 * 2010-03-19  yog  Added support for files having max filesize upto 4GB.
 * 2009-06-12  ebb  Moved rex_xxx calls to fs_osal.h abstraction.
 * 2009-05-18  rp   Fix compiler warning for 8200 which has gcc with more -Ws.
 * 2009-04-24  wek  Added debug values to FS_ERR_FATAL in efs_estdio_init.
 * 2009-01-26  wek  Create. Replacement for fs_stdio API.
 *
 ==========================================================================*/

#include <string.h>
#include "fs_estdio.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_err.h"
#include "fs_util.h"
#include "assert.h"
#include "fs_osal.h"
#include "fs_rollback.h"

/** Maximum number of files that can be simultaneously opened with efs_efopen
 *  or efs_efdopen.*/
#ifndef EFS_MAX_OPEN_STREAMS
  #define EFS_MAX_OPEN_STREAMS         32
#endif

/** Signature to identify a valid stream */
#define ESTREAM_MAGIC 0x279b0D00

/** The state field has markers for various states.
 *  When opening a file with update mode ('+' as a character in mode argument
 *  efs_efopen) both mode READ and WRITE are true.
 */
#define STREAM_MODE_READ      0x0001
#define STREAM_MODE_WRITE     0x0002
#define STREAM_MODE_APPEND    0x0004
#define STREAM_MODE_ROLLBACK  0x0008

/** Indicates Buffered Mode */
#define STREAM_BUFFERED       0x0010

/** Indicates that the file pointer is being used */
#define STREAM_ACTIVE         0x0020

#define STREAM_INACTIVE       0x0000

/* We can check if a file has rollback by checking the state or checking the
 * value of rbk_info.rbk_file_nbr and verifying it's different than 0.
 */
#define HAS_ROLLBACK(stream) ((stream->state & STREAM_MODE_ROLLBACK) != 0)
#define OK_TO_READ(stream)   ((stream->state & STREAM_MODE_READ) != 0)
#define OK_TO_WRITE(stream)  ((stream->state & STREAM_MODE_WRITE) != 0)
#define OK_TO_APPEND(stream) ((stream->state & STREAM_MODE_APPEND) != 0)
/* Checks if the stream is fully buffered, line buffering is not implemented */
#define FULLY_BUFFERED(stream)  ((stream->state & STREAM_BUFFERED) != 0)

/** Default permission mode for newly created files. When EFS2 sets a umask or
 * user mask this can be changed.
 */
#define DEFAULT_EUMASK (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

/** Initialize a lock (rex critical section) */
#define STDIO_INIT_LOCK(s)     fs_os_init_crit_sect(&s->f_lock)
/** Request a lock */
#define STDIO_LOCK(s)  fs_os_enter_crit_sect(&s->f_lock)
/** Release a lock */
#define STDIO_UNLOCK(s)  fs_os_leave_crit_sect(&s->f_lock)

/**
 * Holds current information of the file and buffer.
 *
 * For simplicity and maintainability the buffer we have is a linear buffer.
 * The buffered data always starts from the beginning at buffer [0], where the
 * underlying file position aligns with the buffer. The POSIX standard states
 * between writes/reads there has to be a flush or a seek operation. Therefore,
 * each time a seek or flush operations happens we reset the buffer and
 * continue as it were a clean buffer.
 *
 *  @code
 * |-------------------------- bufsize ------------------------------------|
 * |----------------- byte_count ------------------|
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*|*| | | | | | | | | | | | |
 *  ^               ^
 *  file_pos        curr_index
 *
 *
 * @endcode
 *
 * The buffer stores a continuous image of the file data. In this example the
 * application has asked to read 9 bytes, the STDIO layer then reads up to the
 * next block boundary. The file_pos indicates where the beginning buffer is
 * located with respect to the file. byte_count is the number of bytes
 * currently being stored in the buffer.
 *
 */

struct _efs_efile_io
{
  /** used to check for an initialized stream.    */
  uint32        stream_magic;
  /** Position in the file where the beginning of the buffer aligns with
      the file. */
  fs_size_t      file_pos;
  /** Indicates the number bytes in buf  */
  uint32         byte_count;
  /** Current Position to read and write data     */
  uint32         curr_index;
  /** Buffer size.                                */
  uint32         bufsize;
  /** Holds the temporary data for read/write.    */
  char         *buffer;
  /** File descriptor.                            */
  int           file_desc;
  /** Bitmask with markers for various states. See #define STREAM_MODE_* for
   *  list of available flags. */
  uint16         state;
  /** Set to efs_errno if there is an error on the stream.     */
  uint32         stream_errno;
  /** File descriptor critical section, Used to guarantee one thread is
      accessing the descriptor. */
  fs_os_crit_sect_type f_lock;
  /** Rollback file information. */
  struct efs_rbk_file_info rbk_info;
};

/** Holds what is the block size for the EFS file system */
static unsigned long efs_block_size;

/** Array of file descriptors. */
EFS_EFILE efs_fdescriptor_list[EFS_MAX_OPEN_STREAMS];

/** Critical section for efs_fdescriptor_list
 *
 * Critical section for the pool of file descriptors in the
 * list of efs_fdescriptor_list. This is used to take or return
 * a file descriptor from/to the pool.
 */
fs_os_crit_sect_type efs_fdescriptor_cs;

void
efs_estdio_init (void)
{
  int i;
  struct fs_statvfs efs_info;

  if (efs_statvfs ("/", &efs_info))
    FS_ERR_FATAL ("efs_estdio_init failed, efs_errno=%d.", efs_errno, 0, 0);

  efs_block_size = efs_info.f_bsize;

  /* Check if block size is power of two. */
  if ((efs_block_size & (efs_block_size - 1)) != 0)
    FS_ERR_FATAL ("estdio invalid block size of %d.", efs_block_size, 0, 0);

  fs_os_init_crit_sect (&efs_fdescriptor_cs);

  /* Initialize file descriptor pool. */
  memset (efs_fdescriptor_list, 0, sizeof (efs_fdescriptor_list));

  for (i = 0; i < EFS_MAX_OPEN_STREAMS; i++)
    STDIO_INIT_LOCK ((efs_fdescriptor_list + i));

}


#ifdef FS_UNIT_TEST
/**
 * Gets the rollback file descriptor
 *
 * It's only used on test cases to access the rollback file. Rollback calls
 * use efs_efget_rbk_file_fd() to get the file descriptor of the rollback file.
 * This call should not be used or added in any public context.
 *
 * @param stream File Handle that holds stream information
 *
 * @return File descriptor of rollback file. On error returns -1.
 */
int
efs_efget_rbk_file_fd (EFS_EFILE *stream)
{
  int filedesc = -1;

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (-1);
  }

  STDIO_LOCK (stream);

  if (stream->state == STREAM_INACTIVE
      || stream->stream_magic != ESTREAM_MAGIC)
    efs_errno = EBADF;
  else
    filedesc = stream->rbk_info.rbk_fd;

  STDIO_UNLOCK (stream);
  return (filedesc);
}
#endif /* FS_UNIT_TEST */


/** Parse the user passed file open mode.
 *
 *  This function takes the string used to indicate the open mode of a file and
 *  set the appropriate flags in oflag (for efs_open) and state parameters.
 *
 *  @param mode String of the user passed file open mode. Allowable values are:
 *              fist character has to be r, w or a. next three characters can
 *              be either b, + or u, + for read/write and u for rollback.
 *  @param oflag Used to return the appropriate flags for efs_open.
 *
 *  @param state State of the new file descriptor.
 *
 *  @return 0 on success, -1 on failure.
 *
 */
static int
parse_mode (const char *mode, int *oflag, uint16 *state)
{
  int has_plus = 0;
  int has_rollback = 0;
  char p;
  uint32 temp;

  if (mode == NULL || *mode == '\0')
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* Parse the mode string.  ANSI requires the first character to be one of
   * 'r', 'w', or 'a'. The string can contains a 'b' character (does
   * nothing) or plus sign to indicate update mode. We also added 'u'
   * character to indicate rollback. Since we can scan the mode string for '+'
   * and 'u' characters ignoring all others for portability cases. In four
   * characters we can contain all the modes e.g. "w+bu", so scan only 4. */
  for (temp = 1; temp < 4; temp++)
  {
    p = mode[temp];

    if (p == '\0')
      break;
    else
    {
      /* Check if character is a plus. */
      if (p == '+')
        has_plus = 1;

      /* Check if character is a u (rollback). */
      else if (p == 'u')
        has_rollback = 1;

      /* Ignore all other characters. */
    }
  }

  *oflag = 0;

  switch (*mode)
  {
    case 'r':
      *state = STREAM_MODE_READ;

      if (has_plus)
      {
        /* It's r+,  rb+ or r+b */
        *oflag = O_RDWR;
      }
      else
      {
        /* It's r or rb */
        *oflag = O_RDONLY;
      }
      break;

    case 'w':
      *state = STREAM_MODE_WRITE;

      if (has_plus)
      {
        /* It's w+,  wb+, w+b, wu+ or w+u */
        *oflag = O_RDWR | O_CREAT | O_TRUNC;
      }
      else
      {
        /* It's w, wb or wu */
        *oflag = O_WRONLY | O_CREAT | O_TRUNC;
      }
      break;

    case 'a':
      *state = STREAM_MODE_APPEND | STREAM_MODE_WRITE;

      if (has_plus)
      {
        /* it's a+, ab+, a+b */
        *oflag = O_RDWR | O_APPEND | O_CREAT;
      }
      else
      {
        /* it's a, ab */
        *oflag = O_WRONLY | O_APPEND | O_CREAT;
      }
      break;

    default:
      efs_errno = EINVAL;
      return -1;
  }

  temp = 0;

  if (has_plus)
    temp = STREAM_MODE_READ | STREAM_MODE_WRITE;

  if (has_rollback)
    temp |= STREAM_MODE_ROLLBACK;

  *state |= temp;

  return 0;
}

/** Gets File pointer from the pool of available file descriptors.
 *
 * It takes care of looking for an available file descriptor from the list
 * and returning it's pointer. A stream is available if the field 'state' is
 * zero, it's being used if it's different from zero. free_fpointer()
 * and alloc_fpointer() request a exclusive lock to change this field
 * from zero to different than zero an back.
 *
 * @return EFS_EFILE* Descriptor from the pool to use, NULL if none are
 *                   available.
 */
static EFS_EFILE *
alloc_fpointer (void)
{
  int i;
  EFS_EFILE *fp;

  fp = &efs_fdescriptor_list[0];

  fs_os_enter_crit_sect (&efs_fdescriptor_cs);

  for (i = 0; i < EFS_MAX_OPEN_STREAMS ; i++)
  {
  /* If 'state' is set to zero the stream is not being used, therefore it can
   * be allocated with an open call. If the 'state' is different than zero,
   * then the stream is being used, and 'state' indicates the status of the
   * stream, answering the following questions: Are we allowed to read? Are we
   * allowed to write? Is the stream buffered/unbuffered? Do we have rollback?
   * Are we in append mode?.
   */
    if (fp->state == STREAM_INACTIVE)
    {
      /* This file descriptor is available. Marked it as used. */
      fp->state = STREAM_ACTIVE;
      fp->stream_magic = ESTREAM_MAGIC;

      fp->file_desc = -1;
      fp->stream_errno = 0;
      fp->file_pos = 0;

      fp->buffer = NULL;
      fp->bufsize = 0;
      fp->byte_count = 0;
      fp->curr_index = 0;

      fp->rbk_info.rbk_file_nbr = 0;
      fp->rbk_info.rbk_fd = -1;

      break;
    }

    fp++;
  }

  fs_os_leave_crit_sect (&efs_fdescriptor_cs);

  if (i == EFS_MAX_OPEN_STREAMS)
  {
    /* did not find a descriptor available. */
    fp = NULL;
  }

  return fp;
}

/** Free a file descriptor from the pool.
 *
 * It takes an EFS_EFILE pointer and free it's resources and returns it to
 * the pool of available file descriptors. The upper layer takes care of
 * locking the stream. Free takes care of requesting the exclusive lock to
 * set the field 'state' to zero.
 *
 * @see alloc_fpointer()
 *
 * @param stream File pointer to free, return to the pool.
 */
static void
free_fpointer (EFS_EFILE * stream)
{
  ASSERT (stream != NULL);

  /* Enter critical section for returning a file descriptor to the pool */
  fs_os_enter_crit_sect (&efs_fdescriptor_cs);

  stream->stream_magic = 0;
  stream->file_desc = -1;
  stream->stream_errno = 0;
  stream->file_pos = 0;

  stream->buffer = NULL;
  stream->bufsize = 0;
  stream->byte_count = 0;
  stream->curr_index = 0;

  stream->rbk_info.rbk_file_nbr = 0;
  stream->rbk_info.rbk_fd = -1;

  /* Free resource by setting state to 0. */
  stream->state = STREAM_INACTIVE;

  fs_os_leave_crit_sect (&efs_fdescriptor_cs);
}

EFS_EFILE *
efs_efopen (const char *path, const char *mode)
{
  EFS_EFILE *fp;
  int oflag = 0;

  fp = alloc_fpointer ();

  if (fp == NULL)
  {
    /* There is no file pointer available from the pool. */
    efs_errno = EMFILE;
    return (NULL);
  }

  STDIO_LOCK (fp);

  /* Set the state for Read/Write/Append */
  if (parse_mode (mode, &oflag, &fp->state) != STREAM_INACTIVE)
  {
    efs_errno = EINVAL;
    goto error_case;
  }

  /* If needed, create rollback file. */
  if (HAS_ROLLBACK (fp) &&
      efs_rollback_create_rbk_file (path, & (fp->rbk_info)) == -1)
  {
    /* Could not create rollback file */
    efs_errno = EROLLBACK;
    goto error_case;
  }

  fp->file_desc = efs_open (path, oflag, DEFAULT_EUMASK);

  /* Check if efs_efopen failed */
  if (fp->file_desc < 0)
  {
    /* Open fail, remove rollback file (if created) */
    if (HAS_ROLLBACK (fp))
      efs_rollback_remove_rbk_file (&fp->rbk_info);

    goto error_case;
  }

  /* Get the current position, regardless if it's read/write/append */
  fp->file_pos = (uint32) efs_lseek64 (fp->file_desc, 0LL, SEEK_CUR);

  STDIO_UNLOCK (fp);
  return (fp);

error_case:
  free_fpointer (fp);
  STDIO_UNLOCK (fp);
  return (NULL);
}

EFS_EFILE *
efs_efdopen (int filedes, const char *mode)
{
  int oflag = 0;
  EFS_EFILE * fp;
  fp = alloc_fpointer ();

  if (fp == NULL)
  {
    efs_errno = EMFILE;
    return (NULL);
  }

  STDIO_LOCK (fp);

  /* Set object data. */
  fp->file_desc = filedes;

  if (parse_mode (mode, &oflag, &fp->state) != 0)
    goto error_case;

  /* Check Rollback flag. */
  if (HAS_ROLLBACK (fp))
  {
    /* Should not create rollback file when using efs_efdopen
     * we don't know if the file already has a rollback created with
     * an efs_efopen call. */
    goto error_case;
  }

  /* Get the current position, regardless if it's read/write/append */
  fp->file_pos = (uint32) efs_lseek64 (fp->file_desc, 0LL, SEEK_CUR);

  STDIO_UNLOCK (fp);
  return (fp);

error_case:
  free_fpointer (fp);
  efs_errno = EINVAL;
  STDIO_UNLOCK (fp);
  return (NULL);
}

/**
 * Reads data into the buffer.
 *
 * Calls efs_read() as many times as needed to read the requested data or error
 * occurs. It also takes care calling efs_lseek64() to change the file position
 * Any errors should be set by the underlying layer.
 *
 * @param data_out  Pointer to array to fill with read data.
 * @param bytes Number of bytes to read.
 * @param fd   File descriptor to read the data from.
 * @param file_pos Position to start reading from the file.
 *
 * @return Number of bytes read. Zero means error or end of file.
 */
static fs_size_t
read_from_device (char *data_out, fs_size_t bytes, int fd, fs_size_t file_pos)
{
  int nread;
  uint32 total = 0;

  /* Seek to the current file position */
  if (efs_lseek64 (fd, (fs_off64_t) file_pos, SEEK_SET) == -1)
    return (0);

  /* Loop until the requested data is read */
  do
  {
    nread = efs_read (fd, (void*) data_out, bytes);

    if (nread < 0)
      return (total);

    data_out += nread;
    bytes -= nread;
    total += nread;

  } while (bytes > 0 && nread != 0);

  return (total);
}

fs_size_t
efs_efread (void *ptr, fs_size_t size, fs_size_t nitems, EFS_EFILE *stream)
{
  fs_size_t bytes_to_read;
  fs_size_t total_read = 0;

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (0);
  }

  STDIO_LOCK (stream);

  /* Check if error and if the stream is active. */
  if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
      stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    goto fread_return;
  }

  /* Check if we are allowed to read the stream. */
  if (!OK_TO_READ (stream))
  {
    stream->stream_errno = EBADF;
    efs_errno = EBADF;
    goto fread_return;
  }

  /* If size is zero, return zero. */
  if (size == 0)
    goto fread_return;

  /* Calculate how many bytes to read */
  bytes_to_read = nitems * size;

  /* If the stream was opened in fully buffered mode.
   * (Line buffering not supported) */
  if (FULLY_BUFFERED (stream))
  {
    fs_size_t bytes_from_buffer;

    /* Point to latest data that we can read directly from the buffer */
    bytes_from_buffer = stream->byte_count - stream->curr_index;

    /* If buffer has some data, copy it */
    if (bytes_from_buffer > 0)
    {
      /* Read only as much we need from the buffer */
      if (bytes_from_buffer > bytes_to_read)
        bytes_from_buffer = bytes_to_read;

      fs_memscpy (ptr, bytes_to_read, stream->buffer + stream->curr_index,
                  bytes_from_buffer);
      stream->curr_index += bytes_from_buffer;
      bytes_to_read -= bytes_from_buffer;
      ptr = ((char*)ptr + bytes_from_buffer);
      total_read = bytes_from_buffer;
    }

    /* Check if we still need more data, then read it from FS */
    if (bytes_to_read > 0)
    {
      fs_size_t new_pos;
      fs_size_t bytes_to_buffer = 0;

      new_pos = stream->file_pos + stream->curr_index;

      /* Block align our request size, if our request automatically block
       * aligns then we don't have to fill the buffer */
      if (((new_pos + bytes_to_read) & (efs_block_size - 1)) != 0)
        bytes_to_buffer = ((new_pos + bytes_to_read) | (efs_block_size - 1))
                          + 1 - new_pos;

      /* Take the minimum between the number of bytes to read and bufsize */
      if (bytes_to_buffer > stream->bufsize)
        bytes_to_buffer = stream->bufsize;

      /* If the number of buffered bytes is going to be smaller than the limit
       * do not buffer. Buffer only when we will buffer twice of the bytes we
       * have to read from the device. This way, we service the request with
       * the buffered data and still have a significant portion to use on
       * future efs_efread() calls. This value was chosen after running
       * performance tests. */
      if ((bytes_to_buffer) >= (2 * bytes_to_read))
      {
        /* Buffer the data */
        bytes_to_buffer = read_from_device (stream->buffer, bytes_to_buffer,
                                            stream->file_desc, new_pos);

        /* If we got less than what we asked for, limit what we give */
        if (bytes_to_buffer < bytes_to_read)
          bytes_to_read = bytes_to_buffer;

        /* Copy the buffered data to the user array. */
        fs_memscpy (ptr, bytes_to_read, stream->buffer, bytes_to_read);

        /* Update the stream structure. */
        stream->file_pos = new_pos;
        stream->byte_count = bytes_to_buffer;
        stream->curr_index = bytes_to_read;

        total_read += bytes_to_read;
      }
      /* Else, don't copy to stdio buffer because there will not be enough
       * cached data to use later. */
      else
      {
        bytes_to_read = read_from_device (ptr, bytes_to_read,
                                          stream->file_desc, new_pos);
        stream->file_pos = new_pos + bytes_to_read;
        stream->byte_count = 0;
        stream->curr_index = 0;

        total_read += bytes_to_read;
      }
    }
  }
  else   /* Unbuffered mode. */
  {
    total_read = read_from_device (ptr, bytes_to_read, stream->file_desc,
                                   stream->file_pos);
    stream->file_pos += total_read;
  }

  total_read = total_read / size;

fread_return:
  STDIO_UNLOCK (stream);
  return (total_read);
}


/**
 * Writes data to a file and rollback file.
 *
 * It attempts to write all the data in the array pointed by ptr to the file in
 * stream->file_desc. It also updates the rollback file if needed and moves the
 * file position accordingly.
 *
 * @param ptr Pointer to data to be written
 * @param bytes Number of bytes to write.
 * @param stream File handle that holds the stream information.
 *
 * @return Number of bytes successfully written. Zero indicates error or
 *         file system is full.
 */
static fs_size_t
write_to_device (char *ptr, fs_size_t bytes, EFS_EFILE* stream)
{
  int result;
  fs_size_t total_bytes = 0;

  if (HAS_ROLLBACK (stream))
  {
    /* Rollback : Backup original file contents before overwrite */
    if (efs_rollback_write_req_to_orig_file (stream->rbk_info.rbk_fd,
                           stream->file_desc, bytes, stream->file_pos) == -1)
    {
      efs_errno = EROLLBACK;
      return 0;
    }
  }

  if (efs_lseek64 (stream->file_desc,
                   (fs_off64_t) stream->file_pos,
                   SEEK_SET) == -1)
    return 0;

  total_bytes = bytes;

  do
  {
    result = efs_write (stream->file_desc, ptr, bytes);

    if (result < 0)
      break;

    bytes -= result;
    ptr += result;
  } while (bytes > 0 && result != 0);

  return (total_bytes - bytes);
}


/**
 * Flushes a stream to the device. Does not do sanity check.
 *
 * This function is called only by stdio functions that already acquired
 * the lock on the stream. It assumes that all the standard checks on the
 * stream have been done, i.e. Stream is valid and writable.
 *
 * @param stream Pointer to the stream handle.
 *
 * @return Upon successful completion 0 is returned. Otherwise
 *         EEOF is return and efs_errno and the stream error
 *         are set to indicate the error.
 *
 * @see efs_efflush()
 * @see fflush_helper()
 */
static int
fflush_core (EFS_EFILE *stream)
{
  uint32 write_result;
  uint32 byte_count;

  byte_count = stream->byte_count;

  if (byte_count == 0)
    return 0;

  write_result = write_to_device (stream->buffer, byte_count, stream);

  /* Update the stream structure. */
  stream->file_pos += stream->curr_index;
  stream->byte_count = 0;
  stream->curr_index = 0;

  if (write_result == byte_count)
    return 0;

  return EEOF;
}

/**
 * Does all the necessary sanity checks on the stream and then flushes it.
 *
 * @param stream Pointer to the stream handle.
 *
 * @return Upon successful completion 0 is returned. Otherwise
 *         EEOF is return and efs_errno and the stream error
 *         are set to indicate the error.
 *
 * @see efs_efflush()
 * @see fflush_core()
 */
static int
fflush_helper (EFS_EFILE *stream)
{
  int result = EEOF;
  STDIO_LOCK (stream);

  /* Check if error and if the stream magic is valid in case somebody closed
   * the file. */
  if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
      stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    goto fflush_return;
  }

  /* Check if the file was open for writing */
  if (!OK_TO_WRITE (stream))
  {
    stream->stream_errno = EBADF;
    efs_errno = EBADF;
    goto fflush_return;
  }

  /* Check if in Unbuffered mode */
  if (!FULLY_BUFFERED (stream))
  {
    result = 0;
    goto fflush_return;
  }

  result = fflush_core (stream);

fflush_return:
  STDIO_UNLOCK (stream);
  return (result);
}

int
efs_efflush (EFS_EFILE *stream)
{
  /* If the stream is not NULL, proceed and flush the stream */
  if (stream != NULL)
  {
    return (fflush_helper (stream));
  }
  /* If the stream is NULL, flush all opened streams and return Success */
  else
  {
    int i;
    for (i = 0; i < EFS_MAX_OPEN_STREAMS; i++)
    {
      stream = &efs_fdescriptor_list[i];

      /* Flush the stream if writing is allowed. */
      if (OK_TO_WRITE (stream))
        fflush_helper (stream);
    }

    return (0);
  }
}

fs_size_t
efs_efwrite (void *ptr, fs_size_t size, fs_size_t nitems, EFS_EFILE *stream)
{
  fs_size_t bytes_to_write = 0;
  fs_size_t written_bytes = 0;
  fs_size_t written_items = 0;

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (0);
  }

  STDIO_LOCK (stream);

  /* Check if error and if the stream is active. */
  if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
      stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    goto fwrite_return;
  }

  /* Check if the file was open for reading. */
  if (!OK_TO_WRITE (stream))
  {
    stream->stream_errno = EBADF;
    efs_errno = EBADF;
    goto fwrite_return;
  }

  /* If size is zero, return zero. */
  if (size == 0)
    goto fwrite_return;

  bytes_to_write = nitems * size;

  /* If the stream was opened in fully buffered mode.
   * (Line buffering not supported) */
  if (FULLY_BUFFERED (stream))
  {
    uint32 bytes_to_buffer;
    /* Calculate the space available in the buffer */
    bytes_to_buffer = stream->bufsize - stream->curr_index;

    /* If the data fits in the buffer or the buffer has some data, fill the
     * buffer with data */
    if (bytes_to_buffer >= bytes_to_write || stream->byte_count != 0)
    {
      if (bytes_to_buffer > bytes_to_write)
        bytes_to_buffer = bytes_to_write;

      fs_memscpy (stream->buffer + stream->curr_index, bytes_to_buffer, ptr,
                  bytes_to_buffer);

      stream->byte_count += bytes_to_buffer;
      stream->curr_index += bytes_to_buffer;

      written_bytes = bytes_to_buffer;
      ptr = (((char*)ptr) + bytes_to_buffer);
      bytes_to_write -= bytes_to_buffer;

      /* If the buffer is full, flush it. */
      if (stream->byte_count == stream->bufsize)
      {
        if (fflush_core (stream))
        {
          /* flush failed, bail out */
          goto fwrite_item_return;
        }
      }
    }

    /* If there is no more data to write we are done. */
    if (bytes_to_write == 0)
      goto fwrite_item_return;

    /* Check if data fits in the buffer. */
    if (bytes_to_write >= stream->bufsize)
    {
      uint32 written;
      /* Data will not fit in the buffer. Write the rest of the request
       * to the device. */
      written = write_to_device (ptr, bytes_to_write, stream);

      stream->file_pos += written;
      written_bytes += written;
    }
    else
    {
      /* The data will fit in the buffer, copy it to the buffer. */
      fs_memscpy (stream->buffer, stream->bufsize, ptr, bytes_to_write);
      stream->byte_count += bytes_to_write;
      stream->curr_index += bytes_to_write;

      written_bytes += bytes_to_write;
    }
  }
  /* Unbuffered Mode */
  else
  {
    written_bytes = write_to_device (ptr, bytes_to_write, stream);
    stream->file_pos += written_bytes;
  }

fwrite_item_return:
  written_items = written_bytes / size;

fwrite_return:
  STDIO_UNLOCK (stream);
  return (written_items);
}

int
efs_efclose (EFS_EFILE *stream)
{
  int close_error = 0;

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (EEOF);
  }

  STDIO_LOCK (stream);

  if (stream->state == STREAM_INACTIVE
      || stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    STDIO_UNLOCK (stream);
    return EEOF;
  }

  /* If we are in buffered mode flush the buffer. */
  if (FULLY_BUFFERED (stream) && OK_TO_WRITE (stream))
  {
    if (fflush_core (stream))
      close_error = 1;
  }

  if (efs_close (stream->file_desc) != 0)
    close_error = 1;

  if (HAS_ROLLBACK (stream))
  {
    /* Rollback operation is completed for this file, so we shall remove the
     * rollback-file that we created to do rollback on this file. */
    efs_rollback_remove_rbk_file (&stream->rbk_info);
  }

  free_fpointer (stream);

  STDIO_UNLOCK (stream);

  if (close_error)
    return EEOF;

  return 0;
}

/* Helper function for performing seek. */
static int
efs_efseek_helper (EFS_EFILE *stream, fs_off64_t offset,
                   int whence, enum fs_restrict_api_to_gb restrict_to_gb)
{
  fs_off64_t new_file_pos = -1;
  fs_size_t file_pos;
  int result = -1;

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (-1);
  }

  if (offset > FS_MAX_4GB_VALUE || offset < FS_MIN_4GB_VALUE) {
    efs_errno = EINVAL;
    return (-1);
  }

  /* Lock pointer for editing */
  STDIO_LOCK (stream);

  if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
      stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    goto fseek_return;
  }

  /* If there is something in the buffer, flush it. */
  if (stream->byte_count > 0 && OK_TO_WRITE (stream))
      if (fflush_core (stream))
        goto fseek_return;

  if (whence == SEEK_END)
  {
    new_file_pos = efs_lseek64 (stream->file_desc, offset, whence);
    if (new_file_pos == (fs_off64_t) -1) {
      stream->stream_errno = efs_errno;
      goto fseek_return;
    }
  }
  else
  {
    if (whence == SEEK_SET)
      new_file_pos = offset;

    else if (whence == SEEK_CUR)
      new_file_pos = (fs_off64_t) stream->file_pos + \
                      stream->curr_index + offset;

    else
    {
      /* whence is not SEEK_CUR SEEK_END or SEEK_SET */
      efs_errno = EINVAL;
      goto fseek_return;
    }
  }

  /* Check if we have to restrict it to (2GB-1) bytes.
   * If new_file_pos is greater then FS_MAX_2GB_VALUE then return Error. */
  if ( (restrict_to_gb == FS_RESTRICT_API_TO_2GB) && \
       (new_file_pos > FS_MAX_2GB_VALUE) ) {
    efs_errno = EINVAL;
    goto fseek_return;;
  }

  /* Check for the underrun and overrun of the filesize
   * If new_file_pos lies between 0 and 4GB then only change stream file_pos
   * value or else throw error and set errno as EINVAL. */
  if ( (new_file_pos < 0) || (new_file_pos > FS_MAX_4GB_VALUE) ) {
    efs_errno = EINVAL;
    goto fseek_return;
  }

  /* Now the new_file_pos is : 0 <= new_file_pos < 4GB. So safe to cast. */
  file_pos = (fs_size_t) new_file_pos;

  /* if seek is inside buffer, adjust index. */
  if (!OK_TO_WRITE (stream) && stream->file_pos <= file_pos
      && file_pos < (stream->file_pos + stream->byte_count))
  {
    stream->curr_index = (fs_size_t) new_file_pos - stream->file_pos;
  }
  else
  {
    /* Else it's outside the buffer, reset buffer params. */
    stream->byte_count = 0;
    stream->curr_index = 0;
    stream->file_pos = (fs_size_t) new_file_pos;
  }

  result = 0;

fseek_return:
  STDIO_UNLOCK (stream);
  return result;
}

int
efs_efseek (EFS_EFILE *stream, long offset, int whence)
{
  int result = -1;
  fs_off64_t offset_arg;

  offset_arg = (fs_off64_t) offset;

  /* call th efs_efseek_helper function. */
  result = efs_efseek_helper (stream, offset_arg, whence,
                              FS_RESTRICT_API_TO_2GB);

  return result;
}

int
efs_efseek64 (EFS_EFILE *stream, fs_off64_t offset, int whence)
{
  return efs_efseek_helper (stream, offset, whence, FS_RESTRICT_API_TO_4GB);
}

/* Helper function for eftell api. */
static fs_off64_t
efs_eftell_helper (EFS_EFILE *stream,
                   enum fs_restrict_api_to_gb restrict_to_gb)
{
  fs_off64_t result = -1;

  /* Lock pointer for editing */
  STDIO_LOCK (stream);

  if (stream == NULL)
    efs_errno = EBADF;

  else
  {
    if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
        stream->stream_magic != ESTREAM_MAGIC)
      efs_errno = EBADF;
    else
      result = (fs_off64_t) stream->file_pos + (fs_off64_t) stream->curr_index;
  }

  /* Check if result is greater then 2GB and FS_RESTRICT_TO_2GB is set.
   * Then set errno and return failure. */
  if ((restrict_to_gb == FS_RESTRICT_API_TO_2GB) && \
      (result > FS_MAX_2GB_VALUE))
  {
    result = -1;
    efs_errno = EBADF;
  }

  /* If result is greater then 4GB, then set errno and return failure. */
  if (result > FS_MAX_4GB_VALUE)
  {
    result = -1;
    efs_errno = EBADF;
  }

  /* UnLock pointer */
  STDIO_UNLOCK (stream);
  return (result);
}

long
efs_eftell (EFS_EFILE *stream)
{
  fs_off_t result = -1;
  fs_off64_t ret_val = -1;

  ret_val = efs_eftell_helper (stream, FS_RESTRICT_API_TO_2GB);

  if ((ret_val >= 0) && (ret_val <= FS_MAX_2GB_VALUE))
    result = (fs_off_t) ret_val;

  return (result);
}

fs_off64_t
efs_eftell64 (EFS_EFILE *stream)
{
  fs_off64_t result = -1;

  result = efs_eftell_helper (stream, FS_RESTRICT_API_TO_4GB);

  return (result);
}

int
efs_esetvbuf (EFS_EFILE *stream, char *input_buf, int mode, fs_size_t size)
{
  int result = -1;

  /* Ensure that the size passed in is in the correct range.
   * We support only full buffering or no buffering.
   * (Line Buffering is not supported. */

  if (stream == NULL)
  {
    efs_errno = EBADF;
    return (-1);
  }

  if (mode != EFS_IOFBF && mode != EFS_IONBF && mode != EFS_IOLBF)
  {
    efs_errno = EINVAL;
    return (-1);
  }

  STDIO_LOCK (stream);

  if (stream->stream_errno != 0 || stream->state == STREAM_INACTIVE ||
      stream->stream_magic != ESTREAM_MAGIC)
  {
    efs_errno = EBADF;
    goto setvbuf_return;
  }

  if (FULLY_BUFFERED (stream) && fflush_core (stream))
  {
      /* efs_errno should be set by fflush_core */
      goto setvbuf_return;
  }

  /* if no buffer is given we treat it like No Buffering EFS_IONBF.
   */
  if (mode == EFS_IOFBF && input_buf != NULL)
  {
    stream->buffer = input_buf;
    stream->bufsize = size;

    stream->state |= STREAM_BUFFERED;
  }
  else
  {
    stream->buffer = NULL;
    stream->bufsize = 0;

    stream->state &= ~(STREAM_BUFFERED);
  }

  /* Reset the counters */
  stream->byte_count = 0;
  stream->curr_index = 0;

  result = 0;

setvbuf_return:
  STDIO_UNLOCK (stream);
  return (result);
}

int
efs_efileno (EFS_EFILE *stream)
{
  int filedesc = -1;

  if (stream == NULL)
    efs_errno = EBADF;

  else
  {
    STDIO_LOCK (stream);

    if (stream->state == STREAM_INACTIVE
        || stream->stream_magic != ESTREAM_MAGIC)
      efs_errno = EBADF;
    else
      filedesc = stream->file_desc;

    STDIO_UNLOCK (stream);
  }

  return (filedesc);
}
