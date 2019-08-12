#ifndef GPL1_PTCCH_H
#define GPL1_PTCCH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 PTCCH module

GENERAL DESCRIPTION
   This module contains functions for GPRS timing advance.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---     ----------------------------------------------------------
15/08/13   sk      CR524039 GBTA bringup changes
29/07/13   sk      CR519663 Partial QBTA bringup changes
05/03/13   pg      CR458425: correct input parameters for PTCCH metrics callback
30/03/09   cs      Lint fix for deprecated valid variable
19/01/09   nt      CR151077 - Enhancement to SNR based reselection
23/11/05   og      Update tx call back routines to provide more information.
02/06/05   og      Removal of header-header includes.
18/03/05   og      Fix to PDAN vs TBF suspension clash handling.
11/10/05   og      Added/modified ptcch functional interfaces.
30/11/04   og      Enable interference measurements when cuta is ON.
13/05/04   rm      Structure realignment to minimise padding
28/04/04   rm      Initial version.
===========================================================================*/

/* Avoid including header files */

void gpl1_gprs_add_ul_ta_params(
     timing_advance_params_T timing_advance_params,
     boolean l1_called_from_sngl_blk,
     uint8 single_block_ta_value, gas_id_t gas_id
     );

void gpl1_gprs_add_to_active_ul_ta_params(
     timing_advance_params_T ta_params,
     boolean l1_called_from_sngl_blk,
     uint8 single_block_ta_value, gas_id_t gas_id
     );

void gpl1_gprs_add_to_active_dl_ta_params(
     timing_advance_params_T ta_params, gas_id_t gas_id
     );

void gpl1_gprs_add_to_pending_dl_ta_params(
     timing_advance_params_T ta_params, gas_id_t gas_id
     );

boolean gpl1_gprs_config_global_ta_param(
     global_packet_ta_T global_ta, gas_id_t gas_id
     );

void gpl1_gprs_add_global_ta_param(
     global_packet_ta_T global_ta,gas_id_t gas_id
     );

void gpl1_gprs_reset_pending_ta_params(
     gas_id_t gas_id
     );

void gpl1_gprs_swap_dl_cont_ta_to_ul(
     gas_id_t gas_id
     );

void gpl1_gprs_swap_ul_cont_ta_to_dl(
     gas_id_t gas_id
     );

void gpl1_gprs_setup_cont_ta_params(
     gas_id_t gas_id
     );

void gpl1_gprs_config_ta_param(
     boolean  transfer_init_ind,
     uint8    tsc,
     uint8    lowest_dl_timeslot,
     gas_id_t gas_id
     );

void gpl1_gprs_schedule_ptcch_tx(
     ARFCN_T  gprs_arfcn,
     boolean access_burst_type, gas_id_t gas_id
     );

void gpl1_gprs_schedule_ptcch_rx(
     gas_id_t gas_id
     );

void gpl1_gprs_update_ptcch_ftn (
     gl1_msg_frame_tick_type  *ftn,
     gas_id_t gas_id
     );

boolean gpl1_gprs_timing_advance_valid(
     gas_id_t gas_id
     );

boolean  gpl1_gprs_find_next_tai_during_suspension
         ( uint32 tbf_suspend_start,
           uint32 tbf_suspend_end,gas_id_t gas_id );

uint8 gpl1_gprs_get_global_ta_timeslot(
     gas_id_t gas_id
     );

boolean gpl1_gprs_get_ptcch_snr_status(
     gas_id_t gas_id
     );

boolean gpl1_gprs_get_ptcch_snr_valid(
     gas_id_t gas_id
     );

void gpl1_gprs_reset_ptcch_snr_valid(
     gas_id_t gas_id
     );

/* This function informs the ptcch module of a change
 * in the lowest downlink timeslot and allows the module
 * to update the relative timeslot used to task NPL1.
 * This function conveys this information to any active
 * NPL1 handlers.
 */
void  gpl1_gprs_ptcch_set_lowest_dl_timeslot( uint8  timeslot,gas_id_t gas_id );

/* This function enables the ptcch module to take into
 * account the new lowest downlink timeslot in future
 * rx\tx activities.
 */
void  gpl1_gprs_ptcch_reconfig( gas_id_t gas_id );


#ifdef FEATURE_GSM_COEX_SW_CXM
boolean gpl1_check_if_ptcch_scheduled(uint32 FN,uint8 *ptcch_tn,boolean *dirn,gas_id_t gas_id);
#endif /* FEATURE_GSM_COEX_SW_CXM */

#ifdef FEATURE_GPRS_GBTA
boolean gpl1_gprs_is_ptcch_active(gas_id_t gas_id);
#endif /* FEATURE_GPRS_GBTA */

#include "geran_variation.h"
/*===========================================================================

FUNCTION gpl1_is_pdtch_snr_bad

DESCRIPTION
  This function checks the PDTCH SNR to evaluate if it is below the current
  defined SNR threshold. The boolean is returned to report the result to
  the calling function.

DEPENDENCIES

RETURN VALUE
  boolean TRUE= SNR is below threshold

SIDE EFFECTS
  None

===========================================================================*/
boolean gpl1_is_pdtch_snr_bad( gas_id_t gas_id  );
/*===========================================================================

FUNCTION gpl1_reset_pdtch_snr_variables

DESCRIPTION
  This function clears out the store that calculates the SNR average on PDTCH

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gpl1_reset_pdtch_snr_variables( gas_id_t gas_id );


#endif /* GPL1_PTCCH_H */





