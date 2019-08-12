#ifndef PS_RSVPI_H
#define PS_RSVPI_H
/*===========================================================================
   R E S O U R C E   R E S E R V A T I O N   P R O T O C O L (RSVP)
                 I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
 The RSVP internal header file.


Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_rsvpi.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/14    ss     Update RSVP to send RESV_ERROR with ERROR_SPEC of length 
                   4 conditionally.
03/18/13    ss     Write to RSVP socket only if socket is writeable
11/29/05    sv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ps_rsvp.h"
#include "ps_mem.h"
#include "ps_svc.h"
#include "dstask_v.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define PS_RSVP_DEF_TTL (255)

#define PS_RSVP_DEF_REFRESH_TIME  300          /* Refresh time in seconds */


#define PS_RSVPI_STYLE_OBJ_LENGTH 8
#define PS_RSVPI_STYLE_OBJ_CLASS_NUM 8
#define PS_RSVPI_STYLE_OBJ_DEF_CLASS_TYPE 1

#define PS_RSVPI_RESV_CONF_OBJ_CLASS_NUM 15
#define PS_RSVPI_RESV_CONF_OBJ_V4_CLASS_TYPE 1
#define PS_RSVPI_RESV_CONF_OBJ_V6_CLASS_TYPE 2
#define PS_RSVPI_RESV_CONF_OBJ_V4_LENGTH 8
#define PS_RSVPI_RESV_CONF_OBJ_V6_LENGTH 20

#define PS_RSVPI_TIME_VALUE_OBJ_LENGTH 8
#define PS_RSVPI_TIME_VALUE_OBJ_CLASS_NUM 5
#define PS_RSVPI_TIME_VALUE_OBJ_DEF_CLASS_TYPE 1


#define PS_RSVPI_SESSION_OBJ_CLASS_NUM 1
#define PS_RSVPI_SESSION_OBJ_V4_CLASS_TYPE 1
#define PS_RSVPI_SESSION_OBJ_V6_CLASS_TYPE 2
#define PS_RSVPI_SESSION_OBJ_V4_LENGTH 12
#define PS_RSVPI_SESSION_OBJ_V6_LENGTH 24

#define PS_RSVPI_ERROR_SPEC_OBJ_CLASS_NUM 6
#define PS_RSVPI_ERROR_SPEC_OBJ_V4_CLASS_TYPE 1
#define PS_RSVPI_ERROR_SPEC_OBJ_V6_CLASS_TYPE 2
#define PS_RSVPI_ERROR_SPEC_OBJ_V4_LENGTH 12
#define PS_RSVPI_ERROR_SPEC_OBJ_V6_LENGTH 24
#define PS_RSVPI_ERROR_SPEC_OBJ_EMPTY_PKT_LEN 4

#define PS_RSVPI_VERSION_FLAGS 0x10

#define PS_RSVPI_MAX_MSGS 5

/*---------------------------------------------------------------------------
  RSVP hdr
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   vers_flags;
  uint8   msg_type;
  uint16  checksum;
  uint8   ttl;
  uint8   reserved;
  uint16  length;
} ps_rsvpi_hdr_type;

/*---------------------------------------------------------------------------
  RSVP COMMON object hdr
---------------------------------------------------------------------------*/
typedef struct
{
  uint16  length;
  uint8   class_num;
  uint8   class_type;
} ps_rsvpi_common_obj_hdr;


/*---------------------------------------------------------------------------
  RSVP Session object common v4 & v6 hdr
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   protocol;
  uint8   flags;
  uint16  port;
} ps_rsvpi_common_session_obj_hdr;

/*---------------------------------------------------------------------------
  RSVP STYLE obj hdr
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned int flags    : 8;
  unsigned int reserved : 16;
  unsigned int style    : 8;
} ps_rsvpi_style_obj_hdr;

/*---------------------------------------------------------------------------
  RSVP state type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_RSVPI_CLOSED      = 1,
  PS_RSVPI_SESSION     = 2,
  PS_RSVPI_SEND        = 3,
  PS_RSVPI_RESV        = 4,
  PS_RSVPI_SEND_RESV   = 5
} ps_rsvpi_state_enum_type;

/*---------------------------------------------------------------------------
  RSVP MSG structure
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type  * msg;
  uint16           length;
  uint16           msg_id;
  uint32           obj_mask;
  ps_rsvp_msg_enum_type msg_type;
} ps_rsvpi_msg_type;

/*---------------------------------------------------------------------------
  RSVP Error Spec object common part of hdr for both v4 and v6 addr
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   flags;
  uint8   error_code;
  uint16  error_value;
} ps_rsvpi_common_error_spec_obj_hdr;

/*---------------------------------------------------------------------------
  RSVP control block
---------------------------------------------------------------------------*/
typedef struct
{

  q_link_type              link;   /* Link to next RSVP control block      */
  uint8                    ttl;
  uint8                    family;
  uint32                   session_id;
  ps_rsvpi_state_enum_type state;
  uint32                   rsvp_obj_enum_mask;
  ps_rsvp_cback_fcn_type   cb_fcn;
  void                   * user_data;
  ps_rsvp_protocol_opt_type xprt;
  int16                    sockfd;             /* socket for RSVP message */
  uint32                   socket_event_mask;   /* Socket event mask */
  boolean                  rfc_compliant;
  struct ps_sockaddr_in6   dest_addr;
  
  /* Are we allowed to write to the underlying RSVP socket */
  boolean                  is_socket_write_allowed;

  struct
  {
    struct ps_sockaddr_in6 dest;
    int32                  prot_id;
  } session_obj;

  struct
  {
    dsm_item_type                         *opaque_obj;
  } path;

  struct
  {
    ps_rsvp_time_value_obj_type           time_value_obj;
    ps_rsvp_resv_conf_obj_type            resv_conf_obj;
    ps_rsvp_style_obj_enum_type           style_obj;
    dsm_item_type                         *opaque_obj;
  } resv;

  struct
  {
    dsm_item_type                         *opaque_obj;
  } path_err;

  struct
  {
    ps_rsvp_error_spec_obj_type           error_spec_obj;
    ps_rsvp_style_obj_enum_type           style_obj;
    dsm_item_type                         *opaque_obj;
  } resv_err;

  struct
  {
    ps_rsvp_error_spec_obj_type           error_spec_obj;
    ps_rsvp_resv_conf_obj_type            resv_conf_obj;
    ps_rsvp_style_obj_enum_type           style_obj;
    dsm_item_type                         *opaque_obj;
  } resv_conf;

  struct
  {
    dsm_item_type                         *opaque_obj;
  } resv_tear;

  struct
  {
    dsm_item_type                         *opaque_obj;
  } path_tear;


  ps_rsvpi_msg_type  rcv_msg_array[PS_RSVPI_MAX_MSGS]; /* receive queue */

  dsm_watermark_type  sndq_wm;  /*  pending queue watermark                */
  q_type              sndq;     /*  pending queue for the session          */

  struct rsvp_cb_stats
  {
    uint32 resv_tx;
    uint32 resv_rx;
    uint32 resv_fail_tx;
    uint32 resv_invalid_iface_id_tx;
    uint32 resv_unrecog_ip_family_tx;
    uint32 resv_unsupp_xport_tx;
    uint32 rsvp_rx;
    uint32 resv_err_rx;
    uint32 resv_conf_rx;
    uint32 dropped_rx;
    uint32 malformed_pkt_rx;
    uint32 invalid_cksum_rx;
    uint32 invalid_rsvp_ver_rx;
    uint32 no_sess_obj_rx;
    uint32 ipv4_proto_len_mismatch_rx;
    uint32 ipv6_proto_len_mismatch_rx;
    uint32 sess_obj_unidentified_rx;
    uint32 msg_queue_full_rx;
    uint32 msg_type_unsupp_rx;
    uint32 unable_to_parse_rx;
    uint32 queried;
  } rsvp_i_stats;

} ps_rsvpi_ctrl_blk_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_RSVPI_INIT()

DESCRIPTION
  Initializes RSVP internal module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_rsvpi_init
(
  void
);

/*===========================================================================
FUNCTION PS_RSVPI_ALLOC

DESCRIPTION
  Creates a RSVP session

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Allocates and initializes RSVP control block.
===========================================================================*/
ps_rsvpi_ctrl_blk_type * ps_rsvpi_alloc
(
  void
);

/*===========================================================================
FUNCTION PS_RSVPI_CREATE_SOCKET

DESCRIPTION
  Allocate a UDP socket if the xprt protocol is UDP.

DEPENDENCIES
  None

RETURN VALUE
  Returns success if we are able to allocate and initialize a socket.

SIDE EFFECTS
  Allocates and initializes UDP socket.
===========================================================================*/
int16 ps_rsvpi_create_socket
(
  ps_rsvpi_ctrl_blk_type  * rsvp_cb_ptr,
  int16                   * ps_errno
);

/*===========================================================================
FUNCTION PS_RSVPI_LOOKUP_SESSION

DESCRIPTION
  Look up for RSVP control block

DEPENDENCIES
  None

RETURN VALUE
  Retruns the RSVP control block association with the session id.

SIDE EFFECTS
  None
===========================================================================*/
ps_rsvpi_ctrl_blk_type * ps_rsvpi_lookup_session
(
  int32 sid
);

/*===========================================================================
FUNCTION PS_RSVPI_READ_OBJECT

DESCRIPTION
  Return the requested object.

DEPENDENCIES
  None

RETURN VALUE
  Returns the object mask

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_rsvpi_read_object
(
  ps_rsvpi_msg_type       * msg_ptr,
  ps_rsvp_obj_enum_type     obj_type,
  void                    * data,
  int16                   * ps_errno
);

/*===========================================================================
FUNCTION PS_RSVPI_SEND_RESERVE

DESCRIPTION
  Send RSVP reserve message.

DEPENDENCIES
  None

RETURN VALUE
  Returns success if we are able to allocate and initialize a socket.

SIDE EFFECTS
  Allocates and initializes UDP socket.
===========================================================================*/
int32 ps_rsvpi_send_reserve
(
  ps_rsvpi_ctrl_blk_type * rsvp_cb_ptr,
  struct ps_sockaddr_in6 * v6_sockaddr,
  int16                  * ps_errno
);

/*===========================================================================
FUNCTION PS_RSVPI_SEND_RESV_ERR

DESCRIPTION
  Send RSVP RESV_ERR message.

DEPENDENCIES
  None

RETURN VALUE
  DSS_ERROR for failure.
  DSS_SUCCESS for success.

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_rsvpi_send_resv_err
(
  ps_rsvpi_ctrl_blk_type * rsvp_cb_ptr,
  struct ps_sockaddr_in6 * v6_sockaddr,
  int16                  * ps_errno_ptr
);

/*===========================================================================
FUNCTION PS_RSVPI_PROCESS_SOCKENT_EVENT

DESCRIPTION
  Process socket callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Send RSVP message on write event. Process RSVP message on read event.
===========================================================================*/
void ps_rsvpi_process_socket_event
(
  ds_cmd_type  * ds_cmd_ptr
);

#endif  /* PS_RSVPI_H */
