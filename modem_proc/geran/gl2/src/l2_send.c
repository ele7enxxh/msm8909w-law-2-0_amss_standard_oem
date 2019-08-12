/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2 SEND

GENERAL DESCRIPTION
   This module contains functions that are responsible for filling the fields
   in a message and sending it to L1 or RR.

EXTERNALIZED FUNCTIONS
   l2_send_mdl_error_ind
   l2_send_dl_release_ind
   l2_send_dl_release_cnf
   l2_send_dl_establish_cnf
   l2_send_dl_establish_ind
   l2_send_dl_unit_data_ind
   l2_send_dl_random_access_cnf
   l2_send_dl_suspend_cnf
   l2_send_dl_resume_cnf
   l2_send_dl_reconnect_cnf
   l2_send_dl_data_ind
   l2_send_ph_random_access_req
   l2_send_ph_data_req
   l2_send_dl_cb_block_ind
   l2_send_dl_data_ack_ind

Copyright (c) 2001-2014 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_send.c_v   1.8   05 Jun 2002 10:56:44   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_send.c#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
01/20/12   SJW      New PH_DATA_CNF interface
07/22/11   SJW      Debug update for Cell Broadcast
07/14/11   SJW      Support for Cell Broadcast in Dual SIM mode
10/02/11   SJW      Enhanced support for G+G paging collision
29/10/10   SJW      Added support for Dual SIM
12/09/02   JAC      Added dl_data_ack_ind
10/04/02   JAC      Code reorg
04/30/02   JAC      support for tc field in dl_unit_data_ind
02/28/02   JAC      Logging interface changes
02/20/02   ATM      Merge GS_GSM and GS functions together
12/14/01   JAC      Logged message being sent to L1
                    Commented out messages being sent to CB task
10/19/01   JAC      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include "stringl/stringl.h"
#include "ms.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "rex.h"
#include "timers.h"
#include "gs.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "msg.h"
#include "err.h"
#include "msg.h"
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_msg.h"
#include "l2_send.h"
#include "cfa.h"
#include "l2_log.h"
#include "geran_multi_sim.h"
#include "l2_transmit.h"

#define INVALID_CB_BLOCK_IND_HEADER (0x3F)

/* storage for demultiplexing callback functions */
static dl_data_mux_ind_T demux_callbacks = {{0},NULL,NULL,GERAN_ACCESS_STRATUM_ID_UNDEFINED};

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
static const gs_queue_id_T l2_queue_ids_for_l1[NUM_GERAN_DATA_SPACES] =
                {INITIAL_VALUE_DIFFERENT(GS_QUEUE_LAYER_1,
                                         GS_QUEUE_LAYER_1_2,
                                         GS_QUEUE_LAYER_1_3) };
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*
 *   Function name:  l2_get_l1_queue_id
 *   --------------------------------
 *
 *   Description: Determines the correct queue ID for GL1
 *   ------------
 *
 *
 *   Parameters:  gas_id - access stratum identifier
 *
 *   Returns:     Queue ID
 *   --------
 *
 */
static gs_queue_id_T l2_get_l1_queue_id(const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    return(l2_queue_ids_for_l1[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(GS_QUEUE_LAYER_1);
}

/******************************************************************
 *                       REPEATED SACCH                           *
 ******************************************************************
 * For specifications, see                                        *
 *   - GSM LAYER 1 3GPP 44.004 v6.1.0                             *
 *   - GSM LAYER 2 3GPP 44.006 v6.4.0                             *
 ******************************************************************
 */

#define INVALIDATE_SAPI3_CACHE(pdr) {(pdr).l2_channel_type = DCCH;}

/*
 *   Function name:  l2_sacch_transmit_manager
 *   --------------------------------------------
 *
 *   Description: Determines what uplink SACCH data should be passed to L1
 *   ------------
 *
 *   Parameters:  sapi            - actual SAPI of input buffer
 *                ph_data_req_ptr - original message destined for L1
 *   -----------
 *
 *   Returns:     The actual SAPI encoded in the Layer 2 frame buffer
 *   --------
 */
static SAPI_T l2_sacch_transmit_manager(l2_store_T *l2_data_ptr, SAPI_T sapi, ph_data_req_T *ph_data_req_ptr)
{
  SAPI_T sapi_used = sapi;

  if (ph_data_req_ptr == NULL)
  {
    /* this is an explicit reset call */
    if (sapi == SAPI0)
    {
      l2_data_ptr->l1_has_candidate_buffer = FALSE;
      l2_data_ptr->sacch_repetition_order  = 0;
    }

    INVALIDATE_SAPI3_CACHE(l2_data_ptr->sapi3_cache);
    MSG_GERAN_HIGH_1("R-SACCH: reset Layer 2 info for SAPI%1d", (int)sapi_used);
  }
  else
  {
    boolean send_repeat = FALSE;

    /* L2 now has a chance to transmit **something** on the SACCH */
    if (l2_data_ptr->sapi3_cache.l2_channel_type == SACCH)
    {
      /* SAPI3 data was cached last time, so service it now */
      (void)memscpy(ph_data_req_ptr, sizeof(ph_data_req_T),
                    &(l2_data_ptr->sapi3_cache), sizeof(ph_data_req_T));
      sapi_used = SAPI3;

      /* flag the cache as invalid now that it has been used */
      INVALIDATE_SAPI3_CACHE(l2_data_ptr->sapi3_cache);

      /* manage the T200 timer for SAPI3 */
      if ((l2_data_ptr->sapi3.T200_state        == RUNNING) &&
          (l2_data_ptr->sapi3.T200_channel_type == SACCH))
      {
        /* T200 must have been started by the previous PH_READY_TO_SEND */
        l2_clear_T200_timer(&(l2_data_ptr->sapi3));

        /* next PH_READY_TO_SEND_IND should restart the timer */
        l2_data_ptr->sapi3.T200_start_request = TRUE;
        l2_data_ptr->sapi3.T200_state         = TO_BE_SET;
      }

      MSG_GERAN_HIGH_2("R-SACCH: using cached SAPI3 frame (SAPI=%1d T200=%1d)",
               (int)sapi, (int)l2_data_ptr->sapi3.T200_state);
      /* ensure that this buffer is NOT marked as a repetition candidate */
      sapi = SAPI3;
    }
    else if (l2_data_ptr->sacch_repetition_order == 1)
    {
      /* the SRO has been set by the network, so send a repeated frame */
      if (l2_data_ptr->l1_has_candidate_buffer == TRUE)
      {
        if (sapi == SAPI3)
        {
          /* new data is for SAPI3, but this cannot be serviced now */
          /* cache the new data until the next transmit opportunity */
          (void)memscpy(&(l2_data_ptr->sapi3_cache), sizeof(l2_data_ptr->sapi3_cache),
                        ph_data_req_ptr, sizeof(ph_data_req_T));
          sapi_used = SAPI0;
          MSG_GERAN_HIGH_0("R-SACCH: SAPI3 frame will be cached");
        }

        /* have to repeat the last SAPI0 frame */
        send_repeat = TRUE;

        /* the repetition buffer can only be used once */
        l2_data_ptr->l1_has_candidate_buffer = FALSE;
      }
      else
      {
        MSG_GERAN_HIGH_0("R-SACCH: SRO=1, but no buffer in L1");
      }
    }

    /* finally, set the flag to indicate how L1 should treat this signal */
    if (send_repeat == FALSE)
    {
      /* do not change the input data, but ensure that L1 receives the */
      /* correct indication as to whether to cache this buffer or not  */
      if (sapi == SAPI0)
      {
        ph_data_req_ptr->l2_repetition_info = L2_SET_REPETITION_CANDIDATE;
        l2_data_ptr->l1_has_candidate_buffer = TRUE;
      }
      else
      {
        ph_data_req_ptr->l2_repetition_info = L2_NOT_REPETITION_CANDIDATE;
      }
    }
    else
    {
      /* instruct L1 to repeat its buffered SAPI0 transmission */
      MSG_GERAN_HIGH_0("R-SACCH: SRO=1, request UL repeat to L1");
      ph_data_req_ptr->l2_repetition_info = L2_USE_REPETITION_CANDIDATE;

      /* note that the Layer 2 frame is left intact so that it can be used */
      /* in the event that L1 has changed channels and reset its buffers   */
    }
  }

  return(sapi_used);
}

/*
 *   Function name:  l2_set_sacch_repetition_order
 *   --------------------------------------------
 *
 *   Description: Updates the SRO with latest received on downlink SACCH
 *   ------------
 *
 *   Parameters:  received_sro: value passed by Layer 1 in PH_DATA_IND
 *   -----------
 *
 *   Returns:     Void
 *   --------
 */
void l2_set_sacch_repetition_order(l2_store_T *l2_data_ptr, byte received_sro)
{
  if (received_sro != l2_data_ptr->sacch_repetition_order)
  {
    l2_data_ptr->sacch_repetition_order = received_sro;
    MSG_GERAN_HIGH_1("R-SACCH: SRO changed to %d", (int)received_sro);
  }

  return;
}

/*
 *   Function name:  l2_crc_sacch_failure
 *   --------------------------------------------
 *
 *   Description: Handles a CRC failure indication on SACCH
 *   ------------
 *
 *   Parameters:  Void
 *   -----------
 *
 *   Returns:     Void
 *   --------
 */
void l2_crc_sacch_failure(l2_store_T *l2_data_ptr)
{
  l2_data_ptr->l1_has_candidate_buffer = FALSE;
  l2_data_ptr->sacch_repetition_order  = 0;
  MSG_GERAN_HIGH_0("SACCH CRC failure: L1 buffer reset, treat SRO as 0");

  return;
}


/*
 *   Function name:  l2_send_ph_data_cnf
 *   --------------------------------
 *
 *   Description: Sends PH_DATA_CNF primitive to GSM Layer 1
 *   ------------
 *
 *
 *   Parameters:  cnf_type - type of confirmation
 *                frame_num : FN for which confirmation is being sent
 *
 *   Returns:     VOID
 *   --------
 *
 */

static void l2_send_ph_data_cnf(l2_store_T *l2_data_ptr, ph_data_cnf_type_t cnf_type, uint32 frame_num)
{
  ph_data_cnf_T ph_data_cnf;
  gs_status_T   gs_status;

  ph_data_cnf.message_header.message_set = MS_L2_L1;
  ph_data_cnf.message_header.message_id  = (byte)PH_DATA_CNF;

  PUT_IMH_LEN(sizeof(ph_data_cnf_T) - sizeof(IMH_T),
              &ph_data_cnf.message_header);

  ph_data_cnf.data_type    = cnf_type;
  ph_data_cnf.frame_number = frame_num;
  ph_data_cnf.gas_id       = l2_data_ptr->gas_id;

  gs_status = gs_send_message(l2_get_l1_queue_id(l2_data_ptr->gas_id), &ph_data_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send PH_DATA_CNF message to GL1");
  }

  return;
}

/*
 *   Function name:  l2_send_mdl_error_ind
 *   -------------------------------------
 *
 *   Description: Send the MDL_ERROR_IND to RR
 *   ------------
 *
 *   Parameters:  l2_cause
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_mdl_error_ind(l2_store_T *l2_data_ptr, error_code_T l2_cause)
{
  mdl_error_ind_T mdl_error_ind;
  gs_status_T gs_status;

  mdl_error_ind.message_header.message_set = MS_RR_L2;
  mdl_error_ind.message_header.message_id = (byte)MDL_ERROR_IND;

  PUT_IMH_LEN(sizeof(mdl_error_ind_T) - sizeof(IMH_T),
              &mdl_error_ind.message_header);

  mdl_error_ind.l2_channel_type = l2_data_ptr->current_l2_channel_type;
  mdl_error_ind.SAPI            = l2_data_ptr->current_SAPI;
  mdl_error_ind.error_code      = l2_cause;
  mdl_error_ind.gas_id          = l2_data_ptr->gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &mdl_error_ind, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send MDL_ERROR_IND to RR");
  }

  return;
}


/*
 *   Function name:  l2_send_dl_release_ind
 *   --------------------------------------
 *
 *   Description: Send the DL_RELEASE_IND to RR
 *   ------------
 *
 *   Parameters:  SAPI, release_mode
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_release_ind(l2_store_T *l2_data_ptr, SAPI_T sapi, release_mode_T release_mode)
{
  gs_status_T gs_status;
  dl_release_ind_T dl_release_ind;

  dl_release_ind.message_header.message_set = MS_RR_L2;
  dl_release_ind.message_header.message_id = (byte)DL_RELEASE_IND;

  PUT_IMH_LEN(sizeof(dl_release_ind_T) - sizeof(IMH_T),
              &dl_release_ind.message_header);

  dl_release_ind.SAPI = sapi;
  dl_release_ind.l2_channel_type = l2_data_ptr->current_l2_channel_type;
  dl_release_ind.release_mode = release_mode;
  dl_release_ind.gas_id = l2_data_ptr->gas_id;

  /* reset the transmit manager since this SAPI is being released */
  (void)l2_sacch_transmit_manager(l2_data_ptr, sapi, NULL);

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_release_ind, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_RELEASE_IND message");
  }
}


/*
 *   Function name:  l2_send_dl_release_cnf
 *   --------------------------------------
 *
 *   Description: Send the DL_RELEASE_CNF to RR
 *   ------------
 *
 *   Parameters:  SAPI, release_mode
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_release_cnf(l2_store_T *l2_data_ptr, SAPI_T sapi, release_mode_T release_mode)
{
  gs_status_T gs_status;
  dl_release_cnf_T dl_release_cnf;

  dl_release_cnf.message_header.message_set = MS_RR_L2;
  dl_release_cnf.message_header.message_id = (byte)DL_RELEASE_CNF;

  PUT_IMH_LEN(sizeof(dl_release_cnf_T) - sizeof(IMH_T),
              &dl_release_cnf.message_header);

  dl_release_cnf.SAPI = sapi;
  dl_release_cnf.l2_channel_type = l2_data_ptr->current_l2_channel_type;
  dl_release_cnf.release_mode = release_mode;
  dl_release_cnf.gas_id = l2_data_ptr->gas_id;

  /* reset the transmit manager since this SAPI is being released */
  (void)l2_sacch_transmit_manager(l2_data_ptr, sapi, NULL);

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_release_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_RELEASE_CNF message");
  }

  return;
}

/*
 *   Function name:  l2_send_dl_establish_cnf
 *   ----------------------------------------
 *
 *   Description: Send the DL_ESTABLISH_CNF to RR
 *   ------------
 *
 *   Parameters:   establish_mode
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_establish_cnf(l2_store_T *l2_data_ptr, establish_mode_T mode)
{
  dl_establish_cnf_T dl_establish_cnf;
  gs_status_T        gs_status;

  dl_establish_cnf.message_header.message_set = MS_RR_L2;
  dl_establish_cnf.message_header.message_id = (byte)DL_ESTABLISH_CNF;

  PUT_IMH_LEN(sizeof(dl_establish_cnf_T) - sizeof(IMH_T),
              &dl_establish_cnf.message_header);

  dl_establish_cnf.SAPI           = l2_data_ptr->current_SAPI;
  dl_establish_cnf.establish_mode = mode;
  dl_establish_cnf.gas_id         = l2_data_ptr->gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_establish_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_ESTABLISH_CNF message");
  }

  return;
}

/*
 *   Function name:  l2_send_dl_establish_ind
 *   ----------------------------------------
 *
 *   Description: Send the DL_ESTABLISH_IND to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_establish_ind(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel, boolean for_reestablish)
{
  dl_establish_ind_T dl_establish_ind;
  gs_status_T        gs_status;

  dl_establish_ind.message_header.message_set = MS_RR_L2;
  dl_establish_ind.message_header.message_id = (byte)DL_ESTABLISH_IND;

  PUT_IMH_LEN(sizeof(dl_establish_ind_T) - sizeof(IMH_T),
              &dl_establish_ind.message_header);

  dl_establish_ind.SAPI            = sapi;
  dl_establish_ind.l2_channel_type = channel;
  dl_establish_ind.gas_id          = l2_data_ptr->gas_id;
  dl_establish_ind.reestablishment = for_reestablish;
  dl_establish_ind.num_discarded_msgs = 0;
  if (for_reestablish == TRUE)
  {
    sapi_info_T *sapi_ptr = l2_get_sapi_info(l2_data_ptr, sapi);

    if (sapi_ptr != NULL)
    {
      channel_info_T *chan_ptr = l2_get_channel_info(sapi_ptr, channel);

      if (chan_ptr != NULL)
      {
        /* determine how many messages are about to be discarded on this SAPI */
        dl_establish_ind.num_discarded_msgs += chan_ptr->send_queue.no_of_entries;
      }
    }
  }

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_establish_ind, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_ESTABLISH_IND message");
  }

  return;
}

/*
 *   Function name:  l2_install_demux_callbacks
 *   --------------------------------
 *
 *   Description: Updates the demultiplexer callbacks
 *   ------------
 *
 *   Parameters:  Pointer to received configuration message
 *
 *   Returns:     NULL
 *   --------
 *
 */
l2_store_T * l2_install_demux_callbacks(const dl_data_mux_ind_T *mux_cb_ptr)
{
  if (mux_cb_ptr != NULL)
  {
    (void)memscpy(&demux_callbacks, sizeof(demux_callbacks),
                  mux_cb_ptr, sizeof(dl_data_mux_ind_T));
    MSG_GERAN_HIGH_0("Installed demultiplexer callbacks");
  }

  return(NULL);
}

/*
 *   Function name:  l2_ccch_demultiplexer
 *   --------------------------------
 *
 *   Description: Executes the CCCH multiplexer callbacks
 *   ------------
 *
 *   Parameters:  Pointer to global L2 data context and message data
 *
 *   Returns:     VOID
 *   --------
 *
 */
static void l2_ccch_demultiplexer(l2_store_T *l2_data_ptr, dl_unit_data_ind_T *msg_ptr)
{
  dl_unit_data_ind_T  dest_message[NUM_GERAN_DATA_SPACES];
  dl_unit_data_ind_T *dest_ptr;
  l2_mux_cb_fn_t      ccch_callback;
  gas_id_t            dest_gas_id;
  boolean             data_required;

  ccch_callback = demux_callbacks.ccch_callback;

  for (dest_gas_id = GERAN_ACCESS_STRATUM_ID_1; dest_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; dest_gas_id++)
  {
    if (dest_gas_id == l2_data_ptr->gas_id)
    {
      /* originator of the data must require it */
      data_required = TRUE;
    }
    else if ((ccch_callback != NULL) &&
             (msg_ptr->tc   != 0xFF) &&
             (msg_ptr->l2_channel_type != SACCH))
    {
      /* any other subscriber will be asked if it needs the data via the callback */
      data_required = (*ccch_callback)(L2_MUX_CCCH, msg_ptr->channel_number, dest_gas_id);
    }
    else
    {
      /* no callback specified, or data is not a candidate for sharing */
      data_required = FALSE;
    }

    if (data_required)
    {
      /* separate piece of memory for each signal, otherwise it can be overwritten */
      dest_ptr = &(dest_message[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(dest_gas_id)]);

      /* copy the source data and update the remaining fields */
      memscpy(dest_ptr, sizeof(dl_unit_data_ind_T), msg_ptr, sizeof(dl_unit_data_ind_T));
      dest_ptr->gas_id = dest_gas_id;
      dest_ptr->shared_msg = (boolean)(dest_gas_id != l2_data_ptr->gas_id);

      if (GS_SUCCESS != gs_send_message(GS_QUEUE_RR, dest_ptr, FALSE))
      {
        MSG_ERROR("Unable to send DL_UNIT_DATA_IND message",0,0,0);
      }
    }
  }

  return;
}

/*
 *   Function name:  l2_send_dl_unit_data_ind
 *   ----------------------------------------
 *
 *   Description: Send the DL_UNIT_DATA_IND to RR
 *   ------------
 *
 *   Parameters:  own_paging_block, channel_number
 *   -----------  tc, short_header
 *
 *   Returns:     VOID
 *   --------
 */

void l2_send_dl_unit_data_ind(l2_store_T *l2_data_ptr,
                              boolean     own_paging_block,
                              ARFCN_T     channel_number,
                              byte        tc,
                              boolean     short_header)
{
  dl_unit_data_ind_T dl_unit_data_ind;

  dl_unit_data_ind.message_header.message_set = MS_RR_L2;
  dl_unit_data_ind.message_header.message_id =  (byte)DL_UNIT_DATA_IND;

  PUT_IMH_LEN(sizeof(dl_unit_data_ind_T) - sizeof(IMH_T),
              &(dl_unit_data_ind.message_header));

  dl_unit_data_ind.SAPI = SAPI0;
  dl_unit_data_ind.l2_channel_type = l2_data_ptr->current_l2_channel_type;
  dl_unit_data_ind.own_paging_block = own_paging_block;
  dl_unit_data_ind.channel_number = channel_number;
  dl_unit_data_ind.l3_message_length = l2_data_ptr->l2_segment_length;
  dl_unit_data_ind.short_header = short_header;

  /* TC field is only valid for BCCH */
  if (l2_data_ptr->current_l2_channel_type == BCCH)
  {
    dl_unit_data_ind.tc = tc;
  }
  else
  {
    dl_unit_data_ind.tc = 0xFF;
  }

  if (l2_data_ptr->l2_segment_length <= N201_MAX)
  {
    (void)memscpy(dl_unit_data_ind.layer3_message, sizeof(dl_unit_data_ind.layer3_message),
                  l2_data_ptr->received_l2_segment, l2_data_ptr->l2_segment_length);
    if (l2_data_ptr->l2_segment_length < N201_MAX)
    {
      /* fill unused octets with padding value */
      memset(dl_unit_data_ind.layer3_message + l2_data_ptr->l2_segment_length,
             0x2B, N201_MAX - l2_data_ptr->l2_segment_length);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("L2 segment length %d exceeds max", (int)l2_data_ptr->l2_segment_length);
  }

  /* pass the message to all the subscribed clients */
  l2_ccch_demultiplexer(l2_data_ptr, &dl_unit_data_ind);

  return;
}

/*
 *   Function name:  l2_send_dl_random_access_cnf
 *   --------------------------------------------
 *
 *   Description: Send the DL_RANDOM_ACCESS_CNF to RR
 *   ------------
 *
 *   Parameters:  frame_number,
 *   -----------  random_access_content
 *
 *   Returns:     Pointer to L2 data store
 *   --------
 *
 */
l2_store_T * l2_send_dl_random_access_cnf(ph_random_access_cnf_T *cnf_ptr)
{
  dl_random_access_cnf_T dl_random_access_cnf;
  gs_status_T            gs_status;
  gas_id_t               gas_id = cnf_ptr->gas_id;

  dl_random_access_cnf.message_header.message_set = MS_RR_L2;
  dl_random_access_cnf.message_header.message_id  = (byte)DL_RANDOM_ACCESS_CNF;

  PUT_IMH_LEN(sizeof(dl_random_access_cnf_T) - sizeof(IMH_T),
              &dl_random_access_cnf.message_header);

  dl_random_access_cnf.SAPI = SAPI0;
  dl_random_access_cnf.l2_channel_type       = L2_RACH;
  dl_random_access_cnf.frame_number.msb      = cnf_ptr->frame_number.msb;
  dl_random_access_cnf.frame_number.lsb      = cnf_ptr->frame_number.lsb;
  dl_random_access_cnf.random_access_content = cnf_ptr->random_access_content;
  dl_random_access_cnf.result                = cnf_ptr->result;
  dl_random_access_cnf.gas_id                = gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_random_access_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0_G("Unable to send DL_RANDOM_ACCESS_CNF message");
  }

  return(l2_get_store(gas_id));
}

/*
 *   Function name:  l2_send_dl_suspend_cnf
 *   --------------------------------------
 *
 *   Description: Send the DL_SUSPEND_CNF to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 *
 */

void l2_send_dl_suspend_cnf(l2_store_T *l2_data_ptr)
{
  dl_suspend_cnf_T dl_suspend_cnf;
  gs_status_T      gs_status;

  dl_suspend_cnf.message_header.message_set = MS_RR_L2;
  dl_suspend_cnf.message_header.message_id = (byte)DL_SUSPEND_CNF;

  PUT_IMH_LEN(sizeof(dl_suspend_cnf_T) - sizeof(IMH_T),
              &dl_suspend_cnf.message_header);

  dl_suspend_cnf.SAPI            = SAPI0;
  dl_suspend_cnf.l2_channel_type = DCCH;
  dl_suspend_cnf.gas_id          = l2_data_ptr->gas_id;

  /* reset the transmit manager since this SAPI is being suspended */
  (void)l2_sacch_transmit_manager(l2_data_ptr, SAPI0, NULL);

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_suspend_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_SUSPEND_CNF message");
  }

  return;
}

/*
 *   Function name:  l2_send_dl_resume_cnf
 *   -------------------------------------
 *
 *   Description: Send the DL_RESUME_CNF to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_resume_cnf(l2_store_T *l2_data_ptr)
{
  dl_resume_cnf_T dl_resume_cnf;
  gs_status_T     gs_status;

  dl_resume_cnf.message_header.message_set = MS_RR_L2;
  dl_resume_cnf.message_header.message_id = (byte)DL_RESUME_CNF;

  PUT_IMH_LEN(sizeof(dl_resume_cnf_T) - sizeof(IMH_T),
              &dl_resume_cnf.message_header);

  dl_resume_cnf.SAPI            = SAPI0;
  dl_resume_cnf.l2_channel_type = DCCH;
  dl_resume_cnf.gas_id          = l2_data_ptr->gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_resume_cnf, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_RESUME_CNF message");
  }

  return;
}

/*
 *   Function name:  l2_send_dl_reconnect_cnf
 *   ----------------------------------------
 *
 *   Description: Send the DL_RECONNECT_CNF to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_reconnect_cnf(l2_store_T *l2_data_ptr)
{
  dl_reconnect_cnf_T dl_reconnect_cnf;
  gs_status_T        gs_status;

  dl_reconnect_cnf.message_header.message_set = MS_RR_L2;
  dl_reconnect_cnf.message_header.message_id = (byte)DL_RECONNECT_CNF;

  PUT_IMH_LEN(sizeof(dl_reconnect_cnf_T) - sizeof(IMH_T),
              &dl_reconnect_cnf.message_header);

  dl_reconnect_cnf.SAPI = SAPI0;
  dl_reconnect_cnf.l2_channel_type = DCCH;
  dl_reconnect_cnf.gas_id          = l2_data_ptr->gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_reconnect_cnf, TRUE);
  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_RECONNECT_CNF message");
  }

  return;
}

/*
 *   Function name:  l2_send_dl_data_ind
 *   -----------------------------------
 *
 *   Description: Send the DL_DATA_IND to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_data_ind(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  dl_data_ind_T dl_data_ind;
  sapi_info_T  *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  gs_status_T   gs_status;

  dl_data_ind.message_header.message_set = MS_RR_L2;
  dl_data_ind.message_header.message_id  = (byte)DL_DATA_IND;

  dl_data_ind.SAPI                       = sapi;
  dl_data_ind.l2_channel_type            = channel;
  dl_data_ind.gas_id                     = l2_data_ptr->gas_id;

  if (sapi_info != NULL)
  {
    PUT_IMH_LEN(sizeof(dl_data_ind_T) - sizeof(IMH_T),
                &dl_data_ind.message_header);

    dl_data_ind.l3_message_length = sapi_info->receive_buffer.length;

    (void)memscpy(dl_data_ind.layer3_message, sizeof(dl_data_ind.layer3_message),
                  sapi_info->receive_buffer.layer3_message, sapi_info->receive_buffer.length);

    sapi_info->receive_buffer.length = 0;

    gs_status = gs_send_message(GS_QUEUE_RR, &dl_data_ind, TRUE);
  }
  else
  {
    gs_status = GS_FAILURE;
  }

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_1("Unable to send DL_DATA_IND message for SAPI%1d",(int)sapi);
  }

  return;
}


/*
 *   Function name:  l2_send_dl_data_ack_ind
 *   -----------------------------------
 *
 *   Description: Send the DL_DATA_ACK_IND to RR
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
void l2_send_dl_data_ack_ind(l2_store_T *l2_data_ptr, SAPI_T sapi, message_entry_T *msg_entry)
{
  dl_data_ack_ind_T dl_data_ack_ind;
  gs_status_T       gs_status;

  if (msg_entry == NULL)
  {
    MSG_GERAN_ERROR_0("L3 msg empty");
    return;
  }

  dl_data_ack_ind.message_header.message_set = MS_RR_L2;
  dl_data_ack_ind.message_header.message_id = (byte)DL_DATA_ACK_IND;

  PUT_IMH_LEN(sizeof(dl_data_ack_ind_T) - sizeof(IMH_T),
              &dl_data_ack_ind.message_header);

  dl_data_ack_ind.SAPI = sapi;
  dl_data_ack_ind.l3_message_number = msg_entry->message_number;
  dl_data_ack_ind.gas_id = l2_data_ptr->gas_id;

  gs_status = gs_send_message(GS_QUEUE_RR, &dl_data_ack_ind, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send DL_DATA_ACK_IND message");
  }

  return;
}

/*
 *   Function name:  l2_send_ph_random_access_req
 *   --------------------------------------------
 *
 *   Description: Send the PH_RANDOM_ACCESS_REQ to L1
 *   ------------
 *
 *   Parameters:  Pointer to L2 random access parameter block
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_ph_random_access_req(const dl_random_access_req_T *req_ptr)
{
  ph_random_access_req_T ph_random_access_req;
  gs_status_T            gs_status;
  gas_id_t               gas_id = req_ptr->gas_id;

  ph_random_access_req.message_header.message_set = MS_L2_L1;
  ph_random_access_req.message_header.message_id  = PH_RANDOM_ACCESS_REQ;

  PUT_IMH_LEN(sizeof(ph_random_access_req_T) - sizeof(IMH_T),
              &ph_random_access_req.message_header);

  /* copy supplied parameters into the outbound signal */
  ph_random_access_req.random_access_content    = req_ptr->random_access_content;
  ph_random_access_req.l2_channel_type          = req_ptr->l2_channel_type;

  ph_random_access_req.egprs_ra_content_present = req_ptr->egprs_ra_content_present;
  ph_random_access_req.egprs_ra_content         = req_ptr->egprs_ra_content;
  ph_random_access_req.egprs_tsc                = req_ptr->egprs_training_seq;

  ph_random_access_req.pwr_reduction_valid      = req_ptr->pwr_reduction_valid;
  ph_random_access_req.pwr_reduction_value      = req_ptr->pwr_reduction_value;
  ph_random_access_req.emergency_call           = req_ptr->emergency_call;
  ph_random_access_req.rach_for_ps_traffic      = req_ptr->rach_for_ps_traffic;

  ph_random_access_req.gas_id                   = gas_id;

  gs_status = gs_send_message(l2_get_l1_queue_id(gas_id), &ph_random_access_req, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0_G("GL1 queue cannot accept PH_RANDOM_ACCESS_REQ");
  }

  return;
}

/*
 *   Function name:  l2_send_ph_random_access_ind
 *   --------------------------------------------
 *
 *   Description: Send the PH_RANDOM_ACCESS_IND to L1
 *   ------------
 *
 *   Parameters:  req_ptr - pointer to L2 random access parameter block
 *                repeat_ind - TRUE if this a repeated indication
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
void l2_send_ph_random_access_ind(const dl_random_access_req_T *req_ptr, const boolean repeat_ind)
{
  ph_random_access_ind_T ph_random_access_ind;
  gs_status_T            gs_status;
  gas_id_t               gas_id = req_ptr->gas_id;

  ph_random_access_ind.message_header.message_set = MS_L2_L1;
  ph_random_access_ind.message_header.message_id  = PH_RANDOM_ACCESS_IND;

  PUT_IMH_LEN(sizeof(ph_random_access_ind_T) - sizeof(IMH_T),
              &ph_random_access_ind.message_header);

  /* populate the IND using parameters from the REQ */
  ph_random_access_ind.l2_channel_type     = req_ptr->l2_channel_type;
  ph_random_access_ind.rach_for_ps_traffic = req_ptr->rach_for_ps_traffic;
  ph_random_access_ind.rach_repeat_ind     = repeat_ind;
  ph_random_access_ind.gas_id              = gas_id;

  gs_status = gs_send_message(l2_get_l1_queue_id(gas_id), &ph_random_access_ind, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0_G("GL1 queue cannot accept PH_RANDOM_ACCESS_IND");
  }

  return;
}

/*
 *   Function name:  l2_send_ph_data_req
 *   -----------------------------------
 *
 *   Description: Send the PH_DATA_REQ to L1
 *   ------------
 *
 *   Parameters:  SAPI
 *   -----------  channel_info (provides type and buffer ptr)
 *                frame_length
 *
 *   Returns:     none
 *   --------
 */

void l2_send_ph_data_req(l2_store_T *l2_data_ptr, SAPI_T sapi, channel_info_T *channel_info, byte frame_length)
{
  ph_data_req_T ph_data_req;
  gs_status_T   gs_status;
  byte         *l2_frame = channel_info->transmit_buffer.layer2_message;

  ph_data_req.message_header.message_set = MS_L2_L1;
  ph_data_req.message_header.message_id  = PH_DATA_REQ;

  ph_data_req.l2_channel_type = (l2_channel_type_T)channel_info->channel;

  PUT_IMH_LEN(sizeof(ph_data_req_T) - sizeof(IMH_T),
              &ph_data_req.message_header);

  (void)memscpy(ph_data_req.layer2_frame, sizeof(ph_data_req.layer2_frame),
                l2_frame, frame_length);

  if (ph_data_req.l2_channel_type == SACCH)
  {
    /* allow the Transmit Manager to intercept the data */
    /* note that this means the SAPI in use may change  */
    sapi = l2_sacch_transmit_manager(l2_data_ptr, sapi, &ph_data_req);
  }
  else
  {
    /* for any other channel type, the repetition info is not used */
    ph_data_req.l2_repetition_info = L2_NOT_REPETITION_CANDIDATE;
  }

  ph_data_req.gas_id = l2_data_ptr->gas_id;

  gs_status = gs_send_message(l2_get_l1_queue_id(l2_data_ptr->gas_id), &ph_data_req, TRUE);

  if (gs_status == GS_SUCCESS)
  {
    l2_log_transmission_status(l2_data_ptr, sapi, (byte)ph_data_req.l2_channel_type);
  }
  else
  {
    MSG_GERAN_ERROR_0("Unable to send PH_DATA_REQ message");
  }

  return;
}


/*
 *   Function name:  l2_send_ph_stop_sacch_measurement_report
 *   ------------------------------------------
 *
 *   Description: Send the ph_stop_sacch_measurement_report to L1
 *   ------------
 *
 *   Parameters:   l2_data_ptr
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_ph_stop_sacch_measurement_report(l2_store_T *l2_data_ptr)
{
  ph_stop_sacch_measurement_report_T ph_stop_sacch_measurement_report;
  gs_status_T                        gs_status;

  ph_stop_sacch_measurement_report.message_header.message_set = MS_L2_L1;
  ph_stop_sacch_measurement_report.message_header.message_id = PH_STOP_SACCH_MEASUREMENT_REPORT;

  /*lint -e(778) */
  PUT_IMH_LEN(sizeof(ph_stop_sacch_measurement_report) - sizeof(IMH_T),
              &ph_stop_sacch_measurement_report.message_header);

  ph_stop_sacch_measurement_report.gas_id = l2_data_ptr->gas_id;

  gs_status = gs_send_message(l2_get_l1_queue_id(l2_data_ptr->gas_id), &ph_stop_sacch_measurement_report, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_0("Unable to send PH_STOP_SACCH_MEASUREMENT_REPORT message");
  }

  return;
}


/*
 *   Function name:  l2_cbch_tc_from_fn
 *   --------------------------------
 *
 *   Description: Determines the TC from a frame number (FN)
 *   ------------
 *
 *
 *   Parameters:  frame_num : FN received from L1
 *
 *   Returns:     TC value in range [0..7]
 *   --------
 *
 */

static uint8 l2_cbch_tc_from_fn(uint32 frame_num)
{
  uint8 tc;

  frame_num /= 51;
  tc = frame_num % 8;

  return(tc);
}

/*
 *   Function name:  l2_cbch_demultiplexer
 *   --------------------------------
 *
 *   Description: Executes the CBCH multiplexer callbacks
 *   ------------
 *
 *   Parameters:  Pointer to global L2 data context and message data
 *
 *   Returns:     TRUE if the originating block was forwarded to CB task
 *   --------
 *
 */
static boolean l2_cbch_demultiplexer(l2_store_T *l2_data_ptr, dl_cb_block_ind_T *msg_ptr)
{
  dl_cb_block_ind_T  dest_message[NUM_GERAN_DATA_SPACES];
  dl_cb_block_ind_T *dest_ptr;
  l2_mux_cb_fn_t     cbch_callback;
  gas_id_t           gas_id;
  boolean            data_required;
  boolean            data_confirmed;
  uint8              cbch_tc;

  cbch_callback  = demux_callbacks.cbch_callback;
  data_confirmed = FALSE;
  cbch_tc        = l2_cbch_tc_from_fn(msg_ptr->Frame_no);

  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (gas_id == l2_data_ptr->gas_id)
    {
      /* originator of the data must require it */
      data_required = TRUE;
    }
    else if (cbch_callback != NULL)
    {
      /* any other subscriber will be asked if it needs the data via the callback */
      data_required = (*cbch_callback)(L2_MUX_CBCH, l2_data_ptr->last_channel_number, gas_id);
    }
    else
    {
      /* no callback specified */
      data_required = FALSE;
    }

    if (data_required)
    {
      /* separate piece of memory for each signal, otherwise it can be corrupted */
      dest_ptr = &(dest_message[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)]);
      (void)memscpy(dest_ptr, sizeof(dl_cb_block_ind_T),
                    msg_ptr, sizeof(dl_cb_block_ind_T));

      dest_ptr->as_id = geran_map_gas_id_to_nas_id(gas_id);
      MSG_GERAN_HIGH_3_G("Sending CB_BLOCK_IND with FN=%d (TC=%d) on ASID 0x%02X",
                         (int)(dest_ptr->Frame_no), (int)cbch_tc, (int)(dest_ptr->as_id));

      if (GS_SUCCESS == gs_send_message(GS_QUEUE_CB, dest_ptr, FALSE))
      {
        l2_send_ph_data_cnf(l2_data_ptr, GL2_CB_BLOCK_CNF, msg_ptr->Frame_no);
        if (gas_id == l2_data_ptr->gas_id)
        {
          /* block was passed to the originating subscription */
          data_confirmed = TRUE;
        }
      }
    }
  }

  return(data_confirmed);
}

/*
 *   Function name:  l2_send_dl_cb_block_ind
 *   ---------------------------------------
 *
 *   Description: Send the DL_CB_BLOCK_IND to CB
 *   ------------
 *
 *   Parameters:  l2_frame
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dl_cb_block_ind(l2_store_T *l2_data_ptr, byte *l2_frame, uint32 frame_no)
{
  dl_cb_block_ind_T dl_cb_block_ind;

  dl_cb_block_ind.message_header.message_set = MS_CB_L2;
  dl_cb_block_ind.message_header.message_id = (byte)DL_CB_BLOCK_IND;
  dl_cb_block_ind.Frame_no = frame_no;

  PUT_IMH_LEN(sizeof(dl_cb_block_ind_T) - sizeof(IMH_T),
              &dl_cb_block_ind.message_header);

  if (l2_frame != NULL)
  {
    (void)memscpy(dl_cb_block_ind.cb_block, sizeof(dl_cb_block_ind.cb_block),
                  l2_frame, CB_BLOCK_LEN);
  }
  else
  {
    /* bad block, set payload to null except for first octet */
    (void)memset(dl_cb_block_ind.cb_block, 0, CB_BLOCK_LEN);
    dl_cb_block_ind.cb_block[0] = INVALID_CB_BLOCK_IND_HEADER;
  }

  if (FALSE == l2_cbch_demultiplexer(l2_data_ptr, &dl_cb_block_ind))
  {
    MSG_GERAN_ERROR_0("Unable to send DL_CB_BLOCK_IND message");
  }

  return;
}

