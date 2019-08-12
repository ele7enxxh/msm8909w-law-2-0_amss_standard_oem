#ifndef __DSS_IDSNETPOLICYSCOPE_H__
#define __DSS_IDSNETPOLICYSCOPE_H__

/*===================================================

FILE:  DSS_IDSNetPolicyScope.h

SERVICES:
   A utility class to facilitate IDSNetPolicy fetching
   and releasing.

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IDSNetPolicyScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_NetApp.h"
#include "DSS_Common.h"
#include "DSS_Globals.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "ds_Net_INetworkFactory.h"
#include "DSS_Conversion.h"
#include "ds_Net_IPolicy.h"
#include "ds_Errors_Def.h"
#include "DSS_GenScope.h"

using namespace ds::Error;

// This class provides an abstraction over IDSNetPolicy pointer.
// Define and initialize (Init) an instance of this class at the top of the scope.
// If init is successful, the IDSNetPolicy pointer is valid and can be used
// until the end of the scope where the IDSNetPolicy object's reference counting
// decremented.
class DSSIDSNetPolicyScope {
public:
   DSSIDSNetPolicyScope();
   ~DSSIDSNetPolicyScope();
   AEEResult Init();
   AEEResult Init(DSSNetApp* pDSSNetApp);
   AEEResult Init(ds::Sock::ISocket* pIDSSocket);
   ds::Net::IPolicy* Fetch();

private:
   ds::Net::IPolicy* mpIDSNetPolicy;
};

inline DSSIDSNetPolicyScope::DSSIDSNetPolicyScope() : mpIDSNetPolicy(NULL)
{
}

inline AEEResult DSSIDSNetPolicyScope::Init()
{
   AEEResult res = AEE_SUCCESS;

   // Get the NetworkFactory from DSSGlobals
   ds::Net::INetworkFactory *pNetworkFactory;
   res = DSSGlobals::Instance()->GetNetworkFactory(&pNetworkFactory); 
   if(AEE_SUCCESS != res){
      return res;
   }
   DSSGenScope scopeNetworkFactory(pNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   // Create new Policy object.
   res = pNetworkFactory->CreatePolicy(&mpIDSNetPolicy);

   return res;
}

inline AEEResult DSSIDSNetPolicyScope::Init(ds::Sock::ISocket* pIDSSocket)
{
   AEEResult res;

   if (NULL == pIDSSocket) {
      return QDS_EFAULT;
   }

   // Get the NetworkFactory from DSSGlobals
   ds::Net::INetworkFactory *pNetworkFactory;
   res = DSSGlobals::Instance()->GetNetworkFactory(&pNetworkFactory);
   if(AEE_SUCCESS != res){
     return res;
   }
   DSSGenScope scopeNetworkFactory(pNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   // Create new Policy object.
   res = pNetworkFactory->CreatePolicy(&mpIDSNetPolicy);

   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("Can't create DSNetPolicy: %d", res);
      return res;
   }

#if 0
   res = pIDSSocket->GetNetPolicy(&mpIDSNetPolicy);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("GetNetPolicy operation failed: %d", res);
      return res;
   }
#endif

   return AEE_SUCCESS;
}

inline AEEResult DSSIDSNetPolicyScope::Init(DSSNetApp* pDSSNetApp)
{
   if (NULL == pDSSNetApp) {
      return QDS_EFAULT;
   }
   AEEResult res = pDSSNetApp->GetPolicy(&mpIDSNetPolicy);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("Failed to fetch IDSNetPolicy object: %d", res);
      return res;
   }

   return AEE_SUCCESS;
}

inline ds::Net::IPolicy* DSSIDSNetPolicyScope::Fetch()
{
   return mpIDSNetPolicy;
}

/*lint -e{1551} */
inline DSSIDSNetPolicyScope::~DSSIDSNetPolicyScope()
{
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetPolicy);
}
/*lint –restore */

#endif // __DSS_IDSNETPOLICYSCOPE_H__
