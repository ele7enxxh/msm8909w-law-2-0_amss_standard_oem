#ifndef _DS_QMI_CTL_H
#define _DS_QMI_CTL_H
/*===========================================================================

                         D S _ Q M I _ C T L . H

DESCRIPTION

 The Data Services QMI control service header file.

EXTERNALIZED FUNCTIONS

   qmi_ctl_init()
     Initialize the QMI control service

   qmi_ctl_get_instance_id()
     Retrive the host+msm instance ID of the specified QMI instance

Copyright (c) 2005 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_ctl.h#1 $ $DateTime: 2004/09/20

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/15/13    ab     Added QMI SVC available notification feature
11/10/09    ar     Added qmi_ctl_send_sync prototype
10/23/09    ar     Add supprot for power save indication filtering
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
05/31/05   jd/ks   Code review updates
05/11/05    ks     Code review comments and clean up.
03/14/05    ks     Removed featurization
01/25/05    jd     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds_qmi_defs.h"

/*===========================================================================
  FUNCTION QMI_CTL_INIT()

  DESCRIPTION
    Initialize all instances (one for each physical link supporting QMUX)
    of the QMI_CTL service

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Must be called at startup

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_ctl_init
(
  qmi_instance_e_type  qmi_instance
);


/*===========================================================================
  FUNCTION QMI_CTL_GET_INSTANCE_ID()

  DESCRIPTION
    Retrive the 16-bit instance ID of the specified QMI link based on the
    8-bit instance provided by the host driver and local 8-bit QMI link 
    instance.

  PARAMETERS
    qmi_instance - the QMI instance whose QMI instance ID is being queried
    out_instance_id - set to instance ID if successful, unchanged otherwise

  RETURN VALUE
    TRUE  -  instance ID returned in out_instance_id
    FALSE -  invalid QMI instance specified or
             QMI instance ID has not been set

  DEPENDENCIES
    qmi_init must have been called at startup

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmi_ctl_get_instance_id
(
  qmi_instance_e_type  qmi_instance,
  uint16 *             out_instance_id
);


/*===========================================================================
  FUNCTION QMI_CTL_SEND_SYNC()

  DESCRIPTION
    Called during QMI device port open processing.
    
  PARAMETERS
    qmi_inst          - the QMI instance to operate upon

  RETURN VALUE
    None
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_ctl_send_sync
(
  qmi_instance_e_type  qmi_inst
);

/*===========================================================================
  FUNCTION QMI_CTL_SYNC_IND_CMD_HDLR()

  DESCRIPTION
    Called to process QMI_CMD_CTL_SEND_SYNC_IND message.
    
  PARAMETERS
    sp          - QMI_CTL's service instance state pointer for this qmi link
                  returned by qmux_reg_service()

  RETURN VALUE
    None
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_ctl_sync_ind_cmd_hdlr
(
  void *sp
);

/*===========================================================================
  FUNCTION QMI_CTL_PWRSAVE_MODE_CHANGE_NOTIFY

  DESCRIPTION
    Function to apply the new power save state to a given instance.
    If the state changes, an indication may be generated towards the
    client to notify the state transition.
  
  PARAMETERS
    qmi_inst          - the QMI instance to operate upon
    new_state_handle  - the new power save state handle
    old_state_handle  - the previous power save state handle

  RETURN VALUE
    TRUE if power save state changed
    FALSE otherwise
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    May enqueue indication to associated client.
===========================================================================*/
extern boolean qmi_ctl_pwrsave_mode_change_notify
(
  qmi_instance_e_type    qmi_inst,
  void*                  new_state_handle,
  void*                  old_state_handle
);
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
  FUNCTION QMI_CTL_PROCESS_SERVICE_AVAILABILITY()

  DESCRIPTION
    To report if a service needs to be notified to control point
    for its availability

  PARAMETERS
    svc_id            : QMI Service ID
    qmi_inst          : QMI Instance
    is_legacy_svc     : TRUE if QMI Service is based on LEGACY QMUX, 
                        FALSE Otherwise

  RETURN VALUE
    None
 
  DEPENDENCIES
    qmi_ctl_init() must have been called
 
  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_ctl_process_service_availability
(
  qmux_service_e_type               svc_id,
  qmi_instance_e_type               qmi_inst,
  boolean                           is_legacy_svc
);
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
#endif /* _DS_QMI_CTL_H */
