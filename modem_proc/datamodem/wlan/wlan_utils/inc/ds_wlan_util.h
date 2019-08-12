#ifndef DS_WLAN_UTIL_H
#define DS_WLAN_UTIL_H
/*===========================================================================
                           DS_WLAN_UTIL
===========================================================================*/

/*!
  @file
  ds_wlan_util.h

  @brief
  This module implements wlan util functions

  @detail
*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/wlan_utils/inc/ds_wlan_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/16   ak      Added support for WCDMA hysteresis and WLAN WCDMA timers(FR 20728)
01/06/14   vl      Added support for state based setup timer values
07/28/14   op      Added support for processing WLAN Utils commands
05/29/14   fn      Added support to use new API for posting DS commands 
03/20/14   scb     Added support for MMGSDI SUBSCRIPTION TYPE
02/20/14   scb     Added support for MPPM hysteresis timers
01/28/14   vl      Created module
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds3gmmgsdiif.h"
#include "ds3geventmgr.h"
#include "dstask_v.h"
#include "ps_sys.h"

/*===========================================================================
                             PUBLIC DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
 WLAN utils use to send generic pointer structure
---------------------------------------------------------------------------*/
typedef struct
{
  void *cmd_data_ptr;
} ds_wlan_cmd_type;

/*---------------------------------------------------------------------------
 MMGSDI_SUBSCRIPTION status structure.
---------------------------------------------------------------------------*/

typedef struct
{
  ds3geventmgr_event_type         mmgsdi_event;
  ps_sys_subscription_enum_type   dsd_subs_id;
} mmgsdi_subscription_status_type;



/*===========================================================================
                          VARIABLES AND MACRO DEFINITIONS
==========================================================================*/

/*-----------------------------------------------------------------------------
  Enum to specify the various WLAN offload config types

  DISABLED     : WLAN offload disabled
  WLAN_LB_ONLY : WLAN local breakout enabled
  IWLAN_S2B    : IWLAN_S2B offload feature enabled
                (and inherently WLAN local breakout is enabled)
-----------------------------------------------------------------------------*/
typedef enum
{
  DS_WLAN_OFFLOAD_CONFIG_DISABLED     = 0,
  DS_WLAN_OFFLOAD_CONFIG_WLAN_LB_ONLY = 1,
  DS_WLAN_OFFLOAD_CONFIG_IWLAN_S2B    = 2,
  DS_WLAN_OFFLOAD_CONFIG_MAX,

  DS_WLAN_OFFLOAD_CONFIG_DEFAULT      = DS_WLAN_OFFLOAD_CONFIG_DISABLED
}ds_wlan_offload_config_enum_type;

/*-------------------------------------------------------------------------
    NV refresh event info
-------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH

typedef struct ds_wlan_nv_refresh_event_info_type_s ds_wlan_nv_refresh_event_info_type;

struct ds_wlan_nv_refresh_event_info_type_s
{
  ds_wlan_offload_config_enum_type            prev_wlan_offload_config;
  ds_wlan_offload_config_enum_type            current_wlan_offload_config;
};
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

#define DS_WLAN_DATA_CONF_BUF_SIZE ds_wlan_get_efs_item_file_path_size()

/*---------------------------------------------------------------------------
   Macro to check whether offload is NV is enabled
---------------------------------------------------------------------------*/
#define DS_WLAN_OFFLOAD_CONFIG_NV_ENABLED(wlan_offload_config_nv)          \
        ( (wlan_offload_config_nv == DS_WLAN_OFFLOAD_CONFIG_IWLAN_S2B ) || \
          (wlan_offload_config_nv == DS_WLAN_OFFLOAD_CONFIG_WLAN_LB_ONLY) )

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ds_wlan_powerup_init

DESCRIPTION   Init function called on power-up.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_powerup_init
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

/*===========================================================================
FUNCTION      DW_WLAN_EFS_ITEM_FILE_PATH_SIZE

DESCRIPTION   Returns the total wlan EFS item file paths size

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_wlan_get_efs_item_file_path_size
(
  void
);

/*===========================================================================
  FUNCTION DS_WLAN_UPDATE_DATA_CONFIG_INFO

  DESCRIPTION
    Puts all the required nv item file paths in the data_config_info.conf
    file. Would be called from dsutils during powerup.

  PARAMETERS
    file_paths_buf: To hold the efs item file paths

  DEPENDENCIES
    None.

  RETURN VALUE
    0 for success.
    -1 for failure.

  SIDE EFFECTS
    None.

===========================================================================*/
int32 ds_wlan_update_data_config_info
(
  char  *file_paths_buf
);

/*===========================================================================
FUNCTION ds_wlan_get_wifi_oos_linger_timer

DESCRIPTION
  This function gets the wifi unavailable timer value from EFS file

DEPENDENCIES
  The data config file must have been created

RETURN VALUE
  uint32 - timer value if Success else -1

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_wifi_oos_linger_timer
(
  void
);

/*===========================================================================
FUNCTION ds_wlan_get_wlan_proxy_bringup_retry_timer

DESCRIPTION
  This function returns timer value for wlan proxy bringup retry mechanism

DEPENDENCIES
  None

RETURN VALUE
  uint32 - timer value if Success else -1

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_wlan_proxy_bringup_retry_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_WLAN_OFFLOAD_CONFIG_NV

DESCRIPTION
  Returns the current value of ds_wlan_offload_config_val.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  ds_wlan_offload_config_enum_type

SIDE EFFECTS
  None
===========================================================================*/
ds_wlan_offload_config_enum_type ds_wlan_get_wlan_offload_config_nv
(
  void
);


/*===========================================================================
FUNCTION DS_WLAN_GET_HYST_LTE_NULL_TIMER

DESCRIPTION
  Returns the current value of ds_wlan_hyst_lte_null_timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_wlan_hyst_lte_null_timer

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_hyst_lte_null_timer
(
  void
);

/*===========================================================================
FUNCTION ds_wlan_get_iwlan_s2b_mtu_val

DESCRIPTION
  Returns the current value of ds_iwlan_s2b_mtu_val.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_iwlan_s2b_mtu_val

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_iwlan_s2b_mtu_val
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_HYST_EHRPD_NULL_TIMER

DESCRIPTION
  Returns the current value of ds_wlan_hyst_ehrpd_null_timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_wlan_hyst_ehrpd_null_timer

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_hyst_ehrpd_null_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_HYST_WCDMA_NULL_TIMER

DESCRIPTION
  Returns the current value of ds_wlan_hyst_wcdma_null_timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_wlan_hyst_wcdma_null_timer

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_hyst_wcdma_null_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_HYST_LTE_ATTACH_WAIT_TIMER

DESCRIPTION
  Returns the current value of ds_wlan_hyst_lte_attach_wait_timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_wlan_hyst_lte_attach_wait_timer
  

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_hyst_lte_attach_wait_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_WLAN_THROTTLE_TIMER

DESCRIPTION
  Returns the current value of ds_mppm_wlan_throttle_timer_val.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_mppm_wlan_throttle_timer_val
  

SIDE EFFECTS
  None
===========================================================================*/
rex_timer_cnt_type ds_wlan_get_wlan_throttle_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_WLAN_WCDMA_THROTTLE_TIMER

DESCRIPTION
  Returns the current value of ds_mppm_wlan_wcdma_throttle_timer_val.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE value of ds_mppm_wlan_wcdma_throttle_timer_val
  

SIDE EFFECTS
  None
===========================================================================*/
rex_timer_cnt_type ds_wlan_get_wlan_wcdma_throttle_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_WLAN_PROXY_SIO_CONFIG_TIMER

DESCRIPTION
  Returns the timer value for SIO configurations

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE


SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_wlan_proxy_sio_config_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_GET_WLAN_PROXY_RM_CONFIG_TIMER

DESCRIPTION
  Returns the timer value for RM configurations

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE


SIDE EFFECTS
  None
===========================================================================*/
uint32 ds_wlan_get_wlan_proxy_rm_config_timer
(
  void
);

/*===========================================================================
FUNCTION DS_WLAN_POST_CMD

DESCRIPTION
  A wrapper function for posting DS command. Posts a DS command and
  allocates buffer space for ds command.

DEPENDENCIES
  None

PARAMETERS
  cmd_id - id of the command to be posted 
  payload_data - pointer to a structure to be send along with the DS cmd
  size_t payload_size - size of the payload_data to be sent
  module_type - module type, WLAN util supports MPPM, iWLAN S2B, and WLAN
  queue_type - destination cmd queue, by default it will use default queue

RETURN VALUE 
  FALSE - Fail to post DS command 
  TRUE -  Successfully posted DS command
  

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_wlan_post_cmd
(
  ds_cmd_enum_type cmd_id,
  void *payload_data,
  size_t payload_size
);

/*===========================================================================
FUNCTION      ds_wlan_process_cmd

DESCRIPTION   Handle commands specific to WLAN Utils

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None 
===========================================================================*/
void ds_wlan_process_cmd
(
  ds_cmd_type     *cmd_ptr
);

/*===========================================================================
FUNCTION      ds_wlan_read_efs_nv_items

DESCRIPTION   Called to read WLAN related EFS NV items.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_read_efs_nv_items
(
  void
);
#endif /* FEATURE_DATA_WLAN_MAPCON */
#endif /* DS_WLAN_UTIL_H */
