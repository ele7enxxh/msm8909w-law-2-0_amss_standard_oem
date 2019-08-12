/**
  @file policyman_cfgitem.c

  @brief Functions to manage PM configuration items.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_cfgitem.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_efs.h"

#include "policyman_call_mode.h"
#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_msg.h"
#include "policyman_policies.h"
#include "policyman_rat_capability.h"
#include "policyman_rat_order.h"
#include "policyman_set.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_ue_mode.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_voice_domain_pref.h"
#include "policyman_scan_optimization.h"
#include "policyman_policy.h"
#include "policyman_frequency_list.h"
#include "mre_set.h"


#include "sys.h"
#include "log.h"
#include "stringl/stringl.h"
//#include "qsh.h"



typedef policyman_item_t  *(*get_default_fn_t)(policy_execute_ctx_t* ctx);
typedef boolean            (*items_equal_fn_t)(policyman_item_t *i1, policyman_item_t *i2);
typedef void               (*display_fn_t)(policyman_item_t *item);
typedef void               (*on_update_fn_t)(policyman_item_t *item);
typedef policyman_status_t (*get_efs_fn_t)(cfgitem_execute_ctx_t *pCtx);
typedef void               (*fill_diag_subs_log_fn_t)(policyman_item_t const *item, LOG_PM_SUBS_INFO_type *log_ptr);
typedef void               (*get_limited_item_fn_t)(mre_set_t *pItemSet, sys_modem_as_id_e_type subsId);

extern policyman_crit_sect_t   policyman_item_crit_sect;
STATIC boolean                 limitedCapabilityIsEnabled  = FALSE;

struct policyman_config_info_t
{
  policyman_item_id_t       id;
  const char               *pName;
  get_default_fn_t          pfnGetDefault;
  items_equal_fn_t          pfnItemsEqual;
  display_fn_t              pfnDisplay;
  on_update_fn_t            pfnOnUpdate;
  get_efs_fn_t              pfnGetEFS;
  fill_diag_subs_log_fn_t   pfnFillDiagSubsLog;
  get_limited_item_fn_t     pfnGetLimitedItem;
};

typedef struct 
{
  policyman_item_id_t    id;
  sys_modem_as_id_e_type asubs_id;
} policyman_item_cmp_ctx_t;

/* Database of policyman config info
*/
STATIC policyman_config_info_t  policyman_cfg_info[] =
{
  {
    POLICYMAN_ITEM_DEVICE_CONFIGURATION,
    "Modem configuration",
    policyman_device_config_get_default,
    policyman_device_config_compare,
    policyman_device_config_display,
    (on_update_fn_t)NULL,
    policyman_device_config_get_default_from_efs,
    (fill_diag_subs_log_fn_t)NULL,
    policyman_device_configuration_get_limited_item,
  },
  {
    POLICYMAN_ITEM_RAT_CAPABILITY,
    "RAT capability",
    policyman_rat_capability_get_default,
    policyman_rat_capability_compare,
    policyman_rat_capability_display,
    policyman_rat_capability_on_update,
    policyman_rat_capability_get_default_from_efs,
    policyman_rat_capability_fill_subs_diag_log,
    policyman_rat_capability_get_limited_item,
  },
  {
    POLICYMAN_ITEM_RF_BANDS,
    "RF bands",
    policyman_rf_bands_get_default,
    policyman_rf_bands_compare,
    policyman_rf_bands_display,
    (on_update_fn_t)NULL,
    policyman_rf_bands_get_default_from_efs,
    policyman_rf_bands_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
  {
    POLICYMAN_ITEM_SVC_MODE,
    "SVC mode",
    policyman_svc_mode_get_default,
    policyman_svc_mode_compare,
    policyman_svc_mode_display,
    (on_update_fn_t)NULL,
    policyman_svc_mode_get_default_from_efs,
    policyman_svc_mode_fill_subs_diag_log,
    policyman_svc_mode_get_limited_item,
  },
  {
    POLICYMAN_ITEM_UE_MODE,
    "UE mode",
    policyman_ue_mode_get_default,
    policyman_ue_mode_compare,
    policyman_ue_mode_display,
    policyman_ue_mode_on_update,
    policyman_ue_mode_get_default_from_efs,
    policyman_ue_mode_fill_subs_diag_log,
    policyman_ue_mode_get_limited_item,
  },
  {
    POLICYMAN_ITEM_VOICE_DOMAIN_PREF,
    "Voice Domain Preference",
    policyman_voice_domain_pref_get_default,
    policyman_voice_domain_pref_compare,
    policyman_voice_domain_pref_display,
    (on_update_fn_t)NULL,
    (get_efs_fn_t)NULL,
    policyman_voice_domain_pref_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
  {
    POLICYMAN_ITEM_RAT_ACQ_ORDER,
    "Rat acquisition order",
    policyman_rat_order_get_default,
    policyman_rat_order_compare,
    policyman_rat_order_display,
    (on_update_fn_t)NULL,
    (get_efs_fn_t)NULL,
    policyman_rat_order_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
  {
    POLICYMAN_ITEM_CALL_MODE,
    "Call mode",
    policyman_call_mode_get_default,
    policyman_call_mode_compare,
    policyman_call_mode_display,
    (on_update_fn_t)NULL,
    (get_efs_fn_t)NULL,
    policyman_call_mode_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
  {
    POLICYMAN_ITEM_SCAN_OPTIMIZATION,
    "Scan Optimization",
    policyman_scan_opt_get_default,
    policyman_scan_opt_compare,
    policyman_scan_opt_display,
    (on_update_fn_t)NULL,
    (get_efs_fn_t)NULL,
    policyman_scan_opt_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
  {
    POLICYMAN_ITEM_FREQ_LIST,
    "Frequency List",
    policyman_freq_list_get_default,
    policyman_freq_list_compare,
    policyman_freq_list_display,
    (on_update_fn_t)NULL,
    policyman_freq_list_get_default_from_efs,
    policyman_freq_list_fill_subs_diag_log,
    (get_limited_item_fn_t)NULL,
  },
};

typedef struct
{
  policyman_config_info_t *pCfgInfo;
  sys_modem_as_id_e_type   subsId;
  boolean                  is_changed;
  
  policyman_item_t        *pItem;  
} policyman_config_item_t;

#define NUM_PM_SUBS_ITEMS (ARR_SIZE(policyman_cfg_info) - 1)
#define NUM_PM_CFG_ITEMS  (1 + (NUM_PM_SUBS_ITEMS * POLICYMAN_NUM_SUBS))

/* Database of actual policyman items
    Array size is the number of per-subs items plus one for the device config
*/
STATIC policyman_config_item_t policyman_cfgitems[NUM_PM_CFG_ITEMS];

/*  Sequence number of the database.
 */
STATIC size_t g_seqno;

/* blocking suspend callers while DB update in progress
   related variables and functions----- START
*/

STATIC  atomic_word_t          g_dbUpdateState;
STATIC  policyman_crit_sect_t  g_blockedSignalsCS;
STATIC  mre_set_t             *g_pBlockedSignalSet;

/*-------- db_update_state_init --------*/
STATIC void db_update_state_init(
  void
)
{
  atomic_init(&g_dbUpdateState, POLICYMAN_DBUPDATE_STATE_NONE);
  policyman_init_crit_sect(&g_blockedSignalsCS);

  policyman_enter_crit_sect(&g_blockedSignalsCS);
  g_pBlockedSignalSet = mre_set_new(  sizeof(policyman_signal_t),
                                      5,
                                      MRE_SET_NO_MAX_SIZE,
                                      NULL,
                                      NULL );
  policyman_leave_crit_sect(&g_blockedSignalsCS);

  POLICYMAN_MSG_HIGH_0( "db_update_state_init ");
}

/*-------- policyman_dbupdate_try_enter_update_state --------*/
boolean policyman_dbupdate_try_enter_update_state(
  void
)
{
  return atomic_compare_and_set( &g_dbUpdateState,
                                  POLICYMAN_DBUPDATE_STATE_NONE,
                                  POLICYMAN_DBUPDATE_STATE_UPDATING);
}

/*-------- policyman_dbupdate_try_enter_suspend_state --------*/
boolean policyman_dbupdate_try_enter_suspend_state(
  void
)
{
  return atomic_compare_and_set( &g_dbUpdateState,
                                  POLICYMAN_DBUPDATE_STATE_NONE,
                                  POLICYMAN_DBUPDATE_STATE_SUSPENDED);
}

/*-------- policyman_dbupdate_enter_idle_state --------*/
void policyman_dbupdate_enter_idle_state(
  void
)
{
  atomic_set( &g_dbUpdateState, POLICYMAN_DBUPDATE_STATE_NONE);
}

/*-------- policyman_dbupdate_state_compare_and_set --------*/
policyman_dbupdate_state policyman_dbupdate_state_get(
  void
)
{
  return g_dbUpdateState.value;
}

/*-------- release_dbupdate_blocked_client --------*/
STATIC boolean release_dbupdate_blocked_client(
  void  *pObj,
  void  *pCtx
)
{
  policyman_signal_t  *pSignal = (policyman_signal_t *) pObj;

  if (*pSignal != POLICYMAN_NULL_SIGNAL)
  {
    policyman_client_dbupdate_block_signal_set(*pSignal);
  }

  return TRUE;
}

/*-------- dbupdate_block_signal_set --------*/
STATIC void dbupdate_release_blocked_clients(
  void
)
{
  policyman_enter_crit_sect(&g_blockedSignalsCS);
  // Unblock clients
  mre_set_iterate(g_pBlockedSignalSet, release_dbupdate_blocked_client, NULL);
  while(!mre_set_is_empty(g_pBlockedSignalSet))
  {
    policyman_signal_t  *pSignal = (policyman_signal_t  *)mre_set_elem_ptr(g_pBlockedSignalSet, 0);
    mre_set_remove(g_pBlockedSignalSet, pSignal);
  }
  policyman_leave_crit_sect(&g_blockedSignalsCS);
}

/*-------- policyman_dbupdate_block_signal_wait --------*/
void policyman_dbupdate_block_signal_wait(
  void
)
{
  policyman_signal_t signal = policyman_client_block_sig_get();

  policyman_enter_crit_sect(&g_blockedSignalsCS);
  mre_set_add(g_pBlockedSignalSet, &signal);
  policyman_leave_crit_sect(&g_blockedSignalsCS);

  policyman_client_dbupdate_block_signal_wait(signal);
}

/* blocking suspend callers while DB update in progress
   related variables and functions----- END
*/

STATIC void policyman_cfgitem_notify_clients(
  policyman_set_t *pItemSet,
  policyman_cmd_t *pCmd
);


/*-------- sequence_number_init --------*/
STATIC void sequence_number_init(
  void
)
{
  g_seqno = 0;
}


/*-------- sequence_number_increment --------*/
STATIC void sequence_number_increment(
  void
)
{
  ++g_seqno;
}


/*-------- sequence_number_get --------*/
STATIC size_t sequence_number_get(
  void
)
{
  return g_seqno;
}


/*-------- policyman_config_item_is_device_level --------*/
STATIC boolean policyman_config_item_is_device_level(
  policyman_config_item_t *pCfgItem
)
{
  return POLICYMAN_ITEM_DEVICE_CONFIGURATION == pCfgItem->pCfgInfo->id;
}


/*-------- policyman_subs_id_is_configured --------*/
STATIC boolean policyman_subs_id_is_configured(
  sys_modem_as_id_e_type subs_id 
)
{
  
  size_t  nSim = 0;

  (void) policyman_get_current_num_sim(&nSim) ;

  return ((size_t) subs_id) < nSim;
}


/*-------- policyman_cfgitems_iterate --------*/
STATIC void policyman_cfgitems_iterate(
  elem_iter_fn_t   pfnIter,
  void            *pCtx
)
{
  size_t  i;
  boolean keepGoing = TRUE;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; keepGoing && i < ARR_SIZE(policyman_cfgitems) ; ++i)
  {
    policyman_config_item_t *pCfgItem = &policyman_cfgitems[i];
    
    if (    policyman_config_item_is_device_level(pCfgItem)
        ||  policyman_subs_id_is_configured(pCfgItem->subsId)
       )
    {
      keepGoing = pfnIter(pCfgItem, pCtx);
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);
}


/*-------- policyman_cfgitem_init_item --------*/
void policyman_cfgitem_init_item(
  size_t    subsIndex,
  size_t    cfgIndex,
  size_t    itemIndex
)
{
  policyman_config_item_t   *pCfgItem;

  pCfgItem             = &policyman_cfgitems[itemIndex];
  pCfgItem->pCfgInfo   = &policyman_cfg_info[cfgIndex];
  pCfgItem->subsId     = subsIndex;
  pCfgItem->pItem      = NULL;
  pCfgItem->is_changed = TRUE;
}


/*-------- cfgitems_init --------*/
STATIC void cfgitems_init(
  void
)
{
  // Initialize the special item 0 (device config)
  policyman_cfgitem_init_item(0, 0, 0);

  // Initialize remaining per-subs items
  {
    size_t                   subsIndex; 
    size_t                   cfgIndex; 
    size_t                   itemIndex;

    itemIndex = 1;

    for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; ++subsIndex)
    {
      // Skip device config item 0 for per-subs items
      for (cfgIndex = 1; cfgIndex < ARR_SIZE(policyman_cfg_info); ++cfgIndex)
      {
        policyman_cfgitem_init_item(subsIndex, cfgIndex, itemIndex);
        ++itemIndex;
      }
    }
  }
}

/*-------- cfgitems_read_persisted_values --------*/
STATIC boolean cfgitems_read_persisted_values(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem    = (policyman_config_item_t *)pElem;
  cfgitem_execute_ctx_t   *pCfgCtx     = (cfgitem_execute_ctx_t *)pCtx;
  boolean                  readSuccess = TRUE;
  sys_modem_as_id_e_type   subsId;

  subsId = pCfgItem->subsId;

  if ( !policyman_is_test_mode()
     && pCfgItem->pCfgInfo->pfnGetEFS != NULL
     )
  {
    policyman_status_t      status;
    sys_modem_as_id_e_type  discardSubs = SYS_MODEM_AS_ID_NONE;

    pCfgCtx->asubs_id = subsId;
    pCfgCtx->pItem    = NULL;

    // call the EFS read handler and update the items based on status
    status = pCfgItem->pCfgInfo->pfnGetEFS(pCfgCtx);
    if (POLICYMAN_SUCCEEDED(status))
    {
      policyman_set_add(pCfgCtx->pItemSet, &pCfgCtx->pItem);
      ref_cnt_obj_release(pCfgCtx->pItem);

      pCfgCtx->pItem = NULL;
    }
    else if (status == POLICYMAN_STATUS_ERR_INVALID_VERSION)
    {
      boolean deleteXMLs = FALSE;

      policyman_efs_obliterate_subs_ext(discardSubs, deleteXMLs);
      readSuccess = FALSE;
    }
  }

  POLICYMAN_UT_MSG_3( "subs %d: INIT_ITEM: item with id %d, pItem 0x%08x",
                        subsId,
                        pCfgItem->pCfgInfo->id,
                        pCfgItem->pItem );

  return readSuccess;
}

/*-------- cfgitems_get_persisted_values --------*/
STATIC void cfgitems_get_persisted_values(
  policyman_set_t   *pItemSet
)
{
  cfgitem_execute_ctx_t    cfgCtx;
  device_configuration_t  *pDc;

  cfgCtx.pItemSet = policyman_itemset_new();

  policyman_cfgitems_iterate(cfgitems_read_persisted_values, &cfgCtx);

  // if no device config item in itemset then don't update the database
  pDc = policyman_get_device_config_from_itemset(cfgCtx.pItemSet);

  if (NULL != pDc)
  {
    size_t i;
    size_t nElems;
    policyman_item_t  **ppItem;

    nElems = policyman_set_num_elems(cfgCtx.pItemSet);

    for (i = 0; i < nElems ; ++i)
    {
      ppItem = (policyman_item_t **) policyman_set_elem_ptr(cfgCtx.pItemSet, i);
      policyman_set_add(pItemSet, ppItem);
    }
  }

  REF_CNT_OBJ_RELEASE_IF(pDc);
  ref_cnt_obj_release(cfgCtx.pItemSet);
}


/*-------- policyman_cfgitem_init --------*/
void policyman_cfgitem_init(
  policyman_set_t   *pItemSet
)
{
  db_update_state_init();
  sequence_number_init();
  cfgitems_init();
  cfgitems_get_persisted_values(pItemSet);

  POLICYMAN_MSG_HIGH_0( "Config item initialization complete");
}

/*-------- policyman_cfgitem_persistence_allowed --------*/

STATIC boolean policyman_cfgitem_persistence_allowed(
  void
)
{
  return policyman_state_thermal_allow_persistence();
}

/*-------- policyman_persist_cfgitems --------*/
void
policyman_persist_cfgitems(
  policyman_set_t *pNotifySet
)
{
  size_t                    i;
  size_t                    nElems;
  policyman_item_id_t       id;
  policyman_item_t const   *pItem;
  policyman_item_t const  **ppItem;
  boolean                   doUpdate = FALSE;
  uint8                     subsMask = 0;

  if (!policyman_cfgitem_persistence_allowed())
  {
    POLICYMAN_MSG_HIGH_0("Persistence of Config items not allowed...");
    return;
  }


  /*  Do not persist if we're in test mode, or if device has not been calibrated yet.
   */
  if (policyman_is_test_mode() || policyman_is_FTM())
  {
    return;
  }

  nElems = policyman_set_num_elems(pNotifySet);
  for (i = 0; i < nElems ; ++i)
  {
    ppItem = (policyman_item_t const **) policyman_set_elem_ptr(pNotifySet, i);
    if (ppItem == NULL)
    {
      break;
    }

    pItem = *ppItem;   
    id = policyman_item_get_id(pItem);

    switch(id)
    {
      case POLICYMAN_ITEM_UE_MODE:
        policyman_ue_mode_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_RAT_ACQ_ORDER:
        policyman_rat_order_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_SVC_MODE:
        policyman_svc_mode_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_RAT_CAPABILITY:
      case POLICYMAN_ITEM_RF_BANDS:
        doUpdate = TRUE;
        subsMask |= 1<<(uint8)pItem->asubs_id;
        break;

      case POLICYMAN_ITEM_DEVICE_CONFIGURATION:
        policyman_device_config_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_CALL_MODE:
        policyman_call_mode_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_FREQ_LIST:
        policyman_freq_list_update_to_efs(pItem);
        break;

      default:
        POLICYMAN_MSG_MED_2( SUBS_PREFIX "item id %d doesn't need update",
                             pItem->asubs_id,
                             id );
    }
  }

  if(doUpdate)
  {
    policyman_update_rats_bands_to_efs(subsMask);
  }
}


/*-------- policyman_cfgitem_reset_changed --------*/
STATIC void policyman_cfgitem_reset_changed(
  void
)
{
  size_t                   itemIndex;

  for (itemIndex = 0; itemIndex < NUM_PM_CFG_ITEMS; ++itemIndex)
  {
    policyman_cfgitems[itemIndex].is_changed = FALSE;
  }
}

/*-------- policyman_cfgitem_devcfg_has_changes --------*/
STATIC boolean policyman_cfgitem_devcfg_has_changes(
  void
)
{
  return policyman_cfgitems[0].is_changed;
}

/*-------- policyman_cfgitem_subs_has_changes --------*/
STATIC boolean policyman_cfgitem_subs_has_changes(
  size_t    subsIndex
)
{
  boolean  has_changes = FALSE;
  size_t   itemIndex;
  size_t   startIndex;
  size_t   endIndex;

  startIndex = 1 + (subsIndex * NUM_PM_SUBS_ITEMS);
  endIndex   = startIndex + NUM_PM_SUBS_ITEMS;

  // Skip device config item 0 for per-subs items
  for (itemIndex = startIndex; itemIndex < endIndex; ++itemIndex)
  {
    if (   (policyman_cfgitems[itemIndex].subsId == subsIndex)
        &&  policyman_cfgitems[itemIndex].is_changed )
    {
      has_changes = TRUE;
      break;
    }
  }

  return has_changes;
}

/*-------- policyman_cfgitem_send_diag_cfg_log --------*/
void policyman_cfgitem_send_diag_cfg_log(
  int       notifyHistoryIndex
)
{
  policyman_item_t         *pCfgItem;

  if (!policyman_cfgitem_devcfg_has_changes())
  {
    POLICYMAN_UT_MSG_0("send_diag_cfg_log: no changes to config");
    return;
  }

  pCfgItem = policyman_cfgitems[0].pItem;

  policyman_device_config_create_and_send_diag_cfg_log(pCfgItem, notifyHistoryIndex);
}

STATIC void policyman_cfgitem_set_item_changed_mask(
  LOG_PM_SUBS_INFO_type   *log_ptr,
  policyman_item_t        *pItem,
  size_t                   itemIndex
)
{
  policyman_item_id_t  id;

  id = policyman_item_get_id(pItem);

  POLICYMAN_UT_MSG_4( SUBS_PREFIX "SUBS_LOG: item at index %d, id %d, changed %d",
                      pItem->asubs_id,
                      itemIndex,
                      id,
                      policyman_cfgitems[itemIndex].is_changed );

  if (policyman_cfgitems[itemIndex].is_changed)
  {
    log_ptr->changed_mask |= BM(id);
  }
}

/*-------- policyman_cfgitem_fill_diag_subs_log --------*/
STATIC void policyman_cfgitem_fill_diag_subs_log(
  size_t                      subsIndex,
  LOG_PM_SUBS_INFO_type      *log_ptr
)
{
  size_t   itemIndex;
  size_t   startIndex;
  size_t   endIndex;

  startIndex = 1 + (subsIndex * NUM_PM_SUBS_ITEMS);
  endIndex   = startIndex + NUM_PM_SUBS_ITEMS;

  // Iterate through subs items.
  // Skip device config item 0 for per-subs items
  for (itemIndex = startIndex; itemIndex < endIndex; ++itemIndex)
  {
    policyman_item_t    *pItem;

    pItem = policyman_cfgitems[itemIndex].pItem;

    if (NULL == pItem)
    {
      continue;
    }

    policyman_cfgitem_set_item_changed_mask(log_ptr, pItem, itemIndex);

    POLICYMAN_UT_MSG_1("SUBS_LOG: changed_mask 0x%08x", log_ptr->changed_mask);

    if (NULL != pItem->pCfgInfo->pfnFillDiagSubsLog)
    {
      pItem->pCfgInfo->pfnFillDiagSubsLog(pItem, log_ptr);
    }
  }
}

/*-------- policyman_cfgitem_allocate_and_init_diag_subs_log --------*/
STATIC LOG_PM_SUBS_INFO_type * policyman_cfgitem_allocate_and_init_diag_subs_log(
  void
)
{
  LOG_PM_SUBS_INFO_type      *log_ptr;

  log_ptr = (LOG_PM_SUBS_INFO_type *)log_alloc_ex( LOG_PM_SUBS_INFO_C,
                                                   sizeof(LOG_PM_SUBS_INFO_type) );

  if (NULL != log_ptr)
  {
    log_ptr->version      = LOG_PM_SUBS_INFO_LOG_PACKET_VERSION;
    log_ptr->subs         = SYS_MODEM_AS_ID_1;
    log_ptr->flags        = 0;
    log_ptr->feature0     = 0;
    log_ptr->changed_mask = 0x00000000;
    log_ptr->rat_order_table_len    = 0;
    log_ptr->num_rat_order_entries  = 0;
    log_ptr->voice_dom_pref = 0;
  }

  return log_ptr;
}

struct policyman_notify_history_entry_s {
  size_t                    next;
  uint32                    timestamp;
  LOG_PM_SUBS_INFO_type     subsItems[MAX_AS_IDS];
  LOG_PM_CONFIG_INFO_type   deviceConfig;
};

struct policyman_notify_history_s {
  size_t                            index;
  policyman_notify_history_entry_t  h[PM_DUMP_HISTORY_DEPTH];
};

/*-------- policyman_cfgitem_init_notify_history --------*/
void policyman_cfgitem_init_notify_history(
  policyman_state_t  *pState
)
{
  if (NULL == pState->pNotifyHistory)
  {
    int   idx;

    pState->pNotifyHistory = (policyman_notify_history_t *)policyman_mem_alloc(sizeof(policyman_notify_history_t));

    for (idx = 0; idx < (PM_DUMP_HISTORY_DEPTH); ++idx)
    {
      pState->pNotifyHistory->h[idx].next      = idx + 1;
      pState->pNotifyHistory->h[idx].timestamp = 0xC0FFEE;
    }

    pState->pNotifyHistory->h[PM_DUMP_HISTORY_DEPTH - 1].next = 0;
  }
}

/*-------- policyman_cfgitem_deinit_notify_history --------*/
void policyman_cfgitem_deinit_notify_history(
  policyman_state_t  *pState
)
{
  POLICYMAN_MEM_FREE_IF(pState->pNotifyHistory);
}

/*-------- prepare_next_notify_history_entry --------*/
int prepare_next_notify_history_entry(
  void
)
{
  policyman_state_t                *pState;
  int                               notifyHistoryIndex;
  policyman_notify_history_entry_t *notifyHistoryEntry;

  pState = policyman_state_get_state();

  notifyHistoryIndex = pState->pNotifyHistory->index;
  notifyHistoryEntry = &pState->pNotifyHistory->h[notifyHistoryIndex];

  // Set timestamp
  notifyHistoryEntry->timestamp  = mre_util_get_time();

  // Clear this entry
  memset( &notifyHistoryEntry->deviceConfig,
          0,
          sizeof(notifyHistoryEntry->deviceConfig) );

  memset( &notifyHistoryEntry->subsItems,
          0,
          sizeof(notifyHistoryEntry->subsItems) );

  // Advance to next entry
  pState->pNotifyHistory->index = notifyHistoryEntry->next;

  return notifyHistoryIndex;
}

/*-------- policyman_cfgitem_update_notify_history_cfg --------*/
void policyman_cfgitem_update_notify_history_cfg(
  LOG_PM_CONFIG_INFO_type    *log_ptr,
  int                         notifyHistoryIndex
)
{
  policyman_state_t      *pState;

  pState = policyman_state_get_state();

  if (NULL != pState->pNotifyHistory)
  {
    pState->pNotifyHistory->h[notifyHistoryIndex].deviceConfig = *log_ptr;
  }
}

/*-------- policyman_cfgitem_update_notify_history_subs --------*/
void policyman_cfgitem_update_notify_history_subs(
  size_t                      subsIndex,
  LOG_PM_SUBS_INFO_type      *log_ptr,
  int                         notifyHistoryIndex
)
{
  policyman_state_t      *pState;

  pState = policyman_state_get_state();

  if (NULL != pState->pNotifyHistory)
  {
    pState->pNotifyHistory->h[notifyHistoryIndex].subsItems[subsIndex] = *log_ptr;
  }
}

/*-------- policyman_cfgitem_send_diag_subs_log --------*/
void policyman_cfgitem_send_diag_subs_log(
  size_t  subsIndex,
  int     notifyHistoryIndex
)
{
  LOG_PM_SUBS_INFO_type      *log_ptr;

  if (!policyman_cfgitem_subs_has_changes(subsIndex))
  {
    POLICYMAN_UT_MSG_1("send_diag_subs_log: no changes to subs %d", subsIndex);
    return;
  }

  log_ptr = policyman_cfgitem_allocate_and_init_diag_subs_log();

  if (NULL != log_ptr)
  {
    policyman_cfgitem_fill_diag_subs_log(subsIndex, log_ptr);

    policyman_cfgitem_update_notify_history_subs(subsIndex, log_ptr, notifyHistoryIndex);

    /* Submit the log to DIAG.
    */
    log_commit( (void *)log_ptr );
  }
}

/*-------- policyman_cfgitem_send_diag_logs --------*/
void policyman_cfgitem_send_diag_logs(
  mre_set_t    *pNotifySet
)
{
  size_t  subsIndex;
  int     notifyHistoryEntryIdx;

  if (mre_set_is_empty(pNotifySet))
  {
    return;
  }

  notifyHistoryEntryIdx = prepare_next_notify_history_entry();

  policyman_cfgitem_send_diag_cfg_log(notifyHistoryEntryIdx);

  for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; ++subsIndex)
  {
    policyman_cfgitem_send_diag_subs_log(subsIndex, notifyHistoryEntryIdx);
  }
}

/*-------- policyman_notify_history_entry_to_dump --------*/
STATIC void policyman_notify_history_entry_to_dump(
  policyman_notify_history_entry_t *hist_entry,
  pm_dump_notify_history_t         *dump_entry
)
{
  size_t  idx;

  dump_entry->timestamp      = hist_entry->timestamp;
  dump_entry->deviceConfig   = hist_entry->deviceConfig;

  for (idx = SYS_MODEM_AS_ID_1; idx < POLICYMAN_NUM_SUBS; ++idx)
  {
    dump_entry->subsItems[idx]   = hist_entry->subsItems[idx];
  }
}

/*-------- policyman_notify_get_history --------*/
size_t policyman_notify_get_history(
  pm_dump_notify_history_t *h_buffer,
  size_t                    h_len
)
{
  policyman_state_t      *pState;
  int                     oldest_hist_idx;
  int                     start_idx;
  int                     hist_idx;
  int                     offset;

  pState = policyman_state_get_state();

  // If given length larger than history, get only max history available
  if (h_len > PM_DUMP_HISTORY_DEPTH)
  {
    h_len = PM_DUMP_HISTORY_DEPTH;
  }

  oldest_hist_idx = pState->pNotifyHistory->index;  // points to oldest history record

  // Calculate the index to start with for the most recent h_len history entries
  start_idx = (oldest_hist_idx + (PM_DUMP_HISTORY_DEPTH - h_len)) % PM_DUMP_HISTORY_DEPTH;

  hist_idx = start_idx;

  for (offset = 0; offset < h_len; offset++ )
  {
    policyman_notify_history_entry_to_dump( &pState->pNotifyHistory->h[hist_idx],
                                            &h_buffer[offset] );

    hist_idx = pState->pNotifyHistory->h[hist_idx].next;
  }

  return h_len;
}

/*-------- STRUCT policyman_cmd_dump_t --------*/
typedef struct {
  POLICYMAN_CMD_HDR;

  int                dump_type;   // Was: qsh_dump_type_e
  boolean            async_allowed;
  void              *dump_addr;
  uint32             context_id;
  void              *client_data;
} policyman_cmd_dump_t;

#define QSH_DUMP_TYPE_FULL 1
#define QSH_DUMP_TYPE_MINI 0

/*-------- policyman_cfgitem_fill_dump_buffer --------*/
void policyman_cfgitem_fill_dump_buffer(
  policyman_cmd_t    *pCmd
)
{
  policyman_cmd_dump_t   *pDumpCmd     = (policyman_cmd_dump_t *)pCmd;
  size_t                  size;

  // Collect dump data
  switch (pDumpCmd->dump_type)
  {
    case QSH_DUMP_TYPE_FULL:
      {
        LOG_PM_DUMP_INFO_type *pDump;

        pDump = (LOG_PM_DUMP_INFO_type *)pDumpCmd->dump_addr;
        size = sizeof(LOG_PM_DUMP_INFO_type);

        (void)policyman_cmss_get_history( pDump->history_cm_ss,
                                          ARR_SIZE(pDump->history_cm_ss) );

        (void)policyman_cmph_get_history( pDump->history_cm_ph,
                                          ARR_SIZE(pDump->history_cm_ph) );

        (void)policyman_cmcall_get_history( pDump->history_cm_call,
                                            ARR_SIZE(pDump->history_cm_call) );

        policyman_policies_gather_statistics( 0,  // first policy only
                                              &pDump->policy_info );
      }
      // fallthrough

    case QSH_DUMP_TYPE_MINI:
      {
        LOG_PM_MINI_DUMP_INFO_type *pDump;
        size_t                      subs_idx;

        pDump = (LOG_PM_MINI_DUMP_INFO_type *)pDumpCmd->dump_addr;
        if (QSH_DUMP_TYPE_MINI == pDumpCmd->dump_type)
        {
          // Only reset size if not falling through
          size = sizeof(LOG_PM_MINI_DUMP_INFO_type);
        }

        pDump->version   = LOG_PM_DUMP_INFO_LOG_PACKET_VERSION;
        pDump->dump_time = mre_util_get_time();
        pDump->is_crash  = !pDumpCmd->async_allowed;

        policyman_device_config_fill_diag_cfg_log( (device_configuration_t *)policyman_cfgitems[0].pItem,
                                                   &pDump->device_config.config_info );

        for (subs_idx = (size_t)SYS_MODEM_AS_ID_1; subs_idx < MAX_AS_IDS; subs_idx++)
        {
          policyman_state_fill_diag_subs_info( subs_idx,
                                               &pDump->subs[subs_idx] );

          policyman_cfgitem_fill_diag_subs_log( subs_idx,
                                                &pDump->subs[subs_idx].subs_log_info );
        }

        policyman_uim_fill_diag_session_info( ARR_SIZE(pDump->uim_session_info),
                                              pDump->uim_session_info );
      }
      break;

    default:
      break;
  }
  // End collection

#if defined(OLD_QSH_IFACE)
  {
    char                   *alt_name     = NULL;
    size_t                  alt_name_len = 0;

    policyman_task_qsh_cb_done( pDumpCmd->context_id,
                                size,
                                alt_name,
                                alt_name_len );
  }
#endif /* defined(OLD_QSH_IFACE) */

  if (pDumpCmd->async_allowed)
  {
    ref_cnt_obj_release(pCmd);
  }
}

#if defined(OLD_QSH_IFACE)
/*-------- policyman_cfgitem_handle_dump --------*/
void policyman_cfgitem_handle_dump(
  qsh_cb_params_s   *params
)
{
  policyman_cmd_dump_t    *pCmd;
  policyman_cmd_dump_t     dump_cmd;  // for non-async

  if (params->async_allowed)
  {
    pCmd = POLICYMAN_CMD_NEW( policyman_cmd_dump_t,
                              policyman_cfgitem_fill_dump_buffer,
                              NULL,
                              NULL );
  }
  else
  {
    // For crash scenario, use local cmd
    pCmd = &dump_cmd;
  }

  pCmd->dump_type     = params->dump_type;
  pCmd->async_allowed = params->async_allowed;
  pCmd->dump_addr     = params->dump_addr;
  pCmd->context_id    = params->context_id;
  pCmd->client_data   = params->client_data;

  if (params->async_allowed)
  {
    policyman_queue_put_cmd((policyman_cmd_t *)pCmd);
  }
  else
  {
    policyman_cfgitem_fill_dump_buffer((policyman_cmd_t *)pCmd );
  }
}
#endif /* defined(OLD_QSH_IFACE) */

/*-------- add_cfgitem_to_set --------*/
STATIC boolean add_cfgitem_to_set(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  mre_set_t               *pItemSet = (mre_set_t *) pCtx;

  mre_set_add(pItemSet, &pCfgItem->pItem);

  return TRUE;
}


/*-------- policyman_send_all_config_items --------*/
void policyman_send_all_config_items(
  void
)
{
  policyman_set_t   *pItemSet;

  pItemSet = policyman_itemset_new();

  policyman_cfgitems_iterate(add_cfgitem_to_set, pItemSet);
  policyman_cfgitem_notify_clients(pItemSet, NULL);
  policyman_persist_cfgitems(pItemSet);

  ref_cnt_obj_release(pItemSet);
}


/*-------- ensure_item --------*/
STATIC void ensure_item(
  policyman_config_item_t *pCfgItem,
  policyman_set_t         *pItemSet
)
{
  if (NULL != pCfgItem->pCfgInfo->pfnGetDefault)
  {
    policy_execute_ctx_t  ensure_ctx;
    policyman_item_t     *pItem;

    ensure_ctx.asubs_id =  pCfgItem->subsId;
    ensure_ctx.pItemSet =  pItemSet;

    pItem = pCfgItem->pCfgInfo->pfnGetDefault(&ensure_ctx);
    policyman_set_add(pItemSet, &pItem);
    ref_cnt_obj_release(pItem);
  }
}


/*-------- config_item_is_in_specified_subs --------*/
STATIC boolean config_item_is_in_specified_subs(
  policyman_config_item_t   *pCfgItem,
  sys_modem_as_id_e_type     subsId
)
{
  return (    (subsId == SYS_MODEM_AS_ID_NONE)
          || ( (pCfgItem->subsId == subsId)
                && !policyman_config_item_is_device_level(pCfgItem)) );
}


/*-------- ensure_device_item --------*/
STATIC boolean ensure_device_item(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  policyman_set_t         *pItemSet = (policyman_set_t *)pCtx;

  if (POLICYMAN_ITEM_DEVICE_CONFIGURATION == pCfgItem->pCfgInfo->id)
  {
    ensure_item(pCfgItem, pItemSet);
  }
  return TRUE;
}


/*-------- policyman_ensure_device_config_item --------*/
void policyman_ensure_device_config_item(
  policyman_set_t       *pItemSet
)
{
  policyman_cfgitems_iterate(ensure_device_item, pItemSet);
}


/*-------- ensure_item_per_subs --------*/
STATIC boolean ensure_item_per_subs(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  policy_execute_ctx_t    *ensure_ctx = (policy_execute_ctx_t *)pCtx;

  if (config_item_is_in_specified_subs(pCfgItem, ensure_ctx->asubs_id))
  {
    boolean                  item_is_present;

    item_is_present  = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)ensure_ctx->pItemSet,
                                                                pCfgItem->pCfgInfo->id,
                                                                pCfgItem->subsId,
                                                                NULL);

    if (!item_is_present)
    {
      ensure_item(pCfgItem, (policyman_set_t *)ensure_ctx->pItemSet );
    }
  }

  return TRUE;
}


/*-------- policyman_cfgitem_ensure_items_per_subs --------*/
void policyman_cfgitem_ensure_items_per_subs(
  policyman_set_t        *pItemSet,
  sys_modem_as_id_e_type  subs_id
)
{
  policy_execute_ctx_t   ensure_ctx;
  
  ensure_ctx.asubs_id =  subs_id;
  ensure_ctx.pItemSet =  pItemSet;

  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "========== Ensure subs database items", subs_id);

  policyman_cfgitems_iterate(ensure_item_per_subs, &ensure_ctx);
}

/*-------- policyman_update_subphone_cap_in_itemset --------*/
boolean policyman_update_subphone_cap_in_itemset(
  policyman_set_t     *pItemSet
)
{
  size_t                   nSim;
  policyman_status_t       status;
  size_t                   subsId;

  status = policyman_get_current_num_sim(&nSim);

  for (subsId = SYS_MODEM_AS_ID_1; POLICYMAN_SUCCEEDED(status) && subsId < nSim; subsId++)
  {
    rat_capability_t        *pRatCap;
    boolean                  item_is_present;
    policy_execute_ctx_t     ctx;

    ctx.pItemSet = pItemSet;
    ctx.asubs_id = subsId;

    item_is_present = policyman_cfgitem_is_present_in_itemset( pItemSet,
                                                                POLICYMAN_ITEM_RAT_CAPABILITY,
                                                                subsId,
                                                               (policyman_item_t  **)&pRatCap);

    if (item_is_present)
    {
      pRatCap->subphoneCap = policyman_rat_capability_get_max_config(&ctx);
    }
  }

  return TRUE;
}

/*-------- get_device_config_info --------*/
STATIC boolean get_device_config_info(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t  *pThisItem   = (policyman_config_item_t *) pElem;
  policyman_config_item_t **ppFoundItem = (policyman_config_item_t **) pCtx;

  if (pThisItem->pCfgInfo->id == POLICYMAN_ITEM_DEVICE_CONFIGURATION)
  {
    *ppFoundItem = pThisItem;
    return FALSE;
  }

  return TRUE;
}

/*-------- policyman_cfgitem_get_device_config --------*/
policyman_item_t *policyman_cfgitem_get_device_config(
  void
)
{
  policyman_config_item_t *pCfg = NULL;

  policyman_cfgitems_iterate(get_device_config_info, &pCfg);

  return (pCfg != NULL) ? pCfg->pItem : NULL;
}

/*-------- policyman_item_ensure_device_config --------*/
void policyman_item_ensure_device_config(
  policyman_set_t   *pItemSet
)
{
  boolean            item_is_present;

  POLICYMAN_MSG_HIGH_0("========== Ensure device config");

  item_is_present = policyman_cfgitem_is_present_in_itemset( pItemSet,
                                                             POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                             SYS_MODEM_AS_ID_1,
                                                             NULL);
  if (!item_is_present)
  {
    policyman_ensure_device_config_item(pItemSet);
  }
}

/*-------- policyman_ensure_items_in_itemset --------*/
void policyman_ensure_items_in_itemset(
  policyman_set_t  *pItemSet
)
{
  size_t  subsIndex;

  // Ensure device level initialization
  policyman_item_ensure_device_config(pItemSet);
  policyman_ensure_configurations(pItemSet);

  // Ensure subs level initialization
  for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; subsIndex++)
  {
    policyman_cfgitem_ensure_items_per_subs(pItemSet, subsIndex);
  }

  policyman_update_subphone_cap_in_itemset(pItemSet);
}

/*-------- config_item_deinit --------*/
STATIC boolean config_item_deinit(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  sys_modem_as_id_e_type  *psubsId  = (sys_modem_as_id_e_type *)pCtx;

  if (config_item_is_in_specified_subs(pCfgItem, *psubsId))
  {
    if (NULL != pCfgItem->pItem)
    {
      ref_cnt_obj_release(pCfgItem->pItem);
      pCfgItem->pItem = NULL;
    }
  }

  return TRUE;
}


/*-------- policyman_cfgitem_deinit --------*/
void policyman_cfgitem_deinit(
  void
)
{
  sys_modem_as_id_e_type  subsid = SYS_MODEM_AS_ID_NONE;
  
  policyman_cfgitems_iterate(config_item_deinit, (void *)&subsid);

  policyman_deinit_crit_sect(&g_blockedSignalsCS);
  REF_CNT_OBJ_RELEASE_IF(g_pBlockedSignalSet);
}


/*-------- policyman_cfgitem_new_item --------*/
policyman_item_t* policyman_cfgitem_new_item(
  policyman_item_id_t    id,
  size_t                 itemSize,
  destructor_ptr         dtor
  )
{
  policyman_item_t  *pItem;

  if (id == POLICYMAN_ITEM_INVALID)
  {
    POLICYMAN_ERR_FATAL("Attempting to create policyman item with invalid ID", 0, 0, 0);
  }

  pItem = policyman_mem_alloc(itemSize);
  ref_cnt_obj_init(pItem, dtor);
  
  pItem->pCfgInfo = &policyman_cfg_info[id - 1];

  return pItem;
}



/*-------- policyman_cfgitem_find_item --------*/
/**
@brief  Find an item in the configuration item database.  Private function.

@param[in]  id               ID of the configuration item.

@return
  - Pointer to the configuration item if it is in the database.
  - NULL otherwise.
*/
STATIC policyman_config_item_t* policyman_cfgitem_find_item(
  policyman_item_id_t    id,
  sys_modem_as_id_e_type asubs_id
  )
{
  size_t  itr;
  size_t  start_index;
  size_t  end_index;

  if (id == POLICYMAN_ITEM_DEVICE_CONFIGURATION)
  {
    return &policyman_cfgitems[0];
  }

  if(  asubs_id == SYS_MODEM_AS_ID_NONE
     || asubs_id >= (sys_modem_as_id_e_type) POLICYMAN_NUM_SUBS
    )
  {
    POLICYMAN_MSG_ERROR_1("Unsupported subsId %d", asubs_id);
    return NULL;
  }
  /*
      policyman_cfgitems[] = {device_config_item , 
                                           NUM_PM_SUBS_ITEMS of AS_ID_1 cfg items,
                                           NUM_PM_SUBS_ITEMS of AS_ID_2 cfg items... }
    
       set start index to the start of config items corresponding to asubs_id
   */
  start_index = (NUM_PM_SUBS_ITEMS * asubs_id) + 1;
  end_index = start_index + NUM_PM_SUBS_ITEMS;

  for (itr = start_index ; itr < end_index ; ++itr)
  {
    if (policyman_cfgitems[itr].pCfgInfo->id == id)
    {
      if (policyman_cfgitems[itr].pItem == NULL ||
          policyman_cfgitems[itr].pItem->asubs_id == asubs_id
         )
      {
        return &policyman_cfgitems[itr];
      }
    }
  }

  return NULL;
}

/*-------- policyman_items_equal --------*/
static boolean policyman_items_equal(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
)
{
  if (pItem1 == NULL || pItem2 == NULL)
  {
    return FALSE;
  }
  
  return pItem1->pCfgInfo->pfnItemsEqual(pItem1, pItem2);
}


/*-------- policyman_cfgitem_update_item --------*/
/**
@brief  Update (or add) an item in the configuration database.  Private function.

@param[in]  pNewItem    Pointer to the item with which to replace the old
                        configuration item.
@param[in]  asubs_id   Subscription Id for which old configuration item to be replaced.

@return
  TRUE if item was updated (i.e. the new item differs from the cached item).
*/
STATIC boolean policyman_cfgitem_update_item(
  policyman_item_t        *pNewItem
)
{
  policyman_item_id_t       id;
  policyman_config_item_t  *pInfo;
  boolean                   itemUpdated = FALSE;

  id    = policyman_cfgitem_id(pNewItem);
  pInfo = policyman_cfgitem_find_item(id, pNewItem->asubs_id);

  if (pInfo != NULL)
  {
    policyman_item_t         *pOldItem    = NULL;

    pOldItem = pInfo->pItem;
    itemUpdated = !policyman_items_equal(pNewItem, pOldItem);

    if (itemUpdated)
    {
      /* Add a reference on the new item for the array.
      */
      ref_cnt_obj_add_ref(pNewItem);
      pInfo->pItem = pNewItem;
      
      // Mark the item as changed since last notification
      pInfo->is_changed = TRUE;

      if (pInfo->pCfgInfo->pfnOnUpdate != NULL)
      {
        if (!policyman_is_initializing())
        {
          pInfo->pCfgInfo->pfnOnUpdate(pNewItem);
        }
      }

      if (pInfo->pCfgInfo->pfnDisplay != NULL)
      {
        pInfo->pCfgInfo->pfnDisplay(pNewItem);
      }

      REF_CNT_OBJ_RELEASE_IF(pOldItem);
    }
  }

  return itemUpdated;
}


/*-------- policyman_cfgitem_release_msg_attach --------*/
void
policyman_cfgitem_release_msg_attach(
  msgr_hdr_s  *pMsg
  )
{
  msgr_attach_s *pAttach;
  size_t        i;

  /*  Release all reference-counted attachments
  */
  for (i = 0 ; i < msgr_get_num_attach(pMsg) ; ++i)
  {
    pAttach = msgr_get_attach(pMsg, i);
    if (pAttach->type == MSGR_ATTACH_REFCNT)
    {
      REF_CNT_OBJ_RELEASE_IF(pAttach->data.refcnt.refcnt_obj_ptr);
    }
  }
}


/*-------- policyman_item_collection_dtor --------*/
STATIC void policyman_item_collection_dtor(
  void  *pObj
  )
{
  policyman_item_collection_t *pCollection;
  size_t                      i;

  pCollection = (policyman_item_collection_t *) pObj;

  for (i = 0 ; i < pCollection->numItems ; ++i)
  {
    ref_cnt_obj_release((void *) pCollection->pItems[i]);
  }

  policyman_mem_free(pObj);
}


/*-------- policyman_cfgitem_notify_msg_dtor --------*/
void policyman_cfgitem_notify_msg_dtor(
  void  *pObject
  )
{
  msgr_hdr_s    *pMsg = (msgr_hdr_s *) pObject;

  policyman_cfgitem_release_msg_attach(pMsg);

  /*  Free the message memory.
   */
  policyman_mem_free(pMsg);
}


/*-------- policyman_item_collection_new --------*/
STATIC policyman_item_collection_t * policyman_item_collection_new(
  size_t            nItems,
  policyman_item_t  **ppItems,
  boolean           addRef
  )
{
  policyman_item_collection_t *pCollection;
  size_t                      bufSize = nItems * sizeof(policyman_item_t *);
  size_t                      i;

  /*  Allocate a collection of the proper size and initialize it.
   */
  pCollection = (policyman_item_collection_t *)
                    policyman_mem_alloc(sizeof(policyman_item_collection_t) + bufSize);
  ref_cnt_obj_init(pCollection, policyman_item_collection_dtor);

  /*  Set the sequence number of the collection to the current database sequence number.
   */
  pCollection->seqno = sequence_number_get();

  /*  Add a reference to all items going into the collection.
   */
  if (addRef)
  {
    for (i = 0 ; i < nItems ; ++i)
    {
      ref_cnt_obj_add_ref(ppItems[i]);
    }
  }

  /*  Add the items to the collection.
   */
  pCollection->numItems = nItems;
  memscpy((policyman_item_t *) pCollection->pItems, bufSize, ppItems, bufSize);

  policyman_ue_mode_normalize_collection(pCollection);

  return pCollection;
}


/*-------- policyman_cfgitem_notify_msg_msim_new --------*/
msgr_hdr_s *policyman_cfgitem_notify_msg_msim_new(
  policyman_item_t  **ppItems,
  size_t            numItems,
  size_t            *pMsgSize
)
{
  size_t                      msgSize = sizeof(msgr_hdr_s) + sizeof(msgr_attach_s);
  msgr_hdr_s                  *pMsg;
  msgr_attach_s               *pAttach;
  policyman_item_collection_t *pCollection;

  /*  Allocate a message with a single attachment (the collection).
   */
  pMsg = policyman_mem_alloc(msgSize);

  /*  Set up the message.
   */
  msgr_init_hdr_attach( pMsg,
                        MSGR_POLICYMAN_CFG,
                        POLICYMAN_CFG_UPDATE_MSIM_IND,
                        MSGR_PRIORITY_HIGH,
                        1 );

  /*  Get the items into a collection and attach the collection to the message.
   *  Have the collection add references for each item.
   */
  pCollection = policyman_item_collection_new(numItems, ppItems, TRUE);
  pAttach = msgr_get_attach(pMsg, 0);
  if (pAttach != NULL)
  {
    pAttach->type = MSGR_ATTACH_REFCNT;
    pAttach->data.refcnt.refcnt_obj_ptr = pCollection;
  }

  if (pMsgSize != NULL)
  {
    *pMsgSize = msgSize;
  }

  return pMsg;
}

/*-------- policyman_cfgitem_update_notification_hook --------*/
/**
@brief  Hook function for notification of the result of subs updates.

This creates another MSGR message (not sent by the MSGR) that is stored in the
command that initiated the notification.  The signal to unblock the thread
that sent the update is then set, which releases it and returns the message
to the caller of policyman_report_preference_change().
*/
void policyman_cfgitem_update_notification_hook(
  void               *pContext,
  size_t              nItems,
  policyman_item_t  **ppItems
)
{
  policyman_cmd_t *pCmd = (policyman_cmd_t *) pContext;

  /*  Create a new MSGR message and store it in the command.
   */
  pCmd->pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, nItems, NULL);

  /*  Unblock the thread that issued the command.
   */
  policyman_client_block_signal_set(pCmd->signal);
}

/*-------- policyman_cfgitem_display --------*/
boolean policyman_cfgitem_display(
  void  *pElem,
  void  *pCtx
  )
{
  policyman_item_t  *pItem = *((policyman_item_t **) pElem);

  if (NULL != pItem && NULL != pItem->pCfgInfo)
  {
    pItem->pCfgInfo->pfnDisplay(pItem);
  }

  return TRUE;
}


/*-------- config_item_display --------*/
STATIC boolean config_item_display(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t  *pCfgItem = (policyman_config_item_t *) pElem;

  return  policyman_cfgitem_display(&pCfgItem->pItem, pCtx);
}


/*-------- policyman_cfgitem_display_all --------*/
void policyman_cfgitem_display_all(
  void
)
{
  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
  POLICYMAN_MSG_HIGH_1("Item database sequence#: %d", sequence_number_get());
  policyman_cfgitems_iterate(config_item_display, NULL);
  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
}


STATIC boolean set_item_seqno(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t  *pItem = *((policyman_item_t **) pElem);
  size_t             seqno = (size_t) pCtx;

  pItem->seqno = seqno;

  return TRUE;
}


/*-------- policyman_cfgitem_notify_clients --------*/
STATIC void policyman_cfgitem_notify_clients(
  policyman_set_t *pItemSet,
  policyman_cmd_t *pCmd
)
{
  size_t              nItems;
  policyman_item_t  **ppItems;
  size_t              msgSize;
  msgr_hdr_s         *pMsg;

  /*  Get the number of items to be udpated and a pointer to the buffer
   *  in the itemSet that holds those pointers.
   */
  nItems = policyman_set_num_elems(pItemSet);
  ppItems = (policyman_item_t **) policyman_set_get_buf_ptr(pItemSet);

  /*  Increment database sequence number and set it as the sequence number
   *  on all the items.
   */
  sequence_number_increment();
  mre_set_iterate(pItemSet, set_item_seqno, (void *) sequence_number_get());

  POLICYMAN_MSG_HIGH_2( "********** Notifying clients of changes to %d items, sequence #%d:",
                        nItems,
                        sequence_number_get() );

  mre_set_iterate(pItemSet, policyman_cfgitem_display, NULL);

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_STAR);

  // call client hook if it is set
  if (pCmd != NULL && pCmd->hook != NULL)
  {
    pCmd->hook(pCmd, nItems, ppItems);
    POLICYMAN_MSG_HIGH_0("cmd hook called for processing");
  }

  /*  Send the MSIM message
   */
  pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, nItems, &msgSize);
  msgr_send(pMsg, msgSize);
  policyman_cfgitem_notify_msg_dtor(pMsg);

  policyman_cfgitem_send_diag_logs(pItemSet);
  policyman_cfgitem_reset_changed();
}


/*-------- update_item --------*/
STATIC boolean update_item(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t       *pItem       = *((policyman_item_t **) pElem);
  policyman_set_t         *pNotifySet = (policyman_set_t *) pCtx;
  boolean                 itemUpdated;

  itemUpdated = policyman_cfgitem_update_item(pItem);

  if (itemUpdated && pNotifySet != NULL)
  {
    policyman_set_add(pNotifySet, &pItem);
  }

  return TRUE;
}

/*-------- compare_item --------*/
STATIC boolean find_and_collect_changed_items(
  void  *pElem,
  void  *pCtx
)
{
  policyman_item_t        *pNewItem    = *((policyman_item_t **) pElem);
  policyman_set_t         *pNotifySet  = (policyman_set_t *) pCtx;
  policyman_item_t        *pActualItem = NULL;
  sys_modem_as_id_e_type   asubs_id;
  policyman_item_id_t      item_id;
  policyman_config_item_t *pCfgItem;
  boolean                  itemSame;

  item_id   = policyman_cfgitem_id_msim(pNewItem, &asubs_id);
  pCfgItem  = policyman_cfgitem_find_item(item_id, asubs_id);

  if (NULL != pCfgItem)
  {
    pActualItem = pCfgItem->pItem;
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("Unable to retrieve changed config item id %d", item_id);
    return FALSE;
  }

  itemSame = policyman_items_equal(pNewItem, pActualItem);
  if (!itemSame )
  {
    policyman_set_add(pNotifySet, &pNewItem);
  }

  return TRUE;
}

/*-------- policyman_cfgitem_persist_and_notify --------*/
STATIC void policyman_cfgitem_persist_and_notify(
  policyman_set_t *pNotifySet,
  policyman_cmd_t *pCmd
)
{
  /* Notify all clients of the changes. Don't bother during initialization. */
  if (pNotifySet != NULL && !policyman_set_is_empty(pNotifySet))
  {
    /* Update EFS writing of updating policy items
      */
    policyman_persist_cfgitems(pNotifySet);
    policyman_cfgitem_notify_clients(pNotifySet, pCmd);
  }
  else
  {
    POLICYMAN_MSG_HIGH_0("No configuration items need updating");
  }
}

/*-------- policyman_cfgitem_set_limited_capabilities --------*/
void policyman_cfgitem_set_limited_capabilities(
  void
)
{
  limitedCapabilityIsEnabled = TRUE;
}

/*-------- policyman_cfgitem_limited_capabilities_is_enabled --------*/
boolean policyman_cfgitem_limited_capabilities_is_enabled(
  void
)
{
  return limitedCapabilityIsEnabled;
}

/*-------- handle_item_update --------*/
STATIC void handle_item_update(
  mre_set_t       *pItemSet,
  mre_set_t       *pNotifySet,
  policyman_cmd_t *pCmd,
  boolean          handleRatConcurrency
)
{
  policyman_enter_crit_sect(&policyman_item_crit_sect);

  if (handleRatConcurrency)
  {
    policyman_set_rat_capability_concurrency_masks(pItemSet);
  }
  
  policyman_update_rat_for_thermal_level(pItemSet);
  mre_set_iterate(pItemSet, update_item, pNotifySet);
  policyman_cfgitem_persist_and_notify(pNotifySet, pCmd);
  
  policyman_leave_crit_sect(&policyman_item_crit_sect);
}

/*-------- get_limited_capability_item --------*/
STATIC boolean get_limited_capability_item(
  void  *pElem,
  void  *pItemSet
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;

  if (NULL != pCfgItem->pCfgInfo->pfnGetLimitedItem)
  {
    pCfgItem->pCfgInfo->pfnGetLimitedItem((mre_set_t *)pItemSet, pCfgItem->subsId);
  }

  return TRUE;
}


/*-------- set_limited_capabilities --------*/
STATIC void set_limited_capabilities(
  void
)
{
  if (policyman_cfgitem_limited_capabilities_is_enabled())
  {
    mre_set_t   *pItemSet;

    pItemSet   = policyman_itemset_new();

    // get limited values in itemset
    policyman_cfgitems_iterate(get_limited_capability_item, pItemSet);

    // ensure config database is complete
    policyman_ensure_items_in_itemset(pItemSet);

    // passing Null notifySet to ensure client notification is not sent
    handle_item_update(pItemSet, NULL, NULL, FALSE);

    REF_CNT_OBJ_RELEASE_IF(pItemSet);
  }
}

/*-------- policyman_cfgitem_update_items --------*/
boolean policyman_cfgitem_update_items(
  policyman_set_t *pItemSet,
  policyman_cmd_t *pCmd
)
{
  boolean inLimitedCapability;

  inLimitedCapability = policyman_cfgitem_limited_capabilities_is_enabled();

  if (!inLimitedCapability && policyman_dbupdate_try_enter_update_state())
  {
    policyman_set_t *pNotifySet = NULL;

  if (policyman_is_suspended())
  {
    return TRUE;
  }

  if (!policyman_is_initializing())
  {
    pNotifySet = policyman_itemset_new();
  }

  handle_item_update(pItemSet, pNotifySet, pCmd, TRUE);
  REF_CNT_OBJ_RELEASE_IF(pNotifySet);

  policyman_dbupdate_enter_idle_state();
  dbupdate_release_blocked_clients(); //release blocked clients if any

  return TRUE;
}
  else
  {
    POLICYMAN_MSG_HIGH_2( "discarding itemset: inLimitedCapability %d, g_dbUpdateState is %d (NONE-0/SUSPENDED-1/IN_PROGRESS-2)", 
                          inLimitedCapability,
                          g_dbUpdateState.value);

    set_limited_capabilities();
    return FALSE;
  }
}


/*-------- policyman_cfgitem_id --------*/
policyman_item_id_t policyman_cfgitem_id(
  policyman_item_t const  *pItem
  )
{
  return pItem->pCfgInfo->id;
}



/*-------- policyman_cfgitem_id_msim --------*/
policyman_item_id_t policyman_cfgitem_id_msim(
  policyman_item_t const  *pItem,
  sys_modem_as_id_e_type  *pSubsId
  )
{
  *pSubsId = pItem->asubs_id;
  return policyman_cfgitem_id(pItem);
}


/*-------- policyman_cfgitem_get_items --------*/
size_t policyman_cfgitem_get_items(
  policyman_item_id_t const *pItemIds,
  size_t                  numIds,
  policyman_item_t const  **ppItems
  )
{
  size_t                    i;
  size_t                    numItems = 0;
  policyman_config_item_t  *pItemInfo;
  policyman_item_t          *pItem;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; i < numIds ; ++i)
  {
    POLICYMAN_CL_MSG_LOW_2( "Client thread 0x%x requested item id %d",
                            POLICYMAN_CALLER_THREAD_ID,
                             pItemIds[i]);

    /* Default search for subscription 1
       Use MSIM API if items needed for other subscriptions
      */    
    pItemInfo = policyman_cfgitem_find_item(pItemIds[i], SYS_MODEM_AS_ID_1);
    if (pItemInfo != NULL)
    {
      pItem = pItemInfo->pItem;
      if (pItem != NULL)
      {
        ref_cnt_obj_add_ref(pItem);
        ppItems[numItems++] = pItem;
      }
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);

  return numItems;
}


typedef struct
{
  policyman_item_id_t     id;
  sys_modem_as_id_e_type  subs;
  policyman_item_t       *pItem;
} get_cfgitem_ctx_t;


/*-------- get_cfgitem --------*/
STATIC boolean get_cfgitem(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t  *pCfgItem  = (policyman_config_item_t *) pElem;
  get_cfgitem_ctx_t        *pGetCtx   = (get_cfgitem_ctx_t *) pCtx;
  policyman_item_t         *pItem;

  pItem = pCfgItem->pItem;
  if (    pItem != NULL
       && pCfgItem->subsId == pGetCtx->subs
       && pCfgItem->pCfgInfo != NULL
       && pCfgItem->pCfgInfo->id == pGetCtx->id
     )
  {
    ref_cnt_obj_add_ref(pItem);
    pGetCtx->pItem = pItem;
    return FALSE;
  }

  return TRUE;
}


/*-------- policyman_cfgitem_get_item --------*/
policyman_item_t  const *policyman_cfgitem_get_item(
  policyman_item_id_t     id,
  sys_modem_as_id_e_type  subs
  )
{
  get_cfgitem_ctx_t  ctx;
  policyman_item_t  *pItem = NULL;

  ctx.id = id;
  ctx.subs = subs;
  ctx.pItem = NULL;

  policyman_cfgitems_iterate(get_cfgitem, &ctx);

  return ctx.pItem;
}


/*-------- policyman_cfgitem_is_present --------*/
boolean policyman_cfgitem_is_present(
  policyman_item_id_t    id,
  sys_modem_as_id_e_type subsId
)
{
  boolean                  isPresent = FALSE;
  policyman_item_t const  *pItem;
 
  pItem = policyman_cfgitem_get_item(id, subsId);

  if (pItem != NULL)
  {
    ref_cnt_obj_release((void *) pItem);
    isPresent = TRUE;
  }

  return isPresent;
}

/*-------- policyman_cfgitem_already_present --------*/
boolean policyman_cfgitem_already_present(
  policyman_item_id_t      itemId,
  sys_modem_as_id_e_type   policySubs,
  policyman_subs_t         targetSubs
)
{
  boolean                item_is_present = FALSE;
  size_t                 numSim          = 0;
  sys_modem_as_id_e_type subs;

  (void)policyman_get_current_num_sim(&numSim);

  switch (targetSubs)
  {
    case SUBS_THIS:
      item_is_present = policyman_cfgitem_is_present(itemId, policySubs);
      break;

    case SUBS_OTHER:
      for (subs = SYS_MODEM_AS_ID_1 ; subs < numSim ; ++subs)
      {
        if (   subs != policySubs
            && policyman_cfgitem_is_present(itemId, subs))
        {
          item_is_present = TRUE;
          break;
        }
      }
      break;

    case SUBS_ALL:
      for (subs = SYS_MODEM_AS_ID_1 ; subs < numSim ; ++subs)
      {
        if (policyman_cfgitem_is_present(itemId, subs))
        {
          item_is_present = TRUE;
          break;
        }
      }
      break;

    default:
      item_is_present = FALSE;
      break;
  }

  return item_is_present;
}

/*-------- policyman_cfgitem_id_in_request --------*/
/**
@brief  Return TRUE if the given ID was requested in a call to
        policyman_cfgitem_get_items_msim.
*/
STATIC boolean policyman_cfgitem_id_in_request(
  policyman_item_id_t const id,
  policyman_item_id_t const *pIds,
  size_t                    nIds
  )
{
  while (nIds-- != 0)
  {
    if (*pIds++ == id)
    {
      return TRUE;
    }
  }

  return FALSE;
}


/*-------- policyman_cfgitem_get_items_msim --------*/
policyman_status_t policyman_cfgitem_get_items_msim(
  policyman_item_id_t const   *pIds,
  size_t                      nIds,
  policyman_item_collection_t **ppItems
  )
{
  size_t                    i;
  size_t                    numItems = 0;
  policyman_item_t          *items[ARR_SIZE(policyman_cfg_info) * POLICYMAN_NUM_SUBS];

  /*  Enter the critical section and collect ref-counted copies of any items
   *  requested into our local buffer.
   */
  policyman_enter_crit_sect(&policyman_item_crit_sect);
  for (i = 0 ; i < ARR_SIZE(policyman_cfgitems) ; ++i)
  {
    if ((policyman_cfgitem_id_in_request(policyman_cfgitems[i].pCfgInfo->id, pIds, nIds))
        && (policyman_subs_id_is_configured(policyman_cfgitems[i].subsId)))
    {
      items[numItems] = policyman_cfgitems[i].pItem;

      if (items[numItems] == NULL) { continue; }

      ref_cnt_obj_add_ref(items[numItems]);
      ++numItems;
    }
  }
  policyman_leave_crit_sect(&policyman_item_crit_sect);

  /*  Now, outside the critical section, allocate memory for the item collection
   *  and copy the items to it.  We've already added a reference to the items
   *  while in the critical section, so no need for the collection to do so.
   */
  *ppItems = policyman_item_collection_new(numItems, items, FALSE);

  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_cfgitem_get_items_per_subs --------*/
size_t policyman_cfgitem_get_items_per_subs(
  policyman_item_id_t const *pItemIds,
  size_t                  numIds,
  policyman_item_t const  **ppItems,
  sys_modem_as_id_e_type   subs_id
  )
{
  size_t                  i;
  size_t                  numItems = 0;
  policyman_config_item_t  *pItemInfo;
  policyman_item_t        *pItem;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; i < numIds ; ++i)
  {
    POLICYMAN_CL_MSG_LOW_3( "Client thread 0x%x requested item id %d from subs %d",
                            POLICYMAN_CALLER_THREAD_ID,
                             pItemIds[i],
                             subs_id );

    /* Default search for subscription 1
       Use MSIM API if items needed for other subscriptions
      */    
    pItemInfo = policyman_cfgitem_find_item(pItemIds[i], subs_id);
    if (pItemInfo != NULL)
    {
      pItem = pItemInfo->pItem;
      if (pItem != NULL)
      {
        ref_cnt_obj_add_ref(pItem);
        ppItems[numItems++] = pItem;
      }
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);

  return numItems;
}


/*-------- ids_are_equal --------*/
STATIC boolean ids_are_equal(
  void  *pElem1,
  void  *pElem2
  )
{
  policyman_item_t  *pItem1 = *((policyman_item_t **) pElem1);
  policyman_item_t  *pItem2 = *((policyman_item_t **) pElem2);
  
  return    policyman_cfgitem_id(pItem1) == policyman_cfgitem_id(pItem2)
         && pItem1->asubs_id == pItem2->asubs_id
         ;
}


/*-------- policyman_itemset_new --------*/
policyman_set_t * policyman_itemset_new(
  void
  )
{
  return  policyman_set_refcnt_new(ARR_SIZE(policyman_cfg_info),
              ARR_SIZE(policyman_cfgitems),
              ids_are_equal );
}


/*-------- policyman_report_msg_free --------*/
void policyman_report_msg_free(
  msgr_hdr_s  *pMsg
  )
{
  if (pMsg != NULL)
  {
    policyman_cfgitem_notify_msg_dtor(pMsg);
  }
}

/*-------- policyman_cfgitem_ids_are_equal --------*/
STATIC boolean policyman_cfgitem_ids_are_equal(
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t         *pItem1 = *((policyman_item_t **) pData1);
  policyman_item_cmp_ctx_t *pItem2 = (policyman_item_cmp_ctx_t *) pData2;

  return  policyman_cfgitem_id(pItem1) == pItem2->id
          && pItem1->asubs_id == pItem2->asubs_id
         ;
}


/*-------- policyman_cfgitem_is_present_in_itemset --------*/
boolean policyman_cfgitem_is_present_in_itemset(
  policyman_set_t           *pItemSet,
  policyman_item_id_t        itemId,
  sys_modem_as_id_e_type     subsId,
  policyman_item_t         **ppReturnedItem
)
{
  policyman_item_t  **ppLocalItem = NULL;
  
  ppLocalItem = policyman_cfgitem_find_item_in_itemset( pItemSet,
                                                        itemId,
                                                        subsId );
  
  if ( (ppLocalItem != NULL) && (ppReturnedItem != NULL) )
  {
    *ppReturnedItem = *ppLocalItem;
  }
  
 return (ppLocalItem != NULL);
}


/*-------- policyman_cfgitem_find_item_in_itemset --------*/
void * policyman_cfgitem_find_item_in_itemset(
  policyman_set_t       *pItemSet,
  policyman_item_id_t    id,
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_item_cmp_ctx_t item;

  item.id = id;
  item.asubs_id = asubs_id;
    
  return policyman_set_find( pItemSet, policyman_cfgitem_ids_are_equal, (void *)&item );
}

/*-------- policyman_compute_changed_set --------*/
STATIC void policyman_compute_changed_set(
  policyman_set_t       *pItemSet,
  policyman_set_t       *pChangeSet
)
{
  mre_set_iterate(pItemSet, find_and_collect_changed_items, pChangeSet);
}

/*-------- policyman_cfgitem_get_change_msg_hdr --------
   checks if pItemSet has config items that differ from database
   if found any such items, generates a ref cnted attach object
   and returns hdr to the clients
*/
msgr_hdr_s *policyman_cfgitem_get_change_msg_hdr(
  policyman_set_t  *pItemSet
)
{  
  policyman_set_t    *pChangeSet = NULL;
  msgr_hdr_s         *pMsg       = NULL;
  policyman_item_t  **ppItems;

  pChangeSet = policyman_itemset_new();
  
  /* If itemset is not empty, generate the changed set */
  if (!policyman_set_is_empty(pItemSet))
  {
    policyman_compute_changed_set(pItemSet,pChangeSet);
  }

  if (!policyman_set_is_empty(pChangeSet)) 
  {
    /* Create the MSGR message from changed set */
   
    ppItems = (policyman_item_t **) policyman_set_get_buf_ptr(pChangeSet);
    pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, policyman_set_num_elems(pChangeSet), NULL);
  }
  
  REF_CNT_OBJ_RELEASE_IF(pChangeSet);
  return pMsg;
}

/*-------- policyman_cfgitem_persist_item --------*/
boolean policyman_cfgitem_persist_item(
  char const              *pPath,
  void                    *pData,
  size_t                   dataSize,
  sys_modem_as_id_e_type   asubs_id
)
{
  mre_efs_status_t status;
  /* the last argument is false as Policyman doesn't
   * want to persist the item if /policyman folder
   * is not already present 
   */
  status = mre_efs_put_file( pPath,
                             pData,
                             dataSize,
               /*O_AUTODIR*/ FALSE,
                             asubs_id );

  return (MRE_EFS_STATUS_SUCCESS == status);
}

/*-------- policyman_cfgitem_update_device_config_for_force_notification --------*/
void policyman_cfgitem_update_device_config_for_force_notification(
  policyman_set_t *pItemSet
)
{
  device_configuration_t   *pDc     = NULL;
  device_configuration_t   *pDbItem = NULL;

  /* If pNotifySet already has device config then return */
  pDc = policyman_get_device_config_from_itemset(pItemSet);

  if (NULL == pDc)
  {
    policyman_get_current_device_config((policyman_item_t **) &pDbItem);
    pDc = (device_configuration_t *)policyman_device_config_item_clone(pDbItem);
    policyman_set_append(pItemSet, &pDc);
  }

  /*  This marks the config in the itemset as "dirty" so it will be updated.
   */
  pDc->forceUpdate = TRUE;

  REF_CNT_OBJ_RELEASE_IF(pDbItem);
  REF_CNT_OBJ_RELEASE_IF(pDc);
}


/*-------- policyman_cfgitem_find_item_in_context --------*/
void *policyman_cfgitem_find_item_in_context(
  void                   *pCtx,
  policyman_item_id_t     id,
  sys_modem_as_id_e_type  asubs_id
)
{
  policy_execute_ctx_t  *pContext = (policy_execute_ctx_t *)pCtx;
  policyman_set_t       *pItemSet = pContext->pItemSet;
  policyman_item_t      *pItem    = NULL;
  boolean                item_is_present = FALSE;

  /* check if item with 'id' present in pItemSet */
  if ( NULL != pItemSet )
  {
    item_is_present  = policyman_cfgitem_is_present_in_itemset( pItemSet, id, asubs_id, &pItem);
  }

  /* if item with 'id' found in pItemSet, add ref
     as policyman_cfgitem_find_item_in_itemset doesn't do that*/
  if (item_is_present)
  {
    ref_cnt_obj_add_ref(pItem);
    POLICYMAN_MSG_HIGH_1("Found item %d in context itemset", id);
  }
  else  /* get from Policyman config DB */
  {
    policyman_cfgitem_get_items( &id,
                                 1,
                                (policyman_item_t const **)&pItem );
  }

  return pItem;
}


typedef struct
{
  size_t      seqno;
  mre_set_t  *pItemSet;
} seqno_item_ctx_t;


/*-------- add_to_itemset_if_sequence_number_greater --------*/
boolean add_to_itemset_if_sequence_number_greater(
  void  *pElem,
  void  *pCtx
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  seqno_item_ctx_t        *pSeqnoCtx  = (seqno_item_ctx_t *) pCtx;
  policyman_item_t        *pItem;

  pItem = pCfgItem->pItem;
  if (pSeqnoCtx->seqno < pItem->seqno)
  {
    mre_set_add(pSeqnoCtx->pItemSet, &pItem);
  }

  return TRUE;
}

/*-------- policyman_get_items_since_sequence_number --------*/
policyman_item_collection_t *policyman_get_items_since_sequence_number(
  size_t  seqno
)
{
  policyman_item_collection_t *pCollection = NULL;

  if (sequence_number_get() <= seqno)
  {
    POLICYMAN_MSG_HIGH_1("No items changed since sequence number %d", seqno);
  }
  else
  {
    seqno_item_ctx_t  ctx;
    mre_set_t         *pItemSet;
    size_t             numItems;
    policyman_item_t **ppItems;

    ctx.seqno    = seqno;
    ctx.pItemSet = policyman_itemset_new();

    policyman_cfgitems_iterate( add_to_itemset_if_sequence_number_greater, &ctx);
    
    numItems = mre_set_num_elems(ctx.pItemSet);
    ppItems = mre_set_get_buf_ptr(ctx.pItemSet);
    pCollection = policyman_item_collection_new(numItems, ppItems, TRUE);

    POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
    POLICYMAN_MSG_HIGH_2("Returning %d items changed since sequence number %d:", numItems, seqno);
    mre_set_iterate(ctx.pItemSet, policyman_cfgitem_display, NULL);

    ref_cnt_obj_release(ctx.pItemSet);
  }

  return pCollection;
}
