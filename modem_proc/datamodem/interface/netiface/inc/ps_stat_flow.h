#ifndef PS_STAT_FLOW_H
#define PS_STAT_FLOW_H

/*===========================================================================

                          P S _ S T A T _ F L O W . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to flows.

EXTERNALIZED FUNCTIONS
  ps_stat_get_flow()
    This function supplies the required flow stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_stat_flow.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/07   scb     Added changes to fix RCVT compilation warning
02/21/06   scb     Replaced the INC macro implementation to use the
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
  FLOW - Instance stats only
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  pkts_tx ;
  uint64  bytes_tx ;
  uint16  state ;
  uint32  is_default ;
} ps_stat_flow_i_s_type;

/*---------------------------------------------------------------------------
  FLOW statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc : # of packets transmitted */
  /* type : uint32 */
  PS_STAT_FLOW_PKTS_TX   = 0,
  /* Desc : # of bytes transmitted */
  /* type : uint32 */
  /* NOTE : NOT IMPLEMENTED */
  PS_STAT_FLOW_BYTES_TX  = 1,
  /* Desc : state of the flow */
  /* type : uint32 */
  PS_STAT_FLOW_STATE     = 2,
  /* Desc : Is this the default flow for the iface ?*/
  /* type : uint16 */
  PS_STAT_FLOW_IS_DEFAULT = 3,
  /* Desc : all flow stats */
  /* type : ps_stat_flow_i_s_type */
  PS_STAT_FLOW_ALL
} ps_stat_flow_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_FLOW_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_flow_stats  : instance flow stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_flow_i_s_type     inst_flow_stats;
} ps_stat_inst_flow_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_FLOW_TYPE

DESCRIPTION
  The structure describing a ps_flow instance.

  handle           : Instance handle.  Upper 8 bits are iface handle, lower
                     8 bits are flow handle
  phys_link_handle : Handle to the phys link that the flow is bound to
  capability       : Capability of the flow
  qos_handle       : Handle to the associated QoS spec
---------------------------------------------------------------------------*/
typedef struct
{
  uint16  handle;
  uint8   phys_link_handle;
  uint8   capability;
  uint32  qos_handle;
} ps_stat_inst_desc_flow_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO FLOW_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the flow statistic (field) by the necessary quantity
  (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define FLOW_INC_INSTANCE_STATS(PTR, FIELD, INC)     \
    PS_COMMON_INC_STATS(PTR->flow_i_stats.FIELD, 4, INC)


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
);


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
  ps_stat_flow_enum_type   stat,
  void                     *instance_ptr,
  void                     *return_value,
  uint16                   ret_len
);


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
  int32         handle
);
#endif /* PS_STAT_FLOW_H */
