/*!
  @file
  mutils_circ_q.h

  @brief
  This file contains defines lockless circular queue structure and APIs.


*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/common/mutils_circ_q.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/10   ax      added mutils_circ_q_cnt() API
05/15/08   bn      Initial version
===========================================================================*/

#ifndef MUTILS_CIRC_Q_H
#define MUTILS_CIRC_Q_H

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

/*! @brief modem utils common circular queue structure
*/
typedef struct
{
  uint32  read_index;  /*!< read index */
  uint32  write_index; /*!< write index */
  uint8   *buff;       /*!< buffer pointer */
  uint32  item_size;   /*!< size of each item in the circular queue */
  uint32  buff_size;   /*!< total buffer size, must be a multiple of item size*/
} mutils_circ_q_s;

/*===========================================================================

  FUNCTION:  mutils_circ_q_put_item

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
    mutils_circ_q_init() must be called before using this function.

    @see related_function()
    mutils_circ_q_init(), mutils_circ_q_get_item(),
    nutils_circ_q_is_empty()

*/
/*=========================================================================*/
extern boolean mutils_circ_q_put_item
(
  mutils_circ_q_s      *circ_queue_ptr,  /*!< pointer to circular queue*/
  uint8                *item_ptr        /*!< pointer to the item */
);

/*===========================================================================

  FUNCTION:  mutils_circ_q_get_item

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
    mutils_circ_q_init() must be called before using this function.

    @see related_function()
    mutils_circ_q_init(), mutils_circ_q_put_item(),
    mutils_circ_q_is_empty()

*/
/*=========================================================================*/
extern boolean mutils_circ_q_get_item
(
  mutils_circ_q_s      *circ_queue_ptr, /*!< pointer to circular queue*/
  uint8                *item_ptr        /*!< pointer to the item */
);

/*===========================================================================

  FUNCTION:  mutils_circ_q_init

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
    mutils_circ_q_put_item(), mutils_circ_q_get_item(),
    mutils_circ_q_is_empty()

*/
/*=========================================================================*/
extern void mutils_circ_q_init
(
  mutils_circ_q_s      *circ_queue_ptr, /*!< pointer to circular queue*/
  uint8                *buff_ptr,       /*!< pointer to the buffer  */
  uint32                buff_size,      /*!< size of the buffer*/
  uint32                item_size       /*!< size of the each item*/
);

/*===========================================================================

  FUNCTION:  mutils_circ_q_is_empty

===========================================================================*/
/*!
    @brief
    This function is called to check if the circular queue is empty or not.

    @detail
    - a input pointer to the queue


    @return
    Return TRUE if the queue is empty.

    @note
    mutils_circ_q_init() must be called before using this function.

    @see related_function()
    mutils_circ_q_put_item(), mutils_circ_q_get_item(), mutils_circ_q_init()

*/
/*=========================================================================*/
extern boolean mutils_circ_queue_is_empty
(
  mutils_circ_q_s *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  mutils_circ_q_is_full

===========================================================================*/
/*!
    @brief
    This function is called to check if the circular queue is full or not.

    @detail
    - a input pointer to the queue


    @return
    Return TRUE if the queue is full.

    @note
    mutils_circ_q_init() must be called before using this function.

    @see related_function()
    mutils_circ_q_put_item(), mutils_circ_q_get_item(), mutils_circ_q_init()

*/
/*=========================================================================*/
extern boolean mutils_circ_queue_is_full
(
  mutils_circ_q_s *circ_queue_ptr  /*!< pointer to circular queue*/
);

/*===========================================================================

  FUNCTION:  mutils_circ_q_cnt

===========================================================================*/
/*!
    @brief
    This function returns the number of items in the queue.

    @detail
    - a input pointer to the queue


    @return
    The number of items in the queue.

    @note
    mutils_circ_q_init() must be called before using this function.

    @see related_function()
    mutils_circ_q_put_item(), mutils_circ_q_get_item(), mutils_circ_q_init()

*/
/*=========================================================================*/

extern uint32 mutils_circ_q_cnt
( 
  mutils_circ_q_s *circ_queue_ptr  /*!< pointer to circular queue*/
);

#endif /* MUTILS_CIRC_Q_H */
