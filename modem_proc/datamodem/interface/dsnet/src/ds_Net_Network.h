#ifndef DS_NET_NETWORK_H
#define DS_NET_NETWORK_H

/*===========================================================================
  @file Network.h

  This file defines the class that implements the INetwork interface.

  The Network class (ds::Net::Network) implements the following interfaces:
  IQI
  INetwork
  INetworkPriv
  IMCastManagerMBMSPriv
  IIPFilterManagerPriv

  The following are the different types of network objects supported:

  1. Active Mode Network objects - This is the default type of network
     objects. When created, they perform a bring-up of the network interface
     they associate with. When the network object is returned, it is not
     guaranteed that the network interface is brought up. The clients need
     to register for state changed events on the returned object to check
     when the interface is brought up.

  2. Monitored mode network objects - This type of network objects are used
     by special applications that just need to monitor the state of an
     interface (Example: UI application). When such a network object is
     created, it associates with a certain interface, but does not try to
     bring it up.

  3. Privileged mode network objects - This type of network objects are
     used by privileged applications (DSS implementation layer) to support
     legacy DSS API. Other clients cannot use such network objects.



  Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Network.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2011-08-31 hs  Added GetSubnetMask
  2011-08-30 hs  Added GetGatewayAddress.
  2011-08-24 hs  Added GetDeviceName.
  2009-08-20 hm  Use crit sect interface instead of direct objects.
  2009-02-27 hm  Added IPv6 support.
  2008-09-12 hm  Added Mcast, MBMS, QoS support.
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "ds_Net_INetwork.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_INetworkExt.h"
#include "ds_Net_INetworkExt2.h"
#include "ds_Net_INetwork1x.h"
#include "ds_Net_INetwork1xPriv.h"
#include "ds_Net_INetworkControl.h"
#include "ds_Net_INetworkUMTS.h"
#include "ds_Net_IIPFilterManagerPriv.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Utils.h"
#include "ds_Utils_Factory.h"

#include "ds_Net_IPFilterReg.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Policy.h"
#include "ds_Net_Handle.h"
#include "ds_Net_QoSDefault.h"
#include "ds_Net_QoSManager.h"
#include "ds_Net_MCastManager.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_NetworkIPv6.h"

/*===========================================================================
                     FORWARD DECLERATION
===========================================================================*/

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
/*Suppress lint error because of namespace collisions */
/*lint -save -e578 */
namespace ds
{
namespace Net
{

/*lint -esym(1510, INetwork) */
/*lint -esym(1510, IIPFilterManagerPriv) */
/*lint -esym(1510, INetworkPriv) */
/*lint -esym(1510, INetworkControl) */
/*lint -esym(1510, IQI) */
class Network : public INetwork,
                public INetworkExt,
                public INetworkExt2,
                public INetworkPriv,
                public INetworkControl,
                public IIPFilterManagerPriv,
                public Handle
{
private:

  /*!
  @function
  GetNetwork1x()

  @brief
  Internal function to get the 1x network object associated with this
  network object.

  @details
  If an 1x network object is not present already, one would be created.

  @param[in]   iid - Interface ID. Should be AEEIID_INetwork1x or
                     AEEIID_INetwork1xPriv
  @param[out]  ppo - Pointer to the INetwork1x or INetwork1xPriv interface

  @return      AEE_SUCCESS on success
  @return      AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return      QDS_EFAULT - on invalid inputs.
  */
  ds::ErrorType GetNetwork1x
  (
    AEEIID iid,
    void** ppo
  );

  /*!
  @function
  GetNetworkIPv6()

  @brief
  Internal function to get the IPv6 network object associated with this
  network object.

  @details
  If an IPv6 network object is not present already, one would be created.

  @param[in]   iid - Interface ID. Should be AEEIID_DS_NET_INetworkIPv6
  @param[out]  ppo - Pointer to the INetworkIPv6 interface returned.

  @return      AEE_SUCCESS on success
  @return      AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return      QDS_EFAULT - on invalid inputs.
  */
  ds::ErrorType GetNetworkIPv6
  (
    AEEIID iid,
    void** ppo
  );

  /*!
  @function
  GetNetworkUMTS()

  @brief
  Internal function to get the UMTS network object associated with this
  network object.

  @details
  If an UMTS network object is not present already, one would be created.

  @param[in]   iid - Interface ID. Should be AEEIID_INetworkUMTS
  @param[out]  ppo - Pointer to the INetworkUMTS interface returned.

  @return      AEE_SUCCESS on success
  @return      AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return      QDS_EFAULT - on invalid inputs.
  */
  ds::ErrorType GetNetworkUMTS
  (
    AEEIID iid,
    void** ppo
  );

protected:
  /*-------------------------------------------------------------------------
    Protected member declarations.
  -------------------------------------------------------------------------*/
  NetworkModeType                           mNetworkMode;
  int32                                     mFlags;
  NetDownReasonType                         mLastNetDownReason;
  Policy*                                   mpPolicy;
  IPFilterReg*                              mpIPFilterReg;
  boolean                                   mWouldblock;
  boolean                                   mBringupAgain;
  boolean                                   mBringupFirst;
  boolean                                   mTeardown;
  boolean                                   mOutageEventOccurred;
  uint8                                     mFastDormStatus;

  /*-------------------------------------------------------------------------
   Signal buses to support event registration and notifications.
  -------------------------------------------------------------------------*/
  ISignalBus *                              mpSigBusIfaceUp;
  ISignalBus *                              mpSigBusIfaceGoingDown;
  ISignalBus *                              mpSigBusIfaceDown;
  ISignalBus *                              mpSigBusIfaceActiveOutOfUse;
  ISignalBus *                              mpSigBusIfaceComingUp;
  ISignalBus *                              mpSigBusIfaceConfiguring;
  ISignalBus *                              mpSigBusIfaceRouteable;

  // Note: when changing the number of signal buses here,
  // we must change the constant definition MAX_SIGNAL_BUS_PER_APP

  /*-------------------------------------------------------------------------
   Technology objects
  -------------------------------------------------------------------------*/
  INetwork1x*                               mpTechObjNetwork1x;
  INetwork1xPriv*                           mpTechObjNetwork1xPriv;
  INetworkUMTS*                             mpNetworkUMTS;
  NetworkIPv6*                              mpNetworkIPv6;

  ds::Net::NetworkStateType                 mCachedPreviousState;
  int32                                     mAppPriority;

  /*-------------------------------------------------------------------------
    IFACE_IPV6_EXTERNAL_ADDR_DELETED_EV info cached
  -------------------------------------------------------------------------*/
  ds::INAddr6Type           mDeletedIPv6Addr;

  uint32                    mCachedExtendedMask;
  HandoffinfoType           mcachedHandoffinfo;
  HandoffFailureinfoType    mcachedHandoffFailureinfo;
  /*-------------------------------------------------------------------------
    Protected method declarations.
  -------------------------------------------------------------------------*/
  ds::ErrorType IPAddrIOCTL
  (
    NetPlatform::IfaceIoctlEnumType         ioctlName,
    ds::IPAddrType*                         pDSIPAddr
  );

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
  ) = 0;

  /*!
  @brief
  Sets the object's handle.

  @details
  For network object, object handle is same as iface pointer. The handle is
  set when the network gets associated with PS_IFACE (lookup/bringup), and
  gets cleared when network is brought down (iface down).

  @params     objHandle - handle to set.
  @return     None.
  */
  void SetHandle
  (
    int32 objHandle
  );

public:
  /*!
  @function
  Network()

  @brief
  Network class constructor.

  @details
  Network class constructor. Network object always must have a
  corresponding policy object associated. To create a Network object,
  use the CreateNetwork() method from INetworkFactory.

  @param[in]  pIPolicy- Pointer to the network policy object.
  @param[in]  networkMode - Network mode
  @return     Returns pointer to network object created.
  @see        INetworkFactory
  */
  Network
  (
    Policy           *pPolicy,
    NetworkModeType  networkMode
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
  Network object Destructor.

  @brief
  Network object Destructor.

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
  virtual ~Network
  (
    void
  ) throw();

  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  /*!
  @function
  QueryInterface()

  @brief
  QueryInterface method of Network object.

  @details
              The Network object implements the following interfaces:
              IQI
              IEventManager
              INetwork
              INetworkPriv
              IMCastManagerMBMSPriv
              IIPv6Manager
  All of these interfaces can be got by using the QueryInterface() method
  using appropriate IID.

  Also, other technology specific network objects can be obtained using this
  method. These include:
              INetworkUMTS
              INetwork1X
              INetwork1XPriv

  @param[in]  iid - Interface Id of the required interface.
  @param[out] ppo - Pointer to the interface returned.

  @see        IQI::QueryInterface()

  @return     AEE_SUCCESS on success
  @return     AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return     QDS_EFAULT - on invalid inputs.
  */
  virtual int CDECL QueryInterface
  (
    AEEIID                                  iid,
    void **                                 ppo
  );

  /*-------------------------------------------------------------------------
    INode interface Methods
  -------------------------------------------------------------------------*/
  /*!
  @function
  Process()

  @brief
  This method implements the INode interface's Process() function.

  @details
  This function is used in case of event handling. Whenever an event occurs
  the event manager singleton notifies the list of network objects in the
  system of the event via traversal. As part of the traversal (visitor
  design pattern), Process() method is called on individual network objects.
  If the event belongs to this network object, this method notifies the
  apps that have registered for this event.

  The following logic is used during this Process() function:
  1. Check if the event grou is EVENT_GROUP_NETWORK. This includes events
     possible for network objects: STATE, IP_ADDR_CHANGED etc.
  2. Check if event occurred on the same handle as this network object.
  3. Notify()

  The DSNET library also performs certain special operations in case of
  STATE changed events. Please see ProcessIfaceStateEvent()

  @param[in]  userDataPtr: Event info for the event.

  @see        ds::Utils::INode
  @see        ds::Utils::ITraverser
  @see        ds::Net::Network::ProcessIfaceStateEvent()

  @return     TRUE - If the Process() succeeds.
  @return     FALSE - If the Process() fails.
  */
  virtual boolean Process
  (
    void     *pUserData
  );

  /*-------------------------------------------------------------------------
    Forwarders for Handle.
  -------------------------------------------------------------------------*/
  /*!
  @function
  OnStateChange()

  @brief
  This function allows registering for events on the network object.

  @details
  This function can be used to register for events on interface that are
  implemented by the Network object.

              TODO:List out supported events.

  @param[in]  signalObj - ISignal used for registration.
  @param[in]  eventID - Event that should be registered for.

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments.
  @return     QDS_EOPNOTSUPP - Operation not supported.

  @see        None.
  */
  virtual ds::ErrorType CDECL OnStateChange
  (
    ISignal              *pISignal,
    ds::Net::EventType    eventID,
    IQI**                 regObj
  );

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );

  /*-------------------------------------------------------------------------
    Inherited functions from INetwork.
  -------------------------------------------------------------------------*/
  /*!
  @function
  GetTechObject()

  @brief
  This function allows object extensibility.
  For supported interfaces, objects implementing those interfaces may be
  fetched via this function.

  @details
  The supported interfaces are documented in
  the DS_NET_NetworkFactory.bid file. GetTechObject-supported interface
  does not imply any similar contract in regard to QueryInterface for the
  respective interface.
  Unlike IQI, the availability of a specific interface depends on some
  factors, e.g. current network type. Moreover, there is no guaranty
  for transitivity or symmetry.

  @param[in]  iid  - The interface that should be retrieved.
  @param[out] ppo  - On success, will contain the requested
                        interface instance.

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments
  @return     AEE_ECLASSNOTSUPPORT - Class ID is not supported.
  */
  virtual ds::ErrorType CDECL GetTechObject
  (
    AEEIID                                  iid,
    void**                                   ppo
  );

  /*!
  @function
  GetDeviceName()

  @brief
  This function gets the device name from the OS

  @details
  This function gets the device name from the OS for QNX only

  @param[out]  deviceName  - On success, will contain the requested device name

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetDeviceName
  (
    DeviceName * pDeviceName
  );

  /*!
  @function
  GetGatewayAddress()

  @brief
  This function returns the gateway's IP address

  @details
  This function returns the gateway's IP address

  @param[out]  gatewayAddr  - On success, will contain the gateway's IP address

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetGatewayAddress
  (
  ds::IPAddrType * gatewayAddr
  );

  /*!
  @function
  GetSubnetMask()

  @brief
  This function returns the subnet's mask

  @details
  This function returns the subnet's mask

  @param[out]  pSubnetMask  - On success, will contain the subnet's mask

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetSubnetMask
  (
    ds::IPAddrType * pSubnetMask
  );


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
  ) = 0;

  /*!
  @function
  GetState()

  @brief
  This function gets the status of the network subsystem.

  @details
  TODO

  @param[out] pNetState - Out parameter for network state.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::NetworkStateType
  */
  virtual ds::ErrorType CDECL GetState
  (
    NetworkStateType  *pNetState
  );

  /*!
  @function
  GetIfaceState()

  @brief
  Returns the state of the underlying network interface.

  @details
  TODO: Write details about the function
  TODO: How is this different from GetState()

  @param[out] pNetState - Out parameter for network state.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::NetworkStateType
  */
  virtual ds::ErrorType CDECL GetIfaceState
  (
    NetworkStateType  *pNetState
  );

  /*!
  @function
  GetLastNetDownReason()

  @brief
  This function gets the last network down reason as indicated through
  down indication of the iface.

  @details
  TODO

  @param[out] pNetDownReason - Out parameter for last network down reason.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::NetDownReasonType
  */
  virtual ds::ErrorType CDECL GetLastNetDownReason
  (
    NetDownReasonType   *pNetDownReason
  );

  /*!
  @function
  GetIPAddr()

  @brief
  This function gets the IP address for the iface associated with the
  network object.

  @details
  This function gets the IP address for the iface associated with the network
  object. If a valid iface handle is associated with the network object, this
              function returns that iface's IP address.

  @param[out] pIPAddr - Out parameter for network state.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::IPAddrType
  */
  virtual ds::ErrorType CDECL GetIPAddr
  (
    ds::IPAddrType    *pIPAddr
  );

    /*!
  @function
  GetNatPublicIPAddress()

  @brief
  This function gets the public IP address for the NAT iface.

  @details
  This function gets the public IP address for the NAT iface associated with
  the network object. If a valid iface handle is associated with the
  network object, this function returns that iface's IP address.

  @param[out] pIPAddr - Out parameter for network state.

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments

  @see        ds::IPAddrType
  */
  virtual ds::ErrorType CDECL GetNatPublicIPAddress
  (
    ds::IPAddrType    *pIPAddr
  );

  /*!
  @function
  GetSIPServerDomainNames()

  @brief
  This function gets the SIP server domain names

  @details
  TODO

  @param[out] pSipServerDomainNames - To hold the domain name.
  @param[in]  sipServerDomainNamesLen - Length allocated by client.
  @param[out] pSipServerDomainNamesLenReq - Number of domain names available.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetSIPServerDomainNames
  (
    DomainName   *pSipServerDomainNames,
    int           sipServerDomainNamesLen,
    int          *pSipServerDomainNamesLenReq
  );

  /*!
  @function
  GetSIPServerAddr()

  @brief
  This function gets the SIP server addresses.

  @details
  TODO

  @param[out] pSipServerAddresses - Out parameter for server addrs.
  @param[in]  sipServerAddressesLen - Length allocated by client.
  @param[out] pSipServerAddressesLenReq - If more buffer is required.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::IPAddrType
  */
  virtual ds::ErrorType CDECL GetSIPServerAddr
  (
    ds::IPAddrType   *pSipServerAddresses,
    int               sipServerAddressesLen,
    int              *pSipServerAddressesLenReq
  );

  /*!
  @function
  GetQosAware()

  @brief
  This function checks if the mobile is on a QOS Aware system.

  @details
  TODO

  @param[out] pGetQosAware - Boolean out param for QOS aware status.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetQosAware
  (
    boolean  *pGetQosAware
  );

  /*!
  @function
  GetOutageInfo()

  @brief
  This function gets the Outage notification info.

  @details
  TODO

  @param[out] pOutageInfo - Out param for outage info.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::OutageInfoType
  */
  virtual ds::ErrorType CDECL GetOutageInfo
  (
    OutageInfoType  *pOutageInfo
  );

  /*!
   @function
   GetFastDormStatusInfo()

   @brief
   Gets the result of an attempt at fast dormancy.

   @details
   This function gets the fast dormancy status returned from lower
   layers when fast dormancy completes. This info is retrieved from
   the network object.

   @param[out] uin8 - Out parameter for fast dormancy status.

   @see        No external dependencies

   @return     AEE_SUCCESS - on success
   @return     Error code on error.
   */
  virtual ds::ErrorType CDECL GetFastDormStatusInfo
  (
   uint8  *piFastDormStatusInfo
  );

  /*!
  @function
  GetBearerInfo()

  @brief
  Gets the bearer technology of the underlying interface.

  @details
  This function gets the bearer technology of the underlying interface.
  It calles the network iface IOCTL to get this information.

  @param[out] ppIBearerInfo - Out parameter for bearer technology.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IBearerTech
  */
  virtual ds::ErrorType CDECL GetBearerInfo
  (
    IBearerInfo  **ppIBearerInfo
  );

  /*!
  @function
  GetBearerTechEx()

  @brief
  Gets the bearer technology of the underlying interface.

  @details
  This function gets the bearer technology of the underlying interface.
  The function differs from the above GetBearerInfo function by returning
  a different type which represents the bearer technology.

  @param[out] ppIDSNetBearerTechEx - Out parameter for bearer technology.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IBearerTechEx
  */

  virtual ds::ErrorType CDECL GetBearerTechEx
  (
    IBearerTechEx **ppIDSNetBearerTechEx
  );

  /*!
  @function
  GetDataPathBridgeMode()

  @brief
  Gets data path for the underlying interface.

  @details
  This function gets the data path of the network object's underlying
  interface.

  @param[out] pGetDataPathBridgeMode - Out param for data path.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL GetDataPathBridgeMode
  (
    DataPathBridgeModeType* pGetDataPathBridgeMode
  );

  /*!
  @function
  GetOpResPco()

  @brief
  Gets OpResPco underlying interface.

  @details
  This function gets the OpResPco of the network object's underlying
  interface.

  @param[out] pGetOpResPco - Out param for OpResPco.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL GetOpResPco
  (
    NetworkOpReservedPcoType* pGetOpResPco
  );

  /*!
  @function
  SetDataPathBridgeMode()

  @brief
  Sets data path preference for the underlying interface.

  @details
  This function sets the data path preference of the network object's underlying
  interface.

  @param[in] pSetDataPathBridgeMode - Out param for data path.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL SetDataPathBridgeMode
  (
    DataPathBridgeModeType* pSetDataPathBridgeMode
  );

  /*!
  @function
  _3GPPLTEReestabCalltypeGet()

  @brief
  Gets 3GPP LTE Reestab Calltype for the underlying interface.

  @details
  This function gets the 3GPP LTE Reestab Calltype of the network object's underlying
  interface.

  @param[out] p3GPPLTEReestabCalltypeGet - Out param for 3GPP LTE Reestab Calltype

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL _3GPPLTEReestabCalltypeGet
  (
    _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeGet
  );

  /*!
  @function
  _3GPPLTEReestabCalltypeSet()

  @brief
  Sets 3GPP LTE Reestab Calltype for the underlying interface.

  @details
  This function sets the 3GPP LTE Reestab Calltype of the network object's underlying
  interface.

  @param[in] p3GPPLTEReestabCalltypeSet - Out param for 3GPP LTE Reestab Calltype

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL _3GPPLTEReestabCalltypeSet
  (
    _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeSet
  );

  /*!
  @function
  SetSilentRedial()

  @brief
  Sets whether the modem has to do a silent redial on the underlying interface.

  @details
  This function requests the modem to perform silent redial of the network  
  object's underlying interface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL SetSilentRedial
  (
    SilentRedialStruct* pSilentRedial
  );

/*
  @function
  _3GPPFORCEPDNdisconnect()

  @brief
   3GPP PDN disconnect for the underlying interface.

  @details
  This function does 3GPP PDN disconnect of the network object's underlying
  interface.

  @param[in] None

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL _3GPPFORCEPDNdisconnect
  (
   void
  );

  /*
    @function
    Setkeepalive()
  
    @brief
     Sets keep alive flag for the underlying interface.
  
    @details
    This function does Sets keep alive flag of the network object's underlying
    interface.
  
    @param[in] None
  
    @return     AEE_SUCCESS - on success
    @return     QDS_EFAULT - Invalid arguments
  
    @see        None.
    */

  virtual ds::ErrorType CDECL Setkeepalive
  (
   KeepAliveType pKeepalive
  );

  /*!
  @function
  GetAddressFamily()

  @brief
  Gets the address family for the underlying interface.

  @details
  This function gets the address family of the network object's underlying
  interface.

  @param[out] pAddressFamily- Out param for address family.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        TODO: cite families IP_V4, IP_V6, IP_ANY etc.
  */
  virtual ds::ErrorType CDECL GetAddressFamily
  (
    AddrFamilyType *pAddressFamily
  );

  /*!
  @function
  GetDomainNameSearchList()

  @brief
  This function gets the Domain name search list.

  @details
  TODO

  @param[out] pDomainNameSearchList - out param containing domain
              names search list.
  @param[in]  domainNameSearchListLen - Length of buffer passed by app.
  @param[out] pDomainNameSearchListLenReq - Total number of domain names
              search list buffers available.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::DomainName
  */
  virtual ds::ErrorType CDECL GetDomainNameSearchList
  (
    DomainName  *pDomainNameSearchList,
    int          domainNameSearchListLen,
    int         *pDomainNameSearchListLenReq
  );

  /*!
  @function
  GetNetMTU()

  @brief
  This function gets the MTU of the network subsystem.

  @details
  TODO

  @param[out] pMTU - Out parameter for MTU of the interface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetNetMTU
  (
    int    *pMTU
  );

  /*!
  @function
  GetHWAddress()

  @brief
  Gets the hardware address of the underlying iface.

  @details
  TODO

  @param[out] pHWAddr - Out parameter for hardware address.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::HWAddressType
  */
  virtual ds::ErrorType CDECL GetHWAddress
  (
    HWAddressType  *pHWAddr
  );

  /*!
  @function
  IsLaptopCallActive()

  @brief
  This function checks if the laptop call is active.

  @details
  TODO

  @param[out] pIsActive - Out parameter returning status

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL IsLaptopCallActive
  (
    boolean  *pIsActive
  );

  /*!
  @function
  DhcpArpCacheUpdate()

  @brief
  This function is used to to update device ARP cache with DHCP client's
  IP/MAC as per the lease offered by the server.

  @details
  TODO

  @param[in] dhcpArpCacheUpdateParam - parameter for updating the arp cache

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL DhcpArpCacheUpdate
  (
    const DhcpArpCacheUpdateType  *dhcpArpCacheUpdateParam
  );

  /*!
  @function
  DhcpArpCacheClear()

  @brief
  This function is used to to clear an entry from the ARP cache.

  @details
  TODO

  @param[in] dhcpArpCacheClearParam - parameter for clearing the arp cache

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL DhcpArpCacheClear
  (
    const DhcpArpCacheClearType  *dhcpArpCacheClearParam
  );

  /*!
  @function
  GetDhcpDeviceInfo()

  @brief
  This function is used to get the devices information which are connected
  to DHCP at any given point of time.

  @details
  TODO

  @param[out] connDevInfo - Out parameter containig DHCP connected devices info.

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GetDhcpDeviceInfo
  (
    DhcpGetDeviceInfoType *connDevInfo
  );

  /*!
  @function
  GoDormant()

  @brief
  Makes the underlying interface to go dormant.

  @details
  TODO

  @param[in]  dormantReason - Reason for forcing dormancy.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL GoDormant
  (
    DormantReasonType  dormantReason
  );

  /*!
  @function
  GoNull()

  @brief
  Brings down the physical interface.

  @details
  TODO

  @param[in]  nullReason - Reason for tearing down interface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        NULL reasons.
  */
  virtual ds::ErrorType CDECL GoNull
  (
    ReleaseReasonType  nullReason
  );

  /*!
  @function
  EnableDNSDuringIPCP()

  @brief
  This function enables DNS during IPCP

  @details
  TODO

  @param[in]  enable - In parameter for enable/disable IPCP DNS opt.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL EnableDNSDuringIPCP
  (
    boolean  enable
  );

  /*!
  @function
  RefreshDHCPConfigInfo()

  @brief
  This function refreshes DHCP config info.

  @details
  TODO

  @param      None.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  */
  virtual ds::ErrorType CDECL RefreshDHCPConfigInfo
  (
    void
  );

  /*!
  @function
  GetCurrRFCondition()

  @brief
  This function gets the current RF conditions.

  @details
  TODO

  @param[out] pRFCondition - Out param for current RF conditions.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::RFConditionType
  */
  virtual ds::ErrorType CDECL GetCurrRFCondition
  (
    RFConditionType  *pRFCondition
  );

  /*!
  @function
  CreateQoSManager()

  @brief
  Creates a QoS Manager associated with this network object.


  @details
  QoSManager is needed for secondary QoS objects. The QoSManager object
  implements IQoSManager interface.

  @param[out] ppIQoSManager - Returned QoS Manager.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  @return     AEE_ENOMEMORY - No memory to create QoS Manager.

  @see        Interface: IQoSManager
  */
  virtual ds::ErrorType CDECL CreateQoSManager
  (
    IQoSManager  **ppIQoSManager
  );
#ifdef FEATUTE_DATA_PS_MCAST
  /*!
  @function
  CreateMCastManager()

  @brief
  Creates a MCast Manager associated with this network object.


  @details
  MCastManager is needed for MCastSession objects. The QoSManager object
  implements IMCastManager interface.

  @param[out] ppIMCastManager - Returned MCast Manager.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments
  @return     AEE_ENOMEMORY - No memory to create MCast Manager.

  @see        Interface: IMCastManager
  */
  virtual ds::ErrorType CDECL CreateMCastManager
  (
    IMCastManager  **ppIMCastManager
  );
#endif //FEATUTE_DATA_PS_MCAST

  /*!
  @function
  CreateFirewallManager()

  @brief
  Creates a Firewall Manager associated with this network object.


  @details
  FirewallManager is needed for FirewallRule objects. The FirewallManager object
  implements IFirewallManager interface.

  @param[out] ppIFirewallManager - Returned Firewall Manager.

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments
  @return     DSS_ENOMEM - No memory to create MCast Manager.

  @see        Interface: IFirewallManager
  */
  virtual ds::ErrorType CDECL CreateNetFirewallManager
  (
    IFirewallManager  **ppIFirewallManager
  );
  /*!
  @function
  SetFMCTunnelParams()

  @brief
  Sets the FMC tunnel parameters for this network

  @details
  Sets the FMC tunnel parameters for this network

  @param[in] tunnelParams - FMC tunnel parameters.

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments

  @see        FMCTunnelParamsType
  */
  virtual ds::ErrorType CDECL SetFMCTunnelParams
  (
    const FMCTunnelParamsType* tunnelParams
  );

  /*!
  @function
  ResetFMCTunnelParams()

  @brief
  Resets the FMC tunnel parameters for this network

  @details
  Resets the FMC tunnel parameters for this network

  @return     SUCCESS - on success
  @return     DSS_EFAULT - Invalid arguments

  */
  virtual ds::ErrorType CDECL ResetFMCTunnelParams
  (
    void
  );

  /*!
  @function
  GetIfaceName()

  @brief
  Gets the name of the iface.

  @details
  TODO

  @param[out] pIfaceName - Returned iface name.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IfaceName
  */
  virtual ds::ErrorType CDECL GetIfaceName
  (
    ds::Net::IfaceNameType  *pIfaceName
  );

  /*!
  @function
  GetDHCPRefreshResult()

  @brief
  Returns the results of DHCP Refresh call.

  @details
  TODO

  @param[out] pDHCPRefreshResult - If the DHCP refresh passed or failed.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL GetDHCPRefreshResult
  (
    boolean  *pDHCPRefreshResult
  );

  /*!
  @function
  GetTXPhysLink()

  @brief
  Gets the TX phys link associated with network.

  @details
  TODO

  @param[out] ppIPhysLink - Returned TX Phys link object.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        IPhysLink
  */
  virtual ds::ErrorType CDECL GetTXPhysLink
  (
    IPhysLink **ppIPhysLink
  );

  /*!
  @function
  GetRXPhysLink()

  @brief
  Gets the RX phys link associated with network.

  @details
  TODO

  @param[out] ppIPhysLink - Returned RX Phys link object.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        IPhysLink
  */
  virtual ds::ErrorType CDECL GetRXPhysLink
  (
    IPhysLink  **ppIPhysLink
  );

  /*!
  @function
  GetDNSAddr()

  @brief
  Gets all the DNS addresses associated.

  @details
  TODO

  @param[out] pAddressList - List of DNS server addresses.
  @param[in]  addressListLen - Length of address list buffer passed in.
  @param[out] pAddressListLenReq - Total number of DNS addrs available.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::IPAddrType
  */
  virtual ds::ErrorType CDECL GetDNSAddr
  (
    ds::IPAddrType  *pDNSAddrs,
    int              dnsAddrsLen,
    int             *pDNSAddrsLenReq
  );

  /*!
  @function
  GetDormancyInfoCode()

  @brief
  Gets the Dormancy info code.

  @details
  TODO

  @param[out] pDormancyInfoCode - Dormancy info code.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::DormancyInfoCodeType
  */
  virtual ds::ErrorType CDECL GetDormancyInfoCode
  (
    ds::Net::DormancyInfoCodeType *pDormancyInfoCode
  );

  /*!
  @function
  GetQoSMode()

  @brief
  Gets the QoS Mode.

  @details
  TODO

  @param[out] pQosMode - Pointer to the returned QoS Mode.
  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::QoSModeType
  */
  virtual ds::ErrorType CDECL GetQoSMode
  (
    ds::Net::QoSModeType   *pQoSMode
  );

  /*!
  @function
  GetNetworkStatistics()

  @brief
  Provide network interface statistics.

  @details
  This method is only supported for INetworkPriv interface.

  @param[out] stats - structure to be populated with network interface
  statistics.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - if provided argument is 0

  @see        INetworkPriv
  */
  virtual ds::ErrorType CDECL GetNetworkStatistics
  (
    ds::Net::NetworkStatsType* stats
  );

  /*!
  @function
  ResetNetworkStatistics()

  @brief
  Resets network interface statistics.

  @details
  Resets network interface statistics.

  @param      None
  @return     SUCCESS - on success

  @see        DS::Net::GetNetworkStatistics
  */
  virtual ds::ErrorType CDECL ResetNetworkStatistics
  (
    void
  );

  /*-------------------------------------------------------------------------
    Methods from INetworkPriv interface.
  -------------------------------------------------------------------------*/
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
  only INetwork interface, wherein getting the interface would guarentee
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
  ) = 0;

  /*!
  @function
  LookupInterface()

  @brief
  Looks up for the network and sets the interface handle.


  @details
  This function just does a routing lookup based on the policy and sets
  the handle in the network. Thus this function is used to create a
  monitored network object, that can be used to listen on events that
  occur on the underlying network interface. New applications should use
  only INetwork interface, wherein getting the interface, would guarantee
  Network bringup.

  @param        None.

  @dependencies The network policy must already be initialized.

  @return       AEE_SUCCESS on successful start of the network.
  @return       QDS_EINVAL - Something invalid happened.
                TODO: How to notify if no interface can be found?

  @see          INetworkPriv.
  */
  virtual ds::ErrorType CDECL LookupInterface
  (
    void
  );

  /*!
  @function
  ForceLookupInterfaceByIfaceId()

  @brief
  Forces to looks up for the network according to a given iface ID
  and sets the interface handle.


  @details
  This function does a routing lookup based on the a given iface ID and sets
  the handle in the network. Thus this function is used to create a
  monitored network object, that can be used to listen on events that
  occur on the underlying network interface. New applications should use
  only INetwork interface, wherein getting the interface, would guarantee
  Network bringup.

  @param        pIfaceId - given iface ID.

  @dependencies The network policy must already be initialized.

  @return       AEE_SUCCESS on successful start of the network.
  @return       QDS_EINVAL - Something invalid happened.
                TODO: How to notify if no interface can be found?

  @see          INetworkPriv.
  */
  virtual ds::ErrorType CDECL ForceLookupInterfaceByIfaceId
  (
    ds::Net::IfaceIdType ifaceId
  );

  /*!
  @function
  NotifyNetDown2Monitored()

  @brief
  Notify NetworkMonitored object on network down event.

  @details
  This function is called when its corresponding
  NetworkActive is closing. It then sets the handle to 0,
  and notifies corresponding network event.
  Sockets are registered to this event on the network.
  This way sockets receive network change state event
  when the network is closing.

  @param        None.

  @dependencies None.

  @return       DS::SUCCESS.
  @return       Other ds designated error codes might be returned.

  @see          ds::Net::Handle::SetHandle()
  @see          ds::Net::Handle::Notify()
  */
  virtual ds::ErrorType AEEINTERFACE_CDECL NotifyNetDown2Monitored
  (
    void
  ) = 0;

  /*!
  @function
  GetStaleIfaceId()

  @brief
  This method returns the stale iface ID
  based on the stale handle attribute.
  This iface ID was the iface ID right before it became invalid.
  An iface ID is an ID identifying the underlying interface
  bound to this INetworkPrev object.

  @param[out] value of the stale iface ID

  @see        ds::Net::Handle::SetHandle()
  @see        ds::Net::IfaceIdType
  */
  virtual ds::ErrorType CDECL GetStaleIfaceId
  (
    ds::Net::IfaceIdType* pIfaceId
  );

  /*!
  @function
  GetPolicy()

  @brief
  Gets the policy object associated with the network.

  @details
  TODO

  @param[out] ppIPolicy - Returned network policy.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IPolicy.
  */
  virtual ds::ErrorType CDECL GetPolicy
  (
    IPolicy   **ppIPolicy
  );

  /*!
  @function
  SetPolicy()

  @brief
  Sets the policy object associated with the network.

  @details
  TODO

  @param[out] pIPolicy - network policy to be set.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IPolicy.
  */
  virtual ds::ErrorType CDECL SetPolicy
  (
    IPolicy  *pIPolicy
  );

  /*!
  @function
  GetAllIfaces()

  @brief
  Gets all the ifaces associated with network interface.

  @details
  TODO

  @param[out] pAllIfaces - Returned ifaces
  @param[in]  allIfacesLen - Buffer length passed in.
  @param[out] pAllIfacesLenReq - If more buffers are required.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IfaceIdType

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetAllIfaces
  (
    ds::Net::IfaceIdType   *pAllIfaces,
    int                     allIfacesLen,
    int                    *pAllIfacesLenReq
  );

  /*!
  @function
  GetIfaceId()

  @brief
  Returns the iface id if the interface.
  If this Network isn't bound to an iface,
  LookupInterface() shall be performed.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments.

  @see        ds::Net::IfaceIdType
  */
  virtual ds::ErrorType CDECL GetIfaceId
  (
    ds::Net::IfaceIdType  *pIfaceId
  );

  /*!
  @function
  GetIfaceIdNoLookup()

  @brief
  Returns the iface id if the interface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments.

  @see        ds::Net::IfaceIdType
  */
  virtual ds::ErrorType CDECL GetIfaceIdNoLookup
  (
    ds::Net::IfaceIdType  *pIfaceId
  );

  /*!
  @function
  GetPreviousState()

  @brief
  Gets the previous iface state.

  @details
  TODO

  @param[out] pNetState - Previous state of the iface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Network::StateType
  */
  virtual ds::ErrorType CDECL GetPreviousState
  (
    NetworkStateType  *pNetState
  );

  /*!
  @function
  GetPreviousIPAddr()

  @brief
  Gets the previous IP address of the interface.

  @details
  TODO

  @param[out] pIPAddr - Previous IP address of the iface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::IPAddrType.
  */
  virtual ds::ErrorType CDECL GetPreviousIPAddr
  (
    ds::IPAddrType *pIPAddr
  );

  /*!
  @function
  GetPreviousBearerInfo()

  @brief
  Gets the previous bearer tech object for this iface.

  @details
  TODO

  @param[out] ppIBearerInfo - Previous bearer tech obj.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        IBearerInfo
  */
  virtual ds::ErrorType CDECL GetPreviousBearerInfo
  (
    IBearerInfo  **ppIBearerInfo
  );

  /*!
  @function
  GetQoSAwareInfoCode

  @brief
  Gets the QoS aware event info code.

  @details
  TODO

  @param[out] pQoSInfoCode - Info code for QOS_AWARE_UNAWARE_EV.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::QoS::InfoCodeType
  */
  virtual ds::ErrorType CDECL GetQoSAwareInfoCode
  (
    QoSInfoCodeType  *pQoSInfoCode
  );

  /*-------------------------------------------------------------------------
    Inherited functions from IIPFilterManagerPriv.
  -------------------------------------------------------------------------*/
  /*!
  @function
  RegisterFilters()

  @brief
  Registers filters on the network interface.

  @details
  TODO

  @param[in]  sockID - Socket descriptor
  @param[in]  ppIFilterSpec - Array of filter specifications.
  @param[in]  filtersLen - Number of filters in the array.
  @param[in]  ppIIPFfilterReg - Returned IIPFilterRegPriv interface.
  @param[in]  iface_group - iface group.

  @see        IIPFilterRegPriv

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL RegisterFilters
  (
    int                 sockID,
    IIPFilterPriv       **ppIIPFilterSpec,
    int                 filtersLen,
    IIPFilterRegPriv    **ppIIPFfilterReg,
    IfaceGroupType      iface_group
  );

  /*!
  @function
  GetExtIpv6DelAddr()

  @brief
  Returns deleted external IPv6 address

  @details
  Returns deleted external IPv6 address

  @param[out]  value - deleted external IPv6 address

  @see        INAddr6Type

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetExtIpv6DelAddr
  (
    ::ds::INAddr6Type value
  );

  /*!
  @function
  GetExtIPConfigEx()

  @brief
  Returns ExtIPConfigEx Mask

  @details
  Returns ExtIPConfigEx mask

  @param[out]  value - ExtIPConfigEx mask

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetExtIPConfigEx
  (
    ::ds::Net::NetworkExtendedIpConfigExType* value
  );

    /*!
  @function
  GetApnParamInfo()

  @brief
  Returns ApnParamChangeList

  @details
  Returns ApnParamChangeList

  @param[out]  value - ApnParamChangeList

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetApnParamInfo
  (
    ApnParamChangeList* pGetApnParamInfo
  );

  boolean EventIsNetworkState
  (
    int32 eventId
  );

  EventType NetworkState2Event
  (
    NetworkStateType argNetState
  );

  /*!
  @function
  GetMipInfo()

  @brief
  Gets the MIP info of the underlying interface.

  @details
  This function gets the MIP info of the underlying interface.

  @param[out] pMipInfo - Out parameter for MIP info.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::Net::IBearerTechEx
  */

  virtual ds::ErrorType CDECL GetMipInfo
  (
    ds::IPAddrType* pMipInfo
  );

  /*!
  @function
  GetHandoffInfo()

  @brief
  Returns Handoffinfo

  @details
  Returns Handoffinfo

  @param[out]  value - Handoffinfo

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */

  virtual ds::ErrorType CDECL GetHandoffInfo
  (
   HandoffinfoType *value
  );


  /*!
  @function
  GetHandoffFailureInfo()

  @brief
  Returns HandoffFailureinfo

  @details
  Returns HandoffFailureinfo

  @param[out]  value - HandoffFailureinfo

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */

  virtual ds::ErrorType CDECL GetHandoffFailureInfo
  (
    HandoffFailureinfoType *value
  );

  
  /*!
  @function
  SetPDNTeardownConfig()

  @brief
  Sets pdn teardown preference for the underlying interface.

  @details
  This function sets the pdn teardown preference of the network object's underlying
  interface.

  @param[in] pPDNTeardownConfig - Out param for pdn teardown preference.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL SetPDNTeardownConfig
  (
    PDNTeardownConfigType* pPDNTeardownConfig
  );  

  /*!
  @function
  GetPDNTeardownConfig()

  @brief
  Gets pdn teardown preference for the underlying interface.

  @details
  This function gets the pdn teardown preference of the network object's underlying
  interface.

  @param[in] pPDNTeardownConfig - Out param for pdn teardown preference.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        None.
  */
  virtual ds::ErrorType CDECL GetPDNTeardownConfig
  (
    PDNTeardownConfigType* pPDNTeardownConfig
  );

  /*-------------------------------------------------------------------------
    Macros to define IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()


};/* class Network */
} /* namespace Net */
} /* namespace ds */

/*lint -restore */
#endif /* DS_NET_NETWORK_H */
