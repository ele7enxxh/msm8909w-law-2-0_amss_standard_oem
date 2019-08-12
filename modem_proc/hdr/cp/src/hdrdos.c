/*===========================================================================

       H D R   D A T A   O V E R   S I G N A L I N G  P R O T O C O L  


DESCRIPTION
  This file implements the Data Over Signaling protocol, which is part of the
  Multi-Flow Packet Application, defined in section 4.5 of IS-856-A.

EXTERNALIZED FUNCTIONS (Global)
  hdrdos_send_dos_msg - Send a Data Over Signaling Message
  hdrdos_msg_cb - Callback function that queues incoming messages for this 
                  protocol
  hdrdos_init - Initializes the HDRDOS protocol

EXTERNALIZED FUNCTIONS (Regional)
  hdrdos_process_msg - Process message received for the protocol
  hdrdos_process_cmd - Process command received for the protocol
  hdrdos_process_timer - Process timer expiration event for the protocol

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2004 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrdos.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/02/14   vko     Fixed race condition in dos timer handler
07/09/14   cnx     Support NV refresh without reset.
05/06/13   mbs     Fixed compiler errors for BOLT build
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
12/01/10   cnx     Fixed a bug that dsm item is released twice when DOS 
                   msg does not have payload. 
12/01/09   suren   Added QChat optimization changes.
10/27/09   pba     Klockwork fixes.
08/14/09   wsh     Fixed LUP/DOS related message not being processed in MMPA
08/05/09   pba     Cancel sending DOS msg if it can not fit in the 1st probe. 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
12/10/08   smd     Replaced assert.h with amssassert.h
11/10/08   smd     Replaced errno.h with IxErrno.h  
08/17/08   pba     Always initialize rlp_flow and route not just for EMPA.
09/11/07   wsh     Removed lint high warnings
08/16/07   wsh     Removed compiler and lint critical warnings
06/28/07   pba     Strip SDB header before sending the DBM as DOS.
06/11/07   yll     Removed hdrrxi.h from the include files.
05/08/07   pba     Since EMPA-DOS header has 1 extra byte than MFPA DOS, 
                   subtract the additional byte from max allowed DOS payload
                   in case of EMPA-DOS.
05/01/07   pba     Fixed F3 message error.
04/30/07   pba     Added HDRTRACE_DOS_MO_DOS_STATUS.
04/23/07   pba     While forwarding the MT DOS dsm_item to CM also explicitly 
                   pass the size of the DOS payload.
04/10/07   pba     Fixed an issue where DOS ACK timer was not being reset even 
                   after DOSACK message was received.
02/12/07   mpa     Fixed RVCT warning.
12/19/06   pba     Reverted DOS link_flow_number to rlp_flow.
11/17/06   pba     Added support for DOS over EMPA.
10/19/06   pba     Fixed initialization of DOS message TX sequence number.
06/05/06   pba     Added support to store session data in EFS.
05/22/06   pba     Modified F3 message.
04/07/06   hal     Changed dsm_pushdown() to use hdrutil_pushdown_head()
11/18/05   hal     Uses CM API instead of directly accessing cm_mc_rpt_free_q
09/30/05   pba     Set V(R) to sequence number in last valid DOS message
                   F3 message cleanup
09/26/05   pba     Fixed bug in DOS ACK message packing and sequence number
                   wrap around on RESET
08/29/05   pba     Added DOS protocol addendum changes.
06/22/05   pba     No fragmentation of DOS message on access channel.
06/22/05   pba     Allow up to 3 simultaneous DOS messages queued for TX.
06/22/05   pba     Added hdrdos_timer_handler().
06/22/05   pba     Modified packing routine for DOS message.
05/09/05  ar/pba   Integrated Data Over Signaling protocol code
08/02/04   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "hdrhai.h"
#include "task.h"
#include "hdrdosi.h"
#include "hdrdos.h"
#include "msg.h"
#include "err.h"
#include "hdrhmp.h"
#include "hdrmci.h"
#include "hdrbit.h"
#include "hdrstream.h"
#include "amssassert.h"
#include "hdrtrace.h"
#include "IxErrno.h"
#include "hdrdebug.h"
#include "hdrutil.h"
#include "hdrovhd.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrscmdb.h"

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk_opts.h"
#include "hdrqchatupk.h"
#endif /*FEATURE_HDR_QCHAT*/

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRDOS_TASK_PTR hdrmc_tcb_ptr

/*---------------------------------------------------------------------------
                            PROTOCOL MESSAGE HEADER                      
---------------------------------------------------------------------------*/

/* Message ID field type. */
typedef uint8 hdrdos_msg_id_type;

/* Get message ID field. */
#define HDRDOS_GET_MSG_ID( dsm_ptr, val_ptr ) \
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, 0, 8 )

/* Pack MessageID field */
#define HDRDOS_PACK_MESSAGE_ID( dsm_ptr, val ) \
               HDRBIT_PACK8( dsm_ptr, val, 0, 8 )

/*---------------------------------------------------------------------------
                         DATA OVER SIGNALING MESSAGE                      
---------------------------------------------------------------------------*/

/* Message priority. */
#define HDRDOS_DATA_OVER_SIGNALING_MSG_PRI 50

/* Field sizes in DOS message */
#define HDRDOS_DOS_MSG_ID_SIZE 8
#define HDRDOS_DOS_MSG_LINK_FLOW_NUMBER_SIZE 5
#define HDRDOS_DOS_MSG_ROUTE_SIZE 1
#define HDRDOS_DOS_MSG_ACK_REQUIRED_SIZE 1
#define HDRDOS_DOS_MSG_RESET_SIZE 1
#define HDRDOS_DOS_MSG_RESERVED_SIZE 2
#define HDRDOS_DOS_MSG_MSG_SEQUENCE_SIZE 6

/************************************
  DOS message header unpacking macros 
 ************************************/

/* Get Link flow number field */
#define HDRDOS_GET_LINK_FLOW_NUMBER( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 5 )

/* Get Route field */
#define HDRDOS_GET_ROUTE( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 1 )

/* Get AckRequired field */
#define HDRDOS_GET_ACK_REQUIRED( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 1 )

/* Get Reset field */
#define HDRDOS_GET_RESET( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 1 )

/* Get MessageSequence field */
#define HDRDOS_GET_MESSAGE_SEQUENCE( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 6 )

/********************************** 
  DOS message header packing macros 
**********************************/

#define HDRDOS_CLR_DOS_HDR( ptr )\
          ( *((byte*)(ptr)  ) = 0, \
            *((byte*)(ptr)+1) = 0, \
            *((byte*)(ptr)+2) = 0 )

/* Pack DOS message ID */
#define HDRDOS_PACK_DOS_MSGID( ptr, val )\
          ( *((byte*)(ptr)) = (byte)(val) )

/* Pack ACK required field */
#define HDRDOS_PACK_ACK_REQUIRED( ptr, val )\
          ( *((byte*)(ptr)+1) |= (byte)(val) << 7 )

/* Pack MessageSequence field */
#define HDRDOS_PACK_MESSAGE_SEQUENCE( ptr, val )\
          ( *((byte*)(ptr)+1) |= (byte)(val) )

/* Pack Link flow number field */
#define HDRDOS_PACK_EMPADOS_LINK_FLOW_NUMBER( ptr, val )\
          ( *((byte*)(ptr)+1) |= (byte)(val) << 3 )

/* Pack route field */
#define HDRDOS_PACK_EMPADOS_ROUTE( ptr, val )\
          ( *((byte*)(ptr)+1) |= (byte)(val) << 2 )

/* Pack ACK required field for EMPA DOS */
#define HDRDOS_PACK_EMPADOS_ACK_REQUIRED( ptr, val )\
          ( *((byte*)(ptr)+1) |= (byte)(val) << 1 )

/* Pack MessageSequence field for EMPA DOS */
#define HDRDOS_PACK_EMPADOS_MESSAGE_SEQUENCE( ptr, val )\
          ( *((byte*)(ptr)+2) |= (byte)(val) )

/**************************
 Constant data declaration
***************************/

/* Max Size of DOS message header in bytes */
#define HDRDOS_DOSMSG_HEADER_SIZE 3

/* DOS ACK timeout value in msec */
#define HDRDOS_DOS_ACK_TIMEOUT 1000

/* Number of DOS messages that can be queued to SLP simultanesouly */
#define HDRDOS_MAX_DOS_MSGS 3

/* Max size of overhead information in bytes for EMPA DOS payload which are 
 * DOS header (3 bytes), SLP header (2 bytes) and HMP header (1 byte) */
#define HDRDOS_EMPA_OVHDINFO_MAX 6

/* Max size of overhead information in bytes for MFPA DOS payload which are
 * DOS header (2 bytes), SLP header (2 bytes) and HMP header (1 byte) */
#define HDRDOS_MFPA_OVHDINFO_MAX 5

/* Max size of overhead information in bytes for MMPA DOS payload which are 
 * DOS header (3 bytes), SLP header (2 bytes) and HMP header (1 byte) */
#define HDRDOS_MMPA_OVHDINFO_MAX 6

/* Max bytes for route update message that will be sent along with DOS    
 * message. To fit maximum possible DOS message completely in a given 
 * probe, we include route update with only 6 pilot report (which is
 * a typical size found in the field test) in case of off-frequency 
 * pilots it will be 3 */
#define HDRDOS_ROUTEUPDATE_MSGSIZE_MAX 26

/* Max bytes for connection request message that will be sent along with
 * DOS message.*/
#define HDRDOS_CONNREQUEST_MSGSIZE_MAX 6

#define HDRDOS_BITS_PER_BYTE 8

#define HDRDOS_SDB_HEADER_SIZE 2

/*---------------------------------------------------------------------------
                       DATA OVER SIGNALING ACK MESSAGE                      
---------------------------------------------------------------------------*/

/* DOS ACK Message priority. */
#define HDRDOS_DATA_OVER_SIGNALING_ACK_MSG_PRI   40

/* Field sizes in DOS ACK message */
#define HDRDOS_DOS_ACK_MSG_MSG_ID_SIZE 8
#define HDRDOS_DOS_ACK_MSG_RESERVED_1_BIT_SIZE 1
#define HDRDOS_DOS_ACK_MSG_RESERVED_2_BITS_SIZE 2
#define HDRDOS_DOS_ACK_MSG_ROUTE_SIZE 1

/*************************************
 DOS Ack message header unpacking macros 
*************************************/

/* Get Route field in ACK. */
#define HDRDOS_GET_ACK_ROUTE( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 1 )

/* Get AckSequence field. */
#define HDRDOS_GET_ACK_SEQUENCE( dsm_ptr, val_ptr, offset )\
          HDRBIT_UNPACK8( dsm_ptr, val_ptr, offset, 6 )

/*************************************
 DOS Ack message header packing macros 
*************************************/

/* Pack Route field */
#define HDRDOS_PACK_ACK_ROUTE( dsm_ptr, val, offset ) \
               HDRBIT_PACK8( dsm_ptr, val, offset, 1 )

/* Pack Reserved field in ACK */
#define HDRDOS_PACK_RESERVED( dsm_ptr, val, offset, len ) \
               HDRBIT_PACK8( dsm_ptr, val, offset, len );

/* Pack AckSequence field */
#define HDRDOS_PACK_ACK_SEQUENCE( dsm_ptr, val, offset ) \
               HDRBIT_PACK8( dsm_ptr, val, offset, 6 )

/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* This structure is used to hold the fields of unpacked DOS message */
typedef struct 
{
  uint8 link_flow_number;
  hdrdos_route_enum_type route;
  boolean ack_required;
  boolean reset;
  uint8 msg_seq;

}hdrdos_unpacked_dos_msg;

/* This structure is used to hold the fields of unpacked DOS ACK message */
typedef struct 
{
  hdrdos_route_enum_type route;
  uint8 msg_seq;

}hdrdos_unpacked_dos_ack_msg;

/* This structure is used to keep track and compute timeout value
   for DOS messages with pending DOS ACK */
typedef struct 
{
  uint8 msg_seqnum;                 
    /* Sequence number of the DOS message */

  hdrdos_route_enum_type route;     
  /* Route instance on which the DOS message was sent */

  uint32 timeout_delta;              
  /* Time remaining on the DOS Timer when message was received */
}hdrdos_timer_value_type;

typedef struct
{
  uint8 msg_seq;                 
    /* Sequence number of the DOS message */

  hdrdos_route_enum_type route;     
    /* Route instance on which the DOS message was sent */

}hdrdos_next_msg_requiring_ack_type;

/* This structure is used to keep track of DOS msgs
   for which L3 ACK is recieved but not L2 ACK */
typedef struct 
{
  uint8 msg_seqnum;                 
    /* Sequence number of the DOS message */

  hdrdos_route_enum_type route;     
  /* Route instance on which the DOS message was sent */

}hdrdos_rcvd_dos_ack_type;
/*--------------------------------------------------------------
 Control block of the Data Over Signaling Protocol 
--------------------------------------------------------------*/

/* This stucture holds all the global data for DOS protocol */
typedef struct
{
  hdrdos_config_struct_type  session_data;
    /* DOS protocol Session information */

  rex_timer_type dos_ack_timer;
    /* Common timer for all DOS messages with outstanding DOS ACK */

  hdrdos_next_msg_requiring_ack_type next_msg_req_ack;
   /* DOS message for which AT is expecting a DOS ACK next */
   
  hdrdos_timer_value_type timer[HDRDOS_MAX_DOS_MSGS];
    /* Structure that holds information on DOS message with pending DOS ACK */

  uint8 num_unacked_msgs;
    /* Number of DOS message with pending L2/L3 ACK */
	
  hdrdos_rcvd_dos_ack_type rcvd_dos_ack[HDRDOS_MAX_DOS_MSGS + 1];
    /* Structure that holds information on DOS message for which L3 ACK has arrived but no L2 ACK */
	
  uint8 num_dos_acked; 
   /* Number of DOS messages in the structure rcvd_dos_ack */
   
  rex_crit_sect_type crit_sect;
    /* critical section for dos timer handler */
}hdrdos_info_type;

static hdrdos_info_type hdrdos;

typedef enum
{
  HDRDOS_ACK_TIMER
} hdrdos_timer_name_enum_type;

/* <EJECT> */
/*=============================================================================

                       LOCAL FUNCTION DECLARATIONS

=============================================================================*/

LOCAL void hdrdos_timer_cb
(
  uint32 timer_id
);

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRDOS_POWERUP_INIT

DESCRIPTION
  This function initializes the DOS protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_powerup_init ( void )
{
  uint8 index;                        /* loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrscmdb_get_inuse_config(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL,
                            &hdrdos.session_data);

  hdrdos.num_unacked_msgs = 0;
  hdrdos.num_dos_acked = 0;
  hdrdos.next_msg_req_ack.route = HDRDOS_ROUTE_A;
  hdrdos.next_msg_req_ack.msg_seq = 0;

  for( index = 0; index < HDRDOS_MAX_DOS_MSGS; index++)
  {
    hdrdos.timer[index].timeout_delta = 0;
    hdrdos.timer[index].msg_seqnum = 0;
	hdrdos.rcvd_dos_ack[index].msg_seqnum = 0;
  }
  hdrdos.rcvd_dos_ack[index].msg_seqnum = 0;
  if ( hdrmc_is_first_init() )
  {
    /* Define the L3 ACK timer */
    rex_def_timer_ex( &hdrdos.dos_ack_timer,
                      hdrdos_timer_cb,
                      HDRDOS_ACK_TIMER);
  }
  /*---------------------------------------------------------------------------
     Initialize critical section
  ---------------------------------------------------------------------------*/
  rex_init_crit_sect(&hdrdos.crit_sect);
  
}

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PACK_DOS_ACK_MSG

DESCRIPTION
  This function packs the DOS ACK message fields
  
DEPENDENCIES
  None

PARAMETERS
  msg_ptr     - A pointer to the DSM item.
  app_subtype - AT 's current negotiated SN packet application.                 
  dos_ack_msg - Value of the fields in the DOS ACK message.
  
RETURN VALUE
  Size in bytes of DOS message header packed.
   
SIDE EFFECTS
  None
===========================================================================*/
static void hdrdos_pack_dos_ack_msg
(
  dsm_item_type *msg_ptr,
  hdrhai_app_subtype_enum_type app_subtype,
  hdrdos_unpacked_dos_ack_msg dos_ack_msg
)
{
  uint16 offset = 0;
    /* Offset at which to pack the field in the DOS header */
  uint8 reserved_field = 0;
    /* Reserved field value is always 0 */
/*---------------------------------------------------------------------------*/

  /* Pack the DOS message ID */
  HDRDOS_PACK_MESSAGE_ID( msg_ptr, HDRDOS_DATA_OVER_SIGNALING_ACK_MSG_ID );
  offset += HDRDOS_DOS_MSG_ID_SIZE;

  /* Based on whether it is MPA or EMPA pack the rest of the fields 
     in the header */
  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    HDRDOS_PACK_RESERVED( msg_ptr, reserved_field, offset,
                          HDRDOS_DOS_ACK_MSG_RESERVED_2_BITS_SIZE );
    offset += HDRDOS_DOS_ACK_MSG_RESERVED_2_BITS_SIZE;

    HDRDOS_PACK_ACK_SEQUENCE( msg_ptr, dos_ack_msg.msg_seq, offset );
  }
  else if ((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
           (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    HDRDOS_PACK_ACK_ROUTE( msg_ptr, dos_ack_msg.route, offset ); 
    offset += HDRDOS_DOS_ACK_MSG_ROUTE_SIZE;

    HDRDOS_PACK_ACK_SEQUENCE( msg_ptr, dos_ack_msg.msg_seq, offset );
    offset += HDRDOS_DOS_MSG_MSG_SEQUENCE_SIZE;

    HDRDOS_PACK_RESERVED( msg_ptr, reserved_field, offset,
                          HDRDOS_DOS_ACK_MSG_RESERVED_1_BIT_SIZE ); 
  }
} /* hdrdos_pack_dos_ack_msg() */

/* EJECT */
/*===========================================================================
FUNCTION HDRDOS_SEND_DOS_ACK_MSG

DESCRIPTION
  This function sends a DataOverSignalingAck message.

DEPENDENCIES
  None

PARAMETERS
  app_subtype - AT 's current negotiated SN packet application.                 
  message_sequence - MessageSequence number of the DOS message received for
                     which ACK need to be sent.
  route        - route associated with the DOS Ack message.                   
  stream_id    - stream on which the DOS ack has to be sent.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void hdrdos_send_dos_ack_msg
( 
  hdrhai_app_subtype_enum_type app_subtype,
  uint8 message_sequence,
  hdrdos_route_enum_type route,
  hdrhai_stream_enum_type stream_id
)
{
  dsm_item_type *msg_ptr;                 
    /* DSM buffer for message to send */
  hdrdos_unpacked_dos_ack_msg dos_ack_msg;     
    /* Fields in DOS Ack message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get new buffer */
  msg_ptr = hdrutil_new_msg_buffer();

  dos_ack_msg.msg_seq = message_sequence;
  dos_ack_msg.route = route;

  hdrdos_pack_dos_ack_msg(msg_ptr, app_subtype, dos_ack_msg);

  hdrhmp_app_send_msg( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                       stream_id,
                       HDRHAI_AC_OR_RTC,
                       FALSE,
                       HDRDOS_DATA_OVER_SIGNALING_ACK_MSG_PRI,
                       HDRHAI_IN_USE, /* Instance of protocol sending msg */
                       msg_ptr,
                       NULL,
                       NULL );

} /* hdrdos_send_dos_ack_msg() */

/* EJECT */
/*===========================================================================
FUNCTION HDRDOS_SEND_DOS_MSG

DESCRIPTION
  This function commands DOS protocol to send DOS message.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - A pointer to the DSM item containing the HigherLayerPacket
              to be sent as part of the DataOverSignaling message.
  link_flow_number  - The link with which this DataOverSignaling message is 
              associated.
  route     - Route associated with this DOS message.            
  ack_req   - TRUE if an ACK is required, FALSE otherwise 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrdos_send_dos_msg
(
  dsm_item_type * msg_ptr,
  uint8 link_flow_number,
  hdrdos_route_enum_type route,
  boolean ack_req
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdrdos_cmd_type hdrdos_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdrdos_send_dos_msg");

  /* Fill in all members of the hdrdos_cmd_type for this command */
  hdrdos_cmd.name = HDRDOS_SEND_DOS_MSG_CMD;
  hdrdos_cmd.sender = HDRHAI_HDRMC;

  hdrdos_cmd.params.dos_msg.msg_ptr = msg_ptr;
  hdrdos_cmd.params.dos_msg.link_flow_number = link_flow_number;
  hdrdos_cmd.params.dos_msg.route = route;
  hdrdos_cmd.params.dos_msg.ack_req = ack_req;

  if ( rex_self(  ) == HDRDOS_TASK_PTR )
  {
    hdrdos_process_cmd( &hdrdos_cmd );
  }
  else
  {
    ERR( "Currently no other task ever asks DOS to send dos message !",
          0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, &hdrdos_cmd,
                     sizeof( hdrdos_cmd_type ) );
  }
}

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_SEND_RPT_TO_CM

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

PARAMETERS

  cmd_ptr - pointer to command to send to cm

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrdos_send_rpt_to_cm
(
  cm_mc_rpt_type *cmd_ptr
  /* Pointer to command to send to cm */
)
{

  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1( MSG_LEGACY_LOW, "DOS: Send CM rpt %d",cmd_ptr->hdr.cmd );
  cm_mc_rpt(cmd_ptr);

} /* hdrdos_send_rpt_to_cm() */

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_FORWARD_MT_DOS_DATA

DESCRIPTION
  This function sends the data to the CM task for a mobile terminated DOS

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr   - A pointer to the DSM item containing the HigherLayerPacket
              to be sent to CM for a mobile terminated DOS.

RETURN VALUE
  TRUE if there is a free CM command buffer to send MT DOS payload
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrdos_forward_mt_dos_data
(
  dsm_item_type * msg_ptr,
  uint8 link_flow_number,
  hdrdos_route_enum_type route
)
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */
  boolean  free_packet= FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dsm_length_packet(msg_ptr) == 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "MT DOS message has no payload" );

    /* dsm item has been released when the DOS header is removed, if DOS
     * msg does not have payload. */
    return free_packet;
  }

  /* Queue the message on CM's report queue */
  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->mt_dbm_data.hdr.cmd = CM_MT_DBM_DATA_F;
    cmd_ptr->mt_dbm_data.burst_type = CAI_SHORT_DATA_BURST;
    cmd_ptr->mt_dbm_data.burst_source = CM_DBM_FROM_HDR;
    cmd_ptr->mt_dbm_data.rlp_flow = link_flow_number;
    cmd_ptr->mt_dbm_data.route = route;
    cmd_ptr->mt_dbm_data.dos_payload = (dsm_item_type *) msg_ptr;

    cmd_ptr->mt_dbm_data.num_bytes = dsm_length_packet(msg_ptr);

    hdrdos_send_rpt_to_cm( cmd_ptr );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "DOS: No free buffers on cm_mc_rpt_free_q" );
    free_packet = TRUE;
  }

  return free_packet;
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_SEND_STATUS_TO_CM

DESCRIPTION
  This function sends status regarding DOS messages to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  msg_seq  - Sequence number of the DOS message
  route    - Route instance on which the DOS message was sent
  err_code - Error code describing the status of MO DOS message to CM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrdos_send_status_to_cm
(
  uint8 msg_seq,
  hdrdos_route_enum_type route,
  cm_dbm_mo_status_e_type err_code
  /* Mobile originated DOS message error code */
)
{
  cm_mc_rpt_type *cmd_ptr;
  /* Pointer to CM command buffer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e{571} suppress suspicious cast */
  HDRTRACE_DOS_MO_DOS_STATUS(msg_seq, route, err_code);

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->mo_dbm_status.hdr.cmd = CM_MO_DBM_STATUS_F;
    cmd_ptr->mo_dbm_status.msg_type = CAI_SHORT_DATA_BURST;
    cmd_ptr->mo_dbm_status.status = err_code;
  /*cmd_ptr->mo_dbm_status.burst_overflow_size = 0;*/

    hdrdos_send_rpt_to_cm( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrdos_send_status_to_cm() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRDOS_TIMER_CB

DESCRIPTION
  This function is the callback for all DOS state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrdos_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, timer_id);
} /* hdrdos_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_TIMER_HANDLER

DESCRIPTION
  This function handles starting and stoping of L3 ACK timer for DOS messages
  queued up for tx. Special handling is needed because single timer is used
  for multiple simultaneously queued up DOS messages.

  Calculation of the new timeout values and timeout delta ( the time remaining
  on the timer when the message was received ) are done using the return value
  from rex_get_timer(), which returns the time remaining on the timer or zero.

  Timer always runs for first message on the timer array. Timeout delta for 
  remaining message(s) in the list are always computed w.r.t first message.
 
  1) Computing the new timeout value for DOSMSG2
                                                  (B)
                                               |-------|
                                    (A)
                       |-------------------------------|
                                 T = HDRDOS_DOS_ACK_TIMEOUT
                       |-----------------------=====================|                    
                   T = HDRDOS_DOS_ACK_TIMEOUT
  |       |--------------------------------------------|                    |
  |-------|------------|-----------|-----------|----------------------------|
  |    DOSMSG1      DOSMSG2     DOSMSG3     DOSACK1                         |


  New time out value for DOSMSG2 is HDRDOS_DOS_ACK_TIMEOUT - the time elapsed 
  between receiving the message and now ( i.e we receive L3 ACK for DOSMSG1 or 
  Timer expires for it ) 

  A - time remaining on the timer when DOSMSG2 was received 
  B - time remaining on the timer when DOSACK1 was received
                                  
  Thicker line is the new timeout value for DOSMSG2.
  From the figure above it will be = T - (A - B)

  2) When DOSACK1 is received DOSMSG1 is removed from the list and we recompute
  the timeout delta w.r.t to first message (DOSMSG2) in the list for remaining 
  messages ( DOSMSG3 )
 
                                            (C)
                                   |-------------------|
                                    (A)
                       |-------------------------------|
                               T = HDRDOS_DOS_ACK_TIMEOUT
                       |------------================================| 
                   T = HDRDOS_DOS_ACK_TIMEOUT
  |       |--------------------------------------------|                    |
  |-------|------------|-----------|-----------|----------------------------|
  |    DOSMSG1      DOSMSG2     DOSMSG3     DOSACK1                         |

  A - time remaining on the timer when DOSMSG2 was received 
  C - time remaining on the timer when DOSMSG3 was received

  Thicker line is new timeout delta for DOSMSG3.
  New timeout delta for DOSMSG3 will be = T - (A - C)

  3) When DOSACK is received out of sequence say DOSACK2 is received before 
  DOSACK1, DOSMSG2 entry is deleted from the list. This just decreases the 
  number of outstanding messages and does not alter the state for other 
  messages.

DEPENDENCIES
  None

PARAMETERS
  seq_no - Sequence number of the new DOS message or of an earlier DOS message
           thats being ACK'ed 

  event  - DOS protocol event that occurred.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_timer_handler
(
  hdrdos_route_enum_type route,
  uint8 seq_no,
  hdrdos_dos_event_type event
)
{
  uint8 i = 0;     /* Loop index */
  uint8 j = 0;     /* Loop index */
  uint8 index = 0; /* Loop index */
  uint32 new_timeout_ms = 0; 
    /* New timeout value to be computed for the DOS message */
  boolean recvd_ack_in_seq = FALSE; 
    /* Flag to identify if ACK was received in sequence */
  boolean msg_matched = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT ( hdrdos.num_unacked_msgs <= HDRDOS_MAX_DOS_MSGS );
  
  rex_enter_crit_sect(&hdrdos.crit_sect);  
  if ( event == HDRDOS_RECEIVED_DOS_ACK || event == HDRDOS_DOS_TIMER_EXPIRED ) 
  /* Delete the message from the array and if needed start timer again for 
   * next message in the timer array */
  {
    for ( index = 0; index < hdrdos.num_unacked_msgs; index++ ) 
    {
     /* Find the message sequence in the timer array */
      if ((seq_no == hdrdos.timer[index].msg_seqnum)  && 
          (route == hdrdos.timer[index].route))
      {	     
        /* If this ACK is for first item on the list, then ACK is received in 
         * sequence */
        if ( index == 0 )
        {
          recvd_ack_in_seq = TRUE;
        }
       /* Delete entry corresponding to the message from the array */
        for ( j = index; j < (hdrdos.num_unacked_msgs - 1); j++ )
        {
          hdrdos.timer[j] = hdrdos.timer[j+1];
        }

       /* Clean up the last element in the timer array */
        hdrdos.timer[hdrdos.num_unacked_msgs - 1].timeout_delta = 0;
        hdrdos.timer[hdrdos.num_unacked_msgs - 1].msg_seqnum = 0;
        hdrdos.timer[hdrdos.num_unacked_msgs - 1].route = HDRDOS_ROUTE_A;

       /* Decrement the outstanding message count */
        hdrdos.num_unacked_msgs--;
	    msg_matched = TRUE;
        break;
      }
    }
    if( msg_matched == FALSE)
	{
	   /* if there is no match found in the unacked msgs, then DOS L3 ACK has arrived before L2 ACK */
	   if( hdrdos.num_dos_acked <= HDRDOS_MAX_DOS_MSGS)
	   {
                   /* store the msg detail in rcvd_dos_ack */
		   hdrdos.rcvd_dos_ack[hdrdos.num_dos_acked].msg_seqnum = seq_no;
		   hdrdos.rcvd_dos_ack[hdrdos.num_dos_acked].route = route;
		   hdrdos.num_dos_acked++;
	   }
	   else
	   {
	       HDR_MSG_PROT( MSG_LEGACY_ERROR, 
		                "num_dos_acked already maximum. This should not be happening unless AN did not AC Ack. Clear all" );
		   for(j = 0; j < HDRDOS_MAX_DOS_MSGS + 1; j++)
		   {
		      hdrdos.rcvd_dos_ack[j].msg_seqnum = 0;
			  hdrdos.rcvd_dos_ack[j].route = 0;
		   }
		   hdrdos.num_dos_acked = 0;
	   }	  
	}/* if(msg_matched == FALSE) */

    if ( recvd_ack_in_seq )
   /* We received ACK in sequence or DOS timer expired ( DOS timer always
    * expires in sequence ) */ 
    {                 
      if ( hdrdos.num_unacked_msgs > 0 )
     /* If there are pending messages in the timer array recompute the timeout
      * delta for the messages with respect to first one in the array and start
      * the timer for first message */
      {
        for ( j = 1; j < hdrdos.num_unacked_msgs; j++ )
        {
         /* Compute the timeout delta ( which is the time remaining on the timer
          * when the message was received ) for each pending message w.r.t first
          * message in the list */
          hdrdos.timer[j].timeout_delta = HDRDOS_DOS_ACK_TIMEOUT 
                                          - ( hdrdos.timer[0].timeout_delta 
                                              - hdrdos.timer[j].timeout_delta );
        }

       /* New timeout value is HDRDOS_DOS_ACK_TIMEOUT - ( time elapsed between
        * when the message was received and now ) */
        new_timeout_ms = HDRDOS_DOS_ACK_TIMEOUT 
                         - ( hdrdos.timer[0].timeout_delta 
                             - rex_get_timer( &hdrdos.dos_ack_timer ) );

        (void) rex_set_timer( &hdrdos.dos_ack_timer, new_timeout_ms );

       /* Timeout delta for the first message on the array is no longer needed
        * for clarity and uniformity set this value to zero */
        hdrdos.timer[0].timeout_delta = 0;
      }
      else
      /* There are no pending messages clear the timer */
      {
        (void) rex_clr_timer( &hdrdos.dos_ack_timer );
      }
    }
    else
    {
      /* If ACK is not received in sequence just delete the entry from the 
       * timer array and allow the current timer to run */
    }
  }
  else  /* HDRDOS_RECEIVED_NEW_DOSMSG */
 /* Add to the timer array and start DOS ACK timer if it's not running */
  {
   /* Check we are not queuing more than simultaneouly allowed DOS messages */
    ASSERT( hdrdos.num_unacked_msgs < HDRDOS_MAX_DOS_MSGS );
	
   /* Check if DOS L3 ACK has already been recieved. If yes, ignore L2 ACK
      as there is no processing needs to be done   */
	for(i = 0; i < hdrdos.num_dos_acked; i++)
	{
	  if ((seq_no == hdrdos.rcvd_dos_ack[i].msg_seqnum)  && 
          (route == hdrdos.rcvd_dos_ack[i].route))
	  {
        for(j = i; j < hdrdos.num_dos_acked-1; j++)
        {
		    hdrdos.rcvd_dos_ack[j] = hdrdos.rcvd_dos_ack[j+1];
        }
                hdrdos.rcvd_dos_ack[j].msg_seqnum = 0;
                hdrdos.rcvd_dos_ack[j].route = 0;
		hdrdos.num_dos_acked--;
		rex_leave_crit_sect(&hdrdos.crit_sect);  
		return;
	  }
	}
    
    hdrdos.timer[hdrdos.num_unacked_msgs].timeout_delta = 
                                          rex_get_timer( &hdrdos.dos_ack_timer );
    hdrdos.timer[hdrdos.num_unacked_msgs].msg_seqnum = seq_no;

    hdrdos.timer[hdrdos.num_unacked_msgs].route = route;

    hdrdos.num_unacked_msgs++;

    if ( rex_get_timer( &hdrdos.dos_ack_timer ) == 0 )
    {
      (void) rex_set_timer( &hdrdos.dos_ack_timer, HDRDOS_DOS_ACK_TIMEOUT );
    }
    else
    {
      /* Timer is already running for previously queued message wait until we
       * get an ACK or timer to expire to start the timer for next message */
    }
  }

  /* First message on the timer array is the one for which timer is currently
   * running and DOS ACK is next expected if there are no more pending message
   * this value is set to zero */
  hdrdos.next_msg_req_ack.msg_seq = hdrdos.timer[0].msg_seqnum;
  hdrdos.next_msg_req_ack.route = hdrdos.timer[0].route;
  
  rex_leave_crit_sect(&hdrdos.crit_sect);  

} /* hdrdos_timer_handler() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_DOS_TX_OUTCOME_CB

DESCRIPTION
  Callback function for transmission outcome of the DOS message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data is the DOS message sequence number
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_dos_tx_outcome_cb
(
  void                 *outcome_data_ptr,
  hdrerrno_enum_type    status
)
{
  uint8 msg_sequence = 0;
    /* Dos message sequence for which this outcome_cb is called */
  uint8 route = 0;
    /* Route associated with the message sequence */
/* ------------------------------------------------------------------------ */

  /* Extract the DOS message route for which this outcome callback is for */
  route =  (((uint8)((dword)outcome_data_ptr) & HDRDOS_DOSMSG_ROUTE_MASK) >> 8);

  /* Extract the DOS message sequence for which this outcome 
    callback is for */
  msg_sequence = ((uint8)((dword)outcome_data_ptr) & HDRDOS_DOSMSG_MSGSEQ_MASK);

  /* Report DOS message send status to CM */
  switch(status)
  {
    case E_SUCCESS: 
      /* DOS message transmission was successful start the DOS ACK timer and
         wait for DOS ACK to report success or failure to higher layer */
      hdrdos_timer_handler( (hdrdos_route_enum_type) route, 
                            msg_sequence, HDRDOS_RECEIVED_NEW_DOSMSG ); 
      break;

    case E_DATA_TOO_LARGE:
        /* If the max allowed DOS msg size check in hdrdos_process_send_dos_msg() 
           succeded however if ACMAC could not fit the message in the first probe 
           completely the message is canceled and failure is returned to app */
        hdrdos_send_status_to_cm( msg_sequence,
                                  (hdrdos_route_enum_type) route,
                                  CM_DBM_MO_ACCESS_TOO_LARGE );
        break;

    case E_NOT_ALLOWED:
    case E_CANCELED:
      /* DOS messages are not allowed when HDR call is up or if SLP cancels
         the AC messages because AT goes to traffic before it get chance to
         send DoS message in next AC capsule */
      hdrdos_send_status_to_cm( msg_sequence,
                                (hdrdos_route_enum_type) route,
                                CM_DBM_MO_NOT_ALLOWED_IN_HDR );
      break;
     
    case E_FAILURE:    
    default: 
      /* Message transmission failed could be SLP ACK failure or max access 
         probe or SLP Q is full */ 
      hdrdos_send_status_to_cm( msg_sequence,
                                (hdrdos_route_enum_type) route,
                                CM_DBM_MO_L2_ACK_FAILURE );
      break;
  }
} /* hdrdos_dos_tx_outcome_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_msg_cb 
( 
  dsm_item_type *msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                   HDRHAI_IN_USE, 
                   msg_ptr ); 

} /* hdrdos_msg_cb() */

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PACK_DOS_MSG

DESCRIPTION
  This function packs the DOS message fields
  
DEPENDENCIES
  None

PARAMETERS
  hdrdos_pkt_hdr - Buffer passed in by the calling routine where the DOS 
                   message fields are packed.
  app_subtype    - AT current negotiated SN packet application.                 
  mo_dos_msg     - Value of the fields in the DOS message.
  
RETURN VALUE
  Size in bytes of DOS message header packed.
   
SIDE EFFECTS
  None
===========================================================================*/
uint16 hdrdos_pack_dos_msg
(
  byte *hdrdos_pkt_hdr,
  hdrhai_app_subtype_enum_type app_subtype,
  hdrdos_unpacked_dos_msg mo_dos_msg
)
{
  uint8 dos_hdr_size = 0;
    /* Packed DOS message header size */
/*---------------------------------------------------------------------------*/

  /* Clear all the fields in DOS message before setting it */
  HDRDOS_CLR_DOS_HDR( hdrdos_pkt_hdr );

  /* Pack the DOS message ID */
  HDRDOS_PACK_DOS_MSGID( hdrdos_pkt_hdr, HDRDOS_DATA_OVER_SIGNALING_MSG_ID );

  /* Based on whether it is MPA or EMPA pack the rest of the fields 
     in the header */
  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    HDRDOS_PACK_ACK_REQUIRED( hdrdos_pkt_hdr, mo_dos_msg.ack_required );
    HDRDOS_PACK_MESSAGE_SEQUENCE( hdrdos_pkt_hdr, mo_dos_msg.msg_seq );
    dos_hdr_size = 2;
  }
  else if ((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
           (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    HDRDOS_PACK_EMPADOS_LINK_FLOW_NUMBER( hdrdos_pkt_hdr, 
                                          mo_dos_msg.link_flow_number );
    HDRDOS_PACK_EMPADOS_ROUTE( hdrdos_pkt_hdr, mo_dos_msg.route);
    HDRDOS_PACK_EMPADOS_ACK_REQUIRED( hdrdos_pkt_hdr, mo_dos_msg.ack_required );
    HDRDOS_PACK_EMPADOS_MESSAGE_SEQUENCE( hdrdos_pkt_hdr, mo_dos_msg.msg_seq );
    dos_hdr_size = 3;
  }

  return dos_hdr_size;
} /* hdrdos_pack_dos_msg() */

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_SEND_DOS_MSG

DESCRIPTION
  This function runs in the context of the HDRMC task.  It builds a Data
  Over Signaling message and queues it to HMP for transmit.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - A pointer to the DSM item containing the HigherLayerPacket
              to be sent as part of the DataOverSignaling message.
  link_flow_number  - The link with which this DataOverSignaling message is 
              associated.
  route     - Route associated with this DOS message.            
  ack_req   - TRUE if an ACK is required, FALSE otherwise 
            

RETURN VALUE
  TRUE if we are able to queue the message to HMP, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrdos_process_send_dos_msg
(
  dsm_item_type * msg_ptr,
  uint8 link_flow_number,
  hdrdos_route_enum_type route,
  boolean ack_req
)
{
  hdrhai_stream_enum_type stream_id = HDRHAI_STREAM1; 
    /* Stream to send DOS message on */
  boolean free_buffer = FALSE;  
    /* Whether or not we need to free msg_ptr */
  byte hdrdos_pkt_hdr[HDRDOS_DOSMSG_HEADER_SIZE];
    /* Temp buffer for creating the DOS message header. */
  uint8 capsule_length_max = hdrovhd_get_last_capsule_length_max();
    /* Most recently received capsule length max value in AP message */
  uint32 max_payload_size = 0;
    /* Max allowed payload bytes in ACCapsule */
  uint32 max_bytes = 0;               
    /* Max allowed bytes for DOS message */
  uint32 payload_size = 0;         
    /* Size of DOS message payload in bytes */
  hdrdos_unpacked_dos_msg mo_dos_msg;
    /* This structure is filled and passed to the packing routine */
  uint16 mo_dos_meta_data = 0;
    /* This meta data is used to match the SLP Ack with the particular DOS msg */
  hdrhai_app_subtype_enum_type app_subtype;
    /* Current SN packet subtype */
  uint8 dosmsg_hdr_size = 0;
    /* Size in bytes of DoS message header */
  uint8 dos_ovhdinfo_max = 0;
  byte sdb_hdr[HDRDOS_SDB_HEADER_SIZE];
    /* Temporary storage for SDB header */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Ensure that MPA or EMPA is negotiated and bound */
  if (hdrstream_get_negotiated_stream(
        HDRHAI_MULTIFLOW_PACKET_APP_SN, 
        &stream_id))
  {
    app_subtype = HDRHAI_MULTIFLOW_PACKET_APP_SN;
    dos_ovhdinfo_max = HDRDOS_MFPA_OVHDINFO_MAX;
  }
  else if (hdrstream_get_negotiated_stream(
             HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, 
             &stream_id))
  {
    app_subtype = HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN;
    dos_ovhdinfo_max = HDRDOS_EMPA_OVHDINFO_MAX;
  }
  else if (hdrstream_get_negotiated_stream(
             HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
             &stream_id))
  {
    app_subtype = HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN;
    dos_ovhdinfo_max = HDRDOS_MMPA_OVHDINFO_MAX;
  }
  else
  {
    ERR("Could not get stream for MRLP. Not q-ing any msgs",0,0,0);
    hdrdos_send_status_to_cm( 0, HDRDOS_ROUTE_A, CM_DBM_MO_NO_SRV );
    free_buffer = TRUE;

    return free_buffer;
  }

  /* Check that DOS message payload is not zero */
  if( dsm_length_packet(msg_ptr) == 0 )
  {
    ERR("MO DOS message has no payload",0,0,0 );
    free_buffer = TRUE;

    return free_buffer;
  }

  /* Strip off SDB header if we decided to send the DBM as DOS */
  (void) dsm_pullup( &msg_ptr, sdb_hdr, HDRDOS_SDB_HEADER_SIZE );

  if ( capsule_length_max >= 2 )
  {
   /* Constant 29 here is HDRAMAC_PKT_FRAME_DATA_LEN from hdramaci.h */
    max_payload_size = capsule_length_max * 29;

   /* Max allowed ACMAC payload is 260 bytes ( 1 byte len field, 255 bytes of 
    * payload which includes the last 7 bytes of the header, and 4 byte FCS )*/
    if ( capsule_length_max >= 9 )
    {
      /* There is room for at least 261 (9 * 29) bytes, so the packet size is 
       * limited by the Length field being only 8 bits */
      max_bytes = 255 - ( dos_ovhdinfo_max 
                        + HDRDOS_ROUTEUPDATE_MSGSIZE_MAX
                        + HDRDOS_CONNREQUEST_MSGSIZE_MAX
                        + 7 ); /* HDRAMAC_PKT_HDR_LEN = 8, 
                                  HDRAMAC_PKT_HDR_LEN_FIELD_LEN = 1 */
    }
    else
    {
      /* The packet is limited by the capsule_length_max */
      max_bytes = max_payload_size - ( dos_ovhdinfo_max 
                                     + HDRDOS_ROUTEUPDATE_MSGSIZE_MAX 
                                     + HDRDOS_CONNREQUEST_MSGSIZE_MAX
                                     + 8    /* HDRAMAC_PKT_HDR_LEN = 8 */
                                     + 4 ); /* HDRAMAC_PKT_FCS_LEN = 4 */
    }
  }

  /* Find DOS message payload size */
  payload_size = dsm_length_packet(msg_ptr);

  HDR_MSG_PROT_3( MSG_LEGACY_MED,
                  "capsule_length_max %d max_bytes %d DOS payload size %d", 
                   capsule_length_max, max_bytes, payload_size );

  /* Fail DOS message that do not fit completely in single access probe */
  if (payload_size > max_bytes)
  {
    ERR("DOS message too big to fit in access probe",0,0,0);
    hdrdos_send_status_to_cm( 0, HDRDOS_ROUTE_A, CM_DBM_MO_ACCESS_TOO_LARGE );
    free_buffer = TRUE;

    return free_buffer;
  } 

  memset(&mo_dos_msg, 0, sizeof(hdrdos_unpacked_dos_msg));

  /* Fill in the DOS message fields */
  mo_dos_msg.link_flow_number = link_flow_number;

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    /* If the current SN packet app is MPA then there is only one instance
       of route, so we use route A */
    mo_dos_msg.route = HDRDOS_ROUTE_A; 
  }
  else
  {
    ASSERT ((route == HDRDOS_ROUTE_A) || (route == HDRDOS_ROUTE_B));
    mo_dos_msg.route = route; 
  }

  /* DOS ACK is always expected from the AN, 1 second timer for DOS ACK 
   * is started after we get an L2 ACK success */
  mo_dos_msg.ack_required = ack_req;

  /* Increment DOS message sequence value */    
  if (mo_dos_msg.route == HDRDOS_ROUTE_A)
  {
    hdrdos.session_data.route_a.tx_msg_seq = 
        (hdrdos.session_data.route_a.tx_msg_seq + 1) % 64;
    mo_dos_msg.msg_seq = hdrdos.session_data.route_a.tx_msg_seq;
  }
  else /* mo_dos_msg.route = HDRDOS_ROUTE_B */
  {
    hdrdos.session_data.route_b.tx_msg_seq =
        (hdrdos.session_data.route_b.tx_msg_seq + 1) % 64;
    mo_dos_msg.msg_seq = hdrdos.session_data.route_b.tx_msg_seq;
  }

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "MO DOS (V(S) %d)", 
                    hdrdos.session_data.route_a.tx_msg_seq );
  }
  else if((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    if (mo_dos_msg.route == HDRDOS_ROUTE_A)
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "MO DOS (V(S) %d Route A)", 
                      hdrdos.session_data.route_a.tx_msg_seq );
    }
    else 
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "MO DOS (V(S) %d Route B)", 
                      hdrdos.session_data.route_b.tx_msg_seq );
    }
  }

  /* Pack the the header fields */
  dosmsg_hdr_size = hdrdos_pack_dos_msg(hdrdos_pkt_hdr, app_subtype, 
                                        mo_dos_msg);

  /* Insert the DOS header at the front of the DSM item */
  if (!(hdrutil_pushdown_head( &msg_ptr, 
                               hdrdos_pkt_hdr,
                               dosmsg_hdr_size )))
  {
    ERR_FATAL("DOS header Pushdown failed",0,0,0);
  }

  /* Set the meta data such that we could uniquely identify for which 
     DoS Message SLP ACK was received. The upper 8 bits represent the 
     route and the lower 8 bits the sequence number which is unique
     for a given route */
  mo_dos_meta_data = (mo_dos_msg.route << 8) | mo_dos_msg.msg_seq;

#ifdef FEATURE_HDR_QCHAT
  if (hdrqchatupk_mod_enabled())
  {  
    hdrhmp_app_send_msg( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                         stream_id,
                         HDRHAI_AC,
                         FALSE,
                         HDRDOS_DATA_OVER_SIGNALING_MSG_PRI,
                         HDRHAI_IN_USE, /* Instance of protocol sending msg */
                         msg_ptr,
                         hdrqchatupk_dos_tx_outcome_cb,
                         (void *) ((dword) mo_dos_meta_data) );
  }
  else
#endif /*FEATURE_HDR_QCHAT*/
  {
    hdrhmp_app_send_msg( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                       stream_id,
                       HDRHAI_AC,
                       FALSE,
                       HDRDOS_DATA_OVER_SIGNALING_MSG_PRI,
                       HDRHAI_IN_USE, /* Instance of protocol sending msg */
                       msg_ptr,
                       hdrdos_dos_tx_outcome_cb,
                       (void *) ((dword) mo_dos_meta_data) );

  }

  hdrscmdb_set_gauped_config(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                             &hdrdos.session_data);

  return free_buffer;

} /* hdrdos_process_send_dos_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_CMD

DESCRIPTION
  This function processes commands given to DOS.

DEPENDENCIES
  None

PARAMETERS
  dos_cmd_ptr - Pointer to the command for DOS to process  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrdos_process_cmd( hdrdos_cmd_type * dos_cmd_ptr )
{
  boolean free_buffer = FALSE;   /* Whether or not we need to free msg_ptr */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_CMD( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, dos_cmd_ptr->name,
                dos_cmd_ptr->sender, 0 );

  /*---------------------------------------------------------------------------
  Determine which command has been received.
  ---------------------------------------------------------------------------*/
  switch ( dos_cmd_ptr->name )
  {
    case HDRDOS_SEND_DOS_MSG_CMD:
      /*-----------------------------------------------------------------------
      Send DOS message command
      -----------------------------------------------------------------------*/
      if( hdrdos.num_unacked_msgs < HDRDOS_MAX_DOS_MSGS )
      {
        free_buffer = hdrdos_process_send_dos_msg( dos_cmd_ptr->params.dos_msg.msg_ptr, 
                                         dos_cmd_ptr->params.dos_msg.link_flow_number, 
                                         (hdrdos_route_enum_type) 
                                         dos_cmd_ptr->params.dos_msg.route,
                                         dos_cmd_ptr->params.dos_msg.ack_req );
      }
      else
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Num un-ACK'ed DOS msgs %d",
                        hdrdos.num_unacked_msgs );
        ERR("only 3 simultaneous DOS messages are allowed",0,0,0);
        hdrdos_send_status_to_cm( 0, HDRDOS_ROUTE_A, CM_DBM_MO_OTHER );
      }

      if( free_buffer )
      {
        (void) dsm_free_buffer(dos_cmd_ptr->params.dos_msg.msg_ptr);
      }
      break;

    default:
      /*-----------------------------------------------------------------------
      Unrecognized command.
      -----------------------------------------------------------------------*/
      ERR("DOS: Received Unrecognized command:%d", dos_cmd_ptr->name, 0, 0 );
      break;
  }
} /* hdrdos_process_cmd() */

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Data Over 
  Signalling protocol.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrdos_process_timer
(
  uint32 timer_id
)
{
  /*---------------------------------------------------------------------------
  Determine which DOS timer has expired.
  ---------------------------------------------------------------------------*/
  switch ( (hdrdos_timer_name_enum_type) timer_id )
  {
    case HDRDOS_ACK_TIMER:
      /* Send report to CM */
      hdrdos_timer_handler( hdrdos.next_msg_req_ack.route, 
                            hdrdos.next_msg_req_ack.msg_seq, 
                            HDRDOS_DOS_TIMER_EXPIRED );
      hdrdos_send_status_to_cm( hdrdos.next_msg_req_ack.msg_seq,
                                hdrdos.next_msg_req_ack.route,
                                CM_DBM_MO_L3_ACK_FAILURE );

      ERR("HDR DOS Timer expired",timer_id,0,0);

    break;

  default:
      /*-----------------------------------------------------------------------
    Unrecognized Timer:
      -----------------------------------------------------------------------*/
      ERR("Invalid Timer ID (%d)",timer_id,0,0);

    break;
  }
} /* hdrdos_process_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_UNPACK_DOS_MSG

DESCRIPTION
  This function unpacks the Data Over Signaling message received by the AT.
  
DEPENDENCIES
  None.

PARAMETERS
  app_subtype - Current SN packet subtype
  msg_ptr     - pointer to DSM item pointer which points received message buffer
  mt_dos_msg  - Structure into which the MT DOS message will be unpacked

RETURN VALUE
  Status of MT DOS message unpacking.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrdos_unpack_dos_msg
(
  hdrhai_app_subtype_enum_type app_subtype,
  dsm_item_type **msg_ptr,
  hdrdos_unpacked_dos_msg *mt_dos_msg 
)
{
  int err = 0;                   
    /* Bit unpack error value, E_SUCCESS = 0 */
  uint8 msg_hdr_size = 0;        
    /* Size of DOS message header that was unpacked */
  uint16 offset = HDRDOS_DOS_MSG_ID_SIZE;
    /* Bit Offset at which to start unpacking */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(mt_dos_msg, 0, sizeof(hdrdos_unpacked_dos_msg));

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    err |= HDRDOS_GET_ACK_REQUIRED( *msg_ptr, &mt_dos_msg->ack_required, 
                                    offset );
    offset += HDRDOS_DOS_MSG_ACK_REQUIRED_SIZE;

    err |= HDRDOS_GET_RESET( *msg_ptr, &mt_dos_msg->reset, offset );
    offset += HDRDOS_DOS_MSG_RESET_SIZE;

    err |= HDRDOS_GET_MESSAGE_SEQUENCE( *msg_ptr, &mt_dos_msg->msg_seq, 
                                        offset );
    offset += HDRDOS_DOS_MSG_MSG_SEQUENCE_SIZE;
  }
  else if ((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
           (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    err |= HDRDOS_GET_LINK_FLOW_NUMBER( *msg_ptr, &mt_dos_msg->link_flow_number,
                                        offset );
    offset += HDRDOS_DOS_MSG_LINK_FLOW_NUMBER_SIZE;

    err |= HDRDOS_GET_ROUTE( *msg_ptr, (uint8*)&mt_dos_msg->route, offset);
    offset += HDRDOS_DOS_MSG_ROUTE_SIZE;
    
    err |= HDRDOS_GET_ACK_REQUIRED( *msg_ptr, &mt_dos_msg->ack_required, 
                                    offset );
    offset += HDRDOS_DOS_MSG_ACK_REQUIRED_SIZE;

    err |= HDRDOS_GET_RESET( *msg_ptr, &mt_dos_msg->reset, offset );
    offset += HDRDOS_DOS_MSG_RESET_SIZE;

    /* Skip the reserved field */
    offset += HDRDOS_DOS_MSG_RESERVED_SIZE;

    err |= HDRDOS_GET_MESSAGE_SEQUENCE( *msg_ptr, &mt_dos_msg->msg_seq, 
                                        offset );
    offset += HDRDOS_DOS_MSG_MSG_SEQUENCE_SIZE;
  }

  msg_hdr_size = offset / HDRDOS_BITS_PER_BYTE;

  /* We strip of the MT DOS message header and pass the msg_ptr to 
   * DS as a data packet. In this case DS takes over responsibility 
   * for freeing the packet */
  if ((err != E_SUCCESS) || ( dsm_pullup( msg_ptr, NULL, msg_hdr_size ) == 0 ))
  {
    ERR( "DOS message not big enough!", 0, 0, 0 );
    err = E_FAILURE;
  }

  if ( (*msg_ptr) == NULL)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR,  "Empty DOS message");   
    err = E_DATA_INVALID;
  }

  return (hdrerrno_enum_type) err;

} /* hdrdos_unpack_dos_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_UNPACK_DOS_ACK_MSG

DESCRIPTION
  This function unpacks the Data Over Signaling ACK message received by
  the AT.
  
DEPENDENCIES
  None.

PARAMETERS
  app_subtype - Current SN packet subtype
  msg_ptr     - DSM item pointer to the received message buffer
  dos_ack_msg - Structure into which the MT DOS Ack message will be unpacked

RETURN VALUE
  Status of MT DOS Ack message unpacking.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrdos_unpack_dos_ack_msg
(
  hdrhai_app_subtype_enum_type app_subtype,
  dsm_item_type *msg_ptr,
  hdrdos_unpacked_dos_ack_msg *dos_ack_msg 
)
{
  uint16 offset = HDRDOS_DOS_ACK_MSG_MSG_ID_SIZE;
    /* Bit Offset at which to start unpacking */
  int err = 0;
    /* Bit unpack error value, E_SUCCESS = 0 */
/* --------------------------------------------------------------------------*/

  memset( dos_ack_msg, 0, sizeof(hdrdos_unpacked_dos_ack_msg) );

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    /* Skip the reserved field */
    offset += HDRDOS_DOS_ACK_MSG_RESERVED_2_BITS_SIZE;

    err |= HDRDOS_GET_ACK_SEQUENCE( msg_ptr, &dos_ack_msg->msg_seq, offset );
  }
  else if ((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
           (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    err |= HDRDOS_GET_ROUTE( msg_ptr, (uint8*)&dos_ack_msg->route, offset );
    offset += HDRDOS_DOS_ACK_MSG_ROUTE_SIZE;

    err |= HDRDOS_GET_ACK_SEQUENCE( msg_ptr, &dos_ack_msg->msg_seq, offset );
  }

  return (hdrerrno_enum_type) err;

} /* hdrdos_unpack_dos_ack_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_MT_DOS_MSG

DESCRIPTION
  This function processes the Data Over Signaling message received by the AT.
  
DEPENDENCIES
  None.

PARAMETERS
  app_subtype - Current SN packet subtype
  msg_ptr     - DSM item pointer to the received message buffer
  stream_id   - Stream on which the message was received

RETURN VALUE
  TRUE if there is a free CM command buffer to send MT DOS payload
  FALSE otherwise

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrdos_process_mt_dos_msg
(
  hdrhai_app_subtype_enum_type app_subtype,
  dsm_item_type *msg_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  hdrdos_unpacked_dos_msg mt_dos_msg;         
    /* Fields in MT DOS message */
  boolean free_packet = TRUE;
    /* Whether or not we need to free msg_ptr */
  uint8 last_msg_seq_received;
    /* Sequence number of the last DOS message received by the AT */
  hdrdos_route_enum_type msg_route = HDRDOS_ROUTE_A;
    /* Route instance for which the MT DOS message was received */
  hdrerrno_enum_type unpack_errno;
    /* Error number for unpacking DOS message */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  unpack_errno = hdrdos_unpack_dos_msg( app_subtype, &msg_ptr, &mt_dos_msg );
  if ( unpack_errno != E_SUCCESS )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,"Error while unpacking MT DOS message");

    if ( unpack_errno == E_DATA_INVALID )
    {
      /* msg_ptr has been free by dsm_pullup */
      free_packet = FALSE;
    }
    return free_packet;
  }

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    HDR_MSG_PROT_3(MSG_LEGACY_HIGH, "MT DOS rxed %d Reset= %d (V(R) %d)",
                   mt_dos_msg.msg_seq, mt_dos_msg.reset, 
                   hdrdos.session_data.route_a.rx_msg_seq);
  }
  else if((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    if (mt_dos_msg.route == HDRDOS_ROUTE_A)
    {
      HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                     "MT DOS rxed %d Route= A Reset= %d (V(R) %d)",
                     mt_dos_msg.msg_seq, mt_dos_msg.reset, 
                     hdrdos.session_data.route_a.rx_msg_seq);
    }
    else 
    {
      ASSERT(mt_dos_msg.route == HDRDOS_ROUTE_B);

      /* By default we set "msg_route" to A, but in case the app_subtype is
         EMPA and the route instance is B then set the msg_route to B, so 
         that it could be used later to deal with the appropriate message
         sequence instance */
      msg_route = HDRDOS_ROUTE_B;

      HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                     "MT DOS rxed %d Route= B Reset= %d (V(R) %d)",
                      mt_dos_msg.msg_seq, mt_dos_msg.reset, 
                      hdrdos.session_data.route_b.rx_msg_seq);
    }
  }

  /* RESET bit resets V(R) irrespective of message sequence being valid or
   * not. AN sends a RESET so that DOS message is always accepted by AT */
  if (mt_dos_msg.reset)
  {
    if (msg_route == HDRDOS_ROUTE_A)
    {
      hdrdos.session_data.route_a.rx_msg_seq = (mt_dos_msg.msg_seq+63)%64;
    }
    else
    {
      hdrdos.session_data.route_b.rx_msg_seq = (mt_dos_msg.msg_seq+63)%64;
    }
  }

  /* Send DOS ACK for duplicate DOS messages also. If AT receives the same 
   * DOS message it is possible AN did not receive last DOS ACK */
  if (mt_dos_msg.ack_required)
  {
    hdrdos_send_dos_ack_msg( app_subtype, 
                             mt_dos_msg.msg_seq, 
                             mt_dos_msg.route,
                             stream_id );
  }

  if (msg_route == HDRDOS_ROUTE_A)
  {
    last_msg_seq_received = hdrdos.session_data.route_a.rx_msg_seq;
  }
  else
  {
    last_msg_seq_received = hdrdos.session_data.route_b.rx_msg_seq;
  }

  /* Check the message sequence number in MT DOS is valid */
  if(
#ifdef FEATURE_HDR_QCHAT
    (hdrqchatupk_mcs_enabled() && hdrqchatupk_ignore_dos_seqno()) ||
#endif /*FEATURE_HDR_QCHAT*/
    ( hdrutil_msg_seq_num_is_greater( mt_dos_msg.msg_seq, 
                                      last_msg_seq_received,
                                      6) )
    ) 
  {
    /* Send MT DOS to CM only if its not NULL */
    free_packet = hdrdos_forward_mt_dos_data( msg_ptr, 
                                              mt_dos_msg.link_flow_number,
                                              mt_dos_msg.route );
#ifdef FEATURE_HDR_QCHAT
    if(!hdrqchatupk_ignore_dos_seqno())
    {
#endif /*FEATURE_HDR_QCHAT*/
    /* V(R) is always the sequence number in the last valid 
       DOS message received */
    if (msg_route == HDRDOS_ROUTE_A)
    {
      hdrdos.session_data.route_a.rx_msg_seq = mt_dos_msg.msg_seq ;
    }
    else
    {
      hdrdos.session_data.route_b.rx_msg_seq = mt_dos_msg.msg_seq;
    }

    /* Save the V(S) in EFS so that it could be restored on powercycle */
    hdrscmdb_set_gauped_config(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, 
                               &hdrdos.session_data);
#ifdef FEATURE_HDR_QCHAT
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_MED, "MATI pkt - Ignore  DOS seq no.");
    }
#endif /*FEATURE_HDR_QCHAT*/    
  }
  else
  {
    ERR("Unexpected DOS message sequence!",0,0,0);
  }

  return free_packet;
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_DOS_ACK_MSG

DESCRIPTION
  This function processes the received Data Over Signaling ACK message.
  
DEPENDENCIES
  None.

PARAMETERS
  app_subtype - Current SN packet subtype
  msg_ptr     - DSM item pointer to the received message buffer

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_process_dos_ack_msg
(
  hdrhai_app_subtype_enum_type app_subtype,
  dsm_item_type *msg_ptr
)
{
  hdrdos_unpacked_dos_ack_msg dos_ack_msg;   
   /* Fields of DOS Ack message */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrdos_unpack_dos_ack_msg(app_subtype, msg_ptr, &dos_ack_msg)
       != E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Error while unpacking DOS ACK message");
    return;
  }

  if (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "DOS ACK rxed %d (expected %d)", 
                    dos_ack_msg.msg_seq, hdrdos.next_msg_req_ack.msg_seq );
  }
  else if((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          (app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    HDR_MSG_PROT_4( MSG_LEGACY_HIGH, 
              "DOS ACK rxed %d Route= %d (expected %d Route= %d(0->A, 1->B)",
              dos_ack_msg.msg_seq, dos_ack_msg.route,
              hdrdos.next_msg_req_ack.msg_seq,
              hdrdos.next_msg_req_ack.route);
  }

  hdrdos_timer_handler( dos_ack_msg.route, dos_ack_msg.msg_seq, 
                        HDRDOS_RECEIVED_DOS_ACK );

  /* How will DS know which msg_seq this ACK is for ? */

  /* Message transmission successful */
  hdrdos_send_status_to_cm( dos_ack_msg.msg_seq, dos_ack_msg.route,
                            CM_DBM_MO_OK );

}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_MSG

DESCRIPTION
  This function processes incoming messages for the Data Over Signaling 
  protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr           - DSM item pointer to the received message buffer
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrdos_process_msg 
(
  dsm_item_type *msg_ptr,                  
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  hdrdos_msg_id_type  msg_id;                         
    /* Message ID of the received signaling message */
  hdrhai_app_subtype_enum_type app_subtype;  
    /* Current SN packet subtype */
  hdrhai_stream_enum_type stream_id; 
    /* Stream on which the message was received */
  boolean free_packet = TRUE;   
    /* Whether or not we need to free msg_ptr */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(protocol_instance == HDRHAI_IN_USE);

  /* Find if MPA or EMPA has been negotiated and bound */
  if (hdrstream_get_negotiated_stream(
        HDRHAI_MULTIFLOW_PACKET_APP_SN, &stream_id))
  {
    app_subtype = HDRHAI_MULTIFLOW_PACKET_APP_SN;
  }
  else if (hdrstream_get_negotiated_stream(
             HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, &stream_id))
  {
    app_subtype = HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN;
  }
  else if (hdrstream_get_negotiated_stream(
             HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN, &stream_id))
  {
    app_subtype = HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN;
  }
  else
  {
    ERR("Could not get stream for MRLP. Not q-ing any msgs", 0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
   Extract the Message ID field. 
  -------------------------------------------------------------------------*/
  if ( HDRDOS_GET_MSG_ID( msg_ptr, &msg_id ) != E_SUCCESS )
  {
    ERR("Message ID extraction failed.", 0, 0, 0);
    dsm_free_packet( &msg_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_DATA_OVER_SIGNALING_PROTOCOL, msg_id, 0 );

  /*-------------------------------------------------------------------------
   Perform the corresponding message processing. 
  -------------------------------------------------------------------------*/
  if (protocol_instance == HDRHAI_IN_USE) /*lint !e774 */
  {
    switch( msg_id )
    {
      case HDRDOS_DATA_OVER_SIGNALING_MSG_ID:
        /* DSM item containing the DOS message will be freed if it does not 
           have any Higher layer packet in it */
        free_packet = hdrdos_process_mt_dos_msg(app_subtype, 
                                                msg_ptr, 
                                                stream_id);
        break;

      case HDRDOS_DATA_OVER_SIGNALING_ACK_MSG_ID:
        hdrdos_process_dos_ack_msg(app_subtype, msg_ptr);
        break;
  
      default:
        ERR("Invalid in use message ID %d", msg_id, 0, 0);
        break;
    }
  }
  else /* (protocol_instance == HDRHAI_IN_CONFIG) */
  {
    ERR("No config defined for DataOverSignaling protocol",0,0,0);
  }

  if (free_packet)
  {
    dsm_free_packet( &msg_ptr );
  }
} /* hdrdos_process_msg() */

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of DOS

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of DOS

SIDE EFFECTS
  None

===========================================================================*/
void* hdrdos_get_inconfig_handle(void)
{
  return (void*) &hdrdos.session_data;
} /* hdrdos_get_inconfig_handle() */

/*===========================================================================

FUNCTION HDRDOS_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Data Over Signaling Protocol's config parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Data Over Signaling Protocol's current config
                   structure in SCMDB
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrdos_set_config_to_default
( 
  hdrdos_config_struct_type *current_config 
)
{
  /* Route A */
  current_config->route_a.rx_msg_seq = 0x3F; /* Init V(S) */
  current_config->route_a.tx_msg_seq = 0x3F; /* Init V(R) */

  /* Route B */
  current_config->route_b.rx_msg_seq = 0x3F; /* Init V(S) */
  current_config->route_b.tx_msg_seq = 0x3F; /* Init V(R) */

} /* hdrdos_set_config_to_default() */

/*===========================================================================

FUNCTION HDRDOS_PROCESS_IND

DESCRIPTION
  This function processes indications given to DOS

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for DOS to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrdos_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{

  switch (ind_name)
  {
    case HDRIND_SCM_SESSION_CHANGED:
      /* Update the local copy of current configuration from SCMDB */   
      hdrscmdb_get_inuse_config(HDRHAI_DATA_OVER_SIGNALING_PROTOCOL,
                                &hdrdos.session_data);

      break;

    default:
      ERR( "Unknown indication", 0, 0, 0 );
      break;
  }

} /* hdrdos_process_ind() */

