/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                         D O W N L I N K   T A S K
                         
DESCRIPTION
  This file contains data and function definitions for WCDMA Circuit-Switched 
  Data Downlink Task. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
   

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsddltask.c_v   1.7   14 Jun 2002 13:10:06   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsddltask.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/13   pkp     Dog heart beat implementation.
03/22/12   nd      Fixing compilation error due to TCB variables undeclared.
10/01/11   bvd     Adding rcinit feaurization in task function
04/01/11   ttv     Support for Q6 free floating.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/04/09   sa      AU level CMI modifications.
26/02/07   DA      Fixed high lint errors
05/30/04   AR      Lint corrections.
08/05/03   TMR     Updated watermark callbacks for DSM 2
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
02/28/03   dwp     Add V.80 code to call end handling.
02/12/03   dwp     Add V.80 support.
06/14/02   dgy     Mainlined the Simple Rate Adaption under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
03/12/02   dgy     Checked all the DL signals after each rex_wait() in the DL
                   task.
02/21/02   dgy     Modified F3 messages.
09/20/01   dgy     Added the callback funtion for the DL WM,
                   dswcsd_set_dl_tx_data_sig(), and processing for the signal,
                   DSWCSD_DL_TX_DATA_SIG. Removed the command 
                   DSWCSD_PROC_DL_RLC_DATA_CMD.
08/31/01   dgy     Changed parameters passed into dswcsd_tx_data_to_sio().
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
                   Initialized the DL WM when starting the DL task.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "ds_3gppi_utils.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)


#ifdef FEATURE_DATA_WCDMA_CS
#include "rex.h"
#include "dog_hb_rex.h"
#include "rcinit_rex.h"
#include "modem_mem.h"
#include "msg.h"
#include "err.h"
#include "dswcsddl.h"
#include "dswcsddltask.h"
#include "dswcsddltask_v.h"

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
#include "dswcsdi.h"
#endif

#ifdef FEATURE_DATA_V80
#include "dswcsdv80.h"
#endif




/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  DL Task command queue
---------------------------------------------------------------------------*/  
static q_type dswcsd_dl_cmd_q = {{NULL}}; 

/*---------------------------------------------------------------------------
  TCB pointer for DSWCSD_DL task
---------------------------------------------------------------------------*/
static rex_tcb_type* dswcsd_dl_tcb_ptr = NULL; 


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_DL_TASK_INIT

DESCRIPTION
  This function performs all the power-up initialization for the WCDMA 
  Circuit-Switched Data Downlink Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_dl_task_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/  
  
  memset(&dswcsd_dl_cmd_q, 0, sizeof(q_type));
  (void)q_init( &dswcsd_dl_cmd_q );

} /* dswcsd_dl_task_init() */


/*===========================================================================

FUNCTION DSWCSD_DL_WAIT

DESCRIPTION
  This function suspends execution of the WCDMA Circuit-Switched Data Downlink 
  Task until one (or more) signals from the requested signal mask is set.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type  dswcsd_dl_wait
(
  rex_sigs_type  requested_signal_mask        /* Signal mask to suspend on */
)
{
  rex_sigs_type  set_signals;     /* Signal mask upon return from rex wait */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Wait for a signal to be set.
  -------------------------------------------------------------------------*/  
  set_signals = rex_wait( requested_signal_mask );

  return( set_signals );

} /* dswcsd_dl_wait() */


/*===========================================================================

FUNCTION DSWCSD_SET_DL_TX_DATA_SIG

DESCRIPTION
  This function sets the DL TX_DATA signal for the DL task, which tells the 
  DL task to send data to SIO.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void dswcsd_set_dl_tx_data_sig
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
)
#else
(
   void
)
#endif
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
   dswcsd_dl_tcb_ptr = rcinit_lookup_rextask("dswcsd_dl"); 
   if ( RCINIT_NULL == dswcsd_dl_tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for dswcsd_dl tcb returned NULL");
   }
#else
   dswcsd_dl_tcb_ptr = &dswcsd_dl_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

  (void)rex_set_sigs( dswcsd_dl_tcb_ptr, DSWCSD_DL_TX_DATA_SIG );

} /* dswcsd_set_dl_tx_data_sig() */


/*===========================================================================

FUNCTION DSWCSD_DL_PROCESS_CMDS

DESCRIPTION
  This function de-queues commands from the WCDMA CS Data Downlink Task's 
  command queue, and dispataches commands to the appropriate entity for further
  processing, if necessary. Commands are de-queued until the command queue is
  empty.

DEPENDENCIES
  This function should be called when the DSWCSD_DL_CMD_Q_SIG is set.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_dl_process_cmds( void )
{
  dswcsd_cmd_type *cmd_ptr;                        /* Pointer to a command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, invoke the corresponding function.
  -------------------------------------------------------------------------*/
  
  while( (cmd_ptr = (dswcsd_cmd_type *)q_get( &dswcsd_dl_cmd_q )) != 
         NULL )
  {
    DATA_MSG1_LOW("Rx'ed CS Data DL Task command %d", cmd_ptr->hdr.cmd_id);

    switch( cmd_ptr->hdr.cmd_id )
    {
      /*---------------------------------------------------------------------
        WCDMA CS Data DL Commands
      ---------------------------------------------------------------------*/
      
      case DSWCSD_CONNECT_CALL_CMD:
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
           /*----------------------------------------------------------------
             Initialize the DL SDU count.
           ----------------------------------------------------------------*/  

           dswcsd_set_dl_sdu_cnt(0);

           /*----------------------------------------------------------------
             Initialize the first byte expected in a DL SDU to 'a' in ASCII.
           ----------------------------------------------------------------*/  

           dswcsd_set_dl_expected_first_byte(FIRST_BYTE_START_VAL);
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */
           
           break;

#ifdef FEATURE_DATA_V80

      case DSWCSD_V80_CONNECT_CALL_CMD:
           /*----------------------------------------------------------------
             V.80 call is connected
           ----------------------------------------------------------------*/

           /*----------------------------------------------------------------
             Set flag used in down link data path to know if the R 
             interface is in V.80 mode.
           ----------------------------------------------------------------*/

           dswcsdv80_set_dl_r_iface_mode_is_v80(TRUE);
           break;

      case DSWCSD_V80_SEND_DTE_CMD:
           /*----------------------------------------------------------------
             Send the specified V.80 inband command to the DTE. 

             To prevent conflict of the UL & DL tasks both potentially 
             needing to send commands to the same WM, all V.80 inband
             commands sent to the DTE will be done by the DL task.
           ----------------------------------------------------------------*/

           dswcsdv80_send_cmd_to_dte(cmd_ptr->cmd.v80_cmd);
           break;

#endif /* FEATURE_DATA_V80 */

      case DSWCSD_END_CALL_CMD:
           /*----------------------------------------------------------------
            Tear down DL.
           ----------------------------------------------------------------*/

#ifdef FEATURE_DATA_V80
           dswcsdv80_set_dl_r_iface_mode_is_v80(FALSE);
#endif /* FEATURE_DATA_V80 */

           dswcsd_dl_cleanup();
           break;

      default:
           DS_3GPP_MSG1_ERROR( "Invalid CS Data DL Task command: %d", 
                      (uint8)cmd_ptr->hdr.cmd_id);
           break;
    } /* switch */

    modem_mem_free((void *)cmd_ptr, MODEM_MEM_CLIENT_DATA_CRIT);

  } /* while */

} /* dswcsd_dl_process_cmds() */


/*===========================================================================

FUNCTION DSWCSD_DL_TASK

DESCRIPTION
  This is the entry point for the WCDMA Circuit-Switched Data Downlink Task. 
  This function contains the main processing loop that waits for events 
  (signals or commands) and dispatches each event to the appropriate entity 
  for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void  dswcsd_dl_task
(
  uint32 ignored
)

{
  rex_sigs_type   requested_sigs;             /* Signal mask to suspend on */

  rex_sigs_type   set_sigs;           /* Signals set upon return from wait */
  dog_report_type dswcsd_dl_dog_rpt_id = 0;

#ifdef FEATURE_MODEM_RCINIT_PHASE2
   dswcsd_dl_tcb_ptr = rcinit_lookup_rextask("dswcsd_dl"); 
   if ( RCINIT_NULL == dswcsd_dl_tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for dswcsd_dl tcb returned NULL");
   }
#else
   dswcsd_dl_tcb_ptr = &dswcsd_dl_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  rcinit_handshake_startup();
#else

  /*-------------------------------------------------------------------------
    Do task initialization
  -------------------------------------------------------------------------*/
  dswcsd_dl_task_init();

#endif

  /*-------------------------------------------------------------------------
    Dog heart beat registration
  -------------------------------------------------------------------------*/
  dswcsd_dl_dog_rpt_id = dog_hb_register_rex(
                                 (rex_sigs_type)DSWCSD_DL_DOG_HB_REPORT_SIG);

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /*-------------------------------------------------------------------------
    Wait for the task start signal from TMC.
   -------------------------------------------------------------------------*/
   tmc_task_start();  
#endif

  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = ( DSWCSD_DL_CMD_Q_SIG | DSWCSD_DL_TX_DATA_SIG 
                    | DSWCSD_DL_DOG_HB_REPORT_SIG 
                    );

  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {
    /*-----------------------------------------------------------------------
      Wait for one of the specified signals to be set. Note that watchdog
      kicking is performed in the wait.
    -----------------------------------------------------------------------*/
    set_sigs = dswcsd_dl_wait( requested_sigs );
    DATA_MSG2_LOW( "req_sigs = 0x%x set_sigs = 0x%x", requested_sigs, set_sigs);

    /*-----------------------------------------------------------------------
      Check which signal is set, clear the signal, and take approriate action.
    -----------------------------------------------------------------------*/
    if( ( set_sigs & DSWCSD_DL_DOG_HB_REPORT_SIG ) != 0 )
    {
      /*---------------------------------------------------------------------
        Report to DOG heart beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_dl_tcb_ptr, DSWCSD_DL_DOG_HB_REPORT_SIG );
      dog_hb_report(dswcsd_dl_dog_rpt_id);
    }

    if( ( set_sigs & DSWCSD_DL_CMD_Q_SIG ) != 0 )
    {
      /*---------------------------------------------------------------------
        Clear the command signal and process the command.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_dl_tcb_ptr, DSWCSD_DL_CMD_Q_SIG );
      dswcsd_dl_process_cmds();
    }
    
    if ( ( set_sigs & DSWCSD_DL_TX_DATA_SIG ) != 0 ) 
    {
      /*---------------------------------------------------------------------
        Clear the TX_DATA signal and send data to SIO.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_dl_tcb_ptr, DSWCSD_DL_TX_DATA_SIG );
      dswcsd_tx_data_to_sio();
    }

  } /* forever */

} /* dswcsd_dl_task() */


/*===========================================================================

FUNCTION DSWCSD_DL_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the WCDMA CS Data Downlink Task. 
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

dswcsd_cmd_type *dswcsd_dl_get_cmd_buf( void )

{
  dswcsd_cmd_type *cmd_ptr;                   /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (cmd_ptr = 
        modem_mem_alloc(sizeof(dswcsd_cmd_type), 
                        MODEM_MEM_CLIENT_DATA_CRIT)) == NULL )
  {
    DATA_MSG1_ERROR("No free memory blocks available. Cmd buffer of size : %d",
               sizeof(dswcsd_cmd_type));
  }

  /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
  -------------------------------------------------------------------------*/
  return( cmd_ptr );

} /* dswcsd_dl_get_cmd_buf() */


/*===========================================================================

FUNCTION DSWCSD_DL_PUT_CMD

DESCRIPTION
  This function puts a command on the WCDMA CS DL Task's command queue, and
  sets the DSWCSD_DL_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function shodld have previously allocated a command
  buffer by calling dswcsd_dl_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  dswcsd_dl_put_cmd
(
  dswcsd_cmd_type *cmd_ptr         /* Pointer to command to be sent */
)

{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    DATA_MSG0_ERROR("dswcsd_dl_put_cmd() failed");
    return;
  }
#ifdef FEATURE_MODEM_RCINIT_PHASE2
   dswcsd_dl_tcb_ptr = rcinit_lookup_rextask("dswcsd_dl"); 
   if ( RCINIT_NULL == dswcsd_dl_tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for dswcsd_dl tcb returned NULL");
   }
#else
   dswcsd_dl_tcb_ptr = &dswcsd_dl_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( &dswcsd_dl_cmd_q, &cmd_ptr->hdr.link );
  (void)rex_set_sigs( dswcsd_dl_tcb_ptr, DSWCSD_DL_CMD_Q_SIG );

} /* dswcsd_dl_put_cmd() */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


