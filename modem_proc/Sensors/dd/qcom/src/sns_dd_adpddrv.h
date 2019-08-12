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

/**
@file adpddrv.h
@brief ADPDDrv  - Low level driver  Header 'H' File
*/
#ifndef __SNS_DD_HRM_ADPDDrv_H
#define __SNS_DD_HRM_ADPDDrv_H

//#include "qurt_elite_diag.h"


// a flag to void target specific GPIO conflict
//#define FEATURE_APDP_INT_PIN_CONFIG

// a flag to identify sensor proximity to skin
// #define PROCESS_CONTACT


 typedef enum {
    ADPDDrv_MODE_IDLE = 0,
    ADPDDrv_MODE_PAUSE,
    ADPDDrv_MODE_PWR_OFF,
    ADPDDrv_MODE_SAMPLE
} ADPDDrv_OPERATION_MODE;

typedef enum {
    ADPDDrv_SLOT_A = 0,
    ADPDDrv_SLOT_B,
    ADPDDrv_SLOT_AB
} ADPDDrv_OPERATION_SLOT;


/* Exported constants ------------------------------------------------------ */

/* Chip id */
#define ADPD_CHIPID_0			0x0216		
#define ADPD_CHIPID_1			0x0316		
#define ADPD_CHIPID(id)			ADPD_CHIPID_##id		
                                
/*  ADPD REGISTERS */           
#define REG_INT_STATUS          0x00		
#define REG_INT_MASK            0x01		
#define REG_PAD_IO_CTRL         0x02		
#define REG_I2CS_CTL_MATCH      0x06		
#define REG_CHIP_ID             0x08		
#define REG_OP_MODE             0x10		
#define REG_OP_MODE_CFG         0x11		
#define REG_FIFO_CLK			0x5f		
#define REG_DATA_BUFFER			0x60		
                                
#define MAX_CONFIG_REG_CNT		60		
                                
/*  REGISTER Values */          
#define OP_IDLE_MODE            0		
#define OP_PAUSE_MODE           1		
#define OP_RUN_MODE             2		
								
#define FIFO_CLK_EN             0x0001		
#define FIFO_CLK_DIS            0x0000		
								
// IRQ related		                    
#define FIFO_CLR                0x8000		
#define IRQ_CLR_ALL             0x00FF		
#define IRQ_CLR_FIFO            0x8000		
#define IRQ_MASK_RAW            0x0060		
#define IRQ_RAW_AB_EN           0xC19F		
#define IRQ_RAW_A_EN            0xC1DF		
#define IRQ_RAW_B_EN            0xC1BF		
								
// Slot related		                    
#define SLOT_A_DATA_SIZE        0x8		
#define SLOT_B_DATA_SIZE        0x8		
#define SLOT_AB_DATA_SIZE       0x10		
#define SLOT_MASK               0xF800		
#define SLOT_A_MODE             0x0011		
#define SLOT_B_MODE             0x0120		
#define SLOT_AB_MODE            0x0131		

/* 
	#define USE_DRAGONBOARD_BUILD 
	1 - Enable CONVERTED SENSOR DATA PUBLIC LogID  
	0 - Enable CONVERTED SENSOR DATA LogID 	
*/   
//#define USE_DRAGONBOARD_BUILD 1  // This define is enabled globally
  
// enable this to see driver debug messages
//#define ADPDDrv_DEBUG
// Micro image Support
//#define ADPD_UIMAGE_MODE

/* Basic definitions */
#ifdef USE_DRAGONBOARD_BUILD
sns_ddf_driver_if_s sns_dd_vendor_if_2;
#else /* USE_DRAGONBOARD_BUILD */
extern sns_ddf_driver_if_s sns_dd_ppg_adpd142_if;
#endif  /* USE_DRAGONBOARD_BUILD */

/* register address and value pair */
typedef struct {
	uint8_t       register_address;
	uint16_t      register_value;

} sns_dd_adpddrv_reg_data_t;

typedef struct {
	bool						dri_enable;
	uint16_t					interrupt_gpio;
	sns_ddf_handle_t			smgr_handle;  /* notify_data handle */
	sns_ddf_handle_t			port_handle;  /* I2C bus handle */
	sns_ddf_sensor_data_s*      s_data;
	sns_ddf_sensor_sample_s*    samples;
	uint32_t					hrm_odr;
	sns_ddf_powerstate_e		powerstate;   /* ACTIVE/LOW_POWER */
	sns_ddf_timer_s				timer_obj;
	sns_dd_adpddrv_reg_data_t	device_config[MAX_CONFIG_REG_CNT];
	uint8_t						device_config_size;
	uint8_t						slotmode;
	uint8_t						datasetsize;
} sns_dd_hrm_state_t;

static sns_ddf_sensor_e sns_dd_hrm_adpddrv_sensor_types[] =
{
#ifdef USE_DRAGONBOARD_BUILD
	SNS_DDF_SENSOR_OEM_SENSOR_01
#else  /* USE_DRAGONBOARD_BUILD */
	SNS_DDF_SENSOR_HEART_RATE_RAW
#endif /* USE_DRAGONBOARD_BUILD */
};

#ifdef ADPDDrv_DEBUG
	#if defined QDSP6
		#define DBG_MEDIUM_PRIO DBG_MED_PRIO
		#ifdef ADPD_UIMAGE_MODE
			#define ADPDDrv_MSG_0(level,msg)          UMSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg)
			#define ADPDDrv_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1)
			#define ADPDDrv_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2)
			#define ADPDDrv_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2,p3)
		#else /* ADPD_UIMAGE_MODE */
			#ifdef USE_DRAGONBOARD_BUILD
				#define ADPDDrv_MSG_0(level,msg)          MSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg)
				#define ADPDDrv_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1)
				#define ADPDDrv_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2)
				#define ADPDDrv_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2,p3)
			#else  /* USE_DRAGONBOARD_BUILD */
				#define ADPDDrv_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "ADPDDrv - " msg)
				#define ADPDDrv_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "ADPDDrv - " msg,p1)
				#define ADPDDrv_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2)
				#define ADPDDrv_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "ADPDDrv - " msg,p1,p2,p3)
			#endif  /* USE_DRAGONBOARD_BUILD */
		#endif /* ADPD_UIMAGE_MODE */
	#else /* QDSP6 */
		#define MED MEDIUM
		#include "sns_debug_str.h"
		#define ADPDDrv_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
		#define ADPDDrv_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
		#define ADPDDrv_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
		#define ADPDDrv_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
	#endif /* QDSP6 */
#else /* ADPDDrv_DEBUG */
	#define ADPDDrv_MSG_0(level,msg)
	#define ADPDDrv_MSG_1(level,msg,p1)
	#define ADPDDrv_MSG_2(level,msg,p1,p2)
	#define ADPDDrv_MSG_3(level,msg,p1,p2,p3)
#endif /* ADPDDrv_DEBUG */

#endif // __SNS_DD_HRM_ADPDDrv_H



