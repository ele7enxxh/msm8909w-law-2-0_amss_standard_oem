#ifndef ICB_NOTIFY_H
#define ICB_NOTIFY_H
/*=============================================================================

FILE:      icb_notify.h

DESCRIPTION: This defines the NPA ICB Arbiter Node interface

        Copyright (c) 2015 QUALCOMM Technologies, Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/core.mpss/3.5.c12.3/api/buses/icb_notify.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
  $Change: 11985146 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 2015/05/18   sds     Initial version

=============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
                          DATA TYPES
============================================================================*/
typedef struct icb_notify_type * icb_notify_handle_type;

typedef enum
{
  ICB_NOTIFY_MASTER_NODE,
  ICB_NOTIFY_SLAVE_NODE,
  ICB_NOTIFY_NUM_NODE_TYPES,
  ICB_NOTIFY_NODE_TYPE_PLACEHOLDER = 0x7FFFFFFF
} icb_node_type;

typedef enum
{
  ICB_NOTIFY_AB,
  ICB_NOTIFY_IB,
  ICB_NOTIFY_TOTAL,
  ICB_NOTIFY_NUM_THRESHOLD_TYPES,
  ICB_NOTIFY_THRESHOLD_TYPE_PLACEHOLDER = 0x7FFFFFFF
} icb_notify_threshold_type;

typedef struct
{
  icb_node_type             node_type;
  uint32_t                  id;
  icb_notify_threshold_type threshold_type;
  uint64_t                  high_threshold;
  uint64_t                  low_threshold;
} icb_notify_definition_type;

typedef struct
{
  bool     high_threshold;
  bool     low_threshold;
  uint64_t ib;
  uint64_t ab;
  uint64_t total;
} icb_notify_status_type;

/* ICB notification callback type.
 * IMPORTANT:
 *   The status pointer is ONLY valid during the callback.
 *   Please copy structure at the pointer. */
typedef void (*icb_notify_cb_type)(icb_notify_handle_type handle, icb_notify_status_type *status, void *cb_data);

/*============================================================================
                          FUNCTIONS
============================================================================*/

/**=============================================================================
   @brief Creates a notification handle for the specified node and threshold conditions.
   
   @param definition:    A pointer to notification specification
   @param callback:      A pointer to notification callback to use
   @param callback_data: A pointer to notification callback data to send
   
   @return null if an error occurred, non-null otherwise.

   @remark IMPORTANT: This call can potentially trigger a callback from the same context as
           the caller. If this does occur, it will happen before the function returns.
==============================================================================*/
icb_notify_handle_type icb_notify_register
(
  icb_notify_definition_type *definition,
  icb_notify_cb_type          callback,
  void                       *callback_data
);

/**=============================================================================
   @brief Update an existing notification handle with a new rule definition
   
   @param handle:     The notification handle to deregister
   @param definition: A pointer to notification specification
   
   @return true if successful, false otherwise

   @remark IMPORTANT: This call can potentially trigger a callback from the same context as
           the caller. If this does occur, it will happen before the function returns.
==============================================================================*/
bool icb_notify_update
(
  icb_notify_handle_type      handle,
  icb_notify_definition_type *definition
);

/**=============================================================================
   @brief Deregister the given notification handle
   
   Deregister the passed handle, removing any future notifications and freeing
   any memory associated with the handle. The handle cannot be used again
   afterward.

   @param handle: The notification handle to deregister
   
   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_deregister
(
  icb_notify_handle_type handle
);

/**=============================================================================
   @brief Get the notification status for a given notification handle
   
   @param handle: The notification handle to check
   @param handle: A pointer to the buffer status to fill
   
   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_get_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
);

/**=============================================================================
   @brief Reset the notification status on a given notification handle
   
   @param handle: The notification handle
   @param status: The notification status to reset
   
   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_reset_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
);

#endif /* ICB_NOTIFY_H */
