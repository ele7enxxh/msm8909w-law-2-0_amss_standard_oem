/** @file ClockSBLConfig.c

  Definitions of the support clock perf level for the 8974.

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockSBLConfig.c#1 $
  $DateTime: 2015/10/01 02:54:06 $
  $Author: pwbldsvc $

 when           who     what, where, why
 --------    ---     -----------------------------------------------------------
09/05/12     vph     Set GPLL3 to 200MHz for configure BIMC @ 200MHz with Div-1
08/06/11     vtw     Added SDC clock configurations.
07/15/11     vtw     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockSBLConfig.h"
#include "ClockSBLCommon.h"
#include "ClockHWIO.h"

/*=========================================================================
      Prototypes
==========================================================================*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_SourceMapToAPCS(ClockSourceType eSource, uint32 *nMuxValue);

/*=========================================================================
      Data
==========================================================================*/


Clock_SBLConfigType Clock_SBLConfigData =
{

  /*GPLL0 @ 800MHZ*/
  .PLL0_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL0_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_APCS_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0),
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x29, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0xAAAA000000,
    .nConfigCtl    =  0x4001055B, 
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },
  /*GPLL1 @ 614.4MHZ*/    
  .PLL1_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL1_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_APCS_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL1),
    .nVCO          =  0, 
    .nPreDiv       =  1, 
    .nPostDiv      =  2, 
    .nL            =  0x40, 
    .nM            =  0,
    .nN            =  1,
    .nConfigCtl    =  0x00031000,
    .ePLL          =  CLK_SOURCE_PLL_SR
  },
  /* GPLL2 @ 792MHz */
  .PLL2_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL2_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_RPM_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_RPM_GPLL_ENA_VOTE, GPLL2),
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x29, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0x4000000000,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER   
  },


  /* BIMCPLL @ 768MHz. */
  .BIMCPLL_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_BIMC_PLL_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_RPM_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_RPM_GPLL_ENA_VOTE, BIMC_PLL),
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x28, 
    .nM            =  0,
    .nN            =  0,
    .nAlpha        =  0x0,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  },
  
  /* A7SSPLL @ 998.4MHZ*/
  .A7PLL_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(APCS_SH_PLL_MODE), 
    .nVoteAddr     =  0,
    .nVoteMask     =  0,
    .nVCO          =  0, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  0x34,
    .nM            =  0,
    .nN            =  1,
    .nConfigCtl    =  0x40014665,
    .ePLL          =  CLK_SOURCE_PLL_SR2
  },
 
  .CPU_Cfg = 
  {
    {0, {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0}},                                    /* PERF NONE */
    {19200, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_CXO,    2, 0, 0, 0}},  /* MIN - 19.2 MHz*/
    {400000, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_GPLL0,  4, 0, 0, 0}},  /* NOM - 400 MHz*/
    {800000, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_GPLL0,  2, 0, 0, 0}},  /* MAX - 800 MHz*/
    {800000, {HWIO_ADDR(APCS_CMD_RCGR), MUX_APCS, SRC_GPLL0,  2, 0, 0, 0}}   /* DEFAULT - 800 MHz */
  },

  .SNOC_Cfg = 
  {
    {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                          /* PERF NONE */
    {HWIO_ADDR(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR), MUX_GCC, SRC_CXO,  1, 0, 0, 0},  /* MIN - 19.2 MHz*/
    {HWIO_ADDR(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0},  /* NOM - 100 MHz*/
    {HWIO_ADDR(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR), MUX_GCC, SRC_GPLL0,  8, 0, 0, 0},  /* MAX - 200 MHz*/
    {HWIO_ADDR(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}   /* DEFAULT - SAME AS NOMINAL */
  },

  .PCNOC_Cfg = 
  {
    {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                          /* PERF NONE */
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_CXO,  1, 0, 0, 0},  /* MIN - 19.2 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 32, 0, 0, 0},  /* NOM - 50 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0},  /* MAX - 100 MHz*/
    {HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR ), MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}   /* DEFAULT - 100 MHz (Max Nom) */
  },

  /* SDC configuration : for backwards compatiblity to the old API */
  .SDC_Cfg =
  {
    0,       /* PERF NONE */
    400,     /* MIN - 400KHz */
    25000,   /* NOMINAL - 25MHz */
    50000,   /* MAX - 50MHz */
    25000    /* DEFAULT - SAME AS NOMINAL */
  }, /* END SDC config */

  /*  SDC extended configurations */
  .SDC1_Ext_Cfg =
  {
    {   400,  {0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, MUX_GCC, SRC_GPLL0, 32, 1, 2, 2}},
    { 50000,  {0, MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}},
    {100000,  {0, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}},
    {177000,  {0, MUX_GCC, SRC_GPLL0,  9, 0, 0, 0}},
    { 0 }
  }, /* END SDC1_Ext_Cfg */

  .SDC2_Ext_Cfg =
  {
    {   400,  {0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, MUX_GCC, SRC_GPLL0, 32, 1, 2, 2}},
    { 50000,  {0, MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}},
    {100000,  {0, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}},
    {200000,  {0, MUX_GCC, SRC_GPLL0,  8, 0, 0, 0}},
    { 0 }
  }, /* END SDC2_Ext_Cfg */

  /* Crypto configuration CE_Cfg : 160 MHz */
  .CE_Cfg = 
  {
    HWIO_ADDR(GCC_CRYPTO_CMD_RCGR),
    MUX_GCC, SRC_GPLL0,  /* eSource */
    10, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  /* USB configuration USB_Cfg : 80 MHz */
  .USB_Cfg =
  {
    HWIO_ADDR(GCC_USB_HS_SYSTEM_CMD_RCGR),
    MUX_GCC, SRC_GPLL0,  /* eSource */
    20, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  /*UART configurations UART_cfg : 3.6864 MHz for UART1 to UART6 clocks*/
  .UART_Cfg =
  {
    {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                                   /* PERF-NONE */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /*MIN - 3.6864 MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* NOMINAL - 3.6864 MHz MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* MAX - 3.6864 MHz MHz */
    {HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 1, 72, 15625, 15625}, /* DEFAULT - SAME AS NOMINAL */    
  },
  
  /* RPM configuration : 100 MHz */
  .RPM_Cfg = 
  {
    HWIO_ADDR(GCC_RPM_CMD_RCGR), /*CMD RCGR register*/
    MUX_GCC, SRC_GPLL0,  /* eSource */
    16, /* nDiv2x */
    0,0,0 /* M/N:D */
  },

  .I2C_Cfg = 
  {
     {0, MUX_GCC, SRC_CXO, 0, 0, 0, 0},                                              /* PERF-NONE */
     {HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_CXO,    1, 0, 0, 0}, /* MIN - 19.2 MHz */
     {HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_GPLL0, 32, 0, 0, 0}, /* MAX - 50 MHz */
     {HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR), MUX_GCC, SRC_CXO,    1, 0, 0, 0}, /* DEFAULT - 19.2 MHz */
   },
};

boolean (*Clock_SBL_MuxMap[NUM_MUX_TYPES])(ClockSourceType, uint32 *) =
{
  Clock_SourceMapToGCC, 
  Clock_SourceMapToAPCS
};

/*=========================================================================
      Functions
==========================================================================*/

/* ============================================================================
**  Function : Clock_SBLConfig
** ============================================================================
*/
/*!
    Return a pointer to the SBL configuration data.

   @param  None

   @retval a pointer to the SBL configuration data

*/
Clock_SBLConfigType *Clock_SBLConfig( void )
{
  return &Clock_SBLConfigData;
}

/* ============================================================================
**  Function : Clock_SourceMapToMux
** ============================================================================
*/
boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
)
{
  if( (pConfig == NULL) ||
      (nMuxValue == NULL) ||
      pConfig->eMux >= NUM_MUX_TYPES )
  {
    return FALSE;
  }

  return Clock_SBL_MuxMap[pConfig->eMux](pConfig->eSource, nMuxValue);
}


/* ============================================================================
**  Function : Clock_MuxMapToSource
** ============================================================================
*/
boolean Clock_MuxMapToSource
(
  ClockConfigMuxType *pConfig,
  uint32 nSource
)
{
  return TRUE;
}


/* ============================================================================
**  Function : Clock_SourceMapToGCC
** ============================================================================
*/
/*!
   Map the source enumeration to a physical mux setting for GCC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the SBL configuration data

*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
      break;
    case SRC_BIMCPLL:
      *nMuxValue = 2;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SourceMapToAPCS
** ============================================================================
*/
/*!
   Map a ClockSourceType into a physical mux setting for the APCS muxes.

   @param  None

   @retval a pointer to the SBL configuration data

*/
boolean Clock_SourceMapToAPCS(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 4;
      break;
    case SRC_A7PLL:
      *nMuxValue = 5;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

/* ============================================================================
**  Function : Clock_EnableSource
** ============================================================================
*/

boolean Clock_EnableSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;

    case SRC_GPLL0:
      rtrn = Clock_EnablePLL(&Clock_SBLConfigData.PLL0_Cfg);
      break;
    
    case SRC_GPLL1:
      rtrn = Clock_EnablePLL(&Clock_SBLConfigData.PLL1_Cfg);
      break;

    case SRC_GPLL2:
      if(HWIO_INF(TCSR_SOC_HW_VERSION,MAJOR_VERSION)>=0x2)
      {
         HWIO_OUTF(GCC_GPLL2_TEST_CTL_U, IDCO_SCALE, 0x1);
      } 
      rtrn = Clock_EnablePLL(&Clock_SBLConfigData.PLL2_Cfg);
      break;

    case SRC_BIMCPLL:
      if(HWIO_INF(TCSR_SOC_HW_VERSION,MAJOR_VERSION)>=0x2)
      { 
          HWIO_OUTF(GCC_BIMC_PLL_TEST_CTL_U, IDCO_SCALE, 0x1);
      }
      rtrn = Clock_EnablePLL(&Clock_SBLConfigData.BIMCPLL_Cfg);
      /* Early Output for bimc_ddr_clk_src */
      HWIO_OUTF(GCC_BIMC_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;

    case SRC_A7PLL:
      rtrn = Clock_EnablePLL(&Clock_SBLConfigData.A7PLL_Cfg);
      HWIO_OUTF(APCS_SH_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;

    default:
      break;
  }
  return rtrn;
}

/* ============================================================================
**  Function : Clock_ConfigureSource
** ============================================================================
*/
boolean Clock_ConfigureSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;
    case SRC_GPLL0:
      rtrn = Clock_ConfigurePLL(&Clock_SBLConfigData.PLL0_Cfg);
      /*Enabling Aux output for QDSS*/
      HWIO_OUTF(GCC_GPLL0_USER_CTL, PLLOUT_LV_AUX, 1);
      break;
    case SRC_GPLL1:
      rtrn = Clock_ConfigurePLL(&Clock_SBLConfigData.PLL1_Cfg);
      /*Enabling Aux output for Cam SS*/
      HWIO_OUTF(GCC_GPLL1_USER_CTL, PLLOUT_LV_AUX, 1);
      /*Enabling Early output for GFX*/
      HWIO_OUTF(GCC_GPLL1_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;
    case SRC_GPLL2:
      if(HWIO_INF(TCSR_SOC_HW_VERSION,MAJOR_VERSION)>=0x2)
      {
         HWIO_OUTF(GCC_GPLL2_TEST_CTL_U, IDCO_SCALE, 0x1);
      } 
      rtrn = Clock_ConfigurePLL(&Clock_SBLConfigData.PLL2_Cfg);
      /*Enabling Aux output for QDSS*/
      HWIO_OUTF(GCC_GPLL2_USER_CTL, PLLOUT_LV_AUX , 1);
      break;
    case SRC_BIMCPLL:
      if(HWIO_INF(TCSR_SOC_HW_VERSION,MAJOR_VERSION)>=0x2)
      { 
          HWIO_OUTF(GCC_BIMC_PLL_TEST_CTL_U, IDCO_SCALE, 0x1);
      }
      rtrn = Clock_ConfigurePLL(&Clock_SBLConfigData.BIMCPLL_Cfg);
      /* Early Output for bimc_ddr_clk_src */
      HWIO_OUTF(GCC_BIMC_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;
    case SRC_A7PLL:
      rtrn = Clock_ConfigurePLL(&Clock_SBLConfigData.A7PLL_Cfg);
      HWIO_OUTF(APCS_SH_PLL_USER_CTL, PLLOUT_LV_EARLY, 1);
      break;

    default:
      break;
  }
  return rtrn;
}
