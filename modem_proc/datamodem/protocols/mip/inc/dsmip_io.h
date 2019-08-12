#ifndef _DSMIP_IO_H
#define _DSMIP_IO_H

/*===========================================================================

 D A T A   S E R V I C E S   M O B I L E   I P   I / O   A P I   H E A D E R
                   
DESCRIPTION

 The Data Services Mobile IP I/O Header File.  Contains exported function
 prototypes, definitions and data.

 Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_io.h_v   1.11   29 Dec 2002 16:34:18   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_io.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/10   ls      Move MIPIO_MAX_SOCKS from dsmip_io.c to this header file
12/17/09   ms      Compiler Warning fix.
09/04/09   ss      CMI SU level modifications.
06/22/04   kvd     Added  extern definitions for functions to select/deselect
                   on MIP ICMP sockets to hold readign Router advs until MIP
                   metasm is UP.
04/16/04   jd      Use ps_iface_ioctl directly instead of dss_iface_ioctl.
                   removed unused mipio_get_mip_policy prototype
02/02/04   jd      dss_socket2() callbacks added, removed netlib callbacks.
01/20/04   jd      Mainlined Multimode code (multimode only now)
10/31/03   usb     Added prototype of mipio_set_iface_id().
10/28/03   sv      Added mipio_get_iface_id function declaration.
10/09/03   jd      Moved MIP_UDP_PORTNO here from dsmip_io.c
                   Added function prototypes for MIP IP snooping APIs
12/29/02   jd      Fixed includes for multimode builds.
                   Changed mipio_handle_*_event prototypes to match multimode 
                   sig handler signature, ret val. ignored in old arch.
05/10/02   jd      includes dsmip.h instead of dsmip_regsm.h
                   (moved definition of mip_session_info_type)
01/02/02   aku     Added new macro MIPIO_OPEN_IFACE().
11/09/01   aku     Added enum type for mipio_pppopen_ret_val and 
                   mipio_ppp_open() now returns mipio_pppopen_ret_val instead 
                   of a boolean
08/14/01   aku     Added prototype for accessor function mipioi_get_app_id
07/25/01   jd      added mipio_handle_network_event()
07/20/01   sjy     added function declarations for mipio_ppp_open() and 
                   mipio_ppp_close().
07/12/01   jd      code cleanup (linting and code review)
07/06/01   jd      removed mipio_shutdown as we changed design to initialize
                   on startup and never call shutdown.
06/07/01   jd      replaced conn_cb_type with socket descriptor array
05/25/01   jd      Fixed mipio_handle_sockET_event prototype
09/25/00   jd      Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "ps_iface_defs.h"
#include "dss_netpolicy.h"
#include "dsmip_cfg.h"
#include "dsmip_v.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Definitions of connection mapping table handles for ICMP receive and 
  transmit sockets.

  The connection mapping table maps mip i/o connections to socket 
  descriptors returned by dss_socket().  The logical connections are 
  numbered as follows:
 
  connection          purpose
  -----------         ----------
   0                  UDP socket for Mobile IP session #1
   1                  UDP socket for Mobile IP session #2
   ...                ...
   MIP_MAX_SESSIONS-1 UDP socket for Mobile IP session #(MIP_MAX_SESSIONS)
   MIP_MAX_SESSIONS   ICMP socket to rx ICMP msg type 9 (AAM)
   MIP_MAX_SESSIONS+1 ICMP socket to tx ICMP msg type 10 (ASM)
---------------------------------------------------------------------------*/
#define MIPIO_MAX_SOCKS  (MIP_MAX_SESSIONS+2)

#if (MIPIO_MAX_SOCKS > 255)
  #error MIPIO module does not support > 255 sockets.
#endif

#define MIPIO_ICMP_RX_HANDLE  (MIP_MAX_SESSIONS)    /* ICMP rx socket  */
#define MIPIO_ICMP_TX_HANDLE  (MIP_MAX_SESSIONS+1)  /* ICMP tx socket  */

/*---------------------------------------------------------------------------
  UDP Port to be used for Mobile IP messaging
---------------------------------------------------------------------------*/
#define MIP_UDP_PORTNO    (434)         /* UDP port number for MIP msging  */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
  messages of that type only, and will assign that type to any messages sent
  on that socket.  Thus 2 sockets are required since ASM and AAM have unique
  types. 

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
extern boolean mipio_init
(
  void
);

/*===========================================================================

FUNCTION MIPIO_WRITE

DESCRIPTION
  Writes data to a mipio connection.  

PARAMETERS
  idx       - connection # to write to (see table above for semantics)
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
extern boolean mipio_write
(
  uint8    idx,                            /* session number to write to   */
  void *   data,                           /* data to be written           */
  uint16   data_len,                       /* length of above data         */
  uint32   dest_ip,                        /* destination IP address       */
  uint16   dest_port                       /* destination IP port          */
);

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
extern void mipio_clear_write_event_notification
(
  void
);

/*===========================================================================
FUNCTION MIPIO_GET_IFACE_ID

DESCRIPTION
  Lookup the interface ID which MIP configures
  
PARAMETERS
  None

RETURN VALUE
  Interface ID for MIP if found
  DSS_IFACE_INVALID_ID if interface is not defined

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern ps_iface_type *  mipio_get_iface_id
(
  void
);

/*===========================================================================
FUNCTION MIPIO_SET_IFACE_ID

DESCRIPTION
  Sets the interface ID for use by MIP.
  
PARAMETERS
  iface_id - Id of the interface to set.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Subsequent calls to mipio_get_iface_id() return this ID if valid.
===========================================================================*/
extern void mipio_set_iface_ptr
(
  ps_iface_type *  iface_ptr
);

/*===========================================================================
FUNCTION MIPIO_START_UDP_SNOOPING()

DESCRIPTION
  Install IP filters for mobile IP UDP messages during re-registration
  
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
extern boolean mipio_start_udp_snooping
(
  int ses_ix                                          /* MIP Session index */
);

/*===========================================================================
FUNCTION MIPIO_STOP_UDP_SNOOPING()

DESCRIPTION
  Uninstall IP filters to stop snooping for mobile IP registration pkts
  for the specified session.
  
PARAMETERS
  ses_ix: MIP Session index

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mipio_stop_udp_snooping
(
  int ses_ix                                          /* MIP Session index */
);

/*===========================================================================
FUNCTION MIPIO_START_ICMP_SNOOPING()

DESCRIPTION
  Install IP filters for mobile IP ICMP messages during re-registration
  
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
extern boolean mipio_start_icmp_snooping
(
  void  
);

/*===========================================================================
FUNCTION MIPIO_STOP_ICMP_SNOOPING()

DESCRIPTION
  Uninstall IP filters to stop snooping for mobile IP agent advertisement 
  pkts
  
PARAMETERS
  ses_ix: MIP Session index

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mipio_stop_icmp_snooping
(
  void                                         /* MIP Session index */
);

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

extern void mipio_stop_read_from_rx_icmp_sock (void);

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

extern void mipio_resume_read_from_rx_icmp_sock (void);

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
);

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
extern void mipio_get_mip_policy
(
  dss_net_policy_info_type * pol_ptr
);


/*===========================================================================

FUNCTION MIPIO_CLOSE_ALL_SOCKETS

DESCRIPTION
  Closes all the opened MIP sockets. Used in QTF test case scenario to avoid
  memory leaks. No real use case on target.

PARAMETERS
  None.

RETURN VALUE
  TRUE - if the sockets got closed successfully.
  FALSE - if the method is not supported or if sockets did not close.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_close_all_sockets
(
  void 
);


#endif  /* _DSMIP_IO_H */
