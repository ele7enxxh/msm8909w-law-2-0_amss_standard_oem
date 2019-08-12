#ifndef _DS_QMI_DMS_H
#define _DS_QMI_DMS_H
/*===========================================================================

                         D S _ Q M I _ D M S . H

DESCRIPTION

 The Data Services QMI Device Management Service header file.

EXTERNALIZED FUNCTIONS

   qmi_dms_init()
     Register the DMS service with QMUX for all applicable QMI links

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_dms.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/21/14    sk     QMI-DMS Service QCSI migration.
02/13/14    vrk    Added cmd_type to qmi_dms_process_mmgsdi_operation_complete
06/11/12    sb     Fix to move to correct UIM state in case of CARD ERROR
                   event.
08/18/11    sa     GSDI cleanup changes.
07/14/11    sj     Expose UIM status and PIN1 status to other modules 
06/22/10    wc     Send PRL init indication when PRL is loaded 
06/21/10    kk     Fix to check UIM status before posting commands to MMGSDI.
06/07/10    kk     Fix to update app_type and uim_state from all possible
                   events.
10/16/09    kk     Windows7 compliance - new messages and ATCoP dependencies.
10/08/09    kk     Windows7 compliance - DMS changes.
09/06/06    ks     Featurized pin support code, under FEATURE_MMGSDI.
09/05/06    ks     Added functions to process mmgsdi events and responses.
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
05/11/05    ks     Moved qmi_dms_charger_event_occured to ds_qmi_dms_ext.h
05/31/05   jd/ks   Code review updates
05/11/05    ks     Code Review comments and clean up.
03/14/05    ks     Removed featurization
03/08/05    jd     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"
#include "qmi_csi.h"

#include "msgr.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
  #include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*Added for message POLICYMAN_CFG_UPDATE_MSIM_IND max size*/
typedef struct {
   msgr_hdr_s header;
   msgr_attach_s payload;
}qmi_dms_policyman_cfg_update_ind_type_msg_s;

/*===========================================================================
  FUNCTION QMI_DMS_INIT()

  DESCRIPTION
    Register the DMS service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_dms_init
(
  void
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_MMGSDI_OPERATION_COMPLETE()

  DESCRIPTION
    To send a response to the QMI pin operation requested by the client.
    
  PARAMETERS
    cnf_type : Type of MMGSDI PIN confirmation
    cnf_data : Response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_process_mmgsdi_operation_complete
(
  mmgsdi_cnf_enum_type    cnf_type,
  mmgsdi_cnf_type        *cnf_data
);

/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_MMGSDI_EVENT()

  DESCRIPTION
    To process event received from mmgsdi in DS task context.
    
  PARAMETERS
    event_info : Event info
    qmi_cmd    : Type of qmi cmd

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_process_mmgsdi_event
(
  qmi_if_mmgsdi_evt_info_type *  event_info,
  qmi_cmd_id_e_type              qmi_cmd
);

/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_UIM_GET_STATE()

  DESCRIPTION
    To process event received for SIM status.
    
  PARAMETERS
    event    : MMGSDI event type
    card_err_info: Contains card error code
    session_activated : Contains if session has been activated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_process_uim_get_state
(
  mmgsdi_events_enum_type event,
  mmgsdi_card_err_info_enum_type card_err_info,
  boolean session_activated
);

/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_MMGSDI_PERSO_EVENT()

  DESCRIPTION
    Handler for MMGSDI_PERSO feature events

  PARAMETERS
    feature : Perso feature type
    status  : Perso status type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_process_mmgsdi_perso_event
(
  mmgsdi_perso_feature_enum_type  feature,
  mmgsdi_perso_status_enum_type   status
);

/*===========================================================================
  FUNCTION QMI_DMS_MMGSDI_EVT_CBACK()

  DESCRIPTION
    Event handler callback for QMI DMS subsystem

  PARAMETERS
    event   : MMGSDI event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_mmgsdi_evt_cback
(
  const mmgsdi_event_data_type *event
);

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_SET_OPRT_MODE_RESP()

  DESCRIPTION
    Called when cmd_cb() supplied in cm_ph_cmd_set_oprt_mode() is called 
    by CM. Sends response to control point for the corresponding
	Set Operating Mode Reqs
        
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_generate_set_oprt_mode_resp
(
  void *               user_data,
  cm_ph_cmd_err_e_type ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_OPRT_MODE_IND()

  DESCRIPTION
    Called when CM notifies us that the operating mode has changed.  Sends
    an indication with the new operating mode to clients registered for
    the event.
        
  PARAMETERS
    new_oprt_mode : new operating mode (if this is LPM, QMI_DMS will
                    determine if it is persistent LPM on its own)
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_generate_oprt_mode_ind
(
  sys_oprt_mode_e_type  new_oprt_mode
);

/*===========================================================================
FUNCTION  QMI_DMS_CM_CALL_CMD_CB

DESCRIPTION
  CM call command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
extern void qmi_dms_cm_call_cmd_cb 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type           call_cmd,          /* Command ID            */
  cm_call_cmd_err_e_type       call_cmd_err       /* Command error code    */
);


/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_CM_CALL_ERR()

  DESCRIPTION
   Process the CM call event

  PARAMETERS
    user_data :  user data ptr passed to CM
    call_cmd  :  requested CM call command
    err_type  :  result of request
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_process_cm_call_err
(
  void *                 user_data,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type err_type
);


/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_CALL_EVENT()

  DESCRIPTION
    Called when a QMI_IF call event happens.
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_process_call_event
(
  void
);

/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_CDMA_LOCK_MODE_IND()

  DESCRIPTION
    Called when the cdma lock mode status changes.  Sends
    an indication with the new state to clients registered for
    the event.
        
  PARAMETERS
    new_cdma_lock_mode : new state of the cdma lock mode
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_generate_cdma_lock_mode_ind
(
  boolean  new_cdma_lock_mode
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_DMSI_GET_UIM_STATUS()

  DESCRIPTION
    Returns the status of the UIM based on the values cached from earlier
    MMGSDI events.

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the SIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dmsi_get_uim_status
(
  void
);

/*===========================================================================
  FUNCTION QMI_DMS_GET_UIM_ACCESS_STATUS()

  DESCRIPTION
    Returns the status of the UIM for read/write of PIN1 protected values based
    on the values cached from earlier MMGSDI events.  

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the UIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dms_get_uim_access_status
(
  void
);

/*===========================================================================
  FUNCTION QMI_DMSI_GET_UIM_STATUS_EXT()

  DESCRIPTION
    Returns the status of the UIM based on the values fetched from mmgsdi module.

  PARAMETERSs
    None

  RETURN VALUE
    QMI_ERR_NONE: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the SIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dms_get_uim_access_status_ext
(
   uint8 session_type
);


typedef enum
{
  QMI_PIN_STATUS_MIN           = -1,
  QMI_PIN_NOT_INITIALIZED      = MMGSDI_PIN_STATUS_NOT_INITIALIZED,
  QMI_PIN_ENABLED_NOT_VERIFIED = MMGSDI_PIN_ENABLED_NOT_VERIFIED,
  QMI_PIN_ENABLED_VERIFIED     = MMGSDI_PIN_ENABLED_VERIFIED,
  QMI_PIN_DISABLED             = MMGSDI_PIN_DISABLED,
  QMI_PIN_BLOCKED              = MMGSDI_PIN_BLOCKED,
  QMI_PIN_PERM_BLOCKED         = MMGSDI_PIN_PERM_BLOCKED,
  QMI_PIN_UNBLOCKED            = MMGSDI_PIN_UNBLOCKED,
  QMI_PIN_CHANGED              = MMGSDI_PIN_CHANGED,
  QMI_PIN_STATUS_MAX
} qmi_pin_status_enum_type;

#endif /* FEATURE_MMGSDI_SESSION_LIB */
/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_PRL_INIT_IND()

  DESCRIPTION
    Called when CM notifies us that the PRL is loaded onto device. Sends
    an indication with the PRL init TLV.
        
  PARAMETERS
    None
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_generate_prl_init_ind
(
  void
);

/*===========================================================================
FUNCTION QMI_DMS_TDSCDMA_SET_CONFIG_RES_CB()

  DESCRIPTION
  Function gets called from Message Router (MSGRT) to post the command with
  the required value.
  
  PARAMETERS
  set_resp : message posted by MSGRT to the QMI

  RETURN VALUE
  None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_tdscdma_set_config_res_cb
(
  void *set_resp
);

/*===========================================================================
FUNCTION QMI_DMS_TDSCDMA_GET_CONFIG_RES_CB()

  DESCRIPTION
  Function gets called from Message Router (MSGRT) to post the command with
  the required value.
  
  PARAMETERS
  get_resp : message posted by MSGRT

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_tdscdma_get_config_res_cb
(
  void *get_resp
);

/*===========================================================================
  FUNCTION qmi_dms_device_mode_changed()

  DESCRIPTION  Device mode is changed and checks if an indication needs to be
               sent to clients. This is to handle single sim svlte case.
    Note: Indication will be sent only in case of single sim SVLTE config
          Rest of the things will be taken care of Policyman indications
  PARAMETERS
    none
  RETURN VALUE
    none
  DEPENDENCIES
    none
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_device_mode_changed( void );

#endif /* _DS_QMI_DMS_H */
