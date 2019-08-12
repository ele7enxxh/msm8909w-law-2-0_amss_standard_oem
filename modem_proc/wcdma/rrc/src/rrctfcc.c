/*===========================================================================
              R R C TRANSPORT FORMAT COMBINATION CONTROL M O D U L E 

DESCRIPTION

  This module performs the Transport Format combination Control Procedure. This
  module processes the TFCC Message and onfigures the transport format
  combination subset.
  
  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  
Copyright (c) 2000, 2001, 2004, 2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrctfcc.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/15   as      Added code to support LTA durting activation time
07/27/15   sp      Made changes to send TFCC failure if OTA is received on SRB-1
09/03/14   sp      Made changes to save procedure substates in crash debug info
07/23/14   sp      Made changes to remove F3s as part of FR19999
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
01/03/13   ad      Made changes to reduce the redundant F3s
03/12/12   rl      Made changes to fix the FEATURE_WCDMA_NEW_L2_INTERFACE porting issue.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/17/11   ad      Made changes in RRC related to L2 SW Decoupling
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/15/10   su      Fixed Compilation warnings.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/02/10   su      Provided fix to process TFCC message while another 
                   Reconfiguration is on going.
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/19/10   ssg     Added changes for new WCDMA-L2 interface.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/28/09   gkg     Fixed KW Errors.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7. Removed the 
                   feature flag FEATURE TFCS CONTROL ENABLE
11/14/07   rm      Made changes to update CC for zero control duration under
                   FEATURE TFCC ENHNC
08/21/06   tkk     Added SRB#5 support.
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
08/30/04   sgk     Typecast return of rrclcm_get_ul_rb_id,
                   activationTimeForTFCSubset,
                   tfc_subset->u.minimumAllowedTFC_Number,
                   allow_tfc_list->value, ntfci, non_allow_tfc_list->value,
                   trch_list->value.restrictedTrChIdentity to uint16 in function
                   rrctfcc_process_message to fix lint error Loss of precision
                   (assignment) (32 bits to 16 bits).
                   Typecast rrc_TransactionIdentifier, allow_tfi_list->value
                   to uint8 in function rrctfcc_process_message to fix lint
                   error Loss of precision (assignment) (32 bits to 8 bits).
                   Checked if trch_list->value.ul_TransportChannelType ==
                   rrc_UL_TrCH_Type_dch, if so assigned
                   trch_restriction_info[ntrch].trch_type = DCH;
                   else set invalid_config to true and broke out of while loop
                   in function rrctfcc_process_message to fix lint error Type
                   mismatch (assignment) (enum/enum).
                   Typecast ntfi to uint16 and ntrch to uint8 in function
                   rrctfcc_process_message to fix lint error Loss of precision
                   (assignment) (31 bits to 16 bits).
                   Added default for switch in function rrctfcc_process_message
                   to fix lint error no default for switch.
07/27/04   ttl     Convert TFCC duration from enum to ms.
07/20/04   ttl     Fixed a bug in checking the TDD option in TFCC message.
06/17/04   ttl     Initial created file.


===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "memheap.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rex.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrcllc.h"
#include "rrcllcoc.h"
#include "rrclcm.h"
#include "rrcmmif.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrccu.h"
#include "rrccui.h"

#include "l2ultask.h"

#include "rrctfcc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
mac_tfc_subset_restriction_info_type mac_tfc_subset_info;
rrctfcc_substates_e_type rrctfcc_substate;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION   RRCTFCC_SEND_TFCC_CONFIG_FAILURE

DESCRIPTION
  This function prepares TFCC config failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void  rrctfcc_send_tfcc_config_failure
(
  uint8 transaction_id 
)
{
  rrc_UL_DCCH_Message *msg_ptr;         /* Pointer to uplink DCCH message */ 
  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */
  rrcsend_status_e_type send_status;    /* The status of sending the Initial
                                         * Security Mode Failure */

  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                       DCCH_AM_RADIO_BEARER_ID, 
                                       UE_MODE_ACKNOWLEDGED_DATA);

  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(struct rrc_UL_DCCH_Message));
    msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_transportFormatCombinationControlFailure;

    /* No non Critical Extensions present.  */
    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.transportFormatCombinationControlFailure);

    msg_ptr->message.u.transportFormatCombinationControlFailure.
                                        rrc_TransactionIdentifier = transaction_id;

    msg_ptr->message.u.transportFormatCombinationControlFailure.
                              failureCause.t = T_rrc_FailureCauseWithProtErr_invalidConfiguration;

    send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_TFC,
                                      msg_ptr,
                                      ul_lc_id,
                                      RRCSEND_L2ACK_NOT_REQUIRED);

    if (send_status == RRCSEND_SUCCESS)
    {
      WRRC_MSG0_HIGH("TFCC Failure Message Sent");      
    }
    else
    {
      WRRC_MSG1_ERROR("Error %d sending TFCC FAILURE", send_status);
    }
  }
  else
  {
    WRRC_MSG0_ERROR("LC ID not found in tx'n TFCC failure");
  }

}


/*===========================================================================

FUNCTION   RRCTFCC_GET_DURATION

DESCRIPTION
  Convert TFCC duration value into msec.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static uint16 rrctfcc_get_duration(rrc_TFC_ControlDuration tfcc_duration)
{
  uint16 duration = 0;    /* the default duration is 0. */

  switch (tfcc_duration)
  {
    case rrc_TFC_ControlDuration_tfc_cd1:
      duration = 10;
      break;

    case rrc_TFC_ControlDuration_tfc_cd2:
      duration = 20;
      break;

    case rrc_TFC_ControlDuration_tfc_cd4:
      duration = 40;
      break;

    case rrc_TFC_ControlDuration_tfc_cd8:
      duration = 80;
      break;

    case rrc_TFC_ControlDuration_tfc_cd16:
      duration = 160;
      break;

    case rrc_TFC_ControlDuration_tfc_cd24:
      duration = 240;
      break;

    case rrc_TFC_ControlDuration_tfc_cd32:
      duration = 320;
      break;

    case rrc_TFC_ControlDuration_tfc_cd48:
      duration = 480;
      break;

    case rrc_TFC_ControlDuration_tfc_cd64:
      duration = 640;
      break;

    case rrc_TFC_ControlDuration_tfc_cd128:
      duration = 1280;
      break;

    case rrc_TFC_ControlDuration_tfc_cd192:
      duration = 1920;
      break;

    case rrc_TFC_ControlDuration_tfc_cd256:
      duration = 2560;
      break;

    case rrc_TFC_ControlDuration_tfc_cd512:
      duration = 5120;
      break;
  }

  return duration;
}


/*===========================================================================

FUNCTION   RRCTFCC_UPDATE_TFCC_RESTRICTION_IN_MAC

DESCRIPTION
  This function updates the TFCC restriction in MAC.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/  
static void rrctfcc_update_tfcc_restriction_in_mac
(
  mac_tfc_subset_restriction_info_type *mac_info, 
  tfc_subset_info_type                 *rrc_tfc_info
)
{
  uint32 i, j;

  switch(rrc_tfc_info->restriction_choice)
  {
  case MINIMUM_ALLOWED_TFC:
    mac_info->tfci_restriction_choice = MIN_ALLOWED_TFCI;
    mac_tfc_subset_info.restriction_info.tfci = (uint16)rrc_tfc_info->tfc_choice_info.minimum_allowed_tfc_number;
    break;

  case ALLOWED_TFC_LIST:
    mac_info->tfci_restriction_choice = ALLOWED_TFCI;
    mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc = rrc_tfc_info->tfc_choice_info.allowed_tfc_list.total_allowed_tfc_count;
    
    i = 0;
    while(i < mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc)
    {
      mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[i] = rrc_tfc_info->tfc_choice_info.allowed_tfc_list.allowed_tfc_number[i];
      i++;
    }
    break;

  case NON_ALLOWED_TFC_LIST:
    mac_info->tfci_restriction_choice = NOT_ALLOWED_TFCI;
    mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc = rrc_tfc_info->tfc_choice_info.non_allowed_tfc_list.total_non_allowed_tfc_count;

    i = 0;
    while(i < mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc)
    {
      mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[i] = rrc_tfc_info->tfc_choice_info.non_allowed_tfc_list.non_allowed_tfc_number[i];
      i++;
    }
    break;

  case RESTRICTED_TRCH_LIST:
    mac_info->tfci_restriction_choice = RESTRICTED_TFCI;
    mac_tfc_subset_info.restriction_info.trch_restriction_info_list.ntrch = rrc_tfc_info->tfc_choice_info.restricted_trch_list.no_of_transport_channels;
    
    i = 0;
    while(i < mac_tfc_subset_info.restriction_info.trch_restriction_info_list.ntrch)
    {
      mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].trch_id = (tr_ch_id_type)rrc_tfc_info->tfc_choice_info.restricted_trch_list.trch_info[i].trch_id;
      if(rrc_tfc_info->tfc_choice_info.restricted_trch_list.trch_info[i].dch_present)
      {
        mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].trch_type = DCH;
      }
      else
      {
        mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].trch_type = INVALID_UL_CCTRCH;
      }
      
      if( rrc_tfc_info->tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info_present)
      {
        mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].ntf = rrc_tfc_info->tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info.no_of_allowed_tfis;
       
        j = 0;
        while(j < mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].ntf)
        {
          mac_tfc_subset_info.restriction_info.trch_restriction_info_list.trch_restriction_info[i].tfi[j] = rrc_tfc_info->tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info.tfi[j];
          j++;
        }
      }
      ++i;
    }
    break;
  
  case TFCS_FULL:
    mac_tfc_subset_info.tfci_restriction_choice = FULL_TFCI;
    break;
  
  default:
    WRRC_MSG0_ERROR("Err t .. should not come this far");
    break;
  }
} /* rrctfcc_update_tfcc_restriction_in_mac */
/*===========================================================================

FUNCTION   RRCTFCC_PROCESS_MESSAGE

DESCRIPTION
  This function process the received TFCC message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void rrctfcc_process_message
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  rrc_TFC_Subset *tfc_subset = NULL;
  int ntfci = 0, ntrch = 0, ntfi = 0;
  boolean invalid_config = FALSE;
  rrc_AllowedTFC_List *allow_tfc_list = NULL;
  rrc_Non_allowedTFC_List *non_allow_tfc_list = NULL;
  rrc_RestrictedTrChInfoList *trch_list = NULL;
  rrc_AllowedTFI_List *allow_tfi_list = NULL;
  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;
  boolean tm_tfcc_msg_recvd = FALSE;
  uint8 tfc_subset_idx = 0;
  rrc_RB_Identity tfcc_rb_id;  
uint32 idx=0,idy=0;
  
  tfcc_rb_id = rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);

  if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU)
  {
    if ( cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL )
    {
      /* Get the OTA message pointer */
      dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                      cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
    }
    else
    {
      WRRC_MSG0_ERROR("Rx'd empty TFCC msg");
      return;
    }
  }
  
  
  if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU)
  {
    if((tfcc_rb_id == DCCH_AM_RADIO_BEARER_ID) ||
       (tfcc_rb_id == DCCH_UM_RADIO_BEARER_ID))
    {
      WRRC_MSG1_HIGH("TFCC received on UM/AM .. RBId - %d", tfcc_rb_id);

      if (!(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.transportFormatCombinationControl,
          rrc_TransportFormatCombinationControl,rrc_TransactionIdentifier)))
      { 
        invalid_config = TRUE;
        WRRC_MSG0_HIGH("No TransID presents in TFCC MSG");
      } 

      /* Make sure that TDD option is not presented.  */
      if (RRC_CHECK_COMMON_MSG_TYPE(dcch_msg_ptr->message.u.transportFormatCombinationControl.modeSpecificInfo,
          rrc_TransportFormatCombinationControl_modeSpecificInfo_tdd))
      {
        invalid_config = TRUE;
        WRRC_MSG0_ERROR("Do not support TDD option in TFCC");
      }
      if((rrc_get_state() == RRC_STATE_CELL_FACH) && 
           ( (RRC_MSG_COMMON_BITMASK_IE_TYPE2(dcch_msg_ptr->message.u.transportFormatCombinationControl,
             activationTimeForTFCSubset)) || 
             (RRC_MSG_COMMON_BITMASK_IE_TYPE2(dcch_msg_ptr->message.u.transportFormatCombinationControl,
              tfc_ControlDuration))))
      {
         invalid_config = TRUE;
         WRRC_MSG0_ERROR("TFCC msg received in Cell FACH state with Action Time/Control duration");
      }
      if(rrctfcc_substate == RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF)
      {
        invalid_config = TRUE;
        WRRC_MSG0_ERROR("Recvd 2nd TFCC message when we are procesing one TFCC");
      }
    }
  }
  else if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == RRCLCM_DL_DCCH_SRB5_SDU_TYPE)
  {
    if((rrcllc_has_config_srb5(CURRENT_CONFIG, (uint16)tfcc_rb_id) == TRUE)
      &&(rrctfcc_substate != RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF))
    {  
      tfc_subset_idx = (*((uint8 *)cmd_ptr->cmd.downlink_sdu_ind.dl_sdu)) >> 5;
  
      MSG_HIGH("TFCC received on TM mode for RBId %d Current config has %d subset lists; TFC subset index -> %d", tfcc_rb_id,current_config_ptr->ul_tfc_subset_list.no_of_subset_lists, tfc_subset_idx);

      /* Only entertain TFC subset index for which we have
         TFC subset list information */
      if(tfc_subset_idx < current_config_ptr->ul_tfc_subset_list.no_of_subset_lists)
      {
        tm_tfcc_msg_recvd = TRUE;
        /* Retrieve the corresponding TFC subset information to
           be passed to MAC */

        /* Reset local config memory to have activation time now
           & transaction ID as ZERO */
        memset(&mac_tfc_subset_info, 0, sizeof(mac_tfc_subset_restriction_info_type));
        mac_tfc_subset_info.rb_id = tfcc_rb_id;
        mac_tfc_subset_info.control_duration = 0; /* ZERO signifies that control duration is applicable till next configuration */ 
        mac_tfc_subset_info.activation_time = 0;

        /* copy the relevant TFC choices * restrictions here */
        rrctfcc_update_tfcc_restriction_in_mac(&mac_tfc_subset_info, &current_config_ptr->ul_tfc_subset_list.tfc_subset_info[tfc_subset_idx]);
      }
      else
      {
        invalid_config = TRUE;
        WRRC_MSG0_ERROR("Invalid TFCC recvd on TM .. rejecting");
      }
    }
    else
    {
      invalid_config = TRUE;
      if(rrctfcc_substate == RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF)
      {
        WRRC_MSG0_ERROR("Recvd 2nd TFCC message when we are procesing one TFCC");
      }
      else
      {
        WRRC_MSG0_ERROR("SRB#5 doesn't exist ... TFCC is recvd on TM .. rejecting");
      }
    }
  }
  else
  {
    invalid_config = TRUE;
    WRRC_MSG0_ERROR("TFCC received on unknown RLC mode");
  }

  if (!invalid_config && tm_tfcc_msg_recvd == FALSE)
  {
    /* Reset local config memory  */
    memset(&mac_tfc_subset_info, 0, sizeof(mac_tfc_subset_restriction_info_type));

    tfc_subset = &(dcch_msg_ptr->message.u.transportFormatCombinationControl.dpch_TFCS_InUplink);

    /* Pass RB ID and Transaction ID to MAC. So that once MAC identifies that
     * the TFCC config is invalid, MAC can send the failure report to RRC along
     * w/ these info. RRC then can decide to send a Config Failure report to NW 
     * if the TFCC message is received in SRB2/SRB1.
     */
    mac_tfc_subset_info.rb_id = tfcc_rb_id;
    mac_tfc_subset_info.transaction_id = 
        (uint8)dcch_msg_ptr->message.u.transportFormatCombinationControl.rrc_TransactionIdentifier;

    /* Save Activation time.  */ 
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(dcch_msg_ptr->message.u.transportFormatCombinationControl,
        activationTimeForTFCSubset))
    {
      mac_tfc_subset_info.activation_time = 
          (uint16)dcch_msg_ptr->message.u.transportFormatCombinationControl.activationTimeForTFCSubset;
    }
    else
    {
      mac_tfc_subset_info.activation_time = 0;
    }

    /* TFCC duration is presented. Save the control Duration. */
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(dcch_msg_ptr->message.u.transportFormatCombinationControl,
        tfc_ControlDuration))
    {
      mac_tfc_subset_info.control_duration = 
        rrctfcc_get_duration(dcch_msg_ptr->message.u.
                             transportFormatCombinationControl.tfc_ControlDuration);
    }

    MSG_HIGH("RBId %d, TransId %d t %d", mac_tfc_subset_info.rb_id, 
           mac_tfc_subset_info.transaction_id,tfc_subset->t);

    /* There are five options of TFC subset representations. */
    switch (tfc_subset->t)
    {
      case T_rrc_TFC_Subset_minimumAllowedTFC_Number:
        mac_tfc_subset_info.tfci_restriction_choice = MIN_ALLOWED_TFCI;

        /* The config'ing minimum TFCI > the higest of the current config'd tfci. */
        if (tfc_subset->u.minimumAllowedTFC_Number >= current_config_ptr->ul_tfcs.ntfc)
        {
          invalid_config = TRUE;
          break;
        }
        mac_tfc_subset_info.restriction_info.tfci = (uint16)tfc_subset->u.minimumAllowedTFC_Number;
        WRRC_MSG1_HIGH("MInAllowed TFCI %d", mac_tfc_subset_info.restriction_info.tfci);
        break;

      case T_rrc_TFC_Subset_allowedTFC_List:
        mac_tfc_subset_info.tfci_restriction_choice = ALLOWED_TFCI;

        ntfci = 0;
        allow_tfc_list = tfc_subset->u.allowedTFC_List;
		  idx=0;
        while (allow_tfc_list->n > idx)
        {
          /* For each tfci, it should not greater than the highest config'd TFCI  */
          if (allow_tfc_list->elem[idx] >= current_config_ptr->ul_tfcs.ntfc)
          {
            invalid_config = TRUE;
            break;
          }
          mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[ntfci] = 
                                                                    (uint16)allow_tfc_list->elem[idx];
          WRRC_MSG2_HIGH("Allowed TFCI %d, ntfc %d",
                   mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[ntfci], ntfci);
          ntfci ++;
          if (ntfci == UE_MAX_TFC)
          {
            WRRC_MSG0_HIGH("Too many allow TFCI");
            invalid_config = TRUE;
            break;
          }
          idx++;
        }
        mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc = (uint16)ntfci;
        break;

      case T_rrc_TFC_Subset_non_allowedTFC_List:
        mac_tfc_subset_info.tfci_restriction_choice = NOT_ALLOWED_TFCI;

        ntfci = 0;
        non_allow_tfc_list = tfc_subset->u.non_allowedTFC_List;
		  idx=0;
        while (non_allow_tfc_list->n > idx)
        {
          /* For each tfci, it should not greater than the highest config'd TFCI  */
          if (non_allow_tfc_list->elem[idx] >= current_config_ptr->ul_tfcs.ntfc)
          {
              invalid_config = TRUE;
              break;
            }
            mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[ntfci] = 
            (uint16)non_allow_tfc_list->elem[idx];
            WRRC_MSG2_HIGH("NotAllowed TFCI %d, ntfc %d",
                     mac_tfc_subset_info.restriction_info.tfci_restriction_info.tfci_list[ntfci], ntfci);
            ntfci ++;
            if (ntfci == UE_MAX_TFC)
            {
              WRRC_MSG0_HIGH("Too many non_allow TFCI");
              invalid_config = TRUE;
              break;
            }
            idx++;
        }
        mac_tfc_subset_info.restriction_info.tfci_restriction_info.ntfc = (uint16)ntfci;
        break;

      case T_rrc_TFC_Subset_restrictedTrChInfoList:
        mac_tfc_subset_info.tfci_restriction_choice = RESTRICTED_TFCI;

        /* For each Transport Channel, store the restriction info */
        ntrch = 0;
        trch_list = tfc_subset->u.restrictedTrChInfoList;
		  idx=0;
        while (trch_list->n > idx)
        {
          mac_tfc_subset_info.restriction_info.trch_restriction_info_list.
          trch_restriction_info[ntrch].trch_id = (uint16)trch_list->elem[idx].restrictedTrChIdentity;

        
          if(trch_list->elem[idx].ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
          {
            mac_tfc_subset_info.restriction_info.trch_restriction_info_list.
              trch_restriction_info[ntrch].trch_type = DCH;
          }
          else
          {
            invalid_config = TRUE;
            break;
          }
          ntfi = 0;
          if (trch_list->elem[idx].m.allowedTFI_ListPresent)
          {

            /* Store the TFI list.  */
            allow_tfi_list = &trch_list->elem[idx].allowedTFI_List;
			  idy=0;
            while (allow_tfi_list->n > idy)
            {
              mac_tfc_subset_info.restriction_info.trch_restriction_info_list.
              trch_restriction_info[ntrch].tfi[ntfi] = (uint8)allow_tfi_list->elem[idy];
              ntfi ++;
              if (ntfi == UE_MAX_TF)
              {
                  WRRC_MSG2_HIGH("Too many tf %d in a TrCH %d", ntfi, trch_list->elem[idx].restrictedTrChIdentity);
                  invalid_config = TRUE;
                  break;
                }
                idy++;
            }
            mac_tfc_subset_info.restriction_info.trch_restriction_info_list.
            trch_restriction_info[ntrch].ntf = (uint16)ntfi;
          }
          ntrch ++;
          if (invalid_config || ntrch == UE_MAX_TRCH)
          {
            if (ntrch == UE_MAX_TRCH)
            {
                WRRC_MSG1_HIGH("Too many TrCH in TFCC %d", ntrch);
                invalid_config = TRUE;
              }
              break;
          }
          idx++;
          WRRC_MSG2_HIGH("Restricted TFI %d, ntrch %d", ntfi, ntrch);
        }
        mac_tfc_subset_info.restriction_info.trch_restriction_info_list.ntrch = (uint8)ntrch;
        break;

        case T_rrc_TFC_Subset_fullTFCS:
          /*For FEATURE_CS_VOICE_OVER_HSPA, this will happen so we may need to avoid sending TFCS CFG REQ to MAC,
          if there's no DCH or there's no TFCS restrictions already?*/
          mac_tfc_subset_info.tfci_restriction_choice = FULL_TFCI;
          WRRC_MSG0_HIGH("FULL TFCI LIST chosen");
          break;
        default:break;
      }
  }

  if (invalid_config)
  {
    WRRC_MSG0_HIGH("Invalid CFG hit");
    /*Send TFCC failure if TFCC is recieved on SRB-1/SRB-2 */
    if (tfcc_rb_id == DCCH_AM_RADIO_BEARER_ID ||
        tfcc_rb_id == DCCH_UM_RADIO_BEARER_ID)
    {
      if(dcch_msg_ptr != NULL)
      {
        rrctfcc_send_tfcc_config_failure((uint8)dcch_msg_ptr->message.u.
                                         transportFormatCombinationControl.rrc_TransactionIdentifier);
        WRRC_MSG0_HIGH("Send TFCC failure message");
      }
    }
    if(rrctfcc_substate != RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF)
    {
      rrctfcc_clear_procedure();
    }
  }
  else if((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
          || (rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
         )
  {
    /* Some config is on the way. Do not config TFCC at this time.  */
    WRRC_MSG0_HIGH("Some other config is going, wait for reconfig complete.");
    rrcllc_register_for_reconfig_cnf();

    rrctfcc_substate = RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF;
  }
  else if(rrc_get_state()== RRC_STATE_CELL_FACH)
  {
    WRRC_MSG0_HIGH("TFCC: TFCC message received in FACH state");
    if(mac_tfc_subset_info.control_duration == 0)
    {
      rrcllc_update_tfcc_restriction_in_cc_oc(&mac_tfc_subset_info);
    }
    rrctfcc_clear_procedure();
  }
  else
  {
    l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf();
    /* Fill in the Command Header */
    l2_ul_cmd_ptr->cmd_hdr.cmd_id = CMAC_UL_TFC_SUBSET_CFG_REQ;

    l2_ul_cmd_ptr->cmd_data.mac_tfc_subset_config_info = mac_tfc_subset_info;

      WRRC_MSG0_HIGH("Dispatching out L2 cmd / Sending CMAC_UL_TFC_SUBSET_CFG_REQ");

    /* Send command to L2 UL Task */
    rrc_send_l2_ul_cmd(l2_ul_cmd_ptr);
    rrctfcc_substate = RRCTFCC_WAIT_FOR_MAC_CFG_CNF;
  }
}
/*===========================================================================

FUNCTION   rrctfcc_process_reconfig_cnf

DESCRIPTION
  This function process the received reconfig CNF
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void rrctfcc_process_reconfig_cnf
(
  void
)
{
  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;

  if(rrctfcc_substate != RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF)
  {
    WRRC_MSG1_HIGH("TFCC: Recvd reconfig CNF in wrong state[%d],ignore this cmd",rrctfcc_substate);
    return;
  }

  if(rrc_get_state()== RRC_STATE_CELL_FACH)
  {
    WRRC_MSG0_HIGH("TFCC: TFCC message received in FACH state");

    if(mac_tfc_subset_info.control_duration == 0)
    {
      rrcllc_update_tfcc_restriction_in_cc_oc(&mac_tfc_subset_info);
    }

    rrctfcc_clear_procedure();
  }
  else
  {
    l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf();
    /* Fill in the Command Header */
    l2_ul_cmd_ptr->cmd_hdr.cmd_id = CMAC_UL_TFC_SUBSET_CFG_REQ;
    l2_ul_cmd_ptr->cmd_data.mac_tfc_subset_config_info = mac_tfc_subset_info;
      WRRC_MSG0_HIGH("Dispatching out L2 cmd / Sending CMAC_UL_TFC_SUBSET_CFG_REQ");

    /* Send command to L2 UL Task */
    rrc_send_l2_ul_cmd(l2_ul_cmd_ptr);
    rrctfcc_substate = RRCTFCC_WAIT_FOR_MAC_CFG_CNF;
  }
}

/*===========================================================================

FUNCTION   RRCTFC_PROCEDURE_EVENT_HANDLER

DESCRIPTION
  
  Transport Format Combination Control procedure handler.

        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
void rrctfc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_TFC,rrctfcc_substate,cmd_ptr->cmd_hdr.cmd_id);
  WRRC_MSG2_HIGH("RRCTFCC: command id:%x recd in substate:%d",cmd_ptr->cmd_hdr.cmd_id, rrctfcc_substate);

  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_DOWNLINK_SDU_IND:
      rrctfcc_process_message( cmd_ptr );
      break;
    case RRC_CMAC_UL_TFC_SUBSET_CFG_CNF:
      rrctfcc_process_mac_config_cnf(cmd_ptr);
      break;
    case RRC_STATE_CHANGE_IND:
      rrctfcc_clear_procedure();
      break;
    case RRC_RECONFIG_COMPLETE_IND:
      rrctfcc_process_reconfig_cnf();
      break;

    default:
      WRRC_MSG1_ERROR("Un Expected Command %x",cmd_ptr->cmd_hdr.cmd_id);
      break;
  }
}


/*===========================================================================

FUNCTION  RRCTFCC_CLEAR_PROCEDURE

DESCRIPTION

  This function clears the TFCC Procedure
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrctfcc_clear_procedure
(
  void
)
{
  rrctfcc_substate = RRCTFCC_INITIAL;
  memset(&mac_tfc_subset_info, 0, sizeof(mac_tfc_subset_restriction_info_type));
}


/*===========================================================================

FUNCTION   RRCTFCC_PROCESS_MAC_CONFIG_CNF

DESCRIPTION
  This function process the received MAC CFG CNF
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
void rrctfcc_process_mac_config_cnf
(
  rrc_cmd_type * cmd_ptr
)
{
  WRRC_MSG1_HIGH("TFCC: Recvd CNF from MAC UL status: %d",cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.status);
  if(rrctfcc_substate != RRCTFCC_WAIT_FOR_MAC_CFG_CNF)
  {
    WRRC_MSG1_ERROR("MAC CNF is not expected in substate %d",rrctfcc_substate);
    rrctfcc_clear_procedure();
    return;
  }
  if(cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.status == TRUE)
  {
    /*MAC has been succesfully configured */
    if((cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.transaction_id == mac_tfc_subset_info.transaction_id)&&
        (mac_tfc_subset_info.control_duration == 0))
    {
      /* Update CC with mac_tfc_subset_info */
      rrcllc_update_tfcc_restriction_in_cc_oc(&mac_tfc_subset_info);
    }
  }
  else
  {
    /* If FEATURE TFCC ENHNC is not defned MAC sends this command only when the TFC subset config failure.
       * The RRC then sends config failure info in AM only when the rx'd
       * config is from RRC AM/UM.
       */
    /* MAC has not been configured Correctly */
    if (cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.rb_id == DCCH_AM_RADIO_BEARER_ID ||
        cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.rb_id == DCCH_UM_RADIO_BEARER_ID )
    {
      rrctfcc_send_tfcc_config_failure(
                                      cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.transaction_id);
    }
    else
    {
    WRRC_MSG1_ERROR("TFCC failed for the RB_id %d",cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.rb_id);
    }
  }
  rrctfcc_clear_procedure();
}
/*===========================================================================

FUNCTION  RRCTFCC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the TFCC Procedure
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrctfcc_init_procedure
(
  void
)
{
  rrctfcc_substate = RRCTFCC_INITIAL;
  rrcscmgr_register_for_scn( RRC_PROCEDURE_TFC,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );
  memset(&mac_tfc_subset_info, 0, sizeof(mac_tfc_subset_restriction_info_type));
}

/*===========================================================================

FUNCTION  rrctfcc_is_reconfig_in_progress

DESCRIPTION

  This function will check if TFCC Procedure substate is reconfig in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if TFCC procedure is wating for reconfig in progress
  FALSE: othewise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrctfcc_is_reconfig_in_progress
(
  void
)
{
  boolean status = FALSE;

  if(rrctfcc_substate == RRCTFCC_WAIT_FOR_RECONFIG_CFG_CNF)
  {
    status = TRUE;
  }
  return status;
}
/*===========================================================================

FUNCTION  rrctfcc_get_current_substate

DESCRIPTION

  This function will return the current substate
    
DEPENDENCIES

  None.

RETURN VALUE
rrctfcc_substates_e_type


SIDE EFFECTS

  None.

===========================================================================*/

rrctfcc_substates_e_type rrctfcc_get_current_substate(void)
{
  return rrctfcc_substate;
}


