#ifndef _QMI_TASK_SERVICE_H_
#define _QMI_TASK_SERVICE_H_
/*---------------------------------------------------------------------------
   QMI_TASK_SERVICE.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_task_service.h

  @brief
    This file contains the APIs for task service.

  ---------------------------------------------------------------------------
  Copyright (c) 2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
#include "stdint.h"
#include "core_server_platform.h"
#include "qmi_core_error.h"
#include "qmi_csi.h"

typedef enum qmi_task_service_error
{
    QMI_TASK_SERVICE_NO_ERR = 0,
    QMI_TASK_INVALID_HANDLE = QMI_CORE_TASK_ERROR,
    QMI_TASK_SERVICE_ENTRY_FUNC_NULL,
    QMI_TASK_SERVICE_MEM_ERR,
    QMI_TASK_SERVICE_CREATION_ERR
}qmi_task_service_error_type;


typedef qmi_task_service_error_type (*qmi_set_os_params_fn_type)
                                    (task_impl         *task_attr,
				                     qmi_csi_os_params *os_params);


typedef qmi_task_service_error_type (*qmi_initialize_fn_type)
                                    (task_impl    *task_attr,
                                     void         *priority,
                                     void         *stk_size,
                                     void         *sig);

typedef qmi_task_service_error_type (*qmi_create_task_fn_type)
                                    (char          *name,
                                     task_impl     *task_attr,
                                     void          *entry_func,
                                     void          *args);

typedef qmi_task_service_error_type (*qmi_destroy_task_fn_type)
                                    (task_impl     *task_attr);



typedef struct qmi_task_interface {
    qmi_create_task_fn_type          create_task;
    qmi_initialize_fn_type           initialize_task;
    qmi_destroy_task_fn_type         destroy_task;
	qmi_set_os_params_fn_type        set_os_params;
}qmi_task_interface_type;



typedef struct task_service {
    task_impl                task;
    char                     task_name[TASK_NAME_MAX];
    void                     *entry_point_func;
    void                     *args;
    qmi_task_interface_type  task_if;
    int                      task_started;
    /* Might need a timer interface */
}qmi_task_service_type;


/* Public Interface */
/*===========================================================================
  FUNCTION  qmi_task_service_new
===========================================================================*/
/*!
@brief
  This function creates a task object.

@param[out]  err         Pointer to Error types defined in 
                         qmi_task_service_error_type.
@param[in]   name        Name that will be assigned to the task.
@param[in]   entry_func  Function that will be called when the task/thread
                         is created.
@param[in]   priority    Priority of the task/thread.
@param[in]   stk_size    Stack size used to create the task.
@param[in]   sig         Signal specfic to task.

@retval    Returns a task object on Success, NULL otherwise with approriate
           error code set in err argument.
*/
/*=========================================================================*/
qmi_task_service_type *
qmi_task_service_new(qmi_task_service_error_type  *err,
                     char                         *name,
                     void                         *entry_point_func,
                     void                         *priority,
                     void                         *stk_size,
                     void                         *sig);
                    
/*===========================================================================
  FUNCTION  qmi_task_service_delete
===========================================================================*/
/*!
@brief
  This function destroys a task object.

@param[in]  task    Pointer to task object.

@retval  Returns QMI_TASK_SERVICE_NO_ERR on success.
@retval  Returns  QMI_TASK...... defined in qmi_task_service_error_type
*/
/*=========================================================================*/

qmi_task_service_error_type
qmi_task_service_delete(qmi_task_service_type* task);

/*===========================================================================
  FUNCTION  qmi_task_service_create_task
===========================================================================*/
/*!
@brief
  This function starts a task/thread.

@param[in]  task    Pointer to task object.

@retval  Returns QMI_TASK_SERVICE_NO_ERR on success.
@retval  Returns  QMI_TASK...... defined in qmi_task_service_error_type
*/
/*=========================================================================*/
qmi_task_service_error_type
qmi_task_service_create_task(qmi_task_service_type* task);

/*===========================================================================
  FUNCTION  qmi_task_service_destroy_task
===========================================================================*/
/*!
@brief
  This function stops a task/thread.

@param[in]  task    Pointer to task object.

@retval  Returns QMI_TASK_SERVICE_NO_ERR on success.
@retval  Returns  QMI_TASK...... defined in qmi_task_service_error_type
*/
/*=========================================================================*/
qmi_task_service_error_type
qmi_task_service_destroy_task(qmi_task_service_type* task);

/*===========================================================================
  FUNCTION  qmi_task_set_os_params
===========================================================================*/
/*!
@brief
  This function copies the signaling information from the task object
  to the os_params provided by the user.
  
@param[in]  task        Pointer to task object.
@param[out] os_params   Pointer to OS specific signaling information.

@retval  Returns QMI_TASK_SERVICE_NO_ERR on success.
@retval  Returns  QMI_TASK...... defined in qmi_task_service_error_type
*/
/*=========================================================================*/
qmi_task_service_error_type
qmi_task_set_os_params(qmi_task_service_type* task,
					   qmi_csi_os_params      *os_params);

/*===========================================================================
  FUNCTION  qmi_task_service_has_started
===========================================================================*/
/*!
@brief
  This function returns a bool based on whether a task has started or not.
  
@param[in]  task        Pointer to task object.

@retval  Returns 0 if the task has not been started
@retval  Returns 1 if the task has been started
*/
/*=========================================================================*/
int 
qmi_task_service_has_started(qmi_task_service_type *task);

#endif
