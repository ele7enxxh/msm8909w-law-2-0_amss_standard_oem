#ifndef __PS_DNS_PROXY_H_
#define __PS_DNS_PROXY_H_
/*===========================================================================

                        P S _ D N S _ P R O X Y . H

DESCRIPTION
   This is the DNS Proxy internal header file.


  Copyright (c) 2010-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_dns_proxy.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
05/20/10    rt     Added module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "dsm.h"
#include "dsm_queue.h"
#include "dserrno.h"
#include "ps_dns_proxy_ext.h"
#include "ps_sys.h"
#include "ps_svc.h"
#include "ps_crit_sect.h"

/*===========================================================================

                                CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  PS Mem Buffer counts
---------------------------------------------------------------------------*/
#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
#define PS_DNS_PROXY_INFO_NUM     PS_SYS_MAX_AVAIL_PDNS
#define PS_DNS_PROXY_INFO_HIGH_WM PS_SYS_MAX_AVAIL_PDNS
#define PS_DNS_PROXY_INFO_LOW_WM  1

#define PS_DNS_PROXY_ENABLE_INFO_NUM     (PS_SYS_MAX_AVAIL_PDNS >> 1)
#define PS_DNS_PROXY_ENABLE_INFO_HIGH_WM (PS_SYS_MAX_AVAIL_PDNS >> 1)
#define PS_DNS_PROXY_ENABLE_INFO_LOW_WM  1

#define PS_DNS_PROXY_DISABLE_INFO_NUM     (PS_SYS_MAX_AVAIL_PDNS >> 1)
#define PS_DNS_PROXY_DISABLE_INFO_HIGH_WM (PS_SYS_MAX_AVAIL_PDNS >> 1)
#define PS_DNS_PROXY_DISABLE_INFO_LOW_WM  1

#define PS_DNS_PROXY_SOCK_EVENT_INFO_NUM     PS_SYS_MAX_AVAIL_PDNS
#define PS_DNS_PROXY_SOCK_EVENT_INFO_HIGH_WM PS_SYS_MAX_AVAIL_PDNS
#define PS_DNS_PROXY_SOCK_EVENT_INFO_LOW_WM  1

#else

#define PS_DNS_PROXY_INFO_NUM     2 
#define PS_DNS_PROXY_INFO_HIGH_WM 2
#define PS_DNS_PROXY_INFO_LOW_WM  1

#define PS_DNS_PROXY_ENABLE_INFO_NUM     2
#define PS_DNS_PROXY_ENABLE_INFO_HIGH_WM 2
#define PS_DNS_PROXY_ENABLE_INFO_LOW_WM  1

#define PS_DNS_PROXY_DISABLE_INFO_NUM     2
#define PS_DNS_PROXY_DISABLE_INFO_HIGH_WM 2
#define PS_DNS_PROXY_DISABLE_INFO_LOW_WM  1

#define PS_DNS_PROXY_SOCK_EVENT_INFO_NUM   2
#define PS_DNS_PROXY_SOCK_EVENT_INFO_HIGH_WM 2
#define PS_DNS_PROXY_SOCK_EVENT_INFO_LOW_WM  1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*---------------------------------------------------------------------------
  Total number of DNS queries that can be handled at the same time.
---------------------------------------------------------------------------*/
#define NUM_DNS_PROXY_ENTRIES    50
#define PS_DNS_PROXY_SERVER_PORT (dss_htons(53))

/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  DNS proxy critical section
---------------------------------------------------------------------------*/
extern ps_crit_sect_type dns_proxy_crit_section;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_ENABLE_CMD_INFO_S

DESCRIPTION
  Structure to hold command info for enable command
---------------------------------------------------------------------------*/
typedef struct ps_dns_proxy_enable_cmd_info_s
{
  ps_iface_type  * iface_ptr;
  dns_proxy_mode_enum_type mode;
} ps_dns_proxy_enable_cmd_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_DISABLE_CMD_INFO_S

DESCRIPTION
  Structure to hold command info for disable command
---------------------------------------------------------------------------*/
typedef struct ps_dns_proxy_disable_cmd_info_s
{
  ps_iface_type  * iface_ptr;
} ps_dns_proxy_disable_cmd_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_SOCK_EVENT_CMD_INFO_S

DESCRIPTION
  Structure to hold command info for socket events
---------------------------------------------------------------------------*/
typedef struct ps_dns_proxy_sock_event_cmd_info_s
{
  int16    sockfd;
  uint32   sock_event_mask;
  void   * user_data_ptr;
} ps_dns_proxy_sock_event_cmd_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_UM_IFACE_EV_CMD_INFO_S

DESCRIPTION
  Structure to hold command info for iface events
---------------------------------------------------------------------------*/
typedef struct ps_dns_proxy_um_iface_ev_cmd_info_s
{
  ps_iface_type  * iface_ptr;
  ps_iface_event_enum_type   event;
} ps_dns_proxy_um_iface_ev_cmd_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_QUERY_ENTRY_S

DESCRIPTION
  Structure to hold query information
---------------------------------------------------------------------------*/
typedef struct  ps_dns_proxy_query_entry_s
{
  uint32    src_addr;
  uint16    src_port;
  uint16    orig_query_id;
  uint16    mapped_query_id;
  uint32    time_stamp;
} ps_dns_proxy_query_entry_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_DNS_PROXY_INFO_TYPE

DESCRIPTION
  DNS proxy control block
---------------------------------------------------------------------------*/
typedef struct ps_dns_proxy_info_s 
{
  /* result callback for client */
  dns_proxy_result_cback_f_type result_cb;

  /* The socket handle for the socket we are getting DNS requests over. */
  sint15 server_sockfd;

  /* The socket handle for the socket we are sending DNS requests over. */
  sint15 client_sockfd;

  /* iface pointer */
  ps_iface_type  *iface_ptr;
  
  /* DNS proxy mode */
  dns_proxy_mode_enum_type mode;

  /* Array of Pending DNS requests waiting for response */
  ps_dns_proxy_query_entry_type dns_proxy_query_array[NUM_DNS_PROXY_ENTRIES];

  /* This watermark holds any items that can't be written out right
   * away.  The items will be dequeued and sent when the write event
   * callback is called.
   */
  dsm_watermark_type rm_write_wm;
  dsm_watermark_type um_write_wm;

  /* The size of the watermark DNE, apperently in bytes */
#define DNS_PROXY_SEND_WM_SZ (2000)

  /* This is the queue head needed by the above watermark */
  q_type  rm_write_wm_q;
  q_type  um_write_wm_q;

  /* sanity self-pointer */
  struct ps_dns_proxy_info_s * this_ptr;
} ps_dns_proxy_info_type;

/*===========================================================================

                                MACROS

===========================================================================*/
/*---------------------------------------------------------------------------
MACRO DNS_PROXYI_INFO_IS_ENABLED

DESCRIPTION
  Is DNS control block information valid and DNS proxy has been enabled?
---------------------------------------------------------------------------*/
#define DNS_PROXYI_INFO_IS_ENABLED( dns_proxy_info_ptr )                  \
(                                                                         \
  (dns_proxy_info_ptr != NULL) &&                                         \
  (ps_mem_is_valid( dns_proxy_info_ptr,                                   \
                     PS_MEM_DNS_PROXY_INFO_TYPE ) ) &&                    \
  (dns_proxy_info_ptr->this_ptr == dns_proxy_info_ptr) &&                 \
  (PS_IFACE_IS_VALID(dns_proxy_info_ptr->iface_ptr))                      \
)

/*===========================================================================

                            FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_DNS_PROXY_ENABLE_CMD_HDLR()

DESCRIPTION
  Handler for the DNS proxy enable command - enables the DNS Proxy.

PARAMETERS
  ps_cmd : IN : command to process
  data_ptr : IN : Pointer to the control block information

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
===========================================================================*/
void
ps_dns_proxy_enable_cmd_hdlr
( 
  ps_cmd_enum_type   ps_cmd,
  void             * data_ptr
);

/*===========================================================================
FUNCTION PS_DNS_PROXY_DISABLE_CMD_HDLR()

DESCRIPTION
  Handler for the DNS proxy enable command - disables the DNS Proxy.

PARAMETERS
  ps_cmd : IN : command to process
  data_ptr : IN : Pointer to the control block information

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
===========================================================================*/
void
ps_dns_proxy_disable_cmd_hdlr
( 
  ps_cmd_enum_type   ps_cmd,
  void             * data_ptr
);

/*===========================================================================
FUNCTION PS_DNS_PROXY_SOCK_EVENT_CMD_HANDLER()

DESCRIPTION
  This function is called when the sockets layer notifies the DNS PROXY
  of events of interest.

PARAMETERS
  ps_cmd     - Should be PS_DNS_PROXY_SOCK_EVENT_CMD
  data_ptr   - Points to the dns proxy sock event info.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void 
ps_dns_proxy_sock_event_cmd_handler
(
  ps_cmd_enum_type   ps_cmd,
  void             * data_ptr
);

/*===========================================================================
FUNCTION PS_DNS_PROXY_IFACE_EV_CMD_HANDLER()

DESCRIPTION
  This function is called when the PS notifies the DNS PROXY
  of IFACE CONFIGURING event on UM iface

PARAMETERS
  ps_cmd     - Should be PS_DNS_PROXY_IFACE_EVENT_CMD
  data_ptr   - Points to the iface pointer and event.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void 
ps_dns_proxy_iface_ev_cmd_hdlr
(
  ps_cmd_enum_type   ps_cmd,
  void             * data_ptr
);

#endif /* __PS_DNS_PROXY_H_ */
