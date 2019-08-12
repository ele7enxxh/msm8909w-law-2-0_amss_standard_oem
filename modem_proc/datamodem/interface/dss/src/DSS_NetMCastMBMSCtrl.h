#ifndef __DSS_NETMCASTMBMSCTRL_H__
#define __DSS_NETMCASTMBMSCTRL_H__
#ifdef FEATUTE_DATA_PS_MCAST
/*======================================================

FILE:  DSS_NetMCastMBMSCtrl.h

SERVICES:
Backward Compatibility Layer MCast class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetMCastMBMSCtrl.h#1 $
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


#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "DSS_EventHandler.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "DSS_MCastMBMSCtrlHandler.h"
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
class DSSNetActive;

//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSNetMCastMBMSCtrl
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

class DSSEventHandler;
class DSSMCastHandler;

using ds::Utils::IWeakRef;

class DSSNetMCastMBMSCtrl : public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:

    DSSNetMCastMBMSCtrl(ds::Net::IMCastMBMSCtrlPriv* pMCastMBMSCtrl, uint32 MCastMBMSHandle);
    void InsertToList(DSSNetMCastMBMSCtrl* pDSSMCastMBMS);
    virtual ~DSSNetMCastMBMSCtrl() throw() {}
    virtual void Destructor() throw();
    void GetMCastMBMSHandle(uint32* pMCastMBMSHandle);
    AEEResult RegEventCB(dss_iface_ioctl_event_cb cback_fn,
                         void* user_data,
                         dss_iface_id_type ifaceId);

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

    inline AEEResult GetMBMSCtrl(ds::Net::IMCastMBMSCtrlPriv** ppMCastMBMS) {
       *ppMCastMBMS = mpNetMCastMBMSCtrl;
       (void)mpNetMCastMBMSCtrl->AddRef();
       return AEE_SUCCESS;
    }

    AEEResult GetNext(DSSNetMCastMBMSCtrl** ppDSSMCastMBMS) throw() { //TODO: should be IDSNetQoSSecondary??
       *ppDSSMCastMBMS = mNext;
       return AEE_SUCCESS;
    }

    // Set() and Get() of parent DSSNetActive object.
    // GetDSSNetActive() returns NetActive with storng ref, or NULL if
    // NetActive freed already. who calls GetDSSNetActive(), should call
    // Release() on NetActive when done.
    AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive);

    void SetDSSNetActive(DSSNetActive* pDSSNetActive);

    AEEResult GetEventHandler(dss_iface_ioctl_event_enum_type event, DSSEventHandler** ppEventHandler, bool bInit);

    template<typename HandlerType>
    AEEResult FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit);

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
     ds::Net::IMCastMBMSCtrlPriv* mpNetMCastMBMSCtrl;
     DSSNetMCastMBMSCtrl* mNext;
     uint32 mMCastMBMSHandle;
     DSSMCastMBMSCtrlHandler* mpMCastMBMSHandler;
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
#endif // FEATUTE_DATA_PS_MCAST
#endif // __DSS_NETMCASTMBMSCTRL_H__
