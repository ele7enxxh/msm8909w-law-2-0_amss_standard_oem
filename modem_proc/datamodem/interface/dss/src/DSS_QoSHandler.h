#ifndef __DSS_QOSHANDLER_H__
#define __DSS_QOSHANDLER_H__

/*====================================================

FILE:  DSS_QoSHandler.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerQoS.h"

class DSSQoSHandler : public DSSEventHandlerQoS
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   DSSQoSHandler();
   static DSSQoSHandler* CreateInstance();
};

#endif // __DSS_QOSHANDLER_H__
