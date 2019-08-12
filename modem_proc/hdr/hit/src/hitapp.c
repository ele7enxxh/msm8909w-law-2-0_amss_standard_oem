
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   HIT Test Application Task for hdr and wcdma

GENERAL DESCRIPTION
  This file contains the task implementation for HIT Test App for hdr and wcdma

EXTERNALIZED FUNCTIONS
  hitapp_wait
  hitapp_cmd_handler
  
REGIONAL FUNCTIONS
  hitapp_init
  hitapp_kick_watchdog
  hitapp_process_command
  hitapp_task

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
    
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitapp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
12/08/2015   rmv     Fixed compiler warnings for unused return values 
10/17/2014   ljl     Included amssassert.h. 
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
07/09/2014   cnx     Support NV refresh without reset.
01/15/2013   smd     Featurized hit cmd and hit diag.
08/08/2013   rmv     Featurized tmc APIs with ifndef FEATURE_MODEM_RCINIT
05/06/2013   mbs     Fixed compiler errors for BOLT build
03/04/2013   vko     Fixed compiler warnings
03/01/2013   dsp     Moved watchdog timer code to watchdog heart-beat model.
02/14/2013   dsp     Removed in hitapp task the wait for hdrbc task to be ready.
04/30/2012   dsp     Fixed compiler warnings on Dime. 
03/30/2012   ljl     Used tcm_task_* APIs. 
03/22/2012   arm     Fixed featurization bug for RCINIT.  
03/20/2012   cnx     Resolved TCB compiler errors for Dime. 
03/16/2012   arm     Resolve TCB errors for Dime. 
03/13/2012   smd     Added RCINIT support. 
02/27/2012   smd     Added updates of FTM msgr interface changes.
08/23/2007   vh      Added critical section around command handling
05/07/2007   rmg     Changed dynamic memory allocation to static one
03/28/2007   vh      Created

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "amssassert.h"

#ifdef FEATURE_HIT

#include "comdef.h"
#include "rex.h"
#include "task.h"
#ifndef FEATURE_MODEM_RCINIT
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT */
#include "dog.h"
#include "dog_hb_rex.h"
#include "nv.h"
#include "diagpkt.h"
#include "diagcmd.h"
#ifdef FEATURE_HIT_DIAG_AND_CMD
#include "hitcmd.h"
#include "hitdiag.h"
#include "hitqueue.h"
#endif /* FEATURE_HIT_DIAG_AND_CMD */
#include "hitapp.h"
#include "hdrhitmsg.h"
#include "hdrutil.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

rex_tcb_type *hdrhitapp_tcb_ptr;

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#define HITAPP_RCEVT_PREFIX "HITAPP:"
#define HITAPP_RCEVT_READY HITAPP_RCEVT_PREFIX "ready"
#define HDRBC_RCEVT_PREFIX "HDRBC:"
#define HDRBC_RCEVT_READY HDRBC_RCEVT_PREFIX "ready"
#endif

#define HITAPP_CMD_QUEUE_SIZE     30 
  /* Max number of commands that can be queued */

#ifdef FEATURE_HDR_HIT
extern hitcmd_ret_type hithdrcmd_handler ( hitcmd_buf_type* );
extern rex_crit_sect_type hithdrcmd_crit_sect;
  /* function that processes a hithdr command */
#endif

#ifdef FEATURE_HIT_DIAG_AND_CMD
static hitcmd_handler_info hitapp_cmd_handler_table[]=
{
#ifdef FEATURE_HDR_HIT
  {HIT_HDR, hithdrcmd_handler}, /* handler to execute hdr commands*/
#endif
  {0xffff, NULL}, /* to register a new testapp insert before this line */
};
#endif /* FEATURE_HIT_DIAG_AND_CMD */

#ifdef FEATURE_HDR_HIT
extern void hdrhitcmd_enable ( uint8 hit_enable );
  /* conveys the status of the NV item - hit_enable to HDR */
#endif

#ifdef FEATURE_HIT_DIAG_AND_CMD
static const diagpkt_user_table_entry_type hit_diag_tbl[] =
{
  {0x0000, 0xffff, hitdiag_handler}
};
#endif /* FEATURE_HIT_DIAG_AND_CMD */

/*==========================================================================

                     GLOBAL VARIABLES FOR MODULE

==========================================================================*/

#ifdef FEATURE_HIT_DIAG_AND_CMD
static hitqueue_type hitapp_queue;
  /* The command queue structure for the task*/

static hitqueue_element_type hitapp_queue_elements[HITAPP_CMD_QUEUE_SIZE];
  /* Array to hold queued commands */

#endif /* FEATURE_HIT_DIAG_AND_CMD */

static rex_timer_type hitapp_delay_timer;
  /* timer for delaying command execution as specified by the command */

#if defined( DOG_DYNAMIC_API ) && defined( FEATURE_HDR_WDOG_DYNAMIC )
dog_report_type   hitapp_dog_rpt_var = 0;
  /* Dog rpt */

uint32 hitapp_dog_rpt_time_var = 0xffff;
  /* Dog rpt time */
#endif
/*===========================================================================

FUNCTION hitapp_kick_watchdog

DESCRIPTION
  This procedure kicks the watch dog and resets the watch dog timer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hitapp_kick_watchdog(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  (void) rex_clr_sigs( hdrhitapp_tcb_ptr,  HITAPP_RPT_TIMER_SIG );
  dog_hb_report( hitapp_dog_rpt_var );
}

/*===========================================================================

FUNCTION hitapp_wait

DESCRIPTION
  This procedure performs a REX wait on a signal that the caller specifies. 
  In additions to that the function must handle watchdog timer expiry and 
  STOP and offline signals

DEPENDENCIES
  None

PARAMETERS
  sigs: which signal to wait for

RETURN VALUE
  sigs

SIDE EFFECTS
  None

===========================================================================*/

uint16 hitapp_wait
(
  uint16 sigs
    /* which signal to wait for */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  uint16 rex_signals_mask;

  do
  {
    /* set signal mask */
    rex_signals_mask = rex_wait( sigs | HITAPP_RPT_TIMER_SIG | 
                                 TASK_STOP_SIG | TASK_OFFLINE_SIG );

    if( ( rex_signals_mask & TASK_STOP_SIG ) != 0 )
    {
      MSG( MSG_SSID_HIT,  MSG_LEGACY_MED, "TASK_STOP_SIG received" );
        /*  */

      (void) rex_clr_sigs( hdrhitapp_tcb_ptr, TASK_STOP_SIG );
        /*  */

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();  
#endif /* FEATURE_MODEM_RCINIT */ 
        /*  */
    }

    if ( ( rex_signals_mask & TASK_OFFLINE_SIG ) != 0 ) 
    {
     (void) rex_clr_sigs( hdrhitapp_tcb_ptr, TASK_OFFLINE_SIG );
       /*  */

     MSG( MSG_SSID_HIT,  MSG_LEGACY_MED, "TASK_OFFLINE_SIG received" );
       /*  */

#ifndef FEATURE_MODEM_RCINIT
     tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT */
       /*  */
    }

    if ( ( rex_signals_mask & HITAPP_RPT_TIMER_SIG ) != 0 ) 
    {
     (void) rex_clr_sigs( hdrhitapp_tcb_ptr, HITAPP_RPT_TIMER_SIG );
     hitapp_kick_watchdog(); 
    }

    if ( ( rex_signals_mask & sigs ) != 0 )
    {
      (void) rex_clr_sigs( hdrhitapp_tcb_ptr, sigs );
      break;
    }

  }while( 1 );

  return rex_signals_mask;

}

/*===========================================================================

FUNCTION hitapp_process_command

DESCRIPTION
  This procedure processes each command

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void hitapp_process_command( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_HIT_DIAG_AND_CMD
  hitcmd_buf_type *cmd_buf_ptr;
    /* pointer to the command buffer in queue */

  hitcmd_handler_type handler;

  /* Fetch and process commands from the queue */
  while( hitqueue_get( &hitapp_queue, 
                       ((hitqueue_element_type *)(&cmd_buf_ptr)) ) )
  {
    handler = hitcmd_handler(cmd_buf_ptr->testapp_id,hitapp_cmd_handler_table);
    MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: queue get" );

    if (handler)
    {
      if (cmd_buf_ptr->delay > 0 && cmd_buf_ptr->delay <= 0xffff0000)
      {
        rex_set_timer( &hitapp_delay_timer, cmd_buf_ptr->delay );
        hitapp_wait( HITAPP_TIMER_SIG );
        MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: Delay done" );
      }
      else
      {
        hitapp_kick_watchdog();
        MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: Delay=0" );
      }

      handler(cmd_buf_ptr);
      MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: Cmd Sent" );
    }
    else
    {
      hitcmd_send_status( cmd_buf_ptr, HITCMD_ERROR_TESTAPP_ID, TRUE );
      MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: ERROR_TESTAPP_ID" );
    }
  } /* while */

  MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_PROCESS_CMD: queue done" );
#else  /* FEATURE_HIT_DIAG_AND_CMD */
  MSG( MSG_SSID_HIT, MSG_LEGACY_MED, 
       "HITAPP_PROCESS_CMD: hit cmd processing is not supported" );
#endif /* FEATURE_HIT_DIAG_AND_CMD */
} /* end hitapp_process_command() */


/*===========================================================================

FUNCTION HITAPP_READ_NV_ITEM

DESCRIPTION
  This function reads an item from NV

DEPENDENCIES
  None.

RETURN VALUE
  NV command return value

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE or NOTACTIVE

===========================================================================*/
nv_stat_enum_type hitapp_read_nv_item
(
  nv_items_enum_type item_code, /* item to read */
  nv_item_type * item_ptr /* pointer to where to put the item */
)
{
  static nv_cmd_type hit_nv_cmd_buf; /* Command buffer to NV */

  ASSERT(rex_self() == hdrhitapp_tcb_ptr);

  /* Prepare command buffer to NV */

  hit_nv_cmd_buf.cmd = NV_READ_F;
  hit_nv_cmd_buf.tcb_ptr = hdrhitapp_tcb_ptr;
  hit_nv_cmd_buf.sigs = HITAPP_NV_READ_SIG;
  hit_nv_cmd_buf.done_q_ptr = NULL;
  hit_nv_cmd_buf.item = item_code;
  hit_nv_cmd_buf.data_ptr = item_ptr;

  /* Clear signal, issue the command, wait for the response */
  (void)rex_clr_sigs( hdrhitapp_tcb_ptr, HITAPP_NV_READ_SIG );
  nv_cmd( &hit_nv_cmd_buf );
  (void)hitapp_wait( HITAPP_NV_READ_SIG );
  (void)rex_clr_sigs( hdrhitapp_tcb_ptr, HITAPP_NV_READ_SIG );
  
  /* check and return status */

  if ((hit_nv_cmd_buf.status == NV_DONE_S) ||
      (hit_nv_cmd_buf.status == NV_NOTACTIVE_S))
  {
  }
  else
  {
    ERR_FATAL( "Bad NV read status %d", hit_nv_cmd_buf.status, 0, 0);
  }

  return (hit_nv_cmd_buf.status);

} /* hitapp_read_nv_item */


/*===========================================================================

FUNCTION hitapp_init

DESCRIPTION
  HITAPP task initializtion function

DEPENDENCIES
  None

PARAMETERS
  dummy

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hitapp_init( void )
{

#ifndef FEATURE_MODEM_RCINIT
  /* define CMD Delay Timer */
  rex_def_timer( &hitapp_delay_timer, hdrhitapp_tcb_ptr, HITAPP_TIMER_SIG );
#endif /* FEATURE_MODEM_RCINIT */

#ifdef FEATURE_HDR_HIT
  /* initilize critical section */
  rex_init_crit_sect( &hithdrcmd_crit_sect );
#endif /* FEATURE_HDR_HIT */

#ifdef FEATURE_HIT_DIAG_AND_CMD
  /* initialize hitapp cmd queue */
  hitqueue_init( &hitapp_queue, hitapp_queue_elements, 
                 HITAPP_CMD_QUEUE_SIZE );

  /* initialize hitcmd queue */
  hitcmd_init();
#endif /* FEATURE_HIT_DIAG_AND_CMD */

#ifndef FEATURE_MODEM_RCINIT
  /* Initialize message router functionalities */
  hdrhitmsg_init();
#endif /* FEATURE_MODEM_RCINIT */    
} /* hitapp_init */

/*===========================================================================

FUNCTION hitapp_refresh_nv

DESCRIPTION
  This procedure reads NV item and control the availablity of HIT module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hitapp_refresh_nv(void)
{

#ifdef FEATURE_HIT_DIAG_AND_CMD

  nv_item_type      nv_item;   /* To read the nv item */

#if defined(FEATURE_RUMI_BRINGUP) || defined(FEATURE_RUMI3_BRINGUP)
  nv_item.hit_enable = 1;
  /* Bypass nv item */
#else
  /* Read HIT NV item to see if it is enabled */
  (void) hdrutil_read_nv_item( NV_HIT_ENABLE_I, &nv_item);
#endif /* FEATURE_RUMI_BRINGUP */

  if (nv_item.hit_enable)
  {
    /* register hitdiag_handler to diag */
    DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, \
                                              DIAG_SUBSYS_HIT, hit_diag_tbl);
  }

#ifdef FEATURE_HDR_HIT
  /* convey the status of the NV item to HDR */
  hdrhitcmd_enable( nv_item.hit_enable );
#endif
#endif /* FEATURE_HIT_DIAG_AND_CMD */
}

/*===========================================================================

FUNCTION hitapp_task

DESCRIPTION
  This procedure is the entry procedure for the task. It calls the init function,
  kicks the watchdog and waits for commands. When a command arrives it is processed.

DEPENDENCIES
  None

PARAMETERS
  dummy

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hitapp_task
(
  uint32 dummy
    /* dummy variable required to define a task */
)
{
  nv_item_type      nv_item;   /* To read the nv item */
  rex_sigs_type     sigs;      /* Active task signals */
    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();

  /* Run time synch with other tasks is done here if needed, following APIs
  ** used: rcinit_lookup(),  rcevt_wait_name()
  */
#ifdef FEATURE_HDR_BCMCS
  rcevt_wait_name(HDRBC_RCEVT_READY);
#endif
  (void) rcevt_signal_name(HITAPP_RCEVT_READY);

  /* RCINIT calls init function defined in task scons file in rcinit context
     All tcb related intializations should be done after rcinit starts our
     task */
  hdrhitapp_tcb_ptr = rex_self();

  /* define CMD Delay Timer */
  rex_def_timer( &hitapp_delay_timer, hdrhitapp_tcb_ptr, HITAPP_TIMER_SIG );

  /* Initialize message router functionalities */
  hdrhitmsg_init();

  hitapp_dog_rpt_var = dog_hb_register_rex( (rex_sigs_type) HITAPP_RPT_TIMER_SIG );

#else /* FEATURE_MODEM_RCINIT */

  hdrhitapp_tcb_ptr = rex_self();

  hitapp_init();

  /* The task waits here until it gets the START signal from REX */
  tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT */

#if defined(FEATURE_RUMI_BRINGUP) || defined(FEATURE_RUMI3_BRINGUP)
  nv_item.hit_enable = 1;
  /* Bypass nv item */
#else
  /* Read HIT NV item to see if it is enabled */
  (void) hitapp_read_nv_item( NV_HIT_ENABLE_I, &nv_item);
#endif /* FEATURE_RUMI_BRINGUP */

#ifdef  FEATURE_HIT_DIAG_AND_CMD
  if (nv_item.hit_enable)
  {
    /* register hitdiag_handler to diag */
    DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F, \
                                              DIAG_SUBSYS_HIT, hit_diag_tbl);
  }
#endif /* FEATURE_HIT_DIAG_AND_CMD */

#ifdef FEATURE_HDR_HIT
  /* convey the status of the NV item to HDR */
  hdrhitcmd_enable( nv_item.hit_enable );
#endif

  hitapp_kick_watchdog();
    /*Initially kick the watch dog */

  /* Wait in a loop for commands*/
  for(;;)
  {
    sigs = hitapp_wait( HITAPP_CMD_Q_SIG | HITAPP_MSG_Q_SIG ); 
      /* wait for CMD in the queue */

    if ( sigs & HITAPP_CMD_Q_SIG )
    {
    
      hitapp_process_command();
      /* process the command */
    }
    else if ( sigs & HITAPP_MSG_Q_SIG )
    {
      hdrhitmsg_process_rcvd_msgs();
        /* process the message */
    }
    else
    {
      MSG( MSG_SSID_HIT,  MSG_LEGACY_ERROR, "HIT: unrecognized signal" );
    }
  }
}


/*===========================================================================

FUNCTION hitapp_cmd_handler

DESCRIPTION
  This procedure is the external interface for other tasks to send commands to the task.
  This function uses the hit common routine to put the packet in the queue and then 
  it signals the task.

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr
  signal: If set to true only then the task (test application) is signalled

RETURN VALUE
  status

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_HIT_DIAG_AND_CMD
hitcmd_ret_type hitapp_cmd_handler
(
  hitcmd_buf_type* cmd_buf_ptr
    /* pointer to the incoming command pkt */
)
{

  if( cmd_buf_ptr) 
  {
    if( !hitqueue_put( &hitapp_queue, 
                      (hitqueue_element_type)cmd_buf_ptr ) )
    {
      return HITCMD_ERROR_QUEUE;
    } /* end if */

    MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_HANDLER: Command queued" );

  } /* if( cmd_buf_ptr != NULL ) */
  else
  {
    rex_set_sigs( hdrhitapp_tcb_ptr, HITAPP_CMD_Q_SIG );
      /* signal task indicating a command is queued */

    MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "HITAPP_HANDLER: task signaled" );
  } /* if( signal == TRUE ) */

  return HITCMD_ERROR_NONE;
}
#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* FEATURE_HIT */
