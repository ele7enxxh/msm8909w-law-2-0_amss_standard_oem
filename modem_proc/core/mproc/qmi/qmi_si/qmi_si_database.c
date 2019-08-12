/*---------------------------------------------------------------------------
   qmi_si_database.c - QMI Supported Interfaces Registration
---------------------------------------------------------------------------*/
/*****************************************************************************
  @file    qmi_si_database.c
  @brief   The QMI Supported Interfaces Registration
 
  DESCRIPTION
  This is the interface to register the QMI Supported Interfaces with a
  single entity (this module).  For example, a logging module may choose
  to retrieve the full list of supported interfaces to log it.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/qmi_si/qmi_si_database.c#1 $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
11/30/2011   aep     Initial Version
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qmi_si.h"
#include "qmi_si_os.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief
  Singleton instance of the structure holding state information. 
*//* ------------------------------------------------------------------------*/
static qmi_si_os_sem_type     qmi_si_list_lock;
static qmi_si_list_entry_type *qmi_si_list = NULL;
static boolean                qmi_si_inited = FALSE;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  qmi_si_init
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Initializes QMI SI internal structures

@retval    None
*//* ------------------------------------------------------------------------*/
void qmi_si_init(void)
{
  if(qmi_si_inited == FALSE)
  {
    qmi_si_os_sem_init(&qmi_si_list_lock);
    qmi_si_list = NULL;
    qmi_si_inited = TRUE;
  }
}

/*===========================================================================
  FUNCTION  qmi_si_deinit
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  De-Initializes QMI SI internal structures

@retval    None
*//* ------------------------------------------------------------------------*/
void qmi_si_deinit(void)
{
  if(qmi_si_inited == FALSE)
  {
    return;
  }
  qmi_si_inited = FALSE;

  qmi_si_os_sem_lock(&qmi_si_list_lock);
  qmi_si_free_list(qmi_si_list);
  qmi_si_list = NULL;
  qmi_si_os_sem_unlock(&qmi_si_list_lock);

  qmi_si_os_sem_deinit(&qmi_si_list_lock);
}

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
@param[in]  major_vers            Major version of the service
@param[in]  minor_vers            Minor version of the service
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
)
{
  qmi_si_list_entry_type *entry;

  if(qmi_si_inited == FALSE)
  {
    return QMI_SI_NOT_READY_ERR;
  }

  if(!si_tbl || !service_id)
  {
    return QMI_SI_INVALID_PARAM_ERR;
  }

  qmi_si_os_sem_lock(&qmi_si_list_lock);
  for(entry = qmi_si_list; entry != NULL; entry = entry->next)
  {
    if(entry->service_id == service_id && entry->instance_id == instance_id)
    {
      qmi_si_os_sem_unlock(&qmi_si_list_lock);
      return QMI_SI_DUP_REG_ERR;
    }
  }
  qmi_si_os_sem_unlock(&qmi_si_list_lock);


  entry = qmi_si_os_malloc(sizeof(qmi_si_list_entry_type));
  if(!entry)
  {
    return QMI_SI_NO_MEM_ERR;
  }

  entry->service_id  = service_id;
  entry->instance_id = instance_id;
  entry->major_vers  = major_vers;
  entry->minor_vers  = minor_vers;
  entry->si_tbl      = si_tbl;

  qmi_si_os_sem_lock(&qmi_si_list_lock);
  entry->next = qmi_si_list;
  qmi_si_list = entry;
  qmi_si_os_sem_unlock(&qmi_si_list_lock);

  return QMI_SI_SUCCESS;
}

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
)
{
  uint32_t service_id = 0;
  uint32_t major_vers = 0;
  uint32_t minor_vers = 0;
  int32_t rc;

  if(!service_obj)
  {
    return QMI_SI_INVALID_PARAM_ERR;
  }
  rc = qmi_idl_get_service_id(service_obj, &service_id);
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_SI_INTERNAL_ERR;
  }
  rc = qmi_idl_get_idl_version(service_obj, &major_vers);
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_SI_INTERNAL_ERR;
  }
  rc = qmi_idl_get_idl_minor_version(service_obj, &minor_vers);
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    minor_vers = 0;
  }

  return qmi_si_register_id(service_id, instance_id, 
		  major_vers, minor_vers, si_tbl);
}



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
)
{
  qmi_si_list_entry_type *i = NULL, *to_free = NULL, *prev = NULL;
  qmi_si_error_type rc = QMI_SI_NOT_FOUND_ERR;

  if(qmi_si_inited == FALSE)
  {
    return QMI_SI_INTERNAL_ERR;
  }

  if(service_id == 0)
  {
    return QMI_SI_INVALID_PARAM_ERR;
  }

  qmi_si_os_sem_lock(&qmi_si_list_lock);
  i = qmi_si_list;
  while(i)
  {
    if(i->service_id == service_id && i->instance_id == instance_id)
    {
      rc = QMI_SI_SUCCESS;
      if(prev)
      {
        prev->next = i->next;
      }
      else
      {
        qmi_si_list = i->next;
      }

      to_free = i;
      i = i->next;

      qmi_si_os_free(to_free);
      continue;
    }
    prev = i;
    i = i->next;
  }

  qmi_si_os_sem_unlock(&qmi_si_list_lock);

  return rc;
}

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
  uint32_t                         instance_id
)
{
  uint32_t service_id = 0;
  int32_t rc;

  if(!service_obj)
  {
    return QMI_SI_INVALID_PARAM_ERR;
  }
  rc = qmi_idl_get_service_id(service_obj, &service_id);
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    return QMI_SI_INTERNAL_ERR;
  }

  return qmi_si_unregister_id(service_id, instance_id);
}

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
)
{
  qmi_si_list_entry_type *i = NULL;
  qmi_si_list_entry_type *ret = NULL;
  qmi_si_list_entry_type *node = NULL;

  if(qmi_si_inited == FALSE)
  {
    return NULL;
  }

  qmi_si_os_sem_lock(&qmi_si_list_lock);
  for(i = qmi_si_list; i != NULL; i = i->next)
  {
    if(i->service_id == service_id && i->instance_id == instance_id)
    {
      node = qmi_si_os_malloc(sizeof(qmi_si_list_entry_type));
      if(!node)
      {
        break;
      }
      QMI_MEM_COPY(node, sizeof(qmi_si_list_entry_type), i, 
                   sizeof(qmi_si_list_entry_type));
      node->next = ret;
      ret = node;
    }
  }
  qmi_si_os_sem_unlock(&qmi_si_list_lock);
  return ret;
}

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
)
{
  uint32_t service_id = 0;
  int32_t rc;

  if(!service_obj)
  {
    return NULL;
  }
  rc = qmi_idl_get_service_id(service_obj, &service_id);
  if(rc != QMI_IDL_LIB_NO_ERR)
  {
    return NULL;
  }

  return qmi_si_get_service_id_impl(service_id, instance_id);
}

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
)
{
  qmi_si_list_entry_type *node = NULL, *ret = NULL, *i = NULL;
  
  if(qmi_si_inited == FALSE)
  {
    return NULL;
  }
  qmi_si_os_sem_lock(&qmi_si_list_lock);
  for(i = qmi_si_list; i != NULL; i = i->next)
  {
    node = qmi_si_os_malloc(sizeof(qmi_si_list_entry_type));
    if(!node)
    {
      break;
    }
    QMI_MEM_COPY(node, sizeof(qmi_si_list_entry_type), i, 
                 sizeof(qmi_si_list_entry_type));
    node->next = ret;
    ret = node;
  }
  qmi_si_os_sem_unlock(&qmi_si_list_lock);
  return ret;
}

/*===========================================================================
  FUNCTION  qmi_si_free_list
===========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
  Frees a list provided by qmi_si_get*_impl functions

@param[in]   list     The list to free

@retval    None
*//* ------------------------------------------------------------------------*/
void qmi_si_free_list(qmi_si_list_entry_type *list)
{
  qmi_si_list_entry_type *i = NULL, *to_free = NULL;
  i = list;
  while(i)
  {
    to_free = i;
    i = i->next;
    qmi_si_os_free(to_free);
  }
}

