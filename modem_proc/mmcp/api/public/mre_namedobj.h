#ifndef _MRE_NAMEDOBJ_H_
#define _MRE_NAMEDOBJ_H_

/**
  @file mre_namedobj.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/mre_namedobj.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


#ifdef __cplusplus
extern "C" {
#endif

#include "mre.h"
#include "sys.h"


typedef struct mre_named_object_t mre_named_object_t;


typedef enum
{
  MRE_UNNAMED_OBJ,
  MRE_NAMED_PLMN_SET,
  MRE_NAMED_RF_BANDS,
  MRE_NAMED_TIMER,
  MRE_NAMED_MCC_SET,
  MRE_NAMED_BOOLEAN,
  MRE_NAMED_CONFIG,
} mre_named_object_type_t;


#define MRE_NAMED_OBJECT_BASE       \
  _REF_CNT_OBJ;                     \
  mre_named_object_type_t  objType; \
  char const              *pName;   \
  mre_policy_t            *pPolicy


struct mre_named_object_t
{
  MRE_NAMED_OBJECT_BASE;
};


typedef enum
{
  MRE_NAMESPACE_NONE,
  MRE_NAMESPACE_POLICY,
  MRE_NAMESPACE_SUBS,
  MRE_NAMESPACE_GLOBAL
} mre_named_obj_namespace_t;


typedef sys_modem_as_id_e_type  (*pfnGetPolicySubs_t)( mre_policy_t *pPolicy);

struct mre_namespace_info_t
{
  mre_named_obj_namespace_t  defaultNS;
  pfnGetPolicySubs_t         pfnGetPolicySubs;
  mre_set_t                 *perSubsNamedObjs[MAX_AS_IDS];
  mre_set_t                 *pGlobalNamedObjs;
};

/*-------- mre_named_object_get_namespace --------*/
mre_named_obj_namespace_t mre_named_object_get_namespace(
  mre_xml_element_t const *pElem
);


/*-------- mre_named_object_dtor --------*/
/**
@brief

@param

@return

*/
void mre_named_object_dtor(
  void  *pObj
);


/*-------- mre_named_object_init --------*/
/**
@brief  Initialize the name and type of a named object.

@param[in]  pSet    Pointer to the named object.
@param[in]  pName   The name of the object.
@param[in]  objType The type of the object

@return
  Nothing.
*/
void mre_named_object_init(
  mre_named_object_t      *pObj,
  char const              *pName,
  mre_named_object_type_t  objType
);


/*-------- mre_named_object_add --------*/
/**
@brief  Add a named object.

@param[in]  pObj    Named object.
@param[in]  ns      Namespace in which object is to be added.
@param[in]  pPolicy Pointer to the policy to which this object applies.

@return
  TRUE if added, FALSE if not (too many named objects).
*/
boolean mre_named_object_add(
  mre_named_object_t        *pObj,
  mre_named_obj_namespace_t  ns,
  mre_policy_t              *pPolicy
);

/*-------- mre_set_find_named_object --------*/
/**
@brief  Find a named object within this set with a given name and type.

NOTE: The set on which this is called must be a set of pointers to named object,
and those named objects should have their pName member pointing to the set name.

@param[in]  pSet    Pointer to the policyman set.
@param[in]  pName   The name of the set for which to search.
@param[in]  objType The type of the object which is being looked for.

@return
  Pointer to the object, or NULL if it is not present.
*/
mre_named_object_t * mre_set_find_named_object(
  mre_set_t const         *pSet,
  char const              *pName,
  mre_named_object_type_t  objType
);


/*-------- mre_named_object_find_in_set --------*/
/**
@brief  Replacement for mre_named_object_find when executing in another
        task's context and you don't have a pointer to the governing
        policy.  Requires that you know the address of the set.

@param

@return

*/
mre_named_object_t * mre_named_object_find_in_set(
  char const              *pName,
  mre_named_object_type_t  objType,
  mre_set_t               *pSet
);


/*-------- mre_named_object_find --------*/
/**
@brief  Get a pointer to a named object

@param[in]  pName   Pointer to a NUL terminated name of the object.
@param[in]  objType Type of object.
@param[in]  pPolicy Policy querying for this object.

@return
  Pointer to the named object, if present, NULL otherwise.
  NOTE: A reference has been added to the object before returning it to the caller.
  The caller MUST release this reference when it is done.
*/
mre_named_object_t * mre_named_object_find(
  char const              *pName,
  mre_named_object_type_t  objType,
  mre_policy_t            *pPolicy
);


/*-------- mre_named_object_find_with_subs --------*/
mre_named_object_t * mre_named_object_find_with_subs(
  char const              *pName,
  mre_named_object_type_t  objType,
  sys_modem_as_id_e_type   subs,
  mre_namespace_info_t    *pNamespaceInfo
);



/*-------- mre_boolean_get_value_from_set --------*/
mre_status_t mre_boolean_get_value_from_set(
  char const          *pName,
  boolean             *pValue,
  mre_set_t           *pSet
);


/*-------- mre_boolean_get_value_ --------*/
mre_status_t mre_boolean_get_value(
  char const          *pName,
  boolean             *pValue,
  mre_policy_t        *pPolicy
);


#ifdef __cplusplus
}
#endif

#endif /* _MRE_NAMEDOBJ_H_ */
