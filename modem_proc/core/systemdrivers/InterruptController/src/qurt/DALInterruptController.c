/*==============================================================================

FILE:      DALInterruptController.c

DESCRIPTION: This file implements a Interrupt Controller for QDSP6 L2VIC harware 
interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/InterruptController/src/qurt/DALInterruptController.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/28/07   an     Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <DALInterruptController.h>
#include <DALSys.h>
#include <DALSysCmn.h>
#include "DALInterruptControllerConfig.h"
#include "DDITimetick.h"
#include "DALDeviceId.h"
#ifdef DALINTERRUPT_MPM_WAKEUP
#include "mpmint.h"
#endif
#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "rex.h"
#include "task.h"
#include "stringl.h"
#ifdef TIMETEST
#error code not present
#endif
#ifdef INCTL_ULOG_STUB
#include "DALInterruptControllerUlogStub.h"
#else
  #include "ULogFront.h"
#endif
#define INTERRUPT_THREAD_MEDIAN_PRIORITY  4
#define INTERRUPT_SIGNAL_MASK             0x1

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * This is the internal static data variable used for the functioning of the 
 * Interrupt controller.
 */
static InterruptDataType *pInterruptData;

/*=========================================================================

                           MACRO DEFINITIONS

===========================================================================*/

/*
 * INTERRUPT_TIMETEST
 *
 * Macro to define a timetest ID for an interrupt.
 */
#define TIMETEST_INTERRUPT_ISR_FLAG  0x1000
#define INTERRUPT_TIMETESTID(irq)      (TIMETEST_INTERRUPT_ISR_FLAG | irq)

#define INTERRUPTCONTROLLER_SET_PRIORITY(x)  \
((IST_THREAD_PRIORITY_BASE + x) <= IST_THREAD_HIGHEST_PRIORITY ?  \
(IST_THREAD_PRIORITY_BASE + x) : IST_THREAD_PRIORITY_BASE)

/*
 * Macro to set the IST stack
 *
 * The user defined flag for now should be no greater than 3.
 * the stack is increased in multiples of 2048 bytes.
 */
#define INTERRUPTCONTROLLER_SET_STACK(x)  \
((x) <= 3 ?  \
(DAL_IST_STACK_SIZE*x) : DAL_IST_STACK_SIZE)

/*==========================================================================

                         FUNCTION  DECLARATIONS

==========================================================================*/

static DALResult GetPlatformData(InterruptControllerClientCtxt * pclientCtxt);
#ifdef DALINTERRUPT_MPM_WAKEUP
static DALResult SetWakeupInterruptTrigger(uint32 IntID,uint32 nTrigger);
#endif
static void ClearInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void SetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static boolean GetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void LogInterruptEvent(InterruptControllerClientCtxt  *pclientCtxt,
InterruptLogEventType eEvent,uint32 nInterrupt);
static void TriggerClientEvent(uint32 nInterruptVector,
InterruptControllerClientCtxt *pClientCtxt);
static DALResult SetTrigger(InterruptControllerDevCtxt *pDevice,
uint32 nInterruptVector,uint32 nTrigger);
static DALResult SetQURTTriggerConfig(InterruptControllerDevCtxt *pDevice,
uint32 nInterruptVector,uint32 nTrigger);
static void ISTMain(uint32 nInterruptVector);
static DALResult ConfigureDynamicIST( uint32 nInterruptVector, uint32 nPriority, char* pISTName);
static DALResult InterruptController_RegisterInterrupt(InterruptControllerClientCtxt * pclientCtxt, DALInterruptID  nInterruptVector, 
                                                        const DALISR isr,const DALISRCtx ctx, uint32  IntrFlags,char* pISTName);
/*==========================================================================

                         FUNCTION  DEFINITIONS

===========================================================================

  FUNCTION      ISTMain

  DESCRIPTION   This is the main Interrupt service thread function that 
  processes incoming interrupts from QURT.

  PARAMETERS   Arg this the actual InterruptVector passed in.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
static void ISTMain
(
  uint32 nInterruptVector
)
{
  InterruptControllerDevCtxt *pDevice; 
  int nStatus;
  qurt_anysignal_t interruptSignal;
  
  qurt_anysignal_init( &interruptSignal );

  pDevice = pInterruptData->pClientCtxt->pInterruptControllerDevCtxt; 

  if( pInterruptData->pInterruptState[nInterruptVector].pTCB == NULL )
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: Interrupt service thread has a null TCB");
  }

  pInterruptData->pInterruptState[nInterruptVector].nThreadID = qurt_thread_get_id();

  /*
   * Main loop.  Process an interrupt IPC, then wait for another.
   */
  while(1)
  {
    
   /*
    * check if interrupt registration flag is set and only register if its still set.
    */
    DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 
    if(!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
    {
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
      goto WaitForTaskStart;
    }

   /*
    * Register with QURT using the interrupt vector
    */
   nStatus = qurt_interrupt_register(nInterruptVector, &interruptSignal, INTERRUPT_SIGNAL_MASK);
   if (QURT_EOK != nStatus)
   {
     DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: failed to register the interrupt"); 
   }
   DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);

   while (1)
   {
      unsigned int interruptSignalValue;

      interruptSignalValue = qurt_anysignal_wait( &interruptSignal, 
            INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );

      if ( SIG_INT_ABORT & interruptSignalValue )
      {
        break;
      }
      
      if ( !(INTERRUPT_SIGNAL_MASK & interruptSignalValue) )
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
          "Interrupt Controller: invalid interrupt signal");
        break;
      }

      DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 
      if (GetInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING))
      {
        SetInterruptFlag(nInterruptVector, INTERRUPTF_SUSPENDED);
        DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
        LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_SUSPENDED, 
        nInterruptVector);
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
          "Interrupt Controller: the interrupt is already being processed from the QURT Kernel");
        break;
      }
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
  
      /*
       * Log the start of the ISR.
       */
      LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_ISR_START, 
            nInterruptVector);

      /*
       * As soon as we get an interrupt we lock interrupts so as to 
       * ensure no new interrupt comes in.  Lock will be released right before 
       * triggering the client event.
       */
      DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 
  
      /*
       * log the interrupt and trigger the registered event.
       */
      TriggerClientEvent(nInterruptVector, pInterruptData->pClientCtxt);
  
      /*
       * Release the synchronization object before waiting for next interrupt.
       */
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
  
      /*
       * Log the entry
       */
      LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_ISR_FINISH, 
        nInterruptVector);
  
      /*
       * Clear signal and reactivate interrupt
       */
      qurt_anysignal_clear( &interruptSignal, INTERRUPT_SIGNAL_MASK );
      nStatus = qurt_interrupt_acknowledge(nInterruptVector);
      if (QURT_EOK != nStatus)
      {
        // unable to acknowledge interrupt
        // possibly because the callback function has de-register from this
        // interrupt.
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
           "Interrupt Controller: failed to ACK the interrupt"); 
        break;
      }
    }

    WaitForTaskStart:

    /*
     * Wait for the next time this interrupt is registered with QURT.
     */
    rex_wait(TASK_START_SIG);
    rex_clr_sigs(rex_self(), TASK_START_SIG);

    /* 
     * clear signals before re-registering
     */
    qurt_anysignal_clear( &interruptSignal, 
          INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );
 }
} /* END ISTMain */


/*===========================================================================

  FUNCTION      ConfigureDynamicIST

  DESCRIPTION   This function spawns a single IST thread for handling each interrupts.

  PARAMETERS   
  nInterruptVector : the actual interrupt number on the L2VIC.
  nPriority : The thread is spawned with the priority of the interrupt thread.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
static DALResult ConfigureDynamicIST
(
  uint32 nInterruptVector, 
  uint32 nPriority,
  char* pISTName
)
{
  
  if(pInterruptData->pInterruptState[nInterruptVector].pTCB != NULL)
  {
    /* 
     * An IST thread is already created.
     * It will only be restarted on a re registration.
     */
    rex_set_sigs(pInterruptData->pInterruptState[nInterruptVector].pTCB, TASK_START_SIG);
    return DAL_SUCCESS;
  }

  if(DALSYS_Malloc(
  (sizeof(unsigned char))*(pInterruptData->pInterruptState[nInterruptVector].nISTStackSize),
  (void **)&pInterruptData->pInterruptState[nInterruptVector].pISTStack) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(pInterruptData->pInterruptState[nInterruptVector].pISTStack, 0,
  ((sizeof(unsigned char))* (pInterruptData->pInterruptState[nInterruptVector].nISTStackSize)));

  if(DALSYS_Malloc(((sizeof(char))*IST_TASKNAME_SIZE),
  (void **)&pInterruptData->pInterruptState[nInterruptVector].pISTName) == DAL_ERROR)
  {
    return DAL_ERROR;
  }
 
  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(pInterruptData->pInterruptState[nInterruptVector].pISTName, 0, 
      (sizeof(char))*IST_TASKNAME_SIZE);

  /*
   * Set the IST name to IST<InterruptVector>
   */
  snprintf((char *)pInterruptData->pInterruptState[nInterruptVector].pISTName, 
    (sizeof(unsigned char)*IST_TASKNAME_SIZE), "IST%d",(int)nInterruptVector);

  if(pISTName !=NULL)
  {
    strlcat((char *)pInterruptData->pInterruptState[nInterruptVector].pISTName,pISTName,IST_TASKNAME_SIZE);
  }

  if(DALSYS_Malloc(sizeof(rex_tcb_type),
    (void **)&pInterruptData->pInterruptState[nInterruptVector].pTCB) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(pInterruptData->pInterruptState[nInterruptVector].pTCB, 0, 
  sizeof(rex_tcb_type));

  #ifdef TIMETEST
  #error code not present
#endif
  rex_def_task_ext4(
    REX_ANY_SMT_MASK,
    pInterruptData->pInterruptState[nInterruptVector].pTCB, 
    pInterruptData->pInterruptState[nInterruptVector].pISTStack, 
    pInterruptData->pInterruptState[nInterruptVector].nISTStackSize, 
    nPriority,
    (rex_task_func_type)ISTMain, 
    nInterruptVector,
    pInterruptData->pInterruptState[nInterruptVector].pISTName,
    FALSE ); 
       
  return DAL_SUCCESS;

} /* END ConfigureDynamicIST */


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult InterruptController_DriverInit( InterruptControllerDrvCtxt *pclientCtxt )
{
  return DAL_SUCCESS;

}

DALResult InterruptController_DriverDeInit( InterruptControllerDrvCtxt *pclientCtxt )
{
  return DAL_SUCCESS;

}

/*=============================================================================

  Following functions are declared in IDevice Interface. 

===============================================================================

  FUNCTION      InterruptController_DeviceInit

  DESCRIPTION   This function is called when a driver tries to attach from the 
  DALInterruptController.The purpose of this function is to initialize the 
  device specific data for the interrupt controller.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual client 
  context.

  DEPENDENCIES  DALSYS should be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
DALResult InterruptController_DeviceInit
( 
  InterruptControllerClientCtxt *pclientCtxt 
)
{

  if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
    &pclientCtxt->hSyncIntrCtrlTbl, NULL))
  {
    return DAL_ERROR;
  }

  if(GetPlatformData(pclientCtxt) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  /*
   * Acquire system timer.
   */
  if(DalTimetick_Attach("SystemTimer", &pInterruptData->pTimetickHandle) != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  return DAL_SUCCESS;

} /* END InterruptController_DeviceInit */


/*==========================================================================

  FUNCTION      InterruptController_DeviceDeInit

  DESCRIPTION   This function is called when a driver tries to detach from 
  the DALInterruptController.The purpose of this function is to cleanup all 
  dynamically allocated data and reset the DALInterruptController state.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual 
  client context.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
DALResult InterruptController_DeviceDeInit
( 
  InterruptControllerClientCtxt *pclientCtxt
)
{
  return DAL_SUCCESS;

} /* END InterruptController_DeviceDeInit */


/*=============================================================================

  Following functions are extended in IInterruptController Interface.

===============================================================================

  FUNCTION      InterruptController_RegisterEvent
  DESCRIPTION   See DDIInterruptController.h  This function for now is being 
  used on the Modem until all WCDMA issues are resolved.

=============================================================================*/
DALResult  InterruptController_RegisterEvent
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector,
  const DALSYSEventHandle hEvent,
  uint32 IntrFlags
) 
{

  /* Event trigger api will be supported when we get a confirmation from WCDMA 
   * that they have resolved their interrupt usage issues.  
   */
  return DAL_ERROR;

} /* END InterruptController_RegisterEvent */


/*==========================================================================

  FUNCTION      InterruptController_RegisterISR
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_RegisterISR
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags
) 
{
   return InterruptController_RegisterInterrupt(pclientCtxt, nInterruptVector, isr, ctx, 
                                       IntrFlags, NULL);

} /* END InterruptController_RegisterISR */


/*==========================================================================

  FUNCTION      InterruptController_RegisterIST
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_RegisterIST
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags,
  char* pISTName
) 
{

  return InterruptController_RegisterInterrupt(pclientCtxt, nInterruptVector, isr, ctx, 
                                       IntrFlags, pISTName);

} /* END InterruptController_RegisterIST */


/*==========================================================================

  FUNCTION      InterruptController_RegisterInterrupt
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
static DALResult  InterruptController_RegisterInterrupt
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags,
  char* pISTName
) 
{
  uint32 nInterruptPriority,nTrigger,nStackSize;
  InterruptControllerDevCtxt *pDevice;
  DALISR OldIsr;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  OldIsr = (DALISR)pInterruptData->pInterruptState[nInterruptVector].Isr;
  
  if((nInterruptVector == INVALID_INTERRUPT) ||(isr == NULL) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not Register Interrupt");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  nInterruptPriority = (IntrFlags >> DAL_INTERRUPT_PRIORITY_SHFT) & DAL_INTERRUPT_PRIORITY_MASK;
  nTrigger = IntrFlags & DAL_INTERRUPT_TRIGGER_MASK;
  nStackSize = (IntrFlags >> DAL_INTERRUPT_ISTSTACK_SHFT) & DAL_INTERRUPT_ISTSTACK_MASK;

  /*
   * In the new architecture the user has to specify the interrupt priority. 
   * If this is not given the interrupt controller checks if it has stored 
   * platform data that has this information.
   * In the absense of this information the default base priority is assigned.
   */
  if(nInterruptPriority == 0)
  {
    nInterruptPriority = pInterruptData->pInterruptState[nInterruptVector].nPriority;
  }

  nInterruptPriority = INTERRUPTCONTROLLER_SET_PRIORITY(nInterruptPriority);

  /*
   * In the new architecture stack can be specified in two ways ;
   * 1. InterruptController Platform data can specify the interrupt stack for each IST thread.
   * 2. The User of the interrupt can also request for a higher stack via the flags that are passed in the registratino api.
   * In the absense of this information the default base stack size is assigned.
   */

  if (nStackSize != 0) 
  {
    pInterruptData->pInterruptState[nInterruptVector].nISTStackSize = INTERRUPTCONTROLLER_SET_STACK(nStackSize);
  }
  if(pInterruptData->pInterruptState[nInterruptVector].nISTStackSize == 0)
  {
    pInterruptData->pInterruptState[nInterruptVector].nISTStackSize = DAL_IST_STACK_SIZE;
  }
  
  #ifdef DALINTERRUPT_MPM_WAKEUP

  /*
   * Configure the interrupt as a wakeup source if necessary.
   */
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_MPM))
  { 
    SetWakeupInterruptTrigger(nInterruptVector,nTrigger);  
    mpmint_config_wakeup(
      (mpmint_isr_type)pInterruptData->pInterruptState[nInterruptVector].nMPMID, 
      MPMINT_MODEM);
  }
  #endif /* DALINTERRUPT_MPM_WAKEUP */

  pInterruptData->pInterruptState[nInterruptVector].Isr = isr;
  pInterruptData->pInterruptState[nInterruptVector].nParam = ctx;

  if(SetQURTTriggerConfig(pclientCtxt->pInterruptControllerDevCtxt, nInterruptVector,nTrigger) == DAL_ERROR)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "InterruptController_RegisterISR : Could not set Interrupt trigger");
  }
  if((OldIsr != NULL) && GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_SUCCESS;
  }
  SetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);

  if(ConfigureDynamicIST(nInterruptVector,nInterruptPriority, pISTName) == DAL_ERROR)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: could not t spawn an IST thread for interrupt registration.");
    ClearInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }
 
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_RegisterInterrupt */


/*==========================================================================

  FUNCTION      InterruptController_Unregister
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_Unregister
( 
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector
) 
{
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt data. Could not Unregister Interrupt");
    return DAL_ERROR;
  }

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Failed to unregister : Interrupt was not registered");

    /*
     * If the interrupt is not registered we clean up our state in any case just as a precaution.
     */
  pInterruptData->pInterruptState[nInterruptVector].Isr = NULL;
  pInterruptData->pInterruptState[nInterruptVector].nParam = 0;
  pInterruptData->pInterruptState[nInterruptVector].nTrigger = 0;
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }

  /* 
   * Deregister IST from QURT so it can stop waiting for an interrupt.
   */
  nStatus = qurt_interrupt_deregister(nInterruptVector); 

  if (QURT_EOK != nStatus) 
  { 
     DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: QURT Could not Unregister Interrupt"); 
  } 

  //pInterruptData->pInterruptState[nInterruptVector].nISTRegistered = 0;

  /*
   * Only Deregister the interrupt handler after the IST stops waiting for an interrupt.
   * i.e. let the IST stop waiting for the interrupt.
   */
  pInterruptData->pInterruptState[nInterruptVector].Isr = NULL;
  pInterruptData->pInterruptState[nInterruptVector].nParam = 0;
  pInterruptData->pInterruptState[nInterruptVector].nTrigger = 0;

  #ifdef DALINTERRUPT_MPM_WAKEUP

  /*
   * De-configure the interrupt as a wakeup source if necessary.
   */
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_MPM))
  {
    mpmint_disable_wakeup(
      (mpmint_isr_type)pInterruptData->pInterruptState[nInterruptVector].nMPMID);
  }

  #endif /* DALINTERRUPT_MPM_WAKEUP */

  ClearInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);  
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_UnRegister */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDone
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptDone
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{

  /*
   * QURT does not support the InterruptDone API.
   */
  return DAL_ERROR;

} /* END InterruptController_InterruptDone */


/*==========================================================================

  FUNCTION      InterruptController_InterruptEnable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptEnable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
 
  /* 
   * QURT does not support enabling the interrupt separately.
   */
  return DAL_ERROR;

} /* END InterruptController_InterruptEnable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDisable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptDisable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  
 /*
  *  QURT does not support disabling the interrupt separately.
  */
  return DAL_SUCCESS;

} /* END InterruptController_InterruptDisable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptTrigger
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not Disable it.");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  qurt_interrupt_raise((unsigned int) nInterruptVector); 
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_InterruptClear
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptClear
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not clear it.");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (QURT_EOK != qurt_interrupt_clear(nInterruptVector))
  {
     DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Could not clear interrupt in QURT");
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptClear */


/*==========================================================================

  FUNCTION      InterruptController_InterruptStatus
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_InterruptStatus
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{
  DALResult nResult;
  
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt; 
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not get its status.");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (QURT_EOK != qurt_interrupt_status(nInterruptVector, &nStatus))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not get its status.");
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }
  if(nStatus == 1)
  {
      nResult = DAL_INTERRUPT_SET;
  }
  else
  {
    nResult = DAL_ERROR;
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return nResult;

} /* END InterruptController_InterruptStatus */


/*==========================================================================

   FUNCTION      SetQURTTriggerConfig

  DESCRIPTION   This function sets the trigger for the interrupt vector
                using the QURT OS apis.

  PARAMETERS    
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
static DALResult SetQURTTriggerConfig
(
  InterruptControllerDevCtxt *pDevice,
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
       return DAL_ERROR;    
  }

  /*
  * Change the interrupt trigger only when the interrupt is not registered.
  */
  if (QURT_EOK != qurt_interrupt_set_config(nInterruptVector, nDetectType, nPolarity))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Qurt Could not set the trigger");
    return DAL_ERROR;
  } 
  return DAL_SUCCESS;

} /* SetQURTTriggerConfig */


/*==========================================================================

   FUNCTION      SetTrigger

  DESCRIPTION   This function sets the trigger for the interrupt vector

  PARAMETERS    
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
static DALResult SetTrigger
(
  InterruptControllerDevCtxt *pDevice,
  uint32 nInterruptVector,
  uint32 nTrigger
)
{
  int nStatus;

  nTrigger = nTrigger & DAL_INTERRUPT_TRIGGER_MASK;

  /*
   * Change the interrupt trigger only before the interrupt is registered by the 
   * client or in the interrupt handler of the interrupt.
   * In the thread handler if the trigger is changed we need to register the 
   * interrupt change the trigger and register the interrupt again. 
   */
  if((GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)) && 
  (pInterruptData->pInterruptState[nInterruptVector].nThreadID == qurt_thread_get_id()))
  {
    nStatus = qurt_interrupt_deregister(nInterruptVector); 
    if (QURT_EOK != nStatus) 
    { 
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "Interrupt Controller:Could not set the trigger");
    } 
    if(SetQURTTriggerConfig(pDevice,nInterruptVector,nTrigger)==DAL_ERROR)
  {
    return DAL_ERROR;
  }
    rex_set_sigs(pInterruptData->pInterruptState[nInterruptVector].pTCB, 
          TASK_START_SIG);
  }
  else if(!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {

 /*
  * Change the interrupt trigger only when the interrupt is not registered.
  */
    if(SetQURTTriggerConfig(pDevice,nInterruptVector,nTrigger)==DAL_ERROR)
    {
      return DAL_ERROR;
    } 
  }
 
  pInterruptData->pInterruptState[nInterruptVector].nTrigger = nTrigger;

  return DAL_SUCCESS;

} /* SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_SetTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_SetTrigger
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32 nTrigger
)
{
  DALResult nResult;
  InterruptControllerDevCtxt *pDevice;
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not set the trigger for it");
    return DAL_ERROR;
  }

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  nResult = SetTrigger(pDevice, nInterruptVector,nTrigger);
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  #ifdef DALINTERRUPT_MPM_WAKEUP
  SetWakeupInterruptTrigger(nInterruptVector,nTrigger);  
  #endif /* if DALINTERRUPT_MPM_WAKEUP*/

  return nResult;

} /* END InterruptController_SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not get its pending status.");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (QURT_EOK != qurt_interrupt_status(nInterruptVector, &nStatus))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not get its pending status.");
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    *bState = 0;
    return DAL_ERROR;
  }
  if((nStatus == 1)&&(GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)))
  {
    *bState = 1;
  }
  else
  {
    *bState = 0;
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptEnabled
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsInterruptEnabled
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not get its enabled status.");
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  *bState = GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptEnabled */

/*==========================================================================

  FUNCTION      InterruptController_MapWakeupInterrupt
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_MapWakeupInterrupt
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32 nWakeupIntID
)
{
  #ifdef DALINTERRUPT_MPM_WAKEUP
  uint32 nInterruptTrigger;
  InterruptControllerDevCtxt *pDevice;

  
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;

  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt. Could not map it as an MPM interrupt.");
    return DAL_ERROR;
  }

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  pInterruptData->pInterruptState[nInterruptVector].nMPMID = nWakeupIntID;                      
  SetInterruptFlag(nInterruptVector, INTERRUPTF_MPM);  
  InterruptController_GetInterruptTrigger(pclientCtxt, nInterruptVector,&nInterruptTrigger);                                                        
  (void)SetWakeupInterruptTrigger(nInterruptVector, nInterruptTrigger);
             
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    mpmint_config_wakeup(
      (mpmint_isr_type) pInterruptData->pInterruptState[nInterruptVector].nMPMID, 
      MPMINT_MODEM);
  }                   
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
 
  #endif /* DALINTERRUPT_MPM_WAKEUP */

  return DAL_SUCCESS;


} /* END InterruptController_MapWakeupInterrupt */


/*==========================================================================

  FUNCTION      InterruptController_IsAnyInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsAnyInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  uint32* bState
)
{
  uint32 nIdx;
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  *bState = 0;

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  for(nIdx = 0; nIdx < MAX_QURT_IRQ ; nIdx ++)
  {
    if(GetInterruptFlag(nIdx, INTERRUPTF_REGISTERED))
    {
      if (QURT_EOK != qurt_interrupt_status(nIdx, &nStatus))
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
        "Interrupt Controller: Invalid interrupt. Could not get its pending status.");
        DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
        return DAL_ERROR;
      }
      if (nStatus == 1) 
      {
        *bState = 1;
        break;
      }
    }
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;


} /* END InterruptController_IsAnyInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_Sleep
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Sleep
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{
  /* Nothing to be done for L2VIC*/

  return DAL_SUCCESS;

} /* END InterruptController_Sleep */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_GetInterruptTrigger
(
  InterruptControllerClientCtxt *pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* eTrigger
)
{
  uint32 nPolarity,nDetectType;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(eTrigger ==NULL) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Invalid interrupt data Could not get the trigger");
    return DAL_ERROR;
  }
  qurt_interrupt_get_config((unsigned int)nInterruptVector,
    (unsigned int *)&nDetectType, (unsigned int *)&nPolarity);

  if((nPolarity == 1)&&(nDetectType == 1))
  {
    *eTrigger = DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
  }
  else if((nPolarity == 0)&&(nDetectType == 0))
  {
    *eTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
  }
  else if((nPolarity == 0)&&(nDetectType == 1))
  {
    *eTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
  }
  else if((nPolarity == 1)&&(nDetectType == 0))
  {
    *eTrigger = DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
  }
  else
  {
    return DAL_ERROR;    
  }

  return DAL_SUCCESS;

} /* END InterruptController_GetInterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_RestoreWakeupConfigs
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Wakeup
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{

  /* Nothing to be done for L2VIC*/

  return DAL_SUCCESS;

}/* END InterruptController_Wakeup */


/*=============================================================================

  FUNCTION      GetPlatformData

  DESCRIPTION   This function queries DALSYS to get platform specific information
  for the interrupt Controller GPIO interrupt. The interrupt is disabled.

  PARAMETERS     
  InterruptControllerClientCtxt * : The client context pointer of the interrupt 
    controller DAL.

  DEPENDENCIES  None.

  RETURN VALUE  
  DALResult.  : 
  DAL_ERROR : 
  If the interrupt controller was not able to query the target specific information.
  DAL_SUCCESS :
  Returns a success if all relevant data was 

  SIDE EFFECTS  None.

=============================================================================*/

static DALResult 
GetPlatformData
(
  InterruptControllerClientCtxt * pclientCtxt
)
{
  InterruptControllerDevCtxt *pDevice;
  qurt_mem_region_t      sQmemRegion;
  qurt_mem_region_attr_t sQmemAttr;
  qurt_mem_pool_t        sPool;
  int nStatus;
  uint32 nIdx,nIntIdx;
  InterruptConfigType *pIRQconfigs;

  /* 
   * Structure which holds properties 
   */
  DALSYSPropertyVar system_property_var;
  
  /*
   *  Declare property handle variable 
   */
  DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property); 
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  
  /* 
   * Get the property handle 
   */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_INTERRUPTCONTROLLER,interrupt_property); 
  if(DALSYS_GetPropertyValue(interrupt_property,"INTERRUPT_CONFIG_DATA",
                           0,&system_property_var) == DAL_SUCCESS)
  {
    if(DALSYS_Malloc(sizeof(InterruptDataType),(void **)&pInterruptData) == DAL_ERROR)
    {
       DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: could not allocate data for InterruptController");
      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    DALSYS_memset(pInterruptData, 0, sizeof(InterruptDataType));
    pInterruptData->pPlatformConfig  = (InterruptPlatformDataType *)system_property_var.Val.pdwVal;
  }
  else
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: could not get configuration data for Device");
    return DAL_ERROR;
  }
  if(pInterruptData->pPlatformConfig == NULL)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: Target Config data is corrupt.");
    return DAL_ERROR;
  }  
  if(pInterruptData->pPlatformConfig->nMaxIRQ == 0)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: could not get Interrupts supported for this Device");
    return DAL_ERROR;
  }
  else
  {
    if(DALSYS_Malloc((sizeof(InterruptStateType)*pInterruptData->pPlatformConfig->nMaxIRQ),
                               (void **)&pInterruptData->pInterruptState) == DAL_ERROR)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: could not allocate state data for InterruptController");
      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    DALSYS_memset(pInterruptData->pInterruptState, 0, 
      (sizeof(InterruptStateType)*pInterruptData->pPlatformConfig->nMaxIRQ));
  }
  
  pInterruptData->pClientCtxt = pclientCtxt;

  if(INTERRUPT_LOG_ENTRIES)
  {
    pInterruptData->pLog.nLogSize = INTERRUPT_LOG_ENTRIES;

    /*
     * Create uncached memory for interrupt log
     */
    nStatus = qurt_mem_pool_attach("DEFAULT_PHYSPOOL", &sPool);
    if (QURT_EOK != nStatus)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Unable to attach to default memory pool");
      return DAL_ERROR;
    }
    qurt_mem_region_attr_init( &sQmemAttr );
    sQmemAttr.mapping_type = QURT_MEM_MAPPING_VIRTUAL;
    qurt_mem_region_attr_set_cache_mode(&sQmemAttr, QURT_MEM_CACHE_NONE_SHARED);
    nStatus = qurt_mem_region_create(
        &sQmemRegion, 
        INTERRUPT_LOG_ENTRIES * sizeof(InterruptLogEntryType), 
        sPool, 
        &sQmemAttr);
    if (QURT_EOK != nStatus)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Unable to create uncached memory for DALint log!");
      return DAL_ERROR;
    }

    /*
     * Get the allocation address
     */
    qurt_mem_region_attr_get(sQmemRegion, (qurt_mem_region_attr_t *) &sQmemAttr);
    pInterruptData->pLog.pEntries = (InterruptLogEntryType*) sQmemAttr.virtaddr;
  }

  /* 
   * We first default to a median priority and predefined stack size.
   */
  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  { 
    pInterruptData->pInterruptState[nIdx].nPriority = INTERRUPT_THREAD_MEDIAN_PRIORITY;
    pInterruptData->pInterruptState[nIdx].nISTStackSize = DAL_IST_STACK_SIZE;
  }

  /* 
   * Here we setup the lookup table for the BSP. This is only for preconfiguration 
   * puposes for legacy targets.
   */
  if(pInterruptData->pPlatformConfig->pIRQConfigs != NULL)
  {
    pIRQconfigs = pInterruptData->pPlatformConfig->pIRQConfigs;
   
    /*
     * Go over supported interrupts from the Interrupt Config Data and populate 
     * the existing table
     */
    for(nIdx = 0; pIRQconfigs[nIdx].nInterruptVector != INVALID_INTERRUPT; nIdx ++)
    {     
      nIntIdx = pIRQconfigs[nIdx].nInterruptVector;
      pInterruptData->pInterruptState[nIntIdx].nISTStackSize = pIRQconfigs[nIdx].nISTStackSize;
      pInterruptData->pInterruptState[nIntIdx].pInterruptName = pIRQconfigs[nIdx].pInterruptName;
      if(pIRQconfigs[nIdx].nPriority !=0)
      {
        pInterruptData->pInterruptState[nIntIdx].nPriority = pIRQconfigs[nIdx].nPriority;
      }
    }
  }

  return DAL_SUCCESS;


} /* END InterruptController_GetPlatformData */


/*==========================================================================

  FUNCTION      SetWakeupInterruptTrigger

  DESCRIPTION   This function configures the interrupt trigger for the 
                relevant MPM interrupt.

  PARAMETERS    IntID   - The interrupt whose isr is needed.
                nTrigger -  The trigger to be set.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  See implementation header file.


==========================================================================*/
#ifdef DALINTERRUPT_MPM_WAKEUP
static DALResult SetWakeupInterruptTrigger
(
  uint32 nInterrupt,
  uint32 nTrigger
)
{

  mpmint_detect_type   mao_int_detect_type;
  mpmint_polarity_type mao_nPolarity_type;

  /*
   * If this interrupt has not been mapped from the MPM then return an ERROR.
   */
  if (!GetInterruptFlag(nInterrupt, INTERRUPTF_MPM))
  {
    return DAL_ERROR;
  }

  switch (nTrigger)
  {
    default:
    return DAL_ERROR;

    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      mao_int_detect_type = MPMINT_LEVEL_DETECT;
      mao_nPolarity_type = MPMINT_ACTIVE_HIGH;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      mao_int_detect_type = MPMINT_LEVEL_DETECT;
      mao_nPolarity_type = MPMINT_ACTIVE_LOW;
      break;

    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      mao_int_detect_type = MPMINT_EDGE_DETECT;
      mao_nPolarity_type = MPMINT_ACTIVE_HIGH;
      break;

    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      mao_int_detect_type = MPMINT_EDGE_DETECT;
      mao_nPolarity_type = MPMINT_ACTIVE_LOW;
      break;
  }

  /*
   * Configure MAO.
   */
  mpmint_config_int(
    (mpmint_isr_type)pInterruptData->pInterruptState[nInterrupt].nMPMID,
    mao_int_detect_type,
    mao_nPolarity_type);

  return DAL_SUCCESS;
 
} /* END SetWakeupInterruptTrigger */

#endif /* DALINTERRUPT_MPM_WAKEUP */

/*==========================================================================

  FUNCTION      GetInterruptFlag

==========================================================================*/

static boolean GetInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  return (pInterruptData->pInterruptState[nInterrupt].nFlags & nFlag) ? TRUE : FALSE;

} /* END GetInterruptFlag */


/*==========================================================================

  FUNCTION      SetInterruptFlag

==========================================================================*/

static void SetInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  pInterruptData->pInterruptState[nInterrupt].nFlags |= nFlag;

} /* END SetInterruptFlag */


/*==========================================================================

  FUNCTION      ClearInterruptFlag

==========================================================================*/

static void ClearInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  pInterruptData->pInterruptState[nInterrupt].nFlags &= ~nFlag;

} /* END ClearInterruptFlag */

/*==========================================================================

  FUNCTION      LogEvent

==========================================================================*/

static void LogInterruptEvent
(
  InterruptControllerClientCtxt  *pclientCtxt,
  InterruptLogEventType          eEvent,
  uint32                         nInterrupt
)
{
  uint64 nCount;
  atomic_plain_word_t nIdx;
  if(pInterruptData->pLog.nLogSize)
  {

    /*
     * Obtain log index from an atomic operation
     * Note: log size must be power of 2 for wrapping aound to zero to work.
     */
    nIdx = atomic_inc_return(&pInterruptData->pLog.AtomicIdx);
    nIdx &= (INTERRUPT_LOG_ENTRIES-1);

    /*
     * Update log entry
     */
    pInterruptData->pLog.pEntries[nIdx].eEvent = eEvent;
    pInterruptData->pLog.pEntries[nIdx].nInterrupt = nInterrupt;
    DalTimetick_GetTimetick64(pInterruptData->pTimetickHandle, &nCount);
    pInterruptData->pLog.pEntries[nIdx].pInterruptName = pInterruptData->pInterruptState[nInterrupt].pInterruptName;
    pInterruptData->pLog.pEntries[nIdx].nTimeStamp = nCount;
    pInterruptData->pLog.pEntries[nIdx].pcycles = qurt_get_core_pcycles();

    /* 
     * A static internal copy is kept for Interrupt Log cmm scripts. 
     */
    pInterruptData->pLog.nIdx = nIdx;
  }

} /* END LogEvent */


/*==========================================================================

  FUNCTION      TriggerClientEvent

==========================================================================*/
static void TriggerClientEvent
(
  uint32 nInterruptVector,
  InterruptControllerClientCtxt *pClientCtxt
)
{
  uint64                     nCount;
  DALISRCtx                  ClientParam;
  InterruptControllerDevCtxt *pDevice; 
  DALISR                     ClientIsr; 

  pDevice = pClientCtxt->pInterruptControllerDevCtxt;

  if (pInterruptData->pInterruptState[nInterruptVector].Isr == NULL)
  {
    if(GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
    {

      /*
       * The Interrupt Controller IST thread should never be called for a 
       * fired interrupt that has no handler.
       */
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: Interrupt does not have a client registered");
      return;
    }
    else
    {
      /*
       * Somehow the InterruptController state is messed up . 
       * We need to deregister the interrupt.
       */
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "Interrupt Controller: Interrupt does not have a client registered");
      LogInterruptEvent(pClientCtxt,INTERRUPT_LOG_EVENT_UNHANDLED, nInterruptVector);
      InterruptController_Unregister(pClientCtxt,nInterruptVector);
      return;
    }
  }
  ClientIsr = pInterruptData->pInterruptState[nInterruptVector].Isr;
  ClientParam = pInterruptData->pInterruptState[nInterruptVector].nParam;
 
  /*
   * Set the timetest ID to the executing ISR.
   */
  #ifdef TIMETEST
  #error code not present
#endif
  /*
   * Set the running flag.
   */
  SetInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING);

  DALSYS_SyncLeave(pClientCtxt->hSyncIntrCtrlTbl);
 
  ClientIsr(ClientParam);

  /*
   * Re-lock  if we unlocked.  Use local flag since the global flag may
   * have been cleared during ISR execution.
   */
  DALSYS_SyncEnter(pClientCtxt->hSyncIntrCtrlTbl); 
 
  /*
   * Clear the running flag.
   */
  ClearInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING);

  /*
   * Increase debug count.
   */
  pInterruptData->pInterruptState[nInterruptVector].nCount ++;
  DalTimetick_GetTimetick64(pInterruptData->pTimetickHandle, &nCount);
  pInterruptData->pInterruptState[nInterruptVector].nLastFired = nCount;

} /* END TriggerClientEvent */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptID

==========================================================================*/
DALResult InterruptController_GetInterruptID
(
  InterruptControllerClientCtxt  *pclientCtxt,
  const char* szIntrName,
  uint32* pnIntrID
)
{
  uint32 nIdx;
  DALResult nResult;

  nResult = DAL_ERROR;
  *pnIntrID = INVALID_INTERRUPT;
  for (nIdx=0;nIdx < pInterruptData->pPlatformConfig->nMaxIRQ;nIdx++) 
  {
    if (strcmp(szIntrName, pInterruptData->pInterruptState[nIdx].pInterruptName) == 0)
    {
      nResult = DAL_SUCCESS;
      *pnIntrID = nIdx;
      break;
    }
  }
  return nResult;
} /* END InterruptController_GetInterruptID */


/*==========================================================================

  FUNCTION      InterruptController_LogState

==========================================================================*/
DALResult InterruptController_LogState
(
  InterruptControllerClientCtxt  *pclientCtxt,
  void *pULog
)
{
  uint32 nIdx,nTrigger;
  int bIsEnabled,bIsFired;

  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  {
    qurt_interrupt_status(nIdx, &bIsFired); 
    bIsEnabled = GetInterruptFlag(nIdx, INTERRUPTF_REGISTERED);
    if ((bIsFired != 0)||(bIsEnabled != 0)) 
    {
      InterruptController_GetInterruptTrigger(pclientCtxt,nIdx,&nTrigger);
      ULOG_RT_PRINTF_4(pULog,"Interrupt Vector : %d Enable state %d Fired State %d Trigger type  %d ",nIdx,bIsEnabled,
                     bIsFired,nTrigger);
    }
    
  }

  return DAL_SUCCESS;

} /* END InterruptController_LogState */


