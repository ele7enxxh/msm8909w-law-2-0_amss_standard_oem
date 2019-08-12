/*============================================================================
  FILE:         sleep_perform.c

  OVERVIEW:     This file provides the sleep task and OS-specific
                functionality for the RPM processor.

  DEPENDENCIES: sleep_init() should be called before using any other function
                in the sleep subsystem.

                Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================

$Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/src/sleep_perform.c#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $

========================================================================*/
#include "cortex-m3.h"
#include "DALSysTypes.h"
#include "CoreConfig.h"
#include "CoreVerify.h"
#include "npa.h"
#include "rpm_definitions.h"
#include "time_service.h"
#include "pm_clk_sleep.h"
#include "pm_clk_xo.h"
#include "pm_pon.h"
#include "pm_version.h"
#include "mpm.h"

#include "railway.h"
//#include "dog.h"

/* ==================================================================
                        EXTERNAL VARIABLES
   ================================================================== */

/* ==================================================================
                  EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */
extern void rpm_halt_enter(void);
extern void cxo_shutdown_enter(void);
extern void vdd_min_enter(bool mock_vdd_min);
extern void rpm_halt_exit(void);
extern void cxo_shutdown_exit(void);
extern void vdd_min_exit(bool mock_vdd_min);
extern uint64_t sched_get_next_start(void);
extern void sleep_early_init(void);
extern void sleep_target_init(void);
extern void set_vdd_dig_sleep_voltage (uint32 *vdd_dig);
extern void set_vdd_mem_sleep_voltage (uint32 *vdd_mem);

void sleep_modes_init(void);

int    cx_id, mx_id;
pm_on_off_type tcxo_on;
static npa_query_handle sleep_cxo_query_handle;
typedef void (*lprm_func)(void);
/* ==================================================================
                        GLOBAL DEFINES
   ================================================================== */
#define START_TIME_HAIRY_THRESHOLD (2000)

#ifdef MSM_DISABLE_SLEEP
volatile boolean sleep_allow_low_power_modes = FALSE;
#else
volatile boolean sleep_allow_low_power_modes = TRUE;
#endif
#ifdef MSM_DISABLE_DEEP_SLEEP
volatile boolean deep_sleep_enabled = FALSE;
#else
volatile boolean deep_sleep_enabled = TRUE;
#endif

#ifdef MSM_MOCK_VDD_MIN_ONLY
volatile boolean mock_vdd_min_only = TRUE;
#else
volatile boolean mock_vdd_min_only = FALSE;
#endif

/* ==================================================================
                  INTERNAL FUNCTION DECLARATIONS
   ================================================================== */

void sleep_init(void)
{
    uint32 cx_retention;
    uint32 mx_retention;

    cx_id = rail_id("vddcx");
    mx_id = rail_id("vddmx");
    
    assert(RAIL_NOT_SUPPORTED_BY_RAILWAY!=cx_id);
    assert(RAIL_NOT_SUPPORTED_BY_RAILWAY!=mx_id);

    set_vdd_dig_sleep_voltage(&cx_retention);
    railway_set_corner_voltage(cx_id, RAILWAY_RETENTION, cx_retention);

    set_vdd_mem_sleep_voltage(&mx_retention);
    mx_retention = MAX (mx_retention, cx_retention);
    railway_set_corner_voltage(mx_id, RAILWAY_RETENTION, mx_retention);

    sleep_modes_init();

    /* set tcxo status */
    /* TODO enable after PMIC init validation */
    pm_clk_sleep_src_status(0, PM_CLK_SLEEP, &tcxo_on);
    /* allow the mpm to set up cxo sleep times based on this*/
    mpm_init_xtal();

    CORE_VERIFY_PTR(sleep_cxo_query_handle = npa_create_query_handle( "/xo/cxo" ));
}

/*===========================================================================
FUNCTION: sleep_deep_imminent

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
bool sleep_deep_imminent (void)
{
  npa_query_type   sleep_query_result;
  npa_query_status sleep_query_status;

  if(sleep_cxo_query_handle != NULL)
  {
	sleep_query_status = npa_query(sleep_cxo_query_handle, NPA_QUERY_CURRENT_STATE, &sleep_query_result);
	CORE_VERIFY(NPA_QUERY_SUCCESS == sleep_query_status);
	CORE_VERIFY(NPA_QUERY_TYPE_STATE == sleep_query_result.type);

	return (deep_sleep_enabled && (!(sleep_query_result.data.state)));
  }
  else
  {
	return 0;
  }
}

/*===========================================================================
FUNCTION: sleep_perform_lpm

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
uint32 sleep_perform_lpm(void)
{
    static volatile  uint64 next_work_start = 0;

    if(!sleep_allow_low_power_modes)
    {
        return 0;
    }

    INTLOCK();
    do
    {
        if(is_any_interrupt_pending())
        {
            break;
        }

        /* Calculate the duration until the backoff time */
        next_work_start = sched_get_next_start();
        if ((time_service_now () + START_TIME_HAIRY_THRESHOLD) >= next_work_start)
        {
          //there is imminent work, so we shouldn't go to sleep
          break;
        }

        if(sleep_deep_imminent())
        {
            vdd_min_enter(mock_vdd_min_only);
            vdd_min_exit(mock_vdd_min_only);
        }
        else
        {
            rpm_halt_enter();
            rpm_halt_exit();
        }
    } while(0);
    INTFREE();

    return 0;
}

