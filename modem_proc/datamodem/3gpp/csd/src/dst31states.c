/*=============================================================================
**
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
**   This file contains source code for the state handler functions for the
**   GCSD-FPS T31 Fax Services layer.  These functions are exported for use 
**   solely by T31.
**
** EXTERNALIZED FUNCTIONS USED BY T31 ONLY:
**
**   dst31i_process_idle_state()
**      - performs state machine processing for the
**          DST31_IDLE_STATE state.
**
**   dst31i_process_wait_for_order_state()
**      - performs state machine processing for the
**          DST31_WAIT_FOR_ORDER_STATE state.
**
**   dst31i_process_fth_state()
**      - performs state machine processing for the
**          DST31_FTH_STATE state.
**
**   dst31i_process_frh_state()
**      - performs state machine processing for the
**          DST31_FRH_STATE state.
**
**   dst31i_process_ftm_state()
**      - performs state machine processing for the
**          DST31_FTM_STATE state.
**
**   dst31i_process_frm_state()
**      - performs state machine processing for the
**          DST31_FRM_STATE state.
**
**   dst31i_process_fts_state()
**      - performs state machine processing for the
**          DST31_FTS_STATE state.
**
**   dst31i_process_frs_state()
**      - performs state machine processing for the
**          DST31_FRS_STATE state.
**
** EXTERNALIZED DATA:
**
** INITIALIZATION AND SEQUENCING REQUIREMENTS:
**
**   dst31i_powerup_init() must be called at powerup.
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
   Copyright (c) 2002 - 2009 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31states.c#1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------
04/01/11  TTV   Support for Q6 free floating.
06/29/09  BS    Fixed off-target lint medium & lows.
03/09/09  SA    AU level CMI modifications.
11/15/07  AR    Use safety macros for watermark operations.
02/09/07  DA    Fixed critical lint errors
04/06/06  AR    Replace use of ATCOP globals with control block elements.
02/09/06  AR    Lint corrections.
06/07/04  RLP   Modified WFO state to invoke the FA layer when it is in IDLE to 
                  ensure that it can service the DL watermark to keep it from 
                  filling up.
                Changed some debug messages from MSG_LOW to MSG_HIGH to aid in 
                  field testing.
                LINT cleanup.
05/30/04  AR    Lint corrections.
09/05/03  RLP   Removed unnecessary parameters from function calls to 
                  dst31i_terminate_state().
01/14/03  RLP   Code review updates.
12/20/02  RLP   Modified +FRH state to call dsgcsdfa_is_idle() to detect if the
                  FA layer has entered the IDLE state.
                Added logic to +FRM state to limit how much data is processed 
                  per downlink function call.
11/15/02  RLP   Changes for GCF certification support.
10/21/02  RLP   Nortel IOT updates.  This source file is the final result
                  from Stage 2 testing.
10/09/02  TMR   Updated for new Multi-mode architecture (FEATURE_DATA_MM)
09/11/02  RLP   MMO2 IOT changes.
07/18/02  RLP   Additional changes to maintain TREX compatibilty.
                  - change +FTS/+FRS processing to enter online-data mode
07/18/02  RLP   Additional changes to maintain TREX compatibilty.
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/26/02  RLP   Added ERR messages to log when a session is terminated and why.
06/25/02  RLP   Replaced dsm_pushdown calls with dst31i_dsm_pushdown wrapper
                  function calls.
06/24/02  RLP   Added in dsm_pushdown error checking which was left out when
                  first pass merge was done.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX

#include "amssassert.h"
#include "dsm.h"
#include <stringl/stringl.h>
#include "rex.h"
#include "err.h"
#include "data_msg.h"
#include "crc.h"
#include "dstask_v.h"
#include "dsgcsdfa.h"
#include "dst31.h"
#include "dst31i.h"
#include "dst31hdlci.h"
#include "ds3gsiolib_ex.h"

#include "ds3gsiolib.h"

#ifdef ENABLE_GSM_TREX_NETWORK_INTERFACE
#include "tpsnetutils.h"
#endif

/*=============================================================================
** REGIONAL VARIABLES
**===========================================================================*/
LOCAL uint8 hdlc_flags[] =
{
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
  DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG, DST31_HDLC_FLAG,
};

/* T31 State Names used for debugging */
static char *dst31_state_names[] =
{
  "IDLE",
  "SYNC",
  " WFO",
  "+FTH",
  "+FRH",
  "+FTM",
  "+FRM",
  "+FTS",
  "+FRS",
  "DST31_MAX_STATES"
};

/*=============================================================================
** REGIONAL FUNCTIONS
**===========================================================================*/

/*=============================================================================
** EXTERNALIZED FUNCTIONS FOR USE SOLEY BY T31
**===========================================================================*/

/*=============================================================================
** FUNCTION: dst31i_process_state_reentrancy
**
** DESCRIPTION:
**   This function is used to perform state reentrancy processing.  This is
**   used by all T31 states to check for protocol timeouts, command aborts,
**   and proper mode of operation.
**
** PARAMETERS:
**   curr_state == current T31 processing state
**
** RETURNS:
**   boolean == returns TRUE if reentrancy to the state is ok to proceed,
**              FALSE otherwise
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
LOCAL boolean dst31i_process_state_reentrancy
(
  dst31_state_enum_type curr_state
)
{
  boolean reentrancy_ok = TRUE; /* assume success for now */
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Based on the current state, check to see if state processing is
  ** is ok to continue.  NOTE:  states are grouped together based on
  ** similar reentrancy requirements.
  */
  switch (curr_state)
  {
    case DST31_IDLE_STATE:
    case DST31_SYNC_STATE:
      /* Although this should never happen, it doesn't mean anything
      ** any critical errors have occurred so we let it slide...
      */
      break;

    case DST31_WAIT_FOR_ORDER_STATE:
      if (t31_ctrl_blk->flags.timeout == TRUE)
      {
        DATA_MSG0_HIGH("+FIT timer expired!");

        /* If we are prohibited from setting FCLASS=0 then stop it */
        if (t31_ctrl_blk->at_params.fit[1] == 1)
        {
          t31_ctrl_blk->flags.allow_fclass_0 = FALSE;
        }

        /* No response from the DTE so kill the call */
        dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);

        reentrancy_ok = FALSE; 
      }

      break;

    case DST31_FTH_STATE:
    case DST31_FTM_STATE:
      if (t31_ctrl_blk->flags.timeout == TRUE)
      {
        DATA_MSG0_HIGH("+FIT timer expired!");

        /* If we are prohibited from setting FCLASS=0 then stop it */
        if (t31_ctrl_blk->at_params.fit[1] == 1)
        {
          t31_ctrl_blk->flags.allow_fclass_0 = FALSE;
        }

        /* State processing is completed */
        dst31i_terminate_state(DSAT_OK, TRUE);

        reentrancy_ok = FALSE; 
      }
      else if (!DST31_SIO_IN_ONLINE_DATA_MODE())
      /* Need to wait until we are notified that SIO is in online-data mode */
      {
        if (t31_ctrl_blk->flags.sio_mode_change_pending == FALSE)
        {
          /* The request to change modes hasn't been made yet so let's
          ** go ahead and tell DSMGR to make the switch.
          */
          dst31i_request_sio_mode_change(DST31_SIO_ONLINE_DATA_MODE);
        }

        /* We still have to let the FA do its thing even with no data */
        (void) dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);

        DATA_MSG0_LOW( "Wait for ONLINE_DATA mode"); 

        reentrancy_ok = FALSE; 
      } 
      break;

    case DST31_FRH_STATE:
    case DST31_FRM_STATE:
      if (t31_ctrl_blk->flags.timeout || t31_ctrl_blk->flags.at_cmd_abort)
      {
        if (t31_ctrl_blk->flags.timeout == TRUE)
        {
          DATA_MSG0_HIGH("+FCL timer expired!");
        }
        else
        {
          DATA_MSG0_HIGH("Command aborted by DTE");
        }

        /* State processing is completed */
        dst31i_terminate_state(DSAT_OK, TRUE);
        
        reentrancy_ok = FALSE; 
      }
      else if (!DST31_SIO_IN_ONLINE_DATA_MODE())
      /* Need to wait until we are notified that SIO is in online-data mode */
      {
        if (t31_ctrl_blk->flags.sio_mode_change_pending == FALSE)
        {
          /* The request to change modes hasn't been made yet so let's
          ** go ahead and tell DSMGR to make the switch.
          */
          dst31i_request_sio_mode_change(DST31_SIO_ONLINE_DATA_MODE);
        }

        DATA_MSG0_LOW("Wait for ONLINE_DATA mode");

        reentrancy_ok = FALSE; 
      }
      break;

    case DST31_FTS_STATE:
    case DST31_FRS_STATE:
      if (!DST31_SIO_IN_ONLINE_DATA_MODE())
      {
        if (t31_ctrl_blk->flags.sio_mode_change_pending == FALSE)
        {
          /* The request to change modes hasn't been made yet so let's
          ** go ahead and tell DSMGR to make the switch.
          */
          dst31i_request_sio_mode_change(DST31_SIO_ONLINE_DATA_MODE);
        }

        DATA_MSG0_LOW("Wait for ONLINE_DATA mode");

        reentrancy_ok = FALSE;
      }
      else if (curr_state == DST31_FRS_STATE)
      {
        if (t31_ctrl_blk->flags.at_cmd_abort == TRUE)
        {
          DATA_MSG0_HIGH( "Command aborted by DTE");

          /* State processing is completed */
          dst31i_terminate_state(DSAT_OK, TRUE);
        
          reentrancy_ok = FALSE;
        }
      }

      break;

    default: 
     DATA_MSG1_ERROR("Invalid state=%d", curr_state);
  } /* switch */

  return (reentrancy_ok);
} /* dst31i_process_state_reentrancy */


/*=============================================================================
** FUNCTION: dst31i_process_idle_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_IDLE_STATE state.  In this state there is no active facsimile
**   data call, thus no attempt should be made to invoke the FPS.
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
void dst31i_process_idle_state
(
  void
)
{
  /* Everything required for this state is handled in the dst31i_context_reset
  ** routine. No additional processing is required.  This state handler simply
  ** exists for completeness of the API.
  */
  DATA_MSG0_ERROR( "dst31i_process_idle_state() erroneously accessed!");
} /* dst31i_process_idle_state */


/*=============================================================================
** FUNCTION: dst31i_process_sync_state
**
** DESCRIPTION:
**   This function is used to allow the FA layer to synchronize with the
**   remote FA.  Once synchronization has taken place, the FA will initiate
**   the call CONNECT response back to the local TE which will start normal
**   fax processing.
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
void dst31i_process_sync_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_SYNC_STATE) == FALSE)
  {
    return;
  }

  /* Since this is a shared state between UL/DL processing we need to see
  ** which direction we are currently processing for and call the appropriate
  ** FA function.
  */
  if (t31_ctrl_blk->ota_path == DST31_OTA_DOWNLINK)
  {
    dsm_item_type *         dl_data_ptr = NULL;
    ds_fa_data_enum_type    dl_data_type;
    ds_fa_status_enum_type  fa_stat;

    /* Give the FA a chance to run */
    fa_stat = dsgcsdfa_process_downlink(&dl_data_ptr, &dl_data_type);
  
    /* We shouldn't be getting downlink data in this state */
    if (dl_data_ptr)
    { 
      if ((t31_ctrl_blk->call_type == DST31_MO_FAX_CALL) &&
          (dl_data_type == DS_FA_DATA_BCS))
      {
        /* In this case the remote FA is already sending us HDLC
        ** data so we need to go ahead and get to the correct state
        ** to start processing it...we don't really want to dump it.
        */
        dsm_append(&t31_ctrl_blk->pending_dl_data_ptr,&dl_data_ptr);

        DATA_MSG1_LOW("SYNC::hold pending data, cnt=%d", 
          dsm_length_packet(t31_ctrl_blk->pending_dl_data_ptr));
      }
      else
      {
        DATA_MSG2_HIGH("Unexpected RX data in SYNC state-- Data deleted, cnt=%d type=%d",
                      dsm_length_packet(dl_data_ptr),dl_data_type);
#if 0
        (void) dst31i_dump_dsm_item("T31", dl_data_ptr);
#endif
        dsm_free_packet(&dl_data_ptr);
      }
    }

    /* Always check the outcome of the FA function */
    if (fa_stat == DS_FA_FAILURE)
    {
      DATA_MSG0_ERROR("FA failure during downlink");

      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
  }
  else
  {
    /* While in this state we need to keep calling the FA uplink so
    ** the remote FA can stay happy.
    */
    if (dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE) == DS_FA_FAILURE)
    {
      DATA_MSG0_ERROR("FA failure during downlink");

      /* Major problems - abort the session */
      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
  }
} /* dst31i_process_sync_state */


/*=============================================================================
** FUNCTION: dst31i_process_wait_for_order_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_WAIT_FOR_ORDER_STATE state.  T31 will enter this state after
**   it has completed processing of an ITU-T T.31 AT command to await the 
**   next AT command or call event to process.  
**
**   T31 will continue to invoke the FA layer with NULL data to allow the FA
**   to stay synchronized with the remote FA during uplink processing.  The
**   FA is NOT invoked during downlink processing as T31 would not know what
**   to do with any received data.  T31 has to wait for an AT command to 
**   instruct it as to what state it should be in.  Since the FA will change
**   states before T31, the data is left queued in the TPS watermark until
**   T31 changes to a state where the data can be processed.
**
**   T31 is also responsible for ensuring that the FA is in its IDLE state
**   when it enters this state.  The +FIT timer is used as a safeguard to 
**   ensure that T31 receives some indication from the DTE within a reasonable
**   amount of time.
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
void dst31i_process_wait_for_order_state
(
  void
)
{
  ds_fa_status_enum_type  fa_stat = DS_FA_SUCCESS;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();


  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_WAIT_FOR_ORDER_STATE) == FALSE)
  {
    return;
  }

  /* Since this is a shared state between UL/DL processing we need to see
  ** which direction we are currently processing for and call the appropriate
  ** FA function.
  */
  if (t31_ctrl_blk->ota_path == DST31_OTA_UPLINK)
  {
    if (t31_ctrl_blk->flags.tps_flowctl_asserted == FALSE)
    {
      /* All we need to do here is to let the FA run so it can stay synchronized
      ** with the remote side.  We don't have any data to give to it since we are
      ** waiting for the local TE to tell us what to do next.
      */
      fa_stat = dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE);
    }
  }
  else
  {
    /* On downlink processing, we need to invoke the FA layer only when it is
    ** is in the IDLE state to keep the watermark clear.  If the FA layer is not
    ** in the IDLE state we need to let whatever data has arrived to queue up
    ** in the watermark while we wait for the local fax appl to instruct us as
    ** to what to do next.
    */
    if (dsgcsdfa_is_idle() == TRUE)
    {
      /* Since the FA layer is in IDLE, it will NOT return any data.  These
      ** variables are defined nonetheless because the API expects them.
      */
      dsm_item_type *         fa_data_ptr = NULL;
      ds_fa_data_enum_type    fa_data_type;
      ds_fa_status_enum_type  fa_stat2;
  
      /* Give the FA a chance to run */
      fa_stat2 = dsgcsdfa_process_downlink(&fa_data_ptr, &fa_data_type);
  
      /* Do we need to free any packets? */
      if (fa_data_ptr != NULL)
      {
        if (fa_data_type != DS_FA_DATA_NONE)
        {
          DATA_MSG1_ERROR( "Freeing RX data in FA_IDLE_STATE, type=%d", fa_data_type);
        }

        dsm_free_packet(&fa_data_ptr);
      }

      /* Always check the outcome of the FA function */
      if (fa_stat2 == DS_FA_FAILURE)
      {
        DATA_MSG0_ERROR("FA failure during downlink");

        dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
      }
    }
  }

  /* If there was a FA failure, we quit */
  if (fa_stat == DS_FA_FAILURE)
  {
    DATA_MSG0_ERROR("WFO::FA failure detected");

    /* Major problems - abort the session */
    dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
  }
} /* dst31i_process_wait_for_order_state */


/*=============================================================================
** FUNCTION: dst31i_process_fth_transmit_frame
**
** DESCRIPTION:
**   This function is used to transmit a complete HDLC frame to the remote
**   FA.  To alleviate the possibility of using too much processing time to
**   send a large frame, the function breaks the frame down into smaller 
**   fragments and sends each fragment separately.  Only one fragment is
**   sent per uplink processing invocation.
**
** PARAMETERS:
**   data_state == DST31_FTH_XMIT_FLAGS when the opening flag sequence is sent 
**                 DST31_FTH_XMIT_DATA when actual frame data is sent
**
** RETURNS:
**   DST31_FRAME_COMPLETE if the frame is valid
**   DST31_FRAME_CORRUPT if the frame is invalid
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
LOCAL dst31_frame_status_enum_type dst31i_process_fth_transmit_frame
(
  uint8 data_state
)
{
  static struct dst31_io_data_s *   io_data_ptr;
  static struct dst31_byte_cnts_s * byte_cnts_ptr;
  dsm_item_type *                   ul_data_ptr = NULL;
  ds_fa_status_enum_type            fa_stat = DS_FA_SUCCESS;
  dst31_frame_status_enum_type      frame_stat = DST31_FRAME_READY;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;

  t31_ctrl_blk = dst31_get_t31_ctrl_blk();
  io_data_ptr = &(t31_ctrl_blk->io_data);
  byte_cnts_ptr = &(t31_ctrl_blk->byte_cnts);

  /* Before we transmit the frame we need to transmit some opening
  ** HDLC flags, so queue some up.
  */
  if (data_state == DST31_FTH_XMIT_FLAGS)
  {
    if (dst31i_dsm_pushdown(&ul_data_ptr, hdlc_flags, 
          DST31_FTH_MAX_OPENING_FLAGS, DSM_DS_SMALL_ITEM_POOL) == TRUE)
    {
      /* Keep track of the byte count */
      byte_cnts_ptr->total_tx_hdlc_bytes += DST31_FTH_MAX_OPENING_FLAGS;
    }
    else
    {
      frame_stat = DST31_FRAME_CORRUPT;
    }
  }
  else
  {
    uint16                            ul_byte_cnt;

    /* We are going to transmit the frame in pieces.  This is because
    ** for every 1 byte in the frame it translates into 32 bytes in the
    ** FA layer (@9600bps) and a large frame can cause TX overruns.
    */
    ul_byte_cnt = MIN(DST31_FTH_MAX_BYTES_PER_20MS, io_data_ptr->cnt);

    DATA_MSG1_HIGH( "+FTH::send %d HDLC frame bytes", ul_byte_cnt);
    if( t31_ctrl_blk->tps_ul_wm_ptr )
    {
      T31_MSG_HIGH("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                   t31_ctrl_blk->tps_ul_wm_ptr->current_cnt,
                   t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark,
                   t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark);
    }

    /* Now queue up the frame fragment */
    if (dst31i_dsm_pushdown(&ul_data_ptr, 
          &io_data_ptr->zbuf[byte_cnts_ptr->fth_byte_cnt], 
          ul_byte_cnt, DSM_DS_SMALL_ITEM_POOL) == TRUE)
    {
      /* Make sure to count the header/trailer flags in the total */
      io_data_ptr->cnt -= ul_byte_cnt;
      byte_cnts_ptr->fth_byte_cnt += ul_byte_cnt;

      /* Keep track of the byte count */
      t31_ctrl_blk->byte_cnts.total_tx_hdlc_bytes += ul_byte_cnt;

      /* If we have reached the end of the frame the reset our context flag */
      if (io_data_ptr->cnt == 0)
      {
        t31_ctrl_blk->flags.fth_frame_complete = FALSE;
        frame_stat = DST31_FRAME_COMPLETE;
      }
    }
  }

  /* Now if we have some ready to transmit we can send on to the FA.
  ** If we have completed sending a frame we need send a result code
  ** back to the TE indicating our current processing status.  If this
  ** wasn't the last frame of the sequence we should expect more frames
  ** to follow, otherwise, we can go back to the WFO state.
  */
#if 0 /* Debug use only */
        /* This allows the uplink data to be dumped for debug */
        (void) dst31i_dump_dsm_item("T31-FTH-FROM-SIO", ul_data_ptr);
#endif

  fa_stat = dsgcsdfa_process_uplink(ul_data_ptr, DS_FA_DATA_BCS);

  if ((fa_stat == DS_FA_FAILURE) && (frame_stat != DST31_FRAME_CORRUPT))
  {
    frame_stat = DST31_FRAME_CORRUPT;
  }

  return (frame_stat);
} /* dst31i_process_fth_transmit_frame */


/*=============================================================================
** FUNCTION: dst31i_process_fth_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_FTH_STATE state (transmit T.30 HDLC data).  Data in this state
**   is received from the local DTE and transmitted OTA.
**
**   The state is initially entered when the DTE initiates an AT+FTH command.
**   T31 will then remain in this state until it successfully completes all
**   FTH processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Upon entering the state, T31 will send a 'CONNECT' response to the DTE
**   to indicate that it is ready (carrier is detected) to receive data and
**   will then initiate a return to online-data mode.
**
**   In this state, T31 will be receiving HDLC data frames from the DTE via 
**   the SIO RX watermark for OTA transmission.  It is very unlikely that T31
**   will receive a complete HDLC frame at one time.  Thus, T31 is responsible
**   for constructing a complete HDLC from the data being received during 
**   multiple iterations of this function.
**
**   The DTE may send 1 or more HDLC frames during execution of a single 
**   AT+FTH command.  T31 is responsible for receiving and building individual
**   HDLC frames from data.  T31 will pass one complete frame at a time to the
**   FA layer as they are processed.
**
**   HDLC data will be received in the following format:
**
**      HDLC Frame = <address byte><control byte><data bytes><DLE><ETX>
**
**   (Note:  HDLC data is received lsb first for each byte.)
**
**   The address byte is always 0xFF for a facsimile data call..  The control
**   byte is either 0x03 or 0x13.  The control byte is used to indicate whether
**   or not additional frames will follow (0x13 indicates no more frames).
**   The <DLE><ETX> byte combination is used to separate individual frames when
**   multiple frames are being transmitted.  T31 must use the control field to
**   determine when the last frame has been received.  T31 is not concerned with
**   the content of the data bytes in any manner and the data is added to the 
**   frame unchecked.
**
**   For each frame received, T31 must perform the following additional 
**   operations on the frame before sending it to the FA for OTA transmission:
**
**      1.  Decode the data (DLE filtering) as specified in ITU-T T.31 6.11.1.
**      2.  Compute and append the FCS to each frame.
**      3.  Perform HDLC zero-bit insertion on the data.
**      4.  Prepend an opening HDLC FLAG byte.
**      5.  Append a closing HDLC FLAG byte.
**
**   At this point T31 will take the completed frame and will pass it off 
**   to the FA layer for OTA transmission.
*
**   It is possible for TPS flow-control to be asserted and if so, T31 will
**   not process any incoming HDLC data to allow the TPS to clear its queue.
**
**   When the last frame is passed to the FA, or if an error should occur,
**   T31 will send a final result code to the DTE and will goto the 
**   WAIT_FOR_ORDER state to await the next command or event.
**
**   Refer to dst31i_process_frh_state() below for the complement function
**   to receive HDLC data from OTA.
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
void dst31i_process_fth_state
(
  void
)
{
  ds_fa_status_enum_type       fa_stat = DS_FA_SUCCESS;
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FTH_STATE) == FALSE)
  {
    return;
  }

  /* Do we need to send a CONNECT to the local TE? */
  if ((t31_ctrl_blk->flags.send_connect == TRUE) &&
      (dsgcsdfa_tx_ready() == TRUE))
  {
    /* Tell the local DTE that we are ready to start processing */
    dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

    t31_ctrl_blk->flags.send_connect = FALSE;
  }

  /* If we have a frame ready to be transmitted then we need to get it
  ** sent out.  However, if we try to queue the whole frame at once we
  ** can overload the FA layer and cause TX overruns/underruns which is
  ** bad.  Thus, the frame will be sent out in smaller fragments through
  ** multiple invocations of this function.  When the entire frame has
  ** been sent, SIO processing will resume.
  */
  if (t31_ctrl_blk->flags.fth_frame_complete == TRUE)
  {
    /* Send a frame fragment and check the result */
    frame_stat = dst31i_process_fth_transmit_frame(DST31_FTH_XMIT_DATA);

    /* If we receive a complete frame we need to respond back to the DTE
    ** to keep things moving.
    */
    if (frame_stat == DST31_FRAME_COMPLETE)
    {
      if (t31_ctrl_blk->flags.fth_last_frame == TRUE)
      {
        /* No more data expected in this state and the FA is done processing
        ** the queued data so it is ok to return to WFO to await the next
        ** command. However, first tell DSAT that the async command has 
        ** completed.
        */
        T31_MSG_HIGH("+FTH::complete TX_bytes=%d",
          t31_ctrl_blk->byte_cnts.total_tx_hdlc_bytes,0,0);

        /* Let's go ahead and put the FA back to IDLE since we know we are
        ** done with the current frame and no more data will be coming.
        */
        fa_stat = dsgcsdfa_return_to_idle(FALSE);

        /* State processing is completed */
        dst31i_terminate_state(DSAT_OK, TRUE);
      }
      else
      {
        /* More frames are coming, send a CONNECT to tell the DTE its
        ** ok to send the next frame.
        */
        dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);
      }

      /* Make sure we start with a clean data buffer before we
      ** we start receiving SIO/TPS data and building data frames
      */
      t31_ctrl_blk->byte_cnts.fth_byte_cnt = 0;
      DST31_RESET_IO_BUFFER();
    }
    else if (frame_stat == DST31_FRAME_CORRUPT)
    {        
      /* State processing is completed */
      dst31i_terminate_state(DSAT_OK, TRUE);
    }

    return;
  }

  /* Since we got this far we don't have a complete frame ready to be 
  ** transmitted.  So we need to start building a frame from whatever 
  ** data we have received from SIO.  However, if TPS flow-control has
  ** been asserted then the lower layers need some time to get the 
  ** backlog of uplink data transmitted -- the we will hold off on
  ** pulling any data from SIO to allow the TPS time to get caugt up.
  */
  if (t31_ctrl_blk->flags.tps_flowctl_asserted == FALSE)
  {
    ds_fa_data_enum_type    frame_type = DS_FA_DATA_NONE;
    dsm_item_type *         sio_data_ptr = NULL;
    dsm_item_type *         ul_data_ptr = NULL;


    /* We have to process pending data first before reading any new
    ** data off the watermark.  But first, if this is the first frame
    ** of a sequence we have to send the preamble first.  We send it 
    ** in pieces so we don't overload the FA layer.
    */
    if (t31_ctrl_blk->flags.fth_preamble_active == TRUE)
    {
      uint16 flag_cnt;

      flag_cnt = MIN(DST31_FTH_MAX_PREAMBLE_SIZE, DST31_FTH_MAX_BYTES_PER_20MS); /*lint !e506*/

      if (t31_ctrl_blk->byte_cnts.fth_preamble_cnt < DST31_FTH_MAX_PREAMBLE_SIZE)
      {
        (void) dst31i_dsm_pushdown(&ul_data_ptr, hdlc_flags, flag_cnt,
                 DSM_DS_SMALL_ITEM_POOL);

        /* This is BCS data to the FA */
        frame_type = DS_FA_DATA_BCS;

        DATA_MSG2_LOW("+FTH: send %d pre-flags, cnt=%d",
          flag_cnt,t31_ctrl_blk->byte_cnts.fth_preamble_cnt);

        t31_ctrl_blk->byte_cnts.fth_byte_cnt += flag_cnt;
        t31_ctrl_blk->byte_cnts.fth_preamble_cnt += flag_cnt;
        t31_ctrl_blk->byte_cnts.sio_page_bytes += flag_cnt;
      }
      else
      {
        t31_ctrl_blk->flags.fth_preamble_active = FALSE;
        t31_ctrl_blk->byte_cnts.fth_byte_cnt = 0;
        t31_ctrl_blk->byte_cnts.fth_preamble_cnt = 0;
        DATA_MSG0_HIGH("+FTH: preamble complete");
      }
    }
    else if (t31_ctrl_blk->pending_ul_data_ptr)
    {
      sio_data_ptr = t31_ctrl_blk->pending_ul_data_ptr;
      t31_ctrl_blk->pending_ul_data_ptr = NULL;

      DATA_MSG1_LOW("+FTH::process pending data, cnt=%d", 
        dsm_length_packet(sio_data_ptr));
    }
    else
    {
      /* Read the SIO queue and grab any pending data */
      sio_data_ptr = DSM_DEQUEUE( t31_ctrl_blk->sio_ul_wm_ptr );
    }

    /* If we are finished processing the preamble we can start
    ** pulling data from SIO and building the HDLC frame.  Otherwise,
    ** we simply pass the preamble frames off to the FA as is.
    */
    if (t31_ctrl_blk->flags.fth_preamble_active == FALSE)
    {
      /* Now, if we read any data we can reset the +FIT timer and 
      ** try to build a message frame.  In all cases, we always have
      ** to call the FA to allow it to performs its own UL processing.
      */
      if (sio_data_ptr && (sio_data_ptr->used > 0))
      {
        uint16 ul_byte_cnt;
        uint32 dsm_byte_cnt;

#if 0 /* Debug use only */
        /* This allows the uplink data to be dumped for debug */
        (void) dst31i_dump_dsm_item("T31-FTH-FROM-SIO", sio_data_ptr);
#endif
        /* Got data from the DTE so we can clear the +FIT timer for now */
        DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

        /* We finally got some data so we can clear our special processing
        ** flag if it is still set.
        */
        if (t31_ctrl_blk->flags.wait_for_first_data_byte == TRUE)
        {
          t31_ctrl_blk->flags.wait_for_first_data_byte = FALSE;
        }

        /* We don't want to chew up too much processing time so we have
        ** to limit how much message data we process for one iteration.
        */
        dsm_byte_cnt = dsm_length_packet(sio_data_ptr);
        ul_byte_cnt = (uint16)MIN(DST31_FTH_MAX_BYTES_PER_20MS,
                          dsm_byte_cnt);

        DATA_MSG1_LOW("+FTH::process %d data bytes",ul_byte_cnt);

        frame_stat = dst31i_build_ul_hdlc_frame(&sio_data_ptr, ul_byte_cnt);

        /* If there is still data pending, we need to hold it and process
        ** it on the next iteration before removing new data from the queue
        */
        if (sio_data_ptr && (sio_data_ptr->used > 0))
        {
          t31_ctrl_blk->pending_ul_data_ptr = sio_data_ptr;

          DATA_MSG2_LOW("+FTH::hold pending data, cnt=%d fs=%d",
            dsm_length_packet(sio_data_ptr),frame_stat);
        }
        else
        {
          /* We are done with the packet so free it */
          dsm_free_packet(&sio_data_ptr);
        }

        /* Number of bytes processed from SIO */
        t31_ctrl_blk->byte_cnts.sio_page_bytes += ul_byte_cnt;

        /* If we have a complete frame ready to be transmitted then we
        ** need to send out the opening flag sequence and then send the
        ** frame in fragments.
        */
        if (frame_stat == DST31_FRAME_COMPLETE)
        {
          t31_ctrl_blk->flags.fth_frame_complete = TRUE;

          DATA_MSG2_HIGH("+FTH::send frame OTA, cnt=%d lf=%d",
            t31_ctrl_blk->io_data.cnt, 
            t31_ctrl_blk->flags.fth_last_frame);

          /* Send the frame opening flag sequence */
          (void) dst31i_process_fth_transmit_frame(DST31_FTH_XMIT_FLAGS);
        }
        else if (frame_stat & (DST31_FRAME_INCOMPLETE | DST31_FRAME_READY)) /*lint !e655 */
        {
          /* We are still waiting to build a complete frame.  To keep the
          ** remote side happy we will fill the time by sending an HDLC flag.
          */
          DATA_MSG0_HIGH( "+FTH::wait on frame, send flag!");

          (void) dst31i_dsm_pushdown(&ul_data_ptr, hdlc_flags, 1,
                   DSM_DS_SMALL_ITEM_POOL);

          /* Keep track of the byte count */
          t31_ctrl_blk->byte_cnts.total_tx_hdlc_bytes++;
          frame_type = DS_FA_DATA_BCS;
        }
        else if (frame_stat == DST31_FRAME_CORRUPT)
        {
          T31_ERR("+FTH::bad frame, toss %d byte(s), buf[0]=0x%x",
            t31_ctrl_blk->io_data.cnt,
            t31_ctrl_blk->io_data.buf[0], 0);

#if 1 /* Debug use only */
          /* This allows the uplink data to be dumped for debug */
          (void) dst31i_dump_dsm_item("T31-FTH-TO-UL", ul_data_ptr);
#endif
        }
 
        /* Go ahead and reset the +FIT timer. If no more data is coming it will
        ** be cleared on the state transition.
        */
        DST31_TIMER_RESET("+FIT", t31_ctrl_blk->timer_ptr, 
          t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);
      }
      /* NO SIO data received yet */
      else if (t31_ctrl_blk->flags.wait_for_first_data_byte == TRUE)
      {
        /* Here is where we will send the HDLC flag stream ... this is done 
        ** whenever we have to wait for the initial data stream from the DTE
        ** to start coming in.
        */
        DATA_MSG0_HIGH( "+FTH::no SIO data, send HDLC flag!");

        (void) dst31i_dsm_pushdown(&ul_data_ptr, hdlc_flags, 1,
                 DSM_DS_SMALL_ITEM_POOL);

        /* Keep track of the byte count */
        t31_ctrl_blk->byte_cnts.total_tx_hdlc_bytes++;

        frame_stat = DST31_FRAME_INCOMPLETE;
        frame_type = DS_FA_DATA_BCS;
      }

#if 0 /* Debug use only */
      /* This allows the uplink data to be dumped for debug */
      if (frame_stat == DST31_FRAME_COMPLETE)
      {
        (void) dst31i_dump_dsm_item("T31-FTH-TO-UL", ul_data_ptr);
      }
#endif
    }

    /* So now we invoke the FA to send out whatever data we currently have */
    fa_stat = dsgcsdfa_process_uplink(ul_data_ptr, frame_type);

    /* If we have detected any bad data or the FA had errors bail out */
    if (frame_stat != DST31_FRAME_CORRUPT)
    {
      dst31i_check_fa_status(fa_stat);
    }
    else
    {
      /* State processing is completed */
      dst31i_terminate_state(DSAT_ERROR, TRUE);
    }
  } /* if flow-control */
  else
  {
    /* Since flow-control is asserted we can reset the +FIT timer since
    ** we had to hold off reading any data from SIO -- we don't count that
    ** time against us.
    */
    DST31_TIMER_RESET("+FIT", t31_ctrl_blk->timer_ptr,
      t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);
  }
} /* dst31i_process_fth_state */


/*=============================================================================
** FUNCTION: dst31i_process_frh_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_FRH_STATE state (receive T.30 HDLC data).  Data in this state
**   is received OTA from the TPS and transmitted to the local DTE.
**
**   The state is initially entered when the DTE initiates an AT+FRH command.
**   T31 will then remain in this state until it successfully completes all
**   FRH processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Upon entering the state, T31 will send a 'CONNECT' response to the DTE
**   to indicate that it is ready to begin sending data to the DTE.and will
**   then initiate a return to online-data mode.
**
**   In this state, T31 will be receiving HDLC data frames from the FA via 
**   the TPS downlink watermark.  It is very unlikely that T31 will receive 
**   a complete HDLC frame at one time.  Thus, T31 is responsible for 
**   constructing a complete HDLC from the data being received during 
**   multiple iterations of this function.
**
**   When receiving HDLC frames, the DTE will initiate a separate AT+FRH
**   command for each individual frame.  T31 is responsible for building
**   and formatting each frame from the received OTA data and sending it
**   on to the DTE.
**
**   HDLC data will be received in the following format:
**
**      HDLC Frame = <address byte><control byte><data bytes><DLE><ETX>
**
**   (Note:  HDLC data is received lsb first for each byte.)
**
**   The address byte is always 0xFF for a facsimile data call..  The control
**   byte is either 0x03 or 0x13.  The control byte is used to indicate whether
**   or not additional frames will follow (0x13 indicates no more frames).
**   The <DLE><ETX> byte combination is used to separate individual frames when
**   multiple frames are being transmitted.  T31 must use the control field to
**   determine when the last frame has been received.  T31 is not concerned with
**   the content of the data bytes in any manner and the data is added to the 
**   frame unchecked.
**
**   For each frame received, T31 must perform the following additional 
**   operations on the frame before sending it to the DTE:
**
**      1.  Strip opening and closing HDCL FLAGs.
**      2.  Perform HDLC zero-bit deletion on the data.
**      3.  Decode the data (DLE filtering) as specified in ITU-T T.31 6.11.1.
**      2.  Compute the FCS from the received data.
**            a.  If the data was received ok, send an 'OK' response.
**            b.  If the data was received in error, send an 'ERROR' response.
**
**   At this point T31 will take the completed frame and will transmit it to
**   the DTE following it with the appropriate response code.  T31 will then
**   goto the WAIT_FOR_ORDER state to await the next command or event.
**
**   Refer to dst31i_process_fth_state() above for the complement function
**   to transmit HDLC data OTA.
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
void dst31i_process_frh_state
(
  void
)
{
  boolean                      last_frame = FALSE;
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  dsm_item_type *              dl_data_ptr = NULL;
  ds_fa_data_enum_type         dl_data_type = DS_FA_DATA_BCS;
  ds_fa_status_enum_type       fa_stat = DS_FA_SUCCESS;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FRH_STATE) == FALSE)
  {
    return;
  }

  /* Do we need to send a CONNECT to the local TE? */
  if ((t31_ctrl_blk->flags.send_connect == TRUE) &&
      (t31_ctrl_blk->carrier_detected == TRUE))
  {
    t31_ctrl_blk->flags.send_connect = FALSE;

    /* We are now receiving data so we can clear the CD timer if set */
    if (t31_ctrl_blk->timer_type == DST31_CD_TIMER_TYPE)
    {
      DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);
    }

    /* Tell the local DTE that we are ready to start processing */
    dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);
  }

  /* We have to process pending data first before reading any new
  ** data off the watermark.
  */
  if (t31_ctrl_blk->pending_dl_data_ptr != NULL)
  {
    dl_data_ptr = t31_ctrl_blk->pending_dl_data_ptr;
    t31_ctrl_blk->pending_dl_data_ptr = NULL;

    DATA_MSG1_LOW( "+FRH::process pending data, cnt=%d", dl_data_ptr->used);
  }
  else
  {
    /* Check for data on the watermark */
    fa_stat = dsgcsdfa_process_downlink(&dl_data_ptr, &dl_data_type);
  }

#if 0 /* Debug use only */
  /* This allows the downlink data to be dumped for debug */
  if (dl_data_ptr)
  {
    (void) dst31i_dump_dsm_item("T31-FRH-FROM-DL", dl_data_ptr);
  }
#endif

  if ((fa_stat == DS_FA_FAILURE) || (dl_data_type == DS_FA_DATA_MSG))
  {
    /* We are done with the packet so free it */
    dsm_free_packet(&dl_data_ptr);
  
    if (fa_stat == DS_FA_FAILURE)
    {
      DATA_MSG0_ERROR("+FRH::FA failure detected");

      /* Major problems - abort the session */
      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
    else
    {
      DATA_MSG0_ERROR( "+FRH::Invalid page data, discarded");

      /* State processing is completed */
      dst31i_terminate_state(DSAT_ERROR, TRUE);
    }
  }
  else 
  {
    uint8                      fcs_stat = DST31_FCS_CALC_PENDING;

    ds3g_siolib_status_e_type  sio_stat = DS3G_SIOLIB_OK;

    /* If we are waiting for the FA to return to IDLE then we already
    ** have completed FRH processing.  What we are doing here is allowing
    ** any trailing data (i.e. closing flags) to arrive and be tossed
    ** before exiting the state.
    */
    if (t31_ctrl_blk->flags.frh_wait_for_fa_idle == TRUE)
    {
      if (dl_data_ptr && (dl_data_ptr->used > 0))
      { 
        DATA_MSG1_LOW("Dumping +FRH data, cnt=%d",
          dsm_length_packet(dl_data_ptr));
        dsm_free_packet(&dl_data_ptr);
      }

      if ((fa_stat == DS_FA_SWITCHING_TO_IDLE) || (dsgcsdfa_is_idle() == TRUE))
      {
        DATA_MSG0_HIGH("FA going IDLE, Exit +FRH state");

        t31_ctrl_blk->flags.frh_wait_for_fa_idle = FALSE;

        dst31i_terminate_state(DSAT_OK, FALSE);
      }

      return;  
    }

    /* So now we either received some data and/or the FA is returning
    ** to IDLE meaning it has completed its receive process and we
    ** shouldn't expect any more HDLC data.  If we do have data we need
    ** to build the frame.  If the FA is done, then we go back to the
    ** WAIT_FOR_ORDER state and await the next directive.  If we don't
    ** have data or we don't have a complete frame we keep sending a
    ** stream of HDLC flags to keep the remote side happy.
    */
    if (dl_data_ptr && (dl_data_ptr->used > 0))
    {
      dsm_item_type *       sio_data_ptr = NULL;

      /* Got data from the FA so we can clear the +FCL timer for now */
      DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

      /* Build the DL data frame to send to the local DTE via SIO.  It is
      ** possible for more than one HDLC frame to be received and each one
      ** must be individually transmitted to the DTE.  The P/B bit is used
      ** to determine when the final frame has been received.  
      */
      frame_stat = dst31i_build_dl_hdlc_frame(&dl_data_ptr, 
                     (uint16)dsm_length_packet(dl_data_ptr), &sio_data_ptr,
                     &last_frame, &fcs_stat);


      /* If this is the last frame we need to make sure that we reset the
      ** ZBD routine to be ready for the next +FRH command.  We also need
      ** to clear out the watermark so there isn't any extraneous HDLC data
      ** waiting in the queue (closing flags...).
      */
      if (last_frame == TRUE)
      {
        /* Reset ZBD processing for the next frame */
        dst31hdlci_zbd_init();

        /* Clear out the TPS DL watermark */
        EMPTY_WATERMARK( t31_ctrl_blk->tps_dl_wm_ptr );
      }

      /* If we have a complete frame or an FCS failure go ahead and queue
      ** the data for delivery to the DTE.  The appropriate result code 
      ** will be sent below.
      */
      if ((frame_stat == DST31_FRAME_COMPLETE) ||
          (fcs_stat == DST31_FCS_CALC_DONE_FAIL))
      {
        DATA_MSG2_HIGH("+FRH::frame_sz=%d lf=%d",
            t31_ctrl_blk->io_data.cnt,last_frame);

        /* It is quite possible that a small frame can be received and process
        ** in its entirety during this function call and the CONNECT has yet to
        ** be sent to the local TE.  If so, we need to send the CONNECT response
        ** first before sending the frame or the frame data will get tossed by
        ** the receiving application.
        */
        if (t31_ctrl_blk->flags.send_connect == TRUE)
        {
          dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

          t31_ctrl_blk->flags.send_connect = FALSE;
        }

#if 0 /* Debug use only */
        /* This allows the downlink data to be dumped for debug */
        (void) dst31i_dump_dsm_item("T31-FRH-TO-SIO", sio_data_ptr);
#endif
        /* If the frame is OK let the FA do its thing with it */
        if (fcs_stat == DST31_FCS_CALC_DONE_PASS)
        {
          (void) dsgcsdfa_process_bcs_frame(t31_ctrl_blk->io_data.buf,
                   t31_ctrl_blk->io_data.cnt, FALSE);
        }

        /* Pass the complete frame off to SIO */
        if (sio_data_ptr && (sio_data_ptr->used > 0))
        {
         sio_stat = ds3g_siolib_ex_setup_tx(sio_data_ptr, FALSE,
                                             ds3g_siolib_ex_get_csd_port());
        }

        /* Make sure we start with a clean data buffer before we
        ** we start receiving SIO/TPS data and building data frames
        */
        DST31_RESET_IO_BUFFER();
      }

      /* If there is still data pending, we need to hold it and process
      ** it on the next iteration before removing new data from the queue
      */
      if (dl_data_ptr && (dl_data_ptr->used > 0))
      {
        t31_ctrl_blk->pending_dl_data_ptr = dl_data_ptr;

        DATA_MSG1_LOW("+FRH::hold pending data, cnt=%d",
                dl_data_ptr->used);
      }
      else
      {
        /* We are done with the packet so free it */
        dsm_free_packet(&dl_data_ptr);
      }

      /* Go ahead and reset the +FCL timer. If no more data is coming it will
      ** be cleared on the state transition.
      */
      DST31_TIMER_RESET("+FCL", t31_ctrl_blk->timer_ptr, 
        t31_ctrl_blk->fcl_timeout_val, DST31_FCL_TIMER_TYPE);
    }

    /* Now check the status of what just happened.  If we couldn't get the data
    ** queued to SIO -- that is a bigtime error...as is a FCS failure.  In the
    ** latter we go ahead and send the frame but indicate an ERROR.  The DTE is
    ** responsible for handling error recovery.  If the FA is going IDLE or 
    ** we have just completed receiving a complete frame then send an OK 
    ** response and wait for the next command in WFO.
    */
    if ((sio_stat != DS3G_SIOLIB_OK) || (fcs_stat == DST31_FCS_CALC_DONE_FAIL))
    
    {
      if (sio_stat != DS3G_SIOLIB_OK)
      {
        DATA_MSG0_ERROR("+FRH::SIO TX error to DTE");
      }
      else
      {
        T31_ERR("+FRH::FCS failure!",0,0,0);
      }

      /* State processing is completed */
      dst31i_terminate_state(DSAT_ERROR, TRUE);
    } 
    else if ((fa_stat == DS_FA_SWITCHING_TO_IDLE) || 
             (frame_stat == DST31_FRAME_COMPLETE))
    {
      if (frame_stat == DST31_FRAME_COMPLETE)
      {
        DATA_MSG1_HIGH("+FRH::complete RX_bytes=%d",
             t31_ctrl_blk->byte_cnts.total_rx_hdlc_bytes);

        /* State processing is completed - on the last frame we need to 
        ** wait for the FA layer to go to IDLE to ensure all trailing
        ** HDLC data (i.e. flags) are pulled up and removed
        */
        if ((last_frame == TRUE) && (fa_stat != DS_FA_SWITCHING_TO_IDLE))
        {
          DATA_MSG0_HIGH("+FRH: Waiting on FA to go IDLE");
          t31_ctrl_blk->flags.frh_wait_for_fa_idle = TRUE;
        }
        else
        {
          dst31i_terminate_state(DSAT_OK, FALSE);
        }
      }
      else
      {
        dsat_result_enum_type       dsat_result;

        /* The FA is going IDLE and we don't have a complete frame so
        ** it's going to get tossed.  We treat this as an error to the DTE.
        */
        DATA_MSG1_HIGH("+FRH: toss bad frame, cnt=%d",
                 t31_ctrl_blk->io_data.cnt);

        dsat_result = (dsm_length_packet(dl_data_ptr) == 0)
                        ? DSAT_OK : DSAT_ERROR;
 
        DATA_MSG2_LOW("Dumping +FRH data, cnt=%d rc=%d",
          dsm_length_packet(dl_data_ptr),dsat_result);
        dsm_free_packet(&dl_data_ptr);

        dst31i_terminate_state(dsat_result, FALSE);

        DST31_RESET_IO_BUFFER();
      }
    }

    /* The only thing left is that we haven't received a complete frame so we
    ** will remain in this state and expect to receive more data from the DTE
    ** so we can complete the frame.
    */
  }
} /* dst31i_process_frh_state */


/*=============================================================================
** FUNCTION: dst31i_process_ftm_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_FTM_STATE state (transmit T.30 message data).  Data in this
**   state is received from the local DTE and transmitted OTA.
**
**   The state is initially entered when the DTE initiates an AT+FTM command.
**   T31 will then remain in this state until it successfully completes all
**   FTM processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Upon entering the state, T31 will send a 'CONNECT' response to the DTE
**   to indicate that it is ready (carrier is detected) to receive data and
**   will then initiate a return to online-data mode.
**
**   In this state, T31 will be receiving message data from the DTE via the
**   SIO RX watermark for OTA transmission.  There are no frame boundaries 
**   with message data as there are with HDLC data so the data can be passed
**   to the FA as soon as it is received.  The only exception to this is that
**   data filtering must be performed.  If it happens that a <DLE> is detected
**   as the last byte in the current buffer, the data will be held until the
**   next iteration so filtering can be completed.  If this happens, the FA
**   is still invoked with NULL data so it can stay synchronized with the 
**   remote FA.
**
**   Message data will be received in the following format:
**
**      Message data =<data bytes><DLE><ETX>
**
**   (Note:  Message data is received lsb first for each byte.)
**
**   The <DLE><ETX> byte combination is used to indicate the end of the message
**   byte sequence and subsequently when the end-of-state processing has been
**   reached.
**
**   All data must be decoded (DLE filtered) prior to sending it OTA.
**
**   AT+FTM processing has the following caveats that T31 must adhere to:
**
**     1.  If the transmit buffer becomes empty and the last transmitted
**         character was a NUL, T31 must continue to send NULs until the
**         DTE sends more data or 5 seconds elapses.  If no data received
**         before the timeout interval expires, T31 will send an 'ERROR'
**         result to the DTE and will goto the WAIT_FOR_ORDER state to
**         await the next command or event.
**
**     2.  If the transmit buffer becomes empty and the last transmitted
**         character was not a NUL, T31 will send an 'OK' result to the DTE
**         and will goto the WAIT_FOR_ORDER state to await the next command
**         or event.
**
**   Refer to dst31i_process_frm_state() below for the complement function
**   to receive message data from OTA.
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
void dst31i_process_ftm_state
(
  void
)
{
  static uint8 ones_bytes[] = {0xFF,0xFF,0xFF,0xFF};
  static uint8 null_bytes[] = {0x00,0x00,0x00,0x00};

  ds_fa_status_enum_type  fa_stat = DS_FA_SUCCESS;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FTM_STATE) == FALSE) 
  {
    return;
  }

  /* Do we need to send a CONNECT to the local TE? */
  if (t31_ctrl_blk->flags.send_connect == TRUE)
  {
    /* Tell the local DTE that we are ready to start processing */
    dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

    t31_ctrl_blk->flags.send_connect = FALSE;
  }

  /* The FA layer may not be ready to transmit data yet because the it has
  ** to wait for the remote side to enter the correct state.  If it is not
  ** ready we will go ahead and assert flow control so we don't create a
  ** bottle neck while waiting.
  */
  if ((dsgcsdfa_tx_ready() == TRUE) || DST31_T30_TRAINING_PHASE_ACTIVE())
  {
    DST31_SIO_FLOWCTL_DEASSERT();
  }
  else
  {
    DST31_SIO_FLOWCTL_ASSERT();
  }
     
  /* Can only queue up data if flow-control is not asserted.  However we still
  ** have to call the FA to allow it to get any pending data out which is the 
  ** only way flow control will be de-asserted.
  */
  if (t31_ctrl_blk->flags.tps_flowctl_asserted == FALSE)
  {
    dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
    uint16                       tx_byte_cnt = 0;
    dsm_item_type *              sio_data_ptr;
    dsm_item_type *              ul_data_ptr = NULL;
    ds_fa_data_enum_type         ul_data_type = DS_FA_DATA_MSG;


    /* We have to process pending data first before reading any new
    ** data off the watermark.
    */
    if (t31_ctrl_blk->pending_ul_data_ptr)
    {
      sio_data_ptr = t31_ctrl_blk->pending_ul_data_ptr;
      t31_ctrl_blk->pending_ul_data_ptr = NULL;

      DATA_MSG1_LOW("+FTM::process pending data, cnt=%d",
               sio_data_ptr->used);
    }
    else
    {
      /* Read the SIO queue to grab any pending data */
      sio_data_ptr = DSM_DEQUEUE( t31_ctrl_blk->sio_ul_wm_ptr );
    }

    /* Now, if we read any data we can reset the +FIT timer and 
    ** try to build a message frame.  In all cases, we always have
    ** to call the FA to allow it to performs its own UL processing.
    */
    if (sio_data_ptr && (sio_data_ptr->used > 0))
    {
      uint16 ul_byte_cnt;
      uint32 dsm_byte_cnt;

#if 0 /* Debug use only */
      /* This allows the uplink data to be dumped for debug */
      (void) dst31i_dump_dsm_item("T31-FTM-FROM-SIO", sio_data_ptr);
#endif

      /* Got data from the FA so we can clear the +FIT timer for now */
      DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

      /* We don't want to chew up too much processing time so we have
      ** to limit how much message data we process for one iteration.
      */
      dsm_byte_cnt = dsm_length_packet(sio_data_ptr);
      ul_byte_cnt = (uint16)MIN(DST31_FTM_MAX_BYTES_PER_20MS,
                        dsm_byte_cnt);

      /* Ok we got some data so lets package it up for OTA */
      frame_stat = dst31i_build_ul_msg_frame(&sio_data_ptr,
        ul_byte_cnt, &ul_data_ptr);

      /* If there is still data pending, we need to hold it and process
      ** it on the next iteration before removing new data from the queue
      */
      if ((frame_stat != DST31_FRAME_CORRUPT) &&
          (sio_data_ptr && (sio_data_ptr->used > 0)))
      {
        t31_ctrl_blk->pending_ul_data_ptr = sio_data_ptr;

        DATA_MSG1_LOW("+FTM::hold pending data, cnt=%d",
         dsm_length_packet(sio_data_ptr));
      }
      else
      {
        /* We are done with the packet so free it */
        dsm_free_packet(&sio_data_ptr);
      }

      /* We finally got some data so we can clear our special processing
      ** flag if it is still set.
      */
      if (t31_ctrl_blk->flags.wait_for_first_data_byte == TRUE)
      {
        t31_ctrl_blk->flags.wait_for_first_data_byte = FALSE;
      }

      /* Number of bytes processed from SIO */
      t31_ctrl_blk->byte_cnts.sio_page_bytes += ul_byte_cnt;

      /* Number of bytes to be transmitted */
      tx_byte_cnt = t31_ctrl_blk->io_data.cnt;

      /* Go ahead and reset the +FIT timer. If no more data is coming it will
      ** be cleared on the state transition.
      */
      DST31_TIMER_RESET("+FIT", t31_ctrl_blk->timer_ptr, 
        t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);
    }
    else if (t31_ctrl_blk->flags.wait_for_first_data_byte == TRUE)
    {
      /* Ok, so we are still waiting for the first data byte from 
      ** the DTE...while we wait we have to send (on behalf of the DTE)
      ** a stream of '1' bits to the remote side...so let's queue some
      ** up while we wait.
      */
      DATA_MSG0_LOW("+FTM::no SIO data, send '1' bits");

      tx_byte_cnt = sizeof(ones_bytes);

      (void) dst31i_dsm_pushdown(&ul_data_ptr, ones_bytes, tx_byte_cnt,
               DSM_DS_SMALL_ITEM_POOL);

      /* Keep track of the byte count */
      t31_ctrl_blk->byte_cnts.total_tx_msg_bytes += tx_byte_cnt;
      t31_ctrl_blk->byte_cnts.other_bytes += tx_byte_cnt;

      frame_stat = DST31_FRAME_READY;
    } /* if (sio_data_ptr) */

    /* As per T.31 when processing in the +FTM state, if we don't have any
    ** data to send at the moment and the last character we transmitted was
    ** a NULL, we have to then keep sending NULLs for 5secs or until we get
    ** more data from the DTE.  If the last character was not a NULL, then
    ** we terminate state processing and wait for the next command.
    */
    if (frame_stat & (DST31_FRAME_EMPTY | DST31_FRAME_CORRUPT)) /*lint !e655 */
    {
      if (frame_stat & DST31_FRAME_EMPTY) /*lint !e655 */
      {
        if (t31_ctrl_blk->io_data.last_ftm_tx_char == NULL)
        {
          DATA_MSG0_LOW("+FTM::no SIO data, send '0' bits");

          /* Ok, no data, so keep sending NULLs */
          tx_byte_cnt = sizeof(null_bytes);

          (void) dst31i_dsm_pushdown(&ul_data_ptr, null_bytes, tx_byte_cnt,
                   DSM_DS_SMALL_ITEM_POOL);

          /* Keep track of the byte count */
          t31_ctrl_blk->byte_cnts.total_tx_msg_bytes += tx_byte_cnt;
          t31_ctrl_blk->byte_cnts.other_bytes += tx_byte_cnt;

          frame_stat = DST31_FRAME_READY;
        }
        else
        {
          /* State processing is completed */
          dst31i_terminate_state(DSAT_ERROR, TRUE);
        }
      }
      else
      {
        DATA_MSG0_ERROR("+FTM::corrupt message frame");
      }
    }

    /* So if we have data send it on its way, otherwise bail out. */
    if (frame_stat & (DST31_FRAME_READY | DST31_FRAME_COMPLETE)) /*lint !e655 */
    {
      /* It is possible to not actually have any data if we receive
      ** a message complete indication after training is performed so
      ** if so make sure we tell the FA.
      */
      if (ul_data_ptr && (ul_data_ptr->used == 0))
      {
        ul_data_ptr = NULL;
        ul_data_type = DS_FA_DATA_NONE;
      }
      else
      {
        /* Save the last character to be transmitted in case the DTE does
        ** not have any data ready on the next iteration.
        */
        t31_ctrl_blk->io_data.last_ftm_tx_char = 
          t31_ctrl_blk->io_data.buf[t31_ctrl_blk->io_data.cnt];

        /* Keep track of byte cnt from the DTE */
        t31_ctrl_blk->byte_cnts.fa_page_bytes += dsm_length_packet(ul_data_ptr);
      }

#if 0 /* Debug use only */
      /* This allows the uplink data to be dumped for debug */
      (void) dst31i_dump_dsm_item("T31-FTM-TO-UL", ul_data_ptr);
#endif

      DATA_MSG1_LOW("+FTM::send %d fax data byte(s)",
                     dsm_length_packet(ul_data_ptr));
      if( t31_ctrl_blk->tps_ul_wm_ptr )
      {
        T31_MSG_DETAIL("TPS-UL:Qcnt=%d Qhi=%d Qlo=%d", 
                       t31_ctrl_blk->tps_ul_wm_ptr->current_cnt,
                       t31_ctrl_blk->tps_ul_wm_ptr->hi_watermark,
                       t31_ctrl_blk->tps_ul_wm_ptr->lo_watermark);
      }

      /* We have a complete data packet to pass to the FA so package the data
      ** up and send it on its way. 
      */
#ifdef ENABLE_GSM_TREX_NETWORK_INTERFACE
#if 0
      if (t31_ctrl_blk->byte_cnts.page_cnt >= 1)
      {
        tpsinet_log_tx_page_data(ul_data_ptr);
      }
#endif
#endif

      fa_stat = dsgcsdfa_process_uplink(ul_data_ptr, ul_data_type);

      /* If there is more message data to come we need to reset the
      ** IO buffer so we can start receiving the new data.
      */
      if (frame_stat == DST31_FRAME_READY)
      {
        DST31_CLR_IO_BUFFER();
      }
    }
    
    /* Ok if we are done processing for this state send the result
    ** code and check on out.
    */
    if (frame_stat & (DST31_FRAME_EMPTY | DST31_FRAME_COMPLETE)) /*lint !e655 */
    {
      /* This check will skip the training phase data */
      if (t31_ctrl_blk->byte_cnts.page_cnt > 0)
      {
        T31_MSG_HIGH("+FTM::page %d complete, fs=%d",
          t31_ctrl_blk->byte_cnts.page_cnt-1,frame_stat,0);
      }

      t31_ctrl_blk->byte_cnts.page_cnt++;
        
      DATA_MSG4_LOW("+FTM::SIO_byte_cnt=%d, +FTM::DLEs removed=%d, \
                                 +FTM::Bytes  added=%d, +FTM::OTA_byte_cnt=%d ",
                                 t31_ctrl_blk->byte_cnts.sio_page_bytes,
                                 t31_ctrl_blk->byte_cnts.dle_bytes+2,
                                 t31_ctrl_blk->byte_cnts.other_bytes,
                                 t31_ctrl_blk->byte_cnts.fa_page_bytes);

      /* State processing is completed */
      dst31i_terminate_state(DSAT_OK, TRUE);
    }
  }
  else
  {
    /* Since flow-control is asserted we can reset the +FIT timer since
    ** we had to hold off reading any data from SIO -- we don't count that
    ** time against us.
    */
    DST31_TIMER_RESET("+FIT", t31_ctrl_blk->timer_ptr,
      t31_ctrl_blk->fit_timeout_val, DST31_FIT_TIMER_TYPE);
  }

  /* Check the FA return status before leaving.  This could result in
  ** a state change.
  */
  dst31i_check_fa_status(fa_stat);
} /* dst31i_process_ftm_state */


/*=============================================================================
** FUNCTION: dst31i_process_frm_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters 
**   the DST31_FRM_STATE state (receive T.30 message data).  Data in this state
**   is received OTA from the TPS and transmitted to the local DTE.
**
**   The state is initially entered when the DTE initiates an AT+FRM command.
**   T31 will then remain in this state until it successfully completes all
**   FRM processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Upon entering the state, T31 will send a 'CONNECT' response to the DTE
**   to indicate that it is ready to begin sending data to the DTE.and will
**   then initiate a return to online-data mode.
**
**   In this state, T31 will be receiving message data from the FA via the
**   TPS downlink watermark.  There are no frame boundaries  with message data
**   as there are with HDLC data so the data can be transmitted to the DTE as
**   soon as it is received.  The only exception to this is that data filtering
**   must be performed.  If it happens that a <DLE> is detected as the last byte
**   in the current buffer, the data will be held until the next iteration so
**   filtering can be completed.
**
**   Message data will be received in the following format:
**
**      Message data = <data bytes>
**
**   (Note:  Message data is received lsb first for each byte.)
**
**   Since no direct indication is transmitted as part of the message data to
**   indicate where message processing ends, T31 relies on the FA to tell it
**   when it has reached the end of the message data.  This occurs when the
**   FA enters its IDLE state.  In this state, the FA has completed receiving
**   data from the remote FA which implies to T31 that no more data is coming.
**   At this point T31 will process any remaining message data and will then
**   go to the WAIT_FOR_ORDER state.
**
**   All data must be decoded (DLE filtered) prior to transmitting it to the
**   local DTE.
**
**   When state processing has been completed, T31 will send a 'NO_CARRIER' 
**   result to the DTE and will goto the WAIT_FOR_ORDER state to await the
**   next command or event.
**
**   Refer to dst31i_process_ftm_state() above for the complement function
**   to transmit message data OTA.
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
void dst31i_process_frm_state
(
  void
)
{
  dsm_item_type *              dl_data_ptr = NULL;
  ds_fa_data_enum_type         dl_data_type = DS_FA_DATA_MSG;
  ds_fa_status_enum_type       fa_stat = DS_FA_SUCCESS;
  dst31_frame_status_enum_type frame_stat = DST31_FRAME_INCOMPLETE;
  uint16                       dl_byte_cnt;
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FRM_STATE) == FALSE)
  {
    return;
  }

  /* We have to process pending data first before reading any new
  ** data off the watermark.
  */
  if (t31_ctrl_blk->pending_dl_data_ptr)
  {
    dl_data_ptr = t31_ctrl_blk->pending_dl_data_ptr;
    t31_ctrl_blk->pending_dl_data_ptr = NULL;

    DATA_MSG1_LOW( "+FRM::process pending data, cnt=%d", dl_data_ptr->used);
  }
  else
  {
    /* Check for data on the watermark */
    fa_stat = dsgcsdfa_process_downlink(&dl_data_ptr, &dl_data_type);
  }

  if ((fa_stat != DS_FA_FAILURE) || (dl_data_type == DS_FA_DATA_MSG))
  {
    /* We either have data to process or we reached the end of the data
    ** stream.  In either case we have stuff to do.
    */
    if (dl_data_ptr || (fa_stat == DS_FA_SWITCHING_TO_IDLE))
    {
      dsm_item_type *     sio_data_ptr = NULL;

      /* Got data from the FA so we can clear the +FCL timer for now */
      DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);

      /* If we have data then we need to process it.  If we are at the
      ** end of the data stream then append the <DLE><ETX>.  Ship all
      ** data off to SIO when ready.
      */
      if (dl_data_ptr && (dl_data_ptr->used > 0))
      {
        uint32 dsm_byte_cnt;

#if 0 /* Debug use only */
        /* This allows the downlink data to be dumped for debug */
        (void) dst31i_dump_dsm_item("T31-FRM-FROM-DL", dl_data_ptr);
#endif
        /* We hold off on sending a CONNECT to the local TE until we 
        ** actually have started to receive data from the remote end.  
        ** This actually helps with the response timing back to the TE.
        ** If we don't ever get any data, the TE will time out and abort
        ** the current receive command.
        */
        if (t31_ctrl_blk->flags.send_connect == TRUE)
        {
          t31_ctrl_blk->flags.send_connect = FALSE;

          /* We are now receiving data so we can clear the CD timer if set */
          if (t31_ctrl_blk->timer_type == DST31_CD_TIMER_TYPE)
          {
            DST31_TIMER_CLR(t31_ctrl_blk->timer_ptr);
          }

          /* Tell the local DTE that we are ready to start processing */
          dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);
        }

        /* We don't want to chew up too much processing time so we have
        ** to limit how much message data we process for one iteration.
        */
        dsm_byte_cnt = dsm_length_packet(dl_data_ptr);
        dl_byte_cnt = (uint16)MIN(DST31_FRM_MAX_BYTES_PER_20MS,
                          dsm_byte_cnt);

        t31_ctrl_blk->byte_cnts.fa_page_bytes += dl_byte_cnt;

        /* Build the DL data frame to send to the local DTE via SIO.  The data
        ** is received from the FA and concatenated into the local buffer. The
        ** end of the data stream is signaled when the FA returns a 
        ** DS_FA_SWITCHING_TO_IDLE indication.  A <DLE><ETX> is appended to the
        ** data and it is then queued to SIO for delivery to the local DTE.
        */
        frame_stat = dst31i_build_dl_msg_frame(&dl_data_ptr, dl_byte_cnt,
                       &sio_data_ptr);

        /* If there is still data pending, we need to hold it and process
        ** it on the next iteration before removing new data from the queue
        */
        dl_byte_cnt = (uint16)dsm_length_packet(dl_data_ptr);

        if (dl_byte_cnt > 0)
        {
          t31_ctrl_blk->pending_dl_data_ptr = dl_data_ptr;

          DATA_MSG1_LOW( "+FRM::hold pending data, cnt=%d", dl_byte_cnt);
        }
        else
        {
          /* We are done with the packet so free it */
          dsm_free_packet(&dl_data_ptr);
        }
      }

      /* If the FA is returning back to its IDLE state then we have reached the
      ** end of the incoming fax data stream for the current page.
      */
      if (fa_stat == DS_FA_SWITCHING_TO_IDLE)
      {
        static uint8        hdlc_dle_etx[] = {DST31_DLE_CHAR, DST31_ETX_CHAR};
        static uint8        cnt = sizeof(hdlc_dle_etx);

        /* Terminate the stream so the TE knows where the data ends */
        (void) dst31i_dsm_pushdown_tail(&sio_data_ptr, &hdlc_dle_etx[0],
                 cnt, DSM_DS_SMALL_ITEM_POOL);

        /* Make sure to count the trailer bytes in the total */
        t31_ctrl_blk->io_data.cnt += cnt;
        t31_ctrl_blk->byte_cnts.other_bytes += cnt;

        frame_stat = DST31_FRAME_COMPLETE;
      }

      /* So if we have data send it on its way. */
      if (frame_stat & (DST31_FRAME_READY | DST31_FRAME_COMPLETE)) /*lint !e655 */
      {
        /* Pass the data off to SIO - but only if there is data to send */
        if (sio_data_ptr && (sio_data_ptr->used > 0))
        {
#if 0 /* Debug use only */
          /* This allows the downlink data to be dumped for debug */
          (void) dst31i_dump_dsm_item("T31-FRM-TO-SIO", sio_data_ptr);
#endif

#ifdef ENABLE_GSM_TREX_NETWORK_INTERFACE
#if 0
          if (t31_ctrl_blk->byte_cnts.page_cnt >= 1)
          {
            tpsinet_log_rx_page_data(sio_data_ptr);
          }
#endif
#endif

          DATA_MSG1_LOW( "+FRM::receiving %d fax data bytes", 
            sio_data_ptr->used);

          /* Keep track of byte cnt going to SIO */
          t31_ctrl_blk->byte_cnts.sio_page_bytes += 
            dsm_length_packet(sio_data_ptr);

          /* Queue up the data to SIO */

          if (ds3g_siolib_ex_setup_tx(sio_data_ptr, FALSE,
                           ds3g_siolib_ex_get_csd_port()) != DS3G_SIOLIB_OK)
          {
            DATA_MSG0_ERROR("+FRM::SIO TX error to DTE");
          }
        }

        /* If there is more message data to come we need to clear the
        ** IO buffer so we can start building the page frame.
        */
        DST31_CLR_IO_BUFFER();
      }

      /* If there is still data pending, we need to hold it and process
      ** it on the next iteration before removing new data from the queue
      */
      if (dl_data_ptr && (dl_data_ptr->used > 0))
      {
        t31_ctrl_blk->pending_dl_data_ptr = dl_data_ptr;

        DATA_MSG1_LOW("+FRM::hold pending data, cnt=%d",
              dl_data_ptr->used);
      }
      else
      {
        /* We are done with the packet so free it */
        dsm_free_packet(&dl_data_ptr);
      }

      /* Go ahead and reset the +FCL timer. If no more data is coming it will
      ** be cleared on the state transition.
      */
      DST31_TIMER_RESET("+FCL", t31_ctrl_blk->timer_ptr, 
        t31_ctrl_blk->fcl_timeout_val, DST31_FCL_TIMER_TYPE);
    }

    /* Now the only return code required is NO_CARRIER when there is no
    ** more data.  This is determined by the FA telling us it is switching
    ** back to its IDLE state.
    */
    if (fa_stat == DS_FA_SWITCHING_TO_IDLE)
    {
      /* This check will skip the training phase data */
      if (t31_ctrl_blk->byte_cnts.page_cnt++ > 0)
      {
        DATA_MSG1_HIGH("+FRM::page %d processing complete",
          t31_ctrl_blk->byte_cnts.page_cnt-1);
      }

      DATA_MSG4(MSG_LEGACY_LOW,"+FRM::OTA_byte_cnt=%d, +FRM::DLEs   added=%d, \
                                +FRM::Bytes  added=%d, +FRM::SIO_byte_cnt=%d ",
                                t31_ctrl_blk->byte_cnts.fa_page_bytes,
                                t31_ctrl_blk->byte_cnts.dle_bytes,
                                t31_ctrl_blk->byte_cnts.other_bytes,
                                t31_ctrl_blk->byte_cnts.sio_page_bytes);
 

      /* Notify DSAT that the command has completed. */
      /* State processing is completed */
      dst31i_terminate_state(DSAT_NO_CARRIER, TRUE);
    }
  }
  else
  {
    /* We are done with the packet so free it */
    dsm_free_packet(&dl_data_ptr);

    if (dl_data_type != DS_FA_DATA_MSG)
    {
      DATA_MSG0_ERROR("+FRM::Invalid BCS data rx'd, data discarded");

      /* State processing is completed */
      dst31i_terminate_state(DSAT_ERROR, TRUE);
    }
    else
    {
      DATA_MSG0_ERROR("+FRM::FA failure detected");

      /* Major problems - abort the session */
      dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
    }
  }
} /* dst31i_process_frm_state */


/*=============================================================================
** FUNCTION: dst31i_process_fts_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters
**   the DST31_FTS_STATE state (transmit silence).  No data is transmitted in
**   this state.  It used by the DTE to transmit and 'idle' period.
**
**   The state is initially entered when the DTE initiates an AT+FTS command.
**   T31 will then remain in this state until it successfully completes all
**   FTS processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Processing for this state is very straight forward.  T31 will set a timer
**   for the specified amount of time.  During the silence period, no data will
**   be transmitted.  However, in the uplink case, the FA will still be invoked
**   with NULL data to allow it to stay synchronized with the remote FA.
**
**   Upon expiration of the timer, T31 will send an 'OK' result to the DTE and
**   will goto the WAIT_FOR_ORDER state to await the next command or event.
**
**   Refer to dst31i_process_frs_state() below for the complement function
**   to receive silence.
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
void dst31i_process_fts_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FTS_STATE) == FALSE)
  {
    return;
  }

  /* Do we need to send a CONNECT to the local TE? */
  if (t31_ctrl_blk->flags.send_connect == TRUE)
  {
    /* Tell the local DTE that we are ready to start processing */
    dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

    t31_ctrl_blk->flags.send_connect = FALSE;
  }

  if (t31_ctrl_blk->flags.timeout == TRUE)
  {
    DATA_MSG0_HIGH("+FTS::+FTS timer expired!");

    /* State processing is completed */
    dst31i_terminate_state(DSAT_OK, TRUE);

    return;
  }

  /* Simply invoke the FA layer to basically do nothing while T31 waits for
  ** the timer to expire.
  */
  if (dsgcsdfa_process_uplink(NULL, DS_FA_DATA_NONE) == DS_FA_FAILURE)
  {
    DATA_MSG0_ERROR("+FTS::FA failure detected");

    /* Major problems - abort the session */
    dst31i_terminate_session(DST31_IDLE_STATE, DSAT_ERROR);
  }
} /* dst31i_process_fts_state */


/*=============================================================================
** FUNCTION: dst31i_process_frs_state
**
** DESCRIPTION:
**   This function is used to perform state processing whenever T31 enters
**   the DST31_FRS_STATE state (receive silence).  No data is transmitted in
**   this state.  It used by the DTE to detect a period of silence during the
**   transmission..
**
**   The state is initially entered when the DTE initiates an AT+FRS command.
**   T31 will then remain in this state until it successfully completes all
**   FRS processing, a protocol error occurs, or the call is terminated.  If
**   the call is terminated, T31 will goto the IDLE state, otherwise it will
**   goto the WAIT_FOR_ORDER state.
**
**   Processing for this state is very straight forward.  This state will always
**   successfully detect the specified period of silence.  T31 will set a timer
**   for the specified amount of time and will wait for it to expire.
**
**   Upon expiration of the timer, T31 will send an 'OK' result to the DTE and
**   will goto the WAIT_FOR_ORDER state to await the next command or event.
**
**   Refer to dst31i_process_fts_state() above for the complement function
**   to receive silence.
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
void dst31i_process_frs_state
(
  void
)
{
  dst31_ctrl_blk_struct_type  *t31_ctrl_blk;
  t31_ctrl_blk = dst31_get_t31_ctrl_blk();

  /* Check if current reentrancy is ok to continue.  If not exit out. */
  if (dst31i_process_state_reentrancy(DST31_FRS_STATE) == FALSE)
  {
    return;
  }

  /* Do we need to send a CONNECT to the local TE? */
  if (t31_ctrl_blk->flags.send_connect == TRUE)
  {
    /* Tell the local DTE that we are ready to start processing */
    dst31i_send_dsat_result(DSAT_CONNECT, DST31_DSAT_RESULT_INTERMEDIATE);

    t31_ctrl_blk->flags.send_connect = FALSE;
  }

  if (t31_ctrl_blk->flags.timeout == TRUE)
  {
    DATA_MSG0_HIGH("+FRS::+FRS timer expired!");

    /* State processing is completed */
    dst31i_terminate_state(DSAT_OK, TRUE);
  }
} /* dst31i_process_frs_state */

/*=============================================================================
** FUNCTION: DST31I_GET_DST31_STATE_NAME
**
** DESCRIPTION:
**   This function is used to retrieve the dst31_state_name of particular index.
**
** PARAMETERS:
**   index - index to the array dst31_state_names
**
** RETURNS:
**   char* - the state name
**
** SIDE EFFECTS:
**   None
**===========================================================================*/

char* dst31i_get_dst31_state_name
(
  uint8 index
)
{
  if(index > DST31_MAX_STATES)
    return NULL;
  return dst31_state_names[index];
} /* dst31i_get_dst31_state_name */

#endif /* FEATURE_DATA_GCSD_FAX */
