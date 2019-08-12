/*===========================================================================
                  R R C  P A G I N G  T Y P E 2

DESCRIPTION
  This file contains the implementation for the Paging type 2 procedure.

EXTERNALIZED FUNCTIONS
  rrcpg2_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Paging Type 2 procedure.
  
INTERNAL FUNCTIONS
  rrcpg2_update_rrc_page_ind  
    This function updates the parameters that are required for RRC_PAGE_IND
    command.
  
  rrcpg2_process_paging_type1_msg  
    This function checks whether RRC_DOWNLINK_SDU_IND contains Paging Type 2
    message.If it contains Paging Type 2 mesage,it will processes the same and
    send RRC_PAGE_IND to the MM. 
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2002-2005, 2007-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcpg2.c_v   1.1   07 Mar 2002 17:03:46   kiranc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcpg2.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/15   vi      Made changes to ignore page when page is received while recovering from RLF
09/03/14   sp      Made changes to save procedure substates in crash debug info
07/21/14   sn      f3 reduction changes
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
12/20/05   bd      Changed syntax for suppressing lint error 613
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
08/30/04   sgk     Replaced RRC_CS_DOMAIN_CN_ID and RRC_PS_DOMAIN_CN_ID
                   by rrc_cs_domain and rrc_ps_domain in function 
                   rrcpg2_validate_paging_type_msg to fix lint error 'bad type'.
                   Replaced 3rd param of the function 
                   rrcsend_send_rrc_status_message, rrc_pagingType2_chosen 
                   (value = 10) by required asn1 value rrc_pagingType2 
                   (value = 7) in function rrcpg2_process_paging_type2_msg
                   to fix lint error Type mismatch (arg. no. 3) (int/enum).   
                   Removed RRCPG2_FAILURE_MAJOR enum from rrcpg2_status_e_type  
                   to fix lint error 'not referenced'
                   Type cast RRCLOG_PROTOCOL_ERROR RRC_PROCEDURE_PG2 to uint8
                   in function rrcpg2_process_paging_type2_msg to fix lint error 
                   Converting enum to int.
07/23/04   kc      Fixes for March 04 ASN1 notation
05/21/03   rj      Fixed received message type in the RRC STATUS if invalid 
                   Paging Type2 is received.
04/30/03   vk      Support for EVENT_WCDMA_PAGE_RECEIVED.
02/25/03   kc      Removed incorrect check for nonCriticalExtensions.  Now we
                   do not return failure if nonCriticalExtensions are present.
                   Fix for CR28006
02/14/02   rj      Fixed a typo from the previous check-in.
12/17/02   rj      Added code for error handling. Also added code to select
                   correct pagingRecordTypeID from the received CN Domain Id.
10/25/02   rj      Added code to process unknown paging cause.
10/24/02   rj      Deleted tmc.h from include files.
03/07/02   kc      Added code to log asn1 protocol err if it occurs in the msg.
02/07/02   kc      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "rrcdata_v.h"
#include "rrclcm.h"
#include "rrcsend.h"
#include "comdef.h"

#include "rrccmd_v.h"               /* RRC command definitions */
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcscmgr.h"
#include "rrcpg2.h"
#ifdef FEATURE_QSH_EVENT_METRIC
#include "rrcpg1.h"
#endif
#include "rrclogging.h"
#include "rrccspi.h"
#ifdef T_WINNT  /* Include files when logging is enabled.*/
#error code not present
#endif /* T_WINNT */
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



typedef enum
{
  RRCPG2_SUCCESS,                            /* Successful Status */      
  RRCPG2_FAILURE_MINOR                      /* Minor Failure Status */
} rrcpg2_status_e_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

rrc_cn_domain_identity_e_type   rrcpg2_cn_domain_id;
rrc_paging_cause_e_type         rrcpg2_paging_cause;
/* The UE ID used for paging */
rrc_page_ue_id_choice_e_type    rrcpg2_page_ue_id;

/* Transaction Id for Paging Type 2 Msg */
rrc_RRC_TransactionIdentifier   rrcpg2_transaction_id;
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION rrcpg2_update_rrc_page_ind  

DESCRIPTION
  This function updates the parameters that are required for RRC_PAGE_IND
  command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcpg2_update_rrc_page_ind
(
  rrc_DL_DCCH_Message *dcch_msg_ptr        /* Pointer to DL_DCCH message*/
)
{
  
  rrc_event_paging_info_type paging_event_info;
  
  switch(dcch_msg_ptr->message.u.pagingType2.cn_DomainIdentity)
  { 
    case rrc_CN_DomainIdentity_cs_domain:
      rrcpg2_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
      break;

    case rrc_CN_DomainIdentity_ps_domain:
      rrcpg2_cn_domain_id = RRC_PS_DOMAIN_CN_ID;
      break;

    default:
      WRRC_MSG1_ERROR("Unknown CN Domian Id: %d is received", dcch_msg_ptr->message.u.
          pagingType2.cn_DomainIdentity);
      break;
  }

  switch(dcch_msg_ptr->message.u.pagingType2.pagingCause)
  {
    case rrc_PagingCause_terminatingConversationalCall:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_TERM_CONV_CALL;
      break;

    case rrc_PagingCause_terminatingStreamingCall:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_TERM_STREAM_CALL;
      break;

    case rrc_PagingCause_terminatingInteractiveCall:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_TERM_INTERACT_CALL;
      break;

    case rrc_PagingCause_terminatingBackgroundCall:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_TERM_BACKGND_CALL;
      break;

    case rrc_PagingCause_terminatingHighPrioritySignalling:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING;
      break;

    case rrc_PagingCause_terminatingLowPrioritySignalling:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING;
      break;

    case rrc_PagingCause_terminatingCauseUnknown:
      rrcpg2_paging_cause = RRC_PAGE_CAUSE_UNKNOWN;
      break;

    default:
      WRRC_MSG1_ERROR("Unknown Paging Cause: %d", dcch_msg_ptr->message.u.
          pagingType2.pagingCause);
      break;
  }

  switch( dcch_msg_ptr->message.u.pagingType2.pagingRecordTypeID)
  {
    case rrc_PagingRecordTypeID_imsi_GSM_MAP:
      rrcpg2_page_ue_id = RRC_PAGE_WITH_IMSI;
      break;

    case rrc_PagingRecordTypeID_tmsi_GSM_MAP_P_TMSI:
      if(rrcpg2_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
      {
        rrcpg2_page_ue_id = RRC_PAGE_WITH_TMSI;
      }
      else if(rrcpg2_cn_domain_id == RRC_PS_DOMAIN_CN_ID)
      {
        rrcpg2_page_ue_id = RRC_PAGE_WITH_PTMSI;
      }
      break;

    case rrc_PagingRecordTypeID_imsi_DS_41:
    case rrc_PagingRecordTypeID_tmsi_DS_41:
    default:
      WRRC_MSG1_ERROR("Unsupported UE Id %d is received",
           dcch_msg_ptr->message.u.pagingType2.pagingRecordTypeID);
    break;
  }

  /* Fill the Paging type */
  paging_event_info.paging_type = RRCLOG_PAGING_TYPE_2;
  /* Populate the paging cause field with the appropriate cause*/
  paging_event_info.paging_cause = rrcpg2_paging_cause;
  /* Send the event */

  event_report_payload(EVENT_WCDMA_PAGE_RECEIVED, /* Paging event ID */
                       sizeof(rrc_event_paging_info_type), /* payload size */
                       &paging_event_info); /* paging cause and type */
  
  WRRC_MSG2_HIGH("Page2 type %d, cause %d ",paging_event_info.paging_type,paging_event_info.paging_cause);

} /* rrcpg2_update_rrc_page_ind */
/*===========================================================================

FUNCTION rrcpg2_validate_paging_type_msg  

DESCRIPTION
  This function looks into all Paging Records of the Paging Type 2 message
  until if finds a match for either IMSI, TMSI or PTMSI.

DEPENDENCIES
  None.

RETURN VALUE
  rrcpg2_status_e_type : Returns RRCPG2_SUCCESS if it finds match. Otherwise
  it returns RRCPG2_FAILURE_MINOR.

===========================================================================*/

static rrcpg2_status_e_type rrcpg2_validate_paging_type_msg
(
  rrc_DL_DCCH_Message *dcch_msg_ptr        /* Pointer to DCCH message*/
)
{
  rrcpg2_status_e_type status = RRCPG2_FAILURE_MINOR;

  if(dcch_msg_ptr == NULL)
    return (status);
	
  /* Check whether externsion chosen */
  if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.pagingType2,
      rrc_PagingType2,laterNonCriticalExtensions))
  {
    MSG_LOW("Non Critical Extensions not supported",0,0,0);
  }
  if( (dcch_msg_ptr->message.u.pagingType2.cn_DomainIdentity !=
      rrc_CN_DomainIdentity_cs_domain) &&
      (dcch_msg_ptr->message.u.pagingType2.cn_DomainIdentity !=
      rrc_CN_DomainIdentity_ps_domain) )
  {
    return (status);
  }
  if(dcch_msg_ptr->message.u.pagingType2.pagingCause >
    rrc_PagingCause_terminatingCauseUnknown )
  {
    return (status);
  }
  if( (dcch_msg_ptr->message.u.pagingType2.pagingRecordTypeID != 
      rrc_PagingRecordTypeID_imsi_GSM_MAP) &&
      (dcch_msg_ptr->message.u.pagingType2.pagingRecordTypeID != 
      rrc_PagingRecordTypeID_tmsi_GSM_MAP_P_TMSI) )
  {
    return(status);
  }
  return(RRCPG2_SUCCESS);

} /*rrcpg2_validate_paging_type_msg */
/*===========================================================================

FUNCTION rrcpg2_process_paging_type2_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND contains Paging Type 2
  message.If it contains Paging Type 2 mesage,it will processes the same and
  send RRC_PAGE_IND to the MM. 

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcpg2_process_paging_type2_msg
(
  rrc_cmd_type *cmd_ptr                    /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;  /* Pointer to DCCH message*/

  mm_cmd_type *mm_cmd_ptr;                 /* Pointer to the MM Command */

  uint8 message_choice;                    /* Local varibale for message choice*/  

  rlc_lc_id_type lc_id;                    /* Local variable to store RLC LC Id */

  rrc_state_e_type rrc_state;              /* local variable to store rrc_state */
   
#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_pg_ota page_info;
  memset(&page_info,0,sizeof(rrc_log_pg_ota));
#endif
  
  /* Assert received message is Downlink DCCH message */
  if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU) 
  {
    message_choice = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                   cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

    /*check if decoding was successful otherwise set error flag to true*/
    rrc_protocol_error_reject = FALSE;

    if (cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
    {
      dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
      cmd_ptr->cmd.downlink_sdu_ind.decoded_msg; 
    }
    else
    { /* ASN1 decoding failed */
      rrc_protocol_error_reject = TRUE;
    } 
    
    /* Assert the received message is Paging Type 2 */
    if( message_choice == T_rrc_DL_DCCH_MessageType_pagingType2 )
    {
      WRRC_MSG0_HIGH("Paging Type2 Message is received");

      if(rrc_protocol_error_reject == TRUE)
      {
        rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                (uint8) RRC_PROCEDURE_PG2,
                                (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                              );
         
        /*if we are in cell_pch or ura_pch state then do not send status
         *message, as these cases we cannot send the status on ul dcch.
         *we are handling this case only in case of race conditions
         *as a precaution
         */
        rrc_state = rrc_get_state();

        if( ( rrc_state == RRC_STATE_CELL_PCH ) ||
            ( rrc_state == RRC_STATE_URA_PCH  )  )
        {
          return;
        }

        lc_id = rrclcm_check_ul_rlc_lc_id(    
                UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                            the above Downlink Logical 
                                            Channel type. */
                UE_MODE_ACKNOWLEDGED_DATA);
                                          /* Mode of transmission for the RLC 
                                             service entity. */
        if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
        {
          WRRC_MSG0_HIGH("Cann't find the RLC Id for RB 2");
        }       
        else /* Valid RLC Id */
        {
          if(SUCCESS != rrcsend_send_rrc_status_message(
             T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                           // The protocol error cause to be placed in
                                           // the message
             0,                            // Set transaction id to 0 since ASN1 
                                           // decoding failed 
             rrc_ReceivedMessageType_pagingType2, 
                                           // Received message identification
             RRC_PROCEDURE_PG2,            // RRC procedure
             lc_id,                        /* Logical Channel Id */
             RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                     /* Indicates whether L2 Ack required or not */
          {
            WRRC_MSG0_ERROR("Failed to xmit RRC STATUS msg");
          } 
        } /* Valid RLC Id */
      }
      else
      {
        /*lint -save -e613 */
        /* Update the Transaction Id */
        rrcpg2_transaction_id = 
          dcch_msg_ptr->message.u.pagingType2.rrc_TransactionIdentifier; 

        /* ASN1 decoding is successful.  Validate the Paging Type 2 message */
        if(RRCPG2_SUCCESS == rrcpg2_validate_paging_type_msg(dcch_msg_ptr) )
        {
          /* Allocate memory for RRC_PAGE_IND command */
          if( (mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_PAGE_IND)) != NULL)
          {
            /*update the global variables that will populate the mm cmd buffer*/
            rrcpg2_update_rrc_page_ind(dcch_msg_ptr);

            /* Fill in parameters fro RRC_PAGE_IND command */

            mm_cmd_ptr->cmd.rrc_page_ind.cn_domain_id = rrcpg2_cn_domain_id;

            mm_cmd_ptr->cmd.rrc_page_ind.paging_cause = rrcpg2_paging_cause;

            mm_cmd_ptr->cmd.rrc_page_ind.page_ue_id = rrcpg2_page_ue_id;

#ifdef FEATURE_DUAL_SIM
            mm_cmd_ptr->cmd.rrc_page_ind.as_id = rrc_get_as_id();
#endif

            WRRC_MSG0_HIGH(" Dispatching out MM cmd / Sent RRC_PAGE_IND to the MM");
            rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */
          }
          else
          {
            ERR_FATAL(" Failed allocated memory for MM Command: %d",
                       RRC_PAGE_IND,0,0);
          }
        }/*if for validate function else*/
        else
        {
          lc_id = rrclcm_check_ul_rlc_lc_id(    
                UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                            the above Downlink Logical 
                                            Channel type. */
                UE_MODE_ACKNOWLEDGED_DATA);
                                          /* Mode of transmission for the RLC 
                                             service entity. */
          if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
          {
            WRRC_MSG0_HIGH("Cann't find the RLC Id for RB 2");
          }
          else
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended,
                                             // The protocol error cause to be placed in
                                             // the message
               rrcpg2_transaction_id,        // Set transaction id 
               rrc_ReceivedMessageType_pagingType2, 
                                             // Received message identification
               RRC_PROCEDURE_PG2,            // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                       /* Indicates whether L2 Ack required or not */
            {
              WRRC_MSG0_ERROR("Failed to xmit RRC STATUS msg");
            } 
          }
        }
        /*lint -restore */
      }/*else for asn1 decoding success*/
    }/*if for message type check*/
  }/*if for sdu type check*/
  else
  {
    WRRC_MSG2_HIGH("Invalid Event: %d or SDU type: %d",
                cmd_ptr->cmd_hdr.cmd_id,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
  }

#ifdef FEATURE_QSH_EVENT_METRIC
  page_info.sys_info_mod = 0;
  page_info.page_record_match = rrcpg2_validate_paging_type_msg(dcch_msg_ptr);
  page_info.cn_id = rrcpg2_cn_domain_id;
  page_info.paging_cause = rrcpg2_paging_cause; 
  rrc_qsh_log_paging_params(RRC_PROCEDURE_PG2,&page_info);
#endif  
} /* rrcpg2_process_paging_type2_msg */
/*===========================================================================

FUNCTION rrcpg2_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Paging Type 2 procedure.

  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (with PAGING TYPE 1 Message)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg2_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();
  
  WRRC_MSG1_HIGH("function rrcpg2_procedure_event_handler is called in rrc_state = rrc_state_e_type_value %d",rrc_state);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_PG2,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
    case RRC_STATE_CONNECTING:
      break;
 
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      /* These two states are invalid to receive Paging Type 2 message. 
       * We are handling the cases to take care of race conditions when
       * we get this message as we move from fach/dch to cell_pch/ura_pch state */
        rrcpg2_process_paging_type2_msg(cmd_ptr); 
        break;

    case RRC_STATE_CELL_FACH:    
    case RRC_STATE_CELL_DCH:  
        /* These two states are valid to receive Paging Type 2 message. */
#ifdef FEATURE_WRLF_SYSTEM_SEL
        /* Ignore Paging Type 2 message if UE is in conn mode oos*/
        if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
        {
           WRRC_MSG0_HIGH("UE in conn mode oos, Ignoring Page");
           return;
        }
#endif
        rrcpg2_process_paging_type2_msg(cmd_ptr); 
      break;

    default:
      break;
   }  
} /* rrcpg2_procedure_event_handler */
/*===========================================================================

FUNCTION rrcpg2_init_procedure

DESCRIPTION
  This function initializes everything that the Paging Type 2 procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg2_init_procedure(void)
{
  rrcpg2_transaction_id = 0;
  
} /* rrcpg2_init_procedure */
