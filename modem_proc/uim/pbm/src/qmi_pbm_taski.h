#ifndef QMI_PBM_TASKI_H
#define QMI_PBM_TASKI_H
/*===========================================================================

                          Q M I _ PBM _ T A S K I . H

DESCRIPTION
  QMI Mmcp Task internal header file, has function, data declarations for
  internal usage by QMI Mmcp Task.

EXTERNALIZED FUNCTIONS
  qmi_pbm_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_pbm_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_pbmi_powerup()
    Cold start called before QMI task is started.

  qmi_pbmi_process_cmd()
    Processes command signals in QMI Task.

Copyright (c) 2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_taski.h#1 $
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/07/14    NR     Use v2.0 diag macros
01/25/11    rk     Created module.
===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"
#include "uim_msg.h"
#include "rex.h"
#include "queue.h"
#include "qmi_pbm_task_svc.h"
#include "qmi_pbm_task_cmd.h"
#include "qmi_pbm_crit_sect.h"

extern rex_tcb_type   *qmi_pbm_tcb_ptr; // pointer for myself

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define QMI_PBM_TCB  TASK_HANDLE(qmi_pbm)
#else
  #define QMI_PBM_TCB  qmi_pbm_tcb_ptr  
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_pbm_sig_handler_type  sig_handler;
  void *                user_data_ptr;
} qmi_pbmi_sig_handler_info_type;

/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers.
---------------------------------------------------------------------------*/
extern qmi_pbmi_sig_handler_info_type qmi_pbmi_sig_handler[QMI_PBM_MAX_SIGNALS];
extern qmi_pbm_cmd_handler_type qmi_pbmi_cmd_handler[QMI_PBM_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Watchdog timer for QMI PBM task.
---------------------------------------------------------------------------*/
extern dog_report_type  qmi_pbm_dog_rpt_var;                        /* Dog report ID */

/*--------------------------------------------------------------------------
  Command queues handled by QMI PBM task.
---------------------------------------------------------------------------*/
extern q_type qmi_pbmi_cmd_q;
extern q_type qmi_pbmi_cmd_free_q;

/*--------------------------------------------------------------------------
  The command buffer count.
---------------------------------------------------------------------------*/
#define QMI_PBMI_CMD_BUF_CNT  (300)
extern qmi_pbmi_cmd_type qmi_pbmi_cmd_buf[ QMI_PBMI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for QMI main processing loop.
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
  #define QMI_PBM_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
  extern rex_sigs_type qmi_pbmi_enabled_sig_mask[QMI_PBM_SIG_ARR_LEN];
#else
  extern rex_sigs_type qmi_pbmi_enabled_sig_mask;
#endif /* FEATURE_REX_SIGS_EXT */

/*--------------------------------------------------------------------------
  QMI's own critical section.
---------------------------------------------------------------------------*/
//extern qmi_pbm_crit_sect_type global_qmi_pbm_crit_section;

/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/

#ifdef FEATURE_REX_SIGS_EXT
/*--------------------------------------------------------------------------
  The SIG_IDX macro provides the index of the array for the signal sig_num.
  The SIG_OFFSET macro provides the offset for the signal sig_num within
  the signal dword.
---------------------------------------------------------------------------*/
#define QMI_PBM_SIG_IDX(sig_num) ((sig_num) / (8*sizeof(rex_sigs_type)))
#define QMI_PBM_SIG_OFFSET(sig_num) ((sig_num) % (8*sizeof(rex_sigs_type)))

/*--------------------------------------------------------------------------
  QMI_PBM_CLR_SIG clears the signal sig_num within the mask pointed to by
  val_ptr.
---------------------------------------------------------------------------*/
#define QMI_PBM_CLR_SIG(val_ptr, sig_num)                                       \
{                                                                           \
  *(((rex_sigs_type *)(val_ptr)) + QMI_PBM_SIG_IDX(sig_num)) &=                 \
    ~((rex_sigs_type) 1 << QMI_PBM_SIG_OFFSET(sig_num));                        \
}

/*--------------------------------------------------------------------------
  QMI_PBM_SET_SIG sets the signal sig_num in the mask pointed to by val_ptr.
---------------------------------------------------------------------------*/
#define QMI_PBM_SET_SIG(val_ptr, sig_num)                                       \
{                                                                           \
  *(((rex_sigs_type *)(val_ptr)) + QMI_PBM_SIG_IDX(sig_num)) |=                 \
    ((rex_sigs_type) 1 << QMI_PBM_SIG_OFFSET(sig_num));                         \
}

/*--------------------------------------------------------------------------
  QMI_PBM_IS_SET_SIG verifies if signal sig_num has been set in the mask pointed
  to by val_ptr.
---------------------------------------------------------------------------*/
#define QMI_PBM_IS_SET_SIG(val_ptr, sig_num)                                    \
   (*(((rex_sigs_type *)(val_ptr)) + QMI_PBM_SIG_IDX(sig_num)) &                \
       ((rex_sigs_type) 1 << QMI_PBM_SIG_OFFSET(sig_num)))

/*---------------------------------------------------------------------------
  This is the exported macro that sets a given signal for QMI_PBM task.
---------------------------------------------------------------------------*/
#define QMI_PBM_SET_SIGNAL( sig )                                               \
{                                                                           \
  rex_sigs_type sig_arr[QMI_PBM_SIG_ARR_LEN];                                   \
  uint8 arr_index = 0;                                                      \
  for(arr_index = 0 ; arr_index < QMI_PBM_SIG_ARR_LEN; arr_index++)             \
  {                                                                         \
    sig_arr[arr_index] = 0;                                                 \
  }                                                                         \
  QMI_PBM_SET_SIG(sig_arr, sig);                                                \
  if(FALSE == (rex_set_sigs_ext(QMI_PBM_TCB, sig_arr)))                  \
  {                                                                         \
    UIM_MSG_ERR_1("Failure to set signal %d in QMI_PBM_TCB", sig);      \
    ASSERT(0);                                                              \
  }                                                                         \
}

/*---------------------------------------------------------------------------
  This is the exported macro that clears a given signal for DS_MODEM task.
---------------------------------------------------------------------------*/
#define QMI_PBM_CLR_SIGNAL( sig )                                               \
{                                                                           \
  rex_sigs_type sig_arr[QMI_PBM_SIG_ARR_LEN];                                   \
  uint8 arr_index = 0;                                                      \
  for(arr_index = 0 ; arr_index < QMI_PBM_SIG_ARR_LEN; arr_index++)             \
  {                                                                         \
    sig_arr[arr_index] = 0;                                                 \
  }                                                                         \
  QMI_PBM_SET_SIG(sig_arr, sig);                                                \
  if(FALSE == (rex_clr_sigs_ext(QMI_PBM_TCB, sig_arr)))                  \
  {                                                                         \
    UIM_MSG_ERR_1("Failure to clear signal %d in QMI_PBM_TCB", sig);    \
    ASSERT(0);                                                              \
  }                                                                         \
}

#else

#define QMI_PBM_SET_SIGNAL( sig )                                               \
  (void)rex_set_sigs( QMI_PBM_TCB, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define QMI_PBM_CLR_SIGNAL( sig )                                               \
  (void)rex_clr_sigs( QMI_PBM_TCB, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#endif /* FEATURE_REX_SIGS_EXT */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_PBM_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean
qmi_pbm_default_sig_handler
(
  qmi_pbm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================
FUNCTION QMI_PBM_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbm_default_cmd_handler
(
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================
FUNCTION QMI_PBMI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI_PBM task. This
  function will perform all first time thru functions for the QMI_PBM
  task. After this intiailzation, the QMI_PBM task would signal the TMC
  that it is ready to accept requests from other tasks. Any initializations
  that are NOT dependent upon other tasks should be performed here.

PARAMETERS
  None.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbmi_powerup
(
  void
);

/*===========================================================================
FUNCTION QMI_PBMI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_PBM_CMD_Q_SIGNAL.  It retrieves the
  next command from the QMI command queue and processes the command by calling
  the registered cmd handler.

PARAMETERS
  None.

DEPENDENCIES
  An item must be on the QMI command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean qmi_pbmi_process_cmd
(
  void
);

/*===========================================================================
  FUNCTION qmi_pbm_process_cmd()

  DESCRIPTION
    Register the PBM service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/




#endif /* QMI_PBM_TASKI_H */