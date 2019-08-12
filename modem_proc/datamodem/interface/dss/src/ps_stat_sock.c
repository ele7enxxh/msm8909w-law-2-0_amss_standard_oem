/*===========================================================================

                          P S _ S T A T _ S O C K . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the sockets layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_sock
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/ps_stat_sock.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/10   rp      RAM optimization changes.
12/23/09   cp      Lint Fixes
02/27/07   scb      Added changes to fix RCVT compilation warning
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "stddef.h"
#include "err.h"
#include "msg.h"
#include "ps_stat_sock.h"
#include "ps_stat_common.h"
#include <string.h>
#include "ds_Utils_DebugMsg.h"

/*---------------------------------------------------------------------------
 Statistics declaration
---------------------------------------------------------------------------*/
ps_stat_sock_s_type sock_stats  =
{
  0,         /*  open_socks             */
  0,         /*  open_tcp_socks         */
  0,         /*  open_udp_socks         */
  0,         /*  open_icmp_socks        */
  0,         /*  total_socks_allocated  */
  0,         /*  tcp_socks_allocated    */
  0,         /*  udp_socks_allocated    */
  0,         /*  icmp_socks_allocated   */
  0,         /*  bytes_tx               */
  0,         /*  bytes_rx               */
  0,         /*  scb_alloc_failures     */
  0,         /*  tcb_alloc_failures     */
  0,         /*  ucb_alloc_failures     */
  0,         /*  max_open_tcp_socks     */
  0,         /*  max_open_udp_socks     */
  0          /*  max_open_icmp_socks    */
};

/*===========================================================================
MACRO SOCK_STAT_OFFSET_SIZE(INDEX,FIELD)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_sock_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/


#define SOCK_STAT_OFFSET_SIZE(INDEX,FIELD)                              \
{                                                                       \
  FPOS_STAT(ps_stat_sock_s_type,FIELD),                                 \
  FSIZ(ps_stat_sock_s_type,FIELD)                                       \
}

/*---------------------------------------------------------------------------
 Now, we declare a table that will be used to get the necessary statistic
 based on the enumeration value passed. This is done to avoid the
 switch-case structure that would have been necessary to return the
 appropriate member of the sock_stats structure.
---------------------------------------------------------------------------*/

const ps_stat_access_table_s_type 
      ps_stat_sock_table[(uint32)PS_STAT_SOCK_ALL + 1] =
{
  SOCK_STAT_OFFSET_SIZE(0,open_socks           ),
  SOCK_STAT_OFFSET_SIZE(1,open_tcp_socks       ),
  SOCK_STAT_OFFSET_SIZE(2,open_udp_socks       ),
  SOCK_STAT_OFFSET_SIZE(3,open_icmp_socks      ),
  SOCK_STAT_OFFSET_SIZE(4,total_socks_allocated),
  SOCK_STAT_OFFSET_SIZE(5,tcp_socks_allocated  ),
  SOCK_STAT_OFFSET_SIZE(6,udp_socks_allocated  ),
  SOCK_STAT_OFFSET_SIZE(7,icmp_socks_allocated ),
  SOCK_STAT_OFFSET_SIZE(8,bytes_tx             ),
  SOCK_STAT_OFFSET_SIZE(9,bytes_rx             ),
  SOCK_STAT_OFFSET_SIZE(10,scb_alloc_failures  ),
  SOCK_STAT_OFFSET_SIZE(11,tcb_alloc_failures  ),
  SOCK_STAT_OFFSET_SIZE(12,ucb_alloc_failures  ),
  SOCK_STAT_OFFSET_SIZE(13,max_open_tcp_socks  ),
  SOCK_STAT_OFFSET_SIZE(14,max_open_udp_socks  ),
  SOCK_STAT_OFFSET_SIZE(15,max_open_icmp_socks ),
  {0, sizeof(ps_stat_sock_s_type)}  /* Boundary of ps_stat_sock_table[] */
};

/*===========================================================================

FUNCTION PS_STAT_GET_SOCK()

DESCRIPTION
  This function supplies the required sockets layer statistics. The first
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
  ps_stat_sock_enum_type    stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
)

{
  /*-------------------------------------------------------------------------
    Input Validations
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Instance_ptr validation : should be NULL for socket stats
  -------------------------------------------------------------------------*/

  if( instance_ptr != NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_sock(): "
                    " Instance pointer should be NULL for Socket stats");
    return E_INVALID_ARG ;
  }

  return ps_stat_get_common((uint32)stat,
                            ps_stat_sock_table,
                            (uint16)PS_STAT_SOCK_ALL,
                            &sock_stats,
                            return_value,
                            ret_len
                           ) ;
} /* ps_stat_get_sock */



