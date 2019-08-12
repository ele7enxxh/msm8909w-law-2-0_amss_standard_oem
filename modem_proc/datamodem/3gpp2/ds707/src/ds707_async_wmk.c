/*===========================================================================
                         D S 7 0 7 _ A S Y N C _ W M K

DESCRIPTION
  This file contains functions and definitions related to watermarks used
  in IS707 circuit switched data calls (Async/Fax).

EXTERNALIZED FUNCTIONS

ds707_async_wmk_init
  This function initializes watermarks.

ds707_async_wmk_setup_um_wmk
  This function configures the Um watermark.

ds707_async_wmk_setup_sio_wmk
  This function configures the sio watermark.

Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_wmk.c_v   1.5   02 Oct 2002 13:22:50   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_async_wmk.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/11/11    ack    Global Variable Cleanup
11/22/10    sn     Removed REX premption macros as it is not required for 
                   queue init.
02/05/10    ss     Initialize DSM watermark queues with dsm_queue_init ().
09/04/09    ss     CMI SU level modifications.
03/10/08    psng   Fixed off target medium and low lint errors.
09/10/04    vas    QOS related changes
01/21/04    sv     Replaced all references to DEF_MSS with PS707_ASYNC_DEF_MSS.
08/08/03    rsl    Support for DSM 2.0
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
08/15/02    atp    Added support for mode-specific SIO wmks.
07/16/02    aku    changes to support ds_flow_control.h change.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707_ASYNC
#define FEATURE_DSM_WM_CB
#include "ds3gsiolib.h"
#include "ds707_async_defs.h"
#include "ds707_async_wmk.h"
#include "ds707_wmk.h"
#include "ps707_asynci.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "ps_svc.h"
#include "data_msg.h"
#include "data_err_suppr.h"


/*===========================================================================
                             EXTERNS     
===========================================================================*/


/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  SIO Tx watermark (from async to sio)
---------------------------------------------------------------------------*/
static dsm_watermark_type ds707_wmk_async_to_sio_tx_wmk;
static q_type             ds707_wmk_async_to_sio_tx_q;


/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_UM_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the low watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_um_tx_lo_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error fix */
  DATA_MDM_ARG_NOT_CONST( user_data_ptr );
  DATA_MDM_ARG_NOT_CONST( wm_ptr );

  /*-------------------------------------------------------------------------
    Enable flow from SIO.
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_RLP_MASK, DS_FLOW_ENABLE );

} /* ds707_async_wmk_um_tx_lo_wm_func() */


/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the high watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_um_tx_hi_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error fix */
  DATA_MDM_ARG_NOT_CONST( user_data_ptr );
  DATA_MDM_ARG_NOT_CONST( wm_ptr );

  /*-------------------------------------------------------------------------
    Disable flow from SIO.
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_RLP_MASK, DS_FLOW_DISABLE );

} /* ds707_async_wmk_um_tx_hi_wm_func() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_TO_SIO_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the watermark hits the low
              watermark point.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_to_sio_tx_lo_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
  ds707_async_state_type*   ds707_async_state_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error fix */
  DATA_MDM_ARG_NOT_CONST( user_data_ptr );
  DATA_MDM_ARG_NOT_CONST( wm_ptr );

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "Um flow enabled");

  /*-------------------------------------------------------------------------
    Enable flow from Um.
  -------------------------------------------------------------------------*/
  ds707_async_state_ptr = ds707_get_async_state_ptr();
  ds707_async_state_ptr->um_flow_enable     = TRUE;
  ps_send_cmd ( PS_707_ASYNC_UM_FLOW_ENABLE_CMD, NULL );

} /* ds707_async_wmk_to_sio_tx_lo_wm_func() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_TO_SIO_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the watermark hits the high
              watermark point.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_to_sio_tx_hi_wm_func
(
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
)
{
   ds707_async_state_type*   ds707_async_state_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error fix */
  DATA_MDM_ARG_NOT_CONST( user_data_ptr );
  DATA_MDM_ARG_NOT_CONST( wm_ptr );
  
  DATA_IS707_MSG0(MSG_LEGACY_LOW , "Um flow disabled");

  /*-------------------------------------------------------------------------
    Disable flow from Um.
  -------------------------------------------------------------------------*/
  ds707_async_state_ptr = ds707_get_async_state_ptr();
  ds707_async_state_ptr->um_flow_enable     = FALSE;

} /* ds707_async_wmk_to_sio_tx_hi_wm_func() */


/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_queue_init(&ds707_wmk_async_to_sio_tx_wmk, 
                 0x7FFFFFFF,  /* No MAX queue size */
                 &ds707_wmk_async_to_sio_tx_q);

} /* ds707_async_wmk_init() */

/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_UM_WMK

DESCRIPTION   This function initializes the PPP to RLP tx watermark 
              to operate in 707 async mode. This is the same wm used in
              pkt mode. On the RLP rx side there's no wm, there's only a
              queue.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_um_wmk
(
  void
)
{
  dsm_watermark_type *wm_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  wm_ptr = ds707_wmk_get_pri_wm_list();

  /*-------------------------------------------------------------------------
    Setup Um watermark for PPP to RLP SN/1X tx.
  -------------------------------------------------------------------------*/
  wm_ptr->lo_watermark       = 1000;
  wm_ptr->hi_watermark       = 2000;
  wm_ptr->dont_exceed_cnt    = 0xFFFF;

  wm_ptr->lowater_func_ptr = ds707_async_wmk_um_tx_lo_wm_func;
  wm_ptr->hiwater_func_ptr = ds707_async_wmk_um_tx_hi_wm_func;

  wm_ptr->gone_empty_func_ptr   = NULL;   
  //wm_ptr->non_empty_func_ptr    = NULL;
  wm_ptr->each_enqueue_func_ptr = NULL;

} /* ds707_async_setup_um_wmk() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_SIO_WMK

DESCRIPTION   This function initializes the DS to SIO tx watermark 
              to operate in 707 async mode. This is the same wm used in
              pkt mode. On the SIO rx side there is no wm, instead the
              sio rx func is registered with SIO.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_sio_wmk
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup SIO watermark for DS to SIO TX.
  -------------------------------------------------------------------------*/
  ds707_wmk_async_to_sio_tx_wmk.lo_watermark        = PS707_ASYNC_DEF_MSS + 3;
  ds707_wmk_async_to_sio_tx_wmk.hi_watermark        = 2 * PS707_ASYNC_DEF_MSS;
  ds707_wmk_async_to_sio_tx_wmk.dont_exceed_cnt     = 8 * PS707_ASYNC_DEF_MSS;

  /*-------------------------------------------------------------------------
    This WaterMark Queue is to be used for the purposes of SIO
    transmisssion.  
  -------------------------------------------------------------------------*/
  ds707_wmk_async_to_sio_tx_wmk.lowater_func_ptr
                                = ds707_async_wmk_to_sio_tx_lo_wm_func;
  ds707_wmk_async_to_sio_tx_wmk.hiwater_func_ptr
                                = ds707_async_wmk_to_sio_tx_hi_wm_func;

  ds707_wmk_async_to_sio_tx_wmk.gone_empty_func_ptr   = NULL;
  ds707_wmk_async_to_sio_tx_wmk.non_empty_func_ptr    = NULL;
  ds707_wmk_async_to_sio_tx_wmk.each_enqueue_func_ptr = NULL;

} /* ds707_async_wmk_setup_sio_wmk() */

/*===========================================================================
FUNCTION      DS707_GET_ASYNC_TO_SIO_TX_WMK

DESCRIPTION   Accessor function to ds707_wmk_async_to_sio_tx_wmk

DEPENDENCIES  None

RETURN VALUE  Returns a pointer to ds707_wmk_async_to_sio_tx_wmk

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type* ds707_get_async_to_sio_tx_wmk
(
  void
)
{
  return &ds707_wmk_async_to_sio_tx_wmk;
} /* ds707_get_async_to_sio_tx_wmk */

#endif /* FEATURE_DATA_IS707_ASYNC */

