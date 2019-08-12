/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===

				M A C    H S D P A    D O W N L I N K    T A S K

GENERAL DESCRIPTION
	This file contains the Downlink MAC HSDPA task functions.

Copyright (c) 2000-2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================================*/
/*=============================================================================================

											EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/machstask.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------------
01/29/14   kc      Fix not to request new p-t-c until TSNs from prev p-t-c are all delivered
11/26/13   geg     Segment loading FSL: add dummy task 
11/12/13   kv      Removed deprecated header file, tmc.h
03/26/13   scv     L2 changes to support new dog architecture 
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
03/28/12   zr      Adding feature definitions for Dime
03/08/12   amj     Made changes for RC init task initialization support.
01/02/12   grk     Feature cleanup
09/19/10   av      HS_FACH and Enhanced 0x4220 log changes
09/17/10   grk     Moving Dynamic Dog API changes under FEATURE_WCDMA_L2_WDOG_DYNAMIC.
09/14/10   grk     Support to use Dynamic DOG APIs to register L2 tasks for dog monitoring.
05/07/09   ssg     Updated Copyright Information
02/25/09   kvk     Fixed compile error.
02/24/09   kvk     Added mutex protection to avoid q_next and q_check happening in concurrent
                   with q_get and q_put which can avoid concurrency beetween T1 timer handler as well
                   window handler routines
12/18/08   ksr     L2 Mutex changes to avoid concurrency issues
05/26/06   gsc     Get current state of time sigs before running timer handler as they may
                   have been cleared from window handler while stopping the timer.
03/12/06   sk      Cast the return value to voice for rex_wait and rex_clr_sigs function
01/12/05   gsc     Added 1 sec timer for reporting status log pkt.
01/04/05   gsc     Removed an F3 message.
11/04/04   gsc     Some code cleanup in wcdma_mac_hs_dl_task.
11/03/04   gsc     Featurized with FEATURE_HSDPA.
10/11/04   gsc     Initial revision

=============================================================================================*/


/*===============================================================================================

							I N C L U D E    F I L E S

=============================================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include  "rex.h"
#include  "task.h"
#include  "dog_hb_rex.h"
#include  "machstask.h"
#include  "macdlhs.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#endif /* FEATURE_SEGMENT_LOADING */
/*=============================================================================================

								DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

=============================================================================================*/
/* MAC_HS DL watchdog report timer block  */
rex_timer_type  mac_hs_dl_dog_rpt_timer_blk;

#ifdef FEATURE_WCDMA_L2_WDOG_DYNAMIC   /* Dynamic Dog Interface */
  #undef  DOG_WCDMA_MAC_HS_RPT
  #define DOG_WCDMA_MAC_HS_RPT wcdma_mac_hs_dog_rpt_var
  dog_report_type   wcdma_mac_hs_dog_rpt_var           = 0;      /* Initial Safety */
#endif   /* FEATURE_WCDMA_L2_WDOG_DYNAMIC  */

/*--------------------------------------------------------------------------
 *  Mutex to protect the header read ISR and 
 *  MAC DL recfg flushing the partial/complete PDUs 
 ---------------------------------------------------------------------------*/
extern rex_crit_sect_type  mac_ehs_recfg_mutex;

#ifdef FEATURE_SEGMENT_LOADING
extern void wcdma_task_null (void);
#endif /* FEATURE_SEGMENT_LOADING */
/*=============================================================================================

FUNCTION  wcdma_mac_hs_dl_wait

DESCRIPTION
	This function waits for any signal in a specified signal mask to be set.
	If this procedure detects that any signal in the specified signal mask is
	set, it will return the entire mask of signals set in the task's tcb.

DEPENDENCIES
	None.

RETURN VALUE
	Mask of signals returned by the rex_wait call.

SIDE EFFECTS
	None.

=============================================================================================*/
uint32  wcdma_mac_hs_dl_wait
	 (
	 /* signals to wait on */
	 uint32  wait_sigs
	 )
{
	 /* signals returned by rex_wait call */
	 uint32  sigs;

	 /* Wait for requested signals */
	 while (((sigs = rex_get_sigs( rex_self()) ) & wait_sigs) == 0 )
	 {

			(void)rex_wait(wait_sigs);

	 }
	 return( sigs );

} /* mac_hs_dl_wait */

/*=============================================================================================

FUNCTION  wcdma_mac_hs_dl_task

DESCRIPTION
	This task performs MAC HS DL processing.

DEPENDENCIES
	None.

RETURN VALUE
	None.

SIDE EFFECTS
	None.

=============================================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
void wcdma_mac_hs_dl_task(dword dummy) __attribute__((section(".task_text")));
#endif /* FEATURE_SEGMENT_LOADING */
void  wcdma_mac_hs_dl_task
	 (
	 dword dummy
	 /* Required for REX, ignore */
	 /*lint -esym(715,dummy)
	 ** Have lint not complain about the ignored parameter 'dummy' which is
	 ** specified to make this routine match the template for rex_def_task().
	 */
	 )
{
	 uint32    sigs, timer_sig, mask = 0x1;

	 uint8 i;

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *pi_wcdma_segment = NULL;

  pi_wcdma_segment = get_wcdma_interface();
  if(pi_wcdma_segment == NULL)
  {
    wcdma_task_null ();
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */
 
#ifdef FEATURE_MODEM_RCINIT 
        /* Handshake with the RC Init */
        /* Wait for the TASK_START_SIG from RC Init */
        rcinit_handshake_startup();
#else
         /* Waits for start signal from TMC */
         /* Sets ACK signal when receives the task start signal */
         tmc_task_start();
#endif

#ifndef FEATURE_WCDMA_L2_WDOG_DYNAMIC

	 /* Setup watchdog timer */
	 rex_def_timer(&mac_hs_dl_dog_rpt_timer_blk, wcdma_mac_hs_dl_get_tcb(),
			MAC_HS_DL_DOG_RPT_TIMER_SIG);

	 (void)rex_set_timer(&mac_hs_dl_dog_rpt_timer_blk, DOG_MAC_HS_DL_RPT_TIME);
#else
	    /*Register with DOG, get the Task ID*/
	 wcdma_mac_hs_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)MAC_HS_DL_DOG_RPT_TIMER_SIG);
	
#endif

	 /* Initilize the parameters for DL mac */
	 mac_hs_dl_init();

	 /* Task loop */
	 while (1)
	 {
			/* Wait for MAC HS DL signal to be set */
			sigs = wcdma_mac_hs_dl_wait(
																 MAC_HS_DL_DOG_RPT_TIMER_SIG |
																 MAC_HS_DL_DATA_IND_SIG      |
																 MAC_HS_QUEUE_1_TIMER_SIG    |
																 MAC_HS_QUEUE_2_TIMER_SIG    |
																 MAC_HS_QUEUE_3_TIMER_SIG    |
																 MAC_HS_QUEUE_4_TIMER_SIG    |
																 MAC_HS_QUEUE_5_TIMER_SIG    |
																 MAC_HS_QUEUE_6_TIMER_SIG    |
																 MAC_HS_QUEUE_7_TIMER_SIG    |
																 MAC_HS_QUEUE_8_TIMER_SIG    |
																 MAC_HS_DL_CONFIG_REQ_SIG
#ifdef FEATURE_MAC_HS_QXDM_LOGGING
																 |
																 MAC_HS_DL_STATUS_LOG_TIMER_SIG
#endif
#ifdef FEATURE_WCDMA_HS_FACH

																 |MAC_HS_RESET_BASE_TIMER_SIG
#endif
																 );

			/* MAC HS DL Command Queue Signal is set */
			if (sigs & MAC_HS_DL_DOG_RPT_TIMER_SIG)
			{
				 /* Clear DL watchdog report timer sig */
				 (void)rex_clr_sigs (rex_self(), MAC_HS_DL_DOG_RPT_TIMER_SIG);
#ifndef FEATURE_WCDMA_L2_WDOG_DYNAMIC
				 /* Kick watchdog */
				 dog_report(DOG_MAC_HS_DL_RPT);

				 /* Set watchdog report timer for next report  */
				 (void)rex_set_timer(&mac_hs_dl_dog_rpt_timer_blk, DOG_MAC_HS_DL_RPT_TIME);
#else
                          dog_hb_report(wcdma_mac_hs_dog_rpt_var);

#endif

			}

			/* Received a signal to handle MAC HS config */
			if (sigs & MAC_HS_DL_CONFIG_REQ_SIG)
			{
				 (void)rex_clr_sigs(rex_self(), MAC_HS_DL_CONFIG_REQ_SIG);

				 mac_hs_dl_config_req_handler();

			}

			/* Received a signal to handle MAC HS data */
			if (sigs & MAC_HS_DL_DATA_IND_SIG)
			{
				 (void)rex_clr_sigs (rex_self(), MAC_HS_DL_DATA_IND_SIG);

				 mac_hs_dl_data_handler();
			}

#ifdef FEATURE_WCDMA_HS_FACH

			if (sigs & MAC_HS_RESET_BASE_TIMER_SIG)
			{
				 (void)rex_clr_sigs (rex_self(), MAC_HS_RESET_BASE_TIMER_SIG);
				 mac_hs_reset_base_timer_handler();
			}

#endif

			sigs = rex_get_sigs(rex_self());

			/* Received signal(s) indicating that the queue timer(s) have expired */
			for (i = 0; i < MAC_HS_DL_MAX_QUEUES; i++)
			{
				 timer_sig = sigs & (mask << (2+i));

				 if (timer_sig)
				 {
						(void)rex_clr_sigs (rex_self(), timer_sig);
						mac_hs_dl_timer_expiry_handler( i );
				 }
			}

#ifdef FEATURE_MAC_HS_QXDM_LOGGING
			if (sigs & MAC_HS_DL_STATUS_LOG_TIMER_SIG)
			{
				 (void)rex_clr_sigs (rex_self(), MAC_HS_DL_STATUS_LOG_TIMER_SIG);
				 mac_hs_dl_status_log_timer_expiry_handler();
			}
#endif

	 } /* while loop */
#ifdef FEATURE_DOG
#ifdef FEATURE_WCDMA_L2_WDOG_DYNAMIC
	 dog_hb_deregister(wcdma_mac_hs_dog_rpt_var);
#endif
#endif
}/* mac_hs_dl_task */
