#ifndef DS_NET_NETWORK_IPV6_H
#define DS_NET_NETWORK_IPV6_H

/*===========================================================================
  @file NetworkIPv6.h

  This file declares the NetworkIPv6 class.

  The NetworkIPv6 class (ds::Net::NetworkIPv6) implements the following
  interfaces:
  IQI
  IEventManager
  INetworkIPv6
  INetworkIPv6Priv

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkIPv6.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-02-27 hm  Added IPv6 priv addr and prefixes support.
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_INetworkIPv6.h"
#include "ds_Net_INetworkIPv6Priv.h"
#include "ds_Net_Handle.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_List.h"

/*===========================================================================
                            FORWARD DECLERATION
===========================================================================*/
namespace ds
{
namespace Net
{

/*lint -esym(1510, INetworkIPv6Priv) */
/*lint -esym(1510, INetworkIPv6) */
/*lint -esym(1510, IQI) */
class NetworkIPv6 : public INetworkIPv6Priv,
                    public INetworkIPv6,
                    public Handle
{

private:

  ISignalBus *      mpSigBusPrefixChanged;

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
  virtual ~NetworkIPv6
  (
    void
  )
  throw();

public:
  /*-------------------------------------------------------------------------
    CONSTRUCTOR/DESTRUCTOR
  -------------------------------------------------------------------------*/
  /**
  @brief      NetworkIPv6 class constructor.

  @param      None.

  @see        INetworkIPv6
  @see        INetwork

  @return     None. Creates the Network object.
  */
  NetworkIPv6
  (
    int32      ifaceHandle
  );

  /*!
  @brief
  Construction code that can fail. You must call this function and verify
  it succeeds when creating an instance of this object.

  @params     None.
  @return     AEE_SUCCESS or other error code.
  */
  ds::ErrorType Init(void);

  /*!
  @brief
  This method will update the iface pointer(handler).

  @params     Iface pointer(handle)
  @return     None
  */
  void SetIfaceHandle
   (
     int32 objHandle
   );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_NETWORK_IPV6)


  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  /**
  @brief      QueryInterface method of Network object.

              The NetworkIPv6 object implements the following interfaces:
              IQI
              IEventManager
              INetworkIPv6
              INetworkIPv6Priv
              All of these interfaces can be got by using the
              QueryInterface() method using appropriate IID.

  @param[in]  iid - Interface Id of the required interface.
  @param[out] ppo - Pointer to the interface returned.

  @see        IQI::QueryInterface()

  @return     AEE_SUCCESS on success
  @return     AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return     Other ds error codes may be returned.
  */
  virtual int CDECL QueryInterface
  (
    AEEIID                                  iid,
    void **   ppo
  )
  throw();

  /*-------------------------------------------------------------------------
    Handle interface forwarders
  -------------------------------------------------------------------------*/
  /**
  @brief      This function allows registering for events on the
              NetworkIPv6 object.

              Supported events on NetworkIPv6 object:
              ds::Net::Network::IPv6Priv::Event::PREFIX_CHANGED.

  @param[in]  pISignal - ISignal used for registration.
  @param[in]  eventID - Event that should be registered for.

  @see        None.

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL OnStateChange
  (
    ISignal*                pISignal,
    ds::Net::EventType      eventID,
    IQI**                   regObj
  );

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );

  /*-------------------------------------------------------------------------
    Methods from INetworkIPv6 interface
  -------------------------------------------------------------------------*/
  /**
  @brief      This function generates a IPv6 Privacy address.


  @param[in]  isUnique - If the private address is unique or shareable.
  @param[out] ppIPv6PrivAddr - Returned priv address.

  @see        IIPv6Address.
  @see        ds::IPv6Address::Event::STATE_CHANGED

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL GeneratePrivAddr
  (
    boolean isUnique,
    IIPv6Address**  ppIPv6PrivAddr
  );

  /*-------------------------------------------------------------------------
    Methods from INetworkIPv6Priv interface
  -------------------------------------------------------------------------*/
  /**
  @brief      This function returns all the IPv6 prefixes.

  @param[out] pPrefixes - Pointer to returned prefix info.
  @param[in]  allPrefixesLen - Length of allocated elements in pPrefixes.
  @param[out] allPrefixesLenReq - If additional buffer is required

  @see        None.

  @return     AEE_SUCCESS on success
  @return     Other ds error codes might be returned.
  */
  virtual ds::ErrorType CDECL GetAllIPv6Prefixes
  (
    IPv6PrivPrefixInfoType*    pPrefixes,
    int allPrefixesLen,
    int*                                  allPrefixesLenReq
  );

  // TODO implement
  virtual ds::ErrorType CDECL GetScopeID(unsigned int* scopeID)
  {
     return 0;
  }

  /*-------------------------------------------------------------------------
    Macros to implement IWeakRef/IQI interfaces
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

}; /* class NetworkIPv6 */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_NETWORK_IPV6_H */

