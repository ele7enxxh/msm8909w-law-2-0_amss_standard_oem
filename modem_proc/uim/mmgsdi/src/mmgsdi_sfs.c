/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                              M M G S D I  T A S K

                                    A N D

      S E C U R E F I L E  S Y S T E M  I N T E R F A C E  F U N C T I O N S


GENERAL DESCRIPTION

  This source files contains the MMGSDI to SFS Interface Functions used
  by the MMGSDI Task.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or
software is regulated by the U.S. Government. Diversion contrary to U.S. law
prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sfs.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/16   kv      UIM PSM handling updates
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
01/10/14   df      Use v2.0 diag macros
06/11/13   av      Initial Revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "customer.h"
#include "string.h"
#include "err.h"
#include "mmgsdi.h"
#include "mmgsdi_sfs.h"
#include "fs_fcntl.h"
#include "uim_msg.h"

#include "sfs_api.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ------------------------------------------------------------------------*/
/*                   M M G S D I   S F S   I N T E R F A C E               */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION MMGSDI_SFS_GET_SIZE

DESCRIPTION
  This function will get the size of the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    File size found
                            MMGSDI_ERROR:     SFS error
                            MMGSDI_NOT_FOUND: SFS file did not exist
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_get_size(
  uint32      *size_ptr,
  const char  *sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  int                     sfs_status    = 0;
  int                     mmgsdi_sfs_fd = 0;

  if(size_ptr == NULL ||
     sfs_file_name == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_sfs_fd = sfs_open(sfs_file_name, O_RDONLY);

  if(!mmgsdi_sfs_fd)
  {
    UIM_MSG_ERR_0("Failed to open sfs file");
    return MMGSDI_NOT_FOUND;
  }

  sfs_status = sfs_getSize(mmgsdi_sfs_fd, size_ptr);

  if(sfs_status != E_SUCCESS)
  {
    mmgsdi_status = MMGSDI_ERROR;
  }

  (void)sfs_close(mmgsdi_sfs_fd);

  return mmgsdi_status;
#else
 (void)size_ptr;
 (void)sfs_file_name;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_SEC_SFS */
}/* mmgsdi_sfs_get_size */


/*===========================================================================
FUNCTION MMGSDI_SFS_MKDIR

DESCRIPTION
  This function will make the sfs directory for perso.


DEPENDENCIES
  The parent directory has already been created.

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Directory was created
                            GSDI_ERROR:     Error creating directory
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_mkdir(
  const char  *sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  int mkdir_status = 0;

  UIM_MSG_HIGH_0("Creating sfs directory.");
  if(sfs_file_name == NULL)
  {
    return MMGSDI_ERROR;
  }

  mkdir_status = sfs_mkdir(sfs_file_name);
  if(mkdir_status == E_FAILURE)
  {
    UIM_MSG_ERR_0("Error creating sfs directory.");
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
#else
  (void)sfs_file_name;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_SEC_SFS */

} /* mmgsdi_sfs_mkdir */


/*===========================================================================
FUNCTION MMGSDI_SFS_RMFILE

DESCRIPTION
  Generic Function called by GSDI to delete a file from SFS.  This function
  will validate the input parameters provided and return a status when
  deleted.

DEPENDENCIES
  The file does not exist.

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    File was removed
                            MMGSDI_ERROR:     File removal error

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_rmfile(
  const char* sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  int                     sfs_status    = 0;
  int                     mmgsdi_sfs_fd = 0;

  if(sfs_file_name == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_sfs_fd = sfs_open(sfs_file_name, O_RDONLY);
  if(mmgsdi_sfs_fd)
  {
    UIM_MSG_HIGH_0("Creating data on sfs for first time. Make sure the sfs file doesn't exist");
    sfs_status = sfs_rm(sfs_file_name);
    if(sfs_status != E_SUCCESS)
    {
      UIM_MSG_ERR_0("sfs file deletion failed");
      mmgsdi_status = MMGSDI_ERROR;
    }
  }

  return mmgsdi_status;
#else
  (void)sfs_file_name;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_SEC_SFS */
} /* mmgsdi_sfs_rmfile */


/*===========================================================================
FUNCTION MMGSDI_SFS_WRITE

DESCRIPTION
  This function will write to the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Successful write
                            MMGSDI_ERROR:     SFS error
                            MMGSDI_NOT_FOUND: Unable to open file
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_write(
  uint8      *buffer_ptr,
  uint32     *len,
  const char *sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  int                     sfs_status    = 0;
  int                     mmgsdi_sfs_fd = 0;

  if(buffer_ptr == NULL ||
     sfs_file_name == NULL ||
     len == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_sfs_fd = sfs_open(sfs_file_name, O_WRONLY);
  if(!mmgsdi_sfs_fd)
  {
    UIM_MSG_HIGH_0("Trying to create sfs file");
    mmgsdi_sfs_fd = sfs_open(sfs_file_name, O_RDWR|O_CREAT);
    if(!mmgsdi_sfs_fd)
    {
      UIM_MSG_ERR_0("Failed to open sfs file");
      return MMGSDI_NOT_FOUND;
    }
  }

  sfs_status = sfs_write(mmgsdi_sfs_fd, (char *)buffer_ptr, uint32toint32(*len));
  if(sfs_status == (int)*len)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  (void)sfs_close(mmgsdi_sfs_fd);

  UIM_MSG_HIGH_1("sfs file WRITING returned 0x%x", mmgsdi_status);
  return mmgsdi_status;
#else
  (void)buffer_ptr;
  (void)len;
  (void)sfs_file_name;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_SEC_SFS */
}/* mmgsdi_sfs_write */


/*===========================================================================
FUNCTION MMGSDI_SFS_READ

DESCRIPTION
  This function will read from the sfs file.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCES:    Successful read
                            MMGSDI_ERROR:     SFS error
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sfs_read(
  uint8      *buffer_ptr,
  uint32      len,
  const char *sfs_file_name)
{
#ifdef FEATURE_SEC_SFS
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_NOT_FOUND;
  int                     sfs_status    = 0;
  int                     mmgsdi_sfs_fd = 0;

  if(buffer_ptr == NULL ||
     sfs_file_name == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_sfs_fd = sfs_open(sfs_file_name, O_RDONLY);

  if(!mmgsdi_sfs_fd)
  {
    UIM_MSG_ERR_0("Failed to open sfs file");
    return mmgsdi_status;
  }

  sfs_status = sfs_read(mmgsdi_sfs_fd, (char *)buffer_ptr, len);

  if(sfs_status == (int)len)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  (void)sfs_close(mmgsdi_sfs_fd);

  return mmgsdi_status;
#else
  (void)buffer_ptr;
  (void)len;
  (void)sfs_file_name;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_SEC_SFS */
}/* mmgsdi_sfs_read */

