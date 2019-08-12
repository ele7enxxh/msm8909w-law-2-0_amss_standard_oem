/*===========================================================================

                         D S _ Q M I _ S V C  . C

DESCRIPTION

 The Data Services QMI Services Source file. Contains routines common to
 all QMI services.

 Note this library assumes caller is running in DS task context.

EXTERNALIZED FUNCTIONS

   qmi_svc_prepend_msg_hdr()
     Append the message header type and length to the provided QMI service
     message payload

Copyright (c) 2005-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmi_svc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/31/12    wc     Memory optimization
09/10/12    sb     Len checks for QMI TLVs
07/31/12    sb     Fixed KW Errors.
07/26/12    sb     Fixed KW Errors.
04/09/12    sn     Handling race condition of releasing client(QMI MODEM) by
                   prempting, while transactions are getting used/freed(DCC).
03/29/12    sa     New QMI utils api for spc checks.
04/19/11    kk     Removed improper check for NULL state_handle value.
12/17/10    kk     Added log messages before ASSERTs.
06/19/10    vs     Changes for Dual IP support over single QMI instance
10/23/09    ar     Add supprot for power save indication filtering
10/08/09    kk     Windows7 compliance - DMS changes.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
04/23/07    ic     Factored out common functions from qmi service files
07/06/06    ks     Added generic qmi_svc_get_tl() to get type and length.
09/08/05    ks     Moved QMI_SVC_PKT_PUSH to header file.
06/27/05    jd     Moved QMI_SVC_PKT_PUSH here from header
05/31/05   jd/ks   Code review updates
05/11/05    ks     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "dsm.h"
#include "task.h"
#include "modem_mem.h"
#include "ps_byte.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_ctl.h"
#include "qmi_cmd_process.h"
#include "ds_qmuxi.h"
#include "ps_in.h"
#include "ps_crit_sect.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ds_qmi_cflog.h"
#include "ds_Utils_DebugMsg.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_crit_sect.h"
#include "ps_system_heap.h"
#include <stringl/stringl.h>
#include "ps_utils.h"


/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

extern ps_crit_sect_type rmnet_crit_section;
static qmi_crit_sect_type qmi_svc_pwrsave_crit_section;

/*---------------------------------------------------------------------------
  Power save filtering state information (by QMI instance)
---------------------------------------------------------------------------*/
qmi_svc_pwrsave_info_s_type qmi_svc_pwrsave_info[QMI_INSTANCE_MAX];

/*===========================================================================

                               FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_SVC_PUT_PARAM_TLV()

  DESCRIPTION
    Construct a TLV with the given input type, length and value

  PARAMETERS
    pkt   : message
    type  : value to be put in type field
    len   : value to be put in length field
    value : contents of the value field of TLV

  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_put_param_tlv
(
  dsm_item_type **  pkt,
  uint8             type,
  uint16            len,
  void *            value
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (QMI_SVC_PKT_PUSH(pkt, value, len)) &&
      (QMI_SVC_PKT_PUSH(pkt, &len, sizeof(len))) &&
      (QMI_SVC_PKT_PUSH(pkt, &type, sizeof(type))) )
  {
    return TRUE;
  }
  // free here 
  LOG_MSG_INFO2_0 ("No Memory - Unable to put param TLV");
  return FALSE;

} /* qmi_svc_put_param_tlv() */

/*===========================================================================
  FUNCTION QMI_SVC_PUT_PARAM_TLV2()

  DESCRIPTION
    Construct a TLV with the given input type, length and value

  PARAMETERS
    pkt   : message
    type  : value to be put in type field
    len   : value to be put in length field
    value : contents of the value field of TLV
    file_id : indicated file_id of the caller function.
    line  :indicated line number of file.

  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_put_param_tlv2
(
  dsm_item_type **  pkt,
  uint8             type,
  uint16            len,
  void *            value,
  uint8             file_id,
  unsigned int      line
  
)
{
  if( (QMI_SVC_PKT_PUSH(pkt, value, len)) &&
      (QMI_SVC_PKT_PUSH(pkt, &len, sizeof(len))) &&
      (QMI_SVC_PKT_PUSH(pkt, &type, sizeof(type))) )
  {
    return TRUE;
  }
  LOG_MSG_INFO2_2("No Memory - Unable to put param TLV file id %d,line %d",file_id,line);
  return FALSE;
} /* qmi_svc_put_param_tlv2() */
                                        

/*===========================================================================
  FUNCTION QMI_SVC_PUT_RESULT_TLV()

  DESCRIPTION
    Construct a Result Code TLV with the given result and error codes

  PARAMETERS
    response : response message to append the TLV to
    result   : result code
    error    : error code
    

  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_put_result_tlv
(
  dsm_item_type **   response,
  qmi_result_e_type  result,
  qmi_error_e_type   error
)
{
  PACKED struct PACKED_POST
  {
    uint16 rcode;
    uint16 ecode;
  } v_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (response);

  ds_qmi_cflog_client_resp_result( result, error );

  v_result.rcode = (uint16) result;
  v_result.ecode = (uint16) error;

  return qmi_svc_put_param_tlv(response,
                               QMI_TYPE_RESULT_CODE,
                               sizeof (v_result),
                               &v_result);

} /* qmi_svc_put_result_tlv() */



/*===========================================================================
  FUNCTION QMI_SVC_PREPEND_MSG_HDR()

  DESCRIPTION
    Append the message header type and length to the provided QMI service 
    message payload

  PARAMETERS
    pkt     : message payload
    cmd_val : message type value

  RETURN VALUE
    TRUE
    FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_prepend_msg_hdr
(
  dsm_item_type **  pkt,
  uint16            cmd_val
)
{
  uint16  len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(pkt);

  len = (uint16) dsm_length_packet(*pkt);

  /*-------------------------------------------------------------------------
    Build the QMI generic service header:

      field                          length
    +----------------------------+
    | QMI SVC command number     |    2
    +----------------------------+
    | QMI SVC message length     |    2
    +----------------------------+
    | QMI SVC message payload    |    (length specified in previous field)
    +----------------------------+
  -------------------------------------------------------------------------*/
  if( (QMI_SVC_PKT_PUSH(pkt, &len, sizeof(len))) &&
      (QMI_SVC_PKT_PUSH(pkt, &cmd_val, sizeof(cmd_val))) )
  {
    return TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Freeing the pkt here itself, so we don't need to take care of it in
      caller
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_1("Unable to send Response or ind for cmd %d",cmd_val);
    dsm_free_packet(pkt);
    return FALSE;
  }
} /* qmi_svc_prepend_msg_hdr() */



/*===========================================================================
  FUNCTION QMI_SVC_GET_PARAM_TL()

  DESCRIPTION
    Extract the type and length fields from a message parameter TLV.

  PARAMETERS
    pkt:   pointer to dsm containing tlv
    type:  output - extracted TLV type value
    len:   output - extracted TLV length value

  RETURN VALUE
    TRUE:   Type and length extracted successfully
    FALSE:  Couldn't extract type and length

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_get_param_tl
(
  dsm_item_type **  pkt,
  uint8 *           type,
  uint16 *          len
)
{
  int32 temp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  temp = dsm_pull8( pkt );
  if (-1 == temp)
  {
    return FALSE;
  }
  *type = (uint8) temp; 

  temp = dsm_pull16( pkt );
  if (-1 == temp)
  {
    return FALSE;
  }

  *len = ps_ntohs(temp);

  return TRUE;

} /* qmi_svc_get_param_tl() */



/*===========================================================================
  FUNCTION QMI_SVC_GET_TL()

  DESCRIPTION
    Extract the type and length from the message

  PARAMETERS
    pkt  : message
    type : value in type field
    len  : value in length field

  RETURN VALUE
    TRUE   - extracted ok
    FALSE  - extraction failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_get_tl
(
  dsm_item_type **  pkt,
  uint8 *           type,
  uint16 *          len
)
{
  int32 temp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  temp = dsm_pull8( pkt );
  if (-1 == temp) return FALSE;
  *type = (uint8) temp;
  temp = dsm_pull16( pkt );
  if (-1 == temp) return FALSE;
  *len = ps_ntohs(temp);
  return TRUE;

} /* qmi_svc_get_tl() */


/*===========================================================================
  FUNCTION QMI_SVC_NEW_CMD_BUF()

  DESCRIPTION
    Allocate a QMI command buffer

  PARAMETERS
    x_p : QMI Transaction that the command buf will be added to

  RETURN VALUE
    pointer to allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cmd_buf_type *  qmi_svc_new_cmd_buf
(
  qmi_transaction_type *  x_p
)
{
  qmi_common_svc_state_type *  svc_sp;
  qmi_cmd_buf_type *     cmd_buf_p;
  q_type *               q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);

  if (x_p->n_cmds >= MAX_MSGS_IN_BUNDLE)
  {
    LOG_MSG_INFO1_0 ("Too many commands in transaction");
    return NULL;
  }

  if (x_p->cl_sp)  // cl_sp != NULL means this transaction is client specific
  {
    q_ptr = &x_p->cl_sp->cmd_free_q;
    cmd_buf_p = q_get(q_ptr);
    if (NULL == cmd_buf_p)
    {
      LOG_MSG_INFO1_0 ("No command buffers available!");
      return NULL;
    }
  }
  else /* if no client sp, must be a broadcast indication */
  {
    svc_sp = (qmi_common_svc_state_type *) x_p->svc_sp;
    cmd_buf_p = &svc_sp->bcast_ind_buf;
  }

  cmd_buf_p->x_p    = x_p;
  cmd_buf_p->cmd_no = x_p->n_cmds;
  cmd_buf_p->in_progress = FALSE;
  x_p->cmd_list[x_p->n_cmds] = cmd_buf_p;
  x_p->resp_list[x_p->n_cmds] = QMI_SVC_RESPONSE_PENDING;
  x_p->n_cmds++;

  return cmd_buf_p;
} /* qmi_svc_new_cmd_buf() */



/*===========================================================================
  FUNCTION QMI_SVC_FREE_CMD_BUF()

  DESCRIPTION
    Frees a previously allocated QMI command buffer

  PARAMETERS
    cmd_buf_handle : Handle to command buf pointer which points to the cmd
                     buffer to be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called before.
    *Must be called in the xxx serving task context*
    (xxx depends on service type)

  SIDE EFFECTS
    Removes reference to this command buf from the associated transaction
    structure.  
===========================================================================*/
void qmi_svc_free_cmd_buf
(
  qmi_cmd_buf_type **  cmd_buf_handle
)
{
  qmi_common_client_state_type *  cl_sp;
  qmi_cmd_buf_type *            freeing_cmd_buf_p;
  q_type *                      q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_handle);
  /* get the pointer locally so after we modify the x_p below we don't 
  dereference the null x_p->cmd_list[0] */
  freeing_cmd_buf_p = *cmd_buf_handle; 
  *cmd_buf_handle = NULL; // free the callers reference immediately

  LOG_MSG_INFO3_1 ("Freeing QMI command buffer %x", freeing_cmd_buf_p);

  if (freeing_cmd_buf_p)
  {
    ASSERT (freeing_cmd_buf_p->x_p);
    cl_sp = freeing_cmd_buf_p->x_p->cl_sp;

    /* remove reference to this command buffer from transaction */
    ASSERT(freeing_cmd_buf_p->cmd_no < MAX_MSGS_IN_BUNDLE);
    /* check that cmd_no is in valid range in case ASSERT is removed */
    if (freeing_cmd_buf_p->cmd_no >= MAX_MSGS_IN_BUNDLE)
    {
      LOG_MSG_ERROR_1 ("Freeing cmd_buff cmd no invalid (%d)!",
                       freeing_cmd_buf_p->cmd_no);
      return;
    }

    LOG_MSG_INFO3_3 ("About to clear reference to cmd_buf_p (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );
    ASSERT(freeing_cmd_buf_p);
    /* check that freeing_cmd_buf_p is not NULL in case ASSERT is removed */
    if (freeing_cmd_buf_p == NULL)
    {
      LOG_MSG_ERROR_0 ("Freeing cmd_buff pointer NULL!");
      return;
    }
    
    ASSERT(freeing_cmd_buf_p->x_p);
    /* check that freeing_cmd_buf_p->x_p is not NULL in case ASSERT is removed */
    if (freeing_cmd_buf_p->x_p == NULL)
    {
      LOG_MSG_ERROR_0 ("Freeing cmd_buff transaction pointer NULL!");
      return;
    }

    /* clear the reference to this cmd buffer in transaction to handle
       the case where the response hasn't yet been queued (this would
       also clear the reference) */
    freeing_cmd_buf_p->x_p->cmd_list[freeing_cmd_buf_p->cmd_no] = NULL;

    /* if cmd buffer is freed before response is generated, need to update
       the containing transaction accounting to ensure response is not 
       withheld on account of this freed response */
       
    LOG_MSG_INFO3_3 ("Check if need to clear pending response (%x) from x_p(%x:%d)",
                     freeing_cmd_buf_p,
                     freeing_cmd_buf_p->x_p,
                     freeing_cmd_buf_p->cmd_no );

    if( freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] 
        == QMI_SVC_RESPONSE_PENDING )
    {
      /* indicate that no response is forthcoming for this command */
      freeing_cmd_buf_p->x_p->resp_list[freeing_cmd_buf_p->cmd_no] = 
        (dsm_item_type *) QMI_SVC_RESPONSE_NONE;
    }

    memset( freeing_cmd_buf_p, 0, sizeof (qmi_cmd_buf_type) );
    (void) q_link( freeing_cmd_buf_p, &(freeing_cmd_buf_p->link) );
    if (cl_sp)
    {
      q_ptr = &cl_sp->cmd_free_q;
      q_put( q_ptr, &(freeing_cmd_buf_p->link) );
    }
    else
    {
      LOG_MSG_INFO2_0("Freeing cmd_buf for global transaction");
    }
    
  }
  else
  {
    LOG_MSG_INFO2_0 ("Attempt to free NULL command buffer!");
  }
} /* qmi_svc_free_cmd_buf() */


/*===========================================================================
  FUNCTION QMI_SVC_GET_TRANSACTION()

  DESCRIPTION
    Get a transaction buffer for the specified qmi control point specified
    by the client state pointer provided

  PARAMETERS
    svc_sp :  pointer to service state from which the transaction is allocated
    cl_sp :  client state pointer for client to get transaction buffer for
             or NULL if sending a broadcast indication

  RETURN VALUE
    NULL if no transaction buffer is available
    otherwise, pointer to transaction buffer

  DEPENDENCIES
    If obtaining the global transaction buffer (for broadcast indications),
    the transaction must be freed soon after being allocated, before any
    subsequent call to this function.

  SIDE EFFECTS
    service global transaction might be allocated.
===========================================================================*/
qmi_transaction_type *  qmi_svc_get_transaction
(
  qmi_common_svc_state_type *  svc_sp,
  qmi_common_client_state_type *   cl_sp
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svc_sp);
   
  x_p = NULL;

  if (cl_sp)
  {
    x_p = q_get( &cl_sp->x_free_q );
  }
  else /* if no client sp, must be a broadcast indication */
  {
    /*-----------------------------------------------------------------------
       Check if the global transaction buffer has been freed before 
       allocating it
    -----------------------------------------------------------------------*/
    if(svc_sp->bcast_transaction_buf_free == FALSE)
    {
      /*---------------------------------------------------------------------
       Global transaction buffer SHOULD have been freed before a new request
       to get a global transaction buffer comes
      ---------------------------------------------------------------------*/
      DATA_ERR_FATAL("Broadcast indication in use!");
      return NULL;
    }

    LOG_MSG_INFO2_0 ("allocating the global transaction");
    x_p = &svc_sp->bcast_transaction;
    svc_sp->bcast_transaction_buf_free = FALSE;
  }

  if (x_p)
  {
    x_p->cl_sp = cl_sp;
    x_p->svc_sp = svc_sp;
  }

  return x_p;
} /* qmi_svc_get_transaction() */


/*===========================================================================
  FUNCTION QMI_SVC_FREE_TRANSACTION()

  DESCRIPTION
    Frees the passed transaction buffer, returning it to the client state
    info block if not the global transaction.

  PARAMETERS
    x_p :  handle to transaction pointer pointing to transaction buffer to
           be freed.

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called prior to this Api
    (xxx depends on service)

  SIDE EFFECTS
    Nulls out the caller's transaction buffer pointer
===========================================================================*/
void qmi_svc_free_transaction
(
  qmi_transaction_type **  x_p
)
{
  qmi_common_svc_state_type *     svc_sp;
  qmi_common_client_state_type *  cl_sp;
  int i;
  boolean free_x;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (x_p);

  if (*x_p == NULL)
  {
    LOG_MSG_INFO1_0 ("Attempt to free a NULL transaction pointer!");
    return;
  }
  svc_sp = (qmi_common_svc_state_type *) (*x_p)->svc_sp; 

  if (svc_sp == NULL)
  {
    LOG_MSG_INFO1_0 ("Ignoring free transaction, "
                     "as it is already close to getting freed");
    return;
  }
  
  cl_sp = (*x_p)->cl_sp;

  free_x = TRUE;
  for( i = 0; i < MAX_MSGS_IN_BUNDLE; i++ )
  {
    if( (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_NONE &&
        (*x_p)->resp_list[i] != (dsm_item_type *) QMI_SVC_RESPONSE_PENDING )
    {
      dsm_free_packet( &((*x_p)->resp_list[i]));
    }
    if( (*x_p)->req_list[i] )
    {
      dsm_free_packet( &((*x_p)->req_list[i]) );
    }
    if ((*x_p)->cmd_list[i])
    {
      if( FALSE == (*x_p)->cmd_list[i]->in_progress )
      {
        qmi_svc_free_cmd_buf( &((*x_p)->cmd_list[i]) );
      }
      else
      {
        /*-------------------------------------------------------------------
          don't free cmd buf if it's in process (i.e. currently executing,
          called free_transaction as part of processing, e.g. reset, abort)
        -------------------------------------------------------------------*/
        free_x = FALSE;
        LOG_MSG_INFO2_0 ("Freeing transaction while command in progress - deferred.");
        /* ensure partially freed transaction doesn't get dispatched in this
           case - queued response for pending command should be freed after
           processing is complete. */
      }
    }
  }

  if (free_x)
  {
    memset( *x_p, 0, sizeof(qmi_transaction_type) );

    (void) q_link( *x_p, &((*x_p)->link) );
    (*x_p)->cl_sp = cl_sp;
    (*x_p)->svc_sp = svc_sp;
    if (cl_sp)
    {
      ASSERT (*x_p != &svc_sp->bcast_transaction);
      q_put( &cl_sp->x_free_q, &((*x_p)->link) );
    }
    else
    {
      /*---------------------------------------------------------------------
        this is the global transaction which doesn't need to be queued
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("free the global transaction");
      svc_sp->bcast_transaction_buf_free = TRUE;
    }
    *x_p = NULL;
  }
  else
  {
    LOG_MSG_INFO2_0 ("Transaction in process, "
                     "will be freed once current cmd is completed");
  }
} /* qmi_svc_free_transaction() */



/*===========================================================================
  FUNCTION QMI_SVC_GET_CLIENT_SP_BY_CLID()

  DESCRIPTION
    To obtain cl_sp from svc_sp and clientID
    
  PARAMETERS
    state ptr
    clientID

  RETURN VALUE
    client state ptr

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void *  qmi_svc_get_client_sp_by_clid
(
  qmi_common_svc_state_type *  svc_sp,  
  uint8   clid
)
{
  qmi_common_client_state_type *  cl_sp;
  qmux_svc_info_type *            svc_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  svc_handle = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;

  if (clid == QMI_SVC_CLID_UNUSED || clid == QMI_SVC_CLID_BROADCAST)
  {
    return NULL;
  }

  cl_sp = NULL;

  do 
  {
    cl_sp = svc_handle->cfg.cbs.get_next_client_sp(svc_sp,cl_sp);
    if (cl_sp!= NULL && cl_sp->clid == clid)
    {
      break;
    }
  } while (cl_sp != NULL);

  return cl_sp;

} /* qmi_svc_get_client_sp_by_clid() */



/*===========================================================================
  FUNCTION QMI_SVC_IS_VALID_CLID()

  DESCRIPTION
    Checks validity of the client ID.
                       
  PARAMETERS
    svc_sp:    service provided state pointer (user data)
    clid:  ID allocated to the client, whose validity is to be checked

  RETURN VALUE
    TRUE  :  Client ID is allocated
    FALSE :  Client ID is not allocated
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_is_valid_clid
(
  qmi_common_svc_state_type *   svc_sp,       
  uint8    clid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qmi_svc_get_client_sp_by_clid(svc_sp,clid) != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* qmi_svc_is_valid_clid() */

/*===========================================================================
  FUNCTION QMI_SVC_ALLOC_CLID()

  DESCRIPTION
    Allocate a client ID.  This function is called whenever a QMI_CTL alloc 
    client id message with service type xxx is received.
    (xxx depends on service type)

  PARAMETERS
    svc_sp:  service provided state pointer (user data)

  RETURN VALUE
    Client ID on Sucesss
    Zero      on Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8  qmi_svc_alloc_clid
(
  qmi_common_svc_state_type *  svc_sp
)
{
  qmi_common_client_state_type *  cl_sp;   
  uint8                           assigned_clid;
  qmi_common_client_state_type *  loop_cl_sp;      /* looping client pointer */
  boolean                         in_use;
  qmux_svc_info_type *            svc_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  svc_handle = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;

  cl_sp = NULL;

  do 
  {
    cl_sp = svc_handle->cfg.cbs.get_next_client_sp(svc_sp,cl_sp);
    if (cl_sp != NULL && cl_sp->clid == QMI_SVC_CLID_UNUSED)
    {
      break;
    }
  } while (cl_sp != NULL);

  if (cl_sp == NULL) /* if no unused client, fail */
  {
    LOG_MSG_INFO1_0 ("No available service clids!" );
    return QMI_SVC_CLID_UNUSED;
  }

  /*-------------------------------------------------------------------------
      Attempt to assign Client ID one greater than last assigned, but check
      to make sure we haven't wrapped around and have selected a client id
      that was previously assigned.
  -------------------------------------------------------------------------*/
  do
  {
    assigned_clid = svc_sp->next_clid;
    in_use = FALSE;

    loop_cl_sp = NULL;

    do  
    {
      loop_cl_sp = svc_handle->cfg.cbs.get_next_client_sp(svc_sp,loop_cl_sp);
      if (loop_cl_sp != NULL && loop_cl_sp->clid == assigned_clid)
      {
        /*-------------------------------------------------------------------
          If the client ID was already assigned, we'll increment the next
          client id value and loop.
        -------------------------------------------------------------------*/
        in_use = TRUE;
        break;
      }
    } while (loop_cl_sp != NULL);
    
    /*-----------------------------------------------------------------------
      Increment the next client ID to be assigned.  Ensure client id rolls 
      over to one (skip UNUSED = 0).

      NOTE this logic is premised on specific values for broadcast (255),
      unused (0) and clid field width (8 bits)
    -----------------------------------------------------------------------*/
    svc_sp->next_clid++;
    if (svc_sp->next_clid == QMI_SVC_CLID_BROADCAST) /* go from 254 -> 1 */
    {
      svc_sp->next_clid = QMI_SVC_CLID_UNUSED + 1; /* roll over */
    }
  } while (in_use);

  /*-------------------------------------------------------------------------
    Allocate Client ID (increment counter for next client id)
  -------------------------------------------------------------------------*/
  cl_sp->clid = assigned_clid;
  LOG_MSG_INFO2_2 ("Alloc clid %d for service %x", cl_sp->clid, svc_sp);
  cl_sp->seq_no = 0;

  return (cl_sp->clid);
} /* qmi_svc_alloc_clid() */


/*===========================================================================
  FUNCTION QMI_SVC_DEALLOC_CLIENT()

  DESCRIPTION
    Deallocate ClientID
                       
  PARAMETERS
    svc_sp : Cleint specific xxx service state ptr (user data)
    (xxx depends on service)
    cl_sp :  

  RETURN VALUE
    TRUE  : on Success
    FALSE : on Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void  qmi_svc_dealloc_client
(
  qmi_common_svc_state_type *     svc_sp,
  qmi_common_client_state_type *  cl_sp
)
{
  qmux_svc_info_type *            svc_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (svc_sp != NULL);
  ASSERT (cl_sp != NULL);
  svc_handle = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;

  ASSERT (svc_handle->cfg.cbs.reset_client);
  LOG_MSG_INFO2_1 ("Dealloc service clid %d", cl_sp->clid);

  /*-------------------------------------------------------------------------
    Free resources for client
  -------------------------------------------------------------------------*/
  svc_handle->cfg.cbs.reset_client(cl_sp);
  cl_sp->clid = QMI_SVC_CLID_UNUSED;
  cl_sp->seq_no = 0;
} /* qmi_svc_dealloc_client() */


/*===========================================================================
  FUNCTION QMI_SVC_QMI_LINK_CLOSED()

  DESCRIPTION
    Frees all existing service clients, as QMI link is closed.

  PARAMETERS
    svc_sp : holds xxx service instance state pointer 
         (xxx depends on service)

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_svc_qmi_link_closed
(
  qmi_common_svc_state_type *  svc_sp
)
{
  qmi_common_client_state_type *  cl_sp;
  qmux_svc_info_type *            svc_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svc_sp);
  svc_handle = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;

  LOG_MSG_INFO1_0 ("QMI link closed, do service cleanup");

  cl_sp = NULL;
  do 
  {
    cl_sp = svc_handle->cfg.cbs.get_next_client_sp(svc_sp,cl_sp);
    if (cl_sp!=NULL && cl_sp->clid != QMI_SVC_CLID_UNUSED)
    {
      qmi_svc_dealloc_client( svc_sp, cl_sp );
    }
  } while (cl_sp != NULL); 
} /* qmi_svc_qmi_link_closed() */ 



/*===========================================================================
  FUNCTION QMI_SVC_DEALLOC_CLID()

  DESCRIPTION
    Deallocate ClientID
                       
  PARAMETERS
    state ptr: service provided state pointer (user data)
    clientID : ID of the client to be deallocated

  RETURN VALUE
    TRUE  :on Success
    FALSE :on Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean  qmi_svc_dealloc_clid
(
  qmi_common_svc_state_type *  svc_sp,
  uint8   clid
)
{
  qmi_common_client_state_type *  cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(svc_sp);

  cl_sp = qmi_svc_get_client_sp_by_clid( svc_sp, clid );
  if (cl_sp)
  {
    qmi_svc_dealloc_client( svc_sp,cl_sp );
    return TRUE;
  }
  return FALSE;
} /* qmi_svc_dealloc_clid() */


/*---------------------------------------------------------------------------
  MACRO to check temp return var during message processing
---------------------------------------------------------------------------*/
#define CHECK_TEMP_FOR_FAILURE()  if (temp == -1)               \
                                  { dsm_free_packet( &sdu_in ); \
                                    return; }


/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_POWERUP_INIT()

  DESCRIPTION
    Powerup initialization of QMI_SVC module. (Services need not call this 
    function explicitly)

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_common_svc_powerup_init
(
  void
)
{
  static boolean qmi_svc_pwrup_init = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (FALSE == qmi_svc_pwrup_init)
  {
    /*-----------------------------------------------------------------------
      Initialize critical section for power save indication filtering
    -----------------------------------------------------------------------*/
    QMI_INIT_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
    memset(qmi_svc_pwrsave_info, 0, sizeof(qmi_svc_pwrsave_info));

    qmi_svc_pwrup_init = TRUE;
  }

} /* qmi_svc_common_svc_powerup_init() */

/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_POWERUP_DEINIT()

  DESCRIPTION
    Powerdown the QMI SVC module. 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_common_svc_deinit
(
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_DESTROY_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

}


/*===========================================================================
  FUNCTION QMI_SVC_COMMON_SVC_INIT()

  DESCRIPTION
    Common Service initialization

  PARAMETERS
    sp          :  service provided state pointer (user data)
    qmux_svc_handle :  qmux service handle 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_common_svc_init
(
  void *   sp,
  void *   qmux_svc_handle
)
{
  qmi_common_svc_state_type * svc_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (sp);
  ASSERT(qmux_svc_handle);

  /*-----------------------------------------------------------------------
    Initialize the QMI SVC powersave critical section
  -----------------------------------------------------------------------*/
  qmi_svc_common_svc_powerup_init();

  svc_sp = (qmi_common_svc_state_type *)sp;
  /*-----------------------------------------------------------------------
    client ID's are always non-zero
  -----------------------------------------------------------------------*/
  svc_sp->next_clid = QMI_SVC_CLID_UNUSED + 1;

  svc_sp->qmux_svc_handle = (qmux_svc_info_type *)qmux_svc_handle;

  /*-------------------------------------------------------------------------
    set the boolean to TRUE to indicate that the global trans buffer is free
  -------------------------------------------------------------------------*/
  svc_sp->bcast_transaction_buf_free = TRUE;

} /* qmi_svc_common_svc_init() */

/*===========================================================================
  FUNCTION QMI_SVC_COMMON_CL_INIT()

  DESCRIPTION
    Common client initialization

  PARAMETERS
    sp :  service provided state pointer (user data)
    client_sp  :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_common_cl_init
(
  void * sp,
  void * client_sp
)
{
  qmi_common_svc_state_type * svc_sp;
  qmi_common_client_state_type * cl_sp;
  qmi_cmd_buf_type *            cmd_buf_p;
  qmi_transaction_type *        x_p;
  int                           i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (sp);
  ASSERT (client_sp);

  svc_sp = (qmi_common_svc_state_type *)sp;
  cl_sp = (qmi_common_client_state_type *)client_sp;
  /*-----------------------------------------------------------------------
    initialize client state
    - initialize async command buffers and put them in the free queue
    - initialize the transaction buffers and place in free q.
    - client state pointer in the transaction must always point to the
      client state pointer to ensure even freeing a freed transaction 
      queues the transaction in the correct client state info
  ---------------------------------------------------------------------*/
  memset (&cl_sp->x_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->x_free_q );
  x_p = &cl_sp->transactions[0];
  for (i=MAX_PENDING_TRANSACTIONS; i; i--, x_p++ )
  {
    x_p->svc_sp = svc_sp; 
    x_p->cl_sp = (qmi_common_client_state_type *)cl_sp;   
    (void) q_link( x_p, &x_p->link );
    q_put( &cl_sp->x_free_q, &x_p->link );
  }

  /* initialize the command buffers and and place in free q */
  memset (&cl_sp->cmd_free_q, 0, sizeof(q_type));
  (void) q_init( &cl_sp->cmd_free_q );
  cmd_buf_p = &cl_sp->cmd_buf[0];
  for (i=MAX_PENDING_COMMANDS; i; i--, cmd_buf_p++ )
  {
    (void) q_link( cmd_buf_p, &cmd_buf_p->link );
    q_put( &cl_sp->cmd_free_q, &cmd_buf_p->link );
  }
} /* qmi_svc_common_cl_init() */

/*===========================================================================
  FUNCTION QMI_SVC_COMMON_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    ptr to response
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_common_reset_client
(
  void * cl_sp_in
)
{
  int i;
  qmi_common_client_state_type *  cl_sp;
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp_in);
  cl_sp = (qmi_common_client_state_type *)cl_sp_in;
  /*-------------------------------------------------------------------------
    q_init() is used here as a way to clear the queue and not as a way to 
    initialize the queue.
  -------------------------------------------------------------------------*/ 
  (void) q_init( &cl_sp->x_free_q );
  for( i = 0; i < MAX_PENDING_TRANSACTIONS; i++ )
  {
    x_p = &cl_sp->transactions[i];
    qmi_svc_free_transaction(&x_p );
  }
} /* qmi_svc_common_reset_client() */

/*===========================================================================
  FUNCTION QMI_SVC_INPUT()

  DESCRIPTION
    Process the incoming QMI message and send the response back.
    This function is registered with QMUX.

  PARAMETERS
    svc_sp       : service provided state pointer (user data)
    cmd_buf_p_in : command buffer pointer
    sdu_in    : message to be processed

  RETURN VALUE
    TRUE:   message processed successfully
    FALSE:  invalid message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_svc_input
(
  qmi_common_svc_state_type *    svc_sp,
  qmi_cmd_buf_type *             cmd_buf_p,
  dsm_item_type **               sdu_in
)
{
  qmux_svc_info_type *     svc_handle;
  dsm_item_type *          response_ptr = NULL;
  qmi_svc_cmd_hdlr_type *  cmd_hdlr;
  qmi_svc_hdlr_ftype       hdlr;
  uint16                   cmd;
  boolean                  retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (svc_sp);
  svc_handle = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;
  ASSERT (svc_handle);

  ASSERT (cmd_buf_p);
  ASSERT (sdu_in);

  LOG_MSG_INFO3_2( "Got QMI Service cmd type %d (%d bytes)",
                   cmd_buf_p->cmd_type, dsm_length_packet(*sdu_in));

  /*-------------------------------------------------------------------------
    Checking to see if a cmd hndler is registerd for the input command
  -------------------------------------------------------------------------*/
  cmd_hdlr = svc_handle->cfg.cmd_hdlr_array;
  for (cmd = 0; cmd < svc_handle->cfg.cmd_num_entries; cmd++, cmd_hdlr++)
  {
    if (cmd_buf_p->cmd_type == cmd_hdlr->cmd_type)
    {
      break; 
    }
  }

  if( cmd == svc_handle->cfg.cmd_num_entries )
  {
    LOG_MSG_INFO2_2( "Unrecognized type (=%d) for service (=%d)! Return error",
                     cmd_buf_p->cmd_type, svc_handle->service_id );

    retval = qmi_svc_put_result_tlv( &response_ptr,
                                     QMI_RESULT_FAILURE,
                                     QMI_ERR_INVALID_QMI_CMD );
    if (FALSE == retval)
    {
      dsm_free_packet(&response_ptr);
      response_ptr = NULL;
    }
    goto send_result;
  }

  hdlr = NULL;
  switch (cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE)
  {
    case QMI_FLAG_MSGTYPE_CMD:
      hdlr = cmd_hdlr->request_hdlr;
      break;
    case QMI_FLAG_MSGTYPE_RESP:
      hdlr = NULL; // cmd_hdlr->response_hdlr;
      break;
    case QMI_FLAG_MSGTYPE_IND:
      hdlr = NULL; // cmd_hdlr->indication_hdlr;
      break;
    default:
      LOG_MSG_INFO1_1("Invalid command type! (%d)",
                      cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE);
      qmi_svc_free_cmd_buf(&cmd_buf_p);
      return FALSE;    
  }

  cmd_buf_p->in_progress = TRUE;
  if (hdlr)
  {
    MSG_SPRINTF_1( MSG_SSID_DS, MSG_LEGACY_MED, 
                   "Handling %s", cmd_hdlr->msg_desc);

    ds_qmi_cflog_client_request( svc_handle, cmd_buf_p );

    response_ptr = hdlr( svc_sp, cmd_buf_p, cmd_buf_p->x_p->cl_sp, sdu_in );
  }
  else
  {
    LOG_MSG_INFO2_2 ("Unhandled (%d cmd) msg type (%d)",
                     cmd_buf_p->x_p->ctl&QMI_FLAG_MASK_MSGTYPE,
                     cmd_buf_p->cmd_type);
    response_ptr = NULL;
  }

send_result:
  cmd_buf_p->in_progress = FALSE;

  /*-------------------------------------------------------------------------
    send response if ready.  Check for need to queue this command in 
    pending queue.
  -------------------------------------------------------------------------*/
  if (response_ptr == NULL)
  {
    LOG_MSG_INFO2_0 ("(Service generated no response)");
    qmi_svc_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }
  else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    LOG_MSG_INFO2_1 ("Response pending for cmd_buf_p %p", cmd_buf_p);
    // command buffer will be freed later when response is completed
    return TRUE;
  }

  return qmi_svc_send_response( svc_sp, cmd_buf_p, response_ptr );
} /* qmi_svc_input() */

/*===========================================================================
  FUNCTION QMI_SVC_DISPATCH_TRANSACTION()

  DESCRIPTION
    Dispatches all commands in a passed transaction.  This should be called
    after the recv function has unbundled the request.

  PARAMETERS
    sp  : service provided state pointer (user data)
    x_p : pointer to transaction containing commands to dispatch

  RETURN VALUE
    None

  DEPENDENCIES
    qmi_xxx_init() must have been called previously
    (xxx depends on service type)

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_dispatch_transaction
(
  qmi_common_svc_state_type *  svc_sp,
  qmi_transaction_type *  x_p
)
{
  qmi_cmd_buf_type *  cmd_buf_p;
  dsm_item_type *     msg;
  int                 i;
  boolean             free_t = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Dispatch each of the commands in the transaction
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    cmd_buf_p = x_p->cmd_list[i];
    /*-----------------------------------------------------------------------
      If reset command is present in the bundle, it frees the other commands in the bundle.
      Gracefully handling such freed commands, by continuing through.
      -------------------------------------------------------------------------*/
    if( (i > 0) && (cmd_buf_p == NULL) )
    {
      continue;
    }
    msg = x_p->req_list[i];
    x_p->req_list[i] = NULL;

    /*-----------------------------------------------------------------------
      Dispatch the current SDU/msg transaction
    -----------------------------------------------------------------------*/
    if( FALSE == qmi_svc_input( svc_sp, cmd_buf_p, &msg ) )
    {
      LOG_MSG_INFO2_3 ("Transaction %x Command #%d (%x) processing failed",
                       x_p, i, cmd_buf_p);
      free_t = TRUE;
     //  we should ensure the transaction doesn't stall here.
    }

    /*-----------------------------------------------------------------------
      Ensure the input SDU is freed.  Command handlers don't need to worry
      about freeing the input message.
    -----------------------------------------------------------------------*/
    dsm_free_packet ( &msg );
  }
  if(TRUE == free_t)
  {
    // this indicates that qmi_svc_input() failed for one of the bundled 
    // commands in the transaction. This failure could be either problem
    // with assembling response or invalid command type was sent to the
    // service. In either case right thing to do is to cleanup transaction
    // at this point, otherwise it will never be cleaned up.
    qmi_svc_free_transaction( &x_p );
  }
} /* qmi_svc_dispatch_transaction() */


/*===========================================================================
  FUNCTION QMI_SVC_RECV()

  DESCRIPTION
    Receives the QMUX SDU and passes each individual SVC message to 
    qmi_svc_input. This function is registered with QMUX.
    
  PARAMETERS
    sp     : service provided state pointer (user data)
    clid   : Client ID corresponding the message 
    sdu_in : DSM item containing incoming msg
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_svc_recv
(
  qmi_common_svc_state_type *  svc_sp,
  uint8             clid, 
  dsm_item_type *   sdu_in
)
{
  qmi_common_client_state_type * cl_sp;
  qmi_transaction_type *         x_p;
  qmi_cmd_buf_type *             cmd_buf_p;
  uint16                         msg_x_id;
  uint8                          msg_ctl;
  int32                          temp;
  uint16                         cmd_type;
  uint16                         sdu_len;
  dsm_item_type *                next_sdu;
  uint16                         remaining_bytes;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svc_sp);
  ASSERT(sdu_in);

  cl_sp = (qmi_common_client_state_type *)
            qmi_svc_get_client_sp_by_clid(svc_sp, clid);
  if (NULL == cl_sp)
  {
    // Should never get here since QMUX validates prior to calling this.
    LOG_MSG_ERROR_2 ("Got QMI servce %x message for unassigned client %d! discarding",
                     svc_sp, clid);
    dsm_free_packet (&sdu_in);
    return;
  }

  /*-------------------------------------------------------------------------
    Extract service bundle header - ctl flags and transaction ID apply to 
    all SDUs contained in the message.
  -------------------------------------------------------------------------*/
  temp = dsm_pull8(&sdu_in);
  CHECK_TEMP_FOR_FAILURE();
  msg_ctl  = (uint8) temp;
  temp = dsm_pull16(&sdu_in);
  CHECK_TEMP_FOR_FAILURE();
  msg_x_id = ps_ntohs(temp);

  if (msg_x_id == 0)
  {
    /* This is on the control-point to service path. We don't expect/accept
     * anything but requests here, so there's no additional check needed. */
    LOG_MSG_ERROR_2( "Dropping pkt with x_id '0' (svc_sp = 0x%x, cl_id = %d)",
                     svc_sp, clid );
    dsm_free_packet(&sdu_in);
    return;
  }
  else
  {
    LOG_MSG_INFO3_3( "Recvd request bundle (svc_sp = 0x%x, x_id = %x, ctl = %x)",
                     svc_sp, msg_x_id, msg_ctl);
  }

  /*-------------------------------------------------------------------------
    Get a transaction buffer to track all contained commands in the bundle
  -------------------------------------------------------------------------*/
  x_p = qmi_svc_get_transaction(svc_sp, cl_sp);
  if (x_p == NULL)
  {
    LOG_MSG_INFO1_1 ("No transaction container!  Discarding service %x message",
                     svc_sp);
    dsm_free_packet(&sdu_in);
    return; 
  }

  // copy from temp to allocated buf
  x_p->ctl  = msg_ctl;
  x_p->x_id = msg_x_id;

  /*-------------------------------------------------------------------------                  
    Parse the incoming service message
  -------------------------------------------------------------------------*/
  next_sdu = NULL;
  remaining_bytes = (uint16) dsm_length_packet(sdu_in);
  while (sdu_in)
  {
    /*-----------------------------------------------------------------------
      Extract service message header
    -----------------------------------------------------------------------*/
    temp = dsm_pull16( &sdu_in ); 
    if (temp == -1)
    {
      LOG_MSG_INFO1_0 ("Short QMI svc header!  Discarding transaction");
      dsm_free_packet( &sdu_in );
      qmi_svc_free_transaction( &x_p );
      return;
    }
    cmd_type = ps_ntohs(temp);

    temp = dsm_pull16( &sdu_in );
    if (temp == -1)
    {
      LOG_MSG_INFO1_0 ("Short QMI svc header!  Discarding transaction");
      dsm_free_packet( &sdu_in );
      qmi_svc_free_transaction( &x_p );
      return;
    }
    sdu_len = ps_ntohs(temp);
    remaining_bytes -= 4;

    if (sdu_len > remaining_bytes)
    {
      LOG_MSG_INFO1_1 ("Short QMI svc payload!  Discarding service %x message",
                       svc_sp);
      dsm_free_packet( &sdu_in );
      qmi_svc_free_transaction( &x_p );
      return;
    }
    else if (sdu_len < remaining_bytes)
    {
      /*---------------------------------------------------------------------
        save subsequent SDUs and isolate the current by trimming,
        unbundling individual commands into separate dsm chains
      ---------------------------------------------------------------------*/
      ASSERT (next_sdu == NULL);
      temp = PS_UTILS_DSM_DUP_PACKET( &next_sdu, sdu_in, sdu_len, 65535);      
      if (0 == temp)
      {
        dsm_free_packet( &next_sdu ); // is this needed?
        dsm_free_packet( &sdu_in );
        qmi_svc_free_transaction( &x_p );
        return;
      }
      dsm_trim_packet( &sdu_in, sdu_len );
    }

    /*-----------------------------------------------------------------------
      sdu_in now contains a single command.
      Get a QMI command buffer to hold the command
    -----------------------------------------------------------------------*/
    ASSERT (x_p);
    cmd_buf_p = qmi_svc_new_cmd_buf( x_p );
    if (cmd_buf_p == NULL)
    {
      LOG_MSG_INFO1_1 ("No QMI command buffer!  Discarding service %x message",
                       svc_sp);
      dsm_free_packet(&sdu_in);
      qmi_svc_free_transaction( &x_p );
      return;
      }

    cmd_buf_p->cmd_type = cmd_type;
    x_p->req_list[x_p->n_cmds-1] = sdu_in;

    /*-----------------------------------------------------------------------
      move to next sdu in bundle, if present
    -----------------------------------------------------------------------*/
    sdu_in = next_sdu;
    next_sdu = NULL;
    remaining_bytes -= sdu_len;
  }

  LOG_MSG_INFO2_3 ("Dispatch the transaction(%x) with (%d cmds) ctl= %x",
                   msg_x_id, x_p->n_cmds, msg_ctl);
  qmi_svc_dispatch_transaction( svc_sp, x_p);

} /* qmi_svc_recv() */


/*===========================================================================
  FUNCTION QMI_SVC_QUEUE_RESPONSE()

  DESCRIPTION
    Schedule the response to be sent back to the client.  The bundled 
    response will be sent when all responses have been queued.

  PARAMETERS
    cmd_buf_p :  The command buffer for the request for which the response
                 is being queued.
    msg       :  A DSM chain containing the response

  RETURN VALUE
    TRUE  -  Success
    FALSE -  Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    The command buffer pointed to by cmd_buf_p will be freed.
    The dsm chain referenced by *msg is handled herein.  Caller should 
    never reference the pointer again after this call.
===========================================================================*/
static boolean qmi_svc_queue_response
( 
  qmi_cmd_buf_type **  cmd_buf_p,
  dsm_item_type **     msg
)
{
  qmi_transaction_type *  x_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cmd_buf_p);
  ASSERT (*cmd_buf_p);
  ASSERT ((*cmd_buf_p)->x_p);
  ASSERT (msg);

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = (*cmd_buf_p)->x_p;
  
  /*-------------------------------------------------------------------------
    check if cmd buffer was one of the cmds in the transaction.
  -------------------------------------------------------------------------*/
  if( (*cmd_buf_p) != x_p->cmd_list[(*cmd_buf_p)->cmd_no] )
  {
    /*-----------------------------------------------------------------------
      If cmd was not part of the associated transaction, bad news!
    -----------------------------------------------------------------------*/
    DATA_ERR_FATAL("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet( msg );
    qmi_svc_free_cmd_buf( cmd_buf_p );
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If so, save the response dsm pointer in the transaction
    If response dsm pointer is null, the free command buffer call below
    will set the pointer to 1 to indicate that processing is complete but
    no response was generated.
  -------------------------------------------------------------------------*/
  x_p->cmd_list[(*cmd_buf_p)->cmd_no]  = NULL;
  x_p->resp_list[(*cmd_buf_p)->cmd_no] = *msg;
  *msg = NULL;

  /*-------------------------------------------------------------------------
    The command buffer is no longer needed so free it up for reuse
  -------------------------------------------------------------------------*/
  qmi_svc_free_cmd_buf( cmd_buf_p );

  return TRUE;

} /* qmi_svc_queue_response() */



/*===========================================================================
  FUNCTION QMI_SVC_FILTER_MESSAGE

  DESCRIPTION
    Applies current power save mode indication filter to specified
    message.  Returns boolean result on whether message should be
    dropped.

    When in power save mode, all responses are filtered to avoid
    waking up client processor.  Clients are expected to ensure there
    are no pending transactions before entering power save mode.
    
  PARAMETERS
    qmi_instance    - the QMI instance being used
    svd_id          - QMI service identifier
    msg_type        - whether msg is a cmd, response or indication
    ind_id          - indication identifier
    
  RETURN VALUE
    TRUE if message should be filtered
    FALSE if message should be sent
    
  DEPENDENCIES
    qmi_ctl_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_filter_message
(
  qmi_instance_e_type  qmi_instance,
  qmux_service_e_type  svc_id,
  uint8                msg_type,
  uint16               ind_id
)
{
  qmi_svc_power_spec_s_type  *spec_ptr;
  uint8                       i;
  uint8                       j;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( QMI_INSTANCE_MAX > qmi_instance );
  ASSERT( QMUX_SERVICE_MAX > svc_id );

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    return FALSE;
  }
  
  /* check that svc_id is valid in case ASSERT is removed */
  if (svc_id >= QMUX_SERVICE_MAX )
  {
    LOG_MSG_ERROR_1("QMI service ID not valid (%d)!  Discarding", svc_id);
    return FALSE;
  }
  
  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  spec_ptr = qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr;
  j = svc_id;

  /*-------------------------------------------------------------------------
    If no power save filter is installed, return immediately.
  -------------------------------------------------------------------------*/
  if( NULL == spec_ptr )
  {
    QMI_BRANCH_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Filter all response messages when in power-save state 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
 /*-------------------------------------------------------------------------
    & response_reporting is not configured.
    If response_reporting flag is set to true for the current spec, dont filter
    responses irrespective of the configured power save state.
  -------------------------------------------------------------------------*/
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

  if( QMI_FLAG_MSGTYPE_RESP == msg_type )
  {
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
    if (spec_ptr != NULL && spec_ptr->response_reporting == TRUE)
    {
      QMI_BRANCH_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
      return FALSE;
    }
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
    QMI_BRANCH_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    No registered indications for specified service, block all.
  -------------------------------------------------------------------------*/
  if( (NULL == spec_ptr->svc_inds[j]) || (0 == spec_ptr->svc_inds[j]->num_inds) )
  {
    QMI_BRANCH_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Check current spec to see if passed indication is in registered list.
  -------------------------------------------------------------------------*/
  for( i=0; i < spec_ptr->svc_inds[j]->num_inds; i++ )
  {
    if( ind_id == spec_ptr->svc_inds[j]->ind_list[ i ] )
    {
      /* Indication is in filter list, should be sent. */
      QMI_BRANCH_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
      return FALSE;
    }
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
  
  /* Indication not in spec, should be filtered/dropped. */
  return TRUE;
} /* qmi_svc_filter_message() */



/*===========================================================================
  FUNCTION QMI_SVC_SEND()

  DESCRIPTION
    Send the message to the QMUX layer

  PARAMETERS
    svc_sp    : service provided state pointer (user data)
    cmd_buf_p : cmd_buf pointer for the command that the response is for
    msg_type : message type, whether it is a cmd, response or indication
    msg_ptr   : response to send out

  RETURN VALUE
    TRUE if command was sent (or is queued in transaction for later send)
    FALSE if something was bad

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_send
(
  qmi_common_svc_state_type *  svc_sp,
  qmi_cmd_buf_type *  cmd_buf_p,
  uint8               msg_type,
  dsm_item_type *     msg_ptr
)
{
  qmi_transaction_type *       x_p;
  qmi_svc_hdr_type             hdr;
  int                          i;
  uint8                        clid;
  qmux_svc_info_type *         svc_s;
  qmi_instance_e_type          qmi_instance;
  boolean                      filter_active = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (svc_sp);
  ASSERT (cmd_buf_p);
  ASSERT (msg_ptr);

  /*-------------------------------------------------------------------------
    find this outstanding transaction and save the response.
    If all responses are present, generate the response bundle and send
  -------------------------------------------------------------------------*/
  x_p = cmd_buf_p->x_p;
  ASSERT (x_p);
  
  /*-------------------------------------------------------------------------
    verify that msg_type passed is opposite of that in x_p (issuing)
  -------------------------------------------------------------------------*/
  if( ((x_p->ctl & QMI_FLAG_MASK_MSGTYPE) == QMI_FLAG_MSGTYPE_CMD) &&
      msg_type != QMI_FLAG_MSGTYPE_RESP)
  {
    LOG_MSG_ERROR_0 ("Can't send msg_type %d in response to command!");
    dsm_free_packet( &msg_ptr );
    svc_s = NULL;
    // FALL Though to ensure transaction accounting occurs
  }

  /*-------------------------------------------------------------------------
    Perform filtering on messages based on power save mode
  -------------------------------------------------------------------------*/
  else
  {
    svc_s = (qmux_svc_info_type *)svc_sp->qmux_svc_handle;
    qmi_instance = qmi_instance_by_qmux_handle (svc_s);

    if( qmi_svc_filter_message( qmi_instance,
                                svc_s->service_id,
                                msg_type,
                                cmd_buf_p->cmd_type ) )
    {
      /* Dropping filtered message; clean buffer */
      dsm_free_packet( &msg_ptr );
      
      /* Indicate pending responses should be purged */
      filter_active = TRUE;
      
      LOG_MSG_INFO2_3("Filtered QMI message: type=%d svc=%d msg=%d",
                      msg_type, svc_s->service_id, cmd_buf_p->cmd_type);

      // FALL Though to ensure transaction accounting occurs
    }
  }
  
  ds_qmi_cflog_client_resp( svc_s, cmd_buf_p, msg_type, filter_active );
  
  /*-------------------------------------------------------------------------
    Queue the response (ok if freed above - need to queue regardless for 
    accounting purposes)
  -------------------------------------------------------------------------*/
  if( FALSE == qmi_svc_queue_response( &cmd_buf_p, &msg_ptr ) )
  {
    DATA_ERR_FATAL("Unable to push pending response to send queue");
    ASSERT (msg_ptr == NULL); // msg_ptr is freed in queue_response
    return FALSE;
  }

  ASSERT (cmd_buf_p == NULL);
  ASSERT (msg_ptr == NULL);

  /*-------------------------------------------------------------------------
    If other responses are pending, return now.  Transmission of the bundled
    response will be defered until all responses are "sent" by command hdlrs
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    if( QMI_SVC_RESPONSE_PENDING == x_p->resp_list[i] )
    {
      return TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    If all responses are present, generate the bundle by appending all 
    responses and send the whole out
    dequeue off bundle_resp_q and verify that cmd buffers are correct
  -------------------------------------------------------------------------*/
  for( i = 0; i < x_p->n_cmds; i++ )
  {
    ASSERT( x_p->resp_list[i] != QMI_SVC_RESPONSE_PENDING );
    if( x_p->resp_list[i] != QMI_SVC_RESPONSE_NONE )
    {
      if( filter_active )
      {
        dsm_free_packet( &msg_ptr );
        dsm_free_packet( &x_p->resp_list[i] );
      }
      else
      {
        dsm_append( &msg_ptr, &(x_p->resp_list[i]) );
      }
    }
  }

  if (msg_ptr)
  {
    /*-------------------------------------------------------------------------
      build service header
    -------------------------------------------------------------------------*/
    hdr.ctl_flags = msg_type & QMI_FLAG_MASK_MSGTYPE;

    if( x_p->n_cmds > 1 )
    {
      hdr.ctl_flags |= QMI_FLAG_MASK_COMPOUND;
    }

    (void) hput16( hdr.transaction_id, x_p->x_id );

    if( sizeof(hdr) != PS_UTILS_DSM_PUSHDOWN_PACKED( &msg_ptr,
                                       &hdr,
                                       sizeof (hdr),
                                       DSM_DS_SMALL_ITEM_POOL))
    {
      dsm_free_packet (&msg_ptr);
    }
    else
    { 
      /*---------------------------------------------------------------------
        send the transaction out via QMUX
      ---------------------------------------------------------------------*/
      if (x_p->cl_sp)
      {
        clid = x_p->cl_sp->clid;
      }
      else
      {
        clid = QMI_SVC_CLID_BROADCAST;
      }
      qmi_svc_legacy_svc_send( svc_sp->qmux_svc_handle, clid, msg_ptr );
      msg_ptr = NULL;
    }
  }

  /*-------------------------------------------------------------------------
    free the transaction buffer
  -------------------------------------------------------------------------*/
  qmi_svc_free_transaction( &x_p );  

  return TRUE;
} /* qmi_svc_send() */

/*===========================================================================
  FUNCTION QMI_SVC_SEND_RESPONSE()

  DESCRIPTION
    Send the message to the QMUX layer

  PARAMETERS
    sp    : service provided state pointer (user data)
    cmd_buf_p : cmd_buf pointer for the command that the response is for
    msg_ptr   : response to send out

  RETURN VALUE
    TRUE if command was sent (or is queued in transaction for later send)
    FALSE if something was bad

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_send_response
(
  void *           sp,
  void *           cmd_buf_p_in,
  dsm_item_type *  msg_ptr
)
{  
  qmi_common_svc_state_type *  svc_sp;
  qmi_cmd_buf_type *      cmd_buf_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT (sp);
  ASSERT (cmd_buf_p_in);
  ASSERT (msg_ptr);

  svc_sp = (qmi_common_svc_state_type *)sp;
  cmd_buf_p = (qmi_cmd_buf_type *)cmd_buf_p_in;
  if(NULL == cmd_buf_p)
  {
    return FALSE;
  }
  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type)) 
  {
    LOG_MSG_INFO2_1 ("Unable to send Response for cmd %d",
                     cmd_buf_p->cmd_type);
    /*-----------------------------------------------------------------------
      Note - no need to free msg_ptr, taken care of in 
             qmi_svc_prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    qmi_svc_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }
  return qmi_svc_send(svc_sp, cmd_buf_p, QMI_FLAG_MSGTYPE_RESP, msg_ptr);

} /* qmi_svc_send_response() */

/*===========================================================================
  FUNCTION QMI_SVC_SEND_IND()

  DESCRIPTION
    Sends Indications. For broadcast indications, clid = 255
    
  PARAMETERS
    sp            : service provided state pointer (user data)
    clid          : clientID
    cmd_type      : cmd type
    ind           : indication to send out

  RETURN VALUE
    boolean indicating success/failure
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_send_ind   
(
  void *           sp,
  uint8            clid,
  uint16           cmd_type,
  dsm_item_type *  ind
)
{
  qmi_common_svc_state_type *    svc_sp;
  qmi_common_client_state_type * cl_sp;
  qmi_transaction_type *         x_p;
  qmi_cmd_buf_type *             cmd_buf_p;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (sp);

  svc_sp = (qmi_common_svc_state_type *)sp;
  cl_sp = (qmi_common_client_state_type *)
            qmi_svc_get_client_sp_by_clid(svc_sp, clid);

  if ( (clid != QMI_SVC_CLID_BROADCAST) && (cl_sp == NULL) )
  {
    LOG_MSG_INFO1_1 ("Client %d invalid/already released, dropping indication",
                     clid);
    dsm_free_packet(&ind);
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Build indication from the back to the front
  -------------------------------------------------------------------------*/

  // get an indication command buffer from the indication buffer q
  x_p = qmi_svc_get_transaction(svc_sp, cl_sp);
  if (x_p == NULL)
  {
    LOG_MSG_INFO1_0 ("No transaction buffer to send indication!");
    dsm_free_packet(&ind);
    return FALSE;
  }
  x_p->ctl = QMI_FLAG_MSGTYPE_IND;

  //Skip broadcast indications
  if (cl_sp != NULL)
  {
    //Increment seq no for every indication. Unique per client per service
    cl_sp->seq_no++;
    x_p->x_id = cl_sp->seq_no;
    LOG_MSG_INFO1 ("Client id %d, seq no: %d", cl_sp->clid, x_p->x_id, 0);
  }

  cmd_buf_p = qmi_svc_new_cmd_buf(x_p);
  if (cmd_buf_p == NULL)
  {
    LOG_MSG_INFO1_0 ("No command buffer to send indication!");
    dsm_free_packet(&ind);
    qmi_svc_free_transaction(&x_p);
    return FALSE;
  }

  cmd_buf_p->cmd_type = cmd_type;
  if(FALSE == qmi_svc_prepend_msg_hdr(&ind, cmd_buf_p->cmd_type)) 
  {
    LOG_MSG_INFO2_0 ("Unable to send the indication!");
    /*-----------------------------------------------------------------------
      Note - no need to free msg_ptr, taken care of in 
             qmi_svc_prepend_msg_hdr()
    -----------------------------------------------------------------------*/
    qmi_svc_free_transaction(&x_p);
    return FALSE;
  }

  if( FALSE == qmi_svc_send( svc_sp, cmd_buf_p, QMI_FLAG_MSGTYPE_IND, ind ) )
  {
    LOG_MSG_INFO2_0 ("Unable to send the indication!");
    return FALSE;
  }
  else
  {
    LOG_MSG_INFO2_0 ("Sent the indication (for cmd=%d)!");
  }

  return TRUE;
} /* qmi_svc_send_ind() */

/*===========================================================================
  FUNCTION qmi_svc_get_clid_from_client_sp()

  DESCRIPTION
    Retrieve clid from the client state pointer

  PARAMETERS
    cl_sp  :  client state pointer

  RETURN VALUE
    clid   :  client id

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_svc_get_clid_from_client_sp
(
  void * cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (cl_sp);
  return ( ((qmi_common_client_state_type *)cl_sp)->clid ) ; 
} /* qmi_svc_get_clid_from_client_sp */

/*===========================================================================
  FUNCTION QMI_SVC_GET_CLIENT_SP_BY_CMD_BUF()

  DESCRIPTION
    Retrieve client sp from the command buffer

  PARAMETERS
    cmd_buf_p_in  :  command buffer

  RETURN VALUE
    cl_sp   :  client sp

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * qmi_svc_get_client_sp_by_cmd_buf
(
  void * cmd_buf_p_in
)
{
  qmi_cmd_buf_type * cmd_buf_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == cmd_buf_p_in)
  {
    return NULL;
  }

  cmd_buf_p = (qmi_cmd_buf_type *)cmd_buf_p_in;

  if(NULL == cmd_buf_p->x_p)
  {
    return NULL;
  }

  return cmd_buf_p->x_p->cl_sp;
} /* qmi_svc_get_client_sp_by_cmd_buf() */

/*===========================================================================
FUNCTION QMI_SVC_CMD_BUF_BELONGS_TO_CLIENT_TRANSACTION

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
       the queue.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int qmi_svc_cmd_buf_belongs_to_client_transaction
(
  qmi_cmd_buf_type  *  cmd_buf_p,       // searched value
  qmi_cmd_buf_type  *  comp_param_ptr   // comparison value
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (comp_param_ptr == cmd_buf_p) ? TRUE : FALSE;  
} /* qmi_svc_cmd_buf_belongs_to_client_transaction */

/*===========================================================================
FUNCTION QMI_SVC_CMD_BUF_BELONGS_TO_CLIENT

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
       the queue.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
int qmi_svc_cmd_buf_belongs_to_client
(
  qmi_cmd_buf_type *              cmd_buf_p,
  qmi_common_client_state_type *  cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (cmd_buf_p->x_p->cl_sp == cl_sp) ? TRUE : FALSE;
} /* qmi_svc_cmd_buf_belongs_to_client */


/*===========================================================================
  FUNCTION QMI_SVC_GET_SVC_TYPE_FROM_CMD_BUF

  DESCRIPTION
    To obtain the service type from a given command buffer
    
  PARAMETERS
    cmd_buf   : pointer to the command buffer

  RETURN VALUE
    svc_type  : QMI service type

  DEPENDENCIES
    None

  SIDE EFFECTS
    Returns QMUX_SERVICE_MIN in case an error is encountered
===========================================================================*/
qmux_service_e_type qmi_svc_get_svc_type_from_cmd_buf
(
  qmi_cmd_buf_type *cmd_buf
)
{
  qmi_common_svc_state_type *svc_sp = NULL;
  qmux_svc_info_type        *svc_info = NULL;
  qmux_service_e_type        svc_type = QMUX_SERVICE_MIN;

  if ((cmd_buf != NULL) && (cmd_buf->x_p != NULL))
    svc_sp = (qmi_common_svc_state_type*)cmd_buf->x_p->svc_sp;

  if (svc_sp != NULL)
    svc_info = (qmux_svc_info_type*)svc_sp->qmux_svc_handle;

  if (svc_info != NULL)
    svc_type = svc_info->service_id;

  return svc_type;
} /* qmi_svc_get_svc_type_from_cmd_buf */

/*===========================================================================
  FUNCTION QMI_SVC_LEGACY_SVC_SEND()

  DESCRIPTION
     Send the message from Legacy service to QMUX layer

  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    sdu_out   : Raw QMUX SDU

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_legacy_svc_send
( 
  qmux_svc_info_type * svci,
  uint8                clid, 
  dsm_item_type *      sdu_out
)
{
  qmi_cmd_data_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svci);
  ASSERT(sdu_out);

  cmd_ptr = qmi_get_cmd_data_buf();
  if( cmd_ptr != NULL)
  {
    cmd_ptr->cmd.qmi.id = QMI_CMD_FRAMEWORK_LEGACY_SVC_RESPONSE;
    cmd_ptr->cmd.qmi.data.legacy_svc_resp.clid = clid;
    cmd_ptr->cmd.qmi.data.legacy_svc_resp.svc_handle = svci;
    cmd_ptr->cmd.qmi.data.legacy_svc_resp.sdu_in = sdu_out;
    qmi_send_cmd(QMI_CMD, cmd_ptr);
  }
} /* qmi_svc_legacy_svc_send */

/*===========================================================================
  FUNCTION QMI_SVC_PROCESS_LEGACY_SVC_REQ_CMD()

  DESCRIPTION
    Process the qmi request and route to service handling function in DCC task

  PARAMETERS
    cmd           : DCC cmd ID.
    user_data_ptr : payload to QMI Services running in DCC task.

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_svc_process_legacy_svc_req_cmd
(
  dcc_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr 
)
{
 	qmi_cmd_recv_legacy_svc_req_type    *cmd_ptr = NULL;
  qmux_svc_info_type                  *svci = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	ASSERT(user_data_ptr);
  cmd_ptr = (qmi_cmd_recv_legacy_svc_req_type*) user_data_ptr;

  svci = (qmux_svc_info_type * )cmd_ptr->svc_handle;

  ASSERT(svci);
  ASSERT(cmd_ptr->sdu_in);

  if (svci->cfg.cbs.recv)
  {
    (svci->cfg.cbs.recv)( svci->cfg.sp, (uint8)cmd_ptr->clid , cmd_ptr->sdu_in );
  }
  else
  {
    qmi_svc_recv( svci->cfg.sp, (uint8)cmd_ptr->clid , cmd_ptr->sdu_in );
  }
  modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_DATACOMMON);
} /* qmi_svc_process_legacy_svc_req_cmd */

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_SEND_CLIENT_REG_RESULT()

  DESCRIPTION
    Consolidate the clid alloc/dealloc result then send to QMI CTL.

  PARAMETERS
    reg_result   : Client ID alloc/dealloc result.
    common_hdr   : common msg hdr type.
    qmi_cmd      : QMI cmd type(Alloc or dealloc)

  RETURN VALUE
    None.

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
static void qmi_legacy_svc_send_client_reg_result
(
  qmi_result_e_type                     reg_result,
  qmi_error_e_type                      errval,
  qmi_legacy_svc_common_msg_hdr_type *   common_msg_hdr,
  uint16                                qmi_cmd
)
{
  qmi_cmd_data_buf_type *  cmd_ptr;
/*-------------------------------------------------------------------------*/

  ASSERT(common_msg_hdr);
  ASSERT((qmi_cmd == QMI_CMD_GET_CLIENT_ID_RESP) ||
         (qmi_cmd == QMI_CMD_RELEASE_CLIENT_ID_RESP));  
  
  cmd_ptr = qmi_get_cmd_data_buf();
  if (cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_2 ("No cmd buffer! service %d client ID:%d response message ",
                     common_msg_hdr->service, common_msg_hdr->client_id);
    ASSERT(0);
    return;
  }
  /*-------------------------------------------------------------------------
    Sending client ID response 
  -------------------------------------------------------------------------*/
  cmd_ptr->cmd.qmi.id = qmi_cmd;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.result = reg_result;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.service_id 
                                        = common_msg_hdr->service;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.inst_id  
                                        = (qmi_instance_e_type)common_msg_hdr->qmi_instance;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.cl_id    
                                        = common_msg_hdr->client_id;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info.tx_id    
                                        = common_msg_hdr->transaction_id;
  cmd_ptr->cmd.qmi.data.fw_cl_resp.errval = errval;
  
  qmi_send_cmd(QMI_CMD, cmd_ptr);

} /*qmi_legacy_svc_send_client_reg_result()*/

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_SEND_ALLOC_CLID_RESULT()

  DESCRIPTION
    QMI service calls this function to report client id allocation success /
    failure to QMI_CTL service.

  PARAMETERS
    result     : Client ID allocation result.
    common_hdr : common msg hdr type.

  RETURN VALUE
    None.

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
static void qmi_legacy_svc_send_alloc_clid_result
(
  qmi_result_e_type                     result,
  qmi_legacy_svc_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
)
{
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO2_2 ("Sending alloc clid %d result from QMI Legacy service  %d",
                   common_msg_hdr->client_id, common_msg_hdr->service);

  qmi_legacy_svc_send_client_reg_result( result,
                                         errval,
                                         common_msg_hdr,
                                         QMI_CMD_GET_CLIENT_ID_RESP );  
} /* qmi_legacy_svc_send_alloc_clid_result() */

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_SEND_DEALLOC_CLID_RESULT()

  DESCRIPTION
    QMI service calls this function to report client id deallocation success /
    failure to QMI_CTL service. 

  PARAMETERS
    result     : Client ID deallocation result.
    common_hdr : common msg hdr type.

  RETURN VALUE
    Void.

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None.
==========================================================================*/
static void qmi_legacy_svc_send_dealloc_clid_result
(
  qmi_result_e_type                     result,
  qmi_legacy_svc_common_msg_hdr_type *   common_msg_hdr,
  qmi_error_e_type                      errval
)
{
/*-------------------------------------------------------------------------*/
  ASSERT(common_msg_hdr);

  LOG_MSG_INFO2_2 ("Sending dealloc clid %d result from QMI Legacy service %d",
                   common_msg_hdr->client_id, common_msg_hdr->service);
  
  qmi_legacy_svc_send_client_reg_result( result,
                                         errval,
                                         common_msg_hdr,
                                         QMI_CMD_RELEASE_CLIENT_ID_RESP );

} /* qmi_legacy_svc_send_dealloc_clid_result() */

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_ALLOC_CLID()

  DESCRIPTION
    Allocate a new Client ID to control point. This function is called by QMI_CTL
    service to allocate a client id and client ID allocation is asynchronous.
    On successfull allocation, the service sends a response to ctl by calling
    qmi_legacy_svc_send_alloc_clid_result.

  PARAMETERS
    qmux_svc_handle: Service Handle
    qmux_type: Service 
    clid:    corresponding client id
    tx_id: Transaction ID

  RETURN VALUE
    None.

  DEPENDENCIES
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void  qmi_legacy_svc_alloc_clid
( 
  void * qmux_svc_handle,
  uint8  qmux_type,
  uint8  clid,
  uint8  tx_id
)
{
  qmi_error_e_type	   errval = QMI_ERR_NONE;
  qmi_result_e_type 	 result;
  qmux_svc_config_type * svc_cfg;
  qmi_legacy_svc_common_msg_hdr_type common_msg_hdr;
	
  memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));
  
  svc_cfg = qmux_get_service_cfg( qmux_svc_handle, qmux_type );
  if(NULL == svc_cfg )
  {
    LOG_MSG_ERROR_0("Invalid service handle");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  if (svc_cfg->cbs.alloc_clid) //specific function provided
  {
    common_msg_hdr.client_id = svc_cfg->cbs.alloc_clid(svc_cfg->sp);
  }
  else //specific funtion not provided
  {
    common_msg_hdr.client_id = qmi_svc_alloc_clid(svc_cfg->sp);
  }
	  
  if (!(common_msg_hdr.client_id))
  {
    errval = QMI_ERR_CLIENT_IDS_EXHAUSTED;
    goto send_result;
  }
	
  LOG_MSG_INFO2_1 ("Assigned client ID %d", common_msg_hdr.client_id);
	  
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  
  common_msg_hdr.service = (qmux_service_e_type)qmux_type;
  common_msg_hdr.qmi_instance = qmi_instance_by_qmux_handle(qmux_svc_handle);
  common_msg_hdr.transaction_id = tx_id;
	
  qmi_legacy_svc_send_alloc_clid_result(result, &common_msg_hdr, errval);

}/*qmi_legacy_svc_alloc_clid()*/

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_DEALLOC_CLIID()

  DESCRIPTION
    Handle the release client ID message for QMI Legacy Service.

    Dispatches a request to the specified service to deallocate a client ID.
    Returns the result of the operation to the issuer.

  PARAMETERS
    qmux_svc_handle: Service Handle
    qmux_type: Service 
    clid:    corresponding client id
    tx_id: Transaction ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_legacy_svc_dealloc_clid
(
  void * qmux_svc_handle,
  uint8  qmux_type,
  uint8  clid,
  uint8  tx_id
)
{ 
  qmi_error_e_type       errval = QMI_ERR_NONE;
  qmi_result_e_type      result;
  qmux_svc_config_type * svc_cfg;
  qmi_legacy_svc_common_msg_hdr_type common_msg_hdr;
 
  svc_cfg = qmux_get_service_cfg( qmux_svc_handle, qmux_type );

  if(NULL == svc_cfg )
  {
    LOG_MSG_ERROR_0("Invalid service handle");
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  // if is_valid_clid function is registered, check validity using that function
  // otherwise, check with the qmi_svc_is_valid_clid function
  if ( ((svc_cfg->cbs.is_valid_clid) &&
	     !(svc_cfg->cbs.is_valid_clid(svc_cfg->sp, clid))) ||
	    (!(svc_cfg->cbs.is_valid_clid) &&
	     !(qmi_svc_is_valid_clid(svc_cfg->sp, clid))) )
  {
    errval = QMI_ERR_INVALID_CLIENT_ID;
    goto send_result;
  }

  //specific function provided
  if (svc_cfg->cbs.dealloc_clid)
  {
    if(!(svc_cfg->cbs.dealloc_clid(svc_cfg->sp, clid))) 
    {
	  errval = QMI_ERR_INTERNAL;
	  goto send_result;
    }

    if (!(qmi_svc_dealloc_clid(svc_cfg->sp, clid)))
    {
	  LOG_MSG_INFO2_0("qmi_svc_dealloc_clid() failed after call to service dealloc_clid callback!");
    }

  }else if (!(qmi_svc_dealloc_clid(svc_cfg->sp, clid)))
  { //specific funtion not provided
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  
  memset(&common_msg_hdr, 0, sizeof(common_msg_hdr));
  
  common_msg_hdr.service = (qmux_service_e_type)qmux_type;
  common_msg_hdr.client_id = clid;
  common_msg_hdr.qmi_instance = qmi_instance_by_qmux_handle(qmux_svc_handle);
  common_msg_hdr.transaction_id = tx_id;
  
  qmi_legacy_svc_send_dealloc_clid_result(result, &common_msg_hdr, errval);
}/*qmi_legacy_svc_dealloc_clid()*/

/*===========================================================================
  FUNCTION QMI_LEGACY_SVC_QMI_LINK_CLOSED()

  DESCRIPTION
    Free all existing Legacy service clients, as QMI link is closed.

  PARAMETERS
    instance - the QMI instance whose link was disconnected

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Closes all QMI application state for Legacy Services
===========================================================================*/
void  qmi_legacy_svc_qmi_link_closed
(
  qmi_instance_e_type  instance
)
{
  qmux_clean_legacy_svc_resources(instance);
} /* qmux_clean_instance_resources */


/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_FIND_SPEC

  DESCRIPTION
    Function to find matching spec based on state handle.
  
  PARAMETERS
    qmi_instance    - qmi instance
    state_handle    - power save state handle
    out_spec_ptr    - address of output spec pointer
    
  RETURN VALUE
    TRUE if spec if found, and out_spec_ptr will have valid value
    FALSE otherwise, out_spec_ptr will be NULL
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_svc_pwrsave_find_spec
(
  qmi_instance_e_type            qmi_inst,
  qmi_svc_pwr_state_handle_type  state_handle,
  qmi_svc_power_spec_s_type   ** out_spec_ptr
)
{
  qmi_svc_power_spec_s_type * spec_ptr;
  uint8                       index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *out_spec_ptr = NULL;
  if (qmi_inst >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid qmi inst %d", qmi_inst);
    return FALSE;
  }

  for (index = 0; index < QMI_SVC_PWRSAVE_SPECS_MAX; index++)
  {
    spec_ptr = qmi_svc_pwrsave_info[qmi_inst].specs[index];
    if ( (spec_ptr != NULL) && (spec_ptr->state_handle == state_handle) )
    {
      *out_spec_ptr = spec_ptr;
      return TRUE;
    }
  }

  return FALSE;
} /* qmi_svc_pwrsave_find_spec() */


/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_FIND_EMPTY_SPEC

  DESCRIPTION
    Function to find an empty spec slot.
  
  PARAMETERS
    qmi_instance    - qmi instance
    out_spec_ptr    - address of output spec pointer
    
  RETURN VALUE
    TRUE if spec is allocated, and out_spec_ptr will have valid value
    FALSE otherwise, out_spec_ptr will be NULL
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_svc_pwrsave_find_empty_spec
(
  qmi_instance_e_type            qmi_inst,
  qmi_svc_power_spec_s_type   ** out_spec_ptr
)
{
  qmi_svc_power_spec_s_type * spec_ptr;
  uint8                       index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *out_spec_ptr = NULL;
  if (qmi_inst >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid qmi inst %d", qmi_inst);
    return FALSE;
  }

  for (index = 0; index < QMI_SVC_PWRSAVE_SPECS_MAX; index++)
  {
    spec_ptr = qmi_svc_pwrsave_info[qmi_inst].specs[index];
    if (spec_ptr == NULL)
    {
      PS_SYSTEM_HEAP_MEM_ALLOC(spec_ptr, sizeof(qmi_svc_power_spec_s_type),
                               qmi_svc_power_spec_s_type*);
      if (spec_ptr == NULL)
      {
        return FALSE;
      }
      memset(spec_ptr, 0, sizeof(qmi_svc_power_spec_s_type));
      qmi_svc_pwrsave_info[qmi_inst].specs[index] = spec_ptr;
      *out_spec_ptr = spec_ptr;
      return TRUE;
    }
    else if (spec_ptr->state_handle == QMI_SVC_PWRSAVE_STATE_INT_MIN)
    {
      *out_spec_ptr = spec_ptr;
      return TRUE;
    } 
  }

  LOG_MSG_INFO2_1("No spec slot available inst %d", qmi_inst);
  return FALSE;

} /* qmi_svc_pwrsave_find_empty_spec() */


/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_STORE_SPEC

  DESCRIPTION
    Function to store the power save spec parameters in the QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    state_handle    - power save state handle
    svc_id          - the QMI service
    num_inds        - number of indications in list
    ind_list        - arrary of indications
    
  RETURN VALUE
    TRUE if power save spec successfully stored 
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_pwrsave_store_spec
(
  void                         * service_handle,
  qmi_svc_pwr_state_handle_type  state_handle,
  uint8                          svc_id,
  uint8                          num_inds,
  uint16                       * ind_list
)
{
  qmux_svc_info_type        * svc_s;
  qmi_instance_e_type         qmi_instance;
  qmi_svc_power_spec_s_type * spec_ptr = NULL;
  uint8                       j;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( service_handle );
  ASSERT( ind_list );

  /* check that service_handle is not NULL in case ASSERT is removed */
  if (service_handle == NULL)
  {
    LOG_MSG_ERROR_0 ("QMI_SVC service_handle pointer NULL!");
    return FALSE;
  }

  /* check that ind_list is not NULL in case ASSERT is removed */
  if (ind_list == NULL)
  {
    LOG_MSG_ERROR_0("QMI_SVC ind_list pointer NULL!");
    return FALSE;
  }

  LOG_MSG_INFO3_0("qmi_svc_pwrsave_store_spec");
  
  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);
  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Validate input parameters
  -------------------------------------------------------------------------*/
  if( svc_id >= QMUX_SERVICE_MAX )
  {
    LOG_MSG_INFO2_1("Service specified not valid: %d", svc_id);
    return FALSE;
  }

  // Can't set NORMAL state spec 
  if (state_handle == QMI_SVC_PWRSAVE_STATE_NORMAL)
  {
    LOG_MSG_INFO2_0("Can't set NORMAL state spec");
    return FALSE;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  /*-------------------------------------------------------------------------
    Find a matching or empty spec slot
  -------------------------------------------------------------------------*/
  if (qmi_svc_pwrsave_find_spec(
               qmi_instance, state_handle, &spec_ptr) == FALSE)
  {
    if (qmi_svc_pwrsave_find_empty_spec(qmi_instance, &spec_ptr) == FALSE)
    {
      QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
      return FALSE;
    }
#ifdef FEATURE_DATA_QMI_RESP_FILTERING
    else
    {
      // Make response_reporting as false by default on empty spec
      spec_ptr->response_reporting = FALSE;
    }
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/
  }

  /*-------------------------------------------------------------------------
    Store filter sepcification in the cache
  -------------------------------------------------------------------------*/
  j = svc_id;

  /* Store power save spec */
  LOG_MSG_INFO1_1("Storing power save spec: handle=0x%X", state_handle);
  spec_ptr->state_handle = state_handle;

  /* Free old ind spec */
  if (spec_ptr->svc_inds[j] != NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(spec_ptr->svc_inds[j]);
  }

  /* Allocate new ind spec. One extra element for num_inds */
  PS_SYSTEM_HEAP_MEM_ALLOC(spec_ptr->svc_inds[j], (num_inds + 1) * sizeof(uint16),
                           qmi_svc_ind_spec_s_type*);
  if (spec_ptr->svc_inds[j] == NULL)
  {
    QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
    return FALSE;
  }
  memset(spec_ptr->svc_inds[j], 0, (num_inds + 1) * sizeof(uint16));

  // Copy new ind spec
  spec_ptr->svc_inds[j]->num_inds = num_inds;
  if( num_inds )
  {
    memscpy( spec_ptr->svc_inds[j]->ind_list,
             num_inds * sizeof(uint16),
            ind_list,
            num_inds * sizeof(uint16) );
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  return TRUE;
} /* qmi_svc_pwrsave_store_spec() */



/*===========================================================================
  FUNCTION QMI_SVC_SET_PWRSAVE_STATE_BY_INSTANCE

  DESCRIPTION
    Notify QMUX to update the power save state.  This may result in
    installation of new service indication filter.  If new state is
    normal/full power, filtering will be disabled.

  PARAMETERS  
    qmi_instance      -  the QMI instance being used
    new_state_handle  -  new power save state
    old_state_handle  -  pointer to previous power save state; output
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE on successful operation
    FALSE on error condition

  SIDE EFFECTS
    Indication filtering may be modified
===========================================================================*/
boolean qmi_set_pwrsave_state_by_instance
(
  qmi_instance_e_type             qmi_instance,
  qmi_svc_pwr_state_handle_type   new_state_handle,
  qmi_svc_pwr_state_handle_type * old_state_handle
)
{
  qmi_svc_power_spec_s_type * new_spec_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT( old_state_handle );

  LOG_MSG_INFO3_0("qmi_set_pwrsave_state_by_instance");
  
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("Instance specified not valid: %d", qmi_instance);
    return FALSE;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  /*-------------------------------------------------------------------------
    Check for the normal power case.  Only in this case, power save
    spec will be cleared so the new spec will be NULL.  Otherwise,
    there must be a previously registered power save spec.
  -------------------------------------------------------------------------*/
  if( QMI_SVC_PWRSAVE_STATE_NORMAL != new_state_handle )
  {
    if (qmi_svc_pwrsave_find_spec(
            qmi_instance, new_state_handle, &new_spec_ptr) == FALSE)
    {
      if (qmi_svc_pwrsave_find_empty_spec(
                      qmi_instance, &new_spec_ptr) == FALSE)
      {
        QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
        return FALSE;
      }
      new_spec_ptr->state_handle = new_state_handle;
    }
  }

  /*-------------------------------------------------------------------------
    Capture previous power save state to return
  -------------------------------------------------------------------------*/
  if( NULL != qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr )
  {
    *old_state_handle =
      qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr->state_handle;
  }
  else
  {
    *old_state_handle = QMI_SVC_PWRSAVE_STATE_NORMAL;
  }
  
  /*-------------------------------------------------------------------------
    Check for state transition
  -------------------------------------------------------------------------*/
  if( *old_state_handle != new_state_handle )
  {
    LOG_MSG_INFO1_2("Setting power save state: new=0x%X old=0x%X",
                    new_state_handle, *old_state_handle);
    
    /* Store specified power save spec */
    qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr = new_spec_ptr;
  }

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  return TRUE;
} /* qmi_set_pwrsave_state_by_instance() */


/*===========================================================================
  FUNCTION QMI_SVC_SET_PWRSAVE_STATE

  DESCRIPTION
    Notify QMUX to update the power save state.  This may result in
    installation of new service indication filter.  If new state is
    normal/full power, filtering will be disabled.

  PARAMETERS  
    service_handle    -  qmux service handle returned by qmux_reg_service()
    new_state_handle  -  New power save state
    old_state_handle  -  Pointer to previous power save state; output
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE on successful operation
    FALSE on error condition

  SIDE EFFECTS
    Indication filtering may be modified
===========================================================================*/
boolean qmi_svc_set_pwrsave_state
(
  void                          * service_handle,
  qmi_svc_pwr_state_handle_type   new_state_handle,
  qmi_svc_pwr_state_handle_type * old_state_handle_ptr
)
{
  qmux_svc_info_type        * svc_s;
  qmi_instance_e_type         qmi_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT( service_handle );
  ASSERT( old_state_handle_ptr );

  LOG_MSG_INFO3_0("qmi_svc_set_pwrsave_state");
  
  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);

  if( !qmi_set_pwrsave_state_by_instance( qmi_instance,
                                          new_state_handle,
                                          old_state_handle_ptr ) )
  {
    return FALSE;
  }
  return TRUE;
} /* qmi_svc_set_pwrsave_state() */



/*===========================================================================
  FUNCTION QMI_SVC_GET_PWRSAVE_STATE

  DESCRIPTION
    Query QMUX for the current power save state.

  PARAMETERS  
    service_handle    -  qmux service handle returned by qmux_reg_service()
    state_handle_ptr  -  Pointer to current power save state; output
  
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  RETURN VALUE
    TRUE on successful operation
    FALSE on error condition

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_get_pwrsave_state
(
  void                          * service_handle,
  qmi_svc_pwr_state_handle_type * state_handle_ptr
)
{
  qmux_svc_info_type *     svc_s;
  qmi_instance_e_type  qmi_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( service_handle );
  ASSERT( state_handle_ptr );
  
  LOG_MSG_INFO3_0("qmi_svc_get_pwrsave_state");
  
  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("Instance specified not valid: %d", qmi_instance);
    return FALSE;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  /*-------------------------------------------------------------------------
    Return current power save state
  -------------------------------------------------------------------------*/
  *state_handle_ptr =
       ( qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr ) ?
       qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr->state_handle :
       QMI_SVC_PWRSAVE_STATE_NORMAL;

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
  
  return TRUE;
} /* qmi_svc_get_pwrsave_state() */



/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_EVT_HDLR

  DESCRIPTION
    QMUX power save event handler (runs in QMI task)

  PARAMETERS
    state_handle   - Power save state handle
    data           - Context user data

  RETURN VALUE
    void

  SIDE EFFECTS
    none
===========================================================================*/
void  qmi_svc_pwrsave_evt_hdlr
(
  void*     state_handle,
  void*     data
)
{
  qmi_instance_e_type instance;
  qmi_svc_pwr_state_handle_type old_state_handle = QMI_SVC_PWRSAVE_STATE_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_0("qmi_svc_pwrsave_evt_hdlr");

  /*-------------------------------------------------------------------------
    Process power save event for specified instance.
  -------------------------------------------------------------------------*/
  instance = (qmi_instance_e_type)(uint32)data;
  if (instance >= QMI_INSTANCE_MAX) 
  {
    LOG_MSG_ERROR_1( "Invalid instance specified: %d", instance );
    ASSERT (0);
    return;
  }

  /* Skip uninitialized instances */
  if( !qmi_get_instance_initialized( instance ) )
  {
    LOG_MSG_ERROR_1( "Specified instance initialized, ignoring power event: %d",
                     	 instance );
    return;
  }
    
  /* Update power save filter */
  if( !qmi_set_pwrsave_state_by_instance( instance,
					  state_handle,
					  &old_state_handle ) )
  {
    LOG_MSG_INFO2_1("Failed to set new power state: %d", instance);
  }
  else
  {
    /* Notify client of power state change */
    if( old_state_handle != state_handle )
    {
      if( !qmi_ctl_pwrsave_mode_change_notify( instance,
					       state_handle,
					       old_state_handle ) )
      {
        LOG_MSG_INFO2_1("Failed to notify on power state change: %d",
                        instance);
      }
    }
    else
    {
      LOG_MSG_INFO2_1("No power state change, skipping notification: %d",
                      instance);
    }
  }
  
} /* qmi_svc_pwrsave_evt_hdlr() */



/*=========================================================================
  FUNCTION QMI_SVC_PWR_SAVE_CBACK

  DESCRIPTION
    Notify QMI of power management state change.  
    Used to install Modem event indication filter previously registered 
    by control point.

  PARAMETERS
    qmi_instance   :  QMI instance
    new_state      :  New power save state
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    May change indication filter installed for QMI service(s). 
=========================================================================*/
void qmi_svc_pwr_save_cback
(
  uint8                          qmi_instance,
  qmi_svc_pwr_state_handle_type  new_state
)
{
  qmi_cmd_data_buf_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1( "qmi_svc_pwr_save_cback: state=0x%X", new_state );

  /*-------------------------------------------------------------------------
    Post QMI cmd to host task
  -------------------------------------------------------------------------*/
  if( IS_PWRSAVE_STATE_VALID( new_state ) )
  {
    cmd_ptr = qmi_get_cmd_data_buf();
    if( cmd_ptr != NULL )
    {
      cmd_ptr->cmd.qmi.id = QMI_CMD_PROCESS_POWER_EVT;
      cmd_ptr->cmd.qmi.data.qmi_svc_pwrsave.state_handle = new_state;
      cmd_ptr->cmd.qmi.data.qmi_svc_pwrsave.data         = (void*)(uint32) qmi_instance;

      qmi_send_cmd( QMI_CMD, cmd_ptr );
    }
    else
    {
      LOG_MSG_ERROR_1( "Could not get cmd buffer from QMI task for cmd id %d",
                       QMI_CMD_PROCESS_POWER_EVT );
      ASSERT(0);
    }
  }
}

/*===========================================================================
FUNCTION QMI_SVC_CHECK_SPC()

  DESCRIPTION
    This function checks the SPC and OTKSL. The SPC code itself is used to
    enforce read/write access to protected NV items like ACCOLC etc.

  PARAMETERS
    spc code     - SPC code to be verified
    bCheck_otksl - Flag to specify whether OTKSL needs to be checked
                   if SPC check fails

  RETURN VALUE
    SPC Check Result 

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
qmi_svc_spc_result_e_type qmi_svc_check_spc
(
  uint8 * spc_code,
  boolean bCheck_otksl
)
{
  qmi_svc_spc_result_e_type retval = QMI_SVC_SPC_ERR_INTERNAL;

  if (NULL == spc_code)
  {
    LOG_MSG_ERROR_0( "Unable to function on NULL SPC code" );
    return retval;
  }

  LOG_MSG_INFO1_2( "Initiating check with spc_code_ptr=0x%x and otksl_flag=%d",
                   spc_code, bCheck_otksl );

  retval = (qmi_svc_spc_result_e_type) qmi_if_check_spc(
                                              spc_code, bCheck_otksl );

  return retval;
} /* qmi_svc_check_spc() */

#ifdef FEATURE_DATA_QMI_RESP_FILTERING
/*===========================================================================
  FUNCTION QMI_SVC_PWRSAVE_STORE_RESPONSE_REPORTING

  DESCRIPTION
    Function to store the power save spec response reporting
    parameters in the QMUX instance.
  
  PARAMETERS
    service_handle      - qmux service handle returned by qmux_reg_service()
    state_handle        - power save state handle
    response_reporting  - flag which checks if responses will be reported
                         by the modem during Power Save Mode
 
  RETURN VALUE
    TRUE if power save spec successfully stored response reporting,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_pwrsave_store_response_reporting
(
  void                         * service_handle,
  qmi_svc_pwr_state_handle_type  state_handle,
  boolean                        response_reporting
)
{
  qmux_svc_info_type        * svc_s;
  qmi_instance_e_type         qmi_instance;
  qmi_svc_power_spec_s_type * spec_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check that service_handle & state handle is not NULL in case
     ASSERT is removed */
  if (service_handle == NULL)
  {
    DATA_ERR_FATAL("QMI_SVC service_handle pointer is NULL!");
    return FALSE;
  }

  LOG_MSG_INFO3_0("qmi_svc_pwrsave_store_response_reporting");
  
  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);
  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Validate input parameters
  -------------------------------------------------------------------------*/

  // Can't set NORMAL state spec 
  if (state_handle == QMI_SVC_PWRSAVE_STATE_NORMAL)
  {
    LOG_MSG_INFO2_0("Can't set NORMAL state spec");
    return FALSE;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  /*-------------------------------------------------------------------------
    Find a matching or empty spec slot
  -------------------------------------------------------------------------*/
  if (qmi_svc_pwrsave_find_spec(
               qmi_instance, state_handle, &spec_ptr) == FALSE)
  {
    if (qmi_svc_pwrsave_find_empty_spec(qmi_instance, &spec_ptr) == FALSE)
    {
      QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);
      return FALSE;
    }
  }

  /* Store power save spec */
  LOG_MSG_INFO1_2("Storing power save spec: handle=0x%X, "
                  "response_reporting= %d", state_handle, response_reporting);
  spec_ptr->state_handle = state_handle;
  spec_ptr->response_reporting = response_reporting;

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  return TRUE;
} /* qmi_svc_pwrsave_store_response_reporting() */

/*===========================================================================
  FUNCTION QMI_SVC_GET_RESPONSE_REPORTING_IN_CONFIGURED_PWRSAVE_MODES

  DESCRIPTION
    Function to query for the list of all configured power save mode spec's
    with their corresponsding response reporting parameters in the
    QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    reporting_opt   - pointer to the structure to store the power save spec
                      and response_reporting list
    num_pwr_states -  number of valid configured power save states stored
                      in the QMUX instance
 
  RETURN VALUE
    TRUE if power save spec successfully stored response reporting,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_get_response_reporting_in_configured_pwrsave_modes
(
  void                         * service_handle,
  void                         * reporting_opt,
  uint8                        * num_pwr_states
)
{
  qmi_svc_power_spec_s_type     *spec_ptr = NULL;
  qmux_svc_info_type            * svc_s;
  byte                          **reporting_opt_out;

  qmi_instance_e_type           qmi_instance;
  uint8                         index             = 0;
  uint8                         total_power_states = 0;

/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO3_0("qmi_svc_get_response_reporting_in_configured_pwrsave_modes");
  
  /* check that service_handle,reporting_opt,num_pwr_states is
     not NULL in case ASSERT is removed */
  if (service_handle == NULL || reporting_opt == NULL || num_pwr_states == NULL)
  {
    DATA_ERR_FATAL("QMI_SVC service_handle pointer NULL!");
    return FALSE;
  }

  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);

  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    return FALSE;
  }

  reporting_opt_out = (byte **) reporting_opt;

  total_power_states = 0;
  (*reporting_opt_out)++; //leave 1st location for num_pwrsave_state

/*---------------------------------------------------------------------------
  For each power state spec, store the power state and response reporting flag
---------------------------------------------------------------------------*/
  for (index = 0; index < QMI_SVC_PWRSAVE_SPECS_MAX; index++)
  {
    spec_ptr = qmi_svc_pwrsave_info[qmi_instance].specs[index];
    if ((spec_ptr != NULL))
    {
      //store them
       total_power_states++;

       (*reporting_opt_out) += memscpy((*reporting_opt_out), 
                sizeof(qmi_svc_pwr_state_handle_type),
                &spec_ptr->state_handle, 
                sizeof(spec_ptr->state_handle));

       (*reporting_opt_out) += memscpy((*reporting_opt_out), 
                sizeof(boolean),
                &spec_ptr->response_reporting, 
                sizeof(spec_ptr->response_reporting));
    }
  }
  *num_pwr_states = total_power_states;

  return TRUE;
} /*qmi_svc_get_response_reporting_in_configured_pwrsave_modes() */

/*===========================================================================
  FUNCTION QMI_SVC_GET_RESPONSE_REPORTING_IN_CURRENT_PWRSAVE_MODE

  DESCRIPTION
    Function to get the power save spec response reporting parameter of the
    current power save state in the QMUX instance.
  
  PARAMETERS
    service_handle  - qmux service handle returned by qmux_reg_service()
    response_reporting - flag stored which checks if responses will be reported
                         by the modem during Power Save Mode
 
  RETURN VALUE
    TRUE if power save spec response reporting is successfully returned,
    FALSE otherwise
    
  DEPENDENCIES
    qmux_init() must have been called on 'qmi_instance' previously

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_get_response_reporting_in_current_pwrsave_mode
(
  void                         * service_handle,
  boolean                      * reponse_reporting
)
{
  qmux_svc_info_type            * svc_s;
  qmi_instance_e_type             qmi_instance;

/*-------------------------------------------------------------------------*/

  /* check that service_handle is not NULL in case ASSERT is removed */
  if (service_handle == NULL || reponse_reporting == NULL)
  {
    DATA_ERR_FATAL("QMI_SVC service_handle pointer NULL!");
    return FALSE;
  }

  LOG_MSG_INFO3_0("qmi_svc_get_response_reporting_in_current_pwrsave_mode");
  
  svc_s = (qmux_svc_info_type *) service_handle;
  qmi_instance = qmi_instance_by_qmux_handle (svc_s);
  ASSERT (qmi_instance < QMI_INSTANCE_MAX);

  /* check that qmi_instance is within range in case ASSERT is removed */
  if (qmi_instance >= QMI_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_1("QMI instance out of range (%d)!  Discarding",
                    qmi_instance);
    return FALSE;
  }

  QMI_ENTER_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

  /*-------------------------------------------------------------------------
    Return current power save state
  -------------------------------------------------------------------------*/
  *reponse_reporting =
       ( qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr ) ?
       qmi_svc_pwrsave_info[qmi_instance].cur_spec_ptr->response_reporting :
       FALSE;

  QMI_LEAVE_CRIT_SECTION(&qmi_svc_pwrsave_crit_section);

/*-------------------------------------------------------------------------*/

  return TRUE;
} /*qmi_svc_get_response_reporting_in_current_pwrsave_mode() */
#endif /*FEATURE_DATA_QMI_RESP_FILTERING*/

/*==========================================================================
FUNCTION QMI_SVC_VALIDATE_TYPE_LEGTH()

DESCRIPTION
  This function validates the length based on passed expected and length params.

PARAMETERS
  type - param type
  length - recieved length of specific value.
  expected_len - expected length of value.
  file_id - specifies file id of the caller.
  line - line number of the caller.
  
RETURN VALUE
  returns true if both lengths are equal.
  
DEPENDENCIES
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_svc_validate_type_length
(
  uint8 type,
  uint32 length,
  uint32 expected_len,
  uint8 file_id,
  uint16 line
)
{
  if ((expected_len != 0) && (expected_len != length))
  {
    LOG_MSG_INFO2_4("Invalid TLV len (%d) for type (%d) file id (%d) line (%d)",length,type,file_id,line);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
  FUNCTION QMI_SVC_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI SVC message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to C structure to contain decoded message
    p_req_size  Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, 
    otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_svc_idl_message_decode
 ( 
   qmi_idl_service_object_type svc, 
   uint16_t                    message_id, 
   dsm_item_type**             msg_ptr, 
   void*                       p_req, 
   uint32_t                    p_req_size
 )
 {
   qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
   int32_t            retval = QMI_IDL_LIB_NO_ERR;
   uint32             size;
   uint8              *buf = NULL;
  /*--------------------------------------------------------------------------------*/
   if( msg_ptr != NULL && *msg_ptr != NULL ) // We reutrn QMI_ERR_NONE_V01 if msg_ptr is NULL. 
   {
     size = dsm_length_packet( *msg_ptr );
     LOG_MSG_INFO1_1("decode:dsm_pullup size %d",size);

     if ( size > 0 )
     {
       PS_SYSTEM_HEAP_MEM_ALLOC(buf,size,uint8*);
       if ( buf != NULL )
       {
         if ( size == dsm_pullup( msg_ptr, buf, size ) )
         {
           retval = qmi_idl_message_decode( svc,
                                            QMI_IDL_REQUEST,     // IDL message type
                                            message_id,          // QMI SVC cmd type
                                            (void*)buf,          // pointer to incoming byte stream
                                            (uint32_t)size,      // pointer to no of bytes to be read
                                            p_req,               // pointer to c structure
                                            p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                          );
         }
         else
         {
           retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
           LOG_MSG_ERROR_1("decode: dsm_pullup failed size %d", size);
         }
         PS_SYSTEM_HEAP_MEM_FREE(buf);
       }
       else
       {
         LOG_MSG_ERROR_0("decode: no memory for buffer");
         return QMI_ERR_NO_MEMORY_V01;
       }
     }
     else
     {
       errval = QMI_IDL_LIB_EXTENDED_ERR;
     }
   
     switch( retval )
     {  
       case QMI_IDL_LIB_NO_ERR:
         errval = QMI_ERR_NONE_V01;
         break;
 
       case QMI_IDL_LIB_ARRAY_TOO_BIG:
         errval = QMI_ERR_ARG_TOO_LONG_V01;
         break;
 
       case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
         errval = QMI_ERR_NOT_SUPPORTED_V01;
         break;
 
       case QMI_IDL_LIB_TLV_DUPLICATED :
       case QMI_IDL_LIB_LENGTH_INCONSISTENCY:
       case QMI_IDL_LIB_UNKNOWN_MANDATORY_TLV:
         errval = QMI_ERR_MALFORMED_MSG_V01;
         break;
       case QMI_IDL_LIB_MISSING_TLV:
         errval = QMI_ERR_MISSING_ARG_V01;
         break;
       case QMI_IDL_LIB_EXTENDED_ERR:
       case QMI_IDL_LIB_BUFFER_TOO_SMALL:
       case QMI_IDL_LIB_PARAMETER_ERROR:
       case QMI_IDL_LIB_UNRECOGNIZED_SERVICE_VERSION:
       case QMI_IDL_LIB_INCOMPATIBLE_SERVICE_VERSION:
       default:
         errval = QMI_ERR_INTERNAL_V01;
         break;
     }
 
     if ( retval != QMI_IDL_LIB_NO_ERR )
     {
       LOG_MSG_ERROR_2("decode: failed retval (%d) errval (%d)", retval, errval);
     }
   }
    return errval;
 }/* qmi_svc_idl_message_decode */

/*===========================================================================
  FUNCTION QMI_SVC_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI SVC message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_svc_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void*                        p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type**              p_response 
)
{
  qmi_error_e_type    eval        = QMI_IDL_LIB_NO_ERR;
  int32_t             err_val     = QMI_IDL_LIB_NO_ERR;
  uint32_t            rsp_buf_len = 0;
  uint8 *             rsp_buf     = NULL;
  uint32_t            size_buf    = 0;
  boolean             retval      = FALSE;

  /*--------------------------------------------------------------------------------*/
  if( p_response != NULL)
  {
    *p_response = NULL;
  }
  else
  {
    return FALSE;
  }
  /*--------------------------------------------------------------------------------
    Accessor function for getting the maximum message length of a response
    message. Message length includes c struct size, size of type and size of length.
  ----------------------------------------------------------------------------------*/
  eval = qmi_idl_get_max_message_len( svc, type, message_id, &rsp_buf_len );
  
  if ( eval == QMI_IDL_LIB_NO_ERR )
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(rsp_buf,rsp_buf_len,uint8*);
    if ( rsp_buf != NULL && p_src != NULL)
    {
      memset(rsp_buf, 0x00, rsp_buf_len);
      err_val = qmi_idl_message_encode( svc,
                                      type,         // IDL message type
                                      message_id,   // QMI SVC cmd type
                                      p_src,        // pointer to c structure
                                      p_src_size,   // size of c structure which is to be encoded
                                      rsp_buf,      // buffer to contain encoded message data
                                      rsp_buf_len,  // size of buffer, should be maximum size of the encoded c structure
                                      &size_buf     // pointer to store length of the encoded message
                                    );

      if ( err_val == QMI_IDL_LIB_NO_ERR )
      {
        if ( size_buf != dsm_pushdown_packed( p_response, &rsp_buf[0], size_buf, DSM_DS_SMALL_ITEM_POOL) )
        {
          LOG_MSG_ERROR_1("Pushdown Failed for encode %d", size_buf);
          dsm_free_packet(p_response);
        }
        else
        {
          retval = TRUE;
        }
      }
    }
  }
  LOG_MSG_INFO1_3("encode eval (%d) err_val (%d) retval (%d)",eval,err_val,retval);
  if ( rsp_buf != NULL ) 
  {
    PS_SYSTEM_HEAP_MEM_FREE(rsp_buf);
  }
  return retval;
}/* qmi_svc_idl_message_encode */


