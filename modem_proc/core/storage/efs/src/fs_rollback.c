/***********************************************************************
 * fs_rollback.c
 *
 * Rollback files, which were not closed.
 *
 * Copyright (C) 2006-2007, 2009-2010, 2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rollback.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $


when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-10-09    nr      Replace banned memory functions with safer versions.
2010-10-01    wek     Remove unnecessary asserts.
2009-05-18    rp      Fix compiler warning for 8200 which has gcc with more -Ws
2009-04-15    ebb     Removed from EFS protected segment.
2009-01-28    wek     Changed to only use EFS2 (no stdio calls).
2007-07-25    umr     Removed ASSERT orig_file_path_len which is always true.
2007-05-25    umr     Added support for EFS memory protection feature.
2007-03-02    umr     Updated copyright year.
2007-01-31    rp      Added rollback support for existing files.
2006-11-14    sh      Fix snprintf argument type warning.
2006-11-14    sh      Cleanup, warning fixes.
2006-11-02    rp      Created.

===========================================================================*/

#include "fs_rollback.h"
#include "fs_public.h"
#include "fs_pathbuff.h"
#include "assert.h"
#include <stdio.h>
#include <string.h>
#include "fs_privatedir.h"
#include "fs_errno.h"
#include "fs_util.h"

/* This is the rollback-folder where we will be storing the symbolic-links to
 * the files that we wish to rollback. */
#define FS_ROLLBACK_DIR_PATH FS_EFS_PRIVATE_DIR_PATH"/.rollback"

/* Max number of attempts that should be made to create rollback file-path */
#define EFS_MAX_ROLLBACK_LOOP_CNT 5

#ifdef FS_UNIT_TEST
/* We want these asserts to be effective only for simulator */
#define ROLLBACK_ASSERT(x)  ASSERT(x)
#else
#define ROLLBACK_ASSERT(x)
#endif /* FS_UNIT_TEST */

#ifdef FS_ROLLBACK_DEBUG
/* We want these asserts to be effective only for simulator */
#define ROLLBACK_DBG_ASSERT(x)  ASSERT(x)
#else
#define ROLLBACK_DBG_ASSERT(x)
#endif /* FS_UNIT_TEST */

/* All rollback-files will start with this string, so this string shall help
   us to identify if a given file is indeed a rollback file or not */
static const char fs_rollback_file_magic_string[] = "R/O/L/L/B/A/C/K/";

/* Major and Minor version numbers of a rollback-file, each 2-bytes long */
static const uint16 fs_rollback_file_hdr_major_version = 0x0001;
static const uint16 fs_rollback_file_hdr_minor_version = 0x0000;

/* offset of the Last-Write-Record field in the rollback file header */
#define EFS_ROLLBACK_LAST_WRITE_REC_FIELD_OFFSET ( \
    sizeof (g_fs_rbk_file.hdr.magic_str) + \
    sizeof (fs_rollback_file_hdr_major_version) + \
    sizeof (fs_rollback_file_hdr_minor_version) )


/*=============================================================================
  FUNCTION      efs_rollback_make_rbk_file_path

  DESCRIPTION   This function forms a file-path using given symlink_nbr as:
                Ex: /.efs_private/.rollback/00000007.rbk
  PARAMETERS
    file_path_ptr <- [OUT]
                  The created file-path will be stored here.

    symlink_nbr -> [IN]
                  The 8 hexadecimal digits in the file-path.

  RETURN VALUE  None.
=============================================================================*/
static void
efs_rollback_make_rbk_file_path (char *file_path_ptr, uint32 file_nbr)
{
  (void)snprintf (file_path_ptr, FS_PATH_MAX,
      FS_ROLLBACK_DIR_PATH "/%08lX.rbk", file_nbr);
}

/*=============================================================================
  FUNCTION      efs_rollback_gen_file_nbr

  DESCRIPTION

  This function generates a file-number which is used to form the
  rollback file-name. When name-collision occurs, this function attempts
  the configured number of times to generate the file-number

  Return value : 0 on failure, positive-number on success. This
                 positive-number can be used as rollback file-name
=============================================================================*/
static uint32
efs_rollback_gen_file_nbr (void)
{
  static uint32 curr_file_nbr = 0;
  struct fs_stat fstat;
  unsigned int max_tries = EFS_MAX_ROLLBACK_LOOP_CNT;

  do {
    /* Increment the current file number */
    ++curr_file_nbr;

    /* 0 :-> 0 is reserved to indicate NO-ROLLBACK, so we shall avoid 0 */
    if (curr_file_nbr == 0)
      curr_file_nbr = 1;

    /* make a file-path with this random-number */
    efs_rollback_make_rbk_file_path (fs_path_buff[0], curr_file_nbr);

    /* See if we can create a file using this number */
    if (efs_lstat (fs_path_buff[0], &fstat) != 0)
      break;

  } while (max_tries--);

  if (max_tries == 0) {
    /* Couln't create rbk_file_nbr, so we shall return 0 to indicate failure*/
    return 0;
  }

  return curr_file_nbr;
}


/*=============================================================================
FUNCTION      efs_rollback_write_rbk_file_hdr

DESCRIPTION   This function writes the rollback-file header. The file-format
              of the header is described in the .h file.
PARAMETERS
    rbk_fd <- [IN] : File descriptor of the rollback file.
    file_path <- [IN] : Original file path supplied by user.
    fstat_ptr <- [IN] : fstat of the original user-supplied file.

RETURN VALUE : [-1] if error. [ 0]  if the header is written successfully.

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static int
efs_rollback_write_rbk_file_hdr (int rbk_fd, const char *file_path,
  const struct fs_stat *fstat_ptr)
{
  /* seek to the beginning of the rollback-file */
  if (efs_lseek (rbk_fd, 0, SEEK_SET) == -1) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* initialize the file-header struct to all 0's */
  memset (&g_fs_rbk_file.hdr, 0, sizeof (g_fs_rbk_file.hdr));

  /* Copy the magic-string that identifies rollback file */
  fs_memscpy (g_fs_rbk_file.hdr.magic_str,
              sizeof (g_fs_rbk_file.hdr.magic_str),
              fs_rollback_file_magic_string,
              sizeof (g_fs_rbk_file.hdr.magic_str));

  /* Copy the major version-number for the rollback-file */
  g_fs_rbk_file.hdr.major_version = fs_rollback_file_hdr_major_version;

  /* Copy the minor version-number for the rollback-file */
  g_fs_rbk_file.hdr.minor_version = fs_rollback_file_hdr_minor_version;

  /* write the last-write-record-offset as 0. 0 :-> no means no-records */
  g_fs_rbk_file.hdr.last_write_rec_offset = 0;

  /* Record the original file size, before any modification happens to it */
  g_fs_rbk_file.hdr.orig_file_size = fstat_ptr->st_size;

  /* Record the a-time */
  g_fs_rbk_file.hdr.a_time = fstat_ptr->st_atime;

  /* Record the m-time */
  g_fs_rbk_file.hdr.m_time = fstat_ptr->st_mtime;

  /* Record the original file-path-length. */
  g_fs_rbk_file.hdr.orig_file_path_len = (uint16)strlen (file_path);

  /* write the rbk header onto the rollback-file. */
  if (efs_write (rbk_fd, &g_fs_rbk_file.hdr, sizeof (g_fs_rbk_file.hdr)) !=
        sizeof (g_fs_rbk_file.hdr)) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* Write the original file-path onto the rollback-file. */
  if (efs_write (rbk_fd, file_path, g_fs_rbk_file.hdr.orig_file_path_len) !=
        g_fs_rbk_file.hdr.orig_file_path_len) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  return 0;

Error:
  return -1;
}

int
efs_rollback_create_rbk_file (const char *file_path,
  struct efs_rbk_file_info *rbk_info_ptr)
{
  uint32 rbk_file_nbr = 0;
  struct fs_stat orig_fstat;
  boolean new_file = FALSE;
  int result;
  char *rbk_file_path = fs_path_buff[0];

  FS_PATH_BUFF_LOCK ();

  /* the rollback-file is a symlink, if orig-file is a new-file */
  rbk_info_ptr->rbk_fd = -1;

  /* check if the user-supplied orig-file exists */
  result = efs_lstat (file_path, &orig_fstat);
  if ( (result == -1) && (efs_errno == ENOENT) ) {
    new_file = TRUE;
  }

  /* generate a file-name for the rollback-file */
  rbk_file_nbr = efs_rollback_gen_file_nbr ();
  if (rbk_file_nbr == 0) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* make a rbk-file-path with this random-number */
  efs_rollback_make_rbk_file_path (rbk_file_path, rbk_file_nbr);

  if (new_file) {
    /* create the rollback-file as a symbolic-link to the original-file */
    if (efs_symlink (file_path, rbk_file_path) == -1) {
      goto Error;
    }
  }
  else {
    /* create the rollback-file as a normal-file which will hold original
       file contents */
    rbk_info_ptr->rbk_fd = efs_open (rbk_file_path, O_CREAT | O_RDWR, 0666);
    if (rbk_info_ptr->rbk_fd == -1) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* write the rollback file header */
    if (efs_rollback_write_rbk_file_hdr (rbk_info_ptr->rbk_fd, file_path,
          &orig_fstat) == -1) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }
  }

  rbk_info_ptr->rbk_file_nbr = rbk_file_nbr;

  FS_PATH_BUFF_UNLOCK ();
  return 0;

Error:
  if (rbk_file_nbr != 0) {
    (void)efs_unlink (rbk_file_path);
  }
  FS_PATH_BUFF_UNLOCK ();
  return -1;
}

void
efs_rollback_remove_rbk_file (const struct efs_rbk_file_info *rbk_info_ptr)
{
  char *file_path_ptr = fs_path_buff[0];

  /* Grab a lock to the file-path-buffer arrays */
  FS_PATH_BUFF_LOCK ();

  /* make a file-path with this rollback-number */
  efs_rollback_make_rbk_file_path (file_path_ptr, rbk_info_ptr->rbk_file_nbr);

  /* if the rollback-file is not a symolic-link then close the file before
     deleting it. */
  if (rbk_info_ptr->rbk_fd != -1) {
    (void)efs_close (rbk_info_ptr->rbk_fd);
  }

  /* delete the symlink file */
  (void)efs_unlink (file_path_ptr);

  /* Release the lock to the file-path-buffer arrays */
  FS_PATH_BUFF_UNLOCK ();
}

/*=============================================================================
FUNCTION      efs_rollback_block_copy

DESCRIPTION   This function does file-copy. The function copies bytes from the
              source-file onto the destination-file.
PARAMETERS
  [ src_fstream ] ->  source-file stream.
  [ dest_fd ]     <-  dest-file descriptor.
  [ src_offset ]  ->  offset from where we should read.
  [ dest_offset ] ->  offset from where we should write.
  [ write_size ]  ->  how many bytes to copy?

RETURN VALUE
    [ FALSE ]   on error.
    [ TRUE  ]   on success, when all the bytes are copied.

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static boolean
efs_rollback_block_copy (int src_fd, int dest_fd,
  fs_off_t src_offset, fs_off_t dest_offset, fs_size_t write_size)
{
  uint8 *src_data_ptr = fs_temp_buff[0];
  fs_ssize_t curr_write_size;
  fs_ssize_t write_result;

  /* seek to the desired file-offsets */
  if (efs_lseek (src_fd, src_offset, SEEK_SET) == -1) {
    ROLLBACK_ASSERT(FALSE);
    goto Error;
  }
  if (efs_lseek (dest_fd, dest_offset, SEEK_SET) != dest_offset) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* loop and copy all the bytes from the src-file to the dest-file, but
     restrict each write to the max available temp-buffer size */
  while (write_size > 0) {
    /* check if we can write the remaining bytes in one shot */
    if (write_size > sizeof (fs_temp_buff[0])) {
      /* Remaining bytes exceed our buffer-size, so limit to max buffer-size */
      curr_write_size = sizeof (fs_temp_buff[0]);
    }
    else {
      /* Remaining bytes can fit well in our buffer */
      curr_write_size = write_size;
    }

    /* Read so-many bytes from the source-file */
    curr_write_size = efs_read (src_fd, src_data_ptr, curr_write_size);
    if (curr_write_size <= 0)
    {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* Write the so-many bytes to the destination-file */
    write_result = efs_write (dest_fd, src_data_ptr, curr_write_size);
    if (write_result < 0) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }
    if (write_result != curr_write_size) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* decrease write_size, with how many ever bytes we have just wrote */
    write_size -= curr_write_size;
  }

  return TRUE;

Error:
    return FALSE;
}

/*=============================================================================
FUNCTION      efs_rollback_write_write_record

DESCRIPTION   This function writes the write-record. The file-format
              of the Write-Record is descibed in the .h file.

PARAMETERS
  [ rbk_fd ]      ->  Rollback file's descriptor.
  [ prev_write_rec_offset ]
                  ->  Previous write-record's offset. The new-record will point
                      to this given offset.
  [ orig_file_offset ]
                  ->  Offset of original-file, where bytes will change.
  [ rec_size ]    ->  How many bytes will change?
  [ orig_fstream ]->  Original-File's stream.

RETURN VALUE
    [-1]  if error.
    [ 0]  0 on success, if the write-record is written successfully.

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static int
efs_rollback_write_write_record (int rbk_fd, fs_size_t prev_write_rec_offset,
  long orig_file_offset, fs_size_t rec_size, int orig_fd)
{
  fs_off_t latest_write_rec_offset;
  fs_off_t rbk_offset;

  /* seek to the end of the rollback-file and remember the offset for this
     write-record. */
  latest_write_rec_offset = efs_lseek (rbk_fd, 0, SEEK_END);
  if ( latest_write_rec_offset  == -1 ) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* Prepare the write-record header */
  g_fs_rbk_file.write_rec_hdr.prev_write_rec_offset = prev_write_rec_offset;
  g_fs_rbk_file.write_rec_hdr.orig_file_offset = orig_file_offset;
  g_fs_rbk_file.write_rec_hdr.write_size = rec_size;

  /* Write the Write-Record header. */
  if (efs_write (rbk_fd, &g_fs_rbk_file.write_rec_hdr,
        sizeof (g_fs_rbk_file.write_rec_hdr)) !=
        sizeof (g_fs_rbk_file.write_rec_hdr) ) {
    /* Couldn't write Record-Header, so we shall bail */
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* Adjust the rollback file's offset for the above write. */
  rbk_offset = latest_write_rec_offset + sizeof (g_fs_rbk_file.write_rec_hdr);

  /* now we shall backup the orig-file contents onto the rollback-file */
  if (efs_rollback_block_copy (orig_fd, rbk_fd, orig_file_offset, rbk_offset,
                               rec_size) == FALSE) {
    ROLLBACK_ASSERT(FALSE);
    goto Error;
  }

  /* Now update the last-write-rec-offset in the file-header to this
     latest Write-Record. */

  /* In this rollback-file, seek to the last-write-rec field in the header */
  if ( efs_lseek (rbk_fd, EFS_ROLLBACK_LAST_WRITE_REC_FIELD_OFFSET,
        SEEK_SET) == -1 ) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* Update the last-write-record-offset in the file-header. */
  if (efs_write (rbk_fd, &latest_write_rec_offset, sizeof(fs_size_t)) !=
        sizeof(fs_size_t)) {
    /* error while updating rollback-file , so we shall bail */
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  return 0;

Error:
  return -1;
}

int
efs_rollback_write_req_to_orig_file (int rbk_fd, int orig_fd, fs_size_t size,
                                     fs_size_t filepos)
{
  fs_size_t orig_file_offset = filepos;
  fs_size_t total_write_size = size;
  fs_size_t backup_bytes_size;
  const int two_field_size = (sizeof (g_fs_rbk_file.hdr.last_write_rec_offset)
                                + sizeof (g_fs_rbk_file.hdr.orig_file_size));

  FS_PATH_BUFF_LOCK ();

  /* check if this rollback-file is a symlink */
  if (rbk_fd == -1) {
    /* Rollback file is a symlink which means the original file is a new file,
       so nothing to back-up, we shall bail */
    goto Success;
  }

  /* In this rollback-file, seek to the last-write-rec field */
  if ( efs_lseek (rbk_fd, EFS_ROLLBACK_LAST_WRITE_REC_FIELD_OFFSET,
        SEEK_SET) == -1 ) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* prepare the global rollback struct instance */
  memset (&g_fs_rbk_file, 0, sizeof (g_fs_rbk_file));

  /* Now read the last-write-rec and orig-file-size fields from rbk file */
  if ( efs_read (rbk_fd, &g_fs_rbk_file.hdr.last_write_rec_offset,
        two_field_size) != two_field_size) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* if the curr_seek_posn exceeds the orig-file-size then no need to backup */
  if ( (uint32)orig_file_offset >= g_fs_rbk_file.hdr.orig_file_size) {
    goto Success;
  }

  /* compute how many bytes do we need to backup from original file */
  if ( (orig_file_offset + total_write_size) <=
          g_fs_rbk_file.hdr.orig_file_size ) {
    /* write is happening within old file's boundary, so backup full write */
    backup_bytes_size = total_write_size;
  }
  else {
    /* write is overlapping old-file's and new-files boundary, but we shall
       only backup the old-file's affected bytes */
    backup_bytes_size = g_fs_rbk_file.hdr.orig_file_size - orig_file_offset;
  }

  /* Write the write-Record */
  if (efs_rollback_write_write_record (rbk_fd,
        g_fs_rbk_file.hdr.last_write_rec_offset, orig_file_offset,
        backup_bytes_size, orig_fd) == -1) {
    ROLLBACK_ASSERT(FALSE);
    goto Error;
  }

  /* restore the seek-position of the orig file. */
  if (efs_lseek (orig_fd, orig_file_offset, SEEK_SET)==-1 ){
    ROLLBACK_ASSERT(FALSE);
    goto Error;
  }

  goto Success;

Success:
  FS_PATH_BUFF_UNLOCK ();
  return 0;

Error:
  FS_PATH_BUFF_UNLOCK ();
  return -1;
}

/*=============================================================================
FUNCTION      efs_rollback_open_rbk_file_and_verify_hdr

DESCRIPTION   This function opens the given rollback-file, reads the header and
              verifies if the header-information is intact or not.

PARAMETERS
  [ rbk_file_path ] ->  Rollback file's path.

RETURN VALUE
    [-1]        if error.
    [file desc] Rollback-file's file-stream on success.

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static int
efs_rollback_open_rbk_file_and_verify_hdr (const char *rbk_file_path_ptr)
{
  int rbk_fd;

  /* Open the rbk file for reading */
  rbk_fd = efs_open (rbk_file_path_ptr, O_RDONLY);
  if (rbk_fd == -1) {
    ROLLBACK_ASSERT(FALSE);
    goto Error;
  }

  /* Read the file header */
  if (efs_read (rbk_fd, &g_fs_rbk_file.hdr, sizeof (g_fs_rbk_file.hdr)) !=
      sizeof (g_fs_rbk_file.hdr)) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* Make sure the rollback has the magic string intact */
  if (strncmp (g_fs_rbk_file.hdr.magic_str, fs_rollback_file_magic_string,
        sizeof (g_fs_rbk_file.hdr.magic_str)) != 0) {
    /* Magic string is missing, so lets not work on this file */
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* Now check for the major version */
  if (g_fs_rbk_file.hdr.major_version != fs_rollback_file_hdr_major_version) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* Now check for the minor version */
  if (g_fs_rbk_file.hdr.minor_version != fs_rollback_file_hdr_minor_version) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* Do not vaildate last_write_rec_offset.
     g_fs_rbk_file.hdr.last_write_rec_offset could be '0' and the orig-file
     could still have changed (file grown big), so in this case we still
     want to rollback and truncate the file it its previous orig size. */

  /* sanity check on the original file-path length */
  if ( (g_fs_rbk_file.hdr.orig_file_path_len == 0) ||
       (g_fs_rbk_file.hdr.orig_file_path_len >= sizeof (fs_path_buff[0])) ) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  /* If we get till here, then the header is good */
  return rbk_fd;

Error:
  if (rbk_fd != -1)
    (void)efs_close (rbk_fd);
  return -1;
}

/*=============================================================================
FUNCTION      efs_rollback_open_orig_file_and_verify

DESCRIPTION   This function reads the orig-file-path from the rollback-file,
              opens the orig-file.

PARAMETERS
  [ rbk_fd ] ->  Rollback file's file descriptor.

RETURN VALUE
    [-1]         if error.
    [file desc]  Orig-file's file-descriptor on success.

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static int
efs_rollback_open_orig_file_and_verify (int rbk_fd)
{
  int orig_fd = -1;
  struct fs_stat orig_fstat;

  /* Read the original file path */
  if (efs_read (rbk_fd, g_fs_rbk_file.orig_file_path,
                g_fs_rbk_file.hdr.orig_file_path_len)
      != g_fs_rbk_file.hdr.orig_file_path_len) {
    /* error while reading original file-path, so we shall bail */
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }
  /* Append '\0' to terminate the file-path string */
  g_fs_rbk_file.orig_file_path[g_fs_rbk_file.hdr.orig_file_path_len] = '\0';

  /* get stats on the original file */
  if (efs_lstat (g_fs_rbk_file.orig_file_path, &orig_fstat) == -1) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* if the orig file has grown, then we shall truncate to its previous size */
  if (orig_fstat.st_size != g_fs_rbk_file.hdr.orig_file_size) {
    if (efs_truncate (g_fs_rbk_file.orig_file_path,
      g_fs_rbk_file.hdr.orig_file_size) != 0) {
      goto Error;
    }
  }

  /* open the original file in read-write mode */
  orig_fd = efs_open (g_fs_rbk_file.orig_file_path, O_RDWR);
  if (orig_fd == -1) {
    ROLLBACK_ASSERT (FALSE);
    goto Error;
  }

  return orig_fd;

Error:
  if (orig_fd != -1)
    (void)efs_close (orig_fd);
  return -1;
}

/*=============================================================================
FUNCTION      efs_rollback_playback

DESCRIPTION   This function recovers the original-file from the information
              obtained from the rollback file.
              1. Open the rollback-file, and verify file-header.
              2. Open the orig-file, if needed truncate it to its prev-size.
              3. starting from the last-write, work-backward until the first-
                 write and restore the data from the rollback-file back onto
                 the original-file.

PARAMETERS
  [ rbk_file_path ]
          ->  Rollback file's file-path.
  [ rbk_fstat_ptr ]
          ->  Rollback file's file-stat's. We need this to verify that the
              rollback-file atleast has file-header in it.

RETURN VALUE  : None

DEPENDENCIES
    It is assumed that the caller of this function has acquired
    FS_PATH_BUFF_LOCK() as this function uses global g_fs_rbk_file.
=============================================================================*/
static void
efs_rollback_playback (const char *rbk_file_path,
  struct fs_stat *rbk_fstat_ptr)
{
  int rbk_fd = -1;
  int orig_fd = -1;
  uint32 prev_write_rec_offset;
  fs_off_t rbk_offset;

  /* make sure that the rollback-file file, atleast has file header */
  if (rbk_fstat_ptr->st_size < sizeof (g_fs_rbk_file.hdr) ) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* prepare the global rollback struct instance */
  memset (&g_fs_rbk_file, 0, sizeof (g_fs_rbk_file));
  g_fs_rbk_file.orig_file_path = fs_path_buff[1]; /* [0] is in use */

  /* Check if the rollback's file header is ok? */
  rbk_fd = efs_rollback_open_rbk_file_and_verify_hdr(rbk_file_path);
  if (rbk_fd == -1) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* From the file-header, read orig-file-path and verify orig-file. */
  orig_fd = efs_rollback_open_orig_file_and_verify(rbk_fd);
  if (orig_fd == -1) {
    ROLLBACK_DBG_ASSERT (FALSE);
    goto Error;
  }

  /* Lets begin with the last-write-record. */
  prev_write_rec_offset = g_fs_rbk_file.hdr.last_write_rec_offset;

  /* loop through all write-records and recover. */
  while (prev_write_rec_offset != 0) {
    /* seek to this write-record offset in the rollback-file */
    if (efs_lseek (rbk_fd, prev_write_rec_offset, SEEK_SET) == -1) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* Read this write-record's header */
    if ( efs_read (rbk_fd, &g_fs_rbk_file.write_rec_hdr,
                   sizeof (g_fs_rbk_file.write_rec_hdr)) !=
         sizeof (g_fs_rbk_file.write_rec_hdr) ) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* sanity-check on the previous-write record offset */
    if (g_fs_rbk_file.write_rec_hdr.prev_write_rec_offset >=
        prev_write_rec_offset) {
      /* This should not have happened, We are working from the last write-
         record in the file from the bottom and working towards the beginning
         of the file to locate previous write-records. Now we have entered a
         corrupt write-record, so we shall not work on this file any further.*/
      ROLLBACK_DBG_ASSERT (FALSE);
      goto Error;
    }

    /* Adjust the rollback-file-offset for the record-header, which we
       read above. */
    rbk_offset = prev_write_rec_offset + sizeof (g_fs_rbk_file.write_rec_hdr);

    /* Restore the bytes back to the orig-file from rollback-file. */
    if (efs_rollback_block_copy (rbk_fd, orig_fd, rbk_offset,
          g_fs_rbk_file.write_rec_hdr.orig_file_offset,
          g_fs_rbk_file.write_rec_hdr.write_size) == FALSE) {
      ROLLBACK_ASSERT (FALSE);
      goto Error;
    }

    /* now we shall proceed with the previous write-record */
    prev_write_rec_offset = g_fs_rbk_file.write_rec_hdr.prev_write_rec_offset;
  }

  /* Orig-File successfully restored. */

Error:
  if (rbk_fd != -1) {
    (void)efs_close (rbk_fd);
  }
  if (orig_fd != -1) {
    (void)efs_close (orig_fd);
  }
  return;
}

void
efs_rollback_recover (void)
{
  EFSDIR *dir_iter_ptr;
  struct fs_dirent *dir_entry_ptr;
  int path_len = 0;
  struct fs_stat fstat;
  int result;

  /* Attempt to create the /.rollback/ directory */
  (void) efs_unlink (FS_ROLLBACK_DIR_PATH);
  result = efs_mkdir (FS_ROLLBACK_DIR_PATH, 0755);
  if (result < 0 ) {
    if (efs_errno != EEXIST) {
      /* Directory did not exist, but still we couldn't create it */
      ROLLBACK_ASSERT (FALSE);
      return;
    }
  }

  /* Prepare to iterate through all the files in this directory */
  dir_iter_ptr = efs_opendir (FS_ROLLBACK_DIR_PATH);
  if (dir_iter_ptr == NULL) {
    ROLLBACK_ASSERT (FALSE);
    return;
  }

  /* Grab the lock to the file-path-buffer arrays */
  FS_PATH_BUFF_LOCK ();

  /*---------------------------------------------------------------------------
    Loop through each file in the /.rollback/ directory and:
    1. If the rollback-file is a symbolic-link then delete the file that this
       symbolic-link points to.
    2. If the rollback-file is normal file, then playback the rollback-file to
       recover the original file.
    3. Delete the rollback-file.
    -------------------------------------------------------------------------*/
  while (1) {
    /* Get the next file in this directory */
    dir_entry_ptr = efs_readdir (dir_iter_ptr);

    /* If there are no more files, then we shall bail */
    if (dir_entry_ptr == NULL)
      break;

    path_len = strlen (dir_entry_ptr->d_name);

    /* skip the '.' directory */
    if ((path_len == 1) && (dir_entry_ptr->d_name[0] == '.') )
      continue;

    /* skip the '..' directory */
    if ((path_len == 2) && (dir_entry_ptr->d_name[0] == '.')
         && (dir_entry_ptr->d_name[1] == '.'))
      continue;

    /* resolve full-path of the rollback-file */
    (void)snprintf (fs_path_buff[0], sizeof (fs_path_buff[0]),
              FS_ROLLBACK_DIR_PATH "/%s", dir_entry_ptr->d_name);

    /* Get the stats on this file */
    result = efs_lstat (fs_path_buff[0], &fstat);
    if (result < 0) {
      /* Couldn't stat the file, so delete the symlink */
      (void) efs_unlink (fs_path_buff[0]);
      continue;
    }

    /* If it is a symbolic-link, then delete the file it points to */
    if (S_ISLNK (fstat.st_mode)) {
      /* Original-file is a new-file, so resolve the symbolic-link to locate
         the original file-path, and delete the original-file. */
      memset (fs_path_buff[1], '\0', sizeof (fs_path_buff[1]));
      result = efs_readlink (fs_path_buff[0], fs_path_buff[1],
                             FS_PATH_MAX);
      if (result != -1) {
        /* Delete the actual file */
        (void) efs_unlink (fs_path_buff[1]);
      }
    }
    else {
      /* Original-file was already existing, so using the the rollback-file
         recover the original-file. */
      efs_rollback_playback (fs_path_buff[0], &fstat);
    }

    /* Delete the rollback file */
    (void) efs_unlink (fs_path_buff[0]);
  }

  /* Release the lock to the file-path-buffer arrays */
  FS_PATH_BUFF_UNLOCK ();

  (void)efs_closedir (dir_iter_ptr);
}

