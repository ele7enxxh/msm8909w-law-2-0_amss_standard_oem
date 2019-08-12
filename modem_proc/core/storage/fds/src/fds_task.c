/****************************************************************************
 * fds.c
 *
 * Flash Driver Service Task.
 *
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Service task that handles events for the Flash Driver Service Commands.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_task.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2014-04-28   vm  Use correct MSG macros to save RAM.
2013-10-03  dks  Add commands for Fota Update and Event registration.
2013-07-17  dks  Create.

===========================================================================*/

#include "rex.h"
#include "rcinit.h"
#include "assert.h"
#include "fds_err.h"
#include "fds_i.h"

void
fds_task (dword parm)
{
  rex_tcb_type *tcb;
  rex_sigs_type task_sigs;

  (void) parm;

  tcb = rex_self ();

  fds_lock_init ();
  fds_lock ();

  /* Wait for start signal */
  rcinit_handshake_startup();

  FDS_MSG_MED_0 ("FDS Task Started");

  fds_init ();

  fds_unlock ();

  /* Loop forever handling commands. */
  for (;;)
  {
    task_sigs = rex_wait (FDS_HANDLE_EVENT);

    ASSERT ((task_sigs & FDS_HANDLE_EVENT) != 0);
    FDS_MSG_MED_0 ("FDS_HANDLE_EVENT received");
    (void) rex_clr_sigs (tcb, FDS_HANDLE_EVENT);
    fds_handle_event ();
  }
}

