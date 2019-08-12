/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER DOWNLINK MODULE
***
***
*** DESCRIPTION
***
***  This module contains functionality related to downlink packet
***  transfer
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_dl.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 01/12/05   zyu    changed EDGE logging UL TBF est logging item TFI from previous (current)
***                           to the next. This was stopping the UL LQC actions.
*** 12/16/04   zyu    Added EDGE logging API in function gmac_transfer_build_egprs_pdan(...).
*** 12/16/02   pjr     Changed logical & to logical | in gmac_transfer_pdan_chan_qual_rep_struct
*** 11/09/02   pjr     Change to PDAN_WITH_FAI and PDAN_WITH_CRD
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

#include "grlcx.h"
#include "csn.h"
#include "gmac.h"
#include "gmacdef.h"
#include "gmacrb.h"
#include "gmactransfer.h"
#include "gmactran_dl.h"
#include "gmactran_ul.h"
#include "gmacutil.h"
#include "gmaclog.h"
#include "grlctimer.h"
#include "grlcmac.h"
#include "gmacproc.h"
#include "gmacds.h"
#include "gmactransfer_if.h"
#include "stringl/stringl.h"


#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/
/* Defines GAMMA position in bitmap received from CSN.1 decoder */
#define I_LEVEL_TN0_POS 0x01
#define I_LEVEL_TN1_POS 0x02
#define I_LEVEL_TN2_POS 0x04
#define I_LEVEL_TN3_POS 0x08
#define I_LEVEL_TN4_POS 0x10
#define I_LEVEL_TN5_POS 0x20
#define I_LEVEL_TN6_POS 0x40
#define I_LEVEL_TN7_POS 0x80

/* used for gmac_transfer_dl_ack_nack_count_max_2 */
#define PDAN_COUNT_FOR_PMR_MAX  0x02

/*****************************************************************************
***
***     Private Data
***
*****************************************************************************/
gmac_transfer_dl_data_t gmac_transfer_dl_data[NUM_GERAN_DATA_SPACES];


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

static boolean gmac_transfer_dl_handle_pua(gas_id_t gas_id);
static void gmac_transfer_pdan_chan_qual_rep_struct(packet_downlink_ack_nack_t *out_data, l1_mac_qual_measurements_t *in_data,
                                                    gas_id_t gas_id);
static boolean gmac_transfer_handle_pda(uint8 current_mac_mode, gas_id_t gas_id);
static void gmac_transfer_dl_tbf_release(boolean ul_tbf_ind, gas_id_t gas_id);
static void gmac_transfer_dl_store_pdan_info(gas_id_t gas_id);
static void gmac_transfer_dl_qual_meas(gas_id_t gas_id);
static void gmac_tranfer_dl_egprs_pdan_handler(rm_dl_pkt_acknack_req_t *egprs_acknack_msg_ptr,
                                               gas_id_t gas_id);
static void gmac_transfer_build_egprs_pdan(rm_dl_pkt_acknack_req_t *egprs_acknack_msg_ptr,
                                           gas_id_t gas_id);

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_transfer_dl_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to transfer dl data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_null_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_transfer_dl_data_t* gmac_get_transfer_dl_data_ptr(gas_id_t gas_id)
{
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = &gmac_transfer_dl_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    transfer_dl_data_ptr = &gmac_transfer_dl_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Transfer DL bad gas_id:%d",gas_id);
  }

  return transfer_dl_data_ptr;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_tran_dl_data()
===
===  DESCRIPTION
===  Initialises gmac transfer dl data.  Used for Lower layer resets.
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
void gmac_init_tran_dl_data( gas_id_t gas_id )
{
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  gmac_init_tran_dl_data_non_zero(gas_id);

  /* queue for PDAN information from RLC waiting to be compiled and sent */
  memset( &transfer_dl_data_ptr->gmac_local_pdan_info_store,0, sizeof(q_type) );
  transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr = NULL;
  transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 = 0;
  transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = 0;
  transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;
  transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_tran_dl_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) transfer dl data.
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
void gmac_init_tran_dl_data_non_zero( gas_id_t gas_id )
{
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  transfer_dl_data_ptr->i_level_meas = TRUE;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_tbf_release()
===
===  DESCRIPTION
===  The network may initiate abnormal release of a downlink TBF
===  by sending a PACKET TBF RELEASE message to the MS on
===  the PACCH.
===
===  If a valid RRBP, MS shall transmit a PCA in the uplink radio
===  block specified.
===
===  If there is no UL TBF, the MS shall return to packet idle. Otherwise
===  remain in UL packet transfer.
===
===  DEPENDENCIES
===  ul_tbf_ind - UL TBF resources are to be allocated
===
===  RETURN VALUE
===  release_ind - indicating which TBF has been released abnormally
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_dl_tbf_release(boolean ul_tbf_ind,
                                         gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  packet_tbf_release_t *packet_rel_ptr = (packet_tbf_release_t *)global_data_ptr->csn_dec_data_ptr;

  if (gmac_util_global_tfi(packet_rel_ptr->global_tfi, gas_id))
  {
    MSG_GERAN_MED_3_G("MAC Rcvd PACKET_TBF_RELEASE rel cause: %d, dl rel: %d, ul rel: %d",
                      packet_rel_ptr->tbf_release_cause,packet_rel_ptr->dwnlink_release,packet_rel_ptr->uplink_release);

    rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);

    /* TBF release cause not applicable. Treat as
       abnormal release cause for DL TBF */
    if (packet_rel_ptr->dwnlink_release)
    {
      gmac_util_send_dl_release_to_rlc(gas_id);
      gmac_transfer_dl_release_clean(gas_id);
      gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL, gas_id);
      mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE, gas_id);
      global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
    }

    if (ul_tbf_ind)
    {
      /* Uplink is currently being established */
      if (packet_rel_ptr->uplink_release)
      {
        /* This is an abnormal case because the UL has not connected yet.
           Attempt to recover by releasing L1 and blocking RLC */
        MSG_GERAN_HIGH_0_G("GMAC rx Pkt PDCH Rel for UL in DL state, UL not connected yet");
        global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
        gmac_util_block_rlc(NO_DEL, GMAC_TRAN_DL, __LINE__, gas_id);
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL, gas_id);
        mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE, gas_id);
        gmac_transfer_ul_dynamic_release_clean(gas_id);
      }
      else
      {
        global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
      }
    }
    else
    {
      /* Do not block RLC if the downlink is not being released.  Otherwise
         there is no mechanism to unblock RLC */
      if (packet_rel_ptr->dwnlink_release)
      {
        gmac_transfer_ul_tbf_abnormal_abort(NO_DEL, gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("GMAC No DL release requested in Pkt TBF Rel, No action in DL only state");
      }
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_handle_pua
===
===  DESCRIPTION
===  Handles packet uplink assignment during DL packet transfer
===  If successful, this shall lead to uplink fixed assignment from DL
===
===  - if frequency parameters are not the same as for DL, ignore PUA,
===    retry uplink establishment unless tried 4 times then abnormal
===    release with retry
===  - if violate the multislot class, abnormal release with retry
===  - failure for any other reason, abort UL and continue with DL
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  csn_dec_data_ptr - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===  - TRUE if PUA successful
===  - FALSE if PUA unsuccessful
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean gmac_transfer_dl_handle_pua(gas_id_t gas_id)
{
  packet_uplink_assignment_t  *pua_ptr;

  mac_l1_sig_t                  mac_l1_sig_ul;
  mac_dynamic_tbf_information_T *uld_msg_ptr = &(mac_l1_sig_ul.mac_l1_dynamic_tbf_config.dynamic_tbf);

  uint8                         addr_match = FALSE;
  boolean                       pua_accept_flag = FALSE;
  pua_data_error_t              freq_param_result;


  uint8 usf_mask = 0x01;
  uint8 ts_mask = 0x80;
  uint8 ts_alloc_bitmap = 0;
  uint8 i;

 #define  BEP_PERIOD2_NOT_SPECIFIED 0xFF
  uint16 log_BEP_prd2;
 #define EGPRS_TBF 1
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
          /* no action */
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

      if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
      {
        MSG_GERAN_MED_1_G("MAC Rcvd PACKET_UPLINK_ASSIGNMENT add by DL TFI %d",global_data_ptr->gmac_local_tfi_store.dl_tfi);
      }

      if (pua_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
      {
        MSG_GERAN_MED_1_G("MAC Rcvd PACKET_UPLINK_ASSIGNMENT add by TLLI 0x%x",pua_ptr->tlli);
      }


      if(global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
      {
        MSG_GERAN_LOW_0_G("MAC info PUA ME");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_UPLINK_ASSIGNMENT,gas_id);
        gmac_transfer_ul_tbf_abort_alloc(gas_id);
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
       MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS in tbf mode EGPRS. ignore PUA");
       return (pua_accept_flag);
      }

      if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS && pua_ptr->egprs_pua == valid)
      {
       MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned EGPRS in tbf mode GPRS ignore PUA");
       return (pua_accept_flag);
      }


      if (pua_ptr->allocation_flag == dynamic_alloc_e)
      {

#ifdef FEATURE_GSM_DTM
        /* Do not inhibit EDA when FEATURE_GSM_DTM is featured */
#else
        /* extended dynamic allocation is NOT supported - abort UL */
        if (pua_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC)
        {
          gmac_transfer_ul_tbf_abort_alloc(gas_id);
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

        if (pua_ptr->dynamic_alloc.uplk_tfi_assignment_flag == valid)
        {
          global_data_ptr->gmac_re_assign_ul_tfi = pua_ptr->dynamic_alloc.uplk_tfi_assignment;
        }
        else
        {
          /* no TFI so abort UL and continue DL */
          gmac_transfer_ul_tbf_abort_alloc(gas_id);
          return (pua_accept_flag);
        }

        /*Check for PUA with no timeslots allocated*/
        if ( pua_ptr->dynamic_alloc.usf_bitmap == 0 )
        {
          MSG_GERAN_HIGH_0_G("GMAC, PUA allocated 0 timeslots, ignore msg");
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

        if ( (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL) ||
             (transfer_data_ptr->mac_transfer_substate == TRANSFER_DL_REASSIGN) )
        {
          global_data_ptr->gmac_local_ul_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
        }

        /* multislot class check */
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( gmac_util_multislot_class_check(
                                ts_alloc_bitmap,
                                global_data_ptr->gmac_local_dl_ts_alloc,
                                pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                global_data_ptr->gmac_dtm_cs_ts_alloc,
                                transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                gas_id ) == FALSE )
#else
        if ( gmac_util_multislot_class_check(
                                ts_alloc_bitmap,
                                global_data_ptr->gmac_local_dl_ts_alloc,
                                pua_ptr->dynamic_alloc.ext_dynamic_alloc,
                                global_data_ptr->gmac_dtm_cs_ts_alloc,
                                transfer_data_ptr->t_flags.dtm_active,
                                gas_id ) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
          mac_log_dl_tbf_release(MAC_LOG_DL_MULTISLOT_ERROR,gas_id);
          return (pua_accept_flag);
        }

        /* starting time */
        if (pua_ptr->dynamic_alloc.tbf_start_time_flag == valid)
        {
          if (gmac_util_tbf_starting_ie(&(pua_ptr->dynamic_alloc.tbf_start_frm_num), &(uld_msg_ptr->starting_time),TRUE,
                                        gas_id) == FALSE)
          {
            /* Error with TBF Starting time - abort UL */
            gmac_transfer_ul_tbf_abort_alloc(gas_id);
            return (pua_accept_flag);
          }
          uld_msg_ptr->tbf_start_time_valid = TRUE;;
        }
        else
        {
          uld_msg_ptr->tbf_start_time_valid = FALSE;
        }

        gmac_global_dynamic_alloc_ie(uld_msg_ptr, pua_ptr, gas_id);

        /* PACKET TA IE for dynamic reallocation message */
        /* If TA params not changed, do not send */
        if (gmac_update_local_ta_params(&(pua_ptr->pkt_timing_adv),gas_id))
        {
          (void)gmac_packet_ta_ie(&(pua_ptr->pkt_timing_adv), &(uld_msg_ptr->timing_advance_params),gas_id);
        }
        else
        {
          uld_msg_ptr->timing_advance_params.ta_value_ie.present = FALSE;
          uld_msg_ptr->timing_advance_params.cont_ta_ie.present = FALSE;
        }
      }
      else
      {
        /* single block/extension */
        /* Error - abort UL and continue DL */
        gmac_transfer_ul_tbf_abort_alloc(gas_id);
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
          MSG_GERAN_MED_0_G("MAC info ERROR in PUA UL WS");
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

        if (gmac_util_egprs_mcs_valid(pua_ptr->egprs_chan_coding_cmd,gas_id) == FALSE)
        {
          MSG_GERAN_MED_0_G("MAC info ERROR in PUA UL EGPRS MCS, Abnormal Rel");
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
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

        uld_msg_ptr->tbf_mode = TBF_MODE_EGPRS;

        /***************************  BEP PERIOD  **********************************/
        if(pua_ptr->bep_period2_flag == valid)
        {
          uld_msg_ptr->bep_period2.bep_period2 = pua_ptr->bep_period2;
          uld_msg_ptr->bep_period2.present = TRUE;
        }
        else
        {
           uld_msg_ptr->bep_period2.present = FALSE;
        }

        /*L1 needs to know RLC mode to determine if IR applicable */
        uld_msg_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;

      }

      uld_msg_ptr->test_mode = global_data_ptr->current_test_mode;

      uld_msg_ptr->ts_offset = global_data_ptr->gmac_test_mode_ts_offset;

      uld_msg_ptr->srb_mode = global_data_ptr->gmac_test_mode_srb_mode;

      /* R99 extension : Extended TA */
      /* WARNING : Not applicable to this release and is ignored */

      transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = FALSE;

      /* FREQ PARAMS IE */
      if (pua_ptr->freq_param_flag == valid)
      {
        freq_param_result = gmac_util_frequency_param_ie(TRUE,TRUE,gas_id);
        if (freq_param_result == NO_ERRORS)
        {
          /* save message parameters whilst handling frequency parameters */
          transfer_data_ptr->gmac_transfer_store_l1_dyn_realloc = mac_l1_sig_ul;
          pua_accept_flag = TRUE;
        }
        else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
        {
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
          mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);
        }
        else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
        {
          gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
          mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);
        }
        else if (freq_param_result == FREQ_ERROR_DIFFERENT_PARAMS)
        {
          /* ignore PUA and retry access again retry access up to 4 times then abnormal release with access retry */
          if (transfer_data_ptr->gmac_transfer_ul_request_count < MAX_RETRY_ATTEMPTS)
          {
            transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = TRUE;
            MSG_GERAN_ERROR_0_G("MAC error PUA freq params differ, retry access attempt");
          }
          else
          {
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);
          }
        }
        else
        {
          /* unknown error type */
          gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
          mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);
        }
      }
      else
      {
        /* send MAC_L1_DYNAMIC_TBF_CONFIG with parameters */
        MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DYNAMIC_TBF_CONFIG");
        mac_l1_sig_ul.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
        mac_l1_sig_ul.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
        gprs_l1_mac_acc_func(&mac_l1_sig_ul,gas_id);

        pua_accept_flag = TRUE;
      }

      if (pua_accept_flag == TRUE)
      {
        rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

        memset(&(transfer_data_ptr->gmac_transfer_ul_rlc_info_store), 0, sizeof(mr_ul_alloc_cnf_t));

        /* if we get here than there is no problem in the egprs prams rcvd in the PUA */
        if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
        {
/*fix for cs change handling*/
          MSG_GERAN_MED_3_G("cs_reconfig_flg %d ,l_cs_setting %d, pua_chan_coding_cmd %d",
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
          MSG_GERAN_MED_3_G("cs_reconfig_flg %d ,l_cs_setting %d, pua_chan_coding_cmd %d",
                            transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag,
                            global_data_ptr->gmac_local_cs_setting,pua_ptr->chan_coding_cmd);
          if (global_data_ptr->gmac_local_cs_setting != pua_ptr->chan_coding_cmd)
          {
            transfer_data_ptr->t_flags.gmac_transfer_cs_reconfig_flag = TRUE;
            transfer_data_ptr->pua_ptr_cs_change = pua_ptr->chan_coding_cmd;
          }

          transfer_data_ptr->gmac_transfer_ul_rlc_info_store.tbf_mode = TBF_MODE_GPRS;
          transfer_data_ptr->gmac_transfer_ul_rlc_info_store.cs = pua_ptr->chan_coding_cmd;
        }
        else
        {
            MSG_GERAN_ERROR_1_G("Invalid TBF mode = %d",global_data_ptr->current_tbf_mode);
            gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
            mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);
            pua_accept_flag = FALSE;
        }



        /*Featured ul_tbf_est logging. Consider both feature and both option in EDGE. Step G*/
        /*First decide the BEP_prd2*/
        if(pua_ptr->bep_period2_flag == valid)
        {
          log_BEP_prd2=uld_msg_ptr->bep_period2.bep_period2 ;
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
      }
    }
  }
  return (pua_accept_flag);
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_handle_pda
===
===  DESCRIPTION
===  Handles PACKET DOWNLINK ASSIGNMENT received during packet transfer
===
===  - if invalid frequency parameters perform abnormal release with SI
===  - if does not define PDCH satisfactorily, ignore PDA
===  - failure for any other reason, abnormal release without retry
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
static boolean gmac_transfer_handle_pda(uint8 current_mac_mode,
                                        gas_id_t gas_id)
{
  packet_downlink_assignment_t *pda_ptr;

  uint16 assigned_dl_ws =0;

  boolean                 pda_accept_flag = FALSE;
  boolean                 addr_match = FALSE;
  pua_data_error_t        freq_param_result;
  uint32                  local_rrps_tlli;

  /*TLLI in DL CNF to RLC*/
  uint32 old_local_rrps_tlli;

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

          /*TLLI in DL CNF to RLC*/
          /*Only a DL TBF is active, check TLLI against gmac_store_dl_ass_tlli*/
          MSG_GERAN_HIGH_2_G("PACKET_DOWNLINK_ASS addressed by tlli %x, gmac_store_dl_ass_tlli %x",
                             pda_ptr->tlli,global_data_ptr->gmac_store_dl_ass_tlli);

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
          }
          else
          {
             addr_match = TRUE;
          }

          MSG_GERAN_HIGH_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
                             global_data_ptr->gmac_store_dl_ass_tlli,local_rrps_tlli,old_local_rrps_tlli);
          break;

        case global_tfi_valid:
          /* if stored TFI = received TFI then match else ignore message */
          addr_match = gmac_util_global_tfi(pda_ptr->global_tfi,gas_id);
          /*TLLI in DL CNF to RLC*/
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
      MSG_GERAN_MED_3_G("MAC  gmac_store_dl_ass_tlli %x,L_tlli %x o_tlli %x",
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

    if(global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
    {
      MSG_GERAN_LOW_0_G("MAC info PUA ME");
      gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_MESS_ESCAPE_CAUSE, PACKET_DOWNLINK_ASSIGNMENT,gas_id);
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
      MSG_GERAN_HIGH_0_G("GMAC, PDA timeslot allocation is 0, ignore msg");
      return (pda_accept_flag);
    }

    /*tbf mode*/
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
           assigned_dl_ws = gmac_egprs_decode_dl_window_size(pda_ptr->egprs_win_sz, pda_ptr->timeslot_alloc, gas_id);

           /*If an invalid WS has been allocated abnormal release*/
           if(assigned_dl_ws == WS_ALLOCATION_INVALID)
           {
             MSG_GERAN_MED_0_G("MAC info ERROR in ass dl ws ");
             gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);

             if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE){
               mac_log_ul_tbf_release(MAC_LOG_UL_INVALID_MAC_MODE,gas_id);}
             if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE){
               mac_log_dl_tbf_release(MAC_LOG_DL_INVALID_MAC_MODE,gas_id);}
             return (pda_accept_flag);
           }
           else
           {
             global_data_ptr->current_egprs_dl_ws = assigned_dl_ws;
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
       if( pda_ptr->r99_additions_flag == valid && pda_ptr->egprs_options_flag == valid )
       {
         MSG_GERAN_MED_0_G("MAC info:- in GPRS. Rel99 features contain EGPRS data, igonre PDA");
         return (pda_accept_flag);
       }
    }

#ifdef FEATURE_GSM_DTM
    /* Fixed allocation has already been caught at the task level, so not expecting
       to have to check it here.  Transferring between Dynamic and Extended Dynamic
       is allowed in 44.060 rel 4. So no checking between mac_mode and current_mode is
       required.  There is currently no mechanism in place to maintain a consistent
       single mac_mode (i.e. updated by both PUA, PDA and PTR).  So there seems
       little point in updating current_mac_mode until this is in place.
       This will probably not be required until rel5,6 EDA functionality is implemented*/
    transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.mac_mode = current_mac_mode;
#else
    /* MAC_MODE */
    if (pda_ptr->mac_mode == current_mac_mode)
    {
      gmac_transfer_store_dl_tbf_msg.mac_mode = pda_ptr->mac_mode;
    }
    else
    {
      /* Error in MAC_MODE - does not match UL - ignore PDA */
      return (pda_accept_flag);
    }
#endif /*FEATURE_GSM_DTM*/

    /* RLC_MODE */
    if(global_data_ptr->gmac_store_dl_rlc_mode != pda_ptr->rlc_mode)
    {
      /*IF t3192 is active and crtl_ack BIT in pda is set then a change of RLC mode is allowed*/
      if ( (grlc_dl_t3192_status(gas_id) == TMR_ACTIVE) && (pda_ptr->ctrl_ack == 1) )
      {
        global_data_ptr->gmac_store_dl_rlc_mode = pda_ptr->rlc_mode;
      }
      else
      {
        MSG_GERAN_MED_2_G("MAC info:- Current rlc mode is %d and Recieved rlc mode is %d ",
                          global_data_ptr->gmac_store_dl_rlc_mode,pda_ptr->rlc_mode);
        MSG_GERAN_MED_0_G("MAC info:- GMAC is releasing the downlink tbf abnormally because difference of rlc mode in PDA");
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);
        return (pda_accept_flag);
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
        MSG_GERAN_HIGH_0_G("MAC info PDA Ctrl Ack fail 0");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_NON_DIST_PART_ERR, PACKET_DOWNLINK_ASSIGNMENT,gas_id);
        return (pda_accept_flag);
      }
    }
    else
    {
      if (pda_ptr->ctrl_ack == 1)
      {
        /* Error in CONTROL_ACK - ignore PDA and inform network */
        MSG_GERAN_HIGH_0_G("MAC info PDA Ctrl Ack fail 1");
        gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_NON_DIST_PART_ERR, PACKET_DOWNLINK_ASSIGNMENT,gas_id);
        return (pda_accept_flag);
      }
    }

    /* Only called from DL transfer substates so there is no UL*/
    global_data_ptr->gmac_local_ul_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#ifdef FEATURE_GPRS_PS_HANDOVER
    if ( gmac_util_multislot_class_check(global_data_ptr->gmac_local_ul_ts_alloc,
                                         pda_ptr->timeslot_alloc,
                                         current_mac_mode,
                                         global_data_ptr->gmac_dtm_cs_ts_alloc,
                                         transfer_data_ptr->t_flags.dtm_active, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                         gas_id) == TRUE )
#else
    if ( gmac_util_multislot_class_check(global_data_ptr->gmac_local_ul_ts_alloc,
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

    /* If TA params not changed, do not send */
    if (gmac_update_local_ta_params(&(pda_ptr->pkt_timing_adv),gas_id))
    {
      (void)gmac_packet_ta_ie(&(pda_ptr->pkt_timing_adv), &(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ta_params),gas_id);
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

    /* DOWNLINK_TFI_ASSIGNMENT */
    if (pda_ptr->dwnlk_tfi_flag)
    {
      global_data_ptr->gmac_re_assign_dl_tfi = pda_ptr->downlink_tfi;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi_present = TRUE;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi = pda_ptr->downlink_tfi;
    }
    else
    {
      global_data_ptr->gmac_re_assign_dl_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.dl_tfi_present = FALSE;
    }
/*event logging changes*/
    mac_log_gprs_pda_event( pda_ptr->timeslot_alloc, global_data_ptr->gmac_re_assign_dl_tfi, gas_id );

    /* always  send parameters to L1 */
    if (pda_ptr->pwr_ctrl_flag == valid)
    {
      transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control.present = TRUE;
      gmac_power_control_params_ie(&(pda_ptr->pwr_ctrl_paramtrs), &(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control), gas_id);
    }
    else
    {
     transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.power_control.present = FALSE;
    }

    /* TBF Starting Time */
    if (pda_ptr->tbf_start_flag)
    {
      if (gmac_util_tbf_starting_ie(&(pda_ptr->tbf_start_frm_num),&(transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_start_time),TRUE, gas_id))
      {
       transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.tbf_start_time_valid = TRUE;
      }
      else
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,DEL_ONE,FALSE,gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_STARTING_TIME_ERROR_WITHOUT_RETRY,gas_id);
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

    /*Featured dl_tbf_est logging. Consider both feature and both option in EDGE*/
    if (pda_ptr->egprs_options_flag)
    {
      mac_log_dl_etbf_establish(
        global_data_ptr->gmac_re_assign_dl_tfi,
        transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.ts_allocation,
        global_data_ptr->gmac_store_dl_rlc_mode,
        (byte) pda_ptr->egprs_options_flag,
        global_data_ptr->current_egprs_dl_ws,
        pda_ptr->egprs_link_qual,
        pda_ptr->egprs_bep_prd2,
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

    /* Measurement Mapping */
    /* WARNING : Not applicable to this release and is ignored */

    /* R99 extension : EGPRS, Extended TA, COMPACT */
    /* WARNING : Not applicable to this release and is ignored */

    /* FREQ PARAMS IE */
    if (pda_ptr->freq_param_flag == valid)
    {
      freq_param_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
      if (freq_param_result == NO_ERRORS)
      {
        rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
        pda_accept_flag = TRUE;
      }
      else if (freq_param_result == FREQ_ERROR_ACCESS_RETRY)
      {
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);
        return (pda_accept_flag);
      }
      else if(freq_param_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
      {
        gmac_abnormal_release_procedure(RR_PART_SYS_INFO_REQ,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_dl_tbf_release((mac_log_dl_tbf_release_t)freq_param_result,gas_id);
        return (pda_accept_flag);
      }
      else
      {
        /* unknown error type */
        gmac_abnormal_release_procedure(RR_RETURN_TO_IDLE,REL_RLC_DL,NO_DEL,FALSE,gas_id);
        mac_log_dl_tbf_release(MAC_LOG_DL_UNKNOWN_ERROR,gas_id);
        return (pda_accept_flag);
      }
    }
    else
    {
      pda_accept_flag = TRUE;

      rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
      gmac_transfer_send_stored_dl_config_to_l1(gas_id);

      /*Only inform RLC of change to DL tbf config when PDA is addressed */
      /*via TLLI,the is no current DL active or DL is being released     */
      if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
      {
        gmac_util_send_dl_ass_to_rlc(gas_id);
        transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
      }

    }
  }
  return (pda_accept_flag);
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_pdan_chan_qual_rep_struct
===
===  DESCRIPTION
===  Complete packet downlink ACK/NACK Channel Quality Report Struct
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
static void gmac_transfer_pdan_chan_qual_rep_struct(packet_downlink_ack_nack_t *out_data, l1_mac_qual_measurements_t *in_data,
                                                    gas_id_t gas_id)
{
  out_data->c_value = in_data->c_value;
  out_data->rxqual = in_data->rxqual;
  out_data->sign_var = in_data->sign_var;

  out_data->i_level_bitmap = 0;

  if (in_data->i_lev_meas.i_lev_tn0.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN0_POS;
    out_data->i_level[0] = in_data->i_lev_meas.i_lev_tn0.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn1.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN1_POS;
    out_data->i_level[1] = in_data->i_lev_meas.i_lev_tn1.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn2.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN2_POS;
    out_data->i_level[2] = in_data->i_lev_meas.i_lev_tn2.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn3.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN3_POS;
    out_data->i_level[3] = in_data->i_lev_meas.i_lev_tn3.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn4.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN4_POS;
    out_data->i_level[4] = in_data->i_lev_meas.i_lev_tn4.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn5.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN5_POS;
    out_data->i_level[5] = in_data->i_lev_meas.i_lev_tn5.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn6.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN6_POS;
    out_data->i_level[6] = in_data->i_lev_meas.i_lev_tn6.i_lev_tn;
  }
  if (in_data->i_lev_meas.i_lev_tn7.i_lev_ind)
  {
    out_data->i_level_bitmap |= I_LEVEL_TN7_POS;
    out_data->i_level[7] = in_data->i_lev_meas.i_lev_tn7.i_lev_tn;
  }
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_dl_transfer_init
===
===  DESCRIPTION
===  Initialise information applicable to downlink transfer
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - local queue for PDAN handling
===  gmac_local_ul_tbf_est_pending - if UL request pending
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_dl_transfer_init(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr = q_init(&transfer_dl_data_ptr->gmac_local_pdan_info_store);
  transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = FALSE;
  transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;
  transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 = 0;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_release_clean
===
===  DESCRIPTION
===  When DL released, clean up any outstanding DL memory.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - if not NULL, free pointer
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_dl_release_clean(gas_id_t gas_id)
{
  /* delete PDAN queue if not NULL */
  gmac_dl_pdan_store_t *pdan_info_ptr;
  gmac_global_data_t   *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = FALSE;
  global_data_ptr->current_egprs_dl_ws = WS_ALLOCATION_INVALID;

  if (transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr != NULL)
  {
    while(q_cnt(transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr) != 0)
    {
       pdan_info_ptr = q_get(transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr);
       GPRS_MEM_FREE(pdan_info_ptr);
    }
  }

  /* DL TBF released so TFI no longer valid */
  global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = FALSE;
  transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
  transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = FALSE;
  transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = FALSE;

  transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 = 0;

  transfer_data_ptr->gmac_tran_last_dl_qual_meas_rrbp_value = 0;
  memset(&(transfer_data_ptr->gmac_tran_saved_qual_meas),0,sizeof(l1_mac_qual_measurements_t));
  transfer_data_ptr->t_flags.gmac_tran_saved_qual_meas_valid = FALSE;
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_normal_release_to_idle
===
===  DESCRIPTION
===  Inform L1 and GRR of DL release
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
void gmac_transfer_dl_normal_release_to_idle(gas_id_t gas_id)
{
  mac_l1_sig_t        mac_l1_sig;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

#ifdef FEATURE_GSM_DTM
  gmac_tran_rel_state_handle_rel_req( DL_TBF_NORMAL, gas_id );
#endif /*FEATURE_GSM_DTM*/

  global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
/*event logging changes*/
  mac_log_packet_tbf_rel_event( DL_TBF_NORMAL, gas_id );

  /* Send MAC_L1_RELEASE */
  MSG_GERAN_MED_1_G("MAC Sent MAC_L1_RELEASE %d",DL_TBF_NORMAL);
  mac_l1_sig.mac_l1_release.release_tbf = DL_TBF_NORMAL;
  mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
  mac_log_dl_tbf_release(MAC_LOG_DL_NORMAL_RELEASE, gas_id);

}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_normal_release_with_ul
===
===  DESCRIPTION
===  Inform L1 and GRR of DL release
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
void gmac_transfer_dl_normal_release_with_ul(gas_id_t gas_id)
{
  mac_l1_sig_t   mac_l1_sig;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

#ifdef FEATURE_GSM_DTM
  gmac_tran_rel_state_handle_rel_req( DL_TBF_NORMAL, gas_id );
#endif /*FEATURE_GSM_DTM*/

  global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
/*event logging changes*/
  mac_log_packet_tbf_rel_event( DL_TBF_NORMAL, gas_id );

  /* Send MAC_L1_RELEASE */
  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_RELEASE");
  mac_l1_sig.mac_l1_release.release_tbf = DL_TBF_NORMAL;
  mac_l1_sig.mac_l1_release.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
  mac_log_dl_tbf_release(MAC_LOG_DL_NORMAL_RELEASE, gas_id);

}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_tbf_status
===
===  DESCRIPTION
===  Compile and send PACKET MOBILE TBF STATUS to the network by
===  sending MAC_PH_DATA_REQ to L1 with message content.
===
===  DEPENDENCIES
===  gmac_local_tfi_store - provides TFI
===  csn_encoder_ul_msg - store message data prior to CSN.1 encoder
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_tbf_status(uint8 cause, uint8 mess_type, gas_id_t gas_id)
{
  mac_l1_sig_t                mac_l1_sig;
  mac_ph_data_req_t           *ph_data_msg_ptr = &(mac_l1_sig.mac_ph_data_req);
  gmac_global_data_t          *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid)
  {
    global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.global_tfi.direction = dwnlink_valid;
    global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.global_tfi.dwnlink_tfi = global_data_ptr->gmac_local_tfi_store.dl_tfi;
  }
  else
  {
    global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.global_tfi.direction = uplink_valid;
    global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.global_tfi.uplink_tfi = global_data_ptr->gmac_local_tfi_store.ul_tfi;
  }

  global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.tbf_cause = cause;
  global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.status_mess_type_flag = valid;
  global_data_ptr->csn_encoder_ul_msg.type.pkt_mobile_tbf_status.status_mess_type = mess_type;

  if (gcsn1_encoder( PACKET_MOBILE_TBF_STATUS, &(ph_data_msg_ptr->dl_ctrl_block[1]), &global_data_ptr->csn_encoder_ul_msg) == PASS)
  {
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      ph_data_msg_ptr->dl_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      ph_data_msg_ptr->dl_ctrl_block[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    if(global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
    {
        /* call log-on-demand function */
        (void)mac_log_signalling_message(UL_PACCH_CHANNEL,
                                         PACKET_MOBILE_TBF_STATUS,
                                         SIZE_OF_CONTROL_RADIO_BLOCK,
                                         &(ph_data_msg_ptr->dl_ctrl_block[0]),
                                          gas_id);

        /* send MAC_PH_DATA_REQ message to L1 */
        MSG_GERAN_MED_1_G("MAC Sent PACKET TBF STATUS (%d)",cause);
#ifdef FEATURE_GSM_EDTM
        mac_l1_sig.mac_ph_data_req.message_type = PACKET_MOBILE_TBF_STATUS;
#endif /*FEATURE_GSM_EDTM*/
        mac_l1_sig.mac_ph_data_req.message_header.message_id = (uint8)MAC_PH_DATA_REQ;
        mac_l1_sig.mac_ph_data_req.message_header.message_set = MS_MAC_L1;
        gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);

    }
    else
    {

      memset(&transfer_data_ptr->gmac_local_pts_params,0,sizeof(gmac_pts_params_t));

      /* store CSN.1 encoded PKT TBF SATUTS to be sent via PACCH_RRBP message */
      memscpy(transfer_data_ptr->gmac_local_pts_params.csn_encoded_pts,
              sizeof(transfer_data_ptr->gmac_local_pts_params.csn_encoded_pts),
              ph_data_msg_ptr->dl_ctrl_block,
              SIZE_OF_CONTROL_RADIO_BLOCK);

      transfer_data_ptr->gmac_local_pts_params.pts_pending = TRUE;
    }

  }
  else
  {
    if(geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode TBF STATUS");
      MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
      geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode TBF STATUS");
    }
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_stored_dl_config_to_l1
===
===  DESCRIPTION
===  MAC_L1_DL_TBF_CONFIG data has been stored waiting to send to L1.
===  Copy stored data into message and send.
===
===  DEPENDENCIES
===  gmac_transfer_store_dl_tbf_msg - message data is stored
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_stored_dl_config_to_l1(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);

  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_DL_TBF_CONFIG");

  /* send MAC_L1_DL_TBF_CONFIG with parameters */
  transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.message_header.message_set = MS_MAC_L1;
  transfer_data_ptr->gmac_transfer_store_dl_tbf_msg.message_header.message_id = (uint8)MAC_L1_DL_TBF_CONFIG;
  gprs_l1_mac_acc_func((mac_l1_sig_t *)(&transfer_data_ptr->gmac_transfer_store_dl_tbf_msg), gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_send_stored_ts_reconfig_to_l1
===
===  DESCRIPTION
===  MAC_L1_TS_RECONFIG data has been stored waiting to send to L1.
===  Copy stored data into message and send.
===
===  DEPENDENCIES
===  gmac_transfer_store_ts_reconfig - message data is stored
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_send_stored_ts_reconfig_to_l1(gas_id_t gas_id)
{
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  MSG_GERAN_MED_0_G("MAC Sent MAC_L1_TS_RECONFIG");

  /* send MAC_L1_TS_RECONFIG with parameters */
  transfer_data_ptr->gmac_transfer_store_ts_reconfig.message_header.message_set = MS_MAC_L1;
  transfer_data_ptr->gmac_transfer_store_ts_reconfig.message_header.message_id = (uint8)MAC_L1_TS_RECONFIG;
  gprs_l1_mac_acc_func((mac_l1_sig_t *)&transfer_data_ptr->gmac_transfer_store_ts_reconfig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_store_pdan_info
===
===  DESCRIPTION
===  When DL ACK/NACK request received from RLC, store data received
===  in queue gmac_local_pdan_info_store_ptr.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - store PDAN data from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_transfer_dl_store_pdan_info(gas_id_t gas_id)
{
  gmac_dl_pdan_store_t  *pdan_elem_ptr;
  q_link_type           *link_ptr;
  gmac_global_data_t    *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  pdan_elem_ptr = GPRS_MEM_MALLOC(sizeof(gmac_dl_pdan_store_t));
  if ( pdan_elem_ptr == NULL )
  {
    ERR_GERAN_FATAL_0_G("MAC error MALLOC failed");
  }
  else
  {
    pdan_elem_ptr->pdan_elem = global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req;

    link_ptr = q_link(pdan_elem_ptr, &pdan_elem_ptr->link);
    q_put(transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr, link_ptr);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_qual_meas
===
===  DESCRIPTION
===  Send MAC_L1_QUALITY_RPT to L1 to retreive quality measurements
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
static void gmac_transfer_dl_qual_meas(gas_id_t gas_id)
{
  mac_l1_sig_t          mac_l1_sig;
  mac_l1_quality_rpt_t  *qual_msg_ptr = &(mac_l1_sig.mac_l1_quality_rpt);
  gmac_global_data_t    *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t  *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  qual_msg_ptr->i_level_ind = transfer_dl_data_ptr->i_level_meas;
  qual_msg_ptr->rrbp_value = gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req.frame_number,
                                                   global_data_ptr->msg_ptr->rlc_mac_sig.msg.dl_pkt_acknack_req.rrbp,
                                                   gas_id);

  transfer_data_ptr->gmac_tran_last_dl_qual_meas_rrbp_value = qual_msg_ptr->rrbp_value;

  if (transfer_dl_data_ptr->i_level_meas)
  {
    transfer_dl_data_ptr->i_level_meas = FALSE;
  }
  else
  {
    transfer_dl_data_ptr->i_level_meas = TRUE;
  }

  /* send MAC_L1_QUALITY_RPT with parameters */
  MSG_GERAN_LOW_0_G("MAC Sent MAC_L1_QUALITY_RPT");
  mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
  mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
  gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_pkt_acknack
===
===  DESCRIPTION
===  Complete downlink packet ACK/NACK and send to L1
===  Data for message is stored in local queue for PDAN data which
===  is retrieved and added to message content. CSN.1 encoder is used.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - pointer to local queue for PDAN data
===  gmac_local_ul_tbf_est_pending - indication if UL pending
===  gmac_local_stored_ul_chan_req - stored pending UL channel description
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_dl_pkt_acknack(l1_mac_qual_measurements_t *rx_qual_meas_ptr, gas_id_t gas_id)
{
  mac_l1_sig_t                mac_l1_sig;
  gmac_global_data_t          *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  mac_l1_pacch_rrbp_t         *ml_pacch_rrbp_sig_ptr = &(mac_l1_sig.mac_l1_pacch_rrbp);

  gmac_dl_pdan_store_t        *pdan_info_ptr;
  rm_dl_pkt_acknack_req_t     *pdan_data_ptr;
  uint8                       pdan_ul_req = FALSE;

  boolean                     scan_pdan_q = TRUE;
  gmac_transfer_data_t       *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t    *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);
  gmac_timers_t              *mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  transfer_data_ptr->t_flags.gmac_tran_saved_qual_meas_valid = TRUE;
  transfer_data_ptr->gmac_tran_saved_qual_meas = *rx_qual_meas_ptr;
/* scan the pdan q till a RRBP_FN match is found between stored PDAN RRBP_FN*/
/* and the RRBP_FN rcvd in the quality measurements from L1                 */
while(scan_pdan_q)
{

  pdan_info_ptr = q_get(transfer_dl_data_ptr->gmac_local_pdan_info_store_ptr);

  /* if no ack/nack data waiting to be sent ignore quality measurements from L1 */
  if (pdan_info_ptr != NULL)
  {
    pdan_data_ptr = &(pdan_info_ptr->pdan_elem);
    ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(pdan_data_ptr->frame_number, pdan_data_ptr->rrbp, gas_id);


    if(rx_qual_meas_ptr->rrbp_fn == ml_pacch_rrbp_sig_ptr->absolute_fn)
    {

      global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.downlink_tfi = pdan_info_ptr->pdan_elem.tfi;

      gmac_compile_acknack_desc_ie(&(global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.ack_nack_desc),pdan_info_ptr->pdan_elem.acknack_desc, gas_id);

      gmac_transfer_pdan_chan_qual_rep_struct(&(global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack),rx_qual_meas_ptr, gas_id);

      /* If uplink pending and t3172 is not running (ie a NO previous PDAN w UL request */
      /* has been rejected  with PAR) add channel request                               */
      if ( (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending) && (rex_get_timer(&mac_timers_ptr->timer_t3172) == 0) )
      {
        uint8 t3168_timeout;

        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.chan_req_desc_flag = valid;

        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.chan_req_desc = global_data_ptr->gmac_local_stored_ul_chan_req;

        global_data_ptr->gmac_transfer_ul_cv_zero_ind = FALSE;

        transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
        transfer_data_ptr->gmac_transfer_ul_request_count++;
        pdan_ul_req = TRUE;

        transfer_data_ptr->t_flags.gmac_transfer_ul_req = TRUE;

        /* add the pfi if the Channel Req Desc was included*/
        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.pfi_present = global_data_ptr->pfi_present;
        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.pfi = global_data_ptr->pfi;

        rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&t3168_timeout, gas_id);
        gmac_start_t3168(t3168_timeout, gas_id);

        grlc_ul_restart_await_ass_guard(gas_id, 30000);

        MSG_GERAN_HIGH_1_G("MAC info DL ACKNAK w UL(DL TFI %d)",pdan_info_ptr->pdan_elem.tfi);
      }
      else
      {
        /* Don't add the PFI if Channel Description is not included*/
        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.pfi_present = FALSE;

        global_data_ptr->csn_encoder_ul_msg.type.pkt_dwnlk_ack_nack.chan_req_desc_flag = not_valid;
        MSG_GERAN_HIGH_1_G("MAC info DL ACKNAK(DL TFI %d)",pdan_info_ptr->pdan_elem.tfi);
      }

      if (gcsn1_encoder(PACKET_DOWNLINK_ACK_NACK, &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]), &global_data_ptr->csn_encoder_ul_msg) == PASS)
      {
        ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(pdan_data_ptr->frame_number, pdan_data_ptr->rrbp, gas_id);
        ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = pdan_data_ptr->time_slot;

        /* NOTE: PDAN can only be sent as NB */
        /* ADD MAC CONTROL HEADER */
        if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
        }

        /* call log-on-demand function */
        (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                         PACKET_DOWNLINK_ACK_NACK,
                                         (byte)MAX_SIZE_OF_CONTROL_BLOCK_DATA,
                                         &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                          gas_id);

        ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;

        if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
        {
          ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG_WITH_FAI;
        }
        else if (pdan_ul_req)
        {
          ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG_WITH_CRD;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG;
        }

        if (transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 < PDAN_COUNT_FOR_PMR_MAX)
        {
          transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2++;
        }

#ifdef FEATURE_GSM_EDTM
        ml_pacch_rrbp_sig_ptr->message_type = PACKET_DOWNLINK_ACK_NACK;
#endif /*FEATURE_GSM_EDTM*/

        MSG_GERAN_LOW_0_G("MAC Sent MAC_L1_PACCH_RRBP");
        ml_pacch_rrbp_sig_ptr->message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
        ml_pacch_rrbp_sig_ptr->message_header.message_set = MS_MAC_L1;
        gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);

        GPRS_MEM_FREE(pdan_info_ptr);
        /*stop scanning pdan q*/
        scan_pdan_q = FALSE;
      }
      else
      {
        scan_pdan_q = FALSE;
        GPRS_MEM_FREE(pdan_info_ptr);
        if(geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_0_G("MAC error CSN.1 encode PDAN");
          MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
          geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("MAC error CSN.1 encode PDAN");
        }
      }
    }
    else
    {
      GPRS_MEM_FREE(pdan_info_ptr);
      MSG_GERAN_ERROR_2_G("MAC RRBP_FN mismatch Qual rpt l1 fn %d,mac fn %d ",rx_qual_meas_ptr->rrbp_fn,ml_pacch_rrbp_sig_ptr->absolute_fn);
    }
  }
  else
  {
    /*stop scanning pdan q*/
    scan_pdan_q = FALSE;
  }

}/*end while*/


}


/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_pdan_handler
===
===  DESCRIPTION
===  The mobile station shall transmit a PDAN message in the UL RB specified
===  by the RRBP field, unless another RLC/MAC control message is waiting to
===  be transmitted, in which case the other RLC/MAC control message shall be
===  sent.
===
===  However, the mobile station shall transmit an RLC/MAC control message
===  other than a PDAN at most every second time it is polled.
===
===  NOTE: The MS shall not transmit an RLC/MAC control message other than a
===        PDAN if it contains a FAI or Channel Request Description IE.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - store PDAN data from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_transfer_dl_pdan_handler(rm_dl_pkt_acknack_req_t *acknack_msg_ptr, boolean ul_present,
                                   gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  NOTUSED(ul_present);

  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
  {
      gmac_tranfer_dl_egprs_pdan_handler(acknack_msg_ptr, gas_id);
  }
  else
  {

  /* set up FAL set flag for DL PDAN */
  if (acknack_msg_ptr->acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
  {
    transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
  }
  else
  {
    transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
  }


  if ((transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set) ||
      (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending)  )
  {

    if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
    {
      /* Send PDAN to network */
      gmac_transfer_dl_store_pdan_info(gas_id);
      gmac_transfer_dl_qual_meas(gas_id);

      /*when there is no UL tbf and the DL is being released (ie PDAN with FAI set)*/
      /*only PDAN can be sent on the rrbp so any pending PMR will not be sent. need*/
      /*to inform GRR that sending of PMR failed */
      if((transfer_dl_data_ptr->gprs_can_send_pmr_pccf) && (global_data_ptr->gmac_local_pmr_params.pmr_pending))
      {
        gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE, gas_id);
        /* reset the PMR params*/
        memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
      }

      transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;
    }
#ifdef FEATURE_GSM_EDTM
    else if (global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending == TRUE)
      {
        /* Send EDTM Packet CS Request to network */
        MSG_GERAN_HIGH_0_G("MAC info Send EDTM PKT CS REQ not PDAN");

        gmac_transfer_send_edtm_pcr_on_pacch_rrbp(acknack_msg_ptr->time_slot,
                                                  acknack_msg_ptr->rrbp,
                                                  acknack_msg_ptr->frame_number,
                                                  gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;
      }
#endif /*FEATURE_GSM_EDTM*/
    else if (global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
      {
       /* Send PACKET_CELL_CHANGE_NOTIFICATION to network */
       MSG_GERAN_HIGH_0_G("MAC info Send PCCN not PDAN");

        gmac_transfer_send_pccn_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                              gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        /* reset the PCCN params once PCCN msg has been sent */
        memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));
      }
    else if (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending)
    {
    /* Send PDAN to network */
        gmac_transfer_dl_store_pdan_info(gas_id);
        gmac_transfer_dl_qual_meas(gas_id);
      transfer_dl_data_ptr->gprs_can_send_pmr_pccf = TRUE;
    }
  }
  else
  {
    if(transfer_dl_data_ptr->gprs_can_send_pmr_pccf == TRUE)
    {
      /* The priority of uplink control messages as follows.
      1.PKT CS REQUEST 2 PCCN 3. PCCF 4.PTS 5.PMR*/

#ifdef FEATURE_GSM_EDTM
      if (global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending == TRUE)
      {
        /* Send EDTM Packet CS Request to network */
        MSG_GERAN_HIGH_0_G("MAC info Send EDTM PKT CS REQ not PDAN");

        gmac_transfer_send_edtm_pcr_on_pacch_rrbp(acknack_msg_ptr->time_slot,
                                                  acknack_msg_ptr->rrbp,
                                                  acknack_msg_ptr->frame_number,
                                                  gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      if (global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
      {
        /* Send PACKET_CELL_CHANGE_NOTIFICATION to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PCCN not PDAN");

        gmac_transfer_send_pccn_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                              gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        /* reset the PCCN params once PCCN msg has been sent */
        memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));
      }
      else
      if (global_data_ptr->gmac_local_pccf_params.pccf_pending)
      {
        MSG_GERAN_HIGH_0_G("MAC info Send PCCF not PDAN");
        /* Send PCCF to network */
        gmac_transfer_send_pccf_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                              gas_id);
        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        /* reset the PCCF params once PCCF msg has been sent */
        memset(&global_data_ptr->gmac_local_pccf_params,0,sizeof(gmac_pccf_params_t));
      }
      else if (global_data_ptr->gmac_local_pss_params.pss_pending == TRUE)
      {
        /* Send PACKET_SI_STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PKT SI STATUS not PDAN");

        gmac_transfer_send_pss_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                             gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        /* reset the PSS params once PSS msg has been sent */
        memset(&global_data_ptr->gmac_local_pss_params,0,sizeof(gmac_pss_params_t));
      }
      else if (global_data_ptr->gmac_local_pmr_params.pmr_pending)
      {
        if(transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 == PDAN_COUNT_FOR_PMR_MAX)
        {
          MSG_GERAN_HIGH_0_G("MAC info Send PMR not PDAN");
          /* Send PMR to network */
          gmac_transfer_send_pmr_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                               gas_id);
          gmac_util_send_pmr_cnf_to_grr(MAC_GRR_SUCCESS, gas_id);
          transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

          /* reset the PMR params once PMR msg has been sent */
          memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
        }
        else
        {
          gmac_transfer_dl_store_pdan_info(gas_id);
          gmac_transfer_dl_qual_meas(gas_id);
          transfer_dl_data_ptr->gprs_can_send_pmr_pccf = TRUE;
        }
      }
      else if (transfer_data_ptr->gmac_local_pts_params.pts_pending == TRUE)
      {
        /* Send PKT TBF STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PTS not PDAN");

        gmac_transfer_send_pts_on_pacch_rrbp(acknack_msg_ptr->time_slot,acknack_msg_ptr->rrbp,acknack_msg_ptr->frame_number,
                                             gas_id);

        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = FALSE;

        /* reset the PTS params once PTS msg has been sent */
        memset(&transfer_data_ptr->gmac_local_pts_params,0,sizeof(gmac_pts_params_t));
      }
      else
      {
        /* Send PDAN to network */
        gmac_transfer_dl_store_pdan_info(gas_id);
        gmac_transfer_dl_qual_meas(gas_id);
        transfer_dl_data_ptr->gprs_can_send_pmr_pccf = TRUE;
      }

    }
    else
    {
      /* Send PDAN to network */
      gmac_transfer_dl_store_pdan_info(gas_id);
      gmac_transfer_dl_qual_meas(gas_id);
      transfer_dl_data_ptr->gprs_can_send_pmr_pccf = TRUE;
    }

  }

  }
}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_dl_data_ind
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink transfer
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
void gmac_transfer_dl_data_ind(gas_id_t gas_id)
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
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type, gas_id);
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

    case PACKET_UPLINK_ASSIGNMENT:
      /*if t3168 has exp ignore PUA and perform PRR access retry*/
      if( rex_get_timer(&mac_timers_ptr->timer_t3168) != 0 )
      {

        if (gmac_transfer_dl_handle_pua(gas_id))
        {
          gmac_stop_t3168(gas_id);

          MSG_GERAN_MED_0_G("GMAC State tran = T_DL_WITH_DYN_ALLOC");
          transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_WITH_DYNAMIC_ALLOC;
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = TRUE;

          if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }
          (void)mac_log_state(gas_id);
        }

      }
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* EDA not applicable in DL states */
      if (gmac_transfer_handle_pda(GMAC_MAC_MODE_DYNAMIC_ALLOC,gas_id))
      {
        MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }

        grlc_dl_start_t3190(gas_id);
        transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;
        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      /* PTR cannot establish an UL when T3168 timer is not running.
         T3168 indicates that MAC is waiting for response to a channel request*/
      if( rex_get_timer(&mac_timers_ptr->timer_t3168) != 0 )
      {
        if(gmac_tdynamic_handle_ptr(gas_id))
        {
          gmac_stop_t3168(gas_id);
          MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS_DYN_ALLOC");
          transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;

          if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }

          (void)mac_log_state(gas_id);
        }
      }
      break;

    case PACKET_ACCESS_REJECT:
      gmac_transfer_handle_par(gas_id);
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(FALSE, gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;

    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_transfer_dl_tbf_release(FALSE, gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,DL_ONLY,gas_id);
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
                                                             DL_ONLY,
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
      if(gmac_tran_check_psho_addressing(gas_id) == TRUE)
      {
        MSG_GERAN_MED_0_G("MAC Rcvd PS_HANDOVER_COMMAND");
        gmac_send_data_ind_to_grr(gas_id);
        rrbp_control_block_handler_2(PCA_DL_MSG, gas_id);
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
===  FUNCTION      gmac_transfer_dl_reassign_data_ind
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink
===  reassignment
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
void gmac_transfer_dl_reassign_data_ind(gas_id_t gas_id)
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
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type,gas_id);
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

    /*To handle PUA while DL is being reassigned, this requires L1 and MAC*/
    /*to handle simulations UL and DL configuration while in transfer     */
    case PACKET_UPLINK_ASSIGNMENT:

      /* PUA cannot establish an UL when T3168 timer is not running.
         T3168 indicates that MAC is waiting for response to a channel request*/
      if( rex_get_timer(&mac_timers_ptr->timer_t3168) != 0 )
      {
          if (gmac_transfer_dl_handle_pua(gas_id))
          {
            gmac_stop_t3168(gas_id);

            MSG_GERAN_MED_0_G("GMAC State tran = T_DL_WITH_DYN_ALLOC");
            transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_WITH_DYNAMIC_ALLOC;

            if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
            {
              gmac_transfer_simulate_freq_cnf(gas_id);
            }

            transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = TRUE;
            (void)mac_log_state(gas_id);
          }
      }
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* EDA not applicable in DL states*/
      if (gmac_transfer_handle_pda(GMAC_MAC_MODE_DYNAMIC_ALLOC,gas_id))
      {
       grlc_dl_start_t3190(gas_id);
       transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      /* PTR cannot establish an UL when T3168 timer is not running.
         T3168 indicates that MAC is waiting for response to a channel request*/
      if( rex_get_timer(&mac_timers_ptr->timer_t3168) != 0 )
      {
        if(gmac_tdynamic_handle_ptr(gas_id))
        {
          gmac_stop_t3168(gas_id);
          MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS_DYN_ALLOC");
          transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;

          if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf(gas_id);
          }
          (void)mac_log_state(gas_id);
        }
      }
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(FALSE,gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;

    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_transfer_dl_tbf_release(FALSE, gas_id);
      break;

    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,DL_ONLY,gas_id);
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
                                                             DL_ONLY,
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
===  FUNCTION      gmac_transfer_dl_dynamic_alloc_data_ind
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink transfer
===  with UL dynamic allocation
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
void gmac_transfer_dl_dynamic_alloc_data_ind(gas_id_t gas_id)
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
    case PACKET_UPLINK_ASSIGNMENT:
      if (gmac_tdynamic_handle_pua(gas_id))
      {
        if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
        transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = TRUE;
      }
    break;

    case PACKET_DOWNLINK_ASSIGNMENT:
      /*To handle PDA when UL is being established via DL,this requires L1 and*/
      /*MAC to handle simulations UL and DL configuration while in transfer   */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode, gas_id))
      {
       grlc_dl_start_t3190(gas_id);

       transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;

       if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
       {
         gmac_transfer_simulate_freq_cnf(gas_id);
       }
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if(gmac_tdynamic_handle_ptr(gas_id))
      {
        gmac_stop_t3168(gas_id);
        MSG_GERAN_MED_0_G("GMAC State tran = T_DL_REASS_DYN_ALLOC");
        transfer_data_ptr->mac_transfer_substate = TRANSFER_DL_REASSIGN_DYNAMIC_ALLOC;

        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }

        (void)mac_log_state(gas_id);
      }
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(FALSE, gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;

    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_transfer_dl_tbf_release(TRUE, gas_id);
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
===  FUNCTION      gmac_transfer_dl_reassign_dynamic_alloc_data_ind
===
===  DESCRIPTION
===  Handles received logical channel messages during downlink
===  reassignment with UL dynamic allocation
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
void gmac_transfer_dl_reassign_dynamic_alloc_data_ind(gas_id_t gas_id)
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
    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr(gas_id))
      {
        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(gas_id);
        }
      }
    break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(FALSE,gas_id);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler(gas_id);
      break;

    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate, gas_id);
      break;

    case PACKET_TBF_RELEASE:
      gmac_transfer_dl_tbf_release(TRUE, gas_id);
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
===  FUNCTION      gmac_tranfer_dl_egprs_pdan_handler
===
===  DESCRIPTION
===  The mobile station shall transmit a PDAN message in the UL RB specified
===  by the RRBP field, unless another RLC/MAC control message is waiting to
===  be transmitted, in which case the other RLC/MAC control message shall be
===  sent.
===
===  However, the mobile station shall transmit an RLC/MAC control message
===  other than a PDAN at most every second time it is polled.
===
===  NOTE: The MS shall not transmit an RLC/MAC control message other than a
===        PDAN if it contains a FAI or Channel Request Description IE.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - store PDAN data from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_tranfer_dl_egprs_pdan_handler(rm_dl_pkt_acknack_req_t *egprs_acknack_msg_ptr,
                                               gas_id_t                 gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

  /* set up FAI set flag for DL PDAN */
  if (egprs_acknack_msg_ptr->e_acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
  {
    transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = TRUE;
  }
  else
  {
    transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set = FALSE;
  }

  if ((transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set) ||
      (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending)  )
  {


    if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
    {
    /***************** CSN encode and send PDAN  **********************/
        gmac_transfer_build_egprs_pdan(egprs_acknack_msg_ptr,gas_id);
      /*when there is no UL tbf and the DL is being released (ie PDAN with FAI set)*/
      /*only PDAN can be sent on the rrbp so any pending PMR will not be sent. need*/
      /*to inform GRR that sending of PMR failed */
      if((transfer_dl_data_ptr->egprs_can_send_pmr_pccf) && (global_data_ptr->gmac_local_pmr_params.pmr_pending))
      {
        gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
        /* reset the PMR params*/
        memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
      }

      transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;
    }
#ifdef FEATURE_GSM_EDTM
    else if (global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending == TRUE)
    {
        /* Send PACKET_PSI_STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send EDTM PACKET CS REQUEST not PDAN");

        gmac_transfer_send_edtm_pcr_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,
                                                  egprs_acknack_msg_ptr->rrbp,
                                                  egprs_acknack_msg_ptr->frame_number,
                                                  gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;
    }
#endif /*FEATURE_GSM_EDTM*/
    else if (global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
    {
        /* Send PACKET_CELL_CHANGE_NOTIFICATION to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PCCN not PDAN");

        gmac_transfer_send_pccn_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                              gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        /* reset the PCCN params once PCCN msg has been sent */
        memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));
    }
    else if (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending)
    {
    /***************** CSN encode and send PDAN  **********************/
        gmac_transfer_build_egprs_pdan(egprs_acknack_msg_ptr, gas_id);
      transfer_dl_data_ptr->egprs_can_send_pmr_pccf = TRUE;
    }
  }
  else
  {
    if(transfer_dl_data_ptr->egprs_can_send_pmr_pccf == TRUE)
    {
      /* The priority of uplink control messages as follows.
      1. PKT CS REQUEST 2 PCCN 3. PCCF 4.PTS 5.PMR*/

#ifdef FEATURE_GSM_EDTM
      if (global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending == TRUE)
      {
        /* Send PACKET_PSI_STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send EDTM PACKET CS REQUEST not PDAN");

        gmac_transfer_send_edtm_pcr_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,
                                                  egprs_acknack_msg_ptr->rrbp,
                                                  egprs_acknack_msg_ptr->frame_number,
                                                  gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_pending = FALSE;
      }
      else
#endif /*FEATURE_GSM_EDTM*/
      if (global_data_ptr->gmac_local_pccn_params.pccn_pending == PCCN_PENDING_DL)
      {
        /* Send PACKET_CELL_CHANGE_NOTIFICATION to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PCCN not PDAN");

        gmac_transfer_send_pccn_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                              gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        /* reset the PCCN params once PCCN msg has been sent */
        memset(&global_data_ptr->gmac_local_pccn_params,0,sizeof(gmac_pccn_params_t));
      }
      else

      if (global_data_ptr->gmac_local_pccf_params.pccf_pending)
      {
        MSG_GERAN_HIGH_0_G("MAC info Send PCCF not PDAN");
        /* Send PCCF to network */
        gmac_transfer_send_pccf_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                              gas_id);
        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;
      }
      else if (global_data_ptr->gmac_local_pss_params.pss_pending == TRUE)
      {
        /* Send PACKET_SI_STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PKT SI STATUS not PDAN");

        gmac_transfer_send_pss_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                             gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        /* reset the PSS params once PSS msg has been sent */
        memset(&global_data_ptr->gmac_local_pss_params,0,sizeof(gmac_pss_params_t));
      }
      else if (global_data_ptr->gmac_local_pmr_params.pmr_pending)
      {
        if(transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 == PDAN_COUNT_FOR_PMR_MAX)
        {
          MSG_GERAN_HIGH_0_G("MAC info Send PMR not PDAN");
          /* Send PMR to network */
          gmac_transfer_send_pmr_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                               gas_id);
          gmac_util_send_pmr_cnf_to_grr(MAC_GRR_SUCCESS, gas_id);
          transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;
        }
        else
        {
          /* Send PDAN to network */
          gmac_transfer_build_egprs_pdan(egprs_acknack_msg_ptr, gas_id);
          transfer_dl_data_ptr->egprs_can_send_pmr_pccf = TRUE;
        }
      }
      else if (transfer_data_ptr->gmac_local_pts_params.pts_pending == TRUE)
      {
        /* Send PKT TBF STATUS to network */
        MSG_GERAN_HIGH_0_G("MAC info Send PTS not PDAN");

        gmac_transfer_send_pts_on_pacch_rrbp(egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp,egprs_acknack_msg_ptr->frame_number,
                                             gas_id);

        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = FALSE;

        /* reset the PTS params once PTS msg has been sent */
        memset(&transfer_data_ptr->gmac_local_pts_params,0,sizeof(gmac_pts_params_t));
      }
      else
      {
        /* Send PDAN to network */
        gmac_transfer_build_egprs_pdan(egprs_acknack_msg_ptr, gas_id);
        transfer_dl_data_ptr->egprs_can_send_pmr_pccf = TRUE;
      }

    }
    else
    {
      /* Send PDAN to network */
      gmac_transfer_build_egprs_pdan(egprs_acknack_msg_ptr, gas_id);
      transfer_dl_data_ptr->egprs_can_send_pmr_pccf = TRUE;
    }

  }

}

/*===========================================================================
===
===  FUNCTION      gmac_transfer_build_egprs_pdan
===
===  DESCRIPTION
===  The mobile station shall transmit a PDAN message in the UL RB specified
===  by the RRBP field, unless another RLC/MAC control message is waiting to
===  be transmitted, in which case the other RLC/MAC control message shall be
===  sent.
===
===  However, the mobile station shall transmit an RLC/MAC control message
===  other than a PDAN at most every second time it is polled.
===
===  NOTE: The MS shall not transmit an RLC/MAC control message other than a
===        PDAN if it contains a FAI or Channel Request Description IE.
===
===  DEPENDENCIES
===  gmac_local_pdan_info_store_ptr - store PDAN data from RLC
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/

static void gmac_transfer_build_egprs_pdan(rm_dl_pkt_acknack_req_t *egprs_acknack_msg_ptr,
                                           gas_id_t                 gas_id)
{
  mac_l1_sig_t                mac_l1_sig;
  mac_l1_pacch_rrbp_t        *ml_pacch_rrbp_sig_ptr = &(mac_l1_sig.mac_l1_pacch_rrbp);
  uint8                       pdan_ul_req = FALSE;
  gmac_global_data_t         *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_transfer_data_t       *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_transfer_dl_data_t    *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*****************  build pdan for csn ecncoding **********************/

  /*< DOWNLINK_TFI : bit (5) >*/
  global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.downlink_tfi = egprs_acknack_msg_ptr->tfi;

  global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.ms_out_of_mem = egprs_acknack_msg_ptr->ms_out_of_mem;

  if(egprs_acknack_msg_ptr->e_acknack_desc.len_present == valid)
  {
    MSG_GERAN_MED_2_G("MAC info inc acknack len present =%d, acknack size =%d",
                      egprs_acknack_msg_ptr->e_acknack_desc.len_present,
                      egprs_acknack_msg_ptr->e_acknack_desc.length);
  }

  MSG_GERAN_MED_3_G("MAC info frame number = 0X%x, TS =%d rrbp =%d",
                    egprs_acknack_msg_ptr->frame_number,
                    egprs_acknack_msg_ptr->time_slot,egprs_acknack_msg_ptr->rrbp);

  /*{ 0 | 1 < EGPRS Channel Quality Report : < EGPRS Channel Quality Report IE > >} */
  if(egprs_acknack_msg_ptr->esp == 3)
  {

     if(egprs_acknack_msg_ptr->pl1_qrep_ptr !=0 )
     {
      MSG_GERAN_MED_0_G("MAC info E-PDAN includes qual rpt");

      global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_chn_qual_rpt = *egprs_acknack_msg_ptr->pl1_qrep_ptr;

      global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_chn_qual_rpt_flag = valid;
     }
     else
     {
       MSG_GERAN_ERROR_0_G("MAC info E-PDAN to included qual rpt but no rpt rcvd");
     }
  }
  else
  {
     if(egprs_acknack_msg_ptr->pl1_qrep_ptr !=0)
     {
       /*qual report is optional, but there is space in egprs PDAN so include qual report*/
       global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_chn_qual_rpt_flag = valid;

       global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_chn_qual_rpt = *egprs_acknack_msg_ptr->pl1_qrep_ptr;

     }
     else
     {
       /*qual report is optional but there is NOT enough space to include it into the egprs PDAN*/
       global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_chn_qual_rpt_flag = not_valid;
     }
  }

  MSG_GERAN_MED_2_G("GMAC INFO t_flags.gmac_local_ul_tbf_est_pending: %d, include_chan_req: %d",
                    transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending,egprs_acknack_msg_ptr->include_chan_req);

  /*{ 0 | 1 < Channel Request Description : >Channel Request Description IE > >} */
  if ( (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending == TRUE)
       && (rex_get_timer(&mac_timers_ptr->timer_t3172) == 0)
       && (egprs_acknack_msg_ptr->include_chan_req == TRUE ) )
  {

    uint8 t3168_timeout;

    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.chan_req_desc_flag = valid;

    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.chan_req_desc = global_data_ptr->gmac_local_stored_ul_chan_req;


    transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending = FALSE;
    transfer_data_ptr->gmac_transfer_ul_request_count++;
    pdan_ul_req = TRUE;

    transfer_data_ptr->t_flags.gmac_transfer_ul_req = TRUE;

    rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&t3168_timeout,gas_id);
    gmac_start_t3168(t3168_timeout,gas_id);

    grlc_ul_restart_await_ass_guard(gas_id, 30000);

    MSG_GERAN_HIGH_1_G("MAC info E_DL ACKNAK w UL(DL TFI %d)",egprs_acknack_msg_ptr->tfi);
    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.chan_req_desc_flag = valid;

    /*****  reset flg ****/
    transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = FALSE;

    /* if the chan req is included then add the pfi if available*/
    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.pfi_flag = global_data_ptr->pfi_present;
    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.pfi = global_data_ptr->pfi;

  }
  else
  {
    MSG_GERAN_HIGH_1_G("MAC info E_DL ACKNAK(DL TFI %d)",egprs_acknack_msg_ptr->tfi);
    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.chan_req_desc_flag = not_valid;

    /*pkt chan desc is not included so don't add the pfi*/
    global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.pfi_flag = FALSE;
  }

  /*{ 0 | 1 < Extension Bits : Extension Bits IE > }*/
  global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.extension_bits_flag = not_valid;


  if (egprs_acknack_msg_ptr->e_acknack_desc.len_present == valid)
  {
     MSG_GERAN_MED_0_G("MAC info EGPRS ACK/NACK does NOT fill msg");
  }
  else
  {
     MSG_GERAN_MED_0_G("MAC info EGPRS ACK/NACK does fill msg ");
  }

  global_data_ptr->csn_encoder_ul_msg.type.egprs_pkt_dwnlk_ack_nack.egprs_ack_nack_des = egprs_acknack_msg_ptr->e_acknack_desc;

  /*csn1 encode PDAN and load PDAN into RRBP signal if CSN.1 encoding is succesful*/
  if (gcsn1_encoder(EGPRS_PACKET_DOWNLINK_ACK_NACK, &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]), &global_data_ptr->csn_encoder_ul_msg) == PASS)
  {

    ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(egprs_acknack_msg_ptr->frame_number, egprs_acknack_msg_ptr->rrbp, gas_id);
    ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = egprs_acknack_msg_ptr->time_slot;

    /* NOTE: PDAN can only be sent as NB */
    /* ADD MAC CONTROL HEADER */
    if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
    {
      ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
    }
    else
    {
      ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
    }

    /* call log-on-demand function */
    (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                     EGPRS_PACKET_DOWNLINK_ACK_NACK,
                                     (byte)MAX_SIZE_OF_CONTROL_BLOCK_DATA,
                                     &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                     gas_id);

    ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;

    if (transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set)
    {
      ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG_WITH_FAI;
    }
    else if (pdan_ul_req)
    {
      ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG_WITH_CRD;
    }
    else
    {
      ml_pacch_rrbp_sig_ptr->rrbp_msg = PDAN_MSG;
    }

    if (transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2 < PDAN_COUNT_FOR_PMR_MAX)
    {
      transfer_dl_data_ptr->gmac_transfer_dl_ack_nack_count_max_2++;
    }

/*log PDAN*/  /*use   &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]) . Need to revisit it with length and type not hardcoded*/
    mac_log_dl_acknack(1 /* EGPRS type*/,  22,  &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]), gas_id); /*PDAN always has 22 bytes.*/

#ifdef FEATURE_GSM_EDTM
    ml_pacch_rrbp_sig_ptr->message_type = PACKET_DOWNLINK_ACK_NACK;
#endif /*FEATURE_GSM_EDTM*/

    MSG_GERAN_LOW_0_G("MAC Sent MAC_L1_PACCH_RRBP");
    ml_pacch_rrbp_sig_ptr->message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
    ml_pacch_rrbp_sig_ptr->message_header.message_set = MS_MAC_L1;
    gprs_l1_mac_acc_func(&mac_l1_sig, gas_id);

  }
  else
  {
    MSG_GERAN_ERROR_0_G("MAC info CSN.1 error in encoding E-PDAN");
  }

}

/*===========================================================================
===
===  FUNCTION      new_gmac_trans_calc_e_pdan_size
===
===  DESCRIPTION
===
===   This function is always called by L1 when ever it receives a DL data
===   block that is polled, irrespective of whether a quality report is to
===   included in the E-PDAN
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===   1) Size of free space in E-PDAN for ACK/NACK des. with and without the quality report.
===   2) whether a channel request is to be include in the E-PDAN
===
===  SIDE EFFECTS
===
===========================================================================*/

void new_gmac_trans_calc_e_pdan_size
(
egprs_chn_qual_rpt_t *egprs_qual_rpt,  /* */
uint16 *size_with_qaul,                /* */
uint16 *size_without_qual,             /* */
boolean  *chan_req_present,             /* */
gas_id_t  gas_id
)
{
   #define MAX_SIZE_OF_EPDAN_IN_BITS  176   /*22 octets*/
   #define MESSAGE_TYPE_SIZE_IN_BITS 6

   /*inital size of 6 bits as DL_tfi and ms out of memory are alsways present*/
   /*< DOWNLINK_TFI : bit (5) >  */
   /*< MS OUT OF MEMORY : bit(1)>*/
   uint16 size_without_qual_rpt_in_bits = 6 + MESSAGE_TYPE_SIZE_IN_BITS;

   uint16 size_with_qual_rpt_in_bits = 0;
   uint8 i = 0;
   uint8 mask_value = 1;
   uint8 tn;
   uint8 size_of_chan_req = 0;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
   gmac_transfer_dl_data_t *transfer_dl_data_ptr = gmac_get_transfer_dl_data_ptr(gas_id);

   /******************************************************************************
   *            determine size of CHANNEL REQUEST DESCRIPTION                    *
   ******************************************************************************/

   if( (transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending == TRUE ) && (transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending == FALSE))
   {
       /* { 0 | 1 < Channel Request Description flag 1 bit
       < PEAK_THROUGHPUT_CLASS : bit (4) >
       < RADIO_PRIORITY : bit (2) >
       < RLC_MODE : bit (1) >
       < LLC_ PDU_TYPE : bit (1) >
       < RLC_OCTET_COUNT : bit (16) > }  */
      transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = TRUE;
      *chan_req_present = TRUE;
      size_of_chan_req = 25;
   }
   else
   {
       /* 0 | 1 < Channel Request Description flag 1 bit */
       transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending = FALSE;
       *chan_req_present = FALSE;
       size_of_chan_req = 1;
   }

   /******************************************************************************
   *          determine size of PFC  (currently always set to false)             *
   ******************************************************************************/

   /*this is feature dependent , if the network supports it see PFC_FEATURE_MODE indicated in*/
   /*GPRS Cell Options and if the MS supports it ...got from public store ?*/

   /*{ 0 | 1 < PFI : bit(7) > } */

   /* if the channel req desc is included then add the pfi if it is available*/
   if ( ( global_data_ptr->pfi_present == TRUE ) &&
      ( ( transfer_data_ptr->t_flags.gmac_local_ul_tbf_est_pending == TRUE ) && (transfer_dl_data_ptr->egprs_pdan_with_chan_req_pending == FALSE) ) )
   {
       size_without_qual_rpt_in_bits = size_without_qual_rpt_in_bits + 8;
   }
   else
   {
       /*PFI present flag 1 bit*/
       size_without_qual_rpt_in_bits = size_without_qual_rpt_in_bits + 1;
   }

   /******************************************************************************
   * EXTENSION BIT(note not used so only need to count present/not present flag  *
   ******************************************************************************/

   /*include_extension_bits  present flag 1 bit*/
   size_without_qual_rpt_in_bits = size_without_qual_rpt_in_bits + 1;


   /* EGPRS Channel Quality Report present/not present falg  1 bit*/
   size_without_qual_rpt_in_bits = size_without_qual_rpt_in_bits + 1;


   /*calcualtes the free for the E-PDAN ack/nack des without the qual report*/
   *size_without_qual = (MAX_SIZE_OF_EPDAN_IN_BITS - (size_without_qual_rpt_in_bits + size_of_chan_req));


   /******************************************************************************
   *            determine size of EGPRS BEP Link Quality Measurements IE         *
   ******************************************************************************/

   size_with_qual_rpt_in_bits = size_without_qual_rpt_in_bits;

   /* GMSK_MEAN_BEP present not present flag 1 bit*/
   /* 8PSK_MEAN_BEP present not present flag 1 bit*/
   size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 2;

   /********         EGPRS BEP Link Quality Information elements   **************/

   /* { 0 | 1< GMSK_MEAN_BEP : bit (5) >
             < GMSK_CV_BEP : bit (3) }  >   */
   if(egprs_qual_rpt->gmsk_mean_bep_present == TRUE)
   {
    size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 8;
   }

   /*  { 0 | 1 < 8PSK_MEAN_BEP : bit (5) >
               < 8PSK_CV_BEP : bit (3) > };   */
   if(egprs_qual_rpt->psk_mean_bep_present == TRUE)
   {
    size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 8;
   }

   /*< C_VALUE : bit (6) >*/
   size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 6;

   /***************** EGPRS Timeslot Link Quality Measurements *************************/

   /*BEP_MEASUREMENTS present not present flag*/
   /*INTERFERENCE_MEASUREMENTS present not present flag*/
   size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 2;

   /*{ 0 | 1 { 0 <GMSK_MEAN_BEP _TN0 : bit (4) >
         | 1 < 8PSK_MEAN_BEP _TN0 : bit (4) >}}*/

   if(egprs_qual_rpt->bep_meas_rpt_flg == valid)
   {
   /*tn present not present flag 8 bits*/
     size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 8;

     tn = 0;
     mask_value = 1;

     for ( i=0;  i < 8; i++)
     {
       if (egprs_qual_rpt->tn_bitmap & mask_value)
          tn++;

       mask_value <<=1;
     }

     /*{ 0 | 1 { 0 <GMSK_MEAN_BEP _TN0 : bit (4) >
               | 1 < 8PSK_MEAN_BEP _TN0 : bit (4) >}}*/

     /*GMSK_MEAN_BEP 8PSK_MEAN_BEP present not present (1 bit * number of timeslots) */
     size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + tn;

     /*GMSK_MEAN_BEP_TN 8PSK_MEAN_BEP_TN  (4bits * number of timeslots) */
     size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + (tn *4);

   }

   /*{ 0 | 1 < INTERFERENCE_MEASUREMENTS : Interference Measurement Report Struct >} */
   if(egprs_qual_rpt->int_meas_rpt_flg == valid)
   {
     /*tn present not present flag 8 bits*/
     size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + 8;

     tn =0;
     mask_value = 1;

     for ( i=0;  i < 8; i++)
     {
      if (egprs_qual_rpt->i_level_bitmap & mask_value)
          tn++;

      mask_value <<=1;
     }

     /*{ 0 | 1 { 0 <GMSK_MEAN_BEP _TN0 : bit (4) >
           | 1 < 8PSK_MEAN_BEP _TN0 : bit (4) >}}*/
     size_with_qual_rpt_in_bits = size_with_qual_rpt_in_bits + (tn *4);
   }

   /*calcualtes the free for the E-PDAN ack/nack des with the qual report*/
   *size_with_qaul = MAX_SIZE_OF_EPDAN_IN_BITS -  (size_with_qual_rpt_in_bits + size_of_chan_req);

   MSG_GERAN_MED_3_G("MAC info free sapce for ack/nack des  without qual rpt %d with qual rpt %d, chan_req_present: %d",
                     *size_without_qual,*size_with_qaul,
                     *chan_req_present);

}

/*===========================================================================
===
===  FUNCTION      gmac_tran_check_dl_tbf_status
===
===  DESCRIPTION
===
===  Check to see if MAC is in the process ofsending an GPRS PDAN
===  with the FBI bit set and is waiting on the reception of
===  L1_MAC_QUAL_MEASURMENTS when NO PS ACCESS is rcvd.
===  Ie MAC has not yet sent L1 the PDAN for tx-ing.
===
===  Under these conditions use the previously rcvd L1_MAC_QUAL_MEASURMENTS
===  measurements
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

void gmac_tran_check_dl_tbf_status(gas_id_t gas_id)
{

  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr(gas_id);
  gmac_global_data_t   *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if(transfer_data_ptr->t_flags.gmac_transfer_dl_fai_set == TRUE
     && global_data_ptr->current_tbf_mode == TBF_MODE_GPRS )
  {
    /*act as if MAC has rcvd L1_MAC_QUAL_MEASURMENTS*/
    /*note gmac_transfer_dl_release_clean() is called in gmac_abnormal_release_procedure() */
    if(transfer_data_ptr->t_flags.gmac_tran_saved_qual_meas_valid == TRUE)
    {
      /*use last saved qual meas*/
      transfer_data_ptr->gmac_tran_saved_qual_meas.rrbp_fn = transfer_data_ptr->gmac_tran_last_dl_qual_meas_rrbp_value;

    }
    else
    {
      /*No valid qual meas avalible use default*/
      transfer_data_ptr->gmac_tran_saved_qual_meas.rrbp_fn = transfer_data_ptr->gmac_tran_last_dl_qual_meas_rrbp_value;

      transfer_data_ptr->gmac_tran_saved_qual_meas.c_value = 63;
      transfer_data_ptr->gmac_tran_saved_qual_meas.rxqual = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.sign_var = 0;

      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn0.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn1.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn2.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn3.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn4.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn5.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn6.i_lev_ind = 0;
      transfer_data_ptr->gmac_tran_saved_qual_meas.i_lev_meas.i_lev_tn7.i_lev_ind = 0;

    }

    gmac_transfer_dl_pkt_acknack(&(transfer_data_ptr->gmac_tran_saved_qual_meas), gas_id);

  }

}



/*** EOF: don't remove! ***/
