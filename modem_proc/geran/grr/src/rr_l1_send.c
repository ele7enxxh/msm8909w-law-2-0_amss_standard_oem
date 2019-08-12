/*============================================================================
  rr_l1_send.c

  This file contains functions to send messages to the GL1 task

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l1_send.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_seg_load.h"
#include "rr_l1_send.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_log.h"
#include "rr_sys_info_3g.h"
#include "rr_cell_selection.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/
#include "rr_multi_sim.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w_handover.h"
#include "rr_inter_ho.h"
#endif /* FEATURE_WCDMA */
#include "rr_sim.h"
#include "rr_callback.h"
#include "rr_sys_info_ncell.h"
#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */
#include "geran_tasks.h"
#include "rr_sys_info_cache.h"
#include "rr_pscan.h"
#include "rr_ded_meas_reporting.h"
#include "rr_defs.h"
#include "stringl/stringl.h"
#include "rr_process_pmo_pcco.h"
#include "rr_ded_meas_reporting.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#ifdef FEATURE_SGLTE
#include "rr_sglte.h"
#endif /*FEATURE_SGLTE*/
#include "rr_nv.h"
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
#include "rr_control.h"
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */
#include "grr_mutex.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
#include "rr_irat_overlap_detect.h"
#endif
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define RR_SET_MESSAGE_LENGTH(msg)                                  \
  /*lint -e(778) */                                                 \
  PUT_IMH_LEN(sizeof(msg) - sizeof(IMH_T),&((msg).message_header))

#define RR_PREPARE_L1_MESSAGE(msg, msg_id, gas_id)                   \
  RR_ZERO_STRUCTURE(msg);                                            \
  msg.message_header.message_set = MS_RR_L1;                         \
  msg.message_header.message_id  = (byte) msg_id;                    \
  msg.gas_id = gas_id;                                               \
  RR_SET_MESSAGE_LENGTH(msg);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#define RR_GET_WORD_IMSI_MOD_1000(imsi_mod1000) ((word) (imsi_mod1000.msb) * 256 + (imsi_mod1000.lsb))
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  rr_l1_idle_status_e       l1_idle_status;
  boolean                   current_timing_advance_valid;
  byte                      current_timing_advance;
} rr_l1_send_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_l1_send_data_t rr_l1_send_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_l1_send_data_t*
 */
static rr_l1_send_data_t *rr_l1_send_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_l1_send_data[as_index];
}

/*!
 * \brief Sets the L1 idle status tracking.
 *
 * \param l1_idle_status (in)
 * \param gas_id (in)
 */
static void rr_l1_send_set_l1_idle_status(
  rr_l1_idle_status_e l1_idle_status,
  const gas_id_t gas_id
)
{
  rr_l1_send_data_t *rr_l1_send_data_ptr;

  // Obtain a pointer to the module data
  rr_l1_send_data_ptr = rr_l1_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_send_data_ptr);

  rr_l1_send_data_ptr->l1_idle_status = l1_idle_status;
}

/**
 * Send an inter-task message to Layer 1
 *
 * @param message_ptr pointer to the message to send
 */
static void rr_l1_send_message_to_l1(IMH_T *message_ptr, const gas_id_t gas_id)
{
  gs_status_T gs_status;
  gs_queue_id_T gs_queue;

  RR_NULL_CHECK_FATAL(message_ptr);

  /* log the RR message sent to L1 for QXDM */
  rr_show_output_message(
    "L1",
    message_ptr,
    message_ptr->message_set,
    message_ptr->message_id,
    gas_id
  );

  /* log the message sent out of RR to L1 in trace buffer */
  rr_debug_log_omsg(message_ptr, gas_id);

  gs_queue = rr_ms_get_gs_queue_id(GS_QUEUE_LAYER_1, gas_id);

  gs_status = gs_send_message(gs_queue, (void *)message_ptr, TRUE);

  if (gs_status != GS_SUCCESS)
  {
    if (geran_get_nv_recovery_restart_enabled(gas_id))
    {
      MSG_GERAN_ERROR_0_G("GS unable to send message to L1");
      geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("GS unable to send message to L1");
    }
  }
}

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC

/**
  @brief Populate XMSI parameters structure
*/
static void rr_l1_send_add_xmsi_data(xmsi_params_T *xmsi, const gas_id_t gas_id)
{
  if (xmsi != NULL)
  {
    xmsi->imsi_valid  = rr_siminfo_get_imsi(xmsi->imsi,sizeof(xmsi->imsi), gas_id);
    xmsi->tmsi_valid  = rr_siminfo_get_tmsi(xmsi->tmsi,sizeof(xmsi->tmsi), gas_id);
    xmsi->ptmsi_valid = rr_siminfo_get_ptmsi(xmsi->ptmsi,sizeof(xmsi->ptmsi),gas_id);   

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)   
    if (rr_ms_other_stack_has_same_lai(gas_id))
    {
       MSG_GERAN_HIGH_0_G("Active stack on same LAI");    
       xmsi->epd_pref = RR_L1_EPD_OFF_PREFERRED;      
    }
    else
#endif /*FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/
    {
       xmsi->epd_pref  = RR_L1_EPD_NO_CHANGE;   
    }

    MSG_GERAN_MED_4_G("XMSI: imsi %d tmsi %d ptmsi %d EPD_pref %d",
                        xmsi->imsi_valid,xmsi->tmsi_valid,xmsi->ptmsi_valid,xmsi->epd_pref);
  }
}

#endif /* FEATURE_GSM_PCH_SINGLE_BURST_DEC */

/*!
 * \brief Sets the latest timing advance parameters.
 *
 *  Note: These parameters can be accessed via public API, therefore the updating is protected by mutex.
 * 
 * \param timing_advance_valid (in)
 * \param timing_advance (in)
 * \param gas_id (in)
 */
static void rr_l1_send_update_current_timing_advance_info(
  boolean timing_advance_valid,
  byte timing_advance,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_l1_send_data_t *module_data_ptr = rr_l1_send_get_data_ptr(gas_id);

  if (module_data_ptr != NULL)
  {
    GRR_GENERAL_LOCK();

    /**
     * Note: The timing advance is used differently by USAT and CGPS. For USAT, the timing advance should be the 
     * last timing advance value, even if no longer valid (e.g. don't update the value if the validity flag 
     * indicates the value is not valid). 
     */
    if (timing_advance_valid)
    {
      module_data_ptr->current_timing_advance = timing_advance;
      module_data_ptr->current_timing_advance_valid = TRUE;
    }
    else
    {
      module_data_ptr->current_timing_advance_valid = FALSE;
    }

    GRR_GENERAL_UNLOCK();
  }

  return;
}

/*!
 * \brief Invalidates the current timing advance info.
 *
 * Note: Because this information can be accessed via public API at any time it is guarded by mutex.
 * 
 * \param gas_id (in)
 */
static void rr_l1_send_invalidate_current_timing_advance_info(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_l1_send_data_t *module_data_ptr = rr_l1_send_get_data_ptr(gas_id);

  if (module_data_ptr != NULL)
  {
    GRR_GENERAL_LOCK();

    module_data_ptr->current_timing_advance_valid = FALSE;

    GRR_GENERAL_UNLOCK();
  }

  return;
}

/*!
 * \brief Prints the SI bitmask to diag.
 *
 * \param si_bitmask (in)
 */
static void print_si_bitmask(rr_l1_si_type_T si_bitmask)
{
//#ifdef PERLUTF
#if 1
  if (si_bitmask == RR_L1_SI_TYPE_NONE)
  {
    MSG_GERAN_HIGH_0("  (none)");
  }
  else
  {
    if (si_bitmask & RR_L1_SI_TYPE_1)
    {
      MSG_GERAN_HIGH_0("  SI1");
    }
    if (si_bitmask & RR_L1_SI_TYPE_2)
    {
      MSG_GERAN_HIGH_0("  SI2");
    }
    if (si_bitmask & RR_L1_SI_TYPE_2_bis)
    {
      MSG_GERAN_HIGH_0("  SI2bis");
    }
    if (si_bitmask & RR_L1_SI_TYPE_2_ter)
    {
      MSG_GERAN_HIGH_0("  SI2ter");
    }
    if (si_bitmask & RR_L1_SI_TYPE_2_quater_NORM)
    {
      MSG_GERAN_HIGH_0("  SI2quater (BCCH-Norm)");
    }
    if (si_bitmask & RR_L1_SI_TYPE_2_quater_EXT)
    {
      MSG_GERAN_HIGH_0("  SI2quater (BCCH-Ext)");
    }
    if (si_bitmask & RR_L1_SI_TYPE_3)
    {
      MSG_GERAN_HIGH_0("  SI3");
    }
    if (si_bitmask & RR_L1_SI_TYPE_4)
    {
      MSG_GERAN_HIGH_0("  SI4");
    }
    if (si_bitmask & RR_L1_SI_TYPE_13_NORM)
    {
      MSG_GERAN_HIGH_0("  SI13 (BCCH-Norm)");
    }
    if (si_bitmask & RR_L1_SI_TYPE_13_EXT)
    {
      MSG_GERAN_HIGH_0("  SI13 (BCCH-Ext)");
    }
  }

  return;
#else
  NOTUSED(si_bitmask);
#endif // PERLUTF
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

/**
  @brief Sends MPH_CHANNEL_ASSIGN_REQ to GL1.
*/
void rr_send_mph_channel_assign_req(
  assignment_details_T *assignment_details_ptr,
  multirate_config_T *multirate_config_ptr,
  const gas_id_t gas_id
)
{
  mph_channel_assign_req_T mph_channel_assign_req;

  RR_NULL_CHECK_FATAL(assignment_details_ptr);
  RR_NULL_CHECK_FATAL(multirate_config_ptr);

  RR_PREPARE_L1_MESSAGE(mph_channel_assign_req, MPH_CHANNEL_ASSIGN_REQ, gas_id);

  mph_channel_assign_req.channel_spec = *assignment_details_ptr;
  mph_channel_assign_req.multirate_config = *multirate_config_ptr;

  rr_eng_mode_update_dtx_ul_status(
    mph_channel_assign_req.channel_spec.DTX_indicator,
    gas_id
  );

  rr_l1_send_message_to_l1((IMH_T *)&mph_channel_assign_req, gas_id);
}


/**
  @brief Sends MPH_CHANNEL_MODE_MODIFY_REQ to GL1.
*/
void rr_send_mph_channel_mode_modify_req(
  channel_mode_T channel_mode,
  channel_type_T channel_type,
  byte subchannel,
  multirate_config_T *multirate_config_ptr,
  const gas_id_t gas_id
)
{
  mph_channel_mode_modify_req_T mph_channel_mode_modify_req;

  RR_NULL_CHECK_FATAL(multirate_config_ptr);

  RR_PREPARE_L1_MESSAGE(mph_channel_mode_modify_req, MPH_CHANNEL_MODE_MODIFY_REQ, gas_id);

  mph_channel_mode_modify_req.dedicated_channel_type = channel_type;
  mph_channel_mode_modify_req.subchannel = subchannel;
  mph_channel_mode_modify_req.channel_mode = channel_mode;
  mph_channel_mode_modify_req.multirate_config = *multirate_config_ptr;

  rr_l1_send_message_to_l1((IMH_T *)&mph_channel_mode_modify_req, gas_id);
}


/**
  @brief Sends MPH_HANDOVER_REQ to GL1.
*/
void rr_send_mph_handover_req(
  handover_details_T *handover_details_ptr,
  multirate_config_T *multirate_config_ptr,
  const gas_id_t gas_id
)
{
  mph_handover_req_T mph_handover_req;

  RR_NULL_CHECK_FATAL(handover_details_ptr);
  RR_NULL_CHECK_FATAL(multirate_config_ptr);

  RR_PREPARE_L1_MESSAGE(mph_handover_req, MPH_HANDOVER_REQ, gas_id);

  mph_handover_req.channel_spec = handover_details_ptr->channel_spec;

#ifdef FEATURE_SGLTE
  if(rr_is_sglte_mode_active(gas_id)&& rr_sglte_saved_l2g_ho_being_processed(gas_id))
  {
    ARFCN_T servarfcn = rr_sglte_get_stored_serv_ARFCN(gas_id);

    if(ARFCNS_EQUAL(handover_details_ptr->BCCH_ARFCN,rr_sglte_get_stored_serv_ARFCN(gas_id)))
    {
      BSIC_T serv_bsic = rr_sglte_get_stored_serv_BSIC(gas_id);

      MSG_GERAN_HIGH_0_G("SRVCC Ho on to the same which on which prev camped");
      handover_details_ptr->BCCH_ARFCN = rr_sglte_get_stored_serv_ARFCN(gas_id);
      handover_details_ptr->interrat_measurement_data.number_of_cells = 1;
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].bcch_arfcn = rr_sglte_get_stored_serv_ARFCN(gas_id);
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].fn_offset = 0;
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].qbit_offset= 625;
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].bsic_ncc= serv_bsic.PLMN_colour_code;
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].bsic_bcc= serv_bsic.BS_colour_code;
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].rssi_dBm_x16 = rr_sglte_get_stored_serv_RSSI(gas_id);
      handover_details_ptr->interrat_measurement_data.interrat_measurements[0].frequency_offset= 0;
      handover_details_ptr->synchronisation_indication = SYNCHRONISED;
    }
    else
    {
    
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
      rr_clear_system_information(&scell_info_ptr->gsm);    
    }
		
    MSG_GERAN_HIGH_1_G(" Serving ARFCN was %d ",servarfcn.num);
    MSG_GERAN_HIGH_1_G(" ARFCN in HO is %d ",handover_details_ptr->BCCH_ARFCN.num);
    MSG_GERAN_HIGH_1_G(" Fn offset is %d",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].fn_offset);
    MSG_GERAN_HIGH_1_G(" Qbit_offset is %d ",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].qbit_offset);
    MSG_GERAN_HIGH_1_G(" BSIC NCC is %d ",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].bsic_ncc);
    MSG_GERAN_HIGH_1_G(" BSIC BCC is %d ",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].bsic_bcc);
    MSG_GERAN_HIGH_1_G(" RSSI  is %d ",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].rssi_dBm_x16);
    MSG_GERAN_HIGH_1_G(" Frequency offset   is %d ",handover_details_ptr->interrat_measurement_data.interrat_measurements[0].frequency_offset);
  }


#endif /*FEATURE_SGLTE*/



  mph_handover_req.handover_synchronisation = handover_details_ptr->synchronisation_indication;

  mph_handover_req.BCCH_ARFCN = handover_details_ptr->BCCH_ARFCN;
  mph_handover_req.real_time_difference_valid = handover_details_ptr->real_time_difference_valid;
  mph_handover_req.real_time_difference = handover_details_ptr->real_time_difference;
  mph_handover_req.NCI = handover_details_ptr->NCI;
  mph_handover_req.timing_advance_valid = handover_details_ptr->timing_advance_valid;
  mph_handover_req.timing_advance = handover_details_ptr->timing_advance;
  mph_handover_req.channel_spec.channel_mode_1 = handover_details_ptr->channel_spec.channel_mode_1;
  mph_handover_req.channel_spec.channel_mode_2 = handover_details_ptr->channel_spec.channel_mode_2;

  mph_handover_req.handover_type = handover_details_ptr->handover_type;


#ifdef FEATURE_WCDMA
  mph_handover_req.interrat_measurement_data = handover_details_ptr->interrat_measurement_data;
#endif /* FEATURE_WCDMA */

  mph_handover_req.old_channel_mode = handover_details_ptr->old_channel_mode;
  mph_handover_req.new_channel_mode = handover_details_ptr->new_channel_mode;

  /*Looks like the compiler is not handling a straight BSIC copy, TODO Needs closer look*/
  mph_handover_req.BSIC.PLMN_colour_code = handover_details_ptr->BSIC.PLMN_colour_code;
  mph_handover_req.BSIC.BS_colour_code = handover_details_ptr->BSIC.BS_colour_code;

  memscpy(mph_handover_req.cipher_key,
  	      sizeof(mph_handover_req.cipher_key),
  	      handover_details_ptr->cipher_key,
  	      MM_SIM_EFKC_SIZE
  	      );

  memscpy(mph_handover_req.cipher_key_128,sizeof(mph_handover_req.cipher_key_128),handover_details_ptr->cipher_key_128,CIPHER_KEY_128_LENGTH);

  mph_handover_req.multirate_config = *multirate_config_ptr;

  rr_eng_mode_update_dtx_ul_status(
    mph_handover_req.channel_spec.DTX_indicator,
    gas_id
  );

  rr_l1_send_message_to_l1((IMH_T *)&mph_handover_req, gas_id);

  /* Update RR's global timing advance info */
  rr_l1_send_update_current_timing_advance_info(
    handover_details_ptr->timing_advance_valid,
    handover_details_ptr->timing_advance,
    gas_id
  );
}


/**
  @brief Sends MPH_SET_CIPHERING_MODE_REQ to GL1.
*/
void rr_send_mph_set_ciphering_req(
  boolean cipher_flag,
  cipher_algorithm_T cipher_algorithm,
  byte *cipher_key,
  byte *cipher_key_128,
  const gas_id_t gas_id
)
{
  mph_set_ciphering_mode_req_T mph_set_ciphering_mode_req;
  byte index;

  RR_NULL_CHECK_FATAL(cipher_key);

  RR_PREPARE_L1_MESSAGE(mph_set_ciphering_mode_req, MPH_SET_CIPHERING_MODE_REQ, gas_id);

  mph_set_ciphering_mode_req.cipher_flag = cipher_flag;
  mph_set_ciphering_mode_req.cipher_algorithm = cipher_algorithm;

  for (index = 0; index < CIPHER_KEY_LENGTH; index++)
  {
    mph_set_ciphering_mode_req.cipher_key[index] = cipher_key[index];
  }

  for (index = 0; index < CIPHER_KEY_128_LENGTH; index++)
  {
    mph_set_ciphering_mode_req.cipher_key_128[index] = cipher_key_128[index];
  }

  rr_l1_send_message_to_l1( (IMH_T *)&mph_set_ciphering_mode_req, gas_id );
}


/**
  @brief Sends MPH_STOP_HANDOVER_ACCESS_REQ to GL1.
*/
void rr_send_mph_stop_handover_access_req(
  byte timing_advance,
  const gas_id_t gas_id
)
{
  mph_stop_handover_access_req_T mph_stop_handover_access_req;

  RR_PREPARE_L1_MESSAGE(mph_stop_handover_access_req, MPH_STOP_HANDOVER_ACCESS_REQ, gas_id);

  mph_stop_handover_access_req.timing_advance = timing_advance;

  rr_l1_send_message_to_l1((IMH_T *)&mph_stop_handover_access_req, gas_id);

  /* Update RR's global timing advance information so LSM task can retrieve it */
  rr_l1_send_update_current_timing_advance_info(
    TRUE,             // timing_advance_valid
    timing_advance,   // timing_advance
    gas_id            // gas_id
  );
}


/**
 * Send the MPH_CHANNEL_RELEASE_REQ primitive to layer 1
 *
 * @param valid_bcch_carrier   if TRUE indicates that valid BCCH information is provided and should be used
 *                             to attempt to acquire the specified BCCH carrier
 *
 * @param serving_bcch_carrier ARFCN of the BCCH carrier that should be acquired
 *
 * @param BSIC                 BSIC of the BCCH carrier that should be acquired
 */
void rr_send_mph_channel_release_req(
  boolean        valid_bcch_carrier,
  ARFCN_T        serving_bcch_carrier,
  BSIC_T         BSIC,
  const gas_id_t gas_id
)
{
  mph_channel_release_req_T mph_channel_release_req;

  RR_PREPARE_L1_MESSAGE(mph_channel_release_req, MPH_CHANNEL_RELEASE_REQ, gas_id);

  mph_channel_release_req.valid_bcch_carrier = valid_bcch_carrier;
  mph_channel_release_req.serving_bcch_carrier = serving_bcch_carrier;
  mph_channel_release_req.BSIC = BSIC;

  rr_l1_send_message_to_l1((IMH_T *)&mph_channel_release_req, gas_id);
  /* log the loss of service in RR */

  /* Invalidate RR's global timing advance information */
  rr_l1_send_invalidate_current_timing_advance_info(gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* rr_send_mph_channel_release_req */


/**
 * Sends MPH_DECODE_BCCH_LIST_ABORT_REQ
 */
void rr_send_mph_decode_bcch_list_abort_req(
  const gas_id_t gas_id
)
{
  mph_decode_bcch_list_abort_req_T mph_decode_bcch_list_abort_req;

  RR_PREPARE_L1_MESSAGE(mph_decode_bcch_list_abort_req, MPH_DECODE_BCCH_LIST_ABORT_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_decode_bcch_list_abort_req, gas_id);
} /* rr_send_mph_decode_abort_bcch_list_req */


/**
  @brief Sends MPH_POWER_SCAN_ABORT_REQ to GL1.
*/
void rr_send_mph_power_scan_abort_req(
  const gas_id_t gas_id
)
{
  mph_power_scan_abort_req_T mph_power_scan_abort_req;

  RR_PREPARE_L1_MESSAGE(mph_power_scan_abort_req, MPH_POWER_SCAN_ABORT_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_power_scan_abort_req, gas_id);
}


/**
 * Sends MPH_DECODE_BCCH_LIST_REQ
 *
 * @param frequency_list list of frequencies to decode
 */
void rr_send_mph_decode_bcch_list_req(
  rr_l1_idle_freq_list_T *frequency_list,
  boolean immediate_search,
  boolean si3_only,
  const gas_id_t gas_id)
{
  mph_decode_bcch_list_req_T mph_decode_bcch_list_req;

  RR_NULL_CHECK_FATAL(frequency_list);

  RR_PREPARE_L1_MESSAGE(mph_decode_bcch_list_req, MPH_DECODE_BCCH_LIST_REQ, gas_id);

  if (immediate_search)
  {
    mph_decode_bcch_list_req.search_mode = RR_SEARCH_NORMAL;
  }
  else
  {
    mph_decode_bcch_list_req.search_mode = RR_SEARCH_BACKGROUND;
#if  defined(FEATURE_DUAL_SIM)
    /* explicitly clear the retry status during a background search */
    rr_cell_selection_clear_decode_bcch_retry_status(gas_id);
#endif /* FEATURE_GSM_RGS_MULTIPASS && FEATURE_DUAL_SIM */
  }

  mph_decode_bcch_list_req.frequency_list = *frequency_list;

  mph_decode_bcch_list_req.retry_with_reset = rr_cell_selection_get_decode_bcch_retry_status(gas_id);

  mph_decode_bcch_list_req.si3_only = si3_only;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_l1_blocking_message_sent(MPH_DECODE_BCCH_LIST_CNF, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_decode_bcch_list_req, gas_id );
} /* rr_send_mph_decode_bcch_list_req */


/**
 * send the MPH_POWER_SCAN_REQ primitive to the layer 1
 *
 * @param pscan_results_buffer_ptr
 *                    ptr to buffer L1 will use to store results in
 *
 * @param requested_bands bitmap of bands that should be searched
 */
void rr_send_mph_power_scan_req(
  rr_l1_pscan_results_T *pscan_results_buffer_ptr,
  rr_l1_band_bitmap_T    requested_bands,
  rr_search_mode_T       search_mode,
  const gas_id_t         gas_id
)
{
  mph_power_scan_req_T mph_power_scan_req;

  RR_NULL_CHECK_FATAL(pscan_results_buffer_ptr);

  RR_PREPARE_L1_MESSAGE(mph_power_scan_req, MPH_POWER_SCAN_REQ, gas_id);

  mph_power_scan_req.search_mode = search_mode;
  mph_power_scan_req.pscan_results_buffer_ptr = pscan_results_buffer_ptr;
  mph_power_scan_req.requested_bands = requested_bands;

#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
  if (rr_get_ue_oos_flag(gas_id))
  {
    rr_set_ue_oos_flag(FALSE, gas_id);
    mph_power_scan_req.toggle_antenna = TRUE;
  }
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

#ifndef PERLUTF
  /* check for any results shared by other subscriptions */
  rr_pscan_prepare_common_results(&mph_power_scan_req);
#endif /* !PERLUTF */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (search_mode != RR_SEARCH_BACKGROUND)
  {
    /* for RPS, notify the DSDS controller that we're trying to recover */
    if (search_mode == RR_SEARCH_RESELECTION)
    {
      rr_ms_service_recovery_started(gas_id);
    }

    /* Dual SIM module tracks the responsiveness of the L1 interface */
    rr_ms_l1_blocking_message_sent(MPH_POWER_SCAN_CNF, gas_id);
  }
  else
  {
    /* Scans in background mode are not tracked by the multi-SIM controller */
    /* because they can always be aborted with legacy RR mechanisms instead */
    MSG_GERAN_HIGH_0_G("Background power scan initiated (not tracked)");
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_power_scan_req, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* rr_send_power_scan_req */


/**
  @brief Sends MPH_FREQUENCY_REDEFINITION_REQ to GL1.
*/
void rr_send_mph_frequency_redefinition_req(
  channel_information_T *p_channel_information,
  word starting_time,
  const gas_id_t gas_id
)
{
  mph_frequency_redefinition_req_T mph_frequency_redefinition_req;

  RR_NULL_CHECK_FATAL(p_channel_information);

  RR_PREPARE_L1_MESSAGE(mph_frequency_redefinition_req, MPH_FREQUENCY_REDEFINITION_REQ, gas_id);

  mph_frequency_redefinition_req.channel_information = *p_channel_information;
  mph_frequency_redefinition_req.starting_time[0] = (byte) (starting_time / 256);
  mph_frequency_redefinition_req.starting_time[1] = (byte) (starting_time % 256);

  rr_l1_send_message_to_l1((IMH_T *)&mph_frequency_redefinition_req, gas_id );
}


/**
  @brief Sends MPH_CHANGE_PAGE_MODE_REQ to GL1.
*/
void rr_send_mph_change_page_mode_req(
  paging_mode_T page_mode,
  const gas_id_t gas_id
)
{
  mph_change_page_mode_req_T mph_change_page_mode_req;
  boolean valid_page_mode = TRUE;

  switch (page_mode)
  {
    case PAGING_REORGANIZATION:
    {
      MSG_GERAN_HIGH_0_G("Setting L1 Page Mode to PAGING_REORGANIZATION");
      break;
    }

    case EXTENDED_PAGING:
    {
      MSG_GERAN_HIGH_0_G("Setting L1 Page Mode to EXTENDED_PAGING");
      break;
    }

    case NORMAL_PAGING:
    {
      MSG_GERAN_HIGH_0_G("Setting L1 Page Mode to NORMAL_PAGING");
      break;
    }

    default:
    {
      MSG_GERAN_HIGH_1_G("Invalid Page Mode value (%d)",(int)page_mode);
      valid_page_mode = FALSE;
      break;
    }
  }

  if (valid_page_mode)
  {
    RR_PREPARE_L1_MESSAGE(mph_change_page_mode_req, MPH_CHANGE_PAGE_MODE_REQ, gas_id);

    mph_change_page_mode_req.paging_mode = page_mode;

    rr_l1_send_message_to_l1((IMH_T *)&mph_change_page_mode_req, gas_id);
  }
}


/**
  @brief Sends MPH_DEDICATED_UPDATE_REQ to GL1.
*/
void rr_send_mph_dedicated_update_req(
  inter_task_BCCH_allocation_T *BCCH_allocation,
  boolean                       DTX_indicator,
  byte                          PWRC,
  byte                          NCC_permitted,
  byte                          multiband_reporting,
  const gas_id_t                gas_id
)
{
  mph_dedicated_update_req_T mph_dedicated_update_req;
  int index;

  RR_NULL_CHECK_FATAL(BCCH_allocation);

  RR_PREPARE_L1_MESSAGE(mph_dedicated_update_req, MPH_DEDICATED_UPDATE_REQ, gas_id);

  mph_dedicated_update_req.BA_list.no_of_entries = BCCH_allocation->no_of_entries;

  for (index = 0; index < BCCH_allocation->no_of_entries; index++)
  {
    mph_dedicated_update_req.BA_list.BCCH_ARFCN[index] = BCCH_allocation->BCCH_ARFCN[index];
  }

  rr_log_ba_list(
    &(mph_dedicated_update_req.BA_list),
    gas_id
  );

  mph_dedicated_update_req.DTX_indicator = DTX_indicator;
  mph_dedicated_update_req.PWRC = PWRC;
  mph_dedicated_update_req.NCC_permitted = NCC_permitted;
  mph_dedicated_update_req.multiband_reporting = multiband_reporting;

  rr_eng_mode_update_dtx_ul_status(
    mph_dedicated_update_req.DTX_indicator,
    gas_id
  );

  rr_l1_send_message_to_l1((IMH_T *)&mph_dedicated_update_req, gas_id);
}


/**
  @brief Sends MPH_IMMED_ASSIGN_REQ to GL1.
*/
void rr_send_mph_immed_assign_req(
  boolean DTX_indicator,
  byte timing_advance,
  boolean starting_time_valid,
  word starting_time,
  boolean before_parameters_valid,
  channel_information_T *p_channel_info_before,
  channel_information_T *p_channel_info_after,
  const gas_id_t gas_id
)
{
  mph_immed_assign_req_T mph_immed_assign_req;

  RR_NULL_CHECK_FATAL(p_channel_info_before);
  RR_NULL_CHECK_FATAL(p_channel_info_after);

  RR_PREPARE_L1_MESSAGE(mph_immed_assign_req, MPH_IMMED_ASSIGN_REQ, gas_id);

  mph_immed_assign_req.DTX_indicator = DTX_indicator;
  mph_immed_assign_req.timing_advance = timing_advance;
  mph_immed_assign_req.starting_time_valid = starting_time_valid;
  mph_immed_assign_req.starting_time[0] = (byte) (starting_time / 256);
  mph_immed_assign_req.starting_time[1] = (byte) (starting_time % 256);
  mph_immed_assign_req.channel_info_before = *p_channel_info_before;
  mph_immed_assign_req.channel_info_after = *p_channel_info_after;
  mph_immed_assign_req.before_parameters_valid = before_parameters_valid;

  rr_eng_mode_update_dtx_ul_status(
    mph_immed_assign_req.DTX_indicator,
    gas_id
  );

  rr_l1_send_message_to_l1((IMH_T *)&mph_immed_assign_req, gas_id);

  /* Update RR's global timing advance information so the LSM task can
   * read it when needed.
   */
  rr_l1_send_update_current_timing_advance_info(
    TRUE,             // timing_advance_valid
    timing_advance,   // timing_advance
    gas_id
  );
}


/**
  @brief Sends MPH_RECONNECT_CHANNEL_REQ to GL1.
*/
void rr_send_mph_reconnect_channel_req(
  const gas_id_t gas_id
)
{
  mph_reconnect_channel_req_T reconnect_req;
  rr_dedicated_data_T        *dedic_ptr;

  RR_PREPARE_L1_MESSAGE(reconnect_req, MPH_RECONNECT_CHANNEL_REQ, gas_id);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    /* Provide L1 with the previous channel spec and AMR config */
    reconnect_req.channel_spec = dedic_ptr->prev_channel_spec;
	reconnect_req.multirate_config = dedic_ptr->prev_amr_config;
  }

  rr_eng_mode_update_dtx_ul_status(
    reconnect_req.channel_spec.DTX_indicator,
    gas_id
  );

  rr_l1_send_message_to_l1((IMH_T *)&reconnect_req, gas_id);
} /* end rr_send_mph_reconnect_channel_req() */


/**
 * Send the MPH_SELECT_SPECIFIC_BCCH_REQ primitive to the layer 1
 *
 * @param specific_channel ARFCN of the cell that should be acquired by L1
 */
void rr_send_mph_select_specific_channel(
  ARFCN_T specific_channel,
  BSIC_T BSIC,
  const gas_id_t gas_id
)
{
  mph_select_specific_bcch_req_T mph_select_specific_bcch_req;

  RR_PREPARE_L1_MESSAGE(mph_select_specific_bcch_req, MPH_SELECT_SPECIFIC_BCCH_REQ, gas_id);

  mph_select_specific_bcch_req.specific_channel_no = specific_channel;
  mph_select_specific_bcch_req.bsic = BSIC;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_l1_blocking_message_sent(MPH_SELECT_SPECIFIC_BCCH_CNF, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_select_specific_bcch_req, gas_id);

  /* log the loss of service in RR */

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* rr_send_mph_select_specific_channel */


/**
 * Send the MPH_START_IDLE_REQ primitive to the layer 1
 *
 * @param p_layer1_sys_info pointer to the L1 system information structure
 *
 * @param imsi_valid        indicates if the IMSI information is valid or not
 *
 * @param imsi_mod_1000     imsi mod 1000 information
 */
void rr_send_mph_start_idle_req(
  layer1_sys_info_T *p_layer1_sys_info,
  boolean            imsi_valid,
  word               imsi_mod_1000,
  boolean            decode_bcch,
  const gas_id_t     gas_id
)
{
  mph_start_idle_req_T mph_start_idle_req;

  RR_NULL_CHECK_FATAL(p_layer1_sys_info);

  RR_PREPARE_L1_MESSAGE(mph_start_idle_req, MPH_START_IDLE_REQ, gas_id);

  mph_start_idle_req.layer1_sys_info = *p_layer1_sys_info;
  mph_start_idle_req.imsi_valid = imsi_valid;

  mph_start_idle_req.imsi_mod_1000.msb = (byte) (imsi_mod_1000 / 256);
  mph_start_idle_req.imsi_mod_1000.lsb = (byte) (imsi_mod_1000 % 256);

  mph_start_idle_req.decode_bcch = decode_bcch;

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(&mph_start_idle_req.xmsi, gas_id);
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (imsi_valid == TRUE)
  {
    rr_ms_store_paging_info(p_layer1_sys_info, imsi_mod_1000, gas_id);
  }
  else
  {
    rr_ms_store_paging_info(NULL, 0, gas_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_start_idle_req, gas_id);

  /* log that RR is now in service */
  rr_log_service_change(TRUE, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_GSM_IDLE, gas_id);

#ifdef FEATURE_FAKE_BTS_CELL_INFO
  if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
  {
    rr_send_gcell_info_ind(gas_id);
  }
 #endif /*FEATURE_FAKE_BTS_CELL_INFO*/

} /* rr_send_mph_start_idle_req */


#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

static void rr_ms_process_early_camp(
  control_channel_description_T *ccd_ptr,
  word imsi_mod_1000,
  const gas_id_t gas_id
)
{
  if (ccd_ptr != NULL)
  {
    layer1_sys_info_T l1_sys_info;

    l1_sys_info.BS_AG_BLKS_RES      = ccd_ptr->BS_AG_BLKS_RES;
    l1_sys_info.BS_CC_CHANS         = ccd_ptr->BS_CC_CHANS;
    l1_sys_info.BS_CCCH_SDCCH_COMB  = ccd_ptr->BS_CCCH_SDCCH_COMB;
    l1_sys_info.BS_PA_MFRMS         = ccd_ptr->BS_PA_MFRMS;

    /* pass the parameters into the DSDS controller */
    rr_ms_store_paging_info(
      &l1_sys_info,
      imsi_mod_1000,
      gas_id
    );
  }
  else
  {
    /* invalidate any previously stored parameters */
    rr_ms_store_paging_info(
      NULL,
      0,
      gas_id
    );
  }

  return;
}

/**
 * Determines whether reorganisation mode override is necessary
 *
 * @param gas_id
 *
 * @return boolean - TRUE if reorg mode should be applied
 */
static boolean rr_use_reorg_mode_for_early_camping(const gas_id_t gas_id)
{
  /* Paging reorganisation mode may be invoked to protect "CS Fallback" procedures against */
  /* pre-emption by other clients in multi-standby mode. Once camping is completed, the UE */
  /* will either a) transition to connection establishment, or b) fail due to T304 expiry. */
  /* Both of these cases should be transitory and hence risk of power impact is negligible */
  if (geran_in_multi_standby_mode())
  {
    if (rr_is_csfb_call_in_progress(gas_id))
    {
      MSG_GERAN_HIGH_0_G("Overriding paging mode to REORG for CSFB protection");
      return(TRUE);
    }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE_TO_GSM_CCO)
    else if (rr_x2g_cco_for_csfb_in_progress(gas_id))
    {
      MSG_GERAN_HIGH_0_G("Overriding paging mode to REORG for CSFB from X2G CCO");
      return(TRUE);
    }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE_TO_GSM_CCO */
    else if (geran_was_lock_exchanged(GERAN_EXCHANGE_XTOG, gas_id))
    {
      MSG_GERAN_HIGH_0_G("X2G lock exchanged but CSFB not in progress");
    }
  }

  return(FALSE);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/**
 * Send the mph_early_camping_req primitive to the layer 1
 *
 * @param control_channel_description
 *                      Structure of the control channel needed to listen to pages
 *
 * @param imsi_valid    indicates if IMSI information is valid
 *
 * @param imsi_mod_1000 imsi information
 *
 * @param paging_mode
 */
void rr_send_mph_early_camping_req (
  control_channel_description_T *control_channel_description,
  boolean                        imsi_valid,
  word                           imsi_mod_1000,
  paging_mode_T                  paging_mode,
  const gas_id_t                 gas_id
)
{
  mph_early_camping_req_T mph_early_camping_req;

  RR_PREPARE_L1_MESSAGE(mph_early_camping_req, MPH_EARLY_CAMPING_REQ, gas_id);

  if (control_channel_description != NULL)
  {
    mph_early_camping_req.BS_AG_BLKS_RES      = control_channel_description->BS_AG_BLKS_RES;
    mph_early_camping_req.BS_CC_CHANS         = control_channel_description->BS_CC_CHANS;
    mph_early_camping_req.BS_CCCH_SDCCH_COMB  = control_channel_description->BS_CCCH_SDCCH_COMB;
    mph_early_camping_req.BS_PA_MFRMS         = control_channel_description->BS_PA_MFRMS;
  }

  mph_early_camping_req.imsi_valid = imsi_valid;
  mph_early_camping_req.imsi_mod_1000.msb = (byte) (imsi_mod_1000 / 256);
  mph_early_camping_req.imsi_mod_1000.lsb = (byte) (imsi_mod_1000 % 256);

  mph_early_camping_req.paging_mode = paging_mode;

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(&mph_early_camping_req.xmsi, gas_id);
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (imsi_valid == TRUE)
  {
    rr_ms_process_early_camp(control_channel_description, imsi_mod_1000, gas_id);
  }
  else
  {
    rr_ms_process_early_camp(NULL, 0, gas_id);
  }

  /* check whether reorg override needs to be used here */
  if (rr_use_reorg_mode_for_early_camping(gas_id))
  {
    mph_early_camping_req.paging_mode = PAGING_REORGANIZATION;
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_early_camping_req, gas_id);
  /* log that RR is now in service */
  rr_log_service_change(TRUE, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_EARLY_CAMP_IDLE, gas_id);

  return;
} /* rr_send_mph_early_camping_req */


/**
 * Send the MPH_IDLE_UPDATE_REQ primitive to the layer 1
 *
 * @param p_layer1_sys_info pointer to the layer 1 system information that should be used
 *
 * @param imsi_valid        indicates if the IMSI is valid
 *
 * @param imsi_mod_1000     IMSI mod 1000 value to use
 */
void rr_send_mph_idle_update_req(
  layer1_sys_info_T * p_layer1_sys_info,
  boolean imsi_valid,
  word imsi_mod_1000,
  const gas_id_t gas_id
)
{
  mph_idle_update_req_T mph_idle_update_req;

  RR_NULL_CHECK_FATAL(p_layer1_sys_info);

  RR_PREPARE_L1_MESSAGE(mph_idle_update_req, MPH_IDLE_UPDATE_REQ, gas_id);

  mph_idle_update_req.layer1_sys_info = *p_layer1_sys_info;
  rr_log_ba_list(&(mph_idle_update_req.layer1_sys_info.BA_list), gas_id);
  mph_idle_update_req.imsi_valid = imsi_valid;

  mph_idle_update_req.imsi_mod_1000.msb = (byte) (imsi_mod_1000 / 256);
  mph_idle_update_req.imsi_mod_1000.lsb = (byte) (imsi_mod_1000 % 256);

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(&mph_idle_update_req.xmsi, gas_id);
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (imsi_valid == TRUE)
  {
    rr_ms_store_paging_info(
      p_layer1_sys_info,
      imsi_mod_1000,
      gas_id
    );
  }
  else
  {
    rr_ms_store_paging_info(
      NULL,
      0,
      gas_id
    );
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_idle_update_req, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_GSM_IDLE, gas_id);
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
  {
    rr_send_gcell_info_ind(gas_id);
  }
 #endif /*FEATURE_FAKE_BTS_CELL_INFO*/

}


/**
  @brief Sends MPH_READ_SYS_INFO_REQ to GL1.
*/
void rr_send_mph_read_sys_info_req(
  rr_l1_si_type_T si_required,
  const gas_id_t gas_id
)
{
  mph_read_sys_info_req_T  mph_read_sys_info_req;
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/


  RR_PREPARE_L1_MESSAGE(mph_read_sys_info_req, MPH_READ_SYS_INFO_REQ, gas_id);

  mph_read_sys_info_req.si_mask = si_required;
  mph_read_sys_info_req.replace_prev_mask = TRUE;

  print_si_bitmask(si_required);

  rr_l1_send_message_to_l1((IMH_T *)&mph_read_sys_info_req, gas_id);
#ifdef FEATURE_FAKE_BTS_CELL_INFO
  if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
  {
    MSG_GERAN_HIGH_2_G("SI required is 0x%x, si_refresh_ongoing mask is %d",(int)si_required, scell->bcch_sys_info.si_refresh_ongoing);
    if (( si_required == 0x00 ) && (scell->bcch_sys_info.si_refresh_ongoing == TRUE))
    {
      rr_send_gcell_info_ind(gas_id);
      scell->bcch_sys_info.si_refresh_ongoing = FALSE;
    }
  }
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/


} /* end rr_send_mph_read_sys_info_req() */


/**
  @brief Sends MPH_ABORT_RA_REQ to GL1.
*/
void rr_send_mph_abort_ra_req(
  const gas_id_t gas_id
)
{
  mph_abort_ra_req_T mph_abort_ra;

  RR_PREPARE_L1_MESSAGE(mph_abort_ra, MPH_ABORT_RA_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_abort_ra, gas_id);

} /* end rr_send_mph_abort_ra_req() */


/**
  @brief Send the MPH_STOP_GSM_MODE_REQ primitive to the GL1.

  @param gsm_stop_reason The stop reason that should be sent to GL1.
*/
void rr_send_mph_stop_gsm_mode_req(
  rr_l1_gsm_stop_reason_T gsm_stop_reason,
  const gas_id_t gas_id
)
{
  mph_stop_gsm_mode_req_T mph_stop_gsm_mode_req;

  RR_PREPARE_L1_MESSAGE(mph_stop_gsm_mode_req, MPH_STOP_GSM_MODE_REQ, gas_id);

  mph_stop_gsm_mode_req.rr_l1_gsm_stop_reason = gsm_stop_reason;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  mph_stop_gsm_mode_req.g2x_resel_in_progress = rr_resel_g2w_in_progress(gas_id);
#else
  mph_stop_gsm_mode_req.g2x_resel_in_progress = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* ensure that GRR is not waiting on other signals from L1 */
  rr_ms_l1_blocking_message_sent(MPH_STOP_GSM_MODE_CNF, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_stop_gsm_mode_req, gas_id);

  // RR/L1 are no longer camped
  {
    gprs_scell_info_t *cell_info = rr_gprs_get_scell_info(gas_id);

    if (cell_info != NULL)
    {
      cell_info->l1_idle_mode = GRR_L1_IDLE_MODE_NONE;
    }
  }

  /* log the loss of service in RR */
  rr_log_service_change(FALSE, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* rr_send_mph_stop_gsm_mode_req */


/**
  @brief Sends MPH_START_GSM_MODE_REQ to GL1.
*/
void rr_send_mph_start_gsm_mode_req(
  boolean x2g_resel_in_progress,
  boolean x2g_cco_in_progress,
  const gas_id_t gas_id
)
{
  mph_start_gsm_mode_req_T mph_start_gsm_mode_req;

  RR_PREPARE_L1_MESSAGE(mph_start_gsm_mode_req, MPH_START_GSM_MODE_REQ, gas_id);

  mph_start_gsm_mode_req.gsm_only = rr_gsm_only(gas_id);
  mph_start_gsm_mode_req.x2g_resel_in_progress = x2g_resel_in_progress;
  mph_start_gsm_mode_req.x2g_cco_in_progress = x2g_cco_in_progress;

  MSG_GERAN_MED_3_G("Starting L1 (gsm_only: %d, x2g_resel: %d, x2g_cco: %d)",
    (int)mph_start_gsm_mode_req.gsm_only,
    (int)mph_start_gsm_mode_req.x2g_resel_in_progress,
    (int)mph_start_gsm_mode_req.x2g_cco_in_progress);

  mph_start_gsm_mode_req.multi_sim_mode = geran_get_multi_sim_mode();

  /* before starting GL1, acquire the TRM lock using the exchange mechanism */
  /* which only succeeds for a multimode subscription during an X2G process */
  (void)geran_exchange_lock(GERAN_EXCHANGE_XTOG, gas_id);
  geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);

  MSG_GERAN_MED_2_G("MULTISIM=%d X2G=%d",
    (int)mph_start_gsm_mode_req.multi_sim_mode,
    (int)geran_was_lock_exchanged(GERAN_EXCHANGE_XTOG, gas_id));

#ifdef FEATURE_SGLTE
  mph_start_gsm_mode_req.sglte_mode_active = rr_is_sglte_mode_active(gas_id);
#else
  mph_start_gsm_mode_req.sglte_mode_active = FALSE;
#endif /*FEATURE_SGLTE*/

  rr_l1_send_message_to_l1((IMH_T *)&mph_start_gsm_mode_req, gas_id);

} /* rr_send_mph_start_gsm_mode_req */

/**
  @brief Sends MPH_DEVICE_MODE_IND to GL1
*/
void rr_send_mph_device_mode_ind(const geran_device_mode_cause_t mode_cause, const gas_id_t gas_id)
{
  mph_device_mode_ind_T mph_device_mode_ind;

  RR_PREPARE_L1_MESSAGE(mph_device_mode_ind, MPH_DEVICE_MODE_IND, gas_id);

  /* retrieve the current mode (for GERAN) from GCOMMON utility */
  mph_device_mode_ind.multi_sim_mode = geran_get_multi_sim_mode();

  /* set payload to indicate the cause of the mode change */
  mph_device_mode_ind.device_mode_cause = mode_cause;

  MSG_GERAN_MED_2_G("MPH_DEVICE_MODE_IND MULTISIM=%d CAUSE=%d",
      (int)(mph_device_mode_ind.multi_sim_mode),
      (int)(mph_device_mode_ind.device_mode_cause));

  rr_l1_send_message_to_l1((IMH_T *)&mph_device_mode_ind, gas_id);

  return;
}

/**
  @brief Sends MPH_SET_PRIORITY_IND to GL1
*/
void rr_send_mph_set_priority_ind(const geran_priority_t l1_priority, const boolean new_procedure, const gas_id_t gas_id)
{
  mph_set_priority_ind_T mph_set_priority_ind;

  RR_PREPARE_L1_MESSAGE(mph_set_priority_ind, MPH_SET_PRIORITY_IND, gas_id);

  /* load the message with the parameters */
  mph_set_priority_ind.l1_priority   = l1_priority;
  mph_set_priority_ind.new_procedure = new_procedure;

  MSG_GERAN_MED_2_G("MPH_SET_PRIORITY_IND Priority=0x%02X NewProc=%1d",
                    (int)l1_priority, (int)new_procedure);

  rr_l1_send_message_to_l1((IMH_T *)&mph_set_priority_ind, gas_id);

  return;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#ifdef FEATURE_LTE

/*!
  @brief Adds an entry to the given Frequency/Priority list.

  @param dst The list to modify.
  @param rat The RAT of the frequency to be added.
  @param arfcn The ARFCN of the frequency to be added.
  @param priority The priority of the frequency to be added, relative to
         GERAN priority.
*/
void rr_irat_frequency_priority_list_add(
  rr_l1_irat_frequency_priority_list_t * dst,
  rr_l1_irat_rat_e                       rat,
  uint32                                 arfcn,
  uint8                                  priority,
  const gas_id_t                         gas_id //tlx
)
{
  uint32 i;
  boolean found = FALSE;

  RR_NULL_CHECK_FATAL(dst);

  for (i = 0; i < dst->count && i < MAX_RR_L1_PRIORITY_ENTRIES; ++i)
  {
    if (dst->entries[i].rat == rat &&
        dst->entries[i].arfcn == arfcn)
    {
      found = TRUE;
      break;
    }
  }

  if (found == FALSE)
  {
    if (i < MAX_RR_L1_PRIORITY_ENTRIES)
    {
      dst->entries[i].rat = rat;
      dst->entries[i].arfcn = arfcn;
      dst->entries[i].priority = priority;
      ++dst->count;
    }
  }
}

#ifdef FEATURE_WCDMA
/*!
  @brief Adds a UARFCN to a reselection frequency list if reselection to that
         UARFCN is permitted.

  The UARFCN is only added to the reselection frequency list if reselection to
  cells on the UARFCN is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  Additionally, if priority-based cell reselection is in use the UARFCN will
  only be added to the reselection frequency list if a priority has been
  assigned to it by the network.

  @param dst The list to add the UARFCN to.
  @param uarfcn The UARFCN to add to the list.
  @param priority_and_threshold_information_available TRUE if priority and
         threshold information are available and the priority-based cell
         reselection algorithm should be used, FALSE otherwise.
*/
static void rr_wcdma_reselection_freq_list_add(
  wcdma_reselection_freq_list_T        * wcdma_reselection_freq_list,
  uint16                                 uarfcn,
  boolean                                priority_and_threshold_information_available,
  rr_l1_irat_frequency_priority_list_t * frequency_priority_list,
  const gas_id_t                         gas_id
)
{
  RR_NULL_CHECK_FATAL(wcdma_reselection_freq_list);
  RR_NULL_CHECK_FATAL(frequency_priority_list);

  if (rr_g2x_resel_to_wcdma_frequency_is_allowed(
        uarfcn,
        rr_band_pref_wcdma(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (priority_and_threshold_information_available)
    {
      /* Reselection to IRAT UARFCNs is only possible if a priority has been
      assigned to that UARFCN by the network.  Otherwise, measurements should
      not be performed. */
      rr_resel_priority_e utran_priority = rr_get_resel_priority_for_uarfcn(
        uarfcn,
        RR_UTRAN_FDD,
        gas_id
      );

      if (utran_priority == RR_RESEL_PRIORITY_UNDEFINED)
      {
        return;
      }
      else
      {
        rr_irat_frequency_priority_list_add(
          frequency_priority_list,
          RR_L1_IRAT_RAT_WCDMA,
          uarfcn,
          (uint8) utran_priority,
          gas_id
        );
      }
    }

    if (wcdma_reselection_freq_list->num_of_frequencies < MAX_RR_MPH_FREQ_UPDATE_LIST)
    {
      uint32 i;
      boolean found = FALSE;

      /* Only add the UARFCN if it is not already present. */
      for (i = 0; i < wcdma_reselection_freq_list->num_of_frequencies; ++i)
      {
        if (wcdma_reselection_freq_list->uarfcn[i] == uarfcn)
        {
          found = TRUE;
          break;
        }
      }
      if (found == FALSE)
      {
        MSG_GERAN_HIGH_1_G("Adding supported UARFCN to L1 3G nlist: %d", uarfcn);
        wcdma_reselection_freq_list->uarfcn[wcdma_reselection_freq_list->num_of_frequencies] = uarfcn;
        ++wcdma_reselection_freq_list->num_of_frequencies;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Unable to add UARFCN %d to reselection frequency list - overflow detected", uarfcn);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Reselection to UARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", uarfcn);
  }
}
#endif //FEATURE_WCDMA

#ifdef FEATURE_GSM_TDS
/*!
  @brief Adds a UARFCN to a reselection frequency list if reselection to that
         UARFCN is permitted.

  The UARFCN is only added to the reselection frequency list if reselection to
  cells on the UARFCN is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  Additionally, if priority-based cell reselection is in use the UARFCN will
  only be added to the reselection frequency list if a priority has been
  assigned to it by the network.

  @param dst The list to add the UARFCN to.
  @param uarfcn The UARFCN to add to the list.
  @param priority_and_threshold_information_available TRUE if priority and
         threshold information are available and the priority-based cell
         reselection algorithm should be used, FALSE otherwise.
*/
static void rr_utran_tdd_reselection_freq_list_add(
  utran_tdd_reselection_freq_list_T    * utran_tdd_reselection_freq_list,
  uint16                                 uarfcn,
  boolean                                priority_and_threshold_information_available,
  rr_l1_irat_frequency_priority_list_t * frequency_priority_list,
  const gas_id_t                         gas_id
)
{
  RR_NULL_CHECK_FATAL(utran_tdd_reselection_freq_list);
  RR_NULL_CHECK_FATAL(frequency_priority_list);

  if (rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
        uarfcn,
        rr_band_pref_utran_tdd(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (priority_and_threshold_information_available)
    {
      /* Reselection to IRAT UARFCNs is only possible if a priority has been
      assigned to that UARFCN by the network.  Otherwise, measurements should
      not be performed. */
      rr_resel_priority_e utran_priority = rr_get_resel_priority_for_uarfcn(
        uarfcn,
        RR_UTRAN_TDD,
        gas_id
      );

      if (utran_priority == RR_RESEL_PRIORITY_UNDEFINED)
      {
        MSG_GERAN_HIGH_1_G("Reselection to TDD UARFCN %d not possible (no priority assigned)", uarfcn);
        return;
      }
      else
      {
        rr_irat_frequency_priority_list_add(
          frequency_priority_list,
          RR_L1_IRAT_RAT_UTRAN_TDD,
          uarfcn,
          (uint8) utran_priority,
          gas_id
        );
      }
    }

    if (utran_tdd_reselection_freq_list->num_of_frequencies < MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)
    {
      uint32 i;
      boolean found = FALSE;

      /* Only add the UARFCN if it is not already present. */
      for (i = 0; i < utran_tdd_reselection_freq_list->num_of_frequencies; ++i)
      {
        if (utran_tdd_reselection_freq_list->uarfcn[i] == uarfcn)
        {
          found = TRUE;
          break;
        }
      }
      if (found == FALSE)
      {
        MSG_GERAN_HIGH_1_G("Adding supported TDD UARFCN to L1 3G nlist: %d", uarfcn);
        utran_tdd_reselection_freq_list->uarfcn[utran_tdd_reselection_freq_list->num_of_frequencies] = uarfcn;
        ++utran_tdd_reselection_freq_list->num_of_frequencies;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Unable to add TDD UARFCN %d to reselection frequency list - overflow detected", uarfcn);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Reselection to TDD UARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", uarfcn);
  }
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/*!
  @param priority_and_threshold_information_available TRUE if priority and
         threshold information are available and the priority-based cell
         reselection algorithm should be used, FALSE otherwise.
*/
void rr_wcdma_cell_list_add(
  wcdma_cell_list_T *                    cell_list,
  rr_utran_cell_T                        cell,
  boolean                                priority_and_threshold_information_available,
  rr_l1_irat_frequency_priority_list_t * frequency_priority_list,
  const gas_id_t                         gas_id
)
{
  RR_NULL_CHECK_FATAL(cell_list);
  RR_NULL_CHECK_FATAL(frequency_priority_list);

  if (rr_g2x_resel_to_wcdma_frequency_is_allowed(
        cell.uarfcn.uarfcn,
        rr_band_pref_wcdma(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (priority_and_threshold_information_available)
    {
      /* Reselection to IRAT UARFCNs is only possible if a priority has been
      assigned to that UARFCN by the network.  Otherwise, measurements should
      not be performed. */
      rr_resel_priority_e utran_priority = rr_get_resel_priority_for_uarfcn(
        cell.uarfcn.uarfcn,
        RR_UTRAN_FDD,
        gas_id
      );

      if (utran_priority == RR_RESEL_PRIORITY_UNDEFINED)
      {
        MSG_GERAN_HIGH_2_G("Reselection to FDD UARFCN %d, SC %d not possible (no priority assigned)",
                 cell.uarfcn.uarfcn, cell.cell_code);
        return;
      }
      else
      {
        rr_irat_frequency_priority_list_add(
          frequency_priority_list,
          RR_L1_IRAT_RAT_WCDMA,
          cell.uarfcn.uarfcn,
          (uint8) utran_priority,
          gas_id
        );
      }
    }

    MSG_GERAN_HIGH_2_G("Adding supported FDD UARFCN %d, SC %d to L1 3G nlist",
             cell.uarfcn.uarfcn, cell.cell_code);

    if (cell_list->num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
    {
      cell_list->cell_list[cell_list->num_of_cells].uarfcn = cell.uarfcn.uarfcn;
      cell_list->cell_list[cell_list->num_of_cells].diversity = cell.diversity;
      cell_list->cell_list[cell_list->num_of_cells].scrambling_code = cell.cell_code;
      ++cell_list->num_of_cells;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Unable to add FDD UARFCN %d, SC %d to cell list - overflow detected",
                cell.uarfcn.uarfcn, cell.cell_code);
    }
  }
  else
  {
     MSG_GERAN_HIGH_2_G("Reselection to FDD UARFCN %d, SC %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)",
             cell.uarfcn.uarfcn, cell.cell_code);
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*!
  @param priority_and_threshold_information_available TRUE if priority and
         threshold information are available and the priority-based cell
         reselection algorithm should be used, FALSE otherwise.
*/
static void rr_tds_cell_list_add(
  utran_tdd_cell_list_T *                cell_list,
  rr_utran_cell_T                        cell,
  boolean                                priority_and_threshold_information_available,
  rr_l1_irat_frequency_priority_list_t * frequency_priority_list,
  const gas_id_t                         gas_id
)
{
  RR_NULL_CHECK_FATAL(cell_list);
  RR_NULL_CHECK_FATAL(frequency_priority_list);

  if (rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
        cell.uarfcn.uarfcn,
        rr_band_pref_utran_tdd(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (priority_and_threshold_information_available)
    {
      /* Reselection to IRAT UARFCNs is only possible if a priority has been
      assigned to that UARFCN by the network.  Otherwise, measurements should
      not be performed. */
      rr_resel_priority_e utran_priority = rr_get_resel_priority_for_uarfcn(
        cell.uarfcn.uarfcn,
        RR_UTRAN_TDD,
        gas_id
      );

      if (utran_priority == RR_RESEL_PRIORITY_UNDEFINED)
      {
        MSG_GERAN_HIGH_2_G("Reselection to TDD UARFCN %d, CPID %d not possible (no priority assigned)",
                 cell.uarfcn.uarfcn, cell.cell_code);
        return;
      }
      else
      {
        rr_irat_frequency_priority_list_add(
          frequency_priority_list,
          RR_L1_IRAT_RAT_UTRAN_TDD,
          cell.uarfcn.uarfcn,
          (uint8) utran_priority,
          gas_id
        );
      }
    }

    MSG_GERAN_HIGH_2_G("Adding supported TDD UARFCN %d, CPID %d to L1 3G nlist",
             cell.uarfcn.uarfcn, cell.cell_code);

    if (cell_list->num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
    {
      cell_list->cell_list[cell_list->num_of_cells].uarfcn = cell.uarfcn.uarfcn;
      cell_list->cell_list[cell_list->num_of_cells].diversity = cell.diversity;
      cell_list->cell_list[cell_list->num_of_cells].cell_parameter_id = cell.cell_code;
      cell_list->cell_list[cell_list->num_of_cells].sync_case = cell.sync_case_tstd;
      ++cell_list->num_of_cells;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Unable to add TDD UARFCN %d, CPID %d to cell list - overflow detected",
                cell.uarfcn.uarfcn, cell.cell_code);
    }
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Reselection to TDD UARFCN %d, CPID %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)",
             cell.uarfcn.uarfcn, cell.cell_code);
  }
}
#endif /*FEATURE_GSM_TDS*/

#else /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
/*!
  @brief Adds a UARFCN to a reselection frequency list if reselection to that
         UARFCN is permitted.

  The UARFCN is only added to the reselection frequency list if reselection to
  cells on the UARFCN is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  @param dst The list to add the UARFCN to.
  @param uarfcn The UARFCN to add to the list.
*/
static void rr_wcdma_reselection_freq_list_add(
  wcdma_reselection_freq_list_T * dst,
  uint16                          uarfcn,
  const gas_id_t                  gas_id
)
{
  RR_NULL_CHECK_FATAL(dst);

  if (rr_g2x_resel_to_wcdma_frequency_is_allowed(
        uarfcn,
        rr_band_pref_wcdma(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (dst->num_of_frequencies < MAX_RR_MPH_FREQ_UPDATE_LIST)
    {
      uint32 i;
      boolean found = FALSE;

      /* Only add the UARFCN if it is not already present. */
      for (i = 0; i < dst->num_of_frequencies; ++i)
      {
        if (dst->uarfcn[i] == uarfcn)
        {
          found = TRUE;
          break;
        }
      }
      if (found == FALSE)
      {
        MSG_GERAN_HIGH_1_G("Adding supported FDD UARFCN to L1 3G nlist: %d", uarfcn);
        dst->uarfcn[dst->num_of_frequencies] = uarfcn;
        ++dst->num_of_frequencies;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Unable to add FDD UARFCN %d to reselection frequency list - overflow detected", uarfcn);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Reselection to FDD UARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", uarfcn);
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*!
  @brief Adds a TDD UARFCN to a reselection frequency list if reselection to that
         UARFCN is permitted.

  The UARFCN is only added to the reselection frequency list if reselection to
  cells on the UARFCN is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  @param dst The list to add the UARFCN to.
  @param uarfcn The UARFCN to add to the list.
*/
static void rr_utran_tdd_reselection_freq_list_add(
  utran_tdd_reselection_freq_list_T * dst,
  uint16                              uarfcn,
  const gas_id_t                      gas_id
)
{
  RR_NULL_CHECK_FATAL(dst);

  if (rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
        uarfcn,
        rr_band_pref_utran_tdd(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    if (dst->num_of_frequencies < MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)
    {

      uint8 i = 0;
      boolean found = FALSE;

      for (i = 0; i < dst->num_of_frequencies; ++i)
      {
        if (dst->uarfcn[i] == uarfcn)
        {
          found = TRUE;
          break;
        }
      }
      if (found == FALSE)
      {
        MSG_GERAN_HIGH_1_G("Adding supported TDD UARFCN to L1 3G nlist: %d", uarfcn);
        dst->uarfcn[dst->num_of_frequencies] = uarfcn;
        ++dst->num_of_frequencies;
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Unable to add TDD UARFCN %d to reselection frequency list - overflow detected", uarfcn);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Reselection to TDD UARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", uarfcn);
  }
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/*!
  @brief Adds a cell to a WCDMA cell list if reselection to the UARFCN of the
         cell is permitted.

  The cell is only added to the reselection frequency list if reselection to
  the UARFCN of the cell is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  @param dst The list to add the cell to.
  @param cell The cell to add to the list.
*/
void rr_wcdma_cell_list_add(
  wcdma_cell_list_T * dst,
  rr_utran_cell_T     cell,
  const gas_id_t      gas_id
)
{
  RR_NULL_CHECK_FATAL(dst);

  if (rr_g2x_resel_to_wcdma_frequency_is_allowed(
        cell.uarfcn.uarfcn,
        rr_band_pref_wcdma(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    MSG_GERAN_HIGH_2_G("Adding supported UARFCN %d, SC %d to L1 3G nlist",
             cell.uarfcn.uarfcn, cell.cell_code);

    if (dst->num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
    {
      dst->cell_list[dst->num_of_cells].diversity = cell.diversity;
      dst->cell_list[dst->num_of_cells].scrambling_code = cell.cell_code;
      dst->cell_list[dst->num_of_cells].uarfcn = cell.uarfcn.uarfcn;
      ++dst->num_of_cells;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Unable to add UARFCN %d, SC %d to cell list - overflow detected",
                cell.uarfcn.uarfcn, cell.cell_code);
    }
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Reselection to UARFCN %d, SC %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)",
             cell.uarfcn.uarfcn, cell.cell_code);
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*!
  @brief Adds a cell to a UTRAN TDD cell list if reselection to the UARFCN of the
         cell is permitted.

  The cell is only added to the reselection frequency list if reselection to
  the UARFCN of the cell is currently permitted.  This will not be the case if
  the UARFCN is known to belong to a forbidden LAI, non-equivalent PLMN or
  is unsupported by the UE configuration.

  @param dst The list to add the cell to.
  @param cell The cell to add to the list.
*/
static void rr_utran_tdd_cell_list_add(
  utran_tdd_cell_list_T * dst,
  rr_utran_cell_T         cell,
  const gas_id_t          gas_id
)
{
  RR_NULL_CHECK_FATAL(dst);

  if (rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
        cell.uarfcn.uarfcn,
        rr_band_pref_utran_tdd(gas_id),
        FALSE,
        FALSE,
        gas_id
      ))
  {
    MSG_GERAN_HIGH_2_G("Adding supported TDD UARFCN %d, CPID %d to L1 3G nlist",
             cell.uarfcn.uarfcn, cell.cell_code);

    if (dst->num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
    {
      dst->cell_list[dst->num_of_cells].cell_parameter_id = cell.cell_code;
      dst->cell_list[dst->num_of_cells].diversity = cell.diversity;
      dst->cell_list[dst->num_of_cells].sync_case = cell.sync_case_tstd;
      dst->cell_list[dst->num_of_cells].uarfcn = cell.uarfcn.uarfcn;
      ++dst->num_of_cells;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Unable to add TDD UARFCN %d, CPID %d to cell list - overflow detected",
                cell.uarfcn.uarfcn, cell.cell_code);
    }
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Reselection to TDD UARFCN %d, CPID %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)",
             cell.uarfcn.uarfcn, cell.cell_code);
  }
}
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_LTE */


#ifdef FEATURE_LTE

/**
  @brief Returns TRUE/FALSE if an EARFCN is present in a cell list.
*/
boolean rr_l1_wcdma_cell_update_list_req_contains_earfcn(
  mph_wcdma_cell_update_list_req_T * cell_update_list_req,
  lte_earfcn_t earfcn
)
{
  uint8 i;

  RR_NULL_CHECK_RETURN_PARAM(cell_update_list_req, FALSE);

  for (i = 0;
       i < cell_update_list_req->eutran_frequency_count &&
       i < MAX_RR_L1_EUTRAN_FREQUENCIES;
       ++i)
  {
    if (cell_update_list_req->eutran_frequencies[i].earfcn == earfcn)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  @brief Returns TRUE/FALSE if an EARFCN is present in a cell list.
*/
boolean rr_l1_dedicated_wcdma_cell_update_list_req_contains_earfcn(
  mph_dedicated_wcdma_cell_update_list_req_T * cell_update_list_req,
  lte_earfcn_t earfcn
)
{
  uint8 i;

  RR_NULL_CHECK_RETURN_PARAM(cell_update_list_req, FALSE);

  for (i = 0;
       i < cell_update_list_req->eutran_frequency_count &&
       i < MAX_RR_L1_EUTRAN_FREQUENCIES;
       ++i)
  {
    if (cell_update_list_req->eutran_frequencies[i].earfcn == earfcn)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#endif /* FEATURE_LTE */

/**
  @brief Send MPH_WCDMA_CELL_UPDATE_LIST_REQ to GL1.
*/
void rr_send_mph_wcdma_cell_update_list_req(
  gprs_scell_info_t * scell,
  const gas_id_t      gas_id
)
{
  mph_wcdma_cell_update_list_req_T wcdma_cell_update_list_req;
  uint32 i;
#ifdef FEATURE_GSM_TDS
  boolean use_fdd = FALSE;
  boolean use_tdd = FALSE;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_SGLTE
  /*Add robustness code to protect against case where we are CS_ONLY
    in SGLTE mode and the RAT_PRI_LIST contains non GERAN RAT's*/
  if (rr_is_sglte_mode_active(gas_id))
  {
    rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

    if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
    {
      MSG_GERAN_HIGH_0_G("SGLTE service domain is CS_ONLY, delete old measurement");
#ifdef FEATURE_WCDMA
      if (rr_is_wcdma_enabled(gas_id))
      {
        rr_g2w_wcdma_measurements_delete(gas_id);
      }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
      if (rr_is_lte_enabled(gas_id))
      {
        rr_g2w_lte_measurements_delete(gas_id);
      }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        rr_g2w_utran_tdd_measurements_delete(gas_id);
      }
#endif /* FEATURE_GSM_TDS */
      MSG_GERAN_HIGH_0_G("SGLTE service domain is CS_ONLY, reset RAT_PRI_LIST to GSM only");
      rr_sglte_set_rat_priority_list_gsm_only(gas_id);
    }
  }
#endif /*FEATURE_SGLTE*/

  if (rr_g2x_measurements_allowed(gas_id) == FALSE)
  {
    return;
  }

  RR_PREPARE_L1_MESSAGE(wcdma_cell_update_list_req, MPH_WCDMA_CELL_UPDATE_LIST_REQ, gas_id);

  /* Set default values for various parameters. */
  wcdma_cell_update_list_req.FDD_Qmin = DEFAULT_FDD_QMIN;
  wcdma_cell_update_list_req.WCDMA_QSearch_I = DEFAULT_QSEARCH_I;

  /* 3G_SEARCH_PRIO is always indicated as TRUE.  This parameter is no longer
  broadcast by the network, see 3GPP TSG document GP-092154. */
  wcdma_cell_update_list_req.THREE_G_SEARCH_PRIO = TRUE;
#ifdef FEATURE_LTE
  wcdma_cell_update_list_req.qsearch_c_eutran = RR_DEFAULT_QSEARCH_C_EUTRAN;
#endif /* FEATURE_LTE */

  if (scell != NULL)
  {
#ifdef FEATURE_GSM_TDS
    rr_nw_supp_utran_rats_t nw_supp_rats = rr_nw_supported_utran_rats(scell);

    switch (nw_supp_rats)
    {
      case RR_NW_SUPP_UTRAN_NONE:
        use_fdd = FALSE;
        use_tdd = FALSE;
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
      break;

      case RR_NW_SUPP_UTRAN_FDD:
        if (rr_is_wcdma_enabled(gas_id))
        {
          use_fdd = TRUE;
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
        }
      break;

      case RR_NW_SUPP_UTRAN_TDD:
        if (rr_is_utran_tdd_enabled(gas_id))
        {
          use_tdd = TRUE;
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
        }
      break;

      case RR_NW_SUPP_UTRAN_TDD_FDD: /*not a known NW config, handled here for robustness*/
        if (rr_is_utran_tdd_enabled(gas_id) &&
            rr_is_wcdma_enabled(gas_id) )
        {
          if (rr_is_utran_tdd_enabled_in_nw_plmn(gas_id)) /*If in a TDS PLMN choose TDS*/
          {
             use_tdd = TRUE;
             MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
          }
          else
          {
             use_fdd = TRUE;
             MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
          }
        }
        else if (rr_is_wcdma_enabled(gas_id))
        {
           use_tdd = TRUE;
           MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
        }
        else if (rr_is_wcdma_enabled(gas_id))
        {
           use_fdd = TRUE;
           MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
        }
        else if (rr_is_utran_tdd_enabled(gas_id))
        {
           use_tdd = TRUE;
           MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
        }
        else
        {
          use_tdd = FALSE;
          use_fdd = FALSE;
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
        }
      break;
    }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
    {
      /* The Priority-based reselection algorithm should only be used if
      priority & threshold information has been provided to the UE. */
      boolean use_priority_reselection_algo = FALSE;
      boolean is_lte_supported = rr_is_lte_supported(gas_id);
      boolean serving_cell_priority_parameters_ie_received = rr_serving_cell_priority_parameters_ie_received(scell);

      MSG_GERAN_HIGH_2_G("is_lte_supported: %d, serving_cell_priority_parameters_ie_received: %d",
                         is_lte_supported,
                         serving_cell_priority_parameters_ie_received);

      if (is_lte_supported && serving_cell_priority_parameters_ie_received)
      {
        use_priority_reselection_algo = TRUE;
      }

      if (use_priority_reselection_algo)
      {
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
        if (!rr_utran_priority_parameters_ie_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id),gas_id) 
	    && !( rr_is_pseudo_si2q_data_used(gas_id) ))
#else
        if (!rr_utran_priority_parameters_ie_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id),gas_id))
#endif
          {
            /*For the case where priority based reselection is enabled and UTRAN
              priority parameters are not present delete the UTRAN measurements
              because we will not be sending any UTRAN cell info to L1*/
            MSG_GERAN_HIGH_0_G("RR delete UTRAN measurements, UTRAN priority missing");
#ifdef FEATURE_GSM_TDS
            rr_g2w_utran_tdd_measurements_delete(gas_id);
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_WCDMA
            rr_g2w_wcdma_measurements_delete(gas_id);
#endif /*FEATURE_WCDMA*/  
          }
      }

#ifdef FEATURE_GSM_TDS
      if ( use_fdd )
#else
      if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/
      {
#ifdef FEATURE_WCDMA
        if (scell->gsm.utran_neighbor_list.valid_data == TRUE)
        {
          if (scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI)
          {
            wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.utran_neighbor_list.si2q_params.QSearch_I;
          }
          else
          {
            /* Default value for Qsearch_I is 15, however RR will process SI2quater
            instances as they are received and, if camped, will update L1 with a
            partial 3G neighbour cell list each time an instance is received.  If
            we have received some 3G neighbour cells but have not yet received the
            3G Measurement Parameters then we will end up setting Qsearch_I to 15 -
            search never - this is not desirable as the whole point sending partial
            3G neighbour lists to L1 is to get it scheduling measurements as soon
            as possible, so we set the value to 7 (search always) until we actually
            receive the correct value from the network. */
            wcdma_cell_update_list_req.WCDMA_QSearch_I = 7; /* search always */
          }

          wcdma_cell_update_list_req.FDD_Qmin = scell->gsm.utran_neighbor_list.si2q_params.FDD_Qmin;

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
          #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */

          /* Add frequencies from SI2quater. */
          /* wcdma_freq_only_list has 9 entries to cater for TDSCDMA.  wcdma_cell_update_list_req.wcdma_reselection_freq_list
             only has 8 entries and will be truncate if there are 9 frequencies,
             WCDMA is not expected to have more than 8 frequencies though*/
          for (i = 0;
               i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
               i < MAX_UTRAN_FREQ_UPDATE_LIST;
               ++i)
          {
            if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_FDD)
            {
              rr_wcdma_reselection_freq_list_add(
                &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
                scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn,
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );
            }
          }

          /* Add cells from SI2quater. */
          for (i = 0;
               i < scell->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
               i < MAX_RR_MPH_CELL_UPDATE_LIST;
               ++i)
          {
            if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_FDD)
            {
              /* Every UARFCN in the IRAT neighbour cell list is also added to the
              frequency list, so L1 can easily figure out the UARFCNs it needs to
              search. */
              rr_wcdma_reselection_freq_list_add(
                &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
                scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.uarfcn,
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );

              rr_wcdma_cell_list_add(
                &wcdma_cell_update_list_req.wcdma_cell_list,
                scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i],
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );
            }
          }
        }

        if (scell->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
        {
          /* if SI2quater was sent always use 3G MP from SI2quater */
          if (scell->gsm.utran_neighbor_list.valid_data != TRUE ||
              scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI != TRUE)
          {
            wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.wcdma_neighbor_list_si2ter.Qsearch_I;

            wcdma_cell_update_list_req.FDD_Qmin = scell->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin;

  #ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
            #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */
          }

          /* Add frequencies from SI2ter. */
          for (i = 0;
               i < scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.num_of_frequencies &&
               i < MAX_RR_MPH_FREQ_UPDATE_LIST;
               ++i)
          {
            rr_wcdma_reselection_freq_list_add(
              &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
              scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.uarfcn[i],
              use_priority_reselection_algo,
              &wcdma_cell_update_list_req.frequency_priority_list,
              gas_id
            );
          }
        }
#endif /* FEATURE_WCDMA */
      }
      else
      {
        MSG_GERAN_HIGH_0_G("WCDMA is not enabled");
      }

#ifdef FEATURE_GSM_TDS
      if ( use_tdd )
      {
        if (scell->gsm.utran_neighbor_list.valid_data == TRUE)
        {
          if (scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI)
          {
            wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.utran_neighbor_list.si2q_params.QSearch_I;
          }
          else
          {
            /* Default value for Qsearch_I is 15, however RR will process SI2quater
            instances as they are received and, if camped, will update L1 with a
            partial 3G neighbour cell list each time an instance is received.  If
            we have received some 3G neighbour cells but have not yet received the
            3G Measurement Parameters then we will end up setting Qsearch_I to 15 -
            search never - this is not desirable as the whole point sending partial
            3G neighbour lists to L1 is to get it scheduling measurements as soon
            as possible, so we set the value to 7 (search always) until we actually
            receive the correct value from the network. */
            wcdma_cell_update_list_req.WCDMA_QSearch_I = 7; /* search always */
          }

          wcdma_cell_update_list_req.utran_tdd_bandwidth = scell->gsm.utran_neighbor_list.common_params.utran_tdd_bandwidth;

          /* Add frequencies from SI2quater. */
          for (i = 0;
               i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
               i < MAX_UTRAN_FREQ_UPDATE_LIST;
               ++i)
          {
            if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_TDD)
            {
              rr_utran_tdd_reselection_freq_list_add(
                &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
                scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn,
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );
            }
          }

          /* Add cells from SI2quater. */
          for (i = 0;
               i < scell->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
               i < MAX_RR_MPH_CELL_UPDATE_LIST;
               ++i)
          {
            if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_TDD)
            {
              rr_utran_tdd_reselection_freq_list_add(
                &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
                scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.uarfcn,
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );

              rr_tds_cell_list_add(
                &wcdma_cell_update_list_req.utran_tdd_cell_list,
                scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i],
                use_priority_reselection_algo,
                &wcdma_cell_update_list_req.frequency_priority_list,
                gas_id
              );
            }
          }
        }

        if (scell->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
        {
          /* if SI2quater was sent always use 3G MP from SI2quater */
          if (scell->gsm.utran_neighbor_list.valid_data != TRUE ||
              scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI != TRUE)
          {
            wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.wcdma_neighbor_list_si2ter.Qsearch_I;

            wcdma_cell_update_list_req.FDD_Qmin = scell->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin;

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
            #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */
          }

          if (scell->gsm.wcdma_neighbor_list_si2ter.TDD_BANDWIDTH_valid)
          {
            wcdma_cell_update_list_req.utran_tdd_bandwidth = scell->gsm.wcdma_neighbor_list_si2ter.TDD_BANDWIDTH;
          }

          /* Add frequencies from SI2ter. */
          for (i = 0;
               i < scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.num_of_frequencies &&
               i < MAX_RR_MPH_FREQ_UPDATE_LIST;
               ++i)
          {
            rr_utran_tdd_reselection_freq_list_add(
              &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
              scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.uarfcn[i],
              use_priority_reselection_algo,
              &wcdma_cell_update_list_req.frequency_priority_list,
              gas_id
            );
          }
        }
      }
#endif /*FEATURE_GSM_TDS*/

      wcdma_cell_update_list_req.use_priority_reselection_algo = use_priority_reselection_algo;

      /* use_priority_reselection_algo will be TRUE only if priority
      information is available to the UE. */
      if (use_priority_reselection_algo)
      {
        gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

        wcdma_cell_update_list_req.THRESH_priority_search = scell->serving_cell_priority_params.thresh_priority_search;
        wcdma_cell_update_list_req.geran_priority = (uint8) rr_geran_priority(gas_id);
        wcdma_cell_update_list_req.qsearch_c_eutran = scell->eutran_params.meas_params.qsearch_c;

        if (rr_is_lte_enabled(gas_id) && rr_g2l_resel_allowed(gas_id))
        {
          if (rr_g2l_measurements_enabled_in_nv(gas_id))
          {
            for (i = 0;
                 i < scell->lte_neighbour_list.count &&
                 i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE &&
                 wcdma_cell_update_list_req.eutran_frequency_count < MAX_RR_L1_EUTRAN_FREQUENCIES;
                 ++i)
            {
              rr_lte_neighbour_list_entry_t * src;

              src = &scell->lte_neighbour_list.entries[i];

              if (rr_l1_wcdma_cell_update_list_req_contains_earfcn(&wcdma_cell_update_list_req, src->frequency.earfcn))
              {
                MSG_GERAN_MED_1_G("Filtering duplicate EARFCN %d", src->frequency.earfcn);
              }
              else
              {
                rr_l1_eutran_frequency_t * dst;
                dst = &wcdma_cell_update_list_req.eutran_frequencies[wcdma_cell_update_list_req.eutran_frequency_count];

                if (rr_g2x_resel_to_lte_frequency_is_allowed(
                      src->frequency.earfcn,
                      rr_band_pref_lte(gas_id),
                      FALSE,
                      FALSE,
                      gas_id)
                )
                {
                  /* Priority-based reselection is only possible if a priority level is
                  specified for the IRAT frequency. */
                  rr_resel_priority_e eutran_priority = rr_get_resel_priority_for_earfcn(src->frequency.earfcn, gas_id);

                  MSG_GERAN_HIGH_2_G("LTE frequency: %d, Priority: %d", src->frequency.earfcn, eutran_priority);

                  if (eutran_priority != RR_RESEL_PRIORITY_UNDEFINED)
                  {
                    uint32 pcid;

                    dst->earfcn = src->frequency.earfcn;
                    dst->measurement_bandwidth = src->frequency.measurement_bandwidth;
                    if (lte_rrc_cell_is_tdd(src->frequency.earfcn))
                    {
                      dst->type = RR_L1_EUTRAN_TYPE_TDD;
                    }
                    else
                    {
                      dst->type = RR_L1_EUTRAN_TYPE_FDD;
                    }

                    for (pcid = RR_L1_PCID_MIN; pcid <= RR_L1_PCID_MAX; ++pcid)
                    {
                      if (RR_PCID_GROUP_IE_GET(src->not_allowed_cells.bitmap, pcid))
                      {
                        MSG_GERAN_MED_1_G("PCID %d Not Allowed", pcid);
                        RR_L1_PCID_GROUP_BITMAP_SET(dst->not_allowed_cells, pcid);
                      }
                    }

                    ++wcdma_cell_update_list_req.eutran_frequency_count;

                    rr_irat_frequency_priority_list_add(
                      &wcdma_cell_update_list_req.frequency_priority_list,
                      RR_L1_IRAT_RAT_LTE,
                      src->frequency.earfcn,
                      (uint8) eutran_priority,
                      gas_id
                    );
                  }
                  else
                  {
                    MSG_GERAN_HIGH_1_G("No priority specified for EARFCN %d, omitting from neighbour list", src->frequency.earfcn);
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_1_G("Reselection to EARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", src->frequency.earfcn);
                }
              }
            }
          }
          else
          {
             MSG_GERAN_HIGH_0_G("G2L MEASUREMENTS DISABLED IN NV");
          }
        }
        else
        {
          MSG_GERAN_HIGH_0_G("Reselection to LTE is disallowed by NAS");
        }
      }
      else
      {
        wcdma_cell_update_list_req.qsearch_c_eutran = scell->eutran_params.meas_params.qsearch_c;
      }
    }
#else /* FEATURE_LTE is not defined */
    if (scell->gsm.utran_neighbor_list.valid_data == TRUE)
    {
      if (scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI)
      {
        wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.utran_neighbor_list.si2q_params.QSearch_I;
      }
      else
      {
        /* Default value for Qsearch_I is 15, however RR will process SI2quater
        instances as they are received and, if camped, will update L1 with a
        partial 3G neighbour cell list each time an instance is received.  If
        we have received some 3G neighbour cells but have not yet received the
        3G Measurement Parameters then we will end up setting Qsearch_I to 15 -
        search never - this is not desirable as the whole point sending partial
        3G neighbour lists to L1 is to get it scheduling measurements as soon
        as possible, so we set the value to 7 (search always) until we actually
        receive the correct value from the network. */
        wcdma_cell_update_list_req.WCDMA_QSearch_I = 7; /* search always */
      }

      wcdma_cell_update_list_req.FDD_Qmin = scell->gsm.utran_neighbor_list.si2q_params.FDD_Qmin;

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
      #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */


#ifdef FEATURE_GSM_TDS
      if ( use_tdd )
      {
        wcdma_cell_update_list_req.utran_tdd_bandwidth = scell->gsm.utran_neighbor_list.common_params.utran_tdd_bandwidth;

        /* Add UTRAN TDD frequencies from SI2quater. */
        for (i = 0;
             i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
             i < MAX_UTRAN_FREQ_UPDATE_LIST;
             ++i)
        {
          /* if both W and T are supported by NW then a decision can be put here to inhibit TD measurements*/
          if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_TDD)
          {
            rr_utran_tdd_reselection_freq_list_add(
              &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
              scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn,
              gas_id
            );
          }
        }

        /* Add cells from SI2quater. */
        for (i = 0;
             i < scell->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
             i < MAX_RR_MPH_CELL_UPDATE_LIST;
             ++i)
        {
          if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_TDD)
          {
            rr_utran_tdd_reselection_freq_list_add(
                &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
                scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.uarfcn,
                gas_id
              );

            rr_utran_tdd_cell_list_add(
              &wcdma_cell_update_list_req.utran_tdd_cell_list,
              scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i],
              gas_id
            );
          }
        }
      }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS
      if (use_fdd)
#else
      if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/
      {
#ifdef FEATURE_WCDMA
        /* Add FDD UTRAN frequencies from SI2quater. */
          /* wcdma_freq_only_list has 9 entries to cater for TDSCDMA.  wcdma_cell_update_list_req.wcdma_reselection_freq_list
               only has 8 entries and will be truncate if there are 9 frequencies,
               WCDMA is not expected to have more than 8 frequencies though*/
        for (i = 0;
             i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
               i < MAX_UTRAN_FREQ_UPDATE_LIST;
             ++i)
        {
          if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_FDD)
          {
            rr_wcdma_reselection_freq_list_add(
              &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
              scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn,
              gas_id
            );
          }
        }

        /* Add cells from SI2quater. */
        for (i = 0;
             i < scell->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
             i < MAX_RR_MPH_CELL_UPDATE_LIST;
             ++i)
        {
          if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_FDD)
          {
            rr_wcdma_reselection_freq_list_add(
              &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
              scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.uarfcn,
              gas_id
            );

            rr_wcdma_cell_list_add(
              &wcdma_cell_update_list_req.wcdma_cell_list,
              scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i],
              gas_id
            );
          }
        }
#endif /* FEATURE_WCDMA */
      } /*use_fdd*/
    }

    if (scell->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
    {
      /* if SI2quater was sent always use 3G MP from SI2quater */
      if (scell->gsm.utran_neighbor_list.valid_data != TRUE ||
          scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI != TRUE)
      {
        wcdma_cell_update_list_req.WCDMA_QSearch_I = scell->gsm.wcdma_neighbor_list_si2ter.Qsearch_I;

        wcdma_cell_update_list_req.FDD_Qmin = scell->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin;

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
        #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */
      }

#ifdef FEATURE_GSM_TDS
      if (use_tdd)
      {
        if (scell->gsm.wcdma_neighbor_list_si2ter.TDD_BANDWIDTH_valid)
        {
          wcdma_cell_update_list_req.utran_tdd_bandwidth = scell->gsm.wcdma_neighbor_list_si2ter.TDD_BANDWIDTH;
        }

        /* Add TDD frequencies from SI2ter. */
        for (i = 0;
             i < scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.num_of_frequencies &&
             i < MAX_RR_MPH_FREQ_UPDATE_LIST; /*There can only be 8 instances of SI2ter so the max num of UARFCN's is 8.*/
             ++i)                             /*Note that TDS max UARFCN's is 9, only SI2Quater can use all 9*/
        {
          rr_utran_tdd_reselection_freq_list_add(
            &wcdma_cell_update_list_req.utran_tdd_reselection_freq_list,
            scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.uarfcn[i],
            gas_id
          );
        }
      }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS
      if (use_fdd)
#else
      if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/
      {
#ifdef FEATURE_WCDMA
        /* Add FDD frequencies from SI2ter. */
        for (i = 0;
             i < scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.num_of_frequencies &&
             i < MAX_RR_MPH_FREQ_UPDATE_LIST;
             ++i)
        {
          rr_wcdma_reselection_freq_list_add(
            &wcdma_cell_update_list_req.wcdma_reselection_freq_list,
            scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.uarfcn[i],
            gas_id
          );
        }
#endif /* FEATURE_WCDMA */
      }/*use_fdd*/
    }
#endif /* FEATURE_LTE */
  }

#ifdef FEATURE_WCDMA
  /* 3G Idle mode log packet */
  (void) rr_log_rr_3g_measurement_params(
    FALSE,    // in_dedicated
    gas_id    // gas_id
  );
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_WCDMA
  if ( wcdma_cell_update_list_req.wcdma_reselection_freq_list.num_of_frequencies > 0 )
  {
    rr_detect_and_remove_overlap_uarfcn_in_idle( &wcdma_cell_update_list_req, gas_id);
  }

#endif

#if defined(FEATURE_SGLTE) && defined(FEATURE_GSM_TDS)
  if ( wcdma_cell_update_list_req.utran_tdd_reselection_freq_list.num_of_frequencies > 0 )
  {
     rr_sglte_set_l1_tds_measurements_configured(TRUE, gas_id);
  }
  else
  {
    rr_sglte_set_l1_tds_measurements_configured(FALSE, gas_id);  
  }
#endif /*FEATURE_SGLTE && FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
  if ( wcdma_cell_update_list_req.eutran_frequency_count > 0 )
  {
    rr_set_lte_l1_measurements_configured(TRUE, gas_id);
    rr_detect_and_remove_overlap_earfcn(&wcdma_cell_update_list_req, NULL, gas_id);
  }
  else
  {
    rr_set_lte_l1_measurements_configured(FALSE, gas_id);
  }
#endif


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_set_num_uarfcns_sent_to_l1(rr_count_num_unique_uarfcns(&wcdma_cell_update_list_req), gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  rr_l1_send_message_to_l1((IMH_T *)&wcdma_cell_update_list_req, gas_id);
}

/**
  @brief Send MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ to GL1.
*/
void rr_send_mph_specific_irat_freq_update_req(
  uint32 arfcn,
  rr_rat_e rat,
  uint32 resel_timer,
  specific_update_freq_T *freq_list,
  byte freq_list_length,
  const gas_id_t gas_id
)
{
  mph_specific_irat_freq_update_req_T mph_specific_irat_freq_update_req;
  boolean valid_rat = FALSE, update_req_already_sent = FALSE;
  byte i;

  RR_PREPARE_L1_MESSAGE(mph_specific_irat_freq_update_req, MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ, gas_id);

  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
    case RR_RAT_LTE:
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
#endif /*FEATURE_GSM_TDS*/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      valid_rat = TRUE;
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
      
    default:
      MSG_GERAN_ERROR_1_G("RR_FG2X: Unexpected RAT: %d", rat);
      break;
  }

  // Check if MPH_SPECIFIC_IRAT_UPDATE_REQ is already sent for this ARFCN, based on the current set of measurements. 
  for(i=0; i<freq_list_length; i++)
  {
    if(freq_list->arfcn != 0xFFFFFFFF)
    {
      if((freq_list->arfcn == arfcn) && ((rr_rat_e)(freq_list->rat) == rat))
      {
        // MPH_SPECIFIC_IRAT_UPDATE_REQ already sent for the frequency "arfcn".
        update_req_already_sent = TRUE;
        break;
      }
      else
      {
        freq_list++;
      }
    }
    else
    {
      // Reached end of the list. MPH_SPECIFIC_IRAT_UPDATE_REQ not yet sent for the frequency "arfcn".
      freq_list->arfcn = arfcn;
      freq_list->rat = (uint8)rat;
      break;
    }
  }

  mph_specific_irat_freq_update_req.arfcn = arfcn;
  mph_specific_irat_freq_update_req.rat = (rr_l1_irat_rat_e)rat;
  mph_specific_irat_freq_update_req.resel_timer = resel_timer/1000;

  if(valid_rat && !update_req_already_sent)
  {
    MSG_GERAN_MED_2_G("RR_FG2X: Sending MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ: rat=%d,arfcn=%d",rat,arfcn);
    rr_l1_send_message_to_l1((IMH_T *)&mph_specific_irat_freq_update_req, gas_id);
  }
}

/**
  @brief Send MPH_IGNORE_WCDMA_NEIGHBORS_REQ to GL1.
*/
void rr_send_mph_ignore_wcdma_neighbors_req (
  word uarfcn,
  word scrambling_code,
  boolean ignore_flag,
  boolean threshold_valid,
  uint16 s_qualmin,
  uint16 s_rxlevmin,
  const gas_id_t gas_id
)
{
  mph_ignore_wcdma_neighbors_req_T mph_ignore_wcdma_neighbors_req;

  RR_PREPARE_L1_MESSAGE(mph_ignore_wcdma_neighbors_req, MPH_IGNORE_WCDMA_NEIGHBORS_REQ, gas_id);

  mph_ignore_wcdma_neighbors_req.uarfcn = uarfcn;
  mph_ignore_wcdma_neighbors_req.scrambling_code = scrambling_code;
  mph_ignore_wcdma_neighbors_req.ignore_flag = ignore_flag;
  mph_ignore_wcdma_neighbors_req.threshold_valid = threshold_valid;
  mph_ignore_wcdma_neighbors_req.s_qualmin = s_qualmin;
  mph_ignore_wcdma_neighbors_req.s_rxlevmin = s_rxlevmin;

  rr_l1_send_message_to_l1((IMH_T *)&mph_ignore_wcdma_neighbors_req, gas_id);
}


/**
  @brief Send MPH_UE_MODE_CHANGE_REQ to GL1.
*/
void rr_send_mph_ue_mode_change_req(
  boolean gsm_only,
  const gas_id_t gas_id
)
{
  mph_ue_mode_change_req_T mph_ue_mode_change_req;

  RR_PREPARE_L1_MESSAGE(mph_ue_mode_change_req, MPH_UE_MODE_CHANGE_REQ, gas_id);

  mph_ue_mode_change_req.gsm_only = gsm_only;

#ifdef FEATURE_SGLTE
  mph_ue_mode_change_req.sglte_mode_active = rr_is_sglte_mode_active(gas_id);
#ifdef FEATURE_GSM_TDS
 if ( gsm_only )
 {
   rr_sglte_set_l1_tds_measurements_configured(FALSE, gas_id);
 }
#endif /* FEATURE_GSM_TDS */

#else
  mph_ue_mode_change_req.sglte_mode_active = FALSE;
#endif /*FEATURE_SGLTE*/

  rr_l1_send_message_to_l1((IMH_T *)&mph_ue_mode_change_req, gas_id);
}

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
/*! FUNCTION NAME: rr_sglte_g2x_conn_prep_and_send_mph_ded_cellupd_req
  @brief  This function is specific to SGLTE Connected Mode Cell Reselection Feature

  This function is invoked when GRR is in Connected Mode, with the above mentioned feature active in SGLTE.
  It takes following consideration:
  (a) If PRIORITY Based Information is present and LTE is supported, then it fills the Priority and Fequency based information
      that needs to be sent to GL1.
  (b) If No Priority based Information is present then it ONLY populates the TDS Part using SI2 Information. Please note that this
      feature is SGLE specific hence no WCDMA (i.e. UTRA FDD) information is populated for GL1.

  @param mph_dedicated_wcdma_cell_update_list_req_T This is the cintainer pointer to be populated
  @param gas_id The GPRS Access Stratum ID on which this operation is performed.
*/

static void rr_sglte_g2x_conn_prep_and_send_mph_ded_cellupd_req(mph_dedicated_wcdma_cell_update_list_req_T* mph_dedicated_wcdma_cell_update_list_req_ptr, const gas_id_t gas_id)
{

 /* The Priority-based reselection algorithm should only be used if
 priority & threshold information has been provided to the UE. */

 boolean use_priority_reselection_algo = FALSE;
 gprs_scell_info_t                          *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
 boolean is_lte_supported = rr_is_lte_supported(gas_id);
 boolean serving_cell_priority_parameters_ie_received = rr_serving_cell_priority_parameters_ie_received(scell_info_ptr);
 byte                                        i = 0;
 boolean                                     is_tds_to_be_measured = TRUE;

 MSG_GERAN_HIGH_2_G("is_lte_supported: %d, serving_cell_priority_parameters_ie_received: %d",
                    is_lte_supported,
                    serving_cell_priority_parameters_ie_received);

 if (is_lte_supported && serving_cell_priority_parameters_ie_received)
 {
   use_priority_reselection_algo = TRUE;
 }


 /* Top level check to determine if the TDS is mto be measured or not ?? */
 if (rr_is_utran_tdd_enabled(gas_id) && rr_is_utran_tdd_enabled_in_nw_plmn(gas_id) )
 {
   is_tds_to_be_measured = TRUE;
 }

 if ( use_priority_reselection_algo )
 {
     /*   First LTE frequencies */
   if (rr_g2l_measurements_enabled_in_nv(gas_id))
   {
     for (i = 0;
       i < scell_info_ptr->lte_neighbour_list.count &&
       i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE &&
       mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list.count < MAX_RR_L1_PRIORITY_ENTRIES;
       ++i)
     {
       rr_lte_neighbour_list_entry_t * src;

       src = &scell_info_ptr->lte_neighbour_list.entries[i];
       /* Duplicate Detection not added yet */
       if (rr_g2x_resel_to_lte_frequency_is_allowed(
           src->frequency.earfcn,
           rr_band_pref_lte(gas_id),
           FALSE,
           FALSE,
           gas_id)
           )
       {
         /* Priority-based reselection is only possible if a priority level is
         specified for the IRAT frequency. */
         rr_resel_priority_e eutran_priority = rr_get_resel_priority_for_earfcn(src->frequency.earfcn, gas_id);

         MSG_GERAN_HIGH_2_G("LTE frequency: %d, Priority: %d", src->frequency.earfcn, eutran_priority);

        if (eutran_priority != RR_RESEL_PRIORITY_UNDEFINED)
        {
         /* LTE frequency is valid, add it in two places as required by GL! */
          rr_l1_eutran_frequency_t * dst;

          dst = &mph_dedicated_wcdma_cell_update_list_req_ptr->eutran_frequencies[mph_dedicated_wcdma_cell_update_list_req_ptr->eutran_frequency_count];

          MSG_GERAN_HIGH_1_G("LTE frequency: %d", src->frequency.earfcn);

          rr_irat_frequency_priority_list_add(
           &mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list,
           RR_L1_IRAT_RAT_LTE,
           src->frequency.earfcn,
           (uint8) eutran_priority,
            gas_id
            );

          dst->measurement_bandwidth = src->frequency.measurement_bandwidth;
          dst->earfcn = src->frequency.earfcn;
          if (lte_rrc_cell_is_tdd(src->frequency.earfcn))
          {
            dst->type = RR_L1_EUTRAN_TYPE_TDD;
          }
          else
          {
            dst->type = RR_L1_EUTRAN_TYPE_FDD;
          }

          mph_dedicated_wcdma_cell_update_list_req_ptr->eutran_frequency_count++;
        }
        else
        {
          MSG_GERAN_HIGH_1_G("No priority specified for EARFCN %d, omitting from neighbour list", src->frequency.earfcn);
        }
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Reselection to EARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", src->frequency.earfcn);
      }
     }
   } /* End of if(rr_g2l_measurements_enabled_in_nv(gas_id)) */
   else
   {
     MSG_GERAN_HIGH_0_G("G2L MEASUREMENTS DISABLED IN NV");
   }

   if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
   {

     if (rr_utran_priority_parameters_ie_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id),gas_id))
     {
       is_tds_to_be_measured = TRUE;
     }
     else
     {
            /*For the case where priority based reselection is enabled and UTRAN
              priority parameters are not present delete the UTRAN measurements
              because we will not be sending any UTRAN cell info to L1*/
            MSG_GERAN_HIGH_0_G("RR delete UTRAN measurements, UTRAN priority missing");
            is_tds_to_be_measured = FALSE;
            rr_g2w_utran_tdd_measurements_delete(gas_id);
     }
   }
 } /* use_priority_reselection_algo */
 else
 {
   MSG_GERAN_HIGH_1_G("No LTE Meas:to be done as Priority parameters are absent: use_priority_reselection_algo %d", use_priority_reselection_algo);
 }

if (is_tds_to_be_measured)
{

   mph_dedicated_wcdma_cell_update_list_req_ptr->wcdma_QSearch_C = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.QSearch_I;

 /* Add frequencies from SI2quater. */
 for (i = 0;
      i < scell_info_ptr->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
      i < MAX_UTRAN_FREQ_UPDATE_LIST &&
      mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list.count < MAX_RR_L1_PRIORITY_ENTRIES;
      ++i)
 {
   if (scell_info_ptr->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_TDD)
   {
     rr_utran_tdd_reselection_freq_list_add(
       &mph_dedicated_wcdma_cell_update_list_req_ptr->utran_tdd_freq_list,
       scell_info_ptr->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn,
       use_priority_reselection_algo,
       &mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list,
       gas_id
     );
   }
 }

 /* Add cells from SI2quater. */
 for (i = 0;
    i < scell_info_ptr->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
    i < MAX_RR_MPH_CELL_UPDATE_LIST;
    ++i)
 {
   if (scell_info_ptr->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_TDD)
   {

     rr_tds_cell_list_add(
     &mph_dedicated_wcdma_cell_update_list_req_ptr->utran_tdd_cell_list,
     scell_info_ptr->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i],
     use_priority_reselection_algo,
   &mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list,
     gas_id
   );
   }
  }

}
else
{
  MSG_GERAN_HIGH_0_G("No TDS measurements: to be configured to GL1");

  mph_dedicated_wcdma_cell_update_list_req_ptr->wcdma_QSearch_C = DEFAULT_QSEARCH_I;
}

 mph_dedicated_wcdma_cell_update_list_req_ptr->SEARCH_PRIO_3G = TRUE;

 /* Now that we have populated the Frequency List, time to set the rest of the parameters correctly */
 if ( (mph_dedicated_wcdma_cell_update_list_req_ptr->frequency_priority_list.count > 0) && (use_priority_reselection_algo == TRUE) )
 { /* GL1 is going to measure the Priority based algorithm in Connected Mode */
    mph_dedicated_wcdma_cell_update_list_req_ptr->use_priority_reselection_algo = TRUE;
    mph_dedicated_wcdma_cell_update_list_req_ptr->geran_priority = (uint8) rr_geran_priority(gas_id);
    mph_dedicated_wcdma_cell_update_list_req_ptr->THRESH_priority_search = scell_info_ptr->serving_cell_priority_params.thresh_priority_search;
 }

 /* 3G Ded mode log packet */
 (void) rr_log_rr_3g_measurement_params(
    TRUE,      // in_dedicated
    gas_id     // gas_id
  );

 rr_set_num_uarfcns_sent_to_l1_in_dedicated(
   rr_count_num_unique_uarfcns_in_dedicated(mph_dedicated_wcdma_cell_update_list_req_ptr),
   gas_id
 );

 rr_l1_send_message_to_l1((IMH_T *)mph_dedicated_wcdma_cell_update_list_req_ptr, gas_id);

}
#endif
#endif

/**
  @brief Send MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ to GL1.
*/
void rr_send_mph_dedicated_wcdma_cell_update_list_req(
  utran_idle_neighbor_list_T *wcdma_idle_neighbor_list_ptr,
  wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list_ptr,
  rr_dedicated_mode_params_t * dedicated_mode_params,
  const gas_id_t gas_id
)
{
  boolean                                     source = SOURCE_UNKNOWN;
  rr_utran_cell_list_T                       *cell_list = NULL;
  gprs_scell_info_t                          *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  mph_dedicated_wcdma_cell_update_list_req_T  mph_ded_wcdma_cell_update_list_req;
  byte                                        i, num_of_cells = 0;
  byte                                        num_of_valid_cells = 0;
#ifdef FEATURE_GSM_TDS
  boolean                                     include_tdd = FALSE;
  boolean                                     include_fdd = FALSE;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_SGLTE
  /*Add robustness code to protect against case where we are CS_ONLY
    in SGLTE mode and the RAT_PRI_LIST contains non GERAN RAT's*/
  if (rr_is_sglte_mode_active(gas_id))
  {
    rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

    if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
    {

      MSG_GERAN_HIGH_0_G("SGLTE service domain is CS_ONLY, delete old measurement");
#ifdef FEATURE_WCDMA
      if (rr_is_wcdma_enabled(gas_id))
      {
        rr_g2w_wcdma_measurements_delete(gas_id);
      }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
      if (rr_is_lte_enabled(gas_id))
      {
        rr_g2w_lte_measurements_delete(gas_id);
      }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        rr_g2w_utran_tdd_measurements_delete(gas_id);
      }
#endif /* FEATURE_GSM_TDS */
      MSG_GERAN_HIGH_0_G("SGLTE service domain is CS_ONLY, reset RAT_PRI_LIST to GSM only");
      rr_sglte_set_rat_priority_list_gsm_only(gas_id);
    }
  }
#endif /*FEATURE_SGLTE*/

  if (rr_g2x_measurements_allowed(gas_id) == FALSE)
  {
    return;
  }

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  RR_NULL_CHECK_RETURN_VOID(dedicated_mode_params);


  RR_PREPARE_L1_MESSAGE(mph_ded_wcdma_cell_update_list_req, MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ, gas_id);

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
  if (rr_is_sglte_mode_active(gas_id)  && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
  {
    rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

    if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_PS)
    {
      MSG_GERAN_HIGH_0_G("SGLTE G2X CONNECTED MODE Active and service is CS and PS");
      rr_sglte_g2x_conn_prep_and_send_mph_ded_cellupd_req(&mph_ded_wcdma_cell_update_list_req, gas_id);
      return;
    }
  }

#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /*FEATURE_SGLTE*/


  /* 3G_SEARCH_PRIO is always indicated as TRUE.  This parameter is no longer
  broadcast by the network, see 3GPP TSG document GP-092154. */
  mph_ded_wcdma_cell_update_list_req.SEARCH_PRIO_3G = TRUE;
#ifdef FEATURE_LTE
  mph_ded_wcdma_cell_update_list_req.qsearch_c_eutran = RR_DEFAULT_QSEARCH_C_EUTRAN;
#endif /* FEATURE_LTE */

  if ((wcdma_idle_neighbor_list_ptr != NULL) && (wcdma_ded_neighbor_list_ptr == NULL))
  {
    source = SOURCE_SI2_QUATER;
  }
  else if ((wcdma_idle_neighbor_list_ptr == NULL) && (wcdma_ded_neighbor_list_ptr != NULL))
  {
    source = SOURCE_MEAS_INFO;
  }

  if (source == SOURCE_SI2_QUATER && wcdma_idle_neighbor_list_ptr != NULL)
  {
    cell_list = &wcdma_idle_neighbor_list_ptr->wcdma_cell_list;
  }
  else if (source == SOURCE_MEAS_INFO && wcdma_ded_neighbor_list_ptr != NULL)
  {
    cell_list = &wcdma_ded_neighbor_list_ptr->wcdma_cell_list;
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  mph_ded_wcdma_cell_update_list_req.wcdma_QSearch_C = dedicated_mode_params->utran_params.meas_params.qsearch_c;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  if (cell_list == NULL)
  {
    MSG_GERAN_HIGH_0_G("Sending empty 3G neighbour list to L1");
    num_of_valid_cells = 0;
  }
  else
  {
    num_of_cells = cell_list->num_of_cells;

#ifdef FEATURE_GSM_TDS
    {
      boolean found_tdd = FALSE;
      boolean found_fdd = FALSE;

      for (i = 0; i < num_of_cells; ++i)
      {
        if (cell_list->cell_list[i].uarfcn.rat_type ==  RR_UTRAN_TDD)
        {
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
          if (source == SOURCE_MEAS_INFO)
          {
            rr_dedicated_data_T *dedic_ptr;

            dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

            if (dedic_ptr != NULL)
            {
              /*MI rxd with TDS cells.  This cancels Pseudo FR and reverts to
                normal G->T redirection as command by the NW*/
              if (dedic_ptr->tds_pseudo_fr.active == TRUE)
              {
                MSG_GERAN_HIGH_0_G("RR TDS stop doing PFR, NW supplied TDS cell list");
                rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
              }
            }
          }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

          found_tdd = TRUE;
        }

        if (cell_list->cell_list[i].uarfcn.rat_type ==  RR_UTRAN_FDD)
        {
          found_fdd = TRUE;
        }
      }

      if (found_tdd && found_fdd) /*not a known NW config, handled for robustness*/
      {
        if (rr_is_utran_tdd_enabled(gas_id) &&
            rr_is_wcdma_enabled(gas_id) )
        {
          if (rr_is_utran_tdd_enabled_in_nw_plmn(gas_id)) /*use the PLMN to decide*/
          {
            MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
            include_tdd = TRUE;
          }
          else
          {
            MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
            include_fdd = TRUE;
          }
        }
        else if (rr_is_wcdma_enabled(gas_id))
        {
          MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
          include_fdd = TRUE;
        }
        else if (rr_is_utran_tdd_enabled(gas_id))
        {
          MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
          include_tdd = TRUE;
        }
        else if (rr_is_wcdma_enabled(gas_id))
        {
          MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
          include_fdd = TRUE;
        }
        else
        {
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
        }
      }
      else if (found_fdd)
      {
        if (rr_is_wcdma_enabled(gas_id))
        {
          include_fdd = TRUE;
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
        }
      }
      else if (found_tdd)
      {
        if (rr_is_utran_tdd_enabled(gas_id))
        {
          include_tdd = TRUE;
          MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
      }
    }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS
    if (rr_dedicated_mode_is_wcdma_enabled(gas_id) == FALSE)
    {
      if (include_fdd)
      {
        include_fdd = FALSE;
      }
    }

    if (rr_dedicated_mode_is_tdscdma_enabled(gas_id) == FALSE)
    {
      if (include_tdd)
      {
        include_tdd = FALSE;
      }
    }
#endif /* FEATURE_GSM_TDS */

    for (i = 0; i < num_of_cells; ++i)
    {
#ifdef FEATURE_GSM_TDS
      if (cell_list->cell_list[i].uarfcn.rat_type ==  RR_UTRAN_TDD)
      {
        if (include_tdd)
        {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
          if (tdsrrc_is_arfcn_valid_sub_id(cell_list->cell_list[i].uarfcn.uarfcn, RR_GAS_ID_TO_AS_ID))
#else
          if (tdsrrc_is_arfcn_valid(cell_list->cell_list[i].uarfcn.uarfcn))
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
          {
            mph_ded_wcdma_cell_update_list_req.utran_tdd_cell_list.cell_list[num_of_valid_cells].uarfcn = cell_list->cell_list[i].uarfcn.uarfcn;
            mph_ded_wcdma_cell_update_list_req.utran_tdd_cell_list.cell_list[num_of_valid_cells].cell_parameter_id = cell_list->cell_list[i].cell_code;
            mph_ded_wcdma_cell_update_list_req.utran_tdd_cell_list.cell_list[num_of_valid_cells].diversity = cell_list->cell_list[i].diversity;
            mph_ded_wcdma_cell_update_list_req.utran_tdd_cell_list.cell_list[num_of_valid_cells].sync_case = cell_list->cell_list[i].sync_case_tstd;
            num_of_valid_cells++;
          }
          else
          {
            MSG_GERAN_HIGH_3_G("Ignoring cell on unsupported UARFCN: %d, %d",
              cell_list->cell_list[i].uarfcn.uarfcn,
              cell_list->cell_list[i].cell_code,
              0
            );
          }
        }
      }
      else
#endif /*FEATURE_GSM_TDS*/
      {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_GSM_TDS
        if (include_fdd)
#else
        if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/
        {
          if (rr_g2x_is_uarfcn_valid(
                cell_list->cell_list[i].uarfcn.uarfcn,
                scell_info_ptr->gsm.location_area_identification.plmn_id,
                gas_id
              ))
          {
              mph_ded_wcdma_cell_update_list_req.wcdma_cell_list.cell_list[num_of_valid_cells].uarfcn = cell_list->cell_list[i].uarfcn.uarfcn;
              mph_ded_wcdma_cell_update_list_req.wcdma_cell_list.cell_list[num_of_valid_cells].scrambling_code = cell_list->cell_list[i].cell_code;
              mph_ded_wcdma_cell_update_list_req.wcdma_cell_list.cell_list[num_of_valid_cells].diversity = cell_list->cell_list[i].diversity;
            num_of_valid_cells++;
          }
          else
          {
            MSG_GERAN_HIGH_3_G("Ignoring cell on unsupported UARFCN: %d, %d",
                cell_list->cell_list[i].uarfcn.uarfcn,
                cell_list->cell_list[i].cell_code,
              0
            );
          }
        }
#endif /* FEATURE_WCDMA */
      }
    }
  }

#ifdef FEATURE_GSM_TDS
  if (include_tdd)
  {
    mph_ded_wcdma_cell_update_list_req.utran_tdd_cell_list.num_of_cells = num_of_valid_cells;
  }
  else
#endif /*FEATURE_GSM_TDS*/
  {
    mph_ded_wcdma_cell_update_list_req.wcdma_cell_list.num_of_cells = num_of_valid_cells;
  }

#ifdef FEATURE_LTE
  if (dedicated_mode_params != NULL)
  {
    rr_dedicated_mode_eutran_params_t * eutran_params = &dedicated_mode_params->eutran_params;
    uint32 i;

    mph_ded_wcdma_cell_update_list_req.qsearch_c_eutran = eutran_params->meas_params.qsearch_c;

    if (rr_dedicated_mode_is_lte_enabled(gas_id))
    {
      for (i = 0;
           i < eutran_params->neighbour_list.count &&
           i < ARR_SIZE(eutran_params->neighbour_list.entries) &&
           mph_ded_wcdma_cell_update_list_req.eutran_frequency_count < ARR_SIZE(mph_ded_wcdma_cell_update_list_req.eutran_frequencies);
           ++i)
      {
        rr_dedicated_mode_eutran_neighbour_list_entry_t * src;

        src = &eutran_params->neighbour_list.entries[i];

        if (rr_l1_dedicated_wcdma_cell_update_list_req_contains_earfcn(
              &mph_ded_wcdma_cell_update_list_req,
              src->frequency.earfcn))
        {
          MSG_GERAN_MED_1_G("Filtering duplicate EARFCN %d", src->frequency.earfcn);
          continue;
        }
        else if (src->measurement_control == FALSE)
        {
          MSG_GERAN_MED_1_G("Filtering EARFCN %d, Measurement_Control_E-UTRAN is 0", src->frequency.earfcn);
          continue;
        }
        else
        {
          rr_l1_eutran_frequency_t * dst;

          dst = &mph_ded_wcdma_cell_update_list_req.eutran_frequencies[mph_ded_wcdma_cell_update_list_req.eutran_frequency_count];

          MSG_GERAN_HIGH_1_G("LTE frequency: %d", src->frequency.earfcn);

          if (lte_rrc_earfcn_is_supported(src->frequency.earfcn, rr_band_pref_lte(gas_id)))
          {
            uint32 pcid;

            dst->earfcn = src->frequency.earfcn;
            dst->measurement_bandwidth = src->frequency.measurement_bandwidth;

            if (lte_rrc_cell_is_tdd(src->frequency.earfcn))
            {
              dst->type = RR_L1_EUTRAN_TYPE_TDD;
            }
            else
            {
              dst->type = RR_L1_EUTRAN_TYPE_FDD;
            }

            for (pcid = RR_L1_PCID_MIN; pcid <= RR_L1_PCID_MAX; ++pcid)
            {
              if (RR_PCID_GROUP_IE_GET(src->not_allowed_cells.bitmap, pcid))
              {
                MSG_GERAN_MED_1_G("PCID %d Not Allowed", pcid);
                RR_L1_PCID_GROUP_BITMAP_SET(dst->not_allowed_cells, pcid);
              }
            }

            ++mph_ded_wcdma_cell_update_list_req.eutran_frequency_count;
          }
          else
          {
            MSG_GERAN_HIGH_1_G("Measurements on EARFCN %d not possible (Unsupported band)", src->frequency.earfcn);
          }
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("LTE not enabled in dedicated mode");
    }
  }
  else
  {
    /* No LTE neighbour cell information is configured. */
    mph_ded_wcdma_cell_update_list_req.qsearch_c_eutran = RR_DEFAULT_QSEARCH_C_EUTRAN;
    mph_ded_wcdma_cell_update_list_req.eutran_frequency_count = 0;
  }
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
  /* 3G Ded mode log packet */
  (void) rr_log_rr_3g_measurement_params(
     TRUE,      // in_dedicated
     gas_id     // gas_id
   );
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_set_num_uarfcns_sent_to_l1_in_dedicated(
    rr_count_num_unique_uarfcns_in_dedicated(&mph_ded_wcdma_cell_update_list_req),
    gas_id
  );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
  if (mph_ded_wcdma_cell_update_list_req.wcdma_cell_list.num_of_cells > 0 )
  {
    rr_detect_and_remove_overlap_uarfcn_in_dedicated(&mph_ded_wcdma_cell_update_list_req, gas_id );
  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
/* Before sending detect Duplicates and Remove them from the list */
  if (mph_ded_wcdma_cell_update_list_req.eutran_frequency_count > 0 )
  { 
    rr_detect_and_remove_overlap_earfcn(NULL, &mph_ded_wcdma_cell_update_list_req, gas_id );
  }

  rr_ded_meas_reporting_allocate_lte_measurements_storage(
    &mph_ded_wcdma_cell_update_list_req,
    gas_id
  );
#endif /* FEATURE_LTE */

  rr_l1_send_message_to_l1((IMH_T *)&mph_ded_wcdma_cell_update_list_req, gas_id);
}

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */


/**
  @brief Send MPH_RESET_REQ to GL1.
*/
void rr_send_mph_reset_req(
  const gas_id_t gas_id
)
{
  mph_reset_req_T mph_reset_req;

  RR_PREPARE_L1_MESSAGE(mph_reset_req, MPH_RESET_REQ, gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* ensure that GRR is not waiting on other signals from L1 */
  rr_ms_l1_blocking_message_sent(MPH_RESET_CNF, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_l1_send_message_to_l1((IMH_T *)&mph_reset_req, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* rr_send_mph_reset_req */

#if defined (FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_GSM_TDS)

/**
  @brief Send MPH_INTERRAT_LIST_SRCH_TIME_REQ to GL1.
*/
void rr_send_mph_interrat_list_search_time_req(
  rr_l1_list_srch_time_req_command_T command,
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  mph_interrat_list_srch_time_req_T mph_irat_srch_time_req;

  RR_PREPARE_L1_MESSAGE(mph_irat_srch_time_req, MPH_INTERRAT_LIST_SRCH_TIME_REQ, gas_id);

  mph_irat_srch_time_req.command = command;

  switch (rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      mph_irat_srch_time_req.rat = RR_L1_IRAT_RAT_WCDMA;
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      mph_irat_srch_time_req.rat = RR_L1_IRAT_RAT_LTE;
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      mph_irat_srch_time_req.rat = RR_L1_IRAT_RAT_UTRAN_TDD;
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
      mph_irat_srch_time_req.rat = RR_L1_IRAT_RAT_UNDEFINED;
    }
  }

  rr_l1_send_message_to_l1((IMH_T *)&mph_irat_srch_time_req, gas_id);
}


/**
  @brief Send MPH_SUSPEND_GSM_MODE_REQ to GL1.
*/
void rr_send_mph_suspend_gsm_mode_req(
  const gas_id_t gas_id
)
{
  mph_suspend_gsm_mode_req_T mph_suspend_gsm_mode_req;

  RR_PREPARE_L1_MESSAGE(mph_suspend_gsm_mode_req, MPH_SUSPEND_GSM_MODE_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_suspend_gsm_mode_req, gas_id);
}


/**
  @brief Send MPH_RESUME_GSM_MODE_REQ to GL1.
*/
void rr_send_mph_resume_gsm_mode_req(
  const gas_id_t gas_id
)
{
  mph_resume_gsm_mode_req_T mph_resume_gsm_mode_req;

  RR_PREPARE_L1_MESSAGE(mph_resume_gsm_mode_req, MPH_RESUME_GSM_MODE_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_resume_gsm_mode_req, gas_id);
}

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH || FEATURE_GSM_TDS*/


/**
  @brief Send MPH_START_GSM_MODE_FOR_BPLMN_REQ to GL1.
*/
void rr_send_mph_start_gsm_mode_for_bplmn_req(
  uint32 timer_val,
  const gas_id_t gas_id
)
{
  mph_start_gsm_mode_for_bplmn_req_T mph_start_gsm_bplmn;

  RR_PREPARE_L1_MESSAGE(mph_start_gsm_bplmn, MPH_START_GSM_MODE_FOR_BPLMN_REQ, gas_id);

  mph_start_gsm_bplmn.timer_val      = timer_val;
  mph_start_gsm_bplmn.multi_sim_mode = geran_get_multi_sim_mode();

  /* before starting GL1, acquire the TRM lock using the exchange mechanism */
  /* which only succeeds for a multimode subscription during an X2G process */
  (void)geran_exchange_lock(GERAN_EXCHANGE_XTOG, gas_id);
  geran_set_exchange_on_release(GERAN_EXCHANGE_NULL, gas_id);

  MSG_GERAN_MED_2_G("MULTISIM=%d X2G=%d",
    (int)mph_start_gsm_bplmn.multi_sim_mode,
    (int)geran_was_lock_exchanged(GERAN_EXCHANGE_XTOG, gas_id));

  rr_l1_send_message_to_l1((IMH_T *)&mph_start_gsm_bplmn, gas_id);

  return;
}


/**
  @brief Send MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ to GL1.
*/
void rr_send_mph_resume_gsm_mode_for_bplmn_req(
  uint32 timer_val,
  const gas_id_t gas_id)
{
  mph_resume_gsm_mode_for_bplmn_req_T mph_resume_gsm_bplmn;

  RR_PREPARE_L1_MESSAGE(mph_resume_gsm_bplmn, MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ, gas_id);

  mph_resume_gsm_bplmn.timer_val = timer_val;

  rr_l1_send_message_to_l1((IMH_T *)&mph_resume_gsm_bplmn, gas_id);
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
  @brief Send MPH_L1_DS_ABORT_IND to GL1.
*/
void rr_send_mph_l1_ds_abort_ind(const rr_ds_abort_type_T abort_type, const rr_l1_message_id_enum_T pending_msg, const boolean run_to_completion, const gas_id_t gas_id)
{
  mph_l1_ds_abort_ind_T mph_l1_ds_abort_ind;

  RR_PREPARE_L1_MESSAGE(mph_l1_ds_abort_ind, MPH_L1_DS_ABORT_IND, gas_id);

  mph_l1_ds_abort_ind.abort_type       = abort_type;
  mph_l1_ds_abort_ind.pending_msg      = pending_msg;
  mph_l1_ds_abort_ind.complete_granted = run_to_completion;

  /* lint -e{778} */
  PUT_IMH_LEN(sizeof(mph_l1_ds_abort_ind_T) - sizeof(IMH_T),
              &(mph_l1_ds_abort_ind.message_header));

  rr_l1_send_message_to_l1((IMH_T *)&mph_l1_ds_abort_ind, gas_id);

  return;
}

/**
  @brief Send MPH_DSC_THRESHOLD_IND to GL1.
*/
void rr_send_mph_dsc_threshold_ind(const int8 percent_of_max, 
                                   const l2_channel_type_T l2_channel_type, 
                                   const rr_l1_epd_preference_t epd_pref,
                                   const gas_id_t gas_id
                                  )
{
  mph_dsc_threshold_ind_T mph_dsc_threshold_ind;
  rr_l1_freq_power_T      scell_meas;

  RR_PREPARE_L1_MESSAGE(mph_dsc_threshold_ind, MPH_DSC_THRESHOLD_IND, gas_id)

  /* retrieve the current serving cell measurements */
  scell_meas = rr_gprs_get_serving_measurements(gas_id);

  if ((rr_ms_other_stack_has_same_lai(gas_id)) || (PAGING_REORGANIZATION == rr_ce_get_ota_page_mode(gas_id)))  
  {
    //Other Stack camped on same LAI or GRR in PAGE-Reorg mode  hence overwrite the epd with OFF
    mph_dsc_threshold_ind.epd_pref = RR_L1_EPD_OFF_PREFERRED;
  }
  else
  {
    mph_dsc_threshold_ind.epd_pref = epd_pref;  
  }
  mph_dsc_threshold_ind.percent_of_max  = percent_of_max;
  mph_dsc_threshold_ind.RXLEV_average   = scell_meas.RXLEV_average;
  mph_dsc_threshold_ind.l2_channel_type = l2_channel_type;
  PUT_IMH_LEN(sizeof(mph_dsc_threshold_ind_T)-sizeof(IMH_T),
              &(mph_dsc_threshold_ind.message_header));

  rr_l1_send_message_to_l1((IMH_T *)&mph_dsc_threshold_ind, gas_id);

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/**
  @brief Send MPH_RAT_CHANGE_IND to GL1.
*/
void rr_l1_send_mph_rat_change_ind(
  const gas_id_t gas_id
)
{
  mph_rat_change_ind_T mph_rat_change_ind;

  RR_PREPARE_L1_MESSAGE(mph_rat_change_ind, MPH_RAT_CHANGE_IND, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&mph_rat_change_ind, gas_id);

} /* rr_l1_send_mph_rat_change_ind */


/**
  @brief Sends MPH_ENABLE_SLEEP_IND to GL1.
*/
void rr_l1_send_mph_enable_sleep_ind(
  const gas_id_t gas_id
)
{
  mph_enable_sleep_ind_T mph_enable_sleep_ind;

  RR_PREPARE_L1_MESSAGE(mph_enable_sleep_ind, MPH_ENABLE_SLEEP_IND, gas_id);

  rr_l1_send_message_to_l1((void *) &mph_enable_sleep_ind, gas_id);
}


/**
  @brief Send MPH_STOP_SURROUND_UPDATE_REQ to GL1.
*/
void rr_send_mph_stop_surround_update_req(
  const gas_id_t gas_id
)
{
  mph_stop_surround_update_req_t mph_stop_surround_update_req;

  /*Initialise the structure*/
  memset ((void *)&mph_stop_surround_update_req, 0, sizeof(mph_stop_surround_update_req_t));

  RR_PREPARE_L1_MESSAGE(mph_stop_surround_update_req, MPH_STOP_SURROUND_UPDATE_REQ, gas_id);

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_stop_surround_update_req, gas_id);

} /* end rr_send_mph_stop_surround_update_req() */



/**********************************************************
 *   Function name:  rr_send_mph_start_gprs_idle_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_START_GPRS_IDLE_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  Channel_T and CCCH_PCCCH_params_T union
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_start_gprs_idle_mode_req(
  ccch_pccch_params_t *idle_mode_params,
  const gas_id_t gas_id
)
{
  mph_start_gprs_idle_mode_req_t mph_start_gprs_idle_mode_req;

  /*Initialise the structure*/
  memset ((void *)&mph_start_gprs_idle_mode_req, 0, sizeof(mph_start_gprs_idle_mode_req_t));

  RR_PREPARE_L1_MESSAGE(mph_start_gprs_idle_mode_req, MPH_START_GPRS_IDLE_MODE_REQ, gas_id);

  /*load the data into the message*/
  mph_start_gprs_idle_mode_req.idle_mode_params = *idle_mode_params;

  /* START_GPRS_IDLE logging starts here */
  {
    gprs_scell_info_t              *scell_info_ptr;
    system_information_database_T  *gsm_ptr;

    scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    RR_NULL_CHECK_FATAL(scell_info_ptr);

    (void)rr_log_gprs_cell_selection_parameters(scell_info_ptr, gas_id);
    rr_log_camping_attempt_success(gas_id);
    /* log the loss of service in RR */
    rr_log_service_change(TRUE, gas_id);
    rr_log_ba_list(&(idle_mode_params->ccch_idle_mode_params.layer1_sys_info.BA_list), gas_id);

    gsm_ptr = &scell_info_ptr->gsm;
    if (gsm_ptr)
    {
      /* Log the GSM serving cell system information + cell information values */
      (void)rr_log_cell_info(gsm_ptr, gas_id);
      (void)rr_log_cell_options(&(gsm_ptr->cell_options), gas_id);
      (void)rr_log_cell_selection_parameters(&(gsm_ptr->cell_selection_parameters), gas_id);
      (void)rr_log_control_channel_description(&(gsm_ptr->control_channel_description), gas_id);
      (void)rr_log_rach_control_parameters(&(gsm_ptr->RACH_control_parameters), gas_id);
    }
  }
  /* START_GPRS_IDLE logging ends */

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  {
    gprs_scell_info_t *scell_ptr;
    best_six_ncells_t *best_six_ncells_ptr;

    scell_ptr = rr_gprs_get_scell_info(gas_id);
    RR_NULL_CHECK_FATAL(scell_ptr);

    best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
    RR_NULL_CHECK_FATAL(best_six_ncells_ptr);

    rr_notify_cell_id_change(
      scell_ptr->gsm.cell_identity,
      *best_six_ncells_ptr,
      scell_ptr->gsm.location_area_identification,
      gas_id
    );
  }
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(
    &mph_start_gprs_idle_mode_req.idle_mode_params.ccch_idle_mode_params.layer1_extra_gprs_sys_info.xmsi,
    gas_id
  );
#endif

  {
    gprs_scell_info_t *serving_cell_ptr;
    eng_mode_scell_info_t scell_info = {0};

    serving_cell_ptr = rr_gprs_get_scell_info(gas_id);
    RR_NULL_CHECK_FATAL(serving_cell_ptr);

    scell_info = geran_eng_mode_format_scell_info(
      serving_cell_ptr,
      ENG_MODE_NO_CAUSE,
      gas_id
    );

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_INFO,
      (void*)&scell_info,
      TRUE,
      gas_id
    );

    rr_eng_mode_update_nw_params(gas_id);
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_store_paging_info(
    &(idle_mode_params->ccch_idle_mode_params.layer1_sys_info),
    RR_GET_WORD_IMSI_MOD_1000(idle_mode_params->ccch_idle_mode_params.layer1_extra_gprs_sys_info.imsi_mod1000),
    gas_id
  );
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_start_gprs_idle_mode_req, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_GPRS_IDLE, gas_id);

#ifdef FEATURE_FAKE_BTS_CELL_INFO
  if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
  {
    rr_send_gcell_info_ind(gas_id);
  }
 #endif /*FEATURE_FAKE_BTS_CELL_INFO*/

} /* end rr_send_mph_start_gprs_idle_mode_req() */



/**********************************************************
 *   Function name:  rr_send_mph_gprs_l1param_update_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_GPRS_L1PARAM_UPDATE_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  channel_t and ccch_pccch_params_t union
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_gprs_l1param_update_req(
  ccch_pccch_params_t *l1_system_info_params,
  const gas_id_t gas_id
)
{
  mph_gprs_l1param_update_req_t mph_gprs_l1param_update_req;

  /*Initialise the structure*/
  memset ((void *)&mph_gprs_l1param_update_req, 0, sizeof(mph_gprs_l1param_update_req_t));

  RR_PREPARE_L1_MESSAGE(mph_gprs_l1param_update_req, MPH_GPRS_L1PARAM_UPDATE_REQ, gas_id);

  /*load the data into the message*/
  mph_gprs_l1param_update_req.l1_system_info_params = *l1_system_info_params;

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(
    &mph_gprs_l1param_update_req.l1_system_info_params.ccch_idle_mode_params.layer1_extra_gprs_sys_info.xmsi,
    gas_id
  );
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_store_paging_info(
    &(l1_system_info_params->ccch_idle_mode_params.layer1_sys_info),
    RR_GET_WORD_IMSI_MOD_1000(l1_system_info_params->ccch_idle_mode_params.layer1_extra_gprs_sys_info.imsi_mod1000),
    gas_id
  );
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_gprs_l1param_update_req, gas_id);

} /* end rr_send_mph_gprs_l1param_update_req() */



/**********************************************************
 *   Function name:  rr_send_mph_nc_measurements_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_NC_MEASUREMENTS_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  requires an inter_task_nc_frequency_list_t
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_nc_measurements_req(
  inter_task_BCCH_allocation_T *nc_freq_list,
  const gas_id_t gas_id
)
{
  mph_nc_measurements_req_t mph_nc_measurements_req;

  /*Initialise the structure*/
  memset ((void *)&mph_nc_measurements_req, 0, sizeof(mph_nc_measurements_req_t));

  RR_PREPARE_L1_MESSAGE(mph_nc_measurements_req, MPH_NC_MEASUREMENTS_REQ, gas_id);

  /*load the data into the message*/
  mph_nc_measurements_req.balist = *nc_freq_list;

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_nc_measurements_req, gas_id);

} /* end rr_send_mph_nc_measurements_req() */



/**********************************************************
 *   Function name:  rr_send_stop_mph_nc_measurements_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_STOP_NC_MEASUREMENTS_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  none
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_stop_nc_measurements_req(
  inter_task_BCCH_allocation_T  *balist,
  const gas_id_t gas_id
)
{
  mph_stop_nc_measurements_req_t mph_stop_nc_measurements_req;

  /*Initialise the structure*/
  memset ((void *)&mph_stop_nc_measurements_req, 0, sizeof(mph_stop_nc_measurements_req_t));

  RR_PREPARE_L1_MESSAGE(mph_stop_nc_measurements_req, MPH_STOP_NC_MEASUREMENTS_REQ, gas_id);

  /*Load the message data*/
  mph_stop_nc_measurements_req.balist = *balist;

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_stop_nc_measurements_req, gas_id);

} /* end rr_send_mph_stop_nc_measurements_req() */



/**********************************************************
 *   Function name:  rr_send_mph_gprs_surround_update_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_GPRS_SURROUND_UPDATE_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  data_format_t indication and gprs_surround_data_t
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

boolean rr_send_mph_gprs_surround_update_req(
  gprs_surround_data_t *gprs_surround_data,
  boolean cancel_non_drx,
  bcch_ncell_info_t *bcch_ncell_info,
  const gas_id_t gas_id
)
{
  mph_gprs_surround_update_req_t mph_gprs_surround_update_req;

  MSG_GERAN_MED_1_G("--BCCH SURROUND ARFCN:%d--",CONVERT_FROM_ARFCN_T(gprs_surround_data->bcch_params.bcch_arfcn));

  if (get_waiting_for_surround_response(gas_id))
  {
    clear_waiting_for_surround_response(gas_id);
    /* kill pending surround read */
    rr_send_mph_stop_surround_update_req(gas_id);
  }

  /*Initialise the structure*/
  memset((void *)&mph_gprs_surround_update_req, 0, sizeof(mph_gprs_surround_update_req_t));

  RR_PREPARE_L1_MESSAGE(mph_gprs_surround_update_req, MPH_GPRS_SURROUND_UPDATE_REQ, gas_id);

  /*Load the message data*/
  mph_gprs_surround_update_req.gprs_surround_data = *gprs_surround_data;
  mph_gprs_surround_update_req.cancel_non_drx = cancel_non_drx;

/* SURROUND_UPDATE logging starts here */
  {
    ARFCN_T dummy_pbcch = {INVALID_LOG_ARFCN, (sys_band_T)BAND_NONE};

    rr_log_gprs_surround_search_start(
      gprs_surround_data->bcch_params.bcch_arfcn,
      dummy_pbcch,
      gas_id
    );
  }
/* SURROUND_UPDATE logging ends */

  /* check whether the SI cache can satisfy this request */
  if (GRR_L1_DECODE_SI3_ONLY == gprs_surround_data->bcch_params.si_requested)
  {
    /* if a cached SI3 (or SI4) exists for the CGI of the current ncell*/
    if (bcch_ncell_info != NULL &&
        bcch_ncell_info->gsm.cell_id_valid)
    {

      if (rr_csi_retrieve_SI3_4(gprs_surround_data->bcch_params.bcch_arfcn,
                                &(bcch_ncell_info->gsm.location_area_identification),
                                (uint16*)(&(bcch_ncell_info->gsm.cell_id)),
                                gas_id))
      {
        /* Cached Sys Info can fulfil this request, no need to ask GL1 */
        MSG_GERAN_HIGH_0_G("System Information cache has been used for this request");
        return(TRUE);
      }
    }
  }

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_gprs_surround_update_req, gas_id);

  return(FALSE);
} /* end rr_send_mph_gprs_surround_update_req() */



/**********************************************************
 *   Function name:  rr_send_mph_gprs_reselection_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_GPRS_RESELECTION_REQ primitive
 *   ------------ to layer 1
 *
 *   Parameters:  data_format_t indication and gprs_surround_data_t
 *   ----------
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_gprs_reselection_req(
  gprs_surround_data_t *gprs_surround_data,
  boolean sync_flag,
  const gas_id_t gas_id
)
{
  mph_gprs_reselection_req_t mph_gprs_reselection_req;

  MSG_GERAN_MED_1_G("--BCCH RESELECTION ARFCN:%d--",CONVERT_FROM_ARFCN_T(gprs_surround_data->bcch_params.bcch_arfcn));

  /*Initialise the structure*/
  memset ((void *)&mph_gprs_reselection_req, 0, sizeof(mph_gprs_reselection_req_t));

  RR_PREPARE_L1_MESSAGE(mph_gprs_reselection_req, MPH_GPRS_RESELECTION_REQ, gas_id);

  /*Load the message data*/
  mph_gprs_reselection_req.gprs_surround_data = *gprs_surround_data;
  mph_gprs_reselection_req.sync_flag = sync_flag;

  /* GPRS_RESELECTION logging starts here */
  {
    BSIC_T null_bsic = {0};

    if (rr_log_reselection_log_complete(gas_id))
    {
      rr_log_reselect_start(rr_log_get_reselection_trigger(gas_id), gas_id);
    }

    rr_log_camping_attempt_start(
      gprs_surround_data->bcch_params.bcch_arfcn,
      null_bsic,
      gas_id
    );
    /* log the loss of service in RR */
  }
  /* GPRS_RESELECTION logging ends */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_l1_blocking_message_sent(MPH_RESELECTION_CNF, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_gprs_reselection_req, gas_id);

  rr_l1_send_set_l1_idle_status(RR_L1_IDLE_STATUS_NOT_IDLE, gas_id);

} /* end rr_send_mph_gprs_reselection_req() */


/**********************************************************
 *   Function name:  rr_send_mph_start_mm_non_drx_ind
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_START_MM_NON_DRX_IND primitive
 *   ------------ to layer 1
 *
 *   Parameters:  None
 *   ----------
 *
 *   Returns: none
 *   --------
 *
 ***********************************************************/

void rr_send_mph_start_mm_non_drx_ind(
  const gas_id_t gas_id
)
{
  mph_start_mm_non_drx_ind_t  mph_start_mm_non_drx_ind;

  /*Initialise the structure*/
  memset ((void *)&mph_start_mm_non_drx_ind, 0, sizeof(mph_start_mm_non_drx_ind_t));

  RR_PREPARE_L1_MESSAGE(mph_start_mm_non_drx_ind, MPH_START_MM_NON_DRX_IND, gas_id);

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_start_mm_non_drx_ind, gas_id);

} /* end rr_send_mph_start_mm_non_drx_ind() */



/*===========================================================================

FUNCTION rr_send_mph_stop_mm_non_drx_ind

DESCRIPTION
  Sends MPH_STOP_MM_NON_DRX_IND to L1

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  MPH_STOP_MM_NON_DRX_IND is sent to L1

===========================================================================*/

void rr_send_mph_stop_mm_non_drx_ind(
  const gas_id_t gas_id
)
{
  mph_stop_mm_non_drx_ind_t  mph_stop_mm_non_drx_ind;

  /*Initialise the structure*/
  memset ((void *)&mph_stop_mm_non_drx_ind, 0, sizeof(mph_stop_mm_non_drx_ind_t));

  RR_PREPARE_L1_MESSAGE(mph_stop_mm_non_drx_ind, MPH_STOP_MM_NON_DRX_IND, gas_id);

  /*send the message to the Layer 1 queue*/
  rr_l1_send_message_to_l1((IMH_T *)&mph_stop_mm_non_drx_ind, gas_id);

} /* end rr_send_mph_stop_mm_non_drx_ind() */


#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
/*===========================================================================

FUNCTION rr_gprs_send_mph_wcdma_cell_update_list_req

DESCRIPTION
  This function is used to send the current 3G neighbour cell list to L1
  when the UE is camped on a PBCCH or when the UE is operating in NC2 mode.

PARAMS
  (in) wcdma_neighbor_list

RETURN VALUE
  None

===========================================================================*/

#ifdef FEATURE_LTE

void rr_gprs_send_mph_wcdma_cell_update_list_req(
  const gas_id_t gas_id
)
{
  mph_wcdma_cell_update_list_req_T  msg;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  grr_3g_nlist_entry_t             *entry;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
  uint8                             i;
  gprs_scell_info_t                *scell = rr_gprs_get_scell_info(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_sys_info_3g_data_t            *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
  boolean                           use_priority_reselection_algo = FALSE;
  pmo_pcco_data_t                  *pmo_store_ptr = rr_get_pmo_data(gas_id);
#ifdef FEATURE_GSM_TDS
  boolean                           include_tdd = FALSE;
  boolean                           include_fdd = FALSE;
#endif /*FEATURE_GSM_TDS*/

  if (rr_g2x_measurements_allowed(gas_id) == FALSE)
  {
    return;
  }

  memset ((void *)&msg,0,sizeof(mph_wcdma_cell_update_list_req_T));

  RR_PREPARE_L1_MESSAGE(msg, MPH_WCDMA_CELL_UPDATE_LIST_REQ, gas_id);

  /* 3G_SEARCH_PRIO is always indicated as TRUE.  This parameter is no longer
  broadcast by the network, see 3GPP TSG document GP-092154. */
  msg.THREE_G_SEARCH_PRIO = TRUE;

  /* 3GPP 45.008 Section 10, Table 3, Note 1:

  If PBCCH does not exist, the MS shall perform cell re-selection according to
  the idle mode procedures defined in clause 6 (i.e. use parameter Qsearch_I).
  In case parameter Qsearch_P is broadcast on BCCH it shall be used according
  to clause 10 only if GPRS cell re-selection parameters for one or more cells
  are provided to the MS in a Packet Cell Change Order or Packet Measurement
  Order message. */

   if (rr_gprs_pmo_pcco_ncells_are_present(gas_id) ||
       scell->bcch_sys_info.pmo_scell_params_valid)
  {
       msg.WCDMA_QSearch_I = scell->gprs_3g_meas_params.Qsearch_P;
  }
  else
  {
      if (scell->gsm.utran_neighbor_list.si2q_params.WCDMA_3G_MP_Desc_PI)
      {
          msg.WCDMA_QSearch_I = scell->gsm.utran_neighbor_list.si2q_params.QSearch_I;
      }
      else
      {
          msg.WCDMA_QSearch_I = 7;
      }
  }
  msg.FDD_Qmin = scell->gprs_3g_meas_params.FDD_Qmin;

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
  #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */

  msg.qsearch_c_eutran = RR_DEFAULT_QSEARCH_C_EUTRAN;

  if (rr_is_lte_supported(gas_id) &&
      rr_serving_cell_priority_parameters_ie_received(rr_gprs_get_scell_info(gas_id)))
  {

    if(rr_gprs_get_nc_mode(gas_id) == NC_MEAS_NC2)
    {
      /*In NC2 mode MS should disable priority based reselction if it has not received any nc frequency list in either PMO/PCCO which
              will make sure that incase if priority information is not received in IDLE , GRR will continue reporting 3G cells refer to CR495051
              so make sure that once it comes out of NC2 mode the new list gets updated to L1. And incase  if it has been reported by N/W
              we can continue using priorty based ,so that L1 continues doing the measurement although we dont report the same in PMR*/
      MSG_GERAN_MED_2_G("NC2 mode:Is PMO NC frequency  valid %d and is nc_meas_params valid %d",rr_is_pmo_nc_frequency_list_valid,
                         (pmo_store_ptr->nc_meas_params.valid)
                        );

      if( (!rr_is_pmo_nc_frequency_list_valid(gas_id)) &&
	  	  ((pmo_store_ptr->nc_meas_params.valid))
        )
      {
        MSG_GERAN_MED_0_G("NC2 mode: priority reselection algo not applicable");
        use_priority_reselection_algo = FALSE;

      }
      else
      {
        use_priority_reselection_algo = TRUE;
        MSG_GERAN_MED_0_G("In NC2 mode GRR did not reverted to BA list hence continue using priority based cell ");

      }
    }
    else
    {
      use_priority_reselection_algo = TRUE;
    }

    if (use_priority_reselection_algo)
    {
      if (!rr_utran_priority_parameters_ie_received(rr_get_instances_ptr(SI2Q_INSTANCE, gas_id), gas_id))
      {
        /*For the case where priority based reselection is enabled and UTRAN
          priority parameters are not present delete the UTRAN measurements
          because we will not be sending any UTRAN cell info to L1*/
        MSG_GERAN_HIGH_0_G("RR delete UTRAN measurements UTRAN priority missing");
#ifdef FEATURE_GSM_TDS
        rr_g2w_utran_tdd_measurements_delete(gas_id);
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_WCDMA
        rr_g2w_wcdma_measurements_delete(gas_id);
#endif /*FEATURE_WCDMA*/
      }
    }
  }

#ifdef FEATURE_GSM_TDS
  {
    boolean found_tdd = FALSE;
    boolean found_fdd = FALSE;

    MSG_GERAN_MED_0_G("3G reselection cell list ->");
    for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
    {
      entry = &rr_sys_info_3g_data_ptr->grr_3g_nlist->entries[i];

      if ( entry->valid && !entry->deleted )
      {
        if (entry->type == UTRAN_TDD)
        {
          found_tdd = TRUE;
        }

        if (entry->type == UTRAN_FDD)
        {
          found_fdd = TRUE;
        }
      }
    }

    MSG_GERAN_MED_0_G("3G TDD unindexed cell list ->");
    if (rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.size > 0 ||
        rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.size > 0)
    {
      found_tdd = TRUE;
    }

    MSG_GERAN_MED_0_G("3G FDD unindexed cell list ->");
    if (rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.size > 0 ||
        rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter.size > 0)
    {
      found_fdd = TRUE;
    }

    if (found_tdd && found_fdd) /*not a known NW config, handled for robustness*/
    {
      if (rr_is_utran_tdd_enabled(gas_id) &&
          rr_is_wcdma_enabled(gas_id))
      {
        if (rr_is_utran_tdd_enabled_in_nw_plmn(gas_id)) /*use PLMN to decide*/
        {
           MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
           include_tdd = TRUE;
        }
        else
        {
           MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
           include_fdd = TRUE;
        }
      }
      else if (rr_is_wcdma_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
        include_fdd = TRUE;
      }
      else if (rr_is_utran_tdd_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
        include_tdd = TRUE;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
      }
    }
    else if (found_fdd)
    {
      if (rr_is_wcdma_enabled(gas_id))
      {
        include_fdd = TRUE;
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
      }
    }
    else if (found_tdd)
    {
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        include_tdd = TRUE;
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
    }
  }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
#ifdef FEATURE_GSM_TDS
  if (include_fdd)
#else
  if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/
  {

    if (rr_g2w_measurements_enabled_in_nv(gas_id))
    {
      for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
      {
        entry = &rr_sys_info_3g_data_ptr->grr_3g_nlist->entries[i];

        if (entry->valid && !entry->deleted)
        {
          if (!entry->frequency_only)
          {
            rr_utran_cell_T cell;

            cell.uarfcn.rat_type = RR_UTRAN_FDD;
            cell.uarfcn.uarfcn = entry->uarfcn;
            cell.cell_code = entry->scrambling_code;
            cell.diversity = entry->diversity;

            rr_wcdma_reselection_freq_list_add(
              &msg.wcdma_reselection_freq_list,
              cell.uarfcn.uarfcn,
              use_priority_reselection_algo,
              &msg.frequency_priority_list,
              gas_id
            );

            rr_wcdma_cell_list_add(
              &msg.wcdma_cell_list,
              cell,
              use_priority_reselection_algo,
              &msg.frequency_priority_list,
              gas_id
            );
          }
        }
      }

      if (rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.size > 0 ||
          rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter.size > 0)
      {
        for (i = 0; i < rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.size; i++)
        {
          rr_wcdma_reselection_freq_list_add(
            &msg.wcdma_reselection_freq_list,
            rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.uarfcns[i],
            use_priority_reselection_algo,
            &msg.frequency_priority_list,
            gas_id
          );
        }

        for (i = 0; i < rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter.size; i++)
        {
          rr_wcdma_reselection_freq_list_add(
            &msg.wcdma_reselection_freq_list,
            rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter.uarfcns[i],
            use_priority_reselection_algo,
            &msg.frequency_priority_list,
            gas_id
          );
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("G2W MEASUREMENTS DISABLED IN NV");
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("WCDMA is not enabled");
  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  if (include_tdd)
  {

    if (rr_g2t_measurements_enabled_in_nv(gas_id))
    {

      for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
      {
        entry = &rr_sys_info_3g_data_ptr->grr_3g_nlist->entries[i];

        if (entry->valid && !entry->deleted)
        {
          if (!entry->frequency_only)
          {
            rr_utran_cell_T cell;

            cell.uarfcn.rat_type = RR_UTRAN_TDD;
            cell.uarfcn.uarfcn = entry->uarfcn;
            cell.cell_code = entry->cell_parameter_id;
            cell.diversity = entry->diversity;
            cell.sync_case_tstd = entry->sync_case_TSTD;

            rr_utran_tdd_reselection_freq_list_add(
              &msg.utran_tdd_reselection_freq_list,
              cell.uarfcn.uarfcn,
              use_priority_reselection_algo,
              &msg.frequency_priority_list,
              gas_id
            );

            rr_tds_cell_list_add(
              &msg.utran_tdd_cell_list,
              cell,
              use_priority_reselection_algo,
              &msg.frequency_priority_list,
              gas_id
            );
          }
        }
      }

      if (rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.size > 0 ||
          rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.size > 0)
      {
        for (i = 0; i < rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.size; i++)
        {
          rr_utran_tdd_reselection_freq_list_add(
            &msg.utran_tdd_reselection_freq_list,
            rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.uarfcns[i],
            use_priority_reselection_algo,
            &msg.frequency_priority_list,
            gas_id
          );
        }

        for (i = 0; i < rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.size; i++)
        {
          rr_utran_tdd_reselection_freq_list_add(
            &msg.utran_tdd_reselection_freq_list,
            rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.uarfcns[i],
            use_priority_reselection_algo,
            &msg.frequency_priority_list,
            gas_id
          );
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("G2T MEASUREMENTS DISABLED IN NV");
    }

  }
  else
  {
    MSG_GERAN_HIGH_0_G("TD-SCDMA not in MPH_WCDMA_CELL_UPDATE_LIST_REQ");
  }
#endif /*FEATURE_GSM_TDS*/

  msg.use_priority_reselection_algo = use_priority_reselection_algo;

  /* use_priority_reselection_algo will be TRUE only if LTE is enabled and priority
  information is available to the UE. */
  if (use_priority_reselection_algo)
  {
    msg.geran_priority = (uint8) rr_geran_priority(gas_id);
    msg.THRESH_priority_search = scell->serving_cell_priority_params.thresh_priority_search;
    msg.qsearch_c_eutran = scell->eutran_params.meas_params.qsearch_c;

    if (rr_is_lte_enabled(gas_id) && rr_g2l_resel_allowed(gas_id))
    {
      if (rr_g2l_measurements_enabled_in_nv(gas_id))
      {

        for (i = 0;
             i < scell->lte_neighbour_list.count &&
             i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE &&
             msg.eutran_frequency_count < MAX_RR_L1_EUTRAN_FREQUENCIES;
             ++i)
        {
          rr_lte_neighbour_list_entry_t * src;

          src = &scell->lte_neighbour_list.entries[i];

          if (rr_l1_wcdma_cell_update_list_req_contains_earfcn(&msg, src->frequency.earfcn))
          {
            MSG_GERAN_MED_1_G("Filtering duplicate EARFCN %d", src->frequency.earfcn);
          }
          else
          {
            rr_l1_eutran_frequency_t * dst;
            dst = &msg.eutran_frequencies[msg.eutran_frequency_count];

            MSG_GERAN_HIGH_1_G("LTE frequency: %d", src->frequency.earfcn);

            if (rr_g2x_resel_to_lte_frequency_is_allowed(
                  src->frequency.earfcn,
                  rr_band_pref_lte(gas_id),
                  FALSE,
                  FALSE,
                  gas_id
                )
            )
            {
              /* Priority-based reselection is only possible if a priority level is
              specified for the IRAT frequency. */
              rr_resel_priority_e eutran_priority = rr_get_resel_priority_for_earfcn(src->frequency.earfcn, gas_id);

              if (eutran_priority != RR_RESEL_PRIORITY_UNDEFINED)
              {
                uint32 pcid;

                dst->earfcn = src->frequency.earfcn;
                dst->measurement_bandwidth = src->frequency.measurement_bandwidth;
                if (lte_rrc_cell_is_tdd(src->frequency.earfcn))
                {
                  dst->type = RR_L1_EUTRAN_TYPE_TDD;
                }
                else
                {
                  dst->type = RR_L1_EUTRAN_TYPE_FDD;
                }

                for (pcid = RR_L1_PCID_MIN; pcid <= RR_L1_PCID_MAX; ++pcid)
                {
                  if (RR_PCID_GROUP_IE_GET(src->not_allowed_cells.bitmap, pcid))
                  {
                    MSG_GERAN_MED_1_G("PCID %d Not Allowed", pcid);
                    RR_L1_PCID_GROUP_BITMAP_SET(dst->not_allowed_cells, pcid);
                  }
                }

                ++msg.eutran_frequency_count;

                rr_irat_frequency_priority_list_add(
                  &msg.frequency_priority_list,
                  RR_L1_IRAT_RAT_LTE,
                  src->frequency.earfcn,
                  (uint8) eutran_priority,
                  gas_id
                );
              }
              else
              {
                MSG_GERAN_HIGH_1_G("No priority specified for EARFCN %d, omitting from neighbour list", src->frequency.earfcn);
              }
            }
            else
            {
              MSG_GERAN_HIGH_1_G("Reselection to EARFCN %d not possible (Forbidden LAI/Non-equivalent PLMN/Unsupported band)", src->frequency.earfcn);
            }
          }
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("G2L MEASUREMENTS DISABLED IN NV");
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("LTE disabled");
    }
  }

#ifdef FEATURE_WCDMA
  if ( msg.wcdma_reselection_freq_list.num_of_frequencies > 0 )
  {
    rr_detect_and_remove_overlap_uarfcn_in_idle( &msg, gas_id);
  }
#endif

#ifdef FEATURE_SGLTE
#ifdef FEATURE_GSM_TDS
    if ( msg.utran_tdd_reselection_freq_list.num_of_frequencies > 0 )
    {
       rr_sglte_set_l1_tds_measurements_configured(TRUE, gas_id);
    }
    else
    {
       rr_sglte_set_l1_tds_measurements_configured(FALSE, gas_id);  
    }
#endif
#endif

#ifdef FEATURE_LTE
  if ( msg.eutran_frequency_count > 0 )
  {
    rr_set_lte_l1_measurements_configured(TRUE, gas_id);
    rr_detect_and_remove_overlap_earfcn(&msg, NULL, gas_id);
  }
  else
  {
    rr_set_lte_l1_measurements_configured(FALSE, gas_id);
  }
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_set_num_uarfcns_sent_to_l1(rr_count_num_unique_uarfcns(&msg), gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  rr_l1_send_message_to_l1((IMH_T *)&msg, gas_id);
}

#else /* FEATURE_LTE */

void rr_gprs_send_mph_wcdma_cell_update_list_req(
  const gas_id_t gas_id
)
{
  mph_wcdma_cell_update_list_req_T  msg;
  grr_3g_nlist_entry_t             *entry;
  uint8                             i;
  gprs_scell_info_t                *scell = rr_gprs_get_scell_info(gas_id);
  rr_sys_info_3g_data_t            *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);
#ifdef FEATURE_GSM_TDS
  boolean                           include_tdd = FALSE;
  boolean                           include_fdd = FALSE;
#endif /*FEATURE_GSM_TDS*/
  memset ((void *)&msg,0,sizeof(mph_wcdma_cell_update_list_req_T));

  RR_PREPARE_L1_MESSAGE(msg, MPH_WCDMA_CELL_UPDATE_LIST_REQ, gas_id);

  msg.WCDMA_QSearch_I = scell->gprs_3g_meas_params.Qsearch_P;
  msg.FDD_Qmin = scell->gprs_3g_meas_params.FDD_Qmin;

  #ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
  #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */

  /* 3G_SEARCH_PRIO is always indicated as TRUE.  This parameter is no longer
  broadcast by the network, see 3GPP TSG document GP-092154. */
  msg.THREE_G_SEARCH_PRIO = TRUE;

#ifdef FEATURE_GSM_TDS
  {
    boolean found_tdd = FALSE;
    boolean found_fdd = FALSE;

    MSG_GERAN_MED_0_G("3G reselection cell list ->");
    for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
    {
      entry = &rr_sys_info_3g_data_ptr->grr_3g_nlist->entries[i];

      if ( entry->valid && !entry->deleted )
      {
        if (entry->type == UTRAN_TDD)
        {
          found_tdd = TRUE;
        }

        if (entry->type == UTRAN_FDD)
        {
          found_fdd = TRUE;
        }
      }
    }

    if (found_tdd && found_fdd) /*not a known NW config, handled for robustness*/
    {
      if (rr_is_utran_tdd_enabled(gas_id) &&
          rr_is_wcdma_enabled(gas_id) )
      {
        if (rr_is_utran_tdd_enabled_in_nw_plmn(gas_id)) /*use PLMN to decide*/
        {
           MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
           include_tdd = TRUE;
        }
        else
        {
           MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
           include_fdd = TRUE;
        }
      }
      else if (rr_is_wcdma_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure FDD cells only in dedicated");
        include_fdd = TRUE;
      }
      else if (rr_is_utran_tdd_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("RR both TDD and FDD supported by NW, measure TDD cells only in dedicated");
        include_tdd = TRUE;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
      }
    }
    else if (found_fdd)
    {
      if (rr_is_wcdma_enabled(gas_id))
      {
        include_fdd = TRUE;
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: FDD");
      }
    }
    else if (found_tdd)
    {
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        include_tdd = TRUE;
        MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: TDD");
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("RR monitoring UTRAN RAT: NONE");
    }
  }
#endif /*FEATURE_GSM_TDS*/

  MSG_GERAN_MED_0_G("3G reselection cell list ->");
  for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
  {
    entry = &rr_sys_info_3g_data_ptr->grr_3g_nlist->entries[i];

    if ( entry->valid && !entry->deleted )
    {
#ifdef FEATURE_GSM_TDS

      if (include_tdd)
      {
        if (rr_g2t_measurements_enabled_in_nv(gas_id))
        {
          if (entry->type == UTRAN_TDD)
          {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
            if ( tdsrrc_is_arfcn_valid_sub_id(entry->uarfcn, RR_GAS_ID_TO_AS_ID) )
#else
            if ( tdsrrc_is_arfcn_valid(entry->uarfcn ) )
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
            {
              if (!entry->frequency_only )
              {
                if (msg.utran_tdd_cell_list.num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
                {
                  rr_utran_cell_T cell;

                  cell.uarfcn.rat_type = RR_UTRAN_TDD;
                  cell.uarfcn.uarfcn = entry->uarfcn;
                  cell.cell_code = entry->cell_parameter_id;
                  cell.diversity = entry->diversity;
                  cell.sync_case_tstd = entry->sync_case_TSTD;

                  rr_utran_tdd_reselection_freq_list_add(
                    &msg.utran_tdd_reselection_freq_list,
                    cell.uarfcn.uarfcn,
                    gas_id);

                  rr_utran_tdd_cell_list_add(
                    &msg.utran_tdd_cell_list,
                    cell,
                    gas_id);

                }
              }
            }
          }
        }
        else
        {
          MSG_GERAN_HIGH_0_G("G2T MEASUREMENTS DISABLED IN NV");
        }
      }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS
      if ( (entry->type == UTRAN_FDD) &&
           (include_fdd) )
#else
      if (rr_is_wcdma_enabled(gas_id))
#endif /*FEATURE_GSM_TDS*/

      {
#ifdef FEATURE_WCDMA
        if (rr_g2x_is_uarfcn_valid(
              entry->uarfcn,
              scell->gsm.location_area_identification.plmn_id,
              gas_id
            ))
        {
          if (!entry->frequency_only )
          {
              if (msg.wcdma_cell_list.num_of_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
            {
              rr_utran_cell_T cell;

              cell.uarfcn.rat_type = RR_UTRAN_FDD;
              cell.uarfcn.uarfcn = entry->uarfcn;
              cell.cell_code = entry->scrambling_code;
              cell.diversity = entry->diversity;

              rr_wcdma_reselection_freq_list_add(
                &msg.wcdma_reselection_freq_list,
                cell.uarfcn.uarfcn,
                gas_id);

              rr_wcdma_cell_list_add(
                &msg.wcdma_cell_list,
                cell,
                gas_id);
            }
          }
        }
#endif /* FEATURE_WCDMA */
      } /*entry type: FDD*/
    }
  }

#ifdef FEATURE_WCDMA
  if (rr_is_wcdma_enabled(gas_id))
  {
    if (rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.size > 0 ||
        rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter.size > 0)
    {
      MSG_GERAN_MED_0_G("3G reselection frequency list ->");

      for (i = 0; i < rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.size; i++)
      {
        if (msg.wcdma_reselection_freq_list.num_of_frequencies < MAX_RR_MPH_FREQ_UPDATE_LIST)
        {
          uint16 uarfcn = rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater.uarfcns[i];

          if (rr_g2x_is_uarfcn_valid(
                uarfcn,
                scell->gsm.location_area_identification.plmn_id,
                gas_id
              ))
          {
            rr_wcdma_reselection_freq_list_add(
                &msg.wcdma_reselection_freq_list,
                uarfcn,
                gas_id);
          }
        }
      }
    }
  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  if (rr_is_utran_tdd_enabled(gas_id))
  {
    if (rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.size > 0 ||
        rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.size > 0)
    {
      for (i = 0; i < rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.size; i++)
      {
        if (msg.utran_tdd_reselection_freq_list.num_of_frequencies < MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)
        {
          uint16 uarfcn = rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater.uarfcns[i];

#ifdef FEATURE_TDS_SUB_ID_INTERFACE
          if (tdsrrc_is_arfcn_valid_sub_id(uarfcn, RR_GAS_ID_TO_AS_ID))
#else
          if (tdsrrc_is_arfcn_valid(uarfcn))
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
          {
            rr_utran_tdd_reselection_freq_list_add(
              &msg.utran_tdd_reselection_freq_list,
              uarfcn,
              gas_id);
          }
        }
      }

      for (i = 0; i < rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.size; i++)
      {
        if (msg.utran_tdd_reselection_freq_list.num_of_frequencies < MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)
        {
          uint16 uarfcn = rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter.uarfcns[i];

#ifdef FEATURE_TDS_SUB_ID_INTERFACE
          if (tdsrrc_is_arfcn_valid_sub_id(uarfcn, RR_GAS_ID_TO_AS_ID))
#else
          if (tdsrrc_is_arfcn_valid(uarfcn))
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
          {
            rr_utran_tdd_reselection_freq_list_add(
              &msg.utran_tdd_reselection_freq_list,
              uarfcn,
              gas_id);
          }
        }
      }
    }
  }
#endif /*FEATURE_GSM_TDS*/

#if defined (FEATURE_SGLTE) && defined (FEATURE_GSM_TDS)
  if ( msg.utran_tdd_reselection_freq_list.num_of_frequencies > 0 )
  {
    rr_sglte_set_l1_tds_measurements_configured(TRUE, gas_id);
  }
  else
  {
    rr_sglte_set_l1_tds_measurements_configured(FALSE, gas_id);  
  }
#endif /*FEATURE_SGLTE && FEATURE_GSM_TDS*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_set_num_uarfcns_sent_to_l1(rr_count_num_unique_uarfcns(&msg), gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
  if ( msg.wcdma_reselection_freq_list.num_of_frequencies > 0 )
  {
    rr_detect_and_remove_overlap_uarfcn_in_idle( &msg, gas_id);
  }
#endif

  rr_l1_send_message_to_l1((IMH_T *)&msg, gas_id);
}
#endif /* FEATURE_LTE */
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_DTM
/*===========================================================================

FUNCTION rr_send_mph_dtm_channel_release_req

DESCRIPTION

PARAMS
  (in) valid_bcch_carrier - TRUE if L1 should attempt to acquire the specified carrier
  (in) serving_bcch_carrier - AFCRN of the carrier that should be acquired
  (in) BSIC - BSIC of the carrier that should be acquired

RETURN VALUE
  None

===========================================================================*/
void rr_send_mph_dtm_channel_release_req(
  boolean        valid_bcch_carrier,
  ARFCN_T        serving_bcch_carrier,
  BSIC_T         BSIC,
  boolean        ps_release,
  const gas_id_t gas_id
)
{
  mph_dtm_channel_release_req_t  mph_dtm_channel_release_req;

  memset( (void *)&mph_dtm_channel_release_req, 0, sizeof(mph_dtm_channel_release_req_t) );

  RR_PREPARE_L1_MESSAGE(mph_dtm_channel_release_req, MPH_DTM_CHANNEL_RELEASE_REQ, gas_id);

  mph_dtm_channel_release_req.ps_release           = ps_release;
  mph_dtm_channel_release_req.valid_bcch_carrier   = valid_bcch_carrier;
  mph_dtm_channel_release_req.serving_bcch_carrier = serving_bcch_carrier;
  mph_dtm_channel_release_req.BSIC                 = BSIC;

  rr_l1_send_message_to_l1((IMH_T *)&mph_dtm_channel_release_req, gas_id);


  /* Invalidate RR's global timing advance information */
  rr_l1_send_invalidate_current_timing_advance_info(gas_id);
} /* rr_send_mph_dtm_channel_release_req */

#ifdef FEATURE_GSM_EDTM
/**********************************************************
 *   Function name:  rr_send_mph_dtm_l1param_update_req
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_DTM_L1PARAM_UPDATE_REQ primitive
 *   ------------ to Layer 1
 *
 *   Parameters:  ccch_or_pccch - indicates the cell multiframe type (51 or 52)
 *                scell_ptr     - pointer to serving cell database to be used
 *   ----------
 *   Returns:     Void
 *   --------
 *
 ***********************************************************/
void rr_send_mph_dtm_l1param_update_req(
  gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  mph_dtm_l1param_update_req_t mph_dtm_l1param_update_req;

  /* Initialise the structure */
  memset((void *)&mph_dtm_l1param_update_req, 0, sizeof(mph_dtm_l1param_update_req_t));

  RR_PREPARE_L1_MESSAGE(mph_dtm_l1param_update_req, MPH_DTM_L1PARAM_UPDATE_REQ, gas_id);

  /* load the data into the message */
  rr_gprs_get_l1_params(
    &(mph_dtm_l1param_update_req.l1_system_info_params),
    gas_id
  );

#ifdef FEATURE_GSM_PCH_SINGLE_BURST_DEC
  rr_l1_send_add_xmsi_data(
    &mph_dtm_l1param_update_req.l1_system_info_params.ccch_idle_mode_params.layer1_extra_gprs_sys_info.xmsi,
    gas_id
  );
#endif

  /* send the message to the Layer 1 queue */
  rr_l1_send_message_to_l1((IMH_T *)&mph_dtm_l1param_update_req, gas_id);
}
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/**********************************************************
 *   Function name:  rr_l1_send_mph_ded_wcdma_cell_upd_list_req_for_pseudo_fr
 *   ------------------------------------------------------
 *
 *   Description: Sends the MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ primitive
 *   ------------ to Layer 1 for TDS pseudo FR.  Only the TDD frequency list is
 *                populated.
 *
 *   Parameters:  pseudo_fr_ptr - pseudo fr info including frequency list.
 *   ----------
 *   Returns:     Void
 *   --------
 *
 ***********************************************************/
void rr_l1_send_mph_ded_wcdma_cell_upd_list_req_for_pseudo_fr(
  tdsrr_ho_pseudo_fast_return_type *pseudo_fr_ptr,
  const gas_id_t gas_id
)
{
  mph_dedicated_wcdma_cell_update_list_req_T  w_cell_upd_req;
  uint8                                       i = 0;

  /*Send MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ with Pseudo FR freq list*/
  RR_PREPARE_L1_MESSAGE(w_cell_upd_req, MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ, gas_id);

  w_cell_upd_req.wcdma_QSearch_C = QSEARCH_I_ALWAYS;
  w_cell_upd_req.SEARCH_PRIO_3G = TRUE;
  w_cell_upd_req.utran_tdd_freq_list.num_of_frequencies = pseudo_fr_ptr->num_frequencies;

  for (i=0; i<pseudo_fr_ptr->num_frequencies; i++)
  {
    w_cell_upd_req.utran_tdd_freq_list.uarfcn[i] = pseudo_fr_ptr->frequency_list[i];
  }

#ifdef FEATURE_GSM_TO_LTE
    w_cell_upd_req.qsearch_c_eutran = RR_DEFAULT_QSEARCH_C_EUTRAN;
#endif /*FEATURE_GSM_TO_LTE*/


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_set_num_uarfcns_sent_to_l1_in_dedicated(pseudo_fr_ptr->num_frequencies, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  rr_l1_send_message_to_l1((IMH_T *)&w_cell_upd_req, gas_id);

  MSG_GERAN_HIGH_0_G("RR sent MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ for TDS Pseudo FR");
}
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/**********************************************************
 *   Function name:  rr_l1_send_mph_nv_refresh_req
 *   ------------------------------------------------------
 *
 *   Description: 
 *   Sends the MPH_NV_REFRESH_REQ primitive to Layer 1 for refreshing 
 *   carrier specific EFS / NV items.
 *
 *   Parameters:  
 *   gas_id: GAS ID.
 *
 *   Returns:     Void
 *
 ***********************************************************/
void rr_l1_send_mph_nv_refresh_req(const gas_id_t gas_id)
{
  mph_nv_refresh_req_T nv_refresh_req;

  /* Send MPH_NV_REFRESH_REQ */
  RR_PREPARE_L1_MESSAGE(nv_refresh_req, MPH_NV_REFRESH_REQ, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&nv_refresh_req, gas_id);

  MSG_GERAN_HIGH_0_G("RR sent MPH_NV_REFRESH_REQ");
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/**********************************************************
 *   Function name:  rr_l1_send_mph_deep_sleep_ind
 *   ------------------------------------------------------
 *
 *   Description: 
 *   Sends the MPH_DEEP_SLEEP_IND primitive to Layer 1 for putting L1 
 *   in LPM mode while in INACTIVE state.
 *
 *   Parameters:  
 *   gas_id: GAS ID.
 *
 *   Returns:     Void
 *
 ***********************************************************/
void rr_l1_send_mph_deep_sleep_ind(const gas_id_t gas_id)
{
  mph_deep_sleep_ind_T deep_sleep_ind;

  /* Send MPH_DEEP_SLEEP_IND */
  RR_PREPARE_L1_MESSAGE(deep_sleep_ind, MPH_DEEP_SLEEP_IND, gas_id);

  rr_l1_send_message_to_l1((IMH_T *)&deep_sleep_ind, gas_id);

  MSG_GERAN_HIGH_0_G("RR sent MPH_DEEP_SLEEP_IND");
}

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST

/**
  @brief Sends MPH_REDUCED_NCELL_MEAS_IND to GL1.
*/
void rr_l1_send_reduced_ncell_meas_ind(
  boolean reduced_ncell_list,
  const gas_id_t gas_id
)
{
  mph_reduced_ncell_meas_ind_T mph_reduced_ncell_meas_ind;
    
  /*Initialise the structure*/
  memset ((void *)&mph_reduced_ncell_meas_ind, 0, sizeof(mph_reduced_ncell_meas_ind_T));
  
  RR_PREPARE_L1_MESSAGE(mph_reduced_ncell_meas_ind, MPH_REDUCED_NCELL_MEAS_IND, gas_id);

  /*Load the message data*/
  mph_reduced_ncell_meas_ind.reduced_ncell_list = reduced_ncell_list;
  
  rr_l1_send_message_to_l1((void *) &mph_reduced_ncell_meas_ind, gas_id);
}
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST*/
#ifdef FEATURE_IDLE_DRX_SCALING
/**********************************************************
 *   Function name:  rr_l1_send_mph_update_idle_drx_ind
 *   ------------------------------------------------------
 *
 *   Description: 
 *   Sends the MPH_UPDATE_IDLE_DRX_IND primitive to Layer 1 for updating
 *   extended DRX cycle and skipping IDLE mode meas.
 *
 *   Parameters:  
 *   bs_pa_mfrms: legacy BS_PA_MFRMS
 *   extended_bs_pa_mfrms_valid: TRUE is scaled BS_PA_MFRMS is valid
 *   extended_bs_pa_mfrms: scaled BS_PA_MFRMS
 *   skip_idle_mode_meas: if TRUE GL1 will stop ncell meas
 *   gas_id: GAS ID.
 *
 *   Returns:     Void
 *
 ***********************************************************/
void rr_l1_send_mph_update_idle_drx_ind(
  const byte     bs_pa_mfrms,
  const boolean  extended_bs_pa_mfrms_valid,
  const byte     extended_bs_pa_mfrms,
  const boolean  skip_idle_mode_meas,
  const gas_id_t gas_id
)
{
  mph_update_idle_drx_ind_T idle_drx_ind;

  RR_PREPARE_L1_MESSAGE(idle_drx_ind, MPH_UPDATE_IDLE_DRX_IND, gas_id);

  idle_drx_ind.BS_PA_MFRMS                = bs_pa_mfrms;
  idle_drx_ind.extended_BS_PA_MFRMS_valid = extended_bs_pa_mfrms_valid;
  idle_drx_ind.extended_BS_PA_MFRMS       = extended_bs_pa_mfrms;
  idle_drx_ind.skip_idle_mode_meas        = skip_idle_mode_meas;

  rr_l1_send_message_to_l1((IMH_T *)&idle_drx_ind, gas_id);

  MSG_GERAN_HIGH_4_G("RR sent MPH_UPDATE_IDLE_DRX_IND:: BS_PA_MFRMS=%d, ext_BS_PA_MFRMS=%d, ext_valid=%d, skip_meas=%d",
    bs_pa_mfrms, extended_bs_pa_mfrms, extended_bs_pa_mfrms_valid, skip_idle_mode_meas);
}
#endif /* FEATURE_IDLE_DRX_SCALING */

/**
  @brief Returns the current L1 idle mode
*/
rr_l1_idle_status_e rr_l1_send_get_l1_idle_status(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_l1_send_data_t *rr_l1_send_data_ptr = rr_l1_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_send_data_ptr);

  return rr_l1_send_data_ptr->l1_idle_status;
}

/*!
 * \brief Returns the current timing advance value. 
 *  
 * Note: There is a public API function is geran_grr_api.c which calls this function. That contains a mutex 
 * lock, so it is not required here. 
 * 
 * \param timing_advance_ptr (out)
 * \param gas_id (in)
 * 
 * \return boolean - TRUE if the timing advance value is valid, FALSE otherwise
 */
boolean rr_l1_send_get_current_timing_advance_info(
  byte *timing_advance_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_l1_send_data_t *module_data_ptr = rr_l1_send_get_data_ptr(gas_id);

  boolean timing_advance_valid = FALSE;

  if (timing_advance_ptr != NULL)
  {
    *timing_advance_ptr = module_data_ptr->current_timing_advance;
    timing_advance_valid = module_data_ptr->current_timing_advance_valid;
  }

  return timing_advance_valid;
}

/**
  @brief Initialisation function.
*/
void rr_l1_send_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_l1_send_data_t *rr_l1_send_data_ptr = rr_l1_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_l1_send_data_ptr);

  rr_l1_send_data_ptr->l1_idle_status = RR_L1_IDLE_STATUS_NOT_IDLE;
}

/**
  @brief Initialisation function. Should be called once at RR task start
*/
void rr_l1_send_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_l1_send_data, 0, sizeof(rr_l1_send_data));
}

/**
 * Send the mph_xmsi_update_ind primitive to the layer 1
 *
  * @param xmsi data
 */
void rr_send_mph_xmsi_update_ind(const gas_id_t gas_id)
{

#ifndef PERLUTF
  mph_xmsi_update_ind_T mph_xmsi_update_ind;
  RR_PREPARE_L1_MESSAGE(mph_xmsi_update_ind, MPH_XMSI_UPDATE_IND, gas_id);
  rr_l1_send_add_xmsi_data(&mph_xmsi_update_ind.xmsi, gas_id);
  rr_l1_send_message_to_l1((IMH_T *)&mph_xmsi_update_ind, gas_id);
#else
  #error code not present
#endif /* PERLUTF*/

  return;
} /* rr_send_mph_xmsi_update_ind */

/* EOF */

