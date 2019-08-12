/*============================================================================
  @file rr_ps_domain.c

  @brief This module contains functionality and data storage related to the
         'PS domain', which includes GMM and network interactions

                Copyright (c) 2012-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ps_domain.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_ps_domain.h"
#include "sys_stru.h"
#include "rr_gprs_defs.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_ps_if.h"
#include "rr_resel.h"
#include "rr_sys_info_ncell.h"
#include "rr_l1_send.h"
#include "rr_if.h"
#include "rr_ps_access.h"
#include "rr_mac_send.h"
#ifdef FEATURE_GSM_DTM
#include "rr_nv.h"
#endif // FEATURE_GSM_DTM
#include "sys.h"
#include "rr_multi_sim.h"
#include "rr_mode.h"
#include "rr_classmark.h"
#ifdef FEATURE_SGLTE
#include "rr_grr.h"
#include "rr_control.h"
#endif /* FEATURE_SGLTE */
#include "rr_general.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// GPRS Suspension cause value, see 44.018 10.5.2.47 Suspension Cause
typedef enum
{
  SUSP_CAUSE_MO_CALL           = 0x00,
  SUSP_CAUSE_LA_UPDATE         = 0x01,
  SUSP_CAUSE_MO_SMS            = 0x02,
  SUSP_CAUSE_OTHER_SDCCH       = 0x03,
  SUSP_CAUSE_MT_CS_CONN        = 0x05,
  SUSP_CAUSE_DTM_NOT_SUPPORTED = 0x06
} ota_suspension_cause_t;

typedef enum
{
  MM_NON_DRX_INACTIVE,      /* No RR_GMM_START_MM_NON_DRX_IND received from MM */
  MM_NON_DRX_PENDING,       /* RR_GMM_START_MM_NON_DRX_IND received from MM */
  MM_NON_DRX_ACTIVE         /* MPH_START_MM_NON_DRX_IND sent to L1 */
} mm_non_drx_status_t;

typedef struct
{
  struct
  {
    boolean                gprs_service_req;
    gmm_llc_if_state_type  state;
    uint32                 tlli;
    uint32                 old_tlli;
    boolean                negotiated_ready_timer_zero;
    mm_non_drx_status_t    mm_non_drx_status;
    boolean                gprs_suspended;
    boolean                resumption_ind;
    boolean                cell_update_required;
    boolean                pending_ms_ra_caps;
    boolean                ps_blocked;
  }                      gmm;
  struct
  {
    boolean                gprs_suspended;
    LAI_T                  gprs_suspension_lai;
    byte                   gprs_suspension_rac;
  }                      netw;
} rr_ps_domain_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_ps_domain_data_t rr_ps_domain_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_domain_data_t*
 */
rr_ps_domain_data_t *rr_ps_domain_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_ps_domain_data[as_index];
}

/*!
 * \brief Returns the flag which indicates if Radio Access Capabilities received should be forwarded to LTE.
 * 
 * \param gas_id (in)
 * 
 * \return boolean - TRUE if the message should be forwarded, FALSE otherwise
 */
static boolean rr_gmm_get_gprs_ra_cap_expected(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return(rr_ps_domain_data_ptr->gmm.pending_ms_ra_caps);
}

/*!
 * \brief Constructs and sends a GPRS SUSPENSION message to the network.
 *
 *  The value used for TLLI is the current TLLI as received from GMM ("newTLLI").
 *  The value used for PLMN ID, LAC & RAC are those as previously stored in this module.
 *  The value used for Suspension Cause is that supplied to the function.
 *
 * \param ota_suspension_cause
 * \param gas_id
 */
static void send_gprs_suspension_msg(
  ota_suspension_cause_t ota_suspension_cause,
  const gas_id_t gas_id
)
{
  byte msg_len = 0;
  byte layer3_message[N201_DCCH];
  uint32 local_tlli;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  memset(layer3_message, 0, N201_DCCH);

  /* Encode the Protocol Discriminator and the Skip Indicator */
  layer3_message[msg_len++] = RADIO_RESSOURCES;

  /* Encode the message type. */
  layer3_message[msg_len++] = GPRS_SUSPENSION;

  /* Add the TLLI */
  local_tlli = rr_gmminfo_get_tlli(gas_id);

  layer3_message[msg_len++] = (byte) (local_tlli >> 24);
  layer3_message[msg_len++] = (byte) ((local_tlli >> 16) & 0xFF);
  layer3_message[msg_len++] = (byte) ((local_tlli >> 8) & 0xFF);
  layer3_message[msg_len++] = (byte) (local_tlli & 0xFF);

  /***************************************/
  /* Add the Routing Area Identification */
  /***************************************/

  /* Octet 2 - Bits 5-8 are MCC digit 2 and bits 1-4 are MCC digit 1 */
  layer3_message[msg_len++] = rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[0];

  /* Octet 3 - Bits 5-8 are MNC digit 3 and bits 1-4 are MCC digit 3 */
  layer3_message[msg_len++] = rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[1];

  /* Octet 4 - Bits 5-8 are MNC digit 2 and bits 1-4 are MNC digit 1 */
  layer3_message[msg_len++] = rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[2];

  /* Octets 5 & 6 - LAC */
  layer3_message[msg_len++] = (byte)
    ((rr_ps_domain_data_ptr->netw.gprs_suspension_lai.location_area_code & 0xFF00) >> 8);

  layer3_message[msg_len++] = (byte)
    (rr_ps_domain_data_ptr->netw.gprs_suspension_lai.location_area_code & 0x00FF);

  /* Octet 7 - routing_area_code */
  layer3_message[msg_len++] = rr_ps_domain_data_ptr->netw.gprs_suspension_rac;

  /* Add the Suspension Cause */
  layer3_message[msg_len++] = (byte) ota_suspension_cause;

  MSG_GERAN_HIGH_0_G("OMsg(NW,GPRS_SUSPENSION)");

  (void) rr_send_dl_data_req(
    layer3_message,   // layer3_message
    msg_len,          // length
    SAPI0,            // SAPI
    DCCH,             // l2_channel_type
    gas_id            // gas_id
  );

  rr_ps_domain_data_ptr->netw.gprs_suspended = TRUE;

} /* end send_gprs_suspension_msg() */

/*!
 * \brief Returns TRUE if the current service domain allows PS (i.e. not CS-only).
 *
 * \param gas_id
 *
 * \return boolean
 */
static boolean rr_service_domain_allows_ps(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->service_domain != SYS_SRV_DOMAIN_CS_ONLY)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* end rr_service_domain_allows_ps() */

/*!
 * \brief Returns TRUE if the given service domain allows PS (i.e. not CS-only).
 *
 * \param service_domain
 * \param gas_id
 *
 * \return boolean
 */
static boolean rr_check_service_domain_allows_ps(
  sys_srv_domain_e_type service_domain,
  const gas_id_t gas_id
)
{
  if (service_domain != SYS_SRV_DOMAIN_CS_ONLY)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* end rr_check_service_domain_allows_ps() */


/*!
 * \brief If the service domain is CS+PS, this is changed to CS-only.
 *
 * \param gas_id
 */

#ifdef FEATURE_SGLTE

static void rr_remove_ps_from_service_domain(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    MSG_GERAN_MED_0_G("service domain changed to CS-only");
    rr_store_service_domain(SYS_SRV_DOMAIN_CS_ONLY,gas_id);
  }

} /* end rr_remove_ps_from_service_domain() */

#endif /*FEATURE_SGLTE*/


/*!
 * \brief Call-back function which sends RR_PLMN_SELECT_CNF to MM.
 *
 * \param gas_id
 */
static void send_plmn_select_cnf_cb_fn(const gas_id_t gas_id)
{
  // Send a RR_PLMN_SELECT_CNF indicating success
  rr_send_plmn_select_cnf_success(gas_id);

} /* end send_plmn_select_cnf_cb_fn() */

/*!
 * \brief Call-back function which sends RR_GMM_GPRS_SERVICE_CNF to MM.
 *
 * \param gas_id
 */
static void send_gprs_service_cnf_cb_fn(const gas_id_t gas_id)
{
  // Send a RR_GMM_GPRS_SERVICE_CNF
  rr_send_rr_gmm_gprs_service_cnf(gas_id);

} /* end send_gprs_service_cnf_cb_fn() */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Call-back function which sends RR_PS_ABORT_CNF to MM.
 *
 * \param gas_id
 */
static void send_ps_abort_cnf_cb_fn(const gas_id_t gas_id)
{
  // Send a RR_PS_ABORT_CNF
  rr_ms_send_rr_ps_abort_cnf(gas_id);

} /* send_ps_abort_cnf_cb_fn */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Indicates if a valid TLLI is assigned.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if a valid TLLI is assigned; FALSE otherwise
 */
boolean rr_ps_tlli_is_assigned(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return(TLLI_IS_VALID(rr_ps_domain_data_ptr->gmm.tlli) ||
         TLLI_IS_VALID(rr_ps_domain_data_ptr->gmm.old_tlli));
}

/*!
 * \brief Sends a GPRS SUSPENSION message to the network following a W2G handover.
 *
 *  The location information included in the message is the 'registered_rai' given in the last SIM update.
 */
void grr_send_gprs_suspension_ota_after_w2g_ho(const gas_id_t gas_id)
{
  /* Send the GPRS SUSPENSION message OTA to the network */
  send_gprs_suspension_msg(SUSP_CAUSE_MO_CALL, gas_id);

} /* grr_send_gprs_suspension_ota_after_w2g_ho */

/*!
 * \brief Sends a GPRS SUSPENSION message to the network.
 *
 *  The value of 'Suspension Cause' included in the  message is based on the function parameter
 *  'establishment_cause'.
 *
 * \param establishment_cause
 */
void grr_send_gprs_suspension_ota(
  establishment_cause_T establishment_cause,
  const gas_id_t gas_id
)
{
  ota_suspension_cause_t ota_suspension_cause;

#ifdef FEATURE_GSM_DTM
  /*
  44.018 3.4.25.3 Dual transfer mode not supported
  The GPRS suspension procedure shall be used to suspend GPRS services:
  a)  when the mobile station in a class A mode of operation is handed over to a cell
    where the support of Class A mode of operation is not possible (e.g. a DTM mobile
    station entering a cell not supporting DTM)
  b)  when the GPRS attached mobile station is in a cell that does not support DTM and a
    circuit switched service is initiated.
  In case a), when the mobile station concludes that DTM is not supported in the new cell
  after the handover procedure is completed, it shall initiate the GPRS suspension
  procedure by sending a GPRS SUSPENSION REQUEST message with the suspension cause set to
  "DTM not supported in the cell".
  In case b), the GPRS suspension procedure is initiated by the mobile station by sending
  a GPRS SUSPENSION REQUEST message with the suspension cause set to "DTM not supported in
  the cell".
  */
  if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID) &&
      (rr_cell_supports_dtm(gas_id) == FALSE) &&
      (rr_gmminfo_get_gmm_status(gas_id) != GMM_IDLE_STATE))
  {
    ota_suspension_cause = SUSP_CAUSE_DTM_NOT_SUPPORTED;
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
    switch ( establishment_cause )
    {
      case MM_RR_SHORT_MESSAGE:
        ota_suspension_cause = SUSP_CAUSE_MO_SMS;
        break;

      case MM_RR_LOCATION_UPDATE:
        ota_suspension_cause = SUSP_CAUSE_LA_UPDATE;
        break;

      case MM_RR_SUPPLEMENTARY_SERVICE:
      case MM_RR_IMSI_DETACH:
        ota_suspension_cause = SUSP_CAUSE_OTHER_SDCCH;
        break;

      case MM_RR_ANSWER_TO_PAGING:
        ota_suspension_cause = SUSP_CAUSE_MT_CS_CONN;
        break;

      case MM_RR_MO_FULL_SPEECH:
      case MM_RR_MO_HALF_SPEECH:
      case MM_RR_MO_DATA_9600:
      case MM_RR_MO_DATA_4800:
      case MM_RR_MO_DATA_2400:
      case MM_RR_EMERGENCY_CALL:
      case MM_RR_CALL_REEST:
      default:
        ota_suspension_cause = SUSP_CAUSE_MO_CALL ;
    }
  }

  /* Send GPRS SUSPENSION OTA with given cause */
  send_gprs_suspension_msg(ota_suspension_cause, gas_id);

  /* Set the default resumption indication to GPRS Resumption Failure. This is in */
  /* case of abnormal call release, e.g. Radio Link Failure */
  grr_set_gprs_resumption_indicator(FALSE, gas_id);

} /* end grr_send_gprs_suspension_ota() */

/*!
 * \brief Returns TRUE if a GPRS SUSPENSION message should be sent to the network; FALSE otherwise.
 *
 *  A GPRS SUSPENSION message is sent unless
 *   - GMM state is IDLE
 *   - Cell does not support GPRS (if known)
 *   - DTM is enabled in NV and supported in the cell
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean grr_determine_if_gprs_suspension_should_be_sent_ota(const gas_id_t gas_id)
{
  boolean gprs_suspension_required;

  if ((rr_gmminfo_get_gmm_status(gas_id) == GMM_IDLE_STATE) ||
      (rr_cell_si_valid(gas_id) && (!rr_cell_supports_gprs(gas_id))))
  {
    gprs_suspension_required = FALSE;
  }
  else
  {
#ifdef FEATURE_GSM_DTM
    if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID) && rr_cell_supports_dtm(gas_id))
    {
      gprs_suspension_required = FALSE;
    }
    else
    {
      gprs_suspension_required = TRUE;
    }
#else
    gprs_suspension_required = TRUE;
#endif
  }

  if (gprs_suspension_required)
  {
    MSG_GERAN_MED_0_G("OTA GPRS SUSPENSION required");
  }
  else
  {
    MSG_GERAN_MED_0_G("OTA GPRS SUSPENSION not required");
  }

  return gprs_suspension_required;

} /* end grr_determine_if_gprs_suspension_should_be_sent_ota() */

/*!
 * \brief Sets the PLMN ID, LAC & RAC to be included in the next GPRS SUSPENSION message to the network.
 *
 * \param registered_rai
 * \param gas_id
 */
void grr_set_ota_gprs_suspension_parameters(
  routing_area_id_T registered_rai,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  /*
  registered_rai.location_area_code:
    The LAC is 16 bits, split over 2 octets
    Bit 8 of octet 0 is the MSB and bit 1 of octet 1 the LSB
    (see 24.008 Table 10.5.148)
  */
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id = registered_rai.plmn_id;
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.location_area_code =
    (registered_rai.location_area_code.lac[0] << 8) + registered_rai.location_area_code.lac[1];
  rr_ps_domain_data_ptr->netw.gprs_suspension_rac = registered_rai.routing_area_code;

} /* end grr_set_ota_gprs_suspension_parameters() */

/*!
 * \brief Stores the state of the GPRS Resumption Indicator received in a CHANNEL RELEASE message.
 *
 *  This is then passed to GMM when RR_GMM_GPRS_RESUMPTION_IND is next sent.
 *
 * \param gprs_resumption_ind
 * \param gas_id
 */
void grr_set_gprs_resumption_indicator(
  boolean gprs_resumption_ind,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  rr_ps_domain_data_ptr->gmm.resumption_ind = gprs_resumption_ind;

} /* end grr_set_gprs_resumption_parameters() */


#ifdef FEATURE_GSM_DTM
/*!
 * \brief Flags that a Cell Update is required when GPRS in GMM is next resumed.
 *
 * \param gas_id
 */
void grr_gprs_cell_update_required_when_gmm_resumed(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  rr_ps_domain_data_ptr->gmm.cell_update_required = TRUE;

} /* end grr_gprs_cell_update_required_when_gmm_resumed() */
#endif // FEATURE_GSM_DTM

/*!
 * \brief Returns TRUE if GPRS in GMM is currently suspended; FALSE otherwise.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean grr_is_gmm_suspended(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.gprs_suspended;

} /* end is_gmm_suspended() */

/*!
 * \brief Suspends GPRS in GMM, if currently not suspended.
 *
 * \param gas_id
 */
void rr_gmm_suspend_gprs(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  // No need to suspend GMM if the service domain is CS-only
  if (rr_service_domain_allows_ps(gas_id))
  {
    // If GMM is already suspended, don't send another message
    if (!rr_ps_domain_data_ptr->gmm.gprs_suspended)
    {
      rr_ps_domain_data_ptr->gmm.gprs_suspended = TRUE;
      rr_send_grr_gmm_gprs_suspension_ind(gas_id);
    }
  }
}

/*!
 * \brief Resumes GPRS in GMM, if currently suspended.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_gmm_resume_gprs(const gas_id_t gas_id)
{
  boolean resumption_ind;
  boolean gmm_resumed = FALSE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  // Only resume GMM if it is currently suspended
  if (rr_ps_domain_data_ptr->gmm.gprs_suspended)
  {
#ifdef FEATURE_DUAL_SIM
    // Don't resume GMM if PS is currently blocked by RR_PS_ABORT_REQ
    if (rr_ps_domain_data_ptr->gmm.ps_blocked ||
        (RR_PS_ABORT_ACTIVE_WILL_RESUME == rr_ms_ps_abort_process_active(gas_id)))
    {
      MSG_GERAN_HIGH_0_G("GMM suspended but PS blocked - don't resume");
    }
    else
#endif // #ifdef FEATURE_DUAL_SIM
    {
      rr_ps_domain_data_ptr->gmm.gprs_suspended = FALSE;

      /*
      44.018 3.4.13.1.1 Channel release procedure initiation in dedicated mode
      For a MS supporting the "GPRS" option, the following additional procedures also
      apply:
      - The CHANNEL RELEASE message may include the IE GPRS Resumption. If the GPRS
      Resumption IE indicates that the network has resumed GPRS services, RR shall
      indicate GPRS resumption complete to MM. If the GPRS Resumption IE indicates that
      the network has not successfully resumed GPRS services, RR shall indicate a RR
      GPRS resumption failure to MM.
      - If the UE has performed the GPRS suspension procedure and the GPRS Resumption IE
      is not included in the message, RR shall indicate a RR GPRS resumption failure to
      MM.
      - If the UE has not performed the GPRS suspension procedure and the GPRS Resumption
      IE is not included in the message, the MS shall perform the RR connection release
      procedure as normal
      */

      /*
      If a GPRS SUSPENSION message was sent OTA, then the netw_suspended flag will be
      TRUE. If not, then the value of resumption_ind can be ignored, and a successful
      resumption indicated to GMM. This will ensure that no RAU is triggered.
      */
      if (rr_ps_domain_data_ptr->netw.gprs_suspended)
      {
        resumption_ind = rr_ps_domain_data_ptr->gmm.resumption_ind;

        /*
        If is assumed here that GMM will now perform a RAU, which will resume GPRS in the
        network. Of course, the resetting of the netw_suspended flag is not linked to the
        successful completion of this RAU, but this isn't really possible, as there is no
        way to tell when a RAU has been completed.
        */
        rr_ps_domain_data_ptr->netw.gprs_suspended = FALSE;
      }
      else
      {
        resumption_ind = TRUE;
      }

#ifdef FEATURE_GSM_DTM
      {
        boolean cell_update_required;

        // If a RAU is going to be triggered due to GPRS resumption failure (resumption_ind = 0), then
        // the Cell Update will not be required. So only allow cell_update_required to be set if
        // GPRS resumption is successful (resumption_ind = 1)
        // Also, it is only applicable if GMM is in READY state
        cell_update_required =
          resumption_ind &&
          rr_ps_domain_data_ptr->gmm.cell_update_required &&
          (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE);

        MSG_GERAN_HIGH_3_G("Resuming GPRS in GMM (resumption_ind=%d, cell_update_required=%d [%d])",
                 (int)resumption_ind,
                 (int)cell_update_required,
                 (int)rr_ps_domain_data_ptr->gmm.cell_update_required);

        rr_send_grr_gmm_gprs_resumption_ind(resumption_ind, cell_update_required, gas_id);
      }
#else
      MSG_GERAN_HIGH_1_G("Resuming GPRS in GMM (resumption_ind=%d)",resumption_ind);

      rr_send_grr_gmm_gprs_resumption_ind(resumption_ind, gas_id);
#endif /* FEATURE_GSM_DTM */

      /*
      GMM has now been resumed, so re-initialise the resumption indicator.
      This is important, as if GMM is suspended for an activity which doesn't involve a
      connection establishment, e.g. PLMN search, then upon completion, successful
      resumption should be indicated to avoid unnecessary RAUs
      */
      rr_ps_domain_data_ptr->gmm.resumption_ind = TRUE;
#ifdef FEATURE_GSM_DTM
      rr_ps_domain_data_ptr->gmm.cell_update_required = FALSE;
#endif // FEATURE_GSM_DTM

      gmm_resumed = TRUE;
    }
  }

  return gmm_resumed;
}

/*!
 * \brief Called when RR_GMM_GPRS_SERVICE_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_gprs_service_req(
  rr_gmm_gprs_service_req_T *msg_ptr,
  const gas_id_t gas_id
)
{
  boolean send_ps_access_req = FALSE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);

  // If PS is blocked, ignore this GPRS service request
  if (rr_ps_domain_data_ptr->gmm.ps_blocked == FALSE)
  {
    // A TLLI should always be assigned before any request for GPRS service is made
    if (rr_ps_tlli_is_assigned(gas_id))
    {
      // The PS domain is active
      rr_ps_domain_data_ptr->gmm.gprs_service_req = TRUE;

      // If GMM is suspended, then a request for GPRS services is not expected. It may have been received
      // due to a race between it and suspending GMM
      if (!rr_ps_domain_data_ptr->gmm.gprs_suspended)
      {
#ifdef FEATURE_GSM_DTM
        if (rr_mac_ul_tbf_est_is_allowed(gas_id) || rr_mac_gttp_transfer_is_allowed(gas_id))
#else
        if (rr_mac_ul_tbf_est_is_allowed(gas_id))
#endif
        { 
#ifdef FEATURE_SGLTE
          if (rr_is_sglte_x2g_resel_in_progress(gas_id) == FALSE &&
              grr_is_sglte_x2g_resel_in_progress(gas_id) == FALSE)
#endif /* FEATURE_SGLTE */
          {
            // There is nothing blocking PS access, so it should be granted
            // Check if it is currently blocked or already enabled
            if (rr_mac_ps_access_blocked(gas_id))
            {
              // Send a PS access request into RR
              send_ps_access_req = TRUE;
            }
          }
#ifdef FEATURE_SGLTE
          else
          {
            MSG_GERAN_HIGH_0_G("Blocking PS service while SGLTE X2G in progress");
          }
#endif /* FEATURE_SGLTE */
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("No TLLI assigned, GPRS service request ignored");
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("PS is blocked by NAS, GPRS service request ignored");

    /**
     * If PS is blocked by NAS (via RR_PS_ABORT_REQ), then GMM should have been suspended. It is possible that 
     * GERAN was stopped by NAS and re-started, in which case the GMM suspension state is lost. However, the 
     * PS blocking control is supposed to remain until NAS sends RR_PS_RESUME_IND. 
     * Therefore, re-instate that now by suspending GMM. 
     */
    if (rr_ps_domain_active(gas_id) && !rr_ps_domain_data_ptr->gmm.gprs_suspended)
    {
      MSG_GERAN_HIGH_0_G("GMM should be suspended while PS blocked");
      rr_gmm_suspend_gprs(gas_id);
    }
  }

  if (send_ps_access_req)
  {
    // Send a PS access request into RR.
    // This will be handled according to the RR state. If appropriate, PS access will be enabled. If not, it
    // won't be. But either way, a RR_GMM_GPRS_SERVICE_CNF should be sent to GMM afterwards.
    rr_send_imsg_ps_access_req(
      FALSE,                         // purge_gmm_signalling
      send_gprs_service_cnf_cb_fn,   // confirm_cb_fn
      gas_id                         // gas_id
    );
  }
  else
  {
    // Although no action was required, a confirmation should always be sent to GMM
    rr_send_rr_gmm_gprs_service_cnf(gas_id);
  }
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Called when RR_PS_ABORT_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_ps_abort_req(
  rr_ps_abort_req_T *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);

  if (msg_ptr->abort_cause == MM_RR_DUAL_STANDBY_CS_MO_CALL)
  {
    // PS is blocked for the duration of a timer
    rr_ms_process_ps_abort_req(gas_id);
  }
  else
  {
    // GPRS is suspended in GMM for the duration of the PS abort
    rr_gmm_suspend_gprs(gas_id);

    // PS is blocked until an explicit resume request (RR_PS_RESUME_IND)
    rr_ps_domain_data_ptr->gmm.ps_blocked = TRUE;

    // Request RR-CONTROL to block PS access
    rr_send_imsg_no_ps_access_req(
      FALSE,                    // del_all_pdus
      send_ps_abort_cnf_cb_fn,  // confirm_cb_fn
      gas_id
    );
  }
}
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Called when RR_PS_RESUME_IND is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_ps_resume_ind(
  rr_ps_resume_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  (void)msg_ptr;  // not used

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);

  // Flag PS as no longer blocked
  rr_ps_domain_data_ptr->gmm.ps_blocked = FALSE;

  (void)rr_gmm_resume_gprs(gas_id);
}
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM

/*!
 * \brief Called when RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_gprs_tlli_assignment_req(
  rr_gmm_gprs_tlli_assignment_req_T *msg_ptr,
  const gas_id_t gas_id
)
{
  uint32 new_tlli;
  uint32 old_tlli;
  rrps_store_update_data_t *store_update_ptr;
  boolean tlli_is_valid;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;
  boolean service_domain_allows_ps;
  boolean ps_is_allowed;


  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  new_tlli = (uint32) msg_ptr->newTLLI;
  old_tlli = (uint32) msg_ptr->oldTLLI;

  // Check if current TLLI is valid
  tlli_is_valid = (TLLI_IS_VALID(rr_ps_domain_data_ptr->gmm.tlli) ||
                   TLLI_IS_VALID(rr_ps_domain_data_ptr->gmm.old_tlli));

  // Check if current service domain allows PS
  service_domain_allows_ps = rr_service_domain_allows_ps(gas_id);

  // Store local copy
  rr_ps_domain_data_ptr->gmm.tlli     = new_tlli;
  rr_ps_domain_data_ptr->gmm.old_tlli = old_tlli;

  // Update Public Store */
  store_update_ptr = rr_get_rrps_store_update(gas_id);
  if (store_update_ptr != NULL)
  {
    store_update_ptr->new_tlli = new_tlli;
    store_update_ptr->old_tlli = old_tlli;
    rr_write_public_store(RRPS_UPD_GMM_TLLI, store_update_ptr, gas_id);
    rr_free_rrps_store_update(store_update_ptr, gas_id);
  }

  // Store new service domain setting, explicit indication from NAS
  rr_store_service_domain(msg_ptr->service_domain, gas_id);

  // Assume that PS is allowed (and then check new settings)
  ps_is_allowed = TRUE;

  // Check if new service domain blocks PS
  if (rr_service_domain_allows_ps(gas_id) == FALSE)
  {
    MSG_GERAN_HIGH_1_G("PS blocked by service domain (was %d)", (int)service_domain_allows_ps);
    ps_is_allowed = FALSE;
  }

  // Check if TLLI unassignment blocks PS
  if (TLLI_IS_UNASSIGNED(new_tlli) && TLLI_IS_UNASSIGNED(old_tlli))
  {
    MSG_GERAN_HIGH_1_G("PS blocked by unassigned TLLI (was %d)", (int)tlli_is_valid);
    ps_is_allowed = FALSE;
  }

  // If moving from a valid TLLI to an unassigned TLLI, then send RR_IMSG_NO_PS_ACCESS_REQ
  if (tlli_is_valid)
  {
    if (ps_is_allowed == FALSE)
    {
      rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;

      // PS access should be blocked - but only send the request if this is not currently the case
      if (rr_mac_ps_access_granted(gas_id))
      {
        // Send a no PS access request into RR
        rr_send_imsg_no_ps_access_req(
          TRUE,    // del_all_pdus
          NULL,    // confirm_cb_fn
          gas_id   // gas_id
        );
      }
      else
      {
        // Ensure MAC is blocked with 'del all' cause
        rr_mac_ps_access_blocked_del_all(gas_id);
      }
    }
  }

} /* end rr_ps_process_gmm_gprs_tlli_assignment_req() */

/*!
 * \brief Called when RR_GMM_GPRS_STATE_CHANGE_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_gprs_state_change_req(
  rr_gmm_gprs_state_change_req_T *msg_ptr,
  const gas_id_t gas_id
)
{
  gmm_llc_if_state_type new_state;
  rrps_store_update_data_t *store_update_ptr;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  new_state = msg_ptr->new_state;

  if ((rr_ps_domain_data_ptr->gmm.state == GMM_READY_STATE) &&
      (new_state != GMM_READY_STATE))
  {
    (void) rr_resel_control(EV_GMM_LEFT_READY_STATE, NULL, gas_id);
  }

  // Store local copy
  rr_ps_domain_data_ptr->gmm.state = new_state;

  // Update Public Store */
  store_update_ptr = rr_get_rrps_store_update(gas_id);
  if (store_update_ptr != NULL)
  {
    store_update_ptr->gmm_ready_state = new_state;
    rr_write_public_store(RRPS_UPD_GMM_STATUS, store_update_ptr, gas_id);
    rr_free_rrps_store_update(store_update_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_get_rrps_store_update() returned NULL");
  }

} /* end rr_ps_process_gmm_gprs_state_change_req() */

/*!
 * \brief Called when RR_GMM_READY_TIMER_STATE_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_ready_timer_state_req(
  rr_gmm_ready_timer_state_req_T *msg_ptr,
  const gas_id_t gas_id
)
{
  boolean negotiated_ready_timer_zero;
  rrps_store_update_data_t *store_update_ptr;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  negotiated_ready_timer_zero = msg_ptr->negotiated_ready_timer_zero;

  // Store local copy
  rr_ps_domain_data_ptr->gmm.negotiated_ready_timer_zero = negotiated_ready_timer_zero;

  // Update Public Store */
  store_update_ptr = rr_get_rrps_store_update(gas_id);
  if (store_update_ptr != NULL)
  {
    store_update_ptr->negotiated_ready_timer_zero = negotiated_ready_timer_zero;
    rr_write_public_store(RRPS_UPD_GMM_READY_TIMER, store_update_ptr, gas_id);
    rr_free_rrps_store_update(store_update_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_get_rrps_store_update() returned NULL");
  }

} /* end rr_ps_gmm_ready_timer_state_req() */

/*!
 * \brief Called when RR_GMM_GPRS_MS_RA_CAP_IE_IND is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_gprs_ms_ra_cap_ie_ind(
  rr_gmm_gprs_ms_ra_cap_ie_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  uint16 length_bits;
  uint8 *bit_stream;
  rrps_store_update_data_t *store_update_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  if (rr_gmm_get_gprs_ra_cap_expected(gas_id))
  {
#ifdef FEATURE_LTE
    /* LTE may be waiting for an update response */
    rr_send_lte_ps_capabilities_rsp(msg_ptr, gas_id);
#endif /* FEATURE_LTE */
    rr_gmm_set_gprs_ra_cap_expected(FALSE, gas_id);
  }

  length_bits = msg_ptr->ms_ra_cap_ie_len_bits;
  bit_stream = msg_ptr->ms_ra_cap_ie;

  /* Update Public Store */
  store_update_ptr = rr_get_rrps_store_update(gas_id);
  if (store_update_ptr != NULL)
  {
    store_update_ptr->ms_ra_cap_ie.length_bits = length_bits;
    store_update_ptr->ms_ra_cap_ie.ms_ra_cap_ie = bit_stream;
    rr_write_public_store(RRPS_UPD_GMM_MS_RA_CAP_IE, store_update_ptr, gas_id);
    rr_free_rrps_store_update(store_update_ptr, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_get_rrps_store_update() returned NULL");
  }

} /* end rr_ps_process_gmm_gprs_ms_ra_cap_ie_ind() */

/*!
 * \brief Called when RR_GMM_START_MM_NON_DRX_IND is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_start_mm_non_drx_ind(
  rr_gmm_start_mm_non_drx_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (rr_ps_domain_data_ptr->gmm.mm_non_drx_status == MM_NON_DRX_INACTIVE)
  {
    // When RACHing for GPRS IA starts, L1 will be put in MM non-DRX mode
    rr_ps_domain_data_ptr->gmm.mm_non_drx_status = MM_NON_DRX_PENDING;
  }

} /* end rr_ps_process_gmm_start_mm_non_drx_ind() */

/*!
 * \brief Called when RR_GMM_STOP_MM_NON_DRX_IND is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
void rr_ps_process_gmm_stop_mm_non_drx_ind(
  rr_gmm_stop_mm_non_drx_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  /* Clear MM non-DRX mode in L1 (if previously instructed to enter this mode) */
  rr_gprs_clear_l1_in_mm_non_drx(gas_id);
  rr_gprs_reset_ncell_sysinfo_manager(gas_id);

} /* end rr_ps_process_gmm_stop_mm_non_drx_ind() */

/*!
 * \brief Called when RR_PLMN_SELECT_REQ is received from MM and the service domain is changed.
 *
 * \param old_service_domain
 * \param new_service_domain
 * \param gas_id
 */
void rr_ps_plmn_select_req_service_domain_changed(
  sys_srv_domain_e_type old_service_domain,
  sys_srv_domain_e_type new_service_domain,
  const gas_id_t gas_id
)
{
  boolean send_plmn_select_cnf = TRUE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (old_service_domain == new_service_domain)
  {
    MSG_GERAN_HIGH_1_G("Service domain unchanged (%d)",(int)new_service_domain);
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Service domain changed (%d -> %d)",(int)old_service_domain,(int)new_service_domain);

    // Check if new service domain blocks PS
    if (!rr_check_service_domain_allows_ps(new_service_domain, gas_id))
    {
      // PS access should be blocked - but only send the request if this is not currently the case
      if (rr_mac_ps_access_granted(gas_id))
      {
        // Send a no PS access request into RR
        rr_send_imsg_no_ps_access_req(
          TRUE,                          // del_all_pdus
          send_plmn_select_cnf_cb_fn,    // confirm_cb_fn
          gas_id                         // gas_id
        );

        // The call-back function will send RR_PLMN_SELECT_CNF
        send_plmn_select_cnf = FALSE;
      }
      else
      {
        // Ensure MAC is blocked with 'del all' cause
        rr_mac_ps_access_blocked_del_all(gas_id);
      }

      // The PS domain is no longer active
      rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;
    }
  }

  if (send_plmn_select_cnf)
  {
    rr_send_plmn_select_cnf_success(gas_id);
  }

} /* end rr_ps_process_plmn_select_req() */

#ifdef FEATURE_SGLTE

/*!
 * \brief Call-back function which sends RR_STOP_GSM_MODE_CNF(MM_AS_SUCCESS) to MM.
 * 
 * \param gas_id (in)
 */
void rr_sglte_confirm_rat_change_cb_fn(gas_id_t gas_id)
{
  MSG_GERAN_HIGH_0_G("rr_sglte_confirm_rat_change_cb_fn() sending RR_STOP_GSM_MODE_CNF / RR_SERVICE_IND");
  rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
  grr_update_mm_with_rr_service_ind(gas_id);
}

/*!
 * \brief Called when RR_CHANGE_MODE_IND is recieved from MM.
 * 
 * \param old_service_domain (in)
 * \param new_service_domain (in)
 * \param gas_id (in)
 */
void rr_ps_handle_rr_change_mode_ind_service_domain_changed(
  sys_srv_domain_e_type old_service_domain,
  sys_srv_domain_e_type new_service_domain,
  const gas_id_t        gas_id
)
{
  boolean send_service_ind = TRUE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (old_service_domain == new_service_domain)
  {
    MSG_GERAN_HIGH_1_G("Service domain unchanged (%d)",(int)new_service_domain);
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Service domain changed (%d -> %d)",(int)old_service_domain,(int)new_service_domain);

    // Check if new service domain blocks PS
    if (!rr_check_service_domain_allows_ps(new_service_domain, gas_id))
    {
      // PS access should be blocked - but only send the request if this is not currently the case
      if (rr_mac_ps_access_granted(gas_id))
      {
        // Send a no PS access request into RR
        rr_send_imsg_no_ps_access_req(
          TRUE,                         // del_all_pdus
          grr_update_mm_with_rr_service_ind,    // confirm_cb_fn
          gas_id
        );

        // The call-back function will send RR_PLMN_SELECT_CNF
        send_service_ind = FALSE;
      }
      else
      {
        // Ensure MAC is blocked with 'del all' cause
        rr_mac_ps_access_blocked_del_all(gas_id);
      }

      // The PS domain is no longer active
      rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;
    }
  }

  if (send_service_ind)
  {
    grr_update_mm_with_rr_service_ind(gas_id);
  }
}

/*!
 * \brief Called when RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE) is received from MM.
 *        Note: This is an implicit removal of PS service.
 * 
 * \param cb_fn (in)
 * \param gas_id (in)
 */
void rr_ps_sglte_handle_ps_removal(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
)
{
  boolean sglte_confirm_rat_change = TRUE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  MSG_GERAN_HIGH_0_G("SGLTE removing PS from service domain");

  // The PS domain is no longer active
  rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;

  // PS is removed from the service domain as we are switching to SGLTE CS-only/
  // GSM-only mode.
  rr_remove_ps_from_service_domain(gas_id);

  // PS access should be blocked - but only send the request if this is not currently the case
  if (rr_mac_ps_access_granted(gas_id))
  {
    // Send a no PS access request into RR
    rr_send_imsg_no_ps_access_req(
      TRUE, // del_all_pdus
      cb_fn, // confirm_cb_fn
      gas_id
    );

    // The call-back function will send RR_PLMN_SELECT_CNF
    sglte_confirm_rat_change = FALSE;
  }
  else
  {
    // Ensure MAC is blocked with 'del all' cause
    rr_mac_ps_access_blocked_del_all(gas_id);
  }

  if (sglte_confirm_rat_change)
  {
    cb_fn(gas_id);
  }

} /* end rr_ps_sglte_handle_ps_removal() */

#endif /* FEATURE_SGLTE */

/*!
 * \brief Returns the GMM status as indicated by RR_GMM_GPRS_STATE_CHANGE_REQ.
 *
 * \param gas_id
 *
 * \return gmm_llc_if_state_type
 */
gmm_llc_if_state_type rr_gmminfo_get_gmm_status(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.state;

} /* end rr_gmminfo_get_gmm_status() */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Determines whether any GERAN subscription is in GMM READY state.
 * 
 * \param gas_id_ptr (out) - GAS ID of subscription in ready state
 * 
 * \return boolean - TRUE if any subscription is in GMM READY state, FALSE otherwise
 */
boolean grr_any_subscription_in_ready_state(gas_id_t *gas_id_ptr)
{
  gas_id_t gas_id;

  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (rr_mode_gsm_active_or_starting(gas_id) &&
        (GMM_READY_STATE == rr_gmminfo_get_gmm_status(gas_id)))
    {
      if (gas_id_ptr != NULL)
      {
        *gas_id_ptr = gas_id;
      }
      return(TRUE);
    }
  }

  return(FALSE);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Returns the value of "newTLLI" received in RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ.
 *
 * \param gas_id
 *
 * \return uint32
 */
uint32 rr_gmminfo_get_tlli(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.tlli;

} /* end rr_gmminfo_get_tlli() */

/*!
 * \brief Returns the value of "oldTLLI" received in RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ.
 *
 * \param gas_id
 *
 * \return uint32
 */
uint32 rr_gmminfo_get_old_tlli(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.old_tlli;

} /* end rr_gmminfo_get_old_tlli() */

/*!
 * \brief Returns the value of "negotiated_ready_timer_zero" received in RR_GMM_READY_TIMER_STATE_REQ.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_gmminfo_get_neg_ready_timer_zero(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.negotiated_ready_timer_zero;

} /* end rr_gmminfo_get_neg_ready_timer_zero() */

/*!
 * \brief MM non-DRX mode in L1 is cancelled, if L1 was previously put in MM non-DRX mode.
 *
 * \param gas_id
 */
void rr_gprs_clear_l1_in_mm_non_drx(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (rr_ps_domain_data_ptr->gmm.mm_non_drx_status == MM_NON_DRX_ACTIVE)
  {
    rr_send_mph_stop_mm_non_drx_ind(gas_id);
  }

  /* The status is reset, whether it was 'active' or 'pending' */
  rr_ps_domain_data_ptr->gmm.mm_non_drx_status = MM_NON_DRX_INACTIVE;

} /* end rr_gprs_clear_l1_in_mm_non_drx() */

/*!
 * \brief L1 is put in MM non-DRX mode, if MM had previously requested it.
 *
 * \param gas_id
 */
void rr_gprs_set_l1_in_mm_non_drx(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (rr_ps_domain_data_ptr->gmm.mm_non_drx_status == MM_NON_DRX_PENDING)
  {
    rr_send_mph_start_mm_non_drx_ind(gas_id);

    rr_ps_domain_data_ptr->gmm.mm_non_drx_status = MM_NON_DRX_ACTIVE;
  }

} /* rr_gprs_set_l1_in_mm_non_drx() */

/*!
 * \brief Returns TRUE if L1 is in MM non-DRX mode due to a request from GMM.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_gprs_non_drx_mode_requested(const gas_id_t gas_id)
{
  boolean in_mm_non_drx_mode = FALSE;
  rr_ps_domain_data_t *rr_ps_domain_data_ptr;

  // Obtain a pointer to the module data
  rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (rr_ps_domain_data_ptr->gmm.mm_non_drx_status == MM_NON_DRX_ACTIVE)
  {
    in_mm_non_drx_mode = TRUE;
  }

  return in_mm_non_drx_mode;
}

/*!
 * \brief Returns TRUE is GMM has previously requested GPRS service; FALSE otherwise.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_ps_domain_active(const gas_id_t gas_id)
{
  return rr_ps_tlli_is_assigned(gas_id) && rr_service_domain_allows_ps(gas_id);
}

/*!
 * \brief Indicates if PS is blocked due to an external request (e.g. RR_PS_ABORT_REQ).
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if PS is blocked; FALSE otherwise
 */
boolean rr_ps_blocked_due_to_external_request(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);

  boolean ps_blocked = rr_ps_domain_data_ptr->gmm.ps_blocked;

#ifdef FEATURE_DUAL_SIM
  if (!ps_blocked)
  {
    if (RR_PS_ABORT_NOT_ACTIVE != rr_ms_ps_abort_process_active(gas_id))
    {
      ps_blocked = TRUE;
    }
  }
#endif // FEATURE_DUAL_SIM

  return ps_blocked;
}

/**
  @brief Returns TRUE if GMM has requested GPRS service; FALSE otherwise
*/
boolean rr_gmm_gprs_service_requested(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  return rr_ps_domain_data_ptr->gmm.gprs_service_req;
}

/*!
 * \brief Resets the GPRS service request history from GMM.
 *
 * \param gas_id
 */
void rr_gmm_gprs_service_req_expected(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  if (rr_mac_ps_access_blocked(gas_id))
  {
    MSG_GERAN_HIGH_0_G("GMM request required to enable PS access");
    rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GMM request not required to enable PS access");
  }
}

/*!
 * \brief Sets the flag which indicates if Radio Access Capabilities received should be forwarded to LTE-RRC
 * 
 * \param pending_req (in)
 * \param gas_id (in)
 */
void rr_gmm_set_gprs_ra_cap_expected(
  boolean pending_req,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  rr_ps_domain_data_ptr->gmm.pending_ms_ra_caps = pending_req;

  return;
}

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)

#ifndef FEATURE_DUAL_DATA
/*!
 * \brief Indicates if any other sub has an active PS domain (i.e. TLLI assigned, etc).
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if any other sub has an active PS domain, FALSE otherwise
 */
static boolean ps_domain_active_other_subs(const gas_id_t gas_id)
{
  gas_id_t other_gas_id;

  for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
  {
    if (other_gas_id != gas_id)
    {
      if (rr_mode_gsm_active_or_starting(other_gas_id) && rr_ps_domain_active(other_gas_id))	  	
      {
        // Sub 'other_gas_id' has PS domain active - exit
        return TRUE;
      }
    }
  }

  // All other subs have PS domain inactive
  return FALSE;
}

#endif //#ifndef FEATURE_DUAL_DATA

/*!
 * \brief Queries whether RR_GMM_GPRS_SERVICE_REQ message can be processed.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if message can be processed, FALSE if it should be blocked
 */
boolean rr_ps_can_process_gprs_service_req(const gas_id_t gas_id)
{
#ifndef FEATURE_DUAL_DATA
  /**
   * If a valid TLLI is assigned on this sub, but the PS domain is still active on the other sub, then block 
   * processing until the PS domain on the other sub is completely inactive. 
   * Note: If there is no TLLI assigned on this sub, then processing is allowed so that the 
   * RR_GMM_GPRS_SERVICE_REQ can be processed and then ignored.
   */
  if (rr_ps_tlli_is_assigned(gas_id) &&   // only continue checking if a TLLI is assigned on this sub
      (ps_domain_active_other_subs(gas_id) ||
       (rr_ps_access_blocked_other_subs(gas_id) == FALSE)))
  {
    return FALSE;
  }
#endif // #ifndef FEATURE_DUAL_DATA

  return TRUE;
}
#endif // #if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)

/*!
 * \brief Initialisation function.
 *
 * \param gas_id
 */
void rr_ps_domain_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_ps_domain_data_t *rr_ps_domain_data_ptr = rr_ps_domain_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ps_domain_data_ptr);

  rr_ps_domain_data_ptr->netw.gprs_suspended = FALSE;
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[0] = 0xFF;
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[1] = 0xFF;
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.plmn_id.identity[2] = 0xFF;
  rr_ps_domain_data_ptr->netw.gprs_suspension_lai.location_area_code = 0xFFFF;
  rr_ps_domain_data_ptr->netw.gprs_suspension_rac = 0x00;

  rr_ps_domain_data_ptr->gmm.gprs_service_req = FALSE;
  rr_ps_domain_data_ptr->gmm.gprs_suspended = FALSE;
  rr_ps_domain_data_ptr->gmm.resumption_ind = TRUE;
  rr_ps_domain_data_ptr->gmm.cell_update_required = FALSE;
  rr_ps_domain_data_ptr->gmm.negotiated_ready_timer_zero = FALSE;
  rr_ps_domain_data_ptr->gmm.mm_non_drx_status = MM_NON_DRX_INACTIVE;
}

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_ps_domain_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_ps_domain_data, 0, sizeof(rr_ps_domain_data));

  // Setup any default values which may be non-zero
#ifdef FEATURE_DUAL_SIM
  {
    int i;

    for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
    {
      rr_ps_domain_data[i].gmm.state    = GMM_IDLE_STATE;
      rr_ps_domain_data[i].gmm.tlli     = TLLI_VALUE_WHEN_UNASSIGNED;
      rr_ps_domain_data[i].gmm.old_tlli = TLLI_VALUE_WHEN_UNASSIGNED;
    }
  }
#endif // #ifdef FEATURE_DUAL_SIM

  return;
}

/* EOF */

