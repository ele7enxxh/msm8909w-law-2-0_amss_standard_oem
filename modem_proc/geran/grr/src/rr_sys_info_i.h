#ifndef RR_SYS_INFO_I_H
#define RR_SYS_INFO_I_H

/*! \file rr_sys_info.h 
 
  Definitions shared between rr_sys info_*.c source files.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_i.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_gprs_defs.h"
#include "csn.h"
#include "timers.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Allow a further 8 secs to read SI13 */
#define SI13_SAMPLE_TIMEOUT_EXTENSION  8000

/* 30s sys info refresh period (BCCH and PBCCH) */
#define SYS_INFO_REFRESH_PERIOD  30000

/* The masks below define the SIs required for a refresh of all BCCH SI */
/* Note: These do not include SI7 or SI8, as these contain information only relevant to a neighbour cell */

#define MASK_FULL_BSI_ACQUISITION_NO_SI13  ( MASK_SI_1                          |  \
                                             MASK_SI_2                          |  \
                                             MASK_SI_2_BIS                      |  \
                                             MASK_SI_2_TER | MASK_SI_2_TER_REST |  \
                                             MASK_SI_2_QUATER                   |  \
                                             MASK_SI_3                          |  \
                                             MASK_SI_4 )

#define MASK_FULL_BSI_ACQUISITION          ( MASK_FULL_BSI_ACQUISITION_NO_SI13  |  \
                                             MASK_SI_13 )

#define SYS_INFO_UPDATE_IDENTITY_L1   (1 << 0)
#define SYS_INFO_UPDATE_IDENTITY_MM   (1 << 1)
#define SYS_INFO_UPDATE_IDENTITY_MAC  (1 << 2)

#define MAX_NUM_OF_RFL  16 /*  RFL_NUMBER 0->15 */

#define INVALID_CHANGE_MARK  0xFF

#define CCCH_ACCESS_INFO  0x00 /* PSI14 equivalent to SI13  (BCCH present)  */
#define PCCCH_ACCESS_INFO 0x01 /* PSI14 equivalent to PSI13 (PBCCH present) */
#define PCCCH_DTM_INFO    0x02 /* additional mask for PSI14 during DTM      */
#define PCCCH_EDTM_INFO   0x04 /* additional mask for PSI14 during EDTM     */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef uint8 gprs_sys_info_update_identities_t;

typedef struct
{
  uint8                        bcch_change_mark;
  uint8                        si_change_field;
  uint8                        si13_change_mark;
  ie_decode_status_t           mobile_allocation_status;
  gprs_mobile_allocation_ie_t  mobile_allocation_ie;
  gprs_mobile_allocation_t     mobile_allocation;
  struct
  {
    struct
    {
      uint8                          rac;
      uint8                          spgc_ccch_sup;
      uint8                          priority_access_thr;
      uint8                          network_control_order;
      gprs_cell_options_t            gprs_cell_options;
      global_power_control_params_t  power_control_params;

    }                           bcch;
  }                            bcch_pbcch;
  sgsnr_t                      sgsnr;
  byte                         si_status_ind;
  boolean                      LB_MS_TXPWR_MAX_CCH_valid;
  byte                         LB_MS_TXPWR_MAX_CCH;
} si13_data_t;

typedef enum
{
  SIEV_NO_EVENT,
  SIEV_RESET_SOFTWARE,
  SIEV_STOP_AND_RESET_SOFTWARE,
  SIEV_INPUT_MESSAGE,
  SIEV_SYS_INFO_PART_REFRESH,
  SIEV_SYS_INFO_FULL_REFRESH,
  SIEV_SYS_INFO_FULL_REFRESH_PCCCH_CAMPED,
  SIEV_SYS_INFO_REFRESH_TIMER_EXPIRY,
  SIEV_SYS_INFO_RETRY_TIMER_EXPIRY,
  SIEV_SYS_INFO_ACQUISITION_TIMER_EXPIRY,
  SIEV_SYS_INFO_ACQUISITION_COMPLETE,
  SIEV_SYS_INFO_ACQUISITION_FAILURE,
  SIEV_GPRS_SUSPEND,
  SIEV_GPRS_SUSPEND_NO_SI13,
  SIEV_GPRS_SUSPENDED,
  SIEV_PSI13_RECEIVED,
  SIEV_CAMPED_ON_CELL,
  SIEV_READ_SYS_INFO_FAILURE_L1_IN_TRANSFER,
  SIEV_READ_SYS_INFO_FAILURE_UNSPECIFIED,
  SIEV_SUSPEND_SYS_INFO_ACQUISITION,
  SIEV_ACTIVATE_SYS_INFO_ACQUISITION,
  SIEV_CONTINUE_SYS_INFO_ACQUISITION,
  SIEV_SYS_INFO_MESSAGE_RECEIVED,
  SIEV_ENTER_PAGING_REORG,
  SIEV_EXIT_PAGING_REORG,
  SIEV_SI13_ADDED,
  SIEV_SI13_REMOVED,
  SIEV_CHANGE_L1_IDLE_MODE,
  SIEV_MPH_SELECT_SPECIFIC_BCCH_CNF,
  SIEV_MPH_START_IDLE_MODE_CNF,
  SIEV_SYS_INFO_EXPIRY_DURING_CS_CONN,
  SIEV_MPH_SELECT_SPECIFIC_BCCH_FAIL
} grr_siev_t;

typedef enum
{
  PART_SYS_INFO_ACQUISITION,
  FULL_SYS_INFO_ACQUISITION
} partial_or_full_acquisition_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* In rr_sys_info_main.c */
extern void rr_gprs_start_sys_info_refresh_timer(const gas_id_t gas_id);
extern void rr_gprs_stop_sys_info_refresh_timer(const gas_id_t gas_id);
extern void rr_gprs_start_sys_info_retry_timer(uint32, const gas_id_t gas_id);
extern void rr_gprs_stop_sys_info_retry_timer(const gas_id_t gas_id);
extern void rr_gprs_start_sys_info_acquisition_timer(uint32, const gas_id_t gas_id);
extern void rr_gprs_stop_sys_info_acquisition_timer(const gas_id_t gas_id);
extern grr_siev_t rr_gprs_camped_sys_info_acquisition_complete(const gas_id_t gas_id);
extern grr_siev_t grr_rrev_to_siev(rr_event_T rr_event);
extern rr_event_T grr_siev_to_rrev(grr_siev_t siev);
extern grr_siev_t grr_timerid_to_siev(rr_timer_id_T timer_id);
extern grr_siev_t grr_align_sys_info_acquisition_with_cell_capabilities(const gas_id_t gas_id);

/* In rr_sys_info_pcell.c */
extern void rr_init_pcell_data(const gas_id_t gas_id);

/* In rr_sys_info_si.c */
extern void bsi_reset_message_tracking_data(const gas_id_t gas_id);
extern void rr_init_sys_info_si(const gas_id_t gas_id);
extern void rr_gprs_bcch_sys_info_post_processing(rr_si_type_t, ARFCN_T, const gas_id_t gas_id);
extern void rr_process_si13_from_bcch(ARFCN_T bcch_arfcn, dl_unit_data_ind_T  *message_ptr, const gas_id_t gas_id);
extern void rr_gprs_process_psi13_ccch_camped(si13_data_t *, gprs_scell_info_t *, const gas_id_t gas_id);
extern void rr_gprs_store_si13_or_psi13_data(
  si13_data_t *,
  gprs_scell_info_t *,
  rrps_store_update_data_t *,
  const gas_id_t gas_id
);
extern rr_event_T rr_gprs_bsi_acquisition_timer_handler(rr_timer_id_T, const gas_id_t gas_id);
extern void bsi_enter_paging_reorg(const gas_id_t gas_id);
extern void bsi_exit_paging_reorg(const gas_id_t gas_id);
extern boolean rr_gprs_process_si13_mobile_alloc(
  si13_data_t *,
  cell_channel_description_T *,
  gprs_scell_info_t *,
  const gas_id_t gas_id
);
extern void bsi_activate_camped_sys_info_handling(const gas_id_t gas_id);
extern grr_siev_t bsi_acquisition_control(grr_siev_t, const void *, const gas_id_t gas_id);
extern void bsi_update_messages_being_broadcast(
  gprs_scell_info_t *,
  uint16,
  uint16,
  uint16
);
extern void bsi_set_messages_required_for_acquisition(
  uint16,
  uint16,
  boolean,
  const gas_id_t gas_id
);
extern void rr_gprs_process_psi14_as_si13(
  packet_psi14_t *,
  gprs_scell_info_t *,
  const gas_id_t gas_id
);

/* In rr_gprs_freq_lists.c */
extern void rr_gprs_conv_mobile_alloc_to_rrps_format(
  const gprs_mobile_allocation_t * new_mobile_alloc,
  rrps_frequency_list_t *          store
);

#endif  /* #ifndef RR_SYS_INFO_I_H */

/* EOF */

