/**
 * @file sns_dd_mpu6515_ext.c
 *
 *
 * Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2014-03-05 lka  Fixed critical Klocwork errors.
  2014-02-27 lka  Removed hardcoded attributes.
  2014-02-25 lka  Fixed attributes.
  2014-01-28 lka  Fixed Klocwork errors.
  2014-01-09 lka  Optimized DMP heartbeat function.
  2013-12-19 lka  Enabled gyro bias initialization for DMP.
  2013-12-02  sd  Added MULTIsensor FIFO support
  2013-10-11 lka  Added 15Hz as a supported ODR.
  2013-09-09  ad  Fix compiler warnings
  2013-09-06  pn  Allocates enough memory for FIFO data notification
  2013-08-28 lka  Added read DMP firmware function.
  2013-08-26  pn  Enabled supported_odr_list
  2013-08-19 lka  Added function for supported_odr_list (disabled by default).
  2013-08-14 lka  Fixed sensor names and model number.
  2013-08-08 lka  Modified AKM8963 and BMP280 init sequence.
  2013-07-26 lka  Added debug messages in init().
  2013-07-14 lka  Modified model information and added range info to Mag and
                  Pressure.
  2013-07-12 lka  Modified get_attrib() and init()
 2013-07012   asr  Generalized support for akm8963/bmp280 i2c slave devices in DMP Passthrough configuration and cleaned up code.
  2013-07-12  sd  Using regsitry for axe map
                  Using registry i2c address for slave sensor (mag & pressure)
  2013-07-03  sd  added mem allocation check
  2013-06-28 lka  Added support for using slave devices with MPU6515.
                  Minor code cleanup
  2013-06-20 lka  Initial version

==============================================================================*/

#include "sns_dd_mpu6515_priv.h"

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_util.h"
#ifdef FEATURE_MPU6050_INT_PIN_CONFIG
#include "sns_dd_gyro_priv.h"
#endif

#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

// Macro for printing out NV param error messages
#define MPU6050_MSG_NV_PARAM_ERROR(param,value,upper_lim,lower_lim)   MPU6050_MSG_3(ERROR, "Invalid " param " value of %u. The upper limit is %u and the lower limit is %d (inclusive)", value, upper_lim, lower_lim)

extern volatile boolean EnableI2C;

extern sns_ddf_driver_if_s sns_dd_mpu6515_if;
extern sns_ddf_driver_if_s sns_mag_akm8963_driver_fn_list;
extern sns_ddf_driver_if_s sns_alt_bmp280_driver_fn_list;
static mpu6515_head_s mpu6515_head = {
  NULL,
  {"Qualcomm MPU6XXX Driver", 1}
};

#define NUM_FIFO_SENSOR 5
static sns_ddf_sensor_e fifo_sensors[] =
{
   SNS_DDF_SENSOR_ACCEL,
   SNS_DDF_SENSOR_GYRO,
   SNS_DDF_SENSOR_MAG,
   SNS_DDF_SENSOR_PRESSURE,
   SNS_DDF_SENSOR_GAME_RV,   // Game RV
};
// Accelerometer range values
static sns_ddf_range_s mpu6515_accel_range[] =
{
   { FX_FLTTOFIX_Q16(-2 * G),  FX_FLTTOFIX_Q16(2 * G)  },
   { FX_FLTTOFIX_Q16(-4 * G),  FX_FLTTOFIX_Q16(4 * G)  },
   { FX_FLTTOFIX_Q16(-8 * G),  FX_FLTTOFIX_Q16(8 * G)  },
   { FX_FLTTOFIX_Q16(-16 * G), FX_FLTTOFIX_Q16(16 * G) }
};

// Gyroscope range values
static sns_ddf_range_s mpu6515_gyro_range[] =
{
   { MPU6050_DPS_TO_RPSQ16(-250),  MPU6050_DPS_TO_RPSQ16(250)  },
   { MPU6050_DPS_TO_RPSQ16(-500),  MPU6050_DPS_TO_RPSQ16(500)  },
   { MPU6050_DPS_TO_RPSQ16(-1000), MPU6050_DPS_TO_RPSQ16(1000) },
   { MPU6050_DPS_TO_RPSQ16(-2000), MPU6050_DPS_TO_RPSQ16(2000) }
};

// Temperature range values
//static sns_ddf_range_s mpu6515_temp_range =
//{
//    FX_FLTTOFIX_Q16(-40.0), FX_FLTTOFIX_Q16(85.0)
//};

// Gyroscope low pass filter bandwidth values for MPU6500
q16_t mpu6515_gyro_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(20),
   FX_FLTTOFIX_Q16(41),
   FX_FLTTOFIX_Q16(92),
   FX_FLTTOFIX_Q16(184),
   FX_FLTTOFIX_Q16(250),
   FX_FLTTOFIX_Q16(3600)
};

// Accelerometer low pass filter bandwidth values for MPU6500
static q16_t mpu6515_accel_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(20),
   FX_FLTTOFIX_Q16(41),
   FX_FLTTOFIX_Q16(92),
   FX_FLTTOFIX_Q16(184),
   FX_FLTTOFIX_Q16(460)
};

// Supported sensor types
static sns_ddf_sensor_e mpu6515_sensors[] =
{
   SNS_DDF_SENSOR_ACCEL,
   SNS_DDF_SENSOR_GYRO,
   SNS_DDF_SENSOR_TEMP,
   SNS_DDF_SENSOR_STEP_EVENT,   // step det
   SNS_DDF_SENSOR_SMD,          // SMD
   SNS_DDF_SENSOR_STEP_COUNT,   // step counter
   SNS_DDF_SENSOR_GAME_RV,   // Game RV
};

extern sns_ddf_status_e mpu6515_init_akm8963_slave( sns_ddf_handle_t hndl,
                                                    sns_ddf_nv_params_s*    nv_params,
                                                    sns_ddf_device_access_s device_info[],
                                                    sns_ddf_memhandler_s*   memhandler);

extern sns_ddf_status_e mpu6515_init_bmp280_slave(  sns_ddf_handle_t hndl,
                                                    sns_ddf_nv_params_s*    nv_params,
                                                    sns_ddf_device_access_s device_info[],
                                                    sns_ddf_memhandler_s*   memhandler);

/**
 * @brief Adjust the sensitivity according to the product revision
 * @
 */
sns_ddf_status_e mpu6515_sensitivity_adjust(mpu6515_s* state)
{
  uint8_t buffer = 0;
  uint8_t bytes_read, product_rev=0;
  sns_ddf_status_e status;

  status = sns_ddf_read_port(
        state->port_handle,
        MPU6050_REG_PRODUCT_ID,
        &buffer,
        1,
        &bytes_read);
  if(status != SNS_DDF_SUCCESS)
  {
     return status;
  }
  product_rev = buffer & PRODUCT_REV_MASK;
  if( (state->device_id == DEVICE_ID_MPU6500) || (state->device_id == DEVICE_ID_MPU6515) ||
      ((product_rev == REV_C_1) || (product_rev == REV_C_2)) )
  {
     /* revision C*/
     uint8_t range_indx =0, max_indx=0;
     max_indx = (uint8_t)(MPU6050_TABLE_SIZE(mpu6515_accel_range));
     for(range_indx=0; range_indx<max_indx;  range_indx++ )
     {
        mpu6515_accel_sensitivity[range_indx]= mpu6515_accel_sensitivity[range_indx]*2;
     }
  }

  sns_ddf_write_port_byte(
        state->port_handle,
        MPU6050_REG_INT_PIN_CFG,
        INT_LEVEL_HIGH | LATCH_INT_EN_PULSE | INT_RD_CLEAR_ON_ANY_REG,
        &status);

  return status;
}


/**
 * @brief Validates device ID
 *
 * @param[in] state  Ptr to the driver structure
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e mpu6515_validate_device_id(
   mpu6515_s* state)
{
    uint8_t buffer = 0;
    uint8_t bytes_read;
    sns_ddf_status_e status;
    mpu6515_device_id_e dev_id;

    if(EnableI2C == FALSE)
      return SNS_DDF_SUCCESS;

    status = sns_ddf_read_port( state->port_handle,
                MPU6XXX_REG_WHO_AM_I, &buffer, 1, &bytes_read);
    if(status != SNS_DDF_SUCCESS)
       return status;

    dev_id = (mpu6515_device_id_e) buffer;
    switch(dev_id)
    {
      case DEVICE_ID_MPU6050:
      case DEVICE_ID_MPU6500:
      case DEVICE_ID_MPU6515:
        state->device_id = dev_id;
        status = SNS_DDF_SUCCESS;
        break;
      default:
        state->device_id = DEVICE_ID_INVALID;
        status = SNS_DDF_EDEVICE;
        break;
    }

    return status;
}

/**
 * @brief Writes to an advanced hardware (DMP) register
 *
 * Burst write data to the DMP with sizes of up to 256 bytes at a time.
 *
 * @param[in]  state          Pointer to the MPU6500 struct
 * @param[in]  address        Sixteen-bit destination address
 * @param[in]  value          Pointer to an array of values to write to @address
 * @param[in]  size           The number of elements in @value
 *
 * @return Success if @value is correctly written to @address
 */
sns_ddf_status_e mpu6515_write_dmp_register(mpu6515_s * state,
		uint16_t address, uint8_t * value, uint16_t size)
{
	uint16_t n = 0;
	uint16_t curr_address;
	uint8_t address_bytes[2];
	uint8_t bytes_written, burst_size;
	uint16_t burst_limit;
	uint8_t * burst_buffer;
	sns_ddf_status_e status = SNS_DDF_EFAIL;

	while (n < size)
	{
		// Pointer to the beginning of the next set of data
		burst_buffer = value + n;
		/* Burst-write the address to DMP_CTRL_1 and DMP_CTRL_2 */
		curr_address = address + n;
		address_bytes[0] = MPU6XXX_HIGH_BYTE(curr_address);
		address_bytes[1] = MPU6XXX_LOW_BYTE(curr_address);
		status = sns_ddf_write_port(
			state->port_handle,
			MPU6500_REG_DMP_CTRL_1,
			address_bytes,
			2,
			&bytes_written);
		if (status != SNS_DDF_SUCCESS)
			return status;

		/* Burst-write a bank (up to 256 bytes) of data to DMP_CTRL_3 */

		// Determine the max allowed burst-size for this particular write-cycle
		// Note: The MPU6xxx is limited in its burst-write size by the
		//       boundaries of its memory banks (size of 256)
		burst_limit = MPU6500_MAX_BURST_WRITE_SIZE - (curr_address & 0xFF);
		// If burst_limit is 0x100, then limit it to 0xFF (the maximum write-size defined by sns_ddf_write_port)
    if (burst_limit > 0xFF)
      burst_limit = 0xFF; // Set to the maximum number of bytes allowed for a burst write

		if ((n + burst_limit) > size)
		{
			burst_size = (uint8_t) (size - n);
		}
		else
		{
			burst_size = (uint8_t) burst_limit;
		}

		/* Write the value(s) to the DMP_CTRL_3 */
		// Burst-write @burst_size bytes starting at the address @burst_buffer
		status = sns_ddf_write_port(
			state->port_handle,
			MPU6500_REG_DMP_CTRL_3,
			burst_buffer,
			burst_size,
			&bytes_written);
		if (status != SNS_DDF_SUCCESS || bytes_written != burst_size)
			return status;

		n += bytes_written;	// @bytes_written should equal @burst_size
	}

	return status;
}


/**
 * @brief Loads the DMP firmware with the given byte-array (firmware image)
 *
 *  "Starting with the byte[0] of data of the firmware image, perform the
 *  following for each byte[N] of data:
 *  - Write the value (N >> 8) to DMP_CTRL_1 (0x6D)
 *  - Write the value (N & 0xFF) to the DMP_CTRL_2 (0x6E)
 *  - Write the Nth byte of data to DMP_CTRL_3 (0x6F)
 *  Repeat the steps above for each byte of the DMP firmware image."
 *
 *  For more information regarding burst-writing see below.
 *  Source: Section 7.7 of the MPU-6500 Register Map and Definitions
 *
 * @param[in]  state          Pointer to the MPU6XXX device driver
 * @param[in]  image          Pointer to the DMP image byte-array
 * @param[in]  image_size     The number of elements in @image
 *
 * @return Success if @a dd_handle_ptr was allocated and the driver was
 *         configured properly. Otherwise a specific error code is returned.
 */
sns_ddf_status_e mpu6515_load_dmp_firmware(mpu6515_s * state,
		const unsigned char * image, uint16_t image_size)
{
	sns_ddf_status_e status;
  uint16_t address;
  uint8_t buffer[2];
  uint8_t bytes_written;

  address = MPU6515_DMP_ADDR_FIRST_BYTE;

  MPU6050_MSG_2(HIGH, "load_fw: size: %u, dmp_addr=%u", image_size, address);
  status = mpu6515_write_dmp_register(state, address, (uint8_t*) &(image[0]), image_size);
	if (SNS_DDF_SUCCESS == status)
	{
	  MPU6050_MSG_2(HIGH, "load_fw: %d bytes flashed, DMP_START_ADDR=%x ", image_size, DMP_START_ADDRESS);

    // Load firmware start value (DMP_START_ADDRESS) to start the DMP
    buffer[0] = MPU6XXX_HIGH_BYTE(DMP_START_ADDRESS);
    buffer[1] = MPU6XXX_LOW_BYTE(DMP_START_ADDRESS);
    status = sns_ddf_write_port(
      state->port_handle, MPU6XXX_REG_FW_START_ADDR, buffer, 2, &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
	}
  else
  {
    MPU6050_MSG_1(FATAL, "load_dmp_fw: failed to load DMP image (err=%d)", status);
  }

	return status;
}

#ifdef FEATURE_MPU6050_INT_PIN_CONFIG

#define PART_NUM_OFFSET 4
#define MPU3050_PART_NUM 3
#define MPU3050_I2C_ADDR 0x68
#define MPU3050_I2C_ADDR_MASK 0x6E
/**
 * @brief disable MPU3050 INT since it shares int pin with MPU6050
 * @
 */
sns_ddf_status_e sns_dd_mpu3050_int_disable()
{
  sns_ddf_status_e status;
  uint8_t data, out, read_count;
  sns_dd_gyro3050_state_s* ptr;
  sns_ddf_i2c_config_s    i2c_config;
  sns_ddf_device_access_s device;

  device.device_select = 0;
  device.port_config.bus = SNS_DDF_BUS_I2C;
  device.port_config.bus_config.i2c  = &i2c_config;
  i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
  i2c_config.bus_acq_timeout = -1;
  i2c_config.bus_freq = 400;
  i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_config.slave_addr = MPU3050_I2C_ADDR;
  i2c_config.xfer_timeout = -1;

  status = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_gyro3050_state_s));
  if (status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  status = sns_ddf_open_port(&(ptr->port_handle),
                              &(device.port_config));

  if (status != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(ptr);
    return status;
  }

  status = sns_ddf_read_port(ptr->port_handle,
                                   GYRO_3050_REG_PRODUCT_ID,
                                   &data, 1, &read_count);
  if( (data>>PART_NUM_OFFSET) != MPU3050_PART_NUM)
  {
     /* no MPU 3050, return */
     sns_ddf_close_port(ptr->port_handle);
     sns_ddf_mfree(ptr);
     return SNS_DDF_SUCCESS;
  }

  status = sns_ddf_read_port(ptr->port_handle,
                                   GYRO_3050_REG_I2C_ADDR,
                                   &data, 1, &read_count);
  if ( (data & MPU3050_I2C_ADDR_MASK) != MPU3050_I2C_ADDR )
  {
     /* no MPU 3050, return */
     sns_ddf_close_port(ptr->port_handle);
     sns_ddf_mfree(ptr);
     return SNS_DDF_SUCCESS;
  }

  /* configure interrupt */
  data =  GYRO_3050_BIT_ACTL_LOW | GYRO_3050_BIT_OPEN_DRAIN ;

  status = sns_ddf_write_port(ptr->port_handle,
                               GYRO_3050_REG_INT_CFG,
                               &data,
                               1,
                               &out);

  sns_ddf_close_port(ptr->port_handle);
  sns_ddf_mfree(ptr);
  if (status != SNS_DDF_SUCCESS)
  {
     return status;
  }

  return SNS_DDF_SUCCESS;
}
#endif

/**
 *
 */
mpu6515_s* mpu6515_find_same_port_dev (const sns_ddf_port_config_s *port_config_ptr)
{
  mpu6515_s* cur_dd_dev_ptr = (mpu6515_s*) mpu6515_head.next;

  while (cur_dd_dev_ptr)
  {
    if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
    {
      return cur_dd_dev_ptr;
    }
    else
    {
      cur_dd_dev_ptr = cur_dd_dev_ptr->next;
    }
  }
  return NULL;
}

/**
 *
 */
uint32_t mpu6515_fill_sub_dev_slot(
    mpu6515_s*       state,
    mpu6515_sub_dev_s*      sub_dev_slot,
    sns_ddf_handle_t        smgr_handle,
    uint32_t                gpio,
    uint32_t                num_sensors,
    sns_ddf_sensor_e        sensors[],
    sns_ddf_sensor_e*       sensors_init[])
{
  uint32_t num_sensors_filled = 0, i;

  // Check for null-pointers
  if ((NULL == state) || (NULL == sub_dev_slot) || (NULL == sensors_init))
    return 0;

  // Check bounds
  if (num_sensors > MPU6XXX_MAX_NUM_SENSORS)
    return 0;

  for (i = 0; i < num_sensors; i++)
  {
    if (((sensors[i] == SNS_DDF_SENSOR_ACCEL) || (sensors[i] == SNS_DDF_SENSOR_GYRO) || (sensors[i] == SNS_DDF_SENSOR_TEMP) || MPU_IS_DMP_SENSOR(sensors[i]))
             && (state->device_id == DEVICE_ID_MPU6500 || state->device_id == DEVICE_ID_MPU6515))
    {
      MPU6050_MSG_2(LOW, "sub_dev: added sensor %u hndl 0x%X", sensors[i], smgr_handle);
      sub_dev_slot->sensors[num_sensors_filled].watermark=0;
      sub_dev_slot->sensors[num_sensors_filled++].sensor_type = sensors[i];
      (*sensors_init)[i] = sensors[i];
    }
  }
  sub_dev_slot->smgr_handle = smgr_handle;
  sub_dev_slot->int_gpio = gpio;
  sub_dev_slot->num_sensors = num_sensors_filled;
  sub_dev_slot->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  return sub_dev_slot->num_sensors;
}

/**
 * @brief Parses the nv_params and fills in appropriate values for the axes_map
 */
sns_ddf_status_e mpu6515_parse_nv_params(
    sns_ddf_nv_params_s*  entire_nv_params,
    sns_ddf_axes_map_s*   axes_map,
    sns_ddf_handle_t      dd_handle,
    mpu6515_sensor_s*     sensor)
{
  int32_t* bias_ptr;
  sns_ddf_handle_t  sensor_gyro_hndl;

  // Check for null-pointers
  if (NULL == axes_map || NULL == sensor)
    return SNS_DDF_EINVALID_PARAM;

  if(sensor->sensor_type == SNS_DDF_SENSOR_GAME_RV)
  {
    /* 
     * For Game RV, it gets gyro calibration values from registry (if available)
     * and feeds to DMP.
     */
    if(entire_nv_params != NULL && entire_nv_params->data != NULL)
    {
      sensor_gyro_hndl = mpu6515_get_sensor_handle(dd_handle, SNS_DDF_SENSOR_GYRO);
      
      if(sensor_gyro_hndl != NULL)
      {
        bias_ptr = (int32_t*) entire_nv_params->data;
        mpu6515_set_attr(sensor_gyro_hndl, SNS_DDF_SENSOR_GYRO, SNS_DDF_ATTRIB_BIAS, bias_ptr);

        MPU6050_MSG_3(MED, "nv_params: gyro bias, grp=%u id=%u len=%u",
                      entire_nv_params->nvitem_grp, entire_nv_params->nvitem_id, entire_nv_params->data_len);
        MPU6050_MSG_3(MED, "nv_params: gyro bias, x=%d y=%d z=%d",
                      bias_ptr[0], bias_ptr[1], bias_ptr[2]);
      }
    }
  }
  else
  {
    // If there are no nv_params handed in, then set all to default values
    if (NULL == entire_nv_params)
    {
      MPU6050_MSG_0(MED, "nv_params: n/a , using defaults");
      sns_ddf_axes_map_init(axes_map, NULL);
    }
    else
    {
      MPU6050_MSG_2(MED, "nv_params: group %u, id %u", entire_nv_params->nvitem_grp, entire_nv_params->nvitem_id);
      sns_ddf_axes_map_init(axes_map, entire_nv_params->data);
      MPU6050_MSG_3(MED, "nv_params: indice  %u, %u, %u", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z );
      MPU6050_MSG_3(MED, "nv_params: signs  %u, %u, %u", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z );
    }
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Initializes the driver and sets up devices.
 * @see sns_ddf_driver_if_s.init()
 */
sns_ddf_status_e mpu6515_init(
    sns_ddf_handle_t*       dd_handle_ptr,
    sns_ddf_handle_t        smgr_handle,
    sns_ddf_nv_params_s*    nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t                num_devices,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_e*       sensors[],
    uint32_t*               num_sensors)
{
    sns_ddf_status_e status = SNS_DDF_EFAIL;
    mpu6515_s* state;
    sns_ddf_sensor_e* sensors_init;
    mpu6515_sensor_s * sensor_info;

    // Suppress compiler warning.
    (void)mpu6515_sensors;

    if(dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL || memhandler == NULL)
    {
      MPU6050_MSG_0(ERROR, "init: NULL ptr found");
      return SNS_DDF_EINVALID_PARAM;
    }

    if(*sensors == NULL)
    {
      return SNS_DDF_EFAIL;
    }

    // If the MPU6xxx device instance does not exist, then initialize it
    if ( ((*sensors)[0] == SNS_DDF_SENSOR_MAG ) || ((*sensors)[0] == SNS_DDF_SENSOR_PRESSURE ) )
    {
      MPU6050_MSG_2(LOW, ".init()2 num_sensor %u 1st %u", *num_sensors, (*sensors)[0]);
      state = (mpu6515_s*) mpu6515_head.next;
      if ( NULL == state)
      {
        MPU6050_MSG_2(LOW, ".init()3 num_sensor %u 1st %u", *num_sensors, (*sensors)[0]);
      }
    }
    else
    {
      MPU6050_MSG_2(LOW, ".init()4 num_sensor %u 1st %u", *num_sensors, (*sensors)[0]);
      state = mpu6515_find_same_port_dev(&device_info->port_config);
    }
    if ( NULL == state ) 
    {
      MPU6050_MSG_2(MED, "init: 1st sub-dev, bus_addr=0x%X bus_inst=0x%X", device_info->port_config.bus_config.i2c->slave_addr, device_info->port_config.bus_instance);
      // Manual delay for I2C setup
      sns_ddf_delay(100*1000);

     // Driver being initialized for the first time.
///      status = sns_ddf_malloc((void**)&state, sizeof(mpu6515_s) + (MPU6XXX_STATE_BYTE_ALIGNMENT - 4) /* 4 byte alignment is guaranteed */);
      status = sns_ddf_malloc((void**)&state, sizeof(mpu6515_s));

      if(status != SNS_DDF_SUCCESS)
        return status;
      memset(state, 0, sizeof(mpu6515_s));

///      state = (mpu6515_s *) (((uint32_t) state + (MPU6XXX_STATE_BYTE_ALIGNMENT - 4)) / MPU6XXX_STATE_BYTE_ALIGNMENT * MPU6XXX_STATE_BYTE_ALIGNMENT);

      // Put this device instance at the front of the list (all inserts into
      // the list are placed at the front)
      state->next = NULL;
      mpu6515_head.next = state;
      sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

      status = sns_ddf_malloc(
         (void**)&state->sensor_data.samples,
         sizeof(sns_ddf_sensor_sample_s)*DMP_MAX_NUM_SAMPLES*MPU6XXX_MAX_NUM_AXES);
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Open the communication port
      status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
      if(status != SNS_DDF_SUCCESS)
      {
        MPU6050_MSG_1(ERROR, "open_port err=%d", status);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      // Find out if it is an MPU6050 or MPU6500 connected
      if ((status = mpu6515_validate_device_id(state)) != SNS_DDF_SUCCESS )
      {
        MPU6050_MSG_1(ERROR, "validate dev id err=%d", status);
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      state->sub_device_cnt = 0;

      sensors_init = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e) * MPU6XXX_MAX_NUM_SENSORS);
      if(sensors_init == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      // Check to see if valid input is given for num_sensors and sensors.
      // If not, then it is using the older version of SMGR (without using probe & SSI)
      if (0 == *num_sensors && SNS_DDF_SENSOR__NONE == *sensors)
      {
        *num_sensors = 1;
        (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      }

      // Fill out the first sub-device slot
      uint32_t num_sensors_init = mpu6515_fill_sub_dev_slot(
          state,
          &state->sub_devices[0],
          smgr_handle,
          device_info->first_gpio,
          *num_sensors,
          *sensors,
          &sensors_init);
      if (num_sensors_init == 0)
      {
        MPU6050_MSG_0(ERROR, "init: unable to initialize any sensors.");
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return SNS_DDF_EFAIL;
      }

      // Assign the dd_handle_ptr
      (*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_device_cnt);
      state->sub_device_cnt++;

      sensor_info = mpu6515_get_sensor((*dd_handle_ptr), (*sensors)[0]);
      if(sensor_info == NULL)
      {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return SNS_DDF_EFAIL;
      }
      mpu6515_parse_nv_params(nv_params, &(sensor_info->axes_map), (*dd_handle_ptr), sensor_info);

      // Reset the device
      status = mpu6515_reset(*dd_handle_ptr);
      if(status != SNS_DDF_SUCCESS)
      {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      // Mag/Pressure usually wont be the first sensor to be initialised in the mpu6515.
       if(SNS_DDF_SENSOR_MAG == (*sensors)[0] )
        {
         status =  mpu6515_init_akm8963_slave(*dd_handle_ptr,nv_params,device_info,memhandler);
          if(status != SNS_DDF_SUCCESS)
          {
            return status;
          }
        }
        else if (SNS_DDF_SENSOR_PRESSURE == (*sensors)[0])
        {
          status = mpu6515_init_bmp280_slave(*dd_handle_ptr,nv_params,device_info,memhandler );
           if(status != SNS_DDF_SUCCESS)
          {
            return status;
          }
        }

      memset(state->mpu_reg, 0x00, MPU_REG_MAP_SIZE);

      // Set the output and return
      (*num_sensors) = num_sensors_init;
      (*sensors) = sensors_init;
      MPU6050_MSG_1(MED, "init: port_hndl=0x%08x", state->port_handle);
      MPU6050_MSG_3(MED, "init: hndl=0x%08x, %u sensors, 1st sensor %u", *dd_handle_ptr, *num_sensors, (*sensors)[0]);
      status = SNS_DDF_SUCCESS;
    }
    else
    {
      if (state->sub_device_cnt < MPU6515_MAX_SUB_DEV)
      {
        sns_ddf_sensor_e* sensors_init = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e) * MPU6XXX_MAX_NUM_SENSORS);

        if (NULL == sensors_init )
        {
          MPU6050_MSG_0(ERROR, "init: unable to allocate memory");
          return SNS_DDF_EFAIL;
        }

        MPU6050_MSG_1(MED, "init: current subdevice cnt =%d",state->sub_device_cnt);

        // Fill out the next sub-device slot
        uint32_t num_sensors_init = mpu6515_fill_sub_dev_slot(
            state,
            &state->sub_devices[state->sub_device_cnt],
            smgr_handle,
            device_info->first_gpio,
            *num_sensors,
            *sensors,
            &sensors_init);
        if (num_sensors_init == 0)
        {
          MPU6050_MSG_0(ERROR, "9999 init: unable to initialize any sensors.");
          return SNS_DDF_EFAIL;
        }

        (*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_device_cnt);
        state->sub_device_cnt++;
 
        if(SNS_DDF_SENSOR_MAG == (*sensors)[0] )
        {
          status = mpu6515_init_akm8963_slave(*dd_handle_ptr,nv_params,device_info,memhandler);
          if(status != SNS_DDF_SUCCESS)
          {
            *num_sensors = 0;
            state->sub_device_cnt--;
            *dd_handle_ptr = NULL;
            return status;
          }
        }
        else if (SNS_DDF_SENSOR_PRESSURE == (*sensors)[0])
        {
          status = mpu6515_init_bmp280_slave(*dd_handle_ptr,nv_params,device_info,memhandler );
          if(status != SNS_DDF_SUCCESS)
          {
            *num_sensors = 0;
            state->sub_device_cnt--;
            *dd_handle_ptr = NULL;
            return status;
          }
        }

        sensor_info = mpu6515_get_sensor((*dd_handle_ptr), (*sensors)[0]);
        if(sensor_info == NULL)
        {
          return SNS_DDF_EFAIL;
        }
        mpu6515_parse_nv_params(nv_params, &(sensor_info->axes_map), (*dd_handle_ptr), sensor_info);

        status = mpu6515_reset(*dd_handle_ptr);
        if (status != SNS_DDF_SUCCESS)
          return status;

        (*num_sensors) = num_sensors_init;
        (*sensors) = sensors_init;
        MPU6050_MSG_1(MED, "init: port_hndl=0x%08x", state->port_handle);
        MPU6050_MSG_3(MED, "init: hndl=0x%08x, %u sensors, 1st sensor %u", *dd_handle_ptr, *num_sensors, (*sensors)[0]);
        
        status = SNS_DDF_SUCCESS;
      }
      else
      {
        MPU6050_MSG_1(ERROR, "init: sub-dev slot full (%d)", MPU6515_MAX_SUB_DEV);
        return SNS_DDF_ENOMEM;
      }
    }

    /* Default dynamic range */
    switch(sensor_info->sensor_type)
    {
      case SNS_DDF_SENSOR_ACCEL:
        sensor_info->range = 0;
        break;
      case SNS_DDF_SENSOR_GYRO:
        sensor_info->range = 3;
        break;
      default:
        break;
    }

    return status;
}

/**
 * @brief Builds the list of supported ODRs up to MAX_FREQ for
 *        the given sensor.
 */
sns_ddf_status_e mpu6515_build_odr_list(
   mpu6515_s*            state,
   mpu6515_sensor_s*     sensor,
   sns_ddf_memhandler_s* memhandler,
   void**                value,
   uint32_t*             num_elems)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  const sns_ddf_odr_t  max_odr = 200;
  sns_ddf_odr_t* odr_list = sns_ddf_memhandler_malloc(
     memhandler, sizeof(sns_ddf_odr_t)*max_odr);
  uint16_t num_odrs = 0;

  if (odr_list == NULL)
  {
    return SNS_DDF_ENOMEM;
  }

  switch ( sensor->sensor_type )
  {
    case SNS_DDF_SENSOR_ACCEL:
    case SNS_DDF_SENSOR_GYRO:
    case SNS_DDF_SENSOR_TEMP:
    case SNS_DDF_SENSOR_STEP_EVENT:
    case SNS_DDF_SENSOR_SMD:
    case SNS_DDF_SENSOR_STEP_COUNT:
    case SNS_DDF_SENSOR_GAME_RV:
      odr_list[num_odrs++] = 1;
      odr_list[num_odrs++] = 2;
      odr_list[num_odrs++] = 4;
      odr_list[num_odrs++] = 5;
      odr_list[num_odrs++] = 10;
      odr_list[num_odrs++] = 15;
      odr_list[num_odrs++] = 20;
      odr_list[num_odrs++] = 40;
      odr_list[num_odrs++] = 50;
      odr_list[num_odrs++] = 100;
      odr_list[num_odrs++] = 200;
      break;
    case SNS_DDF_SENSOR_MAG:
      odr_list[num_odrs++] = 1;
      odr_list[num_odrs++] = 2;
      odr_list[num_odrs++] = 4;
      odr_list[num_odrs++] = 5;
      odr_list[num_odrs++] = 10;
      odr_list[num_odrs++] = 15;
      odr_list[num_odrs++] = 20;
      odr_list[num_odrs++] = 40;
      odr_list[num_odrs++] = 50;
      break;
    case SNS_DDF_SENSOR_PRESSURE:
      odr_list[num_odrs++] = 1;
      odr_list[num_odrs++] = 2;
      odr_list[num_odrs++] = 4;
      odr_list[num_odrs++] = 5;
      odr_list[num_odrs++] = 10;
      odr_list[num_odrs++] = 15;
      odr_list[num_odrs++] = 20;
      odr_list[num_odrs++] = 30;
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
      break;
  }
  *value = odr_list;
  *num_elems = num_odrs;
  return status;
}


/* Debug start */
/**
 * @brief read advanced hardware (DMP) register
 *
 * Burst read data from the DMP with sizes of up to 256 bytes at
 * a time. 
 *
 * @param[in]  state          Pointer to the MPU6500 struct
 * @param[in]  address        Sixteen-bit destination address
 * @param[in]  value          Pointer to an array of values to write to @address
 * @param[in]  size           The number of elements in @value
 *
 * @return Success if @value is correctly written to @address
 */
sns_ddf_status_e mpu6515_read_dmp_register(mpu6515_s * state,
		uint16_t address, uint8_t * value, uint16_t size)
{
	uint16_t n = 0;
	uint16_t curr_address = 0;
	uint8_t address_bytes[2];
	uint8_t bytes_written, burst_size;
	uint16_t burst_limit;
	uint8_t * burst_buffer;
	sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint16_t k = 0;
  
	while (n < size)
	{
    // Pointer to the beginning of the next set of data
		burst_buffer = value + n;
		/* Burst-write the address to DMP_CTRL_1 and DMP_CTRL_2 */
		curr_address = address + n;
    MPU6050_MSG_3(MED, "read_dmp_reg: burst_buffer=%u curr_addr=%u n=%u", burst_buffer, curr_address, n);

    address_bytes[0] = MPU6XXX_HIGH_BYTE(curr_address);
		address_bytes[1] = MPU6XXX_LOW_BYTE(curr_address);
		status = sns_ddf_write_port(
			state->port_handle,
			MPU6500_REG_DMP_CTRL_1,
			address_bytes,
			2,
			&bytes_written);
		if (status != SNS_DDF_SUCCESS) {
      MPU6050_MSG_3(ERROR, "read_dmp_reg: burst_buffer=%u curr_addr=%u n=%u", burst_buffer, curr_address, n);
			return status;
    }

		/* Burst-write a bank (up to 256 bytes) of data to DMP_CTRL_3 */

		// Determine the max allowed burst-size for this particular write-cycle
		// Note: The MPU6xxx is limited in its burst-write size by the
		//       boundaries of its memory banks (size of 256)
		burst_limit = MPU6500_MAX_BURST_WRITE_SIZE - (curr_address & 0xFF);
		// If burst_limit is 0x100, then limit it to 0xFF (the maximum write-size defined by sns_ddf_write_port)
    if (burst_limit > 0xFF)
      burst_limit = 0xFF; // Set to the maximum number of bytes allowed for a burst write

		if ((n + burst_limit) > size)
		{
			burst_size = (uint8_t) (size - n);
		}
		else
		{
			burst_size = (uint8_t) burst_limit;
		}

		/* Write the value(s) to the DMP_CTRL_3 */
		// Burst-write @burst_size bytes starting at the address @burst_buffer
		status = sns_ddf_read_port(
			state->port_handle,
			MPU6500_REG_DMP_CTRL_3,
			burst_buffer,
			burst_size,
			&bytes_written);
		if (status != SNS_DDF_SUCCESS || bytes_written != burst_size) {
      MPU6050_MSG_3(ERROR, "read_dmp_reg: burst_buffer=%u curr_addr=%u n=%u", burst_buffer, curr_address, n);
			return status;
    }

		n += bytes_written;	// @bytes_written should equal @burst_size
	}

  MPU6050_MSG_3(HIGH, "read_dmp_reg: k=%u n=%u curr_addr=%d", k, n, curr_address);

  for(k = 0; k < n; k++) {
    MPU6050_MSG_2(HIGH, "dmp_fw: [%u]=0x%02x", k, value[k]);
  }

	return status;
}

sns_ddf_status_e mpu6515_read_dmp_firmware(mpu6515_s * state)
{
	sns_ddf_status_e status;
  uint16_t address;
  uint8_t readout[SNS_DDF_MAX_ESPIMAGE_SIZE] = {0xFF};

  address = MPU6515_DMP_ADDR_FIRST_BYTE;

  MPU6050_MSG_2(HIGH, "read_dmp_fw: size: %u, dmp_addr=%u", SNS_DDF_MAX_ESPIMAGE_SIZE, address);
  status = mpu6515_read_dmp_register(state, address, readout, SNS_DDF_MAX_ESPIMAGE_SIZE);
	if (SNS_DDF_SUCCESS == status)
	{
	  MPU6050_MSG_1(HIGH, "read_dmp_fw: %d bytes read", SNS_DDF_MAX_ESPIMAGE_SIZE);
  }
  else
  {
    MPU6050_MSG_1(FATAL, "read_dmp_fw: failed to read DMP image (err=%d)", status);
  }

	return status;
}

/**
 * @brief Reads DMP firmware heartbeat
 *
 * Note: Register value and data size apply to Rev 0.6
 *
 * @param[i]  state   pointer to driver state 
 *
 * @return    true    if DMP is alive (heartbeat changes)
 *            false   otherwise
 */
bool mpu6515_check_dmp_heartbeat_reg(mpu6515_s * state)
{
  uint8_t buf[4] = { 0 }, i;
  uint32_t hb[2] = { 0 };
  sns_ddf_status_e status = SNS_DDF_EFAIL;

  for(i=0; i<ARR_SIZE(hb); i++)
  {
    status = mpu6515_read_dmp_reg(state, 384, buf, 4);
    if(status != SNS_DDF_SUCCESS)
      break;

    hb[i] = ((buf[0]*256+buf[1])*256+buf[2])*256+buf[3];

    if(i < ARR_SIZE(hb)-1)
    {
      // DMP should increment heartbeat after 5.5 msec
      sns_ddf_delay(5500);
    }
  }

  MPU6050_MSG_3(HIGH, "dmp: hb: hb0=%u  hb1=%u  status=%d", hb[0], hb[1], status);
  return (status == SNS_DDF_SUCCESS) ? (hb[1] > hb[0]) : false;
}

/**
 * @brief Retrieves the value of an attribute for a sensor.
 * @see sns_ddf_driver_if_s.get_attrib()
 */
sns_ddf_status_e mpu6515_get_attr(
   sns_ddf_handle_t      dd_handle,
   sns_ddf_sensor_e      sensor_type,
   sns_ddf_attribute_e   attrib,
   sns_ddf_memhandler_s* memhandler,
   void**                value,
   uint32_t*             num_elems)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  mpu6515_sensor_s* sensor = mpu6515_get_sensor(dd_handle, sensor_type);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if((dd_handle == NULL) || (value == NULL) || (state == NULL) || (sensor == NULL))
  {
    MPU6050_MSG_0(ERROR, "mpu6515_get_attr: NULL parameter handed in.");
    return SNS_DDF_EINVALID_PARAM;
  }

     switch(attrib)
     {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
          sns_ddf_power_info_s* power_attrib = NULL;
          if(sensor_type == SNS_DDF_SENSOR_MAG)
          {
            status = sns_mag_akm8963_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_POWER_INFO,
                        memhandler,
                        (void**)(&power_attrib),
                        num_elems );
          }
          else if(sensor_type == SNS_DDF_SENSOR_PRESSURE)
          {
            status = sns_alt_bmp280_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_POWER_INFO,
                        memhandler,
                        (void**)(&power_attrib),
                        num_elems );
          }
          else
          {
            power_attrib = sns_ddf_memhandler_malloc(
                             memhandler, sizeof(sns_ddf_power_info_s));
            if (power_attrib == NULL)
              return SNS_DDF_ENOMEM;

            status = SNS_DDF_SUCCESS;
            *num_elems = 1;
            switch(sensor_type)
            {
              case SNS_DDF_SENSOR_ACCEL:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_ACCEL;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              case SNS_DDF_SENSOR_GYRO:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_GYRO;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              case SNS_DDF_SENSOR_STEP_EVENT:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_STEP_EVENT;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              case SNS_DDF_SENSOR_SMD:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_SMD;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              case SNS_DDF_SENSOR_STEP_COUNT:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_STEP_COUNT;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              case SNS_DDF_SENSOR_GAME_RV:
                power_attrib->active_current = MPU6515_ACTIVE_CURRENT_DMP_GAME_RV;
                power_attrib->lowpower_current = MPU6515_LOWPOWER_CURRENT;
                break;
              default:
                status = SNS_DDF_EINVALID_PARAM;
                break;
            }
          }

          *value = power_attrib;
          return status;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
          sns_ddf_range_s *attr_ptr = NULL;
          status = SNS_DDF_SUCCESS;

          switch(sensor_type)
          {
            case SNS_DDF_SENSOR_MAG:
              status = sns_mag_akm8963_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_RANGE,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            case SNS_DDF_SENSOR_PRESSURE:
              status = sns_alt_bmp280_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_RANGE,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            case SNS_DDF_SENSOR_GYRO:
              attr_ptr = mpu6515_gyro_range;
              *num_elems = MPU6050_TABLE_SIZE(mpu6515_gyro_range);
              break;
            case SNS_DDF_SENSOR_ACCEL:
              attr_ptr = mpu6515_accel_range;
              *num_elems = MPU6050_TABLE_SIZE(mpu6515_accel_range);
              break;
            default:
              status = SNS_DDF_EINVALID_PARAM;
              *num_elems = 0;
              break;
          }

          *value = attr_ptr;
          return status;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
          sns_ddf_lowpass_freq_t *attr_ptr = NULL;
          status = SNS_DDF_SUCCESS;

          switch(sensor_type)
          {
            case SNS_DDF_SENSOR_GYRO:
              attr_ptr = mpu6515_gyro_lpf;
              *num_elems = MPU6050_TABLE_SIZE(mpu6515_gyro_lpf);
              break;
            case SNS_DDF_SENSOR_ACCEL:
              attr_ptr = mpu6515_accel_lpf;
              *num_elems = MPU6050_TABLE_SIZE(mpu6515_accel_lpf);
              break;
            case SNS_DDF_SENSOR_MAG:
              status = sns_mag_akm8963_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_LOWPASS,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            case SNS_DDF_SENSOR_PRESSURE:
              status = sns_alt_bmp280_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_LOWPASS,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            default:
              status = SNS_DDF_EINVALID_PARAM;
              *num_elems = 0;
              break;
          }

          *value = attr_ptr;
          return status;
        }

        case SNS_DDF_ATTRIB_ODR:
        {
          uint32_t *odr_ptr;
          odr_ptr = &sensor->desired_odr;

          if (0 != *odr_ptr)
          {
            odr_ptr = &sensor->odr;    /* returns current odr */
          }
          *value = odr_ptr;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
        {
          return mpu6515_build_odr_list(state, sensor, memhandler, value, num_elems);
        }

        case SNS_DDF_ATTRIB_BIAS:
        {
          sns_ddf_bias_t *attr_ptr = NULL;
          status = SNS_DDF_SUCCESS;

          switch(sensor_type)
          {
            case SNS_DDF_SENSOR_ACCEL:
              attr_ptr = sensor->biases;
              *num_elems = MPU6050_TABLE_SIZE(sensor->biases);
              break;
            case SNS_DDF_SENSOR_GYRO:
              attr_ptr = sensor->biases;
              *num_elems = MPU6050_TABLE_SIZE(sensor->biases);
              break;
            case SNS_DDF_SENSOR_MAG:
              status = sns_mag_akm8963_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_BIAS,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            case SNS_DDF_SENSOR_PRESSURE:
              status = sns_alt_bmp280_driver_fn_list.get_attrib(
                          &(sensor->aux_dev.st_aux_dev),
                          sensor_type,
                          SNS_DDF_ATTRIB_BIAS,
                          memhandler,
                          (void**)(&attr_ptr),
                          num_elems);
              break;
            default:
              status = SNS_DDF_EINVALID_PARAM;
              break;
          }

          *value = attr_ptr;
          return status;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
          *value = &mpu6515_head.dd_info;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
          sns_ddf_device_info_s* device_info = NULL;
          if(sensor_type == SNS_DDF_SENSOR_MAG)
          {
            status = sns_mag_akm8963_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_DEVICE_INFO,
                        memhandler,
                        (void**)(&device_info),
                        num_elems );
            if(device_info != NULL)
            {
              device_info->name = "Magnetometer";
            }
          }
          else if (sensor_type == SNS_DDF_SENSOR_PRESSURE)
          {
            status = sns_alt_bmp280_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_DEVICE_INFO,
                        memhandler,
                        (void**)(&device_info),
                        num_elems );
            if(device_info != NULL)
            {
              device_info->name = "Barometer";
            }
          }
          else
          {
            device_info = sns_ddf_memhandler_malloc(
                 memhandler, sizeof(sns_ddf_device_info_s));
            if(device_info == NULL)
              return SNS_DDF_ENOMEM;

            status = SNS_DDF_SUCCESS;
            *num_elems = 1;
            device_info->vendor = "InvenSense";
            device_info->version = 1;
            device_info->model = "MPU6515" ;
            device_info->name = (sensor_type==SNS_DDF_SENSOR_ACCEL) ? "Accelerometer" :
                                (sensor_type==SNS_DDF_SENSOR_GYRO)  ? "Gyroscope" :
                                (sensor_type==SNS_DDF_SENSOR_TEMP)  ? "Temperature" :
                                (sensor_type==SNS_DDF_SENSOR_STEP_EVENT) ? "Step Detector" :
                                (sensor_type==SNS_DDF_SENSOR_SMD) ? "Significant Motion Detector" :
                                (sensor_type==SNS_DDF_SENSOR_STEP_COUNT) ? "Step Counter" :
                                (sensor_type==SNS_DDF_SENSOR_GAME_RV) ? "Game Rotation Vector" :
                                "" ;
          }

          *value = device_info;
          return status;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
          sns_ddf_delays_s *delays = NULL;
          if(sensor_type == SNS_DDF_SENSOR_MAG)
          {
            status = sns_mag_akm8963_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_DELAYS,
                        memhandler,
                        (void**)(&delays),
                        num_elems );
          }
          else if (sensor_type == SNS_DDF_SENSOR_PRESSURE)
          {
            status = sns_alt_bmp280_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_DELAYS,
                        memhandler,
                        (void**)(&delays),
                        num_elems );
          }
          else
          {          
            delays = sns_ddf_memhandler_malloc(
                       memhandler, sizeof(sns_ddf_delays_s));
            if (delays == NULL)
              return SNS_DDF_ENOMEM;

            status = SNS_DDF_SUCCESS;
            *num_elems = 1;
            delays->time_to_active = 30000;
            delays->time_to_data = 500;
          }

          *value = delays;
          return status;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
          sns_ddf_resolution_adc_s *device_res = NULL;
          if(sensor_type == SNS_DDF_SENSOR_MAG)
          {
            status = sns_mag_akm8963_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_RESOLUTION_ADC,
                        memhandler,
                        (void**)(&device_res),
                        num_elems );
          }
          else if (sensor_type == SNS_DDF_SENSOR_PRESSURE)
          {
            status = sns_alt_bmp280_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_RESOLUTION_ADC,
                        memhandler,
                        (void**)(&device_res),
                        num_elems );
          }
          else
          {
            device_res = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_resolution_adc_s));
            if(device_res == NULL)
               return SNS_DDF_ENOMEM;

            status = SNS_DDF_SUCCESS;
            *num_elems = 1;
            device_res->bit_len  = 16;
            device_res->max_freq = 200;
          } 

          *value = device_res;
          return status;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
          sns_ddf_resolution_t* device_res = NULL;
          if(sensor_type == SNS_DDF_SENSOR_MAG)
          {
            status = sns_mag_akm8963_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_RESOLUTION,
                        memhandler,
                        (void**)(&device_res),
                        num_elems );
          }
          else if (sensor_type == SNS_DDF_SENSOR_PRESSURE)
          {
            status = sns_alt_bmp280_driver_fn_list.get_attrib(
                        &(sensor->aux_dev.st_aux_dev),
                        sensor_type,
                        SNS_DDF_ATTRIB_RESOLUTION,
                        memhandler,
                        (void**)(&device_res),
                        num_elems );
          }
          else
          {
            device_res = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_resolution_t));
            if(device_res == NULL)
               return SNS_DDF_ENOMEM;

            status = SNS_DDF_SUCCESS;
            *num_elems = 1;
            switch(sensor_type)
            {
              case SNS_DDF_SENSOR_GYRO:
                *device_res = mpu6515_gyro_sensitivity[sensor->range] >> 9;
                break;
              case SNS_DDF_SENSOR_ACCEL:
                *device_res = mpu6515_accel_sensitivity[sensor->range];
                break;
              case SNS_DDF_SENSOR_TEMP:
                *device_res = mpu6515_temp_sensitivity;
                break;
              default:
                *device_res = 1;
                break;
            }
          }

          *value = device_res;
          return status;
        }

        case SNS_DDF_ATTRIB_IO_REGISTER:
        {
          sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) (*value);
          uint8_t bytes_read;
          sns_ddf_status_e status;
          uint32_t reg_addr = (uint32_t) io_reg_in->address_ptr;

          MPU6050_MSG_1(LOW, "Getting attribute IO_REGISTER attrib: %u", SNS_DDF_ATTRIB_IO_REGISTER);

          // Address sizes are limited to uint16_t for the MPU6XXX
          if (reg_addr > 0xFFFF)
          {
            MPU6050_MSG_1(HIGH, "mpu6515_set_attr: invalid address 0x%X", (io_reg_in->address_ptr));
            return SNS_DDF_EINVALID_PARAM;
          }
          // sns_ddf_read_port is limited to reading 0xFF bytes
          if (io_reg_in->length > 0xFF)
          {
            MPU6050_MSG_1(HIGH, "mpu6515_set_attr: length too long, %d", io_reg_in->length);
            return SNS_DDF_EINVALID_PARAM;
          }

          MPU6050_MSG_2(LOW, "mpu6515_get_attr: (IO_REGISTER) Reading %d bytes from register 0x%X", io_reg_in->length, io_reg_in->address_ptr);

          // Allocate memory for the output data structure and for the data to be read from the register(s)
          sns_ddf_io_register_s* io_reg_out = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_io_register_s) + (sizeof(uint8_t) * (io_reg_in->length - 1)));
          if (io_reg_out == NULL)
            return SNS_DDF_ENOMEM;

          status = sns_ddf_read_port(
              state->port_handle,
              (uint16_t) reg_addr,
              &(io_reg_out->data[0]),
              (uint8_t) io_reg_in->length,
              &bytes_read);
          if (status != SNS_DDF_SUCCESS)
            return status;

          // Fill out the information in the output io_register data structure
          io_reg_out->address_ptr = io_reg_in->address_ptr;
          io_reg_out->addr_fixed = io_reg_in->addr_fixed;
          io_reg_out->data_width = io_reg_in->data_width;
          io_reg_out->length = (uint32_t) bytes_read;
          // Return the io_register data structure by reference
          *value = io_reg_out;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_FIFO:
        {
            uint8_t dmp_sensor;
            uint32_t i=0, j=0, size=0;
            sns_ddf_fifo_attrib_get_s *fifo_attr = NULL;
           
            size= sizeof(sns_ddf_fifo_attrib_get_s)+
                       (NUM_FIFO_SENSOR-2)*sizeof(sns_ddf_fifo_share_sensor_s *);
            
            fifo_attr=sns_ddf_memhandler_malloc(memhandler ,size);

            if( fifo_attr == NULL  )
            {
                MPU6050_MSG_0(MED, " No memory for fifo_attr ");
                return SNS_DDF_ENOMEM;
            }

            size = sizeof(sns_ddf_fifo_share_sensor_s);
            for ( i=0; i<( NUM_FIFO_SENSOR-1); i++)
            {
               fifo_attr->share_sensors[i] = sns_ddf_memhandler_malloc(memhandler,size);
               if( fifo_attr->share_sensors[i]==NULL )
               {
                   MPU6050_MSG_1(MED, " No memory for fifo_attr->share_sensors[%d]", i);
                   return SNS_DDF_ENOMEM;
               }
            }
   
            dmp_sensor = sns_dd_dmp_get_sensor(sensor->sensor_type);
            fifo_attr->is_supported = true;
            fifo_attr->is_sw_watermark = false;
            size = dmp_get_pkt_size(dmp_sensor);
            if ( size >0 )
            {
              fifo_attr->max_watermark =  sns_dd_dmp_compute_batch_threshold(128,size );
            }
            else
            {
              MPU6050_MSG_1(MED, " packet size is  nonpositive %d", size);
              fifo_attr->max_watermark = 50;
            }
            fifo_attr->share_sensor_cnt = 0;
            
            MPU6050_MSG_2(MED, "SNS_DDF_ATTRIB_FIFO max_watermark %u, sensor_type %u", fifo_attr->max_watermark, sensor->sensor_type );
            for ( i=0; i< (NUM_FIFO_SENSOR-1); i++)
            {
              if (sensor_type != fifo_sensors[i])
              {
                if (j<(NUM_FIFO_SENSOR-1) )
                {
                  fifo_attr->share_sensors[j]->sensor =fifo_sensors[i];
                  fifo_attr->share_sensors[j]->dd_handle = mpu6515_get_sensor_handle(state,fifo_sensors[i]) ;

                   if (fifo_attr->share_sensors[j]->dd_handle != NULL)
                   {
                     fifo_attr->share_sensor_cnt++;
                     MPU6050_MSG_2(MED, " SNS_DDF_ATTRIB_FIFO2 shared sensor %u handle=%u", fifo_attr->share_sensors[j]->sensor, fifo_attr->share_sensors[j]->dd_handle );
                   }
                   j++;
                }
              }
            }
 
            *value = fifo_attr;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
       }

       default:
         MPU6050_MSG_1(ERROR, "mpu6515_get_attr: Invalid attribute %u", attrib);
         return SNS_DDF_EINVALID_PARAM;
     } /* switch(attrib) */
}

