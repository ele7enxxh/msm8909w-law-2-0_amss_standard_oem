/*
===========================================================================
  @file ClockSBL.c

  This file provides clock initialization for the Apps SBL.
===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  =========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockSBL.c#1 $
  $DateTime: 2015/10/01 02:54:06 $
  $Author: pwbldsvc $
    when		   who	   what, where, why
   --------    ---	   -----------------------------------------------------------
  
  11/05/14	   yps	 Change BIMC Clock to turbo mode for DDI tool.

  =========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockSBL.h"
#include "ClockHWIO.h"
#include "ClockSBLConfig.h"
#include "ddr_drivers.h" 

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

boolean Clock_InitCrypto(void);

/*=========================================================================
      Function Definitions
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/
struct Clock_I2CRegType 
{
  uint32 cmd;
  uint32 cbcr;
};

const struct Clock_I2CRegType Clock_I2CRegs[CLK_BLSP_QUP_I2C_NUM_CLKS] = 
{ 
  { 0,0 },
  { HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP1_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP2_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP3_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP3_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP4_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP4_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP5_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP5_I2C_APPS_CBCR) },
  { HWIO_ADDR(GCC_BLSP1_QUP6_I2C_APPS_CMD_RCGR),  HWIO_ADDR(GCC_BLSP1_QUP6_I2C_APPS_CBCR) },
};

/*=========================================================================
      Function Definitions
=========================================================================*/
/* ============================================================================
**	Function : Clock_SetUARTClockFrequency
** ============================================================================
*/
/*!
	Configure UART clock to a specific perf level.

	@param    ePerfLevel    -	[IN] UART Clock perf level
		      eClock	-	[IN] UART clock to configure
	@return
	TRUE -- Initialization was successful.
	FALSE -- Initialization failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_SetUARTClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockUARTType eClock
)
{
  ClockConfigMuxType UARTCfg = {0};
  const Clock_SBLConfigType *cfg = Clock_SBLConfig();

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;

  /* Copy the config to the local so the nCMDCGRAddr can be modified. */
  UARTCfg = cfg->UART_Cfg[ePerfLevel];

  if( ! Clock_EnableSource( UARTCfg.eSource )) return FALSE;

  switch (eClock)
  {
    case CLK_BLSP1_UART1_APPS:
      UARTCfg.nCMDCGRAddr = HWIO_ADDR(GCC_BLSP1_UART1_APPS_CMD_RCGR);
      if( ! Clock_ConfigMux(&UARTCfg)) return FALSE;
      Clock_ToggleClock(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, CLK_TOGGLE_ENABLE);
      break;
	  
    case CLK_BLSP1_UART2_APPS:
      UARTCfg.nCMDCGRAddr = HWIO_ADDR(GCC_BLSP1_UART2_APPS_CMD_RCGR);
      if( ! Clock_ConfigMux(&UARTCfg)) return FALSE;
      Clock_ToggleClock(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, CLK_TOGGLE_ENABLE);
      break;
	  
    default:
      return FALSE;
  }

  /* Enable BLSP1 AHB CLK */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 1);

  return TRUE;

} /* END Clock_SetUARTClockFrequency */



/* ============================================================================
**	Function : Clock_DisableUARTClock
** ============================================================================
*/
/*!
	Disable specific UART clock.

	@param    eClock       -	[IN] UART clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableUARTClock(ClockUARTType eClock)
{

  switch (eClock)
  {
    case CLK_BLSP1_UART1_APPS:
      Clock_ToggleClock(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, CLK_TOGGLE_DISABLE);
      break;

    case CLK_BLSP1_UART2_APPS:
      Clock_ToggleClock(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, CLK_TOGGLE_DISABLE);
      break;

     default:
      return FALSE;
  }

  return TRUE;
}

/* ============================================================================
**	Function : Clock_SetI2CClockFrequency
** ============================================================================
*/
/*!
	Configure QUP_I2C clock to a specific perf level.

	@param      ePerfLevel	 -	[IN] UART Clock perf level
		        eClock	 -	[IN] UART clock to configure
	@return
	TRUE -- Initialization was successful.
	FALSE -- Initialization failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_SetI2CClockFrequency
(
  ClockBootPerfLevelType ePerfLevel,
  ClockQUPI2CType eClock
)
{
  ClockConfigMuxType I2CCfg = {0};
  const Clock_SBLConfigType *cfg = Clock_SBLConfig();

  if(ePerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;
  if(eClock >= CLK_BLSP_QUP_I2C_NUM_CLKS) return FALSE;

  /* Copy the config to the local so the nCMDCGRAddr can be modified. */
  I2CCfg = cfg->I2C_Cfg[ePerfLevel];

  if( ! Clock_EnableSource( I2CCfg.eSource )) return FALSE;

  I2CCfg.nCMDCGRAddr = Clock_I2CRegs[eClock].cmd;
  if( ! Clock_ConfigMux(&I2CCfg)) return FALSE;
  Clock_ToggleClock(Clock_I2CRegs[eClock].cbcr, CLK_TOGGLE_ENABLE);

  /* Enable BLSP1 AHB CLK */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 1);


 return TRUE;
} /* END Clock_SetI2CClockFrequency */



/* ============================================================================
**	Function : Clock_DisableI2CClock
** ============================================================================
*/
/*!
	Disable  specific I2C clock.

	@param    eClock	-	[IN] I2C clock to Disable
	@return
	TRUE -- Clock disable was successful.
	FALSE -- Clock disable failed.

	@dependencies
	None.

	@sa None
*/
boolean Clock_DisableI2CClock(ClockQUPI2CType eClock)
{
  if(eClock >= CLK_BLSP_QUP_I2C_NUM_CLKS) return FALSE;
  Clock_ToggleClock(Clock_I2CRegs[eClock].cbcr, CLK_TOGGLE_DISABLE);
  
  return TRUE;
}

/* ============================================================================
**  Function : Clock_DebugInit
** ============================================================================
*/
/*!

    This function is called very early in boot.  It is used for work-arounds that
    need to be done before JTAG attaches at the SBL "hold focus button" spin loop.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    You cannot use busywait in this function.

*/
boolean Clock_DebugInit(void)
{
  /* Enable QDSS register access */
  HWIO_OUT(GCC_GCC_IM_SLEEP_CBCR, 0x1);

  /* Needed for Random Stack Canary */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);

  return TRUE;
}

/* ========================================================================
**  Function : Clock_I2CInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for EEPROM to be used Pre DDR.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_I2CInit( void )
{
  /* Enabled QUP5 I2C Apps clk (fixed at 19.2) and vote for the AHB */
  /* NOTE:  The I2C hardware block numbers their QUPS from 0-5, but 
   * clock control uses 1-6.  We are having off-by-one errors so be
   * wary and clear in communicating about these serial devices. */

  // 8x26 removed BLSP2
  // TODO HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_AHB_CLK_ENA, 1);
  // TODO HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_SLEEP_CLK_ENA, 1);
  // TODO HWIO_OUTF(GCC_BLSP2_QUP5_I2C_APPS_CBCR, CLK_ENABLE, 1);

  /* These clocks can be disabled at Clock_ExitBoot, so keep that function
   * in sync */
  return TRUE;
}

/* ========================================================================
**  Function : Clock_InitVotes
** ======================================================================*/
/*
    Description: This function serves two purposes.  It clears 
    unwanted votes that may be left after watchdog or JTAG reset, which 
    does not actually reset all of GCC.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_InitVotes( void )
{
  uint32 rpm_gpll_votes = 0;

  /* 
   * Correct any left over votes from a watchdog or JTAG induced reset 
   */


  /* RPM needs GPLL0 and maybe GPLL1 if it was configured in PBL */
  if(HWIO_INF(GCC_GPLL0_MODE, PLL_VOTE_FSM_ENA ) > 0) {
    rpm_gpll_votes |= HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0);
  }

  HWIO_OUT(GCC_RPM_GPLL_ENA_VOTE, rpm_gpll_votes);

  /* APCS requires the same PLLs */
  HWIO_OUT(GCC_APCS_GPLL_ENA_VOTE, rpm_gpll_votes);

  /* Everybody else has not booted yet */
  HWIO_OUT(GCC_APCS_TZ_GPLL_ENA_VOTE, 0);
  HWIO_OUT(GCC_MSS_Q6_GPLL_ENA_VOTE, 0);
  HWIO_OUT(GCC_WCSS_GPLL_ENA_VOTE, 0);
  HWIO_OUT(GCC_SPARE_GPLL_ENA_VOTE, 0);

}

/* ============================================================================
**  Function : Clock_DDRSpeed
** ============================================================================
*/
/*!
    Return the DDR clock rate in kilohertz.  This clock rate is the bus speed.
    It is not the controller clock (2X) clock for targets that use Legacy mode.

    @param None.
    @return
      The speed configured in Clock_PreDDRInit() of the BIMC/DDR clock.

    @dependencies
    None.

    @sa None
*/

static uint32 ddr_speed_khz = 384000;

uint32 Clock_DDRSpeed()
{
  return ddr_speed_khz;
}

/* ========================================================================
**  Function : Clock_PreDDRInitEx
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_PreDDRInitEx( uint32 ddr_type )
{
  uint32 mask;
 
  Clock_SBLConfigType *cfg = Clock_SBLConfig();
  const ClockConfigMuxType clkCfg = {HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),  MUX_GCC, SRC_BIMCPLL,  2, 0, 0, 0};
  const ClockConfigMuxType apsstcuclkCfg = {HWIO_ADDR(GCC_APSS_TCU_CMD_RCGR),  MUX_GCC, SRC_GPLL0,  5, 0, 0, 0};
  const ClockConfigMuxType bimcgpuclkCfg = {HWIO_ADDR(GCC_BIMC_GPU_CMD_RCGR),  MUX_GCC, SRC_GPLL0,  8, 0, 0, 0};

  

  /*For Speed bin 0, changing the GPLL2 config to 912Mhz, for other cases it is 792Mhz*/
  if(HWIO_INF(QFPROM_RAW_PTE2,SPEED_BIN) == 0)
  {
    cfg->PLL2_Cfg.nL     = 47;
    cfg->PLL2_Cfg.nAlpha = 0x8000000000;
  }
  
  /* Clean up from watchdog/JTAG reset */
  Clock_InitVotes();

 /* Enable Imem and Msg RAM clock */
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);

  /* Enable the necessary voteable KPSS and BIMC related clocks */
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, SYS_NOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA);
    

  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, mask);
  
  /* SPMI clocks are already enabled by default on reset */
  /* gcc_spmi_ser_clk default on */
  /* gcc_spmi_cnoc_ahb_clk default on */
  /* gcc_spmi_ahb_clk default on */

  /* Enable all possible BIMC and DDR clocks, (K) means KPSS_Boot_Clk_CTL On.
     bimc_ddr_xo_clk_src, root auto enables. */

  Clock_ToggleClock(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
  /* gcc_ddr_dim_cfg_clk */
  Clock_ToggleClock(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  /* gcc_ddr_dim_sleep_clk */
  Clock_ToggleClock(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  // JCPLL only on 8974
  /* gcc_bimc_ddr_jcpll0_clk */ 
  //HWIO_OUTF(GCC_BIMC_DDR_CPLL0_CBCR, CLK_ENABLE, 0);
  /* gcc_bimc_ddr_jcpll1_clk */
  //HWIO_OUTF(GCC_BIMC_DDR_CPLL1_CBCR, CLK_ENABLE, 0);
  /* gcc_bimc_ddr_jcpll0_clk */
  //HWIO_OUTF(GCC_BIMC_DDR_CPLL0_CBCR, CLK_ENABLE, 1);
  /* gcc_bimc_ddr_jcpll1_clk */
  //HWIO_OUTF(GCC_BIMC_DDR_CPLL1_CBCR, CLK_ENABLE, 1);

  /* gcc_bimc_xo_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  /* gcc_bimc_cfg_ahb_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  /* gcc_bimc_sleep_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  /* (K)gcc_bimc_sysnoc_axi_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  /* (K)gcc_bimc_clk */
  Clock_ToggleClock(HWIO_GCC_BIMC_CBCR_ADDR, CLK_TOGGLE_ENABLE); 

  /* Perform a BIMC clock switch to 400 MHz (GPLL0 / 2) with the 
   * FSM disabled, because DDR is not ready for switching yet */
  	if( ! Clock_EnableSource( clkCfg.eSource )) return FALSE;
  
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 1);
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_DDR_LEGACY_2X_MODE_EN, 1);

  	if( ! Clock_ConfigMux(&clkCfg)) return FALSE;


  if( ! Clock_EnableSource( bimcgpuclkCfg.eSource )) return FALSE;
  if( ! Clock_ConfigMux(&bimcgpuclkCfg)) return FALSE;
  

  
  /*
   * Trigger the FSM update manually and wait for the frequency to switch.
   */
  if(! Clock_TriggerUpdate(HWIO_ADDR(GCC_BIMC_MISC),
                           HWIO_FMSK(GCC_BIMC_MISC, BIMC_FSM_DIS_DDR_UPDATE))) return FALSE;

  /* gcc_ocmem_sys_noc_axi_clk : voted on previously */
  /* gcc_ocmem_noc_cfg_ahb_clk */
  // 8x26 removed HWIO_OUTF(GCC_OCMEM_NOC_CFG_AHB_CBCR, CLK_ENABLE, 1);

  /*Configuring apss_tcu_async_clk to run at same voltage level as BIMC*/
  if( ! Clock_EnableSource( apsstcuclkCfg.eSource )) return FALSE;
  if( ! Clock_ConfigMux(&apsstcuclkCfg)) return FALSE;
  
  Clock_I2CInit();

  return TRUE;
}

/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Clock_PreDDRInitEx() was added in 8974 to provide a separate API
    for emergency download (EDL) without including Mxdroop work around. Adding wrapper 
    function to avoid compilation erros in 8x26 and 8x10.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_PreDDRInit( uint32 ddr_type )
{
  return Clock_PreDDRInitEx( ddr_type );
}

/* ========================================================================
**  Function : Clock_BIMCConfigFSM
** ======================================================================*/
/*
    Description: Configure BIMC to enable the DDR FSM.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_BIMCConfigFSM( void )
{
  /* Enable the FSM */
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 0);

  /* Enable DEHR, which is a data mover style device that saves and 
   * restores BIMC state. */
   Clock_ToggleClock(HWIO_GCC_DEHR_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
}



/* ========================================================================
**  Function : Clock_ReplaceUSBBootClockEnable
** ======================================================================*/
/*
    Description: The register GCC_USB_BOOT_CLOCK_CTL turns on all the clocks
    necessary to access USB from the APPS.  This function must be ported
    on every new MSM.  It replaces that single register with all the clock
    enables that it replaced.  This allows for fine grained control of these
    clocks on an individual basis.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
void Clock_ReplaceUSBBootClockEnable()
{
  /* Set the CLK_ENABLE for each clock that is enabled by GCC_USB_BOOT_CLOCK_CTL */
  Clock_ToggleClock(HWIO_GCC_SNOC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_PCNOC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_SYS_NOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
  /* Now disable the master boot control, allowing each of those above to replace */
  HWIO_OUTF(GCC_USB_BOOT_CLOCK_CTL, CLK_ENABLE, 0);
}

/* ========================================================================
**  Function : Clock_QPICConfig
** ======================================================================*/
/*
    Description: Configure QPIC(NAND) clocks at 100MHz on GPLL0.

    @param None
    @return None

    @dependencies
    None.

    @sa None
*/
boolean Clock_QPICConfig( void )
{
  /* A mux config for QPIC clock to 100MHz*/
  const ClockConfigMuxType QPICCfg =
  {
    .nCMDCGRAddr = HWIO_ADDR(GCC_QPIC_CMD_RCGR),
    .eMux = MUX_GCC,
    .eSource = SRC_GPLL0,
    .nDiv2x = 16,  /* div8*/
    .nM = 0,
    .nN = 1,
    .n2D = 0
  };

  /* Configure QPIC clock to 100MHz */
  return Clock_ConfigMux( &QPICCfg ) ;

}

ClockPlatformInfoType Clock_platf_info;


/* ============================================================================
**  Function : Clock_Init
** ============================================================================
*/
/*!

    This function turns on the required clocks and configures
    Fabric and Scorpion speeds depending on the System Fabric and
    CPU boot performance level.

    @param eSysPerfLevel   -  [in] Fabric and DDR performance level to initialize.
    @param eCPUPerfLevel   -  [in] Scropion CPU performance level to initialize.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/

boolean Clock_Init
(
  ClockBootPerfLevelType eSysPerfLevel,
  ClockBootPerfLevelType eCPUPerfLevel
)
{
  Clock_SBLConfigType *cfg = Clock_SBLConfig();
  ClockConfigMuxType *RPMCfg;
  RPMCfg  =  &cfg->RPM_Cfg;
 
  if( ! Clock_SetCPUPerfLevel(eCPUPerfLevel)) return FALSE;
  //msm8916 has L2 sync to CPU, so this does not apply
  //if( ! Clock_SetL2PerfLevel(eCPUPerfLevel)) return FALSE;
  if( ! Clock_SetSysPerfLevel(eSysPerfLevel)) return FALSE;
  if( ! Clock_InitCrypto()) return FALSE;

  /* RPM requires that I always configure GPLL0 ,GPLL2(for QDSS used in RPM), BIMC_PLL. */
  /* Configuring A53PLL and enabling early output but not using it in SBL as APPS gets configured to 800MHZ from GPLL0*/
  (void)Clock_ConfigureSource(SRC_GPLL0);
  (void)Clock_ConfigureSource(SRC_GPLL1);
  (void)Clock_ConfigureSource(SRC_GPLL2);

  Clock_platf_info.chip_id = Clock_GetChipId();

  if(Clock_platf_info.chip_id != DALCHIPINFO_ID_MSM8208)
  {
    (void)Clock_ConfigureSource(SRC_BIMCPLL);
  }
  (void)Clock_ConfigureSource(SRC_A7PLL);

  Clock_ReplaceUSBBootClockEnable();
  Clock_ToggleClock(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
  /*
   * Configure RPM to 100MHz in case its source is 
   * configured to XO
   */
  if(HWIO_INF(GCC_RPM_CFG_RCGR, SRC_SEL) == 0x0)
  {
    if (!Clock_EnableSource(RPMCfg->eSource)) return FALSE;
    if (!Clock_ConfigMux(RPMCfg))  return FALSE;
  }

  if( !Clock_QPICConfig()) return FALSE;
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetSysPerfLevel
** ============================================================================
*/
/**
  Configure System and Application Fabric to a perf level.

  @param eSysPerfLevel [in]  -  System NOC and DDR performance level to configure.

  @return
  TRUE -- Fabric was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetSysPerfLevel
(
  ClockBootPerfLevelType eSysPerfLevel
)
{
  /* Configure SysNOC, PCNOC */
  /* Configure BIMC FSM. BIMC already configured in Clock_PreDDRinit*/
  const ClockConfigMuxType *clkCfg;
  Clock_SBLConfigType *cfg = Clock_SBLConfig();

  if(eSysPerfLevel >= CLOCK_BOOT_PERF_NUM) return FALSE;

  /* Configure SNOC */
  clkCfg = &cfg->SNOC_Cfg[eSysPerfLevel];
  if( ! Clock_EnableSource( clkCfg->eSource )) return FALSE;
  if( ! Clock_ConfigMux(clkCfg)) return FALSE;

  /* Configure BIMC FSM */
  Clock_BIMCConfigFSM();

  /* Configure PCNOC */
  clkCfg = &cfg->PCNOC_Cfg[eSysPerfLevel];
  if( ! Clock_EnableSource( clkCfg->eSource )) return FALSE;
  if( ! Clock_ConfigMux(clkCfg)) return FALSE;

  return TRUE;
}

/* ============================================================================
**  Function : Clock_InitCrypto
** ============================================================================
*/
/*!
    Configure Crypto clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_InitCrypto(void)
{
  const ClockConfigMuxType *CECfg;
  Clock_SBLConfigType *cfg = Clock_SBLConfig();
  CECfg = &cfg->CE_Cfg;

  if( ! Clock_EnableSource( CECfg->eSource )) return FALSE;
  if( ! Clock_ConfigMux(CECfg)) return FALSE;

  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_CLK_ENA, 1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AHB_CLK_ENA, 1);

  return TRUE;

} /* END Clock_InitCrypto */

/* ============================================================================
**  Function : Clock_InitForDownloadMode
** ============================================================================
*/
/*!
    Configure clocks for download.  Enable every RAM we want to dump.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
void Clock_InitForDownloadMode(void)
{
  uint32 mask;

  /* Enable the necessary voteable KPSS and BIMC related clocks */
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, SYS_NOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BOOT_ROM_AHB_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, SPMI_PCNOC_AHB_CLK_ENA);
    

  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, mask);

   /* Enable Imem and Msg RAM clock */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);

  // Pronto TCM
  HWIO_OUTF(WCSS_A_PMU_COMMON_GDSCR, SW_COLLAPSE, 0);
  Clock_ToggleClock(HWIO_WCSS_A_PMU_COMMON_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  // LPASS LPM
  /* No separate LPASS power domain and No separate LPASS subsystem on 8916.*/
  //HWIO_OUTF(LPASS_AUDIO_CORE_GDSCR, SW_COLLAPSE, 0);
  Clock_ToggleClock(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  Clock_ToggleClock(HWIO_GCC_ULTAUDIO_AHBFABRIC_IXFABRIC_LPM_CBCR_ADDR, CLK_TOGGLE_ENABLE);

}

/* ============================================================================
**  Function : Clock_ExitBoot
** ============================================================================
*/
/*!

    This function turns off clocks that were used during boot, but are not
    needed after boot.  This should be called at the end of boot.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/
boolean Clock_ExitBoot(void)
{
  /* Calling it here because if done in Clock_Init then the configurations get overridden 
   * with sleep configs during DalTlmm_PostInit() in gpio_init().
   */ 
  if (!Clock_ExtBuckGPIOMisc()) return FALSE;
  /* Disable UART related clocks */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA, 0);

  /* Disable I2C related clocks */
  // 8x26 removed BLSP2
  //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_AHB_CLK_ENA, 0);
  //HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BLSP2_SLEEP_CLK_ENA, 0);
  //HWIO_OUTF(GCC_BLSP2_QUP4_I2C_APPS_CBCR, CLK_ENABLE, 0);

  /* Disable CE1 */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_CLK_ENA, 0);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA, 0);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, CRYPTO_AHB_CLK_ENA, 0);

  /* Disable PRNG */
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 0);

  return TRUE;
}


/* ============================================================================
**  Function : Clock_BIMCQuery
** ============================================================================
*/
/*!
 
  This function lets the client query the BIMC frequency plan and the number 
  of supported frequencies.
  @return
  NONE
  @dependencies
  None.

*/
void Clock_BIMCQuery(ClockQueryType nQuery,void* pResource)
{
  return ;
 
}

/**
  This function is used for switching all the buses to different 
  voltage level for the ddr driver (LOW and HIGH) for DDR SITE
  training. 

  @param 
    eVoltageLevel - Clock Voltage Level (LOW and HIGH)

  @return
    None

  @dependencies
    None

  @sa
    None
*/
void Clock_SwitchBusVoltage( ClockVRegLevelType eVoltageLevel )
{
  return;
}

boolean Clock_SetBIMCSpeed(uint32 nFreqKHz )
{
  return TRUE;
}


extern uint32 clock_apps_speed_khz ;


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/*!
     To find the clock frequency (int KHZ) of a given clock

    @param eClock - clock type for which the speed to be determined
    @param *pnFrequencyHz - This will contain the frequency (in KHZ) value after function returns
    @return
    TRUE -- pnFrequencyHz points to valid frequency
    FALSE -- pnFrequencyHz is invalid.

    @dependencies
    In case of apps clock,  if this function is called before the Clock_Init, this will return false. As
    it requires some initialization to take place.

    @sa None
*/

boolean Clock_GetClockFrequency(ClockType  eClock,  uint32 * pnFrequencyHz)
{
  
  Clock_SBLConfigType *cfg = Clock_SBLConfig();
  boolean status = FALSE;
  
  switch(eClock)
  {
    case CLK_APPS:
    {
      if(clock_apps_speed_khz != 0)
      {
        *pnFrequencyHz = clock_apps_speed_khz;
         status = TRUE;
      }
      break;
    }
    case CLK_DDR:
    {
      *pnFrequencyHz = ddr_speed_khz;
      status = TRUE;
      break;
    }
    default:
    {
      break;
    }
  }
  
  return status;
}



