#ifndef _DSMIP_MSG_H
#define _DSMIP_MSG_H

/*===========================================================================

 D A T A   S E R V I C E S   M O B I L E   I P   M O D U L E   H E A D E R
                   
DESCRIPTION

 The Data Services Mobile IP Header File.  Contains the mobile I/P task
 entry point and exports any necessary information.

Copyright (c) 2000-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_msg.h_v   1.2   10 Oct 2002 15:55:14   jayanthm  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_msg.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/09   ss      CMI SU level modifications.
12/14/06   jd      moved vendor-specific extension def into dsmip_vse_msg.h
07/09/03   jd      Added CVSE extension type definition
10/09/02   jay     Removed redundant #defines
08/16/02   jay     Added DMU key data extension to rrq message type.
07/17/01   jd      Unified router adv. and RRQ FA challenge extension types
09/25/00   jd      Created module.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_v.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Mobile IP message definitions
---------------------------------------------------------------------------*/

#define ICMP_MSGTYPE_ROUTER_SOLICITATION   (10)
#define ICMP_MSGTYPE_ROUTER_ADVERTISEMENT  (9)

#define MIP_EXTTYPE_AUTH_MN_HA             (32)
#define MIP_EXTTYPE_MN_NAI                 (131)
#define MIP_EXTTYPE_MN_FA_CHAL             (132)

/*---------------------------------------------------------------------------
  Router Solicitation message (RFC 1256, S 3)

  +--------+               +---------------+
  |        |               | Foreign Agent |
  | Mobile |-->-- ICMP ->--|     or        |
  |        |               |  Home Agent   |
  +--------+               +---------------+

  Agent Solicitation message is identical to the router solicitation message
  at the ICMP layer, but MUST have IP TTL set to 1.  The IP destination 
  address is set to "all systems on this link" multicast address 224.0.0.1
  as per RFC 2002 (2.1)

  Note that the ICMP header (type, code, checksum) is built by the sockets
  layer (dssicmp.c) for outgoing messages, so these fields are not included 
  in this structure (since we only send router solicitations)
---------------------------------------------------------------------------*/
typedef struct {
  byte   type;          /* defined as 10 for ICMP Router Solicitation      */
  byte   code;          /* defined as 0  for ICMP Router Solicitation      */
  uint16 checksum;      /* checksum of structure (Checksum zeroed)         */
} icmp_header_type;

typedef struct {
  uint32  reserved;     /* reserved, MUST be set to 0                      */
} mip_router_sol_msg;


/*---------------------------------------------------------------------------
  Router Advertisement message (RFC 1256, S 3)

  +---------------+               +--------+
  | Foreign Agent |               |        |
  |      or       |-->-- ICMP ->--| Mobile |
  | Home Agent    |               |        |
  +---------------+               |--------+

  ICMP header is built by dssicmp
---------------------------------------------------------------------------*/
typedef struct {
  uint32 router_addr;   /* Sending router IP address on this interface     */
  uint32 preference;    /* Preference level as default router (2's compl.) */
} r_info;

#define R_INFO_SIZE_32 (2) /* size of r_info structure in dwords (32 bits) */

typedef struct {
  icmp_header_type  hdr;
  byte              num_addrs;     /* # of addr's starting w/ router_info */ 
  byte              addr_size;     /* router info entry size in dwords    */
  uint16            lifetime;      /* max. reg. lifetime supported by MA  */
  r_info            router_info;   /* 1st of Num_Addrs entries in list    */
} mip_router_adv_msg;

#define MIP_ROUTER_ADV_ROUTER_INFO(msg,x)  ((&(msg->router_info))[x])

/*---------------------------------------------------------------------------
  Router Advertisement Extensions

  These extension fields immediately follow the mip_router_adv_msg body.  
  The presence of router advertisement extensions is indicated by the IP
  datagram length, which is increased to include the extension lengths 
  (i.e. is greater than sizeof (mip_router_adv_msg)). 

  ICMP router advertisement extensions: 

  type  description                                     reference
  ----  -----------                                     ---------
    0   One-byte padding (no length nor data fields)    RFC 2002, 2.1.3
   16   Mobility Agent Advertisement                    RFC 2002, 2.1.1
   19   Prefix-Lengths                                  RFC 2002, 2.1.2
   24   Foreign Agent challenge                         RFC 3012, 2

---------------------------------------------------------------------------*/
typedef struct {
  byte   type;          /*  set to 0                                       */
} mip_router_adv_ext_pad;

typedef struct {
  byte   type;          /* Extension type identifier                       */
  byte   length;        /* Length of data field (excl. Type and Length)    */
  byte   prefix_len;    /* array of Prefix lengths (ICMP N_Addrs entries)  */
} mip_router_adv_ext_prefix_len; 

#define MIP_AGENT_ADV_EXT_PREFIX(ext,x)   ((&(ext->prefix_len))[x])

typedef struct {
  byte   type;          /* Extension type identifier                       */
  byte   length;        /* Length of data field (excl. Type and Length)    */
  uint16 seq_num;       /* Sequence number - incremented each time sent    */
  uint16 reg_lifetime;  /* Max registration lifetime granted by this agent */
  byte   flags;         /* bit name  meaning                               */
                        /* --- ----  -------                               */
                        /*  0   R    Reg. req'd even with co-located COA   */
                        /*  1   B    Busy - FA will not accept new regs.   */
                        /*  2   H    Home agent - will serve mobiles as HA */
                        /*  3   F    Foreign agent - will serve as FA      */
                        /*  4   M    supports UDP 'Minimal Encapsulation'? */
                        /*  5   G    supports UDP 'GRE Encapsulation'?     */
                        /*  6   r    reserved - set to 0                   */
                        /*  7   T    FA supports reverse tunnelling        */
  byte   reserved;      /* Reserved, set to 0                              */
  uint32 COA_array;     /* followed by array of Care of Addresses          */
} mip_router_adv_ext_ma_adv;

#define MIP_AGENT_ADV_EXT_COA(ext,x)   ((&(ext->COA_array))[x])


/*---------------------------------------------------------------------------
  Registration Request message (defined: RFC 2002)
  
  +--------+              +---------------+
  |        |              | Foreign Agent |
  | Mobile |-->-- UDP ->--|     or        |
  |        |              |  Home Agent   |
  +--------+              +---------------+

  UDP extension types:

    32              Mobile-Home Authentication
    36, subtype 1   MN-AAA Authorization Extension
   131              Mobile Node NAI extension
   132              Foreign Agent Challenge Response

  NOTE:  (*) The FA challenge extension is variable length, so 
             ext_mn_fa_chal and ext_mn_aaa_auth fields in mip_rrq_msg
             represent maximal space requirements for the msg.  
             ext_mn_aaa_auth address must be calculated using the fa
             challenge length.

  NOTE 2:  (**) SPI must be handled as a byte array since the field is
                not long aligned (ARM cannot do misaligned memory accesses) 
---------------------------------------------------------------------------*/

typedef struct
{
  byte     type;                        /* defined as 32 as per RFC 2002   */
  byte     length;                      /* length of ext. excl. type & len */
  byte     spi[4];                      /* Security parameter index (**)   */
  byte     authenticator[MIP_AUTH_MAX_LEN];   /* Authenticator (var. len)  */
} mip_rrq_ext_mn_ha_auth;

typedef struct
{
  byte     type;                        /* defined as 36 as per RFC 3012   */
  byte     subtype;                     /* defined as 1 as per RFC 3012    */
  byte     length[2];                   /* length of ext. after this field */
  byte     spi[4];                      /* Security parameter index (**)   */
  byte     authenticator[MIP_AUTH_MAX_LEN];   /* Authenticator (var. len)  */
} mip_rrq_ext_mn_aaa_auth;

typedef struct
{
  byte     type;                        /* defined as 131 as per RFC 2794  */
  byte     length;                      /* length of ext. excl. type & len */
  byte     nai[MAX_NAI_LEN];            /* Network Access Identifier       */
} mip_rrq_ext_mn_nai;

typedef struct
{
  byte type;                            /* defined as 132 as per RFC 2794  */
  byte length;                          /* length of ext. excl. type & len */
  byte challenge[256];                  /* Security parameter index        */
} mip_ext_mn_fa_chal;

typedef struct 
{
  /*** ext. fields present only to allocate space, fields are not aligned **/
  mip_rrq_ext_mn_nai      ext_mn_nai;      /*  MN-NAI extension            */
  mip_rrq_ext_mn_ha_auth  ext_mn_ha_auth;  /* MN-HA authentication         */
  mip_ext_mn_fa_chal      ext_mn_fa_chal;  /* FA challenge response (*)    */
  mip_rrq_ext_mn_aaa_auth ext_mn_aaa_auth; /*  MN-AAA authentication (*)   */

#ifdef FEATURE_DS_MOBILE_IP_DMU
  dmu_key_data_ext_type   key_data_ext;    /* DMU extension                */
#endif /* FEATURE_DS_MOBILE_IP_DMU */
} mip_extensions;

typedef struct 
{
  byte                    type;            /* defined as 1 as per RFC 2002    */
  byte                    flags;           /* (from MSB to LSB):              */
                                           /* 0: S - Simultaneous bindings    */
                                           /* 1: B - Broadcast datagrams      */
                                           /* 2: D - Decapsulation by MN      */
                                           /* 3: M - Minimal encapsulation    */
                                           /* 4: G - GRE encapsulation        */
                                           /* 5: R - Reserved (van jacobson)  */
                                           /* 6: T - Reverse tunneling        */
                                           /* 7: rsv - Reserved bits; zero    */
  uint16                  lifetime;        /* requested lifetime of reg.      */
  uint32                  home_addr;       /* Mobile home IP address          */
  uint32                  ha_addr;         /* Home agent IP address           */
  uint32                  coa;             /* Requested care of address       */
  uint32                  id1;             /* ID - NTP seconds since epoch    */
  uint32                  id2;             /* ID - NTP fract. s. or random    */

  mip_extensions          extensions;     /* ext. fields to allocate space  */

} mip_rrq_msg;
  

/*---------------------------------------------------------------------------
  Registration Reply message (defined: RFC 2002)
  
  +---------------+              +--------+
  | Foreign Agent |              |        |
  |     or        |-->-- UDP ->--| Mobile |
  |  Home Agent   |              |        |
  +---------------+              |--------+

  UDP extension types:

    32              Mobile-Home Authentication
   131              Mobile Node NAI extension
---------------------------------------------------------------------------*/
typedef struct 
{
  byte    type;          /* defined as 3 as per RFC 2002                   */
  byte    code;          /* Mobility agent response code                   */
  uint16  lifetime;      /* lifetime of registration                       */
  uint32  home_addr;     /* Mobile home IP address                         */
  uint32  ha_addr;       /* Home agent IP address                          */
  uint32  id1;           /* ID - should match corresponding RRQ            */
  uint32  id2;           /* ID - NTP fractional seconds or random          */
                         /* followed by extensions:                        */
                         /*  1. MN-NAI extension                           */
                         /*  2. MN-HA authentication                       */
                         /*  2. MN-FA challenge                            */
} mip_rrp_msg;

#endif /* FEATURE_DS_MOBILE_IP */

#endif  /* _DSMIP_MSG_H */

