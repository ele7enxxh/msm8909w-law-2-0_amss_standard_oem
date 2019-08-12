#ifndef PS_IFACE_HANDOFF_H
#define PS_IFACE_HANDOFF_H
/*===========================================================================
  @file ps_iface_handoff.h

  TODO

  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_handoff.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-03-10 sun Added changes to support Pre Register Handoff
  2011-06-06 sid Added support for Partial PPP context
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "ps_iface_defs.h"
#include "ps_acl.h"


/*===========================================================================

                              EXTERNAL FUNCTIONS

===========================================================================*/
int32 ps_iface_handoff_initiate
(
  ps_iface_type         * ps_iface_ptr,
  acl_policy_info_type  * acl_policy_ptr,
  int16                 * ps_errno_ptr
);

int32 ps_iface_handoff_swap_rat
(
  ps_iface_type  * ps_iface_ptr,
  int16          * ps_errno_ptr,
  void           * client_data_ptr
);

int32 ps_iface_handoff_failure
(
  ps_iface_type  * ps_iface_ptr,
  int16          * ps_errno_ptr
);

#ifdef FEATURE_DATA_PS_IPV6
int32 ps_iface_handoff_transfer_sec_ipv6_addr
(
  ps_iface_type  * ps_iface_ptr,
  int16          * ps_errno_ptr
);
#endif /* FEATURE_DATA_PS_IPV6 */

/**
  @brief compares ip addressess. 
 
  This method compares ip addressess of given interfaces
 
  @param[in]  target_iface_ptr  Pointer to the target interface
  @param[in]  src_iface_ptr     Pointer to the source interface
 
  @return
    TRUE  on success
    FALSE on failure 
  
  @dependencies 
 	 None
*/
boolean ps_iface_compare_primary_ip_addr
(
  ps_iface_type  *target_iface_ptr,
  ps_iface_type  *src_iface_ptr
);

#endif /* PS_IFACE_HANDOFF_H */
