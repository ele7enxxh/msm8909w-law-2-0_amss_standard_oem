/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 2   T R A N S M I T

GENERAL DESCRIPTION
   This module contains functions used in deciding what message should be
   sent to L1 for sending. It is responsible for keeping track of timers involved in transmission,
   and which transmit buffer to process

EXTERNALIZED FUNCTIONS
   l2_handle_optional_send_timeout
   l2_check_transmit_on_receive
   l2_handle_ph_ready_to_send_ind
   l2_send_DCCH
   l2_send_SACCH
   l2_reset_transmit_flags

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_transmit.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------
28/09/11   SJW       Cancel pending access confirmations when restarting
12/01/11   SJW       Clear the REX timer for Dual SIM pending RACH confirmations
29/10/10   SJW       Added support for Dual SIM
Feb/03/04  VT        In l2_send_msg_transmit_buffer(), checking if
                       channel_info->transmit_buffer is NULL.
06/17/03   HE        Changed FACCH HR timeout to 38 frames
02/21/03   JAC       Set channel type for T200, needed for SAPI3
01/07/03   JAC       SACCH sending triggered on meas report
10/28/02   JAC       Reversed SDCCH and FACCH timeout
10/17/02   JAC       Pseudo SACCH timeout fix, optional ack SACCH,
                     sequence number in optional ack
10/11/01   JAC       started
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
#include "timers.h"
#include "gs.h"
#include "ms_timer.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_msg.h"
#include "l2_send.h"
#include "l2_release.h"
#include "l2_transmit.h"
#include "l2_timer.h"
#include "cfa.h"
#include "cmd.h"           /* for cmd_hdr_type */
#include "l2_log.h"
#include "task.h"
#include "dog.h"
#include "geran_multi_sim.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/****************/
/* Timer values */
/****************/
#define GSM_L2_TIMEOUT_SDCCH   203     /* approx 44 frames, should never timeout since psedo timeout will handle */
#define GSM_L2_TIMEOUT_SACCH  1385     /* Approx 300 frames, should never timeout since pseudo timeout will handle */

#define GSM_L2_TIMEOUT_FACCH_F       167     /* approx 26+10 frames, 10 frames is due to latency in L1, can be as low as 6 though*/
#define L2_SEND_OPTIONAL_ACK_FACCH_F  10     /* have 10 ms (2 frames) (6 frame latency in L1) before making a decision of whether to send ack */

#define GSM_L2_TIMEOUT_FACCH_H       175     /* approx 38 frames. This allows for a proper T200 time difference */
#define L2_SEND_OPTIONAL_ACK_FACCH_H  14     /* have 14 ms (3 frames) (6 frame latency in L1) before making a decision of whether to send ack */

#define L2_SEND_OPTIONAL_ACK_SDCCH    23     /* have 23 ms (5 frames) before making a decision of whether to send ack */

#define L2_MAX_DELAY_DISC             30     /* There is some delay between when L2 receives the PH_READY_TO_SEND_IND */
                                             /* and when L1 actually sends it out                                     */
/**
 * 04.06, 5.8.3
 * Maximum number of octets in an I, UI, SABM and UA frame partially
 * or entirely available for the information field (N201).  The maximum
 * number of octets partially or entirely available for the information
 * field (N201) is:
 *
 * - for frames of format A and B:
 *     for the SACCH: N201 = 18;
 *     for the FACCH and SDCCH: N201 = 20.
 * - for frames of format Bbis:
 *     for BCCH, AGCH, NCH and PCH: N201 = 23;
 * - for frames of format Bter:
 * -   for the SACCH: N201 = 21;
 *     for the FACCH and SDCCH: N201 = 23;
 */
/*****************************************************

                         FUNCTION DECLARATIONS

******************************************************/

/*===========================================================================
FUNCTION l2_timer_expiry

DESCRIPTION

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void l2_timer_expiry(unsigned long param)
{
  timer_expiry_T timer_cmd;

  /* GS services encodes the timer ID as the top 16 bits */
  /* and the client's data payload as the bottom 16 bits */
  timer_cmd.timer_id = (timer_id_T)(param >> 16);
  timer_cmd.data     = (byte) (param & 0x0000FFFF);

  /* Construct message */
  timer_cmd.IMH.message_set = MS_TIMER;
  timer_cmd.IMH.message_id  = MID_TIMER_EXPIRY;

  PUT_IMH_LEN(sizeof (timer_expiry_T) - sizeof (IMH_T), &(timer_cmd.IMH));

  /* Send timer expiry to L2 command queue */
  (void)gs_send_message(GS_QUEUE_LAYER_2, &timer_cmd, FALSE);

  return;
}

/*
 *   Function name:  l2_override_optional_send
 *   ----------------------------------------
 *
 *   Description:  Overrides an optional message if we have something to send instead
 *   ------------
 *
 *   Parameters:  channel_type
 *   -----------
 *
 *   Returns:    TRUE if a message was overrided
 *   --------
 */
static boolean l2_override_optional_send(l2_store_T *l2_data_ptr, l2_channel_type_T channel_type)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info;

  if (channel_type == DCCH)
  {
    sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI0);
    channel_info = l2_get_channel_info(sapi_info, DCCH);
    if (l2_check_frame_optional(channel_info) == TRUE)
    {
      /* replace with an I frame if we have one and nothing else is outstanding */
      if ((channel_info->send_buffer.no_of_entries != 0) &&
          (channel_info->outstanding_i_frame == FALSE)   &&
          (channel_info->outstanding_u_frame == FALSE)
         )
      {
        /* invalidate optional message, when it is time to send it will pick up the I frame */
        channel_info->transmit_buffer.valid_data_flag = FALSE;
        return(TRUE);
      }
    }

    sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI3);
    channel_info = l2_get_channel_info(sapi_info, DCCH);
    if (l2_check_frame_optional(channel_info) == TRUE)
    {
      /* replace with an i frame if we have one and nothing else is outstanding */
      if ((channel_info->send_buffer.no_of_entries != 0) &&
          (channel_info->outstanding_i_frame == FALSE)
         )
      {
        /* invalidate optional message, when it is time to send it will pick up the I frame */
        channel_info->transmit_buffer.valid_data_flag = FALSE;
        return(TRUE);
      }
    }
  }
  else
  {
    sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI3);
    channel_info = l2_get_channel_info(sapi_info, SACCH);
    if (l2_check_frame_optional(channel_info) == TRUE)
    {
      /* replace with an i frame if we have one and nothing else is outstanding */
      if ( (channel_info->send_buffer.no_of_entries != 0) &&
           (channel_info->outstanding_i_frame == FALSE)
         )
      {
        /* invalidate optional message, when it is time to send it will pick up the I frame */
        channel_info->transmit_buffer.valid_data_flag = FALSE;
        return(TRUE);
      }
    }
  }

  return(FALSE);
}

/*
 *   Function name:  l2_check_send_optional
 *   -------------------------------
 *
 *   Description: Checks if we have an optional frame in the buffer.
 *   ------------ Considered optional if is a RR frame with F=0
 *
 *   Parameters:  l2_data_ptr - set to the sapi with the optional ack
 *   -----------  channel_type - channel type to check
 *
 *   Returns:    Valid pointer to the SAPI structure found with an optional message
 *   --------
 */
static sapi_info_T * l2_check_send_optional(l2_store_T *l2_data_ptr, l2_channel_type_T channel_type)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info_ptr;

  (void)l2_override_optional_send(l2_data_ptr, channel_type);

  if (channel_type == DCCH)
  {
    sapi_info_ptr = l2_get_sapi_info(l2_data_ptr, SAPI0);
    channel_info  = l2_get_channel_info(sapi_info_ptr, DCCH);
    if (l2_check_frame_optional(channel_info) == TRUE)
    {
      return(sapi_info_ptr);
    }
    
    sapi_info_ptr = l2_get_sapi_info(l2_data_ptr, SAPI3);
    channel_info  = l2_get_channel_info(sapi_info_ptr, DCCH);
    if (l2_check_frame_optional(channel_info) == TRUE)
    {
      return(sapi_info_ptr);
    }
  }

  return(NULL);
}

/*
 *   Function name:  l2_check_DCCH_mandatory
 *   -------------------------------
 *
 *   Description: Checks if we have to send a message. Considered optional if is a RR
 *   ------------ frame with F=0 and the optional ack timer is running
 *
 *   Parameters:  channel_type - channel type to check
 *   -----------  
 *
 *   Returns:    TRUE if we have to send
 *   --------
 */
static boolean l2_check_DCCH_mandatory(l2_store_T *l2_data_ptr)
{
 (void)l2_override_optional_send(l2_data_ptr, DCCH);

  if (l2_check_frame_optional(&(l2_data_ptr->sapi0.DCCH_channel)) == TRUE)
  {
    /* Now check if we have a timer running for it */
    if (l2_data_ptr->sapi0.T_ack_state == RUNNING)
    {
      return(FALSE);
    }
  }
  else if (l2_check_frame_optional(&(l2_data_ptr->sapi3.DCCH_channel)) == TRUE)
  {
    /* Now check if we have a timer running for it */
    if (l2_data_ptr->sapi3.T_ack_state == RUNNING)
    {
      return(FALSE);
    }
  }

  return(TRUE);
}

static boolean l2_send_msg_send_buffer(l2_store_T *l2_data_ptr, sapi_info_T *sapi_info, channel_info_T *channel_info)
{
  if ((channel_info->send_buffer.no_of_entries != 0) &&
      (channel_info->outstanding_i_frame == FALSE) &&
      (channel_info->outstanding_u_frame == FALSE))
  {
    /**************************************************************/
    /* No frames are ready in the transmit buffer, so get another */
    /* segment from the send_buffer.                              */
    /**************************************************************/

    l2_code_i(0, FALSE, sapi_info, channel_info, FALSE);

    channel_info->outstanding_i_frame = TRUE;

    sapi_info->retransmission_counter = 0;

    sapi_info->VS = (sapi_info->VS + 1) % 8;

    l2_send_ph_data_req(l2_data_ptr, 
                        sapi_info->sapi,
                        channel_info,
                        (byte)(L2_HEADER_LENGTH + channel_info->N201));

    channel_info->transmit_buffer.valid_data_flag = FALSE;

    /*
     * flag that timer is to be started when confirmation
     * of this transmission is received
     */
    sapi_info->T200_start_request = TRUE;
    sapi_info->T200_channel_type = channel_info->channel;

    return(TRUE);
  }

  return(FALSE);
}

static boolean l2_send_msg_transmit_buffer(l2_store_T *l2_data_ptr, sapi_info_T *sapi_info, channel_info_T *channel_info)
{
  gas_id_t gas_id;

  if ((l2_data_ptr  == NULL) ||
      (sapi_info    == NULL) || 
      (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Invalid parameters for l2_send_msg_transmit_buffer");
    return(FALSE);
  }

  gas_id = l2_data_ptr->gas_id;

  if (channel_info->transmit_buffer.valid_data_flag == TRUE)
  {
    /**************************************************/
    /* A frame is ready to be transmitted - do it now */
    /**************************************************/

    channel_info->transmit_buffer.valid_data_flag = FALSE;
    channel_info->transmit_buffer.retransmission = FALSE;

    if ((channel_info->transmit_buffer.frame_type == I) &&
        (channel_info->outstanding_i_frame == FALSE))
    {
      sapi_info->VS = (sapi_info->VS + 1) % 8;
      channel_info->outstanding_i_frame = TRUE;
      if (sapi_info->T200_state != TO_BE_SET)
      {
        MSG_GERAN_ERROR_1_G("T200 not set (%d)", (int)(sapi_info->T200_state));
      }
    }

    /* 3GPP 44.006 section 8.7.1 states:
     * "Only one REJ exception condition for a given direction of information transfer shall be established at a time."
     */
    if ((TRUE == sapi_info->sequence_error) &&
        (REJ  == channel_info->transmit_buffer.frame_type))
    {
      sapi_info->sequence_error = FALSE;
    }

    if (channel_info->transmit_buffer.frame_type != UI_SHORT_HEADER)
    {
      l2_send_ph_data_req(l2_data_ptr,
                          sapi_info->sapi,
                          channel_info,
                          (byte)(L2_HEADER_LENGTH + channel_info->N201));
    }
    else
    {
      l2_send_ph_data_req(l2_data_ptr, 
                          sapi_info->sapi,
                          channel_info,
                          (byte)N201_SACCH_BTER);
    }

    /*************************************************************/
    /* when the message was set up, T200 was requested to be run */
    /* so now flag that timer is to be started when confirmation */
    /* of this transmission is received                          */
    /* ALL msg on sapi0 sacch are UI frames so don't set it      */
    /*************************************************************/
    if (
        ( (sapi_info->T200_state == TO_BE_SET) ||
          (sapi_info->T200_state == TO_BE_SET_MAX_LATENCY) ||
          (sapi_info->T200_state == TO_BE_SET_ASAP)
        ) &&
        !((sapi_info->sapi == SAPI0) && (channel_info->channel == SACCH))
       )
    {
      sapi_info->T200_start_request = TRUE;
      sapi_info->T200_channel_type = channel_info->channel;
    }

    return(TRUE);
  }

  return(FALSE);
}

static boolean l2_send_msg(l2_store_T *l2_data_ptr, sapi_info_T *sapi_info, l2_channel_type_T channel)
{
  channel_info_T *channel_info;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(FALSE);
  }

  if ((sapi_info->sapi == SAPI0) && (channel == SACCH))
  {
    /* For SAPI0 SACCH, only check the transmit buffer, no send queue */
    if (l2_send_msg_transmit_buffer(l2_data_ptr, sapi_info, channel_info) == TRUE)
    {
      return(TRUE);
    }
  }
  else
  {
    if (l2_send_msg_transmit_buffer(l2_data_ptr, sapi_info, channel_info) == TRUE)
    {
      return(TRUE);
    }
    else if (l2_send_msg_send_buffer(l2_data_ptr, sapi_info, channel_info) == TRUE)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}

/*
 *   Function name:  l2_forward_data
 *   -------------------------------
 *
 *   Description: Transmit some data to the layer 1
 *   ------------
 *
 *   Parameters: Channel type
 *   -----------
 *
 *   Returns:     TRUE if message was sent
 *   --------
 */

static boolean l2_forward_data(l2_store_T *l2_data_ptr, l2_channel_type_T channel_type)
{
  sapi_info_T *sapi0_ptr;
  sapi_info_T *sapi3_ptr;
  boolean      frame_sent = FALSE;

  sapi0_ptr = l2_get_sapi_info(l2_data_ptr, SAPI0);
  sapi3_ptr = l2_get_sapi_info(l2_data_ptr, SAPI3);

  if (channel_type == DCCH)
  {
    (void)l2_override_optional_send(l2_data_ptr, DCCH);

    if (l2_send_msg(l2_data_ptr, sapi0_ptr, DCCH) == TRUE)
    {
      l2_clear_T_ack_timer(sapi0_ptr);
      frame_sent = TRUE;
    }
    else if (l2_send_msg(l2_data_ptr, sapi3_ptr, DCCH) == TRUE)
    {
      l2_clear_T_ack_timer(sapi3_ptr);
      frame_sent = TRUE;
    }
  }
  else if (channel_type == SACCH)
  {
    (void)l2_override_optional_send(l2_data_ptr, SACCH);

    /* check SAPI0 first if processed SAPI3 last time*/
    if (l2_data_ptr->sacch_last_sapi_served == SAPI3)
    {
      if (l2_send_msg(l2_data_ptr, sapi0_ptr, SACCH) == TRUE)
      {
        l2_data_ptr->sacch_last_sapi_served = SAPI0;
        frame_sent = TRUE;
      }
    }

    /* Now check SAPI3 if no frames were sent */
    if (frame_sent == FALSE)
    {
      if (l2_send_msg(l2_data_ptr, sapi3_ptr, SACCH) == TRUE)
      {
        l2_data_ptr->sacch_last_sapi_served = SAPI3;
        l2_clear_T_ack_timer(sapi3_ptr);
        frame_sent = TRUE;
      }
    }

    /* check SAPI0 if it wasn't checked first, and nothing was sent on SAPI3 */
    if ((l2_data_ptr->sacch_last_sapi_served == SAPI0) && (frame_sent == FALSE))
    {
      if (l2_send_msg(l2_data_ptr, sapi0_ptr, SACCH) == TRUE)
      {
        l2_data_ptr->sacch_last_sapi_served = SAPI0;
        frame_sent = TRUE;
      }
    }
  }

  return frame_sent;
}


/*===========================================================================

FUNCTION  l2_send_DCCH

DESCRIPTION
  Checks ready state variable to determine if we can send DCCH messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_send_DCCH(l2_store_T *l2_data_ptr)
{
  if (l2_check_DCCH_mandatory(l2_data_ptr) == TRUE)
  {
    if (l2_data_ptr->dcch_ready == TRUE)
    {
      if (l2_forward_data(l2_data_ptr, DCCH))
      {
        l2_data_ptr->dcch_ready = FALSE;
      }
    }
  }

  return;
/*
  Do not send on SACCH until we received the measurement report.

  The measurement report will come 13 frames after the last received SACCH message,
  This gives enough time for the upper layers to send a response if the msg
  was a SMS msg.

  Measurement reports should only be sent if we do not need to send anything else
  or the interval between the last measurement report was 1 frame, this is handled in
  l2_forward_data
*/
}

/*===========================================================================

FUNCTION  l2_send_SACCH

DESCRIPTION
  Checks ready state variable to determine if we can send sacch message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_send_SACCH(l2_store_T *l2_data_ptr)
{
  if (l2_data_ptr->sacch_ready == TRUE)
  {
    if (l2_forward_data(l2_data_ptr, SACCH))
    {
      l2_data_ptr->sacch_ready = FALSE;
    }
  }

  return;
}

/*
 *   Function name:  l2_start_T200_timer
 *   ----------------------------------------------
 *
 *   Description: Starts T200 timer
 *   ------------
 *
 *   Parameters: channel_type
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 */
static void l2_start_T200_timer(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel, unsigned int timeout_value)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  byte            timer_index;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* ask the timer module to define some storage for the timer parameters */
  timer_index = l2_timer_set_params(l2_data_ptr, sapi, channel, 0);
  if (timer_index == BAD_GL2_TIMER_INDEX)
  {
    MSG_GERAN_ERROR_0("Unable to start T200 timer, insufficient storage");
    return;
  }

  MSG_GERAN_MED_2("Start T200 SAPI%1d channel %d",(int)sapi, (int)channel);

  (void) gs_start_timer(GS_QUEUE_LAYER_2,
                        sapi_info->T200,
                        (rex_timer_cb_type)l2_timer_expiry,
                        timeout_value,
                        timer_index);

  sapi_info->T200_start_request = FALSE;
  sapi_info->T200_state         = RUNNING;
  sapi_info->T200_channel_type  = channel;
  sapi_info->T200_index         = timer_index;

  if (sapi == SAPI3)
  {
    l2_data_ptr->pseudo_sapi3_receive = 0;
  }

  return;
}

/*
 *   Function name:  l2_start_T_ack_timer
 *   ----------------------------------------------
 *
 *   Description: Starts T_ack timer
 *   ------------
 *
 *   Parameters: L2 data pointer, SAPI and timeout value
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 */
static void l2_start_T_ack_timer(l2_store_T *l2_data_ptr, SAPI_T sapi, unsigned int timeout_value)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  byte            timer_index;

  channel_info = l2_get_channel_info(sapi_info, DCCH);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* determine next sequence number for this timer */
  if (sapi_info->T_ack_seq_number == 0)
  {
    sapi_info->T_ack_seq_number = 1;
  }
  else
  {
    sapi_info->T_ack_seq_number = 0;
  }

  /* ask the timer module to define some storage for the timer parameters */
  timer_index = l2_timer_set_params(l2_data_ptr, sapi, DCCH, sapi_info->T_ack_seq_number);
  if (timer_index == BAD_GL2_TIMER_INDEX)
  {
    MSG_GERAN_ERROR_0("Unable to start T_ACK timer, insufficient storage");
    return;
  }

  MSG_GERAN_MED_2("Start T_ACK SAPI%1d sequence %d",(int)sapi, (int)(sapi_info->T_ack_seq_number));

  (void)gs_start_timer(GS_QUEUE_LAYER_2,
                       sapi_info->T_ack,
                       (rex_timer_cb_type)l2_timer_expiry,
                       timeout_value,
                       timer_index);

  sapi_info->T_ack_state = RUNNING;
  sapi_info->T_ack_index = timer_index;

  return;
}

#if 0
/*
 *   Function name:  l2_stale_cache_failure
 *   ----------------------------------------------
 *
 *   Description: Called when Layer 2 determines it has 
 *                received READY_TO_SEND for a stale cache
 *
 *   Parameters:  Pointer to cached DL_RANDOM_ACCESS_REQ and GAS ID
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 */
static void l2_stale_cache_failure(dl_random_access_req_T *cache_ptr, const gas_id_t gas_id)
{
  if (cache_ptr != NULL)
  {
    ph_random_access_cnf_T failed_cnf;

    /* build a PH_RANDOM_ACCESS_CNF with a failure result in it */
    failed_cnf.frame_number.msb           = 0xDE;
    failed_cnf.frame_number.lsb           = 0xAD;
    failed_cnf.l2_channel_type            = cache_ptr->l2_channel_type;
    failed_cnf.result                     = GERAN_ACCESS_RESTART_IN_GL2;
    if (cache_ptr->egprs_ra_content_present)
    {
      failed_cnf.random_access_content = cache_ptr->egprs_ra_content;
    }
    else
    {
      failed_cnf.random_access_content = (uint16)(cache_ptr->random_access_content);
    }

    /* cache will have an invalid GAS ID, so use the supplied value */
    failed_cnf.gas_id = gas_id;

    /* send this failure indication to GRR */
    (void)l2_send_dl_random_access_cnf(&failed_cnf);
  }

  return;
}
#endif

/*
 *   Function name:  l2_rach_ready_to_send
 *   ----------------------------------------------
 *
 *   Description: Called when Layer 1 indicates it
 *                can handle a PH_RANDOM_ACCESS_REQ.
 *
 *   Parameters:  Global store pointer
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 */
static void l2_rach_ready_to_send(l2_store_T *l2_data_ptr)
{
  if (l2_data_ptr != NULL)
  {
    dl_random_access_req_T *cache_ptr = &(l2_data_ptr->pending_ra_request);
    gas_id_t                gas_id    = l2_data_ptr->gas_id;

    MSG_GERAN_HIGH_0_G("PH_READY_TO_SEND_IND(RACH) from Layer 1");

    /* check for cache validity: bad GAS ID means the contents have been used already */
    if (GERAN_ACCESS_STRATUM_ID_UNDEFINED == cache_ptr->gas_id)
    {
      MSG_GERAN_ERROR_0_G("DL_RANDOM_ACCESS_REQ cache is stale");
    }
    else
    {
      if (cache_ptr->gas_id != gas_id)
      {
        /* cache does not contain the same GAS ID as the current context */
        MSG_GERAN_ERROR_1_G("Unexpected GAS ID 0x%02X in cached request", (int)(cache_ptr->gas_id));
      }

      /* send the pending request to GL1 and then invalidate the cache */
      l2_send_ph_random_access_req(cache_ptr);
      cache_ptr->gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
    }
  }

  return;
}

/*
 *   Function name:  l2_handle_ph_ready_to_send_ind
 *   ----------------------------------------------
 *
 *   Description: Function to handle the PH_READY_TO_SEND_IND message - also
 *   ------------ called when ready to send can be assumed at start of link
 *                communications
 *
 *   Parameters: channel_type
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 */
void l2_handle_ph_ready_to_send_ind(l2_store_T *l2_data_ptr, l2_channel_type_T channel_type)
{
  unsigned int T200_timeout;

  /*
   * set global parameters based on the channel type
   */
  switch (channel_type)
  {
    case DCCH:  /* DCCH - assume SDCCH */
    case SDCCH:
    case FACCH_F:
    case FACCH_H:
      l2_data_ptr->dcch_ready = TRUE;
      MSG_GERAN_HIGH_1("PH_READY_TO_SEND_IND for chan %d, DCCH ready now", (int)channel_type);

      if (channel_type == FACCH_F)
      {
        l2_data_ptr->last_dcch_type = FACCH_F;
        T200_timeout   = GSM_L2_TIMEOUT_FACCH_F;
      }
      else if (channel_type == FACCH_H)
      {
        l2_data_ptr->last_dcch_type = FACCH_H;
        T200_timeout   = GSM_L2_TIMEOUT_FACCH_H;
      }
      else
      {
        l2_data_ptr->last_dcch_type = SDCCH;
        T200_timeout   = GSM_L2_TIMEOUT_SDCCH;
      }

      /* update the value of the N200 counter based on the DCCH type */
      l2_update_N200_channel_type(l2_data_ptr, l2_data_ptr->last_dcch_type);

      if (l2_data_ptr->sapi0.T200_start_request == TRUE)
      {
        if (l2_data_ptr->sapi0.T200_channel_type != DCCH)
        {
          MSG_GERAN_ERROR_0("SAPI0 not DCCH");
        }

        if (l2_data_ptr->sapi0.T200_state == TO_BE_SET_MAX_LATENCY)
        {
          /* Sending a disc command on timeout tear down the link */
          T200_timeout = L2_MAX_DELAY_DISC;
        }
        else if ((l2_data_ptr->sapi0.T200_state == TO_BE_SET_ASAP) &&
                 (l2_data_ptr->last_dcch_type   != SDCCH))
        {
          T200_timeout = 15;
        }

        l2_start_T200_timer(l2_data_ptr,SAPI0,DCCH,T200_timeout);
      }
      else if ((l2_data_ptr->sapi3.T200_start_request == TRUE) &&
               (l2_data_ptr->sapi3.T200_channel_type  == DCCH))
      {
        if (l2_data_ptr->sapi3.T200_state == TO_BE_SET_MAX_LATENCY)
        {
          /* Sending a disc command on timeout tear down the link */
          T200_timeout = L2_MAX_DELAY_DISC;
        }
        l2_start_T200_timer(l2_data_ptr,SAPI3,DCCH,T200_timeout);
      }
      break;

    case SACCH:
      l2_data_ptr->sacch_ready = TRUE;
      T200_timeout = GSM_L2_TIMEOUT_SACCH;

      /* All msg sent on SAPI0 SACCH are unack so don't need to check */
      if ((l2_data_ptr->sapi3.T200_start_request == TRUE) &&
          (l2_data_ptr->sapi3.T200_channel_type  == SACCH))
      {
        if (l2_data_ptr->sapi3.T200_state == TO_BE_SET_MAX_LATENCY)
        {
          /* Sending a disc command on timeout tear down the link */
          T200_timeout = L2_MAX_DELAY_DISC;
        }

        l2_start_T200_timer(l2_data_ptr,SAPI3,SACCH,T200_timeout);
      }
      break;

    case L2_RACH_WITH_NO_DELAY:
    case L2_RACH:
      /* this RTS indicates Layer 1 is ready to handle PH_RANDOM_ACCESS_REQ now */
      l2_rach_ready_to_send(l2_data_ptr);
      break;

    default:
      MSG_GERAN_ERROR_1("Ready to send - unknown channel %d", (int)channel_type);
      break;
  }

  return;
}


/*===========================================================================

FUNCTION  l2_check_transmit_on_receive

DESCRIPTION
  We received a message on a the DCCH or SACCH, check if we have anything
  to send

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void l2_check_transmit_on_receive(l2_store_T *l2_data_ptr)
{
  /*
   * if T200 is running, it will be timing out soon, so
   * do the timeout processing now so that the correct
   * decision can be made as to what to send to layer 1
   * next - this should only be done for sdcch, as facch
   * doesn't have the same critical timing requirement
   */

  /********************************************************************/
  /*Is this within specs? Yes, on the SDCCH, there is 1 receive and   */
  /*1 transmit every 51 frames so if we get a receive that does not   */
  /*contain the ack we are waiting for we can automatically retransmit*/
  /*since it is impossible to get another ack before the timer expires*/
  /********************************************************************/
  if ((l2_data_ptr->current_l2_channel_type == DCCH) && (l2_data_ptr->last_dcch_type == SDCCH))
  {
    if ((l2_data_ptr->sapi0.T200_state == RUNNING) &&
        (l2_data_ptr->sapi0.T200_channel_type == DCCH))
    {
      l2_clear_T200_timer(&(l2_data_ptr->sapi0));

      l2_data_ptr->current_SAPI = SAPI0;

      MSG_GERAN_MED_0("Pseudo T200(0) timeout");

      l2_SAPI0_control(l2_data_ptr, EV_T200_TIMEOUT);
    }

    if ((l2_data_ptr->sapi3.T200_state == RUNNING) &&
        (l2_data_ptr->sapi3.T200_channel_type == DCCH))
    {
      l2_clear_T200_timer(&(l2_data_ptr->sapi3));

      l2_data_ptr->current_SAPI = SAPI3;

      MSG_GERAN_MED_0("Pseudo T200(3) timeout");

      l2_SAPI3_control(l2_data_ptr, EV_T200_TIMEOUT);
    }
  }
  else if (l2_data_ptr->current_l2_channel_type == SACCH)
  {
    if ((l2_data_ptr->sapi3.T200_state == RUNNING)  &&
        (l2_data_ptr->sapi3.T200_channel_type == SACCH))
    {
      /* For SACCH on sapi3 we must wait for at least
       * 3 receives before retransmitting
       */
      l2_data_ptr->pseudo_sapi3_receive++;

      if (l2_data_ptr->pseudo_sapi3_receive >= 2)
      {
        l2_clear_T200_timer(&(l2_data_ptr->sapi3));

        l2_data_ptr->current_SAPI = SAPI3;

        MSG_GERAN_MED_0("Pseudo T200(3) timeout");

        l2_SAPI3_control(l2_data_ptr, EV_T200_TIMEOUT);
      }
    }
  }

  /**********************************************************/
  /* Check DCCH buffers to see if we need to send anything  */
  /* No need for optional send on SACCH since sending on    */
  /* on SACCH is triggered by receipt of a meas report.     */
  /* The meas report will come ~13 frames later so it       */
  /* acts like the optional send timer                      */
  /**********************************************************/
  if (l2_data_ptr->current_l2_channel_type == DCCH)
  {
    sapi_info_T *sapi_info;

    /************************************************************/
    /* Check if mandatory frame in transmit buffer, if not wait */
    /* longer before deciding what to send. We should check     */
    /* if the timer is already running. On the 8960 they will   */
    /* retransmit with the P bit not set this will cause us to  */
    /* get stuck                                                */
    /************************************************************/
    sapi_info = l2_check_send_optional(l2_data_ptr, DCCH);
    if (sapi_info != NULL)
    {
      /*****************************************************************/
      /* Optional frame, set timer to wake us up, we can decide later  */
      /* When this timer is running and we only have an optional frame */
      /* to send we keep waiting. Once it expires we are forced to     */
      /* send                                                          */
      /*****************************************************************/
      if (sapi_info->T_ack_state != RUNNING)
      {
        if (l2_data_ptr->last_dcch_type == FACCH_F)
        {
          l2_start_T_ack_timer(l2_data_ptr, sapi_info->sapi, L2_SEND_OPTIONAL_ACK_FACCH_F);
        }
        else if (l2_data_ptr->last_dcch_type == FACCH_H)
        {
          l2_start_T_ack_timer(l2_data_ptr, sapi_info->sapi, L2_SEND_OPTIONAL_ACK_FACCH_H);
        }
        else
        {
          l2_start_T_ack_timer(l2_data_ptr, sapi_info->sapi, L2_SEND_OPTIONAL_ACK_SDCCH);
        }
      }
    }
  }

  return;
}


/*
 *   Function name:  l2_handle_optional_send_timeout
 *   ----------------------------------------
 *
 *   Description:  Check if we have anything else to send beside the optional message
 *   ------------
 *
 *   Parameters:  Pointer to L2 store, channel_type
 *   -----------
 *
 *   Returns:    TRUE if message is optional
 *   --------
 */
void l2_handle_optional_send_timeout(l2_store_T *l2_data_ptr, l2_channel_type_T channel_type)
{
  if (channel_type == DCCH)
  {
    if (l2_data_ptr->dcch_ready == TRUE)
    {
      if (l2_forward_data(l2_data_ptr, DCCH))
      {
        l2_data_ptr->dcch_ready = FALSE;
      }
    }
  }
  else if (channel_type == SACCH)
  {
    if (l2_data_ptr->sacch_ready == TRUE)
    {
      if (l2_forward_data(l2_data_ptr, SACCH))
      {
        l2_data_ptr->sacch_ready = FALSE;
      }
    }
  }

  return;
}

/*
 *   Function name:  l2_reset_transmit_flags
 *   ----------------------------------------
 *
 *   Description:  Clears down local flags in this module
 *   ------------
 *
 *   Parameters:  sapi
 *   -----------
 *
 */
void l2_reset_transmit_flags(l2_store_T *l2_data_ptr, SAPI_T sapi)
{
  if (sapi == SAPI0)
  {
    l2_data_ptr->sacch_last_sapi_served = sapi;
  }
  else
  {
    l2_data_ptr->pseudo_sapi3_receive = 0;
  }

  return;
}
