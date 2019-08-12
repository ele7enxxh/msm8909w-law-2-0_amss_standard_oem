/***********************************************************************
 * fs_tar.c
 *
 * FS TAR Module.
 * Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
 *
 * This module can parse an TAR stream and store the TAR contents in
 * the native EFS in the phone.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_tar/src/fs_tar.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-11-21   vm    Authenticating zip tar image using keys in h/w fuse.
2014-11-20   wek   Handle large files with mode bits set as item file.
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-01-29   nr    Migrate to new signed image authentication apis.
2012-12-14   wek   Implement modified efs_utime for item files.
2012-01-24   wek   Implement untar of USTAR format.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-10-11   nr    Added support for ZIP_TAR feature.
2011-09-11   rp    Fix compiler warning seen on 8960 target build.
2011-08-26   rp    Fix compiler warning when SEC_AUTH_IMG is disabled.
2011-07-01   wek   Create a TAR file from a given directory.
2011-06-18   wek   Add secure TAR signing/verification.
2011-03-25   rp    Support file-mode bits.
2010-12-06   rp    Document the limitations of the FS-TAR module.
2010-12-03   rp    Featurize FS-TAR module.
2010-11-30   rp    Don't skip the special item-file folder creation.
2010-11-30   rp    Fix the conversion logic from octal to decimal.
2010-10-29   rp    Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_FS_TAR

#include "fs_sys_types.h"
#include "fs_estdio.h"
#include "fs_util.h"
#include "fs_err.h"
#include "fs_priv_funcs.h"
#include "fs_public.h"
#include "fs_lib.h"
#include "fs_errno.h"
#include "fs_signed_img_param.h"
#include "fs_tar_i.h"

#include "assert.h"
#include "stringl/stringl.h"

#include <stdio.h>

/*****************************************************************************

File-Format of the TAR file :-
--------------------------------

  A tar archive consists of a series of 512-byte records.  Each file system
  object requires a header record which stores basic metadata (pathname,
  mode, owner, permissions, etc.) and zero or more records containing any file
  data. The end of the archive is indicated by two records consisting
  entirely of zero bytes.



      |-----------------------|
      | 1st File-Header       | Header Size = 512 Bytes.
      |-----------------------|
      |                       | File data in 512-byte frames.
      | 1st File-Data         | The last 512-byte frame is padded with 0's
      |                       | to make the File-Data multiple of 512-bytes.
      |-----------------------|
      | 2nd File-Header       | Header Size = 512 Bytes.
      |-----------------------|
      |                       | File data in 512-byte frames.
      | 2nd File-Data         | The last 512-byte frame is padded with 0's
      |                       | to make the File-Data multiple of 512-bytes.
      |-----------------------|
      |        ----           |
      |        ----           | More files.
      |        ----           |
      |-----------------------|
      | Last File-Header      | Header Size = 512 Bytes.
      |-----------------------|
      |                       | File data in 512-byte frames.
      | Last File-Data        | The last 512-byte frame is padded with 0's
      |                       | to make the File-Data multiple of 512-bytes.
      |-----------------------|
      | 0'ed File-Header      | Header Size = 512 Bytes.
      |-----------------------|



Format of USTAR File-Header :-
-------------------------------

The original TAR format was made of members from (a) to (i). This header was
extended in POSIX creating the USTAR header, adding fields from (j) to (p).
USTAR is backwards compatible with previous versions of TAR header.

      |-----------------------|
      |                       |
 (a)  | Entry Name            | 100 bytes. Ascii.
      |                       |
      |-----------------------|
 (b)  | File mode             | 8 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (c)  | Owner user ID         | 8 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (d)  | Owner group ID        | 8 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (e)  | File size             | 12 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (f)  | Last modification-time| 12 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (g)  | Checksum              | 8 bytes. Octal numbers stored in Ascii.
      |-----------------------|
 (h)  | Link Indicator field  | 1 byte.
      |-----------------------|
      |                       |
 (i)  | Symlink File Path     | 100-bytes. Ascii.
      |                       |
      |-----------------------|
 (j)  | Magic                 | 6 bytes. Ascii.
      |-----------------------|
 (k)  | Version               | 2 bytes. Ascii.
      |-----------------------|
 (l)  | User Name             | 32 bytes. Ascii.
      |-----------------------|
 (m)  | Group Name            | 32 bytes. Ascii.
      |-----------------------|
 (n)  | Device Major          | 8 bytes. Platform specific.
      |-----------------------|
 (o)  | Device Minor          | 8 bytes. Platform specific.
      |-----------------------|
 (p)  | Entry Name Prefix     | 155 bytes. Ascii.
      |-----------------------|
 (q)  | Padding               | 12 bytes.
      |-----------------------|

       Header Size = 512 Bytes.

(a) Entry name:
    Last portion of path, stored as a null-terminated string.

(b) File mode:
    File mode, stored as an octal number in ASCII.
    The mode field provides nine bits specifying file permissions and
    three bits to specify the Set UID, Set GID, and Save Text (sticky) modes.

   +---+ +---+ +---+  +---+ +---+ +---+  +---+ +---+ +---+  +---+ +---+ +---+
   |12 | |11 | |10 |  | 9 | | 8 | | 7 |  | 6 | | 5 | | 4 |  | 3 | | 2 | | 1 |
   +---+ +---+ +---+  +---+ +---+ +---+  +---+ +---+ +---+  +---+ +---+ +---+
   SUID  SGID  SVTX   |--> Owner   <--|  |--> Group   <--|  |--> Others  <--|
                            r,w,x              r,w,x              r,w,x
    Bits     : Interpretation
    3,2,1    : Others [r,w,x]
    6,5,4    : Group  [r,w,x]
    9,8,7    : Owner  [r,w,x]
    10       : Sticky-bit.
    11       : SGID, Set Owner Group-ID (field d) while creating file.
    12       : SUID, Set Owner User-ID  (field c) while creating file.

(c) Owner UID:
(d) Owner GID:
    User id and group id of owner, as octal numbers in ASCII.
    When creating new files, use these ids based on the filemode bits 11 & 12

(e) File size:
    File size, stored as an octal number in ASCII.

(f) Last modification-time:
    Modification time of file, as an octal number in ASCII.  This
    indicates the number of seconds since the start of the epoch,
    00:00:00 UTC January 1, 1970.

(g) Checksum:
    Header checksum, stored as an octal number in ASCII.  To compute
    the checksum, set the checksum field to all spaces, then sum all
    bytes in the header using unsigned arithmetic.  This field should
    be stored as six octal digits followed by a null and a space
    character.

(h) Link Indicator field:
    This field specifies the type of file archived.

     Value : File type
      '0'  : Normal file
      NULL : Normal file (now obsolete)
      '1'  : Hard link
      '2'  : Symbolic link
      '3'  : Character special
      '4'  : Block special
      '5'  : Directory
      '6'  : FIFO
      '7'  : Contiguous file

(i) Symlink File Path:
    Symlink path, stored as a null-terminated string.

(j) Magic:
    This field contains the null terminated string "ustar" when the
    header has ustar format.

(k) Version:
    String containing the USTAR version. POSIX specifies "00".

(l) User Name:
(m) Group Name:
    NULL terminated strings to indicate the user name and group name of
    the file. Not used by this implementation.

(n) Device Major:
(o) Device Minor:
    Platform specific fields. Not used by this implementation.

(p) Entry Name Prefix:
    NULL terminated string. Path prefix. Must be added before field (a) to
    generate complete path if it is a ustar header.

(q) Padding:
    Padding, not used by USTAR or regular TAR.

*****************************************************************************/


/*****************************************************************************
  Limitations of the FS-TAR module.

  1. All the limitations of the TAR file format standard.
  2. The NV-Item data length cannot exceed 512 bytes.
  3. The 4 bytes UID and GID in the TAR file will be casted to uint16 to
     be stored in EFS2.

*****************************************************************************/

/*  The maximum size of a path (file or symlink) in TAR is 100 bytes */
#define FS_TAR_NAME_LEN  100
#define FS_TAR_FILE_MODE_LEN 8
#define FS_TAR_UID_LEN       8
#define FS_TAR_GID_LEN       8
#define FS_TAR_FILE_SIZE_LEN 12
#define FS_TAR_MTIME_LEN     12
#define FS_TAR_CHECKSUM_LEN  8
#define FS_TAR_MAGIC_LEN     6
#define FS_TAR_VERSION_LEN   2
#define FS_TAR_UNAME_LEN     32
#define FS_TAR_GNAME_LEN     32
#define FS_TAR_DEVMAJOR_LEN  8
#define FS_TAR_DEVMINOR_LEN  8
#define FS_TAR_PREFIX_LEN    155
#define FS_TAR_PAD_LEN       12

/* TAR File header record. */
struct fs_tar_hdr_format
{
  char      file_name[FS_TAR_NAME_LEN];
  char      file_mode[FS_TAR_FILE_MODE_LEN];
  char      uid[FS_TAR_UID_LEN];
  char      gid[FS_TAR_GID_LEN];
  char      file_size[FS_TAR_FILE_SIZE_LEN];
  char      mtime[FS_TAR_MTIME_LEN];
  char      checksum[FS_TAR_CHECKSUM_LEN];
  char      link_indicator;
  char      link_file_name[FS_TAR_NAME_LEN];
  char      magic[FS_TAR_MAGIC_LEN];
  char      version[FS_TAR_VERSION_LEN];
  char      uname[FS_TAR_UNAME_LEN];
  char      gname[FS_TAR_GNAME_LEN];
  char      devmajor[FS_TAR_DEVMAJOR_LEN];
  char      devminor[FS_TAR_DEVMINOR_LEN];
  char      prefix[FS_TAR_PREFIX_LEN];
  char      pad[FS_TAR_PAD_LEN];
};

/* Link-Indicator field values in the TAR file header. */
#define FS_TAR_REGULAR_FILE_OBSOLETE     0
#define FS_TAR_REGULAR_FILE             '0'
#define FS_TAR_HARD_LINK                '1'
#define FS_TAR_SYM_LINK                 '2'
#define FS_TAR_CHAR_SPECIAL             '3'
#define FS_TAR_BLOCK_SPECIAL            '4'
#define FS_TAR_DIRECTORY                '5'
#define FS_TAR_FIFO                     '6'
#define FS_TAR_CONTIGOUS_FILE           '7'

/* TAR File header conveniently stored in this structure
 * in our preferable format */
struct fs_tar_file_info
{
  /* We have to add 2 bytes to the file name length to account for '/' and
   * NULL character. */
  char            file_name[FS_TAR_NAME_LEN + FS_TAR_PREFIX_LEN + 2];
  fs_mode_t       mode;
  uint32          uid;
  uint32          gid;
  uint32          file_size;
  fs_time_t       mtime;
  uint32          checksum;
  uint8           link_indicator;
  char            link_file_name[FS_TAR_NAME_LEN + 1];

  EFS_EFILE       *fp;
  uint32          curr_file_size;
  int             is_nv_item;
};


/* Our TAR file state machine while procoessing tar file data. */
enum fs_tar_state
{
  FS_TAR_STATE_NOT_STARTED,
  FS_TAR_STATE_PROCESSING_HDR,
  FS_TAR_STATE_PROCESSING_FILE,
  FS_TAR_STATE_PROCESSING_FINISHED
};

/* The main FS tar structure. */
struct fs_tar_info
{
  enum fs_tar_state  tar_state;
  struct fs_tar_file_info  file_info;

  uint8   *buffer;
  uint32  buff_len;
  uint32  buff_idx;
  int     is_in_use;
  /* Number of zero filled headers seen while untaring. Two consecutive
   * zero filled headers/frames mean it is the end of TAR. */
  int     zero_frame_cnt;
};

#define FS_TAR_MAX_INSTANCES    1
static struct fs_tar_info     fs_tar_info_inst[FS_TAR_MAX_INSTANCES];



static int fs_tar_process_hdr (struct fs_tar_info *tar_info);
static void fs_tar_process_file_data (struct fs_tar_info *tar_info);

static void fs_tar_mkdir (struct fs_tar_info *tar_info);
static void fs_tar_create_file (struct fs_tar_info *tar_info);
static void fs_tar_create_symlink (struct fs_tar_info *tar_info);

static int fs_tar_compute_checksum (struct fs_tar_hdr_format *tar_hdr);
static uint32 fs_tar_verify_padding_bytes (struct fs_tar_info *tar_info);
static uint32 fs_tar_get_decimal (const char *ascii_str, int ascii_len);
static void fs_tar_extract_file_details (struct fs_tar_hdr_format *tar_hdr,
                                         struct fs_tar_file_info *file_info);
static uint8 fs_tar_validate_sw_key (uint8 *current_root_key, uint32 hash_len);

#define IS_DIGITS(x)       (((x) >= '0') && ((x) <= '9'))


/* All EFS-Item files will have this string preceding the filepath in tar */
const char *FS_TAR_ITEM_FILE_IDENTIFIER = "efs_item_files/";

/* Turn on printf messages for debugging. */
#undef FS_TAR_ENABLE_DEBUGGING

void
fs_tar_init (void)
{
  ASSERT (sizeof (struct fs_tar_hdr_format) == FS_TAR_HDR_SIZE);
  memset (fs_tar_info_inst, 0, sizeof (fs_tar_info_inst));
}


struct fs_tar_info*
fs_tar_open (void)
{
  uint32 i;
  struct fs_tar_info *tar_info = NULL;

  for ( i = 0; i < FS_TAR_MAX_INSTANCES; ++i )
  {
    if (fs_tar_info_inst[i].is_in_use == 0)
    {
      memset (&fs_tar_info_inst[i], 0, sizeof (fs_tar_info_inst[i]));
      fs_tar_info_inst[i].is_in_use = 1;
      tar_info = &fs_tar_info_inst[i];
      break;
    }
  }

  return tar_info;
}

int
fs_tar_close (struct fs_tar_info *tar_info)
{
  ASSERT (tar_info != NULL);
  ASSERT (tar_info->is_in_use == 1);
  tar_info->is_in_use = 0;
  return 0;
}


int
fs_tar_process_data (struct fs_tar_info *tar_info,
                     uint8 *buffer, uint32 buffer_size)
{
  int result = 1;

  ASSERT (tar_info != NULL);

  /* Validate the supplied arguments. */
  ASSERT (buffer != NULL);
  ASSERT (buffer_size >= FS_TAR_HDR_SIZE);
  ASSERT ((buffer_size % FS_TAR_HDR_SIZE) == 0);

  /* Make sure previous buffer was processed completely, Hookup new buffer. */
  ASSERT (tar_info->buffer == NULL);
  ASSERT (tar_info->buff_len == 0);
  ASSERT (tar_info->buff_idx == 0);
  tar_info->buffer = buffer;
  tar_info->buff_len = buffer_size;
  tar_info->buff_idx = 0;

  while (tar_info->buff_idx < tar_info->buff_len)
  {
    switch (tar_info->tar_state)
    {
      case FS_TAR_STATE_NOT_STARTED:
        tar_info->tar_state = FS_TAR_STATE_PROCESSING_HDR;
        break;

      case FS_TAR_STATE_PROCESSING_HDR:
        result = fs_tar_process_hdr (tar_info);
        break;

      case FS_TAR_STATE_PROCESSING_FILE:
        fs_tar_process_file_data (tar_info);
        break;

      case FS_TAR_STATE_PROCESSING_FINISHED:
        tar_info->buff_idx = tar_info->buff_len;
        result = 0; /* Finished up this tar. lets return 0 now. */
        break;

      default:
        FS_ERR_FATAL ("fs-tar unexpected state [%d]",tar_info->tar_state,0,0);
        break;
    }
  }

  /* We are done with the current buffer. */
  tar_info->buffer = NULL;
  tar_info->buff_len = 0;
  tar_info->buff_idx = 0;

  return result;
}

/* Checks if a buffer is zeroed out. The function assumes that buffer length
 * is a multiple of 4 bytes. */
static int
is_zeroed (void *buffer, uint32 len)
{
  uint32 *buf = (uint32 *)buffer;
  uint32 i;

  len = len / sizeof (uint32);

  for (i = 0; i < len; i++)
  {
    if (*buf != 0)
      return 0;
    buf++;
  }

  /* Make sure the given buffer length is multiple of 4. */
  ASSERT ((len & (sizeof (uint32) - 1)) == 0);

  return 1;
}

static int
fs_tar_process_hdr (struct fs_tar_info *tar_info)
{
  struct fs_tar_hdr_format *tar_hdr;
  struct fs_tar_file_info *file_info;
  int result = 1;

  /* Make sure we have the whole header in the buffer. */
  ASSERT (tar_info->buff_idx < tar_info->buff_len);
  ASSERT ((tar_info->buff_len - tar_info->buff_idx) >= FS_TAR_HDR_SIZE);

  /* Get the file-header. */
  tar_hdr = (struct fs_tar_hdr_format *)&tar_info->buffer[tar_info->buff_idx];
  (void) tar_hdr->version;
  (void) tar_hdr->uname;
  (void) tar_hdr->gname;
  (void) tar_hdr->devmajor;
  (void) tar_hdr->devminor;
  (void) tar_hdr->pad;

  /* End of TAR file will have 0'ed sectors. */
  if (is_zeroed (tar_hdr, sizeof (struct fs_tar_hdr_format)) == 1)
  {
    ++tar_info->zero_frame_cnt;
    if (tar_info->zero_frame_cnt == 2)
    {
      tar_info->tar_state = FS_TAR_STATE_PROCESSING_FINISHED;
      result = 0;
    }
    return result;
  }

  tar_info->zero_frame_cnt = 0;

  memset (&tar_info->file_info, 0, sizeof (tar_info->file_info));
  file_info = &tar_info->file_info;
  tar_info->buff_idx += FS_TAR_HDR_SIZE;

  fs_tar_extract_file_details (tar_hdr, file_info);

  switch (tar_hdr->link_indicator)
  {
    case FS_TAR_DIRECTORY: /* Regular directory */
    {
      fs_tar_mkdir (tar_info);
    }
    break;

    case FS_TAR_REGULAR_FILE: /* Regular File or Item-File */
    case FS_TAR_REGULAR_FILE_OBSOLETE:
    {
      fs_tar_create_file (tar_info);
    }
    break;

    case FS_TAR_SYM_LINK: /* Symlink */
    {
      fs_tar_create_symlink (tar_info);
    }
    break;

    default:
    {
      FS_ERR_FATAL ("Invalid LI [%d]", tar_hdr->link_indicator, 0, 0);
    }
  }

  return result;
}

static void
fs_tar_extract_file_details (struct fs_tar_hdr_format *tar_hdr,
                             struct fs_tar_file_info *file_info)
{
  char *ascii_str;
  uint32 size;
  uint32 stored_checksum;

  /* Verify checksum in the header. */
  size = sizeof (tar_hdr->checksum);
  stored_checksum = fs_tar_get_decimal (tar_hdr->checksum, size);
  file_info->checksum = fs_tar_compute_checksum (tar_hdr);
  ASSERT (stored_checksum == file_info->checksum);

  /* Extract file name */
  ascii_str = file_info->file_name;
  /* Check for the magic fields that identify USTAR format. */
  if (strncmp (tar_hdr->magic, "ustar", FS_TAR_MAGIC_LEN) == 0 &&
      tar_hdr->version[0] == '0' && tar_hdr->version[1] == '0')
  {
    uint32 prefix_size;
    /* Add 1 to the total size of the prefix because the field may not
     * be NULL terminated. Adding one will make it NULL terminated. */
    size = sizeof (tar_hdr->prefix) + 1;
    (void) strlcpy (ascii_str, tar_hdr->prefix, size);
    if (tar_hdr->file_name[0] != 0)
    {
      prefix_size = strlen (ascii_str);
      /* If there is a prefix add slash '/' to the path. */
      if (prefix_size > 0)
      {
        ascii_str[prefix_size++] = '/';
        ascii_str[prefix_size] = '\0';
        ascii_str += prefix_size;
      }
      /* Add 1 to the total size of the file name because the field may not
       * be NULL terminated. Adding one will make it NULL terminated. */
      size = sizeof (tar_hdr->file_name) + 1;
      (void) strlcpy (ascii_str, tar_hdr->file_name, size);
    }
  }
  else
  {
    /* this is not USTAR format. */
    size = sizeof (tar_hdr->file_name);
    (void) strlcpy (ascii_str, tar_hdr->file_name, size);
  }


  /* extract file mode  */
  ascii_str = tar_hdr->file_mode;
  size = sizeof (tar_hdr->file_mode);
  file_info->mode = fs_tar_get_decimal (ascii_str, size);

  /* extract file owner user-id  */
  ascii_str = tar_hdr->uid;
  size = sizeof (tar_hdr->uid);
  file_info->uid = fs_tar_get_decimal (ascii_str, size);

  /* extract file owner group-id  */
  ascii_str = tar_hdr->gid;
  size = sizeof (tar_hdr->gid);
  file_info->gid = fs_tar_get_decimal (ascii_str, size);

  /* extract file size  */
  ascii_str = tar_hdr->file_size;
  size = sizeof (tar_hdr->file_size);
  file_info->file_size = fs_tar_get_decimal (ascii_str, size);

  /* extract last file modification time */
  ascii_str = tar_hdr->mtime;
  size = sizeof (tar_hdr->mtime);
  file_info->mtime = (fs_time_t ) fs_tar_get_decimal (ascii_str, size);

  /* extract link indicator field. */
  file_info->link_indicator = tar_hdr->link_indicator;

  /* extract symlink name */
  ascii_str = file_info->link_file_name;
  size = sizeof (file_info->link_file_name);
  (void) strlcpy (ascii_str, tar_hdr->link_file_name, size);


#ifdef FS_TAR_ENABLE_DEBUGGING
{
  static int file_cnt = 0;
  char hackedup_name[FS_TAR_NAME_LEN];
  int result;

  ++file_cnt;

  ascii_str = file_info->file_name;
  result = strncmp (FS_TAR_ITEM_FILE_IDENTIFIER, file_info->file_name,
                    strlen (FS_TAR_ITEM_FILE_IDENTIFIER));
  if (result == 0)
  {
    /* Ignore the magic prefix for the NV item. */
    ascii_str += strlen (FS_TAR_ITEM_FILE_IDENTIFIER);
  }

  size = sizeof (file_info->file_name);
  (void) strlcpy (hackedup_name, ascii_str, size);

  printf ("-----------------------------------------------------------\n\n");
  printf ("In TAR :\n");
  printf ("filename : [%s]\n", tar_hdr->file_name);

  printf ("In EFS :\n");
  printf ("filename : [%s]\n", hackedup_name);
  printf ("size : [%8d] mode : [%5d] uid : [%3d] gid : [%3d] mtime : [%8u]\n",
          file_info->file_size, file_info->mode, file_info->uid,
          file_info->gid, file_info->mtime);
}
#endif /* FS_TAR_ENABLE_DEBUGGING */

}


static void
fs_tar_chmod (struct fs_tar_file_info *file_info)
{
  int result;
  fs_mode_t mode;

  /* Only the permission/suid/sticky bits can be set */
  mode = file_info->mode & 07777;
  result = efs_chmod (file_info->file_name, mode);
  ASSERT (result == 0);
}


static void
fs_tar_chown (struct fs_tar_file_info *file_info)
{
  int uid, gid;
  int result;

  uid = file_info->uid;
  gid = file_info->gid;

  /*--------------------------------------------------------------------------
   * We have to confirm these 2 bits in the file-mode. Some tar utilities
   * might set these 2 bits if they want the uid and gid values to be used.

    if ((file_info->mode & S_ISUID) != S_ISUID) uid = 0;
    if ((file_info->mode & S_ISGID) != S_ISGID) gid = 0;
  --------------------------------------------------------------------------*/

  if ( (uid != 0) || (gid != 0) )
  {
    if (!file_info->is_nv_item)
    {
      /* TAR has 4 bytes for UID, but EFS2 has only 2 bytes. when we do this
       * efs_chown() we will loose the 2 MSB bytes. */
      result = efs_chown (file_info->file_name, uid, gid);
      ASSERT (result == 0);
    }
  }
}

static void
fs_tar_set_file_times (struct fs_tar_file_info *file_info)
{
  struct fs_utimbuf times;
  int result;

  times.actime = file_info->mtime;
  times.modtime = file_info->mtime;

  if (!file_info->is_nv_item)
  {
    result = efs_utime_item (file_info->file_name, &times);
    ASSERT (result == 0);
  }
}

static void
fs_tar_mkdir (struct fs_tar_info *tar_info)
{
  struct fs_tar_file_info *file_info;
  int result;

  file_info = &tar_info->file_info;

  result = efs_mkdir (file_info->file_name, 0777);
  if ( (result != 0) &&
       (efs_errno != EEXIST) )
  {
    (void) efs_auto_mkdir (file_info->file_name);
    result = efs_mkdir (file_info->file_name, 0777);
    if (result != 0)
    {
      ASSERT (efs_errno == EEXIST);
    }
  }

  fs_tar_chmod (file_info);
  fs_tar_chown (file_info);
  fs_tar_set_file_times (file_info);
}

static void
fs_tar_create_file (struct fs_tar_info *tar_info)
{
  char *ascii_str, *file_path;
  uint32 size;
  int result, is_item_file = 0;

  file_path = (char *)tar_info->file_info.file_name;

  (void) efs_rmdir (file_path);
  (void) efs_unlink (file_path);

  if (S_ISITM (tar_info->file_info.mode))
  {
    is_item_file = 1;
  }

  /* Check if this filename has the magic-string in the start to identify
   * this as the Item-file. */
  result = strncmp (FS_TAR_ITEM_FILE_IDENTIFIER, file_path,
                    strlen (FS_TAR_ITEM_FILE_IDENTIFIER));
  if (result == 0)
  {
    is_item_file = 1;

    /* Ignore the magic prefix for the item-file. */
    ascii_str = file_path;
    ascii_str += strlen (FS_TAR_ITEM_FILE_IDENTIFIER);
    size = sizeof (tar_info->file_info.file_name);
    (void) strlcpy (file_path, ascii_str, size);
  }

  /* If the file size is too large for item file, then store as regular file.*/
  if (tar_info->file_info.file_size > FS_ITEM_MAX)
  {
    is_item_file = 0;
  }

  if (is_item_file)
  {
    tar_info->file_info.is_nv_item = 1;
  }
  else
  {
    tar_info->file_info.is_nv_item = 0;

    tar_info->file_info.fp = efs_efopen (file_path, "wb");
    if (tar_info->file_info.fp == NULL)
    {
      result = efs_auto_mkdir (file_path);
      ASSERT (result == 0);
      tar_info->file_info.fp = efs_efopen (file_path, "wb");
    }
    ASSERT (tar_info->file_info.fp != NULL);
  }

  /* Now lets go ahead and get the data for this file. */
  tar_info->tar_state = FS_TAR_STATE_PROCESSING_FILE;

}

static void
fs_tar_create_symlink (struct fs_tar_info *tar_info)
{
  int result;
  const char *abs_path, *symlink_path;

  symlink_path = (const char *)tar_info->file_info.file_name;
  abs_path = (const char *)tar_info->file_info.link_file_name;

  (void) efs_rmdir (symlink_path);
  (void) efs_unlink (symlink_path);

  result = efs_symlink (abs_path, symlink_path);
  if (result != 0)
  {
    (void ) efs_auto_mkdir (symlink_path);
    result = efs_symlink (abs_path, symlink_path);
  }
  ASSERT (result == 0);
}

static void
fs_tar_finished_creating_file (struct fs_tar_info *tar_info)
{
  struct fs_tar_file_info *file_info;
  int result;

  file_info = &tar_info->file_info;

  if (!file_info->is_nv_item)
  {
    result = efs_efclose (file_info->fp);
    ASSERT (result == 0);
    fs_tar_chmod (file_info);
  }

  fs_tar_chown (file_info);
  fs_tar_set_file_times (file_info);
}

static void
fs_tar_process_file_data (struct fs_tar_info *tar_info)
{
  struct fs_tar_file_info *file_info;
  fs_size_t buff_size;
  fs_size_t file_size;
  fs_size_t write_size;
  uint32 pad_size;
  int result;

  file_info = &tar_info->file_info;

  ASSERT (tar_info->buff_idx < tar_info->buff_len);
  buff_size = tar_info->buff_len - tar_info->buff_idx;

  ASSERT (file_info->curr_file_size <= file_info->file_size);
  file_size = file_info->file_size - file_info->curr_file_size;
  write_size = 0;

  if (file_info->is_nv_item)
  {
    /* Make sure we have the entire NV-item data in the current buffer. */
    ASSERT (file_size <= buff_size);
    result = efs_put (file_info->file_name,
                      &tar_info->buffer[tar_info->buff_idx], file_size,
                      O_CREAT | O_AUTODIR,  file_info->mode);
    ASSERT (result == 0);
    write_size = file_size;
  }
  else
  {
    write_size = (buff_size >= file_size) ? file_size : buff_size;
    result = efs_efwrite (&tar_info->buffer[tar_info->buff_idx], 1,
                          write_size, file_info->fp);
    ASSERT (result == (int)write_size);
  }

  tar_info->buff_idx += write_size;

  file_info->curr_file_size += write_size;
  if (file_info->curr_file_size == file_info->file_size)
  {
    fs_tar_finished_creating_file (tar_info);

    /* we are done with this file, get ready for the next. */
    pad_size = fs_tar_verify_padding_bytes (tar_info);
    tar_info->buff_idx += pad_size;
    tar_info->tar_state = FS_TAR_STATE_PROCESSING_HDR;
  }

}

static uint32
fs_tar_verify_padding_bytes (struct fs_tar_info *tar_info)
{
  uint32 pad_size, buff_size;
  uint32 i;
  char ch;

  pad_size = 0;
  if (tar_info->buff_idx % 512)
  {
    /* Reached EOF, compute how many zero padded bytes. */
    if (tar_info->buff_idx > 512)
      pad_size = 512 - (tar_info->buff_idx % 512);
    else
      pad_size = 512 - tar_info->buff_idx;

    /* Make sure we have all the padding bytes in the current buffer. */
    buff_size = tar_info->buff_len - tar_info->buff_idx;
    ASSERT (pad_size <= buff_size);

    /* Make sure the padded bytes are all 0's */
    for ( i = 0; i < pad_size; ++i )
    {
      ch = tar_info->buffer[tar_info->buff_idx + i];
      ASSERT (ch == 0);
    }
  }

  return pad_size;
}


static uint32
fs_tar_get_decimal (const char *ascii_str, int ascii_len)
{
  char ch;
  uint32 pow_8, digit;
  uint32 decimal;

  ASSERT (ascii_str != NULL);
  ASSERT (ascii_len > 0);

  --ascii_len; /* 0-indexed string */

  pow_8 = 1;
  decimal = 0;

  while (ascii_len >= 0)
  {
    ch = ascii_str[ascii_len];

    if (!IS_DIGITS (ch))
    {
      --ascii_len;
      continue;
    }

    digit = ch - '0';
    if (digit != 0)
      decimal += (digit * pow_8);
    pow_8 *= 8;
    --ascii_len;
  }

  return decimal;
}


static int
fs_tar_compute_checksum (struct fs_tar_hdr_format *tar_hdr)
{
  int i;
  unsigned char *p, ch;
  int computed_checksum;

  memset (tar_hdr->checksum, ' ', sizeof (tar_hdr->checksum));

  computed_checksum = 0;
  p = (unsigned char *)tar_hdr;
  for ( i = 0; i < FS_TAR_HDR_SIZE; ++i )
  {
    ch = *p++;
    computed_checksum += ch;
  }

  return computed_checksum;
}


int
fs_tar_is_this_a_tar_file (uint8 *buffer, uint32 buffer_size)
{
  struct fs_tar_hdr_format *tar_hdr;
  uint8 orig_checksum[8];
  uint32 result, stored_checksum, computed_checksum;
  int size;

  /* We need atleast one header to confirm tar file. */
  if (buffer_size < FS_TAR_HDR_SIZE)
  {
    return 0;
  }

  tar_hdr = (struct fs_tar_hdr_format *)buffer;

  if (tar_hdr->file_name[0] == 0)
  {
    return 0;
  }

  /* Backup the original checksum */
  fs_memscpy (orig_checksum, sizeof (orig_checksum), tar_hdr->checksum,
              sizeof (orig_checksum));

  /* Verify checksum in the header. */
  size = sizeof (tar_hdr->checksum);
  stored_checksum = fs_tar_get_decimal (tar_hdr->checksum, size);
  computed_checksum = fs_tar_compute_checksum (tar_hdr);

  /* Restore the original checksum */
  fs_memscpy (tar_hdr->checksum, sizeof (tar_hdr->checksum), orig_checksum,
              sizeof (orig_checksum));

  result = (stored_checksum == computed_checksum) ? 1 : 0;

  return result;
}

/* This function helps to check if the current key used as root of trust is a
 * dummy key. The current root key is compared to the dummy key. If the keys
 * match, it indicates that the key used for signing is still a dummy one and
 * hence shouldnot be used for authenticating unless
 * FEATURE_FS_TAR_ALLOW_DUMMY_KEY is defined.
 *
 * result = 1 Indicates sw key is valid and can be used for authentication.
 * result = 0 Indicates sw key not valid.
 *
 * For internal testing, we define FEATURE_FS_TAR_ALLOW_DUMMY_KEY
 * so that we can use fs tar file signed with dummy key.
 */


static uint8
fs_tar_validate_sw_key (uint8 *current_root_key, uint32 hash_len)
{
  uint32 dummy_key_len;
  uint8 result = 0;
  int cmp_result;

  /* THE BELOW DUMMY KEY SHOULD NOT BE MODIFIED */
  uint8 fs_dummy_key[] = \
                       {0xd5, 0x85, 0x22, 0xcd, 0x60, 0x2a, 0x50, 0x1c, \
                        0x3a, 0x79, 0x8d, 0x20, 0x15, 0x33, 0xae, 0x31, \
                        0xf9, 0x24, 0x27, 0x2b, 0x0b, 0x76, 0xf1, 0x59, \
                        0xfe, 0x81, 0x75, 0x3b, 0x5f, 0xcf, 0xce, 0x60};

  dummy_key_len = sizeof (fs_dummy_key);

  if (dummy_key_len == hash_len)
  {
    cmp_result = memcmp (fs_dummy_key, current_root_key, hash_len);
    if (cmp_result == 0)
    {
#ifdef FEATURE_FS_TAR_ALLOW_DUMMY_KEY
      result = 1;
#endif
    }
    else
    {
      result = 1;
    }
  }
  else /* key lengths are different, so keys are diff */
  {
    result = 1;
  }

  return result;
}

int
fs_tar_verify_signature (uint8 *buf, uint32 buf_size, uint32 *image_offset,
                         uint32 *image_size, uint8 *handle_buffer,
                         uint32 handle_buffer_size)

{
  uint32 hash_len, img_offset, img_size;
  boolean valid_signature;
  int valid_image;
  uint64 software_id;
  uint32 is_auth_enabled_fuse_blown, result;
  uint8 valid_key;

  secboot_util_fuse_info_type      fuse_info;
  secboot_util_verified_info_type  verified_info;
  secboot_util_handle_type         *sec_boot_handle;

  ASSERT (handle_buffer_size >= sizeof (secboot_util_handle_type));
  ASSERT (handle_buffer != NULL);
  sec_boot_handle = (secboot_util_handle_type *) handle_buffer;

  memset (sec_boot_handle, 0,  sizeof (secboot_util_handle_type));
  memset (&verified_info, 0,  sizeof (verified_info));
  memset (&fuse_info, 0,  sizeof (fuse_info));


  fs_sign_get_software_version_id (&software_id);
  fs_sign_get_msm_id (&fuse_info.msm_hw_id);
  fuse_info.root_of_trust = fs_sign_get_root_hash (&hash_len);
  fuse_info.root_of_trust_len = hash_len;

  /* The serial number has to be supplied one when we are signing the
   * image with debug enable option.
   * For Secure EFS prepopulate signed image verification this is not
   * applicable.So we set serial number as zero and set the use serial
   * number field to zero. Also since we use the other params such as
   * the msm hw id we also set use root of trust only to false.
   */
  fuse_info.auth_use_serial_num = 0;
  fuse_info.serial_num = 0x0;
  fuse_info.use_root_of_trust_only = FALSE;

  /* initalize the return values */
  *image_size = 0;
  *image_offset = 0;
  valid_image = 0;
  valid_signature = FALSE;
  is_auth_enabled_fuse_blown = 0xDEADBEEF;

  /* This is to check if the fuse is blown */
  result = secboot_util_hw_is_auth_enabled
           (
            SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
            &is_auth_enabled_fuse_blown
           );

  ASSERT (result == 0);
  ASSERT (is_auth_enabled_fuse_blown != 0xDEADBEEF);

  if (is_auth_enabled_fuse_blown)
  {
    valid_signature = secboot_util_auth_img_verify
                      (
                      sec_boot_handle,
                      buf,
                      buf_size,
                      SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
                      software_id,
                      NULL,
                      &verified_info
                      );
    FS_MSG_HIGH_1 ("Fuse blown. FSG Image authenticated using h/w key %d",
                    valid_signature);
  }

  if (valid_signature == FALSE)
  {
    /* This indicates that either the h/w fuses are not blown
     * or authentication using the key in h/w fuse has failed.
     * Hence check if the EFS tar file has been signed with
     * hard coded key (other than the dummy key) in the file.
     */
    valid_key = fs_tar_validate_sw_key (fuse_info.root_of_trust,
                                        hash_len);
    if (valid_key == 0)
    {
      /* Indicates a dummy key in root_of_trust. Hence don't use it for
       * authentication. Return.
       */
      FS_MSG_ERROR_2 ("%d, %d, Dummy (Invalid) s/w key in root of trust",
                       valid_key, hash_len);
      goto END;
    }

    /* Check if the image is properly signed.  We pass the
     * SECBOOT_UTIL_HW_MSS_CODE_SEGMENT as it is the value in the modem builds
     * but it is an unused field for EFS images.
     */

    valid_signature = secboot_util_auth_img_verify
                      (
                      sec_boot_handle,
                      buf,
                      buf_size,
                      SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
                      software_id,
                      &fuse_info,
                      &verified_info
                      );

    FS_MSG_HIGH_1 ("FSG Image authenticated using s/w key %d",
                   valid_signature);

  }

  if (valid_signature == TRUE)
  {
    img_offset = verified_info.img_offset;
    img_size = verified_info.img_len;
    //TODO: Validate this change
    //   if (img_offset < buf_size && img_size < buf_size)
    if ((img_offset + img_size) < buf_size)
    {
      /* The image is good to use, return the appropiate offset and size. */
      *image_offset = img_offset;
      *image_size = img_size;
      valid_image = 1;
    }
    else
    {
      FS_MSG_ERROR_3 ("%d, %d, %d, Invalid image size", img_offset, img_size,
                      buf_size);
    }
  }
  else
  {
    FS_MSG_ERROR_0 ("Image fails authentication with h/w and s/w keys");
  }

END:
  return valid_image;
}


/* Item files are of size FS_ITEM_MAX, this must be less than the header size.
 * When we do an efs_raw_get() or efs_get the output buffer to hold the entire
 * item file. TAR creation checks if the supplied buffer is greater than
 * FS_TAR_HDR_SIZE. Therefore it makes the asumption than the item will fit
 * in the same size as the TAR header. */
#if FS_TAR_HDR_SIZE < FS_ITEM_MAX
  #error "TAR creation does not support items bigger than 512"
#endif

enum fs_tar_create_state
{
  FS_TAR_CREATE_HEADER,
  FS_TAR_CREATE_READ_FILE,
  FS_TAR_CREATE_READ_ITEM,
  FS_TAR_CREATE_ZERO_FILLED,
  FS_TAR_CREATE_DONE,
  FS_TAR_CREATE_EOF,
};

/* TAR Creation routines. */
struct fs_tar_create_info
{
  uint8 in_use;
  uint8 zero_filled_frames_sent;
  enum fs_tar_create_state state;
  struct fs_dir_recursive_data *rec_iter; /* Recursive directory iterator */
  int32 entry_size; /* File size (regular/item) populated in the header. */
  EFS_EFILE *curr_fp; /* The current file handle we are reading. */
  const char *item_path;
};

static struct fs_tar_create_info fs_tar_c_info[1] =
                         {{0, 0, FS_TAR_CREATE_HEADER, NULL, 0, NULL, NULL}};

/* Creates a TAR from the given directory. */
struct fs_tar_create_info *
fs_tar_create_open (const char *dir_path)
{
  struct fs_dir_recursive_data *rec_iter;
  struct fs_tar_create_info* t_info;

  if (fs_tar_c_info[0].in_use == 1)
  {
    efs_errno = EMFILE;
    return NULL;
  }

  rec_iter = efs_opendir_recursive (dir_path);
  if (rec_iter == NULL)
    return NULL;

  t_info = &fs_tar_c_info[0];

  t_info->in_use = 1;
  t_info->zero_filled_frames_sent = 0;
  t_info->state = FS_TAR_CREATE_HEADER;
  t_info->rec_iter = rec_iter;
  t_info->entry_size = 0;
  t_info->curr_fp = NULL;
  t_info->item_path = NULL;

  return t_info;
}

/* Populates a given string (dest) with the octal repesentation given by value
 * Will do a MSG_HIGH if the value can't fit in the string. */
static void
fs_tar_populate_octal (char *dest, int len, uint32 value)
{
  int result;

  /* Length has to account for the NULL char at the end. */
  result = snprintf (dest, len, "%0*o", len - 1, (unsigned int)value);
  if (result > len || result < 0)
  {
    FS_MSG_ERROR_3 ("%o can't fit in %d digits in tar %d", value, len, result);
  }
}


/* Populates the buffer with the tar header with the entry in the given path.
 * Returns the number of bytes populated into the header, 512 for a header
 * correctly populated and -1 when an error occured and the header was not
 * created. */
static int
fs_tar_create_header (const char *path, uint8 *buffer,
                      struct fs_tar_create_info *t_info)
{
  struct fs_tar_hdr_format *hdr;
  size_t len;
  struct fs_stat sbuf;
  int result, error;

  /* Path has be less than max supported, including null char. */
  len = strlen (path);
  if (len >= FS_TAR_NAME_LEN)
  {
    FS_MSG_HIGH_2 ("path len %d >= %d not supported", len, FS_TAR_NAME_LEN);
    return -1;
  }

  result = efs_lstat (path, &sbuf);
  if (result != 0)
  {
    error = efs_errno;  /* To keep lint happy */
    FS_MSG_HIGH_2 ("%d %d error stat on path", error, result);

    return -1;
  }

  memset (buffer, 0, FS_TAR_HDR_SIZE);
  hdr = (struct fs_tar_hdr_format *) buffer;
  len = strlcpy (hdr->file_name, path, FS_TAR_NAME_LEN);
  ASSERT (len < FS_TAR_NAME_LEN);

  fs_tar_populate_octal (hdr->file_mode, FS_TAR_FILE_MODE_LEN, sbuf.st_mode);
  fs_tar_populate_octal (hdr->uid, FS_TAR_UID_LEN, sbuf.st_uid);
  fs_tar_populate_octal (hdr->gid, FS_TAR_GID_LEN, sbuf.st_gid);
  fs_tar_populate_octal (hdr->mtime, FS_TAR_MTIME_LEN, sbuf.st_mtime);

  /* Set the entry size. */
  t_info->entry_size = sbuf.st_size;
  if (S_ISLNK (sbuf.st_mode) || S_ISDIR(sbuf.st_mode))
    fs_tar_populate_octal (hdr->file_size, FS_TAR_FILE_SIZE_LEN, 0);
  else
    fs_tar_populate_octal (hdr->file_size, FS_TAR_FILE_SIZE_LEN,
                           sbuf.st_size);

  /* Set the type flag for the current header. */
  if (S_ISLNK (sbuf.st_mode))
  {
    hdr->link_indicator = FS_TAR_SYM_LINK;
    result = efs_readlink (path, hdr->link_file_name, FS_TAR_NAME_LEN);
    if (result <= 0 || result > FS_TAR_NAME_LEN)
    {
      error = efs_errno;   /* To keep lint happy */
      FS_MSG_HIGH_2 ("%d %d readlink tar error.", error, result);

      return -1;
    }
  }
  else
  {
    char typeflag;

    if (S_ISITM (sbuf.st_mode) || S_ISREG (sbuf.st_mode))
      typeflag = FS_TAR_REGULAR_FILE;
    else if (S_ISDIR (sbuf.st_mode))
      typeflag = FS_TAR_DIRECTORY;
    else if (S_ISFIFO (sbuf.st_mode))
      typeflag = FS_TAR_FIFO;
    else if (S_ISCHR (sbuf.st_mode))
      typeflag = FS_TAR_CHAR_SPECIAL;
    else if (S_ISBLK (sbuf.st_mode))
      typeflag = FS_TAR_BLOCK_SPECIAL;
    else
    {
      FS_MSG_HIGH_1 ("%d filetype not supported in tar", sbuf.st_mode);
      efs_errno = EPERM;
      return -1;
    }

    hdr->link_indicator = typeflag;
  }

  /* If the entry size is zero, then it is an empty entry, we don't need to
   * send a frame for it, therefore we don't need to change states. */
  if (sbuf.st_size != 0)
  {
    if (S_ISITM (sbuf.st_mode))
    {
      t_info->state = FS_TAR_CREATE_READ_ITEM;
      t_info->item_path = path;
    }
    else if (S_ISREG (sbuf.st_mode))
    {
      EFS_EFILE *fp;

      fp = efs_efopen (path, "r");
      if (fp == NULL)
      {
        error = efs_errno;    /* To keep lint happy */
        FS_MSG_HIGH_2 ("%d %d can't open file, skip",
                                      error, sbuf.st_size);
        return -1;
      }

      t_info->state = FS_TAR_CREATE_READ_FILE;
      t_info->curr_fp = fp;
    }
  }

  /* All fields in the header are populated, not calculate checksum. */
  memset (hdr->checksum, ' ', FS_TAR_CHECKSUM_LEN);
  result = fs_tar_compute_checksum (hdr);
  /* Checksum needs to end with an space, therefore we reduce the length
   * of octal field by one, leaving the space at the end of checksum. */
  fs_tar_populate_octal (hdr->checksum, FS_TAR_CHECKSUM_LEN - 1, result);

  return FS_TAR_HDR_SIZE;
}

/* Populates the buffer with the contents of an NV item.
 * The NV item data is inside the t_info structure. The buffer must be
 * at least 512 bytes long. */
static int
fs_tar_create_item_file_frame (struct fs_tar_create_info *t_info,
                              uint8 *buffer)

{
  int result, error;

  if (t_info->entry_size > FS_TAR_HDR_SIZE)
  {
    /* We should never encounter an time file greater than 512 bytes.
     * If the structure indicates that the item file size is greater than
     * 512 bytes then there is something horribly wrong, e.g. the
     * structure was overwritten with invalid data. */
    FS_ERR_FATAL ("%d>%d %d Item file size greater than supported",
                  t_info->entry_size, FS_TAR_HDR_SIZE, FS_ITEM_MAX);
  }

  result = efs_get (t_info->item_path, buffer, t_info->entry_size);
  if (result != t_info->entry_size)
  {
    /* The item was removed or the item size changed, eitherway we can't
     * return reliable item file info and reliably create the tar. Return
     * error. */

    error = efs_errno;  /* To keep lint happy */
    FS_MSG_ERROR_3 ("%d %d %d Wrong item size", error, result,
                     t_info->entry_size);

    efs_errno = EILSEQ;
    return -1;
  }
  memset (buffer + result, 0, FS_TAR_HDR_SIZE - result);
  t_info->item_path = NULL;
  result = FS_TAR_HDR_SIZE;

  /* Done reading the item, move state to get the next header.*/
  t_info->state = FS_TAR_CREATE_HEADER;
  return result;
}

/* Read from the opened file descritor and place one frame (512 bytes) into
 * the output buffer. */
static int
fs_tar_create_read_file_frame (struct fs_tar_create_info *t_info,
                               uint8 *buffer)
{
  uint32 read_size;
  long offset;
  int error;

  read_size = efs_efread (buffer, 1, FS_TAR_HDR_SIZE, t_info->curr_fp);
  offset = efs_eftell (t_info->curr_fp);
  if (read_size < FS_TAR_HDR_SIZE)
  {
    if (t_info->entry_size != offset)
    {
      /* The file size was modified while TAR creation. Therefore, the
       * TAR header created earlier is no longer valid. Return error. */

      error = efs_errno;  /* To keep lint happy */
      FS_MSG_ERROR_3 ("%d %d %d Wrong file size", error, offset,
                       t_info->entry_size);

      efs_efclose (t_info->curr_fp);
      t_info->curr_fp = NULL;
      efs_errno = EILSEQ;
      return -1;
    }

    memset (buffer + read_size, 0, FS_TAR_HDR_SIZE - read_size);
    read_size = FS_TAR_HDR_SIZE;
  }

  /* Done reading the file, move state to get the next header. */
  if (t_info->entry_size == offset)
  {
    efs_efclose (t_info->curr_fp);
    t_info->curr_fp = NULL;
    t_info->state = FS_TAR_CREATE_HEADER;
  }

  return read_size;
}

int
fs_tar_create_read (struct fs_tar_create_info *t_info,
                    uint8 *buffer, uint32 buf_size)
{
  uint32 out_size = 0;
  int result = -1;
  const char *path;

  if (t_info == NULL || t_info->in_use != 1 || buffer == NULL ||
      buf_size < FS_TAR_HDR_SIZE)
  {
    efs_errno = EINVAL;
    return -1;
  }

  while (buf_size >= FS_TAR_HDR_SIZE)
  {
    switch (t_info->state)
    {
    case FS_TAR_CREATE_HEADER:
      /* The path returned by readdir_recursive will be used between calls
       * of tar create to read item files. */
      path = efs_readdir_recursive (t_info->rec_iter);
      /* If we got a valid path, then create a header and send it along.
       * If we are done with all the directories move to next state. */
      if (path != NULL)
      {
        result = fs_tar_create_header (path, buffer, t_info);
        if (result != FS_TAR_HDR_SIZE)
          continue;
      }
      else
      {
        t_info->state = FS_TAR_CREATE_ZERO_FILLED;
        continue;
      }
      break;

    case FS_TAR_CREATE_READ_ITEM:
      result = fs_tar_create_item_file_frame (t_info, buffer);
      if (result < 0)
        goto tar_create_read_error;
      break;

    case FS_TAR_CREATE_READ_FILE:
      result = fs_tar_create_read_file_frame (t_info, buffer);
      if (result < 0)
        goto tar_create_read_error;
      break;

    case FS_TAR_CREATE_ZERO_FILLED:
      t_info->zero_filled_frames_sent++;
      memset (buffer, 0, FS_TAR_HDR_SIZE);
      result = FS_TAR_HDR_SIZE;
      /* We need to send 2 zero filled frames to indicate we are done. */
      if (t_info->zero_filled_frames_sent >= 2)
        t_info->state = FS_TAR_CREATE_DONE;
      break;

    case FS_TAR_CREATE_DONE:
      /* We are done, return success inmediatelly. */
      result = 0;
      if (out_size == 0)
        t_info->state = FS_TAR_CREATE_EOF;
      goto tar_create_read_success;

    case FS_TAR_CREATE_EOF:
      efs_errno = EEOF;
      goto tar_create_read_error;

    }

    out_size += result;
    buffer += result;
    buf_size -= result;
  }

tar_create_read_success:
  return out_size;
tar_create_read_error:
  t_info->state = FS_TAR_CREATE_EOF;
  return -1;
}



int
fs_tar_create_close (struct fs_tar_create_info *t_info)
{
  int result;

  if (t_info == NULL || t_info->in_use != 1)
  {
    efs_errno = EINVAL;
    return -1;
  }

  result = efs_closedir_recursive (t_info->rec_iter);
  t_info->zero_filled_frames_sent = 0;
  t_info->state = FS_TAR_CREATE_HEADER;
  t_info->entry_size = 0;
  t_info->curr_fp = NULL;
  t_info->item_path = NULL;
  t_info->rec_iter = NULL;
  t_info->in_use = 0;

  return result;
}


#endif /* FEATURE_FS_TAR */

