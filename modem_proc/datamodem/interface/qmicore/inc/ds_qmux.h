#ifndef _DS_QMUX_H
#define _DS_QMUX_H
/*===========================================================================

                         D S _ Q M U X . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface control channel message
  multiplexing protocol header file.

  Externalizes QMUX functions to other components of the QMI library.

EXTERNALIZED FUNCTIONS

   Upper interface APIs:

   qmux_init()
     Initialize a particular QMUX instance
   qmux_reg_service()
     Register service with a QMUX instance
   qmux_dereg_service()
     Deregister serivce with a QMUX instance
   qmux_send()
     Send a service message (PDU) over a specified QMI link
   qmux_link_down()
     Indicate that the specified QMUX link is down, results in cleanup of
     any outstanding QMUX and registered service resources.

   +----------------------------+
   |  Service(s)                |
   +----------------------------+
     |                        ^
     | qmux_send()            | service.rx_cb
     v                        |
   +----------------------------+
   | QMUX dispatch layer        |
   +----------------------------+

Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/ds_qmux.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
5/15/13     ab     Add QMI SVC available notification feature
11/02/11    wc     Add DTR on SIO control channel feature
10/23/09    ar     Add supprot for power save indication filtering and 
                   port configuration from NVRAM.
12/12/06    ks     Added QOS_SERVICE to qmi_service enum.
05/31/05   jd/ks   Code review updates
05/11/05    ks     Changed name from qmux_svc.h to qmux.h and clean up.
03/14/05    ks     Removed featurization
11/21/04    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "rex.h"
#include "dsm.h"
#include "rdevmap.h"

#include "ds_qmi_defs.h"
#include "ds_qmux_ext.h"
#include "ds_rmnet_defs.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/


/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMUX_REG_SERVICE()

  DESCRIPTION
    Register the service instance identified by 'service' with the qmux
    instance identified by 'qmi_instance' with the configuration provided
    in 'cfg'.

  PARAMETERS
    qmi_instance : instance of QMI
    service      : service type
    cfg          : configuration parameters for the service instance

  RETURN VALUE
    NULL           -  service registration failed
    service handle -  qmux service handle

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void * qmux_reg_service
(
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  qmux_svc_config_type *  cfg
);

/*===========================================================================
  FUNCTION QMUX_DEREG_SERVICE()

  DESCRIPTION
    Deregister the previously registered service instance identified by
    the passed service handle.

  PARAMETERS
    service handle -  qmux service handle returned by qmux_reg_service()

  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmux_dereg_service
(
  void *  service_handle
);

/*===========================================================================
  FUNCTION QMUX_SEND()

  DESCRIPTION
    Package and send the provided service layer PDU as a QMUX message on the
    link inferred by service_handle

  PARAMETERS
    service_handle : handle to the service's qmux link
    qmux_sdu       : service PDU to be sent
    clid           : Client ID for which the message is destined

  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmux_send
(
  void *           service_handle,
  dsm_item_type *  qmux_sdu,
  byte             clid
);

/*===========================================================================
  FUNCTION QMUX_SIO_SEND()

  DESCRIPTION
    Package and send the provided service layer PDU as a QMUX message on the
    link inferred by service_handle

  PARAMETERS
    qmi_instance : Instance of the qmi
    qmux_sdu       : service PDU to be sent
    clid           : Client ID for which the message is destined
    service        : service id

  RETURN VALUE
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmux_sio_send
(
  qmi_instance_e_type   qmi_instance,
  qmux_service_e_type   service, 
  uint32                clid,
  dsm_item_type *       qmux_sdu 
);
/*===========================================================================
  FUNCTION QMUX_LINK_DOWN

  DESCRIPTION
    Callback to cleanup QMUX and associated service resources when their
    serving QMI link is disconnected

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    QMI link disconnection closes all QMI application state
===========================================================================*/
extern void qmux_link_down
(
  qmi_instance_e_type  instance
);

/*===========================================================================
   FUNCTION QMUX_CLEAN_LEGACY_SVC_RESOURCES

   DESCRIPTION
     Cleanup QMUX and associated service resources related to Legacy Services

   PARAMETERS
     instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    Closes all QMI Legacy Service application state
===========================================================================*/
void  qmux_clean_legacy_svc_resources
(
  qmi_instance_e_type  instance
);

/*===========================================================================
  FUNCTION QMUX_CLEAN_INSTANCE_RESOURCES

  DESCRIPTION
    Cleanup QMUX and associated service resources

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    Closes all QMI application state
===========================================================================*/
extern void qmux_clean_instance_resources
(
  qmi_instance_e_type  instance
);

/*===========================================================================
  FUNCTION QMUX_OPEN

  DESCRIPTION
    Opens all the QMUX instances that need to work.
    Called on the RDM Open Callback

  PARAMETERS  
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmux_open
(
  void
);

/*===========================================================================
  FUNCTION QMUX_CLOSE

  DESCRIPTION
    Closes all the QMUX instances that are open.
    Called on the RDM Close Callback

  PARAMETERS  
    None

  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmux_close
(
  void
);

/*===========================================================================
  FUNCTION QMUX_RMNET_INST_OPEN_CLOSE_RESULT()

  DESCRIPTION
    Notify QMUX about the specified RmNet instance open/close result. The
    QMUX post a cmd to qmi_modem task to process it.
 
  PARAMETERS
    result      : Open/Close result
    cmd         : cmd(open or close)
    instance    : QMUX instance
    port_id     : sio_port 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmux_rmnet_inst_open_close_result
(  
  boolean                                  result,
  qmi_cmd_id_e_type                        cmd,
  uint32                                   ep_id
);

/*===========================================================================
  FUNCTION QMUX_PROCESS_RMNET_INST_OPEN_CLOSE_RESULT()

  DESCRIPTION
    This function processes RmNet (data) port open/close result in qmi modem
    task and then proceeds with the control port operation.
    
  PARAMETERS
    result      : Open/Close result
    cmd         : cmd(open or close)
    instance    : QMUX instance
    port_id     : sio_port 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmux_process_rmnet_inst_open_close_result
(  
  boolean                                  result,
  qmi_cmd_id_e_type                        cmd,
  uint32                                   ep_id
);

/*===========================================================================
  FUNCTION QMI_INSTANCE_BY_QMUX_HANDLE()

  DESCRIPTION
    To obtain QMI instance from QMUX handle. Qmux handle passed in must 
    ALWAYS be the service handle.

  PARAMETERS
    qmux_handle : QMUX service handle returned to service by qmux_reg_service

  RETURN VALUE
    QMI Instance if qmux_handle is valid
    QMI_INSTANCE_MAX if qmux_handle is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern qmi_instance_e_type  qmi_instance_by_qmux_handle
(
  qmux_svc_info_type * service_handle
);


/*===========================================================================
  FUNCTION QMI_GET_NUM_INSTANCES()

  DESCRIPTION
    Returns number of QMI instances.

  PARAMETERS
    qmi_instance : instance of QMI

  RETURN VALUE
    Number of QMI instances defined

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern uint8 qmi_get_num_instances( void );

/*===========================================================================
  FUNCTION QMI_GET_INSTANCE_INITIALIZED()

  DESCRIPTION
    Returns instance initialized flag.

  PARAMETERS
    qmi_instance : instance of QMI

  RETURN VALUE
    TRUE if QMUX instance initialized
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmi_get_instance_initialized
(
  qmi_instance_e_type     qmi_instance
);

/*===========================================================================
  FUNCTION QMUX_VERIFY_DTR_HIGH()

  DESCRIPTION
    Returns DTR status for the QMI instance

  PARAMETERS
    qmi_inst : instance of QMI

  RETURN VALUE
    TRUE if DTR is asserted for the instance
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean  qmux_verify_dtr_high
(
  qmi_instance_e_type     qmi_inst
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
/*===========================================================================
FUNCTION QMUX_NOTIFY_SERVICE_REGISTRATION()

DESCRIPTION
  Function to learn about qmi service registration with qmux. This function 
  posts a command to qmi_modem task to notify TE about the new 
  service availability. 

PARAMETERS 
  service_id    : QMI Service ID
  qmi_instance  : QMI Instance (ignored for new QMUX based service)
  is_legacy_svc : The value is true if legacy QMI service,
                 else FALSE for new QMUX based QMI Service
 
RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void  qmux_notify_service_registration
(
  qmux_service_e_type                   service_id,
  qmi_instance_e_type                   qmi_instance,
  boolean                               is_legacy_svc
);
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/

/*===========================================================================
  FUNCTION QMUX_GET_DEFAULT_EP_ID_FROM_INST

  DESCRIPTION
    This function returns default EP id of the QMI instance
  
  PARAMETERS  
    qmi_inst:  QMI instance
  
  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern uint32 qmux_get_default_ep_id_from_inst
(
  qmi_instance_e_type  qmi_inst
);

/*===========================================================================
  FUNCTION QMUX_GET_INST_FROM_EP_ID

  DESCRIPTION
    This function returns the QMI instance which has ep_id as the default EP
  
  PARAMETERS
    ep_id:  physical data port end point id
  
  DEPENDENCIES
    None

  RETURN VALUE
    QMI instance if success
    QMI_INSTANCE_MAX if failure

  SIDE EFFECTS
    None
===========================================================================*/
extern qmi_instance_e_type qmux_get_inst_from_ep_id
(
  uint32  ep_id
);

/*===========================================================================
  FUNCTION QMUX_ADD_CONTROL_PORT

  DESCRIPTION
    This function adds an QMI control port for open
  
  PARAMETERS
    sio_port:       SIO port id
    default_ep_id:  default end point id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_add_control_port
(
  sio_port_id_type  sio_port,
  uint32            default_ep_id
);

/*===========================================================================
  FUNCTION QMUX_REMOVE_CONTROL_PORT

  DESCRIPTION
    This function removes a QMI control port
  
  PARAMETERS  
    sio_port:  SIO port id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_remove_control_port
(
  sio_port_id_type  sio_port
);

/*===========================================================================
  FUNCTION QMUX_ADD_DATA_PORT

  DESCRIPTION
    This function adds an Rmnet data port for open
  
  PARAMETERS  
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_add_data_port
(
  sio_port_id_type  sio_port,
  uint32            ep_id,
  uint32            consumer_pipe_num,
  uint32            producer_pipe_num
);

/*===========================================================================
  FUNCTION QMUX_REMOVE_DATA_PORT

  DESCRIPTION
    This function removes an Rmnet data port
  
  PARAMETERS  
    ep_id:  data port EP id
  
  DEPENDENCIES
    None

  RETURN VALUE
    TRUE: success
    FALSE: failure

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_remove_data_port
(
  uint32  ep_id
);

#endif /* _DS_QMUX_H */

