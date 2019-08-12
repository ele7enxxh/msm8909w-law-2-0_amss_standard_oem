/***********************************************************************
 * fs_qmi_service_task.c
 *
 * Service task to listen to remote efs_put and efs_get events from client on
 * apps processor.
 * Copyright (C) 2011, 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Implements the service task that listens to events from the
 * QMI Common Service Interface(QCSI) which get signaled when client on the
 * remote processor connects, disconnects or sends request messages.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_qmi_put_get/service/src/fs_qmi_service_task.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-03-19   dks   Clean up legacy dog usage, make fs_task not dog monitored.
2013-01-28   nr    Clean up references to tmc task.h.
2011-10-04   dks   Create.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QMI_PUT_GET_SERVICE

#include "fs_qmi_service.h"
#include "fs_osal.h"
#include "fs_err.h"
#include "fs_task_i.h"
#include "assert.h"
#include <string.h>

/* Service task that handles the process request signals from QCSI. QMI does
   not have a task of its own and requires that the processing of requests
   happen in a service task's context. */
void
fs_qmi_service_task (dword parm)
{
  fs_os_sigs_type task_sigs;

  (void) parm;

  /* Initialize the service. */
  (void) fs_qmi_service_init ();

  /* Wait for start signal */
  FS_TASK_PRIV_START ();

  FS_MSG_MED_0 ("efs_qmi_put_get service task started");

  /* Loop forever handling commands. */
  for (;;)
  {
    task_sigs = fs_os_wait (FS_QMI_EFS_SERVICE_HANDLE_EVENT);

    if ((task_sigs & FS_QMI_EFS_SERVICE_HANDLE_EVENT) != 0)
    {
      FS_MSG_MED_0 ("FS_QMI_EFS_SERVICE_HANDLE_EVENT received");
      (void) fs_os_clr_sigs (fs_os_self (), FS_QMI_EFS_SERVICE_HANDLE_EVENT);
      (void) fs_qmi_service_handle_event ();
    }
  }
}

#endif /* FEATURE_EFS_QMI_PUT_GET_SERVICE */
