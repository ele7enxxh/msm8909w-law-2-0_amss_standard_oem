/*!
  @file
  RF Task 1 main control.

  @brief
  This file contains functions specific/internal to the RF Task.

  @defgroup RF_TASK
  @ingroup RF_TASK
  @{
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright(c) 2009 - 2013 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_task_1.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------- 
03/04/14    sw      Remove GFW MSGR framework as its now replaced with RFLM MSGR
01/30/14    sw      Receive RFLM signals on RF_TASK_1
08/12/13    sml     Receive GFW signals on RF_TASK_1
05/30/13    tws     Use the correct API header
05/30/13    tws     The GFW queue initialisation is now task specific. Handle
                    the therm_read responses from GFW in task_2
11/01/12    pl      Fix warnings
07/06/12    kb      Portrd RCINIT changes from RFA3/main/1.0
06/20/12    sty     delete rfm_init() call in rf_task() - as other tasks are 
                    not running yet     
06/14/12    gh      Added timer for ftm_wcdma_exit for W to L FTM transition
06/10/12    sbm     Remove break from void rf_task_handle_sigs().
05/18/12    sar     Changed scope of rf_init_comp_msg to global for this file. 
04/27/12    sar     Modified rf_init_complete_send_ind() and fixed CC Review defects. 
04/25/12    sar     Added rf init complete indication for CM.
04/24/12    jfc     Use TASK_HANDLE instead of rex_self()
03/19/12    jfc     Replace rf_tcb with rex_self().
10/27/11    gh      Added handling for FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG
09/14/11    can     Merge MDM9K tips to NikeL.
07/12/11    sty     Fixed issue where messages were not being processed if 
                    received back-to-back
06/28/11    bmg     Fixed FEATURE_RF_TASK_WDOG_DYNAMIC feature wrapping
06/23/11    tnt     Fix dog timeout when receiving TASK_STOP_SIG
05/23/11    bmg     Mainlined FEATURE_QTF_OFFTARGET
04/29/11    av     Change in ftm_gsm_rx_disable(). The wait is now on the task that 
                    is running: FTM or RF task. Takes care of crash between G to L FTM
                    transition
03/28/11    bmg     Cancel watchdog report timer and exit the RF task function
                    when receiving the TASK_STOP_SIG
09/22/10    mkv     Adding Dynamic watchdog support. 
10/06/09    mkv/pl  Modified rf task init sequence.
08/26/09    mkv     Integrating REX MSGR Interworking into RFA2. 
08/26/09    ans     Removed 1x specific code that should move to MC layer for 1x
07/21/09    mkv     Moving tcb/stack declarations for task.c stub into this file.
07/21/09    mkv     Added REX MSGR Interworking support. 
06/17/09    ans     Initial revision
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
/*! @TODO rf_cap.h is required for MODEM LTE DEV offtarget features. Should be 
  removed once the custrf.h matures for LTE. */
#ifdef FEATURE_LTE
#include "rf_cap.h" 
#endif /*FEATURE_LTE*/
#include "ftm.h"


/* Feature RF Task should be used to encompass complete RF task related file.
   This is not meant to be used for other featurization. This is meant just to 
   provide compatibility with top leavel feature used in TMC and Task modules. */
#ifdef FEATURE_RF_TASK
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "msg.h"
#include "rfcommon_msg.h"

#ifdef FEATURE_GSM
#include "rfgsm_mc.h"
#endif


#include "rf_task.h"
#include "rfcom.h" 
#include "rf_cmd_interface.h"
#include "rf_cmd_dispatcher.h"
#include "rf_rex_msgr_interface.h"
#include "rf_task_ind_umid.h"

#include "rfcommon_msg.h"
#include "rf_task.h"
#include "rf_task_common_services.h"

#ifdef FEATURE_GSM
#include "rfgsm_mc.h"
#include "rfgsm_ftm_cmd_proc.h"
#endif

#ifdef T_WINNT
#error code not present
#endif
/*===========================================================================

                               Macros

===========================================================================*/



/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

static void rf_task_1_handle_sigs( rex_sigs_type mask )
{
  rex_sigs_type sigs = 0;

  boolean rex_msgr_interworking_sucess;

  /* if no signal is required to be received,return immediately */
  if (mask == sigs)
  {
    return;
  }

  /* Loop forever handling commands */
  for (;;)
  {
      sigs = rex_wait( mask );

      /* There is implicit priority given to signals. Only one signal
         is processed at a time.
       */
      if ( sigs & RF_RPT_TIMER_SIG )
      {
        (void) rex_clr_sigs(TASK_HANDLE(rf_1),(RF_RPT_TIMER_SIG));
        rf_task_kick_watchdog(RF_TASK_1);
      }
      if ( sigs & TASK_STOP_SIG )
      {
         (void) rex_clr_sigs(TASK_HANDLE(rf_1), TASK_STOP_SIG);
         /*De-initialize REX MSGR Interface.*/
         rex_msgr_interworking_sucess = rf_rex_msgr_interface_deinit(RF_TASK_1);

         if (rex_msgr_interworking_sucess != TRUE)
         {
           MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RF task_1 REX MSGR Interface De-initialization failed" );
         }

         /*De-initialize the dispatcher. Free allocated memory.*/
         rf_cmd_dispatch_deinit (RF_TASK_1);

         rf_task_deinit_watchdog(RF_TASK_1);

         rfa_set_task_status(RF_TASK_1, FALSE);

  #ifndef FEATURE_MODEM_RCINIT
         tmc_task_stop();
  #else
         RF_MSG( RF_ERROR, "rf_task_1_handle_sigs: need to call right API to stop task");
  #endif /* FEATURE_MODEM_RCINIT */

         /* Task is stopped.  No need to run this function anymore. */
         break;
      }

      if ( sigs & RF_CMD_Q_SIG )
      {
        /* First, clear the signal */
        (void) rex_clr_sigs(TASK_HANDLE(rf_1), RF_CMD_Q_SIG);

         /* process commands in a loop to ensure that RF does not "skip"
            processing of any messages */
         while ( rf_num_of_cmds_in_q(RF_TASK_1) != 0 )
         {
            rf_cmd_process(RF_TASK_1); /* Process RF commands in the queue */
         }
      }

      if ( sigs & RF_RFLM_CMD_Q_SIG )
      {
        (void) rex_clr_sigs(TASK_HANDLE(rf_1),(RF_RFLM_CMD_Q_SIG)); 
        #ifdef FEATURE_GSM       
          rfgsm_rflm_rsp_dispatch( RF_TASK_1 );        
        #endif
      }

      if ( sigs & RF_NV_CMD_SIG )
      {
        /* Reading NV at power-up, continue */
        break;
      }
  } /* for (;;) */

} /* rf_task_1_handle_sigs*/
/*!
  @brief
  RF Task control.

  @details
  This is the Radio Frequency (RF) task, created from the Main Control task. 
  After processing task start procedure, the RF task enters an infiniten loop, 
  awaiting commands received on its command queue.  Each command received is
  processed to completion before the next command is processed.  While in the
  main loop the task kicks the watchdog periodically.

  @param parm
  dummy variable which is ignored.
*/
void  rf_task_1
(
  dword              parm       /* from REX - ignored                     */
)
{
  rex_sigs_type sigs = 0;

  boolean rex_msgr_interworking_sucess;
  /*Initialize RF TASK DATA*/ 
  rf_task_initialize_data(RF_TASK_1);

  /*! @TODO RF NUM MODES is assumed to be number of module's defined in the system. */
  rf_cmd_dispatch_init( RF_TASK_1, RFM_NUM_MODES );

#ifdef FEATURE_GSM
  /* RF GSM msg router q initialisation for RFLM cmds that rfgsm will receive*/
  rfgsm_initialise_rflm_msg_router_q( RF_TASK_1 );
  MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "Initialise RFLM MSGR");
#endif

  /* Initialize timers for watchdog report and sanity. and initialize timers*/
  rf_task_init_watchdog(RF_TASK_1);

  /* Initialize command queue.  */
  rf_cmd_init(RF_TASK_1);

  /*  ** Wait for the start signal from the Task Main Controller*/
  #ifdef FEATURE_MODEM_RCINIT
    rcinit_handshake_startup();
  #else
    tmc_task_start();
  #endif

  /*Indicates that RF Task is now running. QTF returns here after tmc_task_start.
    Do not change the order. Else the QTF test cases will fail to ASSERT.*/
  rfa_set_task_status(RF_TASK_1, TRUE);

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Initialize REX MSGR Interface. This must be done after tmc_task_start() returns
  */

  rex_msgr_interworking_sucess = rf_rex_msgr_interface_init(RF_TASK_1, TASK_HANDLE(rf_1),RF_CMD_Q_SIG);

  /*Register for the RF  L1 UMIDs that RF Task intends to recieve.*/
  if (rex_msgr_interworking_sucess == TRUE)
  {
    rex_msgr_interworking_sucess = rf_rex_task_msgr_register(RF_TASK_1);
  }

  if(rex_msgr_interworking_sucess != TRUE)
  {
     /*Kill the RF task.*/
     rfa_set_task_status(RF_TASK_1, FALSE);

     /*De-initialize the dispatcher. Free allocated memory.*/
     rf_cmd_dispatch_deinit (RF_TASK_1);
  }

  /* RF GSM dispatch registration */
  rfgsm_dispatch_init();
  /* RF GSM FTM dynamic dispatch registration */
  rfgsm_ftm_dispatch_init();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


//  MSG( MSG_SSID_RF, MSG_LEGACY_HIGH, "RF task Initialization finished" );

  /* Kick watchdog */
  rf_task_kick_watchdog(RF_TASK_1);
  
  
  sigs = RF_RPT_TIMER_SIG         /* Watchdog report timer */
         | TASK_STOP_SIG          /* Task shutdown   */
         | RF_CMD_Q_SIG           /* Command queue signal */
         | RF_RFLM_CMD_Q_SIG;     /* signal to receive RFLM cmds */ 


  /* Wait for the Signals and process accordingly */
  rf_task_1_handle_sigs(sigs);
} /* rf_task */


/*!
  @brief
  Dispatch function for common commands dispatched from RF_TASK
 
  @details
  This function is called by RF_TASK when it receives messages/commands for common rf
  
  @param req_ptr
  Pointer to RF Task command
*/
static void
rf_task_1_dispatch
(
  rf_cmd_type* req_ptr
)
{

  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_1_dispatch: NULL Request" );
    return;
  } /* if( req_ptr == NULL ) */

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid for rf task */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_RFA_RF_CONTROL )
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_1_dispatch: Cannot handle UMID 0x%x",
              rf_uimd );
    return;
  }

  /* Dispatch based on the UMID */
  switch( rf_uimd )
  {
    default:
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_1_dispatch: Invalid UMID 0x%x",
             rf_uimd );
      break;
  } /* switch( req_ptr->msgr_hdr.id ) */

  return;

} /* rf_task_1_dispatch */

/*!
  @brief
  Initialize dispatcher for CDMA commands processed through RF Task

  @details
  This function will register all CDMA commands to RF Task dispatcher and
  also registers with message router, if required.
 
  @param msgr_client
  Message router client ID for registration
 
  @param mailbox
  Messgae router ID for registration

  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_task_1_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
)
{
  rf_cmd_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /*No messages to receive for RF_TASK_GSM so comment out for now*/
//static msgr_umid_type rf_control_umid_list[] =
//{
//
//};
//
//ret_status = msgr_register_block(MSGR_RFA_RF_CONTROL,
//                                 msgr_client,
//                                 mailbox,
//                                 rf_control_umid_list,
//                                 0);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("MSGR_RFA_RF_CONTROL reg error %d", ret_status, 0, 0);
    init_status = FALSE;
  }

  /* Register handler for this function */
  dis_res = rf_cmd_dispatch_register_tech( RF_TASK_1, MSGR_TECH_RFA, MSGR_MODULE_RF_CONTROL,
                                           rf_task_1_dispatch );

  if ( dis_res != RF_CMD_REGISTRATION_SUCCESS )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_1_dispatch_init: Failed RFA / RF_1X Tech "
                      "registration"  );    
    init_status = FALSE;
  }

  /* Return Status */
  return init_status;

} /* rf_task_dispatch_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize dispatcher for RF Common Control commands processed through RF Task

  @details
  This function will unregister message router commands, release
  allocated semphores and unrgister RF task.
 
  @param msgr_client
  Message router client ID for unregistration
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rf_task_1_dispatch_deinit
(
  msgr_client_t *msgr_client
)
{
  boolean deinit_status = TRUE;

  /*--------------------------------------------------------------------------*/
  /* RF Task de-registration */
  /*--------------------------------------------------------------------------*/

  rf_cmd_dispatch_deregister_tech(RF_TASK_1, MSGR_TECH_RFA, MSGR_MODULE_RF_CONTROL );

  /* Return Status */
  return deinit_status;

} /* rf_task_dispatch_deinit */


#endif /* FEATURE_RF_TASK */
/*! @} */

