/*=============================================================================
  @file sns_smgr_fifo.c

  This file contains the logic for managing sensor FIFO in SMGR

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_fifo.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-04-20  pn   Must consider possibility of sensor running slower than nominal
  2016-04-12  pn   Resets num_received_samples when FIFO is flushed
  2016-01-07  pn   Updated handing of FIFO_OVERFLOW event
  2015-12-30  pn   Added more conditions for when it's safe to change WM
  2015-12-15  pn   WMs for sibling sensors not updated before valid samples received
  2015-12-08  pn   Only flushes FIFO after streaming starts
  2015-10-15  pn   Fixed handling of ODR_CHANGED event
  2015-09-30  pn   Enabled controls of sensors in FAILED state
  2015-09-24  pn   Code cleanup
  2015-09-15  pn   - Only limits depot_max if depot is smaller than max FIFO 
                   - Fixed ideal watermark calculation
  2015-08-26  MW   Fixed remaining integer overflow in WM calculation
  2015-08-19  MW   Update idle_wm to uint32 to prevent data overflow in
                   sns_smgr_fifo_calculate_ideal_watermark()
  2015-06-05  pn   Same code path for Discrete and Unified FIFOs
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-03-10  jms  - Calculate unified FIFO WM from max report rate of sensors in this FIFO
                   - Removed unused/ineffective discrete FIFO synch code
  2015-02-17  pn   Changed sns_smgr_fifo_configure() signature
  2014-10-22  pn   Triggers update of report interval when updating watermark 
  2014-10-16  vy   Updated EWUR api to handle interrupt latency est
  2014-09-25  pn   Uses depth of appropriate depot
  2014-09-14  vy   Dynamically allocate Shared fifo data struct 
  2014-09-08  pn   Turns on QuP clock before triggering FIFO
  2014-08-05  pn   Separated FIFO flushing from watermark setting
  2014-04-23  pn   Initial version

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <math.h>
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_memmgr.h"
#include "sns_profiling.h"
#include "sns_rh_util.h"
#include "sns_smgr_ddf_if.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"
#include "sns_smgr_pm_if.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SMGR_FIFO_DEBUG
#ifdef SNS_SMGR_FIFO_DEBUG
#define SNS_SMGR_FIFO_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_FIFO_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_FIFO_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_FIFO_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_FIFO_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_FIFO_DBG0(level,msg)
#define SNS_SMGR_FIFO_DBG1(level,msg,p1)
#define SNS_SMGR_FIFO_DBG2(level,msg,p1,p2)
#define SNS_SMGR_FIFO_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_FIFO_DBG4(level,msg,p1,p2,p3,p4)
#endif

#define SNS_SMGR_OK_TO_CHANGE_WM(s)                                  \
  ( SMGR_PRIMARY_SENSOR(s)->first_valid_sample_received &&           \
    (s)->sensor_state==SENSOR_STATE_READY &&                         \
    (s)->fifo_cfg.can_fifo_be_used &&                                \
    SMGR_BIT_CLEAR_TEST((s)->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) && \
    SMGR_BIT_CLEAR_TEST((s)->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) && \
    SMGR_BIT_CLEAR_TEST((s)->flags, SMGR_SENSOR_FLAGS_ODR_CHANGED_B) )

#define SNS_SMGR_FIFO_SAFE_WM_PERCENT 95
#define SNS_SMGR_FIFO_SAMPLING_RATE_PERCENT 95

/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/
SMGR_STATIC sns_ddf_status_e sns_smgr_fifo_set_watermark
(
  sns_smgr_sensor_s*    sensor_ptr,
  sns_ddf_sensor_e      sensor
);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


//============================================================================
//============================================================================
//                       I N I T    F I F O
//============================================================================
//============================================================================

/*===========================================================================
  FUNCTION:   sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle
===========================================================================*/
/*!
  @brief finds sns_smgr_sensor_s* - pointer to sensor
  @details Identifies sensor by ddf_sensor_type and dd_handle_ptr

  @param[in] sns_ddf_sensor_e  ddf_sensor_type
  @param[in] sns_ddf_handle_t  dd_handle_ptr
  @return
   sns_smgr_sensor_s* - pointer to the sensor of interest
 */
/*=====================================================================*/
SMGR_STATIC sns_smgr_sensor_s* sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle
(
  sns_ddf_sensor_e       ddf_sensor_type,
  const sns_ddf_handle_t dd_handle_ptr
)
{
  uint8_t k;
  for ( k=0; k<ARR_SIZE(sns_smgr.sensor); k++ )
  {
    sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[k];
    uint8_t i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      if ( sensor_ptr->dd_handle           == dd_handle_ptr &&
           SMGR_SENSOR_TYPE(sensor_ptr, i) == ddf_sensor_type )
      {
        return sensor_ptr;
      }
    }
  }
  return NULL;
}

/*===========================================================================
  FUNCTION:   sns_smgr_init_fifo_cfg
===========================================================================*/
/*!
  @brief Initializes the fifo configuration of given sensor.
  @param[in/out] sensor_ptr - the sensor of interest
  @return
   none
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_fifo_init_fifo_cfg( sns_smgr_sensor_s *sensor_ptr )
{
  uint8_t  i    = 0;
  uint32_t len  = 0;
  smgr_sensor_fifo_cfg_s*    fifo_cfg_ptr  = &sensor_ptr->fifo_cfg;
  sns_ddf_fifo_attrib_get_s* fifo_attr_ptr = NULL;
  sns_ddf_status_e           ddf_status    = SNS_DDF_SUCCESS;

  SNS_OS_MEMSET(&(sensor_ptr->fifo_cfg),0,sizeof(smgr_sensor_fifo_cfg_s));

  if ( !SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
  {
    return SNS_ERR_NOTALLOWED;
  }

  ddf_status = sns_smgr_get_attr( sensor_ptr,
                                  SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                                  SNS_DDF_ATTRIB_FIFO,
                                  (void**)&fifo_attr_ptr,
                                  &len );

  if ( SNS_DDF_SUCCESS != ddf_status )
  {
    SNS_SMGR_PRINTF1( ERROR, "FIFO Init: get_attr ERROR, SensId=%d",
                      SMGR_SENSOR_ID(sensor_ptr) );
    return SNS_ERR_FAILED;
  }
  
  if ( !fifo_attr_ptr->is_supported || fifo_attr_ptr->max_watermark == 0 )
  {
    SNS_SMGR_PRINTF3(
      ERROR, "init_fifo_cfg - sensor=%u supported=%u max_wm=%u",
      SMGR_SENSOR_ID(sensor_ptr), fifo_attr_ptr->is_supported,
      fifo_attr_ptr->max_watermark);
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
    return SNS_ERR_NOTALLOWED;
  }

  fifo_cfg_ptr->is_fifo_supported      = fifo_attr_ptr->is_supported;
  fifo_cfg_ptr->is_hw_watermark_int_supported = !(fifo_attr_ptr->is_sw_watermark);
  fifo_cfg_ptr->can_fifo_be_used       = true;
  fifo_cfg_ptr->max_watermark          = fifo_attr_ptr->max_watermark;
  fifo_cfg_ptr->suitable_max_watermark = fifo_attr_ptr->max_watermark * 
    SNS_SMGR_FIFO_SAFE_WM_PERCENT / 100;
  fifo_cfg_ptr->depot_max              =
    sns_smgr_get_max_sample_count(SMGR_PRIMARY_SENSOR(sensor_ptr));
  if ( fifo_cfg_ptr->depot_max < fifo_attr_ptr->max_watermark )
  {
    fifo_cfg_ptr->depot_max            = fifo_cfg_ptr->depot_max *
      SNS_SMGR_FIFO_SAFE_WM_PERCENT / 100;
  }

  //----------------- Shared fifo - sensor siblings ------------------------
  sns_q_init(&fifo_cfg_ptr->shared_ddf_sens_q);

  for ( i=0; i < fifo_attr_ptr->share_sensor_cnt; i++ )
  {
    smgr_fifo_shared_sensor_s *shared_sen_ptr;
    sns_ddf_fifo_share_sensor_s* cfg_share_sensor_ptr = fifo_attr_ptr->share_sensors[i];
    sns_smgr_sensor_s* sibling_sensor_ptr = NULL;

    SNS_SMGR_PRINTF3(
      MED, "init_cfg - shared sensor, idx=%u ddf_sensor=%u d_handle=0x%x",
      i, cfg_share_sensor_ptr->sensor, cfg_share_sensor_ptr->dd_handle);

    if ( cfg_share_sensor_ptr->dd_handle == NULL )
    {
      continue;
    }

    sibling_sensor_ptr = sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle(
      cfg_share_sensor_ptr->sensor,
      cfg_share_sensor_ptr->dd_handle);
    if ( sibling_sensor_ptr == NULL )
    {
      continue;
    }

    if ( !SMGR_SENSOR_FIFO_IS_ENABLE(sibling_sensor_ptr) )
    {
      SNS_SMGR_PRINTF0(ERROR, "init_cfg - sibling sensor not configured for FIFO");
      continue;
    }

    if ( sensor_ptr->is_uimg_refac )
    {
      shared_sen_ptr = SMGR_U_ALLOC_STRUCT(smgr_fifo_shared_sensor_s);
    }
    else
    {
      shared_sen_ptr = SMGR_ALLOC_STRUCT(smgr_fifo_shared_sensor_s);
    }
    SNS_ASSERT(shared_sen_ptr != NULL);

    shared_sen_ptr->sensor_ptr = sibling_sensor_ptr;

    sns_q_link(shared_sen_ptr, &shared_sen_ptr->sibling_link);
    sns_q_put(&fifo_cfg_ptr->shared_ddf_sens_q, &shared_sen_ptr->sibling_link);
  }

  sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);

  return SNS_SUCCESS;
}

sns_ddf_status_e sns_smgr_fifo_set_availability
(
  sns_smgr_sensor_s* sensor_ptr,
  bool available
)
{
  smgr_fifo_shared_sensor_s *shared_sensor_ptr;
  // Set the sensor's FIFO configuration availability
  sensor_ptr->fifo_cfg.can_fifo_be_used = available;
  if (!available)
  {
    sensor_ptr->fifo_cfg.is_fifo_enabled = false;
  }
  // Set the availability for any of the shared FIFO configurations
  SMGR_FOR_EACH_Q_ITEM(&sensor_ptr->fifo_cfg.shared_ddf_sens_q,
                       shared_sensor_ptr, sibling_link)
  {
    sns_smgr_sensor_s* sibling_sensor_ptr = shared_sensor_ptr->sensor_ptr;

    sibling_sensor_ptr->fifo_cfg.can_fifo_be_used = available;
    if (!available)
    {
      sibling_sensor_ptr->fifo_cfg.is_fifo_enabled = false;
    }
  }

  return SNS_DDF_SUCCESS;
}
//============================================================================
//============================================================================
//            F I F O     E V E N T     P R O C E S S I N G
//============================================================================
//============================================================================

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_event_fifo_available
===========================================================================*/
/*!
  @brief Fifo is available since the DD enabled fifo
         - configuring fifo on SNS_DDF_EVENT_FIFO_AVAILABLE event
  @param[in/out] sensor_ptr
  @param[in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_available
(
  sns_smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e   sensor
)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_FIFO_DBG1( MED, "FIFO SNS_DDF_EVENT_FIFO_AVAILABLE ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  sns_smgr_fifo_set_availability(sensor_ptr,true);
  sns_smgr_fifo_configure( sensor_ptr, false );
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_AVAILABLE );
  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_event_fifo_unavailable
===========================================================================*/
/*!
  @brief Fifo is unavailable since the DD disabled fifo
         - clearing fifo structures onSNS_DDF_EVENT_FIFO_UNAVAILABLE
  @param[i] sensor_ptr
  @param[i] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_unavailable
(
  sns_smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e   sensor
)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_FIFO_DBG1( MED, "FIFO SNS_DDF_EVENT_FIFO_UNAVAILABLE ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  //TODO: erase all fields -> sns_smgr_fifo_discrete_clean_sync_info();
  sensor_ptr->fifo_cfg.can_fifo_be_used  = false;
  sensor_ptr->fifo_cfg.current_watermark = 0;
  sns_smgr_fifo_set_availability(sensor_ptr,false);

  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_DISABLED );
  return SNS_DDF_SUCCESS;
}


/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_fifo_overflow
===========================================================================*/
/*!
  @brief reconfiguring fifo on SNS_DDF_EVENT_FIFO_OVERFLOW
  @param [in/out] sensor_ptr
  @param [in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_overflow
(
  sns_smgr_sensor_s*   sensor_ptr,
  sns_ddf_sensor_e     sensor
)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_PRINTF1(MED, "on_event_fifo_overflow - ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_OVERFLOW );

  sns_smgr_fifo_flush(sensor_ptr, true);
  
  return SNS_DDF_SUCCESS;
}


//============================================================================
//============================================================================
//                          W A T E R  M A R K
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_set_watermark
===========================================================================*/
/*!
  @brief Set Watermark level for the sensor
  @details
  - This Function sets the watermark for sensor.
  - Watermark Value 0 means disable fifo. Value > 0 means fifo is enabled.
  @param[in] sensor_ptr        - the sensor leader structure
  @param[in] sensor            - sensor type
  @return  sns_ddf_status_e
 */
/*=========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_fifo_set_watermark
(
  sns_smgr_sensor_s*       sensor_ptr,
  sns_ddf_sensor_e         sensor
)
{
  sns_ddf_status_e   ddf_status = SNS_DDF_SUCCESS;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = SMGR_PRIMARY_SENSOR(sensor_ptr);
  uint16_t           curr_wm = sensor_ptr->fifo_cfg.current_watermark;
  uint16_t           desired_wm = sensor_ptr->fifo_cfg.desired_watermark;
  uint32_t           curr_odr = ddf_sensor_ptr->current_odr;

  if( SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) &&
      sensor_ptr->fifo_cfg.can_fifo_be_used)
  {
    q16_t new_intr_rate;
    q16_t old_intr_rate;
    sns_ddf_fifo_set_s fifo_attr;
    SNS_OS_MEMZERO(&fifo_attr,sizeof(sns_ddf_fifo_set_s));

    fifo_attr.watermark = desired_wm;
    ddf_status = sns_smgr_set_attr(sensor_ptr, sensor, SNS_DDF_ATTRIB_FIFO, &fifo_attr);

    sensor_ptr->fifo_cfg.is_fifo_enabled = (desired_wm > 0) ? true : false;
    sensor_ptr->fifo_cfg.current_watermark = desired_wm;

    new_intr_rate = FX_CONV_Q16((desired_wm == 0) ? 0 : (curr_odr/desired_wm), 0);
    old_intr_rate = FX_CONV_Q16((curr_wm == 0 ) ? 0 : (curr_odr/curr_wm), 0);
    sns_smgr_update_ewur(new_intr_rate, old_intr_rate,sensor_ptr->is_uimg_refac, true );
  }
  return ddf_status;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_set_wm_and_intr
===========================================================================*/
/*!
  @brief set FIFO watermark and interrupt for the sensor
  @details
  - This Function sets FIFO watermark if it has changed.
    and intr if requested by enable_intr argument
  - Takes watermark value from sensor_ptr->fifo_cfg.desired_watermark
  @param[in/out] sensor_ptr  - the sensor leader structure
  @return  sns_err_code_e
 */
/*=======================================================================*/
SMGR_STATIC 
sns_err_code_e sns_smgr_fifo_set_wm_and_intr(sns_smgr_sensor_s* sensor_ptr)
{
  sns_ddf_sensor_e ddf_sensor_type = SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr);
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = SMGR_PRIMARY_SENSOR(sensor_ptr);
  bool enable_intr = (sensor_ptr->fifo_cfg.desired_watermark > 0) ? true : false;

  SNS_SMGR_PRINTF4(
    LOW, "set_wm_and_intr - ddf_sensor=%d curr/want=%u/%u enable_intr=%u",
    ddf_sensor_type, sensor_ptr->fifo_cfg.current_watermark, 
    sensor_ptr->fifo_cfg.desired_watermark, enable_intr);

  if ( sensor_ptr->fifo_cfg.desired_watermark >= 
       sensor_ptr->fifo_cfg.max_watermark ) // !tested in WM calc
  {
    SNS_SMGR_PRINTF0(ERROR, "set_wm_and_intr - desired_WM >= max_watermark");
    return SNS_ERR_BAD_PARM;
  }

  if ( sensor_ptr->fifo_cfg.desired_watermark > 0 &&
       sensor_ptr->sensor_state != SENSOR_STATE_READY &&
       sensor_ptr->sensor_state != SENSOR_STATE_CONFIGURING )
  {
    SNS_SMGR_PRINTF0(
      ERROR, "set_wm_and_intr - Sensor not in READY/CONFIGURING state");
    return SNS_ERR_NOTALLOWED;
  }

  if ( sensor_ptr->is_self_sched_intr_enabled )
  {
    sns_smgr_dd_enable_sched_data( sensor_ptr, ddf_sensor_type, false );
  }

  sns_smgr_fifo_flush(sensor_ptr, false);

  if ( sns_smgr_fifo_set_watermark(sensor_ptr, ddf_sensor_type) == SNS_DDF_SUCCESS )
  {
    /* change in FIFO watermark may affect report interval */
    ddf_sensor_ptr->client_stat.client_modifier.last_updated = sns_smgr.last_tick;
  }
  else
  {
    SNS_SMGR_PRINTF0(
      ERROR,"set_wm_and_intr - failed setting WM; re-enabling INT now");
    return SNS_ERR_FAILED;
  }

  if ( enable_intr != sensor_ptr->is_self_sched_intr_enabled )
  {
    sns_smgr_dd_enable_sched_data( sensor_ptr, ddf_sensor_type, enable_intr );
  }

  sns_smgr_fifo_log_event( 
    SMGR_PRIMARY_SENSOR(sensor_ptr),
    (sensor_ptr->fifo_cfg.desired_watermark > 0) ? 
    SNS_DDF_FIFO_ENABLED : SNS_DDF_FIFO_DISABLED );

  SNS_SMGR_PRINTF3(
    HIGH, "set_wm_and_intr - ddf_sensor=%u fifo_enabled=%u wm=%u",
    ddf_sensor_type, sensor_ptr->fifo_cfg.is_fifo_enabled,
    sensor_ptr->fifo_cfg.current_watermark);

  return SNS_SUCCESS;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_get_max_rpt_rate
===========================================================================*/
/*!
  @brief Gets max report rate of given sensor and its siblings, if any.
  @details
  @param[in/out] sensor_ptr   - sns_smgr_sensor_s
  @return  max rpt rate for all sensors on the shared fifo
 */
/*=====================================================================*/
SMGR_STATIC q16_t sns_smgr_fifo_get_max_rpt_rate(const sns_smgr_sensor_s* sensor_ptr)
{
  smgr_fifo_shared_sensor_s *shared_sensor_ptr;
  q16_t max_rpt_rate = sns_rh_get_max_report_rate_for_sensor(sensor_ptr);

  SMGR_FOR_EACH_Q_ITEM(&sensor_ptr->fifo_cfg.shared_ddf_sens_q,
                       shared_sensor_ptr, sibling_link)
  {
    sns_smgr_sensor_s* sibling_sensor_ptr = shared_sensor_ptr->sensor_ptr;
    if ( sibling_sensor_ptr->sensor_state > SENSOR_STATE_IDLE )
    {
      q16_t sibling_max_rr = 
        sns_rh_get_max_report_rate_for_sensor(sibling_sensor_ptr);
      max_rpt_rate = MAX(max_rpt_rate, sibling_max_rr);
    }
  }

  SNS_SMGR_PRINTF3(
    MED, "sensor=%u max_rpt_rate[Hz]=0x%x/%u", 
    SMGR_SENSOR_ID(sensor_ptr), max_rpt_rate, max_rpt_rate>>16);

  return max_rpt_rate;
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_ideal_watermark
===========================================================================*/
/*!
  @brief Calculates WM as if FIFO was unlimited. 
 
  @details Called for each of the sensors sharing the same FIFO.  The @max_wm
           is that of the main sensor.  
 
  @param[in/out] sensor_ptr    - sns_smgr_sensor_s
  @param[in]     max_rpt_rate  - max report rate amongst all sensors sharing the FIFO
  @param[in]     max_mw        - max watermark of the main sensor
  @param[in/out] sum_wm_ptr    - destination for sum of watermarks of all sensors
  @param[in/out] sum_depot_ptr - destination for sum of sample depots of all sensors
 
  @return  max rpt rate for all sensors on the shared fifo
 */
/*=====================================================================*/
SMGR_STATIC void sns_smgr_fifo_calculate_ideal_watermark(
  sns_smgr_sensor_s* sensor_ptr,
  q16_t              max_rpt_rate,
  uint16_t           max_wm,
  uint32_t*          sum_wm_ptr,
  uint32_t*          sum_depot_ptr)
{
  sensor_ptr->fifo_cfg.desired_watermark = 0;
  if ( max_rpt_rate > 0 && sensor_ptr->sensor_state == SENSOR_STATE_READY &&
       SMGR_PRIMARY_SENSOR(sensor_ptr)->client_stat.num_rpt_items != 0 )
  {
    uint16_t odr = smgr_sensor_type_max_odr(sensor_ptr);
    uint32_t ideal_wm = sns_smgr_rate_hz_to_q16(odr) / max_rpt_rate;

    SNS_SMGR_FIFO_DBG4(
      LOW, "ideal_wm - sensor=%u max_odr=%u depot=%u wm=%u", 
      SMGR_SENSOR_ID(sensor_ptr), odr, sensor_ptr->fifo_cfg.depot_max, ideal_wm);

    ideal_wm = MAX(1, ideal_wm);

    sensor_ptr->fifo_cfg.desired_watermark = 
      (uint32_t)ceilf((float)ideal_wm * SNS_SMGR_FIFO_SAMPLING_RATE_PERCENT / 100);

    /*
      Because one sensor may have samples of size different than that of another
      max_wm for one sensor can differ from max_wm for its siblings.  To compute 
      sum_wm it is necessary to find the equivalent ideal_mw of the given sensor 
      with respect to the given max_wm.

      For example, assume the same FIFO can hold up to 100 Accel samples but only
      up to 80 Mag samples.  If ideal_mw is 1000 for each (and to simplify matters, 
      assume each sensor's depot_max is greater than its max_wm) using Accel's 
      max_wm of 100, Mag's equivalent ideal_wm would be 1250, sum_wm would be 
      1000+1250=2250, and divider would be CEILING(2250/100)=23.  Similarly, using 
      Mag's max_wm of 80, Accel's equivalent ideal_wm would be 800, sum_wm would 
      be 800+1000=1800, divider would be CEILING(1800/80)=23.  The resulting 
      real_wm for each would be FLOOR(1000/23)=43.
    */
    if ( ideal_wm != 0 && sensor_ptr->fifo_cfg.max_watermark != max_wm )
    {
      ideal_wm = (max_wm * ideal_wm / sensor_ptr->fifo_cfg.max_watermark);
    }

    if ( ideal_wm != 0 )
    {
      *sum_wm_ptr    += ideal_wm;  /* Total Equivalent Sensor Samples */
      *sum_depot_ptr += sensor_ptr->fifo_cfg.depot_max;
    }
  }
  SNS_SMGR_FIFO_DBG2(
    MED, "ideal_wm - sensor=%u wm=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->fifo_cfg.desired_watermark);
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_divider
===========================================================================*/
SMGR_STATIC uint32_t sns_smgr_fifo_calculate_divider(
  sns_smgr_sensor_s* sensor_ptr,
  uint32_t           sum_wm,
  uint32_t           sum_depot)
{
  uint16_t suitable_max_wm = 
    MIN(sensor_ptr->fifo_cfg.suitable_max_watermark, sum_depot);
  /* TODO - adjust suitable_max_watermark to prevent late/skipped polling cycle */
  uint32_t wm_div = sum_wm / suitable_max_wm;

  if ( sum_wm % suitable_max_wm )
  {
    wm_div++;
  }
  wm_div = MAX(1, wm_div);

  SNS_SMGR_FIFO_DBG4(
    MED, "calc_div - suitable=%u sum_wm=%u sum_depot=%u div=%u", 
    sensor_ptr->fifo_cfg.suitable_max_watermark, sum_wm, sum_depot, wm_div);

  return wm_div;
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_real_watermark
===========================================================================*/
SMGR_STATIC void sns_smgr_fifo_calculate_real_watermark(
  sns_smgr_sensor_s* sensor_ptr,
  uint32_t           wm_div)
{
  if ( sensor_ptr->fifo_cfg.desired_watermark > wm_div )
  {
    sensor_ptr->fifo_cfg.desired_watermark /= wm_div;
  }
  SNS_SMGR_FIFO_DBG2(
    HIGH, "real_wm - sensor=%u desired_wm=%u", 
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->fifo_cfg.desired_watermark);
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_watermarks
===========================================================================*/
/*!
  @brief Calculates FIFO watermark for the given sensor and all sensors sharing its FIFO
 
  @details
    FIFO WM calculation is done in three simple steps:
      1. Find ideal WM as if FIFO was unlimited.  Done for each sensor sharing the FIFO.
      2. Calculate divider based on limits of FIFO and depots
      3. Calculate realistic WM for each sensor
 
  @param[in/out] sensor_ptr - sns_smgr_sensor_s
 
  @return none
 */
/*=====================================================================*/
SMGR_STATIC void sns_smgr_fifo_calculate_watermarks(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t wm_div = 1;
  uint32_t total_depot_depth = 0;
  uint32_t sum_wm = 0;
  q16_t    max_rpt_rate = sns_smgr_fifo_get_max_rpt_rate(sensor_ptr);
  smgr_fifo_shared_sensor_s * shared_sens_ptr;

  /* calculates watermark for each sensor sharing the FIFO */
  sns_smgr_fifo_calculate_ideal_watermark(
    sensor_ptr, max_rpt_rate, sensor_ptr->fifo_cfg.max_watermark,
    &sum_wm, &total_depot_depth);

  SMGR_FOR_EACH_Q_ITEM(&sensor_ptr->fifo_cfg.shared_ddf_sens_q,
                       shared_sens_ptr, sibling_link)
  {
    sns_smgr_sensor_s* sibling_ptr = shared_sens_ptr->sensor_ptr;
    if ( SNS_SMGR_OK_TO_CHANGE_WM(sibling_ptr)  )
    {
      sns_smgr_fifo_calculate_ideal_watermark(
        shared_sens_ptr->sensor_ptr, max_rpt_rate, sensor_ptr->fifo_cfg.max_watermark,
        &sum_wm, &total_depot_depth);
    }
  }

  /* calculates divider */
  wm_div = sns_smgr_fifo_calculate_divider(sensor_ptr, sum_wm, total_depot_depth);

  /* applies divider */
  sns_smgr_fifo_calculate_real_watermark(sensor_ptr, wm_div);

  SMGR_FOR_EACH_Q_ITEM(&sensor_ptr->fifo_cfg.shared_ddf_sens_q,
                       shared_sens_ptr, sibling_link)
  {
    sns_smgr_sensor_s* sibling_ptr = shared_sens_ptr->sensor_ptr;
    if ( SNS_SMGR_OK_TO_CHANGE_WM(sibling_ptr)  )
    {
      sns_smgr_fifo_calculate_real_watermark(shared_sens_ptr->sensor_ptr, wm_div);
    }
  }
}


//============================================================================
//============================================================================
//                      P U B L I C     U T I L S
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_config_fifo
===========================================================================*/
/*!
  @brief configure FIFO for the sensor
  @details
  - This Function configures FIFO if necessary by
    recalculating watermark and sets it if it has changed.
  - Call this function for any change
    in report rates or the sensor ODR properties.
  @param[in/out] sensor_ptr - the sensor leader structure
  @param[in]     force_wm1  - whether to use WM of 1
  @return NONE
 */
/*=========================================================================*/
void sns_smgr_fifo_configure( sns_smgr_sensor_s* sensor_ptr, bool force_wm1 )
{
  smgr_fifo_shared_sensor_s* shared_sensor_ptr;

  SNS_SMGR_PRINTF4(
    HIGH, "configure - sensor=%u state=%u curr_wm=%u force_wm1=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, 
    sensor_ptr->fifo_cfg.current_watermark, force_wm1);

  if ( sensor_ptr->fifo_cfg.can_fifo_be_used ) //could be temporary unavailable
  {
    if ( sensor_ptr->sensor_state != SENSOR_STATE_FAILED )
    {
      if ( force_wm1 )
      {
        sensor_ptr->fifo_cfg.desired_watermark = 1;
      }
      else
      {
        sns_smgr_fifo_calculate_watermarks(sensor_ptr);
      }
      if ( sensor_ptr->fifo_cfg.desired_watermark != 
           sensor_ptr->fifo_cfg.current_watermark )
      {
        sns_smgr_fifo_set_wm_and_intr(sensor_ptr);
      }
    }
    else
    {
      sensor_ptr->fifo_cfg.desired_watermark = 0;
      sns_smgr_dd_enable_sched_data(sensor_ptr, SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr), 
                                    false);
      sns_smgr_fifo_set_watermark(sensor_ptr, SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr));
    }

    SMGR_FOR_EACH_Q_ITEM(&sensor_ptr->fifo_cfg.shared_ddf_sens_q, 
                         shared_sensor_ptr, sibling_link)
    {
      sns_smgr_sensor_s* sibling_ptr = shared_sensor_ptr->sensor_ptr;
      if ( sibling_ptr->fifo_cfg.current_watermark != 
           sibling_ptr->fifo_cfg.desired_watermark )
      {
        if ( SNS_SMGR_OK_TO_CHANGE_WM(sibling_ptr) )
        {
          sns_smgr_fifo_set_wm_and_intr(sibling_ptr);
  
          SNS_SMGR_PRINTF1(
            HIGH, "configure - WM_CHANGED for sensor=%u", SMGR_SENSOR_ID(sibling_ptr));
          sns_os_mutex_pend(sibling_ptr->mutex_ptr, 0, NULL);
          SMGR_BIT_SET(sibling_ptr->flags, SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B);
          sns_os_mutex_post(sibling_ptr->mutex_ptr);
          sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
        }
        else
        {
          sibling_ptr->fifo_cfg.desired_watermark = sibling_ptr->fifo_cfg.current_watermark;
        }
      }
    }
  }
  else
  {
    SNS_SMGR_PRINTF0(
      HIGH, "configure - unable to configure FIFO, it is currently unavailable");
  }
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_flush
===========================================================================*/
/*!
  @brief Flush FIFO for the sensors.
  @details
  - This Function flushes fifo for the respective sensors.
  @param [io] sensor_ptr - the sensor leader structure
  @param [i]  force_flush - true to force flush
  @return NONE
 */
/*=========================================================================*/
void sns_smgr_fifo_flush(sns_smgr_sensor_s* sensor_ptr, bool force_flush)
{
  if ( SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) &&
       (force_flush || sensor_ptr->fifo_cfg.current_watermark > 1) )
  {
    sns_ddf_status_e ddf_status = SNS_DDF_EFAIL;

    /*Flush now, num_samples=0,return data now.*/
    if ( (SENSOR_STATE_FAILED != sensor_ptr->sensor_state) &&
         (NULL != SMGR_DRV_FN_PTR(sensor_ptr)->trigger_fifo_data) )
    {
      bool qup_clck_state = sns_hw_qup_clck_status();
      sns_ddf_sensor_e ddf_sensor_type = SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr);
      bool toggle_self_sched = false;
      sns_ddf_time_t latest_sample_ts = SMGR_PRIMARY_SENSOR(sensor_ptr)->latest_sample_ts;

      sns_hw_set_qup_clk(true);
      if ( sensor_ptr->is_self_sched_intr_enabled )
      {
        toggle_self_sched = true;
        sns_smgr_dd_enable_sched_data(sensor_ptr, ddf_sensor_type, false);
      }
      ddf_status = SMGR_DRV_FN_PTR(sensor_ptr)->trigger_fifo_data(
        sensor_ptr->dd_handle, ddf_sensor_type, 0, true );

      if( ddf_status == SNS_DDF_SUCCESS )
      {
        sns_smgr_fifo_log_event(
          SMGR_PRIMARY_SENSOR(sensor_ptr), SNS_DDF_FIFO_READ_FLUSH);
      }
      if ( toggle_self_sched )
      {
        sns_smgr_dd_enable_sched_data(sensor_ptr, ddf_sensor_type, true);
      }
      sns_hw_set_qup_clk(qup_clck_state);
      if ( latest_sample_ts != SMGR_PRIMARY_SENSOR(sensor_ptr)->latest_sample_ts )
      {
        SMGR_PRIMARY_SENSOR(sensor_ptr)->num_received_samples = 0;
        sns_rh_signal_me(SNS_RH_SAMPLE_READY_SIG);
      }
    }
  }
}

