#ifndef __TASK_H__
#define __TASK_H__

/*
  Copyright (C) 2010, 2013, 2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/patch/task.h#1 $
  $Author: mplcsds1 $
*/

#include "EliteThread_Priority.h"

#define CVS_IST_PRI_ORDER ( ELITETHREAD_CVS_PRIO )
#define CVP_TASK_PRI_ORDER ( ELITETHREAD_CVP_PRIO )
#define MVM_TASK_PRI_ORDER ( ELITETHREAD_MVM_PRIO )
#define CVD_VFR_TASK_PRI_ORDER ( ELITETHREAD_MVM_PRIO )
#define CVD_CAL_LOGGER_TASK_PRI_ORDER ( ELITETHREAD_MVM_PRIO - 1 )

  /**<
   * Note that CVD VFR task runs at the same priority as MVM task. CVD VFR
   * task runs during voice call setup/teardown and it has the same latency
   * requirement as other MVM call setupt/teardown commands. CVD VFR task shall
   * have lower priority than CVS/CVP in order to avoid having this task
   * preempting vocoder packet exchange or VPCM packet exchange during
   * concurrent voice use cases.
   */

#define CVS_MAILBOX_TIMER_TASK_PRI_ORDER ( ELITETHREAD_CVS_PRIO + 1 )
  /**<
    * The mailbox timer thread is not a service thread. It is created only upon
    * the first time when mailbox packet exchange is configured. The thread is
    * used to receive AVTimer signal and trigger CVS to dequeue the decoder
    * packet on behalf of the decoder before the decoder becomes active. The
    * thread becomes inactive as soon as the decoder becomes active. The thread
    * is destroyed (if it was created) upon cvs_deinit. The thread's priority
    * is immediately above CVS since CVS depends on this thread.
    */

#define VOCSVC_TASK_PRI_ORDER ( ELITETHREAD_VOCSVC_PRIO )

#define TASK_PRIORITY( task_pri_macro ) ( 255 - task_pri_macro )
  /**<
   * Note that Elite defines 255 as the highest thread priority and 0 as the
   * lowest thread priority. Whereas QuRT defines 0 as the highest thread 
   * priority. Therefore we must use ( 255 - Elite thread priority ) to 
   * determine the QuRT thread priority. The QuRT priority is being used when 
   * APR thread OSAL creates a thread using QuRT on the ADSP. CVS, CVP, MVM and
   * VOCSVC all use APR thread OSAL to create its thread.
   */

#endif /* __TASK_H__ */

