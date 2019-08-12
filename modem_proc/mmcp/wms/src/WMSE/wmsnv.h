#ifndef WMSNV_H
#define WMSNV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsnv.h -- NV Operations Definitions

  This module provides the utility functions for the use by WMS for accessing
  NV items needed for SMS processing.

  -------------

    Copyright (c) 1999-2008,2010-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsnv.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


#include "mmcp_variation.h"
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/*===========================================================================
EXTERNALIZED FUNCTIONS
  wms_nv_read_wait
    Read an item from NV, handle non-active items

  wms_nv_write_wait
    Write an item to NV

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>

#include "wmsi.h"
#include "err.h"
#include "nv.h"



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define      MAX_SMS_FILE_NAME_SIZE      20

/** EFS id */
typedef enum {
  WMS_NV_EFS_MO_ON_DATA_CHANNEL,
  WMS_NV_EFS_ENABLE_FDN_CONTROL,
  WMS_NV_EFS_1x_SMS_AND_VOICE,
  WMS_NV_EFS_STORE_TO_SIM_IF_NV_FULL,
  WMS_NV_EFS_SMS_RETRY_LIMIT,
  WMS_NV_EFS_TELECOM_SMSP_FALLBACK,
  WMS_NV_EFS_DISABLE_LTE_CB_DUP_DETECTION,
  WMS_NV_EFS_DISABLE_PRES_BC_ALERT,
  WMS_NV_EFS_CS_DOMAIN_FALLBACK,
  WMS_NV_EFS_STK_SMS_RETRY_ENABLE,
  WMS_NV_EFS_MMGSDI_REFRESH_VOTE_OK,
  WMS_NV_EFS_MAX_MT_ALLOWED_IN_ECBM,
  WMS_NV_EFS_ALLOW_SMS_IN_ECBM,
  WMS_NV_EFS_MAX = 0x7FFFFFFF
} wms_nv_efs_id_e_type;

/*===========================================================================
FUNCTION WMS_NV_WRITE_WAIT

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr    /* pointer to data for item */
);

/*===========================================================================
FUNCTION wms_nv_write_wait_per_subs

DESCRIPTION
  Write an item to NV memory per as id.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait_per_subs(
  sys_modem_as_id_e_type  as_id,
  nv_items_enum_type      item,        /* which item */
  nv_item_type           *data_ptr     /* pointer to data for item */
);

/*===========================================================================
FUNCTION WMS_NV_READ_WAIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr          /* pointer to space for item */
);

/*===========================================================================
FUNCTION wms_nv_read_wait_per_subs

DESCRIPTION
  Get an item from the nonvolatile memory per as id.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait_per_subs(
  sys_modem_as_id_e_type  as_id,
  nv_items_enum_type      item,        /* which item */
  nv_item_type           *data_ptr     /* pointer to space for item */
);

/*===========================================================================
FUNCTION WMS_NV_INIT_NVRUIM

DESCRIPTION
  Get certain NV items from the EFS or RUIM depending on NVRUIM's settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_init_nvruim
(
  void
);

/*===========================================================================
FUNCTION WMS_NV_INIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
void wms_nv_init
(
  void
);


#ifdef FEATURE_CDSMS
boolean wms_nv_read_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_write_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_replace_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
);

boolean wms_nv_delete_cdma_sms
(
  wms_message_index_type   index,
  const uint8                    * data
);

boolean wms_nv_read_cdma_params
(
  wms_message_index_type    index,
  uint8                     * data
);

boolean wms_nv_write_cdma_params
(
  wms_message_index_type    index,
  const uint8                     * data
);

void wms_nv_read_cdma_channel_setting(void);

void wms_nv_read_cdma_service_option(void);

/*===========================================================================
FUNCTION wms_nv_read_sms_status

DESCRIPTION
  Read the sms_status item from the non-volatile memory. This in turn will set
  the ruim_last_message_number entry of the config manager.

RETURN VALUE
  void

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_read_sms_status (void);

/*===========================================================================
FUNCTION wms_nv_write_sms_status

DESCRIPTION
  Write the sms_status item to the non-volatile memory.

RETURN VALUE
  void

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_write_sms_status (void);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

boolean wms_nv_read_gw_sms
(
  wms_message_index_type   index,
  uint8                      * data
);


boolean wms_nv_write_gw_sms
(
  wms_message_index_type   index,
  uint8                      * data
);

boolean wms_nv_replace_gw_sms
(
  wms_message_index_type   index,
  uint8                      * data
);

boolean wms_nv_delete_gw_sms
(
  wms_message_index_type   index,
  uint8                      * data
);


boolean wms_nv_read_gw_params
(
  wms_gw_template_s_type  * template_ptr,
  sys_modem_as_id_e_type  as_id
);


boolean wms_nv_write_gw_params
(
  wms_gw_template_s_type  * template_ptr,
  sys_modem_as_id_e_type  as_id
);

boolean wms_nv_read_gw_domain_pref
(
  sys_modem_as_id_e_type      as_id,
  wms_gw_domain_pref_e_type * gw_domain_pref
);

boolean wms_nv_write_gw_domain_pref
(
  sys_modem_as_id_e_type      as_id,
  wms_gw_domain_pref_e_type   gw_domain_pref
);

boolean wms_nv_read_gcf_flag
(
  boolean   * gcf_flag
);

boolean wms_nv_read_ens_flag
(
  boolean   * ens_flag
);

/*===========================================================================
FUNCTION wms_nv_fallback_to_telecom_smsp_is_allowed()

DESCRIPTION
  Returns whether it is allowed to use the SMSP entry in DFtelecom when
  the current USIM does not have EFsmsp present.

DEPENDENCIES
  None

RETURN VALUE
  Boolean:  TRUE    fallback (use of DFtelecom smsp) is allowed
            FALSE   fallback not allowed

SIDE EFFECTS
  None
==========================================================================*/
boolean wms_nv_fallback_to_telecom_smsp_is_allowed(
  void
);

#endif /* FEATURE_GWSMS */



boolean wms_nv_write_routing
(
  const wms_routes_s_type  *ptr
);

boolean wms_nv_read_routing
(
  wms_routes_s_type  *ptr
);

/*===========================================================================
FUNCTION wms_nv_read_bearer_prefs

DESCRIPTION
  Read the Bearer Preferences

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
);

/*===========================================================================
FUNCTION wms_nv_write_bearer_prefs

DESCRIPTION
  Write the Bearer Preferences to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
);
/*===========================================================================
FUNCTION wms_nv_read_max_payload_len

DESCRIPTION
  Read the SMS Max Payload Length

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_max_payload_len
(
  uint32   *max_Payload_Len
);

/*===========================================================================

FUNCTION wms_nv_efs_init

DESCRIPTION
  Initialize WMS EFS items.

DEPENDENCIES
  None

Parameter:
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_nv_efs_init(void);

boolean wms_nv_efs_read_refresh_vote_ok(
  boolean                 *val_out_p,
  sys_modem_as_id_e_type  as_id
);

wms_device_mode_e_type wms_nv_get_device_mode(
  void
);

sys_ue_mode_e_type wms_nv_get_ue_mode(
  void
);

void wms_nv_init_feature_mode(
  void
);


boolean wms_nv_rtre_control_is_nv(
  void
);

void  wms_nv_policy_chg_ind_proc(
   wms_policyman_cfg_s_type  *rpt_ptr
);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION WMS_NV_REFRESH_CB

DESCRIPTION
  Prcoess the NV refresh event.

RETURN VALUE
  None.

DEPENDENCIES
  NONE.
===========================================================================*/
void wms_nv_refresh_cb_proc
(
  wms_cmd_type *cmd_ptr
);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSNV_H */
