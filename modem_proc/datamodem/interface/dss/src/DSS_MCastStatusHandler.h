#ifndef __DSS_MCASTSTATUSHANDLER_H__
#define __DSS_MCASTSTATUSHANDLER_H__
#ifdef FEATUTE_DATA_PS_MCAST
/*====================================================

FILE:  DSS_MCastStatusHandler.h

SERVICES:
   Handle network extended IP config events.

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCastStatusHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerMCast.h"

class DSSMCastStatusHandler : public DSSEventHandlerMCast
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:

   DSSMCastStatusHandler();
   static DSSMCastStatusHandler* CreateInstance();
};
#endif // FEATUTE_DATA_PS_MCAST
#endif // __DSS_MCASTSTATUSHANDLER_H__
