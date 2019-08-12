#ifndef _QM_MEAS_H
#define _QM_MEAS_H

/*===========================================================================

                         QM_MEAS.H

DESCRIPTION

  QMI_MMODE header file for Measurement module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/23/13    aa     Created file
===========================================================================*/

#include "cm.h"
#ifdef FEATURE_TDSCDMA
#include "tds_rrc_msg.h"
#endif
#include "qm_comdef.h"
#include "network_access_service_v01.h"

/*===========================================================================
  FUNCTION QM_MEAS_INIT()

  DESCRIPTION
    This function initializes the measurement module global data

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function should be called before qmi_nas_init() during initialization process
===========================================================================*/
void qm_meas_init ( void );

/*===========================================================================
  FUNCTION QM_MEAS_PROCESS_CM_MEASUREMENT_EVENT()

  DESCRIPTION
    This function processes CM SS EVENT. If Event type is handled, this function checks and 
    updates the global measurement data and calls routines to send QMI_NAS_SIG_INFO_IND 
    and QMI_NAS_ERR_RATE_IND messages to registered clients.

  PARAMETERS
    p_ss_info : Pointer to received CM SS EVENT
    evt         : CM SS EVENT type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_meas_process_cm_measurement_event
( 
  cm_mm_msim_ss_info_s_type *p_ss_info, 
  cm_ss_event_e_type evt
);

#ifdef FEATURE_TDSCDMA
/*===========================================================================
  FUNCTION QM_MEAS_PROCESS_TDS_MEASUREMENT_INFO()

  DESCRIPTION
    This function processes MSG Router TDSCDMA Events. If TDSCDMA is in service, this function checks 
    and updates the global measurement data and calls routines to send QMI_NAS_SIG_INFO_IND and 
    QMI_NAS_ERR_RATE_IND messages to registered clients.

  PARAMETERS
    tds_meas : Pointer to TDS measurement data from MSGR

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_meas_process_tds_measurement_info
( 
  tds_rrc_scell_signal_status_ind_type *tds_meas
);
#endif

/*===========================================================================
  FUNCTION QM_MEAS_POPULATE_GET_SIG_INFO_RESP()

  DESCRIPTION
    This function populates SIG INFO response message based on Measurement module private data.

  PARAMETERS
    rsp_msg : Pointer to SIG INFO response IDL Structure
    sub_id   : Subscription id

  RETURN VALUE
    Relevant error is returned if population routine fails, QMI_ERR_NONE_V01 is returned otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qm_meas_populate_get_sig_info_resp
( 
  nas_get_sig_info_resp_msg_v01 *rsp_msg,
  enum qm_subs_e sub_id
);

/*===========================================================================
  FUNCTION QM_MEAS_POPULATE_GET_ERR_RATE_RESP()

  DESCRIPTION
    This function populates ERR RATE response message based on Measurement module private data.

  PARAMETERS
    rsp_msg : Pointer to ERR RATE response IDL Structure
    sub_id   : Subscription id

  RETURN VALUE
    Relevant error is returned if population routine fails, QMI_ERR_NONE_V01 is returned otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qm_meas_populate_get_err_rate_resp
( 
  nas_get_err_rate_resp_msg_v01 *rsp_msg, 
  enum qm_subs_e sub_id 
);

/*===========================================================================
  FUNCTION QM_MEAS_REPORT_NEW_ACTIVE_RATS()

  DESCRIPTION
    This function reports to Measurement module which new RATs came in service for a subscription id.
    SIG INFO IND and ERR RATE IND are triggered to clients if measurement data is available for active RAT.

  PARAMETERS
    sub_id : subscription id
    active_rat_mask : bitmask indicating which new RATs came in service

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function is called from SYS_INFO module when CM SS EVENT (SRV_CHANGED) is received.
===========================================================================*/
void qm_meas_report_new_active_rats
( 
  enum qm_subs_e sub_id, 
  rat_bitmask_type active_rat_mask
);

/*===========================================================================
  FUNCTION QM_MEAS_REPORT_NEW_INACTIVE_RATS()

  DESCRIPTION
    This function reports to Measurement module which new RATs went out of service for a subscription id.
    Measurement data is invalidated for inactive RATs

  PARAMETERS
    sub_id : subscription id
    inactive_rat_mask : bitmask indicating which new RATs came in service

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function is called from SYS_INFO module when CM SS EVENT (SRV_CHANGED) is received.
===========================================================================*/
void qm_meas_report_new_inactive_rats
( enum qm_subs_e sub_id, 
  rat_bitmask_type inactive_rat_mask
);

#endif // !_QM_MEAS_H


