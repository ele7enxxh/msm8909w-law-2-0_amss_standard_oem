/*===========================================================================
                     D S _ D S D _ T I M E R . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_timer.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/06/13    rs     Changed strlcpy to proper format
08/22/12    ss     Removed call to efs create file as that is moved to 
                   dsutil.
06/07/12    var    Added NV items for DSD timers
11/11/11    sk     Created Module

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "ds3gcfgmgr.h"
#include "ds3gsubsmgr.h"
#include "ds_dsd_event_defs.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_timer.h"
#include "ds_dsd_1xSm.h"
#include "ds_dsd_hdrSm.h"
#include "ds_dsd_lteSm.h"
#include "ds_dsd_wlanSm.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_ext.h"
#include "data_msg.h"
#include "dsutil.h"
#include "dstask_v.h"
#include <stringl/stringl.h>
#include "modem_mem.h"
#include "assert.h"
#include "event.h"
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define MAX_TIMER_PATH_SIZE 60

/*---------------------------------------------------------------------------
  Macro to get timer id for subscription id
    timer_id per subs_id = (subs_id * 100)+timer_id
---------------------------------------------------------------------------*/
#define TIMER_ID_FOR_SUBS_ID(subs_id, timer_id) \
          ((unsigned long)(subs_id*100) + timer_id)

/*---------------------------------------------------------------------------
  Macro to get timer id from subscription timer id
    timer_id = timer_id per subs_id - (subs_id * 100)
---------------------------------------------------------------------------*/
#define TIMER_ID_FROM_SUBS_TIMER_ID(subs_id, timer_id) \
          (timer_id - (unsigned long)(subs_id*100))

/*---------------------------------------------------------------------------
  Macro to get subscription id from timer id for 
    subs_id = (timer_id / 100)
---------------------------------------------------------------------------*/
#define SUBS_ID_FROM_TIMER_ID(timer_id) (timer_id/100)

/*---------------------------------------------------------------------------
  EFS path for DSD timer NV items
---------------------------------------------------------------------------*/
#define DS_1X_ACQHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_1x_acq_hyst_timer"
#define DS_1X_OOSHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_1x_oos_hyst_timer"
#define DS_HDRACQHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_hdr_acq_hyst_timer"
#define DS_HDROOSHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_hdr_oos_hyst_timer"
#define DS_LTEACQHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_lte_acq_hyst_timer"
#define DS_LTEOOSHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_lte_oos_hyst_timer"
#define DS_WLANACQHYST_TIMER_1_PATH "/nv/item_files/data/dsd/data_wlan_acq_hyst_timer"
#define DS_EDCT_TIMER_1_PATH       "/nv/item_files/data/3gpp2/epc_data_context_duration"


/*---------------------------------------------------------------------------
  Default DSD timer values
---------------------------------------------------------------------------*/
#define DS_1X_ACQ_HYST_VAL         15  /* in sec */
#define DS_1X_OOS_HYST_VAL         0   /* in sec */
#define DS_HDR_ACQ_HYST_VAL        15  /* in sec */
#define DS_HDR_OOS_HYST_VAL        22  /* in sec */
#define DS_LTE_ACQ_HYST_VAL        0   /* in sec */
#define DS_LTE_OOS_HYST_VAL        22  /* in sec */
#define DS_EPC_CTX_DURATION        180 /* in sec */
#define DS_HDR_HANDDOWN_VAL        180 /* in sec */
#define DS_HDR_HANDUP_AVOID_VAL    5   /* in sec */
#define DS_WLAN_ACQ_HYST_VAL       5   /* in sec */
#define DS_HDR_NW_DETACH_HYST_VAL  DS_HDR_OOS_HYST_VAL /* in sec */
#define DS_LTE_NW_DETACH_HYST_VAL  DS_LTE_OOS_HYST_VAL /* in sec */


/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/

/*--------------------------------------------------------------------------
  Structure used to read the timer NV items from the item file in EFS
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  uint16 dsd_timer_val;
} dsd_timer_nv_from_efs;

/*--------------------------------------------------------------------------
  Structure used to read the EFS item from the item file in EFS
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  uint8 epc_data_ctx_duration;
} epc_ctx_nv_from_efs;

static timer_group_type dsd_timer_group_non_deferrable;
static timer_group_type dsd_timer_group_deferrable;

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_TIMER_GET_TIMER_INFO_PTR

DESCRIPTION   Utility to get the dsd timer pointer for specified radio and 
              subs_id. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds_dsd_timer_info_type* ds_dsd_timer_get_timer_info_ptr
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_timer_info_type   *dsd_timer_info = NULL;
  ds_dsd_timer_list_type   *dsd_timer_list = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dsd_timer_list = ds_dsd_proc_get_timer_list(subs_id);
  if(dsd_timer_list == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "DSD TIMER id passed is NULL");
    return NULL;
  }

  switch(dsd_timer_id)
  {
    case DS_1xAcqHystTimerID_T1:
      dsd_timer_info = &(dsd_timer_list->cdma_acq_hyst_timer);
      break;

    case DS_1xOOSHystTimerID_T2:
      dsd_timer_info = &(dsd_timer_list->cdma_oos_hyst_timer);
      break;

    case DS_HDRAcqHystTimerID_T3:
      dsd_timer_info = &(dsd_timer_list->hdr_acq_hyst_timer);
      break;

    case DS_HDROOSHystTimerID_T4:
      dsd_timer_info = &(dsd_timer_list->hdr_oos_hyst_timer);
      break;

    case DS_LTEAcqHystTimerID_T5:
      dsd_timer_info = &(dsd_timer_list->lte_acq_hyst_timer);
      break;

    case DS_LTEOOSHystTimerID_T6:
      dsd_timer_info = &(dsd_timer_list->lte_oos_hyst_timer);
      break;

    case DS_WLANAcqHystTimerID_T7:
      dsd_timer_info = &(dsd_timer_list->wlan_acq_hyst_timer);
      break;

    case DS_EPCDataCntxTimerID_T8:
      dsd_timer_info = &(dsd_timer_list->epc_data_cntx_timer);
      break;

    case DS_HDRHandDownTimerID_T9:
      dsd_timer_info = &(dsd_timer_list->hdr_handdown_timer);
      break;

    case DS_HDRHandUpAvoidTimerID_T10:
      dsd_timer_info = &(dsd_timer_list->hdr_handup_avoid_timer);
      break;

    case DS_HDRNwDetachHystTimerID_T11:
      dsd_timer_info = &(dsd_timer_list->hdr_nw_detach_hyst_timer);
      break;

    case DS_LTENwDetachHystTimerID_T12:
      dsd_timer_info = &(dsd_timer_list->lte_nw_detach_hyst_timer);
      break;

    default:
      break;
  }
  return dsd_timer_info;
}/* ds_dsd_timer_get_timer_from_id() */

/*===========================================================================
FUNCTION       DS_DSD_TIMER_GET_EDCT_NV_VALUE

DESCRIPTION    Utility to get the timer value for EDCT timer from NV item. 
               It cannot combined with ds_dsd_timer_get_nv_value()
               since EDCT timer defined as uint8.

DEPENDENCIES  None

RETURN VALUE  TRUE:  timer value present in NV item
              FALSE: timer value absent in NV item

SIDE EFFECTS
===========================================================================*/
boolean ds_dsd_timer_get_edct_nv_value
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  dword                      *edct_timer_val
)
{
  nv_stat_enum_type                     read_efs_nv_retval;
  epc_ctx_nv_from_efs                   edct_timer_efs_struct;
  boolean                               is_nv_present = FALSE;
  dword                timer_val = 0;
  char                                  item_file_path[MAX_TIMER_PATH_SIZE];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return FALSE;
  }


  strlcpy(item_file_path, DS_EDCT_TIMER_1_PATH, sizeof(item_file_path));

  /* Read the NV item into timer value */
  read_efs_nv_retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
                                                 &edct_timer_efs_struct,
                                                 sizeof(edct_timer_efs_struct),
                                                 ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id));

  if (NV_DONE_S == read_efs_nv_retval)
  {
    timer_val = (dword)edct_timer_efs_struct.epc_data_ctx_duration;
    is_nv_present = TRUE;
  }
  else
  {
    timer_val = 0;
    is_nv_present = FALSE;
  }

  DATA_MSG1(MSG_LEGACY_HIGH,
            "ds_dsd_timer_get_edct_nv_value: timer_val=%d",
            timer_val);
  *edct_timer_val = timer_val;

  return is_nv_present;
}/* ds_dsd_timer_get_edct_nv_value() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_GET_NV_VALUE

DESCRIPTION   Utility to get the timer value from NV item

DEPENDENCIES  None.

RETURN VALUE  TRUE:  timer value present in NV item
              FALSE: timer value absent in NV item

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_timer_get_nv_value
(
  ds3gsubsmgr_subs_id_e_type  subs_id,
  uint8                       dsd_timer_id,
  dword                      *dsd_timer_val
)
{
  nv_stat_enum_type           read_efs_nv_retval;
  dsd_timer_nv_from_efs       dsd_timer_efs_struct;
  boolean                     is_timer_valid = FALSE;
  boolean                     is_nv_present = FALSE;
  dword                  timer_val=0;
  char                        item_file_path[MAX_TIMER_PATH_SIZE];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------
    Copy the NV item file path to item_file_path
  -------------------------------------------------------------------*/

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return FALSE;
  }

  switch(dsd_timer_id)
  {
    /* 1x acquisition hysteresis timer */
    case DS_1xAcqHystTimerID_T1:
      strlcpy(item_file_path,
              DS_1X_ACQHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* 1x OOS hysteresis timer */
    case DS_1xOOSHystTimerID_T2:
      strlcpy(item_file_path,
              DS_1X_OOSHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* HDR acquisition hysteresis timer */
    case DS_HDRAcqHystTimerID_T3:
      strlcpy(item_file_path,
              DS_HDRACQHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* HDR OOS hysteresis timer */
    case DS_HDROOSHystTimerID_T4:
      strlcpy(item_file_path,
              DS_HDROOSHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* LTE acquisition hysteresis timer */
    case DS_LTEAcqHystTimerID_T5:
      strlcpy(item_file_path,
              DS_LTEACQHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* LTE OOS hysteresis timer */
    case DS_LTEOOSHystTimerID_T6:
      strlcpy(item_file_path,
              DS_LTEOOSHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* WLAN acquisition hysteresis timer */
    case DS_WLANAcqHystTimerID_T7:
      strlcpy(item_file_path,
              DS_WLANACQHYST_TIMER_1_PATH,
              sizeof(item_file_path));
      is_timer_valid = TRUE;
      break;

    /* NV items not defined for other DSD timers */
    default:
      is_timer_valid = FALSE;
      break;
  }

  if(!is_timer_valid)
  {
    return FALSE;
  }

  /* Read the NV item into timer value */
  read_efs_nv_retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
                                                 &dsd_timer_efs_struct,
                                                 sizeof(dsd_timer_efs_struct),
                                                 ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id));
  if (NV_DONE_S == read_efs_nv_retval)
  {
    timer_val = (dword)dsd_timer_efs_struct.dsd_timer_val;
    is_nv_present = TRUE;
  }
  else
  {
    timer_val = 0;
    is_nv_present = FALSE;
  }

  DATA_MSG3(MSG_LEGACY_HIGH, "ds_dsd_timer_get_nv_value: timer_id=%d,"
              "timer_val=%d: ,is NV item present: %d",
              dsd_timer_id,timer_val,is_nv_present);


  *dsd_timer_val = timer_val;

  return is_nv_present;
}/*ds_dsd_timer_get_nv_value()*/

/*===========================================================================
FUNCTION       DS_DSD_TIMER_CBACK

DESCRIPTION    Function registered when the DSD timer expires

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_timer_cback
(
  unsigned long parm
)
{
  ds_cmd_type                 *cmd_ptr = NULL;
  ds_dsd_timer_cmd_info_type  *data_block_ptr = NULL;
  unsigned long                timer_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(parm == DS_InvalidTimerID_T0)
  {
    return;
  }

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_timer_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  data_block_ptr = (ds_dsd_timer_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  /* Copy the timer id to data block pointer */
  memscpy(&timer_id,
          sizeof(unsigned long),
          &parm,
          sizeof(unsigned long));

  /* Get the subscription id from timer_id */
  data_block_ptr->subs_id = SUBS_ID_FROM_TIMER_ID(timer_id);

  data_block_ptr->timer_id = TIMER_ID_FROM_SUBS_TIMER_ID(
                               data_block_ptr->subs_id,
                               timer_id);

  DATA_MSG2(MSG_LEGACY_HIGH, "DS DSD TIMER CB TIMER:%d subs_id:%d",
            data_block_ptr->timer_id, data_block_ptr->subs_id);

  /*----------------------------------------------------------------------
  This is a callback function called when the timer expires. Puts command
  in ds task internal command queue.
  ----------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_TIMER_EXPIRED;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE,
                     data_block_ptr->subs_id,
                     cmd_ptr);

  return;
}/* ds_dsd_timer_cback() */

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_TIMER_RUNNING

DESCRIPTION   Utility to find if timer is running.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Timer running 
              FALSE:    Timer not running or Timer id not valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_timer_running
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                  ret_val = FALSE;
  ds_dsd_timer_info_type  *dsd_timer_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(dsd_timer_id, subs_id);
  if(dsd_timer_info == NULL)
  {
    return FALSE;
  }

  if( timer_get(&(dsd_timer_info->timer), T_MSEC) )
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }

  DATA_MSG3(MSG_LEGACY_LOW,"Timer %d for subs_id:%d ,is timer running: %d",
            dsd_timer_id, subs_id,ret_val);

  return ret_val;
}/*ds_dsd_timer_running()*/

/*===========================================================================
FUNCTION      DS_DSD_TIMER_START_TIMER

DESCRIPTION   Starts the DSD timer for specified duration.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Timer started or Timer already running.
              FALSE:    Timer id not valid.

SIDE EFFECTS  Starts Timer.
===========================================================================*/
boolean ds_dsd_timer_start_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                  ret_val = FALSE;
  uint32                   duration = 0;
  ds_dsd_timer_info_type  *dsd_timer_info = NULL;
  ds_dsd_proc_type        *dsd_proc_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get DSD proc for subs id */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);
  dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(dsd_timer_id, subs_id);

  if(dsd_proc_ptr == NULL || dsd_timer_info == NULL )
  {
    DATA_MSG3(MSG_LEGACY_ERROR, "DSD proc ptr: %d,dsd timer info ptr: %d "
                                "for subs_id:%d",dsd_proc_ptr,
                                 dsd_timer_info, subs_id);
    return FALSE;
  }

  /* Special handling if LTE OOS hysteresis timer */
  duration = (uint32)dsd_timer_info->value*1000;
  switch(dsd_timer_id)
  {
    case DS_LTEOOSHystTimerID_T6:
      duration += (dsd_proc_ptr->timer_list.epc_data_cntx_timer.value *
                   dsd_proc_ptr->epc_present_flag)*1000;
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    /*---------------------------------------------------------------------
      If WLAN acq WQE supported on Apps side, set Acq Hyst timer to be 0.
    ---------------------------------------------------------------------*/
    case DS_WLANAcqHystTimerID_T7:
      if(dsd_proc_ptr->radio_sm[DS_DSD_RADIO_SM_WLAN]->radio_info
         .acq_wqe_support)
      {
        duration = 0;
      }
      break;
#endif /* FEATURE_DATA_WLAN_MAPCON */

    default:
      break;
  }

  if( !ds_dsd_timer_running(dsd_timer_id, subs_id) )
  {
    if(TE_SUCCESS == timer_set(&(dsd_timer_info->timer),
                               (timetick_type)duration,
                               0,
                               T_MSEC) )
  {
    ret_val = TRUE;
  }
  }

  if(ret_val == TRUE)
  {
    DATA_MSG3(MSG_LEGACY_HIGH,
              "Timer start. Timer ID: %d Duration: %d sec subs_id:%d", 
              dsd_timer_id, duration, subs_id);

    ds_dsd_timer_event_gen (DS_DSD_TIMER_STARTED_EVENT,
                            dsd_timer_id,
                            subs_id,
                            dsd_timer_info->value);
  }
  else
  {
    DATA_MSG2(MSG_LEGACY_HIGH,
              "Timer ID %d for subs_id:%d already running or invalid", 
              dsd_timer_id, subs_id);
  }
  return ret_val;
} /* ds_dsd_timer_start_timer() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_STOP_TIMER

DESCRIPTION   Stop the dsd Timer.

DEPENDENCIES  None.

RETURN VALUE  TRUE: Timer stopped or Timer already stopped.
              FALSE: Timer ID invalid.

SIDE EFFECTS  Stops timer.
===========================================================================*/
boolean ds_dsd_timer_stop_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  boolean                  ret_val= FALSE;
  ds_dsd_timer_info_type  *dsd_timer_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(dsd_timer_id, subs_id);
  if(dsd_timer_info == NULL)
  {
    return FALSE;
  }

  if( ds_dsd_timer_running(dsd_timer_id, subs_id) )
  {
    timer_clr( &(dsd_timer_info->timer), T_MSEC);
    ret_val = TRUE;
  }

  if(ret_val == TRUE)
  {

    ds_dsd_timer_event_gen(DS_DSD_TIMER_STOPPED_EVENT,
                          dsd_timer_id,
                          subs_id,
                          dsd_timer_info->value);
  }
  else
  {
    DATA_MSG2(MSG_LEGACY_HIGH,
              "Timer ID:%d for subs_id:%d not running or invalid",
              dsd_timer_id, subs_id);
  }

  return ret_val;
} /* ds_dsd_stop_timer() */

/*===========================================================================
FUNCTION      DS_DSD_RESET_TIMER

DESCRIPTION   Reset the DSD timer. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Set the timer to 0ms. Timer expires immediately
===========================================================================*/
void ds_dsd_reset_timer
(
  uint8                       dsd_timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_timer_info_type   *dsd_timer_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_dsd_timer_running(dsd_timer_id, subs_id) == FALSE)
  {
    return;
  }

  dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(dsd_timer_id, subs_id);
  if(dsd_timer_info == NULL)
  {
    return;
  }

  if( TE_SUCCESS == timer_set( &(dsd_timer_info->timer), 0, 0, T_MSEC) )
  {
    DATA_MSG1(MSG_LEGACY_HIGH,"Timer Reset EVENT. Timer ID: %d", dsd_timer_id);
  
    ds_dsd_timer_event_gen (DS_DSD_TIMER_STARTED_EVENT,
                            dsd_timer_id,
                            subs_id,
                            dsd_timer_info->value);
  }
  return;
}/* ds_dsd_reset_timer() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_RESET

DESCRIPTION   Stop all the DSD timers

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_reset
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8 dsd_timer_id=0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for( dsd_timer_id = DS_1xAcqHystTimerID_T1;
       dsd_timer_id <= DS_MaxTimerID;
       dsd_timer_id++)
  {
    if(ds_dsd_timer_running(dsd_timer_id, subs_id) == TRUE)
    {
      ds_dsd_timer_stop_timer(dsd_timer_id, subs_id);
    }
  }
  return;
}/* ds_dsd_timer_reset() */

/*===========================================================================
FUNCTION      DS_DSD_GET_OOS_TIMER_ID

DESCRIPTION   Utility function to get OOS timer id from dsd radio 

DEPENDENCIES  None.

RETURN VALUE  Timer id.

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds_dsd_get_oos_timer_id
(
  dsd_radio_type              dsd_radio
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(dsd_radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTEOOSHystTimerID_T6;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDROOSHystTimerID_T4;
      break;

    case DS_DSD_RADIO_1X:
      timer_id = DS_1xOOSHystTimerID_T2;
      break;

    default:
      break;
  }

  DATA_MSG2(MSG_LEGACY_HIGH,
            "ds_dsd_get_oos_timer_id. dsd_radio:%d timer_id:%d",
            dsd_radio, timer_id);

  return timer_id;
}/* ds_dsd_get_oos_timer_id() */

/*===========================================================================
FUNCTION      DS_DSD_GET_DETACH_TIMER_ID

DESCRIPTION   Utility function to get detach timer id from dsd radio 

DEPENDENCIES  None.

RETURN VALUE  Timer id.

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds_dsd_get_detach_timer_id
(
  dsd_radio_type dsd_radio
)
{
  uint8 timer_id = DS_InvalidTimerID_T0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(dsd_radio)
  {
    case DS_DSD_RADIO_LTE:
      timer_id = DS_LTENwDetachHystTimerID_T12;
      break;

    case DS_DSD_RADIO_HDR:
      timer_id = DS_HDRNwDetachHystTimerID_T11;
      break;

    default:
      break;
  }

  DATA_MSG2(MSG_LEGACY_HIGH,
            "ds_dsd_get_detach_timer_id. dsd_radio:%d timer_id:%d",
            dsd_radio, timer_id);

  return timer_id;
}/* ds_dsd_get_detach_timer_id() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_CMD_PROCESSING

DESCRIPTION   This is timer callback command processing function. Depending on the
              the timer id, the appropriate state machine's handler is
              invoked for processing the timer callback.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_cmd_processing
(
  ds_cmd_type  *cmd_ptr
)
{
  boolean                     ret_val = FALSE;
  uint8                       dsd_timer_id=0;
  ds_dsd_timer_cmd_info_type *dsd_timer_cmd_info_ptr = NULL;
  ds_dsd_timer_info_type      *dsd_timer_info = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL dsd_timer_id_container.");
    return;
  }

  dsd_timer_cmd_info_ptr=(ds_dsd_timer_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  dsd_timer_id = (uint8)dsd_timer_cmd_info_ptr->timer_id;
  subs_id = dsd_timer_cmd_info_ptr->subs_id;
  dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(dsd_timer_id, subs_id);

  if(dsd_timer_info == NULL)
  {
    return;
  }

  switch((unsigned long)dsd_timer_id)
  {
    case DS_1xAcqHystTimerID_T1:
    case DS_1xOOSHystTimerID_T2:
    case DS_HDRNwDetachHystTimerID_T11:
    case DS_LTENwDetachHystTimerID_T12:
      ds_dsd_1xSm_timer_cb((unsigned long)dsd_timer_id, subs_id);
      ret_val = TRUE;
      break;

    case DS_HDRAcqHystTimerID_T3:
    case DS_HDROOSHystTimerID_T4:
    case DS_HDRHandDownTimerID_T9:
    case DS_HDRHandUpAvoidTimerID_T10:
      ds_dsd_hdrSm_timer_cb((unsigned long)dsd_timer_id, subs_id);
      ret_val = TRUE;
      break;

    case DS_LTEAcqHystTimerID_T5:
      ds_dsd_lteSm_timer_cb((unsigned long)dsd_timer_id, subs_id);
      ret_val = TRUE;
      break;

    case DS_LTEOOSHystTimerID_T6:
      ds_dsd_lteSm_timer_cb((unsigned long)dsd_timer_id, subs_id);
      ret_val = TRUE;
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS_WLANAcqHystTimerID_T7:
      ds_dsd_wlanSm_timer_cb((unsigned long)dsd_timer_id, subs_id);
      ret_val = TRUE;
      break;
#endif /* FEATURE_DATA_WLAN_MAPCON */

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Invalid parameter : 0x%x", dsd_timer_id);
      break;
  }

  if(ret_val == TRUE)
  {
    DATA_MSG1(MSG_LEGACY_HIGH,"Timer EXPIRE EVENT. Timer ID: %d", dsd_timer_id);

    ds_dsd_timer_event_gen(DS_DSD_TIMER_EXPIRED_EVENT,
                           dsd_timer_id,
                           subs_id,
                           dsd_timer_info->value);
  }
  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(subs_id, DS_CMD_DSD_TIMER_EXPIRED);

  return;
} /* ds_dsd_timer_cback_processing() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_INIT_TIMERS

DESCRIPTION   Define timers for specific subscription

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_init_timers
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8                    timer_indx;
  ds_dsd_timer_info_type  *dsd_timer_info = NULL;
  timer_group_type        *timer_grp_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Define DSD timers
  -------------------------------------------------------------------------*/
  for( timer_indx = DS_1xAcqHystTimerID_T1;
       timer_indx <= DS_MaxTimerID;
       timer_indx++ )
  {
    dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(timer_indx, subs_id);

    /* Make WLAN Acq hysteresis timer as non deferrable */
    if( timer_indx == DS_WLANAcqHystTimerID_T7 )
    {
      timer_grp_ptr = &dsd_timer_group_non_deferrable;
    }
    else
    {
      timer_grp_ptr = &dsd_timer_group_deferrable;
    }

    if( TE_SUCCESS != timer_def(&(dsd_timer_info->timer),
                                timer_grp_ptr,
                                NULL,
                                0,
                     ds_dsd_timer_cback,
                                TIMER_ID_FOR_SUBS_ID(subs_id, timer_indx))
      )
    {
      DATA_MSG2(MSG_LEGACY_ERROR, "timer_id:%d for subs_id:%d not initialized",
                timer_indx, subs_id);
    }
  }

  /* Set deferrable/non-deferrable group types */
  timer_group_set_deferrable( &dsd_timer_group_deferrable, TRUE );
  timer_group_set_deferrable( &dsd_timer_group_non_deferrable, FALSE );

  /*-------------------------------------------------------------------------
    Initialize DSD timer values
  -------------------------------------------------------------------------*/
  ds_dsd_timer_init_timer_val_per_subs_id(subs_id);

  return;
} /* ds_dsd_timer_init_timers() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_INIT_TIMER_VAL_PER_SUBS_ID

DESCRIPTION   Initialize timer values for specific subscription by either 
              reading from NV item or setting it to default values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_init_timer_val_per_subs_id
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  uint8                    timer_indx;
  ds_dsd_timer_list_type  *dsd_timer_list = NULL;
  ds_dsd_timer_info_type  *dsd_timer_info = NULL;
  dword                    timer_val=0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    return;
  }

  dsd_timer_list = ds_dsd_proc_get_timer_list(subs_id);
  if(dsd_timer_list == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize DSD timer values to default values
  -------------------------------------------------------------------------*/
  dsd_timer_list->cdma_acq_hyst_timer.value = DS_1X_ACQ_HYST_VAL;
  dsd_timer_list->cdma_oos_hyst_timer.value = DS_1X_OOS_HYST_VAL;
  dsd_timer_list->hdr_acq_hyst_timer.value  = DS_HDR_ACQ_HYST_VAL;
  dsd_timer_list->hdr_oos_hyst_timer.value  = DS_HDR_OOS_HYST_VAL;
  dsd_timer_list->lte_acq_hyst_timer.value  = DS_LTE_ACQ_HYST_VAL;
  dsd_timer_list->lte_oos_hyst_timer.value  = DS_LTE_OOS_HYST_VAL;
  dsd_timer_list->epc_data_cntx_timer.value  = DS_EPC_CTX_DURATION;
  dsd_timer_list->hdr_handdown_timer.value  = DS_HDR_HANDDOWN_VAL;
  dsd_timer_list->hdr_handup_avoid_timer.value = DS_HDR_HANDUP_AVOID_VAL;
  dsd_timer_list->wlan_acq_hyst_timer.value = DS_WLAN_ACQ_HYST_VAL;
  dsd_timer_list->hdr_nw_detach_hyst_timer.value = DS_HDR_NW_DETACH_HYST_VAL;
  dsd_timer_list->lte_nw_detach_hyst_timer.value = DS_LTE_NW_DETACH_HYST_VAL;

  /*-------------------------------------------------------------------------
    Read the DSD timer values from NV items
  -------------------------------------------------------------------------*/
  for( timer_indx = DS_1xAcqHystTimerID_T1;
       timer_indx <= DS_WLANAcqHystTimerID_T7;
       timer_indx++ )
  {
    timer_val = 0;
    if( ds_dsd_timer_get_nv_value(subs_id, timer_indx, &timer_val) )
    {
      DATA_MSG2(MSG_LEGACY_HIGH,
                "Setting timer_id %d to %d",
                TIMER_ID_FOR_SUBS_ID(subs_id, timer_indx), 
                timer_val);
      dsd_timer_info = ds_dsd_timer_get_timer_info_ptr(timer_indx, subs_id);
      if(dsd_timer_info != NULL) 
      {
        dsd_timer_info->value = timer_val;
      }
    }
  }

  /* Read EDCT timer from NV item */
  timer_val = 0;
  if( ds_dsd_timer_get_edct_nv_value(subs_id, &timer_val) )
  {
    DATA_MSG2(MSG_LEGACY_HIGH,
              "Setting timer_id %d to %d",
              DS_EPCDataCntxTimerID_T8, timer_val);

    dsd_timer_list->epc_data_cntx_timer.value = timer_val;
  }

  DATA_MSG9(MSG_LEGACY_MED,
            "DSD Timer Values "
            "1x Acq Hyst Timer = %d "
            "1x OOS Hyst Timer = %d "
            "HDR Acq Hyst Timer = %d "
            "HDR OOS Hyst Timer = %d "
            "LTE Acq Hyst Timer = %d "
            "LTE OOS Hyst Timer = %d "
            "EDCT Timer = %d"
            "HDR HandDown Timer = %d "
            "HDR HandUp avoid Timer = %d",
            dsd_timer_list->cdma_acq_hyst_timer.value,
            dsd_timer_list->cdma_oos_hyst_timer.value,
            dsd_timer_list->hdr_acq_hyst_timer.value,
            dsd_timer_list->hdr_oos_hyst_timer.value,
            dsd_timer_list->lte_acq_hyst_timer.value,
            dsd_timer_list->lte_oos_hyst_timer.value,
            dsd_timer_list->epc_data_cntx_timer.value,
            dsd_timer_list->hdr_handdown_timer.value,
            dsd_timer_list->hdr_handup_avoid_timer.value);

  return;
} /* ds_dsd_timer_init_timer_val_per_subs_id() */

/*===========================================================================
FUNCTION      DS_DSD_TIMER_EPC_NOTIFY

DESCRIPTION   Called by EPC module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_timer_epc_notify
(
  boolean                     epc_present,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  dword                    remaining_lte_oos_hyst_time = 0;
  dword                    total_lte_oos_hyst_time = 0;
  uint32                   edct_timer_val = 0;
  ds_dsd_timer_info_type  *lte_oos_timer_info = NULL;
  ds_dsd_timer_info_type  *edct_timer_info = NULL;
  ds_dsd_proc_type        *dsd_proc_ptr = NULL;
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_HIGH,"ds_dsd_timer_epc_notify, epc present: %d, subs id %d",
            epc_present,subs_id);

  /* Get DSD proc for subs id */
  dsd_proc_ptr = ds_dsd_proc_get_dsd_proc_instance(subs_id);

  /* Get LTE OOS timer info for subs id */
  lte_oos_timer_info = ds_dsd_timer_get_timer_info_ptr(DS_LTEOOSHystTimerID_T6,
                                                       subs_id);
  /* Get EDCT timer info for subs id */
  edct_timer_info = ds_dsd_timer_get_timer_info_ptr(DS_EPCDataCntxTimerID_T8,
                                                    subs_id);

  if((dsd_proc_ptr == NULL) ||(lte_oos_timer_info == NULL) || 
     (edct_timer_info == NULL))
  {
    DATA_MSG4(MSG_LEGACY_ERROR, "dsd_proc_ptr ptr: %d,lte_oos_timer_info ptr: %d"
                                ", edct_timer_info ptr: %d for subs_id:%d",
                                dsd_proc_ptr ,lte_oos_timer_info,
                                edct_timer_info,subs_id);
    return;
  }

  edct_timer_val = (edct_timer_info->value * 1000);
  remaining_lte_oos_hyst_time = timer_get(&(lte_oos_timer_info->timer),
                                          T_MSEC);
  total_lte_oos_hyst_time = (lte_oos_timer_info->value*1000) + edct_timer_val;
  /*-------------------------------------------------------------------------
    If EPC present moves from TRUE to FALSE while LTE OOS Hysteresis is
    running and EDCT timer value is greater than 0, reset timer to
    appropriate value.
  -------------------------------------------------------------------------*/
  if((epc_present == FALSE) &&
     (dsd_proc_ptr->epc_present_flag == TRUE) && 
     (ds_dsd_timer_running(DS_LTEOOSHystTimerID_T6, subs_id)) &&
     (edct_timer_val > 0))
  {
    if((remaining_lte_oos_hyst_time >  0) &&
       (total_lte_oos_hyst_time - remaining_lte_oos_hyst_time) < 
                       (lte_oos_timer_info->value * 1000))
    {
      if( TE_SUCCESS == timer_set(&(lte_oos_timer_info->timer),
                                  (remaining_lte_oos_hyst_time - edct_timer_val),
                                  0,
                                  T_MSEC) )
      {
      DATA_MSG1(MSG_LEGACY_HIGH,
                "RE-STARTING DSD TIMER DS_LTEOOSHystTimer :%d s",
                (remaining_lte_oos_hyst_time - edct_timer_val)/1000);
      }
    }
    else
    {
       /* Setting the timer value to zero to reset LTE SM */
      if( TE_SUCCESS == timer_set(&(lte_oos_timer_info->timer), 0, 0, T_MSEC) )
      {
        DATA_MSG0(MSG_LEGACY_HIGH,"Stopping LTE OOS HYST timer for EDCT");
      }
    }
  }

  /* cache the epc present flag */
  dsd_proc_ptr->epc_present_flag = epc_present;

  return;
}/* ds_dsd_timer_epc_notify() */

