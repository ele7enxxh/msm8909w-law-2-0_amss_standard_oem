/**
  @file mre_policy.c

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_engine.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_efs.h"
#include "mre_engine.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_xml.h"

#include <stringl/stringl.h>



/*-------- mre__policy_dtor --------*/
void mre_policy_dtor(
  void  *pObj
)
{
  mre_policy_t  *pPolicy = (mre_policy_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(MRE_POLICY_PNAMEDOBJS(pPolicy));
  REF_CNT_OBJ_RELEASE_IF(MRE_POLICY_INITSET(pPolicy));
  REF_CNT_OBJ_RELEASE_IF(MRE_POLICY_ACTIONSET(pPolicy));

  mre_mem_free(pPolicy->pInfo);
  mre_mem_free(pPolicy);
}


/*-------- mre_policy_enable --------*/
void mre_policy_enable(
  mre_policy_t  *pPolicy,
  boolean       enable
)
{
  MRE_POLICY_ENABLED(pPolicy) = enable;
}


/*-------- mre_policy_is_enabled --------*/
boolean mre_policy_is_enabled(
  mre_policy_t  *pPolicy
)
{
  return MRE_POLICY_ENABLED(pPolicy);
}


/*-------- mre_policy_get_info --------*/
/**
@brief  Read the policy_info file for this policy, parse it, and return
        pointers to the parsed document tree.

@param[in]  pPath        Pointer to path to the policy XML file.
@param[in]  subsId       The context to read the policy file from (AS_ID)
@param[out] ppRoot       Pointer to storage for a pointer to the document root.

@return
  MRE_STATUS_SUCCESS if policy XML file was read and successfully parsed
  MRE_STATUS_ERR_NOT_PRESENT if policy XML was not found
  MRE_STATUS_ERR_MALFORMED_XML if policy XML was invalid
*/
static mre_status_t mre_policy_get_info(
  char const             *pPath,
  sys_modem_as_id_e_type  subsId,
  mre_xml_root_t const  **ppPolicyDoc
)
{
  size_t                   size;
  mre_efs_status_t         fs_status;
  mre_status_t             status;
  char const              *pPolicyInfo = NULL;
  mre_xml_root_t const    *pPolicyDoc  = NULL;
  mre_xml_element_t const *pElement;

  /*  Read the policy_info file for this policy.
   */
  fs_status = mre_efs_get_file( pPath,
                               (void **)&pPolicyInfo,
                               &size,
                               subsId );

  if (MRE_EFS_STATUS_SUCCESS != fs_status)
  {
    status = MRE_STATUS_ERR_NOT_PRESENT;
    goto Done;
  }

  /*  Parse the policy file into a document tree.
   */
  status = mre_xml_parse(pPolicyInfo, &pPolicyDoc);
  if (MRE_STATUS_SUCCESS != status)
  {
    MRE_MSG_ERROR_1("Failed to parse policy: %08x", status);
    goto Done;
  }

  /*  Check that the document has a root element.
   */
  pElement = mre_xml_get_root_element(pPolicyDoc);
  if (pElement == NULL)
  {
    status = MRE_STATUS_ERR_MALFORMED_XML;
    MRE_MSG_ERROR_0("Policy document does not have a root element");
    ref_cnt_obj_release((void *) pPolicyDoc);
    pPolicyDoc = NULL;
  }

Done:
  MRE_MEM_FREE_IF(pPolicyInfo);

  *ppPolicyDoc = pPolicyDoc;

  return status;
}


/*-------- mre_policy_get_version --------*/
static void mre_policy_get_version(
  mre_policy_t            *pPolicy,
  const mre_xml_element_t *pElement
)
{
  char const  *pStr;
  char        token[8];

  /*  Find the policy_ver attribute; return if not present.
   */
  pStr = mre_xml_get_attribute(pElement, "policy_ver");
  if (pStr == NULL)
  {
    MRE_UT_MSG_0("no policy version specified");
    return;
  }

  /*  Get anything up to a '.' that separates major and minor versions.
   */
  if (!mre_get_token_ex(&pStr, token, sizeof(token), '.'))
  {
    return;
  }

  /*  Get the policy number
   */
  MRE_POLICY_NUMBER(pPolicy) = atoi(token);

  /*  Get the policy type
   */
  if (*pStr++ == '.' && mre_get_token_ex(&pStr, token, sizeof(token), '.'))
  {
    MRE_POLICY_TYPE(pPolicy) = atoi(token);
  }

  /*  Get the policy version
   */
  if (*pStr++ == '.' && mre_get_token(&pStr, token, sizeof(token)))
  {
    MRE_POLICY_VERSION(pPolicy) = atoi(token);
  }
}


/*-------- mre_policy_get_changelist --------*/
static void mre_policy_get_changelist(
  mre_policy_t            *pPolicy,
  const mre_xml_element_t *pElement
  )
{
  char const  *pStr;
  char         token[32];

  /*  Find the changelist attribute; return if not present.
   */
  pStr = mre_xml_get_attribute(pElement, "changelist");
  if (pStr == NULL)
  {
    MRE_UT_MSG_0("no changelist specified");
    return;
  }

  /*  Get anything up to a ':' that separates RCS keyword from actual CL number.
   */
  if (!mre_get_token_ex(&pStr, token, sizeof(token), ':'))
  {
    return;
  }

  /* Ensure returned token is $Change:
   */
  if (strcasecmp(token, "$Change") != 0)
  {
    MRE_MSG_ERROR_0("token returned is not a changelist");
    return;
  }

  /*  Get the changelist type
   */
  if (*pStr++ == ':')
  {
    MRE_POLICY_CHANGELIST(pPolicy) = atoi(pStr);
  }
}


/*-------- mre_is_node_initial --------*/
static boolean mre_is_node_initial(
  char const  *pStr
)
{
  return strcasecmp(pStr, "initial") == 0;
}


/*-------- policy_add_action --------*/
static mre_status_t policy_add_action(
  mre_xml_element_t const *pElement,
  void                    *pCtx
)
{
  mre_policy_t *pPolicy = (mre_policy_t *) pCtx;
  char const   *pStr;
  boolean       succeeded;

  pStr = mre_xml_get_tag(pElement);
  MRE_XML_MSG("processing %s", pStr, 0, 0);

  if (mre_is_node_initial(pStr))
  {
    succeeded = mre_initset_add(pPolicy, pElement);
  }
  else
  {
    mre_set_t *pActionSet;

    pActionSet = mre_ensure_actionset(pPolicy);
    succeeded = mre_rule_add_action_to_set(pElement, pPolicy, pActionSet);
  }

  return succeeded? MRE_STATUS_SUCCESS : MRE_STATUS_ERR_MALFORMED_XML;
}


/*-------- mre_policy_add_actions --------*/
static mre_status_t mre_policy_add_actions(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElement
)
{
  mre_status_t  status;

  MRE_XML_MSG("###################################################", 0, 0, 0);
  MRE_XML_MSG("adding actions to policy", 0, 0, 0);

  status = mre_xml_process_children(pElement, policy_add_action, pPolicy);

  if (MRE_SUCCEEDED(status))
  {
    MRE_XML_MSG("action setup succeeded", 0, 0, 0);
  }
  else
  {
    MRE_XML_MSG("action setup failed", 0, 0, 0);
  }
  MRE_XML_MSG("###################################################", 0, 0, 0);

  mre_policy_enable(pPolicy, MRE_SUCCEEDED(status));

  return status;
}


/*-------- mre_policy_set_execute_subs --------*/
static void mre_policy_set_execute_subs(
  mre_policy_t            *pPolicy,
  sys_modem_as_id_e_type   subsId
)
{
  POLICY_SUBS(pPolicy) = (subsId == SYS_MODEM_AS_ID_NONE) ? SYS_MODEM_AS_ID_1 : subsId;
}


/*-------- mre_policy_are_preconds_met --------*/
boolean mre_policy_are_preconds_met(
  mre_policy_t  *pPolicy,
  uint32        precondsMet
)
{
  uint32  preconds = MRE_POLICY_PRECONDITIONS(pPolicy);

  return ((preconds & precondsMet) == preconds);
}


/*-------- mre_policy_are_preconds_met --------*/
boolean mre_policy_check_if_preconds_are_met(
  mre_policy_t  *pPolicy,
  uint32        precondsMet
)
{
  uint32  preconds = MRE_POLICY_PRECONDITIONS(pPolicy);

  MRE_MSG_HIGH_2( "Policy preconditions: required 0x%x, met 0x%x",
                  preconds,
                  precondsMet );

  return mre_policy_are_preconds_met(pPolicy, precondsMet);
}


/*=============================================================================
  Public MRE Engine APIs
=============================================================================*/


/*-------- mre_policy_create --------*/
mre_status_t mre_policy_create(
  char                          const  *pPolicyPath,
  sys_modem_as_id_e_type                subsId,
  mre_condition_map_element_t   const  *pCondTable,
  mre_action_map_element_t      const  *pActionTable,
  mre_precond_map_element_t     const  *pPrecondTable,
  pfnNewPolicy_t                        pfnNewPolicy,
  mre_policy_t                        **ppPolicy
)
{
  mre_status_t             status;
  mre_xml_root_t const    *pPolicyDoc  = NULL;
  mre_xml_element_t const *pPolicyElem;
  mre_policy_t            *pPolicy     = NULL;
  char const              *pPolicyName;

  /*---------------------------------------------------------------------------
    Try loading and parsing the XML file
  ---------------------------------------------------------------------------*/
  /*  Try to load the given policy.
   */
  status = mre_policy_get_info( pPolicyPath,
                                subsId,
                                &pPolicyDoc );

  /*  If unable to load policy file return an error.
   */
  if (MRE_FAILED(status) || NULL == pPolicyDoc)
  {
    switch(status)
    {
      case MRE_STATUS_ERR_NOT_PRESENT:
        MRE_UT_MSG_S2( "subs %d: Policy file '%s' was not present in EFS",
                      subsId,
                      pPolicyPath );
        break;

      case MRE_STATUS_ERR_MALFORMED_XML:
        MRE_MSG_ERROR_0("Malformed XML in policy file");
        MRE_UT_MSG_S2( "subs %d: Malformed policy file '%s'",
                      subsId,
                      pPolicyPath );

        break;

      default:
        MRE_MSG_ERROR_1("Unknown error %d in mre__parse_policy", status);
        break;
    }
    goto Done;
  }

  /*---------------------------------------------------------------------------
    Call the client callback to create a policy.
  ---------------------------------------------------------------------------*/

  /*  Get a pointer to the actual policy node.
   */
  pPolicyElem = mre_xml_get_root_element(pPolicyDoc);

  /*  Create the policy.
   */
  pPolicyName = mre_xml_get_attribute(pPolicyElem, "name");
  pPolicy = pfnNewPolicy(pPolicyName, pPolicyElem);
  if (pPolicy == NULL)
  {
    MRE_MSG_ERROR_0("Unable to create policy from XML");
    goto Done;
  }

  /*---------------------------------------------------------------------------
    With policy in hand, set it up.
  ---------------------------------------------------------------------------*/
  *ppPolicy = pPolicy;
  mre_policy_set_execute_subs(pPolicy, subsId);
  pPolicy->pInfo = mre_mem_alloc(sizeof(mre_policy_info_t));
  MRE_POLICY_PRECONDITIONS(pPolicy) = 0;
  MRE_POLICY_CONDTBL(pPolicy) = pCondTable;
  MRE_POLICY_ACTTBL(pPolicy) = pActionTable;
  MRE_POLICY_PRECONDTBL(pPolicy) = pPrecondTable;

  /*  Get the version of the policy.
   */
  mre_policy_get_version(pPolicy, pPolicyElem);

  /*  Get the changelist of the policy.
   */
  mre_policy_get_changelist(pPolicy, pPolicyElem);

  /*  Add the rules to the policy.
   */
  mre_policy_add_actions(pPolicy, pPolicyElem);

Done:
  if (pPolicyDoc != NULL)
  {
    ref_cnt_obj_release((void *) pPolicyDoc);
  }

  return status;
}


/*-------- mre_policy_init --------*/
void mre_policy_init(
  mre_policy_t          *pPolicy,
  mre_namespace_info_t  *pNamespaceInfo,
  void                  *pCtx
  )
{
  MRE_POLICY_NAMESPACE_INFO(pPolicy) = pNamespaceInfo;

  if (NULL != MRE_POLICY_INITSET(pPolicy))
  {
    MRE_POLICY_PRECONDSAREMET(pPolicy) = TRUE;
    mre_actionset_execute(MRE_POLICY_INITSET(pPolicy), pCtx);
  }
}


/*-------- mre_policy_execute --------*/
void mre_policy_execute(
  mre_policy_t  *pPolicy,
  uint32         precondsMet,
  void          *pCtx
)
{
  if (!MRE_POLICY_ENABLED(pPolicy))
  {
    MRE_MSG_ERROR_0("Policy not enabled due to parse/initialization failure");
    return;
  }

  if (NULL == MRE_POLICY_ACTIONSET(pPolicy))
  {
    MRE_MSG_ERROR_0("No actions for policy");
    return;
  }

  MRE_POLICY_PRECONDSAREMET(pPolicy) = mre_policy_check_if_preconds_are_met(pPolicy, precondsMet);
  MRE_POLICY_CURRENT_PRECONDS(pPolicy) = precondsMet;
  MRE_POLICY_SET_TOP_LEVEL(pPolicy);
  MRE_POLICY_CONTEVAL(pPolicy) = FALSE;
  MRE_POLICY_STOPEVAL(pPolicy) = FALSE;

  mre_actionset_execute(MRE_POLICY_ACTIONSET(pPolicy), pCtx);
}
