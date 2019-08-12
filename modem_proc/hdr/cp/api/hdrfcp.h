#ifndef HDRFCP_H
#define HDRFCP_H
/*===========================================================================

                   F L O W  C O N T R O L  P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Flow control Protocol.
  
Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrfcp.h_v   1.4   14 Sep 2001 14:38:54   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrfcp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/26/11   smd     SU API cleanup. 
10/23/06   sju     Added support to send Xon/XoffRequest by using RASM.
06/20/06   mpa     Removed hdrfcp_commit_session_data_to_efs(). 
06/05/06   pba     Added support to store session data in EFS.
09/14/01   vas     Added hdrfcp_stream_is_open(). 
08/17/01   vas     Rearchitected to fit in with the hdrds / hdrpac modules.
06/20/01   vas     Code cleanup
05/30/01   vas     Created module

===========================================================================*/

/*===========================================================================
  
                       INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdrhai.h"
#include "hdrind.h"
#include "dsm_pool.h"
/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRFCP_XON_REQ_MSG_ID         0x07
#define HDRFCP_XOFF_REQ_MSG_ID        0x09
#define HDRFCP_DATA_READY_ACK_MSG_ID  0x0C


/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
typedef void ( *hdrfcp_data_ready_cb_type )( void );

typedef enum
{
  HDRFCP_SEND_XON_CMD = 0,
  HDRFCP_SEND_XOFF_CMD = 1
} hdrfcp_cmd_name_enum_type;

typedef struct
{
   hdrfcp_cmd_name_enum_type name;        /* Name of the command      */
   hdrhai_stream_enum_type stream_id;     /* Stream for which cmd is meant */
   hdrhai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
} hdrfcp_cmd_type;

typedef enum
{
  HDRFCP_CLOSED = 0,         /* Closed State */
  HDRFCP_OPEN = 1,           /* Open State */
} hdrfcp_state_enum_type;

typedef struct
{
  hdrhai_stream_enum_type stream_id;     /* Stream for this instance       */
  hdrfcp_state_enum_type  state;         /* State of the stream            */
}hdrfcp_session_data_type ;

/* Number of Streams managed by FCP */
#define HDRFCP_NUM_STREAMS 3

typedef struct
{
  hdrfcp_session_data_type stream[HDRFCP_NUM_STREAMS];
}hdrfcp_config_struct_type ;

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION HDRFCP_POWERUP_INIT

DESCRIPTION
  This function performs powerup initialization for Flow Control protocol.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_powerup_init( void );

/*===========================================================================

FUNCTION HDRFCP_STREAM_IS_OPEN

DESCRIPTION
  This function checks the state of the specified stream and returns whether
  it is in Open state

DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream being queried.

RETURN VALUE
  TRUE - If the stream state is FCP_OPEN
  FALSE - If the stream state is FCP_CLOSED or FCP_OPEN_XOFF_SENT 
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrfcp_stream_is_open( hdrhai_stream_enum_type stream_id );

/*===========================================================================

FUNCTION HDRFCP_SEND_XON_REQ

DESCRIPTION
  This function is used to command the Flow Control protocol to send a XON 
  request msg to the Access Network.

  This function ignores the stream_id. It always sends the message for 
  Stream 1. It does not queue the message & does not set Xon Response timers.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_xon_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id 
);

/*===========================================================================

FUNCTION HDRFCP_SEND_XOFF_REQ

DESCRIPTION
  This function is used to command the Flow Control protocol to send a XOFF
  request msg to the Access Network.

  This function ignores the stream_id. It always sends the message for 
  Stream 1. It does not queue the message & does not set Xon Response timers.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_send_xoff_req
(
  hdrhai_protocol_name_enum_type sender ,
  hdrhai_stream_enum_type stream_id 
);

/*===========================================================================

FUNCTION HDRFCP_MSG_CB

DESCRIPTION
  This function is called from PAC when it receives a message for FCP.
  The function queues the message on the ds command queue.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDRFCP_PROCESS_MSG

DESCRIPTION
  This function processes received messages for FCP

DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfcp_process_msg
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDRFCP_PROCESS_CMD

DESCRIPTION
  This function processes commands for the FCP

DEPENDENCIES
  None
  
PARAMETERS
  fcp_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfcp_process_cmd
(
  hdrfcp_cmd_type *fcp_cmd_ptr 
);

/*===========================================================================

FUNCTION HDRFCP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of FCP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of FCP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrfcp_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRFCP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Flow Control Protocol's config parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Flow Control Protocol's current config
                   structure in SCMDB
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_set_config_to_default
( 
  hdrfcp_config_struct_type *current_config 
);

/*===========================================================================

FUNCTION HDRFCP_PROCESS_IND

DESCRIPTION
  This function processes indications given to FCP

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for FCP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfcp_process_ind 
(
  hdrind_ind_name_enum_type   ind_name,
  hdrind_ind_data_union_type *ind_data_ptr
);

/*===========================================================================

FUNCTION HDRFCP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the FCP.

DEPENDENCIES
  None

PARAMETERS
 timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrfcp_process_timer( uint32 timer_id );

#endif /* HDRFCP_H */
