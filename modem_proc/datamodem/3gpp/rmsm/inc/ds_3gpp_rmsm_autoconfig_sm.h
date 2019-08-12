#ifndef DS_3GPP_RMSM_AUTOCONFIG_SM_H
#define DS_3GPP_RMSM_AUTOCONFIG_SM_H
/*===========================================================================

             D S  _ 3 G P P _ R M S M _ A U T O C O N F I G _ S M  . H

DESCRIPTION
  This file handles IPv6 autoconfiguration and DHCPv6 on the Rm link.
  This file is used as a Sub-State Machine by RmSm IP to handle IPv6
  related functionality.
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/ds_3gpp_rmsm_autoconfig_sm.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/21/12    vs     Code reviews comments for Ipv6 over PPP + Some cleanup
02/08/12    vs     Added support for 3GPP IPv6 over PPP
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsumts_rmsm.h"
#include "dsumts_rmsmi.h"

/*---------------------------------------------------------------------------
  Declaration of the states of the IPv6 RmSm state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  AUTOCONFIG_SM_MIN_STATE                 = -1,
  AUTOCONFIG_SM_NULL_STATE                = 0,   /* Initial state           */
  AUTOCONFIG_SM_AUTOCFG_IN_PROGRESS_STATE = 1,   /* Addr cfg in progress    */
  AUTOCONFIG_SM_ADDR_CFG_COMPLETE_STATE   = 2,   /* Addr cfg successful     */
  AUTOCONFIG_SM_MAX_STATE
} ds_3gpp_rmsm_autoconfig_sm_state_type;

/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  AUTOCONFIG_SM_START_ADDR_CFG_EV         = 0, /* Start IPv6 addr cfg        */
  AUTOCONFIG_SM_RA_SENT_EV                = 1, /* RA sent to TE              */
  AUTOCONFIG_SM_UM_PREFIX_ADDED_EV        = 2, /* New prefix added on Um     */
  AUTOCONFIG_SM_STOP_ADDR_CFG_EV          = 3, /* Stop IPv6 addr cfg         */
  AUTOCONFIG_SM_DHCP_START_EV     = 4,
  AUTOCONFIG_SM_MAX_EV
} ds_3gpp_rmsm_autoconfig_sm_event_type;


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_INIT

  DESCRIPTION
    This function initializes the IPv6 state machine variables.

  PARAMETERS
    sm_ptr: Pointer to an RmSm IP instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_autoconfig_sm_init
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_INIT_CONFIG

  DESCRIPTION
    This function configures the Neighbor discovery parameters.
    
  PARAMETERS
    start_info: The Neighbor discovery information to populate.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_init_config
(
  dsumts_rmsmi_sm_info_type* sm_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_PORT_EVENT

  DESCRIPTION
 	This function posts an event to Autoconfig Sub-SM and
	enqueues the corresponding command to be processed in the DS task.

  PARAMETERS
    inst ptr: Pointer to the SM instance and Rm Dev instance
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_3gpp_rmsm_autoconfig_sm_event_type  event
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AUTOCONFIG_SM_PROCESS_EV_CMD

  DESCRIPTION
 	This function posts an event to Autoconfig Sub-SM and
	enqueues the corresponding command to be processed in the DS task.

  PARAMETERS
    cmd_ptr - Pointer to the commnd which has to be processed.

  RETURN VALUE
    None

  DEPENDENCIES
    State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_autoconfig_sm_process_ev_cmd
(
  ds_cmd_type  *cmd_ptr
);

#endif /*DS_3GPP_RMSM_AUTOCONFIG_SM_H*/
