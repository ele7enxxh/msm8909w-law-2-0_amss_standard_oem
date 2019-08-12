/*===========================================================================

                          P S _ S T A T _ F L O W . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the flow layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_flow
    This function supplies the required flow stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_stat_flow.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/10   rp      RAM optimization changes.
04/11/07   scb     Fixed High LINT errors
02/27/07   scb     Added changes to fix RCVT compilation warning
02/27/07   scb     Fixed high LINT errors
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_flow.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"

#include "ps_flow.h"
#include "ps_flowi.h"
#include "ps_iface.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

#include <stddef.h>
#include <string.h>


/*===========================================================================

                             INTERNAL DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO FLOW_STAT_OFFSET_SIZE(INDEX,FIELD, SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_flow_i_table.
  INDEX is used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define FLOW_STAT_OFFSET_SIZE(INDEX,FIELD, SIZE)                \
{																\
  FPOS_STAT(ps_flow_type,FIELD),								\
  sizeof(SIZE)													\
}

/*---------------------------------------------------------------------------
  FLOW stats access table
-------------------------------------------------------------------------- */
const ps_stat_access_table_s_type ps_stat_flow_i_table[PS_STAT_FLOW_ALL + 1] = 
{
  FLOW_STAT_OFFSET_SIZE(0,flow_i_stats.pkts_tx,   uint32),
  FLOW_STAT_OFFSET_SIZE(1,flow_i_stats.bytes_tx,  uint64),
  FLOW_STAT_OFFSET_SIZE(2,flow_private.state,	  uint16),
  /*---------------------------------------------------------------------------
  Indices 3 and 4 are unused. Fill with zeros
  - Refer ps_stat_flow_enum_type
  -------------------------------------------------------------------------- */
  STAT_FILL_NULL(3),
  STAT_FILL_NULL(4)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/

static errno_enum_type ps_stat_get_flow_is_default
(
  ps_stat_flow_enum_type  stat,
  ps_flow_type            *flow_ptr,
  void                    *return_value,
  uint16                  ret_len
);

static errno_enum_type ps_stat_get_flow_instance_all
(
  ps_stat_flow_enum_type  stat,
  ps_flow_type            *flow_ptr,
  void                    *return_value,
  uint16                  ret_len
);

static errno_enum_type ps_stat_get_flow_control_block
(
  int               handle,
  ps_flow_type   ** ps_flow_ptr_ptr
);

static void ps_stat_get_flow_desc_log_pkt
(
  int32                         handle
);

static void ps_stat_get_flow_stat_log_pkt
(
  int32                         handle
);

/*===========================================================================
FUNCTION PS_STAT_GET_FLOW_IS_DEFAULT()

DESCRIPTION
  This function supplies the flow is_default statistic

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  flow_ptr       -  IN - pointer that points to the FLOW control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_flow_is_default
(
  ps_stat_flow_enum_type  stat,
  ps_flow_type            *flow_ptr,
  void                    *return_value,
  uint16                  ret_len
)
{
   uint32 *is_default;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   ps_flow_capability_enum_type flow_capability = PS_FLOW_CAPABILITY_DEFAULT;
  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_FLOW_IS_DEFAULT should be passed to
    this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_FLOW_IS_DEFAULT)
  {
    DATA_ERR_FATAL("ps_stat_get_flow_is_default(): Invalid stat value passed to ps_stat_get_flow_i_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate flow_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (flow_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_stat_get_flow_is_default:Null value of flow_ptr passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_flow_is_default(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(uint32))
  {
    LOG_MSG_ERROR_0("ps_stat_get_flow_is_default(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  is_default = (uint32 *) return_value ;
  *is_default = PS_FLOW_GET_CAPABILITY(flow_ptr,flow_capability);
  return E_SUCCESS ;
} /* ps_stat_get_flow_is_default */


/*===========================================================================
FUNCTION PS_STAT_GET_FLOW_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the FLOW statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the flow control block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  flow_ptr        - IN - pointer that points to the FLOW control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_flow_instance_all
(
  ps_stat_flow_enum_type  stat,
  ps_flow_type            *flow_ptr,
  void                    *return_value,
  uint16                  ret_len
)
{
   ps_stat_flow_i_s_type *flow_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   ps_flow_capability_enum_type flow_capability = PS_FLOW_CAPABILITY_DEFAULT;
  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_FLOW_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_FLOW_ALL)
  {
    DATA_ERR_FATAL("ps_stat_get_flow_instance_all(): Invalid stat value passed to ps_stat_get_flow_i_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate flow_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (flow_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_stat_get_flow_instance_all:Null value of flow_ptr passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_flow_instance_all(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_flow_i_s_type))
  {
    LOG_MSG_ERROR_0("ps_stat_get_flow_instance_all(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  flow_i_stats = (ps_stat_flow_i_s_type *) return_value ;

  flow_i_stats->pkts_tx    = flow_ptr->flow_i_stats.pkts_tx ;
  flow_i_stats->bytes_tx   = flow_ptr->flow_i_stats.bytes_tx;
  flow_i_stats->state      = flow_ptr->flow_private.state ;
  flow_i_stats->is_default = PS_FLOW_GET_CAPABILITY(flow_ptr, flow_capability);

  flow_ptr->flow_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_flow_instance_all */


/*===========================================================================
FUNCTION PS_STAT_GET_FLOW_CONTROL_BLOCK()

DESCRIPTION
  This function converts a QXDM handle into a flow control block pointer.
  It is necessary to convert a QXDM handle into a control block pointer here
  because the common stats module does not have access to the flow fctns.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle          - IN - index of the ctrl blk of interest
  ps_flow_ptr_ptr    - OUT- ctrl blk ptr.  void** so it can be used directly
                         in the associated get functions

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_flow_control_block
(
  int               handle,
  ps_flow_type   ** ps_flow_ptr_ptr
)
{
  ps_iface_type  * iface_ptr;
  uint32           iface_handle;
  uint32           flow_handle;
#ifdef FEATURE_DATA_PS_QOS
  void           * sec_flow_handle;
  void           * new_sec_flow_handle;
#endif /* FEATURE_DATA_PS_QOS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Convert the input handle into the correct iface and flow handles
    Flow handle is the lowest 8 bits
    Iface handle is the next 8 bits
  -------------------------------------------------------------------------*/
  // TODO: use RFC-style to depict this
  flow_handle = (handle & 0xFF);
  iface_handle = (handle >> 8);

  /*-------------------------------------------------------------------------
    Sanity check the iface handle value
  -------------------------------------------------------------------------*/
  if (MAX_SYSTEM_IFACES <= iface_handle)
  {
    LOG_MSG_ERROR_1("ps_stat_get_flow_control_block(): "
                    "Invalid handle: %d", handle);
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Convert the handle into the instance control block
  -------------------------------------------------------------------------*/
  iface_ptr = global_iface_ptr_array[iface_handle];

  /*-------------------------------------------------------------------------
    Iface instance has not been initialized yet
  -------------------------------------------------------------------------*/
  if (FALSE == PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_stat_get_flow_control_block(): "
                    "Invalid iface ptr, handle 0x%x", handle);
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Sanity check the flow handle value
  -------------------------------------------------------------------------*/
  if ((iface_ptr->iface_private.flow.default_flow_ptr)->flow_private.cookie ==
       flow_handle)
  {
    *ps_flow_ptr_ptr = iface_ptr->iface_private.flow.default_flow_ptr;
  }
#ifdef FEATURE_DATA_PS_QOS
  else
  {
    /*-------------------------------------------------------------------
      Navigate the list of secondary flow control blocks
      Must occur within a critical section since flows could be added
      or removed on the fly, and we don't want to memset invalid memory
    -------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    sec_flow_handle = ps_iface_get_sec_flow_handle(iface_ptr);

    while ( (sec_flow_handle != NULL) &&
            (ps_iface_get_sec_flow_by_handle(iface_ptr,
                                             sec_flow_handle,
                                             ps_flow_ptr_ptr,
                                             &new_sec_flow_handle)) &&
            ((*ps_flow_ptr_ptr)->flow_private.cookie != flow_handle))

    {
      sec_flow_handle = new_sec_flow_handle;
    }

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    if (NULL == ps_flow_ptr_ptr)
    {
      return E_INVALID_ARG;
    }
  }
#endif /* FEATURE_DATA_PS_QOS */

  return E_SUCCESS;
} /* ps_stat_get_flow_control_block */


/*===========================================================================
FUNCTION PS_STAT_GET_FLOW_DESC_LOG_PKT()

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
void ps_stat_get_flow_desc_log_pkt
(
  int32                         handle
)
{
} /* ps_stat_get_flow_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_FLOW_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
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
void ps_stat_get_flow_stat_log_pkt
(
  int32                         handle
)
{
  ps_iface_type               * iface_ptr;
  ps_flow_type                * flow_ptr;
  uint32                        iface_handle;
  uint32                        flow_handle;
#ifdef FEATURE_DATA_PS_QOS
  void                        * sec_flow_handle;
  void                        * new_sec_flow_handle;
#endif /* FEATURE_DATA_PS_QOS */
  int32                         idx;

  errno_enum_type                        ret;
  uint8                                * log_pkt_ptr           = NULL;
  ps_stat_inst_flow_log_pkt_type       * inst_flow_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx = 0; idx < MAX_SYSTEM_IFACES; ++idx)
    {
      /*---------------------------------------------------------------------
        Convert the handle into the instance control block
      ---------------------------------------------------------------------*/
      iface_ptr = global_iface_ptr_array[idx];
      iface_handle = (idx << 8);

      /*---------------------------------------------------------------------
        Interface has not been initialized yet, so skip
      ---------------------------------------------------------------------*/
      if ( (NULL == iface_ptr) ||
           (NULL == iface_ptr->iface_private.this_iface) )
      {
        continue;
      }

      /*---------------------------------------------------------------------
        All interfaces have a default flow - get the description of the
        default flow first
      ---------------------------------------------------------------------*/
      flow_ptr = iface_ptr->iface_private.flow.default_flow_ptr;
      flow_handle = 0;

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_flow_log_pkt_type),
                                  LOG_PS_STAT_FLOW_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("ps_stat_get_flow_stat_log_pkt(): "
                        "Log packet allocated for flow get stats failed");
        return;
      }

      inst_flow_log_pkt_ptr = (ps_stat_inst_flow_log_pkt_type *) log_pkt_ptr;

      inst_flow_log_pkt_ptr->handle  = (iface_handle | flow_handle);

      (void) ps_stat_get_flow(PS_STAT_FLOW_ALL,
                              (void *) flow_ptr,
                              &(inst_flow_log_pkt_ptr->inst_flow_stats),
                              sizeof(ps_stat_flow_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_flow_log_pkt_type)
      );

#ifdef FEATURE_DATA_PS_QOS
      /*---------------------------------------------------------------------
        Get the description of all of the secondary flows
      ---------------------------------------------------------------------*/
      if (iface_ptr->iface_private.flow.num_sec_flows > 0)
      {
        /*-------------------------------------------------------------------
          Navigate the list of secondary flow control blocks
          Must occur within a critical section since flows could be added
          or removed on the fly, and we don't want to access invalid memory
        -------------------------------------------------------------------*/
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

        sec_flow_handle = ps_iface_get_sec_flow_handle(iface_ptr);

        while( (sec_flow_handle != NULL) &&
               (ps_iface_get_sec_flow_by_handle(iface_ptr,
                                                sec_flow_handle,
                                                &flow_ptr,
                                                &new_sec_flow_handle)) )
        {
          flow_handle = flow_ptr->flow_private.cookie;

          ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_flow_log_pkt_type),
                                      LOG_PS_STAT_FLOW_INST_C,
                                      &log_pkt_ptr);

          if (NULL == log_pkt_ptr)
          {
            LOG_MSG_ERROR_0("ps_stat_get_flow_stat_log_pkt(): "
                            "Log packet allocated for flow get stats failed");
            PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
            return;
          }

          inst_flow_log_pkt_ptr =
            (ps_stat_inst_flow_log_pkt_type *) log_pkt_ptr;

          inst_flow_log_pkt_ptr->handle  = (iface_handle | flow_handle);

          (void) ps_stat_get_flow(PS_STAT_FLOW_ALL,
                                  (void *) flow_ptr,
                                  &(inst_flow_log_pkt_ptr->inst_flow_stats),
                                  sizeof(ps_stat_flow_i_s_type));

          ps_stat_logging_commit_log_pkt
          (
            log_pkt_ptr,
            sizeof(ps_stat_inst_flow_log_pkt_type)
          );

          sec_flow_handle = new_sec_flow_handle;
        }

        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      } /* else secondary flow */
#endif /* FEATURE_DATA_PS_QOS */
    } /* for all interfaces on the system */
  } /* if all instances */
  else
  {
    ret = ps_stat_get_flow_control_block(handle, &flow_ptr);

    if (E_SUCCESS != ret)
    {
      LOG_MSG_ERROR_1("ps_stat_get_flow_stat_log_pkt(): "
                      "Unable to get flow control block for get stats.  Handle: %d",
                      handle);
      return;
    }

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_flow_log_pkt_type),
                                LOG_PS_STAT_FLOW_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_stat_get_flow_stat_log_pkt(): "
                      "Log packet allocated for flow get stats failed");
      return;
    }

    inst_flow_log_pkt_ptr = (ps_stat_inst_flow_log_pkt_type *) log_pkt_ptr;

    inst_flow_log_pkt_ptr->handle  = handle;

    (void) ps_stat_get_flow(PS_STAT_FLOW_ALL,
                            (void *) flow_ptr,
                            &(inst_flow_log_pkt_ptr->inst_flow_stats),
                            sizeof(ps_stat_flow_i_s_type));

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_flow_log_pkt_type)
    );

  } /* else only fill one instance */
} /* ps_stat_get_flow_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_FLOW()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the flow

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_flow
(
  int32            handle
)
{
  errno_enum_type  ret;
  ps_flow_type   * flow_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = ps_stat_get_flow_control_block(handle, &flow_cb_ptr);

  if ( (ret != E_SUCCESS) || !PS_FLOW_IS_VALID(flow_cb_ptr) )
  {
    LOG_MSG_ERROR_1("ps_stat_reset_flow(): "
                    "Unable to get flow control block for reset.  Handle: %d",
                    handle);
    return;
  }

  if (PS_STAT_QUERIED == flow_cb_ptr->flow_i_stats.queried)
  {
    ps_stat_get_flow_stat_log_pkt(handle);
  }

  memset(&(flow_cb_ptr->flow_i_stats), 0, sizeof(flow_cb_ptr->flow_i_stats));

  return;
} /* ps_stat_reset_flow */


/*===========================================================================
FUNCTION PS_STAT_INIT_FLOW()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_flow
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_FLOW,
                                           ps_stat_get_flow_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_FLOW,
                                          ps_stat_get_flow_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_FLOW,
                                            ps_stat_reset_flow);

} /* ps_stat_init_flow */


/*===========================================================================
FUNCTION PS_STAT_GET_FLOW()

DESCRIPTION
  This function supplies the required flow layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  flow control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied to
  this memory address. The fourth argument passes the length of memory
  allocated for the return value and should be  greater than or equal to
  the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_flow
(
  ps_stat_flow_enum_type  stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                  ret_len
)
{
  ps_flow_type * flow_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  flow_ptr = (ps_flow_type *)instance_ptr;

  /*-------------------------------------------------------------------------
    Check Instance pointer; Instance pointer should not be null or invalid
  -------------------------------------------------------------------------*/
  if( !PS_FLOW_IS_VALID(flow_ptr) )
  {
    LOG_MSG_ERROR_0("ps_stat_get_flow(): "
                    "Instance pointer is invalid for FLOW stats");
    return E_INVALID_ARG ;
  }

  if (stat == PS_STAT_FLOW_ALL)
  {
     return ps_stat_get_flow_instance_all(stat,
                                          instance_ptr,
                                          return_value,
                                          ret_len
                                         ) ;
  }
  else if (stat == PS_STAT_FLOW_IS_DEFAULT)
  {
     return ps_stat_get_flow_is_default(stat,
                                        instance_ptr,
                                        return_value,
                                        ret_len
                                       ) ;

  }
  {
    return ps_stat_get_common(stat,
                              ps_stat_flow_i_table,
                              PS_STAT_FLOW_ALL,
                              instance_ptr,
                              return_value,
                              ret_len
                             ) ;
  }

} /* ps_stat_get_flow */

