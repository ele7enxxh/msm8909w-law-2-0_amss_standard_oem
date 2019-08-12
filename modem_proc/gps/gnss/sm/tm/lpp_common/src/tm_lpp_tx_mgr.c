/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Transaction Manager module

GENERAL DESCRIPTION
  This file contains TM's LPP protocol utilities, which encode/decode LPP messages 

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


Copyright (c) 2015 Qualcomm Technologies, Inc.
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_common/src/tm_lpp_tx_mgr.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/30/15   rk      Added a initiator type check condition to remove the transaction from the table.
                     This fixes the premature removal of transaction when the incoming & subsequent outgoing transaction id is same.
  06/10/15   gk      Abort processing robustness
  02/12/15   gk      Ack processing incorrect for seq #0
  03/05/14   sj      Fixed LPP CP Fuzz crash  
  02/27/13   rk      Removed a mem copy since source & destination is same location.
  12/14/11  gk      Lint and other fixes
  04/3/11   gk      Initial version

===========================================================================*/
#include "customer.h"   /* Customer configuration file */
#include "time_svc.h"   /* time_get_secs() */
#include "tm_osysinit.h"
#include "tm_asn1util.h"
#include "tm_lpp.h"
#include "tm_lpp_asn1util.h"
#include "sm_api.h" /* included only for the data structures defined there */
#include "tm_umts_common_utils.h"
#include "msg.h"
#include "tm_lpp_tx_mgr.h"

#include "tm_data.h"   /* tm_core_get_lpp_cp_enable */
#include "tm_lpp_cp.h" 
#include "tm_lpp_cp_transport.h"

/* Local function protocols */

/* list of LPP registered clients */
tm_lpp_tx_mgr_client_list_type tm_lpp_tx_mgr_client_list[TM_LPP_TX_MGR_CLIENT_MAX];

/* Dynamic linked list of outstanding LPP transaction for a given LPP Session */  
tm_lpp_transaction_list_summary_s_type   tm_lpp_tx_summary;


/*===========================================================================
  tm_lpp_tx_mgr_find_seq_number
 
  Description: 
    LPP function to find the transaction with matching sequence number.
  
  Parameters
     u_directionn - 0 - incoming ; 1 - outgoing
     u_seq_number - sequence number to search for a match
    **p_lpp_tx_type- pointer to the transaction table with a match
 
  Returns
    TRUE if a match, FALSE otherwise
=========================================================================*/
boolean tm_lpp_tx_mgr_find_seq_number( 
  uint8                           u_direction,
  uint8                           u_seq_number,
  tm_lpp_transaction_info_s_type **p_lpp_tx_type
)
{
  tm_lpp_transaction_list_s_type  *p_lpp_tx_list = NULL;
  tm_lpp_transaction_info_s_type  *p_lpp_tx_elem = NULL;
  boolean b_found = FALSE;

  if ((tm_lpp_tx_summary.head == NULL) || (tm_lpp_tx_summary.num_trans == 0) ||
      (p_lpp_tx_type == NULL))
  {
    MSG_MED("TM_LPP_TX_MGR: Empty list. No entry found",0,0,0);
    return FALSE;
  }

  p_lpp_tx_list = tm_lpp_tx_summary.head;
  while (p_lpp_tx_list != NULL)
  {
    p_lpp_tx_elem = &p_lpp_tx_list->elem;
    if (p_lpp_tx_elem->b_in_use == TRUE)
    {

      /* check for a match of the seq number and direction of the message */
      if ((1 == u_direction) && (p_lpp_tx_elem->b_out_sequence_num_valid))
      {
        if (u_seq_number == p_lpp_tx_elem->u_out_sequence_num)
        {
          b_found = TRUE;
          break;
        }
      }
      else if ((0 == u_direction) && (p_lpp_tx_elem->b_in_sequence_num_valid))
      {
        if (u_seq_number == p_lpp_tx_elem->u_sequence_num)
        {
          b_found = TRUE;
          break;
        }
      }
    }
    p_lpp_tx_list = (tm_lpp_transaction_list_s_type *) p_lpp_tx_list->next;
  }
  /* if transaction found return the transaction */
  if (b_found == TRUE)
  {
    MSG_MED("TM_LPP_TX_MGR: Found matching Seq No:%d Direction(in 0/out 1): %d tx_id = %d",
            u_seq_number, u_direction, p_lpp_tx_elem->z_current_trans_id.u_transaction_id);
    *p_lpp_tx_type = p_lpp_tx_elem;
    return TRUE;
  }
  MSG_ERROR("TM_LPP_TX_MGR: No matching seq number",0, 0,0);
  return FALSE;
} /* tm_lpp_tx_mgr_find_seq_number */


/*===========================================================================
  tm_lpp_tx_mgr_client_register

  Description:

     This function is used by clients to register with LPP transaction manager
     with a client handle

  Parameters: 

  *p_client: Pointer to client structure
  *handle: Pointer to client handle which will be filled by this function

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_lpp_tx_mgr_client_register(const tm_lpp_tx_mgr_client_type * p_client, uint8 *handle)
{
  uint8 client_table_index=0;
  int8 client_select_index = -1;


  MSG_MED("TM_LPP_TX_MGR:Client Register ", 0,0,0);
  /*Search for available clients */
  for(; client_table_index < TM_LPP_TX_MGR_CLIENT_MAX; client_table_index++)
  {
    if(tm_lpp_tx_mgr_client_list[client_table_index].b_in_use)
    {
      if(tm_lpp_tx_mgr_client_list[client_table_index].client.client_id == p_client->client_id)
      {
        return FALSE;
      }
    }
    else
    {    
      if(client_select_index == -1)
      {
        client_select_index = (int8)client_table_index;
      }
    }
  }

  if(client_select_index < 0)
  {
    return FALSE;
  }

  /*Add the client*/
  tm_lpp_tx_mgr_client_list[client_select_index].b_in_use = TRUE;
  memscpy(&tm_lpp_tx_mgr_client_list[client_select_index].client, 
          sizeof(tm_lpp_tx_mgr_client_list[client_select_index].client),
          p_client, sizeof(*p_client));
  *handle = (uint8)client_select_index;

  MSG_HIGH("TM_LPP_TX_MGR: LPP Tx manager client added: client handle %d", client_select_index,0,0);

  return TRUE;
  
}


/*===========================================================================
  tm_lpp_tx_mgr_client_deregister

  Description:

     This function is used by clients to deregister with LPP transaction manager
     with a client handle

  Parameters: 

  *p_client: Pointer to client structure
  *handle: Pointer to client handle which will be filled by this function

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_lpp_tx_mgr_client_deregister(uint8 client_handle)
{
  

  MSG_MED("TM_LPP_TX_MGR:Client DeRegister ", 0,0,0);
  if( (client_handle >= TM_LPP_TX_MGR_CLIENT_MAX)||
      (!tm_lpp_tx_mgr_client_list[client_handle].b_in_use))
  {
    return FALSE;
  }
  
  /*Clear away the client node*/
  tm_lpp_tx_mgr_client_list[client_handle].b_in_use = FALSE;     
  memset((void *)&(tm_lpp_tx_mgr_client_list[client_handle]), 0, sizeof(tm_lpp_tx_mgr_client_list_type));
  
  MSG_HIGH("TM_LPP_TX_MGR: LPP Tx Manager client deleted: client handle %d", client_handle, 0,0);
  return TRUE;
}


/*===========================================================================
  tm_lpp_tx_mgr_client_get_client_ptr

  Description:

    LPP function to get the client structure given a client handle

  Parameters: 

  handle: client handle 

  Return value: 
    pointer to the client type
=============================================================================*/
tm_lpp_tx_mgr_client_type *tm_lpp_tx_mgr_client_get_client_ptr(uint8 client_handle)
{
  MSG_MED("TM_LPP_TX_MGR:Client GetPtr %d", client_handle,0,0);
  if (client_handle < TM_LPP_TX_MGR_CLIENT_MAX)
  {
    return (&tm_lpp_tx_mgr_client_list[client_handle].client);
  }
  return NULL;
}

/*===========================================================================
  tm_lpp_tx_mgr_get_client_type
 
  Description:
    LPP function to get the client structure given a client handle
  
  Parameters
    client handle
 
  Returns 
    Returns the pointer to the client type
===========================================================================*/
tm_lpp_tx_mgr_client_type *tm_lpp_tx_mgr_get_client_type(uint8 u_handle)
{
  return (&tm_lpp_tx_mgr_client_list[u_handle].client);
}
 
/*===========================================================================
  tm_lpp_tx_mgr_init
 
  Description:
    LPP function to init the transaction manager 
  
  Parameters
    Client ID - initiatizes the transaction table for the appropriate client. 
    If a NULL value is passed all transactions are cleared.
 
  Returns 
    none
===========================================================================*/
void tm_lpp_tx_mgr_init(uint8 u_client_id)
{
  tm_lpp_tx_mgr_client_type *p_client;

  MSG_MED("TM_LPP_TX_MGR:Tx_mgrinit client type %d  num_trans %d ", u_client_id,
          tm_lpp_tx_summary.num_trans,0);

  p_client = tm_lpp_tx_mgr_get_client_type(u_client_id);
  if (p_client)
  {
    tm_lpp_tx_mgr_remove_entry_by_clientid(p_client->client_id);
  }
}

/*===========================================================================
  tm_lpp_get_next_transaction_id
 
  Description:
    LPP function to generate a transaction ID for an outgoing LPP message
 
  Parameters: none
 
  Returns: 
    Returns the Transaction ID. the Initiator is set to SET
===========================================================================*/
static tm_lpp_transaction_id_s_type tm_lpp_get_next_transaction_id(void)
{
  MSG_MED("TM_LPP_TX_MGR:GetNextTx_ID ", 0,0,0);
  if( z_lpp_transaction_id.u_transaction_id < 255)
  {
    z_lpp_transaction_id.u_transaction_id++;
    z_lpp_transaction_id.u_initiator = TM_LPP_TRANS_ID_INIT_SET;
  }
  else
  {
    z_lpp_transaction_id.u_transaction_id = 0;
    z_lpp_transaction_id.u_initiator = TM_LPP_TRANS_ID_INIT_SET;
  }
  z_lpp_transaction_id.m.u_trans_id_valid = 1;
  MSG_MED("TM_LPP_TX_MGR:GetNextTx_ID %d, init %d ", 
    z_lpp_transaction_id.u_transaction_id, z_lpp_transaction_id.u_initiator,0);
  return z_lpp_transaction_id;
}

/*===========================================================================
  tm_lpp_tx_mgr_remove_entry_by_clientid
 
  Description: 
    Remove all transactions associated with the client id.
 
  Parameters:
   u_client_id - client id.
   
  Returns: 
   None
=============================================================================*/
void tm_lpp_tx_mgr_remove_entry_by_clientid(uint8 const u_client_id)
{
  tm_lpp_transaction_list_s_type *p_lpp_tx_curr = NULL,
                                 *p_lpp_tx_prev = NULL,
                                 *p_lpp_tx_next = NULL;

  MSG_MED("TM_LPP_TX_MGR:Remove Entries. Num of trans:%d", tm_lpp_tx_summary.num_trans, 0, 0);

  p_lpp_tx_prev = p_lpp_tx_curr = tm_lpp_tx_summary.head;

  while ( (p_lpp_tx_curr != NULL) && (tm_lpp_tx_summary.num_trans > 0) )
  {
    /* Save the next element */
    p_lpp_tx_next = p_lpp_tx_curr->next;

    if ( u_client_id == p_lpp_tx_curr->elem.u_client_handle )
    {
      /* If the head element is matched for deleting, update the head pointer */
      if ( p_lpp_tx_curr == tm_lpp_tx_summary.head )
      {
        tm_lpp_tx_summary.head = p_lpp_tx_next;
      }
      else
      {
        /* The current node will be deleted. Link the previous & next node */
        p_lpp_tx_prev->next = p_lpp_tx_next;
      }

      MSG_MED("TM_LPP_TX_MGR: FREE TX 0x%x Tx ID:%d, Seq Num: %u",
              p_lpp_tx_curr,
              p_lpp_tx_curr->elem.z_current_trans_id.u_transaction_id, 
              p_lpp_tx_curr->elem.u_out_sequence_num);


      os_MemFree((void **)&p_lpp_tx_curr);
      tm_lpp_tx_summary.num_trans--;

      /* Restore the current node to next node */
      p_lpp_tx_curr = p_lpp_tx_next;

      continue; /* Continue previous node remains the same since current node is deleted */
    }
    /* Point to previous linklist */
    p_lpp_tx_prev = p_lpp_tx_curr;
    /* Increment current pointer to point to next element */
    p_lpp_tx_curr = p_lpp_tx_next;
  } /* while */

}

/*===========================================================================
  tm_lpp_tx_mgr_remove_entry
 
  Description: 
    LPP function to remove one or all transactions associated with a transaction id.
 
  Parameters:
   *p_lpp_tx_type- pointer to the transaction to be removed
   
  Returns: 
   None
=============================================================================*/
void tm_lpp_tx_mgr_remove_entry(tm_lpp_transaction_info_s_type **p_lpp_tx_to_remove)

{
  uint8                           u_transaction_id_to_remove;
  uint8                           u_client_handle_to_remove;
  tm_lpp_transaction_list_s_type  *p_lpp_tx_curr             = NULL,
                                  *p_lpp_tx_prev = NULL,
                                  *p_lpp_tx_next             = NULL;
  uint8                     const u_num_trans_at_start       = tm_lpp_tx_summary.num_trans;
  tm_lpp_trans_id_e_type          u_initiator_to_remove = TM_LPP_TRANS_ID_INIT_SERVER;

  if ( (p_lpp_tx_to_remove == NULL) ||  (0 == tm_lpp_tx_summary.num_trans) )
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_LPP_TX_MGR: Remove entries cancelled due to null ptr(%p) or no transaction(%d)", 
          p_lpp_tx_to_remove, tm_lpp_tx_summary.num_trans);
    return;
  }

  u_transaction_id_to_remove = (*p_lpp_tx_to_remove)->z_current_trans_id.u_transaction_id;
  u_initiator_to_remove = (*p_lpp_tx_to_remove)->z_current_trans_id.u_initiator;
  u_client_handle_to_remove = (*p_lpp_tx_to_remove)->u_client_handle;
  p_lpp_tx_prev = p_lpp_tx_curr = tm_lpp_tx_summary.head;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_LPP_TX_MGR:Remove Entry(TID %d, Intr %d). Num of trans:%d", u_transaction_id_to_remove, u_initiator_to_remove, tm_lpp_tx_summary.num_trans);

  while ( (p_lpp_tx_curr != NULL) && (tm_lpp_tx_summary.num_trans > 0) )
  {
    /* Save the next element */
    p_lpp_tx_next = p_lpp_tx_curr->next;

    if ( (u_transaction_id_to_remove == p_lpp_tx_curr->elem.z_current_trans_id.u_transaction_id) &&
         (u_initiator_to_remove == p_lpp_tx_curr->elem.z_current_trans_id.u_initiator) &&
         (u_client_handle_to_remove == p_lpp_tx_curr->elem.u_client_handle) )
    {
      /* If the head element is matched for deleting, update the head pointer */
      if ( p_lpp_tx_curr == tm_lpp_tx_summary.head )
      {
        tm_lpp_tx_summary.head = p_lpp_tx_next;
      }
      else
      {
        /* The current node will be deleted. Link the previous & next node */
        p_lpp_tx_prev->next = p_lpp_tx_next;
      }

      MSG_MED("TM_LPP_TX_MGR: FREE TX 0x%x Tx ID:%d, Seq Num: %u",
              p_lpp_tx_curr,
              u_transaction_id_to_remove, p_lpp_tx_curr->elem.u_out_sequence_num);


      os_MemFree((void **)&p_lpp_tx_curr);
      tm_lpp_tx_summary.num_trans--;

      /* Restore the current node to next node */
      p_lpp_tx_curr = p_lpp_tx_next;

      continue; /* Continue previous node remains the same since current node is deleted */
    }
    /* Point to previous linklist */
    p_lpp_tx_prev = p_lpp_tx_curr;
    /* Increment current pointer to point to next element */
    p_lpp_tx_curr = p_lpp_tx_next;
  } /* while */

  if ( u_num_trans_at_start == tm_lpp_tx_summary.num_trans )
  {
    MSG_ERROR("TM_LPP_TX_MGR: Cannot find the Tx ID to free", 0, 0, 0);
  }
  else
  {
    /* Found matching transaction & removed */
    *p_lpp_tx_to_remove = NULL;
  }

  return;
} /* tm_lpp_tx_mgr_remove_entry */


/*===========================================================================
  tm_lpp_tx_mgr_insert_entry
 
  Description: 
    LPP function to insert a transaction in transaction 
          manager table
 
  Parameters:
   *p_lpp_tx_id  - pointer to transaction id type
   *p_lpp_tx_type- pointer to the transaction table
    client_handle - the client_handle to which this transaction belongs to

  Returns: 
    Returns the TRUE or FALSE. 
=============================================================================*/
boolean  tm_lpp_tx_mgr_insert_entry(const tm_lpp_transaction_id_s_type *p_lpp_tx_id, 
                                    tm_lpp_transaction_info_s_type **p_lpp_tx_type, 
                                    uint8 u_client_handle)
{
  tm_lpp_transaction_list_s_type  *p_lpp_tx_list = NULL;

  MSG_MED("TM_LPP_TX_MGR:Insert/Update Entry client_handle %d", u_client_handle, 0, 0);

  /* If update the existing transaction */
  if ( *p_lpp_tx_type != NULL )
  {
    memscpy(&((*p_lpp_tx_type)->z_current_trans_id), sizeof((*p_lpp_tx_type)->z_current_trans_id),
            p_lpp_tx_id, sizeof(*p_lpp_tx_id));
    (*p_lpp_tx_type)->b_in_use = TRUE;
    (*p_lpp_tx_type)->b_new_tx = TRUE;
    (*p_lpp_tx_type)->u_client_handle = u_client_handle;
    /* time-stamp the transaction */
    (*p_lpp_tx_type)->q_timestamp = time_get_secs();

    return ( TRUE );
  }
  else
  {
    p_lpp_tx_list = (tm_lpp_transaction_list_s_type *)os_MemAlloc(sizeof(tm_lpp_transaction_list_s_type), OS_MEM_SCOPE_TASK);
    if ( p_lpp_tx_list == NULL )
    {
      return ( FALSE );
    }
    memset(p_lpp_tx_list, 0, sizeof(tm_lpp_transaction_list_s_type));
    memscpy(&p_lpp_tx_list->elem.z_current_trans_id, sizeof(p_lpp_tx_list->elem.z_current_trans_id),
            p_lpp_tx_id, sizeof(*p_lpp_tx_id));
  }

  /*create the list if nothing was there */
  if ( tm_lpp_tx_summary.num_trans == 0 )
  {
    tm_lpp_tx_summary.head = p_lpp_tx_list;
    tm_lpp_tx_summary.num_trans = 1;
  }
  else
  {
    /* add new tx infront of the head */
    p_lpp_tx_list->next = tm_lpp_tx_summary.head;
    tm_lpp_tx_summary.num_trans++;
    tm_lpp_tx_summary.head = p_lpp_tx_list;
  }

  *p_lpp_tx_type = &p_lpp_tx_list->elem;
  (*p_lpp_tx_type)->b_in_use = TRUE;
  (*p_lpp_tx_type)->b_new_tx = TRUE;

  (*p_lpp_tx_type)->u_client_handle = u_client_handle;
  /* time-stamp the transaction */
  (*p_lpp_tx_type)->q_timestamp = time_get_secs();

  MSG_MED("TM_LPP_TX_MGR: NEW TX: Id:%d, initiator:%d num_trans:%d",
          (*p_lpp_tx_type)->z_current_trans_id.u_transaction_id,
          (*p_lpp_tx_type)->z_current_trans_id.u_initiator,
          tm_lpp_tx_summary.num_trans);

  return ( TRUE );
}



/*===========================================================================
  tm_lpp_tx_mgr_find_entry
 
  Description: 
    LPP function to find a transaction in transaction 
          manager table 
  Parameters:
    p_lpp_tx_type - Pointer to transaction type
    p_lpp_tx_id   - Pointer to transaction id
    client_handle - the client_handle to which this transaction belongs to


  Returns: 
    Returns TRUE or FALSE. 
=============================================================================*/
boolean tm_lpp_tx_mgr_find_entry(
        tm_lpp_transaction_info_s_type     **p_lpp_tx_type,
        const tm_lpp_transaction_id_s_type *p_lpp_tx_id,
        uint8 u_client_handle)
{
  tm_lpp_transaction_list_s_type  *p_lpp_tx_list = NULL;
  tm_lpp_transaction_info_s_type  *p_lpp_tx_elem;
  
  MSG_MED("TM_LPP_TX_MGR:Find Entry valid %d trans_id %d, init %d ",    
           p_lpp_tx_id->m.u_trans_id_valid, p_lpp_tx_id->u_transaction_id, p_lpp_tx_id->u_initiator);
  if (!(p_lpp_tx_id->m.u_trans_id_valid))
  {
    MSG_MED("TM_LPP_TX_MGR: Tranaction Id is not valid. Return",0,0,0); 
    return FALSE;
  }
  if (tm_lpp_tx_summary.num_trans == 0)
  {
    MSG_MED("TM_LPP_TX_MGR: Empty list. No entry found",0,0,0);
    return FALSE;
  }

  p_lpp_tx_list = tm_lpp_tx_summary.head;
  while (p_lpp_tx_list != NULL)
  {
    p_lpp_tx_elem = &p_lpp_tx_list->elem;
    if (p_lpp_tx_elem->b_in_use == TRUE &&
        p_lpp_tx_elem->u_client_handle == u_client_handle)
    {
      if ((p_lpp_tx_id->m.u_trans_id_valid) &&
           (p_lpp_tx_elem->z_current_trans_id.u_initiator == 
           p_lpp_tx_id->u_initiator) &&
          (p_lpp_tx_elem->z_current_trans_id.u_transaction_id == 
           p_lpp_tx_id->u_transaction_id))
      {
        MSG_MED("TM_LPP_TX_MGR: Found transaction entry handle %d txid %d", u_client_handle, p_lpp_tx_id->u_transaction_id, 0);
        p_lpp_tx_elem->b_new_tx = FALSE;
        *p_lpp_tx_type = p_lpp_tx_elem;
        return ( TRUE );
      }
    }
    p_lpp_tx_list = (tm_lpp_transaction_list_s_type *) p_lpp_tx_list->next;
  }

  return FALSE;
}
 

/*===========================================================================
  tm_lpp_tx_mgr_get_num_tx
 
  Description: 
    LPP function to find a transaction in transaction 
          manager table 
  Parameters:
    p_lpp_tx_type - Pointer to transaction type
    p_lpp_tx_id   - Pointer to transaction id
    client_handle - the client_handle to which this transaction belongs to


  Returns: 
    Returns TRUE or FALSE. 
=============================================================================*/
uint8 tm_lpp_tx_mgr_get_num_tx(
        uint8 u_client_handle)
{
  tm_lpp_transaction_list_s_type  *p_lpp_tx_list = NULL;
  tm_lpp_transaction_info_s_type  *p_lpp_tx_elem;
  tm_lpp_tx_mgr_client_type       *p_client      = NULL;  
  uint8 num_tx=0;
  
  if (tm_lpp_tx_summary.num_trans == 0)
  {
    MSG_MED("TM_LPP_TX_MGR: Empty list. No entry found",0,0,0);
    return 0;
  }

  p_client = tm_lpp_tx_mgr_get_client_type(u_client_handle);
  if (p_client == NULL)
  {
    MSG_MED("TM_LPP_TX_MGR: no client for client_handle %d", u_client_handle, 0, 0);
    return 0;
  }
  
  p_lpp_tx_list = tm_lpp_tx_summary.head;
  while (p_lpp_tx_list != NULL)
  {
    p_lpp_tx_elem = &p_lpp_tx_list->elem;
#if 0
    MSG_8(  MSG_SSID_GPSSM, MSG_LEGACY_MED,
           "u_client_handle %d elt client_handle %d b_in_use %d tx_id_valid %d "
           "tx_id %d msg_id %d sess_typ %d pending_ue_tx %d", 
              u_client_handle,
              p_lpp_tx_elem->u_client_handle, 
              p_lpp_tx_elem->b_in_use, 
              p_lpp_tx_elem->z_current_trans_id.u_valid,
              p_lpp_tx_elem->z_current_trans_id.u_transaction_id,
              p_lpp_tx_elem->e_messages_id,
              p_lpp_tx_elem->u_lpp_session_type,
              p_lpp_tx_elem->b_pending_ue_tx);
#endif            
            
    if (p_lpp_tx_elem->b_in_use == TRUE &&
        p_lpp_tx_elem->u_client_handle == p_client->client_id)
    {
      num_tx++;
    }
    p_lpp_tx_list = (tm_lpp_transaction_list_s_type *) p_lpp_tx_list->next;
  }

  return num_tx;
}
 



/*===========================================================================
  tm_lpp_tx_mgr_handle_entry
 
  Description: 
    LPP function to handle a transaction. this will either
          create a new entry in transaction manager table or
          update the existing entry
 
  Parameters
    *p_lpp_tx_id  - pointer to the transaction id structure
    *p_lpp_tx_type- pointer to the transaction table
     client_handle - the client_handle to which this transaction belongs to
 
 
  Returns
    TRUE if success, FALSE otherwise
=========================================================================*/
boolean tm_lpp_tx_mgr_handle_entry( 
  uint8 u_client_handle,
  tm_lpp_transaction_id_s_type *p_lpp_tx_id,
  tm_lpp_transaction_info_s_type **p_lpp_tx_type
  )
{
  boolean b_ret = TRUE;
  tm_lpp_transaction_id_s_type z_tx_id;

  MSG_MED("TM_LPP_TX_MGR: Handle entry %d", u_client_handle, 0,0);
  if (p_lpp_tx_id == NULL)
  {
    p_lpp_tx_id = &z_tx_id;
    *p_lpp_tx_id = tm_lpp_get_next_transaction_id();
    b_ret = tm_lpp_tx_mgr_insert_entry(p_lpp_tx_id, p_lpp_tx_type, u_client_handle);
    if ( b_ret == FALSE)
    {
      MSG_ERROR("TM_LPP_TX_MGR: Could not insert transaction", 0,0,0); 
      return FALSE;
    }
  }
  else
  {
    /* search if the transaction already exists in the table */
    b_ret = tm_lpp_tx_mgr_find_entry(p_lpp_tx_type, p_lpp_tx_id, u_client_handle);
    if ( b_ret == FALSE )
    {
      /* if no transaction exists in the table, add it */
      b_ret = tm_lpp_tx_mgr_insert_entry(p_lpp_tx_id, p_lpp_tx_type, u_client_handle);
      if ( b_ret  == FALSE)
      {
        MSG_ERROR("TM_LPP_TX_MGR: could not insert transaction", 0,0,0); 
        return FALSE;
      }
    }
  }
  return TRUE;
}



/*===========================================================================
  tm_lpp_tx_mgr_handle_encode_transaction

  Description:
          LPP function to encode LPP transaction. this function 
          checks the transaction, inserts it in the transaction
          in the table if not already there, handles
          encode of the transaction as passed in

  Parameters: 
      client_handle - handle to the client
      message_type - type of message to be handled
      p_lpp_tx_id - pointer to transaction id
      p_lpp_tx_type - Pointer to the transaction structure
      lpp session type - UP or CP

  Return value: none
=============================================================================*/
boolean tm_lpp_tx_mgr_handle_encode_transaction ( 
      uint8 u_client_handle,
      tm_lpp_message_e_type e_message_type,
      tm_lpp_transaction_id_s_type *p_lpp_tx_id,
      tm_lpp_transaction_info_s_type **p_lpp_tx_type,
      uint8                            lpp_session_type
)
{
  boolean b_ret;
  tm_lpp_tx_mgr_client_type *p_client;
  
  MSG_MED("TM_LPP_TX_MGR: Encode Transaction %d", e_message_type, 0,0);
  
  p_client = tm_lpp_tx_mgr_get_client_type(u_client_handle);
  if ( p_client == NULL )
  {
    MSG_ERROR("TM_LPP_TX_MGR: Unregisterd client_handle %d", u_client_handle,0,0);
    return FALSE;
  }
  {
    //validate the transaction and place it in the table
    b_ret = tm_lpp_tx_mgr_handle_entry(p_client->client_id, p_lpp_tx_id, p_lpp_tx_type);
    if ( b_ret == FALSE )
    {                  
      MSG_ERROR("TM_LPP_TX_MGR: Handle entry returned false", 0,0,0);
      return b_ret;
    }
    (*p_lpp_tx_type)->u_client_handle = p_client->client_id;
    (*p_lpp_tx_type)->e_messages_id = e_message_type;
    (*p_lpp_tx_type)->u_lpp_session_type = lpp_session_type;

    switch ((*p_lpp_tx_type)->e_messages_id  )
    {
      case  TM_LPP_MESG_PROV_CAP:
        b_ret = tm_lpp_encode_prov_cap(p_client, *p_lpp_tx_type);
        break;
      case TM_LPP_MESG_PROV_LOC_INFO:
        b_ret = tm_lpp_encode_prov_loc_info(p_client, *p_lpp_tx_type);
        break;
      case TM_LPP_MESG_REQ_AD:
        b_ret = tm_lpp_encode_req_ad(p_client, *p_lpp_tx_type);
        break;
      case TM_LPP_MESG_ERROR:
        b_ret = tm_lpp_encode_err(p_client, *p_lpp_tx_type);
        break;
      case TM_LPP_MESG_ABORT:
        b_ret = tm_lpp_encode_abort(p_client, *p_lpp_tx_type);
        break;
      default:
        MSG_ERROR("TM_LPP_TX_MGR: Unhandled message %d", (*p_lpp_tx_type)->e_messages_id, 0, 0);
        break;
    }
  }
  
  if (((*p_lpp_tx_type)->b_end_sent == TRUE) && ((*p_lpp_tx_type)->b_new_tx == TRUE))
  {
    MSG_MED("TM_LPP_TX_MGR: Got unsolicited tranc ", 0, 0,0);
    (*p_lpp_tx_type)->b_unsolicited = TRUE;
  }
  
  if ((b_ret == TRUE) && (lpp_session_type == TM_LPP_SESSION_TYPE_CP))
  {
    tm_lpp_cp_wait_for_ack(TRUE);
  }
  return b_ret;
}


/*===========================================================================
  tm_lpp_tx_mgr_handle_decode_transaction

  Description:
          LPP function to Handle LPP transaction. this function 
          checks the transaction, inserts it in the transaction
          in the table if not already there, handles
          encode/decode of the transaction as passed in

  Parameters: 
      client_handle - handle to the client
      p_paylod         - Pointer to the LPP payload to be decoded
      p_lpp_tx_type    - Pointer to the transaction 
      lpp session type - UP or CP
      tm_lpp_error_type - Pointer to LPP Error found during decode

  Return value: TRUE if success /FALSE otherwise
=============================================================================*/
boolean tm_lpp_tx_mgr_handle_decode_transaction ( 
      uint8 u_client_handle,
      const tm_pdu_buf_type *p_payload,
      tm_lpp_transaction_info_s_type **pp_lpp_tx_type,
      uint8                            lpp_session_type,
      tm_lpp_error_type              *p_lpp_error
)
{
  boolean b_ret        = FALSE;
  boolean b_end        = FALSE;
  uint32  q_pdu_type   = 0x0;

  int decode_status = 0;
  tm_lpp_transaction_id_s_type z_lpp_tx_id;

  /* Decoded LPP Message - allocated in OSYS memory */
  LPP_Message                  *p_pdu_decoded_ptr = NULL; 

  tm_lpp_tx_mgr_client_type    *p_client = NULL;
  
  if ((p_lpp_error == NULL) || (p_payload == NULL) || (pp_lpp_tx_type == NULL))
  {
    MSG_ERROR("TM_LPP_TX_MGR: Null ptr passed!! %p %p %p",
              p_lpp_error, p_payload, pp_lpp_tx_type);
    return FALSE;
  }
  p_lpp_error->u_lpp_error_type = LPP_ERROR_NO_ERR;
  
  MSG_MED("TM_LPP_TX_MGR: Decode Transaction", 0, 0,0);
  p_client = tm_lpp_tx_mgr_get_client_type(u_client_handle);
  if ( p_client == NULL )
  {
    MSG_ERROR("TM_LPP_TX_MGR: Unregisterd client_handle %d", u_client_handle,0,0);
    p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;
    p_lpp_error->u_lpp_error_value = LPP_ERROR_UNDEF;
    return FALSE;
  }
    
  /* This function allocates the OSYS memory for the Decoded LPP Message */
  p_pdu_decoded_ptr = (LPP_Message*)tm_lpp_decode_message(p_payload, &decode_status);
    
  if (p_pdu_decoded_ptr == NULL)
  {                            
    MSG_ERROR("TM_LPP_TX_MGR: Decode error",0,0,0);
    p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;
    p_lpp_error->u_lpp_error_value = LPP_ERROR_UNDEF;
    return FALSE;
  }
  if (decode_status != 0 &&
      decode_status != C_ASN1_DECODE_BUFFER_NOT_ALLOCATED) 
  {
    tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);
    p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;   
    p_lpp_error->u_lpp_error_value = LPP_ERROR_MSGBODY_ERROR;
    return FALSE;
  }  

  /***************************************************************************
   * Only LPP CP handled here                                                *
   ***************************************************************************/  
  if (lpp_session_type == TM_LPP_SESSION_TYPE_CP)
  { 
    boolean b_early_exit = FALSE;
    b_ret = tm_lpp_cp_process_ack(p_pdu_decoded_ptr, pp_lpp_tx_type, p_lpp_error, &b_early_exit);

    if (b_early_exit == TRUE)
    {
      tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);	    
      return b_ret;    
    }
  }
 
  /* Save Transaction Id and create/update the entry */
  if (p_pdu_decoded_ptr->m.transactionIDPresent)
  {
    z_lpp_tx_id.m.u_trans_id_valid = 1;
    z_lpp_tx_id.u_initiator = (tm_lpp_trans_id_e_type)p_pdu_decoded_ptr->transactionID.initiator;
    z_lpp_tx_id.u_transaction_id = p_pdu_decoded_ptr->transactionID.transactionNumber;
  }
  else
  {
    z_lpp_tx_id.m.u_trans_id_valid = 0;
  }

  b_end = p_pdu_decoded_ptr->endTransaction;  
  
  b_ret = tm_lpp_tx_mgr_handle_entry(p_client->client_id, &z_lpp_tx_id, pp_lpp_tx_type);

  if ((FALSE == b_ret) || (NULL == *pp_lpp_tx_type))
  {
    MSG_ERROR("TM_LPP_TX_MGR: Transaction error",0,0,0);
    tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);
    p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;   
    p_lpp_error->u_lpp_error_value = LPP_ERROR_INCORRECT_DATA;
    return FALSE;
  }


  if (lpp_session_type == TM_LPP_SESSION_TYPE_CP)
  {
    (*pp_lpp_tx_type)->u_lpp_session_type = TM_LPP_SESSION_TYPE_CP;
    (*pp_lpp_tx_type)->u_sequence_num     = p_pdu_decoded_ptr->sequenceNumber;
    (*pp_lpp_tx_type)->b_in_sequence_num_valid = TRUE; 
  }
  else
  {
    (*pp_lpp_tx_type)->u_lpp_session_type = TM_LPP_SESSION_TYPE_UP;
  }

  if ((p_pdu_decoded_ptr->m.lpp_MessageBodyPresent != 1) ||
      (p_pdu_decoded_ptr->lpp_MessageBody.t != LPP_MESSAGEBODY_C1))
  {
    MSG_ERROR("TM_LPP_TX_MGR: LPP Unknown message body %d", p_pdu_decoded_ptr->lpp_MessageBody.t, 0, 0);
    b_ret = FALSE;
    tm_lpp_asn1_osys_IE_free((void **)&p_pdu_decoded_ptr); 
    p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;   
    p_lpp_error->u_lpp_error_value = LPP_ERROR_MSGBODY_ERROR;
    return b_ret;
  }
  

  MSG_MED(">>>> [Server] Tx id:%d Seq no:%d Msg:%d",
          p_pdu_decoded_ptr->transactionID.transactionNumber,
          p_pdu_decoded_ptr->sequenceNumber,
          p_pdu_decoded_ptr->lpp_MessageBody.u.c1->t); 

  if ( ((*pp_lpp_tx_type)->b_end_sent == TRUE) && (b_end == TRUE))
  {
     /* if we already received end for this transaction and another end is received, it is an abnormal*/
     (*pp_lpp_tx_type)->b_abnomal_transaction = TRUE;
     MSG_ERROR("TM_LPP_TX_MGR: LPP transaction received after receiving an end. Continue to process ", 0,0,0);
  }

  /* Save a few information in Tx: seq number,  session type , etc. */
  (*pp_lpp_tx_type)->b_end_sent = p_pdu_decoded_ptr->endTransaction;
  (*pp_lpp_tx_type)->z_lpp_data_type.p_pdu_ptr = p_pdu_decoded_ptr;

  switch (p_pdu_decoded_ptr->lpp_MessageBody.u.c1->t) 
  {
    case LPP_MESSAGE_PROV_AD:
      b_ret = tm_lpp_handle_prov_ad(p_client, *pp_lpp_tx_type);       
      tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);
      (*pp_lpp_tx_type)->z_lpp_data_type.p_pdu_ptr = NULL;
      break;
    case LPP_MESSAGE_REQ_LOCINFO :
      b_ret = tm_lpp_handle_req_loc_info(p_client, *pp_lpp_tx_type);
      tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);
      (*pp_lpp_tx_type)->z_lpp_data_type.p_pdu_ptr = NULL;
      break;
        
    case LPP_MESSAGE_REQ_CAP:
      b_ret = tm_lpp_handle_req_cap(p_client, *pp_lpp_tx_type);
      tm_lpp_asn1_osys_IE_free((void**)&p_pdu_decoded_ptr);
      (*pp_lpp_tx_type)->z_lpp_data_type.p_pdu_ptr = NULL;
      break;
      
    case LPP_MESSAGE_ERROR:
      b_ret = tm_lpp_handle_error(p_client, *pp_lpp_tx_type);
      break;

    case LPP_MESSAGE_ABORT:
      b_ret = tm_lpp_handle_abort(p_client, *pp_lpp_tx_type);
      break;
        
    default:
      MSG_ERROR("TM_LPP_TX_MGR: LPP Unknown message type %d", p_pdu_decoded_ptr->lpp_MessageBody.u.c1->t, 0, 0); 
      b_ret = FALSE;
      p_lpp_error->u_lpp_error_type |= LPP_ERROR_TYPE_LPP_ERROR;   
      p_lpp_error->u_lpp_error_value = LPP_ERROR_UNDEF;
      break;
  }


  if (lpp_session_type == TM_LPP_SESSION_TYPE_CP)
  {
     if (((q_pdu_type == LPP_MESSAGE_ABORT) ||
          (q_pdu_type == LPP_MESSAGE_ERROR)) && 
         (tm_lpp_tx_mgr_get_num_tx(u_client_handle) == 0))
     {
       MSG_HIGH("Error/Abort did a reset all transactions!! No more action",
                 0, 0, 0);
     }
     else
     {	  
       if (b_ret == FALSE)
       {
         p_lpp_error->u_lpp_error_type  |= LPP_ERROR_TYPE_LPP_ERROR;
         p_lpp_error->u_lpp_error_value = LPP_ERROR_INCORRECT_DATA;
       }

       if (p_lpp_error->u_lpp_error_type == LPP_ERROR_NO_ERR)
       {
         if (( (*pp_lpp_tx_type)->b_end_sent == TRUE )  &&
             ((*pp_lpp_tx_type)->u_wait_ack == 0))
         {
           tm_lpp_tx_mgr_remove_entry(pp_lpp_tx_type);
         }
       }
    }
  }
  
  return b_ret;
} /* tm_lpp_tx_mgr_handle_decode_transaction */

/*===========================================================================
  tm_lpp_tx_mgr_find_entry_by_refloc
 
  Description: 
    Function to find the transaction in the table by Request Location Type.
  Parameters:
    req_loc_type - Find the LPP RLI transaction of this type.
    u_client_handle - Client handle
   
  Returns: 
   None
=============================================================================*/
tm_lpp_transaction_info_s_type* tm_lpp_tx_mgr_find_entry_by_refloc(uint32 const req_loc_type, uint8 const u_client_handle)
{
  tm_lpp_transaction_info_s_type  *p_lpp_tx_elem = NULL;
  tm_lpp_transaction_list_s_type  *p_lpp_tx_list = NULL;
  uint8 u_trans = 0;

  MSG_MED("TM_LPP_CP:Find entry by refloc type 0x%X", req_loc_type, 0, 0);

  p_lpp_tx_list = tm_lpp_tx_summary.head;

  while ( (p_lpp_tx_list !=NULL)) 
  {
    p_lpp_tx_elem = &p_lpp_tx_list->elem;
    /* Find a Req Loc Info or existing Prov Loc Info request */
    if ((TM_LPP_MESG_REQ_LOC_INFO == p_lpp_tx_elem->e_messages_id ) &&
        (TRUE == p_lpp_tx_elem->b_pending_ue_tx) &&
        (req_loc_type == p_lpp_tx_elem->z_lpp_data_type.req_loc_type ) &&
        (u_client_handle == p_lpp_tx_elem->u_client_handle )
        ) 
    {
      return p_lpp_tx_elem;
    }
    p_lpp_tx_list = (tm_lpp_transaction_list_s_type *) p_lpp_tx_list->next;
  }
  return NULL;
}
