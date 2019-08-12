/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   F I L E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock file opertaion functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_file.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
07/08/15   stv     Perform efs sync before calling client call back
07/07/15   stv     Fix to perform efs sync operation for critical file updates
05/15/15   vv      Support for RSU in simlock
12/17/14   vv      Relocate the simlock data files
09/29/14   vv      Added support for the remote SFS
09/09/14   vv      Perform efs_sync() after writing the data
01/07/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_file.h"
#include "simlock_sfs.h"
#include "simlock_efs.h"
#include "simlock_util.h"
#include "simlock_sfs_remote.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Indicates whether a file system write happened
   which requires an efs sync operation */
static boolean simlock_fs_updated = FALSE;


/*===========================================================================
FUNCTION SIMLOCK_FILE_UPDATE_WRITE_STATUS

DESCRIPTION
  This function updates a global variable indicating that a file system
  write has happened

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_file_update_write_status(
  void
)
{
  /* set the variable, indicating a write has occurred
     which requires an efs sync operation */
  simlock_fs_updated = TRUE;
} /* simlock_file_update_write_status */


/*===========================================================================
FUNCTION SIMLOCK_FILE_REQUIRES_EFS_SYNC

DESCRIPTION
  This function returns TRUE if the file enum update requires an
  efs_sync and FALSE if otherwise.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean simlock_file_requires_efs_sync(
  simlock_data_file_enum_type file_enum
)
{
  switch(file_enum)
  {
    /* List of files which doesn't require explicit efs_sync */
    case SIMLOCK_FILE_TIME_INFO:
      return FALSE;

    default:
      break;
  }

  return TRUE;
} /* simlock_file_requires_efs_sync */


/*===========================================================================
FUNCTION SIMLOCK_FILE_READ

DESCRIPTION
  This function reads the data from the requested file by calling the SFS or
  EFS based read functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_read(
  uint8                       ** data_buf_pptr,
  uint32                       * data_buf_len_ptr,
  simlock_data_file_enum_type    file_enum,
  simlock_sfs_enum_type          sfs_type
)
{
  simlock_result_enum_type   simlock_status = SIMLOCK_SUCCESS;
  simlock_filename_type      file_name;

  if((data_buf_pptr == NULL) ||
     (data_buf_len_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(sfs_type == SIMLOCK_SFS_REMOTE)
  {
    simlock_data_type                  read_data       = {0, NULL};
    simlock_file_type                  read_file;

    memset(&read_file, 0x00, sizeof(simlock_file_type));

    simlock_status = simlock_util_file_enum_to_file_type(file_enum, &read_file);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      simlock_status = simlock_sfs_remote_read_file(read_file, &read_data);
      if((simlock_status != SIMLOCK_SUCCESS) ||
         (read_data.data_len == 0))
      {
        SIMLOCK_MSG_ERR_1("config file read failed, status: 0x%x", simlock_status);
        SIMLOCK_MEM_FREE(read_data.data_ptr);
        return SIMLOCK_GENERIC_ERROR;
      }

      *data_buf_len_ptr = read_data.data_len;
      *data_buf_pptr    = read_data.data_ptr;
    }
    return simlock_status;
  }

  /* read from local SFS */
  simlock_status = simlock_util_get_file_path_from_enum(file_enum, file_name);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* get the file size */
  simlock_status = simlock_file_check(file_name, data_buf_len_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_2("file check failed, status: 0x%x, len: 0x%x",
                       simlock_status, *data_buf_len_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* allocate the memory */
  SIMLOCK_CHECK_AND_MEM_MALLOC(*data_buf_pptr,
                               *data_buf_len_ptr);
  if(*data_buf_pptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  simlock_status = simlock_sfs_read_file(*data_buf_pptr, *data_buf_len_ptr, file_name);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

   return simlock_status;
} /* simlock_file_read */


/*===========================================================================
FUNCTION SIMLOCK_FILE_WRITE

DESCRIPTION
  This function writes the data to the requested file by calling the SFS or
  EFS based write functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_write(
  uint8                         * data_buf_ptr,
  uint32                          data_buf_len,
  simlock_data_file_enum_type     file_enum,
  simlock_sfs_enum_type           sfs_type
)
{
  simlock_result_enum_type   simlock_status = SIMLOCK_SUCCESS;
  simlock_filename_type      file_name;

  if((data_buf_ptr == NULL) ||
     (data_buf_len == 0))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(sfs_type == SIMLOCK_SFS_REMOTE)
  {
    simlock_data_type                  write_data      = {0, NULL};
    simlock_file_type                  write_file;

    memset(&write_file, 0x00, sizeof(simlock_file_type));

    simlock_status = simlock_util_file_enum_to_file_type(file_enum, &write_file);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      write_data.data_len = data_buf_len;
      write_data.data_ptr = data_buf_ptr;

      simlock_status = simlock_sfs_remote_write_file(write_file, &write_data);
    }
    return simlock_status;
  }

  /* write to local SFS */
  simlock_status = simlock_util_get_file_path_from_enum(file_enum, file_name);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  simlock_status = simlock_sfs_write_file(data_buf_ptr, data_buf_len, file_name);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  if(simlock_status == SIMLOCK_SUCCESS &&
     simlock_file_requires_efs_sync(file_enum))
  {
    /* Update the variable indicating that a write
       has happened which needs efs sync */
    simlock_file_update_write_status();
  }

  return simlock_status;
} /* simlock_file_write */


/*===========================================================================
FUNCTION SIMLOCK_FILE_CHECK

DESCRIPTION
  This function checks if the requested file exists by calling the SFS or
  EFS based get file size functions

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_check(
  const char  * sfs_file_ptr,
  uint32      * sfs_file_len_ptr
)
{
  if((sfs_file_ptr == NULL) ||
     (sfs_file_len_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  return simlock_sfs_get_file_size(sfs_file_ptr, sfs_file_len_ptr);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_file_check */


/*===========================================================================
FUNCTION SIMLOCK_FILE_MKDIR

DESCRIPTION
  This function creates a directory in the file system

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_mkdir(
  const char * sfs_file_ptr
)
{
  if(sfs_file_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  return simlock_sfs_mkdir(sfs_file_ptr);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* simlock_file_mkdir */


/*===========================================================================
FUNCTION SIMLOCK_FILE_COMMIT_TO_FS_IF_NEEDED

DESCRIPTION
  This function checks the status of the variable, which indicates whether
  a write to the file system has happened which requires an efs sync.
  It performs efs sync operation if required.
  It also resets the variable value.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_file_commit_to_fs_if_needed(
  void
)
{
  if(simlock_fs_updated)
  {
    /* There is a write happened to at least one file
       which requires an efs sync operation */
    (void)simlock_efs_sync();
  }

  simlock_fs_updated = FALSE;
} /* simlock_file_commit_to_fs_if_needed */


/*===========================================================================
FUNCTION SIMLOCK_FILE_MKDIR_EFS

DESCRIPTION
  This function creates a directory in the EFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_mkdir_efs(
  const char * efs_file_ptr
)
{
  if(efs_file_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  return simlock_efs_mkdir(efs_file_ptr);
} /* simlock_file_mkdir_efs */


/*===========================================================================
FUNCTION SIMLOCK_FILE_SECURE

DESCRIPTION
  This function copies the file data from a non-secured location (e.g. EFS)
  to a secured location (e.g. SFS). It then deletes the file from the
  non-secured location

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_file_secure(
  const char                   * src_file_name_ptr,
  uint32                         src_file_len_expected,
  simlock_data_file_enum_type    dest_file_enum
)
{
  simlock_result_enum_type    simlock_status       = SIMLOCK_SUCCESS;
  uint32                      src_file_len_actual  = 0;
  uint8                     * src_file_data_ptr    = NULL;

  if((src_file_name_ptr == NULL) ||
     (src_file_len_expected == 0))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* get the file size */
  simlock_status = simlock_efs_get_file_size(src_file_name_ptr, &src_file_len_actual);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  if(src_file_len_expected != src_file_len_actual)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* allocate the memory */
  SIMLOCK_CHECK_AND_MEM_MALLOC(src_file_data_ptr,
                               src_file_len_actual);
  if(src_file_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  simlock_status = simlock_efs_read_file(src_file_name_ptr,
                                         src_file_len_actual,
                                         src_file_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MEM_FREE(src_file_data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* write the file to SFS location */
  simlock_status = simlock_file_write(src_file_data_ptr,
                                      src_file_len_actual,
                                      dest_file_enum,
                                      SIMLOCK_SFS_LOCAL);

  SIMLOCK_MEM_FREE(src_file_data_ptr);

  /* delete the file after it was copied to SFS
     The file is deleted even if copying to SFS fails */
  (void)simlock_efs_delete_file(src_file_name_ptr);

  return simlock_status;
} /* simlock_file_secure */
#endif /* FEATURE_SIMLOCK */

