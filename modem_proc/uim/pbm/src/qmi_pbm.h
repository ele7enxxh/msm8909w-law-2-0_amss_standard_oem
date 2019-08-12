#ifndef _QMI_PBM_H
#define _QMI_PBM_H
/*===========================================================================

                         Q M I _ P B M . H

DESCRIPTION



EXTERNALIZED FUNCTIONS



Copyright (c) 2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
03/23/10    sk     Initial revision.
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "qmi_framework.h"
#include "qmi_pbm_if.h"

/*===========================================================================
  FUNCTION QMI_PBM_INIT()

  DESCRIPTION
    Register the PBM service with QMUX for all applicable QMI links

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_pbm_init(void);

/*===========================================================================
  FUNCTION QMI_PBM_HANDLE_FW_CMD()

  DESCRIPTION
    Process QMI-PBM cmds posted from Framework callbacks

  PARAMETERS
    cmd: qmi_pbm_cmd_enum_type
    user_data_ptr: void ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_handle_fw_cmd
(
  void *            user_data_ptr
);

/*===========================================================================
  FUNCTION QMI_PBM_INIT_CBACK()

  DESCRIPTION
    Callback for Service initialization from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbm_fw_init_cback
(
  uint16 num_instances
);

/*===========================================================================
  FUNCTION QMI_PBMI_FW_INIT_CBACK_HDLR()

  DESCRIPTION
    Process the Service initialization request from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_init_cback_hdlr
(
  uint16 num_instances
);

/*===========================================================================
  FUNCTION QMI_PBM_ALLOC_CLID_CBACK()

  DESCRIPTION
    Callback to allocate the client from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_pbm_fw_alloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

/*===========================================================================
  FUNCTION QMI_PBM_ALLOC_CLID_HDLR()

  DESCRIPTION
    Process the client allocation request from Framework

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_alloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

/*===========================================================================
  FUNCTION QMI_PBM_FW_DEALLOC_CLID_CBACK()

  DESCRIPTION
    Deallocate the client data.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_pbm_fw_dealloc_clid_cback
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

/*===========================================================================
  FUNCTION QMI_PBM_FW_DEALLOC_CLID_HDLR()

  DESCRIPTION
    Process the dealloc clid request from framework.

  PARAMETERS
    common_msg_hdr: qmi_framework_common_msg_hdr_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_dealloc_clid_hdlr
(
  qmi_framework_common_msg_hdr_type * common_msg_hdr
);

/*===========================================================================
  FUNCTION QMI_PBMI_FW_REQ_HDLR()

  DESCRIPTION
    Process qmi request from Framework

  PARAMETERS
    msg_hdr: qmi_framework_msg_hdr_type ptr
    sdu: dsm_item_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_pbmi_fw_req_hdlr
(
  qmi_framework_msg_hdr_type* msg_hdr,
  dsm_item_type * sdu_in
);

/*===========================================================================
  FUNCTION QMI_PBMI_FILL_ARRAY_TLV()

  DESCRIPTION
    Fill the data for an array type TLV

  PARAMETERS
    itm       : ptr to dsm_item_type ptr
    tag       : Tag type in TLV
    arr_len   : Array length
    instances : Number of instances in the array

  RETURN VALUE
    TRUE if tlv is filled successfully. FALSE otherwise.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_pbmi_fill_array_tlv
(
  dsm_item_type **itm,
  uint8           tag,
  uint16          arr_len,
  uint8           instances
);

#endif /* _QMI_PBM_H */

