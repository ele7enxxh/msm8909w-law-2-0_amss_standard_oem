#ifndef DS_NET_IIPFILTERPRIV_H
#define DS_NET_IIPFILTERPRIV_H

/*============================================================================
  Copyright (c) 20088 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
namespace ds
{
   namespace Net
   {
      typedef int IPFilterIDType;
      namespace IPFilterID
      {
         const ::ds::Net::IPFilterIDType QDS_SRC_ADDR        = 101;
         const ::ds::Net::IPFilterIDType QDS_DST_ADDR        = 102;
         const ::ds::Net::IPFilterIDType QDS_NEXT_HDR_PROTO  = 103;
         const ::ds::Net::IPFilterIDType QDS_IPV4_TOS        = 201;
         const ::ds::Net::IPFilterIDType QDS_IPV6_TRF_CLASS  = 301;
         const ::ds::Net::IPFilterIDType QDS_IPV6_FLOW_LABEL = 302;
         const ::ds::Net::IPFilterIDType QDS_TCP_SRC_PORT    = 401;
         const ::ds::Net::IPFilterIDType QDS_TCP_DST_PORT    = 402;
         const ::ds::Net::IPFilterIDType QDS_UDP_SRC_PORT    = 501;
         const ::ds::Net::IPFilterIDType QDS_UDP_DST_PORT    = 502;
         const ::ds::Net::IPFilterIDType QDS_ESP_SPI         = 601;
         const ::ds::Net::IPFilterIDType QDS_AH_SPI          = 701;
         const ::ds::Net::IPFilterIDType QDS_ICMP_MTYPE      = 1000;
         const ::ds::Net::IPFilterIDType QDS_ICMP_MCODE      = 1001;
      };
      struct _SeqIPFilterIDType__seq_long {
         IPFilterIDType* data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIPFilterIDType__seq_long SeqIPFilterIDType;
      typedef unsigned char IPFilterIPVersionType;
      namespace IPFilterIPVersion
      {
         const ::ds::Net::IPFilterIPVersionType QDS_FILTER_IPV4 = 4;
         const ::ds::Net::IPFilterIPVersionType QDS_FILTER_IPV6 = 6;
      };
      typedef unsigned char IPFilterIPNextProtocolType;
      namespace IPFilterIPNextProtocol
      {
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_ICMP = 1;
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_TCP = 6;
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_UDP = 17;
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_ESP = 50;
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_AH  = 51;
         const ::ds::Net::IPFilterIPNextProtocolType QDS_FILTER_ICMP6 = 58;
      };
      typedef int IPFilterIPv6FlowLabelType;
      typedef unsigned int IPFilterSpiType;
      struct IPFilterIPv4AddrType {
         INAddrType addr;
         INAddrType subnetMask;
      };
      struct IPFilterTOSType {
         unsigned char val;
         unsigned char mask;
      };
      struct IPFilterPortType {
         INPortType port;
         unsigned short range;
      };
      struct IPFilterIPv6AddrType {
         ::ds::INAddr6Type addr;
         unsigned char prefixLen;
      };
      struct IPFilterIPv6TrafficClassType {
         unsigned char val;
         unsigned char mask;
      };
      struct IPFilterAuxInfoType {
         unsigned short fi_id;
         unsigned short fi_precedence;
      };
      const ::AEEIID AEEIID_IIPFilterPriv = 0x106dcc4;
      
      /** @interface IIPFilterPriv
        * 
        * ds IP Filter interface.
        * This interface provides a common base for all the possible values of
        * IP filter. 
        */
      struct IIPFilterPriv : public ::IQI
      {
         
         /**
           * This function creates an identical copy of the IIPFilter.
           * @param filter The created IIPFilter.
           * @retval ds::SUCCESS IIPFilter cloned successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL Clone(IIPFilterPriv** filter) = 0;
         
         /**
           * This attribute represents the valid options - if an option was set,
           * its ID will be in this list.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetValidOptions(::ds::Net::IPFilterIDType* value, int valueLen, int* valueLenReq) = 0;
         
         /**
           * This attribute represents a list of erroneous options into
           * the IIPFilterPriv object.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetErroneousOptions(::ds::Net::IPFilterIDType* value, int valueLen, int* valueLenReq) = 0;
         
         /**
           * This attribute indicates the IP version.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetIPVsn(::ds::Net::IPFilterIPVersionType* value) = 0;
         
         /**
           * This attribute indicates the IP version.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetIPVsn(::ds::Net::IPFilterIPVersionType value) = 0;
         
         /**
           * This attribute indicates the next header protocol.
           * It identifies the higher layer protocol (TCP/UDP) that needs to be
           * considered for filtering an IP packet. If this field is specified,
           * only IP packets belonging to the specified higher layer protocol
           * are considered for filtering. The filtering can be further enhanced
           * by specifying parameters from that protocol header fields.
           * Only parameters from the NextHdrProt are considered (other protocol
           * header fields are ignored).
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNextHdrProt(::ds::Net::IPFilterIPNextProtocolType* value) = 0;
         
         /**
           * This attribute indicates the next header protocol.
           * It identifies the higher layer protocol (TCP/UDP) that needs to be
           * considered for filtering an IP packet. If this field is specified,
           * only IP packets belonging to the specified higher layer protocol
           * are considered for filtering. The filtering can be further enhanced
           * by specifying parameters from that protocol header fields.
           * Only parameters from the NextHdrProt are considered (other protocol
           * header fields are ignored).
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetNextHdrProt(::ds::Net::IPFilterIPNextProtocolType value) = 0;
         
         /**
           * This attribute indicates the source port.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSrcPort(::ds::Net::IPFilterPortType* value) = 0;
         
         /**
           * This attribute indicates the source port.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSrcPort(const ::ds::Net::IPFilterPortType* value) = 0;
         
         /**
           * This attribute indicates the destination port.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDstPort(::ds::Net::IPFilterPortType* value) = 0;
         
         /**
           * This attribute indicates the destination port.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetDstPort(const ::ds::Net::IPFilterPortType* value) = 0;
         
         /**
           * This attribute indicates the source IPv4 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSrcV4(::ds::Net::IPFilterIPv4AddrType* value) = 0;
         
         /**
           * This attribute indicates the source IPv4 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSrcV4(const ::ds::Net::IPFilterIPv4AddrType* value) = 0;
         
         /**
           * This attribute indicates the destination IPv4 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDstV4(::ds::Net::IPFilterIPv4AddrType* value) = 0;
         
         /**
           * This attribute indicates the destination IPv4 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetDstV4(const ::ds::Net::IPFilterIPv4AddrType* value) = 0;
         
         /**
           * This attribute indicates the type of service.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTos(::ds::Net::IPFilterTOSType* value) = 0;
         
         /**
           * This attribute indicates the type of service.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetTos(const ::ds::Net::IPFilterTOSType* value) = 0;
         
         /**
           * This attribute indicates the IPv6 flow label.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetFlowLabel(::ds::Net::IPFilterIPv6FlowLabelType* value) = 0;
         
         /**
           * This attribute indicates the IPv6 flow label.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetFlowLabel(::ds::Net::IPFilterIPv6FlowLabelType value) = 0;
         
         /**
           * This attribute indicates the source IPv6 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetSrcV6(::ds::Net::IPFilterIPv6AddrType* value) = 0;
         
         /**
           * This attribute indicates the source IPv6 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetSrcV6(const ::ds::Net::IPFilterIPv6AddrType* value) = 0;
         
         /**
           * This attribute indicates the destination IPv6 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDstV6(::ds::Net::IPFilterIPv6AddrType* value) = 0;
         
         /**
           * This attribute indicates the destination IPv6 address.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetDstV6(const ::ds::Net::IPFilterIPv6AddrType* value) = 0;
         
         /**
           * This attribute indicates the traffic class.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTrafficClass(::ds::Net::IPFilterIPv6TrafficClassType* value) = 0;
         
         /**
           * This attribute indicates the traffic class.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetTrafficClass(const ::ds::Net::IPFilterIPv6TrafficClassType* value) = 0;
         
         /**
           * This attribute indicates the ESP SPI parameter value.
           * This is a Security Parameter Index as defined in RFC 2406.
           * If specified, the SPI field in ESP header shall be considered for
           * filtering an IP packet.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetEspSpi(int* value) = 0;
         
         /**
           * This attribute indicates the ESP SPI parameter value.
           * This is a Security Parameter Index as defined in RFC 2406.
           * If specified, the SPI field in ESP header shall be considered for
           * filtering an IP packet.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetEspSpi(int value) = 0;

         /**
           * This attribute indicates the AH SPI parameter value.
           * This is a Security Parameter Index as defined in RFC 2406.
           * If specified, the SPI field in AH header shall be considered for
           * filtering an IP packet.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetAhSpi(int* value) = 0;
         
         /**
           * This attribute indicates the AH SPI parameter value.
           * This is a Security Parameter Index as defined in RFC 2406.
           * If specified, the SPI field in AH header shall be considered for
           * filtering an IP packet.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetAhSpi(int value) = 0;
               
         /**
           * This attribute indicates the Filter ID and precedence value.
           */
         /**
           * This attribute indicates the ICMP type.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetICMPType(unsigned char* value) = 0;
         
         /**
           * This attribute indicates the Filter ID and precedence value.
           */
         /**
           * This attribute indicates the ICMP type.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetICMPType(unsigned char value) = 0;
         
         /**
           * This attribute indicates the ICMP code.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetICMPCode(unsigned char* value) = 0;
         
         /**
           * This attribute indicates the ICMP code.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetICMPCode(unsigned char value) = 0;
         virtual ::AEEResult AEEINTERFACE_CDECL GetAuxInfo(::ds::Net::IPFilterAuxInfoType* value) = 0;
      };
      struct _SeqIIPFilterPrivType__seq_IIPFilterPriv_Net_ds {
         IIPFilterPriv** data;
         int dataLen;
         int dataLenReq;
      };
      typedef _SeqIIPFilterPrivType__seq_IIPFilterPriv_Net_ds SeqIIPFilterPrivType;
   };
};
#endif //DS_NET_IIPFILTERPRIV_H
