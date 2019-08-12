#ifndef __CLOCKBSP_H__
#define __CLOCKBSP_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
        B O A R D   S U P P O R T   P A C K A G E   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the BSP interface definitions for the
  MSM DAL Clock Device Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2010-2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockBSP.h#2 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
01/24/13   vph     Add macro for set BSP HW version
01/24/13   pbitra  Replaced DDIChipInfo.h APIs with ChipInfo.h APIs
11/30/10   vs      Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/


#include "HALclk.h"
#include "Chipinfo.h"
#include "HALavs.h"
#include "ClockDefs.h"
#include "voltage_level.h"


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

/*
 * Macros for compare support BSP within the range of HW version
 * BSP HW VERSION:
 *     Bits[ 0: 7] - Max HW version minor bits
 *     Bits[15: 8] - Max HW version major bits
 *     Bits[23:16] - Min HW version minor bits
 *     Bits[31:24] - Min HW version major bits
*/

#define BSP_SUPPORTED     0
#define BSP_NOT_SUPPORTED 0xFFFF0000

#define BSP_HW_VER(nMinMajor, nMinMinor, nMaxMajor, nMaxMinor)               \
            ( ((nMinMajor<<24)& 0xFF000000) | ((nMinMinor<<16) & 0x00FF0000) | \
              ((nMaxMajor<<8) & 0x0000FF00) |  (nMaxMinor&0xFF) )

#define BSP_HW_VER_MIN( nBSPVersion ) ( ((nBSPVersion & 0xFF000000)>>8) | \
                                        ((nBSPVersion & 0x00FF0000)>>16) )
#define BSP_HW_VER_MAX( nBSPVersion ) ( ((nBSPVersion & 0x0000FF00)<<8) | \
                                         (nBSPVersion & 0x000000FF) )

/*
 * Macros for consolidating PLL configuration in a single mask.
 */
#define CLOCK_CONFIG_PLL_FSM_MODE_ENABLE        (1 << 0)
#define CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE      (1 << 1)
#define CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE     (1 << 2)
#define CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE    (1 << 3)


/*
 * ClockSourceConfigType
 *
 * Configuration information for a PLL.
 * 
 *  eSource     - Source ENUM value.
 *  HALConfig   - Configuration to pass to the HAL.
 *  nConfigMask - Mask of misc PLL configurations such as aux out, bist out,
 *                early out, etc.
 *  nFreqHz     - Frequency in Hz for this configuration.
 *  eVRegLevel  - Voltage level for this configuration.
 */
typedef struct
{
  HAL_clk_SourceType      eSource;
  HAL_clk_PLLConfigType   HALConfig;
  uint32                  nConfigMask;
  uint32                  nFreqHz;
  ClockVRegLevelType      eVRegLevel;
} ClockSourceConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz       - Frequency in Hz for this configuration.
 *  HALConfig     - Configuration to pass to the HAL.
 *  eVRegLevel    - Voltage level for this configuration.
 *  nChipVersion  - Version of the chip this configuration is for. '0'
 *                  inidicates all versions.
 *  nFamily       - Chip family this configuration is for. '0' indicates all chips.
 *  pSourceConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                      nFreqHz;
  HAL_clk_ClockMuxConfigType  HALConfig;
  ClockVRegLevelType          eVRegLevel;
  ChipInfoVersionType         nChipVersion;
  ChipInfoFamilyType          nFamily;
  ClockSourceConfigType      *pSourceConfig;
} ClockMuxConfigType;


/*
 * ClockLogType
 *
 * Default configuration for the clock log.
 *
 *  nLogSize         - Log size in bytes.  Must be a power of 2.
 *  nGlobalLogFlags  - Global log flags.
 */
typedef struct
{
  uint32      nLogSize;
  uint8       nGlobalLogFlags;
} ClockLogType;


/*
 * ClockPropertyValueType
 *
 * Generic clock property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *ClockPropertyValueType;


/*
 * ClockPropertyType
 *
 * Generic clock property structure, the BSP data in non-DAL environments
 * will consist of an array of these.
 *
 *  szName - Name of the property
 *  Value  - Property value
 */
typedef struct
{
  const char            *szName;
  ClockPropertyValueType Value;
} ClockPropertyType;



/*
 * ClockAVSCoreCfgType
 *
 * BSP Data structure for describing delay synthesizer to CPU frequency 
 * requirements.
 *
 * nDelaySynth    - The synthesizer value to program to the coprocessor.
 * nMinVdd        - The lowest voltage that AVS can set automatically at this frequency.
 * nPerfVdd       - The performance level associated voltage in millvolts.
 */
typedef struct
{
  uint32 nDelaySynth;
  uint32 nMinVdd;
  uint32 nPerfVdd;
}ClockAVSCoreFreqCfgType;


/*
 * ClockAVSCoreConfigType
 *
 * BSP Data structure for initializing an AVS core.
 *
 * nPhysicalBase    - The physical base address for this AVS core.
 * eCore            - The Core ID for this instance.
 * ePMIC            - The PMIC type for this core.
 * eStepSize        - The step size of voltage adjustment requests.
 * nVddRequestDelay - The delay between HW voltage requests.
 * nClockDiv        - The clock divider.
 */
typedef struct
{
  uint32               nPhysicalBase;
  HAL_avs_CoreType     eCore;
  HAL_avs_PmicType     ePMIC;
  HAL_avs_StepSizeType eStepSize;
  uint32               nVddRequestDelay;
  uint32               nClockDiv;
}ClockAVSCoreConfigType;


#endif  /* __CLOCKBSP_H__ */ 

