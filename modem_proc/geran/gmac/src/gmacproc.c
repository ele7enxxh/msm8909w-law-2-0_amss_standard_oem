/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC COMMON PROCEDURES MODULE    
***
***
*** DESCRIPTION
***
***  This module contains procedures that apply to multiple modes within MAC     
***  
*** EXTERNALIZED FUNCTIONS
***
***  xxx
***  
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  xxx  
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacproc.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 04/12/01   sb      Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "grlcx.h"        /* required for test output usage */
#include "csn.h"
#include "err.h"

#include "gmacdef.h"
#include "gmacrb.h"
#include "gmacutil.h"
#include "gmacmac.h"
#include "gmaclog.h"
#include "gmacproc.h"
#include "gmacnull.h"
#include "gmacgrrsig.h"
#include "gmacds.h"

/*****************************************************************************
***
***     Public Data Declarations
***
***     Global data for entire GMAC subsystem.
***
*****************************************************************************/
gmac_global_data_t gmac_global_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_global_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to global data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_global_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_global_data_t* gmac_get_global_data_ptr(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = &gmac_global_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    global_data_ptr = &gmac_global_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {    
    MSG_GERAN_ERROR_1_G("Global data bad gas_id:%d",gas_id);
  }

  return global_data_ptr;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_common_flags() 
===
===  DESCRIPTION
===  Initialises the common flags, should only be called once at power on.
===
===  PARAMS
===   
===
===  RETURN VALUE
===
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_common_flags( gas_id_t gas_id )
{ 
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  global_data_ptr->c_flags.egprs_arac_sent = FALSE;
  global_data_ptr->c_flags.egprs_rac_retran_req = FALSE;
  global_data_ptr->c_flags.mac_ctrl_header_r_bit = NOT_SET;
  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
  global_data_ptr->c_flags.wait_for_ul_rel_confirm = FALSE;
  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
  global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_mac_proc_data() 
===
===  DESCRIPTION
===  Initialises GMAC PROC data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_mac_proc_data( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  gmac_init_mac_proc_data_non_zero(gas_id);

  memset( &global_data_ptr->mac_mac_sig, 0, sizeof(global_data_ptr->mac_mac_sig) );
  memset( global_data_ptr->dl_msg, 0, sizeof(global_data_ptr->dl_msg) );
  memset( &global_data_ptr->mac_mode, 0, sizeof(global_data_ptr->mac_mode) );
  memset( &global_data_ptr->c_flags, 0, sizeof(global_data_ptr->c_flags) );
  global_data_ptr->gmac_test_mode_ts_offset = 0;
  global_data_ptr->gmac_test_mode_srb_mode = FALSE;
  global_data_ptr->pfi_present = FALSE;
  global_data_ptr->pfi = 0;
  memset( &global_data_ptr->gmac_local_tfi_store, 0, sizeof(global_data_ptr->gmac_local_tfi_store) );
  memset( &global_data_ptr->gmac_local_l1_ta_params, 0, sizeof(global_data_ptr->gmac_local_l1_ta_params) );
  global_data_ptr->gmac_local_rrbp_valid = 0;
  memset( &global_data_ptr->gmac_store_freq_params_in_use, 0, sizeof(global_data_ptr->gmac_store_freq_params_in_use) );
  global_data_ptr->gmac_local_ul_ts_alloc = 0;
  global_data_ptr->gmac_local_dl_ts_alloc = 0;
  global_data_ptr->gmac_local_ul_dl_ctrl_ts = 0;
  global_data_ptr->gmac_local_ul_realloc_ts = 0;
  global_data_ptr->gmac_local_dl_reassign_ts = 0;
  global_data_ptr->gmac_local_ul_realloc_dl_ctrl_ts = 0;
  global_data_ptr->tbf_est_allowed_flag = FALSE;

  memset( &global_data_ptr->gmac_local_pccn_params, 0, sizeof(global_data_ptr->gmac_local_pccn_params) );

  memset( &global_data_ptr->gmac_local_pss_params, 0, sizeof(global_data_ptr->gmac_local_pss_params) );
  memset( &global_data_ptr->gmac_local_stored_ul_chan_req, 0, sizeof(global_data_ptr->gmac_local_stored_ul_chan_req) );
  global_data_ptr->gmac_store_dl_rlc_mode = 0;
  global_data_ptr->gmac_ms_class = 0;
  global_data_ptr->gmac_ms_class_sum = 0;
  global_data_ptr->gmac_ms_class_max_ts_tx = 0;
  global_data_ptr->gmac_ms_class_max_ts_rx = 0;
  global_data_ptr->gmac_ms_class_ts_tta = 0;
  global_data_ptr->gmac_ms_class_ts_ttb = 0;
  global_data_ptr->gmac_ms_class_ts_tra = 0;
  global_data_ptr->gmac_ms_class_ts_trb = 0;
  global_data_ptr->gmac_egprs_ms_class = 0;
  global_data_ptr->gmac_egprs_ms_class_sum = 0;
  global_data_ptr->gmac_egprs_ms_class_max_ts_tx = 0;
  global_data_ptr->gmac_egprs_ms_class_max_ts_rx = 0;
  global_data_ptr->gmac_egprs_ms_class_ts_tta = 0;
  global_data_ptr->gmac_egprs_ms_class_ts_ttb = 0;
  global_data_ptr->gmac_egprs_ms_class_ts_tra = 0;
  global_data_ptr->gmac_egprs_ms_class_ts_trb = 0;

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_ms_class_sum = 0;
  global_data_ptr->gmac_dtm_ms_class_max_ts_tx = 0;
  global_data_ptr->gmac_dtm_ms_class_max_ts_rx = 0;
  global_data_ptr->gmac_dtm_ms_class_ts_tta = 0;
  global_data_ptr->gmac_dtm_ms_class_ts_ttb = 0;
  global_data_ptr->gmac_dtm_ms_class_ts_tra = 0;
  global_data_ptr->gmac_dtm_ms_class_ts_trb = 0;
#endif /*FEATURE_GSM_DTM*/

  global_data_ptr->gmac_local_cs_setting = 0;
  global_data_ptr->gmac_dtm_cs_ts_alloc = 0;

#ifdef FEATURE_GPRS_PS_HANDOVER
  memset( &global_data_ptr->gmac_ps_ho, 0, sizeof(global_data_ptr->gmac_ps_ho) ); 
#endif /*FEATURE_GPRS_PS_HANDOVER*/

#ifdef FEATURE_GSM_EDTM
  global_data_ptr->gmac_edtm_enabled = FALSE; /*mac global indicator for EDTM support which is set from the public store*/ 
  memset( &global_data_ptr->gmac_local_edtm_pcr_params, 0, sizeof(global_data_ptr->gmac_local_edtm_pcr_params) );
#endif /*FEATURE_GSM_EDTM*/

  global_data_ptr->access_tech_req_count = 0 ;
  memset( global_data_ptr->access_tech_req, 0, sizeof(global_data_ptr->access_tech_req) );
  memset( &global_data_ptr->gmac_local_pmr_params, 0, sizeof(global_data_ptr->gmac_local_pmr_params) );
  memset( &global_data_ptr->gmac_local_pccf_params, 0, sizeof(global_data_ptr->gmac_local_pccf_params) );
  global_data_ptr->tbf_type = 0;
  memset( &global_data_ptr->access_requested, 0, sizeof(global_data_ptr->access_requested) );
  global_data_ptr->channel_msg = 0;
  global_data_ptr->gmac_transfer_ul_cv_zero_ind = FALSE;
  global_data_ptr->gmac_re_assign_ul_tfi = 0;
  global_data_ptr->gmac_re_assign_dl_tfi = 0;
  global_data_ptr->csn_msg_bit_length = 0;
  memset( &global_data_ptr->gmac_51_sb_params, 0, sizeof(global_data_ptr->gmac_51_sb_params) );
  memset( &global_data_ptr->gmac_null_store_l1_dl_config, 0, sizeof(global_data_ptr->gmac_null_store_l1_dl_config) );
  global_data_ptr->prr_re_est_rrbp_fn = 0;
  global_data_ptr->prr_re_est_rrbp_ts = 0;
  memset( &global_data_ptr->idle_mode_ul_tbf_release_action, 0, sizeof(global_data_ptr->idle_mode_ul_tbf_release_action) );
  global_data_ptr->rlc_ul_est_tlli = 0;
  memset( &global_data_ptr->gmac_util_data_req_cause, 0, sizeof(global_data_ptr->gmac_util_data_req_cause) );
  global_data_ptr->gmac_current_mac_mode = 0;
  global_data_ptr->current_rcvd_dl_ctrl_ack = FALSE;
  memset( &global_data_ptr->grr_sig, 0, sizeof(global_data_ptr->grr_sig) );
  memset( &global_data_ptr->l1_sig, 0, sizeof(global_data_ptr->l1_sig) );
}

/*===========================================================================
===
===  FUNCTION    gmac_init_mac_proc_data_non_zero() 
===
===  DESCRIPTION
===  Initialises non zero (including all enums) proc data.
===
===  DEPENDENCIES
===  
===
===  RETURN VALUE
===    
===  SIDE EFFECTS
=== 
=== 
===========================================================================*/
void gmac_init_mac_proc_data_non_zero( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  global_data_ptr->current_tbf_mode = TBF_MODE_INVALID;
  global_data_ptr->current_test_mode = TEST_MODE_OFF;
  global_data_ptr->mac_action = NONE;
  global_data_ptr->gmac_store_dl_ass_tlli = (uint32)0xFFFFFFFF;
  global_data_ptr->gmac_store_ul_est_tlli = (uint32)0xFFFFFFFF;
#ifdef FEATURE_GSM_EDTM
  global_data_ptr->gmac_edtm_old_dl_ws = WS_ALLOCATION_INVALID;
  global_data_ptr->gmac_edtm_old_ul_ws = WS_ALLOCATION_INVALID;
#endif /*FEATURE_GSM_EDTM*/

  global_data_ptr->current_egprs_dl_ws = WS_ALLOCATION_INVALID;
  global_data_ptr->current_egprs_ul_ws = WS_ALLOCATION_INVALID;
  global_data_ptr->egprs_2p_sb_mb_alloc = GPRS_SB_ALLOC;
}

/*===========================================================================
===
===  FUNCTION      gmac_comproc_handle_ppcta 
===
===  DESCRIPTION
===  Handles packet power control/timing advance message during UL fixed TBF.
===  Send parameters to L1 using MAC_L1_POWER_CONTROL.        
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data                       
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                      
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_comproc_handle_ppcta(gas_id_t gas_id)
{
  packet_pwr_ctrl_tmng_adv_t *ppcta_data;

  mac_l1_sig_t  mac_l1_sig;
  mac_l1_pwr_ctrl_t *ul_ppcta_msg = &(mac_l1_sig.mac_l1_power_control);

  mac_grr_sig_t        mac_grr_sig;
  mac_grr_store_update_t *grr_pwr_ctrl_msg = &(mac_grr_sig.mac_grr_store_update);
  gmac_global_data_t     *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  ppcta_data = (packet_pwr_ctrl_tmng_adv_t *)global_data_ptr->csn_dec_data_ptr;

  memset( &mac_l1_sig, 0, sizeof( mac_l1_sig_t ));
  memset( &mac_grr_sig, 0, sizeof( mac_grr_sig_t ));

  MSG_GERAN_MED_0_G("MAC Rcvd PACKET_PWR_CTRL_TMNG_ADV");

  MSG_GERAN_HIGH_2_G(" p control %d, ta %d",ul_ppcta_msg->power_control.present,ul_ppcta_msg->global_packet_ta_present);

  /* Always send Power Control Parameters to L1*/
  if(ppcta_data->power_control_flag == valid) 
  {
    ul_ppcta_msg->power_control.present = TRUE;
    gmac_power_control_params_ie(&(ppcta_data->pwr_ctrl_paramtrs), &(ul_ppcta_msg->power_control), gas_id);
  }
  else
  {
    ul_ppcta_msg->power_control.present = FALSE;
  }
  
  /*Always send Global Packet TA Parameters to l1*/
  if(ppcta_data->global_timing_adv_flag == valid) 
  {
    ul_ppcta_msg->global_packet_ta_present = TRUE;
    gmac_global_packet_ta_ie(&(ppcta_data->global_pkt_timing), &(ul_ppcta_msg->global_packet_ta), gas_id);
  }
  else
  {
    ul_ppcta_msg->global_packet_ta_present = FALSE;
  }
  
  /*Always send Global Power Control Parameters to l1*/
  if(ppcta_data->global_pwr_ctrl_flag == valid) 
  {
    grr_pwr_ctrl_msg->id = RRPS_POWER_CTRL;
    gmac_global_power_ctrl_params_ie(&(ppcta_data->global_pwr_ctrl), &(grr_pwr_ctrl_msg->update.global_power_control_params), gas_id);
    
    /* send MAC_GRR_STORE_UPDATE with parameters */
    MSG_GERAN_LOW_0_G( "MAC Send MAC_GRR_STORE_UPDATE");
    mac_grr_sig.mac_grr_store_update.message_header.message_set = MS_MAC_RR;
    mac_grr_sig.mac_grr_store_update.message_header.message_id = (uint8)MAC_GRR_STORE_UPDATE;
    gprs_grr_mac_acc_func(&mac_grr_sig, gas_id);
  }

  
  /* only send parameters to L1 if there are changes */
  if ((ul_ppcta_msg->power_control.present) ||
      (ul_ppcta_msg->global_packet_ta_present))
  {
    /* send MAC_L1_POWER_CONTROL with parameters */
    MSG_GERAN_LOW_0_G( "MAC Send MAC_L1_POWER_CONTROL");
    mac_l1_sig.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
    mac_l1_sig.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;
    gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_normal_ul_tbf_release_proc() 
===
===  DESCRIPTION
===  Called from transfer to release the ul tbf once all rlc data blocks have
===  been successful transferred.
=== 
===  The trigger is the reception of PUAN with FAI (final acknowledgment
===  indicator) of Ack/Nack Description IE set.
===
===  note:- under normal release conditions RLC does not send RM_UL_RELEASE_IND 
===         signal to release the ul.
=== 
===  For the cases where there is more data to send, transfer must have previously
===  received and RM_UL_REALLOC_REQ containing the Channel Request Description IE.
===
===  ref 4.60, 9.3.3.3 and 9.3.2.4 rev 1999
===
===  DEPENDENCIES
===  pointer to last signal received by mac  ie  msg_ptr, this must be pointing
===  to the ph_data_ind that the PUAN was received in. This function needs  
===  access to the MAC control header .
===
===  idle_mode_ul_tbf_release_action  
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
idle_mode_ul_tbf_release_action_t gmac_normal_ul_tbf_release_proc(boolean tbf_est_set,
                                      boolean dl_tbf_active,
                                      boolean est_new_ul_tbf,
                                      gas_id_t gas_id)
{
  control_ack_type_t    control_ack_type;
  uint8                 rrbp_value;
  uint8                 control_ack_value = 0;  /* when using access burst the */
                                                /* control ack type is zero if */
                                                /* re-est from ul tbf release  */
  access_burst_t        ab_type = EIGHT_BIT;
  
  uint32                local_rrps_tlli = 0;
  uint32                old_local_rrps_tlli = 0;

  uint8  rbsn_bit  = 0;
  uint8  fs_bit    = 0;

  mac_l1_sig_t          local_mac_l1_sig;
  mac_int_sig_t         local_mac_mac_sig;
  mac_l1_pacch_rrbp_t   *ml_pacch_rrbp_sig_ptr = &(local_mac_l1_sig.mac_l1_pacch_rrbp);
  mac_l1_quality_rpt_t  *qual_msg_ptr = &(local_mac_l1_sig.mac_l1_quality_rpt);
  gmac_global_data_t    *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  
  /* used by idle mode to determine appropriate action */
  /* to take if re-est from ul tbf release             */
  global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST; 
  
  memset(&local_mac_l1_sig, 0, sizeof(mac_l1_sig_t));
  
  
  memset(&local_mac_mac_sig, 0, sizeof(mac_int_sig_t));
 
    rrbp_value = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x30) >> 4;
         
    if ((tbf_est_set == FALSE) || ((tbf_est_set == TRUE) && (est_new_ul_tbf == FALSE)))
    {
      /********************************************************************
      *              release ul , no ul tbf re-establishment              *
      *  send MAC_L1_PACCH_RRBP in the rrbp of PUAN the FAI was received  *
      *  in (ie a PCA using NB). NOTE control_ack_value = 3;              *
      *********************************************************************/

      control_ack_value = 3;  /*default*/ 

      if( ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) >> 3) == 1)
      {
        
        MSG_GERAN_MED_3_G("payload %d channel_msg %d ctrl_ack_value %d",
                          ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0xc0) >> 6),
                          global_data_ptr->channel_msg,control_ack_value);
        /*check payload is "10"*/
        if(( ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0xc0) >> 6) == 2) && (global_data_ptr->channel_msg != SEGMENTED_MSG))
        {
          /*This is the case where the DL ctrl msg is polled and optional mac control */
          /*header octets are present BUT the DL msg is NOT segmented. The optional   */
          /*headers are being used for power reduction purposes . ref 4.60  9.1.12a.  */
          /*UNDER THESE CONDTIONS the PCA type to be used has to be determined as if  */
          /*DL ctrl msg WAS segmented                                                 */   
         
          rbsn_bit = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 7);
          fs_bit =  ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 1) & 0x01);

          MSG_GERAN_MED_2_G("MAC info optional ctrl headers used for PWR rbsn_bit %d fs_bit %d",rbsn_bit,fs_bit);

          MSG_GERAN_MED_3_G("MAC info optional ctrl headers 0x%x 0x%x 0x%x",global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0],
                                                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1],
                                                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2]);

          if((rbsn_bit == 0) && (fs_bit == 1))
          {
            /*check AC bit, AC = 1 TFI/D field present AC = 0 TFI/D field NOT present*/
            if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x01) == 1)
            {  
              /*note PUAN has to have been addressed correctly to get here so don't need to check TFI field*/

              /*RBSN in the 1st optional dl mac control block header determines the control_ack value */
              if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1]  >> 7) == 1 )
              {
                /*RBSN =1, ctrl ack type = 1   ref 4.60  11.2.2.2*/
                control_ack_value = 1;
              }
              else
              {
                /*RBSN = 0, ctrl ack type = 2  ref 4.60  11.2.2.2*/
                control_ack_value = 2;
              }
            }
            else
            {
              /* 2nd optional octet not present */
              MSG_GERAN_MED_0_G( "ERROR: MAC control header 2nd optional octet not present");
              /*use default "control_ack_value" and send PCA, best to always send a PCA when we are releasing */
            }
          }
          else
          {     
            /*PWR incorrectly specified , RBSN and FS combination incorrect*/
            MSG_GERAN_MED_2_G("MAC info error in PWR option rbsn_bit %d fs_bit %d",rbsn_bit,fs_bit);
            /*use default "control_ack_value" and send PCA, best to always send a PCA when we are releasing */
          }
        }
      }
      
      if (dl_tbf_active == FALSE)
      {
        /* wait for L1 to return to idle before handling new tbf establishment requests */
        global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
      }

      if(est_new_ul_tbf == TRUE)
      {
        /*This is the case where RLC has requested a resource reallocation via RM_UL_REALLOC_REQ*/
        /*and before it can be actioned the UL is released via PUAN with FAI set 1               */
        /*under these conditions release RLC as normal, RLC will then complete its current UL and*/
        /*then initiate a new  ul tbf to send the PDU that generated the initial recourse       */
        /*reallocation request to MAC      */

        global_data_ptr->gmac_transfer_ul_cv_zero_ind = FALSE;
      }
      
      global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.up_ctrl_ack =  control_ack_value ;
      
      rr_read_public_store(RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE,&control_ack_type, gas_id);
      
      /*determine which method of re-establishment to use*/
      if(control_ack_type == ACCESS_BURST )
      {
        /*******************************************************************
        * send MAC_L1_PACCH_RRBP in the rrbp of PUAN the FAI was received  *
        * in (ie a PCA using AB).                                          *
        *******************************************************************/
        
        rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&ab_type, gas_id);
        
        if( ab_type == EIGHT_BIT)
        { 
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_8_BIT_AB_FORMAT + control_ack_value;
          ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_8_BIT;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_11_BIT_AB_FORMAT_BITS_9_TO_2;
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[1] = PCA_11_BIT_AB_FORMAT_BITS_1 + control_ack_value; 
          ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_11_BIT;
        }
      }
      else
      {
        
      /*******************************************************************
      * send MAC_L1_PACCH_RRBP in the rrbp of PUAN the FAI was received  *
      * in (ie a PCA using NB).                                          *
        *******************************************************************/
        
        /*TLLI in*/
        MSG_GERAN_HIGH_1_G(" gmac_store_ul_est_tlli %x",global_data_ptr->gmac_store_ul_est_tlli);
        if(global_data_ptr->gmac_store_ul_est_tlli == (uint32)0xFFFFFFFF)
        {
          rr_read_public_store(RRPS_TLLI,&local_rrps_tlli, gas_id);
         
          /* Check "DETACH" case */
          if(local_rrps_tlli == (uint32)0xFFFFFFFF)
          {
            rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli, gas_id);
            global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = old_local_rrps_tlli;
          }
          else
          {
            global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = local_rrps_tlli;  
          }
        }
        else
        {
          global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = global_data_ptr->gmac_store_ul_est_tlli;  
        }

        
        /* copy csn.1 encoded pca into mac_l1_pacc_rrbp signal */
        if (gcsn1_encoder(PACKET_CONTROL_ACKNOWLEDGE,&(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),&global_data_ptr->csn_encoder_ul_msg) == PASS)
        {
          /* load rlc/mac control header */
          if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
          {
            ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
          }
          else
          {
            ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
          }
          ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;
          
          /* call log-on-demand function */
          (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                           PACKET_CONTROL_ACKNOWLEDGE,
                                           SIZE_OF_CONTROL_RADIO_BLOCK,
                                           &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                           gas_id);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("ERROR: CSN1 ul msg encoder error");
        }
      }
      
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP");
      ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,rrbp_value, gas_id);
      ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
      ml_pacch_rrbp_sig_ptr->rrbp_msg = PCA_UL_FAI_MSG;
#ifdef FEATURE_GSM_EDTM
      ml_pacch_rrbp_sig_ptr->message_type = PACKET_CONTROL_ACKNOWLEDGE;
#endif /*FEATURE_GSM_EDTM*/
      ml_pacch_rrbp_sig_ptr->message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      ml_pacch_rrbp_sig_ptr->message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&local_mac_l1_sig, gas_id);
      
      /**********************************************************************
      *  this signal can not be acted upon in L1 until the PCA has been     *
      *  sent on the PACCH. on reception of the release signal l1 checks    *  
      *  to see if the RRBP buffer is empty before it acts on the signal    * 
      **********************************************************************/
      
      /*               send MAC_L1_RELEASE  to l1                  */
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_RELEASE");

      gmac_util_block_rlc(NO_DEL, GMAC_PROC, __LINE__,gas_id);

      if ( global_data_ptr->mac_mode == TRANSFER_MODE)
      {
        gmac_tran_send_mac_l1_release(UL_TBF_NORMAL,gas_id);
      }
      else
      {
        /*event logging changes*/
        mac_log_packet_tbf_rel_event(UL_TBF_NORMAL,gas_id);
        local_mac_l1_sig.mac_l1_release.release_tbf = UL_TBF_NORMAL;
        local_mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
        local_mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
        gprs_l1_mac_acc_func(&local_mac_l1_sig,gas_id);
      }
    }
    else if((tbf_est_set == TRUE) && (est_new_ul_tbf == TRUE) && (dl_tbf_active == FALSE))
    {
      /*********************************************************************
      *                     ul tbf re-establishment                        *   
      *********************************************************************/ 
      
      rr_read_public_store(RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE,&control_ack_type, gas_id);
      
      /*determine which method of re-establishment to use*/
      if(control_ack_type == ACCESS_BURST )
      {
        /*******************************************************************
        * send MAC_L1_PACCH_RRBP in the rrbp of PUAN the FAI was received  *
        * in (ie a PCA using AB). NOTE control ack type is zero if re-est  *
        * from ul tbf release.                                             *
        * NOTE. L1 is NOT released,MS remoains on the current PDTCH          *
        * Wait in UL_1P_ASS state for a SB assignment use this to send PRR *
        * NOTE t3168 will be started once PUA sb alloc has been received   *
        *******************************************************************/
        
        control_ack_value = 0;
        
        rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&ab_type, gas_id);
        
        if( ab_type == EIGHT_BIT)
        { 
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_8_BIT_AB_FORMAT + control_ack_value;
          ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_8_BIT;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_11_BIT_AB_FORMAT_BITS_9_TO_2;
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[1] = PCA_11_BIT_AB_FORMAT_BITS_1 + control_ack_value; //bit 1 + cat
          ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_11_BIT;
        }
        
        global_data_ptr->idle_mode_ul_tbf_release_action = RE_EST_WAIT_FOR_SB_ALLOC;
        
        MSG_GERAN_MED_0_G("MAC Sent MAC_TI_RE_ESTABLISH ");
        local_mac_mac_sig.sig_id = MAC_TI_RE_ESTABLISH;
        local_mac_mac_sig.task_id = MS_MAC_MAC;            
        gprs_mac_mac_acc_func(&local_mac_mac_sig, gas_id);
        
        MSG_GERAN_MED_0_G("MAC Sent MAC_L1_PACCH_RRBP");
        ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,rrbp_value, gas_id);
        ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
        ml_pacch_rrbp_sig_ptr->rrbp_msg = PCA_UL_FAI_MSG;
#ifdef FEATURE_GSM_EDTM
        ml_pacch_rrbp_sig_ptr->message_type = PACKET_CONTROL_ACKNOWLEDGE;
#endif /*FEATURE_GSM_EDTM*/
        ml_pacch_rrbp_sig_ptr->message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
        ml_pacch_rrbp_sig_ptr->message_header.message_set = MS_MAC_L1;
        gprs_l1_mac_acc_func(&local_mac_l1_sig, gas_id);
      }
      else
      {
        /****************************************************************
        * send MAC_L1_QUALITY_RPT with parameters, wait in UL_1P_ASS for*
        * L1_MAC_QUAL_MEASURMENTS, format PRR send it using the RRBP    *
        * recived in PUAN change state to UL2P_ASS and wait for PUA     *
        * Note t3168 will be started once prr is sent                   *
        * Note need to save ts and fn rrbp is to be tx-ed on            *
        ****************************************************************/
        
        global_data_ptr->prr_re_est_rrbp_fn =  gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,rrbp_value, gas_id);
        global_data_ptr->prr_re_est_rrbp_ts = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
        
        global_data_ptr->idle_mode_ul_tbf_release_action = RE_EST_SEND_PRR;
        
        MSG_GERAN_MED_0_G("MAC sent MAC_TI_RE_ESTABLISH");
        local_mac_mac_sig.sig_id = MAC_TI_RE_ESTABLISH;
        local_mac_mac_sig.task_id = MS_MAC_MAC;            
        gprs_mac_mac_acc_func (&local_mac_mac_sig, gas_id);
        
        MSG_GERAN_LOW_0_G("MAC Send MAC_L1_QUALITY_RPT");
        qual_msg_ptr->rrbp_value = global_data_ptr->prr_re_est_rrbp_fn;
        qual_msg_ptr->i_level_ind = FALSE;
        local_mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
        local_mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
        gprs_l1_mac_acc_func(&local_mac_l1_sig, gas_id);
      }

      /* Return to IDLE because we are re-establishing*/
      global_data_ptr->mac_mode = IDLE_MODE;
      
	  /* TODO mac_transfer_substate can't be set from gmacproc.c */
	  /* mac_transfer_substate = TRANSFER_INACTIVE; */
    }

  return (global_data_ptr->idle_mode_ul_tbf_release_action);
}

/*** EOF: don't remove! ***/
