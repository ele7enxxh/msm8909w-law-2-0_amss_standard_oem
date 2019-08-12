/*===========================================================================

                     GNSS common NV
GENERAL DESCRIPTION
  This module contains the implementation for accessing the GNSS common NV items,

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gnss_common_nv.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
05/12/2014   ns      Initial release
==========================================================================*/


/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/

#include "comdef.h"
#include "msg.h"
#include "gps_nv_efs.h"
#include "gnss_common_nv.h"

/*---------------------------------------------------------------------------
 * Global variables
  ---------------------------------------------------------------------------*/

/* Add common NV items here */
static const cgps_nv_efs_reg_item_struct commonNvRegItems[] =
{
  {
    CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL, /* NV control is visible to external parties*/
    "common/gnss_fp_exception_config", /* NV item file name*/
    sizeof(uint64), /* 64 bits */
    0, /* precision not applicable */
    CGPS_NV_EFS_REG_TYPE_INTEGRAL /* integer */
  },
};

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

boolean gnssCommonNvInit(void)
{
  static boolean b_Inited = FALSE;

  /* Initialize only if not previously initialized */
  if(FALSE == b_Inited)
  {
    b_Inited =  cgps_nv_efs_init( CGPS_NV_EFS_COMMON,
                                  GNSS_NV_EFS_COMMON_MIN_ITEM,
                                  GNSS_NV_EFS_COMMON_MAX_ITEM,
                                  commonNvRegItems);
  }
  return b_Inited;
}

/*
 ******************************************************************************
 * Function description:
 *   gnssCommonNvRead: Read the specified common NV item
 *
 * Parameters:
 *   nvItem: nv item to read
 *   pData : data buffer into which the nv item should be read
 *   size : size of the nv item to read.
 *
 * Return value:
 *   TRUE if success, FALSE if read failed
 *
 ******************************************************************************
*/
boolean gnssCommonNvRead(cgps_nv_efs_reg_item_type nvItem,
                         void *pData, uint32 size)
{
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;

  if((NULL != pData) && (size > 0))
  {
    status =  cgps_nv_efs_reg_item_read( nvItem,
                                         size,
                                         pData,
                                         GNSS_NV_EFS_COMMON_MIN_ITEM,
                                         GNSS_NV_EFS_COMMON_MAX_ITEM,
                                         commonNvRegItems);
  }

  return (status == CGPS_NV_EFS_REG_RW_STAT_OK );
}


