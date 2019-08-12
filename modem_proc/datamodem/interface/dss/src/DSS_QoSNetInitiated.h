#ifndef __DSS_QOSNETINITIATED_H__
#define __DSS_QOSNETINITIATED_H__

/*======================================================

FILE:  DSS_QoSNetInitiated.h

SERVICES:
Backward Compatibility Layer QoS Net Initiated class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSNetInitiated.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-07 sz  Created.

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
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Net_IQoSNetInitiated.h"
#include "DSS_QoS.h"
#include "ds_Net_IPhysLink.h"
#include "DSS_QoSHandler.h"
#include "DSS_QoSInfoCodeUpdatedHandler.h"

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
//  CLASS:      DSSQoSNetInitiated
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

// Forward declarations to prevent circular inclusion of DSSQoSNetInitiated.
class DSSEventHandler;
class DSSQoSNetInitiatedHandler;
class DSSQoSModifyHandler;
class DSSQoSNetInitiatedInfoCodeHandler;
class DSSNetActive;

class DSSQoSNetInitiated : public DSSQoS
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:

    DSSQoSNetInitiated(ds::Net::IQoSNetInitiated*  pNetQoSInitiated, 
                       dss_qos_handle_type  handle,
                       dss_iface_id_type ifaceId,
                       uint32 dssQosNetInitiatedAssocReq);
    virtual ~DSSQoSNetInitiated() throw() {}
    void InsertToList(DSSQoSNetInitiated* pDSSQoSNetInitiated);
    virtual void Destructor() throw();
    virtual void GetHandle(dss_qos_handle_type* pHandle);
    AEEResult GetGrantedFlowSpec(dss_iface_ioctl_qos_get_flow_spec_type* qos_flow_spec);
    AEEResult GetGrantedFlowSpec2(dss_iface_ioctl_qos_get_granted_flow_spec2_type* qos_getGrantedFlowSpec2);
    int32 GetFlow();

    /*-------------------------------------------------------------------------
    Defintions of IQI and IWeakRef Methods
    -------------------------------------------------------------------------*/
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------


    // Set() and Get() of parent DSSNetApp object.
    // GetDSSNetApp() returns NetApp with storng ref, or NULL if
    // NetApp freed already. who calls GetDSSNetApp(), should call
    // Release() on NetApp when done.
    virtual AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive);

    virtual void SetDSSNetActive(DSSNetActive* pDSSNetActive);

    void SetHandle(dss_qos_handle_type handle);    

    void GetIfaceId(dss_iface_id_type* pIfaceId);

    AEEResult GetNext(DSSQoSNetInitiated** ppDSSQoSNetInitiated) throw(){
       *ppDSSQoSNetInitiated = mNext;
       return AEE_SUCCESS;
    }

    inline void GetNetHandle(sint15* pNetHandle)
    {
       *pNetHandle = mNetHandle;
    }
    inline void SetNetHandle(sint15 netHandle)
    {
       mNetHandle = netHandle;
    }

    virtual AEEResult GetQoS(ds::Net::IQoS** ppQoS);
    AEEResult GetQoSNetInitiated(ds::Net::IQoSNetInitiated** ppQoSNetInitiated);

    AEEResult RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                         dss_iface_id_type ifaceId);

    AEEResult GetQoSStatus(dss_iface_ioctl_qos_get_status_type* pqos_status_type);
    AEEResult GetPhysLinkObject(ds::Net::IPhysLink** ppPhyslink);
    
    uint32 GetDSSQoSNetInitiatedAssocReq();
    void ClearDSSQoSNetInitiatedAssocReq();

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
     ds::Net::IQoSNetInitiated* mpQoSNetInitiated;
     DSSQoSNetInitiated* mNext;     // DSSQoSNetInitiated list
     
     // handle identifying the application request object in dss
     dss_qos_handle_type mHandle;

     dss_iface_id_type   mIfaceId;

     // QOS event handlers to handle NIQ flow events
     DSSQoSHandler* mpQoSNetInitiatedHandler;
     DSSQoSModifyHandler* mpQoSNetInitiatedModifyHandler;
     DSSQoSInfoCodeUpdatedHandler* mpQoSNetInitiatedInfoCodeHandler;     
     
     DSSNetActive* mparentNetActive;
     sint15 mNetHandle;  //netHandle of parent DSSNetApp
     // Addr of the associated DSSQoSNetInitiatedReq object
     uint32 mDSSQoSNetInitiatedAssocReq;
  public :

     void * operator new (
        unsigned int numBytes
     )  throw();

     void operator delete (
        void *  bufPtr
     );

};

//===================================================================

#endif // __DSS_QOSNETINITIATED_H__
