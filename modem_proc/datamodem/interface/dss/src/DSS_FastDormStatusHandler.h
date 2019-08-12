#ifndef __DSS_FASTDORMSTATUSHANDLER_H__
#define __DSS_FASTDORMSTATUSHANDLER_H__

/*====================================================

FILE:  DSS_FastDormStatusHandler.h

SERVICES:
   Handle fast dormancy status updates.
=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_FastDormStatusHandler.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2011-05-20 mct History added.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSFastDormStatusHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSFastDormStatusHandler* CreateInstance();
   DSSFastDormStatusHandler();

};

#endif // __DSS_FASTDORMSTATUSHANDLER_H__
