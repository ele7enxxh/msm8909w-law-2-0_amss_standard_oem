/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   G E N E R A L

GENERAL DESCRIPTION
   This module contains procedures used by multiple L2 modules

EXTERNALIZED FUNCTIONS
   l2_clear_sapi
   l2_get_channel_info
   l2_get_sapi_info
   l2_clear_T_ack_timer
   l2_clear_T200_timer
   l2_check_frame_optional


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_general.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
Jan/13/12   SJW     Avoid timer API calls during task startup
Mar/26/09   SJW     Removed redundant locks (including mutexes) around timer calls
Oct/02/08   SJW     Reworked mutex support for test harness compatibility
Jul/31/08   SJW     Added support for KxMutex
Mar/14/06   SJW     Updated l2_check_frame_optional to use correct SAPI and channel
Mar/10/06   AGV     Fixed l2_check_frame_optional to correctly check if an
                    RR Response is needed as per 3GPP 04.06.
Mar/17/04   VT      In l2_clear_T200_timer() and l2_clear_T_ack_timer(): Timer
                      doesn't exist is a MSG_LOW now.
Feb/24/04   VT      In l2_clear_T200_timer() and l2_clear_T_ack_timer(): Added
                      INTLOCK and INTFREE.
Feb/09/04   VT      In l2_clear_T200_timer() and l2_clear_T_ack_timer(): Added
                      call to gs_timer_exists(), to check if the timer is created.
10/04/02   JAC      Started
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include <string.h>
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
#include "l2_transmit.h"
#include "l2_msg.h"
#include "l2_send.h"
#include "l2_timer.h"
#include "msg.h"
#include "err.h"

/*
 *   Function name:  l2_clear_sapi
 *   ---------------------------------
 *
 *   Description: Clears data structures for a sapi
 *   ------------
 *
 *   Parameters:  Pointer to SAPI info structure
 *   -----------
 *
 *   Returns:     none
 *   --------
 *
 */
void l2_clear_sapi(l2_store_T *l2_data_ptr, SAPI_T sapi, boolean clear_all)
{
  sapi_info_T *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);

  if (sapi_info == NULL)
  {
    return;
  }

  sapi_info->VR = 0;
  sapi_info->VS = 0;
  sapi_info->VA = 0;

  sapi_info->sequence_error = FALSE;
  sapi_info->ns_dup_counter = 0;
  sapi_info->retransmission_counter = 0;

  sapi_info->DCCH_channel.outstanding_i_frame = FALSE;
  sapi_info->SACCH_channel.outstanding_i_frame = FALSE;

  sapi_info->DCCH_channel.outstanding_u_frame = FALSE;
  sapi_info->SACCH_channel.outstanding_u_frame = FALSE;

  l2_clear_T200_timer(sapi_info);

  l2_clear_T_ack_timer(sapi_info);

  sapi_info->T_ack_seq_number = 0;

  sapi_info->receive_buffer.length = 0;

  if (clear_all == TRUE)
  {
    /* Clear all the queues and buffers */
    l2_clear_send_queue(sapi_info, DCCH);
    l2_clear_send_buffer(sapi_info, DCCH);

    if (sapi == SAPI3)
    {
      l2_clear_send_queue(sapi_info, SACCH);
      l2_clear_send_buffer(sapi_info, SACCH);
    }

    /* clear transmit flags relevant to the SAPI */
    l2_reset_transmit_flags(l2_data_ptr, sapi);

    sapi_info->DCCH_channel.transmit_buffer.valid_data_flag  = FALSE;
    sapi_info->DCCH_channel.transmit_buffer.retransmission   = FALSE;
    sapi_info->SACCH_channel.transmit_buffer.valid_data_flag = FALSE;
    sapi_info->SACCH_channel.transmit_buffer.retransmission  = FALSE;
  }

  return;
}


/*
 *   Function name:  l2_get_channel_info
 *   ----------------------------------------------
 *
 *   Description:  Returns pointer to channel info
 *   ------------
 *
 *   Parameters:   Pointer to parent SAPI info
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

channel_info_T * l2_get_channel_info(sapi_info_T *sapi_info_ptr, l2_channel_type_T channel)
{
  if (sapi_info_ptr != NULL)
  {
    if (channel == DCCH)
    {
      return(&(sapi_info_ptr->DCCH_channel));
    }
    else if (channel==SACCH)
    {
      return(&(sapi_info_ptr->SACCH_channel));
    }
  }

  return(NULL);
}

/*
 *   Function name:  l2_get_sapi_info
 *   ----------------------------------------------
 *
 *   Description:  Returns pointer to sapi info
 *   ------------
 *
 *   Parameters:   Pointer to L2 store, and SAPI required
 *   -----------
 *
 *   Returns:      Pointer to sapi info
 *   --------
 */
sapi_info_T * l2_get_sapi_info(l2_store_T *l2_data_ptr, SAPI_T sapi)
{
  if (l2_data_ptr != NULL)
  {
    if (sapi == SAPI0)
    {
      return(&(l2_data_ptr->sapi0));
    }
    else if (sapi == SAPI3)
    {
      return(&(l2_data_ptr->sapi3));
    }
  }

  return(NULL);
}

/*
 *   Function name:  l2_clear_T200_timer
 *   ----------------------------------------------
 *
 *   Description: Clears T200 timer
 *   ------------
 *
 *   Parameters:  Pointer to SAPI info
 *   -----------
 *
 *   Returns:     VOID
 *   --------
 *
 */
void l2_clear_T200_timer(sapi_info_T *sapi_info)
{
  if (sapi_info != NULL)
  {
    /* don't call the GS timer API if T200 is being cleared during task init */
    if (sapi_info->T200_state != GS_TMR_UNINITIALISED)
    {
      (void)gs_cancel_timer(GS_QUEUE_LAYER_2, sapi_info->T200);
    }

    l2_timer_clr_params(sapi_info->T200_index);
    sapi_info->T200_state = STOPPED;
    sapi_info->T200_start_request = FALSE;
    sapi_info->T200_index         = BAD_GL2_TIMER_INDEX;

    MSG_GERAN_MED_1("Cleared T200 SAPI%1d",(int)(sapi_info->sapi));
  }

  return;
}

/*
 *   Function name:  l2_clear_T_ack_timer
 *   ----------------------------------------------
 *
 *   Description: Clears T_ack timer
 *   ------------
 *
 *   Parameters:  Pointer to SAPI info
 *   -----------
 *
 *   Returns:     VOID
 *   --------
 *
 */
void l2_clear_T_ack_timer(sapi_info_T *sapi_info)
{
  if (sapi_info != NULL)
  {
    /* don't call the GS timer API if T-ack is being cleared during task init */
    if (sapi_info->T_ack_state != GS_TMR_UNINITIALISED)
    {
      (void)gs_cancel_timer(GS_QUEUE_LAYER_2, sapi_info->T_ack);
    }

    l2_timer_clr_params(sapi_info->T_ack_index);
    sapi_info->T_ack_state = STOPPED;
    sapi_info->T_ack_index = BAD_GL2_TIMER_INDEX;

    MSG_GERAN_MED_1("Cleared TACK SAPI%1d",(int)(sapi_info->sapi));
  }

  return;
}

/*
 *   Function name:  l2_check_frame_optional
 *   -------------------------------
 *
 *   Description: Looks at a frame to see if it is optional
 *   ------------
 *
 *   Parameters:  sapi, channel
 *   -----------
 *
 *   Returns:    TRUE if msg in transmit buffer is a Receive Ready frame with F=0
 *   --------
 */
boolean l2_check_frame_optional(channel_info_T *channel_info_ptr)
{
  /* Ready Receive frames with the F bit set to 0 are optional,
   * since their payload can also be sent as part of an I-frame
   */
  if (channel_info_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("Invalid L2 channel info ptr");
    return(FALSE);
  }

  /* Need to check the correct F bit, valid data flag and frame type */
  /* RR frame control field format is nnnP0001: see 3GPP 04.06 3.8.1 */
  if (((channel_info_ptr->transmit_buffer.layer2_message[1] & 0x1F) == 0x01) &&
      (channel_info_ptr->transmit_buffer.valid_data_flag == TRUE)            &&
      (channel_info_ptr->transmit_buffer.frame_type      == RR))
  {
    return(TRUE);
  }

  return(FALSE);
}
