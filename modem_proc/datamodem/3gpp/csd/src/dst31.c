/*=============================================================================
**
**                        D A T A   S E R V I C E S
**
**           G S M  C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                     T 3 1  F A X  S E R V I C E S
**
**
** DESCRIPTION:
**
**   This file contains source code comprising the T31 Fax Services layer
**   of the GCSD Fax Protocol Stack.  This layer implements the ITU-T T.31
**   Recommendation: Asynchronous Facsimile DCE Control - Service Class 1.
**
** EXTERNALIZED FUNCTIONS:
**
**    dst31_at_cmd_abort()
**      - Invoked via the FPS layer by ATCOP when a abort command sequence 
**        is detected.  This allows T31 to abort +FRM, +FRH, and +FRS actions.
**
**    dst31_call_connect_init()
**      - Invoked via the FPS layer to perform final call initialization 
**        when a fax data call has reached the 'CALL_CONNECTED' state.
**
**    dst31_call_init()
**      - Invoked via the FPS layer to perform initial call initialization 
**        when a fax data call has reached the 'CALL_CONNECTED' state.
**
**    dst31_powerup_init()
**      - Invoked via the FPS layer by GCSDPS to initialize the T31 services 
**        layer at powerup.
**
**    dst31_process_downlink()
**      - Invoked via the FPS layer by GCSDPS to process data received OTA from
**        the TPS on the downlink.  The received data is then transmitted to 
**        the local DTE.
**
**    dst31_process_uplink()
**      - Invoked via the FPS layer by GCSDPS to process data received from the
**        local DTE to be transmitted on the uplink to the remote facsimile 
**        device.
**
**    dst31_process_event()
**      - Invoked via the FPS layer by GCSDPS to perform specific T31 event 
**        processing (e.g. T.31 AT command processing, call event processing).
**
**    dst31_process_signals()
**      - Invoked via the FPS layer by GCSDPS to process T31 signals (e.g.
**        protocol timeouts).
**
** EXTERNALIZED DATA:
**
**
** INITIALIZATION AND SEQUENCING REQUIREMENTS:
**
**   dst31_powerup_init() must be called at powerup.
**
** DOCUMENTATION:
**
**   GSM Circuit-Switched Fax Services, Volumes 1 & 2
**      80-V3512-1, 80-V3512-2
**    
**   ITU-T Recommendation T.31: Asynchronous Facsimile DCE Control -
**      Service Class 1.
**

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Copyright (c) 2002 - 2014 Qualcomm Technologies Incorporated.
   All Rights Reserved
   Qualcomm Confidential and Proprietary.

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31.c#1 $

when      who   what, where, why
--------  ---   --------------------------------------------------------------
07/15/11  RP    QShrink2 changes: MSG_SPRINTF is used instead of MSG buffers.
04/01/11  TTV   Support for Q6 free floating.
06/29/09  BS    Fixed off-target lint medium & lows.
03/09/09  SA    AU level CMI modifications.
07/11/08  SA    Replaced __inline with INLINE for all targets.
05/26/08  SA    Send ERROR to any pending AT command during call end. 
11/15/07  AR    Use safety macros for watermark operations.
02/26/07  AR    Replace banned APIs with safer routines.
08/21/06  AR    Changes to support multi-processor architectures
03/01/06  AR    Add CM call ID to control block structure.
02/09/06  AR    Lint corrections.
10/10/05  AR    Code review issue correction.
07/22/05  AR    Improve invalid frame handling in dst31i_build_dl_hdlc_frame()
06/07/04  RLP   Modified to terminate the session if the time it takes to setup 
                  the T.30 session takes too long and the local fax appl aborts 
                  an implied FTH/FRH command.  In this case the fax appl will 
                  not send any further AT commands and this was causing the 
                  downlink watermark to fill up.
                Changed some debug messages from MSG_LOW to MSG_HIGH to
                  aid in field testing.
                LINT cleanup.
05/30/04  AR    Lint corrections.
09/05/03  RLP   Changed the timing of when the final AT response is sent to 
                  the local DTE.
08/05/03  TMR   Updated watermark callbacks for DSM 2
06/04/03  RLP   Made zbuf static in dst31i_dump_dsm_item() due to change in
                debug macros.
04/25/03  TMR   Changes for new UMTS CS data architecture
03/19/03  RLP   GCF certification changes to fix MT fax issue.
02/10/03  RLP   Spain IOT changes.
                  - calculate CRC before DLE insertion for +FRH.
                  - Disabled debug msgs from watermark callbacks.
                  - Changed DL DLE filtering to use normal DLE chars and
                    not the reversed equivalents due to change in logic.
01/14/03  RLP   Combined +FTH/+FTM SIO/TPS watermark sizings so now the UL
                  watermarks uses the sames settings for both states.
                Code review updates.
12/20/02  RLP   Changed SIO flow control to use new MM functions.
                Removed TREX logic from dst31i_dump_dsm_item (not needed).
                Debug message modifications.
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
10/09/02  TMR   Updated for new Multi-mode architecture (FEATURE_DATA_MM)
09/11/02  RLP   MMO2 IOT changes.
07/18/02  RLP   Additional changes to maintain TREX compatibilty.
                  - change dst31_process_uplink() to not perform any
                    processing if TPS flow control is enabled.
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/26/02  RLP   Added ERR messages to log when a session is terminated and why.
06/25/02  RLP   Changed a debug message format for clarity.
06/25/02  RLP   Added dst31i_dsm_pushdown wrapper functions and replaced calls
                  to dsm_pushdown with the new wrappers.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#ifdef FEATURE_DATA_GCSD_FAX
#include "amssassert.h"

#include <stringl/stringl.h>
#include "stdlib.h"

#include "rex.h"
#include "err.h"
#include "msg.h"
#include "crc.h"
#include "dsm.h"
#include "dstask_v.h"
#include "dsati.h"
#include "dsgcsdi.h"
#include "dsucsdi.h"
#include "dsucsdrlp.h"
#include "ds3gsiolib_ex.h"
#include "ds3gsiolib.h"

#include "dsgcsdfa.h"
#include "dsfps.h"
#include "dst31.h"
#include "dst31i.h"
#include "dst31hdlci.h"


#if 0 /* Not being used at this time */
#define DST31_ENABLE_SIO_TX_FLUSH
#endif

/* DSM pullup() error code */
#define DST31_DSM_PULLUP_ERROR  (-1)

/*=============================================================================
** REGIONAL FUNCTION PROTOTYPES
**===========================================================================*/

/*=============================================================================
** REGIONAL VARIABLES
**============================================================================*/
/* T31 State Event Handler Table */
LOCAL t31_state_handler_tbl_struct_type t31_state_tbl[DST31_MAX_STATES] =
{ 
  /* 
  ** Start-Of-Procedure handler         Main state handler 
  */
  { NULL, /* Handler not required */    NULL /* Handler not required */ },
  { NULL, /* Handler not required */    dst31i_process_sync_state },
  { NULL, /* called directly */         dst31i_process_wait_for_order_state },
  { dst31i_sop_fth_state,               dst31i_process_fth_state },
  { dst31i_sop_frh_state,               dst31i_process_frh_state },
  { dst31i_sop_ftm_state,               dst31i_process_ftm_state },
  { dst31i_sop_frm_state,               dst31i_process_frm_state },
  { dst31i_sop_fts_state,               dst31i_process_fts_state },
  { dst31i_sop_frs_state,               dst31i_process_frs_state }
};

LOCAL char *dsat_result_code_names[] =
{
  "DSAT_OK",
  "DSAT_CONNECT",
  "DSAT_RING",
  "DSAT_NO_CARRIER",
  "DSAT_ERROR",
  "DSAT_NO_DIALTONE",
  "DSAT_BUSY",
  "DSAT_NO_ANSWER"
};

/*=============================================================================
** EXPORTED VARIABLES FOR USE SOLEY BY T31
**===========================================================================*/

/* T31 Control Block */
static dst31_ctrl_blk_struct_type  t31_control_blk = {{NULL}};


/*=============================================================================
** REGIONAL FUNCTIONS
**===========================================================================*/

#ifdef DST31_ENABLE_SIO_TX_FLUSH

/*=============================================================================
** FUNCTION: dst31i_sio_flush_tx_cb
**
** DESCRIPTION:
**   This function is a callback used by SIO to indicate T31 whenever the SIO
**   TX queue has been flushed.  The queue needs to be flushed whenever a
**   DSAT result code is sent to the local TE to make sure it gets there
**   asap.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
LOCAL void dst31i_sio_flush_tx_cb
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if( t31_ctrl_blk->sio_dl_wm_ptr )
  {
    if (t31_ctrl_blk->sio_dl_wm_ptr->current_cnt == 0)
    {
      t31_ctrl_blk->flags.wait_sio_flush_tx = FALSE;

      DATA_MSG0_HIGH("SIO TX flush completed");
    }
    else
    {
      if (ds3g_siolib_ex_flush_tx(dst31i_sio_flush_tx_cb, 
                                  ds3g_siolib_ex_get_csd_port())
                                  != DS3G_SIOLIB_OK)

        {
          DATA_MSG0_ERROR("Error flushing SIO TX queue");
        }

      DATA_MSG1_HIGH("SIO data still pending, flushing %d bytes",
                   t31_ctrl_blk->sio_dl_wm_ptr->current_cnt);
    }
  }
} /* dst31i_sio_flush_tx_cb */
#endif /* DST31_ENABLE_SIO_TX_FLUSH */


/*=============================================================================
** FUNCTION: dst31i_sio_ul_high_wm_handler
**
** DESCRIPTION:
**   This function is used to assert flow-control back to the local DTE to
**   allow the protocol stack to catch up with getting the current queued
**   data transmitted OTA.
**
**   Note that flow-control is not a desired element within a fax data call
**   but none-the-less is required and supported.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
/*ARGSUSED*/
LOCAL void dst31i_sio_ul_high_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if (t31_ctrl_blk->flags.sio_flowctl_asserted == FALSE)
  {
    /* We need to know when flow-control is ASSERTED */
    t31_ctrl_blk->flags.sio_flowctl_asserted = TRUE;

    /* Tell SIO to go ahead and do it */
    ds3g_siolib_ex_set_inbound_flow(DS_FLOW_GCSD_MASK, 
                                    DS_FLOW_DISABLE,
                                    ds3g_siolib_ex_get_csd_port());

    DATA_MSG0_MED( "SIO-UL:Flow-Control ASSERTED");
    if( t31_ctrl_blk->sio_ul_wm_ptr )
    {
      T31_MSG_MED("SIO-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                  t31_ctrl_blk->sio_ul_wm_ptr->current_cnt,
                  t31_ctrl_blk->sio_ul_wm_ptr->hi_watermark,
                  t31_ctrl_blk->sio_ul_wm_ptr->lo_watermark);
    }
  }
} /* dst31i_sio_ul_high_wm_handler */


/*=============================================================================
** FUNCTION: dst31i_sio_ul_low_wm_handler
**
** DESCRIPTION:
**   This function is used to de-assert flow-control back to the local DTE
**   to allow it to once again start sending data to the protocol stack.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
/*ARGSUSED*/
LOCAL void dst31i_sio_ul_low_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if (!DST31_SIO_MODE_CHANGE_PENDING() &&
      (t31_ctrl_blk->flags.sio_flowctl_asserted == TRUE))
  {
    /* We need to know when flow-control is DEASSERTED */
    t31_ctrl_blk->flags.sio_flowctl_asserted = FALSE;

    /* Tell SIO to go ahead and do it */
    ds3g_siolib_ex_set_inbound_flow(DS_FLOW_GCSD_MASK, 
                                    DS_FLOW_ENABLE,
                                    ds3g_siolib_ex_get_csd_port());

    DATA_MSG0_MED( "SIO-UL:Flow-Control DEASSERTED");
    if( t31_ctrl_blk->sio_ul_wm_ptr )
    {
      T31_MSG_MED("SIO-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                  t31_ctrl_blk->sio_ul_wm_ptr->current_cnt,
                  t31_ctrl_blk->sio_ul_wm_ptr->hi_watermark,
                  t31_ctrl_blk->sio_ul_wm_ptr->lo_watermark);
    }
  }
} /* dst31i_sio_ul_low_wm_handler */



/*=============================================================================
** FUNCTION: dst31i_tps_ul_high_wm_handler
**
** DESCRIPTION:
**   This function is used to assert flow-control to the Transport Protocol
**   Stack. In this case, the TPS can not process the data to be sent out
**   OTA fast enough.  Flow-control is asserted so the T31 will not put any 
**   additional data on the watermark until the TPS has had time to catch up.
**
**   Note that flow-control is not a desired element within a fax data call
**   but none-the-less is required and supported.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   Since T31 is not putting any data on the TPS watermark, the SIO watermark
**   will start filling up.  This may cause SIO flow-control to be asserted
**   back to the DTE.
**===========================================================================*/
/*ARGSUSED*/
LOCAL void dst31i_tps_ul_high_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if (t31_ctrl_blk->flags.tps_flowctl_asserted == FALSE)
  {
    t31_ctrl_blk->flags.tps_flowctl_asserted = TRUE;

    DATA_MSG0_MED("TPS-UL:Flow-Control ASSERTED");
    if( t31_ctrl_blk->tps_ul_wm_ptr )
    {
      T31_MSG_MED("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                  t31_ctrl_blk->tps_ul_wm_ptr->current_cnt,
                  t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark,
                  t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark);
    }
  }
} /* dst31i_tps_ul_high_wm_handler */


/*=============================================================================
** FUNCTION: dst31i_tps_ul_low_wm_handler
**
** DESCRIPTION:
**   This function is used to de-assert flow-control to the Transport Protocol
**   Stack.  In this case, the TPS has managed to catch up on the pending
**   data and T31 can begin to place data on the watermark for OTA delivery.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   If SIO flow-control was asserted as a side-effect of asserting TPS flow-
**   control, it will eventually be de-asserted as well when T31 starts pulling
**   data off the SIO watermark.
**===========================================================================*/
/*ARGSUSED*/
LOCAL void dst31i_tps_ul_low_wm_handler
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if (t31_ctrl_blk->flags.tps_flowctl_asserted == TRUE)
  {
    t31_ctrl_blk->flags.tps_flowctl_asserted = FALSE;

    DATA_MSG0_MED("TPS-UL:Flow-Control DEASSERTED");
    if( t31_ctrl_blk->tps_ul_wm_ptr )
    {
      T31_MSG_MED("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                  t31_ctrl_blk->tps_ul_wm_ptr->current_cnt,
                  t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark,
                  t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark);
    }
  }
} /* dst31i_tps_ul_low_wm_handler */


/*=============================================================================
** FUNCTION: dst31_sio_watermark_init
**
** DESCRIPTION:
**   This function is used to initialize the DSM watermarks used by the FPS 
**   to exchange facsimile data with SIO.  This initialization is only 
**   required at powerup.  Initialization includes setting appropriate 
**   threshold values and any hi/lo watermark handler functions used for
**   flow-control.
**
**   Note that GCSDPS actually "owns" these watermarks so T31 just sets local
**   pointers to them.  
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
void dst31i_sio_watermark_init
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Initialize SIO Downlink watermark */
  if( t31_ctrl_blk->sio_dl_wm_ptr )
  {
    t31_ctrl_blk->sio_dl_wm_ptr->lo_watermark        = 
      DST31_SIO_DL_LO_WM_THRESHOLD;
    t31_ctrl_blk->sio_dl_wm_ptr->hi_watermark        = 
      DST31_SIO_DL_HI_WM_THRESHOLD;
    t31_ctrl_blk->sio_dl_wm_ptr->dont_exceed_cnt     = 
      DST31_SIO_DL_DONT_EXCEED_WM_THRESHOLD;
     
     /* Initialize SIO Downlink watermark */
    t31_ctrl_blk->sio_dl_wm_ptr->gone_empty_func_ptr = NULL;
    t31_ctrl_blk->sio_dl_wm_ptr->non_empty_func_ptr  = NULL;
    t31_ctrl_blk->sio_dl_wm_ptr->lowater_func_ptr    = NULL;
    t31_ctrl_blk->sio_dl_wm_ptr->hiwater_func_ptr    = NULL;
  }
  
  /* Initialize SIO Uplink watermark */
  if( t31_ctrl_blk->sio_ul_wm_ptr )
  {
    t31_ctrl_blk->sio_ul_wm_ptr->lo_watermark        = 
      DST31_SIO_UL_LO_WM_THRESHOLD;
    t31_ctrl_blk->sio_ul_wm_ptr->hi_watermark        = 
      DST31_SIO_UL_HI_WM_THRESHOLD;
    t31_ctrl_blk->sio_ul_wm_ptr->dont_exceed_cnt     = 
      DST31_SIO_UL_DONT_EXCEED_WM_THRESHOLD;

    t31_ctrl_blk->sio_ul_wm_ptr->gone_empty_func_ptr = NULL;
    t31_ctrl_blk->sio_ul_wm_ptr->non_empty_func_ptr  = NULL;
    t31_ctrl_blk->sio_ul_wm_ptr->lowater_func_ptr    =
      dst31i_sio_ul_low_wm_handler;
    t31_ctrl_blk->sio_ul_wm_ptr->hiwater_func_ptr    =
      dst31i_sio_ul_high_wm_handler;
  }

  DATA_MSG2_LOW("SIO-TX-WM@=0x%x SIO-RX-WM@=0x%x",
    t31_ctrl_blk->sio_dl_wm_ptr, t31_ctrl_blk->sio_ul_wm_ptr);
    
  if( t31_ctrl_blk->sio_dl_wm_ptr && t31_ctrl_blk->sio_ul_wm_ptr )
  {
    DATA_MSG2_LOW("SIO-TX-WMQ@=0x%x SIO-RX-WMQ@=0x%x",
      t31_ctrl_blk->sio_dl_wm_ptr->q_ptr, t31_ctrl_blk->sio_ul_wm_ptr->q_ptr);
  }

  DATA_MSG0_HIGH( "SIO DSM Watermarks initialized!");
} /* dst31i_sio_watermark_init */


/*=============================================================================
** FUNCTION: dst31i_tps_watermark_init
**
** DESCRIPTION:
**   This function is used to initialize the DSM watermarks used by the FPS 
**   to exchange facsimile data with TPS.  This initialization is performed
**   each time a facsimile call is start as these watermarks are shared with
**   async data services.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None      
**===========================================================================*/
LOCAL void dst31i_tps_watermark_init
(
   void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Initialize TPS Downlink watermark */
  if( t31_ctrl_blk->tps_dl_wm_ptr )
  {
    t31_ctrl_blk->tps_dl_wm_ptr->lo_watermark        = 
      DST31_TPS_DL_LO_WM_THRESHOLD;
    t31_ctrl_blk->tps_dl_wm_ptr->hi_watermark        = 
      DST31_TPS_DL_HI_WM_THRESHOLD;
    t31_ctrl_blk->tps_dl_wm_ptr->dont_exceed_cnt     = 
      DST31_TPS_DL_DONT_EXCEED_WM_THRESHOLD;

    /* Initialize TPS Downlink watermark */
    t31_ctrl_blk->tps_dl_wm_ptr->gone_empty_func_ptr = NULL;
    t31_ctrl_blk->tps_dl_wm_ptr->non_empty_func_ptr  = NULL;
    t31_ctrl_blk->tps_dl_wm_ptr->lowater_func_ptr    = NULL;
    t31_ctrl_blk->tps_dl_wm_ptr->hiwater_func_ptr    = NULL;
  }

  /* Initialize TPS Uplink watermark */
  if( t31_ctrl_blk->tps_ul_wm_ptr )
  {
    t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark        = 
      DST31_TPS_UL_LO_WM_THRESHOLD;
    t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark        = 
      DST31_TPS_UL_HI_WM_THRESHOLD;
    t31_ctrl_blk->tps_ul_wm_ptr->dont_exceed_cnt     = 
      DST31_TPS_UL_DONT_EXCEED_WM_THRESHOLD;

    t31_ctrl_blk->tps_ul_wm_ptr->gone_empty_func_ptr = NULL;
    t31_ctrl_blk->tps_ul_wm_ptr->non_empty_func_ptr  = NULL;
    t31_ctrl_blk->tps_ul_wm_ptr->lowater_func_ptr    =
      dst31i_tps_ul_low_wm_handler;
    t31_ctrl_blk->tps_ul_wm_ptr->hiwater_func_ptr    =
      dst31i_tps_ul_high_wm_handler;
  }

  DATA_MSG2_LOW( "TPS-DL@=0x%x TPS-UL@=0x%x",
    t31_ctrl_blk->tps_dl_wm_ptr, t31_ctrl_blk->tps_ul_wm_ptr);
    
  if( t31_ctrl_blk->tps_dl_wm_ptr && t31_ctrl_blk->tps_ul_wm_ptr )
  {
    DATA_MSG2_LOW("TPS-DLQ@=0x%x TPS-ULQ@=0x%x",
      t31_ctrl_blk->tps_dl_wm_ptr->q_ptr, t31_ctrl_blk->tps_ul_wm_ptr->q_ptr);
  }
  
  DATA_MSG0_HIGH("TPS DSM Watermarks initialized!");
} /* dst31i_tps_watermark_init */




/*=============================================================================
** FUNCTION: dst31i_context_reset
**
** DESCRIPTION:
**   This function is used to reset the T31 processing context whenever a
**   fax data call terminates. (i.e. upon receipt of DS_GCSD_DISCONNECT_MT_REQ/
**   DS_GCSD_DISCONNECT_MO_REQ). All timers are cleared, watermarks are emptied,
**   buffers are cleared, FA is returned to IDLE, T31 state is set to IDLE.
**
** PARAMETERS:
**   end_of_call == boolean set to TRUE if the function is being called at
**     at the end of a call, FALSE if at the beginning of a call.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
LOCAL void dst31i_context_reset
(
  boolean end_of_call
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_HIGH("Resetting T31 context");

  /* Make sure we start out in the correct state */
  t31_ctrl_blk->last_state = DST31_IDLE_STATE;
  t31_ctrl_blk->curr_state = DST31_IDLE_STATE;

  /* This flag is used to let T31 know that we actually processed
  ** a disconnect.  If for some reason never make it through this
  ** function at the end of a call, we'll catch it on the start
  ** of the next call and clean up then.
  */
  t31_ctrl_blk->flags.t31_init = FALSE;

  /* Reset the timer */
  DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);
  t31_ctrl_blk->flags.timeout = FALSE;

  /* Force the FA back to its idle state */
  (void) dsgcsdfa_return_to_idle(TRUE);

  /* Send ERROR if any pending command is in progress */
  if ( (t31_ctrl_blk->dsat_callback) && (end_of_call == TRUE) )
  {
    DATA_MSG0_HIGH("FINAL AT result DSAT_ERROR for pending cmd's");
    t31_ctrl_blk->dsat_callback(DSAT_ERROR);
  }
  t31_ctrl_blk->dsat_callback = NULL;

  /* Reset FCLASS if allowed by the protocol */
  if (t31_ctrl_blk->flags.allow_fclass_0)
  {
    /* Back to a normal data modem */
    DSATUTIL_SET_VAL(DSAT_EXT_FCLASS_IDX,0,0,0,0,NUM_TYPE)
  }

  /* Clear all control flags */
  memset(&t31_ctrl_blk->flags, FALSE, sizeof(t31_ctrl_blk->flags));

  /* Make sure the watermarks are empty */
  EMPTY_WATERMARK( t31_ctrl_blk->sio_ul_wm_ptr );
  EMPTY_WATERMARK( t31_ctrl_blk->sio_dl_wm_ptr );
  EMPTY_WATERMARK( t31_ctrl_blk->tps_ul_wm_ptr );
  EMPTY_WATERMARK( t31_ctrl_blk->tps_dl_wm_ptr );

  /* Toss any pending data */
  dsm_free_packet(&t31_ctrl_blk->pending_ul_data_ptr);
  dsm_free_packet(&t31_ctrl_blk->pending_dl_data_ptr);

  /* Data mode is set back to command mode */
  DST31_SAVE_SIO_PROCESSING_MODE(DST31_SIO_CMD_MODE);

  /* The following only applies if this is the end of a call */
  if (end_of_call == TRUE)
  {
    /* Print out the total TX/RX bytes counts */
    DATA_MSG5(MSG_LEGACY_HIGH,"Total Fax HDLC TX bytes=%d, Total Fax HDLC RX bytes=%d,\
                               Total Fax MSG  TX bytes=%d, Total Fax MSG  RX bytes=%d,\
                               Total Fax Pages=%d", t31_ctrl_blk->byte_cnts.total_tx_hdlc_bytes,\
                               t31_ctrl_blk->byte_cnts.total_rx_hdlc_bytes,\
                               t31_ctrl_blk->byte_cnts.total_tx_msg_bytes,\
                               t31_ctrl_blk->byte_cnts.total_rx_msg_bytes,\
                               t31_ctrl_blk->byte_cnts.page_cnt-1);

    DATA_MSG0_HIGH("Session Complete");
  }

  /* Now reset the counters */
  memset(&t31_ctrl_blk->byte_cnts, 0, sizeof(t31_ctrl_blk->byte_cnts));
} /* dst31i_context_reset */


/*=============================================================================
** FUNCTION: dst31i_send_ds_cmd
**
** DESCRIPTION:
**   This function is used to send the specified command to the DS Task.
**
** PARAMETERS:
**   ds_cmd == identifies the DS command to send.
**
** RETURNS:
**   boolean == TRUE if command successfully sent FALSE otherwise.
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
LOCAL boolean dst31i_send_ds_cmd
(
  ds_cmd_enum_type         ds_cmd
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk = NULL;
  ds_cmd_type                 *cmd_ptr = NULL;
  cm_call_id_type             *call_id_ptr = NULL;
  boolean                      ret_val = FALSE;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  cmd_ptr = ds_allocate_cmd_buf(sizeof(cm_call_id_type));
  if ( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
  {
    ret_val = TRUE;
    cmd_ptr->hdr.cmd_id = ds_cmd;
    call_id_ptr = (cm_call_id_type*)cmd_ptr->cmd_payload_ptr;

    *call_id_ptr = t31_ctrl_blk->call_id;
    ds_put_cmd(cmd_ptr);
  }

  return ret_val;
} /* dst31i_send_ds_cmd */

/*=============================================================================
** FUNCTION: dst31i_send_dsat_result
**
** DESCRIPTION:
**   This function is used to send AT result codes to the local DTE via 
**   DSAT.  Two methods are used:  
**     1) If the AT command being responded to was invoked as an async 
**        command on behalf of DSAT, then a callback function supplied by 
**        DSAT is used to send the response.  The callback is used to tell 
**        DSAT that async processing for the command is complete.
**     2) If the AT command was not invoked using DSAT async services then
**        the response is sent using dsat_send_dsat_result().  This assumes
**        that the state of SIO processing has been changed to online-command
**        mode prior to the response being sent.
**
** PARAMETERS:
**   dsat_result == ATCOP result code to send to the local DTE
**   dsat_result_type == T31_DSAT_RESULT_FINAL | T31_DSAT_RESULT_INTERMEDIATE
**                
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_send_dsat_result
(
  dsat_result_enum_type       dsat_result,
  dst31_dsat_result_enum_type dsat_result_type
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Intermediate result codes are sent using the DSAT API. Final result
  ** codes are sent using the callback function passed to us by DSAT when
  ** command execution began.  The callback is used because T31 AT commands
  ** are processed as asynchronous events by ATCOP.  Thus, the callback is
  ** used to let ATCOP know the command has completed.
  */
  if (dsat_result_type == DST31_DSAT_RESULT_FINAL)
  {
    if (t31_ctrl_blk->dsat_callback)
    {
      DATA_MSG1_HIGH("FINAL AT result via async, RC=%d", dsat_result);
      (*t31_ctrl_blk->dsat_callback)(dsat_result);
    }
    else
    {
      /* This is used as a catch all if the callback isn't defined */
      DATA_MSG1_HIGH("FINAL AT result no async, RC=%d", dsat_result);
      dsat_send_intermediate_result_code(dsat_result);
    }
  }
  else
  {
    DATA_MSG1_HIGH( "Send inter-AT result, RC=%d", dsat_result);

    dsat_send_intermediate_result_code(dsat_result);

#ifdef DST31_ENABLE_SIO_TX_FLUSH
    /* We need to intermediate result codes over to the local TE as quickly 
    ** as possible therefore, we will flush the SIO TX queue.
    */
    t31_ctrl_blk->flags.wait_sio_flush_tx = TRUE;
 
    DATA_MSG0_HIGH( "Scheduling SIO TX FLUSH");
    if (ds3g_siolib_ex_flush_tx(dst31i_sio_flush_tx_cb,
                             ds3g_siolib_ex_get_csd_port()) != DS3G_SIOLIB_OK)
    {
      DATA_MSG0_ERROR("Error flushing SIO TX queue");
    }
#endif 
  }
} /* dst31i_send_dsat_result */


/*=============================================================================
** FUNCTION: dst31i_request_sio_mode_change
**
** DESCRIPTION:
**   This function is used to request an SIO mode change to either online-
**   command or online-data mode via DSMGR.
**
** PARAMETERS:
**   new_mode == indicates which mode to change to:
**      - DST31_SIO_ONLINE_CMD_MODE | DST31_SIO_ONLINE_DATA_MODE
**   
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
void dst31i_request_sio_mode_change
(
  dst31_sio_mode_enum_type  new_sio_mode
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Don't allow the mode change request if we are in IDLE mode or
  ** if we are already in the correct SIO mode.
  */
  if ((!DST31_IN_IDLE_STATE(t31_ctrl_blk->curr_state)) &&
      (t31_ctrl_blk->sio_mode != new_sio_mode))
  {
    ds_cmd_enum_type         ds_cmd;

    /* Set the command to switch to either ONLINE command or
    ** online data mode.
    */
    if (new_sio_mode == DST31_SIO_ONLINE_CMD_MODE)
    {
      DATA_MSG0_HIGH("Request ONLINE_CMD mode change");
      ds_cmd = DS_CMD_ENTER_ONLINE_CMD_TX_FLUSH;
    }
    else
    {
      DATA_MSG0_HIGH("Request ONLINE_DATA mode change");
      ds_cmd = DS_CMD_ENTER_ONLINE_DATA_TX_FLUSH;
    }

    /* Send the command to the DS task */
    if (dst31i_send_ds_cmd(ds_cmd) == FALSE)
    {
      DS_3GPP_MSG1_ERROR("dst31i_send_ds_cmd() failed, cmd=%d", (int)ds_cmd); /*lint !e571 */
    }

    /* Now make sure we keep track that a mode change is in progress. */
    t31_ctrl_blk->flags.sio_mode_change_pending = TRUE;
    t31_ctrl_blk->sio_mode = DST31_SIO_WAITING_FOR_MODE_CHANGE;
  }
} /* dst31i_request_sio_mode_change */


/*=============================================================================
** FUNCTION: dst31i_complete_ul_hdlc_frame
**
** DESCRIPTION:
**   This function is used to complete the processing for an UL HDLC data
**   data stream coming from the DTE.  If the frame is determined to be
**   valid, it will be packaged up into a DSM item so the FA can prepare
**   it for OTA transmission.  If the frame is determined to be invalid
**   the data is discarded.  In the latter case, it is up to the DTE to
**   recover from whatever error(s) may occur.
**
** PARAMETERS:
**   void
**   
** RETURNS:
**   DST31_FRAME_COMPLETE if the frame is valid
**   DST31_FRAME_CORRUPT if the frame is invalid
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
LOCAL dst31_frame_status_enum_type dst31i_complete_ul_hdlc_frame
(
  void
)
{
  struct dst31_io_data_s *      io_data_ptr;
  uint8 *                       buf_ptr;
  dst31_frame_status_enum_type  ret_stat = DST31_FRAME_COMPLETE;
  uint16                        i;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);

  /* Now make sure we have a valid frame */
  if (io_data_ptr->buf[0] == DST31_HDLC_FAX_ADDRESS)
  {
    DATA_MSG2_HIGH("+FTH: HDLC-EOF type=0x%x cnt=%d", 
      (io_data_ptr->buf[2] & DST31_T30_FCF_MASK),
      io_data_ptr->cnt);

    /* Let the FA do its thing with the frame before bit stuffing it */
    (void) dsgcsdfa_process_bcs_frame(io_data_ptr->buf, io_data_ptr->cnt, TRUE);

    /* Need to tell the caller if this the last frame */
    if (io_data_ptr->buf[1] == DST31_HDLC_CONTROL_PF1)
    {
      t31_ctrl_blk->flags.fth_last_frame = TRUE;
    }
    else
    {
      t31_ctrl_blk->flags.fth_last_frame = FALSE;
    }

    /* We need to know when T.30 is in the training phase */
    if (DST31_T30_DCS_FRAME(io_data_ptr->buf))
    {
      DST31_T30_SET_TRAINING_PHASE_ACTIVE();
    }
    else
    {
      DST31_T30_SET_TRAINING_PHASE_INACTIVE();
    }

    /* Calculate and append the FCS */
    /* Need to reverse the bits before calculating the FCS */
    buf_ptr = io_data_ptr->buf;
    for (i = io_data_ptr->cnt; i > 0; i--, buf_ptr++)
    {
      *buf_ptr = GCSD_REFLECT_BYTE(*buf_ptr);
    }

    io_data_ptr->fcs = 
      crc_16_calc(io_data_ptr->buf, (uint16)(io_data_ptr->cnt * 8));

    DATA_MSG1_HIGH("+FTH::calculated FCS=0x%x",io_data_ptr->fcs);

    io_data_ptr->buf[io_data_ptr->cnt++] = (io_data_ptr->fcs & 0xFF00) >> 8;
    io_data_ptr->buf[io_data_ptr->cnt++] = (io_data_ptr->fcs & 0x00FF);

    /* Add the closing flags here so they get bit shifted into the final
    ** output stream.  Only 1 closing flag is required but we add a couple
    ** just to be safe!
    */
    io_data_ptr->buf[io_data_ptr->cnt++] = DST31_HDLC_FLAG;
    io_data_ptr->buf[io_data_ptr->cnt++] = DST31_HDLC_FLAG;

    /* Now zero-bit stuff the data */
    io_data_ptr->cnt = dst31hdlci_zbi(io_data_ptr->buf, io_data_ptr->cnt,
                         io_data_ptr->zbuf);
  }
  else
  {
    ret_stat = DST31_FRAME_CORRUPT;
  }

  return (ret_stat);
} /* dst31i_complete_ul_hdlc_frame */


/*=============================================================================
** FUNCTION: dst31i_perform_ul_dle_filtering
**
** DESCRIPTION:
**   This function is used to perform uplink DLE filtering of raw data 
**   streams received from the DTE for both the +FTH and +FTM states.  
**   DLE filtering is based on the following rules:
**
**      if <DLE><DLE>             -> replace with a single <DLE>
**      if <DLE><ETX>             -> end-of-data stream found, stop!
**      if <DLE><SUB> && +FDD==1  -> replace with <DLE><DLE> else delete
**      if <DLE><any char>        -> remove <DLE><any char>
**
** PARAMETERS:
**   data_byte == the current data byte received from the DTE
**   
** RETURNS:
**   DST31_FRAME_INCOMPLETE if frame is not ready to be sent
**   DST31_FRAME_READY if frame is ready to be sent but more data to come
**   DST31_FRAME_COMPLETE if frame is ready to be sent and end-of-data
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
INLINE dst31_frame_status_enum_type dst31i_perform_ul_dle_filtering
(
  uint8                 data_byte
)
{
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_READY;
    dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  struct dst31_io_data_s *  io_data_ptr;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);

  switch (io_data_ptr->dle_data_state)
  {
    case DST31_FIND_DLE_STATE:
      if (data_byte == DST31_DLE_CHAR)
      {
        frame_stat = DST31_FRAME_INCOMPLETE;

        DATA_MSG0_LOW("found <DLE> new state=DST31_DLE_START_STATE");
        io_data_ptr->dle_data_state = DST31_DLE_START_STATE;
      }
      else
      {
        /* Append the data into the buffer and keep searching for a <DLE> */
        DATA_MSG1_LOW( "buffering raw data, val=0x%x", data_byte);
        io_data_ptr->buf[io_data_ptr->cnt++] = data_byte;
      }
      break;

    case DST31_DLE_START_STATE:
      switch (data_byte)
      {
        case DST31_ETX_CHAR:
          /* We are done! */
          frame_stat = DST31_FRAME_COMPLETE;
          io_data_ptr->dle_data_state = DST31_DLE_STOP_STATE;

          DATA_MSG0_LOW("found <DLE><ETX>, end-of-data");
          break;

        case DST31_DLE_CHAR:
          /* Found <DLE><DLE>, replace with single <DLE> */
          frame_stat = DST31_FRAME_READY;

          DATA_MSG0_LOW("replace <DLE><DLE> -> <DLE>");
          io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;

          /* Keep track of bytes removed */
          t31_ctrl_blk->byte_cnts.dle_bytes++;

          /* Now start over looking for <DLE><ETX> */
          io_data_ptr->dle_data_state = DST31_FIND_DLE_STATE;
          break;

        case DST31_SUB_CHAR:
          /* Found <DLE><SUB>, if +FDD==1 replace with <DLE><DLE> */
          frame_stat = DST31_FRAME_READY;

          DATA_MSG0_LOW("replace <DLE><SUB> -> <DLE><DLE>");

          if (t31_ctrl_blk->at_params.fdd == 1)
          {
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
          }

          /* Now start over looking for <DLE><ETX> */
          io_data_ptr->dle_data_state = DST31_FIND_DLE_STATE;
          break;

        default:
          /* Anything else is invalid so we just discard the data */
          frame_stat = DST31_FRAME_READY;

          DATA_MSG1_LOW("remove invalid <DLE><0x%x> sequence", data_byte);
        
          /* Keep track of bytes removed */
          t31_ctrl_blk->byte_cnts.dle_bytes++;

          /* Now start over looking for <DLE><ETX> */
          io_data_ptr->dle_data_state = DST31_FIND_DLE_STATE;
      } /* switch */
      break;

    case DST31_DLE_STOP_STATE:
      DATA_MSG2_HIGH("DLE filtering complete, bc=%d +DLEs=%d",
        io_data_ptr->cnt, t31_ctrl_blk->byte_cnts.dle_bytes);
      break;

    default:
      DATA_MSG2_ERROR("invalid DLE state, state=%d cnt=%d",
        io_data_ptr->dle_data_state,io_data_ptr->cnt);
  } /* switch */

  return (frame_stat);
} /* dst31i_perform_ul_dle_filtering */


/*=============================================================================
** FUNCTION: dst31i_perform_dl_dle_filtering
**
** DESCRIPTION:
**   This function is used to perform downlink DLE filtering of raw data 
**   streams received from the remote end for both the +FRH and +FRM states.  
**   DLE filtering is based on the following rules:
**
**      if <DLE>                  -> replace with <DLE><DLE>
**      if <DLE><DLE> && +FDD=1   -> replace with <DLE><SUB>
**
** PARAMETERS:
**   data_byte == the current data byte received from the DTE
**   
** RETURNS:
**   None
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
INLINE void dst31i_perform_dl_dle_filtering
(
  uint8                    data_byte
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  struct dst31_io_data_s *  io_data_ptr;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);

  switch (io_data_ptr->dle_data_state)
  {
    case DST31_FIND_DLE_STATE:
      if (data_byte == DST31_DLE_CHAR)
      {
        DATA_MSG0_LOW("found <DLE>, goto DST31_DLE_START_STATE");
        io_data_ptr->dle_data_state = DST31_DLE_START_STATE;
      }
      else
      {
        /* Append the data into the buffer and keep searching for a <DLE> */
        DATA_MSG1_LOW( "buffering raw data, val=0x%x", data_byte);
        io_data_ptr->buf[io_data_ptr->cnt++] = data_byte;
      }
      break;

    case DST31_DLE_START_STATE:
      switch (data_byte)
      {
        case DST31_DLE_CHAR:
          /* Found <DLE><DLE>, if +FDD==1 replace with <DLE><SUB> */
          if (t31_ctrl_blk->at_params.fdd == 1)
          {
            DATA_MSG0_LOW("replace <DLE><DLE> -> <DLE><SUB><DLE><SUB>");
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_SUB_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_SUB_CHAR;
          }
          else
          {
            DATA_MSG0_LOW("replace <DLE><DLE> -> <DLE><DLE><DLE><DLE>");
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
            io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
          }

          /* Keep track of bytes added */
          t31_ctrl_blk->byte_cnts.dle_bytes += 2;

          /* Now start over looking for another <DLE> */
          DATA_MSG0_LOW("goto DST31_FIND_DLE_STATE");
          io_data_ptr->dle_data_state = DST31_FIND_DLE_STATE;
          break;

        default:
          /* Since we got here we must have found a DLE but the next character
          ** wasn't a DLE...so we have to buffer the following: <DLE><DLE><char>
          */
          DATA_MSG1_LOW("inserting <DLE><DLE><0x%x>", data_byte);
          io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
          io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
          io_data_ptr->buf[io_data_ptr->cnt++] = data_byte;

          /* Keep track of bytes added */
          t31_ctrl_blk->byte_cnts.dle_bytes++;

          /* Now start over looking for <DLE><ETX> */
          DATA_MSG0_LOW("goto DST31_FIND_DLE_STATE");
          io_data_ptr->dle_data_state = DST31_FIND_DLE_STATE;
      } /* switch */
      break;

    case DST31_DLE_STOP_STATE:
      DATA_MSG2_HIGH("DLE filtering complete, bc=%d -DLEs=%d",
        io_data_ptr->cnt, t31_ctrl_blk->byte_cnts.dle_bytes);
      break;

    default:
      DATA_MSG2_ERROR("invalid DLE state, state=%d cnt=%d",
        io_data_ptr->dle_data_state,io_data_ptr->cnt);
  } /* switch */
} /* dst31i_perform_dl_dle_filtering */



/*=============================================================================
** EXPORTED FUNCTIONS USED SOLELY BY T31
**===========================================================================*/


/*=============================================================================
** FUNCTION: dst31i_dump_dsm_item
**
** DESCRIPTION:
**   Dumps the contents of a dsm packet chain for debug purposes.
**
** PARAMETERS:
**   debug_msg == pointer to a message to display with the data
**   dsm_ptr == pointer to the dsm item to dump
**
** RETURNS:
**   number of bytes of data in the dsm packet chain
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
uint16 dst31i_dump_dsm_item
(
  char *          debug_msg,
  dsm_item_type * dsm_ptr
)
{
  static char     zbuf[1024];
  uint16          i;
  uint16          pkt_sz;


  /* Safety net */
  ASSERT(debug_msg);

  /* To quiet lint */
  (void) debug_msg;

  if ((pkt_sz = (uint16)dsm_length_packet(dsm_ptr)) > 0)
  {
     MSG_SPRINTF_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"%s::dump %d bytes of data",debug_msg, pkt_sz);

    (void) dsm_extract(dsm_ptr, 0, zbuf, pkt_sz);

    for (i = 0; i < pkt_sz; i++)
    {
      DATA_MSG2_HIGH("  -->buf[%d]=0x%x", i,zbuf[i]);
    }
  }

  return (pkt_sz);
} /* dst31i_dump_dsm_item */


/*=============================================================================
** FUNCTION: dst31i_terminate_session
**
** DESCRIPTION:
**   This function is used to terminate the facsimile session due to either
**   a T31 or FA protocol error which requires the session to be aborted.
**
** PARAMETERS:
**   t31_state == identifies which T31 state to enter next.  This will 
**     either be WAIT_FOR_ORDER or IDLE depending on how the session is
**     being terminated.
**
**   dsat_result == identifies the DSAT result code to send to the DTE prior
**     to terminating the session.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_terminate_session
(
  dst31_state_enum_type         t31_state,
  dsat_result_enum_type         dsat_result
)
{
#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE
  DATA_MSG0_HIGH("Protocol Error, session terminated!");

  /* Notify DSAT that the command has completed. */
  dst31i_send_dsat_result(dsat_result, DST31_DSAT_RESULT_FINAL);

  if (dst31i_send_ds_cmd(DS_CMD_3GPP_GCSD_DISCONNECT) == TRUE)
  {
    dst31i_change_state(t31_state, TRUE);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("dst31i_send_ds_cmd() failed, cmd=%d",
      (int)DS_CMD_3GPP_GCSD_DISCONNECT);
  }
#else
  DATA_MSG1_HIGH("Protocol Error, session terminated!,"
                  "New State=%d", t31_state);

  /* This is required to terminate the call under TREX */
  ds_gcsd_send_cmd("FPS", DS_GCSD_DISCONNECT_MO_REQ);

  /* Reset our context */
  dst31i_context_reset(TRUE);
#endif
} /* dst31i_terminate_session */


/*=============================================================================
** FUNCTION: dst31i_check_fa_status
**
** DESCRIPTION:
**   This function is used to check the return status of any FA interface
**   function call.  Based on the returned status, T31 will behave as follows
**
**      DS_FA_SUCCESS   == stay in the same state
**      DS_FA_FAILURE   == terminate the session
**      DS_FA_SWITCHING_TO_IDLE == transition to DST31_WAIT_FOR_ORDER_STATE
**      DS_FA_UPLINK_COMPLETE == indicates NT mode has finished UL processing
**
** PARAMETERS:
**   fa_stat == current FA interface function return value to check
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_check_fa_status
(
  ds_fa_status_enum_type        fa_stat
)
{

  DATA_MSG1_HIGH( "%d detected", fa_stat);
 
  switch (fa_stat)
  {
    case DS_FA_UPLINK_DATA_QUEUED:
    case DS_FA_UPLINK_COMPLETE:
    case DS_FA_SUCCESS:
      /* Do nothing */
      break;

    case DS_FA_SWITCHING_TO_IDLE:

      /* FA state change detected, goto WAIT_FOR_ORDER state */
      dst31i_change_state(DST31_WAIT_FOR_ORDER_STATE, FALSE);
      break;

    case DS_FA_INVALID_DATA_TYPE:
      /* FA failure detected, terminate the session */

      dst31i_terminate_session(DST31_WAIT_FOR_ORDER_STATE, DSAT_OK);
      break;

    case DS_FA_FAILURE:
      /* FA failure detected, terminate the session */

      dst31i_terminate_session(DST31_WAIT_FOR_ORDER_STATE, DSAT_ERROR);
      break;

    default:
      DATA_MSG1_ERROR("unknown FA status, value=%d", fa_stat);
   } /* switch */
} /* dst31i_check_fa_status */


/*=============================================================================
** FUNCTION: dst31i_build_dl_hdlc_frame
**
** DESCRIPTION:
**   This function builds an HDLC frame from downlink data received via the
**   FA for transmission to the local DTE. The data is appended to any 
**   existing data for the current frame being built.  It may take several 
**   iterations to build a complete HDLC frame since the data being received 
**   the FA will not likely comes as a complete frame.  The P/F bit of the
**   control field is used to determine if more than one frame will be 
**   received.  All HDLC flags are stripped from the data stream. The received 
**   FCS is included in the frame unaltered.
**   
** PARAMETERS:
**   in_data_ptr == pointer to the received data
**   in_byte_cnt == number of data bytes in in_data_ptr
**   out_data_ptr == pointer to return a HDLC frame if one is found
**   last_frame_ptr == pointer to return indication of whether there are
**     more frame coming
**   fcs_stat_ptr == pointer to indicate whether the completed frame has passed
**     the FCS validation
**
** RETURNS:
**   DST31_FRAME_INCOMPLETE if a complete frame is not ready
**   DST31_FRAME_COMPLETE if a complete frame is ready
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
dst31_frame_status_enum_type dst31i_build_dl_hdlc_frame
(
  dsm_item_type **              in_data_ptr,
  uint16                        in_byte_cnt,
  dsm_item_type **              out_data_ptr,
  boolean *                     last_frame_ptr,
  boolean *                     fcs_stat_ptr
)
{
  static uint16                valid_crc_16 = CRC_16_OK;
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  struct dst31_io_data_s *     io_data_ptr;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);


  /* Safety net */
  ASSERT(in_data_ptr);
  ASSERT(out_data_ptr);
  ASSERT(last_frame_ptr);
  ASSERT(fcs_stat_ptr);

  /* We can't calculate the FCS until we have a complete frame so 
  ** let's make sure to indicate as such.
  */
  *fcs_stat_ptr = DST31_FCS_CALC_PENDING;

  if ((in_byte_cnt > 0) &&
      ((in_byte_cnt + io_data_ptr->cnt) < ARR_SIZE(io_data_ptr->buf)))
  {
    /* Keep track of the received byte count */
    t31_ctrl_blk->byte_cnts.total_rx_hdlc_bytes += in_byte_cnt;

    /* Given that the data is zero-bit stuffed and flags may cross byte
    ** boundaries, the ZBD processing routine is responsible for parsing
    ** individual frames.  Thus, we simply send in the received data buffer
    ** to the ZBD routine and let it parse out the frame data.  It will 
    ** tell us when a complete frame has been found at which point we
    ** can filter it, calculate the FCS,and package it up for transmission
    ** to the local DTE.
    */
    frame_stat = dst31hdlci_zbd(in_data_ptr, io_data_ptr->zbuf, 
                   &io_data_ptr->cnt, 
                   &t31_ctrl_blk->carrier_detected);

    DATA_MSG2_LOW("+FRH: frame_stat=%d frame_sz=%d",
                frame_stat, io_data_ptr->cnt);

    /* If we have found a complete frame and can post process it */
    if (frame_stat == DST31_FRAME_COMPLETE) 
    {
      if (io_data_ptr->cnt >= DST31_HDLC_MIN_FRAME_SZ) 
      {
        uint8 * buf_ptr  = io_data_ptr->zbuf;
        uint16  frame_sz = io_data_ptr->cnt - 2; /* don't include the FCS */
        uint16  i;

        /* Calculate our own FCS on the frame */
        io_data_ptr->fcs = crc_16_calc(io_data_ptr->zbuf, (uint16)((io_data_ptr->cnt) * 8));

        *fcs_stat_ptr = (io_data_ptr->fcs == valid_crc_16)
                           ? DST31_FCS_CALC_DONE_PASS
                           : DST31_FCS_CALC_DONE_FAIL;

        /* Need to tell the caller if the FCS was valid or not */
        /* Filter the data before passing it back out.  We don't include the
        ** FCS in the filter process since it is not actual fax data.
        **
        ** Note: we will be pulling/pushing data from the internal zbuf buffer
        ** and pushing it back into the primary data buffer in this process.
        */
        io_data_ptr->cnt = 0;
        buf_ptr          = io_data_ptr->zbuf;

        for (i = frame_sz; i > 0; i--, buf_ptr++)
        {
          /* Reverse the bit order first */
          *buf_ptr = GCSD_REFLECT_BYTE(*buf_ptr);

          /* The filtered data is placed back into io_data_ptr->buf */
          dst31i_perform_dl_dle_filtering(*buf_ptr);
        } /* for */

        /* Now pull in the received FCS */
        io_data_ptr->buf[io_data_ptr->cnt++] = io_data_ptr->zbuf[frame_sz];
        io_data_ptr->buf[io_data_ptr->cnt++] = io_data_ptr->zbuf[frame_sz+1];

        DATA_MSG2_HIGH("+FRH::frame_id=0x%x FCS=0x%x",
          (io_data_ptr->buf[2] & DST31_T30_FCF_MASK), io_data_ptr->fcs);

        /* Append the closing DLE/ETX pairing */
        io_data_ptr->buf[io_data_ptr->cnt++] = DST31_DLE_CHAR;
        io_data_ptr->buf[io_data_ptr->cnt++] = DST31_ETX_CHAR;

        /* Is this the last frame of the sequence? */
        *last_frame_ptr = (io_data_ptr->buf[1] == DST31_HDLC_CONTROL_PF1);

        /* Put the frame into a DSM item so it can be sent off to SIO */
        if (dst31i_dsm_pushdown(out_data_ptr, io_data_ptr->buf, 
              io_data_ptr->cnt, DSM_DS_SMALL_ITEM_POOL) == FALSE)
        {
          frame_stat = DST31_FRAME_CORRUPT;
        }

#if 0 /* Debug use only */
        if (*in_data_ptr)
        {
          (void) dst31i_dump_dsm_item("+FRH-DATA-REMAINING", *in_data_ptr);
        }
#endif
      }
      else
      {
        DATA_MSG0_ERROR("+FRH: invalid HDLC frame rx'd, toss and reset");

        /* Make sure to reset the data buffers */
        dst31hdlci_zbd_init();

        DST31_RESET_IO_BUFFER();

        frame_stat = DST31_FRAME_INCOMPLETE;
      }
    }
  }
  else if (in_byte_cnt)
  {
    /* OK, we don't have enough room in our internal buffer so we are going to
    ** toss the data.  We treat this as a FCS failure which will cause the upper 
    ** layer to return an ERROR condition to the DTE.
    */
    DATA_MSG2_ERROR("+FRH::buffer overflow, rx_bytes=%d buf_sz=%d",
      (in_byte_cnt + io_data_ptr->cnt), ARR_SIZE(io_data_ptr->buf));

    frame_stat = DST31_FRAME_COMPLETE;
    fcs_stat_ptr = FALSE;
  }

  return (frame_stat);
} /* dst31i_build_dl_hdlc_frame */


/*=============================================================================
** FUNCTION: dst31i_build_ul_hdlc_frame
**
** DESCRIPTION:
**   This function builds an HDLC frame for UL transmission using the data
**   received from SIO via DSM. The data is appended to any existing data for 
**   the current frame being built.  It may take several iterations to build 
**   a complete HDLC frame since the data being received from SIO will not 
**   likely comes as a complete frame.  Multiple frames can be received at a 
**   time and are delineated by a <DLE><ETX> pairing in the data. The final 
**   frame in a sequence is signalled by the P/F bit being asserted.
**
** PARAMETERS:
**   in_data_ptr == pointer to the received data
**   in_byte_cnt == number of data bytes in in_data_ptr
**
** RETURNS:
**   DST31_FRAME_INCOMPLETE if a complete frame is not ready
**   DST31_FRAME_COMPLETE if a complete frame is ready
**   DST31_FRAME_CORRUPT if the frame is invalid
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
dst31_frame_status_enum_type dst31i_build_ul_hdlc_frame
(
  dsm_item_type **              in_data_ptr,
  uint16                        in_byte_cnt
)
{
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  uint8                        data_byte;
  uint16                       i;
  struct dst31_io_data_s *     io_data_ptr;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);


  /* Safety net */
  ASSERT(in_data_ptr);

  if ((in_byte_cnt > 0) &&
      ((in_byte_cnt + io_data_ptr->cnt) < ARR_SIZE(io_data_ptr->buf)))
  {
    /* We place data into our local buffer by appending it to whatever data
    ** is currently stored in the buffer.  We keep track of the current 
    ** number of bytes in the buffer and the offset of where to append the
    ** data so we can move things around if a complete HDLC frame is found.
    */
    if (io_data_ptr->cnt == 0)
    {
      DATA_MSG0_HIGH("+FTH: HDLC-SOF");
    }

    /* Start searching through the received data */
    for (i = in_byte_cnt; (i > 0) && (frame_stat != DST31_FRAME_COMPLETE); i--)
    {
      int16 dsm_byte;
      
      dsm_byte = dsm_pull8(in_data_ptr);

      if (dsm_byte == DST31_DSM_PULLUP_ERROR)
      {
        /* No more data in the buffer so quit looping on it */
        break;
      }
      data_byte = (uint8) dsm_byte;

      /* We a pull a byte of data and determine how to process it based on our
      ** current DLE state.  Our goal is to find a <DLE><ETX> sequence which
      ** indicates end-of-data from the DTE.  However, we may find other <DLE>
      ** character pairings that we have to filter according to the following
      ** rules:
      **
      **    if <DLE><DLE>             -> replace with a single <DLE>
      **    if <DLE><ETX>             -> end-of-data stream found, stop!
      **    if <DLE><SUB> && +FDD==1  -> replace with <DLE><DLE> else delete
      **    if <DLE><any char>        -> remove <DLE><any char>
      */
      frame_stat = dst31i_perform_ul_dle_filtering(data_byte);

      if (frame_stat == DST31_FRAME_COMPLETE)
      {
        /* We have a complete frame. Let the FA take a look at it. */
        frame_stat = dst31i_complete_ul_hdlc_frame();
      }
    } /* for */
  } /* if (in_byte_cnt... */
  else if (in_byte_cnt)
  {
    /* OK, we don't have enough room in our internal buffer so we are going
    ** to toss the data.  We treat this as a FCS failure which will cause the
    ** upper layer to return an ERROR condition to the DTE.
    */
    DATA_MSG2_ERROR("+FTH::buffer overflow, rx_bytes=%d buf_sz=%d",
      (in_byte_cnt + io_data_ptr->cnt), ARR_SIZE(io_data_ptr->buf));

    frame_stat = DST31_FRAME_CORRUPT;
  }

  return (frame_stat);
} /* dst31i_build_ul_hdlc_frame */


/*=============================================================================
** FUNCTION: dst31i_build_dl_msg_frame
**
** DESCRIPTION:
**   This function builds a fax message frame (page data) to send to the DTE 
**   using data received from the FA via the remote fax device.  A frame in
**   this case is simply a block of raw page data that has successfully 
**   passed DLE filtering.  Basically, whatever is received for one downlink
**   iteration of this function is processed and transmitted to the DTE.
**
** PARAMETERS:
**   in_data_ptr == pointer to the received data
**   in_byte_cnt == number of data bytes in in_data_ptr
**   out_data_ptr == pointer to return a complete frame if one is found
**
** RETURNS:
**   DST31_FRAME_EMPTY if no data is found
**   DST31_FRAME_READY if msg data has been found
**   DST31_FRAME_COMPLETE if end of message is found
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
dst31_frame_status_enum_type dst31i_build_dl_msg_frame
(
  dsm_item_type **              in_data_ptr,
  uint16                        in_byte_cnt,
  dsm_item_type **              out_data_ptr
)
{
  uint8                        data_byte;
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_EMPTY;
  uint16                       i;
  struct dst31_io_data_s *     io_data_ptr;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &t31_ctrl_blk->io_data;

  /* Safety net */
  ASSERT(in_data_ptr);
  ASSERT(out_data_ptr);

  if ((in_byte_cnt > 0) &&
      ((in_byte_cnt + io_data_ptr->cnt) < ARR_SIZE(io_data_ptr->buf)))
  {
    /* Keep track of the byte count */
    t31_ctrl_blk->byte_cnts.total_rx_msg_bytes += in_byte_cnt;

    /* We place data into our local buffer by appending it to whatever data
    ** is currently being received.  In this state raw fax message data is
    ** being received from the remote end. There is no explicit end-of-data
    ** marker.  We are told when end-of-data occurs when the FA switches back
    ** to its IDLE state.  This is why there the 'eod' indication is passed
    ** into the function. However, we still need to perform <DLE> filtering 
    ** frames according to the following rules:
    **
    **    if <DLE>                  -> replace with <DLE><DLE>
    **    if <DLE><DLE> && +FDD==1  -> replace with <DLE><SUB>
    */

    /* Filter the data before passing it back out */
    for (i = in_byte_cnt; i > 0; i--)
    {
      int16 dsm_byte;

      dsm_byte = dsm_pull8(in_data_ptr);

      if (dsm_byte == DST31_DSM_PULLUP_ERROR)
      {
        /* No more data in the buffer so quit looping on it */
        break;
      }
      data_byte = (uint8) dsm_byte;

      /* Reverse the bit order first */
      data_byte = GCSD_REFLECT_BYTE(data_byte);

      dst31i_perform_dl_dle_filtering(data_byte);
    } /* for */

    /* Just to make sure we actually have data...*/
    if (io_data_ptr->cnt > 0)
    {
      /* Indicate data is ready to be transmitted to the local DTE */
      frame_stat = DST31_FRAME_READY;
    }
    else
    {
      DATA_MSG0_HIGH("+FRM::found NULL message frame!");
      frame_stat = DST31_FRAME_EMPTY;
    }

    /* If there is data ready to be sent then package it up, otherwise
    ** we must be waiting for some additional fax data to arrive to be
    ** able to determine if the data is ready to be sent
    */
    if (frame_stat & (DST31_FRAME_COMPLETE | DST31_FRAME_READY)) /*lint !e655 */
    {
      if (dst31i_dsm_pushdown(out_data_ptr, io_data_ptr->buf,
            io_data_ptr->cnt, DSM_DS_SMALL_ITEM_POOL) == FALSE)
      {
        frame_stat = DST31_FRAME_CORRUPT;
      }
    }
  }
  else if (in_byte_cnt)
  {
    /* OK, we don't have enough room in our internal buffer so we are going to
    ** toss the data.
    */
    DATA_MSG2_ERROR("+FRM::buffer overflow, rx_bytes=%d buf_sz=%d",
      (in_byte_cnt + io_data_ptr->cnt), ARR_SIZE(io_data_ptr->buf));

    frame_stat = DST31_FRAME_INCOMPLETE;
  }

  return (frame_stat);
} /* dst31i_build_dl_msg_frame */


/*=============================================================================
** FUNCTION: dst31i_build_ul_msg_frame
**.
** DESCRIPTION:
**   This function builds a fax message frame (page data) to send to the remote 
**   fax device using data received from the SIO via the local DTE.  A frame in
**   this case is simply a block of raw page data that has successfully 
**   passed DLE filtering.  Basically, whatever is received for one uplink
**   iteration of this function is processed and transmitted to the remote end.
**   The DTE will signal an end-of-data condition by sending a terminating 
**   <DLE><ETX> sequence in the data stream.
**
** PARAMETERS:
**   in_data_ptr == pointer to the received data
**   in_byte_cnt == number of data bytes in in_data_ptr
**   out_data_ptr == pointer to return msg data
**
** RETURNS:
**   DST31_FRAME_EMPTY if no data is found
**   DST31_FRAME_READY if msg data has been found
**   DST31_FRAME_COMPLETE if end of message is found
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
dst31_frame_status_enum_type dst31i_build_ul_msg_frame
(
  dsm_item_type **              in_data_ptr,
  uint16                        in_byte_cnt,
  dsm_item_type **              out_data_ptr
)
{
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  uint8                        data_byte;
  uint16                       i;
  struct dst31_io_data_s *     io_data_ptr;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &t31_ctrl_blk->io_data;

  /* Safety net */
  ASSERT(in_data_ptr);
  ASSERT(out_data_ptr);

  if ((in_byte_cnt > 0) &&
      ((in_byte_cnt + io_data_ptr->cnt) < ARR_SIZE(io_data_ptr->buf)))
  {
    /* We place data into our local buffer by appending it to whatever data
    ** data is currently being received.  In this state raw fax message data
    ** is being received from the DTE.  The end-of-data is marked by the 
    ** a <DLE><ETX> pair so all we have to do is search for this.  However,
    ** we still need to perform <DLE> filtering as was done with the HDLC
    ** frames according to the following rules:
    **
    **    if <DLE><DLE>             -> replace with a single <DLE>
    **    if <DLE><ETX>             -> end-of-data stream found, stop!
    **    if <DLE><SUB> && +FDD==1  -> replace with <DLE><DLE> else delete
    **    if <DLE><any char>        -> remove <DLE><any char>
    */

    /* Start searching through the received data */
    for (i = in_byte_cnt; (i > 0) && (frame_stat != DST31_FRAME_COMPLETE); i--)
    {
      int16 dsm_byte;

      dsm_byte = dsm_pull8(in_data_ptr);

      if (dsm_byte == DST31_DSM_PULLUP_ERROR)
      {
        /* No more data in the buffer so quit looping on it */
        break;
      }
      data_byte = (uint8) dsm_byte;

      /* Filter the data before passing it back out */
      frame_stat = dst31i_perform_ul_dle_filtering(data_byte);
    } /* for */

    /* If there is data ready to be sent then package it up */
    if (frame_stat & (DST31_FRAME_COMPLETE | DST31_FRAME_READY)) /*lint !e655 */
    {
      /* There could be 0 bytes in the buffer if the closing <DLE><ETX>
      ** ends up by itself so don't try to push anything if this is the case.
      */
      if (io_data_ptr->cnt > 0)
      {
        DATA_MSG1_LOW("+FTM::push %d bytes",io_data_ptr->cnt);

        if (dst31i_dsm_pushdown(out_data_ptr, io_data_ptr->buf,
              io_data_ptr->cnt, DSM_DS_LARGE_ITEM_POOL) == TRUE)
        {
          /* Keep track of the byte count */
          t31_ctrl_blk->byte_cnts.total_tx_msg_bytes += io_data_ptr->cnt;
        }
      }
    }
  }
  else if (in_byte_cnt)
  {
    /* OK, we don't have enough room in our internal buffer so we are going to
    ** toss the data.  We treat this as a FCS failure which will cause the upper 
    ** layer to return an ERROR condition to the DTE.
    */
    DATA_MSG2_ERROR("+FTM::buffer overflow, msg_cnt=%d buf_sz=%d",
      (in_byte_cnt + io_data_ptr->cnt), ARR_SIZE(io_data_ptr->buf));

    frame_stat = DST31_FRAME_INCOMPLETE;
  }

  return (frame_stat);
} /* dst31i_build_ul_msg_frame */


/*=============================================================================
** FUNCTION: dst31i_change_state
**
** DESCRIPTION:
**   This function is used to change the T31 state machine to the specified
**   state.  Actual state processing for the new state will begin when T31
**   receives the next notification for uplink/downlink processing from GCSDPS.
**
** PARAMETERS:
**   new_state == identifies the state to transition to
**   set_fa_to_idle == TRUE if the FA should be set to its IDLE state when
**     the state change occurs, FALSE if the FA should be left in its
**     current state.
**                
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_change_state
(
  dst31_state_enum_type new_state,
  boolean               set_fa_to_idle
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Clear our timers just to be safe */
  DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

  /* We don't need to transition if the new state is the same as the 
  ** current state.  This shouldn't happen (yeah right!) but if it does
  ** this cuts down on the overhead since we will already be in the
  ** specified state.
  */
  if (new_state != t31_ctrl_blk->curr_state)
  {
    t31_ctrl_blk->last_state = t31_ctrl_blk->curr_state;
    t31_ctrl_blk->curr_state = new_state;

    T31_MSG_HIGH("Change state: last=%d curr=%d new=%d", 
      t31_ctrl_blk->last_state, t31_ctrl_blk->curr_state, new_state);

    /* Do not alter this flag if we are still waiting for the FA to be set
    ** to IDLE.  We have to let the FA go IDLE to maintain processing 
    ** synchronization.
    */
    if (t31_ctrl_blk->flags.set_fa_to_idle == FALSE)
    {
      t31_ctrl_blk->flags.set_fa_to_idle = set_fa_to_idle;
    }

    /* This flag tells the state machine the we are entering the new
    ** state clean and we need to initialize it before state processing
    ** can begin.
    */
    t31_ctrl_blk->flags.sop = TRUE;

    T31_MSG_STATE_CHANGE(dst31i_get_dst31_state_name(t31_ctrl_blk->last_state),
                         t31_ctrl_blk->last_state,
                         dst31i_get_dst31_state_name(t31_ctrl_blk->curr_state),
                         t31_ctrl_blk->curr_state);

    if (new_state == DST31_IDLE_STATE)
    {
      dst31i_context_reset(TRUE);

      DATA_MSG0_HIGH("Now in IDLE state");
    }
    else if (new_state == DST31_WAIT_FOR_ORDER_STATE)
    {
      /* We need to always make sure that the WFO state is initialized so
      ** we intercede and check for this state change directly. If found we
      ** go ahead and call the SOP initialization function directly.  This 
      ** is done because some fax applications will issue AT commands very 
      ** quickly and under some circumstances T31 will never actually process
      ** anything in the WFO state but T31 must pass through this state to
      ** maintain state processing integrity.
      */
      dst31i_sop_wait_for_order_state();
    }
  }
  else if (new_state == t31_ctrl_blk->last_state)
  {
    DATA_MSG1_ERROR("Erroneous state transition state=%d",new_state);
  }
} /* dst31i_change_state */


/*=============================================================================
** FUNCTION: dst31i_terminate_state
**
** DESCRIPTION:
**   This function is used to terminate current state processing.  The function
**   is called by the state handler whenever it has completed processing for
**   the current state.  It performs the following functions:
**
**     1.  Queues the specified AT result code to be sent the DTE.
**     2.  Requests a switch to online-command mode to accept new AT commands.
**     3.  Performs a state change to the WFO state.
**
** PARAMETERS:
**   dsat_result == ATCOP result code to send to the local DTE
**   dsat_result_type == T31_DSAT_RESULT_FINAL | T31_DSAT_RESULT_INTERMEDIATE
**   set_fa_to_idle == TRUE if the FA should be set to its IDLE state when
**     the state change occurs, FALSE if the FA should be left in its
**     current state.
**                
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31i_terminate_state
(
  dsat_result_enum_type       dsat_result,
  boolean                     set_fa_to_idle
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG1_HIGH("Terminate state, set_fa_to_idle=%d", set_fa_to_idle);

  /* If this is a final AT result code then we need to save it off and wait
  ** for the switch to OLCM mode to complete.  After the switch is done
  ** the watermarks will be ready and the result code can then be sent.
  ** We need to wait for the watermarks to be switched otherwise we create
  ** a race condition in that the DTE will receive the final response code
  ** and can then immediately send the next AT command.  If this occurs during
  ** the watermark switch data can and will be lost causing the call to fail.
  */
  t31_ctrl_blk->dsat_result = dsat_result;

  /* Back to online command mode */
  dst31i_request_sio_mode_change(DST31_SIO_ONLINE_CMD_MODE);

  /* Wait for the next command */
  dst31i_change_state(DST31_WAIT_FOR_ORDER_STATE, set_fa_to_idle);
} /* dst31i_terminate_state */


/*=============================================================================
** FUNCTION: dst31i_dsm_pushdown
**
** DESCRIPTION:
**   This function is simply a wrapper function around dsm_pushdown().  It is
**   used to catch and log errors to remove code redundancy each time that
**   dsm_pushdown() is called by T31.
**
**   NOTE: dsm_pushdown() failures are not considered fatal.  Data will be
**   lost but processing will continue in the hopes that the T.30 protocol
**   may be able to recover etc.
**
** PARAMETERS:
**   None.
**                
** RETURNS:
**   boolean == TRUE is push succeeds, FALSE otherwise
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
boolean dst31i_dsm_pushdown
(
  dsm_item_type **         item_ptr,
  void *                   data_buf,
  uint16                   data_size,
  dsm_mempool_id_enum_type pool_id
)
{
  boolean push_ok = TRUE;
  uint16  push_cnt = 0;


  if (data_size > 0)
  {
    push_cnt = dsm_pushdown(item_ptr, data_buf, data_size, pool_id);

    if (push_cnt != data_size)
    {
      T31_ERR("push error, needed=%d got=%d pid=%d",
        data_size, push_cnt, pool_id);

      push_ok = FALSE;

      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
  }
  else
  {
    DATA_MSG1_HIGH("push error, invalid size=%d",data_size);

    push_ok = FALSE;
  }

  return (push_ok);
} /* dst31i_dsm_pushdown */


/*=============================================================================
** FUNCTION: dst31i_dsm_pushdown_tail
**
** DESCRIPTION:
**   This function is simply a wrapper function around dsm_pushdown_tail().
**   It is used to catch and log errors to remove code redundancy each time
**   that dsm_pushdown_tail() is called by T31.
**
**   NOTE: dsm_pushdown_tail() failures are not considered fatal.  Data will
**   be lost but processing will continue in the hopes that the T.30 protocol
**   may be able to recover etc.
**
** PARAMETERS:
**   None.
**                
** RETURNS:
**   boolean == TRUE is push succeeds, FALSE otherwise
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
boolean dst31i_dsm_pushdown_tail
(
  dsm_item_type **         item_ptr,
  void *                   data_buf,
  uint16                   data_size,
  dsm_mempool_id_enum_type pool_id
)
{
  boolean push_ok = TRUE;
  uint16  push_cnt = 0;


  if (data_size > 0)
  {
    push_cnt = dsm_pushdown_tail(item_ptr, data_buf, data_size, pool_id);

    if (push_cnt != data_size)
    {
      T31_ERR("push error, needed=%d got=%d pid=%d",
        data_size, push_cnt, pool_id);

      push_ok = FALSE;

      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);

    }
  }
  else
  {
    DATA_MSG1_HIGH("push error, invalid size=%d",data_size);

    push_ok = FALSE;
  }

  return (push_ok);
} /* dst31i_dsm_pushdown_tail */

/*=============================================================================
** EXPORTED FUNCTIONS USED BY FPS and DSAT
**===========================================================================*/


/*=============================================================================
** FUNCTION: dst31_wm_init
**
** DESCRIPTION:
**   This function is used to initialize the DSM watermarks used by the FPS 
**   
**
**   Note that GCSDFPS doesn't actually "owns" these watermarks so T31 just sets
**   local pointers to them.  
**
** PARAMETERS:
**  dsm_watermark_type  *tps_dl_wm_ptr     - TPS downlink WM
**  dsm_watermark_type  *tps_ul_wm_ptr     - TPS uplink WM
**  dsm_watermark_type  *fps_sio_dl_wm_ptr - FAX sio downlink WM
**  dsm_watermark_type  *fps_sio_ul_wm_ptr - FAX sio uplink WM
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
void dst31_wm_init
(
  dsm_watermark_type  *tps_dl_wm_ptr,
  dsm_watermark_type  *tps_ul_wm_ptr,
  dsm_watermark_type  *fps_sio_dl_wm_ptr,
  dsm_watermark_type  *fps_sio_ul_wm_ptr
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Initialize the shortcut pointers */
  t31_ctrl_blk->tps_dl_wm_ptr = tps_dl_wm_ptr;
  t31_ctrl_blk->tps_ul_wm_ptr = tps_ul_wm_ptr;
  
  
  t31_ctrl_blk->sio_dl_wm_ptr = fps_sio_dl_wm_ptr;
  t31_ctrl_blk->sio_ul_wm_ptr = fps_sio_ul_wm_ptr;
  
  /* Now initialize the SIO wm's */
  dst31i_sio_watermark_init();
}


/*=============================================================================
** FUNCTION: dst31_poweup_init
**
** DESCRIPTION:
**   This function is used to initialize the T31 layer of the FPS. It should
**   be called at the beginning of a mobile originated or mobile terminated 
**   facsimile call prior to any facsimile data processing.
**
** PARAMETERS:
**   None.
**                
** RETURNS:
**   signal mask used to indicate to GCSDPS which T31 signal to monitor for
**   protocol timeouts.
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
rex_sigs_type dst31_powerup_init
(
  void
)
{  
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  t31_ctrl_blk->timer_ptr = &t31_ctrl_blk->timer;
  t31_ctrl_blk->i_name = "T31";
  t31_ctrl_blk->last_state = t31_ctrl_blk->curr_state = DST31_IDLE_STATE;
 
  t31_ctrl_blk->timer_sig = DS_GCSD_T31_TIMEOUT_SIG;
  rex_def_timer(t31_ctrl_blk->timer_ptr, rex_self(), t31_ctrl_blk->timer_sig);

  /* No final AT result code yet */
  t31_ctrl_blk->dsat_result = DSAT_NO_RSP;

  DATA_MSG0_HIGH("powerup-init completed");

  return (DS_GCSD_T31_TIMEOUT_SIG);
} /* dst31_powerup_init */


/*=============================================================================
** FUNCTION: dst31_at_cmd_abort
**
** DESCRIPTION:
**   This function is called by GCSD Fax Services when it receives 
**   notification from ATCOP that the current T.31 AT action command 
**   should be aborted.  T31 will clear any pending timers and set a flag
**   to indicate to T31 to abort command processing upon the next time the 
**   state processing is entered for the action to be aborted.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_at_cmd_abort
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_HIGH("AT command aborted by DTE");

  /* Just clear any pending timers and set the abort flag */
  DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);
  t31_ctrl_blk->flags.at_cmd_abort = TRUE;
} /* dst31_at_cmd_abort */



/*=============================================================================
** FUNCTION: dst31_set_at_stack_params
**
** DESCRIPTION:
**   This function is invoked to set the AT command stack parameters
**   during call.  A seperate interface is provided as the application
**   may change parameters after call initialization.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_set_at_stack_params
(
  const dsfps_params_type  *param_ptr
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  ASSERT( NULL != param_ptr );
  
  DATA_MSG0_LOW("Updating the FPS paramaters form ATCOP");
  t31_ctrl_blk->at_params = *param_ptr;
}



/*=============================================================================
** FUNCTION: dst31_process_downlink
**
** DESCRIPTION:
**   This function is used to process facsimile data received on the downlink. 
**   Data is received by T31 from the FA and queued for transmission to the 
**   local DTE using SIO.  T31 is responsbile for filtering and preprocessing
**   the data prior to queuing it to the local DTE.
**
**   T31 will also check the SIO RX watermark during downlink processing.  If
**   any data is received on the watermark it is assumed that the local DTE
**   has requested that the current receive command be aborted.  If so, T31
**   will abort receive processing and will return to the WAIT_FOR_ORDER state.
**
** PARAMETERS:
**   void
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_process_downlink
(
  void
)
{
  boolean invoke_handler;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  
  /* If T31 isn't even initialized yet, don't do any processing */
  if ((t31_ctrl_blk->flags.t31_init == FALSE) ||
       DST31_IN_IDLE_STATE(t31_ctrl_blk->curr_state))
  {
    return;
  }

  t31_ctrl_blk->ota_path = DST31_OTA_DOWNLINK;

  /* If T31 is currently in a receive state and SIO is in the correct
  ** mode (namely online-data mode), go ahead and call the 
  ** appropriate handler function.  However, if the state is a transmit
  ** state or we are waiting for SIO to enter online-data mode we 
  ** bypass calling the handler function and invoke the FA directly so
  ** it can still service its downlink processing.
  */
  invoke_handler = DST31_IN_VALID_DL_STATE(t31_ctrl_blk->curr_state);

  /* Now we either invoke the proper handler function or call the 
  ** FA directly to allow it to perform its own downlink processing.
  ** We also trap any abort indications from the DTE and abort command
  ** processing if required.
  */
  if (invoke_handler == TRUE)
  {
#ifndef T31_DISABLE_AT_CMD_ABORTING
    /* Check for command being aborted by the DTE */
    if (DST31_IN_VALID_RX_STATE(t31_ctrl_blk->curr_state))
    {
      dsm_item_type * sio_data_ptr;

      /* First we need to see if there is any data on the uplink queue
      ** from SIO.  If there is, then the DTE has aborted the current
      ** action (+FRH, +FRM, or +FRS) so we exit out and return to the
      ** WAIT_FOR_ORDER state, unless we are still waiting for the call
      ** connect setup to complete and the local fax appl aborts the
      ** call at which point we terminate the session.
      */
      sio_data_ptr = DSM_DEQUEUE( t31_ctrl_blk->sio_ul_wm_ptr );

      if (sio_data_ptr)
      {
#if 1
        (void) dst31i_dump_dsm_item("DTE-ABORT", sio_data_ptr);
#endif

        DATA_MSG2_HIGH("DTE aborted action, st=%d qcnt=%d",
          t31_ctrl_blk->curr_state, sio_data_ptr->used);

        dsm_free_packet(&sio_data_ptr);
      
        if (t31_ctrl_blk->dsat_callback)
        {
          t31_ctrl_blk->dsat_callback(DSAT_OK);
          dst31i_change_state(DST31_WAIT_FOR_ORDER_STATE, TRUE);
        }
        else
        {
          /* The only time there will be no callback registerd is at
          ** the start of the call while we are waiting for the connect
          ** setup to complete (implied +FRH or +FTH command).  If we
          ** get an abort in this state, the call is terminated!
          */
          DATA_MSG0_ERROR("FaxAppl aborting call. Too long for carrier setup");
          dst31i_terminate_session(DST31_IDLE_STATE, DSAT_NO_CARRIER);
        }

        return;
      }
    }
#endif

#ifdef DST31_ENABLE_SIO_TX_FLUSH
    /* Now if we are waiting on the SIO TX queue to be flushed we can't
    ** do any direct downlink processing (i.e. we can't being putting stuff
    ** on the queue while SIO is trying to flush it) so exit out and let
    ** SIO finish up.
    */
    if (t31_ctrl_blk->flags.wait_sio_flush_tx == TRUE)
    {
      return;
    }
#endif

    /* If we are entering the state for the first time we need to
    ** initialize the state by invoking the start of procedure function.
    */
    if (t31_ctrl_blk->flags.sop == TRUE)
    {
      /* Only invoke the SOP if one has been defined for the curren state */
      if (t31_state_tbl[t31_ctrl_blk->curr_state].state_sop)
      {
        (*t31_state_tbl[t31_ctrl_blk->curr_state].state_sop)();
      }

      t31_ctrl_blk->flags.sop = FALSE;
    }

    /* Now invoke the state handler function if one is defined */
    if (t31_state_tbl[t31_ctrl_blk->curr_state].state_handler)
    {
      (*t31_state_tbl[t31_ctrl_blk->curr_state].state_handler)();
    }
  }
  else
  {   
    dsm_item_type *         dl_data_ptr = NULL;
    ds_fa_data_enum_type    dl_data_type;
    ds_fa_status_enum_type  fa_stat;

    fa_stat = dsgcsdfa_process_downlink(&dl_data_ptr, &dl_data_type);
  
    /* We shouldn't be getting downlink data in a transmit state so toss it */
    if (dl_data_ptr)
    { 
      DATA_MSG2_HIGH("Unexpected RX data in TX state,"
                      "--> Data deleted, cnt=%d type=%d",
                      dsm_length_packet(dl_data_ptr),dl_data_type);
      
#if 0
      (void) dst31i_dump_dsm_item("T31", dl_data_ptr);
#endif
      dsm_free_packet(&dl_data_ptr);
    }

    /* Always check the outcome of the FA function */
    if (fa_stat == DS_FA_FAILURE)
    {
      DATA_MSG0_ERROR("FA failure during downlink");

      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
  }
} /* dst31_process_downlink */


/*=============================================================================
** FUNCTION: dst31_process_uplink
**
** DESCRIPTION:
**   This function is used process facsimile data received from the local DTE
**   for transmission on the uplink.  Data is received by T31 from the DTE via
**   the SIO watermark.  After preprocessing the data, T31 will pass the data
**   on to the FA which queues it to the TPS for OTA transmission.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_process_uplink
(
  void
)
{
  boolean invoke_handler;
  ds_fa_status_enum_type  fa_stat = DS_FA_SUCCESS;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();


  /* If T31 isn't even initialized yet, or the TPS is being flow controlled
  ** don't do any processing 
  */
  if ((t31_ctrl_blk->flags.t31_init == FALSE) ||
       DST31_IN_IDLE_STATE(t31_ctrl_blk->curr_state))
  {
    return;
  }

  t31_ctrl_blk->ota_path = DST31_OTA_UPLINK;

  /* We first need to check to see if the FA layer needs to be put back
  ** to its IDLE state.  If so we have to first make sure that there is no
  ** more uplink data pending to be transmitted before setting the FA to
  ** IDLE.  If we don't check for this the FA will delete the queued data
  ** when it enters its IDLE state and bad things will happen!
  */
  if (t31_ctrl_blk->flags.set_fa_to_idle == TRUE)
  {
    /* Call the FA layer to perform uplink processing and check the result.
    ** This will tell us if the FA has finished transmitting all queued
    ** data and whether or not it is ok to put it back to IDLE.
    */
    fa_stat = dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);

    if (fa_stat != DS_FA_UPLINK_DATA_QUEUED)
    {
      t31_ctrl_blk->flags.set_fa_to_idle = FALSE;
       
      fa_stat = dsgcsdfa_return_to_idle(FALSE);
    }
    else
    {
      /* Can't perform any additional processing until all the uplink
      ** data has been transmitted.
      */
      DATA_MSG0_HIGH("WFO::UL data pending, FA not set to IDLE");

      return;
    }
  }

  /* If T31 is currently in a transmit state and SIO is in the correct
  ** mode (namely online-data mode), go ahead and call the 
  ** appropriate handler function.  However, if the state is a receive 
  ** state or we are waiting for SIO to enter online-data mode we 
  ** bypass calling the handler function and invoke the FA directly so
  ** it can still service its uplink processing.
  */
  invoke_handler = DST31_IN_VALID_UL_STATE(t31_ctrl_blk->curr_state); 

  /* Now we either invoke the proper handler function or call the 
  ** FA directly to allow it to services its own uplink processing.
  */
  if (invoke_handler)
  {
#ifdef DST31_ENABLE_SIO_TX_FLUSH
    /* Now if we are waiting on the SIO TX queue to be flushed we can't
    ** do any direct uplink processing (i.e. we can't being putting stuff
    ** on the queue while SIO is trying to flush it) so exit out and let
    ** SIO finish up.
    */
    if (t31_ctrl_blk->flags.wait_sio_flush_tx == TRUE)
    {
      return;
    }
#endif

    /* If we are entering the state for the first time we need to
    ** initialize the state by invoking the start of procedure function.
    */
    if (t31_ctrl_blk->flags.sop &&
        t31_state_tbl[t31_ctrl_blk->curr_state].state_sop)
    {
      (*t31_state_tbl[t31_ctrl_blk->curr_state].state_sop)();
      t31_ctrl_blk->flags.sop = FALSE;
    }

    /* Now invoke the state handler function */
    if (t31_state_tbl[t31_ctrl_blk->curr_state].state_handler)
    {
      (*t31_state_tbl[t31_ctrl_blk->curr_state].state_handler)();
    }
  }
  else
  {
    if (t31_ctrl_blk->flags.tps_flowctl_asserted == FALSE)
    {
      (void) dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);
    }
  }
} /* dst31_process_uplink */


/*=============================================================================
** FUNCTION: dst31_process_event
**
** DESCRIPTION:
**   This function is used to perform event processing for events received
**   via GCSDPS during a facsimile data call.  These include, call processing 
**   T.31 AT action commands, and online-command processing events.
**
** PARAMETERS:
**   event_ptr == Pointer to the structure containing the event context.
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_process_event
(
  ds_gcsd_cmd_type *  event_ptr
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Safety net */
  ASSERT(event_ptr);

  DATA_MSG1_HIGH("received %d", event_ptr->hdr.cmd_id);

  switch (event_ptr->hdr.cmd_id)
  {
    case DS_GCSD_CONNECT_MO_REQ:
    case DS_GCSD_CONNECT_MT_REQ:
      if (event_ptr->hdr.cmd_id == DS_GCSD_CONNECT_MO_REQ)
      {
        DATA_MSG0_HIGH("received DS_GCSD_CONNECT_MO_REQ");
      }
      else
      {
        DATA_MSG0_HIGH("received DS_GCSD_CONNECT_MT_REQ");
      }
      
      break;

    case DS_GCSD_DISCONNECT_MO_REQ:
    case DS_GCSD_DISCONNECT_MT_REQ:
      /* GCSDPS received a disconnect request, so stop processing. */

      if (t31_ctrl_blk->tps_ul_wm_ptr != NULL)
      {
        T31_MSG_HIGH("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
          t31_ctrl_blk->tps_ul_wm_ptr->current_cnt,
          t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark,
          t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark);
      }

      /* Call is disconnecting so reset the fax layer */
      dst31i_context_reset(TRUE);
      break;

    case DS_GCSD_CMD_MODE_REQ:
      /* GCSDPS received a notification of a change to online-command mode.
      ** This mode allows command result codes to be sent to the local DTE.
      */
      t31_ctrl_blk->flags.sio_mode_change_pending = FALSE;
      
      /* Since we are now in online-command mode we should have an AT
      ** result code to send to the local DTE.  If so, send it, otherwise
      ** log a message -- something is wrong if we reach this point w/o
      ** having a result code to send.
      */
      /* No final AT result code yet */
      if (t31_ctrl_blk->dsat_result != DSAT_NO_RSP)
      {
        dst31i_send_dsat_result(t31_ctrl_blk->dsat_result,
          DST31_DSAT_RESULT_FINAL);

        t31_ctrl_blk->dsat_result = DSAT_NO_RSP;
      }
      else
      {
        T31_ERR("OLCM entered with no DSAT_RESULT",0,0,0);
      }

      /* Need to keep track of the mode state so save it */
      DST31_SAVE_SIO_PROCESSING_MODE(DST31_SIO_ONLINE_CMD_MODE);
      break;

    case DS_GCSD_RECONNECT_REQ:
      /* GCSDPS received a notification of a change to online-data mode.
      ** This mode allows data to be exchange between the local DTE and
      ** the FPS.
      */
      t31_ctrl_blk->flags.sio_mode_change_pending = FALSE;

      /* Need to keep track of the mode state so save it */
      DST31_SAVE_SIO_PROCESSING_MODE(DST31_SIO_ONLINE_DATA_MODE);
      break;

    case DS_GCSD_T31_AT_FTH_REQ:
      /* ATCOP detected an AT+FTH command, start +FTH processing */
      dst31i_change_state(DST31_FTH_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    case DS_GCSD_T31_AT_FRH_REQ:
      /* ATCOP detected an AT+FRH command, start +FRH processing */

      dst31i_change_state(DST31_FRH_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    case DS_GCSD_T31_AT_FTM_REQ:
      /* ATCOP detected an AT+FTM command, start +FTM processing */

      dst31i_change_state(DST31_FTM_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    case DS_GCSD_T31_AT_FRM_REQ:
      /* ATCOP detected an AT+FRM command, start +FRM processing */

      dst31i_change_state(DST31_FRM_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    case DS_GCSD_T31_AT_FTS_REQ:
      /* ATCOP detected an AT+FTS command, start +FTS processing */

      dst31i_change_state(DST31_FTS_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    case DS_GCSD_T31_AT_FRS_REQ:
      /* ATCOP detected an AT+FRS command, start +FRS processing */

      dst31i_change_state(DST31_FRS_STATE, FALSE);
      t31_ctrl_blk->dsat_callback = event_ptr->cmd.dsat_callback;
      break;

    default:
      T31_ERR("unknown event received, event=%d",
        event_ptr->hdr.cmd_id,0,0);
  } /* switch */
} /* dst31_process_event */


/*=============================================================================
** FUNCTION: dst31_process_signals
**
** DESCRIPTION:
**   This function is used to process a T31 protocol related signals.
**
** PARAMETERS:
**   sig_mask == identifies the signals which need servicing
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_process_signals
(
  rex_sigs_type sig_mask
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  if (sig_mask & DS_GCSD_T31_TIMEOUT_SIG)
  {
    DATA_MSG1_HIGH("TIMER expiration detected, type=%d",
      t31_ctrl_blk->timer_type);

    /* Process the timeout base on which timer was set */
    switch (t31_ctrl_blk->timer_type)
    {
      case DST31_DCN_TIMER_TYPE:
        DATA_MSG0_HIGH("DCN complete, go IDLE");

        t31_ctrl_blk->flags.timeout = FALSE;

        /* Goto IDLE */
        dst31i_change_state(DST31_IDLE_STATE, TRUE);
        break;

      case DST31_CD_TIMER_TYPE:
        DATA_MSG0_HIGH("CD timeout, send CONNECT");

        t31_ctrl_blk->flags.send_connect = FALSE;

        /* Tell the local DTE that we are ready to start processing */
        dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

        /* Start the +FCL timer.  Since we haven't yet received any data this
        ** timer will limit how much longer we will wait for it to come in.
        */
        DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);
        DST31_TIMER_SET("+FCL", t31_ctrl_blk->timer_ptr, 
          t31_ctrl_blk->fcl_timeout_val, DST31_FCL_TIMER_TYPE);
        break;

      /* These are all treated the same -- we just set a flag that the
      ** timeout occurred and state processing will handle the rest.
      */
      case DST31_FIT_TIMER_TYPE:
      case DST31_FCL_TIMER_TYPE:
      case DST31_WFO_TIMER_TYPE:
      case DST31_FTS_TIMER_TYPE:
      case DST31_FRS_TIMER_TYPE:
        t31_ctrl_blk->flags.timeout = TRUE;
        break;

      /* These are BAD. We either had a timer expire that we didn't know about
      ** or an unknown timer type was set -- either case log an ERR but don't 
      ** give up the ship yet.
      */
      case DST31_NO_TIMER_TYPE:
      default:
        DATA_MSG1_ERROR("Unknown/Invalid timer detected, type=%d", 
          t31_ctrl_blk->timer_type);
    } /* switch */
  }
} /* dst31_process_signals */


/*=============================================================================
** FUNCTION: dst31_call_init
**
** DESCRIPTION:
**   This function is used to initialize the necessary configuration to
**   allow fascimile data call processing to begin for both mobile
**   terminated and originated fascimile data calls.
**
** PARAMETERS:
**   call_is_MO == whether or not call is Mobile Originated
**   call_id    == Call Manager call ID
**                
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**===========================================================================*/
void dst31_call_init
(
  boolean             call_is_MO,
  cm_call_id_type     call_id
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Make sure we start the call from a clean slate */
  dst31i_context_reset(FALSE);

  /* Make sure ZBD processing is reset */
  dst31hdlci_zbd_init();

  /* Keep track of whether the call is MT or MO */
  t31_ctrl_blk->call_type = (call_is_MO) ? DST31_MO_FAX_CALL : DST31_MT_FAX_CALL;
  t31_ctrl_blk->call_id = call_id;
  
  /* Clear our timers just to be safe */
  DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

  /* No final AT result code yet */
  t31_ctrl_blk->dsat_result = DSAT_NO_RSP;

  /* Can't send SOP DSAT_CONNECT yet */
  t31_ctrl_blk->flags.first_dsat_connect = TRUE;

  /* By default we will be in online-data mode */
  t31_ctrl_blk->sio_mode = DST31_SIO_ONLINE_DATA_MODE;

  /* Take the +FCL timeout value from DSAT if it exists else default it */
  t31_ctrl_blk->fcl_timeout_val = 
    (t31_ctrl_blk->at_params.fcl > 0) ? t31_ctrl_blk->at_params.fcl :
                                       DST31_FCL_DEFAULT_TIMEOUT;

  /* Take the +FIT timeout value from DSAT if it exists else default it */
  t31_ctrl_blk->fit_timeout_val = 
    (t31_ctrl_blk->at_params.fit[0] > 0) ? t31_ctrl_blk->at_params.fit[0] :
                                          DST31_FIT_DEFAULT_TIMEOUT;

  /* Allow FCLASS=0 upon termination by default */
  t31_ctrl_blk->flags.allow_fclass_0 = TRUE;

  /* Make sure we start in the correct SIO flow control state */
  if (t31_ctrl_blk->flags.sio_flowctl_asserted == TRUE)
  {
    t31_ctrl_blk->flags.sio_flowctl_asserted = FALSE;
  }

  /* Clear the byte counters, we will eventually be entering a new state */
  memset(&t31_ctrl_blk->byte_cnts, 0, sizeof(t31_ctrl_blk->byte_cnts));

  /* Let's indicate that the T31 layer has been initialized */
  t31_ctrl_blk->flags.t31_init = TRUE;

  /* Initialize the TPS watermarks */
  dst31i_tps_watermark_init();

#ifdef ENABLE_GSM_TREX_NETWORK_INTERFACE
  /* This code is here to kick start the FAs */
  dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);
  dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);
#endif

  if (t31_ctrl_blk->call_type == DST31_MT_FAX_CALL)
  {
    DATA_MSG0_HIGH("Call type is MOBILE_TERMINATED");
  }
  else
  {
    DATA_MSG0_HIGH("Call type is MOBILE_ORIGINATED");
  }

  /* Allow the FA to synchronize */
  dst31i_change_state(DST31_SYNC_STATE, FALSE);
} /* dst31i_call_init */


/*===========================================================================
** FUNCTION: dst31_call_connect_init
**
** DESCRIPTION:
**   This function is used to perform any remaining FPS initialization
**   once the fax data call has reached the CALL_CONNECTED state, meaning
**   the traffic channel is ready for data transfers.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/
void dst31_call_connect_init
(
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  DATA_MSG0_LOW("Call CONNECTED initialization");

  /* Now put T31 into the correct starting state based on whether
  ** the call is MO or MT.
  */
  if (t31_ctrl_blk->call_type == DST31_MO_FAX_CALL)
  {
    dst31i_change_state(DST31_FRH_STATE, FALSE);
  }
  else
  {
    dst31i_change_state(DST31_FTH_STATE, FALSE);
  }

 DATA_MSG0_HIGH("Call CONNECTED init complete");
} /* dst31_call_connect_init */

/*===========================================================================
** FUNCTION: DST31_GET_T31_CTRL_BLK
**
** DESCRIPTION:
**   This function is used to return the address of t31_ctrl_blk variable.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   dst31_ctrl_blk_struct_type* - address of variable t31_ctrl_blk
**
** SIDE EFFECTS:
**   None.
**=========================================================================*/

dst31_ctrl_blk_struct_type* dst31_get_t31_ctrl_blk(void)
{
  return &t31_control_blk;
} /* dst31_get_t31_ctrl_blk */

#endif /* FEATURE_DATA_GCSD_FAX */
