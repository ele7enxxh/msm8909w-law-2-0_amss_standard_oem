/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the Intersil ALS(Light) and Proximity Sensor
   This driver has 6 sub-modules:
   1. The common handler that provides the driver interface
   2. The light(ALS) driver that handles light data type
   3. The proximitydistance driver that handles proximitydistance data type
   4. The proximity driver that handles proximity data type
   5. The proximity common handler that handles common functions for
      proximitydistance and proximity
   6. The arbitration manager that controls sensor hardware

Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------   
10/06/10     ag      Initial port of alsprx driver
10/14/10     ag      Basic sanity testing done on PCSIM   
10/15/10     ag      Ambient light tested standalone without smgr   
10/19/10     ag      - PRX and simultaneous requests tested standalone without smgr
                     - removed tabs
10/26/10     ag      Added log and debug string support   
11/12/10     ag      - Send combined response for ALS and PRX when get_data is called   
                       for two sensors
                     - fixed sensor info for resolution and power
                     - ignore cancel timer return value
                     - fix threshold register which needs to be programmed
                     - minor cleanup to remove commented out code, unused
                       variables etc.
11/15/10     ag      - log raw ADC count   
                     - rename F3 message ID to remove "SMGR"
                     - saturate data in mlux before converting to Q16
                     - fix resolution to 12-bits
11/16/10     ag      - provide ALS data in lux rather than mlux   
11/23/10     ag      - cancel timer and clean up on power down   
                     - remove unused variables
                     - remove tabs
01/11/11     yk      Added support for new attrib SNS_DDF_ATTRIB_RESOLUTION_ADC
                     and redefined SNS_DDF_ATTRIB_RESOLUTION.
01/20/11     br      Added support for attrib SNS_DDF_ATTRIB_RANGE, and changed resolution   
                     values.
01/25/11     sc      Fixed compilation warnings.   
02/24/11     ag      - Increased current to 25mA to address black target issues   
                     - Added support to handle negative ADC values for prx
03/23/11     ag      - featurize distance calculation   
                     - add registry support
                     - fix ALS stale data timeout check
04/12/11     ag      - fix issue with sat_unsigned macro   
                     - make als_factor and prx_factor 16-bits and in units of %
04/18/11     ag      - fix truncation error in als calculations   
                     - remove checks for nvitem_grp and nvitem_id 
05/19/11     ag      Changed priority of registry debug messages from ERR to LOW
05/19/11     ag      Updates for new SMGR scheduler   
08/28/11     ag      Added place holder for self test ptr
04/02/12     ag      Update registry read process to handle changes in framework
05/11/12     ag      Fix check for valid registry content
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_dd.h"
#include "sns_memmgr.h"
#include "sns_dd_alsprx_priv.h"

#ifdef ALSPRX_SDD_TEST
extern sns_ddf_status_e sns_ddf_smgr_notify_data_test
(
  sns_ddf_handle_t       smgr_handle,
  sns_ddf_sensor_data_s  data[],
  uint32_t               data_len
);
void sns_test_alsprx_timer_state(uint8_t active);
#endif
/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
/* ALS resolution mapping table by the range value: range1(0)- range4(3) */
static uint32_t sns_dd_als_rng_to_res_tb[] = { 244, 977, 3907, 15629};

#ifdef ALSPRX_EN_DISTANCE
/* proximitydistance mapping table. The value is IR rejection mode at range2 */
/* The value was derived from green line in ISL29011 data sheet(May/14/2009) fig.11.
 * Assumed the DC offset was 5 in the graph.
 * So, the formula will be:
 * ADC value = ( ADC in graph - 5 ) * 4 * 2 * (100/65)**2 where:
 *      . IR tranmission ratio is 100% instead of 65%
 *      . range is range2 instead of range3 (factor by 4)
 *      . DC mode instead of 360Khz (facotr by 2)
 * Note: The values from 0 to 5 were not derived from the graph.
 *       The values were derived at the assumption of the peak point is 0mm instead of 5mm in the graph
 *       The maximum will be 0x7ff(2047) because the value is represented by 12 bits(1 sign + 11 bits) */

static const int16_t sns_dd_prxdist_tb[] = 
{
/*0,      1,     2,     3,     4,     5,     6,    7,    8,    9 */
  1138,   1125,  1089,  1035,  966,   891,   813,  737,  666, 599,
  540,    486,   438,   396,   358,   325,   296,  271,  248, 227,
  209,    193,   178,   166,   154,   143,   134,  125,  117, 110,
  104,    98,    92,    87,    82,    77,    74,   70,   67,  64,
  61,     58,    55,    52,    50,    48,    46,   44,   42,  41,  
  -10000
};
#define SNS_DD_PRXDIST_ENGINE_TB_SIZE sizeof(sns_dd_prxdist_tb)/sizeof(sns_dd_prxdist_tb[0])
/* Adjusted proximitydistance mapping table by ir tran ratio and dc_offset */
static int16_t sns_dd_prxdist_adj_tb[SNS_DD_PRXDIST_ENGINE_TB_SIZE];
#endif /* ALSPRX_EN_DISTANCE */

/* high threshold in raw ADC for each range for ALS in milli-lux */
static const uint32_t sns_dd_isl_als_rng_high_thresh_tb[] = 
{ 
  900000,   /* for range SNS_DD_ISL_ALSPRX_RNG1 */
  3900000,  /* for range SNS_DD_ISL_ALSPRX_RNG2 */
  15900000, /* for range SNS_DD_ISL_ALSPRX_RNG3 */
  64000000  /* for range SNS_DD_ISL_ALSPRX_RNG4 */ 
};

/* The intended min and max milli-lux for each range for ALS */
static const uint32_t sns_dd_isl_als_rng_min_mlux_tb[] = 
{    
  0,        /* for range SNS_DD_ISL_ALSPRX_RNG1 */
  1000000,  /* for range SNS_DD_ISL_ALSPRX_RNG2 */
  4000000,  /* for range SNS_DD_ISL_ALSPRX_RNG3 */
  16000000  /* for range SNS_DD_ISL_ALSPRX_RNG4 */ 
};
static const uint32_t sns_dd_isl_als_rng_max_mlux_tb[] = 
{    
  1000000,  /* for range SNS_DD_ISL_ALSPRX_RNG1 */
  4000000,  /* for range SNS_DD_ISL_ALSPRX_RNG2 */
  16000000, /* for range SNS_DD_ISL_ALSPRX_RNG3 */
  64000000  /* for range SNS_DD_ISL_ALSPRX_RNG4 */ 
};
#define SNS_DD_ISL_ALS_RNG_TB_SIZE sizeof(sns_dd_isl_als_rng_high_thresh_tb)/sizeof(sns_dd_isl_als_rng_high_thresh_tb[0])
/* tables with adjusted range */
static uint32_t sns_dd_isl_als_rng_high_thresh_adj_tb[SNS_DD_ISL_ALS_RNG_TB_SIZE];
static uint32_t sns_dd_isl_als_rng_min_mlux_adj_tb[SNS_DD_ISL_ALS_RNG_TB_SIZE];
static uint32_t sns_dd_isl_als_rng_max_mlux_adj_tb[SNS_DD_ISL_ALS_RNG_TB_SIZE];
static const sns_ddf_sensor_e sns_dd_alsprx_sensor_types[SNS_DD_ALSPRX_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT
};
/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
static BOOLEAN  sns_dd_alsprx_prxdist_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

static BOOLEAN sns_dd_alsprx_als_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

static sns_ddf_status_e sns_dd_alsprx_isl29011_init
(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
);

static sns_ddf_status_e sns_dd_alsprx_isl29011_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_isl29011_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

static sns_ddf_status_e sns_dd_alsprx_isl29011_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_alsprx_isl29011_handle_timer 
(
  sns_ddf_handle_t dd_handle, 
  void* arg
);

static void sns_dd_alsprx_isl29011_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  uint32_t         irq,
  sns_ddf_time_t   timestamp
);

static sns_ddf_status_e sns_dd_alsprx_isl29011_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_driver_if_s sns_alsprx_driver_fn_list =
{
  &sns_dd_alsprx_isl29011_init,
  &sns_dd_alsprx_isl29011_get_data,
  &sns_dd_alsprx_isl29011_set_attrib, 
  &sns_dd_alsprx_isl29011_get_attrib,
  &sns_dd_alsprx_isl29011_handle_timer,
  &sns_dd_alsprx_isl29011_handle_irq,
  &sns_dd_alsprx_isl29011_reset,
  NULL /* self test */
};

/*===========================================================================
FUNCTION      sns_dd_alsprx_log
  
DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 
===========================================================================*/
void sns_dd_alsprx_log(
   sns_dd_alsprx_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1, 
   uint32_t               data1_q16,
   uint32_t               data2,
   uint32_t               data2_q16,
   uint32_t               raw_data )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_INTERSIL;
     
     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();
     
     /* Log the sensor data */
     if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = raw_data;
      }
     else
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = data2;
       log_struct_ptr->data[3] = data2_q16;
       log_struct_ptr->data[4] = raw_data;
     }
      
     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, 
                                  log_struct_ptr);
   }
   else
   {
     dd_ptr->dropped_logs++;
   }
}

/*===========================================================================

FUNCTION      sns_dd_start_general_timer
  
DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_start_general_timer 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint32_t duration
)
{
#ifdef ALSPRX_SDD_TEST
  sns_test_alsprx_timer_state( 1 );
#else
  sns_ddf_status_e status;

  if ( dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.active )
  {
    return;
  }

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_RES_INVALID,
                               100);
#endif

  status = sns_ddf_timer_start(dd_ptr->timer, duration);
  if ( status != SNS_DDF_SUCCESS )
  {
    return;
  }
  dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.active = TRUE;
#endif
}

/*===========================================================================

FUNCTION      sns_dd_stop_general_timer
  
DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_stop_general_timer( sns_dd_alsprx_state_t* dd_ptr )
{
#ifdef ALSPRX_SDD_TEST
  sns_test_alsprx_timer_state( 0 );
#else
  if ( TRUE == dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.active )
  {
#ifdef ALSPRX_DEBUG_TIMER
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_RES_INVALID,
                                 500);
#endif

    sns_ddf_timer_cancel(dd_ptr->timer); /* will return error if timer already stopped */
    dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.active = FALSE;
  }
#endif
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl_issue_commands
  
DESCRIPTION   issue commands to the real hardware through I2C

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_isl_issue_commands
( 
  sns_dd_alsprx_state_t* dd_ptr,
  uint8_t                i2c_data_cmd1,
  uint8_t                i2c_data_cmd2
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_rw;
  uint16_t         i2c_data= 0;

  /* Set thresholds such that interrupt is generated after every ADC conversion.
     High threshold < Low threshold will do this. */
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_INT_HT_LSB_ADDR,
                              (uint8_t*)&i2c_data, 
                              2, 
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_HI_THRESH_WR_ERR);
    return status;
  }
  i2c_data=1;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_INT_LT_LSB_ADDR,
                              (uint8_t*)&i2c_data, 
                              2, 
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_LO_THRESH_WR_ERR);
    return status;
  }

  /* need to preserve the programming sequence, that is command 2 first then command 1 later */
  /* Program the command 2 register */
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_COMMAND2_ADDR,
                              &i2c_data_cmd2,
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_WR_CMD2_REG_ERR);
    return status;
  }

  /* Program the command 1 register */
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_COMMAND1_ADDR,
                              &i2c_data_cmd1,
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_WR_CMD1_REG_ERR);
    return status;
  }

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_arbit_req_data
  
DESCRIPTION   A arbitration manager function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_arbit_req_data
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint32_t               data_type,
  uint8_t                rng_sel
)
{
  uint8_t i2c_data_cmd1, i2c_data_cmd2;
  sns_dd_arbit_db_type *arbit_db_ptr = &dd_ptr->sns_dd_alsprx_common_db.arbit_db;
  sns_dd_arbit_state_type save_state = arbit_db_ptr->state;
  sns_ddf_status_e status=SNS_DDF_SUCCESS;

  if ( data_type == SNS_DDF_SENSOR_PROXIMITY )
  {
    arbit_db_ptr->state |= SNS_DD_ARBIT_STATE_PRX_PEND;
    arbit_db_ptr->prx_req_rng = rng_sel;
  }
  else /* SNS_DDF_SENSOR_AMBIENT */
  {
    arbit_db_ptr->state |= SNS_DD_ARBIT_STATE_ALS_PEND;
    arbit_db_ptr->als_req_rng = rng_sel;
  }

  if ( save_state != SNS_DD_ARBIT_STATE_IDLE )
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_DEV_STATE_PEND);
  }
  else
  {
    if ( arbit_db_ptr->state == SNS_DD_ARBIT_STATE_ALS_PEND )
    {
      /* Set the command registers' values */
      dd_ptr->sns_dd_als_db.resolution = dd_ptr->sns_dd_als_db.next_resolution;
      i2c_data_cmd1 = SNS_DD_ISL_ALSPRX_MODE_ALS_ONCE;
      i2c_data_cmd2 = arbit_db_ptr->als_req_rng | dd_ptr->sns_dd_als_db.resolution;
      arbit_db_ptr->last_issue_mode = SNS_DDF_SENSOR_AMBIENT;
      arbit_db_ptr->als_req_cnt++;
      dd_ptr->sns_dd_als_db.sample_attempt++;
    }
    else
    {
      /* Set the command registers' values */
      dd_ptr->sns_dd_prx_db.resolution = dd_ptr->sns_dd_prx_db.next_resolution;
      i2c_data_cmd1 = SNS_DD_ISL_ALSPRX_MODE_PRX_ONCE;
      i2c_data_cmd2 = arbit_db_ptr->prx_req_rng | dd_ptr->sns_dd_prx_db.resolution | SNS_DD_PRX_DFLT_MODE_FRQ_DC |
                      SNS_DD_ISL_ALSPRX_IR_LED_25MA | SNS_DD_ISL_ALSPRX_PRX_SCHEME_AMB_IR_REJ;
      arbit_db_ptr->last_issue_mode = SNS_DDF_SENSOR_PROXIMITY;
      arbit_db_ptr->prx_req_cnt++;
    }
    status = sns_dd_alsprx_isl_issue_commands (dd_ptr, i2c_data_cmd1, i2c_data_cmd2);
    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }

#ifdef ALSPRX_USE_POLLING
    sns_dd_start_general_timer (dd_ptr,
                                SNS_DD_ALSPRX_DATA_READY_US);

#endif
  }
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_prxcomm_req_data
  
DESCRIPTION   A handler of proximity common handler that handles data request from proximity or 
              proximitydistance driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_prxcomm_req_data 
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  sns_dd_prxcomm_db_type *prxcomm_db_ptr = &dd_ptr->sns_dd_alsprx_common_db.prxcomm_db;
  sns_dd_prxcomm_state_type save_state = prxcomm_db_ptr->state;

  prxcomm_db_ptr->state |= SNS_PRXCOMM_STATE_PRX_PEND;
  prxcomm_db_ptr->prx_req_cnt++;

  if ( save_state != SNS_PRXCOMM_STATE_IDLE )
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_STATE_PEND);
  }
  else
  {
    sns_dd_alsprx_arbit_req_data (dd_ptr,
                                  SNS_DDF_SENSOR_PROXIMITY, 
                                  SNS_DD_ISL_ALSPRX_RNG2);
  }
}

#ifdef ALSPRX_EN_DISTANCE
/*===========================================================================

FUNCTION      sns_dd_alsprx_prxdist_engine
  
DESCRIPTION   Convert a raw data to a real distance in mm

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

===========================================================================*/
uint32_t sns_dd_alsprx_prxdist_engine (uint64_t time, uint16_t raw_data)
{
  int32_t i;
  for ( i=1; i < SNS_DD_PRXDIST_ENGINE_TB_SIZE; i++ )
  {
    if ( (int16_t)raw_data >= sns_dd_prxdist_adj_tb[i] )
    {
      break;
    }
  }
  return i;
}
#endif /* ALSPRX_EN_DISTANCE */

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_convert_to_mlux
  
DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None 

===========================================================================*/
uint32_t sns_dd_alsprx_als_convert_to_mlux 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint8_t rng, 
  uint16_t raw_data
)
{
  uint16_t max_val;
  uint64_t data_64;
  uint32_t data_32;

  /* TODO - move this to sns_dd_alsprx_set_resolution when it is 
     finally implemented */
  switch ( dd_ptr->sns_dd_als_db.resolution )
  {
    case SNS_DD_ALSPRX_RES_8_BIT:
      max_val = SNS_DD_ALSPRX_RES_08BIT_MAX_VAL;
      break;
    case SNS_DD_ALSPRX_RES_12_BIT:
      max_val = SNS_DD_ALSPRX_RES_12BIT_MAX_VAL;
      break;
    case SNS_DD_ALSPRX_RES_16_BIT:
      max_val = SNS_DD_ALSPRX_RES_16BIT_MAX_VAL;
      break;
    default:
      SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_ALS_RES_INVALID,
                                   dd_ptr->sns_dd_als_db.resolution);
      return 0;
  }

#define ALS_CONVERSION  dd_ptr->sns_dd_als_db.als_conversion / max_val

  switch ( rng )
  {
    case SNS_DD_ISL_ALSPRX_RNG1:
      data_64 = (uint64_t)raw_data * ALS_CONVERSION;
      break;
    case SNS_DD_ISL_ALSPRX_RNG2:
      data_64 = (uint64_t)raw_data * 4 * ALS_CONVERSION;
      break;
    case SNS_DD_ISL_ALSPRX_RNG3:
      data_64 = (uint64_t)raw_data * 16 * ALS_CONVERSION;
      break;
    case SNS_DD_ISL_ALSPRX_RNG4:
      data_64 = (uint64_t)raw_data * 64 * ALS_CONVERSION;
      break;
    default:
      SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_ALS_RNG_INVALID,
                                   rng);
      data_64 = 0;
  }

  data_32 = (uint32_t)((data_64 > 0xFFFFFFFF)? 0xFFFFFFFF : data_64);

#ifdef ALSPRX_DEBUG_REG
  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             raw_data, 
                             ALS_CONVERSION, 
                             11);

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             64, 
                             data_64>>32, 
                             data_64);

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             32,
                             32,
                             data_32);
#endif /* ALSPRX_DEBUG_REG */

  return (data_32);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_async_notify
  
DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_async_notify 
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data_val, 
  uint32_t               prx_bin ,
  uint32_t               raw_data
)
{
  uint8_t num_sensors = 1, num_samples=2;
  uint16_t size;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  static sns_ddf_sensor_sample_s *buf_ptr, *als_buf_ptr, *prx_buf_ptr;
  static sns_ddf_sensor_data_s *sensor_data;
  uint8_t sensor_idx=0;
  uint32_t data_lux;

#ifdef ALSPRX_EN_DISTANCE
  if ( sensor_type == SNS_DDF_SENSOR_PROXIMITY )
  {
    num_samples++; /* for distance */
  }
#endif /* ALSPRX_EN_DISTANCE */

  if ( dd_ptr->two_requests == TRUE )
  {
    num_sensors = 2;
  }

  if ((( dd_ptr->two_requests == TRUE ) && ( dd_ptr->num_requests == 2 )) ||
      ( dd_ptr->two_requests == FALSE ))
  { 
    /* allocate memory for one or both sensors as needed */
    size = num_sensors*sizeof(sns_ddf_sensor_data_s);
    if ((status = sns_ddf_malloc((void **)&sensor_data, size)) != SNS_DDF_SUCCESS )
    {
      /* Failed miserably. Can't even notify SMGR */
      return;
    }
  }

  size = num_samples*sizeof(sns_ddf_sensor_sample_s);
  if ((status = sns_ddf_malloc((void **)&buf_ptr, size)) != SNS_DDF_SUCCESS )
  {
    /* Failed miserably. Can't even notify SMGR */
    return;
  }

  if ( sensor_type == SNS_DDF_SENSOR_AMBIENT )
  {
    if ( dd_ptr->two_requests == TRUE )
    {
      sensor_idx = 1;
    }
     /* convert to lux */ 
    data_lux = (uint32_t) (data_val / 1000);

    /* saturate */
    data_lux = (data_lux > 0x7FFF) ? 0x7FFF : data_lux;

    /* related to floating point library removal; change to FX_CONV_Q16(val,0) */
    buf_ptr[0].sample = FX_FLTTOFIX_Q16(data_lux); 
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;

    sns_dd_alsprx_log(dd_ptr, sensor_type, data_val, buf_ptr[0].sample, 0, 0, raw_data);
    als_buf_ptr = buf_ptr;
  }
  else
  {
    buf_ptr[0].sample = FX_CONV_Q16(prx_bin, 0);  /* proximity binary */
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;
#ifdef ALSPRX_EN_DISTANCE
    buf_ptr[2].sample = FX_CONV_Q16(data_val, 0); /* proximity distance */
    buf_ptr[2].status = SNS_DDF_SUCCESS;
    sns_dd_alsprx_log(dd_ptr, sensor_type, prx_bin, buf_ptr[0].sample, data_val, buf_ptr[2].sample, raw_data);
#else
    sns_dd_alsprx_log(dd_ptr, sensor_type, prx_bin, buf_ptr[0].sample, 0, 0, raw_data);
#endif /* ALSPRX_EN_DISTANCE */

    
    prx_buf_ptr = buf_ptr;
  }

  sensor_data[sensor_idx].status = status;  
  sensor_data[sensor_idx].sensor = sensor_type;
  sensor_data[sensor_idx].num_samples = num_samples;
  sensor_data[sensor_idx].timestamp = sns_ddf_get_timestamp();
  sensor_data[sensor_idx].samples = buf_ptr;

  dd_ptr->num_requests--;
  if ((dd_ptr->two_requests == TRUE) && (dd_ptr->num_requests == 1 ))
  {
    /* still waiting for data. don't send notification yet. */
    return;
  }

#ifdef ALSPRX_SDD_TEST   
  sns_ddf_smgr_notify_data_test(dd_ptr->smgr_handle, sensor_data, num_sensors);
#else
  sns_ddf_smgr_notify_data(dd_ptr->smgr_handle, sensor_data, num_sensors);
#endif 
  if ( dd_ptr->two_requests == TRUE )
  {
    /* free buf for both ALS and PRX */
    dd_ptr->two_requests = FALSE;
    sns_ddf_mfree(als_buf_ptr);
    sns_ddf_mfree(prx_buf_ptr);
  }
  else
  {
    sns_ddf_mfree(buf_ptr);
  }
  sns_ddf_mfree(sensor_data);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_calc_new_rng
  
DESCRIPTION   This function is called to calculate the range to use for the
              passed in milli-lux

DEPENDENCIES  None

RETURN VALUE  The range corresponding to the milli-lux

SIDE EFFECT   None 

===========================================================================*/
uint8_t sns_dd_alsprx_als_calc_new_rng (uint32_t data_mlux)
{
  uint8_t i;

  for ( i = SNS_DD_ISL_ALSPRX_RNG1; i <= SNS_DD_ISL_ALSPRX_RNG4; i++ )
  {
    if ( data_mlux <= sns_dd_isl_als_rng_high_thresh_adj_tb[i] )
    {
      return i;
    }
  }
  SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_MLUX_TOO_LARGE,
                                sns_dd_isl_als_rng_high_thresh_adj_tb[SNS_DD_ISL_ALSPRX_RNG4], 
                                data_mlux);
  return SNS_DD_ISL_ALSPRX_RNG4;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_rsp
  
DESCRIPTION   This function is called by the arbitration manager when ALS(light) data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_als_rsp 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t               raw_data, 
  uint32_t               data_mlux
)
{
  dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_IDLE;
  *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache = data_mlux;

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             raw_data, 
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache, 
                             dd_ptr->sns_dd_als_db.arbit_req_rng_sel);

  sns_dd_alsprx_async_notify(dd_ptr, SNS_DDF_SENSOR_AMBIENT, 
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache, 0, 
                             (uint32_t) raw_data);

  dd_ptr->sns_dd_als_db.sample_attempt = 0;
  dd_ptr->sns_dd_als_db.last_rng_sel = dd_ptr->sns_dd_als_db.arbit_req_rng_sel;
  // check to see if range adjustment maybe needed
  dd_ptr->sns_dd_als_db.next_rng_sel = sns_dd_alsprx_als_calc_new_rng(data_mlux);
  dd_ptr->sns_dd_als_db.last_get_time = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  dd_ptr->sns_dd_als_db.last_mlux  = data_mlux;

  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_NXT_RNG_SEL,
                             dd_ptr->sns_dd_als_db.next_rng_sel);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_raw_data_ready
  
DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_als_raw_data_ready 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t raw_data
)
{
  uint32_t data_mlux;
  uint8_t new_rng;

  if ( dd_ptr->sns_dd_als_db.pend_state != SNS_DD_PEND_STATE_PEND )
  {
    /* Ignore the response */
    SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_ALS_DATA_READY_ERR);
    return;
  }

  /* convert reading to mlux */
  data_mlux = sns_dd_alsprx_als_convert_to_mlux(dd_ptr, 
                                                dd_ptr->sns_dd_als_db.arbit_req_rng_sel, 
                                                raw_data);

  /* if sampling for the 2nd time, simply process the data and recalculate range */
  if ( dd_ptr->sns_dd_als_db.sample_attempt == SNS_DD_ALS_SAMPLE_ATTEMPT )
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_DATA_READY_MAX_ATTEMPTS);
    sns_dd_alsprx_als_rsp(dd_ptr, raw_data, data_mlux);
    return;
  }

  /********************************************************
   * check lux value to see if a different range is needed
   *********************************************************/
  /* check for min */
  if ( data_mlux <= sns_dd_isl_als_rng_min_mlux_adj_tb[dd_ptr->sns_dd_als_db.arbit_req_rng_sel] )
  {
    /* compute new range */
    new_rng = sns_dd_alsprx_als_calc_new_rng(data_mlux);

    /* if we are at edge of range, next sample would be the same range */
    if ( new_rng == dd_ptr->sns_dd_als_db.arbit_req_rng_sel )
    {
      /* process data */
      SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_DATA_READY_NO_RESAMPLE1,
                                 data_mlux, 
                                 sns_dd_isl_als_rng_min_mlux_adj_tb[dd_ptr->sns_dd_als_db.arbit_req_rng_sel]);
      sns_dd_alsprx_als_rsp(dd_ptr, raw_data, data_mlux);
      return;
    }

    /* Resample with new range */
    SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_DATA_READY_RESAMPLE1,
                               data_mlux, 
                               sns_dd_isl_als_rng_min_mlux_adj_tb[dd_ptr->sns_dd_als_db.arbit_req_rng_sel], 
                               new_rng);
    dd_ptr->sns_dd_als_db.arbit_req_rng_sel = new_rng;
    sns_dd_alsprx_arbit_req_data (dd_ptr, SNS_DDF_SENSOR_AMBIENT, dd_ptr->sns_dd_als_db.arbit_req_rng_sel);
  }
  /* check for max/clipping */
  else if ( data_mlux >= sns_dd_isl_als_rng_max_mlux_adj_tb[dd_ptr->sns_dd_als_db.arbit_req_rng_sel] )
  {
    /* If already at range 4, no need to resample */
    if ( dd_ptr->sns_dd_als_db.arbit_req_rng_sel == SNS_DD_ISL_ALSPRX_RNG4 )
    {
      /* process data */
      SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_DATA_READY_NO_RESAMPLE2,
                                 data_mlux);
      sns_dd_alsprx_als_rsp(dd_ptr, raw_data, data_mlux);
      return;
    }

    /* Resample with range 4 */
    SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_DATA_READY_RESAMPLE2,
                               data_mlux,
                               sns_dd_isl_als_rng_max_mlux_adj_tb[dd_ptr->sns_dd_als_db.arbit_req_rng_sel], 
                               SNS_DD_ISL_ALSPRX_RNG4);
    dd_ptr->sns_dd_als_db.arbit_req_rng_sel = SNS_DD_ISL_ALSPRX_RNG4;
    sns_dd_alsprx_arbit_req_data (dd_ptr, SNS_DDF_SENSOR_AMBIENT, dd_ptr->sns_dd_als_db.arbit_req_rng_sel);
  }
  /* lux is within range */
  else
  {
    sns_dd_alsprx_als_rsp(dd_ptr, raw_data, data_mlux);
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_prx_rsp
  
DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_prx_rsp 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t               raw_data_unsigned
)
{
  uint16_t adjusted_adc;

  adjusted_adc = ALSPRX_CONV_TO_UNSIGNED(
                   (raw_data_unsigned - dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset),
                   16);

  if ( adjusted_adc >= dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_NEAR_BY )
    {
      SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_BINARY_NEAR);
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = (uint32_t)SNS_PRX_NEAR_BY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_NEAR_BY;
  }
  else if ( adjusted_adc < dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_FAR_AWAY )
    {
      SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_BINARY_FAR);
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = SNS_PRX_FAR_AWAY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_FAR_AWAY;
  }
}
/*===========================================================================

FUNCTION      sns_dd_alsprx_prxcomm_rsp
  
DESCRIPTION   This function is called by the arbitration manager when proximity 
              data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void sns_dd_alsprx_prxcomm_rsp 
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t               raw_data
)
{
  sns_dd_prxcomm_state_type state = dd_ptr->sns_dd_alsprx_common_db.prxcomm_db.state;
  uint16_t raw_data_unsigned;

  raw_data_unsigned = ALSPRX_CONV_TO_UNSIGNED(raw_data, 12);
  if ( state & SNS_PRXCOMM_STATE_PRX_PEND )
  {
    dd_ptr->sns_dd_alsprx_common_db.prxcomm_db.state &= ~SNS_PRXCOMM_STATE_PRX_PEND;

#ifdef ALSPRX_EN_DISTANCE
    *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache = sns_dd_alsprx_prxdist_engine( 
      sns_ddf_get_timestamp(), 
      raw_data_unsigned );
#else
    *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache = 0;
#endif /* ALSPRX_EN_DISTANCE */

    if ( dd_ptr->sns_dd_prx_db.pend_state != SNS_DD_PEND_STATE_PEND )
    {
      /* Ignore the response */
      SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_PRX_NOT_PENDING);
    }
    else
    {
      dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_IDLE;

      SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_RSP_READY,
                                 raw_data, 
                                 *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache);

      sns_dd_alsprx_prx_rsp(dd_ptr, raw_data_unsigned); /* binary decision */
      sns_dd_alsprx_async_notify(dd_ptr, 
                                 SNS_DDF_SENSOR_PROXIMITY, 
                                 *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache, 
                                 dd_ptr->sns_dd_prx_db.prx_data_cache,
                                 (uint32_t) raw_data);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_handle_irq

===========================================================================*/
/*!
  @brief ISL29011 interrupt handler
 
  @detail
  This function will be called within the context of the Interrupt 
  Service Task (IST), *not* the ISR.
 
  @param[in] dd_handle  Handle to a driver instance. 
  @param[in] irq        The IRQ representing the interrupt that occured. 
  
  @return None 
*/
/*=========================================================================*/
static void sns_dd_alsprx_isl29011_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  uint32_t         irq,
  sns_ddf_time_t   timestamp
)
{
  return;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is  
         the callback function submitted when initializing a timer.
 
  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready
 
  @param[in] dd_handle  Handle to a driver instance. 
  @param[in] arg        The argument submitted when the timer was set. 
  
  @return None 
*/
/*=========================================================================*/
static void sns_dd_alsprx_isl29011_handle_timer 
(
  sns_ddf_handle_t handle, 
  void* arg
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t*)handle;
  sns_dd_arbit_db_type *arbit_db_ptr = &dd_ptr->sns_dd_alsprx_common_db.arbit_db;
  uint16_t    i2c_read_data;
  uint8_t     i2c_cmd_reg;
  uint8_t     i2c_data_cmd1, i2c_data_cmd2;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t     bytes_rw;
#ifdef ALSPRX_SDD_TEST
  static uint16_t timer_fired=0, no_int=0, als=0, prx=0;

  timer_fired++;
#endif

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                DBG_DD_ALSPRX_HANDLE_TIMER);
#endif

  sns_dd_stop_general_timer(dd_ptr);

  /* read command1 reg to verify if sensor data is available */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             SNS_DD_ISL_ALSPRX_COMMAND1_ADDR,
                             (uint8_t*)&i2c_cmd_reg,
                             1,
                             &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_CMD1_READ_ERR);
    return;
  }
  else
  { 
    if ( ( i2c_cmd_reg & SNS_DD_ISL_ALSPRX_INT_TRIG ) == FALSE )
    {
      SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_I2C_READ_CMD1_NO_INT,
                                  i2c_cmd_reg); 
#ifdef ALSPRX_SDD_TEST
    no_int++;
#endif
    return;
    }
    /* issue multiple i2c read */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_ISL_ALSPRX_DATA_LSB_ADDR,
                               (uint8_t*)&i2c_read_data,
                               2,
                               &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_DATA_LSB_READ_ERR);
      return;
    }
  }

  if ( (i2c_cmd_reg & SNS_DD_ISL_ALSPRX_MODE_MASK) == SNS_DD_ISL_ALSPRX_MODE_ALS_ONCE )    /* ALS data was sampled */
  {
    /* verify if the last command was for ALS */
    if ( arbit_db_ptr->last_issue_mode != SNS_DDF_SENSOR_AMBIENT )
    {
      SNS_PRINTF_STRING_ID_HIGH_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_I2C_CMD1_READ_WRONG_DATA_TYPE);
    }

#ifdef ALSPRX_SDD_TEST
  als++;
#endif
    arbit_db_ptr->state &= ~SNS_DD_ARBIT_STATE_ALS_PEND;

    /* indicate the data to the light(ALS) sub-driver */
    sns_dd_alsprx_als_raw_data_ready(dd_ptr, i2c_read_data);

    /* check if proximity sampling is needed */
    if ( arbit_db_ptr->state & SNS_DD_ARBIT_STATE_PRX_PEND )
    {
      arbit_db_ptr->last_issue_mode = SNS_DDF_SENSOR_PROXIMITY;
      /* Set the command registers' values */
      dd_ptr->sns_dd_prx_db.resolution = dd_ptr->sns_dd_prx_db.next_resolution;
      i2c_data_cmd1 = SNS_DD_ISL_ALSPRX_MODE_PRX_ONCE;
      i2c_data_cmd2 = arbit_db_ptr->prx_req_rng | dd_ptr->sns_dd_prx_db.resolution | 
                      SNS_DD_PRX_DFLT_MODE_FRQ_DC | SNS_DD_ISL_ALSPRX_IR_LED_25MA | 
                      SNS_DD_ISL_ALSPRX_PRX_SCHEME_AMB_IR_REJ;
      status = sns_dd_alsprx_isl_issue_commands (dd_ptr, i2c_data_cmd1, i2c_data_cmd2);
      if ( status != SNS_DDF_SUCCESS )
      {
        SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_I2C_CMD2_WR_ERR);
      }
      else
      {
#ifdef ALSPRX_USE_POLLING
        sns_dd_start_general_timer (dd_ptr,
                                    SNS_DD_ALSPRX_DATA_READY_US);
#endif
      }
    }
  }
  else if ( (i2c_cmd_reg & SNS_DD_ISL_ALSPRX_MODE_MASK) == SNS_DD_ISL_ALSPRX_MODE_PRX_ONCE )
  {
    arbit_db_ptr->state &= ~SNS_DD_ARBIT_STATE_PRX_PEND;

    /* verify if the last command was for proximity */
    if ( arbit_db_ptr->last_issue_mode != SNS_DDF_SENSOR_PROXIMITY )
    {
      SNS_PRINTF_STRING_ID_HIGH_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_I2C_CMD1_READ_WRONG_DATA_TYPE);
    }
#ifdef ALSPRX_SDD_TEST
  prx++;
#endif
    /* indicate the data to the proximity common handler */
    sns_dd_alsprx_prxcomm_rsp (dd_ptr, i2c_read_data);

    /* check if ALS sampling is needed */
    if ( arbit_db_ptr->state & SNS_DD_ARBIT_STATE_ALS_PEND )
    {
      arbit_db_ptr->last_issue_mode = SNS_DDF_SENSOR_AMBIENT;
      /* Set the command registers' values */
      dd_ptr->sns_dd_als_db.resolution = dd_ptr->sns_dd_als_db.next_resolution;
      i2c_data_cmd1 = SNS_DD_ISL_ALSPRX_MODE_ALS_ONCE;
      i2c_data_cmd2 = arbit_db_ptr->als_req_rng | dd_ptr->sns_dd_als_db.resolution;
      status = sns_dd_alsprx_isl_issue_commands (dd_ptr, i2c_data_cmd1, i2c_data_cmd2);
      if ( status != SNS_DDF_SUCCESS )
      {
        SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_I2C_CMD2_WR_ERR);
      }
      else
      {
#ifdef ALSPRX_USE_POLLING
        sns_dd_start_general_timer (dd_ptr,
                                    SNS_DD_ALSPRX_DATA_READY_US);
#endif
      }
    }
  }
  else
  {
    SNS_PRINTF_STRING_ID_HIGH_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_I2C_CMD1_READ_ERR);
  }
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_als_init
  
DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
  sns_dd_als_db_type *als_db_ptr = &dd_ptr->sns_dd_als_db;
  sns_dd_alsprx_common_db_type *common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db;
  int8_t i;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_INIT);
  als_db_ptr->pend_state        = SNS_DD_PEND_STATE_IDLE;
  als_db_ptr->last_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->next_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->arbit_req_rng_sel = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->sample_attempt    = 0;
  als_db_ptr->resolution        = SNS_DD_ALSPRX_RES_12_BIT;
  als_db_ptr->next_resolution   = SNS_DD_ALSPRX_RES_12_BIT;
  als_db_ptr->last_get_time     = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX;

  als_db_ptr->als_conversion = (uint32_t)1000000 * common_db_ptr->nv_db.als_factor / 
                                         common_db_ptr->nv_db.visible_ratio;
  /* adjust the high threshold, min, and max mlux tables for Light by the light trans ratio */
  for ( i = 0; i < SNS_DD_ISL_ALS_RNG_TB_SIZE; i++ )
  {
    sns_dd_isl_als_rng_high_thresh_adj_tb[i] = (uint32_t)((uint64_t)sns_dd_isl_als_rng_high_thresh_tb[i] * 
                                                          common_db_ptr->nv_db.als_factor / 
                                                          common_db_ptr->nv_db.visible_ratio);
  }
  for ( i = 0; i < SNS_DD_ISL_ALS_RNG_TB_SIZE; i++ )
  {
    sns_dd_isl_als_rng_min_mlux_adj_tb[i] = (uint32_t)((uint64_t)sns_dd_isl_als_rng_min_mlux_tb[i] * 
                                                       common_db_ptr->nv_db.als_factor / 
                                                       common_db_ptr->nv_db.visible_ratio);
  }
  for ( i = 0; i < SNS_DD_ISL_ALS_RNG_TB_SIZE; i++ )
  {
    sns_dd_isl_als_rng_max_mlux_adj_tb[i] = (uint32_t)((uint64_t)sns_dd_isl_als_rng_max_mlux_tb[i] * 
                                                       common_db_ptr->nv_db.als_factor / 
                                                       common_db_ptr->nv_db.visible_ratio);
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_prx_init
  
DESCRIPTION   initializes the Intersil ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_PRX_INIT);
  prx_db_ptr->pend_state = SNS_DD_PEND_STATE_IDLE;
  prx_db_ptr->accuracy = SNS_DD_PRXDIST_ACCURACY;
  prx_db_ptr->res = SNS_DD_PRXDIST_RES;
  prx_db_ptr->resolution = SNS_DD_ALSPRX_RES_12_BIT;
  prx_db_ptr->next_resolution = SNS_DD_ALSPRX_RES_12_BIT;
  prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN;

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_prxcomm_init
  
DESCRIPTION   Initialize the proximity common handler

DEPENDENCIES  sns_dd_alsprx_common_db.nv_db.ir_ratio and dc_offset shall be initialized in advance

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_prxcomm_init ( sns_dd_alsprx_state_t* dd_ptr )
{
#ifdef ALSPRX_EN_DISTANCE
  uint8_t     ir_ratio;
  uint16_t    dc_offset;
  uint16_t    prx_factor;
  uint32_t    i;
  uint32_t    prx_conversion;
#endif /* ALSPRX_EN_DISTANCE */
  sns_dd_alsprx_common_db_type* common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db;

  common_db_ptr->prxcomm_db.state = SNS_PRXCOMM_STATE_IDLE;
  common_db_ptr->prxcomm_db.prx_req_cnt     = 0;

#ifdef ALSPRX_EN_DISTANCE
  /* re-configure the proxdist engine table */
  /* Apply ir_ratio, prx_factor, and dc_offset */
  ir_ratio = common_db_ptr->nv_db.ir_ratio;
  dc_offset = common_db_ptr->nv_db.dc_offset;
  prx_factor = common_db_ptr->nv_db.prx_factor;

  prx_conversion = ir_ratio * ir_ratio * prx_factor / 1000000;

  for ( i = 0; i < SNS_DD_PRXDIST_ENGINE_TB_SIZE-1; i++ )
  {
    uint32_t adj_value, org_tb_value = (uint32_t)sns_dd_prxdist_tb[i];
    adj_value = org_tb_value * prx_conversion + dc_offset;
    if ( adj_value > SNS_DD_PRXDIST_TB_MAX_VALUE )
    {
      sns_dd_prxdist_adj_tb[i] = SNS_DD_PRXDIST_TB_MAX_VALUE;      
    }
    else
    {
      sns_dd_prxdist_adj_tb[i] = adj_value;      
    }
  }
  sns_dd_prxdist_adj_tb[i] = sns_dd_prxdist_tb[i];      /* the last index of the array */
#endif /* ALSPRX_EN_DISTANCE */

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_common_init
  
DESCRIPTION   initializes the Intersil ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/

sns_ddf_status_e sns_dd_alsprx_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.active = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.sns_dd_timer.defined = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;
  dd_ptr->num_requests = 0;
  dd_ptr->two_requests = FALSE;

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_source);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio);    
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset);    
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near);    
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor);
#endif /* ALSPRX_DEBUG_REG */

  /* initialize the structure for the arbitrator sum-module */
  dd_ptr->sns_dd_alsprx_common_db.arbit_db.state           = SNS_DD_ARBIT_STATE_IDLE;
  dd_ptr->sns_dd_alsprx_common_db.arbit_db.prx_req_cnt     = 0;
  dd_ptr->sns_dd_alsprx_common_db.arbit_db.als_req_cnt     = 0;

  /* initialize proximity common module */
  sns_dd_alsprx_prxcomm_init(dd_ptr);

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_init_data_struct
  
DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

  /* Initialize the common database */
  if ( (init_status = sns_dd_alsprx_common_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_CMN_INIT_ERR);
    return init_status;
  }

  if ( (init_status = sns_dd_alsprx_als_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_INIT_ERR);
    return init_status;
  }

  if ( (init_status = sns_dd_alsprx_prx_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_INIT_ERR);
    return init_status;
  }

  return init_status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_set_powerstate
  
DESCRIPTION   Sets the power state of the ISL ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_set_powerstate
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_powerstate_e   op_mode,
  boolean                init_data
)
{
  uint16_t   i2c_data = 0x0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_w;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
    /* Set thresholds such that interrupt is generated after every ADC conversion.
      High threshold < Low threshold will do this. */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_INT_HT_LSB_ADDR,
                                (uint8_t*)&i2c_data, 
                                2, 
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_LO_THRESH_WR_ERR);
      return status;
    }
    i2c_data = 0x1;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_INT_LT_LSB_ADDR,
                                (uint8_t*)&i2c_data, 
                                2, 
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_LO_THRESH_WR_ERR);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_MODE_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_COMMAND1_ADDR,
                                (uint8_t*)&i2c_data, 
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_CMD1_WR_ERR);
      return status;
    }
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    sns_dd_stop_general_timer( dd_ptr ); /* stop any pending timers */

    if (init_data == TRUE) 
    {
      /* cleanup */
      status = sns_dd_alsprx_init_data_struct(dd_ptr);
      if ( status != SNS_DDF_SUCCESS )
      {
        return status;
      }  
    }
    i2c_data  = SNS_DD_ISL_ALSPRX_MODE_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_COMMAND1_ADDR,
                                (uint8_t*)&i2c_data, 
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_CMD1_WR_ERR);
      return status;
    }
  }

  dd_ptr->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_CONFIGURED;

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_set_resolution
  
DESCRIPTION   Sets the resolution of the ISL ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_set_resolution
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  sns_dd_alsprx_res_e    resolution
)
{
  sns_ddf_status_e status=SNS_DDF_EFAIL;

  /* Fixed resolution for now */
  /*
  switch ( sensor_type )
  {
    case SNS_DDF_SENSOR_AMBIENT:
      dd_ptr->sns_dd_als_db.next_resolution = resolution;
      break;
    case SNS_DDF_SENSOR_PROXIMITY:
      dd_ptr->sns_dd_prx_db.next_resolution = resolution;
      break;
    default:
      status = SNS_DDF_EFAIL;
  }
  */
  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were 
         in after init() was called.
 
  @detail
  Resets  the driver state structure
 
  @param[in] handle  Handle to a driver instance.
 
  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned. 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29011_reset
(
  sns_ddf_handle_t dd_handle
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

  if ( dd_handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( (init_status = sns_dd_alsprx_init_data_struct(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  if ( (init_status = sns_dd_alsprx_set_powerstate(dd_ptr, 
                          SNS_DDF_POWERSTATE_LOWPOWER, FALSE)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_SET_PWR_ST_ERR);
    return init_status;
  }

  return init_status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to 
   associated devices, configures the driver and devices, and places 
   the devices in the default power state. Returns the instance handle along 
   with a list of supported sensors. This function will be called at init 
   time.
    
   @param[out] dd_handle_ptr  Pointer that this function must malloc and 
                              populate. This is a handle to the driver
                              instance that will be passed in to all other
                              functions. NB: Do not use @a memhandler to
                              allocate this memory.
   @param[in]  smgr_handle    Handle used to identify this driver when it 
                              calls into Sensors Manager functions.
   @param[in]  nv_params      NV parameters retrieved for the driver.
   @param[in]  device_info    Access info for physical devices controlled by 
                              this driver. Used to configure the bus
                              and talk to the devices.
   @param[in]  num_devices    Number of elements in @a device_info. 
   @param[in]  memhandler     Memory handler used to dynamically allocate 
                              output parameters, if applicable. NB: Do not
                              use memhandler to allocate memory for
                              @a dd_handle_ptr.
   @param[out] sensors        List of supported sensors, allocated, 
                              populated, and returned by this function.
   @param[out] num_sensors    Number of elements in @a sensors.
  
   @return Success if @a dd_handle_ptr was allocated and the driver was 
           configured properly. Otherwise a specific error code is returned.
 
*/
/*=========================================================================*/                                             
static sns_ddf_status_e sns_dd_alsprx_isl29011_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     reg_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{

  sns_ddf_status_e       stat = SNS_DDF_SUCCESS;
  sns_dd_alsprx_state_t* ptr;

  if ( (dd_ptr == NULL)      || 
       (num_sensors == NULL) || 
       (sensors == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_alsprx_state_t)) != SNS_DDF_SUCCESS )
  {
    return SNS_DDF_ENOMEM;
  }
  ptr->smgr_handle = smgr_handle;

  if ( (stat = sns_ddf_open_port(&ptr->port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS )
  {
    return stat;
  }

  *dd_ptr = (sns_ddf_handle_t)ptr;

  stat = sns_ddf_timer_init(&ptr->timer, 
                            *dd_ptr, 
                            &sns_alsprx_driver_fn_list, 
                            (void *)ptr, 
                            FALSE);
  if ( stat != SNS_DDF_SUCCESS )
  {
    return stat;
  }

  /* Consume registry data */
  ptr->sns_dd_alsprx_common_db.nv_source = SNS_DD_ALSPRX_NV_SOURCE_DEFAULT;
  if ((reg_params->status == SNS_DDF_SUCCESS) &&
      (reg_params->data_len >= sizeof(sns_dd_nv_db_type))&&
      (reg_params->data[0] != 0))
  {
    ptr->sns_dd_alsprx_common_db.nv_size = reg_params->data_len;
    ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = reg_params->data[0];
    ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = reg_params->data[1];
    ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = ((uint16_t)reg_params->data[3]<<8) | 
                                                     reg_params->data[2];
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = ((uint16_t)reg_params->data[5]<<8) | 
                                                       reg_params->data[4];
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = ((uint16_t)reg_params->data[7]<<8) | 
                                                       reg_params->data[6];
    ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = ((uint16_t)reg_params->data[9]<<8) |
                                                       reg_params->data[8];
    ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = ((uint16_t)reg_params->data[11]<<8) |
                                                       reg_params->data[10];
    ptr->sns_dd_alsprx_common_db.nv_source = SNS_DD_ALSPRX_NV_SOURCE_REG;
  }
  else
  {
    /* initialize NV item db to default values if needed */
    ptr->sns_dd_alsprx_common_db.nv_size = 128;
    ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO;
    ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = SNS_DD_IR_TRANS_RATIO;
    ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = SNS_DD_DC_OFFSET;
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = SNS_DD_PRX_THRESH_NEAR;
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = SNS_DD_PRX_THRESH_FAR;
    ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = SNS_DD_PRX_FACTOR;
    ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = SNS_DD_ALS_FACTOR;

    sns_ddf_smgr_notify_event(ptr->smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP); 

  }


#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               reg_params->status);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               reg_params->data_len);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               reg_params->nvitem_grp);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                               reg_params->nvitem_id);
#endif /* ALSPRX_DEBUG_REG */

  /* Fill out supported sensor info */
  *num_sensors = SNS_DD_ALSPRX_NUM_SENSOR_TYPES;
#if ALSPRX_DEBUG_ONE_SENSOR
  *num_sensors = 1;
#endif
  *sensors = (sns_ddf_sensor_e *)sns_dd_alsprx_sensor_types; 

  /* Resets the driver */
  stat = sns_dd_alsprx_isl29011_reset((sns_ddf_handle_t) ptr);

  return stat;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the ISL 29011 ALS/Prx sensor
 
  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.
 
  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM
 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29011_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( (dd_handle == NULL) || 
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      status = sns_dd_alsprx_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            TRUE);
      break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      /* set resolution */
      status = sns_dd_alsprx_set_resolution((sns_dd_alsprx_state_t *)dd_handle,
                                            sensor,
                                            *(sns_dd_alsprx_res_e *)value); 
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  return(status);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_prxdist_prepare_data_from_device
  
DESCRIPTION   For preparing a sensor data, this function issues a request to
              the proximity common handler

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
BOOLEAN sns_dd_alsprx_prxdist_prepare_data_from_device(sns_dd_alsprx_state_t* dd_ptr)
{
  if ( dd_ptr->sns_dd_prx_db.pend_state == SNS_DD_PEND_STATE_IDLE )
  {
    sns_dd_alsprx_prxcomm_req_data(dd_ptr);
    dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_PEND;
    return FALSE;
  }
  else
  {
    /* Already PEND state, so nothing is to be done in here */
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_STATE_PEND);
    return TRUE;
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_prepare_data_from_device
  
DESCRIPTION   For preparing a sensor data, this function issues a request to
              the arbitration manager

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
BOOLEAN  sns_dd_alsprx_als_prepare_data_from_device (sns_dd_alsprx_state_t* dd_ptr)
{
  if ( dd_ptr->sns_dd_als_db.pend_state != SNS_DD_PEND_STATE_PEND )
  {
    /* if the last sampling time is not long time ago, use the same range value */
    if ( ((uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp()) -
          dd_ptr->sns_dd_als_db.last_get_time) > SNS_DD_ALS_LONG_TIME_AGO_US )
    {
      /* start with default range */ 
      sns_dd_alsprx_arbit_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT, SNS_DD_ISL_ALS_DEF_RNG);
      dd_ptr->sns_dd_als_db.arbit_req_rng_sel = SNS_DD_ISL_ALS_DEF_RNG;
    }
    else
    {
      /* use the next range selection */
      sns_dd_alsprx_arbit_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT, dd_ptr->sns_dd_als_db.next_rng_sel);
      dd_ptr->sns_dd_als_db.arbit_req_rng_sel = dd_ptr->sns_dd_als_db.next_rng_sel;
    }
    dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_PEND;
    return FALSE;
  }
  else
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_DEV_STATE_PEND);
    return TRUE;

  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_common_request_data
  
DESCRIPTION   As a async driver, this driver needs preparing a sensor data. 
              This function calls an appropriate preparing function according to
              the data type.

DEPENDENCIES  None

RETURN VALUE  FALSE if successful, TRUE otherwise

SIDE EFFECT   None 
===========================================================================*/
BOOLEAN sns_dd_alsprx_common_request_data
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       data_type
)
{
  BOOLEAN                       err;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_DATA_REQ,
                                data_type);
#endif

  /* make sure device is in the right state */
  if ( dd_ptr->sns_dd_alsprx_common_db.state != SNS_DD_DEV_CONFIGURED )
  {
    /* device state is stopped OR invalid */
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                                 dd_ptr->sns_dd_alsprx_common_db.state);
    return TRUE;
  }

  /* Request data from device */
  if ( data_type == SNS_DDF_SENSOR_AMBIENT )
  {
    err = sns_dd_alsprx_als_prepare_data_from_device(dd_ptr);
  }
  else /* SNS_DDF_SENSOR_PROXIMITY */
  {
    err = sns_dd_alsprx_prxdist_prepare_data_from_device(dd_ptr);
  }

  return err;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data
 
  @detail
  Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested
 
  This driver is a pure asynchronous one, so no data will be written to buffer.
  As a result, the return value will be always PENDING if the process does
  not fail.  This driver will notify the Sensors Manager via asynchronous
  notification when data is available.
 
  @param[in]  dd_handle    Handle to a driver instance.
  @param[in]  sensors      List of sensors for which data is requested.
  @param[in]  num_sensors  Length of @a sensors.
  @param[in]  memhandler   Memory handler used to dynamically allocate 
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements must 
                           match @a num_sensors.
 
  @return SNS_DDF_SUCCESS if data was populated successfully. If any of the 
          sensors queried are to be read asynchronously SNS_DDF_PENDING is
          returned and data is via @a sns_ddf_smgr_data_notify() when
          available. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29011_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
  sns_ddf_status_e status = SNS_DDF_PENDING;
  uint8_t          i, requests=0;
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_DATA_REQ,
                                num_sensors);
#endif

  if ( (dd_handle == NULL) || (sensors == NULL) ||
       (num_sensors < 1) || (num_sensors > 2) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( dd_ptr->two_requests == TRUE )
  {
    /* get data called before previous one is finished */
    return SNS_DDF_EFAIL;
  }

  /* Look at all the requests */
  for ( i = 0; i < num_sensors; i++ )
  {
    if ( sensors[i] != SNS_DDF_SENSOR_AMBIENT &&
         sensors[i] != SNS_DDF_SENSOR_PROXIMITY )
    {
      SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_GET_DATA_INVALID_SENSOR_ERR,
                                   sensors[i]);
      status = SNS_DDF_EINVALID_PARAM;
      break;
    }

    if ( sns_dd_alsprx_common_request_data((sns_dd_alsprx_state_t *)dd_handle, 
                       sensors[i]) == TRUE )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_GET_DATA_REQUEST_ERR);
      status = SNS_DDF_EFAIL;
      break;
    }
    dd_ptr->num_requests++; 
    if (++requests == 2)
    {
      dd_ptr->two_requests = TRUE;/* assuming that the next get_data will not be
                                     received until this one completes */
    }
  }

  return status;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_prx_query
  
DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for proximitydistance data type 

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_prx_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t* thresh_ptr;
  uint16_t* accu_ptr;
  sns_ddf_power_info_s* power_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.001); /* From 1 mm*/
      device_ranges->max = FX_FLTTOFIX_Q16(0.05);  /* to 50 mm*/
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES_ADC,
                                    dd_ptr->sns_dd_prx_db.res);
      resp->bit_len = (uint32_t) dd_ptr->sns_dd_prx_db.res;
      resp->max_freq = 30;
      *value = resp;
      break;
    }
    
    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      q16_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(q16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *resp = SNS_DD_PRXDIST_RES;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES,
                                    *resp);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_PRXDIST_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_PWR,
                                    power_ptr->active_current); // add power_ptr->lowpower_current to F3
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_ACC,
                                    dd_ptr->sns_dd_prx_db.accuracy);
      *accu_ptr = dd_ptr->sns_dd_prx_db.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near;
      *(uint16_t **)value = thresh_ptr;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_THRESH,
                                    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near);
      break;

    default:
      SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_ATTRIB_ERR,
                                  attrib);
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_als_query
  
DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_als_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.015);
      device_ranges->max = FX_FLTTOFIX_Q16(32767.0);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      dd_ptr->sns_dd_als_db.res = sns_dd_als_rng_to_res_tb[dd_ptr->sns_dd_als_db.last_rng_sel];
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES_ADC,
                                    dd_ptr->sns_dd_als_db.res);
      resp->bit_len = (uint32_t) dd_ptr->sns_dd_als_db.res;
      resp->max_freq = 30;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = SNS_DD_ALS_RES;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES,
                                    *resp);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_ALS_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_PWR,
                                    power_ptr->active_current); // add power_ptr->lowpower_current to F3
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      dd_ptr->sns_dd_als_db.accuracy = dd_ptr->sns_dd_als_db.last_mlux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
      if ( dd_ptr->sns_dd_als_db.accuracy < SNS_DD_ALS_HALF_LUX )
      {
        dd_ptr->sns_dd_als_db.accuracy = SNS_DD_ALS_HALF_LUX;
      }
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_ACC,
                                    dd_ptr->sns_dd_als_db.accuracy);
      *accu_ptr = dd_ptr->sns_dd_als_db.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    default:
      SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_ATTRIB_ERR,
                                  attrib);
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29011_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  Returns the requested attribute
 
  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor      Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved. 
  @param[in]  memhandler  Memory handler used to dynamically allocate 
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set 
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    Success if the attribute was retrieved and the buffer was 
    populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_alsprx_isl29011_get_attrib
(
  sns_ddf_handle_t     handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)handle;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  BOOLEAN generic_attrib = FALSE;
  sns_ddf_driver_info_s *driver_info_ptr;
  sns_ddf_driver_info_s driver_info_l = {
    "ISL29011", /* name */
    1           /* version */
  };
  sns_ddf_device_info_s *device_info_ptr;
  sns_ddf_device_info_s device_info_l = {
    "ALSPRX",   /* name */
    "Intersil", /* vendor */
    "ISL29011", /* model */
    1           /* version */
  };
  sns_ddf_registry_group_s *reg_group_ptr;
  uint8_t *reg_group_data_ptr;

  if ( (handle == NULL)      || 
       (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_ATTRIB_REQ,
                                sensor,
                                attrib );
  
  /* check first if the query is for generic attributes */
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *driver_info_ptr = driver_info_l; /* Is memcopy needed because of strings */
      *(sns_ddf_driver_info_s**)value = driver_info_ptr;
      *num_elems = 1;
      generic_attrib = TRUE;
      break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *device_info_ptr = device_info_l;
      *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
      *num_elems = 1;
      generic_attrib = TRUE;
      break;

    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
      if ( (reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
                        
      if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, dd_ptr->sns_dd_alsprx_common_db.nv_size)) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      
      SNS_OS_MEMCOPY(reg_group_data_ptr, &dd_ptr->sns_dd_alsprx_common_db.nv_db, sizeof(sns_dd_nv_db_type));

      reg_group_ptr->group_data = reg_group_data_ptr;
      reg_group_ptr->size = dd_ptr->sns_dd_alsprx_common_db.nv_size;    
      *(sns_ddf_registry_group_s**)value = reg_group_ptr;
      *num_elems = 1;
      generic_attrib = true;      
      break;      

    default:
      /* do nothing */
      break;
  }

  /* The query is not for generic attribute but is for specific data type */
  if ( generic_attrib != TRUE )
  {
    switch ( sensor )
    {
      case SNS_DDF_SENSOR_AMBIENT:
        status = sns_dd_alsprx_als_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      case SNS_DDF_SENSOR_PROXIMITY:
        status = sns_dd_alsprx_prx_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      default:
        SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_SENSOR_TYPE_ERR,
                                    sensor);
        status = SNS_DDF_EINVALID_ATTR;
    }
  }

  return status;
}
