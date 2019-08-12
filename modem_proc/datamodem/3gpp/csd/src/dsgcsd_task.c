/*===========================================================================

              G C S D   P R O T O C O L   S T A C K    
                      S O U R C E   F I L E

DESCRIPTION
  This source file contains the GCSD transport protocol stack high level
  functions, task entry point, and command interface functions.  
  
  This software unit contains the task entry point and the top-level dispatch
  routines for the Data Services (DS) Task. Functions to manipulate the Data
  Services Task's command queue are also included here.

   Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsd.c_v   1.7   27 Jun 2002 16:47:42   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsd_task.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/13   pkp     Dog heart beat implementation.
05/09/12   nd      Memory reduction effort in 3gpp.
03/22/12   nd      Fixing compilation error due to TCB variables undeclared.
11/11/11   nd      Feature cleanup.
08/30/11   bvd     RCINIT : Merged all Task befor tmc_start_task into one 
                   function and added rcinit featurization
04/01/11   ttv     Support for Q6 free floating.
01/06/11   ttv     Changes made to start GCSD general purpose timer after call 
                   origination.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
06/28/10   ss      Fixed compiler warnings.
10/21/09   ss      CMI AU level cleanup.
06/29/09   BS      Fixed off-target lint medium & lows.
04/02/09   SA      Removed multiple initialization of critical section for L1 interface.
03/07/09   MS      CMI Integration fixes
06/06/08   SA      Migrated INTLOCK for V.24 modem control to Rex Critical section.
08/22/06   AR      Changes to supprot multiprocessor architecture.
08/03/07   DA      Implementation of data inactivity timer
08/14/06   AR      Clear RLP interface buffers after handover.
07/10/06   AR      Add Gcsd_curr_call_state to track call processing state.
03/09/06   AR      Add CM call ID to GCSD commands and cache locally.
02/09/06   AR      Lint corrections.
11/21/05   AR      Add support for multiple call instances.
11/30/04   AR      Preserve the existing signals when setting DTX timer.
05/05/04   AR      Set DL signal with UL for WCDMA DRX operation.
09/17/03   TMR     Added new code to support blocking of duplicate uplink data 
                   for NT calls.
09/16/03   ar      Added support for suspend/resume data transfer commands.
07/15/03   TMR     Added new REX timers for RLP V2 and WCDMA Async
04/17/03   AR      Added call to ds_ucsd_nt_pwrup_init() in task init.
                   Lint corrections.
05/21/02   at      Added watchdog support.
08/10/01   TMR     Initial release

===========================================================================*/




/*===========================================================================
Data definitions
===========================================================================*/


/*===========================================================================
Function definitions
===========================================================================*/ 

/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_3gppi_utils.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "dog_hb_rex.h"
#include "rex.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "task.h"
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "queue.h"
#include "cm.h"
#include "dstask.h"      
#include "err.h"
#include "dsgcsdi.h"
#include "dsucsdbc.h"
#include "dsucsdnti.h"
#include "dsucsdrlp.h"
#include "dsatapi.h"
#include "dsgcsdl1if.h"
#include "ds3gsiolib.h"
#include "ds_system_heap.h"
#include "rcinit_rex.h"

#ifdef FEATURE_DATA_GCSD_FAX
#include "dsfps.h"
#endif

#ifdef IMAGE_APPS_PROC
/* Fail build attempt on Apps processor as GCSD designed only for
 * Modem.  This is primarily due to GSM Fax T.31 protocol tight timing
 * constraints. */
#error "GCSD task restricted to Modem procesor builds"
#endif

/* <EJECT> */

/*===========================================================================
                  DATA DEFINITIONS
===========================================================================*/


/*---------------------------------------------------------------------------
  Data services task command queue definitions.
---------------------------------------------------------------------------*/ 

/* General purpose timer */
static rex_timer_type     Gcsd_general_purpose_timer = {NULL};

/* NT WCDMA DRX timer */
static rex_timer_type     Gcsd_nt_process_uplink_timer = {NULL};

/* GCSD Task command queue */
static q_type             Gcsd_cmd_q = {{NULL}};

/* Current data rate */
static ds_ucsd_data_rate_T      Gcsd_curr_data_rate = DS_UCSD_RATE_NONE;

/* Current Call Manager call ID */
static cm_client_id_type        Gcsd_curr_call_id = 0; 

static boolean                  Gcsd_curr_call_is_fax  = FALSE;

static boolean                  Gcsd_curr_call_is_transparent = FALSE;

/* Indicates call processing state requested by upper layers */
static ds_gcsd_call_state_type  Gcsd_curr_call_state   = GCSD_CALL_STATE_IDLE;

/* Indicates data transfer suspended (e.g. in handover) */
static boolean            Gcsd_curr_call_is_suspended     = FALSE;

static dsm_watermark_type       *Gcsd_dl_wm_ptr = NULL;

static dsm_watermark_type       *Gcsd_ul_wm_ptr = NULL;

static dsm_watermark_type       *Gcsd_fax_sio_dl_wm_ptr = NULL;

static dsm_watermark_type       *Gcsd_fax_sio_ul_wm_ptr = NULL;

/* Indicates identifier for watermark functions */
static uint32                    Gcsd_wm_context = 0;


/* Variable local to CSD for storing S30 timer value provided by ATCOP */
static uint8               Gcsd_s30_timer_val = 0;

/* Configurable UL watermark values */
static uint32 gcsd_ul_wm_lo_wm =   7 * GCSD_TPS_SIO_LO_WM_LEVEL;
static uint32 gcsd_ul_wm_hi_wm =   7 * GCSD_TPS_SIO_HI_WM_LEVEL;
static uint32 gcsd_ul_wm_dne_wm = 26 * GCSD_TPS_SIO_DONT_EXCEED_LEVEL;

/*---------------------------------------------------------------------------
  TCB pointer for Gcsd task
---------------------------------------------------------------------------*/
static rex_tcb_type* gcsd_tcb_ptr = NULL; 

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS 
            
This section contains definitions for global variable used in the GCSD
subsystem

===========================================================================*/


/* Current and previous states of the local serial port control lines */
static ds_ucsd_sio_status_T   CurrentModemControlLines;
static ds_ucsd_sio_status_T   OldModemControlLines;

/* Status of the remote (IWF) modem status lines */
static filtered_rx_status_T   Filtered_rx_status;


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* <EJECT> */                       
/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS_GCSD_INIT_CALL_DATA

DESCRIPTION
  This function initializes all data used within the GCSD task on a 
  per call basis.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/
void  
ds_gcsd_init_call_data
(
   cm_call_id_type                   call_id,
   boolean                           call_is_MO,
   sys_sys_mode_e_type               curr_mode,
   ds_ucsd_data_rate_T               data_rate,
   ds_ucsd_frame_size_T              bytes_per_sdu,                           
   uint32                            sdus_per_tti,
   const cm_bearer_capability_s_type     *bearer_cap_ptr,
   const ds_ucsd_nt_rlp_parameters_T     *rlp_params_ptr,
   ds_ucsd_set_data_counters_cb_T         set_data_counters_cb      
)
{
   ASSERT( NULL != bearer_cap_ptr );
   ASSERT( NULL != rlp_params_ptr );

   /* Initialze call processing state tracking */
   Gcsd_curr_call_state = GCSD_CALL_STATE_IDLE;
  
   Gcsd_curr_data_rate    = data_rate;

   /* Determine if call is transparent or non-transparent and do
   ** appropriate initializations
   */
   if( bearer_cap_ptr->connection_element == TRANSPARENT_DATA )
   {
#if defined(FEATURE_DATA_GCSD)

      Gcsd_curr_call_is_transparent = TRUE;
      ds_gcsd_transparent_data_services_initialise( call_id,
                                                    bearer_cap_ptr,
                                                    data_rate,
                                                    curr_mode,
                                                    set_data_counters_cb );
#endif

   }
   else /* Non-transparent */
   {
      boolean bc_compression_enabled;
      
      Gcsd_curr_call_is_transparent = FALSE;
      
      bc_compression_enabled = (bearer_cap_ptr->compression)? TRUE : FALSE;
      
      ds_ucsd_nt_init_call_data( call_id,
                                 curr_mode,
                                 bytes_per_sdu,                           
                                 sdus_per_tti,
                                 rlp_params_ptr,
                                 data_rate,
                                 bc_compression_enabled,
                                 TRUE,
                                 set_data_counters_cb );
                                
   }
   /* Stop inactivity timer in case it is active */
   
   if(TIMER_RUNNING(TIMERID_ATCOP_SREG)== TRUE)
   {
     STOP_TIMER(TIMERID_ATCOP_SREG);
     DATA_MSG0_LOW("Stopping user data inactivity timer");
   }
   
   /* Determine if this call is a FAX call or not, and again, do the
   ** appropriate initializations
   */   
#ifdef FEATURE_DATA_GCSD_FAX
   if( bearer_cap_ptr->information_transfer_capability == ITC_FAX_G3 )
   {
      Gcsd_curr_call_is_fax = TRUE;

      dsfps_call_init( call_id,
                       call_is_MO,
                       data_rate,
                       bearer_cap_ptr );
   }
   else
#endif
   {
      Gcsd_curr_call_is_fax = FALSE;
   }
}

/* <EJECT> */
/*===========================================================================

FUNCTION DS_GCSD_INIT_S30_TIMER_VALUE

DESCRIPTION
  This function initializes the S30 timer value store that is global to GCSD.

DEPENDENCIES
  S30 value is provided by ATCOP.

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/

void ds_gcsd_init_s30_timer_value(void)
{
  int32 S30_val = 0;

  S30_val = dsat_get_sreg_val(DS_GCSD_S30_REG);

  if(S30_val != DS_GCSD_INVALID_SREG_VALUE)
  {
    /* Initialise local store in GCSD for user data inactivity timer
       on returning to online data mode (10sec units) */
    Gcsd_s30_timer_val = (uint8)S30_val;
    DATA_MSG1_HIGH("User data inactivity timer: %d sec",
             (Gcsd_s30_timer_val*10));
  }
  else
  {
    Gcsd_s30_timer_val = 0;
    DS_UCSD_DEBUG_MSG ("S30 read failure");
  }
} /* ds_gcsd_init_s30_timer_value () */


/*===========================================================================

FUNCTION DS_GCSD_READ_S30_TIMER_VALUE

DESCRIPTION
  This function reads S30 timer value from a store that is global to GCSD.

DEPENDENCIES
  None.

RETURN VALUE
  S30 register value

SIDE EFFECTS
  None

===========================================================================*/

uint8 ds_gcsd_read_s30_timer_value(void)
{
  return Gcsd_s30_timer_val;
} /* uint8 ds_gcsd_read_s30_timer_value() */


/*===========================================================================

FUNCTION DS_GCSD_S30_TIMER_CONTROL

DESCRIPTION
  This function handles S30 data inactivity timer start, stop, and expiry. 

  Start Timer: if zero UL or DL user data byte count is detected.
  Stop Timer : if non-zero UL or DL user data byte count is detected.
  On timer Expiry : Terminate call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  CSD call is terminated on data inactivity timer expiry.

===========================================================================*/

void ds_gcsd_s30_timer_control(uint32 data_byte_count)
{
   /* Check status of inactivity timer and data transferred */
   if(TIMER_EXPIRED(TIMERID_ATCOP_SREG))
   {
     DATA_MSG0_HIGH("User data inactivity timer expired");
     ds_gcsd_send_event_to_DS(DS_UMTS_CSD_TERMINATE_CALL_IND,
                              Gcsd_curr_call_id );
   }
   else if ((data_byte_count == 0) && 
            (TIMER_RUNNING(TIMERID_ATCOP_SREG) != TRUE ) &&
            (Gcsd_curr_call_state == GCSD_CALL_STATE_CONNECT))
   {
     if(ds_gcsd_read_s30_timer_value() != 0)
     {
       /* S30 value is in tens of seconds */
       START_TIMER(TIMERID_ATCOP_SREG,
                   (ds_gcsd_read_s30_timer_value()*TIMER_COUNT_1_SEC*10));
       DATA_MSG0_LOW("User data inactivity timer started");
     }
   }
   else if ((data_byte_count != 0) && 
            (TIMER_RUNNING(TIMERID_ATCOP_SREG) == TRUE))
   {
     STOP_TIMER(TIMERID_ATCOP_SREG);
     DATA_MSG0_LOW("User data inactivity timer stopped");
   }
} /* ds_gcsd_s30_timer_control( )*/


/* <EJECT> */
/*===========================================================================

FUNCTION DS_GCSD_INIT_WATERMARKS

DESCRIPTION
  This function specifies and initializes the watermarks which are to
  be used for GCSD call.  Parameters may be NULL to deregister watermarks.

DEPENDENCIES
  Must be called after ds_gcsd_init_call_data().

RETURN VALUE
  None

SIDE EFFECTS
  Sets GCSD global data

===========================================================================*/
void
ds_gcsd_init_watermarks
(
  const uint32                        call_id,
  const ds_ucsd_call_type             call_type,
  dsm_watermark_type                 *gcsd_dl_wm_ptr,
  dsm_watermark_type                 *gcsd_ul_wm_ptr,
  dsm_watermark_type                 *fax_sio_dl_wm_ptr,
  dsm_watermark_type                 *fax_sio_ul_wm_ptr   
)
{
   /* Set the global watermark pointers up.  Note that these pointers
   ** are accessed in some of the initialization routines, so this
   ** should be done first
   */
   Gcsd_dl_wm_ptr         = gcsd_dl_wm_ptr;
   Gcsd_ul_wm_ptr         = gcsd_ul_wm_ptr;
   Gcsd_fax_sio_dl_wm_ptr = fax_sio_dl_wm_ptr;
   Gcsd_fax_sio_ul_wm_ptr = fax_sio_ul_wm_ptr;
   Gcsd_wm_context        = call_id;
   
   /* Initialize the protocol stack downlink watermark pointers */
   if( NULL != gcsd_dl_wm_ptr )
   {
     gcsd_dl_wm_ptr->lo_watermark        = GCSD_TPS_SIO_LO_WM_LEVEL;
     gcsd_dl_wm_ptr->hi_watermark        = GCSD_TPS_SIO_HI_WM_LEVEL;
     gcsd_dl_wm_ptr->dont_exceed_cnt     = GCSD_TPS_SIO_DONT_EXCEED_LEVEL;
     gcsd_dl_wm_ptr->hiwater_func_ptr    = NULL;
     gcsd_dl_wm_ptr->lowater_func_ptr    = NULL;
   }
   
   /* Initialize the protocol stack uplink watermarks */
   if( NULL != gcsd_ul_wm_ptr )
   {
     gcsd_ul_wm_ptr->lo_watermark      = gcsd_ul_wm_lo_wm;
     gcsd_ul_wm_ptr->hi_watermark      = gcsd_ul_wm_hi_wm;
     gcsd_ul_wm_ptr->dont_exceed_cnt   = gcsd_ul_wm_dne_wm;
   }
   
   /* Complete initialization based on call type */   
#ifdef FEATURE_DATA_GCSD_FAX
   if( IS_CALL_TYPE( call_type, DS_UCSD_FAX_CALL ) ) /*lint !e655 */
   {
     Gcsd_curr_call_is_fax = TRUE;
   
     if( NULL != gcsd_ul_wm_ptr )
     {
       gcsd_ul_wm_ptr->hiwater_func_ptr  = NULL;
       gcsd_ul_wm_ptr->lowater_func_ptr  = NULL;
     }
      
     /* Initiaze the fax adapter watermarks */
     dsfps_wm_init( gcsd_dl_wm_ptr,
                    gcsd_ul_wm_ptr,
                    fax_sio_dl_wm_ptr,
                    fax_sio_ul_wm_ptr );
   }
   else
#endif /* FEATURE_DATA_GCSD_FAX */
   {
     /* Install UL flow control management */
     if( NULL != gcsd_ul_wm_ptr )
     {
       gcsd_ul_wm_ptr->hiwater_func_ptr  = ds_gcsd_sio_high_wm_handler;
       gcsd_ul_wm_ptr->lowater_func_ptr  = ds_gcsd_sio_low_wm_handler;
     }
   }
}
   
/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_get_cs_data_event

DESCRIPTION
  This function translates any input command to a GCSD internal event.  If
  no command is input, internal events are grabbed from the internal 
  event queues.

DEPENDENCIES
  None

RETURN VALUE
  Event to process

SIDE EFFECTS
  None

===========================================================================*/
LOCAL 
DSI_Event_T
ds_gcsd_get_cs_data_event
(
   const ds_gcsd_cmd_type *cmd
)
{
   DSI_Event_T event = (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT;
   
   /* If we have command data, translate it into a CS protocol stack event */
   if (cmd != NULL)
   {
      /* Update current call ID if specified. Captured for every event
       * incase lower layers have not been initialized yet. */
      if( CM_CALL_ID_INVALID != cmd->cmd.call_id )
      {
        Gcsd_curr_call_id = cmd->cmd.call_id;
      }
     
      switch (cmd->hdr.cmd_id)
      {
      
         /* Note that this will result in a DS_UMTS_CSD_CONNENCT_COMPLETE_IND
         ** being generated when the connect has finished
         */
         case DS_GCSD_CONNECT_MO_REQ:
         case DS_GCSD_CONNECT_MT_REQ:
            event = (DSI_Event_T)DS_UMTS_CSD_CONNECT_EVENT;
            /* Initialise user data inactivity timer */
            ds_gcsd_init_s30_timer_value();
            break;
         
         /* Always flush transmit data before disconnecting a MO disconnect 
         ** Note that this will result in a DS_UMTS_CSD_DISCONNECT_COMPLETE_IND
         ** being generated when the data is flushed
         */   
         case DS_GCSD_DISCONNECT_MO_REQ:
            event = (DSI_Event_T)DS_UMTS_CSD_DISCONNECT_AFTER_FLUSH_EVENT;
            break;
            
         case DS_GCSD_DISCONNECT_MT_REQ:
            event = (DSI_Event_T)DS_UMTS_CSD_DISCONNECT_EVENT;
            break;
            
         case DS_GCSD_CMD_MODE_REQ:
            event = (DSI_Event_T)DS_UMTS_CSD_ONLINE_CMD_MODE_EVENT;
            break;
            
         case DS_GCSD_RECONNECT_REQ:
            event = (DSI_Event_T)DS_UMTS_CSD_ONLINE_DATA_MODE_EVENT;
            /* Reinitialise user data inactivity timer */
            ds_gcsd_init_s30_timer_value();
            break;
         
 #ifdef FEATURE_DATA_GCSD_FAX
         /* These are fax related and should not affect TDATA processing */
         case DS_GCSD_T31_AT_FTH_REQ:
         case DS_GCSD_T31_AT_FRH_REQ:
         case DS_GCSD_T31_AT_FTM_REQ:
         case DS_GCSD_T31_AT_FRM_REQ:
         case DS_GCSD_T31_AT_FTS_REQ:
         case DS_GCSD_T31_AT_FRS_REQ:
            break;
#endif

         /* No translation for following events; handled elsewhere */
         case DS_GCSD_SUSPEND_DATA_REQ:
         case DS_GCSD_RESUME_DATA_REQ:
            break;

         /* Should never happen */  
         default:
            DATA_MSG1_ERROR("GCSD received illegal message ",cmd->hdr.cmd_id);
            break;
      }
   }
   return event;
}
      
/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_process_cs_data

DESCRIPTION
  This is the high level CS data protocol stack
  processing function.  It uses input commands
  as well as process uplink and process downling indications to process
  CS data.

DEPENDENCIES
  ds_gcsd_init_call_data() must have been called before starting
  call procesing.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_process_cs_data 
(
   boolean                 process_uplink,   /* Perform uplink processing? */
   
   boolean                 process_downlink, /* Perform downlink processing? */
   
   const ds_gcsd_cmd_type *cmd               /* Pointer to command data
                                             ** possibly NULL
                                             */
)
{
   DSI_Event_T event;

   /* Get state machine event */
   event = ds_gcsd_get_cs_data_event (cmd);
   
   /* Process events */
   if (Gcsd_curr_call_is_transparent)
   {
   
#ifdef DS_UCSD_DEBUG
      if (event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
      {
        DS_UCSD_DEBUG_MSG_2 ("GCSD Task T-DATA Event = %d  CallID = %d",
                             event, Gcsd_curr_call_id);
      }
#endif

#if defined(FEATURE_DATA_GCSD)
      /* Do transparent data processing */
      ds_gcsd_control_transparent_data_services( event,
                                                 (uint8)Gcsd_curr_call_id,
                                                 process_uplink,
                                                 process_downlink );
#endif

   }
   else
   {
#ifdef DS_UCSD_DEBUG
      if (event != (DSI_Event_T)DS_UMTS_CSD_NULL_EVENT)
      {
        DS_UCSD_DEBUG_MSG_2 ("GCSD Task NT-DATA Event = %d  CallID = %d",
                             event, Gcsd_curr_call_id);
      }
#endif
      /* Do NT data processing */
      ds_ucsd_nt_process_data( (ds_ucsd_event_T)event,
                               (uint8)Gcsd_curr_call_id,
                               process_uplink,
                               process_downlink );
   }
}

 
/* <EJECT> */
/*===========================================================================

FUNCTION ds_gcsd_CheckModemControlLines

DESCRIPTION
  Checks the input modem lines and triggers events.  At this point, this
  function no longer does so, but is left around just in case we decide
  to support BREAK character at some point in the future.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Copies over OldModemControlLines global variable.

===========================================================================*/
void 
ds_gcsd_CheckModemControlLines (void) 

{
   /* IF Break from DTE */

/*    if ((CurrentModemControlLines.Break == V24_CONTROL_ON) &&  */
/*        (OldModemControlLines.Break     == V24_CONTROL_OFF))   */
/*    {                                                          */
/*       WRITE_EVENT_TO_L2R (L2R_DTE_BREAK_IND);                 */
/*    }                                                          */
   
/*    / * Check the action of the DTR line:                     */
/*     * If AT&D0 then ignore it.                               */
/*     * If AT&D1 then return to command mode.                  */
/*     * If AT&D2 then quit call.                               */
/*     * /                                                      */
/*                                                              */
/*    if ((CurrentModemControlLines.DTR == V24_CONTROL_OFF) &&  */
/*        (OldModemControlLines.DTR     == V24_CONTROL_ON)  &&  */
/*        (ds_isr_config_data->ampersand_d==1))                 */
/*    {                                                         */
/* #if defined LOGDEBUG                                         */
/*       Log ("L2R: DTR Low: +++ ind\n");                       */
/* #endif                                                       */
/*       WRITE_EVENT_TO_L2R (L2R_DTE_PPP_IND);                  */
/*    }                                                         */
/*                                                              */
/*    if ((CurrentModemControlLines.DTR == V24_CONTROL_OFF) &&  */
/*        (OldModemControlLines.DTR     == V24_CONTROL_ON)  &&  */
/*        (ds_isr_config_data->ampersand_d==2))                 */
/*    {                                                         */
/* #if defined LOGDEBUG                                         */
/*       Log ("L2R: DTR Low\n");                                */
/* #endif                                                       */
/*       WRITE_EVENT_TO_L2R (L2R_DTE_DTR_LOW_IND);              */
/*    }                                                         */


   /* Save the state of the input modem lines */
   OldModemControlLines = CurrentModemControlLines;
}



/* <EJECT> */
/*===========================================================================

FUNCTIONS ds_gcsd_send_event_to_DS

DESCRIPTION
   Send an event from the GCSD protocol stack to the GCSD Call control
   subsystem.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_send_event_to_DS 
(
  ds_ucsd_ind_T indication,
  uint8         call_id
)
{
   ds_cmd_enum_type        cmd_id;
   boolean                 send_cmd = TRUE;
   ds_cmd_type             *cmd_ptr = NULL;
   uint8                   *data_ptr = NULL;
  
   /* These events indicate that the call is terminated.  Reset
   ** SIO if this is the case
   */ 
   switch (indication)
   {
      case DS_UMTS_CSD_DISCONNECT_COMPLETE_IND:
      case DS_UMTS_CSD_TERMINATE_CALL_IND:
     
         ds_gcsd_V24_reset();
         
         if(TIMER_RUNNING(TIMERID_ATCOP_SREG))
         {
           STOP_TIMER(TIMERID_ATCOP_SREG);
           DATA_MSG0_LOW("Stopping user data inactivity timer");
         }
      break;
      
      default:
      break;
   }
         
   /* Send event to CC task if necessary 
   */
   switch (indication)
   {
      case DS_UMTS_CSD_TERMINATE_CALL_IND: 
         cmd_id = DS_CMD_3GPP_GCSD_DISCONNECT; 
         Gcsd_curr_call_state = GCSD_CALL_STATE_IDLE;
      break;
      
      case DS_UMTS_CSD_DISCONNECT_COMPLETE_IND:  
         cmd_id = DS_CMD_3GPP_GCSD_TX_DATA_FLUSHED; 
         Gcsd_curr_call_state = GCSD_CALL_STATE_IDLE;
      break;

      case DS_UMTS_CSD_CONNENCT_COMPLETE_IND:
         cmd_id = DS_CMD_3GPP_GCSD_RX_DCD_ON;

         // If a FAX call, wait until we get the FA SYNC indication
         // before we indicate to call control that we are connected
         if (Gcsd_curr_call_is_fax)
         {
            send_cmd = FALSE;
         }         
      
      break;
         
      default:
         cmd_id = DS_MIN_CMD;       // Surpresses compiler warning
         send_cmd = FALSE;
      break;
   }
   
   if (send_cmd)
   {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(uint8));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
         ASSERT(0);
         return;
      }

      cmd_ptr->hdr.cmd_id = cmd_id;
      data_ptr = (uint8*)cmd_ptr->cmd_payload_ptr;

      *data_ptr = call_id;

      ds_put_cmd (cmd_ptr);
   }
}


/*===========================================================================

FUNCTIONS get_gcsd_tcb_ptr

DESCRIPTION:
    This callback function is called to get gcsd task pointer.      

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to gcsd tcb (rex_tcb_type*)

SIDE EFFECTS
  None

===========================================================================*/
rex_tcb_type* get_gcsd_tcb_ptr( void )
{
  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer locally.
  ----------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
  if ( RCINIT_NULL == gcsd_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for gcsd tcb returned NULL");
  }
#else
  gcsd_tcb_ptr = &Gcsd_tcb;
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  return gcsd_tcb_ptr;
}

/* <EJECT> */
/*===========================================================================

FUNCTIONS ds_gcsd_no_wcdma_nt_data_sent

DESCRIPTION:
    This callback function is called by the NT stack if we
    are in WCDMA mode and duplicate TX frame suppression is
    enabled.

    If we are doing uplink processing, but no uplink data was
    transmitted (due to supression of duplicate data in WCDMA mode),
    then set a timer to cause uplink and downlink processing in the in
    the future.  In WCDMA mode, the current implementation uses a DSM
    watermarks' gone_empty callback function to send the uplink
    processing signal.  This will not be called since no data was
    transmitted, so this timer will be used instead.  Similiarly, if
    no packets are received from RRC, no downlink processing will
    occur yet packets may in NT queues.  The timeout value of this
    timer should be more frequent than the lowest TTI used (40 msec.)
    but not too frequent to keep processing overhead low.  We will use
    20 msec.
      

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_no_wcdma_nt_data_sent (void)
{
  rex_sigs_type saved_sigs;
  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer and store it locally
  ----------------------------------------------------------------------*/
  gcsd_tcb_ptr = get_gcsd_tcb_ptr();
  /* Preserve the existing signals as some are reset in REX call */
  saved_sigs = rex_get_sigs ( gcsd_tcb_ptr );

  (void) rex_set_timer( &Gcsd_nt_process_uplink_timer, 20 );

  /* Restore saved signals */
  (void) rex_set_sigs ( gcsd_tcb_ptr, saved_sigs );
}

   

/* <EJECT> */
/*===========================================================================

FUNCTIONS Event Queue Routines

DESCRIPTION
   Routines used for internal GCSD transport protocol stack events
   communication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_DATA_GCSD_FAX)
void 
ds_gcsd_report_fax_connect
(
  cm_call_id_type   call_id
) 
{
   ds_cmd_type *cmd_ptr = NULL;
   uint8       *data_ptr = NULL;

   cmd_ptr = ds_allocate_cmd_buf(sizeof(uint8));
   if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
   {
      ASSERT(0);
      return;
   }

   cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_GCSD_RX_DCD_ON;
   data_ptr = (uint8*)cmd_ptr->cmd_payload_ptr;

   *data_ptr = call_id;
   ds_put_cmd (cmd_ptr);

   DS_UCSD_DEBUG_MSG ("GCSD Task: report FAX CONNECT");
}
   
#endif
 
/*===========================================================================

FUNCTION DS_GCSD_L1_BUFF_INIT

DESCRIPTION
  This function initializes the task critical section for L1 interface.
  It is called once during powerup.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_l1_buff_init( void )
{
  rex_crit_sect_type *ds_gcsd_l1_rx_buf_crit_sect;
  rex_crit_sect_type *ds_gcsd_l1_tx_buf_crit_sect;
  
  ds_gcsd_l1_rx_buf_crit_sect = 
     ds_gcsd_get_l1_rx_buf_crit_sect_ptr();

  ds_gcsd_l1_tx_buf_crit_sect = 
     ds_gcsd_get_l1_tx_buf_crit_sect_ptr();
  memset(ds_gcsd_l1_rx_buf_crit_sect, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(ds_gcsd_l1_rx_buf_crit_sect);
  
  memset(ds_gcsd_l1_tx_buf_crit_sect, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect (ds_gcsd_l1_tx_buf_crit_sect);

} /* ds_gcsd_l1_buff_init */


/*===========================================================================

FUNCTION DS_GCSD_TASK_INIT

DESCRIPTION
  This function performs all the power-up initialization for the Data
  Services Task.

DEPENDENCIES
  None

RETURN VALUE
  Returns a signal mask for each sub-task, containing the signals that the
  sub-task wishes to suspend on.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gcsd_task_init (void)
{

   /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
   -------------------------------------------------------------------------*/  
    memset(&Gcsd_cmd_q, 0, sizeof(q_type));
   (void)q_init( &Gcsd_cmd_q );

   /* Initialise the timers */
   ds_ucsd_timer_init();
   
   /* Initialize V24 modem control critical section */
   ds_gcsd_sio_init();

   /* Initialize L1 layer interface critical section */
   ds_gcsd_l1_buff_init();

   /* Do non-transparent power-up initialization */
   ds_ucsd_nt_pwrup_init ();
   
   ds_ucsd_nt_register_ind_report_func
   (
      ds_gcsd_send_event_to_DS   
   );
   
   ds_ucsd_nt_register_no_tx_frames_sent_func
   (
      ds_gcsd_no_wcdma_nt_data_sent
   );

   ds_ucsd_nt_register_sio_funcs
   (
      ds_gcsd_read_from_serial_port,
      ds_gcsd_write_to_serial_port,
      ds_gcsd_update_serial_status,
      ds_gcsd_set_cts_state,
      ds_gcsd_ota_rx_buffer_full,
      ds_gcsd_suspend_data_to_sio,
      ds_gcsd_V24_reset
   );
   
} /* ds_gcsd_task_init() */



/*===========================================================================

FUNCTION ds_gcsd_process_cmds

DESCRIPTION
  This function performs command processing of commands to the GCSD
  protocol stack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_process_cmds
(
   ds_gcsd_cmd_type   *cmd
)

{
  /* Process command */
  switch (cmd->hdr.cmd_id)
  {
  case DS_GCSD_SUSPEND_DATA_REQ:
    Gcsd_curr_call_is_suspended = TRUE;
    break;
   
  case DS_GCSD_RESUME_DATA_REQ:
    /* Clear the RLP/LL interface Rx buffers */
    ds_HardwareInitialise( FALSE, TRUE );
    
    Gcsd_curr_call_is_suspended = FALSE;
    break;
   
  default:
#if defined(FEATURE_DATA_GCSD_FAX)
    if (Gcsd_curr_call_is_fax)
    {
      dsfps_process_event (cmd);
    }
#endif
    break;
  }
}



/*===========================================================================

FUNCTION ds_gcsd_process_timers

DESCRIPTION
  This function processes REX timer expiries

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_process_timers (void)
{
   (void) rex_set_timer( &Gcsd_general_purpose_timer, DS_GCSD_TIMER_MSEC_PER_TICK );
   ds_ucsd_timer_tick();
}



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS_GCSD_TASK

DESCRIPTION
  This is the entry point for the GCSD Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void  ds_gcsd_task
(
  dword ignored
    /* lint -esym(715,ignored) */
)
{
   rex_sigs_type   sigs,
                   wait_sigs;      /* Signals set upon return from wait */

#ifdef FEATURE_DATA_GCSD_FAX
   rex_sigs_type   fps_sigs = 0;   /* Signals used by the FPS */
#endif

   ds_gcsd_cmd_type *cmd_ptr;      /* Pointer to command buffer         */
   
   boolean         process_uplink, 
                   process_downlink;
  dog_report_type  gcsd_dog_rpt_id = 0;

  /*----------------------------------------------------------------------- 
    We get the gcsd tcb pointer and store it locally
  ----------------------------------------------------------------------*/
  gcsd_tcb_ptr = get_gcsd_tcb_ptr();
                   
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  rcinit_handshake_startup();
#else
   /*-------------------------------------------------------------------------
    Do task initialization. The init function performs all task initialization.
    -------------------------------------------------------------------------*/
   ds_gcsd_task_init();
   
#endif
   
   /*-------------------------------------------------------------------------
     Dog heart beat registration.
   -------------------------------------------------------------------------*/
  gcsd_dog_rpt_id = dog_hb_register_rex(
                      (rex_sigs_type)DS_GCSD_DOG_HB_REPORT_SIG);
					  
   /* General purpose timer */
   rex_def_timer( &Gcsd_general_purpose_timer, gcsd_tcb_ptr, DS_GCSD_TIMER_SIG );

   /* Timer used to poke the NT data uplink and downlink when in WCDMA
   ** call and duplicate blocking is enabled, and no data was
   ** transmitted due to being duplicate (sorry about the run-on
   ** sentence... ).
   */
   rex_def_timer( &Gcsd_nt_process_uplink_timer, gcsd_tcb_ptr,
                  (DS_GCSD_PROC_UL_SIG | DS_GCSD_PROC_DL_SIG)  );

#ifdef FEATURE_DATA_GCSD_FAX
   /*-------------------------------------------------------------------------
    Initialize the Fax Protocol Stack and get the signals to monitor for it.
   -------------------------------------------------------------------------*/
   fps_sigs = dsfps_powerup_init();
#endif
   
#ifndef FEATURE_MODEM_RCINIT_PHASE2
  /*-------------------------------------------------------------------------
    Wait for the task start signal from TMC.
   -------------------------------------------------------------------------*/
   tmc_task_start();  
#endif
   

   /*-------------------------------------------------------------------------
    Main task loop, never exits.
   -------------------------------------------------------------------------*/
   for( ;; )
   {
      
      /* Initialize the cmd/uplink/downlink locals */
      process_uplink = process_downlink = FALSE;
      cmd_ptr = NULL;
      
      /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set and then clear out
      the received signals.
      -----------------------------------------------------------------------*/
      wait_sigs = DS_GCSD_CMD_Q_SIG |
                  DS_GCSD_DOG_HB_REPORT_SIG |
                  DS_GCSD_TIMER_SIG |
                  DS_GCSD_PROC_UL_SIG |
                  DS_GCSD_PROC_DL_SIG;

#ifdef FEATURE_DATA_GCSD_FAX
      wait_sigs |= fps_sigs;
#endif

      sigs = rex_wait (wait_sigs);

      /* The code in this loop can only process one of uplink/downlink signal at
      ** at time (because of the ordering in the fax case).  If both signals
      ** are set, the uplink will take priority, so we will unset the downlink
      ** signal in the sigs variable, so it will not be processed or cleared
      ** and will be handled in a subsequent run of this loop
      */
      if (sigs & DS_GCSD_PROC_UL_SIG)
      {
         sigs &= ~DS_GCSD_PROC_DL_SIG;
      } 


      /* Clear out the received signals */
      (void)rex_clr_sigs( rex_self(), sigs );
    

      /* Process dog heart beat and ack it back. Play ping<->pong */
      if (sigs & DS_GCSD_DOG_HB_REPORT_SIG)
      {
        dog_hb_report ( gcsd_dog_rpt_id );
      }
      
      /* Process any timer expiries */
      if (sigs & DS_GCSD_TIMER_SIG)
      {
         ds_gcsd_process_timers();
      }

#ifdef FEATURE_DATA_GCSD_FAX
      /* Process any FPS signals */
      if (sigs & fps_sigs)
      {
        dsfps_process_signals(sigs);
      }
#endif

      /* Set the cmd_ptr local if we have commands to process */
      if (sigs & DS_GCSD_CMD_Q_SIG)
      {
         cmd_ptr = (ds_gcsd_cmd_type *) q_get (&Gcsd_cmd_q);
      }
      
      /* Set uplink/downlink flags if necessary */
      if (sigs & DS_GCSD_PROC_UL_SIG)
      {
         process_uplink = (Gcsd_curr_call_is_suspended)? FALSE : TRUE;
      }
      
      else if (sigs & DS_GCSD_PROC_DL_SIG)
      {
         process_downlink = (Gcsd_curr_call_is_suspended)? FALSE : TRUE;
      }
           
      /* Now perform command/uplink/downlink processing */
      if (cmd_ptr || process_uplink || process_downlink)
      {
         do
         {
         
            /* Do any required command processing first */
            if (cmd_ptr)
            {
               DS_UCSD_DEBUG_MSG_1 ("GCSD Task CMD ID = %d",cmd_ptr->hdr.cmd_id);
               ds_gcsd_process_cmds(cmd_ptr);
            }
            
#if defined(FEATURE_DATA_GCSD_FAX)
            /* If this is a fax call */
            if (Gcsd_curr_call_is_fax)
            {
               /* Do fax processing first, then protocol stack */
               if (process_uplink)
               {
                  dsfps_process_uplink();
                  
                  ds_gcsd_process_cs_data (TRUE,
                                           FALSE,
                                           cmd_ptr);
               }
               
               /* Do protocol stack processing first, then fax */
               else if (process_downlink)
               {
                  
                  ds_gcsd_process_cs_data (FALSE,
                                           TRUE,
                                           cmd_ptr);
                  dsfps_process_downlink();
               }
               
               /* If there isn't uplink/downlink processing to do, still call
               ** the process_cs_data function since commands are input to the 
               ** protocol stack state machine and need to be looked at by it. 
               */
               else
               {
                  ds_gcsd_process_cs_data (FALSE,
                                           FALSE,
                                           cmd_ptr);
               }
            }
            
            /* Not a fax call, just process CS data */
            else 
#endif /* FEATURE_DATA_GCSD_FAX */
            {
               ds_gcsd_process_cs_data (process_uplink,
                                        process_downlink,
                                        cmd_ptr);
            }
            
            /* Set process uplink/downlink indicators to false */
            process_uplink = process_downlink = FALSE;
            
            /* Free the cmd_ptr memory */
            if (cmd_ptr)
            {
               ds_system_heap_mem_free_crit(cmd_ptr);
            }
          
         /* Keep processing commands as long as they are available */  
         } while ((cmd_ptr = (ds_gcsd_cmd_type *) q_get (&Gcsd_cmd_q)) != NULL);
         
         
      } /* if (cmd | process_uplink | process_downlink) */
   } /* for (;;)  */ 
} /* ds_gcsd_task() */




/*===========================================================================

FUNCTION DS_GCSD_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the GCSD Protocol
  Stack task. The
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or DS_3GPP_MSG3_ERROR, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

ds_gcsd_cmd_type*
ds_gcsd_get_cmd_buf( void )
{
   ds_gcsd_cmd_type    *cmd_ptr = NULL;                /* Pointer to command */

   /*-------------------------------------------------------------------------
    Allocate a new command buffer.
   -------------------------------------------------------------------------*/
   cmd_ptr = 
    (ds_gcsd_cmd_type *)ds_system_heap_mem_alloc_crit(sizeof(ds_gcsd_cmd_type));
   if(cmd_ptr == NULL)
   {
     DS_UCSD_DEBUG_MSG_2("Memory allocation Failure for client:%d, Size:%d",
                          MODEM_MEM_CLIENT_DATA_CRIT, sizeof(ds_gcsd_cmd_type));
   }
   else
   {
     /* Initialize CM call ID to known value */
     cmd_ptr->cmd.call_id = CM_CALL_ID_INVALID;
   }
   /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
   -------------------------------------------------------------------------*/
   return( cmd_ptr );

} /* ds_gcsd_get_cmd_buf() */


/*===========================================================================

FUNCTION DS_GCSD_PUT_CMD

DESCRIPTION
  This function puts a command on the GCSD Protocol
  Stack task's command queue, and
  sets the DS_GCSD_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling ds_gcsd_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  
ds_gcsd_put_cmd
(
   ds_gcsd_cmd_type    *cmd_ptr            /* Pointer to command to be sent */
)
{

  /*-------------------------------------------------------------------------
    Capture the call processing state of upper layer.  Used to manage
    interaction with other subsystem components (SIOLIB).
  -------------------------------------------------------------------------*/
  switch (cmd_ptr->hdr.cmd_id)
  {
  case DS_GCSD_CONNECT_MO_REQ:
  case DS_GCSD_CONNECT_MT_REQ:
    Gcsd_curr_call_state = GCSD_CALL_STATE_CONNECT;
    break;
         
  case DS_GCSD_DISCONNECT_MO_REQ:
  case DS_GCSD_DISCONNECT_MT_REQ:
    Gcsd_curr_call_state = GCSD_CALL_STATE_DISCONNECT;
    break;
    
  default:
    /* Do nothing */
    break;
  }


   /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
   -------------------------------------------------------------------------*/
   q_put( &Gcsd_cmd_q, &cmd_ptr->hdr.link );
   (void)rex_set_sigs( get_gcsd_tcb_ptr(), DS_GCSD_CMD_Q_SIG );

} /* ds_gcsd_put_cmd() */

/*===========================================================================

FUNCTION DS_GCSD_START_GENERAL_PURPOSE_TIMER

DESCRIPTION
  This function starts the Gcsd_general_purpose_timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_start_general_purpose_timer (void)
{
  (void) rex_set_timer( &Gcsd_general_purpose_timer, 
                        DS_GCSD_TIMER_MSEC_PER_TICK );
}

/*===========================================================================

FUNCTION DS_GCSD_STOP_GENERAL_PURPOSE_TIMER

DESCRIPTION
  This function stops the Gcsd_general_purpose_timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
ds_gcsd_stop_general_purpose_timer (void)
{
  (void) rex_clr_timer( &Gcsd_general_purpose_timer);
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_DATA_RATE

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_data_rate.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_data_rate_T - value of the Gcsd_curr_data_rate variable.

SIDE EFFECTS
  None

===========================================================================*/
ds_ucsd_data_rate_T
ds_gcsd_get_Gcsd_curr_data_rate(void)
{
  return Gcsd_curr_data_rate;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_CALL_IS_FAX

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_call_is_fax.

DEPENDENCIES
  None

RETURN VALUE
  boolean - value of the Gcsd_curr_call_is_fax variable.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_gcsd_get_Gcsd_curr_call_is_fax(void)
{
  return Gcsd_curr_call_is_fax;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_CURR_CALL_STATE

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_curr_call_state.

DEPENDENCIES
  None

RETURN VALUE
  ds_gcsd_call_state_type - value of the Gcsd_curr_call_state variable.

SIDE EFFECTS
  None

===========================================================================*/
ds_gcsd_call_state_type 
ds_gcsd_get_Gcsd_curr_call_state(void)
{
  return Gcsd_curr_call_state;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_DL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_dl_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_dl_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* 
ds_gcsd_get_Gcsd_dl_wm_ptr(void)
{
  return Gcsd_dl_wm_ptr;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_UL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_ul_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_ul_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* 
ds_gcsd_get_Gcsd_ul_wm_ptr(void)
{
  return Gcsd_ul_wm_ptr;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_FAX_SIO_DL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_fax_sio_dl_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_fax_sio_dl_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* 
ds_gcsd_get_Gcsd_fax_sio_dl_wm_ptr(void)
{
  return Gcsd_fax_sio_dl_wm_ptr;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_FAX_SIO_UL_WM_PTR

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_fax_sio_ul_wm_ptr.

DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type* - value of the Gcsd_fax_sio_ul_wm_ptr variable.

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type* 
ds_gcsd_get_Gcsd_fax_sio_ul_wm_ptr(void)
{
  return Gcsd_fax_sio_ul_wm_ptr;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_GCSD_WM_CONTEXT

DESCRIPTION
  This function turns the value of the global variable
  Gcsd_wm_context.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - value of the Gcsd_wm_context variable.

SIDE EFFECTS
  None

===========================================================================*/
uint32 
ds_gcsd_get_Gcsd_wm_context(void)
{
  return Gcsd_wm_context;
}

/*===========================================================================

FUNCTION DS_GCSD_GET_CURRENT_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  CurrentModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_sio_status_T* - Pointer of CurrentModemControlLines variable

SIDE EFFECTS
  None

===========================================================================*/

ds_ucsd_sio_status_T* 
ds_gcsd_get_CurrentModemControlLines(void)
{
  return &CurrentModemControlLines;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_OLD_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  OldModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  ds_ucsd_sio_status_T* - Pointer of OldModemControlLines variable

SIDE EFFECTS
  None

===========================================================================*/

ds_ucsd_sio_status_T* 
ds_gcsd_get_OldModemControlLines(void)
{
  return &OldModemControlLines;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_FILTERED_RX_STATUS

DESCRIPTION
  This function turns the value of the global variable
  Filtered_rx_status.

DEPENDENCIES
  None

RETURN VALUE
  filtered_rx_status_T* - Pointer of Filtered_rx_status variable

SIDE EFFECTS
  None

===========================================================================*/
filtered_rx_status_T* 
ds_gcsd_get_Filtered_rx_status(void)
{
  return &Filtered_rx_status;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_CURRENTMODEMCONTROLLINES

DESCRIPTION
  This function turns the value of the global variable
  CurrentModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ds_gcsd_set_CurrentModemControlLines
(
  ds_ucsd_sio_status_T   *CurrentModemControlLines_ptr
)
{
  if(CurrentModemControlLines_ptr == NULL)
  {
    return;
  }
  
  CurrentModemControlLines.DTR = CurrentModemControlLines_ptr->DTR;
  CurrentModemControlLines.RTS = CurrentModemControlLines_ptr->RTS;
}

/*===========================================================================

FUNCTION DS_GCSD_SET_OLD_MODEM_CONTROL_LINES

DESCRIPTION
  This function turns the value of the global variable
  OldModemControlLines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ds_gcsd_set_OldModemControlLines
(
  ds_ucsd_sio_status_T   *OldModemControlLines_ptr
)
{
  if(OldModemControlLines_ptr == NULL)
  {
    return;
  }
  
  OldModemControlLines.DTR = OldModemControlLines_ptr->DTR;
  OldModemControlLines.RTS = OldModemControlLines_ptr->RTS;
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

