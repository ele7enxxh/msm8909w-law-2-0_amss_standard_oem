#ifndef CLOCKDEFS_H
#define CLOCKDEFS_H
/*
===========================================================================
*/
/**
  @file ClockDefs.h
  @brief Public definitions include file for accessing the clock device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Clock_Driver_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      LaTeX. To edit or update any of the file/group text in the PDF, edit the 
      Clock_Driver_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/ClockDefs.h#4 $
  $DateTime: 2015/08/27 05:15:12 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  09/16/14   muralid  ClockVRegLevelType updated to sync with new corner introduced in railway.
  05/17/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup clock_functions
@{ */
  

/*
  * Macro to force an enumeration to be a full 32 bits wide.
   */
#define CLOCK_BSP_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF


/**
  Clock handle.
 */
typedef uint32 ClockIdType;


/**
  Used to search for a matching frequency input. This type describes how 
  frequency matching is done for the clock frequency selection function. It 
  also indicates units in Hz, kHz, or MHz.
 */
typedef enum
{
  CLOCK_FREQUENCY_HZ_AT_LEAST  = 0,  /**< Frequency is the minimum allowed in Hz. */
  CLOCK_FREQUENCY_HZ_AT_MOST   = 1,  /**< Frequency is the maximum allowed in Hz. */
  CLOCK_FREQUENCY_HZ_CLOSEST   = 2,  /**< Closest match in Hz. */
  CLOCK_FREQUENCY_HZ_EXACT     = 3,  /**< Exact match only in Hz. */

  CLOCK_FREQUENCY_KHZ_AT_LEAST = 0x10,  /**< Frequency is the minimum allowed in kHz. */
  CLOCK_FREQUENCY_KHZ_AT_MOST  = 0x11,  /**< Frequency is the maximum allowed in kHz. */
  CLOCK_FREQUENCY_KHZ_CLOSEST  = 0x12,  /**< Closest match in kHz. */
  CLOCK_FREQUENCY_KHZ_EXACT    = 0x13,  /**< Exact match only in kHz. */

  CLOCK_FREQUENCY_MHZ_AT_LEAST = 0x20,  /**< Frequency is the minimum allowed in MHz. */
  CLOCK_FREQUENCY_MHZ_AT_MOST  = 0x21,  /**< Frequency is the maximum allowed in MHz. */
  CLOCK_FREQUENCY_MHZ_CLOSEST  = 0x22, /**< Closest match in MHz. */
  CLOCK_FREQUENCY_MHZ_EXACT    = 0x23, /**< Exact match only in MHz. */
} ClockFrequencyType;


/**
  Type of clock reset. This defines how the reset to the clock domain 
  is to be handled.
 */
typedef enum
{
  CLOCK_RESET_DEASSERT = 0,  /**< Deassert the reset signal. */
  CLOCK_RESET_ASSERT   = 1,  /**< Assert and hold the reset signal. */
  CLOCK_RESET_PULSE    = 2   /**< Assert and immediately deassert. */
} ClockResetType;


/**
 * Clock QDSS levels
 */
typedef enum
{
  CLOCK_QDSS_LEVEL_OFF,      /* clock set to CXO, and off */
  CLOCK_QDSS_LEVEL_DEBUG,    /* clock rate follows VDD_CX */
  CLOCK_QDSS_LEVEL_HSDEBUG,  /* clock set to high speed */
  CLOCK_QDSS_LEVEL_LOW,      /* clock set to low speed */
} ClockQDSSLevelType;


/**
  Type of clock configuration.  This type describes how a clock is 
  configured.
 */
typedef enum
{
  CLOCK_CONFIG_MMSS_SOURCE_CSI0     = 1,  /**< Select CSI0 source. */ 
  CLOCK_CONFIG_MMSS_SOURCE_CSI1     = 2,  /**< Select CSI1 source. */ 
  CLOCK_CONFIG_MMSS_SOURCE_CSI2     = 3,  /**< Select CSI2 source. */ 

  /** @cond 
  */
  CLOCK_CONFIG_TOTAL = 4
  /** @endcond */
} ClockConfigType;


/**
   Type of clock bus resource.
 */
typedef enum
{
  CLOCK_RESOURCE_SNOC = 0,
  CLOCK_RESOURCE_CNOC,
  CLOCK_RESOURCE_PNOC,
  CLOCK_RESOURCE_BIMC
} ClockBusType;

/**
   CLOCK_CALLBACK_MAX is the current maximum number of callbacks per clock
 */
#define CLOCK_CALLBACK_MAX 2

/**
   ClockCallbackHandle is a general pointer handle to a clock callback
 */
typedef void *ClockCallbackHandle;

/**
   ClockPrePostType indicates if the current callback is before or after 
   the clock switch.  Each callback is called twice per clock switch.
 */
typedef enum 
{ 
  CLOCK_PRE_CALLBACK = 0,
  CLOCK_POST_CALLBACK = 1
} ClockPrePostType;

/**
   ClockCallbackType is a pointer to a callback function that takes 4 arguments.

   @param ePrePost      [in] informs the callback if this is before or after the clock change.
   @param nCurrFreqKHz  [in] The current frequency of the clock during the pre-callback in kHz.
   @param nNewFreqKHz   [in] The new frequency of the clock during the post-callback in kHz.
   @param pCallbackData [in] A pointer to data for the callback to use as needed.

  @return
  None
   
  @dependencies
  None.

  @sa
  None
 */
typedef void (*ClockCallbackType)( ClockPrePostType ePrePost, uint32 nCurrFreqKHz, uint32 nNewFreqKHz, void *pCallbackData );

/** @} */ /* end_addtogroup clock_functions */

/** @addtogroup power_domain_functions
@{ */


/**
  Power domain handle.
 */
typedef uint32 ClockPowerDomainIdType;


/** @} */ /* end_addtogroup power_domain_functions */

/** @addtogroup misc_functions
@{ */


/**
  Processor Sleep mode. This type defines the different kinds of sleep modes 
  that can be entered.
 */
typedef enum
{
  CLOCK_SLEEP_MODE_HALT,            /**< Processor halt only. */
  CLOCK_SLEEP_MODE_POWER_COLLAPSE,  /**< Processor power collapse. */
} ClockSleepModeType;


/*
 * ClockVRegLevelType
 *
 * List of voltage levels that a client can request.  Note that the
 * enumeration values map to pmic NPA node request values.
 *
 *  OFF          - Turn off non-CX rail
 *  LOW          - Lowest voltage level, or "do not care"
 *  NOMINAL      - Nominal voltage level
 *  NOMINAL_PLUS - Intermediate leve between Nominal and high (Turbo)
 *  HIGH         - Elevated, or "Super Turbo" voltage level.
 *  
 *  CLOCK_VREG_LEVEL_NOMINAL, CLOCK_VREG_LEVEL_NOMINAL_PLUS, CLOCK_VREG_LEVEL_HIGH enums updated because of new corner introduced in railway.h
 */

typedef enum
{
  CLOCK_VREG_LEVEL_OFF          , /* RAILWAY_NO_REQUEST  */
  CLOCK_VREG_LEVEL_LOW          , /* RAILWAY_SVS_SOC     */
  CLOCK_VREG_LEVEL_LOW_PLUS     , /* RAILWAY_SVS_SOC_PLUS     */
  CLOCK_VREG_LEVEL_NOMINAL      , /* RAILWAY_NOMINAL     */
  CLOCK_VREG_LEVEL_NOMINAL_PLUS , /* RAILWAY_TURBO       */
  CLOCK_VREG_LEVEL_HIGH         , /* RAILWAY_SUPER_TURBO */
  CLOCK_VREG_NUM_LEVELS,
  CLOCK_BSP_ENUM_32BITS(VREG_LEVEL)
} ClockVRegLevelType;


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


/**
  Processor Sleep mode flag masks. RAMP_DOWN_CPU triggers frequency switching 
  to a lower performance level before SWFI.  L2CACHE_FLUSH flushes the contents
  of L2 cache when entering power collapse.
 */
#define CLOCK_SLEEP_FLAG_RAMP_DOWN_CPU          0x00000001
#define CLOCK_SLEEP_FLAG_L2CACHE_FLUSH          0x00000002


/** @} */ /* end_addtogroup misc_functions */

/** @addtogroup npa_functions
@{ */
/** @name NPA Query Requests
@{ */

/**
  NPA query ID for a performance level frequency. This query 
  gets the frequency in kHz for the node performance level. The actual
  performance level is to be added to this ID (i.e., to query frequency
  for performance level 2, pass DALCLOCK_NPA_QUERY_PERF_LEVEL_KHZ + 2).
 */
#define CLOCK_NPA_QUERY_PERF_LEVEL_KHZ (NPA_QUERY_RESERVED_END + 1)

/**
  NPA query ID for the number of supported performance levels.
  This query returns the number of supported performance levels for
  the resource in question.
 */
#define CLOCK_NPA_QUERY_NUM_PERF_LEVELS (CLOCK_NPA_QUERY_PERF_LEVEL_KHZ + 0x20)

/**
  NPA query for the minimum supported frequency in kHz for a resource.
  This query returns the minimum supported frequency for the resource
  based on the target and configured limitations.
 */
#define CLOCK_NPA_QUERY_MIN_FREQ_KHZ   (CLOCK_NPA_QUERY_NUM_PERF_LEVELS + 1)

/**
  NPA query for all supported frequencies in kHz for a resource.
  This query returns the array of supported frequency for the resource
  based on the target and configured limitations.
 */
#define CLOCK_NPA_QUERY_ALL_FREQ_KHZ   (CLOCK_NPA_QUERY_NUM_PERF_LEVELS + 2)

#define CLOCK_NPA_QUERY_SETTLING_TIME  (CLOCK_NPA_QUERY_NUM_PERF_LEVELS + 3)
/** @} */ /* end_name_group NPA Query Requests */
/** @} */ /* end_addtogroup npa_functions */


#endif /* !CLOCKDEFS_H */

