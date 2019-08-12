/*==============================================================================

          MODEM_CFG  -   S E T T I N G

GENERAL DESCRIPTION
  Simple APIs to access ModemCfg-specific NV settings.  Supports both new-style 
  MCFG NV items and old-style NV items, though it is recommended that 
  both exisiting and new code accessing old-style items use the legacy NV API
  when possible.  

EXTERNALIZED FUNCTIONS
  mcfg_nv_read
  mcfg_nv_write

INITIALIZATION AND SEQUENCING REQUIREMENTS
  EFS must be initialized

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_setting.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------------- 
01/09/14 sk   Fix for CR596963
02/14/12 sbt  Initial creation

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_int.h"

#include "mcfg_setting.h"
#include "persistent_device_configuration_v01.h"
#include "mcfg_uim.h"
#include "mcfg_utils_i.h"


/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Private Constants and Macros
==============================================================================*/

/*==============================================================================
  Private Typedefs
==============================================================================*/
/* MCFG Setting based on Slot
*/
typedef PACKED struct PACKED_POST
{
  uint32    version;

  /* Selection Reserved */
  uint8     selection_mode;
  uint8     flex_mapping;

  /* openmkrt subsidized carrier*/
  uint8     subsidized_carrier;
  uint8     selection_reserved;

  /* Refresh*/
  uint8     refresh_mode;
  uint8     refresh_timer;
  uint8     refresh_debug;
  uint8     refresh_reserved;

  /* SSR*/
  uint8     ssr_mode;
  uint8     ssr_timer;
  uint8     ssr_retries;
  uint8     ssr_retry_timer;

  /* Reserved */
  uint8     Reserved[8];
}mcfg_setting_s_type;

/*==============================================================================
  Private Variables
==============================================================================*/
mcfg_setting_s_type mcfg_setting[MCFG_NUM_OF_UIM_SLOT];

/*==============================================================================
  Private Function Prototypes
==============================================================================*/

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

void mcfg_feature_print
(
  mcfg_slot_index_type_e_type sloti
)
{
  MCFG_ASSERT_COND_RET(sloti < MCFG_NUM_OF_UIM_SLOT);

  MCFG_MSG_LOW_1("mcfg_setting.sloti %x", sloti);
  MCFG_MSG_LOW_1("mcfg_setting.version %x", mcfg_setting[sloti].version);
  MCFG_MSG_LOW_1("mcfg_setting.selection_mode %d", mcfg_setting[sloti].selection_mode);
  MCFG_MSG_LOW_1("mcfg_setting.flex_mapping %d", mcfg_setting[sloti].flex_mapping);
  MCFG_MSG_LOW_1("mcfg_setting.carrier %d", mcfg_setting[sloti].subsidized_carrier);
  MCFG_MSG_LOW_1("mcfg_setting.refresh_mode %d", mcfg_setting[sloti].refresh_mode);
  MCFG_MSG_LOW_1("mcfg_setting.refresh_timer %d", mcfg_setting[sloti].refresh_timer);
  MCFG_MSG_LOW_1("mcfg_setting.refresh_debug %d", mcfg_setting[sloti].refresh_debug);
  MCFG_MSG_LOW_1("mcfg_setting.ssr_mode %x", mcfg_setting[sloti].ssr_mode);
  
  return;
}

/*===========================================================================

  FUNCTION mcfg_sanity_check_feature

  DESCRIPTION
    Process the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    feature      [in]  feature setting for specified slot index

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_sanity_check_feature
(
  pdc_set_feature_req_msg_v01 *feature
)
{
  mcfg_uim_cmd_payload_s_type uim_cmd_payload;
  boolean reselect = FALSE;
  boolean changed = FALSE;
  mcfg_setting_s_type mcfg_setting_temp;

  MCFG_CHECK_NULL_PTR_RET_ERROR(feature, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(feature->slot_id < MCFG_NUM_OF_UIM_SLOT, MCFG_ERR_INVALID_PARAMETER);

  if (feature->carrier_valid)
  {
    if (PDC_SELECTION_OPEN_MARKET_V01 != feature->carrier)
    {
      if (feature->selection_mode_valid)
      {
         if(feature->selection_mode != PDC_SELECTION_MODE_DISABLED_V01)
         {
          MCFG_MSG_ERROR("sanity_check_feature subsidized with auto selection enabled");
          return MCFG_ERR_INVALID_PARAMETER;
         }
      }
      else if (mcfg_setting[feature->slot_id].selection_mode != PDC_SELECTION_MODE_DISABLED_V01)
      {
        MCFG_MSG_ERROR("sanity_check_feature subsidized while auto selection already enabled");
        return MCFG_ERR_INVALID_PARAMETER;
      }
    }
  }

  if ((feature->flex_mapping_valid && (feature->flex_mapping > 0)) 
      || (!feature->flex_mapping_valid && (mcfg_setting[feature->slot_id].flex_mapping > 0)))
  {
    if (feature->selection_mode_valid)
    {
       if(feature->selection_mode == PDC_SELECTION_MODE_DISABLED_V01)
       {
         MCFG_MSG_ERROR("sanity_check_feature enable flx mapping with auto selection disabled");
         return MCFG_ERR_INVALID_PARAMETER;
       }
    }
    else if (mcfg_setting[feature->slot_id].selection_mode == PDC_SELECTION_MODE_DISABLED_V01)
    {
      MCFG_MSG_ERROR("sanity_check_feature enable flx mapping while auto selection already disabled");
      return MCFG_ERR_INVALID_PARAMETER;
    }
  }

  if (feature->selection_mode_valid)
  {
    MCFG_ASSERT_COND_RET_ERROR(feature->selection_mode <= PDC_SELECTION_MODE_HYBRID_V01, MCFG_ERR_INVALID_PARAMETER);
  }

  if (feature->refresh_mode_valid)
  {
    MCFG_ASSERT_COND_RET_ERROR(feature->refresh_mode <= PDC_REFRESH_MODE_ENABLED_V01, MCFG_ERR_INVALID_PARAMETER);
  }

  return MCFG_ERR_NONE;
}

/*===========================================================================

  FUNCTION mcfg_set_feature

  DESCRIPTION
    Process the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    feature      [in]  feature setting for specified slot index

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_set_feature
(
  pdc_set_feature_req_msg_v01 *feature
)
{
  mcfg_uim_cmd_payload_s_type uim_cmd_payload;
  boolean reselect = FALSE;
  boolean changed = FALSE;
  mcfg_setting_s_type mcfg_setting_temp;

  MCFG_CHECK_NULL_PTR_RET_ERROR(feature, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(feature->slot_id < MCFG_NUM_OF_UIM_SLOT, MCFG_ERR_INVALID_PARAMETER);

  MCFG_MSG_LOW_1("mcfg_set_feature sloti%d", feature->slot_id);
  if(mcfg_sanity_check_feature(feature)!= MCFG_ERR_NONE)
  {
    MCFG_MSG_ERROR("mcfg_sanity_check_feature failed");
    return MCFG_ERR_INVALID_PARAMETER;
  }

  memscpy(&mcfg_setting_temp, sizeof(mcfg_setting_s_type),
          &mcfg_setting[feature->slot_id], sizeof(mcfg_setting_s_type));

  if (feature->carrier_valid)
  {
    if (mcfg_setting_temp.subsidized_carrier != feature->carrier)
    {
      mcfg_setting_temp.subsidized_carrier = feature->carrier;
      reselect = TRUE;
    }
  }

  if (feature->selection_mode_valid)
  {
    switch (feature->selection_mode)
    {
      case PDC_SELECTION_MODE_DISABLED_V01:
        changed = mcfg_uim_nv71546_update(feature->slot_id, 
                            0, 
                            MCFG_UIM_NV_AUTOSELECT_MASK|MCFG_UIM_NV_IMSISELECT_MASK);
        break;

      case PDC_SELECTION_MODE_IIN_V01:
      case PDC_SELECTION_MODE_IMSI_V01:
        feature->selection_mode = PDC_SELECTION_MODE_HYBRID_V01;

      case PDC_SELECTION_MODE_HYBRID_V01:
        changed = mcfg_uim_nv71546_update(feature->slot_id, 
                            MCFG_UIM_NV_AUTOSELECT_MASK|MCFG_UIM_NV_IMSISELECT_MASK, 
                            MCFG_UIM_NV_AUTOSELECT_MASK|MCFG_UIM_NV_IMSISELECT_MASK);
        break;

      default:
        return MCFG_ERR_NOT_SUPPORTED;
    }

    reselect = reselect || changed;
    mcfg_setting_temp.selection_mode = feature->selection_mode;
  }

  if (feature->flex_mapping_valid)
  {
    changed = mcfg_uim_nv71546_update(feature->slot_id, 
                        feature->flex_mapping << 1, 
                        MCFG_UIM_NV_FLEXMAP_MASK);
    reselect = reselect || changed;
    mcfg_setting_temp.flex_mapping = feature->flex_mapping;
  }

  if (feature->refresh_mode_valid)
  {
    switch (feature->refresh_mode)
    {
      case PDC_REFRESH_MODE_DISABLED_V01:
        changed = mcfg_uim_nv71546_update(feature->slot_id, 
                            MCFG_UIM_NV_REFRESH_MASK, 
                            MCFG_UIM_NV_REFRESH_MASK);
        break;

      case PDC_REFRESH_MODE_ENABLED_V01:
        changed = mcfg_uim_nv71546_update(feature->slot_id, 
                            0, 
                            MCFG_UIM_NV_REFRESH_MASK);
        break;

      default:
        return MCFG_ERR_NOT_SUPPORTED;
    }

    reselect = reselect || changed;
    mcfg_setting_temp.refresh_mode = feature->refresh_mode;
  }

  if (reselect)
  {
    uim_cmd_payload.sloti = feature->slot_id;
    mcfg_uim_cmd_send(MCFG_UIM_CMD_RESELECT, &uim_cmd_payload);
  }

  memscpy(&mcfg_setting[feature->slot_id], sizeof(mcfg_setting_s_type),
          &mcfg_setting_temp, sizeof(mcfg_setting_s_type));

  mcfg_uim_nv71546_write(feature->slot_id);
  mcfg_nv_write(MCFG_NV_ITEM_SETTING, 0, feature->slot_id,
                &mcfg_setting[feature->slot_id], 
                sizeof(mcfg_setting_s_type));

  mcfg_feature_print(feature->slot_id);
  return MCFG_ERR_NONE;
}


/*===========================================================================

  FUNCTION mcfg_get_feature

  DESCRIPTION
    Queries the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    sloti        [in]  specified slot index
    feature      [in]  pointer of feature settings

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_get_feature
(
  mcfg_slot_index_type_e_type sloti, 
  pdc_get_feature_ind_msg_v01 *feature
)
{
  boolean reselect = FALSE;

  MCFG_CHECK_NULL_PTR_RET_ERROR(feature, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(sloti < MCFG_NUM_OF_UIM_SLOT, MCFG_ERR_INVALID_PARAMETER);
  MCFG_MSG_LOW_1("mcfg_get_feature sloti%d", sloti);

  if (mcfg_setting[sloti].version == 0)
  {
    return MCFG_ERR_INTERNAL;
  }

  feature->carrier_valid = TRUE;
  feature->carrier = mcfg_setting[sloti].subsidized_carrier;

  feature->selection_mode_valid = TRUE;
  feature->selection_mode = 
    mcfg_uim_nv_autoselect_enabled(sloti) + (mcfg_uim_nv_imsiselect_enabled(sloti)<< 1);

  feature->flex_mapping_valid = TRUE;
  feature->flex_mapping = mcfg_uim_nv_flexmap_enabled(sloti);

  feature->refresh_mode_valid = TRUE;
  feature->refresh_mode = mcfg_uim_nv_refresh_enabled(sloti);
  
  mcfg_feature_print(sloti);
  return MCFG_ERR_NONE;
}

/*===========================================================================

FUNCTION mcfg_setting_init

DESCRIPTION
  Init setting from stored efs
  
DEPENDENCIES
  None.

RETURN VALUE
  booelan

SIDE EFFECTS

===========================================================================*/
boolean mcfg_setting_init(void) 
{
  mcfg_slot_index_type_e_type sloti;
  mcfg_uim_cmd_payload_s_type uim_cmd_payload;
  mcfg_nv_status_e_type nv_ret;

  memset(&mcfg_setting, 0, sizeof(mcfg_setting));
  for (sloti = 0; sloti < MCFG_NUM_OF_UIM_SLOT; sloti++)
  {
    nv_ret = mcfg_nv_read(MCFG_NV_ITEM_SETTING, 0, sloti, 
                          &mcfg_setting[sloti], sizeof(mcfg_setting_s_type));
    if(nv_ret != MCFG_NV_STATUS_OK || mcfg_setting[sloti].version != 1)
    {
      mcfg_setting[sloti].subsidized_carrier = 0;
    }

    if (mcfg_setting[sloti].subsidized_carrier == PDC_SELECTION_OPEN_MARKET_V01)
    {
      mcfg_setting[sloti].selection_mode = 
        mcfg_uim_nv_autoselect_enabled(sloti) + (mcfg_uim_nv_imsiselect_enabled(sloti)<< 1);
      mcfg_setting[sloti].flex_mapping = mcfg_uim_nv_flexmap_enabled(sloti);
    }
    else
    {
      mcfg_uim_nv71546_update(sloti, 0, MCFG_UIM_NV_AUTOSELECT_MASK|MCFG_UIM_NV_IMSISELECT_MASK);

      uim_cmd_payload.sloti = sloti;
      mcfg_uim_cmd_send(MCFG_UIM_CMD_RESELECT, &uim_cmd_payload);
    }
    mcfg_setting[sloti].refresh_mode = mcfg_uim_nv_refresh_enabled(sloti);

    mcfg_setting[sloti].version = 1;
  }

  mcfg_feature_print(sloti);

  return TRUE;
} /* mcfg_setting_init() */

