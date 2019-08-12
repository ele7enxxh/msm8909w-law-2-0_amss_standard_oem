#ifndef _QM_CBPQ_H
#define _QM_CBPQ_H

/*===========================================================================

                         QM_CBPQ.H

DESCRIPTION

  QMI_MMODE header file for cmd_buf pending queue module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/16/13    sm     Created file
===========================================================================*/

#include "qmi_nas_internal.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/

enum qm_cbpq_qtype_e
{
  QM_CBPQ_QTYPE_MIN          = -1,
  QM_CBPQ_QTYPE_NW_SCAN,
  QM_CBPQ_QTYPE_GET_PREF,
  QM_CBPQ_QTYPE_MAX
};

/*===========================================================================
  FUNCTION QM_CBPQ_INIT()

  DESCRIPTION
    This function initializes the cmd_buf pending queue variables

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_init ( void );

/*===========================================================================
  FUNCTION QM_CBPQ_ADD()

  DESCRIPTION
    This function adds the provided item to the indicated type.

  PARAMETERS
    queue_item : pointer to element to queue
    queue_type : type for the caller

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_add ( enum qm_cbpq_qtype_e queue_type, qmi_cmd_buf_type * queue_item, int cl_id );

/*===========================================================================
  FUNCTION QM_CBPQ_GET()

  DESCRIPTION
    This function retrieves the first element for the indicated type.

  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    void * : pointer to element from queue

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cmd_buf_type * qm_cbpq_get( enum qm_cbpq_qtype_e queue_type );

/*===========================================================================
  FUNCTION QM_CBPQ_COUNT()

  DESCRIPTION
    This function determines the count of items for indicated type.

  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    int : count of remaining items for the type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qm_cbpq_count( enum qm_cbpq_qtype_e queue_type );

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_INVALID_ITEMS()

  DESCRIPTION
    This function is used to remove invalid items from the indicated type
    
  PARAMETERS
    queue_type : type for the caller

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_del_invalid_items( enum qm_cbpq_qtype_e queue_type );

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_CLIENT_ITEMS()

  DESCRIPTION
    This function is used to remove a specific clients items from the indicated type
    
  PARAMETERS
    queue_type : type for the caller
    cl_sp      : client to remove items for 

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_cbpq_del_client_items( enum qm_cbpq_qtype_e queue_type, int cl_id );

/*===========================================================================
  FUNCTION QM_CBPQ_DEL_SPECIFIC_ITEM()

  DESCRIPTION
    This function is used to remove specific item from the indicated type
    
  PARAMETERS
    queue_type : type for the caller
    del_item   : item to remove

  RETURN VALUE
    void * to item removed.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cmd_buf_type * qm_cbpq_del_specific_item( enum qm_cbpq_qtype_e queue_type, qmi_cmd_buf_type * del_item );

#endif // !_QM_CBPQ_H
