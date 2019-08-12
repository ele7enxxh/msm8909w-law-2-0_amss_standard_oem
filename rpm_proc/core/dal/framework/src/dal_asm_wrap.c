/*==============================================================================
  Copyright (c) 2008 Qualcomm Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/rpm.bf/2.1.1/core/dal/framework/src/dal_asm_wrap.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
12/11/08   ML      Implemented Atomic Ops and Move cache control to boot\dalmodenv.c
                   Implemented simulation support
==============================================================================*/
#include "DALFramework.h"
#include "dalconfig.h"

// Override for simulation with here
#ifdef WINSIM
#undef DAL_CONFIG_ARCH
#define DAL_CONFIG_ARCH DAL_CONFIG_ARCH_WINSIM
#endif

/*
   Wrapper around asm implemented functions
*/


// Declare the following internally as they are not available fo
// all architectutes
/*
   LockedGetW
   Tags the address pTarget and retrieves the value pointed by pTarget
*/
uint32 DALFW_LockedGetW(volatile uint32 * pTarget);

/*
   LockedConditionalSetW
   conditionaly sets the "value" of the address pointed by pTarget:
   - if no other other address had been tagged by the current processor and
   - if no other processor has modified *pTarget then this function succeeds
   and has successfully stored the word "value" in *pTarget
*/
DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget,  uint32 value);

void DALFW_memcpy(void * pDest, const void * pSrc, uint32 iLen)
{

#if (DAL_CONFIG_ARCH==DAL_CONFIG_ARCH_QDSP6_V2)
   uint32 i = 0; char * pD = (char*)pDest; char * pS = (char*)pSrc;
   for(;i<iLen; i++, pD++, pS++) 
      *pD = *pS; 

#else
   (void)memcpy(pDest, pSrc, iLen);
#endif

}

void DALFW_memset(void * pDest, int iVal, uint32 iLen)
{

#if (DAL_CONFIG_ARCH==DAL_CONFIG_ARCH_QDSP6_V2)
   uint32 i = 0; char * ptr = (char*)pDest;
   for(; i < iLen; i++, ptr++) 
      *ptr = iVal; 
#else
   (void)memset(pDest, iVal, iLen);
#endif

}


#if (DAL_CONFIG_ARCH & DAL_CONFIG_ARCH_ARM) && (DAL_CONFIG_ARCH <= DAL_CONFIG_ARCH_ARM_V6)
extern uint32 DALFW_LockedSwapW(volatile uint32 *pTarget, uint32 value);
#endif

#if ((DAL_CONFIG_ARCH <= DAL_CONFIG_ARCH_ARM_V5) || (DAL_CONFIG_ARCH == DAL_CONFIG_ARCH_WINSIM))
static DALSYSSyncHandle hSync = NULL;

static void SyncCreateEnter()
{
   if(hSync == NULL)
      DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION, &hSync, NULL);
    DALSYS_SyncEnter(hSync);
}
#endif


/*------------------------------------------------------------------------*/

#if (DAL_CONFIG_ARCH & DAL_CONFIG_ARCH_QDSP)
#include "./q6/DALFwkQdspUtils.h"

uint32 DALFW_LockedGetW(volatile uint32 *pTarget)
{
   uint32 retVal;
   DALFWK_QDSP_UTILS_LOCKED_GETW(retVal,pTarget);
   return retVal;
}

DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget, uint32 value)
{
   uint32 retVal;
   DALFWK_QDSP_UTILS_LOCKED_SETW(pTarget,value,retVal);
   return (retVal?DAL_SUCCESS:DAL_ERROR);
}
#endif


/*------------------------------------------------------------------------*/
/*------------------------------WIN SIM-----------------------------------*/
/*------------------------------------------------------------------------*/
#if ((DAL_CONFIG_ARCH < DAL_CONFIG_ARCH_ARM_V5) || (DAL_CONFIG_ARCH == DAL_CONFIG_ARCH_WINSIM))
uint32 DALFW_LockedSwapW(volatile uint32 *pTarget, uint32 value)
{
   uint32 prevValue;

   SyncCreateEnter();
   prevValue = *pTarget;
   *pTarget = value;
   DALSYS_SyncLeave(hSync);
   return prevValue;
}
#endif

/*------------------------------------------------------------------------*/
/*------------------------------ARM v5/SIM------------------------------------*/
/*------------------------------------------------------------------------*/
#if ((DAL_CONFIG_ARCH <= DAL_CONFIG_ARCH_ARM_V5) || (DAL_CONFIG_ARCH == DAL_CONFIG_ARCH_WINSIM))

uint32 DALFW_LockedGetW(volatile uint32 * pTarget)
{
   return *pTarget;
}


DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget,  uint32 value)
{
   *pTarget = value;
   return DAL_SUCCESS;
}


uint32 DALFW_LockedExchangeW(volatile uint32 *pTarget, uint32 value)
{
   return DALFW_LockedSwapW( pTarget, value );
}

uint32 DALFW_LockedIncrementW(volatile uint32 *pTarget)
{
   uint32 newval;

   SyncCreateEnter();
   newval = ++(*pTarget);
   DALSYS_SyncLeave(hSync);
   return newval;
}

uint32 DALFW_LockedDecrementW(volatile uint32 *pTarget)
{
   uint32 newval;

   SyncCreateEnter();
   newval = --(*pTarget);
   DALSYS_SyncLeave(hSync);
   return newval;
}

uint32 DALFW_LockedGetModifySetW(volatile uint32 *pTarget, uint32 AND_mask, uint32 OR_value)
{
   uint32 newval;

   SyncCreateEnter();
   newval = (*pTarget & AND_mask) | OR_value;
   *pTarget = newval;
   DALSYS_SyncLeave(hSync);
   return newval;
}

uint32 DALFW_LockedCompareExchangeW(volatile uint32 *pTarget, uint32 comparand, uint32 value)
{
   SyncCreateEnter();
   if(*pTarget == comparand)
      *pTarget = value;
   else
      value = *pTarget;   // previous value
   DALSYS_SyncLeave(hSync);
   return value;
}

#else
/*------------------------------------------------------------------------*/
/*--------------------All except ARM v5 & WINSIM--------------------------*/
/*------------------------------------------------------------------------*/
uint32 DALFW_LockedExchangeW(volatile uint32 *pTarget, uint32 value)
{
   uint32 prevValue;
   do
   {
      prevValue = DALFW_LockedGetW(pTarget);
   }
   while (DALFW_LockedConditionalSetW(pTarget, value)!=DAL_SUCCESS);
   return prevValue;
}


uint32 DALFW_LockedIncrementW(volatile uint32 *pTarget)
{
   uint32 prevValue;
   do
   {
      prevValue = DALFW_LockedGetW(pTarget);
   }
   while (DALFW_LockedConditionalSetW(pTarget, ++prevValue)!=DAL_SUCCESS);
   return prevValue;
}


uint32 DALFW_LockedDecrementW(volatile uint32 *pTarget)
{
   uint32 prevValue;
   do
   {
      prevValue = DALFW_LockedGetW(pTarget);
   }
   while (DALFW_LockedConditionalSetW(pTarget, --prevValue)!=DAL_SUCCESS);
   return prevValue;
}


uint32 DALFW_LockedGetModifySetW(volatile uint32 *pTarget, uint32 AND_mask, uint32 OR_value)
{
   uint32 prevValue;
   do
   {
      prevValue = DALFW_LockedGetW(pTarget);
      prevValue = (prevValue & AND_mask) | OR_value;
   }
   while (DALFW_LockedConditionalSetW(pTarget, prevValue)!=DAL_SUCCESS);
   return prevValue;
}

uint32 DALFW_LockedCompareExchangeW(volatile uint32 *pTarget, uint32 comparand, uint32 value)
{
   uint32 prevValue;
   do
   {
      prevValue = DALFW_LockedGetW(pTarget);
      if(prevValue != comparand)
      {
         // untag it using original value and return
         DALFW_LockedConditionalSetW(pTarget, prevValue);
         return prevValue;
      }
      // try setting to new value
   }
   while (DALFW_LockedConditionalSetW(pTarget, value)!=DAL_SUCCESS);
   return value;
}

#endif



/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/
/*-- Semi-Raw Inter CPU (i.e External shared) spinlocks ----------------------------*/
/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/

/*
   Keep in mind these are not raw-spin locks and intended for very low contention
   locks as while these locks are being held the running thread may be interrupted
   which will cause the other processor to loop for an undetermined amount of time.
   For such scenarios it may be necessary that instead the "Try" implementations
   be used conjunction of OS yield/sleep primitives as when used via DALGlbCtxt.

   TEST_LOCK_COUNT controls how often the test access to the shared memory occurs.
   The value must always be a power of 2.
      Use 2 to force test each time.
   This value is especially important when shared memory is Strongly-Ordered as it
   will behave as a memory barrier.
*/
#define TEST_LOCK_COUNT    64

// All our ARM9 & ARM11 Use Swap
#if ((DAL_CONFIG_ARCH & DAL_CONFIG_ARCH_ARM) && (DAL_CONFIG_ARCH <= DAL_CONFIG_ARCH_ARM_V6))

void DALFW_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt=0;
   for(;;)
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0)   //time to test?
      {
         if(0 == *spinLock)               //is the lock currently available?
         {
            if( DALFW_LockedSwapW(spinLock, pid) == 0 ) // try and grab it
               break;
         }
      }
      cnt++;
   }
}

DALResult DALFW_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt;
   for(cnt=0; cnt<=TEST_LOCK_COUNT; cnt++) //always tries twice
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0)   //time to test?
      {
         if(0 == *spinLock)             //is the lock currently available?  
         {
            if( DALFW_LockedSwapW(spinLock, pid) == 0 )  // try and grab it  
               return DAL_SUCCESS;
         }
      }
   }
   return DAL_ERROR;
}

void DALFW_UnLockSpinLockExt(volatile uint32 *spinLock)
{
   *spinLock = 0;     // Use store instead of Atomic Operation 
   //DALFW_LockedSwapW(spinLock, 0);
}


/* raw implementations of the above */

void _raw_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   while ( DALFW_LockedSwapW(spinLock, pid) )
	   ;
}

void _raw_UnLockSpinLockExt(volatile uint32 *spinLock)
{
    DALFW_LockedSwapW(spinLock, 0);
}

DALResult _raw_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   return ((DALFW_LockedSwapW(spinLock, pid)==0)?DAL_SUCCESS:DAL_ERROR);
}



/*--------------------------- Rest use high level exchange-----------------------*/
#else


void DALFW_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt=0;
   for(;;)
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0) //time to test?
      {
         if(0 == *spinLock)             //is the lock currently available? 
         {
            if( DALFW_LockedExchangeW(spinLock, pid) == 0 ) // try and grab it 
               break;
         }
      }
      cnt++;
   }
}

DALResult DALFW_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt;
   for(cnt=0; cnt<=TEST_LOCK_COUNT; cnt++) //always tries twice
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0)   //time to test?
      {
         if(0 == *spinLock)             //is the lock currently available?  
         {
            if( DALFW_LockedExchangeW(spinLock, pid) == 0 )  // try and grab it  
               return DAL_SUCCESS;
         }
      }
   }
   return DAL_ERROR;
}

void DALFW_UnLockSpinLockExt(volatile uint32 *spinLock)
{
    *spinLock = 0;   // Use store instead of Atomic Operation
    //DALFW_LockedExchangeW(spinLock, 0);
}


/* raw implementations of the above */

void _raw_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   while ( DALFW_LockedExchangeW(spinLock, pid) )
	   ;
}

void _raw_UnLockSpinLockExt(volatile uint32 *spinLock)
{
    DALFW_LockedExchangeW(spinLock, 0);
}

DALResult _raw_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   return (DALFW_LockedExchangeW(spinLock, pid)==0)?DAL_SUCCESS:DAL_ERROR;
}


#endif


/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/
/*------------ Raw intra CPU (i.e NON-shared) spinlocks ----------------------------*/
/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/

void DALFW_LockSpinLock(volatile uint32 *spinLock, uint32 pid)
{
   while ( DALFW_LockedExchangeW(spinLock, pid) )
	   ;
}

void DALFW_UnLockSpinLock(volatile uint32 *spinLock)
{
    DALFW_LockedExchangeW(spinLock, 0);
}

DALResult DALFW_TryLockSpinLock(volatile uint32 *spinLock, uint32 pid)
{
   return (DALFW_LockedExchangeW(spinLock, pid)==0)?DAL_SUCCESS:DAL_ERROR;
}

