/*===========================================================================
                  R R C  U E  C A P A B I L I T Y  E N Q U I R Y

DESCRIPTION
  This header file contains function prototypes for UE Capability Enquiry
  procedure

EXTERNALIZED FUNCTIONS
  rrcuece_procedure_event_handler
    This function is the event handler for all events that are sent to the
    UE Capability Enquiry procedure.

  rrcuece_init_procedure
    This function initializes required parameters for UE Capability Enquiry
    procedure.



INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcuece.c_v   1.8   28 Nov 2001 16:15:48   kiranc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcuece.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/14   sp      Made changes to save procedure substates in crash debug info
09/18/13   vi      Made changes to handle systemSpecificCapUpdateReqList when LTE is not enabled
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
01/03/13   ad      Made changes to reduce the redundant F3s
05/03/11   rl      Moved the macro RRC_MAX_SYS_CAPABILITY to header file.
04/29/11   sks     Added code to send EUTRA capabilities when requested by n/w in 
                   UE capability enquiry message.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/18/10   gkg     Fixed Compilation warnings.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
11/23/04   sm      Support for Dynamic Power Class feature
08/30/04   sgk     Typecast all params of function rrc_log_protocol_error to
                   uint8 in function rrcuece_process_ue_capability_enquiry_msg
                   to fix lint error Converting enum to int.
07/03/03   kc      Set transaction id in RRC STATUS msg. correctly when Critical
                   extensions are received.
05/21/03   rj      Fixed a bug while validating UE Capability Enquiry message.
03/18/03   rj      Added code to perform validation checks on UE Capability
                   Enquiry messagae and send suitable RRC status message if
                   validation fails.
02/03/03   kc      Removed lint errors
08/07/02   rj      Deleted Check for System Specific Capability Update Request
                   in the UE Capability Enquiry message.
08/06/02   rj      Added code to honor System Specific UE Capability request
                   from the UTRAN for Dual mode operation.
11/28/01   kc      Code changes for Dec->June Spec conversion.
10/23/01   kc      Initialized uninitialized vars to remove compile time warns.
09/19/10   rj      Modified F3 message text to maximum length of 40 charecters.
09/14/01   rj      Modifeid function rrcuece_process_ue_capability_enquiry_msg
                   not to continue if transaction id is invalid. Cleaned some
                   F3 messages and added F3 message to declare critical
                   extensions are not supported.
09/14/01   kc      Added code to handle protocol error logging
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Added code to implement Peeking OTA message and 
                   re-arranged code that identifies protocol error.
08/10/01   rj      Fixed a bug in rrcuece_process_ue_capability_enquiry_msg
                   function. Added MSG_HIGH for UE Capability Message
                   reception.
06/05/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"              
#include "rrcuece.h"
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrclcm.h"
#include "rrcsend.h"
#include "rrcscmgr.h"
#include "rrclogging.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define RRCUECE_MAX_TRANSACTION_ID 3


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


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

FUNCTION rrcuece_set_radio_capability_update_requirement

DESCRIPTION
  This function reset the variable 'rrc_capability_update_requirement' and
  set 'rrc_capability_update_requirement' in this variable to TRUE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_set_radio_capability_update_requirement (void)
{
  /* re-set the variable rrc_capability_update_requirement */
  memset(&rrc_capability_update_requirement, 
                        0, sizeof(rrc_capability_update_requirement_type));

  /* set the variable radio capability required to TRUE */
  rrc_capability_update_requirement.ue_radio_capability_update_requirement = TRUE;
}

/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement
(
  rrc_CapabilityUpdateRequirement capability_update_requirement
                                          /* Capability Update requirement */
)
{
  uint32 i = 0,count=0;                            /* Local varible for counting */
  
  if(RRC_MSG_COMMON_BITMASK_IE(capability_update_requirement,
       rrc_CapabilityUpdateRequirement,systemSpecificCapUpdateReqList))
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  TRUE;

    while((capability_update_requirement.systemSpecificCapUpdateReqList.n >  count) && 
        (i < RRC_MAX_SYS_CAPABILITY)) 
    {   
      if(capability_update_requirement.systemSpecificCapUpdateReqList.elem[count]
        == rrc_SystemSpecificCapUpdateReq_gsm)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GSM_CAP;
        i++;
      }
      else
      {
        WRRC_MSG1_HIGH("Unknown System Specific Capability Update for %d",
            capability_update_requirement.systemSpecificCapUpdateReqList.elem[count]);
      }
      /* Advance to the next member of the list */     
     count++;
    }
  }
  else
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  FALSE;
  }

  /* Update the count for the Inter system Capability Update Requirement */
  rrc_capability_update_requirement.system_specific_cap_update_req_list.count =(uint8) i;  
 
  /* Update UE Radio Capability Update Requirement */  
  rrc_capability_update_requirement.ue_radio_capability_update_requirement =
    capability_update_requirement.ue_RadioCapabilityFDDUpdateRequirement;
   
} /* rrcuece_copy_capability_update_requirement */

/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement_r7

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_r7
(
  rrc_CapabilityUpdateRequirement_r7 *capability_update_requirement
                                          /* Capability Update requirement */
)
{
  uint32 i = 0,count =0;                            /* Local varible for counting */

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(capability_update_requirement,
       rrc_CapabilityUpdateRequirement_r7,systemSpecificCapUpdateReqList))
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  TRUE;

    while((capability_update_requirement->systemSpecificCapUpdateReqList.n > count) && 
        (i < RRC_MAX_SYS_CAPABILITY)) 
    {   
      if(capability_update_requirement->systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r5_gsm)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GSM_CAP;
        i++;
      }
      else if(capability_update_requirement->systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r5_geranIu)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GERANIU_CAP;
        i++;
      }
      else
      {
        WRRC_MSG1_HIGH("Unknown System Specific Capability Update for %d",
            capability_update_requirement->systemSpecificCapUpdateReqList.elem[count]);
      }
      /* Advance to the next member of the list */     
      count++;
    }
  }
  else
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  FALSE;
  }

  /* Update the count for the Inter system Capability Update Requirement */
  rrc_capability_update_requirement.system_specific_cap_update_req_list.count = (uint8)i;  
 
  /* Update UE Radio Capability Update Requirement */  
  rrc_capability_update_requirement.ue_radio_capability_update_requirement =
    capability_update_requirement->ue_RadioCapabilityFDDUpdateRequirement_FDD;
   
} /* rrcuece_copy_capability_update_requirement_r5 */

/*===========================================================================
FUNCTION rrcuece_copy_capability_update_requirement_r8

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_r8
(
  rrc_CapabilityUpdateRequirement_r8 *capability_update_requirement
                                          /* Capability Update requirement */
)
{
  uint8 i = 0,count =0;                            /* Local varible for counting */

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(capability_update_requirement,
        rrc_CapabilityUpdateRequirement_r8,systemSpecificCapUpdateReqList))
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  TRUE;

    while((capability_update_requirement->systemSpecificCapUpdateReqList.n > count) && 
        (i < RRC_MAX_SYS_CAPABILITY)) 
    {   
      if(capability_update_requirement->systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r8_gsm)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GSM_CAP;
        i++;
      }
      else if(capability_update_requirement->systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r8_geranIu)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GERANIU_CAP;
        i++;
      }
      else if(capability_update_requirement->systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r8_eutra)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_EUTRA_CAP;
        i++;
      }
      else
      {
        WRRC_MSG1_HIGH("Unknown System Specific Capability Update for %d",
            capability_update_requirement->systemSpecificCapUpdateReqList.elem[count]);
      }
      /* Advance to the next member of the list */     
      count++;
    }
  }
  else
  {
    rrc_capability_update_requirement.system_specific_cap_update_req_present =  FALSE;
  }

  /* Update the count for the Inter system Capability Update Requirement */
  rrc_capability_update_requirement.system_specific_cap_update_req_list.count =(uint8) i;  
 
  /* Update UE Radio Capability Update Requirement */  
  rrc_capability_update_requirement.ue_radio_capability_update_requirement =
    capability_update_requirement->ue_RadioCapabilityFDDUpdateRequirement_FDD;

  return;   
} /* rrcuece_copy_capability_update_requirement_r8 */


/*===========================================================================

FUNCTION rrcuece_copy_capability_update_requirement_later_than_r3

DESCRIPTION
  This function copy the Radio Capability Requirement into internal format.
  This stored information will be utilized by any RRC procedure to build 
  UE Capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcuece_copy_capability_update_requirement_later_than_r3
(
  rrc_CapabilityUpdateRequirement_r5 capability_update_requirement
                                          /* Capability Update requirement */
)
{
  uint32 i = 0,count =0;                            /* Local varible for counting */

  if(RRC_MSG_COMMON_BITMASK_IE(capability_update_requirement,
       rrc_CapabilityUpdateRequirement_r5,systemSpecificCapUpdateReqList))
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  TRUE;

    /* Update System Specific Capability Update Requirement info */
    while((capability_update_requirement.systemSpecificCapUpdateReqList.n > count) && 
        (i < RRC_MAX_SYS_CAPABILITY)) 
    {   
      if(capability_update_requirement.systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r5_gsm)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GSM_CAP;
        i++;
      }
      else if(capability_update_requirement.systemSpecificCapUpdateReqList.elem[count] == rrc_SystemSpecificCapUpdateReq_r5_geranIu)
      {
        rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GERANIU_CAP;
        i++;
      }
      else
      {
        WRRC_MSG1_HIGH("Unknown System Specific Capability Update for %d",
            capability_update_requirement.systemSpecificCapUpdateReqList.elem[count]);
      }
      /* Advance to the next member of the list */     
      count++;
    }
  }
  else
  {
    rrc_capability_update_requirement.
      system_specific_cap_update_req_present =  FALSE;
  }

  /* Update the count for the Inter system Capability Update Requirement */
  rrc_capability_update_requirement.system_specific_cap_update_req_list.count =(uint8) i;  
 
  /* Update UE Radio Capability Update Requirement */  
  rrc_capability_update_requirement.ue_radio_capability_update_requirement =
    capability_update_requirement.ue_RadioCapabilityFDDUpdateRequirement_FDD;
   
} /* rrcuece_copy_capability_update_requirement_r5 */

/*===========================================================================

FUNCTION rrcuece_process_ue_capability_enquiry_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND contains UE Capability
  Enquiry message.If it contains UE Capability  Enquiry message,
  it will processes the same and send RRC_INITIATE_UE_CAPABILITY_REQ.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcuece_process_ue_capability_enquiry_msg
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;    /* Pointer to Downlink DCCH */

  rrc_cmd_type *rrc_cmd_ptr;                 /* Pointer to the RRC Command */

  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC LC Id */
  rrc_RRC_TransactionIdentifier transaction_id;
                                             /* Local variable to store 
                                                transaction Id */
  uint8 message_choice;                      /* Local variable for message
                                                choice */

  /* Set the protocol error reject to FALSE */
  rrc_protocol_error_reject = FALSE;

  /* Assert the received message is of type rrc_DL_CCCH_Message */
  if((RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id) && 
     (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU) )
  {
    message_choice = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

    /* Get the OTA message pointer.  If ASN1 decoding has failed then the
       dcch_msg_ptr will be initialized to NULL */
    dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                    cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
  
    /* Check the received message is UE Capability  Enquiry message */
    if( message_choice ==
        T_rrc_DL_DCCH_MessageType_ueCapabilityEnquiry)
    {
      WRRC_MSG0_HIGH("Received UE Capability Enquiry Message");

      if(dcch_msg_ptr == NULL) /* DCCH ASN1 decoding failed */
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
          WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
        } 
        else /* Valid RLC ID found */        
        {
          if(SUCCESS != rrcsend_send_rrc_status_message(
             T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                           /* The protocol error cause to be
                                              placed in the message */
             0,                            /* Set the transaction id to 0 
                                              since ASN1 decoding failed */
             rrc_ReceivedMessageType_ueCapabilityEnquiry,      /* Received message identification */                                          
             RRC_PROCEDURE_UECE,           /* RRC procedure */
             lc_id,                        /* Logical Channel Id */
             RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                           /* Indicates whether L2 Ack 
                                              required or not */
          {
            WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
          } 
        } /* Valid RLC ID found */      
      }
      
      else /* ASN1 decoding is successful */
      {
        if(dcch_msg_ptr->message.u.ueCapabilityEnquiry.t ==
           T_rrc_UECapabilityEnquiry_later_than_r3 )
        {
          WRRC_MSG0_ERROR("Critical Extensions are not supported");

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
            WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
          } 
          else
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_messageExtensionNotComprehended,
                                             // The protocol error cause to be placed in
                                             // the message

               dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.
               ueCapabilityEnquiry_r3.rrc_TransactionIdentifier,                            
                                             // Received transaction Id 

               rrc_ReceivedMessageType_ueCapabilityEnquiry,  
                                             // Received message identification
               RRC_PROCEDURE_UECE,           // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                       /* Indicates whether L2 Ack required or not */
            {
              WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
            }
          }
          return;
        }
        /* Update Transaction Id */
        transaction_id = dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.
                   ueCapabilityEnquiry_r3.rrc_TransactionIdentifier;

        if(transaction_id > RRCUECE_MAX_TRANSACTION_ID)
        {
          rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                  (uint8) RRC_PROCEDURE_UECE,
                                  (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                  (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                );
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
            WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
          } 
          else
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended,
                                             // The protocol error cause to be placed in
                                             // the message
               transaction_id,               // Received transaction Id 
               rrc_ReceivedMessageType_ueCapabilityEnquiry,  
                                             // Received message identification
               RRC_PROCEDURE_UECE,           // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                       /* Indicates whether L2 Ack required or not */
            {
              WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
            }
          }
        }
       
        else /* Valid Transaction id */
        {
          /*Initialize the UE Capability Update Requirement */
          rrc_capability_update_requirement.
            ue_radio_capability_update_requirement = FALSE;

          rrc_capability_update_requirement.
            system_specific_cap_update_req_present = FALSE;

          /* Copy UE Capability Update Requirement from the UE Capability
             Enquiry message */
          rrcuece_copy_capability_update_requirement(
            dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.
            ueCapabilityEnquiry_r3.capabilityUpdateRequirement);


          if((dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.m.laterNonCriticalExtensionsPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
                   v770NonCriticalExtensionsPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                   v770NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                   v770NonCriticalExtensions.v860NonCriticalExtensions.ueCapabilityEnquiry_v860ext.m.capabilityUpdateRequirementPresent) &&
              (dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                   v770NonCriticalExtensions.v860NonCriticalExtensions.ueCapabilityEnquiry_v860ext.capabilityUpdateRequirement.m.systemSpecificCapUpdateReqListPresent))
          {
              /* Appending the list filled with ueCapabilityEnquiry_r3.capabilityUpdateRequirement.rrcuece_copy_capability_update_requirement */
              uint8 count = 0, i = rrc_capability_update_requirement.system_specific_cap_update_req_list.count;
              rrc_SystemSpecificCapUpdateReqList_r8 *systemSpecificCapUpdateReqList;

              systemSpecificCapUpdateReqList = &dcch_msg_ptr->message.u.ueCapabilityEnquiry.u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
                   v590NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.ueCapabilityEnquiry_v860ext.capabilityUpdateRequirement.systemSpecificCapUpdateReqList;

              rrc_capability_update_requirement.system_specific_cap_update_req_present =  TRUE;

              while((systemSpecificCapUpdateReqList != NULL ) && (systemSpecificCapUpdateReqList->n > count) && 
                  (i < RRC_MAX_SYS_CAPABILITY)) 
              {   
                if(systemSpecificCapUpdateReqList->elem[count] == rrc_SystemSpecificCapUpdateReq_r8_gsm)
                {
                  rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GSM_CAP;
                  i++;
                }
                else if(systemSpecificCapUpdateReqList->elem[count] == rrc_SystemSpecificCapUpdateReq_r8_geranIu)
                {
                  rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_GERANIU_CAP;
                  i++;
                }
                #ifdef FEATURE_WCDMA_TO_LTE
                else if(systemSpecificCapUpdateReqList->elem[count] == rrc_SystemSpecificCapUpdateReq_r8_eutra)
                {
                  rrc_capability_update_requirement.system_specific_cap_update_req_list.value[i] = RRC_EUTRA_CAP;
                  i++;
                }
                #endif
                else
                {
                  WRRC_MSG1_HIGH("Unknown System Specific Capability Update for %d",
                      systemSpecificCapUpdateReqList->elem[count]);
                }
                /* Advance to the next member of the list */     
                count++;
              }

              /* Update the count for the Inter system Capability Update Requirement */
              rrc_capability_update_requirement.system_specific_cap_update_req_list.count = i;
          }


          /* Validate UE Capability Update Requirement */
          if(( rrc_capability_update_requirement.
              ue_radio_capability_update_requirement == TRUE) ||
 
              (rrc_capability_update_requirement.
               system_specific_cap_update_req_present == TRUE) )
          {
            if (rrc_capability_update_requirement.
               system_specific_cap_update_req_present == TRUE)
            {
              if(rrc_capability_update_requirement.
                 system_specific_cap_update_req_list.count == 0)
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
                  WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
                } 
                else
                {
                  if(SUCCESS != rrcsend_send_rrc_status_message(
                     T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended,
                                                   // The protocol error cause to be placed in
                                                   // the message
                     transaction_id,               // Received transaction Id 
                     rrc_ReceivedMessageType_ueCapabilityEnquiry,  
                                                   // Received message identification
                     RRC_PROCEDURE_UECE,           // RRC procedure
                     lc_id,                        /* Logical Channel Id */
                     RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                             /* Indicates whether L2 Ack required or not */
                  {
                    WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
                  }
                }
                return;
              }
            } /* System capability == TRUE */
            
            /* Allocates the buffer to RRC Internal command */
            rrc_cmd_ptr =  rrc_get_int_cmd_buf();
            /* Fill in the command parameters for 
             RRC_INITIATE_UE_CAPABILITY_REQ */
            rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_UE_CAPABILITY_REQ;

            rrc_cmd_ptr->cmd.initiate_ue_cap.originator = 
            RRC_UE_CAPABILITY_ORIGINATOR_UTRAN;
          
            rrc_cmd_ptr->cmd.initiate_ue_cap.transaction_id = 
            transaction_id;
          
            /* Send RRC_INITIATE_UE_CAPABILITY_REQ primitive */
            rrc_put_int_cmd(rrc_cmd_ptr);   
            } 
            else
            {
            /* UE Radio Capability Update Requirement is not set. Then send
               RRC Status message */
            WRRC_MSG2_ERROR("UE Radio Cap Update Req. is not set: %d %d",
                      rrc_capability_update_requirement.
                        ue_radio_capability_update_requirement,
                      rrc_capability_update_requirement.
                        system_specific_cap_update_req_present);

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
              WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
            } 
            else
            {
              if(SUCCESS != rrcsend_send_rrc_status_message(
                 T_rrc_ProtocolErrorMoreInformation_type1_ie_ValueNotComprehended,
                                               // The protocol error cause to be placed in
                                               // the message
                 transaction_id,               // Received transaction Id 
                 rrc_ReceivedMessageType_ueCapabilityEnquiry,  
                                               // Received message identification
                 RRC_PROCEDURE_UECE,           // RRC procedure
                 lc_id,                        /* Logical Channel Id */
                 RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                         /* Indicates whether L2 Ack required or not */
              {
                WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
              }
            }
          } 
        } /* Valid Transaction id */
      } /* ASN1 decoding is successful */
    }
    else
    {
      WRRC_MSG1_HIGH("Invalid msg: %d is forwarded", message_choice);
    }
  }
  else
  {
    WRRC_MSG2_HIGH("Invalid Event: %d or SDU type: %d",
              cmd_ptr->cmd_hdr.cmd_id,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
  }

} /* rrcuece_process_ue_capability_enquiry_msg */
/*===========================================================================

FUNCTION rrcuece_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the UE Capability Enquiry procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UE 
    CAPABILITY ENQUIRY message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcuece_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();

  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_UECE,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
    case RRC_STATE_CONNECTING:
      /* In this state there is no RRC Connection. Hence trash the message */
      WRRC_MSG1_HIGH("UECE Msg is received in invalid state: %d",
                rrc_state); 
      break;

    case RRC_STATE_CELL_FACH:    
    case RRC_STATE_CELL_DCH:  
      /* These two states are valid to receive UE Capability Enquiry message */
      rrcuece_process_ue_capability_enquiry_msg(cmd_ptr); 
      break;

    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      /* The logical DCCH is released in these states.Due to race conditions, RRC
         may receive a valid UE Capability Enquiry message. Hence process the 
         same. */
      WRRC_MSG0_HIGH("UECE msg is recievd in PCH state");
      rrcuece_process_ue_capability_enquiry_msg(cmd_ptr);
      break;

    default: 
      /* This should never happen */
      ERR_FATAL("Event fwded to UECE in invalid State: %d",rrc_state,0,0);
      break;
   }  
} /* rrcuece_procedure_event_handler */
/*===========================================================================

FUNCTION rrcuece_init_procedure

DESCRIPTION
  This function initializes required parameters for UE Capability Enquiry
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcuece_init_procedure
( 
  void
)
{
  // This is a template for future expansion */
} /* rrcuece_init_procedure */
