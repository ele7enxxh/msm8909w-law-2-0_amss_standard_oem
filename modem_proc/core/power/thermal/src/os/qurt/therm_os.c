/*============================================================================
  @file therm_os.c

  Thermal OS specific functionality.  Task init.

  Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/os/qurt/therm_os.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "therm_v.h"
#include "qmi_csi.h"
#include "rcinit.h"

#include "DDIPlatformInfo.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
static uint32 gThermMask = THERM_TASK_SIG_MASK;

extern rex_tcb_type thermal_tcb;

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_task

  Thermal Mitigation task.

  @param ignored: Ignored.
*/
void thermal_task
(
  dword ignored
     /* Parameter received from Rex - ignored */
     /*lint -esym(715,ignored)
     ** Have lint not complain about the ignored parameter 'ignored' which is
     ** specified to make this routine match the template for rex_init().
     */
)
{

  if (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI)
  {
    /* Exit thermal task on RUMI platforms.  Init must return after this handshake call.*/  
    rcinit_handshake_startup();
    return;
  }

  qmi_csi_os_params      os_params[2];
  qmi_csi_service_handle service_handle[2] = {NULL};

  rex_sigs_type rcvdSigs;   /* signals returned from REX */

  /*  Initialize the DEM system on this target. */
  therm_target_init();

  therm_monitor_init();

  /* Start the task */
  rcinit_handshake_startup(); // required rcinit handshake

  os_params[0].tcb = &thermal_tcb;
  os_params[0].sig = THERM_QMI_MITIGATION_DEV_SIG;

  os_params[1].tcb = &thermal_tcb;
  os_params[1].sig = THERM_QMI_SENSOR_SIG;

  /* Initialize after task start on this target. */
  therm_target_after_task_start_init();

  therm_monitor_after_task_start_init();

  service_handle[0] = therm_qmi_dev_service_init(&os_params[0]);
  service_handle[1] = therm_qmi_sensor_service_init(&os_params[1]);

  while( 1 )
  {
    /* Wait for new signals to be received. */
    rcvdSigs = rex_wait(gThermMask);

    /* Process rex signal */
    therm_target_process_sigs(rcvdSigs);

    therm_monitor_process_sigs(rcvdSigs);

    therm_qmi_dev_service_process_sigs(rcvdSigs);

    if (rcvdSigs & THERM_QMI_MITIGATION_DEV_SIG)
    {
      rex_clr_sigs(&thermal_tcb, THERM_QMI_MITIGATION_DEV_SIG);
      qmi_csi_handle_event(service_handle[0], &os_params[0]);
    }

    if (rcvdSigs & THERM_QMI_SENSOR_SIG)
    {
      rex_clr_sigs(&thermal_tcb, THERM_QMI_SENSOR_SIG);
      qmi_csi_handle_event(service_handle[1], &os_params[1]);
    }
  }
}

