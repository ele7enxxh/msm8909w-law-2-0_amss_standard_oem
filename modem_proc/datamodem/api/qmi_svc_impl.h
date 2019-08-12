#ifndef QMI_SVC_IMPL_H
#define QMI_SVC_IMPL_H 

/*===========================================================================

                         Q M I _ S V C _ I M P L . H

DESCRIPTION

This file defines the interface for the Terminal Equipment to retrieve the 
 implementation details of a service on the MPSS (Modem Processor Sub-System).
 
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/qmi_svc_impl.h#1 $

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/qmi_svc_impl.h#1 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/12    pb     Ability to retrieve the supported interfaces / TLVs per 
                   service from the implementation 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "dsm_pool.h"
#include "qmi_si.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Generic messages IDs for retriving the supported interfaces / TLVs per 
  service from the implementation 
---------------------------------------------------------------------------*/ 
#define QMI_GET_SUPPORTED_MESSAGES_ID (0x1E)
#define QMI_GET_SUPPORTED_FIELDS_ID   (0x1F)
   
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=========================================================================== 
 
FUNCTION QMI_SVC_IMPL_GET_SUPPORTED_MSGS_RESP()    

DESCRIPTION 
  service supported message-ids with return if it is available
 
PARAMETERS
  si_tbl           : pointer to the list of supported message-ids for a given
                     service
  svc_obj          : handle to the static service object

RETURN VALUE 
  ptr to response or NULL if none.

DEPENDENCIES 
  None 

SIDE EFFECTS 
  None
 
===========================================================================*/ 

extern dsm_item_type* qmi_svc_impl_get_supported_msgs_resp
(
  const qmi_implemented_messages *  si_tbl, 
  qmi_idl_service_object_type       svc_obj
);

/*=========================================================================== 
 
FUNCTION QMI_SVC_IMPL_GET_SUPPORTED_FIELDS_RESP()  

DESCRIPTION 
  service supported TLV fields for request, response and indication messages
  for corresponding message id with return if it is available
 
PARAMETERS
  sdu_in           : incoming request
  si_tbl           : pointer to the list of supported message-ids for a given
                     service
  svc_obj          : handle to the static service object

RETURN VALUE 
  ptr to response or NULL if none.

DEPENDENCIES 
  None 

SIDE EFFECTS 
  None
 
===========================================================================*/ 

extern dsm_item_type* qmi_svc_impl_get_supported_fields_resp
(
  dsm_item_type **                  sdu_in, 
  const qmi_implemented_messages *  si_tbl, 
  qmi_idl_service_object_type       svc_obj
);

#endif /* QMI_SVC_IMPL_H */

