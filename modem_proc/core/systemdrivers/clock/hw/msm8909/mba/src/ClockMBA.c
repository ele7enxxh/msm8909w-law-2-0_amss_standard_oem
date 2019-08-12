/*
===========================================================================
*/
/**
  @file ClockMBA.c

  Implementation file for the MBA's MSS clock API.
*/
/*
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mba/src/ClockMBA.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include <HALhwio.h>
#include "ClockMBAHWIO.h"


/*=========================================================================
      Defines
==========================================================================*/


/*
 * RCGR update timeout. In practice this should be very short (less than 1us),
 * but it depends on the clock frequency being fed into the RCG. Choosing
 * a very conservative value.
 */
#define CLOCK_MBA_UPDATE_TIMEOUT_US 1000


/*
 * Common clock branch fields/masks (*_CBCR register)
 */
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK                           0x80000000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_SHFT                           31
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK                           0x01FF0000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT                           16
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK             0x00004000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK           0x00002000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK          0x00001000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK                        0x00000001
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_SHFT                        0
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IDCO_SCALE_BMSK                0xC00000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IDCO_SCALE_SHFT                22

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)               (sizeof(x) / sizeof(x[0]))
#endif


/*
 * HAL_CLK_FMSK
 *
 * Define a register mask data structure given the register name
 * and field.
 */
#define HAL_CLK_FMSK(io, field)   \
  {                               \
    HWIO_ADDR(io),                \
    HWIO_FMSK(io, field)          \
  }


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
 * mHWVotable   - Register/mask for HW votable configuration setup.
 */
typedef struct
{
  uint32                    nBranchAddr;
  HAL_clk_RegisterMaskType  mHWVotable;
} HAL_clk_ClockRegistersType;

/*
 * PLL2 calibration L-value = 28 --> 537.6MHz
 */
#define PLL2_CALIB_L_VAL 28

/*
 * PLL2 default L-value = 30 --> 576MHz
 */
#define PLL2_DEFAULT_L_VAL 30

/*
 * This is the VCO selection value necessary for the operational frequency
 * range we will be slewing the Q6 PLL within (Low Band: 350 MHz- 700 GHz)
 */
#define PLL2_VCO_SEL 0x2

/*
 * PLL lock timeout (2x max locking time).
 */
#define CLOCK_MBA_PLL_LOCK_TIMEOUT_US 200

/*
 * Warm-up time in microseconds after turning on the PLL.
 */
#define CLOCK_MBA_PLL_WARMUP_TIME_US 50

/*
 * Update time in microseconds setting the PLL UPDATE bit (2x HPG specs)
 */
#define CLOCK_MBA_PLL_UPDATE_TIME_US 2

/*
 * Standby time in microseconds after setting PLL BYPASSNL=1
 */
#define CLOCK_MBA_PLL_STANDBY_TIME_US 5

/*
 * Standby time in microseconds after setting RCGR UPDATE=1
 */
#define  CLOCK_MBA_RCGR_UPDATE_TIMEOUT_US 1000

/*
 * SW override for the ACC settings
 */
#define CLOCK_DEFAULT_STRAP_ACC 0x20


/*=========================================================================
      External and forward references
==========================================================================*/

extern void mba_modem_wait_microseconds(uint32 microseconds);

boolean WaitForClear(uint32 nAddr, uint32 nMask, uint32 nWait);
boolean WaitForSet(uint32 nAddr, uint32 nMask, uint32 nWait);
static boolean ClockMBA_EnablePLLDynProg(void);



/*=========================================================================
      Data
==========================================================================*/


static const HAL_clk_ClockRegistersType aClockRegisters[] =
{
  /*
   * Clock Name: gcc_prng_ahb_clk
   */
  {
    .nBranchAddr = HWIO_ADDR(GCC_PRNG_AHB_CBCR),
    .mHWVotable  = HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA)
  },

  /*
   * Clock Name: gcc_dehr_clk
   */
  {
    .nBranchAddr = HWIO_ADDR(GCC_DEHR_CBCR),
    .mHWVotable  = {0, 0}
  },

  /*
   * Clock Name: clk_nav_snoc_axi
   */
  {
    .nBranchAddr = HWIO_ADDR(MSS_NAV_SNOC_AXI_CBCR),
    .mHWVotable  = {0, 0}
  },

  /*
   * Clock Name: clk_bus_nav_ce_bridge
   */
  {
    .nBranchAddr = HWIO_ADDR(MSS_BUS_NAV_CE_BRIDGE_CBCR),
    .mHWVotable  = {0, 0}
  },

  /*
   * Clock Name: clk_bus_crypto
   */
  {
    .nBranchAddr = HWIO_ADDR(MSS_BUS_CRYPTO_CBCR),
    .mHWVotable  = {0, 0}
  },

};


/*=========================================================================
      Functions
==========================================================================*/


/* ===========================================================================
**  ClockMBA_EnableClock
**
** ======================================================================== */
/**
  Enable a clock using the specified register and mask.

  @param
  None

  @return
  TRUE  -- Successfully enabled clock.
  FALSE -- Failed to enable clock.
  None

  @dependencies
  None
*/

static boolean ClockMBA_EnableClock
(
  const HAL_clk_ClockRegistersType *pRegisters
)
{
  uint32 nAddr, nVal, nMask;
  uint32 nTimeout = 0;

  /*
   * Sanity check - All clocks must have a CBCR
   */
  if (pRegisters->nBranchAddr == 0)
  {
    return FALSE;
  }

  /*
   * Check for HW votable registers
   */
  nAddr = pRegisters->mHWVotable.nAddr;
  if(nAddr)
  {
    nMask = pRegisters->mHWVotable.nMask;
  }
  else
  {
    nAddr = pRegisters->nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return FALSE;
  }

  /*
   * Enable clock
   */
  nVal  = inpdw(nAddr) | nMask;
  outpdw(nAddr, nVal);

  /*
   * Wait for the clock to turn on
   */
  nAddr = pRegisters->nBranchAddr;
  while (inpdw(nAddr) & HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK)
  {
    /*
     * Check for timeout
     */
    if (nTimeout++ >= CLOCK_MBA_UPDATE_TIMEOUT_US)
    {
      return FALSE;
    }

    /*
     * Add enable delay
     */
    mba_modem_wait_microseconds(1);
  }

  return TRUE;

} /* ClockMBA_EnableClock */


/* ===========================================================================
**  ClockMBA_DisableClock
**
** ======================================================================== */
/**
  Disable a clock using the specified register and mask.

  @param
  None

  @return
  None

  @dependencies
  None
*/

static void ClockMBA_DisableClock
(
  const HAL_clk_ClockRegistersType *pRegisters
)
{
  uint32 nAddr, nVal, nMask;

  /*
   * Check for HW votable registers
   */
  nAddr = pRegisters->mHWVotable.nAddr;
  if(nAddr)
  {
    nMask = pRegisters->mHWVotable.nMask;
  }
  else
  {
    nAddr = pRegisters->nBranchAddr;
    nMask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
  }

  /*
   * Sanity check
   */
  if(nAddr == 0)
  {
    return;
  }

  /*
   * Disable clock
   */
  nVal  = inpdw(nAddr);
  nVal &= ~nMask;
  outpdw(nAddr, nVal);

} /* ClockMBA_DisableClock */


/* =========================================================================
**  Function : ClockMBA_Init
** =========================================================================*/
/**
  Initialize the MBA clock driver.

  @param
  None

  @return
  TRUE -- Init was successful.
  FALSE -- Init failed.

  @dependencies
  None
*/

boolean ClockMBA_Init(void)
{
  uint32 i = 0;

  /* 
   * PBL has Q6/BUS @ 384/72
   * MBA needs to Bump up Q6/BUS to 576/144
   */

  /* 
   * Program  MSS_BUS_CFG_RCGR
   *  MSS HPG 5.5.2 step 1, table 5-11.
   * SRC_SEL = MPLL1_OUT_MAIN
   * SRC_DIV = 576 Div4
   */
  HWIO_OUTF(MSS_BUS_CFG_RCGR, SRC_SEL, 1);
  HWIO_OUTF(MSS_BUS_CFG_RCGR, SRC_DIV, 0x07);

  /* Initiate  MSS_BUS_CMD_RCGR update */ 
  HWIO_OUTF(MSS_BUS_CMD_RCGR, UPDATE, 1);

  if(!WaitForClear( HWIO_ADDR(MSS_BUS_CMD_RCGR),
           HWIO_FMSK(MSS_BUS_CMD_RCGR, UPDATE),
           CLOCK_MBA_RCGR_UPDATE_TIMEOUT_US))
  {
    return FALSE;
  }

  /* Now we are at 384/144*/

  /*-----------------------------------------------------------------------*/
  /*  Enable PLL dynamic programming.                                      */
  /*-----------------------------------------------------------------------*/

  if (ClockMBA_EnablePLLDynProg() != TRUE)
  {
    return FALSE;
  }

  /* Now we are at 576/144*/

 /*-----------------------------------------------------------------------*/
  /*  Enable clocks.                                                       */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARRAY_SIZE(aClockRegisters); i++)
  {
    if (ClockMBA_EnableClock(&aClockRegisters[i]) != TRUE)
    {
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Init complete.                                                        */
  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* END ClockMBA_Init */


/* =========================================================================
**  Function : ClockMBA_DeInit
** =========================================================================*/
/**
  Deinitialize the MBA clock driver.

  @param
  None

  @return
  TRUE -- Deinit was successful.
  FALSE -- Deinit failed.

  @dependencies
  None
*/

boolean ClockMBA_DeInit(void)
{
  /*-----------------------------------------------------------------------*/
  /*  Disable clocks (reverse order of init).                              */
  /*-----------------------------------------------------------------------*/

  ClockMBA_DisableClock(&aClockRegisters[4]);
  ClockMBA_DisableClock(&aClockRegisters[3]);
  ClockMBA_DisableClock(&aClockRegisters[2]);
  ClockMBA_DisableClock(&aClockRegisters[1]);
  ClockMBA_DisableClock(&aClockRegisters[0]);

  /*-----------------------------------------------------------------------*/
  /* Deinit complete.                                                      */
  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* END ClockMBA_DeInit */


/* ===========================================================================
**  ClockMBA_EnablePLLDynProg
**
** ======================================================================== */
/**
  Enable PLL dynamic programming.

  @param
  None

  @return
  TRUE  -- Successfully enabled PLL dynamic programming mode.
  FALSE -- Failed to enable.
  None

  @dependencies
  None
*/

static boolean ClockMBA_EnablePLLDynProg
(
  void
)
{

    /* 
     * 1. Patch MPLL2 Config Ctl register to 
     * post-Si value
     */
    HWIO_OUT(MSS_QDSP6SS_PLL_CONFIG_CTL, 0x4001055B);
    /* 
     * 2. Write the calibration value
     */
    HWIO_OUT(MSS_QDSP6SS_PLL_L_VAL, PLL2_CALIB_L_VAL); 
    HWIO_OUT(MSS_QDSP6SS_PLL_ALPHA_VAL, 0); 
    HWIO_OUT(MSS_QDSP6SS_PLL_ALPHA_VAL_U, 0);
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL, VCO_SEL, PLL2_VCO_SEL);
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL, POST_DIV_RATIO, 0x0);
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL, POST_DIV_RATIO, 0x0);
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL, ALPHA_EN, 0x0);
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL_U, LOCK_DET, 0x1);

    /* Updating pll_test_ctl_u[22:23] to 0x01, This will increase
     * internal DCO current(Icore) by 14% instead of 7%(default settings)
     * & provides more robustness/margin to PLL performance @ Cold(-40C) */ 
    if(HWIO_INF(TCSR_SOC_HW_VERSION, MAJOR_VERSION) > 0x1)
    { 
      HWIO_OUTF(MSS_QDSP6SS_PLL_TEST_CTL_U, IDCO_SCALE, 0x1);
    }

    /* enable dynamic programming */ 
    HWIO_OUTF(MSS_QDSP6SS_PLL_USER_CTL_U, LATCH_INTERFACE_BYPASS, 0);  
    HWIO_OUTF(MSS_QDSP6SS_PLL_MODE, PLL_BYPASSNL, 1);
    mba_modem_wait_microseconds(CLOCK_MBA_PLL_STANDBY_TIME_US);
    HWIO_OUTF(MSS_QDSP6SS_PLL_MODE, PLL_RESET_N, 1);
    mba_modem_wait_microseconds(CLOCK_MBA_PLL_WARMUP_TIME_US);
    /* wait for the PLL to lock */
    if( !WaitForSet(HWIO_ADDR(MSS_QDSP6SS_PLL_MODE),  
                                 HWIO_FMSK(MSS_QDSP6SS_PLL_MODE, PLL_LOCK_DET),
                                 CLOCK_MBA_PLL_LOCK_TIMEOUT_US))  
    {
      return FALSE;
    }
    /* enable the PLL output */
    HWIO_OUTF(MSS_QDSP6SS_PLL_MODE, PLL_OUTCTRL, 1);  

    /* 
     * 3. Slew the PLL to its default operational frequency
     */
    HWIO_OUT(MSS_QDSP6SS_PLL_L_VAL, PLL2_DEFAULT_L_VAL); 
    HWIO_OUT(MSS_QDSP6SS_PLL_ALPHA_VAL, 0); 
    HWIO_OUT(MSS_QDSP6SS_PLL_ALPHA_VAL_U, 0); 

    HWIO_OUTF(MSS_QDSP6SS_PLL_MODE, PLL_UPDATE, 1);
    /* wait for PLL update bit to clear */
    if( !WaitForClear(HWIO_ADDR(MSS_QDSP6SS_PLL_MODE),  
                                 HWIO_FMSK(MSS_QDSP6SS_PLL_MODE, PLL_UPDATE),
                                 CLOCK_MBA_PLL_UPDATE_TIME_US))  
    {
      return FALSE;
    }
    mba_modem_wait_microseconds(1);
    /* wait for PLL lock detect to be asserted */
    if( !WaitForSet(HWIO_ADDR(MSS_QDSP6SS_PLL_MODE),  
                                 HWIO_FMSK(MSS_QDSP6SS_PLL_MODE, PLL_LOCK_DET),
                                 CLOCK_MBA_PLL_LOCK_TIMEOUT_US))  
    {
      return FALSE;
    }

    /* 
     * 4. Switch the Q6 to the PLL source
     */
    /* select PLL2 source */
    HWIO_OUTF(MSS_QDSP6SS_CORE_CFG_RCGR, SRC_SEL, 1); 
   
    /* Running Q6 at 288Mhz for 8909 V1.0 */ 
    if(HWIO_INF(TCSR_SOC_HW_VERSION, MAJOR_VERSION) == 0x1)
    { 
       HWIO_OUTF(MSS_QDSP6SS_CORE_CFG_RCGR, SRC_DIV, 3); 
    }
    else
    {
       HWIO_OUTF(MSS_QDSP6SS_CORE_CFG_RCGR, SRC_DIV, 1); 
    }
    /* Initiate MSS_QDSP6SS_CORE_CMD_RCGR update */
    HWIO_OUTF(MSS_QDSP6SS_CORE_CMD_RCGR, UPDATE, 1);    
    /* wait with timeout for register update */
    if( !WaitForClear(HWIO_ADDR(MSS_QDSP6SS_CORE_CMD_RCGR),   
                                 HWIO_FMSK(MSS_QDSP6SS_CORE_CMD_RCGR, UPDATE),
                                 CLOCK_MBA_UPDATE_TIMEOUT_US))  
    {
      return FALSE;
    }

    /* 
     * 5. Switch GFMUX to select QDSP6SS_CORE Clock
     */
    HWIO_OUTF(MSS_QDSP6SS_GFMUX_CTL, CLK_SRC_SEL, 0);
    if( !WaitForClear(HWIO_ADDR(MSS_QDSP6SS_GFMUX_STATUS),
                      HWIO_FMSK(MSS_QDSP6SS_GFMUX_STATUS, SWITCH_STATUS),
                      CLOCK_MBA_UPDATE_TIMEOUT_US))
    {
      return FALSE;
    }


    return TRUE;

} /* END ClockMBA_EnablePLLDynProg */


/* =========================================================================
**  Function : WaitForClear
** =========================================================================*/
/**
  Checks for a clock update.

  @param nAddr [in] -- Address of the register to check for the update.
  @param nMask [in] -- Mask for the update bit to check.
  @param nWait [in] -- Wait delay counter.
*/

boolean WaitForClear(uint32 nAddr, uint32 nMask, uint32 nWait)
{
  uint32 nTimeout = 0;
  /*
   * Wait until bit is cleared
   */
  while(inpdw(nAddr) & nMask)
  {
    if (nTimeout++ >= nWait)
    {
      return FALSE;
    }

    mba_modem_wait_microseconds(1);
  }

  return TRUE;
} /* END WaitForClear */


/* =========================================================================
**  Function : WaitForSet
** =========================================================================*/
/**
  Checks for a clock update.

  @param nAddr [in] -- Address of the register to check for the update.
  @param nMask [in] -- Mask for the update bit to check.
  @param nWait [in] -- Wait delay counter.
*/

boolean WaitForSet(uint32 nAddr, uint32 nMask, uint32 nWait)
{
  uint32 nTimeout = 0;
  /*
   * Wait until bit is set.
   */
  while((inpdw(nAddr) & nMask) == 0)
  {
    if (nTimeout++ >= nWait)
    {
      return FALSE;
    }

    mba_modem_wait_microseconds(1);
  }

  return TRUE;
} /* END WaitForSet */

