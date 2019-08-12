#ifndef PS_STAT_TCP_H
#define PS_STAT_TCP_H

/*===========================================================================

                          P S _ S T A T _ T C P . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to TCP.

EXTERNALIZED FUNCTIONS
  ps_stat_get_tcp()
    This function supplies the required tcp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_stat_tcp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/07   scb     Added changes to fix RCVT compilation warning
02/21/07   scb     Replaced the INC/DEC macro implementation to use the
                   COMMON_INC/COMMON_DEC macro
08/15/05   kr      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"


#include "ps_in.h"
#include "ps_stat_common.h"

/*--------------------------------------------------------------------------
  TCP Global statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  crnt_open_conns       ;
  uint32  bad_hdr_len           ;
  uint32  bad_chksum            ;
  uint32  too_short             ;
  uint32  conn_aborts           ;
  uint32  conn_timeout_aborts   ;
  uint32  conn_keepalive_aborts ;
  uint32  rst_tx                ;
  uint32  rst_rx                ;
  uint32  dropped_segs_noconn_rx;
  uint32  dropped_segs_rx       ;
  uint64  bytes_tx              ;
  uint64  bytes_rx              ;
  uint32  segs_tx               ;
  uint32  segs_rx               ;
  uint64  bytes_rexmit          ;
  uint32  segs_rexmit           ;
  uint32  dup_bytes_rx          ;
  uint32  dup_segs_rx           ;
  uint32  window_probe_tx       ;
  uint32  out_of_order_segs_rx  ;
  uint32  out_of_order_bytes_rx ;
  uint32  rexmit_timeouts       ;
  uint32  piggybacked_acks_tx   ;
  uint32  piggybacked_acks_rx   ;
  uint32  dup_acks_rx           ;
  uint32  dup_acks_tx           ;
} ps_stat_tcp_g_s_type;

/*--------------------------------------------------------------------------
  TCP Instance statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint64  bytes_tx              ;
  uint64  bytes_rx              ;
  uint32  segs_tx               ;
  uint32  segs_rx               ;
  uint64  bytes_rexmit          ;
  uint32  segs_rexmit           ;
  uint32  dup_bytes_rx          ;
  uint32  dup_segs_rx           ;
  uint32  window_probe_tx       ;
  uint32  out_of_order_segs_rx  ;
  uint32  out_of_order_bytes_rx ;
  uint32  rexmit_timeouts       ;
  uint32  piggybacked_acks_tx   ;
  uint32  piggybacked_acks_rx   ;
  uint32  dup_acks_rx           ;
  uint32  dup_acks_tx           ;
  uint16  state                 ;
  struct ps_in6_addr local_ip   ;
  uint16  local_port            ;
  struct ps_in6_addr remote_ip  ;
  uint16  remote_port           ;
} ps_stat_tcp_i_s_type;

/*---------------------------------------------------------------------------
extern declaration for tcp_stats ; defined in ps_stat_tcp.c
---------------------------------------------------------------------------*/
extern ps_stat_tcp_g_s_type tcp_stats;

/*---------------------------------------------------------------------------
  TCP statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc  : # of  current open connections  */
  /* type  : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_CRNT_OPEN_CONNS        = 0,
  /* Desc : # of incoming segments with bad header length */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_BAD_HDR_LEN            = 1,
  /* Desc : # of incoming segments with bad checksum  */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_BAD_CHKSUM             = 2,
  /* Desc : # of short (invalid) packets  */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_TOO_SHORT              = 3,
  /* Desc :  Total # of dropped connections  */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_CONN_ABORTS            = 4,
  /* Desc : # of connections dropped due to timeouts  */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_CONN_TIMEOUT_ABORTS    = 5,
  /* Desc : # of connection dropped in keepalive  */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_CONN_KEEPALIVE_ABORTS  = 6,
  /* Desc :  # of resets sent   */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_RST_TX                 = 7,
  /* Desc :  # of resets received */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_RST_RX                 = 8,
  /* Desc : # of incoming segments  dropped due to absence of a connection */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_DROPPED_SEGS_NOCONN_RX = 9,
  /* Desc : # of incoming segments  dropped   */
  /* type : uint32 */
  /* Scope : Global */
  PS_STAT_TCP_DROPPED_SEGS_RX        = 10,
  /* Desc : # of bytes sent */
  /* type : uint64 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_BYTES_TX               = 11,
  /* Desc : # of bytes received */
  /* type : uint64 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_BYTES_RX               = 12,
  /* Desc : # of segments sent */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_SEGS_TX                = 13,
  /* Desc : # of segments received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_SEGS_RX                = 14,
  /* Desc : # of bytes retransmitted */
  /* type : uint64 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_BYTES_REXMIT           = 15,
  /* Desc : # of segments retransmitted */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_SEGS_REXMIT            = 16,
  /* Desc : # of duplicate bytes received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_DUP_BYTES_RX           = 17,
  /* Desc : # of duplicate segments received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_DUP_SEGS_RX            = 18,
  /* Desc : # of window probes transmitted */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_WINDOW_PROBE_TX        = 19,
  /* Desc : # of out of order segments received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_OUT_OF_ORDER_SEGS_RX   = 20,
  /* Desc : # of out of order bytes received */
  /* type : uint64 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_OUT_OF_ORDER_BYTES_RX  = 21,
  /* Desc : # of retramit timeouts */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_REXMIT_TIMEOUTS        = 22,
  /* Desc : # of piggybacked acks transmitted */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_PIGGYBACKED_ACKS_TX    = 23,
  /* Desc : # of piggybacked acks received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_PIGGYBACKED_ACKS_RX    = 24,
  /* Desc : # of duplicate acks received */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_DUP_ACKS_RX            = 25,
  /* Desc : # of duplciate acks transmitted */
  /* type : uint32 */
  /* Scope : Global / Instance*/
  PS_STAT_TCP_DUP_ACKS_TX            = 26,
  /* Desc : state of tcp connection */
  /* type : uint16 */
  /* Scope : Instance*/
  PS_STAT_TCP_STATE                  = 27,
  /* Desc : Local IP address */
  /* type : ps_in6_addr */
  /* Scope : Instance*/
  PS_STAT_TCP_LOCAL_IP               = 28,
  /* Desc : Local port */
  /* type : uint16 */
  /* Scope : Instance*/
  PS_STAT_TCP_LOCAL_PORT             = 29,
  /* Desc : Remote IP address */
  /* type : ps_in6_addr */
  /* Scope : Instance*/
  PS_STAT_TCP_REMOTE_IP              = 30,
  /* Desc : Remote port*/
  /* type : uint16 */
  /* Scope :Instance*/
  PS_STAT_TCP_REMOTE_PORT            = 31,
  /* Desc : # of segments received */
  /* type : uint32 */
  /* Scope : Instance*/
  PS_STAT_TCP_ALL                    = 32
} ps_stat_tcp_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_GLOBAL_TCP_C

  Header defined in ps_stat_logging.h

  Fields are
    global_tcp_stats : global TCP stats
---------------------------------------------------------------------------*/
typedef struct
{
  ps_stat_tcp_g_s_type  global_tcp_stats;
} ps_stat_global_tcp_log_pkt_type;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_TCP_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_tcp_stats   : instance TCP stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_tcp_i_s_type      inst_tcp_stats;
} ps_stat_inst_tcp_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_TCP_TYPE

DESCRIPTION
  The structure describing a TCP instance.

  handle       : Instance handle.  Address of the associated scb
  local_addr   : Local address of the connection
  remote_addr  : Remote address of the connection
  local_port   : Local port of the connection
  remote_port  : Remote port of the connection
  state        : State of the connection
  reserved     : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint32              handle;
  struct ps_in6_addr  local_addr;
  struct ps_in6_addr  remote_addr;
  uint16              local_port;
  uint16              remote_port;
  uint8               state;
  uint8               reserved[3];
} ps_stat_inst_desc_tcp_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO TCP_INC_STATS()

DESCRIPTION
  This macro increments the tcp statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define TCP_INC_STATS(FIELD, INC)      \
    PS_COMMON_INC_STATS(tcp_stats.FIELD, 4, INC)

/*===========================================================================
MACRO TCP_DEC_STATS()

DESCRIPTION
  This macro decrements the tcp statistic (field) by the necessary quantity
  (dec).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define TCP_DEC_STATS(FIELD, DEC)      \
    PS_COMMON_DEC_STATS(tcp_stats.FIELD, 4, DEC)

/*===========================================================================
MACRO TCP_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the tcp statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

#define TCP_INC_INSTANCE_STATS(PTR, FIELD, INC)   \
    PS_COMMON_INC_STATS(PTR->tcp_i_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_TCP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_tcp
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_TCP()

DESCRIPTION

  This function supplies the required tcp layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the tcp control block.If
  the instance pointer is NULL, it signifies that the required statistic is
  global in scope. The third argument points to the memory location where
  the results will be copied.The result ( statistic) will be copied to this
  memory address. The fourth argument passes the length of memory allocated
  for the return value and should be greater than or equal to the memory size
  of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer that has the value of socket descriptor
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_tcp
(
  ps_stat_tcp_enum_type   stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                  ret_len
);

/*===========================================================================
FUNCTION PS_STAT_RESET_TCP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_tcp
(
  int32    handle
);


#endif /* PS_STAT_TCP_H */
