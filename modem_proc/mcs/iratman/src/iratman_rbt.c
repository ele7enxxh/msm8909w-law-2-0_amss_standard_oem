/*!
  @file   iratman_rbt.c

  @brief  IRAT List Manager Red-Black Tree implementation.

  @details
  This file contains the code for operation of IRATMan Red-Black Tree.

*/

/*===========================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/src/iratman_rbt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/15   pk      Replace assert.h header file with amssassert.h
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
06/03/14   bph     Problem with remove function
01/29/14   bph     Address a KW issue
12/06/13   bph     FR3839 - dynamic tbackoff

===========================================================================*/
/* Common */
#include <amssassert.h>

/* IRATMan */
#include "iratman_rbt.h"
#include "iratman_i.h"

/* Others */
#include "msg.h"

/*===========================================================================
  INTERNAL
===========================================================================*/
/*===========================================================================

  FUNCTION:  IRATMAN_RBT_FIND_MAX

===========================================================================*/
/*!
  @brief
    Find the Max node in the inputs subtree

  @details
    This function finds the max node in the inputs subtree.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC iratman_la_node_type* iratman_rbt_find_max
(
   iratman_la_node_type *node
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_find_max" );

  if ( node == NULL)
  {
    return NULL;
  }

  if (node->right == NULL)
  {
    return node;
  }

  return iratman_rbt_find_max(node->right);

} /* iratman_rbt_find_max */

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_FIND_MIN

===========================================================================*/
/*!
  @brief
    Find the Min node in the inputs subtree

  @details
    This function finds the min node in the inputs subtree.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC iratman_la_node_type* iratman_rbt_find_min
(
   iratman_la_node_type *node
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_find_min" );

  if ( node == NULL)
  {
    return NULL;
  }

  if (node->left == NULL)
  {
    return node;
  }

  return iratman_rbt_find_min(node->left);

} /* iratman_rbt_find_min */

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_FIND_PARENT

===========================================================================*/
/*!
  @brief
    Find node's parent

  @details
    This function returns a pointer to a node's parent

  @note
    None

  @return
    Pointer to node's parent, or NULL

*/
/*=========================================================================*/
STATIC iratman_la_node_type* iratman_rbt_find_parent
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_find_parent" );

  if (*tree == NULL || node == NULL)
  {
    return NULL;
  }

  if ((*tree)->left == node)
  {
    return *tree;
  }
  else if ((*tree)->right == node)
  {
    return *tree;
  }
  else if ((*tree)->data.key > node->data.key)
  {
    return iratman_rbt_find_parent(&((*tree)->left), node);
  }
  else /* (*tree)->data.key < node->data.key */
  {
    return iratman_rbt_find_parent(&((*tree)->right), node);
  }

} /* iratman_rbt_find_parent */

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_REMOVE_NODE_W_ZERO_CHILDREN

===========================================================================*/
/*!
  @brief
    Remove a node with no children

  @details
    This function removes a node with no children

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_rbt_remove_node_w_zero_children
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
)
{
  iratman_la_node_type* parent = iratman_rbt_find_parent(tree, node);

  IRATMAN_DEBUG_MSG( "iratman_rbt_remove_node_w_zero_children" );

  if(parent == NULL)
  {
    /* removing root */
    *tree = NULL;
  }
  else if (parent->left == node)
  {
    parent->left  = NULL;
  }
  else /* parent->right == node*/
  {
    parent->right = NULL;
  }

} /* iratman_rbt_remove_node_w_zero_children */

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_REMOVE_NODE_W_ONE_CHILD

===========================================================================*/
/*!
  @brief
   Remove a node with one child

  @details
    This function removes a node with one child

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_rbt_remove_node_w_one_child
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
)
{
  iratman_la_node_type* parent;
  iratman_la_node_type* child_of_node_being_removed;

  IRATMAN_DEBUG_MSG( "iratman_rbt_remove_node_w_one_child" );

  /* set ptr to the one child of the node being removed */
  if (node->left != NULL)
  {
    child_of_node_being_removed = node->left;
  }
  else
  {
    child_of_node_being_removed = node->right;
  }

  /* set parent ptr */
  parent = iratman_rbt_find_parent(tree, node);

  if(parent == NULL)
  {
    /* removing root case */
    *tree = child_of_node_being_removed;
  }
  else if ( parent->left == node )
  {
    parent->left = child_of_node_being_removed;
  }
  else /* parent->right == node */
  {
    parent->right = child_of_node_being_removed;
  }

} /* iratman_rbt_remove_node_w_one_child */

/*===========================================================================

  FUNCTION:  IRATMAN_RBT_REMOVE_NODE_W_TWO_CHILDREN

===========================================================================*/
/*!
  @brief
    Removes a node with two children

  @details
    This function removes a node with two children

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_rbt_remove_node_w_two_children
(
   iratman_la_node_type **tree,
   iratman_la_node_type  *node
)
{
  iratman_la_node_type *parent;

  IRATMAN_DEBUG_MSG( "iratman_rbt_remove_node_w_two_children" );

  parent = iratman_rbt_find_parent(tree, node);

  if ( parent == NULL )
  {
    /*
     * removing root:
     *  Set root to node's left ptr
     *  Set node's predecessor's right ptr (by defintion NULL) to node's right
        ptr
     */
    iratman_la_node_type *predecessor = iratman_rbt_find_max(node->left);
    ASSERT( predecessor != NULL );

    *tree              = node->left;
    predecessor->right = node->right;
  }
  else if (node == parent->left)
  {
    /*
     * removing left child:
     *  Set parent of node's left ptr to node's left ptr
     *  Set node's predecessor's right ptr (by defintion NULL) to node's right
     *  ptr
     */
    iratman_la_node_type *predecessor = iratman_rbt_find_max(node->left);
    ASSERT( predecessor != NULL );

    parent->left       = node->left;
    predecessor->right = node->right;
  }
  else /* node == parent->right */
  {
    /*
     * removing right child:
     *  Set parent of node's right ptr to node's right ptr
     *  Set node's successor's left ptr (by defintion NULL) to node's left
     *  ptr
     */
    iratman_la_node_type *successor = iratman_rbt_find_min(node->right);
    ASSERT( successor != NULL );

    parent->right   = node->right;
    successor->left = node->left;
  }

} /* iratman_rbt_remove_node_w_two_children */


/*===========================================================================
                      EXPORTED FUNCTION DEFINITIONS
===========================================================================*/
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
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_insert" );

  if (*tree == NULL)
  {
    *tree = node;
    return;
  }
  else if ( node->data.key < (*tree)->data.key )
  {
    iratman_rbt_insert(&((*tree)->left), node);
  }
  else /* node->data.key > (*tree)->data.key */
  {
    iratman_rbt_insert(&((*tree)->right), node);
  }

} /* iratman_rbt_insert */

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
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_find" );

  if (node == NULL)
  {
    return NULL;
  }

  if (cell_id == node->data.key)
  {
    return node;
  }
  else if (cell_id < node->data.key)
  {
    return iratman_rbt_find(node->left, cell_id);
  }
  else /* cell_id > node->data.key */
  {
    return iratman_rbt_find(node->right, cell_id);
  }

} /* iratman_rbt_find */

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
)
{
  IRATMAN_DEBUG_MSG( "iratman_rbt_remove" );

  if (*tree == NULL || node == NULL)
  {
    return;
  }

  if (node->left == NULL && node->right == NULL)
  {
    iratman_rbt_remove_node_w_zero_children(tree, node);
  }
  else if (node->left == NULL || node->right == NULL)
  {
    iratman_rbt_remove_node_w_one_child(tree, node);
  }
  else
  {
    iratman_rbt_remove_node_w_two_children(tree, node);
  }

} /* iratman_rbt_remove */
