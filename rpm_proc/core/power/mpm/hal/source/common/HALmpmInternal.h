#ifndef HAL_MPM_INTERNAL_H
#define HAL_MPM_INTERNAL_H
/*
===========================================================================

FILE:         HALmpmintInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the MPM
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/common/HALmpmInternal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/09/12   mda     MPMv2 for 8974

===========================================================================
             Copyright © 2008-2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "BSPmpm.h"


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_mpm_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_mpm_PlatformEarlyInit.
 *
 *  InitLate:      Optional function executed at the end of HAL_pic_Init.
 *  Save:          Optional function executed at the end of HAL_pic_Save.
 *  Restore:       Optional function executed at the start of
 *                 HAL_pic_Restore.
 */
typedef struct
{
  HAL_mpm_CfgType *mpmCfg;

  void   (*InitLate)(void);
  void   (*Save)(void);
  void   (*Restore)(void);

} HAL_mpm_PlatformType;

#define HAL_MPM_MAX_SSBI_PORT_NUM    2

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
                                       
void HAL_mpm_PlatformInitEarly (HAL_mpm_PlatformType *pmPlatform);

void HAL_mpm_ConfigDebugBus            ( bool32 enable );
void HAL_mpm_ConfigDebugBus_1          ( bool32 enable );
                                       
void HAL_mpm_CfgFreezeClampCtl         ( bool32 disableFreezeIOs, 
                                         bool32 disableClampIOs);
                                       
void HAL_mpm_CfgSwEbi1IoCntl           ( bool32 ebi1IoSwCtlEnable );
                                       
void HAL_mpm_DebugThroughPC            ( bool32 enable );
                                       
/*
===============================================================================
  INTERFACES DOCUMENTATION
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicSbiAddr()
   This procedure configures the slave ID and PMIC register address.  This
   function is empty for targets that have a field for the 2-bit address prefix
   for the slave used in SSBI 2.0.  For those targets, use 
   HAL_mpm_CfgPmicSbiAddr_withPrefix().

Parameters:
   slaveId:   SSBI/SBI slave ID for PMIC
   slaveAddr: PMIC MSMC1 voltage regulator SBI register address

Return value: None

Dependencies: HAL_mpm_CfgPmicSbiPort() should be called to complete PMIC i/f
              configuration

Side effects: None

See Also: HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgPmicSbiAddr_withPrefix()
   This procedure configures the slave ID and PMIC register address.  This
   function is empty for targets that don't have a field for the 2-bit address 
   prefix for the slave used in SSBI 2.0.  For those targets, use 
   HAL_mpm_CfgPmicSbiAddr().

Parameters:
   slaveAddrPrevix: The 2-bit address prefix for slave used in SSBI 2.0
   slaveId:         SSBI/SBI slave ID for PMIC
   slaveAddr:       PMIC MSMC1 voltage regulator SBI register address

Return value: None

Dependencies: HAL_mpm_CfgPmicSbiPort() should be called to complete PMIC i/f
              configuration

Side effects: None

See Also: HAL_mpm_CfgPmicSbiPort()
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_ConfigDebugBus()
   This procedure configures the debug bus to monitor various MPM signal for
   MPM hardware functional verification.  This function configures bit 0
   of MPM_DEBUG_BUS_EN.  On targets that support bit 1 of that register, when
   bit 0 is enabled, bit 1 is automatically disabled in this function.

Parameters:
   enable: TRUE  - Enable debug bus
           FALSE - Disable debug bus

Return value: None

Dependencies: Need to configure Modem debug bus to be able to monitor these 
              signals

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_ConfigDebugBus_1()
   This procedure configures the debug bus to monitor various MPM signal for
   MPM hardware functional verification.  This function configures bit 1
   of MPM_DEBUG_BUS_EN.  This function is empty on targets that don't support
   bit 1 of that register.  On targets that do support it, when bit 1 is 
   enabled, bit 0 is automatically disabled in this function.

Parameters:
   enable: TRUE  - Enable debug bus
           FALSE - Disable debug bus

Return value: None

Dependencies: Need to configure Modem debug bus to be able to monitor these 
              signals

Side effects: None

See Also:
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgFreezeClampCtl()
   This function is used to enable or disable the clamp and freeze functionality
   in the MPM HW.

Parameters: 
   disableFreezeIOs: TRUE  - Disables all freeze_io_* signals
                     FALSE - Enables all freeze_io_* signals (default)
   disableClampIOs:  TRUE  - Disables all clamp_io_* signals
                     FALSE - Enables all clamp_io_* signals (default)                    

Return value: None

Dependencies: None

Side effects: None

See Also: HAL_mpm_CfgMpmSlpProc
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_CfgSwEbi1IoCntl()
   This function is used to enable or disable software control of 
   freeze_io_ebi1.

Parameters: 
   ebi1IoSwCtlEnable: TRUE  - Enables software control of freeze_io_ebi1
                      FALSE - Disables software control of freeze_io_ebi1 
                              (default)

Return value: None

Dependencies: None

Side effects: None

See Also: HAL_mpm_SetEbi1IoSwCtl
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_SetEbi1IoSwCtl()
   This function is used to set the software value for freeze_io_ebi1.  This
   function writes a '1' to the SW_EBI1_CTL_VALUE field of 
   MPM_POWER_COLLAPSE_CFG.

Parameters: None

Return value: None

Dependencies: None

Side effects: None

See Also: HAL_mpm_CfgSwEbi1IoCntl
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_DebugThroughPC()
   This function is used to enable debugging power collapse using JTAG.  When
   enabled, the processor will be stopped when emerging from the next power
   collapse cycle.  This bit is only for enabling debugging power collapse by
   using JTAG.  Not for enabling debugging VDD minimization by using JTAG.  In
   fact, in order to debug VDD minimization using JTAG successfully, we need
   to keep this bit at 0.

Parameters:
   enable: TRUE  - Enables debugging power collapse using JTAG
           FALSE - Disables debugging power collapse using JTAG (default)

Return value: None

Dependencies: None

Side effects: None

See Also: 
==============================================================================*/



#ifdef __cplusplus
}
#endif

#endif /* HAL_MPM_INTERNAL_H */

