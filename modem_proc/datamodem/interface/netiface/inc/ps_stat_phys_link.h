#ifndef PS_STAT_PHYS_LINK_H
#define PS_STAT_PHYS_LINK_H

/*===========================================================================

                          P S _ S T A T _ P H Y S _ L I N K . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to phys_link.

EXTERNALIZED FUNCTIONS
  ps_stat_get_phys_link()
    This function supplies the required phys_link stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_stat_phys_link.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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


/*--------------------------------------------------------------------------
  PHYS_LINK - Instance stats only
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  pkts_tx ;
  uint64  bytes_tx ;
  uint32  pkts_rx ;
  uint64  bytes_rx ;
  uint32  ref_cnt ;
  uint16  state ;
  uint32  is_primary ;
} ps_stat_phys_link_i_s_type;

/*---------------------------------------------------------------------------
  PHYS_LINK statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc : # of packets transmitted */
  /* type : uint32 */
  PS_STAT_PHYS_LINK_PKTS_TX   = 0,
  /* Desc : # of bytes transmitted */
  /* type : uint64 */
  /* NOTE : NOT IMPLEMENTED */
  PS_STAT_PHYS_LINK_BYTES_TX  = 1,
  /* Desc : # of packets received */
  /* type : uint32 */
  /* NOTE : NOT IMPLEMENTED */
  PS_STAT_PHYS_LINK_PKTS_RX   = 2,
  /* Desc : # of bytes received */
  /* type : uint32 */
  /* NOTE : NOT IMPLEMENTED */
  PS_STAT_PHYS_LINK_BYTES_RX  = 3,
  /* Desc : # of flows on this phys_link */
  /* type : uint32 */
  PS_STAT_PHYS_LINK_REF_CNT   = 4,
  /* Desc : state of this physlink */
  /* type : uint16 */
  PS_STAT_PHYS_LINK_STATE     = 5,
  /* Desc : Is this a primary phys_link for the iface ? */
  /* type : uint32 */
  PS_STAT_PHYS_LINK_IS_PRIMARY = 6,
  /* Desc : all phys_link stats */
  /* type : ps_stats_phys_link_i_s_type */
  PS_STAT_PHYS_LINK_ALL
} ps_stat_phys_link_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_PHYS_LINK_INST_C

  Log header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_phys_link_stats : instance phys_link stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                       handle;
  ps_stat_phys_link_i_s_type  inst_phys_link_stats;
} ps_stat_inst_phys_link_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_PHYS_LINK_TYPE

DESCRIPTION
  The structure describing a ps_phys_link instance.

  handle        : Instance handle.  Upper 8 bits are iface handle, lower
                  8 bits are phys link handle
  hdr_comp_type : Type of header compression set on the iface
  reserved      : Used to maintain 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint16  handle;
  uint8   hdr_comp_type;
  uint8   hdr_comp_handle;
} ps_stat_inst_desc_phys_link_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO PHYS_LINK_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the phys_link statistic(field) by the necessary
  quantity (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PHYS_LINK_INC_INSTANCE_STATS(PTR, FIELD, INC)    \
    PS_COMMON_INC_STATS(PTR->phys_link_i_stats.FIELD, 4, INC)


/*===========================================================================
FUNCTION PS_STAT_INIT_PHYS_LINK()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_phys_link
(
  void
);


/*===========================================================================
FUNCTION PS_STAT_GET_PHYS_LINK()

DESCRIPTION
  This function supplies the required phys_link layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  phys_link control block. The third argument points to the memory location
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
errno_enum_type ps_stat_get_phys_link
(
  ps_stat_phys_link_enum_type   stat,
  void                          *instance_ptr,
  void                          *return_value,
  uint16                        ret_len
);


/*===========================================================================
FUNCTION PS_STAT_RESET_PHYS_LINK()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_phys_link
(
  int32         handle
);
#endif /* PS_STAT_PHYS_LINK_H */
