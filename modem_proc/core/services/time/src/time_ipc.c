/*=============================================================================

                  T I M E  - I P C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements IPC time task

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs

EXTERNALIZED FUNCTIONS
  time_ipc_task
    Initializes time Time_IPC task

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_ipc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/12/10   din     Made changes to compile on apps as well.
03/25/10   din     Updated time_ipc to be used for single proc targets.
03/11/10   din     Moved efs write to Time IPC task.
02/22/10   din     Fixed compilation issue incase DOG_DYNAMIC_API is not 
                   defined.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
02/08/10   sri     Made changes for DOG_DYNAMIC_API
05/22/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timer.h"
#include "time_ipc.h"
#include "time_genoff_v.h"
#include "rex.h"
#include "dog_hb_rex.h"
#include "time_efs.h"
#include "ats_v.h"
#include "rcinit.h"
#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
#include "time_service_v01.h"
#include "qmi_client.h"
extern qmi_client_type time_qmi_client_handle;
#endif/*FEATURE_QMI_TIME_REMOTE_CLNT*/

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/* Watchdog report signal */
#define TIME_IPC_RPT_SIG   0x0004

/*  Dog variables - now we MAY be dynamic       */
uint32  dog_time_ipc_rpt       = 0;       /* dog report ID */

static rex_tcb_type *time_ipc_tcb_ptr = NULL;

#ifdef FEATURE_TIME_IPC_TASK

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION TIME_IPC_KICK_WATCHDOG

DESCRIPTION
  This procedure reports to the watchdog and resets the timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
static void time_ipc_kick_watchdog(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dog_hb_report(dog_time_ipc_rpt);

} /* time_ipc_kick_watchdog */



/*===========================================================================

FUNCTION TIME_IPC_READY_START_TASK

DESCRIPTION
  This procedure is called  by timer IPC when it is ready to start its task.
  It will wait on TASK_START_SIG after this call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
static void time_ipc_ready_start_task (void)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rcinit_handshake_startup();                                                   /* Handshake allows startup to progress */

  ATS_MSG("TIME IPC TASK initialization successfull");

} /* time_ipc_ready_start_task */

#endif /* FEATURE_TIME_IPC_TASK */


/*=============================================================================

FUNCTION TIME_IPC_TASK

DESCRIPTION
  Initializes the Time_IPC Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void  time_ipc_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           parm    /*lint -esym(715,ignored) */
)
{

#ifdef FEATURE_TIME_IPC_TASK
  /* Signals returned from rex wait procedure */
  rex_sigs_type   sigs;

  #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
  time_get_leap_sec_resp_msg_v01 time_resp;
  time_genoff_leap_sec_info_type leap_sec_info;
  #endif /*FEATURE_QMI_TIME_REMOTE_CLNT*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep Lint happy */

  #ifdef _lint
    parm = parm;
  #endif

  if ( time_ipc_tcb_ptr == NULL )
  {
    time_ipc_tcb_ptr = rcinit_lookup_rextask("time_ipc");
  }

  /* Initialize the watchdog report timer */
  dog_time_ipc_rpt      = dog_hb_register_rex( TIME_IPC_RPT_SIG );

  /* Ready to start task */
  time_ipc_ready_start_task();

  time_ipc_kick_watchdog();

  for(;;)
  {
    /* Wait for some task signal */
    sigs = rex_wait( TIME_IPC_RPT_SIG 
                        | TIME_IPC_GENOFF_UPDATE
                        | TIME_IPC_EFS_WRITE
                        | TIME_IPC_MM_REGISTRATION
                        | TIME_IPC_SEND_REMOTE_MODEM_UDPATE
                        | TIME_IPC_SEND_REMOTE_USER_UDPATE
                        | TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE
                        | TIME_IPC_QMI_REMOTE_COMM
                        | TIME_IPC_QMI_REMOTE_COMM_TIMEOUT
                        | TIME_IPC_QMI_REMOTE_COMM2 );
   
    /* Just in case TIME_IPC_RPT_SIG fired which would bring us here*/
    if ( sigs & TIME_IPC_RPT_SIG  )
    {
      /*signal handler for TIME_IPC_RPT_SIG goes here*/

      (void)rex_clr_sigs (time_ipc_tcb_ptr,TIME_IPC_RPT_SIG);

      /* Kick watchdog and start the watchdog report timer.   */
        time_ipc_kick_watchdog();
      }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_IPC EFS_WRITE SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_EFS_WRITE )
    {
      (void)rex_clr_sigs ( time_ipc_tcb_ptr,TIME_IPC_EFS_WRITE );
      /* Process the expired timer */
      #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
      time_efs_write();
      #endif /* FEATURE_TIME_SINGLE__MULTI_SERVER_PROC  */
    }

     /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_MM_REGISTRATION_SIGNAL SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    /* We don't support this anymore */
    if ( sigs & TIME_IPC_MM_REGISTRATION )
    {
      ATS_MSG_1("Unsupported signal set on time_ipc_task: 0x%x",TIME_IPC_MM_REGISTRATION);
      (void)rex_clr_sigs ( time_ipc_tcb_ptr,TIME_IPC_MM_REGISTRATION );
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                     TIME_IPC_SEND_REMOTE_MODEM_UDPATE SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_SEND_REMOTE_MODEM_UDPATE )
    {
      (void)rex_clr_sigs ( time_ipc_tcb_ptr,TIME_IPC_SEND_REMOTE_MODEM_UDPATE );
      ATS_MSG("mm_debug_fusion:Processing request for remote modem update");
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_TOD);
    }

    if ( sigs & TIME_IPC_SEND_REMOTE_USER_UDPATE )
    {
      (void)rex_clr_sigs ( time_ipc_tcb_ptr,TIME_IPC_SEND_REMOTE_USER_UDPATE );
      ATS_MSG( "mm_debug_fusion:Processing request for remote user time update");//auto-gen, to change remove 'QSR_' and first param
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_USER);
    }

    if ( sigs & TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE )
    {
      (void)rex_clr_sigs ( time_ipc_tcb_ptr,TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE );
      ATS_MSG("mm_debug_fusion:Processing request for remote user tz dl update");//auto-gen, to change remove 'QSR_' and first param
      /* Process the mm timer expiry */
      time_genoff_set_server_modem_time(ATS_USER_TZ_DL);
    }
   
    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                       TIME_TOD RESYNC SIGNAL                           */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_TOD_RESYNC_SIG )
    {
      ATS_MSG_1("Unsupported signal set on time_ipc_task: 0x%x",TIME_TOD_RESYNC_SIG);
      (void)rex_clr_sigs (time_ipc_tcb_ptr, TIME_TOD_RESYNC_SIG);
    }

    /*-------------------------------------------------------------------------*/
    /*                                                                         */
    /*                       GENOFF OFFSET UPDATE SIGNAL                       */
    /*                                                                         */
    /*-------------------------------------------------------------------------*/

    if ( sigs & TIME_IPC_GENOFF_UPDATE )
    {
      (void)rex_clr_sigs (time_ipc_tcb_ptr, TIME_IPC_GENOFF_UPDATE );
      #if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
      time_genoff_remote_through_time_ipc();
      #endif /* FEATURE_TIME_MULTI_SERVER_PROC  */
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM) 
    {
      (void)rex_clr_sigs (time_ipc_tcb_ptr,TIME_IPC_QMI_REMOTE_COMM);
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM_TIMEOUT) 
    {
      (void)rex_clr_sigs (time_ipc_tcb_ptr,TIME_IPC_QMI_REMOTE_COMM_TIMEOUT);
    }

    if(sigs & TIME_IPC_QMI_REMOTE_COMM2) 
    {
      (void)rex_clr_sigs (time_ipc_tcb_ptr,TIME_IPC_QMI_REMOTE_COMM2);

      #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
      /*if its a TOD offset, then set the leap second info*/
      (void) qmi_client_send_msg_sync(time_qmi_client_handle,
                                      QMI_TIME_LEAP_SEC_GET_REQ_V01,
                                      NULL,
                                      0,
                                      &time_resp,
                                      sizeof(time_resp),
                                      5000);
    
       if(QMI_ERR_NONE_V01 == time_resp.resp.error) 
       {
         leap_sec_info.leap_sec_valid = TRUE;
         leap_sec_info.leap_sec_value = time_resp.leap_second;
         time_genoff_set_leap_sec_info(leap_sec_info);
         ATS_MSG("TIME_IPC_QMI_REMOTE_COMM2: Success");
       }

      #endif
    }
  } /* for (;;) */

#else
  ATS_ERR("TIME_IPC : FEATURE_TIME_IPC_TASK not defined.");
  return;
#endif /* FEATURE_TIME_IPC_TASK */

} /* time_ipc_task */


