/*===========================================================================

                  GM GEN LIST Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM generic linked list

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/15   ssu     Initial creation of file.
===========================================================================*/
#ifndef GM_GEN_LIST_H
#define GM_GEN_LIST_H

#include "gm_core.h"
#include "gm_ebee.h"


/* Data type to maintain the GM generic list nodes */
typedef struct gm_gen_list_node_type_s
{
  struct gm_gen_list_node_type_s* p_next;
  struct gm_gen_list_node_type_s* p_prev;
  /* Pointer to point to the data supplied by node creator */
  void *p_info; 
}gm_gen_list_node_type;

/*===========================================================================

FUNCTION TYPE gm_gen_list_node_compare_cb_type

DESCRIPTION
  Comparator function
  The function returns result < 1 if data1 < data2
  The function returns result = 0 if data1 == data2
  The function returns result > 1 if data1 > data2

DEPENDENCIES

RETURN VALUE
  FALSE if the comparison succeeds.
  TRUE if the comparison fails.
 
SIDE EFFECTS
  None.

===========================================================================*/
typedef boolean (gm_gen_list_node_compare_cb_type)(void *p_d1, void* p_d2, int32* pl_result);

/*===========================================================================

FUNCTION gm_gen_list_add

DESCRIPTION
  This function adds a node to the generic linked list and sorted based on the comparator
  function passed in.
  The function is responsible for allocating the memory for the new node to be added in. 

DEPENDENCIES

RETURN VALUE
  TRUE : If the node was added succesfully.
  FALSE : If the node was not added succesfully. 

SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_gen_list_add(gm_gen_list_node_type **p_head,
				        void *p_data,
				        gm_gen_list_node_compare_cb_type *p_cmp_fcn);

/*===========================================================================

FUNCTION gm_gen_list_find

DESCRIPTION
  This function finds a data in the generic linked list

DEPENDENCIES

RETURN VALUE
  Pointer to the found Node. NULL if node was not found
SIDE EFFECTS
  None.

===========================================================================*/
gm_gen_list_node_type* gm_gen_list_find(gm_gen_list_node_type *p_head,
                                        void* p_data);

/*===========================================================================

FUNCTION gm_gen_list_delete

DESCRIPTION
  This function deletes a given node in the generic linked list.

DEPENDENCIES

RETURN VALUE
  TRUE : If the node was deleted succesfully.
  FALSE : If the node was not deleted succesfully. 

SIDE EFFECTS
  None.

===========================================================================*/
  boolean gm_gen_list_delete(gm_gen_list_node_type **p_head,
                             void *p_data);

/*===========================================================================

FUNCTION gm_gen_list_get_next

DESCRIPTION
  This function returns the next node of the passed in node of the GM generic list .

DEPENDENCIES

RETURN VALUE
  Next node of the passed in node of the GM generic list. 

SIDE EFFECTS
  None.

===========================================================================*/
gm_gen_list_node_type* gm_gen_list_get_next(gm_gen_list_node_type *p_node);

/*===========================================================================

FUNCTION gm_gen_list_get_prev

DESCRIPTION
  This function returns the prev node of the passed in node of the GM generic list .

DEPENDENCIES

RETURN VALUE
  Prev node of the passed in node of the GM generic list. 

SIDE EFFECTS
  None.

===========================================================================*/
gm_gen_list_node_type* gm_gen_list_get_prev(gm_gen_list_node_type *p_node);
#endif

