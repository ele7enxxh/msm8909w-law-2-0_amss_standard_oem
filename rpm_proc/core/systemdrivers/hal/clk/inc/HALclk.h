#ifndef __HALCLK_H__
#define __HALCLK_H__
/*
==============================================================================

FILE:         HALclk.h

DESCRIPTION:

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/inc/HALclk.h#3 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/18/11   vs      Ported from the latest clock driver.
01/24/13   pbitra  Replaced  HAL_clk_GetChipId with HAL_clk_GetChipFamily
==============================================================================
            Copyright (c) 2011-2012 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALcomdef.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*
 * Macro to generate the hardware version number from a given major
 * and minor number.  Used when comparing against return value of
 * HAL_clk_GetChipVersion(). For example to check if this is at least 
 * version 1.2:
 *   if (HAL_clk_GetChipVersion() >= HAL_CLK_CHIP_VERSION(1,2))
 */
#define HAL_CLK_CHIP_VERSION(major, minor)  (((major) << 16) | (minor))

/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_clk_SourceType
 *
 */
typedef enum
{
  HAL_CLK_SOURCE_NULL             = 0,

  /*
   * XO
   */
  HAL_CLK_SOURCE_XO               = 1,

  /*
   * PLL
   */
  HAL_CLK_SOURCE_GPLL0,
  HAL_CLK_SOURCE_GPLL1,
  HAL_CLK_SOURCE_GPLL2,
  HAL_CLK_SOURCE_GPLL3,
  HAL_CLK_SOURCE_GPLL4,
  HAL_CLK_SOURCE_GPLL5,
  HAL_CLK_SOURCE_BIMCPLL,
  HAL_CLK_SOURCE_GPLL6,
  HAL_CLK_SOURCE_MMPLL0,
  HAL_CLK_SOURCE_MMPLL1,
  HAL_CLK_SOURCE_MMPLL2,
  HAL_CLK_SOURCE_MMPLL3,

  /*
   * External source
   */
  HAL_CLK_SOURCE_EXTERNAL,
  HAL_CLK_NUM_OF_EXTERNAL_SOURCES,

  /*
   * Raw
   */
  HAL_CLK_SOURCE_RAW0,
  HAL_CLK_SOURCE_RAW1,
  HAL_CLK_SOURCE_RAW2,

  /*
   * Misc
   */
  HAL_CLK_SOURCE_SLEEPCLK,
  HAL_CLK_SOURCE_GROUND,

  HAL_CLK_NUM_OF_SOURCES,

  HAL_ENUM_32BITS(CLK_SOURCE)
} HAL_clk_SourceType;


/*
 * HAL_clk_ClockConfigType
 *
 * Generic configurations used for clocks.  Each clock will support
 * a different set of these enumerations.
 *
 */
typedef enum
{
  HAL_CLK_CONFIG_NULL = 0,

  /*
   * Using lower half as clock config values that will be passed
   * by the client via top level driver interface.
   */
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI0     = 1,
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI1     = 2,
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI2     = 3,


  /*
   * Using upper half as clock config values that will be internal
   * to our driver.
   */
  HAL_CLK_CONFIG_ASYNC_MODE = 1000,
  HAL_CLK_CONFIG_SYNC_MODE,
  HAL_CLK_CONFIG_1X_MODE,
  HAL_CLK_CONFIG_2X_MODE,

  HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
  HAL_CLK_CONFIG_Q6SS_CORE_LOW_JITTER_PLL,
  HAL_CLK_CONFIG_Q6SS_CORE_PLL_MAIN,

  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_MUX,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_PLL2,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_HSPLL1,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_HSPLL2,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_RAMP_CLOCK,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_JTAG_TCK,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_MUX_SYNC_TO_AXI,
  HAL_CLK_CONFIG_ACPU_SLEEP_USE_SLEEP_EN,
  HAL_CLK_CONFIG_ACPU_SLEEP_MASK_CORE_CLK,
  HAL_CLK_CONFIG_ACPU_SLEEP_WAIT_FOR_AXI_CLK,
  HAL_CLK_CONFIG_ACPU_NO_POWER_DOWN_REQ_ON_SWFI,
  HAL_CLK_CONFIG_ACPU_POWER_DOWN_REQ_ON_SWFI,
  HAL_CLK_CONFIG_ACPU_POWER_UP_REQ_ON_INTERRUPT,
  HAL_CLK_CONFIG_ACPU_NO_POWER_UP_REQ_ON_INTERRUPT,

  HAL_CLK_CONFIG_VFE_VIDEO,
  HAL_CLK_CONFIG_VFE_CAMERA,
  HAL_CLK_CONFIG_VFE_EXTERNAL,

  HAL_ENUM_32BITS(CLK_CONFIG)
} HAL_clk_ClockConfigType;



/*
 * HAL_clk_ClockAPIType
 *
 * Programming interfaces available for controlling clocks.  Used when the
 * driver needs to verify if the functionality is available for a given
 * clock.  See HAL_clk_IsClockSupported.
 */
typedef enum
{
  HAL_CLK_CLOCK_API_ENABLE,
  HAL_CLK_CLOCK_API_DISABLE,
  HAL_CLK_CLOCK_API_RESET,
  HAL_CLK_CLOCK_API_INVERT,
  HAL_CLK_CLOCK_API_ISON,
  HAL_CLK_CLOCK_API_ISENABLED,
  HAL_CLK_CLOCK_API_CONFIG,
  HAL_CLK_CLOCK_API_CONFIGDIVIDER,
  HAL_CLK_CLOCK_API_CALCFREQ,

  HAL_ENUM_32BITS(CLK_CLOCK_API)
} HAL_clk_ClockAPIType;


/*
 * HAL_clk_ClockMuxConfigType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  eSource  - The source to use.
 *  nDiv2x   - The integer (2 * divider) value to use.
 *  nM       - The M value for any M/N counter, or 0 to bypass.
 *  nN       - The N value for any M/N counter.
 *  n2D      - Twice the D value for any M/N counter.
 */
typedef struct
{
  HAL_clk_SourceType eSource;
  uint32             nDiv2x;
  uint32             nM;
  uint32             nN;
  uint32             n2D;
} HAL_clk_ClockMuxConfigType;


/*
 * HAL_clk_SourceConfigType
 *
 * Configuration enumerations for sources.
 *
 */
typedef enum
{
  HAL_CLK_CONFIG_SOURCE_NULL = 0,

  HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE,
  HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_TEST_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_TEST_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE,

  HAL_CLK_CONFIG_XO_CHIP_BUFFER_USING_EXT_CLK_SRC,
  HAL_CLK_CONFIG_XO_USING_EXT_CRYSTAL,
  HAL_CLK_CONFIG_XO_GAIN_0,
  HAL_CLK_CONFIG_XO_GAIN_1,
  HAL_CLK_CONFIG_XO_GAIN_2,
  HAL_CLK_CONFIG_XO_GAIN_3,
  HAL_CLK_CONFIG_XO_GAIN_4,
  HAL_CLK_CONFIG_XO_GAIN_5,
  HAL_CLK_CONFIG_XO_GAIN_6,
  HAL_CLK_CONFIG_XO_GAIN_7,

  HAL_ENUM_32BITS(CLK_SOURCE_CONFIG)
} HAL_clk_SourceConfigType;


/*
 * HAL_clk_PLLVCOType
 *
 * Which voltage-controlled oscillator to use inside the PLL.  The choice
 * will depend on the desired frequency and jitter.
 */
typedef enum
{
  HAL_CLK_PLL_VCO0,
  HAL_CLK_PLL_VCO1,
  HAL_CLK_PLL_VCO2,
  HAL_CLK_PLL_VCO3,

  HAL_CLK_NUM_OF_PLL_VCOS,

  HAL_ENUM_32BITS(CLK_PLL_VCO)
} HAL_clk_PLLVCOType;


/*
 * HAL_clk_PLLConfigType
 *
 * Parameters used for configuring a source that is a PLL.
 *
 *  eSource  - The source to use for the PLL.
 *  eVCO     - The internal VCO to use.
 *  nPreDiv  - The pre-divider value (generally 1 or 2).
 *  nPostDiv - The pre-divider value (generally 1 or 2).
 *  nL       - The L value for the PLL.  The PLL output frequency is derived
 *             as out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv.
 *  nM       - The M value (see above).
 *  nN       - The N value (see above).
 *  nVCOMultiplier     - The PLL quadrupler mode or doubler mode value. 
 *  nAlpha             - The Alpha value for the PLL.  The PLL output frequency is derived
 *                       as out_freq = ((in_freq / nPreDiv) * (L + [AlphaU : Alpha])) / nPostDiv
 *  nAlphaU            - The AlphaU value (see above).
 */
typedef struct
{
  HAL_clk_SourceType  eSource;
  HAL_clk_PLLVCOType  eVCO;
  uint32              nPreDiv;
  uint32              nPostDiv;
  uint32              nL;
  uint32              nM;
  uint32              nN;
  uint32              nVCOMultiplier;
  uint32              nAlpha;
  uint32              nAlphaU;
} HAL_clk_PLLConfigType;


/*
 * HAL_clk_FootswitchConfigType
 *
 *  CORE_FORCE_ON         - Force the core to remain on when the clock
 *                          is disabled.  This is generally the default.
 *  CORE_ALLOW_OFF        - Allow the core logic to turn off when the
 *                          core clock turns off.
 *  PERIPHERAL_FORCE_ON   - Force the peripheral logic to remain on
 *                          when the clock is disabled.
 *  PERIPHERAL_ALLOW_OFF  - Allow the peripheral logic to turn off
 *                          when the core clock turns off.  This is
 *                          generally the default.
 *  PERIPHERAL_FORCE_OFF  - Force the peripheral logic to remain off
 *                          even when the clock is on.
 */
typedef enum
{
  HAL_CLK_FOOTSWITCH_CONFIG_CORE_FORCE_ON,
  HAL_CLK_FOOTSWITCH_CONFIG_CORE_ALLOW_OFF,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_ON,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_OFF,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_OFF,
  HAL_ENUM_32BITS(CLK_FOOTSWITCH_CONFIG)
} HAL_clk_FootswitchConfigType;


/*
 * HAL_clk_Context
 *
 * HAL clock driver context data.
 *
 *  BusyWait      - Pointer to busy wait function.
 *                  Assumption: Will be implemented by the top level driver
 *                  whether in DAl or via clk_busy_wait. If this function
 *                  pointer is NULL then HAL modules that require a
 *                  wait/pause may fail - this will be functional failure
 *                  and not compile time failure.
 *
 *  nChipVersion  - Version of the chip. Compare against 
 *                  HAL_CLK_CHIP_VERSION(major, minor).
 *  nChipId       - ID of the chip.
 */
typedef struct
{
  void (*BusyWait) ( uint32 uSeconds );
  uint32 nChipVersion;
  uint32 nFamily;
} HAL_clk_ContextType;


/*
 * HAL_clk_HWIOBaseType
 *
 * Container for a HWIO base address.  This is used by the driver to
 * map the required physical bases to the provided base pointer.
 *
 *  nPhysAddr  - Physical base address of the base.
 *  nSize      - Size of the memory region in bytes.
 *  pnBaseAddr - Pointer to location for the driver to fill in with
 *               the virtual address.
 */
typedef struct
{
  uint32  nPhysAddr;
  uint32  nSize;
  uint32 *pnBaseAddr;
} HAL_clk_HWIOBaseType;


/*
 * Clock handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_ClockHandleType
 */
typedef void *HAL_clk_ClockHandleType;

/*
 * Clock domain handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_ClockDomainHandleType
 */
typedef void *HAL_clk_ClockDomainHandleType;

/*
 * Power domain handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_PowerDomainHandleType
 */
typedef void *HAL_clk_PowerDomainHandleType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


/* ============================================================================
**    Mandatory Functions
** ==========================================================================*/

void    HAL_clk_Init                    ( HAL_clk_ContextType *pHALClkCtxt );
void    HAL_clk_Reset                   ( void );
void    HAL_clk_Save                    ( void );
void    HAL_clk_Restore                 ( void );

void    HAL_clk_EnableClock             ( HAL_clk_ClockHandleType pClockHandle );
void    HAL_clk_DisableClock            ( HAL_clk_ClockHandleType pClockHandle );
void    HAL_clk_ConfigClock             ( HAL_clk_ClockHandleType pClockHandle, HAL_clk_ClockConfigType eConfig );
void    HAL_clk_ConfigClockDivider      ( HAL_clk_ClockHandleType pClockHandle, uint32 nDiv );
void    HAL_clk_ConfigFootswitch        ( HAL_clk_ClockHandleType pClockHandle, HAL_clk_FootswitchConfigType eFSConfig );
void    HAL_clk_ResetClock              ( HAL_clk_ClockHandleType pClockHandle, boolean bAssert );
boolean HAL_clk_IsClockSupported        ( HAL_clk_ClockHandleType pClockHandle, HAL_clk_ClockAPIType eAPI );
boolean HAL_clk_IsClockEnabled          ( HAL_clk_ClockHandleType pClockHandle );
boolean HAL_clk_IsClockOn               ( HAL_clk_ClockHandleType pClockHandle );

void    HAL_clk_CalcClockFreq           ( HAL_clk_ClockHandleType pClockHandle, uint32 *pnFreqHz );
void    HAL_clk_DetectClockConfig       ( HAL_clk_ClockHandleType pClockHandle, uint32 *pnConfig, boolean *pbValid );
void    HAL_clk_DetectClockDivider      ( HAL_clk_ClockHandleType pClockHandle, uint32 *pnDiv );

void    HAL_clk_SaveClockState          ( void );
void    HAL_clk_ResetClockState         ( void );
void    HAL_clk_RestoreClockState       ( void );

void    HAL_clk_ClockSleepEnable        ( HAL_clk_ClockHandleType pClockHandle );
void    HAL_clk_ClockSleepReset         ( void );

void    HAL_clk_ConfigClockMux          ( HAL_clk_ClockDomainHandleType pClockDomainHandle, const HAL_clk_ClockMuxConfigType *pmConfig );
void    HAL_clk_DetectClockMuxConfig    ( HAL_clk_ClockDomainHandleType pClockDomainHandle, HAL_clk_ClockMuxConfigType *pmConfig );
boolean HAL_clk_IsClockDomainSupported  ( HAL_clk_ClockDomainHandleType pClockDomainHandle );

void    HAL_clk_EnableSource            ( HAL_clk_SourceType eSource );
void    HAL_clk_DisableSource           ( HAL_clk_SourceType eSource );
void    HAL_clk_EnableSourceVote        ( HAL_clk_SourceType eSource );
void    HAL_clk_DisableSourceVote       ( HAL_clk_SourceType eSource );
void    HAL_clk_ConfigSource            ( HAL_clk_SourceType eSource, HAL_clk_SourceConfigType eConfig );
boolean HAL_clk_IsSourceEnabled         ( HAL_clk_SourceType eSource );
boolean HAL_clk_IsSourceVoteEnabled     ( HAL_clk_SourceType eSource );
void    HAL_clk_ConfigPLL               ( HAL_clk_SourceType eSource, const HAL_clk_PLLConfigType *pmConfig );
void    HAL_clk_DetectPLLConfig         ( HAL_clk_SourceType eSource, HAL_clk_PLLConfigType *pmConfig );

void    HAL_clk_EnablePowerDomain       ( HAL_clk_PowerDomainHandleType pPowerDomainHandle, boolean bAsync );
void    HAL_clk_DisablePowerDomain      ( HAL_clk_PowerDomainHandleType pPowerDomainHandle );
boolean HAL_clk_IsPowerDomainSupported  ( HAL_clk_PowerDomainHandleType pPowerDomainHandle );
boolean HAL_clk_IsPowerDomainEnabled    ( HAL_clk_PowerDomainHandleType pPowerDomainHandle );

void    HAL_clk_GetHWIOBases            ( HAL_clk_HWIOBaseType **paHWIOBases );

HAL_clk_ClockHandleType HAL_clk_GetNextClockInDomain(HAL_clk_ClockDomainHandleType pClockDomainHandle, uint32 nClockIndex);
void    HAL_clk_GetNextClockDomain      ( HAL_clk_ClockDomainHandleType pClockDomainHandle, HAL_clk_ClockDomainHandleType *pNextClockDomainHandle );
void    HAL_clk_GetNextPowerDomain      ( HAL_clk_PowerDomainHandleType pPowerDomainHandle, HAL_clk_PowerDomainHandleType *pNextPowerDomainHandle );

uint32  HAL_clk_GetNumberOfClocks       ( void );
uint32  HAL_clk_GetNumberOfClockDomains ( void );
uint32  HAL_clk_GetNumberOfPowerDomains ( void );

void    HAL_clk_GetClockName            ( HAL_clk_ClockHandleType pClockHandle, const char **szClockName );
void    HAL_clk_GetPowerDomainName      ( HAL_clk_PowerDomainHandleType pPowerDomainHandle, const char **szPowerDomainName );

uint32  HAL_clk_GetChipVersion          ( void );
uint32  HAL_clk_GetChipFamily               ( void );
uint32  HAL_clk_GetTestClockId          ( HAL_clk_ClockHandleType pClockHandle );

void    HAL_clk_GFXRailPowerDown        ( void );
void    HAL_clk_GFXRailPowerUp          ( void );
void    HAL_clk_OCMEMSave               ( void );
void    HAL_clk_OCMEMRestore            ( void );
void    HAL_clk_CollapsedMMCoreMemory   ( void );
void    HAL_clk_RestoredMMCoreMemory    ( void );
boolean HAL_clk_OxiliMemOn              ( void );
uint32  HAL_clk_BIMCGetMiscInfo         ( void );
void    HAL_clk_BIMCSetMiscInfo         ( uint32 nValue );
void    HAL_clk_BIMCToggleJCPLL         ( void );
void    HAL_clk_MiscClkDisableDeepSleep ( void );
void    HAL_clk_MiscClkRestoreDuringWakeup ( void );
uint32  HAL_clk_GetDEHRStatus           ( void );
boolean HAL_clk_IsDEHREnable            ( void );
void HAL_clk_GPLL0_disable_deep_sleep   ( void );
void HAL_clk_GPLL0_FSM_reset_deep_sleep ( void );
void HAL_clk_GPLL1_disable_deep_sleep   ( void );
void HAL_clk_GPLL1_FSM_reset_deep_sleep ( void );
uint32 HAL_clk_GetHWParNum(void);

/*
==============================================================================

               DOCUMENTATION FOR MODULE

==============================================================================

HAL_clk_Init( )

Description:
  Initialize the clock HAL data structures and clock hardware.

==============================================================================

HAL_clk_GetHWIOBases( )

Description:
  Retrieve the array of HWIO bases used in this platform.  This API is
  used to map the required physical regions into memory and should be
  invoked prior to HAL_clk_Init.

==============================================================================

HAL_clk_Reset( )

Description:
  Reset the clock hardware to the power-on state.

==============================================================================

HAL_clk_Save( )

Description:
  Save the states of the clocks.

==============================================================================

HAL_clk_Restore( )

Description:
  Restore the states of the clocks.

==============================================================================

HAL_clk_EnableClock( )

Description:
  Enable the given clock.  Note that the source should be enabled before
  invoking this function.

Parameters:
  eClock - The clock to enable.

==============================================================================

HAL_clk_DisableClock( )

Description:
  Disable the given clock.

Parameters:
  eClock - The clock to disable.

==============================================================================

HAL_clk_ConfigClock( )

Description:
  Configures the given clock to the given configuration.  Typically the
  configuration will be a clock rate.  The argument is a clock-dependent
  enumeration.

Parameters:
  eClock  - The clock to configure.
  nConfig - The configuration to use (i.e. HAL_clk_UARTConfigType)

==============================================================================

HAL_clk_ConfigClockDivider( )

Description:
  Configures the given clock with the given divider value.

Parameters:
  nDiv    - The divider value to use (in mathematical notation as opposed
             to raw register field value, i.e. "1" = div-1)

==============================================================================

HAL_clk_ConfigFootswitch( )

Description:
  Configures the given clock with the given footswitch setting.

Parameters:
  eFSConfig - Footswitch configuration.

==============================================================================

HAL_clk_ResetClock( )

Description:
  Asserts or de-asserts the reset signal to the given clock hardware block.

Parameters:
  pClockHandle - The handle of the clock to reset.
  bAssert      - Whether to assert or de-assert the reset signal.

==============================================================================

HAL_clk_IsClockSupported( )

Description:
  Checks if the given clock API is supported for the given clock.

Parameters:
  pClockHandle - The handle of the clock in question.
  eAPI         - Which API to verify support of.

Return:
  boolean - TRUE if the clock API is supported.

==============================================================================

HAL_clk_IsClockEnabled( )

Description:
  Checks if the given clock is enabled.  Note that this simply means the
  HW gate is enabled, not that the clock is actually toggling.  To see if
  the clock is active, use IsClockOn.

Parameters:
  pClockHandle - The handle of the clock to check.

Return:
  boolean - TRUE if the clock is enabled.

==============================================================================

HAL_clk_IsClockOn( )

Description:
  Checks if the given clock is running.  If the clock source is not active
  then even if the clock is enabled it may not be "on".

Parameters:
  pClockHandle - The handle of the clock to check.

Return:
  boolean - TRUE if the clock is on.

==============================================================================

HAL_clk_CalcClockFreq( )

Description:
  This function calculates the current clock frequency for the given clock.
  This is done using internal loop back clock test registers which may not
  be supported on a particular target.

Parameters:
  pClockHandle - The handle of the clock to check.
  pnFreqHz     - The calculated clock frequency in Hz.

Return:
  See pnFreqHz.

==============================================================================

HAL_clk_DetectClockConfig( )

Description:
  Determines the current clock configuration based on the hardware
  settings.  Note that most clocks probably do not implement this
  functionality.

Parameters:
  pClockHandle - The handle of the clock to check.

Return:
  pnConfig - The determined configuration.
  pbValid  - Whether or not the configuration was successfully determined.
             If the current hardware state is not in a supported
             configuration this will be FALSE and pnConfig should be
             ignored.

==============================================================================

HAL_clk_DetectClockDivider( )

Description:
  Determines the current clock configuration based on the hardware
  settings.  Note that most clocks probably do not implement this
  functionality.

Parameters:
  pClockHandle - The handle of the clock to check.

Return:
  pnDiv - The determined divider value.


==============================================================================

HAL_clk_SaveClockState( )

Description:
  Saves the current state of the clocks.  Does not save source or rail state.
  Is only guaranteed to save the state that is lost during
  HAL_clk_ResetClockState, not necessarily lost during power collapse.
  This function is intended to be used before TCXO shutdown when all active
  clocks must be disabled via HAL_clk_ResetClockState.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_Save, HAL_clk_ResetClockState, HAL_clk_RestoreClockState

==============================================================================

HAL_clk_ResetClockState( )

Description:
  Resets (disables) the clocks.  Does not touch source or rail state.
  This function is intended to be used before TCXO shutdown when all active
  clocks must be disabled.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_SaveClockState, HAL_clk_RestoreClockState

==============================================================================

HAL_clk_RestoreClockState( )

Description:
  Restores the clock state saved with HAL_clk_SaveClockState.
  This function is intended to be used after TCXO shutdown to restore clocks
  disabled with HAL_clk_ResetClockState.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_SaveClockState, HAL_clk_ResetClockState

==============================================================================

HAL_clk_ClockSleepEnable()

Description:
  Enables sleep register for the given clock.  This will halt the clock when
  corresponding processor goes to SWFI/halt state.

Parameters:
  pClockHandle - The handle of the clock to check.

Return:
  None

==============================================================================

HAL_clk_ClockSleepReset()

Description:
  Resets sleep registers.  This function will vary based on the target since
  sleep registers are target specific.

Parameters:
  None

Return:
  None

==============================================================================

HAL_clk_ConfigClockMux

Description:
  Configures clock MUX for the given clock domain.

Parameters:
  pClockDomainHandle - The handle to clock domain to configure.
  pmConfig           - The configuration to use.

Return:
  None

==============================================================================

HAL_clk_DetectClockMuxConfig

Description:
  Detects clock MUX configuration for the given clock domain.

Parameters:
  pClockDomainHandle - The handle to clock domain to check

Return:
  pmConfig - The determined configuration

==============================================================================

HAL_clk_IsClockDomainSupported

Description:
  Checks if the given clock domain is supported.

Parameters:
  pClockDomainHandle - The handle to clock domain to check

Return:
  boolean - TRUE if the clock domain is supported.

==============================================================================


HAL_clk_EnableSource( )

Description:
  Enable the given source.  Does nothing if the source is not supported.

Parameters:
  eSource - The source to enable.

==============================================================================

HAL_clk_DisableSource( )

Description:
  Disable the given source.  Does nothing if the source is not supported.

Parameters:
  eSource - The source to disable.

==============================================================================

HAL_clk_EnableSourceVote( )

Description:
  Enables the given source vote.

Parameters:
  eSource - The source to enable vote for.

==============================================================================

HAL_clk_DisableSourceVote( )

Description:
  Disables the given source vote.

Parameters:
  eSource - The source to disable vote for.

==============================================================================

HAL_clk_ConfigSource( )

Description:
  Configures the given source to the given configuration.

Parameters:
  eSource - The source to configure.
  nConfig - The generic configuration to use, the actual type will depend
            on the type of source being configured.

==============================================================================

HAL_clk_IsSourceEnabled( )

Description:
  Checks if the given source is enabled.

Parameters:
  eSource - The source to check.

Return:
  boolean - TRUE if the source is enabled.

==============================================================================

HAL_clk_IsSourceVoteEnabled( )

Description:
  Checks if the given source vote is enabled.

Parameters:
  eSource - The source to check.

Return:
  boolean - TRUE if the source vote is enabled.

==============================================================================

HAL_clk_ConfigPLL( )

Description:
  Configures the given source, which must be a PLL, to the given
  configuration.

Parameters:
  eSource  - The source to configure (must be a PLL).
  pmConfig - The configuration to use.

==============================================================================

HAL_clk_DetectPLLConfig( )

Description:
  Detects the current configuration of the given PLL source.  The eSource
  field of the result will be set to HAL_CLK_SOURCE_NULL on failure.

Parameters:
  eSource  - The source whose configuration to detect (must be a PLL).
  pmConfig - The detected configuration.

==============================================================================

HAL_clk_EnablePowerDomain( )

Description:
  Enable the given power domain.  Does nothing if the power domain is not
  supported. Note that this is merely the clock portion of the power domain
  control.  Generally bus and possibly PMIC interactions will also be required.

Parameters:
  pPowerDomainHandle - The handle to the power domain to enable.

==============================================================================

HAL_clk_DisablePowerDomain( )

Description:
  Disable the given power domain.  Does nothing if the power domain is not
  supported.  Note that this is merely the clock portion of the power domain
  control.  Generally bus and possibly PMIC interactions will also be required.

Parameters:
  pPowerDomainHandle - The handle to the power domain to disable.

==============================================================================

HAL_clk_IsPowerDomainSupported( )

Description:
  Checks if the given power domain is supported.

Parameters:
 pPowerDomainHandle - The handle to the power domain to check.

Return:
  boolean - TRUE if the power domain is supported on this target.

==============================================================================

HAL_clk_IsPowerDomainEnabled( )

Description:
  Checks if the given power domain is enabled.

Parameters:
  pPowerDomainHandle - The handle to the power domain to check.

Return:
  boolean - TRUE if the power domain is supported on this target.

==============================================================================

HAL_clk_GetNextClockInDomain ( )

Description:
  Returns a pointer to the queried (based on nClockCount) clock of this domain.

Parameters:
  pClockDomainHandle - The handle to the clock domain.
  nClockIndex        - The clock for which to get HAL handle for.

Return:
  pClockHandle - The handle to the queried (based on nClockCount) clock of this domain.

==============================================================================

HAL_clk_GetNextClockDomain( )

Description:
  Returns a pointer to the next (or first) clock domain.

Parameters:
  pClockDomainHandle - The handle to the clock domain.

Return:
  pNextClockDomainHandle - The handle to the next (or first) clock domain.

==============================================================================

HAL_clk_GetNextPowerDomain( )

Description:
  Returns a pointer to the next (or first) power domain.

Parameters:
  pPowerDomainHandle - The handle to the power domain.

Return:
  pNextPowerDomainHandle - The handle to the next (or first) power domain.

==============================================================================

HAL_clk_GetClockName( )

Description:
  Returns a name of the given clock.

Parameters:
  pClockHandle - The handle to the clock.

Return:
  szClockName  - The name of the clock.

==============================================================================

HAL_clk_GetPowerDomainName( )

Description:
  Returns the name of the given power domain.

Parameters:
  pPowerDomainHandle - The handle to the power domain.

Return:
  szPowerDomainName  - The name of the power domain.

==============================================================================

HAL_clk_GetChipVersion( )

Description:
  Returns the chip version.

==============================================================================

HAL_clk_GetChipFamily( )

Description:
  Returns the chip family.

==============================================================================

HAL_clk_GetTestClockId( )

Description:
  Returns the Test Clock ID.

==============================================================================

*/

#endif /* !HALCLK_H */

