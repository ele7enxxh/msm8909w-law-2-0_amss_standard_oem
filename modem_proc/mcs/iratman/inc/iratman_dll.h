#ifndef IRATMAN_DLL_H
#define IRATMAN_DLL_H

/*!
  @file   iratman_dll.h

  @brief  IRATMan Doubly Linked List implementation header

  @details

*/

/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/inc/iratman_dll.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
12/06/13   bph     FR3839 - dynamic tbackoff

===========================================================================*/

#include "iratman.h"
#include "iratman_la.h"

/*===========================================================================

  FUNCTION: IRATMAN_DLL_INSERT_AT_HEAD

===========================================================================*/
/*!
  @brief
    Insert a node at the HEAD of doubly linked list

  @details
    This function inserts "node" at the "head" of doubly linked list

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_dll_insert_at_head
(
  iratman_la_node_type **head,
  iratman_la_node_type **tail,
  iratman_la_node_type  *node
);

/*===========================================================================

  FUNCTION: IRATMAN_DLL_REMOVE

===========================================================================*/
/*!
  @brief
    Remove a node from doubly linked list

  @details
    This function removes "node" from doubly linked list accessed via "head"
    and "tail" pointers.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_dll_remove
(
  iratman_la_node_type **head,
  iratman_la_node_type **tail,
  iratman_la_node_type  *node
);

#endif /* IRATMAN_DLL_H */

