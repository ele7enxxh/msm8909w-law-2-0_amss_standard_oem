#ifndef SUBS_PRIO_H
#define SUBS_PRIO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            S U B S C R I P T I O N     P R I O R I T Y     M O D U L E

                      Subscription Priority Module Header

GENERAL DESCRIPTION

  This file provides Subscription Priority interface information


EXTERNALIZED FUNCTIONS

  None
  

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/subs_prio.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   when          who     what, where, why
----------      ---     ---------------------------------------------------------- 
10/22/2015       jm     Add query API support (CR 925084)
01/13/2015       ag     Added new interface for GSM to update multimode ASID to MCS
10/08/2014       ag     Added tech state to the subs prio callback
09/17/2014       sk     Changes to update active sub mask from MMOC(713328) 
08/11/2014       sg     Added new interface to data services for MMS
08/18/2014       sk     support for fallback full RAT DSDA mode in SV+G DSDA(702293)
07/22/2014       mn     Expose sp_trm_mode_override() API for RF usage in 
                         FTM mode (CR: 689879).
07/04/2014       ag     data+mms support
07/07/2014       mn     Changes to support single SIM scenario for TRM
                         mode changes(CR: 679494)
04/14/2014       mn     TRM should look at feature_mode provided by CM for single SIM
                         cases. (CR: 646746).
04/07/2014       ag     Simultaneous capacity in WWAN coexistence is set to FALSE when 
                        not in SGLTE mode or when roaming mode is not set
03/06/2014       rj     Adding support for SGLTE+G DSDA
02/13/2014       ag     API for TRM Client to ASID mapping
02/12/2014       sr     Changes to differentiate 1stack vs 2stack SxLTE(CR:613319)
01/15/2013       sk     Added TRM client for subs priority info.
12/27/2013       rj     Adding support tech ID in NAS interface.
11/06/2013       mn     Adding support for Opp SGLTE/SGTDS/SVLTE + G.
09/19/2013       rj     Adding support for GSM to ASID mapping
07/18/2013       mn     Updating subscription priority for TSTS.
01/03/2013       rj     Adding complete functionality of subs_prio
11/01/2012       rj     Review Comments incorporated
10/25/2012       rj     Initial draft

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/
#include <comdef.h>
#include "sys.h"
#include "trm.h"


/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/* Convert Enum to a Bit mask */
#define SP_CONV_ENUM_TO_BIT_MASK(enum_t) (1 << enum_t)

/* Check whether subscription is GSM only subscription */
#define SP_IS_GSM_ONLY_SUB(sub_techs) ((sub_techs & (~(1 << SYS_SYS_MODE_GSM))) == 0)

#define IS_SUB_IN_CALL(state) (((state)==SP_VOICE_LOW_PRIORITY_STATE) || ((state)==SP_VOICE_HIGH_PRIORITY_STATE) || ((state)==SP_DATA_ONLY_STATE))

#define SP_MAX_STACK_PER_SUB 3

/*----------------------------------------------------------------------------
  States of Subs
    Subs states in increasing order of their priority

----------------------------------------------------------------------------*/

typedef enum
{
  /* Default state */
  SP_NO_SUBS_STATE,

  /* Only data call */
  SP_DATA_ONLY_STATE,

  /* Only data high call */
  SP_DATA_HIGH_ONLY_STATE,

  /* Tx Off, Idle/Acq, Other Signaling activities. Default state */
  SP_BASELINE_STATE,

  /* Page Response for incoming MT call not yet answered */
  SP_MT_CALL_SIGNALING_STATE,

  /* Voice Call in Local Hold or Network Hold */
  SP_VOICE_LOW_PRIORITY_STATE,

  /* Active Voice Call in conversation, Call Originated, Call Answered */
  SP_VOICE_HIGH_PRIORITY_STATE,

  /* For internal bound-checking and array-sizing */
  SP_SUBS_LAST_STATE  = SP_VOICE_HIGH_PRIORITY_STATE
}
sp_subs_states_enum_t;

/*----------------------------------------------------------------------------
  States of Call for each subscription

----------------------------------------------------------------------------*/

typedef enum
{
  /* Subscription is in idle i.e no voice or data calls are active or hold. */
  SP_NO_CALL_STATE,
    
  /* No voice calls are active nor on hold. There is an ongoing data call */
  SP_DATA_CALL_STATE,

  /* No voice calls are active and atleast one call is on hold (LCH (Local Call Hold) or NW (Network Hold)) */
  SP_VOICE_CALL_HOLD_STATE,

  /* Atleast one voice call is active in conversation */
  SP_VOICE_CALL_ACTIVE_STATE,

  /* For internal bound-checking and array-sizing */
  SP_CALL_MAX_STATE ,

  SP_CALL_STATE_NONE = 0xFF
}
sp_call_states_enum_t;


/*----------------------------------------------------------------------------
  States of 1xL1 for each subscription
----------------------------------------------------------------------------*/

typedef enum
{
  /* Subscription not active */
  SP_TECH_INACTIVE_STATE,
    
  /* IDLE state */
  SP_TECH_IDLE_STATE,

  /* stack has received page and in process of processing MT Call  */
  SP_TECH_ACCESS_STATE,

  /* stack is in voice call (active in conversation) */
  SP_TECH_TRAFFIC_STATE,

  /* For internal bound-checking and array-sizing */
  SP_TECH_MAX_STATE,

  SP_TECH_NO_STATE = 0xFF
}
sp_tech_states_enum_t;


/*----------------------------------------------------------------------------
  States of NAS for its subscription
  This will be used by Subs-prio module and 1xL1 state will be mapped to this enum
  
                           MT Call States Mapping
   1xL1 States                                     NAS States
   SP_TECH_INACTIVE_STATE              SP_TECH_MT_CALL_STATE_NONE
   SP_TECH_IDLE_STATE                     SP_TECH_MT_CALL_RELEASE
   SP_TECH_ACCESS_STATE                SP_TECH_MT_CALL_START
   SP_TECH_TRAFFIC_STATE               SP_TECH_MT_CALL_START

----------------------------------------------------------------------------*/

typedef enum
{
  /* Default call State */
  SP_TECH_MT_CALL_STATE_NONE,

  /* Subscription not active */
  SP_TECH_MT_CALL_RELEASE,
    
  /* MT Call started */
  SP_TECH_MT_CALL_START,

  /* For internal bound-checking and array-sizing */
  SP_TECH_MAX_MT_CALL_STATE
}
sp_mt_call_states_enum_t;


/*----------------------------------------------------------------------------
  Subs-Prio Client IDs
----------------------------------------------------------------------------*/

typedef enum 
{
  /* SAR Client */
  SP_SAR,

  /* Coex Client */
  SP_COEX,

  SP_TDSCDMA,

  /* GSM client mapped to multi-mode subscription */
  SP_GSM1,

  /* GSM client mapped to non multi-mode subscription */
  SP_GSM2,

  /* GSM client mapped to non multi-mode subscription */
  SP_GSM3,

  SP_TRM,

  /* For internal bound-checking */
  SP_LAST_CLIENT  = SP_TRM,
  
  SP_MAX_CLIENT,

  SP_NO_CLIENT    = 0xFF
}sp_client_enum_t;


/*-----------------------------------------------------------------------------
  Subscription Prioritiy Default Tie-Breakers

    CM will provide this info to Subs-Prio module during bootup and
    whenever User changes preference
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* Tie-breaker for the case when both subscriptions are in Voice 
       – High Priority or Voice – Low Priority */
  sys_modem_as_id_e_type      default_voice_subs;

  /* Tie-breaker for the case when both subscriptions are in Data Only */
  sys_modem_as_id_e_type      default_data_subs;

  /* Tie-breaker for the case when both subscriptions are 
        in Baseline or MT Voice Call Signaling state */
  sys_modem_as_id_e_type      priority_subs;
  
  /* Indicates the number of simul mode stacks active. */
  uint8                      num_active_simul_mode_stacks;

}sp_default_tie_breakers;

typedef struct 
{
  sys_sys_mode_e_type   mode;   /* Tech Id */
  sp_call_states_enum_t  state;   /* State of the tech */
}sp_stack_info_type;


/*-----------------------------------------------------------------------------
  Subscription Prioritiy subscription info

    Data services will provide this info to Subs-Prio module 
    whenever there is a change in the subscription state
-----------------------------------------------------------------------------*/

typedef struct 
{
  sys_sys_mode_e_type       mode;     /* Tech Id */
  sp_call_states_enum_t     state;    /* State of the tech */
  sys_data_priority_e_type  priority; /* Priority */
}sp_data_stack_info_type;

/*-----------------------------------------------------------------------------
  Subscription Prioritiy subscription info

    CM will provide this info to Subs-Prio module 
    whenever there is a change in the subscription state
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* States of different techs in each subscription… 2nd dimension is stack identifier only for non-TR PLs */
  sp_stack_info_type subs_st[MAX_NUM_OF_SUBSCRIPTIONS][SP_MAX_STACK_PER_SUB];

}sp_subscription_call_state_info;


typedef struct 
{
  /* Data states of different techs in each subscription and their priority */
  sp_data_stack_info_type subs_st[MAX_NUM_OF_SUBSCRIPTIONS];
}sp_data_call_state_info;

/*-----------------------------------------------------------------------------
  Subscription Prioritiy subscription info

    1xL1 will provide this info to Subs-Prio module
    whenever there is a change in the subscription state
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* States of different subscriptions */
  sp_tech_states_enum_t      state;

  /* Tech id of current technology */
  sys_sys_mode_e_type        tech_id;
}sp_tech_subscription_info;

/*-----------------------------------------------------------------------------
  Subscription Prioritiy subscription info

    NAS will provide this info to Subs-Prio module
    whenever there is a change in the subscription state
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* States of different subscriptions */
  sp_mt_call_states_enum_t   state;

  /* Update subscription ID in which tech is active */
  sys_modem_as_id_e_type     subscription_id;

  /* Tech id of stack entering/exiting a call */ 
  sys_sys_mode_e_type tech_id;

}sp_mt_call_info;

/*-----------------------------------------------------------------------------
  Subscription Priority Mode Change Result

    This enum is used by Subscription Priority to notify the result of 
    Mode change triggered by calling sp_subs_info_update.
-----------------------------------------------------------------------------*/
typedef enum 
{
  /* Signifies a generic failure during mode change */
  SP_MODE_CHANGE_RESULT_MODE_CHANGE_FAILED,

  /* Signifies that the harware does not support the configuration */
  SP_MODE_CHANGE_RESULT_MODE_UNSUPPORTED,

  /* Signifies that the mode change was successful. */
  SP_MODE_CHANGE_RESULT_MODE_CHANGE_SUCCESSFUL,

   /* Signifies that mode change initiation was not required */
  SP_MODE_CHANGE_RESULT_MODE_UNCHANGED

} sp_mode_change_result_enum_t;

/*-----------------------------------------------------------------------------
  Subscription Prioritiy Mode Change Result Data
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* This enum is used by Subscription Priority to notify the result of 
     Mode change triggered by calling sp_subs_info_update. */
  sp_mode_change_result_enum_t result;
}sp_mode_change_result_data_t;

/*-----------------------------------------------------------------------------
  Subscription Priority Mode Change Result Callback

    Subscription Priority will trigger this callback when mode change is triggered
    by invoking sp_subs_info_update
-----------------------------------------------------------------------------*/

typedef void (*sp_mode_change_result_cb_type)
(
  sp_mode_change_result_data_t result_data
);

/*-----------------------------------------------------------------------------
  Subscription Prioritiy Multi SIM Info
-----------------------------------------------------------------------------*/
typedef struct 
{
  /* Denotes the maximum number of standby subscriptions supported. 
     It is the number of subscriptions that can be in idle mode at the 
     same time
     For example: For SSSS, max_standby_subscriptions = 1,
                  For DSDS, max_standby_subscriptions = 2,
                  For DSDA, max_standby_subscriptions = 2,
                  For TSTS, max_standby_subscriptions = 3 and so on. */
  uint32 max_standby_subscriptions;

  /* Denotes the maximum number of concurrently active subscriptions supported. 
     It is also the number of subscriptions that can transmit at the same time
     For example: For SSSS, max_concurrent_active_subscriptions = 1,
                  For DSDS, max_concurrent_active_subscriptions = 1,
                  For DSDA, max_concurrent_active_subscriptions = 2,
                  For TSTS, max_concurrent_active_subscriptions = 1 and so on. */
  uint32 max_concurrent_active_subscriptions;
} sp_multi_sim_info;

/*-----------------------------------------------------------------------------
  Subscription Prioritiy active subscription info

    CM will provide info to Subs-Prio module, that which tech is active on available subscription 
    Eg. If SUBSCRIPTION_1 and SUBSCRIPTION_2 are active (mean active_subscription_mask = 0x06)
         active_subscription_mask = SP_CONV_ENUM_TO_BIT_MASK(SUBSCRIPTION_1) |
                                                         SP_CONV_ENUM_TO_BIT_MASK(SUBSCRIPTION_2)

         If 1x and HDR are supported in SUBSCRIPTION_1 tech_supported_bitmask value will be
         tech_supported_bitmask[SUBSCRIPTION_1] = SP_CONV_ENUM_TO_BIT_MASK(SP_1X_TECH_ID) |
                                                         SP_CONV_ENUM_TO_BIT_MASK(SP_HDR_TECH_ID)
-----------------------------------------------------------------------------*/

typedef struct 
{
  /* Bitmask indicating which subscriptions are active. For example 
      in TSDA or TSDS, only two corresponding bits will be set  */ 
  uint8  active_subscription_mask;
 
  /* This is a tech bitmask per subscription indicating the RATs 
      supported by each subscription. */
  uint32  tech_supported_bitmask[MAX_NUM_OF_SUBSCRIPTIONS];

  /*  This is an array of bitmasks to be indexed using sys_sys_mode_e_type. It denotes the 
      technologies on a multimode subscription that are compatible with each other. That is,
      they can hold different chains simultaneously.

      Each bit in the bitmask corresponds to a tech the index tech is simultaneously
      compatible with. 

      Also, the values of the bitmasks should show symmetry. That is, if 1x is compatible
      with LTE, LTE should be made compatible with 1x.
      
      For Example, to enable SVLTE and SGLTE: 
      simult_cap[SYS_SYS_MODE_LTE] =  ( 1 << SYS_SYS_MODE_HDR  )  | 
                                      ( 1 << SYS_SYS_MODE_CDMA )  | 
                                      ( 1 << SYS_SYS_MODE_GSM  ); 
      simult_cap[SYS_SYS_MODE_CDMA] = ( 1 << SYS_SYS_MODE_LTE  );
      simult_cap[SYS_SYS_MODE_HDR]  = ( 1 << SYS_SYS_MODE_LTE  );
      simult_cap[SYS_SYS_MODE_GSM]  = ( 1 << SYS_SYS_MODE_LTE  ); */
  uint32                         simult_cap[SYS_SYS_MODE_MAX];

  /* Denotes the multi-SIM configurations. For example: SSSS vs DSDS vs TSTS etc. */
  sp_multi_sim_info              multi_sim_config;
  
  /* Indicates the feature mode set in policy manager... should not change with
     roaming mode changes */
  sys_subs_feature_t             subs_feature_mode[MAX_NUM_OF_SUBSCRIPTIONS];

  /* Indicate the UE mode set in CM. This can change with roaming modes */
  sys_ue_mode_e_type             ue_mode;

  /* Indication of full rat service scan mode */
  uint8                          svc_scan_mode; 
  
}sp_active_subscription_info;

/*----------------------------------------------------------------------------
  TRM Mode Override API
----------------------------------------------------------------------------*/

typedef struct
{
  /* Modes to be enabled */
  uint8 chain_mapping_mode;
  
  /* Compatibility modes */
  uint8 compatibility_mode;
  
  /* Flag to notify whether to enable or disable the override */
  boolean override;
  
  /* Tie-breaker for the case when both subscriptions are in Data Only */
  sys_modem_as_id_e_type      default_data_subs;
  
  /* Denotes the number of active stacks */
  uint8                       num_active_simul_mode_stacks;

  uint8                       num_concurr_active_subs;

  uint8                       num_standby_subs;

  /* feature mode */
  sys_subs_feature_t          feature_mode;

  /* tech supported bitmask */
  uint32                      tech_supported_bitmask;

}sp_trm_mode_override_input_type;



/*-----------------------------------------------------------------------------
  Subscription Prioritiy info 

    CM will provide info to Subs-Prio module, which tech is active on available 
    subscription and default priority of subscriptions in tie-breaker scenarios.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_DUAL_DATA

typedef struct 
{
  /* Default Tie-breaker info from CM */
  sp_default_tie_breakers        tie_breakers;
 
  /* Subscription update info from CM */
  sp_active_subscription_info    active_tech_info;

  /* Callback function that will be invoked to notify the result of the
     mode change.

     Note: This can be set to NULL if the caller to sp_subs_info_update() API
     does not expect any result callback */
  sp_mode_change_result_cb_type  mode_change_result_cb;

  /* Data priority for a subscription */
  sys_data_priority_e_type        sub_data_prio[MAX_NUM_OF_SUBSCRIPTIONS];

}sp_subscription_info;

#else /* FEATURE_DUAL_DATA */

typedef struct 
{
  /* Default Tie-breaker info from CM */
  sp_default_tie_breakers        tie_breakers;
 
  /* Subscription update info from CM */
  sp_active_subscription_info    active_tech_info;

  /* Callback function that will be invoked to notify the result of the
     mode change.

     Note: This can be set to NULL if the caller to sp_subs_info_update() API
     does not expect any result callback */
  sp_mode_change_result_cb_type  mode_change_result_cb;

}sp_subscription_info;

#endif /* FEATURE_DUAL_DATA */

typedef struct
{
  /* States of different subscriptions */
  sp_subs_states_enum_t   state;

  /* Tech id of stack entering/exiting a call */ 
  sys_sys_mode_e_type     mode;

}sp_mode_final_state_type;

typedef struct
{
  /* AS ID of the subscription */
  sys_modem_as_id_e_type  sub;

  /* Final State of the subscription or macro priority */
  sp_subs_states_enum_t   state;

  /* State of active Tx techs in this subscription */
  sp_mode_final_state_type  tech_state[SP_MAX_STACK_PER_SUB];

  /* Number of techs with active Tx */
  uint8                   num_active_techs;
}sp_subs_state_type;


/*----------------------------------------------------------------------------
  Structure for Multimode AS-ID mapping with GSM tech clients 
----------------------------------------------------------------------------*/

typedef struct
{
  sys_modem_as_id_e_type      multimode_as_id;
  sp_client_enum_t            gsm_id_list[MAX_NUM_OF_SUBSCRIPTIONS];
} sp_tech_map_table_t;


/*----------------------------------------------------------------------------
  Subscriptions Array
----------------------------------------------------------------------------*/

typedef struct 
{
  /* Array of subscriptions in descending priority order */
  sp_subs_state_type        order[ MAX_NUM_OF_SUBSCRIPTIONS ];
  
  /* Number of subscriptions present in the array */
  uint32                    num;

  /* Subscription capability information from CM */
  sp_active_subscription_info active_tech_info;

  /* Subscription capability tech mapping as per multimode subs */
  sp_tech_map_table_t         tech_asid_map;

}sp_subscription_priority_list;

/*----------------------------------------------------------------------------
  Enum for query information
----------------------------------------------------------------------------*/
typedef enum
{
  /* Tiebreaker Info */
  SP_TIEBREAKER_INFO,

  /* Multisim Configuration */
  SP_MULTISIM_INFO,

  /* Enum Max */
  SP_QUERY_INFO_MAX,

} sp_query_info_type;

/*----------------------------------------------------------------------------
  Query API input structure
----------------------------------------------------------------------------*/
typedef struct
{
  sp_query_info_type  info_type;

} sp_query_info_input_type;

/*----------------------------------------------------------------------------
  Union for all get request output information
----------------------------------------------------------------------------*/
typedef union
{
  /* Default Tie-breaker info from CM */
  sp_default_tie_breakers    tie_breakers;

  /* Denotes the multi-SIM configurations. For example: SSSS vs DSDS vs TSTS etc. */
  sp_multi_sim_info           multi_sim_config;

}sp_query_info_return_data;

/*----------------------------------------------------------------------------
  Query Information Output Type
----------------------------------------------------------------------------*/
typedef struct
{
  /* Type of information received by sp */
  sp_query_info_type         info_type;

  /* TRUE if data below can be used; FALSE otherwise */
  boolean                    is_valid_info;

  /* Data based on info type */
  sp_query_info_return_data  info;

}sp_query_info_return_type;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*=======================================================================
FUNCTION SP_SUBSCRIPTION_CALL_STATE_UPDATE
    
DESCRIPTION
  This function will be used by CM to provide call states for each subscription. 
      
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE - If internal data base is updated successfully
  FALSE - If updating fails 
    
SIDE EFFECTS
  None
========================================================================*/
boolean sp_subscription_call_state_update
(
  /* Subscription update info from CM */
  sp_subscription_call_state_info subs_call_state_update
);

/*=======================================================================
FUNCTION SP_DATA_CALL_STATE_UPDATE

DESCRIPTION
  This function will be used by data services to update data call state
  and to indicate if any of the data stacks are doing MMS

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If internal data base is updated successfully
  FALSE - If updating fails

SIDE EFFECTS
  None
========================================================================*/
boolean sp_data_call_state_update
(
  /* Subscription update info from CM */
  sp_data_call_state_info subs_call_state_update
);

/*=======================================================================
FUNCTION SP_TECH_SUBSCRIPTION_STATE_INFO_UPDATE
    
DESCRIPTION
  This function will be used by 1xL1 to provide subscription states in its current active subs. 
      
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE - If internal data base is updated successfully
  FALSE - If updating fails 
    
SIDE EFFECTS
  None
========================================================================*/
boolean sp_tech_subscription_state_info_update
(
  /* Subscription update info from 1xL1 */
  sp_tech_subscription_info tech_subs_update
);

/*=======================================================================
FUNCTION SP_SUBSCRIPTION_MT_CALL_INFO_UPDATE
    
DESCRIPTION
  This function will be used by NAS to provide subscription states in its current active subs. 
      
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE - If internal data base is updated successfully
  FALSE - If updating fails 
    
SIDE EFFECTS
  None
========================================================================*/
boolean sp_subscription_mt_call_info_update
(
  /* Subscription update info from NAS */
  sp_mt_call_info mt_call_subs_update
);


/*=======================================================================
FUNCTION SP_SUBS_INFO_UPDATE
    
DESCRIPTION
  This function will be used by CM to provide default subscription priorities as set in NV/User &
  info regarding active technology in particular subscription. This information will be used 
  in tie-breaker situations.
      
DEPENDENCIES
  Should be called in CM intialization
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void sp_subs_info_update
(
  /* Subscription update info from CM */
  sp_subscription_info subs_info
);


/*=======================================================================
CALLBACK SubsPrioListCallback

DESCRIPTION
  Callback function type that Subscription-Priority module's clients will 
   send so as to return them 'subscription priority list'
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
typedef void (*subsPrioListCallback)(sp_subscription_priority_list  subs_prio_list, sp_client_enum_t client_id);


/*=======================================================================
FUNCTION SP_REGISTER_CLIENT
    
DESCRIPTION
  This function will be used by clients to register there callBack functions with Subs-Prio
  and later Subs-prio module will inform clients using callback functions
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  TRUE - If registration is successful
  FALSE - If registration is unsuccessful

SIDE EFFECTS
  None
========================================================================*/
boolean sp_register_client
(
  /* Client CallBack function */
  subsPrioListCallback call_back_fn, 
  
  /* Client_id of requested client */
  sp_client_enum_t client_id
);

/*=======================================================================
FUNCTION SP_DEREGISTER_CLIENT
    
DESCRIPTION
  This function will be used by clients to de-register there callBack functions with Subs-Prio
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  TRUE - If de-registration is successful
  FALSE - If de-registration is unsuccessful

SIDE EFFECTS
  None
========================================================================*/
boolean sp_deregister_client
(
  /* client_id to verify which client callback data block to de-register */
  sp_client_enum_t client_id
);

/*=======================================================================
FUNCTION SP_GET_ASID_FOR_TRM_CLIENT
    
DESCRIPTION
  This function will be used by clients to get ASID mapping for the TRM
  client
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  FALSE if incorrect client like TRM_CM or not a WWAN Tech.
  TRUE otherwise
 
SIDE EFFECTS
  None
========================================================================*/
 boolean sp_get_asid_for_trm_client
(
  /* TRM Client Id */
  uint32 client_id,
  sys_modem_as_id_e_type* asid_ptr
);

/*=======================================================================
FUNCTION SP_TRM_MODE_OVERRIDE

DESCRIPTION
  This function:
  Changes the mode at TRM.

DEPENDENCIES
  Subs-prio module should be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
sp_mode_change_result_enum_t sp_trm_mode_override(sp_trm_mode_override_input_type* input);

/*=======================================================================
FUNCTION SP_UPDATE_ACTIVE_SUB_INFO
    
DESCRIPTION
  This function will be used by mmoc to update the active subscription
  mask indicating active/inactive subs
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  TRUE if update is success
  FALSE if unsuccessful
 
SIDE EFFECTS
  None
========================================================================*/
boolean sp_update_active_sub_info
(
  /* Sub mask info */
  uint8 active_sub_mask
);

/*=======================================================================
FUNCTION SP_UPDATE_MULTIMODE_SUB
    
DESCRIPTION
  This function will be used by GSM to update which SUB is multimode
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  None
 
SIDE EFFECTS
  None
========================================================================*/
void sp_update_multimode_sub
(
  sys_modem_as_id_e_type multimode_asid
);

/*=======================================================================
FUNCTION SP_QUERY_INFO
    
DESCRIPTION
  This function will be used by clients to retrieve any
  information contained within subs-prio
      
DEPENDENCIES
  Subs-prio module should be initialized
    
RETURN VALUE
  Ouptut data shall be filled
 
SIDE EFFECTS
  None
========================================================================*/
void sp_query_info
(
  sp_query_info_input_type* input,
  sp_query_info_return_type* output
);

#endif /* !SUBS_PRIO_H */

