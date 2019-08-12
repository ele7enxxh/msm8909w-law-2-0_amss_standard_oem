/*==============================================================================
  FILE:         sleep_target.c

  OVERVIEW:     This file provides target-specific functionality for the
                modem Subsystem.

  DEPENDENCIES: None

                Copyright (c) 2011-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/lpr/sleep_target.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdint.h>
#include "dog.h"
#include "sleep_target.h"
#include "timer.h"
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
#include "vmpm.h"
#include "synthTypes.h"
#include "synthLPR.h"

/*==============================================================================
                             GLOBAL DATA DECLARATIONS
 =============================================================================*/
/**
 * @brief g_synthLPRMisRPMAssisted
 *
 * Variable for indication of entry to RPM assisted modes
 */
static boolean g_synthLPRMisRPMAssisted = FALSE;

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
sleep_target_config g_sleepTargetConfig = 
{
  .entry_ticks    = SLEEP_ENTRY_TICKS,
  .exit_ticks     = SLEEP_EXIT_TICKS,
  .init_delay_ms  = SLEEP_INIT_DELAY_MS,
  .chip_family    = DALCHIPINFO_FAMILY_UNKNOWN,
  .chip_ver       = 0,
  .l2_mask        = 0x30,
  .tcm_mask       = 0x0F,
};

/*
 * Dependencies to wait on before registering any LPRs.
 */
static const char *g_LPRDependencies[] =
{
  "/sleep/lpr"
};

/*
 * Dal data struct to hold DAL Interrupt Controller Handle
 */
static DalDeviceHandle *g_DALInterruptCtrlHndl;

/**
 * Handle to the DAL "WakeupTimer" timetick driver, for calculating the
 * wakeup tick during sleep. This timer is used by the DAL and timer services
 * to provide "sleep time".
 */
static DalDeviceHandle *g_DALWakeTimerHandle;

/*==============================================================================
                           EXTERNAL REFERENCES
 =============================================================================*/
/* deferrable timer groups */
extern timer_group_type rex_timer_group;
extern timer_group_type timer_null_group;

/* target-specific nodes that are implemented & initialized in this module */
extern sleep_synth_lpr SleepLPR_synth;

extern npa_node_definition sleep_cpu_vdd_node;
extern npa_node_definition sleep_rpm_sync_node;
extern npa_node_definition sleep_l2cache_node;
extern npa_node_definition sleep_tcm_node;

/*==============================================================================
                           FORWARD DECLARATIONS
 =============================================================================*/
static void   sleepTarget_wakeupHandler(uint32 param);
static uint64 sleepTarget_enableQTMR(uint64 wakeup_tick);
static void   sleepTarget_disableQTMR(void);

/*==============================================================================
                        STATIC FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * sleepTarget_registerLPRs 
 *  
 * @brief  Register the LPRs & nodes that are owned by sleep.
*/
static void sleepTarget_registerLPRs( void         *context,
                                      unsigned int  event_type,
                                      void         *data,
                                      unsigned int  data_size )
{
  npa_resource_state initial_state = 0;
  npa_resource_state initial_state_set = 1;

  /* Register the NPA nodes that this module currently owns. */
  npa_define_node(&sleep_cpu_vdd_node, &initial_state, NULL);
  npa_define_node(&sleep_rpm_sync_node, &initial_state_set, NULL);
  npa_define_node(&sleep_l2cache_node, &initial_state, NULL);
  npa_define_node(&sleep_tcm_node,     &initial_state, NULL);

  return;
}

/** 
 * sleepTarget_wakeupHandler
 * 
 * @brief null handler for the registered idle timer interrupts
 */
static void sleepTarget_wakeupHandler(uint32 param)
{
  return;
}

/**
 * sleepTarget_initialize
 */
void sleepTarget_initialize(void)
{
  uint32                 timer_int_n;
  uint32                 timer_int_trig;

  /* Register a callback for when the necessary dependencies are available,
   * in order to register the nodes and LPRs that sleep owns. */
  npa_resources_available_cb( NPA_ARRAY( g_LPRDependencies ),
                              sleepTarget_registerLPRs, NULL );

  /* set below groups as deferrable */
  timer_group_set_deferrable( &timer_null_group, TRUE );
  timer_group_set_deferrable( &rex_timer_group,  TRUE );

  /* use qtimer frame that is available on all other targets */
  DalTimetick_Attach( "WakeUpTimer", &g_DALWakeTimerHandle );
  CORE_VERIFY_PTR( g_DALWakeTimerHandle );

  DalTimetick_GetInterruptVectorNum(g_DALWakeTimerHandle, &timer_int_n);
  timer_int_trig = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
  
  /* Attaching to interrupt controller and registering interrupt handlers */
  CORE_VERIFY( DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                                 &g_DALInterruptCtrlHndl ) == DAL_SUCCESS );

  CORE_VERIFY( DalInterruptController_RegisterISR( g_DALInterruptCtrlHndl,
                                                   timer_int_n,
                                                   (DALIRQ)sleepTarget_wakeupHandler,
                                                   (const DALIRQCtx) 0,
                                                   timer_int_trig )
               == DAL_SUCCESS );

  return;
}

/**
 * sleepTarget_getCPUArchitecture
 */
void sleepTarget_getCPUArchitecture(DalChipInfoFamilyType   *chipFamily,
                                    DalChipInfoVersionType  *chipVer)
{
  static uint32 init = 0;

  if(init == 0)
  {
    DalDeviceHandle *hChipInfo;

    /* Get global chip architecture  */
    if( DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo) == DAL_SUCCESS )
    {
      DalDevice_Open(hChipInfo, DAL_OPEN_SHARED);
      DalChipInfo_GetChipFamily(hChipInfo, &g_sleepTargetConfig.chip_family);
      DalChipInfo_GetChipVersion(hChipInfo, &g_sleepTargetConfig.chip_ver);
      DalDevice_Close(hChipInfo);
      init = 1;
    }
  }

  if(chipFamily != NULL)
    *chipFamily = g_sleepTargetConfig.chip_family;

  if(chipVer != NULL)
    *chipVer    = g_sleepTargetConfig.chip_ver;

  return;
}

/*
 * sleepTarget_initializeLPR
 */
void sleepTarget_initializeLPR(void)
{
  /* Register the pre-synthesized LPRMs here.  This must be done
   * before any of the component LPRMs are registered, so we do
   * it when the /sleep/lpr node is being initialized. */
  synthLPR_define( &SleepLPR_synth );

  return;
}


/** 
 * sleepTarget_enableQTMR 
 *  
 * @brief Sets wake-up duration for local QTimer
 */
static uint64 sleepTarget_enableQTMR(uint64 wakeupTick)
{
  uint64 matchTick;
  uint64 now = CoreTimetick_Get64();

  /* Setting up a new match value */
  DalTimetick_SetNextInterrupt64(g_DALWakeTimerHandle,
                                 wakeupTick,
                                 now, &matchTick );

  /* Re-enable qtimer interrupt */
  CORE_VERIFY( DalTimetick_Enable(g_DALWakeTimerHandle, 1) == DAL_SUCCESS );

  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, (1*2),
                  " Program QTMR (match tick: 0x%llx)",
                  ULOG64_DATA(matchTick) );

  return matchTick;
}

/** 
 * sleepTarget_disableQTMR 
 *  
 * @breif Disables the Sleep QTimer
 */
static void sleepTarget_disableQTMR( void )
{
  if ( g_DALWakeTimerHandle )
  {
    /* Disable qtimer interrupt */
    CORE_VERIFY( DalTimetick_Enable(g_DALWakeTimerHandle, 0)
                 == DAL_SUCCESS );
  }
  return;
}

/*
 * sleepTarget_enableSleepTimer 
 */
uint64 sleepTarget_enableSleepTimer( uint64 wakeup_tick )
{
  /* Ensure that wakeup_tick is in the future */
  return(sleepTarget_enableQTMR(wakeup_tick));
}

/*
 * sleepTarget_disableSleepTimer
 */
void sleepTarget_disableSleepTimer( void )
{
  sleepTarget_disableQTMR();
  return;
}

/*
 * sleepTarget_deferTimers 
 */
void sleepTarget_deferTimers(void)
{
  timer_defer_match_interrupt_64();
  qurt_timer_group_disable( 0 );
  return;
}

/*
 * sleepTarget_undeferTimers  
 */
void sleepTarget_undeferTimers(void)
{
  qurt_timer_group_enable( 0 );
  timer_undefer_match_interrupt();
  return;
}

/*
 * sleepTarget_enableWatchdog 
 */
void sleepTarget_enableWatchdog(void)
{
  dog_autokick(FALSE);
  return;
}

/*
 * sleepTarget_disableWatchdog 
 */
void sleepTarget_disableWatchdog(void)
{
  dog_autokick(TRUE);
  return;
}

/*
 * sleepTarget_setRPMAssistedMode
 */
void sleepTarget_setRPMAssistedMode( boolean value )
{
  g_synthLPRMisRPMAssisted = value;
  return;
}

/*
 * sleepTarget_getRPMAssistedMode
 */
boolean sleepTarget_getRPMAssistedMode( void )
{
  return(g_synthLPRMisRPMAssisted);
}

/*
 * sleepTarget_getRPMStatisticsAddress
 */
void *sleepTarget_getRPMStatisticsAddress(void)
{
  uint32 rpm_stats_va = 0;

  /* RPM Stats Virtual Address is 128 bytes above
   * the mpm interrupt base virtual address
   */
  rpm_stats_va = (vmpm_getProcMpmBaseVA() - 128);
  return ((void*) rpm_stats_va);
}
