/*===========================================================================

                         Q M I _ P B M _ C M D _ L I S T . C

DESCRIPTION

 The Qualcomm PBM command list management handled here.

EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2009,10,14 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/18/14    stv    Fix for logging strings in F3s 
07/25/14    NR     Use v2.0 diag macros
03/08/14    stv    Move F3 print buffer from stack to heap 
03/29/10    sk     Created Module.
===========================================================================*/

/*===========================================================================

  INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include <memory.h>
#include "amssassert.h"
#include "qmi_pbm_cmd_list.h"
#include "qmi_pbm_if.h"
#include "modem_mem.h"
#include "uim_msg.h"
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_PBM_CMD_LIST_BUF_MAX 10


typedef struct qmi_pbm_cmd_list_buf_tag
{
  qmi_pbm_cmd_list_public_type    pub;
  struct qmi_pbm_cmd_list_buf_tag *prev_ptr;
  struct qmi_pbm_cmd_list_buf_tag *next_ptr;
} qmi_pbm_cmd_list_buf_type;


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! Variables internal to module qmi_pbm_cmd_list.c
*/
typedef struct
{
  qmi_pbm_cmd_list_buf_type cmd[QMI_PBM_CMD_LIST_BUF_MAX];
  qmi_pbm_cmd_list_buf_type *head_ptr;
  qmi_pbm_cmd_list_buf_type *tail_ptr;
  /* Mutex used to control simultaneous update/access to cmd list */
} qmi_pbm_cmd_list_struct_type;

/*! @brief Variables internal to module qmi_pbm_cmd_list.c
*/
static qmi_pbm_cmd_list_struct_type cmd_list;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_find

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on cmd handle

    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_pbm_cmd_list_buf_type *qmi_pbm_cmd_list_find
(
  uint32  hdl
)
{
  qmi_pbm_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if (buf_ptr->pub.hdl == hdl)
    {
      UIM_PBM_MSG_SPRINTF_3( "Found cmd list entry : %s (%d), handle %d",
                      qmi_pbm_if_lookup_cmd_name( buf_ptr->pub.cmd ), buf_ptr->pub.cmd,
                       hdl );
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  UIM_MSG_HIGH_1( "Not found cmd list entry : handle %d", hdl );

  return NULL;
} /* qmi_pbm_cmd_list_find() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_new_buf

===========================================================================*/
/*!
    @brief
    Allocate a new buffer for the list

    @return
    A pointer to a free buffer, or NULL if there are no free buffers
    available.
*/
/*=========================================================================*/
static qmi_pbm_cmd_list_buf_type *qmi_pbm_cmd_list_new_buf( void )
{
  uint32 i;

  /*-----------------------------------------------------------------------*/

  for ( i = 0; i < QMI_PBM_CMD_LIST_BUF_MAX; i++ )
  {
    if (cmd_list.cmd[i].pub.state == QMI_PBM_CMD_FREE)
    {
      return &cmd_list.cmd[i];
    }
  }

  return NULL;
} /* qmi_pbm_cmd_list_new_buf() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_new_all

===========================================================================*/
/*!
    @brief
    Add an entry to the list of outstanding PBM commands.

    @return
    E_SUCCESS if the entry is created properly
    E_NOT_ALLOWED if the entry was already in the list
    E_NO_MEMORY if there were no buffers
    E_DATA_TOO_LARGE if the subsystem specific info exceeded the buffer size
*/
/*=========================================================================*/
static IxErrnoType qmi_pbm_cmd_list_new_all
(
  uint8 cmd,
  uint32  hdl,
  qmi_pbm_cmd_state_e_type state,
  uint32 sub_id
)
{
  qmi_pbm_cmd_list_buf_type *buf_ptr;
  qmi_pbm_cmd_list_public_type *pub_ptr;


  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_pbm_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    /* handle is already in the list */
    UIM_MSG_ERR_1( "Fail to add cmd list entry with handle %d (already exists)", hdl );
    return E_NOT_ALLOWED;
  }

  buf_ptr = qmi_pbm_cmd_list_new_buf();

  if (buf_ptr == NULL)
  {
    /* No free buffers in the list */
    UIM_MSG_ERR_1( "Fail to add cmd list entry for handle %d (no buffer)", hdl );
    return E_NO_MEMORY;
  }

  if ( state == QMI_PBM_CMD_FREE )
  {
    /* Invalid state */
    UIM_MSG_ERR_2( "Fail to add cmd list entry for handle %d (invalid state %d )",
                          hdl , state );
    return E_NOT_ALLOWED;
  }

  pub_ptr = &buf_ptr->pub;

  pub_ptr->cmd = cmd;
  pub_ptr->hdl = hdl;
  pub_ptr->state = state;
  pub_ptr->sub_id = sub_id;


  /* Sort items in the cmd list by the order of PBM command being received */
  buf_ptr->prev_ptr = cmd_list.tail_ptr;
  buf_ptr->next_ptr = NULL;
  if ( cmd_list.head_ptr == NULL )
  {
    cmd_list.head_ptr = buf_ptr;
  }
  if ( cmd_list.tail_ptr != NULL )
  {
    cmd_list.tail_ptr->next_ptr = buf_ptr;
  }

  cmd_list.tail_ptr = buf_ptr;

  UIM_PBM_MSG_SPRINTF_2( "Command %s (%d) added to list ",
                  qmi_pbm_if_lookup_cmd_name( cmd ), cmd );

  return E_SUCCESS;

} /* qmi_pbm_cmd_list_new_all() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_init

===========================================================================*/
/*!
    @brief
    Initialize cmd list.

    @return
    None
*/
/*=========================================================================*/
void qmi_pbm_cmd_list_init
(
  void
)
{
  int i;
  /*-----------------------------------------------------------------------*/

  UIM_MSG_LOW_0( "qmi_pbm_cmd_list_init()" );

  memset( cmd_list.cmd, 0, sizeof( cmd_list.cmd ) );
  for ( i = 0; i < QMI_PBM_CMD_LIST_BUF_MAX; i++ )
  {
    cmd_list.cmd[ i ].pub.state = QMI_PBM_CMD_FREE;
  }
  cmd_list.head_ptr = NULL;
  cmd_list.tail_ptr = NULL;

} /* qmi_pbm_cmd_list_init() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_new

===========================================================================*/
/*!
    @brief
    Add an entry to the list of outstanding PBM Commands.

    @return
    see qmi_pbm_cmd_list_new_all
*/
/*=========================================================================*/
IxErrnoType qmi_pbm_cmd_list_new
(
  int cmd,
  uint32  hdl,
  qmi_pbm_cmd_state_e_type state,
  uint32 sub_id
)
{
  IxErrnoType status;

  /*-----------------------------------------------------------------------*/

  status = qmi_pbm_cmd_list_new_all( cmd, hdl, state, sub_id );

  return status;

} /* qmi_pbm_cmd_list_new() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_free

===========================================================================*/
/*!
    @brief
    Frees an entry from the list of outstanding PBM Commands.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_pbm_cmd_list_free
(
  uint32  hdl
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_pbm_cmd_list_buf_type *buf_ptr;
  qmi_pbm_cmd_list_public_type *pub_ptr;
  qmi_pbm_cmd_list_buf_type *prev_buf_ptr, *next_buf_ptr;
  uint8 cmd;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_pbm_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    cmd = buf_ptr->pub.cmd;

    /* Detach the item from the cmd list */
    prev_buf_ptr = buf_ptr->prev_ptr;
    next_buf_ptr = buf_ptr->next_ptr;

    /* The only item in the cmd list is being removed */
    if ( ( prev_buf_ptr == NULL ) && ( next_buf_ptr == NULL ) )
    {
      cmd_list.head_ptr = NULL;
      cmd_list.tail_ptr = NULL;
    }
    /* First item in the cmd list is being removed */
    else if ( prev_buf_ptr == NULL )
    {
      cmd_list.head_ptr = next_buf_ptr;
      next_buf_ptr->prev_ptr = NULL;
    }
    /* Last item in the cmd list is being removed*/
    else if ( next_buf_ptr == NULL )
    {
      cmd_list.tail_ptr = prev_buf_ptr;
      prev_buf_ptr->next_ptr = NULL;
    }
    /* Middle item in the cmd list is being removed */
    else
    {
      prev_buf_ptr->next_ptr = buf_ptr->next_ptr;
      next_buf_ptr->prev_ptr = buf_ptr->prev_ptr;
    }
    buf_ptr->next_ptr = buf_ptr->prev_ptr = NULL;

    pub_ptr = &buf_ptr->pub;

    pub_ptr->state = QMI_PBM_CMD_FREE;

    UIM_MSG_HIGH_2( "Deleted cmd list entry : handle %d cmd %d", hdl, cmd );

  }
  else
  {
    /* Entry was not found */
    status = E_FAILURE;
  }

  return status;

} /* qmi_pbm_cmd_list_free() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_update_sub_id

===========================================================================*/
/*!
    @brief
    Updates the subsystem ID (e.g Call ID, SS Ref, Invoke ID) of the entry
    in the list for uint32  hdl with the specified information.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_pbm_cmd_list_update_sub_id
(
  uint32  hdl,
  uint32 sub_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_pbm_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/
  buf_ptr = qmi_pbm_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    buf_ptr->pub.sub_id = sub_id;
    UIM_PBM_MSG_SPRINTF_3( "Update sub id field of cmd list entry : %s , handle %d, sub id %lu",
                    qmi_pbm_if_lookup_cmd_name( buf_ptr->pub.cmd ),
                     buf_ptr->pub.hdl, sub_id );
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_pbm_cmd_list_update_sub_id() */


/*===========================================================================

  FUNCTION:  qmi_pbm_cmd_list_query

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given cmd handle and returns it to
    the requesting subsystem.

    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_pbm_cmd_list_query
(
  uint32  hdl,
  qmi_pbm_cmd_list_public_type *info_ptr /*!< Output parameter */
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_pbm_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_pbm_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    *info_ptr = buf_ptr->pub;
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_pbm_cmd_list_query() */

