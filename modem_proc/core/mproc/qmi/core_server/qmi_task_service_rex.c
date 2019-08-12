#include "qmi_task_service.h"
/*---------------------------------------------------------------------------
   QMI_TASK_SERVICE_REX.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_task_service_rex.c

  @brief
    This file contains the rex based implementation of task service APIs.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/



#define QMI_SERVICE_STACK_SIZE  (2048)
#define QMI_DEFAULT_PRIORITY 13;

/*===========================================================================
  FUNCTION  qmi_set_os_params
===========================================================================*/
qmi_task_service_error_type
qmi_set_os_params(task_impl         *task_attr,
          qmi_csi_os_params *os_params)
{
  if (os_params == NULL )
    return QMI_TASK_INVALID_HANDLE;

  os_params->tcb = &task_attr->rex_tcb;
  os_params->sig = task_attr->rex_sigs;

  return QMI_TASK_SERVICE_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_initialize_task
===========================================================================*/
qmi_task_service_error_type
qmi_initialize_task(task_impl    *task_attr,
                    void         *priority,
                    void         *stk_size,
                    void         *sig)
{
    rex_sigs_type *rex_sig;
    rex_stack_word_type *rex_stk_size;
    rex_priority_type *rex_priority;

    /* Populating with default values */
    if (!sig)
    {
        return QMI_TASK_SERVICE_CREATION_ERR;
    }
    else
    {
        rex_sig  = (rex_sigs_type*)sig;
        task_attr->rex_sigs = *rex_sig;
    }

    if (!stk_size)
    {
        task_attr->stk_size = (rex_stack_word_type)QMI_SERVICE_STACK_SIZE;
    }
    else
    {
        rex_stk_size = (rex_stack_word_type*)stk_size;
        task_attr->stk_size = *rex_stk_size;
    }
        
    if (!priority)
    {
        task_attr->rex_prio = QMI_DEFAULT_PRIORITY;
    }
    else
    {
        rex_priority = (rex_priority_type*)priority;
        task_attr->rex_prio = *rex_priority;
    }
        
    return QMI_TASK_SERVICE_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_create_task
===========================================================================*/
qmi_task_service_error_type
qmi_create_task(char          *name,
                task_impl     *task_attr,
                void          *entry_func,
                void          *args)
{

    task_attr->rex_stk = ALLOCATOR(sizeof(rex_stack_word_type) * task_attr->stk_size);
    if  ( task_attr->rex_stk == NULL ) {
       return QMI_TASK_SERVICE_MEM_ERR;
    }

    rex_def_task_ext(&task_attr->rex_tcb,
                 (rex_stack_pointer_type)task_attr->rex_stk,
                 task_attr->stk_size,
                 task_attr->rex_prio,
                 (rex_task_func_type)entry_func, 
                 (dword)args, name, FALSE);

     return QMI_TASK_SERVICE_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_destroy_task
===========================================================================*/
qmi_task_service_error_type
qmi_destroy_task(task_impl *task_attr)
{

    return QMI_TASK_SERVICE_NO_ERR;

}


