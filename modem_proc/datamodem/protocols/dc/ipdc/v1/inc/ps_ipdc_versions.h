#ifndef PS_IPDC_VERSIONS_H
#define PS_IPDC_VERSIONS_H

/*==========================================================================

                     P S _ I P D C _ V E R S I O N S . H

GENERAL DESCRIPTION
  This file provides declares IPDC version specific data types and function
  declarations.

Copyright (c) 2015 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/dc/ipdc/v1/inc/ps_ipdc_versions.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/15   ss      Initial version

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_ipdc_ex.h"
#include "ps_ipdci_ex.h"
#include "ps_ipdc_platform_defs.h"

#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)

#include "ds_Utils_DebugMsg.h"

#endif

/*==========================================================================

                               CONSTANTS

===========================================================================*/
/*--------------------------------------------------------------------------
  Version 1
  Definitions for IP Data Compression (IPDC) packet types that are 
  set in the Compressed Packet's IPDC Header (upper 3 bits)

  PS_IPDC_ENTIRE_PKT_COMPRESSED_PKT_TYPE    011
  PS_IPDC_HEADER_ONLY_COMPRESSED_PKT_TYPE   010
  PS_IPDC_HEADER_NO_MATCHES_PKT_TYPE        001
  PS_IPDC_NOTHING_COMPRESSED_PKT_TYPE       000

  // This one is not yet supported
  PS_IPDC_COMP_DONT_COPY_PKT_TYPE           100
  
  PS_IPDC_RESET_PKT_TYPE                    101
---------------------------------------------------------------------------*/
#define PS_IPDC_ENTIRE_PKT_COMPRESSED_PKT_TYPE__v1     0x60
#define PS_IPDC_HEADER_ONLY_COMPRESSED_PKT_TYPE__v1    0x40
#define PS_IPDC_HEADER_NO_MATCHES_PKT_TYPE__v1         0x20
#define PS_IPDC_NOTHING_COMPRESSED_PKT_TYPE__v1        0x00
#define PS_IPDC_RESET_PKT_TYPE__v1                     0xa0
#define PS_IPDC_USE_PREV_METADATA_PKT_TYPE__v1         0x80

/*==========================================================================

                            FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IPDC_VERSIONS_GET_HDR_PKT_TYPE_MASK

DESCRIPTION
  Function to fetch version specific packet type mask in IPDC header.
  This mask would be used to fetch packet type from first byte in IPDC 
  header.

DEPENDENCIES
  None

RETURN VALUE
  1 byte mask value for IPDC header packet type

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdc_versions_get_hdr_pkt_type_mask
(
  ipdc_lib_version_e_type version
);

/*===========================================================================
FUNCTION IPDC_VERSIONS_GET_HDR_CKSUM_MASK

DESCRIPTION
  Function to fetch version specific header checksum field mask.
  This mask would be used to fetch checksum value from first byte in IPDC 
  header.

DEPENDENCIES
  None

RETURN VALUE
  1 byte mask value for IPDC header checksum value.

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdc_versions_get_hdr_cksum_mask
(
  ipdc_lib_version_e_type version
);

/*===========================================================================
FUNCTION IPDC_VERSIONS_GET_MASKED_CKSUM_VALUE

DESCRIPTION
  This function returns the masked checksum value as per the bits in which
  checksum is to be stored for the specified version.
  For e.g.
  Original Checksum = 0xFFFF
  Library Version   = 2
  Checksum Mask     = 0x1e
  Masked Value      = 0x1e

DEPENDENCIES
  None

RETURN VALUE
  1 byte checksum masked value

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdc_versions_get_masked_cksum_value
(
  uint16                  original_cksum,
  ipdc_lib_version_e_type version
);

/*===========================================================================
FUNCTION IPDC_VERSIONS_GET_RESERVED_BIT_MASK

DESCRIPTION
  Function to fetch version specific reserved bit mask.

DEPENDENCIES
  None

RETURN VALUE
  1 byte mask value for IPDC header reserved bit field.

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdc_versions_get_reserved_bit_mask
(
  ipdc_lib_version_e_type version
);

/*===========================================================================
FUNCTION IPDC_VERSIONS_GET_RESERVED_BIT_FIELD_MASK

DESCRIPTION
  Function to fetch version specific IPDC header packet type value

DEPENDENCIES
  None

RETURN VALUE
  1 byte IPDC header packet type value

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdc_versions_get_hdr_pkt_type_value
(
  ipdc_lib_version_e_type version,
  ipdci_ipdc_hdr_pkt_type pkt_type
);

#endif /* PS_IPDC_VERSIONS_H */
