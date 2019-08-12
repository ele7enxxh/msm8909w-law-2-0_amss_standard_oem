#ifndef QMI_SI_H
#define QMI_SI_H
/*---------------------------------------------------------------------------
   qmi_si.h - QMI Supported Interfaces
---------------------------------------------------------------------------*/
/**===========================================================================
  @file qmi_si.h

@brief 
  QMI Supported Interfaces 
  This module contains the data structures that describe the static
  tables maintained by each QMI service to describe what messages
  and TLVs they support.
 
    
               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary


============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/qmi_si.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
2011-11-11  dna  Initial revision.
2012-10-07  pb   Support for generic functions to retrieve implementation details 

=============================================================================*/

/*****************************************************************************
 * Include Files
 * *************************************************************************/

#include "comdef.h"
#include "stdint.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * *************************************************************************/

#define QMI_SI_SUCCESS              ( 0)
#define QMI_SI_NOT_FOUND_ERR        (-1)
#define QMI_SI_NO_MEM_ERR           (-2)
#define QMI_SI_INTERNAL_ERR         (-3)
#define QMI_SI_INVALID_PARAM_ERR    (-4)
#define QMI_SI_NOT_READY_ERR        (-5)
#define QMI_SI_DUP_REG_ERR          (-6)

#define QMI_SI_TLV_SET( x )  (1 << ( ((x) - 0x10) & 0x1F ))  
#define QMI_SI_TLV_CLEAR( x ) 0

/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
   Data types to represent the static data structures maintained by each
   QMI Service owner to communicate what messages and optional TLVs are 
   supported in this branch of their implementation of the service.
*//* ------------------------------------------------------------------------*/
typedef struct {
  const uint32_t array_size; /* number of items in tlv_array */
  const uint32_t * const tlv_array;
} qmi_implemented_optionals;

typedef struct {
  const qmi_implemented_optionals *req;
  const qmi_implemented_optionals *resp;
  const qmi_implemented_optionals *ind;
} qmi_implemented_optionals_tuple;

typedef struct {
  const uint32_t max_msg_id; /* number of items in message_array - 1 */
  const qmi_implemented_optionals_tuple * const * const message_array;
} qmi_implemented_messages;

typedef struct qmi_si_list_entry_struct qmi_si_list_entry_type;

struct qmi_si_list_entry_struct {
  uint32_t                         service_id;
  uint32_t                         instance_id;
  uint32_t                         major_vers;
  uint32_t                         minor_vers;
  const qmi_implemented_messages   *si_tbl;
  qmi_si_list_entry_type           *next;
};

typedef int qmi_si_error_type;

/*****************************************************************************
 * Function Declarations and Documentation
 * *************************************************************************/

/*===========================================================================
  FUNCTION  qmi_si_register_object
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  All services register their supported interfaces for logging purposes,
  to keep track as the code branches of what messages and optional TLVs
  are supported for each request, response and indication message.
  For testing and documentation purposes the code must log this information.

@param[in]  service_obj           service Object
@param[in]  instance_id           Instance id of this service, 0 if unused
@param[in]  si_tbl                List of messages implemented by the service
                                  and the optional TLVs supported for the message

@retval    QMI_SI_SUCCESS          Success
@retval    QMI_SI_NO_MEM_ERR       Allocation failure
@retval    QMI_SI_INTERNAL_ERR     Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_register_object
(
  qmi_idl_service_object_type    service_obj,
  uint32_t                       instance_id,
  const qmi_implemented_messages *si_tbl
);


/*===========================================================================
  FUNCTION  qmi_si_unregister_object
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Unregister a previously registered service ID from QMI SI.

@param[in]  service_obj           service Object
@param[in]  instance_id           Instance id of this service, 0 if unused

@retval    QMI_SI_SUCCESS          Success
@retval    QMI_SI_INTERNAL_ERR     Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_unregister_object
(
  qmi_idl_service_object_type    service_obj,
  uint32_t                       instance_id
);

/*===========================================================================
  FUNCTION  qmi_si_register_id
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  All services register their supported interfaces for logging purposes,
  to keep track as the code branches of what messages and optional TLVs
  are supported for each request, response and indication message.
  For testing and documentation purposes the code must log this information.

@param[in]  service_id            Service ID of the service.
@param[in]  instance_id           Instance id of this service, 0 if unused
@param[in]  si_tbl                List of messages implemented by the service
                                  and the optional TLVs supported for the message

@retval    QMI_SI_SUCCESS          Success
@retval    QMI_SI_NO_MEM_ERR       Allocation failure
@retval    QMI_SI_INTERNAL_ERR     Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_register_id
(
  uint32_t                         service_id,
  uint32_t                         instance_id,
  uint32_t                         major_vers,
  uint32_t                         minor_vers,
  const qmi_implemented_messages *si_tbl
);


/*===========================================================================
  FUNCTION  qmi_si_unregister_id
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Unregister a previously registered service ID from QMI SI.

@param[in]  service_obj           service Object (NULL if not available)
@param[in]  service_id            service ID. This parameter is ignored if
                                  service_obj is provided (Not NULL).
@param[in]  instance_id           Instance id of this service, 0 if unused

@retval    QMI_SI_SUCCESS          Success
@retval    QMI_SI_INTERNAL_ERR     Unknown failure
*//* ------------------------------------------------------------------------*/
qmi_si_error_type qmi_si_unregister_id
(
  uint32_t                         service_id,
  uint32_t                         instance_id
);

/*===========================================================================
  FUNCTION  qmi_si_get_service_id_impl
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Get a SI Table for a particular service ID or object

@param[in]  service_id            service ID.
@param[in]  instance_id           Instance id of this service, 0 if unused

@retval    pointer to the QMI SI table, NULL on failure
*//* ------------------------------------------------------------------------*/
qmi_si_list_entry_type *qmi_si_get_service_id_impl
(
  uint32_t                         service_id,
  uint32_t                         instance_id
);

/*===========================================================================
  FUNCTION  qmi_si_get_service_obj_impl
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Get a SI Table for a particular service object or object

@param[in]  service_obj           service Object
@param[in]  instance_id           Instance id of this service, 0 if unused

@retval    pointer to the QMI SI table, NULL on failure
*//* ------------------------------------------------------------------------*/
qmi_si_list_entry_type *qmi_si_get_service_obj_impl
(
  qmi_idl_service_object_type    service_obj,
  uint32_t                         instance_id
);

/*===========================================================================
  FUNCTION  qmi_si_get_all_impl
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Get a list of all service SI tables registered with QMI SI.

@retval    pointer to a list containing all the registered QMI SI tables.
           (NULL if the nothing is registered).
*//* ------------------------------------------------------------------------*/
qmi_si_list_entry_type *qmi_si_get_all_impl
(
  void
);

/*===========================================================================
  FUNCTION  qmi_si_free_list
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Frees a list provided by qmi_si_get*_impl functions

@param[in]   list     The list to free

@retval    None
*//* ------------------------------------------------------------------------*/
void qmi_si_free_list
(
  qmi_si_list_entry_type *list
);

/*===========================================================================
  FUNCTION  qmi_si_get_supported_msgs_resp
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Writes all the message-ids supported by a service in 
  qmi_get_supported_msgs_resp_v01 struct.

@param[in]      si_tbl                pointer to the list of supported 
                                      message-ids for a given service.
@param[out]     rsp_msg               outgoing response.

@retval         QMI_SI_SUCCESS        Success
@retval         QMI_SI_INTERNAL_ERR   Unknown failure
*//* ------------------------------------------------------------------------*/
extern qmi_si_error_type qmi_si_get_supported_msgs_resp
(
  const qmi_implemented_messages    *si_tbl,
  qmi_get_supported_msgs_resp_v01   *rsp_msg
);

/*===========================================================================
  FUNCTION  qmi_si_get_supported_fields_resp
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  writes TLV fields for request, response and indication messages for 
  corresponding message-id in struct qmi_get_supported_fields_resp_v01.

@param[in]      si_tbl                    pointer to the list of supported 
                                          message-ids for a given service.
@param[in]      msg_id                    message-id requested for 
                                          implementation details.
@param[out]     rsp_msg                   outgoing response.

@retval         QMI_SI_SUCCESS            Success
@retval         QMI_SI_INTERNAL_ERR       Unknown failure
*//* ------------------------------------------------------------------------*/
extern qmi_si_error_type qmi_si_get_supported_fields_resp
(
  const qmi_implemented_messages       *si_tbl,
  uint16_t                             msg_id, 
  qmi_get_supported_fields_resp_v01    *rsp_msg 
);

#endif /* #ifndef QMI_SI_H */
