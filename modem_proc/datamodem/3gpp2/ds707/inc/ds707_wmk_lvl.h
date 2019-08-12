#ifndef DS707_WMK_LVL_H
#define DS707_WMK_LVL_H

/*===========================================================================
                          D S 7 0 7 _ W M K _ L E L . H

DESCRIPTION

  This file contains the structure and filepaths needed to retrieve the um and 
  rm watermark level values from the NV. 

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_wmk_lvl.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/13/12    ss      Removed init function as the efs write has been moved to 
                    dsutil file.
06/11/12    msh     Removed write wmks to NV function
11/08/11    msh     Made ds707_wmk_lvl_nv_struct a packed struct  
08/18/11    msh     Initial version 
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Structure to hold the watermark level values that are read from the NV
---------------------------------------------------------------------------*/              
typedef PACKED struct PACKED_POST
{
  uint32 um_fwd_lo_watermark_level;  /* UM  fwd link WM low count */
  uint32 um_fwd_hi_watermark_level;  /* UM  fwd link WM high count */   
  uint32 um_pri_lo_watermark_level;  /* UM  primary rev link WM low count */
  uint32 um_pri_hi_watermark_level;  /* UM  primary rev link WM high count */
  uint32 um_pri_dne_watermark_level;  /* UM  primary rev link WM dne count */
  uint32 rmsm_fwd_lo_watermark_level;  /* RM  fwd link WM low count */
  uint32 rmsm_fwd_hi_watermark_level;  /* RM  fwd link WM high count */
  uint32 rmsm_fwd_dne_watermark_level;  /* RM  fwd link WM dne count */
  uint32 rmsm_rev_lo_watermark_level;  /* RM  rev link WM low count */
  uint32 rmsm_rev_hi_watermark_level;  /* RM  rev link WM high count */
  uint32 rmsm_rev_dne_watermark_level;  /* RM  rev link WM dne count */
} ds707_wmk_lvl_nv_struct;     

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
);
#endif /* DS707_WMK_LVL_H */

