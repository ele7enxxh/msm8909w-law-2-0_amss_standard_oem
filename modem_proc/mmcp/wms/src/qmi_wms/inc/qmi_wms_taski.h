#ifndef QMI_WMS_TASKI_H
#define QMI_WMS_TASKI_H
/*===========================================================================

                          Q M I _ WMS _ T A S K I . H

DESCRIPTION
  QMI WMS Task internal header file, has function, data declarations for
  internal usage by QMI WMS Task.

EXTERNALIZED FUNCTIONS
  qmi_wms_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_wms_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_wmsi_powerup()
    Cold start called before QMI task is started.

  qmi_wmsi_process_cmd()
    Processes command signals in QMI Task.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/inc/qmi_wms_taski.h#1 $
  $DateTime: 2005/12/23 17:19:09

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

#include "comdef.h"
#include "customer.h"



#include "rex.h"
#include "queue.h"
#include "qmi_wms_task_svc.h"
#include "qmi_wms_crit_sect.h"
#include "dog_hb_rex.h"

extern  rex_tcb_type   *qmi_wms_tcb_ptr; // pointer for myself

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define QMI_WMS_TCB  TASK_HANDLE(qmi_wms)
#else
  #define QMI_WMS_TCB  qmi_wms_tcb_ptr
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers.
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_wms_sig_handler_type  sig_handler;
  void *                user_data_ptr;
} qmi_wmsi_sig_handler_info_type;

/*--------------------------------------------------------------------------
  Datatype to hold signal, command handlers.
---------------------------------------------------------------------------*/
extern qmi_wmsi_sig_handler_info_type qmi_wmsi_sig_handler[QMI_WMS_MAX_SIGNALS];

/*--------------------------------------------------------------------------
  Watchdog timer for QMI WMS task.
---------------------------------------------------------------------------*/
extern dog_report_type  qmi_wms_dog_rpt_var;              /* Dog report ID */

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled for QMI main processing loop.
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
  #define QMI_WMS_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
  extern rex_sigs_type qmi_wmsi_enabled_sig_mask[QMI_WMS_SIG_ARR_LEN];
#else
  extern rex_sigs_type qmi_wmsi_enabled_sig_mask;
#endif /* FEATURE_REX_SIGS_EXT */

/*--------------------------------------------------------------------------
  QMI's own critical section.
---------------------------------------------------------------------------*/
//extern qmi_wms_crit_sect_type global_qmi_wms_crit_section;

/*===========================================================================

                           PUBLIC MACRO DEFINITIONS

===========================================================================*/

#ifdef FEATURE_REX_SIGS_EXT
/*--------------------------------------------------------------------------
  The SIG_IDX macro provides the index of the array for the signal sig_num.
  The SIG_OFFSET macro provides the offset for the signal sig_num within
  the signal dword.
---------------------------------------------------------------------------*/
#define QMI_WMS_SIG_IDX(sig_num) ((sig_num) / (8*sizeof(rex_sigs_type)))
#define QMI_WMS_SIG_OFFSET(sig_num) ((sig_num) % (8*sizeof(rex_sigs_type)))

/*--------------------------------------------------------------------------
  QMI_WMS_CLR_SIG clears the signal sig_num within the mask pointed to by
  val_ptr.
---------------------------------------------------------------------------*/
#define QMI_WMS_CLR_SIG(val_ptr, sig_num)                                       \
{                                                                           \
  *(((rex_sigs_type *)(val_ptr)) + QMI_WMS_SIG_IDX(sig_num)) &=                 \
    ~((rex_sigs_type) 1 << QMI_WMS_SIG_OFFSET(sig_num));                        \
}

/*--------------------------------------------------------------------------
  QMI_WMS_SET_SIG sets the signal sig_num in the mask pointed to by val_ptr.
---------------------------------------------------------------------------*/
#define QMI_WMS_SET_SIG(val_ptr, sig_num)                                       \
{                                                                           \
  *(((rex_sigs_type *)(val_ptr)) + QMI_WMS_SIG_IDX(sig_num)) |=                 \
    ((rex_sigs_type) 1 << QMI_WMS_SIG_OFFSET(sig_num));                         \
}

/*--------------------------------------------------------------------------
  QMI_WMS_IS_SET_SIG verifies if signal sig_num has been set in the mask pointed
  to by val_ptr.
---------------------------------------------------------------------------*/
#define QMI_WMS_IS_SET_SIG(val_ptr, sig_num)                                    \
   (*(((rex_sigs_type *)(val_ptr)) + QMI_WMS_SIG_IDX(sig_num)) &                \
       ((rex_sigs_type) 1 << QMI_WMS_SIG_OFFSET(sig_num)))

/*---------------------------------------------------------------------------
  This is the exported macro that sets a given signal for QMI_WMS task.
---------------------------------------------------------------------------*/
#define QMI_WMS_SET_SIGNAL( sig )                                           \
{                                                                           \
  rex_sigs_type sig_arr[QMI_WMS_SIG_ARR_LEN];                               \
  uint8 arr_index = 0;                                                      \
  for(arr_index = 0 ; arr_index < QMI_WMS_SIG_ARR_LEN; arr_index++)         \
  {                                                                         \
    sig_arr[arr_index] = 0;                                                 \
  }                                                                         \
  QMI_WMS_SET_SIG(sig_arr, sig);                                            \
  if(FALSE == (rex_set_sigs_ext(QMI_WMS_TCB, sig_arr)))                     \
  {                                                                         \
    MSG_ERROR_1("Failure to set signal %d in QMI_WMS_TCB", sig);            \
    ASSERT(0);                                                              \
  }                                                                         \
}

/*---------------------------------------------------------------------------
  This is the exported macro that clears a given signal for DS_MODEM task.
---------------------------------------------------------------------------*/
#define QMI_WMS_CLR_SIGNAL( sig )                                           \
{                                                                           \
  rex_sigs_type sig_arr[QMI_WMS_SIG_ARR_LEN];                               \
  uint8 arr_index = 0;                                                      \
  for(arr_index = 0 ; arr_index < QMI_WMS_SIG_ARR_LEN; arr_index++)         \
  {                                                                         \
    sig_arr[arr_index] = 0;                                                 \
  }                                                                         \
  QMI_WMS_SET_SIG(sig_arr, sig);                                            \
  if(FALSE == (rex_clr_sigs_ext(QMI_WMS_TCB, sig_arr)))                     \
  {                                                                         \
    MSG_ERROR_1("Failure to clear signal %d in QMI_WMS_TCB", sig);          \
    ASSERT(0);                                                              \
  }                                                                         \
}

#else

#define QMI_WMS_SET_SIGNAL( sig )                                               \
  (void)rex_set_sigs( QMI_WMS_TCB, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define QMI_WMS_CLR_SIGNAL( sig )                                               \
  (void)rex_clr_sigs( QMI_WMS_TCB, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#endif /* FEATURE_REX_SIGS_EXT */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION QMI_WMS_DEFAULT_SIG_HANDLER()

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
qmi_wms_default_sig_handler
(
  qmi_wms_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*===========================================================================
FUNCTION QMI_WMSI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI_WMS task. This
  function will perform all first time thru functions for the QMI_WMS
  task. After this intiailzation, the QMI_WMS task would signal the TMC
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
void qmi_wmsi_powerup
(
  void
);
#endif /* QMI_WMS_TASKI_H */
