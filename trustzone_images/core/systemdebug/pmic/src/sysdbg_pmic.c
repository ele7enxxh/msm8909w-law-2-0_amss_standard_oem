/*=============================================================================
           			 D B I  	P M I C 	D R I V E R 

GENERAL DESCRIPTION     
		  This file contains the PMIC driver implementation for the System debug
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/pmic/src/sysdbg_pmic.c#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
01/29/14    ntonupun    Ported to Bear
06/06/12	AJCheriyan	First cut for Aragorn. First ever DBI

===========================================================================*/


/* ===========================================================================
 *						H E A D E R		 F I L E S 	
 * =========================================================================*/

#include "sysdbg_pmic_private.h"
#include "sysdbg_pmic.h"
#include "string.h"
#include "stringl/stringl.h"
#include "tzbsp_log.h"


/* ========================================================================
===========================================================================*/


/* ===========================================================================
 *					U S E F U L 	M A C R O S 
 * =========================================================================*/
#define BITSET(num, bit) ((num) & (1 << (bit)))
/* ========================================================================
===========================================================================*/

// Pointer to BSP structure 
static PMMapType *PMBsp;

// PMIC Register Map data populated externally
extern PMMapType PM_map_data;

// PMIC Target init routine
extern void PMRegulator_Init(void);
extern void PM_target_wdog_init(uint32, uint32);
extern void PM_target_wdog_disable(void);
extern void PMPSHOLD_Config(void);

/* Wrapper API to get the data for a particular peripheral from the BSP */
void pm_getmapinfo(PMPerphName perph, PMRegEntryType *mapdata)
{
	if (perph < VALID_PERIPH_COUNT)
	{
		// Find the correct entry in the bsp
		PMRegEntryType *regentry = PMBsp->pmap;
	    while (regentry->name != perph)
			regentry++;	
		
		// Copy the bsp data over to map data
		memscpy(mapdata, sizeof(PMRegEntryType), regentry, sizeof(PMRegEntryType));
	}
	else
	{
		// ERR_FATAL here. Not a fatal error but just in here to catch bugs
		TZBSP_LOG_ERR("PMIC Peripheral not valid", 0, 0, 0);
	}

}


/*===========================================================================
**  Function : PM_EnableWatchdog
** ==========================================================================
*/
/*!
* 
* @brief
*	This function enables the PMIC watchdog. Configuration is expected to be done during
*	cold boot. 
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
*	Dog will not be pet when SDI flow runs and if there is a hang, the PMIC Watchdog will help 
*	recover the device into download mode.
*
*/
void PM_EnableWatchdog(void)
{
#ifdef USE_PMIC_WATCHDOG
	static boolean init = FALSE;

	if (!init )
	{
		/* Bite and Bark times in seconds */
		uint32 bite_time = 128, bark_time = 127;

		/* Call the target specific routine to enable the watchdog */
		PM_target_wdog_init(bark_time, bite_time);

		init = TRUE;
	}
#endif
}

/*===========================================================================
**  Function : PM_DisableWatchdog
** ==========================================================================
*/
/*!
* 
* @brief
*	This function disables the PMIC watchdog. Configuration is expected to be done at the SDI exit.
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
*	None
*
*/
void PM_DisableWatchdog(void)
{
#ifdef USE_PMIC_WATCHDOG
	static boolean init = FALSE;

	if (!init )
	{
		/* Call the target specific routine to enable the watchdog */
		PM_target_wdog_disable();
		init = TRUE;
	}
#endif
}


/*===========================================================================
**  Function : SYSDBG_PMIC_INIT
** ==========================================================================
*/
/*!
* 
* @brief
*	PMIC driver initialization function 
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
boolean sysdbg_pmic_init(void)
{
	static boolean init = FALSE;
	if (!init)
	{
		PMBsp = &PM_map_data;
		
		// Call the target specific init routine
		/* Remove PMIC regulator configuration in SDI as it is taken care in SBL PBS RAM */
		//PMRegulator_Init();
		
		/* Configure PS_HOLD to do warm reset to collect the dumps 
		   Update PS_HOLD_RESET_CTL register to 0x1 to do warm reset */
         PMPSHOLD_Config();

		init = TRUE;
	}
	return init;

}


/*===========================================================================
**  Function : PM_SPMIREADDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper function that handles SPMI activity for PM_PerphReadData
*
*
* @param[in] 
*	Pointer to BSP entry for the peripheral, register address within the 
*	peripheral's register map, pointer to read data
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None 
*
*
*/
boolean PM_SPMIReadData(PMRegEntryType *mapdata, uint16 offset, uint32 *data)
{
	spmidatatype spmidata;
	uint8 *byte_addr = (uint8 *)data;


	spmidata.slaveid = mapdata->slave_id; // Accessing this slave
	spmidata.command = SPMI_CMD_EXT_REG_READ_LONG; // Doing a read
	spmidata.address = mapdata->base + (offset & 0xFF) ; // "Offset" from base, should be 8 bits wide
	
	spmidata.datalen = 1; // We are always reading only 1 byte
	
	//	Now send the command
	if (spmi_readdata(&spmidata))
	{
		*byte_addr = (uint8)(spmidata.data[0] & 0xFF);
		return TRUE;
	}
	else
	{
		// Not sure if there is a point in continuing with the process
		TZBSP_LOG_ERR("SPMI Read command failed.", 0, 0, 0);
		return FALSE;
	}
}

/*===========================================================================
**  Function : PM_SPMIWRITEDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper function that handles SPMI activity for PM_PerphWriteData 
*
*
* @param[in] 
*	Pointer to BSP entry for the peripheral, register address within the 
*	peripheral's register map, number of registers to write and pointer to 
*	the data to be written
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	The current API does not support writing more than 8 bytes at a time to a particular 
*	peripheral.  
*
*
*/
boolean PM_SPMIWriteData(PMRegEntryType *mapdata, uint16 offset, uint8 len, uint32 *data)
{
	//uint32 *valid_mask = (uint32 *)mapdata->mask;
	spmidatatype spmidata;
	//uint8 count = 0;
	
	spmidata.slaveid = mapdata->slave_id; // Accessing this slave
	spmidata.command = SPMI_CMD_EXT_REG_WRITE_LONG; // Doing a write
	spmidata.address = mapdata->base + (offset & 0xFF) ; // "Offset" from base, should be 8 bits wide
	spmidata.datalen = len; // We are writing 'len' bytes 


	// Check if all the registers are valid for this peripheral
	// Broken currently, revisit later
/*	while (count < len)
	{
		// Get the mask in which this register is present 
		uint32 *mask = valid_mask + ((offset + count) >> 5) ;
		if (!BITSET(*mask, ((offset + count) % 32)))
			TZBSP_LOG_ERR("Write to invalid register", 0, 0, 0);
		count++;
	} */


				
	// Copy len bytes over. If greater than 8 bytes, it will cause an error in the SPMI write
	// Copy the first word over directly
	spmidata.data[0] = *data;
	if (len > sizeof(uint32))
		spmidata.data[1] = *(data + 1);

	//	Now send the command
	if (spmi_writedata(&spmidata))
		return TRUE;
	else
	{
		// Not sure if there is a point in continuing with the process.
		// It will cause a fatal error in the SPMI write itself
		TZBSP_LOG_ERR("SPMI Read command failed.", 0, 0, 0);
		return FALSE;
	}
}

/*===========================================================================
**  Function : PM_SPMIREADDATAALL
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper API for the SPMI interaction for PM_ReadPerphDataAll function
*
*
* @param[in] 
*	Pointer to the BSP entry for the peripheral, Pointer to the return data
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None  
*
*
*/

boolean PM_SPMIReadDataAll(PMRegEntryType *mapdata, uint32 *data)
{

	uint32 *valid_mask = (uint32 *)mapdata->mask;
	uint8 *byte_addr = (uint8 *)data;
	uint32 bitcount, maskcount = 0 ;
	uint16 reg;


	// We have just 256 bits for 256 regs possible in the 8 bit address space and the mask currently which translates to 8 words
	while (maskcount < sizeof(PeripheralRegValid_type)/sizeof(uint32))
	{
		// Take every bit of every mask and read the data out
		if (valid_mask)
		{
			bitcount = 0;
			while ( bitcount < 32)
			{
				if (BITSET(*valid_mask, bitcount))
				{
					reg = maskcount * 32 + bitcount;
					PM_SPMIReadData(mapdata, reg, (uint32 *)byte_addr);

					// We can skip the else here because if the SPMI transaction failed, it will
					// cause a fatal error in the readdata function
				}
				else
				{
					// Reset the byte addr data
					memset((void *)byte_addr, 0, 1);
				}
				// Move to next dest
				byte_addr++;
				// Move to next bit
				bitcount++;
			}
			// Move to the next mask
			valid_mask++;
			maskcount++;
		}
		else
		{
			// We can zero out all 32 bytes of space because they are invalid for the peripheral
			memset((void *)byte_addr, 0, 32);
			byte_addr = byte_addr + 32;
		}
	}
	
	return TRUE;
}

/*===========================================================================
**  Function : PM_READPERPHDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	This function will let you read one register from a particular peripheral. 
*	The returned 8 bit data is the least significant byte of data
*
* @param[in] 
*	Peripheral Enum entry, Register Enum Entry, Pointer for return data
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
void PM_ReadPerphData(PMPerphName perph, uint16 reg, uint32 *data)
{
	// Since the check for valid perpherial is done in the following function call we are skipping
	// it here 
	PMRegEntryType mapdata;
		
	// Get the data from the bsp
	pm_getmapinfo(perph, &mapdata);

	// Now check if the data is valid 
	// The base address has to be 8 bit aligned, mask shouldn't be NULL and slave id should be
	// valid
	if ((mapdata.slave_id < SPMI_SLAVE_COUNT) && (!(mapdata.base & 0xFF))  && (mapdata.mask))
	{
		PM_SPMIReadData(&mapdata, reg, data);
	}
	else
	{
		// ERR_FATAL here
		TZBSP_LOG_ERR("Invalid BSP parameters", 0, 0, 0);
	}
}


/*===========================================================================
**  Function : PM_WRITEPERPHDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	This function will let you write to 'len' number of registers of peripheral
*	'perph' starting with register 'reg'. It assumes 'len' bytes of data are written
*	to 'len' number of registers.
*
*
* @param[in] 
*	Peripheral Enum entry, Peripheral Register Enum Entry, Number of registers to write,
*	Pointer to data for the registers.
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	The current API does not support writing more than 8 bytes at a time to a particular 
*	peripheral.
*
*
*/
void PM_WritePerphData(PMPerphName perph, uint16 reg, uint8 len, uint32 *data)
{
	// Since the check for valid perpherial is done in the following function call we are skipping
	// it here 
	PMRegEntryType mapdata;
		
	// Get the data from the bsp
	pm_getmapinfo(perph, &mapdata);

	// Now check if the data is valid 
	// The base address has to be 8 bit aligned, mask shouldn't be NULL and slave id should be
	// valid
	if ((mapdata.slave_id < SPMI_SLAVE_COUNT) && (!(mapdata.base & 0xFF))  && (mapdata.mask))
	{
		PM_SPMIWriteData(&mapdata, reg, len, data);
	}
	else
	{
		// ERR_FATAL here
		TZBSP_LOG_ERR("Invalid BSP parameters", 0, 0, 0);
	}
}


/*===========================================================================
**  Function : PM_READPERPHDATAALL
** ==========================================================================
*/
/*!
* 
* @brief
*	This function will let you read all the registers of a particular peripheral
*
*
* @param[in] 
*	Peripheral Enum entry, Pointer to the start of the return data
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	The user should ensure that there is enough space to store all the registers
*	for the peripheral starting with the address provided by data. Currently, we 
*	are looking at 256 bytes starting with data. 
*
*
*/
void PM_ReadPerphDataAll(PMPerphName perph, uint32 *data)
{
	// Since the check for valid perpherial is done in the following function call we are skipping
	// it here 
	PMRegEntryType mapdata;
		
	// Get the data from the bsp
	pm_getmapinfo(perph, &mapdata);

	// Now check if the data is valid 
	// The base address has to be 8 bit aligned, mask shouldn't be NULL and slave id should be
	// valid
	if ((mapdata.slave_id < SPMI_SLAVE_COUNT) && (!(mapdata.base & 0xFF))  && (mapdata.mask))
	{
		PM_SPMIReadDataAll(&mapdata, data);
	}
	else
	{
		// ERR_FATAL here
		TZBSP_LOG_ERR("Invalid BSP parameters", 0, 0, 0);
	}
}





