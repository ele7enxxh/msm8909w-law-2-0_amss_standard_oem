/*!
  @file
  lte_circ_q.h

  @brief
  This file contains defines lockless circular queue structure and APIs for
  LTE common.


*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_circ_q.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/11   bn      Added in none-copied circular queue APIs
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_CIRC_Q_H
#define LTE_CIRC_Q_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief LTE common circular queue structure
*/
typedef struct
{
  uint32  read_index;  /*!< read index */
  uint32  write_index; /*!< write index */
  uint8   *buff;       /*!< buffer pointer */
  uint32  item_size;   /*!< size of each item in the circular queue */
  uint32  buff_size;   /*!< total buffer size, must be a multiple of item size*/
} lte_circ_q_s;

/*===========================================================================

  FUNCTION:  lte_circ_q_put_item

===========================================================================*/
/*!
    @brief
    This function is used to put an item on the circular queue.

    @detail
    - a input pointer to the queue
	- a pointer from user space memory which contains the item want
	to store in the queue.

    @return
    Return TRUE if the put operation is successful.

    @note
    lte_circ_q_init() must be called before using this function.

    @see related_function()
    lte_circ_q_init(), lte_circ_q_get_item(),
    lte_circ_q_is_empty()

*/
/*=========================================================================*/
extern boolean lte_circ_q_put_item
(
  lte_circ_q_s         *circ_queue_ptr,  /*!< pointer to circular queue*/
  uint8                *item_ptr        /*!< pointer to the item */
);

/*===========================================================================

  FUNCTION:  lte_circ_q_get_item

===========================================================================*/
/*!
    @brief
    This function is used to get an item on the circular queue.

    @detail
    - a input pointer to the queue
	- a pointer to user space memory to store the item


    @return
    Return TRUE if the get operation is successful.

    @note
    lte_circ_q_init() must be called before using this function.

    @see related_function()
    lte_circ_q_init(), lte_circ_q_put_item(),
    lte_circ_q_is_empty()

*/
/*=========================================================================*/
extern boolean lte_circ_q_get_item
(
  lte_circ_q_s         *circ_queue_ptr, /*!< pointer to circular queue*/
  uint8                *item_ptr        /*!< pointer to the item */
);

/*===========================================================================

  FUNCTION:  lte_circ_q_init

===========================================================================*/
/*!
    @brief
    This function is used to initialize circular queue.

    @detail
    - a input pointer to the queue
	- a pointer to a buffer in the user space memory
	- the maxinum size of the buffer. This must be multiple of item size
	- the size of each item.


    @return
    None.

    @note

    @see related_function()
    lte_circ_q_put_item(), lte_circ_q_get_item(),
    lte_circ_q_is_empty()

*/
/*=========================================================================*/
extern void lte_circ_q_init
(
  lte_circ_q_s         *circ_queue_ptr, /*!< pointer to circular queue*/
  uint8                *buff_ptr,       /*!< pointer to the buffer  */
  uint32                buff_size,      /*!< size of the buffer*/
  uint32                item_size       /*!< size of the each item*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_is_empty

===========================================================================*/
/*!
    @brief
    This function is called to check if the circular queue is empty or not.

    @detail
    - a input pointer to the queue


    @return
    Return TRUE if the queue is empty.

    @note
    lte_circ_q_init() must be called before using this function.

    @see related_function()
    lte_circ_q_put_item(), lte_circ_q_get_item(), lte_circ_q_init()

*/
/*=========================================================================*/
extern boolean lte_circ_queue_is_empty
(
  lte_circ_q_s *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_is_full

===========================================================================*/
/*!
    @brief
    This function is called to check if the circular queue is full or not.

    @detail
    - a input pointer to the queue


    @return
    Return TRUE if the queue is full.

    @note
    lte_circ_q_init() must be called before using this function.

    @see related_function()
    lte_circ_q_put_item(), lte_circ_q_get_item(), lte_circ_q_init()

*/
/*=========================================================================*/
extern boolean lte_circ_queue_is_full
(
  lte_circ_q_s *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_get_read_pointer

===========================================================================*/
/*!
    @brief
    This function is used to get a read pointer from the circular queue.
    When done with the reading, lte_circ_q_update_read_pointer must be 
    called.

    @detail
    - a input pointer to the queue


    @return
    - Circular queue is empty -> return a NULL pointer
    - Circular queue is not empty -> return pointer to the next read item

    @note
    lte_circ_q_init() must be called before using this function.
    This circular queue API is a none-copied circular queue API. The user will
    work directly with the internal memory of the circular buffer, when user is 
    done, user needs to call the update function to update the read/write 
    pointers

    @see related_function()
    lte_circ_q_init(), lte_circ_q_get_write_pointer(),
    lte_circ_q_is_empty(), lte_circ_q_update_read_pointer(),
    lte_circ_q_update_write_pointer()

*/
/*=========================================================================*/
extern uint8* lte_circ_q_get_read_pointer
(
  lte_circ_q_s         *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_update_read_pointer

===========================================================================*/
/*!
    @brief
    This function is called to update the read pointer. The read pointer 
    will point to the next item

    @detail
    - a input pointer to the queue


    @return
    None

    @note
    lte_circ_q_init() must be called before using this function.
    This circular queue API is a none-copied circular queue API. The user will
    work directly with the internal memory of the circular buffer, when user is 
    done, user needs to call the update function to update the read/write 
    pointers

    @see related_function()
    lte_circ_q_init(), lte_circ_q_get_write_pointer(),
    lte_circ_q_is_empty(), lte_circ_q_get_read_pointer(),
    lte_circ_q_update_write_pointer()

*/
/*=========================================================================*/
extern void lte_circ_q_update_read_pointer
(
  lte_circ_q_s         *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_get_write_pointer

===========================================================================*/
/*!
    @brief
    This function is used to get a write pointer from the circular queue.
    When done with the writing, lte_circ_q_update_write_pointer must be 
    called.

    @detail
    - a input pointer to the queue


    @return
    - Circular queue is FULL -> return a NULL pointer
    - Circular queue is not FULL -> return pointer to the next free item

    @note
    lte_circ_q_init() must be called before using this function.
    This circular queue API is a none-copied circular queue API. The user will
    work directly with the internal memory of the circular buffer, when user is 
    done, user needs to call the update function to update the read/write 
    pointers

    @see related_function()
    lte_circ_q_init(), lte_circ_q_get_read_pointer(),
    lte_circ_q_is_empty(), lte_circ_q_update_read_pointer(),
    lte_circ_q_update_write_pointer()

*/
/*=========================================================================*/
extern uint8* lte_circ_q_get_write_pointer
(
  lte_circ_q_s         *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  lte_circ_q_update_write_pointer

===========================================================================*/
/*!
    @brief
    This function is called to update the write pointer to the next free 
    item

    @detail
    - a input pointer to the queue


    @return
    None

    @note
    lte_circ_q_init() must be called before using this function.
    This circular queue API is a none-copied circular queue API. The user will
    work directly with the internal memory of the circular buffer, when user is 
    done, user needs to call the update function to update the read/write 
    pointers

    @see related_function()
    lte_circ_q_init(), lte_circ_q_get_read_pointer(),
    lte_circ_q_is_empty(), lte_circ_q_update_read_pointer(),
    lte_circ_q_get_write_pointer()

*/
/*=========================================================================*/
extern void lte_circ_q_update_write_pointer
(
  lte_circ_q_s         *circ_queue_ptr  /*!< pointer to circular queue*/
);

#endif /* LTE_CIRC_Q_H */

