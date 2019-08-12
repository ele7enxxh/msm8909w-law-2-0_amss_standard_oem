/*===========================================================================
                              lpr_definition_uber.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2005,2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "mpm.h"
#include "vmpm.h"
#include "time_service.h"
#include "comdef.h"
#include "Clock.h"
#include "npa.h"
#include "npa_resource.h"
#include "ddr_drivers.h"
#include "rpm_hwio.h"
#include "rpm_definitions.h"
#include "cortex-m3.h"
#include "swevent.h"
#include "ClockDefs.h"
#include "system_db.h"
#include "system_db_rpm.h"
#include "dog.h"
#include "svs.h"
#include "temp_ddr_hwio.h"
#include "temp_bimc_m_hwio.h"
#include "rob.h"
#include "image_layout.h"
#include "railway.h"
#include "pmapp_npa.h"
#include "CoreVerify.h"
#include "gpio_debug.h"
#include "pm_clk_sleep.h"
#include "rpm_messageram.h"
#include "saw2_hwio.h"
#include "mpm2_hwio.h"

#include "pm_sleep.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define MAX_XO_CLIENTS  8
#define MAX_CX_MX_DELTA_UV 300000
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct sleep_stats_type
{
  uint32 stat_type;
  uint32 count;
  uint64 last_entered_at;
  uint64 last_exited_at;
  uint64 accumulated_duration;
  uint32 client_votes;
  uint32 reserved[3];
} sleep_stats_type;

typedef enum
{
  SLEEP_STATS_VDD_LOW = MPM_SLEEP_MODE_VDD_LOW,
  SLEEP_STATS_VDD_MIN = MPM_SLEEP_MODE_VDD_MIN,
  SLEEP_STATS_COUNT
} sleep_stats_modes;

typedef enum
{
  SLEEP_SUCCESS = 0,
  SLEEP_ERROR_NOT_ENOUGH_TIME,
  SLEEP_ERROR_PENDING_INTERRUPT,
  SLEEP_RESULT_COUNT,
} sleep_result_type;
//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
uint64 enter_deep_sleep_time;
//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static const unsigned QTIMER_IRQ = 50;

static uint32 deep_sleep_bail_count;
static sleep_result_type sleep_result;
static ClockPowerDomainIdType nPowerDomainId;
static uint8 BIMC_PC_CFG;
static bool bimc_pced = false;

static sleep_stats_type* sleep_stats;
uint64 mpm_enter_latency =0;
uint64 mpm_exit_latency =0;
#ifdef DDR_REG_TRACING
static volatile struct
{
  uint32 SCMO_RCH[1];
  uint32 SCMO_WCH[1];
  uint32 SCMO_CMD_BUF[1];
  uint32 DPE_DRAM[1];
  uint32 BIMC_M_APP[4];
  uint32 BIMC_M_SYS[9];
  uint32 BIMC_M_DSP[4];
} ddr_register_cache;
#endif //DDR_REG_TRACING

#ifdef APPS_SPM_TRACING
static volatile struct
{
  uint32 APCS_SAW2_SPM_STS[5];
  uint32 APCS_SAW2_SPM_CTL[6];
  uint32 APCS_SAW2_SPM_DLY[5];
} spm_register_cache;
#endif
static struct
{
  uint64 exit_latency;
  uint64 enter_latency;
  mpm_sleep_mode_type deep_sleep_mode;
} sleep_mode;

int *enabled_interrupts = NULL;
//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern uint64_t sched_get_next_start(void);
extern uint64_t sched_get_next_required_start(void);
extern void pm_npa_rpm_enter_sleep(void);
extern void pm_npa_rpm_exit_sleep(void);
extern void rbcpr_prepare_for_sleep(void);
extern void rbcpr_exit_sleep(void);
extern int mx_id;

#ifdef TSENSE_SAMPLING_1SEC
extern void HAL_deep_sleep_enter_tsense(void);
extern void HAL_deep_sleep_exit_tsense(void);
#endif

extern int rpm_npa_get_ee_client_id(const npa_client* client);
//===========================================================================
//                             Macro Definitions
//===========================================================================
#define BIMC_PC_ENABLE 2
#define BIMC_PC_DISABLE 1

//===========================================================================
//                           Function Definitions
//===========================================================================

bool debugger_connected(void);

static boolean prepare_interrupts(void)
{
  DALBOOL retval = TRUE;
  int num_masks = ((SCS.IntCtrlType & 0x1F) + 1);
  int     i;

  // Configure the MPM to honor the system's wakeup interrupts
  vmpm_prepare_for_sleep();

  // Check to see if an interrupt came in before we switched over to the MPM
  // If so, we need to abort the xo shutdown process
  if(is_any_interrupt_pending())
  {
    retval = FALSE;
  }

  if (!enabled_interrupts)
  {
    enabled_interrupts = (int*)malloc(num_masks * sizeof(int));
    if (NULL == enabled_interrupts)
    {
      ERR_FATAL("malloc failed to allocate memmory:", 0, 0, 0);
    }
  }

  // Save and disable all enabled interrupts
  for (i = 0; i < num_masks; i++)
  {
    enabled_interrupts[i] = SCS.NVIC.Enable[i];
    SCS.NVIC.Disable[i] = 0xFFFFFFFF;
  }

  // Enable the MPM interrupt so we can wake from sleep.
  interrupt_clear(MPM_IRQ);
  interrupt_enable(MPM_IRQ);

  return retval;
}

static void restore_interrupts(void)
{
  int num_masks = ((SCS.IntCtrlType & 0x1F) + 1);
  int i;

  CORE_VERIFY_PTR(enabled_interrupts);
  // Enable all interrupts previously stored
  for (i = 0; i < num_masks; i++)
  {
    SCS.NVIC.Enable[i] = enabled_interrupts[i];
  }

  // distribute any external wakeup interrupts to interested masters
  vmpm_propogate_interrupts();

  interrupt_disable(MPM_IRQ);
}

static uint64 calculate_halt_wakeup_time(void)
{
  return (sched_get_next_start() - sleep_mode.exit_latency);
}

static uint64 calculate_wakeup_time(void)
{
  uint64 task_start = sched_get_next_required_start();
  if(task_start <= sleep_mode.exit_latency)
  {
    return 0;
  }
  else
  {
    return (task_start - sleep_mode.exit_latency);
  }
}

static uint64 calculate_sleep_duration(void)
{
  uint64 wakeup_time; 
  uint64 sleep_start = time_service_now() + sleep_mode.enter_latency;

  //check if we will endup switching to FAST mode in vdd_min exit
  //and re-estimate task start time
  svs_speed_check_and_update();
  
  wakeup_time = calculate_wakeup_time();
   
   //adjust the delay to accommodate SVS switch 
  if(svs_get_svs_switch_to_fast())
    wakeup_time -= svs_get_switch_time();
  
 
  if(wakeup_time <= sleep_start)
    return 0;
  else
    return wakeup_time - sleep_start;
}

void vdd_min_latency()
{
  //Calculate latency for current clock speed.
  const svs_mode_id rpm_cpu_speed = svs_current_speed();

#if BIMC_PC_PARALLELIZATION_ENABLED
  bimc_pced = Clock_IsBIMCPowerCollapsed();
#endif

  sleep_mode.enter_latency =
    rob_get_duration (((bimc_pced) ? ROB_DURATION_VDD_MIN_BIMC_PCED_ENTER : ROB_DURATION_VDD_MIN_ENTER), rpm_cpu_speed);
  mpm_enter_latency =  mpm_deep_sleep_enter_latency();
  if (sleep_mode.enter_latency == 0)
  {
    sleep_mode.enter_latency = 0xC000 + mpm_enter_latency; //  2.56mS + mpm latency - conservative default value
  }
  else
  {
    sleep_mode.enter_latency += mpm_enter_latency;
  }

  //vdd_min exit latency = PXO warm up time
  //                     + time from ROB_EVENT_MPM_PRE_SW_DONE to ROB_EVENT_VDD_MIN_EXIT
  //                     + constant for other processing in getting out of vdd_min.

  // Current value based on pronto profiling
  // sleep_mode.exit_latency = 0x2A00;
  sleep_mode.exit_latency =
    rob_get_duration (((bimc_pced) ? ROB_DURATION_VDD_MIN_BIMC_PCED_EXIT : ROB_DURATION_VDD_MIN_EXIT), rpm_cpu_speed);
  mpm_exit_latency =  mpm_deep_sleep_exit_latency();

  if (sleep_mode.exit_latency == 0)
  {
    sleep_mode.exit_latency = 0xC000 + mpm_exit_latency; //  2.56mS + mpm latency - conservative default value
  }
  else
  {
    sleep_mode.exit_latency += mpm_exit_latency;
  }

}

//void rpm_halt_latency(sleep_coeffs *poly, uint64 x_value)
boolean rpm_halt_latency()
{
  unsigned switch_time = svs_get_switch_time();
  sleep_mode.enter_latency = switch_time + SLEEP_HALT_ENTER_LATENCY;
  sleep_mode.exit_latency = switch_time + SLEEP_HALT_EXIT_LATENCY;
  return ((time_service_now() + sleep_mode.enter_latency + sleep_mode.exit_latency) < sched_get_next_start());
}

void sleep_modes_init(void)
{
  const uint32 size_of_sleep_stats = sizeof(sleep_stats_type)*SLEEP_STATS_COUNT;

  sleep_mode.enter_latency = 0;
  sleep_mode.exit_latency = 0;
  sleep_mode.deep_sleep_mode = MPM_SLEEP_MODE_VDD_MIN;

  Clock_GetPowerDomainId("VDD_BIMC", &nPowerDomainId);

  deep_sleep_bail_count = 0;
#ifdef MSM_DISABLE_BIMC_PC
  BIMC_PC_CFG = BIMC_PC_DISABLE;
#else
  BIMC_PC_CFG = BIMC_PC_ENABLE;
#endif

  interrupt_configure(MPM_IRQ,    LEVEL_HIGH);
  interrupt_configure(QTIMER_IRQ, LEVEL_HIGH);

  sleep_stats = (sleep_stats_type*)rpm_image_section_array[RPM_SLEEP_STATS].section_base;
  CORE_VERIFY((char*)&sleep_stats[SLEEP_STATS_COUNT] <= rpm_image_section_array[RPM_SLEEP_STATS].section_end);

  memset((void*)sleep_stats, 0, size_of_sleep_stats);

  sleep_stats[0].stat_type = 0x776f6c76; // "vlow" in little endian
  sleep_stats[0].client_votes = 0;
  sleep_stats[1].stat_type = 0x6E696D76; // "vmin" in little endian
  sleep_stats[1].client_votes = 0;
}

__asm static void halt(void)
{
    ; Lock interrupts via primask instead of basepri during sleep, or else we never wake up.
    mrs r0, basepri
    cpsid i
    mov r1, #0
    msr basepri, r1

    ; Do the halt, using ISB to clear the M3 store buffer
    dsb
    isb
    wfi

    ; Revert interrupt lock to basepri-based.
    msr basepri, r0
    cpsie i
    bx r14
}

#ifdef DDR_REG_TRACING
static bool sleep_ddr_active()
{
  ddr_interface_state ddr_status = ddr_get_status();
    return ( (ddr_status.sdram0_cs0 == DDR_ACTIVE));
}

static void pull_ddr_regs(void)
{
  ddr_register_cache.SCMO_RCH[0] = HWIO_IN(BIMC_S_DDR0_SCMO_RCH_STATUS);

  ddr_register_cache.SCMO_WCH[0] = HWIO_IN(BIMC_S_DDR0_SCMO_WCH_STATUS);

  ddr_register_cache.SCMO_CMD_BUF[0] = HWIO_IN(BIMC_S_DDR0_SCMO_CMD_BUF_STATUS);

  ddr_register_cache.DPE_DRAM[0] = HWIO_IN(BIMC_S_DDR0_DPE_DRAM_STATUS_0);

  ddr_register_cache.BIMC_M_APP[0] = HWIO_IN(BIMC_M_APP_MPORT_STATUS_2A);
  ddr_register_cache.BIMC_M_APP[1] = HWIO_IN(BIMC_M_APP_MPORT_STATUS_2B);
  ddr_register_cache.BIMC_M_APP[2] = HWIO_IN(BIMC_M_APP_MPORT_STATUS_3A);
  ddr_register_cache.BIMC_M_APP[3] = HWIO_IN(BIMC_M_APP_MPORT_STATUS_3B);

  ddr_register_cache.BIMC_M_SYS[0] = HWIO_IN(BIMC_M_SYS0_MPORT_INTERRUPT_STATUS);
  ddr_register_cache.BIMC_M_SYS[1] = HWIO_IN(BIMC_M_SYS0_MPORT_STATUS_2A);
  ddr_register_cache.BIMC_M_SYS[2] = HWIO_IN(BIMC_M_SYS0_MPORT_STATUS_2B);
  ddr_register_cache.BIMC_M_SYS[3] = HWIO_IN(BIMC_M_SYS0_MPORT_STATUS_3A);
  ddr_register_cache.BIMC_M_SYS[4] = HWIO_IN(BIMC_M_SYS0_MPORT_STATUS_3B);
  ddr_register_cache.BIMC_M_SYS[5] = HWIO_IN(BIMC_M_SYS1_MPORT_STATUS_2A);
  ddr_register_cache.BIMC_M_SYS[6] = HWIO_IN(BIMC_M_SYS1_MPORT_STATUS_2B);
  ddr_register_cache.BIMC_M_SYS[7] = HWIO_IN(BIMC_M_SYS1_MPORT_STATUS_3A);
  ddr_register_cache.BIMC_M_SYS[8] = HWIO_IN(BIMC_M_SYS1_MPORT_STATUS_3B);

  ddr_register_cache.BIMC_M_DSP[0] = HWIO_IN(BIMC_M_DSP_MPORT_STATUS_2A);
  ddr_register_cache.BIMC_M_DSP[1] = HWIO_IN(BIMC_M_DSP_MPORT_STATUS_2B);
  ddr_register_cache.BIMC_M_DSP[2] = HWIO_IN(BIMC_M_DSP_MPORT_STATUS_3A);
  ddr_register_cache.BIMC_M_DSP[3] = HWIO_IN(BIMC_M_DSP_MPORT_STATUS_3B);
}
#endif  //DDR_REG_TRACING

#ifdef APPS_SPM_TRACING
static void pull_spm_regs(void)
{
  spm_register_cache.APCS_SAW2_SPM_STS[0] = HWIO_IN(APCS_L2_SAW2_SPM_STS);
  spm_register_cache.APCS_SAW2_SPM_STS[1] = HWIO_IN(APCS_ALIAS0_SAW2_SPM_STS);
  spm_register_cache.APCS_SAW2_SPM_STS[2] = HWIO_IN(APCS_ALIAS1_SAW2_SPM_STS);
  spm_register_cache.APCS_SAW2_SPM_STS[3] = HWIO_IN(APCS_ALIAS2_SAW2_SPM_STS);
  spm_register_cache.APCS_SAW2_SPM_STS[4] = HWIO_IN(APCS_ALIAS3_SAW2_SPM_STS);
  spm_register_cache.APCS_SAW2_SPM_CTL[0] = HWIO_IN(APCS_L2_SAW2_SPM_CTL);
  spm_register_cache.APCS_SAW2_SPM_CTL[1] = HWIO_IN(APCS_ALIAS0_SAW2_SPM_CTL);
  spm_register_cache.APCS_SAW2_SPM_CTL[2] = HWIO_IN(APCS_ALIAS1_SAW2_SPM_CTL);
  spm_register_cache.APCS_SAW2_SPM_CTL[3] = HWIO_IN(APCS_ALIAS2_SAW2_SPM_CTL);
  spm_register_cache.APCS_SAW2_SPM_CTL[4] = HWIO_IN(APCS_ALIAS3_SAW2_SPM_CTL);
  spm_register_cache.APCS_SAW2_SPM_DLY[0] = HWIO_IN(APCS_L2_SAW2_SPM_DLY);
  spm_register_cache.APCS_SAW2_SPM_DLY[1] = HWIO_IN(APCS_ALIAS0_SAW2_SPM_DLY);
  spm_register_cache.APCS_SAW2_SPM_DLY[2] = HWIO_IN(APCS_ALIAS1_SAW2_SPM_DLY);
  spm_register_cache.APCS_SAW2_SPM_DLY[3] = HWIO_IN(APCS_ALIAS2_SAW2_SPM_DLY);
  spm_register_cache.APCS_SAW2_SPM_DLY[4] = HWIO_IN(APCS_ALIAS3_SAW2_SPM_DLY);
}
#endif

void rpm_halt_enter(void)
{
  uint64 wakeup_time, wakeup_lo, wakeup_hi;

  sleep_result = SLEEP_ERROR_NOT_ENOUGH_TIME;
  if(rpm_halt_latency())
  {
    sleep_result = SLEEP_SUCCESS;
    rob_set_recording_speed(svs_current_speed());
    rob_mark_event (ROB_EVENT_RPM_HALT_ENTER);

#ifdef DDR_REG_TRACING
    if(sleep_ddr_active())
    {
      pull_ddr_regs();
    }
#endif

#ifdef APPS_SPM_TRACING
    pull_spm_regs();
#endif

    wakeup_time = calculate_halt_wakeup_time();


    // Limit halt to 15 seconds for PMIC wdog
    //wakeup_time = MIN(wakeup_time, (0x112A8800 + time_service_now()) );

    wakeup_lo   = wakeup_time & 0xFFFFFFFFU;
    wakeup_hi   = wakeup_time >> 32;

    SWEVENT(SLEEP_RPM_HALT_ENTER, wakeup_hi, wakeup_lo);
    // Set wakeup time.
    HWIO_OUT(RPM_F0_QTMR_V1_CNTP_CVAL_LO, wakeup_lo);
    HWIO_OUT(RPM_F0_QTMR_V1_CNTP_CVAL_HI, wakeup_hi);
    HWIO_OUT(RPM_F0_QTMR_V1_CNTP_CTL, 1);
    interrupt_enable(QTIMER_IRQ);

    svs_update_speed(1);

#ifndef MSM8952_STUBS
    svs_rpm_clock_xo();
#endif

    dog_enable(false);
    rob_mark_event (ROB_EVENT_RPM_HALT_PRE_CLKOFF);
    halt();
    rob_mark_event (ROB_EVENT_RPM_HALT_POST_CLKOFF);
    dog_enable(true);

#ifndef MSM8952_STUBS
    svs_rpm_clock_svs_slow();
#endif

    svs_update_speed(1);

    // Stop timer; it did its job of waking us up, but we don't need it anymore.
    interrupt_disable(QTIMER_IRQ);

    HWIO_OUT(RPM_F0_QTMR_V1_CNTP_CTL, 0);

    system_db_statehandler(SYSTEMDB_ENTER_HALT);
  }
}

void rpm_halt_exit(void)
{
  if (sleep_result == SLEEP_SUCCESS)
  {
    //this log happens extremely often right now. Disable for the time being
    SWEVENT(SLEEP_RPM_HALT_EXIT);
    system_db_statehandler(SYSTEMDB_EXIT_HALT);
    // Record the timestamp
    rob_mark_event (ROB_EVENT_RPM_HALT_EXIT);
    rob_mark_duration (ROB_DURATION_RPM_HALT_ENTER);
    rob_mark_duration (ROB_DURATION_RPM_HALT_EXIT);
  }
}

void deep_sleep_enter(void)
{
  uint64 sleep_duration;
  pm_err_flag_type pmic_err;

  sleep_result = SLEEP_SUCCESS;

  rob_set_recording_speed (svs_current_speed());
  if (bimc_pced)
  {
    rob_mark_event (ROB_EVENT_VDD_MIN_BIMC_PCED_ENTER);
  }
  else
  {
    rob_mark_event (ROB_EVENT_VDD_MIN_ENTER);
  }

  do
  {
    // Limit sleep time to 15 seconds -> 0x112A8800 = 15 sec in 19.2 MHz ticks
    //sleep_duration = MIN( calculate_sleep_duration(), (0x112A8800) );

    sleep_duration = calculate_sleep_duration();

    if (!sleep_duration)
    {
      //log that we bailed early
      SWEVENT(SLEEP_NO_DEEP_SLEEP, sleep_mode.deep_sleep_mode, 0);
      sleep_result = SLEEP_ERROR_NOT_ENOUGH_TIME;
      deep_sleep_bail_count++;
      break;
    }
    sleep_duration += mpm_enter_latency;
    if(!prepare_interrupts())
    {
      //log that we bailed early
      SWEVENT(SLEEP_NO_DEEP_SLEEP, sleep_mode.deep_sleep_mode, 1);
      sleep_result = SLEEP_ERROR_PENDING_INTERRUPT;
      deep_sleep_bail_count++;
      break;
    }
#ifdef TSENSE_SAMPLING_1SEC
    // save the current sampling rate and reduce the sampling rate to 1 sec
    // to improve sleep current
    HAL_deep_sleep_enter_tsense();
#endif

    gpio_test_low(GPIO_DEEP_SLEEP,GPIO_SLEEP_TRANSITION);
    // Ensure that we don't exceed sleep_stats array bounds --
    CORE_VERIFY(sleep_mode.deep_sleep_mode < SLEEP_STATS_COUNT);
    // update sleep_stats here to avoid updating if we bail out.
    sleep_stats[sleep_mode.deep_sleep_mode].count ++;
    sleep_stats[sleep_mode.deep_sleep_mode].last_entered_at = time_service_now ();
    // Log sleep enter here to avoid claiming we went to sleep if we bail out.
    SWEVENT(SLEEP_DEEP_SLEEP_ENTER, sleep_mode.deep_sleep_mode, sleep_stats[sleep_mode.deep_sleep_mode].count);

#ifdef FEATURE_ETM
    ETM_save_state();
#endif

    rbcpr_prepare_for_sleep();
    // put ddr into self refresh and power collapse bimc if its configured to do so
    Clock_ProcessorSleep(CLOCK_SLEEP_MODE_POWER_COLLAPSE, BIMC_PC_CFG);
    
    
    ddr_enter_sleep();

    mpm_setup_chip_sleep();

    // Configure voltage to sleep.
    pmic_err = pm_rpm_enter_sleep(PM_SLEEP_MODE_VDD_MIN);
    CORE_VERIFY(pmic_err == PM_ERR_FLAG__SUCCESS);

    // put all (capable) clocks to sleep except bimc
    Clock_ProcessorSleep(CLOCK_SLEEP_MODE_POWER_COLLAPSE, 0);

    SWEVENT(SLEEP_DEEP_SLEEP_ENTER_COMPLETE, sleep_mode.deep_sleep_mode, sleep_duration);
    mpm_sw_done(sleep_duration);
  } while(FALSE);
}

void deep_sleep_exit(void)
{
  pm_err_flag_type pmic_err;
  uint64 deep_sleep_time = (time_service_now() - enter_deep_sleep_time);

  SWEVENT(SLEEP_DEEP_SLEEP_EXIT, sleep_mode.deep_sleep_mode);

  if(sleep_result == SLEEP_SUCCESS)
  {
#ifdef TSENSE_SAMPLING_1SEC
    // restore the sampling rate of tsense
    HAL_deep_sleep_exit_tsense();
#endif
    //restore everything except bimc
    Clock_ProcessorRestore(CLOCK_SLEEP_MODE_POWER_COLLAPSE, 0);

    // restore voltage
    pmic_err = pm_rpm_exit_sleep(PM_SLEEP_MODE_VDD_MIN);
    CORE_VERIFY(pmic_err == PM_ERR_FLAG__SUCCESS);

    //take ddr out of self-refresh and restore bimc if its configured to do bimc power collapse
    Clock_ProcessorRestore(CLOCK_SLEEP_MODE_POWER_COLLAPSE, BIMC_PC_CFG);

    ddr_exit_sleep();

    rbcpr_exit_sleep();

#ifdef FEATURE_ETM
    ETM_restore_state();
#endif
  }

  if(sleep_result != SLEEP_ERROR_NOT_ENOUGH_TIME)
  {
  restore_interrupts();
  }

  mpm_teardown_sleep();

  gpio_test_high(GPIO_DEEP_SLEEP, GPIO_ACTIVE_TRANSITION);
  SWEVENT(SLEEP_DEEP_SLEEP_EXIT_COMPLETE, sleep_mode.deep_sleep_mode);

  // mark the event
  if (bimc_pced)
  {
    rob_mark_event (ROB_EVENT_VDD_MIN_BIMC_PCED_EXIT);
    rob_mark_duration (ROB_DURATION_VDD_MIN_BIMC_PCED_ENTER);
    rob_mark_duration (ROB_DURATION_VDD_MIN_BIMC_PCED_EXIT);
  }
  else
  {
    rob_mark_event (ROB_EVENT_VDD_MIN_EXIT);
    rob_mark_duration (ROB_DURATION_VDD_MIN_ENTER);
    rob_mark_duration (ROB_DURATION_VDD_MIN_EXIT);
  }
  if(sleep_result == SLEEP_SUCCESS)
  {
    sleep_stats[sleep_mode.deep_sleep_mode].last_exited_at = time_service_now ();
    sleep_stats[sleep_mode.deep_sleep_mode].accumulated_duration += (sleep_stats[sleep_mode.deep_sleep_mode].last_exited_at - sleep_stats[sleep_mode.deep_sleep_mode].last_entered_at);
  }
}

void vdd_min_enter(bool mock_vdd_min)
{
  railway_settings cx_settings;
  railway_settings mx_settings;
  uint32 sleep_uv_mx;
  uint32 sleep_uv_cx;

  railway_get_current_settings(cx_id, &cx_settings);
  railway_get_current_settings(mx_id, &mx_settings);

  railway_get_sleep_voltages(cx_id, mx_id, &sleep_uv_cx, &sleep_uv_mx);

  if((mock_vdd_min) || (sleep_uv_mx > (sleep_uv_cx + MAX_CX_MX_DELTA_UV)))
  {
    mpm_set_vmin_voltages(mx_settings.microvolts, mx_settings.microvolts,
                          cx_settings.microvolts, cx_settings.microvolts);

    sleep_mode.deep_sleep_mode = MPM_SLEEP_MODE_VDD_LOW;
  }
  else
  {

    CORE_VERIFY(sleep_uv_mx <= mx_settings.microvolts);
    CORE_VERIFY(sleep_uv_cx <= cx_settings.microvolts);
    CORE_VERIFY(sleep_uv_mx <= sleep_uv_cx + MAX_CX_MX_DELTA_UV);

    mpm_set_vmin_voltages(sleep_uv_mx, mx_settings.microvolts,
                          sleep_uv_cx, cx_settings.microvolts);

    if (sleep_uv_cx > railway_get_corner_voltage(cx_id, RAILWAY_RETENTION))
      sleep_mode.deep_sleep_mode = MPM_SLEEP_MODE_VDD_LOW;
    else
      sleep_mode.deep_sleep_mode = MPM_SLEEP_MODE_VDD_MIN;
  }

  if(sleep_mode.deep_sleep_mode == MPM_SLEEP_MODE_VDD_LOW)
  {
    system_db_statehandler(SYSTEMDB_ENTER_XO);
    /*
     * Prevent freeze IOs during vdd_low for MIPI static display
     */
    mpm_config_freeze_ios(FALSE);
  }
  else
  {
    system_db_statehandler(SYSTEMDB_ENTER_VDDMIN);
    /*
     * Use the BSP config data for freeze IOs enablement
     */
    mpm_config_freeze_ios(TRUE);
  }

  vdd_min_latency();

  deep_sleep_enter();
}

void vdd_min_exit(bool mock_vdd_min)
{
  deep_sleep_exit();

  if(sleep_mode.deep_sleep_mode == MPM_SLEEP_MODE_VDD_LOW)
  {
    system_db_statehandler(SYSTEMDB_EXIT_XO);
  }
  else
  {
    system_db_statehandler(SYSTEMDB_EXIT_VDDMIN);
  }
  	
  if(svs_get_svs_switch_to_fast())
  {
     //In vdd_min enter path, it is decided that we go to FAST mode on exit path,
     //So force the FAST mode here.
     svs_set_mode(SVS_FAST, false);
	 svs_set_svs_switch_to_fast(0);
  }
  
}

void sleep_update_xo_stats(const npa_client *xo_clients, const npa_client *client_updated)
{
  int client_id = -1;
  int vote = 0;
  const npa_client *client = NULL;

  if(client_updated->type != NPA_CLIENT_REQUIRED)
  {
    return;
  }

  client_id = rpm_npa_get_ee_client_id(client_updated);

  if(client_id < 0)
  {
    /*
     * ignore clock driver's xo vote change
     */
    return;
  }

  vote = client_updated->work[client_updated->index^1].state; // index is not updated yet. Need to XOR to get correct index

  /*
   * The Least Significant Byte contains the last xo stats.
   * Check if the client's vote is changed from the last one
   */
  if(sleep_stats && !(vote ^ (0x1 & (sleep_stats[0].client_votes >> client_id))))
  {
    /*
     * xo stats is not changed
     */
    return;
  }

  /*
   * update xo stats in MSG RAM
   */
  message_ram_update_master_xo_stats(client_id, vote);

  if(!sleep_stats)
  {
    /*
     * sleep_stats is not initialized in sleep_modes_init yet
     */
    return;
  }

  /*
   * update xo stats in sleep stats
   */
  // allocate space for current vote state
  sleep_stats[0].client_votes <<= MAX_XO_CLIENTS;

  // traverse client list and store the current XO votes
  for(client = xo_clients; client != NULL; client = client->next)
  {
    if(client->type != NPA_CLIENT_REQUIRED)
      continue;

    client_id = rpm_npa_get_ee_client_id(client);

    if(client_id < 0)
      continue;

    if(client != client_updated)
      vote = client->work[client->index].state;
    else
      vote = client->work[client->index^1].state; // index is not updated yet. Need to XOR to get correct index

    if(vote)
    {
      sleep_stats[0].client_votes |= (1 << client_id);
    }
  }
}

/*===========================================================================
FUNCTION: sleep_is_any_interrupt_pending

DESCRIPTION: 
This is the same function as is_any_interrupt_pending in exceptions.c.
The main difference is that by the time this function is called, we would have 
already cleared the NVIC Enabled register. Instead, we use the backup taken in 
a global array. 

RETURN VALUE: True if any interrupt is pending
===========================================================================*/
bool sleep_is_any_interrupt_pending(void)
{
  int num_ints, i;
  bool result = false;
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;
  CORE_VERIFY_PTR(enabled_interrupts);
  for(i = 0; ((i < num_ints / 32) && !result); i++)
  {
    if (enabled_interrupts[i] & SCS.NVIC.Set[i])
      result = true;
  }
  return result;
}

