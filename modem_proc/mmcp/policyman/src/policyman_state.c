/**
  @file policyman_state.c

  @brief
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_state.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"
#include "policyman_cfgitem.h"
#include "policyman_call_events.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_phone_events.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_serving_system.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_util.h"
#include "policyman_uim.h"
#include "trm.h"


#include <stringl/stringl.h>



static policyman_device_info_t   policyman_device_info;
static policyman_subs_state_t    policyman_subs_state_info[POLICYMAN_NUM_SUBS];
static policyman_global_info_t   policyman_global_info;

static policyman_state_t         policyman_state;

static char const               *pIMEI;
static byte                      rawIMEI[9];
static uint8                     trm_cap;

static policyman_crit_sect_t     policyman_state_crit_sect;

void  policyman_state_imei_init(void);


/*=============================================================================
  General state APIs
=============================================================================*/

/*-------- policyman_state_retrieve_persisted_current_mcc --------*/
STATIC boolean policyman_state_retrieve_persisted_current_mcc(
  sys_mcc_type           *pCurrentMcc,
  sys_modem_as_id_e_type  subsId
)
{
  mre_efs_status_t      status;
  sys_mcc_type         *pMcc       = NULL;
  size_t                mccSize    = 0;

  /* Initialize Current MCC to 0
   */
  *pCurrentMcc = 0;

  /*  Try and read from the NV item for Current MCC.
   */
  status = mre_efs_get_file( POLICYMAN_CURRENT_MCC_NV_PATH,
                             (void **)&pMcc,
                             &mccSize,
                             subsId);

  /*  If we were able to read the file set the MCC to that value.
         Otherwise just return.
   */
  if (    status == MRE_EFS_STATUS_SUCCESS
      &&  mccSize == sizeof(sys_mcc_type)
      &&  pMcc != NULL
     )
  {
    *pCurrentMcc = *pMcc;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "current_mcc %d read from EFS",
                          subsId,
                          *pCurrentMcc );
  }

  POLICYMAN_MEM_FREE_IF(pMcc);
  return *pCurrentMcc != 0;
}

/*-------- policyman_state_notify_history_init --------*/
void policyman_state_notify_history_init(
  void
)
{
  policyman_cfgitem_init_notify_history(policyman_state_get_state());
}

/*-------- policyman_state_init --------*/
void policyman_state_init(
  void
)
{
  size_t subs;

  policyman_init_crit_sect(&policyman_state_crit_sect);

  /* Init Policyman device state, subs state and global state
  */
  policyman_state.pDeviceInfo = &policyman_device_info;
  memset(policyman_state.pDeviceInfo, 0x00, sizeof(policyman_device_info_t));

  policyman_state.pGlobalInfo = &policyman_global_info;
  memset(policyman_state.pGlobalInfo, 0x00, sizeof(policyman_global_info_t));

  policyman_state_clear_hlos_mcc(&policyman_state);

  for(subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
  {
    policyman_state.pSubsState[subs] = &policyman_subs_state_info[subs];
    memset(policyman_state.pSubsState[subs], 0x00, sizeof(policyman_subs_state_t));    
    policyman_state.pSubsState[subs]->simLockVerified = TRUE;
  }

  (void) policyman_state_ss_init(&policyman_state);
  (void) policyman_state_uim_init(&policyman_state);
  (void) policyman_state_cmcall_init(&policyman_state);
  (void) policyman_state_network_init(&policyman_state);

  /* Do SUBS init before Phone init since we need memory for subscription allocated before we
       evaluate multimode subscription
    */
  (void) policyman_state_subs_init(&policyman_state);
  (void) policyman_state_cmph_init(&policyman_state);

  (void) policyman_state_notify_history_init();
}


/*-------- policyman_state_static_init --------*/
void policyman_state_static_init(
void
)
{
  policyman_state_imei_init();
  policyman_state_trm_cap_init();
}


/*-------- policyman_state_deinit --------*/
void policyman_state_deinit(
  void
  )
{
  size_t i;

  policyman_subs_deinit(policyman_state.pDeviceInfo->pSubsInfo);

  for(i=0; i<POLICYMAN_NUM_SUBS; i++)
  {
    policyman_cmph_deinit(policyman_state.pSubsState[i]->pCmPhInfo);
    policyman_network_deinit(policyman_state.pSubsState[i]->pNetworkInfo);
    policyman_cmcall_deinit(policyman_state.pSubsState[i]->pCmCallInfo);
    policyman_uim_deinit(policyman_state.pSubsState[i]->pUimInfo);
    policyman_ss_deinit(policyman_state.pSubsState[i]->pSsInfo);
  }

  policyman_cmcall_deinit_history(policyman_state_get_state());
  policyman_cmss_deinit_history(policyman_state_get_state());
  policyman_cmph_deinit_history(policyman_state_get_state());
  policyman_cfgitem_deinit_notify_history(policyman_state_get_state());
  memset(&policyman_state, 0, sizeof(policyman_state));

  policyman_deinit_crit_sect(&policyman_state_crit_sect);
}


/*-------- policyman_state_static_deinit --------*/
void policyman_state_static_deinit(
  void
)
{
  POLICYMAN_MEM_FREE_IF(pIMEI);
}


/*-------- policyman_state_enter_crit_section --------*/
void policyman_state_enter_crit_section(
  void
)
{
  policyman_enter_crit_sect(&policyman_state_crit_sect);
}


/*-------- policyman_state_leave_crit_section --------*/
void policyman_state_leave_crit_section(
  void
)
{
  policyman_leave_crit_sect(&policyman_state_crit_sect);
}


STATIC policyman_global_info_t * policyman_state_deep_copy_global(
  void
)
{
  policyman_global_info_t *pGlobalInfo;

  pGlobalInfo  = (policyman_global_info_t *) policyman_mem_alloc(sizeof(policyman_global_info_t));
  *pGlobalInfo = *policyman_state.pGlobalInfo;

  return pGlobalInfo;
}


STATIC policyman_device_info_t * policyman_state_deep_copy_device(
  void
)
{
  policyman_device_info_t *pDeviceInfo;

  pDeviceInfo = (policyman_device_info_t *) policyman_mem_alloc(sizeof(policyman_device_info_t));

  pDeviceInfo->pSubsInfo = policyman_subs_info_deep_copy(policyman_state.pDeviceInfo->pSubsInfo);

  return pDeviceInfo;
}


STATIC policyman_subs_state_t * policyman_state_deep_copy_subs(
  size_t subs
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = (policyman_subs_state_t *) policyman_mem_alloc(sizeof(policyman_subs_state_t));

  pSubsState->asubs_id = policyman_state.pSubsState[subs]->asubs_id;
  pSubsState->current_mcc = policyman_state.pSubsState[subs]->current_mcc;
  pSubsState->preconditions_met = policyman_state.pSubsState[subs]->preconditions_met;

  pSubsState->pCmCallInfo = policyman_cmcall_info_deep_copy(policyman_state.pSubsState[subs]->pCmCallInfo);
  pSubsState->pCmPhInfo   = policyman_cmph_info_deep_copy(policyman_state.pSubsState[subs]->pCmPhInfo);
  pSubsState->pNetworkInfo = policyman_network_info_deep_copy(policyman_state.pSubsState[subs]->pNetworkInfo);
  pSubsState->pSsInfo = policyman_ss_info_deep_copy(policyman_state.pSubsState[subs]->pSsInfo);
  pSubsState->pUimInfo = policyman_uim_info_deep_copy(policyman_state.pSubsState[subs]->pUimInfo);

  return pSubsState;
}


policyman_state_t * policyman_state_clone(
  void
)
{
  policyman_state_t *pStateClone;
  size_t             subs;

  pStateClone = (policyman_state_t *) policyman_mem_alloc(sizeof(policyman_state_t));
  
  policyman_state_enter_crit_section();

  pStateClone->pGlobalInfo = policyman_state_deep_copy_global();
  pStateClone->pDeviceInfo = policyman_state_deep_copy_device();

  for (subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
  {
    pStateClone->pSubsState[subs] = policyman_state_deep_copy_subs(subs);
  }
  
  policyman_state_leave_crit_section();
  
  return pStateClone;
}


void policyman_state_delete_device(
  policyman_device_info_t *pDevice
)
{
  policyman_subs_deinit(pDevice->pSubsInfo);
}


void policyman_state_delete_subs(
  policyman_subs_state_t *pSubs
)
{  
  policyman_cmph_deinit(pSubs->pCmPhInfo);
  policyman_network_deinit(pSubs->pNetworkInfo);
  policyman_cmcall_deinit(pSubs->pCmCallInfo);
  policyman_uim_deinit(pSubs->pUimInfo);
  policyman_ss_deinit(pSubs->pSsInfo);
}


void policyman_state_delete_clone(
  policyman_state_t *pState
)
{
  int i;

  for (i = POLICYMAN_NUM_SUBS-1; i >= 0; i--)
  {
    policyman_subs_state_t * pSubsState = pState->pSubsState[i];
    
    policyman_state_delete_subs(pSubsState);
    POLICYMAN_MEM_FREE_IF(pSubsState);
  }

  policyman_state_delete_device(pState->pDeviceInfo);
  POLICYMAN_MEM_FREE_IF(pState->pDeviceInfo);

  POLICYMAN_MEM_FREE_IF(pState->pGlobalInfo);

  POLICYMAN_MEM_FREE_IF(pState);
}


/*-------- policyman_state_get_state --------*/
policyman_state_t * policyman_state_get_state(
  void
  )
{
  return &policyman_state;
}

/*-------- policyman_state_get_global_state --------*/
policyman_global_info_t * policyman_state_get_global_state(
  policyman_state_t *pState
)
{
  return pState->pGlobalInfo;
}


/*-------- policyman_state_get_subs_state --------*/
policyman_subs_state_t * policyman_state_get_subs_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
  )
{
  if (asubs_id >= ARR_SIZE(pState->pSubsState))
  {
    POLICYMAN_ERR_FATAL("subs ID %d exceeds pSubsState array bounds", asubs_id, 0, 0);
  }
  return pState->pSubsState[asubs_id];
}


/*-------- policyman_state_get_device_info --------*/
policyman_device_info_t * policyman_state_get_device_info(
  policyman_state_t *pState
)
{
  return pState->pDeviceInfo;
}


/*-------- policyman_state_get_subs_info --------*/
policyman_subs_info_t * policyman_state_get_subs_info(
  policyman_state_t *pState
  )
{
  return pState->pDeviceInfo->pSubsInfo;
}


/*=============================================================================
  Precondition related APIs
=============================================================================*/

/*-------- policyman_state_get_subs_precond --------*/
uint32 policyman_state_get_subs_precond(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
  )
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, asubs_id);
  
  return pSubsState->preconditions_met;
}


/*-------- policyman_state_check_precondition_met --------*/
boolean policyman_state_check_precondition_met(
  policyman_state_t      *pState,
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32  curPrecond = policyman_state_get_subs_precond(pState, asubs_id);

  POLICYMAN_MSG_MED_3( SUBS_PREFIX "preconditons met: 0x%02x, checking for 0x%02x",
                       asubs_id,
                       curPrecond,
                       precondition );

  return ((curPrecond & precondition) == precondition) ;
}


/*-------- policyman_state_set_precondition_met --------*/
void policyman_state_set_precondition_met(
  policyman_state_t      *pState,
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32                  oldPrecond;
  uint32                  newPrecond;
  policyman_subs_state_t *pSubsState;

  oldPrecond = policyman_state_get_subs_precond(pState, asubs_id);
  newPrecond = oldPrecond | precondition;
  pSubsState = policyman_state_get_subs_state(pState, asubs_id);

  pSubsState->preconditions_met = newPrecond;

  if (oldPrecond != newPrecond)
  {
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "Preconditions changed from 0x%x to 0x%x",
                          asubs_id,
                          oldPrecond,
                          newPrecond );
  }
}


/*-------- policyman_state_reset_precondition_met --------*/
void policyman_state_reset_precondition_met(
  policyman_state_t      *pState,
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32                  oldPrecond;
  uint32                  newPrecond;
  policyman_subs_state_t *pSubsState;

  oldPrecond = policyman_state_get_subs_precond(pState, asubs_id);
  newPrecond = oldPrecond & ~precondition;
  pSubsState = policyman_state_get_subs_state(pState, asubs_id);

  pSubsState->preconditions_met = newPrecond;

  if (oldPrecond != newPrecond)
  {
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "Preconditions changed from 0x%x to 0x%x",
                          asubs_id,
                          oldPrecond,
                          newPrecond );
  }
}


/*================================================================================
  Handle updates in PolicyMan state updates
==================================================================================*/

/*-------- policyman_state_handle_update --------*/
void policyman_state_handle_update(
  policyman_state_t *pState
)
{
  policyman_set_t *pItemSet;
  boolean          inTestMode;
  boolean          inFtm;
  boolean          inLimitedMode;

  inTestMode    = policyman_is_test_mode();
  inFtm         = policyman_is_FTM();
  inLimitedMode = policyman_cfgitem_limited_capabilities_is_enabled();
  
  /* Don't run rules in test mode, FTM or in Limited capabilities
   */
  if (inTestMode || inFtm || inLimitedMode)
  {
    POLICYMAN_MSG_ERROR_3( "no rules executed: test mode(%d), FTM(%d) or Limited capabilities(%d)",
                           inTestMode,
                           inFtm,
                           inLimitedMode );
    return;
  }

  pItemSet = policyman_itemset_new();
  policyman_policies_run_policy_check(pState, pItemSet);
  policyman_cfgitem_update_items(pItemSet, NULL);
  ref_cnt_obj_release(pItemSet);
}


/*=============================================================================
  RF-related state APIs
=============================================================================*/

void policyman_state_trm_cap_init(
  void
)
{
  trm_get_info_input_type     input;
  trm_get_info_return_type    output;

  input.client_id = TRM_CM;
  input.info_type = TRM_SIMUL_CAPABILITY;

  /**
   * For input we are giving client id and info type.
   * TRM will provide simulataneos capability in output.info.simul_cap_info
  **/
  trm_get_info(&input, &output);

  trm_cap = (output.info_type == input.info_type) ? output.info.simul_cap_info : 0;

  POLICYMAN_MSG_HIGH_2( "trm_get_info() for TRM_SIMUL_CAPABILITY returned %d with info_type %d",
                        output.info.simul_cap_info,
                        output.info_type );
}


uint8 policyman_state_get_trm_cap(
  void
)
{
  return trm_cap;
}

/*=============================================================================
  Serving-system related APIs
=============================================================================*/
void policyman_state_ss_init(
  policyman_state_t *pState
)
{
  sys_modem_as_id_e_type  subs;

  for (subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
  {
    sys_mcc_type mcc = NO_MCC;
  
    policyman_ss_init(subs, &policyman_state_get_subs_state(pState, subs)->pSsInfo);
    policyman_state_retrieve_persisted_current_mcc(&mcc, subs);
    policyman_state_set_subs_mcc(pState, subs, mcc);
}

  policyman_cmss_init_history(policyman_state_get_state());
}


/*-------- policyman_state_get_serving_system_info --------*/
policyman_ss_info_t *policyman_state_get_serving_system_info(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, subsId);
  
  return pSubsState->pSsInfo;
}


/*=============================================================================
  Phone-Event related APIs
=============================================================================*/
boolean policyman_state_cmph_init(
  policyman_state_t *pState
)
{
  size_t  subs;
   boolean retval;

  for (subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
   {
    retval = policyman_cmph_init(&policyman_state_get_subs_state(pState, subs)->pCmPhInfo, subs);
   }

  policyman_cmph_init_history(policyman_state_get_state());
   return retval;
 }


/*-------- policyman_state_cmph_nv_init --------*/
void policyman_state_cmph_nv_init(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_cmph_nv_init(&policyman_state_get_subs_state(pState, asubs_id)->pCmPhInfo, asubs_id);
}


/*-------- policyman_state_get_cm_ph_state --------*/
policyman_cmph_info_t * policyman_state_get_cm_ph_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, asubs_id);

  return pSubsState->pCmPhInfo;
}

/*-------- policyman_state_set_simlock_verified --------*/
void policyman_state_set_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
)
{
  boolean old = policyman_state.pSubsState[subsId]->simLockVerified;

  if (verified != old)
  {
    policyman_state.pSubsState[subsId]->simLockVerified = verified;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "state simLockVerified set to %d",
                          subsId,
                          verified );
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_HANDLE_FTM_MODE()

  DESCRIPTION
   Handle FTM mode specific procsessing
===========================================================================*/
static void policyman_state_handle_ftm_mode(
  void
)
{
  /* FTM could be due to new calibration. Delete all 
     persisted config items which will be obsolete in that case.
   */
  policyman_efs_obliterate_subs_ext(SYS_MODEM_AS_ID_NONE, FALSE);

  /* Set FTM NV so that no rule execution can happen
   */
  policyman_set_FTM(TRUE);
}

/*===========================================================================
  FUNCTION policyman_state_reset_ss_info()

  DESCRIPTION
   Reseting SS info on LPM
===========================================================================*/
static void policyman_state_reset_ss_info(
)
{
  policyman_ss_info_t  *pSsInfo;
  size_t                stackId;
  size_t                subsId;
  policyman_state_t    *pState;

  pState = policyman_state_get_state();

  for (subsId = 0; subsId < POLICYMAN_NUM_SUBS; subsId++)
  {
    pSsInfo = policyman_state_get_serving_system_info(pState, subsId);

    for (stackId = SYS_MODEM_STACK_ID_1; stackId < SYS_MODEM_STACK_ID_MAX; stackId++)
    {
      policyman_ss_set_default_value(pSsInfo, stackId);
    }

    policyman_state_reset_precondition_met(pState, POLICYMAN_PRECOND_SS, subsId);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_HANDLE_OPRT_MODE()

  DESCRIPTION
   Handle FTM mode specific procsessing
===========================================================================*/
static void policyman_state_handle_oprt_mode(
  sys_oprt_mode_e_type oprt_mode
  )
{

  switch (oprt_mode)
  {
    case SYS_OPRT_MODE_ONLINE:
    {
      POLICYMAN_MSG_MED_0("oprt_mode changed to SYS_OPRT_MODE_ONLINE, print all configuration database");
      policyman_cfgitem_display_all();
    }
    break;

    case SYS_OPRT_MODE_LPM:
    policyman_state_reset_ss_info();
    break;

	default:
	break;
  }//switch (oprt_mode)

  /* Initialize Policy when already in FTM and disable FTM
        For test_mode, we should contiue to be running without policy
    */
  if (   policyman_is_FTM()
     && !policyman_is_test_mode()
    )
  {

    policyman_set_t       *pItemSet;

    pItemSet     = policyman_itemset_new();

    policyman_set_FTM(FALSE);

    policyman_policies_init(&policyman_state, pItemSet);
    policyman_policy_load_global(&policyman_state, pItemSet);
    policyman_sub_policies_init(&policyman_state, pItemSet);
    policyman_policy_init_is_complete();

    /* Update the database and send client notification */
    policyman_cfgitem_update_items(pItemSet, NULL);

    policyman_cfgitem_display_all();

    REF_CNT_OBJ_RELEASE_IF(pItemSet);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_PH_OPRT_MODE()

  DESCRIPTION
    Update operating mode into PM Phone state and calls handler function for its update

  PARAMETERS
    oprt_mode : operating mode to be set

  RETURN VALUE
    None
===========================================================================*/
void policyman_state_update_ph_oprt_mode(
  policyman_state_t    *pState,
  sys_oprt_mode_e_type oprt_mode
)
{
  size_t i;
  boolean updated_oprt_mode = FALSE;

  switch(oprt_mode)
  {
    case SYS_OPRT_MODE_FTM:
      policyman_state_handle_ftm_mode();
      break;

    default:
      policyman_state_handle_oprt_mode(oprt_mode);
      break;
  }

  for (i = 0; i < POLICYMAN_NUM_SUBS; i++)
  {
    sys_modem_as_id_e_type  asubs_id = (sys_modem_as_id_e_type) i;
    policyman_subs_state_t *pSubsState = policyman_state_get_subs_state(pState, asubs_id);

    updated_oprt_mode = policyman_ph_set_oprt_mode(pSubsState->pCmPhInfo, oprt_mode);
  }

  if(updated_oprt_mode)
  {
    policyman_state_handle_update(pState);
  }
}

/*=============================================================================
  UIM related APIs
=============================================================================*/

/*-------- policyman_state_uim_init --------*/
boolean policyman_state_uim_init(
  policyman_state_t *pState
)
{
  size_t i;
  boolean retval;

  for (i = 0; i < POLICYMAN_NUM_SUBS; i++)
  {
    retval = policyman_uim_init(&policyman_state_get_subs_state(pState, i)->pUimInfo);
  }

  return retval;
}


/*-------- policyman_state_get_uim_info --------*/
policyman_uim_info_t * policyman_state_get_uim_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, asubs_id);

  return pSubsState->pUimInfo;
}

/*=============================================================================
  Num Subs related APIs
=============================================================================*/
boolean policyman_state_subs_init(
  policyman_state_t *pState
   )
 {
  return policyman_subs_init(&pState->pDeviceInfo->pSubsInfo);
 }

/*=============================================================================
  Call related APIs
=============================================================================*/
  /*-------- policyman_state_cmcall_init --------*/
boolean policyman_state_cmcall_init(
  policyman_state_t *pState
  )
{
  size_t  subs;
   boolean retval;

  for (subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
   {
    retval = policyman_cmcall_init(&policyman_state_get_subs_state(pState, subs)->pCmCallInfo, subs);
   }

  policyman_cmcall_init_history(policyman_state_get_state());

  return retval;
}


/*-------- policyman_state_get_call_info --------*/
policyman_cmcall_info_t * policyman_state_get_call_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, subsId);

  return pSubsState->pCmCallInfo;
}


/*-------- policyman_state_imei_init --------*/
void policyman_state_imei_init(
  void
  )
{
  byte              *pImei = NULL;
  char              imei[16];
  char              *pDst;
  size_t            i;

  /*  Get the IMEI
 */
  nv_stat_enum_type result;
  nv_item_type      data;

  result = mre_efs_get_nv_item(NV_UE_IMEI_I, &data, SYS_MODEM_AS_ID_1);
  if (result == NV_DONE_S)
  {
    pImei = data.ue_imei.ue_imei;
  }

  /*  The IMEI buffer must start with a length byte of 8 and have 0x0a in
   *  the low nibble of the next byte.  If it doesn't, don't set IMEI.
   */
  if (pImei == NULL)
  {
    POLICYMAN_MSG_HIGH_0("Unable to read IMEI from NV");
    return;
  }

  if (*pImei != 0x08 || (pImei[1] & 0x0f) != 0x0a)
  {
    POLICYMAN_MSG_HIGH_0("IMEI read from NV has invalid format");
    policyman_device_imei_print(pImei);
    return;
  }

  /*  Copy the raw IMEI into the device information.
   */
  memscpy(
      rawIMEI,
      sizeof(rawIMEI),
      pImei,
      sizeof(rawIMEI)
      );

  ++pImei;

  /*  Get a pointer to the storage for the string we'll produce.
   */
  pDst = imei;

  /*  Get the first character of the IMEI.
   */
  *pDst++ = ((*pImei++) >> 4) + '0';

  /*  Get the rest of the characters.
   */
  for (i = 0 ; i < 6 ; ++i)
  {
    *pDst++ = (*pImei & 0x0f) + '0';
    *pDst++ = ((*pImei++) >> 4) + '0';
  }
  *pDst++ = (*pImei & 0x0f) + '0';
  *pDst = '\0';

  POLICYMAN_UT_MSG_1("IMEI is set to %s", imei);

  /*  Store a copy of the string in the device information.
   */
  pIMEI = policyman_str_dup(imei);
}


/*-------- policyman_state_get_imei --------*/
char const * policyman_state_get_imei(
  void
)
{
  return pIMEI;
}


/*-------- policyman_state_get_raw_imei --------*/
byte * policyman_state_get_raw_imei(
  void
)
{
  return rawIMEI;
}


/*=============================================================================
  Network Scan related APIs
=============================================================================*/
boolean policyman_state_network_init(
  policyman_state_t *pState
  )
{
   size_t   subsIdx;
   boolean  retval;

   for (subsIdx = 0; subsIdx<POLICYMAN_NUM_SUBS; subsIdx++)
   {
     retval = policyman_network_init(&policyman_state_get_subs_state(pState, subsIdx)->pNetworkInfo, subsIdx);
   }

  return retval;
}

/*-------- policyman_state_update_network_scan_result --------*/
void policyman_state_update_network_scan_result(
  policyman_state_t *pState,
  cm_ph_info_s_type *pInfo
)
{
  policyman_network_info_t *pNetworkInfo;

  if (pInfo != NULL)
  {
    pNetworkInfo = policyman_state_get_network_info(pState, pInfo->asubs_id);

    /* Update the result and run rules
      */
    if (policyman_network_scan_update_result( pNetworkInfo,
                                             pInfo->asubs_id,
                                             pInfo->available_networks_list_cnf,
                                             &pInfo->available_networks )
      )
    {
      policyman_state_handle_update(pState);
    }
  }

}

/*-------- policyman_state_get_network_info --------*/
policyman_network_info_t * policyman_state_get_network_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId
  )
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, subsId);
  
  return pSubsState->pNetworkInfo;
}

/*=============================================================================
  HLOS MCC related APIs
=============================================================================*/

/*-------- policyman_state_current_mcc_update_to_efs --------*/
STATIC void policyman_state_current_mcc_update_to_efs(
  sys_mcc_type            mcc,
  sys_modem_as_id_e_type  subsId
)
{
  boolean  writeStatus;

  writeStatus = policyman_cfgitem_persist_item( POLICYMAN_CURRENT_MCC_NV_PATH,
                                                &mcc,
                                                sizeof(mcc),
                                                subsId);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "current_mcc %d written to EFS, status %d",
                        subsId,
                        mcc,
                        writeStatus );
}

/*-------- policyman_state_update_hlos_mcc --------*/
void policyman_state_update_hlos_mcc(
  policyman_state_t *pState,
  sys_mcc_type       hlos_mcc,
  size_t             confidenceLevel,
  size_t             status
)
{
  policyman_global_info_t *pGlobal = policyman_state_get_global_state(pState);
  
  pGlobal->mcc             = hlos_mcc;
  pGlobal->confidenceLevel = confidenceLevel;
  pGlobal->status          = status;
}


/*-------- policyman_state_clear_hlos_mcc --------*/
void policyman_state_clear_hlos_mcc(
  policyman_state_t *pState
)
{
  policyman_state_update_hlos_mcc(pState, NO_MCC, 0, 0);
}


/*-------- policyman_state_get_hlos_mcc --------*/
sys_mcc_type policyman_state_get_hlos_mcc(
  policyman_state_t *pState
)
{
  policyman_global_info_t *pGlobal = policyman_state_get_global_state(pState);
  
  return pGlobal->mcc;
}

/*-------- policyman_state_set_subs_mcc --------*/
void policyman_state_set_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId,
  sys_mcc_type mcc
)
{
  policyman_subs_state_t *pSubsState;
  sys_mcc_type            current_mcc;

  pSubsState  = policyman_state_get_subs_state(pState, subsId);
  current_mcc = policyman_state_get_subs_mcc(pState, subsId);

  if (mcc != current_mcc)
  {
    pSubsState->current_mcc = mcc;
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "updating current_mcc from %d to %d",
                          subsId,
                          current_mcc,
                          mcc );

    policyman_state_current_mcc_update_to_efs(mcc, subsId);
  }
}

/*-------- policyman_state_update_subs_mcc --------*/
void policyman_state_update_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type            mcc
)
{
  if (NO_MCC != mcc)
  {
    policyman_state_set_subs_mcc(pState, subsId, mcc);
  }
}

/*-------- policyman_state_get_subs_mcc --------*/
sys_mcc_type policyman_state_get_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_subs_state_t *pSubsState;

  pSubsState = policyman_state_get_subs_state(pState, subsId);

  return pSubsState->current_mcc;
}

/*=============================================================================
  FULL RAT related APIs
=============================================================================*/
void policyman_state_update_full_rat(
  policyman_state_t *pState,
  boolean enterFullRat
  )
{
  policyman_global_info_t *pGlobal = policyman_state_get_global_state(pState);
  
  if (enterFullRat != pGlobal->immediate_fullrat)
  {
    pGlobal->immediate_fullrat = enterFullRat;
    POLICYMAN_MSG_HIGH_1("policyman state updated with full rat status = %d", enterFullRat);
  }
}

/*-------- policyman_state_get_full_rat --------*/
boolean policyman_state_get_full_rat(
  policyman_state_t *pState
  )
{
  policyman_global_info_t *pGlobal = policyman_state_get_global_state(pState);
  
  return pGlobal->immediate_fullrat;
}


/*-------- policyman_state_get_simlock_verified --------*/
boolean policyman_state_get_simlock_verified(
  sys_modem_as_id_e_type  subsId
)
{
  return policyman_state.pSubsState[subsId]->simLockVerified;
}

/*-------- policyman_state_update_simlock_verified --------*/
void policyman_state_update_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
)
{
  boolean old;

  old = policyman_state.pSubsState[subsId]->simLockVerified;

  if (verified != old)
  {
    policyman_state.pSubsState[subsId]->simLockVerified &= verified;

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "state simLockVerified updated from %d to %d",
                          subsId,
                          old,
                          policyman_state.pSubsState[subsId]->simLockVerified );
  }
}

/*-------- policyman_state_get_thermal_rat_disable_mask --------*/
uint32 policyman_state_get_thermal_rat_disable_mask(
  void
)
{
  uint32                    mask       = SYS_SYS_MODE_MASK_NONE;
  policyman_state_t        *pStateInfo = NULL;
  policyman_thermal_level_t lvl ;

  pStateInfo = policyman_state_get_state();
  lvl        = pStateInfo->pDeviceInfo->thermalLevel;
  
  if (lvl == POLICYMAN_THERM_LEVEL_2)
  {
    mask |= SYS_SYS_MODE_MASK_LTE; 
  }
  POLICYMAN_MSG_HIGH_1("get_rat_disable_mask 0x%04x", mask);

  return mask;
}


void policyman_state_update_thermal_level(
  policyman_thermal_level_t  thermalLevel
)
{
  policyman_state_t  *pStateInfo = NULL;

  pStateInfo = policyman_state_get_state();
  
  POLICYMAN_MSG_HIGH_2 ( "Thermal level updated from 0x%04x to 0x%04x", 
                          pStateInfo->pDeviceInfo->thermalLevel, 
                          thermalLevel);
  
  pStateInfo->pDeviceInfo->thermalLevel = thermalLevel;
}

/*-------- policyman_state_thermal_allow_persistence --------*/
boolean policyman_state_thermal_allow_persistence(
  void
)
{
  return (policyman_state_get_thermal_rat_disable_mask() == SYS_SYS_MODE_MASK_NONE) ? TRUE : FALSE;
}
/*-------- policyman_state_fill_diag_subs_info --------*/
void policyman_state_fill_diag_subs_info(
  sys_modem_as_id_e_type  asubs_id,
  pm_dump_subs_info_t    *dump_subs_info
)
{
  policyman_subs_state_t  *pSubsInfo;

  pSubsInfo   = &policyman_subs_state_info[asubs_id];

  dump_subs_info->asubs_id          = asubs_id;
  dump_subs_info->preconditions_met = pSubsInfo->preconditions_met;

  policyman_cmss_fill_diag_ss_info( pSubsInfo->pSsInfo,
                                    &dump_subs_info->cm_ss_info );

  policyman_cmph_fill_diag_ph_info( pSubsInfo->pCmPhInfo,
                                    &dump_subs_info->cm_ph_info );

  policyman_cmcall_fill_diag_call_info( asubs_id,
                                        pSubsInfo->pCmCallInfo,
                                        &dump_subs_info->cm_call_info );

  policyman_uim_fill_diag_uim_info( pSubsInfo->pUimInfo,
                                    &dump_subs_info->uim_info );
}