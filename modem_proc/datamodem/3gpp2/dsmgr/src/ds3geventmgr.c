/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3 G   D A T A   S E R V I C E S   E V E N T   M A N A G E R

GENERAL DESCRIPTION
  This software unit contains functions for 3G Data Services event handling.
  It provides APIs for DS3G event registration and notification.

  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:
  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/14   rs      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#include "modem_mem.h"

#include "ds3geventmgr.h"
#include "ds3gutil.h"
#include "ds3gmshif.h"
#include "ds3gmmgsdiif.h"

#include "mcfg_refresh.h"

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/

/*-------------------------------------------------------------------------
  Filter Structure, used for various filter options while publishing info.
-------------------------------------------------------------------------*/
typedef struct
{
  uint8           subs_type;
  uint8           tech_type;
}ds3geventmgr_ds3g_filter_type;

typedef struct
{
  ds3geventmgr_client_type       client_id;
  ds3geventmgr_event_type        event_id;
}ds3geventmgr_client_reg_info;

typedef struct
{
  q_link_type                    link;
  ds3geventmgr_client_type       client_id;
  ds3geventmgr_event_type        event_id;
  ds3geventmgr_ds3g_filter_type  filter;
  ds3geventmgr_notify_func_type  func_ptr[DS3GSUBSMGR_SUBS_ID_COUNT];
  void                          *data_ptr[DS3GSUBSMGR_SUBS_ID_COUNT];
}ds3g_event_data_block;

typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  mmgsdi_slot_id_enum_type  slot;
}ds3geventmgr_refresh_ind_cmd_type;

/*-------------------------------------------------------------------------
  Structure for Tech Specific cache for event info Query mechanism
-------------------------------------------------------------------------*/
typedef struct
{
  ds3geventmgr_attach_info_type             *attach_conf_info;
  ps_sys_null_resolved_apn_info_type        *null_resolved_apn_conf_info;
  ds3gevent_mgr_throttle_info_type             *pdn_throttle_conf_info;
  ds3geventmgr_pdn_status_info_type         *pdn_status_conf_info;
}ds3geventmgr_conf_tech_3gpp_type;

typedef struct
{
  ps_sys_null_resolved_apn_info_type        *null_resolved_apn_conf_info;
  ds3gevent_mgr_throttle_info_type             *pdn_throttle_conf_info;
  ps_sys_3gpp2_s101_status_change_type      *s101_status_chg_info;
  ds3geventmgr_pdn_status_info_type         *pdn_status_conf_info;
  ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type *handoff_complete_resp_info;
}ds3geventmgr_conf_tech_3gpp2_type;

typedef struct
{
  ps_sys_null_resolved_apn_info_type        *null_resolved_apn_conf_info;
  ds3gevent_mgr_throttle_info_type             *pdn_throttle_conf_info;
  ps_sys_wlan_max_conn_info_type            *wlan_max_conn_conf_info;
  ds3geventmgr_pdn_status_info_type         *pdn_status_conf_info;
}ds3geventmgr_conf_tech_wlan_type;

typedef struct
{
  ps_sys_null_resolved_apn_info_type        *null_resolved_apn_conf_info;
  ds3geventmgr_imsi_info_type               *imsi_conf_info;
  ds3geventmgr_impi_info_type               *impi_conf_info;
}ds3geventmgr_conf_tech_all_type;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Queue of registered events
---------------------------------------------------------------------------*/
static q_type ds3geventmgr_q;

/*---------------------------------------------------------------------------
  Bit mask to keep trach of which clients have registered for NV refresh
---------------------------------------------------------------------------*/
static uint8 ds3geventmgr_nv_refresh_client_reg_status = 0;

/*---------------------------------------------------------------------------
  Hold the processing status of all the DS internal modules during
  LPM/POWER OFF event from Call manager. Each bit maps to one of DS
  internal modules that require clean up of its own during
  LPM/POWER OFF EVENT. The bit wise mapping is shown below


   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
  | BIT 7| BIT 6| BIT 5| BIT 4| BIT 3| BIT 2| BIT  1| BIT 0 |
   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |        - - > 3GPP2_CTRL_BIT
     |      |      |      |      |      |       |
     |      |      |      |      |      |        - - - - - - > 3GPP_CTRL_BIT
     |      |      |      |      |      |
     |      |      |      |      |       - - - - - - - - - - > WLAN_CTRL_BIT
     |      |      |      |      |
     |      |      |      |       - - - - - - - - - - - - - -> FUTURE USE
     |      |      |      |
     |      |      |       - - - - - - - - - - - - - - - - - > FUTURE USE
     |      |      |
     |      |       - - - - - - - - - - - - - - - - - - - - -> FUTURE USE
     |      |
     |       - - - - - - - - - - - - - - - - - - - - - - - - > FUTURE USE
     |
      - - - - - - - - - - - - - - - - - - - - - - - - - - - -> LPM PWR OFF
                                                               MASTER_CTRL_BIT

   LPM_PWR_OFF_MASTER_CTRL_BIT -- Bit Set     : Soon after CM notifies
                                  -------       DS of LPM/POWER OFF EVENT.

                                  Bit Cleared : After all the DS internal
                                  -----------   modules have finished clean up
                                                of all data calls

   LPM_PWR_OFF_3GPP_CTRL_BIT  --  Bit Set     : Soon after 3GPP MH is triggered
                                  -------       to process LPM/PWR OFF EVENT
                                  Bit Cleared : After 3GPP MH has finished
                                  -----------   cleanup of all its data calls
                                                (either gracefully (or) local
                                                clean up)
  LPM_PWR_OFF_3GPP2_CTRL_BIT  --  Bit Set     : Soon after 3GPP2 MH is triggered
                                  -------       to process LPM/PWR OFF EVENT
                                  Bit Cleared : After 3GPP2 MH has finished
                                  -----------   cleanup of all its data calls
                                                (either gracefully (or) local
                                                clean up)
   LPM_PWR_OFF_WLAN_CTRL_BIT  --  Bit Set     : Soon after iWLAN MH is triggered
                                  -------       to process LPM/PWR OFF EVENT
                                  Bit Cleared : After iWLAN MH has finished
                                  -----------   cleanup of all its data calls
                                                (either gracefully (or) local
                                                clean up)                                                
  ---------------------------------------------------------------------------*/
static uint8  ds3geventmgr_lpm_poweroff_status = 0;

/*---------------------------------------------------------------------------
  Hold the processing status of all the DS internal modules during
  SUB_NOT_AVAIL event from Call manager. Each bit maps to one of DS
  internal modules that require clean up of its own during
  SUB_NOT_AVAIL EVENT. The bit wise mapping is shown below.
  This is per subs id.


   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
  | BIT 7| BIT 6| BIT 5| BIT 4| BIT 3| BIT 2| BIT  1| BIT 0 |
   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |        - - > 3GPP2_CTRL_BIT
     |      |      |      |      |      |       |
     |      |      |      |      |      |        - - - - - - > 3GPP_CTRL_BIT
     |      |      |      |      |      |
     |      |      |      |      |       - - - - - - - - - - > FUTURE USE
     |      |      |      |      |
     |      |      |      |       - - - - - - - - - - - - - -> FUTURE USE
     |      |      |      |
     |      |      |       - - - - - - - - - - - - - - - - - > FUTURE USE
     |      |      |
     |      |       - - - - - - - - - - - - - - - - - - - - -> FUTURE USE
     |      |
     |       - - - - - - - - - - - - - - - - - - - - - - - - > FUTURE USE
     |
      - - - - - - - - - - - - - - - - - - - - - - - - - - - -> SUB_NOT_AVAIL
                                                               MASTER_CTRL_BIT

   SUB_NOT_AVAIL_MASTER_CTRL_BIT -- Bit Set     : Soon after CM notifies
                                    -------       DS of SUB_NOT_AVAIL EVENT.

                                  Bit Cleared : After all the DS internal
                                  -----------   modules have finished clean up
                                                of all data calls

   SUB_NOT_AVAIL_3GPP_CTRL_BIT  --  Bit Set     : Soon after 3GPP MH is triggered
                                    -------       to process SUB_NOT_AVAIL EVENT
                                  Bit Cleared : After 3GPP MH has finished
                                  -----------   cleanup of all its data calls
                                                (either gracefully (or) local
                                                clean up)
   SUB_NOT_AVAIL_3GPP2_CTRL_BIT  --  Bit Set     : Soon after 3GPP2 MH is triggered
                                     -------       to process SUB_NOT_AVAIL EVENT
                                  Bit Cleared : After 3GPP2 MH has finished
                                  -----------   cleanup of all its data calls
                                                (either gracefully (or) local
                                                clean up)
  ---------------------------------------------------------------------------*/
static uint8  ds3geventmgr_sub_not_avail_status[DS3GSUBSMGR_SUBS_ID_COUNT] = {0};

/*---------------------------------------------------------------------------
  Hold the processing status of all the DS internal modules during
  NV refresh event from MCFG. Each bit maps to one of DS
  internal modules that require refreshing of its NV items during NV
  refresh. The bit wise mapping is shown below


   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
  | BIT 7| BIT 6| BIT 5| BIT 4| BIT 3| BIT 2| BIT  1| BIT 0 |
   = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |       |
     |      |      |      |      |      |       |        - - > 3GPP2_CTRL_BIT
     |      |      |      |      |      |       |
     |      |      |      |      |      |        - - - - - - > 3GPP_CTRL_BIT
     |      |      |      |      |      |
     |      |      |      |      |       - - - - - - - - - - > MPPM_CTRL_BIT
     |      |      |      |      |
     |      |      |      |       - - - - - - - - - - - - - -> WLAN_CTRL_BIT
     |      |      |      |
     |      |      |       - - - - - - - - - - - - - - - - - > ATCOP_CTRL_BIT
     |      |      |
     |      |       - - - - - - - - - - - - - - - - - - - - -> PROFILE_CTRL_BIT
     |      |
     |       - - - - - - - - - - - - - - - - - - - - - - - - > FUTURE USE
     |
      - - - - - - - - - - - - - - - - - - - - - - - - - - - -> FUTURE USE

   Bits are set when modules finish their actions for NV refresh. Bits are
   all cleared once all required bits are set (i.e. all modules have finished).
---------------------------------------------------------------------------*/
static uint8  ds3geventmgr_nv_refresh_status = 0;

/* reference id assigned by mcfg for NV refresh */
static uint32  ds3geventmgr_mcfg_refresh_reference_id = 0;

/*-------------------------------------------------------------------------
  Array that caches tech specific event info
-------------------------------------------------------------------------*/
static ds3geventmgr_conf_tech_3gpp_type         ds3geventmgr_conf_tech_3gpp
                                                [DS3GSUBSMGR_SUBS_ID_COUNT];

static ds3geventmgr_conf_tech_3gpp2_type        ds3geventmgr_conf_tech_3gpp2
                                                [DS3GSUBSMGR_SUBS_ID_COUNT];

static ds3geventmgr_conf_tech_wlan_type         ds3geventmgr_conf_tech_wlan
                                                [DS3GSUBSMGR_SUBS_ID_COUNT];

static ds3geventmgr_conf_tech_all_type          ds3geventmgr_conf_tech_all
                                                [DS3GSUBSMGR_SUBS_ID_COUNT];

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CHECK_IF_WLAN_CLIENT_IS_REG

DESCRIPTION   returns whether WLAN client is registered or not for specific event
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - If registered
              FALSE - If Not

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3geventmgr_check_if_wlan_client_is_reg 
(
  ds3geventmgr_event_type        event_id
);

/*===========================================================================
FUNCTION      ds3geventmgr_compare_function

DESCRIPTION   Utility function to compare two items.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if itemsa are not equal

SIDE EFFECTS  None.
===========================================================================*/
int ds3geventmgr_compare_function
(
  void  *item_ptr,
  void  *target_val
)
{
  ds3g_event_data_block         *data_block = NULL;
  ds3geventmgr_client_reg_info  *to_compare = NULL;
  boolean                        ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((item_ptr == NULL) ||(target_val == NULL))
  {
    return FALSE;
  }
  data_block = (ds3g_event_data_block *)item_ptr;
  to_compare = (ds3geventmgr_client_reg_info *)target_val;

  if((data_block->client_id == to_compare->client_id) &&
     (data_block->event_id == to_compare->event_id))
  {
    ret_val = TRUE;
  }
  return ret_val;
}/* ds3geventmgr_compare_function()*/

/*===========================================================================
FUNCTION      ds3geventmgr_mcfg_set_reference_id

DESCRIPTION   This function sets the reference id passed by mcfg during nv
              refresh.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3geventmgr_mcfg_set_reference_id
(
  uint32  reference_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds3geventmgr_mcfg_refresh_reference_id = reference_id;
  return;
}/* ds3geventmgr_mcfg_set_reference_id() */

/*===========================================================================
FUNCTION      ds3geventmgr_mcfg_get_reference_id

DESCRIPTION   This function gets the reference id passed by mcfg during nv
              refresh.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds3geventmgr_mcfg_get_reference_id( void )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds3geventmgr_mcfg_refresh_reference_id;
}/* ds3geventmgr_mcfg_get_reference_id() */

/*===========================================================================
FUNCTION      ds3geventmgr_convert_subs_mcfg_to_cm

DESCRIPTION   This function converts mcfg subs index to cm subs id.

DEPENDENCIES  None.

RETURN VALUE  CM subs id.

SIDE EFFECTS  None.
===========================================================================*/
sys_modem_as_id_e_type ds3geventmgr_convert_subs_mcfg_to_cm
(
  mcfg_refresh_index_type  subs_index
)
{
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(subs_index)
  {
    case MCFG_REFRESH_INDEX_0:
      subs_id = SYS_MODEM_AS_ID_1;
      break;

    case MCFG_REFRESH_INDEX_1:
      subs_id = SYS_MODEM_AS_ID_2;
      break;

    case MCFG_REFRESH_INDEX_2:
      subs_id = SYS_MODEM_AS_ID_3;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "mcfg_to_cm subs unknown: %d", subs_index);
      break;
  }

  return subs_id;
}/* ds3geventmgr_convert_subs_mcfg_to_cm() */

/*===========================================================================
FUNCTION      ds3geventmgr_convert_subs_mcfg_to_cm

DESCRIPTION   This function converts mcfg slot index to mmgsdi slot.

DEPENDENCIES  None.

RETURN VALUE  mmgsdi slot id.

SIDE EFFECTS  None.
===========================================================================*/
mmgsdi_slot_id_enum_type ds3geventmgr_convert_slot_mcfg_to_mmgsdi
(
  mcfg_refresh_index_type  slot_index
)
{
  mmgsdi_slot_id_enum_type slot = MMGSDI_MAX_SLOT_ID_ENUM;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(slot_index)
  {
    case MCFG_REFRESH_INDEX_0:
      slot = MMGSDI_SLOT_1;
      break;

    case MCFG_REFRESH_INDEX_1:
      slot = MMGSDI_SLOT_2;
      break;

    case MCFG_REFRESH_INDEX_2:
      slot = MMGSDI_SLOT_3;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "mcfg_to_cm slot unknown: %d", slot_index);
      break;
  }

  return slot;
}/* ds3geventmgr_convert_slot_mcfg_to_mmgsdi() */

/*===========================================================================
FUNCTION      ds3geventmgr_mcfg_refresh_ind_cb

DESCRIPTION   This function is the callback to process mcfg refresh ind.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3geventmgr_mcfg_refresh_ind_cb
(
  mcfg_refresh_info_s_type  *mcfg_refresh_info_ptr
)
{
  sys_modem_as_id_e_type              subs_id = SYS_MODEM_AS_ID_NONE;
  mmgsdi_slot_id_enum_type            slot = MMGSDI_MAX_SLOT_ID_ENUM;
  ds_cmd_type                        *cmd_ptr = NULL;
  ds3geventmgr_refresh_ind_cmd_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3geventmgr_mcfg_set_reference_id(mcfg_refresh_info_ptr->reference_id);

  subs_id =
    ds3geventmgr_convert_subs_mcfg_to_cm(mcfg_refresh_info_ptr->sub_index);

  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE )
  {
    return FALSE;
  }

  slot =
    ds3geventmgr_convert_slot_mcfg_to_mmgsdi(mcfg_refresh_info_ptr->slot_index);

  if( (slot < MMGSDI_SLOT_1) || (slot > MMGSDI_SLOT_AUTOMATIC) )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Invalid slot %d passed by mcfg",slot);
    return FALSE;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3geventmgr_mcfg_refresh_ind_cb reference_id:%d subs_id:%d "
                  "slot:%d",mcfg_refresh_info_ptr->reference_id, subs_id, slot);

  /*-------------------------------------------------------------------------
    Post a command to DS task to notify all modules about MCFG refresh Ind
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3geventmgr_refresh_ind_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return FALSE;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_MCFG_REFRESH_IND;
    data_block_ptr = (ds3geventmgr_refresh_ind_cmd_type*)cmd_ptr->cmd_payload_ptr;
    data_block_ptr->slot = slot;
    data_block_ptr->subs_id = subs_id;
    ds_put_cmd(cmd_ptr);
  }

  return TRUE;
} /* ds3geventmgr_mcfg_refresh_ind_cb() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_INIT_TECH_CONF_CACHE

DESCRIPTION   This function performs initialization of Tech specific cnf cache

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_init_tech_conf_cache
(
  void
)
{
  uint8  index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index = 0; index < DS3GSUBSMGR_SUBS_ID_MAX; index++)
  {
    ds3geventmgr_conf_tech_3gpp[index].attach_conf_info = NULL;
    ds3geventmgr_conf_tech_3gpp[index].null_resolved_apn_conf_info = NULL;
    ds3geventmgr_conf_tech_3gpp[index].pdn_throttle_conf_info = NULL;

    ds3geventmgr_conf_tech_3gpp2[index].null_resolved_apn_conf_info = NULL;
    ds3geventmgr_conf_tech_3gpp2[index].pdn_throttle_conf_info = NULL;

    ds3geventmgr_conf_tech_wlan[index].null_resolved_apn_conf_info = NULL;
    ds3geventmgr_conf_tech_wlan[index].pdn_throttle_conf_info = NULL;
    ds3geventmgr_conf_tech_wlan[index].wlan_max_conn_conf_info = NULL;

    ds3geventmgr_conf_tech_all[index].null_resolved_apn_conf_info = NULL;
    ds3geventmgr_conf_tech_all[index].imsi_conf_info = NULL;
    ds3geventmgr_conf_tech_all[index].impi_conf_info = NULL;
  }
}/*ds3geventmgr_init_tech_conf_cache*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_PERFORM_MASK_VALIDATION

DESCRIPTION   This function performs mask validation to check only 1 bit is
              set at a time since we notify/set/get payload one subs/Tech
              at a time.

DEPENDENCIES  None

RETURN VALUE  TRUE: If mask is valid as per the design
              FALSE: Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_perform_mask_validation
(
  uint32  mask
)
{
  uint8    no_of_bits_set = 0;
  uint8    index = 0;
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < 32; index++)
  {
    if((mask >> index) & 0x01)
    {
      no_of_bits_set++;
      if(no_of_bits_set > 1)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "2 Bits set while providing Subs, 2nd Bit: %d", index);
        ret_val = FALSE;
        break;
      }
    }
  }

  return ret_val;
}/*ds3geventmgr_perform_set_filter_mask_validation*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_MAP_PS_TO_DS3G_FILTER

DESCRIPTION   This function validates the filter info received from clients
              and converts it to DS3G specific filter info

DEPENDENCIES  None

RETURN VALUE  TRUE: If validation was successful
              FALSE: Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_map_ps_to_ds3g_filter
(
  ds3geventmgr_filter_type       *ps_filter_info_ptr,
  ds3geventmgr_ds3g_filter_type  *ds3g_filter_info_ptr
)
{
  boolean                 ret_val = FALSE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ps_filter_info_ptr == NULL ||
     ds3g_filter_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid Input params");
    return ret_val;
  }

  if((ps_filter_info_ptr->tech < PS_SYS_TECH_MIN ||
     ps_filter_info_ptr->tech > PS_SYS_TECH_ALL) ||
     (ps_filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS ||
      ps_filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Tech Type or Subs Passed: %d or %d",
                    ps_filter_info_ptr->tech, ps_filter_info_ptr->ps_subs_id);
    return ret_val;
  }

  memset(ds3g_filter_info_ptr, 0, sizeof(ds3geventmgr_ds3g_filter_type));
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm((ds_sys_subscription_enum_type)
                                         ps_filter_info_ptr->ps_subs_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  DS3GEVENTMGR_MAP_SUBS_ID_TO_MASK(subs_id, ds3g_filter_info_ptr->subs_type);

  if(ps_filter_info_ptr->tech == PS_SYS_TECH_ALL)
  {
    ds3g_filter_info_ptr->tech_type = DS3G_TECH_ALL;
  }
  else
  {
    ds3g_filter_info_ptr->tech_type =
      (1 << (ps_filter_info_ptr->tech));
  }

  ret_val = TRUE;

  return ret_val;
} /* ds3geventmgr_map_ps_to_ds3g_filter() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_MAP_DS3G_TO_PS_FILTER

DESCRIPTION   This function validates the filter info received from clients
              and converts it to DS3G specific filter info

DEPENDENCIES  None

RETURN VALUE  TRUE: If validation was successful
              FALSE: Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_map_ds3g_to_ps_filter
(
  ds3geventmgr_filter_type       *ps_filter_info_ptr,
  ds3geventmgr_ds3g_filter_type  *ds3g_filter_info_ptr
)
{
  boolean                 ret_val = FALSE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(ps_filter_info_ptr, 0, sizeof(ds3geventmgr_filter_type));

  DS3GEVENTMGR_MAP_SUBS_MASK_TO_ID(subs_id,
     ds3g_filter_info_ptr->subs_type);

  ps_filter_info_ptr->ps_subs_id = (ps_sys_subscription_enum_type)
    ds3gsubsmgr_subs_id_cm_to_ds(subs_id);

  DS3GEVENTMGR_MAP_DS3G_TO_PS_TECH(ps_filter_info_ptr->tech,
                                   ds3g_filter_info_ptr->tech_type);

  if((ps_filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS ||
      ps_filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS) ||
     (ps_filter_info_ptr->tech < PS_SYS_TECH_MIN ||
      ps_filter_info_ptr->tech > PS_SYS_TECH_ALL))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Subscription ID: %d or Tech Mask: %d, rejected",
                    ps_filter_info_ptr->ps_subs_id, ps_filter_info_ptr->tech);
    memset(ps_filter_info_ptr, 0, sizeof(ds3geventmgr_filter_type));
    ret_val = FALSE;
  }
  else
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* ds3geventmgr_map_ds3g_to_ps_filter */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_PERFORM_DEREG_FILTER_MASK_VALIDATION

DESCRIPTION   This function checks the de-reg mask for its validity, if you
              have not registerted for a particular Sub or Tech, you cannot
              try to de-reg for that subs/tech.

DEPENDENCIES  None

RETURN VALUE  TRUE: If mask was valid
              FALSE: Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_perform_dereg_filter_mask_validation
(
  ds3geventmgr_ds3g_filter_type  *queue_element_filter,
  ds3geventmgr_ds3g_filter_type  *input_filter
)
{
  uint8    last_bit_queue_element = 0x00;
  uint8    last_bit_input_filter = 0x00;
  uint8    index = 0;
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index = 0; index < 8; index++)
  {
    last_bit_queue_element = (queue_element_filter->subs_type >> index) & 0x01;

    last_bit_input_filter = (input_filter->subs_type >> index) & 0x01;

    if(((last_bit_queue_element ^ last_bit_input_filter) == 0x01) &&
       (last_bit_queue_element == 0x00))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Iteration at which mask validation failed: %d", index);
      ret_val = FALSE;
      break;
    }

    last_bit_queue_element = (queue_element_filter->tech_type >> index) & 0x01;

    last_bit_input_filter = (input_filter->tech_type >> index) & 0x01;

    if(((last_bit_queue_element ^ last_bit_input_filter) == 0x01) &&
       (last_bit_queue_element == 0x00))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Iteration at which mask validation failed: %d", index);
      ret_val = FALSE;
      break;
    }
  }

  return ret_val;
}/*ds3geventmgr_perform_filter_mask_validation*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_EVENT_NOTIFY

DESCRIPTION   Function to notify registered clients.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3geventmgr_event_notify
(
  ds3geventmgr_event_type    event_id,
  void                      *event_info,
  ds3geventmgr_filter_type  *ps_filter_info_p
)
{
  uint8                           arr_index = 0;
  ds3g_event_data_block          *event_block = NULL;
  ds3geventmgr_ds3g_filter_type  ds3g_filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_event_notify event_id:%d",
                  event_id);

  if(ps_filter_info_p == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid Filter Mask, Exiting!!!");
    return;
  }

  if(ds3geventmgr_map_ps_to_ds3g_filter(ps_filter_info_p, &ds3g_filter_info) == 
     FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to map PS filter information to DS3G Filter Info");
    return;
  }

  arr_index = (uint8)ds3gsubsmgr_subs_id_ds_to_ds3g
     ((ds_sys_subscription_enum_type)ps_filter_info_p->ps_subs_id);

  if (arr_index > DS3GSUBSMGR_SUBS_ID_COUNT) 
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Invalid Subscription ID: %d", arr_index);
    return;
  }

  event_block = (ds3g_event_data_block*)q_check(&ds3geventmgr_q);
  while(event_block != NULL)
  {
    if((event_id == event_block->event_id) &&
       (event_block->filter.subs_type & ds3g_filter_info.subs_type) &&
       (event_block->filter.tech_type & ds3g_filter_info.tech_type))
    {
      if(event_block->func_ptr[arr_index] != NULL)
      {
        event_block->func_ptr[arr_index](event_id, ps_filter_info_p, 
                    event_info, event_block->data_ptr[arr_index]);
      }
    }

    event_block = (ds3g_event_data_block *)q_next(&ds3geventmgr_q,
                                                  &(event_block->link));
  }
  return;
}/* ds3geventmgr_event_notify() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_INVOKE_CBACKS

DESCRIPTION   This function invokes cback immediately if event info is
              available during registration

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_invoke_cbacks
(
  ds3geventmgr_event_type                      event_id,
  ds3geventmgr_ds3g_filter_type               *ds3g_filter_info_ptr
)
{
  uint8                          subs_index = 0;
  uint8                          internal_index = 0;
  ds3geventmgr_event_info_type   event_info;
  ds3geventmgr_ds3g_filter_type  local_filter_info;
  ds3geventmgr_filter_type       ps_filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&local_filter_info, 0, sizeof(ds3geventmgr_ds3g_filter_type));

  for(subs_index = 0; subs_index < DS3GSUBSMGR_SUBS_ID_COUNT; subs_index++)
  {
    if((ds3g_filter_info_ptr->subs_type >> subs_index) & 0x01)
    {
      DS3GEVENTMGR_MAP_SUBS_ID_TO_MASK(subs_index, local_filter_info.subs_type);

      if ( ds3g_filter_info_ptr->tech_type == DS3G_TECH_ALL )
      {
        if(ds3geventmgr_map_ds3g_to_ps_filter(&ps_filter_info,
                                               ds3g_filter_info_ptr))
        {
          if(ds3geventmgr_get_conf_info(event_id, (void *)&event_info,
                                        &ps_filter_info) == TRUE)
          {
            ds3geventmgr_event_notify(event_id,
                                      &event_info, &ps_filter_info);
          }
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Mapping to PS Filter Info Failed");
        }
      }

      else
      {
        for(internal_index =0 ; internal_index < 3; internal_index++)
        {
          if((ds3g_filter_info_ptr->tech_type >> internal_index) & 0x01)
          {
            DS3GEVENTMGR_SET_TECH_TYPE_MASK(internal_index,
                                            local_filter_info.tech_type);

            memset(&event_info, 0, sizeof(ds3geventmgr_event_info_type));
            memset(&ps_filter_info, 0, sizeof(ds3geventmgr_filter_type));

            if(ds3geventmgr_map_ds3g_to_ps_filter(&ps_filter_info,
                                                  &local_filter_info))
            {
              if(ds3geventmgr_get_conf_info(event_id, (void *)&event_info, 
                                            &ps_filter_info) == TRUE)
              {
                ds3geventmgr_event_notify(event_id,
                                          &event_info, &ps_filter_info);
              }
              else
              {
                DATA_3GMGR_MSG1(MSG_LEGACY_MED, 
                                "No Info Present for Event: %d",event_id);
              }
            }
            else
            {
              DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Mapping to PS Filter Info Failed");
            }
          }
        }
      }
    }
  }
} /* ds3geventmgr_invoke_cbacks() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_INIT

DESCRIPTION   This function performs initialization of ds3geventmgr.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_init( void )
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_refresh_registration_s_type mcfg_refresh_reg;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize event queues for event registration and notification */
  q_init(&ds3geventmgr_q);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* register for mcfg refresh ind */
  memset(&mcfg_refresh_reg,0,sizeof(mcfg_refresh_registration_s_type));
  mcfg_refresh_reg.order = MCFG_REFRESH_ORDER_50;
  mcfg_refresh_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_refresh_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_refresh_reg.cb = ds3geventmgr_mcfg_refresh_ind_cb;

  if(mcfg_refresh_register(&mcfg_refresh_reg) == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Failed to register for mcfg refresh ind");
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  ds3geventmgr_init_tech_conf_cache();

  return;
} /* ds3geventmgr_init */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_PROCESS_MCFG_REFRESH_IND_CMD

DESCRIPTION   This function process MCFG refresh ind command

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_process_mcfg_refresh_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3geventmgr_event_info_type        event_info;
  ds3geventmgr_refresh_ind_cmd_type  *data_block_ptr = NULL;
  ds3geventmgr_filter_type            filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  data_block_ptr
    = (ds3geventmgr_refresh_ind_cmd_type*)(cmd_ptr->cmd_payload_ptr);

  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                  "ds3g_process_mcfg_refresh_ind_cmd subs_id:%d",
                  data_block_ptr->subs_id);

  /* Notify DS3G modules about MCFG refresh Indication */
  ds3g_process_mcfg_refresh_ind(data_block_ptr->subs_id);

  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                            ds3gsubsmgr_subs_id_cm_to_ds(
                                              data_block_ptr->subs_id);
  filter_info.tech = PS_SYS_TECH_ALL;

  /* Notify all clients about MCFG refresh Indication */
  /* TODO: check if mcfg_refresh_info_ptr->type also need to be sent */
  memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));
  event_info.nv_refresh_info.slot = data_block_ptr->slot;
  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_NV_REFRESH_EV,
                                 (void *)&event_info,&filter_info) == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to provide NV REFRESH info to registered clients");
  }

  return;
} /* ds3geventmgr_process_mcfg_refresh_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_EVENT_REG

DESCRIPTION   Called by clients to register to get DS3G events.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If registration successful.
              FALSE: If registration unsuccessful.

SIDE EFFECTS  Registered client is entered on a queue.
===========================================================================*/
boolean ds3geventmgr_event_reg
(
  ds3geventmgr_event_type         event_id,
  ds3geventmgr_client_type        client_id,
  ds3geventmgr_filter_type       *filter_info_p,
  ds3geventmgr_notify_func_type   cb_func_ptr,
  void                           *cb_data_ptr
)
{
  uint8                            arr_index = 0;
  boolean                          ret_val = FALSE;
  ds3g_event_data_block           *event_block_to_insert = NULL;
  ds3geventmgr_client_reg_info     client_event_info;
  ds3geventmgr_ds3g_filter_type    ds3g_filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3g_event_reg event:%d", event_id);

  if((event_id <= DS3GEVENTMGR_INVALID_EV || event_id >= DS3GEVENTMGR_MAX_EV) ||
     ds3geventmgr_perform_mask_validation((uint32)client_id) == FALSE ||
     filter_info_p == NULL)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Input Params Client ID: %d or Filter Info: 0x%x",
                    client_id, filter_info_p);
    return ret_val;
  }

  if(ds3geventmgr_map_ps_to_ds3g_filter(filter_info_p, &ds3g_filter_info) ==
     FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to map PS filter information to DS3G Filter Info");
    return ret_val;
  }

  arr_index = (uint8)ds3gsubsmgr_subs_id_ds_to_ds3g
     ((ds_sys_subscription_enum_type)filter_info_p->ps_subs_id);

  if (arr_index > DS3GSUBSMGR_SUBS_ID_COUNT) 
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Invalid Subscription ID: %d", arr_index);
    return ret_val;
  }

  if(event_id == DS3GEVENTMGR_NV_REFRESH_EV)
  {
    ds3geventmgr_nv_refresh_client_reg_status |= client_id;
  }

  memset(&client_event_info, 0, sizeof(ds3geventmgr_client_reg_info));
  client_event_info.client_id = client_id;
  client_event_info.event_id = event_id;

  event_block_to_insert = (ds3g_event_data_block *)q_linear_search(
                                                 &ds3geventmgr_q,
                                                 ds3geventmgr_compare_function,
                                                 (void *)&client_event_info);

  if(event_block_to_insert != NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Client and Event ID pair already exists, Updating");

    event_block_to_insert->func_ptr[arr_index] = cb_func_ptr;
    event_block_to_insert->data_ptr[arr_index] = cb_data_ptr;

    event_block_to_insert->filter.subs_type |= ds3g_filter_info.subs_type;
    event_block_to_insert->filter.tech_type |= ds3g_filter_info.tech_type;

    ret_val = TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Allocate memory on heap
    -----------------------------------------------------------------------*/
    event_block_to_insert = (ds3g_event_data_block*)modem_mem_alloc(
                                            sizeof(ds3g_event_data_block),
                                            MODEM_MEM_CLIENT_DATA);

    if(event_block_to_insert != NULL)
    {
      memset(event_block_to_insert, 0 ,sizeof(ds3g_event_data_block));
      event_block_to_insert->event_id = event_id;
      event_block_to_insert->func_ptr[arr_index] = cb_func_ptr;
      event_block_to_insert->data_ptr[arr_index] = cb_data_ptr;
      event_block_to_insert->client_id = client_id;

      event_block_to_insert->filter.subs_type |= ds3g_filter_info.subs_type;
      event_block_to_insert->filter.tech_type |= ds3g_filter_info.tech_type;

      q_link((void*)event_block_to_insert, &(event_block_to_insert->link));
      q_put(&ds3geventmgr_q, &(event_block_to_insert->link));

      ret_val = TRUE;
    }
    else
    {
      DATA_ERR_FATAL("Unable to allocate memory on heap during reg, ASSERT !!");
    }
  }

  if(ret_val == TRUE)
  {
    ds3geventmgr_invoke_cbacks(event_id, &ds3g_filter_info);
  }

  return ret_val;
} /* ds3geventmgr_event_reg */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_EVENT_DEREG

DESCRIPTION   Called by clients to de-register DS3G event notification.

DEPENDENCIES  None.

RETURN VALUE  TRUE: If de-registration successful.
                    If client previously de-registgered.

              FALSE: If de-registration unsuccessful.

Cavaet:       If a client has registered for 2 Subscriptions and Mask contains
              de-registration request for 1 subscription only, Client will
              stay registered for other subscription

              Entry will be deleted from registration queue only when all
              registered tech types & subscriptions are de-registered

SIDE EFFECTS  De-registered client is dequeued.
===========================================================================*/
boolean ds3geventmgr_event_dereg
(
  ds3geventmgr_client_type       client_id,
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type      *filter_info_p
)
{
  boolean                       ret_val = FALSE;
  ds3g_event_data_block        *event_block = NULL;
  ds3geventmgr_client_reg_info  client_event_info;
  ds3geventmgr_ds3g_filter_type ds3g_filter_info;
  boolean                       deletion_required = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3g_event_dereg() client_id: %d event: %d",
                  client_id, event_id);

  if((event_id <= DS3GEVENTMGR_INVALID_EV || event_id >= DS3GEVENTMGR_MAX_EV) ||
     ds3geventmgr_perform_mask_validation((uint32)client_id) == FALSE)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Event ID: %d or Client ID: %d",event_id, client_id);
    return FALSE;
  }

  if(ds3geventmgr_map_ps_to_ds3g_filter(filter_info_p, &ds3g_filter_info) ==
     FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unable to map PS filter information to DS3G Filter Info");
    return ret_val;
  }

  memset(&client_event_info, 0, sizeof(ds3geventmgr_client_reg_info));
  client_event_info.client_id = client_id;
  client_event_info.event_id = event_id;

  event_block = q_linear_search(&ds3geventmgr_q,
                                ds3geventmgr_compare_function,
                                (void *)&client_event_info);

  /*-----------------------------------------------------------------------
   Here, we only delete the block from Event MGR queue if the current
   registration mask matches
  -----------------------------------------------------------------------*/

  if(event_block != NULL)
  {
    if(ds3geventmgr_perform_dereg_filter_mask_validation (&event_block->filter,
                                                          &ds3g_filter_info)== FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Invalid Filter Mask provided,Dereg Failed !!!!");
      return ret_val;
    }

    if(((ds3g_filter_info.subs_type & ~(event_block->filter.subs_type))
        == 0) &&
       ((ds3g_filter_info.tech_type & ~(event_block->filter.tech_type))
        == 0))
    {
      deletion_required = TRUE;
    }
    else
    {
      event_block->filter.subs_type = (event_block->filter.subs_type ^
                                       ds3g_filter_info.subs_type);
      event_block->filter.tech_type = (event_block->filter.tech_type ^
                                       ds3g_filter_info.tech_type);
    }
    if(deletion_required == TRUE)
    {
      q_delete(&ds3geventmgr_q, &event_block->link);
      /* free allocated memory  */
      modem_mem_free(event_block, MODEM_MEM_CLIENT_DATA);
      ret_val = TRUE;
    }

    /* clear nv refresh reg status for this client */
    if(event_id == DS3GEVENTMGR_NV_REFRESH_EV)
    {
      ds3geventmgr_nv_refresh_client_reg_status &= ~(client_id);
    }
  }

  return ret_val;
} /* ds3geventmgr_event_dereg */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_EVENT_INFO

DESCRIPTION   Called by clients to publish payload info to be given to
              registered clients.

DEPENDENCIES  None.

RETURN VALUE  None.

CAVEAT        Information can be provided for 1 Subs/Tech Pair, if
              mask has more than 1 bit set the request is rejected.

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_event_info
(
  ds3geventmgr_event_type    event_id,
  void                      *payload,
  ds3geventmgr_filter_type  *filter_info_p
)
{
  sys_modem_as_id_e_type                    subs_id = SYS_MODEM_AS_ID_NONE;
  ds3gsubsmgr_subs_id_e_type                cache_arr_index = 0;
  boolean                                   invoke_cb = FALSE;
  boolean                                   ret_val = FALSE;
  ds3geventmgr_attach_info_type            *attach_info_cache_p = NULL;
  ps_sys_null_resolved_apn_info_type       *null_resolved_apn_cache_p = NULL;
  ds3gevent_mgr_throttle_info_type            *pdn_throttle_info_cache_p = NULL;
  ps_sys_wlan_max_conn_info_type           *wlan_max_conn_info_cache_p = NULL;
  ps_sys_3gpp2_s101_status_change_type     *s101_status_chg_info_p = NULL;
  ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type
                                           *handoff_complete_resp_info_p = NULL;
  ds3geventmgr_imsi_info_type              *imsi_info_cache_p = NULL;
  ds3geventmgr_impi_info_type              *impi_info_cache_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Payload info received for event:%d",
                  event_id);

  if(event_id <= DS3GEVENTMGR_INVALID_EV || event_id >= DS3GEVENTMGR_MAX_EV ||
     filter_info_p == NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Invalid Input Params, Filter Info: 0x%x", filter_info_p);
    return ret_val;
  }

  if((filter_info_p->ps_subs_id < PS_SYS_DEFAULT_SUBS ||
      filter_info_p->ps_subs_id > PS_SYS_TERTIARY_SUBS) ||
     (filter_info_p->tech < PS_SYS_TECH_MIN ||
      filter_info_p->tech > PS_SYS_TECH_ALL))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Subscription ID: %d or Tech Mask: %d, rejected",
                    filter_info_p->ps_subs_id, filter_info_p->tech);
    return ret_val;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm
                     ((ds_sys_subscription_enum_type)filter_info_p->ps_subs_id);

  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE )
  {
    return ret_val;
  }
  cache_arr_index = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if( !ds3gsubsmgr_is_ds3g_subs_index_valid(cache_arr_index) )
  {
    return ret_val;
  }

  /*---------------------------------------------------------------------
   1. Copy Payload info into cback payload struct
   2. Validate Tech & Mark flag to indicate that cback has to be invoked
   3. Allocate memory on heap to which the static tech specific entry will
      point to which stores the cached payload info
   4. If tech type is not valid, no cback invocation and no caching
   5. Once cback is invoked free the event info struct
  ----------------------------------------------------------------------*/
  switch(event_id)
  {
    case DS3GEVENTMGR_LTE_ATTACH_INIT_EV:
      if(filter_info_p->tech == PS_SYS_TECH_3GPP)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info ==
           NULL)
        {
          attach_info_cache_p = (ds3geventmgr_attach_info_type *)modem_mem_alloc(
                       sizeof(ds3geventmgr_attach_info_type), MODEM_MEM_CLIENT_DATA);
          if(attach_info_cache_p != NULL)
          {
            memset(attach_info_cache_p, 0, sizeof(ds3geventmgr_attach_info_type));
            memscpy(attach_info_cache_p, sizeof(ds3geventmgr_attach_info_type),
                    payload, sizeof(ds3geventmgr_attach_info_type));

            ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info
              = attach_info_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching Attach"
                            " Status Info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                    attach_conf_info,
                    payload, sizeof(ds3geventmgr_attach_info_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing attach info in an existing memory "
                            "block in cache");
            memset(ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info,
                   0, sizeof(ds3geventmgr_attach_info_type));
            memscpy(ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info,
                    sizeof(ds3geventmgr_attach_info_type), payload,
                    sizeof(ds3geventmgr_attach_info_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_CONSOLIDATED_RESOLVED_APN_EV:
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_all[cache_arr_index].
           null_resolved_apn_conf_info == NULL)
        {
          null_resolved_apn_cache_p = (ps_sys_null_resolved_apn_info_type *)
             modem_mem_alloc(sizeof(ps_sys_null_resolved_apn_info_type),
                             MODEM_MEM_CLIENT_DATA);
          if(null_resolved_apn_cache_p != NULL)
          {
            memset(null_resolved_apn_cache_p, 0,
                   sizeof(ps_sys_null_resolved_apn_info_type));
            memscpy(null_resolved_apn_cache_p,
                    sizeof(ps_sys_null_resolved_apn_info_type),
                    payload, sizeof(ps_sys_null_resolved_apn_info_type));

            ds3geventmgr_conf_tech_all[cache_arr_index].null_resolved_apn_conf_info
              = null_resolved_apn_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching Attach"
                            " Status Info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_all[cache_arr_index].
                    null_resolved_apn_conf_info,
                    payload, sizeof(ps_sys_null_resolved_apn_info_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing consolidated resolved APN info "
                            "in an existing memory block in cache");
            memset(ds3geventmgr_conf_tech_all[cache_arr_index].
                   null_resolved_apn_conf_info,
                   0, sizeof(ps_sys_null_resolved_apn_info_type));
            memscpy(ds3geventmgr_conf_tech_all[cache_arr_index].
                    null_resolved_apn_conf_info,
                    sizeof(ps_sys_null_resolved_apn_info_type), payload,
                    sizeof(ps_sys_null_resolved_apn_info_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_NULL_RESOLVED_APN_EV:
      invoke_cb = TRUE;

      if((filter_info_p->tech == PS_SYS_TECH_3GPP) &&
         (ds3geventmgr_conf_tech_3gpp[cache_arr_index].
          null_resolved_apn_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  null_resolved_apn_conf_info, payload,
                  sizeof(ps_sys_null_resolved_apn_info_type))!= 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in 3GPP cache");
          memset(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                 null_resolved_apn_conf_info, 0,
                 sizeof(ps_sys_null_resolved_apn_info_type));
          memscpy(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type), payload,
                  sizeof(ps_sys_null_resolved_apn_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else if((filter_info_p->tech == PS_SYS_TECH_3GPP2) &&
              (ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
               null_resolved_apn_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  null_resolved_apn_conf_info, payload,
                  sizeof(ps_sys_null_resolved_apn_info_type))!= 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in 3GPP2 cache");
          memset(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                 null_resolved_apn_conf_info, 0,
                 sizeof(ps_sys_null_resolved_apn_info_type));
          memscpy(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type), payload,
                  sizeof(ps_sys_null_resolved_apn_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else if((filter_info_p->tech == PS_SYS_TECH_WLAN) &&
              (ds3geventmgr_conf_tech_wlan[cache_arr_index].
               null_resolved_apn_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  null_resolved_apn_conf_info, payload,
                  sizeof(ps_sys_null_resolved_apn_info_type))!= 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in WLAN cache");
          memset(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                 null_resolved_apn_conf_info, 0,
                 sizeof(ps_sys_null_resolved_apn_info_type));
          memscpy(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type), payload,
                  sizeof(ps_sys_null_resolved_apn_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else
      {
        null_resolved_apn_cache_p = (ps_sys_null_resolved_apn_info_type *)
               modem_mem_alloc(sizeof(ps_sys_null_resolved_apn_info_type),
                                                     MODEM_MEM_CLIENT_DATA);
        if(null_resolved_apn_cache_p != NULL)
        {
          memset(null_resolved_apn_cache_p, 0,
                 sizeof(ps_sys_null_resolved_apn_info_type));

          memscpy(null_resolved_apn_cache_p,
                  sizeof(ps_sys_null_resolved_apn_info_type),
                  payload, sizeof(ps_sys_null_resolved_apn_info_type));

          if(filter_info_p->tech == PS_SYS_TECH_3GPP)
          {
            ds3geventmgr_conf_tech_3gpp[cache_arr_index].
              null_resolved_apn_conf_info = null_resolved_apn_cache_p;
            ret_val = TRUE;
          }
          else if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
          {
            ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
              null_resolved_apn_conf_info = null_resolved_apn_cache_p;
            ret_val = TRUE;
          }
          else if (filter_info_p->tech == PS_SYS_TECH_WLAN)
          {
            ds3geventmgr_conf_tech_wlan[cache_arr_index].
              null_resolved_apn_conf_info = null_resolved_apn_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                            "Invalid Tech Type: %d passed, Not caching info",
                            filter_info_p->tech);
            invoke_cb = FALSE;
            modem_mem_free(null_resolved_apn_cache_p,MODEM_MEM_CLIENT_DATA);
            null_resolved_apn_cache_p = NULL;
          }
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                          "Heap Memory allocation failed, not caching Resolved"
                          " APN Info");
        }
      }
      break;

    case DS3GEVENTMGR_PDN_THROTTLE_INFO_EV:
      invoke_cb = TRUE;
      if((filter_info_p->tech == PS_SYS_TECH_3GPP) &&
         (ds3geventmgr_conf_tech_3gpp[cache_arr_index].
          pdn_throttle_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  pdn_throttle_conf_info, payload,
                  sizeof(ds3gevent_mgr_throttle_info_type)) != 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in 3GPP cache");
          memset(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                 pdn_throttle_conf_info, 0,
                 sizeof(ds3gevent_mgr_throttle_info_type));
          memscpy(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type), payload,
                  sizeof(ds3gevent_mgr_throttle_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else if((filter_info_p->tech == PS_SYS_TECH_3GPP2) &&
             (ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
              pdn_throttle_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  pdn_throttle_conf_info, payload,
                  sizeof(ds3gevent_mgr_throttle_info_type)) != 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in 3GPP2 cache");
          memset(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                 pdn_throttle_conf_info, 0,
                 sizeof(ds3gevent_mgr_throttle_info_type));
          memscpy(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type), payload,
                  sizeof(ds3gevent_mgr_throttle_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else if((filter_info_p->tech == PS_SYS_TECH_WLAN) &&
             (ds3geventmgr_conf_tech_wlan[cache_arr_index].
              pdn_throttle_conf_info != NULL))
      {
        if(memcmp(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  pdn_throttle_conf_info, payload,
                  sizeof(ds3gevent_mgr_throttle_info_type)) != 0)
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Over-writing payload in an existing memory "
                          "block in WLAN cache");
          memset(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                 pdn_throttle_conf_info, 0,
                 sizeof(ds3gevent_mgr_throttle_info_type));
          memscpy(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type), payload,
                  sizeof(ds3gevent_mgr_throttle_info_type));
        }
        else
        {
          invoke_cb = FALSE;
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Same Info provided, not invoking cback again");
        }
        ret_val = TRUE;
      }
      else
      {
        pdn_throttle_info_cache_p = (ds3gevent_mgr_throttle_info_type *)
           modem_mem_alloc(sizeof(ds3gevent_mgr_throttle_info_type),
                           MODEM_MEM_CLIENT_DATA);

        if(pdn_throttle_info_cache_p != NULL)
        {
          memset(pdn_throttle_info_cache_p, 0,
                 sizeof(ds3gevent_mgr_throttle_info_type));

          memscpy(pdn_throttle_info_cache_p,
                  sizeof(ds3gevent_mgr_throttle_info_type),
                  payload, sizeof(ds3gevent_mgr_throttle_info_type));

          if(filter_info_p->tech == PS_SYS_TECH_3GPP)
          {
            ds3geventmgr_conf_tech_3gpp[cache_arr_index].pdn_throttle_conf_info =
              pdn_throttle_info_cache_p;
            ret_val = TRUE;
          }
          else if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
          {
            ds3geventmgr_conf_tech_3gpp2[cache_arr_index].pdn_throttle_conf_info =
              pdn_throttle_info_cache_p;
            ret_val = TRUE;
          }
          else if(filter_info_p->tech == PS_SYS_TECH_WLAN)
          {
            ds3geventmgr_conf_tech_wlan[cache_arr_index].pdn_throttle_conf_info =
              pdn_throttle_info_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                            "Invalid Tech Type: %d passed, Not caching info",
                            filter_info_p->tech);
            invoke_cb = FALSE;
            modem_mem_free(pdn_throttle_info_cache_p,MODEM_MEM_CLIENT_DATA);
            pdn_throttle_info_cache_p = NULL;
          }
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                          "Heap Memory allocation failed, not caching Resolved"
                          " APN Info");
        }
      }
      break;

    case DS3GEVENTMGR_WLAN_MAX_CONN_EV:
      if(filter_info_p->tech == PS_SYS_TECH_WLAN)
      {
        invoke_cb = TRUE;

        if(ds3geventmgr_conf_tech_wlan[cache_arr_index].wlan_max_conn_conf_info
           == NULL)
        {
          wlan_max_conn_info_cache_p = (ps_sys_wlan_max_conn_info_type *)
                 modem_mem_alloc(sizeof(ps_sys_wlan_max_conn_info_type),
                                 MODEM_MEM_CLIENT_DATA);
          if(wlan_max_conn_info_cache_p != NULL)
          {
            memset(wlan_max_conn_info_cache_p, 0,
                    sizeof(ps_sys_wlan_max_conn_info_type));
            memscpy(wlan_max_conn_info_cache_p,
                    sizeof(ps_sys_wlan_max_conn_info_type),
                    payload, sizeof(ps_sys_wlan_max_conn_info_type));

            ds3geventmgr_conf_tech_wlan[cache_arr_index].wlan_max_conn_conf_info =
              wlan_max_conn_info_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching WLAN"
                            " Max Conn Info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                    wlan_max_conn_conf_info, payload,
                    sizeof(ps_sys_wlan_max_conn_info_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing WLAN Max Conn info in an existing memory "
                            "block in cache");
            memset(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                   wlan_max_conn_conf_info, 0,
                   sizeof(ps_sys_wlan_max_conn_info_type));

            memscpy(ds3geventmgr_conf_tech_wlan[cache_arr_index].
                   wlan_max_conn_conf_info,
                   sizeof(ps_sys_wlan_max_conn_info_type), payload,
                   sizeof(ps_sys_wlan_max_conn_info_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_S101_STATUS_CHG_EV:
      if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].s101_status_chg_info
           == NULL)
        {
          s101_status_chg_info_p = (ps_sys_3gpp2_s101_status_change_type *)
                 modem_mem_alloc(sizeof(ps_sys_3gpp2_s101_status_change_type),
                                                    MODEM_MEM_CLIENT_DATA);
          if(s101_status_chg_info_p != NULL)
          {
            memset(s101_status_chg_info_p, 0,
                   sizeof(ps_sys_3gpp2_s101_status_change_type));
            memscpy(s101_status_chg_info_p,
                    sizeof(ps_sys_3gpp2_s101_status_change_type ),
                    payload, sizeof(ps_sys_3gpp2_s101_status_change_type));

            ds3geventmgr_conf_tech_3gpp2[cache_arr_index].s101_status_chg_info
              = s101_status_chg_info_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching S101"
                            " Status change Info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                    s101_status_chg_info,
                    payload, sizeof(ps_sys_3gpp2_s101_status_change_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing attach info in an existing memory "
                            "block in cache");
            memset(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                   s101_status_chg_info,
                   0, sizeof(ps_sys_3gpp2_s101_status_change_type));
            memscpy(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                    s101_status_chg_info,
                    sizeof(ps_sys_3gpp2_s101_status_change_type), payload,
                    sizeof(ps_sys_3gpp2_s101_status_change_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV:
      if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                                      handoff_complete_resp_info == NULL)
        {
          handoff_complete_resp_info_p =
            (ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type *)
              modem_mem_alloc(
              sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type),
                         MODEM_MEM_CLIENT_DATA);

          if(handoff_complete_resp_info_p != NULL)
          {
            memset(handoff_complete_resp_info_p, 0,
             sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));

            memscpy(handoff_complete_resp_info_p,
             sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type),
                    payload,
             sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));

            ds3geventmgr_conf_tech_3gpp2[cache_arr_index].handoff_complete_resp_info
              = handoff_complete_resp_info_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching Handoff"
                            " complete resp info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
               handoff_complete_resp_info,
               payload,
               sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type))
               != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing attach info in an existing memory "
                            "block in cache");
            memset(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
             handoff_complete_resp_info,0,
             sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));

            memscpy(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
              handoff_complete_resp_info,
              sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type),
              payload,
              sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_MMGSDI_IMSI_INFO_EV:
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_all[cache_arr_index].
           imsi_conf_info == NULL)
        {
          imsi_info_cache_p = (ds3geventmgr_imsi_info_type *)
            modem_mem_alloc(sizeof(ds3geventmgr_imsi_info_type),
                            MODEM_MEM_CLIENT_DATA);
          if(imsi_info_cache_p != NULL)
          {
            memset(imsi_info_cache_p, 0,
                   sizeof(ds3geventmgr_imsi_info_type));
            memscpy(imsi_info_cache_p,
                    sizeof(ds3geventmgr_imsi_info_type),
                    payload, sizeof(ds3geventmgr_imsi_info_type));

            ds3geventmgr_conf_tech_all[cache_arr_index].imsi_conf_info
              = imsi_info_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching IMSI info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_all[cache_arr_index].
                    imsi_conf_info,
                    payload, sizeof(ds3geventmgr_imsi_info_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing IMSI info "
                            "in an existing memory block in cache");
            memset(ds3geventmgr_conf_tech_all[cache_arr_index].
                   imsi_conf_info,
                   0, sizeof(ds3geventmgr_imsi_info_type));
            memscpy(ds3geventmgr_conf_tech_all[cache_arr_index].
                    imsi_conf_info,
                    sizeof(ds3geventmgr_imsi_info_type), payload,
                    sizeof(ds3geventmgr_imsi_info_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_MMGSDI_IMPI_INFO_EV:
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        invoke_cb = TRUE;
        if(ds3geventmgr_conf_tech_all[cache_arr_index].impi_conf_info == NULL)
        {
          impi_info_cache_p = (ds3geventmgr_impi_info_type *)
            modem_mem_alloc(sizeof(ds3geventmgr_impi_info_type),
                            MODEM_MEM_CLIENT_DATA);
          if(impi_info_cache_p != NULL)
          {
            memset(impi_info_cache_p, 0,
                   sizeof(ds3geventmgr_impi_info_type));
            memscpy(impi_info_cache_p,
                    sizeof(ds3geventmgr_impi_info_type),
                    payload, sizeof(ds3geventmgr_impi_info_type));

            ds3geventmgr_conf_tech_all[cache_arr_index].impi_conf_info
              = impi_info_cache_p;
            ret_val = TRUE;
          }
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                            "Heap Memory allocation failed, not caching IMPI info");
          }
        }
        else
        {
          if(memcmp(ds3geventmgr_conf_tech_all[cache_arr_index].
                    impi_conf_info,
                    payload, sizeof(ds3geventmgr_impi_info_type)) != 0)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Over-writing IMPI info "
                            "in an existing memory block in cache");
            memset(ds3geventmgr_conf_tech_all[cache_arr_index].
                   impi_conf_info,
                   0, sizeof(ds3geventmgr_impi_info_type));
            memscpy(ds3geventmgr_conf_tech_all[cache_arr_index].
                    impi_conf_info,
                    sizeof(ds3geventmgr_impi_info_type), payload,
                    sizeof(ds3geventmgr_impi_info_type));
          }
          else
          {
            invoke_cb = FALSE;
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "Same Info provided, not invoking cback again");
          }
          ret_val = TRUE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No invoking cback & caching"
                        " Payload",filter_info_p->tech);
      }
      break;

    case DS3GEVENTMGR_MODE_LPM_EV:                  
    case DS3GEVENTMGR_NV_REFRESH_EV:
    case DS3GEVENTMGR_LATENCY_INFO_EV:
#ifdef FEATURE_MMGSDI
    case DS3GEVENTMGR_MMGSDI_SUBSCRIPTION_READY_EV:
    case DS3GEVENTMGR_MMGSDI_REFRESH_VOTING_EV:
    case DS3GEVENTMGR_MMGSDI_OK_REFRESH_EV:
    case DS3GEVENTMGR_MMGSDI_REFRESH_FCN_EV:
    case DS3GEVENTMGR_MMGSDI_CARD_REFRESH_COMPLETE_EV:
    case DS3GEVENTMGR_MMGSDI_SESSION_CHANGED_EV:
    case DS3GEVENTMGR_MMGSDI_CARD_ERROR_EV:
#endif /* FEATURE_MMGSDI */
    case DS3GEVENTMGR_THERMAL_MITIGATION_EV:
    case DS3GEVENTMGR_APN_CHANGED_EV:
    case DS3GEVENTMGR_DATA_ROAMING_CHANGED_EV:
    case DS3GEVENTMGR_DATA_CHANGED_EV:
    case DS3GEVENTMGR_PLMN_LIST_CHG_EV:
    case DS3GEVENTMGR_PROFILE_APN_NAME_CHG_EV:
    case DS3GEVENTMGR_PDN_STATUS_INFO_EV:
    case DS3GEVENTMGR_WLAN_PROXY_INFO_CHG_EV:
    case DS3GEVENTMGR_SUB_NOT_AVAIL_EV:
    case DS3GEVENTMGR_SUB_AVAIL_EV:
    case DS3GEVENTMGR_WLAN_IDI_TYPE_EV:
    case DS3GEVENTMGR_ROAMING_STATUS_EV:
      invoke_cb = TRUE;
      ret_val = TRUE;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Un-Handled Event received: %d",
                      event_id);
      break;
  }

  if(invoke_cb == TRUE)
  {
    ds3geventmgr_event_notify(event_id, payload, filter_info_p);
  }

  return ret_val;
}/*ds3geventmgr_set_event_info*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_GET_CONF_INFO

DESCRIPTION   Called by clients to get cached information

DEPENDENCIES  None.

RETURN VALUE  TRUE: If operation was successful & payload was provided
              FALSE: Otherwise

CAVEAT        Information can be provided for 1 Subs/Tech Pair, if
              mask has more than 1 bit set the request is rejected.

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_get_conf_info
(
  ds3geventmgr_event_type                 event_id,
  void                                   *payload,
  ds3geventmgr_filter_type               *filter_info_p
)
{
  boolean                     ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  cache_arr_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Payload Query received for Event: %d",
                  event_id);

  if(event_id <= DS3GEVENTMGR_INVALID_EV || 
     event_id > DS3GEVENTMGR_MAX_EV || payload == NULL || 
     filter_info_p == NULL)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Input Params, Filter Info: 0x%x & Payload: 0x%x",
                     filter_info_p, payload);
    return ret_val;
  }

  if((filter_info_p->ps_subs_id < PS_SYS_DEFAULT_SUBS ||
      filter_info_p->ps_subs_id > PS_SYS_TERTIARY_SUBS) ||
     (filter_info_p->tech < PS_SYS_TECH_MIN ||
      filter_info_p->tech > PS_SYS_TECH_ALL))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid Subscription ID: %d or Tech Mask: %d, rejected",
                    filter_info_p->ps_subs_id, filter_info_p->tech);
    return ret_val;
  }

  cache_arr_index = ds3gsubsmgr_subs_id_ds_to_ds3g(
                      (ds_sys_subscription_enum_type)filter_info_p->ps_subs_id);

  if( ds3gsubsmgr_is_ds3g_subs_index_valid(cache_arr_index) == FALSE )
  {
    return ret_val;
  }

  switch( event_id )
  {
    case DS3GEVENTMGR_LTE_ATTACH_INIT_EV:

      memset(payload, 0, sizeof(ds3geventmgr_attach_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_3GPP)
      {
        if(ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info !=
           NULL)
        {
          memscpy(payload, sizeof(ds3geventmgr_attach_info_type),
                  ds3geventmgr_conf_tech_3gpp[cache_arr_index].attach_conf_info,
                  sizeof(ds3geventmgr_attach_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No Attach Info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_CONSOLIDATED_RESOLVED_APN_EV:

      memset(payload, 0, sizeof(ps_sys_null_resolved_apn_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        if(ds3geventmgr_conf_tech_all[cache_arr_index].
                                  null_resolved_apn_conf_info != NULL)
        {
          memscpy(payload, sizeof(ps_sys_null_resolved_apn_info_type),
                  ds3geventmgr_conf_tech_all[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No Attach Info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_PDN_THROTTLE_INFO_EV:

      memset(payload, 0, sizeof(ds3gevent_mgr_throttle_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_WLAN)
      {
        if(ds3geventmgr_conf_tech_wlan[cache_arr_index].pdn_throttle_conf_info
           != NULL)
        {
          memscpy(payload, sizeof(ds3gevent_mgr_throttle_info_type),
                  ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No PDN Throttle Info Present for WLAN");
          ret_val = FALSE;
        }
      }
      else if(filter_info_p->tech == PS_SYS_TECH_3GPP)
      {
        if(ds3geventmgr_conf_tech_3gpp[cache_arr_index].pdn_throttle_conf_info
           != NULL)
        {
          memscpy(payload, sizeof(ds3gevent_mgr_throttle_info_type),
                  ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No PDN Throttle Info Present for 3GPP");
          ret_val = FALSE;
        }
      }
      else if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].pdn_throttle_conf_info
           != NULL)
        {
          memscpy(payload, sizeof(ds3gevent_mgr_throttle_info_type),
                  ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  pdn_throttle_conf_info,
                  sizeof(ds3gevent_mgr_throttle_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No PDN Throttle Info Present for 3GPP2");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_NULL_RESOLVED_APN_EV:

      memset(payload, 0, sizeof(ps_sys_null_resolved_apn_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_WLAN)
      {
        if(ds3geventmgr_conf_tech_wlan[cache_arr_index].
           null_resolved_apn_conf_info != NULL)
        {
          memscpy(payload, sizeof(ps_sys_null_resolved_apn_info_type),
                  ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No NULL Resolved APN Info Present for WLAN");
          ret_val = FALSE;
        }
      }
      else if(filter_info_p->tech == PS_SYS_TECH_3GPP)
      {
        if(ds3geventmgr_conf_tech_3gpp[cache_arr_index].
           null_resolved_apn_conf_info != NULL)
        {
          memscpy(payload, sizeof(ps_sys_null_resolved_apn_info_type),
                  ds3geventmgr_conf_tech_3gpp[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No NULL Resolved APN Info Present for 3GPP");
          ret_val = FALSE;
        }

      }
      else if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
           null_resolved_apn_conf_info != NULL)
        {
          memscpy(payload, sizeof(ps_sys_null_resolved_apn_info_type),
                  ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  null_resolved_apn_conf_info,
                  sizeof(ps_sys_null_resolved_apn_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No NULL Resolved APN Info Present for 3GPP2");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_WLAN_MAX_CONN_EV:

      memset(payload, 0, sizeof(ps_sys_wlan_max_conn_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_WLAN)
      {
        if(ds3geventmgr_conf_tech_wlan[cache_arr_index].wlan_max_conn_conf_info
           != NULL)
        {
          memscpy(payload, sizeof(ps_sys_wlan_max_conn_info_type),
                  ds3geventmgr_conf_tech_wlan[cache_arr_index].
                  wlan_max_conn_conf_info,
                  sizeof(ps_sys_wlan_max_conn_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "No WLAN Max Conn Info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_S101_STATUS_CHG_EV:

      memset(payload, 0, sizeof(ps_sys_3gpp2_s101_status_change_type));
      if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].s101_status_chg_info !=
           NULL)
        {
          memscpy(payload, sizeof(ps_sys_3gpp2_s101_status_change_type),
                  ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  s101_status_chg_info,
                  sizeof(ps_sys_3gpp2_s101_status_change_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No S101 Status Chg info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV:

      memset(payload, 0,
             sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));
      if(filter_info_p->tech == PS_SYS_TECH_3GPP2)
      {
        if(ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
           handoff_complete_resp_info != NULL)
        {
          memscpy(payload,
            sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type),
                  ds3geventmgr_conf_tech_3gpp2[cache_arr_index].
                  handoff_complete_resp_info,
            sizeof(ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No S101 Status Chg info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_MMGSDI_IMSI_INFO_EV:
      memset(payload, 0, sizeof(ds3geventmgr_imsi_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        if(ds3geventmgr_conf_tech_all[cache_arr_index].imsi_conf_info != NULL)
        {
          memscpy(payload, sizeof(ds3geventmgr_imsi_info_type),
                  ds3geventmgr_conf_tech_all[cache_arr_index].
                  imsi_conf_info,
                  sizeof(ds3geventmgr_imsi_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No IMSI Info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    case DS3GEVENTMGR_MMGSDI_IMPI_INFO_EV:
      memset(payload, 0, sizeof(ds3geventmgr_impi_info_type));
      if(filter_info_p->tech == PS_SYS_TECH_ALL)
      {
        if(ds3geventmgr_conf_tech_all[cache_arr_index].impi_conf_info != NULL)
        {
          memscpy(payload, sizeof(ds3geventmgr_impi_info_type),
                  ds3geventmgr_conf_tech_all[cache_arr_index].
                  impi_conf_info,
                  sizeof(ds3geventmgr_impi_info_type));
          ret_val = TRUE;
        }
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "No IMPI Info Present");
          ret_val = FALSE;
        }
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Invalid Tech Type: %d, No Payload provided",
                        filter_info_p->tech);
        ret_val = FALSE;
      }
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Un-Handled Event received: %d",
                      event_id);
      ret_val = FALSE;
      break;
  }

  return ret_val;
}/*ds3geventmgr_get_conf_info*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_LPM_PWR_OFF_STATUS

DESCRIPTION   Sets master control bit for LPM processing to indicate
              DS processing of LPM/Power off mode has been initiated

DEPENDENCIES  None

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - if the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_lpm_pwr_off_status
(
  ds3geventmgr_lpm_pwr_off_bit_msk_type status
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Validate the incoming status type
  -------------------------------------------------------------------------*/
  if((status != DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK) &&
     (status != DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK)  &&
     (status != DS3GEVENTMGR_LPM_PWROFF_WLAN_CTRL_BIT_MSK)  &&
     (status != DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Invalid power off status ctrl type %d ",
                    status);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Set the master ctrl bit, if not set already
  -------------------------------------------------------------------------*/
  if (!(ds3geventmgr_lpm_poweroff_status &
        DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK))
  {
    ds3geventmgr_lpm_poweroff_status |=
      DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK;
  }

  return TRUE;
}/*ds3geventmgr_set_lpm_pwr_off_status*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CLR_AND_NOTIFY_LPM_PWR_OFF_STATUS

DESCRIPTION   Sets the Incoming LPM/Power off Bit mask. If all the
              DS internal modules has finished processing successfully,
              this function notifies CM that the processing is complete

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_clr_and_notify_lpm_pwr_off_status
(
  ds3geventmgr_lpm_pwr_off_bit_msk_type bit_msk
)
{
  uint32   is_wlan_reg = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((bit_msk != DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK) &&
     (bit_msk != DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK)  &&
     (bit_msk != DS3GEVENTMGR_LPM_PWROFF_WLAN_CTRL_BIT_MSK)  &&
     (bit_msk != DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid power off status ctrl type %d ",
                    bit_msk);
    return;
  }

  /*-------------------------------------------------------------------------
    Ignore if the master control bit is not set.
  -------------------------------------------------------------------------*/
  if(!(ds3geventmgr_lpm_poweroff_status &
       DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Ignore as this not an LPM PWR off event processing");
    return;
  }
  /*-------------------------------------------------------------------------
    Set the incoming status bit
  -------------------------------------------------------------------------*/
  ds3geventmgr_lpm_poweroff_status |= bit_msk;

  is_wlan_reg = ds3geventmgr_check_if_wlan_client_is_reg(DS3GEVENTMGR_MODE_LPM_EV);
  /*-------------------------------------------------------------------------
    Check if all the internal modules have completed processing LPM mode
    in which case, we need to notify CM
  -------------------------------------------------------------------------*/
  if(ds3geventmgr_lpm_poweroff_status ==
     (DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK |
      DS3GEVENTMGR_LPM_PWROFF_3GPP2_CTRL_BIT_MSK |
      DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK  |
      (is_wlan_reg * DS3GEVENTMGR_LPM_PWROFF_WLAN_CTRL_BIT_MSK))
    )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Clearing LPM bit masks and notifying CM");
    /*-----------------------------------------------------------------------
      All modules processed LPM mode. Notify CM
    -----------------------------------------------------------------------*/
    ds3g_cm_data_packet_state_notify(0xFF);

    /*-----------------------------------------------------------------------
      Clear the Master control bit as we are done with the processing
    -----------------------------------------------------------------------*/
    ds3geventmgr_lpm_poweroff_status = 0;
  }
  return;
}/* ds3geventmgr_clr_and_notify_lpm_pwr_off_status */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_IS_LPM_PWR_OFF_MODE_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of LPM/Power off mode is
              still in progress (or) complete

DEPENDENCIES  None

RETURN VALUE  TRUE  --> LPM/PWR off processing still in progress
              FALSE --> LPM/PWR Off processing complete/No LPM/PWR off
                        processing in progress

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress( void )
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((ds3geventmgr_lpm_poweroff_status &
      DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK)
     == DS3GEVENTMGR_LPM_PWFOFF_MASTER_CTRL_BIT_MSK)
  {
    ret_val = TRUE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "Is LPM pwr off mode in progress? %d "
                  "ds3geventmgr_lpm_poweroff_status bits active %d",
                  ret_val, ds3geventmgr_lpm_poweroff_status);

  return ret_val;
}/*ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress*/

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_SUB_NOT_AVAIL_STATUS

DESCRIPTION   Sets the master control bit for SUB_NOT_AVAIL processing
              to indicate DS processing of SUB_NOT_AVAIL has been initiated

DEPENDENCIES  None

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - if the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_sub_not_avail_status
(
  ds3geventmgr_sub_not_avail_bit_msk_type  client_id_mask,
  sys_modem_as_id_e_type                  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Validate the incoming status type
  -------------------------------------------------------------------------*/
  if((client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK) &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_MPPM_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_WLAN_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid card refresh status ctrl type %d on subs %d",
                    client_id_mask, subs_id);
    return FALSE;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*-------------------------------------------------------------------------
    Set the master ctrl bit, if not set already
  -------------------------------------------------------------------------*/
  if (!(ds3geventmgr_sub_not_avail_status[subs_indx] &
        DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK))
  {
    ds3geventmgr_sub_not_avail_status[subs_indx] |=
      DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK;
  }

  return TRUE;
} /* ds3geventmgr_set_sub_not_avail_status() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CLR_AND_NOTIFY_SUB_NOT_AVAIL_STATUS

DESCRIPTION   Sets the Incoming SUB_NOT_AVAIL Bit mask. If all the
              DS internal modules has finished processing successfully,
              this function notifies CM that the processing is complete

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3geventmgr_clr_and_notify_sub_not_avail_status
(
  ds3geventmgr_sub_not_avail_bit_msk_type  client_id_mask,
  sys_modem_as_id_e_type                 subs_id,
  uint8                                  subs_id_mask
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
  ds3gsubsmgr_subs_id_e_type i         = DS3GSUBSMGR_SUBS_ID_INVALID;  
  boolean notify_cm                    = TRUE;                    
  ds3geventmgr_client_reg_info  client_event_info;
  ds3g_event_data_block        *event_block_to_insert = NULL;
  uint32                        wlan_client_reg = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK) &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_MPPM_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_WLAN_CTRL_BIT_MSK)  &&
     (client_id_mask != DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Invalid card refresh status ctrl type %d ", client_id_mask);
    return;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*-------------------------------------------------------------------------
    Ignore if the master control bit is not set.
  -------------------------------------------------------------------------*/
  if(!(ds3geventmgr_sub_not_avail_status[subs_indx] &
       DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Ignore as this not an card refresh event processing");
    return;
  }

  /*-------------------------------------------------------------------------
    Set the incoming status bit
  -------------------------------------------------------------------------*/
  ds3geventmgr_sub_not_avail_status[subs_indx] |= client_id_mask;

  /*-------------------------------------------------------------------------
    Check if IWLAN client has registered for CARD_REFRESH events
  -------------------------------------------------------------------------*/
  memset(&client_event_info, 0, sizeof(ds3geventmgr_client_reg_info));
  client_event_info.client_id = DS3GEVENTMGR_CLIENT_ID_WLAN;
  client_event_info.event_id  = DS3GEVENTMGR_SUB_NOT_AVAIL_EV;

  event_block_to_insert = (ds3g_event_data_block *)q_linear_search(
                                                 &ds3geventmgr_q,
                                                 ds3geventmgr_compare_function,
                                                 (void *)&client_event_info);
  if(event_block_to_insert != NULL)
  {
    wlan_client_reg = 1;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
		  "Client mask: 0x%x Overall Mask is 0x%x wlan_client_reg: %d", 
		  client_id_mask, ds3geventmgr_sub_not_avail_status[subs_indx],
		  wlan_client_reg);

  /*-------------------------------------------------------------------------
    Check if all the internal modules have completed processing LPM mode
    in which case, we need to notify CM
  -------------------------------------------------------------------------*/
  if(ds3geventmgr_sub_not_avail_status[subs_indx] == 
     (DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK |
      DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP2_CTRL_BIT_MSK |
      DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK   |
      DS3GEVENTMGR_SUB_NOT_AVAIL_MPPM_CTRL_BIT_MSK   |
      (wlan_client_reg * DS3GEVENTMGR_SUB_NOT_AVAIL_WLAN_CTRL_BIT_MSK) )
    )
  {
    /*-----------------------------------------------------------------------
      Clear the Master control bit as we are done with the processing
      -----------------------------------------------------------------------*/
    ds3geventmgr_sub_not_avail_status[subs_indx] = 0;

    /*-----------------------------------------------------------------------
      This logic is to make sure DS3G holds on notifying CM until all subs
      are complete. If refresh was only notified on one sub, then notify
      immediately. If refresh was notified on multiple subs, then wait for
      all to finish before notifying.
      -----------------------------------------------------------------------*/
    for(i = DS3GSUBSMGR_SUBS_ID_MIN; i < DS3GSUBSMGR_SUBS_ID_COUNT; i++)
    {
      if(ds3geventmgr_is_sub_not_avail_processing_in_progress(
           ds3gsubsmgr_subs_id_ds3g_to_cm(i)) &&
         (subs_id_mask != SYS_MODEM_AS_ID_MASK_1) &&
         (subs_id_mask != SYS_MODEM_AS_ID_MASK_2) &&
         (subs_id_mask != SYS_MODEM_AS_ID_MASK_3)
         )
      {
        notify_cm = FALSE;
        break;
      }
    }

    if(notify_cm == TRUE)
    {
     /*-----------------------------------------------------------------------
       All modules processed Card Refresh. Notify CM
     -----------------------------------------------------------------------*/
      ds3g_cm_data_packet_state_notify(subs_id_mask);
    }
  }
  return;
} /* ds3geventmgr_clr_and_notify_sub_not_avail_status() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_IS_SUB_NOT_AVAIL_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of SUBS_NOT_AVAIL is 
              still in progress (or) complete

DEPENDENCIES  None

RETURN VALUE  TRUE  --> SUB_NOT_AVAIL processing still in progress 
              FALSE --> SUB_NOT_AVAIL processing complete/No CARD REFRESH
                        processing in progress

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_sub_not_avail_processing_in_progress
(
  sys_modem_as_id_e_type  subs_id
)
{
  boolean                    ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if((ds3geventmgr_sub_not_avail_status[subs_indx] &
      DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK)
     == DS3GEVENTMGR_SUB_NOT_AVAIL_MASTER_CTRL_BIT_MSK)
  {
    ret_val = TRUE;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "Is sub not available on subs %d in progress? %d. "
                  "ds3geventmgr_sub_not_avail_status bits active %d",
                  subs_id,ret_val,ds3geventmgr_sub_not_avail_status[subs_indx]);
  return ret_val ;
} /* ds3geventmgr_is_sub_not_avail_processing_in_progress() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_SET_AND_NOTIFY_NV_REFRESH_STATUS

DESCRIPTION   Sets the Incoming bit mask, if found valid. If all required
              bits are set, then clear all bits and notify MCFG that NV
              refresh is complete.

DEPENDENCIES  None

RETURN VALUE  TRUE  - If the status bit has been set successfully
              FALSE - If the status bit cannot be set

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_set_and_notify_nv_refresh_status
(
  ds3geventmgr_client_type status
)
{
  if ((status != DS3GEVENTMGR_CLIENT_ID_3GPP2) &&
      (status != DS3GEVENTMGR_CLIENT_ID_3GPP) &&
      (status != DS3GEVENTMGR_CLIENT_ID_MPPM) &&
      (status != DS3GEVENTMGR_CLIENT_ID_WLAN) &&
      (status != DS3GEVENTMGR_CLIENT_ID_ATCOP) &&
      (status != DS3GEVENTMGR_CLIENT_ID_PROFILE))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Invalid ds3geventmgr nv refresh status mask %d ",
                    status);
    return FALSE;
  }

  ds3geventmgr_nv_refresh_status |= status;
  DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                    "ds3geventmgr nv refresh status mask %d ",
                    status);

  /* notify mcfg if set bits are equal to registered clients */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if (ds3geventmgr_nv_refresh_status
       == ds3geventmgr_nv_refresh_client_reg_status)
  {
    ds3geventmgr_nv_refresh_status = 0;

    if(mcfg_refresh_done(ds3geventmgr_mcfg_get_reference_id()) == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "Failed to notify MCFG of NV refresh done");
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "notify MCFG of NV refresh done");
    }
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  return TRUE;
}/* ds3geventmgr_set_and_notify_nv_refresh_status() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_IS_NV_REFRESH_PROCESSING_IN_PROGRESS

DESCRIPTION   returns whether DS processing of NV refresh is
              still in progress (or) complete

DEPENDENCIES  None

RETURN VALUE  TRUE  - NV refresh is still in progress
              FALSE - NV refresh is complete

SIDE EFFECTS  None
===========================================================================*/
boolean ds3geventmgr_is_nv_refresh_processing_in_progress( void )
{
    boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds3geventmgr_nv_refresh_status != 0)
  {
    ret_val = TRUE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                  "Is NV refresh in progress? %d "
                  "ds3geventmgr_nv_refresh_status bits active %d",
                  ret_val, ds3geventmgr_nv_refresh_status);

  return ret_val;
} /* ds3geventmgr_is_nv_refresh_processing_in_progress() */

/*===========================================================================
FUNCTION      DS3GEVENTMGR_CHECK_IF_WLAN_CLIENT_IS_REG

DESCRIPTION   returns whether WLAN client is registered or not for specific event
 
DEPENDENCIES  None 

RETURN VALUE  TRUE  - If registered
              FALSE - If Not

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3geventmgr_check_if_wlan_client_is_reg 
(
  ds3geventmgr_event_type        event_id
) 
{
  ds3geventmgr_client_reg_info  client_event_info;
  ds3g_event_data_block        *event_block_to_insert = NULL;
  uint32                        wlan_client_reg = 0;
  /* -------------------------------------------------- */

   /*-------------------------------------------------------------------------
    Check if IWLAN client has registered for CARD_REFRESH events
  -------------------------------------------------------------------------*/
  memset(&client_event_info, 0, sizeof(ds3geventmgr_client_reg_info));
  client_event_info.client_id = DS3GEVENTMGR_CLIENT_ID_WLAN;
  client_event_info.event_id  = event_id;

  
  event_block_to_insert = (ds3g_event_data_block *)q_linear_search(
                                                 &ds3geventmgr_q,
                                                 ds3geventmgr_compare_function,
                                                 (void *)&client_event_info);
  if(event_block_to_insert != NULL)
  {
    wlan_client_reg = 1;
  }

  return wlan_client_reg;
}/* ds3geventmgr_check_if_wlan_client_is_reg  */
