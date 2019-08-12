#include "core_server_platform.h"
#include "qmi_task_service.h"
/*---------------------------------------------------------------------------
   QMI_TASK_SERVICE.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_task_service.c

  @brief
    This file contains the implementation of task service APIs.

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

extern qmi_task_service_error_type
qmi_initialize_task(task_impl    *task_attr,
                    void         *priority,
                    void         *stk_size,
                    void         *sig);

extern qmi_task_service_error_type
qmi_create_task(char          *name,
                task_impl     *task_attr,
                void          *entry_func,
                void          *args);

extern qmi_task_service_error_type qmi_destroy_task(task_impl *task_attr);

extern qmi_task_service_error_type
qmi_set_os_params(task_impl         *task_attr,
				  qmi_csi_os_params *os_params);

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
                     void                         *sig)
{
    qmi_task_service_type *new_task;

    if (entry_point_func == NULL ) {
        *err = QMI_TASK_SERVICE_ENTRY_FUNC_NULL;
        return NULL;
    }

    new_task = ALLOCATOR(sizeof(qmi_task_service_type));

    if (new_task == NULL ) {
        *err = QMI_TASK_SERVICE_MEM_ERR;
        return NULL;
    }

     if ((strlen(name)+1) > TASK_NAME_MAX )
     {
         DEALLOCATE(new_task);
         /* Preventing buffer overflow */
         return NULL;
     }
        

    /* Initializing the task interface depending on the type of platform */
    new_task->task_started = 0;
    new_task->task_if.initialize_task =  qmi_initialize_task;
    new_task->task_if.create_task     =  qmi_create_task;
    new_task->task_if.destroy_task    =  qmi_destroy_task;
	new_task->task_if.set_os_params   =  qmi_set_os_params;

    strlcpy(new_task->task_name, name, CLASS_NAME_MAX);

    /*Initialize the task attributes */
    new_task->task_if.initialize_task(&(new_task->task),
                                      priority,
                                      stk_size,
                                      sig);

    new_task->entry_point_func =  entry_point_func;
    new_task->args = NULL;

    *err =  QMI_TASK_SERVICE_NO_ERR;

    return new_task;
}
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
qmi_task_service_delete(qmi_task_service_type* me)
{


    if  ( me == NULL ) {
        return QMI_TASK_INVALID_HANDLE;
    }

    DEALLOCATE(me);

    return QMI_TASK_SERVICE_NO_ERR;
}
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
qmi_task_service_create_task(qmi_task_service_type* me)
{

    qmi_task_service_error_type rc;

    if (me == NULL ) {
        return QMI_TASK_INVALID_HANDLE;
    }

    rc = me->task_if.create_task(me->task_name,
                                 &(me->task),
                                 me->entry_point_func,
                                 me->args);

    if(rc == QMI_TASK_SERVICE_NO_ERR)
    {
      me->task_started = 1;
    }

    return rc;
}
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
qmi_task_service_destroy_task(qmi_task_service_type* me)
{
    qmi_task_service_error_type rc;

    if (me == NULL ) {
        return QMI_TASK_INVALID_HANDLE;
    }

    rc = me->task_if.destroy_task(&(me->task));

    return rc;
}
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
qmi_task_set_os_params(qmi_task_service_type* me,
					   qmi_csi_os_params      *os_params)
{
	qmi_task_service_error_type rc;

	if (me == NULL)
		return QMI_TASK_INVALID_HANDLE;

	rc = me->task_if.set_os_params(&(me->task),
		                           os_params);
	return rc;
}

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
int qmi_task_service_has_started(qmi_task_service_type *task)
{
  return task ? task->task_started : 0;
}

