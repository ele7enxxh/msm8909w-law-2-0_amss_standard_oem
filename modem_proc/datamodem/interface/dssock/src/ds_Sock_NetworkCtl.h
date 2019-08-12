#ifndef DS_SOCK_NETWORK_CTL_H
#define DS_SOCK_NETWORK_CTL_H

/*===========================================================================
@file ds_Sock_NetworkCtl.h

This file defines the NetworkCtl class.

This class is responsible to provide access to several Network APIs via
Signal interface

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_NetworkCtl.h#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-01-05 vm  Created module.

===========================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_ISignalFactory.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_SignalHandlerBase.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkExt.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_Def.h"
#include "ps_socki_defs.h"
#include "ps_rt_meta_info.h"
#include "ps_route_scope.h"
#include "ps_system_heap.h"

/*===========================================================================

PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class NetworkCtl : public ds::Utils::SignalHandlerBase
    {
    public:

      /**
        @brief NetworkCtl class constructor

        Initializes a NetworkCtl object

        @param None

        @retval None
      */
      NetworkCtl
      (
        void
      );

      /**
        @brief Initializes a NetworkCtl object .

        @param[in] critSect pointer needs to be non NULL

        @retval AEE_SUCCESS      NetworkCtl is initialized
                                        successfully
        @retval AEE_ENOMEMORY   No memory for NetworkCtl
                                        initialization

        @see ds::Error
      */
      ds::ErrorType Init
      (
        ICritSect* critSect
      );

      /**
        @brief Destructor for NetworkCtl class.

        Resets the network controller object. Method is defined as virtual
        so that destructors of derived classes are called when NetworkCtl
        object is deleted.

        @param None

        @retval None
      */
      virtual void Destructor
      (
        void
      )
      throw();

      virtual ~NetworkCtl
      (
        void
      )
      throw() {};

      /**
        @brief Clones NetworkCtl object properties to provided
        NetworkCtl object properties.

        Virtual constructor design pattern implementation

        @param[in] critSect socket critical section
        @param[out] networkCtlPtr resulting NetworkCtl object

        @retval AEE_SUCCESS      NetworkCtl is cloned
        successfully
        @retval AEE_ENOMEMORY   No memory for NetworkCtl
        clone

        @see ds::Error
      */
      virtual ds::ErrorType Clone
      (
        ICritSect          *critSect,
        NetworkCtl*        *networkCtlPtr
      ) = 0;

      /**
        @brief Closes NetworkCtl object.

        Detaches and releases owned network signal

        @param None

        @retval None

      */
      void Close
      (
        void
      )
      throw();

      /**
        @brief Sets Network Policy object to NetworkCtl object

        @param[in] newPolicyPtr Policy object to set

        @retval AEE_SUCCESS  Policy object is set
        successfully
        @retval QDS_EFAULT   provided Policy object
        is 0

        @see ds::Error
      */
      ds::ErrorType SetNetPolicy
      (
        ds::Net::IPolicy       *newPolicyPtr
      );

      /**
        @brief Tells if Network Policy object can be updated.
        Abstract.

        @param None

        @retval TRUE  Policy object can be updated
        @retval FALSE Policy object can't be updated

      */
      virtual boolean IsPolicyUpdateEnabled
      (
        void
      ) = 0;

      /**
        @brief Provides access to owned Network Policy object

        In case owned Network Policy object is uninitialized - 0
        is returned.

        User is responsible to Release received Policy object

        @param None

        @retval ds::Net::Policy object

      */
      ds::Net::IPolicy*  GetPolicy
      (
        void
      );

      /**
        @brief Sets Network object to NetworkCtl object

        @param[in] dsNetObjPtr Network object to set

        @retval AEE_SUCCESS  Network object is set
        successfully
        @retval QDS_EFAULT   provided Network object
        is 0

        @see ds::Error
      */
      virtual ds::ErrorType SetNetwork
      (
        ds::Net::INetwork *dsNetObjPtr
      );

      /**
        @brief Updates provided route scope according to
        owned Network Policy object properties

        @param[out] routeScope Route Scope to be updated

        @retval AEE_SUCCESS  Network object is set
        successfully
        @retval QDS_EFAULT   provided Route Scope object
        is 0

        @see ds::Error
      */
      ds::ErrorType UpdateRouteScope
      (
        ps_route_scope_type    *routeScope
      );

      /**
        @brief Provides owned Network object state

        @param[out] netObjState resulting Network State

        @retval AEE_SUCCESS  Network State is retrieved
        successfully
        @retval QDS_EFAULT   provided Network State
        argument is 0

        @see ds::Error
      */
      ds::ErrorType GetNetworkState
      (
        ds::Net::NetworkStateType *netObjState
      );

      /**
        @brief Provides iface ID to which owned Network object
        is connected

        @param[out] ifaceId resulting iface ID

        @retval AEE_SUCCESS  iface ID is retrieved
        successfully
        @retval QDS_EFAULT   provided iface ID
        argument is 0

        @see ds::Error
      */
      ds::ErrorType GetIfaceId
      (
        ds::Net::IfaceIdType *ifaceId
      );

      /**
        @brief Registers provided Signal.
        Signal is Set when Network State Change event occurs

        @param[in]  clientIfaceUpSignal event IFACE_UP signal to register
        @param[in]  clientIfaceDownSignal event IFACE_DOWN signal to register
        @param[out] regObj RegObj which will be allocated upon
        each registration and which user is responsible to free

        @retval AEE_SUCCESS  Signal is registered
        successfully
        @retval QDS_EFAULT provided one of the signals is 0
        @retval QDS_EFAULT provided RegObj is 0

        @see ds::Error
      */
      ds::ErrorType RegisterClientSignal
      (
        ISignal *clientIfaceUpSignal,
        ISignal *clientIfaceDownSignal,
        IQI**    regObj
      );

      /**
        @brief Tells if Network object is associated with
        this Network Controller object

        @param None

        @retval TRUE  Network object is set
        @retval FALSE Network object was not set

      */
      boolean IsNetworkBound
      (
        void
      );

      /*-------------------------------------------------------------------
      IQI interface Methods
      -------------------------------------------------------------------*/
      virtual ds::ErrorType CDECL QueryInterface
      (
        AEEIID   iid,
        void **  objPtrPtr
      ) throw();

      /**
        @brief Closes NetworkCtl object.

        Detaches and releases a given non-NULL signal

        @param[in] signalCtlPtr given signal to detach

        @retval None

      */
      static void DetachSignal
      (
        ISignalCtl **signalCtlPtr
      )
      throw();

    protected:

      /**
        @brief Support method Sets Network object to NetworkCtl

        @param[in] dsNetObjPtr Network object to set

        @retval AEE_SUCCESS  Network object is set
        successfully
        @retval QDS_EFAULT   provided Network object
        is 0

        @see ds::Error
      */
      ds::ErrorType UpdateNetwork
      (
        ds::Net::INetwork *dsNetObjPtr
      );

      /**
        @brief Provides access to owned Network object

        In case owned Network object is uninitialized - 0
        is returned.

        @param None

        @retval ds::Net::Network object

      */
      inline ds::Net::INetwork* GetNetwork
      (
        void
      )
      {
        return networkPtr;
      }

      /**
        @brief Protected Clone which can only be called by classes inheriting from NetworkCtl.
        Clones NetworkCtl object properties.

        Virtual constructor design pattern implementation

        @param[out] networkCtlPtr resulting NetworkCtl object

        @retval AEE_SUCCESS      NetworkCtl is cloned
        successfully
        @retval AEE_ENOMEMORY   No memory for NetworkCtl
        clone

        @see ds::Error
      */
      virtual ds::ErrorType CloneMembers
      (
        NetworkCtl*        *networkCtlPtr
      );

    private:

      ds::Net::INetwork         *networkPtr;
      ds::Net::IPolicy          *policyPtr;
      ISignalCtl                *ifaceUpSignalCtlPtr;
      ISignalCtl                *ifaceDownSignalCtlPtr;
      ISignal                   *clientIfaceUpSignalPtr;
      ISignal                   *clientIfaceDownSignalPtr;

      ICritSect                 *critSectPtr;

      /**
        @brief Registers owned Signal with owned Network object
        for Network State Changed event.

        @param  None

        @retval AEE_SUCCESS  Signal is registered
        successfully

        @see ds::Error
      */
      ds::ErrorType RegNetworkStateChangedEvent
      (
        void
      );

      /**
        @brief Registers a given Signal with to a given event.

        @param[in]  signalFactoryPtr signal factory to create the signal and signal ctrl
        @param[in]  eventType event to register to
        @param[in]  signalHandlerCallBack input to signal factory create method
        @param[out] signalCtlPtr output to be able to control the signal

        @retval AEE_SUCCESS  Signal is registered
        successfully

        @see ds::Error
      */
      ds::ErrorType RegSignal2Event
      (
        ISignalFactory        *signalFactoryPtr,
        ds::Net::EventType    eventType,
        SignalHandlerCallBack signalHandlerCallBack,
        ISignalCtl            **signalCtlPtr
      );

      /**
        @brief Called when IFACE_UP event occurs
        Static.

        @param[in] userDataPtr data passed on Signal registration

        @retval None

      */
      static void EventCallbackIfaceUp
      (
        SignalHandlerBase *  pSignalHandlerBaseParam
      );

      /**
        @brief Called when IFACE_DOWN event occurs
        Static.

        @param[in] userDataPtr data passed on Signal registration

        @retval None

      */
      static void EventCallbackIfaceDown
      (
        SignalHandlerBase *  pSignalHandlerBaseParam
      );

      /**
      This class implements the IQI interface and used
      to create regObj in RegEvent function.
      */
      class RegBound : public IQI{
      public:
        RegBound():refCnt(1){};
        virtual ~RegBound
        (
          void
        )throw()
        {

        };

        void * operator new
        (
          unsigned int numBytes
        )
        throw()
        {
           void * buf = NULL;
           PS_SYSTEM_HEAP_MEM_ALLOC(buf, sizeof (RegBound), void*);
           return buf; 
        }

        void operator delete
        (
          void *  bufPtr
        )
        throw()
        {
          if(NULL != bufPtr)
          {
            PS_SYSTEM_HEAP_MEM_FREE (bufPtr);
            return;
          }
        }
        DSIQI_DECL_LOCALS()
        DSIQI_ADDREF()
        DSIQI_RELEASE()
        virtual int CDECL QueryInterface (AEEIID iid, void **ppo){
          switch (iid)
          {
          case AEEIID_IQI:
            *ppo = reinterpret_cast <void *> (this);
            (void) AddRef ();
            break;

          default:
            return AEE_ECLASSNOTSUPPORT;
          }

          return AEE_SUCCESS;
        }
      };/* class RegBound */
    };/* class NetworkCtl */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_NETWORK_CTL_H */
