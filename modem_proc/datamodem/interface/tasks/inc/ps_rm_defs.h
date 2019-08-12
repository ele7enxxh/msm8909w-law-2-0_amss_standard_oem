#ifndef PS_RM_DEFS_H
#define PS_RM_DEFS_H
/*===========================================================================

                        P S _ R M _ D E F S . H

DESCRIPTION

  Rm Data processing Task header file.
  All definitions and structures needed by PS_RM task modules.
  

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/ps_rm_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#ifdef __cplusplus
extern "C"
{
#endif

#include "rex.h"
#include "queue.h"
#include "task.h"
#include "ps_rm_task.h"

#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type    * rex_ps_rm_tcb;
#endif

#define PS_RM_SIG_ARR_LEN REX_SIGS_ARRAY_LEN
  
#define PS_RM_SET_SIGNAL( sig ) \
  (void)rex_set_sigs( &ps_rm_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

#define PS_RM_CLR_SIGNAL( sig ) \
  (void)rex_clr_sigs( &ps_rm_tcb, (rex_sigs_type) 1 << (rex_sigs_type)( sig ) )

/*--------------------------------------------------------------------------
  PS_RM Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_RM_CMD_Q_SIGNAL                =  1,   /* PS_RM command Q signal          */
  PS_RM_DOG_HB_REPORT_SIGNAL        =  2,   /* Dog heart beat report signal       */
  PS_RM_SIG_MASK_CHANGE_SIGNAL      =  3,   /* Indicates sig mask has changed*/

  PS_RM_RX_SPLIT_DATA_SIGNAL        =  4, /* PS --> PS_RM data sig        */
  PS_RM_LLC_RX_DATA_Q_SIGNAL        =  5, /* LAN LLC Rx data sig             */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL      =  6, /* RMNET --> PS_RM data sig        */

  PS_RM_HW_EV_CBACK_SIGNAL         = 7,   /* Indicates IPA event is posted */

  PS_RM_MBIM_RX_DATA_Q_SIGNAL       = 8,  /* Phys port --> logical port data sig */
  /*-------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
  -------------------------------------------------------------------------*/
  PS_RM_TASK_OFFLINE_SIGNAL         = 13,   /* Reserved TASK_OFFLINE_SIG     */
  PS_RM_TASK_STOP_SIGNAL            = 14,   /* Reserved TASK_STOP_SIG        */
  PS_RM_TASK_START_SIGNAL           = 15,   /* Reserved TASK_START_SIG       */

  PS_RM_RMNET_RX_DATA_Q_SIGNAL_1    = 16,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_2    = 17,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_3    = 18,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_4    = 19,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_5    = 20,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_6    = 21,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_7    = 22,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_8    = 23,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_9    = 24,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_10   = 25,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_11   = 26,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_12   = 27,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_13   = 28,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_14   = 29,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_15   = 30,   /* RMNET --> PS_RM data sig   */
  PS_RM_RMNET_RX_DATA_Q_SIGNAL_16   = 31,   /* RMNET --> PS_RM data sig   */

  PS_RM_MAX_SIGNALS                         /* use next value for enum       */
} ps_rm_sig_enum_type;


/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_RM_MIN_CMD                      = -1,

  PS_RM_ARP_CACHE_UPDATE_CMD         = 1, /* ARP Updates in PS_RM          */
  PS_RM_ARP_TIMEOUT_CMD              = 2, /* ARP timer callbacks in PS_RM  */
  PS_RM_DPM_CMD                      = 3,

  /* Always must be last */
  PS_RM_MAX_DEFINED_CMD_TYPES
} ps_rm_cmd_enum_type;

/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ps_rm_sig_handler_type)
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ps_rm_cmd_handler_type)
(
  ps_rm_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/

typedef struct
{
  ps_rm_sig_handler_type sig_handler;
  void *user_data_ptr;
} ps_rm_sig_handler_info_type;

/*--------------------------------------------------------------------------
  PS_RM Task Command queues and Command Data Buffers
---------------------------------------------------------------------------*/

typedef struct     
{
  q_link_type       link;		     /* For linking onto cmd queue             */
  ps_rm_cmd_enum_type cmd;         /* Actual command to be processed         */
  void              *user_data_ptr; /* Command specific user parameters    */
} ps_rm_cmd_type;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PS_RM_DEFS_H */
