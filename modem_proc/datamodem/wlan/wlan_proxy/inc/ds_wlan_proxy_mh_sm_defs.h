#ifndef DS_WLAN_PROXY_MH_SM_DEFS_H
#define DS_WLAN_PROXY_MH_SM_DEFS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  DS WLAN PROXY STM      

GENERAL DESCRIPTION
  This is the WLAN_PROXY state machine grammar for stm2 framework.
 
  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This header file contains events for the input to the wlan proxy state
	machine.
  Notice that changes are listed in reverse chronological order.

 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/12   sc      Created Module
===========================================================================*/
typedef enum
{
	WLAN_PROXY_MIN_EV = 0,
	WLAN_PROXY_BRING_UP_EV = WLAN_PROXY_MIN_EV,
  WLAN_PROXY_SIO_CONFIG_COMPLETE_EV,
  WLAN_PROXY_RM_CONFIG_COMPLETE_EV,
  WLAN_PROXY_RM_DOWN_EV,
  WLAN_PROXY_WLAN_UNAVAILABLE_EV,
  WLAN_PROXY_WLAN_AVAILABLE_EV,
  WLAN_PROXY_TEAR_DOWN_EV,
	WLAN_PROXY_MAX_EV = WLAN_PROXY_TEAR_DOWN_EV
}wlan_proxy_sm_input_event_type;

#endif /*DS_WLAN_PROXY_MH_SM_DEFS_H*/
