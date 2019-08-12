#ifndef DHCP6_COMMON__DEFS_H
#define DHCP6_COMMON__DEFS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P 6 _ C O M M O N _ D E F S . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
  @file dhcp6_common_defs.h
  @brief Contains the common enums and typedefs used in the DHCP6 client/server
  public and private interfaces.
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
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp6_common_defs.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/28/11    cp     Initial Revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* group: dhcp */

#include "comdef.h"

/** @addtogroup constants
  @{
*/

/**
  Maximum DUID length is 128 + 2 (typecode) characters. Refer to RFC 3315
  @xhyperref{S3,[S3]}, Section 9.1.
*/
#define DHCP6_MAX_DUID_LENGTH         (130)


/** 
  Hardware type values. Refer to @xhyperref{S20,[S20]}. 
*/
typedef enum 
{
  DHCP_HWTYPE_LOOPBACK = 0,         /**< Not part of RFC. */
  DHCP_HWTYPE_ETHERNET = 1,         /**< Ethernet (10 Mb). */
  DHCP_HWTYPE_EXP_ETHERNET = 2,     /**< Experimental Ethernet (3 Mb). */
  DHCP_HWTYPE_AX_25 = 3,            /**< Amateur Radio AX.25. */
  DHCP_HWTYPE_TOKEN_RING = 4,       /**< Proteon ProNET Token Ring. */
  DHCP_HWTYPE_CHAOS = 5,            /**< Chaos. */
  DHCP_HWTYPE_IEEE_802 = 6,         /**< IEEE 802 Networks. */
  DHCP_HWTYPE_ARCNET = 7,           /**< ARCNET. Refer to RFC 1201 @xhyperref{S30,[S30]}. */
  DHCP_HWTYPE_HYPERCHANNEL = 8,     /**< Hyperchannel. */
  DHCP_HWTYPE_LANSTAR = 9,          /**< Lanstar. */
  DHCP_HWTYPE_AUTONET = 10,         /**< Autonet Short Address. */
  DHCP_HWTYPE_LOCALTALK = 11,       /**< LocalTalk. */
  DHCP_HWTYPE_LOCALNET = 12,        /**< LocalNet (IBM PCNet or SYTEK LocalNET). */
  DHCP_HWTYPE_ULTRA_LINK = 13,      /**< Ultra link. */
  DHCP_HWTYPE_SMDS = 14,            /**< SMDS. */
  DHCP_HWTYPE_FRAME_RELAY = 15,     /**< Frame Relay. */
  DHCP_HWTYPE_ATM_1 = 16,           /**< Asynchronous Transmission Mode (ATM). Refer to RFC 2225 @xhyperref{S21,[S21]}. */
  DHCP_HWTYPE_HDLC = 17,            /**< HDLC. */
  DHCP_HWTYPE_FIBER_CHANNEL = 18,   /**< Fibre Channel. Refer to RFC 4338 @xhyperref{S29,[S29]}. */
  DHCP_HWTYPE_ATM_2 = 19,           /**< Asynchronous Transmission Mode (ATM). */
  DHCP_HWTYPE_SERIAL_LINE = 20,     /**< Serial Line. */
  DHCP_HWTYPE_ATM_3 = 21,           /**< Asynchronous Transmission Mode (ATM). */
  DHCP_HWTYPE_MIL_STD_188_220 = 22, /**< MIL-STD-188-220. Refer to @xhyperref{S22,[S22]}. */
  DHCP_HWTYPE_METRICOM = 23,        /**< Metricom. */
  DHCP_HWTYPE_IEEE1394 = 24,        /**< IEEE 1394.1995. Refer to @xhyperref{S25,[S25]}.  */
  DHCP_HWTYPE_MAPOS = 25,           /**< MAPOS. Refer to RFC 2176 @xhyperref{S26,[S26]}. */
  DHCP_HWTYPE_TWINAXIAL = 26,       /**< Twinaxial. */
  DHCP_HWTYPE_EUI_64 = 27,          /**< EUI-64. Refer to @xhyperref{S23,[S23]}.  */
  DHCP_HWTYPE_HIPARP = 28,          /**< HIPARP. */
  DHCP_HWTYPE_ISO7816_3 = 29,       /**< IP and ARP over ISO/IEC 7816-3. Refer to @xhyperref{S24,[S24]}. */
  DHCP_HWTYPE_ARPSEC = 30,          /**< ARPSec.  */
  DHCP_HWTYPE_IPSEC_TUNNEL = 31,    /**< IPsec tunnel. Refer to RFC 3456 @xhyperref{S31,[S31]}. */
  DHCP_HWTYPE_INFINIBAND = 32,      /**< InfiniBand&tm;. Refer to RFC 4391 @xhyperref{S28,[S28]}. */
  DHCP_HWTYPE_CAI = 33              /**< TIA-102 Project 25 Common Air Interface (CAI). Refer to @xhyperref{S27,[S27]}.  */
} dhcp_hw_type_enum;

/** @} */ /* end_addtogroup constants */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*==========================================================================
FUNCTION DHCP6_DUID_LLT_INIT()
===========================================================================*/
/** @ingroup dhcp6_duid_llt_init
  Initializes a buffer with a DUID LLT.

  @datatypes
  #dhcp_client_hw_type_enum

  @vertspace
  @param[out] duid_buf   Pointer to a buffer with sufficient size to store
                         the DUID of the client interface.
  @param[in] duid_len    Size of the buffer. If the size is insufficient, 
                         the function returns -1.
  @param[in] hw_type     Hardware type.
  @param[in] time        32-bit time value.
  @param[in] ll_addr     Pointer to a variable length link-layer address.
  @param[in] ll_addr_len Length of ll_addr passed in.

  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/
int
dhcp6_duid_llt_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  dhcp_hw_type_enum    hw_type,
  uint32                      time,
  void                      * ll_addr,
  uint32                      ll_addr_len
);

/*==========================================================================
FUNCTION DHCP6_DUID_EN_INIT()
===========================================================================*/
/** @ingroup dhcp6_duid_en_init
  Initializes a buffer with a DUID EN. 

  @param[out] duid_buf      Pointer to a buffer with sufficient size to store
                            the DUID of the client interface.
  @param[in] duid_len       Size of the buffer. If the size is insufficient, 
                            the function returns -1.
  @param[in] enterprise_num 32-bit vendor enterprise number.
  @param[in] identifier     Pointer to a variable length link-layer address.
  @param[in] identifier_len Length of the identifier passed in.

  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/
int
dhcp6_duid_en_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  uint32                      enterprise_num,
  void                      * identifier,
  uint32                      identifier_len
);

/*==========================================================================
FUNCTION DHCP6_DUID_LL_INIT()
===========================================================================*/
/** @ingroup dhcp6_duid_ll_init
  Initializes a buffer with a DUID LL. 

  @param[out] duid_buf   Pointer to a buffer with sufficient size to store
                         the DUID of the client interface.
  @param[in] duid_len    Size of the buffer. If the size is insufficient,
                         the function returns -1.
  @param[in] hw_type     Hardware type.
  @param[in] ll_addr     Variable length link-layer address.
  @param[in] ll_addr_len Length of ll_addr passed in.
  
  @return
  Valid size of the DUID in bytes.\n
  -1 -- Initialization failed.

  @dependencies
  None.
  @newpage
*/
int
dhcp6_duid_ll_init
(
  void                      * duid_buf,
  uint32                      duid_buf_len,
  dhcp_hw_type_enum    hw_type,
  void                      * ll_addr,
  uint32                      ll_addr_len
);

#endif /* DHCP6_COMMON__DEFS_H */
