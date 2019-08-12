/*========================================================================================

                                    R R C C H K

DESCRIPTION
  This file contains the definitions of the functions and data types used in RRCCHK procedure

  Copyright (c) 2005, 2007-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcchk.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------    ---     ----------------------------------------------------------
09/03/14   sp      Made changes to save procedure substates in crash debug info
07/18/14   ad      Made changes to reduce F3s
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/03/13   ad      Made changes to reduce the redundant F3s
01/28/11    rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11    ad      Added changes for RRC SW decoupling
12/31/10    rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
05/08/09    ss      Updated Copyright Information
04/29/09    ss      Made changes to compiler warnings on gcc compilers.
03/06/07    vr      Removed NV GCF flag dependency for sending Counter Check Response
10/17/05    vr      Created the file to support Counter Check Procedure 

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
#include "rrcasn1.h"
#include "rrcscmgr.h"
#include "rrcdata_v.h"
#include "rrclcm.h"
#include "rrcsend.h"
#include "rrcchk.h"


rrc_RRC_TransactionIdentifier chk_transaction_id; /* Variable to hold the transaction id */

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/*===========================================================================
**                  TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                    FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrcchk_process_counter_check_message

DESCRIPTION
  This function validates the Counter Check message

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If the message is valid
  FALSE - If the message is invalid
  
SIDE EFFECTS
  None

===========================================================================*/
static boolean rrcchk_process_counter_check_message
(
   rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;  /* Pointer to DCCH message */  
  uint8 message_choice;                    /* Local varibale for message choice */
  boolean ret_val = TRUE;                  /* Return Value */

  /* Check if the decoding was successful */
  if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
  {
    /* Check the message type */
    message_choice = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
    if(message_choice == T_rrc_DL_DCCH_MessageType_counterCheck)
    {
      WRRC_MSG0_HIGH("Counter Check Message is rcvd");

      /* Extract the transaction id from the message */
      dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;     
      chk_transaction_id = 
      dcch_msg_ptr->message.u.counterCheck.u.r3.counterCheck_r3.rrc_TransactionIdentifier;
    }
    else
    {
      WRRC_MSG1_ERROR("Invalid message %d sent to CHK procedure",message_choice);
      ret_val = FALSE;
    }
  }
  else
  {
    WRRC_MSG0_ERROR("ASN1 Decoding error");
    ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION rrcchk_send_counter_check_response

DESCRIPTION
  This function sends the Counter Check Response message to the network

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
static void rrcchk_send_counter_check_response
(
  void
)
{
  rlc_lc_id_type lc_id;                    /* Local variable to store RLC logical channel Id */
  rrc_UL_DCCH_Message *msg = NULL;         /* Pointer to UL message */

  lc_id = rrclcm_check_ul_rlc_lc_id(
             UE_LOGCHAN_DCCH,              /* Uplink Logical Channel Type. */
             DCCH_AM_RADIO_BEARER_ID,      /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
             UE_MODE_ACKNOWLEDGED_DATA);   /* Mode of transmission for the RLC 
                                              service entity. */
  if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
  {
    ERR_FATAL("Can't find RLC ID for UL RB 2",0,0,0);
  }

  /* Allocate a buffer for an uplink DCCH message. This buffer is released
     by rrcsend_send_ul_sdu().*/
  msg = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof (struct rrc_UL_DCCH_Message));

  /* Set the message type to Counter Check Response */
  RRCSET_MSG_TYPE(msg->message, 
            T_rrc_UL_DCCH_MessageType_counterCheckResponse);

  /* Set the bit mask not to include non-critical extensions */
  RRC_RESET_MSG_IE_PRESENT(msg->message.u.counterCheckResponse);

  /* Set the transaction id */
  msg->message.u.counterCheckResponse.rrc_TransactionIdentifier = chk_transaction_id;

  /* Send the signalling message */
  if(RRCSEND_SUCCESS == rrcsend_send_ul_sdu(
                                            RRC_PROCEDURE_CHK,
                                            msg,
                                            lc_id,
                                            RRCSEND_L2ACK_NOT_REQUIRED
                                           ) )
  {
    WRRC_MSG0_HIGH("Counter Check Response is sent");
  } 
  else
  {
    ERR_FATAL("Failed to send Counter Check Response",0,0,0);
  }
}

/*===========================================================================

FUNCTION rrcchk_procedure_event_handler

DESCRIPTION
  This function is the entry point of rrcchk

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcchk_procedure_event_handler
( 
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)  
{ 
  WRRC_MSG0_HIGH_OPT("function rrcchk_procedure_event_handler is called");
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_CHK,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);
  
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND: 
      if (TRUE == rrcchk_process_counter_check_message(cmd_ptr)) 
      {
        rrcchk_send_counter_check_response();
      }
      break;

    case RRC_STATE_CHANGE_IND:
      MSG_MED("RRC_STATE_CHANGE_IND received",0,0,0);
      break;

    default:     
      WRRC_MSG1_HIGH("Invalid Event: %d received", cmd_ptr->cmd_hdr.cmd_id);
      break;
  }
}

/*===========================================================================

FUNCTION RRCCHK_INIT_PROCEDURE

DESCRIPTION
  This function will register for the necessary state change indication for
  Counter Check procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcchk_init_procedure
(    
  void  
)
{
  rrcscmgr_register_for_scn(RRC_PROCEDURE_CHK,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);
}
