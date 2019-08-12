#ifndef PS_STAT_IFACE_H
#define PS_STAT_IFACE_H

/*===========================================================================

                          P S _ S T A T _ I F A C E . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to PS_IFACE.

EXTERNALIZED FUNCTIONS
  ps_stat_get_iface()
    This function supplies the required iface stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_stat_iface.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/08   rr      Added statistics to track the number of bytes tx and rx
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
#include "ps_in.h"
#include "ps_iface_ioctl.h"

#define PS_STAT_INST_DESC_IFACE_MAX_HW_ADDR_LEN            6

typedef ps_iface_ioctl_iface_stats_type ps_stat_iface_i_s_type; 

/*---------------------------------------------------------------------------
  IFACE statistics enum
---------------------------------------------------------------------------*/
typedef enum
{
  /* Desc : # of packets received */
  /* type : uint32 */
  PS_STAT_IFACE_PKTS_RX          = 0,
  /* Desc : # of multicast packets received */
  /* type : uint32 */
  /* NOTE : NOT IMPLEMENTED */
  PS_STAT_IFACE_MCAST_PKTS_RX    = 1,
  /* Desc : # of incoming packets dropped*/
  /* type : uint32 */
  PS_STAT_IFACE_PKTS_DROPPED_RX  = 2,
  /* Desc : # of packets sent */
  /* type : uint32 */
  PS_STAT_IFACE_PKTS_TX          = 3,
  /* Desc : # of multicast packets received */
  /* type : uint32 */
  PS_STAT_IFACE_MCAST_PKTS_TX    = 4,
  /* Desc : # of outgoing packets dropped */
  /* type : uint32 */
  PS_STAT_IFACE_PKTS_DROPPED_TX  = 5,
  /* Desc : # of bytes received */
  /* type : uint64 */
  PS_STAT_IFACE_BYTES_RX         = 6,
  /* Desc : # of bytes transmitted*/
  /* type : uint64 */ 
  PS_STAT_IFACE_BYTES_TX         = 7,
  /* Desc : state of the interface */
  /* type : uint16 */
  PS_STAT_IFACE_STATE            = 8,
  /* Desc : all ps_iface stats */
  /* type : ps_stat_iface_i_s_type */
  PS_STAT_IFACE_ALL
} ps_stat_iface_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_IFACE_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_iface_stats : instance iface stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_iface_i_s_type    inst_iface_stats;
} ps_stat_inst_iface_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_IFACE_V6_ADDR_TYPE

DESCRIPTION
  The structure describing an IPv6 address associated with an iface.

  addr_prefix : Prefix of the IPv6 address
  addr_iid    : Host addr
  addr_type   : Type of IPv6 address (i.e. public, private, ...)
  addr_state  : State of the address (i.e. active, deprecated, ...)
  reserved    : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint64             addr_prefix;
  uint64             addr_iid;
  uint8              addr_type;
  uint8              addr_state;
  uint8              reserved[2];
} ps_stat_inst_desc_iface_v6_addr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_IFACE_TYPE

DESCRIPTION
  The structure describing a ps_iface instance.  The actual log packet uses
  this structure, and several others, to completely describe an iface
  instance.  In a log packet, an iface is described by:
    ps_stat_inst_desc_iface_type
    ps_stat_inst_desc_iface_v6_addr_type
    ...
    ps_stat_inst_desc_iface_v6_addr_type
    ps_in6_addr
    ...
    ps_in6_addr
    char[name_size]

  The iface description is first, followed by zero or more IPv6 address
  structs.  The number of IPv6 address structs is found in the iface
  description struct.  Following the IPv6 addresses are the multicast group
  addresses that the iface is a member of.  There could be zero or more of
  these addresses, the number is found in the iface description struct.  The
  final element of the description is a character array containing the name
  of the iface, its size is again found in the initial iface description
  struct.

  handle            : Instance handle, index into global AMSS iface array
  name_size         : Size of the iface name
  addr_v6_count     : Number of IPv6 addresses assigned to this iface, used
                      to count the number of addr structs following this
  mcast_group_count : Number of multicast groups active on this iface, used
                      to count the number of addr structs following the
                      IPv6 addrs
  addr_v4           : IPv4 address of this interface
  netmask_v4        : IPv4 network mask of this interface
  hw_addr           : MAC address of this interface
  mtu               : MTU size of this interface
  state             : State of this interface
  bcast_capable     : Is this interface on a bcast or mcast physical network
  reserved          : Used to maintain 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                     handle;
  uint8                     name_size;
  uint8                     addr_v6_count;
  uint8                     mcast_group_count;
  uint32                    addr_v4;
  struct ps_in_addr         netmask_v4;
  uint8                     hw_addr[PS_STAT_INST_DESC_IFACE_MAX_HW_ADDR_LEN];
  uint16                    mtu;
  uint8                     state;
  uint8                     bcast_capable;
  uint8                     reserved[2];
} ps_stat_inst_desc_iface_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO IFACE_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the iface statistic (field) by the necessary quantity
  (inc).
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_INC_INSTANCE_STATS(PTR,FIELD,INC)    \
    PS_COMMON_INC_STATS(PTR->iface_i_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_IFACE()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_iface
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_IFACE()

DESCRIPTION
  This function supplies the required iface layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  iface control block. The third argument points to the memory location
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
errno_enum_type ps_stat_get_iface
(
  ps_stat_iface_enum_type   stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
);

/*===========================================================================
FUNCTION PS_STAT_RESET_IFACE()

DESCRIPTION
  This function resets the statistics on the specified interface.

PARAMETERS
  handle : Handle to the instance that should have its stats reset

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_iface
(
  int32       handle
);

/*===========================================================================
FUNCTION PS_STAT_RESET_IFACE_STATS_USING_INSTANCE_PTR()

DESCRIPTION
  This function resets the statistics on the specified interface.

PARAMETERS
  instance_ptr: Handle to the iface being reset

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_iface_stats_by_iface_ptr
(
  void      *instance_ptr
);

#endif /* PS_STAT_IFACE_H */
