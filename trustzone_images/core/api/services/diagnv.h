#ifndef DIAGNV_H
#define DIAGNV_H
/*==========================================================================
  
General Description
 This module contains prototypes for diag_nv_read & diag_nv_write.
                
Copyright (c) 2001-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/diagnv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   JV      Moved definition of diag_nv_cache_type and extern diagnv
                   from diagi.h to here.
07/15/09   mad     Removed unnecessary includes:diagcmd.h,log_codes.h
07/10/09   as      Mainlined code under FEATURE_MEID_SUPPORT
05/07/09   vk      removed dependencies from diagbuf_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/04/05   as     Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   as     Replaced FEATURE_NV_MEID_ENABLE with
                  FEATURE_DIAG_MEID_ENABLE.
03/14/05   as     Added prototype for diagnv_get_meid.
11/11/01   as     Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "diagpkt.h" /* prldiag.c dependency */
#include "nv.h"

 #define DIAG_MEID_ENABLE

/*===========================================================================

FUNCTION DIAG_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_read ( 
  nv_items_enum_type item,              /* Which NV item to read */
  PACK(void *) buf_ptr           /* Where to put the data */
);

/*===========================================================================

FUNCTION DIAG_NV_WRITE
DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_write ( 
  nv_items_enum_type item,
  PACK(void *) buf_ptr
);



/*===========================================================================

FUNCTION DIAGNV_GET_ESN

DESCRIPTION
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
dword diagnv_get_esn  (void);
/*=========================================================================*/

/* Structure to cache NV data in memory */
typedef struct
{
  dword esn;            /* ESN */

  byte current_nam;     /* current nam index (0-3) */
  dword min1[NV_MAX_NAMS][NV_MAX_MINS]; /* All the MIN1s */
  word min2[NV_MAX_NAMS][NV_MAX_MINS];  /* All the MIN2s */
}
diag_nv_cache_type;

extern diag_nv_cache_type diagnv;
extern nv_item_type nitem;  /* NV Item buffer for NV interface */

#endif /* DIAGNV_H*/

