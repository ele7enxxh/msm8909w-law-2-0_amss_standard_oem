/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   M E S S A G E

GENERAL DESCRIPTION
   This module contains functions that are related to decoding,
   encoding, receiving, and sending a frame.

EXTERNALIZED FUNCTIONS
   l2_decode_frame
   l2_clear_send_queue
   l2_clear_send_buffer
   l2_store_new_l3_message
   l2_code_sabm
   l2_code_disc
   l2_code_dm
   l2_code_ua
   l2_code_ui
   l2_code_i
   l2_code_rr
   l2_code_rej
   l2_next_segment

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_msg.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
10/17/12   SJW      Improved length validation for message buffer handling
07/22/11   SJW      Debug update for Cell Broadcast
05/26/11   SJW      Disable random padding in DSDS builds
05/17/11   SJW      Avoid bad frame debug for CBCH blocks
29/10/10   SJW      Added support for Dual SIM
07/22/09   SJW      Added GL2_ALLOC and GL2_FREE macros for new heap manager
02/11/03   JAC      Added short header support
12/09/02   JAC      Added sending dl_data_ack_ind in l2_next_segment
10/18/02   JAC      Cleaned up segmentation code
10/17/02   JAC      Moved shared functions to l2_transmit.c
10/16/02   JAC      Put back reset of sequence error condition after send rej
10/04/02   JAC      Code reorg
03/25/02   JAC      Increased MSG levels
01/25/02   JAC      Modified l3_message_to_send length to MAX_OCTETS_L3_MESSAGE
12/17/01   JAC      l2_segment_length not set correctly in l2_decode_frame
12/14/01   JAC      Removed PERLUTF feature for timers
                    Set i frame type for sacch sapi3 in l2_forward_data
                    Modified length check for disc frame
                    Modified l2_clear_send_queue to delete all messages
10/19/01   JAC      Initial version
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
#include <stdlib.h>
#include "stringl/stringl.h"
#include "environ.h"
#include "amssassert.h"
#include "msg.h"
#include "l2i.h"
#include "l2_msg.h"
#include "l2_transfer.h"
#include "l2_send.h"
#ifndef FEATURE_L2_SOFTWARE_PRNG
#include "secapi.h"
#endif /* !FEATURE_L2_SOFTWARE_PRNG */

/*****************************************************

 MACROS

******************************************************/
#define L2_PAD_FRAME(buf_ptr,oct_cnt,gsid) l2_generate_filler((buf_ptr),(oct_cnt),(gsid))
#define XOR_PARAM_A (15UL)
#define XOR_PARAM_B (4UL)
#define XOR_PARAM_C (21UL)

/*
 *   Function name:  l2_decode_frame
 *   --------------------------------
 *
 *   Description: Decode and check the syntax of a layer 2 frame
 *   ------------
 *
 *
 *   Parameters:  l2_frame - pointer to rcvd frame
 *   -----------  frame_type - pointer to return parameter of decoded frame type
 *                pSAPI - pointer to return parameter of SAPI
 *                valid_frame - pointer to return parameter of valid_frame flag
 *
 *   Returns:  none
 *   --------
 *
 *
 */

void l2_decode_frame(l2_store_T     *l2_data_ptr,
                     byte           *l2_frame,
                     frame_type_T   *frame_type,
                     SAPI_T         *pSAPI,
                     boolean        *valid_frame,
                     uint32          cbch_frame_no)
{
  byte LPD;
  byte SAPI;
  byte EA;
  byte EL;
  byte N201;
  byte length;
  register byte control_field;
  register byte address_field;
  byte     length_field;
  boolean  filler_frame = FALSE;
  gas_id_t gas_id;
  
  /* validate input pointers */
  if ((l2_data_ptr == NULL) ||
      (l2_frame    == NULL) ||
      (frame_type  == NULL) ||
      (pSAPI       == NULL) ||
      (valid_frame == NULL))
  {
    MSG_GERAN_ERROR_0("Invalid pointer parameter to l2_decode_frame()");
    return;
  }

  /* extract the GAS ID to enable GERAN diag macros */
  gas_id = l2_data_ptr->gas_id;
  *valid_frame = FALSE;

  if (l2_data_ptr->current_l2_channel_type == SACCH)
  {
    N201 = N201_SACCH;
  }
  else if (l2_data_ptr->current_l2_channel_type == DCCH)
  {
    N201 = N201_DCCH;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unexpected channel type passed to decoder");
    return;
  }

  /*************************************************/
  /* Check for Bter format first, has a short      */
  /* header type that does not follow the other    */
  /* formats                                       */
  /*************************************************/
  if ( (l2_data_ptr->current_l2_channel_type == SACCH) && ((l2_frame[0] & 0x03) == 0x00))
  {
    MSG_GERAN_HIGH_3_G("Short header %d %d %d",l2_frame[0],l2_frame[1],l2_frame[2]);
    *frame_type = UI_SHORT_HEADER;
    *pSAPI = 0;
    *valid_frame = TRUE;
    l2_data_ptr->l2_segment_length = N201_SACCH_BTER;
    l2_data_ptr->received_l2_segment = &l2_frame[0];
    return;
  }

  /****************************/
  /* Decode the Address Field */
  /****************************/

  address_field = l2_frame[0];

  LPD  = (byte) (address_field & 0x60) >> 5;
  SAPI = (byte) (address_field & 0x1C) >> 2;
  l2_data_ptr->received_CR = (byte) (address_field & 0x02) >> 1;
  EA = address_field & 0x01;

  /****************************/
  /* Decode the Control Field */
  /****************************/

  control_field = l2_frame[1];

  /***************************/
  /* Decode the Length Field */
  /***************************/

  length_field = l2_frame[2];

  length      = (byte) (length_field & 0xFC) >> 2;
  l2_data_ptr->received_M  = (byte) (length_field & 0x02) >> 1;
  EL          = length_field & 0x01;

  if (((SAPI == 0) || (SAPI == 3)) &&
      (LPD == 0) && (EA == 1) && (EL == 1))
  {
    if ((control_field & 0x01) == 0)
    {
      /***************************/
      /* The frame is an I frame */
      /***************************/

      if (l2_data_ptr->received_CR == 1)
      {
        l2_data_ptr->received_NS = (byte) (control_field & 0x0E) >> 1;
        l2_data_ptr->received_NR = (byte) (control_field & 0xE0) >> 5;
        l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

        if (SAPI == 0)
        {
          MSG_GERAN_HIGH_3_G("I-frm0:    N(R):%1d N(S):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_NS,l2_data_ptr->received_PF);
        }
        else
        {
          MSG_GERAN_HIGH_3_G("I-frm3:    N(R):%1d N(S):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_NS,l2_data_ptr->received_PF);
        }

        if ((length > N201) || (length == 0))
        {
          L2_SEND_MDL_ERROR_IND(l2_data_ptr, I_FRAME_INCORRECT_LENGTH);
        }
        else if ((length < N201) && (l2_data_ptr->received_M == 1))
        {
          L2_SEND_MDL_ERROR_IND(l2_data_ptr, I_FRAME_INCORRECT_M_BIT);
        }
        else
        {
          l2_data_ptr->l2_segment_length = length;
          l2_data_ptr->received_l2_segment = &l2_frame[L2_HEADER_LENGTH];

          *frame_type = I;
          *pSAPI = SAPI;
          *valid_frame = TRUE;
        }
      }
      else
      {
        /***********************************/
        /* The frame is not defined in the */
        /* GSM specification               */
        /***********************************/

        L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);

        ERR("Invalid I frame:This is a response frame",0,0,0);
      }
    }
    else
    {
      if (((byte) (control_field & 0x02) >> 1) == 0)
      {
        if (((byte) (control_field & 0x0C) >> 2) == 3)
        {
          L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
        }
        else
        {
          /**************************/
          /* The frame is a S frame */
          /**************************/

          l2_data_ptr->received_NR = (byte) (control_field & 0xE0) >> 5;
          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if ((l2_data_ptr->received_M == 1) || (length != 0))
          {
            L2_SEND_MDL_ERROR_IND(l2_data_ptr, S_FRAME_INCORRECT_PARAMETERS);
          }
          else if (((byte) (control_field & 0x0C) >> 2) == 0)
          {
            /***************************/
            /* The frame is a RR frame */
            /***************************/
            if (SAPI == 0)
            {
              MSG_GERAN_HIGH_3_G("S-frm0:RR  N(R):%1d C(R):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_CR,l2_data_ptr->received_PF);
            }
            else
            {
              MSG_GERAN_HIGH_3_G("S-frm3:RR  N(R):%1d C(R):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_CR,l2_data_ptr->received_PF);
            }

            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = RR;
          }
          else if (((byte) (control_field & 0x0C) >> 2) == 2)
          {
            /****************************/
            /* The frame is a REJ frame */
            /****************************/
            if (SAPI == 0)
            {
              MSG_GERAN_HIGH_3_G("S-frm0:REJ N(R):%1d C(R):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_CR,l2_data_ptr->received_PF);
            }
            else
            {
              MSG_GERAN_HIGH_3_G("S-frm3:REJ N(R):%1d C(R):%1d P(F):%1d",l2_data_ptr->received_NR,l2_data_ptr->received_CR,l2_data_ptr->received_PF);
            }

            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = REJ;
          }
        }
      }
      else
      {
        /************************************/
        /* The frame is an UNNUMBERED frame */
        /************************************/

        if ((((byte) (control_field & 0x0F) == 0x03)) &&
            (((byte) (control_field & 0xE0) >> 5) == 0))
        {
          /****************************/
          /* The frame is an UI frame */
          /* Format 000P0011 binary   */
          /****************************/

          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if (l2_data_ptr->received_M == 1)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (length > N201)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (l2_data_ptr->received_CR == 0)
          {
            L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
          }
          else if (length != 0)
          {
            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = UI;

            l2_data_ptr->l2_segment_length = length;
            l2_data_ptr->received_l2_segment = &l2_frame[L2_HEADER_LENGTH];
          }
          else
          {
            /* filler frame */
            filler_frame = TRUE;
          }
          /* else do nothing */
        }
        else if ((((byte) (control_field & 0x0F) == 0x0F)) &&
                 (((byte) (control_field & 0xE0) >> 5) == 1))
        {
          /*****************************/
          /* The frame is a SABM frame */
          /* Format 001P1111 binary    */
          /*****************************/

          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if (l2_data_ptr->received_M == 1)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (length > N201)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (l2_data_ptr->received_CR == 0)
          {
            L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
          }
          else
          {
            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = SABM;
            l2_data_ptr->l2_segment_length = length;
          }
        }
        else if ((((byte) (control_field & 0x0F) == 0x03)) &&
                 (((byte) (control_field & 0xE0) >> 5) == 3))
        {
          /***************************/
          /* The frame is a UA frame */
          /* Format 011F0011 binary  */
          /***************************/

          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if (l2_data_ptr->received_M == 1)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (length > N201)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (l2_data_ptr->received_CR == 1)
          {
            L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
          }
          else
          {
            l2_data_ptr->l2_segment_length = length;

            if (length != 0)
            {
              l2_data_ptr->received_l2_segment = &l2_frame[L2_HEADER_LENGTH];
            }
            else
            {
              l2_data_ptr->received_l2_segment = NULL;
            }

            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = UA;
          }
        }
        else if ((((byte) (control_field & 0x0F) == 0x03)) &&
                 (((byte) (control_field & 0xE0) >> 5) == 2))
        {
          /*****************************/
          /* The frame is a DISC frame */
          /* Format 010P0011 binary    */
          /*****************************/

          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if (l2_data_ptr->received_M == 1)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (length != 0)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (l2_data_ptr->received_CR == 0)
          {
            L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
          }
          else
          {
            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = DISC;
          }
        }
        else if ((((byte) (control_field & 0x0F) == 0x0F)) &&
                 (((byte) (control_field & 0xE0) >> 5) == 0))
        {
          /***************************/
          /* The frame is a DM frame */
          /* Format 000F1111 binary  */
          /***************************/

          l2_data_ptr->received_PF = (byte) (control_field & 0x10) >> 4;

          if (l2_data_ptr->received_M == 1)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (length != 0)
          {
            L2_DROP_MDL_ERROR_IND(U_FRAME_INCORRECT_PARAMETERS);
          }
          else if (l2_data_ptr->received_CR == 1)
          {
            L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
          }
          else
          {
            *valid_frame = TRUE;
            *pSAPI = SAPI;
            *frame_type = DM;
          }
        }
        else
        {
          L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
        }
      }
    }
  }
  else if (LPD == 1)
  {
    /*********************************************/
    /* The message is a Cell Broadcast block, if */
    /* the frame number is valid, send it to CB  */
    /*********************************************/
    if (cbch_frame_no != 0)
    {
      l2_send_dl_cb_block_ind(l2_data_ptr, l2_frame, cbch_frame_no);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Frame number for CB Block is 0, ignored");
      filler_frame = TRUE;
    }
  }
  else
  {

    /***********************************************/
    /* The EA bit = 0 is not valid in GSM          */
    /* The EL bit = 0 is not valid in GSM          */
    /* The SAPI should be 0 or 3                   */
    /* The LPD <> from CB and LAPDm is not allowed */
    /***********************************************/

    if ((EA == 0) || (EL == 0))
    {
      L2_DROP_MDL_ERROR_IND(FRAME_NOT_IMPLEMENTED);
    }
  }

  if ((*valid_frame == FALSE) && (filler_frame == FALSE) && (LPD != 1))
  {
    MSG_GERAN_HIGH_3_G("Could not decode ch %1d,0x%02X,0x%02X",l2_data_ptr->current_l2_channel_type,l2_frame[0],l2_frame[1]);
    MSG_GERAN_HIGH_3_G("Bad frame 0x%02X,0x%02X,0x%02X",l2_frame[2],l2_frame[3],l2_frame[4]);
  }

  return;
}


/*
 *   Function name:  l2_clear_send_queue
 *   -----------------------------------
 *
 *   Description: Clear the queue of layer 3 messages
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_clear_send_queue(sapi_info_T *sapi_info, l2_channel_type_T channel)
{
  message_entry_T *entry;
  message_entry_T *new_entry;
  channel_info_T  *channel_info;
  send_queue_T    *current_queue;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if (channel_info == NULL)
  {
    return;
  }

  current_queue = &(channel_info->send_queue);

  if (current_queue->no_of_entries != 0)
  {
    entry = current_queue->first_message;

    while (entry != NULL)
    {
      new_entry = entry->next_message;
      GL2_FREE(entry->message);
      GL2_FREE(entry);
      entry = new_entry;
    }

    current_queue->no_of_entries = 0;
    current_queue->first_message = NULL;
  }

  return;
}

/*
 *   Function name:  l2_clear_send_buffer
 *   ------------------------------------
 *
 *   Description: Clear the send buffer
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_clear_send_buffer(sapi_info_T *sapi_info, l2_channel_type_T channel)
{
  segment_descriptor_T *current_descriptor = NULL;
  segment_descriptor_T *new_descriptor     = NULL;
  channel_info_T       *channel_info;
  send_buffer_T        *send_buffer;
  
  channel_info = l2_get_channel_info(sapi_info, channel);
  if (channel_info == NULL)
  {
    return;
  }

  send_buffer = &(channel_info->send_buffer);

  if (send_buffer->no_of_entries != 0)
  {
    current_descriptor = send_buffer->first_segment;

    while (current_descriptor != NULL)
    {
      new_descriptor = current_descriptor->next;
      GL2_FREE(current_descriptor->segment);
      GL2_FREE(current_descriptor);
      current_descriptor = new_descriptor;
    }

    send_buffer->no_of_entries = 0;
    send_buffer->first_segment = NULL;
    send_buffer->next_segment = NULL;
  }

  return;
}


/*
 *   Function name:  l2_segment_message
 *   ---------------------------------------
 *
 *   Description: Segment a L3 message and
 *   ------------ store it in the send_buffer
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

static void l2_segment_message(channel_info_T *channel_info, message_entry_T *message)
{
  segment_descriptor_T *current_descriptor = NULL;
  segment_descriptor_T *segment_descriptor;
  send_buffer_T        *send_buffer;
  send_queue_T         *send_queue;
  byte                 *message_pointer;
  byte                  remaining_length;
  byte                  N201;

  if (channel_info == NULL)
  {
    return;
  }

  send_buffer = &(channel_info->send_buffer);
  send_queue  = &(channel_info->send_queue);
  N201        = channel_info->N201;

  if ((send_queue == NULL) || (send_buffer == NULL))
  {
    MSG_GERAN_ERROR_1("Invalid queue or buffer for channel %d",(int)channel_info->channel);
    return;
  }

  if (send_buffer->no_of_entries != 0)
  {
    /* Should never happen */
    MSG_GERAN_ERROR_1("Send buffer not empty for channel %d",(int)channel_info->channel);
    send_buffer->no_of_entries = 0;
  }

  /**************************************/
  /* Add the message in the send_buffer */
  /**************************************/
  remaining_length = message->message_length;
  message_pointer  = message->message;

  while (remaining_length != 0)
  {
    /* Segment descriptor points to the new segment */
    segment_descriptor = (segment_descriptor_T *)GL2_ALLOC(sizeof(segment_descriptor_T));

    if (segment_descriptor == NULL)
    {
      MSG_GERAN_ERROR_0("Failed to allocate memory for segment descriptor");
      return;
    }

    if (remaining_length > N201)
    {
      /* Not the last segment */
      segment_descriptor->segment_length = N201;

      segment_descriptor->segment = (byte *)GL2_ALLOC(N201);

      if (segment_descriptor->segment != NULL)
      {
        (void)memscpy(segment_descriptor->segment,
                      segment_descriptor->segment_length,
                      message_pointer, N201);
      }
      else
      {
        MSG_GERAN_ERROR_0("Failed to allocate memory for msg segment");
        segment_descriptor->segment_length = 0;
      }

      message_pointer += N201;
      remaining_length -= N201;
    }
    else
    {
      /* Last segment */
      segment_descriptor->segment_length = remaining_length;

      segment_descriptor->segment = (byte *)GL2_ALLOC(remaining_length);

      if (segment_descriptor->segment != NULL)
      {
        (void)memscpy(segment_descriptor->segment, remaining_length,
                      message_pointer, remaining_length);
      }
      else
      {
        MSG_GERAN_ERROR_0("Failed to allocate memory for msg segment");
        segment_descriptor->segment_length = 0;
      }

      message_pointer = NULL;
      remaining_length = 0;
    }

    segment_descriptor->next = NULL;

    if (send_buffer->no_of_entries == 0)
    {
      /********************************************************/
      /* First segment, need to adjust pointers. The next     */
      /* segment pointer in send buffer points to the segment */
      /* that should be sent next, this is different than the */
      /* next segment pointer in the segment_descriptor       */
      /********************************************************/
      send_buffer->first_segment = segment_descriptor;
      send_buffer->next_segment = segment_descriptor;
      current_descriptor = send_buffer->first_segment;
    }
    else
    {
      /********************************************************************/
      /* Set next pointer to new segment, current descriptor will not be  */
      /* null but we need this check to get rid of lint warnings          */
      /********************************************************************/
      if (current_descriptor != NULL)
      {
        current_descriptor->next = segment_descriptor;
        current_descriptor = segment_descriptor;
      }
    }

    send_buffer->no_of_entries++;
  }
}

/*
 *   Function name:  l2_store_new_l3_message
 *   ---------------------------------------
 *
 *   Description: Stores a L3 message
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_store_new_l3_message(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel, suspension_state_T suspension_state)
{
  message_entry_T *current_message;
  message_entry_T *new_message_entry;
  sapi_info_T     *sapi_info    = l2_get_sapi_info(l2_data_ptr, sapi);
  channel_info_T  *channel_info;
  send_buffer_T   *send_buffer;
  send_queue_T    *send_queue;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if (channel_info == NULL)
  {
    return;
  }

  send_buffer = &(channel_info->send_buffer);
  send_queue  = &(channel_info->send_queue);

  /******************************************************/
  /* Allocate memory for message and copy over contents */
  /******************************************************/
  new_message_entry = (message_entry_T *)GL2_ALLOC(sizeof(message_entry_T));
  if (new_message_entry == NULL)
  {
    MSG_GERAN_ERROR_1("Unable to allocate new msg entry for channel %d",(int)channel_info->channel);
    return;
  }

  new_message_entry->next_message = NULL;
  new_message_entry->message_length = l2_data_ptr->l3_message_length;

  /***********************************************************/
  /* Used by RR to keep track of what L2 has sent for use in */
  /* interrat handover                                       */
  /***********************************************************/
  new_message_entry->message_number = l2_data_ptr->l3_message_number;

  new_message_entry->message = (byte *)GL2_ALLOC(l2_data_ptr->l3_message_length);

  if (new_message_entry->message != NULL)
  {
    L2_ERR_FATAL(l2_data_ptr->l3_message_length <= MAX_OCTETS_L3_MESSAGE);
    (void)memscpy(new_message_entry->message, l2_data_ptr->l3_message_length,
                  l2_data_ptr->l3_message_to_send, l2_data_ptr->l3_message_length);
  }
  else
  {
    MSG_GERAN_ERROR_1("Unable to allocate new msg for channel %d",(int)channel_info->channel);
  }

  /************************/
  /* Add it to send queue */
  /************************/
  if (send_queue->no_of_entries != 0)
  {
    /********************************************/
    /* Add the message at the end of send_queue */
    /********************************************/
    current_message = send_queue->first_message;

    while (current_message->next_message != NULL)
    {
      current_message = current_message->next_message;
    }

    current_message->next_message = new_message_entry;

    send_queue->no_of_entries++;

    /********************************************/
    /* Only add segment to send buffer if it is */
    /* empty and the SAPI is not suspended      */
    /********************************************/
    if ((suspension_state != SAPI_SUSPENDED) &&
        (send_buffer->no_of_entries == 0))
    {
      l2_segment_message(channel_info, send_queue->first_message);
    }
  }
  else
  {
    /*************************************/
    /* Empty queue, add message to front */
    /*************************************/
    send_queue->first_message = new_message_entry;

    send_queue->no_of_entries = 1;

    if (send_buffer->no_of_entries == 0)
    {
      /**************************************/
      /* Add the message in the send_buffer */
      /**************************************/
      l2_segment_message(channel_info, send_queue->first_message);
    } /* empty send_buffer */
  }    /* empty send_queue */

  /**********************************/
  /* The message to send is stored. */
  /* Delete the store               */
  /**********************************/
  l2_data_ptr->l3_message_length = 0;

  return;
}


/*
 *   Function name:  l2_next_segment
 *   -------------------------------
 *
 *   Description: Set the next segment to send
 *   ------------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_next_segment(l2_store_T *l2_data_ptr, sapi_info_T *sapi_info, channel_info_T *channel_info)
{
  segment_descriptor_T *segment_descriptor;
  message_entry_T      *first_message;
  message_entry_T      *next_message;
  send_buffer_T        *send_buffer;
  send_queue_T         *send_queue;

  if (channel_info == NULL)
  {
    return;
  }

  send_buffer = &(channel_info->send_buffer);
  send_queue  = &(channel_info->send_queue);

  /*******************************************************/
  /* if we are suspended the send buffer will be empty,  */
  /* we do not want to queue up a segment to send in     */
  /* that case                                           */
  /*******************************************************/
  if (send_buffer->no_of_entries != 0)
  {
    /*
     * get pointer to segment just transmitted
     * If it isnt the last one for the current entry in send_queue,
     * then set next_segment to point at the succeeding segment.
     * otherwise, we need to remove the current entry in send_queue, and
     * set up the segment parts corresponding to the next entry in send_queue
     * if there is one.
     */

    segment_descriptor = send_buffer->next_segment;

    if (segment_descriptor->next != NULL)
    {
      /*
       * not the last segment for the message
       */
      send_buffer->next_segment = segment_descriptor->next;
    }
    else
    {
      /*
       * All segments of the message have been transmitted,
       * tell RR, clear the old segmants, and
       * remove the old message from the send queue
       */

      /* tell RR msg has been acknowledged by network*/
      l2_send_dl_data_ack_ind(l2_data_ptr, sapi_info->sapi, send_queue->first_message);

      l2_clear_send_buffer(sapi_info, channel_info->channel);

      first_message = send_queue->first_message;
      next_message = first_message->next_message;

      GL2_FREE(first_message->message);
      GL2_FREE(first_message);

      send_queue->first_message = next_message;

      send_queue->no_of_entries -= 1;

      if (send_queue->no_of_entries != 0)
      {
        /*
         * There is another message in the send queue, so cut it into
         * segments, and store it in the send buffer
         */
        l2_segment_message(channel_info,send_queue->first_message);
      }
    }
  }
}

/*
 *   Function name:  l2_xor_get_random
 *   ----------------------------
 *
 *   Description: Generates filler octets using XORShift PRNG
 *   ------------
 *
 *   Parameters:  Input seed state array, output buffer and octet count
 *   -----------
 *
 *   Returns:     Void
 *   --------
 */
static void l2_xor_get_random(unsigned long *sp, byte *spare_octets, uint16 num_octets)
{
  unsigned long tmp;
  size_t        byte_pos;
  uint16        i;
  byte         *random_fill_ptr;

  /* initialise a pointer to the seed array */
  /* the first unsigned long integer in the */
  /* array is treated as a new random value */
  random_fill_ptr = (byte *)sp;
  byte_pos        = 0;

  for (i = 0; i < num_octets; i++)
  {
    if (byte_pos == 0)
    {
      /* generate a new value */
      tmp   = (sp[1]^(sp[1] << XOR_PARAM_A));
      sp[1] = sp[2]; sp[2] = sp[3]; sp[3] = sp[0]; 
      sp[0] = (sp[0]^(sp[0] >> XOR_PARAM_B))^(tmp^(tmp >> XOR_PARAM_C));
    }

    /* extract the next byte from the random value */
    spare_octets[i] = random_fill_ptr[byte_pos];
    byte_pos++;
    if (byte_pos >= sizeof(unsigned long))
    {
      /* reset the byte position */
      byte_pos = 0;
    }
  }

  return;
}

/*
 *   Function name:  l2_generate_filler
 *   ----------------------------
 *
 *   Description: Pads a Layer 2 frame buffer with appropriate filler octets
 *   ------------
 *
 *   Parameters:  Pointer to first spare position, number of octets and GAS ID
 *   -----------
 *
 *   Returns:     Void
 *   --------
 */
void l2_generate_filler(byte *spare_octets, uint16 num_octets, const gas_id_t gas_id)
{
  l2_store_T *l2_data_ptr;

  /* establish pointer to the correct data space */
  l2_data_ptr = l2_get_store(gas_id);

  if ((spare_octets == NULL) ||
      (l2_data_ptr  == NULL))
  {
    return;
  }

  if (l2_data_ptr->random_pad)
  {
#ifndef FEATURE_L2_SOFTWARE_PRNG
    /* if the Crypto API returns any error code, fall back to local RNG */
    /* which allows graceful recovery in the event of buffer exhaustion */
    if (E_SUCCESS != secapi_get_random(SECAPI_TIME_CRITICAL_RANDOM, spare_octets, num_octets))
#endif /* !FEATURE_L2_SOFTWARE_PRNG */
    {
      l2_xor_get_random(l2_data_ptr->xor_seed, spare_octets, num_octets);
    }
  }
  else
  {
    memset((void *)spare_octets, 0x2B, (size_t)num_octets);
  }

  return;
}

/*
 *   Function name:  l2_seed_random
 *   ----------------------------
 *
 *   Description: Seeds the PRNG
 *   ------------
 *
 *   Parameters:  Global data store pointer
 *   -----------
 *
 *   Returns:     None
 *   --------
 */
void l2_seed_random(l2_store_T *l2_data_ptr)
{
  if (l2_data_ptr != NULL)
  {
#ifdef FEATURE_DUAL_SIM
    gas_id_t gas_id = l2_data_ptr->gas_id;
#endif /* FEATURE_DUAL_SIM */

    if (l2_data_ptr->random_pad)
    {
#ifndef FEATURE_L2_SOFTWARE_PRNG
      MSG_GERAN_HIGH_0_G("Feature set: random padding using hardware");
#else
      MSG_GERAN_HIGH_0_G("Feature set: random padding using software");
#endif /* !FEATURE_L2_SOFTWARE_PRNG */
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Random padding is currently disabled");
    }
  }

  return;
}

/*
 *   Function name:  l2_code_sabm
 *   ----------------------------
 *
 *   Description: Code the SABM frame
 *   ------------
 *
 *   Parameters:  Pointer to destination of SABM frame
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_code_sabm(sapi_info_T *sapi_info, channel_info_T *channel_info, boolean retransmission, byte *l3_message_to_send, byte l3_message_length)
{
  byte *sabm_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  sabm_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/

  sabm_frame[0] = 0x01 | (byte) (sapi_info->sapi<< 2);

  /**************************/
  /* Code the Control Field */
  /**************************/

  sabm_frame[1] = 0x2F | 0x10;   /* Set the P bit to 1 */

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  sabm_frame[2] = 0x01;

  if ((l3_message_to_send != NULL) &&
      (l3_message_length  != 0)    &&
      (l3_message_length  <= N201_MAX))
  {
    sabm_frame[2] |= (byte)(l3_message_length << 2);

    (void)memscpy(sabm_frame + L2_HEADER_LENGTH,
                  sizeof(channel_info->transmit_buffer.layer2_message) - L2_HEADER_LENGTH,
                  l3_message_to_send, l3_message_length);

    /* pad to the end of the message */
    if (channel_info->N201 > l3_message_length)
    {
      L2_PAD_FRAME(sabm_frame + L2_HEADER_LENGTH + l3_message_length,
                   channel_info->N201 - l3_message_length, sapi_info->gas_id);
    }
  }
  else
  {
    L2_PAD_FRAME(sabm_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);
  }

  channel_info->transmit_buffer.frame_type      = SABM;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission  = retransmission;

  return;
}

/*
 *   Function name:  l2_code_ua
 *   --------------------------
 *
 *   Description: Code the UA frame
 *   ------------
 *
 *   Parameters:  PF bit to put in message
 *   -----------  pointer to destination of UA frame
 *
 *   Returns:     none
 *   --------
 */

void l2_code_ua(byte PF, sapi_info_T* sapi_info, channel_info_T* channel_info)
{
  byte *ua_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  ua_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/

  ua_frame[0] = 0x03  /* This is a response frame */
                | (byte) (sapi_info->sapi << 2);

  /**************************/
  /* Code the Control Field */
  /**************************/

  ua_frame[1] = 0x63;

  if (PF == 1)
  {
    ua_frame[1] |= 0x10;
  }

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  ua_frame[2] = 0x01;  /* EL = 1; L = 0 */

  L2_PAD_FRAME(ua_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);

  channel_info->transmit_buffer.frame_type = UA;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = FALSE;
}

/*
 *   Function name:  l2_code_dm
 *   --------------------------
 *
 *   Description: Code the DM frame
 *   ------------
 *
 *   Parameters:  PF bit to use
 *   -----------  pointer to destination of DM frame
 *
 *   Returns:     none
 *   --------
 */

void l2_code_dm(byte PF, sapi_info_T* sapi_info, channel_info_T* channel_info)
{
  byte *dm_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  dm_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/

  dm_frame[0] = 0x03  /* This is a response frame */
                | (byte) (sapi_info->sapi << 2);

  /**************************/
  /* Code the Control Field */
  /**************************/

  dm_frame[1] = 0x0F;

  if (PF == 1)
  {
    dm_frame[1] |= 0x10;
  }

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  dm_frame[2] = 0x01;  /* EL = 1; L = 0 */

  L2_PAD_FRAME(dm_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);

  channel_info->transmit_buffer.frame_type = DM;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = FALSE;
}

/*
 *   Function name:  l2_code_disc
 *   ----------------------------
 *
 *   Description: Code the DISC frame
 *   ------------
 *
 *   Parameters:  pointer to destination of disc frame
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_code_disc(sapi_info_T* sapi_info, channel_info_T* channel_info, boolean retransmission)
{
  byte *disc_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  disc_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/
  disc_frame[0] = 0x01 | (byte) (sapi_info->sapi << 2);

  /**************************/
  /* Code the Control Field */
  /**************************/

  disc_frame[1] = 0x43 | 0x10; /* Set the P bit to 1 */

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  disc_frame[2] = 0x01;  /* EL = 1; L = 0 */

  L2_PAD_FRAME(disc_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);

  channel_info->transmit_buffer.frame_type = DISC;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = retransmission;
}


/*
 *   Function name:  l2_code_ui
 *   --------------------------
 *
 *   Description: Code the UI frame
 *   ------------
 *
 *   Parameters:  channel_info, short_header, L3 messaage and length
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_code_ui(sapi_info_T *sapi_info, channel_info_T *channel_info, boolean short_header, byte *l3_message_to_send, byte l3_message_length)
{
  byte *ui_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  ui_frame = channel_info->transmit_buffer.layer2_message;

  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = FALSE;

  if (short_header)
  {
    channel_info->transmit_buffer.frame_type = UI_SHORT_HEADER;

    /* RR should automatically insert the header so we don't have to */
    (void)memscpy(ui_frame, sizeof(channel_info->transmit_buffer.layer2_message),
                  l3_message_to_send, l3_message_length);

    return;
  }
  else
  {
    channel_info->transmit_buffer.frame_type = UI;
  }

  /* Not a short header need to insert header */

  /**************************/
  /* Code the Address Field */
  /* The SAPI is SAPI0      */
  /**************************/

  ui_frame[0] = 0x01;

  /**************************/
  /* Code the Control Field */
  /**************************/

  ui_frame[1] = 0x03;

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  ui_frame[2] = 0x01;

  if (l3_message_length != 0)
  {
    ui_frame[2] |= (byte) (l3_message_length << 2);

    (void)memscpy(ui_frame + L2_HEADER_LENGTH,
                  sizeof(channel_info->transmit_buffer.layer2_message) - L2_HEADER_LENGTH,
                  l3_message_to_send, l3_message_length);

    /* pad to the end of the message */
    if (channel_info->N201 > l3_message_length)
    {
      L2_PAD_FRAME(ui_frame + L2_HEADER_LENGTH + l3_message_length,
                   channel_info->N201 - l3_message_length, sapi_info->gas_id);
    }
  }
  else
  {
    L2_PAD_FRAME(ui_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);
  }

  channel_info->transmit_buffer.frame_type      = UI;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission  = FALSE;

  return;
}


/*
 *   Function name:  l2_code_i
 *   -------------------------
 *
 *   Description: Code the I frame
 *   ------------
 *
 *   Parameters:  PF bit value to use,
 *   -----------  pointer to destination of I frame
 *                previouse VS used
 *
 *   Returns:     none
 *   --------
 */

void l2_code_i(byte PF, boolean previous_vs, sapi_info_T* sapi_info, channel_info_T* channel_info, boolean retransmission)
{
  byte *i_frame;
  byte *information_field;
  byte  information_length;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  i_frame = channel_info->transmit_buffer.layer2_message;

  if (sapi_info->sapi == SAPI0)
  {
    MSG_GERAN_MED_3("Code i frame: SAPI0 VS:%d VR:%d prev:%d",sapi_info->VS,sapi_info->VR,previous_vs);
    MSG_GERAN_MED_1("Code i frame: SAPI0 PF:%d",PF);
  }
  else
  {
    MSG_GERAN_MED_3("Code i frame: SAPI3 VS:%d VR:%d prev:%d",sapi_info->VS,sapi_info->VR,previous_vs);
    MSG_GERAN_MED_1("Code i frame: SAPI3 PF:%d",PF);
  }

  /**************************/
  /* Code the Address Field */
  /**************************/

  i_frame[0] = 0x01 | (byte) (sapi_info->sapi << 2);

  /**************************/
  /* Code the Control Field */
  /* The P bit is set to PF */
  /**************************/

  if (previous_vs == FALSE)
  {
    i_frame[1] = (byte) (sapi_info->VR << 5) + (byte) (sapi_info->VS << 1);
  }
  else
  {
    i_frame[1] = (byte) (sapi_info->VR << 5);

    if (sapi_info->VS != 0)
    {
      i_frame[1] |= (byte) ( (byte) (sapi_info->VS - 1) << 1);
    }
    else
    {
      i_frame[1] |= 0x0E;
    }
  }


  if (PF == 1)
  {
    i_frame[1] |= 0x10;
  }

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  i_frame[2] = 0x01;

  information_length = channel_info->send_buffer.next_segment->segment_length;

  information_field = channel_info->send_buffer.next_segment->segment;
  if (channel_info->send_buffer.next_segment->next != NULL)
  {
    i_frame[2] |= 0x02;
  }

  /* sanity check the length */
  if (information_length <= N201_MAX)
  {
    i_frame[2] |= (byte)(information_length << 2);

    (void)memscpy(i_frame + L2_HEADER_LENGTH,
                  sizeof(channel_info->transmit_buffer.layer2_message) - L2_HEADER_LENGTH,
                  information_field, information_length);

    if (channel_info->N201 > information_length)
    {
      L2_PAD_FRAME(i_frame + L2_HEADER_LENGTH + information_length,
                   channel_info->N201 - information_length, sapi_info->gas_id);
    }

    channel_info->transmit_buffer.frame_type = I;
    channel_info->transmit_buffer.valid_data_flag = TRUE;
    channel_info->transmit_buffer.retransmission = retransmission;
  }

  return;
}


/*
 *   Function name:  l2_code_rr
 *   --------------------------
 *
 *   Description: Code the RR frame
 *   ------------
 *
 *   Parameters:  CR value to use
 *   -----------  PF value to use
 *                pointer to destination of RR frame
 *
 *   Returns:     none
 *   --------
 */

void l2_code_rr(byte CR, byte PF, sapi_info_T* sapi_info, channel_info_T* channel_info)
{
  byte *rr_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  rr_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/
  rr_frame[0] = 0x01 | (byte) (sapi_info->sapi << 2);

  if (CR == 1)
  {
    rr_frame[0] |= 0x02;
  }


  /**************************/
  /* Code the Control Field */
  /**************************/

  rr_frame[1] = 0x01;

  if (PF == 1)
  {
    rr_frame[1] |= 0x10;   /* Set the P bit to 1 */
  }

  rr_frame[1] |= (byte) (sapi_info->VR << 5);

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  rr_frame[2] = 0x01;

  L2_PAD_FRAME(rr_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);

  channel_info->transmit_buffer.frame_type = RR;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = FALSE;
}


/*
 *   Function name:  l2_code_rej
 *   ---------------------------
 *
 *   Description: Code the REJ frame
 *   ------------
 *
 *   Parameters:  CR bit to use,
 *   -----------  PF bit value to use
 *                pointer to destination of REJ frame
 *
 *   Returns:     none
 *   --------
 */

void l2_code_rej(byte CR, byte PF, sapi_info_T* sapi_info, channel_info_T* channel_info)
{
  byte *rej_frame;

  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* coding starts in the channel's transmit buffer */
  rej_frame = channel_info->transmit_buffer.layer2_message;

  /**************************/
  /* Code the Address Field */
  /**************************/
  rej_frame[0] = 0x01 | (byte) (sapi_info->sapi << 2);

  if (CR == 1)
  {
    rej_frame[0] |= 0x02;
  }


  /**************************/
  /* Code the Control Field */
  /**************************/

  rej_frame[1] = 0x09;

  if (PF == 1)
  {
    rej_frame[1] |= 0x10;   /* Set the P bit to 1 */
  }

  rej_frame[1] |= (byte) (sapi_info->VR << 5);

  /***********************************/
  /* Code the Length Indicator Field */
  /* and the Information Field       */
  /***********************************/

  rej_frame[2] = 0x01;

  L2_PAD_FRAME(rej_frame + L2_HEADER_LENGTH, channel_info->N201, sapi_info->gas_id);

  channel_info->transmit_buffer.frame_type = REJ;
  channel_info->transmit_buffer.valid_data_flag = TRUE;
  channel_info->transmit_buffer.retransmission = FALSE;
}
