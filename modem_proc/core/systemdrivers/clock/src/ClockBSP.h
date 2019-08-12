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

      Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/src/ClockBSP.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
05/14/13   vt      Added INTERNAL_CONTROL type to unified flag bits
11/30/10   vs      Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/


#include "ClockDefs.h"
#include "HALclk.h"
#include "DDIChipInfo.h"
#include "VCSDefs.h"


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/


/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_BSP_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF


/*
 * Macros for consolidating PLL configuration in a single mask.
 */
#define CLOCK_CONFIG_PLL_FSM_MODE_ENABLE        (1 << 0)
#define CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE      (1 << 1)
#define CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE     (1 << 2)
#define CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE    (1 << 3)


/**
 * Unified flag bits
 *
 * NOTE: These flags can be used in any node that supports the
 * nFlags field.
 *
 *  LOG_STATE_CHANGE - Enable logging of enable/disable calls
 *    to a particular node.
 *  LOG_FREQ_CHANGE - Enable logging of frequency changes to
 *    a particular node.
 *  LOG_CONFIG_CHANGE - Enable logging of configuration changes
 *    to a particular node.
 *  LOG_RESET - Enable logging of resets to a particular node.
 *  DO_NOT_DISABLE - Ignore requests to disable a particular
 *    node.
 *  SUPPRESSIBLE - Indicates XO shutdown will not be held off
 *    while the clock is on.
 *  DOMAIN_HOLD_VREG_REQUEST - Hold the voltage request even
 *    when the clock is off. When set this means the voltage
 *    request will be based solely on the configured clock
 *    frequency and will not be released when the clock is
 *    disabled.
 *  DOMAIN_DYNAMIC_CONFIG - Indicates if the mux configuration
 *    was set up dynamically so we free up the memory after
 *    switching to a new configuration.
 *  AUTOGATE - Allows hardware to automatically enable or
 *    disable the clock branch based on usage. When this
 *    flag is set, the CLK_ENABLE bit in the CBCR register
 *    is ignored.
 *  BIST_FAILURE_ANTICIPATED - Indicates that a clock is
 *    expected to fail the BIST. The BIST log will show
 *    that the clock failed, but that the failure is
 *    expected. The clock does not count towards the
 *    total number of failed clocks.
 *  INTERNAL_CONTROL - Indicates that this node (source/clock/etc)
 *    cannot be controlled by clients via the DAL API.
 *  SUPPORTS_SLEWING - Indicates that this PLL can be
 *    dynamically re-configured without having to first
 *    be disabled and then re-enabled.
 *  SOURCE_CALIBRATED - Indicates that a PLL has been calibrated.
 *  BYPASS_DEPENDENCIES - Indicates that the resource's requirements
 *    are always satisfied or managed by an external agent.
 *    The clock driver will not make requests for this type
 *    of resource.
 *  ALWAYS_ON - Indicates that the node should not be enabled/disabled
 *    because the resource is always on.
 *  DOMAIN_WAIT_FOR_ROOT_OFF - Indicates that the SW should wait for 
 *    a root-off signal before disabling the clock source.
 */
#define CLOCK_FLAG_LOG_STATE_CHANGE           0x00000001
#define CLOCK_FLAG_LOG_FREQ_CHANGE            0x00000002
#define CLOCK_FLAG_LOG_CONFIG_CHANGE          0x00000004
#define CLOCK_FLAG_LOG_RESET                  0x00000008
// Reserved for future logging                0x00000010
// Reserved for future logging                0x00000020
// Reserved for future logging                0x00000040
// Reserved for future logging                0x00000080
#define CLOCK_FLAG_DO_NOT_DISABLE             0x00000100
#define CLOCK_FLAG_SUPPRESSIBLE               0x00000200
#define CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST   0x00000400
#define CLOCK_FLAG_DOMAIN_DYNAMIC_CONFIG      0x00000800
#define CLOCK_FLAG_AUTOGATE                   0x00001000
#define CLOCK_FLAG_BIST_FAILURE_ANTICIPATED   0x00002000
#define CLOCK_FLAG_INTERNAL_CONTROL           0x00004000
#define CLOCK_FLAG_SUPPORTS_SLEWING           0x00008000
#define CLOCK_FLAG_SOURCE_CALIBRATED          0x00010000
#define CLOCK_FLAG_BYPASS_DEPENDENCIES        0x00020000
#define CLOCK_FLAG_ALWAYS_ON                  0x00040000
#define CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF   0x00080000


/**
 * Global flag bits
 *
 * NOTE: These flags are to be used only in the nGlobalFlags
 * field of the driver context structure.
 *
 *  LOG_x_STATE_CHANGE - Enable logging of enable/disable
 *    calls to all clocks, sources, or power domains as
 *    specified by x.
 *  LOG_x_FREQ_CHANGE - Enable logging of frequency changes
 *    to all clocks, sources as specified by x.
 *  LOG_CLOCK_CONFIG_CHANGE - Enable logging of configuration
 *    changes to all clocks.
 *  LOG_CLOCK_RESET - Enable logging of resets to all clocks.
 *  LOG_VOLTAGE_CHANGE - Enable logging of vdd requirement changes.
 *  DO_NOT_DISABLE_x - Ignore requests to disable all clocks,
 *    sources, or power domains as specified by x.
 *  STUB_API - Stubs out entire API (DAL and NPA nodes expected).
 */
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE          0x00000001
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE           0x00000002
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE         0x00000004
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET                 0x00000008
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE         0x00000010
#define CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE   0x00000020
#define CLOCK_GLOBAL_FLAG_LOG_VOLTAGE_CHANGE              0x00000040
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_FREQ_CHANGE          0x00000080
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_CLOCKS           0x00000100
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_SOURCES          0x00000200
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_POWER_DOMAINS    0x00000400
#define CLOCK_GLOBAL_FLAG_STUB_API                        0x00000800

#define SOURCE_NAME(x)           \
  .eSource = HAL_CLK_SOURCE_ ## x, \
  .szName  = #x


/*
 * ClockHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
  const DalChipInfoIdType *aeChipInfoId;

} ClockHWVersionType;


/*
 * ClockSourceFreqConfigType
 *
 * Frequency configuration information for a PLL.
 *
 *  nFreqHz     - Frequency in Hz for this configuration.
 *  HALConfig   - Configuration to pass to the HAL.
 *  eVRegLevel  - Voltage level for this configuration.
 *  HWVersion   - Version of the chip HW this configuration is for.
 */
typedef struct
{
  uint32                nFreqHz;
  HAL_clk_PLLConfigType HALConfig;
  VCSCornerType         eVRegLevel;
  ClockHWVersionType    HWVersion;
} ClockSourceFreqConfigType;


/*
 * ClockSourceConfigType
 *
 * Configuration information for a PLL.
 * 
 *  eSource           - Source ENUM value.
 *  szName            - Name of source.
 *  nConfigMask       - Mask of misc PLL configurations such as aux out,
 *                      bist out, early out, etc.
 *  pSourceFreqConfig - Pointer to array of source configurations.
 *  pCalibrationFreqConfig - Pointer to the calibration configuration.
 *  eDisableMode           - PLL disable/enable mode.
 */
typedef struct
{
  HAL_clk_SourceType         eSource;
  const char                *szName;
  uint32                     nConfigMask;
  ClockSourceFreqConfigType *pSourceFreqConfig;
  ClockSourceFreqConfigType *pCalibrationFreqConfig;
  HAL_clk_SourceDisableModeType  eDisableMode;
} ClockSourceConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz           - Frequency in Hz for this configuration.
 *  HALConfig         - Configuration to pass to the HAL.
 *  eVRegLevel        - Voltage level for this configuration.
 *  HWVersion         - Version of the chip HW this configuration is for.
 *  pSourceFreqConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                     nFreqHz;
  HAL_clk_ClockMuxConfigType HALConfig;
  VCSCornerType              eVRegLevel;
  ClockHWVersionType         HWVersion;
  ClockSourceFreqConfigType *pSourceFreqConfig;
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
 * ClockXOVoterType
 *
 * Structure to hold clocks which need to explicitly hold a source.
 */
typedef struct
{
  const char *szName;
} ClockXOVoterType;


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
 * ClockFlagNodeType
 *
 * List of nodes that init flags can be applied to.
 *
 *  CLOCK        - Flags are to be applied to the specified clock.
 *  CLOCK_DOMAIN - Flags are to be applied to the specified domain.
 *  SOURCE       - Flags are to be applied to the specified source.
 *  POWER_DOMAIN - Flags are to be applied to the specified power domain.
 *
 */
typedef enum
{
  CLOCK_FLAG_NODE_TYPE_NONE          = 0,
  CLOCK_FLAG_NODE_TYPE_CLOCK         = 1,
  CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN  = 2,
  CLOCK_FLAG_NODE_TYPE_SOURCE        = 3,
  CLOCK_FLAG_NODE_TYPE_POWER_DOMAIN  = 4,
  CLOCK_FLAG_NODE_TYPE_MAX_SUPPORTED = 5
} ClockFlagNodeType;


/*
 * ClockFlagInitType
 *
 * BSP Data structure for initializing the flags of an individual clock,
 * clock domain, power domain, or source.
 *
 *  eNodeType - The node to which the flags should be applied.
 *  id        - The ID to lookup this node info.
 *              (This type is dependent upon the eNodeType).
 *  nFlag     - Init value for the clock's flags.
 */
typedef struct
{
  ClockFlagNodeType  eNodeType;
  void              *pID;
  uint32             nFlag;
} ClockFlagInitType;



/*
 * ClockStubType
 *
 * BSP data structure for stubbing out the clock driver on various platforms.
 *
 * bRUMI   - Stub boolean for RUMI.
 * bVirtio - Stub boolean for Virtio.
 */
typedef struct
{
  boolean bRUMI;
  boolean bVirtio;
} ClockStubType;


#endif  /* __CLOCKBSP_H__ */ 

