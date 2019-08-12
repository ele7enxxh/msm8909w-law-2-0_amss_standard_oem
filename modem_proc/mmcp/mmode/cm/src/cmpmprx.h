#ifndef CMPMPRX_H
#define CMPMPRX_H

/*===========================================================================

          P M P R O X Y    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Call Manager ( CM )
  to interface with the Policy Manager module.

Copyright (c) 2002 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmpmprx.h#1 $

when       who     what, where, why
--------   ---    -------------------------------------------------------
04/29/13   ak     Initial release.

===========================================================================*/


#ifndef EXTERN
#define EXTERN
#endif

#include "policyman.h"
#include "policyman_msg.h"
#include "policyman_cm.h"
#include "sys.h"
#include "cmi.h"
#include "mcfg_refresh.h"

#define PM_CHG_CHANGE_NONE  0
#define PM_CHG_SVC_SCAN   BM(0)
#define PM_CHG_MODE_CAP   BM(1)
#define PM_CHG_BAND_CAP   BM(2)
#define PM_CHG_UE_MODE    BM(3)
#define PM_CHG_DEVICE_MODE BM(4)
#define PM_CHG_VOICE_DOMAIN_PREF BM(5)
#define PM_CHG_MAX_ACTIVE_DATA BM(6)
#define PM_CHG_RAT_ACQ_ORDER BM(7)
#define PM_CHG_CALL_MODE BM(8)
#define PM_CHG_MAX_MODE_CAP     BM(9)

#define PM_CHG_ALL        ( PM_CHG_SVC_SCAN|\
                            PM_CHG_MODE_CAP|\
                            PM_CHG_BAND_CAP|\
                            PM_CHG_UE_MODE |\
                            PM_CHG_DEVICE_MODE|\
                            PM_CHG_VOICE_DOMAIN_PREF|\
                            PM_CHG_RAT_ACQ_ORDER|PM_CHG_CALL_MODE ) 

#define PM_CHG_DEVICE_ALL ( PM_CHG_MAX_ACTIVE_DATA|\
                            PM_CHG_DEVICE_MODE|\
                            PM_CHG_MAX_MODE_CAP )

#define PM_SUB_FMODE_MASK_SVLTE
#define PM_SUB_FMODE_MASK_SGLTE
#define PM_SUB_FMODE_MASK_NORMAL

#define POLICY_ITEM_ARRAY_SIZE   3

typedef uint32  policy_change_mask;
typedef uint32  policy_sub_fmode_mask;

typedef enum cm_policy_subsc_change_e
{
  CMPMPRX_SUBSC_NOT_AVAIL,

  CMPMPRX_SUBSC_AVAIL,

  CMPMPRX_SUBSC_NO_CHANGE
} cm_policy_subsc_change_e_type;

typedef struct
{
  boolean   gwl_subsc;
  boolean   cdma_subsc;
}cm_policy_elig_s_type;

typedef struct
{
  size_t                            nSims;
  size_t                            nMaxActive;
  size_t                            nMaxActiveData;
}cm_policy_device_mode_s_type;

typedef struct
{
  sys_subs_feature_t          sub_feature_mode;
  
  policyman_svc_mode_t        service_scan_mode;

  sys_ue_mode_e_type          ue_operation_mode;

  uint32                      policy_mode_capability;

  sys_band_mask_e_type        policy_band_capability;
  sys_lte_band_mask_e_type    policy_lte_band_capability;
  sys_band_mask_e_type        policy_tds_band_capability;
  sys_voice_domain_pref_e_type  voice_domain_pref;
  boolean                       is_voice_dom_pref_locked;
  cm_acq_pri_order_pref_s_type  acq_order_pref;
  sys_call_mode_t             call_mode;  
  uint32                      policy_max_mode_capability;
}cm_policy_items_s_type;

typedef struct
{
  uint32                      pm_max_mode_cap[POLICY_ITEM_ARRAY_SIZE];
  policyman_status_t          max_cap_return[POLICY_ITEM_ARRAY_SIZE];
  boolean                     is_init_policy_valid;
  boolean                     is_sub_feature_valid[POLICY_ITEM_ARRAY_SIZE];
  sys_subs_feature_t          init_sub_feature_mode[POLICY_ITEM_ARRAY_SIZE];

}cm_policy_debug_s_type;

typedef struct policy{

  cm_policy_elig_s_type       is_cm_eligible[3];
  policy_change_mask          policy_changes[3];
  policy_change_mask          device_policy_changes;
  boolean                     is_force_registration;
  boolean                     is_pm_suspend;
  sys_overall_feature_t       feature_mode;
  cm_policy_device_mode_s_type device_mode;
  /* to hold specialization value from PM that tells is Volte only mode for 
  E911 calls is enabled */
  sys_specialization_t        special_volte_e911_calls;
  #ifdef FEATURE_MODEM_CONFIG_REFRESH
  
  mcfg_refresh_status_e_type  mcfg_refresh_status;
  #endif
  cm_policy_items_s_type      current_policy[POLICY_ITEM_ARRAY_SIZE];
  cm_policy_debug_s_type      debug_var;
  size_t                      seqno;

  /* This flag would store the config for nonDDS. Basically, it conveys whether nonDDS Sub is
   allowed to get PS service (Only for VoLTE/ViLTE/VoWiFi/ViWiFi). Please take a note that the
   Sub is nonDDS ans still IMS services are needed on this Sub*/
  boolean                     ims_for_nondds;

  /* This flag keeps track of whether volte_for_nondds_allowed got changed or not*/
  boolean                     change_in_ims_for_nondds;

}cm_policy_config_s_type;


void cmpmprx_init( boolean is_mcfg_refresh );

/*===========================================================================

FUNCTION cmpmprx_update_cm_policy_consumption_eligibility

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpmprx_update_cm_policy_consumption_eligibility_subsc(

       sys_modem_as_id_e_type     asubs_id,

       cm_policy_subsc_change_e_type   gwl_subsc,

       cm_policy_subsc_change_e_type   cdma_subsc
);

/*===========================================================================

FUNCTION cmpmprx_enforce_policy_oprt_mode

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean                           cmpmprx_enforce_policy_oprt_mode(

       sys_modem_as_id_e_type     asubs_id

);

/*===========================================================================

FUNCTION cmpmprx_read_cm_policy_consumption_eligibility

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_read_cm_policy_consumption_eligibility(

       sys_modem_as_id_e_type     asubs_id

);

/*===========================================================================

FUNCTION cmpmprx_is_mask_set

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_is_mask_set(

       sys_modem_as_id_e_type     asubs_id,

       policy_change_mask         check_mask,

       cm_policy_config_s_type   *pm_ptr
);

/*===========================================================================

FUNCTION cmpmprx_is_device_mask_set

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_is_device_mask_set(

       policy_change_mask         check_mask,

       cm_policy_config_s_type   *pm_ptr
);

/*===========================================================================

FUNCTION cmpmprx_policy_chg_ind_proc

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                       cmpmprx_policy_chg_ind_proc(

       cm_policyman_cfg_s         *rpt_ptr
        /* Pointer to the report sent by Message Router */
);

/*===========================================================================

FUNCTION cmpmprx_fetch_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_fetch_mode_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_sys_mode_mask_e_type   *mode_capability_buffer
);

/*===========================================================================

FUNCTION cmpmprx_fetch_voice_domain_pref

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_get_voice_domain_pref(

       sys_modem_as_id_e_type       asubs_id,

       sys_voice_domain_pref_e_type *voice_domain_pref,

       boolean                      *is_voice_dom_pref_locked
);


/*===========================================================================

FUNCTION cmpmprx_get_rat_acq_order_pref

DESCRIPTION
  Returns the acq_order_pref from the current PM policy

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean                    cmpmprx_get_rat_acq_order_pref(

       sys_modem_as_id_e_type         asubs_id,

       cm_acq_pri_order_pref_s_type   *rat_acq_order_ptr
);


/*===========================================================================

FUNCTION cmpmprx_get_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_get_mode_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_sys_mode_mask_e_type   *mode_capability_buffer
);

/*===========================================================================

FUNCTION cmpmprx_fetch_max_mode_capability

DESCRIPTION
  Fetch the max mode capability and reset the max mode cap mask.
  This should only be called during initialization.

DEPENDENCIES
None

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/
uint32    cmpmprx_fetch_max_mode_capability(

       sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION cmpmprx_read_max_mode_capability

DESCRIPTION
  Read the max mode capability.
  Note this should only be called during initialization. This does not reset the mask.

DEPENDENCIES
None

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/
uint32    cmpmprx_read_max_mode_capability(

       sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION cmpmprx_read_call_mode_capability

DESCRIPTION
  Read the call mode capability.
  Note this should only be called during initialization. This does not reset the mask.

DEPENDENCIES
None

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/

void    cmpmprx_read_call_mode_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_call_mode_t            *call_mode_capability_buffer
);

/*===========================================================================

FUNCTION cmpmprx_get_band_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_get_band_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_band_mask_e_type       *band_capability_buffer,

       sys_lte_band_mask_e_type   *lte_band_capability_buffer,

       sys_band_mask_e_type       *tds_band_capability_buffer,

       boolean                    *force_registration
);

/*===========================================================================

FUNCTION cmpmprx_get_call_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_get_call_mode_capability(
       sys_modem_as_id_e_type     asubs_id,

       sys_call_mode_t   *call_mode_capability_buffer
);

/*===========================================================================

FUNCTION cmpmprx_fetch_call_mode_capability

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean cmpmprx_fetch_call_mode_capability(
       sys_modem_as_id_e_type     asubs_id,

       sys_call_mode_t   *call_mode_capability_buffer
);


/*===========================================================================

FUNCTION cmpmprx_get_ue_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_get_ue_mode(

       sys_modem_as_id_e_type     asubs_id,

       sys_ue_mode_e_type         *ue_mode_buffer
);

/*===========================================================================

FUNCTION cmpmprx_read_ue_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void cmpmprx_read_ue_mode(

       sys_modem_as_id_e_type     asubs_id,

       sys_ue_mode_e_type         *ue_mode_buffer
);



/*===========================================================================

FUNCTION cmpmprx_enforce_service_scan_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                       cmpmprx_enforce_service_scan_mode(

       sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION cmpmprx_enforce_voice_domain_pref

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                       cmpmprx_enforce_voice_domain_pref(

       sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION cmpmprx_get_subs_feature_mode

DESCRIPTION
  API for CM to retrieve the new subs specific feature mode.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New subs featuremode retrieved
  False: Operation failed.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_get_subs_feature_mode(

       sys_modem_as_id_e_type     asubs_id,

       sys_subs_feature_t         *subs_featuremode_buffer
);
/*===========================================================================

FUNCTION cmpmprx_enforce_service_scan_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                       cmpmprx_unforce_policy(

       sys_modem_as_id_e_type     asubs_id

);

/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_proc

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                         cmpmprx_sys_sel_pref_proc(

       cm_ph_cmd_info_s_type        *cmd_info_ptr,

       cm_mode_pref_e_type          prefered_mode,

       cm_pref_s_type              *hybr_2_pref_info_ptr,
           /* Preferences of the HYBR2 stack */
       
       #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
       cm_pref_s_type              *hybr_3_pref_info_ptr,
           /* Preferences of the HYBR3 stack */
       #endif

       cm_acq_pri_order_pref_s_type *rat_acq_pri_order
);

/*===========================================================================

FUNCTION cmpmprx_notify_call_end

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void                       cmpmprx_notify_call_end(

       sys_modem_as_id_e_type     asubs_id,
       boolean is_emerg     
);

/*===========================================================================

FUNCTION cmpmprx_notify_emg_call_start

DESCRIPTION
  The function is called to notify cmpmprx that emergency call has started.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmpmprx_notify_emg_call_start(void);


/*===========================================================================

FUNCTION cmpmprx_get_device_mode

DESCRIPTION
  Gets device mode which is decided based on no. of SIM and max SIMs that are
  active at a given time.
  
===========================================================================*/
sys_modem_device_mode_e_type cmpmprx_get_device_mode( void );

/*===========================================================================

FUNCTION cmpmprx_get_num_of_sims

DESCRIPTION
  Get num of sims supported base on Policy manager configuration item.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
size_t  cmpmprx_get_num_of_sims( void );

/*===========================================================================

FUNCTION cmpmprx_get_num_of_active_data

DESCRIPTION
  Get num of active data supported base on Policy manager configuration item.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
size_t  cmpmprx_get_num_of_active_data( void );

/*===========================================================================

FUNCTION cmpmprx_get_volte_allowed_from_pm_on_sub

DESCRIPTION
  Query PM API to get volte allowed for a given Sub

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_get_volte_allowed_from_pm_on_sub(sys_modem_as_id_e_type asub_id);


/*===========================================================================

FUNCTION cmpmprx_rread_device_mode

DESCRIPTION
  Added for the purpose of debugging in case we need to re-read device mode.
  
===========================================================================*/
void cmpmprx_rread_device_mode( void );

/*===========================================================================

FUNCTION cmpmprx_is_ok_to_enforce_policy

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_is_ok_to_enforce_policy(

       cm_policy_config_s_type *policy_ptr,

       sys_modem_as_id_e_type     asubs_id

);

/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_policy_update

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                    cmpmprx_sys_sel_pref_policy_update(

       sys_modem_as_id_e_type     asubs_id,

       cm_sys_sel_pref_params_s_type* pref_ptr,

	   cm_ph_cmd_s_type          *ph_cmd_ptr
);

/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_policy_update

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ph_cmd_err_e_type         cmpmprx_check_sys_sel_pref_policy_conflict(
  cm_ph_cmd_s_type           *ph_cmd_ptr
);

/*===========================================================================

FUNCTION cmpmprx_thermal_state_change_policy_update

DESCRIPTION
  If the new thermal state cause a policy change, retrieve it
  from Policy Manager and update the cmpm_local.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
extern void           cmpmprx_thermal_state_change_policy_update(

      const cfcm_cmd_msg_type_s *cfcm_ind
);

/*===========================================================================

FUNCTION cmpmprx_is_call_to_reorig

DESCRIPTION
  If UE in SGLTE mode and Hybr GSM not in service, CM may need to modify
  policy and re-evaluate the call as per new policy.

  This is done to allow emergency call to be attempted on other RATs when GSM
  CS service not available in SGLTE.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Call command requeued.
  False: Proceed with call origination.

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_is_call_to_reorig(

       cmcall_s_type              *call_ptr,

       void                       *curr_cmd,

       sys_ue_mode_e_type         curr_msc_ue_mode
);

/*===========================================================================

FUNCTION cmpmprx_msim_subsc_changed

DESCRIPTION
  Called when susbcriptions in multi-mode phones change.
  This function calls into PM to update new susbcriptions.
  PM refreshes policies based on this update.
    - Currently PM shall decide enable/disable LTE in multimode sub
      when subscriptions change. Meaning MSIM standby pref change.
===========================================================================*/
boolean cmpmprx_msim_subsc_changed(

  uint8 active_subs,

  sys_modem_as_id_e_type  default_data_subs,

  cm_dds_cause_e_type  dds_status
);

/*===========================================================================

FUNCTION cmpmprx_get_overall_featuremode

DESCRIPTION
  Called from other modules to read the current value of the overall
  featuremode in CMPM
===========================================================================*/
sys_overall_feature_t             cmpmprx_get_overall_featuremode(void);

/*===========================================================================

FUNCTION cmpmprx_clnup_complete_ind

DESCRIPTION
  CM will notify PMPRX that the cleanup for Refresh type 0 OR operating mode
  is complete and PMPRX can enforce any pending policies.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void                       cmpmprx_clnup_complete_ind( void );

/*===========================================================================

FUNCTION cmpmprx_handle_volte_state_notification_cmd

DESCRIPTION
  

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                    cmpmprx_handle_volte_state_notification_cmd(
       cm_volte_state_notification_cmd_s_type *volte_state_cmd
);

/*===========================================================================

FUNCTION cmpmprx_is_special_volte_e911_calls

DESCRIPTION
  Returns specialization value for SYS_SPECIAL_VOLTE_E911_CALLS.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean cmpmprx_is_special_volte_e911_calls(void);

/*===========================================================================

FUNCTION cmpmprx_compute_restricted_mode

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN cm_network_sel_mode_pref_e_type cmpmprx_compute_restricted_mode(

       sys_modem_as_id_e_type           asubs_id,

       cm_network_sel_mode_pref_e_type  net_sel_mode_pref
);
#ifdef CM_DEBUG
#error code not present
#endif
extern cm_policy_config_s_type    *cmpm_ptr( void );

/*===========================================================================

FUNCTION cmpmprx_enforce_current_policy

DESCRIPTION
  Process the policy currently in cmpm_local
===========================================================================*/
EXTERN void cmpmprx_enforce_current_policy(
       sys_modem_as_id_e_type     asubs_id
   );

/*===========================================================================

FUNCTION cmpmprx_enforce_device_policy

DESCRIPTION
  Force action for device policy masks
===========================================================================*/
EXTERN void cmpmprx_enforce_device_policy( 
  policy_change_mask  mask
);

/*===========================================================================

FUNCTION cmpmprx_get_pm_sfmode

DESCRIPTION
  API for CM to get the PM sub-feature mode configured in XML.  
  Note: For 7+5, PM sub-feature mode is different from the CM internal sub-feature mode.

DEPENDENCIES
  none

RETURN VALUE
  sys_subs_feature_t

SIDE EFFECTS
  none

===========================================================================*/
sys_subs_feature_t    cmpmprx_get_pm_sfmode(

       sys_modem_as_id_e_type     asubs_id

);

/*===========================================================================

FUNCTION cmpmprx_report_full_srv_lost

DESCRIPTION
  Reports to PM whenever Full Service Lost indication is received from SD.
  
===========================================================================*/
EXTERN boolean cmpmprx_report_full_srv_lost(
  sys_modem_as_id_e_type     subs_id,
  sys_modem_stack_id_e_type  stack_id
  );

/*===========================================================================

FUNCTION cmpmprx_report_operational_radio_stacks
	 
DESCRIPTION
Reports to PM whenever there is change in operation flag on either sub/stack.

===========================================================================*/
EXTERN boolean cmpmprx_report_operational_radio_stacks(
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================

FUNCTION cmpmprx_sanitize_max_and_curr_cap

DESCRIPTION
  ERR_FATAL if max mode cap and curr mode cap are not obeying Superset/subset relation
 
DEPENDENCIES
  none

RETURN VALUE
  sys_subs_feature_t

SIDE EFFECTS
  none

===========================================================================*/
void cmpmprx_sanitize_max_and_curr_cap(cm_policy_config_s_type *pm_ptr);

/*===========================================================================

FUNCTION cmpmprx_check_and_enforce_policy

DESCRIPTION
  Force policy if any policy element is set


DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void    cmpmprx_check_and_enforce_policy(void);

/*===========================================================================

FUNCTION cmpmprx_read_mode_capability

DESCRIPTION
  Read the  mode capability.

DEPENDENCIES
None

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/
uint32    cmpmprx_read_mode_capability(

       sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION cmpmprx_session_chgd

DESCRIPTION
  Report session change and receive an early copy of the new policy 
  to use from Policyman

===========================================================================*/
void    cmpmprx_session_chgd(

  mmgsdi_session_type_enum_type sessionType,
  boolean                       isAvailable,
  mmgsdi_app_enum_type          appType

);

#endif  /* CMPMPRX_H */
