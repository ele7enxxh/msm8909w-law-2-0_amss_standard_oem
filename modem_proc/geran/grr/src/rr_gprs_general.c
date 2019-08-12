/*****************************************************************************
***
*** TITLE
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_general.c#1 $
***
*** DESCRIPTION
***
*** Contains non specific functions that could be used by any code/handler
***
*** Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated and its licensors.
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
#include "customer.h"  /* Contains FEATURE_... defines */

/**** Rest of module GPRS only... ****/
#include "rr_seg_load.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "ms.h"
#include "gs.h"
#include "rr_sys_info.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_shared_cell_selection.h"
#include "rr_sys_info_3g.h"
#include "rr_resel_calcs.h"
#include "rr_resel.h"
#include "rr_acq_db.h"
#include "rr_control.h"
#include "rr_conn_establish.h"
#include "gprs_odm_info.h"
#include "rr_gprs_conn_establish.h"
#include "rr_pscan.h"
#include "gprs_mem.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pacch.h"

#include "rr_multiband.h"
#include "rr_multiband_g.h"

#include "rr_log.h"

#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */

#include "rr_resel_nacc.h"

#include "rr_multi_sim.h"
#include "rr_sim.h"
#include "rr_grr.h"
#include "rr_cell_acq.h"
#include "rr_plmn_list.h"
#include "rr_ps_access.h"
#include "rr_init.h"
#include "rr_sys_info_cache.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

#include "rr_nv.h"

#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC


/*****************************************************************************
***
***     External Functions & Variables
***
*****************************************************************************/

/*****************************************************************************
***
***     Local #defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Local Function Prototypes & Variables
***
*****************************************************************************/
typedef struct
{
  rr_event_T                 guard_timer_expiry_event;
  gprs_odm_cell_type_t       odm_cell_type;
  gprs_odm_cell_type_cb_fn_t odm_cell_type_cb_fn;
  ccch_pccch_params_t        l1_params_51;
  ccch_pccch_params_t        l1_params_52;
  int8                       odm_info_rach_counter;
} rr_gprs_general_data_t;

#define _RR_GPRS_GENERAL_DATA_INIT { EV_NO_EVENT,                       /*guard_timer_expiry_event*/   \
                                    ODM_CELL_TYPE_UNKNOWN,             /*odm_cell_type*/              \
                                    NULL                               /*odm_cell_type_cb_fn*/ }

static rr_gprs_general_data_t rr_gprs_general_data[NUM_GERAN_DATA_SPACES] =
                                                     { INITIAL_VALUE(_RR_GPRS_GENERAL_DATA_INIT) };
#undef _RR_GPRS_GENERAL_DATA_INIT

static void init_signal_strength_indicator_handling(const gas_id_t gas_id);
static void gprs_recent_activity_timer_expiry(const gas_id_t gas_id);
static rr_event_T guard_timer_expiry(const gas_id_t gas_id);
static void log_all_gprs_parameters(gprs_scell_info_t * cell_info, const gas_id_t gas_id);


/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_gprs_general_data_t*
 */
static rr_gprs_general_data_t *rr_gprs_general_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_gprs_general_data[as_index];
}


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/****************************************************************************
 *   Function name:  rr_gprs_reset_software
 *   -------------
 *
 *   Description: This function resets the GPRS state machines and timers
 *   ------------
 *
 *   Parameters: void
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/

void rr_gprs_reset_software(const gas_id_t gas_id)
{
  /* Reset main GRR state machines, which also resets sub-state machines */
  // TODO - need to work out what needs to be initialised here and how best to do it
  rr_grr_control_init(gas_id);
  (void) rr_resel_control(EV_RESET_SOFTWARE, NULL, gas_id);
  rr_cell_acq_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_gprs_sys_info_acquisition_control( EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_plmn_list_control( EV_RESET_SOFTWARE, NULL, gas_id);
  (void) grr_conn_est_control(EV_RESET_SOFTWARE, NULL, gas_id);

  /* Reset GPRS data */
  rr_gprs_initialise(gas_id);

  return;
} /* end rr_gprs_reset_software() */



/*===========================================================================

FUNCTION rr_gprs_start_gsm_idle_mode

DESCRIPTION
  This function send MPH_START_IDLE_MODE (GSM) to L1, and if requested, sends a
  RR_SERVICE_IND to MM.
  Note: MAC is always notified of no service.

PARAMS
  bcch_arfcn - ARFCN of the BCCH of the cell to camp onto
  notify_mm - TRUE if the function should update MM with a RR_SERVICE_IND; FALSE for neither

RETURN VALUE
  None

===========================================================================*/

void rr_gprs_start_gsm_idle_mode(
  gprs_scell_info_t *cell_info,
  boolean early_camp_performed,
  const gas_id_t gas_id
)
{
  /* Start Idle Mode in L1. This also copies the pcell data over to the scell */
  /* if required, and clears the pcell data */
  rr_start_idle_mode(
    cell_info,
    FALSE,
    early_camp_performed,
    gas_id
  );

} /* end rr_gprs_start_gsm_idle_mode() */



/*===========================================================================

FUNCTION rr_gprs_get_l1_params

DESCRIPTION
  This function gathers together either CCCH_51 or PCCCH_52 parameters, to pass to
  L1 either when starting idle or updating L1 of changed parameters

PARAMS
  sys_info - pointer to sys info database
  ccch_or_pccch - indication of whether CCCH_51 or PCCCH_52
  ccch_pccch_params_t - structure to be populated

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rr_gprs_get_l1_params(
  ccch_pccch_params_t *ccch_pccch_params,
  const gas_id_t gas_id
)
{
  uint16                          imsi_mod_1000;
  ccch_idle_mode_params_t        *ccch_params;
  system_information_database_T  *gsm;
  gprs_bcch_scell_info_t         *gprs;
  boolean                         cbch_channel_information_valid;
  gprs_scell_info_t              *scell_info = rr_gprs_get_scell_info(gas_id);
  inter_task_BCCH_allocation_T   *ba_list = rr_gprs_get_ba_gprs(gas_id);
   
  RR_NULL_CHECK_FATAL(scell_info);
  RR_NULL_CHECK_FATAL(ba_list);
  RR_NULL_CHECK_FATAL(ccch_pccch_params);

  ccch_params = &ccch_pccch_params->ccch_idle_mode_params;
  gsm = &scell_info->gsm;
  gprs = &scell_info->bcch_sys_info;

  (void) rr_get_imsi_mod_1000(&imsi_mod_1000, gas_id);

  /* Copy over BA(GPRS) list */
  ccch_params->layer1_sys_info.BA_list = (* ba_list);

  cbch_channel_information_valid = gsm->cbch_channel_information_valid;
  if (cbch_channel_information_valid)
  {
    /* If SI1 is received after SI4 then the following code will ensure the   */
    /* CBCH frequency list is populated from the SI1 cell channel description */
    if (rr_set_channel_frequency_list_from_mobile_allocation(
          &gsm->cbch_channel_information,
          &gsm->cbch_mobile_allocation,
          &gsm->cell_channel_description,
          gas_id
        ) == FALSE)
    {
      /* if the channel list setup fails, flag the info as invalid */
      MSG_GERAN_ERROR_0_G("CBCH channel information could not be populated");
      /* note that this is a LOCAL flag, the system info DB still contains TRUE */
      cbch_channel_information_valid = FALSE;
    }
  }

  /* Copy parameters received in SIs - GSM */
  ccch_params->layer1_sys_info.BS_PA_MFRMS = gsm->control_channel_description.BS_PA_MFRMS;
  ccch_params->layer1_sys_info.BS_AG_BLKS_RES = gsm->control_channel_description.BS_AG_BLKS_RES;
  ccch_params->layer1_sys_info.BS_CC_CHANS = gsm->control_channel_description.BS_CC_CHANS;
  ccch_params->layer1_sys_info.BS_CCCH_SDCCH_COMB = gsm->control_channel_description.BS_CCCH_SDCCH_COMB;
  ccch_params->layer1_sys_info.MS_TXPWR_MAX_CCH = gsm->cell_selection_parameters.MS_TXPWR_MAX_CCH;
  ccch_params->layer1_sys_info.PWRC = gsm->cell_options.PWRC;
  ccch_params->layer1_sys_info.NCC_permitted = gsm->NCC_permitted;
  ccch_params->layer1_sys_info.TX_integer = gsm->RACH_control_parameters.tx_integer;
  ccch_params->layer1_sys_info.cbch_channel_information_valid = cbch_channel_information_valid;
  ccch_params->layer1_sys_info.cbch_channel_information = gsm->cbch_channel_information;
  ccch_params->layer1_sys_info.multiband_reporting = gsm->multiband_reporting;

  if (scell_info->bcch_sys_info.si13_data_valid_flag)
  {
    ccch_params->layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid;
    ccch_params->layer1_sys_info.LB_MS_TXPWR_MAX_CCH = scell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH;
  }
  else
  {
    ccch_params->layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = FALSE;
    ccch_params->layer1_sys_info.LB_MS_TXPWR_MAX_CCH = 0;
  }

  /* Copy parameters received in SIs - GPRS */
  ccch_params->layer1_extra_gprs_sys_info.drx_timer_max =  gprs->gprs_cell_options.drx_timer_max;
  ccch_params->layer1_extra_gprs_sys_info.nmo = gprs->gprs_cell_options.nmo;
  ccch_params->layer1_extra_gprs_sys_info.power_control.alpha = gprs->power_control_params.alpha;
  ccch_params->layer1_extra_gprs_sys_info.power_control.n_avg_i = gprs->power_control_params.n_avg_i;
  ccch_params->layer1_extra_gprs_sys_info.power_control.pc_meas_chan = gprs->power_control_params.pc_meas_chan;
  ccch_params->layer1_extra_gprs_sys_info.power_control.t_avg_t = gprs->power_control_params.t_avg_t;
  ccch_params->layer1_extra_gprs_sys_info.power_control.t_avg_w = gprs->power_control_params.t_avg_w;
  ccch_params->layer1_extra_gprs_sys_info.access_burst_type = gprs->gprs_cell_options.access_burst_type;
  ccch_params->layer1_extra_gprs_sys_info.non_drx_timer = gprs->non_drx_timer;

  if ( ( valid == gprs->gprs_cell_options.optional_extension_info_flag ) &&
       ( valid == gprs->gprs_cell_options.optional_extension_info.egprs_supported_flag ) )
  {
    ccch_params->layer1_extra_gprs_sys_info.bep_period = gprs->gprs_cell_options.optional_extension_info.bep_period;
  }
  else
  {
    ccch_params->layer1_extra_gprs_sys_info.bep_period = 0 ;
  }
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 

  if(FALSE == rr_get_nv_anite_gcf_flag(RR_GAS_ID_TO_AS_ID))
  {

   switch (gprs->gprs_cell_options.nmo)
   {
     case NETWORK_OP_MODE_1:
     {
       MSG_GERAN_HIGH_0_G("In NMO1 PCH read during transfer is not allowed");
       ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer = FALSE;
       break;
     }
     case NETWORK_OP_MODE_2:
     {
       ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer = rr_is_read_pch_during_transfer_allowed(gas_id);  
       break;
     }
     default:
     {
       ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer = FALSE;
     }
   }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Anite GCF flag is set");  
    ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer = FALSE;   
  }
  MSG_GERAN_HIGH_3_G("L1:bss_paging_coord=%d GCO:bss_paging_coord=%d NMO=%d",
                    (int)ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer,
                    (int)(gprs->gprs_cell_options.optional_extension_info_flag ? 
                          gprs->gprs_cell_options.optional_extension_info.bss_paging_coordination : 0),
                    (int)gprs->gprs_cell_options.nmo
                   );
#else
  ccch_params->layer1_extra_gprs_sys_info.read_pch_in_transfer = FALSE; 
#endif/*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

  ccch_params->layer1_extra_gprs_sys_info.ext_utbf_nodata =
    rr_get_nv_geran_feature_pack_1_enabled(RR_GAS_ID_TO_AS_ID) &&
    (valid == gprs->gprs_cell_options.optional_extension_info_flag) &&
    (gprs->gprs_cell_options.optional_extension_info.nw_ext_tbf) &&
    (gprs->gprs_cell_options.optional_extension_info.ext_utbf_no_data);

  /* Set IMSImod1000 */
  ccch_params->layer1_extra_gprs_sys_info.imsi_mod1000.msb = (byte) ( imsi_mod_1000 >> 8 );
  ccch_params->layer1_extra_gprs_sys_info.imsi_mod1000.lsb = (byte) ( imsi_mod_1000 & 0xff );

  /* Tell L1 to stop reading the BCCH */
  ccch_params->layer1_extra_gprs_sys_info.decode_bcch = FALSE;

  /* Set the L1 paging mode to the current RR paging mode */
  ccch_params->layer1_extra_gprs_sys_info.paging_mode = rr_ce_get_ota_page_mode(gas_id);

  if ( ba_list->no_of_entries == 0 )
  {
    MSG_GERAN_HIGH_1_G("WARNING! Empty BA list -> L1, Enh EXT ULTBF: %d",(int)ccch_params->layer1_extra_gprs_sys_info.ext_utbf_nodata);
  }
  else
  {
    MSG_GERAN_MED_1_G("Enh EXT ULTBF: %d", (int)ccch_params->layer1_extra_gprs_sys_info.ext_utbf_nodata);
  }

} /* end rr_gprs_get_l1_params() */


/*===========================================================================

FUNCTION rr_start_ccch51_idle_mode

DESCRIPTION
  This function starts L1 CCCH-51 Idle Mode.

PARAMS
  bcch_arfcn - ARFCN of the serving cell
  notify_mm_mac - if set TRUE, MM and MAC are notified of a camped-on-cell

RETURN VALUE
  None

SIDE EFFECTS
  MPH_START_GPRS_IDLE_MODE_REQ is sent to L1
  GRR_MAC_CAMPED_ON_CELL may be sent to MAC
  RR_SERVICE_IND may be sent to MM

===========================================================================*/

void rr_start_ccch51_idle_mode(
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  system_information_database_T  *gsm = &cell_info->gsm;
  rr_gprs_general_data_t         *rr_gprs_general_data_ptr;
  inter_task_BCCH_allocation_T   *ba_list;

   // Obtain a pointer to the module data
  rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  /* Dedicated data should no longer be considered valid, otherwise old
  SI5/5bis/5ter BA lists may be used if a new call is set up. */
  gsm->valid_dedicated_data_flag = FALSE;

  /* Get current parameters for L1 - this also compiles the BA(GPRS) list */
  rr_gprs_get_l1_params(
    &rr_gprs_general_data_ptr->l1_params_51,
    gas_id
  );

  /*
  On a new cell, so the paging mode is reset to Paging Normal. If this is not
  correct the reception of the next paging block will change this. The important
  thing is that GRR is in control of L1 paging mode.
  */
  rr_ce_set_ota_page_mode(NORMAL_PAGING, gas_id);
  rr_gprs_general_data_ptr->l1_params_51.ccch_idle_mode_params.layer1_extra_gprs_sys_info.paging_mode =
    NORMAL_PAGING;

  /* Instruct L1 to start idle mode on the cell */
  rr_send_mph_start_gprs_idle_mode_req(
    &rr_gprs_general_data_ptr->l1_params_51,
    gas_id
  );

  /* if the new serving cell contains a cell broadcast channel */
  if ( gsm->cbch_channel_information_valid )
  {
    /* Send a message to the Cell Broadcast task to inform it that messages are broadcast */
    rr_send_cell_change_ind(
      gsm->location_area_identification,
      gsm->cell_identity,
      gsm->cbch_channel_information_valid,
      gas_id
    );
  }
  else
  {
    rr_send_cell_change_ind(
      gsm->location_area_identification,
      gsm->cell_identity,
      gsm->cbch_channel_information_valid,
      gas_id
    );
  }

  rr_send_cb_info(gsm->cbch_channel_information_valid, gas_id);

  /* Start signal strength refresh timer */
  init_signal_strength_indicator_handling(gas_id);

  /* reset the list of cells which L1 is assumed to be synchronised with */
  rr_gprs_clear_sync_cell_list(gas_id);

  /* always set the five second timer id to the serving five second timer */
  gsm->reselection_info.five_second_timer_id = RR_SERV_FIVE_SEC_C1_TIMER;

  log_all_gprs_parameters(cell_info, gas_id);

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_cell_info(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  ba_list       = rr_gprs_get_ba_gprs(gas_id);
  if (!rr_is_any_cell_selection_mode(gas_id))
  {
    uint8 i;

    (void)rr_acq_db_add_arfcn(
      rr_gprs_get_scell_arfcn(gas_id),
      rr_gprs_get_scell_plmn_id(gas_id),
      gas_id);

    for (i = 0; i < ba_list->no_of_entries; i++)
    {
      (void)rr_acq_db_add_arfcn(
        ba_list->BCCH_ARFCN[i],
        rr_gprs_get_scell_plmn_id(gas_id),
        gas_id
      );
    }
  }

  return;
} /* end rr_start_ccch51_idle_mode() */


/*===========================================================================

FUNCTION rr_gprs_check_surround_response_in_ba_list

DESCRIPTION
  This function scans the supplied BA list to determine whether the pending
  surround response ARFCN is present. If the pending ARFCN cannot be found,
  the waiting flag is cleared, as L1 will never respond to the request.

PARAMS
  ba_list_ptr - pointer to the list of channels to be checked

RETURN VALUE
  None

SIDE EFFECTS
  Flags that indicate a pending surround response may be cleared

===========================================================================*/
static void rr_gprs_check_surround_response_in_ba_list(
  inter_task_BCCH_allocation_T *ba_list_ptr,
  const gas_id_t gas_id
)
{
  if ((ba_list_ptr != NULL) && get_waiting_for_surround_response(gas_id))
  {
    boolean bMatched = FALSE;
    byte    n_arfcns = ba_list_ptr->no_of_entries;
    byte    i;

    for (i = 0; i < n_arfcns; i++)
    {
      bMatched = match_waiting_for_surround_response(ba_list_ptr->BCCH_ARFCN[i], gas_id);
      if (bMatched)
      {
        /* RR is waiting for a surround response for this BA list entry */
        /* so stop searching the list now, as this is perfectly normal  */
        break;
      }
    }

    if (bMatched == FALSE)
    {
      /* no entry in the BA list matches the pending surround response, so clear  */
      /* the "waiting" flags now - L1 will never be able to complete this request */
      clear_waiting_for_surround_response(gas_id);
      MSG_GERAN_HIGH_0_G("Cleared surround response, channel not in BA list");
    }
  }
}

/*===========================================================================

FUNCTION rr_gprs_update_l1_params

DESCRIPTION
  This function updates Layer 1 with new Idle Mode parameters, appropriate to the
  current serving cell mode (GSM, GPRS-51 or GPRS-52)

PARAMS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Either MPH_IDLE_UPDATE_REQ or MPH_GPRS_L1PARAM_UPDATE_REQ is sent to L1

===========================================================================*/

void rr_gprs_update_l1_params(
  const gas_id_t gas_id
)
{
  if (rr_gprs_l1_sys_info_update_allowed(gas_id))
  {
    gprs_scell_info_t  * scell_info = rr_gprs_get_scell_info(gas_id);
    grr_l1_idle_mode_t  l1_idle_mode = scell_info->l1_idle_mode;

    if ( l1_idle_mode == GRR_L1_IDLE_MODE_NONE )
    {
      MSG_GERAN_HIGH_0_G("Can't update params to L1");
    }
    else
    if ( l1_idle_mode == GRR_L1_IDLE_MODE_GSM )
    {
      rr_update_idle_mode(gas_id);
    }
    else
    {
      ccch_pccch_params_t           ccch_pccch_params;
      inter_task_BCCH_allocation_T *ba_list_ptr = NULL;

      ba_list_ptr = &(ccch_pccch_params.ccch_idle_mode_params.layer1_sys_info.BA_list);

      rr_gprs_get_l1_params(&ccch_pccch_params, gas_id);

      /* Send updated parameters to L1 */
      rr_send_mph_gprs_l1param_update_req(&ccch_pccch_params, gas_id);

      /* once the update has been sent to L1, scan the BA list to check */
      /* whether any ARFCN pending a surround response is still present */
      rr_gprs_check_surround_response_in_ba_list(ba_list_ptr, gas_id);
    }
  }

} /* end rr_gprs_update_l1_params() */


/*===========================================================================

FUNCTION rr_gprs_perform_logging

DESCRIPTION
  This function sends out log packets for the GPRS parameters indicated in the
  logging mask

PARAMS
  logging_mask - bit-mask of logging required
  cell_info - pointer to the cell info database

RETURN VALUE
  None

===========================================================================*/

void rr_gprs_perform_logging(
  uint16 logging_mask,
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{

  if ( logging_mask & GRR_LOG_MASK_GPRS_CELL_OPTIONS )
  {
    (void) rr_log_gprs_cell_options(&cell_info->bcch_sys_info.gprs_cell_options, gas_id);
  }

  if ( logging_mask & GRR_LOG_MASK_GPRS_POWER_CTRL_PARAMS )
  {
    (void) rr_log_gprs_power_control_parameters(
      cell_info->bcch_sys_info.power_control_params.alpha,
      cell_info->bcch_sys_info.power_control_params.t_avg_w,
      cell_info->bcch_sys_info.power_control_params.t_avg_t,
      cell_info->bcch_sys_info.power_control_params.pc_meas_chan,
      cell_info->bcch_sys_info.power_control_params.n_avg_i,
      gas_id
    );
  }

  if ( logging_mask & GRR_LOG_MASK_GPRS_GENERAL_PARAMS )
  {
    (void) rr_log_gprs_general_parameters(
      (uint8) cell_info->bcch_sys_info.spgc_ccch_sup,
      (uint8) cell_info->bcch_sys_info.priority_access_thr,
      (uint8) cell_info->bcch_sys_info.nc_meas_params_si13.network_control_order,
      (uint8) cell_info->bcch_sys_info.routing_area_code,
      (uint8) cell_info->bcch_sys_info.sgsnr,
      gas_id
    );
  }

  if ( logging_mask & GRR_LOG_MASK_GPRS_INDICATORS )
  {
    (void) rr_log_gprs_indicators(
      (uint8) cell_info->bcch_sys_info.ra_colour,
      (uint8) cell_info->bcch_sys_info.si13_position,
      gas_id
    );
  }

  if ( logging_mask & GRR_LOG_MASK_SI13_MOBILE_ALLOC )
  {
    if ( IS_MOBILE_ALLOC_VALID( cell_info->mobile_allocations.ma_valid_bitmap, MA_NUMBER_14 ) )
    {
      (void) rr_log_gprs_mobile_allocation_rrps_freq_list(
        &cell_info->mobile_allocations.gprs_mobile_allocation[MAP_MA_NUMBER_TO_OFFSET(MA_NUMBER_14)],
        gas_id
      );
    }
  }


} /* end rr_gprs_perform_logging() */


/*===========================================================================

FUNCTION rr_gprs_general_timer_expiry_handler

DESCRIPTION
  This function is called from RR when a timer expires
  Note: This is called irrespective of RR state (i.e. GRR may not be in control)

PARAMS
  timer_id
  rr_event

RETURN VALUE
  TRUE if the timer expiry has been fully processed; FALSE if processing should
  continue through RR/GRR main state machines

===========================================================================*/

boolean rr_gprs_general_timer_expiry_handler(
  rr_timer_id_T  timer_id,
  rr_event_T * new_rr_event,
  const gas_id_t gas_id)
{
  rr_event_T  rr_event = EV_NO_EVENT;
  boolean     timer_handled = TRUE;

  switch ( timer_id )
  {
    case GRR_SYSTEM_INFO_TIMER:
      /* GRR does not need to maintain this timer whilst it is inactive */
      if (rr_is_control_state(RR_INACTIVE, gas_id))
      {
        /* explicit cancellation of timer */
        (void)rr_timer_cancel(GRR_SYSTEM_INFO_TIMER, gas_id);
        MSG_GERAN_HIGH_0_G("GSM RR task cancelling sys info timer");
      }
      else
      {
        /* this function restarts the timer every time it is called */
        rr_gprs_ncell_sys_info_timer_expiry(gas_id);
      }
      break;

    case GRR_SYS_INFO_REFRESH_TIMER:
    case GRR_SYS_INFO_ACQUISITION_TIMER:
    case GRR_SYS_INFO_RETRY_TIMER:
      rr_event = rr_gprs_sys_info_timer_handler(timer_id, gas_id);
      break;

    case GPRS_RECENT_ACTIVITY_TIMER:
      gprs_recent_activity_timer_expiry(gas_id);
      break;

    case GRR_GUARD_TIMER:
      rr_event = guard_timer_expiry(gas_id);
      break;

    case RESELECTION_HYSTERESIS:
      (void) rr_resel_control(EV_RECENT_RESELECTION_TIMEOUT, NULL, gas_id);
      break;

    case GRR_PENALTY_TIMER_1:
    case GRR_PENALTY_TIMER_2:
    case GRR_PENALTY_TIMER_3:
    case GRR_PENALTY_TIMER_4:
    case GRR_PENALTY_TIMER_5:
    case GRR_PENALTY_TIMER_6:
      /*One of the Penalty timers has expired*/
      rr_gprs_update_penalty_timer_state(timer_id, gas_id);
      break;

    case RR_FIVE_SEC_C2_TIMER_1:
    case RR_FIVE_SEC_C2_TIMER_2:
    case RR_FIVE_SEC_C2_TIMER_3:
    case RR_FIVE_SEC_C2_TIMER_4:
    case RR_FIVE_SEC_C2_TIMER_5:
    case RR_FIVE_SEC_C2_TIMER_6:
      /*One of the 5 sec timers has expired*/
      rr_gprs_update_five_second_timer_state(timer_id, gas_id);
      break;

    case GRR_NACC_CONTAINER_TIMER_0:
    case GRR_NACC_CONTAINER_TIMER_1:
    case GRR_NACC_CONTAINER_TIMER_2:
    case GRR_NACC_CONTAINER_TIMER_3:
      /* A timer has expired for a NACC neighbour cell data container */
      rr_event = rr_gprs_nacc_handle_timer_expiry(timer_id, gas_id);
      break;

    case GRR_SURROUND_MEAS_TIMER:
#ifdef FEATURE_IDLE_DRX_SCALING
      /* When scaled DRX is in use do not clear the timer */
      if (!rr_idle_drx_scaling_in_use(gas_id))
#endif /* FEATURE_IDLE_DRX_SCALING */
      {
        rr_resel_clear_any_active_five_sec_timer(gas_id);
        /* just leave the GRR_SURROUND_MEAS_TIMER timer in its "expired" state */
      }
      rr_event = EV_NO_EVENT;
      break;
      
    case RR_INVALIDATE_PSCAN_RESULTS_TIMER:
      /* just leave the timer in its "expired" state */
      rr_event = EV_NO_EVENT;
      break;

    case T3158:
      if (rr_is_control_state(RR_GRR, gas_id))
      {
        /* this must be handled by rr_gprs_camped_timer_handler() */
        timer_handled = FALSE;
      }
      else if (rr_is_control_state(RR_GOING_INACTIVE, gas_id))
      {
        /* Ignore T3158 timer expiry because RR must have stopped this timer*/
      }
      else
      {
        (void)rr_gprs_nc_measurement_control(EV_T3158_TIMEOUT, NULL, gas_id);
      }
      break;

    case GRR_SYS_INFO_STATUS_TIMER_1:
    case GRR_SYS_INFO_STATUS_TIMER_2:
      rr_event = rr_gprs_sysinfo_status_timer_expired(timer_id, gas_id);
      break;

#ifdef FEATURE_GSM_DTM
    case T3148:
      (void) rr_dtm_control(EV_DTM_T3148_EXPIRY, NULL, gas_id);
      break;

    case RR_GRR_CHN_ASSIGN_REQ_GUARD_TIMER:
      (void) rr_dtm_control(EV_DTM_CHN_ASSIGN_REQ_GUARD_TIMER, NULL, gas_id);

#ifdef FEATURE_GSM_EDTM
    case T3196:
      rr_event = rr_gprs_edtm_control(EV_EDTM_T3196_EXPIRY, NULL, gas_id);
      break;
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_IDLE_DRX_SCALING
    case RR_IDLE_DRX_SCALING_SUSPEND_TIMER:
      rr_idle_drx_control(EV_IDLE_DRX_SCALING_SUSPEND_TIMEOUT, NULL, gas_id);
      break;
#endif /* FEATURE_IDLE_DRX_SCALING */

    default:
      timer_handled = FALSE;
      break;
  }

  (* new_rr_event) = rr_event;

  return timer_handled;

} /* end rr_gprs_general_timer_expiry_handler() */



/*===========================================================================

FUNCTION grr_start_gprs_recent_activity_timer

DESCRIPTION
  This function starts (or re-starts) the GPRS_RECENT_ACTIVITY_TIMER

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/

void grr_start_gprs_recent_activity_timer(
  const gas_id_t gas_id
)
{
  (void)rr_timer_cancel(GPRS_RECENT_ACTIVITY_TIMER, gas_id);

  MSG_GERAN_MED_1_G("Start GPRS recent activity timer (%dms)",(GRR_TIMEOUT_1_SECOND*5));
  (void)rr_timer_start(GPRS_RECENT_ACTIVITY_TIMER, (GRR_TIMEOUT_1_SECOND*5), gas_id);

} /* grr_start_gprs_recent_activity_timer() */



/*===========================================================================

FUNCTION grr_stop_gprs_recent_activity_timer

DESCRIPTION
  This function stops the GPRS_RECENT_ACTIVITY_TIMER.
  This is done after PS domain deactivation.

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/

void grr_stop_gprs_recent_activity_timer(
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(GPRS_RECENT_ACTIVITY_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped GPRS recent activity timer");
  }
} /* grr_stop_gprs_recent_activity_timer() */



/*===========================================================================

FUNCTION grr_has_there_been_recent_gprs_activity

DESCRIPTION
  This function returns TRUE/FALSE indicating if there has been GPRC activity
  recently (i.e. is the GPRS_RECENT_ACTIVITY_TIMER still running)

PARAMS
  None

RETURN VALUE
  TRUE if there has been recent GPRS activity; FALSE otherwise

===========================================================================*/

boolean grr_has_there_been_recent_gprs_activity(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_BPLMN_SEARCH_PRIORITY
  // If FEATURE_BPLMN_SEARCH_PRIORITY is defined, the GPRS RECENT ACTIVITY TIMER is not used (see CR 283381).
  // Note: This is always defined in custgsm.h for GOBI (see CR 152804)
  return FALSE;
#else
  return (rr_timer_is_running(GPRS_RECENT_ACTIVITY_TIMER, gas_id));
#endif /* FEATURE_BPLMN_SEARCH_PRIORITY */

} /* grr_has_there_been_recent_gprs_activity() */




/*===========================================================================

FUNCTION grr_start_guard_timer

DESCRIPTION
  This function starts the GRR guard timer. When this expires after the specified
  time, the specified event will be generated into GRR

PARAMS
  timeout_ms
  expiry_event

RETURN VALUE
  None

===========================================================================*/

void grr_start_guard_timer(
  dword timeout_ms,
  rr_event_T expiry_event,
  const gas_id_t gas_id
)
{
   // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  grr_stop_guard_timer(gas_id);

  rr_gprs_general_data_ptr->guard_timer_expiry_event = expiry_event;

  MSG_GERAN_MED_2_G("Starting GRR guard timer (%dms, ev=%d)",(int)timeout_ms,(int)expiry_event);

  (void)rr_timer_start(GRR_GUARD_TIMER, timeout_ms, gas_id);

} /* end grr_start_guard_timer() */



/*===========================================================================

FUNCTION grr_stop_guard_timer

DESCRIPTION
  This function stops the GRR guard timer.

PARAMS
  timeout_ms
  expiry_event

RETURN VALUE
  None

===========================================================================*/

void grr_stop_guard_timer(
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(GRR_GUARD_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Stopped GRR guard timer");
  }
} /* end grr_stop_guard_timer() */



/*===========================================================================

FUNCTION grr_is_ps_allowed_by_service_domain

DESCRIPTION
  This function returns TRUE if the service domain is set to CS_PS or PS_ONLY;
  FALSE otherwise

PARAMS
  None

RETURN VALUE
  TRUE / FALSE

===========================================================================*/

boolean grr_is_ps_allowed_by_service_domain(
  const gas_id_t gas_id
)
{
  boolean ps_allowed = TRUE;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
  {
    ps_allowed = FALSE;
  }

  return ps_allowed;

} /* end grr_is_ps_allowed_by_service_domain() */


/*****************************************************************************
***
***     Local Functions
***
*****************************************************************************/


/*===========================================================================

FUNCTION init_signal_strength_indicator_handling

DESCRIPTION
  This function initialises the signal strength indicator

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/

static void init_signal_strength_indicator_handling(
  const gas_id_t gas_id
)
{
  rr_measured_signal_data_t *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);

  if (!rr_timer_is_running(SIGNAL_STRENGTH_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Start signal strength timer");
    (void)rr_timer_start(SIGNAL_STRENGTH_TIMER, SIGNAL_STRENGTH_TIMEOUT, gas_id);
  }

  /* Send the Signal Strength Indication to UI */
  rr_send_cm_status_ind(gas_id);

  /* Reset signal quality */
  rr_measured_signal_data_ptr->current_signal_quality = RR_INVALID_RXQUAL_VALUE;

} /* end init_signal_strength_indicator_handling() */


/*===========================================================================

FUNCTION gprs_recent_activity_timer_expiry

DESCRIPTION
  This function sends out log packets for all GPRS parameters

PARAMS
  cell_info - pointer to the cell info database

RETURN VALUE
  None

===========================================================================*/

static void gprs_recent_activity_timer_expiry(
  const gas_id_t gas_id
)
{
  MSG_GERAN_MED_0_G("GPRS recent activity timer expiry");

#ifdef FEATURE_GERAN_BSR_PS_STATUS
  rr_mm_send_rr_ps_status_change_ind(
    FALSE,    // ps_is_active
    gas_id    // gas_id
  );
#endif

} /* gprs_recent_activity_timer_expiry() */



/*===========================================================================

FUNCTION guard_timer_expiry

DESCRIPTION
  This function is called when the guard timer expires. It returns the guard timer
  expiry event

PARAMS
  None

RETURN VALUE
  rr_event_T - guard timer expiry event

===========================================================================*/

static rr_event_T guard_timer_expiry(
  const gas_id_t gas_id
)
{
  rr_event_T expiry_event;
  rr_gprs_general_data_t *rr_gprs_general_data_ptr;

   // Obtain a pointer to the module data
  rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  expiry_event = rr_gprs_general_data_ptr->guard_timer_expiry_event;

  MSG_GERAN_HIGH_0_G("GRR guard timer expiry");

  rr_gprs_general_data_ptr->guard_timer_expiry_event = EV_NO_EVENT;

  return expiry_event;

} /* end guard_timer_expiry() */


/*===========================================================================

FUNCTION grr_throwaway_saved_message

DESCRIPTION
  Removed any saved message from the saved message queue

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/

void grr_throwaway_saved_message(
  const gas_id_t gas_id
)
{
  rr_cmd_bdy_type *message_ptr;

  if (EV_INPUT_MESSAGE == rr_process_saved_message(&message_ptr, gas_id))
  {
    IMH_T *message_header_ptr = (IMH_T *)message_ptr;

    if ((message_header_ptr->message_set == MS_MM_RR) &&
        (message_header_ptr->message_id  == (byte)RR_EST_REQ))
    {
      MSG_GERAN_MED_0_G("Throwing away saved RR_EST_REQ message");
    }
  }

  return;
} /* end grr_throwaway_saved_message() */


/*===========================================================================

FUNCTION log_all_gprs_parameters

DESCRIPTION
  This function sends out log packets for all GPRS parameters

PARAMS
  cell_info - pointer to the cell info database

RETURN VALUE
  None

===========================================================================*/

static void log_all_gprs_parameters(
  gprs_scell_info_t *cell_info,
  const gas_id_t gas_id
)
{
  uint16  logging_mask;

  logging_mask = GRR_LOG_MASK_GPRS_CELL_OPTIONS       |
                 GRR_LOG_MASK_GPRS_POWER_CTRL_PARAMS  |
                 GRR_LOG_MASK_GPRS_GENERAL_PARAMS     |
                 GRR_LOG_MASK_GPRS_INDICATORS         |
                 GRR_LOG_MASK_SI13_MOBILE_ALLOC;

  rr_gprs_perform_logging(logging_mask, cell_info, gas_id);

} /* end log_all_gprs_parameters() */




/* ODM INFORMATION ACCESS FUNCTION */



void rr_gprs_odm_info_rach_attempt(
  const gas_id_t gas_id
)
{
   // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  rr_gprs_general_data_ptr->odm_info_rach_counter += 5;

} /* end rr_gprs_odm_info_rach_attempt() */


void rr_gprs_odm_info_rach_success(
  const gas_id_t gas_id
)
{
   // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  rr_gprs_general_data_ptr->odm_info_rach_counter -= 6;

  if ( rr_gprs_general_data_ptr->odm_info_rach_counter < 0 )
  {
    rr_gprs_general_data_ptr->odm_info_rach_counter = 0;
  }

} /* end rr_gprs_odm_info_rach_success() */


uint8 gprs_odm_info_get_rach_counter(
  void
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  return (uint8) rr_gprs_general_data_ptr->odm_info_rach_counter;

} /* end gprs_odm_info_get_rach_counter() */


/* Returns the current signal strength in dBm */
int8 gprs_odm_info_get_rx_lev_dBm(
  void
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  int8 rx_lev_dBm;
  rr_measured_signal_data_t *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);

  if ( rr_measured_signal_data_ptr->current_signal_strength <= 63 )
  {
    rx_lev_dBm = (int8) rxlev_to_dbm( rr_measured_signal_data_ptr->current_signal_strength );
  }
  else
  {
    rx_lev_dBm = (int8) rxlev_to_dbm( 63 );
  }

  return rx_lev_dBm;

} /* end gprs_odm_info_get_rx_lev_dBm() */


/* Returns the current signal strength in dBm */
uint8 gprs_odm_info_get_rx_lev_status(
  void
)
{
  int8 rx_lev_dBm = gprs_odm_info_get_rx_lev_dBm();

  if ( rx_lev_dBm < -85 )
  {
    return 1;
  }
  else
  {
    return 0;
  }

} /* end gprs_odm_info_get_rx_lev_dBm() */


/* Returns a value in the range 2 to 9, which is BS_PA_MFRMS from the Control Channel Desription IE */
uint8 gprs_odm_info_get_paging_multiframe(
  void
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS;

} /* end gprs_odm_info_get_paging_multiframe() */


/* Returns: 0 - PBCCH not present; 1 - PBCCH present, MS camped on BCCH; 2 - MS camped on PBCCH */
uint8 gprs_odm_info_get_pbcch_status(
  void
)
{
  return 0;

} /* end gprs_odm_info_get_pbcch_status() */


/* Returns the number or neighbour cells */
uint8 gprs_odm_info_get_num_of_neighbour_cells(
  void
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  gprs_ncells_info_t  * ncells_info = rr_gprs_get_ncells_info(gas_id);

  return ncells_info->gsm_neighbour_cell_list.num_of_ncells;

} /* end gprs_odm_info_get_num_of_neighbour_cells() */

/* Returns the current cell capability { UNKNOWN, GSM, GPRS or EGPRS } */
gprs_odm_cell_type_t gprs_odm_info_get_cell_type(
  void
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

   // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  return rr_gprs_general_data_ptr->odm_cell_type;

} /* end gprs_odm_info_get_cell_type() */


#ifndef PERLUTF /* these two ODM functions are removed for PERLUTF builds */

void gprs_odm_register_cell_type_cb(
  gprs_odm_cell_type_cb_fn_t cb_fn
)
{
  const gas_id_t gas_id = GERAN_ACCESS_STRATUM_ID_1;

  // Obtain a pointer to the module data
  rr_gprs_general_data_t *rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  rr_gprs_general_data_ptr->odm_cell_type_cb_fn = cb_fn;

  if ( cb_fn )
  {
    MSG_GERAN_HIGH_0_G("ODM cell type callback registered");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("ODM cell type callback deregistered");
  }

} /* end gprs_odm_register_cell_type_cb() */

void gprs_odm_deregister_cell_type_cb(
  void
)
{
  gprs_odm_register_cell_type_cb( NULL );

} /* end gprs_odm_deregister_cell_type_cb() */

#endif /* PERLUTF */

void rr_gprs_odm_update_cell_type(
  boolean in_service,
  const gas_id_t gas_id
)
{
  gprs_odm_cell_type_t old_cell_type;
  rr_gprs_general_data_t *rr_gprs_general_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_general_data_ptr = rr_gprs_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_general_data_ptr);

  old_cell_type = rr_gprs_general_data_ptr->odm_cell_type;

  if ( in_service )
  {
    gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

    if ( cell_info->gsm.gprs_indicator )
    {
      gprs_cell_options_t *gprs_cell_options_ptr = NULL;

      if ( cell_info->bcch_sys_info.si13_data_valid_flag )
      {
        gprs_cell_options_ptr = &cell_info->bcch_sys_info.gprs_cell_options;
      }

      if ( gprs_cell_options_ptr &&
           gprs_cell_options_ptr->optional_extension_info_flag &&
           gprs_cell_options_ptr->optional_extension_info.egprs_supported_flag )
      {
        rr_gprs_general_data_ptr->odm_cell_type = ODM_CELL_TYPE_EGPRS;
      }
      else
      {
        rr_gprs_general_data_ptr->odm_cell_type = ODM_CELL_TYPE_GPRS;
      }
    }
    else
    {
      rr_gprs_general_data_ptr->odm_cell_type = ODM_CELL_TYPE_GSM;
    }
  }
  else
  {
    rr_gprs_general_data_ptr->odm_cell_type = ODM_CELL_TYPE_UNKNOWN;
  }

  if ( rr_gprs_general_data_ptr->odm_cell_type != old_cell_type )
  {
    MSG_GERAN_HIGH_2_G("ODM reported cell type changed %d->%d",old_cell_type,rr_gprs_general_data_ptr->odm_cell_type);

    if ( rr_gprs_general_data_ptr->odm_cell_type_cb_fn )
    {
      (* rr_gprs_general_data_ptr->odm_cell_type_cb_fn)( rr_gprs_general_data_ptr->odm_cell_type );
    }
  }

} /* end rr_gprs_odm_update_cell_type() */

rrps_store_update_data_t *rr_get_rrps_store_update(
  const gas_id_t gas_id
)
{
  return (rrps_store_update_data_t *)GPRS_MEM_MALLOC(sizeof(rrps_store_update_data_t));
}

void rr_free_rrps_store_update(
  rrps_store_update_data_t *ptr,
  const gas_id_t gas_id
)
{
  if (ptr != NULL)
  {
    GPRS_MEM_FREE(ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("rr_free_rrps_store - trying to free a NULL ptr");
  }
}

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
boolean rr_is_read_pch_during_transfer_allowed(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_nv_read_pch_during_transfer_is_plmn_match(rr_PLMN_request_details_ptr->requested_PLMN_id, gas_id))
  {
    MSG_HIGH("PLMN in white list PCH  reading during transfer allowed",0,0,0);    
    return TRUE;
  }

  return FALSE; 
}
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

/* EOF */
