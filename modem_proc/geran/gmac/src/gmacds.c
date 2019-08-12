/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC Data Space
***
***
*** DESCRIPTION
***
***  Contains utility functions and definitions for data spacing (multi
***  SIM operation)
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacds.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/09/12   SB      Created
***
*****************************************************************************/



/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "gmacds.h"
#include "msg.h"
#include "rex.h"
#include "geran_tasks.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_get_current_gas_id()
===
===  DESCRIPTION
===  Returns the current active gas_id.
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    gas_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gas_id_t gmac_get_current_gas_id(void)
{
  gas_id_t      gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;
  rex_tcb_type *curr_tcb_ptr = rex_self();
  rex_tcb_type *mac_task1_tcb_ptr = geran_tcb_read(GERAN_TASK_ID_GMAC);
#ifdef FEATURE_DUAL_SIM
  rex_tcb_type *mac_task2_tcb_ptr = geran_tcb_read(GERAN_TASK_ID_GMAC2);
#endif /*FEATURE_DUAL_SIM*/

#ifdef FEATURE_TRIPLE_SIM 
  rex_tcb_type *mac_task3_tcb_ptr = geran_tcb_read(GERAN_TASK_ID_GMAC3);
#endif /*FEATURE_TRIPLE_SIM*/

  if (curr_tcb_ptr == mac_task1_tcb_ptr)
  {
    gas_id = gmac_map_task_id_to_gas_id(GERAN_TASK_ID_GMAC);
  }
#ifdef FEATURE_DUAL_SIM
  else if(curr_tcb_ptr == mac_task2_tcb_ptr)
  {
    gas_id = gmac_map_task_id_to_gas_id(GERAN_TASK_ID_GMAC2);
  }
#endif /*FEATURE_DUAL_SIM*/


#ifdef FEATURE_TRIPLE_SIM
  else if(curr_tcb_ptr == mac_task3_tcb_ptr)
  {
    gas_id = gmac_map_task_id_to_gas_id(GERAN_TASK_ID_GMAC3);
  }
#endif /*FEATURE_TRIPLE_SIM*/

  else
  {

#ifdef FEATURE_TRIPLE_SIM
    MSG_GERAN_ERROR_3_G("GMAC gas_id set to UNDEFINED, curr_tcb:0x%x, task1_tcb:0x%x, task2_tcb:0x%x",curr_tcb_ptr,
                                                                                                      mac_task1_tcb_ptr,
                                                                                                      mac_task2_tcb_ptr);

    MSG_GERAN_ERROR_1_G("GMAC gas_id set to UNDEFINED, task3_tcb:0x%x",mac_task3_tcb_ptr);

 
#elif defined (FEATURE_DUAL_SIM)
    MSG_GERAN_ERROR_3_G("GMAC gas_id set to UNDEFINED, curr_tcb:0x%x, task1_tcb:0x%x, task2_tcb:0x%x",curr_tcb_ptr,
                                                                                                      mac_task1_tcb_ptr,
                                                                                                      mac_task2_tcb_ptr);
#else 

    MSG_GERAN_ERROR_2_G("GMAC gas_id set to UNDEFINED, curr_tcb:0x%x, task1_tcb:0x%x",curr_tcb_ptr,mac_task1_tcb_ptr);
#endif /*FEATURE_DUAL_SIM*/


  }

  return gas_id;
}

/*===========================================================================
===
===  FUNCTION    gmac_map_task_id_to_gas_id()
===
===  DESCRIPTION
===  Mapping of TASK_ID to GAS_ID
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    task_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gas_id_t gmac_map_task_id_to_gas_id(geran_task_id_t task_id)
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  if (task_id == GERAN_TASK_ID_GMAC)
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_1;
  }
#ifdef FEATURE_DUAL_SIM
  else if (task_id == GERAN_TASK_ID_GMAC2)
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_2;
  }
#endif /*FEATURE_DUAL_SIM*/


#ifdef FEATURE_TRIPLE_SIM
  else if (task_id == GERAN_TASK_ID_GMAC3)
  {
    gas_id = GERAN_ACCESS_STRATUM_ID_3;
  }
#endif /*FEATURE_TRIPLE_SIM*/
  else
  {
    MSG_GERAN_ERROR_1_G("Not a MAC task id:%d, default to GAS_ID_1",task_id);
  }
  return gas_id;
}

/*===========================================================================
===
===  FUNCTION    gmac_map_gas_id_to_task_id()
===
===  DESCRIPTION
===  Mapping of GAS_ID to TASK_ID
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    task_id
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
geran_task_id_t gmac_map_gas_id_to_task_id( gas_id_t gas_id)
{
  geran_task_id_t task_id = GERAN_TASK_ID_GMAC;

  if (gas_id == GERAN_ACCESS_STRATUM_ID_1)
  {
    task_id = GERAN_TASK_ID_GMAC;
  }
#ifdef FEATURE_DUAL_SIM
  else if (gas_id == GERAN_ACCESS_STRATUM_ID_2)
  {
    task_id = GERAN_TASK_ID_GMAC2;
  }
#endif /*FEATURE_DUAL_SIM*/


#ifdef FEATURE_TRIPLE_SIM
  else if (gas_id == GERAN_ACCESS_STRATUM_ID_3)
  {
    task_id = GERAN_TASK_ID_GMAC3;
  }
#endif /*FEATURE_TRIPLE_SIM*/
  else
  {
    MSG_GERAN_ERROR_0_G("Not a valid gas_id:%d, default to GERAN_TASK_ID_GMAC");
  }
  return task_id;
}

/*===========================================================================
===
===  FUNCTION    gmac_map_gas_id_to_layer1_queue()
===
===  DESCRIPTION
===  Mapping of GAS_ID to GS L1 Queue
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    gs_queue_id_T
=== 
===  SIDE EFFECTS
===    none
===
===
===========================================================================*/
gs_queue_id_T gmac_map_gas_id_to_layer1_queue( gas_id_t gas_id)
{
  gs_queue_id_T gs_l1_queue_id = GS_QUEUE_LAYER_1;

  if (gas_id == GERAN_ACCESS_STRATUM_ID_1)
  {
    gs_l1_queue_id = GS_QUEUE_LAYER_1;
  }
#ifdef FEATURE_DUAL_SIM
  else if (gas_id == GERAN_ACCESS_STRATUM_ID_2)
  {
    gs_l1_queue_id = GS_QUEUE_LAYER_1_2;
  }
#endif /*FEATURE_DUAL_SIM*/

#ifdef FEATURE_TRIPLE_SIM
  else if (gas_id == GERAN_ACCESS_STRATUM_ID_3)
  {
    gs_l1_queue_id = GS_QUEUE_LAYER_1_3;
  }
#endif /*FEATURE_TRIPLE_SIM*/

  else
  {
    MSG_GERAN_ERROR_0_G("Not a valid gas_id:%d, default to GS_QUEUE_LAYER_1");
  }
  return gs_l1_queue_id;
}


/*===========================================================================
===
===  FUNCTION      gmac_grr_mac_msg_insert_gas_id
===
===  DESCRIPTION
===  Inserts the current active gas_id into a message.
===
===  INPUT
===  Pointer to GRR message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_grr_mac_msg_insert_gas_id(mac_grr_sig_t *grr_msg_ptr,
                                    gas_id_t       gas_id)
{
  /* Format IMH header */
  switch (grr_msg_ptr->mac_grr_cell_resel_status.message_header.message_id)
  {
    case MAC_GRR_DATA_IND :
      grr_msg_ptr->mac_grr_data_ind.gas_id = gas_id;
      break;

    case MAC_GRR_CELL_RESEL_STATUS:
      grr_msg_ptr->mac_grr_cell_resel_status.gas_id = gas_id;
      break;

    case MAC_GRR_MAC_STATUS:
      grr_msg_ptr->mac_grr_mac_status.gas_id = gas_id;
      break;

    case MAC_GRR_PAGING_NOTIFICATION:
      grr_msg_ptr->mac_grr_paging_notification.gas_id = gas_id;
      break;

    case MAC_GRR_ABNORMAL_RELEASE:
      grr_msg_ptr->mac_grr_abnormal_release.gas_id = gas_id;
      break;

    case MAC_GRR_MEAS_RPT_CNF:
      grr_msg_ptr->mac_grr_meas_rpt_cnf.gas_id = gas_id;
      break;

    case MAC_GRR_UL_TBF_REQ:
      grr_msg_ptr->mac_grr_ul_tbf_req.gas_id = gas_id;
      break;

    case MAC_GRR_STORE_UPDATE:
      grr_msg_ptr->mac_grr_store_update.gas_id = gas_id;
      break;

    case MAC_GRR_PCCF_SENT:
      grr_msg_ptr->mac_grr_pccf_sent.gas_id = gas_id;
      break;

    case MAC_GRR_TBF_REJ:
      grr_msg_ptr->mac_grr_tbf_rej.gas_id = gas_id;
    break;

    case MAC_GRR_TBF_ACCEPT:
      grr_msg_ptr->mac_grr_tbf_accept.gas_id = gas_id;
    break;

    case MAC_GRR_RACH_CNF:
      grr_msg_ptr->mac_grr_rach_cnf.gas_id = gas_id;
    break;

    case MAC_GRR_RACH_REJ:
      grr_msg_ptr->mac_grr_rach_rej.gas_id = gas_id;
    break;

    case MAC_GRR_RESET_CNF:
      grr_msg_ptr->mac_grr_reset_cnf.gas_id = gas_id;
      break;

    case MAC_GRR_NCELL_DATA_IND:
      grr_msg_ptr->mac_grr_ncell_data_ind.gas_id = gas_id;
      break;


    case MAC_GRR_NCELL_DATA_CNF:
      grr_msg_ptr->mac_grr_ncell_data_cnf.gas_id = gas_id;
      break;



#ifdef FEATURE_GSM_DTM
    case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
      grr_msg_ptr->mac_grr_dtm_channel_assign_rej.gas_id = gas_id;
      break;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
     case MAC_GRR_EDTM_PKT_CS_EST_REJ:
       grr_msg_ptr->mac_grr_edtm_pkt_cs_est_rej.gas_id = gas_id;
       break;

     case MAC_GRR_EDTM_CS_RELEASE_IND:
       grr_msg_ptr->mac_grr_edtm_cs_release_ind.gas_id = gas_id;
       break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case MAC_GRR_PSHO_CNF:
       grr_msg_ptr->mac_grr_psho_cnf.gas_id = gas_id;
      break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

    case MAC_GRR_PS_ACCESS_CNF:
      grr_msg_ptr->mac_grr_ps_access_cnf.gas_id = gas_id;
      break;

    default:
      MSG_GERAN_ERROR_2_G( "Unexpected sig rx. idle task id %d sig id %d" ,grr_msg_ptr->mac_grr_ul_tbf_req.message_header.message_set, 
                                                                           grr_msg_ptr->mac_grr_ul_tbf_req.message_header.message_id);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_get_rr_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming RR msg supplied.
===
===  INPUT
===  grr_mac_sig_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_rr_gas_id( grr_mac_sig_t *grr_msg_ptr )
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch(grr_msg_ptr->sig_id)
  {
    case GRR_MAC_SEND_MEAS_RPT:
      gas_id = grr_msg_ptr->msg.gm_send_meas_rpt.gas_id;
    break;

    case GRR_MAC_SEND_PCCF:
      gas_id = grr_msg_ptr->msg.gm_send_pccf.gas_id;
    break;

    case GRR_MAC_DL_TBF_EST:
      gas_id = grr_msg_ptr->msg.gm_dl_tbf_est.gas_id;
    break;

    case GRR_MAC_DL_SB_EST:
      gas_id = grr_msg_ptr->msg.gm_dl_sb_est.gas_id;
    break;

    case GRR_MAC_UL_TBF_EST:
      gas_id = grr_msg_ptr->msg.gm_ul_tbf_est.gas_id;
    break;

    case GRR_MAC_UL_SB_EST:
      gas_id = grr_msg_ptr->msg.gm_ul_sb_est.gas_id;
    break;

    case GRR_MAC_RACH_REQ:
      gas_id = grr_msg_ptr->msg.gm_rach_req.gas_id;
    break;
    
    case GRR_MAC_SEND_PCCN_REQ:
      gas_id = grr_msg_ptr->msg.gm_send_pccn_req.gas_id;
    break;

    case GRR_MAC_NACC_RELEASE_REQ:
      gas_id = grr_msg_ptr->msg.gm_nacc_release_req.gas_id;
    break;

    case GRR_MAC_SEND_PSS_REQ:
      gas_id = grr_msg_ptr->msg.gm_send_pss_req.gas_id;
    break;

    case GRR_MAC_RESET_REQ:
      gas_id = grr_msg_ptr->msg.gm_reset_req.gas_id;
    break;
#ifdef FEATURE_GSM_DTM
    case GRR_MAC_DTM_CHANNEL_ASSIGN_REQ:
      gas_id = grr_msg_ptr->msg.gm_dtm_channel_assign_req.gas_id;
    break;

    case GRR_MAC_DTM_CHANNEL_RELEASE_IND:
      gas_id = grr_msg_ptr->msg.gm_dtm_channel_release_ind.gas_id;
    break;

    case GRR_MAC_DTM_UL_DCCH_IND:
      gas_id = grr_msg_ptr->msg.gm_dtm_ul_dcch_ind.gas_id;
    break;

    case GRR_MAC_DTM_UL_DCCH_CNF:
      gas_id = grr_msg_ptr->msg.gm_dtm_ul_dcch_cnf.gas_id;
    break;

#ifdef FEATURE_GSM_EDTM
    case GRR_MAC_EDTM_PKT_CS_EST_REQ:
      gas_id = grr_msg_ptr->msg.gm_edtm_pkt_cs_est_req.gas_id;
    break;
#endif /*FEATURE_GSM_EDTM*/
#endif /*FEATURE_GSM_DTM*/

    case GRR_MAC_NO_PS_ACCESS_IND:
      gas_id = grr_msg_ptr->msg.gm_no_ps_access_ind.gas_id;
    break;

    case GRR_MAC_PS_ACCESS_REQ:
      gas_id = grr_msg_ptr->msg.gm_ps_access_req.gas_id;
    break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case GRR_MAC_PSHO_REQ:
      gas_id = grr_msg_ptr->msg.gm_psho_req.gas_id;
    break;

    case GRR_MAC_PSHO_COMPLETE_IND:
      gas_id = grr_msg_ptr->msg.gm_psho_complete_ind.gas_id;
    break;
#endif

    default:
      MSG_GERAN_ERROR_1_G("GMAC RR msg id unknown: 0x%x",grr_msg_ptr->sig_id);
    break;
  }

  return gas_id;
}


/*===========================================================================
===
===  FUNCTION      gmac_get_l1_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming L1 msg supplied.
===
===  INPUT
===  l1_mac_sig_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_l1_gas_id(l1_mac_sig_t *l1_msg_ptr )
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch(l1_msg_ptr->sig_id)
  {
    case L1_MAC_FREQ_ASS_CNF:
      gas_id = l1_msg_ptr->msg.tbf_freq_ass_cnf.gas_id;
    break;

    case L1_MAC_USF_DETECTED:
      gas_id = l1_msg_ptr->msg.usf_detected.gas_id;
    break;

    case L1_MAC_QUAL_MEASURMENTS:
      gas_id = l1_msg_ptr->msg.qual_measurements.gas_id;
    break;

    case L1_MAC_1ST_DATA_BLOCK_SENT:
      gas_id = l1_msg_ptr->msg.first_data_block_sent.gas_id;
    break;

    case L1_MAC_51_SB_FAILURE:
      gas_id = l1_msg_ptr->msg.sb_51_failure.gas_id;
    break;

    case L1_MAC_TBF_REL_CONFIRM:
      gas_id = l1_msg_ptr->msg.tbf_released.gas_id;
    break;

    case L1_MAC_RRBP_FAILED_IND:
      gas_id = l1_msg_ptr->msg.rrbp_failed_ind.gas_id;
    break;

    case L1_MAC_SINGLE_BLOCK_SENT:
      gas_id = l1_msg_ptr->msg.sb_sent.gas_id;
    break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case L1_MAC_PSHO_CNF:
      gas_id = l1_msg_ptr->msg.psho_cnf.gas_id;
    break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    /**************************************************************************
    * L1 to MAC GPRS spec dependent messages (GSM 4.04)
    **************************************************************************/
    case MAC_PH_CONNECT_IND:
      gas_id = l1_msg_ptr->msg.connect_ind.gas_id;
    break;

    case MAC_PH_DATA_IND:
      gas_id = l1_msg_ptr->msg.data_ind.gas_id;
    break;

    case MAC_PH_RANDOM_ACCESS_CONF:
      gas_id = l1_msg_ptr->msg.random_access_cnf.gas_id;
    break;

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM
    case MAC_PH_CS_CONNECT_IND:
      gas_id = l1_msg_ptr->msg.cs_connect_ind.gas_id;
    break;
#endif
#endif

    default:
      MSG_GERAN_ERROR_1_G("GMAC L1 msg id unknown: 0x%x",l1_msg_ptr->sig_id);
    break;
  }

  return gas_id;
}

/*===========================================================================
===
===  FUNCTION      gmac_l1_mac_insert_gas_id
===
===  DESCRIPTION
===  Insert the active gas id in to a MAC->L1 message.
===
===  INPUT
===  Pointer to L1 message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_l1_mac_insert_gas_id(mac_l1_sig_t *common_msg_ptr,
                                      gas_id_t      gas_id)
{
  /* Format IMH header */
  switch (common_msg_ptr->mac_l1_freq_config.message_header.message_id)
  {
    case MAC_L1_FREQ_CONFIG :
      common_msg_ptr->mac_l1_freq_config.gas_id = gas_id;
      break;

    case MAC_L1_DYNAMIC_TBF_CONFIG:
      common_msg_ptr->mac_l1_dynamic_tbf_config.gas_id = gas_id;
      break;

    case MAC_L1_FIXED_TBF_CONFIG:
      common_msg_ptr->mac_l1_fixed_tbf_config.gas_id = gas_id;
      break;

    case MAC_L1_SINGLE_BLOCK_CONFIG:
      common_msg_ptr->mac_l1_single_block_config.gas_id = gas_id;
      break;

    case MAC_L1_DL_SINGLE_BLOCK:
      common_msg_ptr->mac_l1_dl_single_block.gas_id = gas_id;
      break;

    case MAC_L1_DL_TBF_CONFIG:
      common_msg_ptr->mac_l1_dl_tbf_config.gas_id = gas_id;
      break;

    case MAC_L1_PACCH_RRBP:
      common_msg_ptr->mac_l1_pacch_rrbp.gas_id = gas_id;
      break;

    case MAC_L1_CS_RECONFIG:
      common_msg_ptr->mac_l1_cs_reconfig.gas_id = gas_id;
      break;

    case MAC_L1_POWER_CONTROL:
      common_msg_ptr->mac_l1_power_control.gas_id = gas_id;
      break;

    case MAC_L1_PAGE_MODE:
      common_msg_ptr->mac_l1_page_mode.gas_id = gas_id;
      break;

    case MAC_L1_PDCH_REL_TS:
      common_msg_ptr->mac_l1_pdch_rel_ts.gas_id = gas_id;
      break;

    case MAC_L1_DUMMY_CTRL_BLOCK:
      common_msg_ptr->mac_l1_dummy_ctrl_block.gas_id = gas_id;
      break;

    case MAC_L1_RELEASE:
      common_msg_ptr->mac_l1_release.gas_id = gas_id;
      break;

    case MAC_L1_QUALITY_RPT:
      common_msg_ptr->mac_l1_quality_rpt.gas_id = gas_id;
      break;

    case MAC_L1_TS_RECONFIG:
      common_msg_ptr->mac_l1_ts_reconfig.gas_id = gas_id;
      break;

    case MAC_PH_RANDOM_ACCESS_REQ:
      common_msg_ptr->mac_ph_random_access_req.gas_id = gas_id;
      break;

    case MAC_PH_DATA_REQ:
      common_msg_ptr->mac_ph_data_req.gas_id = gas_id;
      break;

    case MAC_L1_CONTENTION_RES:
      common_msg_ptr->mac_l1_contention_res.gas_id = gas_id;
    break;

    case MAC_L1_STOP_RLC_TX:
      common_msg_ptr->mac_l1_stop_rlc_tx.gas_id = gas_id;
    break;

    case MAC_L1_RANDOM_ACCESS_ABORT:
      common_msg_ptr->mac_l1_random_access_abort.gas_id = gas_id;
      break;

#ifdef FEATURE_GSM_DTM
    case MAC_L1_DTM_CHANNEL_ASSIGN_REQ:
      common_msg_ptr->mac_l1_dtm_channel_assign_req.gas_id = gas_id;
      break;

#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
    case MAC_L1_EDTM_CS_REL_TBF_ALLOC:
      common_msg_ptr->mac_l1_edtm_cs_rel_tbf_alloc.gas_id = gas_id;
      break;

    case MAC_L1_DELETE_UL_CTRL_MSG:
      common_msg_ptr->mac_l1_delete_ul_ctrl_msg.gas_id = gas_id;
      break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case MAC_L1_PSHO_REQ:
      common_msg_ptr->mac_l1_psho_req.gas_id = gas_id;
      break;

    case MAC_L1_PSHO_STATUS:
      common_msg_ptr->mac_l1_psho_status.gas_id = gas_id;
      break;

    case MAC_L1_PSHO_CANCEL_PENDING_CONFIG:
      common_msg_ptr->mac_l1_psho_cancel_pending_config.gas_id = gas_id;
      break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    default:
      MSG_GERAN_ERROR_2_G( "Unexpected sig rx. idle task id %d sig id %d" , common_msg_ptr->mac_l1_freq_config.message_header.message_set, 
                                                                            common_msg_ptr->mac_l1_freq_config.message_header.message_id);
      break;

  } /*end switch*/
}

/*===========================================================================
===
===  FUNCTION      gmac_get_rr_gas_id
===
===  DESCRIPTION
===  Returns the gas_id from the incoming RLC msg supplied.
===
===  INPUT
===  rlc_mac_msg_t
===
===  DEPENDENCIES
===  None
===
===  RETURN VALUE
===  gas_id_t
===
===  SIDE EFFECTS
===  None
===
===========================================================================*/
gas_id_t gmac_get_rlc_gas_id( rlc_mac_msg_t *rlc_msg_ptr )
{
  gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  switch(rlc_msg_ptr->msg.sig)
  {
    case RM_UL_EST_TBF_REQ:
      gas_id = rlc_msg_ptr->msg.ul_est_tbf_req.gas_id;
    break;

    case RM_UL_REALLOC_REQ:
      gas_id = rlc_msg_ptr->msg.ul_realloc_req.gas_id;
    break;

    case RM_UL_RELEASE_IND:
      gas_id = rlc_msg_ptr->msg.ul_release_ind.gas_id;
    break;

    case RM_UL_N3104_EXPIRY_IND:
      gas_id = rlc_msg_ptr->msg.ul_n3104_expiry_ind.gas_id;
    break;

    /* confirm to MAC that RLC UL has completed reset */
    case RM_UL_RESET_CNF:
      gas_id = rlc_msg_ptr->msg.ul_reset_cnf.gas_id;
    break;

    /*----------- DOWNLINK --------*/

    /* request MAC to send downlink ack/nack message */
    case RM_DL_PKT_ACKNACK_REQ:
      gas_id = rlc_msg_ptr->msg.dl_pkt_acknack_req.gas_id;
    break;

    /* request MAC to send DL packet control acknowledgement msg (in un-ack mode)
    */
    case RM_DL_PKT_CTRL_ACK_REQ:
      gas_id = rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.gas_id;
    break;

    /* RLC indicates to MAC reception of the 1st data block of a DL assignment */
    case RM_DL_FIRST_BLOCK_IND:
      gas_id = rlc_msg_ptr->msg.dl_first_block_ind.gas_id;
    break;

    /* RLC indicates to MAC of downlink release */
    case RM_DL_RELEASE_IND:
      gas_id = rlc_msg_ptr->msg.dl_release_ind.gas_id;
    break;

    /* confirm to MAC that RLC DL has completed reset */
    case RM_DL_RESET_CNF:
      gas_id = rlc_msg_ptr->msg.dl_reset_cnf.gas_id;
    break;

    case RM_PS_ACCESS_CNF:
      gas_id = rlc_msg_ptr->msg.ps_access_cnf.gas_id;
    break;

    default:
      MSG_GERAN_ERROR_1_G("GMAC RLC msg id unknown: 0x%x",rlc_msg_ptr->msg.sig);
    break;
  }

  return gas_id;
}

/*===========================================================================
===
===  FUNCTION      gmac_rlc_mac_insert_gas_id
===
===  DESCRIPTION
===  Insert the gas id in to a MAC->RLC message.
===
===  INPUT
===  Pointer to RLC message structure of the message to be sent.
===  gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_rlc_mac_insert_gas_id(mac_rlc_msg_t *rlc_msg_ptr,
                                gas_id_t       gas_id)
{
  /* Format IMH header */
  switch (rlc_msg_ptr->msg.sig)
  {
    case MR_UL_ALLOC_CNF :
      rlc_msg_ptr->msg.ul_alloc_cnf.gas_id = gas_id;
      break;

    case MR_UL_ACC_REJ_IND :
      rlc_msg_ptr->msg.ul_acc_rej_ind.gas_id = gas_id;
      break;

    case MR_UL_CONTENTION_IND :
      rlc_msg_ptr->msg.ul_tlli_ind.gas_id = gas_id;
      break;

    case MR_UL_PKT_ACKNACK_IND :
     rlc_msg_ptr->msg.ul_pkt_acknack_ind.gas_id = gas_id;
     break;

    case MR_UL_TIMER_IND :
     rlc_msg_ptr->msg.ul_timer_ind.gas_id = gas_id;
     break;

    case MR_UL_RELEASE_IND :
     rlc_msg_ptr->msg.ul_release_ind.gas_id = gas_id;
     break;

    case MR_UL_CELL_RESEL_IND :
     rlc_msg_ptr->msg.ul_cell_resel_ind.gas_id = gas_id;
     break;

  /* MAC is requesting a complete RLC UL reset */
     case MR_UL_RESET_REQ :
     rlc_msg_ptr->msg.ul_reset_req.gas_id = gas_id;
     break;

     case MR_PS_ACCESS_REQ :
     rlc_msg_ptr->msg.ps_access_req.gas_id = gas_id;
     break;

     case MR_NO_PS_ACCESS_IND :
     rlc_msg_ptr->msg.no_ps_access_ind.gas_id = gas_id;
     break;

#ifdef FEATURE_GPRS_PS_HANDOVER

     case MR_UL_PSHO_START_IND :
     rlc_msg_ptr->msg.ul_psho_start_ind.gas_id = gas_id;
     break;

     case MR_UL_PSHO_COMPLETE_IND :
     rlc_msg_ptr->msg.ul_psho_complete_ind.gas_id = gas_id;
     break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

    /*----------- DOWNLINK -----------*/
     case MR_DL_ASS_IND :
     rlc_msg_ptr->msg.dl_ass_ind.gas_id = gas_id;
     break;

     case MR_DL_TIMER_IND :
     rlc_msg_ptr->msg.dl_timer_ind.gas_id = gas_id;
     break;
  
     case MR_DL_RELEASE_IND :
     rlc_msg_ptr->msg.dl_release_ind.gas_id = gas_id;
     break;

     case MR_DL_TEST_MODE_IND :
     rlc_msg_ptr->msg.dl_test_mode_ind.gas_id = gas_id;
     break;

     case MR_DL_RESET_REQ :
     rlc_msg_ptr->msg.dl_reset_req.gas_id = gas_id;
     break;
#ifdef FEATURE_GPRS_PS_HANDOVER
  
     case MR_DL_PSHO_START_IND :
     rlc_msg_ptr->msg.dl_psho_start_ind.gas_id = gas_id;
     break;

     case MR_DL_PSHO_COMPLETE_IND :
     rlc_msg_ptr->msg.dl_psho_complete_ind.gas_id = gas_id;
     break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_ERROR_1_G( "Unexpected sig rx. idle sig id %d" , rlc_msg_ptr->msg.sig);
      break;

  } /*end switch*/
}

/*** EOF: don't remove! ***/
