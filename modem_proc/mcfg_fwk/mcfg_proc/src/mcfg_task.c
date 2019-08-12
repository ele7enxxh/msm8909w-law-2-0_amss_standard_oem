/*===========================================================================
                        M O D E M C F G    T A S K  

DESCRIPTION

  This file contains the task entry point for MODEM_CFG task. It implements the 
  task initialization, including initialization of various procedures.
  and setting of the proper state. The task entry point function also implements
  the task startup protocol using the RCInit Framework. 


EXTERNALIZED FUNCTIONS   
  modem_cfg_task()
       It is the main entry point for Modem Config Task.
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/src/mcfg_task.c#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
02/27/15   hzhi    CR 798480: enable dynamic reclaiming feature on JO1.0.
06/10/14   skotalwa    CR 670990: removing unnecessary dependency of mmu update on segment loading feature. 
01/14/12   sbt     Initial Creation.

===========================================================================*/

#include "queue.h"
#include "rcinit.h"
#include "task.h"
#include "mcfg_common.h"
#include "mcfg_task.h"
#include "mcfg_int.h"
#include "mcfg_osal.h"
#include "mcfg_proc_i.h"
#include "mcfg_proc.h"
#include "mcfg_utils.h"
#include "mcfg_map.h"
#include "mcfg_uim.h"
#include "mcfg_nv.h"
#include "dog_hb_rex.h"


#include "qmi_csi.h"
#include "qmi_pdc_svc.h"

#ifdef FEATURE_MCFG_REFRESH
#include "mcfg_refresh_i.h"
#endif

#include "mcfg_dynrec.h"

#ifdef FEATURE_MCFG_DIAG_SUPPORT
#include "mcfg_diag.h"
#endif

#ifdef FEATURE_MCFG_MULTI_MBN_SUPPORT
#include "mcfg_multi_mbn.h"
#endif

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/* REX Task Signals declarations. */
#define  MODEM_CFG_DOG_RPT_TIMER_SIG     0x0001      /* Watchdog report signal*/
#define  MODEM_CFG_QMI_EVENT_SIG         0x0002      /* QMI CSI event signal  */
#define  MODEM_CFG_CMD_Q_SIG             0x0004      /* Command queue signal  */
#define  MODEM_CFG_MMGSDI_READY_SIG      0x0008      /* MMGSDI RCEVT signal   */
#define  MODEM_CFG_CM_READY_SIG          0x0010      /* CM RCEVT signal   */
#define  MODEM_CFG_SSR_SHUTDOWN_SIG      0x0020      /* SSR SHUTDOWN RCEVT signal*/

#define  MCFG_RCEVT_NAME_MMGSDI_READY    "MM_GSDI: READY"
#define  MCFG_RCEVT_NAME_CM_READY        "cm:ready"

/* Boundary to align cmd->data pointer to */
#define MCFG_TASK_CMD_DATA_ALIGNMENT (sizeof(int64))

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
extern void seg_load_init(void);
#endif /*FEATURE_SEGMENT_LOADING*/
extern void mcfg_mmu_update_init(void);

/* Command queue */
static q_type mcfg_task_cmd_q;

/* Pointer to MCFG's task control buffer */
static modem_cfg_os_tcb_type *mcfg_tcb = NULL;

static boolean mcfg_bootup_completed = FALSE;

/* Controls EFS Logging */
boolean mcfg_efslog_enabled = FALSE;

/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_task_cmd_proc

  DESCRIPTION
    Dispatches all the commands on the command queue.

  DEPENDENCIES
    Command queue must be initialized.

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_task_cmd_proc
(
  void
)
{
  mcfg_task_cmd_s_type *cmd;

  while ((cmd = (mcfg_task_cmd_s_type *) q_get(&mcfg_task_cmd_q)) != NULL)
  {
    switch (cmd->cmd_id)
    {
      case MCFG_TASK_CMD_UIM:
        mcfg_uim_cmd_proc(cmd);
        break;

      case MCFG_TASK_CMD_UTILS:
        mcfg_utils_cmd_proc(cmd);
        break;

#ifdef FEATURE_MCFG_REFRESH
      case MCFG_TASK_CMD_REFRESH:
        mcfg_refresh_cmd_proc(cmd);
        break;
#endif

      default:
        MCFG_MSG_ERROR_1("Unrecognized cmd_id %d", cmd->cmd_id);
    }

    mcfg_task_cmd_free(cmd);
  }
} /* mcfg_task_cmd_proc() */

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION modem_cfg_task

  DESCRIPTION
    Initializes and runs the MCFG task

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void modem_cfg_task
(
  dword unused
)
{
  uint32 modem_cfg_proc_status;

#ifdef FEATURE_MCFG_MULTI_MBN_SUPPORT
  mcfg_error_e_type process_status;
#endif /*FEATURE_MCFG_MULTI_MBN_SUPPORT*/

  modem_cfg_os_sigs_type sigs_mask = ( MODEM_CFG_DOG_RPT_TIMER_SIG   | /* Watchdog timer      */
                                       MODEM_CFG_QMI_EVENT_SIG       | /* QMI CSI event */
                                       MODEM_CFG_CMD_Q_SIG           | /* Command Queue Signal */
                                       TASK_STOP_SIG                   /* Stop the task    */
                                     );
  
  qmi_csi_os_params      os_params;
  qmi_csi_service_handle service_handle = NULL;
  MODEM_CFG_OS_RCEVT_SIGEX rcevt_sigex;
  dog_report_type modem_cfg_dog_rpt_var = 0;  

  (void) q_init(&mcfg_task_cmd_q);
  mcfg_tcb = modem_cfg_os_self();

  /* Modem Config Processing */
  MCFG_MSG_HIGH("Modem Config task-START");
  mcfg_efslog_enabled = mcfg_nv_efslogging_enabled();
  MCFG_MSG_HIGH_1("Modem Config task-START, mcfg_efslog_enabled %d", mcfg_efslog_enabled);
  mcfg_utils_init();
  mcfg_nv_init();
  mcfg_utils_saved_power_down_reason();
  MCFG_MSG_HIGH_1("mcfg_power_down_reason is ...0x%x", mcfg_pwr_down_reason());
  mcfg_uim_init();
  mcfg_setting_init();
#ifdef FEATURE_MCFG_DIAG_SUPPORT
  mcfg_diag_init();
#endif

#ifdef FEATURE_MCFG_REFRESH
  mcfg_refresh_init();
#endif

#ifdef FEATURE_MCFG_MULTI_MBN_SUPPORT
  process_status = mcfg_multi_mbn_proc();
  if(MCFG_ERR_NONE != process_status && MODEM_CFG_UPDATE_NOT_REQUIRED != process_status)
  {
    MCFG_MSG_ERROR_1("Multi mbn processing Error 0x%X", process_status);
  }
  MCFG_MSG_HIGH("Mulit mbn processing -DONE");
#endif /*FEATURE_MCFG_MULTI_MBN_SUPPORT*/ 

  modem_cfg_proc_status = modem_cfg_process();
  if(modem_cfg_proc_status)
  {
     MCFG_MSG_ERROR_1("Modem Config Processing Error 0x%lx", modem_cfg_proc_status);
  }
  MCFG_MSG_HIGH("Modem Config Processing -DONE");
 
#ifdef FEATURE_SEGMENT_LOADING
  seg_load_init();
#endif /*FEATURE_SEGMENT_LOADING*/
  mcfg_mmu_update_init();
  mcfg_dynrec_init();

  /* Modem Config Processing Done, signal the RCInit to proceed ... */ 
  rcinit_handshake_startup();

  /* Remaining initializion performed here, happens after sequence group is 
     started */
  memset(&os_params, 0, sizeof(os_params));
  os_params.tcb = modem_cfg_os_self();
  os_params.sig = MODEM_CFG_QMI_EVENT_SIG;
  
  service_handle = pdc_svc_init(&os_params);
  
  /* Register with Dog HB. NOTE: Do not register multiple times per task context! */
  modem_cfg_dog_rpt_var = 
    dog_hb_register_rex((rex_sigs_type)MODEM_CFG_DOG_RPT_TIMER_SIG);

  /* Rcevt registration for MMGSDI ready event */ 
  rcevt_sigex.signal = modem_cfg_os_self();
  rcevt_sigex.mask = MODEM_CFG_MMGSDI_READY_SIG;

  if (RCEVT_NULL == rcevt_register_sigex_name(
       MCFG_RCEVT_NAME_MMGSDI_READY, MODEM_CFG_OS_RCEVT_SIGEX_TYPE, 
       &rcevt_sigex))
  {
    MCFG_MSG_ERROR("mcfg_task: fail to register rcevt for GSDI ready");
    return;
  }
  sigs_mask ^= MODEM_CFG_MMGSDI_READY_SIG;

  /* Rcevt registration for CM ready event */ 
  rcevt_sigex.signal = modem_cfg_os_self();
  rcevt_sigex.mask = MODEM_CFG_CM_READY_SIG;

  if (RCEVT_NULL == rcevt_register_sigex_name(
       MCFG_RCEVT_NAME_CM_READY, MODEM_CFG_OS_RCEVT_SIGEX_TYPE, 
       &rcevt_sigex))
  {
    MCFG_MSG_ERROR("fail to register rcevt for CM ready");
    return;
  }
  sigs_mask ^= MODEM_CFG_CM_READY_SIG;

  /* Rcevt registration for SSR shutdown */ 
  rcevt_sigex.signal = modem_cfg_os_self();
  rcevt_sigex.mask = MODEM_CFG_SSR_SHUTDOWN_SIG;
  rcinit_register_term_group(RCINIT_GROUP_7, MODEM_CFG_OS_RCEVT_SIGEX_TYPE, 
                             &rcevt_sigex);
  sigs_mask ^= MODEM_CFG_SSR_SHUTDOWN_SIG;

  mcfg_bootup_completed = TRUE;
  for (;;)
  {
    modem_cfg_os_sigs_type received_sigs = modem_cfg_os_wait(sigs_mask);

    if (received_sigs & MODEM_CFG_DOG_RPT_TIMER_SIG)
    {
      /* Process watchdog report signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_DOG_RPT_TIMER_SIG); 
      /* Report to the Dog HB */
      dog_hb_report(modem_cfg_dog_rpt_var);
    }

    if (received_sigs & MODEM_CFG_QMI_EVENT_SIG)
    {
      /* process QMI_PDC signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_QMI_EVENT_SIG);
      qmi_csi_handle_event (service_handle, &os_params);
    }

    if (received_sigs & MODEM_CFG_CMD_Q_SIG)
    {
      /* Process command queue signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_CMD_Q_SIG);
      mcfg_task_cmd_proc();
    }

    if (received_sigs & MODEM_CFG_MMGSDI_READY_SIG)
    {
      /* Process MMGSDI ready signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_MMGSDI_READY_SIG);
      if(!rcevt_unregister_sigex_name(
         MCFG_RCEVT_NAME_MMGSDI_READY, MODEM_CFG_OS_RCEVT_SIGEX_TYPE, 
         &rcevt_sigex))
      {
        MCFG_MSG_ERROR("fail to unregister rcevt for MMGSDI READY");
      }

      sigs_mask &= ~MODEM_CFG_MMGSDI_READY_SIG;
      mcfg_uim_mmgsdi_init();
    }

    if (received_sigs & MODEM_CFG_CM_READY_SIG)
    {
      /* Process command queue signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_CM_READY_SIG);
      if(!rcevt_unregister_sigex_name(
         MCFG_RCEVT_NAME_CM_READY, MODEM_CFG_OS_RCEVT_SIGEX_TYPE, 
         &rcevt_sigex))
      {
        MCFG_MSG_ERROR("fail to unregister rcevt for CM READY");
      }

      sigs_mask &= ~MODEM_CFG_CM_READY_SIG;
#ifdef FEATURE_MCFG_REFRESH
      mcfg_refresh_cm_init();
#endif /* FEATURE_MCFG_REFRESH */
    }

    if (received_sigs & MODEM_CFG_SSR_SHUTDOWN_SIG)
    {
      /* Process SSR shutdown signal */
      modem_cfg_os_clr_sigs (mcfg_tcb, MODEM_CFG_SSR_SHUTDOWN_SIG);

      mcfg_utils_cmd_send(MCFG_UTILS_CMD_CLEAR_RESET_TIMER);
      rcinit_handshake_term(); // required to handshake
    }

    if (mcfg_efslog_enabled)
    {
      MCFG_MSG_HIGH("Disabling EFS logging..");
      mcfg_efslog_enabled = FALSE;
    }
  }

  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(modem_cfg_dog_rpt_var);
} /* modem_cfg_task() */

/*===========================================================================

  FUNCTION mcfg_task_cmd_alloc

  DESCRIPTION
    Allocates and initializes a command structure. The pointer returned
    by this function (if not NULL) must be either passed to
    mcfg_task_cmd_send() to have the command processed from the context of
    the MCFG task, or passed to mcfg_task_cmd_free() to abort the command.

  DEPENDENCIES
    None

  PARAMETERS
    cmd_id    [in]
    data_size [in] Size of data to allocate in the cmd->data structure. Can
    be 0. The resulting cmd->data buffer will be zero-initialized.

  RETURN VALUE
    Pointer to command structure, or NULL on failure

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_task_cmd_s_type *mcfg_task_cmd_alloc
(
  mcfg_task_cmd_id_e_type cmd_id,
  uint32                  data_size
)
{
  uint32 alloc_size;
  uint32 pad = 0;
  mcfg_task_cmd_s_type *cmd = NULL;

  if (cmd_id <= MCFG_TASK_CMD_MIN || cmd_id >= MCFG_TASK_CMD_MAX)
  {
    MCFG_MSG_ERROR_1("Invalid cmd_id %d", cmd_id);
  }
  else
  {
    /* Bundle the allocation of the command structure and the additional data
       buffer to reduce overhead. */
    if (data_size > 0)
    {
      pad = (sizeof(mcfg_task_cmd_s_type) % MCFG_TASK_CMD_DATA_ALIGNMENT);
      if (pad != 0)
      {
        pad = (MCFG_TASK_CMD_DATA_ALIGNMENT - pad);
      }
      alloc_size = (sizeof(mcfg_task_cmd_s_type) + pad + data_size);
    }
    else
    {
      alloc_size = sizeof(mcfg_task_cmd_s_type);
    }

    cmd = mcfg_malloc(alloc_size);
    if (cmd == NULL)
    {
      MCFG_MSG_ERROR_3("Couldn't allocate cmd struct for cmd_id %d with %lu "
                       "bytes of additonal data (total size %lu)",
                       cmd_id, data_size, alloc_size);
    }
    else
    {
      memset(cmd, 0, alloc_size);
      cmd->cmd_id = cmd_id;
      (void) q_link(cmd, &cmd->link);
      if (data_size > 0)
      {
        cmd->data = ((uint8 *) cmd + sizeof(mcfg_task_cmd_s_type) + pad);
      }
    }
  }

  return cmd;
} /* mcfg_task_cmd_alloc() */

/*===========================================================================

  FUNCTION mcfg_task_cmd_free

  DESCRIPTION
    Frees memory associated with a command allocated by mcfg_task_cmd_alloc().
    This is only expected to be called from outside mcfg_task in the event
    of an unexpected error. It MUST NOT be called during regular command
    processing, as the mcfg_task layer calls it internally.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_task_cmd_free
(
  mcfg_task_cmd_s_type *cmd
)
{
  if (cmd != NULL)
  {
    mcfg_free(cmd);
  }
} /* mcfg_task_cmd_free() */

/*===========================================================================

  FUNCTION mcfg_task_cmd_send

  DESCRIPTION
    Posts a command to the MCFG task for processing.

  DEPENDENCIES
    The command structure must have been allocated and initialized via
    mcfg_task_cmd_alloc().

  PARAMETERS
    None

  RETURN VALUE
    TRUE on success, FALSE on failure

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_task_cmd_send
(
  mcfg_task_cmd_s_type *cmd
)
{
  boolean result = FALSE;

  if (mcfg_tcb == NULL)
  {
    MCFG_MSG_ERROR("Tried to send command but MCFG task not ready!");
  }
  else if (cmd == NULL)
  {
    MCFG_MSG_ERROR("Null pointer given to mcfg_task_cmd_send()");
  }
  else if (cmd->cmd_id <= MCFG_TASK_CMD_MIN || cmd->cmd_id >= MCFG_TASK_CMD_MAX)
  {
    MCFG_MSG_ERROR_1("Invalid cmd_id %d", cmd->cmd_id);
  }
  else
  {
    q_put(&mcfg_task_cmd_q, &cmd->link);
    modem_cfg_os_set_sigs(mcfg_tcb, MODEM_CFG_CMD_Q_SIG);
    result = TRUE;
  }

  return result;
} /* mcfg_task_cmd_send() */

void modem_cfg_init_before_task_start(void)
{
  /* Empty */
}

/*===========================================================================

  FUNCTION mcfg_task_bootup_completed

  DESCRIPTION
    This function check if MCFG task is fully booted up or not

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    TRUE if MCFG task is fully booted up

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_task_bootup_completed
(
  void
)
{
  return mcfg_bootup_completed;
}

