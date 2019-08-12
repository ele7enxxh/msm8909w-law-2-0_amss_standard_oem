#ifndef _DS_RMNET_V4_SM_H
#define _DS_RMNET_V4_SM_H
/*===========================================================================

                        D S _ R M N E T _ V 4 _ S M . H
DESCRIPTION

  The Data Services WWAN Rm State Machine header file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_v4_sm_post_event()
    Post an event to WWAN Rm State Machine.
 
  rmnet_v4_sm_iface()
    Return PS iface in use by RMNET sm

  rmnet_v4_sm_get_um_iface()
     Returns the UM ps_iface ptr for the given RmSM instance  

Copyright (c) 2007-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_v4_sm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/12/11    am     Changed calc_rm_iface_addr to get IP from associated 
                   DHCP server for v4.
10/04/11    sa     Q6 free floating changes for QMI.
10/01/07    mct    Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ps_iface.h"

#include "ds_rmnet_defs.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  RMNET_V4_SM_START_ADDR_CFG_EV         = 0, /* Rm wants packet call       */
  RMNET_V4_SM_GUARD_TIMER_EXPIRATION_EV = 1, /* TE has address             */
  RMNET_V4_SM_DHCP_COMPLETE_EV          = 2, /* TE has address             */
  RMNET_V4_SM_UM_ADDR_CHANGE_EV         = 3, /* Um address changed event   */
  RMNET_V4_SM_STOP_ADDR_CFG_EV          = 4, /* Um address changed event   */
  RMNET_V4_SM_DHCP_STOP_EV              = 5, /* DHCP stop ev               */
  RMNET_V4_SM_MAX_EV
} rmnet_v4_sm_event_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION RMNET_V4_SM_INIT()

  DESCRIPTION
    This function initializes the IPv4 state machine variables.

  PARAMETERS
    instance: The RmNet instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_v4_sm_init
(
  rmnet_instance_e_type  instance
);


/*===========================================================================
  FUNCTION RMNET_V4_SM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to RmNetSM and enqueues the
    corresponding command to be processed in the DCC task.

  PARAMETERS
  instance       : RmNet instance
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_v4_sm_post_event
(
  rmnet_instance_e_type  instance,
  rmnet_v4_sm_event_type    event
);

/*===========================================================================
FUNCTION RMNET_V4_SM_CALC_RM_IFACE_ADDR

DESCRIPTION
  This function is used to calculate the Rm iface IP address and subnet mask
  
PARAMETERS
  um_iface_ptr :  IN 
  rm_ip_addr:  OUT
  subnet_mask:  OUT

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.                                         
===========================================================================*/
extern void rmnet_v4_sm_calc_rm_iface_addr
(
  rmnet_instance_e_type instance,
  uint32 *              te_ip_addr,
  uint32 *              rm_ip_addr,
  uint32 *              subnet_mask
);

/*===========================================================================
FUNCTION RMNET_V4_SM_CLEAR_GUARD_TIMER

DESCRIPTION
  This function is called by QMI_WDS to clear the guard timer and move rmnet
  sm to UP state when client querries ADDR CONFIG info from QMI_WDS
  
PARAMETERS
  instance       : RmNet instance
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.                                         
===========================================================================*/
extern void rmnet_v4_sm_clear_guard_timer
(
  rmnet_instance_e_type  instance
);
#endif /* _DS_RMNET_V4_SM_H */
