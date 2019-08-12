/*===========================================================================

                      P S _ I F A C E _ I P F L T R . C

DESCRIPTION
  This file provides functions to use IP filters for inbound iface specific
  processing of IP packets received on that iface.

EXTERNALIZED FUNCTIONS

  PS_IFACE_IPFLTR_ADD()
    adds filters to the iface

  PS_IFACE_IPFLTR_DELETE()
    deletes filters from the iface

  PS_IFACE_IPFLTR_CONTROL()
    Performs control operation on filters (enable/disable)

  PS_IFACE_IPFLTR_EXECUTE()
    executes filter for the processing inbound IP pkts

  PS_IFACE_IPFLTR_PROCESS_PKT
    Process a raw IP pkt and then performs filtering on the packet

Copyright (c) 2003-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_ipfltr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/18/13    tw     Optimized filters for when client matches alternate PDN
03/28/12    kr     Resetting next_prot_hdr after each filter validation
03/26/12    su     opt filters are applicable only for client/firewall filters
03/25/12    sb     Added support for TCP_UDP protocol in optimized filters.
02/01/12    am     Added port range support and TCP_UDP types to opt filters.
09/22/11    am     Added support for optimized DL path filters.
01/15/11    hs     Made changes to install filter in A2 - port num, IP addr
12/27/10    cp     Fix to process the filter when the next header protocol
                   is set to PS_IPPROTO_TCP_UDP
05/25/10    sy     Featurized inclusion of a2_ipfilter.h
05/13/10    pp     ps_iface_ipfltri_process_default: corrected next_hdr_prot
                   check for IPv6 during filter execution.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
10/01/07    mct    Added support for ICMPv6 filtering.
08/21/2006  rt     Fixed incorrect next protocol field comparison for v6 in
                   ps_iface_ipfltri_process_default().
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/05/05    msr    Added support for fltr validation
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
08/03/05    msr    Posting IPFLTR_UPDATED_EV only if fltr status is changed
05/12/05    ks     fixed lint errors.
05/03/05    msr    Using int16 instead of errno_enum_type.
04/20/05    sv     Movied pkt processing code to pkt_info_utils file.
04/19/05    msr    Reverting earlier changes.
04/19/05    msr    Setting fi_result to default flow if packet doesn't match
                   any filter.
04/17/05    msr    Added subset ID and separated filter ID from filter result.
03/25/05    ifk    Added support for IPv6 filtering.
03/09/05    ssh    Enabled fragment filtering, and made minor tweaks to
                   ipfltr_delete() and ipfltr_process_fragment() functions
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
01/26/05    mct    Fixed incorrect q_insert.
01/10/05    sv     Merged IPSEC changes.
01/10/05    ifk    Added support for filtering on fragments.
01/08/05    msr    Added link initialization for Tx IP filters before they
                   are enqueued.
12/06/04    mct    Fixed loop where ipfilter was not being properly executed.
10/13/04    vp     Removal of ps_iface_ipfltri_ntoh() function everything is
                   supposed to be in network order and related changes.
08/02/04    mct    Included ps_ifacei_utils.h for new internal macro use.
06/11/04    vp     Removed inclusion internet.h, psglobal.h and included
                   ps_pkt_info.h. Changes for representation of IP addresses
                   as struct ps_in_addr or struct ps_in6_addr. Replaced use of
                   TCP_PTCL, UDP_PTCL etc with PS_IPPROTO_TCP, PS_IPPROTO_UDP etc.
04/27/04    usb    Added new fns ps_iface_ipfltr_process_pkt(),
                   ps_iface_ipfltr_conrtol(), support for disabling filters
                   without deleting them, API update.
03/23/04    mct    Changed ip_filter_info_type to ip_pkt_info_type.
02/18/04    usb    IP filtering library interface change, client id support,
                   network byte order support for filters, only allow
                   IPV4 pkts and other misc fixes.
10/22/03    mct    Removed dsm.h and included ps_mem.h. Removed old functions
                   PS_IFACE_IPFLTRI_ALLOC_BUF() and
                   PS_IFACE_IPFLTRI_FREE_BUF() and replaced with macros to
                   ps_mem equivalents.
07/22/03    usb    Included rex.h, wrapped q_delete under appropriate
                   feature, incorporated acl fn prototype change.
02/13/03    usb    Not calling ipfltr_delete() while adding filters, instead
                   moved delete code in ipfltr_add() to avoid duplicate event
                   generation.
02/11/03    usb    Modified MSG logs, passing num filters actually
                   added and deleted to ps_iface_ipfltr_updated_ind()
01/28/03    usb    File created.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ps_mem.h"
#include "ps_iface.h"
#include "ps_iface_ipfltr.h"
#include "ps_in.h"
#include "ps_ip6_addr.h"
#include "ps_iputil.h"
#include "ps_byte.h"
#include "ps_pkt_info.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_ifacei_utils.h"
#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"

#include "ps_ifacei.h"
#include "ps_ifacei_utils.h"
#include "ps_system_heap.h"
#include "ps_iface_addr_v6.h"
#include "sio.h"
#include "ps_iface_ipfltr_mbimi.h"
#include "ps_iface_ipfltr_hw.h"
#include "ps_ifacei_addr_v6.h"
#include "ps_ipfltri.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Defines maximum port that can be opened on a mobile
---------------------------------------------------------------------------*/
#define MAX_PORT  0xFFFF

/*---------------------------------------------------------------------------
  The IP filter information for all ifaces.
---------------------------------------------------------------------------*/
q_type global_ipfltr_info[IP_FLTR_CLIENT_MAX];

/*---------------------------------------------------------------------------
  Optimized global filter array for storing socket filters
---------------------------------------------------------------------------*/
ps_dl_opt_filter_spec_type *global_dl_opt_fltr_arr = NULL;

/*---------------------------------------------------------------------------
  Optimized global filter array to filter uplink packets on RM_GROUP
---------------------------------------------------------------------------*/
ps_dl_opt_filter_spec_type *global_ul_opt_fltr_arr = NULL;

/*---------------------------------------------------------------------------
  Tuning the number of ps iface ip filter buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_IFACE_IPFLTR_BUF_SIZE ((sizeof(ps_iface_ipfilteri_type) + 3) & ~3)

#ifdef FEATURE_DATA_PS_LOW_MEM_CHIPSET
  #define PS_IFACE_IPFLTR_BUF_NUM       20
  #define PS_IFACE_IPFLTR_BUF_HIGH_WM   16
  #define PS_IFACE_IPFLTR_BUF_LOW_WM     5

#else
  #define PS_IFACE_IPFLTR_BUF_NUM       160
  #define PS_IFACE_IPFLTR_BUF_HIGH_WM   120
  #define PS_IFACE_IPFLTR_BUF_LOW_WM    30

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_iface_ipfltr along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_iface_ipfltr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     PS_IFACE_IPFLTR_BUF_NUM,
                                     PS_IFACE_IPFLTR_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_iface_ipfltr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * ps_iface_ipfltr_buf_hdr[PS_IFACE_IPFLTR_BUF_NUM];
static ps_iface_ipfilteri_type * ps_iface_ipfltr_buf_ptr[PS_IFACE_IPFLTR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*---------------------------------------------------------------------------
ps iface ipfltr error codes
---------------------------------------------------------------------------*/
#define PS_IFACE_IPFLTR_ERR_NONE               0
#define PS_IFACE_IPFLTR_ERR_INVALID_CLIENT_ID  1
#define PS_IFACE_IPFLTR_ERR_INVALID_HANLDE     2
#define PS_IFACE_IPFLTR_ERR_INVALID_OP         3
#define PS_IFACE_IPFLTR_ERR_DELETE_FAIL        4
#define PS_IFACE_IPFLTR_ERR_INALID_PS_ERR_PTR  5

/*===========================================================================

                                    MACROS

===========================================================================*/
/*===========================================================================
MACRO IS_PORT_RANGE_VALID()

DESCRIPTION
  Validates if given range of ports is within maximum permissible port number
  i.e within 65535

  NOTE : port and range needs to be converted to uint32 since otherwise
         check will always be TRUE as values wraparound when it overflows

PARAMETERS
  port  : starting port number in network order
  range : range of ports. port + range gives ending port number

RETURN VALUE
   TRUE  : if port is <= 65535
   FALSE : otherwise
===========================================================================*/
#define IS_PORT_RANGE_VALID(port, range)                                 \
  (((((uint32) ps_ntohs(port)) + ((uint32) (range))) > MAX_PORT) ? FALSE : TRUE)

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ALLOC_BUF()

DESCRIPTION
  This function allocates a memory buffer to contain the IP filter being
  added to the iface.

DEPENDENCIES
  None.

RETURN VALUE
  Ptr to the allocated buffer, NULL if the buffer could not be allocated

SIDE EFFECTS
  None.

===========================================================================*/
#define ps_iface_ipfltri_alloc_buf()                                \
        ps_mem_get_buf(PS_MEM_PS_IFACE_IPFLTER_TYPE)

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_FREE_BUF()

DESCRIPTION
  This function frees up the memory buffer coantaining the IP
  filter in the iface.

DEPENDENCIES
  The buffer should have been created using ps_iface_ipfltri_alloc_buf()
  and it should not be on a queue.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
#define ps_iface_ipfltri_free_buf( mi_ptr_ptr )                          \
        PS_MEM_FREE(mi_ptr_ptr)


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_IPV4_PARAM()

DESCRIPTION
  Validates IPV4 parameters of a filter

PARAMETERS
  client_id     : Filtering client id
  fltr_ptr      : ptr to a filter
  next_hdr_prot : OUT param indicating the higher level protocol

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If field mask is IPFLTR_MASK_IP4_NONE, errmask is set to
  IPFLTR_MASK_IP4_ALL

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_ipv4_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr,
  ps_ip_protocol_enum_type             * next_hdr_prot
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->ip_hdr.v4.err_mask = 0;

  if (fltr_ptr->ip_hdr.v4.field_mask == IPFLTR_MASK_IP4_NONE)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_validate_ipv4_param(): "
                    "Invalid IP_V4 field mask: 0x%x",
                    fltr_ptr->ip_hdr.v4.field_mask);
    fltr_ptr->ip_hdr.v4.err_mask = IPFLTR_MASK_IP4_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v4.field_mask & ~IPFLTR_MASK_IP4_ALL)
  {
    fltr_ptr->ip_hdr.v4.err_mask =
      (fltr_ptr->ip_hdr.v4.field_mask & ~IPFLTR_MASK_IP4_ALL);
    is_fltr_valid = FALSE;
  }

  /*-----------------------------------------------------------------------
    If Source address is used in a Tx filter or if Destination address is
    used in a Rx filter, make sure that only a single address value is
    specified. Filters are installed per iface and an iface can have only
    one IP address
    CLAT is an exception since the filters can match a range of addresses
  -----------------------------------------------------------------------*/
  if (client_id != IP_FLTR_CLIENT_CLAT)
  {
    if (client_id >= IP_FLTR_CLIENT_INPUT_MAX)
    {
      if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR) &&
          fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr != 0xFFFFFFFFUL)
      {
        fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
        is_fltr_valid = FALSE;
      }
    }
    else
    {
      if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR) &&
          (fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr != 0xFFFFFFFFUL))
      {
        fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_DST_ADDR;
        is_fltr_valid = FALSE;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Make sure that only one of TCP, UDP, or ICMP is specified as higher layer
    protocol in a filter. Also make sure that ICMP filters are not specified
    by QoS clients
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
  {
    if (fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_UDP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ICMP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ICMP6 ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_IGMP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ESP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_AH ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP)
    {
      *next_hdr_prot =
        (ps_ip_protocol_enum_type) fltr_ptr->ip_hdr.v4.next_hdr_prot;
    }
    else
    {
      fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      is_fltr_valid = FALSE;
    }
  }
  else if (client_id == IP_FLTR_CLIENT_SOCKETS)
  {
    fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS) &&
      fltr_ptr->ip_hdr.v4.tos.mask == 0)
  {
    fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_TOS;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_2("ps_iface_ipfltri_validate_ipv4_param(): "
                    "IP_FLTR V4 field_mask = 0x%x, IP_FLTR V4 err_mask = 0x%x",
                    fltr_ptr->ip_hdr.v4.field_mask,
                    fltr_ptr->ip_hdr.v4.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_ipv4_param() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_IPV6_PARAM()

DESCRIPTION
  Validates IPV6 parameters of a filter

PARAMETERS
  client_id     : Filtering client id
  fltr_ptr      : ptr to a filter
  next_hdr_prot : OUT param indicating the higher level protocol

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If field mask is IPFLTR_MASK_IP6_NONE, errmask is set to
  IPFLTR_MASK_IP6_ALL

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_ipv6_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr,
  ps_ip_protocol_enum_type             * next_hdr_prot
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->ip_hdr.v6.err_mask = 0;

  if (fltr_ptr->ip_hdr.v6.field_mask == IPFLTR_MASK_IP6_NONE)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_validate_ipv6_param(): "
                    "Invalid IP_V6 field mask: 0x%x",
                    fltr_ptr->ip_hdr.v6.field_mask);
    fltr_ptr->ip_hdr.v6.err_mask = IPFLTR_MASK_IP6_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v6.field_mask & ~IPFLTR_MASK_IP6_ALL)
  {
    fltr_ptr->ip_hdr.v6.err_mask =
      (fltr_ptr->ip_hdr.v6.field_mask & ~IPFLTR_MASK_IP6_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    Fail if either of source or destination address is V4 mapped V6
  -------------------------------------------------------------------------*/
  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR) &&
      PS_IN6_IS_ADDR_V4MAPPED(&fltr_ptr->ip_hdr.v6.src.addr))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR) &&
      PS_IN6_IS_ADDR_V4MAPPED(&fltr_ptr->ip_hdr.v6.dst.addr))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_DST_ADDR;
    is_fltr_valid = FALSE;
  }

  /*-----------------------------------------------------------------------
    If Source address is used in a Tx filter or if Destination address is
    used in a Rx filter, make sure that only a single address value is
    specified. Filters are installed per iface and an iface can have only
    one IP address
  -----------------------------------------------------------------------*/
  if (client_id >= IP_FLTR_CLIENT_INPUT_MAX)
  {
    if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR) &&
        fltr_ptr->ip_hdr.v6.src.prefix_len > 128)
    {
      fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      is_fltr_valid = FALSE;
    }
  }
  else
  {
    if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR) &&
        fltr_ptr->ip_hdr.v6.dst.prefix_len > 128)
    {
      fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      is_fltr_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Make sure that only one of TCP, UDP, or ICMP is specified as higher layer
    protocol in a filter
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
  {
    if (fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_UDP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ICMP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ICMP6 ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ESP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_AH ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP)
    {
      *next_hdr_prot =
        (ps_ip_protocol_enum_type) fltr_ptr->ip_hdr.v6.next_hdr_prot;
    }
    else
    {
      fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      is_fltr_valid = FALSE;
    }
  }
  else if (client_id == IP_FLTR_CLIENT_SOCKETS)
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS) &&
      fltr_ptr->ip_hdr.v6.trf_cls.mask == 0)
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    IPV6 flow label already specifies flow ID. No need to further classify
    this packet
  -------------------------------------------------------------------------*/
  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL) &&
      (*next_hdr_prot != PS_NO_NEXT_HDR))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= (IPFLTR_MASK_IP6_NEXT_HDR_PROT |
                                     IPFLTR_MASK_IP6_FLOW_LABEL);
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_2("ps_iface_ipfltri_validate_ipv6_param(): "
                    "IP_FLTR V6 field_mask = 0x%x, IP_FLTR V6 err_mask = 0x%x",
                    fltr_ptr->ip_hdr.v6.field_mask,
                    fltr_ptr->ip_hdr.v6.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_ipv6_param() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_TCP_PARAM()

DESCRIPTION
  Validates TCP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_tcp_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.tcp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp.field_mask & ~IPFLTR_MASK_TCP_ALL)
  {
    fltr_ptr->next_prot_hdr.tcp.err_mask =
      (fltr_ptr->next_prot_hdr.tcp.field_mask & ~IPFLTR_MASK_TCP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    TCP ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp.src.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.tcp.src.port,
                             fltr_ptr->next_prot_hdr.tcp.src.range))
    {
      fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.tcp.dst.port,
                             fltr_ptr->next_prot_hdr.tcp.dst.range))
    {
      fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       (IP_FLTR_CLIENT_SOCKETS == client_id || 
        IP_FLTR_CLIENT_FIREWALL_INPUT == client_id))
  {
    fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_SRC_PORT;
    fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_3("ps_iface_ipfltri_validate_tcp_param(): "
                    "IP_FLTR TCP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR TCP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.tcp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.tcp.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_tcp_param() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_UDP_PARAM()

DESCRIPTION
  Validates UDP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_udp_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.udp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.udp.field_mask & ~IPFLTR_MASK_UDP_ALL)
  {
    fltr_ptr->next_prot_hdr.udp.err_mask =
      (fltr_ptr->next_prot_hdr.udp.field_mask & ~IPFLTR_MASK_UDP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    UDP ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.udp.src.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.udp.src.port,
                             fltr_ptr->next_prot_hdr.udp.src.range))
    {
      fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.udp.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.udp.dst.port,
                             fltr_ptr->next_prot_hdr.udp.dst.range))
    {
      fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       (IP_FLTR_CLIENT_SOCKETS == client_id || 
        IP_FLTR_CLIENT_FIREWALL_INPUT == client_id))
  {
    fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_SRC_PORT;
    fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_3("ps_iface_ipfltri_validate_udp_param(): "
                    "IP_FLTR UDP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR UDP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.udp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.udp.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_udp_param() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_ICMP_PARAM()

DESCRIPTION
  Validates the ICMP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
   TRUE  : if parameters are valid
   FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with the appropriate error codes for values and enums
  which failed validation.

  If a bit mask, which is outside the supported range, is specified, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_icmp_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.icmp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask, which is outside the supported range, is specified flag
    error continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.icmp.field_mask & ~IPFLTR_MASK_ICMP_ALL)
  {
    fltr_ptr->next_prot_hdr.icmp.err_mask =
      (fltr_ptr->next_prot_hdr.icmp.field_mask & ~IPFLTR_MASK_ICMP_ALL);
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_3("ps_iface_ipfltri_validate_icmp_param(): "
                    "IP_FLTR ICMP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR ICMP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.icmp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.icmp.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_icmp_param() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_VALIDATE_TCP_UDP_PARAM()

DESCRIPTION
  Validates Transport parameters of a filter when the next header is not
  specified.

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ps_iface_ipfltri_validate_tcp_udp_param
(
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        ~IPFLTR_MASK_TCP_UDP_ALL)
  {
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask =
      (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
       ~IPFLTR_MASK_TCP_UDP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    All ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        IPFLTR_MASK_TCP_UDP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.port == 0 ||
        !IS_PORT_RANGE_VALID(
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.port,
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.range))
    {
      fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
        IPFLTR_MASK_TCP_UDP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        IPFLTR_MASK_TCP_UDP_DST_PORT) 
  {  
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.port,
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.range))
    {
      fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
        IPFLTR_MASK_TCP_UDP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       (IP_FLTR_CLIENT_SOCKETS == client_id || 
        IP_FLTR_CLIENT_FIREWALL_INPUT == client_id ))
  {
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
      IPFLTR_MASK_TCP_UDP_SRC_PORT;
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
      IPFLTR_MASK_TCP_UDP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    LOG_MSG_INFO1_3("ps_iface_ipfltri_validate_tcp_udp_param(): "
                    "IP_FLTR TRANSPORT PROTOCOL field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR TRANSPORT PROTOCOL err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask);
  }

  return is_fltr_valid;

} /* ps_iface_ipfltri_validate_tcp_udp_param() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_PROCESS_DEFAULT()

DESCRIPTION
  This function executes a specified filter on a specified IP pkt information
  block using the default set of rules.

DEPENDENCIES
  - Filter and pakcet must have the same IP version.
  - If a field in the next hdr is specified as a filter parameter,
    next_hdr_prot field in the IP hdr must be set. This means that ip hdr
    field mask will always be non null since filter should have at least
    one parameter specified.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ps_iface_ipfltri_process_default
(
  ip_filter_type       *fi_ptr,          /* ptr to filter to process       */
  ip_pkt_info_type     *info_ptr         /* ptr to info to apply filter on */
)
{
  uint32 tmp_mask;
  uint8  next_hdr_prot = PS_NO_NEXT_HDR;
  uint8 tmp_prot;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == fi_ptr || NULL == info_ptr)
  {
    LOG_MSG_ERROR_2("ps_iface_ipfltri_process_default(): "
                    "Bad args: fi_ptr 0x%p, info_ptr 0x%p", fi_ptr, info_ptr);
    ASSERT( 0 );
    return FALSE;
  }

  if (fi_ptr->ip_vsn != info_ptr->ip_vsn)
  {
    LOG_MSG_ERROR_2("ps_iface_ipfltri_process_default(): "
                    "Filter address family %d differs from pkt address family %d",
                    fi_ptr->ip_vsn, info_ptr->ip_vsn);
    ASSERT( 0 );
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Algorithm:
    - Based on the IP version process corresponding IP hdr parameters.
    - If parameters for higher level protocol are specified, v4 protocol
    field or v6 next header field must be set in the filter.  Extract next
    header protocol value and amke sure it matches with that of the packet.
    - Next process the next header protocol parameters

    If any parameter specified in the filter fails a match, fail the filter
    execution.  If all specified filter parametrs match the pkt, filter
    is passed.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Note the while loop over a tmp_mask for IP hdr processing as an
    optimization.  More likely parameters should be checked first inside
    the while loop.
  -------------------------------------------------------------------------*/

  switch (fi_ptr->ip_vsn)
  {
    case IP_V4:
    {
      tmp_mask = fi_ptr->ip_hdr.v4.field_mask;

      while (tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP4_SRC_ADDR)
        {
          if ((fi_ptr->ip_hdr.v4.src.addr.ps_s_addr &
              fi_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
             (info_ptr->ip_hdr.v4.source.ps_s_addr &
              fi_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_DST_ADDR)
        {
          if ((fi_ptr->ip_hdr.v4.dst.addr.ps_s_addr &
              fi_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr) !=
             (info_ptr->ip_hdr.v4.dest.ps_s_addr &
              fi_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_DST_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
        {
          if (fi_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP)
          {
            if (info_ptr->ip_hdr.v4.protocol != PS_IPPROTO_TCP &&
                info_ptr->ip_hdr.v4.protocol != PS_IPPROTO_UDP)
            {
              return FALSE;
            }
          }
          else if (fi_ptr->ip_hdr.v4.next_hdr_prot !=
                     info_ptr->ip_hdr.v4.protocol)
          {
            return FALSE;
          }

          next_hdr_prot = fi_ptr->ip_hdr.v4.next_hdr_prot;

          tmp_mask &= ~IPFLTR_MASK_IP4_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_TOS)
        {
          if ((fi_ptr->ip_hdr.v4.tos.val & fi_ptr->ip_hdr.v4.tos.mask) !=
             (info_ptr->ip_hdr.v4.tos & fi_ptr->ip_hdr.v4.tos.mask))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_TOS;
          continue;
        }

        if (0 != tmp_mask)
        {
          LOG_MSG_ERROR_1("ps_iface_ipfltri_process_default(): "
                          "Unknown v4 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
    {
      tmp_mask = fi_ptr->ip_hdr.v6.field_mask;
      while (tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP6_SRC_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL
                       (
                         &fi_ptr->ip_hdr.v6.src.addr,
                         &info_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr,
                         fi_ptr->ip_hdr.v6.src.prefix_len)
                       )
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_DST_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL
                       (
                         &fi_ptr->ip_hdr.v6.dst.addr,
                         &info_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr,
                         fi_ptr->ip_hdr.v6.dst.prefix_len)
                       )
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_DST_ADDR;
          continue;
        }

        /*-------------------------------------------------------------------
          Next hdr for IPv6 is transport protocol type
        -------------------------------------------------------------------*/
        if (tmp_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
        {
          if (fi_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP)
          {
            if (info_ptr->ip_hdr.v6.hdr_type != PS_IPPROTO_TCP &&
                info_ptr->ip_hdr.v6.hdr_type != PS_IPPROTO_UDP)
            {
              return FALSE;
            }
          }
          else if (fi_ptr->ip_hdr.v6.next_hdr_prot !=
                     info_ptr->ip_hdr.v6.hdr_type)
          {
            return FALSE;
          }

          next_hdr_prot = fi_ptr->ip_hdr.v6.next_hdr_prot;
          tmp_mask &= ~IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
        {
          if ((fi_ptr->ip_hdr.v6.trf_cls.val &
                 fi_ptr->ip_hdr.v6.trf_cls.mask) !=
              (info_ptr->ip_hdr.v6.hdr_body.base_hdr.trf_cls &
                 fi_ptr->ip_hdr.v6.trf_cls.mask))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_TRAFFIC_CLASS;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
        {
          if (fi_ptr->ip_hdr.v6.flow_label !=
               info_ptr->ip_hdr.v6.hdr_body.base_hdr.flow_cls)
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_FLOW_LABEL;
          continue;
        }

        if (0 != tmp_mask)
        {
          LOG_MSG_ERROR_1("ps_iface_ipfltri_process_default(): "
                          "Unknown v6 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      /* unsupported IP version */
      ASSERT(0);
      return FALSE;
  }

  if (next_hdr_prot == PS_NO_NEXT_HDR)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Optimization done for IP hdrs above may also be used for protocol hdr
    processing when more than 2 parameters can be specified for a protocol.
    For 2 or less parameters the scheme is less optimal and hence avoided.
  -------------------------------------------------------------------------*/
  switch (next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.tcp.field_mask))
      {
        break;
      }

      if ((tmp_mask & IPFLTR_MASK_TCP_SRC_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.tcp.src.port)) >
            (ps_ntohs(info_ptr->ptcl_info.tcp.src_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.tcp.src.port)) +
            fi_ptr->next_prot_hdr.tcp.src.range <
            (ps_ntohs(info_ptr->ptcl_info.tcp.src_port))))
      {
        return FALSE;
      }

      if ((tmp_mask & IPFLTR_MASK_TCP_DST_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.tcp.dst.port)) >
            (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.tcp.dst.port)) +
            fi_ptr->next_prot_hdr.tcp.dst.range <
            (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port))))
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_UDP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.udp.field_mask))
      {
        break;
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_SRC_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.udp.src.port)) >
            (ps_ntohs(info_ptr->ptcl_info.udp.src_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.udp.src.port)) +
            fi_ptr->next_prot_hdr.udp.src.range <
            (ps_ntohs(info_ptr->ptcl_info.udp.src_port))))
      {
        return FALSE;
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_DST_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.udp.dst.port)) >
            (ps_ntohs(info_ptr->ptcl_info.udp.dst_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.udp.dst.port)) +
            fi_ptr->next_prot_hdr.udp.dst.range <
            (ps_ntohs(info_ptr->ptcl_info.udp.dst_port))))
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.icmp.field_mask))
      {
        break;
      }

      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_TYPE &&
         fi_ptr->next_prot_hdr.icmp.type != info_ptr->ptcl_info.icmp.type)
      {
        return FALSE;
      }

      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_CODE &&
         fi_ptr->next_prot_hdr.icmp.code != info_ptr->ptcl_info.icmp.code)
      {
        return FALSE;
      }
      
      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_ID &&
         fi_ptr->next_prot_hdr.icmp.id != info_ptr->ptcl_info.icmp.id)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_ESP:
    {
      if (IPFLTR_MASK_ESP_NONE == fi_ptr->next_prot_hdr.esp.field_mask)
      {
        break;
      }

      tmp_mask = fi_ptr->next_prot_hdr.esp.field_mask;

      if (tmp_mask & IPFLTR_MASK_ESP_SPI &&
          fi_ptr->next_prot_hdr.esp.spi != info_ptr->esp_hdr.spi)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_AH:
    {
      if (IPFLTR_MASK_AH_NONE == fi_ptr->next_prot_hdr.ah.field_mask)
      {
        break;
      }

      tmp_mask = fi_ptr->next_prot_hdr.ah.field_mask;

      if (tmp_mask & IPFLTR_MASK_AH_SPI &&
          fi_ptr->next_prot_hdr.ah.spi != info_ptr->ah_hdr.spi)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_TCP_UDP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.tcp_udp_port_range.field_mask))
      {
        break;
      }

      if (fi_ptr->ip_vsn == IP_V4)
      {
        tmp_prot = info_ptr->ip_hdr.v4.protocol;
      }
      else
      {
        tmp_prot = info_ptr->ip_hdr.v6.hdr_type;
      }

      switch (tmp_prot)
      {
        case PS_IPPROTO_TCP:
          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) >
                (ps_ntohs(info_ptr->ptcl_info.tcp.src_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.src.range <
                (ps_ntohs(info_ptr->ptcl_info.tcp.src_port))))
          {
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) >
              (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.range <
                (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port))))
          {
            return FALSE;
          }
        break;

        case PS_IPPROTO_UDP:
          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) >
                (ps_ntohs(info_ptr->ptcl_info.udp.src_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.src.range <
                (ps_ntohs(info_ptr->ptcl_info.udp.src_port))))
          {
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) >
                (ps_ntohs(info_ptr->ptcl_info.udp.dst_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.range <
                (ps_ntohs(info_ptr->ptcl_info.udp.dst_port))))
          {
            return FALSE;
          }
        break;

        default:
          break;
      }
    }
  break;

  default:
    /* unsupported protocol */
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If we reach here, all the specified parameters matached
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ps_iface_ipfltri_process_default() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_EXECUTE()

DESCRIPTION Internal function to execute filters on iface or global ip filter
            based on group

DEPENDENCIES

RETURN VALUE
  filter result on successful match
  PS_IFACE_IPFLTR_NOMATCH on failure

SIDE EFFECTS
  None.

===========================================================================*/
static ps_iface_ipfltr_result_type ps_iface_ipfltri_execute
(
  ps_iface_type                       *iface_ptr,
  ps_iface_name_enum_type             iface_group,
  ps_iface_ipfltr_client_id_enum_type client_id,
  ps_iface_ipfltr_subset_id_type      subset_id,
  ip_pkt_info_type                    *ip_pkt_info_ptr
)
{
  q_type                       * q_ptr;
  ps_iface_ipfilteri_type      * filter_buf_ptr;
  ps_iface_ipfltr_result_type    result = PS_IFACE_IPFLTR_NOMATCH;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_3("ps_iface_ipfltr_execute(): "
                  "Executing fltrs if 0x%p, client %d, group 0x%x",
                  iface_ptr, client_id, iface_group);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (iface_ptr != NULL && !PS_IFACE_IS_VALID(iface_ptr))
  {
    ASSERT(0);
    return result;
  }

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_execute(): "
                    "Invalid filtering client id %d", client_id);
    return result;
  }

  if (ip_pkt_info_ptr == NULL)
  {
    LOG_MSG_ERROR_2("ps_iface_ipfltri_execute(): "
                    "If 0x%p client %d null pkt info", iface_ptr, client_id);
    return result;
  }
  
  if (iface_group < ANY_DEFAULT_GROUP || iface_group > ANY_IFACE_GROUP )
   {
     LOG_MSG_ERROR_1("ps_iface_ipfltri_execute(): incorrect iface group 0x%x",
                      iface_group);
    return result;
   }

  /*-------------------------------------------------------------------------
    Check if pkt info is same as current IP family of the ps_iface.
  -------------------------------------------------------------------------*/
  if (iface_ptr != NULL &&
      ps_iface_get_addr_family(iface_ptr) !=
        (ps_iface_addr_family_type) ip_pkt_info_ptr->ip_vsn)
  {
    LOG_MSG_ERROR_2("ps_iface_ipfltri_execute(): "
                    "If 0x%p client %d pkt ip version mismatch",
                    iface_ptr, client_id);
    return result;
  }

#ifndef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    if IPv6 is not defined don't execute the v6 filters.
  -------------------------------------------------------------------------*/
  if (ip_pkt_info_ptr->ip_vsn != IP_V4)
  {
    return result;
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (iface_ptr == NULL)
  {
    q_ptr = &(global_ipfltr_info[client_id]);
  }
  else
  {
    q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  }

  for (filter_buf_ptr = q_check(q_ptr);
       filter_buf_ptr != NULL;
       filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link)))
  {
    /*-----------------------------------------------------------------------
      Skip the filters which are disabled.
      Skip the filters that doesn't belong to same subset_id. If subset_id is
      PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT, go through all filters
    -----------------------------------------------------------------------*/
    if (!filter_buf_ptr->disabled &&
        (subset_id == filter_buf_ptr->subset_id ||
         subset_id == PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT) &&
         filter_buf_ptr->iface_group == iface_group)
    {
      switch (filter_buf_ptr->filter_type)
      {
      case IPFLTR_DEFAULT_TYPE:
        /*-------------------------------------------------------------------
          For default filter, process the filter info to see if required
          fields match.  Only execute the filter which actually matches the
          IP version of the received packet, skip others.
        -------------------------------------------------------------------*/
        if (filter_buf_ptr->filter.fi_default.ip_vsn != ip_pkt_info_ptr->ip_vsn)
        {
          break;
        }

        if (ps_iface_ipfltri_process_default(
             &(filter_buf_ptr->filter.fi_default),
             ip_pkt_info_ptr) == TRUE)
        {
          result = filter_buf_ptr->fi_result;
          LOG_MSG_INFO2_3("ps_iface_ipfltri_execute(): "
                          "Fltr passed if 0x%p, client %d, fi_result 0x%x",
                          iface_ptr, client_id, result);
        }
        break;

      default:
        LOG_MSG_ERROR_1("ps_iface_ipfltri_execute(): "
                        "Invalid filter type %d", filter_buf_ptr->filter_type);
        ASSERT(0);
        break;
      }

      if (result != PS_IFACE_IPFLTR_NOMATCH)
      {
        break;
      }
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return result;
}   /* ps_iface_ipfltri_execute() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ADD_OPT_FILTERS()

DESCRIPTION
  This function adds filters for optimized data path.

PARAMETERS
  iface_ptr          : Iface to add filters to
  fltr_arr           : Filtering specification
  num_filters        : Number of filters to be added
  fi_handle          : Handle associated with this filter

RETURN VALUE
  TRUE if success, FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_iface_ipfltri_add_opt_filter
(
  ps_iface_type                  * iface_ptr,
  ps_iface_name_enum_type          iface_group,
  ip_filter_type                 * fltr_arr,
  uint32                           num_filters,
  ps_iface_ipfltr_handle_type      fi_handle
)
{
  ps_dl_opt_filter_spec_type     * opt_fltr_ptr;
  ps_iface_type                  * base_iface_ptr = NULL;
  ps_iface_type                  * assoc_iface_ptr;
  int32                            opt_fltr_arr_size;
  uint32                           max_fltr;
  uint32                           fltr_index = 0;
  uint8                            opt_fltr_count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get address of opt filter array
      1. If iface is NULL, address points to global opt filter array
      2. Else, address points to opt filter array of the base iface of input
         iface. It must be base iface as DL opt handler runs on physical iface
         and not logical iface
  -------------------------------------------------------------------------*/
  if (iface_ptr == NULL)
  {
    if (iface_group == RM_GROUP)
    {
      opt_fltr_ptr = global_ul_opt_fltr_arr;
    }
    else
    {    
    opt_fltr_ptr = global_dl_opt_fltr_arr;
    }
    max_fltr     = MAX_GLOBAL_OPT_FILTER_ENTRIES;
  }
  else
  {
    if (PS_IFACE_IS_LOGICAL(iface_ptr))
    {
      assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(iface_ptr);

      do
      {
        base_iface_ptr  = assoc_iface_ptr;
        assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(base_iface_ptr);
      } while (PS_IFACE_IS_VALID(assoc_iface_ptr));

      if (!PS_IFACE_IS_VALID(base_iface_ptr))
      {
        LOG_MSG_INFO1_2("Not adding opt fltrs as base of 0x%x:%d "
                        "is invalid", iface_ptr->name, iface_ptr->instance);
        return -1;
      }
    }
    else
    {
      base_iface_ptr = iface_ptr;
    }

    opt_fltr_ptr = PS_IFACEI_GET_OPT_FILTER_BASE(base_iface_ptr);
    max_fltr     = PS_IFACE_MAX_OPT_FILTER_ENTRIES;
  }

  /*-------------------------------------------------------------------------
    If opt fltr array is not already allocated, allocate it
  -------------------------------------------------------------------------*/
  if (opt_fltr_ptr == NULL)
  {
    opt_fltr_arr_size = sizeof(ps_dl_opt_filter_spec_type) * max_fltr;

    PS_SYSTEM_HEAP_MEM_ALLOC(opt_fltr_ptr, opt_fltr_arr_size, 
                             ps_dl_opt_filter_spec_type*);
    if (opt_fltr_ptr == NULL)
    {
      return -1;
    }

    memset(opt_fltr_ptr, 0, opt_fltr_arr_size);

    if (NULL == iface_ptr)
    {
      if (iface_group == RM_GROUP)
      {
        global_ul_opt_fltr_arr = opt_fltr_ptr;
      }
      else
      {      
      global_dl_opt_fltr_arr = opt_fltr_ptr;
    }
    }
    else
    {
      PS_IFACEI_SET_OPT_FILTER_BASE(base_iface_ptr, opt_fltr_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Find the first empty slot in the opt filter array and use it to add
    opt filter
  -------------------------------------------------------------------------*/
  for (opt_fltr_count = 0;
       opt_fltr_count < max_fltr && fltr_index < num_filters;
       opt_fltr_count++, opt_fltr_ptr++)
  {
    if (opt_fltr_ptr->fi_handle != 0)
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Add TCP or UDP or TCP_UDP based filters to opt filter array
    -----------------------------------------------------------------------*/
    if (fltr_arr[fltr_index].ip_vsn == IP_V4)
    {
      opt_fltr_ptr->protocol = fltr_arr[fltr_index].ip_hdr.v4.next_hdr_prot;
    }
    else
    {
      opt_fltr_ptr->protocol = fltr_arr[fltr_index].ip_hdr.v6.next_hdr_prot;
    }

    if (opt_fltr_ptr->protocol == PS_IPPROTO_TCP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.tcp.field_mask &
            IPFLTR_MASK_TCP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.tcp.field_mask &
            IPFLTR_MASK_TCP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if (opt_fltr_ptr->protocol == PS_IPPROTO_UDP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.udp.field_mask &
            IPFLTR_MASK_UDP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.udp.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.udp.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.udp.field_mask &
            IPFLTR_MASK_UDP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.udp.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.udp.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if (opt_fltr_ptr->protocol == PS_IPPROTO_TCP_UDP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if ( (opt_fltr_ptr->protocol == PS_IPPROTO_ICMP) ||
              (opt_fltr_ptr->protocol == PS_IPPROTO_IGMP) )
    {
      opt_fltr_ptr->src_port = 0;
      opt_fltr_ptr->src_port_range = 0;
      opt_fltr_ptr->dst_port = 0;
      opt_fltr_ptr->dst_port_range = 0;
    }
    else
    {
      fltr_index++;
      /*---------------------------------------------------------------------
        Only TCP, UDP, TCP_UDP, ICMP, IGMP  based filters are supported 
        currently inoptimized path. So, reset protocol and move to next filter
      ---------------------------------------------------------------------*/
      opt_fltr_ptr->protocol = 0;
      continue;
    }

    fltr_index++;
    opt_fltr_ptr->fi_handle = fi_handle;
  } /* end for all filters */

  if (fltr_index < num_filters)
  {
    LOG_MSG_ERROR_2("Out of filter space in opt path. "
                    "Added (%d) out of (%d)", fltr_index, num_filters);
    return -1;
  }

  return 0;

} /* ps_iface_ipfltri_add_opt_filter() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_DELETE_OPT_FILTERS()

DESCRIPTION
  This function deletes filters from optimized data path.

PARAMETERS
  iface_ptr          : Iface to add filters to
  fi_handle          : Handle associated with this filter

RETURN VALUE
  TRUE if success, FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_iface_ipfltri_delete_opt_filter
(
  ps_iface_type                  * iface_ptr,
  ps_iface_name_enum_type          iface_group,
  ps_iface_ipfltr_handle_type      fi_handle
)
{
  ps_dl_opt_filter_spec_type * opt_fltr_ptr;
  ps_dl_opt_filter_spec_type * nxt_opt_fltr_ptr;
  ps_dl_opt_filter_spec_type * tmp_opt_fltr_ptr;
  ps_dl_opt_filter_spec_type   tmp_opt_fltr;
  ps_iface_type              * base_iface_ptr;
  ps_iface_type              * assoc_iface_ptr;
  uint32                       max_fltr;
  uint8                        opt_fltr_count;
  uint8                        swap_fltr_count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get address of opt filter array
      1. If iface is NULL, address points to global opt filter array
      2. Else, address points to opt filter array of the base iface of input
         iface. It must be base iface as DL opt handler runs on physical iface
         and not logical iface
  -------------------------------------------------------------------------*/
  if (iface_ptr == NULL)
  {
    if (iface_group == RM_GROUP)
    {
      opt_fltr_ptr = global_ul_opt_fltr_arr;
    }
    else
    {    
    opt_fltr_ptr = global_dl_opt_fltr_arr;
    }
    max_fltr     = MAX_GLOBAL_OPT_FILTER_ENTRIES;
  }
  else
  {
    if (PS_IFACE_IS_LOGICAL(iface_ptr))
    {
      assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(iface_ptr);

      do
      {
        base_iface_ptr  = assoc_iface_ptr;
        assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(base_iface_ptr);
      } while (PS_IFACE_IS_VALID(assoc_iface_ptr));

      if (!PS_IFACE_IS_VALID(base_iface_ptr))
      {
        LOG_MSG_INFO1_2("Not deleting opt fltrs as base of 0x%x:%d "
                        "is invalid", iface_ptr->name, iface_ptr->instance);
        return;
      }
    }
    else
    {
      base_iface_ptr = iface_ptr;
    }

    opt_fltr_ptr = PS_IFACEI_GET_OPT_FILTER_BASE(base_iface_ptr);
    max_fltr     = PS_IFACE_MAX_OPT_FILTER_ENTRIES;
  }

  if (opt_fltr_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Delete all the filters with filter handle as "fi_handle". Remove hole
      in opt filter array by shifting entries to left so that one doesn't have
      to traverse the whole array to look at valid opt filter entries
    -----------------------------------------------------------------------*/
    for (opt_fltr_count = 0;
         opt_fltr_count < max_fltr && opt_fltr_ptr->fi_handle != 0;
         opt_fltr_count++)
    {
      if (opt_fltr_ptr->fi_handle == fi_handle)
      {
        swap_fltr_count  = opt_fltr_count;
        tmp_opt_fltr_ptr = opt_fltr_ptr;
        nxt_opt_fltr_ptr = opt_fltr_ptr + 1;

        while (swap_fltr_count < (max_fltr - 1) &&
               nxt_opt_fltr_ptr->fi_handle != 0)
        {
          /*-----------------------------------------------------------------
            Swap the entries
          -----------------------------------------------------------------*/
          memscpy(&tmp_opt_fltr,
                  sizeof(ps_dl_opt_filter_spec_type),
                  nxt_opt_fltr_ptr,
                  sizeof(ps_dl_opt_filter_spec_type));
          memscpy(nxt_opt_fltr_ptr,
                  sizeof(ps_dl_opt_filter_spec_type),
                  tmp_opt_fltr_ptr,
                  sizeof(ps_dl_opt_filter_spec_type));
          memscpy(tmp_opt_fltr_ptr,
                  sizeof(ps_dl_opt_filter_spec_type),
                  &tmp_opt_fltr,
                  sizeof(ps_dl_opt_filter_spec_type));

          tmp_opt_fltr_ptr = nxt_opt_fltr_ptr;
          swap_fltr_count++;
          nxt_opt_fltr_ptr++;
        }

        memset(tmp_opt_fltr_ptr, 0, sizeof(ps_dl_opt_filter_spec_type));

        /*-------------------------------------------------------------------
          Since hole is filled by shifting entry left and since this entry
          hasn't been processed yet, don't increment opt_fltr_ptr
        -------------------------------------------------------------------*/
      } /* end if matched */
      else
      {
        opt_fltr_ptr++;
      }
    } /* end for all filters */
  } /* end if opt filters exist on this iface */

  return;
} /* ps_iface_ipfltri_delete_opt_filter() */




/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ADD_FILTER()

DESCRIPTION Internal function to add filters on iface or global ip filter
            based on group

DEPENDENCIES

RETURN VALUE
  ps_iface_ipfltr_handle on successful filter addition
  PS_IFACE_IPFLTR_INVALID_HANDLE on failure 

SIDE EFFECTS
  None.

===========================================================================*/
static ps_iface_ipfltr_handle_type ps_iface_ipfltri_add
(
  ps_iface_type                         * iface_ptr,
  ps_iface_name_enum_type                 iface_group,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  const ps_iface_ipfltr_add_param_type  * fltr_param_ptr,
  int16                                 * ps_errno
)
{
  q_type                         * ipfltr_q_ptr;
  ps_iface_ipfilteri_type        * new_filter_buf_ptr = NULL;
  ps_iface_ipfilteri_type        * tmp_tx_fltr_buf = NULL;
  ps_iface_ipfltr_handle_type      fltr_handle;
  int                              prev_cnt = 0;
  uint8                            fltr_index;
  boolean                          install_hw_fltr = FALSE;
  ip_filter_mbim_type            * mbim_filter_ptr = NULL;
  ip_filter_mbim_type            * mbim_filter_arr_ptr = NULL;
  ps_ifacei_v6_addr_type         * v6_iface_addr_ptr = NULL;
  boolean                          retval = FALSE;  
  ip_filter_type                 * fltr_ptr = NULL;
  struct ps_in6_addr               v6_addr;  
  ps_iface_ipfltr_change_type      ipsec_fltrs = {0};
  ps_iface_type                  * base_iface_ptr = NULL; 
  sint15                           ps_err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_iface_ipfltri_add(): "
                  "FLTR ADD called by client %d, iface 0x%p group 0x%x",
                  client_id, iface_ptr, iface_group);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (ps_errno == NULL)
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltri_add(): "
                    "NULL parameter is passed");
    ASSERT(0);
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltri_add(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->filter_type >= IPFLTR_MAX_TYPE)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltri_add(): "
                    "Invalid filter type by client %d", client_id);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fi_ptr_arr == NULL || fltr_param_ptr->num_filters == 0)
  {
    LOG_MSG_INFO1_1("ps_iface_ipfltri_add(): "
                    "No filter specified by client %d", client_id);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fi_result == PS_IFACE_IPFLTR_NOMATCH)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltri_add(): Invallid filter_result"
                    "specified by client %d", client_id);
    *ps_errno = DS_EFAULT;
    ASSERT(0);
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fltr_priority != PS_IFACE_IPFLTR_PRIORITY_FCFS)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltr_add(): Invalid fltr add style %d",
                    fltr_param_ptr->fltr_priority);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  if (iface_group < ANY_DEFAULT_GROUP || iface_group > ANY_IFACE_GROUP )
  {
    *ps_errno = DS_EFAULT;
     LOG_MSG_ERROR_1("ps_iface_ipfltri_add(): incorrect iface group 0x%x",
                      iface_group);
    return PS_IFACE_IPFLTR_INVALID_HANDLE;
  }

  *ps_errno = DS_ENOERR;

  /*-------------------------------------------------------------------------
    Validate filters if they aren't already validated
  -------------------------------------------------------------------------*/
  if (!fltr_param_ptr->is_validated)
  {
    if (IPFLTR_DEFAULT_TYPE == fltr_param_ptr->filter_type)
    {
      if (!ps_iface_ipfltr_validate_fltr_param(client_id,
                                               fltr_param_ptr->fi_ptr_arr,
                                               fltr_param_ptr->num_filters))
      {
        LOG_MSG_INFO1_1("ps_iface_ipfltri_add(): Invalid fltr spec is specified "
                        "by client %d", client_id);
        *ps_errno = DS_EINVAL;
        return PS_IFACE_IPFLTR_INVALID_HANDLE;
      }
    }
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
    if (iface_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_iface_ipfltri_add(): "
                      "Adding fltrs to global ipfltr q");
      ipfltr_q_ptr = &(global_ipfltr_info[client_id]);
    }
    else if (PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_INFO2_2("ps_iface_ipfltri_add(): Adding fltrs to iface 0x%x:%d",
                      iface_ptr->name, iface_ptr->instance);
      ipfltr_q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
    }
    else
    {
      LOG_MSG_ERROR_2("ps_iface_ipfltri_add(): Invalid iface, 0x%p, is passed "
                      "by client %d", iface_ptr, client_id);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      break;
    }

    /* Allocate memory and initialize the ipsec filter list */
     PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(ipsec_fltrs.filters.list_ptr,
               sizeof(ip_filter_type)*fltr_param_ptr->num_filters,
               ip_filter_type*);

    if (ipsec_fltrs.filters.list_ptr == NULL)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *ps_errno = DS_NOMEMORY;
      return PS_IFACE_IPFLTR_INVALID_HANDLE;
    }

    memset(ipsec_fltrs.filters.list_ptr,0,sizeof(ip_filter_type)*fltr_param_ptr->num_filters);
    ipsec_fltrs.filters.num_filters = 0;
    ipsec_fltrs.status = PS_IFACE_IPFLTRS_ADDED;

    /*-------------------------------------------------------------------------
      Check if filters need to be installed with IPA
    -------------------------------------------------------------------------*/
    if (fltr_param_ptr->enable)
    {
      if (IP_FLTR_CLIENT_SOCKETS == client_id )
      {
        install_hw_fltr = TRUE;
      }
    }

    prev_cnt    = q_cnt(ipfltr_q_ptr);
    fltr_handle = PS_IFACE_IPFLTR_INVALID_HANDLE;

    /*-------------------------------------------------------------------------
      Now add the new filters to this iface
    -------------------------------------------------------------------------*/
    for (fltr_index = 0; fltr_index < fltr_param_ptr->num_filters; fltr_index++)
    {
      fltr_ptr = (((ip_filter_type *) fltr_param_ptr->fi_ptr_arr) + fltr_index);

      /*-------------------------------------------------------------------------
        Check if someone is adding destination based filter for client sockets 
        on iface for which v6 ip address doesnt exist on that iface
      -------------------------------------------------------------------------*/
      if(IP_FLTR_CLIENT_SOCKETS == client_id)
      {
        if ((IP_V6 == fltr_ptr->ip_vsn) && (iface_ptr != NULL) &&
            (0 != (IPFLTR_MASK_IP6_DST_ADDR & fltr_ptr->ip_hdr.v6.field_mask)))
        {        
          v6_addr.ps_s6_addr64[0] = fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[0];
          v6_addr.ps_s6_addr64[1] = fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[1];
          
          retval = ps_iface_find_ipv6_addr( &v6_addr,
                                            &v6_iface_addr_ptr,
                                            &iface_ptr);    
  
          if ((FALSE == retval) || (NULL == v6_iface_addr_ptr))
          {        
            continue;
          } 
        }        
      }
      
      if ((new_filter_buf_ptr = ps_iface_ipfltri_alloc_buf()) == NULL)
      {
        LOG_MSG_ERROR_0("ps_iface_ipfltri_add(): "
                        "Failed to create buffer for IP filter");

        *ps_errno = DS_ENOMEM;
        break;
      }

      if (fltr_index == 0)
      {
        fltr_handle = (ps_iface_ipfltr_handle_type) new_filter_buf_ptr;
      }

      switch (fltr_param_ptr->filter_type)
      {
        case IPFLTR_DEFAULT_TYPE:
          new_filter_buf_ptr->filter.fi_default = *fltr_ptr;
          break;

        case IPFLTR_MBIM_TYPE:
        
          /*-----------------------------------------------------------------
            Allocate the memory for mbim filter and copy over the filter 
            params
          -----------------------------------------------------------------*/        
          mbim_filter_arr_ptr = 
            (((ip_filter_mbim_type *) fltr_param_ptr->fi_ptr_arr) + fltr_index);
            
          mbim_filter_ptr = ps_iface_ipfltr_mbimi_alloc(mbim_filter_arr_ptr->len);
          if (NULL == mbim_filter_ptr)
          {
            LOG_MSG_ERROR_0("ps_iface_ipfltri_add(): "
                            "Failed to allocate memory for mbim filter");          
            *ps_errno = DS_ENOMEM;
            break;
          }
          
          mbim_filter_ptr->len = mbim_filter_arr_ptr->len;
          memscpy(mbim_filter_ptr->pattern, 
                  mbim_filter_ptr->len, 
                  mbim_filter_arr_ptr->pattern, 
                  mbim_filter_ptr->len);
          memscpy(mbim_filter_ptr->mask, 
                  mbim_filter_ptr->len, 
                  mbim_filter_arr_ptr->mask, 
                  mbim_filter_ptr->len);
          
          new_filter_buf_ptr->filter.fi_mbim = mbim_filter_ptr;
          break;          

        default:
          LOG_MSG_ERROR_2("ps_iface_ipfltri_add(): "
                          "Invalid filter type, %d, by client %d",
                          fltr_param_ptr->filter_type, client_id);
          ASSERT(0);
          *ps_errno = DS_EFAULT;
          break;
      }

      (void) q_link(new_filter_buf_ptr, &(new_filter_buf_ptr->link));

      new_filter_buf_ptr->filter_type     = fltr_param_ptr->filter_type;
      new_filter_buf_ptr->fi_handle       = fltr_handle;
      new_filter_buf_ptr->fi_result       = fltr_param_ptr->fi_result;
      new_filter_buf_ptr->subset_id       = fltr_param_ptr->subset_id;
      new_filter_buf_ptr->disabled        = !fltr_param_ptr->enable;
      new_filter_buf_ptr->iface_group     = iface_group;
      new_filter_buf_ptr->hw_rule_handle  = NULL;
      new_filter_buf_ptr->hw_rule_handle_2 = NULL;

      if (TRUE == install_hw_fltr)
      {
        if (-1 == ps_iface_ipfltr_hw_add(iface_ptr, 
                                         client_id, 
                                         new_filter_buf_ptr))
        {
          *ps_errno = DS_EFAULT;
          break;
        }
      }

      if (IPFLTR_DEFAULT_TYPE == fltr_param_ptr->filter_type)
      {
        /* Add filter to ipsec list if applicable */
        if (client_id == IP_FLTR_CLIENT_SOCKETS)
        {
          if (PS_IPFLTR_TYPE_IS_ESP(new_filter_buf_ptr->filter.fi_default))
          {
            ipsec_fltrs.filters.list_ptr[ipsec_fltrs.filters.num_filters++] = 
              new_filter_buf_ptr->filter.fi_default;
          }
        }

        /*-------------------------------------------------------------------
          Add the new filters to the iface sorted by precedence
        -------------------------------------------------------------------*/
        tmp_tx_fltr_buf = q_check(ipfltr_q_ptr);

        while (tmp_tx_fltr_buf != NULL &&
               new_filter_buf_ptr->filter.fi_default.ipfltr_aux_info.fi_precedence
               >= tmp_tx_fltr_buf->filter.fi_default.ipfltr_aux_info.fi_precedence)
        {
          tmp_tx_fltr_buf = q_next(ipfltr_q_ptr, &(tmp_tx_fltr_buf->link));
        }
      
        if (NULL == tmp_tx_fltr_buf)
        {
          q_put(ipfltr_q_ptr, &new_filter_buf_ptr->link);
        }
        else
        {
          q_insert(ipfltr_q_ptr, &new_filter_buf_ptr->link, &tmp_tx_fltr_buf->link);
        }
      }
      else
      {
        q_put(ipfltr_q_ptr, &new_filter_buf_ptr->link);
      }
    }

    /*-------------------------------------------------------------------------
      If error occured during install filter then dont install filter 
      in software
    -------------------------------------------------------------------------*/ 
    if (DS_ENOERR != *ps_errno)    
    {
      LOG_MSG_ERROR_0 ("Error occured during install filter");
      break;
    }
    
    /*-------------------------------------------------------------------------
      Wrt QOS rule, in order to keep precedence, we remove all the rules 
      from hardware and add them to hardware again in precedence
    -------------------------------------------------------------------------*/ 
    if (IP_FLTR_CLIENT_QOS_OUTPUT == client_id  && 
        NULL != iface_ptr &&
        TRUE == fltr_param_ptr->enable)
    { 
      ps_iface_ipfltr_hw_delete_all_iface_filters(iface_ptr, client_id);
      ps_iface_ipfltr_hw_add_all_iface_filters(iface_ptr, client_id);
    }      

    /*-------------------------------------------------------------------------
      Post indication only if newly added filters are enabled
    -------------------------------------------------------------------------*/
    if (fltr_param_ptr->enable)
    {
      /*-----------------------------------------------------------------------
      For CLAT filters we don't need HW filters as these are applied on uplink
      and we don't have any filtering in A2 in that direction.
      But we need to install ul opt SW filters to send some specific traffic 
      to legacy path.
      -----------------------------------------------------------------------*/
      if ( (IP_FLTR_CLIENT_SOCKETS == client_id)           
        #ifdef FEATURE_DATA_PS_464XLAT 
          ||  IP_FLTR_CLIENT_CLAT == client_id
        #endif /* FEATURE_DATA_PS_464XLAT */ 
         )
      {
        if (-1 == ps_iface_ipfltri_add_opt_filter(iface_ptr,
                                                  iface_group,
                                                  fltr_param_ptr->fi_ptr_arr,
                                                  fltr_param_ptr->num_filters,
                                                  fltr_handle))
        {
          LOG_MSG_ERROR_2("ps_iface_ipfltri_add():"
                          "Couldnt add Opt filters to iface 0x%x:%d",
                          ((iface_ptr != NULL) ? iface_ptr->name : 0),
                          ((iface_ptr != NULL) ? iface_ptr->instance : 0));
          break;
        }
      }

      ps_iface_ipfltr_updated_ind(iface_ptr,
                                  client_id,
                                  prev_cnt,
                                  q_cnt(ipfltr_q_ptr));
    }

    /* Provide ipsec list to base iface if interested */
    if (PS_IFACE_IS_VALID(iface_ptr))
    {
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
      if (PS_IFACE_GET_CAPABILITY(base_iface_ptr,PS_IFACE_CAPABILITY_IPSEC_FLTR) && 
          (ipsec_fltrs.filters.num_filters > 0))
      {
        LOG_MSG_INFO2_2("Adding %d ipsec fltrs to base iface 0x%x ",
                        ipsec_fltrs.filters.num_filters, base_iface_ptr);
        if (ps_iface_ioctl(base_iface_ptr,
                           PS_IFACE_IOCTL_IPFLTR_CHANGED,
                           &ipsec_fltrs,
                           &ps_err) != 0) 
        {
          LOG_MSG_ERROR_1("PS_IFACE_IOCTL_IPFLTR_CHANGED returned error %d",
                           ps_err);
        }
      }
    }

    PS_SYSTEM_HEAP_MEM_FREE(ipsec_fltrs.filters.list_ptr);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
     
    return fltr_handle;
  } while (0);

  /*-------------------------------------------------------------------------
    Filter failed to install so do the clean up
  -------------------------------------------------------------------------*/  
  (void) ps_iface_ipfltr_delete(iface_ptr, client_id, fltr_handle, ps_errno);
  ps_iface_ipfltri_free_buf(new_filter_buf_ptr);
  PS_SYSTEM_HEAP_MEM_FREE(ipsec_fltrs.filters.list_ptr);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return PS_IFACE_IPFLTR_INVALID_HANDLE;
} /* ps_iface_ipfltri_add() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_POWERUP()

DESCRIPTION
  Initialize the global IP filter queue.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltr_powerup
(
  void
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_IFACE_IPFLTER_TYPE,
                           ps_iface_ipfltr_buf_mem,
                           PS_IFACE_IPFLTR_BUF_SIZE,
                           PS_IFACE_IPFLTR_BUF_NUM,
                           PS_IFACE_IPFLTR_BUF_HIGH_WM,
                           PS_IFACE_IPFLTR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_iface_ipfltr_buf_hdr,
                           (int *) ps_iface_ipfltr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltr_powerup(): Can't power up the module");
  }

  for (i = 0; i < IP_FLTR_CLIENT_MAX; i++)
  {
    (void) q_init(&(global_ipfltr_info[i]));
  }
} /* ps_iface_ipfltr_powerup() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_INIT()

DESCRIPTION
  Adds  global TCP Ack filters, v4 fragmentation fitlers.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltr_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_iface_ipfltr_hw_init();

  return;
} /* ps_iface_ipfltr_init() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD()

DESCRIPTION
  This function adds a set of IP filters to the specified iface. The
  filter set is tied to a filter handle, which uniquely idenifies a set of
  filters added by the given client for the given iface. The filter handle
  is used to manipulate the filter set. A client needs to provide a filter
  result which is returned when a filter successfully match during the filter
  execution.
  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  iface_ptr          : Iface to add filters to
  client_id          : Filtering client id
  fltr_add_param_ptr : ptr to structure containing all the necessary info to
                       add filters to an iface
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  PS_IFACE_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_ipfltr_handle_type ps_iface_ipfltr_add
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  const ps_iface_ipfltr_add_param_type  * fltr_param_ptr,
  int16                                 * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_iface_ipfltri_add(iface_ptr,
                              ANY_DEFAULT_GROUP, 
                              client_id,
                              fltr_param_ptr,
                              ps_errno);
} /* ps_iface_ipfltr_add() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD_BY_GROUP()

DESCRIPTION
  This function adds a set of IP filters to the global ip filter array
  based on group. The filter set is tied to a filter handle, which uniquely
  idenifies a set of filters added by the client for the given iface
  group. The filter handle is used to manipulate the filter set. A client
  needs to provide a filter result which is returned when a filter
  successfully match during the filter execution.

  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  iface_group        : iface group for which filters need to be executed
  client_id          : Filtering client id
  fltr_param_ptr     : ptr to structure containing all the necessary info
                       to add filters to an iface
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  PS_IFACE_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_ipfltr_handle_type ps_iface_ipfltr_add_by_group
(
  ps_iface_name_enum_type                 iface_group,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  const ps_iface_ipfltr_add_param_type  * fltr_param_ptr,
  int16                                 * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_iface_ipfltri_add(NULL,
                              iface_group,
                              client_id,
                              fltr_param_ptr,
                              ps_errno);

} /* ps_iface_ipfltr_add_by_group() */

int ps_iface_ipfltr_add_ex
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client_id,
  const ps_iface_ipfltr_add_param_type  * fltr_param_ptr,
  ps_iface_ipfltr_handle_type           * ipfltr_handle_arr,
  int16                                 * ps_errno
)
{
  int                                   ret_val = 0;
  uint32                                fltr_param_cnt; 
  int                                   index;
  ps_iface_ipfltr_handle_type           tmp_ipfltr_handle;
  ps_iface_ipfltr_add_param_type        tmp_fltr_param;
  boolean                               status = TRUE;
  void                                  * tmp_fi_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (ps_errno == NULL)
    {
      ret_val =  -1;
      break;
    }

    if( ( NULL == iface_ptr ) || ( NULL == fltr_param_ptr ) || 
        ( NULL == ipfltr_handle_arr ) || ( client_id >= IP_FLTR_CLIENT_MAX ) || 
        ( 0 == fltr_param_ptr->num_filters ) )     
    {
      *ps_errno = DS_EFAULT;
       ret_val =  -1;
       break;
    }

    fltr_param_cnt = fltr_param_ptr->num_filters;
  
    if (!fltr_param_ptr->is_validated)
    {
      if (IPFLTR_DEFAULT_TYPE == fltr_param_ptr->filter_type)
      {
        if (!ps_iface_ipfltr_validate_fltr_param(client_id,
                                                 fltr_param_ptr->fi_ptr_arr,
                                                 fltr_param_ptr->num_filters))
        {
          status = FALSE;
        }
      }
    }

    if( status == FALSE )
    {
      *ps_errno = DS_EINVAL;
      ret_val =  -1;
      break;
    }
    
     /*--------------------------------------------------------------------------
      Add filters one by one so that we will get one handle for each of the
      filters. Since the filter rules are already validated directly pass 
      validated as TRUE
      ---------------------------------------------------------------------------*/
    for(index =0; (uint32)index < fltr_param_cnt; index++)
    {
      tmp_ipfltr_handle = PS_IFACE_IPFLTR_INVALID_HANDLE;
      memset( (void*)&tmp_fltr_param, 0, sizeof(ps_iface_ipfltr_add_param_type) );
      
      tmp_fi_ptr = (((ip_filter_type *) fltr_param_ptr->fi_ptr_arr) + index);
          
      tmp_fltr_param.enable              = fltr_param_ptr->enable;
      tmp_fltr_param.fi_result           = fltr_param_ptr->fi_result;
      tmp_fltr_param.fi_ptr_arr          = tmp_fi_ptr;
      tmp_fltr_param.num_filters         = 1;
      tmp_fltr_param.filter_type         = fltr_param_ptr->filter_type;
      tmp_fltr_param.is_validated        = TRUE;
      tmp_fltr_param.fltr_priority       = fltr_param_ptr->fltr_priority;
      tmp_fltr_param.fltr_compare_f_ptr  = fltr_param_ptr->fltr_compare_f_ptr;
  
      tmp_ipfltr_handle = ps_iface_ipfltr_add( iface_ptr, 
                                               client_id, 
                                               &tmp_fltr_param,
                                               ps_errno);
  
      if(PS_IFACE_IPFLTR_INVALID_HANDLE == tmp_ipfltr_handle) 
      {
        LOG_MSG_ERROR_2("Invalid filter_handle( = %x) received  errno %d",
        tmp_ipfltr_handle, *ps_errno);
        status = FALSE;
        break;
      }
      else
      {
        ipfltr_handle_arr[index] = tmp_ipfltr_handle;
      } 
    }
  
    /* --------------------------------------------------------------------------
      If we are not able to add all filters return error. Otherwise enable them
      if enable flag is TRUE as they will be disabled by default
      ---------------------------------------------------------------------------*/
    if(FALSE == status)
    { 
      index -= 1;
  
      for(;index >= 0; index--)
      {
        ps_iface_ipfltr_delete(iface_ptr, 
                               client_id, 
                               ipfltr_handle_arr[index],
                               ps_errno);
      }
  
      ret_val = -1;
    }
  }while (0);
  
  LOG_MSG_INFO1_1( "ps_iface_ipfltr_add_ex: ps_errno=%d", 
                  (NULL != ps_errno)?*ps_errno: DS_EFAULT);
  
  return ret_val;
} /* ps_iface_ipfltr_add_ex() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_DELETE()

DESCRIPTION
  This function deletes all the existing IP filter rules for the specified
  filter handle for a given client from the specified iface.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr : Iface to delete filters from
  client_id : Filtering client id
  fi_handle : filter handle associated with the filter set which was added
              on this iface by this client.
  ps_errno  : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once these filters are deleted
===========================================================================*/
int ps_iface_ipfltr_delete
(
  ps_iface_type                        * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ps_iface_ipfltr_handle_type            fi_handle,
  int16                                * ps_errno
)
{
  ps_iface_ipfilteri_type  * filter_buf_ptr;
  ps_iface_ipfilteri_type  * next_filter_buf_ptr;
  q_type                   * ipfltr_q_ptr;
  int                        prev_cnt;
  boolean                    post_indication = FALSE;
  ps_iface_name_enum_type    iface_group = ANY_DEFAULT_GROUP;
  ps_iface_ipfltr_change_type      ipsec_fltrs = {0}; 
  ps_iface_type                  * base_iface_ptr = NULL;
  sint15                           ps_err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_iface_ipfltr_delete(): FLTR DELETE called "
                  "by client %d, filter_id %d, iface_ptr 0x%p",
                  client_id, fi_handle, iface_ptr);

  if (ps_errno == NULL)
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltr_delete(): "
                    "NULL parameter is passed");
    ASSERT(0);
    return -1;
  }

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_delete(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fi_handle == PS_IFACE_IPFLTR_INVALID_HANDLE)
  {
    LOG_MSG_INFO1_1("ps_iface_ipfltr_delete(): "
                    "Invalid filter handle is passed by client, %d",
                    client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Figure out if filter needs to be deleted from global fltr Q or iface
    specific Q
  -------------------------------------------------------------------------*/
  if (iface_ptr == NULL)
  {
    LOG_MSG_INFO2_0("ps_iface_ipfltr_delete(): "
                    "Deleting fltrs from global ipfltr q");
    ipfltr_q_ptr = &(global_ipfltr_info[client_id]);
  }
  else if (PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_INFO2_2("ps_iface_ipfltr_delete(): "
                    "Deleting fltrs from iface 0x%x:%d",
                    iface_ptr->name, iface_ptr->instance);
    ipfltr_q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_ipfltr_delete(): "
                    "Invalid iface, 0x%p, is passed by client, %d",
                    iface_ptr, client_id);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Loop through fltr Q and delete all the filters which have the same handle
  -------------------------------------------------------------------------*/
  prev_cnt = q_cnt(ipfltr_q_ptr);

  /* if prev_cnt is zero, memory allocation for size 0 is requested and the
     macro asserts. Only if the count is greater than zero memory allocation
     will be attempted  */
  if(prev_cnt != 0 )
  {
    /* Allocate memory and initialize the ipsec filter list */
    PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(ipsec_fltrs.filters.list_ptr,
                                    sizeof(ip_filter_type)*prev_cnt, 
                                     ip_filter_type*);
    if (ipsec_fltrs.filters.list_ptr == NULL)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *ps_errno = DS_NOMEMORY;
      return -1;
    }
    memset(ipsec_fltrs.filters.list_ptr,0,sizeof(ip_filter_type)*prev_cnt);
    ipsec_fltrs.filters.num_filters = 0;
    ipsec_fltrs.status = PS_IFACE_IPFLTRS_DELETED;
  }

  /* If prev_cnt is zero, the filter_buf_ptr will also be NULL and the 
     while loop will not be executed */
  filter_buf_ptr = q_check(ipfltr_q_ptr);
  while (filter_buf_ptr != NULL)
  {
    next_filter_buf_ptr = q_next(ipfltr_q_ptr, &(filter_buf_ptr->link));
    /*-----------------------------------------------------------------------
      If generic filter handle is passed, it means that delete operation needs
      to be applied  on all filters
     //have delete_all api
    -----------------------------------------------------------------------*/
    if (filter_buf_ptr->fi_handle == fi_handle ||
        PS_IFACE_IPFLTR_DELETE_ALL_HANDLE == fi_handle)
    {
      iface_group = filter_buf_ptr->iface_group;

      /*---------------------------------------------------------------------
        Post indication only if any of deleted filters are enabled. Since
        indication is already posted when filters are disabled, another
        indication would be redundant
      ---------------------------------------------------------------------*/
      if (filter_buf_ptr->disabled == FALSE)
      {
        post_indication = TRUE;
      }

#ifndef TEST_FRAMEWORK
      if (NULL != filter_buf_ptr->hw_rule_handle)
      {
        ps_iface_ipfltr_hw_delete(filter_buf_ptr->hw_rule_handle, 
                                  client_id, 
                                  filter_buf_ptr->filter.fi_default.ip_vsn);
        filter_buf_ptr->hw_rule_handle = NULL;
      }  

      if (NULL != filter_buf_ptr->hw_rule_handle_2)
      {
        ps_iface_ipfltr_hw_delete(filter_buf_ptr->hw_rule_handle_2, 
                                  client_id, 
                                  filter_buf_ptr->filter.fi_default.ip_vsn);
        filter_buf_ptr->hw_rule_handle_2 = NULL;
      }
#endif /* TEST_FRAMEWORK */
        
      if (IP_FLTR_CLIENT_MBIM == client_id)
      {
        ps_iface_ipfltr_mbimi_free(filter_buf_ptr->filter.fi_mbim);
      }
      /* Add filter to ipsec list if applicable */
      if (client_id == IP_FLTR_CLIENT_SOCKETS)
      {
        if (PS_IPFLTR_TYPE_IS_ESP(filter_buf_ptr->filter.fi_default))
        {
          ipsec_fltrs.filters.list_ptr[ipsec_fltrs.filters.num_filters++] = 
          filter_buf_ptr->filter.fi_default;
        }
      }
      
      /*---------------------------------------------------------------------
        Delete this filter
      ---------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(ipfltr_q_ptr, &(filter_buf_ptr->link));
#else
      q_delete(&(filter_buf_ptr->link));
#endif

      ps_iface_ipfltri_free_buf(filter_buf_ptr);
    }

    filter_buf_ptr = next_filter_buf_ptr;
  }

  
  /*-----------------------------------------------------------------
    For qos fitlers, reset qos_fltr_offloaded_to_hw flag
  -----------------------------------------------------------------*/        
  if (client_id == IP_FLTR_CLIENT_QOS_OUTPUT && 
      PS_IFACE_IS_VALID(iface_ptr) &&
      (0 == PS_IFACEI_NUM_FILTERS(iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT)) &&
      PS_IFACE_IS_PHYSICAL_IFACE(iface_ptr))
  {
    LOG_MSG_INFO1_0("ps_iface_ipfltr_delete() Offloaded to hw = false");
    iface_ptr->qos_fltr_offloaded_to_hw = FALSE;
  }  

  if (IP_FLTR_CLIENT_SOCKETS == client_id)
  {
    ps_iface_ipfltri_delete_opt_filter(iface_ptr, iface_group, fi_handle);
  }

  /*-------------------------------------------------------------------------
    Post indication only if filters are deleted
  -------------------------------------------------------------------------*/
  if (post_indication && prev_cnt != q_cnt(ipfltr_q_ptr))
  {
    ps_iface_ipfltr_updated_ind(iface_ptr,
                                client_id,
                                prev_cnt,
                                q_cnt(ipfltr_q_ptr));
  }

  /* Provide ipsec list of deleted filters to base iface if interested */
  if (PS_IFACE_IS_VALID(iface_ptr))
  {
    base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
    if (PS_IFACE_GET_CAPABILITY(base_iface_ptr,PS_IFACE_CAPABILITY_IPSEC_FLTR) && 
        (ipsec_fltrs.filters.num_filters > 0))
    {
      LOG_MSG_INFO2_2("Deleting %d ipsec fltrs from base iface 0x%x ",
                      ipsec_fltrs.filters.num_filters, base_iface_ptr);
      if (ps_iface_ioctl(base_iface_ptr,
                         PS_IFACE_IOCTL_IPFLTR_CHANGED,
                         &ipsec_fltrs,
                         &ps_err)!= 0) 
      {
        LOG_MSG_ERROR_1("PS_IFACE_IOCTL_IPFLTR_CHANGED returned error %d ",
                        ps_err);
      }
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE(ipsec_fltrs.filters.list_ptr);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;

} /* ps_iface_ipfltr_delete() */

int ps_iface_ipfltr_delete_ex
(
  ps_iface_type          * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type    client_id,
  ps_iface_ipfltr_handle_type          * ipfltr_handle_arr,
  uint16                                 ipfltr_handle_cnt,
  int16                                * ps_errno
)
{
  uint16                     index;
  int32                      err_no = PS_IFACE_IPFLTR_ERR_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (ps_errno == NULL)
    {
      err_no = PS_IFACE_IPFLTR_ERR_INALID_PS_ERR_PTR;
      break;
    }

    if ( (ipfltr_handle_arr == NULL) || (ipfltr_handle_cnt == 0) )
    {
      *ps_errno = DS_EFAULT;
      err_no = PS_IFACE_IPFLTR_ERR_INVALID_HANLDE;
      break;
    }

    for( index =0; index < ipfltr_handle_cnt; index++)
    {
      if ( -1 == ps_iface_ipfltr_delete( iface_ptr, client_id, 
                                         ipfltr_handle_arr[index], ps_errno ) )
      {
        err_no = PS_IFACE_IPFLTR_ERR_DELETE_FAIL;
        break;
      }
    }
  }while(0);

  LOG_MSG_INFO1_2( "ps_iface_ipfltr_delete_ex: Filter array deletion result."
                   "errno=%d, ps_errno=%d", err_no, 
                   (NULL != ps_errno)?*ps_errno: DS_EFAULT);

  return (err_no == PS_IFACE_IPFLTR_ERR_NONE)?0:-1;
} /* ps_iface_ipfltr_delete_ex() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD_V6_DST_FLTR()

DESCRIPTION
  This function adds ipv6 destination based filters to hardware

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to hardware handle

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_add_v6_dst_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr
)
{
  return ps_iface_ipfltr_hw_add_v6_dst_fltr(iface_ptr, v6_addr_ptr);                                
} /* ps_iface_ipfltr_add_v6_dst_fltr() */

/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_DELETE_V6_DST_FILTER()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltr_delete_v6_dst_fltr
(
  ps_iface_ipfltr_hw_rule_handle_type  v6_dst_fltr_handle_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_iface_ipfltr_hw_delete_v6_dst_fltr(v6_dst_fltr_handle_ptr);
  
} /* ps_iface_ipfltr_delete_v6_dst_fltr() */

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD_V6_CLAT_DST_FLTR()

DESCRIPTION
  This function adds ipv6 CLAT destination based filters to hardware

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to hardware handle

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_ipfltr_hw_rule_handle_type ps_iface_ipfltr_add_clat_v6_dst_fltr
(
  ps_iface_type          * iface_ptr,
  struct ps_in6_addr     * v6_addr_ptr,
  uint8                    prefix_len
)
{
  return ps_iface_ipfltr_hw_add_clat_v6_dst_fltr(iface_ptr, 
                                                 v6_addr_ptr, 
                                                 prefix_len);                                
} /* ps_iface_ipfltr_add_clat_v6_dst_fltr() */
#endif /* FEATURE_DATA_PS_464XLAT */


int32 ps_iface_ipfltr_is_handle_enabled
(
  ps_iface_type                       * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type   client_id,
  ps_iface_ipfltr_handle_type           fi_handle,
  boolean                             * is_enabled_ptr,
  int16                               * ps_errno
)
{
  q_type                   * q_ptr;
  ps_iface_ipfilteri_type  * filter_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("ps_iface_ipfltr_is_handle_enabled(): "
                           "iface 0x%p client %d, fltr_id %d",
                           iface_ptr, client_id, fi_handle);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (ps_errno == NULL)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltr_is_handle_enabled(): "
                    "NULL errno, iface 0x%p", iface_ptr);
    return -1;
  }

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    LOG_MSG_ERROR_2("ps_iface_ipfltr_is_handle_enabled(): "
                    "Invalid filtering client id %d, iface 0x%p",
                    client_id, iface_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fi_handle == PS_IFACE_IPFLTR_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_3("ps_iface_ipfltr_is_handle_enabled(): "
                    "Invalid fltr handle 0x%x is passed by client %d, "
                    "iface 0x%p", fi_handle, client_id, iface_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (is_enabled_ptr == NULL)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltr_is_handle_enabled(): "
                    "NULL arg, iface 0x%p", iface_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (iface_ptr == NULL)
  {
    q_ptr = &(global_ipfltr_info[client_id]);
  }
  else if (PS_IFACE_IS_VALID(iface_ptr))
  {
    q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_is_handle_enabled(): "
                    "Invalid iface 0x%p is passed", iface_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  filter_buf_ptr = q_check(q_ptr);
  while (filter_buf_ptr != NULL)
  {
    if (filter_buf_ptr->fi_handle == fi_handle)
    {
      *is_enabled_ptr = !( filter_buf_ptr->disabled);
      break;
    }

    filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link));
  }

  if (filter_buf_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_ipfltr_is_handle_enabled(): "
                    "Couldn't find a fltr matching fltr handle 0x%x, iface 0x%p",
                    fi_handle, iface_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  LOG_MSG_FUNCTION_EXIT_2("ps_iface_ipfltr_is_handle_enabled(): "
                          "is_enabled %d, iface 0x%p",
                          *is_enabled_ptr, iface_ptr);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_ipfltr_is_handle_enabled() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_CONTROL()

DESCRIPTION
  This function allows a control operation on all IP filters currently
  associated with the specified filter handle of the client for the iface.
  Only operation supported for now is disabling or enabling the filter
  execution state. Disabled filters are not used during filter execution.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr : Iface on which filters are added
  client_id : Filtering client id
  fi_handle : filter handle which identifies a specific filter set added on
              this iface by this client.
  enable    : Whether to enable or disable already installed filters
  ps_errno  : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once a filter set is disabled and they
  may get filtered if a filter set is enabled
===========================================================================*/
int ps_iface_ipfltr_control
(
  ps_iface_type                       *iface_ptr,
  ps_iface_ipfltr_client_id_enum_type client_id,
  ps_iface_ipfltr_handle_type         fi_handle,
  boolean                             enable,
  int16                               *ps_errno
)
{
  q_type                   * q_ptr;
  ps_iface_ipfilteri_type  * filter_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_iface_ipfltr_control(): "
                  "FLTR CONTROL called by client %d, filter_id %d, fltrs enabled=%d",
                  client_id, fi_handle, enable);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (ps_errno == NULL)
  {
    LOG_MSG_ERROR_0("ps_iface_ipfltr_control(): "
                    "NULL parameter is passed");
    ASSERT(0);
    return -1;
  }

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_control(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fi_handle == PS_IFACE_IPFLTR_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltr_control(): "
                    "Invalid filter handle is passed by client, %d",
                    client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (iface_ptr == NULL)
  {
    LOG_MSG_INFO2_0("ps_iface_ipfltr_control(): "
                    "Controlling fltrs on global ipfltr q");
    q_ptr = &(global_ipfltr_info[client_id]);
  }
  else if (PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_INFO2_2("ps_iface_ipfltr_control(): "
                    "Controlling fltrs on iface 0x%x:%d",
                    iface_ptr->name, iface_ptr->instance);
    q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_control(): "
                    "Invalid iface 0x%p is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Perform the control operation on all the filters that match the handle
  -------------------------------------------------------------------------*/
  filter_buf_ptr = q_check(q_ptr);
  while (filter_buf_ptr != NULL)
  {
    if ((filter_buf_ptr->fi_handle == fi_handle) ||
         (PS_IFACE_IPFLTR_DELETE_ALL_HANDLE == fi_handle))
    {
      filter_buf_ptr->disabled = !enable;
      if (IP_FLTR_CLIENT_SOCKETS == client_id || 
#ifdef FEATURE_DATA_PS_464XLAT
          IP_FLTR_CLIENT_CLAT == client_id || 
#endif /* FEATURE_DATA_PS_464XLAT */
          IP_FLTR_CLIENT_QOS_OUTPUT == client_id)
      {
        if (enable)
        {
          if (IP_FLTR_CLIENT_SOCKETS == client_id)
          {
            if (FALSE == ps_iface_ipfltri_add_opt_filter
                         (
                           iface_ptr,
                           ANY_DEFAULT_GROUP,
                           &(filter_buf_ptr->filter.fi_default),
                           1,
                           fi_handle
                         ))
            {
              LOG_MSG_ERROR_2("Couldnt add Opt filters to iface 0x%x:%d",
                               PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->name:0,
                               PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->instance:0);
            }
          }
        
          if (IP_FLTR_CLIENT_SOCKETS == client_id 
              #ifdef FEATURE_DATA_PS_464XLAT
               || IP_FLTR_CLIENT_CLAT == client_id
              #endif /* FEATURE_DATA_PS_464XLAT */ 
             )
          {
            if (-1 == ps_iface_ipfltr_hw_add (iface_ptr,  client_id, filter_buf_ptr))
            {
              LOG_MSG_ERROR_0("Couldn't install filter with hardware");
            }
          }
        }
        else
        {
          /*-----------------------------------------------------------------
            If filter is installed with hardware, delete it from hardware
          -----------------------------------------------------------------*/
          if (NULL != filter_buf_ptr->hw_rule_handle)
          {
            ps_iface_ipfltr_hw_delete(filter_buf_ptr->hw_rule_handle, 
                                      client_id, 
                                      filter_buf_ptr->filter.fi_default.ip_vsn);
            filter_buf_ptr->hw_rule_handle = NULL;
          }

          if (NULL != filter_buf_ptr->hw_rule_handle_2)
          {
            ps_iface_ipfltr_hw_delete(filter_buf_ptr->hw_rule_handle_2, 
                                      client_id, 
                                      filter_buf_ptr->filter.fi_default.ip_vsn);
            filter_buf_ptr->hw_rule_handle_2 = NULL;
          }

          ps_iface_ipfltri_delete_opt_filter(iface_ptr, ANY_DEFAULT_GROUP, fi_handle);
        }
      }
    }
    filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link));
  }

  /*-------------------------------------------------------------------------
    Wrt QOS rule, in order to keep precedence, we remove all the rules 
    from hardware and add them to hardware again in precedence
  -------------------------------------------------------------------------*/    
  if ((IP_FLTR_CLIENT_QOS_OUTPUT == client_id) && 
      (TRUE == enable) && (NULL != iface_ptr) )
  {
    ps_iface_ipfltr_hw_delete_all_iface_filters(iface_ptr, 
                                                client_id); 
                                                  
    ps_iface_ipfltr_hw_add_all_iface_filters(iface_ptr, 
                                             client_id);     
  }

  ps_iface_ipfltr_updated_ind(iface_ptr, client_id, q_cnt(q_ptr), q_cnt(q_ptr));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
} /* ps_iface_ipfltr_control() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_EXECUTE()

DESCRIPTION
  This function executes all the filters in an iface to see if any of
  those passes criteria specified in the information passed in. Processing
  is done until the first match is found and filter id associated
  with that filter is returned. For DEFAULT filter type, each field set in
  the filter is compared against the input info using a fixed set of rules.
  For ACL type filters, the ACL function is called which can contain more
  complex and variant types of rules.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr          - Iface to pick filters from for execution
  client_id          - Filtering client id
  subset_id          - ID which identified a subset of all filters installed
                       on iface. Only these filters are considered for
                       matching a packet
  ip_filter_info_ptr - Ptr to IP pkt information block to apply filter on

RETURN VALUE
  filter id associated with the filter : on a successful filter match
  PS_IFACE_IPFLTR_NOMATCH              : for no match

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_ipfltr_result_type ps_iface_ipfltr_execute
(
  ps_iface_type                       *iface_ptr,
  ps_iface_ipfltr_client_id_enum_type client_id,
  ps_iface_ipfltr_subset_id_type      subset_id,
  ip_pkt_info_type                    *ip_pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_iface_ipfltri_execute(iface_ptr,
                                   ANY_DEFAULT_GROUP,
                                   client_id,
                                   subset_id,
                                  ip_pkt_info_ptr);

} /* ps_iface_ipfltr_execute() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_EXECUTE_BY_GROUP()

DESCRIPTION
  This function executes all the filters pertaining to an iface group to
  see if any of those passes criteria specified in the information passed
  in. Processing is done until the first match is found and filter id
  associated with that filter is returned. For DEFAULT filter type, each
  field set in the filter is compared against the input info using a fixed
  set of rules.
  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  iface_group     - group on which filters needs to be executed
  client_id       - Filtering client id
  subset_id       - ID which identified a subset of all filters installed
                    on iface. Only these filters are considered for
                    matching a packet
  ip_pkt_info_ptr - Ptr to IP pkt information block to apply filter on


RETURN VALUE
  filter result                  : on success
  PS_IFACE_IPFLTR_NOMATCH        : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_ipfltr_result_type ps_iface_ipfltr_execute_by_group
(
  ps_iface_name_enum_type             iface_group,
  ps_iface_ipfltr_client_id_enum_type client_id,
  ps_iface_ipfltr_subset_id_type      subset_id,
  ip_pkt_info_type                    *ip_pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_iface_ipfltri_execute(NULL,
                                  iface_group,
                                  client_id,
                                  subset_id,
                                  ip_pkt_info_ptr);

} /* ps_iface_ipfltr_execute_by_group() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_VALIDATE_FLTR_PARAM()

DESCRIPTION
  Validates parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_arr  : Array of ptr to filters
  num_fltr  : Number of filters in above array

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  the appropriate error masks will be set with the appropriate error codes
  for values and enums which failed validation. Because variables are only
  validated if the enums are set in the field masks the only error masks
  that will be updated will be those corresponding to the values set within
  the field masks.
===========================================================================*/
boolean ps_iface_ipfltr_validate_fltr_param
(
  ps_iface_ipfltr_client_id_enum_type  client_id,
  ip_filter_type                       fltr_arr[],
  uint8                                num_fltr
)
{
  ip_filter_type         * fltr_ptr;
  uint8                    fltr_index;
  ps_ip_protocol_enum_type    next_hdr_prot;
  boolean                  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    LOG_MSG_ERROR_1("ps_iface_ipfltr_validate_fltr_param(): "
                    "Invalid client_id, %d, is passed", client_id);
    ASSERT(0);
    return FALSE;
  }

  if (fltr_arr == NULL)
  {
    DATA_ERR_FATAL("ps_iface_ipfltr_validate_fltr_param(): NULL parameter is passed");
    return FALSE;
  }

  LOG_MSG_INFO2_1("ps_iface_ipfltr_validate_fltr_param(): "
                  "Validating filters of client %d", client_id);

  for (fltr_index = 0; fltr_index < num_fltr; fltr_index++)
  {
    next_hdr_prot = PS_NO_NEXT_HDR;
    fltr_ptr = &fltr_arr[fltr_index];

    /*----------------------------------------------------------------------
      Validate all fltr enums and variables. Validation fails if any of the
      parameters are invalid
    ----------------------------------------------------------------------*/
    if (fltr_ptr->ip_vsn != IP_V4 && fltr_ptr->ip_vsn != IP_V6)
    {
      LOG_MSG_INFO1_2("ps_iface_ipfltr_validate_fltr_param(): "
                      "Incorrect IP version specified %d by client %d",
                      fltr_ptr->ip_vsn, client_id);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Filter spec is invalid if any of the protocol params are invalid and
      hence "is_valid &=" is used below.
    -------------------------------------------------------------------------*/
    if (fltr_ptr->ip_vsn == IP_V4)
    {
      is_fltr_valid &= ps_iface_ipfltri_validate_ipv4_param(client_id,
                                                            fltr_ptr,
                                                            &next_hdr_prot);
    }
    else /* IP version is IPV6 */
    {
      is_fltr_valid &= ps_iface_ipfltri_validate_ipv6_param(client_id,
                                                            fltr_ptr,
                                                            &next_hdr_prot);
    }

    if (next_hdr_prot != PS_NO_NEXT_HDR)
    {
      switch (next_hdr_prot)
      {
        case PS_IPPROTO_TCP:
          is_fltr_valid &=
            ps_iface_ipfltri_validate_tcp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_UDP:
          is_fltr_valid &=
            ps_iface_ipfltri_validate_udp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_ICMP:
        case PS_IPPROTO_ICMP6:
          is_fltr_valid &=
            ps_iface_ipfltri_validate_icmp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_IGMP:
        case PS_IPPROTO_ESP:
        case PS_IPPROTO_AH:
          /* Do nothing */
          break;

        case PS_IPPROTO_TCP_UDP:
          is_fltr_valid &=
            ps_iface_ipfltri_validate_tcp_udp_param(client_id, fltr_ptr);
          break;

        default:
          is_fltr_valid = FALSE;
          break;
      }
    } /* end if (next_prot_hdr != PS_NO_NEXT_HDR) */
  }

  return is_fltr_valid;

} /* ps_iface_ipfltr_validate_fltr_param() */

int ps_iface_ipfltr_stats
(
  ps_iface_type                         * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type     client,
  uint32                                 * max_allowed,
  uint32                                 * used,
  int16                                 * ps_errno
)
{
  q_type                   * ipfltr_q_ptr;
  int                        ret_val = 0;
  /*---------------------------------------------------------------------------------------------*/

  do
  {
    if (ps_errno == NULL)
    {
      break;
    }          
              
    if ( ( max_allowed == NULL ) || ( used == NULL ) || ( iface_ptr == NULL )
          || ( client >= IP_FLTR_CLIENT_MAX ))
    {
      *ps_errno = DS_EFAULT;
      break;
    }

    if (iface_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_iface_ipfltr_stats(): "
                      "Adding fltrs to global ipfltr q");
      ipfltr_q_ptr = &(global_ipfltr_info[client]);
    }
    else if (PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_INFO2_2("ps_iface_ipfltr_stats(): Adding fltrs to iface 0x%x:%d",
                      iface_ptr->name, iface_ptr->instance);
      ipfltr_q_ptr = &(iface_ptr->iface_private.ipfltr_info[client]);
    }
    else
    {
      LOG_MSG_ERROR_2("ps_iface_ipfltr_stats(): Invalid iface, 0x%p, is passed "
                      "by client %d", iface_ptr, client);
      *ps_errno = DS_EFAULT;
       break;
    }

    switch(client)
    {
      case IP_FLTR_CLIENT_POWERSAVE:
        *max_allowed = PS_IFACE_PWRSAVE_IPFLTR_MAX;
        break;
  
      default:
        *ps_errno = DS_EINVAL;
    }
  
    if( (NULL != ps_errno) && (0 != *ps_errno) )
    {
      ret_val = -1;
    }
    else
    {
      *used = q_cnt(ipfltr_q_ptr);  
    }
  }while(0);

  LOG_MSG_INFO1_1( "ps_iface_ipfltr_stats: ps_errno=%d", 
                  (NULL != ps_errno)?*ps_errno: DS_EFAULT);
  return ret_val;
}/* ps_iface_ipfltr_stats */

int ps_iface_ipfltr_get_control_status
(
  ps_iface_type                             * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type         client_id,
  ps_iface_ipfltr_control_status_enum_type  * ipfltr_control_status,
  int16                                     * ps_errno
)
{
  ps_iface_ipfilteri_type      * filter_buf_ptr;
  q_type                       * q_ptr;  
  int                            ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_2("ps_iface_ipfltr_get_control_status(): "
                  "client %d, iface_ptr 0x%p", client_id, iface_ptr);
  
  *ipfltr_control_status =  PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_NONE;  
  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_get_control_status(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }      

  /*-------------------------------------------------------------------------
    Figure out if its global fltr Q or iface specific Q
  -------------------------------------------------------------------------*/
  if (iface_ptr == NULL)
  {
    LOG_MSG_INFO2_0("ps_iface_ipfltr_get_control_status(): "
                    "Check fltrs from global ipfltr q");
    q_ptr = &(global_ipfltr_info[client_id]);
  }
  else if (PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_INFO2_2("ps_iface_ipfltr_get_control_status(): "
                    "Check fltrs from iface 0x%x:%d",
                    iface_ptr->name, iface_ptr->instance);
    q_ptr = &(iface_ptr->iface_private.ipfltr_info[client_id]);
  }
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_ipfltr_get_control_status(): "
                    "Invalid iface, 0x%p, is passed by client, %d",
                    iface_ptr, client_id);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }  
                  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  for (filter_buf_ptr = q_check(q_ptr);
       filter_buf_ptr != NULL;
       filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link)))
  {
    ret_val = 0;
    
    if (TRUE == filter_buf_ptr->disabled)
    {
      if (*ipfltr_control_status == 
          PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_ENABLED)
      {
        *ipfltr_control_status = 
          PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_MIXED;
        break;
      }
      else
      {
        *ipfltr_control_status = 
          PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_DISABLED;
      }
    }
    else
    {
      if (ret_val == PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_DISABLED)
      {
        *ipfltr_control_status =
          PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_MIXED;
        break;
      }
      else
      {
        *ipfltr_control_status =
          PS_IFACE_IPFLTR_ALL_FILTERS_STATUS_ENABLED;
      }
    }
  }
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
  return ret_val;
} /* ps_iface_ipfltr_get_control_status */

int ps_iface_ipfltr_delete_all
(
  ps_iface_type                      * iface_ptr,
  ps_iface_ipfltr_client_id_enum_type  client_id,
  int16                              * ps_errno
)
{
  int                            ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_2("ps_iface_ipfltr_delete_all(): "
                  "client %d, iface_ptr 0x%p", client_id, iface_ptr);
                  
  if (client_id >= IP_FLTR_CLIENT_MAX)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("ps_iface_ipfltr_delete_all(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }        

  ret_val = ps_iface_ipfltr_delete (iface_ptr, 
                                    client_id, 
                                    PS_IFACE_IPFLTR_DELETE_ALL_HANDLE, 
                                    ps_errno);
                                    
  return ret_val;
} /* ps_iface_ipfltr_delete_all */
