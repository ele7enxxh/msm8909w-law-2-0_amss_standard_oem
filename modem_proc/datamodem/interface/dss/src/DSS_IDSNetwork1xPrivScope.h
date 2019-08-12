#ifndef __DSS_IDSNETWORK1XPRIVSCOPE_H__
#define __DSS_IDSNETWORK1XPRIVSCOPE_H__

/*===================================================

FILE:  DSS_IDSNetwork1xPrivScope.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IDSNetwork1xPrivScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_Common.h"
#include "DSS_Conversion.h"

#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_INetwork1xPriv.h"

// This class provides an abstraction over IDSNetwork1xPriv pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the IDSNetwork1xPriv pointer is valid and can be used
// until the end of the scope where the IDSNetwork1xPriv object's reference counting
// decremented.
class DSSIDSNetwork1xPrivScope {
public:
   DSSIDSNetwork1xPrivScope();
   ~DSSIDSNetwork1xPrivScope();
   AEEResult Init(ds::Net::INetwork* pNetwork);
   ds::Net::INetwork1xPriv* Fetch();
   
private:
   ds::Net::INetwork1xPriv* mpIDSNetwork1xPriv;   
};

inline DSSIDSNetwork1xPrivScope::DSSIDSNetwork1xPrivScope() : mpIDSNetwork1xPriv(NULL)
{
}

inline AEEResult DSSIDSNetwork1xPrivScope::Init(ds::Net::INetwork* pNetwork)
{   
   AEEResult res = pNetwork->GetTechObject(ds::Net::AEEIID_INetwork1xPriv, (void**)&mpIDSNetwork1xPriv);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("Failed to fetch INetwork1xPriv from INetwork object: %d",
                      res);
      return res;
   }

   return AEE_SUCCESS;
}
inline ds::Net::INetwork1xPriv* DSSIDSNetwork1xPrivScope::Fetch()
{   
   return mpIDSNetwork1xPriv;
}

/*lint -e{1551} */
inline DSSIDSNetwork1xPrivScope::~DSSIDSNetwork1xPrivScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetwork1xPriv);
}
/*lint –restore */

#endif // __DSS_IDSNETWORK1XPRIVSCOPE_H__
