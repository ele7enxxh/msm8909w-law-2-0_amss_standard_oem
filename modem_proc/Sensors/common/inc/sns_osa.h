#ifndef _SNS_OSA_H_
#define _SNS_OSA_H_
/*============================================================================
  @file sns_osa.h

  @brief
  OS Abstraction layer for sensors.

  This is a common header file; however, each processor & target OS will have
  a specific implementation.

  <br><br>

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/inc/sns_osa.h#1 $
  $DateTime: 2016/12/13 08:00:23 $
  $Author: mplcsds1 $

  when       who    what, where, why
  ---------- --- ----------------------------------------------------------
  2015-10-02 hw  Remove sns_os_task_create which is not usable anymore
  2015-08-26 gju Remove PCSIM compile flag
  2014-09-17 ps  Remove AVS component dependency
  2014-06-05 vy  Added sns_os_mutex_create_uimg to support uImage
  2014-04-23 pn  Changed sns_os_sigs_del() signature
  2013-10-03 sc  On behalf of Amithash from QMI team: use native qurt signal
                 instead of qurt_elite channel/signal structure.
  2012-07-06 sc  Added function prototype sns_os_set_qmi_csi_params()
  2011-08-23 sc  Stub out OS_CPU_INT_LOCK() on PCSIM.
  2011-04-18 jh  Updated include file
  2011-04-13 dc  Include includes/h for SNS_BLAST
  2011-02-24 br  Inserted sns_os_time_dly() and changed sns_os_task_create_ext()
  2010-10-15 sd  Added sns_os_sigs_accept
  2010-08-26 jtl Added edit history. Removed ifdefs from API.
                 Removed unused functions.
  ============================================================================*/

/*=====================================================================
  INCLUDES
  =======================================================================*/
#if defined (SNS_LA)
#include <includes.h>
#endif
#include "sensor1.h"
#include "sns_debug_api.h"

#if defined(QDSP6) || defined(FAST_PLAYBACK)
/* System */
#include "qurt.h"
#include "qurt_atomic_ops.h"
#include "qmi_csi_target_ext.h"
#endif /* QDSP6 */


/*=====================================================================
  INTERNAL DEFINITIONS AND TYPES
  =======================================================================*/
#if defined( QDSP6) || defined(FAST_PLAYBACK)
/* Critical Section */
#define SNS_OS_ENTER_CRITICAL()
#define SNS_OS_EXIT_CRITICAL()
typedef qurt_sem_t     OS_SEM;
#else
/* Critical Section */
#define SNS_OS_ENTER_CRITICAL() OS_ENTER_CRITICAL()
#define SNS_OS_EXIT_CRITICAL() OS_EXIT_CRITICAL()
#define OS_SEM int
#endif /* QDSP6 */

#if defined(QDSP6)
#define OS_CPU_INT_LOCK()
#endif

// script setting from os_qcom_port.h, for now it is set to 10
#define OS_MAX_DAL_RESERVED_PRIO 10

// copying the struct and definitions from ucos_ii.h, os_cpu.h, os_cfg.h
// to keep the sns_osa layer unchanged. only the implementation will be changed
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */


//#define OS_STK                  unsigned int
#define OS_FLAGS                unsigned int
#define OS_PRIO                 unsigned int
#define OS_LOWEST_PRIO          255 // blast supports up to 255 priority, 255 being lowest

/*
*********************************************************************************************************
                          Extern variables 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#ifndef  OS_FALSE
#define  OS_FALSE                       0u
#endif

#ifndef  OS_TRUE
#define  OS_TRUE                        1u
#endif

#define  OS_ASCII_NUL            (INT8U)0

#define  OS_PRIO_SELF                0xFFu              /* Indicate SELF priority                      */

#if OS_TASK_STAT_EN > 0u
#define  OS_N_SYS_TASKS                 2u              /* Number of system tasks                      */
#else
#define  OS_N_SYS_TASKS                 1u
#endif

#define  OS_TASK_STAT_PRIO  (OS_LOWEST_PRIO - 1u)       /* Statistic task priority                     */
#define  OS_TASK_IDLE_PRIO  (OS_LOWEST_PRIO)            /* IDLE      task priority                     */


#define  OS_TASK_IDLE_ID            65535u              /* ID numbers for Idle, Stat and Timer tasks   */
#define  OS_TASK_STAT_ID            65534u
#define  OS_TASK_TMR_ID             65533u

#define  OS_EVENT_EN           (((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u) || (OS_SEM_EN > 0u) || (OS_MUTEX_EN > 0u))

#define  OS_TCB_RESERVED        ((OS_TCB *)1)

/*$PAGE*/
/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
#define  OS_STAT_RDY                 0x00u  /* Ready to run                                            */
#define  OS_STAT_SEM                 0x01u  /* Pending on semaphore                                    */
#define  OS_STAT_MBOX                0x02u  /* Pending on mailbox                                      */
#define  OS_STAT_Q                   0x04u  /* Pending on queue                                        */
#define  OS_STAT_SUSPEND             0x08u  /* Task is suspended                                       */
#define  OS_STAT_MUTEX               0x10u  /* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_FLAG                0x20u  /* Pending on event flag group                             */
#define  OS_STAT_MULTI               0x80u  /* Pending on multiple events                              */

#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)

/*
*********************************************************************************************************
*                           TASK PEND STATUS (Status codes for OSTCBStatPend)
*********************************************************************************************************
*/
#define  OS_STAT_PEND_OK                0u  /* Pending status OK, not pending, or pending complete     */
#define  OS_STAT_PEND_TO                1u  /* Pending timed out                                       */
#define  OS_STAT_PEND_ABORT             2u  /* Pending aborted                                         */

/*
*********************************************************************************************************
*                                        OS_EVENT types
*********************************************************************************************************
*/
#define  OS_EVENT_TYPE_UNUSED           0u
#define  OS_EVENT_TYPE_MBOX             1u
#define  OS_EVENT_TYPE_Q                2u
#define  OS_EVENT_TYPE_SEM              3u
#define  OS_EVENT_TYPE_MUTEX            4u
#define  OS_EVENT_TYPE_FLAG             5u

#define  OS_TMR_TYPE                  100u  /* Used to identify Timers ...                             */
                                            /* ... (Must be different value than OS_EVENT_TYPE_xxx)    */

/*
*********************************************************************************************************
*                                         EVENT FLAGS
*********************************************************************************************************
*/
#define  OS_FLAG_WAIT_CLR_ALL           0u  /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_AND           0u

#define  OS_FLAG_WAIT_CLR_ANY           1u  /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_OR            1u

#define  OS_FLAG_WAIT_SET_ALL           2u  /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_AND           2u

#define  OS_FLAG_WAIT_SET_ANY           3u  /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_OR            3u


#define  OS_FLAG_CONSUME             0x80u  /* Consume the flags if condition(s) satisfied             */


#define  OS_FLAG_CLR                    0u
#define  OS_FLAG_SET                    1u

/*
*********************************************************************************************************
*                                   Values for OSTickStepState
*
* Note(s): This feature is used by uC/OS-View.
*********************************************************************************************************
*/

#if OS_TICK_STEP_EN > 0u
#define  OS_TICK_STEP_DIS               0u  /* Stepping is disabled, tick runs as mormal               */
#define  OS_TICK_STEP_WAIT              1u  /* Waiting for uC/OS-View to set OSTickStepState to _ONCE  */
#define  OS_TICK_STEP_ONCE              2u  /* Process tick once and wait for next cmd from uC/OS-View */
#endif

/*
*********************************************************************************************************
*       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
*********************************************************************************************************
*/
#define  OS_DEL_NO_PEND                 0u
#define  OS_DEL_ALWAYS                  1u

/*
*********************************************************************************************************
*                                        OS???Pend() OPTIONS
*
* These #defines are used to establish the options for OS???PendAbort().
*********************************************************************************************************
*/
#define  OS_PEND_OPT_NONE               0u  /* NO option selected                                      */
#define  OS_PEND_OPT_BROADCAST          1u  /* Broadcast action to ALL tasks waiting                   */

/*
*********************************************************************************************************
*                                     OS???PostOpt() OPTIONS
*
* These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
*********************************************************************************************************
*/
#define  OS_POST_OPT_NONE            0x00u  /* NO option selected                                      */
#define  OS_POST_OPT_BROADCAST       0x01u  /* Broadcast message to ALL tasks waiting                  */
#define  OS_POST_OPT_FRONT           0x02u  /* Post to highest priority task waiting                   */
#define  OS_POST_OPT_NO_SCHED        0x04u  /* Do not call the scheduler if this option is selected    */

/*
*********************************************************************************************************
*                                 TASK OPTIONS (see OSTaskCreateExt())
*********************************************************************************************************
*/
#define  OS_TASK_OPT_NONE          0x0000u  /* NO option selected                                      */
#define  OS_TASK_OPT_STK_CHK       0x0001u  /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR       0x0002u  /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP       0x0004u  /* Save the contents of any floating-point registers       */
#define  OS_TASK_OPT_ISLAND        0x0008u  /* Task expected to run in contrained resources            */

/*
*********************************************************************************************************
*                            TIMER OPTIONS (see OSTmrStart() and OSTmrStop())
*********************************************************************************************************
*/
#define  OS_TMR_OPT_NONE                0u  /* No option selected                                      */

#define  OS_TMR_OPT_ONE_SHOT            1u  /* Timer will not automatically restart when it expires    */
#define  OS_TMR_OPT_PERIODIC            2u  /* Timer will     automatically restart when it expires    */

#define  OS_TMR_OPT_CALLBACK            3u  /* OSTmrStop() option to call 'callback' w/ timer arg.     */
#define  OS_TMR_OPT_CALLBACK_ARG        4u  /* OSTmrStop() option to call 'callback' w/ new   arg.     */

/*
*********************************************************************************************************
*                                            TIMER STATES
*********************************************************************************************************
*/
#define  OS_TMR_STATE_UNUSED            0u
#define  OS_TMR_STATE_STOPPED           1u
#define  OS_TMR_STATE_COMPLETED         2u
#define  OS_TMR_STATE_RUNNING           3u

/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define OS_ERR_NONE                     0u

#define OS_ERR_EVENT_TYPE               1u
#define OS_ERR_PEND_ISR                 2u
#define OS_ERR_POST_NULL_PTR            3u
#define OS_ERR_PEVENT_NULL              4u
#define OS_ERR_POST_ISR                 5u
#define OS_ERR_QUERY_ISR                6u
#define OS_ERR_INVALID_OPT              7u
#define OS_ERR_ID_INVALID               8u
#define OS_ERR_PDATA_NULL               9u

#define OS_ERR_TIMEOUT                 10u
#define OS_ERR_EVENT_NAME_TOO_LONG     11u
#define OS_ERR_PNAME_NULL              12u
#define OS_ERR_PEND_LOCKED             13u
#define OS_ERR_PEND_ABORT              14u
#define OS_ERR_DEL_ISR                 15u
#define OS_ERR_CREATE_ISR              16u
#define OS_ERR_NAME_GET_ISR            17u
#define OS_ERR_NAME_SET_ISR            18u
#define OS_ERR_ILLEGAL_CREATE_RUN_TIME 19u

#define OS_ERR_MBOX_FULL               20u

#define OS_ERR_Q_FULL                  30u
#define OS_ERR_Q_EMPTY                 31u

#define OS_ERR_PRIO_EXIST              40u
#define OS_ERR_PRIO                    41u
#define OS_ERR_PRIO_INVALID            42u

#define OS_ERR_SCHED_LOCKED            50u
#define OS_ERR_SEM_OVF                 51u

#define OS_ERR_TASK_CREATE_ISR         60u
#define OS_ERR_TASK_DEL                61u
#define OS_ERR_TASK_DEL_IDLE           62u
#define OS_ERR_TASK_DEL_REQ            63u
#define OS_ERR_TASK_DEL_ISR            64u
#define OS_ERR_TASK_NAME_TOO_LONG      65u
#define OS_ERR_TASK_NO_MORE_TCB        66u
#define OS_ERR_TASK_NOT_EXIST          67u
#define OS_ERR_TASK_NOT_SUSPENDED      68u
#define OS_ERR_TASK_OPT                69u
#define OS_ERR_TASK_RESUME_PRIO        70u
#define OS_ERR_TASK_SUSPEND_IDLE       71u
#define OS_ERR_TASK_SUSPEND_PRIO       72u
#define OS_ERR_TASK_WAITING            73u

#define OS_ERR_TIME_NOT_DLY            80u
#define OS_ERR_TIME_INVALID_MINUTES    81u
#define OS_ERR_TIME_INVALID_SECONDS    82u
#define OS_ERR_TIME_INVALID_MS         83u
#define OS_ERR_TIME_ZERO_DLY           84u
#define OS_ERR_TIME_DLY_ISR            85u

#define OS_ERR_MEM_INVALID_PART        90u
#define OS_ERR_MEM_INVALID_BLKS        91u
#define OS_ERR_MEM_INVALID_SIZE        92u
#define OS_ERR_MEM_NO_FREE_BLKS        93u
#define OS_ERR_MEM_FULL                94u
#define OS_ERR_MEM_INVALID_PBLK        95u
#define OS_ERR_MEM_INVALID_PMEM        96u
#define OS_ERR_MEM_INVALID_PDATA       97u
#define OS_ERR_MEM_INVALID_ADDR        98u
#define OS_ERR_MEM_NAME_TOO_LONG       99u

#define OS_ERR_NOT_MUTEX_OWNER        100u

#define OS_ERR_FLAG_INVALID_PGRP      110u
#define OS_ERR_FLAG_WAIT_TYPE         111u
#define OS_ERR_FLAG_NOT_RDY           112u
#define OS_ERR_FLAG_INVALID_OPT       113u
#define OS_ERR_FLAG_GRP_DEPLETED      114u
#define OS_ERR_FLAG_NAME_TOO_LONG     115u

#define OS_ERR_PIP_LOWER              120u

#define OS_ERR_TMR_INVALID_DLY        130u
#define OS_ERR_TMR_INVALID_PERIOD     131u
#define OS_ERR_TMR_INVALID_OPT        132u
#define OS_ERR_TMR_INVALID_NAME       133u
#define OS_ERR_TMR_NON_AVAIL          134u
#define OS_ERR_TMR_INACTIVE           135u
#define OS_ERR_TMR_INVALID_DEST       136u
#define OS_ERR_TMR_INVALID_TYPE       137u
#define OS_ERR_TMR_INVALID            138u
#define OS_ERR_TMR_ISR                139u
#define OS_ERR_TMR_NAME_TOO_LONG      140u
#define OS_ERR_TMR_INVALID_STATE      141u
#define OS_ERR_TMR_STOPPED            142u
#define OS_ERR_TMR_NO_CALLBACK        143u

#define OS_ERR_BADPARAMS              144u
#define OS_ERR_FAILURE                145u


// copying this since the there are more than 64 priorities in blast
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 16u + 1u)/* Size of ready table                         */

typedef uint32_t sig_node;

typedef struct os_event {
    uint8_t    OSEventType;                    /* Type of event control block (see OS_EVENT_TYPE_xxxx)    */
    void      *OSEventPtr;                     /* Pointer to message or queue structure                   */
    uint16_t   OSEventCnt;                     /* Semaphore Count (not used if other EVENT type)          */
    OS_PRIO    OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
    OS_PRIO    OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    qurt_mutex_t                             mutex;
} OS_EVENT;

typedef struct os_flag_grp {                /* Event Flag Group                                        */
    uint8_t       OSFlagType;               /* Should be set to OS_EVENT_TYPE_FLAG                     */
    void         *OSFlagWaitList;           /* Pointer to first NODE of task waiting on event flag     */
    OS_FLAGS      OSFlagFlags;              /* 8, 16 or 32 bit flags                                   */
    qurt_anysignal_t                        signal;
    uint32_t                                unChannelWaitMask;
} OS_FLAG_GRP;

/*=====================================================================
  INTERNAL FUNCTION PROTOTYPES
  =======================================================================*/
/* Task */
uint8_t       sns_os_task_create_ext       (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio,
                                            uint16_t         id,
                                            OS_STK          *pbos,
                                            uint32_t         stk_size,
                                            void            *pext,
                                            uint16_t         opt,
                                            uint8_t          *name);

/* Flag */

OS_FLAG_GRP  *sns_os_sigs_create           (OS_FLAGS         flags,
                                            uint8_t         *perr);

OS_FLAGS      sns_os_sigs_pend             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint32_t         timeout,
                                            uint8_t         *perr);

OS_FLAGS      sns_os_sigs_post             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          opt,
                                            uint8_t         *perr);


OS_FLAGS      sns_os_sigs_accept           (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint8_t         *perr);

#ifdef QDSP6
sig_node    *sns_os_sigs_add              (OS_FLAG_GRP     *pgrp,
                                           OS_FLAGS         flags);

sig_node     *sns_os_sigs_del              (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags);

void         sns_os_set_qmi_csi_params    (OS_FLAG_GRP       *pgrp,
                                           OS_FLAGS           flags,
                                           qmi_csi_os_params *os_params,
                                           uint8_t           *perr);
#endif

/* Mutex */

OS_EVENT     *sns_os_mutex_create          (uint8_t          prio,
                                            uint8_t         *perr);

OS_EVENT     *sns_os_mutex_del             (OS_EVENT        *pevent,
                                            uint8_t          opt,
                                            uint8_t         *perr);

void          sns_os_mutex_pend            (OS_EVENT        *pevent,
                                            uint32_t         timeout,
                                            uint8_t         *perr);

uint8_t       sns_os_mutex_post            (OS_EVENT        *pevent);

OS_EVENT     *sns_os_mutex_create_uimg     (uint8_t          prio,
                                            uint8_t         *perr);

void          sns_os_sem_init_val           (OS_SEM         *sem,
                                             uint16_t          val);
int32_t        sns_os_sem_up                 (OS_SEM         *sem);

int32_t        sns_os_sem_down               (OS_SEM         *sem);

#ifdef SNS_DSPS_PROFILE_ON
void          sns_os_time_dly( uint16_t ticks);
#endif

#endif /* _SNS_OSA_H_ */
