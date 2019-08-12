#ifndef _DS_RMNET_V6_SM_H
#define _DS_RMNET_V6_SM_H
/*===========================================================================

                       D S _ R M N E T _ V 6 _ S M . H
DESCRIPTION

  The Data Services WWAN Rm State Machine header file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_sm_post_event()
    Post an event to WWAN Rm State Machine.

  rmnet_sm_iface()
    Return PS iface in use by RMNET sm

  rmnet_sm_get_um_iface()
     Returns the UM ps_iface ptr for the given RmSM instance  

Copyright (c) 2007-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_v6_sm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/19/11    wc     Clear v6 guard timer and save the addr in UM DB if queried.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
10/01/07    mct    Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface.h"

#include "ds_rmnet_defs.h"
#include "ds_rmnet_smi.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  regional feature to auto-connect to WAN if prompted
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  RMNET_V6_SM_START_ADDR_CFG_EV         = 0, /* Start IPv6 addr cfg        */
  RMNET_V6_SM_GUARD_TIMER_EXPIRATION_EV = 1, /* IPv6 guard timer expired   */
  RMNET_V6_SM_RA_SENT_EV                = 2, /* RA sent to TE              */
  RMNET_V6_SM_UM_PREFIX_ADDED_EV        = 3, /* New prefix added on Um     */
  RMNET_V6_SM_STOP_ADDR_CFG_EV          = 4, /* Stop IPv6 addr cfg         */
  RMNET_V6_SM_LL_ADDR_RESOL_EV          = 5, /* LL ADDR Resolved           */
  RMNET_V6_SM_MAX_EV
} rmnet_v6_sm_event_type;


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION RMNET_V6_SM_INIT()

  DESCRIPTION
    This function initializes the IPv6 state machine variables.

  PARAMETERS
    instance: The RmNet instance

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_v6_sm_init
(
  rmnet_instance_e_type  instance
);


/*===========================================================================
  FUNCTION RMNET_V6_SM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void rmnet_v6_sm_post_event
(
  rmnet_instance_e_type  instance,
  rmnet_v6_sm_event_type    event
);

/*===========================================================================
FUNCTION RMNET_V6_SM_CLEAR_GUARD_TIMER

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
extern void rmnet_v6_sm_clear_guard_timer
(
  rmnet_instance_e_type  instance
);
#endif /* _DS_RMNET_V6_SM_H */
#endif /* FEATURE_DATA_PS_IPV6 */
