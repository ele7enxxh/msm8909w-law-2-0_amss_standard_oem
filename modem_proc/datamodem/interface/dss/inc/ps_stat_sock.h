#ifndef PS_STAT_SOCK_H
#define PS_STAT_SOCK_H

/*===========================================================================

                          P S _ S T A T _ S O C K . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to sockets.

EXTERNALIZED FUNCTIONS
  ps_stat_get_sock()
    This function supplies the required socket stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/ps_stat_sock.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/10   rp      RAM optimization changes.
11/28/08   pp      SOCK_UPDATE_MAX_OPEN_STAT: Used more synchronized construct
                   for critical section.
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
#include "ps_stat_common.h"
#include "ps_crit_sect.h"

/*---------------------------------------------------------------------------
  Socket Statistics - all statistics are global to the whole sockets layer on
  on phone. No 'Per Instance' statistics
---------------------------------------------------------------------------*/

typedef struct
{
  uint32  open_socks           ;/*# of currently open sockets ( all types) */
                                /*type :uint32 */
  uint32  open_tcp_socks       ;/*# of currently open tcp sockets */
                                /*type :uint32 */
  uint32  open_udp_socks       ;/*# of currently open udp sockets */
                                /*type :uint32 */
  uint32  open_icmp_socks      ;/*# of currently open icmp sockets */
                                /*type :uint32 */
  uint32  total_socks_allocated;/*Total # of sockets ( past and currently */
                                /*open sockets) */
                                /*type :uint32 */
  uint32  tcp_socks_allocated  ;/*Total # of tcp sockets (past and  */
                                /*currently open sockets)*/
                                /*type :uint32 */
  uint32  udp_socks_allocated  ;/*Total # of udp sockets (past and  */
                                /*currently open sockets)*/
                                /*type :uint32 */
  uint32  icmp_socks_allocated ;/*Total # of icmp sockets (past and  */
                                /*currently open sockets)*/
                                /*type :uint32 */
  uint64  bytes_tx             ;/*Total # of bytes sent on all sockets */
                                /*type :uint64 */
  uint64  bytes_rx             ;/*Total # of bytes received on all sockets */
                                /*type :uint64 */
  uint32  scb_alloc_failures   ;/*# of socket control block allocation */
                                /*failures */
                                /*type :uint32 */
  uint32  tcb_alloc_failures   ;/*# of tcp control block allocation */
                                /*failures*/
                                /*type :uint32 */
  uint32  ucb_alloc_failures   ;/*# of udp control block allocation */
                                /*failures*/
                                /*type :uint32 */
  uint32  max_open_tcp_socks   ;/*max # of simulataneous open tcp socks*/
                                /*type :uint32 */
  uint32  max_open_udp_socks   ;/*max # of simulataneous open udp socks*/
                                /*type :uint32 */
  uint32  max_open_icmp_socks  ;/*max # of simulataneous open icmp socks*/
                                /*type :uint32 */
} ps_stat_sock_s_type ;

/*---------------------------------------------------------------------------
  extern declaration for sock_stats ; defined in ps_stat_sock.c
---------------------------------------------------------------------------*/
extern ps_stat_sock_s_type sock_stats ;

/*---------------------------------------------------------------------------
 For description of the enum values, please see the corr entry in the struct
 definition of ps_stat_sock_s_type
---------------------------------------------------------------------------*/

typedef enum
{
  PS_STAT_SOCK_OPEN_SOCKS            = 0,
  PS_STAT_SOCK_OPEN_TCP_SOCKS        = 1,
  PS_STAT_SOCK_OPEN_UDP_SOCKS        = 2,
  PS_STAT_SOCK_OPEN_ICMP_SOCKS       = 3,
  PS_STAT_SOCK_TOTAL_SOCKS_ALLOCATED = 4,
  PS_STAT_SOCK_TCP_SOCKS_ALLOCATED   = 5,
  PS_STAT_SOCK_UDP_SOCKS_ALLOCATED   = 6,
  PS_STAT_SOCK_ICMP_SOCKS_ALLOCATED  = 7,
  PS_STAT_SOCK_BYTES_TX              = 8,
  PS_STAT_SOCK_BYTES_RX              = 9,
  PS_STAT_SOCK_SCB_ALLOC_FAILURES    = 10,
  PS_STAT_SOCK_TCB_ALLOC_FAILURES    = 11,
  PS_STAT_SOCK_UCB_ALLOC_FAILURES    = 12,
  PS_STAT_SOCK_MAX_OPEN_TCP_SOCKS    = 13,
  PS_STAT_SOCK_MAX_OPEN_UDP_SOCKS    = 14,
  PS_STAT_SOCK_MAX_OPEN_ICMP_SOCKS   = 15,
  PS_STAT_SOCK_ALL                   = 16
} ps_stat_sock_enum_type ;


/*===========================================================================
MACRO SOCK_INC_STATS()

DESCRIPTION
  This macro increments the sock statistic(field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define SOCK_INC_STATS(FIELD, INC)     \
    PS_COMMON_INC_STATS(sock_stats.FIELD, 4, INC)

/*===========================================================================
MACRO SOCK_DEC_STATS()

DESCRIPTION
  This macro decrements the sock statistic(field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define SOCK_DEC_STATS(FIELD, DEC)     \
    PS_COMMON_DEC_STATS(sock_stats.FIELD, 4, DEC)

/*===========================================================================
MACRO SOCK_COMPARE_MAX(TYPE)

DESCRIPTION
  This macro compares the current number of open sockets of type TYPE with
  the max number of open sockets of type TYPE updates max opne sockets if
  necessary

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define SOCK_UPDATE_MAX_OPEN_STAT(TYPE)                                     \
{                                                                           \
  /*lint -e{506, 774} */                                                    \
  if(sizeof(sock_stats.max_open_##TYPE##_socks) == 4)                       \
  {                                                                         \
    if (sock_stats.open_##TYPE##_socks > sock_stats.max_open_##TYPE##_socks)\
    {                                                                       \
      sock_stats.max_open_##TYPE##_socks =  sock_stats.open_##TYPE##_socks ;\
    }                                                                       \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                           \
    if (sock_stats.open_##TYPE##_socks > sock_stats.max_open_##TYPE##_socks)\
    {                                                                       \
      sock_stats.max_open_##TYPE##_socks =  sock_stats.open_##TYPE##_socks ;\
    }                                                                       \
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                         \
  }                                                                         \
}

/*===========================================================================
FUNCTION PS_STAT_GET_SOCK()

DESCRIPTION
  This function supplies the required sock layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an handle to
  an instance ( For per-instance statistics only).The second argument SHOULD
  BE NULL for sockets since sockets supports global statistics only.The third
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
  instance_ptr    - IN - pointer to instance; should be NULL for sockets
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_sock
(
  ps_stat_sock_enum_type     stat,
  void                       *instance_ptr,
  void                       *return_value,
  uint16                     ret_len
) ;
#endif /* PS_STAT_SOCK_H */

