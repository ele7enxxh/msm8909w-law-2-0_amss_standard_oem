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

Copyright (c) 2003-2011 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/dshdr_an_wmk.c#1 $

Imaginary Buffer Line
when       who        what, where, why
--------   ---        ------------------------------------------------------- 
05/13/11   fjia       Migrated to MSG 2.0 macros 
05/12/11   dvk        Fixed Compiler issue 
02/08/11   ls         Global variable cleanup
11/22/10   sn         Removed REX premption macros as it is not required.
02/05/10   ss         Remove q_init for the watermark queue as it is already 
                      initialized inside dsm_queue_init.
01/28/10   ls         Add Queue initializations
12/15/05   TMR        Changed RLP RX queue to watermark and added parameter to 
                      dsrlp_reg_srvc() and RX callback function
09/29/03   dna        Get T_QUASAR to compile and work again for MSM6500
02/25/03   vas        Created file
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#define FEATURE_DSM_WM_CB
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dshdr_an_wmk.h"
#include "ds_flow_control.h"
#include "ps_iface.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "dshdr_an_mgr.h"
#include "data_msg.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  RLP FWD link queue's
---------------------------------------------------------------------------*/
static q_type             dshdr_an_wmk_um_fwd_wmk_q;    /* rlp fwd link queue     */
static dsm_watermark_type dshdr_an_wmk_um_fwd_wmk;

/*---------------------------------------------------------------------------
  RLP REV link watermark (from PPP to RLP) (AN/HDR Link)
---------------------------------------------------------------------------*/
static dsm_watermark_type dshdr_an_wmk_um_rev_wmk; 
static q_type             dshdr_an_wmk_um_rev_q;


/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/

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
)
{
  return &(dshdr_an_wmk_um_rev_wmk);
} /* get_dshdr_an_wmk_um_rev_wmk */

/*===========================================================================
FUNCTION      GET_DSHDR_AN_WMK_UM_FWD_WMK

DESCRIPTION   Accessor function for dshdr_an_wmk_um_fwd_wmk
  
DEPENDENCIES  None

RETURN VALUE  dshdr_an_wmk_um_fwd_wmk

SIDE EFFECTS  None
===========================================================================*/
dsm_watermark_type*  get_dshdr_an_wmk_um_fwd_wmk_ptr 
(
  void
)
{
  return &(dshdr_an_wmk_um_fwd_wmk);
} /* get_dshdr_an_wmk_um_fwd_wmk */

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_UM_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the low watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_wmki_um_tx_lo_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  #ifdef FEATURE_HDR
  ps_iface_type    *iface_ptr;
  #endif /* FEATURE_HDR */

  DATA_HDR_MSG2(MSG_LEGACY_LOW, 
                "dshdr_an_wmki_um_tx_lo_wm_func() - low watermark "
                "with wmptr:0x%x "
                "user_data:0x%x",
                wm_ptr,
                user_data_ptr);

  #ifdef FEATURE_HDR
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Enable the flow on this interface since the TX wm is now below the lower
    limit
  -------------------------------------------------------------------------*/
  iface_ptr = dshdr_an_get_iface_ptr();
  ps_iface_enable_flow(iface_ptr, DS_FLOW_HDRAN_RLP_MASK);
  #endif /* FEATURE_HDR */
} /* dshdr_an_wmki_um_tx_lo_wm_func() */

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_UM_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the high watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_wmki_um_tx_hi_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  #ifdef FEATURE_HDR
  ps_iface_type    *iface_ptr;
  #endif /* FEATURE_HDR */

  DATA_HDR_MSG2(MSG_LEGACY_LOW, 
                "dshdr_an_wmki_um_tx_hi_wm_func() - high watermark "
                "with wmptr:0x%x "
                "user_data:0x%x",
                wm_ptr,
                user_data_ptr);

  #ifdef FEATURE_HDR
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Disable the flow on this interface since the TX wm reached the lo limit
  -------------------------------------------------------------------------*/
  iface_ptr = dshdr_an_get_iface_ptr();
  ps_iface_disable_flow(iface_ptr, DS_FLOW_HDRAN_RLP_MASK);
  #endif /* FEATURE_HDR */
} /* dshdr_an_wmki_um_tx_hi_wm_func() */

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DSHDR_AN_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_wmk_init( void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Mutual exclusion is not required here and so removed premption.
  -------------------------------------------------------------------------*/
  dsm_queue_init (&dshdr_an_wmk_um_fwd_wmk,
                  (int)0x7FFFFFFF,
                  &dshdr_an_wmk_um_fwd_wmk_q);

  dsm_queue_init ( &dshdr_an_wmk_um_rev_wmk,
                   (int)0x7FFFFFFF,
                   &dshdr_an_wmk_um_rev_q );
} /* dshdr_an_wmk_init() */


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
void dshdr_an_wmk_setup_wmks(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Intialize PPP -> RLP AN watermark
  -------------------------------------------------------------------------*/
  dshdr_an_wmk_um_rev_wmk.lo_watermark       = 1000;
  dshdr_an_wmk_um_rev_wmk.hi_watermark       = 2000;
  dshdr_an_wmk_um_rev_wmk.dont_exceed_cnt    = 0xFFFF;
  /*lint -save -e64 suppress error 64*/
  dshdr_an_wmk_um_rev_wmk.lowater_func_ptr   = dshdr_an_wmki_um_tx_lo_wm_func;
  dshdr_an_wmk_um_rev_wmk.hiwater_func_ptr   = dshdr_an_wmki_um_tx_hi_wm_func;
  /*lint -restore suppress error 64*/
  dshdr_an_wmk_um_rev_wmk.non_empty_func_ptr    = NULL;
  dshdr_an_wmk_um_rev_wmk.gone_empty_func_ptr   = NULL;
  dshdr_an_wmk_um_rev_wmk.each_enqueue_func_ptr  = NULL;

} /* dshdr_an_wmk_setup_wmks() */

/*===========================================================================
FUNCTION      DSHDR_AN_WMK_CLEANUP_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by
              HDR AN authentication module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_wmk_cleanup_wmks(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Empty AN watermarks (Fwd Link)
  -------------------------------------------------------------------------*/
  dsm_empty_queue(&dshdr_an_wmk_um_rev_wmk);

  /*-------------------------------------------------------------------------
    Empty AN queues (Rev Link)
  -------------------------------------------------------------------------*/
  dsm_empty_queue (&dshdr_an_wmk_um_fwd_wmk);

} /* dshdr_an_wmk_cleanup_wmks() */

/*===========================================================================
FUNCTION      DSHDR_AN_GET_CURRENT_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in AN UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  uint32

SIDE EFFECTS  None
===========================================================================*/
uint32 dshdr_an_get_current_tx_wmk_cnt(void)
{
  DATA_HDR_MSG1(MSG_LEGACY_MED, 
                "dshdr_an_get_current_tx_wmk_cnt: %d",
                dshdr_an_wmk_um_rev_wmk.current_cnt);
  return dshdr_an_wmk_um_rev_wmk.current_cnt;
}
