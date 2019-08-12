/*==============================================================================
  FILE:         cpu_vdd_lpr.c

  OVERVIEW:     This file provides the sleep LPR definition for the
                QDSP6V4 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2011-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/lpr/cpu_vdd_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $ 
==============================================================================*/
#include "CoreTime.h"
#include "CoreVerify.h"
#include "DDIClock.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "sleep.h"
#include "sleep_lpr.h"
#include "sleepi.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "sleep_statsi.h"
#include "sleep_log.h"
#include "qurt.h"
#include "HALhwio.h"
#include "spm.h"
#include "HALsleep_hwio.h"
#include "kvp.h"
#include "rpmclient.h"

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/**
 * @brief Time at which Sleep timer was programmed to wake up the master 
 *        during standalone low power modes.
 */
static uint64 g_expectedWakeupTick = 0;
static uint32 s_SPMCTL_EN_MEM_ApcrConfig;
static uint32 s_SPMCTL_EN_MEM_PcConfig;
/*==============================================================================
                      INTERNAL FUNCTIONS FOR CPU VDD LPR'S MODES
 =============================================================================*/
/* SPMCTL_EN_MEM BIT configuration for retention and non-retention  */
/* QDSP6SS_SPMCTL_EN_MEM, CLAMP_QMC_MEM   RET = 0x1, NO_RET = 0x1   */
/* QDSP6SS_SPMCTL_EN_MEM, CLAMP_WL        RET = 0x1, NO_RET = 0x1   */
/* QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH      RET = 0x1, NO_RET = 0x1   */
/* QDSP6SS_SPMCTL_EN_MEM, ARR_STBY        RET = 0x0, NO_RET = 0x0   */
/* QDSP6SS_SPMCTL_EN_MEM, JU,             RET = 0x0, NO_RET = 0x3   */
/* QDSP6SS_SPMCTL_EN_MEM, ETB,            RET = 0x0, NO_RET = 0x0   */
/* QDSP6SS_SPMCTL_EN_MEM, L1IU,           RET = 0x0, NO_RET = 0x3   */
/* QDSP6SS_SPMCTL_EN_MEM, L1DU,           RET = 0x0, NO_RET = 0x3   */
/* QDSP6SS_SPMCTL_EN_MEM, L2PLRU,         RET = 0x0, NO_RET = 0x1   */

/** 
 * cpuVddLPR_Q6LPMConfigure
 *  
 * @brief Performs all the hardware related configuration 
 *        that is required to setup the QDSP6 SS for the 
 *        low-power modes supported by this function
 * 
 * @param coreMode:Specifies the low-power mode supported by the 
 *                 SPM driver. 
 *
 * @param pmi_irq :Boolean representing whether PMI IRQ is required
 *                 to be triggered on exit from low-power mode.
 *                 Currently pmi_irq is only required to be triggered
 *                 for APCR based low-power mode that are triggered
 *                 from a single threaded context.
 */

static void cpuVddLPR_Q6LPMConfigure( spm_low_power_mode_type coreMode,
                                      boolean                 pmiIrq)
{
  /* Based on QDSP66v56 HPG              */
  /* SLPC_CFG[MEM_PU_PERI_STAGGER]       */
  /*   1- PC or APCR                     */
  /* SLPC_CFG[MEM_PD_PERI_STAGGER]       */
  /*   0- PC or APCR                     */
  /* SLPC_CFG[MEM_PU_ARRY_STAGGER]       */
  /*   1- PC or APCR                     */
  /* SLPC_CFG[MEM_PD_ARRY_STAGGER]       */
  /*   0- PC or APCR                     */
  /* SLPC_CFG[WAKEUP_IN_EN]              */
  /*   0 - PC                            */
  /*   1 - APCR                          */
  /* Enable QDSP6_wakeup_in handshaking  */
  /* signal                              */

  /* SLPC_CFG[CLK_GATING_MODE]           */
  /*   0 - PC,APCR                       */
  /*   1 - Clock Gating Mode             */
  /* Must be set for non-SPM based Clock */
  /* Gating mode. Cleared for SPM modes  */
 
  /* STATERET[WAKE_IRQ]                  */
  /*   0 - mask pmi                      */
  /*   1 - trigger pmi                   */
  /* Controls whether PMI Interrupt is   */
  /* triggered as part of recovery       */

  /* STATERET[SAVE/RESTORE]              */
  /*   0 - don't save/restore            */
  /*   1 - save/restore                  */
  /* Controls whether CPU state is saved */
  /* and restored around APCR.           */
  
  /* SPMCTL_EN_ARES                      */
  /* 0xXX  - NA for clk gating           */
  /* 0x7F  - APCR                        */
  /* 0x1FF - PC                          */

  uint32 wakeup_in_en;
  uint32 clk_gating_mode; 
  uint32 wake_irq;
  uint32 save_restore;
  uint32 en_ares;
  uint32 spm_config_lpm;
  uint32 spm_ctl_en_mem;
  uint32 l2_cache = 0;

  switch( coreMode )
  {
    case SPM_MODE_APCR_PLL_LPM:
    case SPM_MODE_APCR_PLL_ON:
    case SPM_MODE_APCR_RAIL_LPM:
    {
      wakeup_in_en    = 0x1;
      clk_gating_mode = 0x0;
      wake_irq        = ( pmiIrq == TRUE )? 0x1 : 0x0;
      save_restore    = 0x1;
      en_ares         = 0x7F;
      spm_config_lpm  = 0x1; /* Configure SPM low-power mode */
      spm_ctl_en_mem         = s_SPMCTL_EN_MEM_ApcrConfig;
      break;
    }
    case SPM_MODE_PWRC:
    {
      wakeup_in_en    = 0x0;
      clk_gating_mode = 0x0;
      wake_irq        = 0x0;
      save_restore    = 0x0;
      en_ares         = 0x1FF;
      spm_config_lpm  = 0x1; /* Configure SPM low-power mode */
      l2_cache               = HWIO_INM( QDSP6SS_SPMCTL_EN_MEM, 
                                         ((HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, L2TAG)) |
                                          (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, L2DATA))));
                                     
      spm_ctl_en_mem          = s_SPMCTL_EN_MEM_PcConfig | l2_cache;
      break;
    }
    case SPM_MODE_PWRC_L2RET:
    {
      wakeup_in_en           = 0x0;
      clk_gating_mode        = 0x0;
      wake_irq               = 0x0;
      save_restore           = 0x0;
      en_ares                = 0x1FF;
      spm_config_lpm         = 0x1; /* Configure SPM low-power mode */
      spm_ctl_en_mem         = s_SPMCTL_EN_MEM_ApcrConfig;
      break;
    }	
    case SPM_MODE_CLK_GATE:
    {
      wakeup_in_en    = 0x0; 
      clk_gating_mode = 0x1;
      wake_irq        = 0x0; 
      save_restore    = 0x0; 
      en_ares         = 0x0; 
      spm_config_lpm  = 0x0; /* SPM configuration not required */
      spm_ctl_en_mem        = s_SPMCTL_EN_MEM_ApcrConfig;
      break;
    }
    default:
    {
      CORE_VERIFY(0);
      break;
    }
  }
  
  HWIO_OUT( QDSP6SS_SLPC_CFG, 
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PU_PERI_STAGGER, 0x1         ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PD_PERI_STAGGER, 0x0         ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PU_ARRY_STAGGER, 0x1         ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PD_ARRY_STAGGER, 0x0         ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, WAKEUP_IN_EN,    wakeup_in_en    ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, CLK_GATING_MODE, clk_gating_mode ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, PD_HS_MODE,      0x1             ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, SAW_DBG,         0x0             ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, PD_SRC_SEL,      0x0             ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, EXT_WAKEUP_ENA,  0x0             ) );

  HWIO_OUT( QDSP6SS_SPMCTL_EN_STATERET,
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_STATERET, WAKE_IRQ, wake_irq     ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_STATERET, RESTORE,  save_restore ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_STATERET, SAVE,     save_restore ) );

  HWIO_OUT( QDSP6SS_SPMCTL_EN_ARES, en_ares );

  /** Explicitly configure QDSP6SS_SPMCTL_MEM register for full PC or
   *  APCR */
  HWIO_OUT( QDSP6SS_SPMCTL_EN_MEM, spm_ctl_en_mem );

  /* Configure SPM if required by lprm */
  if ( spm_config_lpm )
  {
    /* SPM Driver does not support efficient storage of sequences when
       multiple low-power mode employ the same spm sequence commands.
       SPM Long sequence supports both PC and APCR_RAIL_LPM modes. 
       Configure SPM to use PWRC when APCR_RAIL_LPM is required */
    if ( coreMode == SPM_MODE_APCR_RAIL_LPM || coreMode == SPM_MODE_PWRC_L2RET)
    {
      coreMode = SPM_MODE_PWRC;
    }

    spm_set_low_power_mode( 0, coreMode );
  }
}

/** 
 * cpuVddLPR_Q6LPMEnter 
 *  
 * @brief Parameterized call to prepare core for different low power modes.
 * 
 * @param coreMode: Mode to which SPM HW will be programmed to 
 *                  execute appropriate command sequence.
 * @param L2Mode: Mode in which core's L2 cache will be kept.
 * @param wakeupTick: Absolute count of wakeup @19.2MHz
 */
static void cpuVddLPR_Q6LPMEnter( spm_low_power_mode_type coreMode,
                                  l2_lprm L2Mode, uint64 wakeupTick )
{
  boolean isRPMAssistedMode;
  uint64  tempTimeTick;
  int32   qurt_shutdown_type = 0;
  uint64 currTime = 0;

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint64 enterLatency = CoreTimetick_Get64();
#endif

  /* Entering a Q6 LPM consists of two parts: */
  /*  i) Configure Registers for LPM          */
  /* ii) Execute Software to trigger LPM      */
  
  /* If Q6 LPM is being Entered as part of 
   * Cached Synth Mode then only i) is required.
   * If Q6 LPM is being Entered as part of 
   * Uncached Synth Mode then both i) and ii) 
   * are required.*/

  if(sleepOS_isProcessorInSTM() == FALSE)
  {
    cpuVddLPR_Q6LPMConfigure(coreMode, FALSE);
    return;
  }
  else
  {
    cpuVddLPR_Q6LPMConfigure(coreMode, TRUE);
  }

  if(SPM_MODE_PWRC == coreMode || SPM_MODE_PWRC_L2RET == coreMode)
  {
    switch (L2Mode )
    {
      case L2_TCM_RET:
      {
        qurt_shutdown_type = QURT_POWER_SHUTDOWN_TYPE_L2RET;
        break;
      }
      case L2_NORET:
      {
        qurt_shutdown_type = QURT_POWER_SHUTDOWN_TYPE_L2NORET;
        break;
      }
      case L2_TCM_NORET:
      {
        qurt_shutdown_type = QURT_POWER_SHUTDOWN_TYPE_SAVETCM;
        break;
      }

      default:
      {
        CORE_VERIFY(0);
      }
    }
  }

  currTime = CoreTimetick_Get64();
  /* Ensure that wakeup_tick is in the future */
  if (currTime > wakeupTick)
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_WARNING, (1*2),
                      "Error fatal: wakeup time in past, curr time: 0x%llx",
                      ULOG64_DATA(currTime) );
    CORE_VERIFY(0);
  }
  
  isRPMAssistedMode = sleepTarget_getRPMAssistedMode();

  /* Program the wakeup timer only if not in a RPM assisted mode.
   * Value could be modified as part of call, so set to value returned */
  if (FALSE == isRPMAssistedMode)
  {
    g_expectedWakeupTick = sleepTarget_enableSleepTimer(wakeupTick);
  }

  tempTimeTick = CoreTimetick_Get64();
  sleepStats_putLprTimeData(tempTimeTick, 
                            SLEEP_STATS_TIME_MSTR_SHUTDOWN_TYPE);

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  /* Save power collapse type*/
  sleepStats_putMiscData(L2Mode , SLEEP_STATS_MISC_PC_L2_MODE_TYPE);

  /* Calculate local enter latency time */
  enterLatency = tempTimeTick - enterLatency;
  sleepStats_putLprLatency((uint32)enterLatency, 
                           SLEEP_STATS_ENTER_LATENCY_TYPE);
#endif

  /* QURT functions perform all wait */ 
  if(SPM_MODE_PWRC == coreMode || SPM_MODE_PWRC_L2RET == coreMode)
  {
    CORE_VERIFY(QURT_EOK == qurt_power_shutdown_enter(qurt_shutdown_type));
  }
  else if ( ( coreMode == SPM_MODE_APCR_PLL_LPM ) || 
            ( coreMode == SPM_MODE_APCR_PLL_ON )  ||
            ( coreMode == SPM_MODE_APCR_RAIL_LPM ) )
  {
    CORE_VERIFY(QURT_EOK == qurt_power_apcr_enter());
  }
  else
  {
    CORE_LOG_VERIFY(0, sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 1,
                                       "Invalid coreMode: 0x%x",
                                       coreMode));
  }

  return;
}

/** 
 * cpuVddLPR_Q6LPMExit 
 *  
 * @brief Will be called when exiting from power collapse after sleep
 *
 * @param coreMode: Mode to which SPM HW was programed to 
 *                  execute command sequence while entering low
 *                  power mode.
 */
static void cpuVddLPR_Q6LPMExit ( spm_low_power_mode_type coreMode )
{
  boolean           isRPMAssistedMode;
  uint64            start_time;
  uint32            lprm_time;
  static uint64     lastBringupAck  = 0;

  /* Configure the Q6 for Clock Gating when exiting any LPRM */
  cpuVddLPR_Q6LPMConfigure(SPM_MODE_CLK_GATE, FALSE);

  /* If LPR Exit is called as part of Cached Synth mode then 
     no additional stats related profiling is required */
  if ( sleepOS_isProcessorInSTM() == FALSE )
  {
    return;
  }

  /* Check if we are in an RPM assisted mode */
  isRPMAssistedMode = sleepTarget_getRPMAssistedMode();

  if(FALSE == isRPMAssistedMode)
  {
    sleepStats_wakeup_reason  reason;
    uint64                    master_wakeup_tick;

    /* Disable Sleep QTimer Frame */
    sleepTarget_disableSleepTimer();

    /* Obtain time when control return to Sleep exit path */
    master_wakeup_tick = 
      sleepStats_getLprTimeData(SLEEP_STATS_TIME_MSTR_RETURN_TYPE);

    /* For standalone PC, wakeup time is scheduled QTimer value or earliest
     * possible point of return to Sleep module */
    if( master_wakeup_tick > g_expectedWakeupTick )
    {
      reason = SLEEP_STATS_WAKEUP_SA_SCHEDULED;

      /* use set Qtimer value as wakeup time */
      master_wakeup_tick = g_expectedWakeupTick;
    }
    else
    {
      reason = SLEEP_STATS_WAKEUP_SA_UNSCHEDULED;
    }

    /* Set stand alone PC wakeup reason and wakeup time in Stats */
    sleepStats_putMiscData(reason, SLEEP_STATS_MISC_WAKEUP_REASON_TYPE);

    start_time = master_wakeup_tick;
  }
  else
  {
    RPM_master_stats  *rpmStats = sleepStats_getRpmDataPtr();
    CORE_VERIFY_PTR(rpmStats);

    /* Catch errors where RPM didn't update its statistics */
    CORE_VERIFY(rpmStats->bringup_ack > lastBringupAck);
    lastBringupAck = rpmStats->bringup_ack;

    /* In RPM assisted modes, the wakeup time will come from RPM. Get that here 
     * so it can be used to calculate proper cpu_vdd backoff latency. */
    start_time = inp64(&rpmStats->bringup_ack);

    /* Set wakeup reason as given from RPM */
    sleepStats_putMiscData(inpdw(&rpmStats->wakeup_reason), 
                           SLEEP_STATS_MISC_WAKEUP_REASON_TYPE);
  }

  /* Set master wakeup time */
  sleepStats_putLprTimeData(start_time, SLEEP_STATS_TIME_WAKEUP_TYPE);

  lprm_time = (uint32)(CoreTimetick_Get64() - start_time);
  sleepStats_putLprLatency(lprm_time, SLEEP_STATS_BACKOFF_LATENCY_TYPE);

  return;
}

/*==============================================================================
                              GLOBAL FUNCTIONS
 =============================================================================*/
/** 
 * cpuVddLPR_initialize
 *  
 * @brief Initialize cpu vdd LPR
 */
void cpuVddLPR_initialize( void )
{
  /* Configure Q6SS registers that are invariant across all 
     cpu_vdd low-power modes */

  HWIO_OUT( QDSP6SS_SPMCTL_EN_CLAMP, 0x01 );
  HWIO_OUT( QDSP6SS_SPMCTL_EN_CLK,   0x7F );
  HWIO_OUT( QDSP6SS_SPMCTL_EN_BHS,   0x01 ); 
  HWIO_OUT( QDSP6SS_SPMCTL_EN_LDO,   
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, VREF_SEL,    0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEOFF,    0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEON,     0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, BYPASS_OPEN, 0x1 ) );

  /* Configure MSS Q6 PLL for FREEZE MODE.
     cpu_vdd low-power mode come in two flavors:
     i) Sequences that don't contain pll low-power commands: "clk_gate" and "apcr"
     ii) Sequence that contain pll low-ower commands: "apcr_pll_lpm", "apcr_rail_off"
         "pc_l2_noret" and "pc_l2_tcm_noret" 

     By configuring PLL related registers for FREEZE, we eliminate the need to execute 
     PLL configuration as part of any cpu_vdd lprms */

  HWIO_OUT( QDSP6SS_SPMCTL_EN_PLL, 
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_PLL, PLL_FREEZE, 0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_PLL, PLL_VOTE,   0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_PLL, PLL_OUTDIS, 0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_PLL, PLL_BYPASS, 0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_PLL, PLL_RESET,  0x0 ) );

  /* Configure MSS Q6 SPMCTL_EN_MEM register and save values for
     2 low power mode configurations during sleep:
     i)  APCR
     ii) Full PC */
  HWIO_OUT( QDSP6SS_SPMCTL_EN_MEM,
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, CLAMP_QMC_MEM, 0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, CLAMP_WL,      0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH,    0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, ARR_STBY,      0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, JU,            0x3 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, ETB,           0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L1IU,          0x3 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L1DU,          0x3 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L2PLRU,        0x1 ) );
			
  /* Save the SPMCTL_EN_MEM configuration for Full PC low power mode */
  s_SPMCTL_EN_MEM_PcConfig = HWIO_IN(QDSP6SS_SPMCTL_EN_MEM);

  HWIO_OUT( QDSP6SS_SPMCTL_EN_MEM,
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, CLAMP_QMC_MEM, 0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, CLAMP_WL,      0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH,    0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, ARR_STBY,      0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, JU,            0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, ETB,           0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L1IU,          0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L1DU,          0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_MEM, L2PLRU,        0x0 ) );

  /* Save the SPMCTL_EN_MEM configuration for APCR low power mode */
  s_SPMCTL_EN_MEM_ApcrConfig = HWIO_IN(QDSP6SS_SPMCTL_EN_MEM);

   HWIO_OUTM( QDSP6SS_SPM_EVENT_CTL,
              HWIO_FMSK(QDSP6SS_SPM_EVENT_CTL, EXT_PLL_OVRRD),   
              HWIO_FVAL(QDSP6SS_SPM_EVENT_CTL, EXT_PLL_OVRRD, 0x0));

  //spm_initialize();			
  
  /* SPM is always enabled for all cpu_vdd lprms
     See @function cpuVDDLP_Q6LPMConfigure implementation */
  spm_enable( 0, 1 ); 

  /* Set up Clock Gate as default Mode */
  cpuVddLPR_Q6LPMConfigure( SPM_MODE_CLK_GATE, FALSE );

  return;
}

/** 
 * cpuVddLPR_l2NonRetentionEnter 
 *  
 * @brief cpu_vdd_LPRM Enter function for PC + L2_NORET + TCM_RET 
 *  
 * @param wakeupTick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpuVddLPR_l2NonRetentionEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter(SPM_MODE_PWRC, L2_NORET, wakeupTick);
  return;
}

/** 
 * cpuVddLPR_l2NonRetentionExit
 *  
 * @brief Exit function for PC + L2_NORET + TCM_RET
 */
void cpuVddLPR_l2NonRetentionExit( void )
{
  cpuVddLPR_Q6LPMExit(SPM_MODE_PWRC);
  return;
}

/** 
 * cpuVddLPR_fullNonRetentionEnter 
 *  
 * @brief cpu_vdd_LPRM Enter function for PC + L2_RET + TCM_NORET 
 *  
 * @param wakeupTick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpuVddLPR_fullNonRetentionEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter(SPM_MODE_PWRC, L2_TCM_NORET, wakeupTick);
  return;
}

/** 
 * cpuVddLPR_fullNonRetentionExit
 *  
 * @brief Exit function for PC + L2_RET + TCM_NORET
 */
void cpuVddLPR_fullNonRetentionExit( void )
{
  cpuVddLPR_Q6LPMExit(SPM_MODE_PWRC);
  return;
}

/** 
 * cpuVddLPR_l2tcmRetentionEnter 
 *  
 * @brief cpu_vdd_LPRM Enter function for PC + L2_RET + TCM_RET 
 *  
 * @param wakeupTick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */

void cpuVddLPR_l2tcmRetentionEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter(SPM_MODE_PWRC_L2RET, L2_TCM_RET, wakeupTick);
  return;
}


/** 
 * cpuVddLPR_l2tcmRetentionExit
 *  
 * @brief Exit function for PC + L2_RET + TCM_RET
 */
void cpuVddLPR_l2tcmRetentionExit( void )
{
  cpuVddLPR_Q6LPMExit(SPM_MODE_PWRC_L2RET);
  return;
}

/** 
 * cpuVddLPR_apcrRailOffEnter 
 *  
 * @brief Enter function for APCR with rail in low power mode.
 */
void cpuVddLPR_apcrRailOffEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter(SPM_MODE_APCR_RAIL_LPM, L2_TCM_RET, wakeupTick);
  return;
}

/** 
 * cpuVddLPR_apcrRailOffExit 
 *  
 * @brief Exit function for APCR with rail in low power mode
 */
void cpuVddLPR_apcrRailOffExit( void )
{
  cpuVddLPR_Q6LPMExit( SPM_MODE_APCR_RAIL_LPM );
  return;
}

/** 
 * cpuVddLPR_apcrPllLowPowerEnter 
 *  
 * @brief Enter function for APCR PLL Low-Power mode 
 *        (Hybrid Sequence) = APCR + PLL Configured for low-power
 */
void cpuVddLPR_apcrPllLowPowerEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter( SPM_MODE_APCR_PLL_LPM, L2_TCM_RET, wakeupTick );
  return;
}

/** 
 * cpuVddLPR_apcrPllLowPowerExit 
 *  
 * @brief Exit function for APCR PLL_LP mode
 */
void cpuVddLPR_apcrPllLowPowerExit( void )
{
  cpuVddLPR_Q6LPMExit( SPM_MODE_APCR_PLL_LPM );
  return;
}

/** 
 * cpuVddLPR_apcrEnter 
 *  
 * @brief Enter function for APCR mode (Short Sequence in Q6HPG)
 */
void cpuVddLPR_apcrEnter( uint64 wakeupTick )
{
  cpuVddLPR_Q6LPMEnter(SPM_MODE_APCR_PLL_ON, L2_TCM_RET, wakeupTick);
  return;
}

/** 
 * cpuVddLPR_apcrExit 
 *  
 * @brief Exit function for APCR mode (Short Sequence in Q6HPG)
 */
void cpuVddLPR_apcrExit(void)
{
  cpuVddLPR_Q6LPMExit( SPM_MODE_APCR_PLL_ON );
  return;
}

/** 
 * cpuVddLPR_clkGatingEnter 
 *  
 * @brief Enter function for Clock Gating low power mode.
 */
void cpuVddLPR_clkGatingEnter( uint64 wakeupTick )
{
  /* Configure for Clock Gate */
  cpuVddLPR_Q6LPMConfigure(SPM_MODE_CLK_GATE, FALSE);
  return;
}

/** 
 * cpuVddLPR_clkGatingExit 
 *  
 * @brief Exit function for Clock Gating low power mode.
 */
void cpuVddLPR_clkGatingExit(void)
{
  return;
}

