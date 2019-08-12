/***********************************************************************
 * fs_mfse_task.c
 *
 * Service task that handles events from off-chip EFS clients.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Implements the service task that listens to events from  QCSI which get
 * signaled when client on the remote processor connects, disconnects or sends
 * request messages.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse_task.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_MFSE

#include "fs_mfse.h"
#include "fs_osal.h"
#include "fs_err.h"
#include "fs_task_i.h"
#include "assert.h"

/* QMI does not have a task of its own and requires that the processing of
   requests happen in a service task's context. */
void
fs_mfse_task (dword parm)
{
  fs_os_sigs_type task_sigs;

  (void) parm;

  fs_mfse_lock_init ();
  fs_mfse_lock ();

  /* Wait for start signal */
  FS_TASK_PRIV_START ();

  FS_MSG_MED ("FS_MFSE Service started", 0, 0, 0);

  fs_mfse_init ();

  fs_mfse_unlock ();

  /* Loop forever handling commands. */
  for (;;)
  {
    task_sigs = fs_os_wait (FS_MFSE_HANDLE_EVENT);

    ASSERT ((task_sigs & FS_MFSE_HANDLE_EVENT) != 0);
    FS_MSG_MED ("FS_MFSE_HANDLE_EVENT received", 0, 0, 0);
    (void) fs_os_clr_sigs (fs_os_self (), FS_MFSE_HANDLE_EVENT);
    fs_mfse_handle_event ();
  }
}

#endif /* FEATURE_EFS_MFSE */

