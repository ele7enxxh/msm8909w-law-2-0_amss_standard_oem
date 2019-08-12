/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                          U P L I N K   T A S K

DESCRIPTION
  This file contains data and function definitions for WCDMA Circuit-Switched 
  Data Uplink Task. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsdultask.c_v   1.11   14 Jun 2002 13:14:52   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdultask.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/13   pp      Dog heart beat implementation.
03/22/12   ND      Fixing compilation error due to TCB variables undeclared.
11/11/11   ND      Feature cleanup.
10/01/11   bvd     Added rcinit featurization to task function
09/23/11   TTV     Fixed an issue of leaving critical section correctly.
04/05/11   SS      Q6 free floating support.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/04/09   sa      AU level CMI modifications.
23/02/07   DA      Fixed high lint errors
19/10/06   DA      Changes to dswcsd_ul_process_cmds() to avoid dswcsd_ul_task
                          from hogging cpu when there is no data available from SIO.
05/30/04   AR      Lint corrections.
08/05/03   TMR     Updated watermark callbacks for DSM 2
07/23/03   dwp     Move dswcsdv80_init() call into dswcsd_ul_task_init().
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA. Moved some definitions
                   here from dswcsd.h. Moved some data definition to 
                   dswcsdul.c.
03/07/03   dwp     Make sync test app runtime configurable.
02/28/03   dwp     Add V.80 code to call end handling.
02/12/03   dwp     Add V.80 support; process DSWCSD_V80_CONNECT_CALL_CMD.
11/18/02   dgy     Removed dswcsd_calculate_bytes_per_sdu() and
                   dswcsd_calculate_sdus_per_tti().
06/14/02   dgy     Mainlined the Simple Rate Adaption under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
05/16/02   dgy     Added support for Multi-Mode CM.
05/06/02   dgy     Computed SDU size and character length on a per-call basis
                   for Asynchronous CS Data.
04/29/02   dgy     Replaced static variables with global variables. Reset the
                   variables for each call.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
03/12/02   dgy     Featurized Asynchronous Transparent CS Data under 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS. Cleaned up the triggering
                   mechanism for Synchronous CS Data. Checked all the UL 
                   signals after each rex_wait() in the UL task.
02/21/02   dgy     Added Synchronous CS Data and a run-time switch to switch 
                   between Asynchronous and Synchronous CS Data.
01/11/02   dgy     Renamed some functions.
10/30/01   dgy     Supported various data rates.
09/20/01   dgy     Built SDUs in WCDMA CS UL task context instead of RLC 
                   task context. Hardcoded to send 2 SDUs per TTI. 
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
                   Initialized the UL WM when starting the UL task.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include <stringl/stringl.h>


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "dog_hb_rex.h"

#include "modem_mem.h"
#include "msg.h"
#include "err.h"
#include "dsat_v.h"
#include "data_msg.h"
#include "rcinit_rex.h"

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
#include "dswcsdi.h"
#endif

#ifdef FEATURE_DATA_V80
#include "dswcsdv80.h"
#endif

#include "dswcsdul.h"
#include "dswcsdultask.h"
#include "dswcsdultask_v.h"
#include "dswcsdmshif.h"

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
#define LINE_FEED                0x0A                  /* Hex value for LF */

#define CARRIAGE_RETURN          0x0D                  /* Hex value for CR */

#define DATA_PATTERN_START_VAL   0x30  /* Start value for the data pattern */

#define DATA_PATTERN_VAL_RANGE   75  /* Number of different values allowed **
                                     **               for the data pattern */
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */


/*---------------------------------------------------------------------------
  DL Task command queue
---------------------------------------------------------------------------*/
static q_type dswcsd_ul_cmd_q = {{NULL}};

/*---------------------------------------------------------------------------
  TCB pointer for DSWCSD_UL task
---------------------------------------------------------------------------*/
static rex_tcb_type* dswcsd_ul_tcb_ptr = NULL; 

/*---------------------------------------------------------------------------
  QRD CSVT call NV item.
---------------------------------------------------------------------------*/  
extern uint8 dswcsd_is_qmss_enabled;

#ifdef FEATURE_DATA_V80
/*---------------------------------------------------------------------------
  Indices into AT+ESA information
---------------------------------------------------------------------------*/
/*
#define ESA_TRANS_IDLE_INDEX    0
#define ESA_FRAMED_IDLE_INDEX   1
#define ESA_FRAMED_UN_OV_INDEX  2
#define ESA_HD_AUTO_INDEX       3
#define ESA_CRC_TYPE_INDEX      4
#define ESA_NRZI_EN_INDEX       5
#define ESA_SYN2_INDEX          7
*/
#define ESA_SYN1_INDEX          6

#endif /* FEATURE_DATA_V80 */

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_UL_TASK_INIT

DESCRIPTION
  This function performs all the power-up initialization for the WCDMA 
  Circuit-Switched Data Uplink Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_ul_task_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_V80

  /*-------------------------------------------------------------------------
    Initialize look up table for DL Transparency Character Escaping
  -------------------------------------------------------------------------*/
  dswcsdv80_init();

#endif

  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/  
  
  memset(&dswcsd_ul_cmd_q, 0, sizeof(q_type));
  (void) q_init( &dswcsd_ul_cmd_q );

} /* dswcsd_ul_task_init() */


/*===========================================================================

FUNCTION DSWCSD_UL_WAIT

DESCRIPTION
  This function suspends execution of the WCDMA Circuit-Switched Data Uplink 
  Task until one (or more) signals from the requested signal mask is set.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type  dswcsd_ul_wait
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

} /* dswcsd_ul_wait() */


/*===========================================================================

FUNCTION DSWCSD_SET_UL_TX_DATA_SIG

DESCRIPTION
  This function sets the UL TX_DATA signal for the UL task, which tells the 
  UL task to send data to RLC.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void dswcsd_set_ul_tx_data_sig
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
  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer locally.
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dswcsd_ul_tcb_ptr = rcinit_lookup_rextask("dswcsd_ul"); 
  if ( RCINIT_NULL == dswcsd_ul_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for dswcsd_ul tcb returned NULL");
  }
#else
  dswcsd_ul_tcb_ptr = &dswcsd_ul_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

  (void)rex_set_sigs( dswcsd_ul_tcb_ptr, DSWCSD_UL_TX_DATA_SIG );
} /* dswcsd_set_ul_tx_data_sig() */


/*===========================================================================

FUNCTION DSWCSD_UL_PROCESS_CMDS

DESCRIPTION
  This function de-queues commands from the WCDMA CS Data Uplink Task's command
  queue, and dispataches commands to the appropriate entity for further
  processing, if necessary. Commands are de-queued until the command queue is
  empty.

DEPENDENCIES
  This function should be called when the DSWCSD_UL_CMD_Q_SIG is set.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dswcsd_ul_process_cmds( void )
{
  dswcsd_cmd_type  *cmd_ptr;                       /* Pointer to a command */
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
  uint8             i;                                            /* Index */
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */
  dswcsd_call_info_T dswcsd_call_info;
  uint16             sio_q_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T), 
          dswcsd_get_call_info_ptr(), sizeof(dswcsd_call_info_T));

  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, invoke the corresponding function.
  -------------------------------------------------------------------------*/
  while( (cmd_ptr = (dswcsd_cmd_type *)q_get( &dswcsd_ul_cmd_q )) != 
         NULL )
  {
    DATA_MSG1_LOW("Rx'ed CS Data UL Task command %d", cmd_ptr->hdr.cmd_id);

    switch( cmd_ptr->hdr.cmd_id )
    {
      /*---------------------------------------------------------------------
        WCDMA CS Data UL Commands
      ---------------------------------------------------------------------*/

      case DSWCSD_CONNECT_CALL_CMD:
           sio_q_cnt = dswcsd_get_from_sio_q_cnt();
           MSG_MED("dswcsd_from_sio_q_cnt %d, dswcsd_dsm_payload_size %d, sdus_per_tti*bytes_per_sdu %d", 
                   sio_q_cnt, 
                   dswcsd_get_dsm_payload_size(),
                   (dswcsd_call_info.sdus_per_tti * dswcsd_call_info.bytes_per_sdu));

           /*----------------------------------------------------------------
            Check NV value, if first call read the NV item and retrieves the
            configured values.
           ----------------------------------------------------------------*/  
           if(dswcsd_is_qmss_enabled == 2)
           {/* First call try reading the NV */
             ds_wcsd_get_csvti_nv_qmss_enabled(cmd_ptr->hdr.subs_id);
           }
           /*----------------------------------------------------------------
            Reset the pointer to DSM item from SIO.
           ----------------------------------------------------------------*/  

           dswcsd_set_sio_dsm_item_ptr(NULL);
		
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
           /*----------------------------------------------------------------
            Reset the DSM item payload size.
           ----------------------------------------------------------------*/  

           dswcsd_set_dsm_payload_size(0);

           if(dswcsd_is_sync_test_app_enabled())
           {
             /*--------------------------------------------------------------
               Initialize the UL SDU count.
             --------------------------------------------------------------*/  

             dswcsd_set_ul_sdu_cnt(0);

             /*--------------------------------------------------------------
               Initialize the first byte in a UL SDU to 'a' in ASCII.
             --------------------------------------------------------------*/  

             dswcsd_set_ul_first_byte(FIRST_BYTE_START_VAL);

             /*--------------------------------------------------------------
               Set the known data pattern for Sync CS Data test application.
             --------------------------------------------------------------*/  

             for ( i = 0; i < dswcsd_call_info.bytes_per_sdu; i++ ) 
             {
               (void)dswcsd_set_known_pattern(i, 
                                              (( i % DATA_PATTERN_VAL_RANGE ) + 
                                                 DATA_PATTERN_START_VAL) );
             }

             /*--------------------------------------------------------------
               The second-to-last byte in an SDU is Carriage Return.
             --------------------------------------------------------------*/

             (void)dswcsd_set_known_pattern((dswcsd_call_info.bytes_per_sdu - 2),
                                             CARRIAGE_RETURN);

             /*--------------------------------------------------------------
               The last byte in an SDU is Line Feed.
             --------------------------------------------------------------*/

             (void)dswcsd_set_known_pattern((dswcsd_call_info.bytes_per_sdu - 1),
                                             LINE_FEED);

            /*----------------------------------------------------------------
             Build and send UL SDUs to RLC.
            ----------------------------------------------------------------*/  

            dswcsd_tx_data_to_rlc();

           } /* dswcsd_sync_test_app_enabled == TRUE */
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

           break;

#ifdef FEATURE_DATA_V80

      case DSWCSD_V80_CONNECT_CALL_CMD:
           /*----------------------------------------------------------------
             V.80 call is connected
           ----------------------------------------------------------------*/

           /*----------------------------------------------------------------
             Set flag used in up link data path to know if the R 
             interface is in V.80 mode.
           ----------------------------------------------------------------*/  

           dswcsdv80_set_ul_r_iface_mode_is_v80(TRUE);
           DATA_MSG0_HIGH("CS data call using V.80 SAM");

           /*---------------------------------------------------------------
             +ES:6,,8 was issued; Record SYN1 from +ESA. If +ESA 
             was not issued, ATCOP is to default it to 0xFF.

             This is pulled from dsat_esa_export_val[].
           ---------------------------------------------------------------*/

           dswcsdv80_set_syn1((uint8)dsat_get_esa_export_val(ESA_SYN1_INDEX));
           DATA_MSG1_HIGH("V.80 SYN1 using %d",
                    (uint8)dsat_get_esa_export_val(ESA_SYN1_INDEX));

           /*----------------------------------------------------------------
             Reset the pointer to DSM item from SIO.
           ----------------------------------------------------------------*/  

           dswcsd_set_sio_dsm_item_ptr(NULL);

           /*----------------------------------------------------------------
             Reset the DSM item payload size.
           ----------------------------------------------------------------*/  

           dswcsd_set_dsm_payload_size(0);

           /*----------------------------------------------------------------
             Build and send UL SDUs to RLC.
           ----------------------------------------------------------------*/  

           dswcsd_tx_data_to_rlc();

           break;

#endif /* FEATURE_DATA_V80 */

      case DSWCSD_END_CALL_CMD:
           /*----------------------------------------------------------------
             Tear down UL.
           ----------------------------------------------------------------*/

#ifdef FEATURE_DATA_V80
           dswcsdv80_set_ul_r_iface_mode_is_v80(FALSE);
#endif /* FEATURE_DATA_V80 */

           dswcsd_ul_cleanup();
           break;

      default:
           DATA_MSG1_ERROR("Invalid CS Data UL Task command: %d", 
                      (uint8)cmd_ptr->hdr.cmd_id);
           break;
    } /* switch */

    /*-----------------------------------------------------------------------
      Free the allocated modem mem block
    -----------------------------------------------------------------------*/
    modem_mem_free((void *)cmd_ptr, MODEM_MEM_CLIENT_DATA_CRIT);

  } /* while */

} /* dswcsd_ul_process_cmds() */


/*===========================================================================

FUNCTION DSWCSD_UL_TASK

DESCRIPTION
  This is the entry point for the WCDMA Circuit-Switched Data Uplink Task. 
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
void  dswcsd_ul_task
(
  uint32 ignored
)

{
  rex_sigs_type   requested_sigs;             /* Signal mask to suspend on */
  rex_sigs_type   set_sigs;           /* Signals set upon return from wait */
  dog_report_type dswcsd_ul_dog_rpt_id = 0;
  /*----------------------------------------------------------------------- 
    We get the dswcsd_ul tcb pointer locally.
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dswcsd_ul_tcb_ptr = rcinit_lookup_rextask("dswcsd_ul"); 
  if ( RCINIT_NULL == dswcsd_ul_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for dswcsd_ul tcb returned NULL");
  }
#else
  dswcsd_ul_tcb_ptr = &dswcsd_ul_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  rcinit_handshake_startup();
#else
  /*-------------------------------------------------------------------------
    Do task initialization
  -------------------------------------------------------------------------*/
  dswcsd_ul_task_init();


#endif

   /*-------------------------------------------------------------------------
    Dog heart beat registration.
  -------------------------------------------------------------------------*/
  dswcsd_ul_dog_rpt_id = dog_hb_register_rex(
                                  (rex_sigs_type)DSWCSD_UL_DOG_HB_REPORT_SIG);

#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /*-------------------------------------------------------------------------
    Wait for the task start signal from TMC.
   -------------------------------------------------------------------------*/
   tmc_task_start();  
#endif

  /*-------------------------------------------------------------------------
    Initialize the queue for incoming SIO data.
  -------------------------------------------------------------------------*/
  dswcsd_init_from_sio_q();

  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = ( DSWCSD_UL_CMD_Q_SIG | DSWCSD_UL_TX_DATA_SIG 
                     | DSWCSD_UL_DOG_HB_REPORT_SIG
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
    set_sigs = dswcsd_ul_wait( requested_sigs );
    DATA_MSG2_LOW("req_sigs = 0x%x set_sigs = 0x%x", requested_sigs, set_sigs);

    /*-----------------------------------------------------------------------
      Check which signal is set, clear the signal, and take approriate action.
    -----------------------------------------------------------------------*/
    if( ( set_sigs & DSWCSD_UL_DOG_HB_REPORT_SIG ) != 0 )
    {
      /*---------------------------------------------------------------------
        Clear the command signal and process the command.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_ul_tcb_ptr, DSWCSD_UL_DOG_HB_REPORT_SIG );
      dog_hb_report(dswcsd_ul_dog_rpt_id); /* Play ping<->pong */
    }

    if( ( set_sigs & DSWCSD_UL_CMD_Q_SIG ) != 0 )
    {
      /*---------------------------------------------------------------------
        Clear the command signal and process the command.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_ul_tcb_ptr, DSWCSD_UL_CMD_Q_SIG );
      dswcsd_ul_process_cmds();
    }
    
    if ( ( set_sigs & DSWCSD_UL_TX_DATA_SIG ) != 0 ) 
    {
      /*---------------------------------------------------------------------
        Clear the TX_DATA signal and send data to RLC.
      ---------------------------------------------------------------------*/
      (void)rex_clr_sigs( dswcsd_ul_tcb_ptr, DSWCSD_UL_TX_DATA_SIG );
      dswcsd_tx_data_to_rlc();
    }
  } /* forever */

} /* dswcsd_ul_task() */


/*===========================================================================

FUNCTION DSWCSD_UL_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the WCDMA CS Data Uplink Task. 
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

dswcsd_cmd_type *dswcsd_ul_get_cmd_buf( void )

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

} /* dswcsd_ul_get_cmd_buf() */


/*===========================================================================

FUNCTION DSWCSD_UL_PUT_CMD

DESCRIPTION
  This function puts a command on the WCDMA CS UL Task's command queue, and
  sets the DSWCSD_UL_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling dswcsd_ul_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  dswcsd_ul_put_cmd
(
  dswcsd_cmd_type *cmd_ptr         /* Pointer to command to be sent */
)

{
  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer locally.
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  dswcsd_ul_tcb_ptr = rcinit_lookup_rextask("dswcsd_ul"); 
  if ( RCINIT_NULL == dswcsd_ul_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for dswcsd_ul tcb returned NULL");
  }
#else
  dswcsd_ul_tcb_ptr = &dswcsd_ul_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */
  if(cmd_ptr == NULL)
  {
    DATA_MSG0_ERROR("NULL ptr passed into dswcsd_ul_cmd()");
    return;
  }
  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( &dswcsd_ul_cmd_q, &cmd_ptr->hdr.link );
  (void)rex_set_sigs( dswcsd_ul_tcb_ptr, DSWCSD_UL_CMD_Q_SIG );

} /* dswcsd_ul_put_cmd() */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


