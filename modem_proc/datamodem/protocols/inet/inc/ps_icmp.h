#ifndef ICMP_NEW_H
#define ICMP_NEW_H

/*===========================================================================

                   I C M P    H E A D E R    F I L E

DESCRIPTION
  Internet Control Message Protocol (ICMP)
  RFC 792

EXTERNALIZED FUNCTIONS
  icmp_input()
   This function Process an incoming ICMP packet. The only ICMP messages
   that are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each
   of these messages will be echoed back to the ICMP peer in the base
   station. Other ICMP messages will be handled by dssicmp_ps_input() in
   dssicmp.c

  icmp_output()
    This function does the following:
      1. Converts from host to network all the elements of the ICMP header
         and inserts the ICMP header in the ICMP packet.
      2. Creates the pseudo-header and calculates ICMP checksum.
      3. Sends ICMP packet down to IP.

  icmp_get_stats()
    This function will copy ICMP stats (enumerated in icmp_stats_enum)
    into the given array.

  icmp_reset_stats()
    This function will reset those ICMP stats which can be reported
    with icmp_get_stats.

Copyright (c) 1995-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_icmp.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_icmp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14   ssh     IP Translation Engine 464XLAT 
11/22/13   ds      Added function prototype for setting the ICMP tokens 
                   parameters values.
03/13/08   pp      Metainfo optimizations.
06/20/05   kr      Removed feature DS_PSTATS
08/15/05   vp      Changed the signature of icmp_hdr_parse()
06/13/05   vp      Renamed icmpi_ntoh() to icmp_hdr_parse().
03/02/05   vp      New function icmp_hdr_create() in place of icmpi_hton().
                   Enum declaration of ICMP types and codes. Declaration of
                   icmp_rate_limit_on_type(). Declaration of icmp_timestamp
                   structure.
08/30/04   ks/msr  Added link field to icmp_cb.
07/16/04   vp      Declaration of icmpv4_token_bucket related constants.
                   Changed the ICMP message types into enum type. Declaration
                   of function icmp_init_token_bucket().
06/11/04   vp      Removed the inclusion of psglobal.h, internet.h and ps_ip.h.
                   Included files ps_in.h, ps_pkt_info.h. Changes for
                   IP address representation as struct ps_in_addr
03/30/04   vp      Merged changes from June04Dev_05.00.00 PS branch.
03/16/04   mct     Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/10/04   vp      Added info_change_upcall to icmp_cb structure.
02/04/04   vp      Changed the signature of icmp_input function to take
                   acl_ipfilter_info_type structure, dsm item and offset
10/17/03   sv      Added sockfd to ICMP control block.
08/23/03   ss      Pass meta_info to icmp_output()
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/01/03   ss/sv   Restructured the code to support IPv6
03/27/21   js      Updated ICMP header definition and introduced new modules
                   to support arbitrary ICMP message types.
06/12/97   ldg     Added new FEATURE_DS_PSTATS
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

#include "customer.h"       /* Customer Specific Features */


#include "dsm.h"
#include "ps_in.h"
#include "ps_pkt_info.h"
#include "ps_tx_meta_info.h"
#include "ps_ip.h"
#include "ps_icmp_hdr.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*==========================================================================
  Internet Control Message Protocol
===========================================================================*/
/*---------------------------------------------------------------------------
  Token bucket parameters to rate limit ICMP messages.
---------------------------------------------------------------------------*/
#define ICMP_TOKENS_PER_SECOND 10 /* # of milliseconds to generate 1 token */
#define ICMP_MAX_TOKENS 10       /* Max # of tokens in icmpv4_token_bucket */

#define ICMP_TYPE_OFFSET                  (0)
#define ICMP_CODE_OFFSET                  (1)
#define ICMP_CKSUM_OFFSET                 (2)

#define ICMP_FRAG_NEEDED_UNUSED_OFFSET    (4)
#define ICMP_FRAG_NEEDED_UNUSED_SIZE      (2)
#define ICMP_FRAG_NEEDED_MTU_OFFSET       (6)
#define ICMP_FRAG_NEEDED_MTU_SIZE         (2)

#define ICMP_PAR_PROB_PTR_OFFSET          (4)
#define ICMP_PAR_PROB_PTR_SIZE            (1)

#define ICMP_PAR_PROB_PTR_VER_IHL           (0)
#define ICMP_PAR_PROB_PTR_TOS               (1)
#define ICMP_PAR_PROB_PTR_TOTAL_LEN_MIN     (2)
#define ICMP_PAR_PROB_PTR_TOTAL_LEN_MAX     (3)
#define ICMP_PAR_PROB_PTR_ID_MIN            (4)
#define ICMP_PAR_PROB_PTR_ID_MAX            (5)
#define ICMP_PAR_PROB_PTR_FLAGS_FRAG_OFFST  (6)
#define ICMP_PAR_PROB_PTR_FRAG_OFFST        (7)
#define ICMP_PAR_PROB_PTR_TTL               (8)
#define ICMP_PAR_PROB_PTR_PROTOCOL          (9)
#define ICMP_PAR_PROB_PTR_HDR_CKSUM_MIN     (10)
#define ICMP_PAR_PROB_PTR_HDR_CKSUM_MAX     (11)
#define ICMP_PAR_PROB_PTR_SRC_ADDR_MIN      (12)
#define ICMP_PAR_PROB_PTR_SRC_ADDR_MAX      (15)
#define ICMP_PAR_PROB_PTR_DST_ADDR_MIN      (16)
#define ICMP_PAR_PROB_PTR_DST_ADDR_MAX      (19)

/*---------------------------------------------------------------------------
   Message types
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP_ECHO_REPLY      = 0,                     /*              Echo Reply */
  ICMP_DEST_UNREACH    = 3,                     /* Destination Unreachable */
  ICMP_QUENCH          = 4,                     /*           Source Quench */
  ICMP_REDIRECT        = 5,                     /*                Redirect */
  ICMP_ALT_HOST_ADDR   = 6,                     /*           Alt Host Addr */
  ICMP_ECHO_REQ        = 8,                     /*            Echo Request */
  ICMP_RTR_ADV         = 9,                     /* MIP agent advertisement */
  ICMP_RTR_SOL         = 10,                    /*  MIP agent solicitation */
  ICMP_TIME_EXCEED     = 11,                    /*   Time-to-live Exceeded */
  ICMP_PARAM_PROB      = 12,                    /*       Parameter Problem */
  ICMP_TIMESTAMP       = 13,                    /*               Timestamp */
  ICMP_TIME_REPLY      = 14,                    /*         Timestamp Reply */
  ICMP_INFO_RQST       = 15,                    /*     Information Request */
  ICMP_INFO_REPLY      = 16,                    /*       Information Reply */
  ICMP_ADDR_MASK       = 17,                    /*    Address mask request */
  ICMP_ADDR_MASK_REPLY = 18,                    /*      Address mask reply */
  ICMP_FQDN_REQUEST    = 37,                    /*     Domain Name Request */
  ICMP_FQDN_REPLY      = 38,                    /*       Domain Name Reply */
  ICMP_TYPES           = 39                     /*                  (next) */
} ps_icmp_msg_enum_type;

/*---------------------------------------------------------------------------
  Destination Unreachable codes
---------------------------------------------------------------------------*/
typedef enum
{

  ICMP_NET_UNREACH            = 0, /*                      Net unreachable */
  ICMP_HOST_UNREACH           = 1, /*                     Host unreachable */
  ICMP_PROTO_UNREACH          = 2, /*                 Protocol unreachable */
  ICMP_PORT_UNREACH           = 3, /*                     Port unreachable */
  ICMP_FRAG_NEEDED            = 4, /*      Fragmentation needed and DF set */
  ICMP_SRC_ROUTE_FAIL         = 5, /*                  Source route failed */
  ICMP_DEST_NET_UNKNOWN       = 6, /*          Destination network unknown */
  ICMP_DEST_HOST_UNKNOWN      = 7, /*             Destination host unknown */
  ICMP_SRC_HOST_OSOLATED      = 8, /*      Source host isolated (obsolete) */
  ICMP_DEST_NET_ADMIN_PROHIB  = 9, /* Destination network admin prohibited */
  ICMP_DEST_HOST_ADMIN_PROHIB = 10,/*    Destination host admin prohibited */
  ICMP_NET_UNREACH_TOS        = 11,/*          Network unreachable for TOS */
  ICMP_HOST_UNREACH_TOS       = 12,/*             Host unreachable for TOS */
  ICMP_COMM_ADMIN_PROHIB      = 13,/*       Communication admin prohibited */
  ICMP_HOST_PREC_VIOLATION    = 14,/*            Host precedence violation */
  ICMP_PREC_CUTOFF            = 15,/*          Precedence Cutoff in effect */
  ICMP_UNREACH_MAX            = 16

} ps_icmp_unreach_code_enum_type;

/*---------------------------------------------------------------------------
  Time Exceeded codes
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP_TTL_EXCEED  = 0,               /*             Time-to-live exceeded */
  ICMP_FRAG_EXCEED = 1,               /* Fragment reassembly time exceeded */
  ICMP_EXCEED_MAX  = 2
} ps_icmp_time_exceed_code_enum_type;

/*---------------------------------------------------------------------------
  Redirect message codes
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP_REDIRECT_NET      = 0,              /*     Redirect for the network */
  ICMP_REDIRECT_HOST     = 1,              /*        Redirect for the host */
  ICMP_REDIRECT_TOS_NET  = 2,              /* Redirect for TOS and Network */
  ICMP_REDIRECT_TOS_HOST = 3,              /*    Redirect for TOS and Host */
  ICMP_REDIRECT_MAX      = 4
} ps_icmp_redirect_code_enum_type;


/*---------------------------------------------------------------------------
  Parameter problem message codes
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP_PARAM_IP_HDR_BAD          = 0,           /*           IP header bad */
  ICMP_PARAM_REQD_OPTION_MISSING = 1,           /* Required option missing */
  ICMP_PARAM_BAD_LENGTH          = 2,           /* Required option missing */
  ICMP_PARAM_MAX                 = 3
} ps_icmp_param_prob_code_enum_type;


/*---------------------------------------------------------------------------
  Structure definition for the ICMP Control block.
---------------------------------------------------------------------------*/
struct icmp_cb
{
  q_link_type link;             /* Link to next ICMB                       */
  ip_cb_type ipcb;              /* IP control block                        */
  int32 sockfd;                 /* socket file descriptor                  */
  uint16 family;
  void * reader;
  void * writer;
  void *scb_ptr;                /* Ptr to socket ctl block                 */
  struct
  {
    uint16 id;
    uint16 seq;
  } hdr_arg;
  uint8  type;
  uint8  code;
};


#define ICMPLEN   8                    /* Length of ICMP header on the net */

#define ICMP_MAX_LEN 576      /* Max length of ICMP error message in bytes */



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ICMP_RATE_LIMIT_ON_TYPE()

DESCRIPTION
  This function rate limits outgoing ICMP messages based on type of message.

DEPENDENCIES
  icmpv4_token_bucket is initialized.

RETURN VALUE
  0 - Rate limit the packet (Drop the packet - no token)
  1 - Message can be sent.

SIDE EFFECTS
  None
===========================================================================*/
boolean icmp_rate_limit_on_type
(
  ps_icmp_msg_enum_type type                          /* ICMP message type */
);
/*===========================================================================

FUNCTION ICMP_HDR_CREATE()

DESCRIPTION
  This function will generate an ICMP header in network byte order,link data,
  and compute the checksum.

DEPENDENCIES


RETURN VALUE
  TRUE  - Success
  FALSE - Failure

SIDE EFFECTS
  None
===========================================================================*/
boolean icmp_hdr_create
(
  struct icmp   *icmp,           /* Extracted ICMP header data             */
  dsm_item_type **bp,            /* address of pointer to ICMP packet data */
  pseudo_header_type *ph         /* Pseudo header to get the length of pkt */
);


/*===========================================================================

FUNCTION ICMP_HDR_PARSE()

DESCRIPTION
 This function will parse ICMP header information.

DEPENDENCIES
  None

RETURN VALUE
  0 if the header extraction was successful, else -1

SIDE EFFECTS
  None
===========================================================================*/

int icmp_hdr_parse
(
  dsm_item_type **bpp,
  uint16        *offset,
  struct icmp   *icmp
);



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION ICMP_OUTPUT

DESCRIPTION
  This function does the following:
  1. Converts from host to network all the elements of the ICMP header
     and inserts the ICMP header in the ICMP packet.
  2. Creates the pseudo-header and calculates ICMP checksum.
  3. Sends ICMP packet down to IP.

DEPENDENCIES
  The source IP address should be correct.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void icmp_output
(
  struct ps_in6_addr *dest_ip_ptr,
  struct icmp        *icmp_hdr_ptr,
  dsm_item_type      *bp,
  struct icmp_cb     *icmp_cb_ptr,
  uint16             length,
  ps_tx_meta_info_type  *meta_info_ptr
);


/*===========================================================================

FUNCTION ICMP_INPUT

DESCRIPTION
 This function Process an incoming ICMP packet. The only ICMP messages that
 are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each of these
 messages will be echoed back to the ICMP peer in the base station. Other
 ICMP messages will be handled by dssicmp_ps_input() in dssicmp.c

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void icmp_input
(
  ip_pkt_info_type*        ip_pkt_info_ptr,   /* IP Filter Info            */
  struct dsm_item_s        *item_ptr,         /* Pointer to ICMP message   */
  uint16                   offset             /* Offset of the ICMP packet */
);

/*===========================================================================

FUNCTION ICMP_INIT_TOKEN_BUCKET()

DESCRIPTION
  This function initializes the icmpv4_token_bucket.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void icmp_init_token_bucket(void);

/*===========================================================================

FUNCTION     ICMPV4_FRAG_NEEDED_ERROR()

DESCRIPTION  This function is used to create ICMPv4 Host Unreachable error 
             messages with Fragmentation Needed error code.
             It internally calls icmpv4_error_send() function to send out
             the acutal ICMP packet.

DEPENDENCIES Passed dsm item must contain the complete offending IP packet.

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             uint16            - The max MTU that the sender can use on 
                                 this link.
             ip_pkt_info_type* - IP info for the outgoing message.


RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
void icmpv4_frag_needed_error
(
  dsm_item_type   **data,
  uint16            mtu,
  ip_pkt_info_type *pkt_info
);

/*===========================================================================

FUNCTION ICMP_SET_V4_TOKENS()

DESCRIPTION
  This function sets the values of ICMP v4 tokens per second and max tokens 
  used for calculating the reply rate.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void icmp_set_v4_tokens
(
  uint16 tokens_per_sec,
  uint16 max_tokens
);

/*===========================================================================

FUNCTION     ICMPV4_ERROR_SEND()

DESCRIPTION  This function is used to send ICMPv4 error messages. 

DEPENDENCIES Passed dsm item must contain the complete offending IP packet. 

PARAMETERS   dsm_item_type**   - Dsm item containing the offending packet.
             struct icmp*      - Structure describing the icmp error to send.
             ip_pkt_info_type* - IP info for the outgoing message.
             

RETURN VALUE None 

SIDE EFFECTS 
    DSM item containing offending packet will be freed in case of error; 
    The offending packet will be sent as payload of icmp error packet in case 
    of success.
===========================================================================*/ 
void icmpv4_error_send
(
  dsm_item_type   **data,
  struct icmp      *icmp,
  ip_pkt_info_type *pkt_info
);

#endif  /* ICMP_NEW_H */
