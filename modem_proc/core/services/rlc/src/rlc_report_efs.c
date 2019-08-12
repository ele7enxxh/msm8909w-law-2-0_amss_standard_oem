/**
@file rlc_report_efs.c
@brief
This module implements reporting through the embedded file system. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through EFS.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_report_efs.h"
#include "msg_diag_service.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Private function for initializing reporting through EFS for a client.

@param[in]  handle      Client handle to be passed in for the client name.
@param[in]  pAttribute  Internal attribute pointer to fill in.

@return
RLC_SUCCESS --  Successfully setup EFS attribute for the client.
RLC_FAILED --   Error in setting up EFS attribute for the client.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_efs_init(rlc_handle_ptr              handle,
                                           rlc_attribute_type          *pAttribute)
{
  int nResult,nBytes;
  char attr_path[RLC_MAX_NAME_SIZE];
  rlc_efs_type* sLocalEfs = NULL;

  if(pAttribute == NULL ||
     pAttribute->report_ptr == NULL)
  {
    return RLC_FAILED;
  }

  /* Check to see if we've already initialized */
  if(pAttribute->report_ptr->report_handle != NULL)
  {
    /* If initialized, just return */
    return RLC_SUCCESS;
  }

  /* Allocate memory for the attribute */
  sLocalEfs = ( rlc_efs_type* ) rlc_malloc( sizeof( rlc_efs_type ) );

  /* Fail if we can't malloc properly */
  if( sLocalEfs == NULL )
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Malloc failed in rlc_efs_init for client: %s",handle->name);
    return RLC_NO_MEMORY;
  }

  /* Write the attribute path based on the name of the client */
  snprintf(attr_path,RLC_MAX_NAME_SIZE,"rlc_attr_%s",handle->name);

  /* Try to create a file */
  nResult = efs_open(attr_path,O_CREAT|O_RDWR|O_EXCL|O_APPEND, 0777);

  /* Check if file exists already based on the error */
  if( nResult < 0 )
  {
    /* If the file exists, efs_errno == EEXIST */
    if( efs_errno == EEXIST )
    {
      /* File has already been created, we just need to restore the contents */
      nResult = efs_open(attr_path,O_RDWR|O_APPEND, 0777);

      if( nResult < 0 )
      {
        /* Cannot handle this case! We cannot read file. */
        MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Cannot write to file that already exists! Path:%s",attr_path);
        return RLC_FS_ERROR;
      }

      /* Read the file from FS */
      nBytes = efs_read(nResult, sLocalEfs, sizeof(rlc_efs_type));

      /* Close the file, it is not needed anymore */
      efs_close(nResult);

      /* Check if the full size of the file has been read */
      if(nBytes != sizeof(rlc_efs_type))
      {
        MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Failed to read full attributes from file system! Path:%s",attr_path);
        return RLC_FS_ERROR;
      }
    }
    else
    {
      /* Cannot handle any other errors here, bail out */
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Failed to read full attributes from file system! Path:%s",attr_path);
      return RLC_FS_ERROR;
    }

    /* Copy the handle of the attributes */
    sLocalEfs->attr_handle   = nResult;
  }
  else
  {
    /* File successfully created, let's create the proper attributes for it */
    sLocalEfs->attr_handle   = nResult;
    sLocalEfs->file_number   = 1;
    sLocalEfs->file_size     = 0;
    sLocalEfs->file_max_size = RLC_EFS_FILE_MAX_SIZE;

    /* Copy the name of the file into the space allocated */
    snprintf(sLocalEfs->file_name, RLC_MAX_NAME_SIZE, "rlc_%s_%d", handle->name, 1);
    snprintf(sLocalEfs->handle_name, RLC_MAX_NAME_SIZE, "%s", handle->name);
    snprintf(sLocalEfs->attr_name, RLC_MAX_NAME_SIZE, "%s", attr_path);

    nBytes = efs_write(nResult, sLocalEfs, sizeof(rlc_efs_type));
    if( nBytes < sizeof(rlc_efs_type) )
    {
      /* Cannot write to EFS right now for some reason */
      MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Cannot write to EFS! Bytes Written:%n Bytes Attempted:%n Path:%s",nBytes,sizeof(rlc_efs_type),attr_path);
      return RLC_FS_ERROR;
    }
  }

  /* Todo: Handle O_EXCL for if file exists already */
  sLocalEfs->file_handle = efs_open(sLocalEfs->file_name,O_CREAT|O_RDWR|O_APPEND, 0777);

  /* Check if file exists already based on the error */
  if(sLocalEfs->file_handle < 0)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Cannot open the file selected, name: %s",sLocalEfs->file_name);
    return RLC_FAILED;
  }

  pAttribute->report_ptr->report_handle = (rlc_report_output_handle)sLocalEfs;

  return RLC_SUCCESS;
}

/** 
Private function for reporting through EFS.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS --  Successfully reported through EFS.
RLC_FAILED  --  Error reporting through EFS.
 
@dependencies  
pAttribute must be setup properly in private efs_init function.
*/

rlc_error_type rlc_report_efs(rlc_attribute_type *pAttribute, char* string)
{
  rlc_efs_type* sLocalEfs = NULL;
  fs_ssize_t        bytesWritten, newBytesWritten;
  fs_size_t         writeLength;
  int               nResult;

  if( pAttribute == NULL ||
      pAttribute->report_ptr == NULL ||
      pAttribute->report_ptr->report_handle == NULL ||
      string == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }

  sLocalEfs = (rlc_efs_type*)pAttribute->report_ptr->report_handle;

  writeLength = strlen(string);
  bytesWritten = efs_write(sLocalEfs->file_handle, string, writeLength);

  if(bytesWritten == writeLength)
  {
    return RLC_SUCCESS;
  }
  if((bytesWritten < (fs_ssize_t)writeLength) && (bytesWritten > 0))
  {
    /* Not all data was written properly, we need to create a new file */
    sLocalEfs->file_number++;
    
    /* Close the old file since it's full */
    efs_close(sLocalEfs->file_handle);

    /* Copy the new name over */
    snprintf(sLocalEfs->file_name, RLC_MAX_NAME_SIZE, "rlc_%s_%d", sLocalEfs->handle_name, (int)sLocalEfs->file_number);

    /* Let's write the rest of the data into the new file */
    sLocalEfs->file_handle = efs_open(sLocalEfs->file_name,O_CREAT|O_RDWR|O_APPEND, 0777);

    newBytesWritten = efs_write(sLocalEfs->file_handle, (char*)(&string + bytesWritten), (writeLength - bytesWritten));

    if(newBytesWritten < ((fs_ssize_t)writeLength - bytesWritten))
    {
      /* This is bad... might have to bail out here */
    }
                                                            
    /* Let's update the attribute now */
    nResult = efs_open(sLocalEfs->attr_name,O_WRONLY,0777);

    efs_write(nResult, sLocalEfs, sizeof(rlc_efs_type));

    efs_close(nResult);

    return RLC_SUCCESS;
  }
  return RLC_FAILED;
}
