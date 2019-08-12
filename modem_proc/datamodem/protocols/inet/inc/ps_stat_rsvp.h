#ifndef PS_STAT_RSVP_H
#define PS_STAT_RSVP_H

/*======================================================================

                          P S _ S T A T _ R S V P . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions
  for the statistics pertaining to Resourse Reservation Protocol.

EXTERNALIZED FUNCTIONS
  ps_stat_get_rsvp()
    This function supplies the required RSVP stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
======================================================================*/

/*======================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
----------  ---     ----------------------------------------------------
12/05/13    ss      Added alignment attribute to ensure 4 byte alignment
02/27/07    scb     Added changes to fix RCVT compilation warning
02/21/07    scb     Replaced the INC macro implementation to use the
                    COMMON_INC macro
08/18/06    scb     Updated enum values to start with 0 for calculating
                    correct offset.
07/03/2006  scb     Initial version

======================================================================*/


/*======================================================================

                     INCLUDE FILES FOR MODULE

======================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"
#include "ps_in.h"


/*----------------------------------------------------------------------
  RSVP Statistics - Instance stats only.
----------------------------------------------------------------------*/
typedef struct
{
  uint32 resv_tx;
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
  uint16 state;
} ps_stat_rsvp_i_s_type;

/*----------------------------------------------------------------------
  RSVP statistics enum along with the description and type of each entity
----------------------------------------------------------------------*/
typedef enum
{
  /*
    Desc: Number of RSVP reserve messages sent
    Type: uint32
  */
  PS_STAT_RSVP_RESV_TX                          = 0 ,
  /*
    Desc: Number of RSVP reserve messages failed to send
    Type: uint32
  */
  PS_STAT_RSVP_RESV_FAIL_TX                     = 1 ,
  /*
    Desc: Number of RSVP reserve messages failed to send because of 
          invalid iface id
    Type: uint32
  */
  PS_STAT_RSVP_RESV_INVALID_IFACE_ID_TX         = 2 ,
  /*
    Desc: Number of RSVP reserve messages failed to send because of 
          unrecognised IP family
    Type: uint32
  */
  PS_STAT_RSVP_RESV_UNRECOG_IP_FAMILY_TX        = 3 ,
  /*
    Desc: Number of RSVP reserve messages failed to send because of 
          unsupported xport.
    Type: uint32
  */
  PS_STAT_RSVP_RESV_UNSUPP_XPORT_TX             = 4 ,
  /*
    Desc: Number of RSVP messages received
    Type: uint32  
  */
  PS_STAT_RSVP_RX                               = 5 ,
  /*
    Desc: Number of RSVP ERROR messages received
    Type: uint32
  */
  PS_STAT_RSVP_RESV_ERR_RX                      = 6 ,
  /*
    Desc: Number of RSVP CONF messages recieved
    Type: uint32
  */
  PS_STAT_RSVP_RESV_CONF_RX                     = 7 ,
  /*
    Desc: Number of RSVP messages received and dropped
    Type: uint32
  */
  PS_STAT_RSVP_DROPPED_RX                       = 8 ,
  /*
    Desc: Number of RSVP malformed packets recieved
    Type: uint32
  */
  PS_STAT_RSVP_MALFORMED_PKT_RX                 = 9  ,
  /*
    Desc: Number of RSVP messages with invalid checksum received
    Type: uint32
  */
  PS_STAT_RSVP_INVALID_CKSUM_RX                 = 10  ,
  /*
    Desc: Number of RSVP messages with invalid version received
    Type: uint32
  */
  PS_STAT_RSVP_INVALID_RSVP_VER_RX              = 11  ,
  /*
    Desc: Number of RSVP messages with no session object received
    Type: uint32
  */
  PS_STAT_RSVP_NO_SESS_OBJ_RX                   = 12  ,
  /*
    Desc: Number of RSVP messages with mismatch in IPv4 protocol lengths
          received
    Type: uint32
  */
  PS_STAT_RSVP_IPv4_PROTO_LEN_MISMATCH_RX       = 13  ,
  /*
    Desc: Number of RSVP messages with mismatch in IPv6 protocol lengths 
          received
    Type: uint32
  */
  PS_STAT_RSVP_IPv6_PROTO_LEN_MISMATCH_RX       = 14  ,
  /*
    Desc: Number of RSVP session objects received whch are unidentified
    Type: uint32
  */
  PS_STAT_RSVP_SESS_OBJ_UNIDENTIFIED_RX         = 15  ,
  /*
    Desc: Number of RSVP messages dropped because message queue is full
    Type: uint32
  */
  PS_STAT_RSVP_MSG_QUEUE_FULL_RX                = 16  ,
  /*
    Desc: Number of RSVP messages with unsupported message type received
    Type: uint32
  */
  PS_STAT_RSVP_MSG_TYPE_UNSUPPORTED_RX          = 17  ,
  /*
    Desc: Number of RSVP messages recieved which are unable to be parsed
    Type: uint32
  */
  PS_STAT_RSVP_UNABLE_TO_PARSE_RX               = 18  ,
  /*
    Desc: Current state of the RSVP client.
    Type: uint32
  */
  PS_STAT_RSVP_STATE                            = 19  ,
  
  PS_STAT_RSVP_ALL 
} ps_stat_rsvp_enum_type ;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_RSVP_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_rsvp_stats  : instance RSVP stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_rsvp_i_s_type     inst_rsvp_stats;
} ps_stat_inst_rsvp_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_RSVP_TYPE

DESCRIPTION
  The structure describing a RSVP instance.

  handle       : Instance handle.  Index of the RSVP CB in the RSVP mem pool
  remote_addr  : Remote address of the RSVP session
  remote_port  : Remote port of the RSVP session
  addr_family  : Type of IP address (v4 or v6)
  reserved     : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint32              handle;
  struct ps_in6_addr  remote_addr;
  uint16              remote_port;
  uint8               addr_family;
  uint8               reserved;
} ps_stat_inst_desc_rsvp_type __attribute__ ((aligned(4)));


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*======================================================================
MACRO RSVP_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the RSVP statistic(field) by the necessary 
  quantity (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
======================================================================*/
#define RSVP_INC_INSTANCE_STATS(RSVP_CB_PTR, FIELD, INC)              \
    PS_COMMON_INC_STATS(RSVP_CB_PTR->rsvp_i_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_RSVP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_rsvp
(
  void
);

/*======================================================================
FUNCTION PS_STAT_GET_RSVP()

DESCRIPTION
  This function supplies the required RSVP layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an 
  handle to an instance ( For per-instance statistics only).
  The third argument points to the memory location where the results 
  will be copied. The result ( statistic) will be copied to this memory   
  address. The fourth argument passes the length of memory that
  allocated for the return value and should be greater than or equal to   
  the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h. The function also calls err_fatal in   
  the case of any error.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer to instance; 
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
======================================================================*/
errno_enum_type ps_stat_get_rsvp
(
  ps_stat_rsvp_enum_type     stat,
  void                       *instance_ptr,
  void                       *return_value,
  uint16                     ret_len
) ;

/*======================================================================
FUNCTION PS_STAT_RESET_RSVP

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance

SIDE EFFECTS
  None
======================================================================*/
void ps_stat_reset_rsvp
(
  int32          handle
) ;
#endif /* PS_STAT_RSVP_H */
