/*  Date: 2013/09/05 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the kxtik accelerometer Sensor

   Copyright (c) 2013-2015 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/pkg/dsps/dev/yihengz.adsp821_3/adsp_proc/Sensors/dd/qcom/src/sns_dd_kxtik.c


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/09/15     AH      Fix compiler warnings
08/23/14     vy      Fixed msg type in a debug print msg
07/22/14     MW      Eliminate compiler warning
03/12/14     MW      Fixed KW issue
09/05/13     yh      Initial version of driver for kxtik
==============================================================================*/
/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_dd_kxtik.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_debug_str.h"
#include "sns_ddf_signal.h"
#include "sns_debug_api.h"

#include "sns_dd_inertial_test.h"



//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"


#if 0
#define NULL ((void *)0) 
#endif /* DDK Only */



/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/

static const uint32_t  sns_dd_accel_range[3] =
{
  	KXTIK_ACCEL_SENSOR_RANGE_2G,
  	KXTIK_ACCEL_SENSOR_RANGE_4G,
  	KXTIK_ACCEL_SENSOR_RANGE_8G
  
};

static const uint32_t  sns_dd_accel_bw[7] =
{
	KXTIK_BW_12_5HZ,
	KXTIK_BW_25HZ,
	KXTIK_BW_50HZ,
	KXTIK_BW_100HZ,
	KXTIK_BW_200HZ,
	KXTIK_BW_400HZ,
	KXTIK_BW_800HZ

};

static const uint32_t sns_dd_accel_freq[7] =
{    
 	KXTIK_ACCEL_SENSOR_BW_VALUE_12_5Hz,      
 	KXTIK_ACCEL_SENSOR_BW_VALUE_25Hz,    
 	KXTIK_ACCEL_SENSOR_BW_VALUE_50Hz,     
 	KXTIK_ACCEL_SENSOR_BW_VALUE_100Hz,    
 	KXTIK_ACCEL_SENSOR_BW_VALUE_200Hz,      
 	KXTIK_ACCEL_SENSOR_BW_VALUE_400Hz,   
 	KXTIK_ACCEL_SENSOR_BW_VALUE_800Hz     
    
};



static const uint32_t  MAX_LSB_VALUE[2] =
{
  MAX_LSB_8BIT,
  MAX_LSB_12BIT
};

const sns_ddf_odr_t kxtik_odr[KIONIX_KXTIK_ODR_NUM] = {
  12,
  25,
  50,
  100,
  200,
  400,
  800
};


/*=======================================================================

                   INTERNAL ENUMS

========================================================================*/
/* Supported datatypes */
typedef enum
{
   SDD_ACC_X = 0,
   SDD_ACC_Y,
   SDD_ACC_Z,
   SDD_ACC_NUM_AXES,
   SDD_ACC_NUM_DATATYPES
} sdd_acc_data_type_e;


/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_acc_kxtik_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_acc_kxtik_set_attr( 
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_acc_kxtik_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static void sns_dd_acc_kxtik_interrupt_handler(
        sns_ddf_handle_t       dd_handle,
        uint32_t               gpio_num,
        sns_ddf_time_t         timestamp);

static sns_ddf_status_e sns_dd_acc_kxtik_reset(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_kxtik_self_test( 
        sns_ddf_handle_t dd_handle, 
        sns_ddf_sensor_e sensor, 
        sns_ddf_test_e test,
        uint32_t* err);
sns_ddf_status_e sns_dd_acc_kxtik_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
);
static sns_ddf_status_e sns_dd_acc_kionix_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors );



/*=======================================	================================

                   STRUCTURE DEFINITIONS

========================================================================*/
typedef struct kxtikacc {
	int16_t	x;
	int16_t	y;
	int16_t	z;
} kxtik;

sns_ddf_driver_if_s sns_accel_kxtik_driver_fn_list =
{
  &sns_dd_acc_kxtik_init,
  &sns_dd_acc_kxtik_get_data,
  &sns_dd_acc_kxtik_set_attr,
  &sns_dd_acc_kxtik_get_attr,
  NULL,
  &sns_dd_acc_kxtik_interrupt_handler,
  &sns_dd_acc_kxtik_reset,
  &sns_dd_acc_kxtik_self_test,
  &sns_dd_acc_kxtik_enable_sched_data,
  &sns_dd_acc_kionix_probe
  
};

/*!  State struct for kxtik driver */
typedef struct {
  /*! SDDI handle used to notify_data */
  sns_ddf_handle_t smgr_hndl; 
  /*! Accelerometer bias */
  q16_t            bias[SDD_ACC_NUM_AXES];
  /*! data cache */
  q16_t            data_cache[SDD_ACC_NUM_DATATYPES]; 
  uint8_t          out_of_bound_flag;
  /*! handle used to access the I2C bus */
  sns_ddf_handle_t port_handle;
  /*! current range selection */
  uint8_t          range;
  /*! current lowpass filter BW */
  uint16_t         lowpass_bw;  
  bool             motion_detect_enabled;
  uint8_t           sensor_mode;
  /*! current powerstate*/
  sns_ddf_powerstate_e powerstate; 
  /*! time of last update */
  sns_ddf_time_t data_cache_time; 
  sns_ddf_status_e data_status[SDD_ACC_NUM_DATATYPES];
  /*! axes map for this form factor received at init */
  sns_ddf_axes_map_s axes_map;
  /*! GPIO number used for interrupts */
  uint32_t gpio_num; 
  bool int_enable;
  sns_ddf_odr_t acc_cur_rate;
  sns_ddf_sensor_data_s data_ptr[SDD_ACC_NUM_AXES];
  sns_ddf_sensor_sample_s data_sample[SDD_ACC_NUM_AXES];
}sns_dd_acc_kxtik_state_t;


static sns_ddf_status_e sns_dd_acc_kxtik_config_motion_int(
   sns_dd_acc_kxtik_state_t  *state,
   bool                       enable);

static sns_ddf_status_e sns_dd_acc_kxtik_config_motion_detect(
    sns_dd_acc_kxtik_state_t  *state);



/*============================================================================

  Global Data Definitions

 ============================================================================*/


static sns_ddf_sensor_e my_sensors[] = 
  {
    SNS_DDF_SENSOR_ACCEL
  };


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_range

===========================================================================*/
/*!
* @brief Sets the kionix acclerometer to the passed in range
*
* @detail
*   The range information is taken from the driver structure whcih is
*   set to kionix_ACCEL_SENSOR_RANGE_2G as part of the reset function
*   before calling this one.
* 
* 
*  @param[in] state: Ptr to the driver structure
*
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_config_range
(
  sns_dd_acc_kxtik_state_t *state
)
{
  uint8_t kxtik_write_buffer, out;
  uint8_t kxtik_read_buffer = 0;
  uint8_t buffer = 0;
  sns_ddf_status_e stat;
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config range --------");
  /*! set range and bandwidth */
  kxtik_write_buffer = state->range;
  
  if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CTRL_REG1, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }  
  kxtik_read_buffer &= 0x7F;
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  
  kxtik_write_buffer = (kxtik_write_buffer | (((kxtik_read_buffer)|(0x80))&(0xE7)));
  
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config range ++++++++");	
  return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_bw


===========================================================================*/
/*!
*  @brief Sets the kionix acclerometer to the passed in bandwidth
*
*
*  @detail
*
*    The bandwidth information is taken from the driver structure whcih is
*    set to kxtik_BW_31_25HZ as part of the reset function
*
*    before calling this one.
* 
* 
*  @param[in] state: Ptr to the driver structure
* 
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_config_bw
(
  sns_dd_acc_kxtik_state_t *state
)
{
  uint8_t kxtik_write_buffer, out, bufferbw;
  uint8_t kxtik_read_buffer = 0;
  uint8_t buffer = 0;
  uint8_t buffer1 = 0x38;
  sns_ddf_status_e stat;
 
  /*! set bandwidth */
  kxtik_write_buffer = state->lowpass_bw;
  if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CTRL_REG1, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }
  kxtik_read_buffer &= 0x7F;
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_BW_SEL_REG,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_INT_CTRL_REG,
                         &buffer1,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_BW_SEL_REG, 
                  &bufferbw,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  } 
  kxtik_write_buffer = (((kxtik_read_buffer)|(0x80)|(0x60)));
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_default

===========================================================================*/
/*!
*  @brief Sets the kionix acclerometer to its default operational state
* 
*  @detail
*  Programs the register kionix_ACCEL_SENSOR_CTRL5_REG_ADDR for the MD
*  threshold
* 
*  @param[in] state: Ptr to the driver structure
* 
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
 
static sns_ddf_status_e sns_dd_acc_kxtik_config_default
(
  sns_dd_acc_kxtik_state_t *state
)
{
  uint8_t out;

  sns_ddf_status_e stat;
  
  uint8_t chip_id;
  //DDF_MSG_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config default in ++++++++");
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config default in ++++++");
  if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CHIP_ID_REG, 
                  &chip_id,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

  sns_ddf_delay(10000);
  
  /*! set the range */
  /*
  if ( (stat = sns_dd_acc_kxtik_config_mode((sns_dd_acc_kxtik_state_t*)dd_handle,
                                           (sns_ddf_powerstate_e *)value)) != SNS_DDF_SUCCESS)
  {
  return stat;
  }
  */
  if ( (stat = sns_dd_acc_kxtik_config_range(state)) != SNS_DDF_SUCCESS)
  {
  return stat;
  }


  /*! set the bandwidth */
  if ( (stat = sns_dd_acc_kxtik_config_bw(state)) != SNS_DDF_SUCCESS)
  {
  return stat;
  }
  //DDF_MSG_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config default out -------");

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_acc_kxtik_match_odr

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e  sns_dd_acc_kxtik_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
  uint8_t idx;

  if((0 == desired_rate) || (desired_rate > KIONIX_KXTIK_MAX_ODR))
    return SNS_DDF_EINVALID_PARAM;

  for(idx=0; idx < KIONIX_KXTIK_ODR_NUM; idx++)
  {
    if(desired_rate <= kxtik_odr[idx]) {
      break;
    }
  }

  if(idx >= KIONIX_KXTIK_ODR_NUM)
  {
    // if desired_rate > max supported ODR then use
    // max supported ODR
    idx = (KIONIX_KXTIK_ODR_NUM-1);
  }

  *new_rate = kxtik_odr[idx];
  *new_index = idx;
  SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "new_rate %d  !!!!", &new_rate);
  return SNS_DDF_SUCCESS;
}


/*===========================================================================

FUNCTION      sns_dd_kxtik_enable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_kxtik_enable_interrupt
(
  sns_dd_acc_kxtik_state_t* state,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_status_e stat;
  uint8_t kxtik_write_buffer, out;
  uint8_t kxtik_read_buffer = 0;
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "sns_dd_kxtik_enable_interrupt  !!!!");
  if (sensor != SNS_DDF_SENSOR_ACCEL)
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  else 
  {
    if (state->int_enable == TRUE)
    {
      return SNS_DDF_SUCCESS;
    }
	else
	{	
		status = sns_ddf_signal_register(state->gpio_num, state, &sns_accel_kxtik_driver_fn_list, SNS_DDF_SIGNAL_IRQ_RISING);
		if (status != SNS_DDF_SUCCESS)
		{ 
			SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "register failed gpio = %d", state->gpio_num);
			SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "register failed");
		}
		if ( (status = sns_ddf_read_port(state->port_handle,
                  KXTIK_INT_REL, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    	{
      		return status;
    	}
		status = sns_ddf_signal_register(state->gpio_num, state, &sns_accel_kxtik_driver_fn_list, SNS_DDF_SIGNAL_IRQ_RISING);
		if (status != SNS_DDF_SUCCESS)
		{ 
			SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "register failed gpio = %d", state->gpio_num);
			SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "register failed");
		}
		SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "sns_dd_kxtik_enable_interrupt --!!");
		state->int_enable = TRUE;
		if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CTRL_REG1, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    	{
    	  return stat;
    	}
		SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "ctrl_reg1 = %d", kxtik_read_buffer);
		kxtik_write_buffer = 0;
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
 		}
		kxtik_write_buffer = (KXTIK_IEN | KXTIK_IEA | KXTIK_IEL);
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_INT_CTRL_REG,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
 		}
		kxtik_write_buffer = ((kxtik_read_buffer | 0x20) & 0xFA);
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
 		}
		SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "sns_dd_kxtik_enable_interrupt --- %d", status);
	}
  }
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_kxtik_disable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_kxtik_disable_interrupt
(
  sns_dd_acc_kxtik_state_t* state,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_status_e stat;
  uint8_t kxtik_write_buffer, out;
  uint8_t kxtik_read_buffer = 0;
  if (sensor != SNS_DDF_SENSOR_ACCEL)
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  else 
  {
  	if (state->int_enable == false || state->motion_detect_enabled == true)
  	{
	  return SNS_DDF_SUCCESS;
  	}
	else
	{
	    status = sns_ddf_signal_deregister(state->gpio_num);
	    if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CTRL_REG1, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    	{
    	  return stat;
    	}  	
		kxtik_write_buffer = 0;
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
 		}
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_INT_CTRL_REG,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
		}
		state->int_enable = false;
		if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_read_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  		{
    	  return stat;
 		}
		
 	}
  }
  return status;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_reset

===========================================================================*/
/*!
*  @brief Resets the driver and device so they return to the state they were 
*         in after init() was called.
* 
*  @detail
*   resets the bias to zero & range to kionix_ACCEL_SENSOR_RANGE_2G which
*   is stored in the driver state structure
*   Calls the sns_dd_acc_kxtik_config_default routine
* 
*  @param[in] state: Ptr to the driver structure
* 
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_reset(sns_ddf_handle_t dd_handle)
{
  uint8_t i;
  sns_dd_acc_kxtik_state_t *state = (sns_dd_acc_kxtik_state_t *)dd_handle;
  
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik reset in ++++++++");
  for (i = 0; i < SDD_ACC_NUM_AXES; i++) 
  {
    state->bias[i] = 0;
  }
  state->range = KXTIK_ACCEL_SENSOR_RANGE_2G;
  state->lowpass_bw = KXTIK_BW_25HZ;
  state->out_of_bound_flag = 0;
  state->sensor_mode = HIGH_CURRENT;
  state->motion_detect_enabled = false;
  state->int_enable = false;
  state->acc_cur_rate = 10;
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik reset out --------");
  return sns_dd_acc_kxtik_config_default(state);

}

#if 0
#define kxtik_GET_BITSLICE(regvar, bitname)\
  ((regvar & bitname##__MSK) >> bitname##__POS)


#define kxtik_SET_BITSLICE(regvar, bitname, val)\
  ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))
#endif

sns_ddf_status_e kxtik_smbus_read_byte_block(sns_ddf_handle_t port_handle,
    unsigned char reg_addr, unsigned char *data, unsigned char len)
{


  sns_ddf_status_e stat;
  uint8_t out;

  if ( (stat = sns_ddf_read_port(port_handle,
                          reg_addr, 
                          data,
                          len,
                          &out)) != SNS_DDF_SUCCESS)
    {
       return stat;
    }
    return stat;

  
}



/*===========================================================================

  FUNCTION:   kxtik_read_accel_xyz

===========================================================================*/

sns_ddf_status_e kxtik_read_accel_xyz(sns_ddf_handle_t port_handle,
    signed char sensor_mode, struct kxtikacc *acc)

{
  sns_ddf_status_e comres = SNS_DDF_EINVALID_DATA;
  unsigned char buffer[6] = {0};
  //DDF_MSG_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik read xyz ++++++++");
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik read xyz begin ++++++++");
  switch (sensor_mode) {
  case 0:
    SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik read xyz case 0++++++++");
	comres = kxtik_smbus_read_byte_block(port_handle,
        KXTIK_OUTX_L, buffer, 6);
/*
	acc->x = (((data[0] & 0xF0) >> 4)|((data[1] << 4) & 0xFF0));
	acc->y = (((data[2] & 0xF0) >> 4)|((data[3] << 4) & 0xFF0));
	acc->z = (((data[4] & 0xF0) >> 4)|((data[5] << 4) & 0xFF0));
*/  
	if (buffer[1] & 0x80) {
			acc->x = (buffer[1] - 256);
		}
		else {
			acc->x = buffer[1];
		}
	if (buffer[3] & 0x80) {
			acc->y = (buffer[3] - 256);
		}
		else {
			acc->y = buffer[3];
		}
	if (buffer[5] & 0x80) {
			acc->z = (buffer[5] - 256);
		}
		else {
			acc->z = buffer[5];
		}
    break;
  case 1:
  	SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik read xyz case 1++++++++");
    comres = kxtik_smbus_read_byte_block(port_handle,
        KXTIK_OUTX_L, buffer, 6);
/*
	acc->x = (((data[0] & 0xF0) >> 4)|((data[1] << 4) & 0xFF0));
	acc->y = (((data[2] & 0xF0) >> 4)|((data[3] << 4) & 0xFF0));
	acc->z = (((data[4] & 0xF0) >> 4)|((data[5] << 4) & 0xFF0));
*/
	if (buffer[1] & 0x80){
			acc->x = (((int)buffer[1]<<4) + (buffer[0]>>4) - 4096);
		}
		else {
			acc->x = (((int)buffer[1]<<4) + (buffer[0]>>4));
		}
	if (buffer[3] & 0x80){
			acc->y = (((int)buffer[3]<<4) + (buffer[2]>>4) - 4096);
		}
		else {
			acc->y = (((int)buffer[3]<<4) + (buffer[2]>>4));
		}
	if (buffer[5] & 0x80){
			acc->z = (((int)buffer[5]<<4) + (buffer[4]>>4) - 4096);
		}
		else {
			acc->z = (((int)buffer[5]<<4) + (buffer[4]>>4));
		}
    break;
  default:
  	if (buffer[1] & 0x80){
			acc->x = (((int)buffer[1]<<4) + (buffer[0]>>4) - 4096);
		}
		else {
			acc->x = (((int)buffer[1]<<4) + (buffer[0]>>4));
		}
	if (buffer[3] & 0x80){
			acc->y = (((int)buffer[3]<<4) + (buffer[2]>>4) - 4096);
		}
		else {
			acc->y = (((int)buffer[3]<<4) + (buffer[2]>>4));
		}
	if (buffer[5] & 0x80){
			acc->z = (((int)buffer[5]<<4) + (buffer[4]>>4) - 4096);
		}
		else {
			acc->z = (((int)buffer[5]<<4) + (buffer[4]>>4));
		}
    break;
  }
  
  
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik read xyz finish ++++++++");
  return comres;
}


#define abs(x) ((x) < 0 ? -(x):(x))

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_init

===========================================================================*/

/*!
*  @brief Initializes the 3 axis kionix accelerometer kxtik
*              determines the device to use
* 
*  @detail
*  - Allocates memory for driver state structure.
*  Opens the device port by calling sns_ddf_open_port routine
*  Calls sns_dd_acc_kxtik_reset routine
*
*
* @param[out] dd_handle_ptr  Pointer that this function must malloc and
*                             populate. This is a handle to the driver
*                             instance that will be passed in to all
*                             other functions.
*  @param[in]  smgr_handle    Handle used to identify this driver when it
*                             calls into Sensors Manager functions.
*  @param[in]  nv_params      NV parameters retrieved for the driver.
*  @param[in]  device_info    Information describing each of the physical
*                             devices controlled by this driver. This is
*                             used to configure the bus and talk to the
*                             device.
*  @param[in]  memhandler     Memory handler used to dynamically allocate 
*                             output parameters, if applicable. NB: Do
*                             not use memhandler to allocate memory for
*                             @a dd_handle_ptr.
* @param[in]  num_devices    Length of @a device_info.
* @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
*  @param[out] num_sensors    Length of @a sensors.
*
*
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/                                             
/*=========================================================================*/                                             
static sns_ddf_status_e sns_dd_acc_kxtik_init(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)                                  
{                                                            
  sns_ddf_status_e stat;
  sns_dd_acc_kxtik_state_t *ptr;
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik init in ++++++++");
  if(sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_acc_kxtik_state_t)) != SNS_DDF_SUCCESS) 
  { 
    return SNS_DDF_ENOMEM;
  }
  ptr->smgr_hndl = smgr_handle;
  ptr->gpio_num = device_info->first_gpio;
  ptr->motion_detect_enabled = false;

  if((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    return stat;
  }

  *num_sensors = 1;
  *sensors = my_sensors;
  *dd_handle_ptr = (sns_ddf_handle_t)ptr;
  stat = sns_dd_acc_kxtik_reset(ptr);
  
  sns_ddf_axes_map_init(
      &ptr->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));
  
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik init out ---------");
  return stat;
}
/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_mode

===========================================================================*/
/*!
*  @brief Sets the kionix acclerometer to the passed in operating mode
* 
*  @detail
*  Set the operational mode (sleep or NORMAL_MODE) by programming
*  kxtik_MODE_CTRL_REG register
* 
*  @param[in] state: Ptr to the driver structure
*  @param[in] mode: Ptr to the variable of sns_ddf_powerstate_e type
* 
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_config_mode
(
   sns_dd_acc_kxtik_state_t *state,
   sns_ddf_powerstate_e      *mode
)
{
   sns_ddf_status_e status;
   uint8_t kxtik_write_buffer, out;
   SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config mode in ++++++++");
   
   if ( (status = sns_ddf_read_port(state->port_handle,
                                 KXTIK_CTRL_REG1,
                                 &kxtik_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
   {
     return status;
   } 
   
   SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config mode %d", kxtik_write_buffer); 
   switch (*mode)
   {
   case SNS_DDF_POWERSTATE_ACTIVE:
   	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik normal power mode ++++++++");
   	  state->sensor_mode = HIGH_CURRENT;
      kxtik_write_buffer = ((kxtik_write_buffer & 0xBF) | KXTIK_ACCEL_SENSOR_NORMAL_MODE);
	  SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config normal mode %d", kxtik_write_buffer); 
      break;
   case SNS_DDF_POWERSTATE_LOWPOWER: 
   	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik low power mode -------");
   	  state->sensor_mode = LOW_CURRENT;
	  kxtik_write_buffer = ((kxtik_write_buffer & 0xBF) | KXTIK_ACCEL_SENSOR_SLEEP_MODE);
	  SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config lowpower mode %d", kxtik_write_buffer); 
      break;
   default:
   	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " default kxtik default power mode ++++++++");
   	  state->sensor_mode = HIGH_CURRENT;
      kxtik_write_buffer = ((kxtik_write_buffer & 0xBF) | KXTIK_ACCEL_SENSOR_NORMAL_MODE);
      return SNS_DDF_EINVALID_PARAM;

   }
   kxtik_write_buffer &= 0x7F;
   SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config mode1 %d", kxtik_write_buffer); 
   if ( (status = sns_ddf_write_port(state->port_handle,
                          KXTIK_CTRL_REG1,
                          &kxtik_write_buffer,
                          1,
                          &out)) != SNS_DDF_SUCCESS)
   {
     return status;
   }
   /*! Set opmode */
   kxtik_write_buffer |=0x80;
   SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config mode2 %d", kxtik_write_buffer); 
   if ( (status = sns_ddf_write_port(state->port_handle,
                          KXTIK_CTRL_REG1,
                          &kxtik_write_buffer,
                          1,
                          &out)) != SNS_DDF_SUCCESS)
   {
     return status;
   }
   SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config mode done -------");
   
   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_set_attr

===========================================================================*/
/*!
*  @brief Sets an attribute of the kionix accelerometer
* 
* @detail
*  Called by SMGR to set certain device attributes that are
*  programmable. Curently its the power mode and range.
* 
*  @param[in] dd_handle   Handle to a driver instance.
*  @param[in] sensor Sensor for which this attribute is to be set.
*  @param[in] attrib      Attribute to be set.
*  @param[in] value      Value to set this attribute.
*
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
*    SNS_DDF_EINVALID_PARAM
*
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_set_attr 
( 
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_odr_t new_rate = 0;
  uint32_t new_rate_index = 0;
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr ++++++++");
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
       /*! set power mode */
       ret_val = sns_dd_acc_kxtik_config_mode((sns_dd_acc_kxtik_state_t*)dd_handle,
                                           (sns_ddf_powerstate_e *)value);
    break;

    case SNS_DDF_ATTRIB_RANGE:
	{
      /*! change the range for accel */
      if (sensor == SNS_DDF_SENSOR_ACCEL) 
      {
        if(*((uint8_t *)value) >= SNS_DD_ACCEL_MAX_RANGE)
        {
          return SNS_DDF_EINVALID_ATTR;
        }
        else
        {
          ((sns_dd_acc_kxtik_state_t*)dd_handle)->range = sns_dd_accel_range[*((uint8_t *)value)];
        }

        ret_val = sns_dd_acc_kxtik_config_range((sns_dd_acc_kxtik_state_t*)dd_handle);
      }
    	break;
    }
    case SNS_DDF_ATTRIB_ODR:
	{
	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr ODR!! -------");
	  if (sensor == SNS_DDF_SENSOR_ACCEL)
	  {
		status =  sns_dd_acc_kxtik_match_odr(*(sns_ddf_odr_t*)value,
              &new_rate,
              &new_rate_index);
	  }
	  if(SNS_DDF_SUCCESS != status)
	  {
        return SNS_DDF_EINVALID_PARAM;
	  }	
	  if(new_rate != ((sns_dd_acc_kxtik_state_t*)dd_handle)->acc_cur_rate)
	  {
	    ((sns_dd_acc_kxtik_state_t*)dd_handle)->lowpass_bw = sns_dd_accel_bw[new_rate_index];
	  	status = sns_dd_acc_kxtik_config_bw((sns_dd_acc_kxtik_state_t*)dd_handle);
        if(SNS_DDF_SUCCESS == status)
        {
        	((sns_dd_acc_kxtik_state_t*)dd_handle)->acc_cur_rate = new_rate;
            SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr ODR cur rate %d -------", ((sns_dd_acc_kxtik_state_t*)dd_handle)->acc_cur_rate);
            return status;
        }
		else
		{
			return status;
		}
	  }
	  else 
	  {
	  	return SNS_DDF_SUCCESS;
	  }
	  break;
    }
    case SNS_DDF_ATTRIB_LOWPASS:
	{
      /*! change the bandwidth for accel */
      if (sensor == SNS_DDF_SENSOR_ACCEL) 
      {
        SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " !! kxtik set BW value %d", *((uint8_t *)value));
        if(*((uint8_t *)value) >= SNS_DD_ACCEL_MAX_BW)
        {
          SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set BW %d", *((uint8_t *)value));
          return SNS_DDF_EINVALID_ATTR;
        }
        else
        {
          ((sns_dd_acc_kxtik_state_t*)dd_handle)->lowpass_bw = sns_dd_accel_bw[*((uint8_t *)value)];
        }

        ret_val = sns_dd_acc_kxtik_config_bw((sns_dd_acc_kxtik_state_t*)dd_handle);
      }

    	break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
     
    break;

    case SNS_DDF_ATTRIB_MOTION_DETECT:
      {
          bool* enable = value;
		  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr motion detect -------");
          if(!sns_ddf_signal_irq_enabled())
          {
               return SNS_DDF_EINVALID_PARAM;
          }
          
          if(*enable)
          {
            
        	if ( (ret_val = sns_dd_acc_kxtik_config_motion_detect(
          	(sns_dd_acc_kxtik_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
          	{
             	return ret_val;
          	}
        	if ( (ret_val = sns_dd_acc_kxtik_config_motion_int(
          	(sns_dd_acc_kxtik_state_t*)dd_handle,1)) != SNS_DDF_SUCCESS)
          	{
          	   return ret_val;
          	}
			SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr motion detect1 -------");
              /*! Register to receive notification when the interrupt fires */
              return sns_ddf_signal_register(
           	((sns_dd_acc_kxtik_state_t*)dd_handle)->gpio_num,
                  dd_handle,
                  &sns_accel_kxtik_driver_fn_list,
                  SNS_DDF_SIGNAL_IRQ_HIGH);
          }
          else
          {
             if(((sns_dd_acc_kxtik_state_t*)dd_handle)->motion_detect_enabled)
             {
                if ( (ret_val = sns_dd_acc_kxtik_config_motion_int(
                  (sns_dd_acc_kxtik_state_t*)dd_handle,0)) != SNS_DDF_SUCCESS)
               {
                 return ret_val;
               }
               return sns_ddf_signal_deregister(((sns_dd_acc_kxtik_state_t*)dd_handle)->gpio_num);
             }
             else
             {
               return SNS_DDF_SUCCESS;  
             }
          }
      }


      default:
            return SNS_DDF_EINVALID_PARAM;
  }
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik set attr done -------");
  return(ret_val);
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_get_data_all

===========================================================================*/
/*!
*  @brief Gets the acceleration data from the kionix accelerometer as well
*  as the temperature
* 
*  @detail
*    read the accel data and temp data registers and scales the values
*    and stores them in the driver state structure in Q16 format
* 
*  @param[in] state: Ptr to the driver structure
* 
*  @return
*    The error code definition within the DDF 
*   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
*
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_get_data_all
(
  sns_dd_acc_kxtik_state_t *state
)
  {
    sns_ddf_status_e stat;
    struct kxtikacc acc;

	SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik get data all ++++++++");
    stat = kxtik_read_accel_xyz(state->port_handle, state->sensor_mode, &acc);
    if(stat != SNS_DDF_SUCCESS)
    {
        SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "read error = %d", stat);
    }
    if(state->sensor_mode == 0){
    	switch (state->range)
    	{
	     case KXTIK_ACCEL_SENSOR_RANGE_2G:
	      	state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	break;
	     case KXTIK_ACCEL_SENSOR_RANGE_4G:
		    state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    break;
	     case KXTIK_ACCEL_SENSOR_RANGE_8G:
	        state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
  	        state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
  	        break;
	     default:
	        state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        break;
    	}
    }else if (state->sensor_mode == 1){
    	switch (state->range)
    	{
	     case KXTIK_ACCEL_SENSOR_RANGE_2G:
	      	state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	      	break;
	     case KXTIK_ACCEL_SENSOR_RANGE_4G:
		    state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*4*G/MAX_LSB_VALUE[state->sensor_mode]);
		    break;
	     case KXTIK_ACCEL_SENSOR_RANGE_8G:
	        state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
  	        state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*8*G/MAX_LSB_VALUE[state->sensor_mode]);
  	        break;
	     default:
	        state->data_cache[SDD_ACC_X] = FX_FLTTOFIX_Q16(acc.x * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Y] = FX_FLTTOFIX_Q16(acc.y * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        state->data_cache[SDD_ACC_Z] = FX_FLTTOFIX_Q16(acc.z * 2*2*G/MAX_LSB_VALUE[state->sensor_mode]);
	        break;
    	}
    }
    
    return SNS_DDF_SUCCESS;
  }

/*===========================================================================

  FUNCTION:   sns_dd_log_data

===========================================================================*/

/*!
*  @brief log sensor data
* 
*  @detail
*   Logs latest set of sensor data sampled from the sensor 
* 
*  @param[in] state: ptr to the driver structure
*  @param[in] sample_time: Time that the sensor was sampled
*
*
*  @return
*  
*/
/*=========================================================================*/
void sns_dd_log_data(
   sns_dd_acc_kxtik_state_t *state,
   sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  
  
  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "kxtik Log Data+++++++++++");
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_KIONIX;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = SDD_ACC_NUM_DATATYPES;
    log_struct_ptr->data[0]  = state->data_cache[SDD_ACC_Y];
    log_struct_ptr->data[1] = state->data_cache[SDD_ACC_X];
    log_struct_ptr->data[2]  = state->data_cache[SDD_ACC_Z];
    


    //Commit log (also frees up the log packet memory)
    err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, 
                                   log_struct_ptr);
  }
  
  if (err_code != SNS_SUCCESS)
  {
    DDF_MSG_1(HIGH, "kxtik Log Data - logpkt_malloc failed with err: %d", err_code);
  }
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_motion_int

===========================================================================*/
/*!
*  @brief Configure (enable/disable) the motion interrupt
* 
*  @detail
*  @param[in] state: Ptr to the driver structure
*  @param[in] enable: If true enable the interrupt if false disable the
*  interrupt
*
*  @return
*   The error code definition within the DDF 
*   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_config_motion_int
(
   sns_dd_acc_kxtik_state_t  *state,
   bool                        enable
)
{
   sns_ddf_status_e status = SNS_DDF_SUCCESS;
   uint8_t kxtik_write_buffer, out;

   if ( (status = sns_ddf_read_port(state->port_handle,
                                 KXTIK_INT_CTRL_REG2, 
                                 &kxtik_write_buffer,
                                 1,
                                 &out)) == SNS_DDF_SUCCESS)
   {
      if (enable)
      {

         kxtik_write_buffer |= KXTIK_INT_ENABLE2;
      
      }
      else
      {

         kxtik_write_buffer &= ~(KXTIK_INT_ENABLE2);
     
      }

      status = sns_ddf_write_port(state->port_handle,
                               KXTIK_INT_CTRL_REG2,
                               &kxtik_write_buffer,
                               1,
                               &out);
   }

   if(status == SNS_DDF_SUCCESS)
   {
       state->motion_detect_enabled = enable;
   }
   
   return status;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_reset_kxtik_motion_int

===========================================================================*/
/*!
*  @brief Resets KIONIX interrupt latched state
* 
* 
*  
*   @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_reset_kionix_motion_int
(
   sns_dd_acc_kxtik_state_t  *state
)
{
   uint8_t kxtik_write_buffer, out;
   uint8_t kxtik_read_buffer = 0;
   sns_ddf_status_e stat;
   
   SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "sns_dd_acc_kxtik_reset_kionix_motion_int ");
   if ( (stat = sns_ddf_read_port(state->port_handle,
                KXTIK_CTRL_REG1, 
                &kxtik_read_buffer,
                1,
                &out)) == SNS_DDF_SUCCESS)
   {
   	  return stat;
   }
   kxtik_write_buffer = 0;
   if((stat = sns_ddf_write_port(state->port_handle,
                               KXTIK_CTRL_REG1,
                               &kxtik_write_buffer,
                               1,
                               &out)) != SNS_DDF_SUCCESS)
   {
      return stat; 
   }
   
   kxtik_write_buffer = KXTIK_INT_ENABLE;   
   if((stat = sns_ddf_write_port(state->port_handle,
                               KXTIK_INT_CTRL_REG,
                               &kxtik_write_buffer,
                               1,
                               &out)) != SNS_DDF_SUCCESS)
   {
      return stat; 
   }
   kxtik_read_buffer |= (1 << 6);
   if((stat = sns_ddf_write_port(state->port_handle,
                               KXTIK_CTRL_REG1,
                               &kxtik_read_buffer,
                               1,
                               &out)) != SNS_DDF_SUCCESS)
   {
      return stat; 
   }

   return stat;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_interrupt_handler

===========================================================================*/
/*!
*  
* 
*  @detail
*  
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static void sns_dd_acc_kxtik_interrupt_handler
(
        sns_ddf_handle_t     dd_handle,
        uint32_t         gpio_num,
        sns_ddf_time_t       timestamp)

{
   sns_ddf_status_e status;
   uint8_t out;
   uint8_t kxtik_read_buffer = 0;
   sns_dd_acc_kxtik_state_t *state = (sns_dd_acc_kxtik_state_t *)dd_handle;
    if ( (status = sns_ddf_read_port(((sns_dd_acc_kxtik_state_t*)dd_handle)->port_handle,
                  KXTIK_INT_REL, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      	return;
    }
	 if ( (status = sns_ddf_read_port(((sns_dd_acc_kxtik_state_t*)dd_handle)->port_handle,
                  KXTIK_INT_SOURCE, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      	return;
    }
	SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "int status = %d", kxtik_read_buffer);
	SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " sns_dd_acc_kxtik_interrupt_handler++++++++");
 	
     
        if ( (status = sns_dd_acc_kxtik_get_data_all((sns_dd_acc_kxtik_state_t *)dd_handle) != SNS_DDF_SUCCESS))
        {
     	   return;
        }
	  sns_ddf_map_axes(&state->axes_map, state->data_cache);
      state->data_ptr[0].samples = state->data_sample;
	  state->data_ptr[0].sensor = SNS_DDF_SENSOR_ACCEL;
	  state->data_ptr[0].status = SNS_DDF_SUCCESS;
	  state->data_ptr[0].timestamp = timestamp;
	  state->data_ptr[0].num_samples = SDD_ACC_NUM_AXES;
	  
	  state->data_sample[0].sample  = state->data_cache[SDD_ACC_X];
	  state->data_sample[0].status = SNS_DDF_SUCCESS;
	  state->data_sample[1].sample  = state->data_cache[SDD_ACC_Y];
	  state->data_sample[1].status = SNS_DDF_SUCCESS;
	  state->data_sample[2].sample  = state->data_cache[SDD_ACC_Z];
	  state->data_sample[2].status = SNS_DDF_SUCCESS;
	  
	  sns_ddf_smgr_notify_data(state->smgr_hndl, state->data_ptr, 1);
	  sns_dd_log_data(state, state->data_ptr[0].timestamp);

	if (kxtik_read_buffer == 0x02)
	{	
		SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, "- - sns_dd_acc_kxtik_interrupt_handler motion detect ");
		sns_dd_acc_kxtik_reset_kionix_motion_int(dd_handle);
		sns_dd_acc_kxtik_config_motion_int((sns_dd_acc_kxtik_state_t*)dd_handle,0);
		status = sns_ddf_signal_deregister(gpio_num);
    	if(status != SNS_DDF_SUCCESS){
			return;
    	}
		sns_ddf_smgr_notify_event(((sns_dd_acc_kxtik_state_t*)dd_handle)->smgr_hndl,
                	SNS_DDF_SENSOR_ACCEL,
                	SNS_DDF_EVENT_MOTION);
	}
	return;
}



/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_config_motion_detect_thresh

===========================================================================*/
/*!
*  @brief Configure the motion interrupt threshold
* 
*  @detail
*  Programs the register kxtik_SLOPE_THRES_REG for the MD
*  threshold
* 
*  @param[in] state: Ptr to the driver structure
* 
*  @return
*  The error code definition within the DDF 
*  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_config_motion_detect
(
   sns_dd_acc_kxtik_state_t *state
)
{
   uint8_t kxtik_write_buffer, out;
   uint8_t kxtik_read_buffer = 0;
   sns_ddf_status_e stat;
   if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_CTRL_REG1, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
   {
      return stat;
   }  
   kxtik_write_buffer = 0;
   if ( (stat = sns_ddf_write_port(state->port_handle,
                         KXTIK_CTRL_REG1,
                         &kxtik_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
   {
    return stat;
   }
   kxtik_write_buffer = KXTIK_WAKEUP_ODR;
   if ((sns_ddf_write_port(state->port_handle,
              KXTIK_CTRL_REG2,
              &kxtik_write_buffer,
              1,
              &out)) != SNS_DDF_SUCCESS)
   {
    return stat;
   }
   kxtik_write_buffer = KXTIK_WAKEUP_TIMER;
   if ((sns_ddf_write_port(state->port_handle,
              KXTIK_WAKEUP_TIMER_REG,
              &kxtik_write_buffer,
              1,
              &out)) != SNS_DDF_SUCCESS)
   {
    return stat;
   }
   kxtik_write_buffer = KXTIK_INT_ENABLE;
   if ((sns_ddf_write_port(state->port_handle,
              KXTIK_INT_CTRL_REG,
              &kxtik_write_buffer,
              1,
              &out)) != SNS_DDF_SUCCESS)
   {
    return stat;
   }
   kxtik_write_buffer = KXTIK_MOTION_DETECT | kxtik_read_buffer;
   if ((sns_ddf_write_port(state->port_handle,
              KXTIK_CTRL_REG1,
              &kxtik_write_buffer,
              1,
              &out)) != SNS_DDF_SUCCESS)
   {
    return stat;
   }
    return stat;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_get_data

===========================================================================*/
/*!
*  @brief Called by the SMGR to get data
* 
*  @detail
*  Requests a single sample of sensor data from each of the specified
*    sensors. Data is returned immediately after being read from the
*    sensor, in which case data[] is populated in the same order it was
*    requested,
* 
*  @param[in]  dd_handle    Handle to a driver instance.
*  @param[in] sensors       List of sensors for which data isrequested.
*  @param[in] num_sensors   Length of @a sensors.
*  @param[in] memhandler    Memory handler used to dynamically allocate
*                           output parameters, if applicable.
*  @param[out] data         Sampled sensor data. The number of elements
*                           must match @a num_sensors.
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_kxtik_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
)
  {
    sns_ddf_status_e stat;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_acc_kxtik_state_t *state = (sns_dd_acc_kxtik_state_t *)dd_handle;
    uint8_t out, kxtik_read_buffer = 0;
	SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik get data begin ++++++++");
  	/* Sanity check*/

    if (sensors[0] != SNS_DDF_SENSOR_ACCEL)
    {
      return SNS_DDF_EINVALID_PARAM;
    }
    if ( (stat = sns_ddf_read_port(state->port_handle,
                  KXTIK_INT_SOURCE, 
                  &kxtik_read_buffer,
                  1,
                  &out)) != SNS_DDF_SUCCESS)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " read fail");
      return stat;
    }
	if ( (stat = sns_dd_acc_kxtik_get_data_all(
        		(sns_dd_acc_kxtik_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
    {
     	return stat;
    }
    if( (data_ptr = sns_ddf_memhandler_malloc(memhandler, 
      (num_sensors)*(sizeof(sns_ddf_sensor_data_s)))) == NULL)
    {
    	return SNS_DDF_ENOMEM;
    }
  
  
    *data = data_ptr;   
  
    //map the sensor data to the target devices's coordinates
    sns_ddf_map_axes(&state->axes_map, state->data_cache);


      data_ptr[0].sensor = sensors[0];
      data_ptr[0].status = SNS_DDF_SUCCESS;
      data_ptr[0].timestamp = sns_ddf_get_timestamp();
    
      if (sensors[0] == SNS_DDF_SENSOR_ACCEL) 
      {  
        if( (data_ptr[0].samples = sns_ddf_memhandler_malloc(memhandler, 
          SDD_ACC_NUM_AXES*sizeof(sns_ddf_sensor_sample_s))) == NULL)
        {
        	return SNS_DDF_ENOMEM;
        }  
    
    
        data_ptr[0].samples[0].sample  = state->data_cache[SDD_ACC_X];
        data_ptr[0].samples[1].sample  = state->data_cache[SDD_ACC_Y];
        data_ptr[0].samples[2].sample  = state->data_cache[SDD_ACC_Z];
    
        data_ptr[0].samples[0].status = SNS_DDF_SUCCESS;
        data_ptr[0].samples[1].status = SNS_DDF_SUCCESS;
        data_ptr[0].samples[2].status = SNS_DDF_SUCCESS;
        data_ptr[0].num_samples = SDD_ACC_NUM_AXES;
    
      }
      else {
	  	return SNS_DDF_EDEVICE;
      }
  
    sns_dd_log_data(state, data_ptr[0].timestamp);

    return SNS_DDF_SUCCESS;
  }

/*===========================================================================

  FUNCTION:   sns_dd_acc_kxtik_get_attr

===========================================================================*/
/*!
*  @brief Called by the SMGR to retrieves the value of an attribute of
*  the sensor.
* 
*  @detail
*  - range and resolution info is from the device data sheet.
* 
*  @param[in]  dd_handle   Handle to a driver instance.
*  @param[in] sensor       Sensor whose attribute is to be retrieved.
*  @param[in]  attrib      Attribute to be retrieved.
*  @param[in] memhandler  Memory handler used to dynamically allocate
*                         output parameters, if applicable.
*  @param[out] value      Pointer that this function will allocate or set
*                         to the attribute's value.
*  @param[out] num_elems  Number of elements in @a value.
*
*  @return
*    The error code definition within the DDF 
*    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
* 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_kxtik_get_attr
(
  sns_ddf_handle_t       dd_handle,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  sns_ddf_memhandler_s*  memhandler,
  void**                 value,
  uint32_t*              num_elems
)
{
  
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power_attrib;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                    sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib = *value;
      power_attrib->active_current = 135;
      power_attrib->lowpower_current = 10;
      *num_elems = 1;
    }
    break;

    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
            
      if (sensor == SNS_DDF_SENSOR_ACCEL) 
      {
        if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      SNS_DD_ACCEL_MAX_RANGE*sizeof(sns_ddf_range_s))) 
            == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_ranges = *value;
        *num_elems = 3;
        device_ranges[0].min = KXTIK_ACCEL_SENSOR_RANGE_2G_MIN;
        device_ranges[0].max = KXTIK_ACCEL_SENSOR_RANGE_2G_MAX;
        device_ranges[1].min = KXTIK_ACCEL_SENSOR_RANGE_4G_MIN;
        device_ranges[1].max = KXTIK_ACCEL_SENSOR_RANGE_4G_MAX;
        device_ranges[2].min = KXTIK_ACCEL_SENSOR_RANGE_8G_MIN;
        device_ranges[2].max = KXTIK_ACCEL_SENSOR_RANGE_8G_MAX;
      }
      else 
      { 
         return SNS_DDF_EDEVICE;
      }
    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
     
      device_res = *value;

      *num_elems = 1;
       device_res->bit_len = KXTIK_ACCEL_SENSOR_ACC_BIT_LEN;
       device_res->max_freq = 200;

    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      device_res = *value;

      *num_elems = 1;
      *device_res = FX_FLTTOFIX_Q16(2*G/4096);
    }
    break;

    case SNS_DDF_ATTRIB_LOWPASS:
    {
      uint8_t i;
      sns_ddf_lowpass_freq_t *freq_set;
      *num_elems = 7;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,(*num_elems)*sizeof(sns_ddf_lowpass_freq_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      freq_set = *value;  
      *num_elems = 7;
      for(i=0; i<*num_elems; i++)
      {
       *(freq_set+i) = sns_dd_accel_freq[i];
      }
    }
    break;
    case SNS_DDF_ATTRIB_ODR:
    {
      // for both
      sns_ddf_odr_t *res = sns_ddf_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_odr_t));
      if(NULL == res){
        return SNS_DDF_ENOMEM;
      }
	  
      //always return the current accel ODR to SMGR.
      if((SNS_DDF_SENSOR_ACCEL == sensor)||(SNS_DDF_SENSOR__ALL == sensor)){
	  	SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " cur rate = %d", ((sns_dd_acc_kxtik_state_t*)dd_handle)->acc_cur_rate);
        *res = ((sns_dd_acc_kxtik_state_t*)dd_handle)->acc_cur_rate;
      }
      else {
        return SNS_DDF_EINVALID_PARAM;
      }	

      *value = res;
      *num_elems = 1;
	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik get ODR done ---------");
      return SNS_DDF_SUCCESS;
    }
    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                    sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay = *value;
      *num_elems = 1;
      device_delay->time_to_active = 5; /*! 3ms is the startup time*/
      device_delay->time_to_data = 1;       
    }
    break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *device_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_info = *value;
      *num_elems = 1;
      device_info->model = "kxtik";
      device_info->vendor = "KIONIX";
      device_info->name = "Accelerometer";
      device_info->version = 1;
    }
    break;

    default:
     return SNS_DDF_EINVALID_PARAM;
  }
  
  return SNS_DDF_SUCCESS;
}

static void sns_dd_acc_kxtik_test_notify(
        sns_ddf_handle_t handle, 
        sns_ddf_sensor_e sensor, 
        sns_ddf_status_e status,
        uint32_t err,
        q16_t* biases)
{
  static const uint8_t num_axes = 3;
  sns_dd_acc_kxtik_state_t *state = handle;
  q16_t* biases_ptr;
  uint8_t i;

  sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                sensor,
                status,err);
  if (status == SNS_DDF_SUCCESS)
    {
      biases_ptr = state->bias;

      for (i = 0; i < num_axes; i++)
        biases_ptr[i] = biases[i];

      sns_ddf_smgr_notify_event(handle,
                    sensor,
                    SNS_DDF_EVENT_BIAS_READY);
      
    }

}




static sns_ddf_status_e sns_dd_acc_kxtik_self_test( 
        sns_ddf_handle_t dd_handle, 
        sns_ddf_sensor_e sensor, 
        sns_ddf_test_e test,
        uint32_t* err)
{
  sns_dd_acc_kxtik_state_t *state = (sns_dd_acc_kxtik_state_t *)dd_handle;
  sns_ddf_status_e stat;
  uint8_t out;
  uint8_t kxtik_write_buffer = 0;
  int32_t kxtik_data_buffer[6];
  int32_t kxtik_test_data[3];
  sns_ddf_status_e status;
  sns_dd_inertial_test_config_s test_config;
  
  *err = 0;

  if(test == SNS_DDF_TEST_OEM)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik test oem "); 
      test_config.sample_rate =  20;
      test_config.num_samples =  64;
      test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.3)),16,32));
      test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.2*G);
      test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.2*G);
      test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.26*G);

      sns_dd_acc_kxtik_reset(dd_handle);    

      status = sns_dd_inertial_test_run(
                  sensor,
                  dd_handle,
                  &sns_accel_kxtik_driver_fn_list,
                  &test_config,
                  &sns_dd_acc_kxtik_test_notify);

      if(status != SNS_DDF_SUCCESS)
        return status;
	  SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik test oem done");
      return SNS_DDF_SUCCESS;


    }
  else if(test == SNS_DDF_TEST_SELF)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DD_ACCEL, " kxtik config SW test"); 
      if ( (stat = sns_dd_acc_kxtik_get_data_all(
        (sns_dd_acc_kxtik_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
      {
     	 return stat;
      }
	  kxtik_data_buffer[SDD_ACC_X] = state->data_cache[SDD_ACC_X];
      kxtik_data_buffer[SDD_ACC_Y] = state->data_cache[SDD_ACC_Y];
      kxtik_data_buffer[SDD_ACC_Z] = state->data_cache[SDD_ACC_Z];
	  
	  kxtik_write_buffer = KXTIK_SELFTEST;
	  if ( (stat = sns_ddf_write_port(state->port_handle,
                     KXTIK_SELFTEST_REG,
                     &kxtik_write_buffer,
                     1,
                     &out)) != SNS_DDF_SUCCESS)
      {
         return stat;
      }

	  sns_ddf_delay(10000);

	  if ( (stat = sns_dd_acc_kxtik_get_data_all(
        (sns_dd_acc_kxtik_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
      {
      	 return stat;
      }
      kxtik_data_buffer[3] = state->data_cache[SDD_ACC_X];
      kxtik_data_buffer[4] = state->data_cache[SDD_ACC_Y];
      kxtik_data_buffer[5] = state->data_cache[SDD_ACC_Z];
	  
	  kxtik_test_data[SDD_ACC_X] = (kxtik_data_buffer[3] - kxtik_data_buffer[0]); 
	  kxtik_test_data[SDD_ACC_Y] = (kxtik_data_buffer[4] - kxtik_data_buffer[1]);
	  kxtik_test_data[SDD_ACC_Z] = (kxtik_data_buffer[5] - kxtik_data_buffer[2]);

	  kxtik_write_buffer = 0x00;
	  if((stat = sns_ddf_write_port(state->port_handle,
                     KXTIK_SELFTEST_REG,
                     &kxtik_write_buffer,
                     1,
                     &out)) != SNS_DDF_SUCCESS)
      {
        return stat;
      }
	  if(((abs(kxtik_test_data[SDD_ACC_X]) < (G + KXTIK_OFFSETX)) || (abs(kxtik_test_data[SDD_ACC_X]) > (G - KXTIK_OFFSETX1))) &&\
	  	((abs(kxtik_test_data[SDD_ACC_X]) < (G + KXTIK_OFFSETY)) || (abs(kxtik_test_data[SDD_ACC_X]) > (G - KXTIK_OFFSETY1))) &&\
	  	((abs(kxtik_test_data[SDD_ACC_X]) < (G + KXTIK_OFFSETZ)) || (abs(kxtik_test_data[SDD_ACC_X]) > (G - KXTIK_OFFSETZ1))))
	  {
	  	 SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " self test axis x = %d !!!!", kxtik_test_data[SDD_ACC_X]);
		 SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " self test axis y = %d !!!!", kxtik_test_data[SDD_ACC_Y]);
		 SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, " self test axis Z = %d !!!!", kxtik_test_data[SDD_ACC_Z]);
		 return SNS_DDF_SUCCESS;
	  }
	  else {
	  	return SNS_DDF_EINVALID_TEST;
	  }
    }
  else
  {
    return SNS_DDF_EINVALID_TEST;
  }
}

sns_ddf_status_e sns_dd_acc_kxtik_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
)
{
  sns_dd_acc_kxtik_state_t* state = (sns_dd_acc_kxtik_state_t*)handle;
  SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DD_ACCEL, "sns_dd_kxtik_enable_sched_data enable = %d !!!!", enable);
  if(enable)
  {
      return sns_dd_kxtik_enable_interrupt(state, sensor);
  }
  else
  {
      return sns_dd_kxtik_disable_interrupt(state, sensor);
  }
}

/*=============================================================================
 * Probe function for KIONIX sensor.
 =============================================================================*/

static sns_ddf_status_e sns_dd_acc_kionix_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors )
{
  /*
   * Initialize data before we begin probing for device existance.
   */
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  status = SNS_DDF_SUCCESS;
  *num_sensors = 0;
  *sensors = NULL;
  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  
  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              KXTIK_CHIP_ID_REG,
                              &i2c_buff,
                              1,
                              &bytes_read );
                             
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }


  if (i2c_buff != KXTIK)
  {
    sns_ddf_close_port( port_handle );
    return status;

  }

  /* This probe is successful */
 
  *num_sensors = 1;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
    status = SNS_DDF_SUCCESS;
  } 
  else 
  {
    status = SNS_DDF_ENOMEM;
  }

  sns_ddf_close_port( port_handle );
  return status;

} 

