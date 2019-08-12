/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


 G E N E R I C   P H O N E B O O K  C A C H I N G   R E L A T E D   F I L E S


GENERAL DESCRIPTION

  This source file contains the PB caching supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.1.1/mmgsdi/src/mmgsdi_pb_cache.c

when       who     what, where, why
--------   ---     ----------------------------------------------------------\
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/10/16   tkl     F3 log prints cleanup
05/10/16   sp      Delete PB cache for the file that is being written currently
05/29/15   nr      Fixing memoory leak during card powerdown and task stop
05/18/15   nr      Fix to distiguish reads on different apps in single slot
04/28/15   nr      Avoid accessing freed globals after session de-activation
02/19/15   nr      Phonebook record read optimization

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdisessionlib.h"
#include "mmgsdi_pb_cache.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdicache.h"
#include "mmgsdi_file.h"
#include "mmgsdi_nv.h"

/* MMGSDI Globals to store the PB cache data */
mmgsdi_pb_cache_type *mmgsdi_pb_cache_tbl_ptr[MMGSDI_MAX_NUM_SLOTS];


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_READ_FILE_NODE

DESCRIPTION
  This function is to read PB cache record file node from the cache list.

DEPENDENCIES
  None

RETURNS
  mmgsdi_pb_cache_element_type*

SIDE EFFECTS
  None

===========================================================================*/
static mmgsdi_pb_cache_element_type* mmgsdi_pb_cache_read_file_node(
  mmgsdi_path_type                    path_type,
  mmgsdi_pb_cache_type               *mmgsdi_pb_cache_table_ptr,
  mmgsdi_session_type_enum_type       session_type
)
{
  mmgsdi_pb_cache_element_type    *pb_cache_element_ptr = NULL;
  mmgsdi_pb_cache_element_type    *mmgsdi_file_node_ptr = NULL;

  if(mmgsdi_pb_cache_table_ptr == NULL || mmgsdi_pb_cache_table_ptr->item_ptr == NULL)
  {
    /* PB file list is empty */
    return NULL;
  }

  pb_cache_element_ptr = mmgsdi_pb_cache_table_ptr->item_ptr;
  while(pb_cache_element_ptr != NULL)
  {
    if(mmgsdi_file_compare_path_type(pb_cache_element_ptr->file_path, path_type) &&
       pb_cache_element_ptr->session_type == session_type)
    {
      /* Match found */
      mmgsdi_file_node_ptr = pb_cache_element_ptr;
      break;
    }

    pb_cache_element_ptr = pb_cache_element_ptr->next_ptr;
  }

  return mmgsdi_file_node_ptr;
}/* mmgsdi_pb_cache_read_file_node */


/*===========================================================================
FUNCTION MMGSDI_PB_SEND_SEARCH_REQ

DESCRIPTION
  This function sends the SEARCH RECORD request to UIM in a sync fashion
  and update the search response data in PB cache file node.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_pb_send_search_req(
  mmgsdi_slot_id_enum_type         slot_id,
  mmgsdi_path_type                 path_type,
  mmgsdi_len_type                  rec_len,
  mmgsdi_rec_num_type              num_of_rec,
  mmgsdi_session_type_enum_type    session_type
)
{
  mmgsdi_return_enum_type        mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_protocol_enum_type      protocol               = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_search_req_type        *search_req_ptr         = NULL;
  mmgsdi_client_id_type          client_id              = 0;
  mmgsdi_search_cnf_type        *search_cnf_ptr         = NULL;
  mmgsdi_pb_cache_element_type  *mmgsdi_file_node_ptr   = NULL;
  uint8                          slot_index             = MMGSDI_SLOT_1_INDEX;
  mmgsdi_session_id_type         session_id             = 0;
  mmgsdi_nv_context_type         nv_context             = MMGSDI_NV_CONTEXT_INVALID;

  /* Validating the slot id */
  mmgsdi_util_is_slot_valid(slot_id);

  if(rec_len <= 0 || num_of_rec <= 0)
  {
    /* Attributes data not present */
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(session_type, &session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read pb file node from cache list */
  mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(path_type,
                                                        mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                        session_type);
  if(mmgsdi_file_node_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_file_node_ptr->in_use_recs_ptr,
                                     num_of_rec * sizeof(boolean));
  if (mmgsdi_file_node_ptr->in_use_recs_ptr == NULL)
  {
    /* Memory allocation failed */
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Mark all records as valid by default */
  memset(mmgsdi_file_node_ptr->in_use_recs_ptr, TRUE,
         (uint32)(num_of_rec) * sizeof(boolean));

   nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  /* Skip SEARCH if feature is enabled */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SKIP_SEARCH_FOR_PB_FILES, nv_context) == MMGSDI_FEATURE_ENABLED)
  {
    return MMGSDI_SUCCESS;
  }

  /* Check if UICC or ICC, PB cache is supported only for UICC */
  protocol = mmgsdi_util_determine_protocol(slot_id);

  if(protocol != MMGSDI_UICC)
  {
    return MMGSDI_ERROR;
  }

  /* Issue search for UICC only */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr,
                                     sizeof(mmgsdi_search_req_type));

  /* memory allocation failed for search req ptr */
  if (search_req_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_node_ptr->in_use_recs_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* free the content of the cnf before freeing the cnf_ptr itself */
    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_node_ptr->in_use_recs_ptr);
    return MMGSDI_ERROR;
  }

  /* Determine record numbers with invalid pattern (empty record)  in PB Files */
  search_req_ptr->request_header.client_id           = client_id;
  search_req_ptr->request_header.session_id          = session_id;
  search_req_ptr->request_header.request_type        = MMGSDI_SEARCH_REQ;
  search_req_ptr->request_header.orig_request_type   = MMGSDI_SEARCH_REQ;
  search_req_ptr->request_header.request_len         = sizeof(mmgsdi_search_req_type);
  search_req_ptr->request_header.payload_len         = 0;
  search_req_ptr->request_header.slot_id             = slot_id;
  search_req_ptr->request_header.client_data         = 0;
  search_req_ptr->request_header.response_cb         = NULL;
  search_req_ptr->access.access_method               = MMGSDI_BY_PATH_ACCESS;
  search_req_ptr->access.file.path_type              = mmgsdi_file_node_ptr->file_path;
  search_req_ptr->search_type                        = MMGSDI_UICC_SIMPLE_SEARCH;
  search_req_ptr->rec_num                            = 1;
  search_req_ptr->search_direction                   = MMGSDI_SEARCH_FORWARD_FROM_REC_NUM;
  search_req_ptr->enhanced_search_offset.offset_data = 0;
  search_req_ptr->enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;
  search_req_ptr->data.data_len                      = rec_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr->data.data_ptr,
                                     search_req_ptr->data.data_len);
  if(search_req_ptr->data.data_ptr == NULL)
  {
    /* free the content of the cnf before freeing the cnf_ptr itself */
    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_file_node_ptr->in_use_recs_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Filling the search patteren FF here */
  memset(search_req_ptr->data.data_ptr, 0xFF, search_req_ptr->data.data_len);

  /* issuing search in sync fashion */
  mmgsdi_status = mmgsdi_uim_uicc_search_record(search_req_ptr,
                                                TRUE,
                                                &search_cnf_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr->data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);

  if(search_cnf_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_file_node_ptr->in_use_recs_ptr);
    return MMGSDI_ERROR;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_search_data_type *search_data_ptr = NULL;
    uint8                    loop_cnt        = 0;

    /* Read pb file node from cache list again.It is possible that PB cache
       pointers becomes NULL in session de-activation handling in main task
       while we are waiting for search response(in sync) from drivers */
    mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(path_type,
                                                          mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                          session_type);
    if(mmgsdi_file_node_ptr == NULL || mmgsdi_file_node_ptr->in_use_recs_ptr == NULL)
    {
      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
      return MMGSDI_ERROR;
    }

    search_data_ptr = &search_cnf_ptr->searched_record_nums;
    for (loop_cnt = 0; loop_cnt < search_data_ptr->data_len; loop_cnt++)
    {
      /* Check that the record index is in range.*/
      if ((search_data_ptr->data_ptr[loop_cnt] > 0) && 
          (search_data_ptr->data_ptr[loop_cnt] <= num_of_rec))
      {
        mmgsdi_file_node_ptr->in_use_recs_ptr[search_data_ptr->data_ptr[loop_cnt] - 1] = FALSE;
      }
    }
  }

  /* free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(search_cnf_ptr);

  return mmgsdi_status;
} /* mmgsdi_pb_send_search_req */


/*===========================================================================
FUNCTION MMGSDI_PB_GET_NEXT_VALID_RECORD

DESCRIPTION
  Returns the next valid record number starting after prev_index. If the
  memory allocation for SEARCH data failed or the SEARCH was unsuccessful,
  returns the next record in the sequence starting from prev_index.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_pb_get_next_valid_record(
  mmgsdi_rec_num_type                       current_index,
  const mmgsdi_attr_cache_element_type     *attr_cache_ptr,
  const mmgsdi_pb_cache_element_type       *mmgsdi_file_node_ptr,
  mmgsdi_rec_num_type                      *next_valid_rec_num_ptr
)
{
  mmgsdi_rec_num_type  next_index = 0;

  MMGSDIUTIL_RETURN_IF_NULL_3(attr_cache_ptr, mmgsdi_file_node_ptr,
                              next_valid_rec_num_ptr);

  if (mmgsdi_file_node_ptr->in_use_recs_ptr != NULL)
  {
    for (next_index  = current_index + 1; next_index <= attr_cache_ptr->num_of_rec; next_index++)
    {
      if (mmgsdi_file_node_ptr->in_use_recs_ptr[next_index - 1])
      {
        break;
      }
    }
  }

  *next_valid_rec_num_ptr = next_index;

  /* check the next_rec_num is in valid record range or not */
  return(next_index > attr_cache_ptr->num_of_rec) ? MMGSDI_ERROR : MMGSDI_SUCCESS;
} /* mmgsdi_pb_get_next_valid_record */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_ADD_RECORD_NODE

DESCRIPTION
  This function updates the PB record node to file list 

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_cache_add_record_node(
  mmgsdi_pb_cache_element_type     **mmgsdi_file_node_pptr,
  mmgsdi_pb_record_element_type     *pb_record_node_ptr
)
{
  mmgsdi_pb_record_element_type    *pb_record_element_ptr = NULL;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3(mmgsdi_file_node_pptr,
                                            (*mmgsdi_file_node_pptr),
                                            pb_record_node_ptr);

  if((*mmgsdi_file_node_pptr)->item_ptr == NULL)
  {
    /* List is empty Add as a first node */
    (*mmgsdi_file_node_pptr)->item_ptr = pb_record_node_ptr;
    return;
  }

  pb_record_element_ptr = (*mmgsdi_file_node_pptr)->item_ptr;
  while(pb_record_element_ptr->next_ptr != NULL)
  {
    pb_record_element_ptr = pb_record_element_ptr->next_ptr;
  }

  /* Adding new record node to the list */
  pb_record_element_ptr->next_ptr = pb_record_node_ptr;
} /* mmgsdi_pb_cache_add_record_node */


/*===========================================================================
FUNCTION MMGSDI_PB_READ_RECORD_CB

DESCRIPTION
  This function updates the PB record cache content and issue further read
  request if applicable.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_read_record_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      data_type,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  mmgsdi_attr_cache_element_type   *attr_cache_ptr        = NULL;
  mmgsdi_pb_cache_element_type     *mmgsdi_file_node_ptr  = NULL;
  mmgsdi_slot_id_enum_type          slot_id               = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_rec_num_type               next_rec_num          = 0;
  mmgsdi_pb_record_element_type    *pb_record_node_ptr    = NULL;
  uint8                             slot_index            = MMGSDI_SLOT_1_INDEX;
  mmgsdi_return_enum_type           mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type     session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_2(cnf_ptr,
                                            cnf_ptr->read_cnf.read_data.data_ptr);

  slot_id = cnf_ptr->response_header.slot_id;

  /* Validating the slot id */
  mmgsdi_util_is_slot_valid(slot_id);

  if(mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS)
  {
    return;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(cnf_ptr->response_header.session_id, &session_type , NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  /* Read pb file node from cache list */
  mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(cnf_ptr->read_cnf.access.file.path_type,
                                                        mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                        session_type);
  if(mmgsdi_file_node_ptr == NULL)
  {
    return;
  }

  /* Update the cache content */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(pb_record_node_ptr,
                                     sizeof(mmgsdi_pb_record_element_type));
  if(pb_record_node_ptr == NULL)
  {
    return;
  }

  pb_record_node_ptr->rec_num = cnf_ptr->read_cnf.accessed_rec_num;
  pb_record_node_ptr->record_data.data_len = 
                      cnf_ptr->read_cnf.read_data.data_len;
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(pb_record_node_ptr->record_data.data_ptr,
                                     pb_record_node_ptr->record_data.data_len);
  if(pb_record_node_ptr->record_data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(pb_record_node_ptr);
    return;
  }

  mmgsdi_memscpy(pb_record_node_ptr->record_data.data_ptr, 
                 pb_record_node_ptr->record_data.data_len, 
                 cnf_ptr->read_cnf.read_data.data_ptr, 
                 cnf_ptr->read_cnf.read_data.data_len);

  /* Adding record node to the file */
  mmgsdi_pb_cache_add_record_node(&mmgsdi_file_node_ptr, pb_record_node_ptr);

  /* Don't trigger caching as the file not matched with current file path */
  if(mmgsdi_file_compare_path_type(mmgsdi_file_node_ptr->file_path,
                                   mmgsdi_pb_cache_tbl_ptr[slot_index]->curr_file_path) == FALSE)
  {
    /* This is the case like file switching from one file to another (ex:file 1 to file 2) 
       (1) Reset this flag to indicate caching in not progress
       (2) Updating last record cached last record response received from card.
           Later it will be used to resume when again file switch happen on same file (file 1) */
    mmgsdi_file_node_ptr->last_record_cached = pb_record_node_ptr->rec_num;
    mmgsdi_file_node_ptr->caching_in_progress = FALSE;
    return;
  }

  /* Get file attributes from attibutes cache */
  mmgsdi_cache_attr_read(cnf_ptr->read_cnf.response_header.session_id,
                         slot_id,
                         &cnf_ptr->read_cnf.access,
                         &attr_cache_ptr);
  if(attr_cache_ptr == NULL)
  {
    /* Attribute read is failed from cache */
    return;
  }

  /* Getting next valid record from search data */
  mmgsdi_status = mmgsdi_pb_get_next_valid_record(pb_record_node_ptr->rec_num,
                                                  attr_cache_ptr,
                                                  mmgsdi_file_node_ptr,
                                                  &next_rec_num);
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Issuing a read request for the valid record */
    mmgsdi_session_read_record (
                               cnf_ptr->response_header.session_id,
                               cnf_ptr->read_cnf.access,
                               next_rec_num,
                               attr_cache_ptr->rec_len,
                               mmgsdi_pb_read_record_cb, 0);
   
    /* Setting this flag to indicate caching is in progress */
    mmgsdi_file_node_ptr->caching_in_progress = TRUE;
  }
  else
  {
    /* This update is required when last valid cached record is not equal to 
       maximum number of records present in the file.*/
    mmgsdi_file_node_ptr->last_record_cached = attr_cache_ptr->num_of_rec;
  }

  /* Freeing the memory allocated */
  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
} /* mmgsdi_pb_read_record_cb */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_DELETE_ALL_RECORDS

DESCRIPTION
  This function deletes all records present in a given file node

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_cache_delete_all_records(
  mmgsdi_pb_cache_element_type       *pb_cache_element_ptr
)
{
  mmgsdi_pb_record_element_type    *pb_record_element_ptr      = NULL;
  mmgsdi_pb_record_element_type    *next_pb_record_element_ptr = NULL;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_2(pb_cache_element_ptr,
                                            pb_cache_element_ptr->item_ptr);

  pb_record_element_ptr = pb_cache_element_ptr->item_ptr;
  while(pb_record_element_ptr != NULL)
  {
    next_pb_record_element_ptr = pb_record_element_ptr->next_ptr;

    /* Free memory allocated for each record node */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(pb_record_element_ptr->record_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(pb_record_element_ptr);

    /* Moving to next record node */
    pb_record_element_ptr = next_pb_record_element_ptr;
  }

  pb_cache_element_ptr->item_ptr = NULL;
}/* mmgsdi_pb_cache_delete_all_records */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_DELETE_FILE_NODE

DESCRIPTION
  This function is to delete PB file node from the cache file list.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_cache_delete_file_node(
  mmgsdi_pb_cache_type             **mmgsdi_pb_cache_table_pptr,
  mmgsdi_pb_cache_element_type      *mmgsdi_file_node_ptr
)
{
  mmgsdi_pb_cache_element_type    *head_file_ptr = NULL;
  mmgsdi_pb_cache_element_type    *prev_file_ptr = NULL;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3(mmgsdi_pb_cache_table_pptr,
                                            (*mmgsdi_pb_cache_table_pptr),
                                            mmgsdi_file_node_ptr);

  if((*mmgsdi_pb_cache_table_pptr)->item_ptr == NULL)
  {
    /* PB file list is empty */
    return;
  }

  head_file_ptr = (*mmgsdi_pb_cache_table_pptr)->item_ptr;

  /* When node to be deleted is head node */
  if(head_file_ptr == mmgsdi_file_node_ptr)
  {
    /* Updating head node */
    (*mmgsdi_pb_cache_table_pptr)->item_ptr = head_file_ptr->next_ptr;
  }
  else
  {
    /* find the previous node */
    prev_file_ptr = head_file_ptr;
    while(prev_file_ptr->next_ptr!= NULL && 
          prev_file_ptr->next_ptr!= mmgsdi_file_node_ptr)
    {
      prev_file_ptr = prev_file_ptr->next_ptr;
    }

    /* Check if node really exists in Linked List */
    if(prev_file_ptr->next_ptr == NULL)
    {
      /* Given node is not present in Linked List */
      return;
    }

    /* Remove node from Linked List */
    prev_file_ptr->next_ptr = prev_file_ptr->next_ptr->next_ptr;
  }

  /* free memory */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_node_ptr->in_use_recs_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_file_node_ptr);
}/* mmgsdi_pb_cache_delete_file_node */


/*===========================================================================
FUNCTION MMGSDI_PB_FREE_CACHE_DATA

DESCRIPTION
  This function deletes phonebook related cache data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_cache_data(
  mmgsdi_slot_id_enum_type      slot_id,
  mmgsdi_session_id_type        session_id,
  boolean                       delete_all_sessions
)
{
  mmgsdi_pb_cache_element_type  *pb_cache_element_ptr      = NULL;
  mmgsdi_pb_cache_element_type  *next_pb_cache_element_ptr = NULL;
  uint8                          slot_index                = MMGSDI_SLOT_1_INDEX;
  mmgsdi_session_type_enum_type  session_type              = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /* Validating the slot id */
  if(mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS)
  {
    return;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL ||
     mmgsdi_pb_cache_tbl_ptr[slot_index]->item_ptr == NULL)
  {
    /* File cache list is empty */
    return;
  }

  /* Session type validation needed only if specific session nodes need to be deleted */
  if(!delete_all_sessions)
  {
    if(mmgsdi_util_get_session_type(session_id, &session_type, NULL) != MMGSDI_SUCCESS)
    {
      return;
    }
  }

  pb_cache_element_ptr = mmgsdi_pb_cache_tbl_ptr[slot_index]->item_ptr;
  while(pb_cache_element_ptr != NULL)
  {
    next_pb_cache_element_ptr = pb_cache_element_ptr->next_ptr;

    /* It is possible that session de-activation come on one app 
       while clinet issuing reads on another app present in same slot */
    if(!delete_all_sessions && pb_cache_element_ptr->session_type != session_type)
    {
      /* Moving to next file node in the PB cache list */
      pb_cache_element_ptr = next_pb_cache_element_ptr;
      continue;
    }

    /* Freeing record list in each file node */
    mmgsdi_pb_cache_delete_all_records(pb_cache_element_ptr);

    /* Deleting the file node here */
    mmgsdi_pb_cache_delete_file_node(&mmgsdi_pb_cache_tbl_ptr[slot_index],
                                      pb_cache_element_ptr);

    /* Moving to next file node in the PB cache list */
    pb_cache_element_ptr = next_pb_cache_element_ptr;
  }
} /* mmgsdi_pb_free_cache_data */


/*==========================================================================
FUNCTION MMGSDI_PB_CACHE_RECORDS

DESCRIPTION
  This function is to trigger phonebook record caching.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_records(
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_rec_num_type        rec_num,
  mmgsdi_session_id_type     session_id
)
{
  mmgsdi_rec_num_type               next_rec_num          = 0;
  mmgsdi_return_enum_type           mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_pb_cache_element_type     *mmgsdi_file_node_ptr  = NULL;
  mmgsdi_attr_cache_element_type   *attr_cache_ptr        = NULL;
  uint8                             slot_index            = MMGSDI_SLOT_1_INDEX;
  mmgsdi_session_type_enum_type     session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /* Validating the slot id */
  mmgsdi_util_is_slot_valid(slot_id);
  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_session_type(session_id, &session_type , NULL) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read pb file node from cache list */
  mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(file_access_ptr->file.path_type,
                                                        mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                        session_type);
  if(mmgsdi_file_node_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Read file attributes from cache */
  mmgsdi_status = mmgsdi_cache_attr_read(session_id, slot_id,
                                         file_access_ptr, &attr_cache_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Don't trigger caching in the following cases
     1) If cache is already in progress as further read requests will be
        triggered from mmgsdi pb registered callback with mmgsdi_session_read_record.
     2) If the incoming path is not same as currently cached one. */
  if(mmgsdi_file_compare_path_type(mmgsdi_file_node_ptr->file_path,
                                   mmgsdi_pb_cache_tbl_ptr[slot_index]->curr_file_path) == FALSE ||
     mmgsdi_file_node_ptr->caching_in_progress)
  {
    if(mmgsdi_file_node_ptr->item_ptr == NULL)
    {
      /* This is to enable upcoming read requests in the same power cycle
         on the same file after client reads all record cached.
         (1) Updating last record cached to zero and 
         (2) Reset caching_in_progress flag. */
      if(mmgsdi_file_node_ptr->last_record_cached == attr_cache_ptr->num_of_rec)
      {
        mmgsdi_file_node_ptr->caching_in_progress = FALSE;
        mmgsdi_file_node_ptr->last_record_cached  = 0;
      }
    }

    /* Freeing the memory allocated attribute cache */
    mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
    return mmgsdi_status;
  }

  /* Check if the current cached record num is equal to the last record cached or not
     If it is not equal resume the PB caching from the last record cached on the current caching file */
  if(rec_num != mmgsdi_file_node_ptr->last_record_cached &&
     mmgsdi_file_node_ptr->last_record_cached > 0 && 
     mmgsdi_file_node_ptr->last_record_cached < attr_cache_ptr->num_of_rec)
  {
    rec_num = mmgsdi_file_node_ptr->last_record_cached;
  }

  mmgsdi_status = mmgsdi_pb_get_next_valid_record(rec_num, attr_cache_ptr,
                                                  mmgsdi_file_node_ptr,
                                                  &next_rec_num);
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Issuing a read request for valid record */
    mmgsdi_status = mmgsdi_session_read_record (session_id,
                                                *file_access_ptr, next_rec_num,
                                                attr_cache_ptr->rec_len,
                                                mmgsdi_pb_read_record_cb, 0);

   /* Setting this flag to indicate caching in progress */
   mmgsdi_file_node_ptr->caching_in_progress = TRUE;
  }
  else
  {
    /* This update is required when my last valid record is not equal to 
       maximum number of records present in the file.*/
    mmgsdi_file_node_ptr->last_record_cached = attr_cache_ptr->num_of_rec;
  }

  /* Freeing the memory allocated attribute cache */
  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
  return mmgsdi_status;
}/* mmgsdi_pb_cache_records */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_DELETE_RECORD_NODE

DESCRIPTION
  This function is to removed PB record from the cache content.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_cache_delete_record_node(
  mmgsdi_pb_cache_element_type     **mmgsdi_file_node_pptr,
  mmgsdi_pb_record_element_type     *pb_record_element_ptr
)
{
  mmgsdi_pb_record_element_type    *head_record_ptr = NULL;
  mmgsdi_pb_record_element_type    *prev_record_ptr = NULL;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3(mmgsdi_file_node_pptr,
                                            (*mmgsdi_file_node_pptr),
                                            pb_record_element_ptr);

  if((*mmgsdi_file_node_pptr)->item_ptr == NULL)
  {
    /* PB record list is empty */
    return;
  }

  head_record_ptr = (*mmgsdi_file_node_pptr)->item_ptr;

  /* When node to be deleted is head node */
  if(head_record_ptr == pb_record_element_ptr)
  {
    /* Updating head node */
    (*mmgsdi_file_node_pptr)->item_ptr = head_record_ptr->next_ptr;
  }
  else
  {
    /* find the previous node */
    prev_record_ptr = head_record_ptr;
    while(prev_record_ptr->next_ptr!= NULL &&
          prev_record_ptr->next_ptr!= pb_record_element_ptr)
    {
      prev_record_ptr = prev_record_ptr->next_ptr;
    }

    /* Check if node really exists in Linked List */
    if(prev_record_ptr->next_ptr == NULL)
    {
      /* Given node is not present in Linked List */
      return;
    }

    /* Remove node from Linked List */
    prev_record_ptr->next_ptr = prev_record_ptr->next_ptr->next_ptr;
  }

  /* free memory */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(pb_record_element_ptr->record_data.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(pb_record_element_ptr);
}/* mmgsdi_pb_cache_delete_record_node */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_ADD_FILE_NODE

DESCRIPTION
  This function is to add new file node to the PB cache list.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mmgsdi_pb_cache_add_file_node(
  mmgsdi_pb_cache_element_type       *mmgsdi_file_node_ptr,
  mmgsdi_pb_cache_type              **mmgsdi_pb_cache_table_pptr
)
{
  mmgsdi_pb_cache_element_type   *pb_cache_element_ptr = NULL;

  MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3(mmgsdi_file_node_ptr,
                                            (*mmgsdi_pb_cache_table_pptr),
                                            mmgsdi_pb_cache_table_pptr);

  if((*mmgsdi_pb_cache_table_pptr)->item_ptr == NULL)
  {
    /* List is empty Add new node as first element */
    (*mmgsdi_pb_cache_table_pptr)->item_ptr = mmgsdi_file_node_ptr;
    return;
  }

  pb_cache_element_ptr = (*mmgsdi_pb_cache_table_pptr)->item_ptr;
  while(pb_cache_element_ptr->next_ptr != NULL)
  {
    pb_cache_element_ptr = pb_cache_element_ptr->next_ptr;
  }

  /* Adding file node to the list */
  pb_cache_element_ptr->next_ptr = mmgsdi_file_node_ptr;
}/* mmgsdi_pb_cache_add_file_node */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_GET_RECORD_NODE

DESCRIPTION
  This function is to get PB record node from the cache list.

DEPENDENCIES
  None

RETURNS
  mmgsdi_pb_record_element_type*

SIDE EFFECTS
  None

===========================================================================*/
static mmgsdi_pb_record_element_type* mmgsdi_pb_cache_get_record_node(
  mmgsdi_rec_num_type               rec_num,
  mmgsdi_pb_cache_element_type     *mmgsdi_file_node_ptr
)
{
  mmgsdi_pb_record_element_type     *current_record_ptr    = NULL;
  mmgsdi_pb_record_element_type     *pb_record_element_ptr = NULL;

  if(mmgsdi_file_node_ptr == NULL || mmgsdi_file_node_ptr->item_ptr == NULL)
  {
    /* PB record list is empty */
    return NULL;
  }

  current_record_ptr = mmgsdi_file_node_ptr->item_ptr;
  while(current_record_ptr != NULL)
  {
    if(current_record_ptr->rec_num == rec_num)
    {
      /* Matched record found */
      pb_record_element_ptr = current_record_ptr;
      break;
    }

    current_record_ptr = current_record_ptr->next_ptr;
  }

  return pb_record_element_ptr;
}/* mmgsdi_pb_cache_get_record_node */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_GET_RECORD

DESCRIPTION
  This function is to get PB record from the cache content.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_get_record(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_rec_num_type        rec_num,
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_data_type          *record_data_holder,
  mmgsdi_session_id_type     session_id
)
{
  mmgsdi_return_enum_type           mmgsdi_status           = MMGSDI_SUCCESS;
  mmgsdi_pb_cache_element_type     *mmgsdi_file_node_ptr    = NULL;
  mmgsdi_attr_cache_element_type   *attr_cache_ptr          = NULL;
  mmgsdi_client_id_type             client_id               = 0;
  boolean                           ret_val                 = TRUE;
  mmgsdi_pb_record_element_type    *pb_record_element_ptr   = NULL;
  uint8                             slot_index              = MMGSDI_SLOT_1_INDEX;
  mmgsdi_len_type                   rec_len                 = 0;
  mmgsdi_rec_num_type               num_of_rec              = 0;
  mmgsdi_session_type_enum_type     session_type            = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /* Validating the slot id */
  mmgsdi_util_is_slot_valid(slot);
  MMGSDIUTIL_RETURN_IF_NULL_2(file_access_ptr, record_data_holder);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Getting client id using session id */
  mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
  if(mmgsdi_generic_data_ptr == NULL || mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  ret_val = mmgsdi_file_compare_path_type(file_access_ptr->file.path_type,
                                          mmgsdi_pb_cache_tbl_ptr[slot_index]->curr_file_path);

  /* Update current file path only if it is different than cached one and queued by client */
  if((ret_val == FALSE )&& (mmgsdi_generic_data_ptr->client_id != client_id))
  {
    mmgsdi_pb_cache_tbl_ptr[slot_index]->curr_file_path = file_access_ptr->file.path_type;
  }

  /* Reading file attributes from cache */
  mmgsdi_status = mmgsdi_cache_attr_read(session_id, slot,
                                         file_access_ptr, &attr_cache_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  rec_len    = attr_cache_ptr->rec_len;
  num_of_rec = attr_cache_ptr->num_of_rec;

  /* Freeing the memory allocated for attribute cache ptr */
  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);

  /* Validating the record number */
  if(rec_num > num_of_rec || rec_num < 0)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read pb file node from cache file list */
  mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(file_access_ptr->file.path_type,
                                                        mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                        session_type);
  if(mmgsdi_file_node_ptr == NULL)
  {
    /* File is not exist in PB cache.Allocate new node */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_file_node_ptr,
                                       sizeof(mmgsdi_pb_cache_element_type));
    /* File node addtion failed */
    if(mmgsdi_file_node_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /* Updating session type to file node */
    mmgsdi_file_node_ptr->session_type = session_type;
    mmgsdi_file_node_ptr->file_path    = file_access_ptr->file.path_type;
    mmgsdi_file_node_ptr->next_ptr     = NULL;

    /* Adding new file node to cache list */
    mmgsdi_pb_cache_add_file_node(mmgsdi_file_node_ptr,
                                  &mmgsdi_pb_cache_tbl_ptr[slot_index]);

    /* Send search request if we have valid records only */
    mmgsdi_status = mmgsdi_pb_send_search_req(slot,
                                              file_access_ptr->file.path_type,
                                              rec_len,
                                              num_of_rec,
                                              session_type);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* return error as it is fresh file */
      return MMGSDI_ERROR;
    }
  }

  /* get pb record table from file node*/
  pb_record_element_ptr = mmgsdi_pb_cache_get_record_node(rec_num,
                                                          mmgsdi_file_node_ptr);

  /* Check cache content */
  if(pb_record_element_ptr != NULL &&
     pb_record_element_ptr->record_data.data_len != 0 &&
     pb_record_element_ptr->record_data.data_ptr != NULL)
  {
    record_data_holder->data_len = pb_record_element_ptr->record_data.data_len;
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(record_data_holder->data_ptr,
                                       record_data_holder->data_len);
    if (record_data_holder->data_ptr == NULL)
    {
      /* Memory allocation failed */
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  
    /* copying record data from phonebook record cache */
    mmgsdi_memscpy(record_data_holder->data_ptr, record_data_holder->data_len,
                   pb_record_element_ptr->record_data.data_ptr,
                   pb_record_element_ptr->record_data.data_len);

    /* Freeing the phonebook cache data */
    mmgsdi_pb_cache_delete_record_node(&mmgsdi_file_node_ptr,
                                       pb_record_element_ptr);
  }
  /* check if it is empty record */
  else if(mmgsdi_file_node_ptr->in_use_recs_ptr != NULL &&
          mmgsdi_file_node_ptr->in_use_recs_ptr[rec_num - 1] == FALSE)
  {
    /* Filling up length from attribute cache */
    record_data_holder->data_len = rec_len;
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(record_data_holder->data_ptr,
                                       record_data_holder->data_len);
    if(record_data_holder->data_ptr == NULL)
    {
      /* Memory allocation failed */
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  
    /* Filling the record with FF since it is empty record */
    memset(record_data_holder->data_ptr, 0xFF, record_data_holder->data_len);
  }
  else
  {
    /* Read from cache failed..Send read request to the card */
    mmgsdi_status = MMGSDI_ERROR;
  }

  return mmgsdi_status;
}/* mmgsdi_pb_cache_get_record */


/*===========================================================================
FUNCTION MMGSDI_PB_INIT_CACHE_DATA

DESCRIPTION
  This function initlizes phonebook related cache data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_init_cache_data(
  void
)
{
  uint8    slot_index       = MMGSDI_SLOT_1_INDEX;

  for(slot_index = MMGSDI_SLOT_1_INDEX; slot_index <= MMGSDI_SLOT_3_INDEX; slot_index++)
  {
    /* Allocating memory  for MMGSDI PB cache data per slot */
    if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_pb_cache_tbl_ptr[slot_index],
                                         sizeof(mmgsdi_pb_cache_type));
    }
  }
} /* mmgsdi_pb_init_cache_data */


/*===========================================================================
FUNCTION MMGSDI_PB_FREE_FCN_CACHE

DESCRIPTION
  This function frees the Phonebook data for the phonebook that are refreshed currently.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_fcn_cache(
  uint8                                          slot_index,
  const mmgsdi_internal_refresh_file_list_type  *refresh_files_ptr,
  mmgsdi_session_id_type                         session_id
)
{
  uint32                          num_files              = 0;
  uint32                          loop_cnt               = 0;
  mmgsdi_pb_cache_element_type   *mmgsdi_file_node_ptr   = NULL;
  mmgsdi_path_type                file_path;
  mmgsdi_slot_id_enum_type        slot                   = MMGSDI_MAX_SLOT_ID_ENUM;

  memset(&file_path, 0x00, sizeof(mmgsdi_path_type));
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  if(refresh_files_ptr == NULL || refresh_files_ptr->file_path_ptr == NULL)
  {
    /* nothing to clear */
    return;
  }

  if(mmgsdi_pb_cache_tbl_ptr[slot_index] == NULL ||
     mmgsdi_util_get_slot_id(slot_index, &slot) != MMGSDI_SUCCESS)
  {
    return;
  }

  num_files = refresh_files_ptr->num_files;

  for(loop_cnt = 0; loop_cnt < num_files; loop_cnt++)
  {
    mmgsdi_memscpy(file_path.path_buf,
                   sizeof(file_path.path_buf),
                   refresh_files_ptr->file_path_ptr[loop_cnt].path_buf,
                   sizeof(refresh_files_ptr->file_path_ptr[loop_cnt].path_buf));
    file_path.path_len = refresh_files_ptr->file_path_ptr[loop_cnt].path_len;

    mmgsdi_pb_free_ef_cache (file_path, session_id, slot);
  }
}/* mmgsdi_pb_free_fcn_cache */


/*===========================================================================
FUNCTION MMGSDI_PB_FREE_EF_CACHE

DESCRIPTION
  This function frees the Phonebook data for the file being passed to this.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_ef_cache(
  mmgsdi_path_type                  path_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_slot_id_enum_type          slot
)
{
  mmgsdi_pb_cache_element_type  *mmgsdi_file_node_ptr = NULL;
  uint8                          slot_index           = 0;
  mmgsdi_session_type_enum_type  session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;

  if(mmgsdi_util_get_slot_index(slot, &slot_index) != MMGSDI_SUCCESS ||
     mmgsdi_util_get_session_type(session_id, &session_type, NULL) != MMGSDI_SUCCESS)
  {
    return;
  }

    /* Check and delete if the file is part of PB cache list */
  mmgsdi_file_node_ptr = mmgsdi_pb_cache_read_file_node(path_type,
                                                        mmgsdi_pb_cache_tbl_ptr[slot_index],
                                                        session_type);
    if(mmgsdi_file_node_ptr != NULL)
    {
    /* Freeing phonebook cache record data for the file being written currently */
      mmgsdi_pb_cache_delete_all_records(mmgsdi_file_node_ptr);

	/* Delete file attributes and search data also as the content may change as part of Write */
      mmgsdi_pb_cache_delete_file_node(&mmgsdi_pb_cache_tbl_ptr[slot_index],
                                       mmgsdi_file_node_ptr);
      mmgsdi_file_node_ptr = NULL;
    }
}/* mmgsdi_pb_free_ef_cache */

