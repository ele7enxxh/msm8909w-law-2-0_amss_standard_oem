#ifndef PS_PPP_SNPIP_H
#define PS_PPP_SNPIP_H
/*===========================================================================

                          P S _ P P P _ S N P I P . H

DESCRIPTION

  This file provides declarations to support ppp device level snooping on all 
  IP packets. The file contains the state machine implementation and actual 
  snooping functions to snoop on all IP packets including - 
  UDP, ICMP, non compresed TCP, VJUCH and VJCH.  

EXTERNALIZED FUNCTIONS
  snoop_ip_init()
    Initialize the IP Snoop State Machine.
  snoop_ip_post_event()
    Post an event to IP Snoop State Machine.

Copyright (c) 2001 - 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snpip.h_v   1.0   13 Feb 2003 14:14:26   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_snpip.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    am     DS Task De-coupling effort.
05/02/03    usb    created file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "ps_ppp_defs.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  SNOOP_IP_MIN_EV                = -1,
  SNOOP_IP_PPP_DEV_UP_EV         = 0, /* PPP device is up for IP           */
  SNOOP_IP_PPP_DEV_DOWN_EV       = 1, /* PPP device is down                */
  SNOOP_IP_FILTERING_ENABLED_EV  = 2, /* IP filtering enabled on iface     */
  SNOOP_IP_FILTERING_DISABLED_EV = 3, /* IP filtering disabled on iface    */
  SNOOP_IP_COMP_ON_EV            = 4, /* VJ compressed pkts being xferred  */
  SNOOP_IP_COMP_OFF_EV           = 5, /* No more VJ compressed pkts        */
  SNOOP_IP_MAX_EV
} snoop_ip_event_type;
           
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION SNOOP_IP_INIT()

  DESCRIPTION
    This function initializes all the instcances of the IP snooping state 
    machine (one for each PPP device).  It also registers all the command 
    handlers with PS task interface.

  PARAMETERS
    None
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    Resets all the control block variables.
===========================================================================*/
void snoop_ip_init(void);


/*===========================================================================
  FUNCTION SNOOP_IP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to the SM and enqueues the
    corresponding command to be processed in the PS task.

  PARAMETERS
    dev: PPP device type to indentify associated SM instance
    event: The event that is being posted to the SM.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
void snoop_ip_post_event
(
  ppp_dev_enum_type     dev,
  snoop_ip_event_type   event
);

#endif /* PS_PPP_SNPIP_H */

