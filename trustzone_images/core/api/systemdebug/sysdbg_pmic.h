/*=============================================================================
           			 D B I  	P M I C 	D R I V E R 

GENERAL DESCRIPTION     
		  This file contains the PMIC driver APIs to be used in the Sysdebug
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdebug/sysdbg_pmic.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
01/29/14    ntonupun    Ported to Bear
06/06/12	AJCheriyan	First cut for Aragorn. First ever DBI

===========================================================================*/

#ifndef _SYSDBG_PMIC_H
#define _SYSDBG_PMIC_H

#include "comdef.h"

#define PMIC_ENUM32(x)	PMIC_##x = 0x7FFFFFFF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

// This structure contains a bit field for every register
// that is valid for a particular peripheral's register map
typedef struct
{
	uint32 valid1;
	uint32 valid2;
	uint32 valid3;
	uint32 valid4;
	uint32 valid5;
	uint32 valid6;
	uint32 valid7;
	uint32 valid8;
}PeripheralRegValid_type;

/* Enumeration for the different types of peripherals in the PMIC. The integer values
 * assigned are not important, even though it might be safer to append new entries
 * at the end */
typedef enum
{
	BBA_CLK1         = 0,
	BBA_CLK2         = 1,
	BOOSTA           = 2,
	BOOSTA_FREQ      = 3,
	BUAA             = 4,
	BUCKA_CMN        = 5,
	BUSA             = 6,
	CCADC1A_MDM      = 7,
	CCADC1A_USR      = 8,
	CLKA_DIST        = 9,
	COINA            = 10,
	DIFFA_CLK1       = 11,
	DIVA_CLK1        = 12,
	DIVA_CLK2        = 13,
	DIVA_CLK3        = 14,
	FLASH1A          = 15,
	GPIO1A           = 16,
	GPIO10A          = 17,
	GPIO11A          = 18,
	GPIO12A          = 19,
	GPIO13A          = 20,
	GPIO14A          = 21,
	GPIO15A          = 22,
	GPIO16A          = 23,
	GPIO17A          = 24,
	GPIO18A          = 25,
	GPIO19A          = 26,
	GPIO2A           = 27,
	GPIO20A          = 28,
	GPIO21A          = 29,
	GPIO22A          = 30,
	GPIO23A          = 31,
	GPIO24A          = 32,
	GPIO25A          = 33,
	GPIO26A          = 34,
	GPIO27A          = 35,
	GPIO28A          = 36,
	GPIO29A          = 37,
	GPIO3A           = 38,
	GPIO30A          = 39,
	GPIO31A          = 40,
	GPIO32A          = 41,
	GPIO33A          = 42,
	GPIO34A          = 43,
	GPIO35A          = 44,
	GPIO36A          = 45,
	GPIO4A           = 46,
	GPIO5A           = 47,
	GPIO6A           = 48,
	GPIO7A           = 49,
	GPIO8A           = 50,
	GPIO9A           = 51,
	HDMIA            = 52,
	INTA             = 53,
	KEYPAD1A         = 55,
	KPDBLA_LUT       = 56,
	KPDBLA_MAIN      = 57,
	LDO1A            = 58,
	LDO10A           = 59,
	LDO11A           = 60,
	LDO12A           = 61,
	LDO13A           = 62,
	LDO14A           = 63,
	LDO15A           = 64,
	LDO16A           = 65,
	LDO17A           = 66,
	LDO18A           = 67,
	LDO19A           = 68,
	LDO2A            = 69,
	LDO20A           = 70,
	LDO21A           = 71,
	LDO22A           = 72,
	LDO23A           = 73,
	LDO24A           = 74,
	LDO3A            = 75,
	LDO4A            = 76,
	LDO5A            = 77,
	LDO6A            = 78,
	LDO7A            = 79,
	LDO8A            = 80,
	LDO9A            = 81,
	LPGA_CHAN1       = 82,
	LPGA_CHAN10      = 83,
	LPGA_CHAN11      = 84,
	LPGA_CHAN12      = 85,
	LPGA_CHAN2       = 86,
	LPGA_CHAN3       = 87,
	LPGA_CHAN4       = 88,
	LPGA_CHAN5       = 89,
	LPGA_CHAN6       = 90,
	LPGA_CHAN7       = 91,
	LPGA_CHAN8       = 92,
	LPGA_CHAN9       = 93,
	LPGA_LUT         = 94,
	LVS1A            = 95,
	LVS2A            = 96,
	LVS3A            = 97,
	MBG1A            = 98,
	MISCA            = 99,
	MPP1A            = 100,
	MPP2A            = 101,
	MPP3A            = 102,
	MPP4A            = 103,
	MPP5A            = 104,
	MPP6A            = 105,
	MPP7A            = 106,
	MPP8A            = 107,
	OTGA             = 108,
	PBSA_CLIENT0     = 109,
	PBSA_CLIENT1     = 110,
	PBSA_CLIENT10    = 111,
	PBSA_CLIENT11    = 112,
	PBSA_CLIENT12    = 113,
	PBSA_CLIENT13    = 114,
	PBSA_CLIENT14    = 115,
	PBSA_CLIENT15    = 116,
	PBSA_CLIENT2     = 117,
	PBSA_CLIENT3     = 118,
	PBSA_CLIENT4     = 119,
	PBSA_CLIENT5     = 120,
	PBSA_CLIENT6     = 121,
	PBSA_CLIENT7     = 122,
	PBSA_CLIENT8     = 123,
	PBSA_CLIENT9     = 124,
	PBSA_CORE        = 125,
	PONA             = 126,
	PWMA_3D          = 127,
	REVIDA           = 128,
	RFA_CLK1         = 129,
	RFA_CLK2         = 130,
	RFA_CLK3         = 131,
	RTCA_ALARM       = 132,
	RTCA_TIMER       = 133,
	RTCA_WR          = 134,
	S1A_CTRL         = 135,
	S1A_FREQ         = 136,
	S1A_PS           = 137,
	S2A_CTRL         = 138,
	S2A_FREQ         = 139,
	S2A_PS           = 140,
	S3A_CTRL         = 141,
	S3A_FREQ         = 142,
	S3A_PS           = 143,
	SLEEPA_CLK1      = 144,
	SMBBA_BAT_IF     = 145,
	SMBBA_BOOST      = 146,
	SMBBA_BUCK       = 147,
	SMBBA_CHGR       = 148,
	SMBBA_DC_CHGPTH   = 149,
	SMBBA_FREQ       = 150,
	SMBBA_MISC       = 151,
	SMBBA_USB_CHGPTH   = 152,
	SPMIA            = 153,
	SPMIA_OPTIONS    = 154,
	TEMPA_ALARM      = 155,
	TRIMA            = 156,
	TRIA_LED         = 157,
	VADC1A_MDM       = 158,
	VADC1A_SEC       = 159,
	VADC1A_USR       = 160,
	VIB1A            = 161,
	VREFLPDDRA       = 162,
	VREGA_TFT        = 163,
	WLED1A           = 164,
	XOA              = 165,
	BCLKB_GEN_MAIN   = 166,
	BUSB             = 167,
	CLKB_DIST        = 168,
	INTB             = 169,
	MBG1B            = 171,
	MISCB            = 172,
	MPP1B            = 173,
	MPP2B            = 174,
	MPP3B            = 175,
	MPP4B            = 176,
	PBSB_CLIENT0     = 177,
	PBSB_CLIENT1     = 178,
	PBSB_CLIENT2     = 179,
	PBSB_CLIENT3     = 180,
	PBSB_CLIENT4     = 181,
	PBSB_CLIENT5     = 182,
	PBSB_CLIENT6     = 183,
	PBSB_CLIENT7     = 184,
	PBSB_CORE        = 185,
	PONB             = 186,
	REVIDB           = 187,
	S1B_CTRL         = 188,
	S1B_FREQ         = 189,
	S1B_PS           = 190,
	S2B_CTRL         = 191,
	S2B_FREQ         = 192,
	S2B_PS           = 193,
	S3B_CTRL         = 194,
	S3B_FREQ         = 195,
	S3B_PS           = 196,
	S4B_CTRL         = 197,
	S4B_FREQ         = 198,
	S4B_PS           = 199,
	S5B_CTRL         = 200,
	S5B_FREQ         = 201,
	S5B_PS           = 202,
	S6B_CTRL         = 203,
	S6B_FREQ         = 204,
	S6B_PS           = 205,
	S7B_CTRL         = 206,
	S7B_FREQ         = 207,
	S7B_PS           = 208,
	S8B_CTRL         = 209,
	S8B_FREQ         = 210,
	S8B_PS           = 211,
	SPMIB            = 212,
	SPMIB_OPTIONS    = 213,
	TEMPB_ALARM      = 214,
	TRIMB            = 215,
	S4A_CTRL		 = 216,
	S4A_FREQ		 = 217,
	S4A_PS			 = 218,
	S5A_CTRL		 = 219,
	S5A_FREQ		 = 220,
	S5A_PS			 = 221,
	S6A_CTRL		 = 222,
	S6A_FREQ		 = 223,
	S6A_PS			 = 224,
	S7A_CTRL		 = 225,
	S7A_FREQ		 = 226,
	S7A_PS			 = 227,
	S8A_CTRL		 = 228,
	S8A_FREQ		 = 229,
	S8A_PS			 = 230,
	S9A_CTRL		 = 231,
	S9A_FREQ		 = 232,
	S9A_PS			 = 233,
	S10A_CTRL		 = 234,
	S10A_FREQ		 = 235,
	S10A_PS			 = 236,
	S11A_CTRL		 = 237,
	S11A_FREQ		 = 238,
	S11A_PS			 = 239,
	S12A_CTRL		 = 240,
	S12A_FREQ		 = 241,
	S12A_PS			 = 242,
	VALID_PERIPH_COUNT = 243,
}PMPerphName;

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
boolean sysdbg_pmic_init(void);

/*===========================================================================
**  Function : PMEnableAppsRails
** ==========================================================================
*/
/*!
* 
* @brief
*	API to enable the Apps rails post-reset
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   Success / Failure
* 
* @par Side Effects
*   None
*
*
*/
boolean PMEnableAppsRails(void);

/*===========================================================================
**  Function : PMCheckAppsRails(uint32 core)
** ==========================================================================
*/
/*!
* 
* @brief
*	API to check the state of the apps rails
*
* @param[in] 
*	Core whose rail has to be checked
*  
* @par Dependencies
*	None
* 
* @retval
*   Returns true if enabled
* 
* @par Side Effects
*   None
*
*
*/
boolean PMCheckAppsRails(uint32 core);


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

void PM_ReadPerphData(PMPerphName perph, uint16 reg, uint32 *data);

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
void PM_ReadPerphDataAll(PMPerphName perph, uint32 *data);


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
void PM_WritePerphData(PMPerphName perph, uint16 reg, uint8 len, uint32 *data);

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
*	Dog will not be pet when SDI runs and if there is a hang, the PMIC Watchdog will help 
*	recover the device into download mode.
*
*/
void PM_EnableWatchdog(void);

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
void PM_DisableWatchdog(void);

void sysdbg_err_fatal_handler(uint32 *pc, uint32 arg1, uint32 arg2, uint32 arg3);

#endif /* _SYSDBG_PMIC_H */
