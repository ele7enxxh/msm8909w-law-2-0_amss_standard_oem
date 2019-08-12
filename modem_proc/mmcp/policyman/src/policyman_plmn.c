/**
  @file policyman_plmn.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_plmn.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_xml.h"
#include "policyman_plmn.h"
#include "policyman_util.h"
#include "policyman_rules.h"
#include "policyman_xml.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_set.h"
#include "policyman_dbg.h"



/*=============================================================================
  PLMN APIs
=============================================================================*/


/*-------- policyman_plmns_are_equal --------*/
boolean policyman_plmns_are_equal(
  sys_plmn_id_s_type  *pPlmn1,
  sys_plmn_id_s_type  *pPlmn2
)
{
  return memcmp(pPlmn1->identity, pPlmn2->identity, sizeof(pPlmn1->identity)) == 0;
}


/*-------- policyman_plmn_clear --------*/
void policyman_plmn_clear(
  sys_plmn_id_s_type  *pPlmn
)
{
  memset(pPlmn, 0xFF, sizeof(sys_plmn_id_s_type));
}


/*-------- policyman_plmn_get_mcc --------*/
uint32 policyman_plmn_get_mcc(
  sys_plmn_id_s_type  *pPlmn
)
{
  byte  p0 = pPlmn->identity[0];
  byte  p1 = pPlmn->identity[1];

  return  (p0 & 0xf) * 100 + (p0 >> 4) * 10 + (p1 & 0x0f);
}


/*-------- policyman_plmn_from_mcc_mnc --------*/
void policyman_plmn_from_mcc_mnc(
  sys_plmn_id_s_type  *pPlmn,
  uint32               mcc,
  uint32               mnc
)
{
  uint32  d1;
  uint32  d2;

  if (mcc > 999 || mnc > 999)     { return; }

  d1 = mcc / 100;
  d2 = (mcc - d1 * 100) / 10;
  pPlmn->identity[0] = (byte) (d1 + (d2 << 4));

  d1 = mcc % 10;
  if (mnc > 99)
  {
    d2 = mnc % 10;
    mnc /= 10;
  }
  else
  {
    d2 = 0x0f;
  }
  pPlmn->identity[1] = (byte) (d1 + (d2 << 4));

  d1 = mnc / 100;
  d2 = (mnc - d1 * 100) / 10;
  pPlmn->identity[2] = (byte) (d1 + (d2 << 4));
}



/*  Index into the PLMN string at which we expect a hyphen.
 */
#define HYPHEN_INDEX  3



/*-------- get_bcd_digit --------*/
static boolean get_bcd_digit(
  char   c,
  byte  *pBcd
)
{
  if (c < '0' || c > '9')   { return FALSE; }

  *pBcd = c - '0';
  return TRUE;
}



/*-------- policyman_parse_ascii_plmn --------*/
boolean policyman_parse_ascii_plmn(
  char const          *pPlmnStr,
  sys_plmn_id_s_type  *pPlmn
)
{
  boolean success = FALSE;
  byte    d1;
  byte    d2;

  if (
        pPlmnStr == NULL
     || pPlmn == NULL
     || pPlmnStr[HYPHEN_INDEX] != '-'
     )
  {
    goto Done;;
  }

  /*  Get first two digits of MCC
   */
  if (!get_bcd_digit(*pPlmnStr++, &d1))       { goto Done; }
  if (!get_bcd_digit(*pPlmnStr++, &d2))       { goto Done; }
  pPlmn->identity[0] = (d2 << 4) + d1;

  /*  Get final digit of MCC
   */
  if (!get_bcd_digit(*pPlmnStr++, &d1))       { goto Done; }
  pPlmn->identity[1] = d1;

  /*  Skip the hyphen
   */
  ++pPlmnStr;

  /*  Get first two digits of MNC
   */
  if (!get_bcd_digit(*pPlmnStr++, &d1))       { goto Done; }
  if (!get_bcd_digit(*pPlmnStr++, &d2))       { goto Done; }
  pPlmn->identity[2] = (d2 << 4) + d1;

  /*  If there is a third digit in the MNC, fill it in; otherwise
   *  use 0xf0.
   */
  if (get_bcd_digit(*pPlmnStr, &d1))
  {
    pPlmn->identity[1] += d1 << 4;
  }
  else
  {
    pPlmn->identity[1] += 0xf0;
  }

  success= TRUE;

Done:
  return success;
}

/*-----------------------------------------------------------------------------
  Generic support for named list actions
-----------------------------------------------------------------------------*/

/*  This is the action to execute to make a named list for a policy.
 */
typedef struct
{
  POLICYMAN_ACTION_BASE;

  policyman_set_t *pList;
} policyman_list_action_t;


/*  Type of the function that reads the text inside the list element
 *  and returns a set that contains the appropriate members.
 */
typedef policyman_set_t * (*pfn_read_list_t)(
  policyman_xml_element_t const  *pElem,
  char                    const  *pStr
);


/*-------- policyman_named_list_action_dtor --------*/
/**
@brief  Destructor for a named list action.
*/
static void policyman_named_list_action_dtor(
  void  *pObj
)
{
  policyman_list_action_t  *pAction = (policyman_list_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pAction->pList);
  policyman_action_dtor(pAction);
}


/*-------- policyman_named_list_read --------*/
/**
@brief  Function to handle reading the text portion of the element and
        returning a named set containing the appropriate members.
*/
mre_status_t policyman_named_list_read(
  policyman_xml_element_t const  *pElem,
  policyman_named_object_type_t   listType,
  pfn_read_list_t                 pfnRead,
  policyman_set_t               **ppSet
)
{
  mre_status_t       status = MRE_STATUS_ERR_INVALID_ACTION;
  char const        *pSetName;
  char const        *pStr;
  policyman_set_t   *pSet = NULL;

  pSetName = policyman_xml_get_attribute(pElem, "name");
  if (pSetName == NULL)
  {
    goto Done;
  }

  pStr = policyman_xml_get_text(pElem);
  if (pStr == NULL)
  {
    goto Done;
  }

  pSet = pfnRead(pElem, pStr);
  if (pSet == NULL)
  {
    goto Done;
  }

  policyman_named_object_init( (policyman_named_object_t *)pSet,
                               pSetName,
                               listType );
  *ppSet = pSet;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- execute_named_list --------*/
static boolean execute_named_list(
  policyman_action_t const      *pAction,
  void                          *pCtx,
  sys_modem_as_id_e_type         subsId
)
{
  policyman_list_action_t *pListAction = (policyman_list_action_t *) pAction;
  policyman_policy_t      *pPolicy     = (policyman_policy_t *) pAction->pPolicy;

  if (pPolicy == NULL)
  {
    POLICYMAN_MSG_ERROR_0("Internal error - executing named list action with no policy");
    return FALSE;
  }

  POLICYMAN_MSG_HIGH_1(SRC_LINE "executing <plmn_list> or <mcc_list>", LINE_NUM(pListAction));

  return mre_named_object_add( (policyman_named_object_t *)pListAction->pList,
                               MRE_NAMESPACE_SUBS,
                               pAction->pPolicy );
}

/*-------- policyman_named_list_action_execute --------*/
static boolean policyman_named_list_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const  *pAct  = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_named_list);
}


/*-------- policyman_named_list_new --------*/
/**
@brief  Function to create a new named list action.  This takes a pointer
        to the function to execute for this
*/
mre_status_t policyman_named_list_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  pfn_read_list_t                 pfnRead,
  policyman_named_object_type_t   listType,
  mre_action_t                  **ppAction
)
{
  mre_status_t             status  = MRE_STATUS_ERR_INVALID_ACTION;
  policyman_list_action_t *pAction = NULL;
  policyman_set_t         *pSet    = NULL;

  status = policyman_named_list_read( pElem,
                                      listType,
                                      pfnRead,
                                      &pSet );

  if (MRE_FAILED(status) || pSet == NULL)
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( policyman_list_action_t,
                                  pPolicy,
                                  policyman_named_list_action_execute,
                                  policyman_named_list_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->pList    = pSet;

  *ppAction = (mre_action_t *) pAction;

Done:
  return status;
}

/*-----------------------------------------------------------------------------
  PLMN list support
-----------------------------------------------------------------------------*/


/*-------- policyman_plmn_list_read --------*/
policyman_set_t * policyman_plmn_list_read(
  policyman_xml_element_t const  *pElem,
  char const  *pStr
)
{
  policyman_set_t     *pSet;
  char                 plmnStr[10];
  sys_plmn_id_s_type   plmn;

  pSet = policyman_set_new(sizeof(plmn), 10, POLICYMAN_SET_NO_MAX_SIZE, NULL, NULL);

  /*  Add the PLMNs to the set
   */
  while (policyman_get_token(&pStr, plmnStr, sizeof(plmnStr)))
  {
    if (!policyman_parse_ascii_plmn(plmnStr, &plmn))
    {
      REF_CNT_OBJ_RELEASE_IF(pSet);
      goto Done;
    }

    policyman_set_add(pSet, &plmn);
  }

  POLICYMAN_MSG_HIGH_2( SRC_LINE "PLMN list created with %d items",
                        mre_xml_get_line_number(pElem),
                        policyman_set_num_elems(pSet));

Done:
  return pSet;
}


/*-------- policyman_plmn_list_new --------*/
mre_status_t policyman_plmn_list_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_named_list_new( pElem,
                                   pPolicy,
                                   policyman_plmn_list_read,
                                   POLICYMAN_NAMED_PLMN_SET,
                                   ppAction );
}


/*-----------------------------------------------------------------------------
  MCC list support
-----------------------------------------------------------------------------*/

/*-------- policyman_mcc_list_read --------*/
policyman_set_t * policyman_mcc_list_read(
  policyman_xml_element_t const  *pElem,
  char const  *pStr
)
{
  policyman_set_t     *pSet;
  char                 mccStr[4];
  uint32               mcc;

  pSet = policyman_set_new(sizeof(mcc), 10, POLICYMAN_SET_NO_MAX_SIZE, NULL, NULL);

  /*  Add the MCCs to the set
   */
  while (policyman_get_token(&pStr, mccStr, sizeof(mccStr)))
  {
    mcc = atoi(mccStr);
    if (mcc == 0)
    {
      REF_CNT_OBJ_RELEASE_IF(pSet);
      goto Done;
    }

    policyman_set_add(pSet, &mcc);
  }

  POLICYMAN_MSG_HIGH_2( SRC_LINE "MCC list created with %d items",
                        mre_xml_get_line_number(pElem),
                        policyman_set_num_elems(pSet));

Done:
  return pSet;
}


/*-------- policyman_mcc_list_new --------*/
mre_status_t policyman_mcc_list_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_named_list_new( pElem,
                                   pPolicy,
                                   policyman_mcc_list_read,
                                   POLICYMAN_NAMED_MCC_SET,
                                   ppAction );
}

/*-------- policyman_plmn_mcc_new --------*/
mre_status_t policyman_plmn_mcc_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition,
  pfn_evaluate_t                  evaluate
)
{
  mre_status_t               status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_mcc_condition_t *pCondition = NULL;
  char const                *pStr;
  policyman_set_t           *pSet = NULL;
  policyman_subs_t           pm_subs = SUBS_NONE;
  char                      *pSetName = NULL;

  /*  See if there is a named MCC list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");

  if (pStr != NULL)
  {
    pSetName = policyman_str_dup(pStr);
  }
  else
  {
    /*  No named set; try and get an inline list.
     */
    pStr = policyman_xml_get_text(pElem);

    if (pStr != NULL)
    {
      pSet = policyman_mcc_list_read(pElem, pStr);
    }
  }

  /*  If there is neither an inline or named list, bail.
   */
  if (pSet == NULL && pSetName == NULL)
  {
    goto Done;
  }

  /*  Get the subscription on which to check the MCC (no "subs" attribute
      means the multimode subs.
   */
  pStr = policyman_xml_get_attribute(pElem, "subs");

  if (pStr != NULL)
  {
    pm_subs = policyman_str_to_subs(pStr);
  }

  if (pm_subs == SUBS_NONE)
  {
    pm_subs = SUBS_THIS;
  }

  /*  Create the condition.
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_mcc_condition_t,
                                        pPolicy,
                                        evaluate,
                                        policyman_condition_mcc_dtor,
                                        pm_subs,
                                        pElem );

  pCondition->pMccSet  = pSet;
  pCondition->pSetName = pSetName;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-------- policyman_plmn_mcc_is_volte_only --------*/
boolean policyman_plmn_mcc_is_volte_only(
  sys_mcc_type            mcc,
  sys_modem_as_id_e_type  subsId
)
{
  policyman_set_t *pMccSet;
  boolean          result  = FALSE;

  pMccSet = (policyman_set_t *) mre_named_object_find_with_subs( "volte_only_mccs",
                                                                 POLICYMAN_NAMED_MCC_SET,
                                                                 subsId,
                                                                 policyman_get_namespace_info() );
  if (pMccSet != NULL)
  {
    result = policyman_set_contains(pMccSet, &mcc);
  }

  POLICYMAN_MSG_HIGH_4( SUBS_PREFIX "mcc_is_volte_only check for mcc %d, set 0x%x, result %d",
                        subsId,
                        mcc,
                        pMccSet,
                        result );

  return result;
}
