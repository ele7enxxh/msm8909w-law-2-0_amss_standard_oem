/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER UPLINK MODULE (FIXED AND DYNAMIC)
***
***
*** DESCRIPTION
*** This module contains functionality related to uplink packet transfer
***
***
*** EXTERNALIZED FUNCTIONS
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_ul.c#1 $
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
#include "customer.h"

#include "grlcx.h"
#include "csn.h"
#include "gmac.h"
#include "gmacdef.h"
#include "gmactransfer.h"
#include "gmacproc.h"
#include "gmacrb.h"
#include "gmactimers.h"
#include "gmactran_ul.h"
#include "gmactran_dl.h"
#include "gmacutil.h"
#include "gmaclog.h"
#include "grlctimer.h"
#include "grlcmac.h"
#include "gmacds.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static void gmac_util_send_stop_rlc_tx_to_l1(gas_id_t gas_id);
static void gmac_transfer_ul_tbf_release(boolean dl_tbf_present, gas_id_t gas_id);

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*For EGPRS logging, as they are used in 4 different functions in this module.*/

 #define  BEP_PERIOD2_NOT_SPECIFIED 0xFF
 #define EGPRS_TBF                  1


/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_init_tran_ul_data()
===
===  DESCRIPTION
===  Initialises gmac transfer ul data.  Used for Lower layer resets.
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
void gmac_init_tran_ul_data( gas_id_t gas_id )
{
  /*No data to reset*/
}

/*===========================================================================
===
===  FUNCTION      gmac_util_send_stop_rlc_tx_to_l1
===
===  DESCRIPTION
===  Send signal MAC_L1_STOP_RLC_TX to L1. No data contained in signal.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_util_send_stop_rlc_tx_to_l1(gas_id_t gas_id)
{
  mac_l1_sig_t  mac_l1_sig;

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_STOP_RLC_TX");
  mac_l1_sig.mac_l1_stop_rlc_tx.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_stop_rlc_tx.message_header.message_id = (uint8)MAC_L1_STOP_RLC_TX;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_handle_pda
===
===  DESCRIPTION
===  Handles PACKET DOWNLINK ASSIGNMENT received during packet transfer
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if frequency parameters are different from what
===    is currently in use for uplink, ignore PDA
===  - if does not define PDCH satisfactorily, ignore PDA
===  - failure for any other reason, abort downlink, continue with uplink
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===  - TRUE if PDA successful
===  - FALSE if PDA unsuccessful
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_transfer_ul_handle_pda(uint8 current_mac_mode,
                                    gas_id_t gas_id)
{
  packet_downlink_assignment_t *pda_ptr;

#define BEP_PERIOD2_FIELD_NOT_SPECIFIED 0xFF
  uint16 assigned_dl_ws =0;
  uint16 log_bep_prd2;
  boolean                 pda_accept_flag = FALSE;
  boolean                 addr_match = FALSE;
  pua_data_error_t        freq_param_result;
  uint32                  local_rrps_tlli;

  /*TLLI in DL CNF to RLC*/
  uint32                  old_local_rrps_tlli;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  pda_ptr = (packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

  /*TLLI in DL CNF to RLC*/
  rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
  rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli,gas_id);

  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {



    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {


      /* handle address - TLLI or global TFI */
      switch (pda_ptr->tlli_gtfi_pkt_ref_valid)
      {
        case tlli_valid:
          /* NOTE: It is possible that addressing using TLLI during transfer
          **       is not valid but at present it is treated as valid.
          */
          MSG_GERAN_HIGH_1_G("PACKET_DOWNLINK_ASS addressed by tlli %x",pda_ptr->tlli);
          /*TLLI in DL CNF to RLC*/

          /*if dl tbf active check against gmac_store_dl_ass_tlli else check against gmac_store_ul_est_tlli*/
          if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid)
          {
            if(pda_ptr->tlli != global_data_ptr->gmac_store_dl_ass_tlli)
            {

                    /*old bit*/
                    if(pda_ptr->tlli == local_rrps_tlli)
                    {
                      global_data_ptr->gmac_store_dl_ass_tlli = local_rrps_tlli;
                      addr_match = TRUE;
                    }
                    else if (pda_ptr->tlli == old_local_rrps_tlli)
                    {
                      global_data_ptr->gmac_store_dl_ass_tlli = old_local_rrps_tlli;
                      addr_match = TRUE;
                    }
                    MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                                       global_data_ptr->gmac_store_dl_ass_tlli,
                                       local_rrps_tlli,
                                       old_local_rrps_tlli);
            }
            else
            {
               MSG_GERAN_HIGH_1_G("MAC  gmac_store_dl_ass_tlli %x,",global_data_ptr->gmac_store_dl_ass_tlli);
               addr_match = TRUE;
            }
          }
          else if(global_data_ptr->gmac_local_tfi_store.ul_tfi_valid)
          {
            if(pda_ptr->tlli != global_data_ptr->gmac_store_ul_est_tlli)
            {
                    if(pda_ptr->tlli == local_rrps_tlli)
                    {
                      global_data_ptr->gmac_store_dl_ass_tlli = local_rrps_tlli;
                      addr_match = TRUE;
                    }
                    else if (pda_ptr->tlli == old_local_rrps_tlli)
                    {
                      global_data_ptr->gmac_store_dl_ass_tlli = old_local_rrps_tlli;
                      addr_match = TRUE;
                    }
                    MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                                       global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);
            }
            else
            {
              MSG_GERAN_HIGH_1_G("MAC  gmac_store_ul_est_tlli %x,",global_data_ptr->gmac_store_ul_est_tlli);
              global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
              addr_match = TRUE;
            }

          }

          break;

        case global_tfi_valid:
          /* if stored TFI = received TFI then match else ignore message */
          addr_match = gmac_util_global_tfi(pda_ptr->global_tfi,gas_id);

          /*TLLI in DL CNF to RLC*/
          if((global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE) && ( addr_match == TRUE ))
          {
            /*There is no DL TBF currently active, PDA addressed by UL TFI*/
            global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
          }

          MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                             global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);

          break;

        default:
          break;
      }

    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;

      /*TLLI in DL CNF to RLC*/
      if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE)
      {
        /*There is no DL TBF currently active, PDA addressed by UL TFI*/
        global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
      }

      MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                         global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }

  }

  if (addr_match)
  {
    MSG_GERAN_MED_0_G("MAC Rcvd PACKET_DOWNLINK_ASSIGNMENT");

    if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
    {
      MSG_GERAN_LOW_0_G("MAC info PDA ME");
      gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_DOWNLINK_ASSIGNMENT, gas_id);
      return (pda_accept_flag);
    }

#ifdef FEATURE_GSM_DTM
    if ( ( transfer_data_ptr->t_flags.dtm_active ) &&
         ( pda_ptr->freq_param_flag == valid ) )
    {
      MSG_GERAN_HIGH_0_G("GMAC, Freq Params rxd in PDA whilst in DTM, AB NORM REL");
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
      mac_log_ul_tbf_release(MAC_LOG_UL_FREQ_ERROR_ACCESS_RETRY,gas_id);

      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
      {
        mac_log_dl_tbf_release(MAC_LOG_DL_FREQ_ERROR_ACCESS_RETRY,gas_id);
      }

      return (pda_accept_flag);
    }
#endif /*FEATURE_GSM_DTM*/

    /*Check for PDA with no timeslots allocated*/
    if ( pda_ptr->timeslot_alloc == 0 )
    {
      MSG_GERAN_HIGH_0_G("GMAC, PDA has a timeslot alloc of 0, ignore msg");
      return (pda_accept_flag);
    }

    /* TBF_MODE */
    if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
    {
#ifdef FEATURE_GSM_EDTM
       /*store ws prior to reassignment, used to restore ws when DTM Ass is received with no PS defined*/
       global_data_ptr->gmac_edtm_old_dl_ws = global_data_ptr->current_egprs_dl_ws;
#endif /*FEATURE_GSM_EDTM*/

       if(pda_ptr->r99_additions_flag == not_valid)
       {
          MSG_GERAN_MED_0_G("MAC info:- in EGPRS mode. PDA ignored no rel 99 features");
          return (pda_accept_flag);
       }
       else
       {
         if (pda_ptr->egprs_options_flag == valid)
         {
           MSG_GERAN_MED_0_G("MAC Info:- in EGPRS. Rel99 features contain EGPRS data");

           transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_mode = TBF_MODE_EGPRS;

           /*Check validity of DL ws*/
           assigned_dl_ws = gmac_egprs_decode_dl_window_size(pda_ptr->egprs_win_sz, pda_ptr->timeslot_alloc,gas_id);

           /*If an invalid WS has been allocated ignore message*/
           if(assigned_dl_ws != WS_ALLOCATION_INVALID)
           {
             global_data_ptr->current_egprs_dl_ws = assigned_dl_ws;
           }
           else
           {
             /* Error in DL window size - ignore PDA */
             MSG_GERAN_HIGH_3_G("MAC info:- PDA encoded WS (%d) for TS alloc (0x%x) is INVALID, current WS: %d, ignore PDA",
                                pda_ptr->egprs_win_sz,pda_ptr->timeslot_alloc,global_data_ptr->current_egprs_dl_ws);
             return (pda_accept_flag);
           }
         }
         else
         {
           /*note Error in DL WS ignore PDA (only PTR need not specify the WS)*/
           MSG_GERAN_MED_0_G("MAC info:- in EGPRS. Rel99 features DON'T contain EGPRS data,ignore PDA");
           return (pda_accept_flag);
         }
       }
    }
    else /*gprs*/
    {
       if(pda_ptr->r99_additions_flag == valid && pda_ptr->egprs_options_flag == valid)
       {
         MSG_GERAN_MED_0_G("MAC info:- in GPRS. Rel99 features contain EGPRS data, igonre PDA");
         return (pda_accept_flag);
       }
    }

#ifdef FEATURE_GSM_DTM
  /* The value of MAC_MODE may be changed between DYNAMIC and EXTENDED DYNAMIC
       during transfer mode for 44.060 rel 6.2.0 onwards.*/
    transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.mac_mode = current_mac_mode;
#else
    /* MAC_MODE */
    if (pda_ptr->mac_mode == current_mac_mode)
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.mac_mode = pda_ptr->mac_mode;
    }
    else
    {
      /* Error in MAC_MODE - does not match UL - ignore PDA */
      return (pda_accept_flag);
    }
#endif /* FEATURE_GSM_DTM */
    /* RLC_MODE */
    if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
    {
      if(global_data_ptr->gmac_store_dl_rlc_mode != pda_ptr->rlc_mode)
      {

        /*IF t3192 is active and crtl_ack BIT in pda is set then a change of RLC mode is allowed*/
        if ( (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE) && (pda_ptr->ctrl_ack == 1) )
        {
          global_data_ptr->gmac_store_dl_rlc_mode = pda_ptr->rlc_mode;
        }
        else
        {

          MSG_GERAN_MED_2_G("MAC info:- Current rlc mode is %d and Recieved rlc mode %d ",
                            global_data_ptr->gmac_store_dl_rlc_mode,pda_ptr->rlc_mode);
          MSG_GERAN_MED_0_G("MAC info:- GMAC is releasing the downlink tbf locally because difference of rlc mode in PDA");
          if(transfer_data_ptr->mac_transfer_substate != TRANSFER_UL_DYNAMIC_DL_ASSIGN)
          {
            gmac_util_send_dl_release_to_rlc(gas_id);
          }
          gmac_transfer_dl_release_clean(gas_id);
          gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
          mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);
          global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
          return (pda_accept_flag);
        }

      }
      else
      {
       global_data_ptr->gmac_store_dl_rlc_mode = pda_ptr->rlc_mode;
      }
    }
    else
    {
      global_data_ptr->gmac_store_dl_rlc_mode = pda_ptr->rlc_mode;
    }
    /* CONTROL_ACK */
    /* If == 1 & if T3192 running (RLC) then consider old TBF released and act upon new assignment
    ** note if T3192 running then FAI=1 for current DL
    */

    global_data_ptr->current_rcvd_dl_ctrl_ack = pda_ptr->ctrl_ack;
    
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#else
    if (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE)
#endif
    {
      if (pda_ptr->ctrl_ack == 0)
      {
        /* Error in CONTROL_ACK - ignore PDA and inform network */
        MSG_GERAN_HIGH_0_G("MAC info PDA Ctrl Ack 0");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_NON_DIST_PART_ERR, PACKET_DOWNLINK_ASSIGNMENT, gas_id);
        return (pda_accept_flag);
      }
    }
    else
    {
      if (pda_ptr->ctrl_ack == 1)
      {  
        uint8 anite_gcf_flag;
        rr_read_public_store( RRPS_MS_ANITE_GCF_FLAG, &anite_gcf_flag,gas_id);
  
        if ((anite_gcf_flag == 0) &&
            (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC ||
             transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_DL_ASSIGN) )
        {
          global_data_ptr->current_rcvd_dl_ctrl_ack = 0;
          MSG_GERAN_ERROR_0_G("PDA Ctrl ack = 1 and T3192 not running when DL TBF not active, ignore ctrl ack");
        }
        else
        {              
          /* Error in CONTROL_ACK - ignore PDA and inform network */
          MSG_GERAN_HIGH_0_G("MAC info PDA Ctrl Ack 1");
          gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_NON_DIST_PART_ERR, PACKET_DOWNLINK_ASSIGNMENT, gas_id);
          return (pda_accept_flag);
        }
      }
    }

    if (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC)
    {
      global_data_ptr->gmac_local_dl_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
    }

    /* TIMESLOT_ALLOCATION */
    /* multislot class check */
#ifdef FEATURE_GSM_DTM
    if ( (current_mac_mode == MAC_L1_MAC_MODE_DYNAMIC) ||
         (current_mac_mode == MAC_L1_MAC_MODE_EXTENDED_DYNAMIC) )
#else
    if (current_mac_mode == MAC_L1_MAC_MODE_DYNAMIC)
#endif /*FEATURE_GSM_DTM*/
    {
      /* This is the case where in PUA was rcvd while in DL only state   */
      /* and before the UL assignment can be connected a PDA is rcvd,    */
      /* so the assignment TS rcvd in the UL have not yet been activated */
      /* As there is no UL connected yet                                 */
      uint8 gmac_ul_ts_alloc = 0;


      if (global_data_ptr->gmac_local_ul_ts_alloc == 0)
      {
        /*NO UL active, but pending connection so use pending */
        /*UL ts allocation in MS calculation                  */
        gmac_ul_ts_alloc = global_data_ptr->gmac_local_ul_realloc_ts;  
      }
      else
      {
        /*there is an active UL. use current active UL allocation*/
        /* in MS calculation                                     */
        gmac_ul_ts_alloc = global_data_ptr->gmac_local_ul_ts_alloc;
      }

#ifdef FEATURE_GPRS_PS_HANDOVER
      if ( gmac_util_multislot_class_check(gmac_ul_ts_alloc,
                                           pda_ptr->timeslot_alloc,
                                           current_mac_mode,
                                           global_data_ptr->gmac_dtm_cs_ts_alloc,
                                           transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                           gas_id) == TRUE )
#else
      if ( gmac_util_multislot_class_check(gmac_ul_ts_alloc,
                                           pda_ptr->timeslot_alloc,
                                           current_mac_mode,
                                           global_data_ptr->gmac_dtm_cs_ts_alloc,
                                           transfer_data_ptr->t_flags.dtm_active,
                                           gas_id) == TRUE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
      {
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ts_allocation = pda_ptr->timeslot_alloc;
      }
      else
      {
        /* ignore PDA */
        return (pda_accept_flag);
      }
    }
    else
    {
        /* ignore PDA */
        return (pda_accept_flag);
    }

    /* DOWNLINK_TFI_ASSIGNMENT */
    if (pda_ptr->dwnlk_tfi_flag)
    {
      global_data_ptr->gmac_re_assign_dl_tfi = pda_ptr->downlink_tfi;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi_present = TRUE;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi = pda_ptr->downlink_tfi;
    }
    else
    {
       if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
       {
          /*If DL active and no tfi in PDA msg then use current DL tfi*/
          MSG_GERAN_MED_1_G("DL active, DL TFI valid %d",global_data_ptr->gmac_local_tfi_store.dl_tfi);

          global_data_ptr->gmac_re_assign_dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
          transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi_present = TRUE;
          transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
       }
       else
       {
          /* ignore PDA */
          MSG_GERAN_HIGH_0_G("No DL currently active, invalid PDA Ass config");
          return (pda_accept_flag);
       }

    }
/*event logging changes*/
    mac_log_gprs_pda_event( pda_ptr->timeslot_alloc, global_data_ptr->gmac_re_assign_dl_tfi, gas_id );

    /* If TA params not changed, do not send */
    if (gmac_update_local_ta_params(&(pda_ptr->pkt_timing_adv), gas_id))
    {
      (void)gmac_packet_ta_ie(&(pda_ptr->pkt_timing_adv), &(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ta_params), gas_id);
    }
    else
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ta_params.ta_value_ie.present = FALSE;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ta_params.cont_ta_ie.present = FALSE;
    }

    if (pda_ptr->dwnlk_pwr_ctrl_flag == pwr_ctrl_valid)
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.downlink_pwr.params_valid = TRUE;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.downlink_pwr.po = pda_ptr->dwnlk_pwr_ctrl.po;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.downlink_pwr.bts_pwr_ctrl_mode = pda_ptr->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.downlink_pwr.pr_mode = pda_ptr->dwnlk_pwr_ctrl.pr_mode;
    }
    else
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.downlink_pwr.params_valid = FALSE;
    }

    /* always send parameters to L1 */
    if (pda_ptr->pwr_ctrl_flag == valid)
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control.present = TRUE;
      gmac_power_control_params_ie(&(pda_ptr->pwr_ctrl_paramtrs), &(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control),
                                     gas_id);
    }
    else
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control.present = FALSE;
    }

    /* TBF Starting Time */
    if (pda_ptr->tbf_start_flag)
    {
      if (gmac_util_tbf_starting_ie(&(pda_ptr->tbf_start_frm_num),&(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_start_time),TRUE,gas_id))
      {
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_start_time_valid = TRUE;
      }
      else
      {
        /* error in starting time - ignore PDA */
        return (pda_accept_flag);
      }
    }
    else
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_start_time_valid = FALSE;
    }

    if (pda_ptr->egprs_options_flag == valid)
    {

      /****************************************************************
      *             ADD EGPRS params to  MAC_L1_DL_TBF_CONFIG         *
      ****************************************************************/

      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.rlc_mode = (mac_rlc_mode_T)pda_ptr->rlc_mode;

      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.link_qual_meas_mode = pda_ptr->egprs_link_qual;

      if (pda_ptr->egprs_bep_prd2_flag == valid)
      {
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.bep_period2.bep_period2 = pda_ptr->egprs_bep_prd2;
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.bep_period2.present = TRUE;
      }
      else
      {
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.bep_period2.present = FALSE;
      }
    }




    /* Measurement Mapping */
    /* WARNING : Not applicable to this release and is ignored */

    /* R99 extension : EGPRS, Extended TA, COMPACT */
    /* WARNING : Not applicable to this release and is ignored */

    /* FREQ PARAMS IE */
    if (pda_ptr->freq_param_flag == valid)
    {
      freq_param_result = gmac_util_frequency_param_ie(TRUE,TRUE,gas_id);
      if (freq_param_result == NO_ERRORS)
      {
        rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
        pda_accept_flag = TRUE;
      }
      else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
           mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
        return (pda_accept_flag);
      }
      else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
      {
        gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
           mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
        return (pda_accept_flag);
      }
      else if (freq_param_result == FREQ_ERROR_DIFFERENT_PARAMS)
      {
        /* ignore PDA */
        return (pda_accept_flag);
      }
      else
      {
        /* unknown error type */
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
           mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
        return (pda_accept_flag);
      }
    }
    else
    {
      pda_accept_flag = TRUE;

      rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
      gmac_transfer_send_stored_dl_config_to_l1(gas_id);

      /*TLLI in DL CNF to RLC*/
      /*DL config to RLC only sent id current DL tbf is being released */
      if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
      {
        gmac_util_send_dl_ass_to_rlc(gas_id);
        transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
      }


    }


    /*Featured dl_tbf_est logging. Consider both feature and both option in EDGE*/

    if (pda_ptr->egprs_bep_prd2_flag == valid)
    {
      log_bep_prd2 = transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.bep_period2.bep_period2;
    }
    else
    {
      log_bep_prd2 = BEP_PERIOD2_FIELD_NOT_SPECIFIED;
    }

    if (pda_ptr->egprs_options_flag)
    {
      mac_log_dl_etbf_establish(
        global_data_ptr->gmac_re_assign_dl_tfi,
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ts_allocation,
        global_data_ptr->gmac_store_dl_rlc_mode,
        (byte) pda_ptr->egprs_options_flag,
        global_data_ptr->current_egprs_dl_ws,
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.link_qual_meas_mode,
        (uint8)log_bep_prd2,
        gas_id);
    }
    else
    {
      mac_log_dl_tbf_establish(
        global_data_ptr->gmac_re_assign_dl_tfi,
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ts_allocation,
        global_data_ptr->gmac_store_dl_rlc_mode,
        gas_id);
    }
  }
  return (pda_accept_flag);
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_ptr
===
===  DESCRIPTION
===  Handles PACKET TIMESLOT RECONFIGURE received during
===  uplink/downlink dynamic packet transfer
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if fixed allocation, abnormal release with retry
===  - if does not define UL and DL PDCH, abnormal release with retry
===  - violates multislot capabilities, abnormal release with retry
===  - failure for any other reason, abnormal release with retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===  TRUE if PTR accepted
===  FALSE if PTR rejected
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_tdynamic_handle_ptr(gas_id_t gas_id)
{
  packet_timeslot_reconfigure_t *ptr_ptr;
  uint16 assigned_dl_ws =0;
  uint16 assigned_ul_ws =0;
  pua_data_error_t        freq_param_result;
  boolean                 ptr_accept_flag = FALSE;

  uint8 ts_alloc_bitmap = 0;
  uint8 usf_mask = 0x01;
  uint8 ts_mask = 0x80;
  uint8 i;

  /*TLLI in DL CNF to RLC*/
  uint32 local_rrps_tlli;
  /*for logging only*/
  uint32 old_local_rrps_tlli;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
  rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli,gas_id);

  /* initialise */
  ptr_ptr = (packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr;

  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    /* if stored TFI = received TFI then match else ignore message */
    if (gmac_util_global_tfi(ptr_ptr->global_tfi,gas_id))
    {
      MSG_GERAN_MED_0_G("MAC Rcvd PACKET_TIMESLOT_RECONFIGURE");

      /*TLLI in DL CNF to RLC*/
      /*with PTR will MAC always send RLC a DL config as PTR must have a DL_TFI and*/
      /* don't do this    rr_read_public_store(RRPS_TLLI,&local_rrps_tlli);
         don't do this    gmac_store_dl_ass_tlli = local_rrps_tlli; */
      if(ptr_ptr->global_tfi.direction == uplink_valid)
      {
      MSG_GERAN_HIGH_1_G("PACKET_TIMESLOT_RECONFIGURE addressed by ul tfi %d",ptr_ptr->global_tfi.uplink_tfi);
      }
      else if(ptr_ptr->global_tfi.direction == dwnlink_valid)
      {
      MSG_GERAN_HIGH_1_G("PACKET_TIMESLOT_RECONFIGURE addressed by dl tfi %d",ptr_ptr->global_tfi.dwnlink_tfi);
      }

      if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE)
      {
        /*There is no DL TBF currently active, PTR addressed by UL TFI*/
        global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
      }
      /*TLLI in DL CNF to RLC*/
      MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x ,o_tlli %x",
                         global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);

      if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
      {
        MSG_GERAN_HIGH_0_G("MAC info PTR ME");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_TIMESLOT_RECONFIGURE, gas_id);
        return (ptr_accept_flag);
      }

#ifdef FEATURE_GSM_DTM
      if ( ( transfer_data_ptr->t_flags.dtm_active ) &&
           ( ptr_ptr->freq_param_flag == valid ) )
      {
        MSG_GERAN_HIGH_0_G("GMAC, Freq Params rxd in PTR whilst in DTM, AB NORM REL");
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
        {
          mac_log_ul_tbf_release(MAC_LOG_UL_FREQ_ERROR_ACCESS_RETRY, gas_id);
        }

        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
        {
          mac_log_dl_tbf_release(MAC_LOG_DL_FREQ_ERROR_ACCESS_RETRY, gas_id);
        }
        return (ptr_accept_flag);
      }
#endif /*FEATURE_GSM_DTM*/

      /*check for GPRS/EGRPS tbf mode conflicts */
      if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS && ptr_ptr->egprs_ptr == not_valid)
      {
       MSG_GERAN_MED_0_G("MAC info ERROR PTR assigned GPRS in tbf mode EGPRS. ignore PTR");
       return (ptr_accept_flag);
      }

      if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS && ptr_ptr->egprs_ptr == valid)
      {
       MSG_GERAN_MED_0_G("MAC info ERROR PTR assigned EGPRS in tbf mode GPRS. ignore PTR");
       return (ptr_accept_flag);
      }

      /* FIXED OR DYNAMIC ALLOC STRUCT */
      if (ptr_ptr->alloc_flag == dynamic)
      {
        /*Check for PTR with no timeslots allocated*/
        if ( (ptr_ptr->dynamic_alloc.usf_bitmap == 0) ||
             (ptr_ptr->dwnlk_ts_alloc == 0) )
        {
          MSG_GERAN_HIGH_2_G("GMAC, PTR has a timeslot alloc of 0, UL TS alloc:0x%x, DL TS alloc:0x%x, ignore msg",
                             ptr_ptr->dynamic_alloc.usf_bitmap,ptr_ptr->dwnlk_ts_alloc);
          return (ptr_accept_flag);
        }

#ifdef FEATURE_GSM_DTM
        /* Do not inhibit EDA when FEATURE_GSM_DTM is featured */
#else
        /* extended dynamic allocation is NOT suuported - ignore PUA */
        if (ptr_ptr->dynamic_alloc.ext_dynamic_alloc == (uint8)valid)
        {
          return (ptr_accept_flag);
        }
#endif /*FEATURE_GSM_DTM*/

        /* clear the data store for the ts_reconfig msg */
        memset( &transfer_data_ptr->gmac_transfer_store_ts_reconfig, 0, sizeof(mac_l1_ts_reconfig_t));

#ifdef FEATURE_GSM_DTM
        if ( ptr_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
        {
          transfer_data_ptr->gmac_transfer_store_ts_reconfig.uplink_tbf_type = MAC_L1_MAC_MODE_EXTENDED_DYNAMIC;
        }
        else
        {
          transfer_data_ptr->gmac_transfer_store_ts_reconfig.uplink_tbf_type = MAC_L1_MAC_MODE_DYNAMIC;
        }
#else
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.uplink_tbf_type = MAC_L1_MAC_MODE_DYNAMIC;
#endif /*FEATURE_GSM_DTM*/

#ifdef FEATURE_GSM_EDTM
        /*store current mac mode for multislot checking when DTM Ass Cmd
          does not contain any PS */
        global_data_ptr->gmac_current_mac_mode = transfer_data_ptr->gmac_transfer_store_ts_reconfig.uplink_tbf_type;
#endif /*FEATURE_GSM_EDTM*/

        if ((gmac_pkt_tmslt_reconfig_dynamic_alloc_ie(&transfer_data_ptr->gmac_transfer_store_ts_reconfig, gas_id)) == FALSE)
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_DATA_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_ASSIGNMENT_DATA_ERROR,gas_id);}
          return (ptr_accept_flag);
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("MAC error PTR fixed alloc");
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
           mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
           mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
        return (ptr_accept_flag);
      }

      /* GLOBAL PACKET TA IE */
      gmac_global_packet_ta_ie(&(ptr_ptr->global_pkt_timing), 
                               &(transfer_data_ptr->gmac_transfer_store_ts_reconfig.global_packet_ta),
                                gas_id);
      /* DOWNLINK RLC MODE */
      if(global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
      {
        if(global_data_ptr->gmac_store_dl_rlc_mode != ptr_ptr->dwnlk_rlc_mode)
        {

          /*IF t3192 is active and crtl_ack BIT in pda is set then a change of RLC mode is allowed*/
          if ( (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE) && (ptr_ptr->control_ack == 1) )
          {
            global_data_ptr->gmac_store_dl_rlc_mode = ptr_ptr->dwnlk_rlc_mode;
          }
          else
          {
            MSG_GERAN_MED_2_G("MAC info:- Current rlc mode is %d and Recieved rlc mode is %d ",
                              global_data_ptr->gmac_store_dl_rlc_mode,ptr_ptr->dwnlk_rlc_mode);
            MSG_GERAN_MED_0_G("MAC info:- GMAC is releasing the downlink and uplink tbf locally because difference of rlc mode in PDA");

            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

            if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
                mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE,gas_id);}
            if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
                mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);}
            return (ptr_accept_flag);
          }

        }
        else
        {
          global_data_ptr->gmac_store_dl_rlc_mode = ptr_ptr->dwnlk_rlc_mode;
        }
      }
      else
      {
        global_data_ptr->gmac_store_dl_rlc_mode = ptr_ptr->dwnlk_rlc_mode;
      }

      /* CONTROL_ACK */
      /* If == 1 & if T3192 running (RLC) then consider old TBF released and act upon new assignment
      ** note if T3192 running then FAI=1 for current DL
      */
      
      global_data_ptr->current_rcvd_dl_ctrl_ack = ptr_ptr->control_ack;
    
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
      #error code not present
#else
      if (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE)
#endif
      {
        if (ptr_ptr->control_ack == 0)
        {
          /* Error in CONTROL_ACK - ignore PTR and inform network */
          MSG_GERAN_HIGH_0_G("MAC info PTA Ctrl Ack 0");
          gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_NON_DIST_PART_ERR, PACKET_TIMESLOT_RECONFIGURE,gas_id);
          return (ptr_accept_flag);
        }
      }
      else
      {
        if (ptr_ptr->control_ack == 1)/* CR 105837 */
        {
          uint8 anite_gcf_flag;
          rr_read_public_store( RRPS_MS_ANITE_GCF_FLAG, &anite_gcf_flag,gas_id);
  
          if ( (anite_gcf_flag == 0) &&
               (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC ||
                transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC ||
                transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_DL_ASSIGN ||
                transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN) )
          {
            global_data_ptr->current_rcvd_dl_ctrl_ack = 0;
            MSG_GERAN_ERROR_0_G("PTR Ctrl ack = 1 and T3192 not running when DL TBF not active, ignore ctrl ack");
          }
          else
          {
            MSG_GERAN_MED_0_G("MAC info:- control ack is set to one when T3192 is not running. so do abnormal release");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

            if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
                mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE,gas_id);}
            if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
                mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);}
            return (ptr_accept_flag);
          }
        }
      }

      /* DOWNLINK_TFI_ASSIGNMENT */
      if (ptr_ptr->dwnlk_tfi_assignment_flag == valid)
      {
        global_data_ptr->gmac_re_assign_dl_tfi = ptr_ptr->dwnlk_tfi_assignment;
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_tfi_present = TRUE;
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_tfi = ptr_ptr->dwnlk_tfi_assignment;
      }
      else
      {
        /* if in concurrency then the dl_tfi_valid should be valid (ie a TFI has already been assigned OR*/

        /* The MAC UL xfer states are checked because when a PTR is received in these states we should be
           assigning a DL TFI ready to go into concurrency

           Note: This code was put in to handle a specific case:
           In DL xfer a PUA is received which assigns a UL TFI.
           Before the Connect IND a PTR is received (addressed by DL TFI)
           which does NOT assign the UL TFI.  In this case we should ab rel with retry.
           But we carry on instead (the dl_tfi_valid is TRUE so we don't drop into the "if " below.
           The states are checked below so that in a non-current UL state we must expect a DL TFI*/


        if  ( ( global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == FALSE ) ||
              ( transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC ) ||
              ( transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC ) ||
              ( transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN ) ||
              ( transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_DL_ASSIGN ) )
        {
          MSG_GERAN_ERROR_0_G("MAC error NO DL TFI in PTR");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_ADDRESSING_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_ASSIGNMENT_ADDRESSING_ERROR,gas_id);}
          return (ptr_accept_flag);
        }
        else
        {
          global_data_ptr->gmac_re_assign_dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
          transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_tfi_present = FALSE;
        }
      }

      /* UPLINK_TFI_ASSIGNMENT */
      if (ptr_ptr->uplk_tfi_assignment_flag == valid)
      {
         global_data_ptr->gmac_re_assign_ul_tfi = ptr_ptr->uplk_tfi_assignment;
      }
      else
      {
        /* if in concurrency then the ul_tfi_valid should be valid (ie a TFI has already been assigned OR*/

        /* The MAC DL xfer states are checked because when a PTR is received in these states we should be
           assigning an UL TFI ready to go into concurrency

           Note: This code was put in to add completeness to the DL case above. */

        if ( ( global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == FALSE ) ||
             ( transfer_data_ptr->mac_transfer_substate == TRANSFER_DL ) ||
             ( transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_REASSIGN ) ||
             ( transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_WITH_DYNAMIC_ALLOC ) ||
             ( transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC ) )
        {
          MSG_GERAN_ERROR_0_G("MAC error NO UL TFI in PTR");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_ASSIGNMENT_ADDRESSING_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_ASSIGNMENT_ADDRESSING_ERROR,gas_id);}
          return (ptr_accept_flag);
        }
        else
        {
          global_data_ptr->gmac_re_assign_ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
        }
      }

      /* MULTISLOT CLASS checking */

      /* reverse usf_bitmap to coincide with TS bitmap */
      for (i=0;i<8;i++)
      {
        if (ptr_ptr->dynamic_alloc.usf_bitmap & usf_mask)
        {
          ts_alloc_bitmap |= ts_mask;
        }
        usf_mask <<= 1;
        ts_mask >>= 1;
      }

      if ( (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL) ||
           (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_REASSIGN) )
      {
        global_data_ptr->gmac_local_ul_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
      }
      else if ( (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC) ||
                (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC) )
      {
        global_data_ptr->gmac_local_dl_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
      }

#ifdef FEATURE_GPRS_PS_HANDOVER
      if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                           ptr_ptr->dwnlk_ts_alloc,
                                           ptr_ptr->dynamic_alloc.ext_dynamic_alloc,
                                           global_data_ptr->gmac_dtm_cs_ts_alloc,
                                           transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                           gas_id) == TRUE )
#else
      if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                           ptr_ptr->dwnlk_ts_alloc,
                                           ptr_ptr->dynamic_alloc.ext_dynamic_alloc,
                                           global_data_ptr->gmac_dtm_cs_ts_alloc,
                                           transfer_data_ptr->t_flags.dtm_active,
                                           gas_id) == TRUE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
      {
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_ts_allocation = ptr_ptr->dwnlk_ts_alloc;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("MAC error PTR multislot class");
        /* abnormal release with access retry */
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_MULTISLOT_ERROR,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_MULTISLOT_ERROR,gas_id);}
        return (ptr_accept_flag);
      }

/*event logging changes*/
      mac_log_ptr_event(ts_alloc_bitmap, ptr_ptr->dwnlk_ts_alloc, global_data_ptr->gmac_re_assign_ul_tfi, global_data_ptr->gmac_re_assign_dl_tfi, gas_id);

      if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
#ifdef FEATURE_GSM_EDTM
        /*store ws prior to reassignment, used to restore ws when DTM Ass is received with no PS defined*/
        global_data_ptr->gmac_edtm_old_ul_ws = global_data_ptr->current_egprs_ul_ws;
        global_data_ptr->gmac_edtm_old_dl_ws = global_data_ptr->current_egprs_dl_ws;
#endif /*FEATURE_GSM_EDTM*/

        /**************   check to see if DL WS is valid   ********************/
        if(ptr_ptr->dl_egprs_window_size_flag == valid)
        {
          /*Check validity of DL ws*/
          assigned_dl_ws = gmac_egprs_decode_dl_window_size(ptr_ptr->dl_egprs_window_size, ptr_ptr->dwnlk_ts_alloc,gas_id);

          /*When in transfer and there is no valid DL WS then there is no DL TBF*/
          if(assigned_dl_ws == WS_ALLOCATION_INVALID)
          {
            MSG_GERAN_MED_0_G("MAC info ERROR in ass dl ws ");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

            if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
              mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
            if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
              mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
            return (ptr_accept_flag);
          }
          else
          {
            global_data_ptr->current_egprs_dl_ws = assigned_dl_ws;
          }
        }
        else
        {
          if(global_data_ptr->current_egprs_dl_ws == WS_ALLOCATION_INVALID)
          {
            /*select default. ws = 64 ref 4.60 9.1.9.2 */
            global_data_ptr->current_egprs_dl_ws = 64;
          }
          else
          {
            /*if no valid dl ws and there is a active dl tbf then */
            /*send rlc the current dl ws value                    */
          }
        }

        /**************   check to see if UL WS is valid   ********************/
        if(ptr_ptr->ul_egprs_window_size_flag == valid)
        {
          assigned_ul_ws = gmac_egprs_decode_ul_window_size(ptr_ptr->ul_egprs_window_size,ptr_ptr->dynamic_alloc.usf_bitmap,gas_id);

          /*When in transfer and there is no valid UL WS then there is no UL TBF*/
          if(assigned_ul_ws == WS_ALLOCATION_INVALID)
          {
            MSG_GERAN_MED_0_G("MAC info ERROR in ass ul ws ");
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

            if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
              mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
            if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
              mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
            return (ptr_accept_flag);
          }
          else
          {
            global_data_ptr->current_egprs_ul_ws = assigned_ul_ws;
          }
        }
        else
        {
          if(global_data_ptr->current_egprs_ul_ws == WS_ALLOCATION_INVALID)
          {
            /*select default. ws = 64 ref 4.60 9.1.9.2 */
            global_data_ptr->current_egprs_ul_ws = 64;
          }
          else
          {
            /*if no valid dl ws and there is a active dl tbf then */
            /*send rlc the current dl ws value                    */
          }
        }

        /**** Check EGPRS MCS validity ******/

        if ( gmac_util_egprs_mcs_valid(ptr_ptr->egprs_chan_coding_cmd,gas_id) == FALSE )
        {
          MSG_GERAN_ERROR_0_G("MAC info ERROR PTR contain invalid EGPRS MCS, ab rel ");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
          return (ptr_accept_flag);
        }

        MSG_GERAN_MED_0_G("MAC info EGPRS ass");

        /***************************************************************************
        ********** Assigned EGPRS UL, add EGPRS L1 UL PARAMTERS to Ul config *******
        ***************************************************************************/

        transfer_data_ptr->gmac_transfer_store_ts_reconfig.tbf_mode = TBF_MODE_EGPRS;
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.link_qual_meas_mode = ptr_ptr->link_qual_meas_mode;

        /*L1 needs to know RLC mode to determine if IR applicable */
        transfer_data_ptr->gmac_transfer_store_ts_reconfig.rlc_mode = (mac_rlc_mode_T)ptr_ptr->dwnlk_rlc_mode;

      }



      /* R99 EXTENSION : EXTENDED TA */
      /* WARNING : Not applicable to this release and is ignored */

      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;

      /* FREQ PARAMS IE */
      if (ptr_ptr->freq_param_flag == valid)
      {
        freq_param_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
        if (freq_param_result == NO_ERRORS)
        {
          /* if the freq_param result is true, keep the stored ts_reconf info */
          /* and wait for L1_MAC_FREQ_ASS_CNF before sending ts_reconfig */
          rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
          ptr_accept_flag = TRUE;
        }
        else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
          return (ptr_accept_flag);
        }
        else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
        {
          gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
          return (ptr_accept_flag);
        }
        else
        {
          /* unknown error type */
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
          return (ptr_accept_flag);
        }
      }
      else
      {
        ptr_accept_flag = TRUE;

        rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
        gmac_transfer_send_stored_ts_reconfig_to_l1(gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
        {
          mac_log_ul_tbf_establish(global_data_ptr->gmac_re_assign_ul_tfi,
                                   ts_alloc_bitmap,
                                   gas_id);
        }

        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
        {
          /*Featured dl_tbf_est logging. Consider both feature and both option in EDGE. Step F: one thing to check -ask Tony */

          if (ptr_ptr->egprs_ptr)
          {
            mac_log_dl_etbf_establish(
              global_data_ptr->gmac_re_assign_dl_tfi,
              transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_ts_allocation,
              global_data_ptr->gmac_store_dl_rlc_mode,
              (byte) ptr_ptr->egprs_ptr,
              global_data_ptr->current_egprs_dl_ws,
              transfer_data_ptr->gmac_transfer_store_ts_reconfig.link_qual_meas_mode,
              BEP_PERIOD2_FIELD_NOT_SPECIFIED,
              gas_id);
          }
          else
          {
            mac_log_dl_tbf_establish(
              global_data_ptr->gmac_re_assign_dl_tfi,
              transfer_data_ptr->gmac_transfer_store_ts_reconfig.dl_ts_allocation,
              global_data_ptr->gmac_store_dl_rlc_mode,
              gas_id);
          }
        }

        /*TLLI in DL CNF to RLC*/
        /*PTR is only addressable by TFI, there is no need to inform RLC if PTR  */
        /*re-assigns the DL except in the instance that the DL is being released.*/
        if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
        {
          gmac_util_send_dl_ass_to_rlc(gas_id);
          transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
        }
      }

      /* Store data for sending UL_ALLOC_CNF to RLC when CONNECT_IND is received from L1 */

      memset(&(transfer_data_ptr->gmac_transfer_ul_rlc_info_store), 0, sizeof(mr_ul_alloc_cnf_t));

      if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
         /* if we get here than this no problem in the egprs prams rcvd in the PUA */
/*fix for cs change handling*/

         MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, ptr_chan_coding_cmd %d",
                           transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                           global_data_ptr->gmac_local_cs_setting,ptr_ptr->egprs_chan_coding_cmd);

         /* ptr_accept_flag == TRUE */
         if (global_data_ptr->gmac_local_cs_setting != ptr_ptr->egprs_chan_coding_cmd)
         {
           transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
           transfer_data_ptr->pua_ptr_cs_change = ptr_ptr->egprs_chan_coding_cmd;
         }

         /***************************************************************************
         ******** Assigned EGPRS UL, add EGPRS RLC UL PARAMTERS to Ul config ********
         ***************************************************************************/

         MSG_GERAN_MED_3_G("MAC info MCS =%d, Reseg =%d, ul_ws =%d",
                           ptr_ptr->egprs_chan_coding_cmd,ptr_ptr->resegment,global_data_ptr->current_egprs_ul_ws);

         transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_EGPRS;
         transfer_data_ptr->gmac_transfer_ul_rlc_info_store.mcs = ptr_ptr->egprs_chan_coding_cmd;
         transfer_data_ptr->gmac_transfer_ul_rlc_info_store.resegment = ptr_ptr->resegment;
         transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ws = global_data_ptr->current_egprs_ul_ws;
      }
      else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
      {
        /* ptr_accept_flag == TRUE */
        if (global_data_ptr->gmac_local_cs_setting != ptr_ptr->chan_coding_cmd)
        {
          transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
          transfer_data_ptr->pua_ptr_cs_change = ptr_ptr->chan_coding_cmd;
        }

        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_GPRS;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.cs = ptr_ptr->chan_coding_cmd;
      }
      else
      {
          MSG_GERAN_ERROR_1_G("Invalid TBF mode = %d",global_data_ptr->current_tbf_mode);
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
             mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
             mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
          return (ptr_accept_flag);
      }

       /***************************************************************************
       ******** Assigned EGPRS DL, add EGPRS RLC DL PARAMTERS to Dl config ********
       ***************************************************************************/

       /*note this is done in function gmac_util_send_dl_ass_to_rlc() when PH_CONNECT_IND is rcvd*/
#ifdef FEATURE_GSM_DTM
      if ( ptr_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
      {
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_EXT_DYNAMIC_ALLOC;
      }
      else
      {
          transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
      }
#else
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
#endif /*FEATURE_GSM_DTM*/

      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_phase = GMAC_NO_PHASE_ACCESS;
      /*gmac_transfer_ul_rlc_info_store.cs = ptr_ptr->chan_coding_cmd;*/
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tlli_block_cs = RM_TLLI_USE_CS_COMMANDED;
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.num_timeslots = gmac_utils_calculate_nts(ptr_ptr->dynamic_alloc.usf_bitmap,
                                                                                                  gas_id);
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.usf_granularity = ptr_ptr->dynamic_alloc.usf_granularity;

      if(ptr_ptr->dynamic_alloc.rlc_data_blks_granted_flag == valid)
      {
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_CLOSE_ENDED_TBF;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = ptr_ptr->dynamic_alloc.rlc_data_blks_granted + 9;
      }
      else
      {
        /* NOTE : MR_UL_ALLOC_CNF blocks_granted is for closed TBF only */
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_OPEN_ENDED_TBF;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = 0;
      }

      /* NOTE : MR_UL_ALLOC_CNF final_alloc is for fixed only */
      /* NOTE : MR_UL_ALLOC_CNF ti is for access only */
      /* NOTE : MR_UL_ALLOC_CNF tlli is for access only */
      /* NOTE : MR_UL_ALLOC_CNF retry is for access only */

      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.sig = MR_UL_ALLOC_CNF;
    }
  }

  if ( ptr_accept_flag == TRUE )
  {
    grlc_dl_start_t3190(gas_id);
  }

  return (ptr_accept_flag);
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_pua
===
===  DESCRIPTION
===  Handles packet uplink assignment during uplink dynamic TBF
===  This leads to uplink dynamic reassignment
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if fixed allocation assigned, abnormal release with retry
===  - if does not properly define UL PDCH, abnormal release with retry
===  - violates multislot class, abnormal release with retry
===  - failure for any other reason, abnormal release with retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===  TRUE if PUA accepted
===  FALSE if PUA rejected
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean gmac_tdynamic_handle_pua(gas_id_t gas_id)
{
  packet_uplink_assignment_t    *pua_ptr;

  mac_l1_sig_t                  mac_l1_sig_uldcf;
  mac_dynamic_tbf_information_T *ul_tbf_msg_ptr = &(mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.dynamic_tbf);

  uint8                         addr_match = FALSE;
  boolean                       pua_accept_flag = FALSE;
  pua_data_error_t              freq_param_result;

  uint8 ts_alloc_bitmap = 0;
  uint8 usf_mask = 0x01;
  uint8 ts_mask = 0x80;
  uint8 i;

  uint16 log_BEP_prd2;
  uint16 assigned_ul_ws =0;

  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  pua_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

  /* ADDRESS GLOBAL TFI, TLLI, TQI, PRR */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {


    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {

      switch (pua_ptr->tlli_gtfi_pkt_ref_valid)
      {
        case tqi_valid:
        case req_ref_valid:
          /* TQI and PRRef not expected during transfer - address error */
          break;

        case tlli_valid:
          /* NOTE: It is possible that addressing using TLLI during transfer
          **       is not valid but at present it is treated as valid.
          */
          addr_match = gmac_match_old_or_new_tlli(pua_ptr->tlli, gas_id);
          break;

        case global_tfi_valid:
          /* if stored TFI = received TFI then match else ignore message */
          addr_match = gmac_util_global_tfi(pua_ptr->global_tfi, gas_id);
          break;

        default:
          break;
      }

    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }



    if (addr_match)
    {
      MSG_GERAN_MED_1_G("MAC Rcvd PACKET_UPLINK_ASSIGNMENT UL TFI %d",global_data_ptr->gmac_local_tfi_store.ul_tfi);

      if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
      {
        MSG_GERAN_LOW_0_G("MAC info PUA ME");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_UPLINK_ASSIGNMENT, gas_id);
        return (pua_accept_flag);
      }

#ifdef FEATURE_GSM_DTM
      if ( ( transfer_data_ptr->t_flags.dtm_active ) &&
           ( pua_ptr->freq_param_flag == valid ) )
      {
        MSG_GERAN_HIGH_0_G("GMAC, Freq Params rxd in PUA whilst in DTM, AB NORM REL");
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
        {
          mac_log_ul_tbf_release(MAC_LOG_UL_FREQ_ERROR_ACCESS_RETRY,gas_id);
        }

        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
        {
          mac_log_dl_tbf_release(MAC_LOG_DL_FREQ_ERROR_ACCESS_RETRY,gas_id);
        }
        return (pua_accept_flag);
      }
#endif /*FEATURE_GSM_DTM*/

      /*check for GPRS/EGRPS tbf mode conflicts */
      if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS && pua_ptr->egprs_pua == not_valid)
      {
       MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS tbf mode EGPRS ignore PUA");
       return (pua_accept_flag);
      }

      if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS && pua_ptr->egprs_pua == valid)
      {
       MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS tbf mode EGPRS ignore PUA");
       return (pua_accept_flag);
      }

      /* DYNAMIC/FIXED/SINGLE BLOCK/EXTENSION STRUCT */
      if (pua_ptr->allocation_flag == dynamic_alloc_e)
      {

#ifdef FEATURE_GSM_DTM
        /* Do not inhibit EDA when FEATURE_GSM_DTM is featured */
#else
        /* extended dynamic allocation is NOT supported */
        if (pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC)
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MOD,gas_idE);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
          return (pua_accept_flag);
        }
#endif /*FEATURE_GSM_DTM*/

        /*store current mac mode */
        if ( pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
        {
          global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_EXTENDED_DYNAMIC_ALLOC;
        }
        else
        {
          global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
        }

       /*Check for PUA with no timeslots allocated*/
        if ( pua_ptr->dynamic_alloc.usf_bitmap == 0 )
        {
          MSG_GERAN_HIGH_0_G("GMAC, PUA has a timeslot alloc of 0, ignore msg");
          return (pua_accept_flag);
        }

        /* reverse usf_bitmap to coincide with TS bitmap */
        for (i=0;i<8;i++)
        {
          if (pua_ptr->dynamic_alloc.usf_bitmap & usf_mask)
          {
            ts_alloc_bitmap |= ts_mask;
          }
          usf_mask <<= 1;
          ts_mask >>= 1;
        }
/*event logging changes*/
        mac_log_gprs_pua_event(ts_alloc_bitmap, pua_ptr->chan_coding_cmd, gas_id );

#ifdef FEATURE_GSM_DTM
        /* gmac_local_dl_ts_alloc is reset here because the TS allocation is not
           reset when exiting from concurrency */

        if((transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC)|| 
           (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC ))
        {
          global_data_ptr->gmac_local_dl_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
        }

#else
        /*For dyn alloc ,when no DL currently active, then DL timeslots are the same as the UL */
        if((transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC)|| (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC ))
        {
          global_data_ptr->gmac_local_dl_ts_alloc = ts_alloc_bitmap;
        }
#endif /*FEATURE_GSM_DTM*/

        /* multislot class check */
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                             global_data_ptr->gmac_local_dl_ts_alloc,
                                             pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                             global_data_ptr->gmac_dtm_cs_ts_alloc,
                                             transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                             gas_id) == FALSE )
#else
        if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                             global_data_ptr->gmac_local_dl_ts_alloc,
                                             pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                             global_data_ptr->gmac_dtm_cs_ts_alloc,
                                             transfer_data_ptr->t_flags.dtm_active,
                                             gas_id) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_MULTISLOT_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_MULTISLOT_ERROR,gas_id);}
          return (pua_accept_flag);
        }

       /* starting time */
        if (pua_ptr->dynamic_alloc.tbf_start_time_flag == valid)
        {
          if (gmac_util_tbf_starting_ie(&(pua_ptr->dynamic_alloc.tbf_start_frm_num), &(ul_tbf_msg_ptr->starting_time),TRUE,gas_id) == FALSE)
          {
            /* Error with TBF Starting time - abort UL */
            gmac_transfer_ul_tbf_abort_alloc(gas_id);
            return (pua_accept_flag);
          }
          ul_tbf_msg_ptr->tbf_start_time_valid = TRUE;;
        }
        else
        {
          ul_tbf_msg_ptr->tbf_start_time_valid = FALSE;
          ul_tbf_msg_ptr->starting_time = 0;
        }

        if (pua_ptr->dynamic_alloc.uplk_tfi_assignment_flag == valid)
        {
          global_data_ptr->gmac_re_assign_ul_tfi = pua_ptr->dynamic_alloc.uplk_tfi_assignment;
        }
        else
        {
          global_data_ptr->gmac_re_assign_ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
        }

        gmac_global_dynamic_alloc_ie(ul_tbf_msg_ptr, pua_ptr, gas_id);

        /* PACKET TA IE */
        /* If TA params not changed, do not send */
        if (gmac_update_local_ta_params(&(pua_ptr->pkt_timing_adv),gas_id))
        {
          (void)gmac_packet_ta_ie(&(pua_ptr->pkt_timing_adv), &(ul_tbf_msg_ptr->timing_advance_params),gas_id);
        }
        else
        {
          ul_tbf_msg_ptr->timing_advance_params.ta_value_ie.present = FALSE;
          ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.present = FALSE;
        }
      }
      else
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
          mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
          mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
        return (pua_accept_flag);
      }

      if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
#ifdef FEATURE_GSM_EDTM
        /*store ws prior to reassignment, used to restore ws when DTM Ass is received with no PS defined*/
        global_data_ptr->gmac_edtm_old_ul_ws = global_data_ptr->current_egprs_ul_ws;
#endif /*FEATURE_GSM_EDTM*/

        /**************   check to see if WS is valid   ********************/
        assigned_ul_ws = gmac_egprs_decode_ul_window_size(pua_ptr->egprs_window_size, pua_ptr->dynamic_alloc.usf_bitmap,
                                                          gas_id);

        /*If an invalid WS has been allocated abnormal release*/
        if(assigned_ul_ws == WS_ALLOCATION_INVALID)
        {
          MSG_GERAN_MED_0_G("MAC info ERROR in ass ul ws ");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
          return (pua_accept_flag);
        }
        else
        {
          global_data_ptr->current_egprs_ul_ws = assigned_ul_ws;
        }

        /**** Check EGPRS MCS validity ******/

        if ( gmac_util_egprs_mcs_valid(pua_ptr->egprs_chan_coding_cmd,gas_id) == FALSE )
        {
          MSG_GERAN_ERROR_0_G("MAC info ERROR PUA contains invalid EGPRS MCS, ab rel ");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
          return (pua_accept_flag);
        }

        MSG_GERAN_MED_0_G("MAC info EGPRS PUA ass");

        /*************  Check if RAC info has been requested ****************/
        if (pua_ptr->access_technologies_req_flag == valid)
        {
           MSG_GERAN_HIGH_0_G("MAC info RAC acquisition not applicable to TRANSFER mode, request ignored");
        }

        /***************************************************************************
        ********** Assigned EGPRS UL, add EGPRS L1 UL PARAMTERS to Ul config *******
        ***************************************************************************/

        ul_tbf_msg_ptr->tbf_mode = TBF_MODE_EGPRS;

        /***************************  BEP PERIOD  **********************************/
        if(pua_ptr->bep_period2_flag == valid)
        {
          ul_tbf_msg_ptr->bep_period2.bep_period2 = pua_ptr->bep_period2;
          ul_tbf_msg_ptr->bep_period2.present = TRUE;
        }
        else
        {
           ul_tbf_msg_ptr->bep_period2.present = FALSE;
           ul_tbf_msg_ptr->bep_period2.bep_period2 = 0;
        }

        /*L1 needs to know RLC mode to determine if IR applicable */
        ul_tbf_msg_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;

      }
      else
      {
        ul_tbf_msg_ptr->tbf_mode = TBF_MODE_GPRS;
        ul_tbf_msg_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;
        ul_tbf_msg_ptr->bep_period2.present = FALSE;
        ul_tbf_msg_ptr->bep_period2.bep_period2 = 0;
      }

      ul_tbf_msg_ptr->test_mode = global_data_ptr->current_test_mode;

      ul_tbf_msg_ptr->ts_offset = global_data_ptr->gmac_test_mode_ts_offset;

      ul_tbf_msg_ptr->srb_mode = global_data_ptr->gmac_test_mode_srb_mode;

      /* R99 extension : Extended TA */
      /* WARNING : Not applicable to this release and is ignored */

      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;

      /* FREQ PARAMS IE */
      if (pua_ptr->freq_param_flag == valid)
      {
        freq_param_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);

        if (freq_param_result == NO_ERRORS)
        {
          rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

          /* save message parameters whilst handling frequency parameters */
          transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc  = mac_l1_sig_uldcf;
          pua_accept_flag = TRUE;
        }
        else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
          return (pua_accept_flag);
        }
        else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
        {
          gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
          return (pua_accept_flag);
        }
        else
        {
          /* unknown error type */
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
            mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
            mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
          return (pua_accept_flag);
        }
      }
      else
      {
        pua_accept_flag = TRUE;

        rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

        /* send MAC_L1_DYNAMIC_TBF_CONFIG with parameters */
        MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DYNAMIC_TBF_CONFIG");
        mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
        mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
        gprs_l1_mac_acc_func(&mac_l1_sig_uldcf,gas_id);
      }

      memset(&(transfer_data_ptr->gmac_transfer_ul_rlc_info_store), 0, sizeof(mr_ul_alloc_cnf_t));


      /* if we get here than this no problem in the egprs prams rcvd in the PUA */

      if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
/*fix for cs change handling*/
        MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, pua_chan_coding_cmd %d",
                          transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                          global_data_ptr->gmac_local_cs_setting,pua_ptr->egprs_chan_coding_cmd);

        if (global_data_ptr->gmac_local_cs_setting != pua_ptr->egprs_chan_coding_cmd)
        {
          transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
          transfer_data_ptr->pua_ptr_cs_change = pua_ptr->egprs_chan_coding_cmd;
        }

        /***************************************************************************
        ******** Assigned EGPRS UL, add EGPRS RLC UL PARAMTERS to Ul config ********
        ****************************************************************************/
        MSG_GERAN_MED_3_G("MAC info MCS =%d, Reseg =%d, ul_ws =%d",
                          pua_ptr->egprs_chan_coding_cmd,pua_ptr->resegment,global_data_ptr->current_egprs_ul_ws);

        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_EGPRS;

        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.mcs = pua_ptr->egprs_chan_coding_cmd;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.resegment = pua_ptr->resegment;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ws = global_data_ptr->current_egprs_ul_ws;

      }
      else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
      {
        MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, pua_chan_coding_cmd %d",
                          transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                          global_data_ptr->gmac_local_cs_setting,pua_ptr->chan_coding_cmd);
        if (global_data_ptr->gmac_local_cs_setting != pua_ptr->chan_coding_cmd)
        {
          transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
          transfer_data_ptr->pua_ptr_cs_change = pua_ptr->chan_coding_cmd;
        }

        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.cs = pua_ptr->chan_coding_cmd;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_GPRS;
      }
      else
      {
        MSG_GERAN_ERROR_1_G("Invalid TBF mode = %d",global_data_ptr->current_tbf_mode);
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
          mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
          mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
        return (pua_accept_flag);
      }

#ifdef FEATURE_GSM_DTM
      if ( pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
      {
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_EXT_DYNAMIC_ALLOC;
      }
      else
      {
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
      }
#else
      gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
#endif /*FEATURE_GSM_DTM*/

      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_phase = GMAC_NO_PHASE_ACCESS;
      /*gmac_transfer_ul_rlc_info_store.cs = pua_ptr->chan_coding_cmd;*/
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tlli_block_cs = pua_ptr->tlli_blk_chan_coding;
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.num_timeslots = 
        gmac_utils_calculate_nts(pua_ptr->dynamic_alloc.usf_bitmap,gas_id);
      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.usf_granularity = pua_ptr->dynamic_alloc.usf_granularity;

      if(pua_ptr->dynamic_alloc.rlc_data_blks_granted_flag == valid)
      {
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_CLOSE_ENDED_TBF;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = pua_ptr->dynamic_alloc.rlc_data_blks_granted + 9;
      }
      else
      {
        /* NOTE : MR_UL_ALLOC_CNF blocks_granted is for closed TBF only */
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_OPEN_ENDED_TBF;
        transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = 0;
      }

      /* NOTE : MR_UL_ALLOC_CNF final_alloc is for fixed only */
      /* NOTE : MR_UL_ALLOC_CNF ti is for access only */
      /* NOTE : MR_UL_ALLOC_CNF tlli is for access only */
      /* NOTE : MR_UL_ALLOC_CNF retry is for access only */

      transfer_data_ptr->gmac_transfer_ul_rlc_info_store.sig = MR_UL_ALLOC_CNF;


      /*Featured ul_tbf_est logging. Consider both feature and both option in EDGE. Step H*/

      /*First decide the BEP_prd2*/
      if(pua_ptr->bep_period2_flag == valid)
      {
        log_BEP_prd2= ul_tbf_msg_ptr->bep_period2.bep_period2;
      }
      else
      {
        log_BEP_prd2=BEP_PERIOD2_NOT_SPECIFIED;
      }

      if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
        mac_log_ul_etbf_establish(
          global_data_ptr->gmac_re_assign_ul_tfi,
          ts_alloc_bitmap,
          (byte) EGPRS_TBF,
          global_data_ptr->current_egprs_ul_ws,
          transfer_data_ptr->gmac_transfer_ul_rlc_info_store.resegment,
          (uint8)log_BEP_prd2,
          gas_id);


      }
      else
      {
        mac_log_ul_tbf_establish(
          global_data_ptr->gmac_re_assign_ul_tfi,
          ts_alloc_bitmap,
          gas_id);
      }
    }
  }
  return (pua_accept_flag);
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_puan
===
===  DESCRIPTION
===  Handles packet uplink ACK/NACK during uplink dynamic TBF.
===  If PUAN contains fixed allocation, abnormal release with access retry
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tdynamic_handle_puan(boolean dl_present, gas_id_t gas_id)
{
  packet_uplink_ack_nack_t  *puan_ptr;
  mac_rlc_msg_t             *mac_rlc_sig_ptr;
  mr_ul_pkt_acknack_ind_t   *ul_pkt_acknack_msg;

  mac_l1_sig_t              mac_l1_sig_ta;
  mac_l1_sig_t              mac_l1_sig_pcp;
  mac_l1_pwr_ctrl_t         *pwr_ctrl_msg = &(mac_l1_sig_pcp.mac_l1_power_control);
  global_tfi_t               puan_tfi = {0};

  /*the fai for EGPRS is in a diffrerent IE to that used for GPRS*/
  boolean fai_set = FALSE;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  puan_ptr = (packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr;  

  if (global_data_ptr->csn_dec_hdr_ptr->error_code == ADDRESS_INFO_ERROR)
  {
    MSG_GERAN_ERROR_1_G("MAC CSN Decode Error ignore PUAN, error: 0x%x",global_data_ptr->csn_dec_hdr_ptr->error_code);
    return;
  }

  puan_tfi.direction = uplink_valid; /* PUAN only contains UL TFI*/
  puan_tfi.uplink_tfi = puan_ptr->uplink_tfi;

  if (gmac_util_global_tfi(puan_tfi,gas_id) == TRUE)
  {
    if((puan_ptr->egprs_puan == not_valid) && (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS))
    {
      MSG_GERAN_MED_0_G("MAC info PUAN rcvd in EGPRS mode containing GPRS IE only. Ignore ");
      return;
    }

    if((puan_ptr->egprs_puan == valid) && (global_data_ptr->current_tbf_mode == TBF_MODE_GPRS))
    {
      MSG_GERAN_MED_0_G("MAC info PUAN rcvd in GPRS mode containing EGPRS IE only. Ignore ");
      return;
    }

    /*determine if the FAI in the EGPRS/GPRS PUAN ack/nack des is set*/
    fai_set =  gmac_egprs_fai_set(puan_ptr,gas_id);



    MSG_GERAN_MED_1_G("MAC Rcvd PACKET_UPLINK_ACK_NACK UL TFI %d",global_data_ptr->gmac_local_tfi_store.ul_tfi);

    if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
    {
      MSG_GERAN_LOW_0_G("MAC info PUAN ME");
      gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_UPLINK_ACK_NACK,gas_id);
      return;
    }

    if (puan_ptr->fixed_alloc_flag == valid)
    {
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

      mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);
      if (dl_present){
        mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
      return;
    }
    else
    {
      /* When FAI is set and S/P bit is not ignore PUAN */
      if(!((fai_set == TRUE) &&
          (!(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08))))
      {
        mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
        if (mac_rlc_sig_ptr == NULL)
        {
          ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
        }
        else
        {
          /***** load up MR_UL_PKT_ACKNACK_IND signal data using EGPRS ack/nackdes ***/
          memset(mac_rlc_sig_ptr, 0, sizeof(mac_rlc_msg_t));
          ul_pkt_acknack_msg = &(mac_rlc_sig_ptr->msg.ul_pkt_acknack_ind);
          ul_pkt_acknack_msg->tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
          ul_pkt_acknack_msg->cs = puan_ptr->chan_coding_cmd;
          ul_pkt_acknack_msg->sp = global_data_ptr->gmac_local_rrbp_valid;

          if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
          {
             ul_pkt_acknack_msg->mcs = puan_ptr->egprs_chan_coding_cmd;
             ul_pkt_acknack_msg->resegment = puan_ptr->resegment ;
             ul_pkt_acknack_msg->pre_emptive_tx = puan_ptr->pre_emptive_tran ;
             ul_pkt_acknack_msg->egprs_acknack_desc = puan_ptr->egprs_ack_nack_des;
             transfer_data_ptr->gmac_transfer_glob_final_ack_ind = puan_ptr->egprs_ack_nack_des.fai;

             /*log puan*/
             mac_log_ul_acknack(global_data_ptr->csn_msg_bit_length, global_data_ptr->dl_msg, gas_id);

             /* TBF_est is not optionl for EGPRS PUAN*/
             global_data_ptr->tbf_est_allowed_flag = TRUE;
          }
          else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
          {
            gmac_read_acknack_desc_ie(puan_ptr->ack_nack_desc,&(ul_pkt_acknack_msg->acknack_desc), gas_id);
            transfer_data_ptr->gmac_transfer_glob_final_ack_ind = puan_ptr->ack_nack_desc.final_ack_ind;

            if (puan_ptr->r99_additions_flag == valid)
            {
              global_data_ptr->tbf_est_allowed_flag = TRUE;
            }
            else
            {
              global_data_ptr->tbf_est_allowed_flag = FALSE;
            }
          }

          /* TA Parameters, send to L1 if there is a change */
          if ((puan_ptr->pkt_timing_adv_flag == valid) &&
              (gmac_util_handle_power_ctrl_TA_IE(puan_ptr->pkt_timing_adv,&(pwr_ctrl_msg->global_packet_ta), gas_id)))
          {
            pwr_ctrl_msg->global_packet_ta_present = TRUE;
          }
          else
          {
            pwr_ctrl_msg->global_packet_ta_present = FALSE;
          }

          /* Power Control Parameters, to L1  */
          if (puan_ptr->pwr_ctrl_flag == valid)
          {
            pwr_ctrl_msg->power_control.present = TRUE;
            gmac_power_control_params_ie(&(puan_ptr->pwr_ctrl_paramtrs), &(pwr_ctrl_msg->power_control), gas_id);
          }
          else
          {
            pwr_ctrl_msg->power_control.present = FALSE;
          }


          /****************/
          /* send signals */
          /****************/
/*fix for cs change handling*/

          /*When CS is changed via PUAN the" local cs setting" is updated immediately */
          /*however if CS is changed via PUA or PTR then the "local cs setting" is    */
          /*updated when ph_connect_ind is rcvd and confirms the assignment           */

          if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
          {
            MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, puan_chan_coding_cmd %d",
                              transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                              global_data_ptr->gmac_local_cs_setting,puan_ptr->egprs_chan_coding_cmd);

            if (global_data_ptr->gmac_local_cs_setting != puan_ptr->egprs_chan_coding_cmd)
            {
              /* send MAC_L1_CS_RECONFIG due to CS change */
              MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
              mac_l1_sig_ta.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
              mac_l1_sig_ta.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
              gprs_l1_mac_acc_func(&mac_l1_sig_ta, gas_id);

              global_data_ptr->gmac_local_cs_setting = puan_ptr->egprs_chan_coding_cmd;
            }
          }
          else
          {
            MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, puan_chan_coding_cmd %d",
                              transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                              global_data_ptr->gmac_local_cs_setting,
                              puan_ptr->chan_coding_cmd);
            if (global_data_ptr->gmac_local_cs_setting != puan_ptr->chan_coding_cmd)
            {
              /* send MAC_L1_CS_RECONFIG due to CS change */
              MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CS_RECONFIG");
              mac_l1_sig_ta.mac_l1_cs_reconfig.message_header.message_id = (uint8)MAC_L1_CS_RECONFIG;
              mac_l1_sig_ta.mac_l1_cs_reconfig.message_header.message_set = MS_MAC_L1;
              gprs_l1_mac_acc_func(&mac_l1_sig_ta, gas_id);

              global_data_ptr->gmac_local_cs_setting = puan_ptr->chan_coding_cmd;
            }
          }

          /* send MAC_L1_STOP_RLC_TX message to L1 to prevent overloading of UL data buffers */
          gmac_util_send_stop_rlc_tx_to_l1(gas_id);

          /* send MR_UL_PKT_ACKNACK_IND message to RLC */
          MSG_GERAN_LOW_0_G("MAC Sent MR_UL_PKT_ACKNACK_IND");
          mac_rlc_sig_ptr->msg.ul_pkt_acknack_ind.sig = MR_UL_PKT_ACKNACK_IND;
          gmac_rlc_mac_insert_gas_id(mac_rlc_sig_ptr,gas_id);
          grlc_put_cmd_mac(gas_id,mac_rlc_sig_ptr);

          /*******************/
          /* Handle FAI last */
          /*******************/

          if(fai_set)
          {
            /* initiate uplink release */
            gmac_stop_t3180(gas_id);
            MSG_GERAN_LOW_0_G( "MAC PUAN FAI SET");

            /*releasing ul clean up*/
            gmac_stop_t3180(gas_id);
            gmac_stop_t3168(gas_id);
            gmac_stop_t3164_transfer(gas_id);

            /* for safety reset global params */
            memset(&global_data_ptr->gmac_local_l1_ta_params, 0xff, sizeof(pkt_timing_adv_t));
            memset(&global_data_ptr->gmac_local_stored_ul_chan_req, 0xff, sizeof(channel_req_desc_t));

            transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
            transfer_data_ptr->gmac_transfer_ul_request_count = 0;
            transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
            transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;

            global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
            transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;

            /* reset on UL TBF rel */
            global_data_ptr->current_egprs_ul_ws = WS_ALLOCATION_INVALID;

            /* When FAI is set and S/P bit is not ignore PUAN */
            if(!((fai_set == TRUE) &&
              (!(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08))))

            {
              if (dl_present)
              {
                (void)gmac_normal_ul_tbf_release_proc(global_data_ptr->tbf_est_allowed_flag, dl_present, FALSE, gas_id);
              }
              else
              {
                (void)gmac_normal_ul_tbf_release_proc(global_data_ptr->tbf_est_allowed_flag, dl_present, global_data_ptr->gmac_transfer_ul_cv_zero_ind, gas_id);
              }
              mac_log_ul_tbf_release(MAC_LOG_UL_FAI_RELEASE, gas_id);
            }
            else
            {
              /* S/P bit not set but PUAN rcvd with FAI ind peform abnormal release of UL*/
              /* note RLC will perfrom a normal relase becaus it see PUAN with FAI set   */

              if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
              {
                MSG_GERAN_MED_2_G("MAC Rcvd PUAN FAI %d, S/p %d abnormal rel",
                                  fai_set,
                                  ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) >> 3));
              }
              else if (global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
              {
                MSG_GERAN_MED_2_G("MAC Rcvd PUAN FAI %d, S/p %d abnormal rel",
                                  puan_ptr->ack_nack_desc.final_ack_ind,
                                  ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) >> 3));
              }

              gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);
              gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
            }
          }
          else
          {
            rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
          }

          /* only send parameters to L1 if there are changes */
          if ((pwr_ctrl_msg->power_control.present) ||
              (pwr_ctrl_msg->global_packet_ta_present))
          {
            /* send MAC_L1_POWER_CONTROL with parameters */
            MSG_GERAN_MED_0_G("MAC Sent MAC_L1_POWER_CONTROL");
            mac_l1_sig_pcp.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
            mac_l1_sig_pcp.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;
            gprs_l1_mac_acc_func(&mac_l1_sig_pcp,gas_id);
          }
        }
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_dynamic_release_clean
===
===  DESCRIPTION
===  When UL dynamic released, clean up any outstanding UL memory.
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
void gmac_transfer_ul_dynamic_release_clean(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_stop_t3180(gas_id);
  gmac_stop_t3168(gas_id);
  gmac_stop_t3164_transfer(gas_id);

  /* UL TBF released so TFI no longer valid */
  global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;

  /* for safety reset global params */
  memset(&global_data_ptr->gmac_local_l1_ta_params, 0xff, sizeof(pkt_timing_adv_t));
  memset(&global_data_ptr->gmac_local_stored_ul_chan_req, 0xff, sizeof(channel_req_desc_t));

  transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
  transfer_data_ptr->t_flags.gmac_transfer_ul_tbf_realloc_pending = FALSE;
  transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = FALSE;

  global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
  transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;

  global_data_ptr->current_egprs_ul_ws = WS_ALLOCATION_INVALID;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_tbf_abort_alloc
===
===  DESCRIPTION
===  Called when an error occurs in the PUA assignment.
===
===  If MAC is establishing an UL from a DL only state then block/unblock
===  RLC and move to the DL state.
===
===  If the UL is being reallocated then perform an abnormal release with retry.
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
void gmac_transfer_ul_tbf_abort_alloc(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  /* 44.060 8.1.2.5.1
     If establishing UL from the DL state then if a PUA is received
     containing an error then abort the UL establishment and maintain
     the DL */
  if ( (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL) ||
       (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_REASSIGN) ||
       (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_WITH_DYNAMIC_ALLOC ) )
  {
    gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);

    gmac_stop_t3168(gas_id);
    transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
    transfer_data_ptr->gmac_transfer_ul_request_count = 0;
    
    transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;

    if ( transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_WITH_DYNAMIC_ALLOC )
    {
      /*Leave RLC blocked and release L1*/
      gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
    }
    else
    {
      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN_UL,__LINE__,gas_id);
    }
  }
  else
  {
    /* 44.060 8.1.1.1.2
     If a PUA for realloc contains an error then do an abnormal rel
     with retry */
    gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_ul_tbf_abnormal_abort
===
===  DESCRIPTION
===  This function is called when a DL TBF only is active in order to
===  abort any pending requests for UL TBF est by RLC.  RLC is always
===  blocked regardless of whether it was requesting access or not.
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
void gmac_transfer_ul_tbf_abnormal_abort(delete_pdu_cause_t delete_pdu_cause,
                                         gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  gmac_stop_t3168(gas_id);
  gmac_util_block_rlc(delete_pdu_cause, GMAC_TRAN_UL, __LINE__,gas_id);
  transfer_data_ptr->t_flags.gmac_transfer_ul_req = FALSE;
  transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
  transfer_data_ptr->gmac_transfer_ul_request_count = 0;
  global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = FALSE;
  transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_handle_ppcta
===
===  DESCRIPTION
===  Handles packet power control/timing advance message during transfer.
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_handle_ppcta(boolean ul_present, gas_id_t gas_id)
{
  packet_pwr_ctrl_tmng_adv_t *ppcta_data;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  ppcta_data = (packet_pwr_ctrl_tmng_adv_t *)global_data_ptr->csn_dec_data_ptr;

  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    /* handle address - global TFI, TQI or PRR IE */
    /* TQI and PRRef not expected during transfer - address error */
    switch (ppcta_data->tlli_gtfi_pkt_ref_valid)
    {
      case global_tfi_valid:

        if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
        {
          MSG_GERAN_MED_0_G("MAC info PPCTA ME");
          gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_PWR_CTRL_TMNG_ADV,gas_id);
          return;
        }

        /* if stored TFI = received TFI then match else ignore message */
        if (gmac_util_global_tfi(ppcta_data->global_tfi,gas_id))
        {
          if (ul_present)
          {
            rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
          }
          else
          {
            rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
          }
          gmac_comproc_handle_ppcta(gas_id);
        }
        break;

      default:
        /* ignore message */
        break;
    }

  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_handle_par
===
===  DESCRIPTION
===  Handles PACKET ACCESS REJECT received during uplink TBF. If none of
===  the given addresses matches the MS then the PAR is ignored. If an
===  address match is made then a reject is sent to RLC indicating whether
===  WAIT_INDICATOR is present or not.
===
===  DEPENDENCIES
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_handle_par(gas_id_t gas_id)
{
  packet_access_reject_t  *par_ptr;
  reject_struct_t         *rej_ptr;
  uint8                   rej_struct_count;
  boolean                 addr_match = FALSE;
  uint32                  local_rrps_tlli;


  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  par_ptr = (packet_access_reject_t *)global_data_ptr->csn_dec_data_ptr;

  rej_struct_count = par_ptr->no_addition_rej_structs + 1;
  rej_ptr = &(par_ptr->reject_struct0);

  mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
  if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
  {
    while ((addr_match == FALSE) && (rej_struct_count > 0))
    {
      if (rej_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
      {
        /* NOTE: It is possible that addressing using TLLI during transfer
        **       is not valid but at present it is treated as valid.
        */
        rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
        if (local_rrps_tlli == rej_ptr->tlli)
        {
          addr_match = TRUE;
        }
      }
      else if (rej_ptr->tlli_gtfi_pkt_ref_valid == req_ref_valid)
      {
        /* Not valid during transfer mode - ignore PAR */
      }
      else if (rej_ptr->tlli_gtfi_pkt_ref_valid == global_tfi_valid)
      {
        /* Received TFI must match MS TFI */
        addr_match = gmac_util_global_tfi(rej_ptr->global_tfi,gas_id);
      }

      if (addr_match == FALSE)
      {
        rej_struct_count--;
        rej_ptr++;
      }
    }

  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
  {
    addr_match = TRUE;
  }
  else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
  {
    addr_match = FALSE;
  }

  if (addr_match)
  {
    MSG_GERAN_MED_0_G("MAC Rcvd PACKET_ACCESS_REJECT");

    if( rex_get_timer(&mac_timers_ptr->timer_t3168) != 0 )
    {
       if ( (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC) ||
        (transfer_data_ptr->mac_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_TRANSFER) ||
            (transfer_data_ptr->mac_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN) )
       {
          /* Note this code was added to fix a problem with TC 42.3.3.3 in
           TRANSFER_UL_DYNAMIC state.
           The solution here is a bit clunky but makes use of the existing
           mechanisms.  This solution relies upon RLC immediately retrying
           after MAC indicates that it is rejected with ACC_REJ_NO_WAIT_INDICATION.
           When RLC retries in TRANSFER_UL_DYNAMIC state flags are set up to
           ensure that RLC is released when the L1_MAC_TBF_REL_CNF is received.

           For the PAR with WAIT case:
           PAR is received in transfer, MAC acc rejs RLC 'with No wait'.
           RLC will then retry access, MAC will acc rej 'with wait' this request.
           After exiting transfer (via L1_MAC_TBF_REL_CNF) MAC will release RLC.
           RLC tries to access again when MAC is in IDLE.  MAC will reject RLC because
           T3172 is running.  When T3172 expires MAC will release RLC (for the final time!).

           For the PAR without WAIT case:
           PAR is received in transfer, MAC acc rejs RLC 'with No wait'.
           RLC will then retry access, MAC will acc rej 'with wait' this request.
           After exiting transfer (via L1_MAC_TBF_REL_CNF) MAC will release RLC.
           RLC tries to access again when MAC is in IDLE, MAC accepts the request
           because T3172 is not running. */

           gmac_stop_t3168(gas_id);
           if (rej_ptr->wait_valid == wait_ind_valid)
           {
                gmac_start_t3172(rej_ptr->wait_indication, rej_ptr->wait_indication_size,gas_id);
           }
          /*reset count as PAR with wait ind terminates the retry process*/
          transfer_data_ptr->gmac_transfer_ul_request_count = 0;
          transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
          transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
          gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);
          global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
          gmac_transfer_ul_dynamic_release_clean(gas_id);
          gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL,gas_id);
        }
        else
        {
          gmac_stop_t3168(gas_id);
          if (rej_ptr->wait_valid == wait_ind_valid)
          {
            /* T3172 running, block RLC until it has expired */
            gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);
            gmac_start_t3172(rej_ptr->wait_indication, rej_ptr->wait_indication_size,gas_id);

            /*reset count as PAR with wait ind terminates the retry process*/
            transfer_data_ptr->gmac_transfer_ul_request_count = 0;
            transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
            transfer_data_ptr->t_flags.failed_to_send_pdan_chan_req = FALSE;
          }
          else
          {
            /* T3172 is not running, allow RLC to access again */
            gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);
            gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_TRAN_UL, __LINE__,gas_id);
          }
        }
    }
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_transfer_qual_meas_req_to_L1
===
===  DESCRIPTION
===  Send MAC_L1_QUALITY_RPT to L1
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
void gmac_transfer_qual_meas_req_to_L1(gas_id_t gas_id)
{
  mac_l1_sig_t          mac_l1_sig;

  mac_l1_sig.mac_l1_quality_rpt.i_level_ind = TRUE;
  mac_l1_sig.mac_l1_quality_rpt.rrbp_value = MAC_L1_RRBP_VALUE_NOT_VALID;

  /* send MAC_L1_QUALITY_RPT with parameters */
  MSG_GERAN_LOW_0_G("MAC Sent MAC_L1_QUALITY_RPT");
  mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_usf_detected
===
===  DESCRIPTION
===  Handles USF detection indication from L1. If final ack indication set
===  then do not start T3180, otherwise re-start timer.
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
void gmac_tdynamic_usf_detected(gas_id_t gas_id)
{
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
   gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  if (transfer_data_ptr->gmac_transfer_glob_final_ack_ind == RM_ACKNACK_FAI_TBF_NOT_COMPLETE)
  {
    gmac_start_t3180(gas_id);
  }
  else
  {
    gmac_stop_t3180(gas_id);
  }

  /*stop timer on first usf detected in transfer*/
  if( rex_get_timer(&mac_timers_ptr->timer_t3164_transfer) != 0 )
  {
    gmac_stop_t3164_transfer(gas_id);
  }

  if(global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_UL)
  {
    global_data_ptr->gmac_local_pccn_params.pccn_pending = NO_PENDING_PCCN;

    MSG_GERAN_MED_0_G(" pccn_pending = NO_PENDING_PCCN");

    MSG_GERAN_MED_0_G("MAC Sent MAC_GRR_NCELL_DATA_CNF ");
    memset(&(global_data_ptr->grr_sig.mac_grr_ncell_data_cnf),0,sizeof (mac_grr_ncell_data_cnf_t));
    global_data_ptr->grr_sig.mac_grr_ncell_data_cnf.container_id = SERVING_CELL_CONTAINER;
    global_data_ptr->grr_sig.mac_grr_ncell_data_cnf.message_header.message_set = MS_MAC_RR;
    global_data_ptr->grr_sig.mac_grr_ncell_data_cnf.message_header.message_id = (uint8)MAC_GRR_NCELL_DATA_CNF;
    gprs_grr_mac_acc_func(&global_data_ptr->grr_sig,gas_id);

    memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));
  }

}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_data_ind
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
void gmac_tdynamic_data_ind(gas_id_t gas_id)
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
      gmac_tdynamic_handle_puan(FALSE,gas_id);
      break;

    case PACKET_UPLINK_ASSIGNMENT:

      /* CR 69716 - no modifications made here because SRB mode should only be
         set in the DL and concurrent states */
      if( rex_get_timer(&mac_timers_ptr->timer_t3180) == 0 )
      {
        /*Ignore PUA during ul rel process ie t3180 will not running*/
        MSG_GERAN_MED_1_G("PUA rcvd during ul rel. t3180 = %d",rex_get_timer(&mac_timers_ptr->timer_t3180));
      }
      else
      {
        if (gmac_tdynamic_handle_pua(gas_id))
        {
          gmac_stop_t3168(gas_id);

          global_data_ptr->gmac_transfer_ul_cv_zero_ind = FALSE;

          MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_REALLOC");
          transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC;

          if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }

          (void)mac_log_state(gas_id);
        }
      }
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:
      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode,gas_id))
      {
        grlc_dl_start_t3190(gas_id);

        MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_DL_ASSIGN");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_DL_ASSIGN;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        gmac_stop_t3168(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_REALLOC_DL_ASSIGN");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;

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
      gmac_transfer_ul_tbf_release(FALSE,gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_ONLY,
                                      gas_id);
      break;

    case PACKET_POLLING_REQUEST:
      /* no action */
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
                                                             UL_ONLY,
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
===  FUNCTION      gmac_tdynamic_realloc_data_ind
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  reallocation
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
void gmac_tdynamic_realloc_data_ind(gas_id_t gas_id)
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
      gmac_tdynamic_handle_puan(FALSE,gas_id);
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      if (global_data_ptr->c_flags.tbf_release_pending_flg)
      {
        MSG_GERAN_HIGH_0_G("GMAC ignoring PUA, UL TBF is releasing");
      }
      else
      {
        if ( gmac_tdynamic_handle_pua(gas_id) == TRUE )
        {
          if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }
        }
      }
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:
      /* ignore until UL reallocation complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        gmac_stop_t3168(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_REALLOC_DL_ASSIGN");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;

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
      gmac_transfer_ul_tbf_release(FALSE,gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_ONLY,gas_id);
      break;

    case PACKET_POLLING_REQUEST:
      /* no action */
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
                                                             UL_ONLY,
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
===  FUNCTION      gmac_tdynamic_dl_assign_data_ind
===
===  DESCRIPTION
===
===  Handles received logical channel messages during uplink dynamic
===  packet transfer with DL assignment
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
void gmac_tdynamic_dl_assign_data_ind(gas_id_t gas_id)
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
      gmac_tdynamic_handle_puan(FALSE,gas_id);
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until DL assignment complete */
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:
      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode,gas_id))
      {
        grlc_dl_start_t3190(gas_id);

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
      }
    break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        gmac_stop_t3168(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = T_UL_DYNAMIC_REALLOC_DL_ASSIGN");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_UL_DYNAMIC_REALLOC_DL_ASSIGN;

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
      gmac_transfer_ul_tbf_release(TRUE,gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_ONLY,gas_id);
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
===  FUNCTION      gmac_tdynamic_realloc_dl_assign_data_ind
===
===  DESCRIPTION
===
===  Handles received logical channel messages during uplink dynamic
===  reallocation with DL assignment in packet transfer
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
void gmac_tdynamic_realloc_dl_assign_data_ind(gas_id_t gas_id)
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
    MSG_GERAN_MED_0_G("GMAC reallocating DTM, PH_DATA_IND inhibited");
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(FALSE,gas_id);
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:
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
      gmac_transfer_handle_ppcta(TRUE,gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;

    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate,gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_transfer_ul_tbf_release(TRUE, gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_ONLY,
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
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change,gas_id);

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
===  FUNCTION      gmac_transfer_ul_tbf_release()
===
===  DESCRIPTION
===  The network may initiate release of an UL TBF by sending a
===  PACKET TBF RELEASE message to the MS.
===
===  If the cause is NORMAL the MS shall continue to the next LLC PDU
===  boundary, starting the count down procedure.
===
===  If the cause is ABNORMAL the MS shall abort the uplink TBF and perform
===  an abnormal release with access retry. If a valid RRBP field is received
===  the MS shall send a PCA in the uplink radio block specified.
===
===  DEPENDENCIES
===  udl_tbf_present - DL TBF resources are to be allocated
===  fixed_alloc    - Indiacting UL TBF is in FIXED UL mode
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_ul_tbf_release(boolean dl_tbf_present, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  packet_tbf_release_t *packet_rel_ptr = (packet_tbf_release_t *)global_data_ptr->csn_dec_data_ptr;

  if (gmac_util_global_tfi(packet_rel_ptr->global_tfi, gas_id))
  {
    MSG_GERAN_MED_1_G("MAC Rcvd PACKET_TBF_RELEASE rel cause %d",packet_rel_ptr->tbf_release_cause);

    rrbp_control_block_handler_2(PCA_UL_MSG, gas_id);

    if (packet_rel_ptr->uplink_release)
    {
      if (packet_rel_ptr->tbf_release_cause == GMAC_PACKET_TBF_REL_NORMAL_CAUSE)
      {
        /* Normal cause value */
        /* RLC to continue to next LLC boundary and use countdown
        ** so do not release UL until RLC indicates the release
        */
        gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL, gas_id);
        mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE, gas_id);

        /* Downlink is being established - always abnormal release */
        if ((dl_tbf_present) && (packet_rel_ptr->dwnlink_release))
        {
          gmac_transfer_dl_release_clean(gas_id);
          gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
          mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);
        }
      }
      else
      {
        global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;

        /* Abnormal cause value */
        gmac_transfer_ul_dynamic_release_clean(gas_id);
        gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__,gas_id);
        /* L1 will pend this release if there is a DL pending connection, L1 will then send an UL Rel
           Cnf just BEFORE the Connect Ind is sent*/
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id);

        mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE, gas_id);
      }
    }
    else if ( packet_rel_ptr->dwnlink_release )
    {
      if ( dl_tbf_present )
      {
        MSG_GERAN_MED_0_G("GMAC info PKT TBF rel DL, perform DL AB procedure");
        gmac_transfer_dl_release_clean(gas_id);
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);
      }
    }
  }
}


/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_handle_egprs_cont_res_pua
===
===  DESCRIPTION
===
===  Handles packet uplink assignment during uplink dynamic TBF
===  This leads to uplink dynamic reassignment
===
===  NOTE this function is only used for EGPRS TBF.
===
===  NOTE PUA is not dependent on :-
===       csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===       csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  TRUE if PUA accepted
===  FALSE if PUA rejected
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_tdynamic_handle_egprs_cont_res_pua(packet_uplink_assignment_t *pua_ptr,
                                                gas_id_t gas_id)
{
  mac_l1_sig_t                  mac_l1_sig_uldcf;
  mac_dynamic_tbf_information_T *ul_tbf_msg_ptr = &(mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.dynamic_tbf);

  boolean                       pua_accept_flag = FALSE;
  pua_data_error_t              freq_param_result;

  uint8                         ts_alloc_bitmap = 0;
  uint8                         usf_mask = 0x01;
  uint8                         ts_mask = 0x80;
  uint8                         i;
  uint16                        assigned_ul_ws =0;
  gmac_global_data_t           *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t         *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  /* NOTE persistence level rcvd in PUA will already of been handled in IDLE mode*/

  /* NOTE any PCA handling when PUA is polled has been handled in IDLE mode */

  /* NOTE PUA address checking has been carried out in IDLE mode */

  MSG_GERAN_MED_1_G("MAC Rcvd PACKET_UPLINK_ASSIGNMENT UL TFI %d",global_data_ptr->gmac_local_tfi_store.ul_tfi);

  /*check for GPRS/EGRPS tbf mode conflicts */
  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS && pua_ptr->egprs_pua == not_valid)
  {
   MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS tbf mode EGPRS ignore PUA");
   return (pua_accept_flag);
  }

  if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS && pua_ptr->egprs_pua == valid)
  {
   MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS tbf mode EGPRS ignore PUA");
   return (pua_accept_flag);
  }

  /***************      DYNAMIC/FIXED/SINGLE BLOCK/EXTENSION STRUCT    **********************/
  if (pua_ptr->allocation_flag == dynamic_alloc_e)
  {
#ifdef FEATURE_GSM_DTM
   /* Do not inhibit EDA when FEATURE_GSM_DTM is featured */
#else
    /* extended dynamic allocation is NOT supported */
    if (pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC)
    {
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
      return (pua_accept_flag);
    }
#endif /*FEATURE_GSM_DTM*/

    /************************   multislot class check    *********************************/

    /* reverse usf_bitmap to coincide with TS bitmap */
    for (i=0;i<8;i++)
    {
      if (pua_ptr->dynamic_alloc.usf_bitmap & usf_mask)
      {
        ts_alloc_bitmap |= ts_mask;
      }
      usf_mask <<= 1;
      ts_mask >>= 1;
    }

#ifdef FEATURE_GSM_DTM
        /* The code below will interfere with the EDA calculation if left in for FEATURE_GSM_DTM. */
        /* There seems to be no need to set gmac_local_dl_ts_alloc here as it appears to have no
           effect when used in function gmac_util_dyn_multislot_class_check */
#else
    /*For dyn alloc ,when no DL currently active, then DL timeslots are the same as the UL */
    if((transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC)|| 
       (transfer_data_ptr->mac_transfer_substate == TRANSFER_UL_DYNAMIC_REALLOC ))
    {
      global_data_ptr->gmac_local_dl_ts_alloc = ts_alloc_bitmap;
    }
#endif /*FEATURE_GSM_DTM*/
#ifdef FEATURE_GPRS_PS_HANDOVER
    if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                         global_data_ptr->gmac_local_dl_ts_alloc,
                                         pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                         global_data_ptr->gmac_dtm_cs_ts_alloc,
                                         transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                         gas_id) == FALSE )
#else
    if ( gmac_util_multislot_class_check(ts_alloc_bitmap,
                                         global_data_ptr->gmac_local_dl_ts_alloc,
                                         pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                         global_data_ptr->gmac_dtm_cs_ts_alloc,
                                         transfer_data_ptr->t_flags.dtm_active,
                                         gas_id) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
    {
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release(MAC_LOG_UL_MULTISLOT_ERROR,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release(MAC_LOG_DL_MULTISLOT_ERROR,gas_id);}
      return (pua_accept_flag);
    }

    /***********************                starting time              **************************/
    if (pua_ptr->dynamic_alloc.tbf_start_time_flag == valid)
    {
      if (gmac_util_tbf_starting_ie(&(pua_ptr->dynamic_alloc.tbf_start_frm_num), &(ul_tbf_msg_ptr->starting_time),TRUE,gas_id) == FALSE)
      {
        /* Error with TBF Starting time - abort UL */
        gmac_transfer_ul_tbf_abort_alloc(gas_id);
        return (pua_accept_flag);
      }
      ul_tbf_msg_ptr->tbf_start_time_valid = TRUE;;
    }
    else
    {
      ul_tbf_msg_ptr->tbf_start_time_valid = FALSE;
    }

    if (pua_ptr->dynamic_alloc.uplk_tfi_assignment_flag == valid)
    {
      global_data_ptr->gmac_re_assign_ul_tfi = pua_ptr->dynamic_alloc.uplk_tfi_assignment;
    }
    else
    {
      global_data_ptr->gmac_re_assign_ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
    }

    gmac_global_dynamic_alloc_ie(ul_tbf_msg_ptr, pua_ptr, gas_id);

    /**************************       PACKET TA IE                     *************************/
    /* If TA params not changed, do not send */
    if (gmac_update_local_ta_params(&(pua_ptr->pkt_timing_adv),gas_id))
    {
      (void)gmac_packet_ta_ie(&(pua_ptr->pkt_timing_adv), &(ul_tbf_msg_ptr->timing_advance_params),gas_id);
    }
    else
    {
      ul_tbf_msg_ptr->timing_advance_params.ta_value_ie.present = FALSE;
      ul_tbf_msg_ptr->timing_advance_params.cont_ta_ie.present = FALSE;
    }
  }
  else
  {
    gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
    if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
      mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
    if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
      mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
    return (pua_accept_flag);
  }

  /****************************************************************************
  *                          check EGPRS params                               *
  ****************************************************************************/
  if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
  {
#ifdef FEATURE_GSM_EDTM
    /*store ws prior to reassignment, used to restore ws when DTM Ass is received with no PS defined*/
    global_data_ptr->gmac_edtm_old_ul_ws = global_data_ptr->current_egprs_ul_ws;
#endif /*FEATURE_GSM_EDTM*/

    /**************   check to see if WS is valid   ********************/
    assigned_ul_ws = gmac_egprs_decode_ul_window_size(pua_ptr->egprs_window_size, pua_ptr->dynamic_alloc.usf_bitmap,
                                                      gas_id);

    if(global_data_ptr->current_egprs_ul_ws == WS_ALLOCATION_INVALID)
    {
      /*When in transfer and there is no valid UL WS then there is no UL TBF*/
      if(assigned_ul_ws == WS_ALLOCATION_INVALID)
      {
        MSG_GERAN_MED_0_G("MAC info ERROR in ass ul ws ");
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
          mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
          mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
        return (pua_accept_flag);
      }
      else
      {
        global_data_ptr->current_egprs_ul_ws = assigned_ul_ws;
      }
    }
    else
    {
      /*check that assigned ul ws is not smaller that the current ul ws*/
      if(assigned_ul_ws < global_data_ptr->current_egprs_ul_ws )
      {
        MSG_GERAN_MED_2_G(" MAC info ERROR in ass ul ws < curr ws . curr ws = %d , ass ws = %d",
                          global_data_ptr->current_egprs_ul_ws,assigned_ul_ws);
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

        if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
          mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
        if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
          mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
        return (pua_accept_flag);
      }
      else
      {
        /*update the ul ws*/
        global_data_ptr->current_egprs_ul_ws = assigned_ul_ws;
      }
    }

    /**** Check EGPRS MCS validity ******/

    if ( gmac_util_egprs_mcs_valid(pua_ptr->egprs_chan_coding_cmd,gas_id) == FALSE )
    {
      MSG_GERAN_ERROR_0_G("MAC info ERROR PUA contains invalid EGPRS MCS, ab rel ");
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
      return (pua_accept_flag);
    }

    MSG_GERAN_MED_0_G("MAC info EGPRS PUA ass");

    /*************  Check if RAC info has been requested ****************/
    if (pua_ptr->access_technologies_req_flag == valid)
    {
       MSG_GERAN_HIGH_0_G("MAC info RAC acquisition not applicable to TRANSFER mode, request ignored");
    }

    /***************************************************************************
    ********** Assigned EGPRS UL, add EGPRS L1 UL PARAMTERS to Ul config *******
    ***************************************************************************/

    ul_tbf_msg_ptr->tbf_mode = TBF_MODE_EGPRS;

    /***************************  BEP PERIOD  **********************************/
    if(pua_ptr->bep_period2_flag == valid)
    {
      ul_tbf_msg_ptr->bep_period2.bep_period2 = pua_ptr->bep_period2;
      ul_tbf_msg_ptr->bep_period2.present = TRUE;
    }
    else
    {
       ul_tbf_msg_ptr->bep_period2.present = FALSE;
    }

    /*L1 needs to know RLC mode to determine if IR applicable */
    ul_tbf_msg_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;

  }

  ul_tbf_msg_ptr->test_mode = global_data_ptr->current_test_mode;

  ul_tbf_msg_ptr->ts_offset = global_data_ptr->gmac_test_mode_ts_offset;

  ul_tbf_msg_ptr->srb_mode = global_data_ptr->gmac_test_mode_srb_mode;

  /* R99 extension : Extended TA */
  /* WARNING : Not applicable to this release and is ignored */

  transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;

  /* FREQ PARAMS IE */
  if (pua_ptr->freq_param_flag == valid)
  {
    freq_param_result = gmac_egprs_cont_res_pua_freq_param_ie_util(FALSE,TRUE,&(pua_ptr->freq_params),gas_id);

    if (freq_param_result == NO_ERRORS)
    {
      /* save message parameters whilst handling frequency parameters */
      transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc = mac_l1_sig_uldcf;

      pua_accept_flag = TRUE;
    }
    else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
    {
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
      return (pua_accept_flag);
    }
    else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
    {
      gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);

      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release((mac_log_ul_tbf_release_t)freq_param_result,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);}
      return (pua_accept_flag);
    }
    else
    {
      /* unknown error type */
      gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

      if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
        mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
        mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
      return (pua_accept_flag);
    }
  }
  else
  {
    pua_accept_flag = TRUE;

    /* send MAC_L1_DYNAMIC_TBF_CONFIG with parameters */
    MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DYNAMIC_TBF_CONFIG");
    mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
    mac_l1_sig_uldcf.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
    gprs_l1_mac_acc_func(&mac_l1_sig_uldcf,gas_id);
  }

  mac_log_ul_tbf_establish(global_data_ptr->gmac_re_assign_ul_tfi,
                           ts_alloc_bitmap,
                           gas_id);

  memset(&(transfer_data_ptr->gmac_transfer_ul_rlc_info_store), 0, sizeof(mr_ul_alloc_cnf_t));

  /* if we get here than this no problem in the egprs prams rcvd in the PUA */

  /******************************************************************************************
  *                                     CS reconfig
  ******************************************************************************************/

  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
  {
    MSG_GERAN_MED_3_G("t_cs_reconfig_flg %d ,l_cs_setting %d, pua_chan_coding_cmd %d",
                      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                      global_data_ptr->gmac_local_cs_setting,pua_ptr->egprs_chan_coding_cmd);

    if (global_data_ptr->gmac_local_cs_setting != pua_ptr->egprs_chan_coding_cmd)
    {
      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
      transfer_data_ptr->pua_ptr_cs_change = pua_ptr->egprs_chan_coding_cmd;
    }

    /***************************************************************************
    ******** Assigned EGPRS UL, add EGPRS RLC UL PARAMTERS to Ul config ********
    ****************************************************************************/
    MSG_GERAN_MED_3_G("MAC info MCS =%d, Reseg =%d, ul_ws =%d",
                      pua_ptr->egprs_chan_coding_cmd,pua_ptr->resegment,global_data_ptr->current_egprs_ul_ws);

    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_EGPRS;

    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.mcs = pua_ptr->egprs_chan_coding_cmd;
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.resegment = pua_ptr->resegment;
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ws = global_data_ptr->current_egprs_ul_ws;
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Invalid TBF mode = %d",global_data_ptr->current_tbf_mode);
    gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

    if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
      mac_log_ul_tbf_release(MAC_LOG_UL_UNKNOWN_ERROR,gas_id);}
    if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
      mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);}
    return (pua_accept_flag);
  }

  /*************************************************************************************
  *                           store RLC ul config data                                 *
  **************************************************************************************/

  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.cs = pua_ptr->chan_coding_cmd;

#ifdef FEATURE_GSM_DTM
  if ( pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
  {
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_EXT_DYNAMIC_ALLOC;
  }
  else
  {
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
  }
#else
  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_mode = RM_DYNAMIC_ALLOC;
#endif /*FEATURE_GSM_DTM*/

  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.access_phase = GMAC_NO_PHASE_ACCESS;

  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tlli_block_cs = pua_ptr->tlli_blk_chan_coding;
  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.ul_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.num_timeslots = 
    gmac_utils_calculate_nts(pua_ptr->dynamic_alloc.usf_bitmap,gas_id);
  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.usf_granularity = pua_ptr->dynamic_alloc.usf_granularity;

  if(pua_ptr->dynamic_alloc.rlc_data_blks_granted_flag == valid)
  {
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_CLOSE_ENDED_TBF;
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = pua_ptr->dynamic_alloc.rlc_data_blks_granted + 9;
  }
  else
  {
    /* NOTE : MR_UL_ALLOC_CNF blocks_granted is for closed TBF only */
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_type = RM_OPEN_ENDED_TBF;
    transfer_data_ptr->gmac_transfer_ul_rlc_info_store.blocks_granted = 0;
  }

  /* NOTE : MR_UL_ALLOC_CNF final_alloc is for fixed only */
  /* NOTE : MR_UL_ALLOC_CNF ti is for access only */
  /* NOTE : MR_UL_ALLOC_CNF tlli is for access only */
  /* NOTE : MR_UL_ALLOC_CNF retry is for access only */

  transfer_data_ptr->gmac_transfer_ul_rlc_info_store.sig = MR_UL_ALLOC_CNF;

  return (pua_accept_flag);
}



/*** EOF: don't remove! ***/
