/*===========================================================================


      R F   C o m m o n  EFS   A P I

                            S o u r c e  F i l e

DESCRIPTION
  This file contains common functions to access EFS file system.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_efs.c#1 $
  $DateTime: 2016/12/13 07:59:52 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/28/14   cd      Provide common EFS API to query EFS file size
07/21/14   cd      Updates to common EFS functionality to support devices
08/27/12   jr      Added rfcommon_efs_put API
03/02/12   sr      Initial version
============================================================================*/


/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "rfcommon_efs.h" 
#include "msg.h"
#include "fs_lib.h"
#include "fs_public.h"
#include <stdio.h>  


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the status of specified file from EFS file system.

  @details
  This function reads specified file from EFS file system.
  If there is any error while reading the file, it returns the data size
  read as -1. Else it returns the valid file size in bytes

  @param
  'file_name' : name of the file to be read from EFS file system.
 
  @retval
  -1 if there is any error while reading the specified file.
  size of the file in bytes : if the spcified file read successfully.
*/
int32 rfcommon_efs_get_size( const char *file_name, 
                             rfcommon_efs_fstat_type *fs_buf )
{
  int32 data_size = -1;
  struct fs_stat temp_buf;

  if (file_name == NULL)
  {
    return data_size;
  }

  /* check to see whether the file is available or not */
  if( efs_stat(file_name,&temp_buf) == -1 )
  {
    MSG_SPRINTF_1( MSG_SSID_RF, MSG_LEGACY_ERROR, 
                   "RF EFS Read Error: File not found! %s", file_name );

    return data_size;
  }

  /* return size of file in EFS */
  data_size = fs_buf->st_size = temp_buf.st_size;
  return data_size;

}/* rfcommon_efs_get_size */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads specified file from EFS file system.

  @details
  This function reads specified file from EFS file system. if the file is larger than expected or if
  there is any error while reading the file, it returns the data size read as -1.

  @param
  'file_name' : name of the file to be read from EFS file system.
  'data' : ptr to where read file data to be stored.
  'data_max_size' : max data size expected to be read from EFS file.
 
  @retval
  0  if there is any error while reading the specified file.
  size of the file in bytes : if the spcified file read successfully.
*/
int32 rfcommon_efs_get(const char *file_name, void *data, int32 data_max_size)
{
  int32 data_size = -1;
  struct fs_stat temp_buf;

  if (file_name == NULL)
  {
    return data_size;
  }

  /* check to see whether the file is available or not */
  if(efs_stat(file_name,&temp_buf) == -1) 
  {
    MSG_SPRINTF_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF EFS Read Error: File not found! %s", file_name);
    return data_size;
  }

  /* make sure that the file size doesn't exceed the expected file size */
  if (temp_buf.st_size > data_max_size)
  {
    MSG_SPRINTF_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF EFS Read Error: File size(%d) larger than expected(%d)! %s", 
                  (int)(temp_buf.st_size), (int)data_max_size, file_name);
    return data_size;
  }

  /* read the file into buffer */
  data_size = efs_get(file_name, data, temp_buf.st_size);

  return data_size;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function writes data to EFS file system.

  @details
  This function writes data to EFS file system. if the file is larger than expected or if
  there is any error while writing the file, it returns the data size read as -1.

  @param
  'file_name' : name of the file to be read from EFS file system.
  'data' : ptr to where writes data
  'data_max_size' : max data size expected to be write to  EFS file.
 
  @retval
  0  if there is any error while writing the specified file.
  size of the file in bytes : if the spcified file read successfully.
*/
boolean rfcommon_efs_put(const char *rfc_efs_file_name, void *data, int32 data_max_size, boolean append_flag)
{
  int oflag;
  int mode;
  int ret_val =0; /* return value for efs_put */  
  int fd;

  if (rfc_efs_file_name == NULL || data == NULL)
  {
    MSG_SPRINTF_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "In correct argument values for file name, data & data_size = %d", data_max_size);
    return FALSE;
  }

  /* special flags.
      O_AUTODIR : Auto create parent directories
      O_CREAT : Create the item if it does not exist
      O_RDWR : To specify that file will be used for reading and writing
      O_APPEND : Append the file if it exists 
      O_TRUNC : truncate the file if it exists */

  mode = 0777; 	 /* initial permissions */

  if(append_flag == 0)
  {

    oflag=  O_RDWR | O_CREAT | O_TRUNC | O_AUTODIR;

    ret_val = efs_put( rfc_efs_file_name,
		       (void *)data,
			data_max_size,
			oflag,
			mode );
	  
  }
  else
  {
    
    oflag= O_APPEND| O_RDWR;	
    fd = efs_open (rfc_efs_file_name, oflag, mode);
    if (fd > 0)
    {
      ret_val = efs_write (fd, data, data_max_size);
      if (ret_val != data_max_size) 
	  {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "error in appending data = %d", ret_val);
      }
      ret_val = efs_close (fd);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "error in opening file = %d", fd);
    }
  }

  if (ret_val == -1)
  {  
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Error while writing EFS file data, error code %d", ret_val);
	return FALSE;
  }

  return TRUE;

}