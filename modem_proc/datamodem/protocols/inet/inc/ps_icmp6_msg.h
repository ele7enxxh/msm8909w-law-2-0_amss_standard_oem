#ifndef PS_ICMP6_MSG_H
#define PS_ICMP6_MSG_H
/*===========================================================================

                         P S _ I C M P 6 _ M S G . H

DESCRIPTION
  Internet Control Message Protocol (ICMP) version 6 message header file
  (RFC 2463 and 2461)

EXTERNALIZED FUNCTIONS
  ps_icmp6_msg_parse()  This function is used to process incoming ICMP6 
                        messages and provides the parsed message as an 
                        output parameter.
                        
  ps_icmp6_msg_create() This function is used to create outgoing ICMP6 
                        messages. This function creates the associated ICMP6 
                        message using the parameters specified by the caller.
    

 Copyright (c) 2003-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_icmp6_msg.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14   ssh     IP Translation Engine 464XLAT 
06/13/05   vp      Added mtu to unon msg_type.
03/02/05   vp      Enums for ICMPv6 types and codes. typedef 
                   ps_icmp6_cn_hdr_type as icmp_hdr.
07/30/04   vp      Added code for parameter problem message definitions.
06/11/04   vp      Removed inclusion of internet.h and included ps_pkt_info.h
04/05/04   sv      Featurized the header file.
03/02/04   rc      Added structure for Router Solicitation Messages.
11/17/03   sv      Added offset declarations for fields in router adv. 
                   Added comments.
11/11/03  rc/aku   Initial Checkin
10/02/02   sv      Added design review changes.
08/20/03   aku     Initial version
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "customer.h"
#include "ps_pkt_info.h"
#include "ps_icmp_hdr.h"
#include "dsm.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define ICMP6_CMN_HDR_SIZE (4)
#define ICMP6_TYPE_OFFSET (0)
#define ICMP6_CODE_OFFSET (1)
#define ICMP6_CKSUM_OFFSET (2)

#define ICMP6_RA_BASE_HDR_SIZE (12)
#define ICMP6_RA_HOP_LIMIT_OFFSET (0)
#define ICMP6_RA_FLAGS_OFFSET (1)
#define ICMP6_RA_LIFE_TIME_OFFSET (2)
#define ICMP6_RA_REACHABLE_TIMER_OFFSET (4)
#define ICMP6_RA_RETRANS_TIMER_OFFSET (8)

#define ICMP6_RA_PREFIX_OPT_SIZE (30)
#define ICMP6_RA_PREFIX_LENGTH_OFFSET (0)
#define ICMP6_RA_PREFIX_FLAGS_OFFSET (1)
#define ICMP6_RA_PREFIX_VALID_LIFETIME_OFFSET (2)
#define ICMP6_RA_PREFIX_PREF_LIFETIME_OFFSET (6)
#define ICMP6_RA_PREFIX_ADDR_OFFSET (14)

#define ICMP6_RA_RDNSS_OPTION_MIN_LEN (3)
#define ICMP6_RA_RDNSS_OPTION_LEN_1_RDNSS_ADDR ICMP6_RA_RDNSS_OPTION_MIN_LEN
#define ICMP6_RA_RDNSS_OPTION_LEN_2_RDNSS_ADDR (ICMP6_RA_RDNSS_OPTION_MIN_LEN + 2)
#define ICMP6_RA_RDNSS_OPTION_RESERVED_SIZE (2)

#define ICMP6_RA_MANAGED_ADDR_CFG_MASK (0x080)
#define ICMP6_RA_OTHER_STATEFUL_CFG_MASK (0x040)
#define ICMP6_RA_ONLINK_FLAG_MASK (0x080)
#define ICMP6_RA_AUTO_ADDR_CFG_FLAG_MASK (0x040)

#define ICMP6_RA_MTU_OPTION_RESERVED_SIZE (2)

#define ICMP6_RS_BASE_HDR_SIZE (4)
#define ICMP6_RS_RESERVED_OFFSET (0)

#define ICMP6_RS_SRC_LINK_LAYER_ADDR_OFFSET (0)

#define ICMP6_NA_BASE_HDR_SIZE (20)
#define ICMP6_NA_FLAGS_OFFSET (0)
#define ICMP6_NA_TARGET_ADDRESS_OFFSET (4)

#define ICMP6_NA_ROUTER_FLAG_MASK (0x080)
#define ICMP6_NA_SOLICITED_FLAG_MASK (0x040)
#define ICMP6_NA_OVERRIDE_FLAG_MASK (0x020)

#define ICMP6_NA_TRGT_LINK_LAYER_ADDR_OFFSET (0)

#define ICMP6_NS_BASE_HDR_SIZE (20)
#define ICMP6_NS_RESERVED_OFFSET (0)
#define ICMP6_NS_TARGET_ADDRESS_OFFSET (4)

#define ICMP6_NS_SRC_LINK_LAYER_ADDR_OFFSET (0)

#define ICMP6_RDIR_BASE_HDR_SIZE (36)
#define ICMP6_RDIR_RESERVED_OFFSET_OFFSET (0)
#define ICMP6_RDIR_TARGET_ADDRESS_OFFSET (4)
#define ICMP6_RDIR_DEST_ADDRESS_OFFSET (20)

#define ICMP6_RDIR_RESERVED_OFFSET (0)
#define ICMP6_RDIR_REDIRECT_PKT_OFFSET (6)

#define ICMP6_MLD_GRP_ADDR_OFFSET (4)

#define ICMP6_PKT_TOO_BIG_MTU_OFFSET             (4)
#define ICMP6_PKT_TOO_BIG_MTU_SIZE               (4)

#define ICMP6_PAR_PROB_PTR_OFFSET                (4)
#define ICMP6_PAR_PROB_PTR_SIZE                  (4)

#define ICMP6_PAR_PROB_PTR_VER_TRF_CLASS         (0)
#define ICMP6_PAR_PROB_PTR_TRF_CLASS_FLOW_LABEL  (1)
#define ICMP6_PAR_PROB_PTR_FLOW_LABEL_MIN        (2)
#define ICMP6_PAR_PROB_PTR_FLOW_LABEL_MAX        (3)
#define ICMP6_PAR_PROB_PTR_PAYLOAD_LEN_MIN       (4)
#define ICMP6_PAR_PROB_PTR_PAYLOAD_LEN_MAX       (5)
#define ICMP6_PAR_PROB_PTR_NEXT_HDR              (6)
#define ICMP6_PAR_PROB_PTR_HOP_LIMIT             (7)
#define ICMP6_PAR_PROB_PTR_SRC_ADDR_MIN          (8)
#define ICMP6_PAR_PROB_PTR_SRC_ADDR_MAX          (23)
#define ICMP6_PAR_PROB_PTR_DST_ADDR_MIN          (24)
#define ICMP6_PAR_PROB_PTR_DST_ADDR_MAX          (39)


/*---------------------------------------------------------------------------
  Typedef for different ICMP6 messages types.  
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------
    Error Messages - MSB is set to 0 - Range is 0 through 127
  -------------------------------------------------------------------------*/
  ICMP6_DEST_UNRCH        =   1,
  ICMP6_PKT_TOO_BIG       =   2,
  ICMP6_TIME_EXCEEDED     =   3,
  ICMP6_PAR_PROBLEM       =   4,

  /*-------------------------------------------------------------------------
    Informational Messages - MSB is set to 1 - Range is 128 through 255
  -------------------------------------------------------------------------*/
  ICMP6_ECHO_REQ          = 128,
  ICMP6_ECHO_RSP          = 129,
  ICMP6_MLD_QUERY 	  = 130, 
  ICMP6_MLD_REPORT	  = 131, 
  ICMP6_MLD_DONE	  = 132,  
  ICMP6_RTR_SOL           = 133,
  ICMP6_RTR_ADV           = 134,
  ICMP6_NGBR_SOL          = 135,
  ICMP6_NGBR_ADV          = 136,
  ICMP6_RDIR_MSG          = 137
} ps_icmp6_msg_enum_type;

/*---------------------------------------------------------------------------
  Destination Unreachable codes 
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP6_NOROUTE_UNRCH      = 0,          /*        No route to destination */
  ICMP6_ADMIN_PROHIB_UNRCH = 1,          /*    Administratively prohibited */
  ICMP6_BEYOND_SCOPE_UNRCH = 2,          /* Beyond scope of source address */
  ICMP6_ADDR_UNRCH         = 3,          /*            Address unreachable */
  ICMP6_PORT_UNRCH         = 4,          /*               Port unreachable */
  ICMP6_MAX_UNRCH          = 5
} ps_icmp6_unrch_code_enum_type;

/*---------------------------------------------------------------------------
  Typedef for Time Exceeded codes.
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP6_HOP_LIMIT_EXCEED  = 0,        /*     Hop limit exceeded in transit */
  ICMP6_FRAG_REASS_EXCEED = 1,        /* Fragment reassembly time exceeded */
  ICMP6_MAX_EXCEED        = 2
} ps_icmp6_time_exceeded_code_enum_type;

/*---------------------------------------------------------------------------
  Typedef for Parameter Problem codes.
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP6_ERRONEOUS_HDR_FIELD   = 0,             /*   Erroneous header field */
  ICMP6_UNRECOGNIZED_NEXT_HDR = 1,             /* Unrecognised next header */
  ICMP6_UNRECOGNIZED_IPV6_OPT = 2,             /* Unrecognised IPv6 option */
  ICMP6_MAX_PAR_PROBLEM       = 3
} ps_icmp6_par_prob_code_enum_type;

/*---------------------------------------------------------------------------
  Typedef for different ICMP6 option types.  
---------------------------------------------------------------------------*/
typedef enum
{
  ICMP6_OPTION_SOURCE_LINK_LAYER_ADDR_TYPE = 1,
  ICMP6_OPTION_TARGET_LINK_LAYER_ADDR_TYPE = 2,
  ICMP6_OPTION_PREFIX_TYPE = 3,
  ICMP6_OPTION_REDIRECT_HEADER_TYPE = 4,
  ICMP6_OPTION_MTU_TYPE = 5,
  ICMP6_OPTION_ADV_INTERVAL_TYPE = 7,
  ICMP6_OPTION_HA_INFO_TYPE = 8,
  ICMP6_OPTION_ROUTE_INFO_TYPE = 9,
  ICMP6_OPTION_RDNSS_TYPE = 25
} ps_icmp6_option_enum_type;


typedef enum
{
  ICMP6_ND_DAD = 1,
  ICMP6_ND_ADDR_RESOLUTION = 2
} ps_icmp6_nd_msg_type;
/*---------------------------------------------------------------------------
  Generic ICMP6 header type  
  struct icmp_hdr
  {
    uint8  type;
    uint8  code;
    uint16 chksum;
  };
---------------------------------------------------------------------------*/
typedef struct icmp_hdr ps_icmp6_cmn_hdr_type;

/*---------------------------------------------------------------------------
  Maximum Number of Prefix Options supported.
---------------------------------------------------------------------------*/
#define ICMP6_MAX_PREFIX_OPTIONS 1

/*---------------------------------------------------------------------------
  Maximum Number of RDNSS options supported.
---------------------------------------------------------------------------*/
#define ICMP6_MAX_RDNSS_OPTIONS 2

/*---------------------------------------------------------------------------
  Maximum Number of RDNSS addresses supported per RDNSS option.
---------------------------------------------------------------------------*/
#define ICMP6_MAX_RDNSS_OPTION_ADDRS 2

/*---------------------------------------------------------------------------
  Maximum Number of Route Information Options supported.
---------------------------------------------------------------------------*/
#define ICMP6_MAX_ROUTE_INFO_OPTIONS 1

/*===========================================================================
                         Neighbor Discovery Options
===========================================================================*/

/*-----------------------------------------------------------------------
  Typedef for Source/Target Link-Layer Address
-----------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  ps_link_layer_addr_type ll_addr;
} ps_icmp6_ll_addr_type;

typedef struct
{
  uint8  type;
  uint8  mtu_len;
  uint32 mtu;

} ps_icmp6_mtu_opt_type;

/*---------------------------------------------------------------------------
  Typedef for Prefix Information Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   type;
  uint8   length;
  uint8   prefix_length;/* Number of leading bits in prefix that are valid */
  boolean on_link_flag;     /* Can prefix be use for onlink determination? */
  boolean autonomous_address_cfg_flag;/*can prefix be used for autoconfig? */
  uint32  pref_lifetime;           /* Time in sec that the prefix is valid */
  uint32  valid_lifetime;      /* Time in sec that the prefix is preferred */
  uint64  prefix_addr[2];         /* IP address or prefix of an IP address */
} ps_icmp6_prefix_type;

/*---------------------------------------------------------------------------
  Typedef for Redirected Header Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  uint8  length;
  char   *packet;
} ps_icmp6_redirect_header_opt_type;

/*---------------------------------------------------------------------------
  Typedef for Advertisement Interval Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  uint8  length;
  uint32 adv_interval;
} ps_icmp6_adv_interval_type;

/*---------------------------------------------------------------------------
  Typedef for Home Agent Information Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  uint8  length;
  uint16 home_agent_preference;
  uint16 home_agent_lifetime;
} ps_icmp6_ha_info_opt_type;

/*---------------------------------------------------------------------------
  Typdef for Route Information Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  uint8  length;
  uint8  prefix_length;
  uint32 route_lifetime;
  struct ps_in6_addr prefix;
} ps_icmp6_route_info_opt_type;

/*---------------------------------------------------------------------------
  Typdef for Recursive DNS Server (RDNSS) Option
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  type;
  uint8  length;
  uint32 lifetime;
  uint8  rdnss_addrs_cnt;
  struct ps_in6_addr rdnss_addrs[ICMP6_MAX_RDNSS_OPTION_ADDRS];
} ps_icmp6_rdnss_opt_type;

/*---------------------------------------------------------------------------
  Typedef for Router Solicitation.  
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                  src_link_addr_incl; /* link addr option included*/
  ps_icmp6_ll_addr_type    src_link_addr; 
} ps_icmp6_rtr_sol_type;

/*---------------------------------------------------------------------------
  Typedef for Router advertisement.  
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                 curr_hop_limit; /* Hop count field for outgoing ip */
  boolean               managed_addr_cfg_flag;   /* Managed address config */ 
  boolean               other_stateful_cfg_flag; /* Other stateful config  */
  boolean               home_agent_flag;   /* advertising rtr is also a HA */
  uint16                rtr_lifetime; /*life time of default router in sec */
  uint32                reachable_timer; /* neighbor reacheable time in ms */
  uint32                retrans_timer; /* Time in ms between retransmitted */
                                       /* neighbor solicitation messages   */
  boolean               src_link_addr_incl; /* link addr option included ? */
  ps_icmp6_ll_addr_type src_link_addr;          /* source link address */
  boolean               mtu_incl;                  /* mtu option included? */
  ps_icmp6_mtu_opt_type mtu;                                        /* mtu */
  uint16                prefix_option_cnt;  /* Number of prefixes received */
  ps_icmp6_prefix_type  prefix_option[ICMP6_MAX_PREFIX_OPTIONS]; /* prefix */
  uint16                   rdnss_option_cnt;     /* No. of rdnss opts rxed */
  ps_icmp6_rdnss_opt_type  rdnss_option[ICMP6_MAX_RDNSS_OPTIONS]; /* rdnss */

  /* currently unsupported options */
  boolean                      adv_interval_option_incl;  
  ps_icmp6_adv_interval_type   adv_interval; /* Advertisement interval opt */
  uint16                       route_info_option_cnt; /* No. of route opts */
  ps_icmp6_route_info_opt_type route_info[ICMP6_MAX_ROUTE_INFO_OPTIONS];
} ps_icmp6_rtr_adv_type;

/*---------------------------------------------------------------------------
  Typedef for Neighbor Solicitation.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                reserved;
  struct ps_in6_addr    target_addr;
  boolean               src_link_addr_incl; /* link addr option included ? */
  ps_icmp6_ll_addr_type src_link_addr;          /* source link address */
} ps_icmp6_nbr_sol_type;

/*---------------------------------------------------------------------------
  Typedef for Neighbor Advertisement.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean               router_flag;
  boolean               solicited_flag;
  boolean               override_flag;
  uint32                reserved;
  struct ps_in6_addr    target_addr;
  boolean               trgt_link_addr_incl;  /* link addr option included */
  ps_icmp6_ll_addr_type trgt_link_addr;         /* target link address */
} ps_icmp6_nbr_adv_type;

/*---------------------------------------------------------------------------
  Typedef for Redirect.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                reserved;
  struct ps_in6_addr    target_addr;
  struct ps_in6_addr    dest_addr;
  boolean               rdir_opt_incl;
  ps_icmp6_redirect_header_opt_type  rdir_opt;
  boolean               trgt_link_addr_incl;  /* link addr option included */
  ps_icmp6_ll_addr_type trgt_link_addr;         /* target link address */
} ps_icmp6_redirect_type;

/*---------------------------------------------------------------------------
  Typedef for Echo Request and Echo Reply.  
---------------------------------------------------------------------------*/
typedef struct
{
  uint16         identifier;              /* Echo request/reply identifier */
  uint16         sequence_num;       /* Echo request/reply sequence number */
} ps_icmp6_echo_msg_type;






/*---------------------------------------------------------------------------
  Typedef for MLD msgs
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr mcast_grp_addr;    /* MLD Address                 */
  uint16 	     max_response_delay;/* Only useful for mld queries */
  uint16	     reserved; 	        /* Set0 by sender, ignored by receivers*/     	
} ps_icmp6_mld_type;



/*---------------------------------------------------------------------------
  Typedef for Parameter Problem message.
  Identifies the octet offset within the invoking packet where the error 
  occurred. 
---------------------------------------------------------------------------*/
typedef uint32 ps_icmp6_param_prob_offset;

/*---------------------------------------------------------------------------
  Typedef for ICMP6 message type.  
  
  Add more comments indicating what the discriminator is going to be.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_icmp6_cmn_hdr_type cmn_hdr;                      /* ICMP6 base header */
  union
  {
    ps_icmp6_rtr_sol_type   rs;       /* ICMP6 Router Solicitation Message */
    ps_icmp6_rtr_adv_type   ra;      /* ICMP6 router Advertisement message */
    ps_icmp6_nbr_sol_type   ns;     /* ICMP6 Neighbor Solicitation Message */
    ps_icmp6_nbr_adv_type   na;    /* ICMP6 Neighbor Advertisement Message */
    ps_icmp6_redirect_type  rdir;                /* ICMP6 Redirect Message */
    ps_icmp6_echo_msg_type  echo_msg;        /* ICMP6 echo request message */
    ps_icmp6_mld_type       mld;            /* ICMP6 mld query    Message */ 
    ps_icmp6_param_prob_offset param_prob_offset; /* Param problem Message */
    uint32                  mtu;
  } msg_type;

} ps_icmp6_msg_type;             

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION     PS_ICMP6_MSG_PARSE()

DESCRIPTION  This function is used to process incoming ICMP6 messages and 
             provides the parsed message as an output parameter.

   The ICMPv6 messages have the following general format:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Type      |     Code      |          Checksum             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                         Message Body                          +
      |                                                               |
PARAMETERS
  data: Incoming ICMP packet
  offset: ICMP header start.
  msg: ICMP message header.

DEPENDENCIES 
  None

RETURN VALUE boolean - TRUE on success
                       FALSE on failure

SIDE EFFECTS 
  Fills in the icmp message header.
===========================================================================*/
boolean ps_icmp6_msg_parse
(
  dsm_item_type     *data,
  uint16            *offset,
  ps_icmp6_msg_type *msg
);

/*===========================================================================
FUNCTION     PS_ICMP6_MSG_CREATE()

DESCRIPTION  This function is used to create outgoing ICMP6 messages. This 
             function creates the associated ICMP6 message using the 
             parameters specified by the caller.

   The ICMPv6 messages have the following general format:

       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Type      |     Code      |          Checksum             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      +                         Message Body                          +
      |                                                               |

PARAMETERS
  data: Outgoing ICMP packet
  msg: ICMP message header.
  ph : Psuedo header.
DEPENDENCIES 
  None

RETURN VALUE 
   A dsm_item_type ptr if message successfully built, NULL otherwise
            
SIDE EFFECTS:
   Packs the ICMP header into DSM item.
===========================================================================*/
boolean ps_icmp6_msg_create
(
  dsm_item_type     **payload,
  ps_icmp6_msg_type *msg,
  pseudo_header_type *ph
);


#endif /* PS_ICMP6_MSG_H */
