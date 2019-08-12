#ifndef DS_NET_DEF_H
#define DS_NET_DEF_H

/*============================================================================
  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"

/**
  * ds Net module.
  * This module groups all the general Net types and constants.
  */

/**
  * EventType: Event Type declaration. All Network subsystem interfaces that provide registration for events
  * use this event type to declare their events.
  * The specific events are declared in each Network subsystem interface.
  */
typedef int ds_Net_EventType;

/**
  * Iface ID identifies the underlying interface bound to an INetwork
  * object.
  * @See INetwork_IfaceId
  * Iface ID is relevant for socket options SO_TX_IFACE and IP_RECVIF
  * as well as for using iface id as scopeId in IPv6 addresses.
  */
typedef unsigned int ds_Net_IfaceIdType;
#define ds_Net_INVALID_IFACE_ID 0x0

/**
  * IfaceGroup: Network interface specification in network policy. All interfaces that belong
  * to the specified group shall be compatible to the policy.
  */
typedef int ds_Net_IfaceGroupType;
#define ds_Net_IfaceGroup_GRP_ANY_DEFAULT 0x1
#define ds_Net_IfaceGroup_GRP_WWAN 0x2
#define ds_Net_IfaceGroup_GRP_BCAST_MCAST 0x8
#define ds_Net_IfaceGroup_GRP_NET_3GPP 0x10
#define ds_Net_IfaceGroup_GRP_NET_3GPP2 0x20
#define ds_Net_IfaceGroup_GRP_NET_EPC   0x40
typedef int ds_Net_IfaceNameType;
#define ds_Net_IfaceName_IFACE_CDMA_SN 0x8001
#define ds_Net_IfaceName_IFACE_CDMA_AN 0x8002
#define ds_Net_IfaceName_IFACE_UMTS 0x8004
#define ds_Net_IfaceName_IFACE_SIO 0x8008
#define ds_Net_IfaceName_IFACE_CDMA_BCAST 0x8010
#define ds_Net_IfaceName_IFACE_WLAN 0x8020
#define ds_Net_IfaceName_IFACE_FLO 0x8080
#define ds_Net_IfaceName_IFACE_DVBH 0x8100
#define ds_Net_IfaceName_IFACE_STA 0x8200
#define ds_Net_IfaceName_IFACE_IPSEC 0x8400
#define ds_Net_IfaceName_IFACE_LOOPBACK 0x8800
#define ds_Net_IfaceName_IFACE_RESERVED1 0x8801
#define ds_Net_IfaceName_IFACE_IWLAN_3GPP 0x8802
#define ds_Net_IfaceName_IFACE_IWLAN_3GPP2 0x8804
#define ds_Net_IfaceName_IFACE_SLIP 0x8810
#define ds_Net_IfaceName_IFACE_UICC 0x8820
#define ds_Net_IfaceName_IFACE_MODEM_LINK_LOCAL 0x8888

/**
  * These info codes provide information of dormancy related conditions.
  */
typedef int ds_Net_DormancyInfoCodeType;
#define ds_Net_DormancyInfoCode_QDS_NOT_SPECIFIED 201

/** @memberof ds_Net_DormancyInfoCode
  * 
  * BUSY: Resuming Network connection from Dormancy failed since the 
  * network is busy (either network congestion does not allow the
  * connection to be resumed or because network resources for the
  * specific application class are not currently available.
  * The application should decide on the appropriate retry mechanism.
  */
#define ds_Net_DormancyInfoCode_QDS_BUSY 202
typedef int ds_Net_DormantReasonType;
#define ds_Net_DormantReason_QDS_NONE 1
typedef unsigned char ds_Net_INAddrType[4];
typedef unsigned char ds_Net_INPortType[2];
#else /* C++ */
#include "ds_Utils_StdDef.h"

/**
  * ds Net module.
  * This module groups all the general Net types and constants.
  */
namespace ds
{
   namespace Net
   {
      
      /**
        * EventType: Event Type declaration. All Network subsystem interfaces that provide registration for events
        * use this event type to declare their events.
        * The specific events are declared in each Network subsystem interface.
        */
      typedef int EventType;
      
      /**
        * Iface ID identifies the underlying interface bound to an INetwork
        * object.
        * @See INetwork::IfaceId
        * Iface ID is relevant for socket options SO_TX_IFACE and IP_RECVIF
        * as well as for using iface id as scopeId in IPv6 addresses.
        */
      typedef unsigned int IfaceIdType;
      const IfaceIdType INVALID_IFACE_ID = 0x0;
      
      /**
        * IfaceGroup: Network interface specification in network policy. All interfaces that belong
        * to the specified group shall be compatible to the policy.
        */
      typedef int IfaceGroupType;
      namespace IfaceGroup
      {
         const ::ds::Net::IfaceGroupType GRP_ANY_DEFAULT = 0x1;
         const ::ds::Net::IfaceGroupType GRP_WWAN = 0x2;
         const ::ds::Net::IfaceGroupType GRP_BCAST_MCAST = 0x8;
         const ::ds::Net::IfaceGroupType GRP_NET_3GPP = 0x10;
         const ::ds::Net::IfaceGroupType GRP_NET_3GPP2 = 0x20;
	 const ::ds::Net::IfaceGroupType GRP_NET_EPC = 0x40;
      };
      typedef int IfaceNameType;
      namespace IfaceName
      {
         const ::ds::Net::IfaceNameType IFACE_CDMA_SN = 0x8001;
         const ::ds::Net::IfaceNameType IFACE_CDMA_AN = 0x8002;
         const ::ds::Net::IfaceNameType IFACE_UMTS = 0x8004;
         const ::ds::Net::IfaceNameType IFACE_SIO = 0x8008;
         const ::ds::Net::IfaceNameType IFACE_CDMA_BCAST = 0x8010;
         const ::ds::Net::IfaceNameType IFACE_WLAN = 0x8020;
         const ::ds::Net::IfaceNameType IFACE_FLO = 0x8080;
         const ::ds::Net::IfaceNameType IFACE_DVBH = 0x8100;
         const ::ds::Net::IfaceNameType IFACE_STA = 0x8200;
         const ::ds::Net::IfaceNameType IFACE_IPSEC = 0x8400;
         const ::ds::Net::IfaceNameType IFACE_LOOPBACK = 0x8800;
         const ::ds::Net::IfaceNameType IFACE_RESERVED1 = 0x8801;
         const ::ds::Net::IfaceNameType IFACE_IWLAN_3GPP = 0x8802;
         const ::ds::Net::IfaceNameType IFACE_IWLAN_3GPP2 = 0x8804;
         const ::ds::Net::IfaceNameType IFACE_SLIP = 0x8810;
         const ::ds::Net::IfaceNameType IFACE_UICC = 0x8820;
         const ::ds::Net::IfaceNameType IFACE_EPC = 0x8880;
         const ::ds::Net::IfaceNameType IFACE_MODEM_LINK_LOCAL = 0x8888;
      };
      
      /**
        * These info codes provide information of dormancy related conditions.
        */
      typedef int DormancyInfoCodeType;
      namespace DormancyInfoCode
      {
         const ::ds::Net::DormancyInfoCodeType QDS_NOT_SPECIFIED = 201;
         
         /**
           * BUSY: Resuming Network connection from Dormancy failed since the 
           * network is busy (either network congestion does not allow the
           * connection to be resumed or because network resources for the
           * specific application class are not currently available.
           * The application should decide on the appropriate retry mechanism.
           */
         const ::ds::Net::DormancyInfoCodeType QDS_BUSY = 202;
      };
      typedef int DormantReasonType;
      namespace DormantReason
      {
         const ::ds::Net::DormantReasonType QDS_NONE = 1;
      };
      typedef unsigned char INAddrType[4];
      typedef unsigned char INPortType[2];

      /**
        * SubsInfo: Subscription related information
        */
      typedef unsigned int SubsInfoType;
      namespace SubsInfo
      {
         const ::ds::Net::SubsInfoType DEFAULT_SUBS   = 0x00;
         const ::ds::Net::SubsInfoType PRIMARY_SUBS   = 0x01;
         const ::ds::Net::SubsInfoType SECONDARY_SUBS = 0x02;
         const ::ds::Net::SubsInfoType TERTIARY_SUBS  = 0x03;
      };
      /**
        * APNInfo: APN related information
        */
      typedef unsigned int ApnInfoType;
      namespace ApnInfo
      {
         const ::ds::Net::ApnInfoType UNSPECIFIED_APN   = 0x00;
         const ::ds::Net::ApnInfoType IMS_APN           = 0x01;
      };
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_NET_DEF_H
