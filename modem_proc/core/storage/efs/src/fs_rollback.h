/***********************************************************************
 * fs_rollback.h
 *
 * File Rollback
 * Copyright (C) 2006-2007, 2009-2010 QUALCOMM Technologies, Inc.
 *
 * Rollback files, if not closed.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rollback.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-10-01   wek   Added documentation about assumptions in rollback.
2009-01-28   wek   Changed to only use EFS2 (no stdio calls).
2007-03-02   umr   Updated copyright year.
2007-03-01    rp   Moved efs_fget_rbk_file_fd() proto from stdio.h file to here
2007-01-31    rp   Added rollback support for existing files.
2006-11-02    rp   Created.

===========================================================================*/

#ifndef _FS_ROLLBACK_H
#define _FS_ROLLBACK_H

#include "comdef.h"
#include "fs_sys_types.h"

/******************************************************************************

Original-File : User's file for which rollback service is intended.
Rollback-File : Internal file for supporting rollback.

There are 2-scenarios as far as rollback is concerned:-
  1. New-files or
  2. Existing-Files.

New Files :-
---------
If the original-file is a new-file, then the rollback-file is just a
symbolic-link to the original-file.

Existing Files :-
--------------
if the original-file is already an existing file, then the rollback-file is a
binary file which will hold the changing bytes of the original file. In this
case, the file-format of the rollback is as described below:


File-Format of the Rollback-File :-
--------------------------------

      |-----------------------|
      |                       |
      | File-Header           | Header Size = 30-Bytes + (Orig-File-Path)
      |                       |
      |-----------------------|
      |                       |
      | 0 or n write-records  | Write-Record-Size =
      |                       |   ( Write-Record-Header-Size [ 12-bytes ] +
      |-----------------------|     m-bytes of orig-file-contents )


Rollback module assumes that each individual element (file header excluding
file path, original file path and write record header) can be written in
a single efs_write call. In other words that they are less than the maximum
transaction size.

Format of File-Header :-
---------------------

      |-----------------------|
      |                       |
      | magic string          | 16 bytes.  R/O/L/L/B/A/C/K/
      |                       |
      |-----------------------|
      | major version         | 2 bytes.
      |-----------------------|
      | minor version         | 2 bytes.
      |-----------------------|
      | last_write_rec offset | 4 bytes. Latest(Last) write record offset.
      |-----------------------|
      | orig file size        | 4 bytes. Original file size.
      |-----------------------|
      | a-time                | 4 bytes. last-access-time (only backedup)
      |-----------------------|
      | m-time                | 4 bytes. Last-modification-time (only backedup)
      |-----------------------|
      | reserved              | 2 bytes. to avoid-packed structures.
      |-----------------------|
      | orig file path length | 2 bytes. Original file path length.
      |-----------------------|
      |                       |
      |  File Path            | m-bytes. Original file-path.
      |                       | size is indicated by the previous field.
      |                       |
      |-----------------------|
      Header Size = 40-Bytes + (m-bytes of File-Path)


Format of a Write-Record :-
------------------------

      |-----------------------|
      | prev-write-rec-offset | 4 bytes. Where is previous write record?
      |-----------------------|
      | orig file offset      | 4 bytes. (Offset, where orig file changed?)
      |-----------------------|
      | size                  | 4 bytes. How big is this backup'ed block.
      |-----------------------|
      |                       |
      |  Orig file contents   | m-bytes. Original file-bytes backed-up.
      |                       |
      |                       |
      |-----------------------|
      Write-Record_Header-Size = 12-Bytes
      Write-Record-Size = ( Write-Record-Header-Size [ 12-bytes ] +
                            m-bytes of orig-file-contents )

******************************************************************************/
struct fs_rollback_file_info {
  /* Rollback-File-Header */
  struct hdr {
    char            magic_str[16];
    uint16          major_version;
    uint16          minor_version;
    uint32          last_write_rec_offset;
    uint32          orig_file_size;
    uint32          a_time;
    uint32          m_time;
    uint16          reserved_1;
    uint16          orig_file_path_len;
  }hdr;
  char              *orig_file_path;

  /* A Write-Record */
  struct write_rec_hdr {
    uint32            prev_write_rec_offset;
    uint32            orig_file_offset;
    uint32            write_size;
  }write_rec_hdr;
  uint8               *orig_file_data;

};



/* This structure-instance is stored inside the EFS_FILE structure */
struct efs_rbk_file_info {
  uint32        rbk_file_nbr; /* Rollback File number, for file-path      */
  int           rbk_fd;       /* file-handle to the rollback file         */
};


/*=============================================================================
  FUNCTION      efs_rollback_init

  DESCRIPTION   Register with Diag to prevent access to our special
                '/.rollback/ folder by Diag users.

  PARAMETERS    None

  RETURN VALUE  None
=============================================================================*/
void
efs_rollback_init (void);

/*=============================================================================
  FUNCTION      efs_rollback_create_sym_link

  DESCRIPTION   Creates the rollback-file to the given file-path. This rollback
                file will be used later to rollback the given file.
                The file-name of the rollback-file will be in the range of the
                uint32. This function will attempt to create a symbolic-link
                for the configured number of times.
  PARAMETERS
    file_path -> [IN]
                path of the file for which rollback-file needs to be created.
    rbk_info_ptr <- [OUT]
                the rollback file-number and the file-descriptor will be passed
                and these values should be stored inside the stream.
  RETURN VALUE
    uint32 <-  [OUT]
              a unique number to identify the symbolic-link file path.
=============================================================================*/
int
efs_rollback_create_rbk_file (const char *file_path,
  struct efs_rbk_file_info *rbk_info_ptr);

/*=============================================================================
  FUNCTION      efs_rollback_remove_sym_link

  DESCRIPTION   This function removes the rollback file.

  PARAMETERS
    rbk_info_ptr -> [IN]
                    The value returned by efs_rollback_create_rbk_file().

  RETURN VALUE  None
=============================================================================*/
void
efs_rollback_remove_rbk_file (const struct efs_rbk_file_info *rbk_info_ptr);


/*=============================================================================
  FUNCTION      efs_rollback_recover

  DESCRIPTION   Iterates over all the rollback-files from the /.rollback
                directory, and recovers the original-file.
                The original-file's will get deleted if they were new-files and
                were created in the previous boot-session, In the case of
                existing-file's the original-file's contents are recovered from
                the rollback-file. The rollback-file was getting updated
                whenever the original-file's contents was getting changed.

  PARAMETERS    None

  RETURN VALUE  None
=============================================================================*/
void
efs_rollback_recover (void);


/*=============================================================================
  FUNCTION      efs_rollback_write_req_to_orig_file

  DESCRIPTION   This function should be called efs_fwrite(). This function will
                backup the affected bytes from original-file onto the rollback-
                file.

  PARAMETERS
    rbk_fd  -> [IN]
                File descriptor of the rollback file.
    orig_fd -> [IN]
                File descriptor of the file to backup.
    size    -> [IN]
                Number of bytes to backup.
    filepos -> [IN]
                Position on file from where we need to backup.

  PARAMETERS    None

  RETURN VALUE  -1 in case of error, 0 on successfull backup.
=============================================================================*/
int
efs_rollback_write_req_to_orig_file (int rbk_fd, int orig_fd, fs_size_t size,
                                     fs_size_t filepos);


#endif /* _FS_ROLLBACK_H */
