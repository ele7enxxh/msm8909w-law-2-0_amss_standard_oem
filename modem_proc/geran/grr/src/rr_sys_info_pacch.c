/*! \file rr_sys_info_pacch.c

  The functionality in this module is related to decoding & processing of PCCO, PMO and PSI messages.

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_pacch.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sys_info_pacch.h"
#include "rr_nv.h"
#include "rr_sys_info.h"
#include "rr_sys_info_pcell.h"
#include "rr_resel_nacc.h"
#include "rr_gprs_main.h"
#include "rr_log.h"
#include "rr_gprs_edtm.h"
#include "rr_general.h"
#include "rr_mac_send.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define IGNORED_TC_VALUE      8

/*  3GPP 44.060 section 5.5.1.4.3 (Request for acquisition of system information):
 * "During the acquisition of PBCCH (respectively BCCH) information,
 *  up to four PACKET PSI STATUS (respectively PACKET SI STATUS)
 *  messages may be sent to the network"
 */
#define MAX_NUMBER_SYSINFO_STATUS_RETRANS 2

#define SI_MSG_NOT_RECEIVED        0x00
#define SI_MSG_SINGLE_INSTANCE     0x01
#define SI_MSG_PART_MULTI_INSTANCE 0x02
#define SI_MSG_FULL_MULTI_INSTANCE 0x03

/* DRX TIMER lookup: see 3GPP 44.060 12.24 GPRS Cell Options */
static const uint8 DRX_TIMER_MAX[8] = {0, 1, 2, 4, 8, 16, 32, 64};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint8  msg_type;
  uint16 msg_mask;
} status_priority_t;

typedef struct
{
  word    bsi_mask;
  boolean ps_release;
  boolean enhanced_priority;
} sys_info_status_t;

typedef struct
{
  sys_info_status_t sysinfo_status;
  int               num_sysinfo_status_retrans;
} rr_sys_info_pacch_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_sys_info_pacch_data_t rr_sys_info_pacch_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id (in)
 *
 * \return rr_psi_data_t*
 */
rr_sys_info_pacch_data_t *rr_sys_info_pacch_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sys_info_pacch_data[as_index];
}

/*!
 * \brief Decodes PSI13.
 *
 * \param psi13 (in)
 * \param decoded_psi13 (out)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the decode was successful; FALSE otherwise
 */
static boolean decode_psi13(
  packet_psi13_t *psi13,
  si13_data_t *decoded_psi13,
  const gas_id_t gas_id
)
{
  boolean range_check_passed = FALSE;

  /* Perform some range checking */
  /* Check that the NMO isn't "reserved" */
  if ( (psi13->pbcch_present_flag == FALSE) &&
       (psi13->gprs_cell_options.nmo == 0x03) )
  {
    MSG_GERAN_ERROR_0_G("PSI13: NMO value 'reserved' (3)");
  }
  else
  {
    range_check_passed = TRUE;
  }

  if ( ! range_check_passed )
  {
    MSG_GERAN_ERROR_0_G("PSI13 contents ignored");
    return FALSE;
  }

  decoded_psi13->bcch_change_mark = psi13->bcch_chng_mrk;
  decoded_psi13->si_change_field = psi13->si_chng_field;
  decoded_psi13->si13_change_mark = psi13->psi13_chng_mrk;

  /* If a GPRS Mobile Allocation IE is present, copy the contents into 'decoded_psi13' */
  /* but no decoding is performed */
  if ( psi13->si13_cm_gprs_ma_flag == valid )
  {
    rr_gprs_decode_mobile_alloc(
      &psi13->gprs_mobile_alloc,              // ma_data
      &decoded_psi13->mobile_allocation_ie,   // ma_store
      NULL,                                   // cell_alloc
      NULL                                    // mobile_alloc
    );

    decoded_psi13->mobile_allocation_status = IE_PRESENT;
  }
  else
  {
    decoded_psi13->mobile_allocation_status = IE_NOT_PRESENT;
  }

  decoded_psi13->bcch_pbcch.bcch.rac = psi13->rac;
  decoded_psi13->bcch_pbcch.bcch.spgc_ccch_sup = psi13->spgc_ccch_sup;
  decoded_psi13->bcch_pbcch.bcch.network_control_order = psi13->network_ctrl_ordr;
  decoded_psi13->bcch_pbcch.bcch.priority_access_thr = psi13->priority_access_thr;

  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.nmo =
    psi13->gprs_cell_options.nmo;
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.t3168 =
    psi13->gprs_cell_options.t3168;
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.t3192 =
    psi13->gprs_cell_options.t3192;
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.drx_timer_max =
    DRX_TIMER_MAX[psi13->gprs_cell_options.drx_timer_max];
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.access_burst_type =
    psi13->gprs_cell_options.access_burst_type;
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.ctrl_ack_type =
    psi13->gprs_cell_options.ctrl_ack_type;
  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.bs_cv_max =
    psi13->gprs_cell_options.bs_cv_max;

  if ( psi13->gprs_cell_options.pan_flag == valid )
  {
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_dec =
      psi13->gprs_cell_options.pan_dec;
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_inc =
      psi13->gprs_cell_options.pan_inc;
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_max =
      psi13->gprs_cell_options.pan_max;
  }
  else
  {
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_dec = 0;
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_inc = 0;
    decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.pan_max = 0;
  }

  decoded_psi13->bcch_pbcch.bcch.gprs_cell_options.optional_extension_info_flag =
    psi13->gprs_cell_options.optional_extension_info_flag;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.egprs_supported_flag =
      psi13->gprs_cell_options.optional_extension_info.egprs_supported_flag;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.
      optional_extension_info.egprs_packet_channel_request =
        psi13->gprs_cell_options.optional_extension_info.egprs_packet_channel_request;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.bep_period =
      psi13->gprs_cell_options.optional_extension_info.bep_period;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.pfc_feature_mode =
       psi13->gprs_cell_options.optional_extension_info.pfc_feature_mode;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.nw_ext_tbf =
      psi13->gprs_cell_options.optional_extension_info.nw_ext_tbf;
  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.ext_utbf_no_data =
      psi13->gprs_cell_options.optional_extension_info.ext_utbf_no_data;

  decoded_psi13->
    bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.ccn_active =
      psi13->gprs_cell_options.optional_extension_info.ccn_active;

#ifdef FEATURE_GSM_DTM
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
  {
    decoded_psi13->
      bcch_pbcch.bcch.gprs_cell_options.
        optional_extension_info.dtm_support =
          psi13->gprs_cell_options.optional_extension_info.dtm_support;
#ifdef FEATURE_GSM_EDTM
    decoded_psi13->
      bcch_pbcch.bcch.gprs_cell_options.
        optional_extension_info.dtm_enhancements_capability =
          psi13->gprs_cell_options.optional_extension_info.dtm_enhancements_capability;
#endif /* FEATURE_GSM_EDTM */
  }
  else
  {
    decoded_psi13->
      bcch_pbcch.bcch.gprs_cell_options.optional_extension_info.dtm_support =
        FALSE;
#ifdef FEATURE_GSM_EDTM
    decoded_psi13->
      bcch_pbcch.bcch.gprs_cell_options.
        optional_extension_info.dtm_enhancements_capability =
          FALSE;
#endif /* FEATURE_GSM_EDTM */
  }
#endif /* FEATURE_GSM_DTM */

  decoded_psi13->bcch_pbcch.bcch.power_control_params.alpha =
    psi13->alpha;
  decoded_psi13->bcch_pbcch.bcch.power_control_params.t_avg_w =
    psi13->t_avg_w;
  decoded_psi13->bcch_pbcch.bcch.power_control_params.t_avg_t =
    psi13->t_avg_t;
  decoded_psi13->bcch_pbcch.bcch.power_control_params.pc_meas_chan =
    psi13->pc_meas_chan;
  decoded_psi13->bcch_pbcch.bcch.power_control_params.n_avg_i =
    psi13->n_avg_i;
  decoded_psi13->bcch_pbcch.bcch.power_control_params.int_meas_chan_list_avail =
    FALSE;

  /* The SGSNR bit indicates the SGSN software rev. It has three states, depending on */
  /* a) its presence                                                                  */
  /* b) whether it is 0 or 1.                                                         */
  /* not present=R97, 0=R98, 1=R99                                                    */
  if ( psi13->r99_additions_flag == valid )
  {
    decoded_psi13->sgsnr = ( psi13->sgsnr ) ? SGSNR_REV_99 : SGSNR_REV_98;
  }
  else
  {
    decoded_psi13->sgsnr = SGSNR_NOT_PRESENT;
  }

  decoded_psi13->si_status_ind = psi13->si_status_ind;

  if (psi13->lb_ms_txpwr_max_cch == INVALID_LB_MS_TXPWR_MAX_CCH)
  {
    decoded_psi13->LB_MS_TXPWR_MAX_CCH_valid = FALSE;
    decoded_psi13->LB_MS_TXPWR_MAX_CCH = 0;
  }
  else
  {
    decoded_psi13->LB_MS_TXPWR_MAX_CCH_valid = TRUE;
    decoded_psi13->LB_MS_TXPWR_MAX_CCH = psi13->lb_ms_txpwr_max_cch;
  }

  return TRUE;

} /* end decode_psi13() */

/*!
 * \brief Processes PSI13.
 *
 *  PSI13 only has one instance, and is only received on the PACCH. It is not received as part of a
 *  scheduled acquisition, and refers to the serving cell only. It is equivalent to receiving SI13.
 *
 * \param psi13_ptr (in)  - Output from the CSN decoder.
 * \param bcch_arfcn (in)
 * \param gas_id (in)
 */
static void process_psi13(
  packet_psi13_t *psi13_ptr,
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
)
{
  si13_data_t decoded_psi13;

  if (rr_is_this_the_scell(bcch_arfcn, gas_id))
  {
    MSG_GERAN_MED_1("PSI13 from scell (arfcn=%d)",CONVERT_FROM_ARFCN_T(bcch_arfcn));

    if (decode_psi13(psi13_ptr, &decoded_psi13, gas_id))
    {
      gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);

      /* PSI13 is received on the PACCH while in transfer. The purpose of this is to update the */
      /* parameters which would be periodically read in SI13 */
      /* Decode the contents of the message as if it were SI13 */
      rr_gprs_process_psi13_ccch_camped(&decoded_psi13, scell_info, gas_id);
    }
  }
  else
  {
    MSG_GERAN_MED_1("PSI13 from unknown (arfcn=%d)",CONVERT_FROM_ARFCN_T(bcch_arfcn));
  }
} /* end process_psi13() */

/*!
 * \brief Treats PSI14 as PSI13 by converting the data into PSI13 format.
 *
 * \param psi14_ptr (in) - Output from the CSN decoder.
 * \param cell_info (in)
 * \param gas_id (in)
 */
static void process_psi14_as_psi13(
  packet_psi14_t *psi14_ptr,
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  si13_data_t decoded_psi13;

  if (rr_gprs_convert_psi14_to_si13_data(psi14_ptr, &decoded_psi13))
  {
    if ((psi14_ptr->ccch_pbcch_flag    == CCCH_ACCESS_INFO) &&
        (psi14_ptr->dtm_additions_flag == not_valid))
    {
      /* make sure the existing RAC is preserved */
      decoded_psi13.bcch_pbcch.bcch.rac = cell_info->bcch_sys_info.routing_area_code;
    }

    /* Decode the contents of the message as if it were SI13 */
    rr_gprs_process_psi13_ccch_camped(
      &decoded_psi13,
      cell_info,
      gas_id
    );
  }

  return;
}

/*!
 * \brief Processes PSI14.
 *
 *  Processes received PSI14 data. PSI14 only has one instance, and is only received on the serving cell's
 *  PACCH, or as part of a NACC container for the pending cell.
 *
 * \param psi14_ptr (in) - Output from the CSN decoder.
 * \param bcch_arfcn (in)
 * \param gas_id (in)
 */
static void process_psi14(
  packet_psi14_t *psi14_ptr,
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_this_the_scell(bcch_arfcn, gas_id))
  {
    gprs_scell_info_t *scell_info = rr_gprs_get_scell_info(gas_id);

    MSG_GERAN_HIGH_2_G("PSI14 from scell %d (PBCCH=%1d)",
             CONVERT_FROM_ARFCN_T(bcch_arfcn),
             (int)(psi14_ptr->ccch_pbcch_flag & 0x01));

    process_psi14_as_psi13(psi14_ptr, scell_info, gas_id);
  }
  else if (rr_is_this_the_pcell(bcch_arfcn, gas_id))
  {
    gprs_scell_info_t *pcell_info = rr_gprs_get_pcell_info(gas_id);

    MSG_GERAN_HIGH_1_G("PSI14 from pcell, PBCCH present = %d", psi14_ptr->ccch_pbcch_flag);

    rr_gprs_process_psi14_as_si13(psi14_ptr, pcell_info, gas_id);
  }
  else
  {
    MSG_GERAN_MED_1("PSI14 from unknown (arfcn=%d)",CONVERT_FROM_ARFCN_T(bcch_arfcn));
  }

  return;
}

/*!
 * \brief Checks the timers for "Pkt SI Status" or "Pkt PSI Status" retransmission.
 *
 *  Checks the timers for "Pkt SI Status" or "Pkt PSI Status" retransmission and cancels any that are
 *  running is the NW has responded correctly.
 *
 * \param msg_type (in) - CSN message type or PACKET_SI_DATA
 * \param gas_id (in)
 */
static void rr_gprs_check_status_resend_timers(
  uint8 msg_type,
  const gas_id_t gas_id
)
{
  boolean bCancel;

  /* SIs may be delivered to the MS via Pkt Serving Cell Data or Pkt Serving Cell SI */
  /* GMAC must be in TRANSFER mode.                                                  */
  bCancel = (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER);

  if (bCancel)
  {
    /* check whether the first retry timer is running */
    if (rr_timer_cancel(GRR_SYS_INFO_STATUS_TIMER_1, gas_id))
    {
      MSG_GERAN_MED_2_G("Cancelled resend timer 0x%02X (format %1d",
              (int)GRR_SYS_INFO_STATUS_TIMER_1, (int)msg_type);
    }

    /* check whether the second retry timer is running */
    if (rr_timer_cancel(GRR_SYS_INFO_STATUS_TIMER_2, gas_id))
    {
      MSG_GERAN_MED_2_G("Cancelled resend timer 0x%02X (format %1d)",
              (int)GRR_SYS_INFO_STATUS_TIMER_2, (int)msg_type);
    }
  }

  return;
}

/*!
 * \brief Converts PACKET_SI_DATA or PACKET_SERVING_CELL_SI into DL_UNIT_DATA_IND.
 *
 * \param mac_data_ind_ptr (in) - Pointer to the MAC_GRR_DATA_IND message
 * \param gas_id (in)
 */
static void rr_gprs_extract_si_data(
  mac_grr_data_ind_t *mac_data_ind_ptr,
  const gas_id_t gas_id
)
{
  dl_unit_data_ind_T si_data_ind;

  /* check for valid pointer before continuing */
  if (mac_data_ind_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Cannot extract SI Data from null message pointer");
    return;
  }

  /* wrap the incoming L3 message in its own DL_UNIT_DATA_IND */
  /* Note that header fields are not required because signal is being simulated */
  si_data_ind.tc = IGNORED_TC_VALUE;
  si_data_ind.SAPI = SAPI0;
  si_data_ind.short_header = FALSE;
  si_data_ind.channel_number = mac_data_ind_ptr->bcch_arfcn;
  si_data_ind.l2_channel_type = BCCH;
  si_data_ind.own_paging_block = FALSE;
  si_data_ind.l3_message_length = N201_MAX;

  /* the "data_ptr" in the csn_decoder_ptrs_t structure is an array of L3 msg octets */
  //Replace memcpy with memscpy
  memscpy(si_data_ind.layer3_message,sizeof(si_data_ind.layer3_message), mac_data_ind_ptr->data_ptr.data_ptr, N201_MAX);


  /* log the message here */
  (void) rr_log_signaling_message(
    BCCH,                             // channel_type
    si_data_ind.layer3_message[2],    // message_type
    N201_MAX,                         // message_length
    si_data_ind.layer3_message,       // message_data
    RR_DOWNLINK_MSG,                  // message_dir
    gas_id                            // gas_id
  );

  /* store the system information */
  rr_store_system_info(
    &si_data_ind,   // message_ptr
    gas_id          // gas_id
  );

  return;
}

/*!
 * \brief Initialises the structure required for encoding a PACKET SI STATUS message.
 *
 * \param si_status_ptr (out) - Pointer to output data structure.
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the network supports the sending of Pkt SI Status; FALSE otherwise
 */
static boolean rr_gprs_initialise_si_status(
  packet_si_status_t *si_status_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* Initialise the output structure */
  if (si_status_ptr != NULL)
  {
    memset(si_status_ptr, 0, sizeof(packet_si_status_t));
    si_status_ptr->bcch_chng_mrk = scell_info_ptr->bcch_sys_info.bcch_change_mark;

    /* the PS_REL_REQ flag is maintained in the main sys info module */
    si_status_ptr->ps_rel_req = rr_gprs_get_sysinfo_status_params(NULL, gas_id);
    si_status_ptr->rel6_additions_valid = valid;
  }

  if (scell_info_ptr->bcch_sys_info.si_status_ind == 0)
  {
    /* network does not support SI Status */
    MSG_GERAN_MED_0_G("WARNING: NW may not support Pkt SI Status");
    return(FALSE);
  }

  return(TRUE);
}

/*!
 * \brief Returns the SI status mask.
 *
 * \param gas_id (in)
 *
 * \return word - SI status mask
 */
static word rr_gprs_retrieve_si_status_mask(
  const gas_id_t gas_id
)
{
  word               bsi_mask;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  if (rr_in_dedicated(gas_id))
  {
    bsi_mask = rr_gprs_edtm_retrieve_bsi_status_mask(gas_id);
  }
  else
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  {
    bsi_mask = scell_info_ptr->gsm.update_message_received;
  }

  return(bsi_mask);
}

/*!
 * \brief Sets the count of received messages within a SI Status structure.
 *
 *  Sets the count of received messages within a SI Status structure, and sets the
 *  "additional msg" flag if the struture is full.
 *
 * \param si_status_ptr (out) - Pointer to output data structure.
 * \param count (in)          - Number of SI messages to be reported
 *
 * \return boolean - TRUE if the output structure is now complete; FALSE otherwise
 */
static boolean si_status_complete(
  packet_si_status_t *si_status_ptr,
  uint8 count
)
{
  si_status_ptr->rx_si_msg_struct.num_msg = count;

  if (count == MAX_SI_MSG)
  {
    /* set flag to indicate that the list is full */
    si_status_ptr->rx_si_msg_struct.additional_msg_type = 1;
    MSG_GERAN_HIGH_0("Pkt SI Status message is full");
    return(TRUE);
  }

  return(FALSE);
}

/*!
 * \brief Loads a data structure with the information needed to encode a PACKET SI STATUS control message.
 *
 *  Call this function ONLY after the MS has camped on to a CCCH cell.
 *  MAC must be in TRANSFER mode in order to send this message.
 *
 * \param si_mask (in)        - Mask of SIs that should be reported.
 * \param si_status_ptr (out) - Pointer to output data structure.
 * \param gas_id (in)
 */
static void rr_gprs_generate_si_status_from_mask(
  word si_mask,
  packet_si_status_t *si_status_ptr,
  const gas_id_t gas_id
)
{
  si2ter_instances_T *si2ter_inst_ptr;
  si_msg_list_t      *msg_ptr;
  instances_T        *si2q_inst_ptr;
  status_priority_t   status_priority[3];
  uint8               priority_count;
  uint8               count;
  word                sis_recvd;
  gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* initialise and establish pointer to first array element */
  msg_ptr = si_status_ptr->rx_si_msg_struct.si_msg_list;

  /* retrieve a mask of additional SIs which have been received */
  (void)rr_gprs_get_sysinfo_status_params(&sis_recvd, gas_id);
  /* and combine this with the provided mask */
  sis_recvd |= si_mask;

  /* the first entries in the status message need to be prioritised */
  if (rr_gprs_use_sysinfo_status_enhanced_priority(gas_id))
  {
    /* use the enhanced order specified in 3GPP 44.060 section 5.5.1.1b.5 */
    status_priority[0].msg_type = SYSTEM_INFORMATION_TYPE_13;
    status_priority[0].msg_mask = MASK_SI_13;
    status_priority[1].msg_type = SYSTEM_INFORMATION_TYPE_3;
    status_priority[1].msg_mask = MASK_SI_3;
    status_priority[2].msg_type = SYSTEM_INFORMATION_TYPE_1;
    status_priority[2].msg_mask = MASK_SI_1;
    priority_count = 3;
  }
  else
  {
    /* use the normal order specified in 3GPP 44.060 Table 5.5.1.4.3.1 */
    /* note that SI13 is not listed in the table, NW assumes reception */
    status_priority[0].msg_type = SYSTEM_INFORMATION_TYPE_1;
    status_priority[0].msg_mask = MASK_SI_1;
    status_priority[1].msg_type = SYSTEM_INFORMATION_TYPE_3;
    status_priority[1].msg_mask = MASK_SI_3;
    priority_count = 2;
  }


  /* load the first entries using the priority array generated above */
  for (count = 0; count < priority_count; count++)
  {
    msg_ptr->msg_type = status_priority[count].msg_type;
    if (ARE_SET(status_priority[count].msg_mask,sis_recvd))
    {
      msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
    }
    else
    {
      msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
    }
    msg_ptr++;
    if (si_status_complete(si_status_ptr, count)) return;
  }


  /* SI2 */
  msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_2;
  if (ARE_SET(MASK_SI_2,sis_recvd))
  {
    msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
  }
  else
  {
    msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
  }
  msg_ptr++;
  count++;
  if (si_status_complete(si_status_ptr, count)) return;


  /* SI2bis */
  msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_2_BIS;
  if (ARE_SET(MASK_SI_2_BIS,sis_recvd))
  {
    msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
  }
  else
  {
    msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
  }
  msg_ptr++;
  count++;
  if (si_status_complete(si_status_ptr, count)) return;


  /* SI2ter - multiple instance */
  si2ter_inst_ptr = rr_get_SI2ter_instances_ptr(gas_id);
  if ((si2ter_inst_ptr != NULL) &&
      (scell_info_ptr->gsm.two_ti != RR_SI2TER_NOT_EXPECTED))
  {
    /* only report Si2ter status if the cell indicates that it is being broadcast */
    msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_2_TER;
    if (si2ter_inst_ptr->num_instances)
    {
      si2ter_rest_T *valid_instance = NULL;
      uint8 i = 0;

      /*find the first valid instance*/
      for (i = 0; i < MAX_SI2TER_INSTANCES; i++)
      {
        if (si2ter_inst_ptr->instance[i].valid_data)
        {
          valid_instance = &si2ter_inst_ptr->instance[i];
          break;
        }
      }

      if (valid_instance == NULL)
      {
        MSG_GERAN_ERROR_1_G("RR error in SI2ter instances, num_inst:%d, no valid data",si2ter_inst_ptr->num_instances);
        msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
      }
      else
      {
        /* build the change mark */
        msg_ptr->six_chng_mrk = 0x00;
        if (valid_instance->SI2ter_3G_CHANGE_MARK)
        {
          msg_ptr->six_chng_mrk |= 0x01;
        }
        if (valid_instance->SI2ter_MP_CHANGE_MARK)
        {
          msg_ptr->six_chng_mrk |= 0x02;
        }

        if (valid_instance->SI2ter_COUNT == (si2ter_inst_ptr->num_instances-1))
        {
          msg_ptr->mess_rec = SI_MSG_FULL_MULTI_INSTANCE;
        }
        else
        {
          uint16 bitmap = 0x0000;
          uint8  msg_index;

          msg_ptr->mess_rec  = SI_MSG_PART_MULTI_INSTANCE;
          msg_ptr->six_count = valid_instance->SI2ter_COUNT;

          for (msg_index = 0; msg_index < MAX_SI2TER_INSTANCES; msg_index++)
          {
            if (si2ter_inst_ptr->instance[msg_index].valid_data)
            {
              bitmap |= (uint16) (1U << si2ter_inst_ptr->instance[msg_index].SI2ter_INDEX);
            }
          }

         msg_ptr->instance_bitmap = bitmap;
        }
      }
    }
    else
    {
      if ( si_mask & MASK_SI_2_TER_REST )
      {
        /* Only one SI2ter instance was required and the instance received did not contain rest octets,
           hence the instance_count of 0 */
        msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE;
      }
      else
      {
        msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
      }
    }
    msg_ptr++;
    count++;
    if (si_status_complete(si_status_ptr, count)) return;
  }


  /* SI2quater - multiple instance */
  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);
  if ((si2q_inst_ptr != NULL) &&
      (scell_info_ptr->gsm.two_quater_position != SI2Q_POSITION_NONE))
  {
    /* only report Si2quater status if the cell indicates that it is being broadcast */
    msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_2_QUATER;
    if (si2q_inst_ptr->num_instances)
    {
      SI2q_MI_instance_T *valid_instance = NULL;
      uint8 i = 0;

      /*find the first valid instance*/
      for (i = 0; i < MAX_INSTANCES; i++)
      {
        if (si2q_inst_ptr->instance[i].valid_data)
        {
          valid_instance = &si2q_inst_ptr->instance[i];
          break;
        }
      }

      if (valid_instance == NULL)
      {
        MSG_GERAN_ERROR_1_G("RR error in SI2Q instances, num_inst:%d, no valid data",si2q_inst_ptr->num_instances);
        msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
      }
      else
      {
        /* build the change mark */
        msg_ptr->six_chng_mrk = 0x00;
        if (valid_instance->common_params.MP_CHANGE_MARK)
        {
          msg_ptr->six_chng_mrk |= 0x01;
        }

        if (valid_instance->common_params.BA_IND_3G)
        {
          msg_ptr->six_chng_mrk |= 0x02;
        }

        if (valid_instance->common_params.BA_IND)
        {
          msg_ptr->six_chng_mrk |= 0x04;
        }

        if (valid_instance->common_params.COUNT == (si2q_inst_ptr->num_instances-1))
        {
          msg_ptr->mess_rec = SI_MSG_FULL_MULTI_INSTANCE;
        }
        else
        {
          uint16 bitmap = 0x0000;
          uint8  msg_index;

          msg_ptr->mess_rec  = SI_MSG_PART_MULTI_INSTANCE;
          msg_ptr->six_count = valid_instance->common_params.COUNT;

          for (msg_index = 0; msg_index < MAX_INSTANCES; msg_index++)
          {
            if (si2q_inst_ptr->instance[msg_index].valid_data)
            {
              bitmap |= (uint16) (1U << si2q_inst_ptr->instance[msg_index].common_params.INDEX);
            }
          }

          msg_ptr->instance_bitmap = bitmap;
        }
      }
    }
    else
    {
      msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
    }
    msg_ptr++;
    count++;
    if (si_status_complete(si_status_ptr, count)) return;
  }


  /* SI4 */
  msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_4;
  if (ARE_SET(MASK_SI_4,sis_recvd))
  {
    msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
  }
  else
  {
    msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
  }
  msg_ptr++;
  count++;
  if (si_status_complete(si_status_ptr, count)) return;


  /* SI7 and SI8 */
  if (scell_info_ptr->gsm.cell_selection_parameters.ACS == TRUE)
  {
    /* only report SI7 and SI8 status if the cell indicates that they are required */
    msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_7;
    if (ARE_SET(MASK_SI_7,sis_recvd))
    {
      msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
    }
    else
    {
      msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
    }
    msg_ptr++;
    count++;
    if (si_status_complete(si_status_ptr, count)) return;


    /* SI8 */
    msg_ptr->msg_type = SYSTEM_INFORMATION_TYPE_8;
    if (ARE_SET(MASK_SI_8,sis_recvd))
    {
      msg_ptr->mess_rec = SI_MSG_SINGLE_INSTANCE; /* single instance received */
    }
    else
    {
      msg_ptr->mess_rec = SI_MSG_NOT_RECEIVED; /* message not received */
    }
    msg_ptr++;
    count++;
    /* set the final count */
    si_status_ptr->rx_si_msg_struct.num_msg = count;
  }

  return;
}

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Store a GPRS mobile allocation.
 *
 * \param ma_number (in)        - MA number (MA_NUMBER_14 or MA_NUMBER_15).
 * \param new_mobile_alloc (in) - Mobile allocation to be stored.
 * \param cell_info (in)        - Serving cell info.
 * \param gas_id (in)
 */
void rr_gprs_store_mobile_alloc(
  uint8                     ma_number,
  gprs_mobile_allocation_t *new_mobile_alloc,
  gprs_scell_info_t        *cell_info,
  const gas_id_t            gas_id
)
{
  if ((ma_number == MA_NUMBER_14) || (ma_number == MA_NUMBER_15))
  {
    gprs_mobile_allocations_t  * mobile_allocs = &cell_info->mobile_allocations;
    rrps_frequency_list_t      * store = &mobile_allocs->gprs_mobile_allocation[MAP_MA_NUMBER_TO_OFFSET(ma_number)];

    rr_gprs_conv_mobile_alloc_to_rrps_format(new_mobile_alloc, store);

    /* Indicate this MA_NUMBER is valid */
    SET_BIT((uint16) (1U << ma_number), mobile_allocs->ma_valid_bitmap);

    /* Now update the new MA valid bit-mask in the Public Store */
    if ( cell_info->gsm.camped_on_flag )
    {
      rrps_store_update_data_t *rrps_store_update_ptr = rr_get_rrps_store_update(gas_id);

      if (rrps_store_update_ptr != NULL)
      {
        rrps_store_update_ptr->ma_valid_bitmap = cell_info->mobile_allocations.ma_valid_bitmap;

        rr_write_public_store(
          RRPS_UPD_VALID_MA_BIT_MAP,    // update_bitmap
          rrps_store_update_ptr,        // data
          gas_id                        // gas_id
        );

        rr_free_rrps_store_update(
          rrps_store_update_ptr,    // ptr
          gas_id                    // gas_id
        );
      }
      else
      {
        MSG_GERAN_ERROR_0_G("rrps_store_update_ptr = NULL");
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Invalid ma_number: %d", ma_number);
  }
} /* end rr_gprs_store_mobile_alloc() */

/*!
 * \brief Updated sys info parameters have been received from GMAC.
 *
 * \param mac_power_control_params (in) - Updated power control parameters.
 * \param gas_id (in)
 */
void rr_gprs_updated_sys_info_params_from_mac(
  global_power_control_params_t *mac_power_control_params,
  const gas_id_t gas_id
)
{
  global_power_control_params_t  *scell_power_control_params;
  gprs_scell_info_t              *scell_info;

  RR_NULL_CHECK_RETURN_VOID(mac_power_control_params);

  scell_info = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info);

  scell_power_control_params = &scell_info->bcch_sys_info.power_control_params;

  if (    ( mac_power_control_params->alpha != scell_power_control_params->alpha )
       || ( mac_power_control_params->n_avg_i != scell_power_control_params->n_avg_i )
       || ( mac_power_control_params->pb != scell_power_control_params->pb )
       || ( mac_power_control_params->pc_meas_chan != scell_power_control_params->pc_meas_chan )
       || ( mac_power_control_params->t_avg_t != scell_power_control_params->t_avg_t )
       || ( mac_power_control_params->t_avg_w != scell_power_control_params->t_avg_w ) )
  {
    rrps_store_update_data_t *rrps_store_update_ptr = rr_get_rrps_store_update(gas_id);

    /* Update global system information */
    (* scell_power_control_params) = (* mac_power_control_params);

    /* Update L1 with the changed parameters */
    rr_gprs_update_l1_params(gas_id);

    if (rrps_store_update_ptr != NULL)
    {
      /* Update the Public Store copy */
      rrps_store_update_ptr->power_control_params = (* mac_power_control_params);

      rr_write_public_store(
        RRPS_UPD_POWER_CTRL,      // update_bitmap
        rrps_store_update_ptr,    // data
        gas_id                    // gas_id
      );

      rr_free_rrps_store_update(
        rrps_store_update_ptr,     // ptr
        gas_id                     // gas_id
      );
    }
  }

} /* end rr_gprs_updated_sys_info_params_from_mac() */

/*!
 * \brief Called when a PSI message is received.
 *
 * \param header (in)     - Indication of the message type
 * \param data (in)       - PSI message data
 * \param bcch_arfcn (in) - Channel received on
 * \param gas_id (in)
 */
void rr_gprs_new_psi(
  rlc_mac_cntrl_t *header,
  uint8 *data,
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(header);

  if (header->error_code != CSN_DECODE_NO_ERROR)
  {
    MSG_GERAN_MED_1_G("CSN decode error reported (%d)",header->error_code);
  }
  else
  {
    // The following PSIs can be received on the PACCH only and are always processed

    switch (header->message_type)
    {
      case PACKET_PSI13:

        process_psi13((packet_psi13_t *)data, bcch_arfcn, gas_id);
        break;

      case PACKET_PSI14:

        process_psi14((packet_psi14_t *)data, bcch_arfcn, gas_id);
        break;

      default:
        break;
    }
  }

} /* rr_gprs_new_psi() */

/*!
 * \brief Converts PSI14 to PSI13 by converting the data format to that of PSI3.
 *
 * \param psi14_ptr (in) - Pointer to PSI14 message (as output from the CSN decoder).
 * \param si13_ptr (out) - Updated with PSI13 message created from PSI14.
 * \param gas_id (in)
 *
 * \return boolean - TRUE if successful; FALSE otherwise.
 */
boolean rr_gprs_convert_psi14_to_si13_data(
  packet_psi14_t *psi14_ptr,
  si13_data_t *si13_ptr
)
{
  boolean converted = TRUE;

  if ((psi14_ptr == NULL) || (si13_ptr == NULL))
  {
    MSG_GERAN_ERROR_2("Invalid ptrs for PSI14 0x%08X 0x%08X",(int)psi14_ptr,(int)si13_ptr);
    return(FALSE);
  }

  memset(si13_ptr, 0, sizeof(si13_data_t));

  si13_ptr->si_change_field = 0xFF; /* update of unknown SI (no messages) */

  if (psi14_ptr->ccch_pbcch_flag == CCCH_ACCESS_INFO)
  {
    ccch_access_info_t *ccch_ptr = &(psi14_ptr->ccch_or_pbcch.ccch_access_info);

    /* START CONVERSION OF CCCH ACCESS INFO */
    if (ccch_ptr->gprs_cell_options.nmo == NMO_RESERVED)
    {
      MSG_GERAN_ERROR_0("PSI14: reserved NMO value");
      converted = FALSE;
    }
    else
    {
      si13_ptr->bcch_change_mark = ccch_ptr->bcch_chng_mrk;
      si13_ptr->sgsnr            = (sgsnr_t)ccch_ptr->sgsnr;

      if (psi14_ptr->dtm_additions_flag == valid)
      {
        si13_ptr->bcch_pbcch.bcch.rac = psi14_ptr->ccch_or_pbcch.ccch_access_info.rac;
        si13_ptr->si_status_ind       = psi14_ptr->ccch_or_pbcch.ccch_access_info.si_status_ind;
      }
      si13_ptr->bcch_pbcch.bcch.spgc_ccch_sup         = ccch_ptr->spgc_ccch_sup;
      si13_ptr->bcch_pbcch.bcch.priority_access_thr   = ccch_ptr->priority_access_thr;
      si13_ptr->bcch_pbcch.bcch.network_control_order = ccch_ptr->network_ctrl_ordr;

      si13_ptr->bcch_pbcch.bcch.gprs_cell_options = ccch_ptr->gprs_cell_options;

      si13_ptr->bcch_pbcch.bcch.power_control_params.alpha        = ccch_ptr->alpha;
      si13_ptr->bcch_pbcch.bcch.power_control_params.t_avg_w      = ccch_ptr->t_avg_w;
      si13_ptr->bcch_pbcch.bcch.power_control_params.t_avg_t      = ccch_ptr->t_avg_t;
      si13_ptr->bcch_pbcch.bcch.power_control_params.pc_meas_chan = ccch_ptr->pc_meas_chan;
      si13_ptr->bcch_pbcch.bcch.power_control_params.n_avg_i      = ccch_ptr->n_avg_i;

      if (ccch_ptr->lb_ms_txpwr_max_cch == INVALID_LB_MS_TXPWR_MAX_CCH)
      {
        si13_ptr->LB_MS_TXPWR_MAX_CCH_valid = FALSE;
        si13_ptr->LB_MS_TXPWR_MAX_CCH = 0;
      }
      else
      {
        si13_ptr->LB_MS_TXPWR_MAX_CCH_valid = TRUE;
        si13_ptr->LB_MS_TXPWR_MAX_CCH = ccch_ptr->lb_ms_txpwr_max_cch;
      }

      if (ccch_ptr->si13_cm_gprs_ma_flag)
      {
        si13_ptr->si13_change_mark = ccch_ptr->si13_chng_mrk;

        si13_ptr->mobile_allocation_status = IE_PRESENT;

        si13_ptr->mobile_allocation_ie.hsn                  = ccch_ptr->gprs_mobile_alloc.hsn;
        si13_ptr->mobile_allocation_ie.num_of_rfl_numbers   = ccch_ptr->gprs_mobile_alloc.size_of_rfl_list;
        si13_ptr->mobile_allocation_ie.num_of_arfcn_indexes = ccch_ptr->gprs_mobile_alloc.size_of_arfcn_list;
              //Replace memcpy with memscpy
		memscpy(si13_ptr->mobile_allocation_ie.rfl_numbers,
			    sizeof(si13_ptr->mobile_allocation_ie.rfl_numbers),
			    ccch_ptr->gprs_mobile_alloc.rfl_number_list, 
			    sizeof(uint8)*MAX_NO_RFL_ELEMENTS);
			  
    	  memscpy(si13_ptr->mobile_allocation_ie.arfcn_indexes,
					  sizeof(si13_ptr->mobile_allocation_ie.arfcn_indexes),
					  ccch_ptr->gprs_mobile_alloc.arfcn_index_list, 
					  sizeof(uint8)*MAX_NO_ARFCN_ELEMENTS);

        si13_ptr->mobile_allocation_ie.ma_bitmap            = ccch_ptr->gprs_mobile_alloc.ma_bitmap;
        if (ccch_ptr->gprs_mobile_alloc.ma_arfcn_flag == ma_list_valid)
        {
          si13_ptr->mobile_allocation_ie.ma_bitmap_valid = TRUE;
        }
        else
        {
          si13_ptr->mobile_allocation_ie.ma_bitmap_valid = FALSE;
        }
      }
      else
      {
        si13_ptr->mobile_allocation_status = IE_NOT_PRESENT;
      }
    }
  }

  return(converted);
}

/*!
 * \brief Routes BCCH sys info from the PACCH to RR modules for processing (e.g. NACC, EDTM).
 *
 * \param mac_data_ind_ptr (in) - Pointer to the MAC_GRR_DATA_IND message (as output from the CSN decoder).
 * \param gas_id (in)
 */
void rr_gprs_process_sys_info_from_pacch(
  mac_grr_data_ind_t *mac_data_ind_ptr,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(mac_data_ind_ptr);

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  if (rr_in_dedicated(gas_id))
  {
    rr_gprs_edtm_process_pacch_sys_info(mac_data_ind_ptr, gas_id);
  }
  else
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  {
    /* for NACC, notify reception of BCCH messages, for use in Pkt SI Status later */

    uint8 msg_type = mac_data_ind_ptr->data_ptr.header_ptr->message_type;
    uint8 *msg_ptr = mac_data_ind_ptr->data_ptr.data_ptr;

    rr_gprs_check_status_resend_timers(msg_type, gas_id);
    rr_gprs_nacc_notify_bcch_msg_received(msg_ptr, msg_type, gas_id);
  }

  /* for SIs received on the PACCH, extract the data and route it into RR */
  rr_gprs_extract_si_data(mac_data_ind_ptr, gas_id);

  return;
}

/*!
 * \brief Send Pkt SI Status requesting N/W to send any outstanding SIs.
 *
 *  On entering pkt transfer check if it's applicable to send Pkt SI status requesting N/W to send any
 *  outstanding SI's that the MS requires via the PACCH of the active TBF.
 *  See 44.060 5.5.1.3.4.
 *
 * \param mac_status (in) - Current MAC status (as received in MAC_GRR_MAC_STATUS)
 * \param gas_id (in)
 */
void rr_gprs_send_si_status_during_si_acquisition(
  rr_mac_status_t mac_status,
  const gas_id_t gas_id
)
{
  gprs_system_info_database_t *gprs_si_database_ptr = rr_get_gprs_system_info_database_ptr(gas_id);

  /******************************************************************
  *  Must be camped on S cell but check, exit if not                *
  *******************************************************************/
  if (gprs_si_database_ptr->scell.gsm.camped_on_flag == FALSE)
  {
    return;
  }

  /******************************************************************
  *  IF MAC not in transfer don't send SI status exit               *
  *******************************************************************/
  if ((mac_status == MAC_IDLE) || (mac_status == MAC_IDLE_BLOCKED))
  {
    MSG_GERAN_MED_1_G("MAC not in transfer: current mac Status = %d",mac_status);
    return;
  }

  /******************************************************************
  *  IF SI status NOT supported on Scell don't send SI status. Exit *
  *******************************************************************/
  if ((gprs_si_database_ptr->scell.bcch_sys_info.si_status_ind) == FALSE)
  {
    MSG_GERAN_MED_0_G("SI status NOT supported on cell");
    return;
  }

  /******************************************************************
  *  If NO missing SI's from the required SI set exit.              *
  *******************************************************************/
  if (gprs_si_database_ptr->scell.bcch_sys_info.si_messages_required == 0)
  {
    MSG_GERAN_MED_0_G("NO missing SI's from the required SI set");
    return;
  }

  /*********************************************************************
  *  Check SI timers,if running & MAC is in transfer then UE is waiting*
  *  for a response to a previously transmitted SI  status. Don't  send*
  *  SI Status. On timer expiry if in transfer a subsequent SI status  *
  *  will be sent                                                      *
  *********************************************************************/

  if (mac_status == MAC_TRANSFER)
  {
    if (rr_timer_is_running(GRR_SYS_INFO_STATUS_TIMER_1, gas_id))
    {
      MSG_GERAN_MED_0_G("GRR_SYS_INFO_STATUS_TIMER_1 is running");
      return;
    }

    if (rr_timer_is_running(GRR_SYS_INFO_STATUS_TIMER_2, gas_id))
    {
      MSG_GERAN_MED_0_G("GRR_SYS_INFO_STATUS_TIMER_2 is running");
      return;
    }
  }

  /******************************************************************
  *  If NACC prodeure currently active , don't send SI status as    *
  *  NACC procedure will handle this                                *
  *******************************************************************/
  if (rr_gprs_is_nacc_active(gas_id) == TRUE)
  {
    MSG_GERAN_MED_0_G("NACC procedure still active");
    return;
  }

  /******************************************************************
  *  All trigger criteria met request sending of SI status.         *
  *******************************************************************/
  MSG_GERAN_MED_3_G("si_status_ind = %d required Si's = 0x%x rcvd Si = 0x%x",
             gprs_si_database_ptr->scell.bcch_sys_info.si_status_ind,
             gprs_si_database_ptr->scell.bcch_sys_info.si_messages_required,
             gprs_si_database_ptr->scell.gsm.update_message_received);


  /* reset the defaults for a single sys info status message */
  rr_gprs_set_sysinfo_status_params(
    MASK_SI_NONE,   // bsi_mask
    FALSE,          // ps_release
    FALSE,          // enh_priority
    gas_id          // gas_id
  );

  /* Send Pkt SI Status */
  MSG_GERAN_HIGH_0_G("Send Pkt PSI Status to acquire required SI's during SI acquisition");
  (void) rr_gprs_send_sysinfo_status(
    GRR_TIMEOUT_1_SECOND,   // resend_timeout
    gas_id                  // gas_id
  );

}

/*!
 * \brief Cancels the SI STATUS timers if GMAC is not in Packet Transfer.
 *
 * \param mac_status (in) - Current MAC status (as received in MAC_GRR_MAC_STATUS)
 * \param gas_id (in)
 */
void rr_gprs_reset_si_status_timers(
  rr_mac_status_t mac_status,
  const gas_id_t gas_id
)
{
  if ((mac_status == MAC_IDLE) || (mac_status == MAC_IDLE_BLOCKED))
  {
    if (rr_timer_cancel(GRR_SYS_INFO_STATUS_TIMER_1, gas_id))
    {
      MSG_GERAN_MED_0_G("Exited pkt transfer stopped GRR_SYS_INFO_STATUS_TIMER_1");
    }

    if (rr_timer_cancel(GRR_SYS_INFO_STATUS_TIMER_2, gas_id))
    {
      MSG_GERAN_MED_0_G("Exited pkt transfer stopped GRR_SYS_INFO_STATUS_TIMER_2");
    }
  }
}

/*!
 * \brief Sends Pkt SI Status if SI13 indicates that BCCH messages have changed during transfer mode.
 *
 *  Sends Pkt SI Status if SI13 indicates that BCCH messages have changed during transfer mode.
 *  Note: Must NOT be during GCF testing, as testcases may not expect the additional RLC/MAC control message
 *  on the uplink PACCH.
 *
 * \param scell_ptr (in) - serving cell info
 * \param gas_id (in)
 */
void rr_send_sysinfo_status_on_bcch_change_mark_update(
  gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  if ((rr_macinfo_get_mac_status(gas_id)            == MAC_TRANSFER) && /* GPRS transfer in progress */
      (rr_get_nv_anite_gcf_flag(RR_GAS_ID_TO_AS_ID) == FALSE) && /* GCF flag is turned OFF */
      (scell_ptr                                    != NULL))
  {
    /* reset the defaults for a single sys info status message */
    rr_gprs_set_sysinfo_status_params(
      MASK_SI_NONE,   // bsi_mask
      FALSE,          // ps_release
      FALSE,          // enh_priority
      gas_id          // gas_id
    );

    /* Send Pkt SI Status */
    MSG_GERAN_HIGH_0_G("BCCH sys info change during TBF: send Pkt SI Status");
    (void) rr_gprs_send_sysinfo_status(
      0,        // resend_timeout
      gas_id    // gas_id
    );
  }

  return;
}

/*!
 * \brief Called when either of the two sys info status timers has expired.
 *
 *  Called when either of the two sys info status timers has expired. Attempts a retransmission of the last
 *  status message if required. Further sendings shall take place at the first suitable opportunity at least
 *  2 secs after the message was sent the previous time.
 *
 * \param timer_id (in)
 * \param gas_id (in)
 *
 * \return rr_event_T
 */
rr_event_T rr_gprs_sysinfo_status_timer_expired(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_event_T                rr_event = EV_NO_EVENT;
  boolean                   bStartSecondary;
  rr_sys_info_pacch_data_t *rr_sys_info_pacch_data_ptr;

  if ((timer_id != GRR_SYS_INFO_STATUS_TIMER_1) &&
      (timer_id != GRR_SYS_INFO_STATUS_TIMER_2))
  {
    MSG_GERAN_ERROR_1_G("Unknown sysinfo status timer ID %d", (int)timer_id);
    return(rr_event);
  }

  // Obtain a pointer to the module data
  rr_sys_info_pacch_data_ptr = rr_sys_info_pacch_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_pacch_data_ptr);

  /* explicitly clear the expired timer */
  (void)rr_timer_cancel(timer_id, gas_id);

  if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
    /* GMAC is in transfer, so a retransmission is possible.  */
    /* The secondary timer may now be started, but only if    */
    /* the current cell still supports the status procedures. */
    bStartSecondary = rr_gprs_send_sysinfo_status(
      0,        // resend_timeout
      gas_id    // gas_id
    );
  }
  else
  {
    /* GMAC is not in transfer, so no opportunity to retransmit.  */
    /* Start the secondary timer and check again when it expires. */
    bStartSecondary = TRUE;
  }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  if (rr_gprs_use_sysinfo_status_enhanced_priority(gas_id))
  {
    /* if the enhanced priority flag is set, it implies EDTM activity, */
    /* and in this case, the secondary timer is controlled externally. */
    bStartSecondary = FALSE;
  }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

  if (bStartSecondary == TRUE)
  {
    if (timer_id == GRR_SYS_INFO_STATUS_TIMER_1)
    {
      /* set counter to indicate how many further retransmissions are allowed */
      rr_sys_info_pacch_data_ptr->num_sysinfo_status_retrans = MAX_NUMBER_SYSINFO_STATUS_RETRANS;

      /* note that this counter is immediately decremented again below */
    }

    if (rr_sys_info_pacch_data_ptr->num_sysinfo_status_retrans > 0)
    {
      /* the first timer has expired, so start the secondary timer */
      (void)rr_timer_start(GRR_SYS_INFO_STATUS_TIMER_2, GRR_TIMEOUT_2_SECONDS, gas_id);

      /* decrement the attempt counter */
      rr_sys_info_pacch_data_ptr->num_sysinfo_status_retrans--;
    }
  }

  return(rr_event);
}

/*!
 * \brief Sets the SI masks and PS_REL_REQ flags for the next SI Status.
 *
 * \param bsi_mask (in)     - Mask of SIs that should be marked as received
 * \param ps_release (in)   - Indicate whether PS_REL_REQ should be set
 * \param enh_priority (in) - Indicate use of enhanced priority order (EDTM)
 * \param gas_id (in)
 */
void rr_gprs_set_sysinfo_status_params(
  word bsi_mask,
  boolean ps_release,
  boolean enh_priority,
  const gas_id_t gas_id
)
{
  rr_sys_info_pacch_data_t *rr_sys_info_pacch_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_pacch_data_ptr = rr_sys_info_pacch_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_pacch_data_ptr);

  if (rr_sys_info_pacch_data_ptr->sysinfo_status.ps_release != ps_release)
  {
    MSG_GERAN_MED_1_G("Set PS_REL_REQ to %d", (int)ps_release);
  }

  rr_sys_info_pacch_data_ptr->sysinfo_status.bsi_mask          = bsi_mask;
  rr_sys_info_pacch_data_ptr->sysinfo_status.ps_release        = ps_release;
  rr_sys_info_pacch_data_ptr->sysinfo_status.enhanced_priority = enh_priority;

  return;
}

/*!
 * \brief Retrieves the current SI Status mask and PS release flag.
 *
 * \param bsi_mask_ptr (out) - SI mask
 * \param gas_id (in)
 *
 * \return boolean - Current value of PS_REL_REQ parameter
 */
boolean rr_gprs_get_sysinfo_status_params(
  word *bsi_mask_ptr,
  const gas_id_t gas_id
)
{
  rr_sys_info_pacch_data_t *rr_sys_info_pacch_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_pacch_data_ptr = rr_sys_info_pacch_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_pacch_data_ptr);

  if (bsi_mask_ptr != NULL)
  {
    *bsi_mask_ptr = rr_sys_info_pacch_data_ptr->sysinfo_status.bsi_mask;
  }

  return(rr_sys_info_pacch_data_ptr->sysinfo_status.ps_release);
}

/*!
 * \brief Determines whether the sys info status enhanced priority should be used.
 *
 * \param gas_id (in)
 *
 * \return boolean - Current value of enhanced priority parameter
 */
boolean rr_gprs_use_sysinfo_status_enhanced_priority(
  const gas_id_t gas_id
)
{
  rr_sys_info_pacch_data_t *rr_sys_info_pacch_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_pacch_data_ptr = rr_sys_info_pacch_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_pacch_data_ptr);

  return(rr_sys_info_pacch_data_ptr->sysinfo_status.enhanced_priority);
}

/*!
 * \brief Creates a Pkt SI Status msg, and if the NW supports it, sends it
 *
 * \param resend_timeout (in) - Duration of resend timer (0 == should not be started)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the NW supports the sending of sys info status
 */
boolean rr_gprs_send_sysinfo_status(
  uint16 resend_timeout,
  const gas_id_t gas_id
)
{
  boolean bSent = FALSE;

  packet_si_status_t si_status;

  /* determine whether SI Status procedures are supported on the cell */
  if (rr_gprs_initialise_si_status(&si_status, gas_id))
  {
    word bsi_mask;

    /* retrieve a mask of the SIs that should be reported */
    bsi_mask = rr_gprs_retrieve_si_status_mask(gas_id);

    /* use the mask to generate a format suitable for the CSN encoder */
    rr_gprs_generate_si_status_from_mask(
      bsi_mask,     // si_mask
      &si_status,   // si_status_ptr
      gas_id        // gas_id
    );

    /* ask MAC to encode and transmit the message on the PACCH */
    rr_send_grr_mac_send_pss_req(
      &si_status,   // si_status_ptr
      gas_id        // gas_id
    );

    bSent = TRUE;
  }

  if ((bSent == TRUE) && (resend_timeout != 0))
  {
    /* start the first retransmission timer with the specified timeout */
    (void)rr_timer_start(GRR_SYS_INFO_STATUS_TIMER_1, (dword)resend_timeout, gas_id);
  }

  return(bSent);
}

/*!
 * \brief Initialisation function.
 *
 * \param gas_id (in)
 */
void rr_sys_info_pacch_init(
  const gas_id_t gas_id
)
{
  rr_sys_info_pacch_data_t *rr_sys_info_pacch_data_ptr;

  // Obtain a pointer to the module data
  rr_sys_info_pacch_data_ptr = rr_sys_info_pacch_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_pacch_data_ptr);

  // Zero the control/data structure(s)
  memset(rr_sys_info_pacch_data_ptr, 0, sizeof(rr_sys_info_pacch_data_t));

  // Setup any special values
  rr_sys_info_pacch_data_ptr->num_sysinfo_status_retrans = MAX_NUMBER_SYSINFO_STATUS_RETRANS;
}

/*!
 * \brief Initialisation function called during RR task start.
 */
void rr_sys_info_pacch_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_sys_info_pacch_data, 0, sizeof(rr_sys_info_pacch_data));
}

/* EOF */