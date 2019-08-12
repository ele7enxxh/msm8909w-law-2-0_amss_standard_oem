#ifndef QMI_FRAMEWORK_H
#define QMI_FRAMEWORK_H
/*===========================================================================

                            Q M I _ F W . H
DESCRIPTION

  The QMI Services Framework - external interface header file.
 
EXTERNALIZED DATA 
  qmi_framework_svc_version_type
    Type for framework version definition.
 
  qmi_framework_common_msg_hdr_type
    Type for common framework msg header.
 
  qmi_framework_msg_hdr_type
    Type for framework msg header.
 
  qmi_framework_svc_cbs_type
    Type for framework callback function.
 
  qmi_framework_svc_config_type
    Type for framework service configuration. 
 
EXTERNALIZED FUNCTIONS
 
  qmi_framework_reg_service()
    Register a service to the Framework with a service configuration.

  qmi_framework_dereg_service()
    Deregister the service from the QMI framework.
 
  qmi_framework_svc_send_response()
    Send a response message from service to the QMI Framework. 
 
  qmi_framework_svc_send_ind()
    Send an Indication from service to the QMI Framework. 
 
  qmi_framework_alloc_msg_header()
    Allocates memory for a msg header. 
 
  qmi_framework_dealloc_msg_header()
    Deallocates the Memory for the msg header. 
 
  qmi_framework_svc_send_alloc_clid_result()
    Send a Client ID allocation result to QMI_CTL service. 
 
  qmi_framework_svc_send_dealloc_clid_result()
    Send a Client ID deallocation result to QMI_CTL service.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
 
                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/qmi_framework.h#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/30/11    rk      Added bundle message support to QMI Framework  
01/20/11    rk      Created Module 
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "qmi_svc_defs.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                               DATA TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI_FRAMEWORK_MAX_CLIENTS - Max clients per service including broadcast clid.
---------------------------------------------------------------------------*/
#define QMI_FRAMEWORK_SVC_MAX_CLIENTS   (0xFF) /* max 255 per QMI service  */

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_ERR_E_TYPE

DESCRIPTION
  Enumeration for qmi Framework Error code.
===========================================================================*/
typedef enum
{
  QMI_FRAMEWORK_ERR_NONE                     = 0,
  QMI_FRAMEWORK_ERR_FW_NOT_READY             = 1,  /* Framework not ready */
  QMI_FRAMEWORK_ERR_SVC_ALREADY_REGISTERED   = 2,
  QMI_FRAMEWORK_ERR_SVC_REG_MISSING_CBACK    = 3,
  QMI_FRAMEWORK_ERR_MEMORY                   = 4,
  QMI_FRAMEWORK_ERR_MAX,
  QMI_FRAMEWORK_ERR_WIDTH                   = 0xFFFF
} qmi_framework_err_e_type;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_SVC_VERSION_TYPE

DESCRIPTION
  Type for framework version definition.
===========================================================================*/
typedef struct
{
  uint16  major; 
  uint16  minor; 
} qmi_framework_svc_version_type;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_COMMON_MSG_HDR_TYPE

DESCRIPTION 
  Type for common framework msg header. 
===========================================================================*/
typedef struct
{
  qmux_service_e_type   service;            /* QMI Service type           */ 
  uint8                 client_id;          /* Service client ID          */ 
  uint16                transaction_id;     /* Transaction ID             */ 
  int32                 qmi_instance;       /* QMI Instance               */ 
}qmi_framework_common_msg_hdr_type;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_MSG_HDR_TYPE

DESCRIPTION 
  Type for framework msg header and this type used to exchange information
  between framework and services. 
===========================================================================*/
typedef struct
{
                                     /* framework common msg header */ 
  qmi_framework_common_msg_hdr_type  common_hdr;   
                                     /* Control flags for serivce message type */ 
  uint8                              msg_ctl_flag; 
                                     /* Total svc messages length in a transaction */  
  uint16                             msg_len;      
} qmi_framework_msg_hdr_type;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_SVC_CBS_TYPE

DESCRIPTION 
  Callback functions for service configuration. Used when service gets
  registered with the Framework.
===========================================================================*/
typedef struct _qmi_framework_svc_callbacks
{
                             /* cb for client ID allocation                */ 
  boolean  (* alloc_clid)    ( qmi_framework_common_msg_hdr_type * svc_common_hdr  );
                             /* cb for client ID de allocation             */ 
  void     (* dealloc_clid)  ( qmi_framework_common_msg_hdr_type * svc_common_hdr );  
                             /* cb for service initialization              */ 
  void     (* init_cback)    ( uint16 num_instances);
                             /* cb for service cmd/msg handler             */ 
  void     (* cmd_hdlr)      ( qmi_framework_msg_hdr_type * msg_hdr, dsm_item_type ** sdu );

} qmi_framework_svc_cbs_type;

/*===========================================================================
TYPEDEF QMI_FRAMEWORK_SVC_CONFIG TYPE

DESCRIPTION 
  QMI Service config type based on new Framework. 
===========================================================================*/
typedef struct
{
  qmi_framework_svc_version_type      base_version;     /* Base version           */ 
  qmi_framework_svc_version_type      addendum_version; /* Addendum version       */ 
  qmi_framework_svc_cbs_type          cbs;              /* Call back for svc reg  */ 
} qmi_framework_svc_config_type;

/*===========================================================================

                              EXTERNAL FUNCTION 

===========================================================================*/
/*===========================================================================
FUNCTION QMI_FRAMEWORK_REG_SERVICE()

DESCRIPTION
  Register a new QMI service to the Framework with a service configuration.
  Once registration is successful subsequently the service init callback
  function gets called to initialize its service specific configuration.

PARAMETERS
  service      : service type
  cfg          : configuration parameters for the service instance

RETURN VALUE
  qmi_error_e_type  : QMI_ERR_NONE on successful registration. 

DEPENDENCIES 
  None

SIDE EFFECTS
  None
===========================================================================*/
qmi_framework_err_e_type qmi_framework_reg_service
(
  qmux_service_e_type              service,
  qmi_framework_svc_config_type *  cfg
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_DEREG_SERVICE()

DESCRIPTION
  Deregister the already registered QMI service from framework.Once
  deregistration is successful subsequently the service dealloc callback
  function gets called to clean up the service specific pending transaction.
  
PARAMETERS
  service      : service type

RETURN VALUE
  None. 

DEPENDENCIES
  Service should be registered previously.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_dereg_service
(
  qmux_service_e_type     service
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_RESPONSE()

DESCRIPTION
  External QMI service modules use this function to send a response message
  to the QMI Framework. The QMI framework processes the response and then
  sends to QMUX layer. If the response come from an external task, then it
  performs a task switch to QMI task.

PARAMETERS
  msg_hdr   : msg header sent from svc
  msg_ptr   : response to send out

RETURN VALUE
  TRUE if Framework was able to start processing the message.
  FALSE otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_svc_send_response
(
  qmi_framework_msg_hdr_type *   msg_hdr,
  dsm_item_type *                msg_ptr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_IND()

DESCRIPTION
  External QMI service modules use this function to send an Indication message
  to the QMI Framework. The QMI framework processes the Indication and then
  sends to QMUX layer. If the Indication come from an external task, then it
  performs a task switch to QMI task.

PARAMETERS
  msg_hdr   : msg header sent from svc
  msg_ptr   : Indication to send out

RETURN VALUE
  TRUE if Framework was able to start processing the message.
  FALSE otherwise.
  
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_framework_svc_send_ind   
(
  qmi_framework_msg_hdr_type *    msg_hdr,
  dsm_item_type *                 msg_ptr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_ALLOC_MSG_HEADER()

DESCRIPTION
  Allocates memory for a framework msg header.

PARAMETERS
  None.

RETURN VALUE
 qmi_framework_msg_hdr_type * : ptr to framework msg header type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_framework_msg_hdr_type * qmi_framework_alloc_msg_header
(
  void
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_DEALLOC_MSG_HEADER()

DESCRIPTION
  Deallocates Memory for the framework msg header. 

PARAMETERS
  msg_hdr: pointer to message header.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_dealloc_msg_header
(
  qmi_framework_msg_hdr_type **  msg_hdr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_ALLOC_CLID_RESULT()

DESCRIPTION
  QMI service calls this function to report client id allocation success /
  failure to QMI_CTL service.

PARAMETERS
  result     : Client ID allocation result.
  common_hdr : common msg hdr type.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_svc_send_alloc_clid_result
(
  qmi_result_e_type                    result,
  qmi_framework_common_msg_hdr_type *  common_msg_hdr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_DEALLOC_CLID_RESULT()

DESCRIPTION
  QMI service calls this function to report client id deallocation success /
  failure to QMI_CTL service. 

PARAMETERS
  result     : Client ID deallocation result.
  common_hdr : common msg hdr type.

RETURN VALUE
  Void.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
==========================================================================*/
void qmi_framework_svc_send_dealloc_clid_result
(
  qmi_result_e_type                    result,
  qmi_framework_common_msg_hdr_type *  common_msg_hdr
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_ALLOC_CLID_RESULT_EX()

DESCRIPTION
  QMI service calls this function to report client id allocation success /
  failure to QMI_CTL service.
  Going forward all services are expected to use this API instead of
  qmi_framework_svc_send_alloc_clid_result()

PARAMETERS
  result     : Client ID allocation result.
  common_hdr : common msg hdr type.
  errval     : Err value 

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_framework_svc_send_alloc_clid_result_ex
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
);

/*===========================================================================
FUNCTION QMI_FRAMEWORK_SVC_SEND_DEALLOC_CLID_RESULT_EX()

DESCRIPTION
  QMI service calls this function to report client id deallocation success /
  failure to QMI_CTL service.
  Going forward all services are expected to use this API instead of
  qmi_framework_svc_send_dealloc_clid_result()

PARAMETERS
  result     : Client ID deallocation result.
  common_hdr : common msg hdr type.
  errval     : Error value
RETURN VALUE
  Void.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
==========================================================================*/
void qmi_framework_svc_send_dealloc_clid_result_ex
(
  qmi_result_e_type                     result,
  qmi_framework_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
);

#endif /* QMI_FRAMEWORK_H */
