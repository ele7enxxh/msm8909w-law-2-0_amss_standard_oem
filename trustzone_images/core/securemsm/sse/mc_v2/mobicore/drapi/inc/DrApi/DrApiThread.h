/** @addtogroup DRAPI_COM
 * @{
 *
 * @file DrApiThread.h
 * @brief  Secure Driver Thread and Interrupt API
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef __DRAPI_THREAD_H__
#define __DRAPI_THREAD_H__

#include "DrApiCommon.h"
#include "DrApiIpcMsg.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/** Definitions */
#define NILTASK             		0 			/**< returns NILTASK. */
#define NILTHREAD           		0 			/**< returns NILTHREAD. */

#define MAX_PRIORITY				(15U)		/**< maximum priority of a task or thread. */


#define TASK_THREAD_QUOTA_SHL       24
#define TASK_TIME_QUOTA_MASK	    ((1 << TASK_THREAD_QUOTA_SHL) - 1)  /**< mask to get/set time quota of a task. */
#define TIME_INFINITE				((time_t)TASK_TIME_QUOTA_MASK)		/**< returns infinite. */

/* Control ids for drApiThreadExRegs() API call */
#define THREAD_EX_REGS_IP       	(1U << 0)  	/**< currently set instruction pointer of the thread is replaced by the specified instruction pointer. */
#define THREAD_EX_REGS_SP       	(1U << 1)  	/**< currently set stack pointer of the thread is replaced by the specified stack pointer. */

//------------------------------------------------------------------------------
// Time data type
typedef word_t  time_t, *time_ptr;


//------------------------------------------------------------------------------
/**
 * Returns task ID for current task.
 *
 * @returns taskid
 * @returns 0 in case of any error
 */
taskid_t  drApiGetTaskid( void );

//------------------------------------------------------------------------------
/**
 * The function returns thread ID corresponding to task ID and thread number specified.
 *
 * @param taskid taskid having the thread.
 * @param threadno Thread number in task.
 *
 * @returns Thread ID in case of success
 * @returns 0 if task ID or thread number are invalid.
 */
threadid_t drApiTaskidGetThreadid(
    taskid_t taskid,
    threadno_t threadNo
);

//------------------------------------------------------------------------------
/**
 * The function returns thread ID for current task corresponding to thread number specified.
 *
 * @param threadNo Thread number in current task.
 *
 * @returns Thread ID in case of success
 * @returns 0 thread number is invalid.
 *
 */
threadid_t drApiGetLocalThreadid(
    threadno_t threadNo
);

//------------------------------------------------------------------------------
/** Suspend current thread.
 *
 * @param timeout Time to suspend thread.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiThreadSleep(
	time_t timeout
);

//------------------------------------------------------------------------------
/** Starts a thread in Driver.
 * @param threadNo Thread number in task.
 * @param threadEntry Thread entry function
 * @param stackPointer Thread top stack pointer (declared statically using DECLARE_STACK)
 * @param priority Thread priority (Maximum level is defined as MAX_PRIORITY, higher priority level corresponds to higher priority thread)
 * @param localExceptionHandler The parameter specifies the number of a thread that serves as an exception handler
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiStartThread(
    const threadno_t    threadNo,
    const addr_t        threadEntry,
    const stackTop_ptr  stackPointer,
    const uint32_t      priority,
    const threadno_t    localExceptionHandlerThreadNo
);


//------------------------------------------------------------------------------
/** Stop a thread in Driver.
 * If thread no. of thread to stop is set to NILTHREAD,
 * the current thread is stopped. The to stopped thread is detached from any
 * previously attached interrupts. If any thread is waiting for stopped thread
 * to do any ipc, this ipc is aborted.
 *
 * @param threadNo Thread number in task.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */

drApiResult_t drApiStopThread(
	const threadno_t    threadNo
);


//------------------------------------------------------------------------------
/** Resumes a thread in Driver.
 *
 * @param threadNo Thread number in task.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */

drApiResult_t drApiResumeThread(
    const threadno_t    threadNo
);


//------------------------------------------------------------------------------
/** Sets priority level for a thread in Driver.
 * @param threadNo Thread number in task.
 * @param priority Thread priority (Maximum level is defined as MAX_PRIORITY, higher priority level corresponds to higher priority thread)
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiSetThreadPriority(
    const threadno_t    threadNo,
    const uint32_t      priority
);

//------------------------------------------------------------------------------
/**
 * Sets the value registers for specified thread.
 * if the @ref THREAD_EX_REGS_IP bit of argument ctrl is set, the currently set instruction pointer is exchanged by the value of the argument ip (3).
 * if the @ref THREAD_EX_REGS_SP bit of argument ctrl is set, the currently set stack pointer is exchanged by the value of the argument sp (4).
 *
 * @param threadNo Number of the thread.
 * @param ctrl     Control flags
 * @param ip       ip value
 * @param sp       sp value
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiThreadExRegs(
    const threadno_t    threadNo,
    const uint32_t      ctrl,
    const addr_t        ip,
    const addr_t        sp
);

//------------------------------------------------------------------------------
/** Attaches an interrupt with the specified trigger condition to current thread.
 * Please refer to the target platform specific MobiCore documentation about
 * the trigger modes supported for each interrupt. In most cases, the mode
 * parameters will be INTR_MODE_RAISING_EDGE, as interrupts usually indicate
 * that a certain event has happened.
 *
 * @param intrNo interrupt number.
 * @param intrMode interrupt mode.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIntrAttach(
    intrNo_t intrNo,
    intrMode_t intrMode
);


//------------------------------------------------------------------------------
/** Detaches interrupt from current thread.
 *
 * @param intrNo interrupt number.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIntrDetach(
	intrNo_t intrNo
);

//------------------------------------------------------------------------------
/** Wait with infinite timeout for interrupt message from kernel.
 *
 * @param intrNo interrupt number (if ANYINTR is used, the interrupt is returned in the parameter intrRet).
 * @param timeout timeout to wait (allowed the same values as for MTK signal_wait()).
 * @param pIntrRet receives interrupt. Parameter can be NULL if caller does not need this.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiWaitForIntr(
    const intrNo_t  intrNo,
    const uint32_t  timeout,
    intrNo_t        *pIntrRet
);

//------------------------------------------------------------------------------
/**
 * Trigger software interrupt in the NWd to notify it.
 *
 * @param intrNo interrupt number.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiTriggerIntr(
    intrNo_t intrNo
);


#ifdef __cplusplus
}
#endif

#endif // __DRAPI_THREAD_H__
