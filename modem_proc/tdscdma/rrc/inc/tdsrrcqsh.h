/*!
  @file
  tdsrrcqsh.h

  @brief
  TDSRRC QSH (Qualcomm Sherlock Holmes) debug support header

  @detail
*/

/*===========================================================================

  Copyright (c) 2008 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrcqsh.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/15/14   cdf     Initial version

===========================================================================*/

#ifndef TDS_RRC_QSH_H 
#define TDS_RRC_QSH_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "tdsrrc_qsh_ext.h"
#include "tdsrrcdata.h"
#include "tdsuecomdef.h"

#ifdef FEATURE_QSH_EVENT_METRIC

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define TDSRRC_QSH_METRIC_UPDATE(metric_id) tdsrrcqsh_metric_stats_update(metric_id)
#define TDSRRC_QSH_SERVING_CELL_UPDATE()    tdsrrcqsh_serving_cell_update()
#define TDSRRC_QSH_RRC_STATE_UPDATE(new_state)       tdsrrcqsh_rrc_state_update(new_state)

typedef enum {
    TDSRRC_QSH_T2L_RESEL_START,
    TDSRRC_QSH_T2T_RESEL_START,
    TDSRRC_QSH_T2G_RESEL_START,
    TDSRRC_QSH_RESEL_FAILURE,
    TDSRRC_QSH_CEL_SEL_FAILURE,
    TDSRRC_QSH_T2L_HO_START,
    TDSRRC_QSH_T2G_HO_START,
    TDSRRC_QSH_T2T_HO_START,
    TDSRRC_QSH_IRAT_HO_FAILURE,
    TDSRRC_QSH_RLF,
    TDSRRC_QSH_OOS,
    TDSRRC_QSH_CONN_REJ,
    TDSRRC_QSH_SCELL_UPDATE,
    TDSRRC_QSH_STATE_UPDATE,
    TDSRRC_QSH_OTA
} tdsrrcqsh_metric_enum_type;


typedef struct
{
  /* start address for metric */
  void                            *data_ptr;

  /* metric size in bytes */
  size_t                          size_bytes;

  /* metric context id that the client needs to pass into 
     qsh_client_metric_log(...) after writing a metric */
  qsh_client_metric_context_id_t  metric_context_id;

  /* if the metric is started (TRUE) or stopped (FALSE) */
  boolean                         started;
}tdsrrc_qsh_metric_cfg_struct_type;


typedef struct {

    /* metric cfg that are from QSH */
    tdsrrc_qsh_metric_cfg_struct_type   metric_cfg[TDSCDMA_NUM_SUBS][TDSRRC_QSH_METRIC_MAX];

    tdsrrc_qsh_metric_resel_cel_sel_s   resel_cel_sel_metric;

    tdsrrc_qsh_metric_rlf_oos_s         rlf_oos_metric;

    tdsrrc_qsh_metric_ho_s              ho_metric;

    tdsrrc_qsh_metric_serving_cell_s    previous_serving_cell_metric;

    tdsrrc_qsh_metric_serving_cell_s    serving_cell_metric;

    tdsrrc_qsh_metric_rrc_state_s       previous_rrc_state;

    tdsrrc_qsh_metric_rrc_state_s       rrc_state;

    tdsrrc_qsh_metric_ota_s             ota;

    tdsrrc_qsh_metric_tmr_and_const_s   tmr_and_const;

    tdsrrc_qsh_metric_conn_end_info_s_type conn_end_info;

    tdsrrc_qsh_metric_multi_rab_status_s_type rab_status;
    
} tdsrrcqsh_global_struct_type;

typedef struct
{
  qsh_client_cb_params_s  cb_params;
} tdsrrc_qsh_cb_ind_type;

typedef enum
{
  TDSRRC_QSH_SDU_TYPE_NONE,
  TDSRRC_QSH_CCCH_DL_SDU,
  TDSRRC_QSH_CCCH_UL_SDU,
  TDSRRC_QSH_DCCH_DL_SDU,
  TDSRRC_QSH_DCCH_UL_SDU,
  TDSRRC_QSH_HO_TO_UTRAN
} tdsrrcqsh_ota_pdu_type_e;

typedef enum
{
  TDSRRC_NORMAL_EVENT, 
  TDSRRC_UNSPECIFIED,
  TDSRRC_PRE_EMPTIVE_RELEASE,
  TDSRRC_CONGESTION,
  TDSRRC_REESTABLISHMENT_REJECT,
  TDSRRC_DIRECTED_SIGNALING_CONNECTION_REESTABLISHMENT,
  TDSRRC_USER_INACTIVITY,
  TDSRRC_OTHER_CAUSE
}tdsrrc_conn_rel_cause_e_type;

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern tdsrrcqsh_global_struct_type tdsrrcqsh_global;

/*===========================================================================

FUNCTION         tdsrrc_qsh_tc_metrics_init

DESCRIPTION
                 Initializing the metrics for test support
  
DEPENDENCIES
                 None.

RETURN VALUE
                 None.

SIDE EFFECTS
                 None.

===========================================================================*/
void tdsrrc_qsh_tc_metrics_init();


/*===========================================================================


/*===========================================================================

FUNCTION        tdsrrcqsh_init

DESCRIPTION     TDSRRC QSH initialization. It will register a callback function
                to QSH module

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_init();

/*===========================================================================

FUNCTION        tdsrrcqsh_client_cb

DESCRIPTION     Client cb from QSH.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_client_cb(qsh_client_cb_params_s *cb_params_p);


/*===========================================================================

FUNCTION        tdsrrcqsh_commit_data

DESCRIPTION     This function commits data to QSH and update the next address.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_commit_data(qsh_metric_id_t metric_id, qsh_client_metric_log_reason_e log_reason);


/*===========================================================================

FUNCTION        tdsrrcqsh_update_metric_cfg

DESCRIPTION     Update address to the buffer where metrics should be written to.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_update_metric_cfg
(
  qsh_client_metric_cfg_s           *qsh_metric_cfg_p
);

/*===========================================================================

FUNCTION        tdsrrcqsh_flush_periodic_metric

DESCRIPTION     Flush periodic QSH metric and update new address.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_flush_periodic_metric
(
  qsh_client_metric_timer_expiry_s  *expiry_data_p,
  qsh_context_id_t                  ctx_id
);

/*===========================================================================

FUNCTION        tdsrrcqsh_send_metric_cb_ind

DESCRIPTION     Post a message back to TDSRRC task from QSH callback context.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_send_metric_cb_ind
(
  qsh_client_cb_params_s *cb_params_p
);


/*===========================================================================

FUNCTION        tdsrrcqsh_handle_metric_cb_ind

DESCRIPTION     Handle QSH callback indication.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_handle_metric_cb_ind
(
  tdsrrc_qsh_cb_ind_type *qsh_cb_ind_p
);

/*===========================================================================

FUNCTION        tdsrrcqsh_initialize_metric_data_by_id

DESCRIPTION     Reset metric data to initial state.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_initialize_metric_data_by_id
(
  qsh_metric_id_t  metric_id
);

/*===========================================================================

FUNCTION        tdsrrcqsh_metric_stats_update

DESCRIPTION     Update metric of given metric id.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_metric_stats_update(tdsrrcqsh_metric_enum_type metric_id);

/*===========================================================================

FUNCTION        tdsrrcqsh_serving_cell_update

DESCRIPTION     Update serving cell information and send event to QSH for the
                update.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_serving_cell_update();

/*===========================================================================

FUNCTION        tdsrrcqsh_rrc_state_update

DESCRIPTION     Send event to QSH if RRC state is updated.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_rrc_state_update(tdsrrc_state_e_type new_state);

/*===========================================================================

FUNCTION        tdsrrcqsh_save_ota

DESCRIPTION     This function saves the passed OTA and send to QSH.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_save_ota
(
  uint8                     *sdu,
  uint16                    sdu_length,
  tdsrrcqsh_ota_pdu_type_e  pdu_type,
  uint8                     msg_type
);

/*===========================================================================

FUNCTION        tdsrrcqsh_conn_end_info_update

DESCRIPTION       
  This function updates RLF/RA failure/L2 failure/RRC release/HO failure to qsh

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_conn_end_info_update(tdsrrc_qsh_metric_conn_end_info_s_type * conn_end_info_ptr);

/*===========================================================================

FUNCTION tdsrrcrce_qsh_update_rlf_or_weak_signal_failure

DESCRIPTION
  This function updates RLF/RA failure/L2 failure to qsh
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdsrrcqsh_update_rlf_or_weak_signal_failure(sys_rlf_weak_signal_e_type type);

/*===========================================================================
FUNCTION tdsrrcrce_qsh_update_connection_release

DESCRIPTION
  This function updates rrc connection release cause to qsh
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrcqsh_update_connection_release(tdsrrc_conn_rel_cause_e_type rel_cause);


/*===========================================================================
FUNCTION tdsrrcrce_qsh_update_connection_release

DESCRIPTION
  This function updates ho failure type to qsh
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsrrcqsh_update_ho_failure(sys_handover_failure_e_type ho_fail_type);

/*===========================================================================

FUNCTION        tdsrrcqsh_rab_status_update

DESCRIPTION     Send event to QSH if rab_status changes.

DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrcqsh_rab_status_update();

#endif /* FEATURE_QSH_EVENT_METRIC */


#endif /* TDS_RRC_QSH_H */
