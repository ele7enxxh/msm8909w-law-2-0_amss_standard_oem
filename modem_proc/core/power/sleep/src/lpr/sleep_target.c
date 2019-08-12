/*==============================================================================
  FILE:         sleep_target.c

  OVERVIEW:     This file provides target-specific functionality for the 8960
                Modem Subsystem.

  DEPENDENCIES: None

                Copyright (c) 2011-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/lpr/sleep_target.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdint.h>
#include "dog.h"
#include "sleep_target.h"
#include "timer.h"
#include "qurt.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "com_dtypes.h"
#include "npa_resource.h"
#include "sleep_lpri.h"
#include "atomic_ops.h"
#include "DDIInterruptController.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "sleepi.h"
#include "HALhwio.h"
#include "HALsleep_hwio.h"
#include "msmhwiobase.h"
#include "vmpm.h"

/* ==================================================================
                     GLOBAL DATA DECLARATIONS
   ================================================================== */
/**
 * @brief g_mode_is_rpm_assisted
 *
 * Variable for indication of entry to RPM assisted modes
 */
static boolean g_mode_is_rpm_assisted = FALSE;

/**
 * @brief SLEEP_ENTRY_TICKS
 *
 * Time required from ramping up the frequency to just before entering
 * low power modes. This will change based on current frequncy.
 * Measured: CoreAU34-576Mhz-L2WB-L2NORET
 * Measured: "Sleep entry" <-> "Entering modes"
 */
#define SLEEP_ENTRY_TICKS 2200

/**
 * @brief SLEEP_EXIT_TIME
 *
 * Time required from removing backoff time to logging wakeup event while
 * exiting from sleep. Like entry time, this will change based on frequency
 * before entering sleep.
 * Measured: CoreAU34-576Mhz-L2WB-L2NORET
 * Measured: "Exiting modes" <-> "Modes exiting(lpr:"cpu_vdd")" +
 * Measured: "Wakeup (reason:)" <-> "qurt_power_shutdown_exit()"
 */
#define SLEEP_EXIT_TICKS (120 + 260)

/**
 * Timer used to holdoff sleep for a certain amount of time at initial
 * boot-up.
 */
#define SLEEP_INIT_DELAY_MS 10000

/*
 * Sleep Target configuration parameters
 *
 */
sleep_target_config target_cfg = {
  .entry_ticks    = SLEEP_ENTRY_TICKS,
  .exit_ticks     = SLEEP_EXIT_TICKS,
  .init_delay_ms  = SLEEP_INIT_DELAY_MS,
  .chip_family    = DALCHIPINFO_FAMILY_UNKNOWN,
  .chip_ver       = 0,
  .l2_mask        = 0x6,
  .tcm_mask       = 0x1,
};

uint64 (*sleep_timer_enable_fp)( uint64 wakeup_tick );
void (*sleep_timer_disable_fp)( void );

/*
 * Variable to control default enablement of l2 LPRMS
 */
volatile unsigned int sleep_target_l2_lprm = SLEEP_ALL_LPRMS;

/* ==================================================================
                        External References
   ================================================================== */

/* deferrable timer groups */
extern timer_group_type rex_timer_group;
extern timer_group_type timer_null_group;

/* target-specific nodes that are implemented & initialized in this module */
extern sleep_lpr_synth SleepLPR_synth;

extern npa_node_definition sleep_cpu_vdd_node;
extern npa_node_definition sleep_rpm_sync_node;
extern npa_node_definition sleep_l2cache_node;
extern npa_node_definition sleep_tcm_node;
/* ==================================================================
                       FORWARD DECLARATIONS
   ================================================================== */

static void sleep_target_wakeup_handler(uint32 param);
static uint64 sleep_target_enable_QTMR( uint64 wakeup_tick );
static uint64 sleep_target_enable_IDLT( uint64 wakeup_tick );
static void sleep_target_disable_IDLT( void );
static void sleep_target_disable_QTMR( void );

/* ==================================================================
                   STATIC FUNCTION DECLARATIONS
   ================================================================== */

/**
 * @brief sleep_target_register_lprs
 *
 * Register the LPRs & nodes that are owned by sleep.
 *
*/
static void sleep_target_register_lprs( void         *context,
                                        unsigned int  event_type,
                                        void         *data,
                                        unsigned int  data_size )
{
  npa_resource_state initial_state = 0;
  npa_resource_state intial_state_set = 1;

  /* Register the NPA nodes that this module currently owns. */
  npa_define_node( &sleep_cpu_vdd_node, &initial_state, NULL );
  npa_define_node( &sleep_rpm_sync_node, &intial_state_set, NULL );
  npa_define_node( &sleep_l2cache_node, &intial_state_set, NULL );
  npa_define_node( &sleep_tcm_node, &intial_state_set, NULL );
}

/* ==================================================================
                     GLOBAL FUNCTION DECLARATIONS
   ================================================================== */

/*
 * Dependencies to wait on before registering any LPRs.
 */
static const char *lpr_dependencies[] =
{
  "/sleep/lpr"
};

/*
 * Dal data struct to hold DAL Interrupt Controller Handle
 */
static DalDeviceHandle *hDALIntCtl;

/**
 * Handle to the DAL "WakeupTimer" timetick driver, for calculating the
 * wakeup tick during sleep. This timer is used by the DAL and timer services
 * to provide "sleep time".
 */
static DalDeviceHandle *hDALWakeTmr;

/**
 * @brief null handler for the registered idle timer interrupts
 *
 */
static void sleep_target_wakeup_handler(uint32 param)
{
  if ( hDALWakeTmr ){
    /* Disable qtimer interrupt */
    CORE_VERIFY( DalTimetick_Enable(hDALWakeTmr, 0)
                 == DAL_SUCCESS );
  }
  return;
}

/*
 * sleep_target_init
 *
*/
sleep_target_config * sleep_target_init(void)
{
  DalChipInfoFamilyType  chip_family;
  DalChipInfoVersionType chip_version;
  uint32                 timer_int_n;
  uint32                 timer_int_trig;

  /* Register a callback for when the necessary dependencies are available,
   * in order to register the nodes and LPRs that sleep owns. */
  npa_resources_available_cb( NPA_ARRAY( lpr_dependencies ),
                              sleep_target_register_lprs, NULL );

  /* set below groups as deferrable */
  timer_group_set_deferrable( &timer_null_group, TRUE );
  timer_group_set_deferrable( &rex_timer_group,  TRUE );

  sleep_target_get_CPU_arch(&chip_family, &chip_version);

  if( chip_family == DALCHIPINFO_FAMILY_MSM8974 &&
      chip_version < DALCHIPINFO_VERSION(2,0) )
  {
    /* use slow clock based idle timer */
    sleep_timer_enable_fp = sleep_target_enable_IDLT;
    sleep_timer_disable_fp = sleep_target_disable_IDLT;
    timer_int_n = 30;
    timer_int_trig = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
  }
  else
  {
    /* use qtimer frame that is available on all other targets */
    DalTimetick_Attach( "WakeUpTimer", &hDALWakeTmr );
    CORE_VERIFY_PTR( hDALWakeTmr );

    sleep_timer_enable_fp = sleep_target_enable_QTMR;
    sleep_timer_disable_fp = sleep_target_disable_QTMR;
    DalTimetick_GetInterruptVectorNum(hDALWakeTmr, &timer_int_n);
    timer_int_trig = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
    
    /* MSS v5.1.0 needs LDO BHS setting */
    HWIO_OUTF( QDSP6SS_SPMCTL_EN_BHS, BHS, 0x1 );

    HWIO_OUT( QDSP6SS_SPMCTL_EN_LDO,
              HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, VREF_SEL,    0x0 )|
              HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEOFF,    0x7 )|
              HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, FORCEON,     0x0 )| 
              HWIO_FVAL( QDSP6SS_SPMCTL_EN_LDO, BYPASS_OPEN, 0x3 ) );
    
  }

  /* Attaching to interrupt controller and registering interrupt handlers */
  CORE_VERIFY( DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                                 &hDALIntCtl ) == DAL_SUCCESS );

  CORE_VERIFY( DalInterruptController_RegisterISR( hDALIntCtl,
                                                   timer_int_n,
                                                   (DALIRQ)sleep_target_wakeup_handler,
                                                   (const DALIRQCtx) 0,
                                                   timer_int_trig )
               == DAL_SUCCESS );

  return &target_cfg;
}

/*
 * sleep_target_get_CPU_arch
 *
*/
void sleep_target_get_CPU_arch(DalChipInfoFamilyType *chipFamily,
                               DalChipInfoVersionType *chipVer)
{
  static uint32 init = 0;

  if(init == 0)
  {
    DalDeviceHandle *hChipInfo;

    /* Get global chip architecture  */
    if( DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo) == DAL_SUCCESS )
    {
      DalDevice_Open(hChipInfo, DAL_OPEN_SHARED);
      DalChipInfo_GetChipFamily(hChipInfo, &target_cfg.chip_family);
      DalChipInfo_GetChipVersion(hChipInfo, &target_cfg.chip_ver);
      DalDevice_Close(hChipInfo);
      init = 1;
    }
  }

  if(chipFamily != NULL)
    *chipFamily = target_cfg.chip_family;

  if(chipVer != NULL)
    *chipVer    = target_cfg.chip_ver;

  return;
}

/*
 * sleep_target_lpr_init
 *
*/
void sleep_target_lpr_init(void)
{
  /* Register the pre-synthesized LPRMs here.  This must be done
   * before any of the component LPRMs are registered, so we do
   * it when the /sleep/lpr node is being initialized. */
  sleep_define_lpr_synth( &SleepLPR_synth );

  return;
}

/**
 *  @brief Sets wake-up duration for IDLT
 */
static uint64 sleep_target_enable_IDLT( uint64 wakeup_tick )
{
  uint64 idlt_sclk;
  uint64 idlt_tt;

  idlt_tt = wakeup_tick - CoreTimetick_Get64();
  idlt_sclk = TICKS_TO_SCLK64(idlt_tt);

  if (idlt_sclk > UINT32_MAX)
    idlt_sclk = UINT32_MAX;

  /* IDLT should not be programmed to < 3
   * due to AHB and 32Khz sync constraints
   */
  if( idlt_sclk < 3)
  {
    idlt_sclk = 3;
    sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 1,
                      " IDLT duration force set to %d sclks",
                      (uint32)idlt_sclk );
  }

  HWIO_OUT( QDSP6SS_IDLT_MATCH_VAL, (uint32)idlt_sclk );
  HWIO_OUT( QDSP6SS_IDLT_ENABLE, 0x1 );

  sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 1,
                    " Program IDLT (duration sclk: %d)",
                    (uint32)idlt_sclk );

  return wakeup_tick;
}

/**
 *  @brief Sets wake-up duration for IDLT
 */
static uint64 sleep_target_enable_QTMR( uint64 wakeup_tick )
{
  uint64 match_tt;
  uint64 now_tt = CoreTimetick_Get64();

  /* Setting up a new match value */
  DalTimetick_SetNextInterrupt64( hDALWakeTmr,
                                  wakeup_tick,
                                  now_tt, &match_tt );

  /* Re-enable qtimer interrupt */
  CORE_VERIFY( DalTimetick_Enable(hDALWakeTmr, 1) == DAL_SUCCESS );

  sleep_log_printf( SLEEP_LOG_LEVEL_INFO, (1*2),
                    " Program QTMR (match tick: 0x%llx)",
                    ULOG64_DATA(match_tt) );

  return match_tt;
}

/**
 * @brief sleep_target_enable_sleep_timer
 *
 * Programs the idle timer to expire at wakeup ticks.
 * It will start counting when the CPU is idle.
 */
uint64 sleep_target_enable_sleep_timer( uint64 wakeup_tick )
{
  /* Ensure that wakeup_tick is in the future */
  CORE_VERIFY( CoreTimetick_Get64() < wakeup_tick );
  return(sleep_timer_enable_fp(wakeup_tick));
}

/**
 * @breif Disables the IDLT
 */
static void sleep_target_disable_IDLT( void )
{
  HWIO_OUT( QDSP6SS_IDLT_ENABLE, 0x0 );
}

/**
 * @breif Disables the Sleep QTimer
 */
static void sleep_target_disable_QTMR( void )
{
}

/**
 * @brief sleep_target_disable_sleep_timer
 *
 * Disables any timer expirations
 */
void sleep_target_disable_sleep_timer( void )
{
  sleep_timer_disable_fp();
}

/**
 * @brief sleep function to disable deferrable timers
 *
 */
void sleep_target_defer_timers(void)
{
  timer_defer_match_interrupt_64();
  qurt_timer_group_disable( 0 );
}

/**
 * @brief sleep function to enable deferrable timers
 *
 */
void sleep_target_undefer_timers(void)
{
  qurt_timer_group_enable( 0 );
  timer_undefer_match_interrupt();
}

/**
 * @brief sleep function to enable DOG
 *
 */
void sleep_target_enable_dog(void)
{
  dog_autokick(FALSE);
}

/**
 * @brief sleep function to enable DOG
 *
 */
void sleep_target_disable_dog(void)
{
  dog_autokick(TRUE);
}

/*
 * setRpmAssistedMode
 */
void sleep_target_set_rpm_assisted_mode( boolean value )
{
  g_mode_is_rpm_assisted = value;
}

/*
 * getRpmAssistedMode
 */
boolean sleep_target_get_rpm_assisted_mode( void )
{
  return(g_mode_is_rpm_assisted);
}

/*
 * sleep_target_get_rpm_stat_ptr
 */
void* sleep_target_get_rpm_statistics_addr(void)
{
  uint32 rpm_stats_va = 0;

  /* RPM Stats Virtual Address is 128 bytes above
   * the mpm interrupt base virtual address
   */
  rpm_stats_va = (vmpm_getProcMpmBaseVA() - 128);
  return ((void*) rpm_stats_va);

}
