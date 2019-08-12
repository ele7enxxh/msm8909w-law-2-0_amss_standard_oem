#ifndef __DSS_MCASTREGISTERHANDLER_2_0_H__
#define __DSS_MCASTREGISTERHANDLER_2_0_H__
#ifdef FEATUTE_DATA_PS_MCAST
/*====================================================

FILE:  DSS_MCastRegisterHandler_2_0.h

SERVICES:
Handle MCast Registration BCMCS rev 2.0 events.

=====================================================

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCastRegisterHandler_2_0.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-29 vm  Module created.

===========================================================================*/

#include "DSS_EventHandlerMCast.h"

class DSSMCastRegisterHandler2_0 : public DSSEventHandlerMCast
{
protected:

   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:

   DSSMCastRegisterHandler2_0();
   static DSSMCastRegisterHandler2_0* CreateInstance();

};
#endif // FEATUTE_DATA_PS_MCAST
#endif // __DSS_MCASTREGISTERHANDLER_2_0_H__
