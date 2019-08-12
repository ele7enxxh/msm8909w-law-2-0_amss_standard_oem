/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L2 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging L2 packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_log.c_v   1.2   07 Mar 2002 17:55:26   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_log.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
29/10/10   SJW     Added support for Dual SIM
02/11/02   JAC     Initial Revision.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "environ.h"
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
#include "environ.h"
#include "msg.h"
#include "l2i.h"
#include "l2_msg.h"
#include "l2_log.h"         /* This module's header file */
#include "log.h"            /* For log_commit() and log_alloc() and log codes */
#include "event.h"          /* For logging RAR event and event codes */
#include "log_codes_gsm.h"  /* For GSM log codes */
#include "log_codes_gsm_v.h"
#include "gsmdiag_v.h"      /* For gsmdiag_log_alloc() and gsmdiag_log_commit() */
#include "l2_transfer.h"
#include "geran_multi_sim.h"

/*===========================================================================

FUNCTION  l2_log_packet_init

DESCRIPTION
  Initialises a gsmdiag_pkt_info structure ready for use with logging functions

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void l2_log_packet_init(gsmdiag_pkt_info_t *info_ptr, log_code_type pkt_id, unsigned int pkt_size, gas_id_t gas_id)
{
  if (info_ptr != NULL)
  {
    info_ptr->pkt_id   = pkt_id;
    info_ptr->pkt_size = pkt_size;
#ifdef FEATURE_DUAL_SIM
    info_ptr->dual_sim_active = TRUE;
    info_ptr->active_sub_id   = 1 + GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    info_ptr->pkt_version_id  = GSMDIAG_PKT_VERSION_DEFAULT;
#endif /*FEATURE_DUAL_SIM*/
  }

  return;
}

/*===========================================================================

FUNCTION  l2_log_state

DESCRIPTION
  This function should be called when L2 state/event info changes.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_log_state(l2_store_T *l2_data_ptr, byte sapi, byte l2_state, l2_event_T l2_event)
{
  LOG_GSM_L2_STATE_C_type *log_ptr;
  gsmdiag_pkt_info_t       pkt_info;

  l2_log_packet_init(&pkt_info, LOG_GSM_L2_STATE_C, sizeof(LOG_GSM_L2_STATE_C_type), l2_data_ptr->gas_id);
  log_ptr = (LOG_GSM_L2_STATE_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    /* Fill log packet and commit it */
    log_ptr->sapi     = sapi;
    log_ptr->l2_state = l2_state;
    log_ptr->l2_event = (byte)l2_event;

    gsmdiag_log_commit(&pkt_info);
  }

  return;
}

/*===========================================================================

FUNCTION  l2_log_transmission_status

DESCRIPTION
  This function should be called when a PH_DATA_REQ is sent to L1, it logs
  L2's internal transmission variables and the type of packet being sent.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_log_transmission_status(l2_store_T *l2_data_ptr, byte sapi, byte channel)
{
  LOG_GSM_L2_TRANSMISSION_STATUS_C_type *log_ptr;
  gsmdiag_pkt_info_t                     pkt_info;
  channel_info_T                        *channel_info;
  sapi_info_T                           *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  byte                                   message_entries;
  byte                                   segment_entries;
  byte                                   retrans;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((channel_info == NULL) || (sapi_info == NULL))
  {
    MSG_GERAN_ERROR_0("No channel or sapi info, cannot log");
    return;
  }

  /* Note: for SACCH on SAPI0 there is no send queue or buffer */
  retrans = sapi_info->retransmission_counter;
  if ((SAPI0 == sapi) && (SACCH == channel))
  {
    message_entries = 0;
    segment_entries = 0;
    /* blank out the retransmission counter for unack frames */
    if (channel_info->transmit_buffer.frame_type == UI)
    {
      retrans = 0;
    }
  }
  else
  {
    message_entries = channel_info->send_queue.no_of_entries;
    segment_entries = channel_info->send_buffer.no_of_entries;
  }

  l2_log_packet_init(&pkt_info, LOG_GSM_L2_TRANSMISSION_STATUS_C, sizeof(LOG_GSM_L2_TRANSMISSION_STATUS_C_type), l2_data_ptr->gas_id);
  log_ptr = (LOG_GSM_L2_TRANSMISSION_STATUS_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    /* Fill packet and commit it */
    log_ptr->sapi                   = (byte)sapi;
    log_ptr->channel_type           = (byte)channel;
    log_ptr->VS                     = (byte)sapi_info->VS;
    log_ptr->VA                     = (byte)sapi_info->VA;
    log_ptr->VR                     = (byte)sapi_info->VR;
    log_ptr->retransmission_counter = (byte)retrans;
    log_ptr->sequence_error         = (byte)sapi_info->sequence_error;
    log_ptr->frame_type             = (byte)channel_info->transmit_buffer.frame_type;
    log_ptr->message_entries        = (byte)message_entries;
    log_ptr->segment_entries        = (byte)segment_entries;

    gsmdiag_log_commit(&pkt_info);
  }

  return;
}

/*===========================================================================

FUNCTION  l2_log_timeout

DESCRIPTION
  This function should be called when the T200 timer expires
  It logs whether L2 is waiting for an ack for a previous sent frame.
  It will send a log packet to the DM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_log_timeout(l2_store_T *l2_data_ptr, 
                    byte SAPI0_DCCH_outstanding_i_frame,
                    byte SAPI3_DCCH_outstanding_i_frame,
                    byte SAPI3_SACCH_outstanding_i_frame)
{
  LOG_GSM_L2_OUTSTANDING_FRAME_C_type *log_ptr;
  gsmdiag_pkt_info_t                   pkt_info;

  l2_log_packet_init(&pkt_info, LOG_GSM_L2_OUTSTANDING_FRAME_C, sizeof(LOG_GSM_L2_OUTSTANDING_FRAME_C_type), l2_data_ptr->gas_id);
  log_ptr = (LOG_GSM_L2_OUTSTANDING_FRAME_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (NULL != log_ptr)
  {
    log_ptr->sapi0_DCCH_outstanding_I_frame  = (byte)SAPI0_DCCH_outstanding_i_frame;
    log_ptr->sapi3_DCCH_outstanding_I_frame  = (byte)SAPI3_DCCH_outstanding_i_frame;
    log_ptr->sapi3_SACCH_outstanding_I_frame = (byte)SAPI3_SACCH_outstanding_i_frame;

    gsmdiag_log_commit(&pkt_info);
  }

  return;
}

