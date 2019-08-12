/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ C O R E _ V 4 . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client core code.  This
  file provides the necessary the DHCP protocol logic, message
  generation, processing and so on.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_core_v4.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/27/15    ds     Fix to handle expansion of DNS names accordingly as both v4 
                   v6 have different encoding techniques. 
07/04/14    ss     Added support for generating 
                   IFACE_EXTENDED_IP_CONFIG_EX_EV
01/10/14    vl     Optimization of F3 messages
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
12/14/12    ssb    Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR
11/18/10    cp     Fix to ignore the parsing of non-spanning options.
01/25/10    ss     Added DHCP_CLIENT_IP_DECLINED status.
01/11/10    ss     Lint warning fixes.
08/07/09    kk     Added support for client identifier option (opt type=61).
05/22/09    am     Fixed memory leak for unfreed Core options.
02/18/09    am     Verifying IP addr after receiving ACK as per RFC2131 sec 
                   3.1, earlier we verified after receiving OFFER.
01/29/09    pp     Added support for SLIP interface.
24/11/08    am     High/Medium lint fixes.
09/26/08    am     Changed malloc/free names for off target support.
08/25/08    am     Added DHCP ongoing determination.
07/10/08    am     Removed client lease release from core_stop().
05/29/08    am     Corrected clear_lease() functionality.
03/26/08    am     KlocWork fixes.
03/14/08    am     Changed RENEW event handling in BOUND and
                   RENEWING state.
01/29/08    am     Added Rapid Commit Option handling.
05/15/07    es     Fixed bug in f_ack where ip_addr was being set to yi_addr
                   instead of zero.
05/03/07    es     Removed outstanding_inform flag to use state instead,
                   moved restriction of inform to dhcpi_client_inform,
                   instead of dhcpi_client_f. Replaced params struct in init
                   with core_params.
04/17/07    es     I-WLAN changes, including use of option request flags to
                   generate parameter request option, sending to unicast
                   address, issuing configuration event, restricting multiple
                   inform msgs.  Removed changes to support earlier IOCTL.
02/21/07    rt     Restructured the code as per file's coding guidelines.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
11/06/06    ks     Cleared the configuration info in dhcp_client_r
10/31/06    es     Fixed lint issues.
08/22/06    es     Fixed bugs where inform timer_count not incremented on
                   retry, timer_count not reset on inform API call.
08/11/06    es     Added SIP option handling.
06/19/06    clp/sv Pad messages out to minimum size for broken DHCP servers.
05/22/06    lyr/rt Changed order of calling status_cb() to after setting
                   config for successful IP assignment case.
10/20/05   as/vrk  Fixed errors reported by Coverity.
09/08/05    clp    Added event logging.
08/18/05    clp    Only set timer if time not infinite.
07/06/05    clp    Send release before releasing lease/calling callback.
05/12/05    clp    Fixed lint issues.
05/11/05    clp    Fixed memory leaks.
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
08/13/04    clp    Initial development work done.
===========================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  The bulk of this file is the state machine.  The state machine is
  described in RFC-2131, particularly in figure 5, but other places as
  well.

  The state machine consists of functions for each event for each
  state, as well as an arival function for each state.

  The states are as follows:
  - PAUSE       (P)
  - REBOOTING   (R)
  - INIT        (I)
  - SELECTING   (S)
  - REQUESTING  (RQ)
  - VERIFY      (V)
  - BOUND       (B)
  - RENEWING    (RN)
  - REBINDING   (RB)
  - INFORM      (I)

  The events that can occur are:
  - offer \
  - ack    +- Server replies
  - nak   /
  - renew
  - release
  - decline
  - timer

  In addition, there are meta-events:
  - init
  - stop

  Meta-events are events that have a common, unique action that
  doesn't need to be encoded into the state machine.  In particular,
  these do the following: init will initialize the state machine to
  start in the approprate state (either INIT or INIT-REBOOT) and stop
  will stop the state machine, clearing any timers that may be
  running.

  In the following state/event table transition table, N means Ignored,
  and '?' means a conditional transition, else its considered N.

      | offer | ack | nak | renew | release | decline | timer | inform |
  ----+-------+-----+-----+-------+---------+---------+-------+--------+
  P   |  N    | N   | N   |  I  R |   P     |   N     | N     |   F    |
  R   |  N    | V   | I   |  R    |   P     |   I     | R  I  |   N    |
  I   |  N    | N   | N   |  I    |   P     |   N     | S     |   N    |
  S   |  RQ   | V?  | N   |  I    |   P     |   I?    | S  I  |   N    |
  RQ  |  N    | V   | I   |  I    |   P     |   I     | RQ I  |   N    |
  V   |  N    | N   | N   |  I    |   P     |   I     | B     |   N    |
  B   |  N    | N   | N   |  RN   |   P     |   I     | RN    |   N    |
  RN  |  N    | V   | I   |  RB   |   P     |   I     | RN RB |   N    |
  RB  |  N    | V   | I   |  R    |   P     |   I     | RB I  |   N    |
  F   |  N    | P   | N   |  N    |   P     |   N     | F     |   F    |
  ----+-------+-----+-----+-------+---------+---------+-------+--------+

  In addition to the state transitions above, there may also be other
  actions that happen on an event, or state transition, such as
  setting a timer.  Some of these will be data dependend, such as the
  lenght of time that the timer is set, the number of time to timeout,
  etc.  Note that in some states in the table above different state
  transitions occur when the timer fires.  The first state is what
  happens if the maximum number of timeout has not happened yet, and
  the second is if the maximum has been hit.

  The state machine will be advanced by 2 functions. One will handle
  an event, look up the current state and the event in the state
  machine table, and call the appropriate function.  The second will
  handle state transitions, which will set the current state, and call
  the enter state function.

  On incoming packets, it is necessary to filter them for what is
  expected, depending on the current progress.  This is accomplished
  by the receiving function by checking which data (XID, Server ID,
  etc) matches with the currently expected data, if any.  The state
  machine will expect transitions to be valid.

  The following table shows the messages sent per state transition.
  The messages are abreviated in the following way:
    DHCPREQUEST : RQ
    DHCPRELEASE : RL
    DHCPDECLINE : DE
    DHCPDISCOVER : DI
    DHCPINFORM  : IN
    None        : <blank>

  Some of these messages go out every time the state is entered, such
  as DHCPDISCOVER in the selecting state and DHCPREQUEST in the
  rebooting state, so these will be handled by the state transition
  function instead.  These are marked with '*'.  Some message are
  conditionally sent out, and these are denoted with '?'.

      | offer | ack | nak | renew | release | decline | timer | inform |
  ----+-------+-----+-----+-------+---------+---------+-------+--------+
  P   |       |     |     |  RQ?* |         |         |       |   IN*  |
  R   |       |     |     |  RQ*  |   RL    |   DE    |  RQ?* |        |
  I   |       |     |     |       |         |         |  DI*  |        |
  S   |       |     |     |       |         |   DE?   |  DI?* |        |
  RQ  |       |     |     |       |   RL    |   DE    |  RQ*  |        |
  V   |       |     |     |       |         |   DE    |       |        |
  B   |       |     |     |  RQ   |   RL    |   DE    |  RQ   |        |
  RN  |       |     |     |  RQ   |   RL    |   DE    |  RQ   |        |
  RB  |       |     |     |  RQ*  |   RL    |   DE    |  RQ   |        |
  F   |       |     |     |       |         |         |  IN*  |   IN*  |
  ----+-------+-----+-----+-------+---------+---------+-------+--------+
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include <string.h>
#include "dhcp_clienti.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ps_in.h"
#include "event.h"
#include "event_defs.h"
#include "dhcp_client_event.h"
#include "ps_dnsi.h"
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/* bootp core magic number (see RFC 2131, section 3) */
#define DHCP_MAGIC_NUM (dss_htonl(0x63825363))

/* Bootp flags field: Broadcast bit and the rest Must Be Zero */
#define DHCP_CLIENT_BOOTP_FLAG_BROADCAST (dss_htons(0x8000))
#define DHCP_CLIENT_BOOTP_FLAG_MBZ_MASK (~DHCP_CLIENT_BOOTP_FLAG_BROADCAST)

/* Error checking macro */
#define DHCP_SET_IF_FAIL( pass, expr ) \
  ((pass) = (expr) ? (pass) : FALSE)

/* Sizes and Offsets of some important bootp message fields, in bytes */
#define DHCP_CLIENT_BOOTP_SIZE (240)
#define DHCP_CLIENT_CHADDR_LEN (16)
#define DHCP_CLIENT_SNAME_LEN (64)
#define DHCP_CLIENT_FILE_LEN (128)

#define DHCP_CLIENT_BOOTP_OFFSET (0)
#define DHCP_CLIENT_SNAME_OFFSET (44)
#define DHCP_CLIENT_FILE_OFFSET (108)
#define DHCP_CLIENT_OPTION_OFFSET (240)

/* SIP OPTION data offset due to enc byte */
#define DHCP_CLIENT_SIP_OPTION_DATA_OFFSET (1)
/* Lengths of options */
#define DHCP_CLIENT_OPTION_IP_LEN 4
#define DHCP_CLIENT_OPTION_MTU_LEN 2
#define DHCP_CLIENT_OPTION_TIME_LEN 4
#define DHCP_CLIENT_OPTION_MSG_TYPE_LEN 1
#define DHCP_CLIENT_OPTION_SIP_DN_MIN_LEN 3

/* Option checking values */
#define DHCP_CLIENT_MTU_MIN 68 /* Via RFC 2132, 5.1 */

/* INPUT convienence macros */
#define DHCP_COPY_INPUT_OPTION_N_BYTES(pass, info, field, N) \
  do { \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->extract \
                    ( \
                      (info)->core.field, \
                      (info)->msg, \
                      (info)->offset, \
                      (N) \
                    ) ); \
  (info)->offset += (N); } while (0)

#define DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, field) \
  do { \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->extract \
                    ( \
                      &(info)->core.field, \
                      (info)->msg, \
                      (info)->offset, \
                      4 \
                    ) ); \
  (info)->offset += 4; } while (0)

#define DHCP_COPY_INPUT_OPTION_2_BYTES(pass, info, field) \
  do { \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->extract \
                    ( \
                      &(info)->core.field, \
                      (info)->msg, \
                      (info)->offset, \
                      2 \
                    ) ); \
  (info)->offset += 2; } while (0)

#define DHCP_COPY_INPUT_OPTION_1_BYTES(pass, info, field) \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->extract \
                    ( \
                      &(info)->core.field, \
                      (info)->msg, \
                      (info)->offset++, \
                      1 \
                    ) )

/* Output convienence macros */
#define DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, field, N) \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->config.client_request_pushdown_tail \
                    ( \
                      (info)->config.client_request_handle, \
                      &(msg), \
                      (field), \
                      (N) \
                    ) )

#define DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 4)

#define DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 2)

#define DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 1)

#define DHCP_CLIENT_MAX_OPTION_HEADER_LEN (2)

/* These values per RFC 2132, 9.3 */
#define DHCP_CLIENT_OPTION_OVERLOAD_FILE (1)
#define DHCP_CLIENT_OPTION_OVERLOAD_SNAME (2)
#define DHCP_CLIENT_OPTION_OVERLOAD_FILE_SNAME (3)

/* Minimum packet length to send to work around borken DHCP servers */
#define DHCP_CLIENT_MIN_MESSAGE_LEN 300

/* Size(in bytes) of the client_id length that precedes the actual
 * client_id in incoming/outgoing messages */
#define DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN     (1)

/* RFC 2132 mandates the use of client_id_type '0' for all custom
 * (i.e. non-hardware-address) client identifiers. */
#define DHCP_CLIENT_CLIENT_ID_TYPE_CUSTOM       (0)

#define IP_VERSION_4 0
/* -------------------------------------------------------------------
   Enum for the operation of message.
  ------------------------------------------------------------------- */
typedef enum {
  DHCP_CLIENT_BOOTP_OP_BOOTREQUEST = 1,
  DHCP_CLIENT_BOOTP_OP_BOOTREPLY = 2
} DHCP_CLIENT_BOOTP_op_enum;

/* -------------------------------------------------------------------
   Enum for the type of message.
  ------------------------------------------------------------------- */
typedef enum {
  DHCP_CLIENT_DHCPDISCOVER = 1,
  DHCP_CLIENT_DHCPOFFER = 2,
  DHCP_CLIENT_DHCPREQUEST = 3,
  DHCP_CLIENT_DHCPDECLINE = 4,
  DHCP_CLIENT_DHCPACK = 5,
  DHCP_CLIENT_DHCPNACK = 6,
  DHCP_CLIENT_DHCPRELEASE = 7,
  DHCP_CLIENT_DHCPINFORM = 8
} dhcp_client_msg_type;

typedef enum {
  DHCP_CLIENT_MIN_S        = -1,
  DHCP_CLIENT_PAUSE_S      = 0,
  DHCP_CLIENT_REBOOT_S     = 1,
  DHCP_CLIENT_INIT_S       = 2,
  DHCP_CLIENT_SELECTING_S  = 3,
  DHCP_CLIENT_REQUESTING_S = 4,
  DHCP_CLIENT_VERIFY_S     = 5,
  DHCP_CLIENT_BOUND_S      = 6,
  DHCP_CLIENT_RENEWING_S   = 7,
  DHCP_CLIENT_REBINDING_S  = 8,
  DHCP_CLIENT_INFORM_S     = 9,
  DHCP_CLIENT_MAX_S        = 10
} dhcp_client_state_enum;

typedef enum {
  DHCP_CLIENT_MIN_E     = -1,
  DHCP_CLIENT_OFFER_E   = 0,
  DHCP_CLIENT_ACK_E     = 1,
  DHCP_CLIENT_NAK_E     = 2,
  DHCP_CLIENT_RENEW_E   = 3,
  DHCP_CLIENT_RELEASE_E = 4,
  DHCP_CLIENT_DECLINE_E = 5,
  DHCP_CLIENT_TIMER_E   = 6,
  DHCP_CLIENT_INFORM_E  = 7,
  DHCP_CLIENT_MAX_E     = 8
} dhcp_client_event_enum;

/* Option values we need to know about.  See RFC 2132, 3361. 
 * when adding a new option, update the below macro too
 * if the option is a spanning option.
 */
typedef enum {
  DHCP_CLIENT_OPTION_PAD           = 0,
  DHCP_CLIENT_OPTION_END           = 255,
  DHCP_CLIENT_OPTION_SUBNET_MASK   = 1,
  DHCP_CLIENT_OPTION_ROUTER        = 3,
  DHCP_CLIENT_OPTION_DNS           = 6,
  DHCP_CLIENT_OPTION_HOSTNAME      = 12,
  DHCP_CLIENT_OPTION_DOMAIN_NAME   = 15,
  DHCP_CLIENT_OPTION_MTU           = 26,
  DHCP_CLIENT_OPTION_REQUESTED_IP  = 50,
  DHCP_CLIENT_OPTION_IP_LEASE      = 51,
  DHCP_CLIENT_OPTION_OVERLOAD      = 52,
  DHCP_CLIENT_OPTION_MSG_TYPE      = 53,
  DHCP_CLIENT_OPTION_SERVER_ID     = 54,
  DHCP_CLIENT_OPTION_PARAMETER_REQ = 55,
  DHCP_CLIENT_OPTION_MESSAGE       = 56,
  DHCP_CLIENT_OPTION_MAX_SIZE      = 57,
  DHCP_CLIENT_OPTION_RENEW_TIME    = 58,
  DHCP_CLIENT_OPTION_REBIND_TIME   = 59,
  DHCP_CLIENT_OPTION_CLIENT_ID     = 61,
  DHCP_CLIENT_OPTION_RAPID_COMMIT  = 80, /* RFC 4039 */
  DHCP_CLIENT_OPTION_SIP           = 120,
  DHCP_CLIENT_OPTION_FORCE_32BIT   = 0x7fffffff
} dhcp_client_option_enum;

/* Macro to check whether the option is a spanning option or not.
 * This check is required to avoid parsing of non spanning options
 * (duplicate options) and just ignore them.
 */
#define IS_SPANNING_OPTION(option) \
    (                                           \
     ( option == DHCP_CLIENT_OPTION_ROUTER ) || \
     ( option == DHCP_CLIENT_OPTION_DNS ) ||    \
     ( option == DHCP_CLIENT_OPTION_SIP )       \
    )

/* Option bookkeeping info */
typedef struct dhcp_client_meta_option {
  struct dhcp_client_meta_option * next;
  dhcp_client_option_enum option;
  uint32 len;
  uint8 * option_txt;
} dhcp_client_meta_option;

typedef struct {
  /* bootp message parameters themselves */
  DHCP_CLIENT_BOOTP_op_enum op; /* message op code */
  uint8 htype;           /* hardware address type */
  uint32 hlen;           /* hardware address length */
  uint32 hops;           /* number of hops (bootp relays) */
  uint32 xid;            /* random transaction id */
  uint16 secs;           /* number of seconds client attemping */
  uint16 flags;          /* flags (broadcast/unicast) */
  uint32 ciaddr;         /* client IP address */
  uint32 yiaddr;         /* your (assigned) IP address */
  uint32 siaddr;         /* next server IP adderss in bootstrap process */
  uint32 giaddr;         /* relay agent IP address */
  uint8  chaddr[DHCP_CLIENT_CHADDR_LEN]; /* Client hardware address */
  uint8  sname[DHCP_CLIENT_SNAME_LEN];   /* server host name (optional) */
  uint8  file[DHCP_CLIENT_FILE_LEN];     /* boot file name */
  uint32 magic;                   /* 99.130.83.99 dec == 63.82.53.63 hex */

  dhcp_client_meta_option * opt;

  /* The following option information are stored in NETWORK byte order! */
  /* Subnet mask */
  uint32 netmask;
  /* Router address */
  uint32 router_addr;
  /* DNS addresses */
  dhcp_client_dns * dns;
  /* Domain Name */
  uint8 * domain_name;
  uint32 domain_name_len;
  /* SIP addresses */
  dhcp_client_sip_addr * sip_addr;
  /* SIP domain name */
  dhcp_client_sip_domain_name * sip_domain_name;
  /* MTU */
  uint16 mtu;
  /* DHCP message type */
  dhcp_client_msg_type dhcp_msg_reply;
  /* Server ID from client */
  uint32 server_id;
  /* ip lease times in seconds relative to now, see RFC2131, 4.4.5 */
  uint32 ip_lease;
  uint32 renew_time;
  uint32 rebind_time;
  /* Set RC if received in DHCPACK message. RFC 4039 */
  boolean rapid_commit_option;

  /* The following are the placeholders for parameters related to
   * DHCP_CLIENT_OPTION_CLIENT_ID found in incoming NACK messages. */

  /* The client identifier type byte */
  uint8  client_id_type;
  /* The client identifier length byte */
  uint8  client_id_len;
  /* The actual client identifier string */
  /* NOTE:
   * This isn't featurized with FEATURE_DATA_PS_DHCP_CLIENT_ID_CUSTOM
   * because this buffer is needed (to copy the client_id) even when
   * DHCP_CLIENT_ID_OPTTYPE_HWADDR is used. */
  uint8 *client_id;
} dhcp_client_core_msg_info;

typedef struct {
  /* The given IP address */
  uint32 lease_ip;
  /* Lease server */
  uint32 server_id;
  /* Lease time from server */
  uint32 lease_time;
  /* Renew time from server, or calculated */
  uint32 T1_time;
  /* Rebind time from server, or calculated */
  uint32 T2_time;
} dhcp_client_core_lease;

typedef struct dhcp_client_core_info {
  /* module init info */
  dhcp_client_core_init init;
  /* module config info */
  dhcp_client_core_config config;
  /* Message info */
  dhcp_client_core_msg_info core;
  /* Lease info */
  dhcp_client_core_lease lease;

  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_client_core_info * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_CLIENT_CORE_COOKIE ((uint32)0xC03EC03E)
  uint32 cookie;
  /* The current state of this module */
  dhcp_client_state_enum state;
  /* The current event, if we are processing an event */
  dhcp_client_event_enum event;
  /* If we are restarting due to a decline, then we need to implement
   * a timer.  This boolean is true in this case.
   */
  boolean decline_restart;
  /* Number of times timer has been hit in current state, or
   * number of seconds in current state, if used
   */
  uint32 timer_count;
  /* Address to use in the ciaddr for the request message.  This is
   * filled out in the state event funcion that sends the request
   * message
   */
  uint32 request_ciaddr;
  /* Send requested IP address option depends on the state from which
   * it was sent. This is filled out by the state event function that
   * sends the reqeust message
   */
  boolean request_ip_addr_option;
  /* The server ID is only sent from some states. This is filled out
   * by the state event function that sends the request message
   */
  boolean request_server_id_option;
  /* Send broadcast depends on the state the message was sent from.
   * This variable is set from the event state function that sends the
   * message, or the message specific send function if instances of a
   * message are sent the same way
   * See RFC 2131, table 4, and section 4.4.4.
   */
  boolean send_broadcast;
  /* The extract function used by the parse routines to get to data in
   * the message
   */
  boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len);
  /* The message handle to use in the extract function above */
  void * msg;
  /* The size of the outgoing message, so that memory can be
   * pre-allocated if needed by the send API
   */
  uint32 size;
  /* The offset into the outgoing message */
  uint32 offset;
  /* Is the incoming message overloaded, and if so, how? */
  uint8 overload;
  /* The hardware address, which is copied from the init pointer on
   * new.
   */
  uint8 hwaddr[DHCP_CLIENT_CHADDR_LEN];
  /* The init count is how many times we have transitioned through the init
   * state.  If this exceeds the core_params.init_tries, we will inform the user
   * with a status callback inidicating failure
   */
  uint32 init_count;
  /* The inform ip address is the address that we have been
   * provisioned with (somehow) that we use for the ciaddr in the
   * inform message.  This is in network byte order.
   */
  uint32 inform_addr;
  /* The diag event type to send the next event report as */
  dhcp_client_diag_event_enum_type diag_event_type;

} dhcp_client_core_info;

/* This prototype defines the state functions for the event/state
 * table and the state transition table below
 */
typedef void (*dhcp_client_fnc_type)(dhcp_client_core_info *);

/* States for parsing options */
typedef enum {
  DHCP_CLIENT_OPTION_RET_SUCCESS = 0,
  DHCP_CLIENT_OPTION_RET_FAIL    = 1,
  DHCP_CLIENT_OPTION_RET_DONE    = 2
} dhcp_client_option_ret_enum;

/* The following option defines the each element in the option handler
 * lookup table.
 */
typedef struct
{
  dhcp_client_option_enum op;
  boolean (*handler)(dhcp_client_core_info *);
} dhcp_client_option_lookup_table_type;

/*===========================================================================

                           FORWARD REFERENCES

===========================================================================*/
/* Helper functions */
static dhcp_client_option_ret_enum
dhcpi_client_option_alloc_link
(
  dhcp_client_core_info   * info,
  dhcp_client_option_enum   option_num,
  uint32             len   /* of alloc and option */
);

/* Generic event functions */
static void dhcpi_client_ack(dhcp_client_core_info *);
static void dhcpi_client_nak(dhcp_client_core_info *);
static void dhcpi_client_nak_status(dhcp_client_core_info *);
static void dhcpi_client_decline(dhcp_client_core_info *);
static void dhcpi_client_decline_status(dhcp_client_core_info *);
static void dhcpi_client_inform(dhcp_client_core_info *);
/* Functions for Pause state */
static void dhcpi_client_p_renew(dhcp_client_core_info *);
static void dhcpi_client_p_release(dhcp_client_core_info *);
/* Functions for Reboot state */
static void dhcpi_client_r_renew(dhcp_client_core_info *);
static void dhcpi_client_r_release(dhcp_client_core_info *);
static void dhcpi_client_r_timer(dhcp_client_core_info *);
/* Functions for Init state */
static void dhcpi_client_i_renew(dhcp_client_core_info *);
static void dhcpi_client_i_release(dhcp_client_core_info *);
static void dhcpi_client_i_timer(dhcp_client_core_info *);
/* Functions for Selecting state */
static void dhcpi_client_s_offer(dhcp_client_core_info *);
static void dhcpi_client_s_renew(dhcp_client_core_info *);
static void dhcpi_client_s_release(dhcp_client_core_info *);
static void dhcpi_client_s_timer(dhcp_client_core_info *);
static void dhcpi_client_s_ack(dhcp_client_core_info *);
static void dhcpi_client_s_decline(dhcp_client_core_info *);
/* Functions for the Verifying state */
static void dhcpi_client_v_renew(dhcp_client_core_info *);
static void dhcpi_client_v_release(dhcp_client_core_info *);
static void dhcpi_client_v_timer(dhcp_client_core_info *);
/* Functions for ReQuest state */
static void dhcpi_client_rq_renew(dhcp_client_core_info *);
static void dhcpi_client_rq_release(dhcp_client_core_info *);
static void dhcpi_client_rq_timer(dhcp_client_core_info *);
/* Functions for Bound state */
static void dhcpi_client_b_renew(dhcp_client_core_info *);
static void dhcpi_client_b_release(dhcp_client_core_info *);
static void dhcpi_client_b_timer(dhcp_client_core_info *);
/* Functions for ReNewing state */
static void dhcpi_client_rn_renew(dhcp_client_core_info *);
static void dhcpi_client_rn_release(dhcp_client_core_info *);
static void dhcpi_client_rn_timer(dhcp_client_core_info *);
/* Functions for ReBinding state */
static void dhcpi_client_rb_renew(dhcp_client_core_info *);
static void dhcpi_client_rb_release(dhcp_client_core_info *);
static void dhcpi_client_rb_timer(dhcp_client_core_info *);
/* Functions for the inForm state */
static void dhcpi_client_f_ack(dhcp_client_core_info *);
static void dhcpi_client_f_release(dhcp_client_core_info *);
static void dhcpi_client_f_timer(dhcp_client_core_info *);

/* Functions for state arrival function table */
static void dhcpi_client_p(dhcp_client_core_info *);
static void dhcpi_client_r(dhcp_client_core_info *);
static void dhcpi_client_i(dhcp_client_core_info *);
static void dhcpi_client_s(dhcp_client_core_info *);
static void dhcpi_client_v(dhcp_client_core_info *);
static void dhcpi_client_rq(dhcp_client_core_info *);
static void dhcpi_client_b(dhcp_client_core_info *);
static void dhcpi_client_rn(dhcp_client_core_info *);
static void dhcpi_client_rb(dhcp_client_core_info *);
static void dhcpi_client_f(dhcp_client_core_info *);

/* Functions for parsing options */
static boolean
  dhcpi_client_option_subnet_mask_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_router_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_dns_handler(dhcp_client_core_info *);
static boolean
  dhcpi_client_option_domain_name_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_mtu_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_ip_lease_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_msg_type_handler(dhcp_client_core_info *);
static boolean
  dhcpi_client_option_server_id_handler(dhcp_client_core_info *);
static boolean
  dhcpi_client_option_renew_time_handler(dhcp_client_core_info *);
static boolean
  dhcpi_client_option_rebind_time_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_message_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_sip_handler(dhcp_client_core_info *);
static boolean dhcpi_client_option_rapid_commit_handler(dhcp_client_core_info *);
static boolean
  dhcpi_client_option_client_id_handler(dhcp_client_core_info *);
/*===========================================================================

                           STATIC DATA

===========================================================================*/


/* This table describes the event functions for each state */
static const dhcp_client_fnc_type
dhcp_client_state_event_fnc[DHCP_CLIENT_MAX_S][DHCP_CLIENT_MAX_E] =
{
  /* DHCP_CLIENT_PAUSE_S */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    NULL,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_p_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_p_release,
    /* DHCP_CLIENT_DECLINE_E */
    NULL,
    /* DHCP_CLIENT_TIMER_E   */
    NULL,
    /* DHCP_CLIENT_INFORM_E  */
    dhcpi_client_inform,
  },
  /* DHCP_CLIENT_REBOOT_S     */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_ack,
    /* DHCP_CLIENT_NAK_E     */
    dhcpi_client_nak,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_r_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_r_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_r_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_INIT_S       */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    NULL,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_i_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_i_release,
    /* DHCP_CLIENT_DECLINE_E */
    NULL,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_i_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_SELECTING_S  */
  {
    /* DHCP_CLIENT_OFFER_E   */
    dhcpi_client_s_offer,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_s_ack,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_s_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_s_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_s_decline,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_s_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_REQUESTING_S */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_ack,
    /* DHCP_CLIENT_NAK_E     */
    dhcpi_client_nak,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_rq_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_rq_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_rq_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_VERIFY_S */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    NULL,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_v_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_v_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_v_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_BOUND_S      */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    NULL,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_b_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_b_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline_status,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_b_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_RENEWING_S   */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_ack,
    /* DHCP_CLIENT_NAK_E     */
    dhcpi_client_nak_status,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_rn_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_rn_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline_status,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_rn_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_REBINDING_S  */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_ack,
    /* DHCP_CLIENT_NAK_E     */
    dhcpi_client_nak_status,
    /* DHCP_CLIENT_RENEW_E   */
    dhcpi_client_rb_renew,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_rb_release,
    /* DHCP_CLIENT_DECLINE_E */
    dhcpi_client_decline_status,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_rb_timer,
    /* DHCP_CLIENT_INFORM_E  */
    NULL,
  },
  /* DHCP_CLIENT_INFORM_S */
  {
    /* DHCP_CLIENT_OFFER_E   */
    NULL,
    /* DHCP_CLIENT_ACK_E     */
    dhcpi_client_f_ack,
    /* DHCP_CLIENT_NAK_E     */
    NULL,
    /* DHCP_CLIENT_RENEW_E   */
    NULL,
    /* DHCP_CLIENT_RELEASE_E */
    dhcpi_client_f_release,
    /* DHCP_CLIENT_DECLINE_E */
    NULL,
    /* DHCP_CLIENT_TIMER_E   */
    dhcpi_client_f_timer,
    /* DHCP_CLIENT_INFORM_E  */
    dhcpi_client_inform,
  }
};

/* This table describes the functions that are called on entry to each
 * state
 */
static const dhcp_client_fnc_type
dhcp_client_state_fnc[DHCP_CLIENT_MAX_S] =
{
  /* DHCP_CLIENT_PAUSE_S */
  dhcpi_client_p,
  /* DHCP_CLIENT_REBOOT_S     */
  dhcpi_client_r,
  /* DHCP_CLIENT_INIT_S       */
  dhcpi_client_i,
  /* DHCP_CLIENT_SELECTING_S  */
  dhcpi_client_s,
  /* DHCP_CLIENT_REQUESTING_S */
  dhcpi_client_rq,
  /* DHCP_CLIENT_VERIFY_S     */
  dhcpi_client_v,
  /* DHCP_CLIENT_BOUND_S      */
  dhcpi_client_b,
  /* DHCP_CLIENT_RENEWING_S   */
  dhcpi_client_rn,
  /* DHCP_CLIENT_REBINDING_S  */
  dhcpi_client_rb,
  /* DHCP_CLIENT_INFORM_S     */
  dhcpi_client_f
};

/* The following table defines the option handlers to gather the
 * information from messages recieved.
 */
static const dhcp_client_option_lookup_table_type dhcp_client_option_lookup[] =
{
#define DHCP_OPTION_LOOKUP_ELEMENT(u,l) \
  { DHCP_CLIENT_OPTION_## u, dhcpi_client_option_## l ##_handler }
  DHCP_OPTION_LOOKUP_ELEMENT(SUBNET_MASK, subnet_mask),
  DHCP_OPTION_LOOKUP_ELEMENT(ROUTER, router),
  DHCP_OPTION_LOOKUP_ELEMENT(DNS, dns),
  DHCP_OPTION_LOOKUP_ELEMENT(MTU, mtu),
  DHCP_OPTION_LOOKUP_ELEMENT(IP_LEASE, ip_lease),
  DHCP_OPTION_LOOKUP_ELEMENT(MSG_TYPE, msg_type),
  DHCP_OPTION_LOOKUP_ELEMENT(SERVER_ID, server_id),
  DHCP_OPTION_LOOKUP_ELEMENT(RENEW_TIME, renew_time),
  DHCP_OPTION_LOOKUP_ELEMENT(REBIND_TIME, rebind_time),
  DHCP_OPTION_LOOKUP_ELEMENT(DOMAIN_NAME, domain_name),
  DHCP_OPTION_LOOKUP_ELEMENT(MESSAGE, message),
  DHCP_OPTION_LOOKUP_ELEMENT(SIP, sip),
  DHCP_OPTION_LOOKUP_ELEMENT(RAPID_COMMIT, rapid_commit),
  DHCP_OPTION_LOOKUP_ELEMENT(CLIENT_ID, client_id)
}; /* dhcp_client_option_lookup[] */
#define DHCP_OPTION_TABLE_SIZE \
  (sizeof(dhcp_client_option_lookup)/\
   sizeof(dhcp_client_option_lookup_table_type))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*========================================================================
                         GENERAL HELPER FUNCTIONS
========================================================================*/
/*=========================================================================
STATIC FUNCTION DHCPI_CLIENT_FREE_CORE_OPTIONS_MEM

DESCRIPTION
  Used to explicitly free memory for allocated options in core.
  Needed in a case when DHCP Client Core is processing a message from
  server and needs to send a reply too.
  In this case, Client allocates mem for opts as it parses the message
  from server, and on processing an event to send a reply it flushes out
  the core without freeing the memory.
  Note these mem are not from core.opt but from individual opt handlers.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_client_core_info * info       - The current client handle.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
static void dhcpi_client_free_core_options_mem
(
 dhcp_client_core_info * info
)
{
  dhcp_client_dns  * dns;
  dhcp_client_sip_addr * sip_addr;
  dhcp_client_sip_domain_name * sip_dn;

  /* sanity check */
  if (NULL == info)
  {
    LOG_MSG_ERROR_0("NULL arg passed");
    return;
  }
  
  if ( NULL != info->core.domain_name )
  {
    info->init.d_free( info->core.domain_name );
    info->core.domain_name = NULL;
    info->core.domain_name_len = 0;
  }

  while ( NULL != info->core.dns)
  {
    dns = info->core.dns->next;
    info->init.d_free( info->core.dns );
    info->core.dns = dns;
  }

  while ( NULL != info->core.sip_addr )
  {
    sip_addr = info->core.sip_addr->next;
    info->init.d_free( info->core.sip_addr );
    info->core.sip_addr = sip_addr;
  }

  while ( NULL != info->core.sip_domain_name )
  {
    sip_dn = info->core.sip_domain_name->next;
    info->init.d_free( info->core.sip_domain_name );
    info->core.sip_domain_name = sip_dn;
  }

  if ( NULL != info->core.client_id )
  {
	info->init.d_free(info->core.client_id);
	info->core.client_id = NULL;
  }

}/* dhcpi_client_free_core_options_mem */

/*=========================================================================
STATIC FUNCTION DHCPI_CLIENT_INIT_PARAMETER_REQUEST_LIST_OPTION

DESCRIPTION
  This function initializes the parameter request list (prl) option
  internally, based on the boolean parameter values initialized
  in info->init.core_params. Note RFC 2132: "The client MAY list the options in
  order of preference.", our implementation currently uses an arbitrary
  order.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_client_core_info * info       - The current client handle.
  uint32                * option_len - Address to place final length of
                                       option.
  boolean    request_ip_lease_option - Whether ip_lease option
                                       (RFC 2132, 9.2) is requested in
                                       parameter request list.

RETURN VALUE
  None.

SIDE EFFECTS
  A new parameter request list option is inserted and initialized at the front of the
  info->core_msg.opt list.
=========================================================================*/
static boolean
dhcpi_client_init_parameter_request_list_option
(
  dhcp_client_core_info * info,
  uint32                * option_len,
  boolean                 request_ip_lease_option
)
{
  boolean ret = TRUE;
  uint32 optlen = 0;
  uint32 prl_count = 0;
  uint8* optcodeptr;

  /* sanity check */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("info NULL");
    return FALSE;
  }
  if ( NULL == option_len )
  {
    DATA_ERR_FATAL("optlen NULL");
    return FALSE;
  }

  /* Get length of prl data, each option takes one byte width */

  /* Always request Server Identifier option */
  optlen++;
  if ( TRUE == request_ip_lease_option )
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.domain_name_server_option )
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.subnet_mask_option)
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.router_option )
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.interface_mtu_option )
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.domain_name_option )
  {
    optlen++;
  }
  if ( TRUE == info->init.core_params.sip_server_option )
  {
    optlen++;
  }

  /* Allocate space for meta option struct and init */
  if (DHCP_CLIENT_OPTION_RET_SUCCESS != dhcpi_client_option_alloc_link(
                                        info,
                                        DHCP_CLIENT_OPTION_PARAMETER_REQ,
                                        optlen))
  {
    LOG_MSG_ERROR_0("Failed to allocate link for PRL option");
    ret = FALSE;
  }
  else
  {
    /* Insert option codes into option data */
    optcodeptr = info->core.opt->option_txt;
    prl_count = 0;

    optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_SERVER_ID;
    if ( TRUE == request_ip_lease_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_IP_LEASE;
    }
    if ( TRUE == info->init.core_params.domain_name_server_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_DNS;
    }
    if ( TRUE == info->init.core_params.subnet_mask_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_SUBNET_MASK;
    }
    if ( TRUE == info->init.core_params.router_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_ROUTER;
    }
    if ( TRUE == info->init.core_params.interface_mtu_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_MTU;
    }
    if ( TRUE == info->init.core_params.domain_name_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_DOMAIN_NAME;
    }
    if ( TRUE == info->init.core_params.sip_server_option )
    {
      optcodeptr[prl_count++] = (uint8)DHCP_CLIENT_OPTION_SIP;
    }

    /* error check count with allocated size */
    if ( prl_count != info->core.opt->len )
    {
      LOG_MSG_ERROR_2("prl_count != SIZE ALLOCATED, %d, %d",
                       prl_count, info->core.opt->len);
      ASSERT(0);
      ret = FALSE;
    }
  }

  *option_len = optlen;
  return ret;
} /* dhcpi_client_init_parameter_request_list_option */

/*========================================================================
                         CALLBACK FUNCTIONS
========================================================================*/
/*=========================================================================
STATIC FUNCTION DHCPI_CLIENT_GET_OPTION_DATA_BYTES

DESCRIPTION
  This function extracts option_data bytes from a meta_option struct. This
  is handed to callback functions needing option data bytes.

DEPENDENCIES
  None.

PARAMETERS
  void * src      - The meta_option struct to extract the data from.
  uint32 offset   - Where to start extracting data from (Note: in relation
                    to start of option bytes, code and len byte counted)
  char * dst_buf - Where to put the data.
  uint32 len      - How much data to extract.

RETURN VALUE
  int - 0 if extracted successful. -1 if failed to extract.

SIDE EFFECTS
  None.
=========================================================================*/
static int
dhcpi_client_get_option_data_bytes
(
  void   * src,          /* The option struct to get data from */
  uint32   offset,       /* Where to start extracting data */
  char   * dst_buf,      /* Destination buffer */
  uint16   dst_buf_size,  /* Destination buffer size */
  uint32   len           /* Number of bytes to get */
)
{
  int retval;
  dhcp_client_meta_option* opt = (dhcp_client_meta_option *) src;

  if ( NULL == src )
  {
    DATA_ERR_FATAL("get_option_data_bytes: src NULL");
    return -1;
  }

  if ( NULL == dst_buf )
  {
    DATA_ERR_FATAL("get_option_data_bytes: dst_buf NULL");
    return -1;
  }

  if ( DHCP_CLIENT_MAX_OPTION_HEADER_LEN > offset )
  {
    LOG_MSG_ERROR_2("offset: %d, minimum offset %d for option data", offset,
                    DHCP_CLIENT_MAX_OPTION_HEADER_LEN);
    ASSERT(0);
    return -1;
  }

  /* check bounds */
  if ( offset - DHCP_CLIENT_MAX_OPTION_HEADER_LEN + len <= opt->len )
  {
    memscpy(dst_buf,
            dst_buf_size,
            &((uint8*)opt->option_txt)[offset - DHCP_CLIENT_MAX_OPTION_HEADER_LEN],
            len);
    retval = 0;
  }
  else
  {
    retval = -1;
  }
  return retval;
} /* dhcpi_client_get_option_data_bytes */

/*-------------------------------------------------------------------------
                           SEND MESSAGE FUNCTIONS
---------------------------------------------------------------------------*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_MESSAGE

Description:
  This function formats and sends a message out.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
  The message must be "filled" out.
===========================================================================*/
static void
dhcpi_client_send_message
(
  dhcp_client_core_info * info
)
{
  void * msg;
  boolean pass;
  dhcp_client_meta_option * opt;
  uint8 end_option = (uint8)DHCP_CLIENT_OPTION_END;
  uint8 hw_type = (uint8)info->init.core_params.hw_type;
  dhcp_client_event_payload_type event_payload;
  uint32 server_ip_addr;
  uint32 size;
  uint8 * message_pad;

  info->size++; /* For the end option */

  /* To work around borken DHCP servers */
  size = MAX(info->size, DHCP_CLIENT_MIN_MESSAGE_LEN);

  pass = info->config.client_request_new( info->config.client_request_handle,
                                          &msg, size );

  /* Generate the bootp part of the message */
  /* Order and sizes here are important.  See RFC2131 */
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.op);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, hw_type);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info,
                                  info->init.core_params.hw_addr_len);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.hops);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->init.xid);
  DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, info->core.secs);
  DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, info->core.flags);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.ciaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.yiaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.siaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.giaddr);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->hwaddr,
                                  DHCP_CLIENT_CHADDR_LEN);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->core.sname,
                                  DHCP_CLIENT_SNAME_LEN);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->core.file,
                                  DHCP_CLIENT_FILE_LEN);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.magic);

  /* Output the options */
  opt = info->core.opt;
  while ( NULL != opt )
  {
    DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, opt->option);
    if ( DHCP_CLIENT_OPTION_PAD != opt->option &&
         DHCP_CLIENT_OPTION_END != opt->option )
    {
      DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, opt->len);
      if ( 0 != opt->len )
      {
        DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info,
                                        opt->option_txt, opt->len);
      }
    }
    opt = opt->next;
  }

  /* Output the end of options option */
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, end_option);

  if ( info->size < size )
  {
    /* Pad out to MIN_MESSAGE_SIZE for borken DHCP servers that
     * require a minimum message length.
     */
    size -= info->size;
    message_pad = info->init.d_malloc( size );
    memset( message_pad, 0, size );
    DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, message_pad, size );
    info->init.d_free( message_pad );
  }

  if ( TRUE == pass )
  {
    server_ip_addr = (TRUE == info->send_broadcast) ?
      info->init.core_params.server_addr : info->lease.server_id;

    info->config.client_request_send( info->config.client_request_handle,
                                      &msg,
                                      server_ip_addr );

    event_payload.server_ip_addr = server_ip_addr;
    event_payload.client_ip_addr = info->core.ciaddr;
    event_payload.state = (uint8) info->state;
    event_payload.event_type = (uint8) info->diag_event_type;
    event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
  }
  else
  {
    info->config.client_request_free( info->config.client_request_handle,
                                      &msg);
  }
} /* dhcpi_client_send_message */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_FILL_CLIENT_ID_OPTION

Description:
  This function appends the DHCP client indentifier(61) option to the list
  of options.

Arguements:
  dhcp_client_core_info * - The current client handle.
  dhcp_client_meta_option ** - pointer to the previous option pointer

Return value:
  None.

Dependencies:
  The core must be started.
  There must be at least one option present before the client_id option.
===========================================================================*/
static void 
dhcpi_client_fill_client_id_option
(
  dhcp_client_core_info *info,
  dhcp_client_meta_option **last_opt_ptr
)
{
  dhcp_client_meta_option *last_opt;
  if (info == NULL)
  {
    DATA_ERR_FATAL("Received NULL info");
    return;
  }
  if (last_opt_ptr == NULL)
  {
    DATA_ERR_FATAL( "Received NULL last_opt_ptr");
    return;
  }

  last_opt = *last_opt_ptr;
  if (last_opt == NULL)
  {
    DATA_ERR_FATAL("Received NULL last_opt");
    return;
  }

  switch (info->init.core_params.client_id_type)
  {
    case DHCP_CLIENT_ID_OPTTYPE_HWADDR:
      /* Verify that the hardware address length is within bounds. */
      if ((info->init.core_params.hw_addr_len <
             DHCP_CLIENT_CLIENT_ID_MIN_LEN) ||
          (info->init.core_params.hw_addr_len >
             DHCP_CLIENT_CHADDR_LEN))
      {
        LOG_MSG_ERROR_2("Invalid hw_addr found - with type:%d, len:%d",
                        info->init.core_params.hw_type,
                        info->init.core_params.hw_addr_len);
      }
      else
      {
        last_opt = last_opt->next = 
          info->init.d_malloc( sizeof(dhcp_client_meta_option) +
                               DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN +
                               info->init.core_params.hw_addr_len );
        ASSERT(NULL != last_opt);
        last_opt->option = DHCP_CLIENT_OPTION_CLIENT_ID;
        last_opt->len = DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN +
                          info->init.core_params.hw_addr_len;
        last_opt->option_txt = (uint8 *)&last_opt[1];
        last_opt->option_txt[0] = (uint8)info->init.core_params.hw_type;
        memscpy ( &(last_opt->option_txt[1]),last_opt->len-1,
                 info->hwaddr,
                 info->init.core_params.hw_addr_len );
        info->size += last_opt->len +
                        DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
      }
      break;
	  
    case DHCP_CLIENT_ID_OPTTYPE_CUSTOM:
      if ((info->init.core_params.client_id_len <
             DHCP_CLIENT_CLIENT_ID_MIN_LEN) ||
          (info->init.core_params.client_id_len>
             DHCP_CLIENT_CLIENT_ID_MAX_LEN))
      {
        LOG_MSG_ERROR_1("Invalid client_id with len=%d found",
                        info->init.core_params.client_id_len);
      }
      else
      {
        last_opt = last_opt->next = 
          info->init.d_malloc( sizeof(dhcp_client_meta_option) +
                               DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN +
                               info->init.core_params.client_id_len );
        ASSERT(NULL != last_opt);
        last_opt->option = DHCP_CLIENT_OPTION_CLIENT_ID;
        last_opt->len = DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN +
                          info->init.core_params.client_id_len;
        last_opt->option_txt = (uint8 *)&last_opt[1];
        last_opt->option_txt[0] = (uint8)DHCP_CLIENT_CLIENT_ID_TYPE_CUSTOM;
        memscpy ( &(last_opt->option_txt[1]),
                 last_opt->len-1,
                 (uint8 *)info->init.core_params.client_id,
                 info->init.core_params.client_id_len );
        info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
      }
      break;

    default:
      break;
  }
  *last_opt_ptr = last_opt;
}

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_REQUEST

Description:
  This function sends a DHCPREQUEST message.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
===========================================================================*/
static void
dhcpi_client_send_request
(
  dhcp_client_core_info * info
)
{
  dhcp_client_meta_option * last_opt;
  uint32 optlen = 0;

  LOG_MSG_INFO2_0("Sending DHCPREQUEST message");
  info->diag_event_type = DHCP_CLIENT_EVENT_REQUEST;

  /* Initialize the core */
  ASSERT( NULL == info->core.opt );

  /* Before NULLing core, free any unfreed options memory */
  dhcpi_client_free_core_options_mem(info);
  
  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* Many of the following values are per RFC 2131, table 5 */
  info->core.op = DHCP_CLIENT_BOOTP_OP_BOOTREQUEST;
  info->core.hops = 0;
  info->core.secs = 0;
  info->core.flags = 0; /* In theory, we don't require a broadcast reply */
  info->core.ciaddr = info->request_ciaddr;
  info->core.yiaddr = 0;
  info->core.siaddr = 0;
  info->core.giaddr = 0;
  info->core.magic = DHCP_MAGIC_NUM;

  info->size = DHCP_CLIENT_BOOTP_SIZE;

  last_opt = info->core.opt = info->init.d_malloc(sizeof(dhcp_client_meta_option) + 1);
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_MSG_TYPE;
  last_opt->len = 1; /* Per RFC 2132, 9.3 */
  last_opt->option_txt = (uint8 *)&last_opt[1];
  last_opt->option_txt[0] = (uint8) DHCP_CLIENT_DHCPREQUEST;

  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  /* If we have a valid client_id set,
   * add the client_id option to the message */
  if ( (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
       (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_CUSTOM) )
  {
    dhcpi_client_fill_client_id_option ( info, &last_opt );
  }

  if ( info->request_ip_addr_option )
  {
    last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
    ASSERT(NULL != last_opt);
    last_opt->option = DHCP_CLIENT_OPTION_REQUESTED_IP;
    last_opt->len = 4; /* Per RFC 2132, 9.1 */
    last_opt->option_txt = (uint8 *) &info->lease.lease_ip;
    info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  if ( info->request_server_id_option )
  {
    last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
    ASSERT(NULL != last_opt);
    last_opt->option = DHCP_CLIENT_OPTION_SERVER_ID;
    last_opt->len = 4; /* Per RFC 2132, 9.7 */
    last_opt->option_txt = (uint8 *) &info->lease.server_id;
    info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  last_opt->next = NULL;

  /* Add Parameter Request List Option */
  if ( TRUE == dhcpi_client_init_parameter_request_list_option(info, &optlen, TRUE) )
  {
    info->size += optlen + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  dhcpi_client_send_message( info );

  last_opt = info->core.opt;
  while ( NULL != info->core.opt )
  {
    last_opt = info->core.opt;
    info->core.opt = last_opt->next;
    info->init.d_free( last_opt );
  }
} /* dhcpi_client_send_request */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_RELEASE

Description:
  This function sends a DHCPRELEASE message.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
===========================================================================*/
static void
dhcpi_client_send_release
(
  dhcp_client_core_info * info
)
{
  dhcp_client_meta_option * last_opt;

  LOG_MSG_INFO2_0("Sending DHCPRELEASE message");
  info->diag_event_type = DHCP_CLIENT_EVENT_RELEASE;

  /* Initialize the core */
  ASSERT( NULL == info->core.opt );

  /* Before NULLing core, free any unfreed options memory */
  dhcpi_client_free_core_options_mem(info);

  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* From RFC 2131, 4.4.4, the release always is unicast */
  info->send_broadcast = FALSE;

  /* Many of the following values are per RFC 2131, table 5 */
  info->core.op = DHCP_CLIENT_BOOTP_OP_BOOTREQUEST;
  /* The following fields have been zeroed out by the memset above:
   * hops secs flags yiaddr siaddr giaddr
   */
  info->core.ciaddr = info->lease.lease_ip;
  info->core.magic = DHCP_MAGIC_NUM;

  info->size = DHCP_CLIENT_BOOTP_SIZE;

  last_opt = info->core.opt = info->init.d_malloc(sizeof(dhcp_client_meta_option) + 1);
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_MSG_TYPE;
  last_opt->len = 1; /* Per RFC 2132, 9.3 */
  last_opt->option_txt = (uint8 *) &last_opt[1];
  last_opt->option_txt[0] = (uint8) DHCP_CLIENT_DHCPRELEASE;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  /* If we have a valid client_id set,
   * add the client_id option to the message */
  if ( (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
       (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_CUSTOM) )
  {
    dhcpi_client_fill_client_id_option ( info, &last_opt );
  }

  last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_SERVER_ID;
  last_opt->len = 4; /* Per RFC 2132, 9.7 */
  last_opt->option_txt = (uint8 *) &info->lease.server_id;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  last_opt->next = NULL;

  dhcpi_client_send_message( info );

  last_opt = info->core.opt;
  while ( NULL != info->core.opt )
  {
    last_opt = info->core.opt;
    info->core.opt = last_opt->next;
    info->init.d_free( last_opt );
  }
} /* dhcpi_client_send_release */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_DISCOVER

Description:
  This function sends a DHCPDISCOVER message.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
===========================================================================*/
static void
dhcpi_client_send_discover
(
  dhcp_client_core_info * info
)
{
  dhcp_client_meta_option * last_opt;
  uint32 optlen = 0;

  LOG_MSG_INFO2_0("Sending DHCPDISCOVER message");
  info->diag_event_type = DHCP_CLIENT_EVENT_DISCOVER;

  /* Initialize the core */
  ASSERT( NULL == info->core.opt );

  /* Before NULLing core, free any unfreed options memory */
  dhcpi_client_free_core_options_mem(info);

  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* From RFC 2131, 4.4.4, by definition, the DISCOVER doesn't know
   * the server address.
   */
  info->send_broadcast = TRUE;

  /* Many of the following values are per RFC 2131, table 5 */
  info->core.op = DHCP_CLIENT_BOOTP_OP_BOOTREQUEST;
  /* The following fields have been zeroed out by the memset above:
   * hops secs flags ciaddr yiaddr siaddr giaddr
   */
  info->core.magic = DHCP_MAGIC_NUM;
  info->size = DHCP_CLIENT_BOOTP_SIZE;

  last_opt = info->core.opt = info->init.d_malloc(sizeof(dhcp_client_meta_option)+1);
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_MSG_TYPE;
  last_opt->len = 1; /* Per RFC 2132, 9.3 */
  last_opt->option_txt = (uint8 *) &last_opt[1];
  last_opt->option_txt[0] = (uint8) DHCP_CLIENT_DHCPDISCOVER;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  /* If we have a valid client_id set,
   * add the client_id option to the message */
  if ( (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
       (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_CUSTOM) )
  {
    dhcpi_client_fill_client_id_option ( info, &last_opt );
  }

  /* If there is a previous IP address, request it */
  if ( 0 != info->lease.lease_ip )
  {
    last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
    ASSERT(NULL != last_opt);
    last_opt->option = DHCP_CLIENT_OPTION_REQUESTED_IP;
    last_opt->len = 4; /* Per RFC 2132, 9.1 */
    last_opt->option_txt = (uint8 *) &info->lease.lease_ip;
    info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  /* Add Rapid Commit Option */
  if ( TRUE == info->init.core_params.rapid_commit_option )
  {
    /* Allocate space for meta option struct and init */
    last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
    ASSERT(NULL != last_opt);
    last_opt->option = DHCP_CLIENT_OPTION_RAPID_COMMIT;
    last_opt->len = 0; /* as per RFC 4039 */
    info->size += DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  last_opt->next = NULL;

  /* Add Parameter Request List Option */
  if ( TRUE == dhcpi_client_init_parameter_request_list_option(info, &optlen, TRUE) )
  {
    info->size += optlen + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  dhcpi_client_send_message( info );

  last_opt = info->core.opt;
  while ( NULL != info->core.opt )
  {
    last_opt = info->core.opt;
    info->core.opt = last_opt->next;
    info->init.d_free( last_opt );
  }
} /* dhcpi_client_send_discover */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_DECLINE

Description:
  This function sends a DHCPDECLINE message.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
===========================================================================*/
static void
dhcpi_client_send_decline
(
  dhcp_client_core_info * info
)
{
  dhcp_client_meta_option * last_opt;

  LOG_MSG_INFO2_0("Sending DHCPDECLINE message");
  info->diag_event_type = DHCP_CLIENT_EVENT_DECLINE;

  /* Initialize the core */
  ASSERT( NULL == info->core.opt );

  /* Before NULLing core, free any unfreed options memory */
  dhcpi_client_free_core_options_mem(info);

  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* From RFC 2131, 4.4.4, the decline message always is broadcast. */
  info->send_broadcast = TRUE;
  /* Many of the following values are per RFC 2131, table 5 */
  info->core.op = DHCP_CLIENT_BOOTP_OP_BOOTREQUEST;
  /* The following fields have been zeroed out by the memset above:
   * hops secs flags ciaddr yiaddr siaddr giaddr
   */
  info->core.magic = DHCP_MAGIC_NUM;
  info->size = DHCP_CLIENT_BOOTP_SIZE;

  last_opt = info->core.opt = info->init.d_malloc(sizeof(dhcp_client_meta_option));
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_REQUESTED_IP;
  last_opt->len = 4; /* Per RFC 2132, 9.1 */
  last_opt->option_txt = (uint8 *) &info->lease.lease_ip;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option) + 1);
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_MSG_TYPE;
  last_opt->len = 1; /* Per RFC 2132, 9.3 */
  last_opt->option_txt = (uint8 *) &last_opt[1];
  last_opt->option_txt[0] = (uint8) DHCP_CLIENT_DHCPDECLINE;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  /* If we have a valid client_id set,
   * add the client_id option to the message */
  if ( (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
       (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_CUSTOM) )
  {
    dhcpi_client_fill_client_id_option ( info, &last_opt );
  }

  last_opt = last_opt->next = info->init.d_malloc(sizeof(dhcp_client_meta_option));
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_SERVER_ID;
  last_opt->len = 4; /* Per RFC 2132, 9.7 */
  last_opt->option_txt = (uint8 *) &info->lease.server_id;
  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  last_opt->next = NULL;

  dhcpi_client_send_message( info );

  last_opt = info->core.opt;
  while ( NULL != info->core.opt )
  {
    last_opt = info->core.opt;
    info->core.opt = last_opt->next;
    info->init.d_free( last_opt );
  }
} /* dhcpi_client_send_decline */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_SEND_INFORM

Description:
  This function sends a DHCPINFORM message.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  The core must be started.
===========================================================================*/
static void
dhcpi_client_send_inform
(
  dhcp_client_core_info * info
)
{
  dhcp_client_meta_option * last_opt;
  uint32 optlen = 0;

  LOG_MSG_INFO2_0("Sending DHCPINFORM message");
  info->diag_event_type = DHCP_CLIENT_EVENT_INFORM;

  /* Initialize the core */
  ASSERT( NULL == info->core.opt );

  /* Before NULLing core, free any unfreed options memory */
  dhcpi_client_free_core_options_mem(info);

  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* Many of the following values are per RFC 2131, table 5 */
  info->core.op = DHCP_CLIENT_BOOTP_OP_BOOTREQUEST;
  info->core.hops = 0;
  info->core.secs = 0;
  info->core.flags = 0; /* In theory, we don't require a broadcast reply */
  info->core.ciaddr = info->inform_addr;
  info->core.yiaddr = 0;
  info->core.siaddr = 0;
  info->core.giaddr = 0;
  info->core.magic = DHCP_MAGIC_NUM;

  info->size = DHCP_CLIENT_BOOTP_SIZE;

  last_opt = info->core.opt = info->init.d_malloc(sizeof(dhcp_client_meta_option) + 1);
  ASSERT(NULL != last_opt);
  last_opt->option = DHCP_CLIENT_OPTION_MSG_TYPE;
  last_opt->len = 1; /* Per RFC 2132, 9.3 */
  last_opt->option_txt = (uint8 *)&last_opt[1];
  last_opt->option_txt[0] = (uint8) DHCP_CLIENT_DHCPINFORM;

  info->size += last_opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

  /* If we have a valid client_id set,
   * add the client_id option to the message */
  if ( (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
       (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_CUSTOM) )
  {
    dhcpi_client_fill_client_id_option ( info, &last_opt );
  }

  last_opt->next = NULL;

  /* Add Parameter Request List Option */
  /* RFC 2131, section 4.4.3 specifies that we should not request lease time option */
  if ( TRUE == dhcpi_client_init_parameter_request_list_option(info, &optlen, FALSE) )
  {
    info->size += optlen + DHCP_CLIENT_MAX_OPTION_HEADER_LEN;
  }

  dhcpi_client_send_message( info );

  last_opt = info->core.opt;
  while ( NULL != info->core.opt )
  {
    last_opt = info->core.opt;
    info->core.opt = last_opt->next;
    info->init.d_free( last_opt );
  }
} /* dhcpi_client_send_inform */

/*-------------------------------------------------------------------------
                           RECEIVE MESSAGE FUNCTIONS
---------------------------------------------------------------------------*/
/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_OPTION_ALLOC_LINK()

DESCRIPTION
  This function allocates an option meta pointer thingy, initializes
  it and inserts it into the gather options list.

DEPENDENCIES
  parameters correct

PARAMETERS
  dhcp_client_core_info * - The handle associated with the current handle.
  dhcp_client_option_enum - The option number that we are handling.
  uint32 len - The extra length for the option text that we should
               allocate, or 0 if we shouldn't allocate anything extra
               for this option.
RETURN VALUE
  dhcp_client_option_ret_enum to indicate success/fail.

SIDE EFFECTS
  Adds the option to the info->gather options list.
===========================================================================*/
static dhcp_client_option_ret_enum
dhcpi_client_option_alloc_link
(
  dhcp_client_core_info   * info,
  dhcp_client_option_enum   option_num,
  uint32             len   /* of alloc and option */
)
{
  dhcp_client_meta_option * opt;
  dhcp_client_option_ret_enum ret = DHCP_CLIENT_OPTION_RET_SUCCESS;

  if (NULL == info)
  {
    DATA_ERR_FATAL("NULL info");
    return DHCP_CLIENT_OPTION_RET_FAIL;
  }

  opt = info->init.d_malloc(sizeof(dhcp_client_meta_option) + len);
  if ( NULL == opt )
  {
    LOG_MSG_ERROR_0("Insufficent memory to alloc option link");
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  else
  {
    /* Caution, pointer magic ahead */
    opt->option_txt = ( 0 != len ) ? (byte*)&opt[1] : NULL;
    opt->option = option_num;
    opt->len = len;
    opt->next = info->core.opt;
    info->core.opt = opt;
  }
  return ret;
} /* dhcpi_client_option_alloc_link */

/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_OPTION_UNLINK()

DESCRIPTION
  This function unlinks the first specified option from the
  core.opt list and returns it.

DEPENDENCIES
  Input parameters correct.

PARAMETERS
  dhcp_client_core_info * - The handle to associated with the message.
  dhcp_client_option_enum - The option to look for.

RETURN VALUE
  The meta option for the requested option if found, or NULL is not.

SIDE EFFECTS
  Removes the specified option from the core.opt list.
===========================================================================*/
static dhcp_client_meta_option *
dhcpi_client_option_unlink
(
  dhcp_client_core_info * info,
  dhcp_client_option_enum option
)
{
  dhcp_client_meta_option ** optptr;
  dhcp_client_meta_option *  optret = NULL;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return NULL;
  }
  optptr = &info->core.opt;

  while ( NULL != *optptr )
  {
    if ( (*optptr)->option == option )
    {
      optret = *optptr;
      *optptr = optret->next;
      optret->next = NULL;
      break;
    }
    optptr = &(*optptr)->next;
  }

  return optret;
} /* dhcpi_client_option_unlink */

/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_LINK_OPTION()

DESCRIPTION
  This function links the specified option to the core.opt list.

DEPENDENCIES
  Input parameters correct.

PARAMETERS
  dhcp_client_core_info * - The handle associated with the message.
  dhcp_client_meta_option - The option to link.

RETURN VALUE
  None

SIDE EFFECTS
  Adds the option to the core.opt list.
===========================================================================*/
static void
dhcpi_client_link_option
(
  dhcp_client_core_info * info,
  dhcp_client_meta_option * option
)
{

  if ( NULL == info || NULL == option )
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }

  /* Link the option */
  option->next = info->core.opt;
  info->core.opt = option;

  return;
} /* dhcpi_client_link_option */


/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_OPT_PARSE_OVERLOAD_HANDLER()

DESCRIPTION
  This function handles the overload option.  From RFC-2132:

  9.3. Option Overload

     This option is used to indicate that the DHCP 'sname' or 'file'
     fields are being overloaded by using them to carry DHCP options. A
     DHCP server inserts this option if the returned parameters will
     exceed the usual space allotted for options.

     If this option is present, the client interprets the specified
     additional fields after it concludes interpretation of the standard
     option fields.

     The code for this option is 52, and its length is 1.  Legal values
     for this option are:

             Value   Meaning
             -----   --------
               1     the 'file' field is used to hold options
               2     the 'sname' field is used to hold options
               3     both fields are used to hold options

      Code   Len  Value
     +-----+-----+-----+
     |  52 |  1  |1/2/3|
     +-----+-----+-----+

DEPENDENCIES
  None.

PARAMETERS
  dhcp_client_core_info * info - Handle associated with current message.
  uint8 optlen - The reported length of the option.

RETURN VALUE
  dhcp_client_option_ret_enum - Indicates success/failure.

SIDE EFFECTS
  dhcp_client_core_infor * gets updated with overload info.
===========================================================================*/
static dhcp_client_option_ret_enum
dhcpi_client_opt_parse_overload_handler
(
  dhcp_client_core_info * info,
  uint8 optlen
)
{
  dhcp_client_option_ret_enum ret = DHCP_CLIENT_OPTION_RET_SUCCESS;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return DHCP_CLIENT_OPTION_RET_FAIL;
  }

  LOG_MSG_INFO3_0("INFO: OVERLOAD option");
  if ( 1 != optlen )
  {
    LOG_MSG_INFO2_0("FAIL: Corrupt option len");
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if ( 0 != info->overload )
  {
    LOG_MSG_INFO2_0("FAIL: multiple overload options?");
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if( ! info->extract( &info->overload, info->msg, info->offset, 1 ) )
  {
    LOG_MSG_INFO2_0("FAIL: sort read in overload option");
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if ( DHCP_CLIENT_OPTION_OVERLOAD_FILE > info->overload ||
       DHCP_CLIENT_OPTION_OVERLOAD_FILE_SNAME < info->overload )
  {
    LOG_MSG_INFO2_1("FAIL: overload value out of range %d",info->overload);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }

  return ret;
} /* dhcpi_client_opt_parse_overload_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_OPTION_PARSE()

DESCRIPTION
  Parse through the options that appear in a field in a recieved
  message.  This function will extract the info from each option and
  add it to a new meta_option and add this to the list of options
  found.

DEPENDENCIES
  Info valid.

PARAMETERS
  dhcp_client_core_info * - handle to instance message recieved on.
  int32 - Size of fixed length in option field, or 0 for unbound
          option field.

RETURN VALUE
  dhcp_client_option_ret_enum - state of message parsing, indicating done or
                         failure.

SIDE EFFECTS
  Offset field of info changed.
===========================================================================*/
static dhcp_client_option_ret_enum
dhcpi_client_option_parse
(
  dhcp_client_core_info * info,
  int32 field_size
)
{
  dhcp_client_option_ret_enum ret = DHCP_CLIENT_OPTION_RET_SUCCESS;
  dhcp_client_option_enum     option;
  uint32               optlen = 0;
  dhcp_client_meta_option   * opt;
  boolean              bound = field_size?TRUE:FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return DHCP_CLIENT_OPTION_RET_FAIL;
  }

  while ( DHCP_CLIENT_OPTION_RET_SUCCESS == ret )
  {
    option = DHCP_CLIENT_OPTION_PAD; /* Zero out any high bits */

    /* Get bytes and verify that we succeeded */
    if ( !(
           (! bound || 1 <= field_size) &&
           info->extract( &option, info->msg, info->offset++, 1)
          )
       )
    {
      ret = DHCP_CLIENT_OPTION_RET_FAIL;
      LOG_MSG_INFO2_0("FAIL: End of options without END option");
      break;
    }
    if ( DHCP_CLIENT_OPTION_END != option && DHCP_CLIENT_OPTION_PAD != option )
    {
      optlen = 0;
      if ( !( ( ! bound ||
                DHCP_CLIENT_MAX_OPTION_HEADER_LEN <= field_size ) &&
           info->extract( &optlen, info->msg, info->offset++, 1) ) )
      {
        ret = DHCP_CLIENT_OPTION_RET_FAIL;
        LOG_MSG_INFO2_0("FAIL: End of options midoption without END option");
        break;
      }
      field_size -= DHCP_CLIENT_MAX_OPTION_HEADER_LEN + (int32)optlen;
      LOG_MSG_INFO3_2("INFO: Parse option %d len %lu", option, optlen);
    }
    else
    {
      field_size--;
      if( DHCP_CLIENT_OPTION_END == option )
      {
        LOG_MSG_INFO3_0( "INFO: Parse END");
        ret = DHCP_CLIENT_OPTION_RET_DONE;
        /* We are done with this option field */
        break;
      }
      LOG_MSG_INFO3_0( "INFO: Parse PAD");
      /* nothing more to process with a PAD */
      continue;
    }
    if ( bound && 0 > field_size )
    {
      LOG_MSG_INFO2_0( "FAIL: End of message mid-option");
      ret = DHCP_CLIENT_OPTION_RET_FAIL;
      break;
    }
    if ( DHCP_CLIENT_OPTION_OVERLOAD == option )
    {
      ret = dhcpi_client_opt_parse_overload_handler(info, (uint8)optlen);
    }
    else
    {
      opt = dhcpi_client_option_unlink(info, option);
      if ( NULL == opt )
      {
        /* Common case */
        ret = dhcpi_client_option_alloc_link(info, option, optlen);
        if (NULL != info->core.opt->option_txt)
        {
          if ( TRUE != info->extract(info->core.opt->option_txt, info->msg,
                              info->offset, optlen) )
          {
            ret = DHCP_CLIENT_OPTION_RET_FAIL;
            LOG_MSG_INFO2_0( "FAIL: End of message mid-option");
            break;
          }
        }/* ELSE: For options like RAPID COMMIT which have 
          * 0 optlen and NULL option_txt, dont do any extracts  */
      }
      else
      {
        if ( IS_SPANNING_OPTION(option) )
        {
          /* Spanning options */
          LOG_MSG_INFO3_1( "INFO: Found spanning opt %d", option);
          ret = dhcpi_client_option_alloc_link(info, 
                                               option, optlen + opt->len);
          if ( DHCP_CLIENT_OPTION_RET_SUCCESS == ret )
          {
            memscpy(info->core.opt->option_txt,
                    optlen + opt->len, 
                    opt->option_txt, 
                    opt->len);
            if ( TRUE != info->extract(&info->core.opt->option_txt[opt->len],
                         info->msg, info->offset, optlen) )
            {
              ret = DHCP_CLIENT_OPTION_RET_FAIL;
              LOG_MSG_INFO2_0( "FAIL: End of message mid-option");
              break;
            }
          }
          info->init.d_free( opt );
        }
        else
        {
          /* Not a Spanning option just ignore it and
           * link the first option back. 
           */
          dhcpi_client_link_option(info, opt);
        }
      }
    }
    info->offset += optlen;
  }
  return ret;
} /* dhcpi_client_option_parse */

/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_OPT_PARSE_BOOTP_HANDLER()

DESCRIPTION
  This function extracts the bootp header information from the message
  and puts it into the core handle.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_client_core_infor * - The handle associated with the current messaage.

RETURN VALUE
  dhcp_client_option_ret_enum - indicated success or failure.

SIDE EFFECTS
  core handle becomes populated with bootp infomation.
===========================================================================*/
static dhcp_client_option_ret_enum
dhcpi_client_opt_parse_bootp_handler
(
  dhcp_client_core_info * info
)
{
  dhcp_client_option_ret_enum ret = DHCP_CLIENT_OPTION_RET_SUCCESS;
  boolean pass = TRUE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return DHCP_CLIENT_OPTION_RET_FAIL;
  }

  LOG_MSG_INFO3_0( "INFO: Parsing bootp 'option'");

  /* Order and sizes here is important.  See RFC2131 */
  DHCP_COPY_INPUT_OPTION_1_BYTES(pass, info, op);
  DHCP_COPY_INPUT_OPTION_1_BYTES(pass, info, htype);
  DHCP_COPY_INPUT_OPTION_1_BYTES(pass, info, hlen);
  DHCP_COPY_INPUT_OPTION_1_BYTES(pass, info, hops);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, xid);
  DHCP_COPY_INPUT_OPTION_2_BYTES(pass, info, secs);
  DHCP_COPY_INPUT_OPTION_2_BYTES(pass, info, flags);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, ciaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, yiaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, siaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, giaddr);
  DHCP_COPY_INPUT_OPTION_N_BYTES(pass, info, chaddr, DHCP_CLIENT_CHADDR_LEN);
  DHCP_COPY_INPUT_OPTION_N_BYTES(pass, info, sname, DHCP_CLIENT_SNAME_LEN);
  DHCP_COPY_INPUT_OPTION_N_BYTES(pass, info, file, DHCP_CLIENT_FILE_LEN);
  DHCP_COPY_INPUT_OPTION_4_BYTES(pass, info, magic);
  
  if ( TRUE != pass )
  {
    LOG_MSG_INFO2_0( "FAIL: Failure copying bootp header");
    return DHCP_CLIENT_OPTION_RET_FAIL;
  }
  /* Validate what we can */
  if ( DHCP_CLIENT_BOOTP_OP_BOOTREPLY != info->core.op )
  {
    LOG_MSG_INFO2_1("FAIL: Invalid op field 0x%x", info->core.op);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if ( info->core.flags & DHCP_CLIENT_BOOTP_FLAG_MBZ_MASK )
  {
    LOG_MSG_INFO2_1( "FAIL: Non-zero MBZ in flags field 0x%x",info->core.flags);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if ( DHCP_MAGIC_NUM != info->core.magic )
  {
    LOG_MSG_INFO2_1( "FAIL: bad magic number 0x%lx", info->core.magic);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  /* Check if it is a reply for us */
  if ( info->core.xid != info->init.xid )
  {
    LOG_MSG_INFO2_2("FAIL: Non-matching xids 0x%lx, 0x%lx",
                    info->core.xid, info->init.xid);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }

  /* Special case ONLY for SLIP: The SLIP stack can accept an IP address 
     that has type ethernet and it will therefore not be a 1:1 match with
     the hw type serial line.*/
  if ( info->init.core_params.hw_type == DHCP_CLIENT_HWTYPE_SERIAL_LINE)
  {
    if (info->core.htype != (uint8)DHCP_CLIENT_HWTYPE_ETHERNET)
    {
      LOG_MSG_INFO2_2("FAIL: Non-Ethernet hw type 0x%x not supported, Expected: 0x%x", 
               info->core.htype, info->init.core_params.hw_type);
      ret = DHCP_CLIENT_OPTION_RET_FAIL;
    }
  } 
  else
  {
    if ( info->core.htype != (uint8)info->init.core_params.hw_type )
    {
      LOG_MSG_INFO2_2("FAIL: Non-matching hardware types 0x%x, 0x%x",
                      info->core.htype, info->init.core_params.hw_type);
      ret = DHCP_CLIENT_OPTION_RET_FAIL;
    }
  }
  
  if ( info->core.hlen != info->init.core_params.hw_addr_len )
  {
    LOG_MSG_INFO2_2( "FAIL: Non-matching hardware lengths 0x%lx, 0x%lx",
             info->core.hlen, info->init.core_params.hw_addr_len);
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }
  if ( memcmp( info->core.chaddr, info->hwaddr,
               MIN(info->init.core_params.hw_addr_len, info->core.hlen) ) )
  {
    LOG_MSG_INFO2_0( "FAIL: Non-matching hardware addrs");
    ret = DHCP_CLIENT_OPTION_RET_FAIL;
  }

  return ret;
} /* dhcpi_client_opt_parse_bootp_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_CLIENT_PARSE_MSG()

DESCRIPTION
  This function deals with parsing a message. Specifically, it calls
  the approprate parsers (bootp handle parser, option parser) for the
  message.  Also deals with option overloads.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_client_core_info * info - handle associated with the incoming
                            message.

RETURN VALUE
  boolean - True if success, else False.
  dhcp_client_core_info * - info parsed, specifically in info->core.

SIDE EFFECTS
  None
===========================================================================*/
static boolean
dhcpi_client_parse_msg
(
  dhcp_client_core_info * info
)
{
  dhcp_client_option_ret_enum ret;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }

  info->overload = 0;
  info->offset = DHCP_CLIENT_BOOTP_OFFSET;
  ret = dhcpi_client_opt_parse_bootp_handler( info );
  if ( DHCP_CLIENT_OPTION_RET_SUCCESS == ret )
  {
    /* Per RFC 2131 4.1, handle option field, then file, then sname */
    info->offset = DHCP_CLIENT_OPTION_OFFSET;
    ret = dhcpi_client_option_parse( info, 0 );
    if( DHCP_CLIENT_OPTION_RET_DONE == ret &&
        ( DHCP_CLIENT_OPTION_OVERLOAD_FILE == info->overload ||
          DHCP_CLIENT_OPTION_OVERLOAD_FILE_SNAME == info->overload ) )
    {
      info->offset = DHCP_CLIENT_FILE_OFFSET;
      ret = dhcpi_client_option_parse( info, DHCP_CLIENT_FILE_LEN );
    }
    if( DHCP_CLIENT_OPTION_RET_DONE == ret &&
        ( DHCP_CLIENT_OPTION_OVERLOAD_SNAME  == info->overload ||
          DHCP_CLIENT_OPTION_OVERLOAD_FILE_SNAME == info->overload ) )
    {
      info->offset = DHCP_CLIENT_SNAME_OFFSET;
      ret = dhcpi_client_option_parse( info, DHCP_CLIENT_SNAME_LEN );
    }
  }
  return (DHCP_CLIENT_OPTION_RET_FAIL == ret) ? FALSE : TRUE;
} /* dhcpi_client_parse_msg */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_SUBNET_MASK_HANDLER

Description:
  This function parses the current subnet_mask option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the subnet_mask option
===========================================================================*/
static boolean
dhcpi_client_option_subnet_mask_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_SUBNET_MASK != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_SUBNET_MASK %d %d",
                    DHCP_CLIENT_OPTION_SUBNET_MASK, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 3.3 */
  if ( DHCP_CLIENT_OPTION_IP_LEN == info->core.opt->len )
  {
    memscpy( &info->core.netmask,
            sizeof(info->core.netmask),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_IP_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_subnet_mask_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_ROUTER_HANDLER

Description:
  This function parses the current router option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the router option
===========================================================================*/
static boolean
dhcpi_client_option_router_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_ROUTER != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_ROUTER %d %d",
                     DHCP_CLIENT_OPTION_ROUTER, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 3.5 */
  if ( 0 == info->core.opt->len % DHCP_CLIENT_OPTION_IP_LEN &&
       DHCP_CLIENT_OPTION_IP_LEN <= info->core.opt->len )
  {
    /* We only use the first router, which is highest preference via RFC 2132,
     * section 3.5
     */
    memscpy( &info->core.router_addr, 
            sizeof(info->core.router_addr),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_IP_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_router_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_DNS_HANDLER

Description:
  This function parses the current dns option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the dns option
===========================================================================*/
static boolean
dhcpi_client_option_dns_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;
  uint8 len = 0;
  dhcp_client_dns ** dns;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_DNS != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_DNS %d %d",
                     DHCP_CLIENT_OPTION_DNS, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }
  ASSERT(NULL == info->core.dns);

  if ( 0 == info->core.opt->len % DHCP_CLIENT_OPTION_IP_LEN &&
       DHCP_CLIENT_OPTION_IP_LEN <= info->core.opt->len )
  {
    dns = &info->core.dns;
    while ( len < info->core.opt->len )
    {
      if ( NULL == (*dns = info->init.d_malloc( sizeof(dhcp_client_dns) ) ) )
      {
        ret = FALSE;
        break;
      }
      /* Length as specified by RFC 2132, 3.8 */
      memscpy( &(*dns)->dns_addr,
              sizeof((*dns)->dns_addr),
              &info->core.opt->option_txt[len],
              DHCP_CLIENT_OPTION_IP_LEN );
      len += DHCP_CLIENT_OPTION_IP_LEN;
      dns = &(*dns)->next;
      ret = TRUE;
    }
    *dns = NULL;
  }
  return ret;
} /* dhcpi_client_option_dns_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_DOMAIN_NAME_HANDLER

Description:
  This function parses the current domain name option and puts the
  information into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the domain name option
===========================================================================*/
static boolean
dhcpi_client_option_domain_name_handler
(
  dhcp_client_core_info * info
)
{
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_DOMAIN_NAME != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_DOMAIN_NAME %d %d",
                    DHCP_CLIENT_OPTION_DOMAIN_NAME, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  if ( NULL ==
      (info->core.domain_name = info->init.d_malloc(info->core.opt->len)))
  {
    LOG_MSG_ERROR_1("Failed to malloc for domain name of len %d",
                     info->core.opt->len);
    return FALSE;
  }
  memscpy( info->core.domain_name,
          info->core.opt->len,
          info->core.opt->option_txt,
          info->core.opt->len );
  info->core.domain_name_len = info->core.opt->len;

  return TRUE;
} /* dhcpi_client_option_domain_name_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_MTU_HANDLER

Description:
  This function parses the current mtu option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the mtu option
===========================================================================*/
static boolean
dhcpi_client_option_mtu_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_MTU != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_MTU %d %d",
                     DHCP_CLIENT_OPTION_MTU, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 5.1 */
  if ( DHCP_CLIENT_OPTION_MTU_LEN == info->core.opt->len )
  {
    memscpy( &info->core.mtu,
            sizeof(info->core.mtu),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_MTU_LEN );
    if ( DHCP_CLIENT_MTU_MIN <= dss_ntohs(info->core.mtu) )
    {
      ret = TRUE;
    }
  }
  return ret;
} /* dhcpi_client_option_mtu_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_IP_LEASE_HANDLER

Description:
  This function parses the current ip_lease option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the ip_lease option
===========================================================================*/
static boolean
dhcpi_client_option_ip_lease_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_IP_LEASE != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_IP_LEASE %d %d",
                    DHCP_CLIENT_OPTION_IP_LEASE, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 9.2 */
  if ( DHCP_CLIENT_OPTION_TIME_LEN == info->core.opt->len )
  {
    memscpy( &info->core.ip_lease,
            sizeof(info->core.ip_lease),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_TIME_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_ip_lease_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_MSG_TYPE_HANDLER

Description:
  This function parses the current msg_type option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the msg_type option
===========================================================================*/
static boolean
dhcpi_client_option_msg_type_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_MSG_TYPE != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_MSG_TYPE %d %d",
                    DHCP_CLIENT_OPTION_MSG_TYPE, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 9.6 */
  if ( DHCP_CLIENT_OPTION_MSG_TYPE_LEN == info->core.opt->len )
  {
    info->core.dhcp_msg_reply = (dhcp_client_msg_type)info->core.opt->option_txt[0];
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_msg_type_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_SERVER_ID_HANDLER

Description:
  This function parses the current server_id option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the server_id option
===========================================================================*/
static boolean
dhcpi_client_option_server_id_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_SERVER_ID != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_SERVER_ID %d %d",
                    DHCP_CLIENT_OPTION_SERVER_ID, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 9.7 */
  if ( DHCP_CLIENT_OPTION_IP_LEN == info->core.opt->len )
  {
    memscpy( &info->core.server_id,
            sizeof(info->core.server_id),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_IP_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_server_id_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_RENEW_TIME_HANDLER

Description:
  This function parses the current renew_time option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the renew_time option
===========================================================================*/
static boolean
dhcpi_client_option_renew_time_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_RENEW_TIME != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_RENEW_TIME %d %d",
                    DHCP_CLIENT_OPTION_RENEW_TIME, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 9.11 */
  if ( DHCP_CLIENT_OPTION_TIME_LEN == info->core.opt->len )
  {
    memscpy( &info->core.renew_time,
            sizeof(info->core.renew_time),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_TIME_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_renew_time_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_REBIND_TIME_HANDLER

Description:
  This function parses the current rebind_time option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the rebind_time option
===========================================================================*/
static boolean
dhcpi_client_option_rebind_time_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = FALSE;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_REBIND_TIME != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_REBIND_TIME %d %d",
                     DHCP_CLIENT_OPTION_REBIND_TIME, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* Length as specified by RFC 2132, 9.12 */
  if ( DHCP_CLIENT_OPTION_TIME_LEN == info->core.opt->len )
  {
    memscpy( &info->core.rebind_time,
            sizeof(info->core.rebind_time),
            info->core.opt->option_txt,
            DHCP_CLIENT_OPTION_TIME_LEN );
    ret = TRUE;
  }
  return ret;
} /* dhcpi_client_option_rebind_time_handler */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_MESSAGE_HANDLER

Description:
  This function parses the current message option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the message option
===========================================================================*/
static boolean
dhcpi_client_option_message_handler
(
  dhcp_client_core_info * info
)
{
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_MESSAGE != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_MESSAGE %d %d",
                     DHCP_CLIENT_OPTION_MESSAGE, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO1_0("DHCP client got MESSAGE option");
  return TRUE;
} /* dhcpi_client_option_message_handler */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_SIP_HANDLER

Description:
  This function parses the current sip option and puts the information
  into the info structure, as approprate.

Arguements:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successfull, false if problem.

Dependencies:
  The current option must be the SIP option
===========================================================================*/
static boolean
dhcpi_client_option_sip_handler
(
  dhcp_client_core_info * info
)
{
  boolean ret = TRUE;
  dhcp_client_meta_option * opt;

  dhcp_client_sip_addr * tmp_addr;
  dhcp_client_sip_domain_name * tmp_dn;

  int num_addrs;
  int i;
  int expandret = 0;

  unsigned int offset;

  /* error check */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }
  if ( DHCP_CLIENT_OPTION_SIP != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option not DHCP_CLIENT_OPTION_SIP %d %d",
                    DHCP_CLIENT_OPTION_SIP, info->core.opt->option);
    ASSERT(0);
    return FALSE;
  }

  /* sip lists should be NULLed out from last message received. */
  ASSERT(NULL == info->core.sip_addr);
  ASSERT(NULL == info->core.sip_domain_name);

  opt = info->core.opt;

  switch ( opt->option_txt[0] )
  {
    case 0: /* content encoding: Domain names */
      /* domain names start after option header and enc byte in option, and
       * we use offset from the start of the option in order to support
       * RFC 1035 4.1.4 domain name compression  */
      offset = DHCP_CLIENT_SIP_OPTION_DATA_OFFSET +
                          DHCP_CLIENT_MAX_OPTION_HEADER_LEN;

      /* minimum length of DN encoding specified in RFC 3361, 3.1 */
      if ( DHCP_CLIENT_OPTION_SIP_DN_MIN_LEN >
           opt->len - DHCP_CLIENT_SIP_OPTION_DATA_OFFSET )
      {
        ret = FALSE;
      }
      else
      {
        /* parse domain name encoding */
        while( offset != opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN )
        {
          if ( NULL == (tmp_dn =
                     info->init.d_malloc(sizeof(dhcp_client_sip_domain_name)) ) )
          {
            LOG_MSG_ERROR_0("malloc failed");
            ret = FALSE;
            break;
          }

          /* list is appended from front */
          tmp_dn->next = info->core.sip_domain_name;
          info->core.sip_domain_name = tmp_dn;

          /* expandret gets offset of input buffer after reading one
           * domain name */
          expandret = info->init.domain_name_expand_cb(
             opt, offset, (char *) tmp_dn->domain_name,
             DHCP_CLIENT_MAX_DOMAIN_NAME_BUF_LEN, &(tmp_dn->domain_name_len),
             dhcpi_client_get_option_data_bytes,
             IP_VERSION_4
                                );
          if ( -1 == expandret )
          {
            /* parsing domain name failed */
            ret = FALSE;
            break;
          }
          else
          {
            /* set up next loop, expandret should be positive if code
             * falls here. */
            ASSERT( expandret > 0 );
            offset = (unsigned int) expandret;

            ASSERT( offset <= opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN );
          }
        } /* while (offset != opt->len + DHCP_CLIENT_MAX_OPTION_HEADER_LEN) */
      }
      break;
    case 1: /* content encoding: IP addresses */
      /* enc byte offset in option data */
      offset = DHCP_CLIENT_SIP_OPTION_DATA_OFFSET;

      if ( 0 != (opt->len - 1) % DHCP_CLIENT_OPTION_IP_LEN ||
           DHCP_CLIENT_OPTION_IP_LEN + 1 > opt->len )
      {
        ret = FALSE;
      }
      else
      {
        num_addrs = (opt->len - 1) / DHCP_CLIENT_OPTION_IP_LEN;
        for ( i = 0; i < num_addrs; i++ )
        {
          if ( NULL == (tmp_addr =
                        info->init.d_malloc( sizeof(dhcp_client_sip_addr) ) ) )
          {
            LOG_MSG_ERROR_0("malloc failed");
            ret = FALSE;
            break;
          }

          /* list is appended from front */
          tmp_addr->next = info->core.sip_addr;
          info->core.sip_addr = tmp_addr;

          /* Length as specified by RFC 3361, 3.2 */
          memscpy( &tmp_addr->addr,
                  sizeof(tmp_addr->addr),
                  &opt->option_txt[offset],
                  DHCP_CLIENT_OPTION_IP_LEN );
          offset += DHCP_CLIENT_OPTION_IP_LEN;
        } /* for ( i = 0; i < num_addrs; i++ ) */
      }
      break;
    default:
      LOG_MSG_INFO1_1("Unsupported SIP option encoding %d",
                      opt->option_txt[0]);
      break;
  } /* switch ( opt->option_txt[0] ) */
  return ret;
} /* dhcpi_client_option_sip_handler */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_RAPID_COMMIT_HANDLER

Description:
  This function parses the rapid commit option and puts the information
  into the info structure, as appropriate.

Arguments:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successful, False if problem.

Dependencies:
  The current option must be the Rapid Commit option
===========================================================================*/
static boolean
dhcpi_client_option_rapid_commit_handler
(
  dhcp_client_core_info * info
)
{
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }

  info->core.rapid_commit_option = TRUE;

  return TRUE;
} /* dhcpi_client_option_rapid_commit_handler */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_OPTION_CLIENT_ID_HANDLER

Description:
  This function parses the current client_id option and puts the information
  into the info structure, as appropriate.

Arguments:
  dhcp_client_core_info * - the information, including the option, to
                            parse.

Return value:
  Boolean - True if successful, false if problem, or message not for us.
    (message to be ignored on false).

Dependencies:
  The current option must be the client_id option
===========================================================================*/
static boolean
dhcpi_client_option_client_id_handler
(
  dhcp_client_core_info * info
)
{
  
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return FALSE;
  }

  if (NULL == info->core.opt)
  {
    LOG_MSG_ERROR_0("opt NULL?");
    ASSERT(0);
    return FALSE;
  }

  if ( DHCP_CLIENT_OPTION_CLIENT_ID != info->core.opt->option )
  {
    LOG_MSG_ERROR_2("option(%d) not DHCP_CLIENT_OPTION_CLIENT_ID(%d)",
                    info->core.opt->option, DHCP_CLIENT_OPTION_CLIENT_ID);
    ASSERT(0);
    return FALSE;
  }

  if (info->core.opt->option_txt == NULL)
  {
    LOG_MSG_ERROR_0("NULL client_id type and value found");
    return FALSE;
  }

  if ( (info->core.opt->len < DHCP_CLIENT_CLIENT_ID_MIN_LEN +
                                DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN) ||
       (info->core.opt->len > DHCP_CLIENT_CLIENT_ID_MAX_LEN +
                                DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN) )
  {
    LOG_MSG_ERROR_2( "Invalid header length %d found for option %d",
         info->core.opt->len, DHCP_CLIENT_OPTION_CLIENT_ID);
    return FALSE;
  }

  info->core.client_id_type = info->core.opt->option_txt[0];
  info->core.client_id_len = (uint8)info->core.opt->len -
                               DHCP_CLIENT_CLIENT_ID_LEN_FIELD_LEN;
  info->core.client_id =
    info->init.d_malloc(info->core.client_id_len);
  ASSERT(NULL != info->core.client_id);
  memscpy( info->core.client_id,
          info->core.client_id_len,
          &(info->core.opt->option_txt[1]),
          info->core.client_id_len );
  
  return TRUE;
} /* dhcpi_client_option_client_id_handler */

/*-------------------------------------------------------------------------
                           STATE MACHINE FUNCTIONS
---------------------------------------------------------------------------*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_PROCESS_EVENT

Description:
  This function calls the approprate state/event function.

Arguements:
  dhcp_client_core_info * - The current client handle.
  dhcp_client_event_enum - The event that has occurred

Return value:
  None.

Dependencies:
  If info not started, event will be discarded iff not start event.
===========================================================================*/
static void
dhcpi_client_process_event
(
  dhcp_client_core_info       * info,
  dhcp_client_event_enum   event
)
{
  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if ( DHCP_CLIENT_MAX_E <= event ||
       DHCP_CLIENT_MIN_E >= event )
  {
    LOG_MSG_ERROR_1("Event out of range %d", event);
    ASSERT(0);
    return;
  }

  if ( DHCP_CLIENT_MAX_S <= info->state ||
       DHCP_CLIENT_MIN_S >= info->state )
  {
    LOG_MSG_ERROR_1("Client state out of range %d", info->state);
    ASSERT(0);
    return;
  }

  info->event = event;
  if ( NULL != dhcp_client_state_event_fnc[info->state][event] )
  {
    LOG_MSG_INFO2_2("DHCP CLIENT processing %d event in %d state",
                    event, info->state);
    dhcp_client_state_event_fnc[info->state][event](info);
  }
  else
  {
    LOG_MSG_INFO2_2("DHCP CLIENT ignoring %d event in %d state",
                    event, info->state);
  }
  return;
} /* dhcpi_client_process_event */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_TRANSITION_STATE

Description:
  This function transitions to a new state, and calls the approriate
  "we have arrived" state function.

Arguements:
  dhcp_client_core_info * - The current client handle.
  dhcp_client_state_enum - The state we are transitioning to.

Return value:
  None.

Dependencies:
  If info not started, event will be discarded.
===========================================================================*/
static void
dhcpi_client_transition_state
(
  dhcp_client_core_info       * info,
  dhcp_client_state_enum   state
)
{
  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if ( DHCP_CLIENT_MAX_S <= state ||
       DHCP_CLIENT_MIN_S >= state )
  {
    LOG_MSG_ERROR_1("state out of range %d", state);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_2("DHCP_CLIENT state %d -> %d", info->state, state);
  /* If we are transitioning to a different state, clear the
   * timer_count, since that is only meaningful within a single state.
   */
  if ( info->state != state )
  {
    info->timer_count = 0;
  }
  /* Stop the timer */
  info->config.timer_stop( info->config.timer_handle );
  /* Set the new state */
  info->state = state;
  /* Call the state transition function */
  if ( NULL != dhcp_client_state_fnc[info->state] )
  {
    dhcp_client_state_fnc[info->state](info);
  }
} /* dhcpi_client_transition_state */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CLEAR_LEASE

Description:
  This function clears the lease information.  This should be called
  in most cases on transition to the pause state.

Arguements:
  dhcp_client_core_info * - The current client handle.
  dhcp_client_status_enum - The status to pass to the status cb.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_clear_lease
(
  dhcp_client_core_info * info,
  dhcp_client_status_enum status
)
{
  boolean ret;
  dhcp_client_lease lease;
  dhcp_client_event_payload_type event_payload;

  memset( &lease, 0, sizeof(dhcp_client_lease) );
  ret = info->config.storage_get_lease(info->config.storage_handle, &lease);
  if (FALSE == ret ||
      (TRUE == ret &&
       FALSE == lease.valid))
  {
    /* Do not report if lease is not set */
    LOG_MSG_INFO1_0("dhcp client lease not set, not clearing");
    return;
  }

  /* Report that we are freeing the lease; */
  event_payload.client_ip_addr = info->lease.lease_ip;
  event_payload.server_ip_addr = info->lease.server_id;
  event_payload.event_type = (uint8)DHCP_CLIENT_EVENT_LEASE_EXPIRE;
  event_payload.state = (uint8)info->state;
  event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
  /* We should clear the lease and configuration */
  memset( &lease, 0, sizeof(dhcp_client_lease) );
  lease.valid = FALSE;
  info->config.storage_set_lease(info->config.storage_handle, &lease);
  info->config.configuration_clear(info->config.configuration_handle);
  info->init.status_cb( info->init.userdata, status );
} /* dhcpi_client_clear_lease */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_ACK

Description:
  This is the ack event function for all states, except the inform
  state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_ack
(
  dhcp_client_core_info * info
)
{
  dhcp_client_provision provision;

  if ( info->core.yiaddr == info->lease.lease_ip )
  {
    dhcpi_client_transition_state( info, DHCP_CLIENT_VERIFY_S);

    /* Start test for IP address conflicts
     */
    memset( &provision, 0, sizeof(dhcp_client_provision));
    provision.ip_addr = info->core.yiaddr;
    info->config.configuration_test( info->config.configuration_handle,
                                    &provision );
  }
  else
  {
    /* We haven't gotten the same IP address, we are confused, start
     * over
     */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  }
} /* dhcpi_client_ack */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_NAK

Description:
  This is the nak event function for all states without a lease.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_nak
(
  dhcp_client_core_info * info
)
{
  if (info->core.client_id != NULL)
  {
    if (info->init.core_params.client_id_type ==
          DHCP_CLIENT_ID_OPTTYPE_NONE)
    {
      LOG_MSG_ERROR_1("client_id not sent in DHCPREQUEST,\
                      but received(%d) in DHCPNACK",info->core.client_id_type);
      return;
    }

    if (info->core.client_id_type > (uint8)DHCP_CLIENT_HWTYPE_CAI)
    {
      LOG_MSG_ERROR_1( "Invalid client_id_type %d found in DHCPNACK message",
                     info->core.client_id_type);
      return;
    }
    else if (info->core.client_id_type ==
               (uint8)DHCP_CLIENT_CLIENT_ID_TYPE_CUSTOM)
    {
      if ( ( info->core.client_id_len !=
               (uint8)info->init.core_params.client_id_len ) ||
           ( memcmp ( info->core.client_id,
                      info->init.core_params.client_id,
                      info->core.client_id_len ) ) )
      {
        LOG_MSG_ERROR_2("DHCP Client Identifer mismatch sent_len=%d recd_len=%d",
                        info->init.core_params.client_id_len,
                        info->core.client_id_len);
        return;
      }
    }
    else
    {
      if ( (info->init.core_params.client_id_type !=
              DHCP_CLIENT_ID_OPTTYPE_HWADDR) ||
           (info->core.client_id_type != 
                        (uint8)info->init.core_params.hw_type) ||
           (info->core.client_id_len > DHCP_CLIENT_CHADDR_LEN) ||
           (memcmp ( info->core.client_id,
                     info->hwaddr,
                     info->core.client_id_len )) )
      {
        LOG_MSG_ERROR_2("DHCP Client Identifer mismatch sent_hw_type=%d\
                        recd_hw_type=%d",info->init.core_params.hw_type,
                        info->core.client_id_type);
        return;
      }
    }
  }
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_nak */
/*===========================================================================
STATIC FUNCTION DHCPi_CLIENT_NAK_STATUS

Description:
  This is the nak event function for all states with a lease.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_nak_status
(
  dhcp_client_core_info * info
)
{
  if ( NULL != info->init.status_cb )
  {
    info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_EXPIRED );
  }
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_nak_status */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_DECLINE

Description:
  This is the decline event function for r state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_decline
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  dhcpi_client_send_decline( info );
} /* dhcpi_client_decline */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_DECLINE_STATUS

Description:
  This is the decline event function for states that have a lease.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_decline_status
(
  dhcp_client_core_info * info
)
{
  /* Process DECLINE ev before iface could be brought down by
   * user.
   */
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  dhcpi_client_send_decline( info );
  if ( NULL != info->init.status_cb )
  {
    /* Indicate to the application that we have declined an IP allocated by 
     * the server and that we would be trying again.
     */  
    info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_DECLINED );
  }
} /* dhcpi_client_decline_status */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_INFORM

Description:
  This is the inform event function for all states.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_inform
(
  dhcp_client_core_info * info
)
{
  if ( DHCP_CLIENT_INFORM_S == info->state )
  {
    LOG_MSG_INFO1_0("Outstanding inform transaction, DHCPINFORM msg not sent");
  }
  else
  {
    /* reset timer count */
    info->timer_count = 0;
    dhcpi_client_transition_state( info, DHCP_CLIENT_INFORM_S );
  }
} /* dhcpi_client_inform */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_P_RENEW

Description:
  This is the renew event function for the pause state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_p_renew
(
  dhcp_client_core_info * info
)
{
  dhcp_client_lease lease;

  if ( TRUE ==
         info->config.storage_get_lease( info->config.storage_handle,
                                         &lease ) &&
       TRUE == lease.valid )
  {
    info->lease.lease_ip = lease.ip_addr;
    info->lease.server_id = lease.server_id;
    dhcpi_client_transition_state( info, DHCP_CLIENT_REBOOT_S );
  }
  else
  {
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  }
} /* dhcpi_client_p_renew */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_P_RELEASE

Description:
  This is the release event function for pause state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_p_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_p_release */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_R_RENEW

Description:
  This is the renew event function for the rebooting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_r_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_REBOOT_S );
} /* dhcpi_client_r_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_R_RELEASE

Description:
  This is the release event function for rebooting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_r_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_release( info );
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_r_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_R_TIMER

Description:
  This is the timer event function for the rebooting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_r_timer
(
  dhcp_client_core_info * info
)
{
  info->timer_count++;

  if ( info->timer_count < info->init.core_params.reboot_tries )
  {
    /* If not max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_REBOOT_S );
  }
  else
  {
    /* If max count reached */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  }
} /* dhcpi_client_r_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_I_RENEW

Description:
  This is the renew event function for the init state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_i_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_i_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_I_RELEASE

Description:
  This is the release event function for the init state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_i_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_i_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_I_TIMER

Description:
  This is the timer event function for the init state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_i_timer
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_SELECTING_S );
} /* dhcpi_client_i_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_OFFER

Description:
  This is the offer event function for the selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_offer
(
  dhcp_client_core_info * info
)
{
  info->request_ciaddr = 0;
  info->request_ip_addr_option = TRUE;
  info->request_server_id_option = TRUE;
  info->send_broadcast = TRUE;
  info->lease.lease_ip = info->core.yiaddr;
  info->lease.server_id = info->core.server_id;

  dhcpi_client_send_request( info );
  dhcpi_client_transition_state( info, DHCP_CLIENT_REQUESTING_S );
} /* dhcpi_client_s_offer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_RENEW

Description:
  This is the renew event function for the selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_s_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_RELEASE

Description:
  This is the release event function for selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_s_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_TIMER

Description:
  This is the timer event function for the selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_timer
(
  dhcp_client_core_info * info
)
{
  info->timer_count++;
  if ( info->timer_count < info->init.core_params.selecting_tries )
  {
    /* Not max count yet, try again */
    dhcpi_client_transition_state( info, DHCP_CLIENT_SELECTING_S );
  }
  else
  {
    /* Max count, start over */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  }
} /* dhcpi_client_s_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_ACK

Description:
  This is the ACK event function for the selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_ack
(
  dhcp_client_core_info * info
)
{
  ASSERT (NULL != info);

  if ( (TRUE == info->init.core_params.rapid_commit_option) && /* Sent */
       (TRUE == info->core.rapid_commit_option)) /* Received */
  {
    info->lease.lease_ip = info->core.yiaddr;
    info->lease.server_id = info->core.server_id;

    dhcpi_client_ack(info);

    /* Have processed the RCO in ACK; reset */
    info->core.rapid_commit_option = FALSE;
  }
  else
  {
    /* Ignore the ACK event */
    LOG_MSG_INFO2("DHCP CLIENT ignoring %d event in %d state",
            DHCP_CLIENT_ACK_E, DHCP_CLIENT_SELECTING_S, 0);
  }
} /* dhcpi_client_s_ack */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S_DECLINE

Description:
  This is the DECLINE event function for the selecting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s_decline
(
  dhcp_client_core_info * info
)
{
  ASSERT (NULL != info);
  if ( TRUE == info->init.core_params.rapid_commit_option && /* Sent */
       TRUE == info->core.rapid_commit_option) /* Received */
  {
    /* Do this only if RCO was sent and received */
    dhcpi_client_decline(info);
  }
  else
  {
    /* Ignore the DECLINE event */
    LOG_MSG_INFO2("DHCP CLIENT ignoring %d event in %d state",
            DHCP_CLIENT_DECLINE_E, DHCP_CLIENT_SELECTING_S, 0);
  }

} /* dhcpi_client_s_decline */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_V_RENEW

Description:
  This is the renew event function for the verifying state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_v_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_v_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_V_RELEASE

Description:
  This is the release event function for verifying state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_v_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_v_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_V_TIMER

Description:
  This is the timer event function for the verifying state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_v_timer
(
  dhcp_client_core_info * info
)
{
  dhcp_client_lease lease;
  dhcp_client_provision provision;
  ps_iface_ioctl_extended_ip_config_ex_type config_change_mask;

  info->lease.server_id = info->core.server_id;
  info->lease.lease_time = dss_ntohl(info->core.ip_lease);
  info->lease.T1_time = dss_ntohl(info->core.renew_time);
  info->lease.T2_time = dss_ntohl(info->core.rebind_time);

  lease.valid = TRUE;
  lease.ip_addr = info->core.yiaddr;
  lease.server_id = info->core.server_id;
  info->config.storage_set_lease( info->config.storage_handle, &lease );

  provision.ip_addr = info->core.yiaddr;
  provision.ip_mask = info->core.netmask;
  provision.router = info->core.router_addr;
  provision.dns_servers = info->core.dns;
  provision.domain_name = info->core.domain_name;
  provision.domain_name_len = info->core.domain_name_len;
  provision.mtu = info->core.mtu;
  provision.sip_addrs = info->core.sip_addr;
  provision.sip_domain_names = info->core.sip_domain_name;

  config_change_mask =
    info->config.configuration_set( info->config.configuration_handle,
                                    &provision );

  if ( NULL != info->init.status_cb )
  {
    info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_ASSIGNED );
  }
  /* Issue extended ip config event in ps */
  info->config.configuration_event_ind(info->config.configuration_handle,
                                       config_change_mask);

  dhcpi_client_transition_state( info, DHCP_CLIENT_BOUND_S );
} /* dhcpi_client_v_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RQ_RENEW

Description:
  This is the renew event function for the requesting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rq_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
} /* dhcpi_client_rq_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RQ_RELEASE

Description:
  This is the release event function for requesting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rq_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_release( info );
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_rq_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RQ_TIMER

Description:
  This is the timer event function for the requesting state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rq_timer
(
  dhcp_client_core_info * info
)
{
  info->timer_count++;
  if ( info->timer_count < info->init.core_params.requesting_tries )
  {
    /* Not max count, try again */
    dhcpi_client_transition_state( info, DHCP_CLIENT_REQUESTING_S );
  }
  else
  {
    /* Max count, start again */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
  }
} /* dhcpi_client_rq_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_B_RENEW

Description:
  This is the renew event function for the bound state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_b_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_RENEWING_S );

  info->request_ciaddr = info->lease.lease_ip;
  info->request_ip_addr_option = FALSE;
  info->request_server_id_option = FALSE;
  info->send_broadcast = FALSE;
  dhcpi_client_send_request( info );
} /* dhcpi_client_b_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_B_RELEASE

Description:
  This is the release event function for bound state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_b_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_release( info );
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_b_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_B_TIMER

Description:
  This is the timer event function for the bound state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_b_timer
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_RENEWING_S );

  info->request_ciaddr = info->lease.lease_ip;
  info->request_ip_addr_option = FALSE;
  info->request_server_id_option = FALSE;
  info->send_broadcast = FALSE;
  dhcpi_client_send_request( info );
} /* dhcpi_client_b_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RN_RENEW

Description:
  This is the renew event function for the renewing state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rn_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_REBINDING_S );

  info->send_broadcast = TRUE;
  info->request_ciaddr = info->lease.lease_ip;
  info->request_ip_addr_option = FALSE;
  info->request_server_id_option = FALSE;
  dhcpi_client_send_request( info );
} /* dhcpi_client_rn_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RN_RELEASE

Description:
  This is the release event function for renewing state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rn_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_release( info );
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_rn_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RN_TIMER

Description:
  This is the timer event function for the renewing state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rn_timer
(
  dhcp_client_core_info * info
)
{
  if ( info->timer_count < info->lease.T2_time - info->lease.T1_time )
  {
    /* Not max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_RENEWING_S );
    info->send_broadcast = FALSE;
  }
  else
  {
    /* Max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_REBINDING_S );
    info->send_broadcast = TRUE;
  }

  info->request_ciaddr = info->lease.lease_ip;
  info->request_ip_addr_option = FALSE;
  info->request_server_id_option = FALSE;
  dhcpi_client_send_request( info );
} /* dhcpi_client_rn_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RB_RENEW

Description:
  This is the renew event function for the rebinding state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rb_renew
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_REBOOT_S );
} /* dhcpi_client_rb_renew */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RB_RELEASE

Description:
  This is the release event function for rebinding state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rb_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_release( info );
  dhcpi_client_clear_lease( info, DHCP_CLIENT_IP_RELEASED );
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_rb_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RB_TIMER

Description:
  This is the timer event function for the rebinding state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rb_timer
(
  dhcp_client_core_info * info
)
{
  if ( info->timer_count < info->lease.lease_time - info->lease.T2_time )
  {
    /* Not max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_REBINDING_S );
    info->send_broadcast = TRUE;
    info->request_ciaddr = info->lease.lease_ip;
    info->request_ip_addr_option = FALSE;
    info->request_server_id_option = FALSE;
    dhcpi_client_send_request( info );
  }
  else
  {
    /* Max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INIT_S );
    if ( NULL != info->init.status_cb )
    {
      info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_EXPIRED );
    }
  }
} /* dhcpi_client_rb_timer */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_F_ACK

Description:
  This is the ACK event function for the INFORM state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_f_ack
(
  dhcp_client_core_info * info
)
{
  dhcp_client_provision provision;
  ps_iface_ioctl_extended_ip_config_ex_type config_change_mask;

  ASSERT(info);

  provision.ip_addr = 0; /* inform is stateless, so we don't set ip_addr */
  provision.ip_mask = info->core.netmask;
  provision.router = info->core.router_addr;
  provision.dns_servers = info->core.dns;
  provision.domain_name = info->core.domain_name;
  provision.domain_name_len = info->core.domain_name_len;
  provision.mtu = info->core.mtu;
  provision.sip_addrs = info->core.sip_addr;
  provision.sip_domain_names = info->core.sip_domain_name;

  config_change_mask = 
    info->config.configuration_set( info->config.configuration_handle,
                                    &provision );
  if ( NULL != info->init.status_cb )
  {
    info->init.status_cb( info->init.userdata, DHCP_CLIENT_PROVISIONED );
  }
  /* Issue extended ip config event in ps */
  info->config.configuration_event_ind(info->config.configuration_handle,
                                       config_change_mask);
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_f_ack */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_F_RELEASE

Description:
  This is the release event function for inform state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_f_release
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
} /* dhcpi_client_f_release */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_F_TIMER

Description:
  This is the timer event function for the inform state.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_f_timer
(
  dhcp_client_core_info * info
)
{
  ASSERT(info);

  info->timer_count++;
  if ( info->timer_count < info->init.core_params.inform_tries)
  {
    /* Not max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_INFORM_S );
  }
  else
  {
    /* Max count */
    dhcpi_client_transition_state( info, DHCP_CLIENT_PAUSE_S );
    if ( NULL != info->init.status_cb )
    {
      info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_FAIL );
    }
    /* Issue extended ip config event in ps */
    info->config.configuration_event_ind(info->config.configuration_handle,
                                         PS_IFACE_EXT_IP_CFG_MASK_NONE);
  }
} /* dhcpi_client_f_timer */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_P

Description:
  This is the pause state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_p
(
  dhcp_client_core_info * info
)
{
  /* We are stopping all services, Don't enable the timer. */
  (void)info;
} /* dhcpi_client_p */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_R

Description:
  This is the rebooting state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_r
(
  dhcp_client_core_info * info
)
{
  info->request_ciaddr = 0;
  info->request_ip_addr_option = TRUE;
  info->request_server_id_option = FALSE;
  info->send_broadcast = TRUE;

/* Technically, we haven't obtained our IP address. */
  info->config.configuration_clear(info->config.configuration_handle);

  /* If we are starting in this state, update the xid */
  if ( 0 == info->init_count++ )
  {
    info->init.xid++;
  }
  dhcpi_client_send_request( info );
  /* Randomized exponential backoff specified in RFC 2131, section 3.2
   * paragraph 3, with reference to section 4.1
   */
  info->config.timer_start( info->config.timer_handle,
                            (info->timer_count + 1) *
                              info->init.core_params.reboot_base_time,
                            DHCP_CLIENT_TIMER_RAND_BACKOFF );
} /* dhcpi_client_r */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_I

Description:
  This is the init state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_i
(
  dhcp_client_core_info * info
)
{
  dhcp_client_lease lease;
  dhcp_client_event_payload_type event_payload;

  /* Report that we are freeing the lease; */
  event_payload.client_ip_addr = info->lease.lease_ip;
  event_payload.server_ip_addr = info->lease.server_id;
  event_payload.event_type = (uint8)DHCP_CLIENT_EVENT_LEASE_EXPIRE;
  event_payload.state = (uint8)info->state;
  event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
  /* Clear the lease. */
  memset( &lease, 0, sizeof( dhcp_client_lease ) );
  lease.valid = FALSE;
  info->config.storage_set_lease( info->config.storage_handle, &lease );
  /* Clear the provisioning, if any */
  info->config.configuration_clear( info->config.configuration_handle );

  /* If we are starting in this state, update the xid */
  if ( 0 == info->init_count )
  {
    info->init.xid++;
  }

  if( info->init.core_params.init_tries <= info->init_count++ )
  {
    info->init_count = 0;
    LOG_MSG_INFO1("Init count exceeded %d, calling status callback",
                  info->init.core_params.init_tries, 0, 0 );
    if ( NULL != info->init.status_cb )
    {
      info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_FAIL );
    }
    /* Issue extended ip config event in ps */
    info->config.configuration_event_ind(info->config.configuration_handle,
                                         PS_IFACE_EXT_IP_CFG_MASK_NONE);
  }

  if ( TRUE == info->decline_restart )
  {
    /* decline time specified in RFC 2131, section 3.1 paragraph 5. */
    info->config.timer_start( info->config.timer_handle,
                              info->init.core_params.decline_restart_time,
                              DHCP_CLIENT_TIMER_NORM );
    info->decline_restart = FALSE;
  }
  else
  {
    /* Randomized init specified in RFC 2131, section 4.4.1 */
    info->config.timer_start( info->config.timer_handle,
                              info->init.core_params.init_restart_time,
                              DHCP_CLIENT_TIMER_RAND_INIT );
  }
} /* dhcpi_client_i */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_S

Description:
  This is the selecting state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_s
(
  dhcp_client_core_info * info
)
{
  dhcpi_client_send_discover( info );
  /* RFC2131 gives minimal guidence in this case, section 3.1,
   * paragraph 3. We default to section 4.1
   */
  info->config.timer_start( info->config.timer_handle,
                            (info->timer_count + 1) *
                              info->init.core_params.select_base_time,
                            DHCP_CLIENT_TIMER_RAND_BACKOFF );
} /* dhcpi_client_s */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_V

Description:
  This is the verifying state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_v
(
  dhcp_client_core_info * info
)
{
  /* RFC2131 gives no guidence in this case. "Implementation dependant"*/
  info->config.timer_start( info->config.timer_handle,
                            info->init.core_params.verify_time,
                            DHCP_CLIENT_TIMER_NORM );
} /* dhcpi_client_v */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RQ

Description:
  This is the requesting state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rq
(
  dhcp_client_core_info * info
)
{
  /* Randomized exponential backoff specified in RFC 2131, section 3.1
   * paragraph 5, with reference to section 4.1
   */
  info->config.timer_start( info->config.timer_handle,
                            (info->timer_count + 1) *
                              info->init.core_params.request_base_time,
                            DHCP_CLIENT_TIMER_RAND_BACKOFF );
} /* dhcpi_client_rq */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_B

Description:
  This is the bound state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_b
(
  dhcp_client_core_info * info
)
{
  /* We have successfully got a lease, zero out the init_count so that we
   * don't hit it prematurely
   */
  info->init_count = 0;
  /* If we have an inifinite time, then skip calculating the other
   * times, and setting the timer.
   */
  if ( 0xffffffff != info->lease.lease_time )
  {
    /* Times in this function are specified by RFC 2131, section 4.4.5 */
    if ( 0 == info->lease.T1_time )
    {
      /* The server didn't specify the T1 time, so we calculate it here */
      info->lease.T1_time = (uint32)(info->lease.lease_time * 0.5);
    }
    if ( 0 == info->lease.T2_time )
    {
      /* The server didn't specify the T2 time, so we calculate it here */
      info->lease.T2_time = (uint32)(info->lease.lease_time * 0.875);
    }
    info->config.timer_start( info->config.timer_handle,
                              info->lease.T1_time, DHCP_CLIENT_TIMER_NORM );
  }
} /* dhcpi_client_b */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RN

Description:
  This is the renewing state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rn
(
  dhcp_client_core_info * info
)
{
  /* Times in this function are specified by RFC 2131, section 4.4.5 */
  uint32 delta;

  /* First, calculate the amount of time left in the state */
  delta = info->lease.T2_time - info->lease.T1_time - info->timer_count;
  /* If the time is less than 60 seconds, per RFC, we are done,
   * otherwise, we cut the time in half
   */
  delta >>= 1;
  if ( 60 > delta )
  {
    delta = info->lease.T2_time - info->lease.T1_time - info->timer_count;
  }
  /* Add delta back into timer_count */
  info->timer_count += delta;
  /* Set the timer */
  info->config.timer_start( info->config.timer_handle, delta,
                            DHCP_CLIENT_TIMER_NORM );
} /* dhcpi_client_rn */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_RB

Description:
  This is the rebinding state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_rb
(
  dhcp_client_core_info * info
)
{
  /* Times in this function are specified by RFC 2131, section 4.4.5 */
  uint32 delta;

  /* First, calculate the amount of time left in the state */
  delta = info->lease.lease_time - info->lease.T2_time - info->timer_count;
  /* If the time is less than 60 seconds, per RFC, we are done,
   * otherwise, we cut the time in half
   */
  delta >>= 1;
  if ( 60 > delta )
  {
    delta = info->lease.lease_time - info->lease.T2_time - info->timer_count;
  }
  /* Add delta back into timer_count */
  info->timer_count += delta;
  /* Set the timer */
  info->config.timer_start( info->config.timer_handle, delta,
                            DHCP_CLIENT_TIMER_NORM );
} /* dhcpi_client_rb */
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_F

Description:
  This is the inform state arrival function.

Arguements:
  dhcp_client_core_info * - The current client handle.

Return value:
  None.

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_f
(
  dhcp_client_core_info * info
)
{
  /* Send the inform message. */
  info->send_broadcast = TRUE;
  dhcpi_client_send_inform( info );

  /* Randomized exponential backoff specified in RFC 2131, section 3.1
   * paragraph 5, with reference to section 4.1
   */
  info->config.timer_start( info->config.timer_handle,
                            (info->timer_count + 1) *
                              info->init.core_params.inform_base_time,
                            DHCP_CLIENT_TIMER_RAND_BACKOFF );
} /* dhcpi_client_f */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_NEW

Description:
  This function initializes the core with internal data and returns a handle
  to a core object.

Arguements:
  dhcp_client_core_init * init - the information (see structure above) needed
    to initialize the core.

Return value:
  void * - A handle to the new core object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other core functions
  are called.  The handle must be started (see dhcp_clienti_core_start() below)
  before the core is fully functional.
===========================================================================*/
void *
dhcp_clienti_core_new
(
  dhcp_client_core_init * init
)
{
  dhcp_client_core_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CORE INIT ---");

  if (NULL == init)
  {
    DATA_ERR_FATAL("init NULL?");
    return NULL;
  }
  if (NULL == init->d_malloc || NULL == init->d_free)
  {
    LOG_MSG_ERROR_2("malloc or free NULL %d %d", init->d_malloc, init->d_free);
    ASSERT(0);
    return NULL;
  }

  if ( DHCP_CLIENT_CHADDR_LEN < init->core_params.hw_addr_len )
  {
    LOG_MSG_ERROR_1("Hardware address too large %d", init->core_params.hw_addr_len);
    ASSERT(0);
    return NULL;
  }

  if (( NULL == init->core_params.hw_addr &&
        0 < init->core_params.hw_addr_len ) ||
      ( NULL != init->core_params.hw_addr &&
        0 == init->core_params.hw_addr_len ))
  {
    DATA_ERR_FATAL("hw_addr does not match hw_addr_len");
    return NULL;
  }

  handle = init->d_malloc(sizeof(dhcp_client_core_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP core");
    return NULL;
  }
  /* Zero out the handle structure */
  memset( handle, 0, sizeof(dhcp_client_core_info) );
  handle->init = *init; /* struct copy */
  handle->cookie = DHCP_CLIENT_CORE_COOKIE;
  if (NULL != init->core_params.hw_addr)
  {
    memscpy( handle->hwaddr,
            DHCP_CLIENT_CHADDR_LEN, 
            init->core_params.hw_addr,
            init->core_params.hw_addr_len );
  }

  if (init->core_params.client_id_type == DHCP_CLIENT_ID_OPTTYPE_CUSTOM)
  {
    if ( (init->core_params.client_id_len <
            DHCP_CLIENT_CLIENT_ID_MIN_LEN) ||
         (init->core_params.client_id_len >
            DHCP_CLIENT_CLIENT_ID_MAX_LEN) )
    {
      handle->init.core_params.client_id_type =
        DHCP_CLIENT_ID_OPTTYPE_NONE;
      handle->init.core_params.client_id_len  = 0;
      memset( handle->init.core_params.client_id,
              0, DHCP_CLIENT_CLIENT_ID_MAX_LEN );
      LOG_MSG_ERROR_1("Invalid custom DHCP client identifier with len=%d found",
                      init->core_params.client_id_len);
    }
  }

  /* safeguard : clear ref to dynamic memory in init parameter */
  handle->init.core_params.hw_addr = NULL;

  return handle;
} /* dhcp_clienti_core_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_START

Description:
  This function configures the core with the information on how to interact
  with the other dhcp client modules.

Arguements:
  void * handle - Handle to the core object to start.
  dhcp_client_core_config * config - The configuartion information needed to
    interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if successful, otherwise FALSE.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() above.
===========================================================================*/
boolean
dhcp_clienti_core_start
(
  void * handle,
  dhcp_client_core_config * config
)
{
  dhcp_client_core_info * info = handle;

  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CORE START ---");
  /* sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return FALSE;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie,DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return FALSE;
  }
  if (NULL == config)
  {
    DATA_ERR_FATAL("config NULL?");
    return FALSE;
  }

  /* Initialize "config" info */
  info->config = *config; /* struct copy */
  info->cookie_ptr = info;
  info->decline_restart = FALSE;

  return TRUE;
} /* dhcp_clienti_core_start */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_STOP

Description:
  This function stops a DHCP client core, including timers and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.  Note:
  This function does not release a lease.  See dhcp_clienti_core_release below.

Arguements:
  void ** handle - The handle to the core to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new().
===========================================================================*/
void
dhcp_clienti_core_stop
(
  void ** handle
)
{
  dhcp_client_core_info * info;

  /* Sanity check the parameters */
  if (NULL == handle)
  {
    DATA_ERR_FATAL("handle pointer NULL?");
    return;
  }

  info = *handle;

  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie,DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }

  info->init.d_free(info);
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CORE STOP ---");
  *handle = NULL;
} /* dhcp_clienti_core_stop */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_RENEW

Description:
  This function causes a the DHCP client core to start a renew sequence.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_renew
(
  void * handle
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie,DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }

  dhcpi_client_process_event( info, DHCP_CLIENT_RENEW_E );
} /* dhcp_clienti_core_renew */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_RELEASE

Description:
  This function causes the DHCP client core to release the lease it has. If
  there is no current lease, nothing will happen.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_release
(
  void * handle
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR("Cookie mismatch %x %x", info->cookie, 
                   DHCP_CLIENT_CORE_COOKIE, 0);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR("cookie pointer mismatch %x %x", info->cookie_ptr, info, 0);
    ASSERT(0);
    return;
  }

  dhcpi_client_process_event( info, DHCP_CLIENT_RELEASE_E );
} /* dhcp_clienti_core_release */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_REPLY

Description:
  This function is used to send messages from servers to the core for
  processing.  Parsing of the message and event processing will happen before
  this function returns.  The memory associated with the incoming data may be
  freed on return of this function.

Arguements:
  void * handle - handle to DHCP Client core.
  void * msg - a handle to the incoming messsage.
  boolean (*extract) - The extract function to extract data from the message
    handle. Arguements as follows:
      - dest - The buffer into which the data is put.
      - src - The message pointer.
      - offset - The offset into the message to start extracting.
      - len - the length of the message to extract.
    Return value will be whether the function succeeds in extracting correct
    amount of data.  If FALSE, the dest will be assumed to not have any new
    data in it.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_reply
(
  void * handle,
  void * msg,
  boolean (*extract)( void * dest, void * src, uint32 offset, uint32 len)
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;
  dhcp_client_meta_option * opt;
  uint32 i;
  boolean ret;
  dhcp_client_event_payload_type event_payload;


  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie, 
                    DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (NULL == extract)
  {
    DATA_ERR_FATAL("extract function NULL?");
    return;
  }

  /* Set up info to be able to get to the message for parsing */
  info->extract = extract;
  info->msg = msg;

  /* Zero the core so that the values are known */
  ASSERT( NULL == info->core.opt );
  memset( &info->core, 0, sizeof ( dhcp_client_core_msg_info ) );

  /* Parse the message */
  ret = dhcpi_client_parse_msg( info );

  /* Grab info from and free the options */
  while ( NULL != info->core.opt )
  {
    for ( i = 0; DHCP_OPTION_TABLE_SIZE > i; i++ )
    {
      if ( info->core.opt->option == dhcp_client_option_lookup[i].op )
      {
        DHCP_SET_IF_FAIL(ret, dhcp_client_option_lookup[i].handler(info));
        break;
      }
    }
    opt = info->core.opt;
    info->core.opt = opt->next;
    info->init.d_free(opt);
  }
  /* If the message was valid, check the type of message and act on it */
  if ( TRUE == ret )
  {
    event_payload.state = (uint8)info->state;
    event_payload.client_ip_addr = info->core.yiaddr;
    event_payload.server_ip_addr = info->core.server_id;
    switch ( info->core.dhcp_msg_reply )
    {
    case DHCP_CLIENT_DHCPACK:
      dhcpi_client_process_event( info, DHCP_CLIENT_ACK_E );
      event_payload.event_type = (uint8)DHCP_CLIENT_EVENT_ACK;
      event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
      break;
    case DHCP_CLIENT_DHCPNACK:
      dhcpi_client_process_event( info, DHCP_CLIENT_NAK_E );
      event_payload.event_type = (uint8)DHCP_CLIENT_EVENT_NAK;
      event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
      break;
    case DHCP_CLIENT_DHCPOFFER:
      dhcpi_client_process_event( info, DHCP_CLIENT_OFFER_E );
      event_payload.event_type = (uint8)DHCP_CLIENT_EVENT_OFFER;
      event_report_payload(EVENT_DHCP, sizeof(event_payload), &event_payload);
      break;
    default:
      /* We got an invalid message */
      LOG_MSG_INFO1_1( "DHCP client invalid type %d",info->core.dhcp_msg_reply);
      break;
    }
  }
  /* Free any data in the core that we may have allocated for
   * processing this message.
   */
  dhcpi_client_free_core_options_mem(info);

} /* dhcp_clienti_core_reply */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_DECLINE

Description:
  This function informs the core that a proposed (or in use) lease has been
  detected to be bad.  This function is called from the configuration layer.

  This function will cause the lease to be declined back to the server, and if
  in use, will cause the network interface to be cleared.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_decline
(
  void * handle
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie,
                    DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }

  info->decline_restart = TRUE;
  dhcpi_client_process_event( info, DHCP_CLIENT_DECLINE_E );
} /* dhcp_clienti_core_decline */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_TIMER_EXPIRE

Description:
  This function informs the core that the timer has expired.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_timer_expire
(
  void * handle
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie, 
                     DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }

  dhcpi_client_process_event( info, DHCP_CLIENT_TIMER_E );
} /* dhcp_clienti_core_timer_expire */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_INFORM

Description:
  This function sends an INFORM event to the core.  If the core is in
  an acceptable state, the INFORM event will cause an inform message
  which will configure the stacks without obtaining an IP lease.

Arguements:
  void * handle - handle to DHCP Client core.
  uint32 inform_addr - the IP address we have been provisioned with, in
                       network byte order.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
void
dhcp_clienti_core_inform
(
  void * handle,
  uint32 inform_addr
)
{
  dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return;
  }
  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie, 
                     DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return;
  }

  info->inform_addr = inform_addr;
  dhcpi_client_process_event( info, DHCP_CLIENT_INFORM_E );
} /* dhcp_clienti_core_inform */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CORE_IS_DHCP_CORE_IN_PROGRESS

Description:
  This function returns whether the DHCP client CORE is operational.

Arguements:
  void * handle - handle to DHCP Client core.

Return value:
  TRUE if client is running, FALSE otherwise.

Dependencies:
  The handle must have been allocated by dhcp_clienti_core_new() and started
  by dhcp_clienti_core_start().
===========================================================================*/
boolean
dhcp_clienti_core_is_dhcp_core_in_progress
(
  void * handle
)
{
  const dhcp_client_core_info * info = (dhcp_client_core_info *) handle;

  /* Sanity check the parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("info NULL");
    return FALSE;
  }

  if ( DHCP_CLIENT_CORE_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie mismatch %x %x", info->cookie, 
                    DHCP_CLIENT_CORE_COOKIE);
    ASSERT(0);
    return FALSE;
  }

  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("cookie pointer mismatch %x %x", info->cookie_ptr, info);
    ASSERT(0);
    return FALSE;
  }

  if (DHCP_CLIENT_PAUSE_S == info->state ||
      DHCP_CLIENT_BOUND_S == info->state)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* dhcp_clienti_core_is_core_running */

