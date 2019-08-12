/*! \file rr_data.c

  This module contains RR-global data declarations and memory management functionality

                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_data.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include <stdio.h>
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "rex.h"
#include "timers.h"
#include "gs.h"
#include "rr_seg_load.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "err.h"
#include "rr_defs.h"
#include "gprs_mem.h"
#include "rr_general.h"
#include "rr_decode_ie.h"
#ifdef FEATURE_LTE
#include "lte_irat_types.h"
#endif /* FEATURE_LTE */
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/**
 * description of each OTA message received by the RR. Contains information about
 * IEs contained in the message (Mandatory Vs. Optional/Conditional, type, length,
 * etc.)
 */
static message_list_T  message_list[NUM_GERAN_DATA_SPACES][NUMBER_OF_MT_RR_MESSAGES];

/**
 * Maximum index in the message_list data structure
 */
static byte  max_message_index[NUM_GERAN_DATA_SPACES];

/**
 * Information about the PLMN currently requested by the MM and the cell selection
 * mode of the RR(normal vs. any cell selection). Also contains flags that are used
 * during cell selection.
 */
static rr_PLMN_request_details_T      rr_PLMN_request_details[NUM_GERAN_DATA_SPACES];

/**
 * Information about the PLMN list request from MM
 */
static rr_PLMN_list_req_info_T   rr_plmn_list_req_info[NUM_GERAN_DATA_SPACES];


/**  Reason for which GSM L1 will be stopped
 *
 */
static rr_l1_gsm_stop_reason_T  rr_l1_gsm_stop_reason[NUM_GERAN_DATA_SPACES];

static rr_l1_gsm_deact_reason_T   rr_l1_gsm_deact_reason[NUM_GERAN_DATA_SPACES];

static si2ter_instances_T  *si2ter_instances_ptrs[NUM_GERAN_DATA_SPACES] = {0};
static instances_T         *si2q_instances_ptrs[NUM_GERAN_DATA_SPACES] = {0};
static instances_T         *mi_instances_ptrs[NUM_GERAN_DATA_SPACES] = {0};

/* used for measurement reporting */
static wcdma_ncell_list_data_t  wcdma_ncell_list_data[NUM_GERAN_DATA_SPACES];
static surr_wcdma_ded_measurements_T  wcdma_ded_meas[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_GSM_TDS
static rr_ded_irat_measurements_t  tds_ded_meas[NUM_GERAN_DATA_SPACES];
#endif /*FEATURE_GSM_TDS*/

/**
 * stores surrounding measurement information in dedicated mode (move to cell_reestablish)
 */
static surrounding_measurement_T      surrounding_measurement[NUM_GERAN_DATA_SPACES];

static rr_measured_signal_data_t  rr_measured_signal_data[NUM_GERAN_DATA_SPACES];

static rr_nas_message_buffer_T  *rr_nas_message_buffer[NUM_GERAN_DATA_SPACES][MAX_NAS_MESSAGE_BUFFER_SIZE];

static rr_chan_rel_cell_selection_indicator_t  rr_cell_select_indicator[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_LTE
/* This is a pointer to a structure containing the current individual priorities. */
static rr_individual_priorities_t *individual_priorities_ptr[NUM_GERAN_DATA_SPACES] = {0};
#endif /* FEATURE_LTE */

/**
  This is a pointer to a structure containing the current set of modifications
  to broadcast system information, as received in Packet Cell Change Order,
  Packet Measurement Order of X2G Cell Change Order (NCO only, in X2G CCO).

  @see rr_params_mods

  The rr_params_mods function returns the address of this pointer (a pointer
  to this pointer) to the caller.
*/
static rr_params_mods_t  * params_mods[NUM_GERAN_DATA_SPACES];
//static rr_params_mods_t ** params_mods_ptr = params_mods;

/**
  This is a pointer to a structure containing the set of modifications
  to broadcast system information, as received in Packet Cell Change Order,
  that have not yet been applied.

  @see rr_pcco_params_mods

  The rr_pcco_params_mods function returns the address of this pointer (a
  pointer to this pointer) to the caller.
*/
static rr_params_mods_t  * pcco_params_mods[NUM_GERAN_DATA_SPACES];
//static rr_params_mods_t ** pcco_params_mods_ptr = pcco_params_mods;


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

#ifdef FEATURE_LTE
/*!
 * \brief Returns a pointer to the individual priorities data.
 *
 * \return rr_individual_priorities_t**
 */
rr_individual_priorities_t ** rr_individual_priorities(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&individual_priorities_ptr[as_index]);
}
#endif /* FEATURE_LTE */

/**
  @brief Returns the address of the pointer to the current set of modifications
  to broadcast system information.
*/
rr_params_mods_t ** rr_params_mods(const gas_id_t gas_id)
{
  
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  
  return &(params_mods[as_index]);
}

/**
  @brief Returns the address of the pointer to the set of modifications to
  broadcast system information provided by a Packet Cell Change Order.
*/
rr_params_mods_t ** rr_pcco_params_mods(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &(pcco_params_mods[as_index]);
}


/*!
 * \brief Initialised the cell seleciton inidicator data.
 */
void rr_init_cell_selection_indicator(
  const gas_id_t gas_id
)
{
  rr_chan_rel_cell_selection_indicator_t *cell_sel_indicator_ptr = rr_get_cell_select_indicator_ptr(gas_id);

  memset(cell_sel_indicator_ptr, 0, sizeof(rr_chan_rel_cell_selection_indicator_t));
  cell_sel_indicator_ptr->valid = FALSE;
  cell_sel_indicator_ptr->type = UNKNOWN_TARGET;
}



/*!
 * \brief Returns a pointer to the SI2Q/MI instances data.
 *
 * \param si2q_or_mi
 * \param gas_id
 *
 * \return instances_T*
 */
instances_T *rr_get_instances_ptr(
  si2q_mi_instance_e_type si2q_or_mi,
  const gas_id_t gas_id
)
{
  instances_T *result_ptr = NULL;

  if (si2q_or_mi == SI2Q_INSTANCE)
  {
    uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    if (si2q_instances_ptrs[as_index] == NULL)
    {
      si2q_instances_ptrs[as_index] = (instances_T *)GPRS_MEM_CALLOC(sizeof(instances_T));
    }

    result_ptr = si2q_instances_ptrs[as_index];
  }
  else if (si2q_or_mi == MI_INSTANCE)
  {
    uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

    if (mi_instances_ptrs[as_index] == NULL)
    {
      mi_instances_ptrs[as_index] = (instances_T *)GPRS_MEM_CALLOC(sizeof(instances_T));
    }

    result_ptr = mi_instances_ptrs[as_index];
  }
  else
  {
    MSG_GERAN_ERROR_1_G("si2q_or_mi value invalid (%d)", (int)si2q_or_mi);
  }

  if (result_ptr != NULL)
  {
    result_ptr->si2q_or_mi = si2q_or_mi;
  }
  else
  {
    MSG_GERAN_FATAL_0_G("rr_get_instances_ptr() alloc failed");
  }

  return (result_ptr);
}

/*!
 * \brief Returns a pointer to the SI2ter instances data.
 * 
 * \param gas_id 
 * 
 * \return si2ter_instances_T* 
 */
si2ter_instances_T *rr_get_SI2ter_instances_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (si2ter_instances_ptrs[as_index] == NULL)
  {
    si2ter_instances_ptrs[as_index] = (si2ter_instances_T *)GPRS_MEM_CALLOC(sizeof(si2ter_instances_T));
  }

  if (si2ter_instances_ptrs[as_index] == NULL)
  {
    MSG_GERAN_FATAL_0_G("rr_get_instances_ptr() alloc failed");
  }

  return (si2ter_instances_ptrs[as_index]);
}


/*!
 * \brief Returns a pointer to the message list data.
 *
 * \return message_list_T*
 */
message_list_T *rr_get_message_list_ptr(
  const gas_id_t gas_id
)
{
  // Obtain data-space index
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&message_list[as_index][0]);
}


/*!
 * \brief Returns the number of elements in the message list.
 *
 * \return uint16
 */
uint16 rr_elements_in_message_list(
  const gas_id_t gas_id
)
{
  // Obtain data-space index
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (NELEMS(message_list[as_index]));
}


/*!
 * \brief Returns a pointer to the PLMN select request data.
 *
 * \return rr_PLMN_request_details_T*
 */
rr_PLMN_request_details_T *rr_get_PLMN_request_details_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&rr_PLMN_request_details[as_index]);
}


/*!
 * \brief Initialise the PLMN select request data (Note: Includes RAT Priority List)
 * 
 * \param gas_id 
 */
void rr_init_plmn_select_req_info(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T *data_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  // Zero the PLMN selection info structure
  memset(data_ptr, 0, sizeof(rr_PLMN_request_details_T));

  // Setup any default values which may be non-zero
  data_ptr->search_reason  = RR_AUTONOMOUS_CELL_SELECTION;
  data_ptr->service_domain = SYS_SRV_DOMAIN_CS_ONLY;

  return;
}


/*!
 * \brief Returns a pointer to the PLMN list request data.
 *
 * \return rr_PLMN_list_req_info_T*
 */
rr_PLMN_list_req_info_T *rr_get_plmn_list_req_info_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&rr_plmn_list_req_info[as_index]);
}


/*!
 * \brief Initialise the PLMN list request data
 * 
 * \param gas_id 
 */
void rr_init_plmn_list_req_info(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *data_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  // Zero the PLMN list info structure
  memset(data_ptr, 0, sizeof(rr_PLMN_list_req_info_T));

  // Setup any default values which may be non-zero
  data_ptr->network_selection_mode = SYS_NETWORK_SELECTION_MODE_NONE;
  data_ptr->trans_id = 0xff;
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  data_ptr->service_search = SYS_SERVICE_SEARCH_NONE;
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  return;
}


/*!
 * \brief Returns the L1 stop reason.
 *
 * This function will always return RR_L1_DEEP_SLEEP if FEATURE_WCDMA is not defined (as a mode change
 * should always result in GSM hardware being deactivated, otherwise this function will return whatever value
 * has previously been set with a call to rr_l1_gsm_stop_reason_set()
 *
 * \return rr_l1_gsm_stop_reason_T
 */
rr_l1_gsm_stop_reason_T rr_l1_gsm_stop_reason_get(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

#ifdef FEATURE_WCDMA
  return (rr_l1_gsm_stop_reason[as_index]);
#else
  /* If we are not a multi-mode device then we should always
  send DEEP_SLEEP reason to GL1, as we can only be switching
  modem off for deep sleep, or switching to CDMA mode, both
  of which require us to shut down the GSM hardware. */
  if (rr_l1_gsm_stop_reason[as_index] != RR_L1_DEEP_SLEEP)
  {
    MSG_GERAN_ERROR_2_G("rr_l1_gsm_stop_reason is %d, expected %d",
              rr_l1_gsm_stop_reason[as_index],
              RR_L1_DEEP_SLEEP);
  }
  return RR_L1_DEEP_SLEEP;
#endif /* FEATURE_WCDMA */
}


/*!
 * \brief Sets the L1 stop reason
 *
 * \param gsm_stop_reason
 * \param gas_id
 */
void rr_l1_gsm_stop_reason_set(
  rr_l1_gsm_stop_reason_T gsm_stop_reason,
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  rr_l1_gsm_stop_reason[as_index] = gsm_stop_reason;
}


/*!
 * \brief Gets the L1 deact reason
 *
 * \param gas_id
 *
 * \return rr_l1_gsm_deact_reason_T
 */
rr_l1_gsm_deact_reason_T rr_l1_gsm_deact_reason_get(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (rr_l1_gsm_deact_reason[as_index]);
}


/*!
 * \brief Sets the L1 deact reason
 *
 * \param gsm_deact_reason
 * \param gas_id
 */
void rr_l1_gsm_deact_reason_set(
  rr_l1_gsm_deact_reason_T gsm_deact_reason,
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  rr_l1_gsm_deact_reason[as_index] = gsm_deact_reason;
}


#ifdef FEATURE_GSM_TDS
/*!
 * \brief Returns a pointer to tds_ded_meas
 *
 * \return rr_ded_irat_measurements_t*
 */
rr_ded_irat_measurements_t *rr_get_tds_ded_meas_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&tds_ded_meas[as_index]);
}
#endif /*FEATURE_GSM_TDS*/


/*!
 * \brief Returns a pointer to wdma_ncell_list_data
 *
 * \return wcdma_ncell_list_data_t*
 */
wcdma_ncell_list_data_t *rr_get_wcdma_ncell_list_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&wcdma_ncell_list_data[as_index]);
}


/*!
 * \brief Returns a pointer to wcdma_ded_meas
 *
 * \return surr_wcdma_ded_measurements_T*
 */
surr_wcdma_ded_measurements_T *rr_get_wcdma_ded_meas_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&wcdma_ded_meas[as_index]);
}

static void rr_data_free_SI2ter_instances_data(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (si2ter_instances_ptrs[as_index] != NULL)
  {
    MSG_GERAN_HIGH_0_G("Freeing memory allocated for SI2ter instances!");
    GPRS_MEM_FREE(si2ter_instances_ptrs[as_index]);
    si2ter_instances_ptrs[as_index] = NULL;
  }
}

static void rr_data_free_SI2quater_instances_data(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  
  if (si2q_instances_ptrs[as_index] != NULL)
  {
    int i = 0;
    /* Free the memory allocated dynamically for structures assoiciated with si2q_params CR#718258*/
    for (i = 0; i < MAX_INSTANCES; ++i)
    {
      rr_si2q_params_free_children(&si2q_instances_ptrs[as_index]->instance[i].si2q_mi.si2q_params);
    }
    MSG_GERAN_HIGH_0_G("Freeing memory allocated for SI2quater instances!");
    GPRS_MEM_FREE(si2q_instances_ptrs[as_index]);
    si2q_instances_ptrs[as_index] = NULL;
  }
}

static void rr_data_free_mi_instances_data(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (mi_instances_ptrs[as_index] != NULL)
  {
    MSG_GERAN_HIGH_0_G("Freeing memory allocated for Measurement Information instances!!");
    GPRS_MEM_FREE(mi_instances_ptrs[as_index]);
    mi_instances_ptrs[as_index] = NULL;
  }
}


void rr_data_discard_data(const gas_id_t gas_id)
{
  MSG_GERAN_HIGH_0_G("Discarding SI2ter, SI2q and MI instances");
  rr_data_free_SI2ter_instances_data(gas_id);
  rr_data_free_SI2quater_instances_data(gas_id);
  rr_data_free_mi_instances_data(gas_id);
}

/*!
 * \brief Returns a poitner to rr_measured_signal_data
 *
 * \return rr_measured_signal_data_t*
 */
rr_measured_signal_data_t *rr_get_measured_signal_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&rr_measured_signal_data[as_index]);
}


/*!
 * \brief Returns a pointer to max_message_index
 *
 * \return byte*
 */
byte *rr_get_max_message_index_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&max_message_index[as_index]);
}


/*!
 * \brief Returns a pointer to rr_cell_select_indicator
 *
 * \return rr_chan_rel_cell_selection_indicator_t*
 */
rr_chan_rel_cell_selection_indicator_t *rr_get_cell_select_indicator_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&rr_cell_select_indicator[as_index]);
}


/*!
 * \brief Returns a pointer to surrounding_measurement
 *
 * \return surrounding_measurement_T*
 */
surrounding_measurement_T *rr_get_surrounding_measurement_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&surrounding_measurement[as_index]);
}


/*!
 * \brief Returns a pointer to rr_nas_message_buffer
 *
 * \return rr_nas_message_buffer_T**
 */
rr_nas_message_buffer_T **rr_get_nas_message_buffer_ptr_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return (&rr_nas_message_buffer[as_index][0]);
}

/* EOF */

