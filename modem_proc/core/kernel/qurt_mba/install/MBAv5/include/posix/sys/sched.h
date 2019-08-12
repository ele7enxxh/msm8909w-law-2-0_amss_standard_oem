#ifndef _POSIX_SCHED_H_
#define _POSIX_SCHED_H_

/*==========================================================================
 * FILE:         sched.c
 *
 * SERVICES:     POSIX Thread sched API interface
 *
 * DESCRIPTION:  POSIX Thread sched API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/posix/sys/sched.h#1 $
 *
 *  when       who     what, where, why
 *  --------   ---     -------------------------------------------------------
 *  03/17/09   cz      Initial version.
 *==========================================================================*/

#include <qurt.h>

#define SCHED_FIFO        0 /* First in, first out (FIFO) scheduling policy. */
#define SCHED_RR          1 /* Round robin scheduling policy. */
#define SCHED_SPORADIC    2 /* Sporadic server scheduling policy. */
#define SCHED_OTHER       3 /* Another scheduling policy. */

typedef struct sched_param   sched_param;
struct sched_param
{
    void *unimplemented;
    int  sched_priority;
};

/** \details 
 * This provides POSIX sched API. 
 */

/** \defgroup sched POSIX sched API */
/** \ingroup sched */
/** @{ */

/** Relinquish the CPU.
 * Please refer to POSIX standard for details.
 */
static inline int sched_yield(void)
{
   return 0;
}

/** Get the maximum priority.
 * Please refer to POSIX standard for details.
 * @param policy [in] SCHED_FIFO is the only valid input for this implementation.
 */
int sched_get_priority_max(int policy);

/** Get the minimum priority.
 * Please refer to POSIX standard for details.
 * @param policy [in] SCHED_FIFO is the only valid input for this implementation.
 */
int sched_get_priority_min(int policy);

/** @} */

#endif /* _POSIX_SCHED_H_ */
