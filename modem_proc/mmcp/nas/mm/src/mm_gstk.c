/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002, 2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_gstk.c#1 $ $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
3/30/11    gps     Defeturizing gstk_get_curr_cell_id_info.
01/06/03   cd      Initial version
01/28/03   cd      Added version control tracking
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
02/17/04   cd      Changed ERR_FATAL to ERR if attempt to allocate memory fails
05/26/05   abe     Added functions mm_gstk_is_location_status_evt_reqd & 
                   mm_gstk_set_location_status_evt.
10/11/05   vdr     o Now blocking NO SRV indication when G or W RATs are going OOS
                   o Added mm_send_gstk_no_service_info() which is called when UE goes 
                     into deep sleep
11/28/05   abe     Removed lint errors. 
08/08/11   pm      Corrected F3 prints.
11/29/11   sla     Added F3 print while sending the IDLE event with no service
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "msg.h"
#include "err.h"
#include "mm_v.h"
#include "mm_coord.h"
#include <stringl/stringl.h>
#include "sys_cnst.h"
#include "emm_utility.h"
#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstk_exp_v.h"
#include "mm_gstk.h"
#include "gmm_int_v.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern nas_mmgsdi_info_type mm_sim_mmgsdi_info_sim[MAX_NAS_STACKS];
#define mm_sim_mmgsdi_info mm_sim_mmgsdi_info_sim[mm_as_id]

#else 
extern nas_mmgsdi_info_type mm_sim_mmgsdi_info;

#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

boolean mm_gstk_send_location_status_sim[MAX_NAS_STACKS];

#define  mm_gstk_send_location_status mm_gstk_send_location_status_sim[mm_as_id]
extern sys_modem_stack_id_e_type mm_set_cm_stack_id(mm_as_id_e_type as_id);
#else
boolean mm_gstk_send_location_status;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

extern boolean reg_nv_is_imsi_switch_enabled(void);


/*===========================================================================

FUNCTION MM_GET_MCC_MNC_LAC

DESCRIPTION
  This function gets the serving LAI information

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if the information is current
  FALSE, if the information is not valid, or in the process of being updated

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_get_mcc_mnc_lac(uint8 *mcc_mnc, uint8 *lac)
{
  /* check to see if MM is in the middle of a PLMN search */
  if (mm_idle_substate == MM_NORMAL_SERVICE || mm_idle_substate == MM_LIMITED_SERVICE || mm_idle_substate == MM_NO_IMSI)
  {
      memscpy(mcc_mnc,PLMN_SIZE, mm_serving_plmn.info.plmn.identity, PLMN_SIZE);
      memscpy(lac,LAC_SIZE, mm_serving_plmn.lac.lac, LAC_SIZE);

      MSG_HIGH_DS_1(MM_SUB ,"=MM= GSTK query mm idle info in sub-state %d",
          mm_idle_substate);

  }
  else
  {
      /* For all other substates return default info */
      memset(mcc_mnc, 0xff, PLMN_SIZE);
      memset(lac, 0xff, LAC_SIZE);
      lac[1] = 0xFE ;
      MSG_HIGH_DS_0(MM_SUB, "=MM= Location info is unavailable") ;
  }

  return TRUE ;
}


void mm_send_gstk_no_service_info(void)
{

  gstk_io_send_mm_idle_evt_type  mm_idle_evt;
  uint8                          dummy;
#ifdef FEATURE_LTE
  boolean is_geo_det_mode =     FALSE;


  if((mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_set == MS_MM_REG) && 
     (mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_id == MMR_REG_REQ) &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)&& 
            emm_bit_mask_check(mmcoord_pending_reg_message.cmd.mmr_reg_req.additional_info,
             (byte)SYS_ADDTNL_INFO_GEO_DETERMINATION_PHASE)
     )
  {
    is_geo_det_mode = TRUE;
    MSG_HIGH_DS_1(MM_SUB,"=MM= additional info says geo determination = %d", emm_bit_mask_check(mmcoord_pending_reg_message.cmd.mmr_reg_req.additional_info,
                                                                                                                  (byte)SYS_ADDTNL_INFO_GEO_DETERMINATION_PHASE));
  
  }
  MSG_HIGH_DS_3(MM_SUB,"=MM= MMSS_GEO_SEARCH = %d, net_sel_mode = %d, mm_follow_on_request = %d",
                                                                           is_geo_det_mode, 
                                                                           mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode, 
                                                                           mm_follow_on_request); 

  if((is_geo_det_mode == TRUE) && (mm_follow_on_request == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= not sending no service event as UE is GEO determination mode");
    return;
  }
#endif
  memscpy(mm_gstk_previous_serving_lai, LAI_SIZE, mm_serving_plmn.info.plmn.identity, PLMN_SIZE);
      memscpy(&(mm_gstk_previous_serving_lai[PLMN_SIZE]),LAI_SIZE-PLMN_SIZE, mm_serving_plmn.lac.lac, LAC_SIZE);

    /* Init */
  memset((void *)&mm_idle_evt, 0x0, sizeof(gstk_io_send_mm_idle_evt_type));

  /* fill in the cmd information */
  mm_idle_evt.location_state =(uint8)MM_AS_NO_SERVICE;
  mm_idle_evt.rat = (uint8)mm_serving_plmn.info.active_rat;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_idle_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  mm_idle_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  mm_idle_evt.as_id = 0;
#endif
  /*Send MM_IDLE info to GSTK*/  
  if(gstk_io_ctrl(GSTK_IO_SEND_MM_IDLE_EVENT, &mm_idle_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS( MM_SUB," =MM= could not send IDLE_INFO to GSTK",0, 0, 0);
  }
  else
  {
    MSG_HIGH_DS_2( MM_SUB, "=MM= MM sent IDLE info to GSTK - service status is %d and rat is %d ",mm_serving_plmn.info.service_status, mm_serving_plmn.info.active_rat);
  }

}
/*===========================================================================

FUNCTION MM_SEND_GSTK_MM_IDLE_INFO

DESCRIPTION
  This function sends tha LAI, location status and RAT information
  to the GSTK whenever MM enters the MM idle state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_gstk_mm_idle_info()
{
  gstk_io_send_mm_idle_evt_type  mm_idle_evt;
  uint8                          dummy;
  gstk_cell_id_type   cell_id;

  boolean is_full_rat_mode =     FALSE;
  if (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_NO_SRV )
  {
    /* Block all NO Service Indications to be replaced when in
    ** Deep Sleep Mode No service indication
    */
    return;
  }

  if (!((mm_gstk_is_location_status_evt_reqd()) || ((!reg_nv_is_imsi_switch_enabled()) && (mm_idle_substate == MM_LIMITED_SERVICE))))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM=  Location Status Event Not to be sent yet !!");
    return;
  }
  if((mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_set == MS_MM_REG) && 
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_id == MMR_REG_REQ) &&
      ((mmcoord_pending_reg_message.cmd.mmr_reg_req.type == MMR_REG_REQ_FULL_RAT_MODE)
#ifdef FEATURE_LTE
       ||
       ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)&& 
         emm_bit_mask_check(mmcoord_pending_reg_message.cmd.mmr_reg_req.additional_info,
                                (byte)SYS_ADDTNL_INFO_GEO_DETERMINATION_PHASE)
         )
#endif
       ))
  {
     is_full_rat_mode = TRUE;
#ifdef FEATURE_LTE
     MSG_HIGH_DS_1(MM_SUB,"=MM= additional info says geo determination = %d", emm_bit_mask_check(mmcoord_pending_reg_message.cmd.mmr_reg_req.additional_info,
                                                                              (byte)SYS_ADDTNL_INFO_GEO_DETERMINATION_PHASE));
#endif
  }
  
  MSG_HIGH_DS_3(MM_SUB,"=MM= is_full_rat_mode/MMSS_GEO_SEARCH = %d, net_sel_mode = %d, mm_follow_on_request = %d",
                                                                                    is_full_rat_mode, 
                                                                                    mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode, 
                                                                                    mm_follow_on_request); 

 if((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY) ||
      ((is_full_rat_mode == TRUE) && (mm_follow_on_request == FALSE))) /* For FULL RAT mode, don't send IDLE event unless there is an EM call */
  {
      MSG_HIGH_DS_0(MM_SUB,"=MM= returning from mm_send_gstk_mm_idle_info");
      return;
  }

#ifdef FEATURE_DUAL_SIM
  if ((mm_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_PRI_SESSION) &&
      (mm_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_SEC_SESSION) 
#ifdef FEATURE_TRIPLE_SIM      
      &&(mm_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_TER_SESSION)
#endif
     )
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Not sending GSTK evt - session type invalid:%d !!", mm_sim_mmgsdi_info.session_type);
    return;
  }
#endif

  /* See if Cell ID is valid */
  if(gstk_get_curr_cell_id_info((gstk_sys_radio_access_tech_e_type) mm_serving_plmn.info.active_rat,
                             mm_serving_plmn.info.plmn.identity,
                             mm_serving_plmn.lac.lac,
                             &cell_id
                             ,mm_sim_mmgsdi_info.session_type ) == FALSE)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Unable to get Cell ID/Rat: 0x%x", mm_serving_plmn.info.active_rat, 0, 0);
    return;
  }


  /* Init */
  memset((void *)&mm_idle_evt, 0x0, sizeof(gstk_io_send_mm_idle_evt_type));

  /* remember last LAI sent to GSTK */
      memscpy(mm_gstk_previous_serving_lai, LAI_SIZE, mm_serving_plmn.info.plmn.identity, PLMN_SIZE);
      memscpy(&(mm_gstk_previous_serving_lai[PLMN_SIZE]),LAI_SIZE-PLMN_SIZE, mm_serving_plmn.lac.lac, LAC_SIZE);



  /* Fill in the structure values for "gstk_io_send_mm_idle_evt_type"*/  
  mm_idle_evt.location_state = (uint8)mm_serving_plmn.info.service_status;
                         

  mm_idle_evt.rat = (uint8)mm_serving_plmn.info.active_rat;
  memscpy(mm_idle_evt.mcc_mnc, GSTK_MAX_PLMN_LEN, mm_serving_plmn.info.plmn.identity, PLMN_SIZE);
  memscpy(mm_idle_evt.cell_id.cell_id, GSTK_MAX_CELL_ID_LEN, cell_id.cell_id, (uint32) cell_id.cell_len);

  mm_idle_evt.cell_id.cell_len = MIN(GSTK_MAX_CELL_ID_LEN, cell_id.cell_len);

  memscpy(mm_idle_evt.lac, GSTK_MAX_LAC_LEN,mm_serving_plmn.lac.lac, LAC_SIZE);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_idle_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  mm_idle_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  mm_idle_evt.as_id = 0;
#endif
  
  /*Send MM_IDLE info to GSTK*/  
  if(gstk_io_ctrl(GSTK_IO_SEND_MM_IDLE_EVENT, &mm_idle_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= could not send IDLE_INFO to GSTK",0,0,0);
  }
  else
  {
    MSG_HIGH_DS_2( MM_SUB, "=MM= MM sent IDLE info to GSTK - service status is %d and rat is %d ", mm_serving_plmn.info.service_status, mm_serving_plmn.info.active_rat);
  }

}

void mm_gstk_set_location_status_evt (boolean flag)
{
  mm_gstk_send_location_status = flag;
}

boolean mm_gstk_is_location_status_evt_reqd ()
{
  return mm_gstk_send_location_status;
}

#ifdef FEATURE_SGLTE
void mm_send_gstk_mm_event(gstk_io_send_mm_ind_enum_type event, mm_as_id_e_type as_id)
{
  gstk_io_send_mm_ind_param_type mm_ind;
  int dummy = 0;
  
  if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
  {
      return;
  }

  memset(&mm_ind, 0x00, sizeof(gstk_io_send_mm_ind_param_type));

  mm_ind.type =	event;

  mm_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  mm_ind.stack_id = mm_set_cm_stack_id(as_id);
  
  if(gstk_io_ctrl(GSTK_IO_SEND_MM_INDICATION, &mm_ind, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB,"=MM= event %d sending failed",event,0,0);
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= MM event %d sent to GSTK",event);
  }
    
}
#endif
/*===========================================================================

FUNCTION MM_SEND_GSTK_GPRS_ATTACH_REJECT_EVT

DESCRIPTION
  This function sends network reject event to GSTK in case of GPRS
  ATTACH Reject.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_gstk_gprs_attach_reject_evt(byte reject_cause)
{
  gstk_io_send_nw_rej_evt_type nw_rej_evt;
  uint8 dummy;
  int i =0;

  memset((void*)&nw_rej_evt, 0x0, sizeof(gstk_io_send_nw_rej_evt_type));
  nw_rej_evt.nw_rej_type = GSTK_GPRS_ATTACH_REJ;
  //memcpy(nw_rej_evt.mcc_mnc,mm_serving_plmn.info.plmn.identity,sizeof(sys_plmn_id_s_type));
  for(i =0;i<GSTK_MAX_PLMN_LEN;i++)
  {
    nw_rej_evt.mcc_mnc[i] = mm_serving_plmn.info.plmn.identity[i];
  }
  //memcpy(nw_rej_evt.lac,gmm_current_lai.location_area_code,2*sizeof(byte));  
  for(i = 0;i<GSTK_MAX_LAC_LEN;i++)
  {
    nw_rej_evt.lac[i] = gmm_current_lai.location_area_code[i];
  }
  
  nw_rej_evt.rac = (uint8)gmm_current_rac;
  nw_rej_evt.rat = (uint8)mm_serving_plmn.info.active_rat;
  nw_rej_evt.rej_cause = (uint8)reject_cause;
  
  switch(gmm_attach_type)
  {
    case GMM_GPRS_ATTACH:
      nw_rej_evt.upd_attach_type = GSTK_GPRS_ATTACH;
      break;
    case GMM_COMBINED_GPRS_IMSI_ATTACH:
      nw_rej_evt.upd_attach_type = GSTK_COMBINED_GSRS_IMSI_ATTACH;
      break;
    default:
      nw_rej_evt.upd_attach_type = GSTK_UPDATE_ATTACH_TYPE_MAX;
      break;
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  nw_rej_evt.as_id = (sys_modem_as_id_e_type)mm_as_id;
#else
  nw_rej_evt.as_id = 0;
#endif

#ifdef FEATURE_GSTK  
  if(gstk_io_ctrl(GSTK_IO_SEND_NW_REJ_EVENT, &nw_rej_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB,"=MM= could not send NW_REJECT_EVENT_ATTACH_REJECT to GSTK",0,0,0);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= sent NW_REJECT_EVENT_ATTACH_REJECT to GSTK");
  }
#endif /*FEATURE_GSTK*/

}



/*===========================================================================

FUNCTION MM_SEND_GSTK_ROUTING_AREA_UPDATE_REJECT_EVT

DESCRIPTION
  This function sends network reject event to GSTK in case of Routing Area
  Update Reject.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void mm_send_gstk_routing_area_update_reject_evt(byte reject_cause)
{
  gstk_io_send_nw_rej_evt_type nw_rej_evt;
  uint8 dummy;
  int i =0;

  memset((void*)&nw_rej_evt, 0x0, sizeof(gstk_io_send_nw_rej_evt_type));
  nw_rej_evt.nw_rej_type = GSTK_ROUTING_AREA_UPDATE_REJ;
  
  //memcpy(nw_rej_evt.mcc_mnc,mm_serving_plmn.info.plmn.identity,sizeof(sys_plmn_id_s_type));
  for(i =0;i<GSTK_MAX_PLMN_LEN;i++)
  {
    nw_rej_evt.mcc_mnc[i] = mm_serving_plmn.info.plmn.identity[i];
  }

  //memcpy(nw_rej_evt.lac,gmm_current_lai.location_area_code,2*sizeof(byte));

  for(i = 0;i<GSTK_MAX_LAC_LEN;i++)
  {
    nw_rej_evt.lac[i] = gmm_current_lai.location_area_code[i];
  }
  
  nw_rej_evt.rac = (uint8)gmm_current_rac;
  nw_rej_evt.rat = (uint8)mm_serving_plmn.info.active_rat;
  nw_rej_evt.rej_cause = (uint8)reject_cause;

  switch(gmm_ra_update_type)
  {
    case GMM_RA_UPDATING:
      nw_rej_evt.upd_attach_type = GSTK_ROUTING_AREA_UPDATE;
      break;
    case GMM_COMBINED_RA_LA_UPDATING:
      nw_rej_evt.upd_attach_type = GSTK_COMBINED_RA_LA_UPDATE;  
      break;
    case GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH:
      nw_rej_evt.upd_attach_type = GSTK_COMBINED_RA_LA_UPDATE_IMSI_ATTACH;
      break;
    case GMM_PERIODIC_UPDATING:
      nw_rej_evt.upd_attach_type = GSTK_PERIODIC_RA_UPDATE;
      break;
    default:
      nw_rej_evt.upd_attach_type = GSTK_UPDATE_ATTACH_TYPE_MAX;
      break;
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  nw_rej_evt.as_id = (sys_modem_as_id_e_type)mm_as_id;
#else
  nw_rej_evt.as_id = 0;
#endif
#ifdef FEATURE_GSTK
  if(gstk_io_ctrl(GSTK_IO_SEND_NW_REJ_EVENT, &nw_rej_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB,"=MM= could not send NW_REJECT_EVENT_RAU_REJECT to GSTK",0,0,0);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= sent NW_REJECT_EVENT_RAU_REJECT to GSTK");
  }
#endif /*FEATURE_GSTK*/
  

}



/*===========================================================================

FUNCTION MM_SEND_GSTK_LOCATION_UPDATING_REJECT_EVT

DESCRIPTION
  This function sends network reject event to GSTK in case of Location Area
  Update Reject.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void mm_send_gstk_location_updating_reject_evt(byte reject_cause, byte lu_cause)
{
  gstk_io_send_nw_rej_evt_type nw_rej_evt;
  uint8 dummy;
  int i = 0;

  memset((void*)&nw_rej_evt, 0x0, sizeof(gstk_io_send_nw_rej_evt_type));
  nw_rej_evt.nw_rej_type = GSTK_LOCATION_UPDATE_REJ;

  //memcpy(nw_rej_evt.lac,&mm_serving_lai[3],2*sizeof(byte));
  for(i = 0;i<GSTK_MAX_LAC_LEN;i++)
  {
    nw_rej_evt.lac[i] = mm_serving_lai[i+3];
  }
  
  //memcpy(nw_rej_evt.mcc_mnc,mm_serving_plmn.info.plmn.identity,sizeof(sys_plmn_id_s_type));
  for(i =0;i<GSTK_MAX_PLMN_LEN;i++)
  {
    nw_rej_evt.mcc_mnc[i] = mm_serving_plmn.info.plmn.identity[i];
  }
  

  nw_rej_evt.rat = (uint8)mm_serving_plmn.info.active_rat;
  nw_rej_evt.rej_cause = (uint8)reject_cause;

  switch(lu_cause)
  {
    case MM_LU_CAUSE_NORMAL:
      nw_rej_evt.upd_attach_type = GSTK_NORMAL_LOC_UPDATE;
      break;
    case MM_LU_CAUSE_PERIODIC:
      nw_rej_evt.upd_attach_type = GSTK_PERIODIC_LOC_UPDATE;  
      break;
    case MM_LU_CAUSE_IMSI_ATTACH:
      nw_rej_evt.upd_attach_type = GSTK_IMSI_ATTACH;
      break;
    default:
      nw_rej_evt.upd_attach_type = GSTK_UPDATE_ATTACH_TYPE_MAX;
      break;
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  nw_rej_evt.as_id = (sys_modem_as_id_e_type)mm_as_id;
#else
  nw_rej_evt.as_id = 0;
#endif
#ifdef FEATURE_GSTK
  if(gstk_io_ctrl(GSTK_IO_SEND_NW_REJ_EVENT, &nw_rej_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB,"=MM= could not send NW_REJECT_EVENT_LAU_REJECT to GSTK",0,0,0);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= sent NW_REJECT_EVENT_LAU_REJECT to GSTK");
  }
#endif /*FEATURE_GSTK*/

}

#endif /* FEATURE_GSTK */
