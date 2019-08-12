#ifndef __DSS_QOSAWAREUNAWAREHANDLER_H__
#define __DSS_QOSAWAREUNAWAREHANDLER_H__

/*====================================================

FILE:  DSS_QoSAwareUnAwareHandler.h

SERVICES:
   Handle QoS Aware/UnAware events.

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSAwareUnAwareHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

// This handler have 2 EventData  edAware,edUnAware that are stored
// in an array in the mEdClone in EventHandler.
class DSSQoSAwareUnAwareHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSQoSAwareUnAwareHandler* CreateInstance();
   DSSQoSAwareUnAwareHandler();
};

#endif // __DSS_QOSAWAREUNAWAREHANDLER_H__
