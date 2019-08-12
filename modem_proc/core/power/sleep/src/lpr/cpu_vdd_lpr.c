/*==============================================================================
  FILE:         cpu_vdd_lpr.c

  OVERVIEW:     This file provides the sleep LPR definition for the
                QDSP6V4 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2011-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/lpr/cpu_vdd_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "CoreTime.h"
#include "CoreVerify.h"
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
#include "HALmssMSA_hwio.h"
#include "kvp.h"
#include "rpmclient.h"
#include "DDIChipInfo.h"

/*===========================================================================
 *                       INTERNAL VARIABLES AND MACROS
 *==========================================================================*/

/* ----------------------- Debug code start ---------------- */
/* Flag to indicate that sleep is running in STM or not */
 uint32 __sleep_in_stm = FALSE;
/* ----------------------- Debug code end ------------------ */

 static DalChipInfoFamilyType chipFamilyInfo;
 
/*
 * target_cfg stores target specific config
 */
extern sleep_target_config target_cfg;

/*
 * Temporary variables for barrier workaround
 */
volatile uint32 temp_barrier  = 0;
volatile uint32 temp_rmb      = 0;

/*
 * @brief Used to enumerate processor lprm configuration
 */
typedef enum {
  CORE_PC,      /* Config processor core for Power Collapse   */
  CORE_BHS,     /* Config processor core for BHS based PC     */
  CORE_VDDMIN,  /* Config processor core for VDD Minimization */
  CORE_CLK_GATE /* Config processor core for clock gating */
} core_lprm;

/**
 * @brief Used to enumerate lprm configuration for q6_spmctl_cfg_l2
 */
typedef enum{
  MEM_RET,       /* Config processor SS & MEM for ret           */
  MEM_NORET,     /* Config processor SS & MEM for non-ret       */
} mem_state;

/*============================================================================
 *                      FUNCTIONS FOR CPU VDD LPR'S MODES
 *============================================================================*/

 /*==============================================================================
                      INTERNAL FUNCTIONS FOR CPU VDD LPR'S MODES
 =============================================================================*/
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
                                      boolean pmiIrq )
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

  switch( coreMode )
  {
    case SPM_MODE_PWRC:
    {
      wakeup_in_en    = 0x0;
      clk_gating_mode = 0x0;
      wake_irq        = 0x0;
      save_restore    = 0x0;
      en_ares         = 0x1FF;
      spm_config_lpm  = 0x1; /* Configure SPM low-power mode */
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
      break;
    }
    default:
    {
      CORE_VERIFY(0);
      break;
    }
  }
  
  HWIO_OUT( QDSP6SS_SLPC_CFG, 
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PU_PERI_STAGGER, 0x1           ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PD_PERI_STAGGER, 0x0           ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PU_ARRY_STAGGER, 0x1           ) |
            HWIO_FVAL( QDSP6SS_SLPC_CFG, MEM_PD_ARRY_STAGGER, 0x0           ) |
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

  spm_enable( 0, spm_config_lpm );

  /* Configure SPM if required by lprm */
  if ( spm_config_lpm )
  {
    spm_set_low_power_mode( 0, coreMode );
  }
}

 /**
 * @brief Q6 specific function to isolate processor
 *        core from L2 interrupt controller.
 *
 */                  
static inline void q6_isolate_core(void)
{
  int slpc_timeout = 200;

  /* Transfer interrupt control to SLPC */
  HWIO_OUT( QDSP6SS_PD_HS_START, 1 );

  while ( !( HWIO_IN( QDSP6SS_PD_HS_DONE ) &
             HWIO_FMSK( QDSP6SS_PD_HS_DONE, DONE ) ) )
  {
    CORE_VERIFY( --slpc_timeout );
  }
}

/**
 * @brief configure Q6 specific SPMCTL registers
 *        for PC / CPU VDD_DIG minimization
 *
 * @param core_mode  : @see core_lprm definition
 */                  
static inline void q6_spmctl_cfg_core(core_lprm core_mode)
{
  /* SPMCTL_EN_CLK is 0x3F for both CORE_PC and CORE_VDDMIN */
  HWIO_OUT( QDSP6SS_SPMCTL_EN_CLK,   0x3F );

  if ( core_mode == CORE_PC || core_mode == CORE_BHS )
  {
    /* Set the SPMCTL overrides for PC */
    HWIO_OUT( QDSP6SS_SPMCTL_EN_ARES,  0x3F );
    HWIO_OUT( QDSP6SS_SPMCTL_EN_CLAMP, 0x1  );
    
    /* Set MEM_PERIPH to 1 for PC */
    HWIO_OUTF( QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH,  0x1 );
  }
  else
  {
    /* Set the SPMCTL overrides for VDD_MIN */
    HWIO_OUT( QDSP6SS_SPMCTL_EN_ARES,  0x0  );
    HWIO_OUT( QDSP6SS_SPMCTL_EN_CLAMP, 0x0  );

    /* Set MEM_PERIPH to 0 for VDD_MIN */
    HWIO_OUTF( QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH,  0x0 );
  }
}

/**
 * @brief parameteried call to transition Q6 to PC modes
 * 
 * @param wakeup_tick: Absolute count of wakeup @19.2MHz
 * @param core_mode  : @see core_lprm definition
 * @param l2_mode    : @see l2_lprm definition
 */
static inline void sleep_q6_cfg_lpm( uint64 wakeup_tick, 
                                     core_lprm core_mode, l2_lprm l2_mode )
{
  sleepStats_wakeup_reason  reason;
  uint64                    tmp_time;
  boolean                   is_rpm_assisted;
  spm_low_power_mode_type   spm_lpm;
  int                       qurt_shutodwn_type  = 0;

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint64 time_dt = CoreTimetick_Get64();
#endif

  /* Core VDD_MIN is not supported yet */
  CORE_VERIFY( core_mode == CORE_PC || core_mode == CORE_BHS );

  is_rpm_assisted = sleep_target_get_rpm_assisted_mode();

  //spm_enable(0, TRUE);

  //q6_isolate_core();

  //q6_spmctl_cfg_core(core_mode);

  switch( core_mode )
  {
    case CORE_PC:
    {
      spm_lpm = SPM_MODE_PWRC;
      break;
    }
  
    case CORE_BHS:
    {
      spm_lpm = SPM_MODE_PWRC_BHS;
      break;
    }

    case CORE_VDDMIN:
    {
      spm_lpm = SPM_MODE_VDD_MIN;
      break;
    }

    case CORE_CLK_GATE:
    {
      spm_lpm = SPM_MODE_CLK_GATE;
      break;
    }
    default:
    {
      CORE_VERIFY(NULL);
    }
  }
  //spm_set_low_power_mode( 0, spm_lpm );

  cpuVddLPR_Q6LPMConfigure( spm_lpm, TRUE );

  switch (l2_mode)
  {
    case L2_TCM_RET:
    {
      qurt_shutodwn_type = QURT_POWER_SHUTDOWN_TYPE_L2RET;
      break;
    }

    case L2_NORET:
    {
      qurt_shutodwn_type = QURT_POWER_SHUTDOWN_TYPE_L2NORET;
      break;
    }

    case L2_TCM_NORET:
    {
      qurt_shutodwn_type = QURT_POWER_SHUTDOWN_TYPE_SAVETCM;
      break;
    }

    default:
    {
      CORE_VERIFY(NULL);
    }
  }

  /* Program the wakeup timer only if not in a RPM assisted mode.
   * Value could be modified as part of call, so set to value returned */
  if (FALSE == is_rpm_assisted)
  {
    wakeup_tick = sleep_target_enable_sleep_timer(wakeup_tick);
  }

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  /* Save power collapse type*/
  sleepStats_putMiscData(l2_mode, SLEEP_STATS_MISC_PC_L2_MODE_TYPE);
    
  tmp_time = CoreTimetick_Get64(); 
  sleepStats_putLprTimeData(tmp_time, SLEEP_STATS_TIME_MSTR_SHUTDOWN_TYPE);

  /* Calculate local enter latency time */
  time_dt =  tmp_time - time_dt;
  sleepStats_putLprLatency((uint32)time_dt, SLEEP_STATS_ENTER_LATENCY_TYPE);
#endif

  /* Added dummy reads to complete any transactions on the apps_bus to 
   * avoid possible bus hang when returning from power collapse.
   */
  temp_barrier  = HWIO_IN(MSS_MSA);
  temp_rmb      = HWIO_IN(MSS_RELAY_MSG_BUFFER_08);

  /* ----------------------- Debug code start ---------------- */
  __sleep_in_stm = TRUE;
  /* ----------------------- Debug code end ------------------ */

  /**** Do actual power collapse ****/
  qurt_power_shutdown_enter(qurt_shutodwn_type);

  /* Record wakeup time as early as possible in master code */
  tmp_time = CoreTimetick_Get64();

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  sleepStats_putLprTimeData(tmp_time, SLEEP_STATS_TIME_MSTR_RETURN_TYPE);
#endif

  if (FALSE == is_rpm_assisted)
  {
    /* Set standalone wakeup reason based on current time and requested scheduled 
     * wakeup time */
    if (tmp_time > wakeup_tick)
    {
      reason    = SLEEP_STATS_WAKEUP_SA_SCHEDULED;
      tmp_time  = wakeup_tick;  /* use set qtmr value for real wakeup time */
    }
    else
    {
      reason = SLEEP_STATS_WAKEUP_SA_UNSCHEDULED;
    }

    /* Set stand alone PC wakeup reason */
    sleepStats_putMiscData(reason, SLEEP_STATS_MISC_WAKEUP_REASON_TYPE);
  
    /* Set wakeup time as either the qtmr value, or the earliest possible point
     * in return to master code 
     * (RPM assisted will overwrite value in RPM assisted cases)*/
    sleepStats_putLprTimeData(tmp_time, SLEEP_STATS_TIME_WAKEUP_TYPE);
  }

  return;
}

/* CPU VDD LPR ****************************************************************/

/**
 * @brief cpu_vdd_lpr_init
 * Initialize spm
 */
void cpu_vdd_lpr_init( void )
{
  chipFamilyInfo = DalChipInfo_ChipFamily ();
 /* Configure Q6SS registers that are invariant across all 
     cpu_vdd low-power modes */

  HWIO_OUT( QDSP6SS_SPMCTL_EN_CLAMP, 0x01 );
  HWIO_OUT( QDSP6SS_SPMCTL_EN_CLK,   0x7F );
  HWIO_OUT( QDSP6SS_SPMCTL_EN_BHS,   0x01 ); 
  /*HWIO_OUT( QDSP6SS_SPMCTL_EN_LDO,   
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, VREF_SEL,    0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEOFF,    0x1 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEON,     0x0 ) |
            HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, BYPASS_OPEN, 0x1 ) );*/

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
   
   /* Set to 0 to wait for external PLL lock in SPM powerup sequence */
   HWIO_OUTM( QDSP6SS_SPM_EVENT_CTL,
              HWIO_FMSK(QDSP6SS_SPM_EVENT_CTL, EXT_PLL_OVRRD),   
	      HWIO_FVAL(QDSP6SS_SPM_EVENT_CTL, EXT_PLL_OVRRD, 0x0));
			
  //spm_initialize();
  
  /* SPM is always enabled for all cpu_vdd lprms
     See @function cpuVDDLP_Q6LPMConfigure implementation */
  /* spm_enable( 0, 1 ); */

  /* Set up Clock Gate as default Mode */
  cpuVddLPR_Q6LPMConfigure( SPM_MODE_CLK_GATE, FALSE );  
}

/**
 * @brief cpu_vdd_LPRM Enter function for PC + L2_NORET + TCM_RET
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_vdd_pc_l2_noret_enter( uint64_t wakeup_tick )
{
  sleep_q6_cfg_lpm(wakeup_tick, CORE_PC, L2_NORET);
}

/**
 * @brief cpu_vdd_LPRM Enter function for PC + L2_RET + TCM_NORET
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_vdd_pc_l2_tcm_noret_enter( uint64 wakeup_tick )
{
  sleep_q6_cfg_lpm(wakeup_tick, CORE_PC, L2_TCM_NORET);
}

/**
 * @brief cpu_vdd_LPRM Enter function for PC + L2_RET + TCM_RET
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_vdd_pc_l2_tcm_ret_enter( uint64 wakeup_tick )
{
  sleep_q6_cfg_lpm(wakeup_tick, CORE_PC, L2_TCM_RET);
}

/**
 * @brief cpu_vdd_LPRM Enter function for BHS + L2_RET + TCM_RET
 * @param wakeup_tick: Absolute wakeup time in 64-bit 19.2MHz ticks
 */
void cpu_vdd_bhs_l2_tcm_ret_enter( uint64 wakeup_tick )
{
  sleep_q6_cfg_lpm(wakeup_tick, CORE_BHS, L2_TCM_RET);
}

/**
 * @brief cpu_vdd_pc_exit
 *
 * Will be called when exiting from power collapse after sleep
 */
void cpu_vdd_pc_exit( void )
{
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint64 start_time = CoreTimetick_Get64();
  uint32 lprm_time;
#endif

  /* Configure the Q6 for Clock Gating when exiting any LPRM */
  cpuVddLPR_Q6LPMConfigure( SPM_MODE_CLK_GATE, FALSE );

  sleep_target_disable_sleep_timer();

  spm_enable(0, FALSE);

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  lprm_time = (uint32)(CoreTimetick_Get64() - start_time);
  sleepStats_putLprLatency(lprm_time, SLEEP_STATS_BACKOFF_LATENCY_TYPE);
#endif

  return;
}

/*============================================================================
 *                      FUNCTIONS FOR L2 LPR'S MODES
 *============================================================================*/

/* SPMCTL_EN_MEM BIT configuration for PC and VDD_MIN             */
/* QDSP6SS_SPMCTL_EN_MEM, L2DATA_STBY     BRING_UP = 0x0, TODO    */
/* QDSP6SS_SPMCTL_EN_MEM, ETB,            RET = 0x0, NO_RET = 0x1 */
/* QDSP6SS_SPMCTL_EN_MEM, L2TAG,          RET = 0x0, NO_RET = 0x1 */
/* QDSP6SS_SPMCTL_EN_MEM, L2DATA,         RET = 0x0, NO_RET = 0x7 */

/* MPSS QDSP6v5 L2DATA = 512KB L2$ + 256KB L2TCM                  */
/* L2_TCM_RET   L2DATA = 0x0 0b000                                */
/* L2_TCM_NORET L2DATA = 0x7 0b111                                */
/* L2_NORET     L2DATA = 0x6 0b110                                */

/**
 * @brief Perform TCM specific configuration for Power Collapse
 *
 * @param tcm_state: enum that specifies whether memory is retained or not
 */                  
static inline void q6_spmctl_cfg_tcm(mem_state tcm_state)
{
  uint32 l2data = HWIO_INF( QDSP6SS_SPMCTL_EN_MEM, L2DATA );

  /* TCM is controlled by target specific config */
  if(tcm_state == MEM_RET)
  {
    /* Clear bit */
    l2data &= ~target_cfg.tcm_mask;
  }
  else
  {
    /* Set bit */
    l2data |= target_cfg.tcm_mask;
  }

  HWIO_OUTF( QDSP6SS_SPMCTL_EN_MEM, L2DATA, l2data );
}

/**
 * @brief Perform L2 specific configuration for Power Collapse
 *
 * @param tcm_state: enum that specifies whether memory is retained or not
 */                  
static inline void q6_spmctl_cfg_l2(mem_state l2_state)
{
  uint32 l2tag;
  uint32 l2data = HWIO_INF( QDSP6SS_SPMCTL_EN_MEM, L2DATA );
  
  /* L2 is controlled by target specific config and L2TAG */
  if(l2_state == MEM_RET)
  {
    l2data &= ~target_cfg.l2_mask;
    l2tag = 0x0;
  }
  else
  {
    l2data |= target_cfg.l2_mask;
    l2tag = 0x1;
  }

  HWIO_OUTM( QDSP6SS_SPMCTL_EN_MEM,
             ((HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, CLAMP_QMC_MEM)) |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, CLAMP_WL))      |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH))    |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, L2DATA_STBY))   |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, ETB))           |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, L2TAG))          |
             (HWIO_FMSK(QDSP6SS_SPMCTL_EN_MEM, L2DATA)) 
             ),
             ((HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, CLAMP_QMC_MEM,  0x01)) |
             (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, CLAMP_WL,       0x01))  |
             (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, MEM_PERIPH,     0x01))  |
             (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, L2DATA_STBY,    0x0))   | 
	     (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, ETB,            0x0))   |
             (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, L2TAG,          l2tag)) |
             (HWIO_FVAL(QDSP6SS_SPMCTL_EN_MEM, L2DATA,         l2data)) 
             )
           );
}

/* L2 LPR & TCM LPR ***********************************************************/

/**
 * @brief Configure L2 for Retention
 */
void l2_ret_enter( uint64_t wakeup_tick )
{
  q6_spmctl_cfg_l2(MEM_RET);
}

/**
 * @brief Configure L2 for Non-Retention
 */
void l2_noret_enter( uint64_t wakeup_tick )
{
  q6_spmctl_cfg_l2(MEM_NORET);
}

/**
 * @brief Configure TCM for Retention
 */
void tcm_ret_enter( uint64_t wakeup_tick )
{
  q6_spmctl_cfg_tcm(MEM_RET);
}

/**
 * @brief Configure TCM for Non-Retention
 */
void tcm_noret_enter( uint64_t wakeup_tick )
{
  q6_spmctl_cfg_tcm(MEM_NORET);
}

void l2_exit( void )
{
  return;
}
