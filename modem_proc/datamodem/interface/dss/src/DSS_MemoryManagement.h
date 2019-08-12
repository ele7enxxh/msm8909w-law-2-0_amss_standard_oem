#ifndef __DSS_MEMORY_MANAGEMENT_H
#define __DSS_MEMORY_MANAGEMENT_H

/*===================================================

FILE:  DSS_MemoryManagement.h

SERVICES:
A utility class to provide generic use of ps_scratchpad.h

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MemoryManagement.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "ps_mem.h"
#include "ds_Utils_DebugMsg.h"
#include "DSS_Globals.h"
#include "DSS_EventHandlerNetApp.h"
#include "ps_system_heap.h"


// The size is of DSSEventHandlerNetApp because there is a member added
// this is the reason we can not use DSSEventHandler to calc the size.
// DSSEventHandlerQoS, DSSEventHandlerPrivIpv6Addr, DSSEventHandlerMCast,DSSEventHandlerMCastMBMSCtrl
// also add only one pointer so the handlers that inherit from them will be the same
// size as those that inherit from DSSEventHandlerNetApp
#define EVENT_HANDLER_SIZE                  (( sizeof( DSSEventHandlerNetApp) + 3) & ~3)

/**********************************************/
/**********************************************/
/* PS_MEM */
/**********************************************/
/**********************************************/

#define PS_MEM_NEW(T)  new T
#define PS_MEM_DELETE(T) \
   if (NULL != T) {      \
     delete T;           \
     T = NULL;           \
   }

#define PS_MEM_RELEASE(T)   \
   if (NULL != T) {         \
      (void)(T)->Release(); \
      T = NULL;             \
   }

#define PS_MEM_RELEASE_WEAK(T)   \
   if (NULL != T) {              \
   (void)(T)->ReleaseWeak();     \
   T = NULL;                     \
   }


// This function should be invoked during phone power-up
// currently invoked from psi_powerup
void DSSmem_pool_init();



#endif // __DSS_MEMORY_MANAGEMENT_H
