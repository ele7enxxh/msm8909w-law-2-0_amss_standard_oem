/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 P B M   N V   R E F R E S H  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the NV Refresh
  commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm_nv_refresh.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
05/04/16   lm      Update ECC cache and send ready after NV Refresh
10/07/15   lm      Fix memory leak
07/21/15   nr      NV Refresh: MCFG - PBM interactions
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "uim_common_efs.h"
#include "pbm_nv_refresh.h"
#include "pbmutils.h"
#include "pbmgsdi.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

PBM_NV_ECC_LIST_PER_SUB  *pbm_slot_specific_ecc_list_ptr[PBM_MAX_SLOTS][PBM_NV_EMERGENCY_NUMBERS_PER_SUB] = {{NULL}};

/*===========================================================================
FUNCTION pbm_free_nv_ecc_list

DESCRIPTION

This function frees memory allocated for NV ECC list

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
void pbm_free_nv_ecc_list(
  uint8       slot_index
)
{
  uint8   sub_index   = 0;

  if(!PBMUTIL_SLOT_INDEX_VALID(slot_index) ||
     pbm_slot_specific_ecc_list_ptr[slot_index] == NULL)
  {
    return;
  }

  for ( sub_index = 0; sub_index < PBM_NV_EMERGENCY_NUMBERS_PER_SUB; sub_index++ )
  {
    if(pbm_slot_specific_ecc_list_ptr[slot_index][sub_index] != NULL)
    {
      PBM_MEM_FREEIF(pbm_slot_specific_ecc_list_ptr[slot_index][sub_index]->dial_string);
      PBM_MEM_FREEIF(pbm_slot_specific_ecc_list_ptr[slot_index][sub_index]);
    }
  }
} /* pbm_free_nv_ecc_list */


/*===========================================================================
FUNCTION PBM_INIT_NV_ECC_PER_SLOT

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  inserted or removed.  If uim_present indicates no SIM is present,
  then the longer hardcoded list of emergency numbers is used.  If
  the SIM is inserted, then the shorter list is used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_nv_ecc_per_slot (
  uint8     slot_index
)
{
  int                                i                           = 0;
  int                                j                           = 0;
  char                             **ecc_num_pptr                = NULL;
  pbm_nv_ecc_per_sub_s_type         *pbm_nv_ecc_list_per_sub_ptr = NULL;
  uim_common_efs_status_enum_type    efs_status                  = UIM_COMMON_EFS_ERROR;
  PBM_NV_ECC_LIST_PER_SUB           *ecc_node_ptr                = NULL;
  uim_common_efs_context_type        efs_context                 = 0;

  efs_context = pbm_util_get_common_efs_item_context(slot_index);
  if (efs_context >= UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  /* Clear the slot specific ECC list to update latest content as part of NV refresh */
  pbm_free_nv_ecc_list(slot_index);
  
  PBM_MEM_ALLOC(pbm_nv_ecc_list_per_sub_ptr,
                PBM_NV_EMERGENCY_NUMBERS_PER_SUB*sizeof(pbm_nv_ecc_per_sub_s_type));
  if( !pbm_nv_ecc_list_per_sub_ptr )
  {
    return ;
  }
  memset((byte *)pbm_nv_ecc_list_per_sub_ptr, 0,
         PBM_NV_EMERGENCY_NUMBERS_PER_SUB*sizeof(pbm_nv_ecc_per_sub_s_type));

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_NV_ECC_LIST_PER_SUB,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   efs_context,
                                   (uint8 *)pbm_nv_ecc_list_per_sub_ptr,
                                   (uint32)PBM_NV_EMERGENCY_NUMBERS_PER_SUB*sizeof(pbm_nv_ecc_per_sub_s_type));
  if (efs_status != UIM_COMMON_EFS_SUCCESS )
  {
    UIM_MSG_ERR_1("PBM NV ECC per sub read failed 0x%x",efs_status);
    PBM_MEM_FREEIF(pbm_nv_ecc_list_per_sub_ptr);
    return;
  }

  PBM_MEM_ALLOC(ecc_num_pptr, PBM_NV_EMERGENCY_NUMBERS_PER_SUB*sizeof(char *));
  if( !ecc_num_pptr )
  {
    PBM_MEM_FREEIF(pbm_nv_ecc_list_per_sub_ptr);
    return;
  }

  memset((byte *)ecc_num_pptr, 0, PBM_NV_EMERGENCY_NUMBERS_PER_SUB*sizeof(char *));
  for ( i = 0; i < PBM_NV_EMERGENCY_NUMBERS_PER_SUB; i++ )
  {
    if( pbm_nv_ecc_list_per_sub_ptr[i].num_digits == 0 ||
        pbm_nv_ecc_list_per_sub_ptr[i].num_digits > 6)
    {
      continue ;
    }

    if( pbm_nv_ecc_list_per_sub_ptr[i].category_length > PBM_EMERGENCY_SERVICE_CAT_LEN )
    {
      pbm_nv_ecc_list_per_sub_ptr[i].category_length = PBM_EMERGENCY_SERVICE_CAT_LEN ;
    }

    if( pbm_nv_ecc_list_per_sub_ptr[i].category_length )
    {
      pbm_nv_ecc_list_per_sub_ptr[i].emergency_category  &= PBM_EMERGENCY_SERVICE_CAT_VALID_MASK;
    }

    PBM_MEM_ALLOC(ecc_num_pptr[i], pbm_nv_ecc_list_per_sub_ptr[i].num_digits + 1);
    if( !ecc_num_pptr[i] )
    {
      PBM_MEM_FREEIF(pbm_nv_ecc_list_per_sub_ptr);
      return;
    }

    memset((byte *)ecc_num_pptr[i], 0,pbm_nv_ecc_list_per_sub_ptr[i].num_digits + 1);
    for (j = 0; j < pbm_nv_ecc_list_per_sub_ptr[i].num_digits; j++)
    {
      ecc_num_pptr[i][j] = pbm_nv_ecc_list_per_sub_ptr[i].digits[j];
    }
    ecc_num_pptr[i][j] = '\0';

    PBM_MEM_ALLOC(ecc_node_ptr, sizeof(PBM_NV_ECC_LIST_PER_SUB));
    if( ecc_node_ptr == NULL)
    {
      PBM_MEM_FREEIF(pbm_nv_ecc_list_per_sub_ptr);
      PBM_MEM_FREEIF(ecc_num_pptr[i]);
      return;
    }
    memset((byte *)ecc_node_ptr, 0, sizeof(PBM_NV_ECC_LIST_PER_SUB));

    ecc_node_ptr->dial_string     = ecc_num_pptr[i];
    ecc_node_ptr->string_length   = pbm_nv_ecc_list_per_sub_ptr[i].num_digits + 1;
    ecc_node_ptr->service_cat_len = pbm_nv_ecc_list_per_sub_ptr[i].category_length;
    if( pbm_nv_ecc_list_per_sub_ptr[i].category_length )
    {
      ecc_node_ptr->service_cat_val = pbm_nv_ecc_list_per_sub_ptr[i].emergency_category ;
    }

    /* Updating srv status flag to ECC global */
    ecc_node_ptr->srv_status_flag = pbm_nv_ecc_list_per_sub_ptr[i].value;

    pbm_slot_specific_ecc_list_ptr[slot_index][i] = ecc_node_ptr;
  }
  PBM_MEM_FREEIF(ecc_num_pptr);
  PBM_MEM_FREEIF(pbm_nv_ecc_list_per_sub_ptr);
} /* pbm_init_nv_ecc_per_sub */


/*===========================================================================
FUNCTION PBM_DELETE_SLOT_SPECIFIC_ECC_NUMBERS_FROM_CACHE

DESCRIPTION

  Delete slot specific ECC list from ecc cache as part of NV refresh 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_delete_slot_specific_ecc_numbers_from_cache(
  uint8                 slot_index
)
{
  pbm_session_enum_type        session_type   = PBM_SESSION_DEFAULT;
  PB_CACHE                    *cache          = NULL;
  uint8                        record_index   = 0;
  uint8                        ecc_index      = 0;
  pbm_slot_type                pbm_slot_id    = PBM_SLOT_1;

  if(pbm_util_get_slot_id(slot_index, &pbm_slot_id) == PBM_ERROR)
  {
    return;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  for(session_type = PBM_SESSION_LPB_GW; session_type < PBM_SESSION_MAX; session_type++)
  {
    /* No need to build ecc cache for card session as it already
       taken care as part of card inserted event */
    if(PBM_LPB != pbm_session_type_to_pb_category(session_type))
    {
      continue;
    }
    
    /* only check in RATs which belong to the requested slot.*/
    if(pbm_session_info[session_type].slot_id != pbm_slot_id)
    {
      continue;
    }

    cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
    if(cache == NULL)
    {
      continue;
    }

    for(ecc_index = 0; ecc_index < PBM_NV_EMERGENCY_NUMBERS_PER_SUB; ecc_index++)
    {
      if(pbm_slot_specific_ecc_list_ptr[slot_index][ecc_index] == NULL)
      {
        continue;
      }

      for (record_index = 1; record_index <= cache->num_of_records; record_index++)
      {
        if (cache->pb_cache_array != NULL && cache->pb_cache_array[record_index])
        {
          if (match_found(PBM_CAT_NONE, PBM_FIELD_SIM_ECC, NULL, 0, 0,
                          cache->pb_cache_array[record_index]))
          {
            /* Avoid adding from NV ECC per sub if same ECC number
               found SIM ECC cache.*/
            if ((pbm_slot_specific_ecc_list_ptr[slot_index][ecc_index]->string_length > 0) &&
                (match_found(PBM_CAT_NONE, PBM_FIELD_SIM_ECC,
                             pbm_slot_specific_ecc_list_ptr[slot_index][ecc_index]->dial_string,
                             (uint16) pbm_slot_specific_ecc_list_ptr[slot_index][ecc_index]->string_length,
                             PBM_SEARCHTYPE_EXACT,
                             cache->pb_cache_array[record_index])))
            {
              pbm_return_type ret_val    = PBM_SUCCESS;

              ret_val = pbm_cache_delete(cache, cache->pb_cache_array[record_index]->record_id);
              if (PBM_SUCCESS != ret_val)
              {
                UIM_MSG_ERR_3("Error 0x%x in deleting ECC entry 0x%x from ECC cache 0x%x",
                               ret_val, cache->pb_cache_array[record_index]->record_id, cache);
              }
            }
          }
        }
      }
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);
} /* pbm_delete_slot_specific_ecc_numbers_from_cache */


/*===========================================================================
FUNCTION PBM_SEND_ECC_READY_AFTER_NV_REFRESH

DESCRIPTION

  Re-pouplate ECC cache with the latest content and send ready to clients 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_send_ecc_ready_after_nv_refresh(
  uint8     slot_index
)
{
  pbm_session_enum_type        session_type   = PBM_SESSION_DEFAULT;
  PB_CACHE                    *cache          = NULL;
  pbm_slot_type                pbm_slot_id    = PBM_SLOT_1;
  pbm_ecc_mode_type            emergency_mode = NOT_EMERGENCY;

  if(pbm_util_get_slot_id(slot_index, &pbm_slot_id) == PBM_ERROR)
  {
    return;
  }

  for(session_type = PBM_SESSION_LPB_GW; session_type < PBM_SESSION_MAX; session_type++)
  {
    /* No need to build ecc cache for card session as it already
       taken care as part of card inserted event */
    if(PBM_LPB != pbm_session_type_to_pb_category(session_type))
    {
      continue;
    }
 
   /* only check in RATs which belong to the requested slot.*/
    if(pbm_session_info[session_type].slot_id != pbm_slot_id)
    {
      continue;
    }

    /* Derive emergency mode based on session type */
    pbm_util_get_emergency_mode(session_type, &emergency_mode);

    pbm_build_slot_specific_ecc_cache(session_type, emergency_mode,
                                      PBM_NV_EMERGENCY_NUMBERS_PER_SUB);

    cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
    if(cache == NULL)
    {
      continue;
    }

    /* sending cache ready to clients */
    PBM_SET_PB_CACHE_READY(cache);
  }
} /* pbm_send_ecc_ready_after_nv_refresh */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION PBMI_NV_REFRESH_SLOT_ITEMS

DESCRIPTION
  Function to refresh the slot-specific items from NV/EFS at the index
  corresponding to the slot on which refresh is happening.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_nv_refresh_slot_items(
  mcfg_refresh_index_type    refresh_index)
{
  uint8                        slot_index     = PBM_SLOT_1_INDEX;

  switch (refresh_index)
  {
    case MCFG_REFRESH_INDEX_0:
      slot_index   = PBM_SLOT_1_INDEX;
      break;
    case MCFG_REFRESH_INDEX_1:
      slot_index   = PBM_SLOT_2_INDEX;
      break;
    case MCFG_REFRESH_INDEX_2:
      slot_index   = PBM_SLOT_3_INDEX;
      break;
    default:
      return PBM_ERROR;
  }

  /* Delete slot specific ECC list from ecc cache as part of NV refresh */
  pbm_delete_slot_specific_ecc_numbers_from_cache(slot_index);

  pbm_init_nv_ecc_per_slot(slot_index);
  
  /* Re-pouplate ECC cache with the latest content and send ready to clients */
  pbm_send_ecc_ready_after_nv_refresh(slot_index);

  return PBM_SUCCESS;
} /* pbm_nv_refresh_slot_items */


/*===========================================================================
FUNCTION PBM_PROCESS_NV_REFRESH_REQ

DESCRIPTION
  This function processes nv refresh req - slot and session nv/efs items
  are refreshed.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_process_nv_refresh_req(
  const pbm_handle_nv_refresh_req_type *req_ptr)
{
  pbm_return_type       pbm_status    = PBM_ERROR;

  PBM_CHECK_PTR_RET(req_ptr, PBM_ERROR);

  UIM_MSG_HIGH_3("Received NV Refresh of type 0x%x - on slot: 0x%x, sub: 0x%x",
                req_ptr->type, req_ptr->slot_index, req_ptr->sub_index);

  /* Invoke NV refresh procedures here */
  if (req_ptr->type == MCFG_REFRESH_TYPE_SLOT || 
      req_ptr->type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    pbm_status = pbm_nv_refresh_slot_items(req_ptr->slot_index);
  }

  /* trigger MCFG that the procedure is done */
  (void)mcfg_refresh_done(req_ptr->reference_id);
  return PBM_SUCCESS;
}/* pbm_process_nv_refresh_req */


/*===========================================================================
FUNCTION PBM_NV_REFRESH_MCFG_CB

DESCRIPTION
  Call back registered with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_nv_refresh_mcfg_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
)
{
  pbm_handle_nv_refresh_req_type  *pbm_nv_refresh_req_ptr = NULL;

  PBM_CHECK_PTR_RET(mcfg_refresh_info_ptr, FALSE);
  pbm_nv_refresh_req_ptr = (pbm_handle_nv_refresh_req_type *)
                             pbm_cmd_alloc(sizeof(pbm_handle_nv_refresh_req_type));
  if (!pbm_nv_refresh_req_ptr)
  {
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return FALSE;
  }

  pbm_nv_refresh_req_ptr->cmd_type = PBM_MCFG_REFRESH_CB;

  /*store mcfg refresh data */
  pbm_nv_refresh_req_ptr->type         = mcfg_refresh_info_ptr->type;
  pbm_nv_refresh_req_ptr->slot_index   = mcfg_refresh_info_ptr->slot_index;
  pbm_nv_refresh_req_ptr->sub_index    = mcfg_refresh_info_ptr->sub_index;
  pbm_nv_refresh_req_ptr->reference_id = mcfg_refresh_info_ptr->reference_id;

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(pbm_nv_refresh_req_ptr));

  UIM_MSG_HIGH_3("PBM MCFG CB: PBM_HANDLE_NV_REFRESH_REQ  type 0x%x, slot 0x%x, sub 0x%x",
                 mcfg_refresh_info_ptr->type,
                 mcfg_refresh_info_ptr->slot_index,
                 mcfg_refresh_info_ptr->sub_index);
  return TRUE;
}/* pbm_nv_refresh_mcfg_cb */


/*===========================================================================
FUNCTION PBM_NV_MCFG_REFRESH_REGISTER

DESCRIPTION
  This function registers for NV refresh notification with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_nv_mcfg_refresh_register(
  void
)
{
  mcfg_refresh_registration_s_type mcfg_reg   = {0};
  boolean                          reg_status = FALSE;

  /* MCFG Registration */
  mcfg_reg.cb        = pbm_nv_refresh_mcfg_cb;
  /* RFU - defaulting it to 0*/
  mcfg_reg.group     = 0;
  mcfg_reg.order     = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.sub_mask  = MCFG_REFRESH_INDEX_MASK_ALL;

  reg_status = mcfg_refresh_register(&mcfg_reg);

  UIM_MSG_HIGH_1("PBM MCFG Registration status: 0x%x", reg_status);
} /* pbm_nv_mcfg_refresh_register */
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

