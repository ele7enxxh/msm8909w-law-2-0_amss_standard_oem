/*! 
  @file
  rf_dispatch_snum.c
 
  @brief
  This file includes common second level dispatch implementation for all task
  used by RFSW
 
*/
 
/*==============================================================================

  Copyright (c) 2013,2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_dispatch_snum.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/28/14   ndb/yb  SNUM debug code 
07/15/13   ndb     Added the Critical section in rf_dispatch_snum_find_item()
07/11/13   Saul    Cal Time Reduction. Inlined some APIs and removed some F3s.
06/05/13   sty     Reduced F3 levels
04/12/13   aro     F3 update
03/25/13   zhw     KW fix
03/21/13   zhw     Updated rf_dispatch_sum_deregister_callback
01/16/13   aro     Renamed handler to handle
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     NULL pointer check
01/15/13   aro     Documentation Update
01/15/13   aro     Updated registration interface to pass dispatch handle
01/10/13   aro     Doxygen documentation update
01/09/13   aro     Renamed UMID to CID
01/09/13   aro     Fixed the bug of peforming duplicate SNUM pool init
01/09/13   aro     Removed memeset
01/09/13   aro     Bug fixes for dynamic registration test case
01/09/13   aro     Added SNUM dispatch
01/08/13   aro     Compiler warning fix
01/08/13   aro     Added deinitialization of SNUM pool to be called during
                   CID deregistration
01/08/13   aro     Organized secondary dispatch data container
01/08/13   aro     Updated snum init interface
01/08/13   aro     Removed init_done flag
01/07/13   aro     Removed cid_info parameter from all internal snum pool
                   functions
01/07/13   aro     Added pool initialization during first snum registration
01/07/13   aro     Implementation to perform callback deregistration for
                   sequence number based secondary dispatching
01/07/13   aro     NULL pointer check
01/07/13   aro     Added SNUM pool pop and push
01/07/13   aro     Implementation to perform callback registration for
                   sequence number based secondary dispatching
01/07/13   aro     Internal helper functions to perform snum pool management
01/07/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rf_dispatch_snum.h"
#include "rfcommon_msg.h"
#include "rf_dispatch.h"
#include "modem_mem.h"
#include "err.h"

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_POOL
 
  @brief
  This module holds implemenation of sequence number pool. This also includes
  the linked list implementation for the sequence nmber pool.
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Insert Item to doubly linked list
  
  @details
  Inserts a rf_dispatch_snum_info_type into a doubly-linked list of 
  rf_dispatch_snum_info_type.  The item is inserted after the item passed
  in as item_before.
 
  This function will form an double linked list as shown in the diagram.
  
  @param item_before
  SNUM Item to which a new item is to be inserted
 
  @param item_to_insert
  New SNUM Item to be inserted
*/
INLINE void
rf_dispatch_snum_insert_item
(
  rf_dispatch_snum_node_type *item_before, 
  rf_dispatch_snum_node_type *item_to_insert
)
{

  item_to_insert->next = item_before->next;
  (item_before->next)->prev = item_to_insert;
  item_before->next = item_to_insert;
  item_to_insert->prev = item_before;

} /* rf_dispatch_snum_insert_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Remove SNUM Item to doubly linked list
  
  @details
  Removes a rf_dispatch_snum_info_type into a doubly-linked list of 
  rf_dispatch_snum_info_type. 
 
  @param item_to_remove
  SNUM Item to be removed
*/
INLINE void
rf_dispatch_snum_remove_item
(
  const rf_dispatch_snum_node_type *item_to_remove
)
{
  (item_to_remove->prev)->next = item_to_remove->next;
  (item_to_remove->next)->prev = item_to_remove->prev;

} /* rf_dispatch_snum_remove_item */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Find SNUM Item containg given item ID in used list
 
  @details
  This function locates a rf_dispatch_snum_info_type in the "used" list.
  The  SNUM is used as the comparison key. If an item is found matching the 
  input comparison key, a pointer to that item is returned. If a match is not
  found a pointer to the sentinel is returned
 
  @param pool_data
  Pointer to the pool data
 
  @param snum
  Sequence number for which the entry is to be found
 
  @return
  If an item is found matching the input comparison key, a pointer to that item 
  is returned. If a match is not found a pointer to the sentinel is returned.
*/
INLINE rf_dispatch_snum_node_type*
rf_dispatch_snum_find_item
(
  rf_dispatch_snum_pool_type *pool_data,
  uint32 snum
)
{
  rf_dispatch_snum_node_type *found = NULL;

  /* Variable to keep track of number of search counts */
  uint8 search_count = 0; 

  /* NULL pointer check */
  if ( pool_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_snum_find_item: NULL Pool Data" ); 
    return NULL;
  } /* if ( pool_data == NULL ) */

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(pool_data->critical_sect) );
  
  /* NULL Pointer check */
  if ( pool_data->sentinel_used_items != NULL )
  {
    /* Initialize the variable to point to next item pointed by sentinel */
    found = pool_data->sentinel_used_items->next;
  
    /* Loop through the linked list to find the item containing the given 
    snum */
    while ( found != pool_data->sentinel_used_items )
    {
     /* Increment the search count */
      search_count++;
  
      /* Since the maximum number of possible nodes is
      RF_DISPATCH_NUM_MAX_CONCURRENT_RSP, the number of search should be finite. 
      If it is more, then the Linked List is corrupted */
      if ( search_count > RF_DISPATCH_NUM_MAX_CONCURRENT_RSP )
      {
        ERR_FATAL( "rf_dispatch_snum_find_item: SNUM.0x%x - "
                   "LinkedList is corrupted", snum, 0, 0 ); 
      } /* if ( search_count > RF_DISPATCH_NUM_MAX_CONCURRENT_RSP ) */
      else
      {
        if ( found->snum_info.snum == snum )
        {
          /* Match is found. Therefore break */
          break;
        } /* if ( found->snum_info.snum == snum) */
        else
        {
          /* No match, traverse through the linked list */
          found = found->next;
        } /* if ! ( found->snum_info.snum == snum ) */
      } /* if ! ( search_count > RF_DISPATCH_NUM_MAX_CONCURRENT_RSP ) */
    } /* while ( found != &(pool_data->sentinel_used_items) ) */
  
    /* Check if the an item with the SNUM was found in the used list.
    If not found, then return NULL*/
    if( found == pool_data->sentinel_used_items )
    {
      found = NULL;
    } /* if( found == &(pool_data->sentinel_used_items) ) */
  } /* if ( pool_data->sentinel_used_items != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_dispatch_snum_find_item: SNUM.0x%x - NULL Used "
              "Pointer", snum ); 
  } /* if ! ( pool_data->sentinel_used_items != NULL ) */

   /* Leave Critical Section */
  rf_common_leave_critical_section( &(pool_data->critical_sect) );

  /* return the item */
  return found;

} /* rf_dispatch_snum_find_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize SNUM pool
  
  @details
  This will initialize SNUM pool linked list.
 
  @param pool_data
  Pointer to SNUM data
 
  @return
  Status of SNUM pool init
*/
INLINE boolean
rf_dispatch_snum_pool_init
(
  void **pool_data
)
{
  uint8 idx = 0;
  boolean ret_val = TRUE;
  rf_dispatch_snum_pool_type *pool_ptr;

  /* Pool data pointer should be NULL when this function is called */
  if ( *pool_data != NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_snum_pool_init: SNUM Pool is not NULL" );
    return FALSE;
  } /* if ( *pool_data != NULL ) */
  
  /* Allocate memory */
  *pool_data = modem_mem_alloc( sizeof(rf_dispatch_snum_pool_type),
                                MODEM_MEM_CLIENT_RFA );
  
  pool_ptr = (rf_dispatch_snum_pool_type*)(*pool_data);

  /* Check if the allocation was successful */
  if ( pool_ptr != NULL )
  {
    /* Memset pool pointer */
    memset( pool_ptr, 0x0, sizeof(rf_dispatch_snum_pool_type) );

    /* Init NO-SNUM-INFO structure */
    pool_ptr->no_snum_info.item_id = 0xFF;
    pool_ptr->no_snum_info.snum = 0;
    pool_ptr->no_snum_info.static_cb_registration = FALSE;
    pool_ptr->no_snum_info.snum_cb = NULL;
    pool_ptr->no_snum_info.snum_cb_data = NULL;

    /* Proceed only if item_list is empty */
    if ( pool_ptr->item_list == NULL )
    {
      pool_ptr->item_list = modem_mem_alloc( 
          RF_DISPATCH_NUM_MAX_CONCURRENT_RSP*sizeof(rf_dispatch_snum_node_type), 
          MODEM_MEM_CLIENT_RFA );

      pool_ptr->sentinel_spare_items = modem_mem_alloc( 
                               sizeof(rf_dispatch_snum_node_type), 
                               MODEM_MEM_CLIENT_RFA );

      pool_ptr->sentinel_used_items = modem_mem_alloc( 
                               sizeof(rf_dispatch_snum_node_type), 
                               MODEM_MEM_CLIENT_RFA );

      /* build the items into the doubly-linked list */

      /* Check if the item list is successfully allocated */
      if ( ( pool_ptr->item_list != NULL ) &&
           ( pool_ptr->sentinel_spare_items != NULL ) &&
           ( pool_ptr->sentinel_used_items != NULL ) )
      {
        memset( pool_ptr->item_list, 0x0, sizeof(rf_dispatch_snum_node_type) );
        pool_ptr->sentinel_spare_items->next = 
                          pool_ptr->sentinel_spare_items;
        pool_ptr->sentinel_spare_items->prev = 
                          pool_ptr->sentinel_spare_items;
    
        /* Insert items into Spare list */
        for ( idx = 0; idx < RF_DISPATCH_NUM_MAX_CONCURRENT_RSP; idx++ )
        {
          pool_ptr->item_list[idx].snum_info.item_id = idx;
          pool_ptr->item_list[idx].snum_info.snum = 0;
          pool_ptr->item_list[idx].snum_info.static_cb_registration = FALSE;
          pool_ptr->item_list[idx].snum_info.snum_cb = NULL;
          pool_ptr->item_list[idx].snum_info.snum_cb_data = NULL;
          rf_dispatch_snum_insert_item( pool_ptr->sentinel_spare_items,
                                        &pool_ptr->item_list[idx] );
        } /* for ( idx = 0; idx < RF_DISPATCH_NUM_MAX_CONCURRENT_RSP; idx++ ) */
    
        pool_ptr->sentinel_used_items->next = 
                           pool_ptr->sentinel_used_items;
        pool_ptr->sentinel_used_items->prev = 
                           pool_ptr->sentinel_used_items;
    
        pool_ptr->num_of_spare_items = RF_DISPATCH_NUM_MAX_CONCURRENT_RSP;
    
        rf_common_init_critical_section( &(pool_ptr->critical_sect) );
    
        RF_MSG( RF_MED, "rf_dispatch_snum_pool_init: SNUM Pool initialized" );
      } /* if ( ( pool_data->item_list != NULL ) &&
                ( pool_data->sentinel_spare_items != NULL ) &&
                ( pool_data->sentinel_used_items != NULL ) ) */
      else
      {
        ret_val = FALSE;
        RF_MSG( RF_ERROR, "rf_dispatch_snum_pool_init: alloc failed" );
      } /* if ! ( ( pool_data->item_list != NULL ) &&
                  ( pool_data->sentinel_spare_items != NULL ) &&
                  ( pool_data->sentinel_used_items != NULL ) ) */
    } /* if ( pool_data->item_list == NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG( RF_ERROR, "rf_dispatch_snum_pool_init: List is not NULL. ABORT "
                        "allocation" );
    } /* if !  ( pool_data->item_list == NULL ) */
  } /* if ( pool_data != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG( RF_ERROR, "rf_dispatch_snum_pool_init: Pool malloc failed " );
  } /* if ! ( pool_data != NULL ) */

  return ret_val;

} /* rf_dispatch_snum_pool_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize SNUM pool
  
  @details
  This will de-initialize SNUM pool linked list.
 
  @param sd_data
  Pointer secondary dispatch data
 
  @return
  Status of SNUM pool deinit
*/
static boolean
rf_dispatch_snum_pool_deinit
(
  rf_dispatch_cid_sec_dispatch_type *sd_data
)
{
  boolean ret_val = TRUE;
  rf_dispatch_snum_pool_type *pool_data;

  /* Get Pool Data */
  pool_data = (rf_dispatch_snum_pool_type*)(sd_data->dispatch_data);

  /* Free the Memory allocated for SNUM Pool item list */
  modem_mem_free( pool_data->item_list, MODEM_MEM_CLIENT_RFA );

  /* Free the Memory allocated for Spare Item*/
  modem_mem_free( pool_data->sentinel_spare_items, MODEM_MEM_CLIENT_RFA );
  /* Free the Memory allocated for Spare Item*/
  modem_mem_free( pool_data->sentinel_used_items, MODEM_MEM_CLIENT_RFA );

  /* De-initialize the critical section */
  ret_val &= rf_common_deinit_critical_section( &(pool_data->critical_sect) );

  /* Memset pool data to 0 */
  memset( pool_data, 0x0, sizeof(rf_dispatch_snum_pool_type) );

  RF_MSG( RF_MED, "rf_dispatch_snum_pool_deinit: SNUM Pool de-initialized" );

  return ret_val;

} /* rf_dispatch_snum_pool_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pop SNUM item from SNUM Pool
 
  @details
  This function will be used pop any available spare SNUM item from
  the SNUMPool. If no spare SNUM is found, it is a unrecoverable situation.
  Thus an ERR_FATAL will be done.
 
  @param pool_data
  Pointer to the structure containing SNUM Pool Data
 
  @return
  Pointer to the SNUM node info which is poped to be used by client
*/
INLINE rf_dispatch_snum_info_type*
rf_dispatch_snum_pop_item
(
  rf_dispatch_snum_pool_type *pool_data
)
{
  rf_dispatch_snum_node_type *found = NULL; /* Var to store the item */
  boolean snum_found = TRUE;

  /* NULL pointer check */
  if ( pool_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_snum_pop_item: NULL Pool Data" ); 
    return NULL;
  } /* if ( pool_data == NULL ) */

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(pool_data->critical_sect) );

  /*--------------------------------------------------------------------------*/
  /* Get a spare item from sentinel_spare_items */
  /*--------------------------------------------------------------------------*/

  /* NULL pointer check for Spare item pointer */
  if ( ( pool_data->sentinel_spare_items != NULL ) &&
       ( pool_data->sentinel_used_items != NULL ) )
  {
    /* Check if the "Spare" list has atleast one item */
    if ( pool_data->sentinel_spare_items->next != 
         pool_data->sentinel_spare_items
       )
    {
      /* There is free item in Spare list. Use that to push to "Used" list */
      found = pool_data->sentinel_spare_items->next;
  
      /* Remove the item from the list */
      rf_dispatch_snum_remove_item( found );
  
      /* Insert it at the head of the used list */
      rf_dispatch_snum_insert_item( pool_data->sentinel_used_items, found );
  
      /* Decrease the number of spare items */
      pool_data->num_of_spare_items--;
    
      snum_found = TRUE;
  
  
    } /* if ( pool_data->sentinel_spare_items->next != 
              pool_data->sentinel_spare_items
            ) */
    else
    {
      /* Spare item not found */
      ERR_FATAL( "rf_dispatch_snum_pop_item: SNUM Node Item Exhausted (Max %d)", 
                 RF_DISPATCH_NUM_MAX_CONCURRENT_RSP, 0, 0 ); 
      snum_found = FALSE;
    } /* if ! ( pool_data->sentinel_spare_items->next != 
                pool_data->sentinel_spare_items
              ) */
  } /* if ( ( pool_data->sentinel_spare_items != NULL ) &&
            ( pool_data->sentinel_used_items != NULL ) ) */
  else
  {
    RF_MSG( RF_MED, "rf_dispatch_snum_pop_item: NULL Spare/Used pointer" );
  } /* if ( ( pool_data->sentinel_spare_items != NULL ) &&
          ( pool_data->sentinel_used_items != NULL ) ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(pool_data->critical_sect) );

  if ( snum_found == TRUE )
  {
    return &(found->snum_info);
  } /* if ( snum_found == TRUE ) */
  else
  {
    return NULL;
  } /* if ! ( snum_found == TRUE ) */

} /* rf_dispatch_snum_pop_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push SNUM item from SNUMPool
 
  @details
  This function will be used to push used SNUM item to
  the SNUMPool.
 
  @param pool_data
  Pointer to the structure containing SNUM Pool Data
 
  @param push_node
  SNUM item Node is to be pushed
 
  @return
  Pointer to the SNUM node info which is poped to be used by client
*/
static boolean
rf_dispatch_snum_push_item
(
  rf_dispatch_snum_pool_type *pool_data,
  rf_dispatch_snum_node_type *push_node
)
{
  boolean ret_val = TRUE;

  /* NULL pointer check */
  if ( pool_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_snum_push_item: NULL Pool Data" ); 
    return FALSE;
  } /* if ( pool_data == NULL ) */

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(pool_data->critical_sect) );

  /* Check if the token is found or not */
  if( push_node == NULL )
  {
    /* Spare item not found */
    ERR_FATAL( "rf_dispatch_snum_push_item: PUSH item not found", 0, 0, 0 );
    ret_val = FALSE;
  } /* if( found == NULL ) */
  else
  {
    /* NULL Pointer check */
    if ( pool_data->sentinel_spare_items != NULL )
    {
      /* Invalidate the SNUM data */
      push_node->snum_info.snum = 0;
      push_node->snum_info.static_cb_registration = FALSE;
      push_node->snum_info.snum_cb = NULL;
      push_node->snum_info.snum_cb_data = NULL;
  
      /* Remove the item from the list */
      rf_dispatch_snum_remove_item( push_node );
      
      /* Insert it at the head of the spare list */
      rf_dispatch_snum_insert_item( pool_data->sentinel_spare_items, 
                                    push_node );
  
      /* Increase the number of spare items */
      pool_data->num_of_spare_items++;
  
      ret_val &= TRUE;
    } /*  if ( pool_data->sentinel_spare_items != NULL ) */
    else
    {
      ret_val &= FALSE;

      RF_MSG( RF_MED, "rf_dispatch_snum_push_item: NULL Spare Pointer" );

    } /*  if ! ( pool_data->sentinel_spare_items != NULL ) */
  } /* if ! ( found == NULL ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(pool_data->critical_sect) );

  return ret_val;

} /* rf_dispatch_snum_push_item */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_HELPER
 
  @brief
  This module holds internal helper functions
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Register callback function used for SNUM dispatching
 
  @details
  This function shall be called to dynamically deregister the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be removed from the row corresponding to the SNUM. With the
  deregistration of callback, the size of the sliding search window will be 
  adjusted (deccrement by 1)
 
  @param pool_data
  Pointer to SNUM pool data
 
  @param push_node
  Pointer to the push node
 
  @param snum
  SequenceNumber for the deregistration. If the message does not have valid
  sequence number, the pass 0 to this field
 
  @return
  Flag indicating the status of callback function deregistration
*/
static boolean
rf_dispatch_snum_deregister
(
  rf_dispatch_snum_pool_type *pool_data,
  rf_dispatch_snum_node_type *push_node,
  uint32 snum
)
{
  boolean ret_val= TRUE;
  boolean push_status = TRUE;

  /* NULL pointer check */
  if ( pool_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_snum_deregister: NULL Pool Data" ); 
    return FALSE;
  } /* if ( pool_data == NULL ) */
  
  /* Based on SNUM number, deregister the callback info in appropriate entry.
  Invalid SNUM (RF_DISPATCH_NO_SNUM) will always be registered at the
  no_snum_info entry. On the other hand, valid SNUM will dynamically
  get entered by poping an SNUM item from SNUMPool */ 
  if ( snum == RF_DISPATCH_NO_SNUM )
  {
    /* Invalid SNUM (RF_DISPATCH_NO_SNUM) will always be registered at
    no_snum_info */
    pool_data->no_snum_info.snum = 0;
    pool_data->no_snum_info.static_cb_registration = FALSE;
    pool_data->no_snum_info.snum_cb = NULL;
    pool_data->no_snum_info.snum_cb_data = NULL;
  } /* if ( snum == RF_DISPATCH_NO_SNUM ) */
  else
  {
    /* NULL Pointer Check */
    if ( push_node != NULL )
    {
      /* PUSH a SNUM item */
      push_status = rf_dispatch_snum_push_item( pool_data, push_node );
  
      if ( push_status == FALSE )
      {  
        ret_val = FALSE;
        RF_MSG_1( RF_ERROR, "rf_dispatch_snum_deregister:  SNUM.0x%x - Failed",
                  snum );
      } /* if ( dup_snum == NULL ) */
    } /* if ( push_node != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_1( RF_ERROR, "rf_dispatch_snum_deregister: "
                "SNUM.0x%x - PUSH item not found", snum );
    } /* if ! ( push_node != NULL ) */
  } /* if ! ( snum == RF_DISPATCH_NO_SNUM ) */

  return ret_val;

} /* rf_dispatch_snum_deregister */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_REG
 
  @brief
  This module exports the interfaces for client to perform registration and
  deregistration of commands for given sequence number. 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register callback function for SNUM dispatching
 
  @details
  This function shall be called to dynamically register the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be saved in the last free row in SNUM dispatcher handler table
  corresponding to the given CID (Command ID). With the registration of
  callback, the size of the sliding search window will be adjusted (increment by 
  1)
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. This handle
  should be queried from the task by cleint before passing to the function
 
  @param cid
  CID (Command ID) pertaining to the SNUM for which the handler callback
  function is to be registered
 
  @param snum
  SequenceNumber for the registration. If the message does not have valid
  sequence number, the pass 0 to this field
 
  @param static_cb_registation
  Flag indicating, if the registraion is static. With static registration, the
  registered command will not be automatically unregistered after dispatching.
  The client is required to perform deregistration explicity. However for 
  dynamic registration, the command will be automatically deregistered after 
  dispatching.
 
  @param snum_handler
  Callback function to be registered for the sequence number
 
  @param snum_data
  Pointer to opaque data to be used by snum_handler function
 
  @param snum_client_debug_info
  Pointer to debug information for second-level dispatching 
  based on sequence number.

  @return
  Flag indicating the status of callback function registration
*/
boolean
rf_dispatch_snum_register_callback
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid,
  uint32 snum,
  boolean static_cb_registation,
  rf_dispatch_snum_handler_fp_type snum_handler,
  void *snum_data,
  rf_dispatch_snum_client_debug_info *snum_client_debug_info
)
{
  rf_dispatch_cid_info_type *cid_info = NULL;
  rf_dispatch_snum_pool_type *pool_data = NULL;
  rf_dispatch_snum_node_type *dup_snum = NULL;
  rf_dispatch_snum_info_type *snum_info = NULL;
  boolean ret_val= TRUE;
  boolean init_ret = TRUE;
  rf_dispatch_snum_debug_info snum_debug_info;

  /* Param validation */
  if ( dispatch_handle == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_dispatch_snum_register_callback: NULL Task "
                        "Dispatch Handle - CID 0x%x", cid );
    return FALSE;
  } /* if ( dispatch_handle == NULL ) */

  /* Get CID (Command ID) info from the linked list*/
  cid_info = rf_dispatch_get_cid_info( dispatch_handle, cid );

  /* Perform NULL pointer check before proceeding */
  if ( cid_info != NULL )
  {
    /* Get Pool data */
    pool_data = 
      (rf_dispatch_snum_pool_type*)(cid_info->secondary.dispatch_data);
    
    /* If Pool data is NULL, this means that pool has not been initialized
    yet. Thus perform initialization */
    if ( pool_data == NULL )
    {
      if ( rf_dispatch_snum_pool_init( 
           &(cid_info->secondary.dispatch_data) ) == TRUE )
      {
        cid_info->secondary.deinit_fp = rf_dispatch_snum_pool_deinit;

        /* Get Pool data */
        pool_data = 
          (rf_dispatch_snum_pool_type*)(cid_info->secondary.dispatch_data);

      } /* if ( rf_dispatch_snum_pool_init( &pool_data ) == TRUE ) */
      else
      {
        init_ret = FALSE;
        ret_val = FALSE;
      } /* if ! ( rf_dispatch_snum_pool_init( &pool_data ) == TRUE ) */
    } /* if ( pool_data == NULL ) */
    
    /* NULL pointer check - init_ret should be TRUE */
    if ( ( pool_data != NULL ) && ( init_ret == TRUE ) )
    {

      /* Based on SNUM number, register the callback info in appropriate entry.
      Invalid SNUM (RF_DISPATCH_NO_SNUM) will always be registered at the
      no_snum_info entry. On the other hand, valid SNUM will dynamically
      get entered by poping an SNUM item from SNUMPool */ 
      if ( snum == RF_DISPATCH_NO_SNUM )
      {
        /* Invalid SNUM (RF_DISPATCH_NO_SNUM) will always be registered at
        no_snum_info */
        pool_data->no_snum_info.snum = RF_DISPATCH_NO_SNUM;
        pool_data->no_snum_info.static_cb_registration = static_cb_registation;
        pool_data->no_snum_info.snum_cb = snum_handler;
        pool_data->no_snum_info.snum_cb_data = snum_data;

        if ( dispatch_handle->debug_mode == TRUE )
        {
          RF_MSG_2( RF_MED, "rf_dispatch_snum_register_callback: [ 0x%x ] "
                    "CID.0x%x NoSNUM - Registred", dispatch_handle->task_uid,
                     cid );
        } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
      } /* if ( snum == RF_DISPATCH_NO_SNUM ) */
      else
      {
        /* Find if the SNUM is already registered */
        dup_snum = rf_dispatch_snum_find_item( pool_data, snum );

        /* NULL means that the item is not found. */
        if ( dup_snum == NULL )
        {
          /* POP a SNUM item */
          snum_info = rf_dispatch_snum_pop_item( pool_data );

          /* Check if an item was successfully pop'ed or not */
          if ( snum_info != NULL )
          {
            snum_info->snum = snum;
            snum_info->static_cb_registration = static_cb_registation;
            snum_info->snum_cb = snum_handler;
            snum_info->snum_cb_data = snum_data;

            if(snum_client_debug_info !=  NULL)
            {
              /*Call the Client's Debug handler*/
              if (snum_client_debug_info->client_cb != NULL)
              {
                snum_debug_info.snum = snum;
                snum_debug_info.num_of_spare_snums = pool_data->num_of_spare_items;
                snum_client_debug_info->client_cb(snum_client_debug_info->client_debug_info,snum_debug_info);
              }
            }
            if ( dispatch_handle->debug_mode == TRUE )
            {
              RF_MSG_4( RF_MED, "rf_dispatch_snum_register_callback: "
                        "[ 0x%x ] CID.0x%x SNUM.0x%x - Registered @ ITEM.%d", 
                        dispatch_handle->task_uid, cid, snum, 
                        snum_info->item_id );
            } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
          } /* if ( snum_info != NULL ) */
          else
          {
            /* This is unrecoverable condition. Thus error fatal ! */
            ret_val = FALSE;
            ERR_FATAL( "rf_dispatch_snum_register_callback: [ 0x%x ] "
                       "CID.0x%x SNUM.0x%x - No free SNUM Index", 
                       dispatch_handle->task_uid, cid, snum );
          } /* if ! ( snum_info != NULL ) */
        } /* if ( dup_snum == NULL ) */
        else
        {
          ret_val = FALSE;
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_4( RF_ERROR, "rf_dispatch_snum_register_callback: "
                          "[ %s ] CID.0x%x SNUM.0x%x - Duplicate @ ITEM.%d", 
                          dispatch_handle->task_name, cid, snum, 
                          dup_snum->snum_info.item_id );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_4( RF_ERROR, "rf_dispatch_snum_register_callback: [ 0x%x ] "
                      "CID.0x%x SNUM.0x%x - Duplicate @ ITEM.%d", 
                      dispatch_handle->task_uid, cid, snum, 
                      dup_snum->snum_info.item_id );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if ( dup_snum == NULL ) */
      } /* if ! ( snum == RF_DISPATCH_NO_SNUM ) */
    } /* if ( pool_data != NULL ) */
    else
    {
      ret_val = FALSE;
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_4( RF_ERROR, "rf_dispatch_snum_register_callback: [ %s ] "
                      "[# %d] CID.0x%x SNUM.0x%x - NULL Pool malloc failed", 
                      dispatch_handle->task_name, cid_info->index_num, 
                      cid_info->cid, snum ); 
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_4( RF_ERROR, "rf_dispatch_snum_register_callback: [ 0x%x ] "
                  "[# %d] CID.0x%x SNUM.0x%x - NULL Pool malloc failed", 
                  dispatch_handle->task_name, cid_info->index_num, 
                  cid_info->cid, snum ); 
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ! ( pool_data != NULL ) */
  } /* if ( cid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_3( RF_ERROR, "rf_dispatch_snum_register_callback: [ %s ] "
                    "CID.0x%x SNUM.0x%x - No CID info", 
                    dispatch_handle->task_name, cid, snum );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_3( RF_ERROR, "rf_dispatch_snum_register_callback: [ 0x%x ] "
                "CID.0x%x SNUM.0x%x - No CID info", dispatch_handle->task_uid, 
                cid, snum );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( cid_info != NULL ) */

  return ret_val;

} /* rf_dispatch_snum_register_callback */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Register callback function used for SNUM dispatching
 
  @details
  This function shall be called to dynamically deregister the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be removed from the row corresponding to the SNUM. With the
  deregistration of callback, the size of the sliding search window will be 
  adjusted (deccrement by 1)
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. This handle
  should be queried from the task by cleint before passing to the function
 
  @param cid
  CID (Command ID) pertaining to the SNUM for which the handler callback
  function is to be deregistered
 
  @param snum
  SequenceNumber for the deregistration. If the message does not have valid
  sequence number, the pass 0 to this field 
   
  @param snum_cb_data_ptr 
  Pointer to the original payload data that gets passed in when snum callback 
  gets registered. This allows the caller to obtain the location of payload for 
  possible clean up. If there is no such need to obatain snum_cb_data, pass 
  in NULL 
 
  @return
  Flag indicating the status of callback function deregistration
*/
boolean
rf_dispatch_sum_deregister_callback
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid,
  uint32 snum,
  void** snum_cb_data_ptr
)
{
  rf_dispatch_cid_info_type *cid_info;
  rf_dispatch_snum_node_type *push_node;
  rf_dispatch_snum_pool_type *pool_data = NULL;
  boolean ret_val= TRUE;

  /* Param validation */
  if ( dispatch_handle == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_dispatch_sum_deregister_callback: NULL Task "
                        "Dispatch Handle - CID 0x%x", cid );
    return FALSE;
  } /* if ( dispatch_handle == NULL ) */

  /* Get CID (Command ID) info from the linked list*/
  cid_info = rf_dispatch_get_cid_info( dispatch_handle, cid );

  /* Perform NULL pointer check before proceeding */
  if ( cid_info != NULL )
  {
    /* Get Pool data */
    pool_data = 
      (rf_dispatch_snum_pool_type*)(cid_info->secondary.dispatch_data);

    /* Find the SNUM entry */
    if ( pool_data != NULL )
    {
      push_node = rf_dispatch_snum_find_item( pool_data, snum );

      if( snum_cb_data_ptr != NULL )
      {
        /* Return snum call back data from node found */
        if( push_node != NULL )
        {
          *snum_cb_data_ptr = push_node->snum_info.snum_cb_data;
        }

        /* If no_snum, return from no_snum_info */
        if( snum == RF_DISPATCH_NO_SNUM )
        {
          *snum_cb_data_ptr = pool_data->no_snum_info.snum_cb_data;
          RF_MSG_1( RF_MED, "NOSNUM cb data at 0x%0x8x", 
                    pool_data->no_snum_info.snum_cb_data );
        }
      }/* if( snum_cb_data_ptr != NULL ) */

      ret_val = rf_dispatch_snum_deregister( pool_data, push_node, snum );

      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_3( RF_MED, "rf_dispatch_sum_deregister_callback: "
                      "[ %s ] CID.0x%x SNUM.0x%x - Deregistered", 
                      dispatch_handle->task_name, cid, snum );
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_3( RF_MED, "rf_dispatch_sum_deregister_callback: "
                  "[ 0x%x ] CID.0x%x SNUM.0x%x - Deregistered", 
                  dispatch_handle->task_uid, cid, snum );
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ( pool_data != NULL ) */
    else
    {
      ret_val = FALSE;
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_MSG_3( RF_ERROR, "rf_dispatch_sum_deregister_callback: [ %s ] "
                  "CID.0x%x SNUM.0x%x - No Pool info", 
                  dispatch_handle->task_name, cid, snum );
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_3( RF_ERROR, "rf_dispatch_sum_deregister_callback: [ 0x%x ] "
                  "CID.0x%x SNUM.0x%x - No Pool info", 
                  dispatch_handle->task_uid, cid, snum );
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ! ( pool_data != NULL ) */
  } /* if ( cid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_2( RF_ERROR, "rf_dispatch_sum_deregister_callback: [ %s ] "
                    "SNUM.0x%x - No CID info", dispatch_handle->task_name, 
                    snum );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_2( RF_ERROR, "rf_dispatch_sum_deregister_callback: [ 0x%x ] "
                "SNUM.0x%x - No CID info", dispatch_handle->task_uid, snum );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( cid_info != NULL ) */

  return ret_val;

} /* rf_dispatch_sum_deregister_callback */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_SECONDARY_SNUM_DISPATCH
 
  @brief
  This module holds the dispatcher code implementation. This will dispatch the
  incoming command to the appropriate sequence number callback function 
  registered for the given command, task, and sequence number.
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the commands by performing secondary dispatching using snum
 
  @details
  This funciton will perform secondary dispatching based on sequence number.
  The callback function will be registered the dispatched based on the command
  ID and the sequence number.
 
  @param cmd_ptr
  Pointer to the memory containing the command data
 
  @param cid_info
  Pointer to the data structure containing the command ID info
 
  @param snum
  Sequence number for which the dispatching is to be done
 
  @return
  Flag indicating the status of dispatching
*/
boolean
rf_dispatch_snum
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  uint32 snum
)
{
  boolean ret_val = TRUE;
  rf_dispatch_snum_node_type *snum_node = NULL;
  rf_dispatch_snum_pool_type *pool_data = NULL;
  const rf_dispatch_snum_info_type *snum_data = NULL;
  rf_dispatch_snum_handler_fp_type snum_cb;
  boolean static_cb_registration;

  /* Check if the callback is present */
  if ( cid_info != NULL )
  {
    /* Get Pool data */
    pool_data = 
      (rf_dispatch_snum_pool_type*)(cid_info->secondary.dispatch_data);

    /* Peform NULL pointer check */
    if ( pool_data != NULL )
    {
      if ( snum == RF_DISPATCH_NO_SNUM )
      {
        /* Get the SNUM data and SNUM callback */
        snum_data = &(pool_data->no_snum_info);
        snum_cb = pool_data->no_snum_info.snum_cb;
        static_cb_registration = pool_data->no_snum_info.static_cb_registration;
        } /* if ( snum == RF_DISPATCH_NO_SNUM ) */
      else
      {
        /* Find the SNUM entry */
        snum_node = rf_dispatch_snum_find_item( pool_data, snum );
    
        /* NULL Pointer check */
        if ( snum_node != NULL )
        {
          /* Get the SNUM data and SNUM callback */
          snum_data = &(snum_node->snum_info);
          snum_cb = snum_node->snum_info.snum_cb;
          static_cb_registration = snum_node->snum_info.static_cb_registration;
        } /* if ( snum_node != NULL ) */
        else
        {
          snum_cb = NULL;
          snum_data = NULL;
          RF_MSG_2( RF_ERROR, "rf_dispatch_snum: CID.0x%x SNUM.0x%x - No "
                              "SNUM Entry", cid_info->cid, snum );
        } /* if ! ( snum_node != NULL ) */
      } /* if ! ( snum == RF_DISPATCH_NO_SNUM ) */

      /* NULL Pointer check */
      if ( snum_cb != NULL )
      {
        /* Call the callback function */
        ret_val &= snum_cb( cmd_ptr, snum_data );

        /* Deregister SNUM Callback */
        if ( static_cb_registration == FALSE )
        {
          ret_val &= rf_dispatch_snum_deregister( pool_data, snum_node, snum );
        } /* if ( snum_node->snum_info.static_cb_registration == FALSE ) */
      } /* if ( snum_node->snum_info.snum_cb != NULL ) */
      else
      {
        RF_MSG_2( RF_HIGH, "********* rf_dispatch_snum: "
                  "CID.0x%x SNUM.0x%x - No Handler Found *********", 
                  cid_info->cid, snum );
      } /* if ! ( snum_node->snum_info.snum_cb != NULL ) */
    } /* if ( pool_data != NULL ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_dispatch_snum: SNUM.0x%x - No "
                          "Pool Data", snum );
    } /* if ! ( pool_data != NULL ) */
  } /* if ( cid_info->cid_cb != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_dispatch_snum: SNUM.0x%x - No "
                        "CID Entry", snum );
  } /* if ! ( cid_info->cid_cb != NULL ) */

  /* Return from the dispatcher */
  return ret_val;
 
} /* rf_dispatch_snum */

/*! @} */


