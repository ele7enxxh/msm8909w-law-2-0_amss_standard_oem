#ifndef __DSS_QOSNETINITIATEDREQHANDLER_H__
#define __DSS_QOSNETINITIATEDREQHANDLER_H__

/*====================================================

FILE:  DSS_QoSNetInitiatedReqHandler.h

SERVICES:
   Handle QoS Net Initiated Request events.

=====================================================

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSNetInitiatedReqHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-08 sz  Created.

===========================================================================*/

#include "DSS_EventHandlerQoSNetInitiatedReq.h"


class DSSQoSNetInitiatedReqHandler : public DSSEventHandlerQoSNetInitiatedReq
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSQoSNetInitiatedReqHandler* CreateInstance();
   DSSQoSNetInitiatedReqHandler();
};

#endif // __DSS_QOSNETINITIATEDREQHANDLER_H__
