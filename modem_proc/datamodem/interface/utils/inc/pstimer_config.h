#ifndef PS_TIMER_CONFIG_H
#define PS_TIMER_CONFIG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S T I M E R_ C O N F I G . H

GENERAL DESCRIPTION
  This is the source file for managing number of PS timers.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_timer_init() MUST be called before any other timer functions are
  called.

EXTERNALIZED FUNCTIONS
  Initialization functions:
    - ps_timer_init(void): Initialize the api timers.

Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/pstimer_config.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/04/14    cx     Increased PS_TIMER_MAX to support 32 wlan clients
07/17/12    ms     C2k Dynamic back pressure removal support for TCP throughput
                   optimization.
10/20/11    ss     Dynamic back pressure for TCP throughput optimization.
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "dss_config.h"
#include "ps_dnsi_defs.h"

#ifdef FEATURE_DATA_PS_PPP
#include "ps_ppp_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Define the maximum number of the timers.
  The number of timers are defined as:

    For sockets, 1 timer per socket for the linger timer, 5 timers per TCP
    socket plus one for each iface (used in the DSSNET state machine when
    waiting for physical iface to come up)

    For Mobile IP, 2 timers for registration, 1 for solication

    3 timers for async data calls when SOCKETS is not defined & for safety.

    2 timers for each PPP instance (LCP/Auth and IPCP)
    1 more timer per PPP instance in MM data for total setup timeout

    1 to time out a DNS query (this timer is freed up upon timeout or upon
      receiving a response to the DNS query)

    1 timer for each IP reassembly descriptor

  Note that this number must be lower than the maximum value that the handler
  can have. The handler is a uint32, but the most significant 16 bits represent
  the cookie value PS_TIMER_HANDLE_COOKIE. Therefore the value of PS_TIMER_MAX
  cannot go above (2^16-1).
---------------------------------------------------------------------------*/
  #define PS_TIMER_SOCKET_TIMERS (DSS_MAX_TCBS * 5 + DSS_MAX_SOCKS)

  #define PS_TIMER_DNS_TIMERS    (PS_DNSI_PS_TIMER)

#ifdef FEATURE_DATA_PS_PPP
#define PS_TIMER_PPP_TIMERS (3 * (uint8)PPP_MAX_DEV)
#else
#define PS_TIMER_PPP_TIMERS (0)
#endif

#define PS_TIMER_IP_REASM_TIMERS (10)

#ifdef FEATURE_DS_MOBILE_IP
// fix this - should be 1 + 3 * (max # sessions)
  #define PS_TIMER_MIP_TIMERS 4
#else
  #define PS_TIMER_MIP_TIMERS 0
#endif

#define PS_TIMER_MAX_3GPP2_TIMERS  1

  #define PS_TIMER_MAX_3GPP_BEARER   8

#ifdef FEATURE_DATA_PS_IPV6
  /*------------------------------------------------------------------------
     These timers are added to support 32 wlan clients. The V6 address cache
     size is increased from 30 to 70 in order to support 1 Modem entity, 
     1 AP entity, 1 USB entity and 32 wlan clients. So additional 40 
     ununsed iid timers are needed. In handoff scenario, the peak usage of 
     timers is doubled to 80
  ------------------------------------------------------------------------*/
  #define PS_TIMER_ADDITIONAL_IPV6_TIMERS 80
#else
  #define PS_TIMER_ADDITIONAL_IPV6_TIMERS 0 
#endif

#define PS_TIMER_MAX (8 +                           \
                      PS_TIMER_SOCKET_TIMERS +      \
                      PS_TIMER_MIP_TIMERS +         \
                      PS_TIMER_DNS_TIMERS +         \
                      PS_TIMER_IP_REASM_TIMERS +    \
                      PS_TIMER_PPP_TIMERS +         \
                      PS_TIMER_MAX_3GPP_BEARER +    \
                      PS_TIMER_MAX_3GPP2_TIMERS +   \
                      PS_TIMER_ADDITIONAL_IPV6_TIMERS)

/*---------------------------------------------------------------------------
  The top 16 bits of each timer handle are a number generated on allocation.
  This ensures the handle was created correctly as well as ensuring
  the 0th entry in the timer array can be used without directly returning
  the index to the array.
---------------------------------------------------------------------------*/
#define PS_TIMER_HANDLE_MASK   0x0000FFFF

/*===========================================================================
FUNCTION PS_TIMER_INIT()

DESCRIPTION
  This function initializes the timer data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_init(void);

/*===========================================================================
FUNCTION PS_TIMER_DEINIT()

DESCRIPTION
  This function cleans up the timer data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ps_timer_deinit(void);


#ifdef __cplusplus
}
#endif

#endif /* PS_TIMERS_H */
