#ifndef DSS_NET_MGR_H
#define DSS_NET_MGR_H
/*===========================================================================

                     D S S _ N E T _ M G R . H

DESCRIPTION

 This header file contains functions to bring up and tear down a network
 interface.

EXTERNALIZED FUNCTIONS
  dss_net_mgr_init()
    Initializes the module

  dss_net_mgr_bring_up_net_iface()
    Bring up the desired network interface

  dss_net_mgr_tear_down_net_iface()
    Tear down the network interface

Copyright (c) 2007-2012 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/dss_net_mgr.h#1 $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dssocket_defs.h"
#include "dss_netpolicy.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  The events that are posted by this module. NET_EVENT_UP is posted when
  net_iface is brough up and NET_EVENT_DOWN is posted when net_iface is
  torn down
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_NET_MGR_NET_EVENT_MIN   = 0,
  DSS_NET_MGR_NET_EVENT_UP    = DSS_NET_MGR_NET_EVENT_MIN,
  DSS_NET_MGR_NET_EVENT_DOWN  = 1,
  DSS_NET_MGR_NET_EVENT_MAX   = 2
} dss_net_mgr_net_event_enum_type;

/*---------------------------------------------------------------------------
  This is the callback that a client can register with this module. It is
  called when this module would like to post an event
---------------------------------------------------------------------------*/
typedef void (*dss_net_mgr_cback_f_type)
(
  int32                              net_mgr_handle,
  dss_iface_id_type                  iface_id,
  dss_net_mgr_net_event_enum_type    net_mgr_event,
  void                             * net_cb_user_data
);

/*---------------------------------------------------------------------------
  Network manager control block
---------------------------------------------------------------------------*/

typedef struct
{
  int32                       net_mgr_handle;
  dss_iface_id_type           iface_id;
  int16                       dss_net_handle;
  dss_net_mgr_cback_f_type    cback_f_ptr;
  void                      * cback_data;
} dss_net_mgr_cb_type;

/*===========================================================================

                      EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_NET_MGR_INIT()

DESCRIPTION
  Initializes the net mgr module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  Must be called at power up

SIDE EFFECTS
  None
===========================================================================*/
void dss_net_mgr_init
(
  void
);

/*===========================================================================
FUNCTION DSS_NET_MGR_BRING_UP_NET_IFACE()

DESCRIPTION
  This function brings up the network interface given a network policy

PARAMETERS
  net_policy_ptr : policy describing the kind of interface, that the client
                   is interested in. If NULL, default network policy is chosen
  iface_id_ptr   : OUT PARAM; iface_id of the brought up interface
  user_cb_f_ptr  : User callback function
  user_cb_data   : Data passed back with the callback function
  dss_errno      : OUT PARAM; updated with reason code on error

RETURN VALUE
  A handle to the session : On success
  DSS_ERROR               : On error

DEPENDENCIES
  Net mgr module must have been initialized
  Clients of this module must be executed in PS context

SIDE EFFECTS
  None
===========================================================================*/
int32 dss_net_mgr_bring_up_net_iface
(
  dss_net_policy_info_type  * net_policy_ptr,
  dss_iface_id_type         * iface_id_ptr,
  dss_net_mgr_cback_f_type    user_cback_f_ptr,
  void                      * user_data_ptr,
  int16                     * dss_errno
);

/*===========================================================================
FUNCTION DSS_NET_MGR_TEAR_DOWN_NET_IFACE()

DESCRIPTION
  This function tears down the network interface

PARAMETERS
  net_mgr_handle : A handle which identifies the session
  dss_errno      : OUT PARAM; updated with reason code on error

RETURN VALUE
  DSS_SUCCESS : On success
  DSS_ERROR   : On error

DEPENDENCIES
  Net mgr module must have been initialized

SIDE EFFECTS
  None
===========================================================================*/
int16 dss_net_mgr_tear_down_net_iface
(
  int32    net_mgr_handle,
  int16  * dss_errno
);


#endif /* DSS_NET_MGR_H */
