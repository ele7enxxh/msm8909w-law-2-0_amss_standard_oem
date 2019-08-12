
/*!
  @file
  cxm_task.c

  @brief
  This file implements the thread/task level of the CoEx task.

  The main-loop of the task is cxm_task() .

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
11/19/13   btl     Handle signals for antenna sharing
06/10/13   btl     Use task signal instead of APC for COEX pwr limit timer
03/12/13   ckk     Update WDOG support to use HeartBeat feature
09/20/12   btl     Add CxM UART RX support
06/26/12   btl     Wdog support
05/21/12   ckk     Initialize task's msgr i/f after it starts
04/08/12   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include <qmi_csi.h>
#include <dog_hb_rex.h>
#include <rcinit.h>
#include <rcevt_rex.h>        /* RCEVT Signal Delivery by REX */
#include <mqcsi_conn_mgr.h>

#include "cxm_utils.h"
#include "cxm_msgr.h"
#include "coex_interface.h"
#include "coex_algos.h"
#include "cxm_trace.h"
#include "cxm_wcn_tx.h"

#include <wci2_core.h>
#include <wci2_uart.h>
#include "coex_qmb.h"
#include "coex_algos_v2.h"
#include "cxm_antenna.h"
#include "cxm_diag.h"

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#ifdef FEATURE_WDOG_DYNAMIC
STATIC dog_report_type cxm_wdog_id = 0; /*!< handle for WDOG reporting */
#endif /* FEATURE_WDOG_DYNAMIC */

/*handler to be called when CXM_OEM_SIG is received*/
static coex_oem_handler_type coex_oem_handler = NULL;

/*behavior of the system*/
cxm_sys_bhvr_t coex_sys_behavior_mask;

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  cxm_register_oem_handler

=============================================================================*/
/*!
    @brief
    Registers an OEM handler that will be called when CXM_OEM_SIG is received
 
    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type cxm_register_oem_handler(coex_oem_handler_type oem_handler)
{
  errno_enum_type estatus = E_SUCCESS;

  /*-----------------------------------------------------------------------*/
  
  if ( oem_handler != NULL )
  {
    coex_oem_handler = oem_handler;
  }
  else
  {
    estatus = E_FAILURE;  
  }
  
  return estatus;
}

/*=============================================================================

  FUNCTION:  cxm_task_init

=============================================================================*/
/*!
    @brief
    Calls alls init functions needed for CXM task
 
    @return
    None
*/
/*===========================================================================*/
void cxm_task_init(void)
{
  errno_enum_type estatus;
  /* Setup task stop signal */
  RCEVT_SIGEX_SIGREX sig;
  RCINIT_GROUP my_group;
  

  /*-----------------------------------------------------------------------*/

  /* Initialize CXM's internal trace logging setup */
  cxm_trace_init();

  /* Initialize CXM's MSGR client & associated data */
  estatus = cxm_msgr_init();
  CXM_ASSERT( E_SUCCESS == estatus );

  /* Initialize COEX */
  coex_service_init();

  /* Initialize timer group */
  cxm_timer_group_init();

  /* initialize data and algorithms */
  coex_algos_init();

  /* Initialize the DIAG i/f */
  cxm_diag_init();  

  #ifdef FEATURE_WDOG_DYNAMIC
  /* Initialize CXM Watchdog (should be at the end of cxm_task_init()
      - Register using WDOG's Heartbeat APIs (no need of task timers) */
  cxm_wdog_id = dog_hb_register_rex( CXM_WDOG_SIG );
  #endif /* FEATURE_WDOG_DYNAMIC */

  /* Setup the TASK STOP signal with rcinit... */
  sig.signal = rex_self();      
  sig.mask = CXM_TASK_STOP_SIG; 
  my_group = rcinit_lookup_group_rextask(sig.signal);
  rcinit_register_term_group(my_group, RCEVT_SIGEX_TYPE_SIGREX, &sig);

  CXM_TRACE( CXM_TRC_TSK_INIT, 0, 0, 0, CXM_TRC_AND_PKT_EN, CXM_LOG_STATE,
             CXM_LOG_GLOBAL_POLICY_PARAMS, CXM_LOG_BAND_FILTER_INFO,
             CXM_LOG_PARAMS_NO_TABLES, CXM_LOG_PARAMS_PORT_TABLE,
             CXM_LOG_PARAMS_VICTIM_TBL );
  /*-----------------------------------------------------------------------*/

  return;
}

/*=============================================================================

  FUNCTION:  cxm_task_deinit

=============================================================================*/
/*!
    @brief
    Tears down the CXM task

    @return
    None
*/
/*===========================================================================*/
STATIC void cxm_task_deinit(void)
{
  errno_enum_type estatus;

  /*-----------------------------------------------------------------------*/

  CXM_MSG_0( HIGH, "Processing TASK STOP request");
  #ifdef FEATURE_WDOG_DYNAMIC
  /* Deinitialize CXM Watchdog (should be at the beginning of cxm_task_deinit()
      - Deregister CXM from Watchdog's task monitoring */
  dog_hb_deregister( cxm_wdog_id );
  #endif /* FEATURE_WDOG_DYNAMIC */
  
  /* shutdown tx sticky bit service */
  cxm_wcn_tx_deinit();

  /* deinitialize  & clear timers */
  coex_algos_deinit();
  
  /* De-initialize CXM's MSGR client & associated data */
  estatus = cxm_msgr_deinit();
  CXM_ASSERT( E_SUCCESS == estatus );

  /* Deinitialize COEX service */
  coex_service_deinit();
  CXM_MSG_0( HIGH, "TASK STOP request complete");
  CXM_TRACE( CXM_TRC_TSK_DEINIT, 0, 0, 0, CXM_TRC_AND_PKT_EN, CXM_LOG_STATE,
             CXM_LOG_GLOBAL_POLICY_PARAMS, CXM_LOG_BAND_FILTER_INFO,
             CXM_LOG_PARAMS_NO_TABLES, CXM_LOG_PARAMS_PORT_TABLE,
             CXM_LOG_PARAMS_VICTIM_TBL );

  /*-----------------------------------------------------------------------*/

  return;

}

/*=============================================================================

  FUNCTION:  cxm_clr_sigs

=============================================================================*/
/*!
    @brief
    Clears the required signals

    @return
    None
*/
/*===========================================================================*/
void cxm_clr_sigs ( cxm_signal_mask_t sigs )
{
  /* Make sure there's at least one signal needs clearing. */
  if(sigs)
  {
    /* Clear the signal, (REX locks interrupts while clearing.) */
    (void) rex_clr_sigs( &cxm_tcb, sigs );
  }
} /* cxm_clr_sigs */

/*=============================================================================

  FUNCTION:  cxm_task

=============================================================================*/
/*!
    @brief
    REX main function used for CXM task
*/
/*===========================================================================*/
void cxm_task
(
  dword dummy
)
{
  static cxm_signal_mask_t sig_mask_to_wait_for;
  cxm_signal_result_t      curr_sigs_set = 0;
  qmi_csi_error            retval = QMI_CSI_NO_ERR;

  /*-----------------------------------------------------------------------*/

  (void)dummy;

  /* Synchronize thread at startup with rcinit */
  rcinit_handshake_startup();

  /* Initialize CXM task functionalities */
  cxm_task_init();

  /* Set REX signal mask to wait for */
  sig_mask_to_wait_for = CXM_SIGS;

  CXM_MSG_0( HIGH, "task active" );

  /* CXM task's main loop */
  while (1)
  {
    curr_sigs_set = rex_wait( sig_mask_to_wait_for );
    cxm_clr_sigs(curr_sigs_set & sig_mask_to_wait_for);

    if ( curr_sigs_set & CXM_WDOG_SIG )
    {
      #ifdef FEATURE_WDOG_DYNAMIC
      /* Pet the dog */
      dog_hb_report( cxm_wdog_id );
      #endif /* FEATURE_WDOG_DYNAMIC */
    }

    /* Process COEX signals */
    if ( curr_sigs_set & CXM_COEX_QMI_SIG )
    {
      /* Process the signal */
      retval = mqcsi_conn_mgr_handle_event ( MQCSI_COEX_SERVICE_ID );

      /* Ensure all is well in the CONNECTION layer */
      CXM_ASSERT( QMI_CSI_NO_ERR == retval );
    }

    if ( curr_sigs_set & CXM_MSGR_Q_SIG )
    {
      /* Process the MSGR messages received */
      cxm_msgr_process_sig();

      /* NOTE: return values are ASSERTed for appropriately within cxm_msgr */
    }

    /* COEX has a message from QMB to process */
    if ( curr_sigs_set & CXM_COEX_QMB_SIG )
    {
      coex_qmb_process_sig();
    }

    /* Process received RX direct read data signal */
    if ( curr_sigs_set & CXM_UART_RX_DIRECT_SIG )
    {
      coex_algos_process_wci2_rx();
    }

    /* Process qmb timeout timer */
    if ( curr_sigs_set & CXM_COEX_QMB_TIMER_SIG )
    {
      coex_qmb_msg_timer_handler();
    }

    /* Process qmb nv exchange timeout timer */
    if ( curr_sigs_set & CXM_COEX_QMB_NV_TIMER_SIG )
    {
      coex_qmb_nv_timer_handler();
    }

    if ( curr_sigs_set & CXM_DIAG_SEND_SIG )
    {
      /* Process the signal */
      cxm_diag_send_arb_msgs_timer_handler();
    }

    if ( curr_sigs_set & CXM_COEX_PWR_LMT_WDOG_SIG )
    {
      /* COEX power limit exceeded time limit. process the signal */
      coex_pwr_lmt_wdog_timer_handler();
    }

    if (curr_sigs_set & CXM_OEM_SIG)
    {
       if (coex_oem_handler != NULL)
       {
         coex_oem_handler();
       }
    }

    if ( curr_sigs_set & CXM_COEX_T7_CONN_ST_TMR_SIG )
    {
      /* handle type 7[conn_state] filtering signal - transition passed filter */
      coex_wci2_type7_timer_handler( COEX_WCI2_T7_CONN_ST_EVNT_MSK );
    }

    if ( curr_sigs_set & CXM_COEX_T7_PWR_ST_TMR_SIG )
    {
      /* handle type 7[pwr_state] filtering signal - transition passed filter */
      coex_wci2_type7_timer_handler( COEX_WCI2_T7_PWR_ST_EVNT_MSK );
    }

    if( curr_sigs_set & CXM_ANTENNA_GRANT_SIG )
    {
      cxm_antenna_handle_grant();
    }

    if( curr_sigs_set & CXM_ANTENNA_RELEASE_SIG )
    {
      cxm_antenna_handle_release();
    }

    if( curr_sigs_set & CXM_LOG_PKT_FLUSH_SIG )
    {
      /* flush log packets */
      if( log_status( LOG_CXM_LOW_PRIO_C ) )
      {
        cxm_send_log( LOG_CXM_LOW_PRIO_C ); 
      }
      if( log_status( LOG_CXM_MED_PRIO_C ) )
      {
        cxm_send_log( LOG_CXM_MED_PRIO_C ); 
      }
      if( log_status( LOG_CXM_HIGH_PRIO_C ) )
      {
        cxm_send_log( LOG_CXM_HIGH_PRIO_C ); 
      }
      if( log_status( LOG_CXM_SMEM_DATA_C ) )
      {
        cxm_send_log( LOG_CXM_SMEM_DATA_C ); 
      }
    }

    if ( curr_sigs_set & CXM_TASK_STOP_SIG )
    {
      /*  Deinitialize everything... */
      cxm_task_deinit();

      /* ACK rcinit that the task deinit is done.. */
      rcinit_handshake_term();
      
      /* Done with the loop */
      break; 
    }
  }

  /*-----------------------------------------------------------------------*/

  return;
}
