#ifndef _DS_QMI_CFLOG_H
#define _DS_QMI_CFLOG_H
/*===========================================================================

                         D S _ Q M I _ C F L O G . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface Call Flow Logging service.
  This module logs a QXDM log packet 0x14CF (LOG_QMI_CALL_FLOW_C) to the 
  .isf file.  APEX can render a call flow diagram from the .isf file.

EXTERNALIZED FUNCTIONS


Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_cflog.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/28/10    dna    Created file.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds_qmi_defs.h"
#include "ds_qmi_svc_ext.h"
#include "ds_qmux_ext.h"
#include "dcc_task_defs.h"

#include "wms.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_REQUEST

  DESCRIPTION
    This function logs a QMI request received from a control point.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_request
( 
  qmux_svc_info_type *svc_s,
  qmi_cmd_buf_type *cmd_buf_p
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_RESP_RESULT

  DESCRIPTION
    This function stores the result code of the next QMI client response.
    There should always be a call to this function immediately before 
    ds_qmi_cflog_client_resp().  This relies on the code convention used
    in QMI.  Otherwise we would need to parse the result TLV back out of 
    the response or else put log messages all over the place...

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_resp_result
( 
  qmi_result_e_type  result,
  qmi_error_e_type   error
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CLIENT_RESP

  DESCRIPTION
    This function logs a QMI response or indication for a control point.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_client_resp
( 
  qmux_svc_info_type *svc_s,
  qmi_cmd_buf_type *cmd_buf_p,
  uint8 msg_type, 
  boolean filter_active
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_AMSS_CALL

  DESCRIPTION
    This function logs calls into AMSS that result in a response i.e. a
    command queued back to QMI that evantually completes a QMI request.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_amss_call
( 
  char *amss_func_name
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_AMSS_RSP

  DESCRIPTION
    This function logs an AMSS response or indication for QMI.

  PARAMETERS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_amss_rsp
( 
  dcc_cmd_data_buf_type  *cmd_ptr
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_META_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_meta_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_V4_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_v4_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_V6_SM_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment.

  PARAMETERS
    old_state - The current state
    new_state - The state being changed to
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_v6_sm_state_change
( 
  uint32 old_state,
  uint32 new_state
);

/*===========================================================================
  FUNCTION DS_QMI_CFLOG_CALL_STATE_CHANGE

  DESCRIPTION
    This function logs a state change banner comment for circuit switched
    (voice) calls.

  PARAMETERS
    label - QMI_VOICE already has the logic to build the label so no
            need to move it for now.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_cflog_call_state_change
( 
  char *label
);

/* Other prototypes for functions to log AMSS calls with detailed parameters
   follow to the end of the file.  The AMSS convention to put a banner header 
   on each function prototype is not followed in order to keep this header 
   file more readable */
void ds_qmi_cflog_cm_sys_sel_pref
(
  cm_mode_pref_e_type              mode_pref_cur,
  cm_mode_pref_e_type              mode_pref_new,
  cm_band_pref_e_type              band_pref_new,
  cm_prl_pref_e_type               prl_pref_new,
  cm_roam_pref_e_type              roam_pref_new
);

void ds_qmi_cflog_cm_sys_sel_pref_net_reg
(
  uint8 register_action,
  cm_network_sel_mode_pref_e_type  network_sel_mode_pref_cur,
  cm_network_sel_mode_pref_e_type  network_sel_mode_pref_new,
  cm_mode_pref_e_type              mode_pref_new,
  sys_plmn_id_s_type *             plmn_ptr
);

void ds_qmi_cflog_cm_sys_sel_pref_net_attach
(
  cm_srv_domain_pref_e_type srv_domain_pref_new
);

void ds_qmi_cflog_cm_sys_sel_pref_tech_pref
(
  cm_mode_pref_e_type              mode_pref_cur,
  cm_mode_pref_e_type              mode_pref_new,
  cm_pref_term_e_type              pref_term_new,
  cm_hybr_pref_e_type              hybr_pref_new
);

void ds_qmi_cflog_cm_call_cmd_orig_otasp
(
  cm_num_s_type            *num_ptr
);

void ds_qmi_cflog_cm_call_cmd_orig
(
  cm_srv_type_e_type       srv_type,
  cm_call_type_e_type      call_type,
  cm_num_s_type            *num_ptr
);

void ds_qmi_cflog_cm_call_cmd_answer
(
  uint8                           call_id,
  cm_call_mode_info_e_type        info_type,
  cm_call_type_e_type             call_type
);

void ds_qmi_cflog_cm_call_cmd_end
(
  uint8                           call_id,
  cm_call_mode_info_e_type        info_type,
  char                            *end_type
);

void ds_qmi_cflog_cm_call_cmd_privacy_pref
(
  uint8                           call_id,
  char                            *privacy_mode_name
);

void ds_qmi_cflog_cm_call_cmd_sups
(
  uint8                           call_id,
  char                            *sups_type
);

void ds_qmi_cflog_cm_call_cmd_sups_flash
(
  uint8                           call_id,
  cm_num_s_type                   *num_ptr
);

void ds_qmi_cflog_cm_call_cmd_setup_res
(
  uint8                           call_id
);

void ds_qmi_cflog_cm_inband_cmd_start_cont_dtmf
(
  uint8               call_id,
  uint8               digit
);

void ds_qmi_cflog_cm_inband_cmd_stop_cont_dtmf
(
  uint8               call_id
);

void ds_qmi_cflog_cm_inband_cmd_burst_dtmf
(
  uint8               call_id,
  char                *digit_buffer,
  uint8               on_length,
  uint8               off_length

);

void ds_qmi_cflog_cm_ph_cmd_oprt_mode
(
  sys_oprt_mode_e_type   new_oprt_mode
);

void ds_qmi_cflog_cm_sups_cmd
(
  char *name,
  int  ss_code,
  int  ss_ref,
  char *ss_buf
);

void ds_qmi_cflog_cm_sups_cmd_cf
(
  char *name,
  int  ss_code,
  int  ss_ref,
  char *ss_buf,
  char *cf_reason
);

void ds_qmi_cflog_wms_bc_mm_set_pref
(
  uint8  mode,
  uint8  bc_pref
);

void ds_qmi_cflog_wms_cfg_set_memory_full
( 
  uint8 memory_available 
);

void ds_qmi_cflog_wms_msg_delete
(
  wms_memory_store_e_type storage_type, 
  uint32 storage_index
);

void ds_qmi_cflog_wms_msg_read
(
  wms_memory_store_e_type storage_type, 
  uint32 storage_index
);

#endif /* _DS_QMI_CFLOG_H */

