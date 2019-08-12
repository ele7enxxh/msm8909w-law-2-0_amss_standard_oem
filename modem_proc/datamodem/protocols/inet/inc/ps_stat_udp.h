#ifndef PS_STAT_UDP_H
#define PS_STAT_UDP_H

/*===========================================================================

                          P S _ S T A T _ U D P . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to UDP.

EXTERNALIZED FUNCTIONS
  ps_stat_get_udp()
    This function supplies the required udp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_stat_udp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/07   scb     Added changes to fix RCVT compilation warning
02/21/07   scb     Replaced the INC macro implementation to use the
                   COMMON_INC macro
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
  UDP Global statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 bad_len ;        /* Desc: # of datagrams with bad payload lengths*/
                          /* type: uint32 */
  uint32 bad_chksum ;     /* # of packets with bad header checksum */
                          /* type: uint32 */
  uint32 dgram_rx ;       /* # of incoming datagrams */
                          /* type: uint32 */
  uint32 dgram_tx ;       /* # of outgoing datagrams */
                          /* type: uint32 */
  uint64 bytes_rx ;       /* # of bytes received */
                          /* type: uint64 */
  uint64 bytes_tx ;       /* # of bytes transmitted */
                          /* type: uint64 */
  uint32 no_port  ;       /* # of incoming datagrams for an */
                          /* unreachable port */
                          /* type: uint32 */
  uint32 dgram_dropped_rx;/* # of incoming datagrams dropped */
                          /* type: uint32 */
  uint32 dgram_dropped_tx;/* # of outgoing datagrams dropped  */
                          /* type: uint32 */
} ps_stat_udp_g_s_type;


/*--------------------------------------------------------------------------
  UDP Instance statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 dgram_rx;          /* # of incoming datagrams */
                            /* type: uint32 */
  uint32 dgram_tx;          /* # of outgoing datagrams */
                            /* type: uint32 */
  uint64 bytes_rx;          /* # of bytes received */
                            /* type: uint64 */
  uint64 bytes_tx;          /* # of bytes transmitted */
                            /* type: uint64 */
  uint32 dgram_dropped_rx ; /* # of incoming datagrams dropped */
                            /* type: uint32 */
  struct ps_in6_addr local_ip; /* IP address of local interface */
                            /* type: ps_in6_addr */
  uint16 local_port;        /* Local port number */
                            /* type: uint16 */
  struct ps_in6_addr remote_ip;/* IP address of remote interface */
                            /* type: ps_in6_addr */
  uint16 remote_port;       /* Remote port number */
                            /* type: int16 */
} ps_stat_udp_i_s_type;


/*---------------------------------------------------------------------------
extern declaration for udp_stats ; defined in ps_stat_udp.c
---------------------------------------------------------------------------*/
extern ps_stat_udp_g_s_type udp_stats;

/*---------------------------------------------------------------------------
  UDP statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc: # of datagrams with bad payload lengths*/
  /* type: uint32 */
  /* Scope : Global */
  PS_STAT_UDP_BAD_LEN          = 0,
  /* # of packets with bad header checksum */
  /* type: uint32 */
  /* Scope : Global */
  PS_STAT_UDP_BAD_CHKSUM       = 1,
  /* # of incoming datagrams */
  /* type: uint32 */
  /* Scope : Global/Instance */
  PS_STAT_UDP_DGRAM_RX         = 2,
  /* # of outgoing datagrams */
  /* type: uint32 */
 /* Scope : Global/Instance */
  PS_STAT_UDP_DGRAM_TX         = 3,
  /* # of bytes received */
  /* type: uint32 */
  /* Scope : Global/Instance */
  PS_STAT_UDP_BYTES_RX         = 4,
  /* # of bytes sent */
  /* type: uint64 */
  /* Scope : Global/Instance */
  PS_STAT_UDP_BYTES_TX         = 5,
  /* # of incoming datagrams for an unreachable port */
  /* type: uint64 */
  /* Scope : Global */
  PS_STAT_UDP_NO_PORT          = 6,
  /* # of incoming datagrams dropped */
  /* type: uint32 */
  /* Scope : Global/Instance*/
  PS_STAT_UDP_DGRAM_DROPPED_RX = 7,
  /* # of outgoing datagrams dropped */
  /* type: uint32 */
  /* Scope : Global */
  PS_STAT_UDP_DGRAM_DROPPED_TX = 8,
  /* IP address of local interface */
  /* type: ps_in6_addr */
  /* Scope : Instance */
  PS_STAT_UDP_LOCAL_IP         = 9,
  /* local port number */
  /* type: uint16 */
  /* Scope : Instance */
  PS_STAT_UDP_LOCAL_PORT       = 10,
  /* IP address of remote interface */
  /* type: ps_in6_addr */
  /* Scope : Instance */
  PS_STAT_UDP_REMOTE_IP        = 11,
  /* remote port number */
  /* type: uint16 */
  /* Scope : Instance */
  PS_STAT_UDP_REMOTE_PORT      = 12,
  /* all statistics */
  /* Scope : Global/Instance */
  PS_STAT_UDP_ALL              = 13
} ps_stat_udp_enum_type ;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_GLOBAL_UDP_C

  Common header defined in ps_stat_logging.

  Fields are
    global_udp_stats : global UDP stats
---------------------------------------------------------------------------*/
typedef struct
{
  ps_stat_udp_g_s_type  global_udp_stats;
} ps_stat_global_udp_log_pkt_type;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_UDP_INST_C

  Common header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_udp_stats   : instance UDP stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_udp_i_s_type      inst_udp_stats;
} ps_stat_inst_udp_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_UDP_TYPE

DESCRIPTION
  The structure describing a UDP instance.

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
} ps_stat_inst_desc_udp_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO UDP_INC_STATS(field, inc)

DESCRIPTION
  This macro increments the udp statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

#define UDP_INC_STATS(FIELD, INC)         \
    PS_COMMON_INC_STATS(udp_stats.FIELD, 4, INC)

/*===========================================================================
MACRO UDP_INC_INSTANCE_STATS(field, inc)

DESCRIPTION
  This macro increments the udp statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define UDP_INC_INSTANCE_STATS(PTR, FIELD, INC)      \
    PS_COMMON_INC_STATS(PTR->udp_instance_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_UDP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_udp
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_UDP()

DESCRIPTION
  This function supplies the required udp layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the udp control block.If
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
errno_enum_type ps_stat_get_udp
(
  ps_stat_udp_enum_type   stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                  ret_len
) ;

/*===========================================================================
FUNCTION PS_STAT_RESET_UDP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance to be reset

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_udp
(
  int32       handle
) ;



#endif /* PS_STAT_UDP_H */
