/***********************************************************************
 * fs_async_put_task.c
 *
 * Task to handle asynchronous write requests to EFS2 Item files.
 * Copyright (C) 2011-2014 QUALCOMM Technologies, Inc.
 *
 * Implementation of task which handles requests for writing to EFS2 Item files
 * asynchronously without blocking caller until write is done. The file handles
 * cmd signal and calls APIs defined in fs_async_put.c to process the requests.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_async_put/src/fs_async_put_task.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-06-27   wek   Initialize global tcb variable to signal into async init.
2013-01-28   nr    Clean up references to tmc task.h.
2012-01-04   dks   Incorporate code review comments : remove clr signal
2011-11-08   dks   Create

===========================================================================*/

#include "fs_async_put_i.h"
#include "fs_task_i.h"
#include "fs_osal.h"
#include "fs_err.h"

void
fs_async_put_task (dword parm)
{
  fs_os_sigs_type task_sigs;

  (void) parm;

  /* Initialize the async_put processing module. */
  fs_async_put_init ();

  /* Wait for start signal */
  FS_TASK_PRIV_START ();

  FS_MSG_MED_0 ("FS_ASYNC_PUT task started");

  /* Loop forever handling commands. */
  for (;;)
  {
    task_sigs = fs_os_wait (FS_ASYNC_PUT_CMD_Q_SIG);

    if ((task_sigs & FS_ASYNC_PUT_CMD_Q_SIG) != 0)
    {
      FS_MSG_MED_0 ("FS_ASYNC_PUT : New request received");
      (void) fs_os_clr_sigs (fs_os_self (), FS_ASYNC_PUT_CMD_Q_SIG);
      fs_async_put_process_commands ();
    }
  }
}
