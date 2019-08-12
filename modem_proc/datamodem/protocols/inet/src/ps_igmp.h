#ifndef PS_IGMP_H
#define PS_IGMP_H
/*===========================================================================
           Internet Group Management Protocol (IGMP)

                 A P I   H E A D E R   F I L E

DESCRIPTION
 The IGMP API header file.


Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_igmp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/13/13    ss     Updates header parse API to input DSM double pointer
06/26/07    rs     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_mcasti.h"
#include "ps_mem.h"
#include "ps_in.h"
#include "ps_svc.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
 IGMP v2 Header fixed length
---------------------------------------------------------------------------*/
#define IGMP_LEN   8

/*---------------------------------------------------------------------------
 IGMP JOIN and LEAVE enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IGMP_JOIN,
  PS_IGMP_LEAVE
}ps_igmp_msg_enum_type;

/*---------------------------------------------------------------------------
  IGMP v2  protocol message type.
---------------------------------------------------------------------------*/
typedef enum
{
  IGMP_MIN_MSG_TYPE                    = 0x00,
  IGMP_MEMBERSHIP_QUERY_MSG_TYPE       = 0x11,
  IGMP_MEMBERSHIP_REPORT_V1_MSG_TYPE   = 0x12,
  IGMP_MEMBERSHIP_REPORT_V2_MSG_TYPE   = 0x16,
  IGMP_LEAVE_GROUP_MSG_TYPE            = 0x17,
  IGMP_MAX_MSG_TYPE                    = IGMP_LEAVE_GROUP_MSG_TYPE
} igmp_msg_enum_type;

/*---------------------------------------------------------------------------
  IGMP v2  protocol header.
---------------------------------------------------------------------------*/
typedef struct ps_igmp_hdr
{
  uint8             igmp_type;
  uint8             igmp_resp_time;
  uint16            igmp_checksum;
  struct ps_in_addr igmp_mcast_grp;
} ps_igmp_hdr;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_IGMP_HDR_PARSE

DESCRIPTION
  This function will update the appropriate fields in passed IGMP header
  after extracting information from dsm item.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on success and FALSE on failure.

SIDE EFFECTS
  None

===========================================================================*/
boolean ps_igmp_hdr_parse
(
  dsm_item_type     ** bpp,
  uint16             * hdrlen,
  struct ps_igmp_hdr * igmph,
  uint16               offset,
  ip_pkt_info_type   * ip_pkt_info_ptr
);


/*===========================================================================

FUNCTION PS_IGMP_INPUT

DESCRIPTION
  This function is called by the function ip_recieve() and is used to
  process an incoming IGMP packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_input
(
  ip_pkt_info_type    *ip_pkt_info_ptr, /* IP Packet Info            */
  struct dsm_item_s   *igmp_data_ptr,   /* data field                */
  uint16               offset           /* offset of the IGMP packet */
);


/*===========================================================================

FUNCTION PS_IGMP_HDR_CREATE

DESCRIPTION
  This function prepares an IGMP header that is later used by the function
  ps_igmp_ouput() to send the IGMP packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_hdr_create
(
  ps_igmp_msg_enum_type    igmp_msg_type,
  ps_iface_type          * iface_ptr,
  struct ps_in_addr      * igmp_grp_ptr,
  dsm_item_type         ** item_ptr
);


/*===========================================================================

FUNCTION PS_IGMP_OUTPUT

DESCRIPTION
  This function prepares a IGMP header and then calls the ip_send() to send
  the IGMP Message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_output
(
  ps_igmp_msg_enum_type    igmp_msg_type,
  ps_iface_type          * iface_ptr,
  struct ps_in_addr      * igmp_grp_ptr
);

/*===========================================================================

FUNCTION PS_IGMP_STOP_TIMER

DESCRIPTION
  This function stops timer for the membership group for which a membership
  report is received if the interface is a member of that group. This
  function is specific to IGMP as it expects an IPV4 address as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_stop_timer
(
  ps_iface_type               *this_iface_ptr,
  struct ps_in_addr           group_address
);


/*===========================================================================

FUNCTION PS_IGMP_START_TIMERS

DESCRIPTION
  This function starts timers for the membership groups associated with the
  given interface. This function is specific to IGMP as it works on IGMP
  header.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_start_timers
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
);


/*===========================================================================

FUNCTION PS_IGMP_PROCESS_GEN_QUERY

DESCRIPTION
  This function processes IGMP general queries.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_process_gen_query
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
);


/*===========================================================================

FUNCTION PS_IGMP_PROCESS_GRP_QUERY

DESCRIPTION
  This function processes IGMP group specific queries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_process_grp_query
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
);


/*===========================================================================

FUNCTION PS_IGMP_FIND_V4_GROUP

DESCRIPTION
  This function is used to find a multicast group in the list of groups
  associated with the interface. This function is specific to IPV4 addresses.

DEPENDENCIES
  None.

RETURN VALUE
  struct ps_mcast_cb *

SIDE EFFECTS
  None

===========================================================================*/
struct ps_mcast_cb * ps_igmp_find_v4_group
(
  ps_iface_type               *this_iface_ptr,
  struct ps_in_addr           igmp_grp
);


/*===========================================================================
FUNCTION PS_IGMP_SET_REPORT_TIMER

DESCRIPTION
  This function sets multicast report timer corresponding to the given
  multicast group.

PARAMETERS
  if_mcast_grp: multicast group whose timer needs to be set
  response_time: response time within which report needs to be sent

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_set_report_timer
(
  struct ps_mcast_cb   * if_mcast_grp,
  int                    response_time
);

#endif  /* PS_IGMP_H */
