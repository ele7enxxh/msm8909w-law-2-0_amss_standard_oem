/*==============================================================================

FILE:      ABT_qurt_interrupt.c

DESCRIPTION: This file bypasses the DAL interface and uses QuRT to register
             interrupts

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2013 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ABT_qurt_interrupt.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
08/30/13   pm      Renamed ABT_Register_Interrupt()
05/24/13   pm      Change "Err.h" to "err.h" to allow Linux compilation
05/13/13   pm      Changed InterruptStateType to avoid T32 symbol collision
04/16/13   pm      Copied to ABT_qurt_interrupt.c and customized for ABT
07/28/07   an      Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "DDIInterruptController.h"
#include <DALSys.h>
#include <DALSysCmn.h>
#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "qurt_anysignal.h"
#include "stringl.h"
#include "ABT_platform.h"
#include "err.h"


/*
 * ABT_InterruptStateType
 *
 * Data structure containing all information about an interrupt
 *
 *  Isr               - Interrupt service routine to invoke.
 *  nParam            - Parameter to pass the ISR.
 *  nTrigger          - The interrupt trigger type.
 * *pTCB              - The task control block of the IST task.
 *  nThreadID         - The thread id of the IST task.
 * *pISTStack         - The IST stack size.
 * *pISTName          - The name of the interrupt service thread.
 * nPriority          - Priority of the interrupt thread.
 * nISTStackSize      - Interrupt Service Thread Stack Size.
 */
typedef struct
{
  DALISR                    Isr;
  DALISRCtx                 nParam;
  uint8                     nTrigger;
  qurt_thread_t             nThreadID;
  unsigned char *           pISTStack;
  uint32                    nInterruptVector;
  char                      *pISTName;
  qurt_anysignal_t          ISTSignal;
  qurt_thread_attr_t        ThreadAttr;
  uint32                    nPriority;
  uint32                    nISTStackSize;
} ABT_InterruptStateType;

ABT_InterruptStateType ABTInterruptState;

#define IST_TASKNAME_SIZE 16
#define INTERRUPT_SIGNAL_MASK             0x1
#define INTERRUPT_TASK_START              0x00008000

/*==========================================================================

                         FUNCTION  DEFINITIONS

===========================================================================*/
/**
@brief
      This is the main Interrupt service thread function that processes
      incoming ABT interrupts from QURT.

@param[in]  ISTParam  The actual InterruptVector passed in

@return
      None.

@dependencies
      None.

@sideeffects
      None.
*/
/*==========================================================================*/
static void ABTISTMain
(
  void * ISTParam
)
{
  int nStatus;
  uint32 nInterruptVector;

  nInterruptVector = *((uint32 *)ISTParam);

  ABTInterruptState.nThreadID = qurt_thread_get_id();

  /*
   * Main loop.  Process an interrupt IPC, then wait for another.
   */
  while(1)
  {

   /*
    * Register with QURT using the interrupt vector
    */
   nStatus = qurt_interrupt_register(nInterruptVector,&ABTInterruptState.ISTSignal,
                                     INTERRUPT_SIGNAL_MASK);
   if (QURT_EOK != nStatus)
   {
      ERR_FATAL("ABTimeout: Cannot register with QuRT using the interrupt vector.", 0, 0, 0);
      return;
   }

   while (1)
   {
      unsigned int interruptSignalValue;

      interruptSignalValue = qurt_anysignal_wait( &ABTInterruptState.ISTSignal,
                                                  INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );

      if ( SIG_INT_ABORT & interruptSignalValue )
      {
        break;
      }

      if ( !(INTERRUPT_SIGNAL_MASK & interruptSignalValue) )
      {
        // we got an invalid signal
        break;
      }

      // call your callback
      ABTInterruptState.Isr(ABTInterruptState.nParam);

      /*
       * Clear signal and reactivate interrupt
       */
      qurt_anysignal_clear(&ABTInterruptState.ISTSignal, INTERRUPT_SIGNAL_MASK );
      nStatus = qurt_interrupt_acknowledge(nInterruptVector);
      if (QURT_EOK != nStatus)
      {
        // unable to acknowledge interrupt
        // possibly because the callback function has de-register from this
        // interrupt.
        ERR_FATAL("ABTimeout: QURT unable to acknowledge interrupt.", 0, 0, 0);
        break;
      }
    }

    /*
     * Wait for the next time this interrupt is registered with QURT.
     */

    if(INTERRUPT_TASK_START & qurt_anysignal_wait(&ABTInterruptState.ISTSignal,
                                                  INTERRUPT_TASK_START))
    {
      qurt_anysignal_clear(&ABTInterruptState.ISTSignal, INTERRUPT_TASK_START);
    }

    /*
     * clear signals before re-registering.
     */
    qurt_anysignal_clear(&ABTInterruptState.ISTSignal,
                         INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );
  }
} /* END ISTMain */


/*==========================================================================*/
/**
@brief
      This function sets the trigger for the interrupt vector using QURT OS
      APIs.

@param[in]  nInterrupt  The interrupt vector number
@param[in]  nTrigger    The interrupt trigger type

@return
      None.

@dependencies
      None.

@sideeffects
      None.
*/
/*==========================================================================*/
static void SetQURTTriggerConfig
(
  uint32 nInterruptVector,
  uint32 nTrigger
)
{
  uint32 nDetectType,nPolarity;

  switch (nTrigger)
  {
    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      nDetectType = 1;
      nPolarity = 1;
      break;

    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      nDetectType = 1;
      nPolarity = 0;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      nDetectType = 0;
      nPolarity = 0;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      nDetectType = 0;
      nPolarity = 1;
      break;

    default:
       return;
  }

  /*
  * Change the interrupt trigger only when the interrupt is not registered.
  */
  if (QURT_EOK != qurt_interrupt_set_config(nInterruptVector, nDetectType, nPolarity))
  {
     ERR_FATAL("ABTimeout: Failed to set the QURT interrupt trigger.", 0, 0, 0);
  }
  return;

} /* SetQURTTriggerConfig */


/*==========================================================================*/
/**
@brief
      This function registers the interrupt service routine

@param[in] nInterruptVector    ID of the interrupt vector with which the client
                               wants to register a callback.
@param[in] isr      Callback function to be registered.
@param[in] ctx      Parameter associated with the callback function.
@param[in] nTrigger Client must use one of the interrupt flags, which
                       contain the interrupt triggers. The trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
                    Optionally, bits 31-24 contain the interrupt priority desired

@return
  ABT_SUCCESS -- Interrupt was enabled successfully and
                 client notification was registered. \n
  ABT_ERROR_INIT_FAILURE -- Otherwise

@dependencies
      None.

@sideeffects
      None.
*/
/*==========================================================================*/
ABT_error_type ABT_Platform_Register_Interrupt_OS
(
  uint32  nInterruptVector,
  const   DALISR isr,
  const   DALISRCtx ctx,
  uint32  nTrigger
)
{
  int nResult;
  unsigned int StackSize;
  unsigned char * StackAddr;

  /* Just for now initializing it here */
  ABTInterruptState.nISTStackSize = 2048;

  // Enter critical section?  Not needed, because ABT only calls into this one time.

  // the below is just a storage structure where you will store your callback and your parameter.
  ABTInterruptState.Isr = isr;
  ABTInterruptState.nParam = ctx;
  ABTInterruptState.nTrigger = nTrigger & 0x00FFFFFF;  /* Mask out above lowest 24 bits */
  ABTInterruptState.nPriority =  nTrigger >> 24;       /* Give us highest 8 bits */
  SetQURTTriggerConfig(nInterruptVector,nTrigger);

  if(ABTInterruptState.pISTStack != NULL)
  {
    /*
     * An IST thread is already created.
     * It will only be restarted on a re registration.
     */
    qurt_anysignal_set(&ABTInterruptState.ISTSignal,
                     INTERRUPT_TASK_START);
    return ABT_SUCCESS;
  }

  if(DALSYS_Malloc((sizeof(unsigned char))*(ABTInterruptState.nISTStackSize),/*this is the stack size needed*/
  (void **)&ABTInterruptState.pISTStack) == DAL_ERROR)
  {
     ERR_FATAL("ABTimeout: Malloc failed  for QURT interrupt stack.", 0, 0, 0);
     return ABT_ERROR_INIT_FAILURE;
  }

  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(ABTInterruptState.pISTStack, 0,
  ((sizeof(unsigned char))* (ABTInterruptState.nISTStackSize)));

  if(DALSYS_Malloc(((sizeof(char))*IST_TASKNAME_SIZE),
  (void **)&ABTInterruptState.pISTName) == DAL_ERROR)
  {
     ERR_FATAL("ABTimeout: Malloc failed for QURT ISTName.", 0, 0, 0);
     return ABT_ERROR_INIT_FAILURE;
  }
  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(ABTInterruptState.pISTName, 0,
    (sizeof(char))*IST_TASKNAME_SIZE);

  /*
   * Set the IST name to IST<InterruptVector>
   */
  snprintf(ABTInterruptState.pISTName,
        (sizeof(unsigned char)*IST_TASKNAME_SIZE), "ABTIST%d",(int)nInterruptVector);

  /*
   *  Initialize any thread attribute and signal objects needed by the interrupt service thread.
   */
  qurt_anysignal_init(&ABTInterruptState.ISTSignal);
  qurt_thread_attr_init (&ABTInterruptState.ThreadAttr);

  /*
   * 64 bit alignment of the stack ptr.
   */
  StackAddr = (unsigned char *)
            (((unsigned long)ABTInterruptState.pISTStack -1) & (~0x7)) + 0x8;

  StackSize = (unsigned int)( ABTInterruptState.nISTStackSize -
            ((unsigned long)StackAddr -
            (unsigned long)ABTInterruptState.pISTStack) );

  /*
   * To ensure that the entire stack (start to end) is 64-bit aligned
   * so the last odd bytes are dropped off.
   */
  StackSize &= ~(0x7);

  ABTInterruptState.nInterruptVector = nInterruptVector;
  qurt_thread_attr_set_stack_size(&ABTInterruptState.ThreadAttr,StackSize);
  qurt_thread_attr_set_stack_addr (&ABTInterruptState.ThreadAttr,(void *)StackAddr);
  qurt_thread_attr_set_priority (&ABTInterruptState.ThreadAttr,
                                 (unsigned short)ABTInterruptState.nPriority);
  qurt_thread_attr_set_name(&ABTInterruptState.ThreadAttr,ABTInterruptState.pISTName);

  nResult = qurt_thread_create((qurt_thread_t *)&ABTInterruptState.nThreadID,
                             &ABTInterruptState.ThreadAttr,
                             ABTISTMain,(void *)&ABTInterruptState.nInterruptVector);

  if (nResult == QURT_EFATAL)
  {
     ERR_FATAL("ABTimeout: Failed to create QURT thread.", 0, 0, 0);
     return ABT_ERROR_INIT_FAILURE;
  }

  //  Leave critical section?  Not needed, because ABT only calls into this one time.

  return ABT_SUCCESS;

} /* END InterruptController_RegisterInterrupt */


