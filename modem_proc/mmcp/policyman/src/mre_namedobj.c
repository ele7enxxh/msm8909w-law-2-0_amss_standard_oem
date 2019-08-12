/**
  @file mre_namedobj.c

  @brief  
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/mre_namedobj.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_dbg.h"
#include "mre_engine.h"
#include "mre_namedobj.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_xml.h"



/*=============================================================================
  Named object APIs
=============================================================================*/


/*-------- ns_string_to_ns --------*/
STATIC mre_named_obj_namespace_t ns_string_to_ns(
  char const  *pStr
)
{
  typedef struct
  {
    const char                *pStr;
    mre_named_obj_namespace_t ns;
  } mask_map;

  static mask_map map[] =
  {
    {"policy",    MRE_NAMESPACE_POLICY},
    {"subs",      MRE_NAMESPACE_SUBS},
    {"global",    MRE_NAMESPACE_GLOBAL},
  };

  mre_named_obj_namespace_t ns = MRE_NAMESPACE_NONE;
  size_t                    idx;

  for (idx = 0 ; pStr != NULL && idx < ARR_SIZE(map) ; ++idx)
  {
    if (strcasecmp(pStr, map[idx].pStr) == 0)
    {
      ns = map[idx].ns;
      break;
    }
  }

  return ns;
}


/*-------- mre_named_object_get_namespace --------*/
mre_named_obj_namespace_t mre_named_object_get_namespace(
  mre_xml_element_t const *pElem
)
{
  char const  *pNamespace;

  pNamespace = mre_xml_get_attribute(pElem, "ns");
  return ns_string_to_ns(pNamespace);
}


/*-------- mre_named_object_dtor --------*/
void mre_named_object_dtor(
  void  *pObj
  )
{
  mre_named_object_t  *pNamedObj = (mre_named_object_t *) pObj;

  MRE_MEM_FREE_IF(pNamedObj->pName);
  mre_mem_free(pNamedObj);
}



/*-------- mre_named_object_init --------*/
void mre_named_object_init(
  mre_named_object_t      *pObj,
  char const              *pName,
  mre_named_object_type_t objType
  )
{
  if (pObj != NULL && pName != NULL)
  {
    pObj->pName = mre_str_dup(pName);
    pObj->objType = objType;
  }
}



/*-------- mre_ensure_obj_set --------*/
STATIC void mre_ensure_obj_set(
  mre_set_t **ppSet
  )
{
  if (NULL == *ppSet)
  {
    *ppSet = mre_set_refcnt_new(5, MRE_SET_NO_MAX_SIZE, NULL);
  }
}


/*-------- mre_named_object_add --------*/
boolean mre_named_object_add(
  mre_named_object_t        *pObj,
  mre_named_obj_namespace_t ns,
  mre_policy_t              *pPolicy
  )
{
  boolean                success = FALSE;
  mre_set_t            **ppNamedObjs = NULL;
  mre_namespace_info_t  *pNamespaceInfo = MRE_POLICY_NAMESPACE_INFO(pPolicy);

  pObj->pPolicy = pPolicy;

  /*  If no namespace is specified, use the default for the policy.
   */
  if ((MRE_NAMESPACE_NONE == ns) && (NULL != pNamespaceInfo))
  {
    ns = pNamespaceInfo->defaultNS;
  }

  /*  Get a pointer to storage for the pointer to the named object set.
   */
  switch (ns)
  {
    case MRE_NAMESPACE_POLICY:
    default:
      ppNamedObjs = &MRE_POLICY_PNAMEDOBJS(pPolicy);
      break;

    case MRE_NAMESPACE_SUBS:
      if (NULL != pNamespaceInfo)
      {
        sys_modem_as_id_e_type  subs = SYS_MODEM_AS_ID_1;
        if (NULL != pNamespaceInfo->pfnGetPolicySubs)
        {
          subs = pNamespaceInfo->pfnGetPolicySubs(pPolicy);
        }
        ppNamedObjs = &pNamespaceInfo->perSubsNamedObjs[(size_t) subs];
      }
      break;

    case MRE_NAMESPACE_GLOBAL:
      if (NULL != pNamespaceInfo)
      {
        ppNamedObjs = &pNamespaceInfo->pGlobalNamedObjs;
      }
      break;
  }

  /*  Create the set if it's not present, then add the object to that set.
   */
  if (NULL != ppNamedObjs)
  {
    mre_ensure_obj_set(ppNamedObjs);
    success = mre_set_add(*ppNamedObjs, &pObj);
  }

  return success;
}


/*-------- mre_named_object_find_in_set --------*/
mre_named_object_t *mre_named_object_find_in_set(
  char const              *pName,
  mre_named_object_type_t objType,
  mre_set_t               *pSet
  )
{
  mre_named_object_t  *pObj = NULL;

  if (NULL != pSet && NULL != pName)
  {
    pObj = mre_set_find_named_object(pSet, pName, objType);  
  }

  return  pObj;
}


/*-------- mre_named_object_find --------*/
mre_named_object_t *mre_named_object_find(
  char const              *pName,
  mre_named_object_type_t objType,
  mre_policy_t            *pPolicy
  )
{
  mre_named_object_t  *pObj = NULL;
  mre_namespace_info_t   *pNamespaceInfo = MRE_POLICY_NAMESPACE_INFO(pPolicy);
  sys_modem_as_id_e_type  subs = SYS_MODEM_AS_ID_1;

  if (NULL == pName)
  {
    goto Done;
  }

  /*  Look in policy first...
   */
  pObj = mre_named_object_find_in_set(pName, objType, MRE_POLICY_PNAMEDOBJS(pPolicy));
  if (NULL != pObj)
  {
    goto Done;
  }
  
  if (NULL != pNamespaceInfo)
  {
    if (NULL != pNamespaceInfo->pfnGetPolicySubs)
    {
      subs = pNamespaceInfo->pfnGetPolicySubs(pPolicy);
    }
    
    pObj = mre_named_object_find_with_subs(pName, objType, subs, pNamespaceInfo);
  }

Done:
  return pObj;
}


/*-------- mre_named_object_find_with_subs --------*/
mre_named_object_t *mre_named_object_find_with_subs(
  char const              *pName,
  mre_named_object_type_t  objType,
  sys_modem_as_id_e_type   subs,
  mre_namespace_info_t    *pNamespaceInfo
     )
  {
  mre_named_object_t     *pObj = NULL;

  if ((NULL == pName) || (NULL == pNamespaceInfo))
  {
    goto Done;
  }

  /*  Look in the subscription namespace...
   */
  pObj = mre_named_object_find_in_set( pName,
                                       objType,
                                       pNamespaceInfo->perSubsNamedObjs[(size_t) subs]);
  if (NULL != pObj)
  {
    goto Done;
  }

  /*  ...then in the globals
   */
  pObj = mre_named_object_find_in_set( pName,
                                       objType,
                                       pNamespaceInfo->pGlobalNamedObjs);

Done:
  return pObj;
}

