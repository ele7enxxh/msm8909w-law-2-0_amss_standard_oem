#ifndef DS_3GPP_RMSM_IP_COMMON_H
#define DS_3GPP_RMSM_IP_COMMON_H
/*===========================================================================

                      D S U M T S _ R M S M _ I P _ C O M M O N  . H

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/ds_3gpp_rmsm_ip_common.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/11/11    vs      Initial file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "dstask_v.h"

/*---------------------------------------------------------------------------
  Enumeration of states of the RmSm IP Common State Machine.
---------------------------------------------------------------------------*/
typedef enum
{
  /*Initial State*/
  DS_3GPP_RMSM_IP_COMMON_NULL_STATE,    

  /*MT PDP call*/
  DS_3GPP_RMSM_IP_COMMON_RINGING_TE_STATE,

  /*Waiting for initial IPCP or IPv6CP Creq packet*/
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE,

  /*Waiting for PPP to be configured with IP address / IID information */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE,

  /*PPP configured with IP address/IID */
  DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE,

  /* Waiting for Um to go down */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE,

  /* Waiting for PPP to go down */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE,

  /* Waiting for Entitlement check to complete */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE,

  /* Waiting for Redial check to complete */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE
}ds_3gpp_rmsm_ip_common_state_type;

#define V4_SM_INSTANCE(sm_inst) \
  ((sm_inst == DSUMTS_RMSM_V4_SM_INSTANCE)? TRUE:FALSE)

#define V6_SM_INSTANCE(sm_inst) \
  ((sm_inst == DSUMTS_RMSM_V6_SM_INSTANCE)? TRUE:FALSE)

#define ANY_SM_INSTANCE(sm_inst) \
  ((sm_inst == DSUMTS_RMSM_ANY_SM_INSTANCE)? TRUE:FALSE)

/*---------------------------------------------------------------------------
  Data Structure which stores IPCP related information.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32                     ipcp_snoop_tag;  
  dsm_item_type              *ipcp_cfg_pkt_ptr;
  boolean                    send_ipcp_naks_flg;    
  rex_timer_type             ipcp_res_del_timer;						   						
  uint8                      ipcp_creq_id; 
  boolean                    ipcp_creq_recd;
  rex_crit_sect_type         ipi_sm_info_crit_sec;  

}ds_3gpp_rmsm_ip_common_v4_info_type;

/*---------------------------------------------------------------------------
  Data Structure which stores IPV6CP related information.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32                     ipv6cp_snoop_tag;  
  dsm_item_type              *ipv6cp_cfg_pkt_ptr;
  boolean                    send_ipv6cp_naks_flg;    
  rex_timer_type             ipv6cp_res_del_timer;						   						
  uint8                      ipv6cp_creq_id;
  boolean                    ipv6cp_creq_recd;
  rex_crit_sect_type         ipi_sm_info_crit_sec;  

}ds_3gpp_rmsm_ip_common_v6_info_type;

/*---------------------------------------------------------------------------
  Data Structure which stores RmSm IP Common Information.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_rmsm_ip_common_state_type     state;
 
  network_params_info_type              ppp_info;       
 
  ds_3gpp_rmsm_ip_common_v4_info_type   v4_info;
  
  ds_3gpp_rmsm_ip_common_v6_info_type   v6_info;

  boolean                              (*configure_ppp_f_ptr) 
                                         (void*, ppp_protocol_e_type );


} ds_3gpp_rmsm_ip_common_info_type;

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_INIT

  DESCRIPTION
    This function initializes the RmSm IP Common Data Structure
    
  PARAMETERS
    Rm Device Instance
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_init
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to PDP-IP type RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
 	inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    event: The event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_ip_sm_init() should have been called first.
    The top level state machine post function checks if we the state machine
    is initialized in the first place

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_post_event
(
  dsumts_rmsm_instance_type* inst_ptr,
  dsumts_rmsm_event_type     event
);



/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_3GPP_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it

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
void ds_3gpp_rmsm_ip_common_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type  cmd_id
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_GET_STATE()

  DESCRIPTION
    This function retrieves the RMSM IP common state.

  PARAMETERS
    inst: Rm Dev Instance

  RETURN VALUE
    ds_3gpp_rmsm_ip_common_state_type

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
ds_3gpp_rmsm_ip_common_state_type ds_3gpp_rmsm_ip_common_get_state
(
  dsumts_rmsm_rm_dev_instance_type  inst
);

#endif /*DS_3GPP_RMSM_IP_COMMON_H*/
