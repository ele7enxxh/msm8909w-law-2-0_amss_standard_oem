#ifndef __DSS_MCAST_H__
#define __DSS_MCAST_H__
#ifdef FEATUTE_DATA_PS_MCAST
/*======================================================

FILE:  DSS_MCast.h

SERVICES:
Backward Compatibility Layer MCast class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCast.h#1 $
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

#include "ds_Utils_StdErr.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"

#include "DSS_EventHandler.h"

#include "DSS_MCastStatusHandler.h"
#include "DSS_MCastRegisterHandler_1_0.h"
#include "DSS_MCastRegisterHandler_2_0.h"
#include "ds_Utils_IWeakRef.h"
#include "ds_Utils_IWeakRefSupport.h"

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
//  CLASS:      DSSMCast
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

class DSSEventHandler;
class DSSMCastHandler;
class DSSNetActive;

using ds::Utils::IWeakRef;

class DSSMCast : public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors / Destructors
//-------------------------------------------------------------------
  public:

    DSSMCast(ds::Net::IMCastSessionPriv* pMCastSession, uint32 flowID);
    void InsertToList(DSSMCast* pDSSMCast);
    virtual ~DSSMCast() throw() {}
    virtual void Destructor() throw();
    void GetMCastHandle(uint32* pMCastHandle);
    AEEResult RegEventCB(dss_iface_ioctl_event_cb cback_fn,
                         void* user_data,
                         dss_iface_ioctl_event_enum_type event_cb,
                         dss_iface_id_type ifaceId);
    void * operator new (unsigned int numBytes) throw();
    void operator delete (void *  bufPtr);

    //-------------------------------------------------------------------------
    //  Defintions of IQI and IWeakRef Methods
    //-------------------------------------------------------------------------
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS()
  
  public:
       
    inline AEEResult GetMCastSession(ds::Net::IMCastSessionPriv** ppMCastSession) {
       *ppMCastSession = mpNetMCastSession;
       (void)mpNetMCastSession->AddRef();
       return AEE_SUCCESS;
    }

    // Set() and Get() of parent DSSNetActive object.
    // GetDSSNetActive() returns NetActive with storng ref, or NULL if
    // NetActive freed already. who calls GetDSSNetActive(), should call
    // Release() on NetActive when done.
    AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive);

    void SetDSSNetActive(DSSNetActive* pDSSNetActive);

    AEEResult GetNext(DSSMCast** ppDSSMCastSession) throw() {
       *ppDSSMCastSession = mNext;
       return AEE_SUCCESS;
    }

    //-------------------------------------------------------------------
    //  Deregisters MCastRegister handler, in case both are deregistered
    //  returns TRUE, indicating that this DSSMCast object can be 
    //  deleted, otherwise returns FALSE
    //-------------------------------------------------------------------
    boolean DeregisterHandler(ds::Net::EventType handlerType);

//-------------------------------------------------------------------
//  Private members
//-------------------------------------------------------------------
  private:
     ds::Net::IMCastSessionPriv* mpNetMCastSession;
     DSSMCast* mNext;
     uint32 mMCastHandle;
     DSSMCastStatusHandler* mpMCastStatusHandler;
     DSSMCastRegisterHandler1_0* mpMCastRegisterHandler1_0;
     DSSMCastRegisterHandler2_0* mpMCastRegisterHandler2_0;
     DSSNetActive* mparentNetActive;
     template<typename HandlerType>
     AEEResult FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit);

};


//===================================================================
#endif // FEATUTE_DATA_PS_MCAST
#endif // __DSS_MCAST_H__
