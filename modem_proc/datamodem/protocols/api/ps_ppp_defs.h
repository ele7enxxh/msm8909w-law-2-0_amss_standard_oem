#ifndef PS_PPP_DEFS_H
#define PS_PPP_DEFS_H
/*===========================================================================

                          P S _ P P P _ D E F S . H

DESCRIPTION
  This file definitions that are used for PPP both internally and externally.


Copyright (c) 2001-2013 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_defs.h_v   1.7   03 Feb 2003 15:01:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_defs.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/17/11    ssh    eHRPD Always On support
04/05/11    ssh    New failure codes - for PAP and CHAP
03/23/11    op     Data SU API cleanup
08/24/10    mga    Added additional VSNCP fail reasons to support throttling
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/14/08    pp     Common Modem Interface: Public/Private API split.
11/23/08    scb    Added fix for OMH package to read correct nai_entry_index
09/12/08    pp     Metainfo optimizations.
05/22/08    scb    Fixed compiler warnings.
05/13/07    scb    Added IPv6CP constants into file.
02/28/07    ssh    Removed VJ from IPv6CP code
01/08/07    ssh    Added PPP_STARTING_EV and defines for PPP vendor protocols
11/01/06    ssh    Added auth_proto_mask to hold allowed auth protocols
04/20/06    ssh    PPP mode enum type PPP_INVALID_MODE 
04/05/06    ifk    Added support for IPv6 compression option.
01/17/06    ssh    Removed slots and slot_compress from ipcp_value_type 
11/28/05  ifk/ssh  Added support for ROHC and IPHC over PPP under 
                   FEATURE_DATA_PS_PPP_ROHC and FEATURE_DATA_PS_PPP_IPHC.
09/01/05    ssh    Moved hdlc_mode_enum_type to HDLC header file
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
11/01/04    rr/ks  Added primary_nbns and secondary_nbns options for WINS.
10/31/04    ifk    Added ppp_ccp_config_e_type for CCP configuration
07/16/04    ifk    Renamed PPP control protocol mode enum values
06/15/04    ifk    Added ppp_event_mask_e_type and ppp_event_info_s_type
06/11/04    vp     Included file ps_in.h. Changed IP addresses in 
                   ipcp_value_type to struct ps_in_addr.
06/03/04    jd     Added CCP and Individual link CCP protocols
04/16/04    jd     Updated net_model_resync_fptr_type to have a return value
03/15/04    ifk    Seperated PPP_CP_IGNORE from PPP_CP_INVALID.
09/25/03    ifk    Added enums and macros for IPv6CP.  Moved typedef of
                   net_model_resync_function here from ps_ppp_ipcp.h
08/13/03    ifk    Added data types for updated event handling and config
                   protocol seperation in preparation for IP6CP.
06/03/03    aku    Changed value of PPP_MAX_USER_ID_LEN to 127 from 64.
04/08/03    mvl    Moved PPP_CHAP_MD5 to ps_ppp_defs.h
04/07/03    usb    Added new PPP mode SNOOP_INTERNAL
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
01/31/03    jd     Changed MAX_PASSWD_LEN to reflect NV_PPP_PASSWORD_I length
                   since this is now used instead of NV_PAP_PASSWORD_I
11/04/02    mvl    Added comment explaining use of TX function.  Moved PPP
                   protocol definitions here.
10/11/02    usb    Changed HDLC hardware feature to FEATURE_HDLC_HW_ACCEL
09/24/02    usb    Added user_data to ppp tx and rx function prototypes
09/18/02    mvl    Added user_data to ppp event callbacks.
08/13/02    mvl    Added session handle.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Moved more definitions in here to clean up include
                   dependencies.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added support for event callbacks.
05/27/02    mvl    Renamed file, and moved some more definitions in here.
08/29/01  pjb/mvl  Added fields for DNS servers addresses.
07/28/01    mvl    created module
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_ppp_ext.h"
#include "dsm.h"
#include "ps_in.h"

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF NEGOTIATE_T

DESCRIPTION
  Type for the bitmasks
---------------------------------------------------------------------------*/
typedef uint16 negotiate_t;

/*---------------------------------------------------------------------------
  Config packet header
---------------------------------------------------------------------------*/
struct config_hdr
{
  uint8  code;
  uint8  identifier;
  uint16 length;
};
typedef struct config_hdr ppp_config_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF FSM_SIDE_ENUM_TYPE

DESCRIPTION
  Type that enumerates the local and remote sides of the PPP FSM data
  structure.
---------------------------------------------------------------------------*/
typedef enum side_e
{
  FSM_LOCAL  = 0,
  FSM_REMOTE = 1,
  FSM_SIDE_MAX
} fsm_side_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF RX_F_PTR_TYPE

DESCRIPTION
  typedef for the PPP receive fucntion pointers.
---------------------------------------------------------------------------*/
typedef dsm_item_type* (*rx_f_ptr_type)( void * );

/*---------------------------------------------------------------------------
TYPEDEF TX_F_PTR_TYPE

DESCRIPTION
  typedef for the PPP TX fucntion pointers.

  NOTE: META INFO ptr is passed in the 2nd argument.
---------------------------------------------------------------------------*/  
typedef void  (*tx_f_ptr_type)( dsm_item_type **, void *, void * );

/*---------------------------------------------------------------------------
TYPEDEF CP_MODE_TYPE

DESCRIPTION
  Enum defining the modes of operation for LCP and IPCP.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_CP_MODE_INVALID  = 0,       /* Protocol is not configured            */
  PPP_CP_MODE_ACTIVE   = 1,       /* Set for active bringup                */
  PPP_CP_MODE_PASSIVE  = 2,       /* Set for passive bringup               */
  PPP_CP_MODE_DISABLED = 3,       /* Send Protocol reject; not for LCP     */
  PPP_CP_MODE_DISCARD  = 4,       /* Discards packets for protocol         */
  /* Legacy mode values */
  PPP_CP_ACTIVE        = PPP_CP_MODE_ACTIVE,
  PPP_CP_PASSIVE       = PPP_CP_MODE_PASSIVE,
  PPP_CP_DISABLED      = PPP_CP_MODE_DISABLED,
  PPP_CP_MODE_MAX
} cp_mode_type;


/*---------------------------------------------------------------------------
  Enum for the PPP CCP configuration
---------------------------------------------------------------------------*/
typedef enum ppp_ccp_config_e_type
{
  PPP_CCP_REJECT      = 0,
  PPP_CCP_PASSTHROUGH = 1
} ppp_ccp_config_e_type;

/*---------------------------------------------------------------------------
  Enum for the PPP mode (i.e. used for snooping, and defining data path)
  PPP_NONE_MODE       : When there is no PPP on the interface
  PPP_RELAY_MODE      : No snooping, bypass ppp pkt processing
  PPP_PSEUDO_NET_MODE : By default forward pkt to bridge device,
                        unframe only if required by snoop
  PPP_FULL_NET_MODE   : By default forward pkt to bridge device,
                        always unframe, snoop if required
  PPP_INTERNAL_SNOOP_MODE : By default pass pkt up the stack in phone
                            always unframe, snoop if required
  PPP_INTERNAL_MODE   : Pass all PPP pkts up the stack in phone,
                        always unframe, never snoop
  PPP_INVALID_MODE    : Invalid value of the enum
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_NONE_MODE            = 0,
  PPP_RELAY_MODE           = 1,
  PPP_PSEUDO_NET_MODE      = 2,
  PPP_FULL_NET_MODE        = 3,
  PPP_INTERNAL_SNOOP_MODE  = 4,
  PPP_INTERNAL_MODE        = 5,
  PPP_INVALID_MODE
} ppp_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF NET_MODEL_RESYNC_FPTR_TYPE

DESCRIPTION
  type declaration for the net model resync function

RETURN VALUE
  TRUE  - resync was successful, so put interface in UP state
  FALSE - resync was unsuccessful for reasons dictated by controlling 
          entity, so don't generate UP indication
---------------------------------------------------------------------------*/
typedef boolean (*net_model_resync_fptr_type)
(
  ppp_dev_enum_type device
);

/*---------------------------------------------------------------------------
  enums defining the LCP and IPCP bitmasks for the options that we can
  negotiate.
---------------------------------------------------------------------------*/
enum
{
  LCP_N_MRU  = 1 << 0,
  LCP_N_ACCM = 1 << 1,
  LCP_N_AP   = 1 << 2,
  LCP_N_QMP  = 1 << 3,
  LCP_N_MAGIC= 1 << 4,
  LCP_N_PFC  = 1 << 5,
  LCP_N_ACFC = 1 << 6,
  LCP_N_OPTION_LIMIT = 7     /* this value should always be last shift + 1 */
};

enum
{
  IPCP_N_COMPRESS      = 1 << 0,
  IPCP_N_1ADDRESS      = 1 << 1,
  IPCP_N_PRIMARY_DNS   = 1 << 2,
  IPCP_N_SECONDARY_DNS = 1 << 3,
  IPCP_N_PRIMARY_NBNS   = 1 << 4, /* For WINS support */
  IPCP_N_SECONDARY_NBNS = 1 << 5, /* For WINS support */

  /*-------------------------------------------------------------------------
    The following three are suboptions of the compress option.
    They are not seperate options and should not be counted
    towards IPCP_N_OPTION_LIMIT
  -------------------------------------------------------------------------*/
  IPCP_N_COMPRESS_VJ    = 1 << 6 /* Negotiate VJ as compression protocol */
};

/*---------------------------------------------------------------------------
  The IPCP_N_OPTION_LIMIT should be the number of unique IPCP options that
  can be negotiated.  This value should not count the compression protocol
  flags which are suboptions of the compress option.
---------------------------------------------------------------------------*/
#define  IPCP_N_OPTION_LIMIT   6   

enum
{
  IPV6CP_N_IFACE_ID      = 1 << 0,
  IPV6CP_N_COMPRESS      = 1 << 1
};

/*---------------------------------------------------------------------------
  The IPV6CP_N_OPTION_LIMIT should be the number of unique IPV6CP options
  that can be negotiated.  This value should not count the compression
  protocol flags which are suboptions of the compress option.
---------------------------------------------------------------------------*/
#define  IPV6CP_N_OPTION_LIMIT   2   

typedef struct ppp_vj_config_type
{
  uint8 num_slots;         /* Slots (0-n)                                  */
  uint8 slot_comp;         /* Slots may be compressed (flag)               */
} ppp_vj_config_type;


/*---------------------------------------------------------------------------
  Enum for device authentication role
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_AUTH_ROLE_MIN           = 0,
  PPP_AUTH_ROLE_NONE          = PPP_AUTH_ROLE_MIN,
  PPP_AUTH_ROLE_AUTHENTICATEE = 1,
  PPP_AUTH_ROLE_AUTHENTICATOR = 2,
  PPP_AUTH_ROLE_MAX
} ppp_auth_role_e_type;


/*---------------------------------------------------------------------------
  bitmask for allowed protocols for PPP authentication
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_AUTH_DEFAULT = 0,
  PPP_AUTH_PAP     = 1 << 0,
  PPP_AUTH_CHAP    = 1 << 1,
  PPP_AUTH_EAP     = 1 << 2,
  PPP_AUTH_MAX     = 1 << 30
} ppp_auth_proto_e_type;

/*---------------------------------------------------------------------------
TYPEDEF LCP_VALUE_TYPE

DESCRIPTION
  Table for LCP configuration requests
  Note that the chap_digest and auth_proto_mask are not LCP options, 
  as are other members of this table.
---------------------------------------------------------------------------*/
typedef struct lcp_value_s
{
  uint32                accm;            /* Async Control Char Map         */
  uint32                magic_number;    /* Magic number value             */
  uint16                mru;             /* Maximum Receive Unit           */
  uint16                authentication;  /* Authentication protocol        */
  uint16                monitoring;      /* Quality Monitoring protocol    */
  uint8                 chap_digest;     /* Digest algorithm used for CHAP */
  ppp_auth_proto_e_type auth_proto_mask; /* bitmask for allowed auth protos*/
  boolean               use_same_auth;   /* to use same auth after PPP resync*/ 
} lcp_value_type;


/*---------------------------------------------------------------------------
TYPEDEF IPCP_VALUE_TYPE

DESCRIPTION
  Table for IPCP configuration requests
---------------------------------------------------------------------------*/
typedef struct ipcp_value_s
{
  struct ps_in_addr address;                 /* address for this side         */
  struct ps_in_addr primary_dns;             /* primary dns server address    */
  struct ps_in_addr secondary_dns;           /* secondary dns server address  */
  struct ps_in_addr primary_nbns;	 /* primary nbns server address for WINS   */
  struct ps_in_addr secondary_nbns; /* secondary nbns server address for WINS */
  uint16 compression;                     /* compression protocol          */
  ppp_vj_config_type   vj_config;
} ipcp_value_type;

/*---------------------------------------------------------------------------
TYPEDEF IPV6CP_VALUE_TYPE

DESCRIPTION
  Table for IPV6CP configuration requests
---------------------------------------------------------------------------*/
typedef struct ipv6cp_value_s
{
  uint64 iid;                      /* Interface identifier */
  uint16 compression;              /* Compression protocol */
} ipv6cp_value_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_E_TYPE

DESCRIPTION
  enum of PPP events that can be registered for.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_MIN_EV      = 0,
  PPP_DOWN_EV     = 0,
  PPP_UP_EV       = 1,
  PPP_RESYNC_EV   = 2,
  PPP_STARTING_EV = 3,
  PPP_SUBS_LIMITED_TO_V4_EV = 4,
  PPP_SUBS_LIMITED_TO_V6_EV = 5,
  PPP_MAX_EV,
  PPP_FAILURE_EV  = PPP_DOWN_EV,
  PPP_SUCCESS_EV  = PPP_UP_EV

} ppp_event_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_MASK_E_TYPE

DESCRIPTION
  enum of PPP event mask that can be registered for
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_NO_EV_MASK                 = (0                              ),
  PPP_DOWN_EV_MASK               = (1 << PPP_DOWN_EV               ),
  PPP_UP_EV_MASK                 = (1 << PPP_UP_EV                 ),
  PPP_RESYNC_EV_MASK             = (1 << PPP_RESYNC_EV             ),
  PPP_STARTING_EV_MASK           = (1 << PPP_STARTING_EV           ),
  PPP_SUBS_LIMITED_TO_V4_EV_MASK = (1 << PPP_SUBS_LIMITED_TO_V4_EV ),
  PPP_SUBS_LIMITED_TO_V6_EV_MASK = (1 << PPP_SUBS_LIMITED_TO_V6_EV ),
  PPP_FAILURE_EV_MASK            = (1 << PPP_FAILURE_EV            ),
  PPP_SUCCESS_EV_MASK            = (1 << PPP_SUCCESS_EV            ) 
} ppp_event_mask_e_type;

/*-------------------------------------------------------------------------
  PPP failover enum type
-------------------------------------------------------------------------*/
typedef enum ppp_failover_e_type
{
  PPP_FAILOVER_IGNORE     = 0,
  PPP_FAILOVER_V4_THEN_V6 = 1,
  PPP_FAILOVER_V6_THEN_V4 = 2
} ppp_failover_e_type;

/*---------------------------------------------------------------------------
  Supported Configuration Protocols
---------------------------------------------------------------------------*/
typedef enum ppp_protocol_e_type
{
  PPP_PROTOCOL_MIN    = 0,
  PPP_PROTOCOL_LCP    = PPP_PROTOCOL_MIN,
  PPP_PROTOCOL_IPCP   = 1,
  PPP_PROTOCOL_IPV6CP = 2,
  PPP_PROTOCOL_AUTH   = 3,
  PPP_PROTOCOL_VSNCP  = 4,
  PPP_PROTOCOL_MAX
} ppp_protocol_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_FAIL_REASON_E_TYPE

DESCRIPTION
  Enum describing reason for PPP failure
---------------------------------------------------------------------------*/
typedef enum ppp_fail_reason_e_type
{
  PPP_FAIL_REASON_NONE    = 0,
  PPP_FAIL_REASON_TIMEOUT = 1,
  PPP_FAIL_REASON_AUTH_FAILURE = 2,
  PPP_FAIL_REASON_OPTION_MISMATCH = 3,
  PPP_FAIL_REASON_VSNCP_TIMEOUT = 4,
  PPP_FAIL_REASON_VSNCP_FAILURE = 5,

 /*---------------------------------------------------------------------------
   VSNCP Config-Rej Error fail reasons.
   Keep the codes from PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN to
   PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX continous.
  ---------------------------------------------------------------------------*/
  PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN = 6 ,
  PPP_FAIL_REASON_VSNCP_3GPP2I_GEN_ERROR = 
                                        PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN,
  PPP_FAIL_REASON_VSNCP_3GPP2I_UNAUTH_APN = 7,
  PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_LIMIT_EXCEED = 8,
  PPP_FAIL_REASON_VSNCP_3GPP2I_NO_PDN_GW = 9,
  PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_GW_UNREACH = 10 ,
  PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_GW_REJ = 11,
  PPP_FAIL_REASON_VSNCP_3GPP2I_INSUFF_PARAM = 12,
  PPP_FAIL_REASON_VSNCP_3GPP2I_RESOURCE_UNAVAIL = 13,
  PPP_FAIL_REASON_VSNCP_3GPP2I_ADMIN_PROHIBIT = 14,
  PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_ID_IN_USE = 15,
  PPP_FAIL_REASON_VSNCP_3GPP2I_SUBSCR_LIMITATION = 16,
  PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN = 17,
  PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED = 19,
  PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX = 30, /* future-proof VSNCP reasons */
  PPP_FAIL_REASON_AUTH_PAP_FAILURE = 31,
  PPP_FAIL_REASON_AUTH_CHAP_FAILURE = 32,
  PPP_FAIL_REASON_CLOSE_IN_PROGRESS = 33,
  PPP_FAIL_REASON_MAX
} ppp_fail_reason_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_CBACK_F_PTR_TYPE

DESCRIPTION
  type of the callback function that is called when a PPP event occurs.  
---------------------------------------------------------------------------*/
typedef void (*ppp_event_cback_f_ptr_type)
(
  ppp_dev_enum_type    device,
  ppp_protocol_e_type  protocol,
  ppp_event_e_type     ppp_event,
  void                *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  fail_reason
);

/*---------------------------------------------------------------------------
  Type for PPP event information - allocated from ps_mem
---------------------------------------------------------------------------*/
typedef struct ppp_event_info_s_type
{
  q_link_type                 link;
  ppp_dev_enum_type           device;
  ppp_event_cback_f_ptr_type  cback[PPP_PROTOCOL_MAX];
  void                       *user_data[PPP_PROTOCOL_MAX];
  ppp_event_mask_e_type       event_mask[PPP_PROTOCOL_MAX];
  void                        *ncp_handle_ptr; 
} ppp_event_info_s_type;

/*===========================================================================
FUNCTION PPP_LSD_RESULT_CBACK_F_TYPE()

DESCRIPTION
  Function pointer for the callback called when PPP link status determination
  (triggered via PPP_IOCTL_TRIGGER_LSD) is complete.

PARAMETERS
  device:        PPP device for which LSD result is being reported
  lsd_succeeded: TRUE or FALSE indicating whether LSD passed or failed

  user_data:     Optional ptr supplied while invoking PPP_IOCTL_TRIGGER_LSD
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef void (*ppp_lsd_result_cback_f_type)
(
  ppp_dev_enum_type device,
  boolean           lsd_succeeded,
  void            * user_data
);

/*---------------------------------------------------------------------------
  typedef for ppp link status determination configuration
  MUST pass non-zero values for attempts and timeout, and MUST supply a
  result callback
---------------------------------------------------------------------------*/
typedef struct ppp_lsd_config_s_type 
{  
  uint8 echo_req_attempts;                    /* LCP Echo-Request attempts */
  uint32 echo_req_timeout;                    /* LCP Echo-Request timeout  */
  ppp_lsd_result_cback_f_type result_cback;   /* Result callback           */
  void * user_data;                           /* Uninterpreted user data   */
} ppp_lsd_config_type;
/*---------------------------------------------------------------------------
  Enum defining auth mode
---------------------------------------------------------------------------*/
enum
{
  PPP_NO_AUTH = 1,
  PPP_DO_PAP  = 2,
  PPP_DO_CHAP = 3,
  PPP_DO_EAP  = 4 
};

/*---------------------------------------------------------------------------
  define for the CHAP digests that we support
---------------------------------------------------------------------------*/
#define PPP_CHAP_MD5               5  /* Value of the MD5 algorithm        */

/*---------------------------------------------------------------------------
  defines for the PPP protocol field - the ones we care about
---------------------------------------------------------------------------*/
#define PPP_NO_PROTOCOL       0x0000  /* used for internal processing      */
#define PPP_ROHC_SCID_PROTOCOL 0x0003 /* ROHC small-CID Protocol           */
#define PPP_ROHC_LCID_PROTOCOL 0x0005 /* ROHC large-CID Protocol           */
#define PPP_IP_PROTOCOL       0x0021  /* Internet Protocol                 */
#define PPP_IPV6_PROTOCOL     0x0057  /* Internet Protocol v6              */
#define PPP_VJCH_PROTOCOL     0x002d  /* Van Jacobson Compressed TCP/IP    */
#define PPP_VJUCH_PROTOCOL    0x002f  /* Van Jacobson Uncompressed TCP/IP  */
#define PPP_IPCP_PROTOCOL     0x8021  /* Internet Protocol Control Protocol*/
#define PPP_IPV6CP_PROTOCOL   0x8057  /* IPV6CP                            */
#define PPP_LCP_PROTOCOL      0xc021  /* Link Control Protocol             */
#define PPP_PAP_PROTOCOL      0xc023  /* Password Authentication Protocol  */
#define PPP_CHAP_PROTOCOL     0xc223  /* Challenge Handshake Auth Protocol */
#define PPP_EAP_PROTOCOL      0xc227 /* Extensible Authentication Protocol */

/*---------------------------------------------------------------------------
 Point-to-Point Protocol (PPP) Vendor Protocol numbers
---------------------------------------------------------------------------*/
#define PPP_VSNP_PROTOCOL     0x005b  /* Vendor-Specific Network Protocol  */
#define PPP_VSP_PROTOCOL      0x405b  /* Vendor-Specific Protocol          */
#define PPP_VSNCP_PROTOCOL    0x805b  /* Vendor-Specific Nw Ctrl Protocol  */
#define PPP_VSAP_PROTOCOL     0xc05b  /* Vendor-Specific Auth Protocol     */

/*---------------------------------------------------------------------------
  IPHC (RFC 2509) protocol numbers
---------------------------------------------------------------------------*/
#define PPP_IPHC_PROTOCOL      0x0061 /* IP header compression (2509)      */
#define PPP_IPHC_FULL_HEADER_PROTOCOL            0x0061
#define PPP_IPHC_COMPRESSED_TCP_PROTOCOL         0x0063
#define PPP_IPHC_COMPRESSED_TCP_NODELTA_PROTOCOL 0x2063
#define PPP_IPHC_COMPRESSED_NON_TCP_PROTOCOL     0x0065
#define PPP_IPHC_CONTEXT_STATE_PROTOCOL          0x2065
#define PPP_IPHC_COMPRESSED_UDP_8_PROTOCOL       0x0067
#define PPP_IPHC_COMPRESSED_UDP_16_PROTOCOL      0x2067
#define PPP_IPHC_COMPRESSED_RTP_8_PROTOCOL       0x0069
#define PPP_IPHC_COMPRESSED_RTP_16_PROTOCOL      0x2069

/*---------------------------------------------------------------------------
  defines for the PPP protocol field - the ones we don't care about
---------------------------------------------------------------------------*/
#define PPP_OSINL_PROTOCOL    0x0023  /* OSI Network Layer                 */
#define PPP_XNS_PROTOCOL      0x0025  /* Xerox NS IDP                      */
#define PPP_DN4_PROTOCOL      0x0027  /* DECnet Phase IV                   */
#define PPP_AT_PROTOCOL       0x0029  /* Appletalk                         */
#define PPP_IPX_PROTOCOL      0x002b  /* Novell IPX                        */
#define PPP_BPDU_PROTOCOL     0x0031  /* Bridging PDU                      */
#define PPP_ST2_PROTOCOL      0x0033  /* Stream Protocol (ST-II)           */
#define PPP_BV_PROTOCOL       0x0035  /* Banyan Vines                      */

#define PPP_8021D_PROTOCOL    0x0201  /* 802.1d Hello Packets              */
#define PPP_LUXCOM_PROTOCOL   0x0231  /* Luxcom                            */
#define PPP_SIGMA_PROTOCOL    0x0233  /* Sigma Network Systems             */

#define PPP_OSINLCP_PROTOCOL  0x8023  /* OSI Network Layer Control Protocol*/
#define PPP_XNSCP_PROTOCOL    0x8025  /* Xerox NS IDP Control Protocol     */
#define PPP_DN4CP_PROTOCOL    0x8027  /* DECnet Phase IV Control Protocol  */
#define PPP_ATCP_PROTOCOL     0x8029  /* Appletalk Control Protocol        */
#define PPP_IPXCP_PROTOCOL    0x802b  /* Novell IPX Control Protocol       */
#define PPP_BNCP_PROTOCOL     0x8031  /* Bridging NCP                      */
#define PPP_ST2CP_PROTOCOL    0x8033  /* Stream Protocol Control Protocol  */
#define PPP_BVCP_PROTOCOL     0x8035  /* Banyan Vines Control Protocol     */
#define PPP_CCP_PROTOCOL      0x80FD  /* Compression Control Protocol      */
#define PPP_ILCCP_PROTOCOL    0x80FB  /* Individual Link CCP               */

#define PPP_LQR_PROTOCOL      0xc025  /* Link Quality Report               */

/*---------------------------------------------------------------------------
TYPEDEF HDLC_MODE_ENUM_TYPE

DESCRIPTION
  Enum defining the hdlc modes: off, software, hardware.
---------------------------------------------------------------------------*/
typedef enum hdlc_mode_enum_type
{
  HDLC_OFF           = 0,
  HDLC_SW            = 1,
  HDLC_HW_PREF       = 2,
  PP_HDLC_HW_PREF    = 3,
  HDLC_A2_AUTONOMOUS = 4,
  HDLC_INVALID_MODE /* this should always be the last entry! */
} hdlc_mode_enum_type;

/*---------------------------------------------------------------------------
ENUM UNFRAME_STATE

DESCRIPTION
  The HDLC unframing mode
---------------------------------------------------------------------------*/
typedef enum hdlc_unframe_state_type
{
  FINDING_7E  = 0,              /* looking for 7E                          */
  FINDING_SOP = 1,              /* looking for start of packet - after 7Es */
  SNOOPING    = 2,              /* early proto detect - looking for proto  */
  FINDING_EOP = 3,              /* looking for end of packet               */
  UNFRAMING   = 4               /* unframing to pass to proto msg detect   */
} hdlc_unframe_state_type;

/*---------------------------------------------------------------------------
  Link Phases - these are the PPP phases.  It is defined as a bit mask to
  allow checking for multiple states more efficiently.
---------------------------------------------------------------------------*/
typedef enum
{
  pppDEAD         = 0x1,     /* Waiting for physical layer                 */
  pppESTABLISH    = 0x2,     /* Link Establishment Phase                   */
  pppAUTHENTICATE = 0x4,     /* Authentication Phase                       */
  pppNETWORK      = 0x8,     /* Network Protocol Phase                     */
  pppTERMINATE    = 0x10     /* Termination Phase                          */
} ppp_link_phase_type;

#endif /* PS_PPP_DEFS_H */
