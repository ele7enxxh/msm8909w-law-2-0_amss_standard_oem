#ifndef DSS_PS_H
#define DSS_PS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    D S    S O C K E T S   P S   T A S K   C M D    H E A D E R   F I L E

GENERAL DESCRIPTION
   This task has the cmd processing for commands sent by the sockets layer
   to PS to be processed.

   These commands include commands to WRITE/CONNECT/CLOSE.

  Copyright (c) 2002,2004-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/06    mct    Added support for RFC3041: IPv6 Privacy Extensions.
06/28/05    mct    Added Mcast event queue.
08/23/04    sv     Split the function dssps_init into powerup_init and init.
07/11/04    aku    Added suport for the QOS feature.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
07/14/02    na/aku   Created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_socket_event_defs.h"


/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern q_type   ps_socket_send_q;

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------
  Global event queue for socket events.
---------------------------------------------------------------------------*/
extern q_type dss_ioctl_qos_event_q;

/*---------------------------------------------------------------------------
  Global event queue for socket multicast events.
---------------------------------------------------------------------------*/
extern q_type dss_ioctl_mcast_event_q;

/*---------------------------------------------------------------------------
  Global event queue for IPv6 Privacy Extensions.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
extern q_type dss_ioctl_ipv6_priv_gen_event_q;
extern q_type dss_ioctl_ipv6_priv_event_q;
#endif /* FEATURE_DATA_PS_IPV6 */


#ifdef __cplusplus
extern "C" {
#endif
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =

                    EXTERNAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSPS_POWERUP_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_powerup_init
(
  void
);
/*===========================================================================
FUNCTION DSSPS_INIT()

DESCRIPTION
  Powerup initialization of the sockets stuff in the PS task.

DEPENDENCIES
  Called at powerup from the PS context

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_init
(
  void
);

void dssps_reg_event_cback
(
  socket_platform_event_cback_f_ptr_type  event_cback_f_ptr
);

/*===========================================================================
FUNCTION DSSPS_SET_NETWORK_POLICY_SETTING

DESCRIPTION
  Sets the Network Policy setting in Sockets config.

DEPENDENCIES

RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void dssps_set_network_policy_setting
(
  ip_addr_enum_type nw_policy
);

#ifdef __cplusplus
}
#endif

#endif /* DSS_PS_H */
