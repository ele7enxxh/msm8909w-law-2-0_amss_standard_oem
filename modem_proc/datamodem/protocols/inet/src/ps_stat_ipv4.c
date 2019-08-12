/*===========================================================================

                          P S _ S T A T _ I P V 4 . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the IPv4 layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_ipv4
    This function supplies the required ipv4 stats.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_stat_ipv4.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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

#include "ps_stat_ipv4.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ds_Utils_DebugMsg.h"

#include <string.h>
#include <stddef.h>

/*---------------------------------------------------------------------------
 Statistics declaration
---------------------------------------------------------------------------*/
ps_stat_ipv4_s_type ipv4_stats  =
{
  0,         /*  bad_hdr_len      */
  0,         /*  bad_len          */
  0,         /*  bad_options      */
  0,         /*  bad_version      */
  0,         /*  bad_chksum       */
  0,         /*  too_short        */
  0,         /*  no_route         */
  0,         /*  no_proto         */
  0,         /*  pkts_rx          */
  0,         /*  pkts_dropped_rx  */
  0,         /*  pkts_dropped_tx  */
  0,         /*  pkts_fwd         */
  0,         /*  pkts_tx          */
  0,         /*  mcast_rx         */
  0,         /*  mcast_tx         */
  0,         /*  frag_ok          */
  0,         /*  frag_fails       */
  0,         /*  frag_create      */
  0,         /*  reasm_reqd       */
  0,         /*  reasm_ok         */
  0,         /*  reasm_fails      */
  0          /*  reasm_timeout    */
};

/*===========================================================================
MACRO IPV4_STAT_OFFSET_SIZE(INDEX,FIELD)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_ipv4_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IPV4_STAT_OFFSET_SIZE(INDEX,FIELD)                              \
{																		\
  FPOS_STAT(ps_stat_ipv4_s_type,FIELD), 								\
  FSIZ(ps_stat_ipv4_s_type,FIELD)										\
}

/*---------------------------------------------------------------------------
 Now, we declare a table that will be used to get the necessary statistic
 based on the enumeration value passed. This is done to avoid the
 switch-case structure that would have been necessary to return the
 appropriate member of the ipv4_stats structure.
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_ipv4_table[PS_STAT_IPV4_ALL + 1] = 
{
  IPV4_STAT_OFFSET_SIZE(0,bad_hdr_len	  ),
  IPV4_STAT_OFFSET_SIZE(1,bad_len		  ),
  IPV4_STAT_OFFSET_SIZE(2,bad_options	  ),
  IPV4_STAT_OFFSET_SIZE(3,bad_version	  ),
  IPV4_STAT_OFFSET_SIZE(4,bad_chksum	  ),
  IPV4_STAT_OFFSET_SIZE(5,too_short 	  ),
  IPV4_STAT_OFFSET_SIZE(6,no_route		  ),
  IPV4_STAT_OFFSET_SIZE(7,no_proto		  ),
  IPV4_STAT_OFFSET_SIZE(8,pkts_rx		  ),
  IPV4_STAT_OFFSET_SIZE(9,pkts_dropped_rx ),
  IPV4_STAT_OFFSET_SIZE(10,pkts_dropped_tx),
  IPV4_STAT_OFFSET_SIZE(11,pkts_fwd 	  ),
  IPV4_STAT_OFFSET_SIZE(12,pkts_tx		  ),
  IPV4_STAT_OFFSET_SIZE(13,mcast_rx 	  ),
  IPV4_STAT_OFFSET_SIZE(14,mcast_tx 	  ),
  IPV4_STAT_OFFSET_SIZE(15,frag_ok		  ),
  IPV4_STAT_OFFSET_SIZE(16,frag_fails	  ),
  IPV4_STAT_OFFSET_SIZE(17,frag_create	  ),
  IPV4_STAT_OFFSET_SIZE(18,reasm_reqd	  ),
  IPV4_STAT_OFFSET_SIZE(19,reasm_ok 	  ),
  IPV4_STAT_OFFSET_SIZE(20,reasm_fails	  ),
  IPV4_STAT_OFFSET_SIZE(21,reasm_timeout  ),
  {0, sizeof(ps_stat_ipv4_s_type)} /* Boundary for ps_stat_ipv4_table */
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static void ps_stat_get_ipv4_global_stat_log_pkt
(
  int32                         handle
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
  ps_stat_ipv4_enum_type  stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                  ret_len
)
{

  /*-------------------------------------------------------------------------
    Input Validations
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Instance_ptr validation : should be NULL for ipv4 stats
  -------------------------------------------------------------------------*/

  if (instance_ptr != NULL)
  {
    LOG_MSG_ERROR_0(" Instance pointer should be NULL for IPV4 stats");
    return E_INVALID_ARG ;
  }

  return ps_stat_get_common((uint32)stat,
                            ps_stat_ipv4_table,
                            (uint16)PS_STAT_IPV4_ALL,
                            &ipv4_stats,
                            return_value,
                            ret_len
                           );
}


/*===========================================================================
FUNCTION PS_STAT_GET_IPV4_GLOBAL_STAT_LOG_PKT()

DESCRIPTION
  This function fills the global IPv4 statistics into a log packet.

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
void ps_stat_get_ipv4_global_stat_log_pkt
(
  int32                         handle
)
{
  uint8                                * log_pkt_ptr             = NULL;
  ps_stat_global_ipv4_log_pkt_type     * global_ipv4_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_stat_logging_get_log_pkt(sizeof(ps_stat_global_ipv4_log_pkt_type),
                              LOG_PS_STAT_GLOBAL_IPV4_C,
                              &log_pkt_ptr);
      
  if (NULL == log_pkt_ptr)
  {
    LOG_MSG_ERROR_0("Log packet allocated for IPV4 failed");
    return;
  }

  global_ipv4_log_pkt_ptr = (ps_stat_global_ipv4_log_pkt_type *) log_pkt_ptr;

  (void) ps_stat_get_ipv4(PS_STAT_IPV4_ALL,
                         NULL,
                         &(global_ipv4_log_pkt_ptr->global_ipv4_stats),
                         sizeof(ps_stat_ipv4_s_type));

  ps_stat_logging_commit_log_pkt
  (
    log_pkt_ptr,
    sizeof(ps_stat_global_ipv4_log_pkt_type)
  );

  return;
} /* ps_stat_get_ipv4_global_stat_log_pkt */


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
)
{
  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_GLOBAL_IPV4,
                                          ps_stat_get_ipv4_global_stat_log_pkt);
}



