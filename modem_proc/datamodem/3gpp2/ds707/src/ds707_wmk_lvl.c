/*===========================================================================

                         D S 7 0 7 _ W M K _ L V L
GENERAL DESCRIPTION
 This file contains the configuration of the UM and RMSM wmk levels.

Copyright (c) 2002 - 2012 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk_lvl.c_v   1.2   19 Nov 2002 19:23:50     $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_wmk_lvl.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
06/30/14   niv        Added wrapper functions & API change for 
                      NV_REFRESH 
08/13/12   ss         Moving efs write operation to dsutil performed during 
                      bootup.
05/25/12   msh        Remove write wmk values to NV 
08/31/11   ms         Initial version  
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707 
#ifdef FEATURE_DATA_WMK_CONFIG
  #include "ds707_wmk_lvl.h"
  #include "dsutil.h"
  #include "data_msg.h"
  #include "msg.h"
  #include "nv.h"
  #include "ds3gcfgmgr.h"

      
/*----------------------------------------------------------------------------
  Define macros for filename that contain the watermark values 
----------------------------------------------------------------------------*/
#define DS707_NV_CONF_FILE_PATH \
  "/nv/item_files/conf/data_config_info.conf"

#define DS707_WMK_LVL_NV_FILENAME \
  "/nv/item_files/data/3gpp2/um_rm_watermark_parameters"

/*===========================================================================
FUNCTION      DS707_WMK_LVL_READ_WMKS_NV

DESCRIPTION   Called from rm and um side to read the NV item

DEPENDENCIES  None.
 
PARAMETERS   *data_ptr : Pointer where nv item is copied
             item_size: Size of structure of nv item
 
RETURN VALUE  0 if read successful 
             -1 for failure 

SIDE EFFECTS  None.
===========================================================================*/
int ds707_wmk_lvl_read_wmks_nv
(
  void               *data_ptr,
  uint32             item_size
)
{ 
  int ret_val;
  ret_val = ds3gcfgmgr_read_efs_nv_ex( DS707_WMK_LVL_NV_FILENAME, data_ptr, item_size, ds707_curr_subs_id() );
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "wmk_lvl efs read returned %d", ret_val);
  return ret_val;
}
#endif /*FEATURE_DATA_WMK_CONFIG*/
#endif /* FEATURE_DATA_IS707 */

