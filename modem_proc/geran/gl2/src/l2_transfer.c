/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   T R A N S F E R

GENERAL DESCRIPTION
   This module contains the procedures for handling I, RR, and REJ frames

EXTERNALIZED FUNCTIONS

   l2_handle_info_reception
   l2_handle_normal_rr_reception
   l2_handle_recovery_rr_reception
   l2_handle_normal_rej_reception
   l2_handle_recovery_rej_reception
   l2_handle_I_retransmission

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_transfer.c_v   1.9   09 Jul 2002 14:18:08   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_transfer.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
01/13/12   SJW      Avoid timer API calls during task startup
02/11/09   SJW      Restart T200 if it is running when a N(S) seq err occurs
10/17/02   JAC      Commented out cpdata/ccrelease hack
                    Added clear_T200 timer in l2_receive_new_ack
                    During retransmission don't clear out RR frame with F=1
10/04/02   JAC      Code reorg
07/07/02   JAC      Added explicit check for NR sequence error
02/28/02   JAC      Modified l2_data_store to include send queue and buffer
02/19/02   JAC      Fixed l2_init_store, previously pointed to wrong buffers
12/15/01   JAC      Clear T200 on a rej frame in nonrecovery mode
                    Adds 1 to VS on a rej frame if retransmitting
12/14/01   JAC      Added functions to send rr, rej and respond to acks
                    Fixed problems identified in code review
10/19/01   JAC      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "environ.h"
#include "stringl/stringl.h"
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
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_timer.h"
#include "l2_msg.h"
#include "l2_send.h"
#include "msg.h"
#include "err.h"

/*****************************************************

 GLOBALS

******************************************************/

/*
 *   Function name:  l2_init_sapi
 *   ----------------------------------------------
 *
 *   Description:  Initializes l2 data store to correct variables
 *   ------------
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
void l2_init_sapi(sapi_info_T *sapi_info, boolean from_power_on)
{
  /*********************************/
  /* Initialise the receive buffer */
  /*********************************/
  sapi_info->receive_buffer.length = 0;

  /***********************************/
  /* Initialise the transmit buffers */
  /***********************************/
  sapi_info->DCCH_channel.transmit_buffer.valid_data_flag  = FALSE;
  sapi_info->DCCH_channel.transmit_buffer.retransmission   = FALSE;
  sapi_info->SACCH_channel.transmit_buffer.valid_data_flag = FALSE;
  sapi_info->SACCH_channel.transmit_buffer.retransmission  = FALSE;


  /***********************************/
  /* Initialise the send queues      */
  /***********************************/
  sapi_info->DCCH_channel.send_queue.no_of_entries  = 0;
  sapi_info->DCCH_channel.send_queue.first_message  = NULL;

  sapi_info->SACCH_channel.send_queue.no_of_entries = 0;
  sapi_info->SACCH_channel.send_queue.first_message = NULL;


  /*******************************/
  /* Initialise the send buffers */
  /*******************************/
  sapi_info->DCCH_channel.send_buffer.no_of_entries  = 0;
  sapi_info->SACCH_channel.send_buffer.no_of_entries = 0;

  sapi_info->DCCH_channel.send_buffer.first_segment  = NULL;
  sapi_info->SACCH_channel.send_buffer.first_segment = NULL;

  sapi_info->DCCH_channel.send_buffer.next_segment   = NULL;
  sapi_info->SACCH_channel.send_buffer.next_segment  = NULL;

  /* set the timer states before clearing the SAPIs */
  if (from_power_on)
  {
    sapi_info->T200_state  = GS_TMR_UNINITIALISED;
    sapi_info->T200_index  = BAD_GL2_TIMER_INDEX;

    sapi_info->T_ack_state = GS_TMR_UNINITIALISED;
    sapi_info->T_ack_index = BAD_GL2_TIMER_INDEX;
  }

  sapi_info->DCCH_channel.channel  = DCCH;
  sapi_info->SACCH_channel.channel = SACCH;
  sapi_info->DCCH_channel.N201     = N201_DCCH;
  sapi_info->SACCH_channel.N201    = N201_SACCH;

  return;
}


/*
 *   Function name:  l2_receive_new_ack
 *   ----------------------------------
 *
 *   Description: updates VA and retransmission of a message
 *   ------------
 *
 *   Parameters: sapi - SAPI currently on
 *   ----------- channel_type_T - channel currently on
 *               new_VA - value to update ack variable with
 *
 *   Returns:     none
 *   --------
 */
static void l2_receive_new_ack(l2_store_T *l2_data_ptr, sapi_info_T* sapi_info, channel_info_T* channel_info, byte new_VA)
{
  if (sapi_info==NULL || channel_info==NULL)
  {
    MSG_GERAN_ERROR_0("Null sapi/channel");
    return;
  }

  sapi_info->VA = new_VA;

  if (channel_info->outstanding_i_frame==TRUE)
  {
    if (sapi_info->sapi==SAPI3 && channel_info->channel==SACCH)
    {
      MSG_GERAN_HIGH_1("SAPI3 SACCH VA=%1d",(int)new_VA);
    }

    /**************************************************************/
    /* Specs say we should not clear T200 if F bit not set to 1   */
    /* when in timer recovery mode 5.5.3.1, in our implementation */
    /* we will start sending the next i frame so we need to reset */
    /**************************************************************/
    l2_clear_T200_timer(sapi_info);

    /*kill retransmission since we got an ack*/
    if (channel_info->transmit_buffer.retransmission==TRUE)
    {
      channel_info->transmit_buffer.valid_data_flag=FALSE;
      channel_info->transmit_buffer.retransmission=FALSE;
    }
    channel_info->outstanding_i_frame=FALSE;
    l2_next_segment(l2_data_ptr, sapi_info, channel_info);
  }
  else
  {
    MSG_GERAN_HIGH_0("No outstanding I frame for ack?");
  }

  return;
}

/*
 *   Function name:  l2_nr_in_range
 *   ----------------------------------------------
 *
 *   Description: Local function to perform N(R) sequence error check, using
 *                modulo arithmetic as detailed in 3GPP 04.06 section 3.5.2.1.
 *   ------------
 *
 *   Parameters:  VA, NR, VS (see 3GPP 04.06 section 5.7.4)
 *   -----------
 *
 *   Returns:     1 if the NR value is within range V(A) <= N(R) <= V(S)
 *   --------
 */
static int l2_nr_in_range(byte l2_VA, byte rcvd_NR, byte l2_VS)
{
  register int lhs;
  register int rhs;

  lhs = (int)rcvd_NR - (int)l2_VA;
  if (lhs < 0)
  {
    lhs += 8;
  }

  rhs = (int)l2_VS - (int)l2_VA;
  if (rhs < 0)
  {
    rhs += 8;
  }

  return (lhs <= rhs);
}

/*
 *   Function name:  l2_msg_passes_filter
 *   ----------------------------------------------
 *
 *   Description: Determines whether a L3 message should be filtered
 *   ------------
 *
 *   Parameters:  msg_ptr   - pointer to L3 message buffer
 *                dcch_type - channel type indicator
 *   -----------
 *
 *   Returns:     TRUE if the msg can be forwarded to Layer 3
 *   --------
 */
static boolean l2_msg_passes_filter(byte *msg_ptr, l2_channel_type_T dcch_type)
{
  boolean msg_ok = TRUE;

  /* See 3GPP CR GP-041705: Packet Assignment on SDCCH should be ignored */
  if ((dcch_type  == SDCCH) &&
      (msg_ptr[0] == 0x06)  && /* RR protocol discriminator */
      (msg_ptr[1] == 0x4B))    /* PACKET ASSIGNMENT command */
  {
    MSG_GERAN_ERROR_0("L2 has discarded a PACKET ASSIGNMENT on SDCCH");
    msg_ok = FALSE;
  }

  return(msg_ok);
}

/*
 *   Function name:  l2_frame_length_valid
 *   ----------------------------------------------
 *
 *   Description: Validate length of incoming I-frame
 *   ------------
 *
 *   Parameters:  SAPI structure pointer and new segment length
 *   -----------
 *
 *   Returns:     TRUE if the frame has a usable length
 *   --------
 */
static boolean l2_frame_length_valid(const sapi_info_T *sapi_info, const byte new_seg_length)
{
  uint16 frame_length;
  uint16 store_length;

  /* use uint16s here to avoid byte overflows */
  frame_length = (uint16)new_seg_length;
  store_length = (uint16)(sapi_info->receive_buffer.length);

  if ((frame_length+store_length) > MAX_OCTETS_L3_MESSAGE)
  {
    /* this I-frame would cause the resulting message to exceed specified maximum */
    return(FALSE);
  }

  return(TRUE);
}

/*
 *   Function name:  l2_link_control_from_info_frame
 *   ----------------------------------------------
 *
 *   Description: Performs data link control functions resulting from receiving downlink I-frame
 *   ------------
 *
 *   Parameters:  L2 data pointer, SAPI and channel info
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
static void l2_link_control_from_info_frame(l2_store_T *l2_data_ptr, sapi_info_T *sapi_info, channel_info_T *channel_info)
{
  gas_id_t gas_id;

  if ((l2_data_ptr  == NULL) ||
      (sapi_info    == NULL) || 
      (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Invalid parameters for l2_link_control_from_info_frame");
    return;
  }

  gas_id = l2_data_ptr->gas_id;
  if (l2_data_ptr->received_NR == sapi_info->VS) /*got last frame that we sent*/
  {
    /*
     * if this is the first acknowledgement of the message
     */
    if (sapi_info->VA != l2_data_ptr->received_NR) /*got ack for last frame we sent*/
    {
      /*******************************************/
      /* The last I frame sent is acknowledged   */
      /*******************************************/
      l2_receive_new_ack(l2_data_ptr, sapi_info, channel_info, l2_data_ptr->received_NR);
    }
  }
  else
  {
    /* this normally indicates a race condition, where the NW peer sent some downlink I-frame that */
    /* crossed over with the UE's own transmission on the uplink. It is harmless, and not an error */
    /* but it does affect the way that the UE responds to any N(S) sequence error conditions below */
    MSG_GERAN_HIGH_3_G("Last frame sent was not acked (PF=%1d, OIF=%1d, BUF=%d)",
                       (int)(l2_data_ptr->received_PF),
                       (int)(channel_info->outstanding_i_frame),
                       (int)(channel_info->send_buffer.no_of_entries));
  }

  if (sapi_info->sequence_error)
  {
    byte pf_bit = l2_data_ptr->received_PF;

    /* if there is still an outstanding I-frame, it will be sent upon expiry of T200 */
    /* so an RR frame is used here to reconfirm the NW peer's previous transmissions */
    /* and the current N(S) sequence error condition will now be considered resolved */
    if (channel_info->outstanding_i_frame)
    {
      /********************************************/
      /* Code an RR response for the peer entity  */
      /********************************************/
      l2_code_rr(1, pf_bit, sapi_info, channel_info);

      /* clear the sequence error condition, the RR frame will go out and then further I-frames can be sent */
      sapi_info->sequence_error = FALSE;
      MSG_GERAN_HIGH_2_G("Seqerr ignored, send RR frame F=%d, T200 state=%d",
                         (int)pf_bit,
                         (int)(sapi_info->T200_state));
    }
    else
    {
      /********************************************/
      /* Code a REJ response for the peer entity  */
      /********************************************/
      l2_code_rej(1, pf_bit, sapi_info, channel_info);
      MSG_GERAN_HIGH_1_G("Seqerr detected, send REJ frame, T200 state=%d", (int)(sapi_info->T200_state));
    }
  }
  else if (l2_data_ptr->received_PF == 1)
  {
    /*******************************************/
    /* We shall send an RR response with F = 1 */
    /*******************************************/
    l2_code_rr(1, 1, sapi_info, channel_info);
  }
  else
  {
    /*****************************************************/
    /* If there is an I-frame available for transmission */
    /* generate the next I-frame in the transmit buffer  */
    /*****************************************************/
    if ((channel_info->outstanding_i_frame == FALSE) &&
        (channel_info->send_buffer.no_of_entries != 0))
    {
      l2_code_i(0, FALSE, sapi_info, channel_info, FALSE);

      sapi_info->retransmission_counter = 0;

      sapi_info->T200_state = TO_BE_SET;
    }
    else
    {
      /*******************************************/
      /* We shall send an RR response with F = 0 */
      /*******************************************/
      l2_code_rr(1, 0, sapi_info, channel_info);
    }
  }

  return;
}

/*
 *   Function name:  l2_handle_info_reception
 *   ----------------------------------------------
 *
 *   Description: Handle I frame
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
void l2_handle_info_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info;
  boolean         new_data_ind = FALSE;
  gas_id_t        gas_id;
  byte            l3_length;

  if (l2_data_ptr == NULL)
  {
    return;
  }

  sapi_info    = l2_get_sapi_info(l2_data_ptr, sapi);
  channel_info = l2_get_channel_info(sapi_info, channel);
  gas_id       = l2_data_ptr->gas_id;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0_G("Null SAPI/channel");
    return;
  }

  if ( (l2_data_ptr->received_NS == sapi_info->VR) &&   /* sequence number of message is correct */
       ((l2_data_ptr->received_NR == sapi_info->VS) ||  /* our last I-frame was received in sequence and successfully */
        (l2_data_ptr->received_NR == sapi_info->VA)))   /* the sender has not ack'd any new msg since its last send */
  {
    /* ensure there is enough buffer space to store the new segment */
    if (!l2_frame_length_valid(sapi_info, l2_data_ptr->l2_segment_length))
    {
      /* bail out now - treating this frame as though it never occurred */
      /* on the grounds that the UE cannot trust any of the OTA payload */
      L2_SEND_MDL_ERROR_IND(l2_data_ptr, I_FRAME_INCORRECT_LENGTH);
      MSG_GERAN_ERROR_0_G("I-frame length fails bounds check, frame ignored");
      return;
    }

    /* sequence number is correct so clear error condition */
    /* (error may also be cleared by SABM or DISC frames)  */
    sapi_info->sequence_error = FALSE;
    sapi_info->ns_dup_counter = 0;

    if (l2_data_ptr->received_M == 1)
    {
      /*******************************************************/
      /* We have to store the information field of the frame */
      /* and concatenate it with previously received frames  */
      /* with the M bit set to 1                             */
      /*******************************************************/
      l3_length = sapi_info->receive_buffer.length;

      /* l2_frame_length_valid() has already checked this length, */
      /* this double checks that the array index is within bounds */
      L2_ERR_FATAL(l3_length < MAX_OCTETS_L3_MESSAGE);

      (void)memscpy(&(sapi_info->receive_buffer.layer3_message[l3_length]),
                    sizeof(sapi_info->receive_buffer.layer3_message)-l3_length,
                    l2_data_ptr->received_l2_segment, l2_data_ptr->l2_segment_length);

      sapi_info->receive_buffer.length += l2_data_ptr->l2_segment_length;
    }
    else
    {
      /*******************************************************/
      /* We have to store the information field of the frame */
      /* and concatenate it with previously received frames  */
      /* with the M bit set to 1                             */
      /* We have to pass the complete layer 3 message to RR  */
      /*******************************************************/
      l3_length = sapi_info->receive_buffer.length;

      L2_ERR_FATAL(l3_length < MAX_OCTETS_L3_MESSAGE);

      (void)memscpy(&(sapi_info->receive_buffer.layer3_message[l3_length]),
                    sizeof(sapi_info->receive_buffer.layer3_message)-l3_length,
                    l2_data_ptr->received_l2_segment, l2_data_ptr->l2_segment_length);

      sapi_info->receive_buffer.length += l2_data_ptr->l2_segment_length;

      if (l2_msg_passes_filter(sapi_info->receive_buffer.layer3_message, l2_data_ptr->last_dcch_type))
      {
        /* data indication must be sent after acknowledgements are handled */
        new_data_ind = TRUE;
      }
      else
      {
        /* message should be discarded: reset the buffer length */
        sapi_info->receive_buffer.length = 0;
      }
    }

    /* Increment V(R) */
    sapi_info->VR = (sapi_info->VR + 1) % 8;

    /* perform data link control functions */
    l2_link_control_from_info_frame(l2_data_ptr, sapi_info, channel_info);
  }
  else if ( (
             (l2_data_ptr->received_NR == sapi_info->VS) ||    /*last I frame sent was received successfully*/
             (l2_data_ptr->received_NR == sapi_info->VA)       /*the sender has not ack any new msg since its last send*/
            ) &&
            (l2_data_ptr->received_NS !=  sapi_info->VR)       /*msg does not have sequence number we expect*/
          )
  {
    /*************************************************/
    /* An unexpected value of N(S) has been received */
    /*************************************************/
    sapi_info->sequence_error = TRUE;

    /* now determine whether this is a N(S) "sequence error", where the NW and UE are significantly */
    /* out-of-sync with respect to the acknowledgement window, or whether this is a poll for an ack */
    /* of the most recent downlink frame sent by the NW peer (i.e. NW has experienced T200 timeout) */
    if ((sapi_info->VR == ((l2_data_ptr->received_NS + 1) % 8)) &&
        (l2_data_ptr->received_PF == 1))
    {
      /* The received N(S) is something the UE has already tried to send an ack for, because (VR == N(S)+1) */
      /* so either the ack didn't get sent properly (UE dropped a TX ?) or the NW didn't receive it somehow */
      sapi_info->ns_dup_counter++;

      /* in the case of concurrent SAPI3 and SAPI0 activities on SDCCH, these repetitions may simply be */
      /* due to the limitations of the channel (1D1U every 51 frames), so account for that here as well */
      if ((SAPI3_NULL != l2_data_ptr->SAPI3_state)    &&
	      (SAPI3_IDLE != l2_data_ptr->SAPI3_state)    &&
          (SDCCH      == l2_data_ptr->last_dcch_type) &&
          (DCCH       == l2_data_ptr->current_l2_channel_type))
      {
        /* avoid the sequence error condition, it will only make the performance worse */
        sapi_info->sequence_error = FALSE;
        MSG_GERAN_HIGH_1_G("Probable T200 expiry at NW peer, SAPI%1d seqerr ignored for SDCCH", (int)sapi);
      }
    }
    else
    {
      MSG_GERAN_HIGH_3_G("N(S) seqerr NR=%d VS=%d VA=%d",l2_data_ptr->received_NR,sapi_info->VS,sapi_info->VA);
      MSG_GERAN_HIGH_3_G("N(S) seqerr NS=%d VR=%d PF=%d",l2_data_ptr->received_NS,sapi_info->VR,l2_data_ptr->received_PF);
    }

    /* 3GPP 44.006 Section 8.7.1 states:
     *   A data link layer entity which receives one or more I frames having sequence errors but otherwise error free,
     *   or subsequent supervisory frames (RR, RNR and REJ), shall use the control field information contained in the N(R)
     *   field and the P or F bit to perform data link control functions; for example, to receive acknowledgement of 
     *   previously transmitted I frames and to cause the data link layer entity to respond if the P bit is set to "1".
     */
    l2_link_control_from_info_frame(l2_data_ptr, sapi_info, channel_info);
  }
  else if (!l2_nr_in_range(sapi_info->VA, l2_data_ptr->received_NR, sapi_info->VS))
  {
    MSG_GERAN_ERROR_3_G("N(R) err VS:%d,NR:%d,NS:%d",sapi_info->VS,l2_data_ptr->received_NR,l2_data_ptr->received_NS);
    MSG_GERAN_ERROR_2_G("N(R) err VR:%d,VA:%d",sapi_info->VR,sapi_info->VA);

    /*****************************************************/
    /* A N(R) sequence error detection has been detected */
    /* Inform Layer 3 on this exception condition        */
    /*****************************************************/
    L2_SEND_MDL_ERROR_IND(l2_data_ptr, SEQUENCE_ERROR);
  }
  else
  {
    MSG_GERAN_ERROR_3_G("Drop msg VS:%d,NR:%d,NS:%d",sapi_info->VS,l2_data_ptr->received_NR,l2_data_ptr->received_NS);
    MSG_GERAN_ERROR_1_G("Drop msg VR:%d",sapi_info->VR);
  }

  /* send any new data indication AFTER any outstanding acknowledgement */
  /* such that GRR acts on the DL_DATA_ACK_IND before a new DL_DATA_IND */
  if (new_data_ind == TRUE)
  {
    l2_send_dl_data_ind(l2_data_ptr, sapi, channel);
  }

  return;
}


/*
 *   Function name:  l2_handle_normal_rr_reception
 *   ---------------------------------------------------
 *
 *   Description: Handle rr reception for normal information transfer
 *   ------------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */
void l2_handle_normal_rr_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);


  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Null SAPI/channel");
    return;
  }

  /*
   * received an new ack for a message
   */
  if (l2_data_ptr->received_NR == (sapi_info->VA+1)%8)
  {
    l2_receive_new_ack(l2_data_ptr, sapi_info, channel_info, l2_data_ptr->received_NR);
  }

  if (l2_data_ptr->received_NR == sapi_info->VS)   /*received our last frame*/
  {
    if (l2_data_ptr->received_CR == 1)
    {
      /************************************/
      /* The RR frame was a command frame */
      /************************************/

      if (l2_data_ptr->received_PF == 1)
      {
        if (sapi_info->sequence_error == FALSE)
        {
          l2_code_rr(1, 1, sapi_info, channel_info);
        }
        else
        {
          /*
           * 04.06 5.5.3.2 says that a supervisory command frame received
           * with P bit set to 1 in sequence error exception condition
           * should reply with a REJ response with F set to 1
           */
          l2_code_rej(1, 1, sapi_info, channel_info);
        }
      }
    }
  }
  else if (!l2_nr_in_range(sapi_info->VA, l2_data_ptr->received_NR, sapi_info->VS))
  {
    MSG_GERAN_HIGH_3("N(R)err VS:%d,NR:%d,NS:%d",sapi_info->VS,l2_data_ptr->received_NR,l2_data_ptr->received_NS);
    MSG_GERAN_HIGH_2("N(R)err VR:%d,VA:%d",sapi_info->VR,sapi_info->VA);
    /*****************************************************/
    /* A N(R) sequence error detection has been detected */
    /* Inform layer 3 on this exception condition        */
    /*****************************************************/

    L2_SEND_MDL_ERROR_IND(l2_data_ptr, SEQUENCE_ERROR);
  }
  else
  {
    MSG_GERAN_HIGH_3("Dropmsg VS:%d,NR:%d,NS:%d",sapi_info->VS,l2_data_ptr->received_NR,l2_data_ptr->received_NS);
    MSG_GERAN_HIGH_1("Dropmsg VR:%d",sapi_info->VR);
  }

  return;
}


/*
 *   Function name:  l2_handle_recovery_rr_reception
 *   -----------------------------------------------------
 *
 *   Description: Receive the RR acknowledgement in timer recovery
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

l2_event_T l2_handle_recovery_rr_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event  = EV_NO_EVENT;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Null SAPI/channel");
    return(EV_NO_EVENT);
  }

  if (l2_data_ptr->received_NR == (sapi_info->VA+1)%8)
  {
    l2_receive_new_ack(l2_data_ptr, sapi_info, channel_info, l2_data_ptr->received_NR);
  }

  if (l2_nr_in_range(sapi_info->VA, l2_data_ptr->received_NR, sapi_info->VS))
  {
    /* A frame has been sent but rej did not ack it, resend frame immediately */
    if (channel_info->outstanding_i_frame==TRUE)
    {
      /*Increment by 1 since on retransmission we code it such that VS has 1 subtracted from it*/
      sapi_info->VS = (l2_data_ptr->received_NR+1)%8;
    }
    else
    {
      sapi_info->VS = l2_data_ptr->received_NR;
    }

    if (l2_data_ptr->received_CR == 1)
    {
      /************************************/
      /* The RR frame was a command frame */
      /************************************/
      if (l2_data_ptr->received_PF == 1)
      {
        if (sapi_info->sequence_error == FALSE)
        {
          l2_code_rr( 1, 1, sapi_info, channel_info);
        }
        else
        {
          l2_code_rej(1, 1, sapi_info, channel_info);
        }
      }
    }
    else
    {
      /*************************************/
      /* The RR frame was a response frame */
      /*************************************/
      if (l2_data_ptr->received_PF == 1)
      {

        /************************/
        /* Reset the timer T200 */
        /************************/
        l2_clear_T200_timer(sapi_info);

        l2_event = EV_CLEAR_RECOVERY_CONDITION;
      }
    }
  }
  else
  {
    /* Section 5.5.7 of 04.06 states:
     * The timer recovery condition is only cleared if the data link layer entity receives
     * a valid supervisory frame response with the F bit set to "1".
     * If the N(R) of this received supervisory frame is within the range from
     * its current state variable V(A) to its current send state variable V(S) inclusive,
     * it shall set its send state variable V(S) to the value of the received N(R).
     * Timer T200 shall be reset if the received supervisory frame response is an RR or
     * REJ response with F bit set to "1".
     * The data link layer entity shall then resume with I frame transmission or retransmission,
     * as appropriate.
     */
    if (l2_data_ptr->received_PF == 1)
    {
      MSG_GERAN_HIGH_3("N(R) out of range(%1d,%1d,%1d) but PF=1", sapi_info->VA, l2_data_ptr->received_NR, sapi_info->VS);

      /************************/
      /* Reset the timer T200 */
      /************************/
      l2_clear_T200_timer(sapi_info);
      l2_event = EV_CLEAR_RECOVERY_CONDITION;
    }
    else
    {
      MSG_GERAN_HIGH_3("N(R)err VS:%d NR:%d NS:%d",sapi_info->VS,l2_data_ptr->received_NR,l2_data_ptr->received_NS);
      MSG_GERAN_HIGH_3("N(R)err VR:%d VA:%d Ch:%d",sapi_info->VR,sapi_info->VA,(int)channel);

      /*****************************************************/
      /* A N(R) sequence error detection has been detected */
      /* Inform layer 3 on this exception condition        */
      /*****************************************************/
      L2_SEND_MDL_ERROR_IND(l2_data_ptr, SEQUENCE_ERROR);
    }
  }

  return(l2_event);
}


/*
 *   Function name:  l2_handle_normal_rej_reception
 *   ----------------------------------------------------
 *
 *   Description: Handle REJ reception in normal transfer mode
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_handle_normal_rej_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr,sapi);
  
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Null sapi/channel");
    return;
  }

  /* validate the NR value before using it */
  if (!l2_nr_in_range(sapi_info->VA, l2_data_ptr->received_NR, sapi_info->VS))
  {
    MSG_GERAN_ERROR_3("REJ N(R)=%d sequence error VA=%d VS=%d", (int)l2_data_ptr->received_NR, (int)(sapi_info->VA), (int)(sapi_info->VS));
    L2_SEND_MDL_ERROR_IND(l2_data_ptr, SEQUENCE_ERROR);
    return;
  }

  /*Cancel timer*/
  l2_clear_T200_timer(sapi_info);

  /*check if ack any new messages*/
  if (l2_data_ptr->received_NR == (sapi_info->VA+1)%8)
  {
    l2_receive_new_ack(l2_data_ptr, sapi_info,channel_info,l2_data_ptr->received_NR);
  }

  if (l2_data_ptr->received_NR != sapi_info->VS)
  {
    /*A frame has been sent but rej did not ack it, resend frame immediately*/
    if (channel_info->outstanding_i_frame==TRUE)
    {
      /*Added 1 because on retransmission we code it such that VS has 1 subtracted from it*/
      sapi_info->VS = (l2_data_ptr->received_NR+1)%8;

      l2_handle_I_retransmission(l2_data_ptr,sapi,channel);
    }
    else
    {
      /*reset send sequence number*/
      sapi_info->VS = l2_data_ptr->received_NR;
    }
  }

  if (l2_data_ptr->received_CR == 1)
  {
    /******************************/
    /* The REJ is a command frame */
    /******************************/
    if (l2_data_ptr->received_PF == 1)
    {
      /*Need to send out rej frame, will retransmit
        I frame once timer expires*/
      if (sapi_info->sequence_error == FALSE)
      {
        l2_code_rr(1, 1, sapi_info, channel_info);
      }
      else
      {
        l2_code_rej(1, 1, sapi_info, channel_info);
      }
    }
  }
  else
  {
    /*******************************/
    /* The REJ is a response frame */
    /*******************************/

    if (l2_data_ptr->received_PF == 1)
    {
      /* harmless, drop it */
      L2_DROP_MDL_ERROR_IND(UNSOLICITED_SUPERVISORY_RESPONSE);
    }
  }

  return;
}

/*
 *   Function name:  l2_handle_recovery_rej_reception
 *   ------------------------------------------------------
 *
 *   Description: Handle REJ reception in recovery mode
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 */

l2_event_T l2_handle_recovery_rej_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event = EV_NO_EVENT;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0("Null sapi/channel");
    return(l2_event);
  }

  if (sapi_info->VA != l2_data_ptr->received_NR)
  {
    l2_receive_new_ack(l2_data_ptr, sapi_info, channel_info, l2_data_ptr->received_NR);
  }

  if ((l2_data_ptr->received_CR == 0) && (l2_data_ptr->received_PF == 1))
  {
    l2_clear_T200_timer(sapi_info);

    l2_event = EV_CLEAR_RECOVERY_CONDITION;

    if (sapi_info->VS != l2_data_ptr->received_NR)
    {
      /* need to retransmit the last frame */
      if (channel_info->outstanding_i_frame==TRUE)
      {
        sapi_info->VS = (l2_data_ptr->received_NR+1)%8;
        l2_handle_I_retransmission(l2_data_ptr,sapi,channel);
      }
      else
      {
        sapi_info->VS = l2_data_ptr->received_NR;
        MSG_GERAN_HIGH_0("Timer rec but no I frame");
      }

    }
  }
  else if (l2_data_ptr->received_CR == 1)
  {
    /*VA already set to NR when processing ack*/

    /******************************/
    /* The REJ is a command frame */
    /******************************/
    if (l2_data_ptr->received_PF == 1)
    {
      if (sapi_info->sequence_error == FALSE)
      {
        l2_code_rr(1, 1, sapi_info, channel_info);
      }
      else
      {
        l2_code_rej(1, 1, sapi_info, channel_info);
      }
    }

  }

  return(l2_event);
}

/*
 *   Function name:  l2_update_N200_channel_type
 *   ------------------------------------------
 *
 *   Description: Update the DCCH channel type used for N200 determination
 *   ------------
 *
 *   Parameters:  channel: SDCCH, FACCH_F or FACCH_H
 *   -----------
 *
 *   Returns:     VOID
 *   --------
 */
void l2_update_N200_channel_type(l2_store_T *l2_data_ptr, l2_channel_type_T channel)
{
  switch(channel)
  {
  case FACCH_F:
    l2_data_ptr->N200_dcch_type = FULL_RATE_TRAFFIC;
    break;

  case FACCH_H:
    l2_data_ptr->N200_dcch_type = HALF_RATE_TRAFFIC;
    break;

  default:
    l2_data_ptr->N200_dcch_type = SDCCH_8;
    break;
  }

  return;
}

/*
 *   Function name:  l2_check_pending_sequence_error
 *   ------------------------------------------
 *
 *   Description: Sanity check on N(S) sequence error condition
 *   ------------
 *
 *   Parameters:  L2 data pointer, SAPI and channel
 *   -----------
 *
 *   Returns:     EV_NO_EVENT
 *   --------
 */
l2_event_T l2_check_pending_sequence_error(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info;
  gas_id_t        gas_id;

  if (l2_data_ptr == NULL)
  {
    return(EV_NO_EVENT);
  }

  sapi_info    = l2_get_sapi_info(l2_data_ptr, sapi);
  channel_info = l2_get_channel_info(sapi_info, channel);
  gas_id       = l2_data_ptr->gas_id;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0_G("Null SAPI/channel");
    return(EV_NO_EVENT);
  }

  if (sapi_info->sequence_error == TRUE)
  {
    /* N(S) sequence error condition was previously detected
     * if there is no valid REJ frame present in the channel
     * transmit buffer, emit a diagnostic warning here
     */
    if ((channel_info->transmit_buffer.valid_data_flag == FALSE) ||
        (channel_info->transmit_buffer.frame_type      != REJ))
    {
      MSG_GERAN_HIGH_1_G("SAPI%1d transmit buffer has no valid REJ frame", (int)sapi);
    }
  }

  return(EV_NO_EVENT);
}

/*
 *   Function name:  l2_handle_I_retransmission
 *   ------------------------------------------
 *
 *   Description: Retransmit an I frame
 *   ------------
 *
 *   Parameters:  SAPI and channel
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_handle_I_retransmission(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel )
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info;
  gas_id_t        gas_id;
  byte            N200;

  if (l2_data_ptr == NULL)
  {
    return;
  }

  sapi_info    = l2_get_sapi_info(l2_data_ptr, sapi);
  channel_info = l2_get_channel_info(sapi_info, channel);
  gas_id       = l2_data_ptr->gas_id;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    MSG_GERAN_ERROR_0_G("Null SAPI/channel");
    return;
  }

  /* Determine the N200 retransmission counter */
  /* as per 3GPP 44.006 section 8.8.2.1        */
  if (channel == SACCH)
  {
    N200 = 5;
  }
  else
  {
    /* channel should be DCCH, determine whether FACCH or SDCCH is in use */
    switch(l2_data_ptr->N200_dcch_type)
    {
    case FULL_RATE_TRAFFIC:
      N200 = 34;
      break;

    case HALF_RATE_TRAFFIC:
      N200 = 29;
      break;

    default:
      N200 = 23;
      if ((l2_data_ptr->N200_dcch_type != SDCCH_8) &&
          (l2_data_ptr->N200_dcch_type != SDCCH_4))
      {
        MSG_GERAN_ERROR_3_G("L2 channel type error %d %d %d",
                  (int)channel,
                  (int)l2_data_ptr->N200_dcch_type,
                  (int)l2_data_ptr->last_dcch_type);
        /* default to SDCCH_8 for N200 */
        l2_data_ptr->N200_dcch_type = SDCCH_8;
      }
      break;
    }
  }

  /********************************************/
  /* T200 has expired: retransmit the I frame */
  /********************************************/
  if (channel_info->outstanding_i_frame == TRUE)
  {
    if (sapi_info->retransmission_counter == N200)
    {
      MSG_GERAN_ERROR_2_G("SAPI%1d max retransmissions (N200=%d)",
                          (int)sapi,(int)N200);
      L2_SEND_MDL_ERROR_IND(l2_data_ptr, N200_EXCEEDED_PAR);
    }
    else
    {
      /*
       * we can only do the retransmission here if there are no
       * supervisory frames pending, so check the buffer first. We
       * can still schedule the timer, so that this function can
       * be entered next time to do the retransmission
       * UNLESS its a optional RR frame, then this can be overridden
       *
       */
      if ((channel_info->transmit_buffer.valid_data_flag == FALSE) ||
          (l2_check_frame_optional(channel_info)         == TRUE))
      {
        MSG_GERAN_HIGH_3_G("SAPI%1d retrans %d (N200=%d)",
                           (int)sapi,(int)sapi_info->retransmission_counter,(int)N200);

        sapi_info->retransmission_counter = sapi_info->retransmission_counter + 1;

        /************************************************/
        /* The P bit of the I frame has to be set to 1. */
        /************************************************/
        l2_code_i(1, TRUE, sapi_info, channel_info, TRUE);
        sapi_info->T200_state = TO_BE_SET;
      }
      else
      {
        /* retransmit outstanding I-frames at earliest */
        /* opportunity per 3GPP 44.006 section 8.8.1.1 */
        sapi_info->T200_state = TO_BE_SET_ASAP;
        MSG_GERAN_HIGH_3_G("SAPI%1d deferred retransmission on chan=%1d last DCCH=%d",
                           (int)sapi,
                           (int)channel,
                           (int)(l2_data_ptr->last_dcch_type));
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("SAPI%1d has no outstanding I-frame to retransmit", (int)sapi);
  }

  return;
}


