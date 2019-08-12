/*===========================================================================

                             D S M S G R R E C V . C
 
GENERAL DESCRIPTION
  Implementation of DS MSGR RECVer-side helper apis.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2014 by Qualcomm Technologies, Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/msgr/src/dsmsgrrecv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/14   vrk     Added support for POLICYMAN_CFG_UPDATE_MSIM_IND
09/25/12   ss      3GPP MH LTE-Off compilation fixes.
10/03/11   ua      Added support for LTE_CPHY_RFCHAIN_CNF to support DRX/PRX
                   in ATCoP. Currently, its only registration. 
11/18/10   sa      Added support of testing LTE_TLB_DS_CTL_READY_IND.
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
08/10/10   sa      Added support of LTE_TLB_LB_OPEN_IND.
04/23/10   vk      Use of MSGR for UL flow control event processing in LTE
08/09/09   vk      Moving DS to its own MSGR technology space
05/01/09   vk      Added support for test synchronization with DS task
12/22/08   vk      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#ifdef TEST_FRAMEWORK
#error code not present
#endif


#include "amssassert.h"
#include "err.h"
#include "dsmsgr.h"
#include "queue.h"
#include "pthread.h"
#include "msgr.h"
#include "appmgr.h"

#ifdef FEATURE_DATA_LTE
#include "lte_pdcp_msg.h"
#include "lte_tlb_msg.h"
#include "lte_tlb.h"
#include "ds_eps_msg.h"
#include "lte_rlc_msg.h"
#include "emm_ext_msg.h"
#include "lte_cphy_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_cphy_rssi_msg.h"
#endif

#include "dsmsgrrecv_msg.h"
#include "ds_3gpp_msg.h"
#include "ds3gpp_ext_msg.h"
#include "data_msg.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "dog.h"
#if (defined(FEATURE_WDOG_DYNAMIC) || defined (DOG_DYNAMIC_API))
#define FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
#endif /* FEATURE_WDOG_DYNAMIC */

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
#include "rex.h"
#include "mcs_timer.h"
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

#ifdef FEATURE_TDSCDMA
#include "tds_rrc_ext_if.h"
#endif  /* FEATURE_TDSCDMA */

#include "ds_qmi_dms.h"
#include "policyman_msg.h"

#include "ds3gflowmgr.h"

#include "cm_msgr_msg.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define ARRSIZ(a)	(sizeof(a)/sizeof(a[0]))

/*
** Receiver-side type defs
*/

/* Union of all messages that DS can receive */

typedef union {
  /* This will be the only info in case the message is a SYNC MSG*/
  msgr_hdr_s                         hdr;

#ifdef FEATURE_DATA_LTE
  /* These messages will be received from PDCP */
  lte_pdcpul_rab_register_cnf_msg_s  ds_lte_pdcpul_rab_register_cnf_msg;
  lte_pdcpdl_rab_register_cnf_msg_s  ds_lte_pdcpdl_rab_register_cnf_msg;
  lte_pdcp_rab_deregister_cnf_msg_s  ds_lte_pdcp_rab_deregister_cnf_msg;
  lte_tlb_lb_close_tl_ind_s          ds_lte_tlb_lb_close_tl_ind_s;
  emm_t3402_changed_ind_s_type       ds_lte_emm_t3402_changed_ind;
  emm_reset_apn_switch_ind_type      ds_lte_reset_attach_apn_ind;
  lte_cphy_ca_event_ind_s            ds_lte_cphy_ca_event_ind;
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_EMBMS
  /* These messages will be received from LTE RRC*/
  lte_rrc_embms_status_change_ind_s   ds_lte_rrc_embms_status_change_ind;
  lte_rrc_embms_act_tmgi_list_ind_s   ds_lte_rrc_embms_act_tmgi_list_ind;
  lte_rrc_embms_avail_tmgi_list_ind_s ds_lte_rrc_embms_avail_tmgi_list_ind;
  lte_rrc_embms_deact_tmgi_ind_s      ds_lte_rrc_embms_deact_tmgi_ind;
  lte_rrc_embms_deact_tmgi_cnf_s      ds_lte_rrc_embms_deact_tmgi_cnf;
  lte_rrc_embms_act_tmgi_cnf_s        ds_lte_rrc_embms_act_tmgi_cnf;
  lte_rrc_embms_oos_warn_ind_s        ds_lte_rrc_embms_warn_tmgi_list_ind;
  lte_rlcdl_embms_wm_register_cnf_msg_s    ds_lte_rlcdl_embms_wm_register_cnf_msg_s;
  lte_rlcdl_embms_wm_deregister_cnf_msg_s  ds_lte_rlcdl_embms_wm_deregister_cnf_msg_s;
  ds_3gpp_embms_content_desc_control_msg_s ds_3gpp_embms_content_desc_control_msg;
#endif /* FEATURE_DATA_EMBMS */
  /* These messages will be received from 3gpp mode handler */
  ds_3gpp_phys_link_flow_disable_msg_s ds_3gpp_phys_link_flow_disable_msg;
  ds_3gpp_phys_link_flow_enable_msg_s  ds_3gpp_phys_link_flow_enable_msg;
  cxm_coex_desense_ind_s               ds_coex_desense_ind;
  qmi_dms_policyman_cfg_update_ind_type_msg_s  policyman_cfg_update_ind_msg;
  cm_plmn_list_chngd_ind_s_type        plmn_list_change_ind;
} dsmsgrrcv_msg_u;


typedef struct {
  q_link_type     link;
  dsmsgrrcv_msg_u msg;
} dsmsgrrcv_cmd_s;

typedef union {
  msgr_hdr_s        hdr;
  struct {
    dsmsgrrcv_msg_u msg;
  } ds_cmd;
  msgr_spr_loopback_s loopback_msg;

#ifdef FEATURE_TDSCDMA
  tds_rrc_set_config_rsp_type         ds_tds_rrc_set_config_rsp_type_msg;
  tds_rrc_get_config_rsp_type         ds_tds_rrc_get_config_rsp_type_msg;
#endif

} dsmsgrrcv_super_cmd_u;

#define     DSMSGRRCV_CMD_MAX        24

static dsmsgrrcv_cmd_s dsmsgrrcv_cmd_arr[DSMSGRRCV_CMD_MAX] = {{{NULL}}};

typedef struct {
  msgr_umid_type        msgrtype;
  dsmsgrcv_msg_hdlr_f   hdlrfunc;
} dsmsgrrcv_msg_map_t;

static dsmsgrrcv_msg_map_t dsmsgrrcv_msg_map_arr[] = {
#ifdef FEATURE_DATA_LTE
  { LTE_PDCPUL_RAB_REGISTER_CNF, NULL },
  { LTE_PDCPDL_RAB_REGISTER_CNF, NULL },
  { LTE_PDCPUL_RAB_DEREGISTER_CNF, NULL },
  { LTE_PDCPDL_RAB_DEREGISTER_CNF, NULL },
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_DATA_LTE
  { LTE_TLB_LB_ACT_IND, NULL },
  { LTE_TLB_LB_CLOSE_TL_IND, NULL },
  { LTE_TLB_LB_OPEN_IND, NULL },
  { LTE_TLB_LB_DEACT_IND, NULL },
  { NAS_EMM_IND_PLMN_CHANGE, NULL},
  { NAS_EMM_ATTACH_COMPLETE_IND, NULL},
  { NAS_EMM_RESET_APN_SWITCH_IND, NULL},
  { LTE_CPHY_TX_POWER_REPORT_INFO_CNF, NULL},
  { LTE_CPHY_RFCHAIN_CNF, NULL },
  { NAS_EMM_T3402_CHANGED_IND, NULL },
  { NAS_EMM_DEACT_NON_EMC_BEARER_IND, NULL },
  { LTE_CPHY_DL_TPUT_ESTM_REPORT_IND, NULL },
  { LTE_CPHY_DL_TPUT_ESTM_REPORT_RSP, NULL },
  { LTE_RRC_DEACTIVATE_REQ, NULL},
  { LTE_RRC_UE_CAP_INFO_IND, NULL},
  { LTE_CPHY_CA_EVENT_IND , NULL },
  { LTE_RRC_CSFB_CALL_STATUS_IND, NULL},
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_EMBMS
  { LTE_RRC_EMBMS_ACT_TMGI_CNF, NULL },
  { LTE_RRC_EMBMS_DEACT_TMGI_CNF, NULL },
  { LTE_RRC_EMBMS_ACT_DEACT_TMGI_CNF, NULL },
  { LTE_RRC_EMBMS_DEACT_TMGI_IND, NULL },
  { LTE_RRC_EMBMS_AVAIL_TMGI_LIST_IND, NULL },
  { LTE_RRC_EMBMS_ACT_TMGI_LIST_IND, NULL},
  { LTE_RRC_EMBMS_AVAIL_SAI_LIST_IND, NULL},  
  { LTE_RRC_EMBMS_STATUS_CHANGE_IND, NULL},
  { LTE_RRC_EMBMS_OOS_WARN_IND, NULL},
  { LTE_RLCDL_EMBMS_REG_CNF, NULL},
  { LTE_RLCDL_EMBMS_DEREG_CNF, NULL},
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
  { DS_LTE_EMBMS_CONTENT_DESC_CONTROL_IND, NULL },
#endif /* FEATURE_DATA_EMBMS */

  { DS_3GPP_PHYS_LINK_FLOW_DISABLE_REQ, NULL },
  { DS_3GPP_PHYS_LINK_FLOW_ENABLE_REQ, NULL },
  { POLICYMAN_CFG_UPDATE_MSIM_IND , NULL },
  { DS_MSGRRECV_DESENSE_IND, NULL },
  { MM_CM_PLMN_LIST_CHANGE_IND, NULL },
  { MM_CM_PSM_GET_CFG_PARAMS_RSP, NULL },
  { MM_CM_PSM_STATUS_IND, NULL }
};

#define DSMSGRRCV_MSG_MAP_ARR_MAX   ARRSIZ(dsmsgrrcv_msg_map_arr)

static q_type dsmsgrrcv_cmd_q = {{NULL}};
static q_type dsmsgrrcv_cmd_free_q = {{NULL}};

/*---------------------------------------------------------------------------
  Dog data structures for DSMSGRRCV POSIX thread
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
LOCAL void dsmsgrrcv_wdog_expired_ind_handler
(
  void
);
LOCAL void dsmsgrrcv_timer_cb
( 
  mcs_timer_cb_data_type data
);

typedef struct
{
  mcs_timer_type timer;               /*!< MCS timer  */
  msgr_tech_module_type  moduleid;    /*!< TECH,MODULE Performing Operation */
  msgr_umid_type umid;                /*!< UMID of message to be posted from callback */
} dsmsgrrcv_timer_s;

LOCAL boolean dsmsgrrcv_dog_rpt_enabled = TRUE; /*! Reporting enabled for this task */
typedef struct
{
  uint32 rpt_var;                     /*! dog report ID       */
  uint32 rpt_time_var;                /*! Report Period       */
  dsmsgrrcv_timer_s rpt_timer;        /*! timer info          */
} dsmsgrrcv_dog_info_s;

LOCAL dsmsgrrcv_dog_info_s dsmsgrrcv_wdog_info;
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

static pthread_mutex_t dsmsgrrcv_mutex;

static pthread_t       dsmsgrrcv_thread;

static msgr_client_t   dsmsgrrcv_msgr_client;
static msgr_id_t       dsmsgrrcv_msgr_q_id;

#define     DSMSGRRCV_MSGR_PRI          1
#define     DSMSGRRCV_MSGR_MAX_MSGS     16
#define     DSMSGRRCV_MSGR_MAX_MSG_SZ   sizeof(dsmsgrrcv_msg_u)

static dsmsgrrcv_super_cmd_u dsmsgrrcv_super_cmd;

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

static dsmsgrrcv_cmd_s * dsmsgrrcv_get_cmd_buf (void)
{
  dsmsgrrcv_cmd_s * cmd_p;

  cmd_p = q_get(&dsmsgrrcv_cmd_free_q);
  ASSERT(cmd_p != NULL);

  return cmd_p;
}

static void dsmsgrrcv_free_cmd_buf (dsmsgrrcv_cmd_s * cmd_p)
{
  ASSERT(cmd_p != NULL);

  q_put(&dsmsgrrcv_cmd_free_q, &cmd_p->link);
}

static void dsmsgrrcv_post_cmd (dsmsgrrcv_cmd_s * cmd_p)
{
  q_put( &dsmsgrrcv_cmd_q, &cmd_p->link );
}

static dsmsgrrcv_cmd_s * dsmsgrrcv_get_next_cmd (void)
{
  dsmsgrrcv_cmd_s * cmd_p;

  cmd_p = q_get(&dsmsgrrcv_cmd_q);
  return cmd_p;
}

static dsmsgrrcv_msg_map_t * dsmsgrrcv_get_msg_map_p (msgr_umid_type msgrtype)
{
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    if (msgrtype == msg_map_p->msgrtype) 
    {
      break;
    }
  }

  if (i == DSMSGRRCV_MSG_MAP_ARR_MAX) 
  {
    DATA_MSG1_ERROR("Invalid msgrtype: %d, no mapping found", msgrtype);
    msg_map_p = NULL;
  }

  return msg_map_p;
}

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

void dsmsgrrcv_reg_hdlr (msgr_umid_type msgrtype, dsmsgrcv_msg_hdlr_f dshdlr)
{
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  ASSERT(dshdlr != NULL);

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    if (msgrtype == msg_map_p->msgrtype) 
    {
#ifndef TEST_FRAMEWORK
      ASSERT(msg_map_p->hdlrfunc == NULL);
#endif
      msg_map_p->hdlrfunc = dshdlr;
      break;
    }
  }

  ASSERT(i <= DSMSGRRCV_MSG_MAP_ARR_MAX);
  return;
}

void dsmsgrrcv_init (void)
{
  int i;
  dsmsgrrcv_cmd_s * cmd_p;

  (void)q_init( &dsmsgrrcv_cmd_q );
  (void)q_init( &dsmsgrrcv_cmd_free_q );

  for (i = 0, cmd_p = &dsmsgrrcv_cmd_arr[0];
       i < DSMSGRRCV_CMD_MAX; 
       ++i, ++cmd_p)
  {
    (void)q_link(cmd_p, &cmd_p->link);
    q_put(&dsmsgrrcv_cmd_free_q, &cmd_p->link);
  }

  (void)pthread_mutex_init(&dsmsgrrcv_mutex, NULL);

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
  if (dsmsgrrcv_dog_rpt_enabled)
  {
    rex_tcb_type *rex_tcb_ptr = rex_self(); /*! Get my TCB pointer */
    if(NULL != rex_tcb_ptr)
    {
      /*-----------------------------------------------------------------------
        Watchdog registration and timer kick-off.
        -----------------------------------------------------------------------*/
      dsmsgrrcv_wdog_info.rpt_var     = dog_register (rex_tcb_ptr, 
                                                      DOG_DEFAULT_TIMEOUT, 
                                                      DOG_DEADLKCHK_ENABLE);
      dsmsgrrcv_wdog_info.rpt_time_var   = 
                         dog_get_report_period (dsmsgrrcv_wdog_info.rpt_var);

      /*----------------------------------------------------------------------
        If the return value from dog_get_report_period() was zero then we MUST
        NOT report to dog directly, as DOG will monitor us in a different manner.
        Hence we won't initialize or start any timer.
      -----------------------------------------------------------------------*/
      if (dsmsgrrcv_wdog_info.rpt_time_var > 0) 
      {
        /*--------------------------------------------------------------------
        Define and start the watchdog timer to wake up for the 1st petting.
        ---------------------------------------------------------------------*/
        dsmsgrrcv_wdog_info.rpt_timer.moduleid = MSGR_DS_MSGRRECV;
        dsmsgrrcv_wdog_info.rpt_timer.umid = 
                             DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI;
        mcs_timer_def(&dsmsgrrcv_wdog_info.rpt_timer.timer,
                      NULL,
                      dsmsgrrcv_timer_cb,
                     (mcs_timer_cb_data_type)&dsmsgrrcv_wdog_info.rpt_timer);
        mcs_timer_set (&dsmsgrrcv_wdog_info.rpt_timer.timer, 
                       dsmsgrrcv_wdog_info.rpt_time_var, 0, T_MSEC);
      }
    }/* NULL != rex_tcb_ptr */
  } /* dsmsgrrcv_dog_rpt_enabled */
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */
}

void dsmsgrrcv_register (void)
{
  errno_enum_type errnum;
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  errnum = msgr_client_create(&dsmsgrrcv_msgr_client);
  ASSERT(errnum == E_SUCCESS);

  errnum = msgr_client_add_mq
           (
             "DS MSGR RECV",
             &dsmsgrrcv_msgr_client,
             DSMSGRRCV_MSGR_PRI,
             DSMSGRRCV_MSGR_MAX_MSGS,
             DSMSGRRCV_MSGR_MAX_MSG_SZ,
             &dsmsgrrcv_msgr_q_id
           );
  ASSERT(errnum == E_SUCCESS);

  /* 
  ** Now register for all messages
  */

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_SPR_LOOPBACK
           );
  ASSERT(errnum == E_SUCCESS);

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_SPR_THREAD_KILL
           );
  ASSERT(errnum == E_SUCCESS);

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    errnum = msgr_register
             (
               MSGR_DS_MSGRRECV, 
               &dsmsgrrcv_msgr_client,
               dsmsgrrcv_msgr_q_id,
               msg_map_p->msgrtype
             );
    ASSERT(errnum == E_SUCCESS);
  }

   /* 
   ** Register for WDOG expiry internal timer indication
   */
#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
   errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI
           );
  ASSERT(errnum == E_SUCCESS);
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

#ifdef FEATURE_TDSCDMA
  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             TDSCDMA_RRC_SET_CONFIG_RSP
           );
  ASSERT(errnum == E_SUCCESS);

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             TDSCDMA_RRC_GET_CONFIG_RSP
           );
  ASSERT(errnum == E_SUCCESS);
#endif
}

void dsmsgrrcv_task_exit (void)
{
  int rval = -1;
  // Release the mailbox
  (void) msgr_client_delete(&dsmsgrrcv_msgr_client);

  // release mutex
  rval = pthread_mutex_destroy(&dsmsgrrcv_mutex);

  if( rval != 0 )
  {
    ERR_FATAL("Failed dsmsgrrcv_mutex destroy",0,0,0);
  }

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
  if (dsmsgrrcv_dog_rpt_enabled)
  {
    dog_deregister(dsmsgrrcv_wdog_info.rpt_var);
    (void ) mcs_timer_clr(&dsmsgrrcv_wdog_info.rpt_timer.timer,T_SCLK);
  }
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

  pthread_exit(NULL);
}

void * dsmsgrrcv_thrd_main (void * arg)
{
  errno_enum_type errnum;
  dsmsgrrcv_cmd_s * cmd_p;
  uint32 msglen;
  dsmsgrrcv_msg_map_t * msg_map_p;

  dsmsgrrcv_init();
  dsmsgrrcv_register();

  appmgr_thread_init_complete(MSGR_DS_MSGRRECV);

  for (;;)
  {
    errnum = msgr_receive
             (
               &dsmsgrrcv_msgr_client,
               (uint8 *)&dsmsgrrcv_super_cmd,
               DSMSGRRCV_MSGR_MAX_MSG_SZ,
               &msglen
             );
    ASSERT(errnum == E_SUCCESS);
    ASSERT(msglen <= DSMSGRRCV_MSGR_MAX_MSG_SZ);

    switch (dsmsgrrcv_super_cmd.hdr.id) {
    case DS_MSGRRECV_SPR_LOOPBACK:
    {
      msgr_spr_loopback_struct_type loopback_msg;

      errnum = msgr_loopback_reply(&loopback_msg, MSGR_DS_MSGRRECV);
      ASSERT(errnum == E_SUCCESS);
      break;
    }
    case DS_MSGRRECV_SPR_THREAD_KILL:
    {
      // Thread is being killed, cleanup
      dsmsgrrcv_task_exit();
      break;
    }

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
    case DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI:
    {
      // WDOG timer expired, report DOG, and kick the timer again.
      if (dsmsgrrcv_dog_rpt_enabled) 
      {
        dsmsgrrcv_wdog_expired_ind_handler();
      }
      break;
    }
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

#ifdef FEATURE_TDSCDMA
     case TDSCDMA_RRC_SET_CONFIG_RSP:
     {
       qmi_dms_tdscdma_set_config_res_cb(
          &dsmsgrrcv_super_cmd.ds_tds_rrc_set_config_rsp_type_msg);
       break;
     }

     case TDSCDMA_RRC_GET_CONFIG_RSP:
     {
       qmi_dms_tdscdma_get_config_res_cb(
          &dsmsgrrcv_super_cmd.ds_tds_rrc_get_config_rsp_type_msg);
       break;
     }
#endif

    default:
    {
      /*policyman_ind may come even before DS task is ready, this check ensures
         not to post any command to DS till some handler is registered*/
      msg_map_p = dsmsgrrcv_get_msg_map_p(dsmsgrrcv_super_cmd.ds_cmd.msg.hdr.id);
      if ((msg_map_p != NULL) && 
          (msg_map_p->hdlrfunc == NULL) && 
          (msg_map_p->msgrtype == POLICYMAN_CFG_UPDATE_MSIM_IND))
      {
        break;
      }

      cmd_p = dsmsgrrcv_get_cmd_buf();
      ASSERT(cmd_p != NULL);

      cmd_p->msg = dsmsgrrcv_super_cmd.ds_cmd.msg;

      dsmsgrrcv_post_cmd(cmd_p);
      dsmsgr_post_ds_cmd();
      break;
    }
    }
  } /* for(;;) */
}

pthread_t dsmsgrrecv_task (int priority)
{
  int rval;
  pthread_attr_t attr;
  struct sched_param param;

  // Init thread attributes
  rval = pthread_attr_init(&attr);
  ASSERT(rval == 0);
  
  memset(&param,0,sizeof(param));
  
  // Set priority to what the app mgr passed in
  param.sched_priority = priority;
  rval = pthread_attr_setschedparam(&attr, &param);

#ifndef T_UNIX
  rval = pthread_attr_setthreadname(&attr, "DSMSGR RECV");
  ASSERT(rval == 0);
#endif

  rval = pthread_create
         (
           &dsmsgrrcv_thread, 
           &attr,
           dsmsgrrcv_thrd_main,
           NULL
         );

  if (rval != 0) 
  {
    DATA_MSG0_ERROR("DSMSGR RECV thread creation failed!");
  }

  return dsmsgrrcv_thread;
}

void dsmsgr_process_ds_cmd (void * ds_cmd_p)
{
  dsmsgrrcv_cmd_s * cmd_p;
  dsmsgrrcv_msg_map_t * msg_map_p;
  boolean rval;

  while ((cmd_p = dsmsgrrcv_get_next_cmd()) != NULL)
  {
    msg_map_p = dsmsgrrcv_get_msg_map_p(cmd_p->msg.hdr.id);

    if(msg_map_p != NULL)
    {
      rval = FALSE;
      if (msg_map_p->hdlrfunc != NULL) 
      {
        rval = msg_map_p->hdlrfunc(msg_map_p->msgrtype,\
		      (msgr_hdr_struct_type*)&cmd_p->msg);
      } 

      if (rval == FALSE) 
      {
        DATA_MSG1_ERROR("Failed to handle msg %d", msg_map_p->msgrtype);
      }
  }

    dsmsgrrcv_free_cmd_buf(cmd_p);
  }
}

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
/*===========================================================================

  FUNCTION:  dsmsgrrcv_timer_cb

===========================================================================*/
/*!
    @brief
    This function is called by MCS when the timer expires.

    @detail
    This function posts a message with the specified message id.

    @return
    
    @note
    Currently no users other than WDOG timer, hence wrapped under that feat.

*/
/*=========================================================================*/
LOCAL void dsmsgrrcv_timer_cb
( 
  mcs_timer_cb_data_type data
)
{
  msgr_hdr_s hdr;
  errno_enum_type result;
  dsmsgrrcv_timer_s* timer;

  timer = ( dsmsgrrcv_timer_s* )data;
  msgr_init_hdr(&hdr, timer->moduleid, timer->umid);
  result = msgr_send(&hdr,sizeof(hdr));
  ASSERT(E_SUCCESS == result);
}

/*===========================================================================

  FUNCTION:  dsmsgrrcv_wdog_expired_handler

===========================================================================*/
/*!
  @brief
  WDOG expiration handler

*/
/*=========================================================================*/
LOCAL void dsmsgrrcv_wdog_expired_ind_handler
(
  void
)
{
  if (dsmsgrrcv_wdog_info.rpt_time_var > 0) 
  {
    dog_report(dsmsgrrcv_wdog_info.rpt_var);
    mcs_timer_set (&dsmsgrrcv_wdog_info.rpt_timer.timer, 
                   dsmsgrrcv_wdog_info.rpt_time_var, 0, T_MSEC);
  }
} /* dsmsgrrcv_wdog_timer_expired_ind_handler */
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */



