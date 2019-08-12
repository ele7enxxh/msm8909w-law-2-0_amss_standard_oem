#ifndef PS_IFACE_MCAST_H
#define PS_IFACE_MCAST_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ I F A C E _ F L O W . H

DESCRIPTION
  Header file defining ps_flow specific ps_iface functions and definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_mcast.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.

===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_in.h"
#include "pstimer.h"


/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  MCAST State machine.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_MCAST_STATE_NON_MEMBER      = 0,
  PS_MCAST_STATE_DELAYING_MEMBER = 1,
  PS_MCAST_STATE_IDLE_MEMBER     = 2
} ps_mcast_state_enum_type;

/*---------------------------------------------------------------------------
  Structure to hold the list of multicast groups that the
  interface is interested in.
---------------------------------------------------------------------------*/
typedef struct ps_mcast_cb
{
  ps_ip_addr_type                 mcast_grp;
  uint32                          membership_count;
  ps_timer_handle_type            mcast_report_timer;
  ps_mcast_state_enum_type        mcast_state;
  boolean                         send_leave_msg;
  ps_iface_type                   *iface_ptr;
  struct ps_mcast_cb              *next_mcast_grp;
} ps_mcast_cb_type;

#endif /* PS_IFACE_FLOW_H */
