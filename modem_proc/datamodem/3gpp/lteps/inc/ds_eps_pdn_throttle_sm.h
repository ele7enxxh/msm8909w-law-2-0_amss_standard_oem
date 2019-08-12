/*!
  @file
  ds_eps_pdn_throttle_sm.h

  
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_pdn_throttle_sm.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/14   vs      Initial version.


===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef DS_EPS_PDN_THROTTLE_SM_H
#define DS_EPS_PDN_THROTTLE_SM_H

#ifdef FEATURE_DATA_LTE
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_svc.h" 
#include "dstask_v.h"

/*===========================================================================

                           STRUCTURE DECLARATIONS

===========================================================================*/

typedef struct
{
  sys_plmn_id_s_type                plmn_id;
  uint8                             failure_count;
  boolean                           is_throttled;
  uint32                            throt_timer_val;  
  rex_timer_type                    throttle_timer;/* Timer to start when the SM is in */
  void 		                   *apn_idx_ptr; /* pointer to APN based table*/  
  sys_modem_as_id_e_type            subs_id;  
}ds_eps_pdn_throttle_sm_plmn_based_s;

typedef struct
{
  boolean  valid;
  byte                                     apn[DS_UMTS_MAX_APN_STRING_LEN+1];    
  ds_eps_pdn_throttle_sm_plmn_based_s     *apn_throt_sm_plmn_ptr[DS_3GPP_MAX_PLMN_SUPPORTED]; 
           
} ds_eps_pdn_throttle_sm_apn_based_dyn_s;

typedef struct
{
  ds_eps_pdn_throttle_sm_apn_based_dyn_s*   apn_based_dyn_tbl[DS_MAX_APN_NUM];
} ds_eps_pdn_throttle_sm_apn_based_s;

typedef struct
{
  boolean                   is_throttled;
  uint32                    throt_timer_val;  
  rex_timer_type            throttle_timer;/* Timer to start when the SM is in */
  sys_modem_as_id_e_type    subs_id;    
  void 	                    *user_data; 
}ds_eps_pdn_throttle_sm_timer_s;
typedef struct
{
  list_link_type              link;	 /* Pointer to the next item in list */
  sys_plmn_id_s_type          plmn_id;  /* PLMN Id */
  uint32                      throt_timer_val;	
  rex_timer_type              throttle_timer;/* Timer to start when the SM is in */  
  sys_modem_as_id_e_type      subs_id;    
}ds_eps_pdn_throt_blocked_plmn_type;
/*===========================================================================

                           MACRO DEFINITIONS

===========================================================================*/
#define DS_EPS_PDN_THROT_MAX_FAIL_COUNT 3
#define DS_EPS_PDN_THROT_2_HOURS_TIMER  7200*1000  /*in milliseconds*/  
#define DS_EPS_PDN_THROT_TIMER          10

/*===========================================================================

                         API DECLARATIONS

===========================================================================*/


/*===========================================================================
  FUNCTION DS_EPS_PDM_THROTTLE_SM_INIT
  
  DESCRIPTION
    This function initializes the ds_eps_throttle_sm module, creates apn table to store throttle info 
    for a given subs_id    
  PARAMETERS  
      subs_id  - subscription ID
  DEPENDENCIES 
    None.
  
  RETURN VALUE 
    None
   
  SIDE EFFECTS 
    None.
    
  ===========================================================================*/
void ds_eps_pdn_throttle_sm_init
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_HANDLE_PDN_CONN_REJ
 
DESCRIPTION
  This function gets called when attach is rejected and given cause code is present in EFS configuration file
  
PARAMETERS   : 
  1. rejected_pdn_cntx_p 
  2. Subs Id
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_handle_pdn_conn_rej
( 
  ds_pdn_context_s *rejected_pdn_cntx_p, 
  esm_cause_enum_T                 esm_cause,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CLEAR_THROTTLE_STATE_FOR_APN_ON_CURRENT_PLMN
 
DESCRIPTION
  This function is used to clear the throttling of given APN  
  
PARAMETERS   : 
  1. apn        - for which throttling needs to be cleared 
  2. Subs Id  
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_clear_throttle_state_for_apn_on_current_plmn
(
  byte *apn, 
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CLEAR_THROTTLE_STATE_FOR_APN_ON_ALL_PLMNS
 
DESCRIPTION
  This function is used to clear the throttling of given APN on all PLMNs
  
PARAMETERS   : 
  1.  apn_index_ptr - pointer to APN throttle table for given APN  
  
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_clear_throttle_state_for_apn_on_all_plmns
(
 ds_eps_pdn_throttle_sm_apn_based_dyn_s *apn_index_ptr
);

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_CLEAR_PLMN_BLOCKING

DESCRIPTION
  This function is used to clear LTE PLMN blocking on the given subscription
  
PARAMETERS  
  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
===========================================================================*/
void ds_eps_pdn_throt_sm_clear_plmn_blocking
(
  sys_modem_as_id_e_type	subs_id
);
/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RESET_THROTTLING

DESCRIPTION
  This function is used to clear throttling of all APN for given subscription
  
PARAMETERS  
  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_reset_throttling
(
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_SM_PLMN_UNBLOCK_TIMER_CB

DESCRIPTION
  This function is the timer call back for the unblock PLMN timer.
  This changes the state of the SM from throttled to unthrottled. Since this
  runs in the timer context
  
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_eps_pdn_throt_sm_plmn_unblock_timer_cb
(
  unsigned long callback_data
);
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_SM_THORT_TIMER_CB

DESCRIPTION
  This function is the timer call back for the LTE acqiosition timer.
  This is used to reset  UMTS throttling if running, this timer is to make sure UE tries to attach on LTE before UMTS on
  RAU complete IND
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  If anything more than such light operation (changing the state of the SM)
  were needed, post a command and process in DS context instead of working in
  the timer context.
  
===========================================================================*/
void ds_eps_pdn_throt_sm_throt_timer_cb
(
  unsigned long callback_data
);

/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_TIMER_EXP_HDLR

DESCRIPTION
  This function gets called on expiry of unblock_plmn timer, throttling will be cleared and PLMN will be unblocked in this 
  function
  
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
 void ds_eps_pdn_throt_sm_timer_exp_hdlr
(
  unsigned long callback_data
);
/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_TIMER_EXP_HDLR

DESCRIPTION
  This function gets called on expiry of unblock_plmn timer, throttling will be cleared and PLMN will be unblocked in this 
  function
  
PARAMETERS  
  callback_data - points to the  throtlling state machine.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
 void ds_eps_pdn_throt_plmn_unblock_timer_exp_hdlr
(
  unsigned long callback_data
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_IS_PLMN_UNBLOCK_TIMER_RUNNING

DESCRIPTION
  This function checks is PLMN unblock timer is running or not

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - if PLMN unblock timer is running
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_throt_is_plmn_unblock_timer_running
(
  sys_modem_as_id_e_type		subs_id  
);


/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_START_LTE_UNBLOCK_PLMN_TIMER

DESCRIPTION
  This function start a timer for given timer value for given subscription
  on expiry of this timer PLMN will be unblocked
  
PARAMETERS  
  1.  timer_val -timer value
  2.  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/
void ds_eps_pdn_throt_start_lte_unblock_plmn_timer
(
  uint32                        timer_val, 
  sys_modem_as_id_e_type	subs_id
);
/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_RAU_COMPLETE_IND

DESCRIPTION
  This function start a timer for given timer value for given subscription
  on expiry of this timer PLMN will be unblocked
  
PARAMETERS  
  1.  timer_val -timer value
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/

void ds_eps_pdn_throt_rau_complete_ind
(
  sys_modem_as_id_e_type	subs_id
);

/*==========================================================================
FUNCTION DS_EPS_PDN_THROT_IS_APN_THROTTLE_ON_CURR_PLMN

DESCRIPTION
  This function check if the given apn is throttled on current PLMN
  
PARAMETERS  
  1.  apn -apn name
  2.  subs_id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
 None
  
===========================================================================*/

boolean ds_eps_pdn_throt_is_apn_throttled_on_curr_plmn
(
  byte                          *apn, 
  sys_modem_as_id_e_type	subs_id
);
/*===========================================================================
FUNCTION DS_EPS_PDN_THROT_LTE_UNBLOCK_ALL_PLMNS

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_eps_pdn_throt_enable_lte_unblock_all_plmns
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_IS_LTE_PLMN_BLOCKED

DESCRIPTION
  This function is used to check if given PLMN ID is blocked on the current subscription
PARAMETERS  
  PLMN id
  Susbcription id
  PLMN ID
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE, if PLMN is blocked
  FALSE, otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_eps_pdn_throt_is_any_lte_plmn_blocked
(
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_START

DESCRIPTION
  This function starts the t3402 timer 

PARAMETERS
   subs_id            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_start
(
  ds_eps_pdn_throt_blocked_plmn_type* blocked_plmn_ptr,
  sys_modem_as_id_e_type	  subs_id  
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXP_CB

DESCRIPTION
  This function posts a cmd to take specific action based on tr3402 timer expiry

PARAMETERS
   call back data ptr - which points to t3402 timer table            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_exp_cb
(
     unsigned long callback_data 
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_T3402_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function resets the sm on timer expiry

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_throt_t3402_timer_expiry_cmd_hdlr
(
  unsigned long client_data_ptr
);
/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_CLEAR_T3402_TIMER

DESCRIPTION
  This function clears the t3402 timer if it is running

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_pdn_throt_clear_t3402_timer
(
  sys_modem_as_id_e_type	  subs_id  
);
/*===========================================================================
FUNCTION  DS_EPS_PDN_THROT_IS_T3402_TIMER_RUNNING

DESCRIPTION
  This function checks is  t3402 timer is running or not

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - if t3402 timer is running
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_throt_is_t3402_timer_running
(
  sys_modem_as_id_e_type         subs_id,  
  sys_plmn_id_s_type	   	  plmn_id
);

/*============================================================================
FUNCTION DS_EPS_PDN_THROT_SM_STOP_PLMN_UNBLOCK_TMR

DESCRIPTION
  This function stops plmn unblock timer if its running.
  
PARAMETERS  
  subs_id - subscription id.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None
  
===========================================================================*/
void ds_eps_pdn_throt_sm_stop_plmn_unblock_tmr
(
  sys_modem_as_id_e_type subs_id
);

#endif /* FEATURE_DATA_LTE */
#endif /* DS_EPS_PDN_THROTTLE_SM_H */
