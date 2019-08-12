
/*!
  @file
  lmtsmgr_task.c

  @brief
  This file implements the thread/task level of the Limits Manager task.

  The main-loop of the task is lmtsmgr_task() .

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/src/lmtsmgr_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/09/14   jm      Remove deprecated TRM API from limitsmgr
06/23/14   sg      desense indication mechanism
01/13/14   sk      Changes to release TRM resource for CM client
11/21/13   jm      Adding DDR management support
05/16/13   rj      Remove registering for STOP_SIG with RCInit
03/20/13   sk      Moved dog timer to new dog_hb mechanism 
03/05/13   rj      Clr wdog timer while task deinit 
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
#include <timer.h>

#include "mqcsi_conn_mgr.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_msgr.h"
#include "rcinit.h"
#include "rcevt_rex.h"        /* RCEVT Signal Delivery by REX */
#include "lmtsmgr_i.h"             
#include "trm.h"          
#include "wwan_coex.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif        
/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/
#ifdef FEATURE_WDOG_DYNAMIC
static dog_report_type lmtsmgr_wdog_id          = 0; /*!< handle for WDOG reporting */
#endif /* FEATURE_WDOG_DYNAMIC */

/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*===========================================================================

  FUNCTION:  lmtsmgr_wdog_setup

===========================================================================*/
/*!
    @brief
    Setting up WATCHDOG for LMTSMGR.

    @detail
    Registers LMTSMGR task with WATCHDOG services.

    Acquires WDOG reporting time and sets up timer for this task

    @return
    None.
*/
/*=========================================================================*/
void lmtsmgr_wdog_setup(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC
  /* Register LMTSMGR task with WATCHDOG for monitoring */
  lmtsmgr_wdog_id = dog_hb_register_rex((rex_sigs_type)LMTSMGR_WDOG_SIG);

  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}

/*===========================================================================

  FUNCTION:  lmtsmgr_wdog_teardown

===========================================================================*/
/*!
    @brief
    Teardown LMTSMGR WATCHDOG.

    @detail
    Deregisters CXM from WATCHDOG services.

    @return
    None.
*/
/*=========================================================================*/
void lmtsmgr_wdog_teardown(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC

  /* Deregister LMTSMGR task from WDOG's task monitoring */
  dog_hb_deregister( lmtsmgr_wdog_id );
  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}

/*===========================================================================

  FUNCTION:  lmtsmgr_wdog_report

===========================================================================*/
/*!
    @brief
    Reports to WATCHDOG services.

    @detail
    "Pets the WDOG" - reports to the WATCHDOG services.

    Also resets the timer for the next cycle.

    @return
    None.
*/
/*=========================================================================*/
void lmtsmgr_wdog_report(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC

  /* "pet the dog" */
  dog_hb_report( lmtsmgr_wdog_id );
  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}


/*=============================================================================

  FUNCTION:  lmtsmgr_task_init

=============================================================================*/
/*!
    @brief
    Calls alls init functions needed for LMTSMGR task
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_task_init(void)
{
  errno_enum_type estatus;
  /* Setup task stop signal */

  /*-----------------------------------------------------------------------*/

  /* Initialize LMTSMGR's MSGR client & associated data */
  estatus = lmtsmgr_msgr_init();
  LMTSMGR_ASSERT( E_SUCCESS == estatus );

  /* Initialize LMTSMGR. */
  lmtsmgr_init();

  /* Initialize LMTSMGR Watchdog (should be at the end of lmtsmgr_task_init() */
  lmtsmgr_wdog_setup();

  /*-----------------------------------------------------------------------*/

  return;
}

/*=============================================================================

  FUNCTION:  lmtsmgr_task_deinit

=============================================================================*/
/*!
    @brief
    Tears down the LMTSMGR task

    @return
    None
*/
/*===========================================================================*/
static void lmtsmgr_task_deinit(void)
{
  errno_enum_type estatus;

  /*-----------------------------------------------------------------------*/

  LMTSMGR_MSG_0( HIGH, "LMTSMGR: task deinit" );

  /* Deinitialize Lmtsmgr core */
  lmtsmgr_core_deinit();

  /* Deinitialize LMTSMGR Watchdog */
  lmtsmgr_wdog_teardown();

  /* De-initialize LMTSMGR's MSGR client & associated data */
  estatus = lmtsmgr_msgr_deinit();
  LMTSMGR_ASSERT( E_SUCCESS == estatus );

  /*-----------------------------------------------------------------------*/

  return;

}

/*===========================================================================

FUNCTION LMTSMGR_SET_SIGS

DESCRIPTION    This function sets the signals for Lmtsmgr Manager task.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void lmtsmgr_set_sigs ( rex_sigs_type sigs )
{

  /* Make sure there's at least on signal that need to be set. */
  if(sigs)
  {
    /* Enter atomic section. Disable interrupts. */
    LMTSMGR_ENTER_ATOMIC_SECT();

    /* Set the signal(s). */
    (void) rex_set_sigs( &lmtsmgr_tcb, sigs );

    /* Leave atomic section. Enable interrupts. */
    LMTSMGR_LEAVE_ATOMIC_SECT();
  }
} /* tcxomgr_set_sigs */

/*=============================================================================

  FUNCTION:  lmtsmgr_clr_sigs

=============================================================================*/
/*!
    @brief
    Clears the required signals

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_clr_sigs ( lmtsmgr_signal_mask_t sigs )
{
  /* Make sure there's at least one signal needs clearing. */
  if(sigs)
  {
    /* Clear the signal, (REX locks interrupts while clearing.) */
    (void) rex_clr_sigs( &lmtsmgr_tcb, sigs );
  }
} /* lmtsmgr_clr_sigs */


/*=============================================================================

  FUNCTION:  lmtsmgr_task_check_dog

=============================================================================*/
/*!
    @brief
    Checks is wdog signal is set and pets it and clears the wdog signal

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_task_check_dog (void)
{
  lmtsmgr_signal_result_t sigs = 0;
  sigs = rex_get_sigs(&lmtsmgr_tcb);
  
  if ( sigs & LMTSMGR_WDOG_SIG )
  {
    /* Clear the WDOG signal */
    lmtsmgr_clr_sigs(LMTSMGR_WDOG_SIG);
	 
    /* Pet the dog */
    lmtsmgr_wdog_report();
	
	LMTSMGR_MSG_1(MED, "Petting the dog in the middle of dequeue %d", sigs );
  } 
}
/*=============================================================================

  FUNCTION:  lmtsmgr_task

=============================================================================*/
/*!
    @brief
    REX main function used for LMTSMGR task
*/
/*===========================================================================*/
void lmtsmgr_task
(
  dword dummy
)
{
  static lmtsmgr_signal_mask_t sig_mask_to_wait_for;
  lmtsmgr_signal_result_t      curr_sigs_set = 0;
  qmi_csi_error                retval = QMI_CSI_NO_ERR;
  trm_release_chain_input_info trm_input;

  /*-----------------------------------------------------------------------*/

  (void)dummy;
  lmtsmgr.task_status = LMTSMGR_INITIALIZING;

  /* Initialize LMTSMGR task functionalities */
  lmtsmgr_task_init();

  /* Set REX signal mask to wait for */
  sig_mask_to_wait_for = LMTSMGR_SIGS;

  #ifdef TEST_FRAMEWORK 
  #error code not present
#endif
  
  /* Synchronize thread at startup with rcinit */
  rcinit_handshake_startup();

  /* Register for TRM callbacks for Tech state update */
  trm_register_lmtsmgr_client(lmtsmgr_trm_state_update_callback);

  lmtsmgr.task_status = LMTSMGR_INITIALIZED;

  wwcoex_set_task_state(TRUE);

  LMTSMGR_MSG_0( HIGH, "LMTSMGR: task active" );

  /* LMTSMGR task's main loop */
  while (1)
  {
    curr_sigs_set = rex_wait( sig_mask_to_wait_for );
    lmtsmgr_clr_sigs(curr_sigs_set & sig_mask_to_wait_for);

    if ( curr_sigs_set & LMTSMGR_WDOG_SIG )
    {
      /* Pet the dog */
      lmtsmgr_wdog_report();
    }

    /* Process SAR DSI signal */
    if ( curr_sigs_set & LMTSMGR_SAR_QMI_SIG )
    {
      /* Process the signal */
      retval = mqcsi_conn_mgr_handle_event (MQCSI_DSI_SERVICE_ID);

      /* Ensure all is well in the CONNECTION layer */
      LMTSMGR_ASSERT( QMI_CSI_NO_ERR == retval );
    }

    if ( curr_sigs_set & LMTSMGR_MSGR_Q_SIG )
    {
      /* Process the MSGR messages received */
      lmtsmgr_msgr_process_sig();

      /* NOTE: return values are ASSERTed for appropriately within lmtsmgr_msgr */
    }

    if( curr_sigs_set & LMTSMGR_TRM_REL_SIG )
    {
     /* process the signal, release the chain */
      trm_input.client = TRM_CM;
      trm_input.tag = 0;
      trm_release_chain(&trm_input);
    }

    if ( (curr_sigs_set & LMTSMGR_TASK_STOP_SIG) 
         #ifdef TEST_FRAMEWORK
          #error code not present
#endif
        )
    {
      /*  Deinitialize everything... */
      lmtsmgr_task_deinit();

      /* Done with the loop */
      break; 
    }

    /* Process SAR Call-Back function signal */
    if ( curr_sigs_set & LMTSMGR_SAR_CALL_BACK_SIG )
    {
      /* Process the signal */
      lmtsmgr_sp_callback_process_input();

    }

    /* Process desense timer expiry signal */
    if ( curr_sigs_set & LMTSMGR_DESENSE_CALC_SIG )
    {
      /* Process the signal */
      wwcoex_process_desense_timeout();

    }

  }

  /*-----------------------------------------------------------------------*/

  return;
}
