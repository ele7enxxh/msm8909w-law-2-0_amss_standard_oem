#ifndef DHCP_CLIENT_COMMON_H
#define DHCP_CLIENT_COMMON_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ C O M M O N. H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
  @file dhcp_client_common.h
  @brief Contains the common enums and typedefs used in the DHCP client public 
  and private interfaces.
 */

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dhcp group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2007-2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp_client_common.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/14    ash    Added Prefix Delegation support.
11/28/11    cp     Moved common DUID, HW type defs to dhcp6_common_defs.h.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
01/25/10    ss     Added DHCP_CLIENT_IP_DECLINED status.
08/07/09    kk     Added support for client identifier option (opt type=61).
12/14/08    pp     Common Modem Interface: Public/Private API split.
08/25/08    cp     Updated changes in draft-ietf-mip6-hiopt-17.txt from
                   draft-ietf-mip6-hiopt-09.txt
05/07/07     es    Initial creation.
===========================================================================*/
/* group: dhcp */

#include "dhcp6_common_defs.h"

/** @addtogroup constants 
  @{
*/

/**
  Maximum DUID length is 128 + 2 (typecode) characters. Refer to RFC 3315
  @xhyperref{S3,[S3]}, Section 9.1.
*/
#define DHCP6_CLIENT_MAX_DUID_LENGTH DHCP6_MAX_DUID_LENGTH

/**
  Maximum size of the MIPv6 home network information value.
*/
#define DHCP6_CLIENT_MIP6_HNINFO_DATA_MAX_SIZE (72)

/**
  MIPv6 Home Network Identification option values.
*/
#define DHCP6_CLIENT_HNINFO_OPTION_IDTYPE_LEN      (1) 

/**
  MIPv6 Home Network Information suboption code field size.
*/
#define DHCP6_CLIENT_HNINFO_SUBOPT_CODE_SIZE       (2)

/**
  MIPv6 Home Network Information suboption len field size.
*/
#define DHCP6_CLIENT_HNINFO_SUBOPT_LEN_SIZE        (2)

/**
  MIPv6 home network ID code.
*/
#define DHCP6_CLIENT_SUBOPT_CODE_HOMENETWORKID     (1)
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief MIPv6 Home Network Information suboption structure. 
  Refer to @xhyperref{S32,[S32]}. 
*/
struct dhcp6_mip6_hninfo_sub_option
{
  uint16 sub_opt_code;
    /**< Suboption code. */
  uint16 sub_opt_len;
    /**< Suboption length. */
  uint8 hninfo[DHCP6_CLIENT_MIP6_HNINFO_DATA_MAX_SIZE];
    /**< Home network information value. */
};


/**
  @brief  MIPv6 Home Network Information option structure.
  Refer to @xhyperref{S32,[S32]}.
 */
typedef struct
{
  uint8 id_type;
    /**< Type of home network information. */

  /** Suboption list. */
  struct dhcp6_mip6_hninfo_sub_option hninfo_sub_opt_list;
  uint32 hninfo_len;
    /**< Length of the home network information options. */
} dhcp6_mip6_hnid_option_vals;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_dhcp
  @{
*/
/**< Dummy HWType. Not part of RFC. */
#define DHCP_CLIENT_HWTYPE_LOOPBACK DHCP_HWTYPE_LOOPBACK

/**< Ethernet (10 Mb). */
#define DHCP_CLIENT_HWTYPE_ETHERNET DHCP_HWTYPE_ETHERNET

/**< Experimental Ethernet (3 Mb). */
#define DHCP_CLIENT_HWTYPE_EXP_ETHERNET DHCP_HWTYPE_EXP_ETHERNET

/**< Amateur Radio AX.25. */
#define DHCP_CLIENT_HWTYPE_AX_25 DHCP_HWTYPE_AX_25

/**< Proteon ProNET Token Ring. */
#define DHCP_CLIENT_HWTYPE_TOKEN_RING DHCP_HWTYPE_TOKEN_RING

/**< Chaos. */
#define DHCP_CLIENT_HWTYPE_CHAOS DHCP_HWTYPE_CHAOS

/**< IEEE 802 Networks. */
#define DHCP_CLIENT_HWTYPE_IEEE_802 DHCP_HWTYPE_IEEE_802

/**< ARCNET. Refer to RFC 1201 @xhyperref{S30,[S30]}. */
#define DHCP_CLIENT_HWTYPE_ARCNET DHCP_HWTYPE_ARCNET

/**< Hyperchannel. */
#define DHCP_CLIENT_HWTYPE_HYPERCHANNEL DHCP_HWTYPE_HYPERCHANNEL

/**< Lanstar. */
#define DHCP_CLIENT_HWTYPE_LANSTAR DHCP_HWTYPE_LANSTAR

/**< Autonet Short Address. */
#define DHCP_CLIENT_HWTYPE_AUTONET DHCP_HWTYPE_AUTONET

/**< LocalTalk. */
#define DHCP_CLIENT_HWTYPE_LOCALTALK DHCP_HWTYPE_LOCALTALK

/**< LocalNet (IBM PCNet or SYTEK LocalNET). */
#define DHCP_CLIENT_HWTYPE_LOCALNET DHCP_HWTYPE_LOCALNET

/**< Ultra link. */
#define DHCP_CLIENT_HWTYPE_ULTRA_LINK DHCP_HWTYPE_ULTRA_LINK

/**< SMDS. */
#define DHCP_CLIENT_HWTYPE_SMDS DHCP_HWTYPE_SMDS

/**< Frame Relay. */
#define DHCP_CLIENT_HWTYPE_FRAME_RELAY DHCP_HWTYPE_FRAME_RELAY

/**< Asynchronous Transmission Mode (ATM).
     Refer to RFC 2225 @xhyperref{S21,[S21]}. */
#define DHCP_CLIENT_HWTYPE_ATM_1 DHCP_HWTYPE_ATM_1

/**< HDLC. */
#define DHCP_CLIENT_HWTYPE_HDLC DHCP_HWTYPE_HDLC

/**< Fibre Channel. Refer to RFC 4338 @xhyperref{S29,[S29]}. */
#define DHCP_CLIENT_HWTYPE_FIBER_CHANNEL DHCP_HWTYPE_FIBER_CHANNEL

/**< Asynchronous Transmission Mode (ATM). */
#define DHCP_CLIENT_HWTYPE_ATM_2 DHCP_HWTYPE_ATM_2

/**< Serial Line. */
#define DHCP_CLIENT_HWTYPE_SERIAL_LINE DHCP_HWTYPE_SERIAL_LINE

/**< Asynchronous Transmission Mode (ATM). */
#define DHCP_CLIENT_HWTYPE_ATM_3 DHCP_HWTYPE_ATM_3

/**< MIL-STD-188-220. Refer to @xhyperref{S22,[S22]}. */
#define DHCP_CLIENT_HWTYPE_MIL_STD_188_220 DHCP_HWTYPE_MIL_STD_188_220

/**< Metricom. */
#define DHCP_CLIENT_HWTYPE_METRICOM DHCP_HWTYPE_METRICOM

/**< IEEE 1394.1995. Refer to @xhyperref{S25,[S25]}.  */
#define DHCP_CLIENT_HWTYPE_IEEE1394 DHCP_HWTYPE_IEEE1394

/**< MAPOS. Refer to RFC 2176 @xhyperref{S26,[S26]}. */
#define DHCP_CLIENT_HWTYPE_MAPOS DHCP_HWTYPE_MAPOS

/**< Twinaxial. */
#define DHCP_CLIENT_HWTYPE_TWINAXIAL DHCP_HWTYPE_TWINAXIAL

/**< EUI-64. Refer to @xhyperref{S23,[S23]}.  */
#define DHCP_CLIENT_HWTYPE_EUI_64 DHCP_HWTYPE_EUI_64

/**< HIPARP. */
#define DHCP_CLIENT_HWTYPE_HIPARP DHCP_HWTYPE_HIPARP

/**< IP and ARP over ISO/IEC 7816-3. Refer to @xhyperref{S24,[S24]}. */
#define DHCP_CLIENT_HWTYPE_ISO7816_3 DHCP_HWTYPE_ISO7816_3

/**< ARPSec.  */
#define DHCP_CLIENT_HWTYPE_ARPSEC DHCP_HWTYPE_ARPSEC

/**< IPsec tunnel. Refer to RFC 3456 @xhyperref{S31,[S31]}. */
#define DHCP_CLIENT_HWTYPE_IPSEC_TUNNEL DHCP_HWTYPE_IPSEC_TUNNEL

/**< InfiniBand&tm;. Refer to RFC 4391 @xhyperref{S28,[S28]}. */
#define DHCP_CLIENT_HWTYPE_INFINIBAND DHCP_HWTYPE_INFINIBAND

/**< TIA-102 Project 25 Common Air Interface (CAI). 
     Refer to @xhyperref{S27,[S27]}.  */
#define DHCP_CLIENT_HWTYPE_CAI DHCP_HWTYPE_CAI

/** Hardware type values. Refer to @xhyperref{S20,[S20]}. */
#define dhcp_client_hw_type_enum dhcp_hw_type_enum

/** @} */ /* end_addtogroup macros_dhcp */

/** @addtogroup constants
  @{
*/
/**
  Minimum prescribed length for the client_id option.
*/
#define DHCP_CLIENT_CLIENT_ID_MIN_LEN           (2)
/**
  Maximum allowed length for the client_id option.
*/
#define DHCP_CLIENT_CLIENT_ID_MAX_LEN           (254)
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  Allows a client to explicitly specify a DHCP client identifier.
*/
typedef enum 
{
  DHCP_CLIENT_ID_OPTTYPE_NONE        = 0,
    /**< Client identifier option is not used. (Default) */
  DHCP_CLIENT_ID_OPTTYPE_HWADDR      = 1,
    /**< Hardware address of the underlying interface, if set, is used as the 
	     DHCP client identifier. */
  DHCP_CLIENT_ID_OPTTYPE_CUSTOM      = 2,
    /**< Client provides the client identifier and its length in octets 
         explicitly. */
  DHCP_CLIENT_ID_OPTTYPE_FORCE_32BIT = 0x7FFFFFFF
    /**< Not a valid client option type. Used to force this enumerator to have
	     a maximum value. */
} dhcp_client_id_option_type_enum;

/**
  Status callback enumerators for the DHCP client.
*/
typedef enum 
{
  DHCP_CLIENT_IP_ASSIGNED    = 0,   /**< IP address has been obtained. */
  DHCP_CLIENT_IP_RELEASED    = 1,   /**< IP address has been released. */
  DHCP_CLIENT_IP_EXPIRED     = 2,   /**< IP address has been lost. */
  DHCP_CLIENT_IP_FAIL        = 3,   /**< Failed to obtain an IP address. */
  DHCP_CLIENT_INIT_FAIL      = 4,   /**< Failed to initialize properly. */
  DHCP_CLIENT_PROVISIONED    = 5,   /**< Received a reply to our INFORM 
                                         and provisioned successfully. */
  DHCP_CLIENT_PROVISION_FAIL = 6,   /**< INFORM sequence failed. */
  DHCP_CLIENT_IP_DECLINED     = 7,   /**< Client declined allocated IP
                                         address. */
  DHCP_CLIENT_PREFIX_ASSIGNED = 8,   /**< Delegated Prefix has been obtained. */
  DHCP_CLIENT_PREFIX_RELEASED = 9,   /**< Delegated Prefix has been released. */
  DHCP_CLIENT_PREFIX_EXPIRED  = 10,   /**< Delegated Prefix has been lost. */
  DHCP_CLIENT_PREFIX_FAIL     = 11,    /**< Failed to obtain an 
                                           Delegated Prefix. */
  DHCP_CLIENT_ABORTED         = 12    /**< Failed to obtain an 
                                           Delegated Prefix. */
} dhcp_client_status_enum;

/**
  @brief Callback to indicate the client status.
*/
typedef void (*dhcp_client_status_cb_type)
(
  void * userdata,               /**< User specified data. */              
  dhcp_client_status_enum status /**< Status of the client. */
);
/** @} */ /* end_addtogroup datatypes */

#endif /* DHCP_CLIENT_COMMON_H */

