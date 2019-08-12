#ifndef __DSS_IDSNETWORKP1XSCOPE_H__
#define __DSS_IDSNETWORKP1XSCOPE_H__

/*===================================================

FILE:  DSS_IDSNetwork1xScope.h

SERVICES:
   A utility class to facilitate IDSNetwork1xPriv fetching
   and releasing.

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IDSNetwork1xScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_Common.h"
#include "DSS_Conversion.h"
#include "ds_Errors_Def.h"

#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_INetwork1x.h"

using namespace ds::Error;

// This class provides an abstraction over IDSNetwork1xPriv pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the IDSNetwork1xPriv pointer is valid and can be used
// until the end of the scope where the IDSNetwork1xPriv object's reference counting
// decremented.
class DSSIDSNetwork1xScope {
public:
   DSSIDSNetwork1xScope();
   ~DSSIDSNetwork1xScope();
   AEEResult Init(ds::Net::INetwork* pNetwork);
   ds::Net::INetwork1x* Fetch();

private:
   ds::Net::INetwork1x* mpIDSNetwork1x;
};

inline DSSIDSNetwork1xScope::DSSIDSNetwork1xScope() : mpIDSNetwork1x(NULL)
{
}

inline AEEResult DSSIDSNetwork1xScope::Init(ds::Net::INetwork* pNetwork)
{
   if (NULL == pNetwork) {
      return QDS_EFAULT;
   }

   AEEResult res = pNetwork->GetTechObject(ds::Net::AEEIID_INetwork1x, (void**)&mpIDSNetwork1x);
   if (res != AEE_SUCCESS) {
      LOG_MSG_ERROR_1("Failed to fetch INetwork1x object: %d", res);
      return res;
   }

   return AEE_SUCCESS;
}
inline ds::Net::INetwork1x* DSSIDSNetwork1xScope::Fetch()
{
   return mpIDSNetwork1x;
}

/*lint -e{1551} */
inline DSSIDSNetwork1xScope::~DSSIDSNetwork1xScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetwork1x);
}
/*lint –restore */


#endif // __DSS_IDSNETWORKP1XSCOPE_H__
