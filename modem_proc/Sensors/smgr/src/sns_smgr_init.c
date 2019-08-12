/*=============================================================================
  @file sns_smgr_init.c

  This file contains the initialization logic for SMGR

*******************************************************************************
*   Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
*   All Rights Reserved.
*   Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*******************************************************************************

============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_init.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-11  bd   Move reserved fifo size to uimage
  2016-01-19  sc   Added TILT_EVENT and ORIENTATION_EVENT
  2015-11-12  sc   Allow for configuration with all drivers in uImage
  2015-10-27  tc   Prevent the depot allocation for INIT_FAILED sensors
  2015-10-20  bd   Added reserved fifo support
  2015-10-13  jtl  Initialized heartbeat_interval
  2015-09-24  pn   Depot size is capped at SMGR_MAX_BUFFER_SIZE
  2015-08-24  bd   - Reduced depot size to fifo size
                   - Adopted only uimg depot or big image depot for a sensor
  2015-08-19  pn   Simplified powerrails control
  2015-07-30  bd   reducing depot size due to new batching feature
  2015-07-09  ps   fix klocwork errors
  2015-06-29  agk  Added power rail changes
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2015-04-03  AH   Added Object Temperature sensor type
  2015-03-12  AH   Added Heart Rate sensor type
  2015-02-18  MW   Added UV sensor type
  2015-01-26  pn   BigImage depot size is no longer featurized
  2014-10-02  pn   Creates depot mutex for all sensors
  2014-09-30  MW   Check depot size before memory is allocated
  2014-09-25  pn   - Changed how memory is allocated and managed for sensor depots
                   - Added big-image depot to all sensors
  2014-09-15  jms  Added SPI support
  2014-09-14  vy   Used new uImage supported DDF api to free memhandler
  2014-09-11  pn   Protects all shared sensor state variables, not just client DB
  2014-08-23  vy   Removed mem allocation for unnecessary cal data structure
  2014-07-31  pn   Enabled power control; removed obsolete code
  2014-06-26  pn   - Handled configs in which Primary sensor was not present
                   - Removed obsolete HP ODR table
  2014-06-16  pn   - Buffer depths calculation based on max supported rate
                   - ODR list storage changes
  2014-05-29  pn   Set buffer depths to half-second duration at max supported rate
  2014-05-12  MW   Enhanced SNS_DDF_ATTRIB_LOWPASS handling
  2014-05-09  pn   Gets range information for each sensor type 
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_debug_api.h"
#include "sns_smgr_ddf_if.h"
#include "sns_smgr_main.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_util.h"
#include "sns_smgr_ddf_priv.h"

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define  SMGR_RETRY_CNT_DD_REG_REQ    20000

#define SMGR_BUFFER_SIZE              5
#define SMGR_MAX_BUFFER_SIZE          200

#define SMGR_RESERVED_FIFO_ACCEL      3000
#define SMGR_RESERVED_FIFO_UNCAL_MAG  600
#define SMGR_RESERVED_FIFO_PRESSURE   300
#define SMGR_RESERVED_FIFO_PROX       100
#define SMGR_RESERVED_FIFO_OTHER      0

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  smgr_sensor_id_e  sensor_id;
  uint8_t           data_type;  /* PRIMARY or SECONDARY */
  uint8_t           num_axes;   /* depending on sensor type */
  uint64_t          suid;
} smgr_sensor_depot_config_s;

typedef struct
{
  uint16_t  size;
  uint16_t  line_num;
  uint8_t   extra_info;
} smgr_heap_data_stat_s;

//#define HEAP_STAT_DBG
#ifdef HEAP_STAT_DBG
static uint8_t smgr_heap_data_items = 0;
smgr_heap_data_stat_s smgr_heap_data[50];

#define HEAP_STAT(ln,sz,ei)  \
  smgr_heap_data[smgr_heap_data_items].line_num = ln; \
  smgr_heap_data[smgr_heap_data_items].size = sz; \
  smgr_heap_data[smgr_heap_data_items].extra_info = ei; \
  smgr_heap_data_items = (smgr_heap_data_items+1) % ARR_SIZE(smgr_heap_data);
#else
#define HEAP_STAT(ln,sz,ei)
#endif

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
const smgr_sensor_depot_config_s smgr_sensor_depot_config[] =
{
  /* ID,                                 data type,                        num axes, SUID */
  {SNS_SMGR_ID_ACCEL_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ACCEL_1_V01},
  {SNS_SMGR_ID_ACCEL_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, SNS_SMGR_SUID_ACCEL_TEMP_1_V01},
  {SNS_SMGR_ID_ACCEL_2_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ACCEL_2_V01},
  {SNS_SMGR_ID_ACCEL_2_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_ACCEL_3_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ACCEL_3_V01},
  {SNS_SMGR_ID_ACCEL_3_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_ACCEL_4_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ACCEL_4_V01},
  {SNS_SMGR_ID_ACCEL_4_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_ACCEL_5_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ACCEL_5_V01},
  {SNS_SMGR_ID_ACCEL_5_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_GYRO_V01,                 SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_GYRO_1_V01},
  {SNS_SMGR_ID_GYRO_V01,                 SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, SNS_SMGR_SUID_GYRO_TEMP_1_V01},
  {SNS_SMGR_ID_MAG_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_MAG_1_V01},
  {SNS_SMGR_ID_MAG_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, SNS_SMGR_SUID_MAG_TEMP_1_V01},
  {SNS_SMGR_ID_PRESSURE_V01,             SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_PRESSURE_1_V01},
  {SNS_SMGR_ID_PRESSURE_V01,             SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, SNS_SMGR_SUID_PRESSURE_TEMP_1_V01},
  {SNS_SMGR_ID_PROX_LIGHT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   2, SNS_SMGR_SUID_PROX_1_V01},
  {SNS_SMGR_ID_PROX_LIGHT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 2, SNS_SMGR_SUID_LIGH_1_V01},
  {SNS_SMGR_ID_IR_GESTURE_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_IR_GESTURE_1_V01},
  {SNS_SMGR_ID_IR_GESTURE_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_TAP_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_DOUBLE_TAP_1_V01},
  {SNS_SMGR_ID_TAP_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_HUMIDITY_V01,             SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_HUMIDITY_1_V01},
  {SNS_SMGR_ID_HUMIDITY_V01,             SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, SNS_SMGR_SUID_AMBIENT_TEMP_1_V01},
  {SNS_SMGR_ID_RGB_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_RGB_1_V01},
  {SNS_SMGR_ID_RGB_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01, 2, SNS_SMGR_SUID_COLORTEMP_CLEAR_1_V01},
  {SNS_SMGR_ID_RGB_2_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_RGB_2_V01},
  {SNS_SMGR_ID_RGB_2_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01, 2, 0},
  {SNS_SMGR_ID_STEP_EVENT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_STEP_EVENT_1_V01},
  {SNS_SMGR_ID_STEP_EVENT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_SMD_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_SMD_1_V01},
  {SNS_SMGR_ID_SMD_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_STEP_COUNT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_STEP_COUNT_1_V01},
  {SNS_SMGR_ID_STEP_COUNT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01, SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_GRV_1_V01},
  {SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01, SNS_SMGR_DATA_TYPE_SECONDARY_V01, 3, 0},
  {SNS_SMGR_ID_SAR_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_SAR_1_V01},
  {SNS_SMGR_ID_SAR_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01, 3, 0},
  {SNS_SMGR_ID_SAR_2_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_SAR_2_V01},
  {SNS_SMGR_ID_SAR_2_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01, 3, 0},
  {SNS_SMGR_ID_HALL_EFFECT_V01,          SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_HALL_EFFECT_1_V01},
  {SNS_SMGR_ID_HALL_EFFECT_V01,          SNS_SMGR_DATA_TYPE_SECONDARY_V01, 1, 0},
  {SNS_SMGR_ID_ULTRA_VIOLET_V01,         SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_ULTRA_VIOLET_1_V01},
  {SNS_SMGR_ID_ULTRA_VIOLET_V01,         SNS_SMGR_DATA_TYPE_SECONDARY_V01, 2, SNS_SMGR_SUID_ULTRA_VIOLET_LIGHT_1_V01},
  {SNS_SMGR_ID_HEART_RATE_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_HEART_RATE_1_V01},
  {SNS_SMGR_ID_HEART_RATE_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 3, 0},
  {SNS_SMGR_ID_OBJECT_TEMP_V01,          SNS_SMGR_DATA_TYPE_PRIMARY_V01,   3, SNS_SMGR_SUID_OBJECT_TEMP_1_V01},
  {SNS_SMGR_ID_OBJECT_TEMP_V01,          SNS_SMGR_DATA_TYPE_SECONDARY_V01, 0, 0},
  {SNS_SMGR_ID_TILT_EVENT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_TILT_EVENT_1_V01},
  {SNS_SMGR_ID_TILT_EVENT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01, 0, 0},
  {SNS_SMGR_ID_ORIENTATION_EVENT_V01,    SNS_SMGR_DATA_TYPE_PRIMARY_V01,   1, SNS_SMGR_SUID_ORIENTATION_EVENT_1_V01},
  {SNS_SMGR_ID_ORIENTATION_EVENT_V01,    SNS_SMGR_DATA_TYPE_SECONDARY_V01, 0, 0},
};

sns_smgr_sensor_info_s  sns_smgr_sensor_info[SNS_SMGR_NUM_SENSORS_DEFINED];

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_smgr_find_config_entry

===========================================================================*/
/*!
  @brief Searches smgr_sensor_depot_config for the entry matching the given
         sensor type

  @param ddf_sensor_ptr - the sensor type of interest
 
  @return entry in smgr_sensor_depot_config or NULL
 */
/*=========================================================================*/
SMGR_STATIC const smgr_sensor_depot_config_s* sns_smgr_find_config_entry(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  const smgr_sensor_depot_config_s* found_cfg_ptr = NULL;
  uint8_t i;

  if ( ddf_sensor_ptr != NULL )
  {
    for ( i=0; i<ARR_SIZE(smgr_sensor_depot_config); i++ )
    {
      const smgr_sensor_depot_config_s* cfg_ptr = &smgr_sensor_depot_config[i];
      if ( (SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr) == cfg_ptr->sensor_id) &&
           (ddf_sensor_ptr->data_type == cfg_ptr->data_type) )
      {
        found_cfg_ptr = cfg_ptr;
        break;
      }
    }
  }
  return found_cfg_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_max_fifo_size

===========================================================================*/
/*!
  @brief Get max fifo size for a sensor

  @param ddf_sensor_ptr - the sensor type of interest
 
  @return Max fifo size
 */
/*=========================================================================*/
SMGR_STATIC uint16_t sns_smgr_get_max_fifo_size(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t len;
  sns_ddf_status_e ddf_status;
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  sns_ddf_fifo_attrib_get_s* fifo_attr_ptr = NULL;
  uint16_t fifo_size=0;

  if ( !SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
  {
    return 0;
  }

  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                     SNS_DDF_ATTRIB_FIFO,
                     (void**)&fifo_attr_ptr,
                     &len);
  if ( SNS_DDF_SUCCESS == ddf_status )
  {
    fifo_size = fifo_attr_ptr->max_watermark;
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
  SNS_SMGR_PRINTF3(
    MED, "max fifo size - sensor=%u/%u max_fifo=%u",
    SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr), ddf_sensor_ptr->data_type, fifo_size);
  return fifo_size;
}

/*===========================================================================

  FUNCTION:   sns_smgr_compute_buffer_size

===========================================================================*/
/*!
  @brief Computes the depth and memory size required for sample buffer
         of the given sensor type.

  @param ddf_sensor_ptr - the sensor type of interest
 
  @return none
 */
/*=========================================================================*/

SMGR_STATIC void sns_smgr_compute_buffer_size(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_smgr_depot_info_s* depot_info_ptr = NULL;
  sns_ddf_odr_t max_rate = 
    DDF_FIXTOODR_CEIL_Q16(ddf_sensor_ptr->max_supported_freq_hz);
  
  if ( ddf_sensor_ptr->sensor_ptr->is_uimg_refac )
  {
    depot_info_ptr = &ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info;
    ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.depth = 0;
    ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.mem_size = 0;
  }
  else
  {
    depot_info_ptr = &ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info;
    ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.depth = 0;
    ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.mem_size = 0;
  }
  depot_info_ptr->depth = sns_smgr_get_max_fifo_size(ddf_sensor_ptr);
  if ( depot_info_ptr->depth == 0 )
  {
    depot_info_ptr->depth = SMGR_BUFFER_SIZE;
  }
  depot_info_ptr->depth = MIN(depot_info_ptr->depth, SMGR_MAX_BUFFER_SIZE);

  /* calculate memory size taking care of memory alignment */
  depot_info_ptr->mem_size =
    (sizeof(sns_smgr_sample_depot_s) + 
     sizeof(sns_smgr_sample_s) * (depot_info_ptr->depth-1) + 
     sizeof(intptr_t)-1) / sizeof(intptr_t) * sizeof(intptr_t);

  SNS_SMGR_PRINTF3(
    MED, "compute_buf_size - sensor=%u/%u max_rate=%u",
    SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr), ddf_sensor_ptr->data_type, max_rate);
  SNS_SMGR_PRINTF3(
    LOW, "compute_buf_size - is uimg:%u, depth:%u, memsize:%u", 
    ddf_sensor_ptr->sensor_ptr->is_uimg_refac, 
    depot_info_ptr->depth, depot_info_ptr->mem_size);
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_depot

===========================================================================*/
/*!
  @brief 

  @param none
 
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_depot(
  const smgr_sensor_depot_config_s* cfg_ptr,
  const sns_smgr_depot_info_s*      depot_info_ptr,
  sns_smgr_sample_depot_s*          depot_ptr)
{
  depot_ptr->num_axes = cfg_ptr->num_axes;
  depot_ptr->max_bufs = depot_info_ptr->depth;
  depot_ptr->last_idx = depot_ptr->max_bufs - 1;
}

/*===========================================================================

  FUNCTION:   sns_smgr_assign_depot_mem

===========================================================================*/
/*!
  @brief Assigns appropriately sized blocks of preallocated memory to
         individual sensor type.

  @param none
 
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_assign_depot_mem(void)
{
  uint8_t s, d;
  intptr_t ui_free_ptr = sns_smgr.uimg_depot_mem_ptr;
  intptr_t bi_free_ptr = sns_smgr.bimg_depot_mem_ptr;

  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    sns_smgr_sensor_s *sensor_ptr = &sns_smgr.sensor[s];
    for ( d=0; d<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); d++ )
    {
      sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
      const smgr_sensor_depot_config_s* cfg_ptr = 
        sns_smgr_find_config_entry(ddf_sensor_ptr);
      SNS_SMGR_PRINTF3(
        HIGH, "assign_depot_mem - i=%u ddf_sensor_ptr=0x%x cfg_ptr=0x%x", 
        d, ddf_sensor_ptr, cfg_ptr);
      if ( ddf_sensor_ptr != NULL && cfg_ptr != NULL )
      {
        uint8_t err;
        ddf_sensor_ptr->depot_mutex_ptr = 
          sns_os_mutex_create_uimg(SNS_SMGR_QUE_MUTEX, &err);
        SNS_ASSERT (err == OS_ERR_NONE && ddf_sensor_ptr->depot_mutex_ptr != NULL);

        if ( ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.mem_size > 0 )
        {
          ddf_sensor_ptr->uimg_depot_ptr = (sns_smgr_sample_depot_s*)ui_free_ptr;
          ui_free_ptr += ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.mem_size;
          sns_smgr_init_depot(cfg_ptr, 
                              &ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info, 
                              ddf_sensor_ptr->uimg_depot_ptr);
        }
        else if ( ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.mem_size > 0 )
        {
          ddf_sensor_ptr->bimg_depot_ptr = (sns_smgr_sample_depot_s*)bi_free_ptr;
          bi_free_ptr += ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.mem_size;
          sns_smgr_init_depot(cfg_ptr, 
                              &ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info, 
                              ddf_sensor_ptr->bimg_depot_ptr);
        }
        else
        {
          SNS_SMGR_PRINTF0(ERROR, "assign_depot_mem - no depot assigned");
        }
        ddf_sensor_ptr->ddf_sensor_info->suid = cfg_ptr->suid;
      }
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_allocate_depot_mem

===========================================================================*/
/*!
  @brief Calculates and allocates enough memory for all sensor depots

  @details Calculates memory size required for sample depots of all
           configured sensors and allocates two memory blocks, one from
           uHeap, the other from big-image heap.  Each block will be divided
           up into smaller blocks for individual sensors.

  @param none
 
  @return true if all needed memory is allocated, false otherwise
 */
/*=========================================================================*/
SMGR_STATIC bool sns_smgr_allocate_depot_mem(void)
{
  bool success = false;
  uint32_t ui_samples = 0;
  uint32_t bi_samples = 0;
  size_t   ui_size    = 0;
  size_t   bi_size    = 0;
  uint8_t  s, d;

  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    sns_smgr_sensor_s *sensor_ptr = &sns_smgr.sensor[s];
    for ( d=0; d<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); d++ )
    {
      sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
      if ( (ddf_sensor_ptr != NULL) && (SENSOR_INIT_SUCCESS == sensor_ptr->init_state) )
      {
        sns_smgr_compute_buffer_size(ddf_sensor_ptr);
        ui_samples += ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.depth;
        bi_samples += ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.depth;
        ui_size += ddf_sensor_ptr->ddf_sensor_info->uimg_depot_info.mem_size;
        bi_size += ddf_sensor_ptr->ddf_sensor_info->bimg_depot_info.mem_size;
      }
    }
  }

  SNS_SMGR_PRINTF4(
    HIGH, "allocate_depot_mem - ui=%u/%u bi=%u/%u", 
    ui_samples, ui_size, bi_samples, bi_size);

  if(ui_size > 0)
  {
    sns_smgr.uimg_depot_mem_ptr = (intptr_t)SNS_OS_U_MALLOC(SNS_DBG_MOD_DSPS_SMGR, ui_size);
  }
  
  if(bi_size > 0)
  {
    sns_smgr.bimg_depot_mem_ptr = (intptr_t)SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, bi_size);
  }
  
  if ( ((sns_smgr.uimg_depot_mem_ptr != (intptr_t)NULL) || (ui_size == 0))&&
       ((sns_smgr.bimg_depot_mem_ptr != (intptr_t)NULL) || (bi_size == 0)) )
  {
    success = true;
    sns_smgr_assign_depot_mem();
  }
  else
  {
    SNS_SMGR_PRINTF2(
      ERROR, "allocate_depot_mem - udepot_ptr=0x%x depot_mem=0x%x", 
      sns_smgr.uimg_depot_mem_ptr, sns_smgr.bimg_depot_mem_ptr);
    if ( sns_smgr.uimg_depot_mem_ptr != (intptr_t)NULL )
    {
      SNS_OS_U_FREE((void*)sns_smgr.uimg_depot_mem_ptr);
      sns_smgr.uimg_depot_mem_ptr = (intptr_t)NULL;
    }
    if ( sns_smgr.bimg_depot_mem_ptr != (intptr_t)NULL )
    {
      SNS_OS_FREE((void*)sns_smgr.bimg_depot_mem_ptr);
      sns_smgr.bimg_depot_mem_ptr = (intptr_t)NULL;
    }
  }

  return success;
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_odr_supported

===========================================================================*/
/*!
  @brief Initializes odr_supported field of given sensor.
  @param [io] ddf_sensor_ptr - the sensor of interest
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_odr_supported(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t len;
  void* odr_ptr;
  sns_ddf_status_e ddf_status;
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;

  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
                     SNS_DDF_ATTRIB_ODR,
                     &odr_ptr,
                     &len);
  if (SNS_DDF_SUCCESS == ddf_status )
  {
    sensor_ptr->odr_attr_supported = true;
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_lpf_table

===========================================================================*/
/*!
  @brief Initializes lpf_table field of given sensor.

  @param sensor_ptr the sensor of interest
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_lpf_table(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t len;
  sns_ddf_lowpass_freq_t* lpf_ptr;
  sns_ddf_status_e ddf_status;
  sns_ddf_sensor_e sensor_type =
    SMGR_SENSOR_TYPE(sensor_ptr, sensor_ptr->const_ptr->range_sensor);

  /* Load definition of low-pass filters if they exist for this device.
     Some device drivers use LOWPASS attribute to pass internal sampling
     rate, but the effect is to control filtering */
  sensor_ptr->num_lpf = 0;
  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     sensor_type,
                     SNS_DDF_ATTRIB_LOWPASS,
                     (void**)&lpf_ptr,
                     &len);
  SNS_SMGR_PRINTF2(HIGH, "init_lpf_table - sensor=%d status=%u", 
                   sensor_type, ddf_status);

  if (SNS_DDF_SUCCESS == ddf_status )
  {
    uint8_t ij;
    if ( (len <= ARR_SIZE(sensor_ptr->lpf_table)) && (len > 0) )
    {
      int16_t  delta;
      for ( ij = 0; ij < len; ij++, lpf_ptr++ )
      {
         sensor_ptr->lpf_table[ij] = *lpf_ptr;
      }
      /* Ensure table changes monotonically. Disregard portion of table
         that doesn't */
      delta = (int16_t)(sensor_ptr->lpf_table[1] - sensor_ptr->lpf_table[0]);
      for ( ij = 1; ij < len; ij++ )
      {
        int16_t delta_ij =
          (int16_t)(sensor_ptr->lpf_table[ij] - sensor_ptr->lpf_table[ij-1]);
        if ( (delta_ij * delta) <= 0 )
        {
          /* Reached a point in the table where delta is not monotonic.
             Limit length of usable table */
          break;
        }
      }
      sensor_ptr->num_lpf = (uint8_t)len;
    }
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_build_odr_list

===========================================================================*/
/*!
  @brief Builds the list of ODRs the given sensor can support

  @param [io] ddf_sensor_ptr - the sensor of interest
  @param [o]  odr_list       - destination for the ODR list

  @return  number of ODRs on the list
 */
/*=========================================================================*/
SMGR_STATIC uint32_t smgr_build_odr_list(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_ddf_odr_t*         odr_list)
{
  uint32_t len = 0;
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;

  if ( !sensor_ptr->odr_attr_supported )
  {
    /* build ODR table from LPF table */
    for ( len=0; len<sensor_ptr->num_lpf && len<ARR_SIZE(sensor_ptr->lpf_table); len++)
    {
      if ( SMGR_BIT_CLEAR_TEST(sensor_ptr->const_ptr->flags,
                               SNS_SMGR_SENSOR_LPF_FROM_RATE) )
      {
        odr_list[len] = FX_FIXTOFLT_Q16(sensor_ptr->lpf_table[len] << 1); /* BW * 2 */
      }
      else
      {
        odr_list[len] = FX_FIXTOFLT_Q16(sensor_ptr->lpf_table[len]);
      }
      SNS_SMGR_PRINTF3(LOW, "build_odr_list - lpf[%u]=%x odr=%u",
                       len, sensor_ptr->lpf_table[len], odr_list[len]);
    }
  }
  else
  {
    /* build ODR table by trial-and-error */
    uint32_t odr, dont_care;
    uint32_t* set_odr_ptr;
    uint32_t set_data = (uint32_t)SNS_DDF_POWERSTATE_ACTIVE;
    uint32_t max_supp_freq = 
      DDF_FIXTOODR_CEIL_Q16(ddf_sensor_ptr->max_supported_freq_hz);
    sns_ddf_sensor_e sensor_type = 
      SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type);

    sns_smgr_set_attr(sensor_ptr,
                      SNS_DDF_SENSOR__ALL,
                      SNS_DDF_ATTRIB_POWER_STATE,
                      &set_data);
    odr = 1;
    while ( odr <= max_supp_freq )
    {
      /* set ODR then get back the ODR actually set by driver */
      if ( (sns_smgr_set_attr(sensor_ptr,
                              sensor_type,
                              SNS_DDF_ATTRIB_ODR,
                              &odr) == SNS_DDF_SUCCESS) &&
           (sns_smgr_get_attr(sensor_ptr,
                              sensor_type,
                              SNS_DDF_ATTRIB_ODR,
                              (void**)&set_odr_ptr,
                              &dont_care) == SNS_DDF_SUCCESS) &&
           ((len == 0) || (*set_odr_ptr > odr_list[len-1])) )
      {
        odr_list[len++] = *set_odr_ptr;
        odr = (*set_odr_ptr) + 1;
      }
      else
      {
        break;
      }
    }
    set_data = (uint32_t)SNS_DDF_POWERSTATE_LOWPOWER;
    sns_smgr_set_attr(sensor_ptr,
                      SNS_DDF_SENSOR__ALL,
                      SNS_DDF_ATTRIB_POWER_STATE,
                      &set_data);
  }
  return len;
}


/*===========================================================================

  FUNCTION:   sns_smgr_init_odr_table

===========================================================================*/
/*!
  @brief Initializes normal_odr field of given sensor.
  @param [io] sensor_ptr - the sensor of interest
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_odr_table(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t       len = 0, size;
  sns_ddf_odr_t* odr_list = NULL;
  sns_ddf_odr_t* alloc_odr_list = NULL;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = NULL;
  sns_ddf_sensor_e sensor_type = SNS_DDF_SENSOR__NONE;

  SNS_SMGR_PRINTF2(
    HIGH, "init_odr_tables - type0=%u type1=%u",
    SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr), SMGR_SENSOR_TYPE_SECONDARY(sensor_ptr) );

  if ( SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr) != SNS_DDF_SENSOR__NONE )
  {
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01];
    sensor_type = SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr);
  }
  else if ( SMGR_SENSOR_TYPE_SECONDARY(sensor_ptr) != SNS_DDF_SENSOR__NONE )
  {
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_SECONDARY_V01];
    sensor_type = SMGR_SENSOR_TYPE_SECONDARY(sensor_ptr);
  }

  if ( ddf_sensor_ptr == NULL )
  {
    SNS_SMGR_PRINTF0(ERROR, "init_odr_tables - no Primary nor Secondary!");
    return;
  }

  sns_smgr_init_odr_supported(ddf_sensor_ptr);

  if ( (sensor_ptr->num_lpf == 0) && !sensor_ptr->odr_attr_supported )
  {
    return;
  }

  if ( sns_smgr_get_attr(sensor_ptr,
                         sensor_type,
                         SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST,
                         (void**)&odr_list,
                         &len) != SNS_DDF_SUCCESS )
  {
    // TODO: let smgr_build_odr_list do the allocation to prevent buffer overflows
    size = sizeof(sns_ddf_odr_t) * SNS_SMGR_MAX_SUPPORTED_ODR_NUM_V01;
    alloc_odr_list = odr_list = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, size);
  }
  if ( (odr_list != NULL) && (len == 0) )
  {
    /* SUPPORTED_ODR_LIST not supported; build the list by other means */
    len = smgr_build_odr_list(ddf_sensor_ptr, odr_list);
  }
  if ( (odr_list != NULL) && (len != 0) )
  {
    size = sizeof(*sensor_ptr->supported_odrs) * len;
    sensor_ptr->supported_odrs = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, size);
    if ( sensor_ptr->supported_odrs != NULL )
    {
      SNS_OS_MEMSCPY(sensor_ptr->supported_odrs, size, odr_list, size);
      sensor_ptr->num_odrs = len;
    }
  }
  if ( (odr_list != NULL) && (sensor_ptr->supported_odrs != NULL) )
  {
    uint32_t  i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
      {
        uint8_t k;
        /* make sure max supported frequency falls within the table */
        sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz = 
          MIN(sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz, 
              DDF_ODRTOFIX_Q16(odr_list[len-1]));

        for ( k=0; k<len && 
                   k<ARR_SIZE(sensor_ptr->ddf_sensor_ptr[i]->
                              ddf_sensor_info->odr_list); k++ )
        {
          sensor_ptr->ddf_sensor_ptr[i]->ddf_sensor_info->odr_list[k] = odr_list[k];
        }
        sensor_ptr->ddf_sensor_ptr[i]->ddf_sensor_info->num_supported_odrs = k;
      }
    }
  }
  /* clean up */
  if ( alloc_odr_list != NULL )
  {
    SNS_OS_FREE(alloc_odr_list);
  }
  sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_max_frequency

===========================================================================*/
/*!
  @brief Initializes max_freq field of given sensor.

  @param sensor_ptr - the sensor of interest
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_max_frequency(sns_smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
    {
      uint32_t len;
      sns_ddf_resolution_adc_s* resolution_adc_ptr;
      sns_ddf_status_e ddf_status =
        sns_smgr_get_attr(sensor_ptr,
                          SMGR_SENSOR_TYPE(sensor_ptr, i),
                          SNS_DDF_ATTRIB_RESOLUTION_ADC,
                          (void**)&resolution_adc_ptr,
                          &len);
      if ( SNS_DDF_SUCCESS == ddf_status )
      {
        uint8_t j;
        for ( j=0; j<len; j++ )
        {
          sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz =
            MAX(sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz,
                FX_CONV_Q16(resolution_adc_ptr[j].max_freq,0));
        }
        sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_sensor_name_vendor_version

===========================================================================*/
/*!
  @brief This function gets sensor name, vendor, version from DDF

  @param[io] dsi_ptr - the sensor info structure

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_get_sensor_name_vendor_version(
  sns_smgr_ddf_sensor_info_s* dsi_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = dsi_ptr->ddf_sensor_ptr->sensor_ptr;
  sns_ddf_device_info_s *dev_info_ptr;
  uint32_t len;

  if ( sns_smgr_get_attr(
         sensor_ptr,
         SMGR_SENSOR_TYPE(sensor_ptr, dsi_ptr->ddf_sensor_ptr->data_type),
         SNS_DDF_ATTRIB_DEVICE_INFO,
         (void**)&dev_info_ptr,
         &len) == SNS_DDF_SUCCESS )
  {
    if (strlen(dev_info_ptr->model) + strlen(dev_info_ptr->name) + 1 < 
        sizeof(dsi_ptr->name))
    {
      snprintf(dsi_ptr->name, sizeof(dsi_ptr->name),
               "%s %s", dev_info_ptr->model, dev_info_ptr->name);
    }
    else
    {
      strlcpy(dsi_ptr->name, dev_info_ptr->name, sizeof(dsi_ptr->name));
    }

    strlcpy(dsi_ptr->vendor, dev_info_ptr->vendor, sizeof(dsi_ptr->vendor));
    dsi_ptr->version = dev_info_ptr->version;
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_sensor_power

===========================================================================*/
/*!
  @brief This function gets sensor power info from DDF

  @param[io] dsi_ptr - the sensor info structure

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_get_sensor_power(sns_smgr_ddf_sensor_info_s* dsi_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = dsi_ptr->ddf_sensor_ptr->sensor_ptr;
  sns_ddf_power_info_s* power_info_ptr;
  uint32_t len;

  if ( sns_smgr_get_attr(
         sensor_ptr,
         SMGR_SENSOR_TYPE(sensor_ptr, dsi_ptr->ddf_sensor_ptr->data_type),
         SNS_DDF_ATTRIB_POWER_INFO,
         (void**)&power_info_ptr,
         &len) == SNS_DDF_SUCCESS )
  {
    dsi_ptr->idle_power   = power_info_ptr->lowpower_current;
    dsi_ptr->active_power = power_info_ptr->active_current;
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_sensor_resolution

===========================================================================*/
/*!
  @brief This function gets sensor resolution info from DDF

  @param[io] dsi_ptr - the sensor info structure

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_get_sensor_resolution(
  sns_smgr_ddf_sensor_info_s* dsi_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = dsi_ptr->ddf_sensor_ptr->sensor_ptr;
  sns_ddf_resolution_t* resolution_ptr;
  uint32_t len;

  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(
         sensor_ptr,
         SMGR_SENSOR_TYPE(sensor_ptr, dsi_ptr->ddf_sensor_ptr->data_type),
         SNS_DDF_ATTRIB_RESOLUTION,
         (void**)&resolution_ptr,
         &len) == SNS_DDF_SUCCESS )
  {
    dsi_ptr->resolution = *resolution_ptr;
  }
  sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_sensor_reserved_fifo_size

===========================================================================*/
/*!
  @brief This function inits sensor reserved fifo size

  @param[io] ddf_sensor_ptr - the sensor structure

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_sensor_reserved_fifo_size(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  sns_ddf_sensor_e sensor_type = 
      SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type);
  switch ( sensor_type )
  {
    case SNS_DDF_SENSOR_ACCEL:
      ddf_sensor_ptr->reserved_sample_cnt_nonwakeup = 
        SMGR_RESERVED_FIFO_ACCEL;
      break;
    case SNS_DDF_SENSOR_MAG:
      ddf_sensor_ptr->reserved_sample_cnt_nonwakeup = 
        SMGR_RESERVED_FIFO_UNCAL_MAG;
      break;
    case SNS_DDF_SENSOR_PRESSURE:
      ddf_sensor_ptr->reserved_sample_cnt_nonwakeup = 
        SMGR_RESERVED_FIFO_PRESSURE;
      break;
    case SNS_DDF_SENSOR_PROXIMITY:
      ddf_sensor_ptr->reserved_sample_cnt_nonwakeup = 
        SMGR_RESERVED_FIFO_PROX;
      break;
    default:
      ddf_sensor_ptr->reserved_sample_cnt_nonwakeup = 
        SMGR_RESERVED_FIFO_OTHER;
      break;
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_get_sensor_range

===========================================================================*/
/*!
  @brief This function gets sensor range from DDF

  @param[io] dsi_ptr - the sensor info structure

  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_get_sensor_range(sns_smgr_ddf_sensor_info_s* dsi_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = dsi_ptr->ddf_sensor_ptr->sensor_ptr;
  uint32_t len;
  sns_ddf_range_s* range_ptr;

  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(
         sensor_ptr,
         SMGR_SENSOR_TYPE(sensor_ptr, dsi_ptr->ddf_sensor_ptr->data_type),
         SNS_DDF_ATTRIB_RANGE,
         (void**)&range_ptr,
         &len) == SNS_DDF_SUCCESS &&
       len != 0 )
  {
    if ( (sensor_ptr->const_ptr->flags & SNS_SMGR_NO_SENSITIVITY) == 0 ||
         (sensor_ptr->const_ptr->sensitivity_default >= len) )
    {
      dsi_ptr->max_range = range_ptr[len-1].max;
    }
    else
    {
      dsi_ptr->max_range = range_ptr[sensor_ptr->const_ptr->sensitivity_default].max;
    }
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_fifo_cfg

===========================================================================*/
/*!
  @brief Initializes the fifo configuration of given sensor.
  @param[in/out] sensor_ptr - the sensor of interest
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_fifo_cfg(sns_smgr_sensor_s *sensor_ptr)
{
  sns_smgr_fifo_init_fifo_cfg( sensor_ptr );
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_sensor_info

===========================================================================*/
/*!
  @brief Initializes sensor_info for the given DDF sensor

  @param [i] ddf_sensor_ptr

  @return
   NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_init_sensor_info(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t d;

  for ( d=0; d<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); d++ )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr;
    sns_smgr_ddf_sensor_info_s* dsi_ptr;

    if ( sensor_ptr->ddf_sensor_ptr[d] == NULL )
    {
      continue;
    }

    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
    dsi_ptr = ddf_sensor_ptr->ddf_sensor_info;

    dsi_ptr->ddf_sensor_ptr    = ddf_sensor_ptr;
    dsi_ptr->max_sampling_rate = 
      (uint16_t) DDF_FIXTOODR_CEIL_Q16(ddf_sensor_ptr->max_supported_freq_hz);
    sns_smgr_get_sensor_name_vendor_version(dsi_ptr);
    sns_smgr_get_sensor_power(dsi_ptr);
    sns_smgr_get_sensor_resolution(dsi_ptr);
    sns_smgr_get_sensor_range(dsi_ptr);
    sns_smgr_init_sensor_reserved_fifo_size(ddf_sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_init_post_cfg

===========================================================================*/
/*!
  @brief continue initialize smgr_sensor_s structure

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_sensor_init_post_cfg(void)
{
  sns_smgr_sensor_s  *sensor_ptr;
  uint32_t            ix, i;

  SNS_OS_MEMZERO(sns_smgr_sensor_info, sizeof(sns_smgr_sensor_info));
  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    
    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
      {
        sns_smgr_ddf_sensor_s*  ddf_sensor_ptr;
        sns_smgr_client_stat_s* cs_ptr;

        sensor_ptr->ddf_sensor_ptr[i] = SMGR_ANY_ALLOC_STRUCT(sns_smgr_ddf_sensor_s);
        SNS_ASSERT(sensor_ptr->ddf_sensor_ptr[i] != NULL);
        HEAP_STAT(__LINE__, sizeof(sns_smgr_ddf_sensor_s),
                  sensor_ptr->const_ptr->sensor_id);

        ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
        SNS_OS_MEMZERO(ddf_sensor_ptr, sizeof(sns_smgr_ddf_sensor_s));
        ddf_sensor_ptr->data_type  = i;
        ddf_sensor_ptr->sensor_ptr = sensor_ptr;
        ddf_sensor_ptr->device_sampling_factor = 1;
        ddf_sensor_ptr->heartbeat_interval = SMGR_MAX_TICKS;
        sns_q_link(ddf_sensor_ptr, &ddf_sensor_ptr->sched_link);

        cs_ptr = &ddf_sensor_ptr->client_stat;
        cs_ptr->sensor_status.ddf_sensor_ptr = ddf_sensor_ptr;
        sns_q_init(&cs_ptr->sensor_status.request_queue);
        cs_ptr->sensor_status.last_status_ts = sns_ddf_get_timestamp();

        ddf_sensor_ptr->ddf_sensor_info = &sns_smgr_sensor_info[ix].ddf_sensor_info[i];

        if ( i == SNS_SMGR_DATA_TYPE_PRIMARY_V01 )
        {
          ddf_sensor_ptr->all_cal_ptr = NULL;
          if ( (sensor_ptr->const_ptr->data_types[i] == SNS_DDF_SENSOR_ACCEL) 
               || (sensor_ptr->const_ptr->data_types[i] == SNS_DDF_SENSOR_GYRO) 
               || (sensor_ptr->const_ptr->data_types[i] == SNS_DDF_SENSOR_MAG)
               || (ddf_sensor_ptr->sensor_ptr->const_ptr->primary_cal_reg_type != SNS_SMGR_REG_ITEM_TYPE_NONE) )
          {
            ddf_sensor_ptr->all_cal_ptr =
               SNS_OS_ANY_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(sns_smgr_all_cal_s));
            SNS_ASSERT(ddf_sensor_ptr->all_cal_ptr != NULL);
            sns_smgr_load_default_cal(&ddf_sensor_ptr->all_cal_ptr->factory_cal);
            sns_smgr_load_default_cal(&ddf_sensor_ptr->all_cal_ptr->auto_cal);
            sns_smgr_load_default_cal(&ddf_sensor_ptr->all_cal_ptr->full_cal);
          }
        }
        sensor_ptr->num_data_types++;
      }
    } /* for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ ) */
    if ( sensor_ptr->num_data_types != 0 )
    {
      uint8_t err;
      sensor_ptr->mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
      SNS_ASSERT (err == OS_ERR_NONE && sensor_ptr->mutex_ptr != NULL);

      sns_q_init(&sensor_ptr->request_queue.protected_q);
      sensor_ptr->request_queue.mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
      SNS_ASSERT (err == OS_ERR_NONE && sensor_ptr->request_queue.mutex_ptr != NULL);
    }
  } /*   for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ ) */
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_init

===========================================================================*/
/*!
  @brief initialize smgr_sensor_s structure

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_sensor_init( void )
{
  uint32_t ix;

  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[ix];
    sensor_ptr->const_ptr = &smgr_sensor_cfg[ix];
    sensor_ptr->reg_item_param.nvitem_grp = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_ptr->reg_item_param.status = SNS_DDF_EFAIL;
    sns_ddf_memhandler_init( &(sensor_ptr->memhandler) );
  }

#ifdef ADSP_STANDALONE
  sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
#else
  sns_smgr.all_init_state = SENSOR_ALL_INIT_NOT_STARTED;
#endif

  sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
}

/*===========================================================================

  FUNCTION:   sns_smgr_dd_init

===========================================================================*/
/*!
  @brief Initialize/install all device drivers

  @detail  Any device driver may be waiting
    for registry data or timer part way through the initialization process, in
    which case, the next driver initialization may be started. A device driver
    issues a sns_ddf_event_init when finished, the status indicates success
    or failure. Timeout of the multi-device initialization process causes
    failure of any devices that have not completed initialization.
  @param Cal_p Calibration pointer
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_dd_init( void )
{
  sns_smgr_sensor_s            *sensor_ptr;
  const smgr_sensor_cfg_s      *sensor_cfg_ptr;
  sns_ddf_sensor_e             *sensor_type_ptr;
  sns_ddf_sensor_e              sensor_type[SMGR_MAX_DATA_TYPES_PER_DEVICE];
  sns_ddf_status_e              drv_status;
  static uint32_t               ix;
  uint32_t                      i;
  uint32_t                      num_sensor_types;
  sns_ddf_device_access_s       dev_access;

  SNS_SMGR_PRINTF1(MED, "dd_init - all_init_state(%u)", sns_smgr.all_init_state);

  if ( sns_smgr.all_init_state != SENSOR_ALL_INIT_CONFIGURED &&
       sns_smgr.all_init_state != SENSOR_ALL_INIT_IN_PROGRESS )
  {
    return;
  }

  if ( sns_smgr.all_init_state == SENSOR_ALL_INIT_CONFIGURED )
  {
    uint16_t off_to_idle_max = 0;

    sns_smgr.all_init_state = SENSOR_ALL_INIT_IN_PROGRESS;
    sns_smgr.init_start_tick = sns_ddf_get_timestamp();
    sns_smgr_sensor_init_post_cfg();
    ix = 0;
    /* as all configuration info is available now, request the calibration data to REG module */
    smgr_send_reg_message_for_fac_cal();

    /* Initialize SMGR internal power rail structure and call pm rail init for each rail */
    sns_hw_init_pwrrails();
    sns_hw_all_powerrails_config(SNS_PWR_RAIL_ON_NPM);
    sns_hw_set_qup_clk(true);
    for ( i = 0; i < SNS_SMGR_NUM_SENSORS_DEFINED; i++ )
    {
      off_to_idle_max = 
        MAX(sns_smgr.sensor[i].const_ptr->off_to_idle_time, off_to_idle_max);
    }
    SMGR_DELAY_US(sns_em_convert_dspstick_to_usec(off_to_idle_max));
  }

  for ( ; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_cfg_ptr = sensor_ptr->const_ptr;
    sensor_type_ptr = sensor_type;

    /* Setup access to driver function list */
    if ( (NULL == SMGR_DRV_FN_PTR(sensor_ptr)) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->init) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->reset) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->get_data) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->set_attrib) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->get_attrib) )
    {
      /* Driver function list not defined */
      sensor_ptr->init_state = SENSOR_INIT_FAIL;
      continue;
    }

    num_sensor_types = 0;
    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      if ( (i == 0) /* always count Primary whether it is used or not */ ||
           ((sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE) &&
            (sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__ALL)) )
      {
        num_sensor_types++;
        sensor_type[i] = sensor_ptr->const_ptr->data_types[i];
      }
    }
    if ( SENSOR_INIT_NOT_STARTED == sensor_ptr->init_state ||
         SENSOR_INIT_WAITING_TIMER == sensor_ptr->init_state ||
         SENSOR_INIT_REG_READY == sensor_ptr->init_state )
    {
#if !defined SNS_PCSIM && !defined ADSP_STANDALONE
      /* Request registry data for driver if not already requested.
         If driver registry data does not arrive by timeout, the init fails.
       */
      if ( SNS_SMGR_REG_ITEM_TYPE_NONE != sensor_cfg_ptr->driver_reg_type &&
           SENSOR_INIT_WAITING_REG > sensor_ptr->init_state )
      {
         sns_err_code_e err = SNS_ERR_FAILED;
         uint32_t i;

         for (i=0; i<SMGR_RETRY_CNT_DD_REG_REQ && err!=SNS_SUCCESS; i++)
         {
            err = sns_smgr_req_reg_data(sensor_cfg_ptr->driver_reg_id,
                                        sensor_cfg_ptr->driver_reg_type);
            if ( SNS_SUCCESS != err )
            {
              SMGR_DELAY_US(1000); /* 1 ms */
            }
         }

         if ( SNS_SUCCESS != err )
         {
           SNS_SMGR_PRINTF0(FATAL, "dd_init - error reading registry!");
           sensor_ptr->init_state = SENSOR_INIT_FAIL;
         }
         else
         {
            /* Request message sent */
            sensor_ptr->init_state = SENSOR_INIT_WAITING_REG;
         }
      }
#endif /* SNS_PCSIM  || ADSP_STANDALONE */
      if ( SENSOR_INIT_WAITING_REG == sensor_ptr->init_state ||
           SENSOR_INIT_FAIL == sensor_ptr->init_state )
      {
        /* Skip init if waiting for registry or failed. We will loop through
           here again */
        continue;
      }


      //---------- Init bus configuration for communication interface ----------
#ifdef ADSP_HWCONFIG_L
      sns_hw_int_gpio_inactive_config(sensor_cfg_ptr->first_gpio);
      dev_access.flags = sensor_cfg_ptr->flags;
#endif
      dev_access.port_config.bus = SNS_DDF_BUS_NONE;
      dev_access.device_select   = sensor_cfg_ptr->device_select;
      dev_access.first_gpio      = (uint32_t)sensor_cfg_ptr->first_gpio;
      dev_access.second_gpio     = (uint32_t)sensor_cfg_ptr->second_gpio;

      drv_status = sns_ddf_comm_bus_init_cfg( &dev_access,
                                              sensor_cfg_ptr->bus_instance,
                                              sensor_cfg_ptr->bus_slave_addr );
      if (  SNS_DDF_SUCCESS != drv_status )
      {
        SNS_SMGR_PRINTF1( HIGH, "ddf init bus failed status=%d", drv_status );
        continue;
      }
      //------------------------------------------------------------------------

      sns_ddf_memhandler_init( &(sensor_ptr->memhandler) );

      drv_status = SMGR_DRV_FN_PTR(sensor_ptr)->init(
                                      &sensor_ptr->dd_handle,
                                      sensor_ptr,
                                      &(sensor_ptr->reg_item_param),
                                      &dev_access,
                                      1,        /* Num devices on this driver */
                                      &(sensor_ptr->memhandler),
                                      &sensor_type_ptr,
                                      &num_sensor_types);

      sns_ddf_comm_bus_free_cfg( &dev_access ); //clean mem after bus configuration

      if ( 0 != sensor_ptr->reg_item_param.data )
      {
        /* Registry data received for device driver. Release copy */
        SNS_OS_FREE( sensor_ptr->reg_item_param.data );
        sensor_ptr->reg_item_param.data = 0;
      }

      if ( (SNS_DDF_PENDING != drv_status) && ( SNS_DDF_SUCCESS != drv_status ) )
      {
        SNS_SMGR_PRINTF3(
          FATAL, "dd_init - sensor=%u #types=%u drv_status=%u", 
          SMGR_SENSOR_ID(sensor_ptr), num_sensor_types, drv_status);
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        continue;
      }

      /* Setup for get_data to retrieve all data types (known to DD as sensors) */
      /* Limit num_sensors to space available */
      if ( sensor_ptr->num_data_types > (uint8_t)num_sensor_types )
      {
        /* device supports fewer types than configured */
        SNS_SMGR_PRINTF2(
          FATAL, "dd_init - device expects more types (%u/%u)!",
          sensor_ptr->num_data_types, num_sensor_types);
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        continue;
      }
      for ( i=0; i<sensor_ptr->num_data_types &&
                  sensor_ptr->init_state != SENSOR_INIT_FAIL; i++ )
      {
        if ( sensor_ptr->const_ptr->data_types[i] != sensor_type_ptr[i] )
        {
          SNS_SMGR_PRINTF3(
            FATAL, "dd_init - mismatched i=%u reg=%u driver=%u",
            i, sensor_ptr->const_ptr->data_types[i], sensor_type_ptr[i]);
          sensor_ptr->init_state = SENSOR_INIT_FAIL;
        }
      }
      /* TODO: If the sensor supports temperature, add that here even if it's
       * not in the initial configuration */
      if ( sensor_ptr->init_state == SENSOR_INIT_FAIL )
      {
        continue;
      }
      if ( SNS_DDF_PENDING == drv_status )
      {
        /* Come back again if driver is pending */
        sensor_ptr->init_state = SENSOR_INIT_WAITING_TIMER;
        continue;
      }
      sensor_ptr->init_state = SENSOR_INIT_SUCCESS;

      /* knowledge of max frequency is needed for depot creation */
      sns_smgr_init_max_frequency(sensor_ptr);

    }   /* END - if ( SENSOR_INIT_NOT_STARTED == sensor_ptr->init_state || */
  }   /* END - for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ ) */

  /* Assume all drivers have completed init */
  sns_smgr.all_init_state = SENSOR_ALL_INIT_DONE;
  /* Now look for exceptions */

  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    if ( SENSOR_INIT_SUCCESS != sensor_ptr->init_state  &&
         (SMGR_MAX_TICKS < ((sns_smgr.init_start_tick + SMGR_DD_INIT_TIMEOUT_TICK) - 
                            sns_ddf_get_timestamp())) )
    {
      /* Timeout and device has not reached success. Set driver failed */
      sensor_ptr->init_state = SENSOR_INIT_FAIL;
    }
    if ( SENSOR_INIT_SUCCESS != sensor_ptr->init_state &&
         SENSOR_INIT_FAIL != sensor_ptr->init_state )
    {
      /* Some device is neither done nor failed, must be in progress */
      sns_smgr.all_init_state = SENSOR_ALL_INIT_IN_PROGRESS;
      break;
    }
  }

  if ( (SENSOR_ALL_INIT_DONE == sns_smgr.all_init_state) &&
       sns_smgr_allocate_depot_mem() )
  {
    for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
    {
      sensor_ptr = &sns_smgr.sensor[ix];

      if ( SENSOR_INIT_SUCCESS == sensor_ptr->init_state )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_IDLE);

        sns_smgr_init_lpf_table(sensor_ptr);
        sns_smgr_init_odr_table(sensor_ptr);
        sns_smgr_init_fifo_cfg(sensor_ptr);
        sns_smgr_init_sensor_info(sensor_ptr);

        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_OFF);
      }
      else if ( SENSOR_INIT_FAIL == sensor_ptr->init_state )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_FAILED);
      }
      sns_ddf_memhandler_free_ex( &sensor_ptr->memhandler, sensor_ptr );

    }   /* END - for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ ) */

    sns_hw_set_qup_clk(false);
    sns_hw_all_powerrails_config(SNS_PWR_RAIL_OFF);

  }   /* END - if ( SENSOR_ALL_INIT_IN_PROGRESS == sns_smgr.all_init_state ) - else */
}

