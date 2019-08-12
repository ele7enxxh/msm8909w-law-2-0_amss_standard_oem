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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/spmi/src/mdm9x07/sysdbg_spmi.c#1 $

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
SpmiCfg_ChannelCfg  pm_spmi_periph_cfg_pmd9607[] =
{

    {0xF, 0x00, 95,  PM_APPS_TZ_OWNER},     /* GID            */
    {0xE, 0x08, 11,  PM_APPS_HLOS_OWNER},   /* PON            */
    {0xE, 0x06, 69,  PM_RPM_OWNER},         /* SPMI           */
    {0xE, 0x05, 68,  PM_RPM_OWNER},         /* INT            */
    {0x2, 0x06, 67,  PM_RPM_OWNER},         /* SPMI_OPTIONS   */
    {0x1, 0x4F, 66,  PM_RPM_OWNER},         /* LDO16          */
    {0x1, 0x4E, 65,  PM_RPM_OWNER},         /* LDO15          */
    {0x1, 0x4D, 64,  PM_RPM_OWNER},         /* LDO14          */
    {0x1, 0x4C, 63,  PM_RPM_OWNER},         /* LDO13          */
    {0x1, 0x4B, 62,  PM_RPM_OWNER},         /* LDO12          */
    {0x1, 0x4A, 61,  PM_RPM_OWNER},         /* LDO11          */
    {0x1, 0x49, 60,  PM_RPM_OWNER},         /* LDO10          */
    {0x1, 0x48, 59,  PM_RPM_OWNER},         /* LDO9           */
    {0x1, 0x47, 58,  PM_RPM_OWNER},         /* LDO8           */
    {0x1, 0x46, 57,  PM_RPM_OWNER},         /* LDO7           */
    {0x1, 0x45, 56,  PM_RPM_OWNER},         /* LDO6           */
    {0x1, 0x44, 55,  PM_RPM_OWNER},         /* LDO5           */
    {0x1, 0x43, 54,  PM_RPM_OWNER},         /* LDO4           */
    {0x1, 0x42, 53,  PM_RPM_OWNER},         /* LDO3           */
    {0x1, 0x41, 52,  PM_RPM_OWNER},         /* LDO2           */
    {0x1, 0x40, 51,  PM_RPM_OWNER},         /* LDO1           */
    {0x1, 0x1F, 50,  PM_RPM_OWNER},         /* S4_FREQ        */
    {0x1, 0x1E, 49,  PM_RPM_OWNER},         /* S4_PS          */
    {0x1, 0x1D, 48,  PM_RPM_OWNER},         /* S4_CTRL        */
    {0x1, 0x1C, 47,  PM_RPM_OWNER},         /* S3_FREQ        */
    {0x1, 0x1B, 46,  PM_RPM_OWNER},         /* S3_PS          */
    {0x1, 0x1A, 45,  PM_RPM_OWNER},         /* S3_CTRL        */
    {0x1, 0x19, 44,  PM_RPM_OWNER},         /* S2_FREQ        */
    {0x1, 0x18, 43,  PM_RPM_OWNER},         /* S2_PS          */
    {0x1, 0x17, 42,  PM_RPM_OWNER},         /* S2_CTRL        */
    {0x1, 0x16, 41,  PM_RPM_OWNER},         /* S1_FREQ        */
    {0x1, 0x15, 40,  PM_RPM_OWNER},         /* S1_PS          */
    {0x1, 0x14, 39,  PM_RPM_OWNER},         /* S1_CTRL        */
    {0x1, 0x10, 38,  PM_RPM_OWNER},         /* BCLK_GEN_MAIN  */
    {0x0, 0xFE, 100, PM_RPM_OWNER},         /* TRIM           */
    {0x0, 0xC5, 92 , PM_APPS_HLOS_OWNER},   /* GPIO6          */
    {0x0, 0xC4, 91 , PM_MSS_OWNER},         /* GPIO5          */
    {0x0, 0xC3, 90 , PM_APPS_HLOS_OWNER},   /* GPIO4          */
    {0x0, 0xC2, 89 , PM_APPS_HLOS_OWNER},   /* GPIO3          */
    {0x0, 0xC1, 88,  PM_APPS_HLOS_OWNER},   /* GPIO2          */
    {0x0, 0xC0, 87,  PM_APPS_HLOS_OWNER},   /* GPIO1          */
    {0x0, 0xA5, 86 , PM_APPS_HLOS_OWNER},   /* MPP6           */
    {0x0, 0xA4, 85 , PM_MSS_OWNER},         /* MPP5           */
    {0x0, 0xA3, 84 , PM_MSS_OWNER},         /* MPP4           */
    {0x0, 0xA2, 83 , PM_APPS_HLOS_OWNER},   /* MPP3           */
    {0x0, 0xA1, 82 , PM_APPS_HLOS_OWNER},   /* MPP2           */
    {0x0, 0xA0, 81 , PM_APPS_HLOS_OWNER},   /* MPP1           */
    {0x0, 0x80, 37 , PM_RPM_OWNER},         /* PBS_CLIENT15   */
    {0x0, 0x7F, 36 , PM_RPM_OWNER},         /* PBS_CLIENT14   */
    {0x0, 0x7E, 35 , PM_RPM_OWNER},         /* PBS_CLIENT13   */
    {0x0, 0x7D, 34 , PM_RPM_OWNER},         /* PBS_CLIENT12   */
    {0x0, 0x7C, 33 , PM_RPM_OWNER},         /* PBS_CLIENT11   */
    {0x0, 0x7B, 32,  PM_RPM_OWNER},         /* PBS_CLIENT10   */
    {0x0, 0x7A, 31 , PM_RPM_OWNER},         /* PBS_CLIENT9    */
    {0x0, 0x79, 10 , PM_APPS_HLOS_OWNER},   /* PBS_CLIENT8    */
    {0x0, 0x78, 30 , PM_RPM_OWNER},         /* PBS_CLIENT7    */
    {0x0, 0x77, 77 , PM_MSS_OWNER},         /* PBS_CLIENT6    */
    {0x0, 0x76, 29 , PM_RPM_OWNER},         /* PBS_CLIENT5    */
    {0x0, 0x75, 28 , PM_RPM_OWNER},         /* PBS_CLIENT4    */
    {0x0, 0x74, 27,  PM_RPM_OWNER},         /* PBS_CLIENT3    */
    {0x0, 0x73, 9 ,  PM_APPS_HLOS_OWNER},   /* PBS_CLIENT2    */
    {0x0, 0x72, 8 ,  PM_APPS_HLOS_OWNER},   /* PBS_CLIENT1    */
    {0x0, 0x71, 7 ,  PM_APPS_HLOS_OWNER},   /* PBS_CLIENT0    */
    {0x0, 0x70, 26 , PM_RPM_OWNER},         /* PBS_CORE       */
    {0x0, 0x62, 25 , PM_APPS_HLOS_OWNER},   /* RTC_TIMER      */
    {0x0, 0x61, 6 ,  PM_APPS_HLOS_OWNER},   /* RTC_ALARM      */
    {0x0, 0x60, 76,  PM_MSS_OWNER},         /* RTC_WR         */
    {0x0, 0x5D, 24 , PM_RPM_OWNER},         /* DIV_CLK3       */
    {0x0, 0x5C, 23,  PM_RPM_OWNER},         /* DIV_CLK        */
    {0x0, 0x5B, 22 , PM_RPM_OWNER},         /* DIV_CLK1       */
    {0x0, 0x5A, 21 , PM_RPM_OWNER},         /* SLEEP_CLK1     */
    {0x0, 0x59, 20 , PM_RPM_OWNER},         /* CLK_DIST       */
    {0x0, 0x57, 19 , PM_RPM_OWNER},         /* DIFF_CLK1      */
    {0x0, 0x56, 18 , PM_RPM_OWNER},         /* RF_CLK3        */
    {0x0, 0x55, 75 , PM_RPM_OWNER},         /* RF_CLK2        */
    {0x0, 0x54, 74 , PM_RPM_OWNER},         /* RF_CLK1        */
    {0x0, 0x52, 17 , PM_RPM_OWNER},         /* BB_CLK2        */
    {0x0, 0x51, 16,  PM_RPM_OWNER},         /* BB_CLK1        */
    {0x0, 0x50, 73,  PM_RPM_OWNER},         /* XO             */
    {0x0, 0x34, 5,   PM_APPS_HLOS_OWNER},   /* VADC2_BTM      */
    {0x0, 0x32, 72,  PM_MSS_OWNER},         /* VADC3_MDM      */
    {0x0, 0x31, 4,   PM_APPS_HLOS_OWNER},   /* VADC1_USR      */
    {0x0, 0x2C, 15,  PM_RPM_OWNER},         /* MBG1           */
    {0x0, 0x24, 3,   PM_APPS_HLOS_OWNER},   /* TEMP_ALARM     */
    {0x0, 0x1C, 71,  PM_MSS_OWNER},         /* BUA_NO_CHARGER */
    {0x0, 0x0A, 2,   PM_APPS_HLOS_OWNER},   /* VREFLPDDR      */
    {0x0, 0x09, 1,   PM_RPM_OWNER},         /* MISC           */
    {0x0, 0x08, 0,   PM_APPS_HLOS_OWNER},   /* PON            */
    {0x0, 0x06, 99,  PM_RPM_OWNER},         /* SPMI           */
    {0x0, 0x05, 98,  PM_RPM_OWNER},         /* INT            */
    {0x0, 0x04, 97,  PM_RPM_OWNER},         /* BUS            */
    {0x0, 0x01, 96,  PM_RPM_OWNER},         /* REVID          */
};

/* Number of peripherals. */
uint32 pm_num_periph_pmd9607 = sizeof(pm_spmi_periph_cfg_pmd9607) / sizeof(SpmiCfg_ChannelCfg);

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
   SpmiCfg_ChannelCfg* periph_cfg_ptr = pm_spmi_periph_cfg_pmd9607;
   if (!init)
   { 
        /* Initialize DAL as SPMI is using dal framework in init sequence */
		DALSYS_InitMod(NULL);
	    if (SpmiCfg_Init(TRUE) == SPMI_SUCCESS)
		{
			/* tz api which configures spmi channels, as it has been disabled in TZ for 8909
			doing it here */
			if (SpmiCfg_ConfigureChannels(periph_cfg_ptr, pm_num_periph_pmd9607) != SPMI_SUCCESS)
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




	
	











