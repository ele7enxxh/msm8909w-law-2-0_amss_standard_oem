/**
  @file policyman.c

  @brief Public interface to Policy Manager
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "comdef.h"
#include "mre_efs.h"
#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_fullrat_config.h"
#include "policyman_i.h"
#include "policyman_phone_events.h"
#include "policyman_plmn.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_serving_system.h"
#include "policyman_rf.h"
#include "policyman_timer.h"
#include "policyman_task.h"

/*------------------------------------------------------------------------
  FUNCTION Prototypes
--------------------------------------------------------------------------*/
void policyman_register_for_cm_events(void);


/*  Boolean used by policyman_is_test_mode() to know if it has determined
 *  whether policyman is in test mode.  This is a static outside the function
 *  because it must be reset each time policyman starts in unit tests.
 */
static boolean  test_mode_read;
static boolean  in_test_mode;
static boolean  in_FTM;


/*-------- ftm_mode_enabled --------*/
static boolean ftm_mode_enabled(
  void
)
{
  nv_stat_enum_type result;
  nv_item_type      data;

  result = mre_efs_get_nv_item(NV_FTM_MODE_I, &data, 0);

  return  ((result == NV_DONE_S) && (data.ftm_mode != 0));
}


/*-------- policyman_is_test_mode --------*/
/**
@brief  Is policyman supposed to run in test-only mode?
*/
boolean policyman_is_test_mode(
  void
)
{
  if (!test_mode_read)
  {
    test_mode_read = TRUE;
    in_test_mode   = mre_efs_file_exists( POLICYMAN_TEST_MODE_EFS_PATH,
                                          SYS_MODEM_AS_ID_NONE );

    /* Read FTM NV here.
         This step will happen at power up so we are fine reading FTM NV
     */
    in_FTM         = ftm_mode_enabled();
  }

  return in_test_mode;
}

/*-------- policyman_is_FTM --------*/
/**
@brief  Is policyman in FTM
*/
boolean policyman_is_FTM(
  void
)
{
  return in_FTM;
}


/*-------- policyman_set_test_mode --------*/
/**
@brief  Set test mode to given value
*/
void policyman_set_test_mode(
  boolean value
)
{
  in_test_mode = value;
  test_mode_read = value;
}

/*-------- policyman_set_FTM --------*/
/**
@brief  Set static in_FTM to given value
*/
void policyman_set_FTM(
  boolean value
)
{
  in_FTM = value;
}


/*-------- policyman_init --------*/
/**
@brief  Initialize Policy Manager.  Private function to be used by PM task code.

@return
  None
*/
void policyman_init(
  void
)
{
  policyman_state_t *pState;
  policyman_set_t       *pItemSet;
  size_t                 subsIndex;
  
  pItemSet     = policyman_itemset_new();

  policyman_state_init();
  policyman_rf_init();
  policyman_state_static_init();

  pState = policyman_state_get_state();

  policyman_cfgitem_init(pItemSet);

  // Device level initialization
  policyman_policies_init(pState, pItemSet);
  policyman_policy_load_global(pState, pItemSet);

  // Subs level initializaton
  for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; subsIndex++)
  {
    policyman_fullrat_config_init_per_subs(subsIndex);
    policyman_policies_per_subs_init(pState, pItemSet, subsIndex);
  }

  policyman_ensure_items_in_itemset(pItemSet);

  policyman_policy_init_is_complete();
  policyman_efs_initialize();

  (void)policyman_cfgitem_update_items(pItemSet, NULL);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);

  POLICYMAN_MSG_HIGH_0("---------- Initial database after initialization");
  policyman_cfgitem_display_all();

  policyman_send_all_config_items();
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_per_subs_deinit --------*/
/**
@brief  Deinitialize specific subscription state, as well as overall configuration for
           Policy Manager.  Private function to be used by PM task code.

@return
  None
*/
void policyman_per_subs_deinit(
  size_t subs
)
{
  policyman_fullrat_config_deinit_per_subs(subs);
  if (!policyman_is_test_mode())
  {
    policyman_device_config_reset_subs_feature((sys_modem_as_id_e_type)subs);
    policyman_policies_per_subs_deinit(subs);
  }

}


/*-------- policyman_get_items --------*/
policyman_status_t policyman_get_items(
  policyman_item_id_t const *pIds,
  size_t                    nIds,
  policyman_item_t const    **ppItem
)
{
  size_t  numItems;

  if (policyman_is_initializing())
  {
    return POLICYMAN_STATUS_ERR_NOT_READY;
  }

  POLICYMAN_CL_MSG_LOW_2( "Client thread 0x%x requested %d items from policyman_get_items()",
                           POLICYMAN_CALLER_THREAD_ID,
                           nIds );

  numItems = policyman_cfgitem_get_items(pIds, nIds, ppItem);

  if (policyman_is_suspended())
  {
    return POLICYMAN_STATUS_WRN_SUSPENDED;
  }
  else
  {
    return (numItems == nIds)? POLICYMAN_STATUS_SUCCESS : POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }
}


/*-------- policyman_get_items_msim --------*/
policyman_status_t policyman_get_items_msim(
  policyman_item_id_t const         *pIds,
  size_t                            nIds,
  policyman_item_collection_t const **ppItems
)
{
  policyman_status_t  status;

  if (policyman_is_initializing())
  {
    return POLICYMAN_STATUS_ERR_NOT_READY;
  }

  POLICYMAN_CL_MSG_LOW_2( "Client thread 0x%x requested %d items from policyman_get_items_msim()",
                          POLICYMAN_CALLER_THREAD_ID,
                           nIds );

  status = policyman_cfgitem_get_items_msim(pIds, nIds, (policyman_item_collection_t **) ppItems);

  return policyman_is_suspended() ? POLICYMAN_STATUS_WRN_SUSPENDED : status;
}


/*-------- policyman_get_items_block --------*/
policyman_status_t policyman_get_items_block(
  policyman_item_id_t const *pIds,
  size_t                    nIds,
  policyman_item_t const    **ppItem
)
{
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH_1( "Client thread 0x%x blocked in policyman_get_items_block()",
                          POLICYMAN_CALLER_THREAD_ID );

    policyman_blocked_clients_add();
  }

  return policyman_get_items(pIds, nIds, ppItem);
}


/*-------- policyman_get_items_block_msim --------*/
policyman_status_t policyman_get_items_block_msim(
  policyman_item_id_t const         *pIds,
  size_t                            nIds,
  policyman_item_collection_t const **ppItems
)
{
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH_1( "Client thread 0x%x blocked in policyman_get_items_block_msim()",
                          POLICYMAN_CALLER_THREAD_ID );

    policyman_blocked_clients_add();
  }

  return policyman_get_items_msim(pIds, nIds, ppItems);
}

/*-------- policyman_get_collection_item --------*/
policyman_item_t *policyman_get_collection_item(
  policyman_item_collection_t const *pCollection,
  policyman_item_id_t                id,
  sys_modem_as_id_e_type             subsId
)
{
  policyman_item_t const *pItem = NULL;
  size_t                  i;
  
  for (i = 0 ; i < pCollection->numItems ; ++i)
  {
    policyman_item_id_t     thisId;
    sys_modem_as_id_e_type  thisSubs;

    pItem = pCollection->pItems[i];
    thisId = policyman_item_get_id_msim(pItem, &thisSubs);
       
    if (id == thisId && subsId == thisSubs)
    {
      POLICYMAN_MSG_HIGH_2("Requested Item(id - %d, subs %d) found in the collection", id, subsId);
      return (policyman_item_t *)pItem;
    }
  }
  POLICYMAN_MSG_ERROR_2("Requested Item(id - %d, subs %d) NOT found in the collection", id, subsId);
  return NULL;
}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*-------- policyman_item_get_id --------*/
policyman_item_id_t policyman_item_get_id(
  policyman_item_t const  *pItem
)
{
  return (pItem == NULL)?   POLICYMAN_ITEM_INVALID
                          : policyman_cfgitem_id(pItem);
}


/*-------- policyman_item_get_id_msim --------*/
policyman_item_id_t policyman_item_get_id_msim(
  policyman_item_t const  *pItem,
  sys_modem_as_id_e_type  *pSubsId
)
{
  return (pItem == NULL)?   POLICYMAN_ITEM_INVALID
                          : policyman_cfgitem_id_msim(pItem, pSubsId);
}


/*-------- policyman_item_addref --------*/
void policyman_item_addref(
  policyman_item_t const  *pItem
)
{
  if (pItem != NULL)
  {
    policyman_cfgitem_addref(pItem);
  }
}


/*-------- policyman_item_collection_release --------*/
void policyman_item_collection_release(
  policyman_item_collection_t const *pCollection
)
{
  ref_cnt_obj_release((void *) pCollection);
}


/*-------- policyman_item_release --------*/
void policyman_item_release(
  policyman_item_t const  *pItem
)
{
  if (pItem != NULL)
  {
    policyman_cfgitem_release(pItem);
  }
}


/*-------- policyman_msgr_attachments_release --------*/
void policyman_msgr_attachments_release(
  msgr_hdr_s  *pMsg
)
{
  if (pMsg != NULL)
  {
    POLICYMAN_MSG_HIGH_1("Client thread called policyman_msgr_attachments_release(0x%08x)", pMsg);

    policyman_cfgitem_release_msg_attach(pMsg);
  }
}


/*-------- policyman_get_subs_boolean_value --------*/
boolean policyman_get_subs_boolean_value(
  char const              *pBooleanName,
  sys_modem_as_id_e_type   subsId,
  boolean                 *pValue
)
{
  mre_namespace_info_t  *pInfo;
  mre_status_t           status;
  boolean                value = FALSE;

  if (MAX_AS_IDS <= subsId)
  {
    status = MRE_STATUS_ERR_NOT_PRESENT;
    goto Done;
  }

  pInfo = policyman_get_namespace_info();

  status =  mre_boolean_get_value_from_set( pBooleanName,
                                            &value,
                                            pInfo->perSubsNamedObjs[subsId] );

  if (status == MRE_STATUS_SUCCESS)
  {
    *pValue = value;
    POLICYMAN_MSG_HIGH_2( SUBS_PREFIX "policyman_get_subs_boolean_value() returns true with value %d",
                          subsId,
                          *pValue );
  }
  else
  {
    POLICYMAN_MSG_HIGH_1( SUBS_PREFIX "policyman_get_subs_boolean_value() returns false",
                          subsId );
  }

Done:
  return MRE_STATUS_SUCCESS == status;
}


/*-------- policyman_get_boolean_value --------*/
boolean policyman_get_boolean_value(
  char const  *pBooleanName,
  boolean     *pValue
)
{
  return policyman_get_subs_boolean_value(pBooleanName, SYS_MODEM_AS_ID_1, pValue);
}

/*-------- policyman_get_boolean_with_default --------*/
boolean policyman_get_boolean_with_default(
  char const  *pBooleanName,
  boolean      defaultValue
)
{
  boolean value;

  return (TRUE == policyman_get_boolean_value(pBooleanName, &value))
          ? value
          : defaultValue;
}


/*-------- policyman_is_plmn_in_list --------*/
boolean  policyman_is_plmn_in_list(
  char const              *pPlmnList,
  sys_plmn_id_s_type      *pPlmn,
  sys_modem_as_id_e_type   subsId
)
{
  if (subsId >= SYS_MODEM_AS_ID_1 || subsId <= SYS_MODEM_AS_ID_2)
  {
    mre_set_t *pPlmnSet;

    pPlmnSet = (mre_set_t *)mre_named_object_find_with_subs( pPlmnList,
                                                           POLICYMAN_NAMED_PLMN_SET,
                                                           subsId,
                                                           policyman_get_namespace_info() );

    if (pPlmnSet != NULL)
    {
      return mre_set_contains(pPlmnSet, (void *)pPlmn);
    }
  }
  
  return FALSE;
}

/*-------- policyman_is_srlte_plmn --------*/
policyman_status_t policyman_is_srlte_plmn(
  sys_plmn_id_s_type  *pPlmnId,
  boolean              checkMccOnly,
  boolean             *pSvdPossible
)
{
  policyman_set_t    *pMccSet;
  policyman_set_t    *pPlmnSet;
  policyman_status_t  retval = POLICYMAN_STATUS_SUCCESS;
  boolean             is_srlte_on_any_plmn = FALSE;
  boolean            *pIsSrlteOnAnyPlmn = NULL;


  pMccSet  = (policyman_set_t *)mre_named_object_find_with_subs( "sxlte_mccs",
                                                                 POLICYMAN_NAMED_MCC_SET,
                                                                 SYS_MODEM_AS_ID_1,
                                                                 policyman_get_namespace_info() );

  pPlmnSet = (policyman_set_t *)mre_named_object_find_with_subs( "sxlte_plmns",
                                                                 POLICYMAN_NAMED_PLMN_SET,
                                                                 SYS_MODEM_AS_ID_1,
                                                                 policyman_get_namespace_info() );

  pIsSrlteOnAnyPlmn = (boolean *)mre_named_object_find_with_subs( "pm:srlte_on_any_plmn",
                                                                  POLICYMAN_NAMED_BOOLEAN,
                                                                  SYS_MODEM_AS_ID_1,
                                                                  policyman_get_namespace_info() );
  if (NULL != pIsSrlteOnAnyPlmn)
  {
    is_srlte_on_any_plmn = *pIsSrlteOnAnyPlmn;
  }

  if (!is_srlte_on_any_plmn)
  {
    // Same as pm:srlte_on_any_plmn set if have sxlte_mccs with no sxlte_plmns
    is_srlte_on_any_plmn = ( (NULL != pMccSet) && (NULL == pPlmnSet) );
  }

  /* If only MCC check is needed, compare against MCC of the PLMNs in list
   */
  if (checkMccOnly || is_srlte_on_any_plmn)
  {
    sys_mcc_type       *listMcc;
    sys_mcc_type        mcc;
    size_t              nElems;
    size_t              i;

    /* find a set of SVD MCCs, if not found then return error
      */
    if (pMccSet == NULL)
    {
      retval = POLICYMAN_STATUS_ERR_NOT_PRESENT;
      goto Done;
    }

    mcc    = policyman_plmn_get_mcc(pPlmnId);
    nElems = policyman_set_num_elems(pMccSet);

    for (i = 0; i < nElems; i++)
    {
      listMcc = (sys_mcc_type *)policyman_set_elem_ptr(pMccSet, i);

      if (listMcc == NULL)
      {
        POLICYMAN_MSG_ERROR_0("policyman_svd_possible_on_plmn: listMcc is NULL");
        retval = POLICYMAN_STATUS_ERR_INVALID_ARGS;
        goto Done;
      }

      if (mcc == *listMcc)
      {
        *pSvdPossible = TRUE;
        break;
      }
    }
  }
  else
  {
    if (pPlmnSet == NULL)
    {
      POLICYMAN_MSG_ERROR_0("policyman_svd_possible_on_plmn: set 'sxlte_plmns' is NULL");
      retval = POLICYMAN_STATUS_ERR_NOT_PRESENT;
      goto Done;
    }

    *pSvdPossible = policyman_set_contains(pPlmnSet, pPlmnId);
  }

Done:
  return retval;
}


/*-------- policyman_svd_possible_on_plmn --------*/
/**
@brief  Function to determine if SVD is possible on PLMN
*/
policyman_status_t policyman_svd_possible_on_plmn(
  sys_plmn_id_s_type  *pPlmnId,
  boolean              mccOnly,
  boolean             *pSvdPossible
)
{
  sys_subs_feature_t     feature = SYS_SUBS_FEATURE_MODE_NORMAL;
  policyman_status_t     retval  = POLICYMAN_STATUS_SUCCESS;

  if (NULL == pPlmnId)
  {
    retval = POLICYMAN_STATUS_ERR_INVALID_ARGS;
    POLICYMAN_MSG_ERROR_0("policyman_svd_possible_on_plmn: invalid arguments");
    goto Done;
  }

  /* Get Device config Policy Item
   */
  {
    policyman_item_t    *pItem;
    retval = policyman_get_current_device_config((policyman_item_t **)&pItem);

    if (retval != POLICYMAN_STATUS_SUCCESS)
    {
     POLICYMAN_MSG_ERROR_0("policyman_svd_possible_on_plmn: internal error");
     goto Done;
    }

   (void)policyman_device_config_get_subs_feature(pItem, 0, SYS_MODEM_AS_ID_1, &feature);
    ref_cnt_obj_release(pItem);
  }

  *pSvdPossible = FALSE;
  POLICYMAN_MSG_PLMN_0("queried PLMN is ", *pPlmnId);

  if (  feature == SYS_SUBS_FEATURE_MODE_SRLTE
     || feature == SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE
     )
  {
    retval = policyman_is_srlte_plmn(pPlmnId, mccOnly, pSvdPossible);
  }

Done:

 POLICYMAN_MSG_HIGH_3( "policyman_svd_possible_on_plmn: feature %d, retval %d, svdpossible %d",
                       feature,
                       retval,
                       *pSvdPossible );

  return retval;
}

/*-------- policyman_is_svd_allowed --------*/
policyman_status_t policyman_is_svd_allowed(
  boolean *pIsSvd
)
{
  /****************************************************************************
    This implementation is a hack.  It relies on knowledge of what is in
    mre_namespace_info_t to get the set in which the "sxlte_allowed" boolean
    will be put, relying on the fact that SXLTE will only ever be on the
    primary subs.
  ****************************************************************************/

  mre_namespace_info_t  *pInfo;

  if (!pIsSvd)
  {
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  pInfo = policyman_get_namespace_info();

  *pIsSvd = FALSE;
  (void) mre_boolean_get_value_from_set( "sxlte_allowed",
                                         pIsSvd,
                                         pInfo->perSubsNamedObjs[0] );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_is_svd_operation_allowed --------*/
boolean policyman_is_svd_operation_allowed(
  sys_modem_as_id_e_type subsId
)
{
  policyman_item_t      *pItem = NULL;
  policyman_status_t    status;
  sys_subs_feature_t    feature;
  boolean               retval = FALSE;

  /* Check range for subsId
   */
  if (subsId < SYS_MODEM_AS_ID_1 || subsId > SYS_MODEM_AS_ID_3)
  {
    return retval;
  }

  /* Get Device config Policy Item
   */
  status = policyman_get_current_device_config((policyman_item_t **)&pItem);

  /* UE is allowed for SVD if subs feature mode is not NORMAL and not SRLTE
   */
  if (status == POLICYMAN_STATUS_SUCCESS)
  {
    switch(subsId)
    {
      case SYS_MODEM_AS_ID_1:

        /* get SUBS feature mode
             */
        status = policyman_device_config_get_subs_feature(pItem, 0, subsId, &feature);
        if (status == POLICYMAN_STATUS_SUCCESS)
        {
          retval = !(   feature == SYS_SUBS_FEATURE_MODE_NORMAL
                     || feature == SYS_SUBS_FEATURE_MODE_SRLTE
                    );
        }
       break;

      /* Any other subscription is NORMAL
          */
      case SYS_MODEM_AS_ID_2:
      case SYS_MODEM_AS_ID_3:
      default:
        break;
    }
  }

  REF_CNT_OBJ_RELEASE_IF(pItem);

  return retval;
}
