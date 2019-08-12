#ifndef TM_STANDALONE_H
#define TM_STANDALONE_H

#ifdef __cplusplus
extern "C" {
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Standalone Sub-module

General Description
  This file contains implementations for TM Diag Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/25/08    gk      On-Demand changes
10/16/06   cl      Initail Release
===========================================================================*/

typedef struct
{
  uint8                             lpm_mode_type;
  uint8                             num_no_fixes;
  boolean                           on_demand_lpm_on;
  boolean                           on_demand_ready_on;
  os_TimerMsgType                   *on_demand_lpm_mode_timer_ptr;
  os_TimerMsgType                   *on_demand_ready_mode_timer_ptr;
  boolean                           on_demand_tracking_lpm_timer_expired;
  boolean                           on_demand_tracking_ready_timer_expired;
  os_TimerMsgType                   *on_demand_poll_timer_ptr;
  os_TimerMsgType                   *on_demand_first_init_timer_ptr;
  os_TimerMsgType                   *ready_mode_extended_on_timer_ptr;
} tm_on_demand_sess_info_s_type;

typedef enum
{
  TM_STANALONE_TIMER_ID_SESSION = TM_STANDALONE_TIMER_ID_RANGE,
  TM_STANDALONE_TIMER_ID_ON_DEMAND_LPM,
  TM_STANDALONE_TIMER_ID_ON_DEMAND_READY, 
  TM_STANDALONE_TIMER_ID_ON_DEMAND_FIRST_INIT, /* started at init to ensure rest of the phone finishes initializing */
  TM_STANDALONE_TIMER_ID_ON_DEMAND_CHARGER_POLL,       /* polls battery charger if on-demand is enabled. Started by first-init timer */
  TM_STANDALONE_TIMER_ID_ON_DEMAND_READY_MODE_EXTENDED_ON, /* started if extended on is permitted when leaving ready mode */

  /* add other stand-alone timers before this line */
  TM_STANDALONE_TIMER_ID_LAST
} tm_standalone_timer_id_e_type; /*lint -e751 */


typedef enum
{
  TM_STANDALONE_SESS_STATE_INIT = 0,
  TM_STANDALONE_SESS_STATE_WAIT_WISHLIST_UPDATE,  
  TM_STANDALONE_SESS_STATE_WAIT_LR,    
  TM_STANDALONE_SESS_STATE_COMPLETED,
  TM_STANDALONE_SESS_STATE_ON_DEMAND  
} tm_standalone_sess_state_e_type;

typedef struct
{
  boolean                      eph_validity;
  boolean                      alm_validity;
  float                        pos_unc;
  float                        time_unc;   
} tm_standalone_aiding_data_info_s_type;

  
typedef struct
{
  /* Session Handle */
  tm_sess_handle_type               sess_handle;

  /* Standalone session state */
  tm_standalone_sess_state_e_type   session_state;  
  
  /* Current Operation Mode */
  tm_sess_operation_mode_e_type     op_mode;  

  pdsm_pd_fix_rate_s_type           fix_rate;      /* Fix rate for tracking sessions. */  
  
  pdsm_pd_qos_type                  qos;
    
  /* Client information */
  pdsm_client_type_e_type           client_type;
  pdsm_client_id_type               client_id;  

  /* Aiding Data Status */
  tm_standalone_aiding_data_info_s_type aiding_data_status;

  /* Phone Status */
  boolean                           oos;

  /* Timers */
  os_TimerMsgType                   *sess_timer_ptr;

  tm_on_demand_sess_info_s_type     on_demand_sess_info;
} tm_standalone_sess_info_s_type;


  
/*===========================================================================

FUNCTION tm_standalone_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_standalone_prtl_init(void);


/*===========================================================================

FUNCTION tm_standalone_sess_termination

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_sess_termination(tm_sess_stop_reason_e_type reason);

#ifdef __cplusplus
}
#endif

#endif /* TM_STANDALONE_H */

