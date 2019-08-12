/*!
   @file rfgsm_core_vbatt_comp.c

   @brief
 
   @details
 
*/

/*! @todo satheesha 10/30/08 do we need to keep the edit history??? */

/*===========================================================================
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_vbatt_comp.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/14   ec      Use new debug NV flags for VBatt Comp logging
03/19/14   hoh     Removed unused vbatt code
02/28/14   sc      Pass core data through mc to core layers
09/23/13   ggs     Bypass MSM ADC client init for Vbatt reads on RUMI platform
07/29/13   tws	   Correct vbatt nv conversion.
07/22/13   ec	   Use new F3s/Null pointer checks  
06/19/13   svi     Fix vbatt comp issues  
05/03/13   sb      Fixes for vbatt read taking too long
03/27/13   sb      Use vbatt reding after bound checking.
03/24/13   tws     Move temp and vbatt read timers to core handles.
03/22/13   sb      Fix memory leak due to repeated calling of DALsys malloc
02/12/13   sr      changes to ptrs before access 
01/29/12   sb      Added changes to obtain vbatt reading for vbatt comp
01/09/13   sr      Made changes to run vbatt comp in rf app task context.
12/03/12   aka     Changes to use NV ptr from rfgsm_core_handle_ptr
11/26/12   aka     Compiler warning fixes
11/26/12   aka     Changes to use a path specific gsm handle
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
10/16/12   tsr     Cleanup RF GSM bringup flags
05/10/12   jr      Reverted CR297404 as it is not applicable to NikeL since it already 
                   uses tx band while computing  Vbatt back off
07/29/11   am      Use proper tx band NV values while computing vbatt back off
01/10/12   sr      changes to enable vbatt comp.
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
31/01/11   sr      moved the vbatt_read() to HAL layer
01/28/11   bmg     Changed clk_def to timer_def2 to complete timer changes
01/25/11   sr      removed the clk.h dependency
12/17/10   sc      Corrected rf_vbatt_parange[0] scaling
11/01/10   lcl     Reduce F3 via NV107
10/11/10   lcl     Changed MSG_LVL to MSG_LEGACY to avoid too many F3 messages
03/19/10   lcl     Pull in changes from QSC6295
02/08/10   sr      lint errors fix
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/22/09   jps     Added rf_vbatt_parange variable 
10/26/09   jps     Enabled Vbatt compensation for TX 
10/12/09   sr      Fixed warnings.
09/17/09   sar     Updated clk interface for CMI conformance.
07/06/09   sr      changes to make the operating band to rfcom_gsm_band type
03/13/09   sr      code cleanup for SCMM build
10/30/08   sr      Initial revision.

============================================================================*/

#include "rfcom.h"
#include "msg.h"
#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "rfcommon_nv.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_core.h"
#include "rfgsm_core_util.h"
#include "rfgsm_msm.h"
#include "rfm_internal.h"
#include "rfcommon_msg.h"
#include "rfgsm_core_vbatt_comp.h"
#include "rfgsm_core_apps_task_processing.h"

/* Vbatt vs. Tx power compensation callback intervals */
const uint32 RFGSM_VBATT_COMP_REPEAT_INT = 10000;     /* milliseconds */
const uint32 RFGSM_VBATT_COMP_START_INT  = 1000;      /* milliseconds */


/*----------------------------------------------------------------------------*/
/*EXTERNAL FUNCTIONS*/
extern void rfgsm_core_set_tx_profile_update_flag( rfgsm_core_handle_type *rfgsm_core_handle_ptr );
extern uint16 rfgsm_nv_get_spare(void);
/*----------------------------------------------------------------------------*/


/* ===========================PUBLIC FUNCTIONS ===============================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the vbatt comp. clock callback.
*/
void rfgsm_core_vbatt_comp_init( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  boolean status = FALSE;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* init the vbatt comp info. */
  rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag = FALSE;
  rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_dBx100 = 0;
  rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv = 3700;
  rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_adc_client_init_status = FALSE;
  rfgsm_core_handle_ptr->vbatt_comp_info.enable_logging = rfgsm_nv_check_debug_nv(rfm_dev, RFGSM_DEBUG_NV_VBATT_COMP_LOGGING_ENABLE);

  if ( rfgsm_debug_flags.disable_vbatt_comp == TRUE )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_HIGH, "disable_vbatt_comp flag is TRUE! rfgsm_msm_adc_client_init is bypassed." );
  }
  else
  {
    status = rfgsm_msm_adc_client_init(rfm_dev);
  }

  rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_adc_client_init_status = status;

  /* define the vbatt compensation call back */
  timer_def2( &rfgsm_core_handle_ptr->rfgsm_core_vbatt_comp_clk_cb, NULL );
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts the callback that calls rf_calc_vbatt_comp .It is called when there is 
  transmition. 
*/
void rfgsm_core_vbatt_comp_start( rfm_device_enum_type rfm_dev )
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  /* Timers are set such that the calculation happens 3 secs later so that it
  ** doesn't start running as soon as TX power is enabled because we don't
  ** know if it is a real call
  */

    timer_reg( &rfgsm_core_handle_ptr->rfgsm_core_vbatt_comp_clk_cb,
               (timer_t2_cb_type)rfgsm_core_set_vbatt_comp_flag,
               (timer_cb_data_type)rfm_dev,
               RFGSM_VBATT_COMP_START_INT,
               RFGSM_VBATT_COMP_REPEAT_INT);
  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops the callback that calls rf_calc_vbatt_comp .It is called when there is 
  no transmition. 
*/
void rfgsm_core_vbatt_comp_stop( rfm_device_enum_type rfm_dev )
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  if ( rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag )
  {
    (void)timer_clr(&rfgsm_core_handle_ptr->rfgsm_core_vbatt_comp_clk_cb, T_NONE);
    rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag = FALSE;
  } 
  else
  {
    /* rfgsm_disable_tx is run before rfgsm_enable_tx, change priority to HIGH*/
    RFGC_MSG( MSG_LEGACY_HIGH, "Trying to stop vbatt comp. calculator even though it is not started!!!" );
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the calculated vbatt backoff value.

  @retval rf vbatt backoff value.
*/

int16 rfgsm_core_vbatt_comp_pwr_get(rfm_device_enum_type rfm_dev)
{

  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", 0);

  return rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_dBx100;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This funtion performs the vbatt read. 
 
*/
void rfgsm_core_vbatt_read_handler(rf_apps_cmd_type* cmd_ptr) 
{
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  uint16 curr_vbatt_read = 0;

  boolean flag;

  if ( rfgsm_debug_flags.disable_vbatt_comp == TRUE )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_HIGH, "disable_vbatt_comp flag is TRUE! vbatt read is not done." );
    return;
  }

  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];
  flag = (boolean)cmd_ptr->payload[1];
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");
    
  curr_vbatt_read = (uint16)rfgsm_msm_get_adc_read(rfm_dev,flag);

  /* Update Vbatt read only when the value is non zero, else use the last value*/
  if (curr_vbatt_read > 0)
  {
    rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv = curr_vbatt_read;
  }
  else
  {
    if (flag == TRUE)
    {
      RFGC_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_vbatt_read_handler, invalid vbatt adc reading detected:%d", curr_vbatt_read );
    }
  }
  
  rfgsm_core_handle_ptr->tx_log.vbatt_mv = rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv;

  if (flag)
  {
    /* trigger the vbatt comp */
    rfgsm_core_vbatt_comp_trigger(rfm_dev);
  }

  return;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates the amount of tx power compensation 
  based on current battery voltage.
*/
void rfgsm_core_vbatt_comp_handler(rf_apps_cmd_type* cmd_ptr)
{ 
  rfm_device_enum_type rfm_dev = RFM_INVALID_DEVICE;
  rfgsm_core_vbatt_comp_info_type *vbatt_comp_info_ptr = NULL;
  static uint16 last_valid_vbatt_val = 0;
  uint16 temp_vbatt_val, vbatt_level_low, vbatt_level_mid,vbatt_level_high;
  boolean enable_logging;
  gsm_vbatt_type vbatt_nv_data;
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  rfm_dev = (rfm_device_enum_type)cmd_ptr->payload[0];

  if ( rfgsm_debug_flags.disable_vbatt_comp == TRUE )
  {
    RFGC_MSG( MSG_LEGACY_HIGH, "disable_vbatt_comp flag is TRUE! vbatt comp is not done." );
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

  RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

  vbatt_comp_info_ptr = &(rfgsm_core_handle_ptr->vbatt_comp_info);
  temp_vbatt_val = vbatt_comp_info_ptr->vbatt_value_mv;

  RF_NULL_CHECK_RTN_V( rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl, "rfgsm_tx_nv_tbl is NULL!" );

  vbatt_nv_data  =  rfgsm_core_handle_ptr->rfgsm_tx_nv_tbl->tx_static_nv_data_ptr->vbatt_data;

  vbatt_level_low = vbatt_nv_data.vbatt_levels[0];
  vbatt_level_mid = vbatt_nv_data.vbatt_levels[1];
  vbatt_level_high = vbatt_nv_data.vbatt_levels[2];

  /* Check if logging should be enabled (set from debug NV at initialisation) */
  enable_logging = rfgsm_core_handle_ptr->vbatt_comp_info.enable_logging;

  /* verify that NV is valid, else don't do vbatt comp */
  if (( vbatt_level_low == vbatt_level_mid )  || (vbatt_level_mid == vbatt_level_high ))
  {
    if ( enable_logging == TRUE )
    {
      RFGC_MSG( MSG_LEGACY_HIGH, "Vbatt ADC values in NV for 3.2, 3.7 and 4.2V are incorrect" );
    }
    return;
  }

  if (vbatt_comp_info_ptr->vbatt_value_mv != last_valid_vbatt_val)
  {

    if (vbatt_comp_info_ptr->vbatt_value_mv > vbatt_level_high )
    {
      temp_vbatt_val = vbatt_level_high;
    }
    else if (vbatt_comp_info_ptr->vbatt_value_mv < vbatt_level_low )
    {
      temp_vbatt_val = vbatt_level_low;
    }

     /* Currently we are using VBATT comp only for the max PCL*/
    if ( temp_vbatt_val >= vbatt_level_mid )
    {

      vbatt_comp_info_ptr->vbatt_comp_dBx100 = (int16) ((vbatt_nv_data.vbatt_comp_value_hi.vbatt_comp_dBx100) * 
                                                              (temp_vbatt_val - vbatt_level_mid) / 
                                                              (vbatt_level_high - vbatt_level_mid));
    }
    else
    {
      vbatt_comp_info_ptr->vbatt_comp_dBx100 = (int16) ((vbatt_nv_data.vbatt_comp_value_lo.vbatt_comp_dBx100) * 
                                                              (temp_vbatt_val - vbatt_level_mid)/ 
                                                              (vbatt_level_low - vbatt_level_mid)); 
    }

    if ( enable_logging == TRUE )
    {
      RFGC_MSG_3( MSG_LEGACY_HIGH, 
                  "Vbatt_update: Curr vbatt= %d, Prev vbatt =%d, rf_vbatt=%d", 
                  vbatt_comp_info_ptr->vbatt_value_mv, 
                  last_valid_vbatt_val, 
                  vbatt_comp_info_ptr->vbatt_comp_dBx100 );

      RFGC_MSG_1( MSG_LEGACY_HIGH, 
                  "Compensated vbatt is %d", 
                  vbatt_comp_info_ptr->vbatt_comp_dBx100 );
    }

    last_valid_vbatt_val = vbatt_comp_info_ptr->vbatt_value_mv;
    rfgsm_core_set_tx_profile_update_flag(rfgsm_core_handle_ptr);

  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function to read the vbatt and is called evey 2seconds. 
  This function will read the battery voltage, the read vbatt value will be used for
  tx power vbatt compensation. */

/*lint -e{715} t_unused is unused - hence the name */
void rfgsm_core_vbatt_read_trigger(uint32 t_unused, int32 rfm_dev, boolean flag)
{
  /* send vbatt read command to rf_apps_task, which is low priority task */
  
  rf_apps_cmd_type *vbatt_read_cmd = NULL;

  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( vbatt_read_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rfgsm_core_vbatt_read_trigger: RF command queue is full!" );
    return;
  }

  vbatt_read_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_VBATT_READ_CMD;
  vbatt_read_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
  vbatt_read_cmd->payload[0] = (uint8)rfm_dev;
  vbatt_read_cmd->payload[1] = (uint8)flag;

  /* place RFGSM_CORE_APPS_TASK_VBATT_READ_CMD to RF command queue */
  rf_apps_cmd_put( vbatt_read_cmd );
   
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will trigger the rf_app task to apply(Calculate) the tx power
  vbatt compensation based on recent vbatt read value. 
  This can be called upon the receipt of recent vbatt read event or whenever the
  Tx band changes */

void rfgsm_core_vbatt_comp_trigger(int32 rfm_dev)
{
  /* send vbatt comp command to rf_apps_task */
  
  rf_apps_cmd_type *vbatt_comp_cmd = NULL;

  /* Get the pointer to the free buffer of RF command queue. */
  if ( ( vbatt_comp_cmd = rf_apps_get_buf() ) == NULL )
  {
    RF_MSG( RF_ERROR, "rfgsm_core_vbatt_comp_trigger: RF command queue is full!" );
    return;
  }

  vbatt_comp_cmd->msgr_hdr.id = RFGSM_CORE_APPS_TASK_VBATT_COMP_CMD;
  vbatt_comp_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_GSM_ID;
  vbatt_comp_cmd->payload[0] = (uint8)rfm_dev;

  /* place RFGSM_CORE_APPS_TASK_VBATT_COMP_CMD to RF APP Task command queue */
  rf_apps_cmd_put( vbatt_comp_cmd );
   
}



void rfgsm_core_set_vbatt_comp_flag(uint32 t_unused, rfm_device_enum_type rfm_dev )
{
    rfgsm_core_handle_type *rfgsm_core_handle_ptr;
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

    RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

    rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_comp_started_flag = TRUE;

}


void rfgsm_core_override_vbatt_read(rfm_device_enum_type rfm_dev, uint32 vbatt_val)
{
   rfgsm_core_handle_type *rfgsm_core_handle_ptr;
   rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);

   RF_NULL_CHECK_RTN_V(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!");

   rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv = (uint16)vbatt_val;

   /* Record this in FTM logs*/
   rfgsm_core_handle_ptr->tx_log.vbatt_mv = rfgsm_core_handle_ptr->vbatt_comp_info.vbatt_value_mv;   

}
