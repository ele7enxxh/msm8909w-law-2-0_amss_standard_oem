/******************************************************************//**
 * @file dcvs_kernel.c
 *
 * @brief Kernel (OS) interface implementation for BLAST platforms
 *
 * Copyright (C) 2010 - 2013 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/os/blast/dcvs_os.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
04/10/13   nj      Compiler warnings fixed
09/11/12   nj      Removed unused header files
03/29/12   nj      Added header macros that include target specific include files 
03/29/12   nj      Adding stdlib.h header file as malloc and free are defined therein
03/27/12   nj      Replaced DALSYS_SyncEnter with npa_resource_lock and DALSYS_SyncExit with 
                   npa_resource_unlock. These APIs use the correct/required argument signatures
03/23/12   nj      Removed unused include files
03/14/12   nj      Added a macro(instead of hardcoding) startup delay value
09/12/11   ss      Removing the unused test code for making vector requests into DCVS.
04/26/11   ss      Fixing Q6 compiler warnings.
03/28/11   ss      Adding context based implementation.
5/6/10     ss      Initial checkin
========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "dcvs_os.h"
#include "dcvs_target.h"
#include "fs_lib.h"
#include "DALSys.h"
#include "CoreVerify.h"
#include <stdlib.h>

#define MAX_NUM_DCVS_TIMERS 4

/**
 * Timer used to holdoff DCVS for a certain amount of time at 
 * initial boot-up. 
 */
#ifndef DCVS_BOOT_UP_DELAY_TIME_MS
  #define DCVS_BOOT_UP_DELAY_TIME_MS 10000
#endif



/* DCVS Target Specific callbacks */
void DCVSTargetInitialize(void);

/* Data type to maintain the initial callback for the DCVS. This call back starts the DCVS algorithm after a specified delay.
   The timer in this type is different from the running DCVS timer.  */
typedef struct
{
  rex_timer_type timer;
  /* The callback function for the timer. */
  dcvs_timer_cb timerCb;
    
} DCVS_TIMER_TYPE;

/* Data type to maintain the initial callback for the DCVS. This call back starts the DCVS algorithm after a specified delay.
   The timer in this type is different from the running DCVS timer.  */
typedef struct 
{
  rex_timer_type timer;
  rex_timer_cb_type timerCb;

} DCVS_INITIAL_CB_TYPE;

typedef struct 
{
  /* The callback type used to create the delay in starting the DCVS algorithm .*/
  DCVS_INITIAL_CB_TYPE initialCbData;
  
  /* Lock to protect the critical sections */
  DALSYSSyncHandle syncLock; 

  /* The DCVS running timers. */
  DCVS_TIMER_TYPE DCVSTimers[MAX_NUM_DCVS_TIMERS]; 

} DCVS_OS_DATA;

/* Structure to maintain the parameters for the callback of the DCVS timer. */
typedef struct
{
  /* DCVS data context */
  DCVS_CONTEXT* context;
  /* The index of the DCVS timer. */
  uint32 timerIndex;
}DCVS_TIMER_CB_PARAM_TYPE;

/* The parameters for the callback of the DCVS timer */
DCVS_TIMER_CB_PARAM_TYPE DCVSCbParams[MAX_NUM_DCVS_TIMERS];

/* The rex timer call back for the DCVS timers */
void DCVSOsTimerCb(uint32 param)
{
  DCVS_CONTEXT* context = DCVSCbParams[param].context;
  uint32 timerIndex = DCVSCbParams[param].timerIndex;
  DCVS_OS_DATA* osDataContext = context->osDataContext;

  if(osDataContext->DCVSTimers[timerIndex].timerCb != NULL)
  {
    /* Do the callback on the timerIndex */
    osDataContext->DCVSTimers[timerIndex].timerCb(context);
  }
}


/**
  @brief dcvs_initial_cb
  
  This is the callback in the kernel which is invoked after the 
  requested delay in starting the DCVS algorithm. 
  This function starts the DCVS algorithm.  

  @param param: The DCVS data context. 
   
*/

void dcvs_initial_cb(uint32 param)
{
  /* Obtain the DCVS data context from the passed in parameter.  */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)param;
  DCVS_OS_DATA* osDataContext = context->osDataContext;

  (*(DCVSOSCallback)(osDataContext->initialCbData.timerCb))(context);
}


static void OsDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  uint32 i;
  DCVS_OS_DATA* osDataContext;
  /* Allocate the memory for the kernel data . */
  if(DALSYS_Malloc(sizeof(DCVS_OS_DATA),  
                   (void**) &osDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }

  /* Initialize the memory allocated for the OS data */
  memset(osDataContext, 0, sizeof(DCVS_OS_DATA));
  context->osDataContext = osDataContext;

  osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  /* Create a lock for synchronization */
  DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                    &osDataContext->syncLock, 
                    NULL);

  for(i = 0; i < MAX_NUM_DCVS_TIMERS; i++)
  {
    /* Init the blast timers . */
    osDataContext->DCVSTimers[i].timerCb = NULL;
    DCVSCbParams[i].context = NULL;
    DCVSCbParams[i].timerIndex = 0;
  }

}

void DCVSOSInitialize(DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext;
  /* Allocate and initialize the OS specific data */
  OsDataMemoryAllocateAndInitialize(context);

  osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  // ToDo: do we need all the critical sections?

  /* Define the Initial callback Timer. This timer starts the DCVS 
     algorithm after a specified delay. This is separate from the 
     running timers of DCVS. */
  rex_def_timer_ex(&osDataContext->initialCbData.timer, dcvs_initial_cb, (uint32)context);

}

void DCVSOSCallbackCreate(DCVSOSCallback cb, uint32 delay_ms, DCVS_CONTEXT *context)
{
  /* Obtain the kernel data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  if (!cb)
    return;

  /* Serialize callback creation */
  // ToDo: do we need all the critical sections?
  DALSYS_SyncEnter(osDataContext->syncLock);
  
  /* Set Callback Function. */
  osDataContext->initialCbData.timerCb = (rex_timer_cb_type)cb;

  /* Set Timer delay */
  rex_set_timer(&osDataContext->initialCbData.timer, delay_ms);

  /* Unlock and return */
  DALSYS_SyncLeave(osDataContext->syncLock);
}

void DCVSOSIdleInfo(uint32* idle_override)
{
  // Never override.
  *idle_override = 100;

  return;
}

DCVSOSTimerHandle DCVSOSTimerCreate(DCVSOSCallback cb, DCVS_CONTEXT* context)
{
   uint32 index;
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;
  for(index = 0; index < MAX_NUM_DCVS_TIMERS; index ++)
  {
    if( NULL == osDataContext->DCVSTimers[index].timerCb )
    {
      /* Setthe callback function for this timer. */
      osDataContext->DCVSTimers[index].timerCb = cb;
      DCVSCbParams[index].context = context;
      DCVSCbParams[index].timerIndex = index;
          
      /* Define the rex timer */
      rex_def_timer_ex(&osDataContext->DCVSTimers[index].timer, DCVSOsTimerCb, (uint32)index);
      return (DCVSOSTimerHandle)(index+1);
    }
  }
  return NULL;
}

DALBOOL DCVSOSTimerDestroy(DCVSOSTimerHandle Handle, DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;
  uint32 timerHandle = (uint32)Handle;
  if (0 == timerHandle)
  {
    return FALSE;
  }

  /* Validate the handle */
  if( NULL == osDataContext->DCVSTimers[timerHandle-1].timerCb ) 
  {
    return FALSE;
  }
  else
  {  
    osDataContext->DCVSTimers[timerHandle-1].timerCb = NULL;
    return TRUE;
  }
}

DALBOOL DCVSOSTimerStart(DCVSOSTimerHandle Handle, uint32 time_ms, DCVS_CONTEXT* context)
{
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  uint32 timerHandle = (uint32)Handle;

  if ((0 == (timerHandle)) ||
      (timerHandle > MAX_NUM_DCVS_TIMERS) ||
      (NULL == osDataContext->DCVSTimers[timerHandle-1].timerCb))
  {
    return FALSE;
  }
  else
  {
    /* Set the timer to expire at the given timeout*/
    rex_set_timer(&osDataContext->DCVSTimers[timerHandle-1].timer, time_ms);
    return TRUE;
  }
}

void DCVSOSTimerStop(DCVSOSTimerHandle Handle, DCVS_CONTEXT* context)
{
  uint32 timerHandle = (uint32)Handle;
  /* Obtain the OS data context. */
  DCVS_OS_DATA* osDataContext = (DCVS_OS_DATA*)context->osDataContext;

  if ((0 == (timerHandle)) ||(timerHandle > MAX_NUM_DCVS_TIMERS))
  {
    return;
  }
  /* Stop the active DCVS timer */
  rex_clr_timer(&osDataContext->DCVSTimers[timerHandle-1].timer);

}

uint32 DCVSOSTicksToMsConvert(uint32 sysTicks)
{
  /* This is a BLAST OS platform. Convert the sclks into ms */
  return( (sysTicks*1000) >> 15 );
}

uint32 DCVSOSStartUpDelay(void)
{
  /* This is a BLAST platform. Delay is as defined by macro */
  return(DCVS_BOOT_UP_DELAY_TIME_MS);
}

void DCVSOSCriticalSectionEnter(DCVS_CONTEXT* context)
{
  npa_resource_lock(context->resDataContext->resHandle);
}

void DCVSOSCriticalSectionExit(DCVS_CONTEXT* context)
{
  npa_resource_unlock(context->resDataContext->resHandle);
}

void* DCVSOSDynamicMemoryAllocate(uint32 size)
{
  return(malloc(size));
}

void DCVSOALDynamicMemoryFree(void* mptr)
{
  free(mptr);
}