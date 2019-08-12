/*==============================================================================

                        M O D U L E   T I T L E

GENERAL DESCRIPTION
  This module provides functions for sending primitives to MAC.  Each primitive
  has a corresponding function.

EXTERNALIZED FUNCTIONS
  [a list of the functions declared in this file]

INITIALIZATION AND SEQUENCING REQUIREMENTS
  rr_mac_send_reset_last_cell_info should be called at task initialisation.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_mac_send.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
20120520  SJW     DSDS priority inversion mechanism for NACC
20120403  SJW     Compilation maintenance for Dual SIM
20070207  GDJ     Remove redundant signals and add new IPA signals and
                  associated functionality.

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_mac_send.h"
#include "gmacgrr.h"
#include "gmacgrrsig.h"
#include "ms.h"
#include "err.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_general.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */
#include "rr_resel.h"
#include "rr_sys_info_pcell.h"
#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif /* FEATURE_GPRS_PS_HANDOVER */
#include "rr_multi_sim.h"
#include "rr_ps_domain.h"
#include "stringl/stringl.h"
#include "rr_public_store.h"
#include "geran_multi_sim_api.h"
#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct
{
  ARFCN_T BCCH_ARFCN;
  BSIC_T  BSIC;
} rr_mac_ps_access_cell_t;

typedef enum
{
  MAC_NO_PS_ACCESS_DEL_ALL,
  MAC_NO_PS_ACCESS,
  MAC_PS_ACCESS
} rr_mac_ps_access_status_t;

typedef struct
{
  rr_mac_ps_access_cell_t    last_ps_access_cell;
  rr_mac_ps_access_status_t  ps_access_status;
  boolean                    dl_msg_sent;
} rr_mac_send_data_t;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static rr_mac_send_data_t rr_mac_send_data[NUM_GERAN_DATA_SPACES];

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
static rr_mac_send_data_t *rr_mac_send_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_mac_send_data[as_index];
}

/*!
 * \brief Sets the PS access status.
 *
 * \param ps_access_status
 * \param gas_id
 */
static void rr_mac_send_set_ps_access_status(
  rr_mac_ps_access_status_t ps_access_status,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_mac_send_data_t *rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  rr_mac_send_data_ptr->ps_access_status = ps_access_status;
}

/*!
 * \brief Returns the PS access status.
 *
 * \param ps_access_status
 * \param gas_id
 */
static rr_mac_ps_access_status_t rr_mac_send_get_ps_access_status(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_mac_send_data_t *rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  return (rr_mac_send_data_ptr->ps_access_status);
}

/*===========================================================================

FUNCTION init_grr_mac_sig

DESCRIPTION
  This function is used to get a signal buffer from MAC, clear it out, and
  fill in the sig_id and task_id elements.

PARAMS
  grr_mac_sig_id_t - sig_id

RETURN VALUE
  Pointer to signal buffer (or NULL if memory allocation fails)

SIDE EFFECTS
  None

===========================================================================*/
static grr_mac_sig_t *init_grr_mac_sig(grr_mac_sig_id_t sig_id)
{
  grr_mac_sig_t *grr_mac_sig_ptr;

  grr_mac_sig_ptr = mac_get_cmd_buf_grr_mac();

  if (grr_mac_sig_ptr != NULL)
  {
    /* Clear out buffer */
    memset((void *)grr_mac_sig_ptr, 0, sizeof(grr_mac_sig_t));

    /* Fill in common signal contents */
    grr_mac_sig_ptr->task_id = MS_MAC_RR;
    grr_mac_sig_ptr->sig_id  = sig_id;
  }
  else
  {
    MSG_GERAN_ERROR_1("No GMAC command buffer for sig 0x%02X",(int)sig_id);
  }

  return(grr_mac_sig_ptr);
} /* end init_grr_mac_sig() */

/*=============================================================================

FUNCTION grr_send_message_to_mac

DESCRIPTION
  Send a message from GRR to MAC.

PARAMS
  mac_cmd_ptr - pointer to message to send to MAC.

RETURN VALUE
  None

SIDE EFFECTS
  May use the virtual MAC interface under Dual SIM conditions

=============================================================================*/
static void grr_send_message_to_mac(
  grr_mac_sig_t *mac_cmd_ptr,
  const gas_id_t gas_id
)
{
  if (mac_cmd_ptr->task_id == MS_MAC_RR)
  {
    /* message pointer is only used for MS_RR_L2 and MS_TIMER messages. Therefore,
       message pointer is set to null for MS_MAC_RR messages */
    rr_show_output_message("MAC", NULL, MS_MAC_RR, (int) mac_cmd_ptr->sig_id,gas_id);
    rr_debug_log_rr_mac_omsg(mac_cmd_ptr->sig_id, gas_id);

    mac_put_cmd_grr_mac(gas_id, mac_cmd_ptr);
  }
} /* end grr_send_message_to_mac() */


/*===========================================================================

FUNCTION:  decode_IA_starting_time

DESCRIPTION:
  Decodes the Starting Time from the IA Rest Octets

DEPENDENCIES:

RETURN VALUE:

SIDE EFFECTS:

===========================================================================*/
static word decode_IA_starting_time(word starting_time)
{
  byte T1_prime;
  byte T2;
  byte T3;

  T1_prime = (byte) ((starting_time & 0xF800) >> 11);
  T3 = (byte) ((starting_time & 0x07E0) >> 5);
  T2 = starting_time & 0x001F;

  starting_time = (word)(51 * ((26+(char)T3 - (char)T2) % 26)) +
                  (word)T3 +
                  (word)(51 * 26 * T1_prime);

  return starting_time;
}


/*===========================================================================

FUNCTION get_ps_access_req_action

DESCRIPTION
  Returns an indication of the actions that should be performed by RLC/MAC
  on resumption of PS services.

PARAMS
  purge_gmm_signalling - TRUE if GMM signalling should be purged even if the
                         current serving cell has not changed.

RETURN VALUE
  Action to be carrier out by RLC/MAC.

SIDE EFFECTS
  None

===========================================================================*/
static action_on_ps_access_t get_ps_access_req_action(
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
)
{
  boolean                 cell_changed = TRUE;
  action_on_ps_access_t   action = NO_CELL_CHANGE_NO_PURGE;
  gprs_scell_info_t     * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_mac_send_data_t    * rr_mac_send_data_ptr;

  // Obtain a pointer to the module data
  rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  if (ARFCNS_EQUAL(scell_info_ptr->gsm.BCCH_ARFCN,
                   rr_mac_send_data_ptr->last_ps_access_cell.BCCH_ARFCN))
  {
    if (scell_info_ptr->gsm.BSIC.BS_colour_code ==
        rr_mac_send_data_ptr->last_ps_access_cell.BSIC.BS_colour_code &&
        scell_info_ptr->gsm.BSIC.PLMN_colour_code ==
        rr_mac_send_data_ptr->last_ps_access_cell.BSIC.PLMN_colour_code)
    {
      cell_changed = FALSE;
    }
  }

  if (cell_changed)
  {
    action = CELL_CHANGE_PURGE_GMM_SIG;
  }
  else if (purge_gmm_signalling)
  {
    action = NO_CELL_CHANGE_PURGE_GMM_SIG;
  }

  if (cell_changed)
  {
    rr_mac_send_data_ptr->last_ps_access_cell.BCCH_ARFCN =
      scell_info_ptr->gsm.BCCH_ARFCN;

    rr_mac_send_data_ptr->last_ps_access_cell.BSIC =
      scell_info_ptr->gsm.BSIC;
  }

  return action;
}



/*=============================================================================

FUNCTION rr_mac_send_reset_last_cell_info

DESCRIPTION
  Resets the cell information used to determine if the cell has changed
  when sending a GRR_MAC_PS_ACCESS_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void rr_mac_send_reset_last_cell_info(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_mac_send_data_t *rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  rr_mac_send_data_ptr->last_ps_access_cell.BCCH_ARFCN.num  = INVALID_ARFCN;
  rr_mac_send_data_ptr->last_ps_access_cell.BCCH_ARFCN.band = SYS_BAND_NONE;

  rr_mac_send_data_ptr->last_ps_access_cell.BSIC.BS_colour_code   = INVALID_BSIC_CC;
  rr_mac_send_data_ptr->last_ps_access_cell.BSIC.PLMN_colour_code = INVALID_PLMN_CC;
} /* rr_mac_send_reset_last_cell_info */



/*=============================================================================

FUNCTION rr_send_grr_mac_ps_access_req

DESCRIPTION
  This function sends the signal GRR_MAC_PS_ACCESS_REQ to MAC.  This signal
  is sent when RR has determined that PS services are available and allowed.

PARAMS
  purge_gmm_signalling - TRUE if RLC should purge GMM signalling, FALSE
                         otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

=============================================================================*/
void rr_send_grr_mac_ps_access_req(
  boolean purge_gmm_signalling,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *sig = init_grr_mac_sig(GRR_MAC_PS_ACCESS_REQ);

  // Ensure the public store is updated with the data required by MAC/RLC
  rr_si_update_public_store(gas_id);

  if (sig != NULL)
  {
    boolean pcco_in_progress = FALSE;

    if (rr_pcco_in_progress(gas_id)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        || rr_x2g_cco_in_progress(gas_id)
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        )
    {
      pcco_in_progress = TRUE;
    }

    // GAS-ID is mandatory
    sig->msg.gm_ps_access_req.gas_id = gas_id;

    sig->msg.gm_ps_access_req.pcco_in_progress = pcco_in_progress;
    sig->msg.gm_ps_access_req.action = get_ps_access_req_action(purge_gmm_signalling, gas_id);

    grr_send_message_to_mac(sig, gas_id);

    /* Start a 5 second guard timer to make sure that we get a response. */
    grr_start_guard_timer(5000, EV_MAC_PS_ACCESS_GUARD_TIMER_EXPIRED, gas_id);

    // Update current PS access status
    rr_mac_send_set_ps_access_status(MAC_PS_ACCESS, gas_id);
  }

  rr_public_store_dump_to_diag(gas_id);
}

/*=============================================================================

FUNCTION rr_send_grr_mac_no_ps_access_ind

DESCRIPTION
  This function sends the signal GRR_MAC_NO_PS_ACCESS_IND to MAC.  This signal
  is sent when RR has determined that PS services are available and allowed.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

=============================================================================*/
void rr_send_grr_mac_no_ps_access_ind(
  delete_pdu_cause_t delete_pdu,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *sig_ptr;

  sig_ptr = init_grr_mac_sig(GRR_MAC_NO_PS_ACCESS_IND);

  if (sig_ptr != NULL)
  {
    // GAS-ID is mandatory
    sig_ptr->msg.gm_no_ps_access_ind.gas_id = gas_id;

    sig_ptr->msg.gm_no_ps_access_ind.delete_pdu = delete_pdu;

    grr_send_message_to_mac(sig_ptr, gas_id);

    /* Start a 5 second guard timer to make sure that we get a response. */
    grr_start_guard_timer(5000, EV_MAC_PS_ACCESS_GUARD_TIMER_EXPIRED, gas_id);

    // Update current PS access status
    if (delete_pdu == DEL_ALL)
    {
      rr_mac_send_set_ps_access_status(MAC_NO_PS_ACCESS_DEL_ALL, gas_id);
    }
    else
    {
      rr_mac_send_set_ps_access_status(MAC_NO_PS_ACCESS, gas_id);
    }
  }

  return;
}

/*=============================================================================

FUNCTION rr_find_best_fit_pmr

DESCRIPTION
  Determines optimal CSN.1 encoding for a PMR that includes 3G neighbours

PARAMS
  msg_ptr - pointer to structure that will be passed to CSN.1 encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void rr_find_best_fit_pmr(packet_measurement_report_t *msg_ptr, const gas_id_t gas_id)
{
  if (msg_ptr != NULL)
  {
    /* adjustment may be required if the PMR contains 3G cells */
    if (msg_ptr->three_g_measurement_report_flag)
    {
      int_data_t csn_struct;
      uint8      csn_encoded_pmr[UPLINK_MESSAGE_SIZE];
      uint8      max_2g;
      uint8      num_2g;

      /* create a local copy of the outbound structure */
      memscpy(&(csn_struct.type.pkt_meas_report),
              sizeof(packet_measurement_report_t),
              msg_ptr,
              sizeof(packet_measurement_report_t));

      /* determine how many 2G cells are proposed for encoding */
      max_2g = msg_ptr->enc_nc_meas_rprt_strct.num_nc_meas;

      for (num_2g = max_2g; num_2g != 0; num_2g--)
      {
        /* update the local structure with the NC count to be attempted */
        csn_struct.type.pkt_meas_report.enc_nc_meas_rprt_strct.num_nc_meas = num_2g;

        /* and then try to encode it - a FAIL response indicates overflow */
        if (PASS == gcsn1_encoder(PACKET_MEASUREMENT_RPT_PREPROCESS,
                                  csn_encoded_pmr,
                                  &csn_struct))
        {
          /* update the count in the outbound message */
          msg_ptr->enc_nc_meas_rprt_strct.num_nc_meas = num_2g;
          return;
        }
      }

      /* if the loop didn't find a best fit, leave the structure untouched per legacy encoding */
      MSG_GERAN_HIGH_1_G("No best fit found for PMR, use %d 2G ncells", max_2g);
    }
  }

  return;
}

/*=============================================================================

FUNCTION rr_send_grr_mac_send_meas_rpt

DESCRIPTION
  This function sends the signal GRR_MAC_SEND_MEAS_RPT to MAC.

PARAMS
  nco, nc_non_drx_period

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

=============================================================================*/
void rr_send_grr_mac_send_meas_rpt(
  pmr_or_pemr_e msg_type,
  pmr_or_pemr_u *msg,
  uint8 nco,
  uint8 nc_non_drx_period,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_SEND_MEAS_RPT );

  if ( grr_mac_sig != NULL )
  {
    if (msg_type == REPORT_TYPE_PMR)
    {
      /* ensure that 3G cells are being prioritised if present */
      rr_find_best_fit_pmr(&(msg->pmr), gas_id);
      MSG_GERAN_HIGH_2_G("Sending PMR with %d 2G ncells, max %d", msg->pmr.enc_nc_meas_rprt_strct.num_nc_meas, MAX_NC_MEAS);
    }

    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_send_meas_rpt.gas_id = gas_id;

    grr_mac_sig->msg.gm_send_meas_rpt.nco = nco;
    grr_mac_sig->msg.gm_send_meas_rpt.nc_non_drx_period = nc_non_drx_period;
    grr_mac_sig->msg.gm_send_meas_rpt.msg_type = msg_type;
    grr_mac_sig->msg.gm_send_meas_rpt.msg = *msg;
    grr_mac_sig->msg.gm_send_meas_rpt.gas_id = gas_id;

    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

  return;
} /* end rr_send_grr_mac_send_meas_rpt() */



/*===========================================================================

FUNCTION rr_send_grr_mac_send_pccf

DESCRIPTION
  This function sends the signal GRR_MAC_SEND_PCCF to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_send_pccf(
  packet_cell_change_failure_t pccf_msg,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig(GRR_MAC_SEND_PCCF);

  if (grr_mac_sig != NULL)
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_send_pccf.gas_id = gas_id;

    grr_mac_sig->msg.gm_send_pccf.pccf = pccf_msg;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_send_pccf() */



/*===========================================================================

FUNCTION rr_send_grr_mac_dl_tbf_est

DESCRIPTION
  This function sends the signal GRR_MAC_DL_TBF_EST to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dl_tbf_est(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t             *ia_rest_octets,
  frequency_list_T             *frequency_list,
  byte                          timing_advance,
  const gas_id_t                gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;
  ia_rest_octets_packet_downlink_ass_t *downlink_ass;
  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);

  downlink_ass = &(ia_rest_octets->contents.packet_downlink_ass);

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DL_TBF_EST );

  if (grr_mac_sig != NULL)
  {
    /* Set the dl_msg_set so when PS has priority over BPLMN and RR_PLMN_LIST_REQ
    from NAS before receiving MAC_GRR_MAC_STATUS BPLMN can be avoided */
    rr_mac_send_set_dl_msg_sent(TRUE,gas_id);

    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dl_tbf_est.gas_id = gas_id;

    /* Packet Channel Info */
    grr_mac_sig->msg.gm_dl_tbf_est.tn = packet_ch_desc->tn;
    grr_mac_sig->msg.gm_dl_tbf_est.tsc = packet_ch_desc->tsc;

    if (packet_ch_desc->rf_channel_config == CONFIG_NON_HOPPING)
    {
      grr_mac_sig->msg.gm_dl_tbf_est.hopping_flag = FALSE;
    }
    else
    {
      grr_mac_sig->msg.gm_dl_tbf_est.hopping_flag = TRUE;

      grr_mac_sig->msg.gm_dl_tbf_est.hsn = packet_ch_desc->hsn;
      grr_mac_sig->msg.gm_dl_tbf_est.maio = packet_ch_desc->maio;
    }

    grr_mac_sig->msg.gm_dl_tbf_est.frequency_list = (* frequency_list);

    /***********************/
    /* IA Rest Octets Info */
    /***********************/
    grr_mac_sig->msg.gm_dl_tbf_est.tlli = downlink_ass->tlli;

    if (downlink_ass->tfi_assignment_flag == valid)
    {
      grr_mac_sig->msg.gm_dl_tbf_est.tfi_assignment = downlink_ass->tfi_assignment;
      grr_mac_sig->msg.gm_dl_tbf_est.rlc_mode = downlink_ass->rlc_mode;

      grr_mac_sig->msg.gm_dl_tbf_est.alpha_flag = downlink_ass->alpha_flag;
      if (grr_mac_sig->msg.gm_dl_tbf_est.alpha_flag == valid)
      {
        grr_mac_sig->msg.gm_dl_tbf_est.alpha = downlink_ass->alpha;
      }
      else
      {
        grr_mac_sig->msg.gm_dl_tbf_est.alpha = scell_info->bcch_sys_info.power_control_params.alpha;
      }

      grr_mac_sig->msg.gm_dl_tbf_est.gamma = downlink_ass->gamma;
      grr_mac_sig->msg.gm_dl_tbf_est.polling_bit = downlink_ass->polling;
      grr_mac_sig->msg.gm_dl_tbf_est.ta_valid = downlink_ass->ta_valid;
    }

    grr_mac_sig->msg.gm_dl_tbf_est.timing_advance_idx_flag = downlink_ass->timing_advance_index_flag;
    if (grr_mac_sig->msg.gm_dl_tbf_est.timing_advance_idx_flag == valid)
    {
      grr_mac_sig->msg.gm_dl_tbf_est.timing_advance_idx = downlink_ass->timing_advance_index;
    }

    grr_mac_sig->msg.gm_dl_tbf_est.tbf_starting_time_flag = downlink_ass->tbf_starting_time_flag;
    if (grr_mac_sig->msg.gm_dl_tbf_est.tbf_starting_time_flag == valid)
    {
      grr_mac_sig->msg.gm_dl_tbf_est.tbf_starting_time = decode_IA_starting_time(downlink_ass->tbf_starting_time);
    }

    grr_mac_sig->msg.gm_dl_tbf_est.dl_pwr_ctrl_flag = downlink_ass->dwnlk_pwr_ctrl_flag;
    if (grr_mac_sig->msg.gm_dl_tbf_est.dl_pwr_ctrl_flag == valid)
    {

     grr_mac_sig->msg.gm_dl_tbf_est.dl_pwr_ctrl = downlink_ass->dwnlk_pwr_ctrl;
    }

    grr_mac_sig->msg.gm_dl_tbf_est.timing_advance = timing_advance;


    /* set EGPRS parameters */
    if ( valid == downlink_ass->egprs_tbf_mode_flag )
    {
      grr_mac_sig->msg.gm_dl_tbf_est.egprs_alloc = TRUE;
      grr_mac_sig->msg.gm_dl_tbf_est.egprs_dl_ws = downlink_ass->egprs_window_size; /*5-bit encoded*/
      grr_mac_sig->msg.gm_dl_tbf_est.egprs_lq_meas_mode = downlink_ass->link_quality_measurement_mode;

      if ( valid == downlink_ass->bep_period2_flag )
      {
        grr_mac_sig->msg.gm_dl_tbf_est.egprs_bep_period2_present = TRUE;
        grr_mac_sig->msg.gm_dl_tbf_est.egprs_bep_period2 = downlink_ass->bep_period2;
      }
    }
    else
    {
      grr_mac_sig->msg.gm_dl_tbf_est.egprs_alloc = FALSE;
    }


    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_dl_tbf_est() */



/*===========================================================================

FUNCTION rr_send_grr_mac_dl_sb_est

DESCRIPTION
  This function sends the signal GRR_MAC_DL_SB_EST to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dl_sb_est(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t             *ia_rest_octets,
  frequency_list_T             *frequency_list,
  byte                          timing_advance,
  const gas_id_t                gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  ia_rest_octets_packet_downlink_ass_t *downlink_ass;
  downlink_ass = &(ia_rest_octets->contents.packet_downlink_ass);

  grr_mac_sig = init_grr_mac_sig(GRR_MAC_DL_SB_EST);

  if ( grr_mac_sig != NULL )
  {
    /* Set the dl_msg_set so when PS has priority over BPLMN and RR_PLMN_LIST_REQ
    from NAS before receiving MAC_GRR_MAC_STATUS BPLMN can be avoided */
    rr_mac_send_set_dl_msg_sent(TRUE,gas_id);

    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dl_sb_est.gas_id = gas_id;

    /***********************/
    /* Packet Channel Info */
    /***********************/
    grr_mac_sig->msg.gm_dl_sb_est.tn = packet_ch_desc->tn;
    grr_mac_sig->msg.gm_dl_sb_est.tsc = packet_ch_desc->tsc;

    if ( packet_ch_desc->rf_channel_config == CONFIG_NON_HOPPING )
    {
      grr_mac_sig->msg.gm_dl_sb_est.hopping_flag = FALSE;
    }
    else
    {
      grr_mac_sig->msg.gm_dl_sb_est.hopping_flag = TRUE;

      grr_mac_sig->msg.gm_dl_sb_est.hsn = packet_ch_desc->hsn;
      grr_mac_sig->msg.gm_dl_sb_est.maio = packet_ch_desc->maio;
    }

    /* Copy over the frequency list */
    grr_mac_sig->msg.gm_dl_sb_est.frequency_list = (* frequency_list);

    /***********************/
    /* IA Rest Octets Info */
    /***********************/
    grr_mac_sig->msg.gm_dl_sb_est.tlli = downlink_ass->tlli;
    grr_mac_sig->msg.gm_dl_sb_est.tbf_starting_time = decode_IA_starting_time(downlink_ass->tbf_starting_time);

    grr_mac_sig->msg.gm_dl_sb_est.timing_advance = timing_advance;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_dl_sb_est() */



/*===========================================================================

FUNCTION rr_send_grr_mac_ul_tbf_est

DESCRIPTION
  This function sends the signal GRR_MAC_UL_TBF_EST to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_ul_tbf_est(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t             *ia_rest_octets,
  frequency_list_T             *frequency_list,
  byte                          timing_advance,
  byte                          no_of_rach_attempts,
  const gas_id_t                gas_id
)
{
  ia_multi_block_alloc_t *ia_rest_octets_ptr;

  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);
  grr_mac_sig_t *grr_mac_sig = init_grr_mac_sig( GRR_MAC_UL_TBF_EST );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_ul_tbf_est.gas_id = gas_id;

    /* Packet Channel Info */
    grr_mac_sig->msg.gm_ul_tbf_est.tn = packet_ch_desc->tn;
    grr_mac_sig->msg.gm_ul_tbf_est.tsc = packet_ch_desc->tsc;

    if ( packet_ch_desc->rf_channel_config == CONFIG_NON_HOPPING )
    {
      grr_mac_sig->msg.gm_ul_tbf_est.hopping_flag = FALSE;
    }
    else
    {
      grr_mac_sig->msg.gm_ul_tbf_est.hopping_flag = TRUE;

      grr_mac_sig->msg.gm_ul_tbf_est.hsn = packet_ch_desc->hsn;
      grr_mac_sig->msg.gm_ul_tbf_est.maio = packet_ch_desc->maio;
    }

    /* Copy over frequency list */
    grr_mac_sig->msg.gm_ul_tbf_est.frequency_list = (* frequency_list);

    if ( ia_rest_octets->contents_type == egprs_packet_uplink_ass )
    {
      /* IA Rest Octets Info */
      ia_rest_octets_egprs_packet_uplink_ass_t *ia_egprs_pua_ptr;
      ia_ro_egprs_pua_single_block_alloc_t     *ia_egprs_sb_ptr;

      ia_egprs_pua_ptr = &(ia_rest_octets->contents.egprs_packet_uplink_ass);
      ia_egprs_sb_ptr = &(ia_rest_octets->contents.egprs_packet_uplink_ass.block_alloc.single_block_alloc);

      grr_mac_sig->msg.gm_ul_tbf_est.tfi = ia_egprs_sb_ptr->tfi_assignment;

      grr_mac_sig->msg.gm_ul_tbf_est.polling_bit = ia_egprs_sb_ptr->polling;

      grr_mac_sig->msg.gm_ul_tbf_est.alloc_flag = ia_egprs_sb_ptr->alloc_flag;

      if ( ia_egprs_sb_ptr->alloc_flag == dynamic )
      {
         //Structure assignemnt 
        grr_mac_sig->msg.gm_ul_tbf_est.alloc.dynamic_alloc = ia_egprs_sb_ptr->alloc.dynamic_alloc;         
      
      }
      else
      {
      //Structure assignemnt
        grr_mac_sig->msg.gm_ul_tbf_est.alloc.fixed_alloc = ia_egprs_sb_ptr->alloc.fixed_alloc;
      }

      /* grr_mac_sig->msg.gm_ul_tbf_est.ch_block_coding_cmd not set for EGPRS */

      grr_mac_sig->msg.gm_ul_tbf_est.tlli_block_ch_coding = ia_egprs_sb_ptr->tlli_block_channel_coding;

      if (ia_egprs_sb_ptr->alpha_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.alpha = ia_egprs_sb_ptr->alpha;
      }
      else
      {
        /* if alpha isn't in the ul msg then take it from the global database */
        grr_mac_sig->msg.gm_ul_tbf_est.alpha = scell_info->bcch_sys_info.power_control_params.alpha;
      }

      grr_mac_sig->msg.gm_ul_tbf_est.gamma =  ia_egprs_sb_ptr->gamma;

      grr_mac_sig->msg.gm_ul_tbf_est.timing_advance_index_flag = ia_egprs_sb_ptr->timing_advance_index_flag;

      if (ia_egprs_sb_ptr->timing_advance_index_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.timing_advance_index = ia_egprs_sb_ptr->timing_advance_index;
      }

      if (ia_egprs_sb_ptr->tbf_starting_time_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time_flag = valid;
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time = decode_IA_starting_time(ia_egprs_sb_ptr->tbf_starting_time);
      }
      else
      {
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time_flag = not_valid;
      }

      /* Set up EGPRS parameters */
      grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_alloc = TRUE;

      if ( ia_egprs_pua_ptr->access_tech_type_count <= MAX_ACCESS_TECH_TYPE )
      {
	  memscpy( grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type,
			  sizeof(grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type),
			  ia_egprs_pua_ptr->access_tech_type,
			  sizeof(grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type[0]) * ia_egprs_pua_ptr->access_tech_type_count );
	
        grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count = ia_egprs_pua_ptr->access_tech_type_count;
      }
      else
      {
        grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.access_tech_type_count = 0;
        MSG_GERAN_ERROR_1_G("EGPRS Bad access_tech_type_count from Immed Ass, EGPRS PUA: %d", ia_egprs_pua_ptr->access_tech_type_count);
      }

      grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_channel_coding_command = ia_egprs_sb_ptr->egprs_channel_coding_command;

      if ( ia_egprs_sb_ptr->bep_period2_flag == valid )
      {
        grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.bep_period2 = ia_egprs_sb_ptr->bep_period2;
        grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.bep_period2_present = TRUE;
      }
      else
      {
        grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.bep_period2_present = FALSE;
      }

      grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.resegment = ia_egprs_sb_ptr->resegment;

      grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_window_size = ia_egprs_sb_ptr->egprs_window_size;
    }
    else if ( ia_rest_octets->contents_type == packet_uplink_ass )
    {

      grr_mac_sig->msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_alloc = FALSE;

      /* IA Rest Octets Info */
      ia_rest_octets_ptr = &(ia_rest_octets->contents.packet_uplink_ass.block_alloc.multi_block_alloc);

      grr_mac_sig->msg.gm_ul_tbf_est.tfi = ia_rest_octets_ptr->tfi_assignment;
      grr_mac_sig->msg.gm_ul_tbf_est.polling_bit = ia_rest_octets_ptr->polling;

      grr_mac_sig->msg.gm_ul_tbf_est.alloc_flag = ia_rest_octets_ptr->alloc_flag;
      if (ia_rest_octets_ptr->alloc_flag == dynamic)
      {
      grr_mac_sig->msg.gm_ul_tbf_est.alloc.dynamic_alloc = ia_rest_octets_ptr->alloc.dynamic_alloc;

      }
      else
      {
      grr_mac_sig->msg.gm_ul_tbf_est.alloc.fixed_alloc = ia_rest_octets_ptr->alloc.fixed_alloc;
      }

      grr_mac_sig->msg.gm_ul_tbf_est.ch_block_coding_cmd = ia_rest_octets_ptr->channel_coding_command;
      grr_mac_sig->msg.gm_ul_tbf_est.tlli_block_ch_coding = ia_rest_octets_ptr->tlli_block_channel_coding;

      if (ia_rest_octets_ptr->alpha_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.alpha = ia_rest_octets_ptr->alpha;
      }
      else
      {
        /* if alpha isn't in the ul msg then take it from the global database */
        grr_mac_sig->msg.gm_ul_tbf_est.alpha = scell_info->bcch_sys_info.power_control_params.alpha;
      }

      grr_mac_sig->msg.gm_ul_tbf_est.gamma = ia_rest_octets_ptr->gamma;

      grr_mac_sig->msg.gm_ul_tbf_est.timing_advance_index_flag = ia_rest_octets_ptr->timing_advance_index_flag;
      if (ia_rest_octets_ptr->timing_advance_index_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.timing_advance_index = ia_rest_octets_ptr->timing_advance_index;
      }

      if (ia_rest_octets_ptr->tbf_starting_time_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time_flag = valid;
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time = decode_IA_starting_time(ia_rest_octets_ptr->tbf_starting_time);
      }
      else
      {
        grr_mac_sig->msg.gm_ul_tbf_est.tbf_starting_time_flag = not_valid;
      }
    }

    grr_mac_sig->msg.gm_ul_tbf_est.timing_advance = timing_advance;

    /* tlli is not present in the uplink msg from the network so use the last TLLI from GMM */
    grr_mac_sig->msg.gm_ul_tbf_est.tlli =  rr_gmminfo_get_tlli(gas_id);

    /**/
    grr_mac_sig->msg.gm_ul_tbf_est.retry_bit = no_of_rach_attempts;

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    /* notify EDTM module */
    rr_gprs_edtm_gmac_uplink_pending(TRUE, gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_ul_tbf_est() */



/*===========================================================================

FUNCTION rr_send_grr_mac_ul_sb_est

DESCRIPTION
  This function sends the signal GRR_MAC_UL_SB_EST to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_ul_sb_est(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t *ia_rest_octets,
  frequency_list_T *frequency_list,
  byte              timing_advance,
  const gas_id_t    gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;
  ia_single_block_alloc_t *ia_rest_octets_ptr;

  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);
  grr_mac_sig = init_grr_mac_sig( GRR_MAC_UL_SB_EST );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_ul_sb_est.gas_id = gas_id;

    /* Packet Channel Info */
    grr_mac_sig->msg.gm_ul_sb_est.tn = packet_ch_desc->tn;
    grr_mac_sig->msg.gm_ul_sb_est.tsc = packet_ch_desc->tsc;

    if ( packet_ch_desc->rf_channel_config == CONFIG_NON_HOPPING )
    {
      grr_mac_sig->msg.gm_ul_sb_est.hopping_flag = FALSE;
    }
    else
    {
      grr_mac_sig->msg.gm_ul_sb_est.hopping_flag = TRUE;

      grr_mac_sig->msg.gm_ul_sb_est.hsn = packet_ch_desc->hsn;
      grr_mac_sig->msg.gm_ul_sb_est.maio = packet_ch_desc->maio;
    }

    /* Copy over the frequency list */
    grr_mac_sig->msg.gm_ul_sb_est.frequency_list = (* frequency_list);

    /* IA Rest Octets Info */
    if ( egprs_packet_uplink_ass == ia_rest_octets->contents_type )
    {
      ia_ro_egprs_pua_multi_block_alloc_t *egprs_multi_alloc_ptr = &ia_rest_octets->contents.egprs_packet_uplink_ass.block_alloc.multi_block_alloc;

      grr_mac_sig->msg.gm_ul_sb_est.alpha_flag = egprs_multi_alloc_ptr->alpha_flag;

      if ( grr_mac_sig->msg.gm_ul_sb_est.alpha_flag == valid )
      {
        grr_mac_sig->msg.gm_ul_sb_est.alpha = egprs_multi_alloc_ptr->alpha;
      }
      else
      {
        /* not sure about this code, it was copied from the code below, why set alpha when the flag is not valid ? */
        grr_mac_sig->msg.gm_ul_sb_est.alpha = scell_info->bcch_sys_info.power_control_params.alpha;
      }

      grr_mac_sig->msg.gm_ul_sb_est.gamma = egprs_multi_alloc_ptr->gamma;

      grr_mac_sig->msg.gm_ul_sb_est.starting_time = decode_IA_starting_time(egprs_multi_alloc_ptr->tbf_starting_time);

      /* Power Control Data */
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl_flag = egprs_multi_alloc_ptr->dwnlk_pwr_ctrl_flag;
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.po = egprs_multi_alloc_ptr->dwnlk_pwr_ctrl.po;
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.bts_pwr_ctrl_mode = egprs_multi_alloc_ptr->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode;
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.pr_mode = egprs_multi_alloc_ptr->dwnlk_pwr_ctrl.pr_mode;

      /* setup EGPRS parameters */
      grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc = TRUE;

      if ( ia_rest_octets->contents.egprs_packet_uplink_ass.access_tech_type_count <= MAX_ACCESS_TECH_TYPE )
      {
	  memscpy( grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type,
			   sizeof(grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type),
			  ia_rest_octets->contents.egprs_packet_uplink_ass.access_tech_type,
			  sizeof(grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type[0]) *
						ia_rest_octets->contents.egprs_packet_uplink_ass.access_tech_type_count );


        grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count = ia_rest_octets->contents.egprs_packet_uplink_ass.access_tech_type_count;
      }
      else
      {
        grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.access_tech_type_count = 0;
        MSG_GERAN_ERROR_1_G("EGPRS Bad access_tech_type_count from Immed Ass, EGPRS PUA: %d", ia_rest_octets->contents.egprs_packet_uplink_ass.access_tech_type_count);
      }

      grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_num_radio_blocks = egprs_multi_alloc_ptr->num_of_radio_blocks_allocated;
    }
    else
    {
      grr_mac_sig->msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc = FALSE;


      /* IA Rest Octets Info */
      ia_rest_octets_ptr = &(ia_rest_octets->contents.packet_uplink_ass.block_alloc.single_block_alloc);

      grr_mac_sig->msg.gm_ul_sb_est.alpha_flag = ia_rest_octets_ptr->alpha_flag;

      if (grr_mac_sig->msg.gm_ul_sb_est.alpha_flag == valid)
      {
        grr_mac_sig->msg.gm_ul_sb_est.alpha = ia_rest_octets_ptr->alpha;
      }
      else
      {
        grr_mac_sig->msg.gm_ul_sb_est.alpha = scell_info->bcch_sys_info.power_control_params.alpha;
      }

      grr_mac_sig->msg.gm_ul_sb_est.gamma = ia_rest_octets_ptr->gamma;

      grr_mac_sig->msg.gm_ul_sb_est.starting_time = decode_IA_starting_time(ia_rest_octets_ptr->tbf_starting_time);

      /* Power Control Data */
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl_flag = ia_rest_octets_ptr->dwnlk_pwr_ctrl_flag;
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.po = ia_rest_octets_ptr->dwnlk_pwr_ctrl.po;//should be p0
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.bts_pwr_ctrl_mode = ia_rest_octets_ptr->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode;
      grr_mac_sig->msg.gm_ul_sb_est.dl_pwr_ctrl.pr_mode = ia_rest_octets_ptr->dwnlk_pwr_ctrl.pr_mode;

    }

    grr_mac_sig->msg.gm_ul_sb_est.timing_advance = timing_advance;

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    /* notify EDTM module */
    rr_gprs_edtm_gmac_uplink_pending(TRUE, gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_ul_sb_est() */



/*===========================================================================

FUNCTION rr_send_grr_mac_rach_req

DESCRIPTION
  This function sends the signal GRR_MAC_RACH_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_rach_req(const gas_id_t gas_id)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_RACH_REQ );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_rach_req.gas_id = gas_id;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_rach_req() */




/*===========================================================================

FUNCTION rr_send_grr_mac_send_pccn_req

DESCRIPTION
  This function sends the signal GRR_MAC_SEND_PCCN_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_send_pccn_req(
  packet_cell_change_notification_t *pccn_ptr,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_SEND_PCCN_REQ );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_send_pccn_req.gas_id = gas_id;

    /* Fill in remainder of message */
    grr_mac_sig->msg.gm_send_pccn_req.pccn_msg = *pccn_ptr;

    /* invert the pccn_sending flag in the source data */
    if (pccn_ptr->pccn_sending != 0)
    {
      pccn_ptr->pccn_sending = 0;
    }
    else
    {
      pccn_ptr->pccn_sending = 1;
    }
    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
}



/*===========================================================================

FUNCTION rr_send_grr_mac_nacc_release_req

DESCRIPTION
  This function sends the signal GRR_MAC_NACC_RELEASE_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_nacc_release_req(
  uint8 container_id,
  uint8 release_flags,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_NACC_RELEASE_REQ );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_nacc_release_req.gas_id  = gas_id;

    /* Fill in remainder of message */
    grr_mac_sig->msg.gm_nacc_release_req.container_id  = container_id;
    grr_mac_sig->msg.gm_nacc_release_req.release_flags = release_flags;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
}




/*===========================================================================

FUNCTION rr_send_grr_mac_send_pss_req

DESCRIPTION
  This function sends the signal GRR_MAC_SEND_PSS_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_send_pss_req(
  packet_si_status_t *si_status_ptr,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_SEND_PSS_REQ );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_send_pss_req.gas_id = gas_id;

    /* Fill in remainder of message */
    grr_mac_sig->msg.gm_send_pss_req.pss_msg = *si_status_ptr;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
}


/*===========================================================================

FUNCTION rr_send_grr_mac_reset_req

DESCRIPTION
  This function sends the signal GRR_MAC_RESET_REQ to MAC.

PARAMS
  None

RETURN VALUE
  TRUE if the request was sent to GMAC, FALSE otherwise

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
boolean rr_send_grr_mac_reset_req(const gas_id_t gas_id)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig(GRR_MAC_RESET_REQ);

  if (grr_mac_sig != NULL)
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_reset_req.gas_id = gas_id;

    /* Put message in destination task queue */
    MSG_GERAN_HIGH_0_G("Sending GRR_MAC_RESET_REQ to initialise MAC/RLC");
    grr_send_message_to_mac(grr_mac_sig, gas_id);

    /* Cancel the guard on any message exchange currently in progress */
    grr_stop_guard_timer(gas_id);

    // Update current PS access status
    rr_mac_send_set_ps_access_status(MAC_NO_PS_ACCESS_DEL_ALL, gas_id);

    /* RESET dl_msg_sent */
    rr_mac_send_set_dl_msg_sent(FALSE,gas_id);
  }

  return((boolean)(grr_mac_sig != NULL));
} /* rr_send_grr_mac_reset_req() */



#ifdef FEATURE_GSM_DTM

/*===========================================================================

FUNCTION rr_send_grr_mac_dtm_channel_assign_req

DESCRIPTION
  This function sends the signal GRR_MAC_DTM_CHANNEL_ASSIGN_REQ to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dtm_channel_assign_req(
  uint8 cs_timeslot,
  cs_chan_desc_t * cs_chan_desc,
  frequency_information_T * dtm_freq_info,
  gprs_l1_cell_opt_t * l1_cell_options,
  rr_packet_uplink_assignment_t * pkt_uplink_assign,
  rr_packet_downlink_assignment_t * pkt_downlink_assign,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DTM_CHANNEL_ASSIGN_REQ );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dtm_channel_assign_req.gas_id = gas_id;

    /* The format of "current_cs_timeslot" is a bit-map, with bit0=TN7 and bit7=TN0 */
    grr_mac_sig->msg.gm_dtm_channel_assign_req.current_cs_timeslot = ( 0x80 >> cs_timeslot );

    grr_mac_sig->msg.gm_dtm_channel_assign_req.gprs_l1_cell_opt = (* l1_cell_options);

    if ( cs_chan_desc && dtm_freq_info )
    {
      grr_mac_sig->msg.gm_dtm_channel_assign_req.cs_present = TRUE;
      grr_mac_sig->msg.gm_dtm_channel_assign_req.cs_desc = (* cs_chan_desc);

      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_freq_present = TRUE;
      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_freq_info = (* dtm_freq_info);
    }

    if ( pkt_uplink_assign && pkt_downlink_assign )
    {
      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_tbf_type = UL_DL_TBF_PRESENT;
      grr_mac_sig->msg.gm_dtm_channel_assign_req.gm_dtm_ul_tbf_ass = (* pkt_uplink_assign);
      grr_mac_sig->msg.gm_dtm_channel_assign_req.gm_dtm_dl_tbf_ass = (* pkt_downlink_assign);
    }
    else
    if ( pkt_uplink_assign )
    {
      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_tbf_type = UL_TBF_PRESENT;
      grr_mac_sig->msg.gm_dtm_channel_assign_req.gm_dtm_ul_tbf_ass = (* pkt_uplink_assign);
    }
    else
    if ( pkt_downlink_assign )
    {
      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_tbf_type = DL_TBF_PRESENT;
      grr_mac_sig->msg.gm_dtm_channel_assign_req.gm_dtm_dl_tbf_ass = (* pkt_downlink_assign);
    }
    else
    {
      grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_tbf_type = NO_TBF_PRESENT;
    }

    MSG_GERAN_HIGH_3_G("DTM_CHANNEL_ASSIGN_REQ: cs_present=%d, tbf_type=%d",
             grr_mac_sig->msg.gm_dtm_channel_assign_req.cs_present,
             grr_mac_sig->msg.gm_dtm_channel_assign_req.dtm_tbf_type,
             0);

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
} /* end rr_send_grr_mac_dtm_channel_assign_req() */



/*===========================================================================

FUNCTION rr_send_grr_mac_dtm_channel_release_ind

DESCRIPTION
  This function sends the signal GRR_MAC_DTM_CHANNEL_RELEASE_IND to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dtm_channel_release_ind(
  dtm_ps_rel_cause_t ps_rel_cause,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DTM_CHANNEL_RELEASE_IND );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dtm_channel_release_ind.gas_id = gas_id;

    grr_mac_sig->msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause = ps_rel_cause;
    grr_mac_sig->msg.gm_dtm_channel_release_ind.dtm_ps_release = TRUE;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
} /* end rr_send_grr_mac_dtm_channel_release_ind() */



/*===========================================================================

FUNCTION rr_send_grr_mac_dtm_ul_dcch_ind

DESCRIPTION
  This function sends the signal GRR_MAC_DTM_UL_DCCH_IND to MAC.

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dtm_ul_dcch_ind(const gas_id_t gas_id)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DTM_UL_DCCH_IND );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dtm_ul_dcch_ind.gas_id = gas_id;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_dtm_ul_dcch_ind() */



/*===========================================================================

FUNCTION rr_send_grr_mac_dtm_ul_dcch_cnf

DESCRIPTION
  This function sends the signal GRR_MAC_DTM_UL_DCCH_CNF to MAC.

PARAMS
  l2_error - any error reported by L2

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_dtm_ul_dcch_cnf(
  error_code_T l2_error,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DTM_UL_DCCH_CNF );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dtm_ul_dcch_cnf.gas_id = gas_id;

    grr_mac_sig->msg.gm_dtm_ul_dcch_cnf.dummy_data = (uint8)l2_error;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_dtm_ul_dcch_cnf() */



#ifdef FEATURE_GSM_EDTM

void rr_send_grr_mac_edtm_pkt_cs_est_req(
  uint8 rr_cause,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig(GRR_MAC_EDTM_PKT_CS_EST_REQ);

  if (grr_mac_sig != NULL)
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_edtm_pkt_cs_est_req.gas_id = gas_id;

    grr_mac_sig->msg.gm_edtm_pkt_cs_est_req.rr_est_cause = rr_cause;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }
} /* end rr_send_grr_mac_edtm_pkt_cs_est_req() */



/*===========================================================================

FUNCTION rr_send_grr_mac_edtm_channel_release_ind

DESCRIPTION
  This function sends the signal GRR_MAC_DTM_CHANNEL_RELEASE_IND to MAC,
  but allows a variation on the function parameters for Enhanced DTM

PARAMS
  cell_type - either GPRS_PCCCH_CELL or GPRS_CCCH_CELL

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_edtm_channel_release_ind(const gas_id_t gas_id)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_DTM_CHANNEL_RELEASE_IND );

  if ( grr_mac_sig != NULL )
  {
    // GAS-ID is mandatory
    grr_mac_sig->msg.gm_dtm_channel_release_ind.gas_id = gas_id;

    grr_mac_sig->msg.gm_dtm_channel_release_ind.dtm_ps_rel_cause = NORMAL_REL;
    grr_mac_sig->msg.gm_dtm_channel_release_ind.dtm_ps_release   = FALSE;

    /* Put message in destination task queue */
    grr_send_message_to_mac(grr_mac_sig, gas_id);
  }

} /* end rr_send_grr_mac_edtm_channel_release_ind() */

#endif /*  FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================

FUNCTION rr_send_grr_mac_psho_req

DESCRIPTION
  This function sends the signal GRR_MAC_PSHO_REQ to MAC

PARAMS
  none

RETURN VALUE
  None

SIDE EFFECTS
  This will result in a task context switch

===========================================================================*/
void rr_send_grr_mac_psho_req(
  psho_type_t psho_type,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;
  ps_handover_command_t *psho_cmd;
  grr_mac_sig = init_grr_mac_sig( GRR_MAC_PSHO_REQ );

  if ( grr_mac_sig == NULL )
  {
    MSG_GERAN_ERROR_0_G("PSHO: mac buffer full");
    return;
  }

  // Ensure the public store is updated with the data required by MAC/RLC
  rr_si_update_public_store(gas_id);

  // GAS-ID is mandatory
  grr_mac_sig->msg.gm_psho_req.gas_id = gas_id;

  grr_mac_sig->msg.gm_psho_req.psho_type = psho_type;
  psho_cmd = (ps_handover_command_t *)rr_psho_get_details(gas_id);
  memscpy(&(grr_mac_sig->msg.gm_psho_req.psho_cmd),
  	      sizeof(grr_mac_sig->msg.gm_psho_req.psho_cmd), 
  	      psho_cmd, 
  	      sizeof(ps_handover_command_t)
  	      );


  grr_send_message_to_mac(grr_mac_sig, gas_id);

  // Update current PS access status
  rr_mac_send_set_ps_access_status(MAC_PS_ACCESS, gas_id);

  rr_public_store_dump_to_diag(gas_id);
}

void rr_send_grr_mac_psho_complete(
  psho_status_t status,
  const gas_id_t gas_id
)
{
  grr_mac_sig_t *grr_mac_sig;

  grr_mac_sig = init_grr_mac_sig( GRR_MAC_PSHO_COMPLETE_IND );
  if ( grr_mac_sig == NULL )
  {
    MSG_GERAN_ERROR_0_G("PSHO: mac buffer full");
    return;
  }

  // GAS-ID is mandatory
  grr_mac_sig->msg.gm_psho_complete_ind.gas_id = gas_id;

  grr_mac_sig->msg.gm_psho_complete_ind.psho_status = status;

  grr_send_message_to_mac(grr_mac_sig, gas_id);
}
#endif /* FEATURE_GPRS_PS_HANDOVER */


/**
  @brief Returns TRUE if PS access is currently enabled in MAC
*/
boolean rr_mac_ps_access_granted(const gas_id_t gas_id)
{
  return (rr_mac_send_get_ps_access_status(gas_id) == MAC_PS_ACCESS);
}


/**
  @brief Returns TRUE if PS access is currently blocked in MAC
*/
boolean rr_mac_ps_access_blocked(const gas_id_t gas_id)
{
  return (rr_mac_send_get_ps_access_status(gas_id) != MAC_PS_ACCESS);
}


/**
  @brief Returns TRUE if PS access is currently blocked in MAC and all PDUs deleted
*/
boolean rr_mac_ps_access_blocked_del_all(const gas_id_t gas_id)
{
  return (rr_mac_send_get_ps_access_status(gas_id) == MAC_NO_PS_ACCESS_DEL_ALL);
}


/**
  @brief Initialisation function
*/
void rr_mac_send_init(const gas_id_t gas_id)
{
  rr_mac_send_reset_last_cell_info(gas_id);
  rr_mac_send_set_ps_access_status(MAC_NO_PS_ACCESS_DEL_ALL, gas_id);
  rr_mac_send_set_dl_msg_sent(FALSE,gas_id);
}


/**
  @brief Initialisation function. Should be called once at RR task start
*/
void rr_mac_send_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_mac_send_data, 0, sizeof(rr_mac_send_data));
}

/*===========================================================================

FUNCTION rr_mac_send_get_dl_msg_sent

DESCRIPTION 
  Returns TRUE if GRR_MAC_DL_TBF_EST or GRR_MAC_DL_SB_EST is sent to GMAC,
  and waitin for MAC_GRR_MAC_STATUS FALSE otherwise
  
PARAMS
  None

RETURN VALUE
  boolean

===========================================================================*/

boolean rr_mac_send_get_dl_msg_sent(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_mac_send_data_t *rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  return (rr_mac_send_data_ptr->dl_msg_sent);

} /* end rr_mac_send_get_dl_msg_sent() */


/*===========================================================================

FUNCTION rr_mac_send_set_dl_msg_sent

DESCRIPTION 
  Sets the to TRUE, once GRR_MAC_DL_TBF_EST or GRR_MAC_DL_SB_EST sent.  

PARAMS
  dl_msg_sent

RETURN VALUE


===========================================================================*/

void rr_mac_send_set_dl_msg_sent(boolean dl_msg_sent,const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_mac_send_data_t *rr_mac_send_data_ptr = rr_mac_send_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_mac_send_data_ptr);

  rr_mac_send_data_ptr->dl_msg_sent = dl_msg_sent;

  return;

} /* end rr_mac_send_set_dl_msg_sent() */

/* EOF */

