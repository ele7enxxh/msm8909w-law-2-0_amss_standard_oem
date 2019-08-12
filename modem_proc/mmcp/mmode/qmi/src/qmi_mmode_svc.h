#ifndef _QMI_MMODE_SVC_H
#define _QMI_MMODE_SVC_H
/*===========================================================================

                         Q M I _ M M C P _ S V C . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_svc.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    sk     Initial version
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"

typedef struct
{
  qmi_framework_svc_config_type  fw_cfg;
  qmi_svc_cmd_hdlr_type *cmd_hdlr_array;
  void *svc_sp;
  uint16 cmd_num_entries;
  int16  service_id;
}qmi_mmode_svc_config_type;


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION QMI_MMODE_SVC_INITIALIZE_COMMON_CLIENT()

DESCRIPTION
  This function initializes the transaction buffers and cmd buffers 
  pertaining to the common client data.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_mmode_svc_initialize_common_client
(
  qmi_common_client_state_type * cmn_cl_sp,
  void *                         svc_sp
);

/*===========================================================================
FUNCTION QMI_MMODE_SVC_REQ_HDLR()

DESCRIPTION
  This function parses the SDU and invokes the corresponding qmi svc request.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_mmode_svc_req_hdlr
(
  qmi_mmode_svc_config_type *     svc_cfg,
  qmi_framework_msg_hdr_type *   msg_hdr,
  qmi_common_client_state_type * cl_sp,
  dsm_item_type *                sdu_in 
);

/*===========================================================================
FUNCTION QMI_MMODE_SVC_SEND_RESPONSE()

DESCRIPTION
  This function forwards the qmi response data to framework.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean qmi_mmode_svc_send_response
(
  qmi_framework_common_msg_hdr_type *  common_hdr,
  qmi_cmd_buf_type *                   cmd_buf_p,
  dsm_item_type *                      msg_ptr
);

/*===========================================================================
FUNCTION QMI_MMODE_SVC_SEND_INDICATION()

DESCRIPTION
  This function forwards the qmi indication to framework.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean qmi_mmode_svc_send_indication
(
  qmi_framework_common_msg_hdr_type  *common_hdr,
  uint16           cmd_type,
  dsm_item_type *  ind
);


/*===========================================================================
FUNCTION QMI_MMODE_SVC_RESET_COMMON_CLIENT()

DESCRIPTION
  This function resets the common client data of qmi services.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void qmi_mmode_svc_reset_common_client
(
  qmi_common_client_state_type * cl_sp
);

/*===========================================================================
  FUNCTION QMI_MMODE_SVCI_FREE_TRANSACTION_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI MMODE SVC transaction command buffer

  PARAMETERS
    cmd_buf_handle : Handle to command buf pointer which points to the cmd
                     buffer to be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    Removes reference to this command buf from the associated transaction
    structure.  
===========================================================================*/
extern void qmi_mmode_svc_free_transaction_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
);

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI NAS message into c structure from incoming dsm item

  PARAMETERS
    svc          The QMI IDL service object.
    message_id   QMI message id
    msg_ptr      Pointer to dsm item that contains TLVs 
    p_req        Pointer to C structure to contain decoded message
    p_req_size   Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_mmode_idl_message_decode
( 
  qmi_idl_service_object_type svc, 
  uint16_t message_id, 
  dsm_item_type **msg_ptr, 
  void *p_req, 
  uint32_t p_req_size
);

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI NAS message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 boolean qmi_mmode_idl_message_encode
 ( 
   qmi_idl_service_object_type svc, 
   qmi_idl_type_of_message_type type, 
   uint16_t message_id, 
   void *p_src, 
   uint32_t p_src_size, 
   dsm_item_type **p_response 
 );

#endif /* _QMI_MMODE_SVC_H */
