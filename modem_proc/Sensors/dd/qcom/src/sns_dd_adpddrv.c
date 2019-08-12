/********************************************************************************
* Copyright (c) 2014, Analog Devices, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of the Analog Devices nor the
*        names of its contributors may be used to endorse or promote products
*        derived from this software without specific prior written permission.
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
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
/*============================================================================
    Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/
 
#include <stdio.h>
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_dd_adpddrv.h"

//#define REG_CONFIG_DATA_QCOM
#define ADPDDrv_SLOT_SELECT_A	0           // 1 - Mode Enabled, 0 - Mode Disabled
#define ADPDDrv_SLOT_SELECT_B	0           // 1 - Mode Enabled, 0 - Mode Disabled
#define ADPDDrv_SLOT_SELECT_AB	1           // 1 - Mode Enabled, 0 - Mode Disabled

#ifdef FEATURE_APDP_INT_PIN_CONFIG  
#include "sns_dd_gyro_priv.h"
#endif

//#define POWER_SAVING                  // Disable FIFO clock when no access.
//#define USE_SAMPLE_IRQ                // use Sample IRQ instead of FIFO IRQ
#define ADPDDrv_INTERRUPT					// DRI mode for normal operation
//#define ADPDDrv_POLLING					// Polling mode

/* ADPDDrv driver version*/
#define ADPDDrv_VERSION			6.0

/*INPUT DEVICE NAME LIST*/
#define MODULE_NAME_HRM			"HRM"
#define CHIP_NAME				"ADPD"
#define DRIVER_NAME				"ADPDDrv"
#define VENDOR					"Analog Devices Inc."

/**
structure hold adpd142 device configuration data to be written during initialization.
*/
static sns_dd_adpddrv_reg_data_t adpd142_config_data[MAX_CONFIG_REG_CNT] = {
#ifdef REG_CONFIG_DATA_QCOM

	{0x00, 0x0000},
	{0x10, 0x0002},
	{0x01, 0x01BF},
	{0x02, 0x0004},
	{0x06, 0x0000},
	{0x11, 0x1030},
	{0x4b, 0x2692},
	{0x4d, 0x425E},
	{0x12, 0x0014},
	{0x13, 0x0320},
	{0x14, 0x0449},
	{0x15, 0x0220},
	{0x1C, 0x8080},
	{0x1D, 0x8080},
	{0x18, 0x2000},
	{0x19, 0x2000},
	{0x1a, 0x2000},
	{0x1b, 0x2000},
	{0x1e, 0x2000},
	{0x1f, 0x2000},
	{0x20, 0x2000},
	{0x21, 0x2000},
	{0x23, 0x1033},
	{0x24, 0x3004},
	{0x25, 0x030C},
	{0x34, 0x0000},
	{0x26, 0x0000},
	{0x27, 0x0800},
	{0x28, 0x0000},
	{0x29, 0x4003},
	{0x30, 0x0430},
	{0x31, 0x2013},
	{0x42, 0x1C35},
	{0x43, 0xADA5},
	{0x39, 0x2CD0},
	{0x35, 0x0430},
	{0x36, 0x0813},
	{0x44, 0x1C35},
	{0x45, 0xADA5},
	{0x59, 0x0808},
	{0x32, 0x0320},
	{0x33, 0x0113},
	{0x3a, 0x22d4},
	{0x3c, 0x0006},
	{0x40, 0x1010},
	{0x41, 0x004c},
	{0x4c, 0x0004},
	{0x4f, 0x2090},
	{0x50, 0x0000},
	{0x52, 0x0040},
	{0x53, 0xe400},
	{0x5e, 0x2808},
	{0x4e, 0x0040},
	{0x3B, 0x2CD0},
	{0x5F, 0x0000},
	{0xFF, 0XFFFF},

#else
	
	{0x10, 0x0001},
	{0x01, 0xc0ff},
	{0x02, 0x0004},
	{0x06, 0x0000},
	{0x11, 0x1011},
	{0x4b, 0x2695},
	{0x4d, 0x4272},
	{0x12, 0x000A},
	{0x13, 0x0320},
	{0x14, 0x0449},
	{0x15, 0x0333},
	{0x1C, 0x8080},
	{0x1D, 0x8080},
	{0x18, 0x1400},
	{0x19, 0x1400},
	{0x1a, 0x1650},
	{0x1b, 0x1000},
	{0x1e, 0x1ff0},
	{0x1f, 0x1Fc0},
	{0x20, 0x1ad0},
	{0x21, 0x1470},
	{0x23, 0x1032},
	{0x24, 0x1034},
	{0x25, 0x02CC},
	{0x34, 0x0000},
	{0x26, 0x0000},
	{0x27, 0x0800},
	{0x28, 0x0000},
	{0x29, 0x4003},
	{0x30, 0x0330},
	{0x31, 0x0813},
	{0x42, 0x1C36},
	{0x43, 0xADA5},
	{0x39, 0x24D4},
	{0x35, 0x0330},
	{0x36, 0x0813},
	{0x44, 0x1C36},
	{0x45, 0xADA5},
	{0x59, 0x0808},
	{0x32, 0x0320},
	{0x33, 0x0113},
	{0x3a, 0x22d4},
	{0x3c, 0x0006},
	{0x40, 0x1010},
	{0x41, 0x004c},
	{0x4c, 0x0004},
	{0x4f, 0x2090},
	{0x50, 0x0000},
	{0x52, 0x0040},
	{0x53, 0xe400},
	{0x5e, 0x2808},
	{0x4e, 0x0040},
	{0x3B, 0x24D4},
	{0xFF, 0XFFFF},	
	
#endif	

};

/**
structure hold adpd153 device configuration data to be written during initialization.
*/
static sns_dd_adpddrv_reg_data_t adpd153_config_data[MAX_CONFIG_REG_CNT] = {
	{0x10, 0x0001},
	{0x01, 0xc0ff},
	{0x11, 0x1011},
	{0x4b, 0x2695},
	{0x4d, 0x4272},
	{0x02, 0x0004},
	{0x06, 0x0000},
	{0x12, 0x000A},
	{0x13, 0x0320},
	{0x14, 0x0447},
	{0x15, 0x0333},
	{0x1C, 0x8080},
	{0x1D, 0x8080},
	{0x18, 0x1F00},
	{0x19, 0x1F00},
	{0x1a, 0x1F00},
	{0x1b, 0x1F00},
	{0x1e, 0x1F00},
	{0x1f, 0x1F00},
	{0x20, 0x1F00},
	{0x21, 0x1F00},
	{0x22, 0x3000},
	{0x23, 0x300C},
	{0x24, 0x300C},
	{0x25, 0x02CC},
	{0x34, 0x0000},
	{0x26, 0x0000},
	{0x27, 0x0800},
	{0x28, 0x0000},
	{0x29, 0x4003},
	{0x30, 0x0330},
	{0x31, 0x0813},
	{0x42, 0x1C36},
	{0x43, 0xADA5},
	{0x39, 0x24D4},
	{0x35, 0x0330},
	{0x36, 0x0813},
	{0x44, 0x1C36},
	{0x45, 0xADA5},
	{0x3b, 0x24D4},
	{0x59, 0x0808},
	{0x32, 0x0320},
	{0x33, 0x0113},
	{0x3a, 0x22d4},
	{0x3c, 0x0006},
	{0x40, 0x1010},
	{0x41, 0x004c},
	{0x4c, 0x0004},
	{0x4f, 0x2090},
	{0x50, 0x0000},
	{0x52, 0x0040},
	{0x53, 0xe400},
	{0x5e, 0x2808},
	{0x4e, 0x0040},
	{0x3B, 0x24D4},
	{0xFF, 0XFFFF},
};

/*===========================================================================*/
/*
*  Local functions
*/
/*===========================================================================*/
static sns_ddf_status_e adpddrv_enable_interrupt(sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor);

static sns_ddf_status_e adpddrv_disable_interrupt(sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor);

static sns_ddf_status_e sns_dd_hrm_adpddrv_set_pwr_mode(sns_dd_hrm_state_t* adpddrv_handler,
	int power_mode);
	
#ifdef FEATURE_APDP_INT_PIN_CONFIG 
static  sns_ddf_status_e   sns_dd_mpu3050_int_disable(void);
#endif 

/*===========================================================================*/
/*
*  External functions
*/
/*===========================================================================*/
extern sns_ddf_status_e sns_dd_hrm_adpddrv_get_data(sns_ddf_handle_t dd_handle,	sns_ddf_sensor_e sensors[],
	uint32_t num_sensors, sns_ddf_memhandler_s* memhandler,	sns_ddf_sensor_data_s** data);
extern void sns_dd_hrm_adpddrv_handle_timer(sns_ddf_handle_t dd_handle, void* arg);
extern void sns_dd_hrm_adpddrv_handle_irq(sns_ddf_handle_t handle, uint32_t gpio_num,
	sns_ddf_time_t timestamp);
/*===========================================================================
FUNCTION      sns_dd_hrm_adpddrv_device_regs_debug

DESCRIPTION   Read device registers and prints the result.

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
static void sns_dd_hrm_adpddrv_device_regs_debug(
	sns_dd_hrm_state_t *adpddrv_handler)
{
	int i;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;
	uint8_t reg_val[2];
	uint8_t nbytes;
	
	for ( i=0 ; i<adpddrv_handler->device_config_size ; i++ )
	{
		reg_val[0] = 0;
		reg_val[1] = 0;
		ret = sns_ddf_read_port(adpddrv_handler->port_handle, adpddrv_handler->device_config[i].register_address,
										(uint8_t *)&reg_val[0], 2, &nbytes);
		if (ret != SNS_DDF_SUCCESS)
		{
			ADPDDrv_MSG_1(ERROR,"read port error %d", ret);
		}
		ADPDDrv_MSG_3(MED,"reg 0x%02X = 0x%02X%02X", adpddrv_handler->device_config[i].register_address, reg_val[0], reg_val[1]);
	}
}

/*===========================================================================
FUNCTION      sns_dd_hrm_adpddrv_reg_write

DESCRIPTION   Function mapped to i2c write functionality

RETURN VALUE  SNS_DDF_SUCCESS if successful, error code otherwise.

SIDE EFFECT   None
===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_reg_write(
	sns_dd_hrm_state_t* adpddrv_handler,
	uint8_t reg_addr,
	uint16_t reg_val)
{
	uint8_t nbytes;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;
	uint8_t reg_data[2];
	
	reg_data[0] = ((reg_val >> 8) & 0xFF);
	reg_data[1] = (reg_val & 0xFF);
	
	ADPDDrv_MSG_2(MED,"Reg_Write = 0x%02X -> 0x%04X", reg_addr, reg_val);
	ret = sns_ddf_write_port(adpddrv_handler->port_handle, reg_addr, (uint8_t*) &reg_data[0], 2, &nbytes);
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
	{
		ADPDDrv_MSG_0(ERROR,"write port failed");
		return ret;
	}
	return ret;
}

/*===========================================================================
FUNCTION      sns_dd_hrm_adpddrv_reg_read

DESCRIPTION   Function mapped to i2c write functionality

RETURN VALUE  SNS_DDF_SUCCESS if successful, error code otherwise.

SIDE EFFECT   None
===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_reg_read(
	sns_dd_hrm_state_t* adpddrv_handler,
	uint8_t reg_addr,
	uint16_t *reg_val)
{
	uint8_t nbytes;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;
	uint8_t reg_data[2];
	
	ret = sns_ddf_read_port(adpddrv_handler->port_handle, reg_addr, (uint8_t*) &reg_data[0], 2, &nbytes);
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
	{
		ADPDDrv_MSG_0(ERROR,"read port failed");
		return ret;
	}
	*reg_val = (((reg_data[0] << 8) & 0xFF00) | (reg_data[1]));
	ADPDDrv_MSG_2(MED,"Reg_Read = 0x%02X -> 0x%04X", reg_addr, *reg_val);
	return ret;
}

/*===========================================================================
FUNCTION      sns_dd_hrm_adpddrv_setslot

DESCRIPTION   Function to set the device's timeslot

RETURN VALUE  SNS_DDF_SUCCESS if successful, error code otherwise.

SIDE EFFECT   None
===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_setslot(
	sns_dd_hrm_state_t* adpddrv_handler,
	uint8_t slot)
{
	uint16_t reg_val;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;
	
	ADPDDrv_MSG_1(MED,"sns_dd_hrm_adpddrv_setslot - %d", slot);
	reg_val = 0;
	ret = sns_dd_hrm_adpddrv_reg_read(adpddrv_handler, REG_OP_MODE_CFG, &reg_val);
	if (ret != SNS_DDF_SUCCESS)
		return ret;
	reg_val &= SLOT_MASK;
	if (slot == ADPDDrv_SLOT_A)
	{
		reg_val |= SLOT_A_MODE;
		adpddrv_handler->slotmode = ADPDDrv_SLOT_A;
		adpddrv_handler->datasetsize = SLOT_A_DATA_SIZE;
	}
	else if (slot == ADPDDrv_SLOT_AB)
	{
		reg_val |= SLOT_AB_MODE;
		adpddrv_handler->slotmode = ADPDDrv_SLOT_AB;
		adpddrv_handler->datasetsize = SLOT_AB_DATA_SIZE;
	}
	else
	{
		reg_val |= SLOT_B_MODE;
		adpddrv_handler->slotmode = ADPDDrv_SLOT_B;
		adpddrv_handler->datasetsize = SLOT_B_DATA_SIZE;
	}

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE_CFG, reg_val);
	if (ret != SNS_DDF_SUCCESS)
		return ret;
	return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_hrm_adpddrv_update_config_reg_local_copy

DESCRIPTION   Update adpddrv_handler->device_config with values in adpddrv_config_data

RETURN VALUE  SNS_DDF_SUCCESS if successful, error code otherwise.

SIDE EFFECT   None
===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_update_config_reg_local_copy(
	sns_dd_hrm_state_t *adpddrv_handler)
{
	int 				idx 	= 0;
	uint16_t			reg_val = 0;
	sns_ddf_status_e	ret 	= SNS_DDF_SUCCESS;
	
	ret = sns_dd_hrm_adpddrv_reg_read(adpddrv_handler, REG_CHIP_ID, &reg_val);
	if (ret != SNS_DDF_SUCCESS)
		return ret;
	reg_val &= 0x0FFF;
	ADPDDrv_MSG_1(HIGH,"Chip Id - 0x%x", reg_val);
	switch(reg_val)
	{
	case ADPD_CHIPID(0):
		for ( idx=0; ; idx++ )
		{
			if((adpd142_config_data[idx].register_address != 0xFF) && (adpd142_config_data[idx].register_value != 0xFFFF))
			{
				adpddrv_handler->device_config[idx].register_address    = adpd142_config_data[idx].register_address;
				adpddrv_handler->device_config[idx].register_value	    = adpd142_config_data[idx].register_value;
			}
			else
			{
				break;
			}
		}
		break;
	case ADPD_CHIPID(1):
		for ( idx=0; ; idx++ )
		{
			if((adpd153_config_data[idx].register_address != 0xFF) && (adpd153_config_data[idx].register_value != 0xFFFF))
			{
				adpddrv_handler->device_config[idx].register_address    = adpd153_config_data[idx].register_address;
				adpddrv_handler->device_config[idx].register_value	    = adpd153_config_data[idx].register_value;
			}
			else
			{
				break;
			}
		}
		break;
	default:
		ADPDDrv_MSG_0(ERROR,"Chip Id is not matched with current driver settings");
		ret = SNS_DDF_EDEVICE;
		break;
	}
	adpddrv_handler->device_config_size = idx;
	ADPDDrv_MSG_1(HIGH,"Device config size - %d", adpddrv_handler->device_config_size);
	
	return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_config_regs: writes device registers.
*
* @param[in] dd_handle Handle to driver instance.
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_config_regs
(
	sns_dd_hrm_state_t *adpddrv_handler
)
{
	int i;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;
	for ( i=0 ; i<adpddrv_handler->device_config_size ; i++ )
	{

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, adpddrv_handler->device_config[i].register_address, 
										   adpddrv_handler->device_config[i].register_value);
		if(ret != SNS_DDF_SUCCESS)
			return ret;
	}
	return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_reset: sets the chip to reset adpddrv_handler.
*
* @param[in] dd_handle Handle to driver instance.
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_reset(
	sns_ddf_handle_t dd_handle)
{
	sns_dd_hrm_state_t* adpddrv_handler = (sns_dd_hrm_state_t *)dd_handle;
	sns_ddf_status_e ret = SNS_DDF_SUCCESS;

#ifdef FEATURE_APDP_INT_PIN_CONFIG 
    sns_dd_mpu3050_int_disable();
#endif 

	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_reset");

	/* basic sanity check */
	if (dd_handle == NULL)
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	ret = sns_dd_hrm_adpddrv_set_pwr_mode(adpddrv_handler, SNS_DDF_POWERSTATE_LOWPOWER);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	// adpddrv_handler variables reset
	adpddrv_handler->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
	adpddrv_handler->hrm_odr = 0;
	return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_init Initializes the ADPDDrv device driver
*
* @detail Allocates handle for this driver instance.
*         Opens I2C communication port. 
*         Places device in default power state.
*         This function will be called at init time.
*   
*  @param[out] dd_handle_ptr  Pointer that this function must malloc and 
*                             populate. This is a handle to the driver
*                             instance that will be passed in to all other
*                             functions. NB: Do not use @a memhandler to
*                             allocate this memory.
*  @param[in]  smgr_handle    Handle used to identify this driver when it 
*                             calls into Sensors Manager functions.
*  @param[in]  nv_params      NV parameters retrieved for the driver.
*  @param[in]  device_info    Access info for physical devices controlled by 
*                             this driver. Used to configure the bus
*                             and talk to the devices.
*  @param[in]  num_devices    Number of elements in @a device_info. 
*  @param[in]  memhandler     Memory handler used to dynamically allocate 
*                             output parameters, if applicable. NB: Do not
*                             use memhandler to allocate memory for
*                             @a dd_handle_ptr.
*  @param[out] sensors        List of supported sensors, allocated, 
*                             populated, and returned by this function.
*  @param[out] num_sensors    Number of elements in @a sensors.
* 
*  @return Success if @a dd_handle_ptr was allocated and the driver was 
*          configured properly. Otherwise a specific error code is returned.
*/
/*===========================================================================*/

sns_ddf_status_e sns_dd_hrm_adpddrv_init(
	sns_ddf_handle_t*        dd_ptr,
	sns_ddf_handle_t         smgr_handle,
	sns_ddf_nv_params_s*     reg_params,
	sns_ddf_device_access_s  device_info[],
	uint32_t                 num_devices,
	sns_ddf_memhandler_s*    memhandler,
	sns_ddf_sensor_e*        sensors[],
	uint32_t*                num_sensors)
{
	sns_dd_hrm_state_t*          adpddrv_handler;
	sns_ddf_status_e             ret = SNS_DDF_SUCCESS;
	ADPDDrv_OPERATION_SLOT		selected_slot = ADPDDrv_SLOT_B;
	
	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_init");

	/* basic sanity check */
	if (dd_ptr == NULL || sensors == NULL || num_sensors == NULL)
	{
		return SNS_DDF_EINVALID_PARAM;
	}
#ifdef ADPD_UIMAGE_MODE
	sns_ddf_smgr_set_uimg_refac(smgr_handle);
		/* allocate _state memory */
	if (sns_ddf_malloc_ex((void **)&adpddrv_handler, sizeof(*adpddrv_handler), smgr_handle) != SNS_DDF_SUCCESS)
	{
		return SNS_DDF_ENOMEM;
	}
	if (sns_ddf_malloc_ex((void **)&adpddrv_handler->s_data, sizeof(adpddrv_handler->s_data), smgr_handle) != SNS_DDF_SUCCESS)
	{
		sns_ddf_mfree_ex(adpddrv_handler, smgr_handle);
		return SNS_DDF_ENOMEM;
	}
	if (sns_ddf_malloc_ex((void **)&adpddrv_handler->samples, 3 * sizeof(sns_ddf_sensor_sample_s), smgr_handle) != SNS_DDF_SUCCESS)
	{
		sns_ddf_mfree_ex(adpddrv_handler->s_data, smgr_handle);
		sns_ddf_mfree_ex(adpddrv_handler, smgr_handle);
		return SNS_DDF_ENOMEM;
	}
#else // uImage_Mode
	/* allocate _state memory */
	if (sns_ddf_malloc((void **)&adpddrv_handler, sizeof(*adpddrv_handler)) != SNS_DDF_SUCCESS)
	{
		return SNS_DDF_ENOMEM;
	}
	if (sns_ddf_malloc((void **)&adpddrv_handler->s_data, sizeof(adpddrv_handler->s_data)) != SNS_DDF_SUCCESS)
	{
		sns_ddf_mfree(adpddrv_handler);
		return SNS_DDF_ENOMEM;
	}

	if (sns_ddf_malloc((void **)&adpddrv_handler->samples, 3 * sizeof(sns_ddf_sensor_sample_s)) != SNS_DDF_SUCCESS)
	{
		sns_ddf_mfree(adpddrv_handler->s_data);
		sns_ddf_mfree(adpddrv_handler);
		return SNS_DDF_ENOMEM;
	}
#endif // uImage_Mode

	*dd_ptr = (sns_ddf_handle_t)adpddrv_handler;
	*num_sensors = 1;
	*sensors = (sns_ddf_sensor_e *)sns_dd_hrm_adpddrv_sensor_types;
	adpddrv_handler->smgr_handle = smgr_handle;
	
	/* open I2C port */
	ret = sns_ddf_open_port(&adpddrv_handler->port_handle, &device_info->port_config);
	if (ret != SNS_DDF_SUCCESS)
	{
		sns_ddf_mfree(adpddrv_handler->samples);
		sns_ddf_mfree(adpddrv_handler->s_data);
		sns_ddf_mfree(adpddrv_handler);
		return ret;
	}

#ifdef ADPDDrv_POLLING
	ret = sns_ddf_timer_init(&adpddrv_handler->timer_obj, 
							adpddrv_handler->smgr_handle,
#ifdef USE_DRAGONBOARD_BUILD
							&sns_dd_vendor_if_2,
#esle  /* USE_DRAGONBOARD_BUILD */
							&sns_dd_ppg_adpd142_if,
#endif  /* USE_DRAGONBOARD_BUILD */
							adpddrv_handler,
							false);
#else
#endif
	if (ret != SNS_DDF_SUCCESS)
	{
		sns_ddf_close_port(adpddrv_handler->port_handle);
		sns_ddf_mfree(adpddrv_handler->samples);
		sns_ddf_mfree(adpddrv_handler->s_data);
		sns_ddf_mfree(adpddrv_handler);
		return ret;
	}
	
	ret = sns_dd_hrm_adpddrv_update_config_reg_local_copy(adpddrv_handler);
	if (ret != SNS_DDF_SUCCESS)
	{
		sns_ddf_close_port(adpddrv_handler->port_handle);
		sns_ddf_mfree(adpddrv_handler->samples);
		sns_ddf_mfree(adpddrv_handler->s_data);
		sns_ddf_mfree(adpddrv_handler);
		return ret;
	}

	// Configure device registers
	ret = sns_dd_hrm_adpddrv_config_regs(adpddrv_handler);
	if ( ret != SNS_DDF_SUCCESS )
	{
	  return ret;
	}	

	sns_dd_hrm_adpddrv_device_regs_debug(adpddrv_handler);

	// reset the device, reset also initializes registers
	ret = sns_dd_hrm_adpddrv_reset(adpddrv_handler);

	if (ret != SNS_DDF_SUCCESS)
	{
		sns_ddf_close_port(adpddrv_handler->port_handle);
		sns_ddf_mfree(adpddrv_handler->samples);
		sns_ddf_mfree(adpddrv_handler->s_data);
		sns_ddf_mfree(adpddrv_handler);
		return ret;
	}
	
#if ADPDDrv_SLOT_SELECT_B
	selected_slot = ADPDDrv_SLOT_B;
#elif ADPDDrv_SLOT_SELECT_A
	selected_slot = ADPDDrv_SLOT_A;
#elif ADPDDrv_SLOT_SELECT_AB
	selected_slot = ADPDDrv_SLOT_AB;
#endif
	
	sns_dd_hrm_adpddrv_setslot(adpddrv_handler, selected_slot);
	
#ifdef ADPDDrv_INTERRUPT
	adpddrv_handler->interrupt_gpio = device_info->first_gpio;

	if (adpddrv_handler->interrupt_gpio != 0xFFFF)
	{
		adpddrv_handler->dri_enable = true;
	}
	else
	{
		adpddrv_handler->dri_enable = false;
	}
#else
#endif
	
	return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_set_attrib: Sets certain device attributes that 
are programmable. Currently only power mode is a settable attribute.
*
* @param[in] dd_handle  Handle to driver instance.
* @param[in] sensor     Sensor for which this attribute is to be set.
* @param[in] attrib     Attribute to be set.
* @param[in] value      Value to set this attribute.
*
*  @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_set_attrib(
	sns_ddf_handle_t     dd_handle,
	sns_ddf_sensor_e     sensor,
	sns_ddf_attribute_e  attrib,
	void*                value)
{
	sns_dd_hrm_state_t * adpddrv_handler = (sns_dd_hrm_state_t *)dd_handle;
	/* basic sanity check */
	if (dd_handle == NULL || value == NULL){
		return SNS_DDF_EINVALID_PARAM;
	}

	ADPDDrv_MSG_3(HIGH,"sns_dd_hrm_adpddrv_set_attrib - sensor=%d, attrib=%d, value=%d", sensor, attrib, *(uint8_t *)value);

	/* set attribute */
	switch (attrib)
	{
		case SNS_DDF_ATTRIB_POWER_STATE:
			return sns_dd_hrm_adpddrv_set_pwr_mode((sns_dd_hrm_state_t *)dd_handle, *(sns_ddf_powerstate_e *)value);

		case SNS_DDF_ATTRIB_RANGE:
			/* Does not support different ranges */
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_ODR:
			adpddrv_handler->hrm_odr = 100; 
			return SNS_DDF_SUCCESS;

		default:
			return SNS_DDF_EINVALID_PARAM;
	}

	return SNS_DDF_EINVALID_PARAM;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_get_attrib: Gets certain device attributes that 
are programmable.
*
* @param[in]  dd_handle   Handle to driver instance.
* @param[in]  sensor      Sensor for which this attribute is to be set.
* @param[in]  attrib      Attribute to be set.
* @param[in]  memhandler  Memory handler used to dynamically allocate 
*                         output parameters, if applicable.
* @param[out] value       Value to get this attribute.
* @param[out] num_elems   number of elements to denote the number of values 
*						  included in value.
*
*  @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_get_attrib(
	sns_ddf_handle_t      handle,
	sns_ddf_sensor_e      sensor,
	sns_ddf_attribute_e   attrib,
	sns_ddf_memhandler_s* memhandler,
	void**                value,
	uint32_t*             num_elems)
{
	sns_dd_hrm_state_t       *adpddrv_handler = (sns_dd_hrm_state_t *)handle;
	sns_ddf_driver_info_s     *driver_info;
	sns_ddf_device_info_s     *device_info;
	sns_ddf_range_s           *device_ranges;
	sns_ddf_resolution_t      *resolution;
	sns_ddf_resolution_adc_s  *resolution_adc;
	sns_ddf_power_info_s      *power;
	sns_ddf_powerstate_e      *power_state;
	uint32_t				  *odr;

	ADPDDrv_MSG_2(HIGH,"sns_dd_hrm_adpddrv_get_attrib - sensor=%d, attrib=%d", sensor, attrib);

	/* basic sanity check */
	if (handle == NULL || memhandler == NULL || value == NULL || num_elems == NULL)
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	/* return attribute */
	switch (attrib)
	{
		case SNS_DDF_ATTRIB_DRIVER_INFO:
			driver_info = sns_ddf_memhandler_malloc(memhandler,
				sizeof(*driver_info));
			if (driver_info == NULL) return SNS_DDF_ENOMEM;
			driver_info->name = DRIVER_NAME;
			driver_info->version = ADPDDrv_VERSION;
			*value = driver_info;
			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_DEVICE_INFO:
			device_info = sns_ddf_memhandler_malloc(memhandler,
				sizeof(*device_info));
			if (device_info == NULL) return SNS_DDF_ENOMEM;
			device_info->name = CHIP_NAME;
			device_info->vendor = VENDOR;
			device_info->model = MODULE_NAME_HRM;
			device_info->version = 1;
			*value = device_info;
			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_RANGE:
			device_ranges = sns_ddf_memhandler_malloc(memhandler,
				sizeof(*device_ranges));
			if (device_ranges == NULL) 
				return SNS_DDF_ENOMEM;

			device_ranges->min = FX_FLTTOFIX_Q16(0);
			device_ranges->max = FX_FLTTOFIX_Q16(SNS_DDF_SENSOR_LAST);  
			*value = device_ranges;
			*num_elems = 1;       
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_RESOLUTION:
			resolution = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution));
			if (resolution == NULL) 
				return SNS_DDF_ENOMEM;

			*resolution = FX_FLTTOFIX_Q16(1); 
			*value = resolution;

			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_RESOLUTION_ADC:
			resolution_adc = sns_ddf_memhandler_malloc(memhandler,
				sizeof(*resolution_adc));
			if (resolution_adc == NULL)
				return SNS_DDF_ENOMEM;

			*value = resolution_adc;
			*num_elems = 1;
			resolution_adc->bit_len = 14;     
			resolution_adc->max_freq = 1000;  

			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_POWER_INFO:
			power = sns_ddf_memhandler_malloc(memhandler, sizeof(*power));
			if (power == NULL) 
				return SNS_DDF_ENOMEM;

			power->active_current = 338;     
			power->lowpower_current = 3;    
			*value = power;
			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_POWER_STATE:
			power_state = sns_ddf_memhandler_malloc(memhandler,
				sizeof(*power_state));
			if (power_state == NULL)
				return SNS_DDF_ENOMEM;

			*power_state = adpddrv_handler->powerstate;
			*value = power_state;

			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_ODR:
			odr = sns_ddf_memhandler_malloc(memhandler,	sizeof(*odr));
			if (odr == NULL)
			{
				ADPDDrv_MSG_0(HIGH,"ODR memory error");
				return SNS_DDF_ENOMEM;
			}

			*odr = adpddrv_handler->hrm_odr;		
			*value = odr;
			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		case SNS_DDF_ATTRIB_REGISTRY_GROUP:

			*num_elems = 1;
			return SNS_DDF_SUCCESS;
			  
		case SNS_DDF_ATTRIB_DELAYS:
		  
			*num_elems = 1;
			return SNS_DDF_SUCCESS;

		default:
			return SNS_DDF_EINVALID_ATTR;
	}
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_set_pwr_mode used to set power mode either
* SNS_DDF_POWERSTATE_LOWPOWER or SNS_DDF_POWERSTATE_ACTIVE
*
* @param[in] adpddrv_handler    Handle to driver instance.
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_set_pwr_mode(
	sns_dd_hrm_state_t* adpddrv_handler,
	int power_mode)
{
#ifndef USE_SAMPLE_IRQ
	uint16_t reg_val;	
#endif

	sns_ddf_status_e ret = SNS_DDF_SUCCESS;

	ADPDDrv_MSG_1(HIGH,"sns_dd_hrm_adpddrv_set_pwr_mode - mode=%d", power_mode);

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_FIFO_CLK, FIFO_CLK_EN);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	if (power_mode == SNS_DDF_POWERSTATE_LOWPOWER)
	{

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_PAUSE_MODE);
		if(ret != SNS_DDF_SUCCESS)
			return ret;

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_FIFO_CLK, FIFO_CLK_EN);
		if(ret != SNS_DDF_SUCCESS)
			return ret;

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_STATUS, (FIFO_CLR|IRQ_CLR_ALL));
		if(ret != SNS_DDF_SUCCESS)
			return ret;

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_IDLE_MODE);
		if(ret != SNS_DDF_SUCCESS)
			return ret;
		
		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_FIFO_CLK, FIFO_CLK_DIS);
		if(ret != SNS_DDF_SUCCESS)
			return ret;
		adpddrv_handler->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

	}
	else if (power_mode == SNS_DDF_POWERSTATE_ACTIVE)
	{
#ifdef USE_SAMPLE_IRQ
		if (adpddrv_handler->slotmode == ADPDDrv_SLOT_A)
		{
			ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_MASK, IRQ_RAW_A_EN);
			if(ret != SNS_DDF_SUCCESS)
				return ret;
		}
		else if (adpddrv_handler->slotmode == ADPDDrv_SLOT_B)
		{
			ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_MASK, IRQ_RAW_B_EN);
			if(ret != SNS_DDF_SUCCESS)
				return ret;	
		}
		else
		{
			ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_MASK, IRQ_RAW_AB_EN);
			if(ret != SNS_DDF_SUCCESS)
				return ret;	
		}
#else

		reg_val = (adpddrv_handler->datasetsize<<7)|1;
		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_I2CS_CTL_MATCH, reg_val);
		if(ret != SNS_DDF_SUCCESS)
			return ret;	
	
#endif

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_PAUSE_MODE);
		if(ret != SNS_DDF_SUCCESS)
			return ret;	
		
		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_FIFO_CLK, FIFO_CLK_EN);
		if(ret != SNS_DDF_SUCCESS)
			return ret;	

		ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_RUN_MODE);
		if(ret != SNS_DDF_SUCCESS)
			return ret;
		
		adpddrv_handler->powerstate = SNS_DDF_POWERSTATE_ACTIVE;
			
	}
	else
	{
		return SNS_DDF_EINVALID_PARAM; 
	}
	
#ifdef POWER_SAVING
	// set clock OFF
	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_FIFO_CLK, FIFO_CLK_DIS);
	if(ret != SNS_DDF_SUCCESS)
		return ret;	
#endif

	return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
* @brief adpddrv_enable_interrupt used to enable interrupt
*
* @param[in] handle		Generic handle given to the driver.
* @param[in] sensor		Sensor types
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e adpddrv_enable_interrupt(
	sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor)
{
	sns_dd_hrm_state_t     *adpddrv_handler = (sns_dd_hrm_state_t *)handle;
	sns_ddf_status_e       ret = SNS_DDF_SUCCESS;
	uint16_t                reg_val = 0;
	
	ADPDDrv_MSG_3(HIGH,"adpddrv_enable_interrupt - sensor=%d, gpio=%d dri_enabled=%d", sensor, adpddrv_handler->interrupt_gpio, adpddrv_handler->dri_enable);
	
	// set REG_INT_MASK to enable only FIFO interrupt
	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_MASK, IRQ_CLR_ALL);
	if(ret != SNS_DDF_SUCCESS)
		return ret;	

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_PAUSE_MODE);
	if(ret != SNS_DDF_SUCCESS)
		return ret;	
	
	// Enable interrupt in the device - Set bit 2 in REG_PAD_IO_CTRL, Set interrupt polarity - active low 0x0001
	ret = sns_dd_hrm_adpddrv_reg_read(adpddrv_handler, REG_PAD_IO_CTRL, &reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;	
	
	reg_val |= 0x00 << 8;
	reg_val |= 0x04;   //Enable interrupt
	reg_val |= 0x01;   //Set interrupt polarity

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_PAD_IO_CTRL, reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;
	
	// Clear pending interrupts - REG_INT_STATUS = 0x80FF
	reg_val = 0x81FF;
	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_INT_STATUS, reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_OP_MODE, OP_RUN_MODE);
	if(ret != SNS_DDF_SUCCESS)
		return ret;
	
	// Register the signal
	return sns_ddf_signal_register(adpddrv_handler->interrupt_gpio,
								   handle,
#ifdef USE_DRAGONBOARD_BUILD
								   &sns_dd_vendor_if_2,
#else  /* USE_DRAGONBOARD_BUILD */
								   &sns_dd_ppg_adpd142_if,
#endif  /* USE_DRAGONBOARD_BUILD */
								   SNS_DDF_SIGNAL_IRQ_FALLING); 
}

/*===========================================================================*/
/*
* @brief adpddrv_disable_interrupt used to disable interrupt
*
* @param[in] handle		Generic handle given to the driver.
* @param[in] sensor		Sensor types
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e adpddrv_disable_interrupt(   
	sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor)
{
	sns_dd_hrm_state_t   *adpddrv_handler = (sns_dd_hrm_state_t *)handle;
	sns_ddf_status_e     ret = SNS_DDF_SUCCESS;
	uint16_t              reg_val;

	// disable interrupt in the device
	reg_val = 0;
	ret = sns_dd_hrm_adpddrv_reg_read(adpddrv_handler, REG_PAD_IO_CTRL, &reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	reg_val &= ~(0x04);

	ret = sns_dd_hrm_adpddrv_reg_write(adpddrv_handler, REG_PAD_IO_CTRL, reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	// De register signal
	return sns_ddf_signal_deregister(adpddrv_handler->interrupt_gpio);
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_enable_sched_data used to enable/disable the FIFO 
*  interrupt
*
* @param[in] handle		Generic handle given to the driver.
* @param[in] sensor		Sensor types
* @param[in] enable		Decision
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_hrm_adpddrv_enable_sched_data(
	sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor,
	bool enable)
{
	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_enable_sched_data");

	if (enable)
	{
		return adpddrv_enable_interrupt(handle, sensor);
	}
	else
	{
		return adpddrv_disable_interrupt(handle, sensor);
	}

	return SNS_DDF_EINVALID_PARAM;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_probe used to probe the device if present 
*
* @param[in]  device_info	Device information provided by .conf file.
* @param[in]  memhandler	Memory handler used to dynamically allocate 
*                          	output parameters, if applicable.
* @param[out] num_sensors	Number sensor handled by this driver.
* @param[out] sensors		Sensor type that handled by this driver.
* 
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_probe
(
	sns_ddf_device_access_s* device_info,
	sns_ddf_memhandler_s* memhandler,
	uint32_t* num_sensors,
	sns_ddf_sensor_e** sensors 
)
{
	sns_ddf_handle_t    port_handle;
	sns_ddf_status_e    ret = SNS_DDF_SUCCESS;
	uint16_t            reg_val; 

	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_probe");

	/* Basic sanity check*/
	if (device_info == NULL || sensors == NULL || num_sensors == NULL)
		return SNS_DDF_EINVALID_PARAM;

	*num_sensors = 0;
	*sensors = NULL; 

	/* open I2C port */
	ret = sns_ddf_open_port(&port_handle, &device_info->port_config);

	if (ret != SNS_DDF_SUCCESS)
		return ret;

	/* Read Chip ID register */
	reg_val = 0;
	ret = sns_dd_hrm_adpddrv_reg_read(port_handle, REG_CHIP_ID, &reg_val);
	if(ret != SNS_DDF_SUCCESS)
		return ret;

	switch(reg_val)
	{
	case ADPD_CHIPID(0):
	case ADPD_CHIPID(1):
		ADPDDrv_MSG_0(HIGH,"Chip Id read - success");
		break;
	default:
		ADPDDrv_MSG_0(ERROR,"Chip Id read - error");
		sns_ddf_close_port(port_handle);		
		return SNS_DDF_SUCCESS;
	}

	/* ADPDDrv device found */
	*num_sensors = 1;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
	if( *sensors != NULL ) {
		(*sensors)[0] = sns_dd_hrm_adpddrv_sensor_types[0];
		ret = SNS_DDF_SUCCESS;
	} else {
		ret = SNS_DDF_ENOMEM;
	}
	
	sns_ddf_close_port(port_handle);
	return ret;	
}

/*===========================================================================*/
/*
 * @brief sns_dd_hrm_adpddrv_run_test called to verify the basic functionality
 *
 * @detail
 *   Only SNS_DDF_TEST_CONNECTIVITY implemented. Performs a read ID Register
 *   to verify the connectivity of the device.
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in] sensor        Sensor for which testing is requested. Can be
 *                          Humidity, Temperature or All
 * @param[in] test          Defines what test has to be done. Only
 *                          SNS_DDF_TEST_CONNECTIVITY supported.
 * @param[out] err          Output Error Code enumerated
 *                          0x00000000: no error
 *                          0x00000001: error while writing to device
 *                          0x00000002: error while reading from device
 *                          0x00000003: ID register content mismatch
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
 /*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_run_test(
	sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor,
	sns_ddf_test_e test,
	uint32_t* err)
{
	sns_dd_hrm_state_t   *adpddrv_handler = NULL;
	sns_ddf_status_e     ret = SNS_DDF_SUCCESS;
	uint16_t             reg_val;
	

	if (!err || !handle ||
		(sensor != SNS_DDF_SENSOR_HEART_RATE_RAW &&
		 sensor != SNS_DDF_SENSOR__ALL))
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	adpddrv_handler = (sns_dd_hrm_state_t *)handle;
	*err = 0;

	switch(test)
	{
		case SNS_DDF_TEST_SELF:
			/* Read Chip ID register */
			reg_val = 0;
			ret = sns_dd_hrm_adpddrv_reg_read(adpddrv_handler, REG_CHIP_ID, &reg_val);
			if(ret != SNS_DDF_SUCCESS)
			{
				ADPDDrv_MSG_0(ERROR,"Chip Id read - error");
				*err = 0x00000002;
			}
			else if (reg_val != ADPD_CHIPID(0) && reg_val != ADPD_CHIPID(1))
			{
				*err = 0x00000003;
				ADPDDrv_MSG_1(HIGH,"Chip Id read %d - is Invalid", reg_val);
				ret = SNS_DDF_EFAIL;
			}
			else {
				ADPDDrv_MSG_0(HIGH,"Chip Id read - Success");
			}
			break;
		default:
			ret = SNS_DDF_EINVALID_TEST;
	}

	return ret;
}



#ifdef FEATURE_APDP_INT_PIN_CONFIG

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
/*===========================================================================
===========================================================================*/
#ifdef USE_DRAGONBOARD_BUILD
sns_ddf_driver_if_s sns_dd_vendor_if_2 =
#else  /* USE_DRAGONBOARD_BUILD */
sns_ddf_driver_if_s sns_dd_ppg_adpd142_if =
#endif /* USE_DRAGONBOARD_BUILD */
{
	.init = &sns_dd_hrm_adpddrv_init,
	.get_data = &sns_dd_hrm_adpddrv_get_data,
	.set_attrib = &sns_dd_hrm_adpddrv_set_attrib, 
	.get_attrib = &sns_dd_hrm_adpddrv_get_attrib,
	.handle_timer = &sns_dd_hrm_adpddrv_handle_timer, 
	.handle_irq = &sns_dd_hrm_adpddrv_handle_irq,
	.reset = &sns_dd_hrm_adpddrv_reset,
	.run_test = &sns_dd_hrm_adpddrv_run_test, /* self test */
	.enable_sched_data = &sns_dd_hrm_adpddrv_enable_sched_data,
	.probe = &sns_dd_hrm_adpddrv_probe,
};
