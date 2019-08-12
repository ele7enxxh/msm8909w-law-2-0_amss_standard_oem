#ifndef MCCQSH_I_H
#define MCCQSH_I_H
/*===========================================================================

         C D M A   Q S H    F R A M E W O R K    S U P P O R T

DESCRIPTION
  This contains all the 1X_CP internal data and API declarations required for 
  supporting QSH (Qualcomm Sherlock Holmes) framework.

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/onex_voice_adapt_if.h   1.8   10 Mar 2015 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccqsh_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
01/27/16   agh     Initial Revision

  ============================================================================*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/
#include "qsh.h"
#include "modem_1x_defs.h"
#include "onexl3_qsh_ext.h"
#include "mccdma_i.h"

#ifdef FEATURE_1XCP_QSH_SUPPORT    
/* Type to hold all the information for 1X_CP that is associated with QSH */
typedef struct {
  qsh_client_metric_cfg_s metric_cfg[ONEXL3_QSH_METRIC_MAX];
  boolean event_action[ONEXL3_QSH_EVENT_MAX];
}mc_qsh_data_type;

/* Holds all QSH configuration related data for 1X_CP */
mc_qsh_data_type mc_qsh_data;

/* Maximum number of Timer Expiry based Metrics reporting QSH commands 
 * that should be queued on mc_cmd_q at a time */
#define MC_QSH_TIMER_METRICS_CUTOFF_CNT 5

#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_1XCP_QSH_SUPPORT    
/*===========================================================================
  
  FUNCTION MC_QSH_INIT
  
  DESCRIPTION
    This function registers 1X_CP client with the QSH module
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None.
  
===========================================================================*/

void mc_qsh_init
( 
  void 
);

/*===========================================================================

FUNCTION MC_QSH_CB

DESCRIPTION
  Callback function invoked by QSH module for 1X to process QSH requests.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_qsh_cb
(
  qsh_client_cb_params_s *cb_params_ptr
);

/*===========================================================================
FUNCTION MC_CDMA_QSH_COMMAND

DESCRIPTION
  This function queues a command received from QSH module on to the MC task.
  This is done to ensure the cmd gets processed in MC task context only to avoid 
  any data integrity issues if the cmd was processed in QSH task context.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cdma_qsh_command
(
  qsh_client_cb_params_s *qsh_cb_cmd
  /* Cmd received from QSH */
);

/*===========================================================================

FUNCTION MC_PROCESS_QSH_CMD

DESCRIPTION
  This API processes the cmds received from QSH module. This could either be 
  invoked in QSH context or in MC task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_process_qsh_cmd
(
  qsh_client_cb_params_s* qsh_cmd_ptr
);

/*===========================================================================

FUNCTION MC_SEND_CELL_INFO_METRICS

DESCRIPTION
  This API would be used by 1X-CP to send cell info metrics to QSH module. 
  This metric is reported whenever there is a 1X cell change after SPM 
  is available on the new cell.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_cell_info_metrics
(
  void
);

/*===========================================================================

FUNCTION MCC_UPDATE_CELL_INFO_METRICS

DESCRIPTION
  This API updates the cell_info metrics structure with the current BS info 
  to be reported to QSH module on BS change.

DEPENDENCIES
  cur_bs_ptr should not be Null and SPM should have been received, otherwise
  garbage values may be populated in the structure.

RETURN VALUE
  True if metrics are available, False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_update_cell_info_metrics
(
  onexl3_qsh_metric_cell_info_s *metrics_ptr
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_CELL_INFO_METRICS

DESCRIPTION
  This API updates the cell_info metrics structure with the current SIB8/C2k
  params info to be reported to QSH module on BS change.

DEPENDENCIES
  None.

RETURN VALUE
  True if metrics are available, False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_csfb_update_cell_info_metrics
(
  onexl3_qsh_metric_cell_info_s *metrics_ptr
);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MC_SEND_QSH_EVENT

DESCRIPTION
  Common API Handler function for sending an event to QSH framework and this API is 
  called from CP, SRCH, MUX.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_qsh_event
(
  onexl3_qsh_event_e onex_event
);

/*===========================================================================

FUNCTION MC_SEND_RF_PARAMS_METRICS

DESCRIPTION
  This API would be used by 1X-CP to send RF params metrics to QSH module. 
  This metric is reported periodically and the required parameters are fetched
  by 1X-CP from 1X-L1 (Searcher) module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_rf_params_metrics
(
  qsh_client_metric_log_reason_e reason
);

/*===========================================================================

FUNCTION MCC_UPDATE_RF_PARAMS_METRICS

DESCRIPTION
  This API updates the RF params metrics structure with the RF params 
  of the current BS to be reported to QSH module periodically.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  True if parameters are available, False otherwise.

===========================================================================*/

boolean mcc_update_rf_params_metrics
(
  onexl3_qsh_metric_rf_params_s *metrics_ptr
);

/*===========================================================================

FUNCTION MC_QSH_COMMIT_DATA

DESCRIPTION
  This API is used by 1X-CP to send metrics info to QSH module. The metrics 
  could be sent due to metric timer expiry or due to 1X internal event such 
  as BS change.
  Metrics sent through this API will also ensure that write address for this
  metric would move to the next index in that metric's FIFO thus ensuring that
  next instance of this metric wouldn't overwrite the previous one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_qsh_commit_data
(
  uint8 metric_id,
  qsh_client_metric_log_reason_e reason,
  qsh_client_metric_cfg_s *metric_cfg
);

/*===========================================================================

FUNCTION MC_INIT_QSH_METRIC_CFG_ARRAY

DESCRIPTION
  To configure the 1X_CP array for metrics conllection before passing it to QSH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_init_qsh_metric_cfg_array
(
  void
);

/*===========================================================================
FUNCTION MC_QSH_IS_METRIC_ENABLED

DESCRIPTION
  Checks if a particular 1X_CP metric is enabled by QSH module or not.

DEPENDENCIES
  None.

RETURN VALUE
  True, if metric is enabled.
  False, otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/

boolean mc_qsh_is_metric_enabled
(
  onexl3_qsh_metric_e metric_id
);

/*===========================================================================

FUNCTION MC_QSH_IS_EVT_ENABLED

DESCRIPTION
  Checks if a particular 1X event is enabled by QSH module or not.

DEPENDENCIES
  None.

RETURN VALUE
  True, if event is enabled.
  False, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mc_qsh_is_evt_enabled
(
  onexl3_qsh_event_e evt
);

/*===========================================================================

FUNCTION MC_QSH_GET_ASID

DESCRIPTION

  This API returns the ASID value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the ASID value
  this API will default the ASID to SYS_MODEM_AS_ID_1

DEPENDENCIES
  None.

RETURN VALUE
  ASID value

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type mc_qsh_get_asid
(  
  void
);
#endif /* FEATURE_1XCP_QSH_SUPPORT */

#endif /* MCCQSH_I_H */

