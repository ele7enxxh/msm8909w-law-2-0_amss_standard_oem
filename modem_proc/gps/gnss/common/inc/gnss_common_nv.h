#ifndef GNSS_COMMON_NV_H
#define GNSS_COMMON_NV_H
/*===========================================================================

                     GNSS common NV 
GENERAL DESCRIPTION
  This module contains the API's for accessing the GNSS common NV items,

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_common_nv.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
05/12/2014   ns      Initial release
==========================================================================*/ 

/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/
#include "comdef.h"
#include "gps_nv_efs.h"
/*
 ******************************************************************************
 * Function description:
 *   gnssCommonNvInit: Initialize the Common NV items. If the NV items have
 *   been initialized already then this function simply returns TRUE.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   TRUE if success, FALSE if initialization failed
 *
 ******************************************************************************
*/
boolean gnssCommonNvInit(void) ;

/*
 ******************************************************************************
 * Function description:
 *   gnssCommonNvRead: Read the specified common NV item
 *
 * Parameters:
 *   nvItem: nv item to read
 *   pData: data buffer into which the nv item should be read
 *   size : size of the nv item to read.
 *
 * Return value:
 *   TRUE if success, FALSE if read failed
 *
 ******************************************************************************
*/
boolean gnssCommonNvRead(cgps_nv_efs_reg_item_type nvItem,
                         void *pData, 
                         uint32 size);

#endif // GNSS_COMMON_NV_H
