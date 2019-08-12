#ifndef __CLOCKDRIVER_H__
#define __CLOCKDRIVER_H__
/*
===========================================================================
*/
/**
  @file ClockDriver.h

  Internal header file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockDriver.h#2 $
  $DateTime: 2015/08/27 05:15:12 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "DALSys.h"
#ifdef CLOCK_ULOG_STUB
#include "ClockULogStub.h"
#else
  #include "ULogFront.h"
#endif
#include "railway.h"
#include "ClockDefs.h"
#include "ClockBSP.h"
#ifdef CLOCK_NPA_STUB
#include "ClockNPAStub.h"
#else
  #include "npa.h"
#endif


/*=========================================================================
      Macro Definitions
==========================================================================*/

/**
 * Define __weak for non-rvct compilers.
 */
#ifdef __GNUC__
  #define __weak __attribute__((weak))
#elif !defined(__ARMCC_VERSION)
  #define __weak
#endif


/**
 * Macro lock the driver mutex.
 */
#define DALCLOCK_LOCK(drvCtxt) DALSYS_SyncEnter(drvCtxt->hLock)


/**
 * Macro to free the driver mutex.
 */
#define DALCLOCK_FREE(drvCtxt) DALSYS_SyncLeave(drvCtxt->hLock)


/**
 * Convert an NPA state to a frequency in Hz.
 */
#define NPA_TO_HZ(state) \
  (uint32)((state) == NPA_MAX_STATE ? (state) : (state) * 1000)


/**
 * Return the number of elements in an array.
 */
#define SIZEOF_ARRAY(p) (sizeof(p) / sizeof(p[0]))

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
 *  DOMAIN_CPU  - Indicates this domain is for the CPU clocks.
 *  DOMAIN_HOLD_VREG_REQUEST - Hold the voltage request even
 *    when the clock is off. When set this means the voltage
 *    request will be based solely on the configured clock
 *    frequency and will not be released when the clock is
 *    disabled.
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
#define CLOCK_FLAG_DOMAIN_CPU                 0x00000200
#define CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST   0x00000400
#define CLOCK_FLAG_DOMAIN_DYNAMIC_CONFIG      0x00000800
#define CLOCK_FLAG_DOMAIN_GFX_VREG            0x00001000


/**
 * Global flag bits
 *
 * NOTE: These flags are to be used only in the nGlobalFlags
 * field of the driver context structure.
 *
 *  LOG_x_STATE_CHANGE - Enable logging of enable/disable
 *    calls to all clocks, sources, or power domains as
 *    specified by x.
 *  LOG_CLOCK_FREQ_CHANGE - Enable logging of frequency changes
 *    to all clocks.
 *  LOG_CLOCK_CONFIG_CHANGE - Enable logging of configuration
 *    changes to all clocks.
 *  LOG_CLOCK_RESET - Enable logging of resets to all clocks.
 *  DO_NOT_DISABLE_x - Ignore requests to disable all clocks,
 *    sources, or power domains as specified by x.
 */
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE          0x00000001
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE           0x00000002
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE         0x00000004
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET                 0x00000008
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE         0x00000010
#define CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE   0x00000020
// Reserved for future logging                            0x00000040
// Reserved for future logging                            0x00000080
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_CLOCKS           0x00000100
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_SOURCES          0x00000200
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_POWER_DOMAINS    0x00000400


/*
 * Macros for checking, setting and clearing the unified flag bits in the
 * driver nodes.  Note these work on any structure containing an "nFlags"
 * field.
 */
#define CLOCK_FLAG_IS_SET(ptr, flag) (((ptr)->nFlags & CLOCK_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_FLAG_SET(ptr, flag)    ((ptr)->nFlags |= CLOCK_FLAG_##flag)
#define CLOCK_FLAG_CLEAR(ptr, flag)  ((ptr)->nFlags &= ~CLOCK_FLAG_##flag)


/*
 * Macros for checking, setting and clearing the global flag bits in the
 * driver context.  Note these work only on the "nGlobalFlags" field
 * within the driver context structure.
 */
#define CLOCK_GLOBAL_FLAG_IS_SET(flag) ((pDrvCtxt->nGlobalFlags & CLOCK_GLOBAL_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_GLOBAL_FLAG_SET(flag)    (pDrvCtxt->nGlobalFlags |= CLOCK_GLOBAL_FLAG_##flag)
#define CLOCK_GLOBAL_FLAG_CLEAR(flag)  (pDrvCtxt->nGlobalFlags &= ~CLOCK_GLOBAL_FLAG_##flag)


/*
 * Values used for making NPA requests on clock sources.
 *  0: XO is not required at all
 *  1: XO is only required when the CPU is running.
 *  2: XO is always required.
 */
#define CLOCK_SOURCE_NOT_REQUIRED     0
#define CLOCK_SOURCE_REQUIRED_FOR_CPU 1
#define CLOCK_SOURCE_REQUIRED         2


/*
 * Default size of the BIST ULOG log buffer.
 *
 * NOTE: The log size should be specified in powers of 2.
 */
#define CLOCK_BIST_DEFAULT_LOG_SIZE   32768

/*
 * Standard wait time for a clock to report itself on.
 */
#define CLK_ON_WAIT_USEC    150

/*=========================================================================
      Type Definitions
==========================================================================*/


/**
 * Clock voltage regular configuration type.
 */
typedef struct ClockVRegConfigType
{
  uint32              nClientCount[CLOCK_VREG_NUM_LEVELS]; /**< Number of active clients */
  ClockVRegLevelType  eMinLevel;           /**< Minimum vreg level */
  int32               nRailID;
  railway_voter_t     sVoter;
} ClockVRegConfigType;


/**
 * Clock source type.
 */
typedef struct ClockSourceNodeType ClockSourceNodeType;
struct ClockSourceNodeType
{
  HAL_clk_SourceType             eSource;             /**< Source enum value. */
  ClockSourceConfigType         *pBSPConfig;          /**< Pointer to BSP data */
  npa_client_handle              NPAHandle;           /**< NPA client handle */
  uint32                         nReferenceCount;     /**< Number of active non-CPU domains */
  uint32                         nReferenceCountCPU;  /**< Number of active CPU domains */
  ClockVRegLevelType             eVRegLevel;          /**< Minimum vreg level */
  uint32                         nFlags;              /**< Flags for the source */
  ClockSourceNodeType           *pSource;             /**< Pointer to source's source node */
};

/**
 * Clock domain callback type
 */
typedef struct
{
  ClockCallbackType            pCallback;
  void                         *pCallbackData;
} ClockDomainCallbackType;

/**
 * Clock domain type.
 */
typedef struct
{
  ClockMuxConfigType            *pBSPConfig;          /**< Pointer to BSP data */
  HAL_clk_ClockDomainHandleType  HALHandle;           /**< Handle to HAL data */
  ClockMuxConfigType            *pActiveMuxConfig;    /**< Pointer to active config */
  uint32                         nReferenceCount;     /**< Number of active clocks */
  ClockVRegLevelType             eVRegLevel;          /**< Minimum vreg level */
  uint32                         nFlags;              /**< Flags for the domain */
  ClockSourceNodeType           *pSource;             /**< Pointer to source node */
  ClockDomainCallbackType       *pCallbacks;                    /**< Pointer to callbacks */
} ClockDomainNodeType;


/**
 * Clock type.
 */
typedef struct
{
  const char                    *szName;              /**< Clock name string */
  HAL_clk_ClockHandleType        HALHandle;           /**< Handle to HAL data */
  uint32                         nReferenceCount;     /**< Number of active client requests */
  uint32                         nFlags;              /**< Flags for the clock */
  ClockDomainNodeType           *pDomain;             /**< Pointer to domain node */
} ClockNodeType;


/**
 * Power domain type.
 */
typedef struct
{
  const char                    *szName;              /**< Power domain name string */
  HAL_clk_PowerDomainHandleType  HALHandle;           /**< Handle to HAL data */
  uint32                         nReferenceCount;     /**< Number of active client requests */
  boolean                        bEnabled;            /**< Enabled flag */
  uint32                         nFlags;              /**< Flags for the power domain */
} ClockPowerDomainNodeType;


/**
 * Clock driver context.
 */
typedef struct
{
  DALSYSSyncObj                  LockObj;             /**< Sync object storage */
  DALSYSSyncHandle               hLock;               /**< Handle to our mutex */

  ULogHandle                     hClockLog;           /**< Handle to our ULOG log */
  uint32                         nGlobalFlags;        /**< Global flags for the driver */

  ULogHandle                     hClockBISTLog;       /**< Handle to our BIST ULOG log */
  boolean                        bBISTEnabled;        /**< Flag used to enable the BIST */
  uint32                         nBISTLogSize;        /**< Size of the BIST ULOG log */

  boolean                        bDCVSEnabled;        /**< DCVS state */
  void                          *pImageCtxt;          /**< Image context */

  uint8                          anSourceIndex[HAL_CLK_NUM_OF_SOURCES]; /**< Array of source indices */
  uint32                         nNumSources;         /**< Number of supported sources */
  ClockSourceNodeType           *aSources;            /**< Array of supported sources */
  uint32                         nNumClockDomains;    /**< Number of supported clock domains */
  ClockDomainNodeType           *aClockDomains;       /**< Array of supported clock domains */
  uint32                         nNumClocks;          /**< Number of supported clocks */
  ClockNodeType                 *aClocks;             /**< Array of supported clocks */
  uint32                         nNumPowerDomains;    /**< Number of supported power domains */
  ClockPowerDomainNodeType      *aPowerDomains;       /**< Array of supported power domains */
  ClockVRegConfigType            CX_VRegConfig;       /**< CX Voltage configuration */
  ClockVRegConfigType            GFX_VRegConfig;      /**< GFX Voltage configuration */
  rail_voltage_level            *mVregRailmap;        /**<Vreg level Railway level mapping>*/
} ClockDrvCtxt;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver.

  This function initializes the clock driver, it is the main init entry
  point.

  @param None

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverInit( void );


/* =========================================================================
**  Function : Clock_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param None

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  void
);


/* =========================================================================
**  Function : Clock_InitCore
** =========================================================================*/
/**
  Initialize the core portion of the clock driver.

  This function initializes the common portion of the clock driver that is
  shared with all images.

  @param None

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitCore( void );


/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/**
  Initialize the image specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image we are running on.  This generally corresponds to a particular
  processor on a particular chipset, but also includes the various boot
  images.

  @param None

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitImage( void );


/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/**
  Initialize the NPA DCVS nodes for this image in the clock driver.

  This function initializes the DCVS module for the current image.  This
  will define and create the required NPA nodes.  Nodes will be be
  active until DalClock_EnableDCVS has been called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_SetClockConfig
** =========================================================================*/
/**
  Sets a clock configuration.

  This function configures a clock domain to a particular configuration.
  It is used internally only.

  @param *pClockDomain [in] -- Clock domain to configure.
  @param *pNewConfig [in] -- New configuration to use.

  @return
  DAL_SUCCESS -- The domain was successfully configured.
  DAL_ERROR -- The domain configuration failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_SetClockConfig
(
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
);


/* =========================================================================
**  Function : Clock_InitSources
** =========================================================================*/
/**
  Initialize clock sources.

  This function allocates memory for the clock source nodes and initializes
  them based on the BSP/XML data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitSources
( 
  ClockDrvCtxt *pDrvCtxt 
);


/* =========================================================================
**  Function : Clock_EnableSource
** =========================================================================*/
/**
  Requests to enable a source.

  This function enables a source, generally as a result of a call to
  Clock_EnableClockDomain().  CPU based source requests are tracked
  independently of non-CPU based as the former will be automatically
  released when the CPU goes to sleep.
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bCPURequest [in] -- TRUE if this is a source request for the CPU.

  @return
  DAL_SUCCESS -- The source was successfully enabled.
  DAL_ERROR -- The source was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableSource
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bCPURequest
);


/* =========================================================================
**  Function : Clock_DisableSource
** =========================================================================*/
/**
  Disables a source.

  This function disables a source, generally as a result of a call to
  Clock_DisableClockDomain().
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bCPURequest [in] -- TRUE if this is a source request for the CPU.

  @return
  DAL_SUCCESS -- The source was successfully disabled.
  DAL_ERROR -- The source was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableSource
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bCPURequest
);


/* =========================================================================
**  Function : Clock_ConfigSource
** =========================================================================*/
/**
  Configures a source.

  This function configures a source, generally a PLL.  This can only be
  done if there are no users of the source and we own the PLL.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param *pConfig [in] -- New configuration.

  @return
  DAL_SUCCESS -- The source was successfully configures.
  DAL_ERROR -- The source was not configured, either due to invalid
               parameter or (more likely) the source was in use.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_ConfigSource
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockSourceNodeType   *pSource,
  ClockSourceConfigType *pConfig
);


/* =========================================================================
**  Function : Clock_InitSource
** =========================================================================*/
/**
  Configures a source.

  This function initializes a source, generally a PLL, from the current BSP
  data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param eSource [in] -- Which source to initialize.

  @return
  DAL_SUCCESS -- The source was successfully initialized.
  DAL_ERROR -- The source was not initialized, generally because the source
               has no valid BSP data.

  @dependencies
  The clock mutex should be locked prior to calling this function or
  we should be a single execution thread environment (like boot).
*/

DALResult Clock_InitSource
(
  ClockDrvCtxt          *pDrvCtxt,
  HAL_clk_SourceType     eSource
);


/* =========================================================================
**  Function : Clock_GetPropertyValue
** =========================================================================*/
/**
  Retrieve a clock driver property from the environment store.

  This function retrieves the given clock property from the relevant
  storage location (may be DAL properties DB or another source).

  @param *szName [in] -- Name of the property to look up
  @param *pValue [out] -- Location to store the result

  @return
  DAL_SUCCESS if property was found and retrieved, DAL_ERROR otherwise.

  @dependencies
  None.
*/

DALResult Clock_GetPropertyValue
(
  const char              *szName,
  ClockPropertyValueType  *pValue
);


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/**
  Initialize voltage tracking subsystem.

  This function prepares the voltage tracking module for managing voltage
  requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitGFXVoltage
** =========================================================================*/
/**
  Initialize voltage for GFX rail

  This function prepares the voltage tracking module for managing GFX voltage
  requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitGFXVoltage
(
  ClockDrvCtxt *pDrvCtxt
);


/*=========================================================================
**  Function : Clock_IsBSPSupported
** =========================================================================*/
/**

  This function is used for checking if BSP config is supported based on
  its chip version.

  @param pBSPConfig [in] - BSP for checking

  @return 
  TRUE - BSP config is supported.
  FALSE - BSP config is not supported.

  @dependencies
  None.
*/

boolean Clock_IsBSPSupported
( 
  ClockMuxConfigType *pBSPConfig
);


/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/**
  Make a voltage regulator request.

  This function is used internally for a clock domain to make a voltage
  regulator level request.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *peCurrentLevel [in/out] -- Pointer to currently requested voltage
                                     level.  Updated to eNewLevel.
  @param eNewLevel [in] -- New voltage requirement.
  @param bGFXVReg [in] -- TRUE - GFX Voltage request
                       -- FALSE - CX Voltage request

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageRequest
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockVRegLevelType  *peCurrentLevel,
  ClockVRegLevelType  eNewLevel,
  boolean             bGFXVReg
);


/* =========================================================================
**  Function : Clock_VoltageSuspend
** =========================================================================*/
/**
  Make a request to suspend voltage requests.

  This function is used internally to suspend voltage requests.

  @param *pDrvCtxt [in]       -- Handle to the DAL driver context.
  @param bSuspendVoltage [in] -- Voltage suspend flag.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageSuspend
(
  ClockDrvCtxt        *pDrvCtxt,
  boolean             bVoltageSuspend
);


/* =========================================================================
**  Function : Clock_VoltageResume
** =========================================================================*/
/**
  Make a request to resume voltage requests with or without update railway

  This function is used internally to resume voltage requests.

  @param *pDrvCtxt [in]      -- Handle to the DAL driver context.
  @param bUpdateRailway [in] -- Flag to update railway with resume or not.

  @return
  none

  @dependencies
  The clock mutex must be locked.
*/

void Clock_VoltageResume
(
  ClockDrvCtxt        *pDrvCtxt,
  boolean             bUpdateRailway
);


ClockDrvCtxt *Clock_GetDrvCtxt(void);

/*=========================================================================
      Function Definitions From Clock.h
==========================================================================*/

DALResult Clock_GetClockId( const char * szClock,  ClockIdType * pnId);
DALResult Clock_EnableClock( ClockIdType  nClock);
DALResult Clock_EnableClockEx( ClockIdType nClock, boolean bWaitForClkOn);
DALResult Clock_DisableClock( ClockIdType  nClock);
DALResult Clock_IsClockEnabled(  ClockIdType  nClock,  boolean * pbIsOn);
DALResult Clock_IsClockOn(  ClockIdType  nClock,  boolean * pbIsOn);
DALResult Clock_WaitForClockOn( ClockNodeType * pClock, uint32 nTimeoutUsec);

DALResult Clock_GetPowerDomainId( const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId);
DALResult Clock_EnablePowerDomain( ClockPowerDomainIdType  nPowerDomainId, boolean bAsync);
DALResult Clock_DisablePowerDomain( ClockPowerDomainIdType  nPowerDomainId);
boolean   Clock_IsPowerDomainEnabled( ClockPowerDomainIdType  nPowerDomainId );

DALResult Clock_ProcessorSleep( ClockSleepModeType  eMode,  uint32  nFlags);
DALResult Clock_ProcessorRestore( ClockSleepModeType  eMode,  uint32  nFlags);

#endif /* !__CLOCK_DRIVER_H__ */

