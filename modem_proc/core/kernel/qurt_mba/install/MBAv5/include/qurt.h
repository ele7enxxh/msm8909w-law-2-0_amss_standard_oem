#ifndef QURT_H
#define QURT_H 

/*======================================================================

                        qurt.h 

GENERAL DESCRIPTION
  Contain Kernel header files which provides Kernel OS API functions, constants, and 
  definitions 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
 by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt.h#1 $ 
 *	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2011-02-25 	op			Add Header file
 ======================================================================*/
 

#ifdef __cplusplus
extern "C" {
#endif

#include "qurt_consts.h"
#include "qurt_alloc.h"
#include "qurt_futex.h"
#include "qurt_mutex.h"
#include "qurt_pipe.h"
#include "qurt_printf.h"
#include "qurt_thread.h"
#include "qurt_trace.h"
#include "qurt_cycles.h"
#include "qurt_sem.h"
#include "qurt_cond.h"
#include "qurt_barrier.h"
#include "qurt_fastint.h"
#include "qurt_allsignal.h"
#include "qurt_anysignal.h"
#include "qurt_signal.h"
#include "qurt_rmutex.h"
#include "qurt_pimutex.h"
#include "qurt_int.h"
#include "qurt_lifo.h"
#include "qurt_power.h"
#include "qurt_event.h"
#include "qurt_pmu.h"
//#include "qurt_version.h"
#include "qurt_tlb.h"
#include "qurt_memory.h"
#include "qurt_qdi.h"
#include "qurt_space.h"
#include "qurt_shmem.h"
#include "qurt_timer.h"
#include "qurt_sclk.h"

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_H */

