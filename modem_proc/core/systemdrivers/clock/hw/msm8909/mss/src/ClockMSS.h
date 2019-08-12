#ifndef CLOCKMSS_H
#define CLOCKMSS_H
/*
===========================================================================
*/
/**
  @file ClockMSS.h 
  
  Internal header file for the clock device driver on the MSS image.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSS.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  10/19/11   vs      Created.

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIClock.h"
#include "ClockMSSBSP.h"
#include <CoreIni.h>
#ifdef CLOCK_ULOG_STUB
#include "ClockULogStub.h"
#else
  #include "ULogFront.h"
#endif
#include "npa_resource.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/


/*
 * Resource scaling disable flags
 *   BY_USER
 *     Set: At init or runtime via debugger.
 *     Cleared: Never.
 *     Usage: Disable feature (at compile or runtime).
 *   BY_EFS
 *     Set: At init.
 *     Cleared: Clock_LoadNV().
 *     Usage: Disable feature until EFS is loaded.
 *   BY_EFUSE
 *     Set: When efuse reads feature not supported.
 *     Cleared: Never.
 *     Usage: Disable feature until efuse is read.
 *   BY_CPU_TEST:
 *     Set: When \clk\cpu.test is active.
 *     Cleared: When \clk\cpu.test transitions to inactive.
 *     Usage: Disable feature while \clk\cpu.test is active.
 *   BY_BSP:
 *     Set: When the BSP flag is set to disable the feature.
 *     Usage: Set when init reads the BSP data.
 *   HW_LOGIC:
 *     Set: At init or runtime via debugger.
 *     Cleared: Never.
 *     Usage: Disable HAL activity for resource.
 */
#define CLOCK_FLAG_DISABLED_BY_USER                (0x1 << 0)
#define CLOCK_FLAG_DISABLED_BY_EFS                 (0x1 << 1)
#define CLOCK_FLAG_DISABLED_BY_EFUSE               (0x1 << 2)
#define CLOCK_FLAG_DISABLED_BY_CPU_TEST            (0x1 << 3)
#define CLOCK_FLAG_DISABLED_BY_BSP                 (0x1 << 4)
//#define CLOCK_FLAG_DISABLED__x                   (0x1 << 5)
#define CLOCK_FLAG_DISABLED_HW_LOGIC               (0x1 << 31)

/*
 * Clock NPA Node Names
 */
#define CLOCK_NPA_NODE_NAME_CRYPTO                "/clk/mss/crypto"
#define CLOCK_NPA_NODE_NAME_CONFIG_BUS            "/clk/mss/config_bus"
#define CLOCK_NPA_NODE_NAME_CPU                   "/clk/cpu"
#define CLOCK_NPA_NODE_NAME_CPU_TEST              "/clk/cpu.test"
#define CLOCK_NPA_NODE_NAME_PCNOC                 "/clk/pcnoc"
#define CLOCK_NPA_NODE_NAME_CXO                   "/xo/cxo"
#define CLOCK_NPA_NODE_NAME_QDSS                  "/clk/qdss"
#define CLOCK_NPA_NODE_NAME_SNOC                  "/clk/snoc"
#define CLOCK_NPA_NODE_NAME_BIMC                  "/clk/bimc"
#define CLOCK_NPA_NODE_NAME_QPIC                  "/clk/qpic"
#define CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE     "/clk/cpu/impulse"


/*
 * Clock configuration names in EFS .ini file
 */
#define CLOCK_EFS_INI_FILENAME                    "/nv/item_files/clock/settings.ini"


/*
 * EFS Sections for MPSS Clock.
 */
#define CLOCK_EFS_MPSS_CONFIG_SECTION             "MPSS_Clock"

/*
 * EFS Keys for MPSS Clock.
 */
#define CLOCK_EFS_MPSS_CLOCK_FLAGS_FLAG           "Flags"
#define CLOCK_EFS_MPSS_DCS_FLAG                   "EnableDCS"
#define CLOCK_EFS_MPSS_MIN_FREQ_HZ_FLAG           "MinFreqHz"
#define CLOCK_EFS_MPSS_MAX_FREQ_HZ_FLAG           "MaxFreqHz"


/*
 * ARRAY_SIZE definition
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*(x)))
#endif

/*
    Definitions for NPA BIST
*/

/* 
 * The measured clock frequency must match the desired frequency within
 * the following error margin expressed in percent of the desired
 * frequency.
 */
#define CLOCK_NPA_BIST_ERROR_MARGIN_PERCENT         1

/*
 * Default size of the NPA BIST ULOG log buffer.
 *
 * NOTE: The log size should be specified in powers of 2.
 */
#define CLOCK_NPA_BIST_DEFAULT_LOG_SIZE   65536

/*
 * Macro to return absolute value of the difference between two values
 */
#define ABS_DIFF(v1, v2)  ((v1 >= v2) ? (v1 - v2) : (v2 - v1))

/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * CPU context.
 *
 *  pClock            - Clock node.
 *  pConfig           - Clock domain configuration.
 *  PerfConfig        - Active clock domain configuration.
 *  nDisableDCS       - Disable flag for clock scaling.
 *  hNPAClkCPUImpulse - NPA handle to impulse client on /clk/cpu.
 *  bNPABISTEnabled   - Flag used to enable the NPA BIST for CPU.
 */
typedef struct
{
  ClockNodeType          *pClock;
  ClockCPUConfigType     *pConfig;
  ClockCPUPerfConfigType  PerfConfig;
  uint32                  nDisableDCS;
  npa_client_handle       hNPAClkCPUImpulse;
  boolean                 bNPABISTEnabled;
} ClockCPUCtxtType;


/*
 * Config Bus context.
 *
 *  pClock           - Clock node.
 *  pConfig          - Clock domain configuration.
 *  bNPABISTEnabled  - Flag used to enable the NPA BIST for config bus.
 */
typedef struct
{
  ClockNodeType            *pClock;
  ClockNodeType            *pCryptoClock;
  ClockConfigBusConfigType *pConfig;
  boolean                   bNPABISTEnabled;
} ClockConfigBusCtxtType;


/**
 * Clock driver image context.
 *
 *  hVCS              - Handle to the VCS DAL component.
 *  CPUCtxt           - CPU context.
 *  ConfigBusCtxt     - Config bus context.
 *  pBSPConfig        - Image specific BSP data.
 *  hClockNPABISTLog  - Handle to our NPA BIST ULOG log.
 *  nNPABISTLogSize   - Size of the NPA BIST ULOG log.
 */
typedef struct
{
  DalDeviceHandle         *hVCS;
  ClockCPUCtxtType         CPUCtxt;
  ClockConfigBusCtxtType   ConfigBusCtxt;
  ClockImageBSPConfigType *pBSPConfig;
  ULogHandle               hClockNPABISTLog;
  uint32                   nNPABISTLogSize;
} ClockImageCtxtType;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitPLL
** =========================================================================*/
/**
  Initialize the modem PLL management subsystem.
 
  This function initializes NPA nodes and resources to control modem PLLs.
 
  @param *pDrvCtxt [in] -- The driver context.

  @return
  DAL_SUCCESS

  @dependencies
  None.
*/ 

DALResult Clock_InitPLL
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitXO
** =========================================================================*/
/**
  Initialize the XO management subsystem.
 
  This function initializes the XO LPR nodes required for communicating 
  resource requests to the RPM.
 
  @param *pDrvCtxt [in] -- The driver context.

  @return
  DAL_SUCCESS

  @dependencies
  None.
*/ 


DALResult Clock_InitXO
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitConfigBus
** =========================================================================*/
/**
  Initialize /clk/mss/config_bus NPA node.

  This function initializes /clk/mss/config_bus NPA node.

  @param *pDrvCtxt [in] -- The driver context.

  @return
  DAL_SUCCESS

  @dependencies
  None.
*/

DALResult Clock_InitConfigBus
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_LoadNV_DCVS
** =========================================================================*/
/**
  Load the DCVS related data from EFS.

  This function loads the DCVS related data from EFS and sets CPU
  configuration based on these parameters.

  @param *pDrvCtxt  [in] -- Pointer to driver context.
  @param  hConfig   [in] -- CoreConfigHandle to clock EFS file.

  @return
  DAL_SUCCESS -- EFS data applied successfully.
  DAL_ERROR  --  EFS data not applied successfully.

  @dependencies
  None.
*/

DALResult Clock_LoadNV_DCVS
(
  ClockDrvCtxt         *pDrvCtxt,
  CoreConfigHandle      hConfig
);


/* =========================================================================
**  Function : Clock_GetSpeedBin
** =========================================================================*/
boolean  Clock_IsSpeedBinSupported
(
  uint32 nSpeedBinFuse
);

/* =========================================================================
**  Function : Clock_NPA_BIST
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on Q6 & bus clocks

  @param *pDrvCtxt[in] -- Pointer to driver context.
  
  @dependencies
  Core and image initialization must be complete prior to running the BIST.
  DCS should be enabled.

*/

void Clock_NPA_BIST
(
  ClockDrvCtxt    *pDrvCtxt
);

#endif /* !CLOCKMSS_H */

