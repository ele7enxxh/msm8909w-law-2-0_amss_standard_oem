/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   E F S   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock EFS functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_efs.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/15/15   vv      Support for RSU in simlock
12/17/14   vv      Relocate the simlock data files
12/11/14   vv      Perform efs_sync() before writing the data
11/05/14   av      Change dir permissions from 0x777 to 777 for mkdir
09/09/14   vv      Perform efs_sync() after writing the data
06/20/14   vv      Added support for diag protection
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_efs.h"
#include "string.h"
#include "fs_errno.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Indicates whether EFS sync has happened */
static boolean simlock_efs_sync_done = FALSE;


/*===========================================================================
FUNCTION SIMLOCK_EFS_GET_FILE_SIZE

DESCRIPTION
  This function retrieves the size of a file in EFS.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_get_file_size
(
  const char *   file_handle_ptr,
  uint32     *   file_size_ptr
)
{
  struct fs_stat         file_stat = {0};

  /* Perform basic param checking. */
  if ( file_handle_ptr == NULL )
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  if ( file_size_ptr == NULL )
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* --------------------------------------------------------------------------
     Call the FS Function to ge the File Size
     Only return a size if FS_OKAY_S is returned. Otherwise, set the length
     to 0.
     ------------------------------------------------------------------------*/
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    *file_size_ptr = file_stat.st_size;
    return SIMLOCK_SUCCESS;
  }
  else
  {
    *file_size_ptr = 0;
    return SIMLOCK_GENERIC_ERROR;
  }
} /* simlock_efs_get_file_size */


/*===========================================================================
FUNCTION SIMLOCK_EFS_READ_FILE

DESCRIPTION
  This function reads the data from a file in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_read_file(
  const char *   file_handle_ptr,
  uint32         data_len,
  uint8      *   data_buffer_ptr
)
{
  int  file_descriptor = 0;

  if (file_handle_ptr  == NULL ||
      data_buffer_ptr  == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Open the File in EFS for Read Only */
  file_descriptor = efs_open(file_handle_ptr, O_RDONLY);

  if(file_descriptor < 0)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Read the file into buffer provided */
  if (efs_read(file_descriptor, data_buffer_ptr, data_len) != data_len)
  {
    (void)efs_close(file_descriptor);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Release the file handle */
  (void)efs_close(file_descriptor);

  return SIMLOCK_SUCCESS;
}  /* simlock_efs_read_file */


/*===========================================================================
FUNCTION SIMLOCK_EFS_MKDIR

DESCRIPTION
  This function creates a directory in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_mkdir
(
  const char* file_handle_ptr
)
{
  /* Perform basic param checking */
  if ( file_handle_ptr == NULL )
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* To create the directory...use the efs_mkdir() function. */
  if(efs_mkdir(file_handle_ptr, 0777) < 0)
  {
    return SIMLOCK_GENERIC_ERROR;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_efs_mkdir */


/*===========================================================================
FUNCTION SIMLOCK_EFS_WRITE_FILE

DESCRIPTION
  This function writes the data to a file in EFS.

DEPENDENCIES
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_efs_write_file
(
  const char *   file_handle_ptr,
  uint32         num_bytes,
  uint8 *        data_buffer_ptr
)
{
  simlock_result_enum_type simlock_status  = SIMLOCK_SUCCESS;
  int                      file_descriptor = 0;
  struct fs_stat           file_stat       = {0};

  /* Open the File in EFS for Create as a New File */
  if ( file_handle_ptr == NULL )
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Remove the file ... Number of Retries*/
  if(efs_stat(file_handle_ptr, &file_stat) == 0)
  {
    if(efs_unlink(file_handle_ptr) < 0)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }
  else if (efs_errno != ENOENT)
  {
    /* Test failed due to reason other than "File Does not Exists".
       efs_errno is defined in fs_public.h and provides the
       reason for EFS API failure */
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Open the File in EFS for Create as a New File */
  file_descriptor = efs_open(file_handle_ptr, O_CREAT | O_TRUNC | O_WRONLY);

  if(file_descriptor < 0)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Write the Data to the File */
  if(efs_write(file_descriptor, data_buffer_ptr, (fs_size_t)num_bytes)
               != (fs_ssize_t)num_bytes)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  /* Data has been successfully written to FS.  Ok to close up the file */
  (void)efs_close(file_descriptor);

  return simlock_status;
} /* simlock_efs_write_file */


/*===========================================================================
FUNCTION SIMLOCK_EFS_NAME_TEST

DESCRIPTION
  This function tests whether a directory or a file
  already exists within the specified file structure.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_name_test
(
  const char * file_name_ptr
)
{
  struct fs_stat          file_stat = {0};

  /* --------------------------------------------------------------------------
     Perform basic param checking.
     1.  file_handle_ptr can not be NULL
     -------------------------------------------------------------------------*/
  if ( file_name_ptr == NULL )
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_GENERIC_ERROR;
  }

  /* --------------------------------------------------------------------------
     Criteria for success is if the file/dir is found
     ------------------------------------------------------------------------*/
  if(efs_stat(file_name_ptr, &file_stat) == 0)
  {
    return SIMLOCK_SUCCESS;
  }

  SIMLOCK_MSG_HIGH_0("efs_stat procedure failed ");
  return SIMLOCK_GENERIC_ERROR;
} /* simlock_efs_name_test */


/*===========================================================================
FUNCTION SIMLOCK_EFS_SYNC

DESCRIPTION
  This function performs an EFS sync operation, which commits the
  data to the filesystem.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_sync
(
  void
)
{
  if(efs_sync("/") == 0)
  {
    simlock_efs_sync_done = TRUE;
    return SIMLOCK_SUCCESS;
  }

  SIMLOCK_MSG_ERR_0("efs_sync procedure failed");
  return SIMLOCK_GENERIC_ERROR;
} /* simlock_efs_sync */


/*===========================================================================
FUNCTION SIMLOCK_EFS_CHECK_EFS_STATUS

DESCRIPTION
  This function checks EFS status by performing certain EFS operations
  and finally an EFS sync operation, which commits the data to the
  filesystem. This is done to ensure that an EFS operation can be performed
  when it is needed.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_check_efs_status
(
  void
)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  char *simlock_efs_temp_file = "simlock_temp";
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  uint8  efs_file_write_data = 1;

  /* if the EFS SYNC was done already return */
  if(simlock_efs_sync_done)
  {
    return SIMLOCK_SUCCESS;
  }

  /* this is the first time performing EFS operation. So
     write one byte to a temp file */
  if(simlock_efs_write_file(simlock_efs_temp_file, sizeof(uint8),
                              &efs_file_write_data) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* delete the temp file */
  if(efs_unlink(simlock_efs_temp_file) < 0)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* perform an efs sync */
  return simlock_efs_sync();
} /* simlock_efs_check_efs_status */


/*===========================================================================
FUNCTION SIMLOCK_EFS_DELETE_FILE

DESCRIPTION
  This function deletes the file

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_efs_delete_file
(
  const char * file_name_ptr
)
{
  struct fs_stat    file_stat   = {0};

  if (file_name_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* Remove the file */
  if(efs_stat(file_name_ptr, &file_stat) == 0)
  {
    if(efs_unlink(file_name_ptr) < 0)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }
  else if (efs_errno != ENOENT)
  {
    /* Test failed due to reason other than "File Does not Exists".
       efs_errno is defined in fs_public.h and provides the
       reason for EFS API failure */
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_efs_delete_file */

#endif /* FEATURE_SIMLOCK */

