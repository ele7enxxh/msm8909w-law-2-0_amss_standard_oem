#ifndef __DSS_MTPDREQUESTHANDLER_H__
#define __DSS_MTPDREQUESTHANDLER_H__

/*====================================================

FILE:  DSS_MTPDRequestHandler.h

SERVICES:
   Handle MTPD events.

=====================================================

Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MTPDRequestHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"
#include "ds_Net_ITechUMTS.h"

class DSSMTPDRequestHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();
   virtual AEEResult SetEventData(dss_iface_ioctl_event_enum_type event,
                                  bool bReg,
                                  dss_iface_ioctl_event_cb userCB,
                                  void* userData,
                                  sint15 appId);

private:
   struct DSSMTPDRequestData : public HandlerData{
      IQI* pMTPDReg;
   };

public:
   static DSSMTPDRequestHandler* CreateInstance();
   DSSMTPDRequestHandler();
   virtual void Destructor() throw();

};

#endif // __DSS_MTPDREQUESTHANDLER_H__
