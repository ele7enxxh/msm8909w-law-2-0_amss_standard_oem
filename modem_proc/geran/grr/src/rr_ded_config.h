#ifndef RR_DED_CONFIG_H
#define RR_DED_CONFIG_H

/*! \file rr_ded_config.h
 
  This is the header file for rr_ded_config.c.
  This module contains access functions to data used during Dedicated.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ded_config.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_l2.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_MAX_DED_SIS 4

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  dl_unit_data_ind_T msg_data[RR_MAX_DED_SIS];
  boolean            msg_rcvd[RR_MAX_DED_SIS];
} rr_dedicated_sys_info_cache_T;

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  typedef struct
  {
    boolean measured;
    uint16  uarfcn;
  } rr_pseudo_fr_freq_list_t;

  typedef struct
  {
    boolean                       active;
    int16                         rscp_threshold; /*-120dBm .. -25dBm*/
    uint8                         num_freq;
    rr_pseudo_fr_freq_list_t      freq_list[MAX_UTRAN_TDS_FREQUNECIES];
  } rr_pseudo_fr_info_t;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

typedef struct
{
  rr_dedicated_sys_info_cache_T sys_info_cache;
  frequency_redefinition_T      frequency_redef_details;
  channel_mode_modify_T         channel_mode_modify_details;
  handover_details_T            handover_details;
  assignment_details_T          assignment_details;
  channel_spec_T                curr_channel_spec;
  channel_spec_T                prev_channel_spec;
  multirate_config_T            prev_amr_config;
  multirate_config_T            curr_amr_config;
  multirate_config_T            next_amr_config;
  link_status_T                 l2_link_status;
  boolean                       cipher_mode_command_received;
  boolean                       IMEI_requested_by_nw_command;
  byte                          handover_cmd_length;
  boolean                       ciphering_started;
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  rr_pseudo_fr_info_t           tds_pseudo_fr;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
} rr_dedicated_data_T;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Access function for dedicated data.
 * 
 * \param gas_id 
 * 
 * \return rr_dedicated_data_T* 
 */
extern rr_dedicated_data_T *rr_get_dedicated_data_ptr(const gas_id_t gas_id);

/*!
 * \brief Access function for current channel specification.
 * 
 * \param dedic_ptr 
 * \param gas_id 
 * 
 * \return channel_spec_T* 
 */
extern channel_spec_T * rr_get_channel_spec(rr_dedicated_data_T *dedic_ptr, const gas_id_t gas_id);

/*!
 * \brief Update the dedicated config to indicate that there is no channel connected.
 * 
 * \param gas_id (in)
 */
extern void rr_ded_set_l1_chan_is_not_connected(const gas_id_t gas_id);

/*!
 * \brief Indicates if L1 has a dedicated channel connected or not.
 * 
 * \param gas_id 
 * 
 * \return boolean - TRUE if L1 has a channel connected, FALSE otherwise
 */
extern boolean rr_ded_l1_chan_is_connected(const gas_id_t gas_id);

/*!
 * \brief Invalidates (clears) the multirate parameters given.
 * 
 * \param multirate_ptr 
 */
extern void rr_invalidate_multirate_config(multirate_config_T *multirate_ptr);

/*!
 * \brief Restores the previous dedicated configuration.
 * 
 * \param gas_id 
 */
extern void rr_restore_previous_dedicated_config(const gas_id_t gas_id);

/*!
 * \brief Saves a copy of the current dedicated configuration.
 * 
 * \param gas_id 
 */
extern void rr_preserve_current_dedicated_config(const gas_id_t gas_id);

/*!
 * \brief Start using the new multirate configuration.
 * 
 * \param dedic_ptr 
 */
extern void rr_switch_to_next_multirate_config(rr_dedicated_data_T *dedic_ptr);

/*!
 * \brief Start using the existing multirate configuration.
 * 
 * \param dedic_ptr 
 */
extern void rr_use_existing_multirate_config(rr_dedicated_data_T *dedic_ptr);

/*!
 * \brief Updates the L2 link status.
 * 
 * \param link_status 
 * \param gas_id 
 */
extern void rr_set_link_status(link_status_T link_status, const gas_id_t gas_id);

/*!
 * \brief Returns the current L2 link status.
 * 
 * \param gas_id 
 * 
 * \return link_status_T 
 */
extern link_status_T rr_get_link_status(const gas_id_t gas_id);

/*!
 * \brief Discards the dedicated run-time data.
 * 
 * \param gas_id 
 */
extern void rr_ded_config_discard_data(const gas_id_t gas_id
);

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/*!
 * \brief Clears TDS-related dedicated data.
 * 
 * \param gas_id 
 */
void rr_tds_pseudo_fr_reset_dedicated_info(const gas_id_t gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

/*!
 * \brief Module initialisation function to be called at task start.
 */
extern void rr_ded_config_task_start_init(
  void
);

#endif /* RR_DED_CONFIG_H */

/* EOF */

