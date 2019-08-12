#ifndef DS_NET_INETWORKPRIV_H
#define DS_NET_INETWORKPRIV_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_IPolicyPriv.h"
#include "ds_Net_QoS_Def.h"
#include "ds_Net_IBearerInfo.h"
namespace ds
{
   namespace Net
   {
      struct _SeqIfaceIdType__seq_unsignedLong {
         IfaceIdType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIfaceIdType__seq_unsignedLong SeqIfaceIdType;
      typedef int IfaceNameTypePriv;
      namespace IfaceName
      {
         
         /**
           * IfaceNameTypePriv is part of the enumeration in IfaceNameType.
           * MBMS iface is moved here until MBMS feature is released publicly.
           */
         const ::ds::Net::IfaceNameTypePriv IFACE_MBMS = 0x8801;
      };
      struct Ipv4Addr {
         unsigned int ps_s_addr;
      };
      namespace ChaddrLen
      {
         const int MAX_CHADDR_LEN = 16;
      };
      namespace ChostnameLen
      {
         const int MAX_CHOSTNAME_LEN = 32;
      };
      struct DhcpServerConnDevicesInfo {
         Ipv4Addr client_ip;
         unsigned char client_hw[16];
         unsigned int client_hw_len;
         unsigned char client_hostname[32];
         unsigned int client_hostname_len;
         unsigned int client_connected_time;
      };
      struct _DhcpGetDeviceInfoListType__seq_DhcpServerConnDevicesInfo_Net_ds {
         DhcpServerConnDevicesInfo* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _DhcpGetDeviceInfoListType__seq_DhcpServerConnDevicesInfo_Net_ds DhcpGetDeviceInfoListType;
      struct DhcpGetDeviceInfoType {
         DhcpServerConnDevicesInfo* dev_info;
         int dev_infoLen;
         int dev_infoLenReq;
      };
      struct DhcpArpCacheType {
         unsigned int ip_addr;
         ::byte hw_addr[16];
         int hw_addr_len;
      };
      typedef DhcpArpCacheType DhcpArpCacheUpdateType;
      typedef DhcpArpCacheType DhcpArpCacheClearType;
      const ::AEEIID AEEIID_INetworkPriv = 0x1072cf1;
      
      /** @interface INetworkPriv
        * 
        * ds Net Network Privileged interface.
        * This interface is intended for internal usage for backward
        * compatibility purposes.
        * It provides additional functionalities to INetwork.
        * It exhibits different behavior than INetwork, specifically -
        * the Data Network is not brought up
        * automatically when INetworkPriv is instantiated.
        */
      struct INetworkPriv : public ::ds::Net::IEventManager
      {
         
         /**
           * This function explicitly brings up the network.        
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL BringUpInterface() = 0;
         
         /**
           * This function performs a route lookup and binds the INetworkPriv
           * object to an underlying interface object.
           * Route lookup is required if an application wants the
           * INetworkPriv object to be correctly bound to the appropriate 
           * underlying interface after it has changed the policy for the
           * INetworkPriv object.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL LookupInterface() = 0;

         /**
           * This function forces a route lookup even if INetworkPriv 
           * object is already bounded to an interface object.
           * It then binds the INetworkPriv object to an underlying
           * interface object.
           * Route lookup is required if an application wants the INetworkPriv
           * object to be correctly bound to the appropriate 
           * underlying interface after it has changed the policy for the
           * INetworkPriv object.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ForceLookupInterfaceByIfaceId(
           ds::Net::IfaceIdType ifaceId) = 0;
         
         /**
           * This function is called when its corresponding
           * NetworkActive is closing. It then sets the handle to 0,
           * and notifies corresponding network event.
           * Sockets are registered to this event on the network.
           * This way sockets receive network change state event
           * when the network is closing.
           * @param None.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds::Net::Handle::SetHandle()
           * @see ds::Net::Handle::Notify()
           */
         virtual ::AEEResult AEEINTERFACE_CDECL NotifyNetDown2Monitored() = 0;

         /**
           * This method returns the stale iface ID
           * based on the stale handle attribute.
           * This iface ID was the iface ID right before it became invalid.
           * An iface ID is an ID identifying the underlying interface
           * bound to this INetworkPrev object.
           * @param value stale iface ID value
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds::Net::Handle::SetHandle()
           * @see ds::Net::IfaceIdType
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetStaleIfaceId(ds::Net::IfaceIdType* ifaceId) = 0;
         
         /**
           * This function returns if laptop call active. "Laptop call" describes 
           * the use case of mobile device being used as a modem to connect
           * to the network. In that use case a computer connects to the mobile
           * device, for example, via USB.
           * This API can be used by privileged network applications that can 
           * piggyback a laptop connection rather than bring up an embedded data call.
           * Since ACTIVE network objects automatically bring up an embedded data call, 
           * there is no point of using this API on ACTIVE mode network objects. 
           * An application can use a MONITORED mode network object to find out 
           * if laptop call is active and, if not, create an ACTIVE mode network object 
           * in order to bring up an embedded data call and use it.
           * @param isActive Argument to hold if laptop call is active.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL IsLaptopCallActive(
           boolean* isActive) = 0;
         
         /**
           * This function is used to get iface ids of all the ifaces available
           * in the system.
           * @param allIfaces List of all the iface ids
           * @retval AEE_SUCCESS Request received successfully.    
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAllIfaces(
           ::ds::Net::IfaceIdType* allIfaces,
           int allIfacesLen,
           int* allIfacesLenReq) = 0;
         
         /**
           * This function is used to get the previous network state.
           * @param netState Holds the previous network state.
           * @see ds::Net::Network::StateType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPreviousState(
           ::ds::Net::NetworkStateType* netState) = 0;
         
         /**
           * This function is used to return the previous IP address 
           * of the interface.
           * @param address Buffer that holds the IP address.
           * @see ds::IPAddrType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPreviousIPAddr(
           ::ds::IPAddrType* address) = 0;
         
         /**
           * This function returns the bearer technology information.
           * Note: The Bearer Rate information in this case is not applicable
           * through the bearerTech.
           * @param bearerTech Object that will hold the bearer information.
           * @see IBearerInfo
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPreviousBearerInfo(
           ::ds::Net::IBearerInfo** bearerTech) = 0;
         
         /**
           * This function can be used to fetch infoCode regarding QOS_AWARE or
           * QOS_UNAWARE event.
           * @param infoCode Infocode that was cached from QOS_AWARE/QOS_UNAWARE 
           * event
           * See ds::Net::QoS::InfoCodeType.
           * @retval AEE_SUCCESS The current info code was successfully fetched
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetQoSAwareInfoCode(
           ::ds::Net::QoSInfoCodeType* infoCode) = 0;
         
         /**
           * This function gets the policy associated with the network.
           * This function is internal and is to be used by the sockets library
           * only. 
           * @param policy - Policy object.
           * See INetPolicy.
           * @retval AEE_SUCCESS The current info code was successfully fetched
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPolicy(
           ::ds::Net::IPolicy** policy) = 0;
         
         /**
           * This function sets the policy associated with the network.
           * This function is internal and is to be used by the sockets library
           * only. 
           * @param policy - Policy object.
           * See INetPolicy.
           * @retval AEE_SUCCESS The current info code was successfully fetched
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetPolicy(
           ::ds::Net::IPolicy* policy) = 0;
         
         /**
           * This function used to enable/disable DNS negotiation during
           * ipcp configuration.
           * @param enable Spicify if to enable / disable.
           * @retval ds::SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL EnableDNSDuringIPCP(
           boolean enable) = 0;
         
         /**
           * This function used to get the devices information which are 
           * connected to DHCP at any given point of time.
           * @param connDevInfo Contains the Connected devices information.
           * @retval DS::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDhcpDeviceInfo(
           ::ds::Net::DhcpGetDeviceInfoType* connDevInfo) = 0;
         
         /**
           * This function is used to to update device ARP cache with DHCP 
           * client's IP/MAC as per the lease offered by the server. This is 
           * used typically for cases where mode handler uses LAN LLC as 
           * MAC level protocol. Here when DHCP server assigns a lease to a 
           * client, it replies with a unicast IP packet to that client. 
           * This triggers LAN LLC to generate unicast ethernet type frames
           * for which the destination MAC needs to be resolved and thus, 
           * triggers ARP requests for the offered unicast address.Since the 
           * DHCP client would not have yet received the offer, it wouldn't 
           * reply to the ARP request by device and DHCP handshake would not
           * complete.If User specifies the callback, DHCP server updates the 
           * device ARP cache before sending unicast OFFERs/ACKs. 
           * Also while free-ing the lease, server would clear the same update 
           * from ARP cache.
           * @param dhcpArpCacheUpdateParam Contains the Entry information 
           * to be updated.
           * @retval DS::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL DhcpArpCacheUpdate(
           const ::ds::Net::DhcpArpCacheUpdateType* dhcpArpCacheUpdateParam) = 0;
         
         /**
           * This function is used to to clear device ARP cache with DHCP 
           * client's IP/MAC as per the lease offered by the server. This is 
           * used typically for cases where mode handler uses LAN LLC as 
           * MAC level protocol. Here when DHCP server assigns a lease to a 
           * client, it replies with a unicast IP packet to that client. 
           * This triggers LAN LLC to generate unicast ethernet type frames
           * for which the destination MAC needs to be resolved and thus, 
           * triggers ARP requests for the offered unicast address.Since the 
           * DHCP client would not have yet received the offer, it wouldn't 
           * reply to the ARP request by device and DHCP handshake would not
           * complete.If User specifies the callback, DHCP server updates the 
           * device ARP cache before sending unicast OFFERs/ACKs. 
           * Also while free-ing the lease, server would clear the same update 
           * from ARP cache.
           * @param dhcpArpCacheClearParam Contains the Entry information 
           * to be cleared.
           * @retval DS::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see ds_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL DhcpArpCacheClear(
           const ::ds::Net::DhcpArpCacheClearType* dhcpArpCacheClearParam) = 0;
      };
   };
};
#endif //DS_NET_INETWORKPRIV_H
