#ifndef DS_SIGI_TASKI_H
#define DS_SIGI_TASKI_H
/*=========================================================================*/
/*!
  @file
  ds_sig_taski.h

  @brief
  Header file containing internal definitions for DS_SIG task services.

  @details
  Exports data types, functions and macros internally used by DS_SIG task.

  @exported_data

  @exported_macros
  DS_SIGI_SET_SIGNAL()
    Set the signal for DS_SIG task (internal macro). Clients should use
    ds_sig_enable_signal()

  DS_SIGI_CLR_SIGNAL()
    Clear the signal for DS_SIG task (internal macro). Clients should use
    ds_sig_disable_signal()

  DS_SIGI_SET_SIG_MASK()
    Helper routine to set the signal mask where REX sig ext are enabled.

  DS_SIGI_CLR_SIG_MASK()
    Helper routine to disable signal mask where REX sig ext are enabled.

  DS_SIGI_IS_SIG_MASK_SET()
    Helper routine to verify if the signal mask is set (for rex sig exts).

  @exported_functions
  ds_sigi_default_sig_handler()
    Implicit handler for signals processed by DS_SIG Task.

  ds_sigi_default_cmd_handler()
    Implicit handler for commands processed by DS_SIG Task.

  ds_sigi_powerup()
    Cold start called before DS_SIG task is started.

  ds_sigi_init()
    After task start initialization of the DS_SIG task.

  ds_sigi_process_cmd()
    Processes command signals in DS_SIG Task.


  Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_sig_taski.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/11   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
08/14/09   hm      Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "ds_sig_task.h"
#include "ds_sig_svc.h"
#include "queue.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "rcinit_rex.h"

/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/
/*--------------------------------------------------------------------------
  Data type for holding DS_SIG command buffers.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type           link;		           /* For linking onto cmd queue   */
  ds_sig_cmd_enum_type  cmd;               /* Command to be processed      */
  void                 *user_data_ptr;     /* Command specific user data   */
} ds_sigi_cmd_type;

/*---------------------------------------------------------------------------
  Data type for holding signal handler.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_sig_signal_handler_type     sig_handler;     /* signal handler cback  */
  void                          *user_data_ptr;   /* signal user data      */
} ds_sigi_sig_handler_info_type;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
extern ds_sigi_sig_handler_info_type ds_sigi_sig_handler[DS_SIG_SIGNAL_MAX];
extern ds_sig_cmd_handler_type       ds_sigi_cmd_handler[DS_SIG_CMD_MAX];

/*--------------------------------------------------------------------------
  Command Queues.
---------------------------------------------------------------------------*/
extern q_type ds_sigi_cmd_q;
extern q_type ds_sigi_cmd_free_q;

/*--------------------------------------------------------------------------
  DS_SIG's own critical section
---------------------------------------------------------------------------*/
extern ps_crit_sect_type ds_sig_crit_sect;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for DS_SIG main processing loop
---------------------------------------------------------------------------*/
extern rex_sigs_type ds_sigi_enabled_sig_mask;

/*--------------------------------------------------------------------------
  DOG report timer for DS_SIG task.
---------------------------------------------------------------------------*/
extern rex_timer_type ds_sig_dog_rpt_timer;

/*--------------------------------------------------------------------------
  Defines if Dynamic DOG is not present.
---------------------------------------------------------------------------*/
#ifndef DOG_DYNAMIC_API
#ifndef DOG_DS_SIG_RPT
#define DOG_DS_SIG_RPT        168
#define DOG_DS_SIG_RPT_TIME   DOG_TO_REX_TIME (1129)
#endif
#endif

/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define DS_SIGI_SET_SIGNAL( sig )                                         \
  {                                                                         \
    (void)rex_set_sigs(rex_ds_sig_tcb,                                      \
                       (rex_sigs_type)1 << (rex_sigs_type)(sig));           \
  }
#else
  #define DS_SIGI_SET_SIGNAL( sig )                                         \
    (void)rex_set_sigs(&ds_sig_tcb, (rex_sigs_type)1 << (rex_sigs_type)(sig))
#endif

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define DS_SIGI_CLR_SIGNAL( sig )                                         \
  {                                                                         \
    (void)rex_clr_sigs(rex_ds_sig_tcb,                                      \
                       (rex_sigs_type)1 << (rex_sigs_type)(sig));           \
  }
#else
  #define DS_SIGI_CLR_SIGNAL( sig )                                         \
    (void)rex_clr_sigs(&ds_sig_tcb, (rex_sigs_type)1 << (rex_sigs_type)(sig))
#endif

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/*!
  @function
  ds_sigi_default_sig_handler

  @brief
  The default Signal handler for DS_SIG task.

  @details
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

  @dependencies
  None.

  @return
  None.

  @note
  None.
*/
/*=========================================================================*/
boolean ds_sigi_default_sig_handler
(
  ds_sig_signal_enum_type sig,             /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);


/*=========================================================================*/
/*!
  @function
  ds_sigi_default_cmd_handler

  @brief
  The default command handler for DS_SIG task.

  @details
  This function is registered as the default command handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

  @dependencies
  None.

  @return
  None.

  @note
  None.
*/
/*=========================================================================*/
void ds_sigi_default_cmd_handler
(
  ds_sig_cmd_enum_type cmd,      /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);


/*=========================================================================*/
/*!
  @function
  ds_sigi_powerup

  @brief
  The powerup init function for DS_SIG task.

  @details
  Powerup (coldstart) initialization function for the DS_SIG task. This
  function will perform all first time thru functions for the DS_SIG task.
  After this intiailzation, the DS_SIG task would signal the TMC that it
  is ready to accept requests from other tasks. Any initializations that
  are NOT dependent upon other tasks should be performed here.

  @dependencies
  None.

  @return
  None.

  @note
  This function must be called only once at system powerup.
*/
/*=========================================================================*/
void ds_sigi_powerup
(
  void
);


/*=========================================================================*/
/*!
  @function
  ds_sigi_init

  @brief
  The power-after init function for DS_SIG task.

  @details
  Initialization function for the DS_SIG task. This function
  performs the functions needed for the DS_SIG task to exit disabled state.

  @dependencies
  None.

  @return
  None.

  @note
  DS_SIG task should have been started.
  This function must be called only once at after signaling task start.
*/
/*=========================================================================*/
void ds_sigi_init
(
  void
);


/*=========================================================================*/
/*!
  @function
  ds_sigi_process_cmd

  @brief
  Processes commands posted to the DS_SIG task.

  @details
  This function is called in response to DS_SIG_CMD_Q_SIGNAL.  It retrieves
  the next command from the DS_SIG command queue and processes the command
  by calling the registered cmd handler.

  @dependencies
  An item must be on the DS_SIG command queue or an LOG_MSG_ERROR will occur.

  @return
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

  @note
  None.
*/
/*=========================================================================*/
boolean ds_sigi_process_cmd
(
  void
);

#endif /* DS_SIGI_SVCI_H */

