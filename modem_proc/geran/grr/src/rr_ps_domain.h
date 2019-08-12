#ifndef RR_PS_DOMAIN_H
#define RR_PS_DOMAIN_H

/*============================================================================
  @file rr_ps_domain.h

  This module provides functionality to manage the suspension of GPRS in GMM and network

                Copyright (c) 2002-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ps_domain.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mm_rr.h"
#include "rr_defs.h"
#include "sys.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TLLI_VALUE_WHEN_UNASSIGNED  0xFFFFFFFFUL

#define TLLI_IS_VALID(x)       ((x) != TLLI_VALUE_WHEN_UNASSIGNED)
#define TLLI_IS_UNASSIGNED(x)  ((x) == TLLI_VALUE_WHEN_UNASSIGNED)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Indicates if a valid TLLI is assigned.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if a valid TLLI is assigned; FALSE otherwise
 */
extern boolean rr_ps_tlli_is_assigned(const gas_id_t gas_id);

/*!
 * \brief Sends a GPRS SUSPENSION message to the network following a W2G handover. 
 *  
 * The location information included in the message is the 'registered_rai' given in the last SIM update. 
 * 
 * \param gas_id 
 */
extern void grr_send_gprs_suspension_ota_after_w2g_ho(const gas_id_t gas_id);

/*!
 * \brief Sets the PLMN ID, LAC & RAC to be included in the next GPRS SUSPENSION message to the network.
 * 
 * \param registered_rai 
 * \param gas_id 
 */
extern void grr_set_ota_gprs_suspension_parameters(
  routing_area_id_T registered_rai,
  const gas_id_t gas_id
);

/*!
 * \brief Sends a GPRS SUSPENSION message to the network. 
 *  
 *  The value of 'Suspension Cause' included in the message is based on the function parameter 'cause'.
 * 
 * \param cause 
 * \param gas_id 
 */
extern void grr_send_gprs_suspension_ota(
  establishment_cause_T cause,
  const gas_id_t gas_id
);

/*!
 * \brief Returns TRUE if a GPRS SUSPENSION message should be sent to the network; FALSE otherwise.
 *  
 *  A GPRS SUSPENSION message is sent unless
 *   - GMM state is IDLE
 *   - DTM is enabled in NV and supported in the cell
 * 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean grr_determine_if_gprs_suspension_should_be_sent_ota(const gas_id_t gas_id);

/*!
 * \brief Stores the state of the GPRS Resumption Indicator received in a CHANNEL RELEASE message.
 * 
 * \param gprs_resumption_ind 
 * \param gas_id 
 */
extern void grr_set_gprs_resumption_indicator(
  boolean gprs_resumption_ind,
  const gas_id_t gas_id
);

#ifdef FEATURE_GSM_DTM
/*!
 * \brief Flags that a Cell Update is required when GPRS in GMM is next resumed.
 * 
 * \param gas_id 
 */
extern void grr_gprs_cell_update_required_when_gmm_resumed(const gas_id_t gas_id);
#endif // FEATURE_GSM_DTM

/*!
 * \brief Returns TRUE if GPRS in GMM is currently suspended; FALSE otherwise.
 * 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean grr_is_gmm_suspended(const gas_id_t gas_id);

/*!
 * \brief Suspends GPRS in GMM, if currently not suspended.
 * 
 * \param gas_id 
 */
extern void rr_gmm_suspend_gprs(const gas_id_t gas_id);

/*!
 * \brief Resumes GPRS in GMM, if currently suspended.
 * 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean rr_gmm_resume_gprs(const gas_id_t gas_id);

/*!
 * \brief Called when RR_GMM_GPRS_SERVICE_REQ is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_gprs_service_req(
  rr_gmm_gprs_service_req_T *msg_ptr,
  const gas_id_t gas_id
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Called when RR_PS_ABORT_REQ is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
extern void rr_ps_process_ps_abort_req(rr_ps_abort_req_T *msg_ptr, const gas_id_t gas_id);

/*!
 * \brief Called when RR_PS_RESUME_IND is received from GMM.
 *
 * \param msg_ptr
 * \param gas_id
 */
extern void rr_ps_process_ps_resume_ind(rr_ps_resume_ind_T *msg_ptr, const gas_id_t gas_id);
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM

/*!
 * \brief Called when RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ is received from GMM.
 *  
 *  If both 'oldTLLI' and 'newTLLI' are unassigned, PS access will be blocked (a RR_IMSG_NO_PS_ACCESS_REQ
 *  will be sent into RR).
 *  
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_gprs_tlli_assignment_req(
  rr_gmm_gprs_tlli_assignment_req_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_GMM_GPRS_STATE_CHANGE_REQ is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_gprs_state_change_req(
  rr_gmm_gprs_state_change_req_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_GMM_READY_TIMER_STATE_REQ is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_ready_timer_state_req(
  rr_gmm_ready_timer_state_req_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_GMM_GPRS_MS_RA_CAP_IE_IND is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_gprs_ms_ra_cap_ie_ind(
  rr_gmm_gprs_ms_ra_cap_ie_ind_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_GMM_START_MM_NON_DRX_IND is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_start_mm_non_drx_ind(
  rr_gmm_start_mm_non_drx_ind_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_GMM_STOP_MM_NON_DRX_IND is received from GMM.
 * 
 * \param msg_ptr 
 * \param gas_id 
 */
extern void rr_ps_process_gmm_stop_mm_non_drx_ind(
  rr_gmm_stop_mm_non_drx_ind_T *msg_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Called when RR_PLMN_SELECT_REQ is received from MM and the service domain is changed.
 * 
 * \param old_service_domain 
 * \param new_service_domain 
 * \param gas_id 
 */
extern void rr_ps_plmn_select_req_service_domain_changed(
  sys_srv_domain_e_type old_service_domain,
  sys_srv_domain_e_type new_service_domain,
  const gas_id_t gas_id
);

#ifdef FEATURE_SGLTE

/*!
 * \brief Call-back function which sends RR_STOP_GSM_MODE_CNF(MM_AS_SUCCESS) to MM.
 * 
 * \param gas_id (in)
 */
extern void rr_sglte_confirm_rat_change_cb_fn(gas_id_t gas_id);

/*!
 * \brief Called when RR_CHANGE_MODE_IND is recieved from MM.
 * 
 * \param old_service_domain (in)
 * \param new_service_domain (in)
 * \param gas_id (in)
 */
extern void rr_ps_handle_rr_change_mode_ind_service_domain_changed(
  sys_srv_domain_e_type old_service_domain,
  sys_srv_domain_e_type new_service_domain,
  const gas_id_t        gas_id
);

/*!
 * \brief Called when RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE) is received from MM.
 *        Note: This is an implicit removal of PS service.
 * 
 * \param cb_fn (in)
 * \param gas_id (in)
 */
extern void rr_ps_sglte_handle_ps_removal(
  rr_cb_fn_t cb_fn,
  const gas_id_t gas_id
);

#endif /* FEATURE_SGLTE */

/*!
 * \brief Returns the GMM status as indicated by RR_GMM_GPRS_STATE_CHANGE_REQ.
 * 
 * \param gas_id 
 * 
 * \return gmm_llc_if_state_type 
 */
extern gmm_llc_if_state_type rr_gmminfo_get_gmm_status(const gas_id_t gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
  @brief Determines whether anysubscription is in READY state
*/ 
extern boolean grr_any_subscription_in_ready_state(gas_id_t *gas_id_ptr);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Returns the value of "newTLLI" received in RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ.
 * 
 * \param gas_id 
 * 
 * \return uint32 
 */
extern uint32 rr_gmminfo_get_tlli(const gas_id_t gas_id);

/*!
 * \brief Returns the value of "oldTLLI" received in RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ.
 * 
 * \param gas_id 
 * 
 * \return uint32 
 */
extern uint32 rr_gmminfo_get_old_tlli(const gas_id_t gas_id);

/*!
 * \brief Returns the value of "negotiated_ready_timer_zero" received in RR_GMM_READY_TIMER_STATE_REQ.
 * 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean rr_gmminfo_get_neg_ready_timer_zero(const gas_id_t gas_id);

/*!
 * \brief MM non-DRX mode in L1 is cancelled, if L1 was previously put in MM non-DRX mode. 
 * 
 * \param gas_id 
 */
extern void rr_gprs_clear_l1_in_mm_non_drx(const gas_id_t gas_id);

/*!
 * \brief L1 is put in MM non-DRX mode, if MM had previously requested it.
 * 
 * \param gas_id 
 */
extern void rr_gprs_set_l1_in_mm_non_drx(const gas_id_t gas_id);

/*!
 * \brief Returns TRUE if L1 is in MM non-DRX mode due to a request from GMM.
 * 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean rr_gprs_non_drx_mode_requested(const gas_id_t gas_id);

/*!
 * \brief Indicates if the PS domain is active (i.e. TLLI is assigned).
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the PS domain is active; FALSE otherwise
 */
extern boolean rr_ps_domain_active(const gas_id_t gas_id);

/*!
 * \brief Indicates if PS is blocked due to an external request (e.g. RR_PS_ABORT_REQ).
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if PS is blocked; FALSE otherwise
 */
extern boolean rr_ps_blocked_due_to_external_request(const gas_id_t gas_id);

/*!
 * \brief Indicates if GMM has requested GPRS service.
 * 
 * \param gas_id 
 *  
 * \return boolean - TRUE if GMM has requested GPRS service; FALSE otherwise
 */
extern boolean rr_gmm_gprs_service_requested(const gas_id_t gas_id);

/*!
 * \brief Resets the GPRS service request history from GMM.
 * 
 * \param gas_id 
 */
extern void rr_gmm_gprs_service_req_expected(const gas_id_t gas_id);

/*!
 * \brief Sets the flag which indicates if Radio Access Capabilities received should be forwarded to LTE
 * 
 * \param pending_req (in) - TRUE if a request has been sent to GMM
 * \param gas_id (in) - access stratum identifier
 *  
 * \return void
 */
extern void rr_gmm_set_gprs_ra_cap_expected(
  boolean pending_req,
  const gas_id_t gas_id
);

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
/*!
 * \brief Queries whether RR_GMM_GPRS_SERVICE_REQ message can be processed.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the message can be processed, FALSE if it should be blocked
 */
extern boolean rr_ps_can_process_gprs_service_req(const gas_id_t gas_id);
#endif // #if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)

/*!
 * \brief Initialisation function.
 * 
 * \param gas_id 
 */
extern void rr_ps_domain_init(const gas_id_t gas_id);

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
extern void rr_ps_domain_task_start_init(void);


#endif /* #ifndef RR_PS_DOMAIN_H */

