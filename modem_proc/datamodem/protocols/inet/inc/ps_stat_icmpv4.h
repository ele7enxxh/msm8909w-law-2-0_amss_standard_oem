#ifndef PS_STAT_ICMPV4_H
#define PS_STAT_ICMPV4_H

/*===========================================================================

                          P S _ S T A T _ I C M P V 4 . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to ICMPv4.

EXTERNALIZED FUNCTIONS
  ps_stat_get_icmpv4()
    This function supplies the required icmpv4 stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_stat_icmpv4.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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
  ICMPv4 Statistics - all statistics are global to the whole ICMPv4 layer on
  on phone. No 'Per Instance' statistics
---------------------------------------------------------------------------*/
typedef struct{
  uint32  msgs_rx ;             /* # of incoming icmp messages */
                                /* type: uint32 */
  uint32  msgs_dropped_rx ;     /* # of incoming icmp messages dropped */
                                /* type: uint32 */
  uint32  addr_mask_replies_rx ;/* # of incoming addr mask replies */
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  addr_masks_rx ;       /* # of incoming address masks */
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  dest_unreach_rx ;     /* # of incoming destination unreachables*/
                                /* type: uint32 */
  uint32  echo_rx ;             /* # of incoming echo mesgs*/
                                /* type: uint32 */
  uint32  echo_reply_rx ;       /* # of incoming echo replies*/
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
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
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  src_quench_rx ;       /* # of incoming source quench messages*/
                                /* type: uint32 */
  uint32  time_exceeds_rx ;     /* # of incoming time exceeded messages*/
                                /* type: uint32 */
  uint32  tstamp_reply_rx ;     /* # of incoming timestamp replies*/
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  tstamp_rx ;           /* # of incoming timestamps*/
                                /* type: uint32 */
  uint32  msgs_tx ;             /* # of outgoing icmp messages*/
                                /* type: uint32 */
  uint32  addr_mask_replies_tx ;/* # of outgoing addr mask replies*/
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  addr_masks_tx ;       /* # of outgoing address masks*/
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
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
  uint32  src_quench_tx ;       /* # of outgoing source quench messages*/
                                /* type: uint32 */
  uint32  time_exceeds_tx ;     /* # of outgoing time exceeded messages*/
                                /* type: uint32 */
  uint32  tstamp_reply_tx ;     /* # of outgoing timestamp replies*/
                                /* type: uint32 */
  uint32  tstamp_tx ;           /* # of outgoing timestamps*/
                                /* type: uint32 */
                                /* note: NOT IMPLEMENTED CURRENTLY*/
  uint32  rate_limit_tx ;       /* # of unsent messages due to rate limit*/
                                /* type: uint32 */
  uint32  msgs_dropped_tx ;     /* # of outgoing messages dropped*/
                                /* type: uint32 */
} ps_stat_icmpv4_s_type ;

/*---------------------------------------------------------------------------
  extern declaration for icmpv4_stats ; defined in ps_stat_icmpv4.c
---------------------------------------------------------------------------*/
extern ps_stat_icmpv4_s_type icmpv4_stats ;

/*---------------------------------------------------------------------------
 For description of the enum values, please see the corr entry in the struct
 definition of ps_stat_icmpv4_s_type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_STAT_ICMPV4_MSGS_RX               = 0,
  PS_STAT_ICMPV4_MSGS_DROPPED_RX       = 1,
  PS_STAT_ICMPV4_ADDR_MASK_REPLIES_RX  = 2,
  PS_STAT_ICMPV4_ADDR_MASKS_RX         = 3,
  PS_STAT_ICMPV4_DEST_UNREACH_RX       = 4,
  PS_STAT_ICMPV4_ECHO_RX               = 5,
  PS_STAT_ICMPV4_ECHO_REPLY_RX         = 6,
  PS_STAT_ICMPV4_BAD_CHKSUM_RX         = 7,
  PS_STAT_ICMPV4_UNSUPPORTED_TYPE_RX   = 8,
  PS_STAT_ICMPV4_PARM_PROBS_RX         = 9,
  PS_STAT_ICMPV4_BAD_CODE_RX           = 10,
  PS_STAT_ICMPV4_REDIRECTS_RX          = 11,
  PS_STAT_ICMPV4_SRC_QUENCH_RX         = 12,
  PS_STAT_ICMPV4_TIME_EXCEEDS_RX       = 13,
  PS_STAT_ICMPV4_TSTAMP_REPLY_RX       = 14,
  PS_STAT_ICMPV4_TSTAMP_RX             = 15,
  PS_STAT_ICMPV4_MSGS_TX               = 16,
  PS_STAT_ICMPV4_ADDR_MASK_REPLIES_TX  = 17,
  PS_STAT_ICMPV4_ADDR_MASKS_TX         = 18,
  PS_STAT_ICMPV4_DEST_UNREACH_TX       = 19,
  PS_STAT_ICMPV4_ECHO_TX               = 20,
  PS_STAT_ICMPV4_ECHO_REPLY_TX         = 21,
  PS_STAT_ICMPV4_PARM_PROBS_TX         = 22,
  PS_STAT_ICMPV4_REDIRECTS_TX          = 23,
  PS_STAT_ICMPV4_SRC_QUENCH_TX         = 24,
  PS_STAT_ICMPV4_TIME_EXCEEDS_TX       = 25,
  PS_STAT_ICMPV4_TSTAMP_REPLY_TX       = 26,
  PS_STAT_ICMPV4_TSTAMP_TX             = 27,
  PS_STAT_ICMPV4_RATE_LIMIT_TX         = 28,
  PS_STAT_ICMPV4_MSGS_DROPPED_TX       = 29,
  PS_STAT_ICMPV4_ALL                   = 30
} ps_stat_icmpv4_enum_type ;

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_GLOBAL_ICMPV4_C

  Common header defined in ps_stat_logging.h

  Fields are
    global_icmpv4_stats : global ICMPv4 stats
---------------------------------------------------------------------------*/
typedef struct
{
  ps_stat_icmpv4_s_type  global_icmpv4_stats;
} ps_stat_global_icmpv4_log_pkt_type;

/*===========================================================================
MACRO ICMPV4_INC_STATS(field, inc)

DESCRIPTION
  This macro increments the icmpv4 statistic(field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ICMPV4_INC_STATS(FIELD, INC)    \
    PS_COMMON_INC_STATS(icmpv4_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_ICMPV4()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_icmpv4
(
  void
);

/*===========================================================================

FUNCTION PS_STAT_GET_ICMPV4()

DESCRIPTION

  This function supplies the required ICMPv4 layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an handle to
  an instance ( For per-instance statistics only).The second argument SHOULD
  BE NULL for icmpv4 since icmpv4 supports global statistics only. The third
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
  instance_ptr    - IN - pointer to instance; should be NULL for icmpv4
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/

errno_enum_type ps_stat_get_icmpv4
(
  ps_stat_icmpv4_enum_type   stat,
  void                       *instance_ptr,
  void                       *return_value,
  uint16                     ret_len
) ;
#endif /* PS_STAT_ICMPV4_H */

