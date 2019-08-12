#ifndef DSUMTS_RMSM_PPP_H
#define DSUMTS_RMSM_PPP_H
/*===========================================================================

                        D S U M T S _ R M S M _ P P P  . H

DESCRIPTION
  The Data Services RM State Machine(PDP-PPP) specific header  file. 
  This contains the state machine specific function definitions and 
  data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsm_ppp.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    vs      Added support for 3GPP IPv6 over PPP
03/04/09    sa      AU level CMI modifications.
04/18/03    vsk     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "dsat_v.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)                    
/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION
    This function initializes the PDP-IP specific state machine information
    pointed by sm_specific_info

  DESCRIPTION
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsm_ppp_sm_init(dsumts_rmsmi_sm_info_type *sm_info_ptr);



/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to PDP-PPP type RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    sm_ptr: The pointer to the UMTS_RMSM instance control block
    event: The event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_ppp_sm_init() should have been called first.
    The top level state machine post function checks if we the state machine
    is initialized in the first place

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ppp_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  dsumts_rmsm_event_type    event
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_PPP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM (for pdp-ppp calls)
    in DS task and calls an appropriate function to handle it

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ppp_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type  cmd_id
);

#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)*/
#endif /* DSUMTS_RMSM_PPP_H */
