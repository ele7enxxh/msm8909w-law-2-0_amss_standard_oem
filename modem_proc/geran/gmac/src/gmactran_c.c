/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER CONCURRENT MODULE
***
***
*** DESCRIPTION
***
***  This module contains functionality related to concurrent packet           
***  transfer (i.e. when UL and DL exist)                 
***  
*** EXTERNALIZED FUNCTIONS
***
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
*** Copyright (c) 2001,2002,2003,2004,2005-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_c.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/12/01   sb      Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "grlcx.h"    /* required for test output usage */
#include "csn.h"
#include "gmacdef.h"
#include "gmacrb.h"
#include "gmactransfer.h"
#include "gmacutil.h"
#include "gmactran_dl.h"
#include "gmactran_ul.h"
#include "gmactran_c.h"
#include "gmaclog.h"
#include "gmacproc.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static void gmac_concurrent_transfer_handle_tbf_release(gas_id_t gas_id);  

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_data_ind(gas_id_t gas_id)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type,
                                  gas_id);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE,gas_id);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode,gas_id))
      {
        grlc_dl_start_t3190(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN_DL_REASS");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN;
        /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
        go to a appropriate transfer state if gmac gets a dl release cnf before connect
        ind from GL1. */
        transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id); 
        }            

        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      if(transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release == FALSE)
      {
        /* CR 69716 */
        if( 
            ( rex_get_timer(&mac_timers_ptr->timer_t3180) == 0 ) && 
            ( global_data_ptr->current_test_mode != TEST_MODE_SRB ) 
          )
        {
          MSG_GERAN_MED_2_G("PUA rcvd during ul rel. t3180 = %d, Test Mode: %d",
                            rex_get_timer(&mac_timers_ptr->timer_t3180),global_data_ptr->current_test_mode); 
        }
        else
        {
          if (gmac_tdynamic_handle_pua(gas_id))
          {
            gmac_stop_t3168(gas_id);
            MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DYN_REALL");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_REALLOC;
  
            if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
            {
              gmac_transfer_simulate_freq_cnf(gas_id); 
            }            
   
            (void)mac_log_state(gas_id);
          }       
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Blocking PUA in T_CON, releasing UL normally");
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
        if (gmac_tdynamic_handle_ptr(gas_id))
        {
          gmac_stop_t3168(gas_id);
          MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DL_REASS_DYN_REALL");
          transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
          /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
          go to a appropriate transfer state if gmac gets a dl release cnf before connect
          ind from GL1 */
          transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;
  
          if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }
  
          (void)mac_log_state(gas_id);
        }
      break;

    case PACKET_ACCESS_REJECT:
      gmac_transfer_handle_par(gas_id);
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE,gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate,gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release(gas_id);
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL,gas_id);
      break;

    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler(gas_id);
      break;
  
    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch(gas_id);
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id);
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL,
                                                             gas_id );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change,gas_id);

        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, new_state_str);       
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state(gas_id);
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      if(gmac_tran_check_psho_addressing(gas_id) == TRUE)
      {
        MSG_GERAN_MED_0_G("MAC Rcvd PS_HANDOVER_COMMAND");
        gmac_send_data_ind_to_grr(gas_id);
        rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);        
      }
    break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_dl_reassign_data_ind(gas_id_t gas_id)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type,
                                  gas_id);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE, gas_id);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* PDA rxd in UL state so apply current_mac_mode */
      if(gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode, gas_id))
      {
         grlc_dl_start_t3190(gas_id);

         if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
         {
           gmac_transfer_simulate_freq_cnf(gas_id); 
         }
      }
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until DL reassignment complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DL_REASS_DYN_REALL");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }

        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_ACCESS_REJECT:
      gmac_transfer_handle_par(gas_id);
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE, gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release(gas_id);
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL,
                                      gas_id);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler(gas_id);
      break;
      
    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch(gas_id);
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id);
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL,
                                                             gas_id );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change, gas_id);

        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state(gas_id);
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler(gas_id);
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_data_ind(gas_id_t gas_id)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type,
                                  gas_id);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE,gas_id);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* cr 697201 PDA rxd while waiting for connect ind for UL in             */
      /*TRANSFER_CONCURRENT_DYNAMIC_REALLOC state. Reception of PDA is         */
      /*going to put MAC into  TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC */ 
      /*therefor  MAC is going to need to know if its expecting to get two     */
      /*connect inds from l1.  1 for the UL and 1 for the DL UL and D so need  */
      /*set waiting_for_ph_con_for_dl and waiting_for_ph_con_for_ul            */

      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode,gas_id))
      {
        
        transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;

        /*Must be in this state but checking to confirm*/
        if(transfer_data_ptr->mac_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_REALLOC)
        {
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = TRUE;
        }
        
        grlc_dl_start_t3190(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
       
        /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
        go to a appropriate transfer state if gmac gets a dl release cnf before connect
        ind from GL1. */
        transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }            

        (void)mac_log_state(gas_id);
      }

      break;

    case PACKET_UPLINK_ASSIGNMENT:
      if (gmac_tdynamic_handle_pua(gas_id))
      {
        if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id); 
        }
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        MSG_GERAN_MED_0_G("GMAC State tran = T_CON_DL_REASS_DYN_REALL");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }

        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE,gas_id);
      break;
      
    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate,gas_id);
      break;
      
    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release(gas_id);
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL,gas_id);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler(gas_id);
      break;

    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch(gas_id);
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id);
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL,
                                                             gas_id );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change, gas_id);

        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state(gas_id);
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler(gas_id);
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind(gas_id_t gas_id)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type,
                                  gas_id);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/
  
  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE, gas_id);
      break;
    
    case PACKET_DOWNLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if(gmac_tdynamic_handle_ptr(gas_id))
      {
        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
      }
    break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE, gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;
      
    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release(gas_id);
      break;
      
    case PACKET_PDCH_RELEASE: 
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL,
                                      gas_id);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler(gas_id);
      break;

    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch(gas_id);
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler(gas_id);
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL,
                                                             gas_id );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change, gas_id);

        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "ds%d: GMAC State tran = %s",gas_id, new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state(gas_id);
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler(gas_id);
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_concurrent_transfer_handle_tbf_release()
===
===  DESCRIPTION
===  Handles PACKET TBF RELEASE for concurrent TBF
===
===  UL TBF
===  ------
===  If the cause is NORMAL the MS shall continue to the next LLC PDU 
===  boundary, starting the count down procedure.
===
===  If the cause is ABNORMAL the MS shall abort the uplink TBF and perform
===  an abnormal release with access retry. If a valid RRBP field is received
===  the MS shall send a PCA in the uplink radio block specified.
===
===  DL TBF
===  ------
===  The network may initiate abnormal release of a downlink TBF.
=== 
===  If a valid RRBP, MS shall transmit a PCA in the uplink radio 
===  block specified.
===
===  If there is no UL TBF, the MS shall return to packet idle. Otherwise
===  remain in UL packet transfer.
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_concurrent_transfer_handle_tbf_release(gas_id_t gas_id)
{
  rrbp_msg_t rrbp_msg;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  packet_tbf_release_t *packet_rel_ptr = (packet_tbf_release_t *)global_data_ptr->csn_dec_data_ptr;
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  
  if (gmac_util_global_tfi(packet_rel_ptr->global_tfi, gas_id))
  {
    MSG_GERAN_MED_3_G("MAC Rcvd PACKET_TBF_RELEASE rel cause %d, UL:%d, DL:%d",
                      packet_rel_ptr->tbf_release_cause,
                      packet_rel_ptr->uplink_release,
                      packet_rel_ptr->dwnlink_release);

    if ( global_data_ptr->current_test_mode == TEST_MODE_SRB )
    {
      /* If the SRB sub-mode is ON then L1 inhibits all control signals (inc. PCA's).
         In this state the NW may send a polled TBF release to terminate SRB mode.  To ensure that
         the PCA is sent the rrbp_msg is set to PCA_SRB to override the inhibit */

      rrbp_msg = PCA_SRB;

      /* The NW should release both UL and DL when in SRB mode.  If it does not
         then force UL+DL release to 1. */
      MSG_GERAN_MED_2_G("MAC EGPRS, Terminating SRB mode, Pkt TBF Rel ul:%d, dl:%d", 
                        packet_rel_ptr->uplink_release, packet_rel_ptr->dwnlink_release);

      packet_rel_ptr->uplink_release = 1;
      packet_rel_ptr->dwnlink_release = 1;
    }
    else
    {
      rrbp_msg = PCA_DL_MSG;
    }

    rrbp_control_block_handler_2(rrbp_msg, gas_id);
    
    if (packet_rel_ptr->tbf_release_cause == GMAC_PACKET_TBF_REL_NORMAL_CAUSE) 
    {
      /* Normal cause value */
      if (packet_rel_ptr->uplink_release)
      {
        gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL, gas_id); 
        mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);
        
      }
      /* downlink release is always abnormal!! */
      if (packet_rel_ptr->dwnlink_release)
      {
        gmac_util_send_dl_release_to_rlc(gas_id);
        gmac_transfer_dl_release_clean(gas_id);
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);        
      }
    }
    else
    {
      /* Abnormal cause value - Release TBF's specified in Packet TBF Release */
      /* Remain in transfer if any currently active TBF's are not specified by the Pkt TBF Rel*/

      if (packet_rel_ptr->uplink_release)
      {
        gmac_transfer_ul_dynamic_release_clean(gas_id);
        gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__, gas_id);
        mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE, gas_id);
      }

      if (packet_rel_ptr->dwnlink_release)
      {
        gmac_transfer_dl_release_clean(gas_id);
        gmac_util_send_dl_release_to_rlc(gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);
      }

      if ( (packet_rel_ptr->uplink_release) &&
           (packet_rel_ptr->dwnlink_release) )
      {
        gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL, gas_id);
        transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;
      }
      else if (packet_rel_ptr->uplink_release)
      {
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id); 
      }
      else if (packet_rel_ptr->dwnlink_release)
      {
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id); 
      }
    }
  }
}

/*** EOF: don't remove! ***/
