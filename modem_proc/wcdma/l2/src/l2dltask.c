/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===

        L A Y E R  2   D O W N L I N K   TASK

GENERAL DESCRIPTION
  This file contains the Downlink Layer 2 task function.

Copyright (c) 2000 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2001-2002,  2005-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================================*/
/*=============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/L2/vcs/l2dltask.c_v   1.11   11 Jun 2002 18:09:22   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l2/src/l2dltask.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------------
09/04/14   scv     L2 changes for FEATURE_WCDMA_DIME_SW cleanup
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
07/01/14   aa      CFCM changes for RLC
11/26/13   geg     Segment loading FSL: add dummy task 
11/12/13   kv      Removed deprecated header tmc.h 
03/26/13   scv     L2 changes to support new dog architecture 
01/29/13   ksr     Disabled the registration with CPU based FC mechanism
01/04/13   kc      Removing duplicate/redundant code
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
03/28/12   zr      Adding feature definitions for Dime
03/08/12   amj     Added changes for RC init task initialization.
02/01/12   grk     Feture cleanup
02/28/11   sv      Support for transition from TMC heap to Modem Heap in WCDMA.
10/18/10   ssg     Added changes for new WCDMA-L2 interface.
09/17/10   grk     Moving Dynamic Dog API changes under FEATURE_WCDMA_L2_WDOG_DYNAMIC.
09/14/10   grk     Support to use Dynamic DOG APIs to register L2 tasks for dog monitoring.
07/07/10   pj      Added changes to use a global base timer for status processing
04/12/10   grk     Fixed compilation error on silver MOB.
                   1. Removed test header inclusion in source files.
                   2. Stubbed L2 function calls.
03/30/10   grk     Changes for RLC QTF.
12/16/09   grk     Fixed KW warnings.
12/04/09   ssg     New L2 interface
10/08/09   pj      Added changes to process FC-commands before PDU-processing command
05/07/09   ssg     Updated Copyright Information
05/04/09   kvk     Added support to send the FC command for WINDOW size update 
                   rather than directly updating the WINDOW size from timer CB function
01/28/09   grk     Merged missed out fixes from taxis to mainline.
12/18/08   ksr     L2 Mutex changes to avoid concurrency issues
05/16/08   ssg     Added new cmd CRLC_DL_CONTINUE_REQ.
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
03/17/08   ar      Added support for DL flow control through rlc_dl_fc_init()
                   rlc_dl_fc_register() and appropriate signal handling.
02/01/08   pj      Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
11/30/07   ssg     Added mbms support under FEATURE_MBMS.
01/04/07   ssg     Added handling for RLC_DL_LOG_CIPHER_PKT_SIG.
11/10/06   vg      added WM clenup cmd handling for PDCP
04/26/06   gsc     Added support for HSDPA Re-config optimizations under
                   FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
11/11/05   pdv     Fix for CR 79364.
11/10/05   gsc/sk  Call proc_dl_mac_config_at_l1_failure_signal in the processing of
                   MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND
08/08/05   gsc     Removed handling of MAC_HS_CONFIG_COMPLETE_SIG.
07/30/05   aw      Added the command RLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT to
                   post RLC SDU discard timer timeout event.
07/20/05   mv      Fixed lint errors and warnings.
05/03/05   mv      Added the command CRLC_DL_ABORT_CIPHER_CONFIG_REQ to abort the new
                   ciphering configuration.
03/04/05   gsc     Merged HSDPA support from l2_msm6275 branch.
12/10/02   ttl     Added RLC_DL_TIMER_BACKUP_SIG for rlc backup timer.
09/04/02   sk      Added function inti_dl_mac().
07/31/02   ttl     Removed RLC_DL_AM0_TIMER_EXP_SIG, RLC_DL_AM1_TIMER_EXP_SIG,
                   RLC_DL_AM2_TIMER_EXP_SIG and RLC_DL_AM3_TIMER_EXP_SIG.
06/06/02   sk      Handled the processing of the signal MAC_PHY_DL_CONFIG_IND.Its under
                   FEATURE_MAC_ACTIVATION_TIME
05/14/02   ttl     Naming changed of wcdma_l2_dl_task and wcdma_l2_dl_tcb.
05/12/02   ttl     Added primitive RLC_DL_DEREGISTER_SRVC_REQ.
03/15/02   sk      Added support in l2_dl_task() to handle the expiry of qxdm log timer signal.
11/09/01   sk      Added support in l2_dltask() to handle the timer signals.
09/18/01   ttl     Added watchdog for L2 DL.
06/18/01   rc      Added support for processing downlink PDUs in Task or interrupt context.
06/07/01   rc      Added primitive RLCI_DL_RELEASE_ALL_REQ.
03/26/01   ttl     Added primitive messages RLC_REGISTER_AM_SDU_DISCARD_CALLBACK_REQ,
                   RLCI_DL_RESET_REQ and RLCI_UL_RESET_DONE.

=============================================================================================*/

/*===============================================================================================

              I N C L U D E    F I L E S

=============================================================================================*/
#include "wcdma_variation.h"
#include  "rex.h"
#include  "task.h"

#ifdef FEATURE_DOG
  #include  "dog.h"
#endif

#include  "msg.h"
#include  "err.h"
#include  "l2dltask.h"
#include  "macdlsubtask.h"
#include  "rlcdl.h"
#include  "rlcdlsubtask.h"
#include  "rlcdlam.h"
#include  "rlcfc.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#endif /* FEATURE_SEGMENT_LOADING */
/*=============================================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

=============================================================================================*/


/* Downlink layer 2 command queue */
q_type          l2_dl_cmd_q;
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
q_type          wcdma_l2_dl_cmd_q;
#endif

#ifdef FEATURE_DOG
#ifdef FEATURE_WCDMA_L2_WDOG_DYNAMIC   /* Dynamic Dog Interface */
  #undef  DOG_WCDMA_L2DL_RPT
  #define DOG_WCDMA_L2DL_RPT wcdma_l2dl_dog_rpt_var
  dog_report_type   wcdma_l2dl_dog_rpt_var           = 0;      /* Initial Safety */

#endif   /* FEATURE_WCDMA_L2_WDOG_DYNAMIC  */

/* L2 DL watchdog report timer block  */
rex_timer_type  l2_dl_dog_rpt_timer_blk;
#endif

extern rlc_fc_type rlc_dl_fc;

/* WCDMA L2 DL TCB handle */
static rex_tcb_type* wcdma_l2_dl_tcb_ptr = RCINIT_NULL;

#ifdef FEATURE_SEGMENT_LOADING
extern void wcdma_task_null (void);
#endif /* FEATURE_SEGMENT_LOADING */

/*=============================================================================================

FUNCTION  l2_dl_wait

DESCRIPTION
  This function waits for any signal in a specified signal mask to be set.
  If this procedure detects that any signal in the specified signal mask is
  set, it will return the entire mask of signals set in the task's tcb.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of signals returned by the rex_wait call.

SIDE EFFECTS
  None.

=============================================================================================*/

uint32  l2_dl_wait
(
  /* signals to wait on */
  uint32  wait_sigs
)
{
  /* signals returned by rex_wait call */
  uint32  sigs;

    /* Wait for requested signals */
  while (((sigs = rex_get_sigs( rex_self()) ) & wait_sigs) == 0 ) {

    (void)rex_wait(wait_sigs);

  }

  return( sigs );

} /* l2_dl_wait */

/*=============================================================================================

FUNCTION  proc_l2_dl_cmd_queue

DESCRIPTION
  This function processes the downlink layer 2 command queue data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  proc_l2_dl_cmd_queue ( void )
{
  l2_dl_cmd_enum_type   cmd_id;
  l2_dl_cmd_type        *cmd_ptr;

  while ((cmd_ptr = q_get(&l2_dl_cmd_q)) != NULL) {

    /* Get the command ID */
    cmd_id = cmd_ptr->cmd_hdr.cmd_id;

    /* Process the command queue data based on the command ID */
    switch (cmd_id) {

      case RLC_DL_REGISTER_SRVC_REQ:
      case RLC_DL_DEREGISTER_SRVC_REQ:
        proc_dl_rlc_commands(cmd_id, &cmd_ptr->cmd_data);

        break;

      /* Invalid command */
      default:
        ERR ("Invalid command:%d", cmd_ptr->cmd_hdr.cmd_id, 0,0  );
        break;
    }/* switch cmd-id */

    /* Free the command buffer */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
    modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_WCDMA_EXT_CMD_CRIT);
#else
    /*lint -e424 */
    mem_free (&tmc_heap, cmd_ptr);
    /*lint +e424 */
#endif /* FEATURE_WCDMA_USE_MODEM_HEAP */

  }/* while (cmd_ptr != NULL) */

} /* proc_l2_dl_cmd_queue */

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
/*=============================================================================================

FUNCTION  proc_wcdma_l2_dl_cmd_queue

DESCRIPTION
  This function processes the downlink layer 2 command queue data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  proc_wcdma_l2_dl_cmd_queue ( void )
{
  wcdma_l2_dl_cmd_enum_type   cmd_id;
  wcdma_l2_dl_cmd_type        *cmd_ptr;

  while ((cmd_ptr = q_get(&wcdma_l2_dl_cmd_q)) != NULL) {

    /* Get the command ID */
    cmd_id = cmd_ptr->cmd_hdr.cmd_id;

    /* Process the command queue data based on the command ID */
    switch (cmd_id) {

      /* DL MAC Commands */
      case CMAC_DL_CONFIG_REQ:
        MSG(MSG_SSID_WCDMA_L2, MSG_LEGACY_LOW , "Rcvd CMAC_DL_CONFIG_REQ");
        proc_dl_mac_commands (cmd_id, &cmd_ptr->cmd_data);
        break;

      /* DL RLC Commands */
      case RLCI_DL_TX_AM_STATUS:
      case CRLC_DL_TM_CONFIG_REQ:
      case CRLC_DL_UM_CONFIG_REQ:
      case CRLC_DL_CIPHER_CONFIG_REQ:
      case CRLC_DL_HFN_REQ:
      case RLC_REGISTER_AM_SDU_DISCARD_CALLBACK_REQ:
      case RLCI_DL_CONFIG_REQ:
      case RLCI_DL_RESET_REQ:       /* RESET request for DL AM RLC  */
      case RLCI_UL_RESET_DONE:      /* Confirm the RESET completion of UL AM RLC  */
      case RLCI_DL_RELEASE_ALL_REQ:
      case RLC_PROCESS_MAC_HS_RESET_REQ:

      case CRLC_DL_ABORT_CIPHER_CONFIG_REQ:

     /* Command to handle SDU discard timeout  */
     case RLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT:
      case RLC_PDCP_CLEANUP_WM:

      case CRLC_DL_CONTINUE_REQ:

        proc_wcdma_dl_rlc_commands(cmd_id, &cmd_ptr->cmd_data);

        break;

      /* Invalid command */
      default:
        ERR ("Invalid command:%d", cmd_ptr->cmd_hdr.cmd_id, 0,0  );
        break;
    }/* switch cmd-id */

    /* Free the command buffer */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
    modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_WCDMA_EXT_CMD_CRIT);
#else
    /*lint -e424 */
    mem_free (&tmc_heap, cmd_ptr);
    /*lint +e424 */
#endif /* FEATURE_WCDMA_USE_MODEM_HEAP */

  }/* while (cmd_ptr != NULL) */

} /* proc_wcdma_l2_dl_cmd_queue */
#endif

/*=============================================================================================

FUNCTION  wcdma_l2_dl_task_init

DESCRIPTION
  This function performs initializations that the wcdma_l2_dl_task_init would need
  before the task is defined.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
void  wcdma_l2_dl_task_init ( void ) __attribute__((section(".task_text")));
#endif /* FEATURE_SEGMENT_LOADING */
void  wcdma_l2_dl_task_init ( void )
{
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *pi_wcdma_segment = NULL;
  pi_wcdma_segment = get_wcdma_interface();
  if(pi_wcdma_segment == NULL)
  {
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */
  /* Initialize downlink layer 2 task command and free command queues */
  (void)q_init(&l2_dl_cmd_q);

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  (void)q_init(&wcdma_l2_dl_cmd_q);
#endif
}

/*=============================================================================================

FUNCTION  wcdma_l2_dl_task

DESCRIPTION
  This task performs downlink layer 2 processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/
#ifdef FEATURE_SEGMENT_LOADING
void wcdma_l2_dl_task(dword dummy) __attribute__((section(".task_text")));
#endif /* FEATURE_SEGMENT_LOADING */
void  wcdma_l2_dl_task
(
  dword dummy
    /* Required for REX, ignore */
    /*lint -esym(715,dummy)
    ** Have lint not complain about the ignored parameter 'dummy' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  uint32    sigs;

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *pi_wcdma_segment = NULL;

  pi_wcdma_segment = get_wcdma_interface();
  if(pi_wcdma_segment == NULL)
  {
    wcdma_task_null ();
    return;
  }
#endif /* FEATURE_SEGMENT_LOADING */

#ifdef FEATURE_MODEM_RCINIT 
  /* Handshake with the RC Init */
  rcinit_handshake_startup();
#else
  wcdma_l2_dl_task_init();
  /* Waits for start signal from TMC */
  /* Sets ACK signal when receives the task start signal */
  tmc_task_start();
#endif

#ifdef FEATURE_DOG
#ifndef FEATURE_WCDMA_L2_WDOG_DYNAMIC
  /* Setup watchdog timer */
  rex_def_timer(&l2_dl_dog_rpt_timer_blk, wcdma_l2_dl_get_tcb(), L2_DL_DOG_RPT_TIMER_SIG);

  (void)rex_set_timer(&l2_dl_dog_rpt_timer_blk, DOG_L2DL_RPT_TIME);
#else
  /*Register with DOG, get the Task ID*/
  wcdma_l2dl_dog_rpt_var      = dog_hb_register_rex((rex_sigs_type)L2_DL_DOG_RPT_TIMER_SIG);
#endif
#endif

  rlc_dl_init();

  /* Initilize the parameters for DL mac */
  init_dl_mac();

  /* Initialize RLC-DL flow control */
  rlc_dl_fc_init();

  /* Task loop */
  /*lint -e716 */
  while (1)
  /*lint +e716 */
  {

    /* Wait for DL L2 signal to be set */
      sigs = l2_dl_wait(L2_DL_CMD_Q_SIG |
                        CMAC_DL_HFN_REQ_SIG |
                      RLC_DL_TIMER_BACKUP_SIG |

#ifdef FEATURE_DOG
                        L2_DL_DOG_RPT_TIMER_SIG |
#endif

                        MAC_PHY_DL_CONFIG_IND |
                        MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND |
                        MAC_PHY_HS_CONFIG_IND|

                        RLC_DL_FC_SIG|
                        RLC_DL_FC_TIMER_SIG|

#ifdef FEATURE_RLC_QXDM_LOGGING
                        RLC_DL_AM_LOG_TIMER_EXP_SIG |
                        RLC_DL_LOG_CIPHER_PKT_SIG |
#endif /* FEATURE_RLC_QXDM_LOGGING */
#if 0
//#ifdef FEATURE_Q6_MT
                        RLC_DL_DATA_REASSEMBLY_SIG |
#endif
                        RLC_DL_AM_BASE_TIMER_SIG  |
                        RLC_DL_DATA_IND_SIG
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
                        | WCDMA_L2_DL_CMD_Q_SIG
#endif
                        );

    if (sigs & RLC_DL_TIMER_BACKUP_SIG)
    {
      /* Clear DL watchdog report timer sig */
      (void)rex_clr_sigs (rex_self(), RLC_DL_TIMER_BACKUP_SIG);
      rlc_dl_am_backup_timer();
    }

#ifdef FEATURE_DOG
    /* Downlink L2 Command Queue Signal is set */
    if (sigs & L2_DL_DOG_RPT_TIMER_SIG)
    {
      /* Clear DL watchdog report timer sig */
      (void)rex_clr_sigs (rex_self(), L2_DL_DOG_RPT_TIMER_SIG);
#ifndef FEATURE_WCDMA_L2_WDOG_DYNAMIC
      /* Kick watchdog */
      dog_report(DOG_L2DL_RPT);
      //MSG_HIGH("Kick watchdog l2dl", 0, 0, 0);
      /* Set watchdog report timer for next report  */
      (void)rex_set_timer(&l2_dl_dog_rpt_timer_blk, DOG_L2DL_RPT_TIME);
#else
      dog_hb_report(wcdma_l2dl_dog_rpt_var);
#endif
    }
#endif /* FEATURE_DOG */

#ifdef FEATURE_RLC_QXDM_LOGGING

    if (sigs & RLC_DL_AM_LOG_TIMER_EXP_SIG)
    {
      /* Clear DL AM log Timer expiration signal  */
      (void)rex_clr_sigs (rex_self(), RLC_DL_AM_LOG_TIMER_EXP_SIG);

      /* Call AM log timer expiration handler  */
      rlci_dl_am_handle_qxdm_log_timer(RLC_DL_AM_LOG_TIMER_EXP_SIG);
    }

    if (sigs & RLC_DL_LOG_CIPHER_PKT_SIG)
    {
      /* Clear the signal */
      (void)rex_clr_sigs(rex_self(), RLC_DL_LOG_CIPHER_PKT_SIG);

      /*---------------------------------------------------------------------
        Process the DL cipher packet logging request.
      ---------------------------------------------------------------------*/
      rlc_dl_log_cipher_pkt();    
    }

#endif /* FEATURE_RLC_QXDM_LOGGING */

    if (sigs & L2_DL_CMD_Q_SIG)
    {

      /* Clear downlink Layer 2 Command Queue Signal */
      (void)rex_clr_sigs (rex_self(), L2_DL_CMD_Q_SIG);

      /* Process downlink layer 2 command queue */
      proc_l2_dl_cmd_queue ();

    }

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
    if (sigs & WCDMA_L2_DL_CMD_Q_SIG)
    {
      /* Clear downlink Layer 2 Command Queue Signal */
      (void)rex_clr_sigs (rex_self(), WCDMA_L2_DL_CMD_Q_SIG);

      /* Process downlink layer 2 command queue */
      proc_wcdma_l2_dl_cmd_queue ();
    }
#endif

    /* DL MAC HFN Request Signal is set */
    if (sigs & CMAC_DL_HFN_REQ_SIG)
    {

      /* Clear downlink MAC HFN Request Signal */
      (void)rex_clr_sigs (rex_self(), CMAC_DL_HFN_REQ_SIG);

      /* Process downlink MAC HFN request */
      //proc_dl_mac_hfn_req ();
    }

    if (sigs & RLC_DL_FC_SIG)
    {
      (void)rex_clr_sigs(rex_self(), RLC_DL_FC_SIG);
      rlc_fc_cmd_hdlr(RLC_FC_DOWNLINK);
    }

    if (sigs & RLC_DL_FC_TIMER_SIG)
    {
      (void)rex_clr_sigs(rex_self(), RLC_DL_FC_TIMER_SIG);
      rlc_fc_timer_cmd_hdlr(RLC_FC_DOWNLINK);
    }

    if (sigs & RLC_DL_DATA_IND_SIG)
    {

      /* Clear downlink MAC HFN Request Signal */
      (void)rex_clr_sigs (rex_self(), RLC_DL_DATA_IND_SIG);

      /* Process RLC Data Ind */
      rlc_enh_dl_pdu_data_handler();
    }

//#ifdef FEATURE_Q6_MT
#if 0
    if (sigs & RLC_DL_DATA_REASSEMBLY_SIG)
    {
      MSG(MSG_SSID_WCDMA_L2, MSG_LEGACY_MED, "L2 DL Task: Processing Reassembly SIGNAL");
      /* Clear downlink MAC HFN Request Signal */
      (void)rex_clr_sigs (rex_self(), RLC_DL_DATA_REASSEMBLY_SIG);

      /* Process RLC Data Ind */
      rlc_enh_dl_data_reassembly_signal();
    }
#endif

    if (sigs & MAC_PHY_DL_CONFIG_IND)
    {
      /* Clear DL config signal received from L1 */
      (void)rex_clr_sigs (rex_self(), MAC_PHY_DL_CONFIG_IND );

      /* Process the signal to configure DL MAC */
      proc_dl_mac_config_signal();
    }

    if (sigs & MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND)
    {
      /* Clear DL config failed signal received from L1 */
      (void)rex_clr_sigs (rex_self(), MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND );

      /* Process the signal to configure DL MAC */
      proc_dl_mac_config_at_l1_failure_signal();
    }

    if (sigs & MAC_PHY_HS_CONFIG_IND)
    {
      /* Clear MAC HS config signal received from L1 */
      (void)rex_clr_sigs (rex_self(), MAC_PHY_HS_CONFIG_IND );

      /* Process the signal to configure DL MAC */
      proc_dl_mac_hs_config_signal();

    }

    if (sigs & RLC_DL_AM_BASE_TIMER_SIG)
    {
      /* Clear MAC HS config signal received from L1 */
      (void)rex_clr_sigs (rex_self(), RLC_DL_AM_BASE_TIMER_SIG );

      /* Process the signal to configure DL MAC */
      rlci_dl_am_base_timer_handler();

    }
   } /* while loop */
   
   rlc_dl_deallocate_mem_ZI_opt();

#ifdef FEATURE_DOG
#ifdef FEATURE_WCDMA_L2_WDOG_DYNAMIC
   dog_hb_deregister(wcdma_l2dl_dog_rpt_var);
#endif
#endif

}/* l2_dl_task */

/*==============================================================================================

FUNCTION  l2_dl_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from downlink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to the downlink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

=============================================================================================*/
l2_dl_cmd_type    *l2_dl_get_cmd_buf
(
  void
)
{
  void * return_ptr =  NULL;
  size_t cmd_size = sizeof(l2_dl_cmd_type);

#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
  return_ptr = modem_mem_calloc(1,cmd_size, MODEM_MEM_CLIENT_WCDMA_EXT_CMD_CRIT);
#else
  return_ptr = mem_malloc (&tmc_heap, cmd_size);
#endif

  if(return_ptr == NULL)
  {
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
    ERR_FATAL("Failed to allocate from Modem Mem Heap: size:0x%x",cmd_size,0,0);
#else
    ERR_FATAL("Failed to allocate from TMC Heap: size:0x%x",cmd_size,0,0);
#endif
  }

  return (return_ptr);

} /* l2_dl_get_cmd_buf */

/*==============================================================================================

FUNCTION  wcdma_l2_dl_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from downlink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to the downlink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

=============================================================================================*/
wcdma_l2_dl_cmd_type    *wcdma_l2_dl_get_cmd_buf
(
  void
)
{
  void * return_ptr =  NULL;
  size_t cmd_size = sizeof(wcdma_l2_dl_cmd_type);

#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
  return_ptr = modem_mem_calloc(1,cmd_size, MODEM_MEM_CLIENT_WCDMA_EXT_CMD_CRIT);
#else
  return_ptr = mem_malloc (&tmc_heap, cmd_size);
#endif

  if(return_ptr == NULL)
  {
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
    ERR_FATAL("Failed to allocate from Modem Mem Heap: size:0x%x",cmd_size,0,0);
#else
    ERR_FATAL("Failed to allocate from TMC Heap: size:0x%x",cmd_size,0,0);
#endif
  }

  return (return_ptr);

} /* wcdma_l2_dl_get_cmd_buf */

/*=============================================================================================

FUNCTION  l2_dl_put_cmd

DESCRIPTION
  This function should be called to send a command to the downlink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for downlink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

=============================================================================================*/

void  l2_dl_put_cmd
(

  l2_dl_cmd_type    *cmd_ptr
    /* pointer to command to be enqueued */
)
{
#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  if ((cmd_ptr->cmd_hdr.cmd_id >= RLC_DL_REGISTER_SRVC_REQ) &&
     ((cmd_ptr->cmd_hdr.cmd_id <= RLC_DL_DEREGISTER_SRVC_REQ)))
  {
#endif  
  /* initialize link field */
  (void)q_link(cmd_ptr, &cmd_ptr->cmd_hdr.link );

  /* put item on queue for wcdma_l2_dl_task */
  q_put( &l2_dl_cmd_q, &cmd_ptr->cmd_hdr.link );

  /* Set signal for the l2_dl_cmd_q */
  (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), L2_DL_CMD_Q_SIG );

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
  }
  else
  {
    ERR_FATAL("Invalid L2 DL command  %d ",cmd_ptr->cmd_hdr.cmd_id,0,0);    
  }
#endif

} /* l2_dl_put_cmd */

/*=============================================================================================

FUNCTION  wcdma_l2_dl_put_cmd

DESCRIPTION
  This function should be called to send a command to the downlink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for downlink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

=============================================================================================*/

void  wcdma_l2_dl_put_cmd
(

  wcdma_l2_dl_cmd_type    *cmd_ptr
    /* pointer to command to be enqueued */
)
  {
  /* initialize link field */
  (void)q_link(cmd_ptr, &cmd_ptr->cmd_hdr.link );

  /* put item on queue for wcdma_l2_dl_task */
  q_put( &wcdma_l2_dl_cmd_q, &cmd_ptr->cmd_hdr.link );

  /* Set signal for the l2_dl_cmd_q */
  (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), WCDMA_L2_DL_CMD_Q_SIG );
} /* wcdma_l2_dl_put_cmd */

/*===========================================================================
 FUNCTION     WCDMA_L2_DL_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA L2 DL TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
void wcdma_l2_dl_init_tcb(void)
{
  if (wcdma_l2_dl_tcb_ptr == RCINIT_NULL)
  {
    wcdma_l2_dl_tcb_ptr = rcinit_lookup_rextask("wcdma_l2_dl"); 
    if (wcdma_l2_dl_tcb_ptr == RCINIT_NULL)
    {
      MSG(MSG_SSID_WCDMA_L2, MSG_LEGACY_ERROR, "Lookup for wcdma_l2_dl_tcb failed" );
      ASSERT(0);
    }
  }
}

/*===========================================================================
 FUNCTION     WCDMA_L2_DL_GET_TCB

 DESCRIPTION
   This function returns a pointer to the WCDMA L2 DL TCB. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   A pointer to the WCDMA L2 DL TCB.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
rex_tcb_type* wcdma_l2_dl_get_tcb(void)
{
  if (wcdma_l2_dl_tcb_ptr == RCINIT_NULL)
  {
    MSG(MSG_SSID_WCDMA_L2, MSG_LEGACY_ERROR, "Trying to get tcb before WL2 DL is initialized");
    wcdma_l2_dl_init_tcb();
  }
  return wcdma_l2_dl_tcb_ptr;
}

