
/*========================================================================================

                                    R R C B M C 

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by RRC and LSM

  Copyright (c) 2005, 2007, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcbmc.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/14   sp      Made changes to save procedure substates in crash debug info
07/18/14   ad      Made changes to reduce F3s
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
09/16/13    mp     Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/13/13    gv     Added support for FEATURE_TRIPLE_SIM
08/13/13    gv     Made changes to pass AS ID in RRC_ETWS_PRIM_NOTIFICATION_IND
01/11/12    sks    Fixed compilation warning.
01/11/12    sn     Changes to send ETWS security info to upper layers
06/29/11    rl     Fix for handling the failure of decoding ETWS OTA msg
05/10/11    rl     Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/04/11    su     Added code to provide Cell broadcast info the CM.
01/28/11    rl     Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11    su     Fixed compilation warnings.
01/12/11    ad     Added changes for RRC SW decoupling
01/05/11    su     Added F3 logs for FEATURE_WCDMA_ETWS.
01/04/11    rl     Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/16/10    sb     Compiler warning fix
12/31/10    rl     Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/22/10    su     Made code changes to support FEATURE_WCDMA_ETWS. 
10/30/10    rl     Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
03/12/10    rm     Added code to support FEATURE_DUAL_SIM
12/15/09    rm     Added code to not use bmc_schedul_msg_period field when 
                   FEATURE_UMTS_BMC_ENH_SCHEDULE is not defined
12/01/09    dm     Added code to send Schedule Period Offset to BMC as part of 
                   FEATURE_UMTS_BMC_ENH_SCHEDULE.
05/08/09    ss      Updated Copyright Information
15/07/05    da     Moved func rrcbmc_get_lac_id() to rrcdata.c.  Changed name
                   to rrc_get_lac_id()
05/05/05    da     Added fix to send BMC RRC_CELL_CHANGE_IND in URA_PCH state.
04/13/05    da     Putting #includes outside FEATURE UMTS BMC to take care
                   of compiler warning when feature is undef 
04/05/06    da     Added Initial Support for BMC functionality.  New Fuctions 
                   are: rrcbmc_send_rrc_new_cell_ind_message() , 
                   rrcbmc_get_lac_id(), rrcbmc_procedure_event_handler(),
                   rrcbmc_init_procedure().

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

#include "wcdma_variation.h"
#include "rrcbmc.h"
#include "rrcbmcif.h"
#include "rrclcm.h"
#include "mm_umts.h"
#include "rrcccm.h"
#include "rrcsibdb.h"
#include "rrcscmgr.h"
#include "rrcdata_v.h"
#ifdef FEATURE_CM_CELL_BROADCAST 
#include "sys.h"
#include "cmwll.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif

#include "uecomdef.h"
#include "ms.h"
#include "gs.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/* Flag used when OOS is detected and send indication to BMC indicating CTCH
 *  not supported 
 */
LOCAL boolean oos_indicator = FALSE;                
#ifdef FEATURE_TRIPLE_SIM
LOCAL boolean indicate_bmc_on_wcdma_stop = FALSE;
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


extern boolean rrcllc_get_ctch_enabled_status(void);
extern uint32 rrcllc_get_ctch_rlc_size(void);
extern void rrcllc_get_ctch_drx_info(uint8 *frame_offset, uint16 *alloc_period);
extern void rrcllc_get_bmc_schedul_info(uint16 *schedul_msg_period);




#ifdef FEATURE_CM_CELL_BROADCAST
/*===========================================================================

FUNCTION rrcbmc_send_cb_info_to_cm

DESCRIPTION
  This function sends CM_RRC_CELL_BROADCAST_INFO_IND primitive to CM 
  This sends the cb_capability info to CM.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
static void rrcbmc_send_cb_info_to_cm(boolean ctch_is_supported)
{

  cm_rpt_type *cmd_ptr;

  /* get a CM command buffer */
  if ((cmd_ptr = cm_get_cmd_buf()) != NULL)
  {
    /* format the primitive and send to the destination task */

    cmd_ptr->hdr.cmd = CM_RRC_CELL_BROADCAST_INFO_IND;

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->asubs_id = rrc_get_as_id();
    cmd_ptr->cmd.cell_bc_info.subscription_id = rrc_get_as_id();
#endif

    if(ctch_is_supported == FALSE)
    {
      cmd_ptr->cmd.cell_bc_info.cell_bc_capability = SYS_CELL_BROADCAST_CAPABILITY_OFF;
    }
    else
    {
      cmd_ptr->cmd.cell_bc_info.cell_bc_capability = SYS_CELL_BROADCAST_CAPABILITY_ON;
    }
    cm_put_cmd(cmd_ptr);
  
    WRRC_MSG1_HIGH("Tx CM_RRC_CB_INFO_IND to CM %d ",ctch_is_supported);
  
  }
  else
  {
    WRRC_MSG0_ERROR("Unable to send CB info to CM");
  }
}
#endif  

/*===========================================================================

FUNCTION rrcbmc_send_new_cell_ind

DESCRIPTION
  This function sends RRC_NEW_CELL_IND primitive to BMC 

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcbmc_send_rrc_new_cell_ind_message(void) 
{
  
  rrcbmc_cell_change_ind_type rrcbmc_cell_change_ind; 
  boolean ctch_present = FALSE;
  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  rrc_state_e_type curr_state;

  gs_status_T  gs_status;

  memset(&(rrcbmc_cell_change_ind.rrcbmc_ctch_info),0,sizeof(rrcbmc_ctch_info_type));

#ifdef FEATURE_DUAL_SIM
  rrcbmc_cell_change_ind.as_id = rrc_get_as_id();
#endif

  rrcbmc_cell_change_ind.message_header.message_set = MS_RRC_BMC;
  rrcbmc_cell_change_ind.message_header.message_id =  RRC_CELL_CHANGE_IND;
  PUT_IMH_LEN((sizeof(rrcbmc_cell_change_ind) - sizeof(IMH_T)),&((rrcbmc_cell_change_ind).message_header));
  
  curr_state = rrc_get_state();

  if ( !(oos_indicator)  && (RRC_CELL_LOSS_NO_NEW_CELL != rrc_ccm_get_serv_plmn_cellid(&rrcbmc_cell_change_ind.cell_id,
                                                                                       &rrcbmc_cell_change_ind.plmn_id))
#ifdef FEATURE_TRIPLE_SIM
        && !(indicate_bmc_on_wcdma_stop)
#endif
     )
  {
    ctch_present = rrcllc_get_ctch_enabled_status();
    if (ctch_present && (curr_state == RRC_STATE_DISCONNECTED || curr_state == RRC_STATE_CELL_PCH
                         || curr_state == RRC_STATE_URA_PCH  ))
    {
      WRRC_MSG0_HIGH("Sending RRC_CELL_CHANGE_IND with CTCH supported");
      

      rrcbmc_cell_change_ind.ctch_is_supported = TRUE;

      if (SUCCESS == rrc_get_lac_id(&lai))
      {
        rrcbmc_cell_change_ind.lac[0] = lai.lac[0];
        rrcbmc_cell_change_ind.lac[1] = lai.lac[1];
        WRRC_MSG2_HIGH("LAC[0] %d  LAC[1] %d",rrcbmc_cell_change_ind.lac[0],rrcbmc_cell_change_ind.lac[1]);
      }
      else
      {
        rrcbmc_cell_change_ind.lac[0] = 0;
        rrcbmc_cell_change_ind.lac[1] = 0;
      }

      rrcllc_get_ctch_drx_info(&rrcbmc_cell_change_ind.rrcbmc_ctch_info.cbs_frame_offset,
                               &rrcbmc_cell_change_ind.rrcbmc_ctch_info.ctch_allocation_period);

      rrcbmc_cell_change_ind.rrcbmc_ctch_info.rlc_lc_id = rrclcm_check_dl_rlc_lc_id(UE_LOGCHAN_CTCH,
                                                                                    CTCH_RADIO_BEARER_ID,
                                                                                    UE_MODE_UNACKNOWLEDGED);

      rrcbmc_cell_change_ind.rrcbmc_ctch_info.rlc_pdu_size = (uint16) rrcllc_get_ctch_rlc_size();
      /* ctch_rf_per_tti is not used yet... so just a default value for now */
      rrcbmc_cell_change_ind.rrcbmc_ctch_info.ctch_rf_per_tti = 1;

      rrcllc_get_bmc_schedul_info(&rrcbmc_cell_change_ind.rrcbmc_ctch_info.bmc_schedul_msg_period);

      MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CBS DRX info N = %d K = %d RLC size = %d RCL lc_id = %d Current cell id is %d",
      rrcbmc_cell_change_ind.rrcbmc_ctch_info.ctch_allocation_period, rrcbmc_cell_change_ind.rrcbmc_ctch_info.cbs_frame_offset,
      rrcbmc_cell_change_ind.rrcbmc_ctch_info.rlc_pdu_size, rrcbmc_cell_change_ind.rrcbmc_ctch_info.rlc_lc_id,
      rrcbmc_cell_change_ind.cell_id
      );

    } // if (ctch_present)
    else
    {
      WRRC_MSG0_HIGH("Sending RRC_CELL_CHANGE_IND with CTCH not supported");
      rrcbmc_cell_change_ind.cell_id = 0;
      rrcbmc_cell_change_ind.ctch_is_supported = FALSE;      
    } // else (ctch is not present)
  }
  else
  {
    /* reset OOS indicator */
    oos_indicator = FALSE;
    /* Cell loss has occured.  Set Cell_id to 0 & ctch_supported flag to false */
    WRRC_MSG0_HIGH("Sending RRC_CELL_CHANGE_IND with CTCH not supported");
    rrcbmc_cell_change_ind.cell_id = 0;
    rrcbmc_cell_change_ind.ctch_is_supported = FALSE;
  }

#ifdef FEATURE_CM_CELL_BROADCAST
  rrcbmc_send_cb_info_to_cm(rrcbmc_cell_change_ind.ctch_is_supported);
#endif

  /* All inforamtion necessary have been filled out.  Send cell_change_ind to BMC */
  WRRC_MSG0_HIGH("Sending RRC_CELL_CHANGE_IND to BMC");
  gs_status = gs_send_message(GS_QUEUE_CB, (void*)(&rrcbmc_cell_change_ind), TRUE);
  if (gs_status != GS_SUCCESS)
  {
    WRRC_MSG1_ERROR("Unable to send msg to GS_QUEUE_CB.  %d",gs_status);
  }

}


#ifdef FEATURE_WCDMA_ETWS

/*============================================================================
FUNCTION: rrcbmc_send_etws_information_ind_message

DESCRIPTION:
This function is responsible for filling up data structures
for etws information sent  to BMC .
This also allocates cmd buffer and sends the command.


DEPENDENCIES:
  None.

RETURN VALUE:
  None.


SIDE EFFECTS:
=============================================================================*/


void rrcbmc_send_etws_information_ind_message
(
  rrc_ETWS_Information etws_information,
  rrc_ETWS_WarningSecurityInfo* etws_WarningSecurityInfo
)
{

  uint8 i = 0;
  byte *data_ptr = NULL;
  
  rrc_etws_prim_notification_ind_type rrc_etws_prim_notification_ind;

  gs_status_T  gs_status;
    
  rrc_etws_prim_notification_ind.msg_hdr.message_set = MS_RRC_BMC;
  rrc_etws_prim_notification_ind.msg_hdr.message_id = RRC_ETWS_PRIM_NOTIFICATION_IND;

  PUT_IMH_LEN((sizeof(rrc_etws_prim_notification_ind) - sizeof(IMH_T)),&((rrc_etws_prim_notification_ind).msg_hdr));

  data_ptr = etws_information.warningType.data;
 
  for( i = 0; i < etws_information.warningType.numocts; i++)
  {
    rrc_etws_prim_notification_ind.warning_type[i] = *data_ptr;
    WRRC_MSG1_HIGH("ETWS : ETWS Information warning type is 0x %x",*data_ptr);
    data_ptr++;
  }
  
  data_ptr = etws_information.messageIdentifier.data;
  rrc_etws_prim_notification_ind.msg_id[0] = *data_ptr;
  rrc_etws_prim_notification_ind.msg_id[1] = *(data_ptr+1);

  WRRC_MSG2_HIGH("ETWS : ETWS Information message identifier is 0x %x%x",*data_ptr,*(data_ptr+1));

  data_ptr = etws_information.serialNumber.data;
  rrc_etws_prim_notification_ind.sr_num[0] = *data_ptr;
  rrc_etws_prim_notification_ind.sr_num[1] = *(data_ptr+1);

  WRRC_MSG2_HIGH("ETWS : ETWS Information serial number is 0x %x%x",*data_ptr,*(data_ptr+1));

  if(etws_WarningSecurityInfo)
  {
    rrc_etws_prim_notification_ind.warning_sec_info_present = TRUE;
    for( i = 0; ((i < etws_WarningSecurityInfo->numocts)&&(i<RRC_ETWS_WARN_SEC_INFO_LEN)); i++)
    {
      rrc_etws_prim_notification_ind.warning_sec_info[i] = etws_WarningSecurityInfo->data[i];
    }
  }
  else
  {
    rrc_etws_prim_notification_ind.warning_sec_info_present = FALSE;
  }
  
  rrc_etws_prim_notification_ind.plmn_id = rrc_ccm_get_curr_camped_plmn();

#ifdef FEATURE_DUAL_SIM
  rrc_etws_prim_notification_ind.as_id = rrc_get_as_id();
#endif

  PRINT_PLMN_ID(rrc_etws_prim_notification_ind.plmn_id);

/* All inforamtion necessary have been filled out.  Send cell_change_ind to BMC */
  WRRC_MSG1_HIGH("Sending RRC_ETWS_PRIM_NOTIFICATION_IND to BMC warning_sec_info_present is  %d ",
  rrc_etws_prim_notification_ind.warning_sec_info_present);

  gs_status = gs_send_message(GS_QUEUE_CB, (void*)(&rrc_etws_prim_notification_ind), TRUE);
  
  if (gs_status != GS_SUCCESS)
  {
    WRRC_MSG1_ERROR("Unable to send msg to GS_QUEUE_CB.  %d",gs_status);
  }

}


#endif


/*===========================================================================

FUNCTION rrcbmc_procedure_event_handler

DESCRIPTION
  This function is the entry point of rrcbmc 

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcbmc_procedure_event_handler
( 
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)  
{ 
#ifdef FEATURE_WCDMA_ETWS
  rrc_DL_DCCH_Message *dcch_msg_ptr;      /* Pointer to the DCCH message */
  rrc_ETWS_WarningSecurityInfo *warning_sec_info=NULL;
#endif
  
  WRRC_MSG0_HIGH_OPT("function rrcbmc_procedure_event_handler is called");
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_BMC,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);
  
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
#ifdef FEATURE_WCDMA_ETWS
    case RRC_DOWNLINK_SDU_IND:
      dcch_msg_ptr=(rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
      if(dcch_msg_ptr != NULL)
      {
        if(dcch_msg_ptr->message.u.etwsPrimaryNotificationWithSecurity.m.dummyPresent)
        {
          warning_sec_info = &dcch_msg_ptr->message.u.etwsPrimaryNotificationWithSecurity.dummy;
        }
        rrcbmc_send_etws_information_ind_message(dcch_msg_ptr->message.u.etwsPrimaryNotificationWithSecurity.
          etws_Information,warning_sec_info);
      }
      else
      {
        WRRC_MSG0_HIGH("DL_DCCH: ETWS ASN1 Decoding Failed");
      }
      break;
#endif
    case RRC_NEW_CELL_IND:  
      rrcbmc_send_rrc_new_cell_ind_message();
      break;

    case RRC_STATE_CHANGE_IND:
      rrcbmc_send_rrc_new_cell_ind_message();
      break;

    case RRC_CPHY_OUT_OF_SERVICE_AREA_IND:
      oos_indicator = TRUE;
      rrcbmc_send_rrc_new_cell_ind_message();
      break;

    default:     
      WRRC_MSG1_HIGH("Invalid Event: %d received", cmd_ptr->cmd_hdr.cmd_id);
      break;
  }
}




/*===========================================================================

FUNCTION RRCBMC_INIT_PROCEDURE

DESCRIPTION
  This function will register for the necessary state change indication for
  bmc procedure, and SIB 5/6 change notification

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcbmc_init_procedure
(    
  void  
)
{

  rrcscmgr_register_for_scn(RRC_PROCEDURE_BMC,
                            RRC_STATE_DISCONNECTED,
                            RRC_STATE_CONNECTING);
  rrcscmgr_register_for_scn(RRC_PROCEDURE_BMC,
                            RRC_STATE_CELL_PCH,
                            RRC_STATE_CELL_FACH);
  rrcscmgr_register_for_scn(RRC_PROCEDURE_BMC,
                            RRC_STATE_URA_PCH,
                            RRC_STATE_CELL_FACH);
  rrcscmgr_register_for_scn(RRC_PROCEDURE_BMC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_CELL_PCH);
  rrcscmgr_register_for_scn(RRC_PROCEDURE_BMC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_URA_PCH);
}

#ifdef FEATURE_TRIPLE_SIM
/*===========================================================================

FUNCTION rrcbmc_send_cell_change_on_wcdma_stop

DESCRIPTION
This function sends BMC not supported indication during WCDMA stop

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcbmc_send_cell_change_on_wcdma_stop
(    
  void  
)
{
  indicate_bmc_on_wcdma_stop = TRUE;
  WRRC_MSG0_HIGH("TSTS: Sending cell change ind to BMC on RRC Stop");
  rrcbmc_send_rrc_new_cell_ind_message();
  indicate_bmc_on_wcdma_stop = FALSE;
}

#endif

