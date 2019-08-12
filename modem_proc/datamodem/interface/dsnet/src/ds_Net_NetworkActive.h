#ifndef DS_NET_NETWORK_ACTIVE_H
#define DS_NET_NETWORK_ACTIVE_H

/*===========================================================================
  @file DS_Net_NetworkActive.h

  This file defines the NetworkActive class, which is derived from the
  Network class.

  When created, active networks perform a bring-up of the network interface
  they associate with. When the network object is returned, it is not
  guaranteed that the network interface is brought up. The clients need to
  register for state changed events on the returned object to check when the
  interface is brought up.

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkActive.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-09-13 ea  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_Net_Network.h"

/*===========================================================================
                     FORWARD DECLERATION
===========================================================================*/

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Net
  {

    class NetworkActive : public Network
    {
    protected:
      /*!
      @function
      ProcessIfaceStateEvent()

      @brief
      Internal method that performs processing of the iface state event.

      @details
      Special processing for iface state is required by the network object.
      1. If the network object's earlier bringup failed because of CLOSE_IN_PROG
      then DSNET needs to perform bring up again.
      2. If IFACE_DOWN event is reported, then network object needs to update
      the last network down reason.

      @param[in]  pEventInfo - Event info pointer passed by EventManager.
      @return     None.
      @see        ds::Net::EventManager
      */
      virtual void ProcessIfaceStateEvent
      (
        EventInfoType   *pEventInfo
      );

    public:
      /*!
      @function
      NetworkActive()

      @brief
      NetworkActive class constructor.

      @details
      NetworkActice class constructor. NetworkActice object always must have a
      corresponding policy object associated. To create a NetworkActice object,
      use the CreateNetwork() method from INetworkFactory, with QDS_ACTIVE as
      the networkMode parameter.

      @param[in]  pIPolicy- Pointer to the network policy object.
      @return     Returns pointer to network object created.
      @see        INetworkFactory
      */
      NetworkActive
      (
        Policy   *pPolicy
      );

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
      virtual ~NetworkActive
      (
        void
      ) throw();

      /*!
      @function
      NetworkActive object Destructor.

      @brief
      NetworkActive object Destructor.

      @details
      Should not be directly called! Use Release() method of IQI interface
      to release the network object.

      @param      None
      @return     None
      @see        INetwork::Release()
      */
      virtual void Destructor
      (
        void
      )
      throw();

      /*!
      @function
      Stop()

      @brief
      This function stops the network connection.

      @details
      Initiates termination to bring down PPP and the traffic channel. After
      successful teardown of the network connection, a signal is set to
      inform the application that the PPP connection has been closed.

      If there are other applications using the network interface, then Stop()
      can succeed immediately without actual teardown of the iface.

      @param      None

      @return     AEE_SUCCESS - on success
      @return     QDS_EFAULT - Invalid arguments

      @see        GoNull()
      */
      virtual ds::ErrorType CDECL Stop
      (
        void
      );

      /*!
      @function
      BringUpInterface()

      @brief
      Bring up the network interface.

      @details
      This method is only supported for INetworkPriv interface. For
      INetworkPriv, the network object can be created without having to bring
      it up first (Lookup mode). This is required to register for events on
      iface ids, to support backward compatibility. New applications should use
      only INetwork interface, wherein getting the interface would guarantee
      Network bringup.

      @param      None.

      @return     AEE_SUCCESS - on success
      @return     AEE_EWOULDBLOCK - If the network interface bringup is blocking
      and would complete asynchronously. Applications should register
      for state changed event in this scenario.

      @see        INetworkPriv
      */
      virtual ds::ErrorType CDECL BringUpInterface
      (
        void
      );

     /*!
      @function
      GetSignalBus()

      @brief
      Get the signal bus that matches the event.

      @details
      Get the signal bus that matches the event

      @param      None

      @return     AEE_SUCCESS - on success
      @return     QDS_EFAULT - if ppISigBus = 0
      @return     QDS_EINVAL - if the signal bus is NULL

      */
      virtual ds::ErrorType GetSignalBus
      (
        ds::Net::EventType  eventID,
        ISignalBus **       ppISigBus
      );
      /*!
      @function
      NotifyNetDown2Monitored()

      @brief
      Notify NetworkMonitored object on network down event.

      @details
      Not supported for NetworkActive.

      @param      None

      @return       QDS_EINVAL.

      @see          ds::Net::Handle::SetHandle()
      @see          ds::Net::Handle::Notify()
      */
      virtual ds::ErrorType CDECL NotifyNetDown2Monitored
      (
        void
      );

      /*-------------------------------------------------------------------------
      Functions to overload the new and delete operators of this class.
      -------------------------------------------------------------------------*/
      DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_NETWORK_ACTIVE)

    };/* class NetworkActive */
  } /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_NETWORK_ACTIVE_H */

