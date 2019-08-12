/***********************************************************************
 * fs_global_lock.c
 *
 * FS_GLOBAL_LOCK critical section
 * Copyright (C) 2007-2013 QUALCOMM Technologies, Inc.
 *
 * The FS_GLOBAL_LOCK is used to protect the file system data from
 * concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_global_lock.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-28   nr    Wait for the rcinit handshake before initializing EFS.
2012-03-20   wek   Cleanup customer header file inclusion.
2011-03-08   dks   Add device setup calls.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-06-16   rp    Featurize the memory protection code for boot.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-04-01   ebb   Exposed memory protect/unprotect fns for format/chkfat.
2009-03-27   ebb   Removed #include fs_efs2.h
2009-03-23   ebb   Fix L4/Iguana dependencies.
2009-03-03   ebb   Added kernel protection domain extensions.
2008-12-15   yg    Do not include pd.h unless memory protection is defined.
2008-07-14   np    Added support for enabling l4 memory protection.
2007-12-27   yg    Added memory protection related header file
2007-12-05   rp    Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-11-16   sh    Rename TIMETEST2 calls to Babylon calls
2007-10-26   rp    Corrected featurization of FS_PARANOIA_CHECKS.
2007-08-19   rp    Create

===========================================================================*/

#include "fs_global_lock.h"
#include "fs_osal.h"
#include "fs_babylon.h"
#include "fs_device.h"
#include "fs_config_i.h"
#include <stdio.h>

#ifdef FEATURE_DAC_MEMORY_PROTECTION
  #ifndef HAVE_DAC_MEMORY_PROTECTION
    #error "Rex version mismatch with EFS"
  #endif /* HAVE_DAC_MEMORY_PROTECTION */

  #include "armmmu.h"
#endif /* FEATURE_DAC_MEMORY_PROTECTION */

#ifdef FS_STANDALONE
#include "ut_utils.h"
#endif

#if defined(FEATURE_L4) && \
    defined(FEATURE_ENABLE_EFS_MEMORY_PROTECTION)
  #include <iguana/env.h>
  #include <iguana/pd.h>
  #include <iguana/pd_extension.h>
  #ifdef HAVE_L4_PD_EXTENSION_MULTIPLE
    pd_extension_token_t *token;
  #endif
#endif /* FEATURE_L4 && FEATURE_ENABLE_EFS_MEMORY_PROTECTION*/

/* FS_GLOBAL_LOCK critical section definition memory allocation */
static fs_os_crit_sect_type fs_crit_sect;

/* Global variable to ensure only one global lock init occurs for every
 * phone boot. */
uint8 fs_global_lock_init_complete = 0;

void
fs_global_lock_init(void)
{
  /* If we already finished initialization we can just return. */
  if (fs_global_lock_init_complete != 0)
  {
    return;
  }

  /* Initialize token if we are running on L4 */
#if (defined(HAVE_L4_PD_EXTENSION_MULTIPLE) && \
     defined(FEATURE_ENABLE_EFS_MEMORY_PROTECTION))

  char seg_name[16];
  snprintf (seg_name, sizeof (seg_name), "%d_PD_EXT",
            env_const (iguana_getenv ("EFS_PROTECTED_SEGMENT")));
  token = env_pd_extension_token(iguana_getenv(seg_name));
#endif

  fs_os_init_crit_sect (&fs_crit_sect);

  fs_device_setup_init ();

  /* Init is now complete so set the static flag */
  fs_global_lock_init_complete = 1;
}

/* This function replaces the FS_GLOBAL_LOCK macro */
void
fs_global_lock (void)
{
  /* Enter critical section and Unlock the EFS RW and ZI */
  /* In the current implementation we take the global lock then call the
   * kernel memory protection domain API to unlock the protected EFS
   * segment. On some versions only single threaded access will be allowed
   * (HAVE_L4_PD_EXTENSION_ACTIVATE and prior) but on later
   * version (HAVE_L4_PD_EXTENSION_MULTIPLE and later) multiple threads can
   * operate concurrently in the protected segment. Currently we support
   * all kernel APIs although we only allow a single thread to operate at a
   * time due to our lack of multi-threaded support.
   */
  fs_os_enter_crit_sect (&fs_crit_sect);
  fs_efs2_unprotect_memory ();

#ifdef FS_STANDALONE
/* Call simulator specific global lock */
  ut_fs_global_lock ();
#endif

  fs_device_setup_start ();

  EFS_BAB_START (EFS_BAB_GLOBAL_LOCK); /* TIMETEST2 marker */
}

/* Freeze data structure hash */
void
fs_global_unlock (void)
{
  fs_device_setup_done ();

  fs_os_leave_crit_sect (&fs_crit_sect);

  /* Lock the EFS critical RW and ZI */
  fs_efs2_protect_memory ();

#ifdef FS_STANDALONE
  /* Call simulator specific global unlock */
  ut_fs_global_unlock ();
#endif

  EFS_BAB_END (EFS_BAB_GLOBAL_LOCK); /* TIMETEST2 marker */
}

#ifdef FEATURE_ENABLE_EFS_MEMORY_PROTECTION

/* Lock the EFS RW and ZI memory block for any read or write access. */
void
fs_efs2_protect_memory (void)
{
#ifndef FEATURE_EFS_BOOT_ENV
  fs_os_tcb_type *task_tcb;
  task_tcb = fs_os_self ();

  /* Lock down the EFS WR and ZI section */
  if (task_tcb != fs_crit_sect.owner)
  {
    /* rex_set_dac interface */
#if defined(FEATURE_DAC_MEMORY_PROTECTION)
    fs_os_set_dac (task_tcb, MMU_PROTECT_EFS_MEMORY);

    /* L4 memory extensions - version 1 */
#elif defined(HAVE_L4_PD_EXTENSION_ACTIVATE)
    pd_extension_deactivate(NULL);

    /* L4 memory extensions - version 2 */
#elif defined(HAVE_L4_PD_EXTENSION_MULTIPLE)
    pd_extension_exit (token);

#else
  #error "EFS memory protection defined w/o OS interface"
#endif
  }
  /* ELSE - There is a nesting of entering critical sections.. Just skip */
#endif /* FEATURE_EFS_BOOT_ENV */
}

/* UnLock the EFS RW and ZI memory block for all read or write access. */
void
fs_efs2_unprotect_memory (void)
{
#ifndef FEATURE_EFS_BOOT_ENV
  /* rex_set_dac interface */
#if defined(FEATURE_DAC_MEMORY_PROTECTION)
  fs_os_set_dac (task_tcb, MMU_PROTECT_EFS_MEMORY);
  fs_os_tcb_type *task_tcb;

  task_tcb = fs_os_self ();
  fs_os_set_dac (task_tcb, MMU_UNPROTECT_EFS_MEMORY);

  /* L4 memory extensions - version 1 */
#elif defined(HAVE_L4_PD_EXTENSION_ACTIVATE)
  pd_extension_activate(NULL);

  /* L4 memory extensions - version 2 */
#elif defined(HAVE_L4_PD_EXTENSION_MULTIPLE)
  pd_extension_enter (token);

#else
  #error "EFS memory protection defined w/o OS interface"
#endif
#endif /* FEATURE_EFS_BOOT_ENV */
}

#else /* Memory protection not enabled */

/* If memory protection is not available, the protect and unprotect functions
 * become no-op.
 */
void
fs_efs2_protect_memory (void)
{
  return;
}

void
fs_efs2_unprotect_memory (void)
{
  return;
}

#endif
