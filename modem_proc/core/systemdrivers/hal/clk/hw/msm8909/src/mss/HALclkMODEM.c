/*
==============================================================================

FILE:         HALclkMODEM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   MODEM clocks.

   List of clock domains:
   -HAL_clk_mMSSPLL0MAINClkDomain
   -HAL_clk_mMSSPLL1EARLYDIV3ClkDomain
   -HAL_clk_mMSSPLL1MAINClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/mss/HALclkMODEM.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
10/01/2013            Auto-generated.

==============================================================================
            Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


void HAL_clk_ModemDiv3DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
);


void HAL_clk_ModemDiv5DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
);


void HAL_clk_MPLL0MAINCLKDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
);


void HAL_clk_MPLL1MAINCLKDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
);


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_mModemDiv3ClockDomainControl
 *
 * Functions for controlling ModemDiv3 clock domain
 */
HAL_clk_ClockDomainControlType HAL_clk_mModemDiv3ClockDomainControl =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_ModemDiv3DetectMuxConfig,
   /* .pmSourceMap        = */ NULL
};


/*
 * HAL_clk_mModemDiv5ClockDomainControl
 *
 * Functions for controlling ModemDiv5 clock domain
 */
HAL_clk_ClockDomainControlType HAL_clk_mModemDiv5ClockDomainControl =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_ModemDiv5DetectMuxConfig,
   /* .pmSourceMap        = */ NULL
};

/*
 * HAL_clk_mMPLL0MainClockDomainControl
 *
 * Functions for controlling ModemDiv3 clock domain
 */
HAL_clk_ClockDomainControlType HAL_clk_mMPLL0MainClockDomainControl =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_MPLL0MAINCLKDetectMuxConfig,
   /* .pmSourceMap        = */ NULL
};


/*
 * HAL_clk_mMPLL1MainClockDomainControl
 *
 * Functions for controlling ModemDiv5 clock domain
 */
HAL_clk_ClockDomainControlType HAL_clk_mMPLL1MainClockDomainControl =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_MPLL1MAINCLKDetectMuxConfig,
   /* .pmSourceMap        = */ NULL
};

/*
 * HAL_clk_mModemClockControl
 *
 * Functions for controlling the Modem clocks.
 */
HAL_clk_ClockControlType HAL_clk_mModemClockControl =
{
  /* .Enable           = */ HAL_clk_GenericEnable,
  /* .Disable          = */ HAL_clk_GenericDisable,
  /* .IsEnabled        = */ HAL_clk_GenericIsEnabled,
  /* .IsOn             = */ HAL_clk_GenericIsOn,
  /* .Reset            = */ NULL,
  /* .IsReset          = */ NULL,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ NULL,
  /* .DetectDivider    = */ NULL
};


 #if 0 //to avoid compilation error. this register doesnot exist. need to discuss with MSS clock designer
/*                           
 *  HAL_clk_mPLL0MAINClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPLL0MAINClkDomainClks[] =
{
  {
    /* .szClockName      = */ "mpll0_main_modem_cx_to_ls",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL0_MAIN_MODEM_CX_TO_LS_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mModemClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_MPLL0_MAIN_MODEM_CX_TO_LS
  },
};


/*
 * HAL_clk_mMSSPLL0MAINClkDomain
 *
 * PLL0 MAIN clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL0MAINClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mPLL0MAINClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPLL0MAINClkDomainClks)/sizeof(HAL_clk_mPLL0MAINClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMPLL0MainClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
#endif


/*                           
 *  HAL_clk_mPLL1EARLYDIV3ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPLL1EARLYDIV3ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "mpll1_early_div3_modem_cx_to_ls",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL1_EARLY_DIV3_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mModemClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_MPLL1_EARLY_DIV3_MODEM_CX_TO_LS
  },
};

/*                           
 *  HAL_clk_mPLL1EARLYDIV5ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPLL1EARLYDIV5ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "mpll1_early_div5",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL1_EARLY_DIV5_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mModemClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_MPLL1_EARLY_DIV5
  },
};


/*
 * HAL_clk_mMSSPLL1EARLYDIV3ClkDomain
 *
 * PLL1 EARLY DIV3 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1EARLYDIV3ClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mPLL1EARLYDIV3ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPLL1EARLYDIV3ClkDomainClks)/sizeof(HAL_clk_mPLL1EARLYDIV3ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mModemDiv3ClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
/*
 * HAL_clk_mMSSPLL1EARLYDIV5ClkDomain
 *
 * PLL1 EARLY DIV5 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1EARLYDIV5ClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mPLL1EARLYDIV5ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPLL1EARLYDIV5ClkDomainClks)/sizeof(HAL_clk_mPLL1EARLYDIV5ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mModemDiv5ClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};




                                    
/*                           
 *  HAL_clk_mPLL1MAINClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPLL1MAINClkDomainClks[] =
{
  {
    /* .szClockName      = */ "mpll1_main_modem_cx_to_ls",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL1_MAIN_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mModemClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_MPLL1_MAIN_MODEM_CX_TO_LS
  },
};


/*
 * HAL_clk_mMSSPLL1MAINClkDomain
 *
 * PLL1 MAIN clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1MAINClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mPLL1MAINClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPLL1MAINClkDomainClks)/sizeof(HAL_clk_mPLL1MAINClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMPLL1MainClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_ModemDiv3DetectMuxConfig
**
** ======================================================================== */

void HAL_clk_ModemDiv3DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  if (pmConfig == NULL)
  {
    return;
  }

  /*
   * Manually match up the BSP config so the domain points to the
   * correct source.
   */
  pmConfig->eSource = HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3;
  pmConfig->nDiv2x  = 2;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

} /* END HAL_clk_ModemDiv3DetectMuxConfig */


/* ===========================================================================
**  HAL_clk_ModemDiv5DetectMuxConfig
**
** ======================================================================== */

void HAL_clk_ModemDiv5DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  if (pmConfig == NULL)
  {
    return;
  }

  /*
   * Manually match up the BSP config so the domain points to the
   * correct source.
   */
  pmConfig->eSource = HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5;
  pmConfig->nDiv2x  = 2;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

} /* END HAL_clk_ModemDiv5DetectMuxConfig */

/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_ModemDiv3DetectMuxConfig
**
** ======================================================================== */

void HAL_clk_MPLL0MAINCLKDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  if (pmConfig == NULL)
  {
    return;
  }

  /*
   * Manually match up the BSP config so the domain points to the
   * correct source.
   */
  pmConfig->eSource = HAL_CLK_SOURCE_MPLL0;
  pmConfig->nDiv2x  = 2;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

} /* END HAL_clk_ModemDiv3DetectMuxConfig */


/* ===========================================================================
**  HAL_clk_ModemDiv5DetectMuxConfig
**
** ======================================================================== */

void HAL_clk_MPLL1MAINCLKDetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  if (pmConfig == NULL)
  {
    return;
  }

  /*
   * Manually match up the BSP config so the domain points to the
   * correct source.
   */
  pmConfig->eSource = HAL_CLK_SOURCE_MPLL1;
  pmConfig->nDiv2x  = 2;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

} /* END HAL_clk_ModemDiv5DetectMuxConfig */
