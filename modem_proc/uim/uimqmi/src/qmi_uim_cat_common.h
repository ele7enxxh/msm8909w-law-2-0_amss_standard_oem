/*================================================================================

DESCRIPTION


Copyright (c) 2012 - 2013, 2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_cat_common.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/11/16    ar     Changes for LPA support
03/27/14    at     Usage of common QMI APIs from DS header
07/02/13    kb     Added support for memory leak detection
06/12/13    kb     Added QMI malloc and free wrapper functions
09/07/12    at     Initial version
=============================================================================*/

#ifndef QMI_UIM_CAT_COMMON_H
#define QMI_UIM_CAT_COMMON_H

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "customer.h"
#include "comdef.h"
#include "mmgsdilib_common.h"

#include "qmi_framework.h"
#include "qmi_svc_utils.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  Service related common defines
---------------------------------------------------------------------------*/
#define UIMQMI_SVC_INVALID_INSTANCE    (-1)
#define UIMQMI_SVC_MAX_CLIDS           (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION UIMQMI_MALLOC

  DESCRIPTION
    The uimqmi_malloc calls modem_mem_calloc to request memory from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * uimqmi_malloc
(
  uint32 size
);

/*===========================================================================
  FUNCTION UIMQMI_FREE

  DESCRIPTION
    The uimqmi_free frees the pointer from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_free
(
  void * ptr
);

/*===========================================================================
  FUNCTION UIMQMI_MMGSDI_SLOT_TO_SLOT_INDEX()

  DESCRIPTION
    Determines the slot index from the MMGSDI slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id
    slot_index_ptr      : output parameter containing slot index

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type uimqmi_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type       mmgsdi_slot_id,
  uint8                        * slot_index_ptr
);

/*===========================================================================
  FUNCTION UIMQMI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_deep_copy
(
  void       ** dest,
  const void  * src,
  uint32        size
);

/*===========================================================================
  FUNCTION UIMQMI_SEND_INDICATION()

  DESCRIPTION
    This function calls the QMI Framework API to send out the UIM or CAT
    service indication to client.

  PARAMETERS
    client_id   : client ID
    cmd_type    : type of indication
    ind_ptr     : dsm item
    service_type: service type

  RETURN VALUE
    TRUE  - For success
    FALSE - For qmi svc failure

  DEPENDENCIES
    QMI UIM or CAT service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
boolean uimqmi_send_indication
(
  uint8                        client_id,
  uint16                       cmd_type,
  dsm_item_type              * ind_ptr,
  qmux_service_e_type          service_type
);

/*===========================================================================
  FUNCTION UIMQMI_PUT_RESULT_SEND_RESPONSE_IND

  DESCRIPTION
    This function copies the result TLV and send response or indication to
    the client.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_put_result_send_response_ind
(
  qmi_cmd_buf_type *       cmd_buf_p,
  dsm_item_type *          response_p,
  qmi_error_e_type         errval,
  qmux_service_e_type      service_type,
  boolean                  ind_required,
  uint8                    clid,
  uint16                   request_id
);
#endif /* QMI_UIM_CAT_COMMON_H */
