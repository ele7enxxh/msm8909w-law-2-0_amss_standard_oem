/*=============================================================================
						S P M I		D R I V E R   

GENERAL DESCRIPTION     
  SPMI driver for debug image. Implementation is polling based.
  
  

	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/spmi/src/msm8909/sysdbg_spmi.c#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
03/10/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/
#include "sysdbg_spmi.h"
#include "string.h"
// TZ spmi includes
#include "SpmiCfg.h"
#include "SpmiBus.h"
#include "tzbsp_log.h"
#include "DALSys.h"

/*
 * pm_periph_intr_owner
 *
 * A type to choose the interrupt owner for the peripheral.
 */
typedef enum {
  PM_APPS_HLOS_OWNER = 0,
  PM_APPS_TZ_OWNER,
  PM_MSS_OWNER,
  PM_LPASS_OWNER,
  PM_RPM_OWNER,
  PM_WCONNECT_OWNER
} pm_periph_intr_owner;

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

// SPMI channel details
SpmiCfg_ChannelCfg  pm_spmi_periph_cfg_pm8909[] =
{

    {0xF, 0x00, 116, PM_APPS_TZ_OWNER},     /* GID            */
    {0x2, 0x06, 55, PM_RPM_OWNER},         /* SPMI_OPTIONS   */
    {0x1, 0xF3, 24,  PM_APPS_HLOS_OWNER},   /* CDC_NCP_FREQ    */
    {0x1, 0xF2, 23,  PM_APPS_HLOS_OWNER},   /* CDC_BOOST_FREQ    */
    {0x1, 0xF1, 22,  PM_APPS_HLOS_OWNER},   /* CDC_A    */
    {0x1, 0xF0, 21,  PM_APPS_HLOS_OWNER},   /* CDC_D    */
    {0x1, 0xC0, 20,  PM_APPS_HLOS_OWNER},   /* VIB1     */
    {0x1, 0xBC, 19,  PM_APPS_HLOS_OWNER},   /* PWM_3D         */
    {0x1, 0x53, 54, PM_RPM_OWNER},         /* LDO20          */
    {0x1, 0x52, 53, PM_RPM_OWNER},         /* LDO19          */
    {0x1, 0x51, 52, PM_RPM_OWNER},         /* LDO18          */
    {0x1, 0x50, 58, PM_RPM_OWNER},         /* LDO17          */
    {0x1, 0x4F, 86, PM_RPM_OWNER},         /* LDO16          */
    {0x1, 0x4E, 85, PM_RPM_OWNER},         /* LDO15          */
    {0x1, 0x4D, 84, PM_RPM_OWNER},         /* LDO14          */
    {0x1, 0x4C, 57, PM_RPM_OWNER},         /* LDO13          */
    {0x1, 0x4B, 51, PM_RPM_OWNER},         /* LDO12          */
    {0x1, 0x4A, 50, PM_RPM_OWNER},         /* LDO11          */
    {0x1, 0x49, 49, PM_RPM_OWNER},         /* LDO10          */
    {0x1, 0x48, 48, PM_RPM_OWNER},         /* LDO9           */
    {0x1, 0x47, 47, PM_RPM_OWNER},         /* LDO8           */
    {0x1, 0x46, 46, PM_RPM_OWNER},         /* LDO7           */
    {0x1, 0x45, 56, PM_RPM_OWNER},         /* LDO6           */
    {0x1, 0x44, 45, PM_RPM_OWNER},         /* LDO5           */
    {0x1, 0x43, 44, PM_RPM_OWNER},         /* LDO4           */
    {0x1, 0x42, 43, PM_RPM_OWNER},         /* LDO3           */
    {0x1, 0x41, 42, PM_RPM_OWNER},         /* LDO2           */
    {0x1, 0x40, 41, PM_RPM_OWNER},         /* LDO1           */
    {0x1, 0x19, 69 , PM_APPS_HLOS_OWNER},         /* S2_FREQ        */
    {0x1, 0x18, 70 , PM_APPS_HLOS_OWNER},         /* S2_PS          */
    {0x1, 0x17, 68 , PM_APPS_HLOS_OWNER},         /* S2_CTRL        */
    {0x1, 0x16, 66,  PM_RPM_OWNER},         /* S1_FREQ        */
    {0x1, 0x15, 65, PM_RPM_OWNER},         /* S1_PS          */
    {0x1, 0x14, 67, PM_RPM_OWNER},         /* S1_CTRL        */
    {0x1, 0x10, 40,  PM_RPM_OWNER},   /* BCLK_GEN_MAIN  */
    {0x0, 0xFE, 115, PM_RPM_OWNER},         /* TRIM           */
    {0x0, 0xC3, 98 , PM_APPS_HLOS_OWNER},   /* GPIO4          */
    {0x0, 0xC2, 97 , PM_APPS_HLOS_OWNER},   /* GPIO3          */
    {0x0, 0xC1, 96,  PM_WCONNECT_OWNER},   /* GPIO2          */
    {0x0, 0xC0, 95, PM_MSS_OWNER},         /* GPIO1          */
    {0x0, 0xA3, 94 , PM_APPS_HLOS_OWNER},   /* MPP4           */
    {0x0, 0xA2, 93 , PM_APPS_HLOS_OWNER},   /* MPP3           */
    {0x0, 0xA1, 92 , PM_APPS_HLOS_OWNER},   /* MPP2           */
    {0x0, 0xA0, 91 , PM_APPS_HLOS_OWNER},   /* MPP1           */
    {0x0, 0x77, 83 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT6    */
    {0x0, 0x76, 82 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT5    */
    {0x0, 0x75, 81 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT4    */
    {0x0, 0x74, 80, PM_MSS_OWNER},         /* PBS_CLIENT3    */
    {0x0, 0x73, 39 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT2    */
    {0x0, 0x72, 38 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT1    */
    {0x0, 0x71, 37 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT0    */
    {0x0, 0x70, 36 , PM_RPM_OWNER},         /* PBS_CORE       */
    {0x0, 0x62, 15 , PM_APPS_HLOS_OWNER},   /* RTC_TIMER      */
    {0x0, 0x61, 14 , PM_APPS_HLOS_OWNER},   /* RTC_ALARM      */
    {0x0, 0x60, 79, PM_MSS_OWNER},         /* RTC_WR         */
    {0x0, 0x5A, 35 , PM_RPM_OWNER},   /* SLEEP_CLK1     */
    {0x0, 0x59, 34 , PM_RPM_OWNER},   /* CLK_DIST       */
    {0x0, 0x55, 75 , PM_RPM_OWNER},   /* RF_CLK2        */
    {0x0, 0x54, 74 , PM_RPM_OWNER},   /* RF_CLK1        */
    {0x0, 0x52, 33 , PM_RPM_OWNER},   /* BB_CLK2        */
    {0x0, 0x51, 32,  PM_RPM_OWNER},   /* BB_CLK1        */
    {0x0, 0x50, 78, PM_MSS_OWNER},   /* XO             */
    {0x0, 0x40, 13 , PM_APPS_HLOS_OWNER},   /* BMS_VM       */
    {0x0, 0x35, 12 , PM_APPS_HLOS_OWNER},   /* VADC4_LC_VBAT        */
    {0x0, 0x34, 11 , PM_APPS_HLOS_OWNER},   /* VADC2_BTM      */
    {0x0, 0x33, 10, PM_MSS_OWNER},         /* VADC3_LC_VBMS      */
    {0x0, 0x32, 77,  PM_RPM_OWNER},         /* VADC3_MDM      */
    {0x0, 0x31, 9 , PM_APPS_HLOS_OWNER},   /* VADC1_USR      */
    {0x0, 0x2C, 31,  PM_RPM_OWNER},   /* MBG1           */
    {0x0, 0x28, 8  , PM_APPS_HLOS_OWNER},   /* COIN           */
    {0x0, 0x24, 7  , PM_APPS_HLOS_OWNER},   /* TEMP_ALARM     */
    {0x0, 0x1C, 76, PM_MSS_OWNER},         /* BUA_EXT_CHARGER*/
    {0x0, 0x16, 6  , PM_APPS_HLOS_OWNER},   /* LBC_MISC      */
    {0x0, 0x13, 5  , PM_APPS_HLOS_OWNER},   /* LBC_USB      */
    {0x0, 0x12, 4  , PM_APPS_HLOS_OWNER},   /* LBC_BAT_IF      */
    {0x0, 0x10, 3  , PM_APPS_HLOS_OWNER},   /* LBC_CHGR      */
    {0x0, 0x0A, 2  , PM_APPS_HLOS_OWNER},   /* VREFLPDDR      */
    {0x0, 0x09, 0,   PM_RPM_OWNER},   /* MISC           */
    {0x0, 0x08, 1  , PM_APPS_HLOS_OWNER},   /* PON            */
    {0x0, 0x06, 114,  PM_RPM_OWNER},         /* SPMI           */
    {0x0, 0x05, 113, PM_RPM_OWNER},         /* INT            */
    {0x0, 0x04, 112, PM_RPM_OWNER},         /* BUS            */
    {0x0, 0x01, 111, PM_RPM_OWNER},         /* REVID          */
};

/* Number of peripherals. */
uint32 pm_num_periph_pm8909 = sizeof(pm_spmi_periph_cfg_pm8909) / sizeof(SpmiCfg_ChannelCfg);

extern int tzbsp_chipset_init(uint32 debug);

/*===========================================================================
**  Function : SPMI_INIT
** ==========================================================================
*/
/*!
* 
* @brief
*	SPMI driver init
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*
*
*/
boolean spmi_init(void)
{
   static boolean init = FALSE;
   SpmiCfg_ChannelCfg* periph_cfg_ptr = pm_spmi_periph_cfg_pm8909;
   if (!init)
   { 
        /* Initialize DAL as SPMI is using dal framework in init sequence */
		DALSYS_InitMod(NULL);
	    if (SpmiCfg_Init(TRUE) == SPMI_SUCCESS)
		{
			/* tz api which configures spmi channels, as it has been disabled in TZ for 8909
			doing it here */
			if (SpmiCfg_ConfigureChannels(periph_cfg_ptr, pm_num_periph_pm8909) != SPMI_SUCCESS)
			{
				TZBSP_LOG_ERR("SPMI Channel Configuration failed.", 0, 0, 0);
			}
			else
			{
			    init = TRUE;
				TZBSP_LOG_ERR("SPMI Channel Configuration Succeeded.", 0, 0, 0);
			}
		}
		else
		{
		    TZBSP_LOG_ERR("SPMI Initialization Failed.", 0, 0, 0);
		}
    }
	return init;
}

/*===========================================================================
**  Function : SPMI_READDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper to Spmi Read data function
*
* @param[in] 
*	pointer to spmidatatype which has been filled with which peripheral register to read
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*
*
*/
boolean spmi_readdata(spmidatatype *data)
{
	
	SpmiBus_ResultType status;
	uint32 TotalBytesRead = 0;
    status = SpmiBus_ReadLong(data->slaveid,
                    SPMI_BUS_ACCESS_PRIORITY_HIGH, 
                    data->address,
                    (uint8 *)data->data,
                    data->datalen,
		    &TotalBytesRead);
					
	if(status == SPMI_BUS_SUCCESS)
		// Return sucess
		return TRUE;
	else
		return FALSE;
}
/*===========================================================================
**  Function : SPMI_WRITEDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper to Spmi Write data function
*
* @param[in] 
*	pointer to spmidatatype which has been filled with which peripheral register to read
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*
*
*/
boolean spmi_writedata(spmidatatype *data)
{

  SpmiBus_ResultType status;
  status = SpmiBus_WriteLong(data->slaveid,
                    SPMI_BUS_ACCESS_PRIORITY_HIGH, 
                    data->address,
                    (uint8*)data->data,
                    data->datalen);
					
	if(status == SPMI_BUS_SUCCESS)
		// Return sucess
		return TRUE;
	else
		return FALSE;
}




	
	











