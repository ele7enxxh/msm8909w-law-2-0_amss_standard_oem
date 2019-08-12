#ifndef DSHDR_AN_WMK_H
#define DSHDR_AN_WMK_H

/*===========================================================================
                        D S H D R _ A N _ W M K
                        
GENERAL DESCRIPTION
  This file contains the configuration for the watermarks which go between
  PPP and RLP for the AN authentication module.  Includes the functions called
  at particular watermark events.

EXTERNALIZED FUNCTIONS
  dshdr_an_wmk_init
    Called at startup.  Initializes the watermarks and underlying
    queues.
      
  dshdr_an_wmk_setup_wmks
    Called to setup the watermarks and set various levels, etc.
    
  dshdr_an_wmk_cleanup_wmks
    Cleans up the watermarks at the end of a call.  Frees up any
    remaining DSM items.
  
 INITIALIZATION AND SEQUENCING REQUIREMENTS
   Must call dshdr_an_wmk_init() at startup.

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/dshdr_an_wmk.h#1 $

-----------------------------------------------------------------------------
02/08/11      ls      Global variable cleanup
02/25/03      vas     Created file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "queue.h"
#include "ps_iface.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      GET_DSHDR_AN_WMK_UM_FWD_WMK

DESCRIPTION   Accessor function for dshdr_an_wmk_um_fwd_wmk
  
DEPENDENCIES  None

RETURN VALUE  dshdr_an_wmk_um_fwd_wmk

SIDE EFFECTS  None
===========================================================================*/
dsm_watermark_type* get_dshdr_an_wmk_um_fwd_wmk_ptr 
(
  void
);

/*===========================================================================
FUNCTION      GET_DSHDR_AN_WMK_UM_REV_WMK

DESCRIPTION   Accessor function for dshdr_an_wmk_um_rev_wmk
  
DEPENDENCIES  None

RETURN VALUE  dshdr_an_wmk_um_rev_wmk

SIDE EFFECTS  None
===========================================================================*/
dsm_watermark_type*  get_dshdr_an_wmk_um_rev_wmk_ptr 
(
  void
);

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_wmk_init( void);

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_SETUP_WMKS

DESCRIPTION   This function will initialize the service-configurable HDR AN
              authentication module.

DEPENDENCIES  Hi/Lo Watermark Settings: The Lo and Hi watermark values and
              the don't exceed values for each watermark is inter-dependent
              with the DSM small and large item counts (DSM_LARGE_ITEM_CNT
              and DSM_SMALL_ITEM_CNT) in the DSM module. Any change to lo/hi
              watermark or don't exceed values can cause ERR_FATALs due to
              insufficient memory items available.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_wmk_setup_wmks(void);

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_CLEANUP_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by
              HDR AN authentication module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_wmk_cleanup_wmks(void);

/*===========================================================================
FUNCTION      DSHDR_AN_GET_CURRENT_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in AN UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  uint32

SIDE EFFECTS  None
===========================================================================*/
uint32 dshdr_an_get_current_tx_wmk_cnt(void);

#endif /* DSHDR_AN_WMK_H */
