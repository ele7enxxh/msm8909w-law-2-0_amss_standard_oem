#ifndef _DAL_INTRCTRLR_CONFIG_H
#define _DAL_INTRCTRLR_CONFIG_H
/*==============================================================================

FILE:      DALInterruptController_config.h

DESCRIPTION: This file implements a Interrupt Controller for QDSP6 hardware
             interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2012 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/InterruptController/src/qurt/DALInterruptControllerConfig.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/28/07   an     Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "DALInterruptController.h"
#include "atomic_ops.h"
#include "rex.h"
/*=========================================================================

                           CONSTANTS

===========================================================================*/
#define MAX_QURT_IRQ 310
#ifndef DALINTERRUPT_SUPPORTED_INTERRUPTS
  #define DALINTERRUPT_SUPPORTED_INTERRUPTS MAX_QURT_IRQ
#else
  #define DALINTERRUPT_MEMORY_SAVE_MODE
#endif

/*
 * IST THREAD PRIORITY  LIMITS
 *
 * Base IST priority level. This is the lowest priority of an interrupt service thread.
 * Highest priority level. These two limits are checks on the range of the Interrupt 
 * Thread priorities that can be set.
 */
#define IST_THREAD_PRIORITY_BASE    183
#define IST_THREAD_HIGHEST_PRIORITY 191


 /*
  *  Invalid interrupt value for error checking
  */
#define INVALID_INTERRUPT                0x7FFFFFFF
#define IST_TASKNAME_SIZE 16
/*
 * For each interrupt handler thread a Default IST stack size is declared.
*/
#define DAL_IST_STACK_SIZE     2048
#define DAL_IST_STACK_SIZE_4k  4096

/* Type of IST configuration */
#define DAL_IST_CONFIG_DYNAMIC      1

/*
 * Interrupt Controller internal interrupt state flags.
 */
#define INTERRUPTF_MPM           0x00000001
#define INTERRUPTF_CLEARBEFORE   0x00000002
#define INTERRUPTF_CLEARAFTER    0x00000004
#define INTERRUPTF_REGISTERED    0x00000008
#define INTERRUPTF_RUNNING       0x00000010
#define INTERRUPTF_SUSPENDED     0x00000020
#define INTERRUPTF_FAKETRIGGER   0x00000040



/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*
 * InterruptLogEventType
 *
 * Log event type.
 *
 *  NULL:          NULL event
 *  NEST_START:    Start of an interrupt processing nesting level.
 *  ISR_START:     Start of ISR processing.
 *  ISR_FINISH:    Finished ISR processing.
 *  NEST_FINISH:   Completion of an interrupt processing nesting level.
 *  SET:           An interrupt was manually triggered.
 *  SEND:          Sent an interrupt to another processor.
 *  SLEEP:         Went to sleep.
 *  WAKEUP:        Woke-up from sleep.
 *  UNHANDLED:     Unhandled interrupt received.
 *  REENTRY:       Interrupt received while still being processed.  This
 *                 is an error since re-entrance is not allowed.
 */
typedef enum
{
  INTERRUPT_LOG_EVENT_NULL,

  /*
   * Common log events
   */
  INTERRUPT_LOG_EVENT_ISR_START,
  INTERRUPT_LOG_EVENT_ISR_FINISH,
  INTERRUPT_LOG_EVENT_SUBISR_START,
  INTERRUPT_LOG_EVENT_SUBISR_FINISH,
  INTERRUPT_LOG_EVENT_TRIGGERED,
  INTERRUPT_LOG_EVENT_SENT,
  INTERRUPT_LOG_EVENT_UNHANDLED,
  INTERRUPT_LOG_EVENT_SUSPENDED,
  INTERRUPT_LOG_EVENT_UNSUPPORTED,
  INTERRUPT_LOG_EVENT_SLEEP,
  INTERRUPT_LOG_EVENT_WAKEUP,
  INTERRUPT_LOG_EVENT_SHUTDOWN,
  INTERRUPT_NUM_LOG_EVENTS,
  PLACEHOLDER_InterruptLogEventType = 0x7fffffff
} InterruptLogEventType;

/*
 * InterruptLogEntriesType
 *
 * Log entry type.
 *
 *  event        - The event that is being logged.
 *  irq:         - The interrupt number, or NULL_IRQ if not applicable.
 *  timestamp    - The timestamp in sclks.
 *  pcycles      - the number of pcycles that has elapsed since the qdsp6 core is up.
 */
typedef struct
{
  InterruptLogEventType     eEvent;
  uint32                    nInterrupt;
  char*                     pInterruptName;
  uint64                    nTimeStamp;
  unsigned long long int    pcycles;
} InterruptLogEntryType;


/*
 * InterruptLogType
 *
 * Log type container.
 */
typedef struct
{
  atomic_word_t                AtomicIdx;
  uint32                       nIdx;
  uint32                       nLogSize;
  InterruptLogEntryType*       pEntries;
} InterruptLogType;

typedef void (*DALISR_HandlerType) (uint32 param);

/*
 * InterruptStateType
 *
 * Data structure containing all information about an interrupt
 *
 *  Isr               - Interrupt service routine to invoke.
 *  nParam            - Parameter to pass the ISR.
 *  nTrigger          - The interrupt trigger type.
 *  flags             - Interrupt flags.
 *  nMPMID            - MAO interrupt ID if this is a wakeup source for the MPM
 *                      hardware.  Only used if TRAMPF_MAOINT is set.
 *  nCount            - Count of how many times the interrupt fired.
 *  nLastFired        - The time stamp of the last time the interrupt fired.
 * *pTCB              - The task control block of the IST task.
 *  nThreadID         - The thread id of the IST task.
 * *pISTStack         - The IST stack size.
 * *pISTName          - The name of the interrupt service thread.
 * nTimeTestProfileID - The time test id is used only for profiling purposes.
 * nPriority          - Priority of the interrupt thread.
 * nISTStackSize      - Interrupt Service Thread Stack Size.
 */
typedef struct
{
  DALISR                    Isr;
  DALISRCtx                 nParam;
  uint8                     nTrigger;
  uint8                     nFlags;
  #ifdef DALINTERRUPT_MPM_WAKEUP
  uint32                    nMPMID;
  #endif
  uint32                    nCount;
  uint64                    nLastFired;
  rex_tcb_type              *pTCB;
  int                       nThreadID;
  unsigned char *           pISTStack;
  char                     *pISTName;
  uint16                    nTimeTestProfileID;
  uint32                    nPriority;
  uint32                    nISTStackSize;
  char*                     pInterruptName;
} InterruptStateType;

/*
 * InterruptConfigType
 */
typedef struct
{
  uint32                     nInterruptVector;
   uint8                     nPriority;
  uint32                     nISTStackSize;
  char                       *pInterruptName;
 
} InterruptConfigType;


 /*
  * InterruptPlatformDataType
  *
  * pIRQConfigs            - Pointer to an array of interrupt definitions in the BSP.
  * nMaxIRQ                - Maximum number of supported interrupts.
  */
typedef struct
{
  InterruptConfigType       *pIRQConfigs;
  uint32                    nMaxIRQ;
} InterruptPlatformDataType;

 /*
  * InterruptDataType
  *
  * nLUTIdx                - The lookup table index.
  * *pTimetickHandle       - Client handle to the timetick dal.
  * pClientCtxt            - Handle to the Client context.
  * pInterruptState        - pointer to the array keeping track of the interrupt state.
  * *pLog                  - Log for interrupts that come in.
  * *pPlatformConfig       - Interrupt Controller platform configuration data.
  */
typedef struct
{
  DalDeviceHandle                *pTimetickHandle;
  InterruptControllerClientCtxt  *pClientCtxt;
  InterruptStateType             *pInterruptState;
  InterruptLogType               pLog;
  InterruptPlatformDataType      *pPlatformConfig;
} InterruptDataType;



#endif //_DAL_INTRCTRLR_CONFIG_H

