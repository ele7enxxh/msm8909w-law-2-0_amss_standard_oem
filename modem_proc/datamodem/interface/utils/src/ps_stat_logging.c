/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       P S _ S T A T _ L O G G I N G . C


GENERAL DESCRIPTION
  This file implements an API to log PS stats. Currently IP stats are logged

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_stat_logging_init() must be called

Copyright (c) 2005-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_stat_logging.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
11/19/08    pp     Lint fixes.
10/24/08    pp     Fixed compiler warning observed in OFFTARGET build.
07/04/08    rr     Added support for logging instance statistics
12/05/05    msr    Created file
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"



#include <stringl/stringl.h>
#include "diagcmd.h"
#include "diagpkt.h"

#include "err.h"
#include "amssassert.h"
#include "log.h"
#include "log_codes.h"
#include "ps_svc.h"
#include "ps_stat_logging.h"

#include "ps_mem.h"
#include "ps_byte.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                        FORWARD DECLARATIONS

===========================================================================*/
static PACKED void *ps_stat_loggingi_get_inst_desc_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
);

static PACKED void *ps_stat_loggingi_get_inst_stat_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
);

static PACKED void *ps_stat_loggingi_reset_inst_stat_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
);

static ps_stat_logging_stat_cmd_type *ps_stat_loggingi_get_stat_list_from_sscmd
(
  uint8        * req_pkt,
  uint16         pkt_len
);

static void ps_stat_loggingi_process_get_inst_desc_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
);

static void ps_stat_loggingi_process_get_inst_stat_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
);

static void ps_stat_loggingi_process_reset_inst_stat_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
);
/*===========================================================================

                                CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added to each DIAG request/response
---------------------------------------------------------------------------*/
#define DIAG_HDR_LEN  (sizeof(diagpkt_subsys_header_type))

/*---------------------------------------------------------------------------
  Size of the payload of the DIAG request for descriptions
  2 byte to hold the type of description being requested
---------------------------------------------------------------------------*/
#define DIAG_PAYLOAD_DESC_LEN 8

/*---------------------------------------------------------------------------
  Size of the payload of the DIAG request for statistics
  1 byte to hold the type of instance being queried
  4 bytes to hold the handle to the instance being queried
---------------------------------------------------------------------------*/
#define DIAG_PAYLOAD_STAT_LEN 5

/*---------------------------------------------------------------------------
  Offset in to DIAG header where sub system command code (SSCC) is found
---------------------------------------------------------------------------*/
#define DIAG_HDR_SSCC_OFFSET  2

/*---------------------------------------------------------------------------
  Processor ID of this AMSS instance
---------------------------------------------------------------------------*/
#define PS_PROC_ID 0x1

/*---------------------------------------------------------------------------
  Processor ID offset for use in the subsystem command
---------------------------------------------------------------------------*/
#define PS_PROC_ID_OFFSET (PS_PROC_ID << 12)

/*---------------------------------------------------------------------------
  The sub-system id's specific commands we handle.  There is a
  higher-level command defined in DIAG for DIAG_SUBSYS_PS_DATA_LOGGING
  See ps_logging_diag_ssid_cmd_type for other PS SSCCs
  SSCC 1-5 are used by DPL, hence stats SSCCs start at 6
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Subsystem command code requesting instance descriptions
---------------------------------------------------------------------------*/
#define PS_STAT_INST_GET_DESC_SSCC (PS_PROC_ID_OFFSET | 0x06)

/*---------------------------------------------------------------------------
  Subsystem command code requesting instance statistics
---------------------------------------------------------------------------*/
#define PS_STAT_INST_GET_STAT_SSCC (PS_PROC_ID_OFFSET | 0x07)

/*---------------------------------------------------------------------------
  Subsystem command code requesting that instance statistics be reset
---------------------------------------------------------------------------*/
#define PS_STAT_INST_RESET_STAT_SSCC (PS_PROC_ID_OFFSET | 0x08)


/*===========================================================================

                                VARIABLES

===========================================================================*/
/*---------------------------------------------------------------------------
  Sequence number for instance statistics packets
---------------------------------------------------------------------------*/
//static uint32 global_stat_log_pkt_seq_num = 0;

/*---------------------------------------------------------------------------
 diag command format:

  { 75 = SSIDDISPATCH, XX = SSID = 42 for DPL, YY = SSID_CMD, 0x00 }

   diag handler structure format:
   [ { handler }, { handler }, ..., { last handler } ]

   where { handler } is

   { lower ssid_cmd value limit,
     upper ssid_cmd value limit,
     ssid-specific command handler }
---------------------------------------------------------------------------*/
static const diagpkt_user_table_entry_type ps_stat_logging_inst_stats_tbl[] =
{
  {
   (PS_STAT_INST_GET_DESC_SSCC),
   (PS_STAT_INST_GET_DESC_SSCC),
   ps_stat_loggingi_get_inst_desc_req
  },
  {
   (PS_STAT_INST_GET_STAT_SSCC),
   (PS_STAT_INST_GET_STAT_SSCC),
   ps_stat_loggingi_get_inst_stat_req
  },
  {
   (PS_STAT_INST_RESET_STAT_SSCC),
   (PS_STAT_INST_RESET_STAT_SSCC),
   ps_stat_loggingi_reset_inst_stat_req
  }
};

/*---------------------------------------------------------------------------
  GET_DESC_LOG_PKT_CBACK_ARR -
  Array of function pointers to all of the the callback functions to dispatch
  log packets with the instance descriptions that have registered with the
  stats module.  The semantics of this array are that empty slots have a
  value of NULL, and any other value is the function to call.  The array is
  indexed using the instance types.
---------------------------------------------------------------------------*/
static ps_stat_inst_fill_desc_f_ptr_type
  get_desc_log_pkt_cback_arr[(uint32)PS_STAT_MODULE_MAX + 1] = {NULL};

/*---------------------------------------------------------------------------
  GET_STAT_LOG_PKT_CBACK_ARR -
  Array of function pointers to all of the the callback functions to dispatch
  log packets with the instance statistics that have registered with the
  stats module.  The semantics of this array are that empty slots have a
  value of NULL, and any other value is the function to call.  The array is
  indexed using the instance types.
---------------------------------------------------------------------------*/
static ps_stat_inst_get_stat_f_ptr_type
  get_stat_log_pkt_cback_arr[(uint32)PS_STAT_MODULE_MAX + 1] = {NULL};

/*---------------------------------------------------------------------------
  RESET_STAT_LOG_PKT_CBACK_ARR -
  Array of function pointers to all of the the callback functions to dispatch
  log packets with the instance descriptions that have registered with the
  stats module (if they have been queried before). It also sets the stats
  struct of the specified instance to 0. The semantics of this array are that
  empty slots have a value of NULL, and any other value is the function to
  call.  The array is indexed using the instance types.
---------------------------------------------------------------------------*/
static ps_stat_inst_reset_stat_f_ptr_type
  reset_stat_cback_arr[(uint32)PS_STAT_MODULE_MAX + 1] = {NULL};


/*===========================================================================

                            PS_MEM DECLARATIONS

===========================================================================*/
#define PS_STAT_LOGGING_STAT_CMD_BUF_SIZE \
  ((sizeof(ps_stat_logging_stat_cmd_type) + 3) & ~3)
#define PS_STAT_LOGGING_STAT_CMD_BUF_NUM     10
#define PS_STAT_LOGGING_STAT_CMD_BUF_HIGH_WM 9
#define PS_STAT_LOGGING_STAT_CMD_BUF_LOW_WM  2

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_stat_logging_stat_cmd_type along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_stat_logging_stat_cmd_buf_mem
  [
    PS_MEM_GET_TOT_SIZE_OPT
    (
      PS_STAT_LOGGING_STAT_CMD_BUF_NUM,
      PS_STAT_LOGGING_STAT_CMD_BUF_SIZE
    )
  ] = {0};

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_stat_logging_stat_cmd_type
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type
  * ps_stat_logging_stat_cmd_buf_hdr[PS_STAT_LOGGING_STAT_CMD_BUF_SIZE];
static ps_stat_logging_stat_cmd_type
  * ps_stat_logging_stat_cmd_buf_ptr[PS_STAT_LOGGING_STAT_CMD_BUF_SIZE];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


#define PS_STAT_LOGGING_DESC_CMD_BUF_SIZE \
  ((sizeof(ps_stat_logging_desc_cmd_type) + 3) & ~3)
#define PS_STAT_LOGGING_DESC_CMD_BUF_NUM     2
#define PS_STAT_LOGGING_DESC_CMD_BUF_HIGH_WM 2
#define PS_STAT_LOGGING_DESC_CMD_BUF_LOW_WM  1

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_stat_logging_desc_cmd_type along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_stat_logging_desc_cmd_buf_mem
  [
    PS_MEM_GET_TOT_SIZE_OPT
    (
      PS_STAT_LOGGING_DESC_CMD_BUF_NUM,
      PS_STAT_LOGGING_DESC_CMD_BUF_SIZE
    )
  ] = {0};

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_stat_logging_desc_cmd_type
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type
  * ps_stat_logging_desc_cmd_buf_hdr[PS_STAT_LOGGING_DESC_CMD_BUF_SIZE];
static ps_stat_logging_desc_cmd_type
  * ps_stat_logging_desc_cmd_buf_ptr[PS_STAT_LOGGING_DESC_CMD_BUF_SIZE];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_GET_INST_DESC_REQ()

DESCRIPTION
  This function is the registered subsystem command handler for obtain
  instance description subsystem commands.  It validates the incoming
  command, posts a command to the protocol stack, and dispatches a subsystem
  response packet.

  This runs in the DIAG context.

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command packet.

RETURN VALUE
  None

DEPENDENCIES
  This function must have been registered in the diag table in order to be
  invoked when the appropriate subsystem command is recognized.

SIDE EFFECTS
  Dispatches a subsystem command response packet.
===========================================================================*/
static PACKED void *ps_stat_loggingi_get_inst_desc_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
)
{
  PACKED void                    * return_ptr;      /* Response message */
  uint8                          * request_ptr;     /* Read req pkt */
  ps_stat_logging_desc_cmd_type  * desc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_stat_loggingi_get_inst_desc_req(): "
                  "QXDM called GET_INST_DESC");

#ifdef FEATURE_DATA_PS_LOGGING_DEBUG
  memdump ((void*) req_pkt, pkt_len);
#endif

  /*-------------------------------------------------------------------------
    Validate the request pkt's length
  -------------------------------------------------------------------------*/
  if (NULL == req_pkt || (DIAG_HDR_LEN + DIAG_PAYLOAD_DESC_LEN) != pkt_len)
  {
    LOG_MSG_INFO1_0("ps_stat_loggingi_get_inst_desc_req(): "
                    "Invalid pkt or pkt length");
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Validate request pkt's SSCC
    The upper 4 bits of the subsystem command code are the processor ID, and
    the lower 12 bits of the subsystem command code is the command ID itself
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_SSCC_OFFSET;

  if (PS_STAT_INST_GET_DESC_SSCC != hget16(request_ptr))
  {
    LOG_MSG_INFO1_1("ps_stat_loggingi_get_inst_desc_req(): "
                    "Invalid command code %d.", hget16(request_ptr));

    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Skip Diag header of request pkt
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_LEN;

  desc_ptr = (ps_stat_logging_desc_cmd_type *)
               ps_mem_get_buf(PS_MEM_LOG_DESC_CMD_TYPE);
  if (NULL == desc_ptr)
  {
    //TODO: Any other cleanup reqd??
    return NULL;
  }

  *desc_ptr = hget64(request_ptr);

  /*-------------------------------------------------------------------------
    Send the request on to the protocol stack to obtain the descriptions
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_STAT_INST_GET_DESC_CMD, (void *) desc_ptr);

  /*-------------------------------------------------------------------------
    Response packet has no payload, so its size is just DIAG_HDR_LEN
  -------------------------------------------------------------------------*/
  return_ptr = diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_PS,
                                    PS_STAT_INST_GET_DESC_SSCC,
                                    DIAG_HDR_LEN);

  ASSERT(NULL != return_ptr);

  return(return_ptr);

} /* ps_stat_loggingi_get_inst_desc_req */


/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_GET_STAT_LIST_FROM_SSCMD()

DESCRIPTION
  This function implements the functionality of converting a subsystem command
  with multiple type/handle pairs into a linked list of these type/handle
  pairs for passing from the diag context to the PS context.

  This runs in the DIAG context.

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command payload

RETURN VALUE
  None

DEPENDENCIES
  PS_MEM must have been allocated before calling

SIDE EFFECTS
  None
===========================================================================*/
ps_stat_logging_stat_cmd_type *ps_stat_loggingi_get_stat_list_from_sscmd
(
  uint8        * req_pkt,
  uint16         pkt_len
)
{
  uint8                         * request_ptr;   /* Read from req pkt       */
  uint8                           num_instances; /* Number of instances     */
  uint8                           idx;

  ps_stat_logging_stat_cmd_type * inst_info_ptr; /* Instance handle & type  */
  ps_stat_logging_stat_cmd_type * inst_list_head_ptr = NULL;
  ps_stat_logging_stat_cmd_type * inst_list_prev_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  request_ptr = req_pkt;

  num_instances = get8(request_ptr);

  /*-------------------------------------------------------------------------
    One byte for the number of instances.  Each type/handle pair is 5 bytes
    (1 byte for type, 4 for handle)
  -------------------------------------------------------------------------*/
  if (pkt_len != (DIAG_HDR_LEN + 1 + (num_instances * 5)))
  {
    LOG_MSG_ERROR_0("ps_stat_loggingi_get_stat_list_from_sscmd(): "
                    "Invalid cmd pkt size");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Skip the number of instances
  -------------------------------------------------------------------------*/
  request_ptr++;

  /*-------------------------------------------------------------------------
    It was decided to implement our own linked list here (as opposed to using
    existing linked list code) because of the limited number of operations
    we need to perform on this list and the additional overhead that the
    existing linked list code would have
    All of the requests are grouped into one PS command in order to minimize
    the number of commands that stats has to send to the PS.  There is a
    limited number of PS command buffers, and it would be bad if stats ended
    up using them all.
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < num_instances; ++idx)
  {
    inst_info_ptr = (ps_stat_logging_stat_cmd_type *)
                     ps_mem_get_buf(PS_MEM_LOG_STAT_CMD_TYPE);
    if (NULL == inst_info_ptr)
    {
      //TODO: Do cleanup of already allocated inst_info_ptrs here??
      return NULL;
    }

    // TODO: change previous to current
    /* add the type/handle pair to the linked list */
    if (NULL == inst_list_head_ptr)
    {
      inst_list_head_ptr = inst_info_ptr;
    }
    else
    {
      inst_list_prev_ptr->next_ptr = inst_info_ptr;
    }

    /*-----------------------------------------------------------------------
      The get() and hget() functions do not increment the pointer values, so
      we must do that ourselves here
    -----------------------------------------------------------------------*/
    inst_info_ptr->type = (ps_stat_module_enum_type) get8(request_ptr);
    request_ptr++;

    inst_info_ptr->handle = hget32(request_ptr);
    request_ptr += sizeof(int32);

    inst_info_ptr->next_ptr = NULL;
    inst_list_prev_ptr = inst_info_ptr;
  }

  return inst_list_head_ptr;
}


/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_GET_INST_STAT_REQ()

DESCRIPTION
  This function is the registered subsystem command handler for obtain
  instance statistics subsystem commands.  It validates the incoming
  command, posts a command to the protocol stack, and dispatches a subsystem
  response packet.

  This runs in the DIAG context.

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command packet.

RETURN VALUE
  None

DEPENDENCIES
  This function must have been registered in the diag table in order to be
  invoked when the appropriate subsystem command is recognized.

SIDE EFFECTS
  Dispatches a subsystem command response packet.
===========================================================================*/
static PACKED void *ps_stat_loggingi_get_inst_stat_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
)
{
  PACKED void                   * return_ptr;    /* Response message        */
  uint8                         * request_ptr;   /* Read from req pkt       */
  ps_stat_logging_stat_cmd_type * inst_list_head_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_stat_loggingi_get_inst_stat_req(): "
                  "QXDM called PS_STAT_LOGGINGI_GET_INST_STAT");

#ifdef FEATURE_DATA_PS_LOGGING_DEBUG
  memdump ((void*) req_pkt, pkt_len);
#endif

  /*-------------------------------------------------------------------------
    Validate the request pkt's length
  -------------------------------------------------------------------------*/
  if (NULL == req_pkt)
  {
    LOG_MSG_INFO1_0("ps_stat_loggingi_get_inst_stat_req(): "
                    "Invalid request was sent by QXDM");
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Validate request pkt's SSCC
    The upper 4 bits of the subsystem command code are the processor ID, and
    the lower 12 bits of the subsystem command code is the command ID itself
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_SSCC_OFFSET;

  if (PS_STAT_INST_GET_STAT_SSCC != hget16(request_ptr))
  {
    LOG_MSG_INFO1_1("ps_stat_loggingi_get_inst_stat_req(): "
                    "Invalid command code %d was sent by QXDM",
                    hget16(request_ptr));
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Skip Diag header of request pkt
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_LEN;

  inst_list_head_ptr = ps_stat_loggingi_get_stat_list_from_sscmd
    (
      request_ptr,
      pkt_len
    );

  /*-------------------------------------------------------------------------
    Send the request on to the protocol stack to obtain the descriptions
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_STAT_INST_GET_STAT_CMD, (void *) inst_list_head_ptr);

  /*-------------------------------------------------------------------------
    Response packet has no payload, so its size is just DIAG_HDR_LEN
  -------------------------------------------------------------------------*/
  return_ptr = diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_PS,
                                    PS_STAT_INST_GET_STAT_SSCC,
                                    DIAG_HDR_LEN);

  ASSERT(NULL != return_ptr);

  return(return_ptr);

} /* ps_stat_logging_get_inst_stat_req */



/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_RESET_INST_STAT_REQ()

DESCRIPTION

  This runs in the DIAG context.

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command packet.

RETURN VALUE
  None

DEPENDENCIES
  This function must have been registered in the diag table in order to be
  invoked when the appropriate subsystem command is recognized.

SIDE EFFECTS
  Dispatches a subsystem command response packet.
===========================================================================*/
static PACKED void *ps_stat_loggingi_reset_inst_stat_req
(
  PACKED void  * req_pkt,
  uint16         pkt_len
)
{
  PACKED void                 * return_ptr;   /* Response message          */
  uint8                       * request_ptr;  /* Read from req pkt         */
  ps_stat_logging_stat_cmd_type * inst_list_head_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_stat_loggingi_reset_inst_stat_req(): "
                  "QXDM called PS_STAT_LOGGINGI_RESET_INST_STAT");
#ifdef FEATURE_DATA_PS_LOGGING_DEBUG
  memdump ((void*) req_pkt, pkt_len);
#endif

  /*-------------------------------------------------------------------------
    Validate the request pkt's length
  -------------------------------------------------------------------------*/
  if (NULL == req_pkt)
  {
    LOG_MSG_INFO1_0("ps_stat_loggingi_reset_inst_stat_req(): "
                    "Invalid request was sent by QXDM");
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Validate request pkt's SSCC
    The upper 4 bits of the subsystem command code are the processor ID, and
    the lower 12 bits of the subsystem command code is the command ID itself
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_SSCC_OFFSET;

  if ( PS_STAT_INST_RESET_STAT_SSCC != hget16(request_ptr))
  {
    LOG_MSG_INFO1_0("ps_stat_loggingi_reset_inst_stat_req(): "
                    "Invalid request is sent by QXDM");
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Skip Diag header of request pkt
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *) req_pkt + DIAG_HDR_LEN;

  inst_list_head_ptr = ps_stat_loggingi_get_stat_list_from_sscmd
    (
      request_ptr,
      pkt_len
    );

   /*-------------------------------------------------------------------------
    Send the request on to the protocol stack to obtain the descriptions
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_STAT_INST_RESET_STAT_CMD, (void *) inst_list_head_ptr);

  /*-------------------------------------------------------------------------
    Response packet has no payload, so its size is just DIAG_HDR_LEN
  -------------------------------------------------------------------------*/
  return_ptr = diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_PS,
                                    PS_STAT_INST_RESET_STAT_SSCC,
                                    DIAG_HDR_LEN);

  ASSERT(NULL != return_ptr);

  return(return_ptr);

} /* ps_stat_loggingi_reset_inst_stat_req */


/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_PROCESS_GET_INST_DESC_REQ()

DESCRIPTION
  This function generates log packets containing descriptions of all of the
  instance types that are specified in the input user_info_ptr.

PARAMETERS
  cmd_name:       Protocol stack command that is being handled.
  *user_info_ptr: Specifies which description log packets are being requested.

RETURN VALUE
  0  Success
  -1 Error

DEPENDENCIES
  This function must be registered as a ps command handler to be invoked
  by the command sent by the subsystem command handler.

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_loggingi_process_get_inst_desc_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
)
{
  ps_stat_logging_desc_cmd_type  * bitmask_ptr = NULL;
  ps_stat_logging_desc_cmd_type    desc_type = 0;
  ps_stat_logging_desc_cmd_type    bitmask   = 0;
  uint32                           idx       = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bitmask_ptr = (ps_stat_logging_desc_cmd_type *) user_info_ptr;

  bitmask = *bitmask_ptr;

  LOG_MSG_INFO2_1("ps_stat_loggingi_process_get_inst_desc_req(): "
                  "PS cmd to get inst descs %d", bitmask);

  for (idx = 0; idx < (uint32)PS_STAT_MODULE_MAX; ++idx)
  {
    /*-----------------------------------------------------------------------
      Since each bit in the bitmask identifies a different description type,
      iterate through each description type.  The bit values in the bitmask
      line up with the instance types exactly.  If the bit is set, invoke
      the callback of that type.  i is equal to the instance type, since
      we go through the bitmask in a linear manner.
    -----------------------------------------------------------------------*/
    desc_type = (bitmask >> idx);

    if (desc_type & 0x00000001)
    {
      if (NULL != get_desc_log_pkt_cback_arr[idx])
      {
        get_desc_log_pkt_cback_arr[idx](PS_STAT_INST_DESC_ALL);
      }
      else
      {
        LOG_MSG_INFO1_1("ps_stat_loggingi_process_get_inst_desc_req(): "
                        "Callback not registered %d", idx);
      }
    }
  }

  /* Free the Buffer associated */
  PS_MEM_FREE(user_info_ptr);

  return;
} /* ps_stat_loggingi_process_get_inst_desc_req */


/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_PROCESS_GET_INST_STAT_REQ()

DESCRIPTION
  This function generates log packets containing the statistics associated
  with the instance type and handle that are specified in the
  input user_info_ptr.

PARAMETERS
  cmd_name:       Protocol stack command that is being handled.
  *user_info_ptr: Specifies which description log packets are being requested.

RETURN VALUE
  0  Success
  -1 Error

DEPENDENCIES
  This function must be registered as a ps command handler to be invoked
  by the command sent by the subsystem command handler.

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_loggingi_process_get_inst_stat_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
)
{
  ps_stat_logging_stat_cmd_type        * inst_cmd_info_ptr;
  ps_stat_logging_stat_cmd_type        * inst_cmd_info_next_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_stat_loggingi_process_get_inst_stat_req(): "
                  "Processing GET_INST_STAT");

  inst_cmd_info_ptr = (ps_stat_logging_stat_cmd_type *) user_info_ptr;

  /*-------------------------------------------------------------------------
   The list of instance type/handle pairs needs to be traversed, and for each
   entry in the list invoke the registered callback for that instance type.
   Once the statistics callback has returned, free the memory being used
   to hold the type/handle pair
  -------------------------------------------------------------------------*/
  while (NULL != inst_cmd_info_ptr)
  {
    if (NULL != get_stat_log_pkt_cback_arr[inst_cmd_info_ptr->type])
    {
      get_stat_log_pkt_cback_arr[inst_cmd_info_ptr->type]
        (inst_cmd_info_ptr->handle);
    }
    else
    {
      LOG_MSG_INFO1_1("ps_stat_loggingi_process_get_inst_stat_req(): "
                      "Callback not registered %d", inst_cmd_info_ptr->type);
    }

    inst_cmd_info_next_ptr = inst_cmd_info_ptr->next_ptr;

    /* Free the Buffer associated */
    PS_MEM_FREE(inst_cmd_info_ptr);

    inst_cmd_info_ptr = inst_cmd_info_next_ptr;
  }

  return;
} /* ps_stat_loggingi_process_get_inst_stat_req */


/*===========================================================================
FUNCTION PS_STAT_LOGGINGI_PROCESS_RESET_INST_STAT_REQ()

DESCRIPTION

PARAMETERS
  cmd_name:       Protocol stack command that is being handled.
  *user_info_ptr: Specifies which description log packets are being requested.

RETURN VALUE
  0  Success
  -1 Error

DEPENDENCIES
  This function must be registered as a ps command handler to be invoked
  by the command sent by the subsystem command handler.

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_loggingi_process_reset_inst_stat_req
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
)
{
  ps_stat_logging_stat_cmd_type                * inst_info_ptr;
  ps_stat_logging_stat_cmd_type                * inst_info_prev_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //TODO: collapse common code into an internal function
  //pass array as arg to function
  LOG_MSG_INFO2_0("ps_stat_loggingi_process_reset_inst_stat_req(): "
                  "Processing GET_INST_STAT");

  inst_info_ptr = (ps_stat_logging_stat_cmd_type *) user_info_ptr;

  while (NULL != inst_info_ptr)
  {
    if (NULL != reset_stat_cback_arr[inst_info_ptr->type])
    {
      reset_stat_cback_arr[inst_info_ptr->type](inst_info_ptr->handle);
    }
    else
    {
      LOG_MSG_INFO1_1("ps_stat_loggingi_process_reset_inst_stat_req(): "
                      "Callback not registered %d", inst_info_ptr->type);
    }

    inst_info_prev_ptr = inst_info_ptr;
    inst_info_ptr = inst_info_prev_ptr->next_ptr;

    /* Free the Buffer associated */
    PS_MEM_FREE(inst_info_prev_ptr);
  }

} /* ps_stat_loggingi_process_reset_inst_stat_req */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_STAT_LOGGING_INIT()

DESCRIPTION
  Initializes PS stats logging module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_logging_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ps_set_cmd_handler(PS_STAT_INST_GET_DESC_CMD,
                            ps_stat_loggingi_process_get_inst_desc_req);

  (void) ps_set_cmd_handler(PS_STAT_INST_GET_STAT_CMD,
                            ps_stat_loggingi_process_get_inst_stat_req);

  (void) ps_set_cmd_handler(PS_STAT_INST_RESET_STAT_CMD,
                            ps_stat_loggingi_process_reset_inst_stat_req);

  DIAGPKT_DISPATCH_TABLE_REGISTER((word)DIAG_SUBSYS_PS,
                                  ps_stat_logging_inst_stats_tbl);

  /*-------------------------------------------------------------------------
    This memory pool holds the instance statistic type/handle pairs
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_LOG_STAT_CMD_TYPE,
                           ps_stat_logging_stat_cmd_buf_mem,
                           PS_STAT_LOGGING_STAT_CMD_BUF_SIZE,
                           PS_STAT_LOGGING_STAT_CMD_BUF_NUM,
                           PS_STAT_LOGGING_STAT_CMD_BUF_HIGH_WM,
                           PS_STAT_LOGGING_STAT_CMD_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_stat_logging_stat_cmd_buf_hdr,
                           (int *) ps_stat_logging_stat_cmd_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                           ) == -1)
  {
    LOG_MSG_ERROR_0("ps_stat_logging_init(): "
                    "Can't init the stats logging module");
  }

  /*-------------------------------------------------------------------------
    This memory pool holds the bitmask passed when getting description types
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_LOG_DESC_CMD_TYPE,
                           ps_stat_logging_desc_cmd_buf_mem,
                           PS_STAT_LOGGING_DESC_CMD_BUF_SIZE,
                           PS_STAT_LOGGING_DESC_CMD_BUF_NUM,
                           PS_STAT_LOGGING_DESC_CMD_BUF_HIGH_WM,
                           PS_STAT_LOGGING_DESC_CMD_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_stat_logging_desc_cmd_buf_hdr,
                           (int *) ps_stat_logging_desc_cmd_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                           ) == -1)
  {
    LOG_MSG_ERROR_0("ps_stat_logging_init(): "
                    "Can't init the stats logging module");
  }


} /* ps_stat_logging_init() */


/*===========================================================================
FUNCTION PS_STAT_LOGGING_INST_REG_FILL_DESC_CBACK()

DESCRIPTION
  This function is used to register the technology-specific fill description
  functions with the stats module.

PARAMETERS
  type:      The type of instance that the specified function will handle.
  *func_ptr: A pointer to the function that is being registered.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_logging_inst_reg_fill_desc_cback
(
  ps_stat_module_enum_type            func_type,
  ps_stat_inst_fill_desc_f_ptr_type   func_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (func_type >= PS_STAT_MODULE_MAX)
  {
    LOG_MSG_INFO1_1("ps_stat_logging_inst_reg_fill_desc_cback(): "
                    "Register fill desc cback with invalid type: %d",
                    func_type);
    return;
  }

  if (NULL == func_ptr)
  {
    LOG_MSG_INFO1_0("ps_stat_logging_inst_reg_fill_desc_cback(): "
                    "Register fill desc cack with void function pointer");
    return;
  }

  get_desc_log_pkt_cback_arr[func_type] = func_ptr;
  return;

} /* ps_stat_logging_inst_reg_fill_desc_cback */

/*===========================================================================
FUNCTION PS_STAT_LOGGING_INST_REG_GET_STAT_CBACK()

DESCRIPTION
  This function is used to register the technology-specific fill description
  functions with the stats module.

PARAMETERS
  type:      The type of instance that the specified function will handle.
  *func_ptr: A pointer to the function that is being registered.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_logging_inst_reg_get_stat_cback
(
  ps_stat_module_enum_type           func_type,
  ps_stat_inst_get_stat_f_ptr_type   func_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((func_type >= PS_STAT_MODULE_MAX) || (NULL == func_ptr))
  {
    LOG_MSG_INFO1_1("ps_stat_logging_inst_reg_get_stat_cback(): "
                    "Registering get stat cback with invalid type or NULL function pointer: %d",
                    func_type);
    return;
  }

  get_stat_log_pkt_cback_arr[func_type] = func_ptr;
  return;

} /* ps_stat_logging_inst_reg_get_stat_cback */

/*===========================================================================
FUNCTION PS_STAT_LOGGING_INST_REG_RESET_STAT_CBACK()

DESCRIPTION
  This function is used to register the technology-specific reset stats
  functions with the stats module.

PARAMETERS
  type:      The type of instance that the specified function will handle.
  *func_ptr: A pointer to the function that is being registered.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_logging_inst_reg_reset_stat_cback
(
  ps_stat_module_enum_type             func_type,
  ps_stat_inst_reset_stat_f_ptr_type   func_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((func_type >= PS_STAT_MODULE_MAX) || (NULL == func_ptr))
  {
    LOG_MSG_INFO1_1("ps_stat_logging_inst_reg_reset_stat_cback(): "
                    "Registering reset stat cback with invalid type or NULL function pointer: %d",
                    func_type);
    return;
  }

  reset_stat_cback_arr[func_type] = func_ptr;
  return;

} /* ps_stat_logging_inst_reg_reset_stat_cback */


/*===========================================================================
FUNCTION PS_STAT_LOGGING_INST_UPDATE_DESC()

DESCRIPTION
  This function creates a delta log packet for the selected instance,
  fills in the description, and dispatches the log packet to QXDM.

  It is called by all of the instance creation, modification, and
  removal functions.

PARAMETERS
  type:   The type of instance that has changed.  Needed to select the proper
          callback function.
  handle: Handle to the instance that is being changed.
  action: What is changing about the instance?  Is it being added, updating
          a description parameter, or is it being removed?

RETURN VALUE
  None

DEPENDENCIES
  The callback functions to retrieve the instance length and description
  associated with the specified instance type must have already been
  registered with the statistics module.

SIDE EFFECTS
  A log packet is dispatched to the diagnostic channel.
===========================================================================*/
void ps_stat_logging_inst_update_desc
(
  ps_stat_module_enum_type       inst_type,
  ps_stat_inst_handle_type       inst_handle,
  ps_stat_inst_update_enum_type  inst_update
)
{
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr    = NULL;
  uint8                                      * log_pkt_ptr      = NULL;
  uint8                                      * log_pkt_fill_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_stat_logging_inst_update_desc(): "
                  "Updating inst desc. Type: %d. Handle: %d. Action: %d",
                  inst_type, inst_handle, inst_update);

  /*-------------------------------------------------------------------------
    Validate the input provided by the calling function
    Abort and return on error
  -------------------------------------------------------------------------*/
  if (inst_type >= PS_STAT_MODULE_MAX)
  {
    LOG_MSG_ERROR_0("ps_stat_logging_inst_update_desc(): "
                    "Invalid instance type during description update");
    return;
  }

  if (inst_update >= PS_STAT_INST_UPDATE_MAX)
  {
    LOG_MSG_ERROR_0("ps_stat_logging_inst_update_desc(): "
                    "Invalid instance update action");
    return;
  }

  if (NULL == get_desc_log_pkt_cback_arr[inst_type])
  {
    LOG_MSG_ERROR_0("ps_stat_logging_inst_update_desc(): "
                    "Update inst desc - fill inst desc cback null");
    return;
  }

  if (PS_STAT_INST_DELETED == inst_update)
  {
    ps_stat_logging_get_log_pkt
    (
      (sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type) +
        sizeof(ps_stat_inst_handle_type)),
      LOG_PS_STAT_DESC_DELTA_INST_C,
      &log_pkt_ptr
    );

    if (NULL == log_pkt_ptr)
    {
      return;
    }

    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type = (uint8)inst_type;
    delta_pkt_ptr->deleted = TRUE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    /*-----------------------------------------------------------------------
      The log packet that we dispatch needs to include the handle of the
      instance to be deleted.  We reuse the delta header log pkt type struct
      for the delta descriptions, so we don't include the handle field directly
      in the struct, it is just placed at the end of the code
    -----------------------------------------------------------------------*/
    log_pkt_fill_ptr =
      log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type);

    *log_pkt_fill_ptr = (uint8)inst_handle;

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type) +
       sizeof(ps_stat_inst_handle_type))
    );
  }
  else
  {
    get_desc_log_pkt_cback_arr[inst_type](inst_handle);
  }

  return;

} /* ps_stat_logging_inst_update_desc */


/*===========================================================================
FUNCTION PS_STAT_LOGGING_GET_LOG_PKT()

DESCRIPTION
  This function gets a log packet for the fill function.

PARAMETERS
  size: How much memory is required
  log_pkt_ptr: OUT pointer to memory allocated

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_logging_get_log_pkt
(
  int32      pkt_size,
  uint16     log_pkt_type,
  uint8   ** log_pkt_ptr
)
{
  ps_stat_logging_hdr_log_pkt_type           * log_hdr_ptr   = NULL;
  uint8                                      * tmp_log_ptr   = NULL;
  uint32                                       alloc_pkt_size = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Add the header size onto the packet to allocate
    Calling function should not need to worry about the size of this hdr
  -------------------------------------------------------------------------*/
  alloc_pkt_size =
        (uint32) pkt_size + sizeof(ps_stat_logging_hdr_log_pkt_type);

  tmp_log_ptr = (uint8 *) log_alloc(log_pkt_type, alloc_pkt_size);

  if (NULL == tmp_log_ptr)
  {
    *log_pkt_ptr = NULL;
    return;
  }

  log_hdr_ptr = (ps_stat_logging_hdr_log_pkt_type *) tmp_log_ptr;

  switch (log_pkt_type)
  {
    case LOG_PS_STAT_DESC_ALL_TCP_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_TCP_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_UDP_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_UDP_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_MEM_POOL_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_MEM_POOL_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_IFACE_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_IFACE_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_PPP_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_PPP_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_ARP_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_ARP_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_LLC_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_LLC_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_RSVP_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_RSVP_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_FLOW_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_FLOW_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_ALL_PHYS_LINK_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_ALL_PHYS_LINK_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_DESC_DELTA_INST_C:
      log_hdr_ptr->version = PS_STAT_DESC_DELTA_INST_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_TCP_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_TCP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_UDP_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_UDP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_PPP_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_PPP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_IFACE_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_IFACE_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_MEM_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_MEM_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_FLOW_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_FLOW_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_PHYS_LINK_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_PHYS_LINK_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_ARP_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_ARP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_LLC_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_LLC_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_IPHC_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_IPHC_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_ROHC_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_ROHC_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_RSVP_INST_C:
      log_hdr_ptr->version = PS_STAT_INST_RSVP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_TCP_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_TCP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_UDP_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_UDP_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_IPV4_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_IPV4_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_IPV6_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_IPV6_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_ICMPV4_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_ICMPV4_LOG_PKT_VER;
      break;

    case LOG_PS_STAT_GLOBAL_ICMPV6_C:
      log_hdr_ptr->version = PS_STAT_GLOBAL_ICMPV6_LOG_PKT_VER;
      break;

    default:
      DATA_ERR_FATAL("ps_stat_logging_get_log_pkt(): Invalid log type passed in to GET_LOG_PKT");
      break;
  }

  log_hdr_ptr->proc_id = PS_PROC_ID;
  memset(log_hdr_ptr->reserved, 0, 2);

  tmp_log_ptr = tmp_log_ptr + sizeof(ps_stat_logging_hdr_log_pkt_type);

  *log_pkt_ptr = tmp_log_ptr;

  return;

} /* ps_stat_logging_get_log_pk */


/*===========================================================================
FUNCTION PS_STAT_LOGGING_COMMIT_LOG_PKT()

DESCRIPTION
  This function commits a log packet from the fill function.

PARAMETERS
  log_pkt_ptr: pointer to memory allocated for log pkt
  log_pkt_shorten_len: number of bytes to shorten the log packet by.  If 0
                       then the pkt is not impacted
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  A log packet is dispatched
===========================================================================*/
void ps_stat_logging_commit_log_pkt
(
  uint8    * log_pkt_ptr,
  int32      log_pkt_payload_size
)
{
  uint8                                   * log_hdr_ptr;
  uint32                                    log_pkt_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  log_hdr_ptr = log_pkt_ptr - sizeof(ps_stat_logging_hdr_log_pkt_type);

  log_pkt_size = (uint32)
    log_pkt_payload_size + sizeof(ps_stat_logging_hdr_log_pkt_type);

  log_shorten(log_hdr_ptr, log_pkt_size);

  log_commit(log_hdr_ptr);

  return;
} /* ps_stat_logging_inst_commit_log_pkt */

