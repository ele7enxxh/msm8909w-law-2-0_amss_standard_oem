/*!
  @file
  coex_nv.c

  @brief
  Implementation of APIs to manage for CXM's connection/transport interfaces

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                                 ( CXM )

                   Co-Existence Manager Source File

GENERAL DESCRIPTION

  This file provides Interface to the Connection Manager's NV items


EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coex_nv.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/20/14   tak     Updated to version 8 NV
09/30/13   tak     Updated to version 7 NV
12/21/12   btl     Port Optimize efs init writes at boot if file exists and
                     cxm_uart baud rate powerup fix
09/27/12   cab     Cleaned up compiler warning for unused return value
09/06/12   cab     Set default values, remove assert
07/20/12   cab     Extend featurization to deal with lte deps
07/18/12   cab     Updated NV versioning and protocols
07/10/12   cab     Adding some messaging
05/16/12   cab     Initial revision

==========================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include <IxErrno.h>
#include <comdef.h>
#include "fs_public.h"
#include "msg.h"
#include "coex_nv.h"
#include "coex_algos.h"
#include "cxm_utils.h"
#include <wci2_uart.h>
#include "npa.h"

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/


/*=============================================================================

                       INTERNAL LOCAL HELPER FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_nv_set_default

=============================================================================*/
/*!
    @brief
    This method maintains the internal mapping between the indication method
    id and offset of the message in the message map.

    @return
    int32 offset
*/
/*===========================================================================*/
static void coex_nv_set_default (
  coex_config_params_v8 *coex_nv
)
{
  int32 fd;

/*--------------------------------------------------------------------------*/

  /*set coex params to default values*/
  CXM_MSG_0( HIGH, "Setting NV to default values" );
  set_coex_params();

  fd = efs_open( COEX_CONFIG_DATA_FILE_PATH, O_CREAT|O_WRONLY|O_TRUNC, 
                 ALLPERMS );
  if ( fd < 0 )
  {
    /* cannot open the file for writing, what gives? */
    CXM_MSG_0( ERROR, "Cannot write data to NV" );
  }
  else
  {
    /* write default data to file */
    efs_write( fd, coex_nv, sizeof(coex_config_params_v8) );
    efs_close( fd );
  }

}

/*=============================================================================

  FUNCTION:  coex_nv_read_config_data

=============================================================================*/
/*!
    @brief

    @return
*/
/*===========================================================================*/
void coex_nv_read_config_data (
  coex_config_params_v8 *coex_nv
)
{
  uint8 version = 0;
  int32 fd = efs_open( COEX_CONFIG_DATA_FILE_PATH, O_RDONLY, ALLPERMS );

  if ( fd < 0 )
  {
    /* could not read the existing file */

    /* create default values and open file to write */
    efs_mkdir( COEX_CONFIG_DATA_FILE_DIR, ALLPERMS );
    coex_nv_set_default(coex_nv);
  }
  else
  {
    efs_read( fd, &version, sizeof(uint8) );
    efs_close( fd );

    CXM_MSG_1( HIGH, "NV version %d found, expected verson 8", version );

    if ( version == CXM_CONFIG_VERSION_8 )
    {
      /* read version 8 data from file */
      fd = efs_open( COEX_CONFIG_DATA_FILE_PATH, O_RDONLY, ALLPERMS );
      efs_read( fd, coex_nv, sizeof(coex_config_params_v8) );
      efs_close( fd );
    }
    else
    {
      coex_nv_set_default(coex_nv);
    }
  }
}

/*=============================================================================

  FUNCTION:  coex_nv_init

=============================================================================*/
/*!
    @brief

    @return
*/
/*===========================================================================*/
void coex_nv_init (
  coex_config_params_v8 *coex_nv
)
{
  int32          fd;
  char           eol = '\n';
  struct fs_stat conf_file_det;
  size_t         data_size;
  int            file_stat_result;

/*--------------------------------------------------------------------------*/

  file_stat_result = efs_stat(CXM_EFS_CONF_FILE_PATH, &conf_file_det);
  data_size = (size_t)( strlen(COEX_CONFIG_DATA_FILE_PATH) + sizeof(eol) );

  /* If the conf file does not exist or if the size of the existing conf file is
     not what we expect, create a new conf file */
  if ( file_stat_result != 0 || data_size != conf_file_det.st_size )
  {
    /* Create conf file for backing up EFS files, erase it if it already exists */
    fd = efs_open( CXM_EFS_CONF_FILE_PATH, O_CREAT|O_WRONLY|O_TRUNC, ALLPERMS );

    if (fd < 0)
    {
      CXM_MSG_1( ERROR, "Error opening EFS config file %d", fd );
    }
    else
    {
      /* Write the conf file */
      efs_write( fd, COEX_CONFIG_DATA_FILE_PATH, 
                 strlen(COEX_CONFIG_DATA_FILE_PATH) );
      efs_write( fd,&eol,sizeof(eol) );

      efs_close( fd );
    }
  }

  /* obtain or create coex data in NV */
  coex_nv_read_config_data(coex_nv);  
}



