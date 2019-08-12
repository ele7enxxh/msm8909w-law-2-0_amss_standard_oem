/*!
  @file ds_3gpp_auth_mgr.c

  @brief
  Includes functionality for the authentication manager which manages
  auth fallback.

*/ 

/*===========================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_auth_mgr.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
2/16/12    rr      Auth fallback support for cause code 31
12/20/11   rr      Added messages for debugging purposes
09/30/11   rr      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ds_3gpp_auth_mgr.h"
#include "ds_3gpp_nv_manager.h"
#include "list.h"
#include <stringl/stringl.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DS_3GPP_AUTH_FB_TBL_SIZE (16) /* Max number of entries in the 
                                         auth_type_fb_tbl */

#define DS_3GPP_AUTH_MGR_MAX_RETRIES (3)

/*===========================================================================
  Cookie to ensure that dynamic memory allocated is valid
===========================================================================*/
#define DS_3GPP_AUTH_MGR_PER_SUBS_INFO_COOKIE 0xabcdefab

typedef struct
{
  list_link_type             link;
  byte                       apn[DS_UMTS_MAX_APN_STRING_LEN + 1];
  ds_umts_pdp_auth_enum_type auth_type;
  boolean                    success;  /* if TRUE, call succeeded to apn 
                                         using the auth_type in this entry*/
  int                        count;    /* counts the number of attempts 
                                         before a call is sucessful */
} ds_3gpp_auth_mgr_entry_s;

/*===========================================================================
   This Structure is used to maintain all subscription-speicifc global                              
   variables of ds_3gpp_auth_mgr.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW                                         
   SUBSCRIPTION-SPECIFIC GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  list_type list;
  uint32    cookie;
}ds_3gpp_auth_mgr_per_subs_info_type;

/*===========================================================================
   This Structure is used to maintain all global variables of ds_3gpp_auth_mgr.
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  ds_3gpp_auth_mgr_per_subs_info_type *per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX];
}ds_3gpp_auth_mgr_info_type;

static ds_3gpp_auth_mgr_info_type ds_3gpp_auth_mgr_info = {{NULL}};



/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/ 

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Subscription specific Auth Mgr Information
 
SIDE EFFECTS
  None.
===========================================================================*/
static ds_3gpp_auth_mgr_per_subs_info_type* 
         ds_3gpp_auth_mgr_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS 
  per_subs_info_hndl: Handle to the Subscription specific Auth Mgr Information 
  subs_id:            Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  
 
SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_auth_mgr_set_per_subs_info_hndl
(
  ds_3gpp_auth_mgr_per_subs_info_type* hndl,
  sys_modem_as_id_e_type               subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS
  hndl  : Handle to the Subscription specific
          Auth Mgr Information

DEPENDENCIES
  None.

RETURN VALUE
  TRUE,  if the handle is valid
  FALSE, if invalid

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_auth_mgr_validate_per_subs_info_hndl
(
  ds_3gpp_auth_mgr_per_subs_info_type *hndl
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_LIST_PTR

DESCRIPTION
  This function fetches a pointer to the Auth mgr List
  
PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Pointe to the Auth Mgr List

SIDE EFFECTS
  None.
===========================================================================*/
static list_type* ds_3gpp_auth_mgr_get_list_ptr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_NEW_ENTRY

DESCRIPTION
  This function allocates a new entry of type
  ds_3gpp_auth_mgr_entry_s from heap.
 
PARAMETERS   : 
  None

DEPENDENCIES
  None

RETURN VALUE
 Pointer to the newly allocated chunk of memory.

SIDE EFFECTS
  None
===========================================================================*/
    
LOCAL ds_3gpp_auth_mgr_entry_s*
ds_3gpp_auth_mgr_get_new_entry( void )
{
  ds_3gpp_auth_mgr_entry_s *entry = DS_3GPP_MEM_ALLOC(sizeof(ds_3gpp_auth_mgr_entry_s));
  if(entry)
  {
    memset(entry, 0, sizeof(ds_3gpp_auth_mgr_entry_s));
  }
  return entry;
} /* ds_3gpp_auth_mgr_get_new_entry() */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_NEXT

DESCRIPTION
  Given an auth_type, the function returns the next auth type in
  sequence to try during auth fallback. The sequence used in auth
  fallback is {CHAP, PAP, NONE}

PARAMETERS   :
  auth_type  -  Authentication type 

DEPENDENCIES
  None

RETURN VALUE
  The next auth_type to try in sequence.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_umts_pdp_auth_enum_type
ds_3gpp_auth_next(ds_umts_pdp_auth_enum_type auth_type)
{
  /*------------------------------------------------------------------
     Return the next auth type to try in the cycle
   ------------------------------------------------------------------*/
  switch(auth_type)
  {
  case DS_UMTS_PDP_AUTH_CHAP:
    return DS_UMTS_PDP_AUTH_PAP;
  case DS_UMTS_PDP_AUTH_PAP:
    return DS_UMTS_PDP_AUTH_NONE;
  case DS_UMTS_PDP_AUTH_NONE:
    return DS_UMTS_PDP_AUTH_CHAP;
  default:
    DS_3GPP_MSG1_ERROR("ds_3gpp_auth_next: Invalid auth_type %d", auth_type);
    return auth_type;
  }
} /* ds_3gpp_auth_next() */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_APN_COMPARE

DESCRIPTION
  This function is used as a support function for
  list_linear_search(). Compares the apn string with an auth table
  entry.

PARAMETERS   :
  entry  -  Auth manager list entry
  apn    -  Apn to compare with
DEPENDENCIES
  None

RETURN VALUE
  The next auth_type to try in sequence.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL int
ds_3gpp_auth_mgr_apn_compare
(
  void *item_ptr,
  void *compare_val
)
{
  ds_3gpp_auth_mgr_entry_s *entry;
  
  entry = (ds_3gpp_auth_mgr_entry_s *) item_ptr;
  return (strcasecmp((char *)entry->apn, (char *)compare_val) == 0);
} /* ds_3gpp_auth_mgr_apn_compare()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_LOOKUP_ENTRY

DESCRIPTION
  This function searches for an entry in the auth table, using
  the apn string. It should only be used if auth fallback feature is
  enabled, since it accesses ds_3gpp_auth_mgr_list. 

PARAMETERS   :
  apn      -  Apn name to be used for the table lookup
  subs_id  -  Subscription id

DEPENDENCIES
  None

RETURN VALUE
  If entry found, a pointer to an entry in the auth table
  (ds_3gpp_auth_fb_tbl)
  Else NULL

SIDE EFFECTS
   
===========================================================================*/ 
LOCAL ds_3gpp_auth_mgr_entry_s *
ds_3gpp_auth_mgr_lookup_entry
(
  const byte              *apn,
  sys_modem_as_id_e_type   subs_id
)
{
  list_type*                list_ptr = NULL;
  ds_3gpp_auth_mgr_entry_s* list_entry_ptr = NULL;
  /*------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_auth_mgr_get_list_ptr(subs_id);

  if (list_ptr != NULL)
  {
    list_entry_ptr =  
      list_linear_search(list_ptr, ds_3gpp_auth_mgr_apn_compare,(byte*)apn);
  }

  return list_entry_ptr; 
} /* ds_3gpp_auth_mgr_lookup_entry()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_INIT_ENYRY

DESCRIPTION
  This function initializes an auth table entry 

PARAMETERS   :
  entry   -      Pointer to entry to be initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 
===========================================================================*/
LOCAL  void
ds_3gpp_auth_mgr_init_entry
(
  ds_3gpp_auth_mgr_entry_s *entry,
  const byte               *apn
)
{
  memset((void *)entry->apn, 0, DS_UMTS_MAX_APN_STRING_LEN + 1);
  strlcpy((char *)entry->apn, (char *)apn, DS_UMTS_MAX_APN_STRING_LEN + 1);
  entry->count = 0;
  entry->success = FALSE;
  entry->auth_type = DS_UMTS_PDP_AUTH_CHAP;
} /*ds_3gpp_auth_mgr_init_entry()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_UPDATE_ENTRY

DESCRIPTION
  This function updates an entry in ds_3gpp_auth_fb_tbl. List of valid
  updates:
  Auth successful scenario -
  (auth_type, success, count) = {auth_type, true, 0}
  Auth failure scenario -
  (auth_type, success, count) = {next(auth_type), false, count + 1}
 
PARAMETERS   :
  entry  -  Pointer to the entry to be updated.
  success - Specifies the type of update
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void 
ds_3gpp_auth_mgr_update_entry
(
  ds_3gpp_auth_mgr_entry_s   *entry,
  boolean                    success
)
{ 
  if(entry == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_auth_fb_tbl_update_entry: Cannot update."
                       " entry NULL");
    return;
  }
  
  /*------------------------------------------------------------------ 
     Entry exists. Update
   ------------------------------------------------------------------*/
  if(success == TRUE)
  {
    /*------------------------------------------------------------------ 
      ds_3gpp_auth_mgr_update_entry called because of auth success
     ------------------------------------------------------------------*/
    entry->success = TRUE;
    entry->count = 0;
  }
  else 
  {
    /*------------------------------------------------------------------ 
      ds_3gpp_auth_mgr_update_entry called because of auth failure
     ------------------------------------------------------------------*/
    entry->success = FALSE;
    entry->count++;
    entry->auth_type = ds_3gpp_auth_next(entry->auth_type);
  }
} /*ds_3gpp_auth_mgr_update_entry()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_EVICT_ENTRY

DESCRIPTION
  This function removes and returns the least recently used entry from
  the auth manager list.
 
PARAMETERS   :
  List Ptr

DEPENDENCIES
  None

RETURN VALUE
 Pointer to the evicted entry

SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_3gpp_auth_mgr_entry_s * ds_3gpp_auth_mgr_evict_entry
(
  list_type                *list_ptr
)
{
  ds_3gpp_auth_mgr_entry_s  *auth_mgr_entry_ptr = NULL;
  /*----------------------------------------------------------------------*/

  if (list_ptr != NULL)
  {
    auth_mgr_entry_ptr = (ds_3gpp_auth_mgr_entry_s *)list_pop_back(list_ptr);
  }

  return auth_mgr_entry_ptr;

} /* ds_3gpp_auth_mgr_evict_entry()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_CREATE_ENTRY

DESCRIPTION
  This function creates an auth manager list entry
  The new entry has values (auth_type, success, count) = {CHAP, FALSE,
  0} 
 
PARAMETERS   : 
  apn     -  Apn name for which an entry is being created
  subs_id -  Subscription Id

DEPENDENCIES
  None

RETURN VALUE
 On success, returns a pointer to the newly created entry
 On failure, NULL
 
SIDE EFFECTS
  None
===========================================================================*/
LOCAL ds_3gpp_auth_mgr_entry_s *
ds_3gpp_auth_mgr_create_entry
(
  const byte             *apn,
  sys_modem_as_id_e_type  subs_id 
)
{
  ds_3gpp_auth_mgr_entry_s *entry = NULL;
  list_type                *list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_auth_mgr_get_list_ptr(subs_id);

  if (list_ptr == NULL)
  {
    ASSERT(0);
    return NULL;
  }

  if(list_size(list_ptr) == DS_3GPP_AUTH_FB_TBL_SIZE)
  {
    /*------------------------------------------------------------------ 
      List is full, so we try to evict an entry
     ------------------------------------------------------------------*/
    entry = ds_3gpp_auth_mgr_evict_entry(list_ptr);
  }
  else
  {
    /*------------------------------------------------------------------     
     List is not full. We allocate a new entry in the list. If alloc
     fails, we try to evict.
    -------------------------------------------------------------------*/
    entry = ds_3gpp_auth_mgr_get_new_entry();
    if(entry == NULL)
    {
      entry = ds_3gpp_auth_mgr_evict_entry(list_ptr);
    }
  }
  
  if(entry != NULL) 
  {
    list_push_front(list_ptr, &(entry->link));
    ds_3gpp_auth_mgr_init_entry(entry, apn);
  }

  return entry;
}/*ds_3gpp_auth_mgr_create_entry()*/

/*===========================================================================
FUNCTION ds_3gpp_auth_mgr_nv_refresh_per_subs_info

DESCRIPTION
  This function clears/resets the subscription-specific variables in auth manager.
 
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

void ds_3gpp_auth_mgr_nv_refresh_per_subs_info
(
  sys_modem_as_id_e_type  subs_id 
)
{

  list_type                *list_ptr = NULL;
  ds_3gpp_auth_mgr_entry_s *current_entry = NULL;
  ds_3gpp_auth_mgr_entry_s *next_entry = NULL;
  ds_3gpp_auth_mgr_per_subs_info_type *hndl;

  list_ptr = ds_3gpp_auth_mgr_get_list_ptr(subs_id);


  DS_3GPP_MSG0_MED("3GPP Auth Manager Clearing Per Subsid");

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  
  if(list_ptr != NULL)
  { 
    current_entry = 
      (ds_3gpp_auth_mgr_entry_s*)list_peek_front( list_ptr );

    while (current_entry != NULL)
    { 
      next_entry= 
           list_peek_next(list_ptr, &(current_entry->link));
      list_pop_item(list_ptr, &(current_entry->link) );
      modem_mem_free(current_entry,MODEM_MEM_CLIENT_DATA);
      current_entry = next_entry;
    }
    list_destroy(list_ptr);
  }
  hndl = ds_3gpp_auth_mgr_get_per_subs_info_hndl(subs_id);
  modem_mem_free(hndl,MODEM_MEM_CLIENT_DATA);
  hndl = NULL;
  ds_3gpp_auth_mgr_set_per_subs_info_hndl(hndl,subs_id);
}

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes the subscription-specific variables in auth manager.
 
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_auth_mgr_per_subs_info_init
(
  sys_modem_as_id_e_type      subs_id
)
{
  ds_3gpp_auth_mgr_per_subs_info_type* hndl = NULL;
  /*---------------------------------------------------------------------*/

  hndl = ds_3gpp_auth_mgr_get_per_subs_info_hndl(subs_id);

  do
  {
    if (hndl != NULL &&
        hndl->cookie == DS_3GPP_AUTH_MGR_PER_SUBS_INFO_COOKIE)
    {
      DS_3GPP_MSG1_ERROR ("Memory already allocated for subs id: %d", subs_id );
      break;
    }
    else
    {
      hndl = (ds_3gpp_auth_mgr_per_subs_info_type*)
               modem_mem_alloc (sizeof(ds_3gpp_auth_mgr_per_subs_info_type),
                                MODEM_MEM_CLIENT_DATA);
      if (hndl == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for subs id: %d", 
                            subs_id );
        break;
      }

      hndl->cookie = DS_3GPP_AUTH_MGR_PER_SUBS_INFO_COOKIE;
      
      list_init(&(hndl->list));

      ds_3gpp_auth_mgr_set_per_subs_info_hndl(hndl, subs_id);
    }
  }
  while (0); 

  return;
} /* ds_3gpp_auth_mgr_per_subs_info_init */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_INIT

DESCRIPTION
  This function initializes the auth manager. It sets up the auth
  table (ds_3gpp_auth_fb_tbl).
 
PARAMETERS   :
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void ds_3gpp_auth_mgr_init( void )
{
  sys_modem_as_id_e_type   subs_id = ds3g_get_ps_subs_id();

  if(ds_3gpp_nv_manager_get_auth_fallback(subs_id) == TRUE)
  {
    DS_3GPP_MSG0_HIGH("ds_3gpp_auth_mgr_init: Auth fallback enabled");
  }
  else
  {  
    DS_3GPP_MSG0_HIGH("ds_3gpp_auth_mgr_init: Auth fallback disabled");
  }
  ds_3gpp_auth_mgr_per_subs_info_init(subs_id);

} /*ds_3gpp_auth_mgr_init()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_NEED_REDIAL

DESCRIPTION
  Queris the auth manager if a redial is required.
 
PARAMETERS   :
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  cause_code           - PS IFACE cause code specifying the reason for
                         prior termination
  subs_id              - Subscription Id
DEPENDENCIES 
  None

RETURN VALUE
  TRUE - if redialing is required
  FALSE - Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean
ds_3gpp_auth_mgr_need_redial
(
  byte                          *apn,
  ds_umts_pdp_auth_enum_type    requested_auth_type,
  ps_iface_net_down_reason_type cause_code,
  boolean                       call_was_connected,
  sys_modem_as_id_e_type        subs_id
)
{
  ds_3gpp_auth_mgr_entry_s *entry;

  /*-------------------------------------------------------------------------*/

  if( (ds_3gpp_nv_manager_get_auth_fallback(subs_id) == FALSE)
     || requested_auth_type != DS_UMTS_PDP_AUTH_CHAP_OR_PAP
     || (cause_code != PS_NET_DOWN_REASON_AUTH_FAILED
         && cause_code != PS_NET_DOWN_REASON_ACTIVATION_REJECT)
     || call_was_connected )
  {
    DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_need_redial: No"
                            " redial. APN: %s, requested_auth_type: %d,"
                            " cause_code: %d", (char *) apn,
                            requested_auth_type, cause_code);
    return FALSE;
  }

  entry = ds_3gpp_auth_mgr_lookup_entry(apn, subs_id);
  if(entry == NULL)
  {
    /*------------------------------------------------------------------ 
       Lookup failed. We dont have info to determine if we should
       redial.
     ------------------------------------------------------------------*/
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ds_3gpp_auth_mgr_need_redial: Apn "
                            "lookup failed for %s returning FALSE",(char *)apn);
    return FALSE;
  }
  /*------------------------------------------------------------------ 
       Lookup succeeded. We dont have to examine count.
       ds_3gpp_auth_mgr_call_failure() removes the entry if max retries
       has exceeded. Return TRUE.
  ------------------------------------------------------------------*/
  DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_need_redial: "
                          "Return True. Apn: %s, count: %d", (char *) apn,
                          entry->count);

  return TRUE;
} /*ds_3gpp_auth_mgr_need_redial()*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_AUTH_TYPE

DESCRIPTION
  Queries the auth manager for the appropriate auth_type to use for an
  outgoing call.
 
PARAMETERS   : 
  IN
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id
  OUT
  auth_type_to_use_p  - Pointer to the auth_type to be used

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the LRU auth manager list 
===========================================================================*/  
void 
ds_3gpp_auth_mgr_get_auth_type 
(
  const byte                  *apn, 
  ds_umts_pdp_auth_enum_type  requested_auth_type, 
  ds_umts_pdp_auth_enum_type  *auth_type_to_use_p,
  sys_modem_as_id_e_type      subs_id
)
{
  ds_3gpp_auth_mgr_entry_s *entry;
  list_type*                list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_auth_mgr_get_list_ptr(subs_id);

  if(list_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  if(auth_type_to_use_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_auth_mgr_get_auth_type: Invalid args."
                       " auth_type_to_use_p NULL");
    return;
  }

  *auth_type_to_use_p = requested_auth_type;

  if( (ds_3gpp_nv_manager_get_auth_fallback(subs_id) == TRUE)
     && (requested_auth_type == DS_UMTS_PDP_AUTH_CHAP_OR_PAP))
  {
    entry = ds_3gpp_auth_mgr_lookup_entry(apn, subs_id);
    if(entry == NULL)
    {
      /* Entry does not exist, create it*/
      entry = ds_3gpp_auth_mgr_create_entry(apn, subs_id);    
      
      if(entry == NULL)
      {
        /*------------------------------------------------------------------ 
           If create failed, we proceed as if authentication fall back was
         disasbled and try the call with auth type CHAP_OR_PAP.
         ds_3gpp_auth_mgr_need_redial returns FALSE next time
        ------------------------------------------------------------------*/ 
        DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ds_3gpp_auth_mgr_get_auth_type: "
                                "Could not lookup/create auth table entry for apn %s", apn);         
      }
      else
      {
        /*------------------------------------------------------------------ 
          Create succeeded, we try with auth type CHAP
         ------------------------------------------------------------------*/
        *auth_type_to_use_p = DS_UMTS_PDP_AUTH_CHAP;
      }
    }
    else
    {
      /*------------------------------------------------------------------ 
        Entry exists, use auth type from fallback table
       ------------------------------------------------------------------*/
      *auth_type_to_use_p = entry->auth_type;

      /*------------------------------------------------------------------ 
        Re-order list as LRU
       ------------------------------------------------------------------*/
      list_pop_item(list_ptr, &(entry->link));
      list_push_front(list_ptr, &(entry->link));
    }
  }

  DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_get_auth_type:"
                          "returns auth_type: %d apn: %s",*auth_type_to_use_p,
                          (char *)apn);

} /*ds_3gpp_auth_mgr_get_auth_type() */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_CALL_SUCCESS

DESCRIPTION
  This function notifies the auth manager of a call success event.
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_3gpp_auth_mgr_call_success 
(
  byte                       *apn, 
  ds_umts_pdp_auth_enum_type  requested_auth_type,
  sys_modem_as_id_e_type      subs_id
)
{
  ds_3gpp_auth_mgr_entry_s * entry;

  /*-------------------------------------------------------------------------*/

  if( (ds_3gpp_nv_manager_get_auth_fallback(subs_id) == FALSE)
      || (requested_auth_type != DS_UMTS_PDP_AUTH_CHAP_OR_PAP))
  {
    /*------------------------------------------------------------------ 
      Not an auth fallback scenario, nothing to do
     ------------------------------------------------------------------*/
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_success:"
                            "Not an auth fallback scenario. APN: %s, "
                            "requested_auth_type: %d",
                            (char *)apn, requested_auth_type);

    return;
  }

  entry = ds_3gpp_auth_mgr_lookup_entry(apn, subs_id);
  if(entry == NULL)
  {
    DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_ERROR,"ds_3gpp_auth_mgr_call_success:"
                            " Error, Auth table entry missing on call success."
                            " Apn: %s, requested_auth_type: %d",
                            (char *)apn, requested_auth_type);

    return;
  }

 ds_3gpp_auth_mgr_update_entry(entry, TRUE);

  DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_success:"
                          " APN: %s, auth_type: %d",
                            (char *)apn, entry->auth_type);

} /* ds_3gpp_auth_mgr_call_success() */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_CALL_FAILURE

DESCRIPTION
  This function notifies the auth manager of a call failure event..
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_3gpp_auth_mgr_call_failure 
(
  byte                          *apn, 
  ds_umts_pdp_auth_enum_type    requested_auth_type, 
  ps_iface_net_down_reason_type cause_code,
  boolean                       call_was_connected,
  sys_modem_as_id_e_type        subs_id
)
{
  ds_3gpp_auth_mgr_entry_s *entry;
  list_type                *list_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_auth_mgr_get_list_ptr(subs_id);

  if (list_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  if( (ds_3gpp_nv_manager_get_auth_fallback(subs_id) == FALSE)
     || (requested_auth_type != DS_UMTS_PDP_AUTH_CHAP_OR_PAP)
     || (cause_code != PS_NET_DOWN_REASON_AUTH_FAILED
         && cause_code != PS_NET_DOWN_REASON_ACTIVATION_REJECT)
     || (call_was_connected))
  {
    /*------------------------------------------------------------------ 
      Not an auth fallback scenario, nothing to do
     ------------------------------------------------------------------*/
    DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_failure:"
                            " Not an auth fallback scenario. APN: %s, "
                            "requested_auth_type: %d, cause_code: %d",
                            (char *)apn, requested_auth_type, cause_code);
    return;
  }
 
  entry = ds_3gpp_auth_mgr_lookup_entry(apn, subs_id);
  if(entry == NULL)
  {
    DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_failure:"
                            " Error, Auth table entry.  missing on call failure."
                            " Apn: %s, requested_auth_type: %d,  cause_code: %d",
                            (char *)apn, requested_auth_type, cause_code);

    return;
  }
  
  if(entry->count == DS_3GPP_AUTH_MGR_MAX_RETRIES - 1) 
  {
    /*------------------------------------------------------------------ 
      This was the last unsuccessful try with an auth_type. We remove the
      entry. need_redial() will return false if the entry is not found.
     ------------------------------------------------------------------*/
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_failure:"
                            " Max retries exceeded.APN: %s", (char *)apn);

    list_pop_item(list_ptr, &(entry->link));
    DS_3GPP_MEM_FREE(entry);
    return;
  }

  /*------------------------------------------------------------------ 
    Update the entry to reflect the new auth failure
   ------------------------------------------------------------------*/
  ds_3gpp_auth_mgr_update_entry(entry, FALSE);

  DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"ds_3gpp_auth_mgr_call_failure:"
                          " APN: %s, auth_type: %d, count: %d", 
                          (char *)apn, entry->auth_type, entry->count);

} /* ds_3gpp_auth_mgr_call_failure() */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Subscription specific Auth Mgr Information
 
SIDE EFFECTS
  None.
===========================================================================*/
static ds_3gpp_auth_mgr_per_subs_info_type* 
  ds_3gpp_auth_mgr_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_auth_mgr_per_subs_info_type *hndl = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    hndl =  ds_3gpp_auth_mgr_info.per_subs_info_p[subs_id];
  }
  return hndl;
} /* ds_3gpp_auth_mgr_get_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS 
  per_subs_info_hndl: Handle to the Subscription specific Auth Mgr Information 
  subs_id:            Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  
 
SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_auth_mgr_set_per_subs_info_hndl
(
  ds_3gpp_auth_mgr_per_subs_info_type* hndl,
  sys_modem_as_id_e_type               subs_id
)
{
  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    ds_3gpp_auth_mgr_info.per_subs_info_p[subs_id] = hndl;
  }

  return;

} /* ds_3gpp_auth_mgr_set_per_subs_info_hndl */


/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates the handle to the Subscription Specific
  Auth Mgr Information. 
  
PARAMETERS
  hndl  : Handle to the Subscription specific
          Auth Mgr Information

DEPENDENCIES
  None.

RETURN VALUE
  TRUE,  if the handle is valid
  FALSE, if invalid

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_auth_mgr_validate_per_subs_info_hndl
(
  ds_3gpp_auth_mgr_per_subs_info_type *hndl
)
{
  boolean ret_val = FALSE;
  /*----------------------------------------------------------*/

  if (hndl != NULL && hndl->cookie == DS_3GPP_AUTH_MGR_PER_SUBS_INFO_COOKIE)
  {
    ret_val = TRUE;
  }
  
  return ret_val;
} /* ds_3gpp_auth_mgr_validate_per_subs_info_hndl */

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_LIST_PTR

DESCRIPTION
  This function fetches a pointer to the Auth mgr List
  
PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Pointe to the Auth Mgr List

SIDE EFFECTS
  None.
===========================================================================*/
static list_type* ds_3gpp_auth_mgr_get_list_ptr
(
  sys_modem_as_id_e_type subs_id
)
{
  list_type                           *list_ptr = NULL;
  ds_3gpp_auth_mgr_per_subs_info_type *hndl;
  /*--------------------------------------------------------------*/

  hndl = ds_3gpp_auth_mgr_get_per_subs_info_hndl(subs_id);

  if (ds_3gpp_auth_mgr_validate_per_subs_info_hndl(hndl))
  {
    list_ptr = &(hndl->list);
  }

  return list_ptr;
} /* ds_3gpp_auth_mgr_get_list_ptr */
