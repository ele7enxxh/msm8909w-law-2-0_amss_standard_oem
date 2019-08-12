/*===========================================================================

                         D S _ Q M I _ V O I C E _ C M D _ L I S T . C

DESCRIPTION

 The Data Services Qualcomm Voice command list management handled here.

EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cmd_list.c#1 $ $DateTime:$ $Author:$

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/26/09    sk     Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include <memory.h>
#include "amssassert.h"
#include "qmi_voice_cmd_list.h"
#include "msgcfg.h"
#include "msg_diag_service.h"
#include "qmi_mmode_task_cmd.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifndef FEATURE_NO_QMI_VOICE
#define QMI_VOICE_CMD_LIST_BUF_MAX 20


typedef struct
{
  uint32 sub_size;
} qmi_voice_cmd_list_private_type;

typedef struct qmi_voice_cmd_list_buf_tag
{
  qmi_voice_cmd_list_public_type pub;
  qmi_voice_cmd_list_private_type priv;
  struct qmi_voice_cmd_list_buf_tag *prev_ptr;
  struct qmi_voice_cmd_list_buf_tag *next_ptr;
} qmi_voice_cmd_list_buf_type;


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*! Variables internal to module ds_qmi_voice_cmd_list.c
*/
typedef struct
{
  qmi_voice_cmd_list_buf_type cmd[QMI_VOICE_CMD_LIST_BUF_MAX];
  qmi_voice_cmd_list_buf_type *head_ptr; 
  qmi_voice_cmd_list_buf_type *tail_ptr;
  /* Mutex used to control simultaneous update/access to cmd list */
} qmi_voice_cmd_list_struct_type;

/*! @brief Variables internal to module ds_qmi_voice_cmd_list.c
*/
static qmi_voice_cmd_list_struct_type cmd_list;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find

===========================================================================*/
/*!
    @brief
    Find an entry in the list based on cmd handle
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find
( 
  uint32  hdl
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    if (buf_ptr->pub.hdl == hdl)
    {             
      QM_MSG_MED_3( "Found cmd list entry : %s (%d), handle %d", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), buf_ptr->pub.cmd, 
                       hdl );
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_1("Not found cmd list entry :  handle %d", hdl);

  return NULL;

} /* qmi_voice_cmd_list_find() */

/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_is_empty

===========================================================================*/
/*!
    @brief
    to check if atleast one entry is present in the command list
 
    @return
    TRUE if even one entry is not present, FALSE otherwise
*/
/*=========================================================================*/
boolean qmi_voice_cmd_list_is_empty()
{
  if (cmd_list.head_ptr == NULL)
  { 
    return TRUE;
  }
  else
  { 
    return FALSE;
  }

} /* qmi_voice_cmd_list_is_empty() */

/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find_by_event

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on a cmd event
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find_by_event
( 
  uint32 pending_event_id,
  uint32 state,
  uint8  as_id
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.state & state ) &&
         ( buf_ptr->pub.pending_event_id == pending_event_id )  &&
         ( buf_ptr->pub.as_id == as_id))
    {
      QM_MSG_MED_3( "Found cmd list entry : %s (%d), pending %s ", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), buf_ptr->pub.cmd,
                      qmi_voice_cm_if_lookup_event_name( pending_event_id ) );
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_2( "Not found cmd list entry waiting for %s (%lu)", 
                  qmi_voice_cm_if_lookup_event_name( pending_event_id ), pending_event_id );

  return NULL;

} /* qmi_voice_cmd_list_find_by_event() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find_by_event_and_sub_id

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on a cmd event and 
    a subsystem ID (e.g. Call ID, SS Ref, Invoke ID).
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find_by_event_and_sub_id
( 
  uint32 pending_event_id,
  uint32 sub_id,
  uint32 state,
  uint8  as_id
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.state & state ) &&
         ( buf_ptr->pub.pending_event_id == pending_event_id ) &&
         ( buf_ptr->pub.sub_id == sub_id ) &&
         ( buf_ptr->pub.as_id == as_id ) )
    {
      QM_MSG_MED_3( "Found cmd list entry : %s , pending %s , sub id %lu", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), 
                       qmi_voice_cm_if_lookup_event_name( pending_event_id ), 
                       sub_id );                 
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_3( "Not found cmd list entry waiting for %s (%lu) and sub id %lu", 
                  qmi_voice_cm_if_lookup_event_name( pending_event_id ), pending_event_id, sub_id );

  return NULL;

} /* qmi_voice_cmd_list_find_by_event_and_sub_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find_by_event_and_sub_id_all_subs

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on a cmd event and 
    a subsystem ID across all subscriptions (e.g. Call ID, SS Ref, Invoke ID).
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find_by_event_and_sub_id_all_subs
( 
  uint32 pending_event_id,
  uint32 sub_id,
  uint32 state
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.state & state ) &&
         ( buf_ptr->pub.pending_event_id == pending_event_id ) &&
         ( buf_ptr->pub.sub_id == sub_id ) )
    {
      QM_MSG_MED_3( "Found cmd list entry : %s , pending %s , sub id %lu", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), 
                       qmi_voice_cm_if_lookup_event_name( pending_event_id ), 
                       sub_id );                 
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_3( "Not found cmd list entry waiting for %s (%lu) and sub id %lu", 
                  qmi_voice_cm_if_lookup_event_name( pending_event_id ), pending_event_id, sub_id );

  return NULL;

} /* qmi_voice_cmd_list_find_by_event_and_sub_id_all_subs() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find_by_sub_id

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on sub id
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find_by_sub_id
( 
  uint32 sub_id,
  uint32 state
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.state & state ) &&
         ( buf_ptr->pub.sub_id ==  sub_id) )
    {
      QM_MSG_MED_3( "Found cmd list entry : %s (%d), sub id %d ", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), buf_ptr->pub.cmd,
                       buf_ptr->pub.sub_id);
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_1("Not found cmd list entry with sub id %d ", sub_id);

  return NULL;

} /* qmi_voice_cmd_list_find_by_sub_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_find_by_cmd

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on cmd
 
    @return
    A pointer to the entry if found, NULL otherwise.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_find_by_cmd
( 
  uint16 cmd,
  uint32 state,
  uint8  as_id
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.state & state ) &&
         ( buf_ptr->pub.cmd ==  cmd) && 
         (buf_ptr->pub.as_id == as_id))
    {
      QM_MSG_MED_3( "Found cmd list entry : %s (%d), with cmd %d ", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), buf_ptr->pub.cmd,
                       cmd);
      return buf_ptr;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  QM_MSG_MED_1("Not found cmd list entry with cmd %d ", cmd);

  return NULL;

} /* qmi_voice_cmd_list_find_by_cmd() */

/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_new_buf

===========================================================================*/
/*!
    @brief
    Allocate a new buffer for the list
 
    @return
    A pointer to a free buffer, or NULL if there are no free buffers 
    available.
*/
/*=========================================================================*/
static qmi_voice_cmd_list_buf_type *qmi_voice_cmd_list_new_buf( void )
{
  uint32 i;

  /*-----------------------------------------------------------------------*/

  for ( i = 0; i < QMI_VOICE_CMD_LIST_BUF_MAX; i++ )
  {
    if (cmd_list.cmd[i].pub.state == QMI_VOICE_CMD_FREE)
    {
      return &cmd_list.cmd[i];
    }
  }

  return NULL;

} /* qmi_voice_cmd_list_new_buf() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_new_all

===========================================================================*/
/*!
    @brief
    Add an entry to the list of outstanding RIL_REQUESTs.
 
    @return
    E_SUCCESS if the entry is created properly
    E_NOT_ALLOWED if the entry was already in the list
    E_NO_MEMORY if there were no buffers
    E_DATA_TOO_LARGE if the subsystem specific info exceeded the buffer size
*/
/*=========================================================================*/
static IxErrnoType qmi_voice_cmd_list_new_all
( 
  int cmd, 
  uint32  hdl,
  qmi_voice_cmd_state_e_type state,
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_u_type *sub_ptr, /*!< Subsystem specific information */
  uint32 sub_size,                /*!< Size in bytes of subsystem specific information */
  uint8  as_id
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr;
  qmi_voice_cmd_list_public_type *pub_ptr;
  qmi_voice_cmd_list_private_type *priv_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    /* handle is already in the list */
    QM_MSG_ERROR_1("Fail to add cmd list entry with handle %d (already exists)", hdl);
    return E_NOT_ALLOWED;
  }

  buf_ptr = qmi_voice_cmd_list_new_buf();

  if (buf_ptr == NULL)
  {
    /* No free buffers in the list */
    QM_MSG_ERROR_1("Fail to add cmd list entry for handle %d (no buffer)", hdl);
    return E_NO_MEMORY;
  }

  if ( state == QMI_VOICE_CMD_FREE )
  {
    /* Invalid state */
    QM_MSG_ERROR_2("Fail to add cmd list entry for handle %d (invalid state %d )", hdl, state);
    return E_NOT_ALLOWED;
  }

  if (sub_size > sizeof( qmi_voice_cmd_list_u_type ) )
  {
    QM_MSG_ERROR_2( "Fail to add cmd list entry for handle %d (data too large %lu)", 
                           hdl, sub_size );
    return E_DATA_TOO_LARGE;
  }

  pub_ptr = &buf_ptr->pub;
  priv_ptr = &buf_ptr->priv;

  pub_ptr->state = state;
  pub_ptr->hdl = hdl;
  pub_ptr->cmd = cmd;
  pub_ptr->pending_event_id = (qmi_voice_cm_if_evt_e_type)pending_event_id;
  pub_ptr->sub_id = sub_id;
  pub_ptr->as_id = as_id;
  priv_ptr->sub_size = sub_size;
  if(sub_ptr)
  {
    memscpy( (void *) &pub_ptr->sub, sizeof(pub_ptr->sub), sub_ptr, sub_size ); 
  }

  /* Sort items in the cmd list by the order of RIL command being received */
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
QM_MSG_MED_4("Added cmd list entry with handle %ld, Command %s (%d) pending receipt of %s",
             hdl, qmi_voice_cm_if_lookup_cmd_name(cmd), cmd,
                  qmi_voice_cm_if_lookup_event_name( pending_event_id ) );

  return E_SUCCESS;

} /* qmi_voice_cmd_list_new_all() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_init

===========================================================================*/
/*!
    @brief
    Initialize cmd list.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_cmd_list_init
( 
  void
)
{
  int i;
  /*-----------------------------------------------------------------------*/

  QM_MSG_LOW("qmi_voice_cmd_list_init()");

  memset( cmd_list.cmd, 0, sizeof( cmd_list.cmd ) ); 
  for ( i = 0; i < QMI_VOICE_CMD_LIST_BUF_MAX; i++ )
  {
    cmd_list.cmd[ i ].pub.state = QMI_VOICE_CMD_FREE;
  }
  cmd_list.head_ptr = NULL;
  cmd_list.tail_ptr = NULL;

} /* qmi_voice_cmd_list_init() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_new

===========================================================================*/
/*!
    @brief
    Add an entry to the list of outstanding RIL_REQUESTs.
 
    @return
    see qmi_voice_cmd_list_new_all
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_new
( 
  int cmd, 
  uint32  hdl,
  qmi_voice_cmd_state_e_type state,
  qmi_voice_cm_if_evt_e_type pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_u_type *sub_ptr, /*!< Subsystem specific information */
  uint32 sub_size,                /*!< Size in bytes of subsystem specific information */
  uint8  as_id
)
{
  IxErrnoType status;

  /*-----------------------------------------------------------------------*/

  status = qmi_voice_cmd_list_new_all( cmd, hdl, state,
                                  pending_event_id, sub_id,
                                  sub_ptr, sub_size, as_id );

  if( status != E_SUCCESS)
   	{
   	QM_MSG_ERROR("Could not add entry to the list ");
   	}

  return status;


} /* qmi_voice_cmd_list_new() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_free_all

===========================================================================*/
/*!
    @brief
    Free all entries in the list of outstanding RIL_REQUESTs.
 
    @return
    None
*/
/*=========================================================================*/
void qmi_voice_cmd_list_free_all
( 
  void
)
{
  qmi_voice_cmd_list_buf_type *buf_ptr = cmd_list.head_ptr;
  qmi_voice_cmd_list_buf_type *next_buf_ptr;

  /*-----------------------------------------------------------------------*/

  while ( buf_ptr != NULL )
  {
    next_buf_ptr = buf_ptr->next_ptr;
    buf_ptr->pub.state = QMI_VOICE_CMD_FREE;
    buf_ptr->prev_ptr = NULL;
    buf_ptr->next_ptr = NULL;
    buf_ptr = next_buf_ptr;
  }

  cmd_list.head_ptr = NULL;
  cmd_list.tail_ptr = NULL;

} /* qmi_voice_cmd_list_free_all */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_free

===========================================================================*/
/*!
    @brief
    Frees an entry from the list of outstanding Voice Commands.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_free
( 
  uint32  hdl
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;
  qmi_voice_cmd_list_public_type *pub_ptr;
  qmi_voice_cmd_list_buf_type *prev_buf_ptr, *next_buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
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

    pub_ptr->state = QMI_VOICE_CMD_FREE;

    QM_MSG_MED_1("Deleted cmd list entry :  handle %d", hdl);

  }
  else
  {
    /* Entry was not found */
    status = E_FAILURE;
    QM_MSG_ERROR("Failed to delete cmd list entry ");
  }

  return status;

} /* qmi_voice_cmd_list_free() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_update_state

===========================================================================*/
/*!
    @brief
    Updates the state of the entry in the list for uint32  hdl with the specified 
    information.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_update_state
( 
  uint32  hdl,
  qmi_voice_cmd_state_e_type state
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;
  char *state_name[] = {"", "QMI_VOICE_CMD_FREE", "QMI_VOICE_CMD_AWAITING_CALLBACK", "", "QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS" };

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    /* Invalid state */
    if ( ( state == QMI_VOICE_CMD_FREE ) ||
         ( ( state == QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS ) && ( buf_ptr->pub.pending_event_id == QMI_VOICE_CM_IF_EVT_NONE ) ) )
    {
      status = E_NOT_ALLOWED;
    }
    else
    {
      buf_ptr->pub.state = state;
      QM_MSG_MED_3( "Update cmd list entry : %s , handle %d, state %s", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), 
                      buf_ptr->pub.hdl, state_name[ state ] );
    }
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_cmd_list_update_state() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_update_pending_event_id

===========================================================================*/
/*!
    @brief
    Updates the pending event ID of the entry in the list for uint32  hdl 
    with the specified information.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_update_pending_event_id
( 
  uint32  hdl,
  qmi_voice_cm_if_evt_e_type pending_event_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    buf_ptr->pub.pending_event_id = pending_event_id;
    QM_MSG_MED_3( "Update event field of cmd list entry : %s, handle %d, pending %s ", 
                    qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ),  
                     buf_ptr->pub.hdl, qmi_voice_cm_if_lookup_event_name( pending_event_id ));
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_cmd_list_update_pending_event_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_update_pending_event_and_state

===========================================================================*/
/*!
    @brief
    Updates the pending event ID and state of the entry in the list for uint32  hdl 
    with the specified information.  
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found or fully updated
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_update_pending_event_and_state
(
  uint32  hdl,
  qmi_voice_cm_if_evt_e_type pending_event_id,
  qmi_voice_cmd_state_e_type state
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;
  char *state_name[] = {"", "QMI_VOICE_CMD_FREE", "QMI_VOICE_CMD_AWAITING_CALLBACK", "", "QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS" };
  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );
  if (buf_ptr != NULL)
  {
    buf_ptr->pub.pending_event_id = pending_event_id;
    
    if ( ( state != QMI_VOICE_CMD_FREE ) &&
         !( ( state == QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS ) && ( buf_ptr->pub.pending_event_id == QMI_VOICE_CM_IF_EVT_NONE ) ) )
    {
      buf_ptr->pub.state = state;
      QM_MSG_MED_4("Update cmd list entry: %s, handle %d, state %s, event %s",
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), 
                   buf_ptr->pub.hdl, state_name[state],
                   qmi_voice_cm_if_lookup_event_name(pending_event_id));
    }
    else
    {
      QM_MSG_MED("Only pending event updated. State not updated");
      status = E_FAILURE;
    }
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

}


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_update_sub_id

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
IxErrnoType qmi_voice_cmd_list_update_sub_id
( 
  uint32  hdl,
  uint32 sub_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/
  buf_ptr = qmi_voice_cmd_list_find( hdl );

  if (buf_ptr != NULL)
  {
    buf_ptr->pub.sub_id = sub_id;
    QM_MSG_MED_3( "Update sub id field of cmd list entry : %s , handle %d, sub id %lu", 
                    qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ), 
                     buf_ptr->pub.hdl, sub_id );
  }
  else
  {
    /* handle not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_cmd_list_update_subs_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query

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
IxErrnoType qmi_voice_cmd_list_query
( 
  uint32  hdl,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find( hdl );

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

} /* qmi_voice_cmd_list_query() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_event

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given a pending event ID, and 
    returns it to the requesting subsystem.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_event
( 
  qmi_voice_cm_if_evt_e_type pending_event_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8                          as_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_event( pending_event_id, QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS, as_id);

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

} /* qmi_voice_cmd_list_query_by_event() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_event_and_sub_id

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given a pending event ID, and 
    subsystem ID (e.g. Call ID, SS Ref, Invoke ID etc), and returns it to 
    the requesting subsystem.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id
( 
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8 as_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_event_and_sub_id( pending_event_id, sub_id, QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS, as_id );

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

} /* qmi_voice_cmd_list_query_by_event_and_sub_id() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_event_and_sub_id_all_subs

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given a pending event ID, and 
    subsystem ID across all subscriptions (e.g. Call ID, SS Ref, Invoke ID etc),
    and returns it to the requesting subsystem.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id_all_subs
( 
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_event_and_sub_id_all_subs( pending_event_id, sub_id, QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS );

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

} /* qmi_voice_cmd_list_query_by_event_and_sub_id_all_subs() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_event_and_sub_id_all_states

===========================================================================*/
/*!
    @brief
    Find the first entry in the list based on a cmd event and 
    a subsystem ID (e.g. Call ID, SS Ref, Invoke ID). Ignore the
    state of the cmd.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id_all_states
(
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8 as_id
)
{
  IxErrnoType status = E_FAILURE;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/
  buf_ptr = cmd_list.head_ptr;

  while ( buf_ptr != NULL  )
  {
    if ( ( buf_ptr->pub.pending_event_id == pending_event_id ) &&
         ( buf_ptr->pub.sub_id == sub_id ) &&
         ((as_id == QMI_VOICE_CM_IF_AS_ID_MAX) || (buf_ptr->pub.as_id == as_id)))
    {
      QM_MSG_MED_3( "Found cmd list entry : %s , pending %s , sub id %lu", 
                      qmi_voice_cm_if_lookup_cmd_name( buf_ptr->pub.cmd ),
                      qmi_voice_cm_if_lookup_event_name( pending_event_id ), 
                      sub_id );                 
      *info_ptr = buf_ptr->pub;
      status = E_SUCCESS;
      break;
    }
    else
    {
      buf_ptr = buf_ptr->next_ptr;
    }
  }

  if(E_FAILURE == status)
  {
    QM_MSG_MED_3( "Not found cmd list entry waiting for %s (%lu) and sub id %lu", 
                  qmi_voice_cm_if_lookup_event_name( pending_event_id ), pending_event_id, sub_id );
  }
  return status;
} /* qmi_voice_cmd_list_query_by_event_and_sub_id_all_states() */


/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_event_all_states

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given a pending event ID, and 
    returns it to the requesting subsystem. Ignore state of the requests.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_event_all_states
( 
  qmi_voice_cm_if_evt_e_type     pending_event_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8                          as_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_event( pending_event_id, 
                                         QMI_VOICE_CMD_FREE | QMI_VOICE_CMD_AWAITING_CALLBACK | QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS, as_id );

  if (buf_ptr != NULL)
  {
    *info_ptr = buf_ptr->pub;
  }
  else
  {
    /* Token not in the list */
    status = E_FAILURE;
  }

  return status;

} /* qmi_voice_cmd_list_query_by_event_all_states() */

/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_sub_id

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given  
    subsystem ID (e.g. Call ID, SS Ref, Invoke ID etc), and returns it to 
    the requesting subsystem.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_sub_id
( 
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_sub_id( sub_id, QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS);

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

} /* qmi_voice_cmd_list_query_by_sub_id() */

/*===========================================================================

  FUNCTION:  qmi_voice_cmd_list_query_by_cmd_all_states

===========================================================================*/
/*!
    @brief
    Finds the entry in the list for the given  
    command and returns it to 
    the requesting subsystem.
 
    @return
    E_SUCCESS if the entry was found
    E_FAILURE if the entry was not found
    E_NOT_ALLOWED if the entry was not waiting for an event
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cmd_list_query_by_cmd_all_states
( 
  uint16 cmd,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8  as_id
)
{
  IxErrnoType status = E_SUCCESS;
  qmi_voice_cmd_list_buf_type *buf_ptr;

  /*-----------------------------------------------------------------------*/

  ASSERT( info_ptr != NULL );

  /*-----------------------------------------------------------------------*/

  buf_ptr = qmi_voice_cmd_list_find_by_cmd( cmd, 
                        QMI_VOICE_CMD_FREE|QMI_VOICE_CMD_AWAITING_CALLBACK|QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS, as_id);

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

} /* qmi_voice_cmd_list_query_by_cmd() */


#endif/* !FEATURE_NO_QMI_VOICE */

#endif // FEATURE_QMI_SERVICE_SPLIT

