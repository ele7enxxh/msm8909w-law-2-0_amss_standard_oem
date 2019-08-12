#ifndef DS_NET_INETWORK_H
#define DS_NET_INETWORK_H

/*============================================================================
  Copyright (c) 2008 - 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
namespace ds
{
   namespace Net
   {
      namespace NetworkEvent
      {
         
         /**
           * Data network state change - deprecated - do not use 0x106e60f
           */
         //const ::ds::Net::EventType QDS_EV_STATE_CHANGED = 0x106e60f;

         /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_UP = 0x106e613;

         /**
           * Data network going down
           */
         const ::ds::Net::EventType QDS_EV_IFACE_GOING_DOWN = 0x106e619;

         /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_ENABLED = 0x106e61a;

         /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_DISABLED = 0x106e61b;

         /**
           * Data network down
           */
         const ::ds::Net::EventType QDS_EV_IFACE_DOWN = 0x106e61c;

          /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_ACTIVE_OUT_OF_USE = 0x106e61d;

         /**
           * Data network coming up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_COMING_UP = 0x106e61f;

          /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_CONFIGURING = 0x106e621;

         /**
           * Data network up
           */
         const ::ds::Net::EventType QDS_EV_IFACE_ROUTEABLE = 0x106e622;

         /**
           * IP address change
           */
         const ::ds::Net::EventType QDS_EV_IP_ADDR_CHANGED = 0x106e610;

         /**
           * Outage (mobile handoff) event
           */
         const ::ds::Net::EventType QDS_EV_OUTAGE = 0x106e614;

         /**
           * // RF Condition event
           */
         const ::ds::Net::EventType QDS_EV_RF_CONDITIONS_CHANGED = 0x106e617;

         /**
          * // Fast Dormancy Status event
          */
         const ::ds::Net::EventType QDS_EV_FAST_DORM_STATUS = 0x106e620;

         /**
          * // External IPv6 Address deleted event
          */
         const ::ds::Net::EventType QDS_EV_IPV6_EXT_ADDR_DEL_EV = 0x106e623;
      };
      typedef int NetworkStateType;
      typedef uint32 NetworkExtendedIpConfigExType;
      namespace NetworkState
      {

         /**
           * CLOSED: When an ACTIVE MODE Network object reaches this state it
           *         should be released. 
           *         MONITORED MODE Network object shall be in this state when
           *         the network connection it is monitoring is down.
           *         It is valid for MONITORED MODE Network object to remain in
           *         this state (as the application may wish to continue
           *         monitoring the network interface).
           *         Most operations invoked while the Network object is in this
           *         state will fail.
           *         @See INetwork::Stop
           */
         const ::ds::Net::NetworkStateType QDS_CLOSED = 0x2;

         /**
           * OPEN_IN_PROGRESS: An ACTIVE MODE Network object may be in this
           *                   state upon its creation.
           *                   MONITORED MODE Network object shall be in this
           *                   state when the network connection it is monitoring 
           *                   is in the process of being established.          
           *                   Most operations invoked while the Network object
           *                   is in this state will fail.
           */
         const ::ds::Net::NetworkStateType QDS_OPEN_IN_PROGRESS = 0x4;

         /**
           * OPEN: This is the normal state of an ACTIVE MODE Network object.
           *       MONITORED MODE Network object shall be in this state when
           *       the network connection it is monitoring is established.          
           *       @See INetworkControl::GoDormant
           */
         const ::ds::Net::NetworkStateType QDS_OPEN = 0x20;

         /**
           * CLOSE_IN_PROGRESS: An ACTIVE MODE Network object may be in this
           *                    state after Stop was called.
           *                    A MONITORED MODE Network object may be in this
           *                    state after GoNull was called, or, in general,
           *                    when the network connection it is monitoring
           *                    is in the process of being torn down.
           *                    Most operations invoked while the Network object
           *                    is in this state will fail.
           */
         const ::ds::Net::NetworkStateType QDS_CLOSE_IN_PROGRESS = 0x40;

         /**
           * LINGERING: The underlying network interface is not used anymore by
           *            any client, but it stays in OPEN state for a while to
           *            optimize a scenario where a client starts using it again
           *            closely after the last client stopped using it.
           *            This state is applicable only for MONITORED MODE Network
           *            objects.
           */
         const ::ds::Net::NetworkStateType QDS_LINGERING = 0x80;
      };
      typedef int RFConditionType;
      namespace RFCondition
      {
         const ::ds::Net::RFConditionType QDS_BAD = 1;
         const ::ds::Net::RFConditionType QDS_GOOD = 2;
      };
      typedef unsigned char DomainName[256];
      typedef unsigned char DeviceName[256];
      typedef int OutageStateType;
      namespace OutageState
      {

         /**
           * The state of network outage cannot be determined and information
           * regarding expected outage cannot be provided.
           * The timeToOutage provided is not valid and should be ignored.
           */
         const ::ds::Net::OutageStateType QDS_INVALID = 0;

         /**
           * The OutageInfo provided is valid.
           */
         const ::ds::Net::OutageStateType QDS_VALID = 1;

         /**
           * By the time the application has queried for the network outage
           * information, the outage has actually already started.
           * i.e. The device is currently in network outage state.
           */
         const ::ds::Net::OutageStateType QDS_STARTED = 2;

         /**
           * By the time the application has queried for the network outage
           * information, the outage has already occurred and finished.
           * The device is not in network outage state anymore
           */
         const ::ds::Net::OutageStateType QDS_EXPIRED = 3;
      };
      struct OutageInfoType {
         OutageStateType state;
         int timeToOutage;
         int duration;
      };

      namespace HWAddress
      {
         const int QDS_MAX_HW_ADDR = 6;
      };
      struct HWAddressType {
         unsigned char len;
         unsigned char hwAddr[6];
      };

      typedef int DataPathType;
      namespace DataPath
      {
        const int QDS_DATA_PATH_HARDWARE = 0;
        const int QDS_DATA_PATH_SOFTWARE = 1;
      };

      struct DataPathBridgeModeType {
        DataPathType dataPath;
      };

      typedef int _3GPPLTEReestabCalltypeType;
      namespace _3GPPLTEReestabCalltype
      {
        const int QDS_3GPP_LTE_REESTAB_CALLTYPE_NORMAL = 0;
        const int QDS_3GPP_LTE_REESTAB_CALLTYPE_VOLTE = 1;
      };

      struct _3GPPLTEReestabCalltypeStruct {
        _3GPPLTEReestabCalltypeType _3gpplteReestabCalltype;
      };

      struct ApnParamList {
        char apn_name[101];
        /**< Container to store APN name*/

        uint32 pdp_type;
        /**< Container to store PDN's IP Type */
      };

      typedef uint32 ApnParamMaskType;

      struct ApnParamChangeList {
        ApnParamMaskType apn_mask;
        /**< Apn Mask to specify which parameter changed*/
        ApnParamList oldApnParams;
        ApnParamList newApnParams;
      };

      typedef boolean SilentRedialType;

      struct SilentRedialStruct {
        SilentRedialType silentRedialType;
      };

      struct NetworkStatsType {
         uint64 bytesRX;
         uint64 bytesTX;
         unsigned int pktsRX;
         unsigned int mcastPktsRX;
         unsigned int pktsDroppedRX;
         unsigned int pktsTX;
         unsigned int mcastPktsTX;
         unsigned int pktsDroppedTX;
         unsigned short state;
         AEEINTERFACE_PADMEMBERS(__pad, 6)
      };
      
      struct NetworkOpReservedPcoType {
        uint16  mcc;
        uint16  mnc;
        boolean mnc_includes_pcs_digit;
        uint8 app_specific_info_len;
        uint8  app_specific_info[255];
        uint16 container_id;
      };

      typedef boolean KeepAliveType;

      struct PDNTeardownConfigType {
        boolean local_abort;
      };
      
      /**
        * Number of bytes received on this iface since the last reset. 
        */
      /**
        * Number of bytes sent via this iface since the last reset. 
        */
      /**
        * Total bytes transferred via this iface (since last reset upto now). 
        */
      /**
        * Number of bytes transferred via this iface during current connection. 
        */
      /**
        * Number of packets received on this iface since the last reset. 
        */
      /**
        * Number of multicast packets received on this iface since the last reset. 
        */
      /**
        * Number of dropped packets received on this iface since the last reset. 
        */
      /**
        * Number of packets sent via this iface since the last reset. 
        */
      /**
        * Number of multicast packets sent via this iface since the last reset. 
        */
      /**
        * Number of dropped packets sent via this iface since the last reset. 
        */
      /**
        * Time in seconds for this iface been open (since last reset upto now). 
        */
      /**
        * Time in seconds for this iface been active (between first IO after last reset upto last IO).
        */
      /**
        * Time in seconds since the last IO on this iface. 
        */
      /**
        * Time in seconds since last time this iface was brought up. 
        */
      /**
        * State of this iface since the last reset. 
        */
      const ::AEEIID AEEIID_INetwork = 0x106c546;

      /** @interface INetwork
        * 
        * ds Network interface.
        * This is the main interface for accessing information of a Data
        * Connection and controlling it.
        * A instance of this interface can only be created via INetworkFactory.
        * If created in ACTIVE mode, the network is brought up upon creation
        * (@See CreateNetwork). The ACTIVE object is not reusable once the
        * network has been shut down.
        * For creating QoS and MCast objects @See interface
        * ds_Net_INetworkExt.
        *
        * Events that can be registered on this interface via OnStateChange
        * (as part of IEventManager interface): 
        * - ds::Net::NetworkEvent::STATE_CHANGED. Use GetState to fetch the
        *   state information.
        * - ds::Net::NetworkEvent::IP_ADDR_CHANGED. Use GetIPAddr to fetch the
        *   IP address.
        * - ds::Net::NetworkEvent::OUTAGE. Use GetOutageInfo to fetch Outage
        *   information.
        * - ds::Net::NetworkEvent::RF_CONDITIONS_CHANGED. Use GetCurrRFCondition
        *   to fetch current RF conditions.
        */
      struct INetwork : public ::ds::Net::IEventManager
      {
         struct _SeqIPAddrType__seq_IPAddrType_ds {
            ::ds::IPAddrType* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqIPAddrType__seq_IPAddrType_ds SeqIPAddrType;
         struct _SeqDomainNameType__seq_octet_Net_ds {
            ::ds::Net::DomainName* data;
            int dataLen;
            int dataLenReq;
         };
         typedef _SeqDomainNameType__seq_octet_Net_ds SeqDomainNameType;

         /**
           * This attribute indicates the network state.
           * When an ACTIVE MODE Network object reach a CLOSED state it should
           * be released.
           * Operations invoked on such object in such state shall fail.
           * @param value Attribute value
           * @see ds::Net::NetworkStateType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetState(::ds::Net::NetworkStateType* value) = 0;

         /**
           * This attribute indicates the name of the iface.          
           * @param value Attribute value
           * @see ds::Network::IfaceName
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceName(::ds::Net::IfaceNameType* value) = 0;

         /**
           * This attribute indicates an ID identifying the underlying interface
           * bound to this INetwork object. If this INetwork isn't bound to an iface,
           * LookupInterface() shall be performed.
           * Iface ID is relevant for socket options SO_TX_IFACE and IP_RECVIF
           * as well as for using iface id as scopeId in IPv6 addresses.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceId(::ds::Net::IfaceIdType* value) = 0;

         /**
           * This attribute indicates an ID identifying the underlying interface
           * bound to this INetwork object. If this INetwork isn't bounded to an iface,
           * LookupInterface() shall not be performed and PS_IFACE_INVALID_ID is returned.
           * Iface ID is relevant for socket options SO_TX_IFACE and IP_RECVIF
           * as well as for using iface id as scopeId in IPv6 addresses.         
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIfaceIdNoLookup(::ds::Net::IfaceIdType* value) = 0;

         /**
           * This attribute indicates the last network down reason.
           * This attribute has a valid value if the network went down at least
           * once.
           * @param value Attribute value
           * @see ds::Net::NetDownReasonType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetLastNetDownReason(::ds::Net::NetDownReasonType* value) = 0;

         /**
           * This attribute indicates the IP address of the local host or device,
           * in network byte order.          
           * @param value Attribute value
           * @see ds::IPAddrType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIPAddr(::ds::IPAddrType* value) = 0;

         /**
           * This function is used to return all the DNS server addresses for
           * the network interface associated with the network object.
           * If available, the Primary DNS address shall always be specified
           * in the returned sequence before the Secondary address.
           * If only a single address is specified in the returned sequence
           * it should be treated as the Primary address.
           * @param addressList Buffer that holds the IP addresses.
           * @see SeqIPAddrType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDNSAddr(::ds::IPAddrType* DNSAddresses, int DNSAddressesLen, int* DNSAddressesLenReq) = 0;

         /**
           * This function returns the SIP (Session Initiation Protocol) server
           * Domain Names for this network.
           * @param sipServerDomainNames Buffer array that holds the SIP server
           * domain names.
           * @see SeqDomainNameType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSIPServerDomainNames(::ds::Net::DomainName* sipServerDomainNames, int sipServerDomainNamesLen, int* sipServerDomainNamesLenReq) = 0;

         /**
           * This function returns the SIP (Session Initiation Protocol) server IP
           * addresses for this network.
           * @param sipServerAddresses Buffer array that holds the SIP server
           * addresses.
           * @see SeqIPAddrType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSIPServerAddr(::ds::IPAddrType* sipServerAddresses, int sipServerAddressesLen, int* sipServerAddressesLenReq) = 0;

         /**
           * This function is used to return the public IP address of the NAT iface, in
           * network byte order.
           * @param address Buffer that holds the IP address.
           * @see DS::IPAddrType.
           * @retval DS::SUCCESS Request received successfully.
           * @retval Other DS designated error codes might be returned.
           * @see DS_Errors.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNatPublicIPAddress(::ds::IPAddrType* address) = 0;

         /**
           * This attribute holds information of the latest outage event
           * associated with this data network.          
           * @param value Attribute value
           * @see ds::Network::Outage::Info
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetOutageInfo(::ds::Net::OutageInfoType* value) = 0;

         /**
           * This attribute indicates the IP Address family of this INetwork
           * object.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAddressFamily(::ds::AddrFamilyType* value) = 0;
         
         /**
           * This function returns the Domain Name Search List for this network.
           * @param domainNameSearchList Buffer array that holds the domain name
           * search list.
           * @see SeqDomainNameType.
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDomainNameSearchList(::ds::Net::DomainName* domainNameSearchList, int domainNameSearchListLen, int* domainNameSearchListLenReq) = 0;

         /**
           * This attribute returns the IP Layer Maximum Transmission Unit
           * associated with this network.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNetMTU(int* value) = 0;

         /**
           * This attribute indicates the HW address associated with this network.
           * @param value Attribute value
           * @retval AEE_SUCCESS Request received successfully.
           * @retval QDS_EINVAL Iface does not support HW address.
           * @retval Other ds designated error codes might be returned.
           * @see ds::Net::Network::HWAddressType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetHWAddress(::ds::Net::HWAddressType* value) = 0;

         /**
           * This function is used to explicitly bring down the network
           * connection.
           * Note that the applicable Network interface shall be brought down as
           * well ONLY if no other entities are using it when this function is
           * called. Still, the application can expect STATE_CHANGED event if it
           * is registered. The application may hold the Network object after
           * calling this API, but it should release it once the Network moves 
           * to QDS_CLOSED state.          
           * 
           * @retval AEE_SUCCESS Request received successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Stop() = 0;

         /**
           * This attribute indicates the current RF condition.          
           * @param value Attribute value
           * @see ds::Net::Network::RFConditionType.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetCurrRFCondition(::ds::Net::RFConditionType* value) = 0;

         /**
           * This attribute indicates the current fast dormancy status.
           * @param value Attribute value
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetFastDormStatusInfo(uint8* value) = 0;

         /**
           * This attribute points the Tx physical link object.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTXPhysLink(::ds::Net::IPhysLink** value) = 0;

         /**
           * This attribute points the Rx physical link object.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRXPhysLink(::ds::Net::IPhysLink** value) = 0;

         /**
           * This attribute indicates the dormancy info code when default phys
           * link is dormant.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDormancyInfoCode(::ds::Net::DormancyInfoCodeType* value) = 0;

         /**
           * This attribute provides info on Network related statistics
           * @param value Attribute value
           * @param stats Output Network Statistics
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNetworkStatistics(::ds::Net::NetworkStatsType* value) = 0;

         /**
           * This attribute resets Network related statistics
           * @param None
           */
         virtual ::AEEResult AEEINTERFACE_CDECL ResetNetworkStatistics() = 0;

         /**
           * This function allows object extensibility.
           * For supported interfaces, objects implementing those interfaces may be 
           * fetched via this function. The supported interfaces are documented in
           * the DS_NET_NetworkFactory.bid file. GetTechObject-supported interface
           * does not imply any similar contract in regard to QueryInterface for the
           * respective interface.
           * Unlike IQI, the availability of a specific interface depends on some 
           * factors, e.g. current network type. Moreover, there is no guaranty 
           * for transitivity or symmetry. 
           * Note: 'interface' parameter will map to iid and techObj.
           * @param iid The interface that should be retrieved.
           * @param techObj On success, will contain the requested interface instance.
           * @retval ds::SUCCESS Interface retrieved successfully.
           * @retval AEE_ECLASSNOTSUPPORT Specified interface unsupported.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTechObject(AEEIID techObj_iid, void** techObj) = 0;

         /**
           * This function queries the active iface for its OS name.
           * @param deviceName The device name returned by the OS.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDeviceName(::ds::Net::DeviceName* deviceName) = 0;

         /**
           * This function returns the Gateway's IP address.
           * @param gatewayAddr The gateway's IP address.
           */

         virtual ::AEEResult AEEINTERFACE_CDECL GetGatewayAddress(ds::IPAddrType* gatewayAddr) = 0;
         /**
           * This function returns the subnet's mask.
           * @param pSubnetMask The subnet's mask.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSubnetMask(::ds::IPAddrType* pSubnetMask) = 0;
         /**
           * This function returns External deleted IPv6 Address.
           * @param value External deleted IPv6 Address.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetExtIpv6DelAddr(::ds::INAddr6Type value) = 0;
         /**
           * This function returns ExtIPConfigEx mask.
           * @param value ExtIPConfigEx mask.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetExtIPConfigEx(::ds::Net::NetworkExtendedIpConfigExType* value) = 0;
         /**
           * This function returns Apn param information .
           * @param value ApnParamChangeList .
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetApnParamInfo(::ds::Net::ApnParamChangeList* value) = 0;

         /**
           * This function returns data path.
           * @param value data path.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDataPathBridgeMode(
           DataPathBridgeModeType* pGetDataPathBridgeMode) = 0;

         /**
           * This function returns OpResPco.
           * @param value OpResPco.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetOpResPco(
           NetworkOpReservedPcoType* pGetOpResPco) = 0;

         /**
           * This function sets the data path preference.
           * @param value is data path preference to be set.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetDataPathBridgeMode(
           DataPathBridgeModeType* pSetDataPathBridgeMode) = 0;

         /**
           * This function returns 3GPP LTE Reestab Calltype.
           * @param value 3GPP LTE Reestab Calltype.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL _3GPPLTEReestabCalltypeGet(
           _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeGet) = 0;

         /**
           * This function sets the 3GP PLTE Reestab Calltype.
           * @param value is 3GPP LTE Reestab Calltype to be set.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL _3GPPLTEReestabCalltypeSet(
           _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeSet) = 0;

         /**
           * This function request the modem to perform a silent redial.
           * 
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSilentRedial(
           SilentRedialStruct* pSilentRedialtype) = 0;

         /**
         * This function disconnects PDN.
         */
         virtual ::AEEResult AEEINTERFACE_CDECL _3GPPFORCEPDNdisconnect() = 0;

         /**
         * This function sets keep alive.
         */
         virtual ::AEEResult AEEINTERFACE_CDECL Setkeepalive(
           KeepAliveType pKeepalive) = 0;

         /**
           * This function returns MIP info.
           * @param pMipInfo MIP info.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetMipInfo(::ds::IPAddrType* pMipInfo) = 0;

         /**
           * This function sets the pdn teardown config preference.
           * @param value is pdn teardown config preference to be set.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetPDNTeardownConfig(
           PDNTeardownConfigType* pPDNTeardownConfig) = 0;   

         /**
           * This function gets the pdn teardown config preference.
           * @param value is pdn teardown config preference to be returned.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetPDNTeardownConfig(
           PDNTeardownConfigType* pPDNTeardownConfig) = 0;      

      };
   };
};
#endif //DS_NET_INETWORK_H
