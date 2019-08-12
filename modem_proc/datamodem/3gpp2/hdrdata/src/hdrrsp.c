/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

                  R O U T E   S E L E C T I O N   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the implementation of the EV-DO Rev A Route
  Selection Protocol (see 3GPP2 C-S0063-0 v2.0).  In particular,
  referring to the state machine illustrated in Figure 2.4.4.1.2-1
  will be helpful in understanding this code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrrsp_startup() needs to be called once at powerup.

 Copyright (c) 2005-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrrsp.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/09/14   ds      Adding FEATURE_DATA_PS_ROHC.
08/11/14   vm      Gracefully handle when rlp_id_ptr->ver becomes invalid
11/18/11   vpk     HDR SU API cleanup
07/26/11   vpk     Move away from hdrbit header
05/12/11   op      Migrated to MSG 2.0 macros
10/18/10   ms      Precede all Assert(0)s with Debug messages.
03/08/10   sn      Fixed klocwork critical errors.
11/30/09   as      Fixed critical section init to happen only once during powerup
07/27/07   ac      Fixed the error, when attaching the ds1x_meta_info, the 
				   kind field of DSM item needs to be DS1X_META_INFO_KIND(2)
06/25/07   ac      Fixed the RSP error when TwoRoutesSupported=0 
12/21/06   spn     fixed compilation error.
07/12/06   ksu     EMPA: RSP part 2 code review comments
05/30/06   ksu     EMPA: RLP init code review comments
05/04/06   ksu     EMPA: RSP part 2
03/27/06   ksu     EMPA: add route to fb_cookie
03/24/06   ksu     split out dsrsp from hdrrsp
03/01/06   ksu     EMPA: properly dequeue RoHC feedback-only packets
02/07/06   ksu     EMPA: forward link RoHC testing
01/20/06   ksu     EMPA: link up hdrmrlpcif to hdrmrlpc
12/29/05   ksu     EMPA: framework for route protocol (RP)
12/29/05   ksu     EMPA: trivial route selection protocol (RSP)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_HDR_EMPA
#include "hdrmrlp.h"
#include "hdrrsp.h"
#include "hdrrlp_util.h"
#include "hdrmrlpctl.h"
#include "hdrmrlpcif.h"
#include "dsrsp.h"
#include "amssassert.h"
#include "ps_pkt_info.h"
#ifdef FEATURE_DATA_PS_ROHC
#include "ps_rohc.h"
#endif /* FEATURE_DATA_PS_ROHC */
#include "ps_svc.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"

/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* Declare a task critical section for use in place of TASKLOCK()/TASKFREE(). */
static rex_crit_sect_type hdrrsp_dsm_enqueue_crit_sect;

/*---------------------------------------------------------------------------
  RSP PS COMMAND HANDLING

  Several operations can originate in other tasks.  Since the data
  path is operating in the context of the PS task, commands are used
  to perform all operations within the PS task, for thread safety.
  This is especially important since state transitions can be
  triggered by both a) signaling messages and b) the arrival of data
  on a rising route.

  All the possible command payloads easily fit within 32 bits, so the
  void* user_data_ptr is used directly as a uint32, with byte values
  shifted within it.
---------------------------------------------------------------------------*/
enum 
{
  HDRRSP_CMD_INIT,
  HDRRSP_CMD_INIT_FLOW,
  HDRRSP_CMD_UPDATE_FLOW,
  HDRRSP_CMD_PROCESS_MSG,
  HDRRSP_CMD_CLEANUP
};
#define HDRRSP_CMD_SHIFT                 0
#define HDRRSP_CMD_FLOW_SHIFT            8
#define HDRRSP_CMD_VER_SHIFT            16
#define HDRRSP_CMD_ROUTE_SHIFT           8
#define HDRRSP_CMD_TRANS_ID_SHIFT       16
#define HDRRSP_CMD_MSG_ID_SHIFT         24

/*---------------------------------------------------------------------------
  MESSAGE ID VALUES
---------------------------------------------------------------------------*/
#define HDRRSP_ROUTE_SELECT_ACK_MSG_ID   0x1f
#define HDRRSP_ACTIVATE_ROUTE_ACK_MSG_ID 0x21

/*---------------------------------------------------------------------------
  PRIORITY OF THE RSP SIGNALING MESSAGES
---------------------------------------------------------------------------*/
#define HDRRSP_SIG_PRI_ROUTE_SELECT_ACK_MSG_ID   40
#define HDRRSP_SIG_PRI_ACTIVATE_ROUTE_ACK_MSG_ID 40

/*---------------------------------------------------------------------------
  For creating RSP signaling messages, offset to leave for header 
  information.  The value is in bytes.
---------------------------------------------------------------------------*/
#define HDRRSP_SIGHDR_OFFSET 12

/* Below is a temp hack */
/* This macro is used to unpack the MessageID field from RSP specific messages */
#define HDRRSP_UNPACK_MSG_ID( ptr, val_ptr )  \
           HDRBIT_UNPACK8( ptr, val_ptr, 0, 8 )

/* This macro is used to unpack the TransactionID field from RSP specific messages */
#define HDRRSP_UNPACK_TRANS_ID( ptr, val_ptr ) \
           HDRBIT_UNPACK8( ptr, val_ptr, 8,  8 ) 
           
/* This macro is used to unpack the Route field from RSP specific messages */
#define HDRRSP_UNPACK_ROUTE( ptr, val_ptr ) \
           HDRBIT_UNPACK8( ptr, val_ptr, 16,  1 ) 

/* Flow specific data */
typedef struct
{
  boolean simultaneous_delivery_on_both_routes_fwd;
  hdrcp_mrlpc_route_prot_id_type route_protocol; /* HDRMRLPC_ROUTE_PROT_ID_* */
  void* rp_handle_list[DSRSP_NUM_ROUTES];     /* e.g. RoHC handles */
  boolean open_route_packet_received;
} hdrrsp_flow_type;


typedef void (*hdrrsp_process_msg_func_type)
( 
  byte msg_id,                  /* MessageID */
  byte trans_id,                /* TransactionID */
  byte route                    /* Route */
);
static void hdrrsp_process_msg_setting
( 
  byte msg_id,                  /* MessageID */
  byte trans_id,                /* TransactionID */
  byte route                    /* Route */
);
static void hdrrsp_process_msg_rising
( 
  byte msg_id,                  /* MessageID */
  byte trans_id,                /* TransactionID */
  byte route                    /* Route */
);

typedef dsm_item_type* (*hdrrsp_q_get_item_func_type)
( 
  hdrrsp_flow_type* flow_ptr,   /* the current flow */
  dsm_watermark_type wm_list[], /* the watermark list to get the next item from */
  byte* route_ptr               /* [out] the route that the item came from */
);
static dsm_item_type* hdrrsp_q_get_item_setting
( 
  hdrrsp_flow_type* flow_ptr,   /* the current flow */
  dsm_watermark_type wm_list[], /* the watermark list to get the next item from */
  byte* route_ptr               /* [out] the route that the item came from */
);
static dsm_item_type* hdrrsp_q_get_item_rising
( 
  hdrrsp_flow_type* flow_ptr,   /* the current flow */
  dsm_watermark_type wm_list[], /* the watermark list to get the next item from */
  byte* route_ptr               /* [out] the route that the item came from */
);

/* Most RSP operations are highly state specific */
typedef struct {

  byte open_route;              /* Route that is currently Open */

  hdrrsp_process_msg_func_type process_msg;
  hdrrsp_q_get_item_func_type q_get_item;

} hdrrsp_ops_type;

static struct
{
  /* 2.4.4.1.2 The RSP may be in one of four states */
  hdrrsp_ops_type ops_a_open_b_setting;
  hdrrsp_ops_type ops_a_open_b_rising;
  hdrrsp_ops_type ops_a_setting_b_open;
  hdrrsp_ops_type ops_a_rising_b_open;

  /* Flow specifics */
  hdrrsp_flow_type flow_list_fwd[HDRCP_MRLPC_N_MRLP_MAX_FWD];
  hdrrsp_flow_type flow_list_rev[HDRCP_MRLPC_N_MRLP_MAX_REV];

  /* current state operations */
  hdrrsp_ops_type* ops_ptr;

} hdrrsp =
{
  {
    DSRSP_ROUTE_A,
    hdrrsp_process_msg_setting,
    hdrrsp_q_get_item_setting
  },
  {
    DSRSP_ROUTE_A,
    hdrrsp_process_msg_rising,
    hdrrsp_q_get_item_rising
  },
  {
    DSRSP_ROUTE_B,
    hdrrsp_process_msg_setting,
    hdrrsp_q_get_item_setting
  },
  {
    DSRSP_ROUTE_B,
    hdrrsp_process_msg_rising,
    hdrrsp_q_get_item_rising
  },  
};
#endif /* FEATURE_HDR_EMPA */

/*===========================================================================

FUNCTION HDRRSP_INIT

DESCRIPTION
  Called at connection open. This function is not featurized under EMPA
  to avoid RPC issues. As we do not want to have featurization in the apis.h
  file (dshdr_apis.h in this case), we always remote this API and as a result 
  featurize the function body instead of the function itself.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initialization.
  
===========================================================================*/
void hdrrsp_init( void )
{
#ifdef FEATURE_HDR_EMPA
  uint32 command_data = 0;                                /* PS cmd data   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Forward a command to the PS task for actual initialization */
  command_data |= HDRRSP_CMD_INIT << HDRRSP_CMD_SHIFT;
  ps_send_cmd(PS_HDRRSP_CMD, (void*)command_data);
#endif /* FEATURE_HDR_EMPA */

}

/*===========================================================================

FUNCTION HDRRSP_CLEANUP

DESCRIPTION
  Called at connection close.  Since this can be called from vaious
  tasks, a command is forwarded to the PS task for actual cleanup. This 
  function is not featurized under EMPA to avoid RPC issues. As we do not 
  want to have featurization in the apis.h file (dshdr_apis.h in this case), 
  we always remote this API and as a result featurize the function body 
  instead of the function itself.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrsp_cleanup( void )
{
#ifdef FEATURE_HDR_EMPA
  /* Forward a command to the PS task for actual cleanup */
  ps_send_cmd(PS_HDRRSP_CMD, (void*)HDRRSP_CMD_CLEANUP);
#endif /* FEATURE_HDR_EMPA */
}

#ifdef FEATURE_HDR_EMPA
/*===========================================================================

                     STATIC HELPER FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRRSP_GET_FLOW_PTR

DESCRIPTION
  Get the appropriate flow pointer based on the rlp indentity

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the indicated flow.

SIDE EFFECTS
  None.
  
===========================================================================*/
static hdrrsp_flow_type* hdrrsp_get_flow_ptr
(
  const dsrlp_rlp_identity_type* rlp_id_ptr /* Flow ID */
)
{
  hdrrsp_flow_type* flow_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (rlp_id_ptr->ver)
  {
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      flow_ptr = &hdrrsp.flow_list_rev[rlp_id_ptr->flow];
      break;

    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
      flow_ptr = &hdrrsp.flow_list_fwd[rlp_id_ptr->flow];
      break;

    default:
      DATA_HDR_MSG2(MSG_LEGACY_ERROR, "RSP: unknown flow identity: flow=%d, ver=%d",
                    rlp_id_ptr->flow, (int)rlp_id_ptr->ver);
      break;
  }
 
  return flow_ptr;
}

/*===========================================================================

FUNCTION HDRRSP_RESET_OPEN_ROUTE_PACKET_RECEIVED

DESCRIPTION
  Resets the open_route_packet_received flag for all forward flows.

DEPENDENCIES
  This needs to be called whenever the state is changed to Setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrrsp_reset_open_route_packet_received(void)
{
  hdrrsp_flow_type* flow_ptr;
  int i;

  for (i = 0; i < HDRCP_MRLPC_N_MRLP_MAX_FWD; ++i)
  {
    flow_ptr = &hdrrsp.flow_list_fwd[i];

    flow_ptr->open_route_packet_received = FALSE;
  }
}

/*===========================================================================

FUNCTION HDRRSP_Q_GET_ITEM_RISING

DESCRIPTION
  This function removes an item from the head of the queue of the 
  specified watermark list when the state is Rising for <rising_route> 
  and Open for <open_route>.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to dsm_queue_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified watermark list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the queue of the specified 
  watermark list.

===========================================================================*/
static dsm_item_type* hdrrsp_q_get_item_rising
(
  hdrrsp_flow_type* flow_ptr,   /* the current flow */
  dsm_watermark_type wm_list[], /* the watermark list to get the next item from */
  byte* route_ptr               /* [out] the route that the item came from */
)
{
  byte rising_route = !hdrrsp.ops_ptr->open_route;
  dsm_item_type* item_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 2.4.4.1.2.[2|4].1 Upon receiving Flow Protocol PDU on */
  /* <rising_route> of any Link Flow, the access terminal shall store */
  /* the Flow Protocol PDU received from <rising_route> ... */
  if (wm_list[rising_route].current_cnt > 0)
  {
    if (rising_route == DSRSP_ROUTE_A)
    {
      /* ... for processing in the A Open B Setting state and shall */
      /* transition to the A Open B Setting state. */
      DATA_HDR_MSG0(MSG_LEGACY_MED,
                    "RSP: transitioning to A Open B Setting state");
      hdrrsp.ops_ptr = &hdrrsp.ops_a_open_b_setting;
      hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
    }
    else
    {
      /* ... for processing in the A Setting B Open state and shall */
      /* transition to the A Setting B Open state. */
      DATA_HDR_MSG0(MSG_LEGACY_MED,
                    "RSP: transitioning to A Setting B Open state");
      hdrrsp.ops_ptr = &hdrrsp.ops_a_setting_b_open;
      hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
    }
    hdrrsp_reset_open_route_packet_received();

    /* Since we have just transtioned to a setting state, we need to */
    /* use the corresponding q_get_item(). */
    return hdrrsp.ops_ptr->q_get_item(flow_ptr, wm_list, route_ptr);
  }

  item_ptr = dsm_dequeue(&wm_list[hdrrsp.ops_ptr->open_route]);
  if (item_ptr != NULL)
  {
    *route_ptr = hdrrsp.ops_ptr->open_route;
    return item_ptr;
  }

  return NULL;
}

/*===========================================================================

FUNCTION HDRRSP_Q_GET_ITEM_SETTING

DESCRIPTION
  This function removes an item from the head of the queue of the 
  specified watermark list when the state is Setting for <setting_route> 
  and Open for <open_route>.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to dsm_queue_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified watermark list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the queue of the specified 
  watermark list.

===========================================================================*/
static dsm_item_type* hdrrsp_q_get_item_setting
(
  hdrrsp_flow_type* flow_ptr,   /* the current flow */
  dsm_watermark_type wm_list[], /* the watermark list to get the next item from */
  byte* route_ptr               /* [out] the route that the item came from */
)
{
  byte setting_route = !hdrrsp.ops_ptr->open_route;
  dsm_item_type* item_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 2.4.4.1.2.[1.4|3.3] Receiver requirements */

  /* First discard any unusable <setting_route> packets */
  if (!flow_ptr->simultaneous_delivery_on_both_routes_fwd &&
      flow_ptr->open_route_packet_received &&
      (wm_list[setting_route].current_cnt > 0))
  {
    DATA_HDR_MSG2(MSG_LEGACY_HIGH,
                  "RSP %d: received stale packet(s) for Setting Route %c - discarding", 
                  flow_ptr-hdrrsp.flow_list_fwd, "AB"[setting_route]);
    while (NULL != (item_ptr = dsm_dequeue(&wm_list[setting_route])))
    {
      dsm_free_packet(&item_ptr);
    }
  }
  
  /* Next process any usable <setting_route> packets, as those packets are older */
  item_ptr = dsm_dequeue(&wm_list[setting_route]);
  if (item_ptr != NULL)
  {
    *route_ptr = setting_route;
    return item_ptr;
  }

  /* The access terminal shall pass Flow Protocol PDUs received on */
  /* <open_route> to the Flow Protocol */
  item_ptr = dsm_dequeue(&wm_list[hdrrsp.ops_ptr->open_route]);
  if (item_ptr != NULL)
  {
    flow_ptr->open_route_packet_received = TRUE;
    *route_ptr = hdrrsp.ops_ptr->open_route;
    return item_ptr;
  }

  return NULL;
}

/*===========================================================================

FUNCTION HDRRSP_SEND_MSG

DESCRIPTION
  This function sends a signalling message via SLP.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
static void hdrrsp_send_msg
(
  byte    msg_id,               /* MessageID */
  byte    trans_id,             /* TransactionID */
  boolean reliable,             /* Set to true if reliable delivery is required */
  uint8   priority              /* Priority of the message */
)
{
  dsm_item_type            *item_ptr;      /* ptr to sig msg queued to SLP */
  uint8                    field;          /* field to insert into message */
  hdrcom_hai_stream_enum_type  stream_id;
  uint8                    hmp_hdr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Build the message for SLP by adding the message type.  Allocate a */
  /* DSM item to hold the signaling message. Account for lower layer */
  /* headers by including a header offset */
  item_ptr = dsm_offset_new_buffer(DSM_DS_POOL_SIZE(DSM_DS_SMALL_ITEM_SIZ),
                                   HDRRSP_SIGHDR_OFFSET);
  if (item_ptr == NULL)
  {
    DATA_HDR_MSG1(MSG_LEGACY_HIGH,
                  "RSP: mem alloc failed for msg_id=0x%x", msg_id);
    return; 
  }

  /* Insert the MessageID */
  field = msg_id;
  (void)dsm_pushdown_tail(&item_ptr, &field, 1, DSM_DS_SMALL_ITEM_POOL);

  /* Insert the TransactionID */
  field = trans_id;
  (void)dsm_pushdown_tail(&item_ptr, &field, 1, DSM_DS_SMALL_ITEM_POOL);

  /* Determine the HMP header to add to the messages when they are */
  /* queued to SLP based on the stream for which they are meant. */
  if( (hdrcp_stream_get_stream( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, &stream_id ) == FALSE ) &&
      (hdrcp_stream_get_stream( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN, &stream_id ) == FALSE ) )
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RSP: could not get stream for EMPA. Not queueing any msgs");
    dsm_free_packet(&item_ptr);
    return;
  }
  switch(stream_id)
  {
    case HDRHAI_STREAM1:
      hmp_hdr = (uint8)HDRHAI_N_APP1_TYPE;
      break;
    case HDRHAI_STREAM2:
      hmp_hdr = (uint8)HDRHAI_N_APP2_TYPE;
      break;
    case HDRHAI_STREAM3:
      hmp_hdr = (uint8)HDRHAI_N_APP3_TYPE;
      break;
    default:
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RSP: EMPA mapped to invalid stream");
      dsm_free_packet(&item_ptr);
      return;
  }
  
  /* Send the message directly to SLP without passing through HMP.  As */
  /* such, the HMP header must be added, prior to queueing for SLP. */
  /* Note that since we have created the new item with a specified */
  /* offset, the dsm_pushdown() will not allocate a new item. */
  (void)dsm_pushdown(&item_ptr, (void *)&hmp_hdr, 1, DSM_DS_SMALL_ITEM_POOL);

  /* Queue directly to SLP.  SLP is responsible for freeing the memory */
  /* items used for message building. */
  DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                "RSP: queue msg_id=0x%x to SLP", msg_id);
  if (hdrcp_slp_send_msg(HDRHAI_RTC,                         /* HDR channel */
                      reliable,
                      priority,
                      item_ptr,                    /* pointer to sig msg */
                      NULL,      /* callback after message has been sent */
                      NULL)                 /* user data for cb function */
      != E_SUCCESS)
  {
    DATA_HDR_MSG1(MSG_LEGACY_ERROR,
                  "Unable to send the msg_id=0x%x", msg_id);
    dsm_free_packet(&item_ptr);
    return;
  }
}

/*===========================================================================

FUNCTION HDRRSP_PROCESS_MSG_RISING

DESCRIPTION
  This function processes incoming messages for this protocol when the
  state is Rising for <rising_route> and Open for <open_route>
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
static void hdrrsp_process_msg_rising
( 
  byte msg_id,                  /* MessageID */
  byte trans_id,                /* TransactionID */
  byte route                    /* Route */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 2.4.4.1.2.[2|4].1 */
  switch (msg_id)
  {
    case HDRRSP_ROUTE_SELECT_MSG_ID:
    {
      DATA_HDR_MSG1(MSG_LEGACY_MED,
                    "RSP: received RouteSelect msg for Route %c - handling", 
                    "AB"[route]);

      /* Upon receiving a RouteSelect message requesting to select */
      /* <route>, the access terminal shall respond with a */
      /* RouteSelectAck message... */
      hdrrsp_send_msg(HDRRSP_ROUTE_SELECT_ACK_MSG_ID,
                      trans_id,
                      FALSE,  /* Best Effort */
                      HDRRSP_SIG_PRI_ROUTE_SELECT_ACK_MSG_ID);
      DATA_HDR_MSG1(MSG_LEGACY_MED,
                    "RSP: transitioning to A Open B Setting/Open state route %c ", 
                    "AB"[route]);

      if (route == DSRSP_ROUTE_A)
      {
        /* ... and shall transition to the A Open B Setting state */
        
        hdrrsp.ops_ptr = &hdrrsp.ops_a_open_b_setting;
        hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
      }
      else
      {
        /* ... and shall transition to the A Setting B Open state */
        
        hdrrsp.ops_ptr = &hdrrsp.ops_a_setting_b_open;
        hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
      }
      hdrrsp_reset_open_route_packet_received();
      break;
    }

    case HDRRSP_ACTIVATE_ROUTE_MSG_ID:
    {
      /* If the access terminal receives an ActivateRoute message, the */
      /* message shall be ignored. */
      DATA_HDR_MSG1(MSG_LEGACY_MED,
                    "RSP: received ActivateRoute msg for Route %c - ignoring", 
                    "AB"[route]);
      break;
    }

    default:		
	  DATA_HDR_MSG1(MSG_LEGACY_ERROR, " Invalid Msg_ID : %d ", msg_id);
      ASSERT(0);
      break;
  }
}

/*===========================================================================

FUNCTION HDRRSP_PROCESS_MSG_SETTING

DESCRIPTION
  This function processes incoming messages for this protocol when the
  state is Setting for <setting_route> and Open for <open_route>
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
static void hdrrsp_process_msg_setting
( 
  byte msg_id,                  /* MessageID */
  byte trans_id,                /* TransactionID */
  byte route                    /* Route */
)
{
  byte setting_route = !hdrrsp.ops_ptr->open_route;
  hdrrsp_flow_type* flow_ptr = NULL;
  errno_enum_type err_num;
  int i;
  uint8 verno;  
  uint32 value;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( setting_route > (DSRSP_NUM_ROUTES -1) )
  {
          /* Assert as setting_route is not valid range*/
          ASSERT(0);
  }
        

  /* 2.4.4.1.2.[1|3].1 */
  switch (msg_id)
  {
    case HDRRSP_ROUTE_SELECT_MSG_ID:
      if (route == hdrrsp.ops_ptr->open_route)
      {
        DATA_HDR_MSG1(MSG_LEGACY_MED, 
                      "RSP: received RouteSelect msg for Open Route %c - handling",
                      "AB"[hdrrsp.ops_ptr->open_route]);
        
        /* Upon receiving a RouteSelect message for Route */
        /* <open_route>, the access terminal shall respond with a */
        /* RouteSelectAck message. */
        hdrrsp_send_msg(HDRRSP_ROUTE_SELECT_ACK_MSG_ID,
                        trans_id,
                        FALSE,  /* Best Effort */
                        HDRRSP_SIG_PRI_ROUTE_SELECT_ACK_MSG_ID);
      }
      else
      {
        /* The standard does not specify this case, but since this */
        /* would only happen if the AN misbehaves, it should be safe */
        /* to simply discard the message. */
        DATA_HDR_MSG1(MSG_LEGACY_ERROR, "RSP: received RouteSelect msg for "
                      "Setting Route %c - discarding", 
                      "AB"[setting_route]);
        return;
      }
      break;

    case HDRRSP_ACTIVATE_ROUTE_MSG_ID:
      if(hdrmrlpcif_get_two_route_supported_attrib(HDRMRLPCIF_CLIENT_MRLP_SN,
                                    &value,
                                    &verno) == E_FAILURE)
      {
	    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RSP: received RouteActivate msg, "
                      "can't retrieve the attribute--TwoRouteSupported ");
        return;
      }
      if (route == DSRSP_ROUTE_B && 
		  route == setting_route && 
		  value == 0)
      {
        DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RSP: received RouteActivate msg, "
                      "on Route B, when TwoRouteSupported==0, "
                      "ignore the message ");
		return;
      }
      if (route == setting_route)
      {
        DATA_HDR_MSG1(MSG_LEGACY_MED, 
                      "RSP: received ActivateRoute msg for Setting Route %c - handling",
                      "AB"[setting_route]);

        /* Upon receiving an ActivateRoute message requesting to */
        /* activate <setting_route>, the access terminal shall perform */
        /* the following: */

        /* The Route Selection Protocol shall issue a */
        /* RadioLinkProtocol.InitializeRoute command with */
        /* <setting_route> as the argument. */
        hdrmrlpctl_rlp_initialize_route(setting_route);

        /* The access terminal shall initialize the Route Protocol */
        /* bound to <setting_route>. */

#ifdef FEATURE_DATA_PS_ROHC
        for (i = 0; i < HDRCP_MRLPC_N_MRLP_MAX_FWD; ++i)
        {
          flow_ptr = &hdrrsp.flow_list_fwd[i];

          if (flow_ptr->route_protocol == HDRMRLPC_ROUTE_PROT_ID_ROHC)
          {
            (void)rohc_ioctl(flow_ptr->rp_handle_list[setting_route],
                             ROHC_IOCTL_CONTEXT_REINITIALIZATION,
                             NULL,
                             &err_num);
          }
        }
        for (i = 0; i < HDRCP_MRLPC_N_MRLP_MAX_REV; ++i)
        {
          flow_ptr = &hdrrsp.flow_list_rev[i];

          if (flow_ptr->route_protocol == HDRMRLPC_ROUTE_PROT_ID_ROHC)
          {
            (void)rohc_ioctl(flow_ptr->rp_handle_list[setting_route],
                             ROHC_IOCTL_CONTEXT_REINITIALIZATION,
                             NULL,
                             &err_num);
          }
        }
#endif /* FEATURE_DATA_PS_ROHC */

        /* After the Radio Link Protocol and the Route Protocol are */
        /* initialized, the access terminal shall send an */
        /* ActivateRouteAck message, ... */
        hdrrsp_send_msg(HDRRSP_ACTIVATE_ROUTE_ACK_MSG_ID,
                        trans_id,
                        TRUE,  /* Reliable */
                        HDRRSP_SIG_PRI_ACTIVATE_ROUTE_ACK_MSG_ID);
 
        if (route == DSRSP_ROUTE_A)
        {
          /* ... and shall transition to the A Rising B Open state */
          DATA_HDR_MSG0(MSG_LEGACY_MED,
                        "RSP: transitioning to A Rising B Open state");
          hdrrsp.ops_ptr = &hdrrsp.ops_a_rising_b_open;
          hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
        }
        else
        {
          /* ... and shall transition to the A Open B Rising state */
          DATA_HDR_MSG0(MSG_LEGACY_MED,
                        "RSP: transitioning to A Open B Rising state");
          hdrrsp.ops_ptr = &hdrrsp.ops_a_open_b_rising;
          hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
        }
      }
      else 
      {
        /* The standard does not specify this case, but since this */
        /* would only happen if the AN misbehaves, it should be safe */
        /* to simply discard the message. */
        DATA_HDR_MSG1(MSG_LEGACY_ERROR,
                      "RSP: received ActivateRoute msg for Open Route %c - discarding", 
                      "AB"[hdrrsp.ops_ptr->open_route]);
        return;
      }
      break;

    default:		
      DATA_HDR_MSG1(MSG_LEGACY_MED, " Invalid Msg_ID : %d ", msg_id);
      ASSERT(0);
      break;
  }
}

/*===========================================================================

FUNCTION HDRRSP_CMD_HANDLER

DESCRIPTION
  This function processes incoming messages for this protocol when the
  state is A Open B Setting.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
static void hdrrsp_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  uint32 command_data = (uint32)user_data_ptr;
  byte command;
  uint8 flow;               /* RLP FLow used in sig msgs to identify RLP   */
  dsrlp_ver_enum_type ver;  /* Version of M-RLP (Fwd or Rev)               */
  byte msg_id;                                            /* MessageID     */
  byte trans_id;                                          /* TransactionID */
  byte route;                                             /* Route         */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(cmd == PS_HDRRSP_CMD);

  command = (byte)(command_data >> HDRRSP_CMD_SHIFT);
  switch (command)
  {
    case HDRRSP_CMD_INIT:       /* connection opened */
    {
      /* No global attributes at this time */
    }
    break;

    case HDRRSP_CMD_INIT_FLOW:  /* flow activated */
    case HDRRSP_CMD_UPDATE_FLOW: /* GAUP received */
    {
      flow = (byte)(command_data >> HDRRSP_CMD_FLOW_SHIFT);
      ver = (dsrlp_ver_enum_type)(command_data >> HDRRSP_CMD_VER_SHIFT);

      if (ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD)
      {
        /* Get the latest FlowNNSimultaneousDeliveryOnBothRoutesFwd */
        /* configuration attribute */
        (void)hdrmrlpcif_get_simultaneous_delivery_on_both_routes_fwd_attrib
          (
            HDRMRLPCIF_CLIENT_MRLP_SN,
            flow,
            ver,
            &hdrrsp.flow_list_fwd[flow].simultaneous_delivery_on_both_routes_fwd
          );
      }
    }
    break;

    case HDRRSP_CMD_PROCESS_MSG: /* message received */
    {
      msg_id = (byte)(command_data >> HDRRSP_CMD_MSG_ID_SHIFT);
      trans_id = (byte)(command_data >> HDRRSP_CMD_TRANS_ID_SHIFT);
      route = (byte)(command_data >> HDRRSP_CMD_ROUTE_SHIFT);
      
      /* Message processing is highly state dependent */
      ASSERT(hdrrsp.ops_ptr->process_msg);
      hdrrsp.ops_ptr->process_msg(msg_id, trans_id, route);
    }
    break;

    case HDRRSP_CMD_CLEANUP:    /* connection closed */
    {
      /* 2.4.4.1.2 If the Route Protocol receives an ConnectedState.ConnectionClosed */
      /* indication, it shall enter the A Open B Setting state. */
      hdrrsp.ops_ptr = &hdrrsp.ops_a_open_b_setting;
      hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
      hdrrsp_reset_open_route_packet_received();
    }
    break;

    default:
    {
      ERR_FATAL("RSP: unknown command: 0x%x", command_data, 0, 0);
      break;
    }
  }
}


/*===========================================================================

                     EXTERNALLY VISIBLE FUNCTIONS

===========================================================================*/


/*===========================================================================

FUNCTION HDRRSP_CLEAR_ROUTE_PROTOCOL

DESCRIPTION
  Unsets the route protocol and optional handle list for an RSP instance

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrsp_clear_route_protocol
(
  const dsrlp_rlp_identity_type* rlp_id_ptr /* Flow ID */
)
{
  hdrrsp_flow_type* flow_ptr = hdrrsp_get_flow_ptr(rlp_id_ptr);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(flow_ptr != NULL)
  {
    flow_ptr->route_protocol = HDRMRLPC_ROUTE_PROT_ID_NULL;
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Null flow ptr. Unable to clear route prot");
  }
}

/*===========================================================================

FUNCTION HDRRSP_Q_GET

DESCRIPTION
  This function removes an item from the head of a specified queue
  list, and passes it through any configured route protocol.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to dsm_queue_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified watermark list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the queue of the specified 
  watermark list.

===========================================================================*/
dsm_item_type* hdrrsp_q_get
(
  const dsrlp_rlp_identity_type* rlp_id_ptr, /* Flow ID */
  dsm_watermark_type wm_list[]  /* the watermark list to get the next item from */
)
{
  dsm_item_type*       item_ptr = NULL;
  dsm_item_type*       fb_pkt = NULL;
  ip_version_enum_type ip_ver;
  errno_enum_type      err;
  hdrrsp_flow_type*    flow_ptr = NULL;
  void*                rohc_handle = NULL;
  byte                 route;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  flow_ptr = hdrrsp_get_flow_ptr(rlp_id_ptr);

  if(flow_ptr == NULL || rlp_id_ptr->ver != DSRLP_VER_HDR_MULTIFLOW_FORWARD)
  {
    return item_ptr;
  }
  /*If rlp_id_ptr->ver gets changed after this point, flow_ptr->route_protocol
    will be set to HDRMRLPC_ROUTE_PROT_ID_NULL, which is handled.
    rohc_decompress() also handles case of rohc_handle == NULL*/
  
  /* Process the rx queue until a) the rx queue is empty, or */
  /* b) there is a valid item to return */
  while (1)
  {
    /* Get the next item from the appropriate rx queue */
    ASSERT(hdrrsp.ops_ptr->q_get_item);
    item_ptr = hdrrsp.ops_ptr->q_get_item(flow_ptr, wm_list, &route);
    if (item_ptr == NULL)
    {
      /* a) the rx queue is empty */
      break;
    }

    switch(flow_ptr->route_protocol)
    {
      case HDRMRLPC_ROUTE_PROT_ID_NULL:
        /* Simple pass thru */
        break;
          
#ifdef FEATURE_DATA_PS_ROHC          
      case HDRMRLPC_ROUTE_PROT_ID_ROHC:
      {
        rohc_handle = flow_ptr->rp_handle_list[route];
          
        /* ROHC decompress */
        if (rohc_decompress(rohc_handle, 
                            NULL, 
                            NULL, 
                            &item_ptr, 
                            &ip_ver, 
                            &fb_pkt, 
                            (uint16)item_ptr->app_field,
                            &err
                            ) != 0)
        {
          /* De-Compression Error */
          DATA_HDR_MSG1(MSG_LEGACY_ERROR, "RSP: RoHC De-Compression Error (%d), "
                        "dropping packet!", err);
          dsm_free_packet(&item_ptr);
        }   
 
        if(fb_pkt != NULL)
        {
          DATA_HDR_MSG0(MSG_LEGACY_HIGH, "No support for forwarding feedback");
          dsm_free_packet(&fb_pkt);
        }

        /* Note that the item_ptr may have become NULL at this point */
        /* for two reasons: 1) a decompression failure just caused the */
        /* item to be freed, or 2) a feedback-only packet was received */
        /* and the decompressor forwarded it to the associated */
        /* compressor.  In either case, we need to process the next */
        /* packet on the rx queue, if present.  Else we are done. */

        break;
      }
#endif /* FEATURE_DATA_PS_ROHC */
        
      default:
        ERR_FATAL("RSP: flow %d: Unknown route protocol: %d",
                  rlp_id_ptr->flow, (int)flow_ptr->route_protocol, 0);
        break;
    }

    if (item_ptr != NULL)
    {
      /* b) there is a valid item to return */
      break;
    }
  }

  return item_ptr;
}

/*===========================================================================

FUNCTION HDRRSP_WM_CURRENT_CNT

DESCRIPTION
  Returns the total number of bytes ready for tx in all routes.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

RETURN VALUE
  uint32 - the current count in all routes

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdrrsp_wm_current_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr, /* Flow ID */
  dsm_watermark_type wm_list[]  /* Tx wm list[DSRSP_NUM_ROUTES] */
)
{
  uint32 current_cnt = 0;
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE);

  /* Note that the SELECTED route only plays a role on rx; all packets */
  /* from all routes will be tx. */
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p) 
  {
    current_cnt += wm_list[p].current_cnt;
  }

  return current_cnt;
}

/*===========================================================================

FUNCTION HDRRSP_WM_FREE_CNT

DESCRIPTION
  Returns the total number of bytes free in the selected route

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

RETURN VALUE
  uint32 - the current free count in the selected route

SIDE EFFECTS
  None
===========================================================================*/
uint32 hdrrsp_wm_free_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr, /* Flow ID */
  dsm_watermark_type wm_list[]  /* Tx wm list[DSRSP_NUM_ROUTES] */
)
{
  ASSERT(rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE);

  if (wm_list[hdrrsp.ops_ptr->open_route].hi_watermark < 
      wm_list[hdrrsp.ops_ptr->open_route].current_cnt)
  {
    return 0;
  }
  else
  {
    return wm_list[hdrrsp.ops_ptr->open_route].hi_watermark -
      wm_list[hdrrsp.ops_ptr->open_route].current_cnt;
  }
}

/*===========================================================================
FUNCTION HDRRSP_DSM_ENQUEUE()

DESCRIPTION
  This function will put the passed DSM item to the queue of the passed 
  shared watermark list then check for and perform any 'put' events.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrsp_dsm_enqueue
(
  const dsrlp_rlp_identity_type* rlp_id_ptr, /* Flow ID */
  dsm_watermark_type wm_list[], /* Pointer to Watermark item to put to */
  dsm_item_type **pkt_head_ptr, /* Pointer to pointer to item to add to queue */
  ds1x_meta_info_type *ds1x_meta_info_ptr /* Pointer to ds_meta_info to prepend */
)
{
  hdrrsp_flow_type* flow_ptr;
  errno_enum_type   err;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  flow_ptr = hdrrsp_get_flow_ptr(rlp_id_ptr);

  if(flow_ptr == NULL || rlp_id_ptr->ver != DSRLP_VER_HDR_MULTIFLOW_REVERSE)
  {
    dsm_free_packet(pkt_head_ptr);
    return;
  }
  /*If rlp_id_ptr->ver gets changed after this point, flow_ptr->route_protocol
    will be set to HDRMRLPC_ROUTE_PROT_ID_NULL, which is handled.
    rohc_compress() also handles case of rohc_handle == NULL */
  
  switch(flow_ptr->route_protocol)
  {
    case HDRMRLPC_ROUTE_PROT_ID_NULL:
      /* Simple pass thru */
      break;

#ifdef FEATURE_DATA_PS_ROHC
    case HDRMRLPC_ROUTE_PROT_ID_ROHC:
    {
      void* rohc_handle = flow_ptr->rp_handle_list[hdrrsp.ops_ptr->open_route];

      /* ROHC compress */
      if (rohc_compress(rohc_handle, NULL, NULL, pkt_head_ptr, &err) != 0) 
      {
        /* Compression Error */
        DATA_HDR_MSG0(MSG_LEGACY_ERROR,
                      "RSP: RoHC Compression Error, dropping packet!!");
        dsm_free_packet(pkt_head_ptr);
        return;
      }

      break;
    }
#endif /* FEATURE_DATA_PS_ROHC */

    default:
      ERR_FATAL("RSP: flow %d: Unknown route protocol: %d",
                rlp_id_ptr->flow, (int)flow_ptr->route_protocol, 0);
      break;
  }

  if (ds1x_meta_info_ptr) 
  {
    (void)dsm_pushdown( pkt_head_ptr,
                  ds1x_meta_info_ptr,
                  sizeof(ds1x_meta_info_type),
                  DSM_DS_SMALL_ITEM_POOL);
	(*pkt_head_ptr)->kind = DS1X_META_INFO_KIND;
  }
  if(rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE)
  {
    rex_enter_crit_sect(&hdrrsp_dsm_enqueue_crit_sect);
    dsm_enqueue(&wm_list[hdrrsp.ops_ptr->open_route], pkt_head_ptr);
    rex_leave_crit_sect(&hdrrsp_dsm_enqueue_crit_sect);
  }
  else
  {
    dsm_free_packet(pkt_head_ptr);
    return;
  }  
}

/*===========================================================================

FUNCTION HDRRSP_SET_ROUTE_PROTOCOL

DESCRIPTION
  Sets the route protocol and optional handle list for an RSP instance

DEPENDENCIES
  The handle list (if non-NULL) must have at least DSRSP_NUM_ROUTES
  entries, each of which must have been properly initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrsp_set_route_protocol
(
  dsrlp_rlp_identity_type* rlp_id_ptr, /* Flow ID */
  hdrcp_mrlpc_route_prot_id_type route_protocol, /* HDRMRLPC_ROUTE_PROT_ID_* */
  void *rp_handle_list[]        /* (optional) handle list */
)
{
  hdrrsp_flow_type* flow_ptr = hdrrsp_get_flow_ptr(rlp_id_ptr);
  int p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(flow_ptr == NULL)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR,"Null Flow Ptr Unable to set Route Protocol");
    return;
  }

  flow_ptr->route_protocol = route_protocol;

  if (rp_handle_list != NULL) 
  {
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p) 
    {
      flow_ptr->rp_handle_list[p] = rp_handle_list[p];
    }
  }
}

/*===========================================================================

FUNCTION HDRRSP_GET_OPEN_ROUTE

DESCRIPTION
  Returns the active route

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
byte hdrrsp_get_open_route(void)
{
  DATA_HDR_MSG1(MSG_LEGACY_MED,
                "Open route is %c", 
                "AB"[hdrrsp.ops_ptr->open_route]);
  return hdrrsp.ops_ptr->open_route;  
}

/*===========================================================================

FUNCTION HDRRSP_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrsp_msg_cb
( 
  dsm_item_type** msg_ptr_ptr   /* handle to received message buffer */
)
{
/* Below is a temp hack */
  byte msg_id;                                            /* MessageID     */
  byte trans_id;                                          /* TransactionID */
  byte route;                                             /* Route         */
  uint32 command_data = 0;                                /* PS cmd data   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Process signaling message */
  DATA_HDR_MSG0(MSG_LEGACY_MED, "RSP: received signalling message");

  /* The MessageID is the first byte of the header.  Obtain the message ID */
  /* and process accordingly.   */
  if (HDRRSP_UNPACK_MSG_ID(*msg_ptr_ptr, &msg_id) != E_SUCCESS)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR,
                  "RSP: err extracting MessageID from msg - discarding");
    dsm_free_packet(msg_ptr_ptr);
    return;
  }

  switch (msg_id)
  {
    case HDRRSP_ROUTE_SELECT_MSG_ID:
    case HDRRSP_ACTIVATE_ROUTE_MSG_ID:
    {
      /* Extract the TransactionID */
      if (HDRRSP_UNPACK_TRANS_ID(*msg_ptr_ptr, &trans_id) != E_SUCCESS)
      {
        DATA_HDR_MSG1(MSG_LEGACY_ERROR, "RPS: err extracting TransactionID "
                      "from msg 0x%x - discarding",
                      msg_id);
        break;
      }

      /* Determine the route */
      if (HDRRSP_UNPACK_ROUTE(*msg_ptr_ptr, &route) != E_SUCCESS)
      {
        DATA_HDR_MSG2(MSG_LEGACY_ERROR, "RSP: err extracting route from msg "
                      "0x%x, trans 0x%x - discarding",
                      msg_id, trans_id);
        break;
      }
 
      /* Forward a command to the PS task for actual processing */
      command_data |= HDRRSP_CMD_PROCESS_MSG << HDRRSP_CMD_SHIFT;
      command_data |= msg_id << HDRRSP_CMD_MSG_ID_SHIFT;
      command_data |= trans_id << HDRRSP_CMD_TRANS_ID_SHIFT;
      command_data |= route << HDRRSP_CMD_ROUTE_SHIFT;
      ps_send_cmd(PS_HDRRSP_CMD, (void*)command_data);
      
      break;
    }

    default:
      /* Unknown message received */
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "RSP: unrecognized msg 0x%x - discarding",
                    msg_id);
      break;

  }

  /* Make sure the message gets freed */
  if (*msg_ptr_ptr != NULL)
  {
    dsm_free_packet(msg_ptr_ptr);
  }
}

/*===========================================================================

FUNCTION HDRRSP_UPDATE_FLOW

DESCRIPTION
  Called when a GAUP message is received for this flow.  Update
  configuration attributes that may have changed.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initialization.
  
===========================================================================*/
void hdrrsp_update_flow
( 
  const dsrlp_rlp_identity_type* rlp_id_ptr /* Flow ID */
)
{
  uint32 command_data = 0;                                /* PS cmd data   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Forward a command to the PS task for actual initialization */
  command_data |= HDRRSP_CMD_UPDATE_FLOW << HDRRSP_CMD_SHIFT;
  command_data |= (uint32)rlp_id_ptr->flow << HDRRSP_CMD_FLOW_SHIFT;
  command_data |= (uint32)rlp_id_ptr->ver << HDRRSP_CMD_VER_SHIFT;
  ps_send_cmd(PS_HDRRSP_CMD, (void*)command_data);
}

/*===========================================================================

FUNCTION HDRRSP_INIT_FLOW

DESCRIPTION
  Called at flow activation.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initialization.
  
===========================================================================*/
void hdrrsp_init_flow
( 
  const dsrlp_rlp_identity_type* rlp_id_ptr /* Flow ID */
)
{
  uint32 command_data = 0;                                /* PS cmd data   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Forward a command to the PS task for actual initialization */
  command_data |= HDRRSP_CMD_INIT_FLOW << HDRRSP_CMD_SHIFT;
  command_data |= (uint32)rlp_id_ptr->flow << HDRRSP_CMD_FLOW_SHIFT;
  command_data |= (uint32)rlp_id_ptr->ver << HDRRSP_CMD_VER_SHIFT;
  ps_send_cmd(PS_HDRRSP_CMD, (void*)command_data);
}


/*===========================================================================

FUNCTION HDRRSP_STARTUP

DESCRIPTION
  Called once at powerup for initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrrsp_startup( void )
{
  /* 2.4.4.1.2 If no other Link Flows are activated when a Link Flow */
  /* is activated, the the Route Selection Protocol shall enter the A */
  /* Open B Setting state. */
  hdrrsp.ops_ptr = &hdrrsp.ops_a_open_b_setting;
  hdrmrlp_update_active_route(hdrrsp.ops_ptr->open_route);
  hdrrsp_reset_open_route_packet_received();

  /* Handle commands coming from other tasks */
  (void)ps_set_cmd_handler(PS_HDRRSP_CMD, hdrrsp_cmd_handler);

  /* Init critical section variable */
   rex_init_crit_sect (&hdrrsp_dsm_enqueue_crit_sect);

}

#endif /* FEATURE_HDR_EMPA */
