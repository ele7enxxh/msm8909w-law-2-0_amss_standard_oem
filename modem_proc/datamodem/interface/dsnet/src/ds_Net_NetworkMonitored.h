#ifndef DS_NET_NETWORK_MONITORED_H
#define DS_NET_NETWORK_MONITORED_H

/*===========================================================================
  @file DS_Net_NetworkMonitored.h

  This file defines the NetworkMonitored class, which is derived from the
  Network class.


  Monitored mode network objects are used by special applications that just
  need to monitor the state of an interface (Example: UI application). When
  such a network object is created, it associates with a certain interface,
  but does not try to bring it up.

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkMonitored.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
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

    class NetworkMonitored : public Network
    {
    protected:
      ISignalBus* mpSigBusBearerTech;
      ISignalBus* mpSigBusIPAddr;
      ISignalBus* mpSigBusExtendedIPConfig;
      ISignalBus* mpSigBusQoSAware;
      ISignalBus* mpSigBusDelExtIPv6Addr;
      ISignalBus* mpSigBusBearerTechEx;
      ISignalBus* mpSigBusOutage;
      ISignalBus* mpSigBusHandoffInit;
      ISignalBus* mpSigBusHandoffSuccess;
      ISignalBus* mpSigBusHandoffFailure;
      ISignalBus* mpSigBusRFConditions;
      ISignalBus* mpSigBusExtendedIpConfigEx;
      ISignalBus* mpSigBusApnParamChange;
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
      NetworkMonitored()

      @brief
      NetworkMonitored class constructor.

      @details
      NetworkActice class constructor. NetworkActice object always must have a
      corresponding policy object associated. To create a NetworkActice object,
      use the CreateNetwork() method from INetworkFactory, with QDS_ACTIVE as
      the networkMode parameter.

      @param[in]  pIPolicy- Pointer to the network policy object.
      @return     Returns pointer to network object created.
      @see        INetworkFactory
      */
      NetworkMonitored
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
      virtual ~NetworkMonitored
      (
        void
      ) throw();
      /*!
      @function
      Stop()

      @brief
      This function stops the network connection.

      @details
      Not supported for NetworkMonitored.

      @param      None
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
      Not supported for NetworkMonitored

      @param      None
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
      This function is called when its corresponding
      NetworkActive is closing. It then sets the handle to 0,
      and notifies NetworkEvent::QDS_EV_STATE_CHANGED.
      Sockets are registered to this event on the network.
      This way sockets receive network change state event
      when the network is closing.

      @param      None

      @return       DS::SUCCESS.

      @see          ds::Net::Handle::SetHandle()
      @see          ds::Net::Handle::Notify()
      */
      virtual ds::ErrorType CDECL NotifyNetDown2Monitored
      (
        void
      );

      /*!
      @function
      Init()

      @brief
      Performs initialization that can fail.

      @return     Error code if any, or Success.
      */
      virtual ds::ErrorType Init
      (
        void
      );

      /*!
      @function
      NetworkMonitored object Destructor.

      @brief
      NetworkMonitored object Destructor.

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

      /*-------------------------------------------------------------------------
      Functions to overload the new and delete operators of this class.
      -------------------------------------------------------------------------*/
      DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_NETWORK_MONITORED)

    };/* class Network */
  } /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_NETWORK_MONITORED_H */

