#ifndef _DSSOCKET_DEFS_H
#define _DSSOCKET_DEFS_H

/*===========================================================================

   D A T A   S E R V I C E S   S O C K E T   D E F S   H E A D E R   F I L E

DESCRIPTION

 The Data Services Socket Definitions Header File. This file contains
 declarations used internally by the DMSS code as well as by the socket
 applications.

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dssocket_defs.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/10/14    ssh    IP Translation Engine 464XLAT 
11/11/13    rk     Removed feature mobileap.
03/08/12    su     Changes for VToLTE feature 
10/02/10    pp     Modified DSS_MAX_RCVBUF, DSS_MAX_SNDBUF to 256K to support 
                   RevB field [high RTT] scenarios. 
06/30/10    rt     NAT IFACE addition.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
03/11/09    jy     Changes to avoid name conflicts when build with windows.h
                   using prefixing and macros.
03/06/09    pp     Moved config macros to dss_config.h
08/12/08    pp     Added default TCP RTT parameter to sock_config_cb.
07/01/08    pp     Added DSS_IFACE_UICC, DSS_IFACE_SLIP.
02/28/07    sv     Increase ICMP sockets to support ping.
09/18/06    hm     Changed macro DSS_IFACE_GET_ID3 to refer to ps_iface_get_id3
04/14/06    ss     Added DVBH_IFACE to the iface names
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
11/29/05    sv     Added support for RSVP
08/15/05    vp     Moved out definition of dss_sock_extended_err and related
                   definitions.
05/27/05    rt     Added tcp_count in sock_config_cb to keep track of
                   allocated tcb's.
06/13/05    vp     Definition of struct sock_extended_err and related const's
05/04/05    mct    Added new iface FLO_IFACE.
04/18/05    vp     Typedef for dss_scope_id_type.Defined DSS_INVALID_SCOPE_ID
04/17/05    msr    Changed DSS_IFACE_GET_ID3 to take flow instance instead of
                   that of phys link's.
02/10/05    mct    Moved/renamed dssocki_msghdr to dss_msghdr in dssocket.h.
12/23/04    lyr    Added WLAN iface support in dss_iface_name_enum_type
10/30/04    mct    Moved dssocki_msghdr here to provide the new recv/sndmsg
                   api functionality.
10/30/04    msr    Changed the definition of DSS_IFACE_INVALID_ID to point
                   to PS_IFACE_INVALID_ID.
08/30/04    sv     Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
08/16/04    ks/msr Added SCB/ACB/TCB/UCB/ICMB queues to socket config control
                   block.
07/30/04    sv     Added declaration for socket config control block.
06/11/04    vp     Inclusion of ps_in.h and definition of byte ordering macros.
05/17/04    aku    Removed featurization of BCMCS iface with the objevtive of
                   not having any features in the header file.
04/28/04    aku    Added support for BCMCS iface.
03/30/04    vp     Merged changes from June04Dev_05.00.00 ps branch.
02/09/04    aku    Include ps_iface_defs.h
10/28/03    sv     Added version macro for dss_get_iface_id.
10/20/03    sv     Changed dss_get_iface_id to take an dss_nethandle instead of union.
08/28/03    ss     Moved/added iface name and id related declarations to this
                   file.
08/18/03    sv     Moved dss_htonl and related macros to ps_in header file.
07/25/03    ss     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_socket_defs.h"
#include "ps_in_alias.h"    /* Macros to map certain data/data types with
                               prefix "ps_" or "dss_" prefixed to data/data
                               type without prefix */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Address/Protocol Family
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_PF_INET  = 1,                        /* Protocol Family - Internet   */
  DSS_PF_INET6 = 2                         /* Protocol Family - Internet v6*/
} dss_protocol_family_enum_type;

enum
{
  DSS_AF_INET    = DSS_PF_INET,            /* Address Family - Internet    */
  DSS_AF_INET6   = DSS_PF_INET6,           /* Address Family - Internet v6 */
  DSS_AF_UNSPEC  = 0,                      /* Unspecified address family   */
  DSS_AF_ANY     = 4                       /* Any address family           */
};

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define DSS_SUCCESS         0                      /* successful operation */
#define DSS_ERROR          -1                    /* unsuccessful operation */


/*---------------------------------------------------------------------------
                             Socket types
---------------------------------------------------------------------------*/
enum
{
  DSS_SOCK_STREAM = 0,                             /* TCP - streaming socket */
  DSS_SOCK_DGRAM  = 1                               /* UDP - datagram socket */
};

/*---------------------------------------------------------------------------
                      Asynchronous Socket Events
---------------------------------------------------------------------------*/
#define DS_WRITE_EVENT  0x01         /* associated with a writeable socket */
#define DS_READ_EVENT   0x02          /* associated with a readable socket */
#define DS_CLOSE_EVENT  0x04         /* associated with a closeable socket */
#define DS_ACCEPT_EVENT 0x08        /* associated with an accetable socket */

/*---------------------------------------------------------------------------
  Maximum bytes you can read using a single read call.
---------------------------------------------------------------------------*/
#define DSS_READ_MAX_BYTES ( (1 << 15) - 1)
#define DSS_WRITE_MAX_BYTES ( (1 << 15) - 1)

/*---------------------------------------------------------------------------
  Range values for various socket options
---------------------------------------------------------------------------*/
#define DSS_MIN_RCVBUF (536)
#define DSS_MAX_RCVBUF (256 * 1024)

#define DSS_MIN_SNDBUF (536)
#define DSS_DEF_SNDBUF (10 * 1024)
#define DSS_MAX_SNDBUF (256 * 1024)

/*===========================================================================
  Define flags passed to dss_sendto() calls here. Any time a new flag is to
  be added, define it here and OR it with the existing flags in
  MSG_VALID_BITS macro definition in dssocki.c so that the reserved bit
  mask can be properly built.

  Note: Refer to the SDB related flags defined in dssocket_defs.h when
        adding a new flag to avoid assigning the same value.
===========================================================================*/
#define DSS_MSG_ERRQUEUE 0x2000

#define DSS_MSG_TRUNC    8

/*---------------------------------------------------------------------------
  Socket extended error structure for ICMP(v6) error reporting and related
  constants.
---------------------------------------------------------------------------*/
#define DSS_SO_EE_ORIGIN_LOCAL PS_SO_EE_ORIGIN_LOCAL
#define DSS_SO_EE_ORIGIN_ICMP  PS_SO_EE_ORIGIN_ICMP
#define DSS_SO_EE_ORIGIN_ICMP6 PS_SO_EE_ORIGIN_ICMP6

typedef struct ps_sock_extended_err dss_sock_extended_err;

/*---------------------------------------------------------------------------
  Message header structure for the common I/o functions.
---------------------------------------------------------------------------*/
struct dss_msghdr
{
  void               * msg_name;            /* protocol address            */
  uint16               msg_namelen;         /* size of protocol address    */
  struct ps_iovec    * msg_iov;             /* scatter/gather array        */
  uint16               msg_iovlen;          /* # elements in msg_iov       */
  void               * msg_control;         /* ancillary data              */
  uint16               msg_controllen;      /* length of ancillary data    */
  int                  msg_flags;           /* flags returned by recvmsg   */
};

/*---------------------------------------------------------------------------
  Message header structure for ancillary data.
---------------------------------------------------------------------------*/
struct dss_cmsghdr
{
  uint32   cmsg_len;                  /* data byte count, including header */
  int32    cmsg_level;                             /* originating protocol */
  int32    cmsg_type;                            /* protocol-specific type */
};

/*---------------------------------------------------------------------------
  Macros required to process and control the ancillary data struct cmsghdr.
---------------------------------------------------------------------------*/


#define DSS_CMSG_DATA(cmsg) ((void*)((unsigned char *)(cmsg) + \
                             DSS_CMSG_ALIGN(sizeof(struct dss_cmsghdr))))

#define DSS_CMSG_NXTHDR(msg, cmsg) cmsg_nxthdr (msg, cmsg)

#define DSS_CMSG_FIRSTHDR(msg) \
  ((uint32) (msg)->msg_controllen >= sizeof (struct dss_cmsghdr)          \
   ? (struct dss_cmsghdr *) (msg)->msg_control : NULL)

#define DSS_CMSG_ALIGN(len) ( ((len) + sizeof(uint32) - 1) \
                              & (uint32) ~(sizeof(uint32) - 1) )

#define DSS_CMSG_SPACE(len) ( DSS_CMSG_ALIGN(len) \
                              + DSS_CMSG_ALIGN(sizeof(struct dss_cmsghdr)) )

#define DSS_CMSG_LEN(len) ( DSS_CMSG_ALIGN (sizeof(struct dss_cmsghdr)) + \
                            (len) )

#ifdef __cplusplus
extern "C" {
#endif
INLINE struct dss_cmsghdr * cmsg_nxthdr
(
  struct dss_msghdr  * msg,
  struct dss_cmsghdr * cmsg
)
{
  if ((uint32) cmsg->cmsg_len < sizeof (struct dss_cmsghdr))
    return NULL;

  cmsg = (struct dss_cmsghdr *) ((unsigned char *) cmsg
                   + DSS_CMSG_ALIGN (cmsg->cmsg_len));
  if((unsigned char *) (cmsg + 1) > ((unsigned char *) msg->msg_control
                    + msg->msg_controllen))
    return NULL;
  return cmsg;
}
#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------
  Definition for invalid handle
---------------------------------------------------------------------------*/
#define DSS_INVALID_HANDLE_TYPE     0

/*---------------------------------------------------------------------------
  Invalid iface_id: does not identify any valid iface
---------------------------------------------------------------------------*/
#define DSS_IFACE_INVALID_ID  (PS_IFACE_INVALID_ID)

/*---------------------------------------------------------------------------
  Flags passed to dss_sendto() call for SDB. Non-SDB related flags are
  defined in dssocket.h. Any time a SDB related flags is added here, OR it
  with the existing flags in MSG_VALID_BITS macro definition in dssocki.c
  so that the reserved bit mask can be properly built.

  SDB flags will cause short data burst to be used if the mobile is not on
  traffic. Don't set any bit if one doesn't want SDB and traffic is to be
  brought up. Set both bits if one doesn't care which channel is used for
  SDB transmission, else set the appropriate bit.
---------------------------------------------------------------------------*/
#define MSG_EXPEDITE      0x00000001   /* use ACH/REACH/TCH                */
#define MSG_FAST_EXPEDITE 0x00000002   /* use     REACH/TCH                */
#define MSG_WAKEUP        0x00000004   /* Bring up      TCH                */
/*---------------------------------------------------------------------------
  Definition of iface names
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_CDMA_SN          = CDMA_SN_IFACE,
  DSS_IFACE_CDMA_AN          = CDMA_AN_IFACE,
  DSS_IFACE_CDMA_BCAST       = CDMA_BCAST_IFACE,
  DSS_IFACE_FLO              = FLO_IFACE,
  DSS_IFACE_DVBH             = DVBH_IFACE,
  DSS_IFACE_UMTS             = UMTS_IFACE,
  DSS_IFACE_SIO              = SIO_IFACE,
  DSS_IFACE_LO               = LO_IFACE,
  DSS_IFACE_WLAN             = WLAN_IFACE,
  DSS_IFACE_IWLAN_S2B        = IWLAN_S2B_IFACE,
  DSS_IFACE_MBMS             = MBMS_IFACE,
  DSS_IFACE_SLIP             = SLIP_IFACE,
  DSS_IFACE_UICC             = UICC_IFACE,
  DSS_IFACE_WWAN             = WWAN_GROUP,
  DSS_IFACE_ANY_DEFAULT      = ANY_DEFAULT_GROUP,
  DSS_IFACE_ANY              = ANY_IFACE_GROUP,
  DSS_IFACE_RM                = RM_GROUP,
  DSS_IFACE_3GPP_ANY          = IFACE_3GPP_GROUP,
  DSS_IFACE_3GPP2_ANY         = IFACE_3GPP2_GROUP,
  DSS_IFACE_EPC_ANY           = IFACE_EPC_GROUP,
  DSS_IFACE_IWLAN_3GPP        = IWLAN_3GPP_IFACE,
  DSS_IFACE_IWLAN_3GPP2       = IWLAN_3GPP2_IFACE,
  DSS_IFACE_UW_FMC            = UW_FMC_IFACE,
  DSS_IFACE_EPC               = EPC_IFACE,
  DSS_IFACE_CLAT              = CLAT_IFACE,
  DSS_IFACE_MODEM_LINK_LOCAL  = MODEM_LINK_LOCAL_IFACE
} dss_iface_name_enum_type;

/*---------------------------------------------------------------------------
  opaque interface id
---------------------------------------------------------------------------*/

typedef uint32 dss_iface_id_type;

/*---------------------------------------------------------------------------
  Enum to indicate the SDB ack status code to the application. The pkt mgr
  receives the status from CM and maps it to the this enum to be returned
  to the application. Changes to CM/Pkt mgr may necessitate modifying this
  enum definition.
---------------------------------------------------------------------------*/
typedef enum dss_sdb_ack_status_enum_type
{
  DSS_SDB_ACK_NONE = PS_PHYS_LINK_DOS_ACK_NONE,
  DSS_SDB_ACK_OK   = PS_PHYS_LINK_DOS_ACK_OK,

  DSS_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT =
    PS_PHYS_LINK_DOS_ACK_HOLD_ORIG_RETRY_TIMEOUT,

  DSS_SDB_ACK_HOLD_ORIG           = PS_PHYS_LINK_DOS_ACK_HOLD_ORIG,
  DSS_SDB_ACK_NO_SRV              = PS_PHYS_LINK_DOS_ACK_NO_SRV,
  DSS_SDB_ACK_ABORT               = PS_PHYS_LINK_DOS_ACK_ABORT,
  DSS_SDB_ACK_NOT_ALLOWED_IN_AMPS = PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_AMPS,
  DSS_SDB_ACK_NOT_ALLOWED_IN_HDR  = PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_HDR,
  DSS_SDB_ACK_L2_ACK_FAILURE      = PS_PHYS_LINK_DOS_ACK_L2_ACK_FAILURE,
  DSS_SDB_ACK_OUT_OF_RESOURCES    = PS_PHYS_LINK_DOS_ACK_OUT_OF_RESOURCES,
  DSS_SDB_ACK_ACCESS_TOO_LARGE    = PS_PHYS_LINK_DOS_ACK_ACCESS_TOO_LARGE,
  DSS_SDB_ACK_DTC_TOO_LARGE       = PS_PHYS_LINK_DOS_ACK_DTC_TOO_LARGE,
  DSS_SDB_ACK_OTHER               = PS_PHYS_LINK_DOS_ACK_OTHER,
  DSS_SDB_ACK_ACCT_BLOCK          = PS_PHYS_LINK_DOS_ACK_ACCT_BLOCK,
  DSS_SDB_ACK_L3_ACK_FAILURE      = PS_PHYS_LINK_DOS_ACK_L3_ACK_FAILURE,
  DSS_SDB_ACK_FORCE_32_BIT        = 0x7FFFFFFF  /* Force enum to be 32-bit */
} dss_sdb_ack_status_enum_type;

/*---------------------------------------------------------------------------
  The structure containing information about the sdb ack status. This info
  is returned to the socket application in the sdb ack callback if the sdb
  ack callback socket option is set.
---------------------------------------------------------------------------*/
typedef struct dss_sdb_ack_status_info_type
{
  uint32                       overflow;
  dss_sdb_ack_status_enum_type status;
} dss_sdb_ack_status_info_type;

/*---------------------------------------------------------------------------
  Opaque Scope id for the application.
---------------------------------------------------------------------------*/
typedef uint32 dss_scope_id_type;

/*---------------------------------------------------------------------------
  QOS handle type.
---------------------------------------------------------------------------*/
typedef uint32 dss_qos_handle_type;

#define DSS_INVALID_SCOPE_ID 0

/*===========================================================================

                          PUBLIC MACRO DECLARATIONS

===========================================================================*/
#define DSS_IFACE_GET_ID3 ps_iface_get_id3
/*===========================================================================

MACRO DSS_HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htonl(x) ps_htonl(x)

/*===========================================================================

MACRO DSS_HTONLL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htonll(x) ps_htonll(x)

/*===========================================================================

MACRO DSS_HTONS()

DESCRIPTION
 Converts host-to-network short integer.  Handles potential byte order
 differences between different computer architectures and different network
 protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htons(x) ps_htons(x)

/*===========================================================================

MACRO DSS_NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohl(x) ps_ntohl(x)

/*===========================================================================

MACRO DSS_NTOHLL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohll(x) ps_ntohll(x)

/*===========================================================================

MACRO DSS_NTOHS()

DESCRIPTION
  Converts network-to-host short integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohs(x) ps_ntohs(x)


#endif  /* _DSSOCKET_DEFS_H */

