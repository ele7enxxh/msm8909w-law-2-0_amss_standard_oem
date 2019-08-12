/*===========================================================================

     D A T A  S E R V I C E S   M O B I L E   I P   I / O   A P I  F I L E
                   
DESCRIPTION

 The Data Services Mobile IP I/O implementation.  Contains API and internal
 functions for Mobile IP connection control.
 
EXTERNALIZED FUNCTIONS

  mipio_init()
    Initializes all control structures for Mobile IP and opens all the
    sockets to support the specified # of sessions.  Must be called 
    before the mobile IP service is started (i.e. before any function below).

  mipio_write()
    Sends specified data to specified session.

  mipio_handle_socket_event
    Callback to handle events on the mobile IP socket, called by the ps task

Copyright (c) 2000-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_io.c_v   1.25   19 Feb 2003 15:57:20   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_io.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
05/20/11    op     Migrated to MSG 2.0 macros
05/14/10    ls     Move MIPIO_MAX_SOCKS from this file to header file
12/17/09    ms     Compiler Warning fix.
09/18/09    ss     DataCommon CMI SU level impacts.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
05/23/07    vp     MIP CCoA related changes.
03/19/06    as     Set the TTL to be 255 for all MIP sockets but ICMP TX
09/02/06    jd     Log error if can't set ICMP TX socket TTL to 1
10/24/05   as/vrk  Fixed coverity error.
08/24/04    usb    Removed DSS ioctl support for filtering, using dssocki API
06/22/04    kvd    Added functions to select/deselect on MIP ICMP sockets
                   to hold readign Router advs until MIP metasm is UP.
04/16/04    jd     Use ps_iface_ioctl directly instead of dss_iface_ioctl. 
02/18/04    usb    IP filtering library interface change.
02/02/04    jd     Use dss_socket2() instead of netlib callbacks.
                   MIP is triggered by dssnet instead of network events now.
                   Moved sockfd into mipioi_info struct.
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
11/13/03    jd     Cache the MIP interface ID
11/13/03    jd     Clear write events on MIP sockets at end of call,
                   Comment and MSG() cleanup
10/31/03    usb    Defined mipio_set_iface_id(). Modified network callback
                   to use dss_netstatus() rather than dss_iface_ioctl() 
                   to get the network status.
10/28/03    sv     Modified dss_get_iface_id calls to use the new API.
10/28/03    sv     Changed code to use dss_set_app_net_policy instead of
                   hard coding the values.
10/09/03    jd     Added APIs to start and stop IP filtering for MIP messages
09/23/03    jd     Removed FEATURE_DATA_MM_SINGLE_MODE_TARGET from mainline
08/28/03    ss     Updated to use modified ioctl() call
08/06/03    jd     Featurized set_routeable_app_opt under 
                   FEATURE_DATA_MM_SINGLE_MODE_TARGET to remove dependency 
                   for 6100 target
07/20/03    usb    Setting routeable option for MIP App.
05/14/03    jd     Force Um interface during pppopen (after iface is created)
05/05/03    jd     Force Um to use 1x interface
                   For multimode builds, removed dependencies on dsiface
05/02/03    usb    Passing errno param to dss_close().
04/23/03    usb    Correctly passing in the from_addr buffer length to 
                   recvfrom() instead of NULL.
12/29/02    jd     Changed mipio_handle_*_event prototypes to match multimode
                   sig handler signature, ret val. ignored in old arch.
11/19/02    jd     For MM, don't call mip_iface_in_use() (done in pppopen),
                   don't post UM_MIP_DOWN to RMSM on net_ev w/ iface UP
11/14/02    jd     call dssocki_mip_iface_in_use in MM instead of ppp_in_use
11/14/02    jd     changed mipioi_app_id unassigned value to APPAVAIL to 
                   ensure dssocki_is_iface_in_use works correctly.
05/10/02    jd     moved definition of rx buffer size here from dsmip.h
01/02/02    aku    dss_ppp_in_use() now returns uint instead of a boolean.
11/12/01    aku    Replaced call to dssocki_is_socket_app_active() with
                   dss_ppp_in_use()
11/09/01    aku    mipio_ppp_open() now returns mipio_pppopen_ret_val type
                   instead of a boolean
09/04/01    aku    Notify MIP meta sm when iface is closed.
08/23/01    aku    Renamed dss_getifacestatus as dss_get_iface_status
08/17/01    aku    Added accessor function to access mipioi_app_id
08/14/01    jd     pass sender's IP address to mip_parse_msg()
08/07/01    jd     mipio_write() returns completion status
08/02/01    jd     added a debug message to report reception of net event
07/31/01    sjy    Renamed RMSM_UM_IFACE_DOWN to DS707_RMSM_UM_MIP_DOWN_EV
07/30/01    na     Added some messages and removed an unecessary ASSERT
07/30/01    aku    Posting event to RM SM when Um iface comes up with SIP
07/27/01    na     Fixed mipio_handle_network_event so that we return UM_MIP
                   _UP_EV to rmsm.
07/12/01    jd     now register for write event notification if write fails.
                   removed close event code in mipioi_event_helper (unused).
                   code cleanup (linting and code review).
                   removed fromaddr from mipio_handle_sock_event (unused).
07/06/01    jd     removed mipio_shutdown as we changed the design to 
                   initialize on startup and never call shutdown
06/28/01    mvl    modified some debug messages to reduce debug chatter
06/26/01    jd     use setsockopt to set all mobile IP sockets to system mode 
06/18/01    jd     ignore dss_close_netlib result in mipio_shutdown 
06/13/01    mvl    updated references to match names in session info block.
06/11/01    jd     changed rx buffer declaration to ensure long word 
                   alignment
06/07/01    jd     replaced conn_cb_type with socket descriptor array
05/26/01    jd     removed: fromaddr check in sock event cb, bind to UDP port
09/26/00    jd     Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DS_MOBILE_IP

#include <string.h>
#include "amssassert.h"
#include "dssocket.h"
#include "dssicmp_api.h"
#include "dssinternal.h"
#include "err.h"
#include "queue.h"
#include "task.h"

#include "ps_mip_task.h"
#include "dsmip_io.h"
#include "dsmip_parse.h"
#include "dsmip_solsm.h"
#include "dsmip_regsm.h"
#include "dsmip_metasm.h"
#include "dsmip_msg.h"

#include "ps_svc.h"
#include "dssocket_defs.h"

#include "ps_iface.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  MIP_MN_MAX_RXMSG_LEN is sufficient to hold
  - agent advertisement + expected extensions, or
  - registration reply + expected extensions
---------------------------------------------------------------------------*/
#define MIP_MN_MAX_RXMSG_LEN   (1024)

/*---------------------------------------------------------------------------
  xCEIL - macro to give "the minimum number of units of size 's' bytes 
          required to contain 'x' bytes. 
  LCEIL - macro to give the minimum number of long words (uint32's) that 
          will contain 'x' bytes.
---------------------------------------------------------------------------*/
#define xCEIL(x,s)  ((x+(s-1))/s)
#define LCEIL(x)    xCEIL(x,sizeof(uint32))

/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

/*---------------------------------------------------------------------------
  Flag to track the initialization status of mobile IP i/o library
---------------------------------------------------------------------------*/
LOCAL boolean  mipioi_initialized = FALSE;   /* flag - true if initialized */

/*---------------------------------------------------------------------------
  Read buffer for incoming messages on MIP sockets
---------------------------------------------------------------------------*/
LOCAL uint32  mipioi_read_buffer[LCEIL(MIP_MN_MAX_RXMSG_LEN)];

/*---------------------------------------------------------------------------
  Control block for MIP IO Control.
  NOTE: This struct should contain all the data used by MIP IO.
---------------------------------------------------------------------------*/
struct
{
  ps_iface_type *   iface_ptr;
  sint15            sd[MIPIO_MAX_SOCKS];        /* socket descriptor array */
} mipioi_info;


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*lint -save -e641 Reason for suppressing error 641*/

/*===========================================================================

FUNCTION MIPIOI_SOCK_EVENT_CB

DESCRIPTION
  Socket event callback.  Sends command to PS to call MIP socket event 
  handler.

PARAMETERS
  appid - ignored
  sockfd - ignored
  event_mask - event that occurred, copied to command
  identifier - mipio socket identifier (user data), copied to command

RETURN VALUE
  None.

DEPENDENCIES
  mipio_open() must be called successfully before mipioi_sock_event_cb()
  is called.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void mipioi_socket_cb
(
  sint15 appid,                                          /* Application id */
  sint15 sockfd,                                      /* socket descriptor */
  uint32 event_mask,                                     /* Event occurred */
  void * identifier                               /* MIP socket identifier */
)
{
  uint32  cmd_data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Code socket event into cmd data field

     3      2 2      1 1      0 0      0
     10987654 32109876 54321098 76543210
    +--------+--------+--------+--------+
    | mip_id |  none  |  none  | events |
    +--------+--------+--------+--------+

    where
      mip_id is the connection identifier
      events is the lower 8 bits of the event mask (WRITE/READ)
        -> assert to be sure there are no events in the upper 24 bits
      none is set to 0
  -------------------------------------------------------------------------*/
  ASSERT (!(event_mask >> 8));
  cmd_data_ptr = (event_mask & 0x000000FFU) | (((uint32) identifier) << 24);
 
  ps_send_cmd(MIP_SOCKET_EVENT_CMD, (void*)cmd_data_ptr);

} /* mipioi_sock_event_cb() */



/*===========================================================================

FUNCTION MIPIOI_CLOSE_ALL_SOCKETS

DESCRIPTION
  Closes all MIPIO socket connections.  Doesn't break if the socket is not 
  open.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_close_all_sockets
(
  void
)
{
  sint15  result;                 /* holds return value for called fns     */
  uint8   i;                      /* loop counter                          */
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Close all connections 
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    /*-----------------------------------------------------------------------
      If the connection is not open (indicated by sd=DSS_ERROR), skip it
    -----------------------------------------------------------------------*/
    if (mipioi_info.sd[i] == DSS_ERROR) 
    {
      continue;
    }
  
    /*-----------------------------------------------------------------------
      Close the specified socket, this will never block since we're dealing
      with UDP & ICMP sockets
    -----------------------------------------------------------------------*/
    result = dss_close( mipioi_info.sd[i], &l_errno );
    if (result == DSS_ERROR)
    {
      DATA_MIP_MSG3(MSG_LEGACY_ERROR,
                    "Error %d in closing MIP i/o connection #%d (sd=%d)",
                    l_errno, i, mipioi_info.sd[i]); 
    }
  
    /*-----------------------------------------------------------------------
      Set the socket descriptor value to DSS_ERROR to indicate 'not in use'
    -----------------------------------------------------------------------*/
    mipioi_info.sd[i] = DSS_ERROR;
  }
} /* mipioi_close_all_sockets() */



/*===========================================================================

FUNCTION MIPIO_GET_MIP_POLICY

DESCRIPTION
  Fills provided policy structure with MIP policy for sockets, route_get, etc.

PARAMETERS
  None.

RETURN VALUE
  pol_ptr   policy structure to be populated

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mipio_get_mip_policy
(
  dss_net_policy_info_type * pol_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dss_init_net_policy_info( pol_ptr );
  pol_ptr->iface.kind      = DSS_IFACE_NAME;
  pol_ptr->iface.info.name = DSS_IFACE_CDMA_SN;
  pol_ptr->family          = DSS_AF_INET;
  pol_ptr->is_routeable    = TRUE;

#ifdef FEATURE_DATA_PS_MIP_CCOA
  /* Send registration etc. on physical interface */
  pol_ptr->sip_iface_reqd = TRUE;
#endif

} /* mipio_get_mip_policy() */



/*===========================================================================

FUNCTION MIPIOI_OPEN_SOCKET

DESCRIPTION
  Opens the socket of the type and protocol specified.

PARAMETERS
  prot - protocol of socket to open (PS_IPPROTO_ICMP | PS_IPPROTO_UDP)
  type - for ICMP -> ICMP msg type to receive with this socket
         for UDP  -> DSS_SOCK_DGRAM
  code - ICMP message code to bind to this socket (not used for UDP)

RETURN VALUE
  socket descriptor or DSS_ERROR if the open failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static sint15 mipioi_open_socket
( 
  byte    prot,
  uint8   type,
  uint32  identifier      /* value passed back in socket_cb() as user_data */
)
{
  dss_net_policy_info_type  net_policy; /* network policy                  */
  sint31  result;                 /* holds return value for called fns     */
  sint15  sd;                     /* opened socket descriptor              */
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd = DSS_ERROR;

  /*-------------------------------------------------------------------------
    This function is called internal to this file, so we know that prot
    will only be ICMP or UDP and thus no range checking is performed here.
    Assertion included for development debugging.
  -------------------------------------------------------------------------*/
  ASSERT (prot == PS_IPPROTO_ICMP || prot == PS_IPPROTO_UDP);
  ASSERT (identifier < MIPIO_MAX_SOCKS);

  if (prot == PS_IPPROTO_ICMP)
  {
    /*-----------------------------------------------------------------------
      Setup socket policy for Mobile IP ICMP sockets
      - force routing to resolve CDMA SN interface (iface)
      - Allow socket to write to routeable interface (is_routeable)
    -----------------------------------------------------------------------*/
    mipio_get_mip_policy( &net_policy );
    sd = dssicmp_socket( DSSICMP_ICMP_V4,
                         type,
                         0,
                         mipioi_socket_cb,
                         (void *) identifier,
                         &net_policy, 
                         &l_errno);
  }
  else /* (prot == PS_IPPROTO_UDP) */
  {
    /*-----------------------------------------------------------------------
      Setup socket policy for Mobile IP UDP socket(s)
      - force routing to resolve CDMA SN interface (iface)
      - Allow socket to write to routeable interface (is_routeable)
    -----------------------------------------------------------------------*/
    mipio_get_mip_policy( &net_policy );
    sd = dss_socket2( DSS_AF_INET,
                      type,
                      prot,
                      mipioi_socket_cb,
                      (void *) identifier,
                      &net_policy,
                      &l_errno);
  }

  /*-------------------------------------------------------------------------
    If a socket couldn't be opened, close all and return failure 
  -------------------------------------------------------------------------*/
  if (sd == DSS_ERROR)
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Couldn't open socket!");
    return (DSS_ERROR);
  } 

  /*-------------------------------------------------------------------------
    Ask to be notified when data arrives on the socket (readable).  
    We assume that the socket is writeable as soon as it is opened.
    If dss_async_select fails, the socket descriptor is corrupt, so return
    and close gracefully (socket will be closed in mipioi_close_sockets).
  -------------------------------------------------------------------------*/
  result = dss_async_select( sd, DS_READ_EVENT, &l_errno );
  if (result != DSS_SUCCESS)
  {
    /*-----------------------------------------------------------------------
      If a socket couldn't be opened, close all and return failure 
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Reg for read event failed! (sd=%d)", sd);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Return the opened socket descriptor
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Opened MIP socket %d (sd=%d)", identifier, sd);
  return (sd);

} /* mipioi_open_socket() */



/*===========================================================================

FUNCTION MIPIOI_OPEN_ALL_SOCKETS

DESCRIPTION
  Opens the socket of the type and protocol specified.

PARAMETERS
  None

RETURN VALUE
  TRUE - all sockets opened ok
  FALSE - something failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean mipioi_open_all_sockets
(
  void
)
{  
  uint8   i;                      /* loop counter                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Open one UDP sockets for each session (identifier = connection ID = i)
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIP_MAX_SESSIONS; i++)
  {
    mipioi_info.sd[i] = mipioi_open_socket ( PS_IPPROTO_UDP, DSS_SOCK_DGRAM, i );
    if (mipioi_info.sd[i] == DSS_ERROR)
    {
      return (FALSE);
    } 
  } /* for (all sessions) */

  DATA_MIP_MSG1(MSG_LEGACY_MED, "MIP supports %d concurrent session(s)",i);

  /*-------------------------------------------------------------------------
    Open the ICMP RX socket, type = MIP_AAM_TYPE, id = ICMP_RX_HANDLE
  -------------------------------------------------------------------------*/
  mipioi_info.sd[MIPIO_ICMP_RX_HANDLE] = 
    mipioi_open_socket ( PS_IPPROTO_ICMP,
                         ICMP_MSGTYPE_ROUTER_ADVERTISEMENT,
                         MIPIO_ICMP_RX_HANDLE );
  if (mipioi_info.sd[MIPIO_ICMP_RX_HANDLE] == DSS_ERROR)
  {
    return (FALSE);
  }

  /*-------------------------------------------------------------------------
    Open the ICMP TX socket, type = MIP_ASM_TYPE
  -------------------------------------------------------------------------*/
  mipioi_info.sd[MIPIO_ICMP_TX_HANDLE] = 
    mipioi_open_socket ( PS_IPPROTO_ICMP,
                         ICMP_MSGTYPE_ROUTER_SOLICITATION,
                         MIPIO_ICMP_TX_HANDLE ); 
  if (mipioi_info.sd[MIPIO_ICMP_TX_HANDLE] == DSS_ERROR)
  {
    return (FALSE);
  } 

  return TRUE;

} /* mipioi_open_all_sockets() */



/*===========================================================================

FUNCTION MIPIOI_EVENT_HELPER

DESCRIPTION
  Processes current events on the specified socket.

  This section of code is seperated simply for clarity and compactness.

PARAMETERS
  index - connection number that event occurred on.
  event - event that occurred (as defined in dssocket.h), 
          (a single event, not the event mask!)
  l_errno - errno returned from dss_*() call

RETURN VALUE
  None.

DEPENDENCIES
  None. 

SIDE EFFECTS
  None.

===========================================================================*/
static void mipioi_event_helper
( 
  int  index,                   /* connection index that event occurred on */
  int  event_mask,               /* current event mask */
  sint15  *l_errno              /* errno returned from dss_recvfrom() call */
)
{
  struct ps_sockaddr_in  fa;       /* for recvfrom(), the sender's address */
  uint16                 fa_addr_len; /* sender's address buffer length    */
  sint15                 result;   /* return value for called functions    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (event_mask & DS_WRITE_EVENT)
  {
    /*-----------------------------------------------------------------------
      A write event indicates that a write to the socket is possible.  

      Send a retry event to the appropriate state machine - 

      connections 0..MAX_SESSIONS-1 are UDP sockets
        which imply kicking the registration state machine, 
      connection  MAX_SESSIONS   is the ICMP RX socket
      connection  MAX_SESSIONS+1 is the ICMP TX socket
        which imply kicking the solicitation state machine.

      MIPIO only registers for this event when a previous write failed.
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_LOW,  "DS_WRITE_EVENT on connection %d", index);
    if (index < MIP_MAX_SESSIONS) 
    {
      mip_reg_sm_post_event (INDEX_2_MIP_SES_PTR(index), 
                             RSM_IMMED_RETRY_EV);
    }
    else
    {
      mip_sol_sm_post_event (SSM_IMMED_RETRY_EV);
    }
    event_mask &= ~DS_WRITE_EVENT;
  }
 
  if (event_mask & DS_READ_EVENT)
  {
    /*-----------------------------------------------------------------------
      A read event on the ICMP socket indicates data has arrived, so read 
      it.  Pass the data along to the parser.
    -----------------------------------------------------------------------*/

    DATA_MIP_MSG1(MSG_LEGACY_MED,  "DS_READ_EVENT on connection %d", index);

    while (TRUE)
    {
      DATA_MIP_MSG0(MSG_LEGACY_LOW, "polling for message");
      /*---------------------------------------------------------------------
        Read data off socket, pass to parser.  Returns length of received
        message (i.e. stored in 'result')
      ---------------------------------------------------------------------*/
      fa_addr_len = sizeof(fa);
      result = dss_recvfrom ( mipioi_info.sd[index], 
                              mipioi_read_buffer, 
                              sizeof(mipioi_read_buffer),
                              0, 
                              (struct ps_sockaddr *) &fa,
                              &fa_addr_len, 
                              (sint15 *) l_errno );
      if (result == DSS_ERROR)
      {
        break;
      }

      /*---------------------------------------------------------------------
        pass the data along to the parser.
      ---------------------------------------------------------------------*/
      mip_parse_msg (index,mipioi_read_buffer, fa.ps_sin_addr.ps_s_addr, result); 
    }
    event_mask &= ~DS_READ_EVENT;
  }

} /* mipioi_event_helper() */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIPIO_INIT

DESCRIPTION

  Opens and configures all sockets required for Mobile IP messaging.

  One UDP socket is required for each session, over which registration
  request messages (RRQ's) are sent and registration reply messages (RRP's)
  are received.

  Two ICMP sockets total are required for Mobile IP to handle mobility
  agent (i.e., foreign or home agent) advertisements (AAM's) and
  solicitations (ASM's).

  The DMSS ICMP implementation avoids wasting memory on extraneous messages
  by assigning a type filter to each socket.  The socket will receive ICMP
  messages of that type only, and will assign that type to any messages 
  sent on that socket.  Thus 2 sockets are required since ASM and AAM have 
  unique types.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - initialized ok (or already initialized)
  FALSE - initialization failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_init
(
  void
)
{
  uint32  optval;                 /* option value for dss_setsockopt()     */
  uint32  optlen;                 /* length of above option value          */
  sint15  l_errno;                /* errno returned from dss_socket() call */
  sint15  sock_err;                /* return value of dss_setsockopt       */
  uint8   i;                      /* loop counter                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to be sure this is the first call to mipio_init() as this should
    only be done once
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == TRUE)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Initialize all socket descriptors to DSS_ERROR to indicate 'not in use'
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    mipioi_info.sd[i] = DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Open sockets for MIP client.  Provide socket callback function for 
    each, as mobile IP does not open netlib or PPP, and thus does not 
    receive network events via app callback.
  -------------------------------------------------------------------------*/
  if( !mipioi_open_all_sockets() )
  {
    /*-----------------------------------------------------------------------
      If any socket open fails, close all and return failure 
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Couldn't open mobile IP sockets!");
    mipioi_close_all_sockets();
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Set the System Socket option for all MIP sockets
  -------------------------------------------------------------------------*/
  for (i = 0; i < MIPIO_MAX_SOCKS; i++)
  {
    optval = 1;
    optlen = sizeof(uint32);
    (void) dss_setsockopt ( mipioi_info.sd[i], 
                            DSS_SOCK, 
                            (int) DSS_SO_SYS_SOCK, 
                            &optval,
                            &optlen,
                            &l_errno );

    optval = 255;
    optlen = sizeof(uint32);
    sock_err = dss_setsockopt ( 
                          mipioi_info.sd[i], 
                          DSS_IPPROTO_IP, 
                          (int) DSS_IP_TTL, 
                          &optval,
                          &optlen,
                          &l_errno );
    if (sock_err == DSS_ERROR)
    {
      DATA_MIP_MSG2(MSG_LEGACY_ERROR,
                    "Couldn't set TTL to 255 (errno=%d) for MIP socket %d!",
                    l_errno,i);
    }
  }

  /*-------------------------------------------------------------------------
    Set the TTL to 1 for ICMP TX socket, as req'd for agent solicitations.
      (which are the only msgs we send on this socket).
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_MIP_CCOA
  optval = 2;
#else
  optval = 1;
#endif
  optlen = sizeof(uint32);
  sock_err = dss_setsockopt ( mipioi_info.sd[MIPIO_ICMP_TX_HANDLE], 
                          DSS_IPPROTO_IP, 
                          (int) DSS_IP_TTL, 
                          &optval,
                          &optlen,
                          &l_errno );
  if (sock_err == DSS_ERROR)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR,
                  "Couldn't set TTL to 1 (errno=%d) for MIP ICMP tx socket!",
                  l_errno);
  }


  /*-------------------------------------------------------------------------
    Clear the ID of the interface which MIP is configuring
  -------------------------------------------------------------------------*/
  mipioi_info.iface_ptr = NULL;

  /*-------------------------------------------------------------------------
    Initialization complete, transition to idle state
  -------------------------------------------------------------------------*/
  mipioi_initialized = TRUE;

  return (TRUE);

} /* mipio_init() */



/*===========================================================================

FUNCTION MIPIO_HANDLE_SOCKET_EVENT

DESCRIPTION
  Process socket event(s) on Mobile IP socket specified in identifier

PARAMETERS
  identifier - socket on which event occurred
  event_mask - current event_mask for socket

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
void mipio_handle_socket_event
(
  uint32  identifier,                            /* MIP socket identifier  */
  uint32  event_mask                             /* Event(s) that occurred */
)
{
  sint31  result;                     /* return value for called functions */
  sint15  l_errno = -1;               /* errno returned from dss_*() call  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == FALSE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIPIO not initialized!");
    return;
  }

  /*-------------------------------------------------------------------------
    Handle the event
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG2(MSG_LEGACY_MED,  "Socket event mask %x on MIP socket %d",event_mask,identifier);
  mipioi_event_helper (identifier, event_mask, &l_errno);

  if (l_errno == DS_EWOULDBLOCK)
  {

    /*-----------------------------------------------------------------------
      Rearm the read event notification on this socket
    -----------------------------------------------------------------------*/
    result = dss_async_select( mipioi_info.sd[identifier], 
                               DS_READ_EVENT, 
                               &l_errno );
    ASSERT (result == DSS_SUCCESS);
  }

  else
  {
    /*-----------------------------------------------------------------
    Unexpected error (busted sockets, etc.) - ignore.
    If this actually happens, the socket should be reinitialized
    (this should never happen though).
    -----------------------------------------------------------------*/
    DATA_MIP_MSG2(MSG_LEGACY_ERROR, "Error on socket read (sd=%d), errno = %d",
                  mipioi_info.sd[identifier], l_errno);
  }

  DATA_MIP_MSG0(MSG_LEGACY_LOW, "Done processing mip socket event");
} /* mipio_handle_socket_event() */



/*===========================================================================

FUNCTION MIPIO_WRITE

DESCRIPTION
  Writes data to a mipio connection.

PARAMETERS
  index     - connection # to write to (see table above for semantics)
  data      - pointer to data to be written
  data_len  - length of data pointed to by 'data'
  dest_ip   - destination IP address
  dest_port - destination port number (ignored for ICMP messages)

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mipio_write
(
  uint8   index,                           /* session number to write to   */
  void *  data,                            /* data to be written           */
  uint16  data_len,                        /* length of above data         */
  uint32  dest_ip,                         /* destination IP address       */
  uint16  dest_port                        /* destination IP port          */
)
{
  struct ps_sockaddr_in  dest;       /* message destination                */
  sint31                 result;     /* holds return value for called fns  */
  sint15                 l_errno;    /* errno returned from dss_*() calls  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (mipioi_initialized == FALSE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIPIO not initialized!");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Validate the arguments
  -------------------------------------------------------------------------*/
  if (index >= MIPIO_MAX_SOCKS)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Write to invalid session (%d), ignoring!", index);
    ASSERT (0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
    Initialize variables
  -------------------------------------------------------------------------*/
  dest.ps_sin_family               = DSS_AF_INET;
  dest.ps_sin_port                 = dss_htons(dest_port);
  dest.ps_sin_addr.ps_s_addr       = dest_ip;
  ((sint31 *) dest.ps_sin_zero)[0] = 0;
  ((sint31 *) dest.ps_sin_zero)[1] = 0;

  /*-------------------------------------------------------------------------
    Send current data
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG3(MSG_LEGACY_MED, "sending %d bytes to %x:%d", data_len, 
                                        ps_ntohl(dest.ps_sin_addr.ps_s_addr), 
                                        dest.ps_sin_port);
  result = dss_sendto ( mipioi_info.sd[index], 
                        data,  
                        data_len, 
                        0, 
                        (struct ps_sockaddr *) &dest, 
                        sizeof (struct ps_sockaddr), 
                        &l_errno );

  /*-------------------------------------------------------------------------
    Check to see if the write succeeded.  Since we're writing IP packets
    and UDP datagrams, we can assume that if a write doesn't block, it has
    been fully transmitted over the socket.
  -------------------------------------------------------------------------*/
  if(result == DSS_ERROR)
  {
    if(l_errno == DS_EWOULDBLOCK)
    {
      /*---------------------------------------------------------------------
        Ask to be notified when write is possible, i.e. ppp is synced
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "MIPIO can't send data, registering for write event");

      mip_reg_sm_set_io_failure(TRUE);
      
      result = dss_async_select (mipioi_info.sd[index], 
                                 DS_WRITE_EVENT, 
                                 &l_errno);
      if (result != DSS_SUCCESS)
      {
        DATA_MIP_MSG1(MSG_LEGACY_HIGH, "Reg for write event failed! (sd=%d)", 
                  mipioi_info.sd[index]);
      }
    }
    else
    {
      DATA_MIP_MSG2(MSG_LEGACY_HIGH, "Unexpected write error (%d), session %d",l_errno,index);
    }
    return FALSE;
  }

  return TRUE;

} /* mipio_write() */



/*===========================================================================

  FUNCTION MIPIO_CLEAR_WRITE_EVENT_NOTIFICATION

  DESCRIPTION
    Clears any outstanding write event notification on the Mobile IP tx
    sockets.  This is called at the end of a MIP call to ensure the write
    event doesn't inadvertently trigger sending ASM or RRQ on the next call.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    mipio_init() must have been called previously.

  SIDE EFFECTS
    None.

===========================================================================*/
void mipio_clear_write_event_notification
(
  void  
)
{
  sint15 l_errno;                          /* return error code            */
  sint15 status;                           /* return status value          */
  byte   i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Make sure mipio has been initialized 
  -------------------------------------------------------------------------*/
  if (!mipioi_initialized)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "PPP failed to close - MIPIO not initialized");
    return;  
  }

  /*-------------------------------------------------------------------------
    Clear any pending async_select(WRITE) on the MIP sockets.
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG0(MSG_LEGACY_LOW, "Clear write async select for ICMP and all UDP sockets");

  /*-------------------------------------------------------------------------
    Clear the ICMP TX socket (ICMP RX socket is never written to, so skip)
  -------------------------------------------------------------------------*/
  status = dss_async_deselect( mipioi_info.sd[MIPIO_ICMP_TX_HANDLE], 
                               DS_WRITE_EVENT, 
                               &l_errno );
  ASSERT( status == DSS_SUCCESS );

  /*-------------------------------------------------------------------------
    Clear the UDP socket for each MIP session.  
 
    Revisit this for multiple sessions since write event should only be 
    cleared for the session that is being closed - clear all for now (safe)
  -------------------------------------------------------------------------*/
  for( i = 0; i < MIP_MAX_SESSIONS; i++ )
  {
    status = dss_async_deselect( mipioi_info.sd[i], 
                                 DS_WRITE_EVENT, 
                                 &l_errno );
    ASSERT( status == DSS_SUCCESS );
  }

} /* mipio_clear_write_event_notification() */



/*===========================================================================
FUNCTION MIPIO_GET_IFACE_ID

DESCRIPTION
  Returns the interface ID MIP is operating on.
  
PARAMETERS
  None

RETURN VALUE
  Interface ID which MIP configures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type *  mipio_get_iface_ptr
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Return the iface ID that MIP configures for this call.
  -------------------------------------------------------------------------*/
  return mipioi_info.iface_ptr;

} /* mipio_get_iface_ptr() */



/*===========================================================================
FUNCTION MIPIO_SET_IFACE_ID

DESCRIPTION
  Sets the interface ID MIP is operating on.
  
PARAMETERS
  iface_ptr - Id of the interface to set.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Subsequent calls to mipio_get_iface_ptr() return this ID if valid.
===========================================================================*/
void mipio_set_iface_ptr
(
  ps_iface_type *  iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mipioi_info.iface_ptr = iface_ptr;
  DATA_MIP_MSG1(MSG_LEGACY_MED, "MIP interface ID set to 0x%x", mipioi_info.iface_ptr);

} /* mipio_set_iface_ptr() */


/*===========================================================================
FUNCTION MIPIO_START_ICMP_SNOOPING()

DESCRIPTION
  Install IP filters to get mobile IP ICMP messages during re-registration
  
PARAMETERS
  None

RETURN VALUE
  TRUE if successful in installing filters
  FALSE for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mipio_start_icmp_snooping
(
  void
)
{
  ip_filter_type                        filter;
  ps_iface_type                         *iface_ptr;
  sint15                                ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MIP_MSG0(MSG_LEGACY_MED, "Registering MIP ICMP filters");

  /*-------------------------------------------------------------------------
    Get the MIP interface ID
  -------------------------------------------------------------------------*/
  if( (iface_ptr = mipio_get_iface_ptr()) == NULL )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Register the ICMP AAM filter with IP filtering module via iface_ioctl
  -------------------------------------------------------------------------*/
  filter.ip_vsn                   = IP_V4;
  
  filter.ip_hdr.v4.field_mask     = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
  filter.ip_hdr.v4.next_hdr_prot  = PS_IPPROTO_ICMP;

  filter.next_prot_hdr.icmp.field_mask  = IPFLTR_MASK_ICMP_MSG_TYPE;  
  filter.next_prot_hdr.icmp.type        = ICMP_MSGTYPE_ROUTER_ADVERTISEMENT;

  if(dss_reg_ip_filter(mipioi_info.sd[MIPIO_ICMP_RX_HANDLE],
                           PS_IFACE_GET_ID(iface_ptr),
                           IPFLTR_DEFAULT_TYPE,
                           1,
                           (void *)&filter,
                           &ps_errno) < 0)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed %d to reg ICMP filter", ps_errno);
    return FALSE;
  }

  return TRUE;

} /* mipio_start_icmp_snooping() */



/*===========================================================================
FUNCTION MIPIO_START_UDP_SNOOPING()

DESCRIPTION
  Install IP filters to get mobile IP UDP messages during re-registration
  
PARAMETERS
  ses_ix: MIP Session index

RETURN VALUE
  TRUE if successful in installing filters
  FALSE for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mipio_start_udp_snooping
(
  int ses_ix                                          /* MIP Session index */
)
{
  ip_filter_type                        filter;
  ps_iface_type                         *iface_ptr;
  sint15                                ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MIP_MSG1(MSG_LEGACY_MED, "Registering MIP UDP filters ses %d", ses_ix);

  /*-------------------------------------------------------------------------
    Get the MIP interface ID
  -------------------------------------------------------------------------*/
  if( (iface_ptr = mipio_get_iface_ptr()) == NULL )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Register the UDP RRP filter with IP filtering module via iface_ioctl
  -------------------------------------------------------------------------*/
  filter.ip_vsn                   = IP_V4;
  
  filter.ip_hdr.v4.field_mask     = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
  filter.ip_hdr.v4.next_hdr_prot  = PS_IPPROTO_UDP;

  filter.next_prot_hdr.udp.field_mask  = IPFLTR_MASK_UDP_SRC_PORT;  
  filter.next_prot_hdr.udp.src.port    = dss_htons(MIP_UDP_PORTNO);
  filter.next_prot_hdr.udp.src.range   = 0;
  
  if(dss_reg_ip_filter(mipioi_info.sd[ses_ix],
                           PS_IFACE_GET_ID(iface_ptr),
                           IPFLTR_DEFAULT_TYPE,
                           1,
                           (void *)&filter,
                           &ps_errno) < 0)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed %d to reg UDP filter", ps_errno);
    return FALSE;
  }

  return TRUE;

} /* mipio_start_udp_snooping() */



/*===========================================================================
FUNCTION MIPIO_STOP_ICMP_SNOOPING()

DESCRIPTION
  Uninstall ICMP filter to stop snooping for Agent Advertisments
  
PARAMETERS
  None

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mipio_stop_icmp_snooping
(
  void
)
{
  ps_iface_type                         *iface_ptr;
  sint15                                ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MIP_MSG0(MSG_LEGACY_MED, "Deregistering ICMP filters");

  /*-------------------------------------------------------------------------
    Get the MIP interface ID
  -------------------------------------------------------------------------*/
  if( (iface_ptr = mipio_get_iface_ptr()) == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Deregister the ICMP AAM filter with IP filtering module via iface_ioctl
  -------------------------------------------------------------------------*/
  if (dss_dereg_ip_filter(mipioi_info.sd[MIPIO_ICMP_RX_HANDLE],
                              PS_IFACE_GET_ID(iface_ptr),
                              &ps_errno) < 0)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed %d to dereg ICMP filter", ps_errno);
  }

} /* mipio_stop_icmp_snooping() */



/*===========================================================================
FUNCTION MIPIO_STOP_UDP_SNOOPING()

DESCRIPTION
  Uninstall UDP filter for specified session to stop snooping for 
  registration replies
  
PARAMETERS
  ses_ix: MIP Session index

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mipio_stop_udp_snooping
(
  int ses_ix                                          /* MIP Session index */
)
{
  ps_iface_type                        *iface_ptr;
  sint15                               ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MIP_MSG1(MSG_LEGACY_MED, "Deregistering UDP filter ses %d", ses_ix);

  /*-------------------------------------------------------------------------
    Get the MIP interface ID
  -------------------------------------------------------------------------*/
  if( (iface_ptr = mipio_get_iface_ptr()) == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Deregister the UDP RRP filter with IP filtering module via iface_ioctl
  -------------------------------------------------------------------------*/
  if (dss_dereg_ip_filter(mipioi_info.sd[ses_ix],
                              PS_IFACE_GET_ID(iface_ptr),
                              &ps_errno) < 0)
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed %d to dereg UDP filter", ps_errno);
  }

} /* mipio_stop_udp_snooping() */


/*===========================================================================
FUNCTION MIPIO_STOP_READ_FROM_RX_ICMP_SOCK()

DESCRIPTION
  Stop readign AAMs from RX icmp sock, until the metasm is OPEN
  
PARAMETERS
  None

RETURN VALUE
  None
  
DEPENDENCIES
  We dont want to read any data pkts(eg. router advs) until MIP call is UP;
  Deselect on the MIP icmp socket until MIP is UP.

SIDE EFFECTS
  Hold receiving data on MIP ICMP socket  until MIP is up.
===========================================================================*/

void mipio_stop_read_from_rx_icmp_sock
(
  void
)
{
  sint15  status;
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!mipioi_initialized)
   {
     DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIP IO not initialized ");
     return;
   }

   /*-----------------------------------------------------------------------
     Waiting for MIP call to come up. We dont want to read any data pkts(eg.
     router advs) until MIP call is UP; deselect on the MIP icmp socket.
     Clear the ICMP RX
   -----------------------------------------------------------------------*/
   status = dss_async_deselect( mipioi_info.sd[MIPIO_ICMP_RX_HANDLE],
                                DS_READ_EVENT,
                                &l_errno );
   ASSERT( status == DSS_SUCCESS );
}

/*===========================================================================
FUNCTION MIPIO_RESUME_READ_FROM_RX_ICMP_SOCK()

DESCRIPTION
  Resume reading from rx icmp sock ( AAMs) once METASM is in OPEN state
  
PARAMETERS
  None

RETURN VALUE
  None
  
DEPENDENCIES
  We dont want to read any data pkts(eg. router advs) until MIP call is UP;
  Now that MIP is up we do a select on the MIP icmp socket to start 
  receiveing Router Advertisements.

SIDE EFFECTS
  Start receiving data on MIP ICMP socket
===========================================================================*/

void mipio_resume_read_from_rx_icmp_sock
(
  void
)
{
  sint15  status;
  sint15  l_errno;                /* errno returned from dss_*() calls     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!mipioi_initialized)
   {
     DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIP IO not initialized ");
     return;
   }

   /*-----------------------------------------------------------------------
     We dont want to read any data pkts(eg. router advs) until MIP call is UP;
     Now that MIP is up we do a select on the MIP icmp socket to start 
     receiveing Router Advertisements.
   -----------------------------------------------------------------------*/
   status = (sint15) dss_async_select( mipioi_info.sd[MIPIO_ICMP_RX_HANDLE],
                                DS_READ_EVENT,
                                &l_errno );
   ASSERT( status == DSS_SUCCESS );
}

/*===========================================================================
FUNCTION MIPIO_GET_SOCK_DESCRIPTOR_FROM_ARRAY_INDEX()

DESCRIPTION
  This function returns the socket descriptor from the index

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  
SIDE EFFECTS
  
===========================================================================*/

sint15 mipioi_get_sock_descriptor_from_array_index(
  uint8 index
)
{
  if ( index < MIPIO_MAX_SOCKS )
  {
    return mipioi_info.sd[index];
  }
  else
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, 
                  "Error getting socket desc, index: %d",
                  index);
    return -1;
  }
}

/*lint -restore Restore lint error 641*/

#ifdef TEST_FRAMEWORK
#error code not present
#else

boolean mipio_close_all_sockets
(
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MIP_MSG0 (MSG_LEGACY_ERROR, "Method not supported");
  ASSERT(0);
  return FALSE;

} /* mipio_close_all_sockets() */

#endif /* TEST_FRAMEWORK */



#endif  /* FEATURE_DS_MOBILE_IP */
