#ifndef WLE_EFS_H
#define WLE_EFS_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      GTP-WIFI-LP Location Engine (WLE) Internal header file

GENERAL DESCRIPTION
  This file contains data-structure/function prototypes for EFS access in
  WLE module
     
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Copyright (c) 2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wle/common/inc/wle_nv_efs.h#1 $
  
when       who   what, where, why
--------   ---   ----------------------------------------------------------
04/06/15   dc     Initial release - GNSS_NV_EFS_WLE_WLM_ASYNC_SCAN_CONTROL_DEFAULT supported
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "gps_nv_efs.h"

/*===========================================================================
=============================================================================

     Macros
   
=============================================================================
===========================================================================*/

#define GNSS_NV_EFS_WLE_WLM_ASYNC_SCAN_CONTROL_DEFAULT (1) /* 0:Disabled, 1: Enabled */ 

/*===========================================================================
=============================================================================

   WLE data structures and declarations
   
=============================================================================
===========================================================================*/

typedef struct
{
  const cgps_nv_efs_reg_item_struct * cpz_Table;
  cgps_nv_efs_reg_item_type e_MinItem;
  cgps_nv_efs_reg_item_type e_MaxItem;
} wle_NvEfsItemTableType;

/*==============================================================================

       Functions

==============================================================================*/

/*==============================================================================
  FUNCTION
    wle_nv_efs_reg_get_default

  DESCRIPTION
    Retrieve the specified item and store it in 'dest'. Note that the size of
    'dest' is implicit via the item type. 
 
  RETURN VALUE
    TRUE : If Successful
    FALSE: Otherwise
 
DEPENDENCIES
  
==============================================================================*/
boolean wle_nv_efs_reg_get_default(cgps_nv_efs_reg_item_type const e_item, uint8 u_data_size,
                                    void *p_data_ptr) ;

/*===========================================================================

FUNCTION wle_nv_efs_init

DESCRIPTION
  This function is used to initialize internal WLE-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void wle_nv_efs_init(const wle_NvEfsItemTableType * cpz_NvEfs,
                     const cgps_nv_efs_subsys_type e_SubSystem);

/*===========================================================================

FUNCTION wle_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read NV item. If the read fails, it sets it to
  default value .
  
RETURN VALUE
  TRUE : If read operation is successful
  FALSE: Otherwise
  
DEPENDENCIES
 
===========================================================================*/
boolean wle_nv_efs_reg_item_read(const wle_NvEfsItemTableType * cpz_NvEfs,
                                 cgps_nv_efs_reg_item_type const e_item, 
                                 uint8 u_data_size, void *p_data_ptr);

/*===========================================================================

FUNCTION wle_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 

  
RETURN VALUE
  TRUE : If write operation is successful
  FALSE: Otherwise

  
DEPENDENCIES
 
===========================================================================*/
boolean wle_nv_efs_reg_item_write(const wle_NvEfsItemTableType * cpz_NvEfs,
                                  cgps_nv_efs_reg_item_type const e_item, 
                                  uint8 u_data_size, const void *p_data_ptr);



#ifdef __cplusplus
}
#endif

#endif /* WLE_EFS_H */

