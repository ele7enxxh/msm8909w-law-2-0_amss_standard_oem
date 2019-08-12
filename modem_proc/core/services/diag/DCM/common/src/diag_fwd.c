/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Fwd Task 

General Description
This routines in this file pertain to the task called diag_fwd_task
that on the master processor process traffic from slave processors.
This is a more robust design and will help diag_task be more efficient.
  
Copyright (c) 2010-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_fwd.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/15/13   sr      Moved to new Dog heartbeat implementation  
09/14/10   is      New control channel and central routing features 
06/16/10   VG      Added DIAG_FWD_RPT_TIMER_SIG to osal_wait 
05/26/10   JV      Featurized dog functionality for WM and aDSP
04/06/10   JV      Created
===========================================================================*/
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

#include "diag_fwd_v.h"
#include "diagi_v.h"
#include "diagcomm_v.h"
#include "diagcomm_smd.h"
#include "diag_v.h"
#include "diagcomm_io_smdl.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "dog.h"
#include "assert.h"
#endif /* (!DIAG_QDSP6_APPS_PROC) */
#include "diagcomm_cmd.h"
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern osal_tcb_t diag_fwd_task_tcb;

#if defined (DIAG_DOG_SUPPORT)

dog_report_type diag_fwd_dog_rpt_var = 0;
uint32 diag_fwd_dog_rpt_time_var = 0xffff;

osal_timer_t diag_fwd_rpt_timer;
#endif

void diag_fwd_task_start(uint32 params)
{
	#if defined (DIAG_DOG_SUPPORT)
diag_fwd_dog_rpt_var = osal_dog_hb_register((osal_sigs_t)DIAG_RPT_SIG);
#endif

/* Start the dog timer */
	diag_fwd_kick_watchdog ();
	diag_fwd_handle_sigs();
}

void diag_fwd_kick_watchdog(void)
{
#if defined (DIAG_DOG_SUPPORT)
osal_dog_hb_report(diag_fwd_dog_rpt_var);
#endif
}

void diag_fwd_handle_sigs()
{
	osal_sigs_t sigs = 0;
	osal_sigs_t return_sigs = 0;

	while (1)
	{
		sigs = osal_thread_wait (&diag_fwd_task_tcb, (DIAG_TX_SLAVE_SIG | DIAG_FWD_RPT_SIG));

		/* Pet the dog before doing any work. */
		diag_fwd_kick_watchdog ();
		if (sigs & DIAG_SMDL_CMD_READ_SIG)
		{
			(void)osal_reset_sigs(&diag_fwd_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
			diagcomm_smdl_buffer_read(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1]);
			diagcomm_smdl_buffer_read(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1]);

		} 
		else if (sigs & DIAG_SMDL_READ_SIG)
		{
			(void)osal_reset_sigs(&diag_fwd_task_tcb, DIAG_SMDL_READ_SIG, &return_sigs);
			int i, j;
			for (i = 0; i < NUM_SMD_PORT_TYPES; i++)
			{
				for (j = 0; j < NUM_SMD_PORTS; j++)
				{
					diagcomm_smdl_buffer_read(&diagcomm_io_conn[i][j]);
					diagcomm_smdl_buffer_read(&diagcomm_io_ctrl_conn[i][j]);
				}
			}

		}

		else if ( sigs & DIAG_RX_RSP_TX_SIG )
		{
		  (void)osal_reset_sigs( &diag_fwd_task_tcb, DIAG_RX_RSP_TX_SIG, &return_sigs );
		  
		  if( diagcomm_cmd_get_rx_wmq_cnt(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1) )
		    diagcomm_cmd_process_slave_tx(DIAGCOMM_PORT_1);
			   
		  if( diagcomm_cmd_get_rx_wmq_cnt(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2) )
		    diagcomm_cmd_process_slave_tx(DIAGCOMM_PORT_2);	   
		}

		if (sigs & DIAG_TX_SLAVE_SIG)
		{
			(void) osal_reset_sigs (&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);

			if(diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1))
			   diagcomm_smd_process_slave_tx (DIAGCOMM_PORT_1);
  
			if(diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_2))
			   diagcomm_smd_process_slave_tx(DIAGCOMM_PORT_2);
			 
		}
		if( diagcomm_status(DIAGCOMM_PORT_1) && 
		   (diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_1) || diagcomm_smd_get_rx_wmq_cnt(DIAGCOMM_PORT_2))
		  )
		{
			osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
		}
	}
}
#endif