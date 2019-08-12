/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GRR MULTI SIM HEADER

GENERAL DESCRIPTION
  Provides Dual SIM support for GERAN Radio Resources (GRR)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2014 by QUALCOMM Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_multi_sim.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
31/05/13    SJW     Created from legacy rr_dual_sim.h
dd/mm/yy    SJW     Sample text for edit history
===========================================================================*/
#ifndef __RR_MULTI_SIM_H__
#define __RR_MULTI_SIM_H__

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "geran_dual_sim.h"
#include "gs.h"
#include "rr_defs.h"
#include "rr_control.h"

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#include "rr_gprs_defs.h"
#include "rr_task.h"
#include "cause.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "cb_rr.h"
#include "rr_main.h"
#include "mm_rr.h"

#define RR_MS_INVALID_L1_MESSAGE ((rr_l1_message_id_enum_T)MPH_INVALID_MESSAGE_ID)

/* Adjust this constant to alter the rate at which indeterminate block */
/* quality (where GL1 failed to read) is treated as bad block quality. */
/* A higher number will cause a lower Downlink Signalling Failure rate */
/* but will also make GRR stay on a blocked cell for a longer period   */
#define RR_MS_BLOCK_QUALITY_UNIT                              (2)
#define RR_MS_BLOCK_QUALITY_RATE  (RR_MS_BLOCK_QUALITY_UNIT << 3)

/* other DSDS specific definitions */
#define RR_MS_INTERFACE_TIMEOUT         4000
#define RR_MS_HEARTBEAT_SECONDS            1
#define RR_MS_HIBERNATE_SECONDS            4
#define RR_MS_HEARTBEAT_LIMIT          16384
#define RR_MS_INVALID_PAGING_GROUP    0xFFFF
#define RR_MS_INVALID_TIME_STAMP           0
#define RR_MS_SERVICE_REQUEST_TIMEOUT     10 /* 10 seconds */

typedef enum
{
  RR_PS_ABORT_NOT_ACTIVE = 0,     /* no PS abort procedure is in progress */
  RR_PS_ABORT_ACTIVE_WILL_RESUME, /* PS abort is in progress and will resume PS when it completes */
  RR_PS_ABORT_ACTIVE_TBF_BLOCKED  /* PS abort is in progress but only affects TBF establishments  */
} rr_ps_abort_block_t;

extern void                    rr_ms_cs_connected(const boolean, const gas_id_t);
extern boolean                 rr_ms_other_stack_in_dedicated(const gas_id_t);
extern void                    rr_ms_complete_nas_service_ind(rr_service_ind_T *, const gas_id_t);
extern void                    rr_ms_complete_nas_service_cnf(rr_plmn_select_cnf_T *, const gas_id_t);
extern void                    rr_ms_complete_nas_cb_ind(rr_cell_change_ind_T *, const gas_id_t);
extern void                    rr_ms_service_recovery_started(const gas_id_t);
extern void                    rr_ms_csfb_status_changed(const boolean, const gas_id_t);
extern void                    rr_ms_cs_establishment_confirmed(const gas_id_t);
extern rr_process_msg_action_e rr_ms_process_task_message(const rr_cmd_bdy_type *, gas_id_t *);
extern void                    rr_ms_task_start_init(void);
extern void                    rr_ms_process_heartbeat(const gas_id_t);

extern void                    rr_ms_store_paging_info(layer1_sys_info_T *, word, const gas_id_t);

extern gl1_block_quality_t rr_ms_adjust_block_quality(const l2_channel_type_T, const gas_id_t);
extern void                rr_ms_reset_block_quality(const gas_id_t);
extern void                rr_ms_l1_blocking_message_sent(const rr_l1_message_id_enum_T, const gas_id_t);
extern void                rr_ms_access_stratum_deactivated(const sys_modem_as_id_e_type, const boolean);
extern boolean             rr_ms_plmn_list_search_valid(const rr_state_T, gas_id_t);
extern void                rr_ms_cell_selection_deactivating(const gas_id_t);
extern void                rr_ms_power_scan_aborted(const gas_id_t);
extern boolean             rr_ms_other_sim_in_background_traffic(const gas_id_t);
extern void                rr_ms_notify_sys_info_ignored(const gas_id_t);
extern void                rr_ms_process_ps_abort_req(const gas_id_t);
extern void                rr_ms_process_l1_ds_abort_ind(mph_l1_ds_abort_ind_T *);
extern rr_ps_abort_block_t rr_ms_ps_abort_process_active(const gas_id_t);
extern void                rr_ms_x2g_acquisition_started(const gas_id_t);
extern void                rr_ms_ps_abort_timer_expiry(const rr_timer_id_e, const gas_id_t);
extern boolean             rr_ms_any_other_active_gas_id(const gas_id_t);
extern boolean             rr_ms_two_stacks_have_same_lai(const gas_id_t, const gas_id_t);
#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/* these functions are always defined, even if the feature is not enabled */
extern boolean             rr_multi_sim_mode_enabled(void);
extern boolean             rr_multi_sim_api_valid(const gas_id_t);
extern int                 rr_ms_get_active_sim_count(void);
extern void                rr_ms_x2g_procedure_failure(const rr_proc_failure_type_t, const gas_id_t);
extern boolean             rr_ms_all_stacks_in_state(const rr_state_T rr_state );
extern boolean             rr_ms_sys_info_refresh_blocked(const gas_id_t);
extern boolean             rr_ms_other_stack_has_same_lai(const gas_id_t);

/*!
 * \brief Returns the GS queue ID based on a given GS queue ID and a GAS ID.
 * 
 * \param queue_id (in)
 * \param gas_id (in) 
 *  
 * \return gs_queue_id_T - GS queue ID to use to send messages. 
 */
extern gs_queue_id_T rr_ms_get_gs_queue_id(const gs_queue_id_T, const gas_id_t);

#endif /* __RR_MULTI_SIM_H__ */

/* EOF */

