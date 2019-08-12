#ifndef PS_IP6I_SM
#define PS_IP6I_SM
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I P 6 I _ S M . H

DESCRIPTION
  Internal Header file defining the API for the IPv6 state machine. 

DEPENDENCIES
  The module MUST execute in the PS context.

EXTERNALIZED FUNCTIONS
  ip6_sm_powerup_init
  ip6_sm_init
  ip6_sm_include_rdnss_opt
  ip6_sm_read_nv_efs
  dssps_set_ipv6_enabled_setting
  dssps_get_ipv6_enabled_setting
    
Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_ip6i_sm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/30/14    cx     Added API to deinit and destory the IPv6 state machine
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort [Split from ps_ip6_sm.h].
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IP6_SM_POWERUP_INIT()

DESCRIPTION
  This function associates the SM with the task that initializes it, and
  registers the command handling functions.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_powerup_init
(
  void
);

/*===========================================================================
FUNCTION IP6_SM_INIT()

DESCRIPTION
  This function performs NV related initializations of IP6 state machine.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  This function is called after signalling PS task start.

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_init
(
  void
);

/*===========================================================================

FUNCTION IP6_SM_READ_NV_EFS()

DESCRIPTION
  This function performs NV related initializations of IP6 state machine.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  This function is called after signalling PS task start and also during 
  refresh event.

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_read_nv_efs
(
  void
);

/*===========================================================================
FUNCTION IP6_SM_NCLUDE_RDNSS_OPT()

DESCRIPTION
  This function returns a boolean indicating if outgoing RA packets should 
  contain the RDNSS option

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ip6_sm_include_rdnss_opt
(
  void
);

/*===========================================================================
FUNCTION dssps_set_ip_enabled_setting

DESCRIPTION
  Sets the iPv6 enable setting flag

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssps_set_ipv6_enabled_setting
( 
  boolean ip_v6_enable
);

/*===========================================================================
FUNCTION dssps_get_ip_enabled_setting

DESCRIPTION
   Get the iPv6 enable setting flag

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssps_get_ipv6_enabled_setting
( 
  void
);

#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
#endif /* PS_IP6I_SM */
