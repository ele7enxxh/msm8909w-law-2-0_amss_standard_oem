#ifndef DSUMTS_RMSM_IP_H
#define DSUMTS_RMSM_IP_H
/*===========================================================================

                      D S U M T S _ R M S M _ I P  . H

DESCRIPTION
  The Data Services RM State Machine(PDP-IP) specific header file. 
  This contains the state machine specific function definitions and 
  data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsm_ip.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    vs      Added support for 3GPP IPv6 over PPP
03/04/09    sa      AU level CMI modifications.
04/22/03    vsk     code review cleanup. updated function headers
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
#include "dsumts_rmsmi.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
  
   

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_SM_INIT 
    This function initializes the PDP-IP specific state machine information
    pointed by sm_info

  DESCRIPTION
    
  PARAMETERS
   sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
   TRUE,  if success
   FALSE, if failure

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/  
boolean dsumts_rmsm_ip_init
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it.

  PARAMETERS
    inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    cmd id to be processed
    
  RETURN VALUE
    none

  DEPENDENCIES
    none  

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type cmd_id
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_DOWN_ARB_EV()

  DESCRIPTION
    This function processes UMTS_RMSM_UM_IFACE_DOWN_ARB_EV event. 

  PARAMETERS
   sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_down_arb_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_RM_PPP_UP_EV()

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_rm_ppp_up_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_RM_PPP_DOWN_EV()

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_rm_ppp_down_ev
( 
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_FAILURE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_FAILURE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_failure_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_CHANGED_EV()

  DESCRIPTION
    This function processes ADDR_CFG_CHANGED_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_changed_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_COMPLETE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_COMPLETE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_complete_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_END_EV()

  DESCRIPTION
    This function processes ADDR_CFG_END_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_end_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV()

  DESCRIPTION
    This function processes BRING_UP_UM_IFACE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_bring_up_um_iface_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_UP_EV()

  DESCRIPTION
    This function processes UM_IFACE_UP_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_up_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes UM_IFACE_DOWN_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

dsumts_rmsm_ipi_state_type dsumts_rmsm_ip_get_state
(
  dsumts_rmsmi_sm_info_type *sm_ptr
);

#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
         || defined (FEATURE_DATA_LTE) */
#endif /* DSUMTS_RMSM_IP_H */
