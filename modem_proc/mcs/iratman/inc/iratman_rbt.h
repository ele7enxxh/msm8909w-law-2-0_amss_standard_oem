#ifndef IRATMAN_RBT_H
#define IRATMAN_RBT_H

/*!
  @file   iratman_rbt.h

  @brief  IRATMan Red-Black Tree implementation header

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/inc/iratman_rbt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
12/06/13   bph     FR3839 - dynamic tbackoff

===========================================================================*/

#include "iratman.h"
#include "iratman_la.h"

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_INSERT

===========================================================================*/
/*!
  @brief
    Insert a node into the tree

  @details
    This function insert a node into the tree

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_rbt_insert
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
);

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_FIND

===========================================================================*/
/*!
  @brief
    Find a node in the tree

  @details
    This function takes a key and returns the node.

  @note
    None

  @return
    Pointer to the node, or NULL if the key does not exist in the tree.

*/
/*=========================================================================*/
iratman_la_node_type* iratman_rbt_find
(
   iratman_la_node_type *node,
   uint32                cell_id
);

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_REMOVE

===========================================================================*/
/*!
  @brief
    Remove a node from the tree

  @details
    This function removes a node

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_rbt_remove
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
);

#endif /* IRATMAN_RBT_H */

