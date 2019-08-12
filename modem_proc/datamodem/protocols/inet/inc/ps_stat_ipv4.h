#ifndef PS_STAT_IPV4_H
#define PS_STAT_IPV4_H

/*===========================================================================

                          P S _ S T A T _ I P V 4 . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to IPv4.

EXTERNALIZED FUNCTIONS
  ps_stat_get_ipv4()
    This function supplies the required ipv4 stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_stat_ipv4.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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
#include "ps_stat_common.h"
#include "IxErrno.h"

/*--------------------------------------------------------------------------
  IPv4 Statistics - all statistics are global to the whole IPv4 layer on the
  phone. No 'Per Instance' statistics
---------------------------------------------------------------------------*/
typedef struct{
  uint32 bad_hdr_len ;    /* Desc: # of packets with invalid header length */
                          /* type: uint32 */
  uint32 bad_len ;        /* Desc: # of packets with bad payload lengths  */
                          /* type: uint32 */
  uint32 bad_options ;    /* Desc: # of packets with errors in option */
                          /* type: uint32 */
                          /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32 bad_version ;    /* # of packets with bad version in ip header */
                          /* type: uint32 */
  uint32 bad_chksum ;     /* # of packets with bad header checksum */
                          /* type: uint32 */
  uint32 too_short ;      /* # of ip packets that are shorter */
                          /* than the size of a valid ip header */
                          /* type: uint32 */
  uint32 no_route;        /* # of lpackets not transmitted  */
                          /* because of absence of route */
                          /* type: uint32 */
  uint32 no_proto ;       /* # of incoming packets with unknown protocol */
                          /* type: uint32 */
  uint32 pkts_rx  ;       /* # of incoming packets */
                          /* type: uint32 */
  uint32 pkts_dropped_rx; /* # of incoming packets dropped at the ip layer */
                          /* type: uint32 */
  uint32 pkts_dropped_tx; /* # of outgoing packets dropped at the ip layer */
                          /* type: uint32 */
  uint32 pkts_fwd ;       /* # of forwarded ip packets*/
                          /* type: uint32 */
  uint32 pkts_tx;         /* # of outgoing packets*/
                          /* type: uint32 */
  uint32 mcast_rx ;       /* # of incoming multicast packets*/
                          /* type: uint32 */
  uint32 mcast_tx ;       /* # of outgoing multicast packets*/
                          /* type: uint32 */
  uint32 frag_ok   ;      /* # of  ip packets that have been successfully */
                          /* fragmented  */
                          /* type: uint32 */
  uint32 frag_fails ;     /* # of ip packets that failed fragmentation */
                          /* ( e.g Don’t fragment flag was set) */
                          /* type: uint32 */
  uint32 frag_create ;    /* # of ip fragments created   */
                          /* type: uint32 */
  uint32 reasm_reqd  ;    /* # of incoming ip fragments that needed to */
                          /* be reassembled */
                          /* type: uint32 */
  uint32 reasm_ok    ;    /* # of incoming ip packets successfully */
                          /* reassembled */
                          /* type: uint32 */
  uint32 reasm_fails  ;   /* # of  incoming ip packets that failed */
                          /* reassembly */
                          /* type: uint32 */
  uint32 reasm_timeout ;  /* # of incoming packets that failed reassembly */
                          /* due to timeout */
                          /* type: uint32 */
} ps_stat_ipv4_s_type ;

/*---------------------------------------------------------------------------
extern declaration for ipv4_stats ; defined in ps_stat_ipv4.c
---------------------------------------------------------------------------*/
extern ps_stat_ipv4_s_type ipv4_stats ;

/*---------------------------------------------------------------------------
 For description of the enum values, please see the corr entry in the struct
 definition of ps_stat_ipv4_s_type
---------------------------------------------------------------------------*/
typedef enum{
  PS_STAT_IPV4_BAD_HDR_LEN     = 0,
  PS_STAT_IPV4_BAD_LEN         = 1,
  PS_STAT_IPV4_BAD_OPTIONS     = 2,
  PS_STAT_IPV4_BAD_VERSION     = 3,
  PS_STAT_IPV4_BAD_CHKSUM      = 4,
  PS_STAT_IPV4_TOO_SHORT       = 5,
  PS_STAT_IPV4_NO_ROUTE        = 6,
  PS_STAT_IPV4_NO_PROTO        = 7,
  PS_STAT_IPV4_PKTS_RX         = 8,
  PS_STAT_IPV4_PKTS_DROPPED_RX = 9,
  PS_STAT_IPV4_PKTS_DROPPED_TX = 10,
  PS_STAT_IPV4_PKTS_FWD        = 11,
  PS_STAT_IPV4_PKTS_TX         = 12,
  PS_STAT_IPV4_MCAST_RX        = 13,
  PS_STAT_IPV4_MCAST_TX        = 14,
  PS_STAT_IPV4_FRAG_OK         = 15,
  PS_STAT_IPV4_FRAG_FAILS      = 16,
  PS_STAT_IPV4_FRAG_CREATE     = 17,
  PS_STAT_IPV4_REASM_REQD      = 18,
  PS_STAT_IPV4_REASM_OK        = 19,
  PS_STAT_IPV4_REASM_FAILS     = 20,
  PS_STAT_IPV4_REASM_TIMEOUT   = 21,
  PS_STAT_IPV4_ALL             = 22
} ps_stat_ipv4_enum_type ;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_GLOBAL_IPV4_C

  Common header defined in ps_stat_logging.h

  Fields are
    global_ipv4_stats : global IPv4 stats
---------------------------------------------------------------------------*/
typedef struct
{
  ps_stat_ipv4_s_type  global_ipv4_stats;
} ps_stat_global_ipv4_log_pkt_type;

/*===========================================================================
MACRO IPV4_INC_STATS()

DESCRIPTION
  This macro increments the ipv4 statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IPV4_INC_STATS(FIELD, INC)     \
    PS_COMMON_INC_STATS(ipv4_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_IPV4()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_ipv4
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_IPV4()

DESCRIPTION

  This function supplies the required IPv4 layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an handle to
  an instance ( For per-instance statistics only).The second argument SHOULD
  BE NULL for ipv4 since ipv4 supports global statistics only. The third
  argument points to the memory location where the results will be copied.
  The result ( statistic) will be copied to this memory address. The fourth
  argument passes the length of memory allocated for the return value and
  should be greater than or equal to the memory size of the statistic
  requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h. The function also calls err_fatal in the
  case of any error.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer to instance; should be NULL for ipv4
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ipv4
(
  ps_stat_ipv4_enum_type   stat,
  void                     *instance_ptr,
  void                     *return_value,
  uint16                   ret_len
) ;
#endif /* PS_STAT_IPV4_H */
