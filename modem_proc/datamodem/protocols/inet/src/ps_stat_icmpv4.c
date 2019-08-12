/*===========================================================================

                          P S _ S T A T _ I C M P V 4 . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the ICMPv4 layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_icmpv4
    This function supplies the required icmpv4 stats.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_stat_icmpv4.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/10   rp      RAM optimization changes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/23/09   cp      Lint Fixes
02/27/07   scb     Added changes to fix RCVT compilation warning
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_icmpv4.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ds_Utils_DebugMsg.h"

#include "stddef.h"
#include <string.h>


/*---------------------------------------------------------------------------
 Statistics declaration
---------------------------------------------------------------------------*/
ps_stat_icmpv4_s_type icmpv4_stats  =
{
  0,         /*  msgs_rx               */
  0,         /*  msgs_dropped_rx       */
  0,         /*  addr_mask_replies_rx  */
  0,         /*  addr_masks_rx         */
  0,         /*  dest_unreach_rx       */
  0,         /*  echo_rx               */
  0,         /*  echo_reply_rx         */
  0,         /*  bad_chksum_rx         */
  0,         /*  unsupported_type_rx   */
  0,         /*  parm_probs_rx         */
  0,         /*  bad_code_rx           */
  0,         /*  redirects_rx          */
  0,         /*  src_quench_rx         */
  0,         /*  time_exceeds_rx       */
  0,         /*  tstamp_reply_rx       */
  0,         /*  tstamp_rx             */
  0,         /*  msgs_tx               */
  0,         /*  addr_mask_replies_tx  */
  0,         /*  addr_masks_tx         */
  0,         /*  dest_unreach_tx       */
  0,         /*  echo_tx               */
  0,         /*  echo_reply_tx         */
  0,         /*  parm_probs_tx         */
  0,         /*  redirects_tx          */
  0,         /*  src_quench_tx         */
  0,         /*  time_exceeds_tx       */
  0,         /*  tstamp_reply_tx       */
  0,         /*  tstamp_tx             */
  0,         /*  rate_limit_tx         */
  0          /*  msgs_dropped_tx       */
};

/*===========================================================================
MACRO ICMPV4_STAT_OFFSET_SIZE(INDEX,FIELD)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_icmpv4_table.
  INDEX is used only for readability, not for usage.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ICMPV4_STAT_OFFSET_SIZE(INDEX,FIELD)                            \
{																		\
  FPOS_STAT(ps_stat_icmpv4_s_type, FIELD),								\
  FSIZ(ps_stat_icmpv4_s_type, FIELD)									\
}

/*---------------------------------------------------------------------------
 Now, we declare a table that will be used to get the necessary statistic
 based on the enumeration value passed. This is done to avoid the
 switch-case structure that would have been necessary to return the
 appropriate member of the icmpv4_stats structure.
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_icmpv4_table[PS_STAT_ICMPV4_ALL + 1] = 
{
  ICMPV4_STAT_OFFSET_SIZE(0,msgs_rx                ),
  ICMPV4_STAT_OFFSET_SIZE(1,msgs_dropped_rx 	   ),
  ICMPV4_STAT_OFFSET_SIZE(2,addr_mask_replies_rx   ),
  ICMPV4_STAT_OFFSET_SIZE(3,addr_masks_rx          ),
  ICMPV4_STAT_OFFSET_SIZE(4,dest_unreach_rx 	   ),
  ICMPV4_STAT_OFFSET_SIZE(5,echo_rx                ),
  ICMPV4_STAT_OFFSET_SIZE(6,echo_reply_rx          ),
  ICMPV4_STAT_OFFSET_SIZE(7,bad_chksum_rx          ),
  ICMPV4_STAT_OFFSET_SIZE(8,unsupported_type_rx    ),
  ICMPV4_STAT_OFFSET_SIZE(9,parm_probs_rx          ),
  ICMPV4_STAT_OFFSET_SIZE(10,bad_code_rx           ),
  ICMPV4_STAT_OFFSET_SIZE(11,redirects_rx          ),
  ICMPV4_STAT_OFFSET_SIZE(12,src_quench_rx         ),
  ICMPV4_STAT_OFFSET_SIZE(13,time_exceeds_rx       ),
  ICMPV4_STAT_OFFSET_SIZE(14,tstamp_reply_rx       ),
  ICMPV4_STAT_OFFSET_SIZE(15,tstamp_rx             ),
  ICMPV4_STAT_OFFSET_SIZE(16,msgs_tx               ),
  ICMPV4_STAT_OFFSET_SIZE(17,addr_mask_replies_tx  ),
  ICMPV4_STAT_OFFSET_SIZE(18,addr_masks_tx         ),
  ICMPV4_STAT_OFFSET_SIZE(19,dest_unreach_tx       ),
  ICMPV4_STAT_OFFSET_SIZE(20,echo_tx               ),
  ICMPV4_STAT_OFFSET_SIZE(21,echo_reply_tx         ),
  ICMPV4_STAT_OFFSET_SIZE(22,parm_probs_tx         ),
  ICMPV4_STAT_OFFSET_SIZE(23,redirects_tx          ),
  ICMPV4_STAT_OFFSET_SIZE(24,src_quench_tx         ),
  ICMPV4_STAT_OFFSET_SIZE(25,time_exceeds_tx       ),
  ICMPV4_STAT_OFFSET_SIZE(26,tstamp_reply_tx       ),
  ICMPV4_STAT_OFFSET_SIZE(27,tstamp_tx             ),
  ICMPV4_STAT_OFFSET_SIZE(28,rate_limit_tx         ),
  ICMPV4_STAT_OFFSET_SIZE(29,msgs_dropped_tx       ),
  {0, sizeof(ps_stat_icmpv4_s_type)} /* Boundary for ps_stat_icmpv4_table */
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static void ps_stat_get_icmpv4_global_stat_log_pkt
(
  int32                         handle
);

/*===========================================================================

FUNCTION PS_STAT_GET_ICMPV4()

DESCRIPTION

  This function supplies the required ICMPv4 layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an handle to
  an instance (For per-instance statistics only).The second argument SHOULD
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
  ps_stat_icmpv4_enum_type  stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
)
{

  /*-------------------------------------------------------------------------
    Instance_ptr validation : should be NULL for icmpv4 stats
  -------------------------------------------------------------------------*/

  if (instance_ptr != NULL)
  {
    LOG_MSG_ERROR_0(" Instance pointer should be NULL for ICMPV4 stats");
    return E_INVALID_ARG ;
  }

  return ps_stat_get_common((uint32)stat,
                            ps_stat_icmpv4_table,
                            (uint16)PS_STAT_ICMPV4_ALL,
                            &icmpv4_stats,
                            return_value,
                            ret_len
                           );
} /* ps_stat_get_icmpv4 */


/*===========================================================================
FUNCTION PS_STAT_GET_ICMPV4_GLOBAL_STAT_LOG_PKT()

DESCRIPTION
  This function fills the global ICMPv4 statistics into a log packet.

PARAMETERS
  handle          : not used, required because of the shared function 
                    definition with the instance stats get functions

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_icmpv4_global_stat_log_pkt
(
  int32                         handle
)
{
  uint8                                  * log_pkt_ptr               = NULL;
  ps_stat_global_icmpv4_log_pkt_type     * global_icmpv4_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_stat_logging_get_log_pkt(sizeof(ps_stat_global_icmpv4_log_pkt_type),
                              LOG_PS_STAT_GLOBAL_ICMPV4_C,
                              &log_pkt_ptr);
      
  if (NULL == log_pkt_ptr)
  {
    LOG_MSG_ERROR_0("Log packet allocated for ICMPV4 failed");
    return;
  }

  global_icmpv4_log_pkt_ptr = 
    (ps_stat_global_icmpv4_log_pkt_type *) log_pkt_ptr;

  (void) ps_stat_get_icmpv4(PS_STAT_ICMPV4_ALL,
                         NULL,
                         &(global_icmpv4_log_pkt_ptr->global_icmpv4_stats),
                         sizeof(ps_stat_icmpv4_s_type));

  ps_stat_logging_commit_log_pkt
  (
    log_pkt_ptr,
    sizeof(ps_stat_global_icmpv4_log_pkt_type)
  );

  return;
} /* ps_stat_get_icmpv4_global_stat_log_pkt */


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
)
{
  ps_stat_logging_inst_reg_get_stat_cback
  (
    PS_STAT_MODULE_GLOBAL_ICMPV4,
    ps_stat_get_icmpv4_global_stat_log_pkt
  );
}



