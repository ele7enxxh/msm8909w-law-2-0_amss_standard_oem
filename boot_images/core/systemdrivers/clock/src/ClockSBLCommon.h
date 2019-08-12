#ifndef CLOCKSBLCOMMON_H
#define CLOCKSBLCOMMON_H

/*
===========================================================================
*/
/**
  @file ClockSBLCommon.h

  Common functions for PLL and clock management
*/
/*
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/src/ClockSBLCommon.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  when        who     what, where, why
  --------    ---     -------------------------------------------------
  2012/03/13  bc      Initial revision.
  ====================================================================
*/

/*=========================================================================
      Definitions
==========================================================================*/

/*
 * Clock voting register.
 */
#define PROC_CLK_BRANCH_ENA_VOTE GCC_APCS_CLOCK_BRANCH_ENA_VOTE

/*
 * RCGR update timeout. In practice this should be very short (less than 1us), 
 * but it depends on the clock frequency being fed into the RCG. Choosing 
 * a very conservative value.
 */
#define CLOCK_UPDATE_TIMEOUT_US 1000

/*=========================================================================
      Type Definitions
==========================================================================*/
/*
 * ClockSourceType
 *
 * A type to be used for clock sources in the configuration data and functions.
 */

typedef enum {
  SRC_CXO,
  SRC_GPLL0,
  SRC_GPLL1,
  SRC_USB3_PIPE_CLK = 2,
  SRC_GPLL2,
  SRC_GPLL3,
  SRC_GPLL4,
  SRC_GPLL5,
  SRC_BIMCPLL,
  SRC_GPLL6,
  SRC_MMPLL0,
  SRC_MMPLL1,
  SRC_MMPLL2,
  SRC_MMPLL3,
  SRC_LPAPLL0,
  SRC_LPAPLL1,
  SRC_WCNPLL,
  SRC_MPLL0,
  SRC_MPLL1,
  SRC_MPLL2,
  SRC_KPPLL0,
  SRC_KPPLL1,
  SRC_KPPLL2,
  SRC_KPPLL3,
  SRC_KPL2PLL,
  SRC_A7PLL,
  SRC_A53PLL,
  //SRC_A53PWRPLL,
  SRC_A53PERFPLL,
  SRC_A53CCIPLL,
  SRC_RAW,
} ClockSourceType;

/*
 * ClockSourceMuxType
 *
 * A type to choose the mapping for the mux.
 */
typedef enum {
  MUX_GCC,
  MUX_APCS,
  MUX_APSSTCU,
  MUX_BIMC,
  MUX_CCI,
  NUM_MUX_TYPES
} ClockSourceMuxType;

/*
 * Clock_SourcePllType
 *
 * A type to choose the type of PLL.
 */
typedef enum{
  CLK_SOURCE_PLL_SR,
  CLK_SOURCE_PLL_SR2,
  CLK_SOURCE_PLL_STROMER,
  NUM_CLK_SOURCE_PLL_TYPES
} Clock_SourcePllType;

/*
 * ClockConfigPLLType
 *
 * Parameters used for configuring a source that is a PLL.
 *
 *  nPLLModeAddr  - The address of the PLL MODE register.
 *  nVoteAddr     - The address of the PLL voting register. Set to NULL for 
 *                  non-FSM mode.
 *  nVoteMask     - The mask of the voting bit.
 *  eVCO          - The internal VCO to use.
 *  nPreDiv       - The pre-divider value (generally 1 or 2).
 *  nPostDiv      - The pre-divider value (generally 1 or 2).
 *  nL            - The L value for the PLL.  The PLL output frequency is derived
 *                  as out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv.
 *  nM            - The M value (see above).
 *  nN            - The N value (see above).
 *  nAlpha        - The Alpha(prog) value. For the 20nm PLLs, the output frequency is
 *                  derived as :
 *                  out_freq = ((in_freq / nPreDiv) * (L + ALPHAfrac)) / nPostDiv.
 *                  ALPHAprog = 2^b x ALPHAfrac where “b” is 40 for Prius and 40 for Tesla.
 */
typedef struct ClockConfigPLL
{
  uint32 nPLLModeAddr;
  uint32 nVoteAddr;
  uint32 nVoteMask;
  uint32 nVCO;
  uint32 nPreDiv;
  uint32 nPostDiv;
  uint32 nL;
  uint32 nM;
  uint32 nN;
  uint32 nConfigCtl;
  uint64 nAlpha;
  Clock_SourcePllType ePLL;
} ClockConfigPLLType;

/*
 * ClockConfigMuxType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  nCMDCGRAddr - The address of the CMD RCGR register.
 *  eMux        - The mux type for mapping eSource to register source values.
 *  eSource     - The source to use.
 *  nDiv2x      - The integer (2 * divider) value to use.
 *  nM          - The M value for any M/N counter, or 0 to bypass.
 *  nN          - The N value for any M/N counter.
 *  n2D         - Twice the D value for any M/N counter.
 */
typedef struct ClockConfigMux
{
  uint32 nCMDCGRAddr;
  ClockSourceMuxType eMux;
  ClockSourceType eSource;
  uint32 nDiv2x;
  uint32 nM;
  uint32 nN;
  uint32 n2D;
} ClockConfigMuxType;

/*
 * Clock_CBCRtoggleType
 *
 * A type to choose the the operation on clocks(enable/disable).
 */
typedef enum{
  CLK_TOGGLE_DISABLE,
  CLK_TOGGLE_ENABLE,
  NUM_CLK_TOGGLE_TYPES
} Clock_CBCRtoggleType;

typedef struct
{
  ClockSourceType    ePLL;
  ClockConfigPLLType PLLCfg;
} ClockSourceConfigType;

typedef struct
{
  ClockConfigMuxType        BimcMux;
  ClockConfigMuxType        DDRMux;
  ClockConfigMuxType        BimcGPUMux;
}BimcMuxType;

typedef struct
{
  uint32                    nFrequency;
  ClockVRegLevelType        eVRegLevel;
  ClockSourceConfigType     *pSourceConfig;
  BimcMuxType               Mux;
} BIMCClockCfgType;

typedef struct
{
  uint32                nFrequency;
  ClockConfigMuxType    Cfg;
} ClockSDCCfgType;


typedef struct
{
  uint32                nFrequency;
  ClockConfigMuxType    ApcsMux;
}ClockAPCSCfgType;

/* ============================================================================
**  Function : Clock_EnablePLL
** ============================================================================
*/
/*!
    Configure and enable a PLL.  If it is voteable, it will be set for FSM mode
    and voted for using the vote register.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnablePLL(const ClockConfigPLLType *pConfig);

/* ============================================================================
**  Function : Clock_DisablePLL
** ============================================================================
*/
/*!
    Disable a PLL.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- The PLL was succesfully disabled.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisablePLL(const ClockConfigPLLType *pConfig);

/* =========================================================================
**  Function : Clock_ConfigurePLL
** =========================================================================*/
/**
  Configures a PLL but do not enable it.  If the PLL is already enabled or
  set to FSM mode, this will fail.

  @param *pConfig [in] -- PLL configuration
  @return
  TRUE -- Configuration was successful
  FALSE -- The PLL was already enabled or in FSM mode.

*/
boolean Clock_ConfigurePLL(const ClockConfigPLLType *pConfig);

/* =========================================================================
**  Function : Clock_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux. 

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ConfigMux (const ClockConfigMuxType *pConfig);

/* =========================================================================
**  Function : Clock_DetectMuxCfg
** =========================================================================*/
/*!
    Detect a clock mux configuration. 

    @param pMuxCfg -  [IN] Clock mux config structure.
    @param CurrentCfg -  [IN] Current hardware Clock mux config structure.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_DetectMuxCfg(const ClockConfigMuxType *pMuxCfg,ClockConfigMuxType *CurrentCfg);

/* =========================================================================
**  Function : Clock_TriggerUpdate
** =========================================================================*/
/*!
    Trigger a clock switch, and wait for a timeout.

    @param cmdReg : HWIO_ADDR(*_CMD_CBCR)
    @return
    TRUE -- Clock switch completed.
    FALSE -- Clock switch timed out.

    @dependencies
    None.

    @sa None
*/
boolean Clock_TriggerUpdate(uint32 cmdReg, uint32 nMask);

/* =========================================================================
**  Function : Clock_ToggleClock
** =========================================================================*/
/*!
    Enable/Disable a Clock. 

    @param CBCR_addr - Address of the CBCR register
           enable :-  enable/disable the CBCR 
    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_ToggleClock(uint32 CBCR_addr, Clock_CBCRtoggleType clk_enable);

/* =========================================================================
**  Function : Clock_IsPLLEnabled
** =========================================================================*/
/**
  Find out if the PLL is enabled.

  @param *pConfig [in] -- PLL configuration
  @return
  TRUE -- if the PLL was enabled
  FALSE -- The PLL was disabled.

*/
boolean Clock_IsPLLEnabled(const ClockConfigPLLType *pConfig);

#endif /* CLOCKSBLCOMMON_H */
