/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_edtm.c#1 $
***
*** DESCRIPTION
***
***  Contains Enhanced DTM functionality
***
*** Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"  /* Includes FEATURE_... defines */
#if defined(FEATURE_GSM_EDTM) && defined(FEATURE_GSM_DTM)

#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#include "rr_control.h"
#include "rr_conn_establish.h"
#include "rr_conn_release.h"
#include "gprs_mem.h"
#include "ms.h"
#include "rr_log.h"
#include "rr_l2_send.h"
#include "rr_mac_send.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_init.h"
#include "rr_sys_info_i.h"
#include "rr_gprs_edtm.h"
#include "rr_gprs_conn_establish.h"
#include "rr_cipher.h"
#include "rr_sys_info_si.h"
#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif /* FEATURE_GPRS_PS_HANDOVER */
#include "rr_decode_ie.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_sys_info_pacch.h"
#include "rr_ps_domain.h"
#include "rr_nv.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "geran_multi_sim_api.h"

#define INVALID_L3_MSG_TYPE        0xFF
#define EDTM_MIN_LENGTH_IA_MSG       10
#define EDTM_MIN_LENGTH_IAREJ_MSG    19
#define EDTM_IAREJ_T3122_OCTET        7
#define EDTM_IA_DMT_OFFSET            3

#define EDTM_FULL_BSI_SET (MASK_SI_1  | MASK_SI_3  | MASK_SI_13)

#define EDTM_INVALID_ESTABLISHMENT_CAUSE(cause) ((cause) == (byte)MM_RR_ANSWER_TO_PAGING)

#define EDTM_INVALID_PLMN_ID_ARRAY {INVALID_PLMN_ID,INVALID_PLMN_ID,INVALID_PLMN_ID}

#define EDTM_INVALID_LAI {{EDTM_INVALID_PLMN_ID_ARRAY},INVALID_LAC_WORD}

  #define EDTM_INVALID_SCELL {INVALID_ARFCN, SYS_BAND_NONE}

typedef enum
{
  EDTM_NULL,
  EDTM_CS_ESTABLISHMENT,
  EDTM_DTM_ESTABLISHMENT
} edtm_control_state_t;

typedef enum
{
  RESTORE_AS_SAVED_MSG,
  RESTORE_IN_RR_QUEUE
} edtm_restore_type_t;

typedef enum
{
  EDTM_CELL_TYPE_CCCH = 0,
  EDTM_CELL_TYPE_UNKNOWN
} edtm_scell_type_t;

typedef struct
{
  edtm_control_state_t state;
  byte                *pending_l3_msg_ptr;
  byte                 pending_l3_msg_len;
  byte                 pending_l3_msg_typ;
  byte                 establishment_cause;
  byte                 establishment_msg_len;
  byte                 establishment_message[N201_MAX];
#ifdef FEATURE_LTE
  boolean              is_periodic_lau;
#endif /* FEATURE_LTE */
  byte                 wait_indication;
  struct
  {
    byte                 channel_needed;
    rr_identity_type_e   identity_type;
    byte                 identity[RR_MAX_IDENTITY_SIZE];
  }                    cs_page_info;
  boolean              cs_request_sent;
  boolean              gmac_uplink_pending;
  boolean              cs_release_started;
  boolean              final_status_sent;
  word                 received_si_mask;
  boolean              ps_release_ind;
  boolean              l1_ps_release;
  boolean              l1_cs_release;
  boolean              l1_error_code;
  LAI_T                scell_LAI;
  ARFCN_T              scell_ARFCN;
  edtm_scell_type_t    scell_type;
  uint16               received_psi_mask;
} edtm_data_t;

#ifdef FEATURE_LTE

#define EDTM_DATA_INIT { EDTM_NULL,\
                         NULL,\
                         0,\
                         INVALID_L3_MSG_TYPE,\
                         (byte)MM_RR_ANSWER_TO_PAGING,\
                         0,\
                         {0},\
                         FALSE,\
                         0,\
                         {ANY_CHANNEL,RR_IDENTITY_TYPE_NONE},\
                         FALSE,\
                         FALSE,\
                         FALSE,\
                         FALSE,\
                         0x0000,\
                         TRUE,\
                         TRUE,\
                         TRUE,\
                         TRUE,\
                         EDTM_INVALID_LAI,\
                         EDTM_INVALID_SCELL,\
                         EDTM_CELL_TYPE_UNKNOWN,\
                         0x0000 \
                        }
#else

#define EDTM_DATA_INIT { EDTM_NULL,\
                         NULL,\
                         0,\
                         INVALID_L3_MSG_TYPE,\
                         (byte)MM_RR_ANSWER_TO_PAGING,\
                         0,\
                         {0},\
                         0,\
                         {ANY_CHANNEL,RR_IDENTITY_TYPE_NONE},\
                         FALSE,\
                         FALSE,\
                         FALSE,\
                         FALSE,\
                         0x0000,\
                         TRUE,\
                         TRUE,\
                         TRUE,\
                         TRUE,\
                         EDTM_INVALID_LAI,\
                         EDTM_INVALID_SCELL,\
                         EDTM_CELL_TYPE_UNKNOWN,\
                         0x0000 \
                        }

#endif /* FEATURE_LTE */



static edtm_data_t rr_gprs_edtm_data[NUM_GERAN_DATA_SPACES] =
            { INITIAL_VALUE(EDTM_DATA_INIT) };


#define EDTM_STATE_TRANSITION(vs,cs,gi) if ((vs) != (cs))\
{\
  rr_gprs_edtm_log_state_change((vs),(cs),(gi));\
  (vs) = (cs);\
}

#ifdef PERLUTF
#error code not present
#else
#define EDTM_ERROR_DEBUG(st,p1,p2,p3) MSG_ERROR(st,p1,p2,p3)
#endif /* PERLUTF */


/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return edtm_data_t*
 */
static edtm_data_t *rr_gprs_edtm_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_gprs_edtm_data[as_index];
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_reset_est_data
 *   --------------
 *
 *   Description: Reset local data used for Enhanced CS Establishment
 *
 *   Parameters:  full_reset - TRUE if all the data should be reset
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_reset_est_data(
  boolean full_reset,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_gprs_edtm_data_ptr->pending_l3_msg_ptr);
  }
  rr_gprs_edtm_data_ptr->pending_l3_msg_ptr = NULL;
  rr_gprs_edtm_data_ptr->pending_l3_msg_len = 0;
  rr_gprs_edtm_data_ptr->pending_l3_msg_typ = INVALID_L3_MSG_TYPE;

  if (rr_timer_cancel(T3196, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped EDTM guard timer");
  }

  if (full_reset == TRUE)
  {
    rr_gprs_edtm_data_ptr->establishment_cause   = (byte)MM_RR_ANSWER_TO_PAGING;
    rr_gprs_edtm_data_ptr->establishment_msg_len = 0;
#ifdef FEATURE_LTE
    rr_gprs_edtm_data_ptr->is_periodic_lau       = FALSE;
#endif /* FEATURE_LTE */
    rr_gprs_edtm_data_ptr->wait_indication       = 0;
    rr_gprs_edtm_data_ptr->cs_page_info.channel_needed = ANY_CHANNEL;
    rr_gprs_edtm_data_ptr->cs_page_info.identity_type  = RR_IDENTITY_TYPE_NONE;
    memset(rr_gprs_edtm_data_ptr->cs_page_info.identity, 0, RR_MAX_IDENTITY_SIZE);
    rr_gprs_edtm_data_ptr->cs_request_sent       = FALSE;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_reset_rel_data
 *   --------------
 *
 *   Description: Reset local data used for Enhanced CS Release
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_reset_rel_data(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  rr_gprs_edtm_data_ptr->cs_release_started = FALSE;
  rr_gprs_edtm_data_ptr->final_status_sent  = FALSE;

  rr_gprs_edtm_data_ptr->received_si_mask   = 0x0000;
  rr_gprs_edtm_data_ptr->ps_release_ind     = TRUE;

  rr_gprs_edtm_data_ptr->l1_ps_release      = TRUE;
  rr_gprs_edtm_data_ptr->l1_cs_release      = TRUE;
  rr_gprs_edtm_data_ptr->l1_error_code      = TRUE;

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_store_cell_parameters
 *   --------------
 *
 *   Description: Stores the details of the current serving cell and LAI
 *                when dedicated mode is started.
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_store_cell_parameters(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_ptr;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  scell_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_ptr);

  /* cache the current Location Area Info */
  rr_gprs_edtm_data_ptr->scell_LAI = scell_ptr->gsm.location_area_identification;

  /* cache the serving ARFCN */
  rr_gprs_edtm_data_ptr->scell_ARFCN = scell_ptr->gsm.BCCH_ARFCN;

  MSG_GERAN_HIGH_3_G("EDTM cell info: ARFCN=%d, LAC=%d",
           (int)rr_gprs_edtm_data_ptr->scell_ARFCN.num,
           (int)rr_gprs_edtm_data_ptr->scell_LAI.location_area_code,
           0);

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_available_in_cell
 *   --------------
 *
 *   Description: Determines whether the current serving cell supports E-DTM
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if sys info has indicated support for E-DTM
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_available_in_cell(
  const gas_id_t gas_id
)
{
  gprs_cell_options_t *gco_ptr = rr_si_get_gprs_cell_options(gas_id);

  if ((gco_ptr != NULL) &&
      (gco_ptr->optional_extension_info_flag == valid) &&
      (gco_ptr->optional_extension_info.dtm_support == TRUE) &&
      (gco_ptr->optional_extension_info.dtm_enhancements_capability == TRUE))
  {
    return(TRUE);
  }

  return(FALSE);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_supported
 *   --------------
 *
 *   Description: Determines whether E-DTM can be used by the UE
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if and only if
 *   --------       1) sys info has indicated support for E-DTM, AND
 *                  2) UE has enabled NV settings for DTM (3628) and E-DTM (4209)
 *
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_supported(
  const gas_id_t gas_id
)
{
  boolean ue_support;
  boolean nw_support;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  ue_support = rr_get_nv_dtm_enabled(as_id) && rr_get_nv_edtm_enabled(as_id);
  nw_support = rr_gprs_edtm_available_in_cell(gas_id);

  MSG_GERAN_HIGH_2_G("E-DTM support: UE=%1d NW=%1d",(int)ue_support,(int)nw_support);

  return(ue_support && nw_support);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_enhanced_release_allowed
 *   --------------
 *
 *   Description: Determines whether Enhanced CS Release is allowed
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if and only if
 *   --------       1) sys info has indicated support for E-DTM, AND
 *                  2) UE has NV mem setting for E-DTM (item 4209) enabled
 *                  3) UE is in established dual transfer mode
 *
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_enhanced_release_allowed(
  const gas_id_t gas_id
)
{
  boolean bAllowed;

  if ((rr_macinfo_get_mac_status(gas_id)     == MAC_TRANSFER) &&
      (rr_mac_ps_access_granted(gas_id)      == TRUE )        &&
      (rr_in_dedicated(gas_id)               == TRUE)         &&
      (rr_power_scan_on_edtm_release(gas_id) == FALSE)        &&
      (rr_gprs_edtm_supported(gas_id)        == TRUE))
  {
    MSG_GERAN_MED_0_G("Dual Transfer Mode in progress, scell OK, attempt EDTM release");
    bAllowed = TRUE;
  }
  else
  {
    bAllowed = FALSE;
  }

  return(bAllowed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_lai_has_changed
 *   --------------
 *
 *   Description: Compares the current Location Area Information against the
 *                details cached when dedicated mode or DTM was last initiated.
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if the LAI has changed
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_lai_has_changed(
  const gas_id_t gas_id
)
{
  LAI_T             *new_LAI_ptr = NULL;
  LAI_T             *old_LAI_ptr;
  boolean            has_changed;
  gprs_scell_info_t *scell_info_ptr;
  edtm_data_t       *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  old_LAI_ptr = &(rr_gprs_edtm_data_ptr->scell_LAI);
  RR_NULL_CHECK_FATAL(old_LAI_ptr);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  new_LAI_ptr = &(scell_info_ptr->gsm.location_area_identification);

  /* determine whether the Location Area Info has changed */
  if (rr_same_lai(new_LAI_ptr, old_LAI_ptr))
  {
    has_changed = FALSE;
  }
  else
  {
    has_changed = TRUE;
    MSG_GERAN_MED_2_G("EDTM: Location Area Info has changed (LAC %d->%d)",
            (int)old_LAI_ptr->location_area_code,
            (int)new_LAI_ptr->location_area_code);
  }

  return(has_changed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_set_sysinfo_status_params
 *   --------------
 *
 *   Description: Sets appropriate flags in the (P)SI Status message
 *
 *   Parameters:  pbcch_present - used to determine which GPRS Cell Options to use
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_set_sysinfo_status_params(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t    *scell_ptr;
  network_op_mode_type  nmo;
  boolean               ps_rel_req;

  scell_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_ptr);

  nmo = (network_op_mode_type)scell_ptr->bcch_sys_info.gprs_cell_options.nmo;

  /* Latest spec revisions state that if LAI has changed, and NMOI is in effect, */
  /* the UE should set the PS_REL_REQ flag to indicate PS release is required.   */
  if ((rr_gprs_edtm_lai_has_changed(gas_id) == TRUE) && (nmo == NETWORK_OP_MODE_1))
  {
    ps_rel_req = TRUE;
  }
  else
  {
    MSG_GERAN_MED_1_G("PS_REL_REQ=0 (NMO=%1d", (int)nmo+1);
    ps_rel_req = FALSE;
  }

  /* set SI Status parameters: extra SI masks, PS_REL_REQ & EDTM priority */
  rr_gprs_set_sysinfo_status_params(
    0x0000,         // bsi_mask
    ps_rel_req,     // ps_release
    TRUE,           // enh_priority
    gas_id          // gas_id
  );

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_initiate_enhanced_release
 *   --------------
 *
 *   Description: Initiates the E-DTM CS Release procedure
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if the Pkt SI/PSI Status has been sent on the cell
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_initiate_enhanced_release(
  const gas_id_t gas_id
)
{
  boolean release_started;

  if (rr_gprs_edtm_enhanced_release_allowed(gas_id))
  {
    /* set SI Status parameters */
    rr_gprs_edtm_set_sysinfo_status_params(gas_id);

    /* initiate the enhanced release by sending the first */
    /* SI Status, and starting a retransmission timer */
    release_started = rr_gprs_send_sysinfo_status(EDTM_STATUS_TIMEOUT, gas_id);
  }
  else
  {
    release_started = FALSE;
  }

  return(release_started);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_allowed_by_gmac
 *   --------------
 *
 *   Description: Determines whether GMAC can perform CS establishment
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if GMAC is likely to be ready for a Pkt CS Request
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_allowed_by_gmac(
  const gas_id_t gas_id
)
{
  boolean bAllowed = FALSE;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_mac_ps_access_granted(gas_id))
  {
    rr_mac_status_t mac_status = rr_macinfo_get_mac_status(gas_id);

    if (mac_status == MAC_TRANSFER)
    {
      rr_gprs_edtm_data_ptr->gmac_uplink_pending = FALSE;
      bAllowed = TRUE;
    }
    else if (mac_status == MAC_IDLE_BLOCKED)
    {
      /* GMAC may be performing establishment via GRR */
      /* in which case the Pkt CS Request may be sent */
      /* once contention resolution has completed.    */
      bAllowed = rr_gprs_edtm_data_ptr->gmac_uplink_pending;
    }
  }

  return(bAllowed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_enhanced_establishment_allowed
 *   --------------
 *
 *   Description: Determines whether Enhanced CS establishment is allowed
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if and only if
 *   --------       1) sys info has indicated support for E-DTM, AND
 *                  2) UE has NV mem setting for E-DTM (item 4209) enabled
 *                  3) UE is in established packet transfer
 *
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_enhanced_establishment_allowed(
  const gas_id_t gas_id
)
{
  boolean bAllowed;

  if ((rr_gprs_edtm_allowed_by_gmac(gas_id) == TRUE) &&
      (rr_gprs_edtm_supported(gas_id)       == TRUE))
  {
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (rr_psho_in_progress(gas_id) == TRUE)
    {
      MSG_GERAN_ERROR_0_G("EDTM establishment blocked by PSHO");
      bAllowed = FALSE;
    }
    else
#endif /* FEATURE_GPRS_PS_HANDOVER */
    {
      bAllowed = TRUE;
    }
  }
  else
  {
    bAllowed = FALSE;
  }

  return(bAllowed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_initiate_enhanced_establishment
 *   --------------
 *
 *   Description: Initiates the E-DTM CS Establishment procedure
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     TRUE if the enhanced establishment procedure was started
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_initiate_enhanced_establishment(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_enhanced_establishment_allowed(gas_id))
  {
    rr_cmd_bdy_type *rr_cmd_ptr;

    /* reset all the establishment data in this module */
    rr_gprs_edtm_reset_est_data(TRUE, gas_id);

    /* there should be a saved RR_EST_REQ from MM - process it now */
    if (rr_process_saved_message(&rr_cmd_ptr, gas_id) == EV_INPUT_MESSAGE)
    {
      if ((rr_cmd_ptr->message_header.message_set == MS_MM_RR) &&
          (rr_cmd_ptr->message_header.message_id  == (byte)RR_EST_REQ))
      {
        rr_est_req_T *rr_est_req_ptr = &(rr_cmd_ptr->rr_est_req);

        if (EDTM_INVALID_ESTABLISHMENT_CAUSE(rr_est_req_ptr->establishment_cause))
        {
          MSG_GERAN_MED_1_G("EDTM establishment not supported for cause %d",
                  (int)rr_est_req_ptr->establishment_cause);

          /* do not use EDTM establishment for this call type, */
          /* just restore the saved RR_EST_REQ, and let normal */
          /* GPRS suspension and CS establishment take place.  */
          (void)rr_save_msg_for_edtm(rr_cmd_ptr, gas_id);

          return(FALSE);
        }

        /* use the RR connection establishment module to check access classes */
        /* this function also stores the 1st message and indicates its length */
        if (rr_ce_process_est_req(
              rr_est_req_ptr,
              &(rr_gprs_edtm_data_ptr->establishment_msg_len),
              gas_id
            ) == EV_INIT_ACCESS_PROCEDURE)
        {
          byte access_ref;

          /* extract establishment cause and message from the MM request */
          rr_gprs_edtm_data_ptr->establishment_cause = rr_est_req_ptr->establishment_cause;
          /*memcpy(rr_gprs_edtm_data_ptr->establishment_message,
                 rr_est_req_ptr->layer3_message,
                 rr_gprs_edtm_data_ptr->establishment_msg_len);*/
      //Replace memcpy with memscpy
          memscpy(rr_gprs_edtm_data_ptr->establishment_message,
              sizeof(rr_gprs_edtm_data_ptr->establishment_message),
                  rr_est_req_ptr->layer3_message,
                  rr_gprs_edtm_data_ptr->establishment_msg_len);

#ifdef FEATURE_LTE
          rr_gprs_edtm_data_ptr->is_periodic_lau = rr_est_req_ptr->is_periodic_lau;
#endif /* FEATURE_LTE */

          /* convert the establishment cause into an access reference */
          access_ref = rr_ce_generate_access_reference(
            rr_gprs_edtm_data_ptr->establishment_cause,
            NULL,
            gas_id
          );

          /* flag that a request is being sent to the NW */
          rr_gprs_edtm_data_ptr->cs_request_sent = TRUE;

          /* apply veto for GPRS Tuneaway so that the uplink control message does not get pre-empted */
          geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_EDTM, TRUE, geran_map_gas_id_to_nas_id(gas_id));

          MSG_GERAN_MED_1_G("GMAC in transfer (pend=%1d), attempt EDTM establishment",
                  (int)rr_gprs_edtm_data_ptr->gmac_uplink_pending);

          /* start a guard timer to protect against lack of response in GMAC */
          (void)rr_timer_start(T3196, T3196_GUARD_TIMEOUT, gas_id);

          /* and then ask GMAC to transmit the Pkt CS Request */
          rr_send_grr_mac_edtm_pkt_cs_est_req(access_ref, gas_id);
        }
      }
    }

    return(rr_gprs_edtm_data_ptr->cs_request_sent);
  }

  return(FALSE);
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/****************************************************************************
 *   Function name:  rr_gprs_edtm_check_sys_info_complete
 *   --------------
 *
 *   Description:    Determines whether PACCH sys info is enough to maintain
 *                   packet transfer
 *
 *   Parameters:     msg_format - indicates whether SIs or PSIs are required
 *   -----------
 *
 *   Returns:        TRUE if all the required SIs or PSIs have been received
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_check_sys_info_complete(
  const gas_id_t gas_id
)
{
  boolean bComplete = FALSE;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (ARE_SET(EDTM_FULL_BSI_SET, rr_gprs_edtm_data_ptr->received_si_mask))
  {
    /* sys info is only complete if it is being checked during an EDTM release */
    bComplete = rr_gprs_edtm_data_ptr->cs_release_started;
  }

  return(bComplete);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_send_final_sys_info_status
 *   --------------
 *
 *   Description:    Initiates a SI/PSI status transmission
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_send_final_sys_info_status(
  const gas_id_t gas_id
)
{
  boolean bResendFinal;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* cancel the first retransmission timer if it is running */
  if (rr_timer_cancel(GRR_SYS_INFO_STATUS_TIMER_1, gas_id))
  {
    /* UE has received all the required sys info BEFORE the first retransmission, */
    /* so cancel the initial retransmission timer, and it can then be restarted.  */
    /* This is an attempt to ensure delivery of this "final" status message.      */
    bResendFinal = TRUE;
  }
  else
  {
    bResendFinal = FALSE;
  }

  /* this module may need to allow the normal SI/PSI decoders to complete in this */
  /* thread before sending the final status message, so use the secondary timer   */
  if (rr_gprs_edtm_data_ptr->final_status_sent == FALSE)
  {
    /* set a flag to stop the final status being sent more than once */
    rr_gprs_edtm_data_ptr->final_status_sent = TRUE;

    if (bResendFinal == TRUE)
    {
      (void)rr_timer_start(GRR_SYS_INFO_STATUS_TIMER_1, EDTM_STATUS_TIMEOUT, gas_id);
    }
    MSG_GERAN_HIGH_1_G("EDTM: sys info complete, trigger final status(%1d)", (int)bResendFinal);

    /* start the secondary status timer so that it expires in the next thread */
    (void)rr_timer_start(GRR_SYS_INFO_STATUS_TIMER_2, 1, gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_process_pacch_sys_info
 *   --------------
 *
 *   Description:    Processes SI/PSI messages received on the PACCH in DTM
 *
 *   Parameters:     mac_data_ind_ptr - pointer to GMAC data
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_process_pacch_sys_info(
  mac_grr_data_ind_t *mac_data_ind_ptr,
  const gas_id_t gas_id
)
{
  uint8 pkt_msg_type;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* sanity check the pointers provided */
  if (mac_data_ind_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("WARNING: null pointer passed as MAC->GRR data");
    return;
  }

  if (mac_data_ind_ptr->data_ptr.header_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("WARNING: null pointer passed as CSN header");
    return;
  }

  if (mac_data_ind_ptr->data_ptr.data_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("WARNING: null pointer passed as CSN data");
    return;
  }

  /* extract the RLC/MAC control message type */
  pkt_msg_type = mac_data_ind_ptr->data_ptr.header_ptr->message_type;

  /* SI message are contained in PSCD or PSCS wrappers */
  if ((pkt_msg_type == (uint8)PACKET_SERVING_CELL_SI) ||
      (pkt_msg_type == (uint8)PACKET_SI_DATA))
  {
    uint8 *l3_msg_ptr;
    uint8  message_octet;

    l3_msg_ptr    = mac_data_ind_ptr->data_ptr.data_ptr;
    message_octet = *(l3_msg_ptr+2);

    switch(message_octet & 0x3F)
    {
    case SYSTEM_INFORMATION_TYPE_1:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_1;
      break;

    case SYSTEM_INFORMATION_TYPE_2:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_2;
      break;

    case SYSTEM_INFORMATION_TYPE_2_BIS:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_2_BIS;
      break;

    case SYSTEM_INFORMATION_TYPE_2_TER:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_2_TER;
      break;

    case SYSTEM_INFORMATION_TYPE_3:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_3;
      break;

    case SYSTEM_INFORMATION_TYPE_4:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_4;
      break;

    case SYSTEM_INFORMATION_TYPE_7:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_7;
      break;

    case SYSTEM_INFORMATION_TYPE_8:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_8;
      break;

    case SYSTEM_INFORMATION_TYPE_13:
      rr_gprs_edtm_data_ptr->received_si_mask |= MASK_SI_13;
      break;

    default:
      break;
    }

    MSG_GERAN_HIGH_3_G("EDTM: received SI 0x%02X on PACCH (0x%04X)",
             (int)message_octet,
             (int)rr_gprs_edtm_data_ptr->received_si_mask,
             0);

    if (rr_gprs_edtm_check_sys_info_complete(gas_id))
    {
      rr_gprs_edtm_send_final_sys_info_status(gas_id);
    }
  }
  else if ((pkt_msg_type & 0x30) == PACKET_PSI13)
  {
    /* PSI13 updates SI13 sys info mask */
    rr_gprs_edtm_data_ptr->received_si_mask  |= MASK_SI_13;
  }

  (void)mac_data_ind_ptr; /* for LINT */
  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_retrieve_bsi_status_mask
 *   --------------
 *
 *   Description:    Determines which BCCH sys info messages have been
 *                   received during Dual Transfer Mode
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        16-bit mask of SI messages received
 *   --------
 *
 *
 ****************************************************************************/
word rr_gprs_edtm_retrieve_bsi_status_mask(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  return(rr_gprs_edtm_data_ptr->received_si_mask);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_save_pending_ccch_message
 *   --------------
 *
 *   Description:    Converts the pending L3 message into a DL_UNIT_DATA_IND
 *                   and then saves the message in RR's private store
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        TRUE if the message was stored successfully by RR
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_save_pending_ccch_message(
  const gas_id_t gas_id
)
{
  boolean            msg_saved = FALSE;
  gprs_scell_info_t *scell_ptr;
  edtm_data_t       *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  scell_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
  {
    rr_cmd_bdy_type *rr_cmd_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

    if (rr_cmd_ptr != NULL)
    {
      /* copy the pending CCCH message data into a DL_UNIT_DATA_IND structure */
      rr_cmd_ptr->dl_unit_data_ind.message_header.message_set = MS_RR_L2;
      rr_cmd_ptr->dl_unit_data_ind.message_header.message_id  = (byte)DL_UNIT_DATA_IND;
      PUT_IMH_LEN(sizeof(dl_unit_data_ind_T)-sizeof(IMH_T),&(rr_cmd_ptr->dl_unit_data_ind.message_header));

      rr_cmd_ptr->dl_unit_data_ind.channel_number    = scell_ptr->gsm.BCCH_ARFCN;
      rr_cmd_ptr->dl_unit_data_ind.l2_channel_type   = CCCH;
      rr_cmd_ptr->dl_unit_data_ind.own_paging_block  = FALSE;
      rr_cmd_ptr->dl_unit_data_ind.SAPI              = SAPI0;
      rr_cmd_ptr->dl_unit_data_ind.short_header      = FALSE;
      rr_cmd_ptr->dl_unit_data_ind.tc                = RR_EDTM_TC;

      rr_cmd_ptr->dl_unit_data_ind.l3_message_length = rr_gprs_edtm_data_ptr->pending_l3_msg_len;
    //Replace mecpy with memscpy
      memscpy(rr_cmd_ptr->dl_unit_data_ind.layer3_message,
            sizeof(rr_cmd_ptr->dl_unit_data_ind.layer3_message),
             rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
             rr_gprs_edtm_data_ptr->pending_l3_msg_len);


      /* ask RR to save the new DL_UNIT_DATA_IND for processing later */
      msg_saved = rr_save_msg_for_edtm(rr_cmd_ptr, gas_id);

      GPRS_MEM_FREE(rr_cmd_ptr);
    }
  }

  return(msg_saved);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_restore_rr_est_req
 *   --------------
 *
 *   Description:    Creates a RR_EST_REQ signal using the saved establishment
 *                   details and then stores the message for RR to process
 *
 *   Parameters:     restore_type - specifies whether the message should be saved
 *                   in the normal RR store, or posted back to the RR task queue
 *   -----------
 *
 *   Returns:        TRUE if the message was restored successfully
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_restore_rr_est_req(
  edtm_restore_type_t restore_type,
  const gas_id_t gas_id
)
{
  boolean bRestored = FALSE;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_ce_allow_cs_connection(rr_gprs_edtm_data_ptr->establishment_cause, gas_id))
  {
    rr_cmd_bdy_type *rr_cmd_ptr;
    uint16          msg_len;

    rr_cmd_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

    if (rr_cmd_ptr != NULL)
    {
      /* copy the received establishment message into a RR_EST_REQ structure */
      rr_cmd_ptr->rr_est_req.message_header.message_set = MS_MM_RR;
      rr_cmd_ptr->rr_est_req.message_header.message_id  = (byte)RR_EST_REQ;

      /* header length has to be set to ensure the L3 message length is correct */
      msg_len = (sizeof(rr_est_req_T) - sizeof(IMH_T));

      PUT_IMH_LEN(msg_len, &(rr_cmd_ptr->rr_est_req.message_header));

      rr_cmd_ptr->rr_est_req.establishment_cause = rr_gprs_edtm_data_ptr->establishment_cause;
#ifdef FEATURE_LTE
      rr_cmd_ptr->rr_est_req.is_periodic_lau = rr_gprs_edtm_data_ptr->is_periodic_lau;
#endif /* FEATURE_LTE */

      //Replace mecpy with memscpy
      memscpy(rr_cmd_ptr->rr_est_req.layer3_message,
              sizeof(rr_cmd_ptr->rr_est_req.layer3_message),
             rr_gprs_edtm_data_ptr->establishment_message,
             rr_gprs_edtm_data_ptr->establishment_msg_len);


      memset(rr_gprs_edtm_data_ptr->establishment_message, 0, N201_MAX);

      rr_cmd_ptr->rr_est_req.as_id = geran_map_gas_id_to_nas_id(gas_id);
      rr_cmd_ptr->rr_est_req.l3_message_size = rr_gprs_edtm_data_ptr->establishment_msg_len;

      if (restore_type == RESTORE_AS_SAVED_MSG)
      {
        /* ask RR to save the new RR_EST_REQ for processing later */
        bRestored = rr_save_msg_for_edtm(rr_cmd_ptr, gas_id);
      }
      else
      {
        /* post the message back to the RR task queue */
        bRestored = (gs_send_message(GS_QUEUE_RR, rr_cmd_ptr, TRUE) == GS_SUCCESS);
      }

      GPRS_MEM_FREE(rr_cmd_ptr);
    }
  }

  return(bRestored);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_validate_pending_ccch_message
 *   --------------
 *
 *   Description:    Validates the IA or IAREJ stored in the local buffer
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        TRUE if the normal RR validation succeeds for the message
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_validate_pending_ccch_message(
  const gas_id_t gas_id
)
{
  boolean msg_valid = FALSE;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == IMMEDIATE_ASSIGNMENT)
  {
    ie_descriptor_T *first_IE_ptr;
    RR_cause_T       error_type = RR_NORMAL_EVENT;
    boolean          decoded_ok = FALSE;

    /* parse the Immediate Assignment stored in the pending buffer */
    rr_check_for_protocol_errors(
      rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
      rr_gprs_edtm_data_ptr->pending_l3_msg_len,
      CCCH,
      FALSE, /* not short header format */
      &decoded_ok,
      &error_type,
      gas_id
    );

    /*********************************************************/
    /* get a pointer to the first IE of the buffered message */
    /*********************************************************/
    first_IE_ptr = rr_get_first_ie_of_message(IMMEDIATE_ASSIGNMENT, gas_id);

    if ((first_IE_ptr != NULL) && (decoded_ok == TRUE))
    {
      channel_information_T  before_params = {0};
      channel_information_T  after_params  = {0};
      rr_ia_params_T         ia_params     = {0};

      /* call the RR function that validates IAs */
      if (rr_respond_to_immediate_assignment(
            first_IE_ptr,
            &before_params,
            &after_params,
            &ia_params,
            FALSE,
            (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr + rr_gprs_edtm_data_ptr->pending_l3_msg_len),
            gas_id
          ) == EV_IMMEDIATE_ASSIGNMENT)
      {
        msg_valid = TRUE;
      }
      else
      {
        EDTM_ERROR_DEBUG("EDTM: IA in Pkt CS Command is not valid (%d,%d,%d)",
                         (int)before_params.channel_type,
                         (int)after_params.channel_type,
                         (int)ia_params.starting_time);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G("EDTM: error(%d) whilst parsing IA in Pkt CS Command",
                (int)error_type);
    }
  }
  else if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == IMMEDIATE_ASSIGNMENT_REJECT)
  {
    /* already validated that the IA REJECT is correctly sized */
    /* so just extract the T3122 wait indication from the msg  */
    rr_gprs_edtm_data_ptr->wait_indication = rr_gprs_edtm_data_ptr->pending_l3_msg_ptr[EDTM_IAREJ_T3122_OCTET];
    msg_valid = TRUE;
  }

  return(msg_valid);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_parse_pending_message
 *   --------------
 *
 *   Description:    Parses the L3 message stored in the local buffer
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        RR event for calling state machine
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_edtm_parse_pending_message(
  const gas_id_t gas_id
)
{
  RR_cause_T error_type = RR_NORMAL_EVENT;
  rr_event_T return_event;
  boolean    decoded_ok;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("EDTM ERROR: unexpected NULL pointer to L3 message");
    decoded_ok = FALSE;
  }
  else if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == DTM_ASSIGNMENT_COMMAND)
  {
    /* parse the DTM Assignment stored in the pending buffer */
    rr_check_for_protocol_errors(
      rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
      rr_gprs_edtm_data_ptr->pending_l3_msg_len,
      DCCH,
      FALSE, /* not short header format */
      &decoded_ok,
      &error_type,
      gas_id
    );

    /* log the signalling message */
    (void)rr_log_signaling_message(
      DCCH,
      rr_gprs_edtm_data_ptr->pending_l3_msg_typ,
      rr_gprs_edtm_data_ptr->pending_l3_msg_len,
      rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
      RR_DOWNLINK_MSG,
      gas_id
    );
  }
  else if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ != INVALID_L3_MSG_TYPE)
  {
    /* for the CCCH messages, perform specific validation now, */
    /* and do a sanity check for the presence of CS resources  */
    /* by inspecting the half octet "Dedicated mode or TBF".   */
    if ((rr_gprs_edtm_data_ptr->pending_l3_msg_ptr[EDTM_IA_DMT_OFFSET] & 0xF0) == 0x00)
    {
      decoded_ok = rr_gprs_edtm_validate_pending_ccch_message(gas_id);
    }
    else
    {
      decoded_ok = FALSE;
      error_type = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
    }

    /* log the signalling message (even if it will be discarded) */
    (void)rr_log_signaling_message(
      CCCH,
      rr_gprs_edtm_data_ptr->pending_l3_msg_typ,
      rr_gprs_edtm_data_ptr->pending_l3_msg_len,
      rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
      RR_DOWNLINK_MSG,
      gas_id
    );
  }
  else
  {
    MSG_GERAN_ERROR_0_G("EDTM ERROR: use of uninitialised L3 message");
    decoded_ok = FALSE;
  }

  if (decoded_ok == TRUE)
  {
    if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == DTM_ASSIGNMENT_COMMAND)
    {
      return_event = EV_EDTM_DTM_ASSIGNMENT_RECEIVED;
    }
    else if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == IMMEDIATE_ASSIGNMENT)
    {
      return_event = EV_EDTM_IMM_ASSIGNMENT_RECEIVED;
    }
    else if (rr_gprs_edtm_data_ptr->pending_l3_msg_typ == IMMEDIATE_ASSIGNMENT_REJECT)
    {
      return_event = EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED;
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Unexpected L3 message 0x%02X",(int)rr_gprs_edtm_data_ptr->pending_l3_msg_typ);
      return_event = EV_DTM_ASSIGNMENT_ERROR;
    }
  }
  else
  {
    EDTM_ERROR_DEBUG("Invalid assignment received (msg=0x%02X,err=%d)",
                      (int)rr_gprs_edtm_data_ptr->pending_l3_msg_typ, (int)error_type, 0);
    return_event = EV_DTM_ASSIGNMENT_ERROR;
  }

  return(return_event);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_process_dl_data_ind
 *   --------------
 *
 *   Description:    Processes the contents of a converted Pkt CS Command
 *
 *   Parameters:     dl_data_ind_ptr - input pointer to converted message
 *   -----------
 *
 *   Returns:        RR event for calling state machine
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_edtm_process_dl_data_ind(
  dl_data_ind_T *dl_data_ind_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T return_event  = EV_NO_EVENT;
  boolean    store_msg     = TRUE;
  boolean    add_l2_header = FALSE;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* cancel the guard timer */
  (void)rr_timer_cancel(T3196, gas_id);

  /* Extract message type from 2nd octet of DL_DATA_IND L3 buffer */
  switch(dl_data_ind_ptr->layer3_message[1])
  {
    case DTM_ASSIGNMENT_COMMAND:
      rr_gprs_edtm_data_ptr->pending_l3_msg_typ = DTM_ASSIGNMENT_COMMAND;
      rr_gprs_edtm_data_ptr->pending_l3_msg_len = dl_data_ind_ptr->l3_message_length;
      break;

    case IMMEDIATE_ASSIGNMENT_REJECT:
      if (rr_gprs_edtm_data_ptr->cs_request_sent == FALSE)
      {
        EDTM_ERROR_DEBUG("Unsolicited IAREJ received in Pkt CS Command",0,0,0);
        store_msg = FALSE;
        break;
      }
      else if (rr_timer_is_running(T3122, gas_id) == TRUE)
      {
        MSG_GERAN_MED_0_G("IAREJ received during T3122 wait period: ignored");
        store_msg = FALSE;
        break;
      }
      else if (dl_data_ind_ptr->l3_message_length < EDTM_MIN_LENGTH_IAREJ_MSG)
      {
        EDTM_ERROR_DEBUG("IAREJ received in Pkt CS Command is too short",0,0,0);
        store_msg = FALSE;
        return_event = EV_DTM_ASSIGNMENT_ERROR;
        break;
      }
      /* fallthrough */
/*lint -save -e825*/
    case IMMEDIATE_ASSIGNMENT:
      if (dl_data_ind_ptr->l3_message_length < EDTM_MIN_LENGTH_IA_MSG)
      {
        EDTM_ERROR_DEBUG("IA received in Pkt CS Command is too short",0,0,0);
        store_msg = FALSE;
        return_event = EV_DTM_ASSIGNMENT_ERROR;
      }
      else
      {
        rr_gprs_edtm_data_ptr->pending_l3_msg_typ = dl_data_ind_ptr->layer3_message[1];
        rr_gprs_edtm_data_ptr->pending_l3_msg_len = N201_CCCH;
        add_l2_header                = TRUE;

        /* if the supplied message is too long, truncate it */
        if (dl_data_ind_ptr->l3_message_length > (N201_CCCH - 1))
        {
          EDTM_ERROR_DEBUG("WARNING: NW has supplied %d octets for CCCH message",
                           (int)dl_data_ind_ptr->l3_message_length, 0, 0);
          dl_data_ind_ptr->l3_message_length = N201_CCCH - 1;
        }
      }
/*lint -restore*/
      break;

    default:
      /* Unexpected message type */
      store_msg = FALSE;
      return_event = EV_DTM_ASSIGNMENT_ERROR;
      EDTM_ERROR_DEBUG("Invalid octets (0x%02X,0x%02X,0x%02X) in Pkt CS Command",
                       (int)dl_data_ind_ptr->layer3_message[0],
                       (int)dl_data_ind_ptr->layer3_message[1],
                       (int)dl_data_ind_ptr->layer3_message[2]);
      break;
  }

  /* if a valid msg was received, make a copy of the L3 message data for later use */
  if (store_msg == TRUE)
  {
    /* free any existing message allocation */
    if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
    {
      GPRS_MEM_FREE(rr_gprs_edtm_data_ptr->pending_l3_msg_ptr);
    }

    /* then allocate memory for the new message */
    rr_gprs_edtm_data_ptr->pending_l3_msg_ptr = (byte *)GPRS_MEM_MALLOC(rr_gprs_edtm_data_ptr->pending_l3_msg_len);

    if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
    {
      /* fill the buffer with padding octets first */
      memset(rr_gprs_edtm_data_ptr->pending_l3_msg_ptr, 0x2B, rr_gprs_edtm_data_ptr->pending_l3_msg_len);

      if (add_l2_header == TRUE)
      {
        rr_gprs_edtm_data_ptr->pending_l3_msg_ptr[0] = (byte)(rr_gprs_edtm_data_ptr->pending_l3_msg_len << 2) | 0x01;
    //Replace memcpy with memscpy
        memscpy(rr_gprs_edtm_data_ptr->pending_l3_msg_ptr + 1,
          rr_gprs_edtm_data_ptr->pending_l3_msg_len-1,
                dl_data_ind_ptr->layer3_message,
                dl_data_ind_ptr->l3_message_length);

      }
      else
      {
      //Replace memcpy with memscpy
      memscpy(rr_gprs_edtm_data_ptr->pending_l3_msg_ptr,
          rr_gprs_edtm_data_ptr->pending_l3_msg_len,
          dl_data_ind_ptr->layer3_message,
          dl_data_ind_ptr->l3_message_length);

      }

      /* now deal with the message that has just been stored */
      return_event = rr_gprs_edtm_parse_pending_message(gas_id);
    }
    else /* memory allocation has failed */
    {
      /* reset the L3 buffers used for enhanced establishment, but */
      /* preserve the state-related variables for the next attempt */
      rr_gprs_edtm_reset_est_data(FALSE, gas_id);
      MSG_GERAN_ERROR_0_G("Unable to allocate memory for L3 message");
    }
  }

  return(return_event);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_respond_to_dtm_assignment
 *   --------------
 *
 *   Description:    Processes a DTM Assignment command from the PACCH
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        RR event for caller
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_edtm_respond_to_dtm_assignment(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  RR_cause_T           error_cause;
  sys_band_T           new_band;
  rr_event_T           return_event;

  /* RR's global stores can be initialised here, as this */
  /* represents an initial entry point to dedicated mode */
  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr == NULL)
  {
    return(EV_DTM_ASSIGNMENT_ERROR);
  }

  dedic_ptr->curr_channel_spec.channel_mode_1 = SIGNALLING_ONLY;
  dedic_ptr->curr_channel_spec.channel_mode_2 = SIGNALLING_ONLY;
  rr_invalidate_multirate_config(&(dedic_ptr->curr_amr_config));

  error_cause = rr_dtm_process_dtm_assignment_command(
    &new_band,
    DTM_CS_ASSIGNMENT_REQUIRED,
    gas_id
  );

  if (error_cause == RR_NORMAL_EVENT)
  {
    /* A DTM Assignment can contain GPRS Cell Options which */
    /* could (mistakenly) disable EDTM support in the cell  */
    if (rr_gprs_edtm_available_in_cell(gas_id) == FALSE)
    {
      EDTM_ERROR_DEBUG("WARNING: EDTM support disabled by DTM Assignment", 0, 0, 0);

      return_event = EV_DTM_ASSIGNMENT_ERROR;
    }
    else
    {
      rr_set_current_dedicated_band((rr_internal_band_T)new_band, gas_id);

      return_event = EV_DTM_ASSIGNMENT_COMMAND_CHECKED;
    }
  }
  else
  {
    return_event = EV_DTM_ASSIGNMENT_ERROR;
  }

  return return_event;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_tmsi_valid
 *   --------------
 *
 *   Description:    Retrieves TMSI from the RR public store and validates it
 *
 *   Parameters:     tmsi_ptr - output pointer to uint32 variable for TMSI
 *   -----------
 *
 *   Returns:        TRUE if a valid TMSI is available
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_tmsi_valid(
  uint32 *tmsi_ptr,
  byte destsize,
  const gas_id_t gas_id
)
{
  if (tmsi_ptr != NULL)
  {
    rr_siminfo_get_tmsi((byte *)tmsi_ptr,destsize, gas_id);

    if (*tmsi_ptr != TMSI_PTMSI_INVALID)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_page_response_allowed
 *   --------------
 *
 *   Description:    Initialises local and global stores for EDTM page response
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        TRUE if a response to the EDTM page is allowed
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_edtm_page_response_allowed(
  const gas_id_t gas_id
)
{
  uint32 local_tmsi = TMSI_PTMSI_INVALID;
  byte   local_id[RR_MAX_IDENTITY_SIZE];
  rr_identity_type_e identity_type = RR_IDENTITY_TYPE_IMSI;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* 3GPP 24.008 (v6.13.0) section 10.5.1.4 states:  */
  /* use the TMSI if it is available, IMSI otherwise */
  if (rr_gprs_edtm_tmsi_valid(&local_tmsi,sizeof(local_tmsi), gas_id) == TRUE)
  {
    /* first two octets in the identity array specify length and format */
    local_id[0] = TMSI_SIZE + 1;
    local_id[1] = 0xF0 | 0x04;

    /* subsequent 4 octets contain the TMSI digits */
    local_id[2] = (byte)((local_tmsi & 0xFF000000) >> 24);
    local_id[3] = (byte)((local_tmsi & 0x00FF0000) >> 16);
    local_id[4] = (byte)((local_tmsi & 0x0000FF00) >>  8);
    local_id[5] = (byte)((local_tmsi & 0x000000FF));

    identity_type = RR_IDENTITY_TYPE_TMSI;
  }
  else if (rr_siminfo_get_imsi(local_id, sizeof(local_id),gas_id) == FALSE)
  {
    MSG_GERAN_ERROR_0_G("EDTM: No IMSI available");
    return(FALSE);
  }
  else
  {
    byte imsi_length;

    /* IMSI was valid, double-check the length octet */
    imsi_length = local_id[0];
    if ((imsi_length == 0) || (imsi_length >= MM_SIM_IMSI_SIZE))
    {
      MSG_GERAN_ERROR_1_G("EDTM: IMSI length %d is invalid", (int)imsi_length);
      return(FALSE);
    }
  }

  /* copy the local_id array to the RR conn. establishment local store */
  rr_ce_edtm_set_paged_identity(local_id, gas_id);

  /* NOTE: the CHANNEL NEEDED for a RACH is not set here, because EDTM */
  /* does not use the AGCH for CS establishment. If we were paged, GRR */
  /* will already have called rr_ce_set_channel_needed() */

  /* set up the local establishment cause and identity type */
  rr_gprs_edtm_data_ptr->establishment_cause = (byte)MM_RR_ANSWER_TO_PAGING;
  rr_gprs_edtm_data_ptr->cs_page_info.identity_type = identity_type;
  //Replace mecpy with memscpy
  memscpy(rr_gprs_edtm_data_ptr->cs_page_info.identity,sizeof(rr_gprs_edtm_data_ptr->cs_page_info.identity), local_id, RR_MAX_IDENTITY_SIZE);


  return(rr_gprs_edtm_enhanced_establishment_allowed(gas_id));
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_paging_failure
 *   --------------
 *
 *   Description:    Recovers from a failure during EDTM paging (i.e. failure
 *                   during the processing of an unsolicited assignment)
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_paging_failure(
  const gas_id_t gas_id
)
{
  rr_page_info_t page_info;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  (void)rr_gprs_edtm_page_response_allowed(gas_id);

  MSG_GERAN_HIGH_1_G("EDTM page failed, attempt recovery: channel needed=%d",
           (int)rr_gprs_edtm_data_ptr->cs_page_info.channel_needed);

  // Send RR_IMSG_PAGE_IND into RR-CONTROL to indicate a page has been received
  // This contains all the page details
  page_info.domain = RR_PAGE_DOMAIN_CS;
  page_info.identity_type = rr_gprs_edtm_data_ptr->cs_page_info.identity_type;
   //Replace mecpy with memscpy
  memscpy(page_info.identity,sizeof(page_info.identity), rr_gprs_edtm_data_ptr->cs_page_info.identity, sizeof(page_info.identity));
  page_info.channel_needed = rr_gprs_edtm_data_ptr->cs_page_info.channel_needed;

  rr_send_imsg_page_ind(&page_info, gas_id);

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_connection_event
 *   --------------
 *
 *   Description:    Move the RR CE state machine into a suitable state
 *
 *   Parameters:     Input event
 *   -----------
 *
 *   Returns:        None
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_connection_event(
  rr_event_T rr_event,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* reset connection establishment so that it starts from a known state */
  (void)rr_connection_establishment_control(EV_GPRS_RESET_RR_CE_STATE, NULL, gas_id);

  /* then pump in the event required by EDTM */
  if (rr_event != EV_NO_EVENT)
  {
    if (rr_event != EV_RESET_SOFTWARE)
    {
      /* set the connection establishment parameters */
      rr_ce_set_edtm_access_parameters(rr_gprs_edtm_data_ptr->establishment_cause, gas_id);
    }

    (void)rr_connection_establishment_control(rr_event, NULL, gas_id);
  }

  return;
}

/*!
 * \brief Indicates if a CS establishment using E-DTM is in progress
 *
 * \return boolean - TRUE if a CS establishment using E-DTM is in progress
 */
boolean rr_gprs_edtm_cs_establishment_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
  {
    return TRUE;
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_complete_cs_establishment
 *   --------------
 *
 *   Description:    Called when CS establishment is completed, determines
 *                   whether to send RR_EST_CNF or RR_EST_IND to MM.
 *                   Also resets the CS establishment data in this module.
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        None
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_complete_cs_establishment(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->pending_l3_msg_ptr != NULL)
  {
    rr_gprs_edtm_reset_est_data(TRUE, gas_id);
  }

  (void)rr_gprs_edtm_control(EV_RR_EST_CNF, NULL, gas_id);

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_store_channel_needed
 *   --------------
 *
 *   Description: Stores the CS channel needed for subsequent recovery
 *
 *   Parameters:  channel_type - the CS channel type that is about to be used
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_store_channel_needed(
  channel_type_T channel_type,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if ((channel_type == FULL_RATE_TRAFFIC) || (channel_type == HALF_RATE_TRAFFIC))
  {
    rr_gprs_edtm_data_ptr->cs_page_info.channel_needed = FULL_RATE_OR_HALF_RATE_TRAFFIC;
  }
  else if ((channel_type == SDCCH_4) || (channel_type == SDCCH_8))
  {
    rr_gprs_edtm_data_ptr->cs_page_info.channel_needed = SDCCH_NEEDED;
  }
  else
  {
    rr_gprs_edtm_data_ptr->cs_page_info.channel_needed = ANY_CHANNEL;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_prioritise_saved_msg
 *   --------------
 *
 *   Description: Called by RR to prioritise an incoming message
 *
 *   Parameters:  new_msg_ptr - the incoming msg that RR wants to save
 *   -----------
 *
 *   Returns:     TRUE if the caller can process the incoming message
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_prioritise_saved_msg(
  rr_cmd_bdy_type *new_msg_ptr,
  const gas_id_t gas_id
)
{
  boolean bProcess = TRUE;

  if (new_msg_ptr->message_header.message_set == MS_MM_RR)
  {
    if (new_msg_ptr->message_header.message_id == (byte)RR_EST_REQ)
    {
      /* establishment collision */
      EDTM_ERROR_DEBUG("EDTM has detected establishment collision", 0, 0, 0);

      /* reset the local establishment data */
      rr_gprs_edtm_reset_est_data(TRUE, gas_id);

      /* discard the assignment message saved by the EDTM module */
      grr_throwaway_saved_message(gas_id);
    }
    else
    {
      if (new_msg_ptr->message_header.message_id == (byte)RR_PLMN_LIST_REQ)
      {
        rr_plmn_list_req_T *req_ptr = (rr_plmn_list_req_T *)new_msg_ptr;

        /* lower priority than the pending assignment */
        bProcess = FALSE;

        /* send back an abort confirmation */
        rr_send_plmn_list_cnf(NULL, MM_AS_ABORTED, req_ptr, gas_id);
      }
      else if (new_msg_ptr->message_header.message_id == (byte)RR_PLMN_SELECT_REQ)
      {
        gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

        /* lower priority than the pending assignment */
        bProcess = FALSE;

        /* send a confirmation that indicates a connection is in progress */
        rr_send_plmn_select_cnf(
          MM_AS_CONNECTED,
          MM_AS_SERVICE_AVAILABLE,
          &(scell_info_ptr->gsm.location_area_identification),
          NULL,
          scell_info_ptr->gsm.BCCH_allocation,
          &scell_info_ptr->gsm,
          gas_id
        );
      }
      else
      {
        EDTM_ERROR_DEBUG("NAS signal %d will abort EDTM operation",
                         (int)new_msg_ptr->message_header.message_id, 0, 0);
      }
    }
  }

  return(bProcess);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_process_pkt_cs_est_reject
 *   --------------
 *
 *   Description:    Process the PKT_CS_EST_REJ sent by GMAC when the Pkt CS Request
 *                   cannot be serviced for some reason.
 *
 *   Parameters:     Message pointer
 *   -----------
 *
 *   Returns:        RR event for calling state machine
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T rr_gprs_edtm_process_pkt_cs_est_reject(
  mac_grr_edtm_pkt_cs_est_rej_t *pkt_cs_est_rej_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T return_event;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  (void)rr_timer_cancel(T3196, gas_id);

  if (rr_gprs_edtm_data_ptr->cs_request_sent == TRUE)
  {
    rr_event_T next_event;

    /* at this stage, all the reject causes are treated in the same way */
    if (rr_mac_ps_access_blocked(gas_id))
    {
      /* if GMAC has been suspended since the EDTM request was sent, */
      /* it implies that some other process has been initiated by RR */
      MSG_GERAN_HIGH_1_G("EDTM request rejected by GMAC suspension (cause=%1d)",
               (int)pkt_cs_est_rej_ptr->reject_cause);

      /* abort the ongoing request, advise MM, and reset this module */
      rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
      next_event = EV_RESET_SOFTWARE;
    }
    else
    {
      MSG_GERAN_HIGH_1_G("EDTM reject received from GMAC (cause=%1d)",
               (int)pkt_cs_est_rej_ptr->reject_cause);

      next_event = EV_EDTM_ESTABLISHMENT_REJECTED;
    }

    /* advise the main state machine, which takes care of these actions */
    return_event = rr_gprs_edtm_control(next_event, NULL, gas_id);
  }
  else
  {
    EDTM_ERROR_DEBUG("E-DTM reject received from GMAC without request", 0, 0, 0);
    return_event = EV_NO_EVENT;
  }

  (void)pkt_cs_est_rej_ptr; /* for LINT */
  return(return_event);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_process_pkt_cs_release
 *   --------------
 *
 *   Description:    Process the PKT_CS_RELEASE sent by GMAC
 *
 *   Parameters:     Message pointer
 *   -----------
 *
 *   Returns:        RR event for main state machine
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T rr_gprs_edtm_process_pkt_cs_release(
  mac_grr_edtm_cs_release_ind_t *pkt_cs_release_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T return_event;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (pkt_cs_release_ptr != NULL)
  {
    if (pkt_cs_release_ptr->release_cause == EDTM_CS_REL_ABNORMAL)
    {
      /* RR treats this as a Radio Link Failure (RLF) */
      return_event = EV_RADIO_LINK_FAILURE;
      rr_gprs_edtm_data_ptr->cs_release_started = FALSE;
    }
    else
    {
      return_event = EV_NO_EVENT;
      rr_gprs_edtm_data_ptr->cs_release_started = rr_gprs_edtm_initiate_enhanced_release(gas_id);
      if (rr_gprs_edtm_data_ptr->cs_release_started == TRUE)
      {
        /* stop all SI updates to other layers until the release has completed */
        rr_gprs_sys_info_block_all_updates(gas_id);
      }
    }
  }
  else
  {
    return_event = EV_NO_EVENT;
    MSG_GERAN_ERROR_0_G("Null pointer passed as Pkt CS Release Ind: ignored");
  }

  (void)pkt_cs_release_ptr; /* for LINT */
  return(return_event);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_process_channel_release
 *   --------------
 *
 *   Description:    Process the CHANNEL RELEASE sent by the network
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        TRUE if PS services are allowed to resume
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_process_channel_release(
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  byte            *start_IE;
  boolean          ps_resumed = FALSE;
  edtm_data_t     *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(CHANNEL_RELEASE, gas_id);

  /* locate the Enhanced DTM CS Release Indication IE */
  start_IE = rr_find_start_of_IE(current_IE, EDTM_CS_RELEASE_IEI);

  if (start_IE != NULL)
  {
    /* See 3GPP 44.018 Table 10.5.2.14e.1 */
    if (*start_IE & 0x01)
    {
      /* the mobile station is allowed to continue in packet transfer mode, */
      /* and can use this as an implicit default for the GPRS Resumption IE */
      ps_resumed = TRUE;
      rr_gprs_edtm_data_ptr->ps_release_ind = FALSE;
    }
    else
    {
      /* the mobile station is not allowed to continue in packet transfer mode */
      rr_gprs_edtm_data_ptr->ps_release_ind = TRUE;
    }
  }
  else
  {
    /* No IE present, assume that PS services are not allowed to continue */
    rr_gprs_edtm_data_ptr->ps_release_ind = TRUE;
  }

  return(ps_resumed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_ps_release_required
 *   --------------
 *
 *   Description:    Determines whether the PS connection must be released
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        TRUE if the PS connection must be released
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_ps_release_required(
  const gas_id_t gas_id
)
{
  boolean            ps_must_release;
  edtm_data_t       *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (rr_gprs_edtm_data_ptr->cs_release_started == TRUE)
  {
    /* check whether there is sufficient system info to sustain transfer */
    if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
    {
      /* sys info is OK, check local flag for the latest requirement */
      ps_must_release = rr_gprs_edtm_data_ptr->ps_release_ind;
    }
    else
    {
      ps_must_release = TRUE;
    }
  }
  else
  {
    ps_must_release = TRUE;
  }

  MSG_GERAN_MED_3_G("EDTM: PS release required=%1d (cs=%1d)",
          (int)ps_must_release,
          (int)rr_gprs_edtm_data_ptr->cs_release_started,
          0);

  return(ps_must_release);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_cache_dtm_release
 *   --------------
 *
 *   Description:    Store the contents of the MPH_DTM_CHANNEL_RELEASE_IND
 *
 *   Parameters:     Input message pointer
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_cache_dtm_release(
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  mph_dtm_channel_release_ind_t *msg_ptr;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  msg_ptr = (mph_dtm_channel_release_ind_t *)message_ptr;

  if (msg_ptr != NULL)
  {
    rr_gprs_edtm_data_ptr->l1_ps_release = msg_ptr->ps_release;
    rr_gprs_edtm_data_ptr->l1_cs_release = msg_ptr->cs_release;
    rr_gprs_edtm_data_ptr->l1_error_code = msg_ptr->error_code;
    MSG_GERAN_MED_3_G("L1->RR DTM release: ps=%1d cs=%1d err=%1d",
            (int)rr_gprs_edtm_data_ptr->l1_ps_release,
            (int)rr_gprs_edtm_data_ptr->l1_cs_release,
            (int)rr_gprs_edtm_data_ptr->l1_error_code);
  }
  else
  {
    /* caller can specify a NULL pointer to reset the error flag */
    rr_gprs_edtm_data_ptr->l1_error_code = TRUE;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_signal_cs_release
 *   --------------
 *
 *   Description:    Signals GMAC that an EDTM CS release is in progress, and
 *                   sends updated system information parameters to GPL1
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_signal_cs_release(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);
  /* reset the local store for the imminent L1 release signal */
  rr_gprs_edtm_cache_dtm_release(NULL, gas_id);

  /* ensure that the Layer 1 idle mode is set up correctly */
  scell_ptr->l1_idle_mode = GRR_L1_IDLE_MODE_CCCH;
  scell_ptr->bcch_sys_info.si13_data_valid_flag = TRUE;

  /* a usable set of system info has been acquired */
  scell_ptr->gsm.valid_data_flag = TRUE;

  /* ensure system information is available for MAC & RLC */
  rr_si_update_public_store(gas_id);

  /* inform GPL1 of the latest parameter set for the serving cell */
  rr_send_mph_dtm_l1param_update_req(scell_ptr, gas_id);

  /* inform GMAC of the release and advise it of the new serving cell type */
  rr_send_grr_mac_edtm_channel_release_ind(gas_id);

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_ps_allowed_after_cs_release
 *   --------------
 *
 *   Description:    Determines whether PS services can continue after CS release
 *
 *   Parameters:     Pointer to current RR release mode (input and output)
 *   -----------
 *
 *   Returns:        TRUE if PS services can be maintained
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_ps_allowed_after_cs_release(
  rr_conn_release_mode_t mode,
  const gas_id_t gas_id
)
{
  boolean ps_allowed;
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  if (mode == RELEASE_MODE_EDTM)
  {
    /* EDTM release mode is only set if the following have occurred (in this order) */
    /* - a Pkt CS Release Indication was received during Dual Transfer Mode;        */
    /* - the cell supports SI/PSI status procedures;                                */
    /* - a Channel Release was received indicating that PS services can persist.    */
    /* Otherwise, the normal GSM or DTM mode procedures will be used by RR.         */
    if ((rr_gprs_edtm_data_ptr->l1_ps_release == FALSE) &&
        (rr_gprs_edtm_data_ptr->l1_error_code == FALSE))
    {
      /* L1 has confirmed that PS is still active */
      ps_allowed = TRUE;
    }
    else
    {
      /* PS has been released, the DTM module needs to take over */
      /* this will perform any appropriate suspension/resumption */
      MSG_GERAN_MED_0_G("Unable to use EDTM release mode: perform normal release");
      ps_allowed = FALSE;

      // Inform DTM module that E-DTM CS release is not possible
      (void)rr_dtm_control(EV_EDTM_RELEASE_ABORTED, NULL, gas_id);
    }
  }
  else
  {
    /* release mode has not been set by the EDTM module */
    ps_allowed = FALSE;
  }

  return(ps_allowed);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_complete_cs_release
 *   --------------
 *
 *   Description:    Performs final actions required for PS continuity
 *
 *   Parameters:     None
 *   -----------
 *
 *   Returns:        Event for main RR state machine (via conn release)
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_complete_cs_release(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_ptr;
  edtm_data_t       *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  scell_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_ptr);

  /* clear all the bits for SIs received so far */
  CLEAR_BIT(rr_gprs_edtm_data_ptr->received_si_mask,
            scell_ptr->bcch_sys_info.si_messages_required);

  /* unblock sys info updates to NAS, L1 etc. */
  rr_gprs_sys_info_allow_all_updates(gas_id);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_dedicated_mode_started
 *   --------------
 *
 *   Description:    Called at started of a dedicated mode connection
 *
 *   Parameters:     message_id - the L1 signal ID that started the new mode
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_dedicated_mode_started(
  rr_l1_message_id_enum_T message_id,
  const gas_id_t gas_id
)
{
  if (message_id == MPH_IMMED_ASSIGN_CNF)
  {
    /* this is the entry point into dedicated mode, so cache the */
    /* current cell parameters and check for changes on release  */
    rr_gprs_edtm_store_cell_parameters(gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_dual_transfer_started
 *   --------------
 *
 *   Description:    Called each time DTM mode is entered (not on reassignment)
 *
 *   Parameters:     using_edtm - TRUE if this is a result of EDTM establishment
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_dual_transfer_started(
  boolean using_edtm,
  const gas_id_t gas_id
)
{
  /* initialise all the SI and PSI masks ready for enhanced release */
  rr_gprs_edtm_reset_rel_data(gas_id);

  if (using_edtm == TRUE)
  {
    /* this is the entry point into dedicated mode, as well as dual transfer mode */
    /* so cache the current cell parameters and check for any changes on release  */
    rr_gprs_edtm_store_cell_parameters(gas_id);

    /*******************************************************/
    /* Reset the ciphering flags for the new RR connection */
    /*******************************************************/
    rr_set_ciphering_mode_command_received(FALSE, gas_id);
    rr_ciphering_has_started(FALSE, gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_gmac_uplink_pending
 *   --------------
 *
 *   Description:    Notifies this module whether an uplink assignment is
 *                   pending in GMAC (i.e. contention resolution is starting)
 *
 *   Parameters:     uplink_pending - TRUE if an assignment has been sent
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
void rr_gprs_edtm_gmac_uplink_pending(
  boolean uplink_pending,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  edtm_data_t *rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  rr_gprs_edtm_data_ptr->gmac_uplink_pending = uplink_pending;

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_enhanced_release_started
 *   --------------
 *
 *   Description:    Determines whether an EDTM release has been started
 *
 *   Parameters:     VOID
 *   -----------
 *
 *   Returns:        Current value of cs_release_started flag
 *   --------
 *
 *
 ****************************************************************************/
boolean rr_gprs_edtm_enhanced_release_started(
  const gas_id_t gas_id
)
{
  edtm_data_t *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  return(rr_gprs_edtm_data_ptr->cs_release_started);
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_control_state_name
 *   --------------
 *
 *   Description:    Returns a string describing the specified EDTM state
 *
 *   Parameters:     state - input value of state machine
 *   -----------
 *
 *   Returns:        Pointer to character string
 *   --------
 *
 *
 ****************************************************************************/
static char *rr_gprs_edtm_control_state_name(
  edtm_control_state_t state
)
{
  char *s;

  switch (state)
  {
    case EDTM_NULL:
      s = "NULL";
      break;

    case EDTM_CS_ESTABLISHMENT:
      s = "CS_ESTABLISHMENT";
      break;

    case EDTM_DTM_ESTABLISHMENT:
      s = "DTM_ESTABLISHMENT";
      break;

    default:
      s = "UNKNOWN";
      break;
  }

  return(s);

}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_log_state_change
 *   --------------
 *
 *   Description:    Outputs debug text to show a change of state
 *
 *   Parameters:     old_state, new_state
 *   -----------
 *
 *   Returns:        Void
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_edtm_log_state_change(
  edtm_control_state_t old_state,
  edtm_control_state_t new_state,
  const gas_id_t gas_id
)
{
  if ((EDTM_CS_ESTABLISHMENT == old_state) &&
      (EDTM_CS_ESTABLISHMENT != new_state))
  {
    /* leaving the CS establishment state, allow GPRS Tuneaway */
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_EDTM, FALSE, geran_map_gas_id_to_nas_id(gas_id));
  }

  RR_LOG_STATE_TRANSITION(
    "rr_gprs_edtm_control",
    old_state,
    new_state,
    rr_gprs_edtm_control_state_name,
    gas_id
  );

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_edtm_control
 *   --------------
 *
 *   Description:    Main state machine for EDTM procedures
 *
 *   Parameters:     Input event and message pointer
 *   -----------
 *
 *   Returns:        RR event for calling state machine
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T rr_gprs_edtm_control(
  rr_event_T event_in,
  void *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T                  event_out = EV_NO_EVENT;
  rr_event_T                  new_event = EV_NO_EVENT;
  byte                        message_id;
  byte                        message_set;
  edtm_data_t                *rr_gprs_edtm_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_edtm_data_ptr = rr_gprs_edtm_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_edtm_data_ptr);

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* message_ptr and EV_INPUT_MESSAGE returned. If message_ptr, message_set or */
  /* message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  event_in = rr_extract_message_header_details(event_in,
                                               message_ptr,
                                               &message_set,
                                               &message_id);

  if (event_in == EV_INPUT_MESSAGE)
  {
    if ( (message_set == MS_RR_L2)           &&
         (message_id  == (byte) DL_DATA_IND) &&
         (message_ptr != NULL) )
    {
      event_in = rr_gprs_edtm_process_dl_data_ind((dl_data_ind_T *)message_ptr, gas_id);
    }
    else if ( (message_set == MS_RR_L1)                          &&
              (message_id  == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) &&
              (message_ptr != NULL) )
    {
      /* L1 has completed the DTM connection for EDTM establishment */
      event_in = EV_DTM_ASSIGNMENT_COMPLETE;
    }
    else if ( (message_set == MS_RR_L1)                           &&
              (message_id  == (byte) MPH_DTM_CHANNEL_RELEASE_IND) &&
              (message_ptr != NULL) )
    {
      /* L1 has released the DTM connection during establishment */
      event_in = EV_DTM_PERFORM_CSPS_RELEASE;
    }
    else if ( (message_set == MS_MAC_RR)                            &&
              (message_id  == (byte)MAC_GRR_DTM_CHANNEL_ASSIGN_REJ) &&
              (message_ptr != NULL) )
    {
      /* GRR sent a DTM channel assignment to GMAC, but GMAC has rejected it */
      event_in = EV_DTM_ASSIGNMENT_ERROR;
    }
    else if ( (message_set == MS_MAC_RR)                &&
              (message_id  == (byte)MAC_GRR_MAC_STATUS) &&
              (message_ptr != NULL) )
    {
      mac_grr_mac_status_t *mac_status_ptr = (mac_grr_mac_status_t *)message_ptr;

      if (mac_status_ptr->mac_status == MAC_IDLE)
      {
        event_in = EV_MAC_IDLE;
      }
      else if (mac_status_ptr->mac_status == MAC_IDLE_BLOCKED)
      {
        event_in = EV_MAC_IDLE_BLOCKED;
      }
      else
      {
        event_in = EV_NO_EVENT;
      }
    }
  }
  else if (event_in == EV_MAC_IDLE_MODE)
  {
    /* should only be generated by a GMAC status change for cell reselection */
    MSG_GERAN_MED_0_G("EDTM: cell reselection indicates GMAC idle");
    event_in = EV_MAC_IDLE;
  }
  else if (event_in == EV_RR_EST_CNF)
  {
    EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
    event_in = EV_NO_EVENT;
  }

  if ((event_in == EV_MAC_IDLE_BLOCKED) ||
      (event_in == EV_MAC_IDLE))
  {
    /* reset the flag that determines whether an uplink is pending */
    rr_gprs_edtm_data_ptr->gmac_uplink_pending = FALSE;
  }

  while (event_in != EV_NO_EVENT)
  {
    if (event_in == EV_RESET_SOFTWARE)
    {
      /* reset private data used for enhanced CS establishment */
      rr_gprs_edtm_reset_est_data(TRUE, gas_id);

      /* reset private data used for enhanced release */
      rr_gprs_edtm_reset_rel_data(gas_id);

      /* return to NULL state */
      EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
      event_in = EV_NO_EVENT;
      continue;
    }

    switch (rr_gprs_edtm_data_ptr->state)
    {
/****************************************************************************/
/*                                                                          */
/* NULL state - EDTM module is inactive, waiting for input                  */
/*                                                                          */
/****************************************************************************/
      case EDTM_NULL:
        /* EV_EDTM_DTM_ASSIGNMENT_RECEIVED
         * An unsolicited DTMA has arrived in a Pkt CS Command, so the UE
         * is being paged for CS services and must transition to DTM.
         * The DTM message has been stored in this module for the time being
         */
        if (event_in == EV_EDTM_DTM_ASSIGNMENT_RECEIVED)
        {
          /* check whether an EDTM page response can be handled */
          if (rr_gprs_edtm_page_response_allowed(gas_id) == TRUE)
          {
            MSG_GERAN_MED_0_G("Unsolicited DTM Assignment: respond to EDTM paging");

            /* transition to the DTM establishment state */
            EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_DTM_ESTABLISHMENT, gas_id);

            /* and handle the response there */
            new_event = rr_gprs_edtm_respond_to_dtm_assignment(gas_id);

            rr_ce_init_conn_info(
              FALSE,    // requested_by_nas
              gas_id    // gas_id
            );
          }
          else
          {
            EDTM_ERROR_DEBUG("Ignoring unsolicited DTM Assignment", 0, 0, 0);
          }
        }
        /* EV_EDTM_IMM_ASSIGNMENT_RECEIVED
         * An unsolicited IA has arrived in a Pkt CS Command, so the UE
         * is being paged for CS services and must suspend any ongoing TBF.
         * The IA message has been stored in this module, and can only be
         * triggered once GMAC has gone idle after releasing the TBF.
         */
        else if (event_in == EV_EDTM_IMM_ASSIGNMENT_RECEIVED)
        {
          /* save the pending Immediate Assignment message */
          if ((rr_gprs_edtm_page_response_allowed(gas_id)     == TRUE) &&
              (rr_gprs_edtm_save_pending_ccch_message(gas_id) == TRUE))
          {
            MSG_GERAN_MED_0_G("Unsolicited IA received: respond to EDTM paging");

            /* disable system information refresh */
            rr_gprs_suspend_camped_sys_info_handling(FALSE, gas_id);

            /* get connection establishment control ready */
            rr_gprs_edtm_connection_event(event_in, gas_id);

            /* push DTM control into the CS establishment state */
            (void)rr_dtm_control(event_in, NULL, gas_id);

            /* suspend GPRS in GMM before blocking PS access and starting CS establishment */
            rr_gmm_suspend_gprs(gas_id);

            /* tell GRR-CONTROL to block PS access (it will then process the saved message) */
            rr_grr_send_imsg_no_ps_access_req(
              FALSE,   // del_all_pdus
              NULL,    // confirm_cb_fn
              gas_id   // gas_id
            );

            rr_ce_init_conn_info(
              FALSE,    // requested_by_nas
              gas_id    // gas_id
            );
          }
          else
          {
            /* RR couldn't save the pending message, so reset now */
            EDTM_ERROR_DEBUG("Ignoring unsolicited Immediate Assignment", 0, 0, 0);
            rr_gprs_edtm_reset_est_data(TRUE, gas_id);
          }
        }
        /* EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED
         * An unsolicited IAREJ has arrived in a Pkt CS Command.
         */
        else if (event_in == EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED)
        {
          EDTM_ERROR_DEBUG("Ignoring unsolicited Immediate Assignment Reject", 0, 0, 0);
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);
        }
        /* EV_RR_EST_REQ
         * MM has requested a CS connection and the DTM module is checking
         * whether enhanced establishment is possible.
         */
        else if (event_in == EV_RR_EST_REQ)
        {
          if (rr_gprs_edtm_initiate_enhanced_establishment(gas_id))
          {
            /* return a valid event to the caller */
            event_out = EV_EDTM_ESTABLISHMENT_STARTED;

            /* then transition to the establishment state */
            EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_CS_ESTABLISHMENT, gas_id);
          }
          else
          {
            event_out = EV_NO_EVENT;
          }
        }
        else if (event_in == EV_DTM_ASSIGNMENT_ERROR)
        {
          /* Unsolicited bad assignment received - ignore it */
          EDTM_ERROR_DEBUG("EDTM ignored invalid assignment", 0, 0, 0);
        }
        else if (event_in == EV_CONNECTION_ABORTION)
        {
          /* an Immediate Assignment received in this module was aborted */
          /* and the RR connection establishment module wants to recover */
          EDTM_ERROR_DEBUG("EDTM aborted Immediate Assignment", 0, 0, 0);

          /* reset all establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);
        }
        break;

/****************************************************************************/
/*                                                                          */
/* CS ESTABLISHMENT state - attempting to establish CS connection via PACCH */
/* Note that this state implies a Mobile Originated connection (from NAS)   */
/*                                                                          */
/****************************************************************************/
      case EDTM_CS_ESTABLISHMENT:
        /* EV_EDTM_DTM_ASSIGNMENT_RECEIVED
         */
        if (event_in == EV_EDTM_DTM_ASSIGNMENT_RECEIVED)
        {
          /* transition to the DTM establishment state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_DTM_ESTABLISHMENT, gas_id);

          /* and handle the response there */
          new_event = rr_gprs_edtm_respond_to_dtm_assignment(gas_id);
        }
        /* EV_DTM_ASSIGNMENT_ERROR
         * A DTM Assignment was received, but the decoding failed
         */
        else if (event_in == EV_DTM_ASSIGNMENT_ERROR)
        {
          /* treat this as a GMAC reject, and start normal CS establishment */
          new_event = EV_EDTM_ESTABLISHMENT_REJECTED;
        }
        /* EV_EDTM_IMM_ASSIGNMENT_RECEIVED
         * An IA has arrived in a Pkt CS Command, in response to our request
         * so suspend any ongoing TBF.
         * The IA message has been stored in this module, and can only be
         * triggered once GMAC has gone idle after releasing the TBF.
         */
        else if (event_in == EV_EDTM_IMM_ASSIGNMENT_RECEIVED)
        {
          /* save the pending Immediate Assignment message */
          if (rr_gprs_edtm_save_pending_ccch_message(gas_id))
          {
             /* disable system information refresh */
             rr_gprs_suspend_camped_sys_info_handling(FALSE, gas_id);

            /* get connection establishment control ready */
            rr_gprs_edtm_connection_event(event_in, gas_id);

            /* push DTM control into the CS establishment state */
            (void)rr_dtm_control(event_in, NULL, gas_id);

            /* suspend GPRS in GMM before blocking PS access and starting CS establishment */
            rr_gmm_suspend_gprs(gas_id);

            /* tell GRR-CONTROL to block PS access (it will then process the saved message) */
            rr_grr_send_imsg_no_ps_access_req(
              FALSE,   // del_all_pdus
              NULL,    // confirm_cb_fn
              gas_id   // gas_id
            );
          }
          else
          {
            /* RR couldn't save the pending message, so reset now */
            EDTM_ERROR_DEBUG("Ignoring Immediate Assignment", 0, 0, 0);
            rr_gprs_edtm_reset_est_data(TRUE, gas_id);
            rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);

            /* allow GRR to resume camped procedures */
            event_out = RR_EV_CAMPED_ON_CELL;
          }

          /* transition back to NULL state, nothing further to do here */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        /* EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED
         * An IAREJ has arrived in a Pkt CS Command, in response to our request.
         * The only action necessary is to start T3122 (if required), inform MM,
         * and allow packet services to continue without interruption
         */
        else if (event_in == EV_EDTM_IMM_ASSIGNMENT_REJECT_RECEIVED)
        {
          /* use utility function to start T3122 only */
          MSG_GERAN_HIGH_0_G("IAREJ is for mobile");

          // Start T3122 using wait_indication duration (secs)
          rr_start_t3122(
            rr_gprs_edtm_data_ptr->wait_indication,
            rr_gprs_edtm_data_ptr->establishment_cause,
            gas_id
          );

          /* advise NAS that the request has been rejected */
          rr_send_abort_ind(RR_ABORT_IA_REJECT_RECEIVED, MM_RR_OTHER_FAILURE, gas_id);

          /* if T3122 is not started (wait indication is zero) then inform MM
           * that T3122 is not running - normally this would happen upon expiry
           */
          if (rr_gprs_edtm_data_ptr->wait_indication == 0)
          {
            grr_update_mm_with_rr_service_ind(gas_id);
          }

          /* free the saved message, transition back to NULL state */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);

          event_out = EV_NO_EVENT;
        }
        /* EV_EDTM_ESTABLISHMENT_REJECTED
         * GMAC has rejected the attempt to establish a CS call
         * via PACCH, or has had no valid response from the network.
         */
        else if (event_in == EV_EDTM_ESTABLISHMENT_REJECTED)
        {
          /* try to restore the RR_EST_REQ that was saved earlier */
          if (rr_gprs_edtm_restore_rr_est_req(RESTORE_AS_SAVED_MSG, gas_id))
          {
            /* tell GRR-CONTROL to block PS access (it will then process the saved message) */
            rr_grr_send_imsg_no_ps_access_req(
              FALSE,   // del_all_pdus
              NULL,    // confirm_cb_fn
              gas_id   // gas_id
            );
          }
          else
          {
            EDTM_ERROR_DEBUG("GMAC reject: unable to restore RR_EST_REQ", 0, 0, 0);
            rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);

            /* allow GRR to resume camped procedures */
            event_out = RR_EV_CAMPED_ON_CELL;
          }

          /* reset all EDTM establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);

          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        /* EV_MAC_IDLE
         * GMAC has gone idle during the establishment attempt
         * and so no response on the PACCH can be expected
         */
        else if (event_in == EV_MAC_IDLE)
        {
          /* try to restore the RR_EST_REQ that was saved earlier */
          if (rr_gprs_edtm_restore_rr_est_req(RESTORE_IN_RR_QUEUE, gas_id))
          {
            MSG_GERAN_MED_0_G("Restoring RR_EST_REQ in RR queue");
          }
          else
          {
            EDTM_ERROR_DEBUG("GMAC idle: unable to restore RR_EST_REQ", 0, 0, 0);
            rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
          }

          /* no output event here : ignored by GRR anyway */

          /* reset all EDTM establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);

          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        else if (event_in == EV_EDTM_T3196_EXPIRY)
        {
          /* the GRR guard timer has expired, GMAC has not responded */
          (void)rr_timer_cancel(T3196, gas_id);

          /* treat this as a GMAC reject, and start normal CS establishment */
          new_event = EV_EDTM_ESTABLISHMENT_REJECTED;
        }
        else if (event_in == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        break;

/****************************************************************************/
/*                                                                          */
/* DTM ESTABLISHMENT state - attempting to establish DTM via PACCH          */
/*                                                                          */
/****************************************************************************/
      case EDTM_DTM_ESTABLISHMENT:
        /* EV_DTM_ASSIGNMENT_COMMAND_CHECKED
         * the DTM Assignment has been validated and can be given to GMAC
         */
        if (event_in == EV_DTM_ASSIGNMENT_COMMAND_CHECKED)
        {
          new_event = rr_dtm_send_edtm_assignment_to_gmac(gas_id);
        }
        /* EV_DTM_ASSIGNMENT_ERROR
         * the DTM Assignment cannot be processed, or has been rejected by GMAC
         */
        else if (event_in == EV_DTM_ASSIGNMENT_ERROR)
        {
          if (rr_gprs_edtm_data_ptr->cs_request_sent == TRUE)
          {
            /* try to restore the RR_EST_REQ that was saved earlier */
            if (rr_gprs_edtm_restore_rr_est_req(RESTORE_AS_SAVED_MSG, gas_id))
            {
              /* suspend GPRS in GMM before blocking PS access and starting CS establishment */
              rr_gmm_suspend_gprs(gas_id);

              /* tell GRR-CONTROL to block PS access (it will then process the saved message) */
              rr_grr_send_imsg_no_ps_access_req(
                FALSE,   // del_all_pdus
                NULL,    // confirm_cb_fn
                gas_id   // gas_id
              );
            }
            else
            {
              EDTM_ERROR_DEBUG("GMAC reject: unable to restore RR_EST_REQ", 0, 0, 0);
              rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
            }
          }
          else
          {
            /* unsolicited assignment, respond to paging failure */
            rr_gprs_edtm_paging_failure(gas_id);
          }

          /* reset all EDTM establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);

          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        /* EV_DTM_ASSIGNMENT_COMPLETE
         * L1 has connected the physical channels for DTM
         */
        else if (event_in == EV_DTM_ASSIGNMENT_COMPLETE)
        {
          /* complete the RR processing of the new CS configuration */
          rr_dtm_start_using_new_channel_spec(gas_id);

          /* get connection establishment control ready (send first L2 message) */
          rr_gprs_edtm_connection_event(EV_EDTM_DTM_ASSIGNMENT_RECEIVED, gas_id);

          /* return an event that shifts RR into the connection pending state */
          event_out = EV_EDTM_ESTABLISHMENT_COMPLETED;
        }
        /* EV_MAC_IDLE - abnormal event in this state
         * GMAC has left packet transfer mode, so abort the establishment
         * note that this should only ever happen before L2 has established
         */
        else if (event_in == EV_MAC_IDLE)
        {
          EDTM_ERROR_DEBUG("GMAC idle received during EDTM establishment",0,0,0);

          /* full reset of RR connection establishment */
          rr_gprs_edtm_connection_event(EV_RESET_SOFTWARE, gas_id);

          if (rr_is_control_state(RR_CONNECTION_PENDING, gas_id))
          {
            /* push DTM control back to PS_ALLOWED */
            (void)rr_dtm_control(EV_EDTM_ESTABLISHMENT_ABORTED, NULL, gas_id);

            /* ask L2 to abnormally release */
            rr_send_mdl_release_req(SAPI0, DCCH, gas_id);

            /* Inform RR-CONN-RELEASE that the L2 link has been reset already */
            rr_conn_set_release_cause(RELEASE_CAUSE_L2_LINK_RESET, gas_id);

            /* this return event will take RR to DATA_TRANSFER */
            /* and the imminent DL_RELEASE_IND from L2 will    */
            /* then initiate a local end connection release    */
            event_out = EV_RR_EST_CNF;
          }
          else if (rr_gprs_edtm_data_ptr->cs_request_sent == TRUE)
          {
            /* try to restore the RR_EST_REQ that was saved earlier */
            if (rr_gprs_edtm_restore_rr_est_req(RESTORE_IN_RR_QUEUE, gas_id))
            {
              MSG_GERAN_MED_0_G("Restoring RR_EST_REQ in RR queue");
            }
            else
            {
              EDTM_ERROR_DEBUG("GMAC idle: unable to restore RR_EST_REQ", 0, 0, 0);
              rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
            }
          }
          else
          {
            /* unsolicited assignment, respond to paging failure */
            rr_gprs_edtm_paging_failure(gas_id);
          }

          /* reset all EDTM establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);

          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        /* EV_DTM_PERFORM_CSPS_RELEASE - abnormal event in this state
         * L1 has released the DTM channel assignment, so abort the establishment
         * note that this should only ever happen before L2 has established
         */
        else if (event_in == EV_DTM_PERFORM_CSPS_RELEASE)
        {
          EDTM_ERROR_DEBUG("L1 DTM release: abort EDTM establishment", 0, 0, 0);

          /* full reset of RR connection establishment */
          rr_gprs_edtm_connection_event(EV_RESET_SOFTWARE, gas_id);

          /* push DTM control back to PS_ALLOWED */
          (void)rr_dtm_control(EV_EDTM_ESTABLISHMENT_ABORTED, NULL, gas_id);

          /* ask L2 to abnormally release */
          rr_send_mdl_release_req(SAPI0, DCCH, gas_id);

          /* Inform RR-CONN-RELEASE that the L2 link has been reset already */
          rr_conn_set_release_cause(RELEASE_CAUSE_L2_LINK_RESET, gas_id);

          /* this return event will take RR to DATA_TRANSFER */
          /* and the imminent DL_RELEASE_IND from L2 will    */
          /* then initiate a local end connection release    */
          event_out = EV_RR_EST_CNF;

          /* reset all EDTM establishment data */
          rr_gprs_edtm_reset_est_data(TRUE, gas_id);

          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        else if (event_in == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          /* return to the NULL state */
          EDTM_STATE_TRANSITION(rr_gprs_edtm_data_ptr->state, EDTM_NULL, gas_id);
        }
        break;

      default:
        MSG_GERAN_ERROR_1_G("Invalid EDTM control state %d", rr_gprs_edtm_data_ptr->state);
        break;
    }

#ifdef DEBUG_RR_TRACE_MSG
    /* no old_state in this state machine: NOTE event conversion */
    if ((event_in != new_event) && (new_event != EV_NO_EVENT))
    {
      rr_store_trace_msg_buf(
        RR_EDTM_SM,
        new_event,
        (byte)rr_gprs_edtm_data_ptr->state,
        NULL,
        gas_id
      );
    }
#endif /* DEBUG_RR_TRACE_MSG */

    if (new_event != EV_NO_EVENT)
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }
  }

  return(event_out);
}
#else /* EDTM and DTM features are not defined, declare a dummy function */

/****************************************************************************
 *   Function name:  rr_gprs_edtm_supported
 *   --------------
 *
 *   Description: Placeholder function for builds that do not enabled E-DTM
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     0
 *   --------
 *
 *
 ****************************************************************************/
int rr_gprs_edtm_supported(void)
{
  return(0);
}

#endif /* FEATURE_GSM_EDTM */

/* EOF */

