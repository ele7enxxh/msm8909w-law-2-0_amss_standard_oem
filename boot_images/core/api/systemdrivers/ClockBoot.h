#ifndef __CLOCKBOOT_H__
#define __CLOCKBOOT_H__

/** @file ClockBoot.h

  This file contains the external interface definitions for the clock regime device driver.
  This driver is support for the Krait Second Primary Boot Loader.

  Copyright (c) 2010-2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/ClockBoot.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/08/11   vtw     Added support for SDC clocks.
 04/11/11   vtw     Created from 8960 clock driver project.

=============================================================================*/

/*=========================================================================
      Includes
==========================================================================*/

#include "com_dtypes.h"
#include "DDIChipInfo.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF

/**
@brief List of boot speeds that the clock driver supports.

MIN - XO frequency perf level.
NOMINAL - Highest frequency at nominal voltage level.
MAX - Max frequency support by the frequency plan.
DEFAULT - Same as NOMINAL.

*/
typedef enum
{
  CLOCK_BOOT_PERF_NONE,
  CLOCK_BOOT_PERF_MIN,
  CLOCK_BOOT_PERF_NOMINAL,
  CLOCK_BOOT_PERF_MAX,
  CLOCK_BOOT_PERF_DEFAULT,
  CLOCK_BOOT_PERF_NUM,
  CLOCK_ENUM_32BITS(BOOT_PERF)
} ClockBootPerfLevelType;

/**
@brief List of SDC clocks that the clock driver supports.
*/
typedef enum
{
   CLK_SDC_NONE,
   CLK_SDC1,
   CLK_SDC2,
   CLK_SDC3,
   CLK_SDC4,
   CLK_SDC5,
   CLK_SDC_NUM_CLKS
} ClockSDCType;

/**
@brief List of SDC clocks that the clock driver supports.
*/
typedef enum
{
   CLK_BLSP_UART_NONE,
   CLK_BLSP1_UART1_APPS,
   CLK_BLSP1_UART2_APPS,
   CLK_BLSP1_UART3_APPS,
   CLK_BLSP1_UART4_APPS,
   CLK_BLSP1_UART5_APPS,
   CLK_BLSP1_UART6_APPS,
   CLK_BLSP2_UART1_APPS,
   CLK_BLSP2_UART2_APPS,
   CLK_BLSP2_UART3_APPS,
   CLK_BLSP2_UART4_APPS,
   CLK_BLSP2_UART5_APPS,
   CLK_BLSP2_UART6_APPS,
   CLK_BLSP_UART_NUM_CLKS
} ClockUARTType;


/**
@brief List of QUP_I2C clocks that the clock driver supports.
*/
typedef enum
{
   CLK_BLSP_QUP_I2C_NONE,
   CLK_BLSP1_QUP1_I2C_APPS,
   CLK_BLSP1_QUP2_I2C_APPS,
   CLK_BLSP1_QUP3_I2C_APPS,
   CLK_BLSP1_QUP4_I2C_APPS,
   CLK_BLSP1_QUP5_I2C_APPS,
   CLK_BLSP1_QUP6_I2C_APPS,
   CLK_BLSP_QUP_I2C_NUM_CLKS
} ClockQUPI2CType;

typedef enum
{
  /* No request / Invalid request  */
  CLOCK_RESOURCE_QUERY_INVALID         = 0,

  /*
  Clock driver query ID for the number of supported performance levels.
  This query returns the number of supported performance levels for
  the clock resource in question.
 */
  CLOCK_RESOURCE_QUERY_NUM_PERF_LEVELS,

  /*
  CLock driver query for the minimum supported frequency in kHz for a clock resource.
  This query returns the minimum supported frequency for the resource
  based on the target.
 */
  CLOCK_RESOURCE_QUERY_MIN_FREQ_KHZ, 

  /*
  CLock driver for all supported frequencies in kHz for a clock resource.
  This query returns the array of supported frequency for the resource
  based on the target and the voltage levels.
 */
  CLOCK_RESOURCE_QUERY_ALL_FREQ_KHZ,
} ClockQueryType;

/*
 * ClockVRegLevelType
 *
 * List of voltage levels that match to a certain clock frequency.  Note that the
 * enumeration values map to pmic. It has to match with railway levels as well.
 *
 *  OFF          - Turn off non-CX rail
 *  LOW          - Lowest voltage level, or "do not care"
 *  NOMINAL      - Nominal voltage level
 *  NOMINAL_PLUS - Intermediate leve between Nominal and high (Turbo)
 *  HIGH         - Elevated, or "Super Turbo" voltage level.
 */

typedef enum
{
  CLOCK_VREG_LEVEL_OFF          = 0, /* RAILWAY_NO_REQUEST  */
  CLOCK_VREG_LEVEL_LOW_MINUS    = 2, /* RAILWAY_SVS_KRAIT   */
  CLOCK_VREG_LEVEL_LOW          = 3, /* RAILWAY_SVS_SOC     */
  CLOCK_VREG_LEVEL_LOW_PLUS     = 4, /* RAILWAY_SVS_HIGH    */
  CLOCK_VREG_LEVEL_NOMINAL      = 5, /* RAILWAY_NOMINAL     */
  CLOCK_VREG_LEVEL_NOMINAL_PLUS = 6, /* RAILWAY_TURBO       */
  CLOCK_VREG_LEVEL_HIGH_MINUS   = 7, /* RAILWAY_TURBO_HIGH  */
  CLOCK_VREG_LEVEL_HIGH         = 8, /* RAILWAY_SUPER_TURBO */
  CLOCK_VREG_LEVEL_HIGH_NO_CPR  = 9, /* RAILWAY_SUPER_TURBO_NO_CPR */
  CLOCK_VREG_NUM_LEVELS,
} ClockVRegLevelType;


/*
 * ClockType
 * List of clocktype supported by the api.Clock_GetClockFrequency
 *
 *  CLK_APPS         - APSS Clock
 *  CLK_DDR          - DDR clock
 */


typedef enum
{
   CLK_APPS,
   CLK_DDR,
   CLK_NUM,
   CLOCK_ENUM_32BITS(CLOCK_TYPE)
} ClockType;


/*
 * ClockPlanType
 *
 * Struct of clock plan that has frequency that associates with voltage levels
 */
typedef struct 
{
  uint32             nFreqKHz;
  ClockVRegLevelType eVRegLevel;
}ClockPlanType;

/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.
*/
boolean Clock_PreDDRInit( uint32 ddr_type );


/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.
*/
boolean Clock_PreDDRInitEx( uint32 ddr_type );

/* ============================================================================
**  Function : Clock_Init
** ============================================================================
*/
/*!

    This function turns on the required clocks and configures
    Fabric and Krait speeds depending on the System Fabric and
    CPU boot performance level.

    @param eSysPerfLevel   -  [in] Fabric and DDR performance level to initialize.
    @param eCPUPerfLevel   -  [in] Scropion CPU performance level to initialize.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/

boolean Clock_Init(ClockBootPerfLevelType eSysPerfLevel,
                    ClockBootPerfLevelType eCPUPerfLevel);

/* ============================================================================
**  Function : Clock_SetSysPerfLevel
** ============================================================================
*/
/**
  Configure System and Application Fabric to a perf level.

  @param eSysPerfLevel [in]  -  Fabric and DDR performance level to configure.

  @return
  TRUE -- Fabric was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetSysPerfLevel(ClockBootPerfLevelType eSysPerfLevel);

/* ============================================================================
**  Function : Clock_SetCPUPerfLevel
** ============================================================================
*/
/**
  Configure Krait CPU to a specific perf level.

  @param eCPUPerfLevel [in] - CPU performance level.

  @return
  TRUE -- CPU was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel);

/* ============================================================================
**  Function : Clock_EnableL2
** ============================================================================
*/
/*!
    Configure L2 cache to a specific perf level.

    @param eL2PerfLevel   -  [IN] CPU performance level

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetL2PerfLevel(ClockBootPerfLevelType eL2PerfLevel);

/* ============================================================================
**  Function : Clock_SetSDCClockFrequency
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetSDCClockFrequency(ClockBootPerfLevelType ePerfLevel,
                                    ClockSDCType eClock);


/* ============================================================================
**  Function : Clock_SetSDCClockFrequencyExt
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param nFreqKHz          -  [IN] Frequency in KHz
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSDCClockFrequencyExt( uint32 nFreqKHz, ClockSDCType eClock);

/* ============================================================================
**  Function : Clock_SetUARTClockFrequency
** ============================================================================
*/
/*!
    Configure UART clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
                     eClock            -  [IN] UART clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetUARTClockFrequency(ClockBootPerfLevelType ePerfLevel,
                                    ClockUARTType eClock);


/* ============================================================================
**	Function : Clock_DisableUARTClock
** ============================================================================
*/
/*!
	Disable  specific UART clock.

	@param    eClock	-	[IN] UART clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableUARTClock(ClockUARTType eClock);


/* ============================================================================
**  Function : Clock_SetI2CClockFrequency
** ============================================================================
*/
/*!
    Configure an I2C clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
                     eClock  -  [IN] I2C clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetI2CClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockQUPI2CType eClock
);


/* ============================================================================
**	Function : Clock_DisableI2CClock
** ============================================================================
*/
/*!
	Disable a specific I2C clock.

	@param    eClock	-	[IN] I2C clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableI2CClock(ClockQUPI2CType eClock);

/* ============================================================================
**  Function : Clock_InitForDownloadMode
** ============================================================================
*/
/*!
    This will initialize clocks for the RAMs in the system that are to be saved
    during crash dump procedures in the downloader.

    @param
      None
    @return
      None

    @dependencies
    None.

    @sa None
*/
void Clock_InitForDownloadMode(void);

/* ============================================================================
**  Function : Clock_InitUSB
** ============================================================================
*/
/*!
    Configure USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB(void);

/* ============================================================================
**  Function : Clock_InitCrypto
** ============================================================================
*/
/*!
    Configure Crypto clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_InitCrypto(void);

/* ============================================================================
**  Function : Clock_ExitBoot
** ============================================================================
*/
/*!

    This function turns off clocks that were used during boot, but are not
    needed after boot.  This should be called at the end of boot.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    Call this last in the boot sequence.

*/
boolean Clock_ExitBoot(void);

/* ============================================================================
**  Function : Clock_DebugInit
** ============================================================================
*/
/*!

    This function is called very early in boot.  It is used for work-arounds that
    need to be done before JTAG attaches at the SBL "hold focus button" spin loop.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    You cannot use busywait in this function.

*/
boolean Clock_DebugInit(void);

/* ============================================================================
**  Function : Clock_DisableUSB
** ============================================================================
*/
/*!
    Disable USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisableUSB(void);

/* ============================================================================
**  Function : Clock_DDRSpeed
** ============================================================================
*/
/*!
    Return the DDR clock rate in kilohertz.  This clock rate is the bus speed.
    It is not the controller clock (2X) clock for targets that use Legacy mode.

    @param None.
    @return
      The speed configured in Clock_PreDDRInit() of the BIMC/DDR clock.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_DDRSpeed(void);

/* ========================================================================
**  Function : Clock_I2CInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for EEPROM to be used Pre DDR.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_I2CInit(void);

/* ========================================================================
**  Function : Clock_EnableQPICForDownloadMode
** ======================================================================*/
/*
    Description: Enable QPIC clocks.
                 The function is used in Download Mode where QPIC clocks are
                 not enabeld by PBL.

    @param None
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnableQPICForDownloadMode( void );

/* ========================================================================
**  Function : Clock_BIMCQuery
** ======================================================================*/

void Clock_BIMCQuery(ClockQueryType nQuery,void* pResource);

void Clock_SwitchBusVoltage(ClockVRegLevelType eVoltageLevel);

boolean Clock_SetBIMCSpeed(uint32 nFreqKHz );

/* ========================================================================
**  Function : Clock_ExtBuck_GPIO_Misc
** ======================================================================*/
/*
    Description: Configure GPIO 111 to OUTPUT PULL UP HIGH
                 and set the TLMM_SPARE register for 8916 and 8936

    @param      None.
    @return
    TRUE  -- Configuration was success.
    FALSE -- Configuration failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ExtBuckGPIOMisc( void );

/* ============================================================================
**  Function : Clock_Usb30SwitchPipeClk
** ============================================================================
*/
/*!
    Switch to USB3_PIPE_CLK.

    @param None.
    @return
    TRUE -- Switch was successful.
    FALSE -- Switch failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30SwitchPipeClk(void);


/* ============================================================================
**  Function : Clock_Usb30EnableSWCollapse
** ============================================================================
*/
/*!
    Enable SW Collapse for USB30

    @param Enable/Disable.
    @return
    TRUE always

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30EnableSWCollapse(boolean enable);

/* ============================================================================
**  Function : Clock_Usb30GetSWCollapse
** ============================================================================
*/
/*!
    Returns the status of SW Collapse for USB30

    @param None
    @return
    TRUE if enabled
    FALSE if disabled

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30GetSWCollapse(void);

/* ========================================================================
**  Function : Clock_EnableQPICForDownloadMode
** ======================================================================*/
/*
    Description: Configure QPIC(NAND) clocks at 100MHz on GPLL0 and enable
                 QPIC clocks.
                 The function is used in Download Mode where QPIC clocks are
                 not enabeld by PBL.

    @param None
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnableQPICForDownloadMode( void );


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/*!
     To find the clock frequency (in KHZ) of a given clock

    @param eClock - clock type for which the speed to be determined
    @param *pnFrequencyHz - This will contain the frequency (in KHZ) value after function returns
    @return
    TRUE -- pnFrequencyHz points to valid frequency
    FALSE -- pnFrequencyHz is invalid.

    @dependencies
    In case of apps clock,  if this function is called before the Clock_Init, this will return false. As
    it requires some initialization to take place.

    @sa None
*/

boolean Clock_GetClockFrequency(ClockType  eClock,  uint32 * pnFrequencyHz);

/* ========================================================================
**  Function : Clock_GetChipId()
** ======================================================================*/
/*
    Description: Get chip ID type stub API

    @param   None
    @return
    TRUE  -- Configuration was success.
    FALSE -- Configuration failed.

    @dependencies
    None.

    @sa None
*/

DalChipInfoIdType Clock_GetChipId(void);

#endif  /* __CLOCKBOOT_H__ */
