#ifndef __HALCLKINTERNAL_H__
#define __HALCLKINTERNAL_H__
/*
==============================================================================

FILE:         HALclkInternal.h

DESCRIPTION:
  Internal definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/v2/HALclkInternal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
09/14/12   ll      Added PLL Type (SR, SR2) field to PLL Context typedef
07/18/11   vs      Ported from the latest clock driver.

==============================================================================
            Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALclk.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * NOT_2D / NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * in a type 1 mux structure.  Used to prepare the value for writing
 * to the hardware register field.
 */
#define NOT_2D(mux)         (~(mux)->n2D)
#define NOT_N_MINUS_M(mux)  (~((mux)->nN - (mux)->nM))

/*
 * DIVIDER
 *
 * Macro to return the normalized divider for a given mux structure.
 * Just subtracts 1 unless the value is already 0.
 */
#define DIVIDER(mux)  ((mux)->nDivider ? (mux)->nDivider - 1 : 0)

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * HAL_CLK_FMSK
 *
 * Define a register mask data structure given the register name
 * and field.
 */
#define HAL_CLK_FMSK(io, field)   \
  {                               \
    HWIO_OFFS(io),                \
    HWIO_FMSK(io, field)          \
  }


/*
 * HAL_CLK_FMSKI
 *
 * Define a register mask data structure given the index, register name
 * and field.
 */
#define HAL_CLK_FMSKI(io,index,field)   \
  {                               \
    HWIO_OFFSI(io,index),         \
    HWIO_FMSK(io, field)          \
  }


/*
 * HAL_CLK_FMSKM
 *
 * Define a register mask data structure given the register name
 * and a mask.
 */
#define HAL_CLK_FMSKM(io, mask)   \
  {                               \
    HWIO_OFFS(io),                \
    mask                          \
  }


/*
 * Define an invalid source index for error detection.
 */
#define HAL_CLK_SOURCE_INDEX_INVALID 0xFFFFFFFF


/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_clk_InitFuncType
 *
 * Prototype for a generic module initialization function.
 */
typedef void (*HAL_clk_InitFuncType)(void);


/*
 * HAL_clk_PlatformType
 *
 * Platform information structure.  Each image defines one of these and
 * the core code uses it during initialization.
 *
 * afInitFuncs - Array of initialization functions.
 * aHWIOBases  - Array of HWIO bases required for this platform.
 */
typedef struct
{
  HAL_clk_InitFuncType *afInitFuncs;
  HAL_clk_HWIOBaseType *aHWIOBases;
} HAL_clk_PlatformType;


/*
 * Forward declaration of structs b/c of the cross references within these 
 * structs 
 */ 
typedef struct HAL_clk_ClockDescType       HAL_clk_ClockDescType;
typedef struct HAL_clk_ClockDomainDescType HAL_clk_ClockDomainDescType;
typedef struct HAL_clk_SourceDescType      HAL_clk_SourceDescType;
typedef struct HAL_clk_PowerDomainDescType HAL_clk_PowerDomainDescType;
typedef struct HAL_clk_DebugDescType       HAL_clk_DebugDescType;
  
/*
 * HAL_clk_RegisterMaskType
 *
 * Contains a register address and mask, used for setting and clearing
 * a given set of bits.
 *
 * nAddr - Address of the register (32-bit).
 * nMask - 32-bit mask.
 */
typedef struct
{
  uint32 nAddr;
  uint32 nMask;
} HAL_clk_RegisterMaskType;


/*
 * HAL_clk_ClockRegistersType
 *
 * nBranchAddr  - Clock branch address/offset.
 * nResetAddr   - Clock block reset address/offset.
 * mHWVotable   - Register/mask for HW votable configuration setup.
 */
typedef struct
{
  uint32                    nBranchAddr;
  uint32                    nResetAddr;
  HAL_clk_RegisterMaskType  mHWVotable;
} HAL_clk_ClockRegistersType;


/*
 * HAL_clk_ClockControlType
 *
 * Set of function pointers used to control a clock.
 *
 * Enable           - Enable the given clock.
 * Disable          - Disable the given clock.
 * IsEnabled        - Check if the given clock is enabled.
 * IsOn             - Check if the given clock is on.
 * Reset            - Assert or deassert the reset for given clock.
 * Config           - Configure the given clock.
 * DetectConfig     - Return the current configuration for the clock.
 * ConfigDivider    - Set the divider for the clock in question.
 * DetectDivider    - Return the current divider for the clock in question.
 * ConfigFootswitch - Configure the clock footswitch.
 */
typedef struct
{
  void    (*Enable)           ( HAL_clk_ClockDescType  *pmClockDesc );
  void    (*Disable)          ( HAL_clk_ClockDescType  *pmClockDesc );
  boolean (*IsEnabled)        ( HAL_clk_ClockDescType  *pmClockDesc );
  boolean (*IsOn)             ( HAL_clk_ClockDescType  *pmClockDesc );
  void    (*Reset)            ( HAL_clk_ClockDescType  *pmClockDesc, boolean bAssert );
  void    (*Config)           ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_ClockConfigType eConfig );
  void    (*DetectConfig)     ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnConfig, boolean *pbValid );
  void    (*ConfigDivider)    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 nDiv );
  void    (*DetectDivider)    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnDiv );
  void    (*ConfigFootswitch) ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_FootswitchConfigType eConfig );
} HAL_clk_ClockControlType;


/*
 * HAL_clk_SourceControlType
 *
 * Set of function pointers used to control a clock source.
 *
 * Enable          - Enable the given source.
 * Disable         - Disable the given source.
 * IsEnabled       - Check if the given source is enabled.
 * Config          - Configure the given source.
 * ConfigPLL       - Configure the given PLL source parameters.
 * DetectPLLConfig - Get the current configuration for the source.
 * GetFreq         - Get the frequency for the given source and config.
 * EnableVote      - Enable vote the given source.
 * DisableVote     - Disable vote the given source.
 * IsVoteEnabled   - Check if the given source vote is enabled.
 */
typedef struct
{
  void    (*Enable)          ( HAL_clk_SourceDescType *pSource );
  void    (*Disable)         ( HAL_clk_SourceDescType *pSource );
  boolean (*IsEnabled)       ( HAL_clk_SourceDescType *pSource );
  void    (*Config)          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
  void    (*ConfigPLL)       ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig );
  void    (*DetectPLLConfig) ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
  void    (*EnableVote)      ( HAL_clk_SourceDescType *pSource );
  void    (*DisableVote)     ( HAL_clk_SourceDescType *pSource );
  boolean (*IsVoteEnabled)   ( HAL_clk_SourceDescType *pSource );
} HAL_clk_SourceControlType;


/*
 * HAL_clk_SourceMapType
 *
 * eSource      - Source enum value
 * nSourceIndex - Value written to register to represent the source
 */
typedef struct
{
  HAL_clk_SourceType eSource;
  uint32             nSourceIndex;
} HAL_clk_SourceMapType;


/*
 * HAL_clk_ClockDomainControlType
 * 
 * ConfigMux         - Configure the given clock mux.
 * DetectMuxConfig   - Return the current configuration of the mux for the clock. 
 * pmSourceMap       - Pointer to source mapping array
 */
typedef struct
{
  void               (*ConfigMux)       ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, const HAL_clk_ClockMuxConfigType *pmConfig );
  void               (*DetectMuxConfig) ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
  HAL_clk_SourceMapType  *pmSourceMap;
} HAL_clk_ClockDomainControlType;


/*
 * HAL_clk_PowerDomainControlType
 * 
 * Enable    - Enable power domain.
 * Disable   - Disable power domain.
 * IsEnabled - Check whether power domain is enabled.
 */
typedef struct
{
  void    (*Enable)     ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc, boolean bAsync );
  void    (*Disable)    ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  boolean (*IsEnabled)  ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  boolean (*IsOn)       ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
} HAL_clk_PowerDomainControlType;


/*
 * HAL_clk_ClockDescType
 *
 * Description of a clock, including required data to control the
 * clock.
 *
 * szClockName - Pointer to the clock (string) name
 * mRegisters  - (optional) Set of registers used by the
 *               common clock control functions (HAL_clk_ClockControl*).
 * pmControl   - Pointer to the control functions for this clock.  If NULL
 *               then the clock is not supported.
 * nTestClock  - Chipset and subsystem specific value for programming
 *               the debug circuitry to measure this clock.  Generally
 *               from the HALclkDebugClock.h chipset include file.
 *               
 */
struct HAL_clk_ClockDescType
{
  const char                    *szClockName;
  HAL_clk_ClockRegistersType    mRegisters;
  HAL_clk_ClockControlType      *pmControl;
  uint32                        nTestClock;
};


/*
 * HAL_clk_ClockDomainDescType
 *
 * Description of a clock domain, including required data to control the
 * clock domain.
 * 
 * nCGRAddr          - (First) Clock generation register address/offset.
 * pmClocks          - Pointer to an array of clocks constituting the domain
 *                     under this clock.  All clocks in the domain are affected
 *                     when the master is configured.  The array terminates with NULL.
 * nClockCount       - Number of clocks in this domain.
 * pmControl         - Pointer to the control functions for this domain.
 * pmNextClockDomain - Next clock domain (will be connected in the install domain
 *                     function), if any, otherwise NULL.
 */
struct HAL_clk_ClockDomainDescType
{
  uint32                             nCGRAddr;
  HAL_clk_ClockDescType              *pmClocks;
  uint32                             nClockCount;
  HAL_clk_ClockDomainControlType     *pmControl;
  struct HAL_clk_ClockDomainDescType *pmNextClockDomain;
};


/*
 *  HAL_clk_PLLType
 *
 *  PLL Type List.
 */
typedef enum
{
  HAL_CLK_PLL_PRIUS,
  HAL_CLK_PLL_TESLA,

  HAL_ENUM_32BITS(CLK_PLL_TYPE)
} HAL_clk_PLLType;


/*
 * HAL_clk_PLLContextType
 *
 * PLL context information.
 *
 * nAddr         - Address of the PLL control registers.
 * VoterRegister - Address and mask of the voting register for this PLL for the
 *                 current processor.
 * ePLLType      - PLL Type enum, e.g. SR, SR2
 */
typedef struct
{
  uint32                   nAddr;
  HAL_clk_RegisterMaskType VoterRegister;
  HAL_clk_PLLType          ePLLType;
} HAL_clk_PLLContextType;


/*
 * HAL_clk_SourceDescType
 *
 * Description of a clock source, including control functions.
 *
 * eSource    - Source number.
 * pControl   - Pointer to the control functions for this source.  If NULL
 *               then the source is not supported.
 * pContext   - Pointer to any context for managing this source.  For PLLs
 *              will generally be of type HAL_clk_PLLContextType.
 */
struct HAL_clk_SourceDescType
{
  HAL_clk_SourceType         eSource;
  HAL_clk_SourceControlType *pControl;
  void                      *pContext;
};


/*
 * HAL_clk_PowerDomainDescType
 *
 * Description of a power domain, including control functions.
 *
 * szPowerDomainName    - Pointer to the power domain (string) name.
 * nGDSCRAddr           - Global distributed switch controller (GDSC) register address.
 * pmControl            - Pointer to the control functions for this power domain.  If
 *                        NULL then the power domain is not supported. 
 * pmNextPowerDomain    - Next power domain (will be connected in the install domain
 *                        function), if any, otherwise NULL.
 */
struct HAL_clk_PowerDomainDescType
{
  const char                           *szPowerDomainName;
  uint32                               nGDSCRAddr;
  HAL_clk_PowerDomainControlType       *pmControl;
  struct HAL_clk_PowerDomainDescType   *pmNextPowerDomain;
};


/*
 * HAL_clk_DebugDescType
 *
 * Description of clock debug parameters for a given sub-system.
 * 
 *  nTestDomainID             -  Test domain ID - should match a value in HALclkTest.h.
 *  nTestDomainClkSelect      -  Clock select value programmed into CLK_TEST register for a given domain.
 *  SetupDebugRegisters       -  Setup sub-system specific debug registers.
 *  pmNextDebugDesc           -  Next debug descriptor, if any.
 */
struct HAL_clk_DebugDescType
{
  uint32                          nTestDomainID;
  uint32                          nTestDomainClkSelect;
  void                           (*SetupDebugRegisters) ( HAL_clk_ClockDescType  *pmClockDesc );
  struct HAL_clk_DebugDescType   *pmNextDebugDesc;
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


boolean HAL_clk_WaitForClockOn   ( HAL_clk_ClockDescType  *pmClockDesc );
boolean HAL_clk_WaitForClockOff  ( HAL_clk_ClockDescType  *pmClockDesc );
void    HAL_clk_BusyWait         ( uint32 uSec );

void HAL_clk_InstallClockDomain  ( HAL_clk_ClockDomainDescType * pmClockDomainDesc, uint32 nBaseAddress );
void HAL_clk_InstallClockDomains ( HAL_clk_ClockDomainDescType ** pmClockDomainDesc, uint32 nBaseAddress );
void HAL_clk_InstallSource       ( HAL_clk_SourceType eSource, HAL_clk_SourceControlType *pControl, void *pContext);
void HAL_clk_InstallPowerDomain  ( HAL_clk_PowerDomainDescType * pmPowerDomainDesc, uint32 nBaseAddress );
void HAL_clk_InstallPowerDomains ( HAL_clk_PowerDomainDescType ** pmPowerDomainDesc, uint32 nBaseAddress );
void HAL_clk_InstallDebugDesc    ( HAL_clk_DebugDescType * pmDebugDesc );
void HAL_clk_GetNextDebugDesc    ( HAL_clk_DebugDescType  *pmDebugDesc, HAL_clk_DebugDescType **pmNextDebugDesc);
void HAL_clk_EnableClockDomain   ( HAL_clk_ClockDomainHandleType pClockDomainHandle, uint32 *pnMask );
void HAL_clk_DisableClockDomain  ( HAL_clk_ClockDomainHandleType pClockDomainHandle, uint32 *pnMask );


/*
==============================================================================

               DOCUMENTATION FOR MODULE

==============================================================================
 
 
 
 
==============================================================================
 
HAL_clk_WaitForClockOn

Description:
  Waits up to 100 us for the given clock to turn on.  Returns TRUE if
  the clock enabled, FALSE otherwise.

============================================================================== 
 
HAL_clk_WaitForClockOff

Description:
  Waits up to 100 us for the given clock to turn off.  Returns TRUE if
  the clock disabled, FALSE otherwise.

==============================================================================

HAL_clk_EnableClockDomain

Description:
  Enable all clocks in the given clock domain. 

Parameters:
  pClockDomainHandle  - The clock domain to enable.
  pnMask              - Input value:  Which clocks to enable.
                        Output value: Which clocks were originally disabled.

============================================================================== 
 
HAL_clk_DisableClockDomain

Description:
  Disable all clocks in the given clock domain. 

Parameters:
  pClockDomainHandle  - The clock domain to enable.
  pnMask              - Input value:  Which clocks to disable.
                        Output value: Which clocks were originally enabled.

============================================================================== 

HAL_clk_InstallClockDomain 
 
Description:
  Installs (connects) the given clock domain to other clock domains via linked
  list.
 
==============================================================================

HAL_clk_InstallClockDomains

Description:
  Installs (connects) the given array of clock domains to other clock domains
  via linked list (internally calls HAL_clk_InstallClockDomain).

==============================================================================
 
HAL_clk_InstallSource 
 
Description: 
  Installs the given source into a global source array. 
 
==============================================================================
 
HAL_clk_InstallPowerDomain
 
Description:
  Installs (connects) the given power domain to other power domains via linked
  list.
 
============================================================================== 

HAL_clk_InstallPowerDomains

Description:
  Installs (connects) the given array of power domains to other power domains
  via linked list (internally calls HAL_clk_InstallPowerDomain).

==============================================================================
 
HAL_clk_BusyWait
 
Description:
  Provides busy wait capability via internal function pointer.
 
============================================================================== 
 
HAL_clk_PlatformInit

Description:
  Platform initialization entry-point.

==============================================================================

*/

#endif /* !HALCLKINTERNAL_H */

