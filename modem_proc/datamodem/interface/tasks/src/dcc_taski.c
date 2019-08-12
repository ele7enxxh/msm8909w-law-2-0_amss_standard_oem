/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              D C C _ T A S K I . C

DESCRIPTION
  DataCommon Control Task internal API file. Contains APIs and definitions
  used internal to DCC Task processing. 

EXTERNALIZED FUNCTIONS
  dcc_default_sig_handler()
    Implicit handler for signals processed by DCC Task.

  dcc_default_cmd_handler()
    Implicit handler for commands processed by DCC Task.

  dcci_powerup()
    Cold start called before DCC task is started.

  dcci_process_cmd()
    Processes command signals in DCC Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the DCC APIs can be used unless DCC task is created and started.

Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/dcc_taski.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/11    scb    Port changes for FMC into DCC task
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10    ss     DYNAMIC WDOG support for DATA tasks.
09/24/10    pp     Free the user_data_ptr only if the user memory is coming 
                   from DCC data buffers.
07/14/10    asn    Address initialization of DS Profile and UMTS Profile Reg
11/05/09    ar     QTF support changes.
05/22/09    am     Make sure ps_mem_init() is called before ps_mem_pool_init.
02/19/09    am     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "ds_Utils_DebugMsg.h"


#include "dog_hb_rex.h"

#include "dcc_taski.h"
#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "ps_mem.h"


/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Dog data structure for DCC task.
--------------------------------------------------------------------------*/
dog_report_type dcc_dog_rpt_id = 0; 		      /*! Dog report ID	      */

q_type dcci_cmd_q;
q_type dcci_cmd_free_q;

#define DCC_CMD_DATA_BUF_SIZE  ((sizeof(dcc_cmd_data_buf_type) + 3) & ~3)

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
dcci_sig_handler_info_type dcci_sig_handler[DCC_MAX_SIGNALS];
dcc_cmd_handler_type dcci_cmd_handler[DCC_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

static dcci_cmd_type dcci_cmd_buf[DCCI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type dcci_enabled_sig_mask;

/*--------------------------------------------------------------------------
  DCC's own critical section
---------------------------------------------------------------------------*/
ps_crit_sect_type global_dcc_crit_section;

/*--------------------------------------------------------------------------
  Max memory for cmd bufs = Max cmds processed at a time by DCC
---------------------------------------------------------------------------*/
#define DCC_CMD_DATA_BUF_NUM      DCCI_CMD_BUF_CNT
#define DCC_CMD_DATA_BUF_HIGH_WM  (DCCI_CMD_BUF_CNT - 5)
#define DCC_CMD_DATA_BUF_LOW_WM   5

/*----------------------------------------------------------------------------
  Allocate memory to hold DCC cmd buf along with ps_mem header
----------------------------------------------------------------------------*/
static int dcc_cmd_data_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                (
                                  DCC_CMD_DATA_BUF_NUM,
                                  DCC_CMD_DATA_BUF_SIZE
                                )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the second points to actual buf
----------------------------------------------------------------------------*/                        
static ps_mem_buf_hdr_type * dcc_cmd_data_buf_hdr[DCC_CMD_DATA_BUF_NUM];
static dcc_cmd_data_buf_type * dcc_cmd_data_buf_ptr[DCC_CMD_DATA_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

#if DCC_MAX_SIGNALS > 31
#error Signal Count exceeded maximum allowed
#endif

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION DCC_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean 
dcc_default_sig_handler
(
  dcc_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("dcc_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* dcc_default_sig_handler() */

/*===========================================================================

FUNCTION DCC_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
dcc_default_cmd_handler
(
  dcc_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("dcc_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* dcc_default_cmd_handler() */



/*===========================================================================
FUNCTION DCCI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the DCC task. 
  
  This function performs initialization before DCC task is started.
  Note that initializations internal to DCC should be performed before
  initializing other components that run in DCC task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
dcci_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("dcci_powerup(): "
                  "Data Common Control Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    DCC context
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&global_dcc_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the PS mem module
  -------------------------------------------------------------------------*/
  ps_mem_init();

  /*-------------------------------------------------------------------------
    Initialize memory for DCC command data buffers
  -------------------------------------------------------------------------*/
  if( PS_MEM_POOL_INIT_OPT( PS_MEM_DCC_CMD_DATA_BUF_TYPE,
                            dcc_cmd_data_buf_mem,
                            DCC_CMD_DATA_BUF_SIZE, 
                            DCC_CMD_DATA_BUF_NUM,
                            DCC_CMD_DATA_BUF_HIGH_WM,
                            DCC_CMD_DATA_BUF_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG                   
                            (int *) dcc_cmd_data_buf_hdr,
                            (int *) dcc_cmd_data_buf_ptr
#else
                             NULL,
                             NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */                   
                           ) == -1)
  {
    LOG_MSG_ERROR_0("dcci_powerup(): "
                    "DCC: Cmd data buf mem init error");
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &dcci_cmd_q );
  (void) q_init( &dcci_cmd_free_q );

  for( i = 0; i < DCCI_CMD_BUF_CNT; i++ )
  {
    q_put( &dcci_cmd_free_q, q_link( &dcci_cmd_buf[i], &dcci_cmd_buf[i].link) );
  }


  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

  for( i = 0; i < (int) DCC_MAX_SIGNALS; i++ )
  {
    dcci_sig_handler[i].sig_handler = dcc_default_sig_handler;
    dcci_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) DCC_MAX_DEFINED_CMD_TYPES; i++ )
  {
    dcci_cmd_handler[i] = dcc_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS Task
  -------------------------------------------------------------------------*/
  dcci_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) DCC_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) DCC_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) DCC_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) DCC_DOG_HB_REPORT_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) DCC_SIG_MASK_CHANGE_SIGNAL );

  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

} /* dcci_powerup() */

/*===========================================================================

FUNCTION DCCI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to DCC_CMD_Q_SIGNAL.  It retrieves the
  next command from the DCC command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the DCC command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
dcci_process_cmd
(
  void
)
{
  static byte    dcc_cmds_proc_hi_score = 0;
  dcci_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &dcci_cmd_q ) == 0)
  {
    LOG_MSG_ERROR_0("dcci_process_cmd(): "
                    "No DCC command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( dcci_cmd_type * ) q_get( &dcci_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= DCC_MAX_DEFINED_CMD_TYPES )
    {
      LOG_MSG_ERROR_1("dcci_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &dcci_cmd_free_q, &cmd_ptr->link );


      continue;
    }
    processed++;
    dcci_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

    /* This IF block is no longer needed, if all clients uses dcc_send_cmd_ex
       for posting commands to DCC */
    if (FALSE == cmd_ptr->user_allocated)
    {
      /* Since memory for user data is allocated using DCC data buffers, 
      DCC task frees it */
      dcc_free_cmd_data_buf(cmd_ptr->user_data_ptr);
    }

    cmd_ptr->user_data_ptr = NULL;
    q_put( &dcci_cmd_free_q, &cmd_ptr->link );


  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_dcc_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer DCC processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &dcci_cmd_q ) == 0 )
  {
    DCC_CLR_SIGNAL(DCC_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION(&global_dcc_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > dcc_cmds_proc_hi_score )
  {
    dcc_cmds_proc_hi_score = processed;
    LOG_MSG_INFO2_1 ("dcci_process_cmd(): "
                     "New high # DCC commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear DCC_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the DCC_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* dcci_process_cmd() */
