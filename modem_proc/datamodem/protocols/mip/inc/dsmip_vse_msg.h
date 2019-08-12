#ifndef _DSMIP_VSE_MSG_H
#define _DSMIP_VSE_MSG_H

/*===========================================================================

 D A T A   S E R V I C E S   M O B I L E   I P   M O D U L E   H E A D E R
                   
DESCRIPTION

  The Data Services Mobile IP VSE Header File.

  Contains definitions for vendor specific extensions (as per RFC 3115)
  in Mobile IPv4 registration messages.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_vse_msg.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/06   jd      Created module.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Vendor Specific extension constants
---------------------------------------------------------------------------*/

#define MIP_EXTTYPE_CVSE              (38)        /* Critical Vendor spec. */
#define MIP_EXTTYPE_NVSE              (134)       /* Normal Vendor spec.   */

#define MIP_DNS_SENDER_HAAA            (1)

/*---------------------------------------------------------------------------
  Vendor Specific Extension constants
  - Orgnization identifiers
    (registry at http://www.iana.org/assignments/enterprise-numbers)
---------------------------------------------------------------------------*/
#define MIP_VSE_QUALCOMM_ORG_ID       (1449)
#define MIP_VSE_3GPP2_ORG_ID          (5535)
#define MIP_VSE_VERIZON_ORG_ID        (12951)

#define MIP_3GPP2_NVSETYPE_DNS        (17) /* 3GPP2 DNS server address ext */

/*---------------------------------------------------------------------------
  Vendor Specific extension header formats
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte   type;          /* 38 for CVSE                                     */
  byte   reserved;      /* must set to 0 on tx and ignore on receipt       */
  uint16 length;        /* length of extension excl. type and length       */
  uint32 vendor_id;     /* 0 byte, followed by SMI Network Mgmt            */
                        /* Private Enterprise Code of the vendor           */
  uint16 vse_type;      /* Vendor specific type field                      */
  /*     vse_value; */  /* followed by CVSE specific data                  */
} mip_cvse_type;

typedef PACKED struct PACKED_POST
{
  byte   type;          /* 134 for NVSE                                    */
  byte   length;        /* length of extension excl. type and length       */
  uint16 reserved;      /* must set to 0 on tx and ignore on receipt       */
  uint32 vendor_id;     /* 0 byte, followed by SMI Network Mgmt            */
                        /* Private Enterprise Code of the vendor           */
  uint16 vse_type;      /* Vendor specific type field                      */
  /*     vse_value; */  /* followed by CVSE specific data                  */
} mip_nvse_type;

/*---------------------------------------------------------------------------
  3GPP2 Vendor Specific extension definitions
  17 = DNS server IP address
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  mip_nvse_type  nvse_hdr;  /* NVSE header                                 */
  byte           sender;    /* 1 = HAAA, 2 = VAAA, 3 = HA                  */
  byte           pri_subtype; /* 1 = primary DNS                           */
  byte           pri_length;  /* set to 6 as per standard (will be ignored)*/
  uint32         pri_dns_v4addr;        /* Primary DNS server IPv4 address */
  byte           sec_subtype; /* 2 = secondary DN                          */
  byte           sec_length;  /* set to 6 as per standard (will be ignored)*/
  uint32         sec_dns_v4addr;        /* Primary DNS server IPv4 address */
  byte    unused;                    /* unused byte to enforce alignment   */
} mip_3gpp2_dns_nvse_type;

#endif /* FEATURE_DS_MOBILE_IP */
#endif  /* _DSMIP_VSE_MSG_H */
