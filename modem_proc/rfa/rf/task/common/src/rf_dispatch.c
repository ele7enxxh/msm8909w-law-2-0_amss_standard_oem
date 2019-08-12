/*! 
  @file
  rf_dispatch.c
 
  @brief
  This file includes common dispatch implementation for all task used by RFSW
*/
 
/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_dispatch.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/11/13   Saul    Cal Time Reduction. Inlined some APIs and removed some F3s.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/05/13   sty     Reduced message levels
04/15/13   aro     Added CID dispatch logging
03/22/13   zhw     Update rf_dispatch_command API
01/16/13   aro     Renamed handler to handle
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     Documentation Update
01/15/13   aro     Updated registration interface to pass dispatch handle
01/10/13   aro     Doxygen documentation update
01/09/13   aro     Added registration action mask in command registration
01/09/13   aro     Renamed UMID to CID
01/09/13   aro     Added support for RF_TASK
01/08/13   aro     Fixed NULL pointer access
01/08/13   aro     Rex data query and CID extraction functions added for
                   RF FW response task
01/08/13   aro     F3 update
01/08/13   aro     Added deinitialization of secondary dispatching during
                   CID deregistration
01/07/13   aro     Interface to get CID information
01/07/13   aro     Renamed function pointer type name
01/07/13   aro     Doxygen Update
01/07/13   aro     Type name update
01/07/13   aro     Command dispatch function for spawned tasks
01/07/13   aro     Added func pointer to extarct CID from command
01/07/13   aro     Implementation of CID deregistration
01/07/13   aro     variable to indicate if the CID is registered wih MSGR
01/07/13   aro     Flag indicating if a task is registered to the dispatcher
01/07/13   aro     Implementation of CID registration function
01/07/13   aro     Added helper function to create and fetch tech nodes
01/07/13   aro     Interface to perform CID deregistration
01/07/13   aro     Interface to perform CID registration
01/04/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rf_dispatch.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "msgr_rex.h"
#include "stringl.h" /* for memscpy */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_DEBUG
 
  @brief
  This module contains the helper functions used for debug logging of
  RF Dispatch.
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Log the Dispatch info
 
  @details
  This function will log the debug data when the command is dispatched.
 
  @param cid_info
  Pointer to str containing CID info
*/
void
rf_dispatch_debug_dispatch_log
(
  rf_dispatch_cid_info_type *cid_info
)
{
  if ( cid_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_debug_dispatch_log: NULL CID info" );
    return;
  } /* if ( cid_info == NULL ) */

  /* Increment the counter */
  cid_info->dbg_data.curr_idx++;

  /* Make sure that current index does not exceed RF_DISPATCH_CID_DBG_MAX_LEN */
  if ( cid_info->dbg_data.curr_idx >= RF_DISPATCH_CID_DBG_MAX_LEN )
  {
    cid_info->dbg_data.curr_idx = 0;
  } /* if ( cid_info->dbg_data.curr_idx >= RF_DISPATCH_CID_DBG_MAX_LEN ) */

  cid_info->dbg_data.dispatch_time[cid_info->dbg_data.curr_idx] = 
                                                      rf_time_get_tick();

  return;

} /* rf_dispatch_debug_dispatch_log */
/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_MAIN_DISPATCH_REX
 
  @brief
  This module contains the implementation of Rex-MessageRouter registration.
  The functions in this module will be called if the command ID registration
  requires rex-msgr registration as additional third party registration.
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rex MSGR interface initialization

  @details
  This funciton creates the message router client on the Task TCB, registers 
  for all the MSGR commands that the gievn task wishes to recieve.
 
  Create message queue to receive message router messages from other SW
  entities. RF is a REX Task and it waits on REX signals. Hence a REX message 
  queue has to be added to the message router client.

  @param msgr_data
  Structure containing message router data
 
  @param rex_data
  Structure containing rex data
 
  @return
  Flag indicating, if clients are successfully created or not.
*/
static boolean 
rf_dispatch_rex_msgr_init
(
  rf_dispatch_msgr_data_type *msgr_data,
  rf_dispatch_rex_data_type *rex_data
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create RF CDMA FWRSP Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_create(&(msgr_data->client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_rex_msgr_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  } /* if ( msgr_status != E_SUCCESS ) */

  if (msgr_status == E_SUCCESS)
  {
    /* Add rex Q */
    msgr_status = msgr_client_add_rex_q( &(msgr_data->client),
                                         rex_data->rex_tcb,
                                         rex_data->rex_sig, 
                                         rex_data->cmd_q,
                                         rex_data->free_q,
                                         rex_data->header_size,
                                         rex_data->q_link_size,
                                         rex_data->command_size,
                                         &(msgr_data->mailbox) );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rf_dispatch_rex_msgr_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    } /* if (msgr_status != E_SUCCESS) */
  } /* if (msgr_status == E_SUCCESS) */

  /* Return Status */
  return ret_val;

} /* rf_dispatch_rex_msgr_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rex MSGR interface de-initialization

  @details
  This funciton deletes the message router client on the Task TCB.

  @param msgr_data
  Structure containing message router data
  
  @return
  Flag indicating, if clients are successfully destroyed or not.
*/
static boolean 
rf_dispatch_rex_msgr_deinit
(
  rf_dispatch_msgr_data_type *msgr_data
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create RF CDMA FWRSP Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_delete(&(msgr_data->client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_rex_msgr_deinit : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  } /* if ( msgr_status != E_SUCCESS ) */

  /* Return Status */
  return ret_val;

} /* rf_dispatch_rex_msgr_deinit */


/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_MAIN_DISPATCH_LL
 
  @brief
  This module contains the details of the linked list implementation. Linked
  list are used to store the command registration data. Each node in the
  linked list pertains to a technology (8 MSB of command ID, CID (Command ID)).
  All the CID (Command ID)s pertaining to the technology will be bundled in a
  linked list node
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Traverse through the linked list
 
  @details
  This function will traverse through the dispatch node linked list to find the
  node corresponding to the given tech. If no node is found, NULL will be
  returned.
 
  @param head_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech comparison token to match the node.
 
  @return
  Pointer to the Node corresponding to the given tech
*/
INLINE rf_dispatch_tech_node_type*
rf_dispatch_get_tech_node
(
  rf_dispatch_tech_node_type *head_node,
  uint8 tech
)
{
  rf_dispatch_tech_node_type *node_ptr = NULL;
  rf_dispatch_tech_node_type *match_node = NULL;
  
  /* Intialize the current node with head node value */
  node_ptr = head_node;
  
  /* Proceed only if head_node passsed in not NULL */
  if ( node_ptr != NULL )
  {
    do /* ( curr_node != NULL ) */
    {
      if ( node_ptr->tech_cids.tech == tech )
      {
        match_node = node_ptr;
        break;
      } /* if ( curr_node->tech_cids.tech == tech ) */
  
      /* Traverse to next node */
      node_ptr = node_ptr->next;

    } while ( node_ptr != NULL );
  } /* if ( curr_node != NULL ) */

  /* return the match node */
  return match_node;

} /* rf_dispatch_get_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create new tech Node
 
  @details
  This function will traverse through the dispatch node linked list to find the
  last node and will create a new node.
 
  @param head_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech for which the node is to be created.
 
  @return
  Pointer to the new Node corresponding to the given tech
*/
static rf_dispatch_tech_node_type*
rf_dispatch_create_tech_node
(
  rf_dispatch_tech_node_type **head_node,
  uint8 tech
)
{
  rf_dispatch_tech_node_type *curr_node = NULL;
  rf_dispatch_tech_node_type *last_node = NULL;
  rf_dispatch_tech_node_type *ret_val = NULL;

  /* Intialize the current node with head node value */
  curr_node = *head_node;
  last_node = *head_node;

  /* Proceed only if head_node passsed in not NULL */
  if ( *head_node == NULL )
  {
    *head_node = modem_mem_alloc( sizeof(rf_dispatch_tech_node_type),
                                  MODEM_MEM_CLIENT_RFA );
    if ( *head_node == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_dispatch_create_tech_node: TECH.0x%x - Head node "
                        "Malloc failed", tech );
      ret_val = NULL;
    } /* if ( *linked_list == NULL ) */
    else
    {
      memset( *head_node, 0x0, sizeof(rf_dispatch_tech_node_type) );
      (*head_node)->next = NULL;
      (*head_node)->tech_cids.tech = tech;
      (*head_node)->tech_cids.num_of_cids = 0;
      (*head_node)->tech_cids.cid_list = NULL;
      ret_val = *head_node;
      RF_MSG_1( RF_MED, "rf_dispatch_create_tech_node: TECH.0x%x - "
                        "Head Node created", tech );
    } /* if ! ( *linked_list == NULL ) */
  }
  else
  {
    do /* ( curr_node != NULL ) */
    {
      if ( curr_node->next == NULL )
      {
        last_node = curr_node;
        break;
      } /* if ( curr_node->next == NULL ) */
  
      /* Traverse to next node */
      curr_node = curr_node->next;

    } while ( curr_node != NULL );

    last_node->next = modem_mem_alloc( sizeof(rf_dispatch_tech_node_type),
                                       MODEM_MEM_CLIENT_RFA );
    if ( last_node->next == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_dispatch_create_tech_node: TECH.0x%x - "
                          "Node Malloc failed", tech );
      ret_val = NULL;
    } /* if ( *linked_list == NULL ) */
    else
    {
      memset( last_node->next, 0x0, sizeof(rf_dispatch_tech_node_type) );
      last_node->next->next = NULL;
      last_node->next->tech_cids.tech = tech;
      last_node->next->tech_cids.num_of_cids = 0;
      last_node->next->tech_cids.cid_list = NULL;
      ret_val = last_node->next;
      RF_MSG_1( RF_MED, "rf_dispatch_create_tech_node: TECH.0x%x - "
                        "Node created", tech );
    } /* if ! ( *linked_list == NULL ) */
  }

  /* return the Tech node */
  return ret_val;

} /* rf_dispatch_create_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Delete tech Node
 
  @details
  This function will traverse through the dispatch node linked list to find the
  node corresponding to the given tech to delete.
 
  @param head_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech for which the node is to be deleted.
 
  @return
  Flag indicating the status of Tech Node deletion
*/
static boolean
rf_dispatch_delete_tech_node
(
  rf_dispatch_tech_node_type **head_node,
  uint8 tech
)
{
  rf_dispatch_tech_node_type *curr_node = NULL;
  rf_dispatch_tech_node_type *match_node = NULL;
  rf_dispatch_tech_node_type *match_n_1_node = NULL;
  boolean ret_val = TRUE;

  /* Intialize the current node with head node value */
  curr_node = *head_node;

  /* Proceed only if head_node passsed in not NULL */
  if ( *head_node != NULL )
  {
    do /* ( curr_node != NULL ) */
    {
      if ( curr_node->tech_cids.tech == tech )
      {
        match_node = curr_node;
        break;
      } /* if ( curr_node->tech_cids.tech == tech ) */
  
      /* Store the current node */
      match_n_1_node = curr_node;

      /* Traverse to next node */
      curr_node = curr_node->next;

    } while ( curr_node != NULL );

    /* Check if a  match is found. For the case where match is found, set
    the NEXT pointers in linked list and delete the node */
    if ( match_node != NULL )
    {
      if ( match_node == *head_node )
      {
        /* Case when head node is to be removed */
        *head_node = match_node->next;
      } /* if ( match_node == *head_node ) */
      else 
      {
        /* Case when non-head node is to be removed */
        match_n_1_node->next = match_node->next;
      } /* if ! ( match_node == *head_node ) */
    
      /* Dellocate the memory for the given tech node */
      modem_mem_free( match_node, MODEM_MEM_CLIENT_RFA );
      match_node = NULL;
    } /* if ( match_node != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_1( RF_FATAL, "rf_dispatch_delete_tech_node: "
                          "Tech 0x%x node not found", tech );
    } /* if ! ( match_node != NULL ) */

  } /* if ( head_node != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_FATAL, "rf_dispatch_delete_tech_node: Tech 0x%x NULL head",
              tech );
  } /* if ! ( head_node != NULL ) */

  return ret_val;

} /* rf_dispatch_delete_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query CID (Command ID) dispatcher info
 
  @details
  This function will locate the linked list node corresponding to the tech and
  locate the appropriate CID (Command ID) entry.
 
  @param dispatch_handle
  Pointer to the dipatch data
 
  @param cid
  Messsage router CID (Command ID) for which the entry is to be located
 
  @return
  Pointer to the memory location which stores CID (Command ID) entry
*/
INLINE rf_dispatch_cid_info_type*
rf_dispatch_fetch_cid_info
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid
)
{
  rf_dispatch_tech_node_type *tech_node = NULL;
  rf_dispatch_cid_info_type *cid_info = NULL;
  rf_dispatch_cid_info_type *cid_start_ptr = NULL;
  rf_dispatch_cid_info_type *cid_ptr = NULL;
  uint16 cid_idx = 0;

  /* Param validation */
  if ( dispatch_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_fetch_cid_info: NULL Task Handle " );    
    return NULL;
  } /* if ( dispatch_handle == NULL ) */

  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_dispatch_get_tech_node( dispatch_handle->head_node,
                                         MSGR_TECH_VAL(cid) );

  /* Perform NULL pointer check. */
  if ( tech_node != NULL )
  {
    /* Set the CID (Command ID) Pointer */
    cid_start_ptr = (tech_node->tech_cids.cid_list);

    /* Loop through all entries to locate the CID (Command ID) entry */
    for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids;
          cid_idx++ )
    {
      cid_ptr = cid_start_ptr + cid_idx;
      if ( cid_ptr->cid == cid )
      {
        cid_info = cid_ptr;
        if ( dispatch_handle->debug_mode == TRUE )
        {
          RF_MSG_3( RF_MED, "rf_dispatch_fetch_cid_info: [ 0x%x ] CID.0x%x - "
                    "Found @ index %d", dispatch_handle->task_uid, cid,
                    cid_idx );
        } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        break;
      } /* if ( cid_ptr->cid == cmid ) */
    } /* for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids;
          cid_idx++ ) */
  } /* if ( tech_node != NULL ) */
  else
  {
    cid_info = NULL;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_MSG_2( RF_ERROR, "rf_dispatch_fetch_cid_info: [ 0x%x ] CID.0x%x - "
                "No Tech Node", dispatch_handle->task_uid, cid );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( tech_node != NULL ) */

  /* Retrn CID (Command ID) INFO */
  return cid_info;

} /* rf_dispatch_fetch_cid_info */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_INIT
 
  @brief
  This module exports the interfaces for Tasks to perform
  Dispatch Handle initialization

  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the dispatch handle for a task
 
  @details
  This function will be called by each task to initialize the dispatch handle.
  This handle will be used by clients to perform command registration,
  deregistration and disptaching.
 
  RF Dispatch module will not store any data. The Dispatch data should be
  maintained by the individual task it self.
 
  @param dispatch_handle
  Pointer to the memory allocated by the TASK where the dispatch handle data
  is stored
 
  @param init_data
  Initialization data used for to initialize dispatch handle
 
  @return
  Flag indicating, if the initialization is succesful.
*/
boolean
rf_dispatch_init_handle
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_init_handle_data_type init_data
)
{
  boolean ret_val = TRUE; /* Return value */

  /* Parameter validation */
  if ( dispatch_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_init_handle: NULL Task Handle " );    
    return FALSE;
  } /* if ( dispatch_handle == NULL ) */

  /* Store Task name */
  memscpy( &(dispatch_handle->task_name), sizeof(char)*RF_DISPATCH_TASK_NAME_LEN,
           init_data.task_name, sizeof(char)*RF_DISPATCH_TASK_NAME_LEN );

  /* Store Task unique ID */
  dispatch_handle->task_uid = init_data.task_uid;

  /* Store Rex Data */
  memscpy( &(dispatch_handle->rex_data), sizeof(rf_dispatch_rex_data_type),
           &(init_data.rex_data), sizeof(rf_dispatch_rex_data_type) );

  /* Store function pointer to extract command ID */
  dispatch_handle->extract_cid_fp = init_data.extract_cid_fp;

  /* Update debug flag */
  dispatch_handle->debug_mode = init_data.debug_mode;

  /* Flag that teh dispatch is initialized */
  dispatch_handle->dispatch_init_done = TRUE;

  return ret_val;

} /* rf_dispatch_init_handle */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_REG_DREG
 
  @brief
  This module exports the interfaces for client to perform
  registration and deregistration of commands with the command dispatcher.
  Dispatcher module has dispatch data instances for each task which
  uses the dispatcher.
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register CID (Command ID) to the task dispatcher
 
  @details
  This function shall be called to register a command to the given task.
  The CID (Command ID) passed to this function will be first registered with
  message router module. A new linked list node will be created for each
  CID (Command ID) tech. All CID (Command ID) for a given tech will be 
  registered in the same linked list node.
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. This handle
  should be queried from the task by cleint before passing to the function
 
  @param module_self
  MSGR module corresponding to the client which is attempting to perform
  command registration
 
  @param cid
  MSGR ID which is to be regsitered with the MSGR module and the given task
 
  @param cid_handler
  Function pointer pointing to the function which is to be called when
  dispatching for the given CID (Command ID) is to be done.
 
  @param cb_data
  Opaque Pointer to the command ID callback data
 
  @param reg_action
  16-bit mask indicating the type of additional third party registration 
  to be done on the command ID by RF dispatch module. Each bit field is defined 
  by #rf_dispatch_registration_action_enum. For instance, if bit 0 of this
  is set (RF_DISPATACH_MSGR_REG), then this means that the RF dispatch module
  will perform message router initialization internally.
 
  @return
  Flag indicating the status of commond registration
*/
boolean
rf_dispatch_register_command
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_module_type module_self,
  rf_dispatch_cid_type cid,
  rf_dispatch_cid_handler_fp_type cid_handler,
  void *cb_data,
  rf_dispatch_registration_action reg_action
)
{
  rf_dispatch_tech_node_type *tech_node = NULL;
  boolean ret_val = TRUE;
  uint16 cid_list_size = 0;
  rf_dispatch_cid_info_type *cid_start_ptr = NULL;
  rf_dispatch_cid_info_type *cid_ptr = NULL;
  uint16 cid_idx = 0;
  boolean duplicate_cid = FALSE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */  
  boolean msgr_reg_done = FALSE;

  /* Perform NULL pointer check */
  if ( dispatch_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_register_command: NULL Task Handle " );    
    return FALSE;
  } /* if ( dispatch_handle == NULL ) */

  /*------------ all good to proceed -----------------------------------------*/

  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_dispatch_get_tech_node( dispatch_handle->head_node,
                                         MSGR_TECH_VAL(cid) );

  /* If NULL, this means that the node is not present, need to create
  new node */
  if ( tech_node == NULL )
  {
    /* Since the tech node does not exist, create a new new one at the
    end of the linked list */
    tech_node = rf_dispatch_create_tech_node( &(dispatch_handle->head_node),
                                              MSGR_TECH_VAL(cid) );
  } /* if ( tech_node == NULL ) */

  /* Proceed, only if the tech node was successfully created */
  if ( tech_node != NULL )
  {
    /* Perform Duplicate CID (Command ID) entry check */
    if ( tech_node->tech_cids.cid_list != NULL )
    {
      /* Set the CID (Command ID) pointer to the point to correct CID (
      Command ID) entry location */
      cid_start_ptr = (tech_node->tech_cids.cid_list);
      
      /* Loop through all entries to make sure that there is no duplicate
      CID (Command ID) entry */
      for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids;
            cid_idx++ )
      {
        /* Increment the CID (Command ID) pointer */
        cid_ptr = cid_start_ptr + cid_idx;
  
        /* Check for duplicate */
        if ( cid_ptr->cid == cid )
        {
          /* Flag indicating that CID (Command ID) is duplicate */
          duplicate_cid = TRUE;
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_4( RF_ERROR, "rf_dispatch_register_command: [ %s ] "
                          "MOD.0x%x CID.0x%x FP.0x%x - Duplicate CID",
                          dispatch_handle->task_name, module_self, cid,
                          cid_handler );    
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_4( RF_ERROR, "rf_dispatch_register_command: [ 0x%x ] "
                      "MOD.0x%x CID.0x%x FP.0x%x - Duplicate CID",
                      dispatch_handle->task_uid, module_self, cid,
                      cid_handler );    
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
          break;
        } /* if ( cid_ptr->cid == cid ) */
      } /* for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids;
            cid_idx++ ) */
    } /* if ( tech_node->tech_cids.cid_list != NULL ) */

    /* Allocate the memory to store list of CID (Command ID). If it is the 
    first CID (Command ID) for the given tech, then perform allocation. 
    If not, perform reallocation.*/

    /* Compute the size (in bytes) to store all CID (Command ID)s including the 
    latest registration */
    cid_list_size = sizeof(rf_dispatch_cid_info_type) * 
                         (tech_node->tech_cids.num_of_cids + 1);

    /* Perfom allocation or reallocation based if it is first registration. */
    if ( tech_node->tech_cids.cid_list == NULL )
    {
      tech_node->tech_cids.cid_list = 
                modem_mem_alloc( cid_list_size, MODEM_MEM_CLIENT_RFA );
      if ( tech_node->tech_cids.cid_list != NULL )
      {
        memset( tech_node->tech_cids.cid_list, 0x0, cid_list_size );
      } /* if ( tech_node->tech_cids.cid_list != NULL ) */
    } /* if ( tech_node->tech_cids.cid_list == NULL ) */
    else
    {
      /* Perform realloc only if CID (Command ID) is not duplicate */
      if ( duplicate_cid == FALSE )
      {
        /* Increase the  size of CID (Command ID) List*/
        tech_node->tech_cids.cid_list = 
                  modem_mem_realloc( tech_node->tech_cids.cid_list,
                                     cid_list_size,
                                     MODEM_MEM_CLIENT_RFA );

        cid_ptr = tech_node->tech_cids.cid_list;
        cid_ptr += tech_node->tech_cids.num_of_cids;
        memset( cid_ptr, 0x0, sizeof(rf_dispatch_cid_info_type) );
      } /* if ( duplicate_cid == FALSE ) */
    } /* if ! ( tech_node->tech_cids.cid_list == NULL ) */

    /* Perform registration, only if there is no duplicate registration */
    if ( duplicate_cid == FALSE )
    {
      if ( tech_node->tech_cids.cid_list != NULL )
      {
        /* Perform MSGR registration, only if directed */
        if ( (reg_action & (1 << RF_DISPATACH_MSGR_REG))  == TRUE )
        {
          /* Perform REX-MSGR registration only once. */
          if ( dispatch_handle->rex_msgr_reg_done == FALSE )
          {
            /* Return */
            boolean rex_msgr_ret = TRUE; 

            /* Perform initialization */
            rex_msgr_ret &= rf_dispatch_rex_msgr_init( 
                                     &(dispatch_handle->msgr_data),
                                     &(dispatch_handle->rex_data) );

            /* If initialization is successful, flag. This init needs to be
            done only once  */
            if ( rex_msgr_ret == TRUE )
            {
              dispatch_handle->rex_msgr_reg_done = TRUE;
            } /* if ( rex_msgr_ret == TRUE ) */
            else
            {
              /* Flag the overall registration as a failure */
              ret_val = FALSE;
            } /* if ! ( rex_msgr_ret == TRUE ) */
          } /* if ( dispatch_handle->rex_msgr_reg_done == FALSE ) */

          /* Register with Message router */
          msgr_res = msgr_register( module_self, 
                                    &(dispatch_handle->msgr_data.client), 
                                    dispatch_handle->msgr_data.mailbox,
                                    cid );

          /* Check if the message router registration was successful or not */
          if ( msgr_res != E_SUCCESS )
          {
            if ( dispatch_handle->debug_mode == TRUE )
            {
              RF_STR_MSG_4( RF_ERROR, "rf_dispatch_register_command: [ %s ] "
                            "MOD.0x%x CID.0x%x FP.0x%x - Failed MSGR "
                            "registration", dispatch_handle->task_name, 
                            module_self, cid, cid_handler );    
            } /* if ( dispatch_handle->debug_mode == TRUE ) */
            else
            {
              RF_MSG_4( RF_ERROR, "rf_dispatch_register_command: [ 0x%x ] "
                        "MOD.0x%x CID.0x%x FP.0x%x - Failed MSGR registration",
                        dispatch_handle->task_uid, module_self, cid, 
                        cid_handler );    
            } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
            ret_val = FALSE;
          } /* if ( msgr_res != E_SUCCESS ) */
          else
          {
            /* Indicate that the CID (Command ID) is registered with MSGR */
            msgr_reg_done = TRUE;
          } /* if ( msgr_res != E_SUCCESS ) */
        } /* if ( (reg_action & (1 << RF_DISPATACH_MSGR_REG))  == TRUE ) */
        else
        {
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_4( RF_MED, "rf_dispatch_register_command: [ %s ] "
                          "MOD.0x%x CID.0x%x FP.0x%x - MSGR registration "
                          "skipped", dispatch_handle->task_name, module_self, 
                          cid, cid_handler );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_4( RF_MED, "rf_dispatch_register_command: [ 0x%x ] "
                      "MOD.0x%x CID.0x%x FP.0x%x - MSGR registration skipped",
                      dispatch_handle->task_uid, module_self, cid, 
                      cid_handler );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if ! ( (reg_action & (1 << RF_DISPATACH_MSGR_REG))  == TRUE ) */

        /* Set the CID (Command ID) pointer to the point to correct CID 
        (Command ID) entry location */
        cid_ptr = (tech_node->tech_cids.cid_list);
        cid_ptr += tech_node->tech_cids.num_of_cids;

        /* Update the CID (Command ID) entries */
        cid_ptr->index_num = tech_node->tech_cids.num_of_cids;
        cid_ptr->cid = cid;
        cid_ptr->cid_cb = cid_handler;
        cid_ptr->cid_cb_data = cb_data;
        cid_ptr->msgr_registered = msgr_reg_done;

        /* Increase the number of registered CID (Command ID)s.*/
        tech_node->tech_cids.num_of_cids += 1;

        if ( dispatch_handle->debug_mode == TRUE )
        {
          RF_STR_MSG_5( RF_MED, "rf_dispatch_register_command: [ %s ] "
                        "MOD.0x%x CID.0x%x FP.0x%x - [# %d] Registered !", 
                        dispatch_handle->task_name, module_self, cid, 
                        cid_handler, cid_ptr->index_num );
        } /* if ( dispatch_handle->debug_mode == TRUE ) */
        else
        {
          RF_MSG_5( RF_MED, "rf_dispatch_register_command: [ 0x%x ] "
                    "MOD.0x%x CID.0x%x FP.0x%x - [# %d] Registered !", 
                    dispatch_handle->task_uid, module_self, cid, cid_handler, 
                    cid_ptr->index_num );
        } /* if ! ( dispatch_handle->debug_mode == TRUE ) */

      } /* if ( tech_node->tech_cids.cid_list != NULL ) */
      else
      {
        ret_val = FALSE;
        if ( dispatch_handle->debug_mode == TRUE )
        {
          RF_STR_MSG_4( RF_FATAL, "rf_fwrsp_register_command: [ %s ] MOD.0x%x "
                        "CID.0x%x FP.0x%x - CID List allocaiton failed",
                        dispatch_handle->task_name, module_self, cid, 
                        cid_handler );
        } /* if ( dispatch_handle->debug_mode == TRUE ) */
        else
        {
          RF_MSG_4( RF_FATAL, "rf_fwrsp_register_command: [ 0x%x ] "
                    "MOD.0x%x CID.0x%x FP.0x%x - CID List allocaiton failed",
                    dispatch_handle->task_uid, module_self, cid, cid_handler );
        } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
      } /* if ! ( tech_node->tech_cids.cid_list != NULL ) */

    } /* if ( duplicate_cid == FALSE ) */
    else
    {
      ret_val = FALSE;
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_4( RF_FATAL, "rf_dispatch_register_command: [ %s ] "
                      "MOD.0x%x CID.0x%x FP.0x%x - CID already registered !",
                      dispatch_handle->task_name, module_self, cid, 
                      cid_handler );
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_4( RF_FATAL, "rf_dispatch_register_command: [ 0x%x ] "
                  "MOD.0x%x CID.0x%x FP.0x%x - CID already registered !",
                  dispatch_handle->task_uid, module_self, cid, cid_handler );
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ! ( duplicate_cid == FALSE ) */

  } /* if ( tech_node != NULL ) */
  else
  {
    ret_val = FALSE;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_4( RF_FATAL, "rf_dispatch_register_command: [ %s ] "
                    "MOD.0x%x CID.0x%x FP.0x%x - Tech Node creation failed", 
                    dispatch_handle->task_name, module_self, cid, 
                    cid_handler );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_4( RF_FATAL, "rf_dispatch_register_command: [ 0x%x ] "
                "MOD.0x%x CID.0x%x FP.0x%x - Tech Node creation failed", 
                dispatch_handle->task_uid, module_self, cid, cid_handler );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( tech_node != NULL ) */

  /* Return Registration status */
  return ret_val;

} /* rf_dispatch_register_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register CID (Command ID) to the task
 
  @details
  This function shall be called to deregister a command from a given task.
  The CID (Command ID) passed to this function will be first deregistered
  with message router module. Then the CID (Command ID) and its corresponding
  callback function will be deregistered from appropriate linked list node. If 
  all CID (Command ID)s are deregistered, then the Linked List node will be 
  deallocated.
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. This handle
  should be queried from the task by cleint before passing to the function
 
  @param module_self
  MSGR module corresponding to the client which is attempting to perform
  command deregistration
 
  @param cid
  MSGR ID which is to be deregsitered with the MSGR module and RF_FWRSP task
 
  @return
  Flag indicating the status of commond deregistration
*/
boolean
rf_dispatch_deregister_command
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_module_type module_self,
  rf_dispatch_cid_type cid
)
{
  rf_dispatch_tech_node_type *tech_node = NULL;
  boolean ret_val = TRUE;
  uint16 cid_list_size = 0;
  rf_dispatch_cid_info_type *cid_start_ptr = NULL;
  rf_dispatch_cid_info_type *cid_ptr = NULL;
  rf_dispatch_cid_info_type *insert_cid = NULL;
  uint16 cid_idx = 0;
  boolean dereg_cid_found = FALSE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */  
  uint16 adjusted_index_num = 0;

  /* Perform NULL pointer check */
  if ( dispatch_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_deregister_command: NULL Task Handle " );    
    return FALSE;
  } /* if ( dispatch_handle == NULL ) */

  /*------------ all good to proceed -----------------------------------------*/


  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_dispatch_get_tech_node( dispatch_handle->head_node,
                                         MSGR_TECH_VAL(cid) );

  /* Perform NULL pointer check to find out if the tech node is found or not */
  if ( tech_node != NULL )
  {
    /* Set the CID (Command ID) Pointer */
    cid_start_ptr = (tech_node->tech_cids.cid_list);

    /* Find the entry for the CID (Command ID) in the tech table */
    for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids; 
          cid_idx++ )
    {
      /* Set the CID (Command ID) pointer to the point to correct 
      CID (Command ID) entry location */
      cid_ptr = cid_start_ptr + cid_idx;

      /* Check if the CID (Command ID) entry is found or not. If found, then 
      memset the entry. The entry location for matched CID (Command ID) is 
      saved so that it can be used to copy remaining CID (Command ID) entries 
      after the matched CID (Command ID) to this location. This is done in 
      order to pack all CID (Command ID) entries. After PACKing is done, realloc 
      is done to reduce memory usage.*/
      if ( cid_ptr->cid == cid )
      {
        /* Perform MSGR deregisteted, if it were registered */
        if ( cid_ptr->msgr_registered == TRUE )
        {
          /* De-register with MSGR */
          msgr_res = msgr_deregister( module_self, 
                                     &(dispatch_handle->msgr_data.client), 
                                     cid );

          /* Update adjusted_index_num so reflect the cid_idx corresponding
          to the message to be deregistered */
          adjusted_index_num = cid_idx;

          /* Check if the message router registration was successful or not */
          if ( msgr_res != E_SUCCESS )
          {
            if ( dispatch_handle->debug_mode == TRUE )
            {
              RF_STR_MSG_3( RF_ERROR, "rf_dispatch_deregister_command: "
                            "[ %s ] MOD.0x%x CID.0x%x - MSGR Failure", 
                            dispatch_handle->task_name, module_self, cid );    
            } /* if ( dispatch_handle->debug_mode == TRUE ) */
            else
            {
              RF_MSG_3( RF_ERROR, "rf_dispatch_deregister_command: "
                        "[ 0x%x ] MOD.0x%x CID.0x%x - MSGR Failure", 
                        dispatch_handle->task_uid, module_self, cid );    
            } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
            ret_val = FALSE;
          } /* if ( msgr_res != E_SUCCESS ) */
          else
          {
            /* Indicate that the CID (Command ID) is deregistered with MSGR */
            cid_ptr->msgr_registered = FALSE;
          } /* if ! ( msgr_res != E_SUCCESS ) */
        } /* if ( cid_ptr->msgr_registered == TRUE ) */

        /* Save the CID (Command ID) pointer. This will be used later to copy 
        the remaining CID (Command ID) to this location */
        insert_cid = cid_ptr;

        /* De-initialize the secondary dispatcher, if present */
        if ( cid_ptr->secondary.deinit_fp != NULL )
        {
          ret_val &= cid_ptr->secondary.deinit_fp( &(cid_ptr->secondary) );
        } /* if ( cid_ptr->secondary.deinit_fp != NULL ) */
        
        /* Memset the CID (Command ID) entry */
        memset(insert_cid, 0x0, sizeof(rf_dispatch_cid_info_type));

        /* Flag that CID (Command ID) is found and deregistered. This will be
        used later to pack the UMIS list  */
        dereg_cid_found = TRUE;

      } /* if ( cid_ptr->cid == cid ) */
      else if ( dereg_cid_found == TRUE )
      {
        /* PACK the CID (Command ID) entries */
        memscpy( insert_cid, sizeof(rf_dispatch_cid_info_type), cid_ptr, sizeof(rf_dispatch_cid_info_type) );

        /* Update the index with adjusted index */
        insert_cid->index_num = adjusted_index_num;

        /* Increment adjusted index */
        adjusted_index_num++;

        /* Move to new insert location */
        insert_cid++;
      } /* else if ( rearrange_csid == TRUE ) */
    } /* for ( cid_idx = 0; cid_idx < tech_node->tech_cids.num_of_cids; 
          cid_idx++ ) */

    /* Since CID (Command ID) is found and list of CID (Command ID)s are 
    PACKed, the CID (Command ID) List memory needs to be reduced by 1 entry. If 
    all CID (Command ID)s are unregistered, then complete deallocation is done. 
    If not, then just CID (Command ID) list is shortened */
    if ( dereg_cid_found == TRUE )
    {
      /* Compute the size (in bytes) to store all CID (Command ID)s including 
      the latest de-registration */
      cid_list_size = sizeof(rf_dispatch_cid_info_type) * 
                           (tech_node->tech_cids.num_of_cids - 1);

      if ( cid_list_size > 0 )
      {
        /* Decrese the  size of CID (Command ID) List */
        tech_node->tech_cids.cid_list = 
                  modem_mem_realloc( tech_node->tech_cids.cid_list,
                                     cid_list_size,
                                     MODEM_MEM_CLIENT_RFA );

        /* NULL Pointer check */
        if ( tech_node->tech_cids.cid_list != NULL )
        {
          tech_node->tech_cids.num_of_cids -= 1;
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_3( RF_MED, "rf_dispatch_deregister_command: "
                          "[ %s ] MOD.0x%x CID.0x%x - "
                          "Deregistered ", 
                          dispatch_handle->task_name, module_self, cid );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_3( RF_MED, "rf_dispatch_deregister_command: "
                      "[ 0x%x ] MOD.0x%x CID.0x%x - "
                      "Deregistered ", 
                      dispatch_handle->task_uid, module_self, cid );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if ( tech_node->tech_cids.cid_list != NULL ) */
        else
        {
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                          "[ %s ] MOD.0x%x CID.0x%x - Realloc failed ",
                          dispatch_handle->task_name, module_self, cid );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                      "[ 0x%x ] MOD.0x%x CID.0x%x - Realloc failed ",
                      dispatch_handle->task_uid, module_self, cid );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if ! ( tech_node->tech_cids.cid_list != NULL ) */
      } /* if ( cid_list_size > 0 ) */
      else
      {
        /* De-allocaye the CID List */
        modem_mem_free( tech_node->tech_cids.cid_list,
                        MODEM_MEM_CLIENT_RFA );
        tech_node->tech_cids.cid_list = NULL;

        /* Delete the linked list node corresponding to the Tech */
        if ( rf_dispatch_delete_tech_node( &(dispatch_handle->head_node),
                                           MSGR_TECH_VAL(cid) ) == TRUE )
        {
          /* When head_node is NULL, then the entire task is deregistered */
          if ( dispatch_handle->head_node == NULL )
          {
            /* Perform REX-MSGR deregistration */
            if ( dispatch_handle->rex_msgr_reg_done == TRUE )
            {
              /* Return */
              boolean rex_msgr_ret = TRUE; 

              /* Perform de-initialization */
              rex_msgr_ret = rf_dispatch_rex_msgr_deinit( 
                                            &(dispatch_handle->msgr_data) ); 

              /* If deinitialization is successful, flag. This init needs to be
              done only once  */
              if ( rex_msgr_ret == TRUE )
              {
                dispatch_handle->rex_msgr_reg_done = FALSE;
              } /* if ( rex_msgr_ret == TRUE ) */
              else
              {
                /* Flag the overall registration as a failure */
                ret_val = FALSE;
              } /* if ! ( rex_msgr_ret == TRUE ) */
            } /* if ( dispatch_handle->rex_msgr_reg_done == TRUE ) */

          } /* if ( dispatch_handle->head_node == NULL ) */

          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_3( RF_MED, "rf_dispatch_deregister_command: "
                          "[ %s ] MOD.0x%x CID.0x%x - Deregistered + "
                          "Deallocated ", dispatch_handle->task_name, 
                          module_self, cid );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_3( RF_MED, "rf_dispatch_deregister_command: "
                      "[ 0x%x ] MOD.0x%x CID.0x%x - Deregistered + Deallocated", 
                      dispatch_handle->task_uid, module_self, cid );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if ( rf_dispatch_delete_tech_node( &(dispatch_handle->head_node),
                                     MSGR_TECH_VAL(cid) ) == TRUE ) */
        else
        {
          ret_val = FALSE;
          if ( dispatch_handle->debug_mode == TRUE )
          {
            RF_STR_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                          "[ %s ] MOD.0x%x CID.0x%x - Node Deletion failed ", 
                          dispatch_handle->task_name, module_self, cid );
          } /* if ( dispatch_handle->debug_mode == TRUE ) */
          else
          {
            RF_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                      "[ 0x%x ] MOD.0x%x CID.0x%x - Node Deletion failed ", 
                      dispatch_handle->task_uid, module_self, cid );
          } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
        } /* if !( rf_dispatch_delete_tech_node( &(dispatch_handle->head_node),
                                     MSGR_TECH_VAL(cid) ) == TRUE ) */
      } /* if ! ( cid_list_size > 0 ) */
    } /* if ( dereg_cid_found == TRUE ) */
    else
    {
      ret_val = FALSE;
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                      "[ %s ] MOD.0x%x CID.0x%x - CID not found", 
                      dispatch_handle->task_name, module_self, cid );
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                  "[ 0x%x ] MOD.0x%x CID.0x%x - CID not found", 
                  dispatch_handle->task_uid, module_self, cid );
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ! ( dereg_cid_found == TRUE ) */

  } /* if ( tech_node != NULL ) */
  else
  {
    ret_val = FALSE;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                    "[ %s ] MOD.0x%x CID.0x%x - Tech Node Not found", 
                    dispatch_handle->task_name, module_self, cid );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_3( RF_FATAL, "rf_dispatch_deregister_command: "
                "[ 0x%x ] MOD.0x%x CID.0x%x - Tech Node Not found", 
                dispatch_handle->task_uid, module_self, cid );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( tech_node != NULL ) */
  
  /* Return Registration status */
  return ret_val;
} /* rf_dispatch_deregister_command */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_QUERY
 
  @brief
  This module exports the query interfaces
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query CID (Command ID) dispatcher info
 
  @details
  This function will locate the linked list node corresponding to the tech and
  locate the appropriate CID (Command ID) entry.
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. This handle
  should be queried from the task by cleint before passing to the function
 
  @param cid
  Messsage router CID (Command ID) for which the entry is to be located
 
  @return
  Pointer to the memory location which stores CID (Command ID) entry
*/
rf_dispatch_cid_info_type*
rf_dispatch_get_cid_info
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid
)
{
  rf_dispatch_cid_info_type *cid_info = NULL;

  /*------------ all good to proceed -----------------------------------------*/

  cid_info = rf_dispatch_fetch_cid_info( dispatch_handle, cid );

  /* Retrn CID (Command ID) INFO */
  return cid_info;

} /* rf_dispatch_get_cid_info */

/*! @} */

/*============================================================================*/
/*!
  @addtogroup RF_DISPATCH_MAIN_DISPATCH
 
  @brief
  This module holds the primary dispatcher code implementation. This will
  dispatch the incoming command to the appropriate callback function
  registered for the given command and task.
 
  @{
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
 
  @param dispatch_handle
  Pointer to the memory containing the dispatch handle data. 
 
  @param cmd_ptr
  Pointer to the memory containing command to the task
 
  @return
  Flag indicating the status of command dispatching
*/
rf_dispatch_error_code_enum
rf_dispatch_command
(
  rf_dispatch_handle_type *dispatch_handle,
  void *cmd_ptr
)
{
  boolean ret_val = RF_DISPATCH_SUCCESS;
  rf_dispatch_cid_type cid;
  rf_dispatch_cid_info_type *cid_info = NULL;

  /* Perform task ID validation */
  if ( dispatch_handle == NULL )
  {
    RF_MSG( RF_ERROR, "rf_dispatch_command: NULL Task Handle " );    
    return RF_DISPATCH_FAILURE;
  } /* if ( dispatch_handle == NULL ) */

  /* Perform NULL pointer check */
  if ( cmd_ptr == NULL )
  {
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_1( RF_ERROR, "rf_dispatch_command: [ %s ] - No command in Q",
                    dispatch_handle->task_name );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_dispatch_command: [ 0x%x ] - No command in Q",
                dispatch_handle->task_uid );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    return RF_DISPATCH_FAILURE;
  } /* if ( cmd == NULL ) */

  /*------------ all good to proceed -----------------------------------------*/

  /* Extract CID (Command ID) from the command pointer */
  if ( dispatch_handle->extract_cid_fp != NULL )
  {
    if ( dispatch_handle->extract_cid_fp( cmd_ptr, &cid ) == FALSE )
    {
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_1( RF_ERROR, "rf_dispatch_command: [ %s ] - CID could not "
                      "be extracted", dispatch_handle->task_name );    
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_1( RF_ERROR, "rf_dispatch_command: [ 0x%x ] - CID could not "
                            "be extracted", dispatch_handle->task_uid );    
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
      return RF_DISPATCH_FAILURE;
    } /* if ( fp_table->extract_cid_fp( cmd_ptr, &cid ) == FALSE ) */
  } /* if ( fp_table->extract_cid_fp != NULL ) */
  else
  {
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_1( RF_ERROR, "rf_dispatch_command: [ %s ] - NULL FP to "
                    "extract CID", dispatch_handle->task_name );    
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_dispatch_command: [ 0x%x ] - NULL FP to "
                          "extract CID", dispatch_handle->task_uid );    
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    return RF_DISPATCH_FAILURE;
  } /* if ! ( fp_table->extract_cid_fp != NULL ) */

  /* Get CID (Command ID) info from the linked list*/
  cid_info = rf_dispatch_fetch_cid_info( dispatch_handle, cid );

  /* NULL Pointer check */
  if ( cid_info != NULL )
  {
    /* Check if the callback is present */
    if ( cid_info->cid_cb != NULL )
    {
      cid_info->cid_cb( cmd_ptr, cid_info, cid_info->cid_cb_data );  

      if ( dispatch_handle->debug_mode == TRUE )
      {
        rf_dispatch_debug_dispatch_log(cid_info);
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ( cid_info->cid_cb != NULL ) */
    else
    {
      ret_val = RF_DISPATCH_FAILURE;
      if ( dispatch_handle->debug_mode == TRUE )
      {
        RF_STR_MSG_2( RF_ERROR, "rf_dispatch_command: [ %s ] "
                      "CID.0x%x - No Handler", dispatch_handle->task_name, 
                      cid );
      } /* if ( dispatch_handle->debug_mode == TRUE ) */
      else
      {
        RF_MSG_2( RF_ERROR, "rf_dispatch_command: [ 0x%x ] "
                  "CID.0x%x - No Handler", dispatch_handle->task_uid, cid );
      } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
    } /* if ! ( cid_info->cid_cb != NULL ) */
  } /* if ( cid_info != NULL ) */
  else
  {
    ret_val = RF_DISPATCH_FAILURE;
    if ( dispatch_handle->debug_mode == TRUE )
    {
      RF_STR_MSG_2( RF_ERROR, "rf_dispatch_command: [ %s ] "
                    "CID.0x%x - No Entry", dispatch_handle->task_name, cid );
    } /* if ( dispatch_handle->debug_mode == TRUE ) */
    else
    {
      RF_MSG_2( RF_ERROR, "rf_dispatch_command: [ 0x%x ] "
                "CID.0x%x - No Entry", dispatch_handle->task_uid, cid );
    } /* if ! ( dispatch_handle->debug_mode == TRUE ) */
  } /* if ! ( cid_info != NULL ) */

  /* Return from the dispatcher */
  return ret_val;
 
} /* rf_dispatch_command */

/*! @} */




