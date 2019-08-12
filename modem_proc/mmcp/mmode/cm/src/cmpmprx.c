/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      C A L L   M A N A G E R   P O L I C Y   M A N A G E R   P R O X Y

GENERAL DESCRIPTION
  This file is the main implementation for the Policy Manager(PM) proxy. The
  purpose of PMProxy is to act as a proxy to interface with the Policy Manager.
  Interpreting the Policy Manager messages into Call Manager actions.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmpmprx_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmpmprx.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/09/14   ss       Do not enforce policy when UE is OFFLINE
09/12/13    jvo     Fix 2 static analysis errors with one change relating to
                    policy comparison
04/29/13    ak      Initial design

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"       /* Customer configuration file */
#include "comdef.h"         /* Definition for basic types and macros */
#include "cmph.h"
#include "cm_msgr_msg.h"
#include "cmtaski.h"
#include "cmidbg.h"
#include "cmpmprx.h"
#include "cmss.h"
#include "cmmsc.h"
#include "cmsimcoord.h"
#include "cmefs.h"
#include "cmsoa.h"
#include "cmsds.h"
//#include "cmwcall.h"
#ifdef FEATURE_DOMAIN_SELECTION
#include "cmsds.h"
#endif
#include "cmclnup.h"

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmemgext.h"


/*===========================================================================
=============================================================================
=============================================================================
============================  Internal Functions ==================================
=============================================================================
=============================================================================
===========================================================================*/

/* Forward declaration
*/
static policyman_status_t       cmpmprx_retrieve_policy_item_data
(
       cm_policy_config_s_type    *pm_ptr,

       policyman_item_collection_t const *policy_item,

       policy_change_mask         *change_mask,

       policy_change_mask         *device_change_mask

);
static boolean cmpmprx_init_pm_feature_mode( boolean is_mcfg_refresh );
static void cmpmprx_init_pm_ue_mode( boolean is_pwr_up );
static void cmpmprx_set_policy_chg_flag( sys_modem_as_id_e_type asubs_id, policy_change_mask change_value );
static void cmpmprx_reset_policy_chg_flag( sys_modem_as_id_e_type asubs_id, policy_change_mask change_value );
static void cmpmprx_print_current_policy(sys_modem_as_id_e_type asubs_id);
static boolean cmpmprx_compare_policies(cm_policy_config_s_type    *policy_1,cm_policy_config_s_type    *policy_2);

extern void cmph_force_srv_domain_on_nondds_sub(cm_srv_domain_pref_e_type srv_domain_pref);

static cm_policy_config_s_type cmpm_local;
  /* The one and only phone object */

static uint8                   cmpm_active_subs;
static int8                    cmpm_default_data_subs;


/* Function Definations
*/
/*===========================================================================

FUNCTION cmpmprx_is_item_value_changed

DESCRIPTION
  Check if the value of any specific item on subs is really changed.

DEPENDENCIES
None

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_is_item_value_changed(
  cm_policy_config_s_type *curr_policy, 
  cm_policy_config_s_type *local_policy, 
  policyman_item_id_t      item, 
  sys_modem_as_id_e_type   sub_id);

/*===========================================================================

FUNCTION cmpm_ptr

DESCRIPTION
  Return a pointer to the one and only phone policy object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_policy_config_s_type  *cmpm_ptr( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmpm_local;

} /* cmph_ptr() */

/*===========================================================================

FUNCTION cmpmprx_configure_num_policies

DESCRIPTION
  Based on the new number of policies add or delete an existing instance on
  policy in CMPMPRX.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                      cmpmprx_configure_num_policies
(
       cm_policy_config_s_type   *pm_ptr,

       size_t                    new_nSims
)
{
  int                       min_sims = 0;
  uint8                     i=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If number of Sims has not changed, we still have to deal with same number
  ** of policies. */
  if (pm_ptr->device_mode.nSims == new_nSims)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Compute the min value.
  */
  min_sims = MIN(pm_ptr->device_mode.nSims,new_nSims );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If new sims is greater create more policy structures.*/
  for (i=min_sims; i<new_nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
  {
    pm_ptr->current_policy[i].policy_band_capability     = SYS_BAND_MASK_ANY;
    pm_ptr->current_policy[i].policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
    pm_ptr->current_policy[i].policy_tds_band_capability = SYS_BAND_MASK_ANY;
    pm_ptr->current_policy[i].service_scan_mode          = POLICYMAN_SVC_MODE_FULL;
    pm_ptr->current_policy[i].ue_operation_mode          = SYS_UE_MODE_NORMAL;
    if(i == 0)
    {
      pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_ANY;
    }
    else
    {
      pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_GSM;
    }
    pm_ptr->current_policy[i].policy_mode_capability     = pm_ptr->current_policy[i].policy_max_mode_capability;

    pm_ptr->policy_changes[i] = PM_CHG_CHANGE_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If new sims is less than existing sims. reset the extra existing policies */
  for (i=min_sims;i<pm_ptr->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
  {
    pm_ptr->current_policy[i].policy_band_capability     = SYS_BAND_MASK_ANY;
    pm_ptr->current_policy[i].policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
    pm_ptr->current_policy[i].policy_tds_band_capability = SYS_BAND_MASK_ANY;
    pm_ptr->current_policy[i].service_scan_mode          = POLICYMAN_SVC_MODE_FULL;
    pm_ptr->current_policy[i].ue_operation_mode          = SYS_UE_MODE_NORMAL;
    if(i == 0)
    {
      pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_ANY;
    }
    else
    {
      pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_GSM;
    }
    pm_ptr->current_policy[i].policy_mode_capability     = pm_ptr->current_policy[i].policy_max_mode_capability;

    pm_ptr->policy_changes[i] = PM_CHG_CHANGE_NONE;
  }

}

static void cmpmprx_configure_policy_max_active( void )
{
  cm_policy_config_s_type   *pm_ptr = cmpm_ptr();
  cmph_s_type *ph_ptr = cmph_ptr();

    CM_MSG_HIGH_2("Inform device mode to MMOC nsim %d, nmaxactive %d", 
                pm_ptr->device_mode.nSims, pm_ptr->device_mode.nMaxActive); 
    /* Push standby pref change to MMOC */
    cmmsimc_proc_cmd_standby_pref_chgd(ph_ptr->internal_standby_pref, 
                                       ph_ptr->active_subs, 
                                       ph_ptr->is_cdma_subscription_available,
                                       ph_ptr->is_gwl_subscription_available,
                                       ph_ptr->is_hybr_gw_subscription_available,
                                       ph_ptr->is_hybr_gw3_subscription_available,
                                       FALSE);
    
  /* Inform new Max active subs to MCS */
	cmph_sp_subs_info_update();
}

static void cmpmprx_configure_policy_max_active_data( void )
{
 #ifdef FEATURE_MMODE_DUAL_SIM

  sys_modem_as_id_e_type curr_subs;
  cm_ss_e_type ss;
  uint8 loop=0;
  cmph_s_type *ph_ptr = cmph_ptr();
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  
  
  /* This should take care of the case:
     1. SUB1 (IMS for nonDDS) + SUB2 (CS+PS)
     2. maxActiveData changes 2 -> 1
     3. SUB1 (CS) + SUB2 (CS+PS)  */
  if((cmpmprx_get_num_of_active_data() == 1) && cmpm_ptr()->change_in_ims_for_nondds && !pm_ptr->ims_for_nondds)
  {
    cmph_force_srv_domain_on_nondds_sub(CM_SRV_DOMAIN_PREF_CS_ONLY);
  }
  /* This should take care of the case:
     1. SUB1 (CS) + SUB2 (CS+PS)
     2. maxActiveData changes 1 -> 2
     3. SUB1 (IMS for nonDDS) + SUB2 (CS+PS)  */  
  else if((cmpmprx_get_num_of_active_data() == 2) && pm_ptr->change_in_ims_for_nondds && pm_ptr->ims_for_nondds)
  {
    cmph_force_srv_domain_on_nondds_sub(CM_SRV_DOMAIN_PREF_CS_PS);
  }
  else if(cmpmprx_get_num_of_active_data() == 1)
  {
 for(loop=0; loop<pm_ptr->device_mode.nSims; loop++)  
  {
  curr_subs = loop;

  if( loop != ph_ptr->default_data_subs )
  {
   ss = cmph_map_subs_to_ss(curr_subs);

   if(cmcall_check_if_ps_call_active(ss))
   {
     CM_MSG_HIGH_1("Active PS calls on ss %d", ss);

	 if(cmcall_is_no_call_in_gw_ps(ss) == FALSE )
	 {
	   CM_MSG_HIGH_1("Ending active PS calls on ss %d", ss);
	   //cmwcall_send_pdp_abort_req(ss);
	
          cmcall_end_each_call_with_type_per_sub(CM_CALL_TYPE_DATA,
            CM_CALL_DUAL_DATA_NOT_SUPPORTED, curr_subs);
	 }
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
		cmcall_end_active_1x_datacalls(TRUE);
    #endif

  }
  if(ss == CM_SS_MAIN)
  {
   #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
	cmcall_end_active_HDR_datacalls();
   #endif
  }

   /* Delete Data call pref from priority queue, which might be there
   ** for 4 sec after end of data call
   */
  if(cmtask_orig_para_search_act_type(ss,CM_ACT_TYPE_DATA_CALL) != NULL)
  {
    cmph_remove_orig_mode( ss, CM_ACT_TYPE_DATA_CALL, CM_ACT_PRIORITY_MAX );
   /* Force CS_ONLY on the non-DDS sub */
   /* Instead of forcing from persistent
   Force the preferences from top of the queue by calling cmph_force_orig_mode_on_ss
   */
   //cmph_force_orig_mode_on_ss(ss, ph_ptr);
  }

  if(cmtask_orig_para_search_act_type(CM_SS_HDR,CM_ACT_TYPE_DATA_CALL) != NULL)
  {
 	cmph_remove_orig_mode( CM_SS_HYBR_1, CM_ACT_TYPE_DATA_CALL, CM_ACT_PRIORITY_MAX );
  }



      /* Force CS Only mode to NON DDS Subs */
 if(cmph_map_subs_stack_to_ss(curr_subs,0) == CM_SS_MAIN)
 {
   (void) cmph_force_pref_on_the_fly( CM_SS_MAX,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->main_stack_info.pref_info.pref_term,
                                         ph_ptr->main_stack_info.pref_info.mode_pref,
                                         ph_ptr->main_stack_info.pref_info.band_pref,
                                         ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->main_stack_info.pref_info.prl_pref,
                                         ph_ptr->main_stack_info.pref_info.roam_pref,
                                         ph_ptr->main_stack_info.pref_info.hybr_pref,
                                         ph_ptr->main_stack_info.pref_info.plmn,
                                         CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         CM_OTASP_ACT_CODE_NONE,
                                         ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                         ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                         (cm_act_id_type)ph_ptr,
                                         CM_ACT_UPDATE_REAS_USER,
                                         FALSE,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_MAX),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->main_stack_info.pref_info.csg_id,
                                         ph_ptr->main_stack_info.pref_info.csg_rat);
   }

    #ifdef FEATURE_MMODE_DUAL_SIM
      if (!cmph_is_sxlte() && cmph_is_msim() && (cmph_map_subs_stack_to_ss(curr_subs,0) == CM_SS_HYBR_2))
      {
         (void) cmph_force_pref_on_the_fly2( CM_SS_HYBR_2,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->hybr_2_stack_info.pref_info.pref_term,
                                         ph_ptr->hybr_2_stack_info.pref_info.mode_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.prl_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.roam_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.hybr_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.plmn,
                                             CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         ph_ptr->hybr_2_stack_info.pref_info.acq_order_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.network_sel_mode_pref,
                                             &ph_ptr->hybr_2_stack_info.pref_info,
                                          #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                                             &ph_ptr->hybr_3_stack_info.pref_info,
                                          #endif
                                             CM_OTASP_ACT_CODE_NONE,
                                         (cm_act_id_type)ph_ptr,
                                         CM_ACT_UPDATE_REAS_USER,
                                         FALSE,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_2),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_rat);	

	  }
 #endif	  
 
 #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
      if ((cmph_is_msim() || cmph_is_sxlte()) && (cmph_map_subs_stack_to_ss(curr_subs,0) == CM_SS_HYBR_3))
	  {
         CM_MSG_HIGH_0("Forcing CS Only to HYBR3");
         (void)cmph_force_pref_on_the_fly2(
             CM_SS_HYBR_3,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->hybr_3_stack_info.pref_info.pref_term,
                                         ph_ptr->hybr_3_stack_info.pref_info.mode_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.prl_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.roam_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.hybr_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.plmn,
             CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         ph_ptr->hybr_3_stack_info.pref_info.acq_order_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.network_sel_mode_pref,
             &ph_ptr->hybr_2_stack_info.pref_info,
          #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
             &ph_ptr->hybr_3_stack_info.pref_info,
          #endif
             CM_OTASP_ACT_CODE_NONE,
                                         (cm_act_id_type)ph_ptr,
                                         CM_ACT_UPDATE_REAS_USER,
                                         FALSE,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_3),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_rat);

      }
 #endif		

 }
    }
  }

 #endif /* FEATURE_MMODE_DUAL_SIM */ 
  return;
}

/*===========================================================================

FUNCTION cmpmprx_enforce_device_policy

DESCRIPTION
  Force action for device policy masks
===========================================================================*/
EXTERN void cmpmprx_enforce_device_policy( 
  policy_change_mask  mask
)
{

  /* Enforce device mode policy only if any subscription is available */
  /* Otherwise device policy will be picked up during subscription available procedure */
  if(cmph_ptr()->oprt_mode == SYS_OPRT_MODE_ONLINE &&
     ( cmph_ptr()->is_cdma_subscription_available ||
       cmph_ptr()->is_gwl_subscription_available ||
       cmph_ptr()->is_hybr_gw_subscription_available ||
       cmph_ptr()->is_hybr_gw3_subscription_available))
  {
    
   if(cmpm_ptr()->device_policy_changes & PM_CHG_DEVICE_MODE && 
      mask & PM_CHG_DEVICE_MODE) 
   {  
    cmpmprx_configure_policy_max_active();
   }
  }
  else
  {
    CM_MSG_HIGH_0("Device policy discarded. UE NOT Online");
  }

   if((cmpm_ptr()->device_policy_changes & PM_CHG_MAX_ACTIVE_DATA)
      && (mask & PM_CHG_MAX_ACTIVE_DATA))
   {
     /* This should cover both cases i.e. maxActiveData 1->2 and 2->1*/
     cmpmprx_configure_policy_max_active_data();
   }

   if(cmpm_ptr()->device_policy_changes & PM_CHG_MAX_MODE_CAP &&
      mask & PM_CHG_MAX_MODE_CAP)
   {
     cmmsimc_send_subsc_chgd_current_pref(PROT_SUBSC_CHG_GW, PROT_SUBSC_CHG_GW, PROT_SUBSC_CHG_GW);
   }

  cmpm_ptr()->device_policy_changes &= (~mask);
  cmpm_ptr()->change_in_ims_for_nondds = FALSE;

  return;
}

/*===========================================================================

FUNCTION cmpmprx_init_pm_special_volte_e911_calls

DESCRIPTION
  This function is to read E911 specialization from PM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_init_pm_special_volte_e911_calls( void )
{

  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  boolean                 is_policy_valid   = FALSE;

  #ifndef CM_DEBUG

  policyman_item_t        *pItem = NULL;

  policyman_item_id_t     id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  sys_specialization_t    pSpecial = SYS_SPECIAL_NONE;

  if (POLICYMAN_SUCCEEDED(policyman_get_items_block((policyman_item_id_t const *)&id,
                                                     1,
                                                     (policyman_item_t const **)&pItem)
                          )
     )
  {
    if(POLICYMAN_SUCCEEDED(policyman_device_config_get_specialization(pItem, &pSpecial)))
    {
      is_policy_valid = TRUE;
    }
  }
  policyman_item_release((policyman_item_t const *)pItem);
  if(is_policy_valid)
  {
    pm_ptr->special_volte_e911_calls = pSpecial;
  }
  #else    
    #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error in reading the policy item(s). Discard everything and initialize to
  ** default values.
  */
  
  if (!is_policy_valid)
  {
    pm_ptr->special_volte_e911_calls = SYS_SPECIAL_NONE;
  }
  

  /* Print feature mode */
  CM_MSG_HIGH_2("INIT PMPRX: is feature_mode policy valid %d, special_volte_e911_calls %d",
               is_policy_valid, pm_ptr->special_volte_e911_calls);

  /* update SD about the slpecialization item as well */
  sd_ss_update_is_special_volte_e911_calls(pm_ptr->special_volte_e911_calls);

  return;
}
#ifdef FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================

FUNCTION cmpmprx_is_valid_mcfg_refresh

DESCRIPTION
  This function validates compatible subs feature mode changes

DEPENDENCIES
  none

RETURN VALUE
  TRUE if valid sfmode change
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/

static boolean cmpmprx_is_valid_mcfg_refresh (

  sys_subs_feature_t    current_feature_mode,
  /* Current subs feature mode */

                                                  sys_subs_feature_t    new_feature_mode
  /* New subs feature mode */

                                                  )
{
  uint8 i;
  static const struct {
      
            sys_subs_feature_t    current_feature_mode;
                /* current feature mode */
      
            sys_subs_feature_t    new_feature_mode;
                /* new feature mode */
      
        } cmpmprx_feature_mode_change[] = {
      
         /* current feature mode */                 /* new feature mode */
        {SYS_SUBS_FEATURE_MODE_SVLTE,         SYS_SUBS_FEATURE_MODE_SGLTE},
        {SYS_SUBS_FEATURE_MODE_SVLTE,         SYS_SUBS_FEATURE_MODE_SRLTE},

        {SYS_SUBS_FEATURE_MODE_SRLTE,         SYS_SUBS_FEATURE_MODE_SVLTE},
        {SYS_SUBS_FEATURE_MODE_SRLTE,         SYS_SUBS_FEATURE_MODE_SGLTE},

        {SYS_SUBS_FEATURE_MODE_SGLTE,         SYS_SUBS_FEATURE_MODE_SVLTE},
        {SYS_SUBS_FEATURE_MODE_SGLTE,         SYS_SUBS_FEATURE_MODE_SRLTE},


        };

   
  /* For multi sim device check for compatible feature mode changes 
   */

      for( i=0; i < ARR_SIZE(cmpmprx_feature_mode_change); i++ )
      {
        if( cmpmprx_feature_mode_change[i].current_feature_mode == current_feature_mode &&
            cmpmprx_feature_mode_change[i].new_feature_mode   == new_feature_mode )
        {
      return TRUE;
      }
    }

  CM_MSG_HIGH_2("MCFG refresh: Incompatible feature mode change from %d to %d",
    current_feature_mode, new_feature_mode);

  return FALSE;
}

#endif //#ifdef FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================

FUNCTION cmpmprx_init_pm_feature_mode

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmpmprx_init_pm_feature_mode( boolean is_mcfg_refresh )
{

  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  boolean                 is_policy_valid   = FALSE;
  policyman_status_t  status = POLICYMAN_STATUS_ERR;

  #ifndef CM_DEBUG
  policyman_item_collection_t const   *pCollection;

  policyman_item_id_t     id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;

  if (POLICYMAN_SUCCEEDED(policyman_get_items_block_msim((policyman_item_id_t const *)&id,
                                                     1,
                                                     &pCollection)
                          )
     )
  {
    sys_overall_feature_t new_overall_feature_mode = SYS_OVERALL_FEATURE_MODE_NORMAL;
    size_t                new_nSims                = 0;
    size_t                new_nMaxActive           = 0;
	size_t                new_nMaxActiveData       = 1;  
    policyman_item_t const  *pItem = NULL;

    pItem = pCollection->pItems[0];         
    status = policyman_device_config_overall_feature( pItem,
                                                      &new_overall_feature_mode);
    if (POLICYMAN_SUCCEEDED(status))
    {
      if ( new_overall_feature_mode != SYS_OVERALL_FEATURE_MODE_MULTISIM )
      {
        new_nSims = new_nMaxActive = 1;
      }
      else
      {
        if(!POLICYMAN_SUCCEEDED(policyman_device_config_num_sims(pItem, &(new_nSims)))
            ||
		   !POLICYMAN_SUCCEEDED(policyman_device_config_max_active_data(pItem, &(new_nMaxActiveData)))
	        ||
           !POLICYMAN_SUCCEEDED(policyman_device_config_max_active(pItem, &(new_nMaxActive)))
          )
        {
          status = POLICYMAN_STATUS_ERR;
        }
      }
    }

    if (POLICYMAN_SUCCEEDED(status))
    {
      size_t i=0;
	  
      cmpmprx_configure_num_policies(pm_ptr,new_nSims);
      if(is_mcfg_refresh)
      {
       /* During MCFG refresh on a multisim device when there is device mode change
       ** do not update cmpm ptr values and return after updating the mcfg status 
       ** We expect reset in such scenarios
       */ 
       #ifdef FEATURE_MODEM_CONFIG_REFRESH
        if(pm_ptr->device_mode.nSims != new_nSims)
        {
          pm_ptr->mcfg_refresh_status = MCFG_REFRESH_STATUS_INCOMPAT_DEVICE_MODE;
          CM_MSG_HIGH_2("Incompatible device mode change from %d to %d",pm_ptr->device_mode.nSims, new_nSims);
		  policyman_item_collection_release(pCollection);
          return FALSE;
        }
        #endif
      }
      pm_ptr->feature_mode             = new_overall_feature_mode;
      pm_ptr->device_mode.nSims        = new_nSims;
      pm_ptr->device_mode.nMaxActive   = new_nMaxActive;
	  pm_ptr->device_mode.nMaxActiveData = new_nMaxActiveData;
	  pm_ptr->seqno = pCollection->seqno;
      for (i=0;i<new_nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
      {
        sys_subs_feature_t          temp_sub_feature_mode;
        if(POLICYMAN_SUCCEEDED(policyman_device_config_get_subs_feature(pItem,0,i,&temp_sub_feature_mode)))
        {
          #ifdef FEATURE_MODEM_CONFIG_REFRESH
          /* During MCFG refresh on a multisim device whenever there is
          ** incompatible feature mode change return without updating cmpm ptr values
          ** When MCFG refresh fails device reset will be triggered
          ** When device is DSSS mode allow feature mode changes to go through
          ** as the other sub is not active 
          */ 
          if(is_mcfg_refresh && cmph_is_msim()&& 
            (pm_ptr->current_policy[i].sub_feature_mode != temp_sub_feature_mode) &&
            (cmph_ptr()->user_active_subs != SYS_MODEM_AS_ID_1_MASK))
          {
            if (!cmpmprx_is_valid_mcfg_refresh(pm_ptr->current_policy[i].sub_feature_mode, 
              temp_sub_feature_mode))
            {
              pm_ptr->mcfg_refresh_status = MCFG_REFRESH_STATUS_INCOMPAT_FEATURE_MODE;
              return FALSE;
            }
          
          }
          #endif
          pm_ptr->current_policy[i].sub_feature_mode = temp_sub_feature_mode;
          pm_ptr->debug_var.is_sub_feature_valid[i] = TRUE;
          pm_ptr->debug_var.init_sub_feature_mode[i] = temp_sub_feature_mode;
        }
        else
        {
          pm_ptr->current_policy[i].sub_feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL;
          pm_ptr->debug_var.is_sub_feature_valid[i] = FALSE;
          pm_ptr->debug_var.init_sub_feature_mode[i] = SYS_SUBS_FEATURE_MODE_NORMAL;
          status = POLICYMAN_STATUS_ERR;
        }
      }
    }
    if (POLICYMAN_SUCCEEDED(status))
    {
      size_t j=0;
      uint32 max_mode_cap = 0;
      /* Read Device max capabilities */
      for(j=0;j < new_nSims; j++)
      {
        policyman_status_t max_cap_policy_status = 
          policyman_device_config_get_current_subphone_capability(pItem, 
            j, &max_mode_cap);
        pm_ptr->debug_var.max_cap_return[j] = max_cap_policy_status;
        if(!POLICYMAN_SUCCEEDED(max_cap_policy_status))
        {
          status = POLICYMAN_STATUS_ERR;
          CM_MSG_HIGH_1("INIT PMPRX: MaxCap read error sim %d",j);
        }
        else
        {
          if(status != POLICYMAN_STATUS_ERR)
          {
            status = POLICYMAN_STATUS_SUCCESS;
          }
          pm_ptr->current_policy[j].policy_max_mode_capability = max_mode_cap;
          pm_ptr->debug_var.pm_max_mode_cap[j] = max_mode_cap;
          CM_MSG_HIGH_2("INIT PMPRX: updating max mode cap sub %d to %d",j,max_mode_cap);
        }

      }
    }
    if(POLICYMAN_SUCCEEDED(status))
    {
      is_policy_valid = TRUE;
    }
  }
  
  policyman_item_collection_release(pCollection);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If num of sims from PM exceeds the MAX_SIMS supported in CM,
  ** default the policy.
  */
  if (pm_ptr->device_mode.nSims > MAX_SIMS || pm_ptr->device_mode.nMaxActive > MAX_SIMS || pm_ptr->device_mode.nMaxActiveData > MAX_SIMS)
  {
    is_policy_valid = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error in reading the policy item(s). Discard everything and initialize to
  ** default values.
  */
  CM_MSG_HIGH_1("INIT PMPRX: is feature_mode policy valid %d", is_policy_valid);
  if (!is_policy_valid)
  {
    pm_ptr->feature_mode = SYS_OVERALL_FEATURE_MODE_NORMAL;
    cmpmprx_configure_num_policies(pm_ptr,1);
    pm_ptr->device_mode.nSims = 1;
    pm_ptr->device_mode.nMaxActive = 1;
	pm_ptr->device_mode.nMaxActiveData = 1;
	pm_ptr->seqno = 0;
  }
  #endif

  /* Print feature mode */
  CM_MSG_HIGH_1("INIT PMPRX: Overall Feature_mode = %d", pm_ptr->feature_mode);

  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_compare_policies

DESCRIPTION
  To compare policy for all subs.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: change in policy
  False: no change in policy.

SIDE EFFECTS
  none

===========================================================================*/
static boolean                    cmpmprx_compare_policies(

       cm_policy_config_s_type    *old_policy,

       cm_policy_config_s_type    *new_policy
)
{
  unsigned int i = 0;

  for(i=0; i<new_policy->device_mode.nSims; i++)
  {
    if(memcmp(&(old_policy->current_policy[i]),
              &(new_policy->current_policy[i]),
              sizeof(cm_policy_items_s_type)) != 0)
    {
      return TRUE;
    }
  }

  return FALSE;
  
}

/*===========================================================================

FUNCTION cmpmprx_read_pm_device_mode

DESCRIPTION
  Reads no. of SIM and max active SIMs.
===========================================================================*/

cm_policy_device_mode_s_type   cmpmprx_read_pm_device_mode(void)
{
  cm_policy_device_mode_s_type  device_mode;
  const policyman_item_t *pItem = NULL;
  const policyman_item_id_t id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();

  /* If non multi-sim, set the default value */
  if(pm_ptr->feature_mode != SYS_OVERALL_FEATURE_MODE_MULTISIM)
  {
    device_mode.nSims = 1;
    device_mode.nMaxActive = 1;
    device_mode.nMaxActiveData = 1;
  }
  else
  {
    if (policyman_get_items_block(&id, 1, &pItem)== POLICYMAN_STATUS_SUCCESS)
    {
      policyman_device_config_num_sims(pItem, &(device_mode.nSims));
      policyman_device_config_max_active(pItem, &(device_mode.nMaxActive));

      // If there's clear agreement, we can always query PM API.
      #ifdef CM_DEBUG
      #error code not present
#else
      device_mode.nMaxActiveData = 1;
      #endif
      
    }
    else
    {
      device_mode.nSims = 1;
      device_mode.nMaxActive = 1;
      device_mode.nMaxActiveData = 1;
    }
    policyman_item_release(pItem);
  }
  return device_mode;
}

/*===========================================================================

FUNCTION cmpmprx_init_pm_ue_mode

DESCRIPTION
  Read sglte related configuration from Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_init_pm_ue_mode( boolean is_pwr_up )
{

  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  boolean                 is_policy_valid   = FALSE;
  uint8                   i                 = 0;
  cm_policy_config_s_type *local_policy = NULL;

  #ifndef CM_DEBUG
  policy_change_mask      local_mask[3]     = {0,0,0};
  policyman_item_id_t     pIds[]            =
  {
    POLICYMAN_ITEM_RAT_CAPABILITY,
    POLICYMAN_ITEM_RF_BANDS,
    POLICYMAN_ITEM_SVC_MODE,
    POLICYMAN_ITEM_UE_MODE,
    POLICYMAN_ITEM_VOICE_DOMAIN_PREF,
    POLICYMAN_ITEM_RAT_ACQ_ORDER,
    POLICYMAN_ITEM_CALL_MODE,
    POLICYMAN_ITEM_DEVICE_CONFIGURATION,
  };

  size_t                  numIds            = ARR_SIZE(pIds);

  policyman_item_collection_t const  *pCollection;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pm_ptr != NULL );

  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! CMPMPRX INIT: unable to process policy as cannot allocate memory");
    return;
  }

  *local_policy = *(cmpm_ptr());
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the configuration from the policy manager. */
  if(POLICYMAN_SUCCEEDED(policyman_get_items_block_msim(pIds,numIds,&pCollection)))
  {
    is_policy_valid = TRUE;

    if( !POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( local_policy,pCollection,local_mask,NULL)) )
    {
      
      is_policy_valid = FALSE;
      CM_MSG_HIGH_0("CMPRX: cmpmprx_retrieve_policy_item_data failed");
    }
    else
    {
      *pm_ptr = *local_policy;
      for (i=0;i<(int)pm_ptr->device_mode.nSims;i++)
      {
        if (is_pwr_up)
        {
          local_mask[i] |= (PM_CHG_MODE_CAP | PM_CHG_BAND_CAP | PM_CHG_SVC_SCAN | PM_CHG_UE_MODE | 
            PM_CHG_VOICE_DOMAIN_PREF | PM_CHG_RAT_ACQ_ORDER | PM_CHG_CALL_MODE);
        }
       pm_ptr->policy_changes[i] |= local_mask[i];
       CM_MSG_HIGH_2("Msgr policy[%d] mask = %x",i,pm_ptr->policy_changes[i]); 
      }
    }
  }

  policyman_item_collection_release(pCollection);

  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error in reading the policy item(s). Discard everything and initialize to
  ** default values.
  */
  CM_MSG_HIGH_1("CMPRX: is_policy_valid %d", is_policy_valid);
  pm_ptr->debug_var.is_init_policy_valid = is_policy_valid;

  if (!is_policy_valid)
  {
    for(i = 0 ; i < pm_ptr->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE; ++i)
    {
      pm_ptr->current_policy[i].policy_band_capability     = SYS_BAND_MASK_ANY;
      pm_ptr->current_policy[i].policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
      pm_ptr->current_policy[i].policy_tds_band_capability = SYS_BAND_MASK_ANY;
      pm_ptr->current_policy[i].service_scan_mode          = POLICYMAN_SVC_MODE_FULL;
      pm_ptr->current_policy[i].ue_operation_mode          = SYS_UE_MODE_NORMAL;
      pm_ptr->current_policy[i].call_mode                  = SYS_CALL_MODE_NORMAL;
      #ifndef CM_DEBUG
      pm_ptr->current_policy[i].policy_mode_capability     = SD_SS_MODE_PREF_ANY;
      if(i == 0)
      {
        pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_ANY;
      }
      else
      {
        pm_ptr->current_policy[i].policy_max_mode_capability = SD_SS_MODE_PREF_GSM;
      }
      CM_MSG_HIGH_2("CMPRX: setting sub %d policy max mode cap to %d", i,pm_ptr->current_policy[i].policy_max_mode_capability);

      #endif
      pm_ptr->current_policy[i].policy_mode_capability     = pm_ptr->current_policy[i].policy_max_mode_capability;

      cmpmprx_reset_policy_chg_flag( i,PM_CHG_ALL );
    }
    /* No action need be taken to enforce this policy. So resetting all flags.
    */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i = 0 ; i < pm_ptr->device_mode.nSims ; ++i)
  {
    if(is_pwr_up)
    {
      cmpmprx_update_cm_policy_consumption_eligibility_subsc(i,
                                                      CMPMPRX_SUBSC_NOT_AVAIL,
                                                      CMPMPRX_SUBSC_NOT_AVAIL);
    }
    else
    {
      CM_MSG_HIGH_0("Not updating policy consumptin eligibility");
    }

    cmpmprx_print_current_policy(i);
  }
  if (local_policy != NULL)
  {
    cm_mem_free(local_policy);
  }
  
  return;

}

/*===========================================================================
FUNCTION cmpmprx_update_flags_ims_for_nondds

DESCRIPTION
  Here, we update the flag ims_for_nondds by calling PM boolean API. PM boolean API is called only when
  Max Active Data is 2. Basically, at the time of DDS change and Device Policy change, CM needs to update
  flags pertaining to 'IMS for nonDDS feature'.

DEPENDENCIES
  none

RETURN VALUE
  Void

SIDE EFFECTS
  none
===========================================================================*/
void cmpmprx_update_flags_ims_for_nondds(sys_modem_as_id_e_type  default_data_subs)
{
  cm_policy_config_s_type  * pm_ptr = cmpm_ptr();
  sys_modem_as_id_e_type non_default_data_subs = (default_data_subs == SYS_MODEM_AS_ID_1)? SYS_MODEM_AS_ID_2 : SYS_MODEM_AS_ID_1;
  
  // For DSDS only, PM needs to be queried and flags need to be updated.
  if(cmpmprx_get_device_mode()== SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY)
  {
    boolean ims_for_nondds = cmpmprx_get_volte_allowed_from_pm_on_sub(non_default_data_subs);

    /* This should cover two cases 1) DDS change when 'IMS for nonDDS feature' is already up 2) maxActiveData changes 1 -> 2*/
    if(ims_for_nondds) 
    {
      if(pm_ptr->change_in_ims_for_nondds == FALSE)
      {
        pm_ptr->change_in_ims_for_nondds = ims_for_nondds ^ pm_ptr->ims_for_nondds;
      }
      else
      {
        CM_MSG_HIGH_0("Flag change_in_ims_for_nondds already Set!!"); // Ideally this shouldn't happen (frequently) so adding F3
      }

      pm_ptr->ims_for_nondds = ims_for_nondds;
    }
    else/* This should cover the case of maxActiveData changes 2 -> 1 */
    {
      if(pm_ptr->ims_for_nondds)
      {
        pm_ptr->ims_for_nondds = FALSE;
        pm_ptr->change_in_ims_for_nondds = TRUE;
        CM_MSG_HIGH_1("nMaxActiveData = %d. Set change_in_ims_for_nondds & Reset ims_for_nondds!",cmpmprx_get_num_of_active_data());
      }
    }

    CM_MSG_HIGH_3("non_default_data_subs %d, ims_for_nondds %d, change_in_ims_for_nondds %d",
      non_default_data_subs,pm_ptr->ims_for_nondds,cmpm_ptr()->change_in_ims_for_nondds);
  }
}

/*===========================================================================

FUNCTION cmpmprx_init

DESCRIPTION
  Initialize the common functionality of Policy Manager Proxy object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpmprx_init( boolean is_mcfg_refresh )
{

  #ifdef FEATURE_MODEM_CONFIG_REFRESH
  cmpm_ptr()->mcfg_refresh_status = MCFG_REFRESH_SUCCESS;
  #endif

  if(!is_mcfg_refresh)
  {
    cmpm_ptr()->ims_for_nondds = FALSE;
  }

  /* Query feature_mode from policy mgr */
  /* During MCFG refresh when incompatible device mode/feature mode change is detected
  ** return without continuing with cmpmprx init
  */
  if(!cmpmprx_init_pm_feature_mode(is_mcfg_refresh))
  {
     return;
  }

  #ifdef CM_DEBUG
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set active subs to 0 in init, get max sub capability */
  if (!is_mcfg_refresh)
  {
  cmpm_active_subs = 0;
    
    /* Initialize is_pm_supsend */
    cmpm_ptr()->is_pm_suspend = FALSE;
  }
  
  cmpm_default_data_subs = -1;  
  /* Retrieve the initial policy from the Policy Manager. */
  cmpmprx_init_pm_ue_mode(!is_mcfg_refresh);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  cmpmprx_init_pm_special_volte_e911_calls();

  return;
}

/*===========================================================================

FUNCTION cmpmprx_set_policy_chg_flag

DESCRIPTION
  Maintain the bitmask for what all policy configuration items have changed
  since last policy was enforced.

  If PM_CHG_NO_CHANGE passed to the function, clear the bitmask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_set_policy_chg_flag(

       sys_modem_as_id_e_type asubs_id,

       policy_change_mask change_value
)
{
  if(asubs_id == SYS_MODEM_AS_ID_1 || asubs_id == SYS_MODEM_AS_ID_2 || asubs_id == SYS_MODEM_AS_ID_3 )
  {
  cmpm_ptr()->policy_changes[asubs_id] |= change_value;
    CM_MSG_HIGH_3("cmpmprx_set_policy_chg_flag() on subs_id = %d change_value = %d policy_changes = %d",asubs_id,change_value,cmpm_ptr()->policy_changes[asubs_id]);
  }
}

/*===========================================================================

FUNCTION cmpmprx_reset_policy_chg_flag

DESCRIPTION
  Maintain the bitmask for what all policy configuration items have changed
  since last policy was enforced.

  If PM_CHG_NO_CHANGE passed to the function, clear the bitmask.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                       cmpmprx_reset_policy_chg_flag(

       sys_modem_as_id_e_type     asubs_id,

       policy_change_mask         change_value
)
{
  if(asubs_id == SYS_MODEM_AS_ID_1 || asubs_id == SYS_MODEM_AS_ID_2 || asubs_id == SYS_MODEM_AS_ID_3 )
  {
  cmpm_ptr()->policy_changes[asubs_id] &= (~change_value);
    //CM_MSG_HIGH_3("cmpmprx_reset_policy_chg_flag() on subs_id = %d change_value = %d policy_changes = %d",asubs_id,change_value,cmpm_ptr()->policy_changes[asubs_id]);
  }
}

/*===========================================================================

FUNCTION cmpmprx_reset_device_policy_chg_flag

DESCRIPTION
  Maintain the bitmask for what all device policy configuration items have changed
  since last policy was enforced.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                       cmpmprx_reset_device_policy_chg_flag(
       policy_change_mask mask
)
{
  cmpm_ptr()->device_policy_changes &= (~mask);
}

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
EXTERN boolean                    cmpmprx_is_mask_set(

       sys_modem_as_id_e_type     asubs_id,

       policy_change_mask         check_mask,

       cm_policy_config_s_type   *pm_ptr
)
{
  if( pm_ptr == NULL )
  {
    pm_ptr = cmpm_ptr();
  }

  if (( asubs_id > (int)pm_ptr->device_mode.nSims &&
        asubs_id != SYS_MODEM_AS_ID_MAX )
        ||
        asubs_id == SYS_MODEM_AS_ID_NO_CHANGE
        ||
        asubs_id == SYS_MODEM_AS_ID_NONE
     )
  {
    CM_MSG_HIGH_3("PMPRX MASK: Checking  0x%x on subs_id %d nSim %d - invalid asubs_id",
                     check_mask,asubs_id,pm_ptr->device_mode.nSims);
    return FALSE;
  }

  if ( asubs_id == SYS_MODEM_AS_ID_MAX )
  {
    uint8 i=0;
    for (i=0;i<pm_ptr->device_mode.nSims;i++)
    {
      if (pm_ptr->policy_changes[i] & check_mask)
      {
        return TRUE;
      }
    }
  }
  else
  {
    if(pm_ptr->policy_changes[asubs_id] & check_mask)
    {
      return TRUE;
    }
  }

  return FALSE;
}

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
EXTERN boolean                    cmpmprx_is_device_mask_set(

       policy_change_mask         check_mask,

       cm_policy_config_s_type   *pm_ptr
)
{
  if( pm_ptr == NULL )
  {
    pm_ptr = cmpm_ptr();
  }

  if(pm_ptr->device_policy_changes & check_mask)
  {
    return TRUE;
  }

  return FALSE;

}


/*===========================================================================

FUNCTION cmpmprx_print_current_policy

DESCRIPTION
  Print the value of the configuration items of the current policy.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_print_current_policy(sys_modem_as_id_e_type asubs_id)
{
  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  if( (asubs_id > SYS_MODEM_AS_ID_NONE) && 
     (asubs_id < SYS_MODEM_AS_ID_NO_CHANGE) &&
     (asubs_id < POLICY_ITEM_ARRAY_SIZE)
  )
  {

  CM_MSG_HIGH_5("PMPRX: MAX_ALLOWED = 0x%x, asubs_id = %d, sfmode = %d RATs supported = 0x%x, Service scan mode = %d",
                                  pm_ptr->current_policy[asubs_id].policy_max_mode_capability,
                                  asubs_id,
                                  pm_ptr->current_policy[asubs_id].sub_feature_mode,
                                  pm_ptr->current_policy[asubs_id].policy_mode_capability,
                                  pm_ptr->current_policy[asubs_id].service_scan_mode
                                  );


  CM_MSG_HIGH_3("PMPRX: UE mode = %d Voice_domain_pref = 0x%x, is_voice_domain_pref_locked 0x%x",
                                  pm_ptr->current_policy[asubs_id].ue_operation_mode,
                                  pm_ptr->current_policy[asubs_id].voice_domain_pref,
                                  pm_ptr->current_policy[asubs_id].is_voice_dom_pref_locked);

  CM_MSG_HIGH_4("PMPRX: BANDs(MSB)(LSB) GW = 0x%08x %08x, TDS = 0x%08x 0x%08x",
                          QWORD_HIGH(pm_ptr->current_policy[asubs_id].policy_band_capability),
                          QWORD_LOW(pm_ptr->current_policy[asubs_id].policy_band_capability),
                          QWORD_HIGH(pm_ptr->current_policy[asubs_id].policy_tds_band_capability) ,
                          QWORD_LOW(pm_ptr->current_policy[asubs_id].policy_tds_band_capability) );
  cm_print_lte_band_mask(pm_ptr->current_policy[asubs_id].policy_lte_band_capability);

  CM_MSG_HIGH_4("PMPRX: Change flags = %d, CM Eligibility to consume subsc = %d, asubs_id = %d, all mode = %d",
                                                       pm_ptr->policy_changes[asubs_id],
                                  cmpmprx_read_cm_policy_consumption_eligibility(asubs_id),
                                  asubs_id,pm_ptr->current_policy[asubs_id].call_mode);
  }
}

/*===========================================================================

FUNCTION cmpmprx_retrieve_policy_item_data

DESCRIPTION
  Initialize the Policy Manager object

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static policyman_status_t        cmpmprx_retrieve_policy_item_data
(
       cm_policy_config_s_type    *local_policy_ptr,

       policyman_item_collection_t const  *policy_item,

       policy_change_mask         *change_mask,

       policy_change_mask         *device_change_mask
)
{
  policyman_status_t  return_val = POLICYMAN_STATUS_ERR;
  sys_modem_as_id_e_type sub_id = SYS_MODEM_AS_ID_NONE;
  uint8 i=0, j=0;
  policyman_item_t const  *pItem;
  cmph_s_type                *ph_ptr = cmph_ptr();
  cm_policy_config_s_type    *pm_ptr = cmpm_ptr();

  CM_ASSERT( local_policy_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  #ifndef CM_DEBUG
  CM_ASSERT( policy_item != NULL );
  #endif

  CM_MSG_HIGH_1("POLICY_ITEM: RETRIEVE, numItems %d",
                 policy_item->numItems);
  
  /* Updating the policy sequence number */

  local_policy_ptr->seqno = policy_item->seqno;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i=0;i<policy_item->numItems;i++)
  {
    pItem = policy_item->pItems[i];
    switch (policyman_item_get_id_msim(pItem,&sub_id))
    {
     case POLICYMAN_ITEM_RAT_CAPABILITY:
       if(sub_id > SYS_MODEM_AS_ID_NONE && 
         sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
         sub_id < POLICY_ITEM_ARRAY_SIZE
       )
       {
         return_val = policyman_get_rat_capability( pItem,
                        &(local_policy_ptr->current_policy[sub_id].policy_mode_capability));

         if (POLICYMAN_SUCCEEDED(return_val) && 
              cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_RAT_CAPABILITY, sub_id))
         {
           change_mask[sub_id] |= PM_CHG_MODE_CAP;
           CM_MSG_HIGH_2("PM -> PMPRX: mode_cap_chgd: asubs_id=%d, mode_cap=%d",
                            sub_id,
                            local_policy_ptr->current_policy[sub_id].policy_mode_capability);
         }
       }
       
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_RF_BANDS:
       if(sub_id > SYS_MODEM_AS_ID_NONE && 
         sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
         sub_id < POLICY_ITEM_ARRAY_SIZE
       )
       {
         return_val = policyman_get_rf_bands(pItem,
                                           &(local_policy_ptr->current_policy[sub_id].policy_band_capability),
                                           &(local_policy_ptr->current_policy[sub_id].policy_lte_band_capability),
                                           &(local_policy_ptr->current_policy[sub_id].policy_tds_band_capability));

         if ( POLICYMAN_SUCCEEDED(return_val) && 
              cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_RF_BANDS, sub_id))
         {
           change_mask[sub_id] |= PM_CHG_BAND_CAP;
  
           if ( !POLICYMAN_SUCCEEDED(policyman_get_rf_bands_force( pItem,
                                                                 &(local_policy_ptr->is_force_registration)))
            )
           {
             local_policy_ptr->is_force_registration = FALSE;
           }
  
           CM_MSG_HIGH_2("PM -> PMPRX: band_cap_chgd: asubs_id=%d, is_force_registration=%d", 
                            sub_id, local_policy_ptr->is_force_registration);
         }
       }
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_SVC_MODE:
       if(sub_id > SYS_MODEM_AS_ID_NONE &&
         sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
         sub_id < POLICY_ITEM_ARRAY_SIZE
       )
       {
         return_val = policyman_get_svc_mode( pItem,
                                              &(local_policy_ptr->current_policy[sub_id].service_scan_mode));

         if ( POLICYMAN_SUCCEEDED(return_val) && 
              cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_SVC_MODE, sub_id))
         {
           change_mask[sub_id] |= PM_CHG_SVC_SCAN;
           CM_MSG_HIGH_2("PM -> PMPRX: srv_mode_chgd: asubs_id=%d, srv_mode=%d",
                            sub_id,
                            local_policy_ptr->current_policy[sub_id].service_scan_mode);
         }
       }
       break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_UE_MODE:
         if(sub_id > SYS_MODEM_AS_ID_NONE &&
           sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
           sub_id < POLICY_ITEM_ARRAY_SIZE
         )
         {
           return_val = policyman_get_ue_mode( pItem,
                                               &(local_policy_ptr->current_policy[sub_id].ue_operation_mode));

           if ( POLICYMAN_SUCCEEDED(return_val) && 
              cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_UE_MODE, sub_id))
           {
             change_mask[sub_id] |= PM_CHG_UE_MODE;
             CM_MSG_HIGH_2("PM -> PMPRX: ue_mode_chgd: asubs_id=%d, ue_mode=%d",
                              sub_id,
                              local_policy_ptr->current_policy[sub_id].ue_operation_mode);
           }
         }

		 break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
     case POLICYMAN_ITEM_DEVICE_CONFIGURATION:
     {
       size_t                new_nSims                = 0;
       size_t                new_nMaxActive           = 0;
       size_t                new_nMaxActiveData       = 0;
       uint32                max_mode_cap[MAX_AS_IDS];
       
       if(!POLICYMAN_SUCCEEDED(policyman_device_config_num_sims(pItem, &(new_nSims)))
         ||
		 !POLICYMAN_SUCCEEDED(policyman_device_config_max_active_data(pItem, &(new_nMaxActiveData)))
		 ||
         !POLICYMAN_SUCCEEDED(policyman_device_config_max_active(pItem, &(new_nMaxActive)))
        )
       {
         return_val = POLICYMAN_STATUS_ERR;
         CM_MSG_HIGH_0("MaxSIM numSIM nMaxActiveData read error");
       }
       else
       {
         return_val = POLICYMAN_STATUS_SUCCESS;
       }

       if (POLICYMAN_SUCCEEDED(return_val))
       {

         CM_MSG_HIGH_3("PM -> PMPRX: device_mode_chgd: nSIM %d nMaxActive %d nMaxActiveData %d",
                          new_nSims, new_nMaxActive, new_nMaxActiveData);

         if ( device_change_mask != NULL )
         {
           if ( local_policy_ptr->device_mode.nSims != new_nSims ||
                local_policy_ptr->device_mode.nMaxActive != new_nMaxActive )
           {
             /* If sub is deactivating and received device mode change do no enfore policy as
                       ** we expect device reset to handle such device mode changes. This check is
                       ** required only while processing deactivate_subs cmd.
                       */
             if (cmph_ptr()->mcfg_deactivate_subs && 
                ((cmph_ptr()->active_subs & cmph_ptr()->mcfg_deactivate_subs ) != SYS_MODEM_AS_ID_NO_ACTIVE_MASK))
             {
               CM_MSG_HIGH_1("Sub %d is deactivating do not enfore policy now",cmph_ptr()->mcfg_deactivate_subs);
               return POLICYMAN_STATUS_ERR;
             }

             *device_change_mask = PM_CHG_DEVICE_MODE;
           }

           if( local_policy_ptr->device_mode.nMaxActiveData != new_nMaxActiveData )
           {
             *device_change_mask |= PM_CHG_MAX_ACTIVE_DATA;
           }
         }
  
         local_policy_ptr->device_mode.nSims        = new_nSims;
         local_policy_ptr->device_mode.nMaxActive   = new_nMaxActive;
         local_policy_ptr->device_mode.nMaxActiveData   = new_nMaxActiveData;

         cmpmprx_configure_num_policies(local_policy_ptr,new_nSims);
       }

       CM_MSG_HIGH_2("MaxCap: RETRIEVE, active_subs %d, new_nSims %d",
                      cmpm_active_subs, new_nSims);

       if(cmpm_active_subs == SYS_MODEM_AS_ID_1_MASK || cmpm_active_subs == SYS_MODEM_AS_ID_2_MASK)
       {                 
         /* Read emergency capability from PM */
         if(!POLICYMAN_SUCCEEDED(policyman_get_e911_subphone_capability(pItem, new_nSims, max_mode_cap)))
         {
           return_val = POLICYMAN_STATUS_ERR;
           CM_MSG_HIGH_1("E911 MaxCap: RETRIEVE read error, active subs %d",cmpm_active_subs);
         }
         else
         {
           CM_MSG_HIGH_2("E911 MaxCap: RETRIEVE success,  nSims %d, size of max cap %d", 
             local_policy_ptr->device_mode.nSims, sizeof(max_mode_cap)/sizeof(uint32));

           if (local_policy_ptr->device_mode.nSims == 2 && (sizeof(max_mode_cap)/sizeof(uint32)) == 2)
           {
             CM_MSG_HIGH_2("E911 MaxCap: RETRIEVE success,  max_mode_cap 0x%x 0x%x",
                           max_mode_cap[0], max_mode_cap[1]);
           }
           if(return_val != POLICYMAN_STATUS_ERR)
           {
             return_val = POLICYMAN_STATUS_SUCCESS;
           }
         }
       }
       else
       {
       /* Read Device max capabilities */
       for(j=0;j < new_nSims; j++)
       {
         if(!POLICYMAN_SUCCEEDED(policyman_device_config_get_current_subphone_capability(pItem, 
              j, &(max_mode_cap[j]))))
         {
           return_val = POLICYMAN_STATUS_ERR;
           CM_MSG_HIGH_1("MaxCap read error sim %d",j);
         }
         else
         {
             if (local_policy_ptr->device_mode.nSims == 2 && sizeof(max_mode_cap)/sizeof(uint32) == 2)
             {
               CM_MSG_HIGH_2("MaxCap: RETRIEVE success,  max_mode_cap[%d] 0x%x",
                             j, max_mode_cap[j]);
             }
             
           if(return_val != POLICYMAN_STATUS_ERR)
           {
             return_val = POLICYMAN_STATUS_SUCCESS;
           }
           }
         }
       }

       for(j=0;j < new_nSims;j++)
       {
         CM_MSG_HIGH_2("MaxCap: sub %d is %d",j,max_mode_cap[j]);
         
         if(max_mode_cap[j] != local_policy_ptr->current_policy[j].policy_max_mode_capability)
         {
           local_policy_ptr->current_policy[j].policy_max_mode_capability = max_mode_cap[j];
           CM_MSG_HIGH_2("updating max mode cap sub %d to %d",j,max_mode_cap[j]);
              if ( device_change_mask != NULL )
              {
                *device_change_mask |= PM_CHG_MAX_MODE_CAP;
              }
           }
         }
       }
     break;
     /*- - - - - - - - - -- - - - - - - - - - - - - - - - - -*/

     case POLICYMAN_ITEM_VOICE_DOMAIN_PREF:
       if(sub_id > SYS_MODEM_AS_ID_NONE &&
         sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
         sub_id < POLICY_ITEM_ARRAY_SIZE
       )
       {
          return_val = policyman_get_voice_domain_pref(pItem,
                                                        &(local_policy_ptr->current_policy[sub_id].voice_domain_pref),
                                                        &(local_policy_ptr->current_policy[sub_id].is_voice_dom_pref_locked));
          if ( POLICYMAN_SUCCEEDED(return_val) && 
              cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_VOICE_DOMAIN_PREF, sub_id))
          {
            change_mask[sub_id] |= PM_CHG_VOICE_DOMAIN_PREF;
            CM_MSG_HIGH_3("PM -> PMPRX: voice_domain_pref: asubs_id=%d, voice_domain_pref=%d is_locked=%d",
                            sub_id,
                            local_policy_ptr->current_policy[sub_id].voice_domain_pref,
                            local_policy_ptr->current_policy[sub_id].is_voice_dom_pref_locked);
          }
       }
       break;
       case POLICYMAN_ITEM_CALL_MODE:
       if(sub_id > SYS_MODEM_AS_ID_NONE &&
         sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
         sub_id < POLICY_ITEM_ARRAY_SIZE
       )
       {		
         return_val = policyman_get_call_mode_from_item(pItem, &(local_policy_ptr->current_policy[sub_id].call_mode));       
        
         if(POLICYMAN_SUCCEEDED(return_val) && 
            cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_CALL_MODE, sub_id))
		{ 
			change_mask[sub_id] |= PM_CHG_CALL_MODE;
			/* If current value is different than the new one, send Volte Mode indication to Clients */       
            cmemg_send_call_mode_ind(sub_id, local_policy_ptr->current_policy[sub_id].call_mode, FALSE);		
		}
       }
      break;
     /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
	  case POLICYMAN_ITEM_RAT_ACQ_ORDER:          
      if(sub_id > SYS_MODEM_AS_ID_NONE &&
           sub_id < SYS_MODEM_AS_ID_NO_CHANGE &&
           sub_id < POLICY_ITEM_ARRAY_SIZE
         )
       {
        CM_MSG_HIGH_2("PM -> PMPRX: policy changes for sub 0:%x. PMPRX: policy changes for sub 1:%x",
                      pm_ptr->policy_changes[0],  pm_ptr->policy_changes[1]);

         return_val = policyman_get_rat_order(pItem,
                                           &(local_policy_ptr->current_policy[sub_id].acq_order_pref));
                                           
        if(POLICYMAN_SUCCEEDED(return_val) && 
            cmpmprx_is_item_value_changed(pm_ptr, local_policy_ptr, POLICYMAN_ITEM_RAT_ACQ_ORDER, sub_id))
         {
          change_mask[sub_id] |= PM_CHG_RAT_ACQ_ORDER;

          CM_MSG_HIGH_6("PM -> PMPRX: rat_acq_chgd : list elements=%d %d %d %d %d %d",
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[0],
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[1],
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[2],
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[3],
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[4],
                          local_policy_ptr->current_policy[sub_id].acq_order_pref.acq_order[5]);
         }
      }
     break;

     default:
       return_val = POLICYMAN_STATUS_SUCCESS;
    }
    CM_MSG_HIGH_3("Retrieve policy data pitem %d sub id %d change mask %x",pItem, sub_id, change_mask[sub_id]); 
  }

  CM_MSG_HIGH_3("PMPRX: retrieve policy item data returned %d, sfmode %d %d",return_val, \
    local_policy_ptr->current_policy[0].sub_feature_mode,\
    local_policy_ptr->current_policy[1].sub_feature_mode);
  cmpmprx_sanitize_max_and_curr_cap(local_policy_ptr);
  return return_val;
}

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
EXTERN boolean                    cmpmprx_fetch_mode_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_sys_mode_mask_e_type   *mode_capability_buffer
)
{
  if ( !cmpmprx_is_mask_set( asubs_id, PM_CHG_MODE_CAP, cmpm_ptr() )||
       mode_capability_buffer == NULL       ||
       !cmpmprx_read_cm_policy_consumption_eligibility(asubs_id)
     )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("PMPRX -> CM: Mode capability 0x%x, asubs_id %d, change mask: %d",
                 cmpm_ptr()->current_policy[asubs_id].policy_mode_capability, asubs_id, cmpm_ptr()->policy_changes[asubs_id]);

  *mode_capability_buffer = cmpm_ptr()->current_policy[asubs_id].policy_mode_capability;
  cmpmprx_reset_policy_chg_flag( asubs_id, PM_CHG_MODE_CAP );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_get_voice_domain_pref

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN boolean                    cmpmprx_get_voice_domain_pref(

       sys_modem_as_id_e_type       asubs_id,

       sys_voice_domain_pref_e_type *voice_domain_pref,

       boolean                      *is_voice_dom_pref_locked
)
{
  size_t sim_idx = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("PMPRX -> CM: voice domain pref 0x%x, asubs_id %d, is_locked %d",
                 cmpm_ptr()->current_policy[asubs_id].voice_domain_pref, asubs_id, 
                 cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked);

  *voice_domain_pref = cmpm_ptr()->current_policy[asubs_id].voice_domain_pref;
  *is_voice_dom_pref_locked = cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked;
  for (sim_idx = 0; sim_idx < cmpmprx_get_num_of_sims() && sim_idx<POLICY_ITEM_ARRAY_SIZE; sim_idx++)
  {
    cmpmprx_reset_policy_chg_flag( sim_idx, PM_CHG_VOICE_DOMAIN_PREF);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

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
EXTERN boolean                    cmpmprx_get_mode_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_sys_mode_mask_e_type   *mode_capability_buffer
)
{

   if(asubs_id == SYS_MODEM_AS_ID_1 || asubs_id == SYS_MODEM_AS_ID_2 || asubs_id == SYS_MODEM_AS_ID_3 )
   {
     *mode_capability_buffer = cmpm_ptr()->current_policy[asubs_id].policy_mode_capability;
   }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

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
EXTERN boolean                    cmpmprx_get_rat_acq_order_pref(

       sys_modem_as_id_e_type         asubs_id,

       cm_acq_pri_order_pref_s_type   *rat_acq_order_ptr
)
{
  cm_policy_config_s_type* pm_ptr = cmpm_ptr();
  uint8 i = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("PMPRX -> CM: rat_acq_order for asubs_id %d, num_rat %d",
                asubs_id, pm_ptr->current_policy[asubs_id].acq_order_pref.num_rat);

  /* Check if num_rats from PM is valid */
  if(pm_ptr->current_policy[asubs_id].acq_order_pref.num_rat > SYS_MAX_ACQ_PREF_RAT_LIST_NUM ||
     pm_ptr->current_policy[asubs_id].acq_order_pref.num_rat <= 0)
  {
    CM_MSG_HIGH_1("rat_acq_order: invalid num_rat %d, setting default CGWHLT",
                     pm_ptr->current_policy[asubs_id].acq_order_pref.num_rat);

    /* default rat_acq_order has 6 rats CGWHLT */
    rat_acq_order_ptr->num_rat = 6;

    rat_acq_order_ptr->acq_order[0] = SYS_SYS_MODE_CDMA;
    rat_acq_order_ptr->acq_order[1] = SYS_SYS_MODE_GSM;
    rat_acq_order_ptr->acq_order[2] = SYS_SYS_MODE_WCDMA;
    rat_acq_order_ptr->acq_order[3] = SYS_SYS_MODE_HDR;
    rat_acq_order_ptr->acq_order[4] = SYS_SYS_MODE_LTE;
    rat_acq_order_ptr->acq_order[5] = SYS_SYS_MODE_TDS;

    return FALSE;
  }
  else
  {
    /* num_rats is valid, so copy the data from PM */
    for(i = 0; i < pm_ptr->current_policy[asubs_id].acq_order_pref.num_rat; i++)
    {
      CM_MSG_HIGH_2("rat_acq_order[%d]: %d",
                    i, pm_ptr->current_policy[asubs_id].acq_order_pref.acq_order[i]);
    }
  
    *rat_acq_order_ptr = pm_ptr->current_policy[asubs_id].acq_order_pref;
  }

  for(i= 0; i < cmpmprx_get_num_of_sims() && i < POLICY_ITEM_ARRAY_SIZE; i++)
  {
    cmpmprx_reset_policy_chg_flag( i, PM_CHG_RAT_ACQ_ORDER);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}


/*===========================================================================

FUNCTION cmpmprx_fetch_max_mode_capability

DESCRIPTION
  Fetch the max mode capability and reset the max mode cap mask.

DEPENDENCIES
None

RETURN VALUE
  uint32

SIDE EFFECTS
  none

===========================================================================*/
uint32    cmpmprx_fetch_max_mode_capability(

       sys_modem_as_id_e_type     asubs_id
)
{
   if(asubs_id == SYS_MODEM_AS_ID_1 ||
      asubs_id == SYS_MODEM_AS_ID_2 || 
      asubs_id == SYS_MODEM_AS_ID_3
     )
   {
     cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(asubs_id);
     uint32 cmpm_policy_max_mode_cap = cmpm_ptr()->current_policy[asubs_id].policy_max_mode_capability;

     state_machine->stack_common_info.ue_max_mode_capability = cmpm_policy_max_mode_cap;
     cmpmprx_reset_device_policy_chg_flag(PM_CHG_MAX_MODE_CAP);
     return cmpm_policy_max_mode_cap;
   }
   else
   {
     return (uint32)cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);
   }
}

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
)
{
  cm_policy_config_s_type       *pm_ptr = cmpm_ptr();
  
  if ((asubs_id == SYS_MODEM_AS_ID_1 ||
       asubs_id == SYS_MODEM_AS_ID_2 || 
       asubs_id == SYS_MODEM_AS_ID_3) &&
      (asubs_id < (sys_modem_as_id_e_type)pm_ptr->device_mode.nSims))
      
      
  {
    *call_mode_capability_buffer = cmpm_ptr()->current_policy[asubs_id].call_mode;
  }
  else
  {
    *call_mode_capability_buffer = SYS_CALL_MODE_NORMAL;
  }
  
  CM_MSG_HIGH_1("PMPRX -> CM: read call mode %d",*call_mode_capability_buffer);
}

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
)
{
   if(asubs_id == SYS_MODEM_AS_ID_1 ||
      asubs_id == SYS_MODEM_AS_ID_2 || 
      asubs_id == SYS_MODEM_AS_ID_3
     )
   {
     return cmpm_ptr()->current_policy[asubs_id].policy_max_mode_capability;
   }
   else
   {
     return (uint32)cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);
   }
}

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
EXTERN boolean                    cmpmprx_get_band_capability(

       sys_modem_as_id_e_type     asubs_id,

       sys_band_mask_e_type       *band_capability_buffer,

       sys_lte_band_mask_e_type   *lte_band_capability_buffer,

       sys_band_mask_e_type       *tds_band_capability_buffer,

       boolean                    *force_registration
)
{
  cm_policy_items_s_type* curr_policy_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !cmpmprx_is_mask_set( asubs_id,PM_CHG_BAND_CAP, cmpm_ptr() )   ||
       band_capability_buffer     == NULL     ||
       lte_band_capability_buffer == NULL     ||
       tds_band_capability_buffer == NULL     ||
       force_registration         == NULL     ||
       !cmpmprx_read_cm_policy_consumption_eligibility(asubs_id)
     )
  {
    return FALSE;
  }

  curr_policy_ptr = &(cmpm_ptr()->current_policy[asubs_id]);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_4("PMPRX -> CM(MSB)(LSB): BANDs GW = 0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
                      QWORD_HIGH(cmpm_ptr()->current_policy[asubs_id].policy_band_capability),
                      QWORD_LOW(cmpm_ptr()->current_policy[asubs_id].policy_band_capability),
                      QWORD_HIGH(cmpm_ptr()->current_policy[asubs_id].policy_tds_band_capability),
                      QWORD_LOW(cmpm_ptr()->current_policy[asubs_id].policy_tds_band_capability) );
  cm_print_lte_band_mask(curr_policy_ptr->policy_lte_band_capability);

  CM_MSG_HIGH_1("PMPRX -> CM: force_registration = %d",
                      cmpm_ptr()->is_force_registration);

  *band_capability_buffer     = curr_policy_ptr->policy_band_capability;
  *lte_band_capability_buffer = curr_policy_ptr->policy_lte_band_capability;
  *tds_band_capability_buffer = curr_policy_ptr->policy_tds_band_capability;
  *force_registration         = cmpm_ptr()->is_force_registration;

  cmpmprx_reset_policy_chg_flag( asubs_id, PM_CHG_BAND_CAP );
  cmpm_ptr()->is_force_registration = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
}

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
EXTERN boolean cmpmprx_get_call_mode_capability(
       sys_modem_as_id_e_type     asubs_id,

       sys_call_mode_t   *call_mode_capability_buffer
)
{

  if(asubs_id == SYS_MODEM_AS_ID_1 || asubs_id == SYS_MODEM_AS_ID_2 || asubs_id == SYS_MODEM_AS_ID_3 )
  {
    if ( !cmpmprx_is_mask_set(asubs_id, PM_CHG_CALL_MODE, NULL)||
         call_mode_capability_buffer == NULL       ||
         !cmpmprx_read_cm_policy_consumption_eligibility(asubs_id)
       )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_2("PMPRX -> CM: Call Mode capability %d, subs_id %d",
                         cmpm_ptr()->current_policy[asubs_id].call_mode, asubs_id);

    *call_mode_capability_buffer = cmpm_ptr()->current_policy[asubs_id].call_mode;
    cmpmprx_reset_policy_chg_flag(asubs_id, PM_CHG_CALL_MODE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    return TRUE;
  }
  else
  {
    CM_MSG_HIGH_0("Invalid subs id passed to get call mode capability");
  }
  return FALSE;
}


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
)
{

  if(asubs_id == SYS_MODEM_AS_ID_1 || asubs_id == SYS_MODEM_AS_ID_2 || asubs_id == SYS_MODEM_AS_ID_3 )
  {
	  if ( call_mode_capability_buffer == NULL )
	  {
	    CM_MSG_MED_0("PMPRX -> CM: NO call mode capability update NULL Buffer");
	    return FALSE;
	  }

	  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	  CM_MSG_HIGH_2("PMPRX -> CM: Call Mode capability %d, subs_id %d",
	                       cmpm_ptr()->current_policy[asubs_id].call_mode, asubs_id);

	  *call_mode_capability_buffer = cmpm_ptr()->current_policy[asubs_id].call_mode;
      cmpmprx_reset_policy_chg_flag(asubs_id, PM_CHG_CALL_MODE );
	  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	  return TRUE;
  }
  else
  {
    CM_MSG_HIGH_0("Invalid subs id passed to fetch call mode capability");
  }
  return FALSE;
}



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
EXTERN boolean cmpmprx_is_special_volte_e911_calls(void)
{
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("PMPRX: special_volte_e911_calls %d",cmpm_ptr()->special_volte_e911_calls);

  return (cmpm_ptr()->special_volte_e911_calls == SYS_SPECIAL_VOLTE_E911_CALLS);
}


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
boolean cmpmprx_is_ok_to_enforce_policy(

       cm_policy_config_s_type   *policy_ptr,

       sys_modem_as_id_e_type     asubs_id

)
{
  uint8 i=0;

  if(policy_ptr == NULL)
  {
    policy_ptr = cmpm_ptr();
  }

  CM_ASSERT(policy_ptr != NULL);

  /* If Subscription not available
  ** or operating mode not online
  ** or no policy change (not expected)
  */
  if ( ((!cmpmprx_read_cm_policy_consumption_eligibility(asubs_id)||
       !cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, policy_ptr ))
       && policy_ptr->device_policy_changes == 0) ||
       cmclnup_is_ongoing_subsc(BM(asubs_id)))
  {
    CM_MSG_HIGH_0( "Policy Not forced, due to Subsc not avail or No policy changed");
    return FALSE;
  }

  /* Check the UE Operation Mode. If the UE is OFFLINE,
  ** return FALSE. Mode OFFLINE_CDMA and OFFLINE_AMPS need
  ** to be checked here for Legecy compatibility because
  ** QXDM sends down OFFLINE as OFFLINE_CDMA */
  if( cmph_ptr()->oprt_mode == SYS_OPRT_MODE_OFFLINE ||
      cmph_ptr()->oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA ||
      cmph_ptr()->oprt_mode == SYS_OPRT_MODE_OFFLINE_AMPS )
  {
    CM_MSG_HIGH_0( "UE is OFFLINE. Cannot Enforce Policy");
    return FALSE;
  }

  /* Evaluate individual policy items.*/
  /* UE mode first as it may cause change in number of active stacks. */
  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_UE_MODE, policy_ptr ) )
  {
    if ( cmtask_orig_para_is_obj_on_any_q(CM_ACT_TYPE_EMERG_CALL)||
         cmph_is_in_emergency_cb() )
    {
      CM_MSG_HIGH_0("Policy Not forced, due to Emerg call or UE in ECBM ");
      return FALSE;
    }

    /* Voice call in traffic on either sub. */
    for (i=0;i<cmpm_ptr()->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
    {
      #ifdef FEATURE_MMODE_SC_SGLTE
      if(cmph_is_subs_feature_mode_sglte(asubs_id)
          && i != asubs_id)
      {
        /*For SGLTE still consider only calls on current sub*/
        continue;
      }
      #endif
      if ( cmcall_find_active_voice_call_1(i) != CM_CALL_ID_INVALID )
      {
        CM_MSG_HIGH_1("Policy Not forced, due to active voice Call on sub_idx=%d", i);
        return FALSE;
      }
    }

    /* If service scan mode is changed, active voice calls wont matter as they
    ** will be put below PM object.
    */
    if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_SVC_SCAN, policy_ptr ) )
    {
      CM_MSG_HIGH_0("Handling for PM_CHG_SVC_SCAN ");
      return TRUE;
    }

    /* No Voice calls in traffic or emergency calls. End all the others and
    ** proceed with the UE mode change as per policy.
    */
    cmcall_clean_voice_call_from_q();

	
    /* No SMS calls in traffic. End all the others and
    ** proceed with the UE mode change as per policy.
    */
    cmcall_clean_sms_call_from_q();

    /* No CS Data calls in traffic. End all the others and
    ** proceed with the UE mode change as per policy.
    */
    cmcall_clean_cs_data_call_from_q();

    return TRUE;
  }

  /* Service scan mode changed, UE mode not changed. */
  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_SVC_SCAN, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_MODE_CAP, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_BAND_CAP, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_VOICE_DOMAIN_PREF, policy_ptr ) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_CALL_MODE, policy_ptr) )
  {
    return TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_RAT_ACQ_ORDER, policy_ptr) )
  {
    return TRUE;
  }

  if(policy_ptr->device_policy_changes != 0)
  {
    return TRUE;
  }

  CM_MSG_HIGH_0("Policy Not forced, No conditions meets..");
  return FALSE; /* Code execution should not come here. */
}

/*===========================================================================

FUNCTION cmpmprx_enforce_current_policy

DESCRIPTION
  Process the policy currently in cmpm_local.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void         cmpmprx_enforce_current_policy(

       sys_modem_as_id_e_type     asubs_id

   )
{
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  cmph_s_type *ph_ptr = cmph_ptr();
  boolean ue_mode_chgd = FALSE;
  boolean rat_acq_order_chgd = FALSE;
  cm_act_update_reas_e_type  act_update_reas = CM_ACT_UPDATE_REAS_USER;
  boolean force_top_pref = FALSE;


  CM_MSG_HIGH_2("cmpmprx_enforce_current_policy asubs_id = %d, ss = %d",asubs_id, cmph_map_subs_stack_to_ss(asubs_id,0));
	  
  cmpmprx_print_current_policy(asubs_id);

  if ( !cmpmprx_is_ok_to_enforce_policy( pm_ptr, asubs_id ) )
  {
    return;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_RAT_ACQ_ORDER, pm_ptr )
	 )
  {
    CM_MSG_HIGH_3("PM->PMPRX :updating phone object:%d, policy_chgd %x %x", 
                   asubs_id, pm_ptr->policy_changes[0], pm_ptr->policy_changes[1]);
    cmph_get_sub_config_ptr(asubs_id)->rat_acq_order_pref
      = pm_ptr->current_policy[asubs_id].acq_order_pref;
    //cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_RAT_ACQ_ORDER);
    rat_acq_order_chgd = TRUE;
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_MODE_CAP, pm_ptr ) )
  {
    CM_MSG_HIGH_1("PMPRX: mode_cap changed on sub:%d", asubs_id);
    cmph_sp_subs_info_update();
  }

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if(cmph_is_subs_feature_mode_srlte(asubs_id) &&
     cmpmprx_is_mask_set(asubs_id, PM_CHG_UE_MODE, pm_ptr))
  {
    cmsoa_srlte_ue_mode_update(pm_ptr->current_policy[asubs_id].ue_operation_mode);
    ue_mode_chgd = TRUE;
  }
  #endif

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_VOICE_DOMAIN_PREF, cmpm_ptr() ) )
  {
    cmpmprx_enforce_voice_domain_pref(asubs_id);
  }

  if ( cmpmprx_is_mask_set( asubs_id, PM_CHG_SVC_SCAN, pm_ptr ) )
  {
    cmpmprx_enforce_service_scan_mode(asubs_id);
    cmph_sp_subs_info_update();
  }
  else if(cmph_map_subs_stack_to_ss(asubs_id,0) == CM_SS_MAIN &&
         ( cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, cmpm_ptr()) ||
           rat_acq_order_chgd ))
  {

    /* If forcing is to be done on MAIN/HYBR_1/HYBR_2 combined */

    cm_orig_q_s_type  *main_stack_top = cmtask_orig_para_get_top(CM_SS_MAIN);
    if((main_stack_top != NULL) && 
       (main_stack_top->act_type == CM_ACT_TYPE_PH_OBJ ||
        main_stack_top->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT) )
    {
      force_top_pref = TRUE;
  	  act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
    }
      (void) cmph_force_pref_on_the_fly( CM_SS_MAX,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->main_stack_info.pref_info.pref_term,
                                         ph_ptr->main_stack_info.pref_info.mode_pref,
                                         ph_ptr->main_stack_info.pref_info.band_pref,
                                         ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->main_stack_info.pref_info.prl_pref,
                                         ph_ptr->main_stack_info.pref_info.roam_pref,
                                         ph_ptr->main_stack_info.pref_info.hybr_pref,
                                         ph_ptr->main_stack_info.pref_info.plmn,
                                         ph_ptr->main_stack_info.pref_info.srv_domain_pref,
                                         CM_OTASP_ACT_CODE_NONE,
                                         ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                         ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                         (cm_act_id_type)ph_ptr,
                                         act_update_reas,
                                         force_top_pref,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_MAX),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->main_stack_info.pref_info.csg_id,
                                         ph_ptr->main_stack_info.pref_info.csg_rat);
   
  }
  
  #ifdef FEATURE_MMODE_DUAL_SIM
      if (!cmph_is_sxlte() && cmph_is_msim() && (cmph_map_subs_stack_to_ss(asubs_id,0) == CM_SS_HYBR_2) &&
          ( cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, cmpm_ptr()) ||
            rat_acq_order_chgd ))
      {
        cm_orig_q_s_type  *hybr2_stack_top = cmtask_orig_para_get_top(CM_SS_HYBR_2);
        CM_MSG_HIGH_0("DUAL_SIM, PREF forced on HYBR2 ");   
	  
	if( (hybr2_stack_top != NULL) &&
        (hybr2_stack_top->act_type == CM_ACT_TYPE_PH_OBJ ||
        hybr2_stack_top->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT) )
        {
         force_top_pref = TRUE;
   	 act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
        }
	 else
	 {
	   force_top_pref = FALSE;
   	   act_update_reas = CM_ACT_UPDATE_REAS_USER;
	 }

               (void)cmph_force_pref_on_the_fly_ss(CM_SS_HYBR_2,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->hybr_2_stack_info.pref_info.network_sel_mode_pref,
                                         &(CMPH_HYBR_2_PREF(ph_ptr)),
                                         #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
                                         &(CMPH_HYBR_3_PREF(ph_ptr)),
                                         #endif
                                         CM_OTASP_ACT_CODE_NONE,
                                         (cm_act_id_type)ph_ptr,
                                         act_update_reas,
                                         force_top_pref,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_2),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_rat);	
	  }
 #endif	  
 
 #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
      if ((cmph_is_msim() || cmph_is_sxlte()) && (cmph_map_subs_stack_to_ss(asubs_id,0) == CM_SS_HYBR_3) &&
          ( cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, cmpm_ptr()) ||
            rat_acq_order_chgd ))
	  {
 	    cm_orig_q_s_type  *hybr3_stack_top = cmtask_orig_para_get_top(CM_SS_HYBR_3);
            if( (hybr3_stack_top != NULL) &&
            (hybr3_stack_top->act_type == CM_ACT_TYPE_PH_OBJ ||
            hybr3_stack_top->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT) )
            {
              force_top_pref = TRUE;
   	      act_update_reas = CM_ACT_UPDATE_REAS_ACT_START;
            }
            else
            {
              force_top_pref = FALSE;
   	      act_update_reas = CM_ACT_UPDATE_REAS_USER;
            }

                (void)cmph_force_pref_on_the_fly_ss(CM_SS_HYBR_3,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_PH_OBJ,
                                         ph_ptr->hybr_3_stack_info.pref_info.network_sel_mode_pref,
                                         &(CMPH_HYBR_2_PREF(ph_ptr)),
                                         #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
                                         &(CMPH_HYBR_3_PREF(ph_ptr)),
                                         #endif
                                         CM_OTASP_ACT_CODE_NONE,
                                         (cm_act_id_type)ph_ptr,
                                         act_update_reas,
                                         force_top_pref,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_3),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_rat);

        CM_MSG_HIGH_0("TRIPLE_SIM or SXLTE_G , PREF forced on HYBR3 ");
										 
      }
  #endif

  /*Reset for rat _acq_order here itself*/
  cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_RAT_ACQ_ORDER);
										 
  #ifdef FEATURE_DOMAIN_SELECTION
  if(cmph_is_subs_feature_mode_srlte(asubs_id) &&
    ue_mode_chgd)
  {
    cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(asubs_id);
   
    if(state_machine->stack_common_info.ue_mode
                 == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
    {
      /* For SRLTE, when we receive ue mode change indication from PM with new 
      ** ue mode=1xsrlte_csfb_pref(CSFB->SRLTE_CSFB_PREF transition) and we 
      ** are already in stable 1x service, we should perform c2k switch from single to
      ** dual stack after ue mode change has been processed.
      ** It can happen in below scenario:
      ** 1.ue_mode=CSFB, and acquired home 1x service
      ** 2.Switch to dual stack wasn't done here was ue_mode=CSFB.
      ** 3.Indicated 1x service to PM.
      ** 4.PM sends ue mode change from CSFB to CSFB_PREF.
      ** 5.we should perform c2k switch from sinlge->dual stack here.
      ** */
      #if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
        cmsoa_check_and_switch_fromc2k_to_srlte(CM_SS_MAIN);
      #endif
	  
      if(cmsds_ptr()->pending_uemode_cmsds_skipped == TRUE)
      {
        sd_si_info_s_type     si_info;
        sd_si_info_ptr_get( SD_SS_MAIN, &si_info);
        CM_MSG_HIGH_2("resume dom sel srv %d mode %d",si_info.srv_status,si_info.mode);
        if(si_info.srv_status == SYS_SRV_STATUS_SRV && si_info.mode == SYS_SYS_MODE_LTE)
        {
          cmsds_process_srv_ind(CM_SS_MAIN, &si_info);
        }
      }    
    }
  }
  #endif 
  
}

/*===========================================================================

FUNCTION cmpmprx_update_curr_policy

DESCRIPTION
  Update cmpm_local with new policy retrieved from Policy Manager..

DEPENDENCIES
  cmpm_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                              cmpmprx_update_curr_policy(

       cm_policy_config_s_type    *local_policy

)
{
  if ( local_policy != NULL )
  {
    cm_policy_config_s_type *curr_policy = cmpm_ptr();

    *curr_policy = *local_policy;
  }
  return;
}


/*===========================================================================

FUNCTION cmpmprx_process_retrieved_policy

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_process_retrieved_policy(

       sys_modem_as_id_e_type     asubs_id,

       cm_policy_config_s_type    *local_policy,

       policy_change_mask         local_mask
)

{
  if ( local_policy != NULL )
  {
    CM_MSG_HIGH_3("cmpmprx_process_retrieved_policy() on subs_id = %d local_mask = %d policy_changes = %d",
                  asubs_id,local_mask,cmpm_ptr()->policy_changes[asubs_id]);
    local_policy->policy_changes[asubs_id] |= local_mask;
    cmpmprx_update_curr_policy(local_policy);
    cmpmprx_enforce_current_policy(asubs_id);
  }
}

/*===========================================================================

FUNCTION cmpmprx_process_msgr_msg

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean                    cmpmprx_process_msgr_msg(

       cm_policyman_cfg_s         *rpt_ptr,

       cm_policy_config_s_type    *local_policy,

       policy_change_mask         *device_local_mask_ptr
)
{
  int                     i                   = 0;
  msgr_attach_s           *pAttach            = NULL;
  boolean                 is_new_policy_valid = TRUE;
  policy_change_mask      local_mask[3]       = {0,0,0};
  policy_change_mask      device_local_mask   = 0;
  cm_policy_config_s_type *curr_policy        = cmpm_ptr();
  boolean                 is_any_policy_chg   = FALSE;
  boolean                 is_policy_seq_invalid = FALSE;
  size_t                  local_seqno = cmpm_ptr()->seqno;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_1("PROC MSGR: num_attach=%d",
                 msgr_get_num_attach(&(rpt_ptr->msg_hdr)));
  
  *device_local_mask_ptr = 0;

  for (i = 0 ; i < msgr_get_num_attach(&(rpt_ptr->msg_hdr)) ; ++i)
  {
    
    policyman_item_collection_t const *policy_item = NULL;

    pAttach = msgr_get_attach(&(rpt_ptr->msg_hdr), i);
    if (pAttach == NULL || pAttach->type != MSGR_ATTACH_REFCNT)
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Invalid attachment %d, Abort new policy",i);
      is_new_policy_valid = FALSE;
      break;
    }

    policy_item = pAttach->data.refcnt.refcnt_obj_ptr;

    if (policy_item->seqno <= curr_policy->seqno)
	{
	   CM_MSG_HIGH_2("PROC MSGR: current policy is already handled: policy_item->seqno= %d, curr_policy->seqno = %d",policy_item->seqno, curr_policy->seqno);
	   return FALSE;
	}

	if(policy_item->seqno != curr_policy->seqno + 1)
	{
	   is_policy_seq_invalid = TRUE;
      CM_MSG_HIGH_2("PROC MSGR: CM-PM policy seq nums are out of Sync policy_item->seqno= %d, curr_policy->seqno = %d",
                    policy_item->seqno, curr_policy->seqno);
	   break;
	}
    if(!POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( local_policy, policy_item, local_mask, &device_local_mask )))
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Unable to retrieve data for attachment %d",i);
	   /*As the sequence number would be also be valid in case retrieval policy fails*/
	  local_seqno = policy_item->seqno;
      is_new_policy_valid = FALSE;
      break;
    }
	local_seqno = policy_item->seqno;
  } /* for() */

  if(is_policy_seq_invalid)
  {
    policyman_item_collection_t const *new_policy_item = policyman_get_items_since_sequence_number(curr_policy->seqno);

    if (new_policy_item == NULL)
    {
      CM_MSG_HIGH_0("PROC MSGR: PMPRX: Invalid attachment %d, Abort new policy");
      is_new_policy_valid = FALSE;
    }
	 else if(!POLICYMAN_SUCCEEDED(cmpmprx_retrieve_policy_item_data( local_policy, new_policy_item, local_mask, &device_local_mask )))
    {
      CM_MSG_HIGH_1("PROC MSGR: PMPRX: Unable to retrieve data for attachment %d",i);
      is_new_policy_valid = FALSE;
    }

    if(new_policy_item != NULL)
	{
      /* update the sequence number both in case retrieval is success or failure
	  retrieve fail shall also update policy sequence number */
      local_seqno = new_policy_item->seqno;
	  policyman_item_collection_release(new_policy_item);
	}
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  is_any_policy_chg = (device_local_mask != 0);
  
  for (i=0;i<(int)local_policy->device_mode.nSims;i++)
  {
    if(i<3)
    {
      is_any_policy_chg = (is_any_policy_chg || (local_mask[i] != 0));
    }
    else
    {
      break;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !is_new_policy_valid                   ||
       !is_any_policy_chg                      ||
       (device_local_mask == 0 &&
        (cmpmprx_compare_policies(local_policy,curr_policy) == 0)))
  {
    CM_MSG_HIGH_3("PROC MSGR: PMPRX: Discard new policy valid = %d, is_any_policy_chg = %d device_local_mask %d",
                                            is_new_policy_valid,is_any_policy_chg,device_local_mask);
	
	cmpm_ptr()->seqno = local_seqno;
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Copy device level policy mask   */
  local_policy->device_policy_changes |= device_local_mask;
  *device_local_mask_ptr = device_local_mask;

  CM_MSG_HIGH_1("PM->CM: msgr msg, device policy changes: 0x%x",device_local_mask);

  /* Copy change mask for each SIM */
  for (i=0;i<(int)local_policy->device_mode.nSims;i++)
  {
     local_policy->policy_changes[i] |= local_mask[i];
     CM_MSG_HIGH_2("PROC MSGR: Msgr policy[%d] mask = %x",i,local_policy->policy_changes[i]); 
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
}

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
void cmpmprx_policy_chg_ind_proc(
    cm_policyman_cfg_s          *rpt_ptr
     /* Pointer to the report sent by Message Router */
   )
{
  cm_policy_config_s_type *local_policy = NULL;
  uint8 i=0;
  policy_change_mask device_local_policy = 0;
  boolean is_wt_rat_not_on_sub_2 = FALSE;
  cmph_s_type             *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0("MSGR RXD: policy_chg_ind");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Loop thru the subs_ids and see if there are any pending policy.
  ** If any policy item has changed, attempt to enforce the new policy first before processing new 
  ** policy.
  */
  if (cmpm_ptr()->device_policy_changes == 0)
  {
    for (i=0;i<cmpm_ptr()->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
    {
      if (cmpmprx_is_mask_set( i, PM_CHG_ALL, cmpm_ptr()))
      {
        cmpmprx_enforce_current_policy(i);
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( rpt_ptr == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! PM -> PMPRX: Incoming policy message NULL");
    return;
  }

  /* Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! PM -> PMPRX: unable to process policy as cannot allocate memory");
    return;
  }

  *local_policy = *(cmpm_ptr());
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  if (cmpmprx_process_msgr_msg(rpt_ptr,local_policy,&device_local_policy))
  {

    uint32 prev_sub1_mode_cap = cmpm_ptr()->current_policy[0].policy_mode_capability;

    #ifdef FEATURE_CM_DEBUG_BUFFER
    cmdbg_add_to_dbg_buffer(CM_BUFF_MSG_RTR_TYPE, CM_DBG_BUFF_MSGR_RTR_PM_CFG_UPDATE_IND, (void*)local_policy);
    #endif
    cmpmprx_update_curr_policy(local_policy);
  
    CM_MSG_HIGH_3("device local policy 0x%x SUB1 policy 0x%x SUB2 policy 0x%x",device_local_policy,
      local_policy->policy_changes[0],local_policy->policy_changes[1]);

    if(device_local_policy & PM_CHG_MAX_ACTIVE_DATA)
    {
      cmpmprx_update_flags_ims_for_nondds(cmph_ptr()->default_data_subs);
    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Enforce device level policy */
  if(cmpm_ptr()->device_policy_changes != 0)
  {
    cmpmprx_enforce_device_policy(PM_CHG_DEVICE_ALL);
  }

  /* When this condition is false, we are currently processing the policy change
    ** indication from PM associated with a subscription change. PM has received the UIM
    ** event before CM, so we are processing that before processing the UIM event
    ** ourselves. So in this case, skip enforcing the new policy as it will be
    ** enforced next when CM receives the UIM event subscription change.
    */
    if(!((!cmpmprx_is_ok_to_enforce_policy(cmpm_ptr(), SYS_MODEM_AS_ID_1) && 
           (prev_sub1_mode_cap & SYS_SYS_MODE_MASK_HDR))
           &&
          (cmpmprx_is_ok_to_enforce_policy(cmpm_ptr(), SYS_MODEM_AS_ID_2) && 
        ((cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_WCDMA) ||
          (cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_TDS))) 
      )) 
    {

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Loop thru the subs_ids and see if any policy changed. If yes then enforce*/
  /* If any policy item has changed, attempt to enforce the new policy. */
  // Enforce current policy for non W rat SUB first followed by remaining subs
    if(cmpm_ptr()->device_mode.nSims > 1 && 
      !(cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_WCDMA)&&
      !(cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_TDS))
    {
      is_wt_rat_not_on_sub_2 = TRUE;
      
      if( cmpmprx_is_mask_set( SYS_MODEM_AS_ID_2, PM_CHG_ALL, cmpm_ptr() ) )
      {
        CM_MSG_HIGH_1("PMPRX: PM is_wt_rat_not_on_sub_2, enforcing policy on sub2 with mask =0x%x",cmpm_ptr()->policy_changes[SYS_MODEM_AS_ID_2]);
        cmpmprx_enforce_current_policy(SYS_MODEM_AS_ID_2);
      }
    }
  
    for (i=0;i<cmpm_ptr()->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
    {
      if(is_wt_rat_not_on_sub_2 && (i == SYS_MODEM_AS_ID_2))
      {
        continue;
      }
      CM_MSG_HIGH_2("PMPRX: policy_chgd for sub: Due to call end, mask=0x%x, sim_idx=%d", 
                    cmpm_ptr()->policy_changes[i], i);
      if( cmpmprx_is_mask_set( i, PM_CHG_ALL, cmpm_ptr() ) )
      {
       cmpmprx_enforce_current_policy(i);
      }
    }
  }
  else
  {
      CM_MSG_HIGH_0("HDR found in primary sub and W/T found in secondary sub");
  }
  }
  else
  {
    CM_MSG_HIGH_0("PMPRX<< PM: MSGR return false");
  }
  cm_mem_free(local_policy);
  
  policyman_msgr_attachments_release(&(rpt_ptr->msg_hdr));

  return;
}

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
void   cmpmprx_update_cm_policy_consumption_eligibility_subsc(

       sys_modem_as_id_e_type     asubs_id,

       cm_policy_subsc_change_e_type   gwl_subsc,

       cm_policy_subsc_change_e_type   cdma_subsc
)
{
  boolean prev_eligibility = cmpmprx_read_cm_policy_consumption_eligibility(asubs_id);
  cm_policy_config_s_type *curr_policy = cmpm_ptr();

  if(gwl_subsc == CMPMPRX_SUBSC_AVAIL)
  {
    curr_policy->is_cm_eligible[asubs_id].gwl_subsc = TRUE;
  }
  else if(gwl_subsc == CMPMPRX_SUBSC_NOT_AVAIL)
  {
    curr_policy->is_cm_eligible[asubs_id].gwl_subsc = FALSE;
  }

  if(cdma_subsc == CMPMPRX_SUBSC_AVAIL)
  {
    curr_policy->is_cm_eligible[asubs_id].cdma_subsc = TRUE;
  }
  else if(cdma_subsc == CMPMPRX_SUBSC_NOT_AVAIL)
  {
    curr_policy->is_cm_eligible[asubs_id].cdma_subsc = FALSE;
  }

  CM_MSG_HIGH_2("PMPRX: Updating CM eligibility to consume policy: cdma subsc: %d, gwl subsc:%d",
        curr_policy->is_cm_eligible[asubs_id].cdma_subsc, curr_policy->is_cm_eligible[asubs_id].gwl_subsc);

  if ( cmpmprx_read_cm_policy_consumption_eligibility(asubs_id) &&
       cmpmprx_is_mask_set( asubs_id, PM_CHG_RAT_ACQ_ORDER, cmpm_ptr() )
	 )
  {
    CM_MSG_HIGH_1("CM_DEBUG PMPRX: rat acq changed on sub:%d", asubs_id);
    cmph_get_sub_config_ptr(asubs_id)->rat_acq_order_pref
      = curr_policy->current_policy[asubs_id].acq_order_pref;
	cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_RAT_ACQ_ORDER);
  }

  /* There are pending policy changes and CM just became eligible to consume
  ** Enforce service scan. All others will be read by subscription change.
  */
  if ( !prev_eligibility &&
       cmpmprx_read_cm_policy_consumption_eligibility(asubs_id) &&
       cmpmprx_is_mask_set( asubs_id, PM_CHG_SVC_SCAN, cmpm_ptr() ) )
  {
    cmpmprx_enforce_service_scan_mode(asubs_id);
  }

}

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
EXTERN boolean                    cmpmprx_enforce_policy_oprt_mode(

       sys_modem_as_id_e_type     asubs_id
)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  uint8 i=0;
  boolean is_no_op = TRUE;
  /*
  **Loop thru all the subs ids and check for all.
  */
  if (asubs_id == SYS_MODEM_AS_ID_MAX)
  {
    for (i=0;i<curr_policy->device_mode.nSims;i++)
    {
      if ( curr_policy->current_policy[i].service_scan_mode != POLICYMAN_SVC_MODE_FULL )
      {
        cmpmprx_set_policy_chg_flag(i,PM_CHG_SVC_SCAN);
        is_no_op = FALSE;

        /* There are pending policy changes and CM just became eligible to consume*/
        if ( cmpmprx_read_cm_policy_consumption_eligibility(i) &&
             cmpmprx_is_mask_set( i, PM_CHG_ALL, cmpm_ptr() ) )
  {
          cmpmprx_enforce_current_policy(i);
        }
      }
    }
  }
  else
  {
    if ( curr_policy->current_policy[asubs_id].service_scan_mode != POLICYMAN_SVC_MODE_FULL )
    {
      cmpmprx_set_policy_chg_flag(asubs_id,PM_CHG_SVC_SCAN);
      is_no_op = FALSE;

  /* There are pending policy changes and CM just became eligible to consume*/
      if ( cmpmprx_read_cm_policy_consumption_eligibility(asubs_id) &&
           cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, cmpm_ptr() ) )
  {
        cmpmprx_enforce_current_policy(asubs_id);
      }
    }
  }

  return (!is_no_op);
}

/*===========================================================================

FUNCTION cmpmprx_notify_call_end - remove_feature

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_notify_call_end(

       sys_modem_as_id_e_type     asubs_id,

       boolean is_emerg
)
{
  uint8 i=0;
  boolean is_wt_rat_not_on_sub_2 = FALSE;
  
  CM_MSG_HIGH_2("PMPRX: resume PM as_id %d, is_em %d", asubs_id, is_emerg);
  if ( is_emerg == TRUE && cmpm_ptr()->is_pm_suspend)
  {
    policyman_resume();
    cmpm_ptr()->is_pm_suspend = FALSE;
  }

  if(cmpm_ptr()->device_policy_changes & PM_CHG_MAX_MODE_CAP)
  {
    return;
  }
  // Enforce current policy for non W rat SUB first followed by remaining subs
  if(cmpm_ptr()->device_mode.nSims > 1 && 
    !(cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_WCDMA)&&
    !(cmpm_ptr()->current_policy[1].policy_mode_capability & SYS_SYS_MODE_MASK_TDS))
  {
    is_wt_rat_not_on_sub_2 = TRUE;
    if( cmpmprx_is_mask_set( SYS_MODEM_AS_ID_2, PM_CHG_ALL, cmpm_ptr() ) )
    {
      CM_MSG_HIGH_0("PMPRX: PM is_w_rat_not_on_sub_2, enforcing policy on sub2");
      cmpmprx_enforce_current_policy(SYS_MODEM_AS_ID_2);
    }
  }

  for (i=0;i<cmpm_ptr()->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
  {
    if(is_wt_rat_not_on_sub_2 && (i == SYS_MODEM_AS_ID_2))
    {
      continue;
    }
    CM_MSG_HIGH_2("PMPRX: policy_chgd for sub: Due to call end, mask=0x%x, sim_idx=%d", 
                    cmpm_ptr()->policy_changes[i], i);
    if( cmpmprx_is_mask_set( i, PM_CHG_ALL, cmpm_ptr() ) )
    {
      cmpmprx_enforce_current_policy(i);
    }
  }
}

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
void  cmpmprx_notify_emg_call_start()
{
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  
  /* Suspend Policy Manager so it will not process any rules for the duration
  ** of the emergency call. 
  */
  if (!pm_ptr->is_pm_suspend)
  {
    CM_MSG_HIGH_0("PMPRX: suspend PM");
    policyman_suspend();
    pm_ptr->is_pm_suspend = TRUE;
  }
  else
  {
    CM_MSG_HIGH_0("PMPRX: PM already in suspend");
  }

}

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
)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();

  //Size of array is 3 , For as_id 1, 2, 3
  if( (asubs_id > SYS_MODEM_AS_ID_NONE) && (asubs_id < SYS_MODEM_AS_ID_NO_CHANGE) )
  {
    return (curr_policy->is_cm_eligible[asubs_id].gwl_subsc ||
           curr_policy->is_cm_eligible[asubs_id].cdma_subsc );
  }
  else
  {
    return FALSE ;
  }
}

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

       sys_modem_as_id_e_type       asubs_id,

   cm_network_sel_mode_pref_e_type  net_sel_mode_pref
)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  cm_network_sel_mode_pref_e_type  pm_net_sel_mode_pref = net_sel_mode_pref;

  switch (curr_policy->current_policy[asubs_id].service_scan_mode)
  {
     case POLICYMAN_SVC_MODE_LIMITED:
       pm_net_sel_mode_pref =  CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV ;
     break;

     case POLICYMAN_SVC_MODE_CAMP_ONLY:
     case POLICYMAN_SVC_MODE_ALL:
       {
         cmph_s_type *ph_ptr = cmph_ptr();
         cmph_stack_info_s_type *stack_info;
         #if defined(FEATURE_MMODE_SXLTE_G)|| defined(FEATURE_MMODE_TRIPLE_SIM)
         if(ph_ptr->hybr_3_stack_info.asubs_id == asubs_id)
         {
           stack_info = &(ph_ptr->hybr_3_stack_info);
         }
         else 
         #endif
         if(ph_ptr->hybr_2_stack_info.asubs_id == asubs_id && !cmph_is_sxlte())
         {
           stack_info = &(ph_ptr->hybr_2_stack_info);
         }
         else
         {
           stack_info = &(ph_ptr->main_stack_info);
         }

         if (stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_CAMP_ONLY ||
           stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_LIMITED_SRV ||
           stack_info->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
         {
           pm_net_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_MANUAL_CAMP_ONLY;
         }
         else
         {
           pm_net_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTO_CAMP_ONLY;
         }

       }
       break;
     case POLICYMAN_SVC_MODE_FULL:
     default:
       break;
  } /* switch */

  if(net_sel_mode_pref != pm_net_sel_mode_pref)
  {
    CM_MSG_HIGH_3("cmpmprx_compute_restricted_mode: asubs_id %d, net_sel_mode_pref %d, srv_scan:%d",
                         asubs_id, net_sel_mode_pref, curr_policy->current_policy[asubs_id].service_scan_mode);
  }
  return pm_net_sel_mode_pref;
}

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
EXTERN void                       cmpmprx_enforce_service_scan_mode(

       sys_modem_as_id_e_type     asubs_id
)
{
  cm_policy_config_s_type *curr_policy = cmpm_ptr();
  cmph_s_type             *ph_ptr = cmph_ptr();
  cm_network_sel_mode_pref_e_type rest_net_sel_mode_pref = ph_ptr->main_stack_info.pref_info.network_sel_mode_pref;

  if ( !cmpmprx_is_mask_set( asubs_id, PM_CHG_SVC_SCAN, cmpm_ptr() ) )
  {
    return;
  }

  cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_SVC_SCAN);

  if ( curr_policy->current_policy[asubs_id].service_scan_mode == POLICYMAN_SVC_MODE_FULL)
  {
    int                   stack_id = 0;

    for ( stack_id = 0; stack_id < MAX_STACK_PER_SUB; stack_id++ )
    {
      cm_ss_e_type ss = CM_SS_NONE ;
      #if defined(FEATURE_MMODE_TRIPLE_SIM)
      if( asubs_id == SYS_MODEM_AS_ID_3 )
      {
          ss = ph_ptr->subs_stack_map[asubs_id][stack_id];
      }
      else
      #endif
      if( (asubs_id == SYS_MODEM_AS_ID_1) || (asubs_id == SYS_MODEM_AS_ID_2))
      {
        
          ss = ph_ptr->subs_stack_map[asubs_id][stack_id];
      }
      else
      {
        ss = CM_SS_NONE ;   
      }
      CM_MSG_HIGH_1("getting, ss=%d", ss);
      if(ss != CM_SS_NONE)
      {
        cm_orig_q_s_type *pm_rest_obj_ptr = cmtask_orig_para_search_act_type( ss,
                                                            CM_ACT_TYPE_POLICYMAN_RESTRICT);
        if( pm_rest_obj_ptr != NULL )
        {
          cm_orig_q_s_type *ph_obj_ptr = cmtask_orig_para_search_act_type( ss,
                                                              CM_ACT_TYPE_PH_OBJ);
          cm_pref_s_type pref_info = CMPH_SS_PREF_PTR(asubs_id,ph_ptr);

          uint16 sys_sel_pref_req_id = (ph_obj_ptr == NULL)? CM_DEFAULT_VALUE:
                                        ph_obj_ptr->orig->sys_sel_pref_req_id;
          CM_MSG_HIGH_2("Exit full_rat_search mode, asubs_id=%d, ss=%d", asubs_id, ss);

          /*Update PHONE OBJ if CDMA_HDR is not supported, 
                    else retain HICPS preference if already enforced*/

          CM_MSG_HIGH_2("Exit full_rat mode,pm mode_cap=%d,band_cap_sup_mode=%d", curr_policy->current_policy[asubs_id].policy_mode_capability, 
                         (int)(sd_ss_get_supp_mode_pref(
                   curr_policy->current_policy[asubs_id].policy_mode_capability,
                   curr_policy->current_policy[asubs_id].policy_band_capability,
                   curr_policy->current_policy[asubs_id].policy_lte_band_capability,
                   curr_policy->current_policy[asubs_id].policy_tds_band_capability)
                   & SD_SS_MODE_PREF_CDMA_HDR));


          if(! ( SD_MODE_CONTAIN( curr_policy->current_policy[asubs_id].policy_mode_capability, SD_SS_MODE_PREF_CDMA_HDR) &&
                 ((int)(sd_ss_get_supp_mode_pref(
                   curr_policy->current_policy[asubs_id].policy_mode_capability,
                   curr_policy->current_policy[asubs_id].policy_band_capability,
                   curr_policy->current_policy[asubs_id].policy_lte_band_capability,
                   curr_policy->current_policy[asubs_id].policy_tds_band_capability)
                   & SD_SS_MODE_PREF_CDMA_HDR) != 0) ) )
          {

          (void) cmph_force_pref_on_the_fly(ss,
                                            ph_ptr,
                                            SD_SS_PREF_REAS_USER,
                                            CM_ACT_TYPE_PH_OBJ,
                                            pref_info.pref_term,
                                            pref_info.mode_pref,
                                            pref_info.band_pref,
                                            pref_info.lte_band_pref,
                                            pref_info.tds_band_pref,
                                            pref_info.prl_pref,
                                            pref_info.roam_pref,
                                            pref_info.hybr_pref,
                                            pref_info.plmn,
                                            pref_info.srv_domain_pref,
                                            CM_OTASP_ACT_CODE_NONE,
                                            pref_info.acq_order_pref,
                                            pref_info.network_sel_mode_pref,
                                            (cm_act_id_type) ph_ptr,
                                            CM_ACT_UPDATE_REAS_ACT_CONT,
                                            FALSE,
                                            cmph_get_acq_pri_order_per_sub(ss),
                                            sys_sel_pref_req_id,
                                            pref_info.csg_id,
                                            pref_info.csg_rat);
           }
           cmph_delete_update_orig_mode( ss,
                                        (cm_act_id_type) curr_policy,
                                        TRUE );

        }
      }
    }
  }
  else
  {
    rest_net_sel_mode_pref = cmpmprx_compute_restricted_mode( asubs_id, ph_ptr->main_stack_info.pref_info.network_sel_mode_pref );

    if ( cmpmprx_read_cm_policy_consumption_eligibility(asubs_id) )
    {
      cm_ss_e_type ss = cmph_map_subs_stack_to_ss(asubs_id,0);
      cm_orig_q_s_type  *ph_obj_ptr  = cmtask_orig_para_search_act_type(
                                            ((ss==CM_SS_MAX)?CM_SS_MAIN:ss),
                                                          CM_ACT_TYPE_PH_OBJ);
      cm_mode_pref_e_type ph_obj_mode_pref;

      if (cmph_is_subs_feature_mode_sglte(asubs_id) || 
          cmph_is_subs_feature_mode_1x_sxlte(asubs_id))
      {
        ss = CM_SS_MAX;
      }

      /* If HICPS has removed the 3GPP and Hybrid HDR acquisitions are ongoing
      ** we dont want to interrupt those. So instead of ph_ptr mode preference
      ** force the phone object mode preference.
      */
      if ( ph_obj_ptr != NULL )
      {
        ph_obj_mode_pref = ph_obj_ptr->orig->orig_mode_pref;
      }
      else
      {
        ph_obj_mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);
      }

      CM_MSG_HIGH_2("Enter full_rat_search mode, asubs_id=%d, mode_pref=%d", asubs_id,ph_obj_mode_pref);
      
      /* Since full rat mode does only limited acq, forcing CS only as domain pref*/   
	  if(ss == CM_SS_MAIN || ss == CM_SS_MAX)
      {
	    (void) cmph_force_pref_on_the_fly(ss,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                         ph_ptr->main_stack_info.pref_info.pref_term,
                                         ph_obj_mode_pref,
                                         ph_ptr->main_stack_info.pref_info.band_pref,
                                         ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->main_stack_info.pref_info.prl_pref,
                                         ph_ptr->main_stack_info.pref_info.roam_pref,
                                         ph_ptr->main_stack_info.pref_info.hybr_pref,
                                         ph_ptr->main_stack_info.pref_info.plmn,
                                         CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         CM_OTASP_ACT_CODE_NONE,
                                         ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                         rest_net_sel_mode_pref,
                                         (cm_act_id_type) curr_policy,
                                         CM_ACT_UPDATE_REAS_ACT_START,
                                         TRUE,
                                         cmph_get_acq_pri_order_per_sub(ss),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->main_stack_info.pref_info.csg_id,
                                         ph_ptr->main_stack_info.pref_info.csg_rat);
   	 }

	#ifdef FEATURE_MMODE_DUAL_SIM
      else if (!cmph_is_sxlte() && cmph_is_msim() && (ss == CM_SS_HYBR_2))
      { 
		 (void) cmph_force_pref_on_the_fly(CM_SS_HYBR_2,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                         ph_ptr->hybr_2_stack_info.pref_info.pref_term,
                                         ph_obj_mode_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.prl_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.roam_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.hybr_pref,
                                         ph_ptr->hybr_2_stack_info.pref_info.plmn,
                                         CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         CM_OTASP_ACT_CODE_NONE,
                                         ph_ptr->hybr_2_stack_info.pref_info.acq_order_pref,
                                         rest_net_sel_mode_pref,
                                         (cm_act_id_type) curr_policy,
                                         CM_ACT_UPDATE_REAS_ACT_START,
                                         TRUE,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_2),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_2_stack_info.pref_info.csg_rat);
	}
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
     else if ((cmph_is_msim()) && (ss == CM_SS_HYBR_3))
	 {
 
		(void) cmph_force_pref_on_the_fly(CM_SS_HYBR_3,
                                         ph_ptr,
                                         SD_SS_PREF_REAS_USER,
                                         CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                         ph_ptr->hybr_3_stack_info.pref_info.pref_term,
                                         ph_obj_mode_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.lte_band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.tds_band_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.prl_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.roam_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.hybr_pref,
                                         ph_ptr->hybr_3_stack_info.pref_info.plmn,
                                         CM_SRV_DOMAIN_PREF_CS_ONLY,
                                         CM_OTASP_ACT_CODE_NONE,
                                         ph_ptr->hybr_3_stack_info.pref_info.acq_order_pref,
                                         rest_net_sel_mode_pref,
                                         (cm_act_id_type) curr_policy,
                                         CM_ACT_UPDATE_REAS_ACT_START,
                                         TRUE,
                                         cmph_get_acq_pri_order_per_sub(CM_SS_HYBR_3),
                                         CM_DEFAULT_VALUE,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_id,
                                         ph_ptr->hybr_3_stack_info.pref_info.csg_rat);										 
      }
  #endif		
    }
  }
}

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
EXTERN void                       cmpmprx_enforce_voice_domain_pref(

       sys_modem_as_id_e_type     asubs_id
)
{
  #ifdef FEATURE_DOMAIN_SELECTION

  cmph_s_type             *ph_ptr = cmph_ptr();
  cm_ss_e_type            ss      = CM_SS_MAX;
  cm_orig_q_s_type        *stack_top_ptr = NULL;
  boolean                 is_force_top = FALSE;
  cm_pref_s_type          *hybr_gw_pref_ptr = NULL;
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_pref_s_type          *hybr_3_gw_pref_ptr = NULL;
  #endif
  cmsds_s_type            *sds_ptr = cmsds_ptr();
  /* Pointer to domain selection object */
  cmsds_sub_config_s_type *cmsds_config = cmsds_get_config_ptr(asubs_id);

  if(!cmpmprx_is_mask_set(asubs_id, PM_CHG_VOICE_DOMAIN_PREF, cmpm_ptr()))
  {
    return;
  }

  ss = cmph_map_subs_to_ss(asubs_id);

  cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_VOICE_DOMAIN_PREF);

  if(!cmph_is_lte_capable_on_sub(asubs_id))
  {
    CM_MSG_HIGH_1("Return without enforcing voice domain pref, sub %d is not multimode",
                    asubs_id);
    return;
  }

  /* Reset VOPS status to send appropriate VOPS support to RM after TAU completion */
  if(cmsds_config->voice_domain_pref != cmpm_ptr()->current_policy[asubs_id].voice_domain_pref)
  {
    sds_ptr->lte_ims_voice_avail = CMSDS_IMS_VOICE_NONE;
  }
  else if(cmsds_config->is_voice_domain_pref_locked != cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked )
  {
    CM_MSG_HIGH_3("Update and Return: only is_locked flag changed for sub %d, sds_ptr %d, cmpm_ptr %d",
                    asubs_id,
                    cmsds_config->is_voice_domain_pref_locked,
                    cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked);
    cmsds_config->is_voice_domain_pref_locked = cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked;
    return;
  }
  else
  {
    CM_MSG_HIGH_3("Return: no change in voice domain pref for sub %d, sds_ptr %d, cmpm_ptr %d",
                    asubs_id,
                    cmsds_config->voice_domain_pref,
                    cmpm_ptr()->current_policy[asubs_id].voice_domain_pref);
    return;
  }
  CM_MSG_HIGH_3("Enforce new voice_dom_pref for sub %d from PM, voice_dom_pref %d, is_locked %d",
                  asubs_id,
                  cmpm_ptr()->current_policy[asubs_id].voice_domain_pref,
                  cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked);

  cmsds_config->voice_domain_pref = cmpm_ptr()->current_policy[asubs_id].voice_domain_pref;
  cmsds_config->is_voice_domain_pref_locked = cmpm_ptr()->current_policy[asubs_id].is_voice_dom_pref_locked;

  *(CMPH_SS_VOICE_DOMAIN_PREF_PTR(asubs_id, ph_ptr)) = cmsds_config->voice_domain_pref;

  if(cmph_is_subs_feature_mode_sglte(asubs_id) || cmph_is_subs_feature_mode_svlte(asubs_id) ||
     cmph_map_subs_stack_to_ss(asubs_id,0) == CM_SS_MAIN)
  {
    stack_top_ptr     = cmtask_orig_para_get_top(CM_SS_MAIN);
    ss                = CM_SS_MAX;
  }
  else
  {
    stack_top_ptr = cmtask_orig_para_get_top(ss);
  }

  if(stack_top_ptr != NULL &&
      (stack_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ ||
        stack_top_ptr->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT))
  {
    is_force_top      = TRUE;
  }
  else
  {
    is_force_top      = FALSE;
  }

  if(cmph_is_msim())
  {
    hybr_gw_pref_ptr = cm_pref_ptr();
    memscpy(hybr_gw_pref_ptr, sizeof(cm_pref_s_type),
                  &(ph_ptr->hybr_2_stack_info.pref_info), sizeof(cm_pref_s_type));
    
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    hybr_3_gw_pref_ptr = cm_pref_ptr();
    memscpy(hybr_3_gw_pref_ptr, sizeof(cm_pref_s_type),
                  &(ph_ptr->hybr_3_stack_info.pref_info), sizeof(cm_pref_s_type));
    #endif
  }

  (void)cmph_force_pref_on_the_fly2(
                                     ss,
                                     ph_ptr,
                                     SD_SS_PREF_REAS_USER,
                                     CM_ACT_TYPE_PH_OBJ,
                                     ph_ptr->main_stack_info.pref_info.pref_term,
                                     ph_ptr->main_stack_info.pref_info.mode_pref,
                                     ph_ptr->main_stack_info.pref_info.band_pref,
                                     ph_ptr->main_stack_info.pref_info.lte_band_pref,
                                     ph_ptr->main_stack_info.pref_info.tds_band_pref,
                                     ph_ptr->main_stack_info.pref_info.prl_pref,
                                     ph_ptr->main_stack_info.pref_info.roam_pref,
                                     ph_ptr->main_stack_info.pref_info.hybr_pref,
                                     ph_ptr->main_stack_info.pref_info.plmn,
                                     ph_ptr->main_stack_info.pref_info.srv_domain_pref,
                                     ph_ptr->main_stack_info.pref_info.acq_order_pref,
                                     ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                                     hybr_gw_pref_ptr,
                                     #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                                     hybr_3_gw_pref_ptr,
                                     #endif
                                     CM_OTASP_ACT_CODE_NONE,
                                     (cm_act_id_type)ph_ptr,
                                     CM_ACT_UPDATE_REAS_USER,
                                     is_force_top,
                                     cmph_get_acq_pri_order_per_sub(ss),
                                    CM_DEFAULT_VALUE,
                                    ph_ptr->main_stack_info.pref_info.csg_id,
                                    ph_ptr->main_stack_info.pref_info.csg_rat);

  cmefs_fs_write(CMEFS_VOICE_DOMAIN_PREF,
                (byte*)&cmsds_config->voice_domain_pref,
                sizeof(sys_voice_domain_pref_e_type),
                asubs_id);

   /* Notify clients of phone event.
   */
   cmph_event_per_subs( ph_ptr, CM_PH_EVENT_SYS_SEL_PREF, 
                       cmph_map_subs_to_ss(asubs_id));

  /* Free allocated buffer */
  if (hybr_gw_pref_ptr)
  {
    cm_mem_free (hybr_gw_pref_ptr);
  }
  
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if (hybr_3_gw_pref_ptr)
  {
    cm_mem_free (hybr_3_gw_pref_ptr);
  }
  #endif

  #endif
}

/*===========================================================================

FUNCTION cmpmprx_unforce_policy

DESCRIPTION
  Process config commands.

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                       cmpmprx_unforce_policy(

       sys_modem_as_id_e_type     asubs_id

)
{

  cmph_s_type             *ph_ptr = cmph_ptr();
  int					stack_id = 0;

  cmpmprx_set_policy_chg_flag( asubs_id,PM_CHG_ALL );

  cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_VOICE_DOMAIN_PREF);
  
  for ( stack_id = 0; stack_id < MAX_STACK_PER_SUB; stack_id++ )
  {
    cm_ss_e_type ss = CM_SS_NONE ;
    #if defined(FEATURE_MMODE_TRIPLE_SIM)
    if( asubs_id == SYS_MODEM_AS_ID_3 )
    {
      ss = ph_ptr->subs_stack_map[asubs_id][stack_id];
    }
    else
    #endif
    if( (asubs_id == SYS_MODEM_AS_ID_1) || (asubs_id == SYS_MODEM_AS_ID_2))
    {
    
      ss = ph_ptr->subs_stack_map[asubs_id][stack_id];
    }
    else
    {
      ss = CM_SS_NONE ;
    }
    CM_MSG_HIGH_1("unforce_policy:getting, ss=%d", ss);
    if(ss != CM_SS_NONE)
    {
      cm_orig_q_s_type *pm_rest_obj_ptr = cmtask_orig_para_search_act_type( ss, 
                                                   CM_ACT_TYPE_POLICYMAN_RESTRICT);
  if ( pm_rest_obj_ptr != NULL )
  {
        (void)cmtask_orig_para_delete(ss,pm_rest_obj_ptr,TRUE);
      }
    }
  }
}

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
EXTERN void                         cmpmprx_sys_sel_pref_proc(

       cm_ph_cmd_info_s_type        *cmd_info_ptr,

       cm_mode_pref_e_type          prefered_mode,

       cm_pref_s_type               *hybr_2_pref_info_ptr,
           /* Preferences of the HYBR2 stack */
       
       #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
       cm_pref_s_type               *hybr_3_pref_info_ptr,
           /* Preferences of the HYBR3 stack */
       #endif

       cm_acq_pri_order_pref_s_type *rat_acq_pri_order
)
{
  cm_orig_q_s_type        *pm_rest_obj_ptr  =
                          cmtask_orig_para_search_act_type( cmph_map_as_id_to_cm_ss(cmd_info_ptr->cmd_subs),
                                                            CM_ACT_TYPE_POLICYMAN_RESTRICT);

  cm_network_sel_mode_pref_e_type net_sel_mode_pref = 
                cmpmprx_compute_restricted_mode( cmd_info_ptr->cmd_subs,cmd_info_ptr->network_sel_mode_pref);
  
  if ( pm_rest_obj_ptr == NULL )
  {
    return;
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(cmd_info_ptr->cmd_subs == SYS_MODEM_AS_ID_3 || 
      (cmd_info_ptr->cmd_subs == SYS_MODEM_AS_ID_2 && cmph_is_sxlte()))
  {
    hybr_3_pref_info_ptr->network_sel_mode_pref = net_sel_mode_pref;
  }
  else
  #endif
  if(cmd_info_ptr->cmd_subs == SYS_MODEM_AS_ID_2)
  {
    hybr_2_pref_info_ptr->network_sel_mode_pref = net_sel_mode_pref;
  }
  
  CM_MSG_HIGH_0("PMPRX: Force the user preferences on PolicyMan obj too");

  (void)cmph_force_pref_on_the_fly2(
                                 ((cmd_info_ptr->cmd_subs == SYS_MODEM_AS_ID_1)? CM_SS_MAX : (cmph_map_as_id_to_cm_ss(cmd_info_ptr->cmd_subs))),
                                     cmph_ptr(),
                                     SD_SS_PREF_REAS_USER,
                                     CM_ACT_TYPE_POLICYMAN_RESTRICT,
                                     cmph_ptr()->main_stack_info.pref_info.pref_term,
                                     prefered_mode,
                                     cmd_info_ptr->band_pref,
                                     cmd_info_ptr->lte_band_pref,
                                     cmd_info_ptr->tds_band_pref,
                                     cmd_info_ptr->prl_pref,
                                     cmd_info_ptr->roam_pref,
                                     cmd_info_ptr->hybr_pref,
                                     cmd_info_ptr->plmn,
                                     CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                     cmd_info_ptr->acq_order_pref,
                                     net_sel_mode_pref,
                                 hybr_2_pref_info_ptr,
                                 #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                                 hybr_3_pref_info_ptr,
                                 #endif
                                 CM_OTASP_ACT_CODE_NONE,
                                     (cm_act_id_type) cmpm_ptr(),
                                     CM_ACT_UPDATE_REAS_ACT_START,
                                     FALSE,
                                     rat_acq_pri_order,
                                     cmd_info_ptr->sys_sel_pref_req_id,
                                     cmd_info_ptr->csg_id,
                                     cmd_info_ptr->csg_rat);

}

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
)
{
  if ( !cmpmprx_is_mask_set( asubs_id, PM_CHG_UE_MODE, cmpm_ptr() ) ||
       ue_mode_buffer == NULL       ||
       !cmpmprx_read_cm_policy_consumption_eligibility(asubs_id)
     )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_1("PMPRX -> CM: Policyman ue_mode=%d",
               cmpm_ptr()->current_policy[asubs_id].ue_operation_mode);

  *ue_mode_buffer   = cmpm_ptr()->current_policy[asubs_id].ue_operation_mode;
  cmpmprx_reset_policy_chg_flag( asubs_id, PM_CHG_UE_MODE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_read_ue_mode

DESCRIPTION
  This API should be used only when 
  cmmsc_init is not called yet .

DEPENDENCIES
None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                    cmpmprx_read_ue_mode(

       sys_modem_as_id_e_type     asubs_id,

       sys_ue_mode_e_type         *ue_mode_buffer
)
{
#ifdef CM_DEBUG
  #error code not present
#else
  CM_MSG_HIGH_1("PMPRX -> CM: Policyman ue_mode=%d",
               cmpm_ptr()->current_policy[asubs_id].ue_operation_mode);
  
  *ue_mode_buffer   = cmpm_ptr()->current_policy[asubs_id].ue_operation_mode;
#endif
}


/*===========================================================================

FUNCTION cmpmprx_get_device_mode

DESCRIPTION
  Gets device mode which is decided based on no. of SIM and max SIMs that are
  active at a given time.

===========================================================================*/
sys_modem_device_mode_e_type cmpmprx_get_device_mode( void )
{
  cm_policy_config_s_type           *pm_info = cmpm_ptr();
  cm_policy_device_mode_s_type    pm_device_mode = pm_info->device_mode;
  sys_modem_device_mode_e_type  mapped_device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;

  if(pm_device_mode.nSims == 3)
  {
    mapped_device_mode = SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY;
  }

  if(pm_device_mode.nSims == 2)
  {
    switch(pm_device_mode.nMaxActive)
    {
      case 2:
        mapped_device_mode = SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE;
        break;

      case 1:
      default:
        mapped_device_mode = SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY;
    }
  }

  pm_info->device_policy_changes &= (~PM_CHG_DEVICE_MODE);
  
  return mapped_device_mode;
}

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
size_t  cmpmprx_get_num_of_sims( void )
{
  cm_policy_config_s_type           *pm_info = cmpm_ptr();
  return (pm_info->device_mode.nSims);
}

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
size_t  cmpmprx_get_num_of_active_data( void )
{
  cm_policy_config_s_type           *pm_info = cmpm_ptr();
  return (pm_info->device_mode.nMaxActiveData);
}

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
boolean cmpmprx_get_volte_allowed_from_pm_on_sub(sys_modem_as_id_e_type asub_id)
{
  boolean ims_for_nondds = FALSE;

  if(cmpmprx_get_num_of_active_data() == 2 && (cmpmprx_read_mode_capability(asub_id) & (uint32)SD_SS_MODE_PREF_LTE))
  {
    ims_for_nondds = policyman_get_subs_boolean_value("cm:volte_allowed",asub_id,&ims_for_nondds) && ims_for_nondds;
  }

  return (ims_for_nondds);
}

/*===========================================================================

FUNCTION cmpmprx_rread_device_mode

DESCRIPTION
  Added for the purpose of debugging in case we need to re-read device mode
===========================================================================*/
void cmpmprx_rread_device_mode( void )
{
  cm_policy_config_s_type *pm_ptr = cmpm_ptr();
  pm_ptr->device_mode = cmpmprx_read_pm_device_mode();
}

/*===========================================================================

FUNCTION cmpmprx_check_sys_sel_pref_policy_conflict

DESCRIPTION
  If the new system selection preferences cause a policy change, check if it 
  in conflict with current device state.
  If policy change results in UE mode change and voice call ongoing, then
  mark conflict as TRUE

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
cm_ph_cmd_err_e_type                     cmpmprx_check_sys_sel_pref_policy_conflict(

  cm_ph_cmd_s_type           *ph_cmd_ptr
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  msgr_attach_s           *pAttach            = NULL;
  int                      i = 0;
  uint8                    j = 0;
  policy_change_mask       change_mask = 0;
  cm_sys_sel_pref_params_s_type pref_info;
  cm_ph_cmd_info_s_type*    cmd_info_ptr;
  sys_modem_as_id_e_type    asubs_id;
  cmph_s_type*              ph_ptr = cmph_ptr();
  cm_ph_cmd_err_e_type    return_val = CM_PH_CMD_ERR_NOERR;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );
  asubs_id = cmd_info_ptr->cmd_subs;

  /* Call into Policy manager and retrieve new policy if needed.
  ** Use the same preferences as used in the force_pref_on_fly() below.
  */
  pref_info.mode_pref        = cmd_info_ptr->mode_pref;
  pref_info.term_pref        = ph_ptr->main_stack_info.pref_info.pref_term;
  pref_info.pref_duration    = cmd_info_ptr->pref_duration;
  pref_info.band_pref        = cmd_info_ptr->band_pref;
  pref_info.lte_band_pref    = cmd_info_ptr->lte_band_pref;
  pref_info.tds_band_pref    = cmd_info_ptr->tds_band_pref;
  pref_info.prl_pref         = cmd_info_ptr->prl_pref;
  pref_info.roam_pref        = cmd_info_ptr->roam_pref;
  pref_info.hybr_pref        = cmd_info_ptr->hybr_pref;
  pref_info.srv_domain_pref  = cmd_info_ptr->srv_domain_pref;
  pref_info.network_sel_mode_pref = cmd_info_ptr->network_sel_mode_pref;
   /* this is to avoid sending null PLMN values to PM when network sel mode is set to no_change from manual */
  if((cmd_info_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL) && ((sys_plmn_id_is_null(cmd_info_ptr->plmn))|| (sys_plmn_id_is_undefined(cmd_info_ptr->plmn))))
  {
    pref_info.plmn_ptr	   = &(ph_ptr->main_stack_info.pref_info.plmn);
  }
  else
  {
    pref_info.plmn_ptr         = &(cmd_info_ptr->plmn);
  }

  //pref_ptr->rat_acq_order_pref_ptr = &rat_acq_pri_order;
  pref_info.csg_id           = cmd_info_ptr->csg_id;
  pref_info.csg_rat          = cmd_info_ptr->csg_rat;
  pref_info.ue_usage_setting = CMPH_SS_UE_USAGE_SETTING(asubs_id, ph_ptr);

  /* Ensure that the subscription id is valid.
  */
  if ( asubs_id > SYS_MODEM_AS_ID_3 )
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  
  if (cmd_info_ptr->srv_domain_pref == CM_SRV_DOMAIN_PREF_NO_CHANGE)
  {
    pref_info.srv_domain_pref = CMPH_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr );
  }

  if(pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ATTACH)
  {
    if( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_ONLY ) ||
          ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS ) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_PS;
    }
    else if ( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_NONE) ||
       ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_PS_ONLY) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_PS_ONLY;
    }
  }

  if(pref_info.srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_DETACH)
  {
    if( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_ONLY ) ||
          ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS ) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
    }
    else if ( ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_NONE) ||
       ( CMPH_SS_SRV_DOMAIN_PREF(asubs_id,ph_ptr) == CM_SRV_DOMAIN_PREF_PS_ONLY) )
    {
      pref_info.srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_compute_policy_with_newpref( asubs_id, &pref_info);


  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_MED_0("PMPRX: New preferences have conflict in Policy Manager, Rsp NULL");

    /* For UT verification*/
    #ifdef CM_DEBUG
    #error code not present
#endif

    return return_val;
  }
  
  for (i = 0 ; i < msgr_get_num_attach(&(pm_msgr_rsp->msg_hdr)) ; ++i)
  {
    policyman_item_collection_t const *policy_item = NULL;
    pAttach = msgr_get_attach(&(pm_msgr_rsp->msg_hdr), i);
    if (pAttach == NULL || pAttach->type != MSGR_ATTACH_REFCNT)
    {
      CM_MSG_HIGH_1("PMPRX: Invalid attachment %d.Abort new policy",i);
      change_mask = PM_CHG_CHANGE_NONE;
      break;
    }

    policy_item = pAttach->data.refcnt.refcnt_obj_ptr;
    
    for (j = 0 ;j < policy_item->numItems ; j++)
    {
      policyman_item_t const *pItem = policy_item->pItems[j];
      switch (policyman_item_get_id_msim(pItem,&asubs_id))
      {
       /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
       case POLICYMAN_ITEM_UE_MODE:
         change_mask |= PM_CHG_UE_MODE;
         break;
       /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
       case POLICYMAN_ITEM_DEVICE_CONFIGURATION:
         change_mask |= PM_CHG_DEVICE_MODE;
         break;
         
        /*- - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - -*/
       default:
         break;
      }
    }
  }

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  CM_MSG_HIGH_1("PMPRX: New Change mask:%d",change_mask);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(((change_mask & PM_CHG_UE_MODE)
      &&(( cmcall_find_active_cs_call_per_subs (asubs_id)
            != CM_CALL_ID_INVALID)
           || (cmcall_is_there_a_call_type_per_sub(asubs_id,CM_CALL_TYPE_SMS,NULL)
            != CM_CALL_ID_INVALID)
         ))
      ||
      ((change_mask & PM_CHG_DEVICE_MODE)
      &&( cmcall_find_active_voice_call()
      != CM_CALL_ID_INVALID)))
  {
    return_val = CM_PH_CMD_ERR_IN_USE_S;
  }

  return return_val;
}

/*===========================================================================

FUNCTION cmpmprx_sys_sel_pref_policy_update

DESCRIPTION
  If the new system selection preferences cause a policy change, retrieve it
  from Policy Manager and update the cmpm_local.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
boolean                           cmpmprx_sys_sel_pref_policy_update(

       sys_modem_as_id_e_type     asubs_id,

       cm_sys_sel_pref_params_s_type* pref_ptr,

	    cm_ph_cmd_s_type          *ph_cmd_ptr
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  boolean                 return_val    = FALSE;
  policy_change_mask device_local_policy = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pref_ptr == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: Incoming pref_ptr NULL");
    return return_val;
  }

  /* Ensure that the subscription id is valid.
  */
  if ( asubs_id == SYS_MODEM_AS_ID_NO_CHANGE || asubs_id == SYS_MODEM_AS_ID_MAX 
      || asubs_id == SYS_MODEM_AS_ID_NONE)
  {
    CM_MSG_HIGH_1("Returning from cmpmprx_sys_sel_pref_policy_update() since as_id is %d",asubs_id);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_preference_change( asubs_id, pref_ptr);


  if ( pm_msgr_rsp == NULL )
  {
    return return_val;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
    return return_val;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  return_val = cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy,&device_local_policy);

  CM_MSG_HIGH_1("device local policy %d",device_local_policy);

  /* If there is a pref change while ue is in voice all and the retrieved
  ** policy has ue_mode change, dont enforce the policy. Wait for it to arrive
  ** on message router message so it wont interfere with call.
  */
  if ( return_val == TRUE &&
       cmpmprx_is_ok_to_enforce_policy( local_policy, asubs_id ) )
  {
    CM_MSG_HIGH_1("policy_chgd: SYS_SEL_PREF, asubs_id=%d", 
                   asubs_id);
    cmpmprx_update_curr_policy(local_policy);
  }
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Enforce device level policy */
  if(device_local_policy != 0)
  {
    CM_MSG_HIGH_2("device_policy_chgd: SYS_SEL_PREF, mask=0x%x, asubs_id=%d", 
                   device_local_policy, asubs_id);

    if( ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE || 
	   ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE ||
	   ph_cmd_ptr->cmd == CM_PH_CMD_DUAL_STANDBY_PREF ||
	   ph_cmd_ptr->cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED )
    {
      cmpmprx_enforce_device_policy(PM_CHG_DEVICE_MODE|PM_CHG_MAX_ACTIVE_DATA);
    }
    else
    {
      cmpmprx_enforce_device_policy(PM_CHG_DEVICE_ALL);
    }
  }

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  cm_mem_free(local_policy);
  return return_val;
}

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
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  boolean                 return_val    = FALSE;
  dds_info_t              dds_info;
  policy_change_mask device_local_policy = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If neither active_subs nor default_data_subs changed, do not inform PM */
  
  if(active_subs == cmpm_active_subs &&
     default_data_subs == cmpm_default_data_subs && 
     cmph_ptr()->prst_dual_standby_pref.default_data_subs == default_data_subs)
  {
    if(cmpm_ptr()->device_policy_changes != 0)
    {
      cmpmprx_update_flags_ims_for_nondds(default_data_subs);
    }  
    return FALSE;
  }

  /* PM implementation is such a way that policyman_msim_report_subs_info
  ** is expected to be invoked only when PM is resumed. Since emergency call is
  ** anyway ended by now. Going ahead and resuming PM
  */

  if ( cmpm_ptr()->is_pm_suspend)
  {
    CM_MSG_HIGH_0("PMPRX: resume PM");
    policyman_resume();
    cmpm_ptr()->is_pm_suspend = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  CM_MSG_HIGH_3("PMPRX<< CM: DDS/ACTIVE_SUBS change active_subs %d, dds %d, dds_cause %d",
                 active_subs, default_data_subs, dds_status);

  cmpm_active_subs = active_subs;
  cmpm_default_data_subs = default_data_subs;

  dds_info.isPermanent = (dds_status == DDS_CAUSE_TEMPORARY)?FALSE:TRUE;
  dds_info.subsId = default_data_subs;
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                  policyman_msim_report_subs_info( &dds_info, active_subs);

  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_MED_0("PMPRX: New MSIM subs have no effect on policy");
    cmpmprx_update_flags_ims_for_nondds(default_data_subs);

    return TRUE;
  }

  local_policy = (cm_policy_config_s_type *)
                    cm_mem_malloc(sizeof(cm_policy_config_s_type));

  *local_policy = *(cmpm_ptr());

  return_val = cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy,&device_local_policy);

  if ( return_val == TRUE &&
       (((active_subs & BM(SYS_MODEM_AS_ID_1)) && (cmpmprx_is_ok_to_enforce_policy(cmpm_ptr(), SYS_MODEM_AS_ID_1)
         || cmpmprx_is_ok_to_enforce_policy(local_policy, SYS_MODEM_AS_ID_1))) ||
        ((active_subs & BM(SYS_MODEM_AS_ID_2)) && (cmpmprx_is_ok_to_enforce_policy(cmpm_ptr(), SYS_MODEM_AS_ID_2)
         || cmpmprx_is_ok_to_enforce_policy(local_policy, SYS_MODEM_AS_ID_2))) ||
       #ifdef FEATURE_MMODE_TRIPLE_SIM
        ((active_subs & BM(SYS_MODEM_AS_ID_3)) && (cmpmprx_is_ok_to_enforce_policy(cmpm_ptr(), SYS_MODEM_AS_ID_3)
         ||cmpmprx_is_ok_to_enforce_policy(local_policy, SYS_MODEM_AS_ID_3))) ||
       #endif
        local_policy->device_policy_changes != 0
      ))
  {  
    CM_MSG_HIGH_2("policy_chgd: DDS/ACTIVE_SUBS, device_policy mask=0x%x, active_subs=%d", 
                   local_policy->device_policy_changes,
                   active_subs
                 );
    cmpmprx_update_curr_policy(local_policy);
  }
  else
  {
    CM_MSG_HIGH_3("No policy_chgd: DDS/ACTIVE_SS, active_ss=%d, PM return_val=%d, device local policy %d", 
                   active_subs, return_val,device_local_policy);
  }
       
  policyman_report_msg_free( (msgr_hdr_s *)pm_msgr_rsp );
  
  cm_mem_free(local_policy);

  /* this is called to update subs capability to TRM/MCS */
  cmph_sp_subs_info_update();

  cmpmprx_update_flags_ims_for_nondds(default_data_subs);

  return return_val;
}

/*===========================================================================

FUNCTION cmpmprx_emergency_call_policy_update

DESCRIPTION
  Enforce Full RAT mode on the UE.
  *** THIS IS TO BE USED ONLY FOR EMERGENCY CALL WHEN HYBRID2 NOT IN SRV ***

  Restore the original policy to cmpm_local so that when the call is ended, it
  will be enforced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void                       cmpmprx_emergency_call_policy_update(sys_modem_as_id_e_type  asubs_id)
{

  cm_policy_config_s_type *emerg_policy = NULL; // Store the full rat policy.
  cm_policy_config_s_type *curr_policy = NULL; // Store the full rat policy.

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate memory for FULL RAT mode policy. */
  emerg_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  curr_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));

  if ( emerg_policy == NULL || curr_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to enforce Full RAT as cannot allocate memory");
    return;
  }

  *emerg_policy = *(cmpm_ptr());
  *curr_policy = *(cmpm_ptr());

  /* Populate FULL RAT mode for the emergency call. */
  emerg_policy->current_policy[asubs_id].ue_operation_mode          = SYS_UE_MODE_SGLTE_ROAM;
  emerg_policy->current_policy[asubs_id].service_scan_mode          = POLICYMAN_SVC_MODE_LIMITED;
  emerg_policy->current_policy[asubs_id].policy_mode_capability     = SD_SS_MODE_PREF_ALL_MODES;
  emerg_policy->current_policy[asubs_id].policy_band_capability     = SYS_BAND_MASK_ANY;
  emerg_policy->current_policy[asubs_id].policy_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
  emerg_policy->current_policy[asubs_id].policy_tds_band_capability = SYS_BAND_MASK_ANY;

  emerg_policy->policy_changes[asubs_id] = (PM_CHG_ALL & ~PM_CHG_VOICE_DOMAIN_PREF);
  

  cmpmprx_update_curr_policy(emerg_policy);

  cm_mem_free(emerg_policy);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Enforce FULL RAT mode for the emergency call. */
  cmpmprx_enforce_current_policy(asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_policy->policy_changes[0] = PM_CHG_ALL;
  cmpmprx_reset_policy_chg_flag(asubs_id,PM_CHG_VOICE_DOMAIN_PREF);

  cmpmprx_update_curr_policy(curr_policy);

  cm_mem_free(curr_policy);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}

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
)
{
  cm_cmd_type              *cmd_ptr = NULL;
  cm_service_status_s_type srv_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* This processing applies only to emergency call orignation in SGLTE mode*/

  if (call_ptr == NULL ||
     (!cmph_is_subs_feature_mode_sglte(call_ptr->asubs_id))||      
      curr_cmd == NULL )
  {
    return FALSE;
  }

  if ( curr_msc_ue_mode    != SYS_UE_MODE_SGLTE_HOME ||
       call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
  {
    CM_MSG_HIGH_2("PMPRX: call_type %d ue_mode %d",call_ptr->call_type,
                                                curr_msc_ue_mode);
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the stack being used for emergency call origination in */
  cmss_get_service_status_per_stack(call_ptr->ss,&srv_info);

  CM_MSG_HIGH_2("PMPRX: Service %d available on ss %d for emerg call",
                                                srv_info.srv_status,
                                                       call_ptr->ss);
  if (sys_srv_status_is_srv(srv_info.srv_status))
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  **Change the policy locally to FULL RAT mode.
  */
  cmpmprx_emergency_call_policy_update(call_ptr->asubs_id);

  CM_MSG_HIGH_0("PMPRX: Requeing emerg call for orig on FULL RAT");
  /*
  **connection id will be reserved later.
  */
  cmcall_unreserve_connection_id (call_ptr);

  cmd_ptr = (cm_cmd_type *) curr_cmd;
  cmd_ptr->cmd_type = CM_CMD_TYPE_CALL;
  CALL_CMD_PTR(cmd_ptr)->cmd = CM_CALL_CMD_ORIG;
  cmd_ptr->is_reused = TRUE;
  cm_cmd_queue (cmd_ptr);

  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_get_overall_featuremode

DESCRIPTION
  Called from other modules to read the current value of the overall
  featuremode in CMPM
===========================================================================*/
sys_overall_feature_t cmpmprx_get_overall_featuremode(void)
{
  return (cmpm_ptr()->feature_mode);
}

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
EXTERN boolean                    cmpmprx_get_subs_feature_mode(

       sys_modem_as_id_e_type     asubs_id,

       sys_subs_feature_t         *subs_featuremode_buffer
)
{
  if ( subs_featuremode_buffer == NULL)
  {
    CM_MSG_MED_0("PMPRX -> CM: NO policyman ue_mode update");
    return FALSE;
  }
  else
  {
    sys_subs_feature_t pm_sfmode = cmpm_ptr()->current_policy[asubs_id].sub_feature_mode;

  CM_MSG_HIGH_2("PMPRX -> CM: Policyman subs_id = %d, subs_feature =%d",
                   asubs_id,pm_sfmode);

    /* Map SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE to srlte+normal internally */
    if (pm_sfmode == SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE)
    {
      if (asubs_id == SYS_MODEM_AS_ID_1)
        *subs_featuremode_buffer   = SYS_SUBS_FEATURE_MODE_SRLTE;
      else
        *subs_featuremode_buffer   = SYS_SUBS_FEATURE_MODE_NORMAL;
    }
    else
    {
      *subs_featuremode_buffer   = pm_sfmode;
    }

  #ifndef FEATURE_MMODE_SXLTE_G
  if (cmph_ptr()->feature_mode == SYS_OVERALL_FEATURE_MODE_MULTISIM)
  {
    CM_MSG_HIGH_2("fmode: changing subs_feature_mode to NORMAL for asubs_id %d, pm %d", asubs_id, *subs_featuremode_buffer);
    *subs_featuremode_buffer = SYS_SUBS_FEATURE_MODE_NORMAL;
  }
  #endif
    
  return TRUE;
}
}

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
EXTERN void                       cmpmprx_clnup_complete_ind()
{
  sys_modem_as_id_e_type          asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (asubs_id = SYS_MODEM_AS_ID_1; asubs_id < SYS_MODEM_AS_ID_NO_CHANGE; asubs_id++)
  {
    if ( cmpmprx_read_cm_policy_consumption_eligibility( asubs_id ) &&
         cmpmprx_is_mask_set( asubs_id, PM_CHG_ALL, cmpm_ptr() )
       )
    {
      cmpmprx_enforce_current_policy(asubs_id);
    }
  }
}

/*===========================================================================

FUNCTION cmpmprx_process_thermal_policy_change

DESCRIPTION
  Handles policy change due to thermal state change and takes necessary action

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmpmprx_process_thermal_policy_change(
    const cfcm_cmd_msg_type_s *cfcm_ind,
    cm_policy_config_s_type  *new_policy
    )
{
  cm_policy_config_s_type *pm_ptr           = cmpm_ptr();
  unsigned int                  i           = 0;
  uint32                   removed_mode_mask;
  cmph_s_type             *ph_ptr           = cmph_ptr();
  if(cfcm_ind->fc_cmd.cmd != CFCM_CMD_DOWN ||
      !(cfcm_ind->fc_cmd.monitors_active & CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX)
     )
  {
    CM_MSG_HIGH_0("No need to end the calls in current state");
    return;
  }

  for(i=0; i<new_policy->device_mode.nSims; i++)
  {
    if(!cmpmprx_read_cm_policy_consumption_eligibility(i))
      continue;

    if(cmpmprx_is_mask_set(i,(PM_CHG_SVC_SCAN|PM_CHG_UE_MODE),new_policy))
    {

      CM_MSG_HIGH_0("PM_CHG_SVC_SCAN|PM_CHG_UE_MODE");

      cmcall_endall_non_emerg_cs_calls(CM_CALL_END_THERMAL_EMERGENCY, BM(i));
      return;
    }

    if(cmpmprx_is_mask_set(i,PM_CHG_MODE_CAP,new_policy))
    {
      CM_MSG_HIGH_0("PM_CHG_MODE_CAP"); 

      removed_mode_mask = (pm_ptr->current_policy[i].policy_mode_capability & 
                      ~new_policy->current_policy[i].policy_mode_capability);

      if(removed_mode_mask &
        cmph_map_cm_mode_pref_to_sd_mode_pref(CMPH_SS_MODE_PREF(i, ph_ptr)))
      {
        cmcall_endall_non_emerg_cs_calls(CM_CALL_END_THERMAL_EMERGENCY, BM(i));
      }
    }
  }

}

/*===========================================================================

FUNCTION cmpmprx_thermal_state_change_policy_update

DESCRIPTION
  If the new system selection preferences cause a policy change, retrieve it
  from Policy Manager and update the cmpm_local.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: New policy retrieved and updated.
  False: No change in policy.

SIDE EFFECTS
  none

===========================================================================*/
void                           cmpmprx_thermal_state_change_policy_update(

       const cfcm_cmd_msg_type_s *cfcm_ind
)
{
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  unsigned int             i            = 0;
  policy_change_mask device_local_policy = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cfcm_ind == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: Incoming cfcm_ind NULL");
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_thermal_state( &cfcm_ind->fc_cmd);


  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_MED_0("PMPRX: New preferences have no effect on policy");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
    return ;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy,&device_local_policy) )
  {
    cmpmprx_process_thermal_policy_change(cfcm_ind,local_policy);
    cmpmprx_update_curr_policy(local_policy);
  }

  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );

  cm_mem_free(local_policy);

  if(cfcm_ind->fc_cmd.cmd == CFCM_CMD_SHUT_DOWN)
  {
    CM_MSG_HIGH_0("In Lv3: policy change will be handled after moving to emergency mode");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Enforce device level policy */
  if(cmpm_ptr()->device_policy_changes != 0)
  {
    cmpmprx_enforce_device_policy(PM_CHG_DEVICE_ALL);
  }
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Loop thru the subs_ids and see if any policy changed. If yes then enforce*/
  /* If any policy item has changed, attempt to enforce the new policy. */
  for (i=0;i<cmpm_ptr()->device_mode.nSims;i++)
  {
    if ( cmpmprx_is_mask_set( i, PM_CHG_ALL, cmpm_ptr() ) )
    {
      cmpmprx_enforce_current_policy(i);
    }
  }
}

/*===========================================================================

FUNCTION cmpmprx_map_volte_mode_enable_to_volte_state

DESCRIPTION
  Handles E911 Volte On/Off command from QMI-V
  and forwards it to PM 


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cm_volte_enable_e_type cmpmprx_map_volte_mode_enable_to_volte_state(boolean volte_mode_from_ap)
{ 

  if(volte_mode_from_ap == TRUE)
  {
    return CM_VOLTE_ENABLED;
  }  
  else
  {
    return CM_VOLTE_DISABLED;
  }
  
}


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
)
{
  cm_sys_sel_pref_params_s_type *pmprx_pref_ptr = NULL;
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  cm_policy_config_s_type *local_policy = NULL;
  boolean return_val;
  policy_change_mask device_local_policy = 0;
  cmph_s_type *ph_ptr = cmph_ptr();
  sys_modem_as_id_e_type  asubs_id = volte_state_cmd->as_id;
  sys_plmn_id_s_type    plmn = CMPH_SS_PLMN_PREF(asubs_id,ph_ptr);

  CM_MSG_HIGH_2("PMPRX<< CM: volte_state_noti asubs_id %d volte_mode_enable %d",
                 volte_state_cmd->as_id,
                 volte_state_cmd->volte_mode_enable);

  if(!(volte_state_cmd->as_id > SYS_MODEM_AS_ID_NONE &&
         volte_state_cmd->as_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_0("Invalid AS_ID in volte state notification cmd");
	return;
  }
  pmprx_pref_ptr = (cm_sys_sel_pref_params_s_type*)
                         cm_mem_malloc(sizeof(cm_sys_sel_pref_params_s_type));

  if (pmprx_pref_ptr == NULL)
  {
    CM_MSG_HIGH_0("PMPRX: pmprx_pref_ptr NULL");
    return;
  }

  

  /* Fill all the fields with No_change, for fields like term_pref use from cmph_ptr */

  pmprx_pref_ptr->mode_pref        = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);  
  pmprx_pref_ptr->pref_duration    = 0;
  pmprx_pref_ptr->band_pref        = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->lte_band_pref    = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->tds_band_pref    = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->prl_pref         = CMPH_SS_PRL_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->roam_pref        = CMPH_SS_ROAM_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->hybr_pref        = CMPH_SS_HYBR_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->srv_domain_pref  = CMPH_SS_SRV_DOMAIN_PREF(asubs_id, ph_ptr);
  pmprx_pref_ptr->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, ph_ptr);  
  pmprx_pref_ptr->csg_id           = ph_ptr->csg_id;
  pmprx_pref_ptr->csg_rat          = ph_ptr->csg_rat;
  pmprx_pref_ptr->ue_usage_setting = CMPH_SS_UE_USAGE_SETTING(asubs_id, ph_ptr);

  if( pmprx_pref_ptr->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL
      &&
     (!sys_plmn_id_is_null(plmn) &&
      !sys_plmn_id_is_undefined(plmn))
    )
  {
    pmprx_pref_ptr->plmn_ptr       = &plmn;
  }
  else
  {
    sys_plmn_undefine_plmn_id(&plmn);
    pmprx_pref_ptr->plmn_ptr       = &plmn;
  }

  {
    cm_ss_e_type cm_ss = cmph_map_subs_to_ss(volte_state_cmd->as_id);
	CM_MSG_LOW_2("cm_ss %d, for subs id %d", cm_ss, volte_state_cmd->as_id);
    if(cm_ss == CM_SS_HYBR_2)
	{
	  pmprx_pref_ptr->term_pref        = cmph_ptr()->hybr_2_stack_info.pref_info.pref_term;
	  pmprx_pref_ptr->plmn_ptr         = &(cmph_ptr()->hybr_2_stack_info.pref_info.plmn);
	}
	else if(cm_ss == CM_SS_MAIN)
	{
	  pmprx_pref_ptr->term_pref        = cmph_ptr()->main_stack_info.pref_info.pref_term;
	  pmprx_pref_ptr->plmn_ptr         = &(cmph_ptr()->main_stack_info.pref_info.plmn);
	}	
  }
  /* fill new value of volte mode coming from AP */
  pmprx_pref_ptr->volte_enable_state = cmpmprx_map_volte_mode_enable_to_volte_state(volte_state_cmd->volte_mode_enable);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifndef CM_DEBUG
  /* Send the new preferences to Policy Manager and receive the new policy
  ** if it is changed.
  */
  pm_msgr_rsp = (cm_policyman_cfg_s*)
                      policyman_report_preference_change( volte_state_cmd->as_id, pmprx_pref_ptr);
 
  #endif
  if ( pm_msgr_rsp == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: New Volte Mode Enable have no effect on policy");
	cm_mem_free(pmprx_pref_ptr);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Policy changed so we need to retrieve the new policy data items.
  ** Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("PMPRX: unable to process policy as cannot allocate memory");
	cm_mem_free(pmprx_pref_ptr);
    return;
  }

  *local_policy = *(cmpm_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retrieve the policy data into a local buffer. If all the policy items in
  ** the new policy are successfully retrieved, only then update the current
  ** policy and attempt to enforce it.
  */
  return_val = cmpmprx_process_msgr_msg(pm_msgr_rsp,local_policy,&device_local_policy);

  CM_MSG_HIGH_1("device local policy %d",device_local_policy);

  /* If there is a pref change while ue is in voice all and the retrieved
  ** policy has ue_mode change, dont enforce the policy. Wait for it to arrive
  ** on message router message so it wont interfere with call.
  */
  if ( return_val == TRUE &&
       cmpmprx_is_ok_to_enforce_policy( local_policy, volte_state_cmd->as_id))
  {
    CM_MSG_HIGH_1("policy_chgd: volte_state_noti, asubs_id=%d", 
                   volte_state_cmd->as_id);
    
    cmpmprx_update_curr_policy(local_policy);
	/* enforce the current policy as well,
	as we need to do detach for volte on<->off transition */

	cmpmprx_check_and_enforce_policy();
  }

  #ifndef CM_DEBUG
  policyman_report_preference_msg_free( (msgr_hdr_s *)pm_msgr_rsp );
  #endif

  cm_mem_free(local_policy);
  cm_mem_free(pmprx_pref_ptr);

  return;
}

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

)
{
  cm_policy_config_s_type  *pm_info = cmpm_ptr();

  if (asubs_id >= SYS_MODEM_AS_ID_1 && 
      asubs_id < SYS_MODEM_AS_ID_NO_CHANGE &&
      asubs_id < ARR_SIZE(pm_info->current_policy))
  {
    return (pm_info->current_policy[asubs_id].sub_feature_mode);
  }
  else
  {
    return (pm_info->current_policy[SYS_MODEM_AS_ID_1].sub_feature_mode);
  }  
}

/*===========================================================================

FUNCTION cmpmprx_report_full_srv_lost

DESCRIPTION
  Reports to PM whenever Full Service Lost indication is received from SD.
  
===========================================================================*/

boolean cmpmprx_report_full_srv_lost(
  sys_modem_as_id_e_type     subs_id,
  sys_modem_stack_id_e_type  stack_id
  )
{
  if(!cmph_is_as_id_valid(subs_id))
  {
    return FALSE;
  }
  CM_MSG_HIGH_2("Reporting full_srv_lost to PM. subs_id = %d, stack_id = %d",subs_id,stack_id);
  policyman_report_acq_fail(subs_id,stack_id);
  return TRUE;
}

/*===========================================================================

FUNCTION cmpmprx_report_operational_radio_stacks

DESCRIPTION
  Reports to PM whenever there is change in operation flag on either sub/stack.
  
===========================================================================*/

boolean cmpmprx_report_operational_radio_stacks(
  sys_modem_as_id_e_type     subs_id
  )
{
  cmss_s_type *ss_ptr = cmss_ptr();
  uint32 stackMask=0;

  if(cmph_ptr()->is_sub2_map_to_hybr3)
  {
    if (subs_id == SYS_MODEM_AS_ID_1)
    {
      if(ss_ptr->info.is_main_operational)
      {
        stackMask |= BM(SYS_MODEM_STACK_ID_1);
      }
      if((ss_ptr->info.hdr_hybrid && cmph_is_sxlte_second_stack_allowed_on_mode(subs_id,CM_MODE_PREF_HDR_ONLY) ) || 
         (cmph_is_sxlte_second_stack_allowed_on_mode(subs_id, CM_MODE_PREF_LTE_ONLY) && ss_ptr->info.is_hybr_gw_operational ))
      {
        stackMask |= BM(SYS_MODEM_STACK_ID_2);
      }
    }
    else if (subs_id == SYS_MODEM_AS_ID_2)
    {
       if(ss_ptr->info.is_gw3_operational)
       {
         stackMask |= BM(SYS_MODEM_STACK_ID_1);
       }
    }
  }
  else
  {
    if (subs_id == SYS_MODEM_AS_ID_1)
    {
      if(ss_ptr->info.is_main_operational)
      {
        stackMask |= BM(SYS_MODEM_STACK_ID_1);
      }
      if(ss_ptr->info.hdr_hybrid && cmph_is_sxlte_second_stack_allowed_on_mode(subs_id,CM_MODE_PREF_HDR_ONLY))
      {
        stackMask |= BM(SYS_MODEM_STACK_ID_2);
      }
    }
    else if (subs_id == SYS_MODEM_AS_ID_2)
    {
      if(ss_ptr->info.is_hybr_gw_operational)
      {
        stackMask |= BM(SYS_MODEM_STACK_ID_1);
      }
    }
  }

    CM_MSG_HIGH_2("Reporting operational_radio_stacks to PM. subs_id = %d, stackMask=%d",subs_id,stackMask);
    
    policyman_report_operational_radio_stacks(subs_id, stackMask);
    return TRUE;
}
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
void cmpmprx_sanitize_max_and_curr_cap(cm_policy_config_s_type *pm_ptr)
{
  uint8   i         = 0;
  uint32  diff_bits = 0;
  
  for (; i< MIN(ARR_SIZE(pm_ptr->is_cm_eligible), ARR_SIZE(pm_ptr->current_policy)); i++)
  {
    // Error Fatal in case if Max Mode Capability is not super set of  capability
    diff_bits = pm_ptr->current_policy[i].policy_max_mode_capability ^ pm_ptr->current_policy[i].policy_mode_capability;
   
    if ((cmpm_active_subs & BM(i)) && 
		   (pm_ptr->current_policy[i].policy_mode_capability & diff_bits))
    {
      CM_MSG_HIGH_3(" Sanity fail for sub %d! PM provided incorrect MMC CMC !! max_mode_cap 0x%x, curr_mode_cap 0x%x", i, pm_ptr->current_policy[i].policy_max_mode_capability,
            pm_ptr->current_policy[i].policy_mode_capability);
      
      CM_ERR_FATAL_0("PM provided incorrect max_mode_cap and curr_mode_cap Error!! ");
      
    }
  }
}

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
void    cmpmprx_check_and_enforce_policy(void)
{
  cm_policy_config_s_type  *pm_info = cmpm_ptr();
  uint8 i=0;
  

  /* Enforce device level policy */
  if(pm_info->device_policy_changes != 0)
  {
    cmpmprx_enforce_device_policy(PM_CHG_DEVICE_ALL);
  }
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Loop thru the subs_ids and see if any policy changed. If yes then enforce*/
  /* If any policy item has changed, attempt to enforce the new policy. */
  for (i=0;i<pm_info->device_mode.nSims && i<POLICY_ITEM_ARRAY_SIZE;i++)
  {
    if ( cmpmprx_is_mask_set( i, PM_CHG_ALL, pm_info ) )
    {
      cmpmprx_enforce_current_policy(i);
    }
  } 

  return;
}/*===========================================================================

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
)
{
   if(asubs_id == SYS_MODEM_AS_ID_1 ||
      asubs_id == SYS_MODEM_AS_ID_2 || 
      asubs_id == SYS_MODEM_AS_ID_3
     )
   {
     return cmpm_ptr()->current_policy[asubs_id].policy_mode_capability;
   }
   else
   {
     return (uint32)cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_GSM);
   }
}

/*===========================================================================

FUNCTION cmpmprx_is_item_value_changed

DESCRIPTION
  Check if the value of any specific item on subs is really changed.

DEPENDENCIES
None

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmpmprx_is_item_value_changed(
  cm_policy_config_s_type *curr_policy, 
  cm_policy_config_s_type *local_policy, 
  policyman_item_id_t      item, 
  sys_modem_as_id_e_type   sub_id)
{
  switch(item)
  {
    
    case POLICYMAN_ITEM_RAT_CAPABILITY:
      if (curr_policy->current_policy[sub_id].policy_mode_capability != 
           local_policy->current_policy[sub_id].policy_mode_capability)
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_RF_BANDS:
      if ((curr_policy->current_policy[sub_id].policy_band_capability != 
           local_policy->current_policy[sub_id].policy_band_capability) ||
          (curr_policy->current_policy[sub_id].policy_tds_band_capability != 
           local_policy->current_policy[sub_id].policy_tds_band_capability) ||
          !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(
            &(curr_policy->current_policy[sub_id].policy_lte_band_capability),
            &(local_policy->current_policy[sub_id].policy_lte_band_capability))

           )
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_SVC_MODE:
      if (curr_policy->current_policy[sub_id].service_scan_mode != 
          local_policy->current_policy[sub_id].service_scan_mode)
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_UE_MODE:
      if (curr_policy->current_policy[sub_id].ue_operation_mode != 
          local_policy->current_policy[sub_id].ue_operation_mode)
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_VOICE_DOMAIN_PREF:
      if ((curr_policy->current_policy[sub_id].voice_domain_pref != 
           local_policy->current_policy[sub_id].voice_domain_pref) ||
          (curr_policy->current_policy[sub_id].is_voice_dom_pref_locked != 
           local_policy->current_policy[sub_id].is_voice_dom_pref_locked))
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_RAT_ACQ_ORDER:
      if (memcmp(&(curr_policy->current_policy[sub_id].acq_order_pref),
                  &(local_policy->current_policy[sub_id].acq_order_pref),
                  sizeof(cm_acq_pri_order_pref_s_type)))
      {
        return TRUE;
      }
      break;

    case POLICYMAN_ITEM_CALL_MODE:
      if (curr_policy->current_policy[sub_id].call_mode != 
          local_policy->current_policy[sub_id].call_mode)
      {
        return TRUE;
      }
      break;
      
    case POLICYMAN_ITEM_DEVICE_CONFIGURATION :
    default:
      return TRUE;
      break;
  }

  return FALSE;
}

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

)
{
  cm_policy_config_s_type *local_policy = NULL;
  cm_policyman_cfg_s      *pm_msgr_rsp  = NULL;
  policy_change_mask       device_local_policy = 0;
  policyman_session_avail_enum_t notifyType;
  cm_policy_config_s_type  * pm_ptr = cmpm_ptr();

  /* Allocate local buffer for new memory and initialize it to current policy.
  */
  local_policy = (cm_policy_config_s_type *) cm_mem_malloc(sizeof(cm_policy_config_s_type));
  if ( local_policy == NULL )
  {
    CM_MSG_HIGH_0("ERROR!!! CMPMPRX INIT: unable to process policy as cannot allocate memory");
    return;
  }

  *local_policy = *(cmpm_ptr());

  if(isAvailable)
  {
    notifyType = SESSION_AVAILABLE;
  }
  else
  {
    notifyType = SESSION_NOT_AVAILABLE;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Report session availability and retrieve new policy */

  CM_MSG_HIGH_3("Report session info, notifyType %d sessionType %d appType %d", notifyType, sessionType, appType);
  if((pm_msgr_rsp = (cm_policyman_cfg_s*)
                   policyman_report_session_availability(notifyType, sessionType, appType)))
  {
    if(cmpmprx_process_msgr_msg(pm_msgr_rsp, local_policy, &device_local_policy))
    {
      CM_MSG_HIGH_0("Updating curr policy");
      cmpmprx_update_curr_policy(local_policy);
    }
  }

  cm_mem_free(local_policy);
  
}

