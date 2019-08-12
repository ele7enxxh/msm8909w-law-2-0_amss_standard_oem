#ifndef IWLAN_3GPP_RT_PROF_H
#define IWLAN_3GPP_RT_PROF_H
/*===========================================================================

                IW L A N _ 3 G P P _ R T _ P R O F . H

DESCRIPTION
   This is IWLAN 3GPP iface Routing Profile API header file.


Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/api/iwlan_3gpp_rt_prof.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ps_acl.h"

boolean
iwlan_3gpp_rt_is_profile_in_use
( 
  uint32                   pdp_profile_num,     /* PDP profile number*/
  acl_policy_info_type   * policy_info_ptr      /* Policy Information */
);

#endif /* IWLAN_3GPP_RT_ACL_H */
