/*==========================================================================
Description
  Target-specific diagnostic system routines.  These were moved to a separate 
  file to avoid coupling with MC and other IS95 DMSS components.
    
Copyright (c) 2001-2014 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagmm.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0. 
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
11/01/13   rh      Switched from /core/cpu/vdd to /core/cpu/latency/usec
02/15/13   sr      Moved to new Dog heartbeat implementation   
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes 
07/11/12   ra      Removed Deprecated Sleep API; now using NPA  
04/02/2012 rks     Removed False Dependency to TMC.H in CoreBSP-Services
11/09/11   is      Clear DIAG_RPT_TIMER_SIG after handling it
09/28/11   is      Support dynamic sleep voting
06/16/11   is      Support transition of initialization and tasks from CoreImg
                   into RCINIT framework
09/17/10   vs      set rpt timer based on run-time params
05/11/10   is      Replaced external feature FEATURE_IS2000 with internal 
                   feature DIAG_IS2000
05/10/10   is      Mapped external features for call manager functionality with
                   internal feature DIAG_CM_AVAIL.
04/29/10   is      Removed unused function mc_set_digital_offline()
03/18/10   JV      Replaced deprecated APIs
12/23/09   vg      removed call to srch_rude_wakeup( FALSE );
09/01/09   JV      Fixed compile error.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Brought back FEATURE_DIAG_NV
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09   as      Mainlined API for FEATURE_OTASP and FEATURE_UIM_RUIM
06/23/09   JV      Introduced OS abstraction layer
05/07/09   vk      changed includes from external to internal headers
03/02/09   is      Fix featurizations to keep modem/apps procs in sync in using 
                   new vs old sleep API
02/03/09   is      Fix featurization for WM builds related to diag_sleep_okts_handle
01/19/09   is      Switch to new sleep voting interface
12/15/06   as      Fixed critical lint errors.
10/31/05   as      Fixed lint errors.
02/23/05   as      Included srchmc.h to prevent compiler warnings
01/07/03   djm     add mc set digital offline function for WPLT use, mode change
01/08/03   ma      featurization for GSM PLT
12/08/02   djm     corrected WPLT featurization issue
11/20/01   as      Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#include "diagtarget.h"
#endif
#include "diag_v.h"
#include "diagi_v.h"
#include "log.h"
#include "msg.h"
#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#include "assert.h"
#endif

#ifdef DIAG_CM_AVAIL
#include "cm.h"
#endif

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#endif

#if defined(FEATURE_RCINIT)
#include "rcevt.h"
#include "rcinit.h"
#endif

#if defined (DIAG_DYNAMIC_SLEEP)
#include "npa.h" /* For npa_client_handle, etc */
npa_client_handle diag_npa_handle = NULL;
npa_client_handle diagtest_npa_handle = NULL;
#endif

#if defined(DIAG_DOG_SUPPORT)
extern osal_dog_report_type diag_dog_rpt_var;
#endif
extern diag_sleep_state_enum_type diag_sleep_state; /*from diag.c */
/*===========================================================================

FUNCTION DIAG_OFFLINE

DESCRIPTION
  This function performs processing required when the diag task receives
  a signal to go offline.  This processing consists of clearing the signal
  DIAG_OFFLINE_COMPLETE_SIG and acknowledging mc's offline signal.

===========================================================================*/
void diag_offline (void)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Got offline state change");

  /* Notify the task manager that DIAG has gone offline.  DIAG doesn't 
     actually do anything differnt, but it must ACK the offline
     command nonetheless. */
  /* Now that we have transitioned to RCINIT, we might want to
     find out if we should be doing anything like this.
     It does not seem to be called anywhere -- rhalstea */
#if !defined(FEATURE_WINCE) && !defined (DIAG_QDSP6_APPS_PROC) && !defined (FEATURE_RCINIT)
  tmc_task_offline ();
#endif
} /* diag_offline */



/*===========================================================================

FUNCTION DIAG_READY_START_TASK

DESCRIPTION
  This procedure is called by diag when it is ready to start its task.
  Diag will wait on DIAG_TASK_START_SIG (0x8000) after this call.

===========================================================================*/
void diag_ready_start_task (void)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"DIAG task is started");
    
#if defined (DIAG_QDSP6_APPS_PROC)
  osal_sigs_t return_sigs;
  int return_val = 0;
  osal_thread_wait(diag_task_tcb, DIAG_TASK_START_SIG);
  return_val = osal_reset_sigs(diag_task_tcb, DIAG_TASK_START_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
  
#elif !defined(FEATURE_WINCE)

  #if defined(FEATURE_RCINIT)
    /* Handshake allows startup to progress */
    rcinit_handshake_startup(); 
  #else
    /* Instruct the task controller service to start this task */
    tmc_task_start();  
  #endif
  
#endif
} /* diag_ready_start_task */



/*===========================================================================

FUNCTION DIAG_SHUTDOWN

DESCRIPTION
  This procedure performs shutdown processing for the Diagnostic task.

  After this procedure is called the Diagnostic task will not be running.  
 
===========================================================================*/
void diag_shutdown (void) 
{
   osal_sigs_t return_sigs;
   int return_val = 0;
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Diag shutdown state change");

   return_val = osal_reset_sigs(diag_task_tcb, DIAG_TASK_STOP_SIG, &return_sigs);
   ASSERT(OSAL_SUCCESS == return_val);
   /* Now that we have transitioned to RCINIT, we might want to
      find out if we should be doing anything like this.
      It does not seem to be called anywhere -- rhalstea */
#if !defined(FEATURE_WINCE) && !defined (DIAG_QDSP6_APPS_PROC) && !defined (FEATURE_RCINIT)
   /* Instruct the task controller service to shut down the system. */
   tmc_task_stop ();
#endif
} /* diag_shutdown */



/*===========================================================================

FUNCTION DIAG_POWERDOWN

DESCRIPTION
  This procedure sends the command to MC to power down the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The phone must powerdown as a result of this call.
 
===========================================================================*/
void diag_powerdown (void) 
{
#ifdef DIAG_CM_AVAIL
    (void) cm_ph_cmd_oprt_mode(
                        NULL,
                        NULL,
                        CM_CLIENT_ID_ANONYMOUS,         /* Dummy client ID */
                        SYS_OPRT_MODE_PWROFF
                       );
#endif /* DIAG_CM_AVAIL */                       
} /* diag_powerdown */


/*===========================================================================

FUNCTION DIAG_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

===========================================================================*/
void diag_kick_watchdog(void)
{
#if defined(DIAG_DOG_SUPPORT)
  int return_val = 0;
  osal_sigs_t return_sigs;
  
  return_val = osal_reset_sigs(diag_task_tcb, DIAG_RPT_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  /*Report to the Dog HB*/
  osal_dog_hb_report(diag_dog_rpt_var);        

#endif
  return;
}


/*===========================================================================

FUNCTION DIAG_GET_CPU_CLIENT_HANDLE_CB

DESCRIPTION
  This callback is called when sleep task is ready for NPA handles to be
  created.

===========================================================================*/
#if defined (DIAG_DYNAMIC_SLEEP)
static void diag_get_cpu_client_handle_cb( void *context,
                                           unsigned int event_type,
                                           void *data,
                                           unsigned int data_size )
{
  diag_npa_handle = npa_create_sync_client( "/core/cpu/latency/usec",
                                            "DIAG_NPA_CLIENT",
                                            NPA_CLIENT_REQUIRED );
                                            
  diagtest_npa_handle = npa_create_sync_client( "/core/cpu/latency/usec", 
                                                "DIAGTEST_NPA_CLIENT",
                                                NPA_CLIENT_REQUIRED );
  
} /* diag_get_cpu_client_handle_cb */
#endif


/*===========================================================================

FUNCTION DIAG_SLEEP_INIT

DESCRIPTION
  This procedure initializes variables for Diag sleep voting.

===========================================================================*/
void diag_sleep_init( void )
{  
#if defined (DIAG_DYNAMIC_SLEEP)
  
  /* Registering a callback function to the resource to obtain its client
     and query handles. The CB will be called immediately if the 
     NPA node already exists, so it's safe to just register the CB here. */  
  npa_resource_available_cb( "/core/cpu/latency/usec", 
                             diag_get_cpu_client_handle_cb, 
                             NULL );

#endif
                                            
} /* diag_sleep_init */


/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  This procedure votes for sleep for the Diag task.

===========================================================================*/
void diag_allow_sleep (void)
{
#if defined (DIAG_DYNAMIC_SLEEP)
  if( diag_npa_handle )
  {
    npa_cancel_request( diag_npa_handle );
    diag_sleep_state= DIAG_SLEEP_STATE_ASLEEP;
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Cannot vote for sleep: diag_npa_handle is null");
  }
#endif

} /* diag_allow_sleep */


/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  This procedure votes against sleep for the Diag task.

===========================================================================*/
void diag_forbid_sleep(void)
{
#if defined (DIAG_DYNAMIC_SLEEP)
  if( diag_npa_handle )
  {
      npa_issue_required_request( diag_npa_handle, 1);
      diag_sleep_state = DIAG_SLEEP_STATE_AWAKE;
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Cannot vote against sleep: diag_npa_handle is null");
  }
#endif

} /* diag_forbid_sleep */

/*lint +esym(715,pkt_len,req_ptr) */



/*===========================================================================

FUNCTION DIAG_OTASP_UPDATE

DESCRIPTION
  This procedure tells DIAG to refresh its status cache from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void diag_otasp_update( void )
{
  osal_sigs_t return_sigs; 
  int return_val = 0;
  return_val = osal_set_sigs(diag_task_tcb, DIAG_REFRESH_CACHE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
} /* diag_otasp_update */

