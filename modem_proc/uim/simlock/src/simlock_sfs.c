/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   S F S   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the sfs functions.

 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_sfs.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/15   vv      Fix a possible memory leak
06/03/14   vv      Fix a compiler issue
04/15/14   vv      Added the support for FEATURE_SEC_SFS
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "string.h"
#include "fs_fcntl.h"
#include "simlock_sfs.h"

#ifndef FEATURE_UIM_TEST_FRAMEWORK
#include "sfs_api.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_SFS_WRITE_FILE

DESCRIPTION
  This function gets the size of the requested file by calling the SFS based
  get file size function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_write_file
(
  uint8      * data_buf_ptr,
  uint32       data_buf_len,
  const char * sfs_file_name
)
{
#ifdef FEATURE_SEC_SFS
  simlock_result_enum_type   write_file_status  = SIMLOCK_SUCCESS;
  int                        sfs_fd             = 0;
  int                        sfs_status         = 0;

  if((data_buf_ptr == NULL) ||
     (data_buf_len == 0) ||
     (sfs_file_name == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* if the file already exists remove it */
  sfs_fd = sfs_open(sfs_file_name, O_RDONLY);
  if(sfs_fd)
  {
    (void)sfs_close(sfs_fd);

    sfs_status = sfs_rm(sfs_file_name);
    if(sfs_status != E_SUCCESS)
    {
      SIMLOCK_MSG_ERR_0("sfs file deletion failed");
      return SIMLOCK_GENERIC_ERROR;
    }
  }

  sfs_fd = sfs_open(sfs_file_name, O_WRONLY);
  if(!sfs_fd)
  {
    sfs_fd = sfs_open(sfs_file_name, O_RDWR|O_CREAT);
    if(!sfs_fd)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }

  sfs_status = sfs_write(sfs_fd, (char *)data_buf_ptr, (int32)data_buf_len);
  if(sfs_status == data_buf_len)
  {
    write_file_status = SIMLOCK_SUCCESS;
  }

  sfs_status = sfs_close(sfs_fd);
  if(sfs_status != E_SUCCESS)
  {
    write_file_status = SIMLOCK_GENERIC_ERROR;
  }

  return write_file_status;
#else
  (void)data_buf_ptr;
  (void)data_buf_len;
  (void)sfs_file_name;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
} /* simlock_sfs_write_file */


/*===========================================================================
FUNCTION SIMLOCK_SFS_GET_FILE_SIZE

DESCRIPTION
  This function gets the size of the requested file by calling the SFS based
  get file size function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_get_file_size(
  const char  * sfs_file_name,
  uint32      * size)
{
#ifdef FEATURE_SEC_SFS
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;
  int                      sfs_status     = 0;
  int                      simlock_sfs_fd = 0;

  if(size == NULL ||
     sfs_file_name == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_sfs_fd = sfs_open(sfs_file_name, O_RDONLY);
  if(!simlock_sfs_fd)
  {
    SIMLOCK_MSG_ERR_0("sfs_open error");
    return SIMLOCK_GENERIC_ERROR;
  }

  sfs_status = sfs_getSize(simlock_sfs_fd, size);
  if(sfs_status != E_SUCCESS)
  {
    SIMLOCK_MSG_ERR_0("sfs_getSize error");
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  sfs_status = sfs_close(simlock_sfs_fd);
  if(sfs_status != E_SUCCESS)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  return simlock_status;
#else
  (void)sfs_file_name;
  (void)size;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
}/* simlock_sfs_get_file_size */


/*===========================================================================
FUNCTION SIMLOCK_SFS_READ_FILE

DESCRIPTION
  This function reads the data from the requested file by calling the SFS 
  based read function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_read_file
(
  uint8      * buffer_ptr,
  uint32       len,
  const char * sfs_file_name
)
{
#ifdef FEATURE_SEC_SFS
  simlock_result_enum_type simlock_status = SIMLOCK_GENERIC_ERROR;
  int                      sfs_status     = 0;
  int                      simlock_sfs_fd = 0;

  if(buffer_ptr == NULL ||
     sfs_file_name == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_sfs_fd = sfs_open(sfs_file_name, O_RDONLY);
  if(!simlock_sfs_fd)
  {
    return simlock_status;
  }

  sfs_status = sfs_read(simlock_sfs_fd, (char *)buffer_ptr, len);
  if(sfs_status == len)
  {
    simlock_status = SIMLOCK_SUCCESS;
  }

  sfs_status = sfs_close(simlock_sfs_fd);
  if(sfs_status != E_SUCCESS)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  return simlock_status;
#else
  (void)buffer_ptr;
  (void)len;
  (void)sfs_file_name;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
}/* simlock_sfs_read_file */


/*===========================================================================
FUNCTION SIMLOCK_SFS_MKDIR

DESCRIPTION
  This function creates a directory by calling the SFS based make dir 
  function

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_sfs_mkdir(
  const char  * sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  int mkdir_status = 0;

  if(sfs_file_name == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  mkdir_status = sfs_mkdir(sfs_file_name);
  if(mkdir_status == E_FAILURE)
  {
    SIMLOCK_MSG_ERR_0("Error creating sfs directory.");
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
#else
  (void)sfs_file_name;
  return SIMLOCK_UNSUPPORTED;
#endif /* FEATURE_SEC_SFS */
} /* simlock_sfs_mkdir */

#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_SIMLOCK */

