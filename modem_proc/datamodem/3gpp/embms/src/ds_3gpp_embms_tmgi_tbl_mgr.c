
/*==============================================================================

          e M B M S D A T A - S P E C I F I C   H A N D L E R 


                   D S _ 3 G P P _ E M B M S _ R R C I F . C
GENERAL DESCRIPTION
  This file contains the functions that implement the interface between
  3GPP eMBMS mode handler (eMBMS MH) and LTE RRC. The mode of communicatiob 
  between eMBMS MH and LTE RRC is through message router interface. eMBMS MH
  sends request messages to LTE RRC when it receives an IOCTL from the upper
  layers. eMBMS MH registers for messages from LTE RRC and handles them 
  accordingly. The messages received can be solicited or unsolicited.

EXTERNALIZED FUNCTIONS
  boolean ds_3gpp_embms_rrcif_is_iface_bring_up_allowed(void)


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/src/ds_3gpp_embms_tmgi_tbl_mgr.c#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/19/11    hs     Created file/Initial version.
==============================================================================*/

/*==============================================================================
                          INCLUDE FILES FOR MODULE
==============================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_EMBMS

#include "ds_3gpp_embms_tmgi_tbl_mgr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_embms_hdlr.h"
#include "ps_iface_embms.h"
#include "modem_mem.h"

/*==============================================================================
                            VARIABLES
==============================================================================*/

/*------------------------------------------------------------------------------
  eMBMS TMGI management table. This is a table of pointers. 
  The table has a maximum of 16 entries. LTE RRC limits the max number of 
  TMGIs that can be activated to LTE_EMBMS_MAX_ACTIVE_SESSIONS (8 as of now). 
  Assuming there are max connections and that all of them can replaced by
  a higher prio connections, we should be able to handle twice the number of
  entries as LTE_EMBMS_MAX_ACTIVE_SESSIONS.

  Whenever a connection is requested, memory is allocated for the table
  entry and the pointer in the entry is made to point to the block obtained.
------------------------------------------------------------------------------*/

typedef struct
{
  ds_3gpp_embms_context_info_type* 
    embms_context_table[DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE];
} ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type;

/*------------------------------------------------------------------------------ 
  Pointers to the EMBMS Context Table for each subscription
----------------------------------------------------------------------------*/
static ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type
         *ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX] = 
           {NULL};

/*==============================================================================
                            Forward declarations
==============================================================================*/

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_CNTXT_TBL

DESCRIPTION
  This function is used to fetch the EMBMS Context Table corresponding to the
  give subscription.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the EMBMS Context Table

SIDE EFFECTS  
  None
==============================================================================*/
static ds_3gpp_embms_context_info_type** ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_VALIDATE_CNTXT_TBL

DESCRIPTION
  This function is used to validate the EMBMS Context Table corresponding to the
  give subscription.

PARAMETERS
  Pointer to the EMBMS Context Table

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if valid
  FALSE, otherwise

SIDE EFFECTS  
  None
==============================================================================*/
static boolean ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl
(
  ds_3gpp_embms_context_info_type **cntxt_tbl_p
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function is used to get a handle to the subscription specific information
  of this module.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Subscription Specific information

SIDE EFFECTS  
  None
==============================================================================*/
static ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type* 
         ds_3gpp_embms_tmgi_tbl_mgr_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function is used to set the subscription specific information
  of this module.

PARAMETERS
  Handle to the Subscription Specific information
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS  
  None
==============================================================================*/
static void ds_3gpp_embms_tmgi_tbl_mgr_set_per_subs_info_hndl
(
  ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type   *hndl,
  sys_modem_as_id_e_type                          subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_MATCH_TMGI_SESSION_INFO

DESCRIPTION
  This function is used to match the TMGI session info between req and existing
  info.

  The same function can be reused to retrieve an entry from the context table
  on getting a response from LTE RRC. In this case of retrieval, the information
  in the context table is considered as the request and the information from RRC
  is considered as the current info. This is due to the way we match session id.

  An activate response from LTE RRC may bear session id when the request doesn't
  have it.

PARAMETERS
  incoming_req_ptr - incoming TMGI_Session request
  current_info_ptr - current TMGI_Session info

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there was a match
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL boolean ds_3gpp_embms_tmgi_tbl_mgr_match_tmgi_session_info
(
  ps_iface_embms_tmgi_type *incoming_req_ptr,
  ps_iface_embms_tmgi_type *current_info_ptr
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_INIT_CONTEXT_INFO

DESCRIPTION
  This function is used to initialize the context information blob that was
  allocated dynamically on activate_tmgi request.

  This initilializes the list to hold client.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  List init instantiates a list mutex. Before this memory is freed, we need to
  call list_destroy
==============================================================================*/
LOCAL void ds_3gpp_embms_tmgi_tbl_mgr_init_context_info
(
  ds_3gpp_embms_context_info_type *context_info_ptr
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_CLIENT_LIST_SEARCH_FN

DESCRIPTION
  This function is used to search the list of clients to see if a particular
  client is present. This is a call back function from list_linear_search.

PARAMETERS
  item_ptr    - pointer to an item in the list
  compare_val - value that we are interested in finding in the list

DEPENDENCIES
  None.

RETURN VALUE
  nonzero indicates a match - We will return 1
  Zero - no match

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_tmgi_mgr_client_list_search_fn
( 
  void *item_ptr, 
  void *compare_val 
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_RESET_CONTEXT_INFO

DESCRIPTION
  This function is used to reset the context information blob that was
  allocated dynamically on activate_tmgi request.

  This destroys the list to hold client.

DEPENDENCIES
  Should be called before the memory allocated from modem heap is returned.
  If this is not followed, there will be a libc heap exhaustion (due to mutexes
  created at list initilialization)

RETURN VALUE
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
LOCAL void ds_3gpp_embms_tmgi_tbl_mgr_reset_context_info
(
  ds_3gpp_embms_context_info_type *context_info_ptr
);

/*==============================================================================
                            Functions implemented in this module
==============================================================================*/

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_FIND_TMGI

DESCRIPTION
  This function is used to find if the TMGI table already has an entry for
  the TMGI.

  If there is a TMGI match a valid index (between 0 and 15) is returned

PARAMETERS
  IN
    TMGI - to check against all valid entries in table
    is_response_processing - is function called while processing response
    subs_id                - Subscription Id 

  OUT
    index - the entry in the table where the TMGI match occured

DEPENDENCIES
  The matching logic has to behave differently between trying to process a req
  from the app and trying to process a response from LTE RRC.

  While processing the request, the TMGI table is believed to have more updated
  info and while processing the response from LTE RRC, the response is expected
  to have more updated session info.

  It is for this reason the caller has to pass info about whether this function
  is being called while processing a request or a response.

RETURN VALUE
  True - if entry is present
  False otherwise

SIDE EFFECTS  
  None
===========================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
(
  ps_iface_embms_tmgi_type *tmgi_session_info_ptr,
  boolean                  is_response_processing,
  uint8                    *index_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  uint8                           loop_index;
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr;
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ps_iface_embms_tmgi_type        *incoming_req_ptr = NULL;
  ps_iface_embms_tmgi_type        *current_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(index_ptr != NULL);

  /*----------------------------------------------------------------------------
    Initialize the incoming out param for failure case
  ----------------------------------------------------------------------------*/
  *index_ptr = DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();
  
  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }


  for(loop_index = 0; 
      loop_index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; 
      loop_index++)
  {
    if(cntxt_tbl_p[loop_index] == NULL)
    {
      continue;
    }

    /*--------------------------------------------------------------------------
      If the entry has a non NULL value, it points to a TMGI context info blob
    --------------------------------------------------------------------------*/
    tmgi_context_info_ptr = cntxt_tbl_p[loop_index];

    if(is_response_processing == FALSE)
    {
      /*------------------------------------------------------------------------
        Processing request. Table info is believed to be more accurate.
      ------------------------------------------------------------------------*/
      incoming_req_ptr = tmgi_session_info_ptr;
      current_info_ptr = &(tmgi_context_info_ptr->tmgi_session_info);
    }
    else
    {
      /*------------------------------------------------------------------------
        Processing response. RRC info is believed to be more accurate.
        This may seem less intutive but is expected to reduce similar code being
        written in similar functions.
      ------------------------------------------------------------------------*/
      current_info_ptr = tmgi_session_info_ptr;
      incoming_req_ptr = &(tmgi_context_info_ptr->tmgi_session_info);
    }

    if(ds_3gpp_embms_tmgi_tbl_mgr_match_tmgi_session_info(incoming_req_ptr,
                                                      current_info_ptr) == TRUE)
    {
      /*------------------------------------------------------------------------
        TMGI Session info matched.
      ------------------------------------------------------------------------*/
      DS_EMBMS_MSG1_HIGH("eMBMS TMGI Table match. Index: %d",loop_index);
      DS_EMBMS_MSG2_MED("session_id_present: %d, session_id: %d",
                        tmgi_session_info_ptr->session_id_present,
                        tmgi_session_info_ptr->session_id);
      *index_ptr = loop_index;
      ret_val = TRUE;

    } /* TMGI_Session info match */
    
  } /* for loop to go through all table entries */

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_CLIENT_LIST_SEARCH_FN

DESCRIPTION
  This function is used to search the list of clients to see if a particular
  client is present. This is a call back function from list_linear_search.

PARAMETERS
  item_ptr    - pointer to an item in the list
  compare_val - value that we are interested in finding in the list

DEPENDENCIES
  None.

RETURN VALUE
  nonzero indicates a match - We will return 1
  Zero - no match

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_tmgi_mgr_client_list_search_fn
( 
  void *item_ptr, 
  void *compare_val 
)
{
  ds_3gpp_embms_client_info_type *client_info_ptr = NULL;
  int                            ret_val = 0;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  client_info_ptr = (ds_3gpp_embms_client_info_type*)item_ptr;

  if( (client_info_ptr != NULL) && 
      (client_info_ptr->user_data_ptr == compare_val) )
  {
    ret_val = 1;
  }

  return ret_val;

} /* ds_3gpp_embms_tmgi_mgr_client_list_search_fn */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_ADD_CLIENT_TO_LIST

DESCRIPTION
  This function is used to add a client to the list of clients that have 
  requested the activation of the same TMGI

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be added to the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_FAIL    - if client cannot be added
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_SUCCESS - if client was added
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_DUP     - if client in list

SIDE EFFECTS  
  None
==============================================================================*/
ds_3gpp_embms_tmgi_tbl_client_add_enum_type  
                                   ds_3gpp_embms_tmgi_tbl_mgr_add_client_to_list
(
  uint8                     index,
  void                      *client_id,
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_embms_tmgi_tbl_client_add_enum_type  ret_val = 
                                         DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_FAIL;
  ds_3gpp_embms_context_info_type             *context_info_ptr = NULL;
  ds_3gpp_embms_client_info_type              *client_info_ptr = NULL;
  rex_crit_sect_type                          *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type             **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    /*--------------------------------------------------------------------------
      Get the pointer from the TMGI table using the index passed into the fn.
    --------------------------------------------------------------------------*/
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Find out if the list contains the client already. If yes, set ret_val to
        TRUE. Nothing else needs to be done
      ------------------------------------------------------------------------*/
      if(list_linear_search(&(context_info_ptr->client_list),
           (list_compare_func_type)ds_3gpp_embms_tmgi_mgr_client_list_search_fn,
                            client_id) == NULL)
      {
        /*----------------------------------------------------------------------
          Client id is not present in the list
        ----------------------------------------------------------------------*/
        client_info_ptr = (ds_3gpp_embms_client_info_type*)
                         modem_mem_alloc(sizeof(ds_3gpp_embms_client_info_type),
                                         MODEM_MEM_CLIENT_DATA);

        if(client_info_ptr != NULL)
        {
          /*-------------------------------------------------------------------- 
            Store the client id into the list item
          --------------------------------------------------------------------*/
          client_info_ptr->user_data_ptr = client_id;

          /*--------------------------------------------------------------------
            Push the list item into the list
          --------------------------------------------------------------------*/
          list_push_front(&(context_info_ptr->client_list), 
                                                    &(client_info_ptr->link));

          /*--------------------------------------------------------------------
            Update the ref count in the table corresponding to this entry 
          --------------------------------------------------------------------*/
          context_info_ptr->ref_count += 1;

          /*--------------------------------------------------------------------
            Set the ret_val to SUCCESS
          --------------------------------------------------------------------*/
          ret_val = DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_SUCCESS;
        } /* Memory allocation was successful */
        else
        {
          DS_EMBMS_MSG0_ERROR("Could not allocate mem to add to client list");
        }
      }/* Client id is not present in list*/
      else
      {
        /*----------------------------------------------------------------------
          Set the ret_val to DUP since the client is already present in the
          list
        ----------------------------------------------------------------------*/
        DS_EMBMS_MSG2_HIGH("Client: 0x%x already present in index: %d of table",
                           client_id,index);

        ret_val = DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_DUP;
      } /* client present in list */
    } /* Non null context info pointer*/
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer at index: %d of context table is NULL",
                          index);
    }
  }/* Valid index into the TMGI table */
  else
  {
    DS_EMBMS_MSG1_ERROR("Invalid table index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_update_client_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_REMOVE_CLIENT_FROM_LIST

DESCRIPTION
  This function is used to remove a client from the list of clients that have 
  requested the activation of the same TMGI

  Note: This function decrements the ref_count in the TMGI context info.

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be removed from the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to remove the entry.
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_remove_client_from_list
(
  uint8                   index,
  void                   *client_id,
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type *context_info_ptr = NULL;
  ds_3gpp_embms_client_info_type  *client_info_ptr = NULL;
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    /*--------------------------------------------------------------------------
      Get the pointer from the TMGI table using the index passed into the fn.
    --------------------------------------------------------------------------*/
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Find out if the list contains the client already. If yes, remove the 
        client from the list
      ------------------------------------------------------------------------*/
      client_info_ptr = list_linear_search(&(context_info_ptr->client_list),
           (list_compare_func_type)ds_3gpp_embms_tmgi_mgr_client_list_search_fn,
                                           client_id);

      if(client_info_ptr != NULL)
      {                  
        /*----------------------------------------------------------------------
          Pop the list item from the list
        ----------------------------------------------------------------------*/
        list_pop_item(&(context_info_ptr->client_list), 
                                                      &(client_info_ptr->link));

        /*----------------------------------------------------------------------
          Update the ref count in the table corresponding to this entry 
        ----------------------------------------------------------------------*/
        context_info_ptr->ref_count -= 1;

        /*----------------------------------------------------------------------
          Free the modem heap memory used for client info
        ----------------------------------------------------------------------*/\
        modem_mem_free((void*)client_info_ptr, MODEM_MEM_CLIENT_DATA);

        /*----------------------------------------------------------------------
          Set the ret_val to TRUE
        ----------------------------------------------------------------------*/
        ret_val = TRUE;

      }/* Client id is present in list*/
      else
      {
        /*----------------------------------------------------------------------
          Client is not present in the list
        ----------------------------------------------------------------------*/
        DS_EMBMS_MSG2_ERROR("Client: 0x%x not present in the list at index: %d",
                            client_id, index);
      } /* client not present in list */
    } /* Non null context info pointer*/
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer at index: %d of context table is NULL",
                          index);
    }
  }/* Valid index into the TMGI table */
  else
  {
    DS_EMBMS_MSG1_ERROR("Invalid table index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_remove_client_from_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_ALLOCATE_ENTRY

DESCRIPTION
  This function is used to locate and unused entry in the TMGI context table and
  allocate memory needed for storing the TMGI context information.

  Note: List init will happen for the list of clients. The list should be 
        destroyed before freeing the memory allocated for this context. Freeing
        this context will happen when the last client deactivates TMGI.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to find free index and allocate memory
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_allocate_entry
(
  ps_iface_embms_tmgi_type *tmgi_session_info_ptr,
  void                     *client_id,
  uint8                    *index_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  uint8                           loop_index;
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr;
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_client_info_type  *client_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Initialize the incoming out param for failure case
  ----------------------------------------------------------------------------*/
  *index_ptr = DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return FALSE;
  }

  for(loop_index = 0; 
      loop_index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; 
      loop_index++)
  {
    tmgi_context_info_ptr = cntxt_tbl_p[loop_index];
    if(tmgi_context_info_ptr == NULL)
    {
      break;
    }
  } /* loop over all entries in table to locate a free one */

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(loop_index))
  {
    /*--------------------------------------------------------------------------
      Got a free index. Allocate memory from modem heap

      Requesting memory only using non critical client id. If failures are seen
      often while requesting memory consider using the Data_Critical client id.
    --------------------------------------------------------------------------*/
    tmgi_context_info_ptr = (ds_3gpp_embms_context_info_type*)
                        modem_mem_alloc(sizeof(ds_3gpp_embms_context_info_type),
                                        MODEM_MEM_CLIENT_DATA);

    client_info_ptr = (ds_3gpp_embms_client_info_type*)
                         modem_mem_alloc(sizeof(ds_3gpp_embms_client_info_type),
                                         MODEM_MEM_CLIENT_DATA);

    if((tmgi_context_info_ptr == NULL) || (client_info_ptr == NULL))
    {
      /*------------------------------------------------------------------------
        Modem heap mem allocation failed
      ------------------------------------------------------------------------*/
      DS_EMBMS_MSG0_HIGH("eMBMS TMGI Table: Heap allocation Failure");

      /*------------------------------------------------------------------------
        Free up the memory whose allocation did not fail
      ------------------------------------------------------------------------*/
      if(tmgi_context_info_ptr != NULL)
      {
        modem_mem_free((void*)tmgi_context_info_ptr, MODEM_MEM_CLIENT_DATA);
      }

      if(client_info_ptr != NULL)
      {
        modem_mem_free((void*)client_info_ptr, MODEM_MEM_CLIENT_DATA);
      }
    }
    else
    {
      /*------------------------------------------------------------------------
        Store the pointer to the memory allocated in the index that was selected
        for use
      ------------------------------------------------------------------------*/
      cntxt_tbl_p[loop_index] = tmgi_context_info_ptr;

      ds_3gpp_embms_tmgi_tbl_mgr_init_context_info(tmgi_context_info_ptr);

      /*------------------------------------------------------------------------
        Copy the TMGI_SESSION ID information into the memory allocated.
        Copy the session id only if the session id is valid in the incoming
        request
      ------------------------------------------------------------------------*/
      memscpy((void*)&(tmgi_context_info_ptr->tmgi_session_info.tmgi),
              PS_IFACE_EMBMS_TMGI_SIZE,
             (void*)&(tmgi_session_info_ptr->tmgi), PS_IFACE_EMBMS_TMGI_SIZE);

      if(tmgi_session_info_ptr->session_id_present == TRUE)
      {
        tmgi_context_info_ptr->tmgi_session_info.session_id_present = TRUE;
        tmgi_context_info_ptr->tmgi_session_info.session_id = 
                                              tmgi_session_info_ptr->session_id;
      }

      /*------------------------------------------------------------------------
        Store the client info that was passed into this function. The info is
        stored in the queue element. 
        Also set the next_ptr to NULL. This will anyway be overwritten by 
        list_push_front
      ------------------------------------------------------------------------*/
      client_info_ptr->user_data_ptr = client_id;
      client_info_ptr->link.next_ptr = NULL;

      /*------------------------------------------------------------------------
        Store the client id in the list
      ------------------------------------------------------------------------*/
      list_push_front(&(tmgi_context_info_ptr->client_list),
                      &(client_info_ptr->link));

      /*------------------------------------------------------------------------
        Increment the ref count.
      ------------------------------------------------------------------------*/
      tmgi_context_info_ptr->ref_count += 1;

      *index_ptr = loop_index;
      ret_val = TRUE;
    } /* Heap allocation successful */
  }
  else
  {
    DS_EMBMS_MSG0_HIGH("eMBMS TMGI Table: Allocation - can't get table entry");
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_allocate_entry */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_INIT_CONTEXT_INFO

DESCRIPTION
  This function is used to initialize the context information blob that was
  allocated dynamically on activate_tmgi request.

  This initilializes the list to hold client.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  List init instantiates a list mutex. Before this memory is freed, we need to
  call list_destroy
==============================================================================*/
LOCAL void ds_3gpp_embms_tmgi_tbl_mgr_init_context_info
(
  ds_3gpp_embms_context_info_type *context_info_ptr
)
{
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Initialize RB id to an invalid value
  ----------------------------------------------------------------------------*/
  context_info_ptr->rb_id = 0xFF;

  /*----------------------------------------------------------------------------
    Initialize Ref count to zero. Ref count is used to figure out the number of
    clients present for the TMGI. This should be the same as the list_size
  ----------------------------------------------------------------------------*/
  context_info_ptr->ref_count = 0;

  /*----------------------------------------------------------------------------
    Initialize the state of the TMGI to inactive.
  ----------------------------------------------------------------------------*/
  context_info_ptr->state = DS_3GPP_EMBMS_TMGI_STATE_INACTIVE;

  /*----------------------------------------------------------------------------
    Memset the TMGI_SESSION info blob to zero. This will be updated by the 
    caller of intialization function.
  ----------------------------------------------------------------------------*/
  memset((void*)&(context_info_ptr->tmgi_session_info), 0, 
                                   sizeof(context_info_ptr->tmgi_session_info));

  /*----------------------------------------------------------------------------
    Explicitly set the session ID to invalid
  ----------------------------------------------------------------------------*/
  context_info_ptr->tmgi_session_info.session_id = 
                                              PS_IFACE_EMBMS_INVALID_SESSION_ID;

  /*----------------------------------------------------------------------------
    Initialize the list to hold clients that have activated this TMGI
  ----------------------------------------------------------------------------*/
  list_init(&(context_info_ptr->client_list));
} /* ds_3gpp_embms_tmgi_tbl_mgr_init_context_info */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_RESET_CONTEXT_INFO

DESCRIPTION
  This function is used to reset the context information blob that was
  allocated dynamically on activate_tmgi request.

  This destroys the list to hold client.

DEPENDENCIES
  Should be called before the memory allocated from modem heap is returned.
  If this is not followed, there will be a libc heap exhaustion (due to mutexes
  created at list initilialization)

RETURN VALUE
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
LOCAL void ds_3gpp_embms_tmgi_tbl_mgr_reset_context_info
(
  ds_3gpp_embms_context_info_type *context_info_ptr
)
{
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Initialize RB id to an invalid value
  ----------------------------------------------------------------------------*/
  context_info_ptr->rb_id = 0xFF;

  /*----------------------------------------------------------------------------
    Initialize Ref count to zero. Ref count is used to figure out the number of
    clients present for the TMGI. This should be the same as the list_size
  ----------------------------------------------------------------------------*/
  context_info_ptr->ref_count = 0;

  /*----------------------------------------------------------------------------
    Initialize the state of the TMGI to inactive.
  ----------------------------------------------------------------------------*/
  context_info_ptr->state = DS_3GPP_EMBMS_TMGI_STATE_INACTIVE;

  /*----------------------------------------------------------------------------
    Memset the TMGI_SESSION info blob to zero. This will be updated by the 
    caller of intialization function.
  ----------------------------------------------------------------------------*/
  memset((void*)&(context_info_ptr->tmgi_session_info), 0, 
                                   sizeof(context_info_ptr->tmgi_session_info));

  /*----------------------------------------------------------------------------
    Explicitly set the session ID to invalid
  ----------------------------------------------------------------------------*/
  context_info_ptr->tmgi_session_info.session_id = 
                                              PS_IFACE_EMBMS_INVALID_SESSION_ID;

  /*----------------------------------------------------------------------------
    Destroy the list to free the mutex that was created at the list init.
  ----------------------------------------------------------------------------*/
  list_destroy(&(context_info_ptr->client_list));
} /* ds_3gpp_embms_tmgi_tbl_mgr_init_context_info */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_SET_STATE

DESCRIPTION
  This function is used to set a state for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to set state
  tmgi_state - The state to set to.
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to set state
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_set_state
(
  uint8                      index,
  ds_3gpp_embms_tmgi_state_e tmgi_state,
  sys_modem_as_id_e_type     subs_id
)
{
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr = NULL;
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  tmgi_context_info_ptr = cntxt_tbl_p[index];

  if(tmgi_context_info_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Set the state
    --------------------------------------------------------------------------*/
    tmgi_context_info_ptr->state = tmgi_state;
    ret_val = TRUE;
  } /* TMGI context info pointer non-NULL */
  else
  {
    DS_EMBMS_MSG1_ERROR("Could not find table entry at index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;

} /* ds_3gpp_embms_tmgi_tbl_mgr_set_state */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_REF_COUNT

DESCRIPTION
  This function is used to get ref_count for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to get ref_count from
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - the number of clients that have activated the TMGI
  0 - indicates failure

SIDE EFFECTS  
  None
==============================================================================*/
uint8 ds_3gpp_embms_tmgi_tbl_mgr_get_ref_count
(
  uint8                      index,
  sys_modem_as_id_e_type     subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  uint8                           ret_val = 0;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  tmgi_context_info_ptr = cntxt_tbl_p[index];

  if(tmgi_context_info_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Get the ref_count in the return value
    --------------------------------------------------------------------------*/
    ret_val = tmgi_context_info_ptr->ref_count;
  } /* TMGI context info pointer non-NULL */
  else
  {
    DS_EMBMS_MSG1_ERROR("Could not find table entry at index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_get_ref_count */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_STATE

DESCRIPTION
  This function is used to get state for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to get state from
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_embms_tmgi_state_e - state of the TMGI

SIDE EFFECTS  
  None
==============================================================================*/
ds_3gpp_embms_tmgi_state_e ds_3gpp_embms_tmgi_tbl_mgr_get_state
(
  uint8                      index,
  sys_modem_as_id_e_type     subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_tmgi_state_e      ret_val = DS_3GPP_EMBMS_TMGI_STATE_INVALID;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  tmgi_context_info_ptr = cntxt_tbl_p[index];

  if(tmgi_context_info_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Get the state in the return value
    --------------------------------------------------------------------------*/
    ret_val = tmgi_context_info_ptr->state;
  } /* TMGI context info pointer non-NULL */
  else
  {
    DS_EMBMS_MSG1_ERROR("Could not find table entry at index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_get_state */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_MATCH_TMGI_SESSION_INFO

DESCRIPTION
  This function is used to match the TMGI session info between req and existing
  info.

  The same function can be reused to retrieve an entry from the context table
  on getting a response from LTE RRC. In this case of retrieval, the information
  in the context table is considered as the request and the information from RRC
  is considered as the current info. This is due to the way we match session id.

  An activate response from LTE RRC may bear session id when the request doesn't
  have it.

PARAMETERS
  incoming_req_ptr - incoming TMGI_Session request
  current_info_ptr - current TMGI_Session info

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there was a match
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL boolean ds_3gpp_embms_tmgi_tbl_mgr_match_tmgi_session_info
(
  ps_iface_embms_tmgi_type *incoming_req_ptr,
  ps_iface_embms_tmgi_type *current_info_ptr
)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(incoming_req_ptr!= NULL && current_info_ptr != NULL);

  if(memcmp((void*)current_info_ptr->tmgi,
            (void*)incoming_req_ptr->tmgi,
            PS_IFACE_EMBMS_TMGI_SIZE) == 0)
  {
    /*------------------------------------------------------------------------
      TMGI match happened. Now check the session id fields of the incoming req
      and the table. Logic to be used is as follows:

      Case 1: Incoming request has no session ID - Select entry irrespective 
              of whether session id in current_info is available or not.

      Case 2: Incoming request specifies session ID - Match only if session id
              matches in current_info. If current_info has no session id, do not
              select this entry as a match. This may end up sending new request 
              to RRC. A new table entry will be added. 
              If RRC returns saying TMGI is already active, we should merge 
              these two table entries.
    ------------------------------------------------------------------------*/
    if( 
       (incoming_req_ptr->session_id_present == FALSE) || 
       (
        (incoming_req_ptr->session_id_present == TRUE) && 
        (current_info_ptr->session_id_present == TRUE) &&
        (incoming_req_ptr->session_id == current_info_ptr->session_id)
       ) 
      )
    {
      ret_val = TRUE;
    } /* Session matched*/
  } /* TMGI matched */
  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_match_tmgi_session_info */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_CLIENT_IN_LIST

DESCRIPTION
  This function is used to find if a client id is present in the list of clients
  that have requested the activation of a TMGI

  This function does not do any changes to the client list

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be found in the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to find the entry.
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_is_client_in_list
(
  uint8                   index,
  void                   *client_id,
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type *context_info_ptr = NULL;
  ds_3gpp_embms_client_info_type  *client_info_ptr = NULL;
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    /*--------------------------------------------------------------------------
      Get the pointer from the TMGI table using the index passed into the fn.
    --------------------------------------------------------------------------*/
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Find out if the list contains the client already. If yes, remove the 
        client from the list
      ------------------------------------------------------------------------*/
      client_info_ptr = list_linear_search(&(context_info_ptr->client_list),
           (list_compare_func_type)ds_3gpp_embms_tmgi_mgr_client_list_search_fn,
                                           client_id);

      if(client_info_ptr != NULL)
      {                  
        DS_EMBMS_MSG2_HIGH("Client: 0x%x present in the list at index: %d",
                            client_id, index);
        /*----------------------------------------------------------------------
          Found the client id. Set the ret_val to TRUE
        ----------------------------------------------------------------------*/
        ret_val = TRUE;

      }/* Client id is present in list*/
      else
      {
        /*----------------------------------------------------------------------
          Client is not present in the list
        ----------------------------------------------------------------------*/
        DS_EMBMS_MSG2_ERROR("Client: 0x%x not present in the list at index: %d",
                            client_id, index);
      } /* client not present in list */
    } /* Non null context info pointer*/
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer at index: %d of context table is NULL",
                          index);
    }
  }/* Valid index into the TMGI table */
  else
  {
    DS_EMBMS_MSG1_ERROR("Invalid table index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_is_client_in_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_REMOVE_ENTRY

DESCRIPTION
  This function is used to remove an entry from the ds_3gpp_embms_context_table.
  This function deallocates client list memory, destroys the client list and
  frees up the memory that was allocated for the entry at the index specified.

PARAMETERS
  index     - the index into the ds_3gpp_embms_context_table
  subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_remove_entry
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type *context_info_ptr = NULL;
  list_type                       *list_ptr = NULL;
  ds_3gpp_embms_client_info_type  *client_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return;
  }

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Get the pointer to the list of clients
      ------------------------------------------------------------------------*/
      list_ptr = &(context_info_ptr->client_list);

      if(list_ptr != NULL)
      {
        /*----------------------------------------------------------------------
          Deallocate memory that was allocated to store the clients
        ----------------------------------------------------------------------*/
        client_info_ptr = (ds_3gpp_embms_client_info_type*)
                                                       list_pop_front(list_ptr);

        while(client_info_ptr != NULL)
        {
          modem_mem_free((void*)client_info_ptr, MODEM_MEM_CLIENT_DATA);

          client_info_ptr = (ds_3gpp_embms_client_info_type*)
                                                       list_pop_front(list_ptr);
        }/* loop over all clients */

        /*----------------------------------------------------------------------
          Call list destroy API so that we delete the list critical section
        ----------------------------------------------------------------------*/
        list_destroy(list_ptr);
      }
      else
      {
        DS_EMBMS_MSG1_ERROR("List Pointer at index:%d NULL. Skip list clean up",
                           index);
      }

      /*------------------------------------------------------------------------
        Free the memory that was got for this entry in the table.
      ------------------------------------------------------------------------*/
      modem_mem_free((void*)context_info_ptr,MODEM_MEM_CLIENT_DATA);

      /*------------------------------------------------------------------------
        Reset the pointer value in the table
      ------------------------------------------------------------------------*/
      cntxt_tbl_p[index] = NULL;
    } /* pointer at index non NULL*/
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer at index: %d is NULL. Skip clean up",
                         index);
    } /* pointer at index is NULL */
  } /* Valid index into table*/
  else
  {
    DS_EMBMS_MSG1_ERROR("Remove TMGI tbl entry. Invalid index:%d Skip clean up",
                       index);
  } /* Invalid index into the table */

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
} /* ds_3gpp_embms_tmgi_tbl_mgr_remove_entry */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_STORE_MRB_ID

DESCRIPTION
  This function is used to store the Multicast Radio Bearer (m-rb) id in the 
  TMGI table. Currently this m-RB is not used for any purpose. But if the WM
  at lower layers are going to be different for different radio bearers, we will
  need this for registration with lower layers.

PARAMETERS
    index     - the index into the ds_3gpp_embms_context_table
    mrb_id    - the radio bearer id for this TMGI
    subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_store_mrb_id
(
  uint8                  index,
  uint8                  mrb_id,
  sys_modem_as_id_e_type subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type *context_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return;
  }

  if(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    /*--------------------------------------------------------------------------
      Get the pointer from the TMGI table using the index passed into the fn.
    --------------------------------------------------------------------------*/
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Update the MRB Id
      ------------------------------------------------------------------------*/
      context_info_ptr->rb_id = mrb_id;
    }
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer NULL at TMGI TBL index: %d. Skip update MRB",
                         index);
    }
  } /* Valid index into table */
  else
  {
    DS_EMBMS_MSG1_ERROR("Invalid index into table. Index: %d. Skip update MRB",
                       index);
  } /* Invalid index into table */

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

} /* ds_3gpp_embms_tmgi_tbl_mgr_store_mrb_id */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_STORE_SESSION_ID

DESCRIPTION
  This function is used to store the session id returned by LTE RRC. The update
  to our table will happen only if the table currently has no session id info
  in it. Otherwise the update will be ignored. On a similar note, we will not
  set the session id to be 0xFF (invalid session id).

PARAMETERS
    index      - the index into the ds_3gpp_embms_context_table
    session_id - the session id that was activated
    subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_store_session_id
(
  uint8                  index,
  uint8                  session_id,
  sys_modem_as_id_e_type subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type *context_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return;
  }

  if( DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index) && 
      (session_id != PS_IFACE_EMBMS_INVALID_SESSION_ID) )
  {
    /*--------------------------------------------------------------------------
      Get the pointer from the TMGI table using the index passed into the fn.
    --------------------------------------------------------------------------*/
    context_info_ptr = cntxt_tbl_p[index];

    if(context_info_ptr != NULL)
    {
      /*------------------------------------------------------------------------
        Check if the table already has session info
      ------------------------------------------------------------------------*/
      if(context_info_ptr->tmgi_session_info.session_id_present == FALSE)
      {
        /*----------------------------------------------------------------------
          Update the table with info from lower layers
        ----------------------------------------------------------------------*/
        context_info_ptr->tmgi_session_info.session_id_present = TRUE;
        context_info_ptr->tmgi_session_info.session_id = session_id;

        DS_EMBMS_MSG2_HIGH("Session info at index: %d updated. Session_id: %d",
                           index,
                           context_info_ptr->tmgi_session_info.session_id);
      }
      else
      {
        DS_EMBMS_MSG2_HIGH("TMGI at index: %d already has session: %d",
                           index,
                           context_info_ptr->tmgi_session_info.session_id);
      }
    }
    else
    {
      DS_EMBMS_MSG1_ERROR("Pointer NULL at TMGI TBL index: %d. Skip update",
                         index);
    }
  } /* Valid index into table and valid session id*/
  else
  {
    DS_EMBMS_MSG2_ERROR("Invalid index or session id. Index: %d. Session_id %d",
                       index, session_id);
  } /* Invalid index into table or invalid session id*/

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

} /* ds_3gpp_embms_tmgi_tbl_mgr_store_session_id */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_ANY_TMGI_ACTIVE

DESCRIPTION
  This function is used to figure out if there are any TMGIs present in the
  TMGI table.

PARAMETERS
    subs_id  - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if there are TMGIs in the table
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_is_any_tmgi_active
(
  sys_modem_as_id_e_type subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  uint8                           index = 0;
  boolean                         ret_val = FALSE;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  for(index = 0; index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; index++)
  {
    if(cntxt_tbl_p[index] != NULL)
    {
      ret_val = TRUE;
      break;
    }
  }
  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_is_any_tmgi_active */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_ANY_TMGI_ACTIVE

DESCRIPTION
  This function is used to figure out if there are any TMGIs present in the
  TMGI table.

PARAMETERS
    subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if there are TMGIs in the table
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_init_context_table
( 
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type *hndl = NULL;
  int                                            i = 0; 
  /*---------------------------------------------------------------------------*/

  hndl = ds_3gpp_embms_tmgi_tbl_mgr_get_per_subs_info_hndl(subs_id);

  do
  {
    if (hndl != NULL)
    {
      DS_EMBMS_MSG0_ERROR("EMBMS Context Tbl Ptr is already allocated");
      break;
    }
    else
    {
      hndl = 
       (ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type *)
        modem_mem_alloc (sizeof(ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type),
        MODEM_MEM_CLIENT_DATA);

      if (hndl == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs Id (CM): %d", 
                            subs_id );
        break;
      }

      /*----------------------------------------------------------------------------
        Initialize TMGI table. This is very simple here as all we have to do
        is to memset the table of pointers.
      ----------------------------------------------------------------------------*/

      for (i=0; i<DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; i++ )
      {
        hndl->embms_context_table[i] = NULL;
      }

      ds_3gpp_embms_tmgi_tbl_mgr_set_per_subs_info_hndl(hndl, subs_id);
    }
  } while (0); 

  return;

} /* ds_3gpp_embms_tmgi_tbl_mgr_init_context_table */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_LIST

DESCRIPTION
  This function is used to fetch the list of clients.

PARAMETERS
  index       - index into the table
  subs_id     - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the list if there are no errors
  NULL otherwise

SIDE EFFECTS  
  None
==============================================================================*/
list_type* ds_3gpp_embms_tmgi_tbl_mgr_get_list
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
)
{
  rex_crit_sect_type              *crit_section_ptr = NULL;
  list_type                       *ret_val = NULL;
  ds_3gpp_embms_context_info_type *tmgi_context_info_ptr = NULL;
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  tmgi_context_info_ptr = cntxt_tbl_p[index];

  if(tmgi_context_info_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Get the ref_count in the return value
    --------------------------------------------------------------------------*/
    ret_val = &(tmgi_context_info_ptr->client_list);
  } /* TMGI context info pointer non-NULL */
  else
  {
    DS_EMBMS_MSG1_ERROR("Could not find table entry at index: %d", index);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_get_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_FILL_ACT_TMGI_LIST

DESCRIPTION
  This function is used to populate the active TMGI list into a container sent
  into this function.

PARAMETERS
  IN
    subs_id               - Subscription Id
  OUT
    act_tmgi_list_ind_ptr - pointer to the container

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if everything was successful
  FALSE otherwise

SIDE EFFECTS  
  Allocates memory for the TMGI list to be sent. Caller of this function
  has to free the memory after sending the indication.
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_fill_act_tmgi_list
(
  ps_iface_embms_tmgi_list_info_type* act_tmgi_list_ind_ptr,
  sys_modem_as_id_e_type              subs_id
)
{
  int                                    loop_index;
  int                                    num_tmgi = 0;
  boolean                                ret_val = FALSE;
  rex_crit_sect_type                     *crit_section_ptr = NULL;
  ds_3gpp_embms_context_info_type        **cntxt_tbl_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
 
  ASSERT(act_tmgi_list_ind_ptr != NULL);
  
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  cntxt_tbl_p = ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl(subs_id);

  if (!ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl(cntxt_tbl_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
    return ret_val;
  }

  for(loop_index = 0; 
      loop_index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; 
      loop_index++)
  {
    if(cntxt_tbl_p[loop_index] == NULL)
    {
      continue;
    }

    if(ds_3gpp_embms_tmgi_tbl_mgr_get_state(loop_index, subs_id) == 
                                              DS_3GPP_EMBMS_TMGI_STATE_ACTIVE)
    {
      num_tmgi++;
    }
  } /* loop over all entries of TMGI table */

  DS_EMBMS_MSG1_HIGH("FILL ACT TMGI LIST: Number of Active TMGIs: %d", 
                      num_tmgi);

  act_tmgi_list_ind_ptr->tmgi_list.num_tmgi = num_tmgi;
  act_tmgi_list_ind_ptr->info_code = IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID;
  /*----------------------------------------------------------------------------
    Allocate memory needed for the list of TMGIs in the indication
  ----------------------------------------------------------------------------*/
  if(num_tmgi > 0)
  {
    act_tmgi_list_ind_ptr->tmgi_list.list = 
      (ps_iface_embms_tmgi_type*)modem_mem_alloc(
                                      sizeof(ps_iface_embms_tmgi_type)*num_tmgi,
                                      MODEM_MEM_CLIENT_DATA);

    if(act_tmgi_list_ind_ptr->tmgi_list.list != NULL)
    {
      /*------------------------------------------------------------------------
        Reset num_tmgi so that we can use it as the index into the blob of mem
        that we allocated
      ------------------------------------------------------------------------*/
      num_tmgi = 0;

      /*------------------------------------------------------------------------
        Loop over the TMGI table again and get the TMGI list
      ------------------------------------------------------------------------*/
      for(loop_index = 0; 
          loop_index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE; 
          loop_index++)
      {
        if(cntxt_tbl_p[loop_index] == NULL)
        {
          continue;
        }

        if(ds_3gpp_embms_tmgi_tbl_mgr_get_state(loop_index, subs_id) == 
                                               DS_3GPP_EMBMS_TMGI_STATE_ACTIVE)
        {
          memscpy((void*)(act_tmgi_list_ind_ptr->tmgi_list.list+num_tmgi),
		      sizeof(ps_iface_embms_tmgi_type),
                  (void*)&(cntxt_tbl_p[loop_index]->tmgi_session_info),
                   sizeof(ps_iface_embms_tmgi_type));
          num_tmgi++;
        }
      } /* loop over all entries of TMGI table */

      ASSERT(num_tmgi == act_tmgi_list_ind_ptr->tmgi_list.num_tmgi);

      /*------------------------------------------------------------------------
        Change the retval to TRUE
      ------------------------------------------------------------------------*/
      ret_val = TRUE;
    } // successful mem allocation
    else
    {
      DS_EMBMS_MSG0_ERROR("Could not allocate mem needed for list ind");
    }
  } // num act tmgi non zer
  else
  {
    ret_val = TRUE;
  }// num act tmgi is 0

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_fill_act_tmgi_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_CNTXT_TBL

DESCRIPTION
  This function is used to fetch the EMBMS Context Table corresponding to the
  give subscription.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the EMBMS Context Table

SIDE EFFECTS  
  None
==============================================================================*/
static ds_3gpp_embms_context_info_type** 
         ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_context_info_type **cntxt_tbl_p = NULL;
  ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type *per_subs_info_p = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    per_subs_info_p =
      ds_3gpp_embms_tmgi_tbl_mgr_get_per_subs_info_hndl(subs_id);

    if (per_subs_info_p != NULL)
    {
      cntxt_tbl_p = per_subs_info_p->embms_context_table;
    }
  }

  return cntxt_tbl_p;
} /* ds_3gpp_embms_tmgi_tbl_mgr_get_cntxt_tbl */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_VALIDATE_CNTXT_TBL

DESCRIPTION
  This function is used to validate the EMBMS Context Table corresponding to the
  give subscription.

PARAMETERS
  Pointer to the EMBMS Context Table

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if valid
  FALSE, otherwise

SIDE EFFECTS  
  None
==============================================================================*/
static boolean ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl
(
  ds_3gpp_embms_context_info_type **cntxt_tbl_p
)
{
  boolean ret_val = FALSE;

  if (cntxt_tbl_p != NULL)
  {
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("EMBMS CONTEXT TBL is INVALID");
  }

  return ret_val;
} /* ds_3gpp_embms_tmgi_tbl_mgr_validate_cntxt_tbl */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function is used to get a handle to the subscription specific information
  of this module.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Subscription Specific information

SIDE EFFECTS  
  None
==============================================================================*/
static ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type* 
         ds_3gpp_embms_tmgi_tbl_mgr_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type *hndl = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    hndl = ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_p[subs_id];
  }

  return hndl;
} /* ds_3gpp_embms_tmgi_tbl_mgr_get_per_subs_info_hndl */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function is used to set the subscription specific information
  of this module.

PARAMETERS
  Handle to the Subscription Specific information
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS  
  None
==============================================================================*/
static void ds_3gpp_embms_tmgi_tbl_mgr_set_per_subs_info_hndl
(
  ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_type   *hndl,
  sys_modem_as_id_e_type                          subs_id
)
{
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    ds_3gpp_embms_tmgi_tbl_mgr_per_subs_info_p[subs_id] = hndl;
  }

  return;
} /* ds_3gpp_embms_tmgi_tbl_mgr_set_per_subs_info_hndl */

#endif /* FEATURE_DATA_EMBMS */
