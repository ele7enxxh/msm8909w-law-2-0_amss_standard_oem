/*==============================================================================

                M O D E M   C O N F I G   R E F R E S H

GENERAL DESCRIPTION
 
Copyright (c) 2012-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/src/mcfg_refresh.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 cys   Created module

==============================================================================*/

#include "comdef.h"
#include "list.h"

#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

#include "mcfg_common.h"
#include "mcfg_int.h"
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_task.h"
#include "mcfg_refresh.h"
#include "mcfg_refresh_i.h"
#include "mcfg_proc.h"
#include "mcfg_uim.h"
#include "mcfg_sel.h"
#include "mcfg_utils.h"
#include "persistent_device_configuration_v01.h"
#include "qmi_pdc_svc.h"
#include "timer.h"

#include "cm.h"
#include "cm_v.h"

#ifdef FEATURE_MCFG_REFRESH

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define MCFG_REFRESH_NUM_OF_SLOTS (MCFG_REFRESH_INDEX_MAX+1)
#define MCFG_REFRESH_NUM_OF_SUBS  (MCFG_REFRESH_INDEX_MAX+1)

/*==============================================================================
  Typedefs
==============================================================================*/
typedef enum mcfg_refresh_cmd_e {
  MCFG_REFRESH_CMD_REGISTER = 0,
  MCFG_REFRESH_CMD_DONE,
  MCFG_REFRESH_CMD_UIMDATA,
  MCFG_REFRESH_CMD_MMCP_DEACT,
  MCFG_REFRESH_CMD_MMCP_TEAR_DOWN_ACK,
  MCFG_REFRESH_CMD_CM_CALL_END,
  MCFG_REFRESH_CMD_TIMEOUT,
  MCFG_REFRESH_CMD_CM_PH_STATE,
} mcfg_refresh_cmd_e_type;


typedef struct
{
  mcfg_refresh_index_type     sub_index;
  cm_ph_state_e_type          ph_state;
} mcfg_refresh_cm_ph_state_s_type;

typedef struct
{
  mcfg_refresh_index_type     sub_index;
  cm_ph_cmd_err_e_type        ph_cmd_err;
} mcfg_refresh_mmcp_cb_s_type;

typedef struct mcfg_refresh_cmd_data_s {
  mcfg_refresh_cmd_e_type cmd_id;

  union {
    mcfg_refresh_registration_s_type register_req;

    mcfg_refresh_info_s_type     info;
    mcfg_refresh_uimdata_s_type  uimdata;
    mcfg_refresh_index_mask_type submask;
    mcfg_refresh_mmcp_cb_s_type  mmcpdata;
    mcfg_refresh_index_type      slot_index;
    mcfg_refresh_cm_ph_state_s_type cmphstate;
  } data;

  mcfg_refresh_status_e_type status;
} mcfg_refresh_cmd_data_s_type;

typedef struct 
{
  mcfg_refresh_registration_s_type  reg;
  uint32                            reference_id[MCFG_REFRESH_NUM_OF_SLOTS];
} mcfg_refresh_client_s_type;

typedef list_type mcfg_refresh_client_list_type;

typedef struct
{
  list_link_type                link;
  mcfg_refresh_client_s_type    client;
}mcfg_refresh_client_node_type;

typedef struct
{
  mcfg_refresh_state_e_type      state;
  uint8                          cb_count;
  mcfg_refresh_order_e_type      current_order;
  mcfg_refresh_orig_e_type       orig;
  mcfg_config_id_s_type          config_id;
  uint8                          timer;
  uint32                         mmgsdi_reference;
  boolean                        aborting;
  mcfg_refresh_uimdata_s_type    *b2b;
}mcfg_refresh_cache_slotdata_s_type;

typedef struct
{
  mcfg_refresh_client_list_type       client_list;
  mcfg_refresh_cache_slotdata_s_type  slotdata[MCFG_REFRESH_NUM_OF_SLOTS];
  cm_client_id_type                   mcfg_cm_client_id;
}mcfg_refresh_cache_s_type;

/*==============================================================================
  Variables
==============================================================================*/
static mcfg_refresh_cache_s_type mcfg_refresh_cache;

static timer_type       mcfg_refresh_timer[MCFG_NUM_OF_UIM_SLOT];  
static timer_group_type mcfg_refresh_timer_group;
#define MCFG_REFRESH_TIMER_TIMEOUT (5)           //in seconds

/*==============================================================================

                 PRIVATE FUNCTION FORWARD DECLARATIONS

==============================================================================*/
mcfg_error_e_type mcfg_refresh_client_order_complete
(
  mcfg_sub_id_type_e_type sub, 
  mcfg_refresh_order_e_type order
);

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

  FUNCTION  mcfg_refresh_sub_to_slot_index

  DESCRIPTION
    This function is used to mapping sub index to slot index

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_refresh_index_type mcfg_refresh_sub_to_slot_index
(
  mcfg_refresh_index_type sub_index
)
{
  return (mcfg_refresh_index_type)mcfg_uim_map_sub_id_to_slot_index(
    (mcfg_sub_id_type_e_type)sub_index);
}

/*===========================================================================

  FUNCTION  mcfg_refresh_slot_to_sub_index

  DESCRIPTION
    This function is used to mapping slot index to sub index

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_refresh_index_type mcfg_refresh_slot_to_sub_index
(
  mcfg_refresh_index_type slot_index
)
{
  return (mcfg_refresh_index_type)mcfg_uim_map_slot_index_to_sub_id(
    (mcfg_slot_index_type_e_type)slot_index);
}

/*===========================================================================

  FUNCTION  mcfg_refresh_slot_index_to_mmgsdi_slot

  DESCRIPTION
    This function is used to mapping slot index to mmgsdi slot

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static mmgsdi_slot_id_enum_type mcfg_refresh_slot_index_to_mmgsdi_slot
(
  mcfg_refresh_index_type slot_index
)
{
  MCFG_ASSERT_COND_RET_ERROR(slot_index <= MCFG_REFRESH_INDEX_MAX, 
                             MMGSDI_MAX_SLOT_ID_ENUM);
  
  return (mmgsdi_slot_id_enum_type)(slot_index+1);
}

/*===========================================================================

  FUNCTION  mcfg_refresh_sub_index_to_mmgsdi_context

  DESCRIPTION
    This function is used to mapping sub index to mmgsdi context

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static mmgsdi_nv_context_type mcfg_refresh_sub_index_to_mmgsdi_context
(
  mcfg_refresh_index_type sub_index
)
{
  mmgsdi_nv_context_type context;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (sub_index)
  {
    case MCFG_REFRESH_INDEX_0:
      context = MMGSDI_NV_CONTEXT_PRIMARY;
       break;

    case MCFG_REFRESH_INDEX_1:
      context = MMGSDI_NV_CONTEXT_SECONDARY;
       break;

    case MCFG_REFRESH_INDEX_2:
      context = MMGSDI_NV_CONTEXT_TERTIARY;
       break;

    default:
      context = MMGSDI_NV_CONTEXT_INVALID  ;
       break;
  }
  
  return context;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_autoselect_enabled

  DESCRIPTION
    This function is used to get refresh autoenabled setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_refresh_get_timer
(
  mcfg_refresh_index_type slot_index
)
{
  uint8 timer;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_REFRESH_INDEX_MAX);

  if (mcfg_refresh_cache.slotdata[slot_index].timer == 0)
  {
    status = mcfg_nv_read(MCFG_NV_ITEM_REFRESH_TIMER, 0, slot_index,
                          &timer, sizeof(timer));
    if (status != MCFG_NV_STATUS_OK)
    {
      timer = MCFG_REFRESH_TIMER_TIMEOUT;
      if (status != MCFG_NV_STATUS_NOT_ACTIVE)
      {
        MCFG_MSG_HIGH_2("Couldn't read refresh timer: status %d sloti %d",
                         status, slot_index);
      }
    }
    MCFG_MSG_LOW_1("mcfg_refresh_get_timer timeout %ds", timer);

    mcfg_refresh_cache.slotdata[slot_index].timer = timer;
  }

  return mcfg_refresh_cache.slotdata[slot_index].timer;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_autoselect_enabled

  DESCRIPTION
    This function is used to get refresh autoenabled setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_refresh_read_autoselect_nv
(
  mcfg_refresh_index_type slot_index
)
{
  uint8 autoselect_enabled;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_REFRESH_INDEX_MAX);

  status = mcfg_nv_read(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, slot_index,
                        &autoselect_enabled, sizeof(autoselect_enabled));
  if (status != MCFG_NV_STATUS_OK)
  {
    autoselect_enabled = MCFG_UIM_AUTOSELECT_DEFAULT;
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      MCFG_MSG_HIGH_2("Couldn't read autoselect feature NV item: status %d sloti %d",
                       status, slot_index);
    }
  }
  MCFG_MSG_LOW_2("mcfg_refresh_autoselect_enabled NV=%x for sloti %d", 
                 autoselect_enabled, slot_index);

  return autoselect_enabled;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_enabled

  DESCRIPTION
    This function is used to get refresh autoenabled setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_enabled
(
  mcfg_refresh_index_type slot_index
)
{
  return mcfg_uim_nv_refresh_enabled((mcfg_slot_index_type_e_type)slot_index);
}

/*===========================================================================

  FUNCTION  mcfg_refresh_autoselect_enabled

  DESCRIPTION
    This function is used to get refresh autoenabled setting

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_autoselect_enabled
(
  mcfg_refresh_index_type slot_index
)
{
  return (mcfg_refresh_enabled(slot_index) && 
         (mcfg_uim_nv_autoselect_enabled((mcfg_slot_index_type_e_type)slot_index) ||
          mcfg_uim_nv_imsiselect_enabled((mcfg_slot_index_type_e_type)slot_index)));
}

/*===========================================================================

  FUNCTION  mcfg_refresh_abort

  DESCRIPTION
    This function is used to abort current refresh process

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_abort
(
  mcfg_refresh_index_type slot_index
)
{
  MCFG_ASSERT_COND_RET_FALSE(slot_index <= MCFG_REFRESH_INDEX_MAX);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&mcfg_refresh_cache.slotdata[slot_index], 0, 
         sizeof(mcfg_refresh_cache_slotdata_s_type));
  mcfg_sel_sw_config_store_updated((mcfg_slot_index_type_e_type)slot_index);
  return TRUE;
}
/*===========================================================================

  FUNCTION mcfg_refresh_is_ongoing

  DESCRIPTION
    Check if refresh is ongoing for any of the slot/sub specified in mask

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    True if efresh is ongoing for any of the slot/sub, FALSE if all idle

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_is_ongoing
(
  mcfg_refresh_index_mask_type slot_mask
)
{
  mcfg_refresh_index_type sloti;
  mcfg_refresh_state_e_type state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (sloti = MCFG_REFRESH_INDEX_0; sloti < MCFG_REFRESH_INDEX_MAX; sloti++)
  {
    state = mcfg_refresh_get_state(sloti);
    if((slot_mask & (1<<sloti)) && 
       (state != MCFG_REFRESH_STATE_IDLE))
    {
      MCFG_MSG_MED_2("Still refreshing for sloti %d in state %d", sloti, state);
      return TRUE;
    }
  }

  return FALSE;
} /* mcfg_refresh_is_ongoing() */

boolean mcfg_refresh_is_ongoing_by_type
(
  mcfg_refresh_index_mask_type slot_mask,
  mcfg_refresh_type_e_type refresh_type
)
{
  mcfg_refresh_index_type sloti;
  mcfg_refresh_state_e_type state, cur_state;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state = (refresh_type == MCFG_REFRESH_TYPE_SUBS) ? 
    MCFG_REFRESH_STATE_WAIT_FOR_SESSION : MCFG_REFRESH_STATE_IDLE;

  for (sloti = MCFG_REFRESH_INDEX_0; sloti < MCFG_REFRESH_INDEX_MAX; sloti++)
  {
    cur_state = mcfg_refresh_get_state(sloti);
    if((slot_mask & (1<<sloti)) && (cur_state > state))
    {
      if (refresh_type == MCFG_REFRESH_TYPE_SUBS)
      {
        MCFG_MSG_MED_2("Still refreshing sub items at state %d for sloti %d", cur_state, sloti);
      }
      else
      {
        MCFG_MSG_MED_2("Still refreshing slot/sub items at state %d for sloti %d", cur_state, sloti);
      }
      return TRUE;
    }
  }

  return FALSE;
} /* mcfg_refresh_is_ongoing_by_type() */

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_shelve_uimdata

  DESCRIPTION
    To shelve UIM update cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_shelve_uimdata
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
  mcfg_refresh_uimdata_s_type    *b2b = NULL;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uimdata);
  MCFG_ASSERT_COND_RET_FALSE(uimdata->slot_index < MCFG_REFRESH_NUM_OF_SLOTS);
  b2b = mcfg_refresh_cache.slotdata[uimdata->slot_index].b2b;
  if (b2b == NULL)
  {
    b2b = mcfg_malloc(sizeof(mcfg_refresh_uimdata_s_type));
    mcfg_refresh_cache.slotdata[uimdata->slot_index].b2b = b2b;
  }
  else
  {
    MCFG_MSG_HIGH_1("Received new b2b update, overwriting ref id %lx", 
                     b2b->reference_id);
  }

  if (b2b)
  {
    MCFG_MSG_HIGH_1("received b2b update with ref id %lx, caching", 
                     uimdata->reference_id);
    memscpy(b2b, sizeof(mcfg_refresh_uimdata_s_type),
            uimdata, sizeof(mcfg_refresh_uimdata_s_type));

    if (uimdata->iccid.data_len != 0 && uimdata->iccid.data_ptr)
    {
      b2b->iccid.data_ptr = mcfg_malloc(uimdata->iccid.data_len);
      if (b2b->iccid.data_ptr)
      {
        b2b->iccid.data_len = uimdata->iccid.data_len;
        memscpy(b2b->iccid.data_ptr, b2b->iccid.data_len,
                uimdata->iccid.data_ptr, uimdata->iccid.data_len);
        MCFG_MSG_HIGH_2("Caching b2b iccid data sloti %d, subi %d", 
                         uimdata->slot_index, uimdata->sub_index);
      }
      else
      {
        MCFG_MSG_ERROR_1("malloc failed for iccid %ld", uimdata->iccid.data_len);
        b2b->iccid.data_len = 0;
      }
    }

    if (uimdata->imsi.data_len != 0 && uimdata->imsi.data_ptr)
    {
      b2b->imsi.data_ptr = mcfg_malloc(uimdata->imsi.data_len);
      if (b2b->imsi.data_ptr)
      {
        b2b->imsi.data_len = uimdata->imsi.data_len;
        memscpy(b2b->imsi.data_ptr, b2b->imsi.data_len,
                uimdata->imsi.data_ptr, uimdata->imsi.data_len);
        MCFG_MSG_HIGH_2("Caching b2b iccid data sloti %d, subi %d", 
                         uimdata->slot_index, uimdata->sub_index);
      }
      else
      {
        MCFG_MSG_ERROR_1("Can't malloc for imsi %ld", uimdata->imsi.data_len);
        b2b->imsi.data_len = 0;
      }
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("malloc failed for b2b iccid update with ref id %lx", 
                     uimdata->reference_id);
    return MCFG_ERR_NULL_POINTER;
  }

  return MCFG_ERR_NONE;
} /* mcfg_refresh_cmd_shelve_uimdata() */

/*===========================================================================

  FUNCTION mcfg_refresh_unshelv_uimdata

  DESCRIPTION
    To shelve UIM update cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_unshelv_uimdata
(
  mcfg_refresh_index_mask_type slot_mask
)
{
  mcfg_refresh_index_type sloti;
  mcfg_refresh_uimdata_s_type *b2b = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_LOW_1("check unshelv_uimdata request for slot_mask %x", slot_mask);
  for (sloti = MCFG_REFRESH_INDEX_0; sloti < MCFG_REFRESH_INDEX_MAX; sloti++)
  {
    b2b = mcfg_refresh_cache.slotdata[sloti].b2b;
    if((slot_mask & (1<<sloti)) && (b2b != NULL))
    {
      MCFG_MSG_MED_1("unshelv_uimdata for sloti %d", sloti);
      mcfg_refresh_update_from_uim(b2b);
      if (b2b->iccid.data_len && b2b->iccid.data_ptr)
      {
        mcfg_free(b2b->iccid.data_ptr);
      }
      if (b2b->imsi.data_len && b2b->imsi.data_ptr)
      {
        mcfg_free(b2b->imsi.data_ptr);
      }
      mcfg_free(b2b);
      mcfg_refresh_cache.slotdata[sloti].b2b = NULL;
      return TRUE;
    }
  }

  return FALSE;
} /* mcfg_refresh_unshelv_uimdata() */

/*===========================================================================

  FUNCTION mcfg_refresh_resume

  DESCRIPTION
    To resume onheld refresh

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_resume
(
  mcfg_refresh_index_mask_type slot_mask
)
{
  mcfg_refresh_index_type sloti;
  mcfg_refresh_state_e_type state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_LOW_1("check mcfg_refresh_resume request for slotmaks %x", slot_mask);
  for (sloti = MCFG_REFRESH_INDEX_0; sloti < MCFG_REFRESH_INDEX_MAX; sloti++)
  {
    if(slot_mask & (1<<sloti))
    {
      state = mcfg_refresh_get_state(sloti);
      if (state == MCFG_REFRESH_STATE_ONHOLD || state == MCFG_REFRESH_STATE_TEAR_DOWN_NACK)
      {
        MCFG_MSG_MED_2("mcfg_refresh_resume for sloti %d from state %d", sloti, state);
        mcfg_refresh_tear_down(mcfg_refresh_slot_to_sub_index(sloti));
        return TRUE;
      }
    }
  }

  return mcfg_refresh_unshelv_uimdata(MCFG_REFRESH_INDEX_MASK_ALL);
} /* mcfg_refresh_resume() */

/*===========================================================================

  FUNCTION mcfg_refresh_resume_start_timer

  DESCRIPTION
    To resume onheld refresh

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_refresh_resume_start_timer
(
  void
)
{
  mcfg_refresh_index_type sloti;
  mcfg_refresh_state_e_type state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (sloti = MCFG_REFRESH_INDEX_0; sloti < MCFG_REFRESH_INDEX_MAX; sloti++)
  {
    state = mcfg_refresh_get_state(sloti);
    if (state == MCFG_REFRESH_STATE_ONHOLD || state == MCFG_REFRESH_STATE_TEAR_DOWN_NACK 
        || mcfg_refresh_cache.slotdata[sloti].b2b)
    {
      MCFG_MSG_MED_1("Starting refresh timer to resume refresh on sloti %d", sloti);
      timer_set(&mcfg_refresh_timer[sloti], 1, 0, T_SEC);
      return;
    }
  }

  return;
} /* mcfg_refresh_resume() */


/*===========================================================================

  FUNCTION  mcfg_refresh_cm_event_cb

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_refresh_cm_event_cb(
  cm_call_event_e_type          call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{
  static cm_client_id_type mcfg_cm_client_id = CM_CLIENT_ID_ANONYMOUS;
  cm_client_status_e_type status;
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(call_event == CM_CALL_EVENT_END);

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_CM_CALL_END;

    MCFG_MSG_MED_1("call end sub %d", call_info_ptr->asubs_id);

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for call end!");
      mcfg_task_cmd_free(cmd);
    }
  }
  
  return;
} /* mcfg_refresh_cm_event_cb() */


/*===========================================================================

  FUNCTION  mcfg_refresh_cm_ph_event_cb

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_refresh_cm_ph_event_cb(
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type *event_ptr
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(ph_event == CM_PH_EVENT_SYS_SEL_PREF);

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_CM_PH_STATE;
    
    refresh_cmd_data->data.cmphstate.ph_state = event_ptr->ph_state;
    refresh_cmd_data->data.cmphstate.sub_index = \
                      (mcfg_refresh_index_type)event_ptr->asubs_id;

    MCFG_MSG_MED_1("cm_ph_event sub %d", event_ptr->asubs_id);

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for cm ph state!");
      mcfg_task_cmd_free(cmd);
    }
  }
  
  return;
} /* mcfg_refresh_cm_ph_event_cb() */

/*===========================================================================

  FUNCTION  mcfg_refresh_register

  DESCRIPTION
    This function is used to register with MCFG to receive refresh update
    through registered callback function for MCFG switching

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_register
(
  mcfg_refresh_registration_s_type *p_reg
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(p_reg);

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_REGISTER;

    memscpy(&(refresh_cmd_data->data.register_req),
            sizeof(mcfg_refresh_registration_s_type),
            p_reg, 
            sizeof(mcfg_refresh_registration_s_type));

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for refresh register!");
      mcfg_task_cmd_free(cmd);
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_done

  DESCRIPTION
    This function is used to notify MCFG task that registered client has
    completed refresh operation. The client is expected to return the same
    reference id passed through registered callback

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_done
(
  uint32 reference_id
)
{
  return mcfg_refresh_done_w_status(reference_id, MCFG_REFRESH_SUCCESS);
};

/* ==========================================================================
FUNCTION: mcfg_refresh_done_w_status

  This function is used to notify MCFG task that registered client has
  completed refresh operation. The client is expected to return the same
  reference id passed through registered callback with status
 
  @param[in]reference_id:  reference_id
        [in]status: status

  @return
  TRUE for success, otherwise FALSE

  @dependencies
  None
===========================================================================*/
boolean mcfg_refresh_done_w_status
(
  uint32 reference_id,
  mcfg_refresh_status_e_type status
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_DONE;
    refresh_cmd_data->data.info.reference_id = reference_id;

    refresh_cmd_data->status = status;

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for refresh done!");
      mcfg_task_cmd_free(cmd);
      return FALSE;
    }
  }

  MCFG_MSG_MED_2("mcfg_refresh_done reference_id=%x w/ status %d", reference_id, status);

  return TRUE;
}

void mcfg_refresh_tear_down_ack
(
  void                    *data_block_ptr,
  cm_ph_cmd_e_type        ph_cmd,
  cm_ph_cmd_err_e_type    ph_cmd_err
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(data_block_ptr);

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_MMCP_TEAR_DOWN_ACK;

    refresh_cmd_data->data.mmcpdata.sub_index = 
                  *((mcfg_refresh_index_type *)data_block_ptr);
    refresh_cmd_data->data.mmcpdata.ph_cmd_err = ph_cmd_err;

    MCFG_MSG_LOW_2("tear_down_cb subi %d err %d", 
                   refresh_cmd_data->data.mmcpdata.sub_index, ph_cmd_err);

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for tear down!");
      mcfg_task_cmd_free(cmd);
    }
  }
  mcfg_free(data_block_ptr);
  
}

/*===========================================================================

  FUNCTION  mcfg_refresh_tear_down

  DESCRIPTION
    Tearing down the protocol stack before sub related item refresh

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_tear_down
(
  mcfg_refresh_index_type sub_index
)
{
  mcfg_error_e_type err = MCFG_ERR_NONE;
  mcfg_refresh_index_type *sub_data_ptr;
/*-------------------------------------------------------------------------*/
  MCFG_ASSERT_COND_RET_FALSE(sub_index < MCFG_REFRESH_NUM_OF_SUBS);
  MCFG_MSG_MED_1("mcfg_refresh_tear_down subi %d", sub_index);

  if(mcfg_refresh_cache.mcfg_cm_client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    MCFG_MSG_MED("Not initialized with CM yet");
    mcfg_refresh_set_state(mcfg_refresh_sub_to_slot_index(sub_index), 
                           MCFG_REFRESH_STATE_WAIT_CM_INIT);
    return MCFG_ERR_DEVICE_IN_USE; 
  }

  sub_data_ptr = mcfg_malloc(sizeof(mcfg_refresh_index_type));
  if (sub_data_ptr)
  {
    *sub_data_ptr = sub_index;
  }

  mcfg_refresh_set_state(mcfg_refresh_sub_to_slot_index(sub_index), 
                         MCFG_REFRESH_STATE_TEAR_DOWN_REQ);

  if(!cm_ph_cmd_deactivate_subs(mcfg_refresh_tear_down_ack, 
                                sub_data_ptr, 
                                mcfg_refresh_cache.mcfg_cm_client_id, 
                                1 << sub_index))
  {
    err = MCFG_ERR_DEVICE_IN_USE;
    mcfg_refresh_set_state(mcfg_refresh_sub_to_slot_index(sub_index), 
                           MCFG_REFRESH_STATE_TEAR_DOWN_NACK);
  }
  
  return err;
}

/*===========================================================================

 FUNCTION  mcfg_refresh_update_validation

  DESCRIPTION
    vaidating uimdata from UIM

  DEPENDENCIES
    None

  PARAMETERS
  uimdata

  RETURN VALUE
    TRUE: If validation success
    FALSE: Otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_update_validation
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
   boolean status = TRUE;
  if (uimdata->type == MCFG_REFRESH_UIM_ICCID) 
  {
     if (uimdata->iccid.data_ptr == NULL || uimdata->iccid.data_len == 0)
     {
        MCFG_MSG_ERROR_2("uimdata->iccid.data_ptr is  %p uimdata->iccid.data_len %ld", uimdata->iccid.data_ptr, uimdata->iccid.data_len);
        status = FALSE;
     }
     else if(uimdata->slot_index > MCFG_REFRESH_INDEX_MAX || uimdata->slot_index == MCFG_REFRESH_INDEX_INVALID)
     {
        MCFG_MSG_ERROR_1("invalid sloti %d", uimdata->slot_index);
        status = FALSE;
     }
     else
     {
        MCFG_MSG_LOW("valid iccid from uim ! ");
     }
  }
  else if(uimdata->type == MCFG_REFRESH_UIM_IMSI)
  {
     if (uimdata->imsi.data_ptr == NULL || uimdata->imsi.data_len == 0)
     {
        MCFG_MSG_ERROR_2("uimdata->imsi.data_ptr is  %p uimdata->imsi.data_len  %ld", uimdata->imsi.data_ptr, uimdata->iccid.data_len);
        status = FALSE;
     }
     else if(uimdata->slot_index > MCFG_REFRESH_INDEX_MAX || uimdata->slot_index == MCFG_REFRESH_INDEX_INVALID)
     {
        MCFG_MSG_ERROR_1("invalid sloti %d", uimdata->slot_index);
        status = FALSE;
     }
     else if(uimdata->sub_index > MCFG_REFRESH_INDEX_MAX || uimdata->sub_index == MCFG_REFRESH_INDEX_INVALID)
     {
        MCFG_MSG_ERROR_1("invalid subi %d", uimdata->sub_index);
        status = FALSE;
     }
     else
     {
        MCFG_MSG_LOW("valid imsi from uim ! ");
     }
  }
  else if(uimdata->type == MCFG_REFRESH_UIM_SESSION)
  {
     if(uimdata->slot_index > MCFG_REFRESH_INDEX_MAX || uimdata->slot_index == MCFG_REFRESH_INDEX_INVALID)
     {
        MCFG_MSG_ERROR_1("invalid sloti %d", uimdata->slot_index);
        status = FALSE;
     }
     else if(uimdata->sub_index > MCFG_REFRESH_INDEX_MAX || uimdata->sub_index == MCFG_REFRESH_INDEX_INVALID)
     {
        MCFG_MSG_ERROR_1("invalid subi %d", uimdata->sub_index);
        status = FALSE;
     }
     else
     {
        MCFG_MSG_LOW("valid session from uim ! ");
     }
  }
  else
  {
     MCFG_MSG_ERROR_2("Invalid refresh type from uim %d sloti %d", uimdata->type, uimdata->slot_index);
     status = FALSE;
  }

  return status;
}/* mcfg_refresh_update_validation() */

/*===========================================================================
  FUNCTION  mcfg_refresh_update_from_uim

  DESCRIPTION
    Process uim update call

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_update_from_uim
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uimdata);

  if (!mcfg_refresh_update_validation(uimdata)) 
  {
     MCFG_MSG_ERROR_2("Vaildation failed for uimdata from uim %d sloti %d", uimdata->type, uimdata->slot_index);
     return FALSE;
  }
  
  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_UIMDATA;

    memscpy(&(refresh_cmd_data->data.uimdata), sizeof(mcfg_refresh_uimdata_s_type),
            uimdata, sizeof(mcfg_refresh_uimdata_s_type));

    if (uimdata->iccid.data_len != 0 && uimdata->iccid.data_ptr)
    {
      refresh_cmd_data->data.uimdata.iccid.data_ptr = 
        mcfg_malloc(uimdata->iccid.data_len);
      if (refresh_cmd_data->data.uimdata.iccid.data_ptr)
      {
        refresh_cmd_data->data.uimdata.iccid.data_len = uimdata->iccid.data_len;
        memscpy(refresh_cmd_data->data.uimdata.iccid.data_ptr,
                refresh_cmd_data->data.uimdata.iccid.data_len,
                uimdata->iccid.data_ptr, 
                uimdata->iccid.data_len);

      }
      else
      {
        MCFG_MSG_ERROR_1("malloc failed for iccid %ld", uimdata->iccid.data_len);
        refresh_cmd_data->data.uimdata.iccid.data_len = 0;
      }
    }

    if (uimdata->imsi.data_len != 0 && uimdata->imsi.data_ptr)
    {
      refresh_cmd_data->data.uimdata.imsi.data_ptr = 
        mcfg_malloc(uimdata->imsi.data_len);
      if (!refresh_cmd_data->data.uimdata.imsi.data_ptr)
      {
        refresh_cmd_data->data.uimdata.imsi.data_len = uimdata->imsi.data_len;
        memscpy(refresh_cmd_data->data.uimdata.imsi.data_ptr,
                refresh_cmd_data->data.uimdata.imsi.data_len,
                uimdata->imsi.data_ptr, 
                uimdata->imsi.data_len);

      }
      else
      {
        MCFG_MSG_ERROR_1("Can malloc for imsi %ld", uimdata->imsi.data_len);
        refresh_cmd_data->data.uimdata.imsi.data_len = 0;
      }
    }

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for uimdata update!");
      mcfg_task_cmd_free(cmd);
      return FALSE;
    }
  }

  return TRUE;
} /* mcfg_refresh_update_from_uim() */

/*===========================================================================

  FUNCTION  mcfg_refresh_client_by_order

  DESCRIPTION
    refresh registered client by specified order group

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
uint8 mcfg_refresh_client_by_order
(
  mcfg_refresh_type_e_type      type,
  mcfg_refresh_index_type       slot,
  mcfg_refresh_index_type       sub,
  mcfg_refresh_order_e_type     order
)
{
  mcfg_refresh_index_mask_type slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_refresh_index_mask_type sub_mask = MCFG_REFRESH_INDEX_MASK_NONE;

  mcfg_refresh_client_node_type *refresh_node = NULL;
  mcfg_refresh_info_s_type info;
  static uint32 reference_id = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(sub <= MCFG_REFRESH_INDEX_MAX, 0);

  MCFG_MSG_LOW_4("mcfg_refresh_client_by_order type %d sloti %d subi %d order %d", 
                  type, slot, sub, order);
  if (type == MCFG_REFRESH_TYPE_SLOT || type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    slot_mask = (1 << slot);
  }

  if (type == MCFG_REFRESH_TYPE_SUBS || type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    sub_mask = (1 << sub);
  }

  refresh_node = list_peek_front(&(mcfg_refresh_cache.client_list));
  while (refresh_node != NULL)
  {
    if(refresh_node->client.reg.cb && (order == refresh_node->client.reg.order) &&
       ((slot_mask & refresh_node->client.reg.slot_mask) ||
       (sub_mask & refresh_node->client.reg.sub_mask)))
    {
      info.type = type;
      if (type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
      {
        if (!(sub_mask & refresh_node->client.reg.sub_mask))
        {
          info.type = MCFG_REFRESH_TYPE_SLOT;
        }
        else if (!(slot_mask & refresh_node->client.reg.slot_mask))
        {
          info.type = MCFG_REFRESH_TYPE_SUBS;
        }
      }
      
      info.slot_index = slot;
      info.sub_index = sub;
      reference_id++; 
      info.reference_id = reference_id;
      refresh_node->client.reference_id[slot] = reference_id;
      mcfg_refresh_cache.slotdata[slot].cb_count++;
      if((refresh_node->client.reg.cb)(&info))
      {
        MCFG_MSG_MED_3("refresh cb %p order %d reference_id %x", 
                       refresh_node->client.reg.cb, 
                       refresh_node->client.reg.order, 
                       reference_id);
      }
      else
      {
        MCFG_MSG_ERROR_2("refresh cb %p for ref id %lx failed", 
                       refresh_node->client.reg.cb, reference_id);
        mcfg_refresh_cache.slotdata[slot].cb_count--;
        refresh_node->client.reference_id[slot] = 0;
      }
    }

    refresh_node = list_peek_next(&(mcfg_refresh_cache.client_list), 
                                  &(refresh_node->link));
  }

  return mcfg_refresh_cache.slotdata[slot].cb_count;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_clients

  DESCRIPTION
    refresh registered clients

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_clients
(
  mcfg_refresh_type_e_type  type,
  mcfg_refresh_index_type   slot
)
{
  uint8 n_refreshed;
  mcfg_refresh_orig_e_type orig;
/*-------------------------------------------------------------------------*/
  MCFG_ASSERT_COND_RET_ERROR(slot < MCFG_REFRESH_NUM_OF_SLOTS, 
                             MCFG_ERR_INVALID_PARAMETER)
  MCFG_ASSERT_COND_RET_ERROR(type <= MCFG_REFRESH_TYPE_MAX, 
                             MCFG_ERR_INVALID_PARAMETER)

  MCFG_MSG_MED_3("mcfg_refresh_clients type %d sloti %d order %d", 
                  type, slot, mcfg_refresh_cache.slotdata[slot].current_order);
  while(mcfg_refresh_cache.slotdata[slot].current_order < MCFG_REFRESH_ORDER_MAX)
  {
    mcfg_refresh_cache.slotdata[slot].current_order++;

    n_refreshed = mcfg_refresh_client_by_order(type, slot, 
      mcfg_refresh_slot_to_sub_index(slot), mcfg_refresh_cache.slotdata[slot].current_order);
    if (n_refreshed > 0)
    {
      MCFG_MSG_LOW_2("refresh %d clients for order %d, wait for refresh done call back", 
                      n_refreshed, mcfg_refresh_cache.slotdata[slot].current_order);
      return MCFG_ERR_NONE;
    }

  }
  
  mcfg_refresh_cache.slotdata[slot].current_order = MCFG_REFRESH_ORDER_INIT;
  if (type == MCFG_REFRESH_TYPE_SLOT)
  {
    mcfg_refresh_set_state(slot, MCFG_REFRESH_STATE_WAIT_FOR_SESSION);
    mmgsdi_continue_after_nv_refresh(
      mcfg_refresh_slot_index_to_mmgsdi_slot(slot), 
      mcfg_refresh_sub_index_to_mmgsdi_context(mcfg_refresh_slot_to_sub_index(slot)),
      mcfg_refresh_cache.slotdata[slot].mmgsdi_reference);

  }
  else
  {
    cm_ph_cmd_activate_subs(NULL, NULL, mcfg_refresh_cache.mcfg_cm_client_id, 
                            1 << mcfg_refresh_slot_to_sub_index(slot));
    mmgsdi_continue_after_nv_refresh(
      mcfg_refresh_slot_index_to_mmgsdi_slot(slot), 
      mcfg_refresh_sub_index_to_mmgsdi_context(mcfg_refresh_slot_to_sub_index(slot)),
      mcfg_refresh_cache.slotdata[slot].mmgsdi_reference);

    orig = mcfg_refresh_get_orig(slot);
    if (orig == MCFG_REFRESH_ORIG_APP)
    {
#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01
      MCFG_MSG_LOW_1("APP centric, send activate ind for sloti %d", slot);
      pdc_send_cached_activate_config_ind(slot, MCFG_ERR_NONE);
#endif
    }
    else if (orig == MCFG_REFRESH_ORIG_PDC)
    {
#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01
      MCFG_MSG_LOW_1("PDC triggered, send activate config ind for sloti %d", slot);
      pdc_send_cached_activate_config_ind(slot, MCFG_ERR_NONE);
#endif
#ifndef REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01
      MCFG_MSG_LOW_1("PDC triggered, send config change ind for sloti %d", slot);
      pdc_generate_config_change_ind(MCFG_TYPE_SW, &mcfg_refresh_cache.slotdata[slot].config_id);
#endif
    }
    else if (orig == MCFG_REFRESH_ORIG_UIM || orig == MCFG_REFRESH_ORIG_FLEX)
    {
#ifndef REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01
      MCFG_MSG_LOW_1("UIM triggered, send config change ind for sloti %d", slot);
      pdc_generate_config_change_ind(MCFG_TYPE_SW, &mcfg_refresh_cache.slotdata[slot].config_id);
#endif
    }
    else
    {
      MCFG_MSG_LOW_1("no change for UIM triggered for sloti %d", slot);
    }

    memset(&mcfg_refresh_cache.slotdata[slot].config_id, 0, sizeof(mcfg_config_id_s_type));
    mcfg_refresh_set_state(slot, MCFG_REFRESH_STATE_IDLE);
  }

  return MCFG_ERR_NONE;
}

/*===========================================================================

  FUNCTION  mcfg_refresh_cmd_proc_register

  DESCRIPTION
    process refresh register cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_proc_register
(
  mcfg_refresh_registration_s_type *p_reg
)
{
  mcfg_refresh_client_node_type      *refresh_node;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(p_reg);
  MCFG_CHECK_NULL_PTR_RET_FALSE(p_reg->cb);

  /*create a Node Element*/
  refresh_node = (mcfg_refresh_client_node_type *)
    mcfg_malloc(sizeof(mcfg_refresh_client_node_type));
  MCFG_CHECK_NULL_PTR_RET_FALSE(refresh_node);

  memset(refresh_node, 0, sizeof(mcfg_refresh_client_node_type));
  memscpy(&(refresh_node->client.reg),
          sizeof(mcfg_refresh_registration_s_type),
          p_reg, 
          sizeof(mcfg_refresh_registration_s_type));

  /*If we reach here, a refresh node has been successfully created,
    Insert the node into the list Now*/
  list_push_back(&(mcfg_refresh_cache.client_list), &(refresh_node->link));

  MCFG_MSG_LOW_2("mcfg_refresh_add_client order %d cb %p", 
                 p_reg->order, p_reg->cb);

  return MCFG_ERR_NONE;
}

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_proc_done

  DESCRIPTION
    process refresh callback done cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_proc_done
(
  mcfg_refresh_info_s_type  *info,
  mcfg_refresh_status_e_type status
)
{
  mcfg_refresh_client_node_type *refresh_node;
  mcfg_refresh_type_e_type  type;
  mcfg_refresh_index_type   slot;
  mcfg_refresh_orig_e_type  orig;

  boolean found = FALSE;
  mcfg_refresh_state_e_type state;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_ERROR(info, MCFG_ERR_INVALID_PARAMETER);

  refresh_node = list_peek_front(&(mcfg_refresh_cache.client_list));
  while (refresh_node != NULL && !found)
  {
    for (slot = MCFG_REFRESH_INDEX_MIN; slot <= MCFG_REFRESH_INDEX_MAX; slot++)
    {
      if(info->reference_id == refresh_node->client.reference_id[slot])
      {
        found = TRUE;
        refresh_node->client.reference_id[slot] = 0;
        break;
      }
    }
    refresh_node = list_peek_next(&(mcfg_refresh_cache.client_list), &(refresh_node->link));
  }

  if (!found)
  {
    MCFG_MSG_ERROR_1("Unable to find client for reference_id %lu", info->reference_id); 
    return MCFG_ERR_INVALID_REFERENCE;
  }
  //MCFG_MSG_MED_2("find client for reference_id %d for sloti %d", info->reference_id, slot); 
  
  state = mcfg_refresh_get_state(slot);
  if (state == MCFG_REFRESH_STATE_UPDATING_SLOT)
  {
    type = MCFG_REFRESH_TYPE_SLOT;
  }
  else if (state == MCFG_REFRESH_STATE_UPDATING_SUBS)
  {
    orig = mcfg_refresh_get_orig(slot);
    type = (orig == MCFG_REFRESH_ORIG_UIM) ?
      MCFG_REFRESH_TYPE_SUBS : MCFG_REFRESH_TYPE_SLOT_N_SUBS; 
  }
  else
  {
    MCFG_MSG_ERROR_2("proc_done invalid state %d for sloti %d", state, slot); 
    return MCFG_ERR_INVALID_STATE;
  }

  mcfg_refresh_cache.slotdata[slot].cb_count--;

  if (status != MCFG_REFRESH_SUCCESS)
  {
    mcfg_refresh_cache.slotdata[slot].cb_count = 0;
    MCFG_MSG_ERROR_2("refresh done w/ error %d for reference %lx", 
                   status, info->reference_id);
    orig = mcfg_refresh_get_orig(slot);
    if (orig == MCFG_REFRESH_ORIG_APP)
    {
#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01
      MCFG_MSG_HIGH_1("APP centric, send activate config ind and abort for sloti %d", slot);
      pdc_send_cached_activate_config_ind(slot, MCFG_ERR_REFRESH_FAILED);
#endif
    }
    else if (orig == MCFG_REFRESH_ORIG_PDC)
    {
#ifndef REMOVE_QMI_PDC_ACTIVATE_CONFIG_IND_V01
      MCFG_MSG_HIGH_1("PDC triggered, send activate config ind and reset for sloti %d", slot);
      pdc_send_cached_activate_config_ind(slot, MCFG_ERR_REFRESH_FAILED);
#endif
      mcfg_utils_reset_modem(MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE);
    }
    else if (orig == MCFG_REFRESH_ORIG_UIM || orig == MCFG_REFRESH_ORIG_FLEX)
    {
#ifndef REMOVE_QMI_PDC_CONFIG_CHANGE_IND_V01
      MCFG_MSG_HIGH_2("UIM triggered, reset for sloti %d orig %d", slot, orig);
      pdc_generate_config_change_ind(MCFG_TYPE_SW, &mcfg_refresh_cache.slotdata[slot].config_id);
#endif
      mcfg_utils_reset_modem(MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE);
    }

    mcfg_refresh_abort(slot);
    mcfg_refresh_set_state(slot, MCFG_REFRESH_STATE_IDLE);
    return MCFG_ERR_REFRESH_FAILED;
  }

  MCFG_MSG_LOW_3("cb_count=%d state=%d sloti=%d", mcfg_refresh_cache.slotdata[slot].cb_count, state, slot);
  if (mcfg_refresh_cache.slotdata[slot].cb_count == 0)
  {
    mcfg_refresh_clients(type, slot);
  }
  
  return MCFG_ERR_NONE;
}

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_proc_uimdata_iccid

  DESCRIPTION
    process UIM update iccid cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_proc_uimdata_iccid
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
  char card_iccid[MCFG_UIM_ICCID_STR_BUF_LEN];
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uimdata);
  MCFG_CHECK_NULL_PTR_RET_FALSE(uimdata->iccid.data_ptr);
  MCFG_ASSERT_COND_RET_FALSE(uimdata->slot_index <= MCFG_REFRESH_INDEX_MAX 
                             && uimdata->slot_index != MCFG_REFRESH_INDEX_INVALID);

  mcfg_refresh_cache.slotdata[uimdata->slot_index].mmgsdi_reference = uimdata->reference_id;
  if(!mcfg_refresh_enabled(uimdata->slot_index) 
     || !mcfg_uim_nv_autoselect_enabled((mcfg_slot_index_type_e_type)uimdata->slot_index))
  {
    MCFG_MSG_HIGH_2("autoselection disable, send continue directly sloti %d subi %d", 
                    uimdata->slot_index, uimdata->sub_index);
    mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_WAIT_FOR_SESSION);
    mcfg_refresh_set_orig(uimdata->slot_index, MCFG_REFRESH_ORIG_NOCHANGE);
    mmgsdi_continue_after_nv_refresh(
      mcfg_refresh_slot_index_to_mmgsdi_slot(uimdata->slot_index),
      mcfg_refresh_sub_index_to_mmgsdi_context(uimdata->sub_index),
      uimdata->reference_id);
  }
  else
  {
    mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_ACTIVATING_SLOT);

    mcfg_refresh_set_orig(uimdata->slot_index, MCFG_REFRESH_ORIG_UIM);

    if(mcfg_uim_decode_iccid(uimdata->iccid.data_ptr, card_iccid))
    {
      MCFG_MSG_SPRINTF_2("Matching new config for ICCID %s for sloti %d", card_iccid, uimdata->slot_index);
      mcfg_sel_sw_by_iccid((mcfg_sub_id_type_e_type)mcfg_refresh_slot_to_sub_index(uimdata->slot_index), 
                           card_iccid);
    }

    if(mcfg_refresh_get_state(uimdata->slot_index) != MCFG_REFRESH_STATE_UPDATING_SLOT)
    {
      mcfg_refresh_set_orig(uimdata->slot_index, MCFG_REFRESH_ORIG_NOCHANGE);
      mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_WAIT_FOR_SESSION);
      mmgsdi_continue_after_nv_refresh(
        mcfg_refresh_slot_index_to_mmgsdi_slot(uimdata->slot_index), 
        MMGSDI_NV_CONTEXT_INVALID,
        uimdata->reference_id);
    }
  }

  return MCFG_ERR_NONE;
}

/*===========================================================================
 
  FUNCTION mcfg_refresh_cmd_proc_uimdata_session

  DESCRIPTION
    process UIM update session cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_proc_uimdata_session
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
  boolean ret = TRUE;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(uimdata);

  MCFG_MSG_MED_2("mcfg_refresh_cmd_proc_uimdata_session sloti %d subi %d", 
                 uimdata->slot_index, uimdata->sub_index);

  if (!mcfg_uim_nv_flexmap_enabled((mcfg_slot_index_type_e_type)uimdata->slot_index))
  {
    MCFG_MSG_HIGH("mcfg_refresh_cmd_proc_uimdata_session flex mappign disabled");
    MCFG_ASSERT_COND_RET_FALSE(uimdata->slot_index == uimdata->sub_index);
  }
  else if (mcfg_uim_process_mapping((mcfg_slot_index_type_e_type)uimdata->slot_index, 
                                    (mcfg_sub_id_type_e_type)uimdata->sub_index, TRUE))
  {
    MCFG_MSG_HIGH("mbn switched after flex mapping");
  }

  if (mcfg_utils_selected_config_available(MCFG_TYPE_SW, (mcfg_sub_id_type_e_type)uimdata->sub_index))
  {
    MCFG_MSG_MED_1("pending config set for subi %d", uimdata->sub_index);
    mcfg_refresh_set_orig(uimdata->slot_index, MCFG_REFRESH_ORIG_UIM);
  }
  else
  {
    MCFG_MSG_MED_1("no pending config for subi %d", uimdata->sub_index);
    mcfg_refresh_set_orig(uimdata->slot_index, MCFG_REFRESH_ORIG_NOCHANGE);
  }

  mcfg_refresh_cache.slotdata[uimdata->slot_index].mmgsdi_reference = uimdata->reference_id;
  if(!mcfg_refresh_autoselect_enabled(uimdata->slot_index) || 
     mcfg_refresh_get_orig(uimdata->slot_index) == MCFG_REFRESH_ORIG_NOCHANGE)
  {
        MCFG_MSG_LOW_2("autoselection disabled or no mbn change, send continue directly slot %d sub %d", 
                    uimdata->slot_index, uimdata->sub_index);
    mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_IDLE);
    mmgsdi_continue_after_nv_refresh(
      mcfg_refresh_slot_index_to_mmgsdi_slot(uimdata->slot_index),
      mcfg_refresh_sub_index_to_mmgsdi_context(uimdata->sub_index),
      uimdata->reference_id);
  }
  else
  {
    if (mcfg_refresh_is_ongoing_by_type(MCFG_REFRESH_INDEX_MASK_ALL, MCFG_REFRESH_TYPE_SUBS))
    {
      MCFG_MSG_MED_2("Refresh onhold slot %d sub %d", uimdata->slot_index, uimdata->sub_index);
      mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_ONHOLD);
    }
    else
    {
      ret = mcfg_refresh_tear_down(uimdata->sub_index);
    }
  }
  return ret;
}

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_proc_uimdata

  DESCRIPTION
    process UIM update cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_refresh_cmd_proc_uimdata
(
  mcfg_refresh_uimdata_s_type *uimdata
)
{
  mcfg_refresh_state_e_type state;
  mcfg_error_e_type err = MCFG_ERR_NONE;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_ERROR(uimdata, MCFG_ERR_NULL_POINTER);

  if(uimdata->slot_index >= MCFG_REFRESH_NUM_OF_SLOTS)
  {
    err = MCFG_ERR_INVALID_PARAMETER;
  }
  else
  {
    state = mcfg_refresh_get_state(uimdata->slot_index);
    switch (uimdata->type)
    {
      case MCFG_REFRESH_UIM_ICCID:
        if (state == MCFG_REFRESH_STATE_IDLE)
        {
          if (mcfg_refresh_is_ongoing_by_type(~(1<<uimdata->slot_index), MCFG_REFRESH_TYPE_SUBS))
          {
            MCFG_MSG_HIGH_1("Shelving the iccid update sloti %d while other slot in refreshing", 
                             uimdata->slot_index);
            mcfg_refresh_cmd_shelve_uimdata(uimdata);
          }
          else
          {
            if (mcfg_refresh_cache.slotdata[uimdata->slot_index].b2b)
            {
              MCFG_MSG_HIGH_3("Ignore the shelved b2b update for sloti %d with ref id %lx and process new ref id %lx", 
                             uimdata->slot_index, mcfg_refresh_cache.slotdata[uimdata->slot_index].b2b->reference_id, uimdata->reference_id);
              mcfg_free(mcfg_refresh_cache.slotdata[uimdata->slot_index].b2b);
            }
            mcfg_refresh_cmd_proc_uimdata_iccid(uimdata);
          }
        }
        else
        {
          if (mcfg_refresh_cache.slotdata[uimdata->slot_index].mmgsdi_reference
                == uimdata->reference_id)
          {
            MCFG_MSG_ERROR_2("received unexpected iccid in state %d with same ref id %lx", 
                             state, uimdata->reference_id);
          }
          else
          {
            if (state == MCFG_REFRESH_STATE_UPDATING_SLOT || 
                state == MCFG_REFRESH_STATE_WAIT_FOR_SESSION || 
                state == MCFG_REFRESH_STATE_ONHOLD)
            {
              MCFG_MSG_HIGH_2("received b2b iccid update in state %d with ref id %lx, aborting", 
                              state, uimdata->reference_id);
              mcfg_refresh_abort(uimdata->slot_index);
              mcfg_refresh_set_state(uimdata->slot_index, MCFG_REFRESH_STATE_IDLE);
              mcfg_refresh_cmd_proc_uimdata_iccid(uimdata);
            }
            else
            {
              //save the uimdata
              MCFG_MSG_HIGH_3("received b2b iccid update in state %d with ref id %lx sloti %d, shelving", 
                              state, uimdata->reference_id, uimdata->slot_index);
                mcfg_refresh_cache.slotdata[uimdata->slot_index].aborting = TRUE;
              if(mcfg_refresh_cmd_shelve_uimdata(uimdata) != MCFG_ERR_NONE)
              {
                mcfg_refresh_cache.slotdata[uimdata->slot_index].aborting = FALSE;
              }
            }
          }
        }
        break;

      case MCFG_REFRESH_UIM_SESSION:
      MCFG_MSG_HIGH_3("received UIM session in state %d sloti %d subi %d", 
                                     state, uimdata->slot_index, uimdata->sub_index);
      
      /* shelve uim session, if flex mapping enabled and refresh is in progress for any slot*/
      if (mcfg_uim_nv_flexmap_enabled((mcfg_slot_index_type_e_type)uimdata->slot_index) &&
          (mcfg_refresh_slot_to_sub_index(uimdata->slot_index) != uimdata->sub_index) &&
          (mcfg_refresh_is_ongoing_by_type(MCFG_REFRESH_INDEX_MASK_ALL, MCFG_REFRESH_TYPE_SUBS)))
      {
        MCFG_MSG_HIGH_1("Shelving the UIM session update sub_id %d while refresh in progress", 
                            mcfg_refresh_slot_to_sub_index(uimdata->slot_index));
        
        mcfg_refresh_cmd_shelve_uimdata(uimdata);
      }
      else if (state != MCFG_REFRESH_STATE_WAIT_FOR_SESSION && state != MCFG_REFRESH_STATE_IDLE)
        {
          MCFG_MSG_HIGH_3("received session mapping in state %d sloti %d subi %d, continue", 
                           state, uimdata->slot_index, uimdata->sub_index);
          mmgsdi_continue_after_nv_refresh(
            mcfg_refresh_slot_index_to_mmgsdi_slot(uimdata->slot_index),
            mcfg_refresh_sub_index_to_mmgsdi_context(uimdata->sub_index),
            uimdata->reference_id);
        }
        else
        {
          mcfg_refresh_cmd_proc_uimdata_session(uimdata);
        }
        break;

      case MCFG_REFRESH_UIM_IMSI:
      default:
        MCFG_MSG_ERROR_2("Invalid refresh type from uim %d sloti %d", uimdata->type, uimdata->slot_index);
        break;
    }
  }

  if (uimdata->iccid.data_ptr)
  {
    mcfg_free(uimdata->iccid.data_ptr);
  }

  if (uimdata->imsi.data_ptr)
  {
    mcfg_free(uimdata->imsi.data_ptr);
  }

  return err;
}

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_proc_mmoc_deactivated

  DESCRIPTION
    process MMOC deactivated cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_cmd_proc_mmoc_deactivated
(
  mcfg_refresh_index_mask_type sub_mask
)
{
  mcfg_refresh_state_e_type state;
  mcfg_refresh_orig_e_type orig;
  mcfg_refresh_type_e_type type;
  mcfg_refresh_index_type slot_index;
  mcfg_refresh_index_type sub_index = MCFG_REFRESH_INDEX_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_MSG_MED_1("mcfg_refresh_cmd_proc_mmoc_deactivated Submask%x", sub_mask);
  MCFG_ASSERT_COND_RET_FALSE(sub_mask>0);

  do
  {
    if (sub_mask & (1 << sub_index))
    {
      slot_index = mcfg_refresh_sub_to_slot_index(sub_index);

      state = mcfg_refresh_get_state(slot_index);
      if ( state != MCFG_REFRESH_STATE_TEAR_DOWN_REQ && 
           state != MCFG_REFRESH_STATE_TEAR_DOWN_ACK)
      {
        MCFG_MSG_ERROR_2("Unexpected deactivation success in state %d sloti %d", state, slot_index);
      }
      else
      {
        orig = mcfg_refresh_get_orig(slot_index);
        if (orig == MCFG_REFRESH_ORIG_UIM)
        {
          mcfg_refresh_set_state(slot_index, MCFG_REFRESH_STATE_ACTIVATING_SUBS);
          type = MCFG_REFRESH_TYPE_SUBS;
          modem_cfg_process_single(MCFG_TYPE_SW, type, sub_index);
        }
        else if (orig == MCFG_REFRESH_ORIG_PDC || orig == MCFG_REFRESH_ORIG_FLEX)
        {
          mcfg_refresh_set_state(slot_index, MCFG_REFRESH_STATE_ACTIVATING_SUBS);
          type = MCFG_REFRESH_TYPE_SLOT_N_SUBS;
          mcfg_utils_deactivate_config(MCFG_TYPE_SW, 
                                       (mcfg_sub_id_type_e_type)sub_index, TRUE);
          modem_cfg_process_single(MCFG_TYPE_SW, type, sub_index);
        }
        else
        {
          type = MCFG_REFRESH_TYPE_SLOT_N_SUBS;
          MCFG_MSG_MED_2("Skip activating mcfg for app centric, orig %d sloti %d", orig, slot_index);
        }

        MCFG_MSG_MED_2("Refresh clients for subi %d sloti %d.....", sub_index, slot_index);
        mcfg_refresh_set_state(slot_index, MCFG_REFRESH_STATE_UPDATING_SUBS);
        mcfg_refresh_clients(type, slot_index);
      }
    }
    sub_index ++;
  } while(sub_index <= MCFG_REFRESH_INDEX_MAX);

  return TRUE;
}

boolean mcfg_refresh_cmd_proc_mmoc_tear_down_ack
(
  mcfg_refresh_index_type sub_index,
  cm_ph_cmd_err_e_type    ph_cmd_err
)
{
  mcfg_refresh_index_type slot_index;
  sys_oprt_mode_e_type    oprt_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  slot_index = mcfg_refresh_sub_to_slot_index(sub_index);

  MCFG_ASSERT_COND_RET_FALSE(sub_index < MCFG_REFRESH_NUM_OF_SUBS);
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_REFRESH_NUM_OF_SUBS);

  if(ph_cmd_err == CM_PH_CMD_ERR_NOERR)
  {
    mcfg_refresh_set_state(slot_index, 
                           MCFG_REFRESH_STATE_TEAR_DOWN_ACK);
  }
  else if(ph_cmd_err == CM_PH_CMD_ERR_OPRT_MODE_S)
  {
    oprt_mode = cm_ph_get_oprt_mode();
    if(oprt_mode == SYS_OPRT_MODE_OFFLINE ||
       oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA ||
       oprt_mode == SYS_OPRT_MODE_OFFLINE_AMPS )
    {
       MCFG_MSG_ERROR_2("Aborting tear down, for subi %d, oprt_mode = %d", sub_index, oprt_mode);
       mmgsdi_continue_after_nv_refresh(
         mcfg_refresh_slot_index_to_mmgsdi_slot(slot_index), 
         mcfg_refresh_sub_index_to_mmgsdi_context(sub_index),
         mcfg_refresh_cache.slotdata[slot_index].mmgsdi_reference);

       mcfg_refresh_set_state(slot_index, MCFG_REFRESH_STATE_IDLE);
    }
    else
    {
      mcfg_refresh_set_state(slot_index, 
                             MCFG_REFRESH_STATE_TEAR_DOWN_ACK);
    }
  }
  else
  {
    MCFG_MSG_ERROR_2("CMPH tear down returns error %d for subi %d", ph_cmd_err, sub_index);
    mcfg_refresh_set_state(slot_index, MCFG_REFRESH_STATE_TEAR_DOWN_NACK);
  }

  return (ph_cmd_err == CM_PH_CMD_ERR_NOERR);
}

boolean mcfg_refresh_cmd_proc_cm_event
(
  void
)
{
  mcfg_refresh_index_type slot_index;
  mcfg_refresh_index_type sub_index;
  mcfg_refresh_state_e_type state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (slot_index = MCFG_REFRESH_INDEX_0; slot_index <= MCFG_REFRESH_INDEX_MAX; slot_index++)
  {
    state = mcfg_refresh_get_state(slot_index);
    if (state == MCFG_REFRESH_STATE_TEAR_DOWN_NACK)
    {
      mcfg_refresh_resume_start_timer();
      return TRUE;
    }
  }

  MCFG_MSG_MED("Ignore call end event");
  return FALSE;
} /* mcfg_refresh_cmd_proc_cm_event() */

void mcfg_refresh_cmd_proc_timeout
(
  mcfg_refresh_index_type slot
)
{
  mcfg_refresh_client_node_type *refresh_node;
  mcfg_refresh_state_e_type state;
  void *cb = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET(slot < MCFG_REFRESH_NUM_OF_SUBS);
  state = mcfg_refresh_get_state(slot);

  if (state == MCFG_REFRESH_STATE_ONHOLD || state == MCFG_REFRESH_STATE_TEAR_DOWN_NACK 
      || mcfg_refresh_cache.slotdata[slot].b2b)
  {
    MCFG_MSG_LOW_2("resume refresh from state %d for sloti %d", state, slot);
    mcfg_refresh_resume(MCFG_REFRESH_INDEX_MASK_ALL);

    return;
  }

  if (state == MCFG_REFRESH_STATE_IDLE)
  {
    MCFG_MSG_LOW_1("idle state for sloti %d, skip", slot);
    return;
  }

  if (state == MCFG_REFRESH_STATE_UPDATING_SLOT || 
      state == MCFG_REFRESH_STATE_UPDATING_SUBS)
  {
    refresh_node = list_peek_front(&(mcfg_refresh_cache.client_list));
    while (refresh_node != NULL)
    {
      if(refresh_node->client.reference_id[slot] != 0)
      {
        MCFG_MSG_ERROR_3("timeout on cb %p in state %d for sloti %d", refresh_node->client.reg.cb, state, slot);
        cb = refresh_node->client.reg.cb;
      }
      refresh_node = list_peek_next(&(mcfg_refresh_cache.client_list), &(refresh_node->link));
    }
  }

  if (cb)
  {
    MCFG_MSG_ERROR_2("MCFG Refresh timeout on cb %p in state %d, bail out with reset...",
                cb, state);
    mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_NOW);
  }
  else
  {
    MCFG_MSG_ERROR_1("MCFG Refresh timeout, bail out with reset in state %d...", state);
    mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_NOW);
  }
} /* mcfg_refresh_cmd_proc_timeout() */

boolean mcfg_refresh_cmd_proc_cm_ph_event
(
  mcfg_refresh_cm_ph_state_s_type *cmphstate
)
{
  mcfg_refresh_index_type slot_index;
  mcfg_refresh_index_type sub_index;
  mcfg_refresh_state_e_type state;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sub_index = cmphstate->sub_index;
  MCFG_ASSERT_COND_RET_FALSE(sub_index < MCFG_REFRESH_NUM_OF_SUBS);
  
  slot_index = mcfg_refresh_sub_to_slot_index(sub_index);

  MCFG_MSG_HIGH_2("Received cm_ph_state %d for sub %d", cmphstate->ph_state, sub_index);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state = mcfg_refresh_get_state(slot_index);

  if (state == MCFG_REFRESH_STATE_TEAR_DOWN_NACK &&
      cmphstate->ph_state == CM_PH_STATE_NORMAL)
  {
    MCFG_MSG_HIGH("CM ph state changed in STATE_TEAR_DOWN_NACK, send tear down again");
    mcfg_refresh_tear_down(sub_index);
    return TRUE;
  }

  MCFG_MSG_MED("Ignore cm ph event");
  return FALSE;
} /* mcfg_refresh_cmd_proc_cm_ph_event() */

/*===========================================================================

  FUNCTION mcfg_refresh_cmd_proc

  DESCRIPTION
    process refresh command

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_refresh_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
)
{
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET(cmd);
  MCFG_CHECK_NULL_PTR_RET(cmd->data);

  refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
  switch (refresh_cmd_data->cmd_id)
  {
    case MCFG_REFRESH_CMD_REGISTER:
      mcfg_refresh_cmd_proc_register(&(refresh_cmd_data->data.register_req));
      break;

    case MCFG_REFRESH_CMD_DONE:
      mcfg_refresh_cmd_proc_done(&refresh_cmd_data->data.info,
                                 refresh_cmd_data->status);
      break;

    case MCFG_REFRESH_CMD_UIMDATA:
      mcfg_refresh_cmd_proc_uimdata(&refresh_cmd_data->data.uimdata);   
      break;

    case MCFG_REFRESH_CMD_MMCP_DEACT:
      mcfg_refresh_cmd_proc_mmoc_deactivated(refresh_cmd_data->data.submask);  
      break; 

    case MCFG_REFRESH_CMD_MMCP_TEAR_DOWN_ACK:
      mcfg_refresh_cmd_proc_mmoc_tear_down_ack(
        refresh_cmd_data->data.mmcpdata.sub_index,
        refresh_cmd_data->data.mmcpdata.ph_cmd_err);   
      break;

    case MCFG_REFRESH_CMD_CM_CALL_END:
      mcfg_refresh_cmd_proc_cm_event();
      break;

    case MCFG_REFRESH_CMD_TIMEOUT:
      mcfg_refresh_cmd_proc_timeout(refresh_cmd_data->data.slot_index);
      break;

    case MCFG_REFRESH_CMD_CM_PH_STATE:
      mcfg_refresh_cmd_proc_cm_ph_event(&refresh_cmd_data->data.cmphstate);
      break;

    default:
      MCFG_MSG_ERROR_1("Unexpected REFRESH cmd_id %d", refresh_cmd_data->cmd_id);
  }
} /* mcfg_refresh_cmd_proc() */

/*===========================================================================

  FUNCTION mcfg_refresh_get_orig

  DESCRIPTION
    Get current refresh origination

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_refresh_orig_e_type mcfg_refresh_get_orig
(
  mcfg_refresh_index_type slot_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(slot_index < MCFG_REFRESH_NUM_OF_SLOTS, 
                             MCFG_REFRESH_ORIG_INVALID);
  return mcfg_refresh_cache.slotdata[slot_index].orig;
}

/*===========================================================================

  FUNCTION mcfg_refresh_set_orig

  DESCRIPTION
    Set current refresh origination

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_set_orig
(
  mcfg_refresh_index_type  slot_index, 
  mcfg_refresh_orig_e_type orig
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_REFRESH_NUM_OF_SLOTS)
  switch (orig)
  {
    case MCFG_REFRESH_ORIG_UIM:
      MCFG_MSG_HIGH_1("Originated by uim hot swap for sloti %d", slot_index);
      break;

    case MCFG_REFRESH_ORIG_PDC:
      MCFG_MSG_HIGH_1("Originated by MCFG PDC activation for sloti %d", slot_index);
      break;

    case MCFG_REFRESH_ORIG_PDC_UPDATE_ONLY:
      MCFG_MSG_HIGH_1("Originated by MCFG PDC activation for MBN update only for sloti %d", slot_index);
      break;

    case MCFG_REFRESH_ORIG_APP:
      MCFG_MSG_HIGH_1("Forced by APP centric refresh for sloti %d", slot_index);
      break;

    case MCFG_REFRESH_ORIG_NOCHANGE:
      MCFG_MSG_HIGH_1("Originated by uim hot swap without any change for sloti %d", slot_index);
      break;

    case MCFG_REFRESH_ORIG_FLEX:
      MCFG_MSG_HIGH_1("Originated by uim flex mapping for sloti %d", slot_index);
      break;

    default:
      MCFG_MSG_ERROR_2("Invalid Orig %d for sloti %d", orig, slot_index);
      return FALSE;
  }

  mcfg_refresh_cache.slotdata[slot_index].orig = orig;
  return TRUE;
}

/*===========================================================================

  FUNCTION mcfg_refresh_get_state

  DESCRIPTION
    Get current refresh state

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_refresh_state_e_type mcfg_refresh_get_state
(
  mcfg_refresh_index_type slot_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_ERROR(slot_index < MCFG_REFRESH_NUM_OF_SLOTS, 
                             MCFG_REFRESH_STATE_INVALID);
  return mcfg_refresh_cache.slotdata[slot_index].state;
}

/*===========================================================================

  FUNCTION mcfg_refresh_set_state

  DESCRIPTION
    Update refresh state

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_set_state
(
  mcfg_refresh_index_type   slot_index, 
  mcfg_refresh_state_e_type state
)
{
  mcfg_refresh_state_e_type prev_state;
  uint8 timer;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_REFRESH_NUM_OF_SLOTS);

  if (mcfg_refresh_cache.slotdata[slot_index].b2b &&
     (mcfg_refresh_cache.slotdata[slot_index].b2b->type == MCFG_REFRESH_UIM_ICCID) )
  {
    if (state == MCFG_REFRESH_STATE_UPDATING_SLOT || 
        state == MCFG_REFRESH_STATE_WAIT_FOR_SESSION)
    {
      MCFG_MSG_HIGH_3("Aborting from state %d to idle, and requeue uim iccid update ref %lx sloti %x", 
                      state, mcfg_refresh_cache.slotdata[slot_index].b2b->reference_id, slot_index);
      mcfg_refresh_cache.slotdata[slot_index].cb_count = 0;

      mcfg_refresh_update_from_uim(mcfg_refresh_cache.slotdata[slot_index].b2b);
      mcfg_free(mcfg_refresh_cache.slotdata[slot_index].b2b);
      mcfg_refresh_abort(slot_index);
      state = MCFG_REFRESH_STATE_IDLE;
    }
  }

  prev_state = mcfg_refresh_cache.slotdata[slot_index].state;
  if (state == prev_state && state != MCFG_REFRESH_STATE_IDLE)
  {
    MCFG_MSG_HIGH_2("State remains the same %d, sloti = %d", state, slot_index);
    return TRUE;
  }

  mcfg_refresh_cache.slotdata[slot_index].state = state;
  switch (state)
  {
    case MCFG_REFRESH_STATE_IDLE:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed back to >>> IDLE", prev_state, slot_index);
      MCFG_MSG_HIGH_1("Stopping refresh Timer for sloti %d", slot_index);
      timer_clr(&mcfg_refresh_timer[slot_index], T_SEC);

      if (!mcfg_refresh_cache.slotdata[slot_index].b2b)
      {
        memset(&mcfg_refresh_cache.slotdata[slot_index], 0, sizeof(mcfg_refresh_cache_slotdata_s_type)); // reset cache
      }
      else if (prev_state == MCFG_REFRESH_STATE_UPDATING_SUBS)
      {
        MCFG_MSG_HIGH_2("requeue uim iccid update ref %lx sloti %d", mcfg_refresh_cache.slotdata[slot_index].b2b->reference_id, slot_index);
        mcfg_refresh_update_from_uim(mcfg_refresh_cache.slotdata[slot_index].b2b);
        mcfg_free(mcfg_refresh_cache.slotdata[slot_index].b2b);
        memset(&mcfg_refresh_cache.slotdata[slot_index], 0, sizeof(mcfg_refresh_cache_slotdata_s_type)); // reset cache
      }
      mcfg_refresh_resume_start_timer();
      break;

    case MCFG_REFRESH_STATE_ACTIVATING_SLOT:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> ACTIVATING SLOT", prev_state, slot_index);
      break;

    case MCFG_REFRESH_STATE_UPDATING_SLOT:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> UPDATING SLOT", prev_state, slot_index);
      timer = mcfg_refresh_get_timer(slot_index);
      if (timer > 0)
      {
        MCFG_MSG_HIGH_2("Starting refresh Timer for sloti %d, %ds count down", slot_index, timer);
        timer_set(&mcfg_refresh_timer[slot_index], timer, 0, T_SEC);
      }
      break;

    case MCFG_REFRESH_STATE_WAIT_FOR_SESSION:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> WAIT_FOR_SESSION", prev_state, slot_index);
      MCFG_MSG_HIGH_1("Stopping refresh Timer for sloti %d", slot_index);
      timer_clr(&mcfg_refresh_timer[slot_index], T_SEC);
      break;

    case MCFG_REFRESH_STATE_ONHOLD:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> ONHOLD", prev_state, slot_index);
      break;

    case MCFG_REFRESH_STATE_WAIT_CM_INIT:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> WAIT_CM_INIT", prev_state, slot_index);
      break;

    case MCFG_REFRESH_STATE_TEAR_DOWN_REQ:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> TEAR_DOWN_REQ", prev_state, slot_index);
      timer = 90; // Need to be greater than the 72s CM timer for stop_req
      MCFG_MSG_HIGH_2("Starting refresh Timer for sloti %d, %ds count down", slot_index, timer);
      timer_set(&mcfg_refresh_timer[slot_index], timer, 0, T_SEC);
      break;

    case MCFG_REFRESH_STATE_TEAR_DOWN_ACK:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> TEAR_DOWN_ACK", prev_state, slot_index);
      break;

    case MCFG_REFRESH_STATE_TEAR_DOWN_NACK:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> TEAR_DOWN_NACK", prev_state, slot_index);
      MCFG_MSG_HIGH_1("Stopping refresh Timer for sloti %d", slot_index);
      timer_clr(&mcfg_refresh_timer[slot_index], T_SEC);
      break;

    case MCFG_REFRESH_STATE_ACTIVATING_SUBS:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> ACTIVATING_SUBS", prev_state, slot_index);
      MCFG_MSG_HIGH_1("Stopping refresh Timer for sloti %d", slot_index);
      timer_clr(&mcfg_refresh_timer[slot_index], T_SEC);
      break;

    case MCFG_REFRESH_STATE_UPDATING_SUBS:
      MCFG_MSG_HIGH_2("State %d of sloti %d changed to >>> UPDATING_SUBS", prev_state, slot_index);
      timer = mcfg_refresh_get_timer(slot_index);
      if (timer > 0)
      {
        MCFG_MSG_HIGH_2("Starting refresh Timer for sloti %d, %ds count down", slot_index, timer);
        timer_set(&mcfg_refresh_timer[slot_index], timer, 0, T_SEC);
      }
      break;

    default:
      MCFG_MSG_ERROR_2("Invalid State %d of sloti %d ", state, slot_index);
      return FALSE;
  }

  return TRUE;
}

/*===========================================================================

  FUNCTION mcfg_refresh_store_id

  DESCRIPTION
    Save config ID for refreshing and PDC config changeind . 

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_refresh_store_id
(
  mcfg_refresh_index_type slot_index, 
  mcfg_config_id_s_type  *config_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_ASSERT_COND_RET_FALSE(slot_index < MCFG_REFRESH_NUM_OF_SLOTS);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_id);

  memscpy(&mcfg_refresh_cache.slotdata[slot_index].config_id,
          sizeof(mcfg_config_id_s_type),
          config_id,
          sizeof(mcfg_config_id_s_type));
  return TRUE;
}

/*===========================================================================

  FUNCTION mcfg_refresh_timer_cb

  DESCRIPTION
    Refresh Timer callback. 

  DEPENDENCIES
    None

  PARAMETERS
    data    [in]  timer callback data

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_refresh_timer_cb(timer_cb_data_type data)
{
  cm_client_status_e_type status;
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_TIMEOUT;

    MCFG_MSG_MED_1("refresh_timer_cb data %d", data);
    refresh_cmd_data->data.slot_index = (mcfg_refresh_index_type)data;

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for refresh_timer_cb!");
      mcfg_task_cmd_free(cmd);
    }
  }
  
  return;
} /* mcfg_refresh_timer_cb() */

/*===========================================================================

  FUNCTION  mcfg_refresh_cm_init

  DESCRIPTION
    This function is used to get cm client id and register CM event cb

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_refresh_cm_init
(
  void
)
{
  mcfg_refresh_index_type i;
  cm_client_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = cm_client_init( CM_CLIENT_TYPE_MCFG, &mcfg_refresh_cache.mcfg_cm_client_id );
  if (status != CM_CLIENT_OK)
  {
    MCFG_MSG_ERROR_1("MCFG unable to register as CM client:%d", (int)status);
    mcfg_refresh_cache.mcfg_cm_client_id = CM_CLIENT_ID_ANONYMOUS;
    return;
  }

  status = cm_mm_client_call_reg(mcfg_refresh_cache.mcfg_cm_client_id,
                      mcfg_refresh_cm_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_END,
                      CM_CALL_EVENT_END,
                      NULL);
  if (status != CM_CLIENT_OK)
  {
    MCFG_MSG_ERROR("MCFG unable to register for CM Call end event");
    return;
  }

  status = cm_client_ph_reg(mcfg_refresh_cache.mcfg_cm_client_id,
                      mcfg_refresh_cm_ph_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_PH_EVENT_SYS_SEL_PREF,
                      CM_PH_EVENT_SYS_SEL_PREF,
                      NULL);
  if (status != CM_CLIENT_OK)
  {
    MCFG_MSG_ERROR("MCFG unable to register for CM PH event");
    return;
  }

  for (i = MCFG_REFRESH_INDEX_MIN; i <= MCFG_REFRESH_INDEX_MAX; i++ )
  {
    if(mcfg_refresh_get_state(i) == MCFG_REFRESH_STATE_WAIT_CM_INIT)
    {
      MCFG_MSG_MED_1("state = WAIT_CM_INIT, tear_down for index i %d",i);
      mcfg_refresh_tear_down(mcfg_refresh_slot_to_sub_index(i));
    }
  }
} /* mcfg_refresh_cm_init() */

/*===========================================================================

  FUNCTION mcfg_refresh_init

  DESCRIPTION
    Refresh Initialization. 

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_refresh_init
(
  void
)
{
  mcfg_refresh_index_type i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&mcfg_refresh_cache, 0, sizeof(mcfg_refresh_cache_s_type));

  mcfg_refresh_cache.mcfg_cm_client_id = CM_CLIENT_ID_ANONYMOUS;

  for (i = MCFG_REFRESH_INDEX_0; i < MCFG_NUM_OF_UIM_SLOT; i++)
  {
  /* define timer for timeout */
    timer_def(&mcfg_refresh_timer[i],
            &mcfg_refresh_timer_group,
            NULL,
            0,
            mcfg_refresh_timer_cb,
              i);
  }
} /* mcfg_refresh_init() */

/*===========================================================================

  FUNCTION mcfg_notify_deactivate_success

  DESCRIPTION
    Process deactivation notify call from mmoc

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_notify_deactivate_success
(
  mcfg_refresh_index_mask_type sub_mask
)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_refresh_cmd_data_s_type *refresh_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd = mcfg_task_cmd_alloc(
    MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
  if (cmd != NULL)
  {
    refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
    refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_MMCP_DEACT;
    refresh_cmd_data->data.submask = sub_mask;

    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command for deactivation success!");
      mcfg_task_cmd_free(cmd);
      return FALSE;
    }
  }

  return TRUE;
}
#endif

