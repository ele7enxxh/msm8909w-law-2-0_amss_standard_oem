/*!
  @file
  ds_3gpp_throttle_sm.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_throttle_sm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/11   ss      Changes to support RPM.
08/30/10   hs      Initial version

===========================================================================*/

#ifndef DS_3GPP_THROTTLE_SM_H
#define DS_3GPP_THROTTLE_SM_H

#define DS_THROT_LOG_PARAM_NA          0x0F

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "rex.h"
#include "sys.h"
#include "ds_3gppi_utils.h"
#include "dsumtspdpreg.h"
#include "ds_3gpp_hdlr.h"
#include "ps_sys_event.h"
#include "ds_3gpp_pdn_context.h"
#include "dsumtspdpreg.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_3GPP_PDN_REJECT_TIMER_MIN_SECOND 1
#define DS_3GPP_PDN_REJECT_TIMER_MAX_SECOND 15
#define DS_3GPP_SECOND_TO_MILISECOND_MULTIPLIER 1000
#define DS_3GPP_SYS_SEL_RETRY_MAX_VAL 16


/*-------------------------------------------------------------------------- 
  Enum to represent various throttling actions
  --------------------------------------------------------------------------*/
typedef enum
{
  NO_ACTION = 0,
  DISABLE_LTE = 1,
  DISABLE_LTE_PER_PLMN = 2,
  ENABLE_LTE = 3,
  ENABLE_LTE_PER_PLMN = 4,
}ds_3gpp_throt_svc_action_e_type;

/*-------------------------------------------------------------------------- 
  Structure to hold information about service request throttling
  for logging
  -------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8     subs_id;
  boolean   is_throttled;
  uint8     throttle_action;
  uint8     counter;
  uint16    duration;
  uint16    mcc;
  uint16    mnc;
  char      reserved[4];
}ds_3gpp_throt_svc_event_type;

typedef enum
{
  DS_3GPP_NO_SYS_SEL_RETRY,
  DS_3GPP_LTE_ENABLE_RETRY,
  DS_3GPP_LTE_DISABLE_RETRY,
  DS_3GPP_LTE_PLMN_BLOCK_RETRY,
  DS_3GPP_LTE_PLMN_UNBLOCK_RETRY
}ds_3gpp_throt_sys_sel_retry_e_type;

/*-------------------------------------------------------------------------- 
  These RAT Masks will be used to indicate the RATs on which Throttling
  operations need to be performed. 
  Examples: 
  1) When SIM refresh happens we are supposed to clear  
  throttling information for all RATs, 
  so DS_3GPP_THROT_RAT_MASK_ALL should be used. 
  2) If throttling applies on LTE and Global RATs,
  DS_3GPP_THROT_RAT_MASK_GLOBAL|DS_3GPP_THROT_RAT_MASK_LTE
  should be used.
  2) If throttling needs applies to all UMTS RATs, then 
  DS_3GPP_THROT_RAT_MASK_UMTS should be used.
  3) If throttling needs to be performed across all RATs, 
  DS_3GPP_THROT_RAT_MASK_GLOBAL should be used
  -------------------------------------------------------------------------*/

#define DS_3GPP_THROT_RAT_MASK_ALL  DS_3GPP_THROT_RAT_MASK_GLOBAL| \
                                    DS_3GPP_THROT_RAT_MASK_UMTS|   \
                                    DS_3GPP_THROT_RAT_MASK_LTE     \


#define DS_3GPP_THROT_RAT_MASK_NONE   0x00000000 /* Not Set */
#define DS_3GPP_THROT_RAT_MASK_GLOBAL 0x00000001 /* Global Rat Mask. Indicates 
                                                    throttling which 
                                                    applies across all RATs */
#define DS_3GPP_THROT_RAT_MASK_UMTS   0x00000002 /* UMTS Rat Mask */
#define DS_3GPP_THROT_RAT_MASK_LTE    0x00000004 /* LTE Rat Mask */


/*-------------------------------------------------------------------------- 
  This enumeration is used to indicate the RAT Type on which Throttling
  will be performed.
  -------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_THROT_RAT_MIN = -1,
  DS_3GPP_THROT_RAT_GLOBAL = 0, /* Across all RATs */
  DS_3GPP_THROT_RAT_LTE,
  DS_3GPP_THROT_RAT_UMTS,
  DS_3GPP_THROT_RAT_ILLEGAL
} ds_3gpp_throt_rat_e_type;

#define DS_3GPP_THROT_RAT_MAX DS_3GPP_THROT_RAT_ILLEGAL
/*-------------------------------------------------------------------------- 
  This structure is used to determine the retry timer and at what intervals
  should the retry occur
  --------------------------------------------------------------------------*/
typedef struct
{
  uint16                                 retry_timer_val;
  rex_timer_type                         sys_sel_retry_timer; 
  cm_plmn_blocking_info_s_type           plmn_blocking_info;
  cm_sys_sel_pref_params_s_type          sys_sel_pref_params;
  ds_3gpp_throt_sys_sel_retry_e_type     retry_type;
  sys_modem_as_id_e_type                 subs_id;
  ds_3gpp_throt_svc_event_type           svc_event_type;
}ds_3gpp_throt_sys_sel_retry_sm_s;


/*------------------------------------------------------------------------------
  Function pointer type for core SM timer start/stop callbacks 
------------------------------------------------------------------------------*/
typedef void (*ds_3gppi_throttle_sm_timer_ev_cb_f) (void * throt_sm_ptr, 
                                                    void* cb_data);

/*------------------------------------------------------------------------------
  Function pointer type for fetching the correct Throttling Timer Value
------------------------------------------------------------------------------*/
typedef uint32 (*ds_3gppi_throttle_sm_get_timer_cb_f) (void * throt_sm_ptr, 
                                                       void* cb_data);

/*------------------------------------------------------------------------------
  Struct to contain information about user_data
  ------------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_net_down_reason_type                  cause_code;
  ds_umts_pdp_auth_type                         *auth_info_p;
}ds_3gpp_throt_user_data;
/*------------------------------------------------------------------------------
  This structure is used to implement the throttling state machine
  This is a generic state machine and anyone who is interested in using the
  throttling algorithm can make use of this module.
------------------------------------------------------------------------------*/
typedef struct
{
  boolean            is_throttled;  /* boolean indicating if throttling is ON */
  uint8              failure_count; /* Numer of attempts rejected by the 
                                                                      network */
  rex_timer_type     throttle_timer;/* Timer to start when the SM is in 
                                                                     throttle */
  uint32             throt_tmr_val; /* The value that the throt timer has to be
                                                                        set to*/
  sys_plmn_id_s_type plmn_id;       /* PLMN that this SM is applicable to     */

  byte               apn[DS_UMTS_MAX_APN_STRING_LEN + 1]; /* APN for this SM */


  ds_3gpp_throt_user_data  user_data; /*To contain any particular user_data*/

  ds_3gppi_throttle_sm_timer_ev_cb_f throttle_sm_timer_start_cb;
                                    /* This function cb is invoked when the
                                       throttle_timer is started */
  ds_3gppi_throttle_sm_timer_ev_cb_f throttle_sm_timer_expiry_cb;
                                    /* This function cb is invoked when the
                                       throttle_timer expires */
  ds_3gppi_throttle_sm_get_timer_cb_f throttle_sm_get_timer_val_cb;
                                    /* This function cb is invoked to determine
                                       the correct throttling timer value to be
                                       applied.*/
  uint16             throttle_sys_mask;/* current sys-mode mask */

  void*              throttle_sm_timer_start_cb_data;

  void*              throttle_sm_timer_expiry_cb_data;

  void*              throttle_sm_get_timer_val_cb_data;

  sys_modem_as_id_e_type subs_id;

  uint32 cookie;

  uint32            min_random_timer_val;/*user configured min/max random timer values*/

  uint32            max_random_timer_val;/*user configured min/max random timer values*/

} ds_3gppi_throttle_sm_s;

extern rex_crit_sect_type ds_3gppi_throttle_sm_crit_sect;

/*------------------------------------------------------------------------------
  This is the structure to be used to implement the system wide srv req throt 
  state machine. This structure holds an array of pointers to the core throt sm.

  We will instantiate only one object of this type. However, inside this object
  we have an array of 8 pointers to core state machines. This means we will hold
  throttling state of 8 PLMNS simultaneously. 

  If all throt sm's are in use and there is a PLMN
  reselection, we will not be able to throttle on the PLMN that the UE 
  reselected to.
------------------------------------------------------------------------------*/
typedef struct
{
  ds_3gppi_throttle_sm_s *sm_ptr[DS_3GPP_MAX_PLMN_SUPPORTED]; 
                                       // array of pointers to the core throt sm
  uint8                  free_count;   // counter to hold the number of free 
                                       // core sm
} ds_3gpp_throttle_sm_s;

/*------------------------------------------------------------------------------
This enum specifies the possible values of the NV DS_3GPP_CFG_AUTH_TYPE_FALLBACK. 
------------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN = 0, /* Service req throttling with PLMN blocking */
  DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402, /* Service request throttling with PLMN blocking for T3402 duration */
  DS_3GPP_SRV_REQ_THROT_NO_PLMN_BLOCK, /* Service req throt with no plmn blocking */
  DS_3GPP_NO_SRV_REQ_THROT_NO_BLOCK_PLMN,/* No service req throt and no plmn blocking*/
  DS_3GPP_NO_SRV_REQ_THROT_MAX
}ds_3gpp_srv_req_throttle_behavior_e;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CORE_SM_INIT

DESCRIPTION
  This function is used to do the power up procedures needed for the proper
  functioning of the core state machine.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_core_sm_init
(
  void
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_CORE_THROTTLE_SM

DESCRIPTION
  This function gets a new throttle state machine instance to a client. It
  also initializes all the fields appropriately.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gppi_throttle_sm_s* ds_3gppi_throt_sm_get_core_throttle_sm
(
  sys_plmn_id_s_type           plmn_id,
  sys_modem_as_id_e_type       subs_id
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_TIMER_START_EV_CB

DESCRIPTION
  This function registers the throttle_timer_start_ev_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_timer_start_ev_cb
(
  void                               *throt_sm_ptr,
  ds_3gppi_throttle_sm_timer_ev_cb_f  cb_fptr,
  void                               *cb_data
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_TIMER_START_EV_CB

DESCRIPTION
  This function registers the throttle_timer_expiry_ev_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to the newly obtained throttle SM.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_timer_expiry_ev_cb
(
  void                               *throt_sm_ptr,
  ds_3gppi_throttle_sm_timer_ev_cb_f  cb_fptr,
  void                               *cb_data
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_REGISTER_GET_THROTTLE_TIMER_VAL_CB

DESCRIPTION
  This function registers the get_throttle_timer_val_cb for the core SM
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cb_fptr - callback function to register
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_register_get_throttle_timer_val_cb
(
  void                                *throt_sm_ptr,
  ds_3gppi_throttle_sm_get_timer_cb_f   cb_fptr,
  void                                 *cb_data
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FREE_CORE_STATE_MACHINE

DESCRIPTION
  This function destroys a throttle SM that a client has been using. This can
  be called whenever throttling is stopped.

  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - Pointer to the throttling SM to be released.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_free_core_state_machine
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_FEED_FAILURE_TO_CORE_SM

DESCRIPTION
  This function is used to apply throttling. This performs the following
  operations:
  1. Set the throttle SM to THROTTLED state
  2. Increment the counter corresponding to the number of rejects
  3. Get the throttle timer value based on the number of rejects
  4. Start the throttling timer
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr              Pointer to the throttling SM to be released.
  cause_code                cause code
  sys_modem_as_id_e_type    subs_id
  *pdn_context_p            Pointer to PDN context
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_feed_failure_to_core_sm
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  void*                   cause_code,
  sys_modem_as_id_e_type   subs_id,
  ds_pdn_context_s        *pdn_context_p
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines if a throttling SM is in throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_is_throttled_flag
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_PLMN_ID

DESCRIPTION
  This function returns the plmn_id for the core SM

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  sys_plmn_id_s_type - plmn_id of the core SM. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_id_s_type ds_3gppi_throt_sm_get_plmn_id
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_FAILURE_COUNT

DESCRIPTION
  This function is used to get the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  failure_count_ptr - out param pointing to the failure count
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - if the incoming params were non-NULL
  FALSE - incoming params NULL
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_get_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  *failure_count_ptr
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_FAILURE_COUNT

DESCRIPTION
  This function is used to set the count of failed attempts that is maintained
  by the core state machine. Note that such an API will not be exposed from
  any wrapper. Failures will always be fed into the wrapper. The wrapper has
  the logic needed to increment counter values.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr  - pointer to the throttling state machine
  failure_count - failure_count to be stored in the core sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - On success
  FALSE - On any failure
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gppi_throt_sm_set_failure_count
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  uint8                  failure_count
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function returns the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - the value that the throttling timer was started with.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_GET_REMAINING_THROTTLE_TIME

DESCRIPTION
  This function returns the remaining time in milliseconds for which
  throttling occurs
  
  This is the internals of state machine implementation - Will also be refered
  to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint32 - remaining throttle time.  

SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gppi_throt_sm_get_remaining_throttle_time
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr  
);

#ifdef FEATURE_DATA_RPM
/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_CLEAR_THROTTLE_TIMER

DESCRIPTION
  This function clears the throttling timer for the specified core throt_sm.

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gppi_throt_sm_clear_throttle_timer
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
);
#endif /* FEATURE_DATA_RPM */

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_INIT

DESCRIPTION
  This function initializes Service Request Throttle SM for a particular
  Subscription.

PARAMETERS  
  Subs Id

DEPENDENCIES 
  None.

RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_srv_req_throt_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_INIT

DESCRIPTION
  This function initializes the singleton systemwide throttling state machine
  This makes use of the functionality available in the core state machine. In
  other words, this is the client of the core state machine.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_init
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLING_SM

DESCRIPTION
  This function will be invoked by clients to get a handle to the singleton
  object that will be maintained for system wide throttling of service reqs.
  
PARAMETERS  
  create_if_not_present - This flag is used to determine if we have to create
                          a core sm if the wrapper sm does not have one for
                          the current PLMN
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void* - A non-null void pointer will be given to clients. This is an opaque
          handle for the clients

  NULL  - If the singleton object for system wide throttling of service req was
          not created during power up. 

SIDE EFFECTS 
  None.
  
===========================================================================*/
void* ds_3gpp_throt_sm_get_throttling_sm
(
  boolean                create_if_not_present,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_PLMN_NON_NULL

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_FEED_FAILURE

DESCRIPTION
  This function feeds Service reject into the throttlig machine. This func
  takes care of figuring out if the number of consecutive failures has gone
  up and uses the core sm to apply appropriate back-off

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  failure_count - The number of consecutive failures 
   ds_pdn_context_s  - *pdn_cntxt_ptr
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_feed_failure
(
  void*                  throt_sm_ptr,
  sys_modem_as_id_e_type subs_id,
  ds_pdn_context_s      *pdn_cntxt_ptr
);
/*===========================================================================
DESCRIPTION
  This function used to set the min failure count to given value if it is less 
 than that

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  sys_modem_as_id_e_type subs id  
uint16 fail_count
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/

void ds_3gpp_throt_sm_validate_min_failure_count
(
  void * throt_sm_ptr, 
  sys_modem_as_id_e_type subs_id, 
  uint16 fail_count 
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_RESET

DESCRIPTION
  This function is used to reset to state machine. This function internally
  cleans up the core state machine.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_reset
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
);
/*===========================================================================
FUNCTION DS_3GPP_GET_IS_THROTTLED_FLAG

DESCRIPTION
  This function examines the presence of a core SM. The abscence of a core
  SM means no throttling is in place. If the core sm is present, then we
  query the core sm about the throttling state.
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  subs_id      - data subs

DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_3gpp_throt_sm_get_is_throttled_flag
(
  void*                   throt_sm_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*FUNCTION DS_3GPP_THROT_SM_RESET_REL10_PER_PLMN_SM

DESCRIPTION
  This function resets the throttle statae machine 
  paramaters asscoiated with release 10 paramters 
  
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine

DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE  - If the SM is in throttled state
  FALSE - If the SM is not in throttled state
   
SIDE EFFECTS 
  None.

===========================================================================*/


void ds_3gpp_throt_sm_reset_rel_10_per_plmn_sm
(
  void*                   throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_CLR_SRV_REQ_THROT

DESCRIPTION
  This function is registered as callback for LPM event. It clears the
  service request throttling state machine.
  
PARAMETERS  
  event_info - info about the LPM event
  user_data  - ignored as of now
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_clr_srv_req_throt
(
  ds_3gpp_event_info_u_type event_info,
  uint32                    user_data
);
/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GO_NULL

DESCRIPTION
  This function is used to clean up the service request throttling state machine.
  This function internally cleans up the core state machine.

PARAMETERS  
 subs_id
 enable_plmn
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_go_null
(
  sys_modem_as_id_e_type subs_id,
  boolean                enable_plmn
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_BACK_OFF_TIMER_VAL

DESCRIPTION
  This function is used to get the time for which throttling will happen.
  The client can use this any way it wants.

PARAMETERS  
  void*         - Pointer to the throttling SM - opaque handle from client
  uint32*       - Pointer to the timer_val - OUT param 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_get_back_off_timer_val
(
  void                  *sm_ptr, 
  uint32*                timer_val_ptr,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLING_STATE

DESCRIPTION
  This function is used to get the state of the throttling sm

PARAMETERS  
  throt_sm_ptr         - Pointer to the throttling SM - opaque handle from client
  throt_state_ptr      - Pointer to the throt_state - OUT param 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If all went fine
  FALSE - If anything fails.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_get_throttling_state
(
  void                    *throt_sm_ptr, 
  boolean                 *throt_state_ptr,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_3GPPI_THROT_SM_SET_THROTTLE_TIMER_VAL

DESCRIPTION
  This function sets the timer value for which throttling is to be done.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine
 
PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  timer_val    - Value to be set
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_throt_sm_set_throttle_timer_val
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr,
  uint32                  timer_val 
);

/*===========================================================================
FUNCTION DS_3GPPI_SET_IS_THROTTLED_FLAG

DESCRIPTION
  This function sets a throttling SM to throttled or unthrottled
  state
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  is_throttled - boolean value to set the is_throttled flag
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_3gppi_throt_sm_set_is_throttled_flag
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  boolean                 is_throttled
);

/*===========================================================================
FUNCTION DS_3GPPI_GET_USER_DATA

DESCRIPTION
  This function returns the user_data contained in the core throttle sm.
  
  This is the internals of state machine implementation - Will also be
  referred to as the core state machine

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ds_3gpp_throt_user_data 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/

ds_3gpp_throt_user_data *ds_3gppi_throt_sm_get_user_data
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_GET_THROTTLE_FAILURE_TIMER

DESCRIPTION
  This function returns the throttle failure timer based on cause_code

PARAMETERS  
  throt_sm_ptr - pointer to the throttling state machine
  cause_code - cause_code provided by network in reject message
  ds_pdn_context_s      - *pdn_context_p
  srv_req_throt         - Indicates if throttling is srv_req
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void *
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_throt_sm_get_throttle_failure_timer
(
  ds_3gppi_throttle_sm_s *throt_sm_ptr,
  void                   *cause_code,
  ds_pdn_context_s       *pdn_context_p,
  boolean                 srv_req_throt
);

/*===========================================================================
FUNCTION      DS_3GPP_APN_THROTTLE_GET_T3396_TIMER

DESCRIPTION 
  This function returns the throttling failure timer value

PARAMETERS 
  apn_name     - apn name passed by the client
  plmn_id      - plmn id passed by the client
  subs_id      - subscription passed by the client  

RETURN VALUE 
  PDN throttle Failure Timer value in msec

SIDE EFFECTS 
  None
===========================================================================*/
uint32 ds_3gpp_apn_throt_get_t3396_timer
(
  byte                     *apn_name,
  sys_plmn_id_s_type        plmn_id,
  uint8                    *index,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SYS_SEL_PREF_NO_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an no error, This function clears all the retry/timer values
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_throt_sm_sys_sel_pref_no_err_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_SYS_SEL_PREF_ERR_CMD_HDLR
 
DESCRIPTION
  This function handles when sys selection enable/disable has resulted in
  an error, It establishes a retry mechanism to enable/disable LTE
  
PARAMETERS   : 
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_throt_sm_sys_sel_pref_err_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_RETRY_TIMER_EXPIRY_CMD_HDLR

DESCRIPTION
  This function would retry to enable/disable sys selection

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_retry_timer_expiry_cmd_hdlr
(
   void
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_CLR_RETRY_SM

DESCRIPTION
  This function clears sys_sel_retry_sm 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_retry_timer_expiry
(
 unsigned long callback_data  
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_CLR_RETRY_SM

DESCRIPTION
  This function clears sys_sel_retry_sm 

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_clr_retry_sm
(
   void
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_ENABLE_CC_SP_THROTTLING

DESCRIPTION
  This function returns the NV item value of the cc_sp_throttling

PARAMETERS  
None
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_set_cc_sp_throttling
(
  sys_modem_as_id_e_type  subs_id,
  boolean                 value
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_IS_CC_SP_THROTTLING_ENABLED

DESCRIPTION
  This function returns the NV item value of the cc_sp_throttling

PARAMETERS  
None
DEPENDENCIES 
  None.
  
RETURN VALUE 
boolean
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_sm_is_cc_sp_throttling_enabled
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_RETRY_TYPE

DESCRIPTION
  This function sets the retry type

PARAMETERS
  ds_3gpp_throt_sys_sel_retry_e_type     retry_type           
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_retry_type
(
   ds_3gpp_throt_sys_sel_retry_e_type     retry_type
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_PLMN_BLK_INFO

DESCRIPTION
  This function sets the PLMN BLK INFO, This is needed to 
  recalculate the retry time when a disable fails.

PARAMETERS 
  PLMN blocking info BLOB 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_plmn_blk_info
(
   cm_plmn_blocking_info_s_type *plmn_blocking_info_p
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SYS_SEL_PREF_PARAMS

DESCRIPTION
  This function sets the sys_sel_pref_params, This is needed to 
  send out the LTE disable retry request

PARAMETERS 
  sys_sel_pref_params_p 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_sys_sel_pref_params
(
   cm_sys_sel_pref_params_s_type *sys_sel_pref_params_p
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SUBS_ID

DESCRIPTION
  This function sets the subs id

PARAMETERS
  subs_id           
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_subs_id
(
   sys_modem_as_id_e_type  subs_id
);


#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/

/*===========================================================================
FUNCTION DS_3GPP_THROT_PROCESS_CMDS

DESCRIPTION
  This function processes the commands sent to Throttle SM.

PARAMETERS  
  CMD Ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_throt_sm_process_cmds
(
  const ds_cmd_type *cmd_ptr 
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_IS_SRV_REG_THROTTLE_BEHAVIOUR_T3402_ENABLED

DESCRIPTION
  This function checks if T3402 throttle is enabled.

PARAMETERS  
  CMD Ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean TRUE :  T3402 enabled
          FALSE : T3402 disabled
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throttle_is_srv_req_throttle_behavior_t3402_enabled
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SRV_THROT_EV_ALLOC_MEM

DESCRIPTION
  This function would allocate memory for svc_throt_event_ptr from heap

PARAMETERS
  Void
 
DEPENDENCIES
  None.

RETURN VALUE 
  ds_3gpp_throt_svc_event_type ptr
 
SIDE EFFECTS
  None.

===========================================================================*/

ds_3gpp_throt_svc_event_type *ds_3gpp_throt_srv_throt_ev_alloc_mem
(
   void 
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_SRV_THROT_GEN_EVENT

DESCRIPTION
  This function would generate an event when service request throttling
  happens

PARAMETERS
  ds_3gpp_throt_svc_event_type  ptr
 
DEPENDENCIES
  The ptr must be populated with appropriate values

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/

void  ds_3gpp_throt_srv_throt_gen_event
(
   ds_3gpp_throt_svc_event_type *srv_throt_ev_ptr
);


/*===========================================================================
FUNCTION  DS_3GPP_THROT_HDL_SRV_THROT_GEN_EVENT

DESCRIPTION
  This function would populate the structure and generate an event
  when service request throttling happens

PARAMETERS
   uint8                            failure_count
   uint16                           timer_val
   boolean                          is_throttled
   ds_3gpp_throt_svc_action_e_type  svc_action_type
   sys_modem_as_id_e_type           subs_id
   sys_plmn_id_s_type               *plmn_id
 
DEPENDENCIES

RETURN VALUE 
  void
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_hdl_srv_throt_gen_event
(
   uint8                            failure_count,
   uint32                           timer_val,
   boolean                          is_throttled,
   ds_3gpp_throt_svc_action_e_type  svc_action_type,
   sys_modem_as_id_e_type           subs_id,
   sys_plmn_id_s_type               *plmn_id
);


/*===========================================================================
FUNCTION  DS_3GPP_THROT_SYS_SEL_SET_SVC_THROT_EV_INFO

DESCRIPTION
  This function sets the svc throttle info necessary for logging 

PARAMETERS 
  
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_throt_sys_sel_set_svc_throt_ev_info
(
   ds_3gpp_throt_svc_event_type *svc_throt_ev_info
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_PREP_FOR_LTE_BLOCKING_AND_LOG

DESCRIPTION
  This function prepares all the necessary variables, throttle retry sm's
  before calling ds3gmshif function to actually block/disable LTE
 
PARAMETERS  
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  sys_modem_as_id_e_type  subs_id,
  uint8                   failure_count
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE                    If the function succeeds
  FALSE                   Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_prep_for_lte_blocking_and_log
(
  sys_plmn_id_s_type      plmn_id,
  uint32                  timer_val,
  sys_modem_as_id_e_type  subs_id,
  uint8                   failure_count,
  boolean                 detach
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_IS_SERV_REQ_THROT_SM

DESCRIPTION
  This function checks whether the throt_sm_ptr passed is an 
   instance of service req throttling.
 
PARAMETERS  
  ds_3gppi_throttle_sm_s      throt_sm_ptr,
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE                    If throt_sm_ptr is service_req_throt_sm
  FALSE                   Otherwise
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_throt_is_serv_req_throt_sm
(
  ds_3gppi_throttle_sm_s* throt_sm_ptr
);

/*===========================================================================
FUNCTION  DS_3GPP_THROT_GET_SRV_REQ_THROTTLE_BEHAVIOR

DESCRIPTION
  This function returns the 3GPP srv req throttle behavior

PARAMETERS
   subs id - subs id for the throttle behavior            
 
DEPENDENCIES
  None.

RETURN VALUE 
   ds_3gpp_srv_req_throttle_behavior - 3GPP srv req throttle behavior

SIDE EFFECTS
  None.

===========================================================================*/
ds_3gpp_srv_req_throttle_behavior_e ds_3gpp_throt_get_srv_req_throttle_behavior
(
  sys_modem_as_id_e_type   subs_id
);


/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_MPPM

DESCRIPTION
  This function sends the currently blocked list of of APNs to MPPM
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm
(
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_MPPM

DESCRIPTION
  This function sends the currently blocked list of of APNs to MPPM
  
PARAMETERS 
  void

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_mppm_on_plmn
(
  sys_modem_as_id_e_type               subs_id,
  sys_plmn_id_s_type               plmn_id
);

/*===========================================================================
FUNCTION DS_3GPP_THROT_SM_ADVERTISE_BLOCKED_APNS_TO_EXTERNAL_CLIENTS

DESCRIPTION
  This function sends the currently blocked list of of APNs to
  external clients
  
PARAMETERS 
  subs_id: Susbcription Id

DEPENDENCIES
  None.

RETURN VALUE 
  void 
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_throt_sm_advertise_blocked_apns_to_external_clients
(
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_MAP_CM_SYS_MODE_TO_RAT_MASK

DESCRIPTION
  This function returns the RAT Mask corresponding to the given CM Sys Mode.
 
PARAMETERS  
  mode: CM Sys Mode
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  RAT Mask
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_map_cm_sys_mode_to_rat_mask
(
  sys_sys_mode_e_type mode
);

/*===========================================================================
FUNCTION DS_3GPP_MAP_CM_SYS_MODE_TO_RAT_TYPE

DESCRIPTION
  This function returns the RAT Type corresponding to the given CM Sys Mode.
 
PARAMETERS  
  mode: CM Sys Mode
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  RAT Type
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_3gpp_throt_rat_e_type ds_3gpp_throt_map_cm_sys_mode_to_rat_type
(
 sys_sys_mode_e_type sys_mode
);

#endif /* DS_3GPP_THROTTLE_SM_H */
