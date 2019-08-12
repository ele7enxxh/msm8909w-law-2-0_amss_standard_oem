/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER EFS

GENERAL DESCRIPTION
  This file contains the functions used to manage phonebook entries within the
  EFS.

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmefs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/16   nr      Fix heap corruption issue in pbm_efs_read()
05/24/16   sp      Logging improvements
04/03/15   stv     Migrate EFS items from PBM to uim_common_efs
09/18/14   stv     Fix for logging strings in F3s
08/05/14   NR      Removal of unused feature flags
07/07/14   NR      Use v2.0 diag macros
05/07/14   stv     migration of memcpy to memscpy
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
04/14/14   stv     EFS write optimisation during garbage collection
03/25/10  krishnac Removing TASKLOCK and TASKFREE
03/25/10  krishnac Migrated from EFS1 to EFS2
07/10/09  ashwanik Support for CSIM and Dual Sim Features
01/28/08   clm     Do not free write cmd buffer if file write fails.  This
                   will be freed when the file is closed.
01/16/07   cvs     Add event for phonebooks becoming ready
10/10/06   cvs     Move messages to PBM's diag msg ID
05/03/06   cvs     Lint cleanup.
04/12/06   cvs     better logging
03/24/06   cvs     %x -> 0x%x in log messages
02/23/06   cvs     rework memory allocation macros
01/09/06   cvs     Lint fixes
11/15/05   cvs     Lint fixes
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/05/05   cvs     Revert previous change
08/05/05   cvs     Finish initing EFS PBs
07/29/05   AT      Fixed race condition between PBM and FS.  When doing
                   a deletion, the fs_close_cb would free the write command
                   buffer.  We then would check a variable in this buffer,
                   this causes big problems when PBM is getting deletions and
                   writes quicker than the PBM task gets the processor.
07/27/05   AT      With increased maximum size, dynamically allocating EFS
                   buffer.
07/19/05   AT      Added a range change.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/10/04   cvs     Add missing critical section in pbm_efs_garbage_recollect()
04/14/05   cvs     Fix bug where PB cache wouldn't go back to ready
                   after garbage collection
04/07/05   cvs     Better error handling if UID field is wrong size at init
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/08/05   PA      Support for PBM notifications.
01/20/05   PA      Support for read lock and reentrant complex field
                   enumeration.
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added more PBM kick dogs.
12/08/04   PA      Kick dog before efs_init to be safe
11/19/04   PA      Start unique id at 1.
11/18/04   PA      Support for complex fields and garbage collection with
                   write lock.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "mcfg_fs.h"

#include "pbm.h"
#include "pbmefs.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "stdio.h"
#include "stringl/stringl.h"

#define PBM_GARBAGE_HEAP_SIZE  (5*PBM_MAX_EFS_REC_LEN)

#define PBMEFS_MASTER_FILENAME      "pbm_master_v_20.dat"
#define PBMEFS_DELETE_FILENAME      "pbm_delete_file.dat"
#define PBMEFS_NEW_MASTER_FILENAME  "new_master_file.dat"
#define PBMEFS_CRPT_MASTER_FILENAME "cor_master_file.dat"
#define PBMEFS_V1_MASTER_FILENAME   "pbm_master_file.dat"
#define PBMEFS_OLD_MASTER_FILENAME  "old_master_file.dat"
#define PBMEFS_PH_UID_FILENAME      "pbm_phone_uid.dat"

/*===========================================================================
    Declare static globals for EFS
===========================================================================*/
#ifdef FEATURE_MMCP_DISABLE_PBM_EFS
boolean pbm_efs_disable=TRUE;
#else
boolean pbm_efs_disable=FALSE;
#endif
/* Will be TRUE if EFS support is to be disabled , by default FALSE here */

/* Buffer to handle async EFS writes */
static pbm_async_efs_buffer pbm_efs_buffers[PBM_ASYNC_BUFFERS];

/* Assuming that fs_write callbacks will be in the same order as fs_writes */
static uint8 pbm_efs_buf_first = 0;
static uint8 pbm_efs_buf_last = 0;

/* Boolean to differentiate between empty and full condition,
 * pbm_async_efs_buffer is much bigger than unsigned char */
static boolean pbm_efs_buf_empty = TRUE;

/* Flag to indicate whether the phone unique ID file got reset/corrupted. */
static boolean g_ph_uid_is_reset = FALSE;

/* This is the "phone unique ID", intended to remain unique for the life of the record */
static uint64 g_ph_unique_id = 0;

/* This unique ID is for tracking purposes only, and is subject to change. */
static uint16 g_unique_id = 1;

/* The master_filename is the current version of master file. */
  static char *g_master_filename      = PBMEFS_MASTER_FILENAME;
  static char *g_delete_filename      = PBMEFS_DELETE_FILENAME;

/* A "new" master file is created from existing master file and delete file. */
  static char *g_new_master_filename  = PBMEFS_NEW_MASTER_FILENAME;

/* These file are backed up in the case we find an error in the master file. */
  static char *g_crpt_master_filename = PBMEFS_CRPT_MASTER_FILENAME;

/* Outdated master file names. */
  static char *g_v1_master_filename   = PBMEFS_V1_MASTER_FILENAME;
  static char *g_old_master_filename  = PBMEFS_OLD_MASTER_FILENAME;

/* Phone unique ID file name */
  static char *g_ph_uid_filename      = PBMEFS_PH_UID_FILENAME;

static pbm_return_type pbm_efs_cache_init(boolean powerup);

static pbm_return_type pbm_convert_old_files(void);
static pbm_return_type pbm_efs_garbage_collect(boolean);
static pbm_return_type pbm_create_initial_ph_uid_file(uint64 starting_value, boolean uid_is_reset);
static pbm_return_type pbm_init_phone_unique_id(void);
static pbm_return_type pbm_renumber_with_unique_id(void);

/* <EJECT> */
/*==========================================================================*
 *                      PBM EFS UTILITIES                                   *
 *==========================================================================*/

/*===========================================================================
FUNCTION PBM_EFS_INIT

DESCRIPTION
  Initialize EFS based phonebooks.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_init()
{
  pbm_phonebook_type               pb_id           = {0};
  uim_common_efs_status_enum_type  efs_status      = UIM_COMMON_EFS_ERROR;

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_EFS_SUPPORT_DISABLE,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                  (uint8 *)&pbm_efs_disable,
                                  (uint32)sizeof(boolean));

  UIM_MSG_HIGH_2("pbm_efs_disable 0x%x efs_status 0x%x",
                  pbm_efs_disable, efs_status);

  if ( pbm_efs_disable )
  {
    pb_id.device_type = PBM_EFS;
    if (PBM_SUCCESS != pbm_efs_cache_structure_init(&efs_cache, pb_id,
                                                  0, FALSE))
        return PBM_ERROR;

    pb_id.device_type = PBM_SDL;
    if (PBM_SUCCESS != pbm_efs_cache_structure_init(&sdl_cache, pb_id,
                                                  0, FALSE))
        return PBM_ERROR;

    pb_id.device_type = PBM_RCVD;
    if (PBM_SUCCESS != pbm_efs_cache_structure_init(&incoming_calls_cache, pb_id,
                                                  0, TRUE))
        return PBM_ERROR;

    pb_id.device_type = PBM_DIALED;
    if (PBM_SUCCESS != pbm_efs_cache_structure_init(&outgoing_calls_cache, pb_id,
                                                  0, TRUE))
        return PBM_ERROR;

    pb_id.device_type = PBM_MISS;
    if (PBM_SUCCESS != pbm_efs_cache_structure_init(&missed_calls_cache, pb_id,
                                                  0, TRUE))
        return PBM_ERROR;

  }
  else
  {

    pbm_return_type ret_val;

    pbm_kick_dog();

    ret_val = pbm_convert_old_files();
    if (ret_val != PBM_SUCCESS)
    {
      return ret_val;
    }

    ret_val = pbm_efs_garbage_collect(TRUE);
    if (ret_val != PBM_SUCCESS)
    {
      return ret_val;
    }

    ret_val = pbm_efs_cache_init(TRUE);
    if (ret_val != PBM_SUCCESS)
    {
      return ret_val;
    }

    ret_val = pbm_init_phone_unique_id();
    if (ret_val != PBM_SUCCESS)
    {
      return ret_val;
    }

    /* Do not set any cache that uses master phonebook as ready
     * until after initializing the PH UID. */
  
    if(efs_cache.num_of_records > 0)
    {
      PBM_SET_PB_CACHE_READY(&efs_cache);
    }
    if(sdl_cache.num_of_records > 0)
    {
      PBM_SET_PB_CACHE_READY(&sdl_cache);
    }
    if(incoming_calls_cache.num_of_records > 0)
    {
      PBM_SET_PB_CACHE_READY(&incoming_calls_cache);
    }
    if(outgoing_calls_cache.num_of_records > 0)
    {
      PBM_SET_PB_CACHE_READY(&outgoing_calls_cache);
    }
    if(missed_calls_cache.num_of_records > 0)
    {
      PBM_SET_PB_CACHE_READY(&missed_calls_cache);
    }
  }
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_SET_UNIQUE_ID

DESCRIPTION
Sets the Unique ID for a record stored in EFS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_efs_set_unique_id(uint16 unique_id)
{
  g_unique_id = unique_id;
  return;
}


/*===========================================================================
FUNCTION PBM_CLEANUP_CORRUPT_FILES

DESCRIPTION
  This function should *never* be called.  However, we have a lot of code
  that checks for corruption in the master file.  There should be no way
  for this to happen, but if it ever does, this function will allow someone
  to salvage the phone book, and allow the user to continue to use it.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_cleanup_corrupt_files(char *filename)
{
  pbm_kick_dog();
  /* Remove Old backup of a corrupt master file . */
  (void)mcfg_fs_delete((const char*)g_crpt_master_filename,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  /* Rename the given file as corrupt master file*/
  (void)mcfg_fs_rename((const char*)filename,
                       (const char*) g_crpt_master_filename,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_NONE);

  /* Remove Delete File*/
  (void)mcfg_fs_delete((const char*)g_delete_filename,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

}


/*===========================================================================
FUNCTION PBM_EFS_READ_V1

DESCRIPTION
  This function gets the next (non-deleted) address record in the
  Master File.  If no record has yet been read, reads the first record.
  If at EOF, returns PBM_EOF. The calling function must allocate return_rec,
  but pbm_efs_read() mallocs fields and data that are pointed to by
  return_rec.(Other error conditions????)  This can be designed as a
  sequential read, since it is only used at startup, and no random-access
  reads are required.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_read_v1(int fd, pbm_addr_cache_s_type *return_rec)
{
  char                 *buffer              = NULL ;
  pbm_record_id_type    rec_len             = 0;
  uint16                data_len            = 0;
  uint16                offset              = 0;
  uint16                temp_offset         = 0;
  uint16                i                   = 0;
  uint16                j                   = 0;
  uint16                actual_rec_len_size = 0;
  uint16                efs_recid           = 0;
  uint16                temp_rec_len        = 0;
  uint32               *data_sizes          = NULL;
  int                   read_count          = 0;

  PBM_CHECK_PTR_RET(return_rec, PBM_ERROR);

  PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN, NO_ACTION);

  /* Start by reading the record length*/
  /* Note: record length provided is the *total*, including the 2 bytes for*/
  /*       the length info. itself*/
  pbm_kick_dog();
  read_count = mcfg_fread(fd, (void *)buffer,(uint32)sizeof(efs_recid),MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != sizeof(efs_recid)))
  {
    if (mcfg_fclose (fd, MCFG_FS_TYPE_EFS))
    {
      PBM_MEM_FREEIF(buffer);
      return PBM_ERROR;
    }
    if (read_count == 0)
    {
      /* If EOF, close master file and return PBM_EOF.*/
      PBM_MEM_FREEIF(buffer);
      return PBM_EOF;
    }
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  memscpy(&temp_rec_len, sizeof(temp_rec_len), buffer, sizeof(efs_recid));
  rec_len = temp_rec_len;
  actual_rec_len_size = sizeof(efs_recid);

  /* read next 2 bytes here, if they are zero that would mean record is 4 bytes */
  read_count = mcfg_fread (fd,
                           (void *)buffer,
                           (uint32)sizeof(efs_recid),
                           MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != sizeof(efs_recid)))
  {
     if (mcfg_fclose (fd, MCFG_FS_TYPE_EFS))
     {
       PBM_MEM_FREEIF(buffer);
       return PBM_ERROR;
     }
     if (read_count == 0)
     {
       /* If EOF, close master file and return PBM_EOF.*/
       PBM_MEM_FREEIF(buffer);
       return PBM_EOF;
     }
     PBM_MEM_FREEIF(buffer);
     UIM_MSG_ERR_0("Error reading Master File");
     return PBM_ERROR;
  }

  if( (*buffer == 0) && (*(buffer +1) == 0) ) /* the efs file comes from the code base where rec_len was 4 bytes */
  {
     actual_rec_len_size = sizeof(efs_recid) + 2;
  }
  else
  {
    /* we already have unique id in the buffer and file pointer is placed after it */
    /* need to seek File pointer 2 position back where it now is */
    temp_offset = sizeof(uint16);
  }

  if (rec_len > PBM_MAX_EFS_REC_LEN ||
      rec_len <= (actual_rec_len_size + temp_offset))
  {
    (void)mcfg_fclose (fd, MCFG_FS_TYPE_EFS);
     UIM_MSG_ERR_2("rec_len is either too short or too long than expected :0x%x actual_rec_len_size 0x%x",
                   rec_len, actual_rec_len_size);
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Record Length is OK.  Now read the rest of the record*/
  /* here for reading the rest of record, length is to be calculated carefully */
  /* since we can have rest of the record length as, rec_len - 4 or rec_len - 2*/
  read_count = mcfg_fread (fd,
                           (void *)buffer,
                           (uint32)(rec_len - actual_rec_len_size - temp_offset),
                           MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != (rec_len - actual_rec_len_size - temp_offset)) )
  {
    (void)mcfg_fclose (fd, MCFG_FS_TYPE_EFS);
    UIM_MSG_ERR_0("Error reading Master File");
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  offset = 0;

  memscpy(&(return_rec->unique_id), sizeof(return_rec->unique_id),buffer + offset,
           sizeof(return_rec->unique_id));
  offset += sizeof(return_rec->unique_id);

  /* use actual_rec_len_size to know if the record-id is of 4 bytes or 2 bytes */
  return_rec->record_id = 0;
  memscpy(&(return_rec->record_id),sizeof(return_rec->record_id),
          buffer + offset, actual_rec_len_size);
  offset += actual_rec_len_size;
  UIM_MSG_HIGH_3("pbm_efs_read_v1 length of record read %d, record id read %d offset is %d", rec_len, return_rec->record_id, offset);

  memscpy(&(return_rec->category), sizeof(return_rec->category),
           buffer + offset, sizeof(return_rec->category));
  offset += sizeof(return_rec->category);

  /* Count the number of fields in the record*/
  for (i = 0; offset < (rec_len - actual_rec_len_size); i++)
  {
    /* Advance over field ID and field type*/
    offset += sizeof(return_rec->fields[0].field_id) +
              sizeof(return_rec->fields[0].field_type);
    memscpy(&data_len, sizeof(uint16), buffer + offset, sizeof(data_len));
    offset += sizeof(data_len);
    offset += data_len;  /* Advance over the data*/
  }

  if (offset != (rec_len - actual_rec_len_size))
  {
    /* File is corrupt!*/
    (void)mcfg_fclose (fd, MCFG_FS_TYPE_EFS);
    UIM_MSG_ERR_0("Master File is corrupt. Offset did not matched");
    PBM_MEM_FREEIF(buffer);
    return PBM_ERROR;
  }

  /* Now i is the number of fields in the record*/
  return_rec->num_fields = i;
  PBM_ALLOC_MEM_W_ERR_RET(data_sizes, i * sizeof(uint32), PBM_MEM_FREEIF(buffer););
  if (data_sizes != NULL)
  {
    boolean  ret_val = FALSE;

    /* Set offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             actual_rec_len_size +
             sizeof(return_rec->category);

    /* Determine the field data sizes and copy the data in.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint16 temp_size = 0;

      offset += sizeof(return_rec->fields[0].field_id) +
                sizeof(return_rec->fields[0].field_type);

      memscpy( &(temp_size), sizeof(uint16), buffer + offset,
              sizeof(return_rec->fields[0].data_len) );
      data_sizes[j] = (int)temp_size;
      offset += sizeof(return_rec->fields[0].data_len);

      offset += (uint16) data_sizes[j];
    }

    ret_val = pbm_allocate_fields_internal( &(return_rec->fields),
                                   (uint32) return_rec->num_fields,
                                   data_sizes );
    PBM_MEM_FREEIF(data_sizes);
    PBM_CHECK_PTR_RET(return_rec->fields, PBM_ERROR);
    if (!ret_val)
    {
      PBM_MEM_FREEIF(buffer);
      return PBM_ERROR;
    }

    /* Reset offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             actual_rec_len_size +
             sizeof(return_rec->category);

    /* Copy in the field info.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      void  *field_ptr = NULL ;

      memscpy(&(return_rec->fields[j].field_id),
              sizeof(return_rec->fields[0].field_id),
              buffer + offset,
              sizeof(return_rec->fields[0].field_id));

      offset += sizeof(return_rec->fields[0].field_id);

      memscpy(&(return_rec->fields[j].field_type),
              sizeof(return_rec->fields[0].field_type),
              buffer + offset,
              sizeof(return_rec->fields[0].field_type));

      offset += sizeof(return_rec->fields[0].field_type);

      /* Fix an issue where we could have written garbage to the
       * upper 16 bytes of the field_type.  Safely assume high
       * 16 bits of field_type should have been 0's when file was
       * written.
       */
      return_rec->fields[j].field_type &= (pbm_field_type_e_type)0x0000FFFF;

      memscpy(&(return_rec->fields[j].data_len),
              sizeof(return_rec->fields[0].data_len),
              buffer + offset,
              sizeof(return_rec->fields[0].data_len));

      offset += sizeof(return_rec->fields[0].data_len);

      field_ptr = pbm_get_field_data_ptr(return_rec->fields,j);
      if (field_ptr == NULL)
      {
        UIM_MSG_ERR_2("field_ptr NULL 0x%x 0x%x", return_rec->fields, j);

        PBM_MEM_FREEIF(buffer);
        return PBM_ERROR;
      }

      memscpy(field_ptr,
              return_rec->fields[j].data_len,
              buffer + offset,
              return_rec->fields[j].data_len);

      offset += return_rec->fields[j].data_len;
    }
  }

  PBM_MEM_FREEIF(buffer);
  return PBM_SUCCESS;
}/* pbm_efs_read_v1 */


/*===========================================================================
FUNCTION PBM_EFS_READ

DESCRIPTION
  This function gets the next (non-deleted) address record in the
  Master File.  If no record has yet been read, reads the first record.
  If at EOF, returns PBM_EOF. The calling function must allocate return_rec,
  but pbm_efs_read() mallocs fields and data that are pointed to by
  return_rec.(Other error conditions????)  This can be designed as a
  sequential read, since it is only used at startup, and no random-access
  reads are required.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_read(int fd, pbm_addr_cache_s_type *return_rec)
{
  char                 *buffer              = NULL;
  int                   read_count          = 0;
  pbm_record_id_type    rec_len             = 0;
  uint16                data_len            = 0;
  uint16                offset              = 0;
  uint16                temp_offset         = 0;
  uint16                i                   = 0;
  uint16                j                   = 0;
  uint16                actual_rec_len_size = 0;
  uint32               *data_sizes          = NULL;
  boolean               alloc_ret           = FALSE;
  uint16                efs_recid           = 0;
  uint16                temp_rec_len        = 0;
  pbm_return_type       ret_val             = PBM_ERROR;

  PBM_CHECK_PTR_RET(return_rec, PBM_ERROR);

  PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN, NO_ACTION);

  /* Start by reading the record length*/
  /* Note: record length provided is the *total*, including the 2 bytes for*/
  /*       the length info. itself*/
  pbm_kick_dog();

  read_count = mcfg_fread (fd,
                           (void *)buffer,
                           (uint32)sizeof(efs_recid),
                            MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != sizeof(efs_recid)))
  {
    if(read_count == 0)
    {
      /* If EOF, close master file and return PBM_EOF.*/
      ret_val = PBM_EOF;
      goto send_result;
    }
    goto send_result;
  }

  memscpy(&temp_rec_len, sizeof(temp_rec_len), buffer, sizeof(efs_recid));
  rec_len = temp_rec_len;
  actual_rec_len_size = sizeof(efs_recid);
   /* read next 2 bytes here, if they are zero that would mean record is 4 bytes */
  read_count = mcfg_fread (fd,
                           (void *)buffer,
                           (uint32) sizeof(efs_recid),
                           MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != sizeof(efs_recid)))
  {
    if(read_count == 0)
    {
      /* If EOF, close master file and return PBM_EOF.*/
      ret_val = PBM_EOF;
      goto send_result;
    }
    goto send_result;
  }

/* the efs file comes from the code base where rec_len was 4 bytes */
  if( (*buffer == 0) && (*(buffer +1) == 0) )
  {
     actual_rec_len_size = sizeof(efs_recid) + 2;
  }
  else
  {
    /* we already have unique id in the buffer and file pointer is placed after it 
       since in case of 2 bytes record id, we ahve already read the unique id till this step
       we need to remember that information */
    temp_offset = sizeof(uint16);
  }

  if (rec_len > PBM_MAX_EFS_REC_LEN ||
      rec_len <= (actual_rec_len_size + temp_offset))
  {
    UIM_MSG_ERR_2("rec_len is either zero or too long than expected :0x%x actual_rec_len_size 0x%x",
                   rec_len, actual_rec_len_size);
    goto send_result;
  }

  /* Record Length is OK.  Now read the rest of the record*/
  read_count = mcfg_fread (fd,
                           (void *)(buffer + temp_offset),
                           (uint32)(rec_len -actual_rec_len_size - temp_offset),
                           MCFG_FS_TYPE_EFS);

  if ((read_count == -1) || (read_count != (rec_len - actual_rec_len_size - temp_offset)))
  {
    ret_val = PBM_ERROR;
    goto send_result;
  }

  memscpy(&(return_rec->unique_id),
          sizeof(return_rec->unique_id),
          buffer,
          sizeof(return_rec->unique_id));

  offset = sizeof(return_rec->unique_id);

  return_rec->record_id = 0;
  /* initialize record id here since record id is four bytes and in below memcpy
     we will copy just 2 bytes in case of 2 bytes record id */

  if( PBM_MAX_EFS_REC_LEN - offset >= actual_rec_len_size )
  {
    memscpy(&(return_rec->record_id),
            sizeof(return_rec->record_id),
            buffer + offset,
            actual_rec_len_size);
    offset += actual_rec_len_size;
  }
  else
  {
    goto send_result;
  }

  UIM_MSG_HIGH_3("pbm_efs_read length of record read 0x%x, record id 0x%x offset 0x%x",
                 rec_len, return_rec->record_id, offset);

  if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(return_rec->category) )
  {
    memscpy(&(return_rec->category),
            sizeof(return_rec->category),
            buffer + offset,
            sizeof(return_rec->category));

    offset += sizeof(return_rec->category);
  }
  else
  {
    goto send_result;
  }

  /* Count the number of fields in the record*/
  for (i = 0; offset < (rec_len - actual_rec_len_size); i++)
  {
    /* Advance over field ID and field type*/
    offset += sizeof(return_rec->fields[0].field_id);
    offset += sizeof(uint32); /* return_rec->fields[0].field_type */

    /* Copy the data length. */
    if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(data_len) )
    {
      memscpy(&data_len, sizeof(uint16), buffer + offset, sizeof(data_len));
      offset += sizeof(data_len);
    }
    else
    {
      goto send_result;
    }

    /* Advance over the data*/
    offset += data_len;
  }

  if (offset != (rec_len - actual_rec_len_size))
  {
    /* File is corrupt!*/
    goto send_result;
  }

  /* Now i is the number of fields in the record*/
  return_rec->num_fields = i;
  PBM_ALLOC_MEM_W_ERR_RET(data_sizes, i * sizeof(uint32),
                          PBM_MEM_FREEIF(buffer));
  if (data_sizes != NULL)
  {
    /* Set offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             actual_rec_len_size+
             sizeof(return_rec->category);

    /* Determine the field data sizes and copy the data in.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint16  temp_size  = 0;

      offset += sizeof(return_rec->fields[0].field_id) +
                sizeof(uint32); /* return_rec->fields[0].field_type */

      if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(data_len) )
      {
        memscpy( &(temp_size), sizeof(uint16), buffer + offset,
                 sizeof(return_rec->fields[0].data_len) );
      }
      else
      {
        goto send_result;
      }

      data_sizes[j] = (int)temp_size;
      offset += sizeof(return_rec->fields[0].data_len);

      offset += (uint16) data_sizes[j];
    }

    alloc_ret = pbm_allocate_fields_internal( &(return_rec->fields),
                                             (uint32) return_rec->num_fields,
                                             data_sizes );

    PBM_MEM_FREEIF(data_sizes);
    if (!alloc_ret)
    {
      ret_val = PBM_ERROR_MEM_FULL;
      goto send_result;
    }

    /* Reset offset to beginning of fields*/
    offset = sizeof(return_rec->unique_id) +
             actual_rec_len_size +
             sizeof(return_rec->category);

    if (return_rec->fields == NULL)
    {
       goto send_result;
    }

    /* Copy in the field info.*/
    for (j = 0; j < return_rec->num_fields; j++)
    {
      uint32      temp_field_type = 0;
      void       *field_ptr       = NULL;

      if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(return_rec->fields[0].field_id))
      {
        memscpy(&(return_rec->fields[j].field_id),
                sizeof(return_rec->fields[0].field_id),
                buffer + offset,
                sizeof(return_rec->fields[0].field_id));
        offset += sizeof(return_rec->fields[0].field_id);
      }
      else
      {
        goto send_result;
      }

      if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(uint32))
      {
        memscpy(&temp_field_type,
                sizeof(uint32),
                buffer + offset,
                sizeof(uint32));

        return_rec->fields[j].field_type = (pbm_field_type_e_type)temp_field_type;
        offset += sizeof(uint32);
      }
      else
      {
        goto send_result;
      }

      if( PBM_MAX_EFS_REC_LEN - offset >= sizeof(return_rec->fields[0].data_len))
      {
        memscpy(&(return_rec->fields[j].data_len),
                sizeof(return_rec->fields[0].data_len),
                buffer + offset,
                sizeof(return_rec->fields[0].data_len) );

        offset += sizeof(return_rec->fields[0].data_len);
      }
      else
      {
        goto send_result;
      }

      field_ptr = pbm_get_field_data_ptr(return_rec->fields,j);
      if (field_ptr == NULL)
      {
        goto send_result;
      }

      if( PBM_MAX_EFS_REC_LEN - offset >= return_rec->fields[j].data_len )
      {
        memscpy( field_ptr,
                 return_rec->fields[j].data_len,
                 buffer + offset,
                 return_rec->fields[j].data_len );
      }
      else
      {
        goto send_result;
      }

      offset += return_rec->fields[j].data_len;
    }
  }

ret_val = PBM_SUCCESS;

send_result:
  UIM_MSG_HIGH_1("pbm_efs_read called with ret_val 0x%x", ret_val);
  PBM_MEM_FREEIF(buffer);
  return ret_val;
} /* pbm_efs_read */


/*=======================================================================
/FUNCTION pbm_convert_efs_rec_to_buf
 DESCRIPTION
  Converts the record read from EFS to buffer format.

DEPENDENCIES

SIDE EFFECTS

=========================================================================*/
pbm_return_type pbm_convert_efs_rec_to_buf(uint16 *buf_offset, const pbm_addr_cache_s_type *new_record)
{

  uint16 efs_recid = 0,count = 0,rec_length;
  uint16 offset=0, buffer_len = 0 ;
  char *buffer = pbm_efs_buffers[pbm_efs_buf_last].buffer;
  int i;

  PBM_CHECK_PTR2_RET(new_record,buf_offset,PBM_ERROR);
  UIM_MSG_HIGH_1("calling pbm_convert_efs_rec_to_buf %d",pbm_efs_buf_last);

  buffer_len = sizeof( pbm_efs_buffers[pbm_efs_buf_last].buffer ) ;
  memset(buffer, 0, buffer_len );

  efs_recid = (uint16)new_record->record_id;
  count = sizeof(rec_length) +
          sizeof(new_record->unique_id) +
          sizeof(efs_recid) +
          sizeof(new_record->category);

  for (i = 0; i < new_record->num_fields; i++)
  {
    count+= sizeof(new_record->fields[i].field_id) +
            sizeof(uint32) + /* new_record->fields[i].field_type */
            sizeof(new_record->fields[i].data_len) +
            new_record->fields[i].data_len;
  }

  rec_length = count;

  if (rec_length > PBM_MAX_EFS_REC_LEN)
  {
    UIM_MSG_ERR_1("Record len %d is longer than max. allowed record length.", rec_length);
    return PBM_ERROR;
  }

  memscpy(buffer, buffer_len , &rec_length, sizeof(rec_length));
  offset += MIN ( buffer_len ,sizeof(rec_length) );

  memscpy(buffer + offset, buffer_len - offset , &(new_record->unique_id), sizeof(new_record->unique_id));
  offset += MIN ( buffer_len - offset ,sizeof(new_record->unique_id) ) ;

  memscpy(buffer + offset, buffer_len - offset , &efs_recid, sizeof(efs_recid));
  offset += MIN ( buffer_len - offset , sizeof(efs_recid) ) ;

  memscpy(buffer + offset, buffer_len - offset , &(new_record->category), sizeof(new_record->category));
  offset += MIN ( buffer_len - offset , sizeof(new_record->category) ) ;

  for (i = 0; i < new_record->num_fields; i++)
  {
    uint32 temp_field_type = 0;
    void *field_ptr;

    if (new_record->fields == NULL)
    {
      UIM_MSG_ERR_0("new_record->fields NULL");
      return PBM_ERROR;
    }

    memscpy( buffer + offset, buffer_len - offset ,
             &(new_record->fields[i].field_id),
            sizeof(new_record->fields[i].field_id));
    offset += MIN ( buffer_len - offset ,sizeof(new_record->fields[i].field_id) );

    temp_field_type = (uint32)new_record->fields[i].field_type;
    memscpy( buffer + offset, buffer_len - offset , &temp_field_type, sizeof(uint32));
    offset += MIN ( buffer_len - offset , sizeof(uint32) );

    memscpy( buffer + offset, buffer_len - offset , &(new_record->fields[i].data_len),
            sizeof(new_record->fields[i].data_len));
    offset += MIN ( buffer_len - offset , sizeof(new_record->fields[i].data_len) );

    field_ptr = pbm_get_field_data_ptr(new_record->fields,i);
    if (field_ptr == NULL)
    {
      UIM_MSG_ERR_2("field_ptr NULL 0x%x %d",new_record->fields,i);
      return PBM_ERROR;
    }

    memscpy(buffer + offset, buffer_len - offset , field_ptr,
           new_record->fields[i].data_len);
    offset += MIN( buffer_len - offset  , new_record->fields[i].data_len );
  }

  if(buf_offset)
     *buf_offset = offset;

  UIM_MSG_HIGH_2("Offset is %d and %d",offset,*buf_offset);


  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_GENERATE_UNIQUE_ID

DESCRIPTION
Generates a Unique ID for a record stored in EFS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint16 pbm_efs_generate_unique_id(void)
{
  return (g_unique_id++);
}

/*===========================================================================
FUNCTION PBM_EFS_ADD

DESCRIPTION
  Adds a record to the end of the Master File.

DEPENDENCIES
  Must be called from within a critical section.

SIDE EFFECTS
===========================================================================*/
static int pbm_efs_add_open(const char *filename)
{
  int fd = -1;
  PBM_CHECK_PTR_RET(filename, -1);

  /* Open Master File for append*/
  pbm_kick_dog();
  fd = mcfg_fopen (filename,
                   O_APPEND | MCFG_FS_O_RDWR,
                   MCFG_FS_ALLPERMS,
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE);
  if (fd<0)
  {
    fd = mcfg_fopen (filename,
                     MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);
    if (fd<0)
    {
      /* Still doesn't work*/
      return -1;
    }
  }
  return fd;
}

/*===========================================================================
FUNCTION PBM_EFS_ADD

DESCRIPTION
  Adds a record to the end of the Master File.

DEPENDENCIES
  Must be called from within a critical section.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_add(int fd, const pbm_addr_cache_s_type *new_record, boolean async)
{
  char *buffer = pbm_efs_buffers[pbm_efs_buf_last].buffer;
  uint16 rec_len;
  pbm_return_type ret_val = PBM_ERROR;
  boolean opened = FALSE;
  int result;
  pbm_async_efs_buffer *buf;
  int close_status;

  PBM_CHECK_PTR_RET(new_record, PBM_ERROR);

  if (fd < 0)
  {
    fd = pbm_efs_add_open(g_master_filename);
    if (fd < 0)
      return PBM_ERROR;

    opened = TRUE;
  }

  ret_val = pbm_convert_efs_rec_to_buf(&rec_len, new_record);
  if (ret_val != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("Error 0x%x Failed Conversion of record to buffer .", ret_val);
  }

  /* Write the buffer to the file*/
  pbm_kick_dog();
  if (async)
  {
    pbm_efs_buffers[pbm_efs_buf_last].count = rec_len;
    pbm_efs_buf_last = (pbm_efs_buf_last + 1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = FALSE;

    result = mcfg_fwrite(fd,(void *)buffer,(uint32)rec_len,MCFG_FS_TYPE_EFS);
    UIM_MSG_HIGH_1("efs write rec len %d ",rec_len);
    close_status = mcfg_fclose (fd, MCFG_FS_TYPE_EFS);
    buf = &pbm_efs_buffers[pbm_efs_buf_first];
    if((result == -1) || (result < (int32) rec_len)) /* signed unsigned relational operator error */
    {
        pbm_list_recover(buf->write, buf->deleted);
        pbm_write_notify(buf->write, PBM_ERROR);

    }
    else
    {
      if (close_status)
      {
        pbm_list_recover(buf->write, buf->deleted);
        pbm_write_notify(buf->write, PBM_ERROR);

      }
      else
      {
        pbm_list_update(buf->write);
        pbm_write_notify(buf->write, PBM_SUCCESS);
      }
    }

    pbm_write_cmd_free(buf->write);

    /* After the close, we are done with this callback, and can get
     * the next one. */
    pbm_efs_buf_first = (pbm_efs_buf_first+1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = (pbm_efs_buf_first == pbm_efs_buf_last);

  }
  else
  {
    result = mcfg_fwrite (fd, (void *)buffer,(uint32) rec_len,MCFG_FS_TYPE_EFS);
    UIM_MSG_HIGH_1("efs write rec len %d ",rec_len);
    if ((result == -1) || (result!= rec_len))
    {
      if (opened)
      {
        (void)mcfg_fclose (fd, MCFG_FS_TYPE_EFS);
      }
      return PBM_ERROR;
    }

    /* Close the file*/
    if (opened)
    {
      if (mcfg_fclose (fd, MCFG_FS_TYPE_EFS))
      {
        return PBM_ERROR;
      }
    }
  }

  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_EFS_DELETE

DESCRIPTION
Adds a record's Unique ID to the Delete File.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_delete(const pbm_addr_cache_s_type *record_to_delete,
                               boolean async)
{
  static int            file_descriptor = -1;
  int                   fd              = -1;
  int                   result          = 0;
  pbm_async_efs_buffer *buf             = NULL;
  int                   close_status    = 0;

  PBM_CHECK_PTR_RET(record_to_delete, PBM_ERROR);

  /* Open Delete File for append*/
  fd = mcfg_fopen ((const char*)g_delete_filename,
                    O_APPEND | MCFG_FS_O_RDWR,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    MCFG_FS_SUBID_NONE);

  if (fd<0)
  {
    fd = mcfg_fopen ((const char*)g_delete_filename,
                      MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      /* Still doesn't work*/
      return PBM_ERROR;
    }
  }
  file_descriptor=fd;

  /* Write the Unique ID to the file*/
  pbm_kick_dog();
  if (async)
  {
    char *buffer = pbm_efs_buffers[pbm_efs_buf_last].buffer;

    pbm_efs_buffers[pbm_efs_buf_last].count =
                                    sizeof(record_to_delete->unique_id);
    pbm_efs_buf_last = (pbm_efs_buf_last + 1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = FALSE;
    memscpy(buffer, sizeof(record_to_delete->unique_id),
           &(record_to_delete->unique_id),sizeof(record_to_delete->unique_id));
    result = mcfg_fwrite (file_descriptor,
                          (void *)buffer,
                          (uint32)sizeof(record_to_delete->unique_id),
                          MCFG_FS_TYPE_EFS);

    close_status = mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);
    buf = &pbm_efs_buffers[pbm_efs_buf_first];
    if((result == -1) || (result < (int)sizeof(record_to_delete->unique_id)))
    {
        pbm_list_recover(buf->write, buf->deleted);
        pbm_write_notify(buf->write, PBM_ERROR);

    }
    else
    {
      if (close_status)
      {
        pbm_list_recover(buf->write, buf->deleted);
        pbm_write_notify(buf->write, PBM_ERROR);

      }
      else
      {
        pbm_list_update(buf->write);
        pbm_write_notify(buf->write, PBM_SUCCESS);
      }
    }

    pbm_write_cmd_free(buf->write);

    /* After the close, we are done with this callback, and can get
     * the next one. */
    pbm_efs_buf_first = (pbm_efs_buf_first+1)%PBM_ASYNC_BUFFERS;
    pbm_efs_buf_empty = (pbm_efs_buf_first == pbm_efs_buf_last);

  } else
  {
    result = mcfg_fwrite (file_descriptor,
                         (void *)&(record_to_delete->unique_id),
                         (uint32)sizeof(record_to_delete->unique_id),
                          MCFG_FS_TYPE_EFS);

    if ((result == -1) || (result != (sizeof(record_to_delete->unique_id))))
    {
      (void)mcfg_fclose(file_descriptor, MCFG_FS_TYPE_EFS);
      return PBM_ERROR;
    }

    /* Close the file*/
    if (mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS))
    {
      return PBM_ERROR;
    }
  }

  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_CONVERT_OLD_FILES

DESCRIPTION
  Looks at the EFS and makes sure the master_file is the latest
  version.  If its not, then it updates it and removes old files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_convert_old_files(void)
{
  pbm_return_type                ret_val     = PBM_SUCCESS;
  int                            old_fd      = -1;
  int                            new_fd      = -1;
  int                            fd          = -1;
  int                            v1_status;
  struct fs_stat                 sbuf;
  struct fs_stat                 sbuf1;
  char                           *buffer     = NULL;
  pbm_addr_cache_s_type           record;
  mcfg_fs_status_e_type           status     = MCFG_FS_STATUS_EFS_ERR;

  pbm_kick_dog();

  /* Remove the backup Master File if one exists */
  (void)mcfg_fs_delete((const char*)g_old_master_filename,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  /* Check if "Version 1" master file exists. */
  status = mcfg_fs_stat((const char *)g_v1_master_filename,
                        &sbuf,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  v1_status = efs_errno;
  if(v1_status == ENOERR && status  == MCFG_FS_STATUS_OK)
  {
    dword delete_size = 0;  /* num of elements in delete list*/

    /* Check if delete file exists, if its too big, we'll just delete the
     * old stuff. */

    status = mcfg_fs_stat((const char *)g_delete_filename,
                          &sbuf1,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

    if(status == MCFG_FS_STATUS_OK)
    {
      /* The file exists. */
      delete_size = sbuf1.st_size/sizeof(unsigned short);
      if (delete_size > PBM_MAX_DELETE_SIZE)
      {

        (void)mcfg_fs_delete((const char *)g_v1_master_filename,
                                 MCFG_FS_TYPE_EFS,
                                 MCFG_FS_SUBID_NONE);

        (void)mcfg_fs_delete((const char *)g_delete_filename,
                                 MCFG_FS_TYPE_EFS,
                                 MCFG_FS_SUBID_NONE);
        return PBM_SUCCESS;
      }
    }
    else
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      UIM_MSG_ERR_1("efs_stat failed. 0x%x", status);
    }

    /* Open "V1" Master File*/
    pbm_kick_dog();
    fd = mcfg_fopen ((const char*)g_v1_master_filename,
                     MCFG_FS_O_RDONLY,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      return PBM_ERROR;
    }
    old_fd = fd;

    /* Remove the partially build V2 Master File if one exists */
    (void)mcfg_fs_delete((const char*)g_master_filename,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

    /* Open "V2" Master File*/
    fd = mcfg_fopen ((const char*)g_master_filename,
                      MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      (void)mcfg_fclose(old_fd, MCFG_FS_TYPE_EFS);
      return PBM_ERROR;
    }
    new_fd=fd;

    /* Allocate the read buffer*/
    PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN * sizeof(char),
            if (mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS))
              UIM_MSG_ERR_0("Could not close file ");
            if (mcfg_fclose (new_fd, MCFG_FS_TYPE_EFS))
              UIM_MSG_ERR_0("Could not close file "););

    while (PBM_SUCCESS == (ret_val = pbm_efs_read_v1(old_fd, &record)))
    {
      ret_val = pbm_efs_add(new_fd, &record, FALSE);
      if (ret_val != PBM_SUCCESS)
      {
        break;
      }
      pbm_free_fields_internal(&record.fields, record.num_fields);
    }
    if ((ret_val == PBM_ERROR_NOT_FOUND) || (ret_val == PBM_EOF))
    {
      ret_val = PBM_SUCCESS;
    }

    PBM_MEM_FREEIF(buffer);

    pbm_kick_dog();
    (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
    (void)mcfg_fclose (new_fd, MCFG_FS_TYPE_EFS);

    /* Remove the replaced V1 Master File */
    (void)mcfg_fs_delete((const char*)g_v1_master_filename,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_NONE);
  }
  else if (v1_status != ENOENT)
  {
      /* Test failed due to reason other than "File Does not Exists"*/
    UIM_MSG_ERR_1 ("efs_stat failed 0x%x", status);
  }

  return ret_val;
} /* pbm_convert_old_files */


/*===========================================================================
FUNCTION PBM_EFS_GARBAGE_COLLECT

DESCRIPTION
Copies the non-deleted records of the Master File to a new file and removes
the Delete File. The unique id is reset only if reset_uid is set to TRUE.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_garbage_collect(boolean reset_uid)
{
  static int                   old_fd                   = -1;
  static int                   new_fd                   = -1;
  static int                   delete_fd                = -1;
  int                          read_count               = 0;
  int                          fd                       = -1;
  static uint16               *delete_list              = NULL;
  static dword                 delete_size              = 0;
  pbm_addr_cache_s_type        record;
  char                        *buffer                   = NULL;
  uint16                       i                        = 0;
  boolean                      deleted_record           = FALSE;
  uint32                       num_deleted_records      = 0;
  uint16                       added_recs               = 0;
  pbm_return_type              ret_val;
  uint32                       garb_heap_offset         = 0;
  uint16                       buf_offset               = 0;
  int                          result                   = -1;
  char                        *buffer_record            = pbm_efs_buffers[pbm_efs_buf_last].buffer;
  char                        *pbm_garb_heap_buf        = NULL;
  mcfg_fs_status_e_type        status                   = MCFG_FS_STATUS_EFS_ERR;
  mcfg_fs_status_e_type        new_master_file_status   = MCFG_FS_STATUS_EFS_ERR;
  struct fs_stat               sbuf                     = {0};
  struct fs_stat               gdel                     = {0};
  struct fs_stat               gnewmaster               = {0};
  struct fs_stat               gmaster                  = {0};
  struct fs_stat               sbuf1                    = {0};

  /* Kick the dog as EFS operation might take some time */
  pbm_kick_dog();

  status = mcfg_fs_stat((const char *)g_delete_filename,
                          &sbuf,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

  if( status != MCFG_FS_STATUS_OK && efs_errno != ENOENT )
  {
    return PBM_ERROR;
  }
  else
  {
    gdel = sbuf;
  }


  new_master_file_status = mcfg_fs_stat((const char *)g_new_master_filename,
                                         &sbuf1,
                                         MCFG_FS_TYPE_EFS,
                                         MCFG_FS_SUBID_NONE);

  /* Check if "new" master file exists. */
  if( new_master_file_status == MCFG_FS_STATUS_OK )
  {
    if(status == MCFG_FS_STATUS_OK )
    {
      /* Delete file found. Remove the "new" file that was created and do
               garbage collection again this time. */
      status = mcfg_fs_delete((const char *)g_new_master_filename,
                              MCFG_FS_TYPE_EFS,
                              MCFG_FS_SUBID_NONE);

      if(status != MCFG_FS_STATUS_OK)
      {
        return PBM_ERROR;
      }
      gnewmaster =  sbuf1;
    }
    else
    {
      /* Delete file not found. Previous attempt at garbage collection
      * was successful. Copy "new" masterfile to the actual master file. */
      status = mcfg_fs_stat((const char *)g_master_filename,
                        &sbuf1,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

      if(status == MCFG_FS_STATUS_OK)
      {
        status = mcfg_fs_delete((const char *)g_master_filename,
                                 MCFG_FS_TYPE_EFS,
                                 MCFG_FS_SUBID_NONE);

        if( status == MCFG_FS_STATUS_OK )
        {
          gmaster = sbuf1;
        }
        else
        {
          return PBM_ERROR;
        }
      }
      else
      {
      /* Test failed due to reason other than "File Does not Exists"*/
        UIM_MSG_ERR_1 ("EFS stat for master file fail 0x%x", efs_errno);
      }

      if(mcfg_fs_rename((const char*)g_new_master_filename,
                        (const char*) g_master_filename,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE) !=0 )
      {
        return PBM_ERROR;
      }
    }
  }
  /* Return Success if delete file not found */
  status = mcfg_fs_stat((const char *)g_delete_filename,
                        &sbuf,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  if(status != MCFG_FS_STATUS_OK && efs_errno == ENOENT )
  {
    UIM_MSG_HIGH_0("Returning from garbage collect - No delete file");
    return PBM_SUCCESS;
  }

  delete_size = sbuf.st_size/sizeof(delete_list[0]);
  if ((delete_size * sizeof(delete_list[0])) != sbuf.st_size)
  {
    UIM_MSG_ERR_0("Delete file is not an even number of bytes.");
    return PBM_ERROR;
  }

  if (delete_size == 0)
  {
    /* No delete list -- we're done.*/
    return PBM_SUCCESS;
  }

  if (delete_size <= PBM_MAX_DELETE_SIZE)
  {
    /* Open the Delete File*/
    pbm_kick_dog();
    fd = mcfg_fopen ((const char*)g_delete_filename,
                     MCFG_FS_O_RDONLY,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      return PBM_ERROR;
    }

    delete_fd = fd;

    PBM_ALLOC_MEM_W_ERR_RET(delete_list, delete_size * sizeof(delete_list[0]),
                    NO_ACTION);

    /* Read in the delete list*/

    read_count = mcfg_fread (delete_fd,
                             (void *) delete_list,
                             (uint32)(delete_size*sizeof(delete_list[0])),
                             MCFG_FS_TYPE_EFS);

    if((read_count == -1) || (read_count != delete_size * sizeof(delete_list[0])))
    {
      PBM_MEM_FREEIF(delete_list);
      return PBM_ERROR;
    }

    if (mcfg_fclose (delete_fd, MCFG_FS_TYPE_EFS))
    {
      PBM_MEM_FREEIF(delete_list);
      return PBM_ERROR;
    }
    /* Open "old" Master File*/
    fd = mcfg_fopen ((const char*)g_master_filename,
                     MCFG_FS_O_RDONLY,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      PBM_MEM_FREEIF(delete_list);
      return PBM_ERROR;
    }
    old_fd = fd;

    /* Open "new" Master File*/

    fd = mcfg_fopen ((const char*)g_new_master_filename,
                     MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);

    if (fd<0)
    {
      PBM_MEM_FREEIF(delete_list);
      (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
      return PBM_ERROR;
    }
    new_fd = fd;

    /* Allocate the read buffer*/
    PBM_ALLOC_MEM_W_ERR_RET(buffer, PBM_MAX_EFS_REC_LEN * sizeof(char),
                            (void)mcfg_fclose(old_fd, MCFG_FS_TYPE_EFS);
                            PBM_MEM_FREEIF(delete_list););

    if (reset_uid)
      pbm_efs_set_unique_id (1);

    status = mcfg_fs_stat((const char *)g_master_filename,
                           &sbuf1,
                           MCFG_FS_TYPE_EFS,
                           MCFG_FS_SUBID_NONE);

    if(status == MCFG_FS_STATUS_OK)
    {
      gmaster = sbuf1;
    }

    PBM_MEM_ALLOC(pbm_garb_heap_buf,PBM_GARBAGE_HEAP_SIZE*sizeof(char));

    if( pbm_garb_heap_buf == NULL )
    {
      PBM_MEM_FREEIF(delete_list);
      PBM_MEM_FREEIF(buffer);
      (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
      return PBM_ERROR_MEM_FULL;
    }
    memset(pbm_garb_heap_buf , 0 , PBM_GARBAGE_HEAP_SIZE*sizeof(char) );
    while (PBM_SUCCESS == (ret_val = pbm_efs_read(old_fd, &record)))
    {
      UIM_MSG_HIGH_5("efs add recs 0x%x master size 0x%x record_id 0x%x efs del file size 0x%x new master size 0x%x",
                      added_recs, gmaster.st_size, record.record_id, gdel.st_size, gnewmaster.st_size);
      /* Check if it's a deleted record.*/
      deleted_record = FALSE;
      for (i = 0; i < delete_size; i++)
      {
        if (delete_list[i] == record.unique_id)
        {
          /* Deleted record -- toss out*/
          deleted_record = TRUE;
          break;
        }
      }

      if (!deleted_record)
      {
        if (reset_uid)
          record.unique_id = pbm_efs_generate_unique_id();
        ret_val = pbm_convert_efs_rec_to_buf(&buf_offset, &record);
        if (ret_val != PBM_SUCCESS)
        {
          UIM_MSG_ERR_1("Error 0x%x Failed Conversion of record to buffer .", ret_val);
          break;
        }
        added_recs++;
      } else
      {
        num_deleted_records++;
      }

      pbm_free_fields_internal(&record.fields, record.num_fields);

      if ( garb_heap_offset+buf_offset > PBM_GARBAGE_HEAP_SIZE)
      {
         result = mcfg_fwrite (new_fd,
                               (void *)pbm_garb_heap_buf,
                               garb_heap_offset,
                               MCFG_FS_TYPE_EFS);

         if((result == -1) || (result < (int32) garb_heap_offset))
         {
            UIM_MSG_ERR_3("Error writing to Master File %d fd %d garb_heap_offset %d",
                           result,new_fd,garb_heap_offset);
         }
         else
         {
           UIM_MSG_HIGH_2("EFS write successful %d garb_heap_offset %d",result,garb_heap_offset);
         }

         garb_heap_offset = 0;
         memset(pbm_garb_heap_buf , 0 , PBM_GARBAGE_HEAP_SIZE*sizeof(char) );
         memscpy(pbm_garb_heap_buf+garb_heap_offset, PBM_GARBAGE_HEAP_SIZE*sizeof(char) - garb_heap_offset ,buffer_record,
                 MIN(buf_offset,sizeof(pbm_efs_buffers[pbm_efs_buf_last].buffer)) );
         garb_heap_offset += MIN( PBM_GARBAGE_HEAP_SIZE*sizeof(char) - garb_heap_offset ,
                                  MIN(buf_offset,sizeof(pbm_efs_buffers[pbm_efs_buf_last].buffer)) );
         UIM_MSG_HIGH_2("garb_heap_offset %d  buf_offset %d",garb_heap_offset,buf_offset);
      }
      else
      {
         memscpy(pbm_garb_heap_buf+garb_heap_offset, PBM_GARBAGE_HEAP_SIZE*sizeof(char) - garb_heap_offset ,
                 buffer_record,MIN(buf_offset,sizeof(pbm_efs_buffers[pbm_efs_buf_last].buffer)));
         garb_heap_offset += MIN ( PBM_GARBAGE_HEAP_SIZE*sizeof(char) - garb_heap_offset  ,
                                   MIN(buf_offset,sizeof(pbm_efs_buffers[pbm_efs_buf_last].buffer)) )  ;
         UIM_MSG_HIGH_2("garb_heap_offset %d  buf_offset %d",garb_heap_offset,buf_offset);
      }
    }

    if ( garb_heap_offset != 0 )
    {
       result = mcfg_fwrite (new_fd,
                             (void *)pbm_garb_heap_buf,
                             garb_heap_offset,
                             MCFG_FS_TYPE_EFS);

       if((result == -1) || (result < (int32) garb_heap_offset))
       {
          UIM_MSG_ERR_3("Error writing to Master File %d fd %d garb_heap_offset %d",
                         result,new_fd,garb_heap_offset);
       }
       else
       {
          UIM_MSG_HIGH_2("EFS write successful %d garb_heap_offset %d",result,garb_heap_offset);
       }
    }

   PBM_MEM_FREEIF(pbm_garb_heap_buf);
    pbm_kick_dog();
    (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
    (void)mcfg_fclose (new_fd, MCFG_FS_TYPE_EFS);
    PBM_MEM_FREEIF(buffer);
    PBM_MEM_FREEIF(delete_list);
    if (num_deleted_records != delete_size)
    {
      UIM_MSG_ERR_2("Delete size %d records deleted %d", delete_size,
          num_deleted_records);
    }
  } else
  {
    ret_val = PBM_ERROR;
  }

  if ((ret_val != PBM_ERROR_NOT_FOUND) && (ret_val != PBM_EOF))
  {
    pbm_cleanup_corrupt_files(g_master_filename);
    ret_val = PBM_SUCCESS;
  } else
  {
    ret_val = PBM_SUCCESS;

    /* Remove Delete File*/
    pbm_kick_dog();

    status = mcfg_fs_delete((const char *)g_delete_filename,
                             MCFG_FS_TYPE_EFS,
                             MCFG_FS_SUBID_NONE);

    if( status != MCFG_FS_STATUS_OK )
    {
      return PBM_ERROR;
    }

    status = mcfg_fs_delete((const char *)g_master_filename,
                             MCFG_FS_TYPE_EFS,
                             MCFG_FS_SUBID_NONE);

    if( status != MCFG_FS_STATUS_OK )
    {
      return PBM_ERROR;
    }

    /* Copy new master file to the actual master file */
    if(mcfg_fs_rename((const char*)g_new_master_filename,
                      (const char*)g_master_filename,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE) !=0)
    {
      return PBM_ERROR;
    }
  }

  return ret_val;
}


/*===========================================================================
FUNCTION PBM_EFS_CACHE_STRUCTURE_INIT

DESCRIPTION
  Initializes a new cache to contain a given number of records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_cache_structure_init(PB_CACHE *cache,
                                             pbm_phonebook_type pb_id,
                                             int num_of_records,
                                             boolean circular)
{
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  rex_enter_crit_sect(&pbm_crit_sect);
  cache->pb_id          = pb_id;
  if(num_of_records > 0)
  {
    cache->status         = PBM_STATUS_NOT_INIT;
  }
  else
  {
    cache->status         = PBM_STATUS_NOT_AVAILABLE;
  }
  cache->circular          = circular;
  cache->text_length[0]    = 255;
  cache->record_length[0]  = 65535;
  cache->file_length       = 0;
  cache->num_of_records    = num_of_records;
  cache->records_in_use    = 0;
  cache->start             = 1;
  cache->pb_cache_array    = NULL;

  PBM_ALLOC_MEM_W_ERR_RET(cache->pb_cache_array,
                          (uint32) (cache->num_of_records+1) *
                                    sizeof(pbm_addr_cache_s_type *),
                          NO_ACTION);

  /* Zero-out cache to start with*/
  if (cache->pb_cache_array)
    memset(cache->pb_cache_array, 0,
        (uint32) (cache->num_of_records+1) * sizeof(pbm_addr_cache_s_type *) );
  rex_leave_crit_sect(&pbm_crit_sect);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_EFS_CACHE_INIT

DESCRIPTION
Loads all records from EFS.
The following caches are initialized from this file:
  Address Book cache.
  Speed Dial List
  Call History Caches.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_efs_cache_init(boolean powerup)
{
  pbm_addr_cache_s_type             *record              = NULL;
  int                                fd                  = -1;
  pbm_return_type                    ret_val             = PBM_SUCCESS;
  uint16                             location            = 0;
  static int                         file_descriptor     = -1;
  uint16                             max_unique_id       = 0;
  pbm_phonebook_type                 pb_id               = {PBM_GPB};
mcfg_fs_status_e_type                status              = MCFG_FS_STATUS_EFS_ERR;
  struct fs_stat                     sbuf;


  pb_id.device_type = PBM_EFS;

  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&efs_cache, pb_id,
                                                  PBM_MAX_EFS_RECS, FALSE))
    return PBM_ERROR;
  pb_id.device_type = PBM_SDL;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&sdl_cache, pb_id,
                                                  PBM_MAX_SPEED_DIALS, FALSE))
    return PBM_ERROR;
  pb_id.device_type = PBM_RCVD;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&incoming_calls_cache, pb_id,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;
  pb_id.device_type = PBM_DIALED;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&outgoing_calls_cache, pb_id,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;
  pb_id.device_type = PBM_MISS;
  if (PBM_SUCCESS != pbm_efs_cache_structure_init(&missed_calls_cache, pb_id,
                                                  PBM_MAX_CALL_HISTORY, TRUE))
    return PBM_ERROR;

  /* Open Master File*/
  status = mcfg_fs_stat((const char *)g_master_filename,
                        &sbuf,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  if(status == MCFG_FS_STATUS_OK)
  {
    fd = mcfg_fopen ((const char*)g_master_filename,
                     MCFG_FS_O_RDONLY,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);
    if (fd<0)
    {
      return PBM_ERROR;
    }
  }

  if (fd >= 0)
  {
    file_descriptor = fd;

    /* Load from EFS*/
    record = pbm_cache_record_new();
    PBM_CHECK_PTR_RET(record, PBM_ERROR);
    while (PBM_SUCCESS == (ret_val = pbm_efs_read(file_descriptor, record)))
    {
      PB_CACHE *cache_pointer;

      /*Act as if this is the last record, and set Unique ID generator*/
      if (record->unique_id > max_unique_id)
        max_unique_id = record->unique_id;
      else
      {
        UIM_MSG_ERR_2("Found unique id %d less than current max %d",
                     record->unique_id, max_unique_id);
        pbm_free_fields_internal(&record->fields, record->num_fields);
        continue;
      }

      cache_pointer = pbm_pb_id_to_cache(
                          pbm_session_record_id_to_phonebook_type(record->record_id));

      if (cache_pointer == NULL)
      {
        pbm_free_fields_internal(&record->fields, record->num_fields);
        UIM_MSG_ERR_1("Add to cache failed for record ID %d.",
            record->record_id);
        continue;
      }

      location = pbm_record_id_to_location(record->record_id);
      if ((location > 0) &&
          (location <= cache_pointer->num_of_records))
      {
        if (cache_pointer->pb_cache_array[location] &&
            cache_pointer->pb_cache_array[location]->num_fields)
        {
          UIM_MSG_ERR_1("Duplicate record ID 0x%x found in master file",
              record->record_id);
          /* Make sure the old record is deleted. */
          (void) pbm_efs_delete(cache_pointer->pb_cache_array[location], FALSE);
        }
      }
      ret_val = pbm_cache_add(cache_pointer, record, FALSE);
      if ( ret_val != PBM_SUCCESS )
      {
        pbm_free_fields_internal(&record->fields, record->num_fields);
        UIM_MSG_ERR_1("Add to cache failed with error %d.", ret_val);
        continue;
      }
      /* Assuming latest entry will be later in file. */
      if (cache_pointer->circular) {
        cache_pointer->start = pbm_record_id_to_location(record->record_id);
      }

      /* Allocate new record */
      record = pbm_cache_record_new();
      if (!record)
      {
        (void)mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);
        return PBM_ERROR_MEM_FULL;
      }
    }
    pbm_efs_set_unique_id (++max_unique_id);

    /* Free our memory, close the files. */
    PBM_MEM_FREEIF(record);
    (void)mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);

    /* Empty Master File -- normal exit condition*/
    if (ret_val != PBM_EOF)
    {
      UIM_MSG_ERR_0("Error initializing PBM cache.");
      return ret_val;
    }
  }

  PBM_CIRC_CACHE_ADDSTART(&incoming_calls_cache);
  PBM_CIRC_CACHE_ADDSTART(&outgoing_calls_cache);
  PBM_CIRC_CACHE_ADDSTART(&missed_calls_cache);

  /* on powerup, we want to wait until after phone UID is intialized
   * to say that the caches are ready.  Otherwise, they're ready now. */
  if (!powerup)
  {
    if(efs_cache.num_of_records > 0)
      PBM_SET_PB_CACHE_READY(&efs_cache);
    if(sdl_cache.num_of_records > 0)
      PBM_SET_PB_CACHE_READY(&sdl_cache);
    if(incoming_calls_cache.num_of_records > 0)
      PBM_SET_PB_CACHE_READY(&incoming_calls_cache);
    if(outgoing_calls_cache.num_of_records > 0)
      PBM_SET_PB_CACHE_READY(&outgoing_calls_cache);
    if(missed_calls_cache.num_of_records > 0)
      PBM_SET_PB_CACHE_READY(&missed_calls_cache);
  }

  {
    int verify_index;
    int cache_index;
    PB_CACHE *circ_cache[] = {&incoming_calls_cache,
                              &outgoing_calls_cache,
                              &missed_calls_cache};

    /* Check each of the circular caches to make sure the entries are
     * stored contiguously.  If the EFS file is corrupted, this may not
     * have been detected yet.  If we find an issue, clear out that
     * cache, but allow the phone to still use that call history, just
     * forget about the bad (old) contents.
     */
    rex_enter_crit_sect(&pbm_crit_sect);
    for (cache_index = 0; cache_index < (int) ARR_SIZE(circ_cache); cache_index++)
    {
      for (verify_index = 1; verify_index <= (circ_cache[cache_index])->records_in_use; verify_index++)
      {
        if (!circ_cache[cache_index]->pb_cache_array[
            PBM_CIRC_CACHE_NTHLOC(circ_cache[cache_index], verify_index)])
        {
          UIM_MSG_ERR_3("Built cache %d wrong entry %d of %d is NULL. Clearing cache.",
              cache_index, verify_index, circ_cache[cache_index]->records_in_use);

          /* Delete all entries in the file. */
          for (verify_index = 1;
               verify_index <= circ_cache[cache_index]->num_of_records;
               verify_index++)
          {
            if (circ_cache[cache_index]->pb_cache_array[verify_index] &&
                circ_cache[cache_index]->pb_cache_array[verify_index]->num_fields)
              (void) pbm_efs_delete(circ_cache[cache_index]->pb_cache_array[verify_index], FALSE);
          }

          /* Protect the sanctity of the cache. */

          pbm_clear_cache(circ_cache[cache_index]);
          if (PBM_SUCCESS == pbm_efs_cache_structure_init(circ_cache[cache_index],
                                                          circ_cache[cache_index]->pb_id,
                                                          PBM_MAX_CALL_HISTORY, TRUE))
          {
            PBM_SET_PB_CACHE_READY(circ_cache[cache_index]);
          }
          break;
        }
      }
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);

  return PBM_SUCCESS;
}



/*===========================================================================
FUNCTION PBM_EFS_GARBAGE_RECOLLECT

DESCRIPTION
  Does garbage collection at any point after init stage. Actual
  garbage collection occurs only when the number of records deleted after
  the last reboot exceeds a certain value.

  If reset is TRUE, the cache is cleared and unique id is regenerated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_efs_garbage_recollect()
{
  int                    i           = 0;
  pbm_return_type        retval      = PBM_ERROR;
  dword                  delete_size = 0;
  mcfg_fs_status_e_type  status      = MCFG_FS_STATUS_EFS_ERR;
  struct fs_stat         sbuf;
  PB_CACHE              *pb_caches[] = { &efs_cache,
                                         &sdl_cache,
                                         &incoming_calls_cache,
                                         &outgoing_calls_cache,
                                         &missed_calls_cache
                                       };

  /* Does the delete file exist? */
  pbm_kick_dog();

  status = mcfg_fs_stat((const char *)g_delete_filename,
                        &sbuf,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

  if(status != MCFG_FS_STATUS_OK)
  {
    UIM_MSG_ERR_1("Error while checking if Delete File exists %d", efs_errno);
    return;
  }

  delete_size = sbuf.st_size/sizeof(uint16);

  if (delete_size < PBM_MAX_EFS_DELETES)
    return;     /* Not enough garbage, don't clean it up. */

  rex_enter_crit_sect(&pbm_crit_sect);
  for (i = 0; i < (int) ARR_SIZE(pb_caches); ++i)
  {
      pbm_clear_cache(pb_caches[i]);
  }
  rex_leave_crit_sect(&pbm_crit_sect);
  (void) pbm_efs_garbage_collect(TRUE);

  retval = pbm_efs_cache_init(FALSE);
  if (retval != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("Cache Init returned a failure %d", retval);
  }
  return;
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_EFS

DESCRIPTION
  Adds a record to EFS. This is an internal function and should be called only from
  within PBM task.

DEPENDENCIES
  The caller should free write cmd structure in case of an error.
  If successful, write cmd is freed by this function (synchronous operation) or
  within the write callbacks (asynchronous operation).

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_efs(pbm_write_record_s_type *write)
{
  pbm_return_type ret_val;
  boolean async = FALSE;
  boolean deletion_only;

  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  if (write->proc_func)
  {
    async = TRUE;
  }
  deletion_only = (write->curr_rec && write->curr_rec->num_fields) ?
                  FALSE : TRUE;

  if (async) {

    if (!pbm_efs_buf_empty && (pbm_efs_buf_first == pbm_efs_buf_last))
    {
      UIM_MSG_ERR_3("Could not write to efs buffers.%d %d %d", pbm_efs_buf_empty,
                  pbm_efs_buf_first, pbm_efs_buf_last);
      pbm_list_recover(write, FALSE);
      return PBM_ERROR_MEM_FULL;
    }

    pbm_efs_buffers[pbm_efs_buf_last].write = write;
    pbm_efs_buffers[pbm_efs_buf_last].deleted = FALSE;
  }

  if (write->prev_rec && write->prev_rec->num_fields)
  {
    /* Delete record from file.
     * An async delete is initiated only if there no addition within this
     * operation. */
    ret_val = pbm_efs_delete(write->prev_rec,
                             (boolean) (async && deletion_only));
    /* if async && deletion_only is TRUE, then write gets freed in the
     * callback pbm_efs_close_cb. Make sure it is no longer used in
     * that case */

    if (ret_val != PBM_SUCCESS)
    {
      pbm_list_recover(write, FALSE);
      UIM_MSG_ERR_1("PBM EFS delete failed with error %d.", ret_val);
      return ret_val;
    }
    pbm_efs_buffers[pbm_efs_buf_last].deleted = TRUE;
  }

  if (!deletion_only)
  {
    /* Add record to file */
    ret_val = pbm_efs_add(-1, write->curr_rec, async);
    if (ret_val != PBM_SUCCESS)
    {
      UIM_MSG_ERR_1("EFS add failed with error %d.", ret_val);
      pbm_list_recover(write, TRUE);
      return ret_val;
    }
  }

  /* We are done with write command */
  if (!async)
  {
    pbm_list_update(write);
    pbm_write_notify(write, PBM_SUCCESS);
    pbm_write_cmd_free(write);
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_INIT_PHONE_UNIQUE_ID

DESCRIPTION
  Initializes the phone unique ID.  If the phone unique ID file does not exist, but the master
  phonebook file does exists, then an error flag is raised indicating the UIDs may have changed.
  Note that this situation could also occur if the phonebook contains only non-EFS entries
  and the UID file never initialized.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

static pbm_return_type pbm_init_phone_unique_id(void)
{
  pbm_return_type        nRet                 = PBM_SUCCESS;
  int                    file_descriptor      = -1;
  int                    fd                   = -1;
  int                    i                    = 0;
  int                    read_count           = 0;
  uint64                 file_ph_uid          = 0;
  uint64                 rec_ph_uid           = 0;
  boolean                file_ph_uid_is_reset = FALSE;
  boolean                doRenumber           = FALSE;
  mcfg_fs_status_e_type  status = MCFG_FS_STATUS_EFS_ERR;
  struct fs_stat         sbuf;

  do
  {
    status = mcfg_fs_stat((const char *)g_ph_uid_filename,
                          &sbuf,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

    if(status == MCFG_FS_STATUS_OK)
    {
      fd = mcfg_fopen ((const char*)g_ph_uid_filename,
                        MCFG_FS_O_RDONLY,
                        MCFG_FS_ALLPERMS,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

      if (fd<0)
      {
        /* Error opening PH UID file, initialize failed. */
        nRet = PBM_ERROR;
        break;
      }
    }
    else if (efs_errno == ENOENT)
    {
      doRenumber = TRUE;
      break; /* Done with initialization. */
    }
    file_descriptor = fd;

    read_count = mcfg_fread (file_descriptor,
                             (void *)&file_ph_uid,
                             (uint32)sizeof(file_ph_uid),
                             MCFG_FS_TYPE_EFS);

    if ((read_count == -1) || (read_count != (sizeof(file_ph_uid))))
    {
      UIM_MSG_ERR_0("Error reading PH UID file");
      (doRenumber) = (TRUE);
      break;
    }

    read_count = mcfg_fread (file_descriptor,
                             (void *)&file_ph_uid_is_reset,
                             (uint32)sizeof(file_ph_uid_is_reset),
                             MCFG_FS_TYPE_EFS);

    if ((read_count == -1) || (read_count != (sizeof(file_ph_uid_is_reset))))
    {
      UIM_MSG_ERR_0("Error reading PH UID file");
      (doRenumber) = (TRUE);
      break;
    }

     /* Check if any EFS records have PH UID fields greater than the PH UID on file.
     * If so, then renumber the phonebook. */
    for (i = 0; i < efs_cache.num_of_records; i++)
    {
      /* Skip empty records. */
      if ( (efs_cache.pb_cache_array[i] == NULL) ||
         (efs_cache.pb_cache_array[i]->fields == NULL) ||
         (efs_cache.pb_cache_array[i]->num_fields == 0) )
      {
         continue;
      }

      /* If any record doesn't have a unique ID, renumber the phonebook */
      if (pbm_get_phone_unique_id_from_record(efs_cache.pb_cache_array[i],
        &rec_ph_uid) != PBM_SUCCESS)
      {
        doRenumber = TRUE;
        break;
      }

      /* if any record has a UID bigger than the counter file, renumber the phonebook */
      if (rec_ph_uid > file_ph_uid)
      {
        doRenumber = TRUE;
        break;
      }
    }

    if (doRenumber)
    {
      break;
    }

    g_ph_unique_id = file_ph_uid;
    g_ph_uid_is_reset = file_ph_uid_is_reset;
  }
  while (FALSE); /*lint !e717 */

  if (file_descriptor >= 0)
  {
    (void)mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);
    file_descriptor = -1;
  }

  if (doRenumber == TRUE)
  {
    nRet = pbm_renumber_with_unique_id(); /* This will re-create PH UID file and set global flags. */
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_EFS

DESCRIPTION
  Gets the next phone unique ID.  Increments the counter in EFS.

DEPENDENCIES
  Must be called from within pbm_crit_sect critical section.

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_get_next_phone_unique_id(uint64 *ph_unique_id)
{
  pbm_return_type          nRet            = PBM_SUCCESS;
  int                      file_descriptor = -1;
  int                      fd              = -1;
  int                      write_count     = 0;
  uint64                   new_ph_uid      = 0;

  PBM_CHECK_PTR_RET(ph_unique_id, PBM_ERROR);

  do
  {
    fd = mcfg_fopen ((const char*)g_ph_uid_filename,
                      MCFG_FS_O_RDWR,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE);

    if(fd < 0)
    {
      (nRet) = (PBM_ERROR);
      break;
    }

    file_descriptor = fd;

    new_ph_uid = g_ph_unique_id + 1;

    write_count = mcfg_fwrite (file_descriptor,
                               (void *)&new_ph_uid,
                               (uint32) sizeof(new_ph_uid),
                               MCFG_FS_TYPE_EFS);

    if ((write_count == -1) || (write_count != (sizeof(new_ph_uid))))
    {
      (nRet) = (PBM_ERROR);
      break;
    }

    *ph_unique_id = g_ph_unique_id = new_ph_uid;
  }
  while (FALSE); /*lint !e717 */

  if (file_descriptor >= 0)
  {
    (void)mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);
    file_descriptor = -1;
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_CREATE_INITIAL_PH_UID_FILE

DESCRIPTION
  Helper function to create an initial PH UID file. Initialized with the uint64 value 0.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO

===========================================================================*/

static pbm_return_type pbm_create_initial_ph_uid_file(uint64 starting_value, boolean uid_is_reset)
{
  pbm_return_type          nRet             = PBM_SUCCESS;
  int                      file_descriptor  = -1;
  int                      fd               = -1;
  int                      write_status     = 0;
  mcfg_fs_status_e_type    status           = MCFG_FS_STATUS_EFS_ERR;
  struct fs_stat           sbuf;

  status = mcfg_fs_stat((const char *)g_ph_uid_filename,
                         &sbuf,
                         MCFG_FS_TYPE_EFS,
                         MCFG_FS_SUBID_NONE);

  if(status == MCFG_FS_STATUS_OK)
  {
    /* Remove the file. */
    status = mcfg_fs_delete((const char *)g_ph_uid_filename,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_NONE);

    if(status != MCFG_FS_STATUS_OK)
    {
      return PBM_ERROR;
    }
  }
  else if (efs_errno != ENOENT)
  {
      /* Test failed due to reason other than "File Does not Exists"*/
    UIM_MSG_ERR_1 ("stat for ph_uid_file failed %d", status);
    return PBM_ERROR;
  }

  /* Create the file. */
  fd = mcfg_fopen ((const char*)g_ph_uid_filename,
                    MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    MCFG_FS_SUBID_NONE);

  if(fd <0)
  {
    return PBM_ERROR;
  }

  file_descriptor = fd;

  write_status = mcfg_fwrite (file_descriptor,
                              (void *) &starting_value,
                              (uint32)sizeof(starting_value),
                              MCFG_FS_TYPE_EFS);

  if ((write_status == -1) || (write_status != (sizeof(starting_value))))
  {
    UIM_MSG_ERR_0("Error writing PH UID file");
    (nRet) = (PBM_ERROR);
  }

  write_status = mcfg_fwrite (file_descriptor,
                             (void *)&uid_is_reset,
                             (uint32)sizeof(uid_is_reset),
                              MCFG_FS_TYPE_EFS);

  if ((write_status == -1) || (write_status != (sizeof(uid_is_reset))))
  {
    UIM_MSG_ERR_0("Error writing PH UID file");
    (nRet) = (PBM_ERROR);
  }

  (void)mcfg_fclose (file_descriptor, MCFG_FS_TYPE_EFS);
  file_descriptor = -1;

  return nRet;
}

/*===========================================================================
FUNCTION PBM_GET_PHONE_UNIQUE_ID_FROM_RECORD

DESCRIPTION
  Given a cache record, retrieves the PH UID value and places it in the
  ph_unique_id parameter.

  Returns PBM_ERROR if not found or the record has an invalid entry.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/

pbm_return_type pbm_get_phone_unique_id_from_record(const pbm_addr_cache_s_type *record,
   uint64 *ph_unique_id)
{
  int index;
  void *field_ptr;

  PBM_CHECK_PTR2_RET(record, ph_unique_id, PBM_ERROR);

  if (pbm_find_phone_uid_field(record->fields, record->num_fields, &index) == FALSE)
  {
    return PBM_ERROR;
  }

  if (record->fields[index].data_len != PBM_DDWORD_SIZE)
  {
    UIM_MSG_ERR_1("Found invalid PH UID field size at record %d", record->record_id);
    return PBM_ERROR;
  }

  field_ptr = pbm_get_field_data_ptr(record->fields,index);
  PBM_CHECK_PTR_RET(field_ptr, PBM_ERROR);

  memscpy((void *)ph_unique_id, PBM_DDWORD_SIZE, field_ptr, PBM_DDWORD_SIZE);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_RENUMBER_WITH_UNIQUE_ID

DESCRIPTION
   Renumbers all EFS phonebook records with new PH_UID fields. Creates a new
   PH_UID file with the 'is_reset' flag set to TRUE.

DEPENDENCIES

SIDE EFFECTS
   If successful, g_ph_unique_id is set with the last new unique ID after renumbering.
   g_ph_uid_is_reset flag is set to TRUE.  The PH UID file is re-created with the last
   new unique ID.

COMMENTS

SEE ALSO
===========================================================================*/

static pbm_return_type pbm_renumber_with_unique_id(void)
{
  pbm_return_type            nRet = PBM_SUCCESS;
  int                        i            = 0;
  int                        index        = 0;
  int                        fd           = -1;
  int                        new_fd       = -1;
  int                        old_fd       = -1;
  pbm_addr_cache_s_type     *record       = NULL;
  uint16                     location     = 0;
  uint64                     ph_unique_id = 0;
  pbm_phonebook_type         pb_id        = {PBM_GPB};
  struct fs_stat             sbuf;
  pbm_addr_cache_s_type      old_rec;

  /* If there are no records in PBM_EFS then just write out the PH UID file. */

  if (!efs_cache.records_in_use)
  {
    nRet = pbm_create_initial_ph_uid_file(ph_unique_id, TRUE);
    g_ph_unique_id = 0;
    g_ph_uid_is_reset = TRUE;
    return nRet;
  }

  /* Renumber the EFS cache first. */

  for (i = 0; i < efs_cache.num_of_records; i++)
  {
    /* Skip empty records. */
    if ( (efs_cache.pb_cache_array[i] == NULL) ||
      (efs_cache.pb_cache_array[i]->fields == NULL) ||
      (efs_cache.pb_cache_array[i]->num_fields == 0) )
    {
      continue;
    }

    record = efs_cache.pb_cache_array[i];

    if (pbm_find_phone_uid_field(record->fields,
        record->num_fields, &index) == FALSE)
    {
      /* The given record does not have the PH UID field. Tag it. */

      ph_unique_id++;

      nRet = pbm_append_phone_uid_field(ph_unique_id,
         &record->fields, &record->num_fields);
      if (nRet != PBM_SUCCESS)
      {
         break;
      }
    }
    else
    {
      if ( (record->fields[index].data_len != PBM_DDWORD_SIZE) )
      {
        /* We have a record with a PH_UID field that has an invalid
         * length. For now, just set that field type to PBM_FIELD_NONE,
         * and add a good UID field. This shouldn't happen, unless the fields
         * get renumbered. */

        UIM_MSG_ERR_1("Found invalid PH UID field size at record %d",
          record->record_id);

        record->fields[index].field_id = PBM_FIELD_NONE;

        ph_unique_id++;

        nRet = pbm_append_phone_uid_field(ph_unique_id,
          &record->fields, &record->num_fields);

        if (nRet != PBM_SUCCESS)

        {
          break;
        }
      }
      else  /* size is correct */
      {
        void *data_ptr;

        ph_unique_id++;

        data_ptr = pbm_get_field_data_ptr(record->fields,index);

        PBM_CHECK_PTR_BREAK(data_ptr, nRet, PBM_ERROR);
        memscpy(data_ptr,sizeof(ph_unique_id),(void *)&ph_unique_id,sizeof(ph_unique_id));
      }
    }
  }

  /* Bail out if we encountered an error during cache renumbering. */
  if (nRet != PBM_SUCCESS)
  {
    return nRet;
  }

  /* Write the EFS cache out to the filesystem. */
  do
  {
    mcfg_fs_status_e_type    status         = MCFG_FS_STATUS_EFS_ERR;
    /* Check if "new" master file exists, remove it if so. */
    pbm_kick_dog();

    status = mcfg_fs_stat((const char *)g_new_master_filename,
                          &sbuf,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

    if(status == MCFG_FS_STATUS_OK)
    {
      /* Should not occur since garbage collection already complete during initialization. */
      status = mcfg_fs_delete((const char *)g_new_master_filename,
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE);

      if(status != MCFG_FS_STATUS_OK)
      {
        UIM_MSG_ERR_1("Error deleting New Master File %d", status);
        (nRet) = (PBM_ERROR);
        break;
      }
    }
    else if (efs_errno != ENOENT)
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      UIM_MSG_ERR_1 ("efs_stat failed.%d", status);
      return PBM_ERROR;
    }

    /* Open "old" Master File*/
    fd = mcfg_fopen ((const char*)g_master_filename,
                      MCFG_FS_O_RDONLY,
                      MCFG_FS_ALLPERMS,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE);

    if(fd < 0)
    {
      (nRet) = (PBM_ERROR);
      break;
    }

    old_fd = fd;

    /* Create "new" Master File*/
    fd = mcfg_fopen ((const char*)g_new_master_filename,
                     MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_EXCL,
                     MCFG_FS_ALLPERMS,
                     MCFG_FS_TYPE_EFS,
                     MCFG_FS_SUBID_NONE);

    if(fd < 0)
    {
      (nRet) = (PBM_ERROR);
      break;
    }

    new_fd = fd;

    while (PBM_SUCCESS == (nRet = pbm_efs_read(old_fd, &old_rec)))
    {
      /* Simply copy over any records that do not fall into the PBM_EFS phonebook. */
      pb_id = pbm_session_record_id_to_phonebook_type(old_rec.record_id);
      if (pb_id.device_type != PBM_EFS)
      {
        nRet = pbm_efs_add(new_fd, &old_rec, FALSE);
        if (nRet != PBM_SUCCESS)
        {
          pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
          break;
        }
      }
      else
      {
        /* Use the updated efs_cache version of any records that are in PBM_EFS. */

        location = pbm_record_id_to_location(old_rec.record_id);
        if ((location > 0) &&
            (location <= efs_cache.num_of_records))
        {
          record = efs_cache.pb_cache_array[location];

          nRet = pbm_efs_add(new_fd, record, FALSE);
          if (nRet != PBM_SUCCESS)
          {
            UIM_MSG_ERR_1("Error 0x%x Writing to new file.", nRet);
            pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
            break;
          }
        }
      }

      pbm_free_fields_internal(&old_rec.fields, old_rec.num_fields);
    }

    if (nRet != PBM_EOF)
    {
      break;
    }

    (void)mcfg_fclose (new_fd, MCFG_FS_TYPE_EFS);
    new_fd = -1;

    (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
    old_fd = -1;

    /* Copy new master file to the actual master file */
    status = mcfg_fs_delete((const char *)g_master_filename,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_NONE);

    if(status != MCFG_FS_STATUS_OK)
    {
      UIM_MSG_ERR_1("Error deleting Master File %d", status);
      (nRet) = (PBM_ERROR);
      break;
    }

    if(mcfg_fs_rename((const char*)g_new_master_filename,
                      (const char*) g_master_filename,
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE) != 0)
    {
      UIM_MSG_ERR_0("Error renaming new master file to Master File");
      (nRet) = (PBM_ERROR);
      break;
    }

    /* Create the PH UID file with is_reset flag set. */
    nRet = pbm_create_initial_ph_uid_file(ph_unique_id, TRUE);
    if (nRet != PBM_SUCCESS)
    {
      break;
    }

    /* Initialization successful. Set the memory values. */
    g_ph_unique_id = ph_unique_id;
    g_ph_uid_is_reset = TRUE;
  }
  while (FALSE);

  if (new_fd >= 0)
  {
    (void)mcfg_fclose (new_fd, MCFG_FS_TYPE_EFS);
    new_fd = -1;
  }

  if (old_fd >= 0)
  {
    (void)mcfg_fclose (old_fd, MCFG_FS_TYPE_EFS);
    old_fd = -1;
  }

  return nRet;
}

