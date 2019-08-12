/**
  @file policyman_network_events.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_network_events.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_i.h"
#include "policyman_network_events.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_rat_capability.h"
#include "policyman_rf.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_xml.h"

#include "cm_dualsim.h"
#include "sys.h"
#include <stringl/stringl.h>


/*=============================================================================
 Data Structure for Network Info
==============================================================================*/

typedef struct
{
  rat_capability_info_t     ratMaskPref;
  rf_bands_t                bandPref;
} policyman_network_scan_pref_t;


struct policyman_network_info_s
{
  sys_modem_as_id_e_type             subsId;             // Subscription Id
  policyman_network_scan_e_type      ScanStatus;         // Scan Status
  policyman_network_scan_pref_t      scanPref;           // Preferred Rats and Bands for scan request
  sys_detailed_plmn_list_s_type      *available_networks; // Indicates the current searched PLMN list

  policyman_timer_t                  *pNetScanTimer;     // Timer object for retrying of Net Scan
  boolean                            is_search_allowed;  // Boolean to indicate if periodic search is allowed
};

/*=============================================================================
  Network Info callback definition.
=============================================================================*/
typedef struct
{
  POLICYMAN_CMD_HDR;

  void                 *pInfo;
  cm_ph_cmd_e_type     cmd;
  cm_ph_cmd_err_e_type err;
} policyman_network_scan_cmd_t;

typedef struct
{
  sys_modem_as_id_e_type asubs_id;
} policyman_network_scan_ctx_t;

typedef struct
{
  POLICYMAN_CMD_HDR;

  sys_modem_as_id_e_type   subsId;
  boolean                  is_search_allowed;
} policyman_periodic_search_t;


cm_mode_pref_e_type cmph_map_sd_mode_pref_to_cm_mode_pref(
  sd_ss_mode_pref_e_type  mode_pref
);


/*=============================================================================
  APIs for Network Scan Events
=============================================================================*/

/*-------- policyman_network_scan_request_search --------*/
STATIC boolean policyman_network_scan_request_search(
  policyman_state_t             *pState,
  sys_modem_as_id_e_type         subsId,
  policyman_network_scan_pref_t *scanPref,
  policyman_set_t               *pItemSet
)
{
  boolean                       retval  = FALSE;
  uint32                        ratMask;
  cm_mode_pref_e_type           cm_mode_pref;
  policyman_network_scan_ctx_t *pCtx;
  policy_execute_ctx_t  rcCtx;

  if (scanPref != NULL)
  {
    cm_band_pref_e_type   scanPrefLteBands;

    rcCtx.pState   = pState;
    rcCtx.pItemSet = pItemSet;
    rcCtx.asubs_id = subsId;

    ratMask      = policyman_rat_capability_evaluate(&scanPref->ratMaskPref, &rcCtx);
    cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(policyman_util_rat_mask_to_sd_mode_pref(ratMask));

    if (cm_mode_pref == CM_MODE_PREF_MAX)
    {
      POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "incorrect rat mask 0x%x, no associated CM mode_pref - no scan issued",
                             subsId,
                             ratMask );
      goto Done;
    }

    pCtx = (policyman_network_scan_ctx_t *) policyman_mem_alloc(sizeof(policyman_network_scan_ctx_t));
    pCtx->asubs_id = subsId;

    // Only the lower 64 bits are used for scans
    scanPrefLteBands = sys_map_lte_band_mask_to_band_mask(scanPref->bandPref.bands.lteBands);

    retval = cm_ph_cmd_get_networks_per_subs_extn( policyman_network_scan_cmd_cb,
                                                   (void *) pCtx,
                                                   policyman_event_get_cm_client_id(),
                                                   CM_NETWORK_LIST_PRIORITY_PLMN_SEARCH_BGND,
                                                   cm_mode_pref,
                                                   subsId,
                                                   scanPref->bandPref.bands.gwBands,
                                                   scanPrefLteBands,
                                                   scanPref->bandPref.bands.tdsBands );

    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "started <network_scan> with mode_pref %d and RF bands, cmd status %d",
                          subsId,
                          cm_mode_pref,
                          retval );

    policyman_rf_print_bands( subsId,
                              scanPref->bandPref.bands.gwBands,
                              scanPref->bandPref.bands.lteBands,
                              scanPref->bandPref.bands.tdsBands );
  }

Done:
  return retval;
}


/*-------- policyman_network_scan_stop_search --------*/
static boolean policyman_network_scan_stop_search(
  void
)
{
  boolean retval = cm_ph_cmd_terminate_get_networks( NULL,
                                                     NULL,
                                                     policyman_event_get_cm_client_id() );

  POLICYMAN_MSG_HIGH_1("stopping <network_scan>, cmd status %d", retval);
  return retval;
}

/*-------- policyman_network_scan_timer_expired --------*/
static void policyman_network_scan_timer_expired(
  policyman_timer_t *pTimer
)
{
  policyman_state_t        *pState;
  sys_modem_as_id_e_type    subsId;
  policyman_network_info_t *pNetworkInfo;

  pState = policyman_state_get_state();
  subsId = policyman_timer_get_subs(pTimer);
  pNetworkInfo = policyman_state_get_network_info(pState, subsId);

  /* reissue the network scan if operation is pending and increase timer duration
   */
  if (policyman_network_scan_request_search(pState, subsId, &pNetworkInfo->scanPref, NULL))
  {
    /* Increase new duration by 5 secs
      */
    policyman_timer_modify_interval(pTimer, 5);
  }
}

/*-------- policyman_network_scan_timer_init --------*/
static void policyman_network_scan_timer_init(
  policyman_network_info_t *pNetworkInfo,
  sys_modem_as_id_e_type    subsId
)
{
  policyman_timer_t      *pNetScanTimer = NULL;

  if (pNetworkInfo)
  {
    /*---------------------------------------------------------------------------
      Find and initialize the Net Scan timer.
     ---------------------------------------------------------------------------*/
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "Network Scan timer created with fixed interval = 30 sec", subsId);

    pNetScanTimer = policyman_timer_create_fixed_timer( "netscan",
                                                        subsId,
                                                        NULL,  // No policy for netscan timer
                                                        TIMER_NET_SCAN,
                                                        30,              // fixed duration 30 sec
                                                        policyman_network_scan_timer_expired );

    pNetworkInfo->pNetScanTimer = pNetScanTimer;
  }
}

/*-------- policyman_network_scan_timer_deinit --------*/
void policyman_network_scan_timer_deinit(
  policyman_network_info_t *pNetworkInfo
)
{
  /*  Release the timer object
   */
  REF_CNT_OBJ_RELEASE_IF(pNetworkInfo->pNetScanTimer);
}

/*-------- policyman_network_init --------*/
boolean policyman_network_init(
  policyman_network_info_t **ppNetworkInfo,
  sys_modem_as_id_e_type     subsId
)
{
  policyman_network_info_t *pInfo;

  pInfo = (policyman_network_info_t *) policyman_mem_alloc(sizeof(policyman_network_info_t));

   pInfo->subsId = subsId;
   pInfo->is_search_allowed = TRUE;

  *ppNetworkInfo = pInfo;

  return (*ppNetworkInfo != NULL);
}

/*-------- policyman_network_deinit --------*/
void policyman_network_deinit(
  policyman_network_info_t *pInfo
)
{
  policyman_network_scan_timer_deinit(pInfo);

  /* Don't free it here, call <network_scan action="clear">
       after scan results are tested to clear the list
   */
  //POLICYMAN_MEM_FREE_IF(pInfo->available_networks);
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_network_info_deep_copy --------*/
policyman_network_info_t * policyman_network_info_deep_copy(
  policyman_network_info_t  *pNetworkState
)
{
  policyman_network_info_t *pInfo;

  pInfo = (policyman_network_info_t *) policyman_mem_alloc(sizeof(policyman_network_info_t));

  *pInfo = *pNetworkState;

  return pInfo;
}

/*-------- policyman_network_scan_get_scan_status --------*/
policyman_network_scan_e_type policyman_network_scan_get_scan_status(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_network_info_t *pNetworkInfo;

  pNetworkInfo = policyman_state_get_network_info(pState, subsId);

  return pNetworkInfo->ScanStatus;
}

/*-------- policyman_network_scan_set_scan_status --------*/
void policyman_network_scan_set_scan_status(
  policyman_network_info_t      *pNetworkInfo,
  policyman_network_scan_e_type  status
)
{
  pNetworkInfo->ScanStatus = status;
}

/*-------- policyman_execute_periodic_plmn_search_allowed --------*/
static void policyman_execute_periodic_plmn_search_allowed(
  policyman_cmd_t *pCmd
)
{
  policyman_periodic_search_t *pSearchCmd   = (policyman_periodic_search_t *) pCmd;
  policyman_network_info_t    *pNetworkInfo = NULL;

  if (NULL != pSearchCmd)
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "process periodic_plmn_search; is_search_allowed %d",
                          pSearchCmd->subsId,
                          pSearchCmd->is_search_allowed );

    policyman_state_enter_crit_section();
    pNetworkInfo = policyman_state_get_network_info(pSearchCmd->pState, pSearchCmd->subsId);
    pNetworkInfo->is_search_allowed = pSearchCmd->is_search_allowed;
    policyman_state_leave_crit_section();
    policyman_state_handle_update(pSearchCmd->pState);
  }
}

/*-------- policyman_set_periodic_plmn_search_allowed--------*/
void policyman_set_periodic_plmn_search_allowed(
   sys_modem_as_id_e_type  subsId,
   boolean                 isAllowed
)
{
  policyman_periodic_search_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "received set_periodic_plmn_search_allowed; isAllowed %d",
                        subsId,
                        isAllowed );

  pCmd = POLICYMAN_CMD_NEW( policyman_periodic_search_t,
                            policyman_execute_periodic_plmn_search_allowed,
                            NULL,
                            NULL ); // No policy associated with Network Scan Event

  pCmd->is_search_allowed = isAllowed;
  pCmd->subsId = subsId;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}


/*-------- policyman_network_scan_handle_error --------*/
static void policyman_network_scan_handle_error(
  policyman_state_t      *pState,
  cm_ph_cmd_err_e_type   err,
  sys_modem_as_id_e_type subsId
)
{
  policyman_network_info_t *pNetworkInfo;

  pNetworkInfo = policyman_state_get_network_info(pState, subsId);

  /* Intialize a timer for handling Net Scan errors. Initializing here guarantees
         that we don't allocate memory for time object when network scan is not
         being done. Another advantage is we'll initialize timer only for mmode SUBS
         so if mmode SUBS changes, timer for new one will automatically be created
   */
  if (pNetworkInfo->pNetScanTimer == NULL)
  {
    policyman_network_scan_timer_init(pNetworkInfo, subsId);
  }

  /*
     If no error reported, then wait for CM event(AVAILABLE_NETWROKS_CONF)
     that shall have a list of available networks
  */
  if (err == CM_PH_CMD_ERR_NOERR)
  {
    POLICYMAN_MSG_HIGH_1( SUBS_PREFIX "waiting for cm ph event AVAILABLE_NETWROKS_CONF",
                          subsId );
  }
  else
  {
    /* Net Scan cmd itself is rejected, try giving scan after some time
      */
    policyman_network_scan_set_scan_status(pNetworkInfo, POLICYMAN_NET_SCAN_ABORTED);
    policyman_timer_start(pNetworkInfo->pNetScanTimer);
    POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "Network Search cmd rejected with error %d, retry after timer expiry duration %d",
                          subsId,
                          err,
                          policyman_timer_get_interval(pNetworkInfo->pNetScanTimer) );
  }
}

/*-------- policyman_execute_network_scan_cmd --------*/
void policyman_execute_network_scan_cmd(
  policyman_cmd_t *pCmd
)
{
  policyman_network_scan_cmd_t *pScanCmd = (policyman_network_scan_cmd_t*) pCmd;

  if (NULL != pScanCmd)
  {
    policyman_network_scan_ctx_t *pCtx = NULL;
    sys_modem_as_id_e_type        subsId = SYS_MODEM_AS_ID_1;

    pCtx = (policyman_network_scan_ctx_t *)pScanCmd->pInfo;

    if (NULL != pCtx)
    {
      subsId = (pCtx->asubs_id);
      policyman_mem_free(pCtx);
    }

    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "process network scan cmd %d",
                          subsId,
                          pScanCmd->cmd );

    switch (pScanCmd->cmd)
    {
      case CM_PH_CMD_GET_NETWORKS:
        policyman_state_enter_crit_section();
        policyman_network_scan_handle_error(pScanCmd->pState, pScanCmd->err, subsId);
        policyman_state_leave_crit_section();
       break;

      default:
        break;
    }
  }
}

/*-------- policyman_network_scan_cmd_cb --------*/
void policyman_network_scan_cmd_cb(
  void                 *pInfo,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  err
)
{
  policyman_network_scan_cmd_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received cm ph cmd %d", cmd);

  pCmd = POLICYMAN_CMD_NEW( policyman_network_scan_cmd_t,
                            policyman_execute_network_scan_cmd,
                            NULL,
                            NULL ); // No policy associated with Network Scan Event

  pCmd->cmd   = cmd;
  pCmd->err   = err;
  pCmd->pInfo = pInfo;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}

 /*-------- policyman_network_scan_clear_results --------*/
static void policyman_network_scan_clear_results(
  policyman_network_info_t  *pNetworkInfo
)
{
  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "cleared <network_scan> results in state", pNetworkInfo->subsId);
  POLICYMAN_MEM_FREE_IF(pNetworkInfo->available_networks);
}

/*-------- policyman_network_scan_update_list --------*/
static boolean policyman_network_scan_update_list(
  policyman_network_info_t      *pNetInfo,
  sys_modem_as_id_e_type         subsId,
  sys_detailed_plmn_list_s_type *pPlmnList
)
{
  size_t    i;
  boolean                            changed       = FALSE;
  sys_detailed_plmn_list_info_s_type *pDetailedPlmnList;

  policyman_network_scan_set_scan_status(pNetInfo, POLICYMAN_NET_SCAN_COMPLETED);

  if (pPlmnList != NULL && pPlmnList->length > 0)
  {
    /* clear any existing list
      */
    if (pNetInfo->available_networks != NULL)
    {
      policyman_network_scan_clear_results(pNetInfo);
    }

    /* Update the information in state
      */
    pNetInfo->subsId     = subsId;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "updated %d PLMNs for network scan result in state",
                          subsId,
                          pPlmnList->length );

    /* Allocate memory for new list, copy the returned detailed PLMNs/RAT info to state
      */
    pNetInfo->available_networks = (sys_detailed_plmn_list_s_type *) policyman_mem_alloc(sizeof(sys_detailed_plmn_list_s_type));
    pNetInfo->available_networks->length = pPlmnList->length;

    for (i = 0; i < pPlmnList->length; ++i)
    {
      pDetailedPlmnList = &pNetInfo->available_networks->info[i];
      memscpy( pDetailedPlmnList,
               sizeof(sys_detailed_plmn_list_info_s_type),
               &pPlmnList->info[i],
               sizeof(sys_detailed_plmn_list_info_s_type));

      POLICYMAN_MSG_PLMN_0("", pDetailedPlmnList->plmn);
    }

    changed = TRUE;
  }

  return changed;
}

/*-------- policyman_network_scan_delete_list --------*/
static boolean policyman_network_scan_delete_list(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_detailed_plmn_list_s_type *pPlmnList
)
{
    /* clear any existing list
      */
  policyman_network_scan_clear_results(pInfo);

    /* Update the information in state
      */
  pInfo->subsId = subsId;
  policyman_network_scan_set_scan_status(pInfo, POLICYMAN_NET_SCAN_NOT_STARTED);

  return TRUE;
}

/*-------- policyman_network_scan_update_result --------*/
boolean policyman_network_scan_update_result(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_plmn_list_status_e_type    listStatus,
  sys_detailed_plmn_list_s_type *pPlmnList
)
{
  boolean  changed = FALSE;

  /* First check if we requested for network scan on this subs
    */
  if (pInfo->ScanStatus != POLICYMAN_NET_SCAN_PENDING)
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "not updating scan result in PM state, current scan status is %d",
                          subsId,
                          pInfo->ScanStatus );
    return changed;
  }

  switch (listStatus)
  {
    case SYS_PLMN_LIST_SUCCESS:
      changed = policyman_network_scan_update_list(pInfo, subsId, pPlmnList);
      break;

    case SYS_PLMN_LIST_AS_ABORT:
      changed = policyman_network_scan_delete_list(pInfo, subsId, pPlmnList);
      break;

    default:
      break;
  }

  return changed;
}

/*-------- policyman_net_scan_update_state_rf_info --------*/
void policyman_net_scan_update_state_rf_info(
  policyman_state_t             *pState,
  policyman_network_scan_pref_t *pScanPref
)
{
  policyman_network_info_t      *pNetworkInfo   = NULL;
  policyman_network_scan_pref_t *pStateScanPref = NULL;
  size_t  subsId;

  /* Update for all SUBS - updating only for mmode subs is not correct
       since mmode SUBS might change during run time and we'll attempt
       to issue scan with no RATs/BANDs in state for that SUBS.
   */
  if (NULL != pScanPref)
  {
    for (subsId = 0; subsId < POLICYMAN_NUM_SUBS; subsId++)
    {
      pNetworkInfo = policyman_state_get_network_info(pState, subsId);
      pStateScanPref = &pNetworkInfo->scanPref;

      memscpy( pStateScanPref,
               sizeof(policyman_network_scan_pref_t),
               pScanPref,
               sizeof(policyman_network_scan_pref_t) );
    }
  }
}


/*-------- policyman_net_scan_rf_bands_read_info --------*/
policyman_status_t policyman_net_scan_rf_bands_read_info(
  policyman_xml_element_t const   *pElem,
  rf_bands_t                      *pBands,
  policyman_policy_t              *pPolicy
)
{
  char const         *pListName;
  rf_bands_t         *rfBandList;
  policyman_status_t status = POLICYMAN_STATUS_ERR_MALFORMED_XML;

  /*  If this action uses a named bands list, look up for named list
        and get bands from it.
   */
  pListName = policyman_xml_get_attribute(pElem, "list");
  if (pListName != NULL)
  {
    rfBandList = (rf_bands_t *) mre_named_object_find( pListName,
                                                POLICYMAN_NAMED_RF_BANDS,
                                                       (mre_policy_t *) pPolicy );
    if (rfBandList != NULL)
    {
      policyman_get_bands_for_named_object(rfBandList, pBands);
      status = POLICYMAN_STATUS_SUCCESS;
      goto Done;
    }
  }

  /* Otherwise, read the inline band list.
   */
  if (!policyman_rf_band_list_read(pElem, pBands))
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "invalid <rf_bands> specification", POLICY_SUBS(pPolicy));
    goto Done;
  }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_net_scan_rf_bands_read_info returned status %d",
                        POLICY_SUBS(pPolicy),
                        status );
  return status;
}


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*=============================================================================
  Network-Search related conditions and actions
=============================================================================*/

/*-----------------------------------------------------------------------------
  Scan Results
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  char    *pTestList;
} policyman_scan_results_condition_t;


/*-------- policyman_condition_scan_result_dtor --------*/
static void policyman_condition_scan_result_dtor(
  void  *pObj
)
{
  policyman_scan_results_condition_t *pCondition = (policyman_scan_results_condition_t *)pObj;

  POLICYMAN_MEM_FREE_IF(pCondition->pTestList);
  policyman_condition_dtor(pCondition);
}


/*-------- evaluate_scan_results --------*/
static boolean evaluate_scan_results(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_set_t                     *pTestPlmnSet    = NULL;
  policyman_scan_results_condition_t  *pCond           = (policyman_scan_results_condition_t *)pCondition;
  policy_execute_ctx_t                *pnsCtx          = (policy_execute_ctx_t *)pCtx;
  policyman_network_info_t const      *pNetworkInfo;
  boolean                             result           = FALSE;
  sys_plmn_id_s_type                  *testPlmn;
  size_t                               i;
  size_t                               j;
  size_t                               nElems;
  sys_mcc_type                         mcc;
  sys_mcc_type                         mcc1;
  sys_mnc_type                         mnc;
  sys_mnc_type                         mnc1;
  boolean                              plmn_is_undefined;
  boolean                              plmn_is_undefined1;
  boolean                              mnc_includes_pcs_digit;
  boolean                              mnc_includes_pcs_digit1;

  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "evaluating <scan_results pmsubs=%d>",
                        CONDITION_SUBS(pCondition),
                        subsId );

  pNetworkInfo = policyman_state_get_network_info(pnsCtx->pState, subsId);

  /*  Find the Test PLMN set to use; if we don't have one return FALSE.
   */
  if (pCond->pTestList != NULL)
  {
    pTestPlmnSet = (policyman_set_t *) mre_named_object_find( pCond->pTestList,
                            POLICYMAN_NAMED_PLMN_SET,
                                                              pCond->pPolicy );
  }
  if (pTestPlmnSet == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "Test PLMN set not found", subsId);
    goto Done;
  }

  if (pNetworkInfo->ScanStatus != POLICYMAN_NET_SCAN_COMPLETED)
  {
    POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "scan results not available, current scan status %d",
                           subsId,
                           pNetworkInfo->ScanStatus );
    goto Done;
  }

  /* Run through each PLMN in test PLMN set and see if that is present in scanned result
   */
  nElems = policyman_set_num_elems(pTestPlmnSet);
  POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "evaluating <scan_results>:  %d PLMNS in PLMN set",
                        subsId,
                        nElems );

  if (pNetworkInfo->available_networks != NULL)
  {
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "evaluating <scan_results>: available_networks %d",
                          subsId,
                          pNetworkInfo->available_networks->length );

    for (i = 0; i < nElems; i++)
    {
      /* Get PLMN out of the set and break if present in scanned list
          */
      testPlmn = (sys_plmn_id_s_type *)policyman_set_elem_ptr(pTestPlmnSet, i);
      POLICYMAN_MSG_PLMN_2(SUBS_PREFIX "PLMN[%d] in set: ", subsId, i, *testPlmn);
      sys_plmn_get_mcc_mnc(*testPlmn, &plmn_is_undefined, &mnc_includes_pcs_digit, &mcc, &mnc);

      j=0;
      while (j < pNetworkInfo->available_networks->length)
      {
        POLICYMAN_MSG_HIGH_2( SUBS_PREFIX ">>>> searched PLMN[%d] <<<<",
                              subsId,
                              j );
        POLICYMAN_MSG_PLMN_0( "", pNetworkInfo->available_networks->info[j].plmn);

        sys_plmn_get_mcc_mnc( pNetworkInfo->available_networks->info[j].plmn,
                              &plmn_is_undefined1,
                              &mnc_includes_pcs_digit1,
                              &mcc1,
                              &mnc1 );

        result =  mcc == mcc1 && mnc == mnc1;
        j++;

        if (result==TRUE)
        {
          POLICYMAN_MSG_MED_1(SUBS_PREFIX "PLMN match found", subsId);
          break;
        }
      }
      if (result==TRUE)
      {
        break;
      }
    }
  }

Done:
  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <scan_result pmsubs=%d> returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );
  return result;
}


/*-------- policyman_condition_network_event_evaluate --------*/
static boolean policyman_condition_scan_result_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
)
{
  policyman_condition_t const *pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_scan_results);
}


/*-------- policyman_condition_scan_results_new --------*/
mre_status_t policyman_condition_scan_results_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t                        status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_scan_results_condition_t  *pCondition;
  char const                          *pTestList;
  char const                          *pStr;
  policyman_subs_t                     pm_subs    = SUBS_THIS;

  /* Get a named list of Test PLMNs
   */
  pStr = policyman_xml_get_attribute(pElem, "test");

  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<scan_results> needs a 'test' attribute", POLICY_SUBS(pPolicy));
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from the policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_scan_results_condition_t,
                                        pPolicy,
                                        policyman_condition_scan_result_evaluate,
                                        policyman_condition_scan_result_dtor,
                                        pm_subs,
                                        pElem );

  /* Check if list of PLMNs needs to be checked
   */
  if (strcmp(pStr, "plmns")==0)
  {
    pTestList = policyman_xml_get_attribute(pElem, "list");
    if (pTestList == NULL)
    {
      POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<scan_results> needs a 'list' attribute", POLICY_SUBS(pPolicy));
      goto Done;
    }
    pCondition->pTestList = policyman_str_dup(pTestList);
  }
  else if (strcmp(pStr, "better")==0)
  {
    POLICYMAN_MSG_ERROR_1(SUBS_PREFIX "<scan_results> 'better' not handled in attribute 'test'", POLICY_SUBS(pPolicy));
    goto Done;
  }

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "condition <scan_results pmsubs=%d> returned status %d",
                        POLICY_SUBS(pPolicy),
                        pm_subs,
                        status );
  return status;
}


/*-----------------------------------------------------------------------------
  Scan active Condition
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_network_scan_e_type  ScanStatus;
} policyman_net_scan_active_condition_t;


/*-------- evaluate_network_scan_active --------*/
static boolean evaluate_network_scan_active(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_net_scan_active_condition_t const *pCond  = (policyman_net_scan_active_condition_t *) pCondition;
  boolean                                      result       = FALSE;

  result = pCond->ScanStatus == POLICYMAN_NET_SCAN_PENDING;
  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "condition <network_scan_active pmsubs=%d> returned %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        subsId,
                        result );
  return result;
}


/*-------- policyman_condition_network_scan_active_evaluate --------*/
static boolean policyman_condition_network_scan_active_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
)
{
  policyman_condition_t const *pCond = (policyman_condition_t const *)pCondition;

  return policyman_evaluate_cond(pCond, pCtx, evaluate_network_scan_active);
}

/*-------- policyman_condition_network_scan_active_new --------*/
mre_status_t policyman_condition_network_scan_active_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t                          status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_net_scan_active_condition_t *pCondition = NULL;
  policyman_subs_t                       pm_subs    = SUBS_THIS;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = POLICYMAN_CONDITION_NEW( policyman_net_scan_active_condition_t,
                                        pPolicy,
                                        policyman_condition_network_scan_active_evaluate,
                                        policyman_condition_dtor,
                                        pm_subs,
                                        pElem );


  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "condition_new <network_scan_active pmsubs=%d> returned status %d",
                        POLICY_SUBS(pPolicy),
                        pm_subs,
                        status );
  return status;
}

/*-----------------------------------------------------------------------------
  Network Scan Request
-----------------------------------------------------------------------------*/
typedef enum
{
  POLICYMAN_NET_SCAN_ACTION_NONE  = 0,
  POLICYMAN_NET_SCAN_ACTION_START = 1,
  POLICYMAN_NET_SCAN_ACTION_STOP  = 2,
  POLICYMAN_NET_SCAN_ACTION_ABORT = 3,
  POLICYMAN_NET_SCAN_ACTION_CLEAR = 4
} policyman_scan_action_e;

typedef struct
{
  POLICYMAN_ACTION_BASE;

  policyman_scan_action_e       action;
  policyman_network_scan_pref_t scan_pref;
} network_scan_action_t;

/*-------- policyman_network_scan_read_info --------*/
static policyman_status_t policyman_network_scan_read_info(
  policyman_xml_element_t const   *pElem,
  network_scan_action_t           *pAction,
  policyman_policy_t              *pPolicy
)
{
  policyman_status_t            status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  policyman_network_scan_pref_t *pInfo = &pAction->scan_pref;
  char const        *pActionStr;
  size_t            nChildren;
  size_t            i;

  memset(pInfo, 0x00, sizeof(policyman_network_scan_pref_t));

  pActionStr = policyman_xml_get_attribute(pElem, "action");
  if (pActionStr == NULL)
  {
    goto Done;
  }

  /* Start network scan
    */
  if (strcmp(pActionStr, "start")==0)
  {
    nChildren = policyman_xml_num_children(pElem);
    if (nChildren == 0)
    {
      status = POLICYMAN_STATUS_SUCCESS;
      goto Done;
    }

    for (i = 0 ; i < nChildren ; ++i)
    {
      policyman_xml_element_t const *pChild;

      pChild = policyman_xml_get_child(pElem, i);
      if (pChild == NULL)
      {
        POLICYMAN_MSG_ERROR_2( SUBS_PREFIX "Internal error - no element #%d",
                               POLICY_SUBS(pPolicy),
                               i );
        goto Done;
      }

      if      (policyman_xml_tag_is(pChild, "rats"))
      {
        status = policyman_rat_capability_read_info(pChild, &pInfo->ratMaskPref);
      }
      else if (policyman_xml_tag_is(pChild, "bands"))
      {
        status = policyman_net_scan_rf_bands_read_info( pChild,
                                                        (rf_bands_t *)&pInfo->bandPref,
                                                        pPolicy );
      }
      else
      {
        goto Done;
      }
    }

    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "setting network scan action to START", POLICY_SUBS(pPolicy));
    pAction->action = POLICYMAN_NET_SCAN_ACTION_START;
   }
   /* Stop network scan
     */
  else if (strcmp(pActionStr, "stop")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_STOP;
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "setting network scan action to STOP", POLICY_SUBS(pPolicy));
   }
   /* Abort network scan
     */
  else if (strcmp(pActionStr, "abort")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_ABORT;
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "setting network scan action to ABORT", POLICY_SUBS(pPolicy));
   }
   /* Clear the network scan state
     */
  else if (strcmp(pActionStr, "clear")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_CLEAR;
    POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "setting network scan action to CLEAR", POLICY_SUBS(pPolicy));
   }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- execute_network_scan --------*/
static boolean execute_network_scan(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  network_scan_action_t    *pnsAction = (network_scan_action_t *)pAction;
  policy_execute_ctx_t     *pnsCtx = (policy_execute_ctx_t *)pCtx;
  policyman_network_info_t *pNetworkInfo;
  boolean                   retval = TRUE;

  pNetworkInfo = policyman_state_get_network_info(pnsCtx->pState, subsId);

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "<network_scan> state: is_allowed %d, scan_status %d",
                        subsId,
                        LINE_NUM(pnsAction),
                        pNetworkInfo->is_search_allowed,
                        pNetworkInfo->ScanStatus );

  /* Clear the pending scan list. This case will hit when back to back
        NET SCAN is triggered when previous list didn't had the desired PLMN.
   */
  if (pNetworkInfo->available_networks != NULL)
   {
    policyman_network_scan_set_scan_status(pNetworkInfo, POLICYMAN_NET_SCAN_NOT_STARTED);
     policyman_network_scan_clear_results(pNetworkInfo);
   }

  /* Start Net Scan if it is allowed
    */
  if (   pnsAction->action == POLICYMAN_NET_SCAN_ACTION_START
     && pNetworkInfo->is_search_allowed == TRUE
      && policyman_network_scan_request_search(pnsCtx->pState, subsId, &pNetworkInfo->scanPref, pnsCtx->pItemSet)
    )
  {
    policyman_network_scan_set_scan_status(pNetworkInfo, POLICYMAN_NET_SCAN_PENDING);
  }
  /* Stop/Abort any ongoing Net Scan
    */
  else if (  (pnsAction->action == POLICYMAN_NET_SCAN_ACTION_STOP
          || pnsAction->action == POLICYMAN_NET_SCAN_ACTION_ABORT)
          && pNetworkInfo->ScanStatus == POLICYMAN_NET_SCAN_PENDING
          && policyman_network_scan_stop_search()
         )
  {
    policyman_network_scan_set_scan_status(pNetworkInfo, POLICYMAN_NET_SCAN_ABORTED);
  }
  /* Error case
   */
  else
  {
    retval = FALSE;
  }

  return retval;
}


/*-------- policyman_network_scan_execute --------*/
static boolean policyman_network_scan_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_network_scan);
}

/*-------- policyman_network_scan_action_new --------*/
mre_status_t policyman_network_scan_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t            status = MRE_STATUS_ERR_INVALID_ACTION;
  network_scan_action_t  *pAction = NULL;
  policyman_subs_t        pm_subs = POLICY_SUBS(pPolicy);

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( network_scan_action_t,
                                  pPolicy,
                                  policyman_network_scan_execute,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  status = policyman_network_scan_read_info(pElem, pAction, (policyman_policy_t *) pPolicy);

  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG_1(SUBS_PREFIX "<network_scan> invalid XML", POLICY_SUBS(pPolicy));
    REF_CNT_OBJ_RELEASE_IF(pAction);
    goto Done;
  }

  *ppAction = (mre_action_t *) pAction;

  /* Update the state with specified RATs/BANDs for all SUBS when action is START
        For ABORT/STOP/CLEAR no need to update the database for scan preference as
        it overwrites the previous preferences
   */
  if (pAction->action == POLICYMAN_NET_SCAN_ACTION_START)
  {
    policyman_state_t *pState;

    pState = policyman_state_get_state();

    policyman_net_scan_update_state_rf_info(pState, &pAction->scan_pref);
  }

Done:
  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "action_new <network_scan pmsubs=%d> returned status %d",
                        POLICY_SUBS(pPolicy),
                        pm_subs,
                        status );
  return status;
}
