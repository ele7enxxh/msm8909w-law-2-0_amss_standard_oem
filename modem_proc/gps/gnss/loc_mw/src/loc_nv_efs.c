/*===========================================================================

                     LOC_MW NV EFS 
GENERAL DESCRIPTION
  This module contains the implementation for accessing the LOC_MW NV items,

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_nv_efs.c#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
09/01/2015   yh       Initial release
==========================================================================*/ 


/*--------------------------------------------------------------------------
 * Include Files
 * -----------------------------------------------------------------------*/

#include "comdef.h"
#include "msg.h"
#include "gps_nv_efs.h"
#include "loc_nv_efs.h"

/*--------------------------------------------------------------------------- 
 * Global variables 
  ---------------------------------------------------------------------------*/

/* Add common NV items here */
static const cgps_nv_efs_reg_item_struct locNvRegItems[] =
{
  /* data location, item file name, size */

  /* GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL */
  {
	CGPS_NV_EFS_REG_ST_ITEM_INTERNAL, /* NV control is not visible to external parties*/
	"loc/loc_supported_min_interval", /* NV item file name*/
	sizeof(uint32), /* 32 bits */
	0, /* precision not applicable */
	CGPS_NV_EFS_REG_TYPE_INTEGRAL /* integer */ 
  },

	
};

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

boolean locNvEfsInit(void) 
{
  static boolean v_Inited = FALSE;

  /* Initialize only if not previously initialized */
  if(FALSE == v_Inited)
  {
    v_Inited =  cgps_nv_efs_init( CGPS_NV_EFS_LOC,
                                  GNSS_NV_EFS_LOC_MIN_ITEM,
                                  GNSS_NV_EFS_LOC_MAX_ITEM,
                                  locNvRegItems);
  }

  MSG_MED("locNvEfsInit, v_Inited = %d", v_Inited,0,0);
  return v_Inited;
}

/*
 ******************************************************************************
 * Function description:
 *   locNvEfsRead: Read the specified LOC_MW NV item
 *
 * Parameters:
 *   nvItem: nv item to read
 *   pData: data buffer into which the nv item should be read
 *   q_size : q_size of the nv item to read.
 *
 * Return value:
 *   TRUE if success, FALSE if read failed
 *
 ******************************************************************************
*/
boolean locNvEfsRead(cgps_nv_efs_reg_item_type nvItem,
                         void *pData, uint32 q_size)
{
  cgps_nv_efs_reg_rw_stat_enum_type status = CGPS_NV_EFS_REG_RW_STAT_INVALID;
    
  if((NULL != pData) && (q_size > 0))
  {
    status =  cgps_nv_efs_reg_item_read( nvItem, 
                                         q_size, 
                                         pData, 
                                         GNSS_NV_EFS_LOC_MIN_ITEM,
                                         GNSS_NV_EFS_LOC_MAX_ITEM, 
                                         locNvRegItems);
  }

  MSG_MED("locNvEfsRead, status = %d", status,0,0);
      
  return (status == CGPS_NV_EFS_REG_RW_STAT_OK );
}


