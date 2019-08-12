/*!
  @file
  ds_3gpp_op_pco_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_op_pco_hdlr.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
12/20/13     vb     Handling operator pco operations
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ds_3gpp_op_pco_hdlr.h"
#include "queue.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_apn_table.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

static q_type ds_3gpp_op_pco_q;


/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_INIT

DESCRIPTION
  This function initializes the op_pco queue.
  The function is called during power up.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

=============================================================================*/
void ds_3gpp_op_pco_init( void )
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  q_init(&ds_3gpp_op_pco_q);
  
}

/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_MOD_IND

DESCRIPTION
  This api is called by the client to indicate the reception of a OP_PCO_EV
  It is the client's responsibility to populate memory apn and
  get_op_pco_f_ptr.

PARAMETERS
  ds_3gpp_op_pco_ev_enum_type           op_modified_ev_name,
  char                                 *apn,
  ds_3gpp_get_op_pco_f_ptr_type        *get_op_pco_f_ptr

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  --  PCO MOD is successful
  FALSE --  Otherwise

SIDE EFFECTS
  None.

=============================================================================*/

boolean ds_3gpp_op_pco_mod_ind
(
  ds_3gpp_op_pco_ev_enum_type           op_modified_ev_name,
  char                                 *apn,
  ds_3gpp_get_op_pco_f_ptr_type         get_op_pco_f_ptr
)
{
  boolean                               ret_val = FALSE;
  ds_3gpp_op_pco_q_block               *op_pco_q_blk_ptr = NULL;
  ds_pdn_context_s                     *pdn_cntxt_ptr = NULL;
  ds_pdn_context_s                     *pdn_cntxt_ptrs[DS_3GPP_MAX_PDN_CONTEXT]
									                      = {NULL};
  uint8                                 pdn_cntxt_ptrs_index = 0;
  ds_bearer_context_s                  *bearer_cntxt_ptr = NULL;
  uint8                                 bearer_index = 0;	
  ds_3gpp_redial_e_type                 redial_reason = DS_3GPP_NO_REDIAL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((apn == NULL) || (get_op_pco_f_ptr == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input params passed is NULL:apn:0x%x;"
                       "get_op_pco_f_ptr:0x%x",apn,get_op_pco_f_ptr);
    return ret_val;
  }

  if (strlen(apn) > DS_UMTS_MAX_APN_STRING_LEN)
  {
    DS_3GPP_MSG1_ERROR("APN length is INVALID:%d",strlen(apn));
    return ret_val;
  }

  switch (op_modified_ev_name)
  {
    /*------------------------------------------------------------------------- 
    Determine if a new entry needs to be added to op_pco_holder or if an
    entry already exists.
    Irrespective check if a PDN connection to the corresponding apn exists
    If a PDN to the apn exists:
    Perform Silent redial i.e Send out a PDN disconnect request and
    reconnect again, Query the client to fill in new PCO contents.
    -------------------------------------------------------------------------*/
    case DS_3GPP_ADD_OP_PCO_EV:

      op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)q_linear_search(&ds_3gpp_op_pco_q,
                                              ds_3gpp_op_pco_holder_search_entry,
                                              apn);
      if (op_pco_q_blk_ptr == NULL)
      {
        DS_3GPP_MSG0_HIGH("Add entry to OP PCO holder");

        if(!ds_3gpp_op_pco_holder_add_entry(apn,get_op_pco_f_ptr))
        {
          DS_3GPP_MSG0_ERROR("PCO queue add failure");
          return ret_val;
        }
      }

      ds_3gpp_pdn_cntxt_get_active_pdns(apn,pdn_cntxt_ptrs);

      for (pdn_cntxt_ptrs_index = 0;pdn_cntxt_ptrs_index < DS_3GPP_MAX_PDN_CONTEXT;
           pdn_cntxt_ptrs_index++)
      {

        if (pdn_cntxt_ptrs[pdn_cntxt_ptrs_index] == NULL)
        {
          break;
        }

        pdn_cntxt_ptr = pdn_cntxt_ptrs[pdn_cntxt_ptrs_index];
        /*---------------------------------------------------------------------- 
         If pdn context ptr returned is not NULL, perrform silent redial ie
         PDN disconnect followed by PDN reconnect
        -----------------------------------------------------------------------*/
        /*----------------------------------------------------------------------
          Tear down all the bearers associated with the PDN context.
        ----------------------------------------------------------------------*/
        for (bearer_index=0;bearer_index < 
              DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;bearer_index++)
        {
          bearer_cntxt_ptr = pdn_cntxt_ptr->ds_pdn_context_dyn_p->
          bearer_context_ptr_tbl[bearer_index];

          if (!ds_bearer_cntx_validate_bearer_context(bearer_cntxt_ptr))
          {
            continue;
          }

          redial_reason = DS_3GPP_REDIAL_OP_PCO_CHG;
          ds_3gpp_pdn_cntxt_prepare_for_redial((void *)bearer_cntxt_ptr,
                                               redial_reason);

        }
      }
      ret_val = TRUE;
      break;

     case DS_3GPP_DEL_OP_PCO_EV:
      ret_val = ds_3gpp_op_pco_holder_del_entry(apn);
      break;

     default:
      DS_3GPP_MSG0_ERROR("Invalid event name");
      break;

  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_op_pco_mod_ind is returning %d",ret_val);
  return ret_val;
}

/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_HOLDER_ADD_ENTRY 
 
DESCRIPTION
  This is used to add the apn, f_ptr entry in the op_pco_holder

PARAMETERS
  char                                 *apn,
  ds_3gpp_get_op_pco_f_ptr_type        *get_op_pco_f_ptr

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - If PCO addition is successful
  FALSE - Otherwise

SIDE EFFECTS
  None.

=============================================================================*/
boolean ds_3gpp_op_pco_holder_add_entry
(
  char                                 *apn,
  ds_3gpp_get_op_pco_f_ptr_type         get_op_pco_f_ptr
)
{
  boolean                         ret_val = FALSE;
  ds_3gpp_op_pco_q_block         *op_pco_q_blk_ptr = NULL;
  ds_3gpp_op_pco_holder_type     *op_pco_holder_ptr = NULL;
  uint8  						  str_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((apn == NULL) || (get_op_pco_f_ptr == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input params passed is NULL:apn:0x%x;"
                       "get_op_pco_f_ptr:0x%x",apn,get_op_pco_f_ptr);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
   Allocate op_pco_holder_ptr and also populate op_pco_blk_ptr, Then populate
   the queue
   -------------------------------------------------------------------------*/
  op_pco_holder_ptr = (ds_3gpp_op_pco_holder_type *)modem_mem_alloc
    (sizeof(ds_3gpp_op_pco_holder_type), MODEM_MEM_CLIENT_DATA);

  if (op_pco_holder_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("OP PCO holder memory allocation failed");
    return ret_val;
  }

  op_pco_holder_ptr->apn_name = (char *)modem_mem_alloc(strlen(apn)+1,
                                 MODEM_MEM_CLIENT_DATA);


  if (op_pco_holder_ptr->apn_name == NULL)
  {
    /*-------------------------------------------------------------------- 
    apn memory allocation failed, Return failure and free
    allocated memory 
    ---------------------------------------------------------------------*/
    DS_3GPP_MSG0_ERROR("OP_PCO_HOLDER apn allocation failed!");
    goto mem_free2;

  }
  
  str_len = strlen(apn)+1;

  (void)strlcpy((char*)op_pco_holder_ptr->apn_name,(char*)apn,
                str_len);

  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"OP PCO added APN name: %s",
                          op_pco_holder_ptr->apn_name);

  op_pco_holder_ptr->get_op_pco_f_ptr = get_op_pco_f_ptr;

  /*-------------------------------------------------------------------------- 
    All memory allocation is successful, Now insert into the queue
  -------------------------------------------------------------------------*/
  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)modem_mem_alloc
                      (sizeof(ds_3gpp_op_pco_q_block),MODEM_MEM_CLIENT_DATA);

  if(op_pco_q_blk_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("OP PCO Q BLK mem alloc failure");
    goto mem_free1;
  }

  op_pco_q_blk_ptr->op_pco_holder_ptr = op_pco_holder_ptr;

  q_link(op_pco_q_blk_ptr, &(op_pco_q_blk_ptr->link));
  q_put(&ds_3gpp_op_pco_q, &(op_pco_q_blk_ptr->link));

  ret_val = TRUE;
  return ret_val;

  mem_free1:
    modem_mem_free((void *)op_pco_holder_ptr->apn_name,MODEM_MEM_CLIENT_DATA);

  mem_free2:
    modem_mem_free((void *)op_pco_holder_ptr,MODEM_MEM_CLIENT_DATA);
    return ret_val;

}

/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_HOLDER_DEL_ENTRY 
 
DESCRIPTION
  This is used to delete the apn, f_ptr entry in the op_pco_holder

PARAMETERS
  char   *apn,

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Deletion is successful
  FALSE - Otherwise

SIDE EFFECTS
  None.

=============================================================================*/
boolean ds_3gpp_op_pco_holder_del_entry
(
  char *apn
)
{
  boolean ret_val = FALSE;
  ds_3gpp_op_pco_q_block		 *op_pco_q_blk_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input param apn is NULL");
    return ret_val;
  }

  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)q_check(&ds_3gpp_op_pco_q);

  if (op_pco_q_blk_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("OP PCO Q is empty");
    return ret_val;
  }


  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)q_linear_search( &ds_3gpp_op_pco_q,
                                               ds_3gpp_op_pco_holder_search_entry,
                                               apn);

  if (op_pco_q_blk_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("Entry not present in OP PCO holder");
    return ret_val;
  }

  /*---------------------------------------------------------------------------
    Free the op_pco_q_blk_ptr and then delete the entry from the queue
   --------------------------------------------------------------------------*/
  q_delete(&ds_3gpp_op_pco_q, &(op_pco_q_blk_ptr->link));

  modem_mem_free((void *)op_pco_q_blk_ptr->op_pco_holder_ptr->apn_name,
                 MODEM_MEM_CLIENT_DATA);

  op_pco_q_blk_ptr->op_pco_holder_ptr->get_op_pco_f_ptr = NULL;

  modem_mem_free((void *)op_pco_q_blk_ptr->op_pco_holder_ptr,
                 MODEM_MEM_CLIENT_DATA);

  ret_val = TRUE;
  return ret_val;
}


/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_HOLDER_SEARCH_ENTRY 
 
DESCRIPTION
  This is used to search if the searched apn string is present in the
  op_pco_holder, this is used only by q_linear_search

PARAMETERS
 void *item_ptr - Queue from which target_val needs to be searched
 void *target_val - Apn string
 
DEPENDENCIES
  None.

RETURN VALUE
  1  -- Search is successful
  0  -- Search is not successful

SIDE EFFECTS
  None.

=============================================================================*/
int ds_3gpp_op_pco_holder_search_entry
(
  void *item_ptr, 
  void *target_val
)
{
  int                         ret_val = 0;
  ds_3gpp_op_pco_q_block     *op_pco_q_blk_ptr = NULL;
  char 	                     *search_apn = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((item_ptr == NULL) || (target_val == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input_parameters passed is NULL:item_ptr:0x%x:"
                       "target_val:0x%x",item_ptr,target_val);
    return ret_val;
  }

  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)item_ptr;
  search_apn = (char *)target_val;

  if ((op_pco_q_blk_ptr->op_pco_holder_ptr == NULL) ||
      (op_pco_q_blk_ptr->op_pco_holder_ptr->apn_name == NULL))
  {
    DS_3GPP_MSG1_ERROR("Memory for queue not allocated successfully:0x%x",
                       op_pco_q_blk_ptr->op_pco_holder_ptr);
    return ret_val;
  }

  if((strcasecmp((char*)op_pco_q_blk_ptr->op_pco_holder_ptr->apn_name,
                 (char *)search_apn) == 0))
  {
    DS_3GPP_MSG0_HIGH("String comparision success");
    ret_val = 1;
  }

  return ret_val;
}



/*=============================================================================
FUNCTION DS_3GPP_OP_PCO_HOLDER_SEARCH_APN_ENTRY 
 
DESCRIPTION
  This is used to search if the searched apn string is present in the
  op_pco_holder

PARAMETERS
  pdn_context_ptr 
 
DEPENDENCIES
  None.

RETURN VALUE
  void *

SIDE EFFECTS
  None.

=============================================================================*/
void *ds_3gpp_op_pco_holder_search_apn_entry
(
   ds_pdn_context_s *pdn_cntxt_ptr
)
{
  ds_3gpp_op_pco_q_block     *op_pco_q_blk_ptr = NULL;
  byte                       *resolved_apn = NULL;
/* - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_ptr))
  {
    return NULL;
  }

  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"OP PCO search APN name: %s",
                          pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.
                          context.apn);

  /*------------------------------------------------------------------------- 
  Do a q_linear_search to see if the apn is present in the op_pco_holder
  Try a match with the profile's apn and then with the resolved APN
  ------------------------------------------------------------------------*/
  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)q_linear_search(
                             &ds_3gpp_op_pco_q,
                             ds_3gpp_op_pco_holder_search_entry,
                             pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn);

  if (op_pco_q_blk_ptr == NULL)
  {
    (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_cntxt_ptr,&(resolved_apn));

    if (resolved_apn == NULL)
    {
      DS_3GPP_MSG0_ERROR("Resolved APN is a NULL PTR");
      return NULL;
    }

    op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block*)
                        q_linear_search(&ds_3gpp_op_pco_q,
                        ds_3gpp_op_pco_holder_search_entry,
                        (char *)resolved_apn);

    if (op_pco_q_blk_ptr == NULL)
    {
      DS_3GPP_MSG0_HIGH("APN entry in OP PCO block is not found");
    }

  }


  return (void *)op_pco_q_blk_ptr;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif
