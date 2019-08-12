#ifndef __DSS_PRIVIPV6ADDR_H__
#define __DSS_PRIVIPV6ADDR_H__

/*======================================================

FILE:  DSS_PrivIpv6Addr.h

SERVICES:
Backward Compatibility Layer Private Ipv6 address class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PrivIpv6Addr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------

#include "comdef.h"
#include "customer.h"
#include "dssocket.h"

#include "ds_Net_IIPv6Address.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_IWeakRef.h"
#include "ds_Utils_IWeakRefSupport.h"

using namespace ds::Error;

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSPrivIpv6Addr
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

// Forward declarations to prevent circular inclusion of DSSPrivIpv6Addr.

class DSSEventHandler;
class DSSPrivIpv6AddrHandler;
class DSSNetActive;

using ds::Utils::IWeakRef;


class DSSPrivIpv6Addr : public IWeakRef
{
   //-------------------------------------------------------------------
   //  Constructors/Desctructors
   //-------------------------------------------------------------------
public:

  DSSPrivIpv6Addr(ds::Net::IIPv6Address* pNetIpv6Address, uint32 flowID, boolean isUnique);
   void InsertToList(DSSPrivIpv6Addr* pDSSPrivIpv6Addr);
   virtual ~DSSPrivIpv6Addr() throw() {};
   virtual void Destructor() throw();

   //-------------------------------------------------------------------
   //  Interface members
   //-------------------------------------------------------------------
public:

   /*-------------------------------------------------------------------------
   Defintions of IQI and IWeakRef Methods
   -------------------------------------------------------------------------*/
   DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

   //-------------------------------------------------------------------
   //  Get/Set functions for protected members access
   //
   //-------------------------------------------------------------------

   inline AEEResult GetIDSNetIpv6Address(ds::Net::IIPv6Address** ppNetIpv6Address)
   {
      if (NULL == mpNetIpv6Address) {
         return QDS_EINVAL;
      }
      *ppNetIpv6Address = mpNetIpv6Address;
      (void)mpNetIpv6Address->AddRef();
      return AEE_SUCCESS;
   }


   AEEResult GetNext(DSSPrivIpv6Addr** ppDSSPrivIpv6Addr) throw()
   {
      *ppDSSPrivIpv6Addr = mNext;
      return AEE_SUCCESS;
   }

   // Set() and Get() of parent DSSNetActive object.
   // GetDSSNetActive() returns NetActive with storng ref, or NULL if
   // NetActive freed already. who calls GetDSSNetActive(), should call
   // Release() on NetActive when done.
   AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive);

   void SetDSSNetActive(DSSNetActive* pDSSNetActive);

   inline void GetFlowID(uint32* pFlowID)
   {
      *pFlowID = mFlowID;
   }

   inline void GetIsUnique(boolean* pIsUnique)
   {
      *pIsUnique = mbIsUnique;
   }

   AEEResult RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                        dss_iface_id_type ifaceId);

   //-------------------------------------------------------------------
   //  Protected members
   //-------------------------------------------------------------------
protected:
   template<typename HandlerType>
   AEEResult FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit);

   AEEResult GetEventHandler(dss_iface_ioctl_event_enum_type event, DSSEventHandler** ppEventHandler, bool bInit);

   //-------------------------------------------------------------------
   //  Data members
   //-------------------------------------------------------------------
protected:
   ds::Net::IIPv6Address* mpNetIpv6Address;
   DSSPrivIpv6Addr* mNext;     // DSSNetQoS list
   uint32 mFlowID;
   boolean mbIsUnique;
   DSSPrivIpv6AddrHandler* mpIpv6PrivAddrEventHandler;
   DSSNetActive* mparentNetActive;

public :

   void * operator new (
      unsigned int numBytes
   )  throw();

   void operator delete (
      void *  bufPtr
   );

};

//===================================================================

#endif // __DSS_PRIVIPV6ADDR_H__
