#ifndef _QMI_PBM_SVC_H
#define _QMI_PBM_SVC_H
/*===========================================================================

                         Q M I _ M M C P _ S V C . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2011QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
     $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_svc.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/04/14    stv    Migration to DATA exposed API usage 
02/28/14    stv    get_pb_capabilities new tlv addition packing issue
                   addition of encode/decode idl libraries 
12/26/11    sk     Initial version
===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "ds_qmi_fw_common.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"

typedef struct
{
  ds_qmi_fw_svc_cfg_type fw_svc_cfg;
  void *svc_sp;
  int16  service_id;
}qmi_pbm_svc_config_type;


/*===========================================================================
  FUNCTION QMI_PBM_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI PBM message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to structure to contain decoded message
    p_req_size  Size of structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful,
     otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_pbm_idl_message_decode
( 
  qmi_idl_service_object_type svc, 
  uint16_t message_id, 
  dsm_item_type *msg_ptr, 
  void *p_req, 
  uint32_t p_req_size
);


/*===========================================================================
  FUNCTION QMI_PBM_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI PBM message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to structure containing message data
    p_src_size   Size of structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbm_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void                         *p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type                **p_response 
);



#endif /* _QMI_PBM_SVC_H */
