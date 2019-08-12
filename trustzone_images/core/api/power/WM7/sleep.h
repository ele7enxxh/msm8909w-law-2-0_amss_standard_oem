#ifndef SLEEP_H
#define SLEEP_H
/*===========================================================================

              S L E E P    T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Task.
  This module is used in the portable only.

      Copyright (c) 1992-2009
                    by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/WM7/sleep.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/09   clm     Add extended sleep functions.
05/15/09   sep     Rename ioctl_qcom_cfg.h to ioctl_cfg.h
01/28/08   jcw     Port for WM7 from old sleep.h

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <windows.h>
#include "ioctl_cfg.h"

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

#include "sleep_types.h"

#define SLEEP_ASSERT_OKTS( sig )
#define SLEEP_NEGATE_OKTS( sig )


/*===========================================================================

FUNCTION SLEEP_REGISTER

DESCRIPTION
  Required at init time for any client that wants a unique vote for sleep.
  Pointer passed in must be unique, and the initial vote for sleep is
  indicated by the second parameter.

DEPENDENCIES
  None.

RETURN VALUE
  Sleep client handle, used for subsequent calls to assert and negate.

SIDE EFFECTS
  After this function completes, a new client record in the internal sleep 
  array structure is reserved for the calling client.

===========================================================================*/
__forceinline sleep_okts_handle sleep_register (const char * name,
                                                boolean      okts)
{
  tSleepRegister    sig;
  sleep_okts_handle okts_handle;

  memcpy (sig.name,
              name,
              CLIENT_NAME_LENGTH);
  sig.process_handle = GetCurrentProcess();
  sig.okts = okts;
    
  (void) KernelIoControl(QC_IOCTL_SLEEP_REGISTER,
                         &sig, sizeof (tSleepRegister),
                         &okts_handle, sizeof (UINT32),
                         NULL);

  return okts_handle;
}

/*===========================================================================

FUNCTION SLEEP_DEGREGISTER

DESCRIPTION
  Remove a client from the list of those currently registered

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

__forceinline void sleep_deregister (sleep_okts_handle handle)
{
  sleep_okts_handle sig = handle;
  (void) KernelIoControl(QC_IOCTL_SLEEP_DEREGISTER,
                         &sig, sizeof(sleep_okts_handle),
                         NULL, 0,
                         NULL);
}

/*===========================================================================

FUNCTION SLEEP_ASSERT_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is ok to sleep (tcxo off).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to TRUE.

===========================================================================*/
__forceinline void sleep_assert_okts (sleep_okts_handle handle)
{
  sleep_okts_handle sig = handle;
  (void) KernelIoControl(QC_IOCTL_ASSERT_OKTS,
                         &sig, sizeof(sleep_okts_handle),
                         NULL, 0,
                         NULL);
}

/*===========================================================================

FUNCTION SLEEP_NEGATE_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is not ok to sleep (tcxo on).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to FALSE.

===========================================================================*/
__forceinline void sleep_negate_okts (sleep_okts_handle handle)
{
  sleep_okts_handle sig = handle;
  (void) KernelIoControl(QC_IOCTL_NEGATE_OKTS,
                         &sig, sizeof(sleep_okts_handle),
                         NULL, 0,
                         NULL);
}

__forceinline void sleep_set_swfi_restriction
(
  sleep_okts_handle          handle,
  sleep_swfi_restrict_type   swfi_restriction
)
{
    tSleepSetSWFIRestriction sig;
    sig.handle = handle;
    sig.swfi_restriction = swfi_restriction;
    (void) KernelIoControl(QC_IOCTL_SLEEP_SET_SWFI_RESTRICTION,
                           &sig, sizeof(tSleepSetSWFIRestriction),
                           NULL, 0,
                           NULL);
    
}

/*===========================================================================

FUNCTION SLEEP_GET_ACQUIRED_RESOURCES

DESCRIPTION
  This function can be used to get a bitmask that represents all types of sleep
  that currently have someone voting against them.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
__forceinline uint32 sleep_get_acquired_resources( void )
{
  uint32 resources;
  (void) KernelIoControl(QC_IOCTL_SLEEP_GET_ACQUIRED_RESOURCES,
                         NULL, 0,
                         &resources, sizeof(uint32),
                         NULL);
  return resources;
}


/*===========================================================================

FUNCTION SLEEP_ACQUIRE_RESOURCES

DESCRIPTION
  This function changes state of internal sleep values, marking the resources
  passed in the second parameter as "in use."  Resources in use will never be
  transitioned to a low power state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some types of sleep may cease to occur.

===========================================================================*/
__forceinline void sleep_acquire_resources
(
  sleep_okts_handle handle,
  uint32            resources
)
{
  tSleepResourceCall  args;
  args.handle = handle;
  args.resources = resources;
      
  (void) KernelIoControl(QC_IOCTL_SLEEP_ACQUIRE_RESOURCES,
                         &args, sizeof (tSleepResourceCall),
                         NULL, 0,
                         NULL);
}

/*===========================================================================

FUNCTION SLEEP_RELEASE_RESOURCES

DESCRIPTION
  This function changes state of internal sleep values, marking the resources
  passed in the second parameter as not "in use."  Resources not in use may
  be transitioned to a low power state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some previously disabled types of sleep may start to occur.

===========================================================================*/
__forceinline void sleep_release_resources
(
  sleep_okts_handle handle,
  uint32            resources
)
{
  tSleepResourceCall  args;
  args.handle = handle;
  args.resources = resources;
      
  (void) KernelIoControl(QC_IOCTL_SLEEP_RELEASE_RESOURCES,
                         &args, sizeof (tSleepResourceCall),
                         NULL, 0,
                         NULL);
}

#endif /* SLEEP_H */

