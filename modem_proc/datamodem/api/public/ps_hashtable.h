/*!
  @file
  ps_hashtable.h

  @brief
  This file contain declarations of PS Hash table data stucture

*/
/*===========================================================================

  Copyright (c) 2013-14 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_hashtable.h#1 $
$DateTime: 2016/12/13 08:00:03 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/14   ash     Initial version
===========================================================================*/

#ifndef PS_HASHTABLE_H
#define PS_HASHTABLE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>

/*===========================================================================

                              CONSTANTS

===========================================================================*/
#define PS_HASHTABLE_CB_NUM     16
#define PS_HASHTABLE_CB_HIGH_WM 12
#define PS_HASHTABLE_CB_LOW_WM  5

#define PS_HASHTABLE_MAX_TBL_SIZE 100
#define PS_HASHTABLE_MAX_BKT_SIZE 20

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
typedef void* ps_hashtable_type;

/*===========================================================================

                              MACROS

===========================================================================*/

/*! @brief Hash node data structure */
typedef enum
{
  PS_HT_SUCCESS =  0, /* Hash table operation success */
  PS_HT_FAILURE = -1, /* Hash table operation failed */
  PS_HT_EOL     =  1, /* Hash table End of List reached */
  PS_HT_MAX
} ps_ht_return_type;


/*! @brief Hash node data structure */
typedef struct ps_ht_node_s
{
  /*! Next node pointer */
  struct ps_ht_node_s        *next;
  /*! Previous node pointer */
  struct ps_ht_node_s        *prev;
  /*! Hash key for this node */
  uint64                      key;
  /*! Hash value for this node */
  void                       *value;
} ps_ht_node_type;


/*! @brief Hash bucket data structure */
typedef struct
{
  /*! Bucket list head */
  ps_ht_node_type         *head;
  /*! Bucke list tail */
  ps_ht_node_type         *tail;
  /*! Number of nodes in the bucket */
  uint16                   cnt;
} ps_ht_bkt_type;


/*! @brief Hash Table Data structure */
typedef struct ps_ht_cb_s
{
  /*! Self pointer */
  struct ps_ht_cb_s        *this_cb_ptr;
  /*! Hash table */
  ps_ht_bkt_type           *hashtbl;
  /*! Size of Hash table*/
  uint16                    table_size;
  /*! Depth of Hash bucket */
  uint16                    bucket_depth;
  /*! Index of last node that returned  */
  int32                     last_idx;
  /*! pointer to last node that returned */
  ps_ht_node_type          *last_node;
  /*! Free API to free the items in the node.*/
  void (*free_f_cb)(void*);
} ps_ht_cb_type;

/*==============================================================================

                    EXTERNAL FUNCTIONS

==============================================================================*/

/*===========================================================================
FUNCTION PS_HT_POWERUP_INIT()

DESCRIPTION
  This function initializes PS mem control blocks for the Hashtables.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ht_powerup_init
(
  void
);

/*===========================================================================

  FUNCTION:  ps_hashtable_create

===========================================================================*/
/*!
    @brief            
    Allocate & Initialize internal structure of hash table
 
    @detail

    @return
    Handle to hashtable on success
    NULL on failure

    @note
*/
ps_hashtable_type ps_hashtable_create
(
   uint16 table_size,
   uint16 bucket_depth,
   void (*free_f_cb)(void*)
);

/*===========================================================================

  FUNCTION:  ps_hashtable_add

===========================================================================*/
/*!
    @brief            
    Add the node to Hashtable.
 
    @detail

    @return
    TRUE on sucess
    FALSE on failure

    @note
*/
boolean ps_hashtable_add
(
   ps_hashtable_type   handle,
   uint64              key,
   void               *value
);

/*===========================================================================

  FUNCTION:  ps_hashtable_lookup

===========================================================================*/
/*!
    @brief            
    Lookup the node in Hashtable.
 
    @detail

    @return
    TRUE on sucess
    FALSE on failure

    @note
*/
boolean ps_hashtable_lookup
(
   ps_hashtable_type  handle,
   uint64             key,
   void             **value
);

/*===========================================================================

  FUNCTION:  ps_hashtable_reset_state

===========================================================================*/
/*!
    @brief            
    Returns the internal state. Thus ps_hashtable_get_next_key_value_pair()
    can start from begining.
 
    @detail

    @return
    Handle to hashtable on success
    NULL on failure

    @note
*/
int ps_hashtable_reset_state
(
  ps_hashtable_type  handle
);

/*===========================================================================

  FUNCTION:  ps_hashtable_get_next_key_value_pair

===========================================================================*/
/*!
    @brief            
    Returns the next key value pair in the hashtable.
 
    @detail

    @return
    Handle to hashtable on success
    NULL on failure

    @note
*/
ps_ht_return_type ps_hashtable_get_next_key_value_pair
(
  ps_hashtable_type  handle,
  uint64            *key,
  void             **value
);

/*===========================================================================

  FUNCTION:  ps_hashtable_remove

===========================================================================*/
/*!
    @brief            
    Remove the node from Hashtable.
 
    @detail

    @return
    TRUE on sucess
    FALSE on failure

    @note
*/

boolean ps_hashtable_remove
(
   ps_hashtable_type  handle,
   uint64             key,
   void             **value
);

/*===========================================================================
FUNCTION PS_HASHTABLE_REMOVE_ALL()

DESCRIPTION
  This function is used to remove all Hashtable entries.

PARAMETERS
  (IN) handle: hashtable handle

RETURN VALUE
  TRUE on success
  FALSE on failures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_remove_all
(
   ps_hashtable_type  handle
);

/*===========================================================================
FUNCTION PS_HASHTABLE_DESTROY()

DESCRIPTION
  This function is used to destroy Hashtable and free the control block.

PARAMETERS
  (IN) handle: hashtable handle

RETURN VALUE
  TRUE on success
  FALSE on failures

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_hashtable_destroy
(
   ps_hashtable_type  handle
);

#endif /* PS_HASHTABLE_H */
