#ifndef RF_DISPATCH_SNUM_H
#define RF_DISPATCH_SNUM_H

/*! 
  @file
  rf_dispatch_snum.h
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_dispatch_snum.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/28/14   ndb/yb  SNUM debug code 
03/21/13   zhw     Updated rf_dispatch_sum_deregister_callback
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     Updated registration interface to pass dispatch handle
01/09/13   aro     Renamed UMID to CID
01/09/13   aro     Bug fixes for dynamic registration test case
01/09/13   aro     Added SNUM dispatch
01/08/13   aro     Removed init_done flag
01/07/13   aro     Added pool initialization during first snum registration
01/07/13   aro     Implementation to perform callback deregistration for
                   sequence number based secondary dispatching
01/07/13   aro     Implementation to perform callback registration for
                   sequence number based secondary dispatching
01/07/13   aro     Defined data structure for SNUM based secondary dispatching
01/07/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_locks.h"
#include "rf_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enumeration to be used when there is no valid SNUM */
#define RF_DISPATCH_NO_SNUM 0

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the maximum number of concurrent MSGR messages */
#define RF_DISPATCH_NUM_MAX_CONCURRENT_RSP 3

/*----------------------------------------------------------------------------*/
/*! Semaphore token ID type */
typedef uint8 rf_dispatch_snum_item_id_type;

/*----------------------------------------------------------------------------*/
/*! Forward declaration of SNUM dispatch info type */
struct rf_dispatch_snum_info_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the sequence number based second-level dispatch handler 
  function associated with each command.

  @param cmd_ptr
  Pointer to RF_FWRSP command payload
 
  @param snum_data
  Pointer to the opaque data pertaining to sequence number
*/
typedef boolean (*rf_dispatch_snum_handler_fp_type)( 
                   void *cmd_ptr,
                   const struct rf_dispatch_snum_info_type_t *snum_data );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition containing debug information for snum.
 
  @details
  This structure holds information for debugging second-level dispatching based on
  sequence number. 
*/
typedef struct rf_dispatch_snum_debug_info_type_t
{
  /*Sequence Number */
  uint32 snum;

  /*Number of snums available */
  uint8 num_of_spare_snums;
} rf_dispatch_snum_debug_info;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to the debug function of the client
  
  @param client_debug_info
  Pointer to the debug payload of the client
  
  @param snum_debug_info
  Snum debug information needed by client 
*/
typedef void (*rf_dispatch_snum_debug_handler_fp_type)(
              void *client_debug_info, 
              rf_dispatch_snum_debug_info snum_debug_info);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition containing debug information from client for second-level
  dispatching based on sequence number.
 
  @details
  This structure holds information sent by client for debugging second-level 
  dispatching based on sequence number. 
*/
typedef struct rf_dispatch_snum_client_debug_info_type_t
{
  /*Client info passed in to be passed back to callback function*/
  void* client_debug_info;

  /*Callback function to be called after snum pop for client to take customized 
  action with debug information */
  rf_dispatch_snum_debug_handler_fp_type client_cb;
} rf_dispatch_snum_client_debug_info;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition containing information for second-level dispatching based 
  on sequence number.
 
  @details
  This structure holds information for second-level dispatching based on
  sequence number. 
*/
typedef struct rf_dispatch_snum_info_type_t
{
  rf_dispatch_snum_item_id_type item_id;
  /*!< Semaphore token ID type */

  uint32 snum;
  /*!< SequenceNumber corresponding to the MSGR message*/

  boolean static_cb_registration;
  /*!< Flag indicating, if the callback registered should be automatically 
  deregistered at the end of the dispatching. FALSE will force the auto 
  deregistration, where as TRUE will ensure no auto-deregistration. 
  Inc ase of TRUE, client should explicity perform deregistration */ 

  rf_dispatch_snum_handler_fp_type snum_cb;
  /*!< Callback function pertaining to the sequence number */

  void *snum_cb_data;
  /*!< Pointer to the opaque data pertaining to the sequence number */

} rf_dispatch_snum_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition for sequence number doubly linked list containing 
  information for second-level dispatching based on sequence number.
 
  @details
  This structure holds information for second-level dispatching based on
  sequence number. 
*/
typedef struct rf_dispatch_snum_node
{
  rf_dispatch_snum_info_type snum_info;
  /*!< Structure to hold information for second-level dispatching based on
  sequence number. */

  struct rf_dispatch_snum_node *prev;
  /*!< Pointer to Previous Node in the linked list */

  struct rf_dispatch_snum_node *next;
  /*!< Pointer to Next Node in the linked list */

} rf_dispatch_snum_node_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold sequence number Pool data */
typedef struct
{
  rf_dispatch_snum_info_type no_snum_info;
  /*!< Structure to hold information for second-level dispatching for the 
  message without valid sequence number. */

  rf_dispatch_snum_node_type *item_list;
  /*!< SNUM items where each entry will correspond to one 
  SNUM entry */

  rf_dispatch_snum_node_type *sentinel_spare_items;
  /*!< 
  @brief 
  Sentinel for Spare items 
   
  @details 
  This list contains unallocated items. Based on client usage, 
  item status from this list will be removed and put in sentinel_used 
  list. 
  */

  rf_dispatch_snum_node_type *sentinel_used_items;
  /*!< 
  @brief 
  Sentinel for Used items 
   
  @details 
  This list contains items which are used atleast once. Based on client usage, 
  item from sentinel_spare list will be removed and 
  put in this list. The items in sentinel_used are 
  maintained in time order.  The first item in the list is the most recent 
  (current) item being used
  */

  uint8 num_of_spare_items;
  /*!< Variable to indicate the total number of free items */

  rf_lock_data_type critical_sect;
  /*!< critical section protection to pop and push SNUM items */
  
} rf_dispatch_snum_pool_type;

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_snum_register_callback
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type sid,
  uint32 snum,
  boolean static_cb_registation,
  rf_dispatch_snum_handler_fp_type snum_handler,
  void *snum_data,
  rf_dispatch_snum_client_debug_info *debug_info
);

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_sum_deregister_callback
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid,
  uint32 snum,
  void** snum_cb_data_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_snum
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  uint32 snum
);

#ifdef __cplusplus
}
#endif

#endif /* RF_DISPATCH_SNUM_H */

