/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wms.c --

  The WMS module which implements the User API for SMS. This source file
  contains the functions for the client applications to call. It also
  implements some internal functions such as module initialization,
  processing of signals, commands, reports and timers.
  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wms.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#ifdef FEATURE_CMI
#include "wmstask.h"
#endif /* FEATURE_CMI */
#include "wmsi.h"
#include "wmsll.h"
#include "msg.h"
#include "err.h"
#include "dog_hb_rex.h"
#include "wmscl.h"
#include "wmscfg.h"
#include "wmsmsg.h"
#include "wmsnv.h"
#include "wmscm.h"
#include "queue.h"
#include "err.h"
#include "wmsdc.h"
#include "wmsbc.h"
#include "wmsts.h"
#include "wmsdbg.h"
#include "wmsutils.h"

#if defined(FEATURE_MMGSDI) ||  defined(FEATURE_GWSMS)
#include "wmssim.h"
#endif /* FEATURE_MMGSDI */

#include "wmsdiag.h"

#include "wmscm_gw.h"


#ifdef FEATURE_CDSMS
#include "mc.h"
#endif /* FEATURE_CDSMS */

#include "wmsi.h"
#include "msgr_rex.h"
#include "wms_msgr_msg.h"
#include "cm_msgr_msg.h"

#ifdef FEATURE_FUZZ_TEST
#include "event.h"
#endif /* FEATURE_FUZZ_TEST */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#include "rcevt_rex.h"
#endif /* FEATURE_MODEM_RCINIT */

#include "qmi_wms.h"
#include "stringl.h"

#ifdef FEATURE_WCDMA
#include "rrcmmif.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "tdsrrcmmif.h"
#endif /*FEATURE_TDSCDMA*/

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"

#ifdef FEATURE_WCDMA
#include "IWCDMA.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "ITDSCDMA.h"
#endif /* FEATURE_TDSCDMA */

#endif /* FEATURE_SEGMENT_LOADING */

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static q_type               wms_cmd_q;
static int                  wms_cmd_avail_cnt = WMS_MAX_CMDS;

#define WMS_MSGR_Q_SIZE     20
#define WMS_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))
static q_type               wms_msgr_q;
static q_type               wms_msgr_free_q;
static wms_msgr_cmd_s_type  wms_msgr_bufs[WMS_MSGR_Q_SIZE];
static msgr_client_t        wms_msgr_client;
static msgr_id_t            wms_msgr_rex_q_id;

/* Define watchdog report handle */
static dog_report_type      wms_reg_dog_rpt_var  = 0;

// Define a variable to hold the WMS task TCB.  This is done for
// a several reasons:
//  1) The return value of rex_self() never changes at runtime, so
//     there is nothing gained by repeatedly fetching it.
//  2) Directly using wms_tcb requires that 'task.h' be included.
//     It is now prohibited to include 'task.h'.
//  3) The use of rex_self() cannot be used in any context where
//     the wms_tcb is specifically required (such as when a command
//     is placed into a queue and the task signal is set).
//  4) A task's tcb is not available if the task is not started.
//     Worse, signaling a task that isn't running causes an exception.
//  5) The variable allows access to the correct tcb in case 3 and
//     provides state to use in determining if the task is running.
static rex_tcb_type *wms_myself = NULL;  // To be initialized to rex_self()
                                         // when task is running

/* Critsect wms_cmd_queue_crit_sect protects:
 *   - variable "wms_cmd_avail_cnt"
 *   - alloc/free of command buffers (wms_get_cmd_buf)
 */
static rex_crit_sect_type           wms_cmd_queue_crit_sect;

/*===========================================================================
=================== FORWARD DECLARATIONS ====================================
===========================================================================*/
extern void wms_cfg_print_active_bearers( void );

/*===========================================================================
=============================== MISCELLANEOUS =================================
================================= FUNCTIONS =================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_offline()                                             (INTERNAL)

DESCRIPTION
  Get ready for phone going offline.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void wms_offline(
  void
)
{
  wms_msg_offline();

  return;
} /* wms_offline() */

/*===========================================================================
FUNCTION wms_kick_dog()                                            (INTERNAL)

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void wms_kick_dog(
  void
)
{
  (void)rex_clr_sigs( wms_myself, WMS_RPT_TIMER_SIG );
  dog_hb_report( wms_reg_dog_rpt_var );
} /* wms_kick_dog() */

/*===========================================================================
FUNCTION wms_msgr_init()                                           (INTERNAL)

DESCRIPTION
  Initialize WMS message router .
  This function must be called before any other part of CM is initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void wms_msgr_init(void)
{
  /* Message Router */
  errno_enum_type       msgr_error;
  byte                  count = 0;

  MSG_HIGH_0("wms_msgr_init()");

  /* Initialize MSGR Client and register for messages */
  (void)q_init(&wms_msgr_free_q);
  (void)q_init(&wms_msgr_q);

  /*( Initialize the free Q with cmd type structures*/
  for(count = 0; count < WMS_MSGR_Q_SIZE; count++)
  {
    q_put(&wms_msgr_free_q, q_link(&wms_msgr_bufs[count], &wms_msgr_bufs[count].link));
  }

  /* Create wms_msgr_client*/
  msgr_error = msgr_client_create(&wms_msgr_client);
  if (msgr_error != E_SUCCESS) {
      MSG_ERROR_1( "wms_msgr_create() faild with error = %d", msgr_error);
      return;
  }

  msgr_error = msgr_client_add_rex_q(  &wms_msgr_client,
                                       wms_myself,
                                       WMS_MSGR_Q_SIG,
                                       &wms_msgr_q,
                                       &wms_msgr_free_q,
                                       (uint16)WMS_OFFSETOF( wms_msgr_cmd_s_type, cmd ),
                                       MSGR_NO_OFFSET,
                                       sizeof(wms_msgr_cmd_s_type),
                                       &wms_msgr_rex_q_id
                                       );

  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_client_add_rex_q() failed with error = %d", msgr_error);
      return;
  }

#ifdef FEATURE_SMS_PWS_OVER_LTE
  /* Register with MSG ROUTER for ETWS primary notification */
  msgr_error = msgr_register(MSGR_WMS_LTE, &wms_msgr_client, wms_msgr_rex_q_id, LTE_RRC_ETWS_PRIM_IND);
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(LTE_RRC_ETWS_PRIM_IND) failed with msgr error = %d", msgr_error);
  }

  /* Register with MSG ROUTER for ETWS secondary notification */
  msgr_error = msgr_register(MSGR_WMS_LTE, &wms_msgr_client, wms_msgr_rex_q_id, LTE_RRC_ETWS_SEC_IND);
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(LTE_RRC_ETWS_SEC_IND) failed with msgr error = %d", msgr_error);
  }

  /* Register with MSG ROUTER for CMAS notification */
  msgr_error = msgr_register(MSGR_WMS_LTE, &wms_msgr_client, wms_msgr_rex_q_id, LTE_RRC_CMAS_IND);
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(LTE_RRC_CMAS_IND) failed with msgr error = %d", msgr_error);
  }

  /* Register with MSG ROUTER for LTE CELL CHANGE notification */
  msgr_error = msgr_register(MSGR_WMS_LTE, &wms_msgr_client, wms_msgr_rex_q_id, LTE_RRC_SERVICE_IND);
  if (msgr_error != E_SUCCESS)
  {
    MSG_ERROR_1("msgr_register(LTE_RRC_SERVICE_IND) failed with msgr error = %d", msgr_error);
  }
#endif /* FEATURE_SMS_PWS_OVER_LTE */

  msgr_error = msgr_register(  MSGR_WMS_LTE,
                               &wms_msgr_client,
                               wms_msgr_rex_q_id,
                               MM_DOM_SEL_DOMAIN_SELECTED_CHANGED_IND );
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(MM_DOM_SEL_DOMAIN_SELECTED_CHANGED_IND) failed with msgr error = %d", msgr_error);
  }

  /* Register with MSG ROUTER for OFFLINE notification */
  msgr_error = msgr_register(MSGR_WMS_LTE, &wms_msgr_client, wms_msgr_rex_q_id, MM_MMOC_OFFLINE_IND);
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(MM_MMOC_OFFLINE_IND) failed with msgr error = %d", msgr_error);
  }

#ifdef FEATURE_DOMAIN_SELECTION
  /* Register with MSG ROUTER for LTE DOMAIN PREF change notification */
  msgr_error = msgr_register(MSGR_WMS_CM, &wms_msgr_client, wms_msgr_rex_q_id, MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_CHANGED_IND);
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_CHANGED_IND) failed with msgr error = %d", msgr_error);
  }
#endif /* FEATURE_DOMAIN_SELECTION */

  /* Register with MSG ROUTER for Policy Man device mode change notification */
  msgr_error = msgr_register( MSGR_WMS_POLICYMAN, &wms_msgr_client, wms_msgr_rex_q_id, POLICYMAN_CFG_UPDATE_MSIM_IND );
  if (msgr_error != E_SUCCESS)
  {
      MSG_ERROR_1("msgr_register(POLICYMAN_CFG_UPDATE_MSIM_IND) failed with msgr error = %d", msgr_error);
  }

  return;
} /* wms_msgr_init() */

/*===========================================================================
FUNCTION wms_msg_msgr_proc()                                       (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_msg_msgr_proc(
  void
)
{
  wms_msgr_cmd_s_type *rpt_ptr = NULL;

  MSG_HIGH_0( "In wms_msg_msgr_proc() receives WMS_MSGR_Q_SIG");

  (void) rex_clr_sigs(wms_myself, WMS_MSGR_Q_SIG);

  /* Get the command from the wms_msgr_q and process it */
  while ((rpt_ptr = q_get(&wms_msgr_q)) != NULL)
  {
    switch (rpt_ptr->cmd.msg_hdr.id)
    {
#ifdef FEATURE_SMS_PWS_OVER_LTE
      case LTE_RRC_ETWS_PRIM_IND:
        MSG_HIGH_0("Received ETWS primary notification");
        wms_msg_lte_etws_prim_proc(&(rpt_ptr->cmd.lte_etws_prim));
        break;
		
      case LTE_RRC_ETWS_SEC_IND:
        MSG_HIGH_0("Received ETWS secondary notification");
        wms_msg_lte_etws_sec_proc(&(rpt_ptr->cmd.lte_etws_sec));
        break;

      case LTE_RRC_CMAS_IND:
        MSG_HIGH_0("Received CMAS notification");
        wms_msg_lte_cmas_proc(&(rpt_ptr->cmd.lte_cmas));
        break;

      case LTE_RRC_SERVICE_IND:
        MSG_HIGH_0("Received LTE service ready notification");
        wms_msg_lte_srv_ind_proc(&(rpt_ptr->cmd.lte_service_ind));
        break;
#endif 
      case MM_MMOC_OFFLINE_IND:
        MSG_HIGH_0("Received OFFLINE notification");
 
       /* Process task offline procedure */
        wms_offline();
        break;

      case POLICYMAN_CFG_UPDATE_MSIM_IND:
        MSG_HIGH_0("Received Policy Item Change notification");
        wms_nv_policy_chg_ind_proc((wms_policyman_cfg_s_type *)&rpt_ptr->cmd.policyman_cfg);
        break;

      case MM_DOM_SEL_DOMAIN_SELECTED_CHANGED_IND:
        if (CM_CALL_TYPE_SMS_MASK & rpt_ptr->cmd.dom_sel_cmd.cm_domain_sel_domain_chg_ind.data.call_type_mask)
        {
          wms_cfg_s_type   *cfg_s_ptr = wms_cfg_s_ptr();
			
          if (SYS_CM_DOMAIN_SEL_DOMAIN_1X == rpt_ptr->cmd.dom_sel_cmd.cm_domain_sel_domain_chg_ind.data.domain &&
              SYS_SYS_MODE_LTE            == rpt_ptr->cmd.dom_sel_cmd.cm_domain_sel_domain_chg_ind.data.system)
          {
            cfg_s_ptr->sms_over_s102 = TRUE;
          }
          else
          {
            cfg_s_ptr->sms_over_s102 = FALSE;
          }
				  
          MSG_HIGH_1("Received MM Domain Selected Changed notification (S102 Available = %d)", cfg_s_ptr->sms_over_s102);
          wms_cfg_print_active_bearers();
        }
        break;
			
#ifdef FEATURE_DOMAIN_SELECTION
      case MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_CHANGED_IND:
        if (CM_DOM_SEL_CONFIG_TYPE_LTE_SMS_DOM_PREF == rpt_ptr->cmd.dom_sel_cmd.cm_domain_sel_config_chg_ind.data.config) 
        {
          wms_cfg_s_type   *cfg_s_ptr = wms_cfg_s_ptr();
          cfg_s_ptr->lte_domain_pref  = rpt_ptr->cmd.dom_sel_cmd.cm_domain_sel_config_chg_ind.data.value.lte_sms_dom_pref;
          MSG_HIGH_1("Received LTE Domain Selected Changed notification (lte_sms_domain_pref = %d)", cfg_s_ptr->lte_domain_pref);
        }
        break;
#endif

      default:
        break;
    }

    //Return the buffer back to free Q
    q_put (&wms_msgr_free_q, &(rpt_ptr->link));
  }

} /* wms_msg_msgr_proc() */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION wms_rrc_cell_change_ind_cb()                              (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void wms_rrc_cell_change_ind_cb(
  cell_info_type  cell_info 
)
{
  wms_cmd_type *cmd_ptr = NULL;

  cmd_ptr = wms_get_cmd_buf();

  MSG_HIGH_0("In wms_rrc_cell_change_ind_cb()");

  if ( NULL != cmd_ptr )
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB;
    cmd_ptr->hdr.client_id   = (uint8) WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;
    cmd_ptr->hdr.appInfo_ptr = NULL;

    /* Copy cell_info field-by-field */
    cmd_ptr->cmd.rrc_cb.cell_info.as_id   = cell_info.as_id;
    cmd_ptr->cmd.rrc_cb.cell_info.cell_id = cell_info.cell_id;

    cmd_ptr->cmd.rrc_cb.cell_info.lac[0] =  cell_info.lac[0];
    cmd_ptr->cmd.rrc_cb.cell_info.lac[1] =  cell_info.lac[1];

    memscpy( (void*)&cmd_ptr->cmd.rrc_cb.cell_info.plmn_id,
             sizeof(cmd_ptr->cmd.rrc_cb.cell_info.plmn_id),
             (void*)&cell_info.plmn_id,
             sizeof(cell_info.plmn_id) );

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("Out of cmd buffer");
  }

  return;
} /* wms_rrc_cell_change_ind_cb() */
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION wms_tdscdma_rrc_cell_change_ind_cb()                      (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void wms_tdscdma_rrc_cell_change_ind_cb(
  tdscell_info_type cell_info 
)
{
  cell_info_type wcdma_cell;

  MSG_HIGH_0("wms_tdscdma_rrc_cell_change_ind_cb()");

  wcdma_cell.as_id   = cell_info.as_id;
  wcdma_cell.cell_id = cell_info.cell_id;

  wcdma_cell.lac[0] = cell_info.lac[0];
  wcdma_cell.lac[1] = cell_info.lac[1];

  (void)memscpy( (void*)&wcdma_cell.plmn_id,
                 sizeof(wcdma_cell.plmn_id),
                 (void*)&cell_info.plmn_id,
                 sizeof(cell_info.plmn_id) );

  wms_rrc_cell_change_ind_cb( wcdma_cell );

  return;
}
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION wms_rrc_init_wcdma()                                      (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void wms_rrc_init_wcdma(
  void 
)
{
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = NULL;

  ptr_W = get_wcdma_interface();

  if (NULL != ptr_W)
  {
    (void)IWCDMA_rrc_register_serving_cell_change_ind_cb( ptr_W,
                                                          wms_rrc_cell_change_ind_cb );
  }

#else  /* FEATURE_SEGMENT_LOADING */

  (void)rrc_register_serving_cell_change_ind_cb( wms_rrc_cell_change_ind_cb );

#endif /* FEATURE_SEGMENT_LOADING */

  return;
} /* wms_rrc_init_wcdma() */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION wms_rrc_init_tdscdma()                                    (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void wms_rrc_init_tdscdma(
  void 
)
{
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_T = NULL;

  ptr_T = get_tdscdma_interface();

  if (NULL != ptr_T)
  {
    (void)ITDSCDMA_tdsrrc_register_serving_cell_change_ind_cb( ptr_T,
                                                               wms_tdscdma_rrc_cell_change_ind_cb );
  }

#else /* FEATURE_SEGMENT_LOADING */

  (void)tdsrrc_register_serving_cell_change_ind_cb( wms_tdscdma_rrc_cell_change_ind_cb );  

#endif /* FEATURE_SEGMENT_LOADING */

  return;
} /* wms_rrc_init_tdscdma() */
#endif /* FEATURE_TDSCDMA */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION wms_rrc_init()                                            (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void wms_rrc_init(
  void 
)
{
#ifdef FEATURE_WCDMA
  wms_rrc_init_wcdma();
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
  wms_rrc_init_tdscdma();
#endif /* FEATURE_TDSCDMA */

  return;
} /* wms_rrc_init() */
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

/*===========================================================================
FUNCTION wms_init_before_task_start()                              (INTERNAL)

DESCRIPTION
  Initialize the WMS module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initialization of internal data is done.

===========================================================================*/
void wms_init_before_task_start(
  void
)
{
  MSG_HIGH_0("RCINIT_DBG: wms_init_before_task_start() entered");

  /* ------------- Initialize API Command Buffers ---------------*/
  (void) q_init( & wms_cmd_q );

  rex_init_crit_sect(&wms_cmd_queue_crit_sect);

  /* Perform Local WMS Task Initialization */

  // Initialize the CFG (config) module:
  //   - init the AppInfo structures
  wms_cfg_pre_init();

  // Initialize the CL (client module):
  //   - init the CL critical section
  //   - init the client array
  //   - register the WMS internal client
  wms_cl_init();

  // Initialize the BC (broadcast) module:
  //   - init the BC critical section
  //   - create the LTE duplicate detection lists
  wms_bc_pre_init();

  MSG_HIGH_0("RCINIT_DBG: wms_init_before_task_start() returning");
}

/*===========================================================================
FUNCTION wms_init()                                                (INTERNAL)

DESCRIPTION
  Initialize the WMS module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initialization to internal data is done.

===========================================================================*/
void wms_init(
  void
)
{
  MSG_HIGH_0("RCINIT_DBG: wms_init() entered");

#ifdef FEATURE_MODEM_RCINIT
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  {
    /* Register for TASK_STOP_SIG */
    RCINIT_GROUP        wms_group;
    RCEVT_SIGEX_SIGREX  wms_stop_sig;

    wms_stop_sig.signal = wms_myself;    // REX Task Context
    wms_stop_sig.mask = TASK_STOP_SIG;   // REX signal mask

    wms_group = rcinit_lookup_group_rextask(wms_myself);
    rcinit_register_term_group(wms_group, RCEVT_SIGEX_TYPE_SIGREX, &wms_stop_sig);
  }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  /* Inform RCInit that WMS initialization phase is complete */
  rcinit_handshake_startup();
#else /* FEATURE_MODEM_RCINIT */
  /* If not using RCInit framework, initialize WMS queue and clients. */
  /* Otherwise, RCInit has already called this function in the init step. */
  wms_init_before_task_start();

#endif /* FEATURE_MODEM_RCINIT */

  /* Register with DOG HB */
  wms_reg_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) WMS_RPT_TIMER_SIG);

  (void) rex_clr_sigs( wms_myself, TASK_START_SIG );

  // At this point in the initialization, pre-initialization is complete
  // and the code path is now common for both TMC and RCINIT based tasks.
  // Refer to the function 'wms_init_before_task_start() for the early
  // initialization details.

  // Obtain the device_mode and UE mode.
  // Device mode is used to determine how the device should behave
  // in regards to Single Sim, DSDS, and DSDA.
  {
    wms_cfg_s_type         *cfg_s_ptr = wms_cfg_s_ptr();

    cfg_s_ptr->device_mode = wms_nv_get_device_mode();
    cfg_s_ptr->ue_mode     = wms_nv_get_ue_mode();
    wms_nv_init_feature_mode();
  }

  wms_nv_init();

  wms_sim_mmgsdi_init();

  wms_cm_init();
  wms_msg_init();
  wms_dc_init();
  wms_dbg_init();

  wms_msgr_init();

  wms_cfg_init();
  wms_ts_init();

  wmsdiag_init();

#if (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA))
  wms_rrc_init();
#endif /* (defined(FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)) */

  /* SIM & RUIM init will be done after full service is started */

  /* Initialize QMI_WMS
  */
  qmi_wms_init();

  MSG_HIGH_0("RCINIT_DBG: wms_init() returning");

  return;
} /* wms_init() */

/*===========================================================================
FUNCTION wms_get_cmd_buf()                                         (INTERNAL)

DESCRIPTION
  Allocate a WMS Task command buffer.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  Pointer to allocated WMS command buffer.
  NULL pointer is returned if no more buffer is available.

SIDE EFFECTS
  Remove a command from the free queue.

===========================================================================*/
wms_cmd_type *wms_get_cmd_buf(
  void
)
{
  wms_cmd_type *cmd_ptr = NULL;

  rex_enter_crit_sect(&wms_cmd_queue_crit_sect);
  if(wms_cmd_avail_cnt > 0)
  {
    cmd_ptr = (wms_cmd_type *) wms_mem_malloc (sizeof (wms_cmd_type));
  }

  if (cmd_ptr != NULL)
  {
    wms_cmd_avail_cnt--;
    MSG_LOW_1("Command Count = %d", wms_cmd_avail_cnt);
  }
  else
  {
     MSG_ERROR_1("wms_mem_malloc failied or max out cmds %d", wms_cmd_avail_cnt);
  }

  rex_leave_crit_sect(&wms_cmd_queue_crit_sect);

  return cmd_ptr;

} /* wms_get_cmd_buf() */

/*===========================================================================
FUNCTION wms_check_for_valid_command()                             (INTERNAL)

DESCRIPTION
  This function call will check for which commands
   to allow / disallow during special scenarios like
   1. WMS Refresh
   2. CDMA Init Not Completed
   3. GSM Init Not Completed
   It will return the appropriate Command Error to be
   returned to the client in the Callback

DEPENDENCIES
  General Rule: Dont Change Command Error if it is a
  Valid Command. (cmd_err initialized to NONE)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
wms_cmd_err_e_type wms_check_for_valid_command(
  wms_cmd_type *cmd_ptr
)
{
  wms_cmd_err_e_type  ret_cmd_err = WMS_CMD_ERR_NONE;
  char               *cmd_str;

  cmd_str = wms_dbg_get_cmd_string(cmd_ptr->hdr.cmd_id);

  // Commands that do not have appInfo:
  if (NULL == cmd_ptr->hdr.appInfo_ptr)
  {
    switch (cmd_ptr->hdr.cmd_id)
    {
      // This is a list of commands allowed to not have appInfo_ptr initialized
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_MSG_TRANSPORT_REG:
      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
      case WMS_CMD_MSG_GSTK_EVT:
      case WMS_CMD_CM_ONLINE_E:
      case WMS_CMD_CM_LPM_E:
      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB:
      case WMS_CMD_IPAPP_CHANGE_INFO:
      case WMS_CMD_MMGSDI_EVENT_CB:
      case WMS_CMD_MMGSDI_RESPONSE_CB:
      case WMS_CMD_CM_PH_EVENT_CB:
      case WMS_CMD_CM_SUBS_EVENT_CB:
      case WMS_CMD_CARD_EF_INFO_UPDATE:
      case WMS_CMD_CLIENT_ACTIVATE:
      case WMS_CMD_CFG_SET_LTE_DOMAIN_PREF:
#ifdef FEATURE_FUZZ_TEST
      case WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND:
#endif /* FEATURE_FUZZ_TEST */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case WMS_CMD_NV_REFRESH_CB:
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
        break;

      default:
      {
        // Any command not in the above list is rejected
        ret_cmd_err = WMS_CMD_ERR_UNSUPPORTED;

        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Command found without appInfo_ptr: %d(%s)",
                       cmd_ptr->hdr.cmd_id,
                       cmd_str );
      }
    }

    return ret_cmd_err;
  }

  switch(cmd_ptr->hdr.cmd_id)
  {

  case WMS_CMD_CFG_SET_MESSAGE_WAITING:
  case WMS_CMD_MSG_SEND:
  case WMS_CMD_MSG_READ:
  case WMS_CMD_MSG_WRITE:
  case WMS_CMD_MSG_DELETE:
  case WMS_CMD_MSG_DELETE_ALL:
  case WMS_CMD_MSG_MODIFY_TAG:
  case WMS_CMD_MSG_WRITE_TEMPLATE:
  case WMS_CMD_MSG_DELETE_TEMPLATE:
  case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
  case WMS_CMD_MSG_READ_STS_REPORT:
  case WMS_CMD_MSG_WRITE_STS_REPORT:
  case WMS_CMD_MSG_DELETE_STS_REPORT:
  case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
  case WMS_CMD_BC_MM_ADD_SRV:
  case WMS_CMD_BC_MM_DELETE_SRV:
  case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
  case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
  case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
  case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
  case WMS_CMD_DBG_SET_MSG_REF_NUM:
  case WMS_CMD_DBG_CLEAR_SMMA:
  case WMS_CMD_MSG_ACK:
  case WMS_CMD_CFG_SET_MEMORY_FULL:
    if ((NULL != cmd_ptr->hdr.appInfo_ptr) && cmd_ptr->hdr.appInfo_ptr->mmgsdi_is_busy)
    {
      MSG_HIGH_0("SIM is Busy, Blocking SIM related Command");
      ret_cmd_err = WMS_CMD_ERR_BUSY;
      return ret_cmd_err;
    }
    break;

  default:
    break;
  }

#ifdef FEATURE_GSTK
  /* Checking for 1st Special Scenario - REFRESH */
  if (cmd_ptr->hdr.appInfo_ptr->refresh_in_progress)
  {
    switch(cmd_ptr->hdr.cmd_id)
    {
      /* Valid Refresh Commands */
      case WMS_CMD_CFG_SET_ROUTES:
    case WMS_CMD_CFG_GET_ROUTES:

#ifdef FEATURE_GWSMS
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
#endif /* FEATURE_GWSMS */

      case WMS_CMD_CFG_SET_LTE_DOMAIN_PREF:
      case WMS_CMD_MSG_TRANSPORT_REG:
      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
      case WMS_CMD_MSG_GSTK_EVT:
      case WMS_CMD_CM_ONLINE_E:
      case WMS_CMD_CM_LPM_E:
      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB:
      case WMS_CMD_IPAPP_CHANGE_INFO:
      case WMS_CMD_MMGSDI_EVENT_CB:
#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case WMS_CMD_NV_REFRESH_CB:
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
      case WMS_CMD_MMGSDI_RESPONSE_CB:
      case WMS_CMD_CM_PH_EVENT_CB:
      case WMS_CMD_CM_SUBS_EVENT_CB:
      case WMS_CMD_CARD_EF_INFO_UPDATE:
      case WMS_CMD_MMGSDI_SEEK_CB:
      case WMS_CMD_CLIENT_ACTIVATE:
      case WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT:
      case WMS_CMD_MSG_MC_MO_STATUS_E:
      case WMS_CMD_MSG_MC_MT_MSG_E:
      case WMS_CMD_MSG_ACK:
        break;

      default:
        {
          /* Invalid Refresh Commands Return BUSY */
          ret_cmd_err = WMS_CMD_ERR_BUSY;

          MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Command not allowed during refresh: %d(%s)",
                         cmd_ptr->hdr.cmd_id,
                         cmd_str );

        }
        break;
    }
  }
#endif /*FEATURE_GSTK */

#ifdef FEATURE_CDSMS
  /* Checking for 2nd Special Scenario - CDMA Init */
  if ( (MMGSDI_1X_PROV_PRI_SESSION == cmd_ptr->hdr.appInfo_ptr->mmgsdi_appInfo.session_type) &&
       (!cmd_ptr->hdr.appInfo_ptr->sim_init_complete) )
  {
    switch(cmd_ptr->hdr.cmd_id)
    {
      /* Valid Commands */
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_CFG_SET_MEMORY_FULL:
      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_CFG_GET_LINK_CONTROL:
      case WMS_CMD_CFG_SET_MESSAGE_WAITING:
      case WMS_CMD_CFG_GET_MESSAGE_WAITING:
      case WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT:
        break;

      case WMS_CMD_CFG_GET_MEMORY_STATUS:
        switch (cmd_ptr->cmd.cfg_get_mem_status.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_CFG_GET_MESSAGE_LIST:
        switch (cmd_ptr->cmd.cfg_get_msg_list.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_SEND:
        if (cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_READ:
        switch(cmd_ptr->cmd.msg_read.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE:
        if ( (cmd_ptr->cmd.msg_write.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
             !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
        {
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_DELETE:
        switch (cmd_ptr->cmd.msg_delete.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_DELETE_ALL:
        switch (cmd_ptr->cmd.msg_delete_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_MODIFY_TAG:
        switch (cmd_ptr->cmd.msg_modify_tag.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_READ_TEMPLATE:
        switch (cmd_ptr->cmd.msg_read_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE_TEMPLATE:
        if ( (cmd_ptr->cmd.msg_write_template.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
             !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
        {
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE:
        switch (cmd_ptr->cmd.msg_delete_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
        switch (cmd_ptr->cmd.msg_delete_template_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_CDMA:
          case WMS_MEMORY_STORE_RUIM:
          case WMS_MEMORY_STORE_NV_CDMA:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_READ_STS_REPORT:
      case WMS_CMD_MSG_WRITE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
      case WMS_CMD_MSG_TRANSPORT_REG:
        break;

      case WMS_CMD_DC_CONNECT:
      case WMS_CMD_DC_DISCONNECT:
        {
          wms_dc_s_type *dc_s_ptr = wms_dc_s_ptr();
  
          if (NULL != dc_s_ptr)
          {
            MSG_HIGH_1("updating dc_s_ptr->wms_initiated_call from %d to 0", dc_s_ptr->wms_initiated_call);
            dc_s_ptr->wms_initiated_call = FALSE;
          }
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
        break;

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
      case WMS_CMD_BC_MM_GET_CONFIG:
        if (cmd_ptr->cmd.bc_mm_get_config.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_PREF:
        if (cmd_ptr->cmd.bc_mm_get_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PREF:
        if (cmd_ptr->cmd.bc_mm_set_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_TABLE:
        if (cmd_ptr->cmd.bc_mm_get_table.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_SRV:
        if (cmd_ptr->cmd.bc_mm_select_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
        if (cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_get_srv_info.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_ADD_SRV:
        if (cmd_ptr->cmd.bc_mm_add_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_SRV:
        if (cmd_ptr->cmd.bc_mm_delete_srv.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_change_srv_info.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
        break;

      case WMS_CMD_BC_MM_GET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode == WMS_MESSAGE_MODE_CDMA)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
#ifdef FEATURE_FUZZ_TEST
      case WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND:
#endif /* FEATURE_FUZZ_TEST */
        break;

      case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_DBG_SET_MSG_REF_NUM:
      case WMS_CMD_DBG_CLEAR_SMMA:
        break;

      // Allow MT SMS, TL ACK and MO Status
      // even if network goes down
      case WMS_CMD_MSG_MC_MO_STATUS_E:
      case WMS_CMD_MSG_MC_MT_MSG_E:
      case WMS_CMD_MSG_ACK:
      case WMS_CMD_DC_CALL_CONNECT_E:
      case WMS_CMD_DC_CALL_END_E:
      case WMS_CMD_DC_CALL_ERROR_E:
        break;

      case WMS_CMD_MSG_MC_STATE_CHANGED_E:
      case WMS_CMD_MSG_MC_MWI_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_GSTK_EVT:
        break;

      case WMS_CMD_DC_CALL_INCOM_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_CM_ONLINE_E:
        break;

      //case WMS_CMD_GSDI_ASYNC_CB:
      //case WMS_CMD_GSDI_CARD_STATUS_CB:
      case WMS_CMD_MMGSDI_EVENT_CB:
      case WMS_CMD_MMGSDI_RESPONSE_CB:
      case WMS_CMD_MMGSDI_SEEK_CB:
      case WMS_CMD_CARD_EF_INFO_UPDATE:
#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case WMS_CMD_NV_REFRESH_CB:
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
        break;

      //case WMS_CMD_GSDI_REFRESH_FCN_IND:
      case WMS_CMD_BC_SCPT_DATA:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CM_LPM_E:
      case WMS_CMD_IPAPP_CHANGE_INFO:
      case WMS_CMD_CLIENT_ACTIVATE:
        break;

      default:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
    }

    if (WMS_CMD_ERR_NONE != ret_cmd_err)
    {
      MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "1X init is not complete: %d(%s)",
                     cmd_ptr->hdr.cmd_id,
                     cmd_str );
    }

  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
  /* Checking for 3rd Special Scenario - GW Init */
  if ( ( (MMGSDI_GW_PROV_PRI_SESSION == cmd_ptr->hdr.appInfo_ptr->mmgsdi_appInfo.session_type) ||
         (MMGSDI_GW_PROV_SEC_SESSION == cmd_ptr->hdr.appInfo_ptr->mmgsdi_appInfo.session_type)  ||
         (MMGSDI_GW_PROV_TER_SESSION == cmd_ptr->hdr.appInfo_ptr->mmgsdi_appInfo.session_type) ) &&
       (!cmd_ptr->hdr.appInfo_ptr->sim_init_complete) )
  {
    switch(cmd_ptr->hdr.cmd_id)
    {
      /* Valid Commands */
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT:
        break;

      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_CFG_GET_LINK_CONTROL:
      case WMS_CMD_CFG_SET_MESSAGE_WAITING:
      case WMS_CMD_CFG_GET_MESSAGE_WAITING:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_CFG_SET_MEMORY_FULL:
        break;

      case WMS_CMD_CFG_GET_MEMORY_STATUS:
        switch (cmd_ptr->cmd.cfg_get_mem_status.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_CFG_GET_MESSAGE_LIST:
        switch (cmd_ptr->cmd.cfg_get_msg_list.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            ret_cmd_err = WMS_CMD_ERR_BUSY;
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_SEND:
        /* Do not Allow this Command */
        if (cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
        {
           ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_ACK:
        break;

      case WMS_CMD_MSG_READ:
        /* Do not Allow this Command */
        switch(cmd_ptr->cmd.msg_read.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE:
        /* Do not Allow this Command */
        if ( (cmd_ptr->cmd.msg_write.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW) &&
             !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
        {
           ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_DELETE:
        switch (cmd_ptr->cmd.msg_delete.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_DELETE_ALL:
        switch (cmd_ptr->cmd.msg_delete_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_MODIFY_TAG:
        switch (cmd_ptr->cmd.msg_modify_tag.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_READ_TEMPLATE:
        switch (cmd_ptr->cmd.msg_read_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_WRITE_TEMPLATE:
        if ( (cmd_ptr->cmd.msg_write_template.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW) &&
             !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
        {
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE:
        switch (cmd_ptr->cmd.msg_delete_template.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
        switch (cmd_ptr->cmd.msg_delete_template_all.mem_store)
        {
          case WMS_MEMORY_STORE_RAM_GW:
          case WMS_MEMORY_STORE_SIM:
          case WMS_MEMORY_STORE_NV_GW:
          case WMS_MEMORY_STORE_USIM:
            if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
            {
              ret_cmd_err = WMS_CMD_ERR_BUSY;
            }
            break;

          default:
            break;
        }
        break;

      case WMS_CMD_MSG_READ_STS_REPORT:
      case WMS_CMD_MSG_WRITE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
        if ( !wms_sim_mmgsdi_init_done(cmd_ptr->hdr.appInfo_ptr) )
        {
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        }
        break;

      case WMS_CMD_MSG_TRANSPORT_REG:
        break;

      case WMS_CMD_DC_CONNECT:
      case WMS_CMD_DC_DISCONNECT:
      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
        break;

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
      case WMS_CMD_BC_MM_GET_CONFIG:
        if (cmd_ptr->cmd.bc_mm_get_config.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_PREF:
        if (cmd_ptr->cmd.bc_mm_get_pref.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PREF:
        if (cmd_ptr->cmd.bc_mm_set_pref.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_TABLE:
        if (cmd_ptr->cmd.bc_mm_get_table.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_SRV:
        if (cmd_ptr->cmd.bc_mm_select_srv.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
        if (cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_get_srv_info.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_ADD_SRV:
        if (cmd_ptr->cmd.bc_mm_add_srv.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_SRV:
        if (cmd_ptr->cmd.bc_mm_delete_srv.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
        if (cmd_ptr->cmd.bc_mm_change_srv_info.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
        if (cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_GET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_BC_MM_SET_READING_PREF:
        if (cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode == WMS_MESSAGE_MODE_GW)
          ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

      case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
        break;

      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
#ifdef FEATURE_FUZZ_TEST
      case WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND:
#endif /* FEATURE_FUZZ_TEST */
        break;

      case WMS_CMD_DBG_SET_MSG_REF_NUM:
      case WMS_CMD_DBG_CLEAR_SMMA:
        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;

      case WMS_CMD_MSG_MC_MT_MSG_E:
      case WMS_CMD_MSG_MC_MO_STATUS_E:
      case WMS_CMD_MSG_MC_STATE_CHANGED_E:
      case WMS_CMD_MSG_MC_MWI_E:
        break;

      case WMS_CMD_MSG_GSTK_EVT:
        break;

      case WMS_CMD_DC_CALL_INCOM_E:
      case WMS_CMD_DC_CALL_CONNECT_E:
      case WMS_CMD_DC_CALL_END_E:
      case WMS_CMD_DC_CALL_ERROR_E:
        break;

      case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
        break;

      case WMS_CMD_CM_ONLINE_E:
      //case WMS_CMD_GSDI_ASYNC_CB:
      //case WMS_CMD_GSDI_CARD_STATUS_CB:
      case WMS_CMD_MMGSDI_EVENT_CB:
      case WMS_CMD_MMGSDI_RESPONSE_CB:
      case WMS_CMD_MMGSDI_SEEK_CB:
      case WMS_CMD_CARD_EF_INFO_UPDATE:
#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case WMS_CMD_NV_REFRESH_CB:
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
        break;

      //case WMS_CMD_GSDI_REFRESH_FCN_IND:
      //  ret_cmd_err = WMS_CMD_ERR_BUSY;
      //  break;

      case WMS_CMD_BC_SCPT_DATA:
        break;

      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB:
      case WMS_CMD_CM_LPM_E:
      case WMS_CMD_IPAPP_CHANGE_INFO:
      case WMS_CMD_CLIENT_ACTIVATE:
        break;

      default:
        MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Unhandled command, reporting busy: %d(%s)",
                       cmd_ptr->hdr.cmd_id,
                       cmd_str );

        ret_cmd_err = WMS_CMD_ERR_BUSY;
        break;
    }

    if (WMS_CMD_ERR_NONE != ret_cmd_err)
    {
      MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "GW init is not complete: %d(%s), asid %d",
                     cmd_ptr->hdr.cmd_id,
                     cmd_str,
                     cmd_ptr->hdr.appInfo_ptr->asid );
    }
  }
#endif /* FEATURE_GWSMS */

  return ret_cmd_err;
} /* wms_check_for_valid_command() */

/*===========================================================================
FUNCTION wms_put_cmd()                                             (INTERNAL)

DESCRIPTION
  The API functions calls uses this function to put commands to the WMS.

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the command queue.

===========================================================================*/
void wms_put_cmd(
  wms_cmd_type  *cmd_ptr
)
{
  char                *cmd_str;

  cmd_str = wms_dbg_get_cmd_string(cmd_ptr->hdr.cmd_id);

  MSG_SPRINTF_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Putting %d %s (client %d, as_id %d, appInfo_ptr 0x%08x)",
                 cmd_ptr->hdr.cmd_id,
                 cmd_str,
                 cmd_ptr->hdr.client_id,
                 cmd_ptr->hdr.as_id,
                 cmd_ptr->hdr.appInfo_ptr );

  (void) q_link( cmd_ptr, & cmd_ptr->hdr.link );  /* init link */
  q_put( & wms_cmd_q, & cmd_ptr->hdr.link );          /* and queue it */

  if (NULL != wms_myself)
  {
    // If WMS task isn't started, no signal is set
    (void) rex_set_sigs( wms_myself, WMS_CMD_Q_SIG ); /* signal the task */
  }

  MSG_LOW_1("wms_cmd_q cnt=%d", q_cnt( &wms_cmd_q ));

} /* wms_put_cmd() */

/*===========================================================================
FUNCTION wms_get_cmd_count()                                       (INTERNAL)

DESCRIPTION
  The API function call returns the number of commands to WMS

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  Count of the number of commands

SIDE EFFECTS
  None

===========================================================================*/
int wms_get_cmd_count(
  void
)
{
  MSG_LOW_1("wms_cmd_q cnt=%d", q_cnt( &wms_cmd_q ));
  return (q_cnt( &wms_cmd_q ));
} /* wms_get_cmd_count() */

/*===========================================================================
FUNCTION wms_process_cmd()                                         (INTERNAL)

DESCRIPTION
  Process the commands from the API clients.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  None

SIDE EFFECTS
  Command buffer is put back to the free queue.

===========================================================================*/
void wms_process_cmd(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err;
  wms_client_s_type   *client_ptr = wms_client_ptr_fetch(cmd_ptr->hdr.client_id);
  char                *cmd_str;

  cmd_str = wms_dbg_get_cmd_string(cmd_ptr->hdr.cmd_id);

  MSG_SPRINTF_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Processing %d %s (client %d, as_id %d, appInfo_ptr 0x%08x)",
                 cmd_ptr->hdr.cmd_id,
                 cmd_str,
                 cmd_ptr->hdr.client_id,
                 cmd_ptr->hdr.as_id,
                 cmd_ptr->hdr.appInfo_ptr );
  do
  {
    // Ensure valid and active client, otherwise issue error via command callback
    if ((NULL == client_ptr) || (FALSE == client_ptr->activated))
    {
      MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Unregistered or non-active client (client %d, client_ptr 0x%08x)",
                     cmd_ptr->hdr.client_id,
                     client_ptr );

      break; // out of do-while
    }

    // Ensure valid command, otherwise issue error via command callback
    if ((cmd_err = wms_check_for_valid_command(cmd_ptr)) != WMS_CMD_ERR_NONE)
    {
      if(cmd_ptr->hdr.cmd_id != WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT)
      {
        /* Dont Process the Command because of an Error
           Return Error Code returned by function */
        wms_client_cmd_status(cmd_ptr, cmd_err);
  
        break; // out of do-while
      }
    }

    // Process the command
    switch (cmd_ptr->hdr.cmd_id)
    {
      /* Configuration group commands
      */
      case WMS_CMD_CFG_SET_ROUTES:
      case WMS_CMD_CFG_GET_ROUTES:
      case WMS_CMD_CFG_GET_MEMORY_STATUS:
      case WMS_CMD_CFG_GET_MESSAGE_LIST:
#ifdef FEATURE_GWSMS
      case WMS_CMD_CFG_SET_GW_DOMAIN_PREF:
      case WMS_CMD_CFG_GET_GW_DOMAIN_PREF:
#endif /* FEATURE_GWSMS */
      case WMS_CMD_CFG_SET_LTE_DOMAIN_PREF:
      case WMS_CMD_CFG_SET_PRIMARY_CLIENT:
      case WMS_CMD_CFG_SET_MEMORY_FULL:
      case WMS_CMD_CFG_SET_LINK_CONTROL:
      case WMS_CMD_CFG_GET_LINK_CONTROL:
      case WMS_CMD_CFG_SET_MESSAGE_WAITING:
      case WMS_CMD_CFG_GET_MESSAGE_WAITING:
      case WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT:
      case WMS_CMD_CFG_RRC_CELL_CHANGE_EVENT_CB:
        wms_cfg_process_cmd( cmd_ptr );
        break;

      /* Message group commands
      */
      case WMS_CMD_MSG_SEND:
      case WMS_CMD_MSG_ACK:
      case WMS_CMD_MSG_READ:
      case WMS_CMD_MSG_WRITE:
      case WMS_CMD_MSG_DELETE:
      case WMS_CMD_MSG_DELETE_ALL:
      case WMS_CMD_MSG_MODIFY_TAG:
      case WMS_CMD_MSG_READ_TEMPLATE:
      case WMS_CMD_MSG_WRITE_TEMPLATE:
      case WMS_CMD_MSG_DELETE_TEMPLATE:
      case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
      case WMS_CMD_MSG_TRANSPORT_REG:

#ifdef FEATURE_GWSMS
      case WMS_CMD_MSG_READ_STS_REPORT:
      case WMS_CMD_MSG_WRITE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT:
      case WMS_CMD_MSG_DELETE_STS_REPORT_ALL:
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_MO_CONTROL
      case WMS_CMD_MSG_GSTK_EVT:
#endif /* FEATURE_GWSMS_MO_CONTROL */

#ifdef FEATURE_CDSMS
      /* Internal: events from MC
      */
      case WMS_CMD_MSG_MC_MT_MSG_E:
      case WMS_CMD_MSG_MC_MO_STATUS_E:
      case WMS_CMD_MSG_MC_STATE_CHANGED_E:
      case WMS_CMD_MSG_MC_MWI_E:
#endif /* FEATURE_CDSMS */
        wms_msg_process_cmd( cmd_ptr );
        break;

#ifdef FEATURE_CDSMS
      /* DC group commands
      */
      case WMS_CMD_DC_CONNECT:
      case WMS_CMD_DC_DISCONNECT:
      case WMS_CMD_DC_ENABLE_AUTO_DISCONNECT:
      case WMS_CMD_DC_DISABLE_AUTO_DISCONNECT:
        wms_dc_process_cmd( cmd_ptr );
        break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
      /* BC MM commands
      */
      case WMS_CMD_BC_MM_GET_CONFIG:
      case WMS_CMD_BC_MM_GET_PREF:
      case WMS_CMD_BC_MM_SET_PREF:
      case WMS_CMD_BC_MM_GET_TABLE:
      case WMS_CMD_BC_MM_SELECT_SRV:
      case WMS_CMD_BC_MM_GET_ALL_SRV_IDS:
      case WMS_CMD_BC_MM_GET_SRV_INFO:
      case WMS_CMD_BC_MM_ADD_SRV:
      case WMS_CMD_BC_MM_DELETE_SRV:
      case WMS_CMD_BC_MM_CHANGE_SRV_INFO:
      case WMS_CMD_BC_MM_DELETE_ALL_SERVICES:
      case WMS_CMD_BC_MM_SELECT_ALL_SERVICES:
      case WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES:
      case WMS_CMD_BC_MM_MSG_DELETE_INDICATION:
      case WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION:
      case WMS_CMD_BC_MM_GET_READING_PREF:
      case WMS_CMD_BC_MM_SET_READING_PREF:
#ifdef FEATURE_CDSMS_BROADCAST
      case WMS_CMD_BC_SCPT_DATA: /* Internal command */
#endif /* FEATURE_CDSMS_BROADCAST */
        wms_bc_mm_process_cmd( cmd_ptr );
        break;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


      /* DBG commands
      */
#ifdef FEATURE_CDSMS
      case WMS_CMD_DBG_RESET_TL_SEQ_NUM:
        wms_dbg_process_cmd( cmd_ptr );
        break;
#endif /* FEATURE_CDSMS */
#ifdef FEATURE_GWSMS
      case WMS_CMD_DBG_SET_MSG_REF_NUM:
      case WMS_CMD_DBG_CLEAR_SMMA:
        wms_dbg_process_cmd( cmd_ptr );
        break;
#endif/*FEATURE_GWSMS*/
      case WMS_CMD_DBG_GET_RETRY_INTERVAL:
      case WMS_CMD_DBG_SET_RETRY_INTERVAL:
      case WMS_CMD_DBG_GET_RETRY_PERIOD:
      case WMS_CMD_DBG_SET_RETRY_PERIOD:
#ifdef FEATURE_FUZZ_TEST
      case WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND:
#endif /* FEATURE_FUZZ_TEST */
        wms_dbg_process_cmd( cmd_ptr );
        break;

      /* Internal: events from CM
      */
#ifdef FEATURE_CDSMS
      case WMS_CMD_DC_CALL_INCOM_E:
      case WMS_CMD_DC_CALL_CONNECT_E:
      case WMS_CMD_DC_CALL_END_E:
      case WMS_CMD_DC_CALL_ERROR_E:  /* in case of CM cmd errors */
        wms_dc_process_cmd( cmd_ptr );
        break;
#endif /* FEATURE_CDSMS */

      case WMS_CMD_CM_BC_ENABLE_FAILURE_E:
      case WMS_CMD_CM_ONLINE_E:
      case WMS_CMD_SS_CHANGE_INFO:
      case WMS_CMD_CM_LPM_E:

      case WMS_CMD_IPAPP_CHANGE_INFO:
        wms_cm_process_event(cmd_ptr);
        break;

      case WMS_CMD_MMGSDI_EVENT_CB:
        wms_sim_mmgsdi_event_cb_proc(cmd_ptr);
        break;

      case WMS_CMD_MMGSDI_RESPONSE_CB:
        wms_sim_mmgsdi_response_cb_proc(cmd_ptr);
        break;

      case WMS_CMD_MMGSDI_SEEK_CB:
        wms_sim_mmgsdi_seek_cb_proc(cmd_ptr);
        break;

      case WMS_CMD_CLIENT_ACTIVATE:
        wms_cfg_activate_client(cmd_ptr->hdr.client_id);
        break;

      case WMS_CMD_CM_PH_EVENT_CB:
        wms_cm_ph_event_cb_proc(cmd_ptr);
        break;

      case WMS_CMD_CM_SUBS_EVENT_CB:
        wms_cm_subs_event_cb_proc(cmd_ptr);
        break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case WMS_CMD_NV_REFRESH_CB:
        wms_nv_refresh_cb_proc(cmd_ptr);
        break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

      default:
        break;
    } // switch( cmd_ptr->hdr.cmd_id )

#ifdef FEATURE_FUZZ_TEST
    if (WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND == cmd_ptr->hdr.cmd_id)
    {
       wms_evt_sms_command_processed_s_type evt;

       evt.cmd_id = cmd_ptr->hdr.cmd_id;
       event_report_payload(EVENT_SMS_COMMAND_PROCESSED, sizeof(evt), (void*)&evt);
    }
#endif /* FEATURE_FUZZ_TEST */
  } while (0);

  /* ----------- done with command processing -------------- */

  // Clean up command item
  rex_enter_crit_sect(&wms_cmd_queue_crit_sect);
  wms_mem_free(cmd_ptr);
  wms_cmd_avail_cnt++;
  rex_leave_crit_sect(&wms_cmd_queue_crit_sect);
  WMS_CLIENT_PTR_RELEASEIF(client_ptr);

  return;
} /* wms_process_cmd() */

/*===========================================================================
FUNCTION wms_process_signals()                                     (INTERNAL)

DESCRIPTION
  Process the signals for SMS related commands and timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_process_signals(
  rex_sigs_type              sigs
)
{
  wms_cmd_type             *cmd_ptr;
  rex_sigs_type             wms_sigs;

  if (0 != (sigs & TASK_STOP_SIG))
  {
     MSG_HIGH_0( "TASK_STOP_SIG received");
     (void) rex_clr_sigs(wms_myself, TASK_STOP_SIG);

     /* Release Client Memory */
     wms_client_release_all();

#ifdef FEATURE_SMS_PWS_OVER_LTE
     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             LTE_RRC_ETWS_PRIM_IND );

     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             LTE_RRC_ETWS_SEC_IND );

     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             LTE_RRC_CMAS_IND );

     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             LTE_RRC_SERVICE_IND );

     wms_bc_cleanup();
#endif /* FEATURE_SMS_PWS_OVER_LTE */

     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             MM_DOM_SEL_DOMAIN_SELECTED_CHANGED_IND );

     (void) msgr_deregister( MSGR_WMS_LTE,
                             &wms_msgr_client,
                             MM_MMOC_OFFLINE_IND );

     (void) msgr_deregister( MSGR_WMS_POLICYMAN,
                             &wms_msgr_client,
                             POLICYMAN_CFG_UPDATE_MSIM_IND );

     (void) msgr_client_delete( &wms_msgr_client );

     wms_msg_cleanup();
     wms_cfg_cleanup();

#ifdef FEATURE_MODEM_RCINIT_PHASE2
     {
       /* Deregister TASK_STOP_SIG */
       RCINIT_GROUP        wms_group;
       RCEVT_SIGEX_SIGREX  wms_stop_sig;

       wms_stop_sig.signal = wms_myself;    // REX Task Context
       wms_stop_sig.mask = TASK_STOP_SIG;   // REX signal mask

       wms_group = rcinit_lookup_group_rextask(wms_myself);
       rcinit_unregister_term_group(wms_group, RCEVT_SIGEX_TYPE_SIGREX, &wms_stop_sig);
       rcinit_handshake_term();
     }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

     return;
  }


#ifndef FEATURE_MODEM_RCINIT_PHASE2
  if (0 != (sigs & TASK_OFFLINE_SIG))
  {
     MSG_HIGH_0(" TASK_OFFLINE_SIG received");
     (void) rex_clr_sigs(wms_myself, TASK_OFFLINE_SIG);

     /* Process task offline procedure.*/
     wms_offline();
  }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  /* Dispatch other rex signals */
  if (0 == (sigs & WMS_SIGNALS))
  {
     /* No WMS signals, early return */
     return;
  }

  /* Kick the dog */
  wms_kick_dog();

#ifdef FEATURE_GWSMS
  if( sigs & WMS_RPT_Q_SIG )
  {
    MSG_HIGH_0( "got WMS_RPT_Q_SIG");

    (void) rex_clr_sigs( wms_myself, WMS_RPT_Q_SIG );

    while(TRUE == wms_cm_gw_dispatch_one()) {
       wms_sigs = rex_get_sigs(wms_myself);

       if (wms_sigs & WMS_RPT_TIMER_SIG) {
          /* Also clears WMS_RPT_TIMER_SIG */
          wms_kick_dog();
       }
    }
  } /* if WMS_RPT_Q_SIG */
#endif /* FEATURE_GWSMS */


  if( sigs & WMS_CMD_Q_SIG )
  {
    MSG_HIGH_0( "got WMS_CMD_Q_SIG");

    (void) rex_clr_sigs( wms_myself, WMS_CMD_Q_SIG );

    while( ( cmd_ptr = (wms_cmd_type *) q_get( & wms_cmd_q )) != NULL )
    {
      wms_sigs = rex_get_sigs(wms_myself);

      if (wms_sigs & WMS_RPT_TIMER_SIG)
      {
        /* Also clears WMS_RPT_TIMER_SIG */
        wms_kick_dog();
      }

      wms_process_cmd( cmd_ptr );     /* actually handle it */
    }

  } /* if WMS_CMD_Q_SIG */


#ifdef FEATURE_CDSMS

  if( sigs & WMS_DC_CALL_OUT_TIMER_SIG )
  {
    wms_dc_call_out_timer_proc();
  }

  if( sigs & WMS_DC_IDLE_TIMER_SIG )
  {
    wms_dc_idle_timer_proc();
  }

  if( sigs & WMS_MT_ACK_TIMER_SIG )
  {
    wms_msg_cdma_mt_ack_timer_proc();
  }

  if( sigs & WMS_AWISMS_ACK_TIMER_SIG )
  {
    wms_msg_awi_ack_timer_proc();
  }
#endif /* FEATURE_CDSMS */

  if(sigs & WMS_MO_RETRY_TIMER_SIG)
  {
    wms_msg_process_retry_sig();
  }

  if(sigs & WMS_MSGR_Q_SIG)
  {
    wms_msg_msgr_proc();
  }

  return;
} /* end of wms_process_signals() */

/*===========================================================================
FUNCTION wms_task()                                                (INTERNAL)

DESCRIPTION
  The main WMS function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void wms_task(
  uint32 dummy               /* ignore fact that this isn't used */
)
{
  rex_sigs_type  sigs;       /* hold signals */

  (void)dummy;

  wms_myself = rex_self(); // Will never change after this

  wms_init();              /* initialize queues, timers, etc. */

  // Start the queue processing
  (void) rex_set_sigs( wms_myself, WMS_CMD_Q_SIG );
  (void) rex_set_sigs( wms_myself, WMS_RPT_Q_SIG );
#ifdef FEATURE_MODEM_RCINIT
  (void)rcevt_signal_name("WMS: READY");
#endif

  for( ;; )
  {
    /* Wait on REX signals, repeat forever */

    sigs = rex_wait( WMS_SIGNALS
                   | TASK_STOP_SIG
#ifndef FEATURE_MODEM_RCINIT_PHASE2
                   | TASK_OFFLINE_SIG
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
                   );

    wms_process_signals( sigs );

#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */

  } // for(;;)

} /* wms_task() */

/*===========================================================================
FUNCTION wms_client_cmd_status()                                   (INTERNAL)

DESCRIPTION
  Issue client command status (error or no-error) to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_client_cmd_status(
  wms_cmd_type        *cmd_ptr,
  wms_cmd_err_e_type   cmd_err
)
{

  if (cmd_err != WMS_CMD_ERR_NONE)
  {
    /* There is some real error */
    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Error: %d %s",
                   cmd_err,
                   wms_dbg_get_cmd_err_string(cmd_err) );
  }

  if( cmd_ptr && cmd_ptr->hdr.cmd_cb )
  {
    cmd_ptr->hdr.cmd_cb( cmd_ptr->hdr.cmd_id,
                         cmd_ptr->hdr.user_data,
                         cmd_err );
  }

} /* wms_client_cmd_status() */

/*===========================================================================
FUNCTION wms_notify_client_cmd_status()                            (INTERNAL)

DESCRIPTION
  This function is used to notify client of command error that results
  in the command not being queued for WMS processing, such as:
    - there is no memory resource left to allocate a command buffer
    - a command is not supported
    - a required argument (pointer) is not provided (NULL)


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_notify_client_cmd_status(
  wms_cmd_cb_type      cmd_cb,
  wms_cmd_id_e_type    cmd_id,
  const void           *user_data,
  wms_cmd_err_e_type    cmd_err
)
{
  /* There is some real error */
  MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Error: %d %s",
                 cmd_err,
                 wms_dbg_get_cmd_err_string(cmd_err));

  if (NULL != cmd_cb)
  {
    cmd_cb( cmd_id, (void*)user_data, cmd_err);
  }
} /* wms_notify_client_cmd_status() */

/*===========================================================================
FUNCTION wms_signal_wait()                                         (INTERNAL)

DESCRIPTION
  Suspend WMS from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES

RETURN VALUE
  wms_status_e_type

SIDE EFFECTS
  None

===========================================================================*/
wms_status_e_type wms_signal_wait(
  rex_sigs_type    mask
)
{
  rex_sigs_type    sigs; /* Signals returned by rex_wait */

  /* Loop while waiting for one of the specified signals
  ** to be set.
  */
  for( ;; )
  {
    /* Wait on specified signals, as well as,
    ** on the watchdog signal.
    */
    sigs = rex_wait( mask | WMS_RPT_TIMER_SIG );
    MSG_HIGH_1("Got Signal 0x%x", sigs);

    /* If watchdog signal is set, report back to the watchdog
    */
    if( sigs & WMS_RPT_TIMER_SIG )
    {
      wms_kick_dog();
    }

    /* If one or more of the specified signals is set
    ** return now.
    */
    if ( sigs & WMS_SIM_BUSY_SIG )
    {
      MSG_HIGH_0("Got SIM busy Event, Send out the Error for SIM command");
      (void)rex_clr_sigs( wms_myself, WMS_SIM_BUSY_SIG );
      return WMS_GENERAL_ERROR_S;
    }

    if ( sigs & mask )
    {
      MSG_HIGH_0("Got Response from CM/MMGSDI");
      (void)rex_clr_sigs( wms_myself, mask );
      return WMS_OK_S;
    }

  } /* for */

} /* wms_signal_wait() */

/*===========================================================================
=============================================================================
============================== API FUNCTIONS ================================
=============================================================================
===========================================================================*/

/*===========================================================================

                            Configuration Group

                        API FUNCTION DEFINITIONS

===========================================================================*/
  sys_modem_as_id_e_type wms_get_default_as_id(
    void
  )
  {
    sys_modem_as_id_e_type  as_id     = SYS_MODEM_AS_ID_1;
    wms_cfg_s_type         *cfg_s_ptr = wms_cfg_s_ptr();
  
    if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
    {
      if (SYS_MODEM_DS_PREF_SINGLE_STANDBY == cfg_s_ptr->wms_ph_info.standby_pref)
      {
        as_id = cfg_s_ptr->wms_ph_info.active_subs;
      }
      else
      {
        as_id = cfg_s_ptr->wms_ph_info.default_voice_subs;
      }
  
      if (as_id < SYS_MODEM_AS_ID_NONE ||
          as_id > MAX_AS_IDS )
      {
        as_id = SYS_MODEM_AS_ID_1;
      }
    }
  
    MSG_HIGH_1("Selected default AS_ID: %d", as_id);
  
    return as_id;
  } /* wms_get_default_as_id() */



/*===========================================================================

                            Message Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION wms_msg_ms_send_common

DESCRIPTION
  Processes the common code for wms_msg_ms_send(), wms_msg_ms_send_ext(),
  wms_msg_ms_resend(), and wms_msg_ms_resend_ext() API functions which
  allow the client to send a message over a specified transport layer.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_send_common(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type   *message_ptr,
  boolean                            resend_msg,
  wms_message_number_type            client_msg_id,
  wms_msg_transport_type_e_type      transport_type
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_SEND;
#ifdef FEATURE_CDSMS
  wms_client_ch_pref_e_type    ch_pref      = WMS_CLIENT_CH_NONE;
  wms_dc_so_e_type             dc_so        = WMS_DC_SO_AUTO;
  boolean                      follow_on_dc = FALSE;
#endif /* FEATURE_CDSMS */

  MSG_HIGH_3( "In wms_msg_ms_send_common(client %d, as_id %d, send_mode %d)",
              client_id,
              as_id,
              send_mode );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    if (NULL == message_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    MSG_HIGH_1("In wms_msg_ms_send_common(message_mode %d)", message_ptr->msg_hdr.message_mode);

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_ptr->msg_hdr.message_mode);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

#ifdef FEATURE_CDSMS
    {
      wms_client_s_type           *client_ptr = wms_client_ptr_fetch(client_id);

      if ( NULL != client_ptr )
      {
        ch_pref      = client_ptr->ch_pref;
        dc_so        = client_ptr->so;
        follow_on_dc = client_ptr->follow_on_dc;
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_CLIENT_ID);
        st = WMS_INVALID_CLIENT_ID_S;
      }

      WMS_CLIENT_PTR_RELEASEIF(client_ptr);

      if (WMS_OK_S != st)
      {
        break;
      }
    }
#endif /* FEATURE_CDSMS */

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_send.send_mode      = send_mode;
    cmd_ptr->cmd.msg_send.message        = *message_ptr;
    cmd_ptr->cmd.msg_send.resend_msg     = resend_msg;
    cmd_ptr->cmd.msg_send.client_msg_id  = client_msg_id;
    cmd_ptr->cmd.msg_send.transport_type = transport_type;
#ifdef FEATURE_CDSMS
    cmd_ptr->cmd.msg_send.ch_pref        = ch_pref;
    cmd_ptr->cmd.msg_send.dc_so          = dc_so;
    cmd_ptr->cmd.msg_send.follow_on_dc   = follow_on_dc;
#endif /* FEATURE_CDSMS */

    wms_put_cmd( cmd_ptr );
  } while (0);

  return st;
} /* wms_msg_ms_send_common() */


/*=========================================================================
FUNCTION wms_msg_ms_send

DESCRIPTION
  Allow the client to send a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_send(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type   *message_ptr
)
{
  return wms_msg_ms_send_common( client_id, as_id, cmd_cb,
                                 user_data, send_mode, message_ptr,
                                 FALSE, WMS_DUMMY_MESSAGE_NUMBER,
                                 WMS_MSG_TRANSPORT_TYPE_INTERNAL);

} /* wms_msg_ms_send */

/*=========================================================================
FUNCTION wms_msg_ms_send_ext

DESCRIPTION
  Allow the client to send a message over a specified transport layer.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_send_ext(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type   *message_ptr,
  wms_msg_transport_type_e_type      transport_type
)
{
  return wms_msg_ms_send_common( client_id, as_id, cmd_cb,
                                 user_data, send_mode, message_ptr,
                                 FALSE, WMS_DUMMY_MESSAGE_NUMBER,
                                 transport_type);

} /* wms_msg_ms_send_ext */

/*=========================================================================
FUNCTION wms_msg_ms_resend

DESCRIPTION
  Allow the client to resend a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_resend(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type   *message_ptr,
  wms_message_number_type            client_msg_id
)
{
  return wms_msg_ms_send_common( client_id, as_id, cmd_cb,
                                 user_data, send_mode, message_ptr,
                                 TRUE, client_msg_id,
                                 WMS_MSG_TRANSPORT_TYPE_INTERNAL);

} /* wms_msg_ms_resend */

/*=========================================================================
FUNCTION wms_msg_ms_resend_ext

DESCRIPTION
  Allow the client to resend a message over a specified transport layer.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_resend_ext(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type   *message_ptr,
  wms_message_number_type            client_msg_id,
  wms_msg_transport_type_e_type      transport_type
)
{
  return wms_msg_ms_send_common( client_id, as_id, cmd_cb,
                                 user_data, send_mode, message_ptr,
                                 TRUE, client_msg_id,
                                 transport_type);

} /* wms_msg_ms_resend_ext */

/*=========================================================================
FUNCTION wms_msg_ms_ack

DESCRIPTION
  Allow the client to acknowledge a message if the message has not been
  acknowledged.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_ack(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  const wms_ack_info_s_type      *ack_info_ptr
)
{
  return wms_msg_ms_ack_ext(client_id, as_id, cmd_cb,
                            user_data, ack_info_ptr,
                            WMS_MSG_TRANSPORT_TYPE_INTERNAL);

} /* wms_msg_ms_ack */

/*=========================================================================
FUNCTION wms_msg_ms_ack_ext

DESCRIPTION
  Allow the client to acknowledge a message if the message has not been
  acknowledged over a specified transport layer.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_ack_ext(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  const wms_ack_info_s_type      *ack_info_ptr,
  wms_msg_transport_type_e_type   transport_type

)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_ACK;

  MSG_HIGH_2( "In wms_msg_ms_ack_ext(client %d, as_id %d)",
              client_id,
              as_id );

  do
  {
    wms_prov_app_info_s_type    *appInfo_ptr;
    wms_cmd_type                *cmd_ptr;

    if (NULL == ack_info_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    MSG_HIGH_1("In wms_msg_ms_ack_ext(message_mode %d)", ack_info_ptr->message_mode);

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, ack_info_ptr->message_mode);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_ack.ack_info       = *ack_info_ptr;
    cmd_ptr->cmd.msg_ack.transport_type = transport_type;
#ifdef FEATURE_CDSMS
    cmd_ptr->cmd.msg_ack.ch_pref        = WMS_CLIENT_CH_PREF_ACH_OR_TCH;
#endif /* FEATURE_CDSMS */

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_ack_ext */

/*=========================================================================
FUNCTION wms_msg_ms_read

DESCRIPTION
  Allow the client to read a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_read(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_READ;

  MSG_HIGH_3( "In wms_msg_ms_read(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_read.mem_store   = mem_store;
    cmd_ptr->cmd.msg_read.index       = index;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_read */


/*=========================================================================
FUNCTION wms_msg_ms_write

DESCRIPTION
  Allow the client to write a message to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_write(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type   *message_ptr
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_WRITE;

  MSG_HIGH_2( "In wms_msg_ms_write(client %d, as_id %d)",
              client_id,
              as_id );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    if (NULL == message_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    MSG_HIGH_1("In wms_msg_ms_write(mem_store %d)", message_ptr->msg_hdr.mem_store);

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, message_ptr->msg_hdr.mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_write.write_mode = write_mode;
    cmd_ptr->cmd.msg_write.message    = *message_ptr;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_write */

/*=========================================================================
FUNCTION wms_msg_ms_delete

DESCRIPTION
  Allow the client to delete a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The corresponding Status Report for the Message if stored is also deleted.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_DELETE;

  MSG_HIGH_3( "In wms_msg_ms_delete(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete.mem_store   = mem_store;
    cmd_ptr->cmd.msg_delete.index       = index;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_delete */

/*=========================================================================
FUNCTION wms_msg_ms_read_sts_report

DESCRIPTION
  Allow the client to read a Status Report from a memory store on specified
  subscription account.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_read_sts_report(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_READ_STS_REPORT;

  MSG_HIGH_3( "In wms_msg_ms_read_sts_report(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

#ifdef FEATURE_GWSMS
  {
    do
    {
      wms_prov_app_info_s_type  *appInfo_ptr;
      wms_cmd_type              *cmd_ptr;

      appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

      if (NULL == appInfo_ptr)
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
        st = WMS_NULL_PTR_S;
        break;
      }

      cmd_ptr = wms_get_cmd_buf();

      if (NULL == cmd_ptr)
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
        st = WMS_OUT_OF_RESOURCES_S;
        break;
      }

      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void *)user_data;

      cmd_ptr->cmd.msg_read_sts_report.mem_store   = mem_store;
      cmd_ptr->cmd.msg_read_sts_report.index       = index;

      wms_put_cmd( cmd_ptr );

    } while (0);
  }
#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_ms_read_sts_report */

/*=========================================================================
FUNCTION wms_msg_ms_read_template

DESCRIPTION
  Allow the client to read a template from a memory store on
  specified subscription account. If memory store is NV,
  the message index is ignored since only one template is in NV.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_read_template(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_READ_TEMPLATE;

  MSG_HIGH_3( "In wms_msg_ms_read_template(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_read_template.mem_store   = mem_store;
    cmd_ptr->cmd.msg_read_template.index       = index;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_read_template */

/*=========================================================================
FUNCTION wms_msg_ms_write_template

DESCRIPTION
  Allow the client to write a template from a memory store on
  specified subscription account. If memory store is NV, the message
  index is ignored since only one template is in NV.
  Otherwise, only SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.data
                              to a Valid Value or NULL
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.len
                              to a Valid Value or 0

=========================================================================*/
wms_status_e_type wms_msg_ms_write_template(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  wms_write_mode_e_type            write_mode,
  const wms_client_message_s_type *message_ptr
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_WRITE_TEMPLATE;

  MSG_HIGH_2( "In wms_msg_ms_write_template(client %d, as_id %d)",
              client_id,
              as_id );

  do
  {
    uint8               *alpha_id_data_buff_ptr = NULL;

    if (NULL == message_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    MSG_HIGH_1("In wms_msg_ms_write_template(mem_store %d)", message_ptr->msg_hdr.mem_store);

    // We cannot assume the client set the right combination of message mode and
    // memory store. Without this check the phone might crash.
    // For example, without this check, a GW template with a wrong message mode (CDMA)
    // would call wms_ts_encode_smsp() if mem_store == STORE_SIM. wms_ts_encode_smsp()
    // would crash because alpha ID field was not copied over to the cmd_ptr.
    if (wmsi_msg_check_for_validity(message_ptr->msg_hdr.message_mode,
                                       message_ptr->msg_hdr.mem_store) == FALSE )
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_MSG_MEMORY_STORE);
      st = WMS_GENERAL_ERROR_S;
      break;
    }

    if (WMS_MESSAGE_MODE_GW == message_ptr->msg_hdr.message_mode)
    {
      if ( (NULL != message_ptr->u.gw_template.alpha_id.data) &&
           (0 != message_ptr->u.gw_template.alpha_id.len) )
      {
        alpha_id_data_buff_ptr = (uint8 *)wms_mem_malloc(message_ptr->u.gw_template.alpha_id.len);

        if (NULL == alpha_id_data_buff_ptr)
        {
          wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
          st = WMS_OUT_OF_RESOURCES_S;
          break;
        }

        (void)memscpy( alpha_id_data_buff_ptr,
                       message_ptr->u.gw_template.alpha_id.len,
                       message_ptr->u.gw_template.alpha_id.data,
                       message_ptr->u.gw_template.alpha_id.len );
      }
    } /* end if MESSAGE_MODE_GW */

    {
      wms_prov_app_info_s_type  *appInfo_ptr;
      wms_cmd_type              *cmd_ptr;

      appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, message_ptr->msg_hdr.mem_store);

      if (NULL == appInfo_ptr)
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
        st = WMS_NULL_PTR_S;
        break;
      }

      cmd_ptr = wms_get_cmd_buf();

      if (NULL == cmd_ptr)
      {
        WMS_MEM_FREEIF(alpha_id_data_buff_ptr);
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
        st = WMS_OUT_OF_RESOURCES_S;
        break;
      }

      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void *)user_data;

      cmd_ptr->cmd.msg_write_template.write_mode = write_mode;
      cmd_ptr->cmd.msg_write_template.message    = *message_ptr;

      // Add the msg mode checking to make sure gw alpha_id assignments won't overwrite
      // cdma_template
      if ( WMS_MESSAGE_MODE_GW == message_ptr->msg_hdr.message_mode )
      {
        cmd_ptr->cmd.msg_write_template.message.u.gw_template.alpha_id.len = 0;
        cmd_ptr->cmd.msg_write_template.message.u.gw_template.alpha_id.data = NULL;

        if (NULL != alpha_id_data_buff_ptr)
        {

          cmd_ptr->cmd.msg_write_template.message.u.gw_template.alpha_id.len =
                             message_ptr->u.gw_template.alpha_id.len;
          cmd_ptr->cmd.msg_write_template.message.u.gw_template.alpha_id.data = alpha_id_data_buff_ptr;
        }
      }

      wms_put_cmd( cmd_ptr );
    }
  } while (0);

  return st;
} /* wms_msg_ms_write_template() */

/*=========================================================================
FUNCTION wms_msg_ms_delete_template

DESCRIPTION
  Allow the client to delete a template from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete_template(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_DELETE_TEMPLATE;

  MSG_HIGH_3( "In wms_msg_ms_delete_template(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete_template.mem_store   = mem_store;
    cmd_ptr->cmd.msg_delete_template.index       = index;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_delete_template */

/*=========================================================================
FUNCTION wms_msg_ms_delete_all

DESCRIPTION
  Allow the client to delete all message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete_all(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_tag_e_type          tag
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_DELETE_ALL;

  MSG_HIGH_3( "In wms_msg_ms_delete_all(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete_all.mem_store   = mem_store;
    cmd_ptr->cmd.msg_delete_all.tag         = tag;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_delete_all */

/*=========================================================================
FUNCTION wms_msg_ms_modify_tag

DESCRIPTION
  Allow the client to modify the tag of a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_modify_tag(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_MODIFY_TAG;

  MSG_HIGH_3( "In wms_msg_ms_modify_tag(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_modify_tag.mem_store   = mem_store;
    cmd_ptr->cmd.msg_modify_tag.index       = index;
    cmd_ptr->cmd.msg_modify_tag.tag         = tag;

    wms_put_cmd( cmd_ptr );
  } while (0);

  return st;
} /* wms_msg_ms_modify_tag */

/*=========================================================================
FUNCTION wms_msg_ms_delete_template_all

DESCRIPTION
  Allow the client to delete all templates from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete_template_all(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_DELETE_TEMPLATE_ALL;

  MSG_HIGH_3( "In wms_msg_ms_delete_template_all(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete_template_all.mem_store   = mem_store;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_msg_ms_delete_template_all */

/*=========================================================================
FUNCTION wms_msg_ms_write_sts_report

DESCRIPTION
  Allow the client to write a status report to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The Status Report will only be stored if the corresponding SMS Message
  is also stored in the same Memory. If the SMS Message is not found,
  WMS_CMD_ERR_MSG_NO_MO_MSG will be returned in Command Callback.

=========================================================================*/
wms_status_e_type wms_msg_ms_write_sts_report(
  wms_client_id_type                 client_id,
  sys_modem_as_id_e_type             as_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type   *message_ptr
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_WRITE_STS_REPORT;

  MSG_HIGH_2( "In wms_msg_ms_write_sts_report(client %d, as_id %d)",
              client_id,
              as_id );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    if (NULL == message_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, message_ptr->msg_hdr.mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    MSG_HIGH_1( "wms_msg_ms_write_sts_report(mem_store %d)", message_ptr->msg_hdr.mem_store);

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_write_sts_report.message    = *message_ptr;
    cmd_ptr->cmd.msg_write_sts_report.write_mode = write_mode;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_ms_write_sts_report */

/*=========================================================================
FUNCTION wms_msg_ms_delete_sts_report

DESCRIPTION
  Allow the client to delete a status report from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete_sts_report(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
)
{
  wms_status_e_type    st     = WMS_OK_S;
  wms_cmd_id_e_type    cmd_id = WMS_CMD_MSG_DELETE_STS_REPORT;

  MSG_HIGH_3( "In wms_msg_ms_delete_sts_report(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete_sts_report.mem_store   = mem_store;
    cmd_ptr->cmd.msg_delete_sts_report.index       = index;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_ms_delete_sts_report */

/*=========================================================================
FUNCTION wms_msg_ms_delete_sts_report_all

DESCRIPTION
  Allow the client to delete all status reports from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ms_delete_sts_report_all(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_memory_store_e_type         mem_store
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_MSG_DELETE_STS_REPORT_ALL;

  MSG_HIGH_3( "In wms_msg_ms_delete_sts_report_all(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.msg_delete_sts_report_all.mem_store   = mem_store;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_msg_ms_delete_sts_report_all */


/*=========================================================================
FUNCTION wms_cfg_ms_get_memory_status

DESCRIPTION
  Allow the client to retrieve the status for a memory store.
  If tag==WMS_TAG_NONE: all used slots are calculated;
  otherwise, only messages having this tag value are counted as used_tag_slots.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_get_memory_status(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_GET_MEMORY_STATUS;

  MSG_HIGH_3( "In wms_cfg_ms_get_memory_status(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_get_mem_status.mem_store = mem_store;
    cmd_ptr->cmd.cfg_get_mem_status.tag       = tag;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_get_memory_status */

/*=========================================================================
FUNCTION wms_cfg_ms_get_message_list

DESCRIPTION
  Allow the client to retrieve the message list for a memory store.
  If tag==WMS_TAG_NONE: all types of messages are included in the list;
  otherwise, only messages having this tag value are included.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_get_message_list(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_GET_MESSAGE_LIST;

  MSG_HIGH_3( "In wms_cfg_ms_get_message_list(client %d, as_id %d, mem_store %d)",
              client_id,
              as_id,
              mem_store );

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_mem_store(as_id, mem_store);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_get_msg_list.mem_store = mem_store;
    cmd_ptr->cmd.cfg_get_msg_list.tag       = tag;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_get_message_list */

/*=========================================================================
FUNCTION wms_cfg_ms_set_gw_domain_pref

DESCRIPTION
  Allow the client to set the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_set_gw_domain_pref(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  wms_gw_domain_pref_e_type        gw_domain_pref
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_GW_DOMAIN_PREF;

  MSG_HIGH_3( "In wms_cfg_ms_set_gw_domain_pref(client %d, as_id %d, gw_domain_pref %d)",
              client_id,
              as_id,
              gw_domain_pref );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_set_gw_domain_pref.gw_domain_pref = gw_domain_pref;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED);
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_set_gw_domain_pref */

/*=========================================================================
FUNCTION wms_cfg_set_lte_domain_pref

DESCRIPTION
  Allow the client to set LTE Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_lte_domain_pref(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  uint8                            lte_domain_pref
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_LTE_DOMAIN_PREF;

  MSG_HIGH_2( "In wms_cfg_set_lte_domain_pref(client %d, lte_domain_pref %d)",
              client_id,
              lte_domain_pref );

#ifdef FEATURE_DOMAIN_SELECTION
  do
  {
    wms_cmd_type *cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_set_lte_domain_pref.lte_domain_pref = (sys_sms_domain_pref_e_type)lte_domain_pref;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_DOMAIN_SELECTION */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED);
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_DOMAIN_SELECTION */

  return st;
} /* wms_cfg_set_lte_domain_pref */

/*=========================================================================
FUNCTION wms_cfg_ms_get_gw_domain_pref

DESCRIPTION
  Allow the client to get the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_get_gw_domain_pref(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_GET_GW_DOMAIN_PREF;

  MSG_HIGH_2( "In wms_cfg_ms_get_gw_domain_pref(client %d, as_id %d)",
              client_id,
              as_id );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED);
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_get_gw_domain_pref */

/*===========================================================================
FUNCTION wms_cfg_ms_set_link_control

DESCRIPTION
  Send a request to the lower layers to disable/enable SMS link control for
  connection establishment and timer duration

  Parameter values:
    control_option
        0: Disable - Lower layers don't need to keep up the link when a
                    MO SMS is sent
        1: Enable - If the timer (link period) expires, close the link and
                    automatically switch back to 0 (Disable)
        2: Enable - If the timer (linke period) expires, close the link and
                    retain the perference to be 2
    link_period - timer between the previous and next send command
                    (1 to 5 second interval)
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_cfg_ms_set_link_control(
  wms_client_id_type                client_id,
  sys_modem_as_id_e_type            as_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                       *user_data,
  wms_cfg_link_control_mode_e_type  control_option,
  uint8                             idle_timer /* in seconds */
)
{
  wms_status_e_type    st     = WMS_OK_S;
  wms_cmd_id_e_type    cmd_id = WMS_CMD_CFG_SET_LINK_CONTROL;

  MSG_HIGH_3( "In wms_cfg_ms_set_link_control(client %d, as_id %d, control_option %d)",
              client_id,
              as_id,
              control_option );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr    = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id         = cmd_id;
    cmd_ptr->hdr.client_id      = client_id;
    cmd_ptr->hdr.as_id          = as_id;
    cmd_ptr->hdr.appInfo_ptr    = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb         = cmd_cb;
    cmd_ptr->hdr.user_data      = (void *)user_data;

    cmd_ptr->cmd.cfg_set_link_control.link_control.link_control_mode   = control_option;
    cmd_ptr->cmd.cfg_set_link_control.link_control.link_control_status = WMS_LINK_CONTROL_ACTIVE;
    cmd_ptr->cmd.cfg_set_link_control.link_control_timer               = idle_timer;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED);
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_set_link_control */

/*===========================================================================
FUNCTION wms_cfg_ms_get_link_control

DESCRIPTION
  Retrieves the current link control setting.  WMS will send a cfg event to
  all clients to infom if the link is up or down.and the mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
wms_status_e_type wms_cfg_ms_get_link_control(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type    st     = WMS_OK_S;
  wms_cmd_id_e_type    cmd_id = WMS_CMD_CFG_GET_LINK_CONTROL;

  MSG_HIGH_2( "In wms_cfg_ms_get_link_control(client %d, as_id %d)",
              client_id,
              as_id );

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr    = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id         = cmd_id;
    cmd_ptr->hdr.client_id      = client_id;
    cmd_ptr->hdr.as_id          = as_id;
    cmd_ptr->hdr.appInfo_ptr    = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb         = cmd_cb;
    cmd_ptr->hdr.user_data      = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED);
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_get_link_control()  */

/*=========================================================================
FUNCTION wms_cfg_ms_get_routes

DESCRIPTION
  Allow the client to retrieve the current message routing configuration
  on specified subscription account.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_get_routes(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_GET_ROUTES;

  MSG_HIGH_2( "In wms_cfg_ms_get_routes(client %d, as_id %d)",
              client_id,
              as_id );

  do
  {
    wms_cmd_type              *cmd_ptr;

    if (!asid_is_valid(as_id))
    {
      wms_notify_client_cmd_status( cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_GENERAL_ERROR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void*)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_get_routes */

/*=========================================================================
FUNCTION wms_cfg_ms_set_routes

DESCRIPTION
  Allow the client to change the message routing configuration on
  specified subscription account.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_set_routes(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  const wms_routes_s_type         *routes_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_ROUTES;

  MSG_HIGH_2( "In wms_cfg_ms_set_routes(client %d, as_id %d)",
              client_id,
              as_id );

  do
  {
    wms_cmd_type              *cmd_ptr;

    if (!asid_is_valid(as_id))
    {
      wms_notify_client_cmd_status( cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_GENERAL_ERROR_S;
      break;
    }

    if (NULL == routes_ptr)
    {
      wms_notify_client_cmd_status( cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_set_routes.routes = *routes_ptr;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_set_routes */

/*=========================================================================
FUNCTION wms_cfg_ms_set_primary_client

DESCRIPTION
  This Allow the client to make itself the primary client. The advantage of
  being a  primary client is that your memory status is checked for memory
  full and SMMA Request sent to the network.

USAGE
  Once a primary client is set, no other client can become the primary client.
  If the set_primay == FALSE; the primary client is deregistered

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_ms_set_memory_full()

=========================================================================*/
wms_status_e_type wms_cfg_ms_set_primary_client(
  wms_client_id_type      client_id,
  sys_modem_as_id_e_type  as_id,
  wms_cmd_cb_type         cmd_cb,
  const void             *user_data,
  boolean                 set_primary,
  boolean                 use_client_memory
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_PRIMARY_CLIENT;

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;
    wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();

    appInfo_ptr    = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    /* Handle the case where WMS is not initialized yet and client tries to set itself as
       primary client.
     
       In single SIM mode, it is safe to use appInfo pointer mapped to GW primary session
       (Per current USIM spec, GW SEC is not allowed with single SIM, though it is technically
       possible that cards might have multiple USIM apps). */
    if ((NULL == appInfo_ptr) && (WMS_DEVICE_MODE_SS == cfg_s_ptr->device_mode))
    {
      appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_GW_PROV_PRI_SESSION);
    }

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_set_primary_client.set_primary       = set_primary;
    cmd_ptr->cmd.cfg_set_primary_client.use_client_memory = use_client_memory;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_set_primary_client */

/*=========================================================================
FUNCTION wms_cfg_ms_set_memory_full

DESCRIPTION
  This allows the primary client to specify its Memory Status.

USAGE
  Only the primary client will be allowed to set/reset the Memory Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_ms_set_primary()
=========================================================================*/
wms_status_e_type wms_cfg_ms_set_memory_full(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  boolean                          memory_full
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_MEMORY_FULL;

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr    = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.cfg_set_memory_full.memory_full = memory_full;

    wms_put_cmd( cmd_ptr );

  } while (0);

  return st;
} /* wms_cfg_ms_set_memory_full */


/*=========================================================================
FUNCTION wms_cfg_ms_get_message_waiting

DESCRIPTION
  Allow the client to get the message waiting information on specified
  subscription account.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_get_message_waiting(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_GET_MESSAGE_WAITING;

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    MSG_HIGH_1("wms_cfg_ms_get_message_waiting as_id = %d in WMS context", as_id);
    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);
#else /* FEATURE_GWSMS*/
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_get_message_waiting() */


/*=========================================================================
FUNCTION wms_cfg_ms_set_message_waiting

DESCRIPTION
  Allow the client to change the message waiting information on specified
  subscription account.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_ms_set_message_waiting(
  wms_client_id_type               client_id,
  sys_modem_as_id_e_type           as_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  wms_gw_mwi_info_s_type          *mwi_info,
  uint8                            num_mwi
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SET_MESSAGE_WAITING;

  MSG_HIGH_0("wms_cfg_ms_set_message_waiting");
  if (NULL == mwi_info)
  {
    /* MWI Info is NULL */
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
    st = WMS_NULL_PTR_S;
    return st;
  }

  if ((uint8)WMS_GW_MSG_WAITING_KIND_MAX < num_mwi)
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_BAD_PARM);
    st = WMS_NULL_PTR_S;
    return st;
  }

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;
    int i;

    MSG_HIGH_1("as_id = %d in WMS context", as_id);
    appInfo_ptr    = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    for( i=0; i<num_mwi; i++ )
    {
      cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].type = mwi_info[i].type ;
      cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].count = mwi_info[i].count;
      cmd_ptr->cmd.cfg_set_mwi_info.mwi_info[i].is_active = mwi_info[i].is_active;
    }

    cmd_ptr->cmd.cfg_set_mwi_info.num_mwi = num_mwi;

    wms_put_cmd( cmd_ptr );

  } while (0);
#else /* FEATURE_GWSMS*/
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_cfg_ms_set_message_waiting */

/*=========================================================================
FUNCTION wms_cfg_send_sim_efs_read_event_to_pbm

DESCRIPTION
  Allow PBM to receive indication when WMS finishes
  reading SIM EFS. Asynchronous API

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_send_sim_efs_read_event_to_pbm(
  wms_cfg_efs_read_evt_s_type   send_read_evt_param
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_CFG_SEND_SIM_EFS_READ_EVT;
  wms_cmd_type       *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_session_type(send_read_evt_param.session_type);
    cmd_ptr->hdr.mmgsdi_appInfo = wms_get_mmgsdi_appInfo_by_session_type(send_read_evt_param.session_type); //get the mmgsdi appinfo from session type
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.user_data   = (void *)send_read_evt_param.user_data;
    cmd_ptr->cmd.cfg_send_sim_efs_read_complete_evt = send_read_evt_param;

    MSG_HIGH_3( "SEND_SIM_EFS_READ_EVT requested for session_type %d, t_id %d, app_info %d",
                send_read_evt_param.session_type,
                send_read_evt_param.t_id,
                cmd_ptr->hdr.appInfo_ptr);

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_cfg_send_sim_efs_read_event_to_pbm() */

/*=========================================================================
FUNCTION wms_reg_msg_info_cache_cb

DESCRIPTION
  Allow the client to register its callback function for telling WMS how to
  cache the message information for the messages from the memory.

  If a null pointer is passed, the callback is de-registered. No message
  information will be cahced.

  NOTE: Only one cache callback is allowed.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
void wms_reg_msg_info_cache_cb(
  wms_msg_info_cache_cb_type    msg_info_cache_cb
)
{
  (void)msg_info_cache_cb; // parameter not used

  MSG_ERROR_0("WMS no longer supports msg info cache!");

  return;
} /* wms_reg_msg_info_cache_cb() */


/*===========================================================================

                      Dedicated Channel (DC) Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION wms_dc_enable_auto_disconnect

DESCRIPTION
  This function enables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period. The client will receive an event if the connection is disconnected.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_enable_auto_disconnect(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  uint32                          timeout    /* in seconds */
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DC_ENABLE_AUTO_DISCONNECT;

  MSG_HIGH_2( "In wms_dc_enable_auto_disconnect(client %d, timeout %d)",
              client_id,
              timeout );

#ifdef FEATURE_CDSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.dc_enable_auto_disc.timeout = timeout;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_CDSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_dc_enable_auto_disconnect() */

/*=========================================================================
FUNCTION wms_dc_disable_auto_disconnect

DESCRIPTION
  This function disables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disable_auto_disconnect(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DC_DISABLE_AUTO_DISCONNECT;

  MSG_HIGH_1( "In wms_dc_disable_auto_disconnect(client %d)", client_id);

#ifdef FEATURE_CDSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_CDSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_dc_disable_auto_disconnect() */

/*=========================================================================
FUNCTION wms_dc_connect

DESCRIPTION
  This function requests to establish a DC connection. It will queue
  the request and signal the SMS task to process it. Once the DC connection
  is established or fails, an event will be sent to the client.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_connect(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_dc_so_e_type                so
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DC_CONNECT;

  MSG_HIGH_2( "In wms_dc_connect(client %d, so %d)",
              client_id,
              so );

#ifdef FEATURE_CDSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.dc_connect.so  = so;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_CDSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_dc_connect() */

/*=========================================================================
FUNCTION wms_dc_disconnect

DESCRIPTION
  This function is used to disconnect the DC connection when there is a
  request from the client or from the base station. It will send CDMA
  messages to the base station to disconnect the DC connection.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disconnect(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DC_DISCONNECT;

  MSG_HIGH_1("In wms_dc_disconnect(client %d)", client_id);

#ifdef FEATURE_CDSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_CDSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_dc_disconnect() */

/*===========================================================================

                       Multimode Broadcast Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION wms_bc_ms_get_config

DESCRIPTION
  This function is used to retrieve the configuration for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_ms_get_config(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_CONFIG;

  MSG_HIGH_3( "In wms_bc_ms_get_config(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_get_config.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_get_config() */

/*=========================================================================
FUNCTION wms_bc_ms_get_pref

DESCRIPTION
  This function is used to retrieve the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_get_pref(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_PREF;

  MSG_HIGH_3( "In wms_bc_ms_get_pref(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_get_pref.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_get_pref() */

/*=========================================================================
FUNCTION wms_bc_ms_set_pref

DESCRIPTION
  This function is used to set the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_set_pref(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  wms_bc_pref_e_type              pref
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_SET_PREF;

  MSG_HIGH_3( "In wms_bc_ms_set_pref(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_set_pref.message_mode = message_mode;
      cmd_ptr->cmd.bc_mm_set_pref.bc_pref      = pref;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_set_pref() */

/*=========================================================================
FUNCTION wms_bc_ms_get_table

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_get_table(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_TABLE;

  MSG_HIGH_3( "In wms_bc_ms_get_table(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_get_table.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_get_table() */

/*=========================================================================
FUNCTION wms_bc_ms_select_service

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_select_service(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type    *srv_id_ptr,
  boolean                         selected
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_SELECT_SRV;

  MSG_HIGH_3( "In wms_bc_ms_select_service(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    if (NULL != srv_id_ptr)
    {
      wms_cmd_type        *cmd_ptr;

      cmd_ptr = wms_get_cmd_buf();

      if (NULL != cmd_ptr)
      {
        cmd_ptr->hdr.cmd_id      = cmd_id;
        cmd_ptr->hdr.client_id   = client_id;
        cmd_ptr->hdr.as_id       = as_id;
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
        cmd_ptr->hdr.cmd_cb      = cmd_cb;
        cmd_ptr->hdr.user_data   = (void*)user_data;
        cmd_ptr->cmd.bc_mm_select_srv.message_mode = message_mode;
        cmd_ptr->cmd.bc_mm_select_srv.srv_id       = *srv_id_ptr;
        cmd_ptr->cmd.bc_mm_select_srv.selected     = selected;

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
    else
    {
      /* srv_id_ptr is NULL */
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_select_service() */

/*=========================================================================
FUNCTION wms_bc_ms_get_all_service_ids

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_get_all_service_ids(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_ALL_SRV_IDS;

  MSG_HIGH_3( "In wms_bc_ms_get_all_service_ids(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_get_all_srv_ids.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_get_all_service_ids() */

/*=========================================================================
FUNCTION wms_bc_ms_get_service_info

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_get_service_info(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
 const wms_bc_mm_srv_id_type     *srv_id_ptr
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_SRV_INFO;

  MSG_HIGH_3( "In wms_bc_ms_get_service_info(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    if (NULL != srv_id_ptr)
    {
      wms_cmd_type        *cmd_ptr;

      cmd_ptr = wms_get_cmd_buf();

      if (NULL != cmd_ptr)
      {
        cmd_ptr->hdr.cmd_id      = cmd_id;
        cmd_ptr->hdr.client_id   = client_id;
        cmd_ptr->hdr.as_id       = as_id;
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
        cmd_ptr->hdr.cmd_cb      = cmd_cb;
        cmd_ptr->hdr.user_data   = (void*)user_data;
        cmd_ptr->cmd.bc_mm_get_srv_info.message_mode = message_mode;
        cmd_ptr->cmd.bc_mm_get_srv_info.srv_id       = *srv_id_ptr;

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
    else
    {
      /* srv_id_ptr is NULL */
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_get_service_info() */

/*=========================================================================
FUNCTION wms_bc_ms_add_services

DESCRIPTION
  This function is used to add entries to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_add_services(
  wms_client_id_type                     client_id,
  sys_modem_as_id_e_type                 as_id,
  wms_cmd_cb_type                        cmd_cb,
  const void                            *user_data,
  wms_message_mode_e_type                message_mode,
  uint8                                  num_entries,
  const wms_bc_mm_service_info_s_type   *entries
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_ADD_SRV;

  MSG_HIGH_3( "In wms_bc_ms_add_services(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(entries == NULL || num_entries == 0)
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR );
    st = WMS_NULL_PTR_S;
    return st;
  }


  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_bc_mm_service_info_s_type *entries_buff_ptr = NULL;

    entries_buff_ptr = (wms_bc_mm_service_info_s_type*)
      wms_mem_malloc( num_entries * sizeof(wms_bc_mm_service_info_s_type) );

    if (NULL != entries_buff_ptr)
    {
      wms_cmd_type        *cmd_ptr;

      cmd_ptr = wms_get_cmd_buf();

      if (NULL != cmd_ptr)
      {
        cmd_ptr->hdr.cmd_id      = cmd_id;
        cmd_ptr->hdr.client_id   = client_id;
        cmd_ptr->hdr.as_id       = as_id;
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
        cmd_ptr->hdr.cmd_cb      = cmd_cb;
        cmd_ptr->hdr.user_data   = (void*)user_data;
        cmd_ptr->cmd.bc_mm_add_srv.message_mode = message_mode;
        cmd_ptr->cmd.bc_mm_add_srv.num_entries  = num_entries;
        cmd_ptr->cmd.bc_mm_add_srv.entries      = entries_buff_ptr;
        (void)memscpy( cmd_ptr->cmd.bc_mm_add_srv.entries,
                       num_entries * sizeof(wms_bc_mm_service_info_s_type),
                       entries,
                       num_entries * sizeof(wms_bc_mm_service_info_s_type) );

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
        wms_mem_free(entries_buff_ptr);
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_add_serivices() */

/*=========================================================================
FUNCTION wms_bc_ms_delete_services

DESCRIPTION
  This function is used to delete entries from the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_delete_services(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_srv_id_type    *srv_ids
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_DELETE_SRV;

  MSG_HIGH_3( "In wms_bc_ms_delete_services(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(srv_ids == NULL || num_entries == 0)
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR );
    st = WMS_NULL_PTR_S;
    return st;
  }

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_bc_mm_srv_id_type *srv_id_buff_ptr = NULL;

    srv_id_buff_ptr = wms_mem_malloc( num_entries * sizeof(wms_bc_mm_srv_id_type));

    if (NULL != srv_id_buff_ptr)
    {
      wms_cmd_type        *cmd_ptr;

      cmd_ptr = wms_get_cmd_buf();

      if (NULL != cmd_ptr)
      {
        cmd_ptr->hdr.cmd_id      = cmd_id;
        cmd_ptr->hdr.client_id   = client_id;
        cmd_ptr->hdr.as_id       = as_id;
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
        cmd_ptr->hdr.cmd_cb      = cmd_cb;
        cmd_ptr->hdr.user_data   = (void*)user_data;
        cmd_ptr->cmd.bc_mm_delete_srv.message_mode = message_mode;
        cmd_ptr->cmd.bc_mm_delete_srv.num_entries  = num_entries;
        cmd_ptr->cmd.bc_mm_delete_srv.srv_ids      = srv_id_buff_ptr;
        (void)memscpy( cmd_ptr->cmd.bc_mm_delete_srv.srv_ids,
                       num_entries * sizeof(wms_bc_mm_srv_id_type),
                       srv_ids,
                       num_entries * sizeof(wms_bc_mm_srv_id_type) );

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
        wms_mem_free(srv_id_buff_ptr);
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_delete_services() */

/*=========================================================================
FUNCTION wms_bc_ms_change_service_info

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_change_service_info(
  wms_client_id_type                     client_id,
  sys_modem_as_id_e_type                 as_id,
  wms_cmd_cb_type                        cmd_cb,
  const void                            *user_data,
  wms_message_mode_e_type                message_mode,
  const wms_bc_mm_service_info_s_type   *srv_info_ptr
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_CHANGE_SRV_INFO;

  MSG_HIGH_3( "In wms_bc_ms_change_service_info(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    if (NULL != srv_info_ptr)
    {
      wms_cmd_type        *cmd_ptr;

      cmd_ptr = wms_get_cmd_buf();

      if (NULL != cmd_ptr)
      {
        cmd_ptr->hdr.cmd_id      = cmd_id;
        cmd_ptr->hdr.client_id   = client_id;
        cmd_ptr->hdr.as_id       = as_id;
        cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
        cmd_ptr->hdr.cmd_cb      = cmd_cb;
        cmd_ptr->hdr.user_data   = (void*)user_data;
        cmd_ptr->cmd.bc_mm_change_srv_info.message_mode = message_mode;
        cmd_ptr->cmd.bc_mm_change_srv_info.srv_info     = *srv_info_ptr;

        wms_put_cmd( cmd_ptr );
      }
      else
      {
        wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
        st = WMS_OUT_OF_RESOURCES_S;
      }
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR );
      st = WMS_NULL_PTR_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_change_service_info() */

/*=========================================================================
FUNCTION wms_bc_ms_delete_all_services

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_delete_all_services(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_DELETE_ALL_SERVICES;

  MSG_HIGH_3( "In wms_bc_ms_delete_all_services(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_delete_all_srvs.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_delete_all_services() */

/*=========================================================================
FUNCTION wms_bc_ms_select_all_services

DESCRIPTION
  This function is used to select/deselect all entries from the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_select_all_services(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         selected
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_SELECT_ALL_SERVICES;

  MSG_HIGH_3( "In wms_bc_ms_select_all_services(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }
  else if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_select_all_srvs.message_mode = message_mode;
      cmd_ptr->cmd.bc_mm_select_all_srvs.selected     = selected;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_select_all_services() */

/*=========================================================================
FUNCTION wms_bc_ms_set_priority_for_all_services

DESCRIPTION
  This function is used to set the priorities for all entries from the
  broadcast service table. (Specific to CDMA, Does not apply to GW)

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_set_priority_for_all_services(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  wms_priority_e_type             priority
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES;

  MSG_HIGH_2( "In wms_bc_ms_set_priority_for_all_services(client %d, as_id %d)",
              client_id,
              as_id );

  if(message_mode == WMS_MESSAGE_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_CDSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_set_priority_all_srvs.message_mode = message_mode;
      cmd_ptr->cmd.bc_mm_set_priority_all_srvs.priority     = priority;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }
  return st;
} /* wms_bc_ms_set_priority_all_services() */

/*=========================================================================
FUNCTION wms_bc_ms_msg_delete_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the UI to indicate
  the deletion of a GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_msg_delete_indication(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_gw_cb_page_header_s_type   *cb_page_header
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_MSG_DELETE_INDICATION;

  MSG_HIGH_2( "In wms_bc_ms_msg_delete_indication(client %d, as_id %d)",
              client_id,
              as_id );

#ifdef FEATURE_GWSMS_BROADCAST

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    if (NULL == cb_page_header)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR);
      st = WMS_NULL_PTR_S;
      break;
    }

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    cmd_ptr->cmd.bc_mm_msg_delete_indication.page_hdr = *cb_page_header;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS_BROADCAST */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS_BROADCAST */

  return st;
} /* wms_bc_ms_msg_delete_indication() */

/*=========================================================================
FUNCTION wms_bc_ms_msg_delete_all_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the UI to indicate
  the deletion of ALL GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_msg_delete_all_indication(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION;

  MSG_HIGH_2( "In wms_bc_ms_msg_delete_all_indication(client %d, as_id %d)",
              client_id,
              as_id );

#ifdef FEATURE_GWSMS_BROADCAST

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS_BROADCAST */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS_BROADCAST */

  return st;
} /* wms_bc_ms_msg_delete_all_indication() */

/*=========================================================================
FUNCTION wms_bc_ms_get_reading_pref()

DESCRIPTION
  This function is used to Get the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_get_reading_pref(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_GET_READING_PREF;

  MSG_HIGH_3( "In wms_bc_ms_get_reading_pref(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_get_reading_pref.message_mode = message_mode;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }

  return st;
} /* wms_bc_ms_get_reading_pref */

/*=========================================================================
FUNCTION wms_bc_ms_set_reading_pref

DESCRIPTION
  This function is used to Set the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_ms_set_reading_pref(
  wms_client_id_type              client_id,
  sys_modem_as_id_e_type          as_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         reading_advised,
  boolean                         reading_optional
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  boolean is_command_allowed = FALSE;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_BC_MM_SET_READING_PREF;

  MSG_HIGH_3( "In wms_bd_ms_set_reading_pref(client %d, as_id %d, message_mode %d)",
              client_id,
              as_id,
              message_mode );

  if(message_mode == WMS_MESSAGE_MODE_GW)
  {
#ifdef FEATURE_GWSMS_BROADCAST
    is_command_allowed = TRUE;
#endif /* FEATURE_GWSMS_BROADCAST */
  }

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  if (is_command_allowed)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.as_id       = as_id;
      cmd_ptr->hdr.appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, message_mode);
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.bc_mm_set_reading_pref.message_mode     = message_mode;
      cmd_ptr->cmd.bc_mm_set_reading_pref.reading_advised  = reading_advised;
      cmd_ptr->cmd.bc_mm_set_reading_pref.reading_optional = reading_optional;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
    st = WMS_UNSUPPORTED_S;
  }

  return st;
} /* wms_bc_ms_set_reading_pref */

/*===========================================================================

                            Debugging Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION wms_dbg_reset_tl_seq_num

DESCRIPTION
  Reset internal count for the CDMA SMS TL sequence number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_dbg_reset_tl_seq_num(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_RESET_TL_SEQ_NUM;

  MSG_HIGH_1( "In wms_dbg_reset_tl_seq_num(client %d)", client_id);

#ifdef FEATURE_CDSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(SYS_MODEM_AS_ID_1, WMS_MESSAGE_MODE_CDMA);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_1;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_CDSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_CDSMS */

  return st;
} /* wms_dbg_reset_tl_seq_num() */

/*=========================================================================
FUNCTION wms_dbg_set_msg_ref

DESCRIPTION
  Sets THE TP-MR to 0. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS


=========================================================================*/
wms_status_e_type wms_dbg_set_msg_ref(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_SET_MSG_REF_NUM;

  MSG_HIGH_1( "In wms_dbg_set_msg_ref(client %d)", client_id);

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;
    wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();
    sys_modem_as_id_e_type     as_id     = SYS_MODEM_AS_ID_1;

    if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
    {
      as_id = wms_get_default_as_id();
    }

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_dbg_set_msg_ref */

/*=========================================================================
FUNCTION wms_dbg_clear_smma_flag

DESCRIPTION
  Resets the SMMA Flag. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_clear_smma_flag(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st     = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_CLEAR_SMMA;

  MSG_HIGH_1( "In wms_dbg_clear_smma_flag(client %d)", client_id);

#ifdef FEATURE_GWSMS

  do
  {
    wms_prov_app_info_s_type  *appInfo_ptr;
    wms_cmd_type              *cmd_ptr;
    wms_cfg_s_type            *cfg_s_ptr = wms_cfg_s_ptr();
    sys_modem_as_id_e_type     as_id     = SYS_MODEM_AS_ID_1;

    if (WMS_DEVICE_MODE_SS != cfg_s_ptr->device_mode)
    {
      as_id = wms_get_default_as_id();
    }

    appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode(as_id, WMS_MESSAGE_MODE_GW);

    if (NULL == appInfo_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_AS_ID);
      st = WMS_NULL_PTR_S;
      break;
    }

    cmd_ptr = wms_get_cmd_buf();

    if (NULL == cmd_ptr)
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE);
      st = WMS_OUT_OF_RESOURCES_S;
      break;
    }

    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.as_id       = as_id;
    cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.user_data   = (void *)user_data;

    wms_put_cmd( cmd_ptr );

  } while (0);

#else /* FEATURE_GWSMS */
  wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_UNSUPPORTED );
  st = WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */

  return st;
} /* wms_dbg_clear_smma_flag */

/*=========================================================================
FUNCTION wms_dbg_get_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_get_retry_interval(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_GET_RETRY_INTERVAL;
  wms_cmd_type        *cmd_ptr;

  MSG_HIGH_1("In wms_dbg_get_retry_interval(client %d)", client_id);

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.user_data   = (void*)user_data;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_get_retry_interval() */

/*=========================================================================
FUNCTION wms_dbg_set_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry
  Attempts Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_set_retry_interval(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  uint32                           retry_interval /* seconds */
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_SET_RETRY_INTERVAL;
  wms_cmd_type        *cmd_ptr;

  MSG_HIGH_2( "In wms_dbg_set_retry_interval(client %d, retry_interval %d)",
              client_id,
              retry_interval );

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.user_data   = (void*)user_data;
    cmd_ptr->cmd.dbg_set_retry_interval.retry_interval = retry_interval;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_set_retry_interval() */


/*=========================================================================
FUNCTION wms_dbg_get_retry_period

DESCRIPTION
  Allow a client to retrieve the period of allowing retries of sms messages.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_get_retry_period(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_GET_RETRY_PERIOD;
  wms_cmd_type        *cmd_ptr;

  MSG_HIGH_1("In wms_dbg_get_retry_period(client %d)", client_id);

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.user_data   = (void*)user_data;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_get_retry_period() */

/*=========================================================================
FUNCTION wms_dbg_set_retry_period

DESCRIPTION
  Allow a client to specify the period of allowing retries of sms messages.
  NOTE: if period = 0 seconds, retry will not be attempted.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_set_retry_period(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                      *user_data,
  uint32                           retry_period /* seconds */
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_SET_RETRY_PERIOD;
  wms_cmd_type        *cmd_ptr;

  MSG_HIGH_2( "In wms_dbg_set_retry_period(client %d, retry_period %d)",
              client_id,
              retry_period );

  cmd_ptr = wms_get_cmd_buf();

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id      = cmd_id;
    cmd_ptr->hdr.client_id   = client_id;
    cmd_ptr->hdr.cmd_cb      = cmd_cb;
    cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
    cmd_ptr->hdr.appInfo_ptr = NULL;
    cmd_ptr->hdr.user_data   = (void*)user_data;
    cmd_ptr->cmd.dbg_set_retry_period.retry_period = retry_period;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_dbg_set_retry_period() */


#ifdef FEATURE_FUZZ_TEST
/*=========================================================================
FUNCTION wms_dbg_send_transport_rpt_ind

DESCRIPTION
  Send the transport report indication for debug purpose.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_send_transport_rpt_ind(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                        *user_data,
  wms_msg_transport_rpt_ind_s_type  *rpt_ind_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_id_e_type   cmd_id = WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND;

  MSG_HIGH_1("In wms_dbg_send_transport_rpt_ind(client %d)", client_id);

  if (NULL != rpt_ind_ptr)
  {
    wms_cmd_type        *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = cmd_id;
      cmd_ptr->hdr.client_id   = client_id;
      cmd_ptr->hdr.cmd_cb      = cmd_cb;
      cmd_ptr->hdr.as_id       = SYS_MODEM_AS_ID_NONE;
      cmd_ptr->hdr.appInfo_ptr = NULL;
      cmd_ptr->hdr.user_data   = (void*)user_data;
      cmd_ptr->cmd.transport_rpt_ind = *rpt_ind_ptr;

      wms_put_cmd( cmd_ptr );
    }
    else
    {
      wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NO_RESOURCE );
      st = WMS_OUT_OF_RESOURCES_S;
    }
  }
  else
  {
    /* rpt_ind_ptr is NULL */
    wms_notify_client_cmd_status(cmd_cb, cmd_id, user_data, WMS_CMD_ERR_NULL_PTR );
    st = WMS_NULL_PTR_S;
  }

  return st;
} /* wms_dbg_send_transport_rpt_ind() */

#endif /* FEATURE_FUZZ_TEST */

/*==============================================================================
FUNCTION wms_check_idle

DESCRIPTION
  Function that gets to check if WMS is idle or not
  for determining whether to refresh or not

RETURN VALUE
  TRUE: WMS is idle; refresh can proceed
  FALSE: WMS is not idle; refresh cannot proceed

PARAMETER
  allow_current_command: to return Idle even if the
  currently executed command is calling it
DEPENDENCIES
  None
==============================================================================*/
boolean wms_check_idle(
  sys_modem_as_id_e_type as_id,
  boolean allow_current_command
)
{
  boolean wms_idle = TRUE;

  if (wms_msg_check_in_use_records(as_id))
  {
    /* Some MO, MT records are in use */
    MSG_HIGH_0("WMS Not Idle: Some Mo / MT Records in Use");
    wms_idle = FALSE;
  }
  else
  {
    if (allow_current_command)
    {
      if (wms_get_cmd_count() > 1)
      {
        /* WMS has other commands in the queue */
        MSG_HIGH_0("WMS Not Idle: Command Count > 1");
        wms_idle = FALSE;
      }
    }
    else
    {
      if (wms_get_cmd_count() > 0)
      {
        /* WMS is processing some command */
        MSG_HIGH_0("WMS Not Idle: Command Count > 0");
        wms_idle = FALSE;
      }
    }

#ifdef FEATURE_GWSMS
    wms_idle = wms_cm_gw_is_idle();
#endif /* FEATURE_GWSMS */
  }

  return wms_idle;
} /* wms_check_idle */

sys_modem_as_id_e_type wms_get_as_id_from_message_mode(
   wms_message_mode_e_type message_mode
)
{
   sys_modem_as_id_e_type as_id;

   switch (message_mode)
   {
   case WMS_MESSAGE_MODE_CDMA:
      as_id = SYS_MODEM_AS_ID_1;
      break;
   case WMS_MESSAGE_MODE_GW:
      as_id = SYS_MODEM_AS_ID_2;
      break;
   default:
      as_id = SYS_MODEM_AS_ID_1;
      /* Return a valid as_id to allow error handling in calling functions
      * otherwise _get_prov_app will return WMS_CMD_ERR_AS_ID.
      */
      break;
   }

   return as_id;
}

sys_modem_as_id_e_type wms_get_as_id_from_mem_store(
   wms_memory_store_e_type mem_store
)
{
   sys_modem_as_id_e_type as_id;

   switch (mem_store)
   {
   case WMS_MEMORY_STORE_CSIM:
   case WMS_MEMORY_STORE_RUIM:
   case WMS_MEMORY_STORE_NV_CDMA:
     as_id = SYS_MODEM_AS_ID_1;
     break;
   case WMS_MEMORY_STORE_USIM:
   case WMS_MEMORY_STORE_SIM:
   case WMS_MEMORY_STORE_RAM_GW:
   case WMS_MEMORY_STORE_NV_GW:
     as_id = SYS_MODEM_AS_ID_2;
     break;
   default:
     as_id = SYS_MODEM_AS_ID_1;
     /* Return a valid as_id to allow error handling in calling functions
      * otherwise _get_prov_app will return WMS_CMD_ERR_AS_ID.
      */
     break;
   }

   return as_id;
}

wms_status_e_type wms_msg_put_transport_update_cmd(
  wms_msg_transport_id_type   transport_id,
  sys_modem_as_id_e_type      as_id,
  uint32                      update_item,
  uint32                      item_len,
  void                       *item
)
{
  wms_status_e_type         st            = WMS_UNSUPPORTED_S;
  wms_msg_transport_s_type *transport_ptr = wms_msg_transport_ptr(transport_id);

  MSG_HIGH_5( "wms_msg_put_transport_update_cmd, transport: %d, as_id: %d, update_item: %d, item_len: %d, item: %x",
              transport_id, as_id, update_item, item_len, item );

  if (!asid_is_valid(as_id))
  {
    MSG_ERROR_0( "Invalid as_id" );
  }
  else if (NULL != transport_ptr && NULL != item)
  {
    switch(update_item)
    {
    case WMS_MSG_TRANSPORT_RPT_IND:
      {
        wms_msg_transport_rpt_ind_s_type *rpt_ind_ptr;
        rpt_ind_ptr = (wms_msg_transport_rpt_ind_s_type *) item;
        st = wms_msg_transport_ms_rpt_ind( rpt_ind_ptr, as_id );
      }
      break;
    case WMS_MSG_TRANSPORT_RPT_MWI_IND:
      {
        wms_msg_transport_rpt_mwi_ind_s_type *rpt_mwi_ind_ptr;
        rpt_mwi_ind_ptr = (wms_msg_transport_rpt_mwi_ind_s_type *) item;
        st = wms_msg_transport_ms_rpt_mwi_ind( rpt_mwi_ind_ptr, as_id );
      }
      break;
    case WMS_MSG_TRANSPORT_UPDATE_NW_REG_STATUS:
      {
        wms_msg_transport_nw_reg_status_e_type transport_nw_reg_status;
        memscpy( &transport_nw_reg_status, sizeof(wms_msg_transport_nw_reg_status_e_type), item, item_len );
        st = wms_msg_transport_ms_nw_reg_status_update( transport_id, as_id, transport_nw_reg_status );
      }
      break;
    case WMS_MSG_TRANSPORT_UPDATE_MO_SMS_CB:
      {
        wms_msg_transport_mo_sms_cb_type mo_sms_cb;
        memscpy( &mo_sms_cb, sizeof(wms_msg_transport_mo_sms_cb_type), item, item_len );
        st = wms_msg_transport_ms_reg_mo_sms_cb( transport_id, as_id, mo_sms_cb );
      }
      break;
    case WMS_MSG_TRANSPORT_UPDATE_CAP:
      {
        wms_msg_transport_cap_type transport_cap;
        memscpy( &transport_cap, sizeof(wms_msg_transport_cap_type), item, item_len );
        st = wms_msg_transport_ms_cap_update( transport_id, as_id, transport_cap );
      }
      break;
 
    case WMS_MSG_TRANSPORT_UPDATE_F_TIMER:
      {
        wms_msg_transport_f_timer_type transport_ims_f_timer;
        wms_cmd_err_e_type  cmd_err;
        wms_msg_s_type *msg_s_ptr = wms_msg_s_ptr();

        memscpy( &transport_ims_f_timer, sizeof(wms_msg_transport_f_timer_type), item, item_len );

        MSG_HIGH_1("Received Timerf value = %d milli seconds ", transport_ims_f_timer);
        msg_s_ptr->timerf_value = transport_ims_f_timer;
        cmd_err = wms_cm_gw_rl_update_timer_f(as_id, transport_ims_f_timer);
        st = (WMS_CMD_ERR_NONE == cmd_err) ? WMS_OK_S : WMS_OUT_OF_RESOURCES_S;
      }
      break;
	
    default:
      MSG_ERROR_1("Invalid update_item: %d", update_item);
      break;
    }
  }
  
  return st;
} /* wms_msg_put_transport_update_cmd */

/*=========================================================================
FUNCTION wmsi_msg_check_for_validity

DESCRIPTION
  Check if the message template has the right combination of message mode
  and memory store

DEPENDENCIES
  None

RETURN VALUE
  FALSE: The memory store is not supported for that message mode
  TRUE: The memory store might be supported for that message mode

SIDE EFFECTS
  None

=========================================================================*/
boolean wmsi_msg_check_for_validity(
  wms_message_mode_e_type message_mode,
  wms_memory_store_e_type mem_store
)
{
  if ((message_mode == WMS_MESSAGE_MODE_GW &&
       (mem_store == WMS_MEMORY_STORE_SIM ||
        mem_store == WMS_MEMORY_STORE_USIM ||
        mem_store == WMS_MEMORY_STORE_NV_GW ||
        mem_store == WMS_MEMORY_STORE_RAM_GW))
      ||
      (message_mode == WMS_MESSAGE_MODE_CDMA &&
      (mem_store == WMS_MEMORY_STORE_CSIM ||
       mem_store == WMS_MEMORY_STORE_RUIM ||
       mem_store == WMS_MEMORY_STORE_NV_CDMA)))
  {
    return TRUE;
  }

  return FALSE;
} /* wmsi_msg_check_for_validity */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wms.c */
