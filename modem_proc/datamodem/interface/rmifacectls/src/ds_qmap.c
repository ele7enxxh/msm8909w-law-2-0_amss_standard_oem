/*===========================================================================

                         D S _ Q M A P . C

DESCRIPTION

  QMAP (Qualcomm Mux and Aggregation) source file.

EXTERNALIZED FUNCTIONS

  ds_qmap_send_cmd()
    Send a QMAP command

Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_qmap.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
05/24/13    wc     Initial file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsm_item.h"
#include "ps_in.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_qmap.h"
#include "queue.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"
#include "ps_system_heap.h"
#include "ds_qmi_task.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_nv_util.h"
#include "ds_rmnet_shim.h"
#include "stringl/stringl.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

#define DS_QMAP_CMD_RESPONSE_TIME     (20)   // Response timer in ms
#define DS_QMAP_CMD_MAX_TIMEOUT       (5000) // Max time out in ms
#define DS_QMAP_CMD_MAX_RETRY         (3)   // Max retry

static q_type  qmap_cmd_pend_q;
static uint32  qmap_cmd_id;         
static uint32  qmap_cmd_resp_time;

typedef PACKED struct PACKED_POST
{
  uint8                  reserved;
  uint8                  ip_family;
  uint16                 seq_num;
  uint32                 qos_id;
} fc_payload_type;

/*---------------------------------------------------------------------------
  Data type used to qmap cmd resp functions
---------------------------------------------------------------------------*/
typedef struct
{
  sio_stream_id_type         stream_id;
  uint8                      cmd_name;
  uint8                      cmd_type;
  uint32                     tx_id;
  union
  {
    fc_payload_type          fc;
  } payload;
} ds_qmap_cmd_resp_type;

/*---------------------------------------------------------------------------
  QMAP command buffer type
---------------------------------------------------------------------------*/
typedef struct
{
  uint16       cmd_id;

  union
  {
    struct
    {
      sio_stream_id_type    stream_id;
      uint8                 cmd_name;
      uint8                 cmd_type;
      uint32                tx_id;
      dsm_item_type       * dsm_ptr;
    } qmap_resp;

    struct
    {
      uint32    search_id;
    } timer_info;

  } qmap;

} ds_qmap_cmd_buf_type;

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

static void ds_qmap_build_fc_cmd
(
  ds_qmap_cmd_flow_ctrl_type   * qmap_fc_cmd,
  uint8                          cmd,
  uint8                          ip_family,
  uint16                         seq_num,
  uint32                         qos_id
);

static void ds_qmap_process_cmd
(
  void * cmd_ptr
);

static void * ds_qmap_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
);

#define ds_qmap_free_cmd_buf(buf_ptr) PS_SYSTEM_HEAP_MEM_FREE(buf_ptr)

static void ds_qmap_cmd_timer_cback
(
  timer_cb_data_type  user_data
);

static boolean ds_qmap_is_cmd_expired
(
  ds_qmap_cmd_info_type  * current_info_ptr,
  ds_qmap_cmd_info_type  * pending_info_ptr
);

static void ds_qmap_process_qmap_resp
(
  sio_stream_id_type          stream_id,
  uint8                       cmd_name,   
  uint8                       cmd_type,
  uint32                      tx_id,
  dsm_item_type             * dsm_ptr
);

static void ds_qmap_process_timer_exp
(
  uint32 search_id
);

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS_QMAP_INIT()

  DESCRIPTION
    This function initializes the QMAP related functions.
                                                                                        
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmap_init
(
  void
)
{
   // Initialize QMAP command response pending queue
  memset(&qmap_cmd_pend_q, 0, sizeof(qmap_cmd_pend_q));
  (void) q_init( &qmap_cmd_pend_q );

  qmi_task_set_qmap_cmd_handler(ds_qmap_process_cmd);

  if (qmi_nv_read(QMI_NV_ITEM_QMAP_CMD_RESP_TIME, 0, 0,
                  &qmap_cmd_resp_time,
                  sizeof(qmap_cmd_resp_time)) != QMI_NV_STATUS_OK)
  {
    qmap_cmd_resp_time = DS_QMAP_CMD_RESPONSE_TIME;
  }

  // Register a call back func when a response for a cmd is received
  if (qmap_cmd_resp_time > 0)
  {
    if (!rmnet_shim_register_qmap_cmd_cb())
    {
      LOG_MSG_ERROR_0("Failed to registered QMAP cmd callback");
    }
  }
}

/*===========================================================================
  FUNCTION DS_QMAP_SEND_CMD()

  DESCRIPTION
    This function sends a QMAP command.

  PARAMETERS
    port_id:               SIO port id
    mux_id:                QMAP mux_id
    cmd_size:              QMAP command size
    cmd_ptr:               QMAP command pointer
    cmd_info_ptr:          Ptr to the retransmitted pkt in queue/
                           NULL if not a retransmitted pkt
    

  RETURN VALUE
    TRUE/FALSE 

  DEPENDENCIES
    SIO port must be opened and QMAP data format is enabled.

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_qmap_send_cmd
(
  rmnet_xport_type       * xport_info,
  uint16                   cmd_size,
  void                   * cmd_ptr,
  ds_qmap_cmd_info_type  * cmd_info_ptr
)
{
  uint8                         cmd;
  uint8                         cmd_type;
  dsm_item_type               * pkt = NULL;
  boolean                       result = FALSE;
  ds_qmap_cmd_info_type       * qmap_cmd_info = NULL;
  int                           q_count = 0;
  ds_qmap_cmd_info_type       * item_ptr = NULL;
  ds_qmap_cmd_flow_ctrl_type  * qmap_fc = NULL;
  uint32                        tx_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (xport_info == NULL || cmd_ptr == NULL ||
                     cmd_size < DS_QMAP_CMD_PAYLOAD_OFFSET)
  {
    LOG_MSG_ERROR_3("Invalid parameters %p %p %d",
                    xport_info, cmd_ptr, cmd_size);
    return FALSE;
  }

  cmd = *((uint8 *)cmd_ptr + DS_QMAP_CMD_OFFSET);
  cmd_type = (*((uint8 *)cmd_ptr + DS_QMAP_CMD_TYPE_OFFSET)) & 0x03;
  tx_id = cmd_info_ptr ? cmd_info_ptr->tx_id : 0;

  // Construct payload packet
  pkt = dsm_offset_new_buffer(
              DSM_DS_SMALL_ITEM_POOL, DS_QMAP_CMD_PAYLOAD_OFFSET);
  if (pkt == NULL)
  {
    LOG_MSG_ERROR_0("Unable to get a new DSM item");
    return FALSE;
  }

  if (dsm_pushdown_packed(
               &pkt,
               (uint8 *)cmd_ptr + DS_QMAP_CMD_PAYLOAD_OFFSET,
               cmd_size - DS_QMAP_CMD_PAYLOAD_OFFSET,
               DSM_DS_SMALL_ITEM_POOL) != cmd_size - DS_QMAP_CMD_PAYLOAD_OFFSET)
  {
    LOG_MSG_ERROR_1("Failed to build DSM pkt for QMAP cmd %d", cmd);
    return FALSE;
  }

  // Send QMAP commmand
  result = rmnet_shim_send_qmap_cmd(
                xport_info,
                cmd,
                cmd_type,
                pkt,
                &tx_id,
                cmd_info_ptr ? TRUE : FALSE);
  if (!result)
  {
    LOG_MSG_ERROR_2("Failed to send QMAP cmd [%d] to SIO port 0x%x",
                    cmd, xport_info->sio_port);
    dsm_free_packet(&pkt);
    return result;
  }

  if (DS_QMAP_IS_FC_CMD(cmd))
  {
    qmap_fc = (ds_qmap_cmd_flow_ctrl_type *)cmd_ptr; 
    LOG_MSG_INFO1_6("Sent QMAP cmd [%d] IP [%d] seq [%d] "
                    "qos [0x%x] strm [0x%x] tx_id [0x%x]",
                    cmd,
                    qmap_fc->ip_family,
                    ps_ntohs(qmap_fc->seq_num),
                    ps_ntohl(qmap_fc->qos_id),
                    xport_info->stream_id,
                    tx_id);
  }
  else
  {
    LOG_MSG_INFO1_2("Sent QMAP cmd [%d] to stream 0x%x",
                    cmd, xport_info->stream_id);
    return result;  // Handle other response in the future
  }

  // If response timer is configured to be 0, no response is expected
  if (qmap_cmd_resp_time == 0)
  {
    return result;
  }

  if (!cmd_info_ptr)
  {
    qmap_cmd_id++;

    // Copy into qmap flow cmd info structure
    PS_SYSTEM_HEAP_MEM_ALLOC(qmap_cmd_info, sizeof(ds_qmap_cmd_info_type),
                             ds_qmap_cmd_info_type*);
    
    if (qmap_cmd_info == NULL)
    {
      return result;
    }

    memset(qmap_cmd_info, 0, sizeof(ds_qmap_cmd_info_type));
    qmap_cmd_info->xport_info = xport_info;
    qmap_cmd_info->tx_id = tx_id;
    qmap_cmd_info->cmd_size = cmd_size;
    qmap_cmd_info->qmap_cmd_id = qmap_cmd_id;
    qmap_cmd_info->stream_id = xport_info->stream_id;
    qmap_cmd_info->timeout = qmap_cmd_resp_time;
    PS_SYSTEM_HEAP_MEM_ALLOC(qmap_cmd_info->cmd_ptr, cmd_size, void*);
    if (qmap_cmd_info->cmd_ptr != NULL)
    {
      memscpy(qmap_cmd_info->cmd_ptr, cmd_size, cmd_ptr, cmd_size);
    }
    else
    {
      PS_SYSTEM_HEAP_MEM_FREE(qmap_cmd_info);
      return result;
    }

    // Initialize timer
    timer_def( &(qmap_cmd_info->qmap_cmd_timer),
           NULL,
           NULL,
           0,
           ds_qmap_cmd_timer_cback,
           (timer_cb_data_type)qmap_cmd_id );
  }
  else
  {
    // Just queue it back; Previous retx, double timeout.
    qmap_cmd_info = cmd_info_ptr;
    qmap_cmd_info->timeout <<= 1;
  }
    
  // Clean pending requests
  q_count = q_cnt(&qmap_cmd_pend_q);
  while (q_count--)
  {
    item_ptr = (ds_qmap_cmd_info_type *) q_get(&qmap_cmd_pend_q);

    if (item_ptr == NULL)
    {
      LOG_MSG_ERROR_0("Invalid queue entry ! Skipping");
    }
    else if ( !ds_qmap_is_cmd_expired(qmap_cmd_info, item_ptr) )
    {
      // Put back into the queue
      q_link(item_ptr, &(item_ptr->link));
      q_put(&qmap_cmd_pend_q, &(item_ptr->link));
    }
    else
    {
      // Old pending cmd; Free the memory
      (void) timer_clr( &(item_ptr->qmap_cmd_timer), T_NONE );
      timer_undef( &(item_ptr->qmap_cmd_timer) );
      PS_SYSTEM_HEAP_MEM_FREE(item_ptr->cmd_ptr);
      PS_SYSTEM_HEAP_MEM_FREE(item_ptr);
    }
  }
   
  // Add the new cmd to pending queue
  q_link(qmap_cmd_info, &(qmap_cmd_info->link));
  q_put(&qmap_cmd_pend_q, &(qmap_cmd_info->link));   

  // Start timer
  if (qmap_cmd_info->timeout > DS_QMAP_CMD_MAX_TIMEOUT)
  {
    qmap_cmd_info->timeout = DS_QMAP_CMD_MAX_TIMEOUT;
  }
  (void) timer_set( &(qmap_cmd_info->qmap_cmd_timer),
                         qmap_cmd_info->timeout, 0, T_MSEC );

  return result;

} /* ds_qmap_send_cmd() */
 
/*===========================================================================
FUNCTION   DS_QMAP_CMD_RESP_CB()

DESCRIPTION
  This callback function is called when a response for a
   QMAP command is received on the UL

PARAMETERS:
    stream_id :  Stream ID on which response was received
    cmd_name  :  Command name as indicated in the QMAP control command spec
    cmd_type  :  Command type as indicated in the QMAP control command spec
    tx_id     :  QMAP command Transaction ID
    dsm_ptr   :  Pointer to the DSM item which contains the command payload. 
                 The QMAP control header is stripped out of the dsm item and passed
                 to DS/client. DS is expected to free this item

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_qmap_cmd_resp_cb
(
  sio_stream_id_type              stream_id,
  uint8                           cmd_name,   
  uint8                           cmd_type,
  uint32                          tx_id,
  dsm_item_type                 * dsm_ptr
)
{
  ds_qmap_cmd_buf_type         * cmd_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (ds_qmap_cmd_buf_type *)
                    ds_qmap_get_cmd_buf(QMI_CMD_QMAP_CMD_RESP_CB);
  if (cmd_ptr != NULL)
  {
    cmd_ptr->cmd_id = QMI_CMD_QMAP_CMD_RESP_CB;
    cmd_ptr->qmap.qmap_resp.stream_id  = stream_id;
    cmd_ptr->qmap.qmap_resp.cmd_name   = cmd_name;
    cmd_ptr->qmap.qmap_resp.cmd_type   = cmd_type;
    cmd_ptr->qmap.qmap_resp.tx_id      = tx_id;
    cmd_ptr->qmap.qmap_resp.dsm_ptr    = dsm_ptr;
    
    dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  }
  else
  {
    LOG_MSG_ERROR_0("Out of cmd buf");
  }

} /* ds_qmap_cmd_resp_cb() */

/*===========================================================================
  FUNCTION DS_QMAP_BUILD_FLOW_ENABLE_CMD()

  DESCRIPTION
    This function builds a QMAP flow enable command.

  PARAMETERS
    qmap_fc_cmd:   QMAP command ptr
    ip_family:     IP family
    seq_num:       sequence number
    qos_id:        qos_id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmap_build_flow_enable_cmd
(
  ds_qmap_cmd_flow_ctrl_type   * qmap_fc_cmd,
  uint8                          ip_family,
  uint16                         seq_num,
  uint32                         qos_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qmap_fc_cmd == NULL)
  {
    return;
  }

  ds_qmap_build_fc_cmd(
                   qmap_fc_cmd,
                   DS_QMAP_CMD_FLOW_ENABLE,
                   ip_family,
                   seq_num,
                   qos_id);

} /* ds_qmap_build_flow_enable_cmd() */

/*===========================================================================
  FUNCTION DS_QMAP_BUILD_FLOW_DISABLE_CMD()

  DESCRIPTION
    This function builds a QMAP flow disable command.

  PARAMETERS
    qmap_fc_cmd:   QMAP command ptr
    ip_family:     IP family
    seq_num:       sequence number
    qos_id:        qos_id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_qmap_build_flow_disable_cmd
(
  ds_qmap_cmd_flow_ctrl_type   * qmap_fc_cmd,
  uint8                          ip_family,
  uint16                         seq_num,
  uint32                         qos_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qmap_fc_cmd == NULL)
  {
    return;
  }

  ds_qmap_build_fc_cmd(
                   qmap_fc_cmd,
                   DS_QMAP_CMD_FLOW_DISABLE,
                   ip_family,
                   seq_num,
                   qos_id);

} /* ds_qmap_build_flow_disable_cmd() */

/*===========================================================================
FUNCTION DS_QMAP_CLEAN_CMD

DESCRIPTION
  Clean all the pending commands for a specific call

PARAMETERS
  xport_info: call xport_info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_qmap_clean_cmd
(
  rmnet_xport_type * xport_info
)
{
  ds_qmap_cmd_info_type  * item_ptr;
  uint8                  * cmd_ptr;
  int                      q_count;
  uint8                    cmd;
  uint8                    ip;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Clean all requests for the call
  q_count = q_cnt(&qmap_cmd_pend_q);
  while (q_count--)
  {
    item_ptr = (ds_qmap_cmd_info_type *) q_get(&qmap_cmd_pend_q);
    if (item_ptr != NULL)
    {
      if (item_ptr->xport_info != xport_info)
      {
        // Put back into the queue
        q_link(item_ptr, &(item_ptr->link));
        q_put(&qmap_cmd_pend_q, &(item_ptr->link));
      }
      else
      {
        cmd_ptr = item_ptr->cmd_ptr;
        cmd = *(cmd_ptr + DS_QMAP_CMD_OFFSET);
        ip = *(cmd_ptr + DS_QMAP_CMD_PAYLOAD_OFFSET + 1);

        // Old pending cmd; Free the memory
        (void) timer_clr( &(item_ptr->qmap_cmd_timer), T_NONE );
	 timer_undef( &(item_ptr->qmap_cmd_timer) );
        PS_SYSTEM_HEAP_MEM_FREE(item_ptr->cmd_ptr);
        PS_SYSTEM_HEAP_MEM_FREE(item_ptr);
        LOG_MSG_INFO2_2("QMAP FC cmd [%d] IP [%d] removed", cmd, ip);
      }
    }
  }

} /* ds_qmap_clean_cmd() */

/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
  FUNCTION DS_QMAP_BUILD_FC_CMD()

  DESCRIPTION
    This function builds a QMAP flow control command (without cmd type)

  PARAMETERS
    qmap_fc_cmd:   QMAP command ptr
    ip_family:     IP family
    seq_num:       sequence number
    qos_id:        qos_id 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmap_build_fc_cmd
(
  ds_qmap_cmd_flow_ctrl_type   * qmap_fc_cmd,
  uint8                          cmd,
  uint8                          ip_family,
  uint16                         seq_num,
  uint32                         qos_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qmap_fc_cmd == NULL)
  {
    ASSERT(0);
    return;
  }

  memset(qmap_fc_cmd, 0, sizeof(ds_qmap_cmd_flow_ctrl_type));

  qmap_fc_cmd->cmd_hdr.cmd      = cmd;
  qmap_fc_cmd->cmd_hdr.cmd_type = DS_QMAP_CMD_TYPE_REQUEST;

  if (ip_family == IPV4_ADDR) 
  {
    qmap_fc_cmd->ip_family  = DS_QMAP_ADDR_FAMILY_V4;
  }
  else if (ip_family == IPV6_ADDR)
  {
    qmap_fc_cmd->ip_family  = DS_QMAP_ADDR_FAMILY_V6;
  }
  else
  {
    qmap_fc_cmd->ip_family  = DS_QMAP_ADDR_FAMILY_ANY ;
  }

  qmap_fc_cmd->seq_num      = ps_htons(seq_num);
  qmap_fc_cmd->qos_id       = ps_htonl(qos_id);

} /* ds_qmap_build_fc_cmd() */

/*===========================================================================
  FUNCTION DS_QMAP_PROCESS_CMD

  DESCRIPTION
    This function processes a QMAP command or event.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMAP command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmap_process_cmd
(
  void * cmd_ptr
)
{
  ds_qmap_cmd_buf_type * cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  if (cmd_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL QMAP cmd_ptr!");
    return;
  }

  cmd_buf_ptr = (ds_qmap_cmd_buf_type *)cmd_ptr;

  switch (cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_QMAP_CMD_RESP_CB:
      ds_qmap_process_qmap_resp (cmd_buf_ptr->qmap.qmap_resp.stream_id,
                                 cmd_buf_ptr->qmap.qmap_resp.cmd_name,
                                 cmd_buf_ptr->qmap.qmap_resp.cmd_type,
                                 cmd_buf_ptr->qmap.qmap_resp.tx_id,
                                 cmd_buf_ptr->qmap.qmap_resp.dsm_ptr);
      break;

    case QMI_CMD_QMAP_CMD_TIMER_EXP_CB:
      ds_qmap_process_timer_exp (
                  cmd_buf_ptr->qmap.timer_info.search_id);
      break;

    default:
      LOG_MSG_ERROR_1("Ignoring QMAP cmd %d", cmd_buf_ptr->cmd_id);
      break;
  }

  ds_qmap_free_cmd_buf(cmd_ptr);
  return;

} /* qmi_qmux_process_cmd() */

/*===========================================================================
  FUNCTION DS_QMAP_GET_CMD_BUF()

  DESCRIPTION
    Allocate and assign a QMAP command buffer from the PS MEM heap

  PARAMETERS
    cmd_id - QMI command type

  RETURN VALUE
    cmd_buf_ptr - Pointer to the allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void * ds_qmap_get_cmd_buf
(
  qmi_cmd_id_e_type cmd_id
)
{
  void * cmd_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_ptr, sizeof(ds_qmap_cmd_buf_type),
                           void*);
  return cmd_buf_ptr;

} /* ds_qmap_get_cmd_buf() */

/*===========================================================================
  FUNCTION DS_QMAP_FLOW_CMD_TIMER_CBACK()

  DESCRIPTION
    QMAP Flow command timer is invoked when the QMAP cmd response is
    not received within the specified timeout(8s)

  PARAMETERS
    user_data:  Contains the id of the pending QMAP cmd

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmap_cmd_timer_cback
(
  timer_cb_data_type  user_data
)
{ 
  uint32                    search_id;
  ds_qmap_cmd_buf_type   *  cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  search_id = (uint32) user_data;

  cmd_ptr = (ds_qmap_cmd_buf_type *)
                    ds_qmap_get_cmd_buf(QMI_CMD_QMAP_CMD_TIMER_EXP_CB);
  if (cmd_ptr != NULL)
  {
    cmd_ptr->cmd_id = QMI_CMD_QMAP_CMD_TIMER_EXP_CB;
    cmd_ptr->qmap.timer_info.search_id = search_id;
    dcc_send_cmd_ex(DCC_QMI_CMD, cmd_ptr);
  }
  else
  {
    LOG_MSG_ERROR_0("Out of cmd buf");
    return;
  }

} /* ds_qmap_cmd_timer_cback() */

/*===========================================================================
FUNCTION DS_QMAP_COMPARE_PENDING_RESP

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
    the queue. This function checks whether the qmap response received
    is in the pending queue

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static int ds_qmap_compare_pending_resp
(
  ds_qmap_cmd_info_type        * cmd_info_ptr,    // from queue
  ds_qmap_cmd_resp_type        * cmd_resp_ptr     // resp info
)
{
  uint8          * cmd_ptr = NULL;
  int              result = FALSE;

  uint8            cmd; 
  uint8            ip_family;
  uint16           seq_no;
  uint32           qos_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_info_ptr->tx_id == cmd_resp_ptr->tx_id) &&
       (cmd_info_ptr->stream_id == cmd_resp_ptr->stream_id) )
  {
    cmd_ptr = cmd_info_ptr->cmd_ptr;

    // Compare cmd name
    cmd = *(cmd_ptr + DS_QMAP_CMD_OFFSET);
    if (cmd != cmd_resp_ptr->cmd_name)
    {
      return FALSE;
    }

    if (DS_QMAP_IS_FC_CMD(cmd))
    {
      ip_family = *(uint8 *) (cmd_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 1));
      seq_no    = *(uint16 *)(cmd_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 2));
      qos_id    = *(uint32 *)(cmd_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 4));

      // Compare payload
      if ( (ip_family == cmd_resp_ptr->payload.fc.ip_family) &&
           (seq_no == cmd_resp_ptr->payload.fc.seq_num) &&
           (qos_id == cmd_resp_ptr->payload.fc.qos_id) )
      {
         result = TRUE;
      }
    }
  }

  return result;

} /* ds_qmap_compare_pending_resp() */

/*===========================================================================
FUNCTION DS_QMAP_COMPARE_CMD_ID

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
    the queue. This function check if the passed qmap_id matched the
    queue item.
 

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static int ds_qmap_compare_cmd_id
(
  ds_qmap_cmd_info_type   *  cmd_info_ptr,  // from queue
  int                     *  qmap_id        // cmd id for timer cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (*qmap_id == cmd_info_ptr->qmap_cmd_id) ? TRUE : FALSE;  

} /* ds_qmap_compare_cmd_id() */

/*===========================================================================
FUNCTION DS_QMAP_IS_CMD_EXPIRED

DESCRIPTION
  Queue Compare Function
    Used by the queue searching functions to determine if an item is in
       the queue. This function checks if the passed qmap cmd is
       expired

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if the element should be operated upon, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_qmap_is_cmd_expired
(
  ds_qmap_cmd_info_type  * current_info_ptr,
  ds_qmap_cmd_info_type  * pending_info_ptr
)
{
  uint8    * cmd1_ptr;
  uint8    * cmd2_ptr;
  uint8      cmd1, cmd2;
  uint8      ip1, ip2;
  uint32     qos1, qos2;
  boolean    result = FALSE; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (current_info_ptr->stream_id == pending_info_ptr->stream_id)
  {
    cmd1_ptr = current_info_ptr->cmd_ptr;
    cmd2_ptr = pending_info_ptr->cmd_ptr;

    cmd1   = *(cmd1_ptr + DS_QMAP_CMD_OFFSET);
    cmd2   = *(cmd2_ptr + DS_QMAP_CMD_OFFSET);

    if ( DS_QMAP_IS_FC_CMD(cmd1) && DS_QMAP_IS_FC_CMD(cmd2) )
    {
      ip1   = *(cmd1_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 1));
      ip2   = *(cmd2_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 1));

      qos1  = *(uint32 *)(cmd1_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 4));
      qos2  = *(uint32 *)(cmd2_ptr + (DS_QMAP_CMD_PAYLOAD_OFFSET + 4));

      if ((ip1 == ip2) && (qos1 == qos2))
      {
        result = TRUE;
      }
    }
  }

  return result;

} /* ds_qmap_is_cmd_expired() */

/*===========================================================================
  FUNCTION DS_QMAP_PROCESS_QMAP_RESP()

  DESCRIPTION
    Called when a qmap command response is received
        
  PARAMETERS
    resp_info : ptr to the qmap resp info
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmap_process_qmap_resp
(
  sio_stream_id_type          stream_id,
  uint8                       cmd_name,   
  uint8                       cmd_type,
  uint32                      tx_id,
  dsm_item_type             * dsm_ptr
)
{
  ds_qmap_cmd_info_type        * cmd_info_ptr = NULL;
  ds_qmap_cmd_resp_type          qmap_cmd_resp;
  void                         * value;
  uint16                         pkt_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&qmap_cmd_resp, 0, sizeof(ds_qmap_cmd_resp_type));

  qmap_cmd_resp.stream_id  = stream_id;
  qmap_cmd_resp.cmd_name   = cmd_name;
  qmap_cmd_resp.cmd_type   = cmd_type;
  qmap_cmd_resp.tx_id      = tx_id;

  pkt_size = (uint16)dsm_length_packet(dsm_ptr);
  if (pkt_size == 0)
  {
    LOG_MSG_ERROR_0( "Invalid DSM ptr " );
    dsm_free_packet(&dsm_ptr);
    return;
  }

  if (DS_QMAP_IS_FC_CMD(cmd_name))
  {
      if ( pkt_size == DS_QMAP_FC_CMD_PAYLOAD_SIZE &&
           pkt_size <= sizeof(qmap_cmd_resp.payload.fc) )
      {
        value = &(qmap_cmd_resp.payload.fc);
      } 
      else
      {
        LOG_MSG_ERROR_1( "Invalid QMAP FC payload length %d", pkt_size );
        goto send_result;
      }
  }
  else
  {
    LOG_MSG_INFO3_1( "Ingoring CB for non FC cmds %d ", cmd_name );
    goto send_result;
  }

  if (pkt_size != dsm_pullup( &dsm_ptr, value, pkt_size ))
  {
    goto send_result;
  }

  LOG_MSG_INFO1_6("Recvd QMAP resp cmd [%d] IP [%d] seq [%d] "
                  "qos [0x%x] strm [0x%x] tx_id [0x%x]",
                     qmap_cmd_resp.cmd_name,
                     qmap_cmd_resp.payload.fc.ip_family,
                     ps_ntohs(qmap_cmd_resp.payload.fc.seq_num),
                     ps_ntohl(qmap_cmd_resp.payload.fc.qos_id),
                     qmap_cmd_resp.stream_id,
                     qmap_cmd_resp.tx_id);

  // Delete QMAP response from pending queue
  cmd_info_ptr = q_linear_delete_new(
                     &qmap_cmd_pend_q,
                     (q_compare_func_type) ds_qmap_compare_pending_resp,
                     &qmap_cmd_resp,
                     (q_action_func_type) NULL,
                     NULL);

  if (cmd_info_ptr == NULL)
  {
    LOG_MSG_INFO2_0("Invalid/Old resp recvd: Ignoring!");
  }
  else
  {
    // Stop corresponding timer
    (void) timer_clr( &(cmd_info_ptr->qmap_cmd_timer), T_NONE );
    timer_undef( &(cmd_info_ptr->qmap_cmd_timer) );
    PS_SYSTEM_HEAP_MEM_FREE(cmd_info_ptr->cmd_ptr);
    PS_SYSTEM_HEAP_MEM_FREE(cmd_info_ptr);
  }

send_result:
  dsm_free_packet(&dsm_ptr);

} /* ds_qmap_process_qmap_resp() */

/*===========================================================================
  FUNCTION DS_QMAP_PROCESS_TIMER_EXP()

  DESCRIPTION
    Called when a qmap response timer cb occurs
        
  PARAMETERS
    search_id :  ID of the expired qmap cmd
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ds_qmap_process_timer_exp
(
  uint32 search_id
)
{
  ds_qmap_cmd_info_type   * cmd_info_ptr = NULL;
  boolean                   result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1( "Timer exp for QMAP resp id %d", search_id);
  
  // Search resp in pending queue and remove; Clear timer
  cmd_info_ptr = q_linear_delete_new(
                   &qmap_cmd_pend_q,
                   (q_compare_func_type) ds_qmap_compare_cmd_id,
                   &search_id,
                   (q_action_func_type) NULL,
                   NULL);

  if (cmd_info_ptr == NULL)
  {
    LOG_MSG_INFO3_0("Old timer: Ignoring!" );
  }
  else
  {
    // Stop corresponding timer 
    (void) timer_clr( &(cmd_info_ptr->qmap_cmd_timer), T_NONE );

    if (cmd_info_ptr->num_retx < DS_QMAP_CMD_MAX_RETRY)
    {
      cmd_info_ptr->num_retx++;
    }
    else
    {
      // Drop the packet; Free the memory 
      LOG_MSG_INFO2_0( "Max retries reached. No more retx !" );
      // Stop the timer
      timer_undef( &(cmd_info_ptr->qmap_cmd_timer) );
      PS_SYSTEM_HEAP_MEM_FREE(cmd_info_ptr->cmd_ptr);
      PS_SYSTEM_HEAP_MEM_FREE(cmd_info_ptr);
      return;
    }

    // Resend QMAP cmd with retx flag set to TRUE
    result = ds_qmap_send_cmd(
                  cmd_info_ptr->xport_info,
                  cmd_info_ptr->cmd_size,
                  cmd_info_ptr->cmd_ptr,
                  cmd_info_ptr);

    if (!result)
    {
      LOG_MSG_ERROR_0("Unable to resend QMAP pkt ");
    }
  }

} /* ds_qmap_process_timer_exp() */

