/********************************************************************************

    MMC3416 MAGNETIC SENSOR DRIVER
* Copyright (c) 2013, "Memsic Inc."

* All rights reserved.

*

* Redistribution and use in source and binary forms, with or without

* modification, are permitted provided that the following conditions are met:

*     1. Redistributions of source code must retain the above copyright

*      notice, this list of conditions and the following disclaimer.

*     2. Redistributions in binary form must reproduce the above copyright

*       notice, this list of conditions and the following disclaimer in the

*       documentation and/or other materials provided with the distribution.

  
*     3. Neither the name of "Memsic Inc." nor the

*       names of its contributors may be used to endorse or promote products

*       derived from this software without specific prior written permission.

*

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND

* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED

* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE

* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY

* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES

* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;

* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND

* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT


* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)

*******************************************************************************/

/*============================================================================

                                INCLUDE FILES

============================================================================*/

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"

#define DDK
#ifndef DDK
//Include files required to support magnetic log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif /* DDK */

#include "sns_dd_mag_mmc3xxx.h"

#ifdef DDK
#define BOOLEAN bool

#endif /* DDK */

#define MAG_UNIFY(_r, _o, _s)	(((float)(_r) - (float)(_o)) / ((float)(_s)))
#define MMC3XXX_RESET_PERIOD               15 // unit Second

/* Enable this macro to view magnetic_32bit debug logs */
//#define MMC3XXX_DEBUG

/*=======================================================================

                   INTERNAL ENUMS

========================================================================*/
/* Supported datatypes */
typedef enum
{
   SDD_MAG_X = 0,
   SDD_MAG_Y,
   SDD_MAG_Z,
   SDD_MAG_NUM_AXES,
   SDD_TEMP = SDD_MAG_NUM_AXES,
   SDD_MAG_NUM_DATATYPES
} sdd_mag_data_type_e;

/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_mmc3xxx_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_mmc3xxx_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_mmc3xxx_set_attr( 
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_mmc3xxx_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static sns_ddf_status_e sns_dd_mmc3xxx_test(sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err);

static sns_ddf_status_e sns_dd_mmc3xxx_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );

static sns_ddf_status_e sns_dd_mmc3xxx_reset(sns_ddf_handle_t dd_handle);

static void sns_dd_mmc3xxx_handle_timer(sns_ddf_handle_t dd_handle, void* arg);

static uint32_t self_test_counter = 0;
static int32_t mmc3280_raw[3] = {0, 0, 0};
static uint8_t  mmc3xxx_saturation_flag = 0; 
static uint32_t last_reset_time = 0;

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_mmc3xxx_driver_fn_list =
{
  .init = &sns_dd_mmc3xxx_init,
  .get_data = &sns_dd_mmc3xxx_get_data,
  .set_attrib = &sns_dd_mmc3xxx_set_attr,
  .get_attrib = &sns_dd_mmc3xxx_get_attr,
  .handle_timer = &sns_dd_mmc3xxx_handle_timer, /* handle timer */
  .handle_irq = NULL, /* handle irq */
  .reset = &sns_dd_mmc3xxx_reset,
  .run_test = &sns_dd_mmc3xxx_test,
  .enable_sched_data = NULL,
  .probe = &sns_dd_mmc3xxx_probe
};

/* State struct for magnetic driver */
typedef struct {
   sns_ddf_handle_t smgr_hndl;    /* SDDI handle used to notify_data */
   q16_t            bias[SDD_MAG_NUM_AXES];    /* magnetic sensor bias */
   q16_t            data_cache[SDD_MAG_NUM_DATATYPES]; /* data cache */
   uint8_t          out_of_bound_flag;
   sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
   uint8_t          range;       /* current range selection */
   uint16_t         lowpass_bw;   /* current lowpass filter BW */
   sns_ddf_timer_s  mmc3xxx_timer;
   sns_ddf_axes_map_s  axes_map;
} sns_dd_mmc3xxx_state_t;


/*============================================================================

  Global Data Definitions

 ============================================================================*/
static sns_ddf_sensor_e my_sensors[] = 
  {
    SNS_DDF_SENSOR_MAG 
  };

static MMC3XXX_PRODUCT_ID mag_sensor_list[MMC3XXX_MAX_NUM];
static MMC3XXX_PRODUCT_ID *mag_sensor = NULL;

static MMC3XXX_TYPE sns_dd_mmc3xxx_detect(uint8_t *id)
{
  if ((id[0] == 0x3) && (id[1] == 0x5))
    return MMC33160MT;
  else if ((id[0] == 0x3) && (id[1] == 0x4))
    return MMC3280MA;
  else if ((id[0] == 0x1) && (id[1] == 0x6))
    return MMC34160PJ;
  else
    return MMC3XXX;
}

static uint32_t monitorSaturation(float *mag)
{
	float squaredMagMag;
        float overRangeSquared;

	squaredMagMag=mag[0]*mag[0]+mag[1]*mag[1]+mag[2]*mag[2];
        overRangeSquared = (float)((mag_sensor->range - 2) * (mag_sensor->range - 2));

        DDF_MSG_2(FATAL, "in M34160PJ-monitor saturated %d %d", squaredMagMag, overRangeSquared);
	if (squaredMagMag>overRangeSquared) 
		return(1);
	else 
		return(0);
}

static uint8_t isSaturated(void)
{
    return mmc3xxx_saturation_flag;
}
/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_config_default

===========================================================================*/
/*!
  @brief Sets the magnetic sensor to its default operational state
 
  @detail
  - Programs the register MAGNETIC_SENSOR_CTRL1_REG_ADDR
 
  @param[in] state: Ptr to the driver structure
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mmc3xxx_config_default
(
  sns_dd_mmc3xxx_state_t *state
)
{
  uint8_t  magnetic_write_buffer, out;
  sns_ddf_status_e stat;
  int i;
  uint8_t id[2];
  MMC3XXX_TYPE sensor_type;
  uint8_t magnetic_read_data[6];
#ifdef MMC3XXX_DEBUG
  int32_t magnetic_32bit;
#endif // MMC3XXX_DEBUG
  
  strlcpy(mag_sensor_list[0].name, "MMC3280MA", sizeof(mag_sensor_list[0].name));
  mag_sensor_list[0].type = MMC3280MA;
  mag_sensor_list[0].bias = 4096;
  mag_sensor_list[0].bit_len = 13;
  mag_sensor_list[0].freq = 150;
  mag_sensor_list[0].scale = 512;
  mag_sensor_list[0].range = 8;
  strlcpy(mag_sensor_list[1].name, "MMC33160MT", sizeof(mag_sensor_list[1].name));
  mag_sensor_list[1].type = MMC33160MT;
  mag_sensor_list[1].bias = 8192;
  mag_sensor_list[1].bit_len = 14;
  mag_sensor_list[1].freq = 150;
  mag_sensor_list[1].scale = 512;
  mag_sensor_list[1].range = 16;
  strlcpy(mag_sensor_list[2].name, "MMC34160PJ", sizeof(mag_sensor_list[2].name));
  mag_sensor_list[2].type = MMC34160PJ;
  mag_sensor_list[2].bias = 32768;
  mag_sensor_list[2].bit_len = 16;
  mag_sensor_list[2].freq = 250;
  mag_sensor_list[2].scale = 2048;
  mag_sensor_list[2].range = 16;
  DDF_MSG_0(FATAL, "in M34160PJ-config default");
  mag_sensor = &mag_sensor_list[2];  //changed-memsic-9-10
  
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_ID_REG1_ADDR, 
                              &id[0],
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return stat;
	  DDF_MSG_0(FATAL, "in M34160PJ-error default");
  }
  
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_ID_REG2_ADDR, 
                              &id[1],
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return stat;
	 DDF_MSG_0(FATAL, "in M34160PJ-error default");
  }
  
  sensor_type = sns_dd_mmc3xxx_detect(id);
  if (sensor_type == MMC3XXX)
  {
  
    return SNS_DDF_EDEVICE;
	DDF_MSG_0(FATAL, "in M34160PJ-error default");
    
  }
  
  for (i = 0; i < MMC3XXX_MAX_NUM; i++) {
    if (sensor_type == mag_sensor_list[i].type) {
      mag_sensor = &mag_sensor_list[i];
      break;
    }
  }
  
 if (mag_sensor == NULL)
   return SNS_DDF_EDEVICE;

  if (mag_sensor->type == MMC34160PJ) {
     magnetic_write_buffer = MMC3XXX_SENSPR_FILLCAP;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    DDF_MSG_0(FATAL, "in M34160PJ-error default");
	return stat;
  }
  DDF_MSG_0(FATAL, "in M34160PJ-config default -set");
  /* waiting 75 ms after set*/
  sns_ddf_delay(75000);
  }

  magnetic_write_buffer = MMC3XXX_SENSOR_SET;
     if( (stat = sns_ddf_write_port(state->port_handle,
                                    MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                                    &magnetic_write_buffer,
                                    1,
                                    &out)) != SNS_DDF_SUCCESS)
     {
        return stat;
     }
  DDF_MSG_0(FATAL, "in M34160PJ-config default -set");
  sns_ddf_delay(50000);    
  
    
  
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }

  /* waiting 10 ms after take measurement command */
  sns_ddf_delay(10000);
  
  /* Read the data, no temperature data read */
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return stat;
  }
  DDF_MSG_0(FATAL, "in M34160PJ-config default -data read");

#ifdef MMC3XXX_DEBUG
  magnetic_32bit = ((int32_t)magnetic_read_data[1]) << 8 | (int32_t)magnetic_read_data[0];
  magnetic_32bit = ((int32_t)magnetic_read_data[3]) << 8 | (int32_t)magnetic_read_data[2];
  magnetic_32bit = ((int32_t)magnetic_read_data[5]) << 8 | (int32_t)magnetic_read_data[4];
  
  DDF_MSG_1(FATAL, "in M34160PJ- config default -data read magnetic_32bit %d", magnetic_32bit);
#endif // MMC3XXX_DEBUG

    if (mag_sensor->type == MMC34160PJ) {
     magnetic_write_buffer = MMC3XXX_SENSPR_FILLCAP;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  /* waiting 50 ms after reset*/
  sns_ddf_delay(75000);
  }

  magnetic_write_buffer = MMC3XXX_SENSOR_RESET;
     if( (stat = sns_ddf_write_port(state->port_handle,
                                    MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                                    &magnetic_write_buffer,
                                    1,
                                    &out)) != SNS_DDF_SUCCESS)
     {
        return stat;
     }
  
 

  sns_ddf_delay(50000);
  
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }

  /* waiting 10 ms after take measurement command */
  sns_ddf_delay(10000);

  /* Read the data, no temperature data read */
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return stat;
  }

#ifdef MMC3XXX_DEBUG
  magnetic_32bit = ((int32_t)magnetic_read_data[1]) << 8 | (int32_t)magnetic_read_data[0];
  magnetic_32bit = ((int32_t)magnetic_read_data[3]) << 8 | (int32_t)magnetic_read_data[2];
  magnetic_32bit = ((int32_t)magnetic_read_data[5]) << 8 | (int32_t)magnetic_read_data[4];
  
  DDF_MSG_1(FATAL, "in M34160PJ-config default -before ifdefFL -data read magnetic_32bit %d", magnetic_32bit);
#endif // MMC3XXX_DEBUG
 
#ifdef FL
{
  uint8_t magnetic_read_data[6];
  int32_t magnetic_32bit;
  
for (i = 0; i < 5; i++) {
  if (mag_sensor->type == MMC34160PJ) {
     magnetic_write_buffer = MMC3XXX_SENSPR_FILLCAP;
     if( (stat = sns_ddf_write_port(state->port_handle,
                                    MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                                    &magnetic_write_buffer,
                                    1,
                                    &out)) != SNS_DDF_SUCCESS)
     {
        return stat;
     }
  }
  
  magnetic_write_buffer = MMC3XXX_SENSOR_RESET;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }
  /* take measurement */
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }

  /* waiting 10 ms after take measurement command */
  sns_ddf_delay(10000);

  /* Read the data, no temperature data read */
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return stat;
  }

#ifdef MMC3XXX_DEBUG
  magnetic_32bit = ((int32_t)magnetic_read_data[1]) << 8 | (int32_t)magnetic_read_data[0];
  magnetic_32bit = ((int32_t)magnetic_read_data[3]) << 8 | (int32_t)magnetic_read_data[2];
  magnetic_32bit = ((int32_t)magnetic_read_data[5]) << 8 | (int32_t)magnetic_read_data[4];
#endif //MMC3XXX_DEBUG
}
}
#endif
DDF_MSG_0(FATAL, "in M34160PJ-config default -out");
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_set_attr

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were 
         in after init() was called.
 
  @detail
  - resets mmc3xxx and recover from saturation
 
  @param[in] state: Ptr to the driver structure
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: Check if open and close port operation is required here?
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mmc3xxx_set_attr 
( 
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  uint8_t  magnetic_write_buffer = 0, out;
  sns_ddf_status_e stat;
  sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
  sns_dd_mmc3xxx_state_t *state = (sns_dd_mmc3xxx_state_t *)dd_handle;

  if ((mag_sensor->type == MMC3280MA) ||
      (mag_sensor->type == MMC33160MT))
    return SNS_DDF_EINVALID_PARAM;
  
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
       break;

    case SNS_DDF_ATTRIB_RANGE:
       break;
    
    case SNS_DDF_ATTRIB_LOWPASS:
       break;
    
    case SNS_DDF_ATTRIB_THRESHOLD:
	  DDF_MSG_0(FATAL, "in M34160PJ-attrib_threshold");
      if (*(int *)value == 1)
        magnetic_write_buffer |= MMC3XXX_SENSOR_FREQ_25HZ | MMC3XXX_SENSOR_CONTMODE;
      else if (*(int *)value == 2)
        magnetic_write_buffer |= MMC3XXX_SENSOR_FREQ_12HZ | MMC3XXX_SENSOR_CONTMODE;
      else if (*(int *)value == 3)
        magnetic_write_buffer |= MMC3XXX_SENSOR_FREQ_1_5HZ | MMC3XXX_SENSOR_CONTMODE;
      else 
        magnetic_write_buffer |= MMC3XXX_SENSOR_FREQ_50HZ;
      if( (stat = sns_ddf_write_port(state->port_handle,
                                     MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                                     &magnetic_write_buffer,
                                     1,
                                     &out)) != SNS_DDF_SUCCESS)
     {
        return SNS_DDF_EBUS;
     }
     break;
      
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
	  DDF_MSG_0(FATAL, "in M34160PJ-attrib_resadc");
      if (*(int *)value == 1)
        magnetic_write_buffer |= MMC3XXX_SENSOR_BW_16_FAST;
      else if (*(int *)value == 2)
        magnetic_write_buffer |= MMC3XXX_SENSOR_BW_14;
      else 
        magnetic_write_buffer |= MMC3XXX_SENSOR_BW_16_SLOW;
      if( (stat = sns_ddf_write_port(state->port_handle,
                                     MMC3XXX_SENSOR_CTRL1_REG_ADDR, 
                                     &magnetic_write_buffer,
                                     1,
                                     &out)) != SNS_DDF_SUCCESS)
      {
         return SNS_DDF_EBUS;
      }
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  return(ret_val);
}

static sns_ddf_status_e sns_dd_mmc3xxx_test(sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err)
{
  uint8_t  magnetic_write_buffer, out;
  sns_ddf_status_e stat;
  sns_dd_mmc3xxx_state_t *state = (sns_dd_mmc3xxx_state_t *)dd_handle;
  uint8_t magnetic_read_data[6];
  int32_t magnetic_32bit;
  int32_t prev_raw[3], curr_raw[3];
  uint8_t result = 0;
  DDF_MSG_0(FATAL, "in M34160PJ-attrib_test");
  magnetic_write_buffer = MMC3XXX_SENSOR_SET;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
    return stat;
  }
  DDF_MSG_0(FATAL, "in M34160PJ-test bef delay");
  /* waiting 50 ms after set*/
  sns_ddf_delay(50000);
  
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
    return stat;
  }
   DDF_MSG_0(FATAL, "in M34160PJ-attrib_test2");
  /* waiting 10 ms after take measurement command */
  sns_ddf_delay(10000);
  
  /* Read the data, no temperature data read */
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
	DDF_MSG_0(FATAL, "in M34160PJ-test error read");
     return stat;
  }

  magnetic_32bit = ((int32_t)magnetic_read_data[1]) << 8 | (int32_t)magnetic_read_data[0];
  prev_raw[0] = magnetic_32bit;

  magnetic_32bit = ((int32_t)magnetic_read_data[3]) << 8 | (int32_t)magnetic_read_data[2];
  prev_raw[1] = magnetic_32bit;

  magnetic_32bit = ((int32_t)magnetic_read_data[5]) << 8 | (int32_t)magnetic_read_data[4];
  prev_raw[2] = magnetic_32bit;

  magnetic_write_buffer = MMC3XXX_SENSOR_RESET;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
	DDF_MSG_0(FATAL, "in M34160PJ-test err write");
    return stat;
  }
 
  /* waiting 50 ms after reset*/
  sns_ddf_delay(50000);
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
	DDF_MSG_0(FATAL, "in M34160PJ-test err write");
    return stat;
  }

  /* waiting 10 ms after take measurement command */
  sns_ddf_delay(10000);
  
  /* Read the data, no temperature data read */
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
    *err = SNS_DDF_EDEVICE;
	DDF_MSG_0(FATAL, "in M34160PJ-test err read");
     return stat;
  }

  magnetic_32bit = ((int32_t)magnetic_read_data[1]) << 8 | (int32_t)magnetic_read_data[0];
  curr_raw[0] = magnetic_32bit;
  DDF_MSG_0(FATAL, "in M34160PJ-test raw data x");
  magnetic_32bit = ((int32_t)magnetic_read_data[3]) << 8 | (int32_t)magnetic_read_data[2];
  curr_raw[1] = magnetic_32bit;
  DDF_MSG_0(FATAL, "in M34160PJ-test raw data y");
  magnetic_32bit = ((int32_t)magnetic_read_data[5]) << 8 | (int32_t)magnetic_read_data[4];
  curr_raw[2] = magnetic_32bit;
  DDF_MSG_0(FATAL, "in M34160PJ-test raw data z");
  switch (mag_sensor->type) {
  case MMC33160MT:
        if ((abs(curr_raw[0]-prev_raw[0]) > 25) || (abs(curr_raw[1]-prev_raw[1]) > 25) ||
             (abs(curr_raw[2]-prev_raw[2]) > 25))
            result = 1;
			DDF_MSG_0(FATAL, "in M34160PJ-test raw data result mmc33160");
        break;
  case MMC34160PJ:
        if ((abs(curr_raw[0]-prev_raw[0]) > 100) || (abs(curr_raw[1]-prev_raw[1]) > 100) ||
             (abs(curr_raw[2]-prev_raw[2]) > 100))
            result = 1;
			DDF_MSG_0(FATAL, "in M34160PJ-test result");
        break;
  default:
        break;
  }
  
  // Have to tilt the board up/down at any angles. Then read the return value.
  if (mag_sensor->type == MMC3280MA) {
    if (self_test_counter % 2) {
      if ((abs(curr_raw[0]-mmc3280_raw[0]) > 40) || (abs(curr_raw[1]-mmc3280_raw[1]) > 40) ||
             (abs(curr_raw[2]-mmc3280_raw[2]) > 40))
            result = 1;
			DDF_MSG_0(FATAL, "in M34160PJ-test raw data data result mmc3280");
    }
    else {
      mmc3280_raw[0] = curr_raw[0];
      mmc3280_raw[1] = curr_raw[1];
      mmc3280_raw[2] = curr_raw[2];
      result = 1;
	  DDF_MSG_0(FATAL, "in M34160PJ-test raw data data result other");
	  
    }
  }
  DDF_MSG_0(FATAL, "in M34160PJ-test counter");
  self_test_counter++;
           
  if (result){
  DDF_MSG_0(FATAL, "in M34160PJ-test counter success");
  
      return SNS_DDF_SUCCESS;
	  }
  else
  {
      return SNS_DDF_EDEVICE;
	  }
}

static sns_ddf_status_e sns_dd_mmc3xxx_reset(sns_ddf_handle_t dd_handle)
{
  uint8_t i;
  sns_ddf_status_e stat;
  sns_dd_mmc3xxx_state_t *state = (sns_dd_mmc3xxx_state_t *)dd_handle;
 
  
  DDF_MSG_0(FATAL, "in M34160PJ-reset");
  for (i = 0; i < SDD_MAG_NUM_AXES; i++) 
  {
    state->bias[i] = 0;
  }
    state->out_of_bound_flag = 0;

  stat =sns_dd_mmc3xxx_config_default(state);
  return stat;
}

/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_init

===========================================================================*/
/*!
  @brief Initializes the 3 axis magnetic sensor
              determines the device to use
 
  @detail
  - Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_mag_reset routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler     Memory handler used to dynamically allocate 
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: 
*/
/*=========================================================================*/                                             
static sns_ddf_status_e sns_dd_mmc3xxx_init(
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
  sns_dd_mmc3xxx_state_t *ptr;
  
  if ( (dd_handle_ptr == NULL) ||
         (num_sensors == NULL) ||
         (sensors == NULL) )
    {
        DDF_MSG_0(HIGH, "invalid param");
		
        return SNS_DDF_EINVALID_PARAM;
    }
    
  if(sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_mmc3xxx_state_t)) != SNS_DDF_SUCCESS) 
  { 
    DDF_MSG_0(HIGH, "1 -bad");
		
    return SNS_DDF_ENOMEM;
  }
    
	     DDF_MSG_0(HIGH, "2");
	
	
  if((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
        DDF_MSG_0(HIGH, "2 -bad");
	        
	return stat;
  }
         DDF_MSG_1(FATAL, "sns ddf open port return is %d", stat);
		  
    ptr->smgr_hndl = smgr_handle;
     DDF_MSG_0(FATAL, "in M34160PJ-init");
     
  
  /* Fill out supported sensor info */
 
  *dd_handle_ptr = (sns_ddf_handle_t)ptr;
  
         DDF_MSG_0(HIGH, "4");
	
  if (( stat = sns_dd_mmc3xxx_reset(ptr)) != SNS_DDF_SUCCESS)
   {
      DDF_MSG_1(FATAL, "reset failed with code %d", stat);
	  return SNS_DDF_EDEVICE;
   }
  *num_sensors = 1;
  *sensors = my_sensors;  

     sns_ddf_axes_map_init(
      &ptr->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));
  
  DDF_MSG_0(FATAL, "in M34160PJ-init timer");

  /* init timer */
  if ( (stat = sns_ddf_timer_init(&(ptr->mmc3xxx_timer), *dd_handle_ptr, 
                                  &sns_mmc3xxx_driver_fn_list, NULL, 0)) != SNS_DDF_SUCCESS)
  {
    DDF_MSG_0(FATAL, "in M34160PJ-init main tmr err");

	return stat;
  }
   
   DDF_MSG_1(FATAL, "in M34160PJ-init leaving -stat is is %d", stat);
  return stat;
}



static void sns_dd_mmc3xxx_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  uint8_t magnetic_read_data[6], out, size;
  int32_t magnetic_32bit[3];  //changed-memsic-9-10 
  float magData[3];
  sns_ddf_status_e stat;
  sns_ddf_sensor_data_s*    mmc3xxxx_data_out;
  sns_ddf_sensor_sample_s*  data_ptr;
  uint8_t  magnetic_write_buffer;
  sns_dd_mmc3xxx_state_t *state = (sns_dd_mmc3xxx_state_t *)dd_handle;
  DDF_MSG_0(FATAL, "in M34160PJ-handle timer");
  size = sizeof(sns_ddf_sensor_data_s);
  
  if((stat = sns_ddf_malloc((void **)&mmc3xxxx_data_out, size)) != SNS_DDF_SUCCESS)
  {
    DDF_MSG_0(FATAL, "in M34160PJ-malloc mmc3xxxdataout error");
	return;
  }
   
  size = SDD_MAG_NUM_AXES*sizeof(sns_ddf_sensor_sample_s);
  if((stat =sns_ddf_malloc((void **)&data_ptr, size)) != SNS_DDF_SUCCESS)
 
  {
        DDF_MSG_0(FATAL, "in M34160PJ-malloc data_ptr error");
		return;
  }  
  
  if (isSaturated()) 
  {
      DDF_MSG_0(FATAL, "in M34160PJ-saturated");
      mmc3xxx_saturation_flag = 0;
      if (mag_sensor->type == MMC34160PJ) {
          magnetic_write_buffer = MMC3XXX_SENSOR_SET;
          if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
          {
              return;
          }
      }
  } 
  else  // not issaturated()
  {
  DDF_MSG_0(FATAL, "in M34160PJ-handle timer after malloc");
  
  if ( (stat = sns_ddf_read_port(state->port_handle,
                              MMC3XXX_SENSOR_DATA_REG_ADDR, 
                              magnetic_read_data,
                              6,
                              &out)) != SNS_DDF_SUCCESS)
  {
     return;
  }

   DDF_MSG_0(FATAL, "in M34160PJ-handle timer X");
   magnetic_32bit[0] = (((int32_t)magnetic_read_data[1] << 8) | (int32_t)magnetic_read_data[0]) & 0xffff;  //changed-memsic-9-20
   DDF_MSG_1(FATAL, "in M34160PJ-handle timer X %d", magnetic_32bit[0]);
 
  
   magnetic_32bit[1] = (((int32_t)magnetic_read_data[3] << 8) | (int32_t)magnetic_read_data[2]) & 0xffff;  //changed-memsic-9-20
   DDF_MSG_1(FATAL, "in M34160PJ-handle timer Y %d", magnetic_32bit[1]);
   magnetic_32bit[2] = (((int32_t)magnetic_read_data[5] << 8) | (int32_t)magnetic_read_data[4]) & 0xffff;  //changed-memsic-9-20
   DDF_MSG_1(FATAL, "in M34160PJ-handle timer Z %d", magnetic_32bit[2]);

   magData[0] = MAG_UNIFY(magnetic_32bit[0], mag_sensor->bias, mag_sensor->scale); 
   magData[1] = MAG_UNIFY(magnetic_32bit[1], mag_sensor->bias, mag_sensor->scale); 
   magData[2] = MAG_UNIFY(magnetic_32bit[2], mag_sensor->bias, mag_sensor->scale); 
   DDF_MSG_2(FATAL, "in M34160PJ-handle timer mag data  %d %d %",  magData[0], magData[1]);
   DDF_MSG_1(FATAL, "in M34160PJ-handle timer mag data  %d",  magData[2]);
   DDF_MSG_2(FATAL, "in M34160PJ-handle timer mag bias  %d %d ", mag_sensor->bias, mag_sensor->scale);

      if (monitorSaturation(magData)) 
           {
		    mmc3xxx_saturation_flag = 1;
		    DDF_MSG_0(FATAL, "in M34160PJ- set as saturated");
		   }
      else
           {
		    mmc3xxx_saturation_flag = 0;
		   }

      if (!isSaturated()) {
	       DDF_MSG_0(FATAL, "in M34160PJ-not saturated");
   
   /* X-axis */
   /* lsb=magnetic_read_data[0], msb=magnetic_read_data[1] */
   /* retrieve the data as a 10 bit value and perform sign extension */
   DDF_MSG_0(FATAL, "in M34160PJ-start data");
    
   state->data_cache[SDD_MAG_X] = FX_DIV_Q16(FX_FLTTOFIX_Q16((magnetic_32bit[0] - mag_sensor->bias)), FX_FLTTOFIX_Q16(mag_sensor->scale));
		
   DDF_MSG_1(FATAL, "in M34160PJ-state data cache x %d",  state->data_cache[SDD_MAG_X]);
   /* Y-axis */
   /* lsb=magnetic_read_data[2], msb=magnetic_read_data[3] */
   /* retrieve the data as a 10 bit value and perform sign extension */
  
   state->data_cache[SDD_MAG_Y] = FX_DIV_Q16(FX_FLTTOFIX_Q16((magnetic_32bit[1] - mag_sensor->bias)), FX_FLTTOFIX_Q16(mag_sensor->scale));  
              
   DDF_MSG_1(FATAL, "in M34160PJ-state data cache y %d",  state->data_cache[SDD_MAG_Y]);
   /* Z-axis */
   /* lsb=magnetic_read_data[4], msb=magnetic_read_data[5] */
   /* retrieve the data as a 10 bit value and perform sign extension */

   state->data_cache[SDD_MAG_Z] = FX_DIV_Q16(FX_FLTTOFIX_Q16((magnetic_32bit[2] - mag_sensor->bias)),  FX_FLTTOFIX_Q16(mag_sensor->scale));

   DDF_MSG_1(FATAL, "in M34160PJ-state data cache z  %d",  state->data_cache[SDD_MAG_Z]);  
 
  }  //  not issaturated sub functions
  
   DDF_MSG_0(FATAL, "in M34160PJ-end not issaturated");
   
}   // end not issaturated
  
  DDF_MSG_0(FATAL, "in M34160PJ end not saturated ");

  /* The filling of the data doen in reverse order to avoid extra checks
  to fill up the logging structure
  */
  // Data to the phone's coordinate sys.
  sns_ddf_map_axes(&state->axes_map, state->data_cache);
  
  data_ptr[0].sample = state->data_cache[SDD_MAG_X];
  data_ptr[1].sample = state->data_cache[SDD_MAG_Y];
  data_ptr[2].sample = state->data_cache[SDD_MAG_Z];
  
  data_ptr[0].status = SNS_DDF_SUCCESS;
  data_ptr[1].status = SNS_DDF_SUCCESS;
  data_ptr[2].status = SNS_DDF_SUCCESS;
	  
  DDF_MSG_0(FATAL, "in M34160PJ-get data samples");  
  mmc3xxxx_data_out->sensor = SNS_DDF_SENSOR_MAG;
  mmc3xxxx_data_out->status = SNS_DDF_SUCCESS;
  mmc3xxxx_data_out->timestamp = sns_ddf_get_timestamp();
  mmc3xxxx_data_out->samples     = data_ptr;
  mmc3xxxx_data_out->num_samples = SDD_MAG_NUM_AXES;

  if (((mmc3xxxx_data_out->timestamp / 1000) - last_reset_time) >= MMC3XXX_RESET_PERIOD)  //changed from 1000000 by memsic 9-27
  {
     DDF_MSG_1(FATAL, "in M34160PJ- over 15 seconds %d", last_reset_time);
     mmc3xxx_saturation_flag = 1;
  }
  
  
  DDF_MSG_1(FATAL, "in M34160PJ- samples 0 %d", data_ptr[0].sample);
  DDF_MSG_1(FATAL, "in M34160PJ- samples 1 %d", data_ptr[1].sample);
  DDF_MSG_1(FATAL, "in M34160PJ- samples 2 %d", data_ptr[2].sample);
  
  if( (stat = sns_ddf_smgr_notify_data(state->smgr_hndl, mmc3xxxx_data_out, 1)) != SNS_DDF_SUCCESS)
  {
    DDF_MSG_1(FATAL, "in M34160PJ- notify errror %d", stat);
	return;
  }
 
  DDF_MSG_0(FATAL, "in M34160PJ- at memfree");
  sns_ddf_mfree(data_ptr); 
  sns_ddf_mfree(mmc3xxxx_data_out); 


}

/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_get_data_from_device

===========================================================================*/
/*!
  @brief Apply bias to the magnetic data scaled by the range from the
  device.
 
  @detail
  - Calls the sns_dd_mag_get_data_all to fetch the scaled 3 axis
    and temp data from the device and apply bias to it.
 
  @param[in] state: Ptr to the driver structure
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: Probably change the name of this function to reflect what it is
  or even get away with this function and do the bias in the
  sns_dd_mag_get_data_all routine
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mmc3xxx_get_data_from_device
(
  sns_dd_mmc3xxx_state_t *state
)
{
  uint8_t i;
  sns_ddf_status_e stat;
   DDF_MSG_0(FATAL, "in M34160PJ-get data frm dev");
  
  uint8_t  magnetic_write_buffer, out;
  

  /* Reseting the out of bound flag which would be set in this routine if 
     if the raw data is out of bound of bound.
  */
  state->out_of_bound_flag = 0;
  DDF_MSG_0(FATAL, "in M34160PJ-get data all entry");
   // changed-memsic-9-10 at this time, change the entire body of function
   if (isSaturated()) 
   {
      last_reset_time = sns_ddf_get_timestamp() / 1000;  // chagned to 1000 from 1000000 by memsic 9/27
      if (mag_sensor->type == MMC34160PJ) 
	  {
          magnetic_write_buffer = MMC3XXX_SENSPR_FILLCAP;
          if( (stat = sns_ddf_write_port(state->port_handle,
                                    MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                                    &magnetic_write_buffer,
                                    1,
                                    &out)) != SNS_DDF_SUCCESS)
          {
              return stat;
          }
          DDF_MSG_0(FATAL, "in M34160PJ-reset timerstart");
          if( (stat = sns_ddf_timer_start(state->mmc3xxx_timer, 10000)) != SNS_DDF_SUCCESS)
          {
              DDF_MSG_0(FATAL, "in M34160PJ-reset timerstart -errr");
	      return stat;
          }
      }
      else 
	  {
          magnetic_write_buffer = MMC3XXX_SENSOR_SET;
          if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
          {
              return stat;
          }
          DDF_MSG_0(FATAL, "in M33160/m3280-reset timerstart");
          if( (stat = sns_ddf_timer_start(state->mmc3xxx_timer, 10000)) != SNS_DDF_SUCCESS)
          {
              //DDF_MSG_0(FATAL, "in M36160-reset timerstart -errr");
	      return stat;
          }
      }
  }
  else   //not issaturated()
  {
  /* take measurement */
  magnetic_write_buffer = MMC3XXX_SENSOR_TM;
  if( (stat = sns_ddf_write_port(state->port_handle,
                              MMC3XXX_SENSOR_CTRL0_REG_ADDR, 
                              &magnetic_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
      {
          return stat;
      }
  
      DDF_MSG_0(FATAL, "in M34160PJ-get data timerstart");
      if( (stat = sns_ddf_timer_start(state->mmc3xxx_timer, 10000)) != SNS_DDF_SUCCESS)
        {
          DDF_MSG_0(FATAL, "in M34160PJ-get data timerstart -errr");
	  return stat;
        }
  
   if ( stat == SNS_DDF_SUCCESS)
  {
     /* Bias correction, for mag sensor, set bias to 0 */
     for (i=0; i<SDD_MAG_NUM_AXES; i++)
     {
       state->bias[i] = 0;
       state->data_cache[i] = state->data_cache[i] - state->bias[i];
     }
  }
  
}      
  return stat;

}

#ifndef DDK
/*===========================================================================
  FUNCTION:   sns_dd_magnetic_log_data
===========================================================================*/
/*!
  @brief log sensor data
 
  @detail
  - Logs latest set of sensor data sampled from the sensor. 
 
  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None
 
  $TODO: 
*/
/*=========================================================================*/
void sns_dd_magnetic_log_data(
   sns_dd_mmc3xxx_state_t *state,
   sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAGNETIC;  
#ifndef ACCELEROMETERDRV
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_MEMSIC;
#else
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;
#endif
    
    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = SDD_MAG_NUM_DATATYPES;
    log_struct_ptr->data[0]  = state->data_cache[SDD_MAG_Y];
    log_struct_ptr->data[1] = state->data_cache[SDD_MAG_X];
    log_struct_ptr->data[2]  = state->data_cache[SDD_MAG_Z];
    log_struct_ptr->data[3] = state->data_cache[SDD_TEMP];


    //Commit log (also frees up the log packet memory)
    err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, 
                                   log_struct_ptr);
  }

  if (err_code != SNS_SUCCESS)
  {
    //printf("Failed to log sensor data packet with error code %d", err_code);
  }
}
#endif

/*===========================================================================

  FUNCTION:   sns_dd_mag_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data
 
  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Data is returned immediately after being read from the
    sensor, in which case data[] is populated in the same order it was
    requested,
 
  @param[in]  dd_handle    Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: Update the each sample status after a check for bound is done
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mmc3xxx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
)
{
  uint8_t i;
  sns_ddf_status_e stat;
  
  sns_dd_mmc3xxx_state_t *state = (sns_dd_mmc3xxx_state_t *)dd_handle;
  
  DDF_MSG_0(FATAL, "in M34160PJ-get data main");
  /* Sanity check*/
  for (i = 0; i < num_sensors; i++)
  {
    if (sensors[i] != SNS_DDF_SENSOR_MAG )
    {
     DDF_MSG_0(FATAL, "in M34160PJ-get data main err");     
	 return SNS_DDF_EINVALID_PARAM;
    }
  }
 
   
  /* this is a sync driver, so try to read data now */
  if ( (stat = sns_dd_mmc3xxx_get_data_from_device(state) != SNS_DDF_SUCCESS))
  {
     DDF_MSG_0(FATAL, "in M34160PJ-get data main err");
	 return stat;
  }	 
  else
  {
    DDF_MSG_0(FATAL, "in M34160PJ-get data -out");
    return SNS_DDF_PENDING;
  }
  
 
}

/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  - range and resolution info is from the device data sheet.
 
  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO: 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_mmc3xxx_get_attr
(
  sns_ddf_handle_t       dd_handle,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  sns_ddf_memhandler_s*  memhandler,
  void**                 value,
  uint32_t*              num_elems
)
{

  if ( (dd_handle == NULL) || (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
    if (sensor != SNS_DDF_SENSOR_MAG)
   {
    return SNS_DDF_EINVALID_PARAM;
   }

  
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
    {
      DDF_MSG_0(FATAL, "in M34160PJ- attribute pwr info");
	  sns_ddf_power_info_s* power_attrib;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                    sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib = *value;
      power_attrib->active_current = 700;
      *num_elems = 1;
    }
    break;

    // repeated twice I pic the one below
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
       DDF_MSG_0(FATAL, "in M34160PJ-get attr range");     
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      SNS_DD_MAGNETIC_MAX_RANGE*sizeof(sns_ddf_range_s))) 
            == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_ranges = *value;
        *num_elems = 1;
		device_ranges->min  = FX_FLTTOFIX_Q16((-1)*MMC3XXX_SENSOR_OPERATION_RANGE);
        device_ranges->max  = FX_FLTTOFIX_Q16(MMC3XXX_SENSOR_OPERATION_RANGE);
        DDF_MSG_2(MED, "range min%d range max %d", (-1)*(MMC3XXX_SENSOR_OPERATION_RANGE), (MMC3XXX_SENSOR_OPERATION_RANGE));
      
    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      DDF_MSG_0(FATAL, "in M34160PJ-get attr res aDC");
	  sns_ddf_resolution_adc_s *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
     
      device_res = *value;

      *num_elems = 1;
      device_res->bit_len = mag_sensor->bit_len;
      device_res->max_freq = mag_sensor->freq;
    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION:
     {
	  DDF_MSG_0(FATAL, "in M34160PJ-get attr res");
	  sns_ddf_resolution_t *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      device_res = *value;

      *num_elems = 1;
      if (mag_sensor->type == MMC3280MA)
	    {
         *device_res = FX_DIV_Q16(FX_FLTTOFIX_Q16(1),FX_FLTTOFIX_Q16(512));
		 DDF_MSG_2(MED, "mmc3280ma res %x resq16 %x", FX_FLTTOFIX_Q16(1 / 512), FX_DIV_Q16(FX_FLTTOFIX_Q16(1),FX_FLTTOFIX_Q16(512)));
		}
      else if (mag_sensor->type == MMC33160MT)
	    {
         *device_res = FX_DIV_Q16(FX_FLTTOFIX_Q16(1),FX_FLTTOFIX_Q16(512));
		 DDF_MSG_2(MED, "mmc33160t res %x resq16 %x", FX_FLTTOFIX_Q16(1 / 512),FX_DIV_Q16(FX_FLTTOFIX_Q16(1),FX_FLTTOFIX_Q16(512)));
		}
      else if (mag_sensor->type == MMC34160PJ)
        {
		 *device_res = FX_DIV_Q16(FX_FLTTOFIX_Q16(1), FX_FLTTOFIX_Q16(2048));
		 DDF_MSG_2(MED, "mmc34160pj res %x resq16 %x", FX_FLTTOFIX_Q16(1 / 2048),FX_DIV_Q16(FX_FLTTOFIX_Q16(1),FX_FLTTOFIX_Q16(2048)));
		}
	 }
     break;

    case SNS_DDF_ATTRIB_DELAYS:
    {
      DDF_MSG_0(FATAL, "in M34160PJ-get attr delay");
	  sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                    sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay = *value;
      *num_elems = 1;
      //Not clear what this should be!
      device_delay->time_to_active = 7000; 
      device_delay->time_to_data = 10000;       
    }
    break;

    case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      DDF_MSG_0(FATAL, "in M34160PJ-get attr drv info");
	  sns_ddf_driver_info_s *driver_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      driver_info = *value;
      *num_elems = 1;
      driver_info->name = "M34160PJ";
      driver_info->version = 1;
    }
    break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      DDF_MSG_0(FATAL, "in M34160PJ-get attr dev info");
	  sns_ddf_device_info_s *device_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_info = *value;
      *num_elems = 1;
      if (mag_sensor->type == MMC34160PJ)
          device_info->model = "MMC34160PJ";
      else if (mag_sensor->type == MMC33160MT)
          device_info->model = "MMC33160MT";
      else
          device_info->model = "MMC3280MA";
          device_info->vendor = "MEMSIC";
          device_info->name = mag_sensor->name;
          device_info->version = 1;
    }
    break;

    default:
     return SNS_DDF_EINVALID_PARAM;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_mmc3xxx_probe

===========================================================================*/
/**
 * @brief Probes for the device with a given configuration.
 *
 * This commands the driver to look for the device with the specified
 * configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
 * struct.
 *
 * @param[in]  dev_info    Access info for physical devices controlled by 
 *                         this driver. Used to determine if the device is
 *                         physically present.
 * @param[in]  memhandler  Memory handler used to dynamically allocate 
 *                         output parameters, if applicable.
 * @param[out] num_sensors Number of sensors supported. 0 if none.
 * @param[out] sensor_type Array of sensor types supported, with num_sensor
 *                         elements. Allocated by this function.
 *
 * @return SNS_DDF_SUCCESS if the part was probed function completed, even
 *         if no device was found (in which case num_sensors will be set to
 *         0).
 */
static sns_ddf_status_e sns_dd_mmc3xxx_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  MMC3XXX_TYPE probe_sensor_type;
  uint8_t bytes_read;
  uint8_t pid[2];

  status = SNS_DDF_SUCCESS;
  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }
    DDF_MSG_0(FATAL, "in M34160PJ-probe");
  
  /* Read & Verify Device ID */
  
  status = sns_ddf_read_port(port_handle,
                              MMC3XXX_SENSOR_ID_REG1_ADDR, 
                              &pid[0],
                              1,
                              &bytes_read);
							  
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  
   status = sns_ddf_read_port(port_handle,
                              MMC3XXX_SENSOR_ID_REG2_ADDR, 
                              &pid[1],
                              1,
                              &bytes_read);
							  
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  
  probe_sensor_type = sns_dd_mmc3xxx_detect(pid);
   DDF_MSG_0(FATAL, "in M34160PJ-probe -pid detect");

   /*sns_dd_mmc3xxx_detect behavior
  pid[0] == 0x3) && (pid[1] == 0x5) ==> MMC33160M, return 0
  (pid[0] == 0x3) && (pid[1] == 0x4) ==> MMC3280MA; return 1
  (pid[0] == 0x1) && (pid[1] == 0x6) ==> MMC34160PJ; return 2
  if none of these, ==> MMC3XXX; return 3
*/
  
  if (probe_sensor_type == MMC3XXX)
   {
    /* Incorrect value. Return now with no MMC3xxxx part detected */
    sns_ddf_close_port( port_handle );
    return status;
   }
  
    DDF_MSG_0(FATAL, "found sensor");
  /* Registers are correct. This is probably an MMC3xxxx mag */
  *num_sensors = 1;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_MAG;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;

}



