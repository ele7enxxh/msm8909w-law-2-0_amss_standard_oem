#ifndef PS_STAT_ICMPV6_H
#define PS_STAT_ICMPV6_H

/*===========================================================================

                          P S _ S T A T _ I C M P V 6 . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to ICMPv6.

EXTERNALIZED FUNCTIONS
  ps_stat_get_icmpv6()
    This function supplies the required icmpv6 stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_stat_icmpv6.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#include "ps_stat_common.h"
#include "IxErrno.h"
/*--------------------------------------------------------------------------
  ICMPv6 Statistics - all statistics are global to the whole ICMPv6 layer on
  on phone. No 'Per Instance' statistics
---------------------------------------------------------------------------*/
typedef struct{
  uint32  msgs_rx ;             /* # of incoming icmp messages */
                                /* type: uint32 */
  uint32  msgs_dropped_rx ;     /* # of incoming icmp messages dropped */
                                /* type: uint32 */
  uint32  dest_unreach_rx ;     /* # of incoming destination unreachables*/
                                /* type: uint32 */
  uint32  echo_rx ;             /* # of incoming echo mesgs*/
                                /* type: uint32 */
  uint32  echo_reply_rx ;       /* # of incoming echo replies*/
                                /* type: uint32 */
  uint32  bad_chksum_rx ;       /* # of incoming icmp messages with bad */
                                /* checksum*/
                                /* type: uint32 */
  uint32  unsupported_type_rx ; /* # of incoming icmp messages with */
                                /* unsupported type  */
                                /* type: uint32 */
  uint32  parm_probs_rx ;       /* # of incoming icmp parameter problem */
                                /* messages*/
                                /* type: uint32 */
  uint32  bad_code_rx ;         /* # of incoming messages with bad code*/
                                /* type: uint32 */
  uint32  redirects_rx ;        /* # of incoming redirects*/
                                /* type: uint32 */
  uint32  time_exceeds_rx ;     /* # of incoming time exceeded messages*/
                                /* type: uint32 */
  uint32  grp_query_rx ;        /* # of incoming group member queries*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  grp_resp_rx ;         /* # of incoming group member responses*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  grp_reductions_rx ;   /* # of incoming group member reductions*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  routersolicits_rx ;   /* # of incoming router solicits*/
                                /* type: uint32 */
  uint32  neighborsolicits_rx ; /* # of incoming neighbour solicits*/
                                /* type: uint32 */
  uint32  neighboradvts_rx ;    /* # of incoming neighbour advertisements*/
                                /* type: uint32 */
  uint32  pkt_too_big_rx ;      /* # of incoming packet too big messages*/
                                /* type: uint32 */
  uint32  routeradvts_rx ;      /* # of incoming router advertisements*/
                                /* type: uint32 */
  uint32  msgs_tx ;             /* # of outgoing icmp messages*/
                                /* type: uint32 */
  uint32  dest_unreach_tx ;     /* # of outgoing destination unreachables*/
                                /* type: uint32 */
  uint32  echo_tx ;             /* # of outgoing echo mesgs*/
                                /* type: uint32 */
  uint32  echo_reply_tx ;       /* # of outgoing echo replies*/
                                /* type: uint32 */
  uint32  parm_probs_tx ;       /* # of outgoing icmp parameter problem */
                                /* messages*/
                                /* type: uint32 */
  uint32  redirects_tx ;        /* # of outgoing redirects*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  time_exceeds_tx ;     /* # of outgoing time exceeded messages*/
                                /* type: uint32 */
  uint32  grp_query_tx ;        /* # of outgoing group member queries*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  grp_resp_tx ;         /* # of outgoing group member responses*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  grp_reductions_tx ;   /* # of outgoing group member reductions*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  routersolicits_tx ;   /* # of outgoing router solicits*/
                                /* type: uint32 */
  uint32  neighborsolicits_tx ; /* # of outgoing neighbour solicits*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  neighboradvts_tx ;    /* # of outgoing neighbour advertisements*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  pkt_too_big_tx ;      /* # of outgoing packet too big messages*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  routeradvts_tx ;      /* # of outgoing router advertisements*/
                                /* type: uint32 */
                                /* Note : NOT IMPLEMENTED */
  uint32  rate_limit_tx ;       /* # of unsent messages due to rate limit*/
                                /* type: uint32 */
  uint32  msgs_dropped_tx ;     /* # of outgoing messages dropped*/
                                /* type: uint32 */
} ps_stat_icmpv6_s_type ;

/*---------------------------------------------------------------------------
extern declaration for icmpv6_stats ; defined in ps_stat_icmpv6.c
---------------------------------------------------------------------------*/
extern ps_stat_icmpv6_s_type icmpv6_stats ;

/*---------------------------------------------------------------------------
 For description of the enum values, please see the corr entry in the struct
 definition of ps_stat_icmpv6_s_type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_STAT_ICMPV6_MSGS_RX               = 0,
  PS_STAT_ICMPV6_MSGS_DROPPED_RX       = 1,
  PS_STAT_ICMPV6_DEST_UNREACH_RX       = 2,
  PS_STAT_ICMPV6_ECHO_RX               = 3,
  PS_STAT_ICMPV6_ECHO_REPLY_RX         = 4,
  PS_STAT_ICMPV6_BAD_CHKSUM_RX         = 5,
  PS_STAT_ICMPV6_UNSUPPORTED_TYPE_RX   = 6,
  PS_STAT_ICMPV6_PARM_PROBS_RX         = 7,
  PS_STAT_ICMPV6_BAD_CODE_RX           = 8,
  PS_STAT_ICMPV6_REDIRECTS_RX          = 9,
  PS_STAT_ICMPV6_TIME_EXCEEDS_RX       = 10,
  PS_STAT_ICMPV6_GRP_QUERY_RX          = 11,
  PS_STAT_ICMPV6_GRP_RESP_RX           = 12,
  PS_STAT_ICMPV6_GRP_REDUCTIONS_RX     = 13,
  PS_STAT_ICMPV6_ROUTERSOLICITS_RX     = 14,
  PS_STAT_ICMPV6_NEIGHBORSOLICITS_RX   = 15,
  PS_STAT_ICMPV6_NEIGHBORADVTS_RX      = 16,
  PS_STAT_ICMPV6_PKT_TOO_BIG_RX        = 17,
  PS_STAT_ICMPV6_ROUTERADVTS_RX        = 18,
  PS_STAT_ICMPV6_MSGS_TX               = 19,
  PS_STAT_ICMPV6_DEST_UNREACH_TX       = 20,
  PS_STAT_ICMPV6_ECHO_TX               = 21,
  PS_STAT_ICMPV6_ECHO_REPLY_TX         = 22,
  PS_STAT_ICMPV6_PARM_PROBS_TX         = 23,
  PS_STAT_ICMPV6_REDIRECTS_TX          = 24,
  PS_STAT_ICMPV6_TIME_EXCEEDS_TX       = 25,
  PS_STAT_ICMPV6_GRP_QUERY_TX          = 26,
  PS_STAT_ICMPV6_GRP_RESP_TX           = 27,
  PS_STAT_ICMPV6_GRP_REDUCTIONS_TX     = 28,
  PS_STAT_ICMPV6_ROUTERSOLICITS_TX     = 29,
  PS_STAT_ICMPV6_NEIGHBORSOLICITS_TX   = 30,
  PS_STAT_ICMPV6_NEIGHBORADVTS_TX      = 31,
  PS_STAT_ICMPV6_PKT_TOO_BIG_TX        = 32,
  PS_STAT_ICMPV6_ROUTERADVTS_TX        = 33,
  PS_STAT_ICMPV6_RATE_LIMIT_TX         = 34,
  PS_STAT_ICMPV6_MSGS_DROPPED_TX       = 35,
  PS_STAT_ICMPV6_ALL                   = 36
} ps_stat_icmpv6_enum_type ;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_GLOBAL_ICMPV6_C

  Common header defined in ps_stat_logging.h

  Fields are
    global_icmpv6_stats : global ICMPv6 stats
---------------------------------------------------------------------------*/
typedef struct
{
  ps_stat_icmpv6_s_type  global_icmpv6_stats;
} ps_stat_global_icmpv6_log_pkt_type;

/*===========================================================================
MACRO ICMPV6_INC_STATS()

DESCRIPTION
  This macro increments the icmpv6 statistic(field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ICMPV6_INC_STATS(FIELD, INC)    \
    PS_COMMON_INC_STATS(icmpv6_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_GET_ICMPV6()

DESCRIPTION

  This function supplies the required ICMPv6 layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an handle to
  an instance ( For per-instance statistics only).The second argument SHOULD
  BE NULL for icmpv6 since icmpv6 supports global statistics only. The third
  argument points to the memory location where the results will be copied.
  The result (statistic) will be copied to this memory address. The fourth
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
  instance_ptr    - IN - pointer to instance; should be NULL for icmpv6
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_icmpv6
(
  ps_stat_icmpv6_enum_type   stat,
  void                       *instance_ptr,
  void                       *return_value,
  uint16                     ret_len
) ;

/*===========================================================================
FUNCTION PS_STAT_INIT_ICMPV6()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_icmpv6
(
  void
);

#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
#endif /* PS_STAT_ICMPV6_H */

