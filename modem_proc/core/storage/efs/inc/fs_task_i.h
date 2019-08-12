/***********************************************************************
 * fs_task_i.h
 *
 * This file contains declarations that are internal to fs_task()
 *
 * Copyright (C) 2010-2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_task_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-03-19   dks   Clean up legacy dog usage, make fs_task not dog monitored.
2013-01-28   nr    Clean up references to tmc task.h.
2012-05-31   wek   Change the priority of benchmark two steps above sleep.
2012-04-30   nr    Register shutdown callback with RCINIT.
2012-02-29   rp    Make Tasks free-float (CPU affinity free)
2011-11-11   rp    Removed dependency on FEATURE_RCINIT.
2011-09-21   dks   Change feature deciding RCINIT or TMC task initialization.
2011-06-01   dks   Migrate from TMC to rcinit.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-04-28   vr    Create

===========================================================================*/

#ifndef __FS_TASK_I_H__
#define __FS_TASK_I_H__

#include "fs_task.h"
#include "task.h"
#include "fs_osal.h"
#include "fs_config_i.h"


/* REX Task Signals declarations. */

#define  FS_HEARTBEAT_TIMER_SIG    0x0001      /* Montiors health of FS task */

/* These are still used by EFS1. */
#define  FS_WRITE_TIMER_SIG        0x0002      /* Write timer signal */
#define  FS_CMD_Q_SIG              0x0004      /* Command queue signal */

#define FS_EFS_SYNC_SIG            0x0008      /* Sync timer signal */

#define FS_FTL_SYNC_SIG            0x0020      /* FTL sync timer signal */

/* Compatibility defines to map RCINIT functions.
 */
#define FS_TASK_PRIV_START()      rcinit_handshake_startup()
#define FS_TASK_REGISTER_SHUTDOWN(x)  \
                                 rcinit_register_termfn_group(RCINIT_GROUP_0,x)
#define FS_TASK_LOOKUP_PRIO(task)  rcinit_lookup_prio (#task)


/* If Benchmark priority is not defined, then set the FS benchmark task
   priority above sleep. */
#ifndef FS_BENCHMARK_PRI
  /* Get the priority of the "sleep" task. If the sleep task is not defined
     TASK_PRIO will return 0. To guarantee benchmark runs, set the benchmark
     priority two higher than the value returned. */
    #define FS_BENCHMARK_PRI   (FS_TASK_LOOKUP_PRIO(sleep) + 2)
#endif

/* If CPU Hog priority is not defined, then set the priority above FS
   benchmark task. */
#ifndef FS_CPU_HOG_PRI
  #define FS_CPU_HOG_PRI   (FS_BENCHMARK_PRI + 1)
#endif

#endif /* not __FS_TASK_I_H__ */
