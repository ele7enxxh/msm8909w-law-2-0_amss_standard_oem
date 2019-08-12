#ifndef _DS_QMI_FW_COMMON_H
#define _DS_QMI_FW_COMMON_H
/*===========================================================================

                  D S _ Q M I _ F W _ C O M M O N . H

DESCRIPTION

 The Data Services QMI framework common header file. This file contains
 routines to support common message handling for QMI services using the
 new QMI framework.

EXTERNALIZED FUNCTIONS

Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_qmi_fw_common.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/13/14    vrk    Added cmd_type to ds_qmi_fw_async_cmd_buf_type
08/20/12    wc     Initial version.
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "qmi_framework.h"
#include "qmi_svc_defs.h"

#include "dsm.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
typedef struct
{
  qmi_framework_svc_config_type    fw_cfg;
  qmi_svc_cmd_hdlr_type *          cmd_hdlr_array;   
  uint16                           cmd_num_entries;  
  boolean                          registered;
} ds_qmi_fw_svc_cfg_type;

typedef struct
{
  uint32 cookie;
  void * cmd_buf;
  uint16 cmd_type;
} ds_qmi_fw_async_cmd_buf_type;
/*===========================================================================

                         EXTERNAL FUNCTIONS 

===========================================================================*/

/*===========================================================================
  FUNCTION DS_QMI_FW_RECV()

  DESCRIPTION
    Receives the QMUX SDU and passes each individual SVC message to
    ds_qmi_fw_input.

  PARAMETERS
    svc_cfg: service configuration info
    svc_sp : service provided state pointer
    msg_hdr: message hdr passed in from the QMI framework 
    cl_sp  : client state pointer 
    sdu_in : DSM item containing incoming msg

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_fw_recv
(
  ds_qmi_fw_svc_cfg_type            * svc_cfg,
  void                              * svc_sp,
  qmi_framework_msg_hdr_type        * msg_hdr,
  qmi_common_client_state_type      * cl_sp,
  dsm_item_type                     * sdu_in
);

/*===========================================================================
FUNCTION DS_QMI_FW_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI Service response to a
  clients.

PARAMETERS 
  common_hdr: QMI framework common message hdr
  cmd_buf_p:  command buffer
  msg_ptr :   dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI service must be initialized and registered with Framework

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qmi_fw_send_response
(
  qmi_framework_common_msg_hdr_type * common_hdr,
  qmi_cmd_buf_type                  * cmd_buf_p,
  dsm_item_type                     * msg_ptr
);

/*===========================================================================
  FUNCTION DS_QMI_FW_SEND_IND()

  DESCRIPTION
    This function calls the QMI Framework API to send out the QMI service
    indication to a client.

  PARAMETERS
    common_hdr: QMI Framework message header
    cmd_type  : type of indication
    ind       : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_qmi_fw_send_ind
(
  qmi_framework_common_msg_hdr_type * common_hdr,
  uint16                              cmd_type,
  dsm_item_type                     * ind
);

/*===========================================================================
  FUNCTION DS_QMI_FW_NEW_CMD_BUF()

  DESCRIPTION
    Allocate a QMI message command buffer

  PARAMETERS
    x_p : QMI Transaction that the command buf will be added to

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cmd_buf_type * ds_qmi_fw_new_cmd_buf
(
  qmi_transaction_type * x_p
);

/*===========================================================================
  FUNCTION DS_QMI_FW_FREE_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI message command buffer

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
void ds_qmi_fw_free_cmd_buf
(
  qmi_cmd_buf_type ** cmd_buf_handle
);

/*===========================================================================
  FUNCTION DS_QMI_FW_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the QMI service 
    by the client state pointer provided

  PARAMETERS
    svc_sp :  pointer to service state from which the transaction is allocated
    cl_sp :  client state pointer for client to get transaction buffer for
             or NULL if sending a broadcast indication

  RETURN VALUE
    NULL if no transaction buffer is available
    otherwise, pointer to transaction buffer

  DEPENDENCIES
    QMI service must be initialized and registered with Framework
 
  SIDE EFFECTS
    service global transaction might be allocated.
===========================================================================*/
qmi_transaction_type * ds_qmi_fw_get_transaction
(
  void                         * svc_sp,
  qmi_common_client_state_type * cl_sp
);

/*===========================================================================
  FUNCTION DS_QMI_FW_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI service must be initialized and registered with Framework

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
void ds_qmi_fw_free_transaction
(
  qmi_transaction_type **  x_p
);

/*===========================================================================
  FUNCTION DS_QMI_FW_NEW_ASYNC_CMD_BUF()

  DESCRIPTION
    Allocate a QMI asynchronous command buffer

  PARAMETERS
    cmd_buf_p : original QMI message command buffer
    cookie       : cookie to validate asynchronous command buffer

  RETURN VALUE
    pointer to allocated asynchronous command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ds_qmi_fw_async_cmd_buf_type * ds_qmi_fw_new_async_cmd_buf
(
  qmi_cmd_buf_type * cmd_buf_p,
  uint32             cookie
);

/*===========================================================================
  FUNCTION DS_QMI_FW_COMMON_CL_INIT()

  DESCRIPTION
    Common client initialization

  PARAMETERS
    sp         :  service provided state pointer (user data)
    client_sp  :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_fw_common_cl_init
(
  void                         * svc_sp,
  qmi_common_client_state_type * cl_sp
);

/*===========================================================================
  FUNCTION DS_QMI_FW_COMMON_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    ptr to response
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmi_fw_common_reset_client
(
  qmi_common_client_state_type * cl_sp
);

#define ds_qmi_fw_free_async_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

#endif /* _DS_QMI_FW_COMMON_H */

