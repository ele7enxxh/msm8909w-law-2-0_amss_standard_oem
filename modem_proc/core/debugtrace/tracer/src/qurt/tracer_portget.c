/*===========================================================================
  FILE: tracer_portget.c

  OVERVIEW: Get a STM port from the port pool. Extension of tracer_portmgr.c

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_atomic.h"
#include "tracer_portmgr.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

//=========================================================================//
//Caller can check that port manager is initialized before calling this
//function.
STMTracePortType tracer_port_get(int *key)
{
   int myCount = 0;  //Escape route.
   int myKey;
   int newKey;

   if (TRUE == bPortMgrInitialized) // Additional safeguard. Needed?
   {
      do
      {
         myCount++;
         if (myCount > TRACER_PORT_POOL_SIZE)
         {  // Get out if can't find a port
            *key = ~0;
            return (_gPortMgr.trashPort);
         }

         // Get next port from pool.
         do
         {
            myKey = _gPortMgr.nextKey;
            newKey = ((myKey + 1) >= TRACER_PORT_POOL_SIZE) ?
               0 : (myKey + 1);

         } while (!TRACER_ATOMIC_CAS(&_gPortMgr.nextKey, myKey, newKey));

      // Verify the port is not already in use.
      } while (!TRACER_ATOMIC_CAS(&(_gPortMgr.pool[myKey].inUseFlag), 0, 1));

      // Track usage
      TRACER_ATOMIC_INC (&(_gPortMgr.inUseCount));
      if (_gPortMgr.inUseHiMark < _gPortMgr.inUseCount)
      {
         TRACER_ATOMIC_SET (&(_gPortMgr.inUseHiMark),_gPortMgr.inUseCount);
      }

      *key = myKey;  // Return key value
      return (_gPortMgr.pool[*key].port); // Return port number.
   }
   else
   {
      *key = ~0;
      return 0;
   }
}

//=========================================================================//
void tracer_port_free(int key)
{
   if (~0 != key)
   {
      TRACER_ATOMIC_SET (&(_gPortMgr.pool[key].inUseFlag), 0);
      TRACER_ATOMIC_DEC (&(_gPortMgr.inUseCount));
   }
   return;
}

