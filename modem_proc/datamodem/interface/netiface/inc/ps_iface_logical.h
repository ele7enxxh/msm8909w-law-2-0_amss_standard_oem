#ifndef PS_IFACE_LOGICAL_H
#define PS_IFACE_LOGICAL_H
/*===========================================================================
  @file ps_iface_logical.h

  TODO

  Copyright (c) 2009-15 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_logical.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_iface_defs.h"
#include "ps_acl.h"


/*===========================================================================

                              EXTERNAL FUNCTIONS

===========================================================================*/
int32 ps_iface_logical_create
(
  ps_iface_type            * this_iface_ptr,
  ps_iface_name_enum_type    name,
  acl_type                 * this_iface_outgoing_acl_ptr,
  acl_type                 * this_iface_incoming_acl_ptr,
  ps_phys_link_type        * phys_link_array,
  uint8                      num_phys_links,
  boolean                    inherit_ip_info
);

int32 ps_iface_logical_default_bring_up_cmd_hdlr
(
  ps_iface_type  * ps_iface_ptr,
  void           * client_data_ptr
);

int32 ps_iface_logical_default_bring_up_cmd_ex_hdlr
(
  ps_iface_type  * ps_iface_ptr,
  int32            app_priority,
  void           * client_data_ptr
);

int32 ps_iface_logical_default_tear_down_cmd_hdlr
(
  ps_iface_type  * ps_iface_ptr,
  void           * client_data_ptr
);

int32 ps_iface_logical_default_tear_down_cmd_ex_hdlr
(
  ps_iface_type  * ps_iface_ptr,
  int32            app_priority,
  void           * client_data_ptr
);

int32 ps_iface_logical_swap_rat
(
  ps_iface_type  * ps_iface_ptr,
  int16          * ps_errno_ptr
);

int32 ps_iface_logical_process_iface_up_ev
(
  ps_iface_type  * ps_iface_ptr,
  int16          * ps_errno_ptr
);

void ps_iface_logical_process_iface_down_ev
(
  ps_iface_type                  * ps_iface_ptr,
  ps_iface_net_down_reason_type    net_down_reason
);

#endif /* PS_IFACE_LOGICAL_H */
