/**
  @file policyman_policies.c

  @brief 1. enumerate and manage loaded policies
            2. Manage suspend resume framework
*/

/*
    Copyright (c) 2015,2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_policies.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre.h"
#include "mre_efs.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_engine.h"


#include "policyman_call_events.h"
#include "policyman_call_mode.h"
#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_fullrat_config.h"
#include "policyman_network_events.h"
#include "policyman_plmn.h"
#include "policyman_policy.h"
#include "policyman_policies.h"
#include "policyman_rat_capability.h"
#include "policyman_rat_order.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_tech.h"
#include "policyman_timer.h"
#include "policyman_ue_mode.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_voice_domain_pref.h"
#include "policyman_volte.h"
#include "policyman_xml.h"
#include "policyman_scan_optimization.h"
#include "policyman_frequency_list.h"

#include <stringl/stringl.h>


/*=============================================================================
  Mappings between condition tags and the functions to create a new condition
  from the XML document.
=============================================================================*/


STATIC mre_condition_map_element_t const  policyman_condition_map[] =
{
  /*---------------------------------------------------------------------------
    Serving system conditions
  ---------------------------------------------------------------------------*/
  {"service_status_in",       policyman_condition_service_status_new,             POLICYMAN_PRECOND_NONE},
  {"srv_domain_has",          policyman_condition_serving_domain_has_new,         POLICYMAN_PRECOND_NONE},
  {"serving_mcc_in",          policyman_condition_serving_mcc_new,                POLICYMAN_PRECOND_SS},
  {"serving_plmn_in",         policyman_condition_serving_plmn_new,               POLICYMAN_PRECOND_SS},
  {"serving_rat_in",          policyman_condition_serving_rat_new,                POLICYMAN_PRECOND_SS},
  {"serving_band_in",         policyman_condition_serving_rf_band_new,            POLICYMAN_PRECOND_SS},
  {"reg_reject_cause_in",     policyman_condition_reg_reject_new,                 POLICYMAN_PRECOND_SS},
  {"oos_scan_cnt",            policyman_condition_oos_scan_cnt_new,               POLICYMAN_PRECOND_NONE},
  {"location_mcc_in",         policyman_condition_location_mcc_new,               POLICYMAN_PRECOND_NONE},
  {"have_service",            policyman_condition_have_service_new,               POLICYMAN_PRECOND_NONE},
  {"have_location",           policyman_condition_have_location_new,              POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    UIM conditions
  ---------------------------------------------------------------------------*/
  {"imsi_plmn_in",            policyman_condition_imsi_plmn_new,                  POLICYMAN_PRECOND_IMSI},
  {"imsi_mcc_in",             policyman_condition_imsi_mcc_new,                   POLICYMAN_PRECOND_IMSI},
  {"sim_type",                policyman_condition_sim_type_new,                   POLICYMAN_PRECOND_SIMTYPE},
  {"imei",                    policyman_condition_imei_new,                       POLICYMAN_PRECOND_NONE},
  {"sim_refreshed",           policyman_condition_sim_refresh_new,                POLICYMAN_PRECOND_IMSI},

  /*---------------------------------------------------------------------------
    Current state conditions
  ---------------------------------------------------------------------------*/
  {"ue_mode_is",              policyman_condition_ue_mode_new,                    POLICYMAN_PRECOND_NONE},
  {"svc_mode_is",             policyman_condition_svc_mode_new,                   POLICYMAN_PRECOND_NONE},
  {"num_subs",                policyman_condition_num_subs_new,                   POLICYMAN_PRECOND_NONE},
  {"enforce_full_rat",        policyman_condition_enforce_full_rat_new,           POLICYMAN_PRECOND_NONE},
  {"is_subs_dds",             policyman_condition_is_subs_dds_new,                POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    User preference conditions
  ---------------------------------------------------------------------------*/
  {"user_domain_pref",        policyman_condition_ph_domain_pref_new,             POLICYMAN_PRECOND_NONE},
  {"user_mode_pref_contains", policyman_condition_ph_rat_pref_new,                POLICYMAN_PRECOND_NONE},
  {"user_mcc_in",             policyman_condition_ph_user_mcc_new,                POLICYMAN_PRECOND_NONE},
  {"network_selection_mode",  policyman_condition_ph_network_selection_mode_new,  POLICYMAN_PRECOND_NONE},
  {"voice_domain_pref_is",    policyman_condition_voice_domain_pref_new,          POLICYMAN_PRECOND_NONE},
  {"call_mode_is",            policyman_condition_call_mode_new,                  POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Timer conditions
  ---------------------------------------------------------------------------*/
  {"timer_expired",           policyman_condition_timer_expired_new,              POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Phone Operating Mode conditions
  ---------------------------------------------------------------------------*/
  {"phone_operating_mode",    policyman_condition_ph_operating_mode_new,          POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Technology conditions
  ---------------------------------------------------------------------------*/
  {"tech_loaded",             policyman_condition_tech_loaded_new,                POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Call conditions
  ---------------------------------------------------------------------------*/
  {"in_call",                 policyman_condition_call_event_new,                 POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Network Search conditions
  ---------------------------------------------------------------------------*/
  {"scan_results",            policyman_condition_scan_results_new,               POLICYMAN_PRECOND_NONE},
  {"network_scan_active",     policyman_condition_network_scan_active_new,        POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Special conditions
  ---------------------------------------------------------------------------*/
  {"volte_enabled",           policyman_condition_volte_enabled_new,              POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL, NULL, POLICYMAN_PRECOND_NONE}
};



/*=============================================================================
  Mappings between action tags and the functions to create a new action
  from the XML document.
=============================================================================*/

STATIC mre_action_map_element_t const  policyman_action_map[] =
{
  /*  Configuration item actions
   */
  {"device_configuration",    policyman_device_config_action_new},
  {"device_configuration_if", policyman_device_config_if_action_new},
  {"define_config",           policyman_device_config_define_new},
  {"use_config",              policyman_device_config_use_new},
  {"use_config_if",           policyman_device_config_use_if_new},
  {"rat_capability",          policyman_rat_capability_action_new},
  {"rat_capability_if",       policyman_rat_capability_if_action_new},
  {"ue_mode",                 policyman_ue_mode_action_new},
  {"ue_mode_if",              policyman_ue_mode_if_action_new},
  {"rf_bands",                policyman_rf_bands_action_new},
  {"rf_bands_if",             policyman_rf_bands_if_action_new},
  {"svc_mode",                policyman_svc_mode_action_new},
  {"feature",                 policyman_device_config_feature_new},
  {"voice_domain_pref",       policyman_voice_domain_pref_action_new},
  {"rat_order",               policyman_rat_order_action_new},
  {"call_mode",               policyman_call_mode_action_new},
  {"call_mode_if",            policyman_call_mode_if_action_new},
  {"volte",                   policyman_volte_action_new},
  {"scan_optimization",       policyman_scan_opt_action_new},
  {"specialization",          policyman_device_config_specialization_new},
  {"select_config",           policyman_device_config_select_config_new},
  {"freq_list",               policyman_freq_list_action_new},
  {"freq_list_if",            policyman_freq_list_if_action_new},

  /*  Actions to define named objects
   */
  {"plmn_list",               policyman_plmn_list_new},
  {"rf_band_list",            policyman_rf_band_list_action_new},
  {"mcc_list",                policyman_mcc_list_new},
  {"define_fullrat_config",   policyman_fullrat_config_new},
  {"fullrat_enter",           policyman_fullrat_enter_new},

  /*---------------------------------------------------------------------------
    Timer actions
  ---------------------------------------------------------------------------*/
  {"define_timer",            policyman_named_timer_action_new},
  {"expired_timer_handled",   policyman_expired_timer_handled_new},
  {"timer_start",             policyman_timer_start_new},
  {"timer_stop",              policyman_timer_stop_new},

  /*  Actions that affect state of system
   */
  {"exclude_tech",            policyman_tech_exclude_new},

  /* Network Scan actions
   */
  {"network_scan",            policyman_network_scan_action_new},

  /* UIM actions
   */
  {"sim_refresh_reset",      policyman_uim_sim_refresh_reset_action_new},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL, NULL}
};


/*=============================================================================
  Mappings between precondition tags and their corresponding bitmasks
=============================================================================*/

STATIC mre_precond_map_element_t policyman_precond_map[] =
{
  {"IMSI",        POLICYMAN_PRECOND_IMSI},
  {"SS",          POLICYMAN_PRECOND_SS},
  {"SIM_TYPE",    POLICYMAN_PRECOND_SIMTYPE},
  {"CDMASIMTYPE", POLICYMAN_PRECOND_CDMASIMTYPE},
  {NULL,          0}
};

STATIC  policyman_set_t       *g_pPolicySet;
STATIC  policyman_set_t       *g_pGlobalPolicySet;
STATIC  policyman_set_t       *g_pSubsPolicySet;
STATIC  boolean                g_policy_is_initialized;
STATIC  mre_namespace_info_t   g_namespaceInfo;
STATIC  policyman_crit_sect_t  g_suspendResumeCS;
STATIC  mre_set_t             *g_pSuspendClientSet;


/*-----------------------------------------------------------------------------
  Policyman suspend/resume framework
-----------------------------------------------------------------------------*/

/*-------- compare_clients --------*/
STATIC boolean compare_clients(
  void  *pElem1,
  void  *pElem2
)
{
  policyman_client_t  client1 = *((policyman_client_t *) pElem1);
  policyman_client_t  client2 = *((policyman_client_t *) pElem2);

  return client1 == client2;
}


/*-------- policyman_init_suspend_resume_framework --------*/
STATIC void policyman_init_suspend_resume_framework(
  void
)
{
  policyman_init_crit_sect(&g_suspendResumeCS);

  policyman_enter_crit_sect(&g_suspendResumeCS);
  g_pSuspendClientSet = mre_set_new(  sizeof(policyman_client_t),
                                      5,
                                      MRE_SET_NO_MAX_SIZE,
                                      compare_clients,
                                      NULL );
  policyman_leave_crit_sect(&g_suspendResumeCS);
}


/*-------- policyman_deinit_suspend_resume_framework --------*/
STATIC void policyman_deinit_suspend_resume_framework(
  void
)
{
  policyman_deinit_crit_sect(&g_suspendResumeCS);
  REF_CNT_OBJ_RELEASE_IF(g_pSuspendClientSet);
}


/*-------- policyman_is_suspended_by_clients --------*/
STATIC boolean policyman_is_suspended_by_clients(
  void
)
{
  return !mre_set_is_empty(g_pSuspendClientSet);
}


/*-------- policyman_is_suspended --------*/
boolean policyman_is_suspended(
  void
)
{
  return policyman_is_suspended_by_clients();
}

/*-------- try_suspend --------*/
STATIC boolean  try_suspend(
  policyman_client_t  client
)
{
  policyman_dbupdate_state     state;
  boolean                      suspended;

  policyman_enter_crit_sect(&g_suspendResumeCS);

  state = policyman_dbupdate_state_get();
  suspended =     (POLICYMAN_DBUPDATE_STATE_SUSPENDED == state)
              ||  policyman_dbupdate_try_enter_suspend_state() ;

  if (suspended)
  {
    mre_set_add(g_pSuspendClientSet, &client);
  }

  policyman_leave_crit_sect(&g_suspendResumeCS);

  return suspended;
}

/*-------- policyman_suspend_by_client --------*/
void policyman_suspend_by_client(
  policyman_client_t  client
)
{
  size_t  numClients;

  // return if in test mode
  if (policyman_is_test_mode())
  {
    POLICYMAN_MSG_HIGH_0("policyman_suspend_by_client.. test mode is enabled ..not suspending");
    return;
  }

  /* Block the client if Policyman DB update is in progress
     Wait till DB update completes and try_suspend returns true
  */
  {
    boolean suspended = FALSE;

    while (!suspended)
    {
      suspended = try_suspend(client);
      if (!suspended)
      {
        policyman_dbupdate_block_signal_wait();
      }
    }

  numClients = mre_set_num_elems(g_pSuspendClientSet);

  POLICYMAN_MSG_HIGH_2( "policyman suspended by client %d; %d clients have suspended",
                        client,
                        numClients );
  }
}


/*------- policyman_execute_process_resume --------*/
STATIC void policyman_execute_process_resume(
  policyman_cmd_t *pCmd
)
{
  policyman_state_handle_update(pCmd->pState);
}

/*-------- policyman_resume_internal --------*/
size_t policyman_resume_internal(
  policyman_client_t  client
)
{
  size_t           numClients;

  policyman_enter_crit_sect(&g_suspendResumeCS);
  mre_set_remove(g_pSuspendClientSet, &client);
  numClients = mre_set_num_elems(g_pSuspendClientSet);
  policyman_leave_crit_sect(&g_suspendResumeCS);

  if (numClients > 0)
  {
    POLICYMAN_MSG_HIGH_2( "policyman resumed by client %d; still suspended by %d clients",
                          client,
                          numClients );
  }
  else
  {
    POLICYMAN_MSG_HIGH_1("policyman resumed by client %d", client);
  }

  return numClients;  
}

/*-------- policyman_resume_by_client --------*/
void policyman_resume_by_client(
  policyman_client_t  client
)
{
  size_t           numClients;
  policyman_cmd_t *pCmd = NULL;

  if (policyman_is_test_mode())
  {
    POLICYMAN_MSG_HIGH_0("policyman_resume_by_client : test mode is enabled");
    return;
  }

  numClients = policyman_resume_internal(client);


  if (numClients == 0)
  {
    pCmd = POLICYMAN_CMD_NEW( policyman_cmd_t,
                              policyman_execute_process_resume,
                              NULL,
                              NULL );

    policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
    ref_cnt_obj_release(pCmd);

    policyman_dbupdate_enter_idle_state( );
  }
}


/*-------- policyman_suspend --------*/
void policyman_suspend(
  void
)
{
  policyman_suspend_by_client(POLICYMAN_CLIENT_CM);
}

/*-------- policyman_resume--------*/
void policyman_resume(
  void
)
{
  policyman_resume_by_client(POLICYMAN_CLIENT_CM);
}

/*-------- policyman_get_namespace_info --------*/
mre_namespace_info_t  *policyman_get_namespace_info(
  void
)
{
  return &g_namespaceInfo;
}


/*-------- policyman_namespace_info_init --------*/
STATIC void policyman_namespace_info_init(
  void
)
{
  memset(&g_namespaceInfo, 0, sizeof(g_namespaceInfo));

  g_namespaceInfo.defaultNS         = MRE_NAMESPACE_SUBS;
  g_namespaceInfo.pfnGetPolicySubs  = policyman_policy_get_subs;
}


/*-------- policyman_namespace_info_per_subs_deinit --------*/
STATIC void policyman_namespace_info_per_subs_deinit(
  sys_modem_as_id_e_type subsId
)
{
  REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.perSubsNamedObjs[subsId]);
}


/*-------- policyman_namespace_info_deinit --------*/
STATIC void policyman_namespace_info_deinit(
  void
)
{
  size_t  subs;

  for (subs = 0 ; subs < ARR_SIZE(g_namespaceInfo.perSubsNamedObjs) ; ++subs)
  {
    REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.perSubsNamedObjs[subs]);
  }
  REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.pGlobalNamedObjs);
}

/*-------- policyman_get_policy --------*/
STATIC policyman_policy_t * policyman_get_policy(
  size_t n
)
{
  policyman_policy_t  **ppPolicy = NULL;
  size_t                gNumElem = 0;
  size_t                sNumElem = 0;

  gNumElem = mre_set_num_elems(g_pGlobalPolicySet);
  sNumElem = mre_set_num_elems(g_pSubsPolicySet);

  if (    (g_pPolicySet != NULL)
       && (mre_set_num_elems(g_pPolicySet) > 0)
       && (gNumElem > 0 || sNumElem > 0) )
  {
    if (n < gNumElem)
    {
      ppPolicy = (policyman_policy_t **) mre_set_elem_ptr(g_pGlobalPolicySet, n);
    }
    else
    {
      n -= gNumElem;
      ppPolicy = (policyman_policy_t **) mre_set_elem_ptr(g_pSubsPolicySet, n);
    }
  }

  return (ppPolicy == NULL) ? NULL : *ppPolicy;
}


/*-------- policyman_global_policy_files --------*/
STATIC char const  *policyman_global_policy_files[] =
{
  POLICYMAN_DEVICE_CONFIG_XML_NAME,
};


/*-------- policyman_subs_policy_files --------*/
STATIC char const  *policyman_subs_policy_files[] =
{
  POLICYMAN_CARRIER_POLICY_XML_NAME,
  POLICYMAN_RESTRICTIONS_XML_NAME,
  POLICYMAN_SEGMENT_LOADING_XML_NAME,
};


/*-------- policyman_policies_file_enum_init --------*/
void policyman_policies_file_enum_init(
  policyman_policy_enum_ctx *pCtx
)
{
  *pCtx = 0;
}

/*-------- policyman_policy_global_file_enum_next --------*/
boolean policyman_policy_global_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
)
{
  policyman_policy_enum_ctx index     = *pCtx;
  size_t                    arraySize = ARR_SIZE(policyman_global_policy_files);
  boolean                   havePath  = FALSE;

  POLICYMAN_UT_MSG_1("policyman_policies_global_file_enum_next: processing file index %d", index);

  /* We have more XMLs to look into under different folders
        Ensure folder name string fits well into the buffer space
   */
  if (index < arraySize)
  {
    snprintf(pFileName, nameLength, POLICYMAN_EFS_ROOT_DIR"/%s", policyman_global_policy_files[index]);
    havePath = TRUE;
  }

  *pCtx = index + 1;
  return havePath;
}

/*-------- policyman_policies_file_enum_next --------*/
boolean policyman_policies_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
)
{
  policyman_policy_enum_ctx index     = *pCtx;
  size_t                    arraySize = ARR_SIZE(policyman_subs_policy_files);
  boolean                   havePath  = FALSE;

  POLICYMAN_UT_MSG_1("policyman_policies_file_enum_next: processing file index %d", index);

  /* We have more XMLs to look into under different folders
        Ensure folder name string fits well into the buffer space
   */
  if (index < arraySize)
  {
    snprintf(pFileName, nameLength, POLICYMAN_EFS_ROOT_DIR"/%s", policyman_subs_policy_files[index]);
    havePath = TRUE;
  }

  *pCtx = index + 1;
  return havePath;
}

/*-------- policyman_policy_load_global --------*/
void policyman_policy_load_global(
  policyman_state_t        *pState,
  policyman_set_t          *pItemSet  
)
{
  policyman_policy_enum_ctx  fileCtx;
  char                       fileName[128] = {0};

  if (policyman_is_test_mode())
  {
    return ;
  }

  policyman_policies_file_enum_init(&fileCtx);

  while (policyman_policy_global_file_enum_next(&fileCtx, fileName, sizeof(fileName)))
  {
    mre_status_t               status;
    policyman_policy_t        *pPolicy           = NULL;

    status = mre_policy_create( fileName,
                                SYS_MODEM_AS_ID_NONE,
                                policyman_condition_map,
                                policyman_action_map,
                                policyman_precond_map,
                                policyman_policy_new,
                                (mre_policy_t **) &pPolicy );

    if (MRE_STATUS_SUCCESS == status)
    {
      POLICYMAN_MSG_S1( "Loaded global policy file %s",
                        fileName );

      policyman_set_add(g_pGlobalPolicySet, &pPolicy);
      ref_cnt_obj_release(pPolicy);

      policyman_policy_initialize(pPolicy, pState, pItemSet);
    }
  }

}


/*-------- policyman_policy_load_subs --------*/
void policyman_policy_load_subs(
  policyman_state_t        *pState,
  sys_modem_as_id_e_type    policySubsId,
  policyman_set_t          *pItemSet 
)
{
  policyman_policy_enum_ctx  fileCtx;
  char                       fileName[128] = {0};

  policyman_policies_file_enum_init(&fileCtx);

  while (policyman_policies_file_enum_next(&fileCtx, fileName, sizeof(fileName)))
  {
    mre_status_t               status;
    policyman_policy_t        *pPolicy           = NULL;

    status = mre_policy_create( fileName,
                                policySubsId,
                                policyman_condition_map,
                                policyman_action_map,
                                policyman_precond_map,
                                policyman_policy_new,
                                (mre_policy_t **) &pPolicy );

    if (MRE_STATUS_SUCCESS == status)
    {
      POLICYMAN_MSG_S2( SUBS_PREFIX "Loaded policy file %s",
                        policySubsId,
                        fileName );

      policyman_set_add(g_pSubsPolicySet, &pPolicy);
      ref_cnt_obj_release(pPolicy);

      policyman_policy_initialize(pPolicy, pState, pItemSet);
    }
  }

}

/*-------- policyman_policies_init --------*/
void policyman_policies_init(
  policyman_state_t *pState,
  policyman_set_t   *pItemSet  
)
{
  sys_modem_as_id_e_type  subsId;

  /* Return FALSE if policy already initialized
   */
  if (g_policy_is_initialized || policyman_is_test_mode())
  {
    return;
  }

  policyman_namespace_info_init();
  policyman_init_suspend_resume_framework();

  
  /*  Create the sets to hold the policies.
   */
  g_pPolicySet       = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);
  g_pGlobalPolicySet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);
  g_pSubsPolicySet   = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);

}



/*-------- policyman_set_policy_initialized --------*/
void policyman_set_policy_initialized(
  boolean  is_initialized
)
{
  g_policy_is_initialized = is_initialized;
}


/*-------- policyman_policy_init_is_complete --------*/
void policyman_policy_init_is_complete(
  void 
)
{
  if (policyman_is_test_mode())
  {
    return;
  }
  
  if(!g_policy_is_initialized)
  {
    policyman_set_add(g_pPolicySet, &g_pGlobalPolicySet);
    policyman_set_add(g_pPolicySet, &g_pSubsPolicySet);
    g_policy_is_initialized = TRUE;
  }
}


/*-------- policyman_sub_policies_init --------*/
void policyman_sub_policies_init(
  policyman_state_t *pState,
  policyman_set_t   *pItemSet  
)
{
  sys_modem_as_id_e_type  subsId;
  
  if (g_policy_is_initialized)
  {
    return;
  }

  for (subsId = SYS_MODEM_AS_ID_1; subsId < POLICYMAN_NUM_SUBS; ++subsId)
  {
    policyman_policies_per_subs_init(pState, pItemSet, subsId);
  }
}


/*-------- policyman_policies_per_subs_init --------*/
void  policyman_policies_per_subs_init(
  policyman_state_t      *pState,
  policyman_set_t        *pItemSet,
  sys_modem_as_id_e_type  subsId
)
{
  if (policyman_is_test_mode())
  {
    return;
  }

  if (!g_policy_is_initialized)
  {
    policyman_policy_load_subs(pState, subsId, pItemSet);
  }
}


/*-------- policyman_policies_deinit --------*/
void
policyman_policies_deinit(
  void
  )
{
  g_policy_is_initialized = FALSE;
  policyman_deinit_suspend_resume_framework();
  policyman_namespace_info_deinit();

  REF_CNT_OBJ_RELEASE_IF(g_pGlobalPolicySet);
  REF_CNT_OBJ_RELEASE_IF(g_pSubsPolicySet);
  REF_CNT_OBJ_RELEASE_IF(g_pPolicySet);
}




typedef struct
{
  sys_modem_as_id_e_type  subsId;
  mre_set_t              *pItemSet;
} policy_remove_ctx_t;


/*-------- policyman_policy_to_remove --------*/
STATIC boolean policyman_policy_to_remove(
  void  *pElem,
  void  *pCtx
)
{
  policyman_policy_t  *pPolicy    = *((policyman_policy_t **) pElem);
  policy_remove_ctx_t *pRemoveCtx = (policy_remove_ctx_t *) pCtx;

  if (pRemoveCtx->subsId == pPolicy->subs)
  {
    policyman_set_add(pRemoveCtx->pItemSet, &pPolicy);
  }

  return TRUE;
}


/*-------- policyman_policy_remove --------*/
STATIC boolean policyman_policy_remove(
  void  *pElem,
  void  *pCtx
)
{
  policyman_policy_t *pPolicy    = *((policyman_policy_t **) pElem);
  policyman_set_t    *pPolicySet = (policyman_set_t *) pCtx;

  policyman_set_remove(pPolicySet, &pPolicy);

  REF_CNT_OBJ_RELEASE_IF(pPolicy);

  return TRUE;
}


/*-------- policyman_policies_per_subs_deinit --------*/
void policyman_policies_per_subs_deinit(
  sys_modem_as_id_e_type subsId
)
{
  policy_remove_ctx_t   ctx;
  policyman_set_t        *pRemoveSet;

  g_policy_is_initialized = FALSE;


  /*  Release any subs specific named object namespaces that were created.
   */
  policyman_namespace_info_per_subs_deinit(subsId);

  pRemoveSet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);
  ctx.subsId    = subsId;
  ctx.pItemSet  = pRemoveSet;

  /* Remove subs specific policies from the set
   */
  mre_set_iterate(g_pSubsPolicySet, policyman_policy_to_remove, &ctx);
  mre_set_iterate(pRemoveSet, policyman_policy_remove, g_pSubsPolicySet);

  REF_CNT_OBJ_RELEASE_IF(pRemoveSet);
}

/*-------- can_policy_execute --------*/
STATIC boolean can_policies_execute(
  void
)
{
  boolean testModeIsEnabled;
  boolean suspendedIsEnabled;
  boolean inLimitedCapability;

  testModeIsEnabled   = policyman_is_test_mode();
  suspendedIsEnabled  = policyman_is_suspended(); 
  inLimitedCapability = policyman_cfgitem_limited_capabilities_is_enabled();

  // Return FALSE if suspended, in test mode or in limited capabilities
  if (testModeIsEnabled || suspendedIsEnabled || inLimitedCapability)
  {
    POLICYMAN_MSG_ERROR_3( "Policies not executed: test mode is enabled %d,  policyman is suspended %d, in limited capabilities %d",
                           testModeIsEnabled, 
                           suspendedIsEnabled,
                           inLimitedCapability );
    return FALSE;
  }

  return TRUE;
}


typedef struct
{
  policy_execute_ctx_t  *pCtx;
  elem_iter_fn_t         pfn;
} policy_set_iterate_ctx_t;


/*-------- policyman_policy_set_iterate --------*/
STATIC boolean policyman_policy_set_iterate(
  void  *pElem,
  void  *pCtx
)
{
  policyman_set_t           *pPolicySet = *((policyman_set_t **) pElem);
  policy_set_iterate_ctx_t  *pIterCtx   = (policy_set_iterate_ctx_t *) pCtx;

  mre_set_iterate(pPolicySet, pIterCtx->pfn, pIterCtx->pCtx);

  return TRUE;
}


/*-------- policyman_policyset_execute --------*/
STATIC void policyman_policyset_execute(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  boolean                 haveService,
  elem_iter_fn_t          pfnIter
)
{
  policy_execute_ctx_t      ctx;
  policy_set_iterate_ctx_t  iterCtx;
  policyman_set_t          *pItemSet;

  pItemSet        = policyman_itemset_new();
  ctx.pState      = pState;
  ctx.pItemSet    = pItemSet;
  ctx.asubs_id    = subsId;

  iterCtx.pCtx    = &ctx;
  iterCtx.pfn     = pfnIter;

  mre_set_iterate(g_pPolicySet, policyman_policy_set_iterate, &iterCtx);
  policyman_fullrat_config_evaluate(&ctx);

  policyman_cfgitem_update_items(pItemSet, NULL);
  ref_cnt_obj_release(pItemSet);
}

/*-------- policyman_policies_run_policy_check --------*/
/**
@brief  Run a policy check and collect updates into the itemset passed.

@param

@return

*/
void policyman_policies_run_policy_check(
  policyman_state_t       *pState,
  policyman_set_t         *pItemSet
)
{
  if (can_policies_execute())
  {
    policy_execute_ctx_t      ctx;
    policy_set_iterate_ctx_t  iterCtx;

    ctx.pState    = pState;
    ctx.pItemSet  = pItemSet;
    ctx.asubs_id  = SYS_MODEM_AS_ID_NONE;

    iterCtx.pCtx    = &ctx;
    iterCtx.pfn     = policyman_policy_run;

    mre_set_iterate(g_pPolicySet, policyman_policy_set_iterate, &iterCtx);
    policyman_fullrat_config_evaluate(&ctx);
  }
}

/*-------- policyman_policy_handle_user_pref_update --------*/
void policyman_policy_handle_user_pref_update(
  policyman_state_t     *pState,
  sys_modem_as_id_e_type asubs_id
)
{
  if (can_policies_execute())
  {
    boolean             haveService = TRUE;

    policyman_policyset_execute(pState, asubs_id, haveService, policy_handle_user_pref_update);
  }
}

/*-------- policyman_evaluate_cond_for_this_subs --------*/
boolean policyman_evaluate_cond_for_this_subs(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
)
{
  return pfnEvaluate(pCond, pCtx, CONDITION_SUBS(pCond));
}

/*-------- policyman_evaluate_cond_for_other_subs --------*/
boolean policyman_evaluate_cond_for_other_subs(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
)
{
  sys_modem_as_id_e_type  subs;
  size_t                  nSim = 0;

  (void) policyman_get_current_num_sim(&nSim);

  for (subs = SYS_MODEM_AS_ID_1 ; subs < nSim ; ++subs)
  {
    if (   subs != CONDITION_SUBS(pCond)
        && pfnEvaluate(pCond, pCtx, subs)
       )
    {
      return TRUE;
    }
  }

  return FALSE;
}


/*-------- policyman_evaluate_cond_for_all_subs --------*/
boolean policyman_evaluate_cond_for_all_subs(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
)
{
  return    policyman_evaluate_cond_for_this_subs(pCond, pCtx, pfnEvaluate)
        &&  policyman_evaluate_cond_for_other_subs(pCond, pCtx, pfnEvaluate);
}

/*-------- policyman_evaluate_cond_for_any_subs --------*/
boolean policyman_evaluate_cond_for_any_subs(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
)
{
  return    policyman_evaluate_cond_for_this_subs(pCond, pCtx, pfnEvaluate)
        ||  policyman_evaluate_cond_for_other_subs(pCond, pCtx, pfnEvaluate);
}


/*-------- policyman_evaluate_cond --------*/
boolean policyman_evaluate_cond(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs       pfnEvaluate
)
{
  switch (pCond->pmSubsId)
  {
    case SUBS_THIS:
      return policyman_evaluate_cond_for_this_subs(pCond, pCtx, pfnEvaluate);
      break;

    case SUBS_OTHER:
      return policyman_evaluate_cond_for_other_subs(pCond, pCtx, pfnEvaluate);
      break;

    case SUBS_ALL:
      return policyman_evaluate_cond_for_all_subs(pCond, pCtx, pfnEvaluate);
      break;

    case SUBS_ANY:
      return policyman_evaluate_cond_for_any_subs(pCond, pCtx, pfnEvaluate);
      break;

    default:
      return FALSE;
      break;
  }
}


/*-------- policyman_execute_action_for_this_subs --------*/
boolean policyman_execute_action_for_this_subs(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
)
{
  return pfnExecute(pAction, pCtx, ACTION_SUBS(pAction));
}

/*-------- policyman_execute_action_for_other_subs --------*/
boolean policyman_execute_action_for_other_subs(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
)
{
  sys_modem_as_id_e_type  subs;
  size_t                  nSim = 0;

  (void) policyman_get_current_num_sim(&nSim);

  for (subs = SYS_MODEM_AS_ID_1 ; subs < nSim ; ++subs)
  {
    if (   subs != ACTION_SUBS(pAction)
        && pfnExecute(pAction, pCtx, subs)
       )
    {
      return TRUE;
    }
  }

  return FALSE;
}


/*-------- policyman_execute_action_for_all_subs --------*/
boolean policyman_execute_action_for_all_subs(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
)
{
  return    policyman_execute_action_for_this_subs(pAction, pCtx, pfnExecute)
        &&  policyman_execute_action_for_other_subs(pAction, pCtx, pfnExecute);
}


/*-------- policyman_execute_action --------*/
boolean policyman_execute_action(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs   pfnExecute
)
{
  switch (pAction->pmSubsId)
  {
    case SUBS_THIS:
      return policyman_execute_action_for_this_subs(pAction, pCtx, pfnExecute);
      break;

    case SUBS_OTHER:
      return policyman_execute_action_for_other_subs(pAction, pCtx, pfnExecute);
      break;

    case SUBS_ALL:
      return policyman_execute_action_for_all_subs(pAction, pCtx, pfnExecute);
      break;

    case SUBS_ANY:  // Can't run action on unspecified subs
    default:
      return FALSE;
      break;
  }

}

/*-------- policyman_condition_new --------*/
policyman_condition_t * policyman_condition_new(
  size_t                 condSize,
  mre_policy_t          *pPolicy,
  pfn_evaluate_t         pEvaluate,
  destructor_ptr         pDtor,
  policyman_subs_t         pmSubsId,
  mre_xml_element_t const *pElem
)
{
  policyman_condition_t *pCondition;

  pCondition = (policyman_condition_t *)mre_condition_new( condSize,
                                                           (mre_policy_t *)pPolicy,
                                                           pEvaluate,
                                                           pDtor,
                                                           pElem );
  pCondition->pmSubsId = pmSubsId;

  return pCondition;
}

/*-------- policyman_action_new --------*/
policyman_action_t * policyman_action_new(
  size_t                   actionSize,
  mre_policy_t            *pPolicy,
  pfn_execute_t            pExecute,
  destructor_ptr           pDtor,
  policyman_subs_t         pmSubsId,
  mre_xml_element_t const *pElem
)
{
  policyman_action_t *pAction;

  pAction = (policyman_action_t *)mre_action_new( actionSize,
                                                  (mre_policy_t *)pPolicy,
                                                  pExecute,
                                                  pDtor,
                                                  pElem );
  pAction->pmSubsId = pmSubsId;

  return pAction;
}

/*-------- policyman_policies_gather_statistics --------*/
void policyman_policies_gather_statistics(
  size_t                 policy_idx,
  pm_dump_policy_info_t *pPolicyInfo
)
{
  policyman_policy_t   *pPolicy;

  pPolicy = policyman_get_policy(policy_idx);

  if ( (NULL != pPolicy) && (NULL != pPolicyInfo) )
  {
    mre_rules_gather_policy_statistics((mre_policy_t *) pPolicy, (mre_dump_policy_info_t *) pPolicyInfo);
    pPolicyInfo->suspend_count   = mre_set_num_elems(g_pSuspendClientSet);
    pPolicyInfo->is_policy_init  = g_policy_is_initialized;
  }
}
