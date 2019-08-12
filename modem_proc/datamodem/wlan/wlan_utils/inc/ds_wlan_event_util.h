#ifndef DS_WLAN_EVENT_UTIL_H
#define DS_WLAN_EVENT_UTIL_H
/*===========================================================================
                        DS_WLAN_IWLAN_S2B_EVT_UTIL
===========================================================================*/

/*!
  @file
  ds_wlan_event_util.h

  @brief
  This module implements wlan util functions

  @detail
*/

/*===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/wlan_utils/inc/ds_wlan_event_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/14   fn      Created module
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds3geventmgr.h"
#include "ds_wlan_util.h"
#include "ps_sys.h"
#include "ps_acl.h"

/*===========================================================================
                             PUBLIC DATA DECLARATIONS
===========================================================================*/
#define WLAN_SSID_CHG_MASK              (0x00000001)
#define WLAN_MAC_ADDR_CHG_MASK          (0x00000010)
#define WLAN_V4_IP_ADDR_CHG_MASK        (0x00000100)
#define WLAN_V6_IP_ADDR_CHG_MASK        (0x00001000)
#define WLAN_DNS_ADDR_CHG_MASK          (0x00010000)
#define WLAN_INFO_CHG_ALL_ATTRS_MASK ( WLAN_SSID_CHG_MASK | \
                                       WLAN_MAC_ADDR_CHG_MASK | \
                                       WLAN_V4_IP_ADDR_CHG_MASK | \
                                       WLAN_V6_IP_ADDR_CHG_MASK | \
                                       WLAN_DNS_ADDR_CHG_MASK )

/*===========================================================================
FUNCTION ds_wlan_is_call_disable_for_thermal

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define WLAN_IS_CALL_DISABLE_FOR_THERMAL() \
  ds_wlan_is_call_disable_for_thermal(policy_info_ptr)

boolean ds_wlan_is_call_disable_for_thermal
(
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
FUNCTION      ds_wlan_process_ds3g_event_mgr_cmd

DESCRIPTION   Processes DS_CMD_WLAN_UTILS_DS3G_EVENT_MGR command

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_process_ds3g_event_mgr_cmd
(
  ds_wlan_cmd_type   *wlan_cmd_ptr
);

/*===========================================================================
FUNCTION      ds_wlan_event_util_init

DESCRIPTION   Event util initialize and ds3geventmgr events

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_event_util_init
(
  void
);

/*===========================================================================
FUNCTION      ds_wlan_powerup_deinit

DESCRIPTION   De-Init function called during shutdown.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* FEATURE_DATA_WLAN_MAPCON */
#endif /* DS_WLAN_EVENT_UTIL_H */
