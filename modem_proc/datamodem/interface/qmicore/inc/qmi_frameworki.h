#ifndef QMI_FRAMEWORKI_H
#define QMI_FRAMEWORKI_H
/*===========================================================================

                            Q M I _ F W I . H

DESCRIPTION

  The QMI Service Framework internal interface header file.

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/inc/qmi_frameworki.h#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/20/11    rk     Created Module
===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"

#include "ds_qmi_svc_ext.h"
#include "qmi_framework.h"
#include "ds_qmi_ctl_ext.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*===========================================================================
TYPEDEF QMI_FRAMEWORK_SVC_INFO_TYPE

DESCRIPTION 
  QMI Service Info type based on new Framework.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_instance_e_type     client_instance_map[QMI_FRAMEWORK_SVC_MAX_CLIENTS];
  boolean                 registered; /* Service reg status used internally */
  qmux_service_e_type     service_id; /* Service type                       */
  qmi_framework_svc_config_type  cfg;        /* Service configuration              */
  uint16                   seq_no[QMI_FRAMEWORK_SVC_MAX_CLIENTS]; /* Seq no */
} qmi_framework_svc_info_type;

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_RECV()

DESCRIPTION
  This function passes the QMUX SDU to QMI external svc cmd handler. The SVC
  cmd handler should be registered during the service registration. 
  
PARAMETERS
  qmi_instance  : qmi instance.
  service       : Individual service to the coresponding message.
  client_id     : Client ID corresponding the message. 
  sdu_in        : DSM item containing incoming msg.
  
RETURN VALUE
  TRUE  : Received QMUX SDU send to SVC hdlr successful.
  FALSE : if there was an error.

DEPENDENCIES
  QMI service must be registered previously to recive QMUX SDU. 

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean qmi_framework_svc_recv
( 
  qmi_instance_e_type     qmi_instance,
  qmux_service_e_type     service,
  uint8                   client_id, 
  dsm_item_type *         sdu_in
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_PROCESS_SVC_SEND()

DESCRIPTION
  This function process the external QMI service response or indication and
  then send to QMUX layer.

PARAMETERS
  user_data_ptr : user data
  qmi_cmd       : QMI cmd for Response or Indication.

RETURN VALUE
  TRUE if command was sent.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean qmi_framework_process_svc_send
(
  void *        user_data_ptr,
  uint16        qmi_cmd
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_PROCESS_CLIENT_REGISTRATION_RESULT()

DESCRIPTION
  This function process the external QMI service client registration result.

PARAMETERS
  result            : Client ID allocation result.
  client_reg_result : Client reg result.
  qmi_cmd           : reg/dereg.
  errval            : err value

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_framework_process_client_registration_result
(
  qmi_result_e_type                 reg_result,
  qmi_ctl_client_reg_result_type *  client_reg_result,
  uint16                            qmi_cmd,
  qmi_error_e_type                  errval
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_ALLOC_CLID()

DESCRIPTION
  Allocate a new Client ID to control point. This function is called by QMI_CTL
  service to allocate a client id and client ID allocation is asynchronous.
  On successfull allocation, the service sends a response to ctl by calling
  qmi_framework_svc_send_alloc_clid_result.

PARAMETERS
  common_hdr : framework common msg hdr type.

RETURN VALUE
  TRUE if the service process clid allocation.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern qmi_error_e_type  qmi_framework_svc_alloc_clid
(
  qmi_framework_common_msg_hdr_type *  common_hdr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_DEALLOC_CLID()

DESCRIPTION
  Deallocate the new Client ID.This function is called by QMI_CTL service to
  deallocate the client id and client ID de allocation is asynchronous. On
  successfull deallocation, the service sends a response to ctl by calling
  qmi_framework_svc_send_dealloc_clid_result.
  
PARAMETERS
  common_hdr : common header.

RETURN VALUE
  TRUE if the service process clid deallocation.
  FALSE if there was an error.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern qmi_error_e_type qmi_framework_svc_dealloc_clid
(
  qmi_framework_common_msg_hdr_type *  common_hdr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_POWERUP_INIT()

DESCRIPTION
  QMI Framework power up initialization.

PARAMETERS
  None

RETURN VALUE
  TRUE on successful initialization.
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean qmi_framework_powerup_init
(
  void
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_QMI_LINK_CLOSED()

DESCRIPTION
  Frees all existing service clients, as QMI link is closed.

PARAMETERS
  instance - the QMI instance whose link was disconnected

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void  qmi_framework_svc_qmi_link_closed
(
  qmi_instance_e_type  instance
);

/*===========================================================================
  FUNCTION QMI_FRAMEWORK_GET_SERVICE_CFG()

  DESCRIPTION
    Get Service configuration

  PARAMETERS
    svc_type    : type of qmux service    

  RETURN VALUE
    qmi_framework_svc_config_type
    NULL if input service handle or service type is invalid

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_framework_svc_config_type *  qmi_framework_get_service_cfg
(
  qmux_service_e_type  svc_type
);

#endif /* QMI_FRAMEWORKI_H */

