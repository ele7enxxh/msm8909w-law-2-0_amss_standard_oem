#ifndef DS_NET_NETWORK_IPV6_ADDRESS_H
#define DS_NET_NETWORK_IPV6_ADDRESS_H

/*===========================================================================
  @file NetworkIPv6Address.h

  This file declares NetworkIPv6Address class.

  The NetworkIPv6Address class (ds::Net::NetworkIPv6Address) implements the
  following interfaces:
  IQI
  IEventManager
  INode
  IIPv6Address

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkIPv6Address.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-02-27 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_IIPv6Address.h"
#include "ds_Net_NetworkIPv6.h"


/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
namespace Net
{
   /*lint -esym(1510, IIPv6Address) */
   /*lint -esym(1510, IQI) */
class NetworkIPv6Address : public IIPv6Address,
                           public Handle
{
private:
  ds::INAddr6Type           mIPv6Addr;
  IPv6AddrStateType         mState;
  ISignalBus *              mpSigBusStateChange;

  /*!
  @brief
  Private destructor.

  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual void Destructor
  (
    void
  )
  throw();

  /*!
  @brief
  Dummy destructor. Only used to free memory.

  @details
  For objects that support weak references, destruction of the object and
  freeing up of memory can happen at different times. Hence Destructor()
  is the one which performs actual destruction of the object, where as
  ~Object() would perform memory cleanup.
  @params     None.
  @return     None.
  */
  virtual ~NetworkIPv6Address
  (
    void
  )
  throw();

public:
  /**
  @brief      NetworkIPv6Address class constructor.

  @param

  @see        INetworkFactory

  @return     Returns pointer to network object created.
  */

  NetworkIPv6Address
  (
    const ds::INAddr6Type      ipv6Addr,
    IPv6AddrStateType          ipv6AddrState
  );

  /*!
  @brief
  Construction code that can fail. You must call this function and verify
  it succeeds when creating an instance of this object.

  @params[in] ifaceHandle - handle to network interface     
  @return     AEE_SUCCESS or other error code.
  */
  ds::ErrorType Init
  (
    int32                      ifaceHandle
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_NETWORK_IPV6_ADDRESS)

  /*-------------------------------------------------------------------------
    INode interface Methods
  -------------------------------------------------------------------------*/
  /**
  @brief      This method implements the INode interface's Process() function.

              This function is used in case of event handling. This function
              is invoked for the following iface level IPv6 Address events:
              IFACE_IPV6_PRIV_ADDR_GENERATED_EV
              IFACE_IPV6_PRIV_ADDR_DEPRECATED_EV
              IFACE_IPV6_PRIV_ADDR_EXPIRED_EV
              IFACE_IPV6_PRIV_ADDR_DELETED_EV

              If this event is relevant to the current IPv6 Address object
              then the event posted from DSNET layer is:
              ds::IPv6Addr::Event::STATE_CHANGED

  @param[in]  userDataPtr: Event info for the event.

  @see        ds::Utils::INode
  @see        ds::Utils::ITraverser
  @see        ds::IPv6Addr::Event::STATE_CHANGED

  @return     TRUE - If the Process() succeeds.
  @return     FALSE - If the Process() fails.
  */
  virtual boolean Process
  (
    void*                                   userDataPtr
  );


  /*-------------------------------------------------------------------------
    Forwarders for Handle.
  -------------------------------------------------------------------------*/
  /**
  @brief      This function allows registering for events on the
              IPv6Address object.

              The following events can be registered using
              OnStateChange() method on NetworkIPv6Address object:
              ds::IPv6Address::Event::STATE_CHANGE.

              The application is supposed to pass an instance
              of the ISignal interface which would be set when this
              event is triggered.

  @param[in]  signalObj - ISignal used for registration.
  @param[in]  eventID - Event that should be registered for.

  @see        None.

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL OnStateChange
  (
    ISignal*           signalObj,
    ds::Net::EventType eventID,
    IQI**              regObj
  );

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );


  /*-------------------------------------------------------------------------
    Functions inherited from IIPv6Address
  -------------------------------------------------------------------------*/
  /**
  @brief      This function returns V6 address corresponding to this object.

  @param[out] addr - Returned IPv6 address

  @see        ds::INAddr6Type

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL GetAddress
  (
    ds::INAddr6Type addr
  );


  /**
  @brief      This function returns state of the V6 address object.

  @param[out] pState - State of the V6 address object.

  @see        ds::IPv6Addr::StateType

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL GetState
  (
    IPv6AddrStateType* state
  );

  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IIPv6Address)


}; /* class NetworkIPv6Address */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_NETWORK_IPV6_ADDRESS_H */
