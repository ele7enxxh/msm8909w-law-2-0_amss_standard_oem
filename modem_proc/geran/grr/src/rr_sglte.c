/*============================================================================
  @file rr_sglte.c

  @brief This module provides various functionality related to the SGLTE
         feature.

                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "gprs_mem.h"


#ifdef FEATURE_SGLTE

#include "rr_sglte.h"
#include "rr_task.h"
#include "rr_msgr.h"
#include "rr_general.h"
#include "rr_tds_send.h"
#ifdef FEATURE_LTE
#include "rr_w2g_service_redirection.h"
#endif /* FEATURE_LTE */
#include "rr_sys_info_ncell.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/**
  @brief Sends a GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP or
         GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP indicating that service is not
         available.

  @param umid The umid to send.
*/
static void rr_send_irat_sglte_serving_meas_rsp_no_service(
  msgr_umid_type umid,
  const gas_id_t gas_id
)
{
  geran_grr_irat_sglte_serving_meas_rsp_t sglte_serving_meas_rsp;
  errno_enum_type                         result;

  RR_ASSERT_WARN(umid == GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP ||
                 umid == GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP);

  memset(&sglte_serving_meas_rsp, 0, sizeof(sglte_serving_meas_rsp));
  
  msgr_init_hdr(
    &sglte_serving_meas_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    umid
  );

  sglte_serving_meas_rsp.service_available = FALSE;

  result = rr_msgr_send(&sglte_serving_meas_rsp.msg_hdr, sizeof(sglte_serving_meas_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

/**
  @brief Sends a GERAN_GRR_LTE_SGLTE_NEIGHBORMEAS_RSP indicating no valid measurement
                
*/


static void rr_send_irat_sglte_neighbor_meas_rsp_no_service(
 msgr_umid_type umid,
 const gas_id_t gas_id
)
{
  geran_grr_irat_sglte_neighbor_meas_rsp_t sglte_neighbor_meas_rsp;
  errno_enum_type                         result;

  memset(&sglte_neighbor_meas_rsp, 0, sizeof(sglte_neighbor_meas_rsp));
  
  msgr_init_hdr(
    &sglte_neighbor_meas_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    umid
  );

  sglte_neighbor_meas_rsp.service_available = FALSE;

  result = rr_msgr_send(&sglte_neighbor_meas_rsp.msg_hdr, sizeof(sglte_neighbor_meas_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

static void rr_send_irat_sglte_neighbor_meas_rsp(
  msgr_umid_type umid,
  ARFCN_T        arfcn,
  boolean        bsic_known,
  BSIC_T         bsic,
  uint8          rxlev,
  const gas_id_t gas_id
)
{
  geran_grr_irat_sglte_neighbor_meas_rsp_t sglte_neighbor_meas_rsp;
  errno_enum_type                          result;
  uint8                                    i;
  best_six_ncells_t                       *local_best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  memset(&sglte_neighbor_meas_rsp, 0, sizeof(sglte_neighbor_meas_rsp));

  msgr_init_hdr(
    &sglte_neighbor_meas_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    umid
  );

  sglte_neighbor_meas_rsp.service_available = TRUE;

  /*Fill the serving cell information first*/
  MSG_GERAN_HIGH_0_G("Adding Serving cell measuremnet ");
  
  sglte_neighbor_meas_rsp.n_cell_meas[0].arfcn = arfcn.num;
  if (arfcn.band == SYS_BAND_PCS_1900)
  {
   sglte_neighbor_meas_rsp.n_cell_meas[0].band_ind = GERAN_GRR_BAND_IND_PCS_1900;
  }
  else
  {
    sglte_neighbor_meas_rsp.n_cell_meas[0].band_ind = GERAN_GRR_BAND_IND_DCS_1800;
  }
  
  if (bsic_known)
  {
    sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id_valid = TRUE;
    sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id.ncc = bsic.PLMN_colour_code;
    sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id.bcc = bsic.BS_colour_code;
  }
  sglte_neighbor_meas_rsp.n_cell_meas[0].rssi_dBm = rxlev_to_dbm(rxlev);
  sglte_neighbor_meas_rsp.n_cell_meas[0].is_serving_cell = TRUE;
  sglte_neighbor_meas_rsp.ncell_count++;
  
  MSG_GERAN_HIGH_1_G("Serving cell ARFCN is = %d", sglte_neighbor_meas_rsp.n_cell_meas[0].arfcn);
  MSG_GERAN_HIGH_1_G("Serving] cell Band is = %d ", sglte_neighbor_meas_rsp.n_cell_meas[0].band_ind);
  MSG_GERAN_HIGH_1_G("Serving cell cell_id_valid is = %d", sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id_valid);
  MSG_GERAN_HIGH_1_G("Serving cell cell_id NCC part is = %d",sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id.ncc);
  MSG_GERAN_HIGH_1_G("Serving cell cell_id BCC part is = %d",sglte_neighbor_meas_rsp.n_cell_meas[0].cell_id.bcc);	  
  MSG_GERAN_HIGH_1_G("Serving cell rssi_dBm is = %d",sglte_neighbor_meas_rsp.n_cell_meas[0].rssi_dBm);

  MSG_GERAN_HIGH_0_G("Serving cell added now add N cell measuremnet ");

  if(local_best_six_ncells_ptr != NULL)
  {
   MSG_GERAN_HIGH_1_G("No. of N cell  known are  %d", local_best_six_ncells_ptr->no_of_entries);

   for (i= 0; (i < local_best_six_ncells_ptr->no_of_entries) && (i < MAX_NUMBER_OF_BEST_CELLS ) ; i++)  	
   {
      sglte_neighbor_meas_rsp.n_cell_meas[i+1].is_serving_cell = FALSE;
	
      sglte_neighbor_meas_rsp.n_cell_meas[i+1].arfcn = CONVERT_FROM_ARFCN_T (local_best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn);
	 
      if (local_best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn.band == SYS_BAND_PCS_1900)
      {
        sglte_neighbor_meas_rsp.n_cell_meas[i+1].band_ind = GERAN_GRR_BAND_IND_PCS_1900;
      }
      else
      {
        sglte_neighbor_meas_rsp.n_cell_meas[i+1].band_ind = GERAN_GRR_BAND_IND_DCS_1800;
      }
  
      if(local_best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic_valid)
      {
        sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id_valid = TRUE ;  
        sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id.ncc= local_best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.PLMN_colour_code;
        sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id.bcc= local_best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.BS_colour_code;		 
      }

      sglte_neighbor_meas_rsp.n_cell_meas[i+1].rssi_dBm = rxlev_to_dbm(local_best_six_ncells_ptr->ncell_entry[i].ncell_data->rxlev);  
      sglte_neighbor_meas_rsp.ncell_count++;

      MSG_GERAN_HIGH_2_G("N[%d] cell ARFCN is = %d", i,sglte_neighbor_meas_rsp.n_cell_meas[i+1].arfcn);
      MSG_GERAN_HIGH_2_G("N[%d] cell Band is = %d ", i,sglte_neighbor_meas_rsp.n_cell_meas[i+1].band_ind);
      MSG_GERAN_HIGH_2_G("N[%d] cell cell_id_valid  is = %d",i, sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id_valid);
      MSG_GERAN_HIGH_2_G("N[%d] cell cell_id NCC part is =  %d", i,sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id.ncc);
      MSG_GERAN_HIGH_2_G("N[%d] cell cell_id BCC part is =  %d", i,sglte_neighbor_meas_rsp.n_cell_meas[i+1].cell_id.bcc);	   
      MSG_GERAN_HIGH_2_G("N[%d] cell rssi_dBm is = %d",i, sglte_neighbor_meas_rsp.n_cell_meas[i+1].rssi_dBm);
   }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("local_best_six_ncells_ptr is NULL hence no info for N cell:");  
  }

  MSG_GERAN_HIGH_1_G("Total no. of cell in the measurement list are %d", sglte_neighbor_meas_rsp.ncell_count);

  result = rr_msgr_send(&sglte_neighbor_meas_rsp.msg_hdr, sizeof(sglte_neighbor_meas_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}


/**
  @brief Sends a GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP or
         GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP indicating that service is
         available, along with details of the serving cell.

  @param umid The umid to send.
  @param arfcn The ARFCN of the serving cell.
  @param bsic_known TRUE if the BSIC of the serving cell is known, FALSE if
         not.
  @param bsic The BSIC of the serving cell (if known).
  @param rxlev The RXLEV of the serving cell.
*/
static void rr_send_irat_sglte_serving_meas_rsp(
  msgr_umid_type umid,
  ARFCN_T        arfcn,
  boolean        bsic_known,
  BSIC_T         bsic,
  uint8          rxlev,
  const gas_id_t gas_id
)
{
  geran_grr_irat_sglte_serving_meas_rsp_t sglte_serving_meas_rsp;
  errno_enum_type                         result;

  memset(&sglte_serving_meas_rsp, 0, sizeof(sglte_serving_meas_rsp));

  msgr_init_hdr(
    &sglte_serving_meas_rsp.msg_hdr,
    MSGR_GERAN_GRR,
    umid
  );

  sglte_serving_meas_rsp.service_available = TRUE;
  sglte_serving_meas_rsp.arfcn = arfcn.num;
  if (arfcn.band == SYS_BAND_PCS_1900)
  {
    sglte_serving_meas_rsp.band_ind = GERAN_GRR_BAND_IND_PCS_1900;
  }
  else
  {
    sglte_serving_meas_rsp.band_ind = GERAN_GRR_BAND_IND_DCS_1800;
  }
  if (bsic_known)
  {
    sglte_serving_meas_rsp.cell_id_valid = TRUE;
    sglte_serving_meas_rsp.cell_id.ncc = bsic.PLMN_colour_code;
    sglte_serving_meas_rsp.cell_id.bcc = bsic.BS_colour_code;
  }
  sglte_serving_meas_rsp.rssi_dBm = rxlev_to_dbm(rxlev);

  result = rr_msgr_send(&sglte_serving_meas_rsp.msg_hdr, sizeof(sglte_serving_meas_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

#ifdef FEATURE_LTE
void rr_send_lte_sglte_serving_meas_rsp_no_service(const gas_id_t gas_id)
{
  rr_send_irat_sglte_serving_meas_rsp_no_service(
    GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP,
    gas_id
  );
}
#endif /* FEATURE_LTE */

void rr_send_lte_sglte_neighbor_meas_rsp_no_service(const gas_id_t gas_id)
{
  rr_send_irat_sglte_neighbor_meas_rsp_no_service(
    GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_RSP, gas_id
  );
}

void rr_send_tds_sglte_serving_meas_rsp_no_service(const gas_id_t gas_id)
{
  rr_send_irat_sglte_serving_meas_rsp_no_service(
    GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP,
    gas_id
  );
}

#ifdef FEATURE_LTE
void rr_send_lte_sglte_serving_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
)
{
  rr_send_irat_sglte_serving_meas_rsp(
    GERAN_GRR_LTE_SGLTE_SERVING_MEAS_RSP,
    arfcn,
    bsic_known,
    bsic,
    rxlev,
    gas_id
  );
}
#endif /* FEATURE_LTE */

void rr_send_lte_sglte_neighbor_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
)
{
  rr_send_irat_sglte_neighbor_meas_rsp(
    GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_RSP,
    arfcn,
    bsic_known,
    bsic,
    rxlev,
    gas_id
  );
}


void rr_send_tds_sglte_serving_meas_rsp(
  ARFCN_T arfcn,
  boolean bsic_known,
  BSIC_T  bsic,
  uint8   rxlev,
  const gas_id_t gas_id
)
{
  rr_send_irat_sglte_serving_meas_rsp(
    GERAN_GRR_TDS_SGLTE_SERVING_MEAS_RSP,
    arfcn,
    bsic_known,
    bsic,
    rxlev,
    gas_id
  );
}

void rr_sglte_set_rat_priority_list_gsm_only(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * rr_plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_priority_list_info_s_type * gsm_priority_list_info;

  gsm_priority_list_info = rr_rat_pri_list_info_get_sys_mode_entry(
    &rr_plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_GSM
  );                      

  if (gsm_priority_list_info != NULL)
  {
    rr_plmn_request_details->rat_pri_list_info.num_items = 1;
    rr_plmn_request_details->rat_pri_list_info.priority_list_info[0] = *gsm_priority_list_info;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to find GSM priority info in RAT priority list");
  }
}

#ifdef FEATURE_LTE
void rr_sglte_reject_lte_resel_req(const gas_id_t gas_id)
{
  geran_grr_lte_resel_failed_rsp_t lte_resel_failed_rsp;
  errno_enum_type                  result;

  MSG_GERAN_HIGH_0_G("Rejecting SGLTE L2G reselection, RR inactive");

  memset(&lte_resel_failed_rsp, 0, sizeof(lte_resel_failed_rsp));

  msgr_init_hdr(
    &lte_resel_failed_rsp.hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_RESEL_FAILED_RSP
  );
  lte_resel_failed_rsp.resel_fail_cause = GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_UNDEFINED;
  result = rr_msgr_send(&lte_resel_failed_rsp.hdr, sizeof(lte_resel_failed_rsp), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

void rr_sglte_reject_lte_redir_req(const gas_id_t gas_id)
{
  rr_x2g_send_lte_redir_failure(gas_id);
  return;
}

void rr_sglte_reject_lte_plmn_srch_req(const gas_id_t gas_id)
{
  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
  errno_enum_type          result;

  MSG_GERAN_HIGH_0_G("Rejecting L2G PLMN List Search");

  plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

  RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);  
  msgr_init_hdr(
    &plmn_srch_rsp_ptr->msg_hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_PLMN_SRCH_RSP
  );

  plmn_srch_rsp_ptr->srch_done = TRUE;

  rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

  result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
  GPRS_MEM_FREE(plmn_srch_rsp_ptr);  
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
void rr_sglte_reject_tds_resel_req(const gas_id_t gas_id)
{
  rr_send_tds_interrat_reselection_rej(
    TDSRR_GSM_OTHER_REASON,
    0,
    gas_id
  );
}

void rr_sglte_reject_tds_redir_req(const gas_id_t gas_id)
{
  rr_send_tds_interrat_redirect_rej(
    TDSRR_REDIRECT_FAIL_OTHER_REASON,
    NULL,
    gas_id
  );
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
void rr_sglte_send_lte_plmn_srch_rsp_in_dedicated(
  sys_plmn_id_s_type        plmn,
  uint8                     rxlev,
  sys_srv_capability_e_type service_capability,
  const gas_id_t            gas_id
)
{
  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
  errno_enum_type          result;

  MSG_GERAN_HIGH_0("SGLTE L2G PLMN List Search, RR in dedicated mode, respond with serving PLMN");

  plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

  RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);  
  

  msgr_init_hdr(
    &plmn_srch_rsp_ptr->msg_hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_PLMN_SRCH_RSP
  );

  plmn_srch_rsp_ptr->srch_done = TRUE;

  plmn_srch_rsp_ptr->available_plmn_list.length = 1;
  plmn_srch_rsp_ptr->available_plmn_list.info[0].plmn = plmn;
  plmn_srch_rsp_ptr->available_plmn_list.info[0].rat = SYS_RAT_GSM_RADIO_ACCESS;
  plmn_srch_rsp_ptr->available_plmn_list.info[0].signal_strength = rr_convert_rxlex_to_absolute_dBm(rxlev);
  plmn_srch_rsp_ptr->available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_LOW;
  if (plmn_srch_rsp_ptr->available_plmn_list.info[0].signal_strength < 85)
  {
    plmn_srch_rsp_ptr->available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
  }
  else
  {
    plmn_srch_rsp_ptr->available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_LOW;
  }
  plmn_srch_rsp_ptr->available_plmn_list.info[0].plmn_service_capability = service_capability;

  rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

  result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }

  GPRS_MEM_FREE(plmn_srch_rsp_ptr);    
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
void rr_sglte_reject_tds_plmn_srch_req(const gas_id_t gas_id)
{
  MSG_GERAN_HIGH_0_G("Rejecting L2G PLMN List Search");

  rr_send_tds_interrat_plmn_search_cnf(
    TRUE,
    FALSE,
    NULL,
    gas_id
  );
}

void rr_sglte_send_tds_plmn_srch_rsp_in_dedicated(
  sys_plmn_id_s_type        plmn,
  uint8                     rxlev,
  sys_srv_capability_e_type service_capability,
  const gas_id_t gas_id
)
{
  sys_detailed_plmn_list_s_type available_plmn_list;

  MSG_GERAN_HIGH_0_G("SGLTE T2G PLMN List Search, RR in dedicated mode, respond with serving PLMN");

  memset(&available_plmn_list, 0, sizeof(available_plmn_list));

  available_plmn_list.length = 1;
  available_plmn_list.info[0].plmn = plmn;
  available_plmn_list.info[0].rat = SYS_RAT_GSM_RADIO_ACCESS;
  available_plmn_list.info[0].signal_strength = rr_convert_rxlex_to_absolute_dBm(rxlev);
  available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_LOW;
  if (available_plmn_list.info[0].signal_strength < 85)
  {
    available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_HIGH;
  }
  else
  {
    available_plmn_list.info[0].signal_quality = SYS_SIGNAL_QUALITY_LOW;
  }
  available_plmn_list.info[0].plmn_service_capability = service_capability;

  rr_send_tds_interrat_plmn_search_cnf(
    TRUE,
    FALSE,
    &available_plmn_list,
    gas_id
  );
}
#endif /*FEATURE_GSM_TDS*/

void rr_sglte_send_irat_plmn_srch_abort_cnf(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  switch (rat)
  {
#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      lte_irat_abort_plmn_srch_rsp_s abort_plmn_srch_rsp;
      errno_enum_type                result;

      memset(&abort_plmn_srch_rsp, 0, sizeof(abort_plmn_srch_rsp));

      msgr_init_hdr(
        &abort_plmn_srch_rsp.msg_hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP
      );

      result = rr_msgr_send(&abort_plmn_srch_rsp.msg_hdr, sizeof(abort_plmn_srch_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_send_tds_interrat_plmn_search_abort_cnf(gas_id);
      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
    }
  }
}

/*===========================================================================

FUNCTION  rr_sglte_multimode_using_g_only_device

DESCRIPTION Determines if the multimode SUB is currently using the G only 
            WTR (WTR2605).  This function only works in SGLTE+G DSDA mode. 

DEPENDENCIES gas_id_t GAS ID

RETURN VALUE TRUE - multi-mode sub is using G only WTR, FALSE - opposite

SIDE EFFECTS

===========================================================================*/
boolean rr_sglte_multimode_using_g_only_device(const gas_id_t gas_id)
{
  boolean g_only_dev = FALSE;
#ifdef FEATURE_SGLTE_DUAL_SIM
  trm_get_info_return_type trm_query;

  if (rr_is_sglte_mode_active(gas_id) &&
      (SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE == geran_get_multi_sim_mode()) &&
      (TRM_DENIAL != grm_get_granted(TRM_GSM1, &trm_query, gas_id)))
  {
    /* check whether the multimode GSM client has been granted device 2 */
    if (RFM_DEVICE_2 == trm_query.info.client_info.dev)
    {
      trm_get_info_input_type input;

      /* Now query the TRM simultaneous "capabilities" info */
      /* Reuse of trm_query for the TRM output should be OK */
      input.client_id = TRM_GSM1;
      input.info_type = TRM_SIMUL_CAPABILITY;
      trm_get_info(&input, &trm_query);

      if ((TRM_SGLTE_SGTDS_IS_ENABLED & trm_query.info.simul_cap_info) &&
          (TRM_DSDA_IS_ENABLED        & trm_query.info.simul_cap_info))
      {
        g_only_dev = TRUE;
      }
    }
  }

  MSG_GERAN_HIGH_1_G("rr_sglte_multimode_using_g_only_device returns %1d", (int)g_only_dev);
#endif /* FEATURE_SGLTE_DUAL_SIM */

  return(g_only_dev);
}

#endif /* FEATURE_SGLTE */

