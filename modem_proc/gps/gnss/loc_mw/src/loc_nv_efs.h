#ifndef LOC_NV_EFS_H
#define LOC_NV_EFS_H
/*===========================================================================

                     LOC_MW NV EFS
GENERAL DESCRIPTION
  This module contains the API's for accessing the LOC_MW NV items,

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_nv_efs.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
09/01/2015   yh      Initial release
==========================================================================*/ 

/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/
#include "comdef.h"
#include "gps_nv_efs.h"

/* This default value can be controlled at compile time */
#ifdef FEATURE_LOCTECH_10HZ
#define GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT   (100) /* 100 msec */
#elif FEATURE_LOCTECH_5HZ
#define GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT   (200) /* 200 msec */
#elif FEATURE_LOCTECH_2HZ
#define GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT   (500) /* 500 msec */
#elif FEATURE_LOCTECH_1HZ
#define GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT   (1000) /* 1000 msec */
#else //Default Value
#define GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL_DEFAULT   (1000) /* 1000 msec */
#endif 

/*
 ******************************************************************************
 * Function description:
 *   locNvEfsInit: Initialize the LOC_MW NV items. If the NV items have
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
boolean locNvEfsInit(void) ;

/*
 ******************************************************************************
 * Function description:
 *   locNvEfsRead: Read the specified LOC_MW NV item
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
boolean locNvEfsRead(cgps_nv_efs_reg_item_type nvItem,
                         void *pData, 
                         uint32 q_size);

#endif // LOC_NV_EFS_H
