#ifndef DS_PROFILE_3GPP_H
#define DS_PROFILE_3GPP_H

/** 
  @file  ds_profile_3gpp.h 
  @brief Definitions associated with profile related functions for UMTS
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dsProfile group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2009-2016 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_profile_3gpp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/12   sk      Added field emergency_calls_are_supported.
06/27/12   kv      Support for mode T in Apn bearer field.
01/25/12   rr      Added support for profile apn bearer
09/13/11   ttv     Changes to add src_stat_desc as part of profile param.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/12/11   ars     Added comments for Doxygen
07/21/11   ttv     Added changes required for source statistics descriptor.
04/19/11   ttv     Changes to have unique identifiers for apn related 
                   parameters.
03/02/11   mg      doxygen comments
02/28/11   ars     Fixed compiler error caused by enabling -fy compiler option
02/24/11   hs      Added changes to support APN class in profile
02/11/11   ttv     Added changes for consolidated profile family.
01/26/11   ars     Completed the support for APN disable flag and inactivity
                   timer. Added enums and get/set api
01/14/11   ss      Added support for Iface linger parameters.
05/12/10   hs      Added support for APN Disable flag and Inactivity timer
10/15/09   vk      Added support for LTE params
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/
/* group: dsprofile */

#include "comdef.h"    
#include "ds_profile.h"        

/** @addtogroup constants 
  @{
*/
/* Constant definitions
*/

/* MAX Number of PDN Throttling Timers */
#define DS_PROFILE_3GPP_FAILURE_TIMER_MAX        10

/** TFT Filter ID 1. */
#define DS_PROFILE_3GPP_TFT_FILTER_ID1     0
/** TFT Filter ID 2. */
#define DS_PROFILE_3GPP_TFT_FILTER_ID2     1
/** Maximum number of TFT parameter sets. */
#define DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS 2 
/** Maximum length of the PDP context APN string. */
#define DS_PROFILE_3GPP_MAX_APN_STRING_LEN   100 
/** Maximum length of the username and password. */
#define DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN 127
/** Maximum length of the profile name. */  		
#define DS_PROFILE_3GPP_MAX_PROFILE_NAME_LEN 15
/** Maximum length of the OTAP network access point ID. */ 
#define DS_PROFILE_3GPP_MAX_OTAP_NAPID_LEN   48 

/* Profile Family type. 
*/
/** Embedded profile family for all socket calls. */
#define DS_PROFILE_3GPP_SOCKETS_PROFILE_FAMILY DS_PROFILE_EMBEDDED_PROFILE_FAMILY
/** Tethered profile family for all ATCoP calls. */
#define DS_PROFILE_3GPP_ATCOP_PROFILE_FAMILY   DS_PROFILE_TETHERED_PROFILE_FAMILY
/** Tethered profile family for all RmNET calls. */
#define DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY   DS_PROFILE_TETHERED_PROFILE_FAMILY
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/** IP version.
*/ 
typedef PACKED enum { 
  DS_PROFILE_3GPP_IP_V4 = 4,    /**< IP version IPv4. */
  DS_PROFILE_3GPP_IP_V6 = 6,    /**< IP version IPv6. */
  DS_PROFILE_3GPP_IP_V4V6 = 10, /**< IP version IPv4v6. */ 
  /** @cond
  */
  DS_PROFILE_3GPP_IP_MAX = 0xff /**< Internal use only. */
  /** @endcond */
} PACKED_POST ds_profile_3gpp_ip_version_enum_type;

/** PDP access control.
*/
typedef enum 
{
  DS_PROFILE_3GPP_PDP_ACCESS_CONTROL_NONE       = 0x0,
    /**< No PDP Access control. */
  DS_PROFILE_3GPP_PDP_ACCESS_CONTROL_REJECT     = 0x1,
    /**< PDP Access control is set to Reject. */
  DS_PROFILE_3GPP_PDP_ACCESS_CONTROL_PERMISSION = 0x2,
    /**< PDP Access control is set to Permission. */
  /** @cond 
  */
  DS_PROFILE_3GPP_PDP_ACCESS_CONTROL_MAX        = 0xff  /**< Internal use only. */
  /** @endcond */
}ds_profile_3gpp_pdp_access_control_e_type;

/** PDP type.
*/
typedef enum
{
  DS_PROFILE_3GPP_PDP_IP = 0x0,          /**< PDP type is IP. */
  DS_PROFILE_3GPP_PDP_PPP,               /**< PDP type is PPP. */
  DS_PROFILE_3GPP_PDP_IPV6,              /**< PDP type is IPv6. */
  DS_PROFILE_3GPP_PDP_IPV4V6,            /**< PDP type is IPv4v6. */
  /** @cond
  */
  DS_PROFILE_3GPP_PDP_MAX = 0xff         
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte. 
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_pdp_type_enum_type;

/** Authentication type for a PDP context.
*/
typedef enum
{
  DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED = 0x0,
    /**< No authentication.  */
  DS_PROFILE_3GPP_AUTH_PREF_PAP_ONLY_ALLOWED     = 0x1,
    /**< PAP authentication. */
  DS_PROFILE_3GPP_AUTH_PREF_CHAP_ONLY_ALLOWED    = 0x2,
    /**< CHAP authentication. */
  DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_ALLOWED     = 0x3,
    /**< PAP/CHAP authentication. */
  /** @cond
  */
  DS_PROFILE_3GPP_AUTH_PREF_MAX                  = 0xff
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
} ds_profile_3gpp_auth_pref_type;


#define  DS_PROFILE_3GPP_APN_BEARER_FLAG_G   (0x1)  /**< Bearer G. */  
#define  DS_PROFILE_3GPP_APN_BEARER_FLAG_W   (0x2)  /**< Bearer W. */   
#define  DS_PROFILE_3GPP_APN_BEARER_FLAG_L   (0x4)  /**< Bearer L. */  
#define  DS_PROFILE_3GPP_APN_BEARER_FLAG_T   (0x8) /**< Bearer T*/
#define  DS_PROFILE_3GPP_APN_BEARER_FLAG_ALL (0xff) /**< Bearer Any. */       

/** QoS traffic class type.
*/
typedef enum
{
  DS_PROFILE_3GPP_TC_SUBSCRIBED     = 0x0,   /**< Subscribed. */
  DS_PROFILE_3GPP_TC_CONVERSATIONAL = 0x1,   /**< Conversational. */
  DS_PROFILE_3GPP_TC_STREAMING      = 0x2,   /**< Streaming. */
  DS_PROFILE_3GPP_TC_INTERACTIVE    = 0x3,   /**< Interactive. */
  DS_PROFILE_3GPP_TC_BACKGROUND     = 0x4,   /**< Background. */
  /** @cond
  */
  DS_PROFILE_3GPP_TC_RESERVED       = 0xff   
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_traffic_class_type;

/** QoS delivery order type.
*/
typedef enum
{
  DS_PROFILE_3GPP_DO_SUBSCRIBED = 0x0,       /**< Subscribed. */
  DS_PROFILE_3GPP_DO_ON         = 0x1,       /**< With a delivery order. */
  DS_PROFILE_3GPP_DO_OFF        = 0x2,       /**< Without a delivery order. */
  /** @cond
  */
  DS_PROFILE_3GPP_DO_RESERVED   = 0xff       
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_qos_delivery_order_type;

/** QoS SDU error codes. Refer to 3GPP TS 24.008 @xhyperref{S35,[S35]}, 
  Section 10.5.6.5.
*/   
typedef enum
{
  DS_PROFILE_3GPP_SDU_ERR_RATIO_SUBSCRIBE = 0x0,  /**< Subscribed. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG2    = 0x1,  /**< 1E-2. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_7ENEG3    = 0x2,  /**< 7E-3. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3    = 0x3,  /**< 1E-3. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4    = 0x4,  /**< 1E-4. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG5    = 0x5,  /**< 1E-5. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG6    = 0x6,  /**< 1E-6. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG1    = 0x7,  /**< 1E-1. */
  DS_PROFILE_3GPP_SDU_ERR_RATIO_MAX       = 0x8,  /**< Maximum value. */
  /** @cond
  */
  DS_PROFILE_3GPP_SDU_ERR_RESERVED        = 0xff  
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_sdu_err_ratio_type;

/** QoS RES BER codes. Refer to 3GPP TS 24.008 @xhyperref{S35,[S35]}, 
  Section 10.5.6.5.
*/
typedef enum
{
  DS_PROFILE_3GPP_RESIDUAL_BER_SUBSCRIBE  = 0x0,  /**< Subscribed. */
  DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG2     = 0x1,  /**< 5E-2. */
  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG2     = 0x2,  /**< 1E-2. */
  DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG3     = 0x3,  /**< 5E-3. */
  DS_PROFILE_3GPP_RESIDUAL_BER_4ENEG3     = 0x4,  /**< 4E-3. */
  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG3     = 0x5,  /**< 1E-3. */
  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG4     = 0x6,  /**< 1E-4. */
  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5     = 0x7,  /**< 1E-5. */
  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG6     = 0x8,  /**< 1E-6. */
  DS_PROFILE_3GPP_RESIDUAL_BER_6ENEG8     = 0x9,  /**< 6E-8. */
  /** @cond
  */
  DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED   = 0xff  
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_residual_ber_ratio_type;

/** QoS Erroneous SDU delivery options. Refer to 3GPP TS 24.008 
  @xhyperref{S35,[S35]}, Section 10.5.6.5.
*/
typedef enum
{
  DS_PROFILE_3GPP_DELIVER_ERR_SDU_SUBSCRIBE  = 0x0,
    /**< Subscribed. */
  DS_PROFILE_3GPP_DELIVER_ERR_SDU_NO_DETECT  = 0x1,
    /**< No detection. */
  DS_PROFILE_3GPP_DELIVER_ERR_SDU_DELIVER    = 0x2,
    /**< Erroneous SDU delivered. */
  DS_PROFILE_3GPP_DELIVER_ERR_SDU_NO_DELIVER = 0x3,
    /**< Erroneous SDU not delivered. */
  /** @cond
  */
  DS_PROFILE_3GPP_DELIVER_ERR_SDU_RESERVED   = 0xff
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_deliver_err_sdu_type;

/** @brief IPv4 or IPv6 addresses.
*/
typedef PACKED struct PACKED_POST
{
  /** Type of IPv6 address. */
  PACKED union PACKED_POST
  {
    uint8   u6_addr8[16]; /**< IPv6 8-bit PDP address size. */
    uint16  u6_addr16[8]; /**< IPv6 16-bit PDP address size. */ 
    uint32  u6_addr32[4]; /**< IPv6 32-bit PDP address size. */ 
    uint64  u6_addr64[2]; /**< IPv6 64-bit PDP address size. */ 
  }in6_u;  
}ds_profile_3gpp_pdp_addr_type_ipv6;
 
/** @brief Holds IPv4 or IPv6 addresses.
*/
typedef PACKED struct PACKED_POST
{
  /** PDP Address. */
  PACKED struct PACKED_POST
  {
    uint32 ds_profile_3gpp_pdp_addr_ipv4; 
      /**< IPv4 PDP address. */  
    ds_profile_3gpp_pdp_addr_type_ipv6 ds_profile_3gpp_pdp_addr_ipv6;
      /**< IPv6 PDP address. */
  }ds_profile_3gpp_pdp_addr;
#define ds_profile_3gpp_pdp_addr_ipv4 ds_profile_3gpp_pdp_addr.ds_profile_3gpp_pdp_addr_ipv4
 /**< IPv4 PDP address. */
#define ds_profile_3gpp_pdp_addr_ipv6 ds_profile_3gpp_pdp_addr.ds_profile_3gpp_pdp_addr_ipv6
 /**< IPv6 PDP address. */
}ds_profile_3gpp_pdp_addr_type;

/** PDP header compression types.
*/
typedef PACKED enum 
{
  DS_PROFILE_3GPP_PDP_HEADER_COMP_OFF = 0,  
    /**< PDP header compression is off. */
  DS_PROFILE_3GPP_PDP_HEADER_COMP_ON  = 1,  
    /**< Manufacturer preferred compression. */
  DS_PROFILE_3GPP_PDP_HEADER_COMP_RFC1144,  
    /**< PDP header compression based on RFC 1144 @xhyperref{S36,[S36]}. */
  DS_PROFILE_3GPP_PDP_HEADER_COMP_RFC2507,  
    /**< PDP header compression based on RFC 2507 @xhyperref{S9,[S9]}. */
  DS_PROFILE_3GPP_PDP_HEADER_COMP_RFC3095,  
    /**< PDP header compression based on RFC 3095 @xhyperref{S10,[S10]}. */ 
  /** @cond 
  */
  DS_PROFILE_3GPP_PDP_HEADER_COMP_MAX =0xFF 
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte.
      Internal use only. */
  /** @endcond */
} PACKED_POST ds_profile_3gpp_pdp_header_comp_e_type;


/** PDP data compression types. Refer to TS 44.065 @xhyperref{S15,[S15]},
  Section 6.6.1.1.4. 
*/
typedef PACKED enum 
{
  DS_PROFILE_3GPP_PDP_DATA_COMP_OFF = 0,   
    /**< PDP data compression is off. */
  DS_PROFILE_3GPP_PDP_DATA_COMP_ON  = 1,   
    /**< Manufacturer preferred compression. */
  DS_PROFILE_3GPP_PDP_DATA_COMP_V42_BIS,   
    /**< V.42BIS data compression. */
  DS_PROFILE_3GPP_PDP_DATA_COMP_V44,       
    /**< V.44 data compression. */
  /** @cond
  */
  DS_PROFILE_3GPP_PDP_DATA_COMP_MAX = 0xFF 
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte. 
      Internal use only. */
  /** @endcond */
} PACKED_POST ds_profile_3gpp_pdp_data_comp_e_type;

/** IPv4 address allocation mechanism.
*/ 
typedef PACKED enum
{
  DS_PROFILE_3GPP_PDP_IPV4_ADDR_ALLOC_NAS = 0,    
    /**< Address allocation using NAS. */
  DS_PROFILE_3GPP_PDP_IPV4_ADDR_ALLOC_DHCPV4 = 1, 
    /**< Address allocation using DHCPv4. */
  /** @cond
  */
  DS_PROFILE_3GPP_PDP_IPV4_ADDR_ALLOC_MAX = 0xFF /**< Internal use only. */
  /** @endcond */
} PACKED_POST ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type;

/** LTE QoS class identifier values.
*/
typedef PACKED enum
{
  DS_PROFILE_3GPP_LTE_QCI_0 = 0,   /**< LTE QoS QCI is 0. */
  DS_PROFILE_3GPP_LTE_QCI_1 = 1,   /**< LTE QoS QCI is 1. */ 
  DS_PROFILE_3GPP_LTE_QCI_2 = 2,   /**< LTE QoS QCI is 2. */
  DS_PROFILE_3GPP_LTE_QCI_3 = 3,   /**< LTE QoS QCI is 3. */ 
  DS_PROFILE_3GPP_LTE_QCI_4 = 4,   /**< LTE QoS QCI is 4. */
  DS_PROFILE_3GPP_LTE_QCI_5 = 5,   /**< LTE QoS QCI is 5. */ 
  DS_PROFILE_3GPP_LTE_QCI_6 = 6,   /**< LTE QoS QCI is 6. */
  DS_PROFILE_3GPP_LTE_QCI_7 = 7,   /**< LTE QoS QCI is 7. */ 
  DS_PROFILE_3GPP_LTE_QCI_8 = 8,   /**< LTE QoS QCI is 8. */
  DS_PROFILE_3GPP_LTE_QCI_9 = 9,   /**< LTE QoS QCI is 9. */ 
  DS_PROFILE_3GPP_LTE_QCI_INVALID, /**< LTE QoS QCI is invalid. */
  /** @cond 
  */
  DS_PROFILE_3GPP_LTE_QCI_MAX = 0xFF  /**< Internal use only. */
  /** @endcond */
} PACKED_POST ds_profile_3gpp_lte_qci_e_type;

/** Type of PDP context number. */
typedef byte ds_profile_3gpp_pdp_context_number_type;
/** Type of secondary PDP context flag. */
typedef boolean ds_profile_3gpp_pdp_context_secondary_flag_type;
/** Type of primary PDP context ID. */
typedef byte ds_profile_3gpp_pdp_context_primary_id_type;
/** Type of IPv4 PDP address. */
typedef uint32   ds_profile_3gpp_pdp_addr_type_ipv4;
/** Type of proxy-call session control function address flag. */
typedef boolean  ds_profile_3gpp_request_pcscf_address_flag_type;
/** Type of proxy-call session control function using DHCP flag. */
typedef boolean  ds_profile_3gpp_request_pcscf_address_using_dhcp_flag_type;
/** Type of domain name server using DHCP flag. */
typedef boolean  ds_profile_3gpp_request_dns_address_using_dhcp_flag_type;
/** Type of IP multimedia core network flag. */
typedef boolean  ds_profile_3gpp_im_cn_flag_type;
/** Type of APN disable flag. */
typedef boolean  ds_profile_3gpp_apn_disable_flag_type;
/** Type of APN inactivity timer value. */
typedef uint32   ds_profile_3gpp_inactivity_timer_val_type;
/** Type of APN class. */
typedef uint8    ds_profile_3gpp_apn_class_type;
/** Type of  flag used to mark profiles that can support
    emergency calls */
typedef boolean ds_profile_3gpp_emergency_calls_are_supported;

/** @brief Stores the PDP authentication parameters.
*/
typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_auth_pref_type  
            auth_type;                             
    /**< Authentication type. */
  byte      password[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
    /**< Password/secret string. */
  byte      username[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
    /**< Username string. */
}ds_profile_3gpp_pdp_auth_type;

/** @brief Stores the UMTS QoS parameters.
*/
typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_traffic_class_type 
              traffic_class;               /**< Traffic class. */
  uint32      max_ul_bitrate;              /**< Maximum upload bitrate. */
  uint32      max_dl_bitrate;              /**< Maximum download bitrate. */
  uint32      gtd_ul_bitrate;              /**< Guaranteed upload bitrate. */
  uint32      gtd_dl_bitrate;              /**< Guaranteed download bitrate. */
  ds_profile_3gpp_qos_delivery_order_type                                     
              dlvry_order;                 /**< SDU delivery order. */
  uint32      max_sdu_size;                /**< Maximum SDU size. */
  ds_profile_3gpp_sdu_err_ratio_type                                          
              sdu_err;                     /**< SDU error ratio index. */
  ds_profile_3gpp_residual_ber_ratio_type                                     
              res_biterr;                  /**< Residual bit error index. */
  ds_profile_3gpp_deliver_err_sdu_type                                        
              dlvr_err_sdu;                /**< Delivery of error SDU. */
  uint32      trans_delay;                 /**< Transfer delay. */
  uint32      thandle_prio;                /**< Traffic handling priority. */
  boolean     sig_ind;                     /**< Signaling indication flag. */
}ds_profile_3gpp_3gpp_qos_params_type;

/** @brief Stores the GPRS QoS parameters.
*/
typedef PACKED struct PACKED_POST
{
  uint32      precedence;                  /**< Precedence class. */
  uint32      delay;                       /**< Delay class. */
  uint32      reliability;                 /**< Reliability class. */
  uint32      peak;                        /**< Peak throughput class. */
  uint32      mean;                        /**< Mean throughput class. */
}ds_profile_3gpp_gprs_qos_params_type;

/** @brief Stores the LTE QoS parameters.
*/
typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_lte_qci_e_type qci;             /**< QoS class identifier. */
  uint32                         g_dl_bit_rate;   /**< Guaranteed download bitrate. */
  uint32                         max_dl_bit_rate; /**< Maximum download bitrate. */
  uint32                         g_ul_bit_rate;   /**< Guaranteed upload bitrate. */
  uint32                         max_ul_bit_rate; /**< Maximum upload bitrate. */
} ds_profile_3gpp_lte_qos_params_type;

/** @brief Stores the DNS parameters for the PDP profile.
*/
typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_pdp_addr_type  primary_dns_addr;  /**< Primary DNS address. */
  ds_profile_3gpp_pdp_addr_type  secondary_dns_addr;/**< Secondary DNS address. */
}ds_profile_3gpp_dns_addresses_type;
  
/** @brief Stores the traffic flow template parameters.
*/
typedef PACKED struct PACKED_POST
{
  /** Type of TFT address. */
    PACKED union PACKED_POST
  {
    uint32 ds_profile_3gpp_tft_addr_ipv4; 
      /**< Type of IPv4 TFT address. */
    ds_profile_3gpp_pdp_addr_type_ipv6 ds_profile_3gpp_tft_addr_ipv6;
      /**< Type of IPv6 TFT address. */
  }ds_profile_3gpp_tft_addr;
#define ds_profile_3gpp_tft_addr_ipv4 ds_profile_3gpp_tft_addr.ds_profile_3gpp_tft_addr_ipv4
  /**< Type of IPv4 TFT address. */
#define ds_profile_3gpp_tft_addr_ipv6 ds_profile_3gpp_tft_addr.ds_profile_3gpp_tft_addr_ipv6
  /**< Type of IPv6 TFT address. */
}ds_profile_3gpp_tft_addr_type;

typedef PACKED union PACKED_POST
{
 uint32 mask;                      /**< Address mask for IPv4 address. */
 uint8  prefix_len;                /**< Prefix length for IPv6 address. */
   /*~ CASE DS_IP_V4 ds_umts_mask_type.mask */
   /*~ CASE DS_IP_V6 ds_umts_mask_type.prefix_len */
   /*~ DEFAULT ds_umts_mask_type.mask */
} ds_profile_3gpp_mask_type;

/** @brief Stores the types for the TFT source IP address and IP subnet mask. */
typedef PACKED struct PACKED_POST
{
  ds_profile_3gpp_tft_addr_type   address;  /**< IPv4v6 address.         */
  ds_profile_3gpp_mask_type       mask;     /**< IPv4v6 subnet mask.     */
}ds_profile_3gpp_address_mask_type;

/** @brief Stores the ranges for the TFT source and destination ports. */
typedef PACKED struct PACKED_POST
{
  uint16      from; /**< Range lower limit. */
  uint16      to;   /**< Range upper limit. */
} ds_profile_3gpp_port_range_type;

/** @brief Stores the TFT parameters. */
typedef PACKED struct PACKED_POST
{
  byte                               filter_id;    
    /**< Filter identifier. */
  byte                               eval_prec_id; 
    /**< Evaluation precedence index. */
  ds_profile_3gpp_ip_version_enum_type ip_version; 
    /**< IP version for address. */
  ds_profile_3gpp_address_mask_type  src_addr;     
    /**< Source address and mask. */
  byte                prot_num;    
    /**< Protocol number in IPv6 is next_header. */
  ds_profile_3gpp_port_range_type    dest_port_range; 
    /**< Destination port range. */
  ds_profile_3gpp_port_range_type    src_port_range;  
    /**< Source port range. */
  uint32                             ipsec_spi;  
    /**< Security parameter index. */
  uint16              tos_mask;    
    /**< Type of service and mask is the traffic class in IPv6. */
  uint32                             flow_label;      
    /**< Flow label. */
}ds_profile_3gpp_tft_params_type;



typedef PACKED struct PACKED_POST
{
  uint16 mnc_digits;   
  /**< actual mnc digit */
  boolean mnc_includes_pcs_digit;
  /**< This field is used to interpret the length of the corresponding
    MNC reported Values:
   - TRUE -- MNC is a three-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 090

  - FALSE -- MNC is a two-digit value; e.g., a reported value of 
    90 corresponds to an MNC value of 90 */
}ds_profile_3gpp_mnc_type;

/** 4 byte opaque user data. */
typedef uint32  ds_profile_3gpp_user_app_data;

/**< PDN Throttle Timers */
typedef uint32 ds_profile_3gpp_failure_timers_type[DS_PROFILE_3GPP_FAILURE_TIMER_MAX];

/**< Override Home PDP type */
typedef enum
{
  DS_PROFILE_3GPP_OVERRIDE_HOME_PDP_IP = 0x0,          /**< PDP type is IP. */
  DS_PROFILE_3GPP_OVERRIDE_HOME_PDP_IPV6,              /**< PDP type is IPv6. */
  DS_PROFILE_3GPP_OVERRIDE_HOME_PDP_IPV4V6,            /**< PDP type is IPv4v6. */
  /** @cond
  */
  DS_PROFILE_3GPP_OVERRIDE_HOME_PDP_MAX = 0xff
    /**< Default value if nothing is configured. */
  /** @endcond */
}ds_profile_3gpp_override_home_pdp_type_enum_type;

/**LTE and UMTS Roaming PDP_TYPE*/

typedef enum
{
  DS_PROFILE_3GPP_ROAMING_PDP_IP = 0x0,          /**< PDP type is IP. */
  DS_PROFILE_3GPP_ROAMING_PDP_IPV6,              /**< PDP type is IPv6. */
  DS_PROFILE_3GPP_ROAMING_PDP_IPV4V6,            /**< PDP type is IPv4v6. */
  /** @cond
  */
  DS_PROFILE_3GPP_ROAMING_PDP_MAX = 0xff         
    /**< Forces the maximum to 0xff so the enumerator is defined as a byte. 
      Internal use only. */
  /** @endcond */
}ds_profile_3gpp_roaming_pdp_type_enum_type;

/** Technology-specific identifiers for profile parameters.
*/
typedef enum
{
  DS_PROFILE_3GPP_PROFILE_PARAM_INVALID                    = 0, 
    /**< Invalid parameter. */
  DS_PROFILE_3GPP_PROFILE_PARAM_MIN                        = 0x10,
    /**< Minimum parameter for the boundary check. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PROFILE_NAME               = 0x10,
    /**< Profile name. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE       = 0x11,
    /**< PDP context type. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP         = 0x12,
    /**< PDP header compression support. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP         = 0x13,
    /**< PDP data compression support. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN            = 0x14,
    /**< PDP context access point name. */
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY        = 0x15,
    /**< Primary IPv4 DNS address. */
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY      = 0x16,
    /**< Secondary IPv4 DNS address. */

  /** @cond
  */
  DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS_EXTENDED      = 0x17, 
    /**< Internal use only. */
  DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS_EXTENDED      = 0x18,
    /**< Internal use only. */
  /** @endcond */

  DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS               = 0x19,
    /**< GPRS QoS request. */
  DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS               = 0x1A,
    /**< GPRS minimum QoS request. */
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME              = 0x1B,
    /**< PDP authentication username. */
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD              = 0x1C,
    /**< PDP authentication password. */
  DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE                  = 0x1D,
    /**< PDP authentication type. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4    = 0x1E,
    /**< IPv4 address in PDP context. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG             = 0x1F,
    /**< PCSCF request flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG = 0x20,
    /**< Mobile Terminated Access Control flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG        = 0x21,
    /**< PCSCF request using the DHCP flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG                 = 0x22,
    /**< IP Multimedia Core Network flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1             = 0x23,
    /**< TFT filter ID 1. */
  DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2             = 0x24,
    /**< TFT filter ID 2. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER         = 0x25,
    /**< PDP context number. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG = 0x26,
    /**< Secondary PDP context flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID     = 0x27,
    /**< Primary PDP context ID. */
  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6    = 0x28,
    /**< IPv6 address in PDP context. */
  DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS               = 0x29,
    /**< Request UMTS QoS. */
  DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS               = 0x2A,
    /**< Request for minimum UMTS QoS. */
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY        = 0x2B,
    /**< Primary IPv6 DNS address. */
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY      = 0x2C,
    /**< Secondary IPv6 DNS address. */
  DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC            = 0x2D,
    /**< IPv4 address allocation. */
  DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS                = 0x2E,
    /**< Request LTE QoS. */
  DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG           = 0x2F,
    /**< APN Disable flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL       = 0x30,
    /**< APN inactivity timer value. */
  DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS                  = 0x31,
    /**< APN class parameter. */
  DS_PROFILE_3GPP_PROFILE_PARAM_LINGER_PARAMS              = 0x32,
    /**< Iface linger parameter */
  DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ         = 0x33,
    /**< Source statistics descriptor for requested UMTS QoS. */
  DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN          = 0x34,
    /**< Source Statistics Descriptor for minimum UMTS QoS. */
  DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER                = 0x35,
    /** < APN bearer type specifier */
  DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED = 0x36,
    /**< Flag used to identify is emergency calls can be made using 
      this profile. */
  DS_PROFILE_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO     = 0x37,  
      /** OPERATOR RESERVED PCO */
  DS_PROFILE_3GPP_PROFILE_PARAM_MCC                       = 0x38,
     /** MCC */
  DS_PROFILE_3GPP_PROFILE_PARAM_MNC                       = 0x39,
     /** MNC */
  DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_PER_BLOCK         = 0x3A,
     /** Max PDN Connections per Time Block */
  DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_TIMER              = 0x3B,
     /** Max PDN Connection Time */
  DS_PROFILE_3GPP_PROFILE_PDN_REQ_WAIT_TIMER              = 0x3C,
     /** PDN Request Wait Time */
  DS_PROFILE_3GPP_PROFILE_USER_APP_DATA                   = 0x3D,
       /** Reserved for OEM Usage */
  DS_PROFILE_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED        = 0x3E,
  DS_PROFILE_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME      = 0x3F,
  DS_PROFILE_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG         = 0x40,
   /**< DNS request using the DHCP flag. */
  DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE      = 0x41,
  /**< LTE Roaming PDP type*/
  DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE     = 0x42,
  /**< UMTS Roaming PDP type*/
  DS_PROFILE_3GPP_PROFILE_PARAM_IWLAN_TO_LTE_ROAMING_HO_FLAG = 0x43,
  /**< Roaming handover from IWLAN to LTE allowed flag*/
  DS_PROFILE_3GPP_PROFILE_PARAM_LTE_TO_IWLAN_ROAMING_HO_FLAG = 0x44,
  /**< Roaming handover from LTE to IWLAN allowed flag*/
  DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS            = 0x45,

  DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE    = 0x46,
  /**< Override IP type of the default home pdp type*/

  DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID                   = 0x47,

     /** SUBS ID */
  DS_PROFILE_3GPP_PROFILE_PARAM_MAX = DS_PROFILE_3GPP_PROFILE_PARAM_SUBS_ID
    /**< Maximum parameter for the boundary check. */
}ds_profile_3gpp_param_enum_type;

/** @} */ /* end_addtogroup datatypes */

#endif /* DS_PROFILE_3GPP_H */




