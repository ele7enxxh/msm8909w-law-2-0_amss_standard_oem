/*!
  @file
  RF Task main control.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_task.c#1 $

when        who     what, where, why
--------    ---     -----------------------------------------------------------
03/04/14   sw      Remove GFW MSGR framework
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
07/29/13   tws      Move GSM functionality to RF_TASK_1
07/11/13    Saul    Cal Time Reduction. Disabled debug mode.
05/30/13   tws      Use the API header
05/30/13   tws      The GFW queue initialisation is now task specific
03/25/13    sc      Correct featurisation naming error
03/11/13   sar      Updated file for APQ, GNSS only target.
01/31/13   tsr      Added framework for recieving therm read response from GFW
01/31/13    sc      Added dispatcher initialisation to rf_task
01/16/13   aro      Renamed handler to handle
01/16/13   aro      Renamed handler to handle
01/16/13    aro     Renamed dispatch_handler to dispatch_handle
01/15/13    aro     Updated registration interface to pass dispatch handle
01/09/13    aro     Added query functions to get REX data and UMID
11/28/12    bmg     Standardized RFM_INITIALIZED_EVENT_NAME and added RF task
                    init function to insure the rfm_init() event is available.
11/08/12    jfc     Add a useful error message when RF task is deinit'ed 
11/01/12    pl      Fix warnings
10/05/12    av      Expanded rf task architecture to include more tasks. 
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


#include "rf_task.h"
#include "rfcom.h" 
#include "rf_cmd_interface.h"
#include "rf_cmd_dispatcher.h"
#include "rf_rex_msgr_interface.h"
#include "rf_task_ind_umid.h"
#include "rfm_init_event.h"

#include "rfcommon_msg.h"
#include "rf_task_common_services.h"
#include "rfm_umid_list.h"
#include "rfcommon_cmd_processing.h"

boolean rfm_init_complete = FALSE;
boolean rfm_init_complete_status_pending = FALSE;


#ifdef FEATURE_RF_MEAS
extern void rfmeas_dispatch_init(void);
#endif


/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

extern boolean rfm_init ( rex_tcb_type *,
                          rex_sigs_type,
                          void (*)( rex_sigs_type ));

extern rf_cmd_struct_type* rf_cmd_get_queue_address( rf_task_num_type task_num );

/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
rf_init_comp_msg_type  rf_init_comp_msg;
boolean rfm_init_status = FALSE;
static rf_dispatch_handle_type rf_task_dispatch_handle;

// rex_timer_type ftm_wcdma_enter_exit_wait_timer_rf_tcb;

/*===========================================================================

                               Macros

===========================================================================*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rex Data
 
  @details
  In order perform REX-MSGR initialization for RF task, REX data is
  required. This interface return the REX data to be used by RF dispatch to
  perform REX-MSGR registration
 
  @param rex_data_ptr
  Pointer to the container where REX data is populated
 
  @return
  Flag indicating the validity of the populated Rex data
*/
static boolean
rf_task_get_rex_data
(
  rf_dispatch_rex_data_type *rex_data_ptr
)
{
  rf_cmd_struct_type *ptr_rf_cmd;

  /* NULL Pointer check */
  if ( rex_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_task_get_rex_data: NULL container" );
    return FALSE;
  } /* if ( rex_data_ptr == NULL ) */

  ptr_rf_cmd =  rf_cmd_get_queue_address(RF_TASK_MAIN);
  
  rex_data_ptr->rex_tcb = TASK_HANDLE(rf);
  rex_data_ptr->rex_sig = RF_CMD_Q_SIG;
  rex_data_ptr->cmd_q = &(ptr_rf_cmd->cmd_q);
  rex_data_ptr->free_q = &(ptr_rf_cmd->free_q);
  rex_data_ptr->header_size = sizeof(rf_cmd_rex_hdr_type);
  rex_data_ptr->q_link_size = sizeof(q_link_type);
  rex_data_ptr->command_size = sizeof(rf_cmd_type);

  return TRUE;
} /* rf_task_get_rex_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract UMID
 
  @details
  This function will extract the UMID for the command payload pointer
 
  @param cmd_ptr
  Pointer to command payload
 
  @param umid
  Contianer to store the extracted UMID
 
  @return
  Flag indicating the validity of the UMID extraction
*/
static boolean
rf_task_extract_umid
(
  void *cmd_ptr,
  msgr_umid_type *umid
)
{
  rf_cmd_type *cmd;

  /* NULL Pointer check */
  if (( cmd_ptr == NULL ) || ( umid == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_task_extract_umid: NULL Data" );
    return FALSE;
  } /* if (( cmd_ptr == NULL ) || ( umid == NULL ) ) */

  /* Typecast */
  cmd = (rf_cmd_type*)cmd_ptr;

  /* Extract UMID */
  *umid = cmd->msgr_hdr.id;

  return TRUE;
} /* rf_task_extract_umid */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Dispatch Handle
 
  @details
  This function return the dispatch handle to used used by the client for
  command registration and deregistration
 
  @return
  Pointer to RF_TASK dispatch handle data
*/
rf_dispatch_handle_type *
rf_task_get_dispatch_handle
(
  void
)
{
  return &(rf_task_dispatch_handle);
} /* rf_task_get_dispatch_handle */

/*!
  @brief
  rf_init_complete_send_resp

  @details
  Sends msg via message router to cm task that rfm_init has completed.

  @param[in]  msgr_tech_module_type Destination of the indication.
  @param[in]  msgr_umid_type        umid of the indication message.
  @param[in]  void *                Pointer to the payload.
  @param[in]  uint32                Total message length (header and payload) in bytes
 
*/
void rf_init_complete_send_rsp( void )
{

  errno_enum_type      retval = E_FAILURE;

  if(!rfm_init_complete)
  {
    rfm_init_complete_status_pending = TRUE;
    return;
  }
  msgr_init_hdr( &rf_init_comp_msg.hdr, MSGR_RFA_RF_CONTROL, RFA_RF_CONTROL_INIT_COMPLETE_RSP );

  /* Set init status to saved value from rfm_init() */
  rf_init_comp_msg.rf_init_success = rfm_init_status;

  retval = msgr_send(&rf_init_comp_msg.hdr, sizeof(rf_init_comp_msg));
  
  if (retval != E_SUCCESS)
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "RF Init Complete Ind error %0x", 
           RFA_RF_CONTROL_INIT_COMPLETE_RSP );
  }
} /* rf_init_complete_send_ind */




static void rf_task_handle_sigs( rex_sigs_type mask )
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
      (void) rex_clr_sigs(TASK_HANDLE(rf),(RF_RPT_TIMER_SIG));
      rf_task_kick_watchdog(RF_TASK_MAIN);
    }
    if ( sigs & TASK_STOP_SIG )
    {
      

       (void) rex_clr_sigs(TASK_HANDLE(rf), TASK_STOP_SIG);
       /*De-initialize REX MSGR Interface.*/
       rex_msgr_interworking_sucess = rf_rex_msgr_interface_deinit(RF_TASK_MAIN);

       if (rex_msgr_interworking_sucess != TRUE)
       {
         MSG( MSG_SSID_RF, MSG_LEGACY_LOW, "RF task_main REX MSGR Interface De-initialization failed" );
       }

       /*De-initialize the dispatcher. Free allocated memory.*/
       rf_cmd_dispatch_deinit (RF_TASK_MAIN);

       rf_task_deinit_watchdog(RF_TASK_MAIN);

       rfa_set_task_status(RF_TASK_MAIN, FALSE);

#ifndef FEATURE_MODEM_RCINIT
       tmc_task_stop();
#else
       RF_MSG( RF_ERROR, "rf_task_handle_sigs: need to call right API to stop task");
#endif /* FEATURE_MODEM_RCINIT */

       /* Task is stopped.  No need to run this function anymore. */
       break;
    }
    if ( sigs & RF_CMD_Q_SIG )
    {
      /* First, clear the signal */
      (void) rex_clr_sigs(TASK_HANDLE(rf), RF_CMD_Q_SIG);

       /* process commands in a loop to ensure that RF does not "skip" 
          processing of any messages */
       while ( rf_num_of_cmds_in_q(RF_TASK_MAIN) != 0 )
       {          
          rf_cmd_process(RF_TASK_MAIN); /* Process RF commands in the queue */
       }       
    }

    if ( sigs & RF_NV_CMD_SIG )
    {
      /* Reading NV at power-up, continue */
      break;
    }
  } /* for (;;) */

} /* rf_task_handle_sigs*/

void
rf_task_init_function
(
  void
)
{
  RCEVT_HANDLE handle;

  RF_MSG(RF_HIGH, "rf_task_init_function");

  handle = rcevt_create_name(RFM_INITIALIZED_EVENT_NAME);

  if ( handle == RCEVT_NULL )
  {
    ERR_FATAL("Failed to create RFM initialized event handle.  Cannot continue.", 0, 0, 0);
  }
} /* rf_task_init_function */

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
void  rf_task
(
  dword              parm       /* from REX - ignored                     */
)
{
  rex_sigs_type sigs = 0;             
  boolean rex_msgr_interworking_sucess;
  rf_dispatch_init_handle_data_type init_data;
  RCEVT_HANDLE handle;

  /*Initialize RF TASK DATA*/ 
  rf_task_initialize_data(RF_TASK_MAIN);

  /*-------------------------------------------------------------------------*/
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /*! @TODO RF NUM MODES is assumed to be number of module's defined in the system. */
  rf_cmd_dispatch_init( RF_TASK_MAIN, RFM_NUM_MODES );
   
     
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rfcommon_dispatch_init(RF_TASK_MAIN);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize timers for watchdog report and sanity. and initialize timers
    */

   rf_task_init_watchdog(RF_TASK_MAIN);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize command queue.
  */

  rf_cmd_init(RF_TASK_MAIN);

  
//  rex_def_timer(&ftm_wcdma_enter_exit_wait_timer_rf_tcb, &rf_tcb, FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*
  ** Wait for the start signal from the Task Main Controller
  */
  #ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
  #else
  tmc_task_start();
  #endif

  /*Indicates that RF Task is now running. QTF returns here after tmc_task_start. 
    Do not change the order. Else the QTF test cases will fail to ASSERT.*/
  rfa_set_task_status(RF_TASK_MAIN, TRUE);

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize REX MSGR Interface. This must be done after tmc_task_start() returns 
  */

  rex_msgr_interworking_sucess = rf_rex_msgr_interface_init(RF_TASK_MAIN, TASK_HANDLE(rf),RF_CMD_Q_SIG);

    /*Register for the RF  L1 UMIDs that RF Task intends to recieve.*/
  if (rex_msgr_interworking_sucess == TRUE)
  {
    rex_msgr_interworking_sucess = rf_rex_task_msgr_register(RF_TASK_MAIN);
  }


  if(rex_msgr_interworking_sucess != TRUE)
  {
      /*Kill the RF task.*/
      rfa_set_task_status(RF_TASK_MAIN, FALSE);

      /*De-initialize the dispatcher. Free allocated memory.*/
      rf_cmd_dispatch_deinit (RF_TASK_MAIN);

      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "RF task init failed due to error in MSGR init/registration. " 
           "RF main task and RF cmd dispatch deinitialized" );
  }

  /* Initialize Dispatch handler */
  init_data.task_name = "RF_TASK";
  init_data.task_uid = 0x63e66dd8; /* MD5(RF_TASK) */
  init_data.extract_cid_fp = rf_task_extract_umid;
  init_data.debug_mode = FALSE;
  rf_task_get_rex_data( &(init_data.rex_data) );
  rf_dispatch_init_handle( &rf_task_dispatch_handle,
                            init_data );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MSG( MSG_SSID_RF, MSG_LEGACY_HIGH, "RF task Initialization finished" );

#ifdef FEATURE_RF_MEAS
  /* RF MEAS dispatch registration */
  rfmeas_dispatch_init();
#endif

  /* Kick watchdog */
  rf_task_kick_watchdog(RF_TASK_MAIN);

  #ifdef FEATURE_MODEM_RCINIT
  rfm_init_status = rfm_init ( rex_self(),
               RF_NV_CMD_SIG|RF_RPT_TIMER_SIG,
               rf_task_handle_sigs );
             
  handle = rcevt_signal_name(RFM_INITIALIZED_EVENT_NAME);
  
  #endif

  rfm_init_complete = TRUE;
               
  if(rfm_init_complete_status_pending)
  {
    rf_init_complete_send_rsp();
  }

  sigs = RF_RPT_TIMER_SIG         /* Watchdog report timer */
         | TASK_STOP_SIG          /* Task shutdown   */
         | RF_CMD_Q_SIG;          /* Command queue signal */

  /* Wait for the Signals and process accordingly */
  rf_task_handle_sigs(sigs);

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
rf_task_dispatch
(
  rf_cmd_type* req_ptr
)
{

  msgr_umid_type rf_uimd; /* Received UMID */

  /* Validate Request Pointer */
  if( req_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_dispatch: NULL Request" );
    return;
  } /* if( req_ptr == NULL ) */

  /* Get UMID */
  rf_uimd = req_ptr->msgr_hdr.id;

  /* Check if the dispatched message is valid for rf task */
  if( MSGR_TECH_MODULE_VAL(rf_uimd) != MSGR_RFA_RF_CONTROL )
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_dispatch: Cannot handle UMID 0x%x",
              rf_uimd );
    return;
  }

  /* Dispatch based on the UMID */
  switch( rf_uimd )
  {
    case RFA_RF_CONTROL_INIT_COMPLETE_CMD:
      rf_init_complete_send_rsp();
      break;

    default:
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_dispatch: Invalid UMID 0x%x",
             rf_uimd );
      break;
  } /* switch( req_ptr->msgr_hdr.id ) */

  return;

} /* rf_task_dispatch */

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
rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
)
{
  rf_cmd_dispatch_status_type dis_res; /* cmd_dispatch result */
  boolean init_status = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  static msgr_umid_type rf_control_umid_list[] = 
  {
    RFA_RF_CONTROL_INIT_COMPLETE_CMD
  };

  ret_status = msgr_register_block(MSGR_RFA_RF_CONTROL,
                                   msgr_client,
                                   mailbox,
                                   rf_control_umid_list,
                                   1);
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("MSGR_RFA_RF_CONTROL reg error %d", ret_status, 0, 0);
    init_status = FALSE;
  }

  /* Register handler for this function */
  dis_res = rf_cmd_dispatch_register_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_CONTROL,
                                           rf_task_dispatch );

  if ( dis_res != RF_CMD_REGISTRATION_SUCCESS )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_task_dispatch_init: Failed RFA / RF_1X Tech "
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
rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
)
{
  boolean deinit_status = TRUE;

  /*--------------------------------------------------------------------------*/
  /* RF Task de-registration */
  /*--------------------------------------------------------------------------*/

  rf_cmd_dispatch_deregister_tech( RF_TASK_MAIN, MSGR_TECH_RFA, MSGR_MODULE_RF_CONTROL );

  /* Return Status */
  return deinit_status;

} /* rf_task_dispatch_deinit */

#endif /* FEATURE_RF_TASK */
/*! @} */

