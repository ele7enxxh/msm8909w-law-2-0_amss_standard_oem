/*===========================================================================

                          P S _ S T A T _ U D P . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the udp layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_get_udp_stats
    This function supplies the required udp stats.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_stat_udp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/27/10   rp      RAM optimization changes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/23/09   cp      Lint Fixes.
09/09/09   ss      Critical section released for error cases.
09/04/08   am      Lint fixes.
04/11/07   scb     Fixed High LINT errors
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

#include "ps_stat_udp.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"

#include "ps_ip.h"
#include "ps_udp_io.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

#include <stddef.h>
#include <string.h>


/*===========================================================================

                             INTERNAL DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO UDP_STAT_OFFSET_SIZE(INDEX,FIELD)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_udp_g_table.
  INDEX is used only for readability, not for usage.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define UDP_STAT_OFFSET_SIZE(INDEX,FIELD)                               \
{																		\
  FPOS_STAT(ps_stat_udp_g_s_type,FIELD),								\
  FSIZ(ps_stat_udp_g_s_type,FIELD)										\
}

/*===========================================================================
MACRO UDP_INSTANCE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_udp_i_table.
  INDEX is used only for readability, not for usage.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define UDP_INSTANCE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                 \
{																		\
  FPOS_STAT(struct udp_cb,FIELD),										\
  sizeof(SIZE)															\
}

/*---------------------------------------------------------------------------
 Statistics declaration
---------------------------------------------------------------------------*/
ps_stat_udp_g_s_type udp_stats  =
{

 0,         /*  bad_len          */
 0,         /*  bad_chksum       */
 0,         /*  dgram_rx         */
 0,         /*  dgram_tx         */
 0,         /*  bytes_rx         */
 0,         /*  bytes_tx         */
 0,         /*  no_port          */
 0,         /*  dgram_dropped_rx */
 0          /*  dgram_dropped_tx */
};

/*---------------------------------------------------------------------------
  Global statistics table:
  Now, we declare a table that will be used to get the necessary statistic
  based on the enumeration value passed. This is done to avoid the
  switch-case structure that would have been necessary to return the
  appropriate member of the udp_stats structure.
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_udp_g_table[PS_STAT_UDP_ALL + 1] = 
{
  UDP_STAT_OFFSET_SIZE(0,bad_len		  ),
  UDP_STAT_OFFSET_SIZE(1,bad_chksum 	  ),
  UDP_STAT_OFFSET_SIZE(2,dgram_rx		  ),
  UDP_STAT_OFFSET_SIZE(3,dgram_tx		  ),
  UDP_STAT_OFFSET_SIZE(4,bytes_rx		  ),
  UDP_STAT_OFFSET_SIZE(5,bytes_tx		  ),
  UDP_STAT_OFFSET_SIZE(6,no_port		  ),
  UDP_STAT_OFFSET_SIZE(7,dgram_dropped_rx ),
  UDP_STAT_OFFSET_SIZE(8,dgram_dropped_tx ),
  /*---------------------------------------------------------------------------
  Indices from 9(PS_STAT_UDP_LOCAL_IP) to 12(PS_STAT_UDP_REMOTE_PORT) 
  are unused. Fill with zeros
  - Refer ps_stat_udp_enum_type
  ---------------------------------------------------------------------------*/
  STAT_FILL_NULL(9),
  STAT_FILL_NULL(10),
  STAT_FILL_NULL(11),
  STAT_FILL_NULL(12),
  {0, sizeof(ps_stat_udp_g_s_type)} /* Boundary for ps_stat_udp_g_table */
};

/*---------------------------------------------------------------------------
  Access table for Instance statistics
---------------------------------------------------------------------------*/
static const ps_stat_access_table_s_type ps_stat_udp_i_table[PS_STAT_UDP_ALL + 1] = 
{
  /*---------------------------------------------------------------------------
  Indices 0,1,6,8 and 13 are unused. Fill with zeros
  - Refer ps_stat_udp_enum_type
  ---------------------------------------------------------------------------*/
  STAT_FILL_NULL(0),
  STAT_FILL_NULL(1),
  UDP_INSTANCE_STAT_OFFSET_SIZE(2,udp_instance_stats.dgram_rx,            uint32),
  UDP_INSTANCE_STAT_OFFSET_SIZE(3,udp_instance_stats.dgram_tx,            uint32),
  UDP_INSTANCE_STAT_OFFSET_SIZE(4,udp_instance_stats.bytes_rx,            uint64),
  UDP_INSTANCE_STAT_OFFSET_SIZE(5,udp_instance_stats.bytes_tx,            uint64),
  STAT_FILL_NULL(6),
  UDP_INSTANCE_STAT_OFFSET_SIZE(7,udp_instance_stats.dgram_dropped_rx,    uint32),
  STAT_FILL_NULL(8),
  UDP_INSTANCE_STAT_OFFSET_SIZE(9,conn.local.address,         struct ps_in6_addr),
  UDP_INSTANCE_STAT_OFFSET_SIZE(10,conn.local.port,                       uint16),
  UDP_INSTANCE_STAT_OFFSET_SIZE(11,conn.remote.address,       struct ps_in6_addr),
  UDP_INSTANCE_STAT_OFFSET_SIZE(12,conn.remote.port,                      uint16),
  STAT_FILL_NULL(13)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_udp_instance_all
(
  ps_stat_udp_enum_type  stat,
  struct udp_cb          *ucb_ptr,
  void                   *return_value,
  uint16                 ret_len
);

static errno_enum_type ps_stat_get_udp_control_block
(
  int               handle,
  struct udp_cb  ** ctrl_blk_ptr
);


static void ps_stat_get_udp_desc_log_pkt
(
  int32               handle
);

static void ps_stat_get_udp_inst_stat_log_pkt
(
  int32                            handle
);

static void ps_stat_get_udp_global_stat_log_pkt
(
  int32                            handle
);
/*===========================================================================
FUNCTION PS_STAT_GET_UDP_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the UDP statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the udp control block

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  ucb_ptr         - IN - pointer that points to the UDP control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_udp_instance_all
(
  ps_stat_udp_enum_type  stat,
  struct udp_cb          *ucb_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
   ps_stat_udp_i_s_type *udp_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat' ; only PS_STAT_UDP_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_UDP_ALL || (ucb_ptr == NULL) )
  {
    DATA_ERR_FATAL("ps_stat_get_udp_instance_all: Invalid stat ir NULL udp_ptr");
    return E_NOT_ALLOWED ;
  }


  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_udp_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  udp_i_stats = (ps_stat_udp_i_s_type *) return_value ;

  /*-------------------------------------------------------------------------
    Copy the stats from udp control block
  -------------------------------------------------------------------------*/
  udp_i_stats->dgram_rx = ucb_ptr->udp_instance_stats.dgram_rx;
  udp_i_stats->dgram_tx = ucb_ptr->udp_instance_stats.dgram_tx;
  udp_i_stats->bytes_rx = ucb_ptr->udp_instance_stats.bytes_rx;
  udp_i_stats->bytes_tx = ucb_ptr->udp_instance_stats.bytes_tx;
  udp_i_stats->dgram_dropped_rx =
                                ucb_ptr->udp_instance_stats.dgram_dropped_rx;
  udp_i_stats->local_ip = ucb_ptr->conn.local.address;
  udp_i_stats->remote_ip = ucb_ptr->conn.remote.address;
  udp_i_stats->local_port = ucb_ptr->conn.local.port;
  udp_i_stats->remote_port = ucb_ptr->conn.remote.port;

  ucb_ptr->udp_instance_stats.queried = PS_STAT_QUERIED;
  return E_SUCCESS ;
} /* ps_stat_get_udp_instance_all */


/*===========================================================================
FUNCTION PS_STAT_GET_UDP_CONTRL_BLOCK()

DESCRIPTION
  This function converts a QXDM handle into a UDP control block pointer.
  It is necessary to convert a QXDM handle into a control block pointer here
  because the common stats module does not have access to the dssocket fctns.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle          - IN - sockfd of the ctrl blk of interest
  ctrl_blk_ptr    - IN - pointer for return value
                  - OUT- return value (ctrl blk ptr)

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_udp_control_block
(
  int               handle,
  struct udp_cb  ** ctrl_blk_ptr
)
{
  return E_NOT_SUPPORTED;
} /* ps_stat_get_udp_control_block */

/*===========================================================================
FUNCTION PS_STAT_GET_UDP_INST_DESC()

DESCRIPTION
  This function fills the description of one UDP instance into a log packet.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from
  *log_pkt_ptr    : Pointer to the log packet that the description will be
                    copied to

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_GET_UDP_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_udp_desc_log_pkt
(
  int32               handle
)
{
  return;

} /* ps_stat_get_udp_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_UDP_INST_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all instances, it
  sends one log packet for each instance that is active on the phone.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_udp_inst_stat_log_pkt
(
  int32                            handle
)
{
  return;

} /* ps_stat_get_udp_inst_stat_log_pkt */

/*===========================================================================
FUNCTION PS_STAT_GET_UDP_GLOBAL_STAT_LOG_PKT()

DESCRIPTION
  This function fills the global UDP statistics into a log packet.

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
void ps_stat_get_udp_global_stat_log_pkt
(
  int32                            handle
)
{
  uint8                               * log_pkt_ptr            = NULL;
  ps_stat_global_udp_log_pkt_type     * global_udp_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_stat_logging_get_log_pkt(sizeof(ps_stat_global_udp_log_pkt_type),
                              LOG_PS_STAT_GLOBAL_UDP_C,
                              &log_pkt_ptr);

  if (NULL == log_pkt_ptr)
  {
    LOG_MSG_ERROR_0("Log packet allocated for UDP failed");
    return;
  }

  global_udp_log_pkt_ptr = (ps_stat_global_udp_log_pkt_type *) log_pkt_ptr;

  (void) ps_stat_get_udp(PS_STAT_UDP_ALL,
                         NULL,
                         &(global_udp_log_pkt_ptr->global_udp_stats),
                         sizeof(ps_stat_udp_g_s_type));

  ps_stat_logging_commit_log_pkt
  (
    log_pkt_ptr,
    sizeof(ps_stat_global_udp_log_pkt_type)
  );

  return;
} /* ps_stat_get_udp_global_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

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
  handle : handle to the instance to be reset

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_udp
(
  int32                 handle
)
{
  struct udp_cb       * udp_cb_ptr;
  errno_enum_type       ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = ps_stat_get_udp_control_block(handle, &udp_cb_ptr);

  if (ret != E_SUCCESS)
  {
    LOG_MSG_ERROR_1("Unable to get UDP control block for reset.  Handle: %d",
                      handle);
    return;
  }

  if (PS_STAT_QUERIED == udp_cb_ptr->udp_instance_stats.queried)
  {
    ps_stat_get_udp_inst_stat_log_pkt(handle);
  }

  memset(&(udp_cb_ptr->udp_instance_stats),
         0,
         sizeof(udp_cb_ptr->udp_instance_stats));

  return;
} /* ps_stat_reset_udp */


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
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_UDP,
                                           ps_stat_get_udp_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_UDP,
                                          ps_stat_get_udp_inst_stat_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_GLOBAL_UDP,
                                          ps_stat_get_udp_global_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_UDP,
                                            ps_stat_reset_udp);

} /* ps_stat_init_udp */


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
  ps_stat_udp_enum_type  stat,
  void                   *instance_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check Instance pointer; If it is NULL, call ps_satat_get_common with
    global access table
  -------------------------------------------------------------------------*/

  if ( instance_ptr == NULL)
  {
    return ps_stat_get_common((uint32)stat,
                              ps_stat_udp_g_table,
                              (uint16)PS_STAT_UDP_ALL,
                              &udp_stats,
                              return_value,
                              ret_len
                             );
  }
  else
  {
    if (stat == PS_STAT_UDP_ALL)
    {
      return ps_stat_get_udp_instance_all(stat,
                                          (struct udp_cb *)(instance_ptr),
                                          return_value,
                                          ret_len
                                         );
    }
    else
    {
      return ps_stat_get_common((uint32)stat,
                                ps_stat_udp_i_table,
                                (uint16)PS_STAT_UDP_ALL,
                                instance_ptr,
                                return_value,
                                ret_len
                               );
    }
  }
} /* ps_stat_get_udp */


