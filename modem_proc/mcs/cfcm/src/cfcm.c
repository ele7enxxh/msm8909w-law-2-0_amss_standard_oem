
/*!
  @file 
  cfcm.c

  @brief
  This file contains the centralized flow-control manager(CFCM) task processing

  @author
  rohitj
  
*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/27/14   rj      Adding QTF support for CFCM
08/11/14   rj      Adding support for QSH in CFCM
07/28/14   rj      CFCM task active set TRUE before calling rcevt_signal_name()
07/03/14   rj      BUS BW changes
04/07/14   rj      initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <dog_hb_rex.h>

#include "rcevt_rex.h"        /* RCEVT Signal Delivery by REX */
#include <msgr.h>
#include <amssassert.h>
#include <comdef.h>
#include <mqueue.h>
#include <appmgr.h>
#include <rcevt.h>
#include "timetick.h"

#include "cfcm_cfg.h"
#include "cfcm_msg.h"
#include "cfcm.h"
#include "cfcmi.h"
#include "cfcm_client.h"
#include "cfcm_monitor.h"
#include "cfcm_cpu_monitor.h"
#include "cfcm_msgr.h"
#include "cfcm_qsh.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief The structure containing the CFCM global data
*/
cfcm_s cfcm;
STATIC cfcm_s* const cfcm_ptr = &cfcm;

#ifdef FEATURE_WDOG_DYNAMIC
static dog_report_type cfcm_wdog_id          = 0; /*!< handle for WDOG reporting */
#endif /* FEATURE_WDOG_DYNAMIC */


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*============================================================================

                            TASK FUNCTION DECLARATIONS

============================================================================*/

/*=======================================================================
FUNCTION CFCM_WDOG_SETUP

DESCRIPTION
  Setting up WATCHDOG for CFCM. Registers CFCM task with WATCHDOG services.
  Acquires WDOG reporting time and sets up timer for this task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_wdog_setup(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC
  /* Register CFCM task with WATCHDOG for monitoring */
  cfcm_wdog_id = dog_hb_register_rex((rex_sigs_type)CFCM_WDOG_SIG);

  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}

/*=======================================================================
FUNCTION CFCM_WDOG_TEARDOWN

DESCRIPTION
  Teardown CFCM WATCHDOG. Deregisters CFCM from WATCHDOG services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_wdog_teardown(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC

  /* Deregister CFCM task from WDOG's task monitoring */
  dog_hb_deregister( cfcm_wdog_id );
  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}

/*=======================================================================
FUNCTION CFCM_WDOG_REPORT

DESCRIPTION
  Reports to WATCHDOG services. "Pets the WDOG" - reports to the WATCHDOG services.
  Also resets the timer for the next cycle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_wdog_report(void)
{
  #ifdef FEATURE_WDOG_DYNAMIC

  /* "pet the dog" */
  dog_hb_report( cfcm_wdog_id );
  #endif /* FEATURE_WDOG_DYNAMIC */

  return;
}




/*===========================================================================

  FUNCTION:  cfcm_task_deinit

===========================================================================*/
/*!
  @brief
  Termination for CFCM task

  @return
*/
/*=========================================================================*/
static void cfcm_task_deinit( void )
{ 
  errno_enum_type estatus;

  /*-----------------------------------------------------------------------*/

  CFCM_MSG_0( HIGH, "CFCM: task deinit" );

  /* Deinitialize CFCM Monitor related functionalities */
  cfcm_monitor_deinit();

  /* Deinitialize CFCM Watchdog */
  cfcm_wdog_teardown();

  /* De-initialize CFCM's MSGR client & associated data */
  estatus = cfcm_msgr_deinit();
  CFCM_ASSERT( E_SUCCESS == estatus );

  /*-----------------------------------------------------------------------*/
  
#ifdef TEST_FRAMEWORK
#error code not present
#endif

  cfcm_ptr->active = FALSE;

} /* cfcm_task_deinit */

/*===========================================================================

FUNCTION CFCM_SET_SIGS

DESCRIPTION
  This function sets the signals for CFCM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cfcm_set_sigs ( cfcm_signal_mask_t sigs )
{

  /* Make sure there's at least on signal that need to be set. */
  if(sigs)
  {
    /* Enter atomic section. Disable interrupts. */
    CFCM_ENTER_ATOMIC_SECT();

    /* Set the signal(s). */
    (void) rex_set_sigs( &cfcm_tcb, sigs );

    /* Leave atomic section. Enable interrupts. */
    CFCM_LEAVE_ATOMIC_SECT();
  }
} /* cfcm_set_sigs */

/*===========================================================================

FUNCTION CFCM_CLR_SIGS

DESCRIPTION
  This function Clears the required signals for CFCM task.

DEPENDENCIES
  CFCM Task should be intialized

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cfcm_clr_sigs ( cfcm_signal_mask_t sigs )
{
  /* Make sure there's at least one signal needs clearing. */
  if(sigs)
  {
    /* Clear the signal, (REX locks interrupts while clearing.) */
    (void) rex_clr_sigs( &cfcm_tcb, sigs );
  }
} /* lmtsmgr_clr_sigs */


/*===========================================================================

  FUNCTION:  cfcm_task_init

===========================================================================*/
/*!
  @brief
  This function creates CFCM task with the given priority.

  @return
  the CFCM thread of type pthread_t.
*/
/*=========================================================================*/
static void cfcm_task_init ( void )
{
  errno_enum_type estatus;
#ifdef FEATURE_MODEM_RCINIT
  RCEVT_HANDLE    rc_handle;
#endif /* FEATURE_MODEM_RCINIT */
#ifdef CFCM_ON_TARGET
  DALResult       result;
#endif /* CFCM_ON_TARGET */

  CFCM_MSG_0( HIGH, "CFCM: task init" );

  /* Initialize CFCM's Configuration Data */
  cfcm_cfg_init();

  /* Initialize CFCM's Client Data */
  cfcm_client_init();

  cfcm_monitor_init();

  cfcm_cpu_monitor_init();

  /* Initialize CFCM's MSGR client & associated data */
  estatus = cfcm_msgr_init();
  CFCM_ASSERT( E_SUCCESS == estatus );

  /* Initialize CFCM Watchdog (should be at the end of cfcm_task_init() */
  cfcm_wdog_setup();

  // Set a flag to indicate we are alive..
  cfcm_ptr->active = TRUE;

  /* signal task depending on CFCM (A2) that CFCM is started */
#ifdef FEATURE_MODEM_RCINIT
  rc_handle = rcevt_signal_name("CFCM:STARTED");
  if (rc_handle == NULL)
  {
    MSG_ERROR("Received NULL rc_handle", 0, 0, 0);
  }
#endif

  /* CFCM Disable Mask */
  cfcm_ptr->cfcm_disable = cfcm_cfg_get_cfcm_disabled();

  /* CFCM Testing via Diag command Enabled */
  cfcm_ptr->diag_test_enabled = cfcm_cfg_get_diag_test_enable_mask();

#ifdef CFCM_ON_TARGET
  /* get timetick handle */
  result = DalTimetick_Attach("SystemTimer", &cfcm_ptr->timetick_handle);
  if (DAL_SUCCESS != result)
  {
    ERR_FATAL("DalTimetick_Attach to SystemTimer failed, Result=%d", result, 0, 0);
  }
#endif /* CFCM_ON_TARGET */

#ifndef FEATURE_MODEM_CFCM_DIAG_TEST
  /* This is to avoid compilation error */
  cfcm_test_diag_command_inputs (0xFF);
  cfcm_client_get_diag_test_enable_mask();
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */
} /* cfcm_task_init */

/*=======================================================================
FUNCTION CFCM_TASK

DESCRIPTION
  REX main function used for CFCM task.

DEPENDENCIES
  Should be called only once when CFCM initializes at bootup

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_task ( dword dummy )
{
  static cfcm_signal_mask_t sigs_wait;
  cfcm_signal_result_t      curr_sigs_set = 0;
  qmi_csi_error            retval = QMI_CSI_NO_ERR;

  /* Avoid unused warnings */
  (void)dummy;

  /* Enter atomic section. Disable interrupts. */
  CFCM_ENTER_ATOMIC_SECT();

  /* Initialize the Subs-Prio's critical section */
  CFCM_INIT_CRIT_SECT(cfcm.crit_sect);

  /* Leave atomic section. Enable interrupts. */
  CFCM_LEAVE_ATOMIC_SECT();

  /* Initalize CFCM task */
  cfcm_task_init();

  /* Set REX signal mask to wait for */
  sigs_wait = CFCM_SIGS;

  #ifdef TEST_FRAMEWORK 
  #error code not present
#endif
  
  /* Synchronize thread at startup with rcinit */
  rcinit_handshake_startup();

  /* register client with QSH */
  qsh_reg_client_with_cb(
    CFCM_CLT_QSH, 
    cfcm_qsh_analysis_cb,
    CFCM_QSH_VER_MAJOR,
    CFCM_QSH_VER_MINOR);

  /*-----------------------------------------------------------------------*/

  /* CFCM task's main loop */
  while (1)
  {
    curr_sigs_set = rex_wait( sigs_wait );
    cfcm_clr_sigs(curr_sigs_set & sigs_wait);

    if ( curr_sigs_set & CFCM_WDOG_SIG )
    {
      /* Pet the dog */
      cfcm_wdog_report();
    }

    if ( curr_sigs_set & CFCM_MSGR_Q_SIG )
    {
      /* Process the MSGR messages received */
      cfcm_msgr_process_sig();
    }

    if ( (curr_sigs_set & CFCM_TASK_STOP_SIG) 
      #ifdef TEST_FRAMEWORK
      #error code not present
#endif
        )
    {
      /*  Deinitialize everything... */
      cfcm_task_deinit();

      /* Done with the loop */
      break; 
    }

    /* Process CFCM QMI signal */
    if ( curr_sigs_set & CFCM_TASK_QMI_SIG )
    {
      /* Process the signal */
      retval = mqcsi_conn_mgr_handle_event (MQCSI_CFCM_SERVICE_ID);

      /* Ensure all is well in the CONNECTION layer */
      CFCM_ASSERT( QMI_CSI_NO_ERR == retval );
    }

  }

  /*-----------------------------------------------------------------------*/
} /* cfcm_task_main() */


/*===========================================================================

  FUNCTION:  cfcm_is_active

===========================================================================*/
/*!
  @brief
  Can be used to query if the CFCM task has been initialized

  @return
  TRUE if the task has been initialized.
*/
/*=========================================================================*/
EXTERN boolean cfcm_task_is_active(void)
{
  /*-----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------*/

  return cfcm_ptr->active;

} /* cfcm_is_active() */



/*=======================================================================
FUNCTION CFCM_VALIDATE_REGISTER_REQ

DESCRIPTION
  Validates a Register Req

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
========================================================================*/
static boolean cfcm_validate_register_req 
(
  cfcm_reg_req_type_s *cfcm_reg_req
)
{

  if (cfcm_reg_req == NULL)
  {
    MSG_FATAL("CFCM:: Invalid input.", 0, 0, 0);
    return FALSE;
  }

  /* If this entry is not valid no need to check further so return FALSE */
  if (!CFCM_IS_CLIENT_VALID(cfcm_reg_req->client_id))
  {
    MSG_FATAL("CFCM:: Reg Request invalid client", cfcm_reg_req->client_id, 0, 0);
    return FALSE;
  }

  /* Client's Monitor mask should be non-Zero */
  if (cfcm_reg_req->monitor_mask == 0)
  {
    CFCM_MSG_0(ERROR, "CFCM:: Reg Request Monitor Mask 0");
    return FALSE;
  }

  return TRUE;
}


/*=======================================================================
FUNCTION CFCM_CLIENT_REGISTER

DESCRIPTION
  This function is externally exposed to allow registration of the client

DEPENDENCIES
  MSGR should be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_client_register
( 
  cfcm_reg_req_type_s* client_req
)
{
  errno_enum_type        send_status;
  cfcm_reg_req_msg_type_s      reg_msg;

  /*--------------------------------------------------------------------------*/

  if (IS_CFCM_DISABLED(cfcm_ptr->cfcm_disable))
  {
    CFCM_MSG_1( ERROR, "cfcm_client_register: CFCM Disabled %d ", cfcm_ptr->cfcm_disable );
    return;
  }

  /* Check Inputs params */
  if(!cfcm_validate_register_req(client_req) || (cfcm_ptr->active != TRUE))  
  {
    CFCM_MSG_1( ERROR, "cfcm_client_register: invalid params active %d ", cfcm_ptr->active );
    return;
  }
  /*--------------------------------------------------------------------------*/
  
  reg_msg.cfcm_req.monitor_mask = client_req->monitor_mask;

  reg_msg.cfcm_req.client_id = client_req->client_id;
  reg_msg.cfcm_req.req_umid = client_req->req_umid;
  reg_msg.cfcm_req.req_cb = client_req->req_cb;

  send_status = cfcm_msgr_send_msg(&reg_msg.hdr,
                     MCS_CFCM_CLIENT_REGISTER_REQ,
                     sizeof(cfcm_reg_req_msg_type_s));

  CFCM_ASSERT(send_status == E_SUCCESS);
} /* cfcm_client_register() */


/*=======================================================================
FUNCTION CFCM_CLIENT_DEREGISTER

DESCRIPTION
  This function is externally exposed to allow deregistration of the client

DEPENDENCIES
  MSGR should be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_client_deregister
(
  cfcm_dereg_req_type_s*  client_req
)
{
  errno_enum_type        send_status;
  cfcm_dereg_req_msg_type_s    dereg_msg;
  /*--------------------------------------------------------------------------*/

  if (IS_CFCM_DISABLED(cfcm_ptr->cfcm_disable))
  {
    CFCM_MSG_1( ERROR, "cfcm_client_deregister: CFCM Disabled %d ", cfcm_ptr->cfcm_disable );
    return;
  }

  CFCM_ASSERT(client_req != NULL);
  CFCM_ASSERT(CFCM_IS_CLIENT_VALID(client_req->client_id));

  if(cfcm_ptr->active != TRUE)
  {
    CFCM_MSG_1( HIGH, "cfcm_client_deregister: CFCM active %d ", cfcm_ptr->active );
    return;
  }
  /*--------------------------------------------------------------------------*/

  dereg_msg.client.client_id = client_req->client_id;

  send_status = cfcm_msgr_send_msg(&dereg_msg.hdr,
                     MCS_CFCM_CLIENT_DEREGISTER_REQ,
                     sizeof(cfcm_dereg_req_msg_type_s));

  CFCM_ASSERT(send_status == E_SUCCESS);
} /* cfcm_client_deregister() */


/*=======================================================================
FUNCTION CFCM_CLIENT_GET_STEP_TIMER_VALS

DESCRIPTION
  This function is externally exposed for clients to get different step timer values

DEPENDENCIES
  CFCM should be initialized

RETURN VALUE
  TRUE/FALSE - Thresholds Valid or NOT
  cpu_step_timer - CPU FC step timer in msecs
  dsm_step_timer - DSM FC step timer in msecs
  thermal_step_timer - thermal step timer in unit of seconds
  bus_bw_step_timer - Bus Bandwidth step timer in msecs

SIDE EFFECTS
  None
========================================================================*/
boolean cfcm_client_get_step_timer_vals
( 
  cfcm_client_e     client_id,    /*!<  client ID, CFCM only works with known client*/
  cfcm_default_step_timer_type_s* timers
)
{
  boolean retval = FALSE;

  /*--------------------------------------------------------------------------*/

  if(!CFCM_IS_CLIENT_VALID(client_id) || (timers == NULL))
  {
    CFCM_MSG_1(ERROR, "cfcm_client_get_step_timer_vals Error client %d ", client_id);
    return retval;
  }


  /* Enter Critical Section */
  CFCM_ENTER_CRIT_SECT(cfcm.crit_sect);

  /* Check Inputs params */
  if(cfcm_ptr->active == TRUE)
  {
    retval = cfcm_cfg_get_client_step_timers(client_id, timers);
  }
  else
  {
    CFCM_MSG_1( ERROR, "cfcm_client_get_step_timer_vals: CFCM active %d ", cfcm_ptr->active );
  }

  /* Leave Critical Section */
  CFCM_LEAVE_CRIT_SECT(cfcm.crit_sect);

  return retval;
} /* cfcm_client_get_step_timer_vals() */


/*==============================================================================

  FUNCTION:  cfcm_client_get_diag_test_enable_mask

==============================================================================*/
/*!
    @brief
    this function returns CFCM testing via Diag command enabled based on the EFS/NV read

    @return
    TRUE/FALSE
*/
/*============================================================================*/
boolean cfcm_client_get_diag_test_enable_mask( void )
{
  boolean test_enable = FALSE;
  /*--------------------------------------------------------------------------*/

  /* Enter Critical Section */
  CFCM_ENTER_CRIT_SECT(cfcm.crit_sect);

  test_enable = cfcm_ptr->diag_test_enabled;

  /* Leave Critical Section */
  CFCM_LEAVE_CRIT_SECT(cfcm.crit_sect);


  return test_enable;
} /* cfcm_client_get_diag_test_enable_mask() */



/*===========================================================================

  FUNCTION:  cfcm_timetick_get_ms

===========================================================================*/
/*!
  @brief
  Returns the current timetick value in msec and stored in 32 bit.
  This number is monotonically increasing and wraps around in 1193 hours
*/
/*=========================================================================*/
cfcm_timetick_ms_t cfcm_timetick_get_ms
(
  void
)
{
  cfcm_timetick_ms_t timetick = 0;
#ifdef CFCM_ON_TARGET
  timetick = timetick_get_ms();
#endif /* CFCM_ON_TARGET */
  return timetick;
}

/*===========================================================================

  FUNCTION:  cfcm_timetick_get

===========================================================================*/
/*!
  @brief
  Returns the current timetick value in 32-bit timeticks.
*/
/*=========================================================================*/
cfcm_timetick_t cfcm_timetick_get
(
  void
)
{
  cfcm_timetick_t timetick = 0;
#ifdef CFCM_ON_TARGET
  DalTimetick_Get(cfcm_ptr->timetick_handle, &timetick);
#endif /* CFCM_ON_TARGET */
  return timetick;
}


/*===========================================================================

                                UNIT TEST
==========================================================================
 Bits    |     7     |     6     |     5     |     4     |     3     |     2     |     1     |     0     |
-----------------------------------------------------------------------------------------
  7th Bit - SAR (0) /CFCM Test (1)

  For CFCM:
  6 & 5th Bits - Monitors BITs - CPU (00), Thermal (01), DSM (10), BW (11)

  For Thermal Monitor:
  Bit 4 to 0 --> Thermal Input 

  For DSM Monitor:
  4 Bit - RLC (0), LTE (1)
  3rd and 2nd Bit - DSM Pool types
  1st and 0th Bit - DSM Level types (FEW/MANY/DNE)
==========================================================================

===========================================================================*/
/* 7th Bit */
#define CFCM_TEST_DIAG_BIT 0x80
/* 6th and 5th Bits */
#define CFCM_TEST_DIAG_MONITOR_BITS 0x60
#define CFCM_TEST_DIAG_MONITOR_INPUT_MASK 0x1F


void cfcm_test_diag_command_inputs
( 
 uint8 input
)
{
  
  CFCM_MSG_3( HIGH, "cfcm_test_diag_command_inputs: CFCM test input %d (0x%x) monitor %d", input, input, 
                                    ((input & CFCM_TEST_DIAG_MONITOR_BITS) >> 5));

#ifdef FEATURE_MODEM_CFCM_DIAG_TEST
  #error code not present
#else
  CFCM_UNUSED(input);
#endif /* FEATURE_MODEM_CFCM_DIAG_TEST */

  return;

}

