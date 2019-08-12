/*===========================================================================

         C D M A   Q S H    F R A M E W O R K    S U P P O R T

DESCRIPTION
  This contains all the 1X_CP internal data and API definitions required for 
  supporting QSH (Qualcomm Sherlock Holmes) framework.

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/onex_voice_adapt_if.h   1.8   10 Mar 2015 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccqsh.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/03/16   ppr     QSH:Generic Event changes
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
07/21/16   sjo     Avoid processing QSH cmd when 1X is inactive.
06/16/16   agh     Log only valid values of TX power in traffic state
06/14/16   sjo     Changes to fix compilation issues on certain flavours.
01/27/16   agh     Initial Revision

  ============================================================================*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/
#include "mccqsh_i.h"
#include "txccommon_i.h"
#include "srch_v.h"
#include "srchi.h"


/* Macros and Data definitions */
#define MC_QSH_METRIC_CELL_INFO_COUNT 10
#define MC_QSH_METRIC_RF_PARAMS_COUNT 10

/* Periodicity of metrics in ms */
#define MC_QSH_METRIC_PERIODICITY 200

#ifdef FEATURE_1XCP_QSH_SUPPORT
qsh_ext_metric_cfg_s  onexl3_qsh_metric_default_cfg_arr[ONEXL3_QSH_METRIC_MAX];
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/* Variable to store last known valid value of TX power in traffic state */
static int16 valid_tx_power = -32768;

/*===========================================================================

                      FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_1XCP_QSH_SUPPORT
/*===========================================================================
  
  FUNCTION MC_QSH_INIT
  
  DESCRIPTION
    This function registers 1X_CP client with the QSH module
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None.
  
===========================================================================*/

void mc_qsh_init
( 
  void 
)
{
  qsh_client_reg_s mc_client_reg;  
  int i;

  /* Initialize the local global array to keep track of registered metrics 
   * and events */
  memset(&mc_qsh_data, 0, sizeof(mc_qsh_data_type));
  for (i = 0; i<ONEXL3_QSH_METRIC_MAX; i++)
  {
    mc_qsh_data.metric_cfg[i].id = QSH_METRIC_ID_NONE;
    mc_qsh_data.metric_cfg[i].action = QSH_METRIC_ACTION_MAX;
  }
  
  /* Initialize 1XCP client */
  qsh_client_reg_init(&mc_client_reg);
  
  /* Provide 1X_CP client information to QSH */
  mc_client_reg.client = QSH_CLT_ONEXL3;
  mc_client_reg.client_cb_ptr = mc_qsh_cb;
  mc_client_reg.major_ver = ONEXL3_QSH_MAJOR_VER;
  mc_client_reg.minor_ver = ONEXL3_QSH_MINOR_VER;
  mc_client_reg.cb_action_support_mask = QSH_ACTION_METRIC_CFG | 
                                         QSH_ACTION_METRIC_TIMER_EXPIRY|
                                         QSH_ACTION_CFG;

#ifdef FEATURE_QSH_EVENT_METRIC
  qsh_client_metric_cfg_init(onexl3_qsh_metric_default_cfg_arr,
                          ONEXL3_QSH_METRIC_MAX);

 /* Initialize the metrics array with parameters corresponding to each metric 
  * that needs to be collected */
  mc_init_qsh_metric_cfg_array();  
  
  mc_client_reg.metric_info.metric_cfg_count = 
                    ONEXL3_QSH_METRIC_MAX;
  mc_client_reg.metric_info.metric_cfg_arr_ptr = 
                             &onexl3_qsh_metric_default_cfg_arr[0];
#endif /* FEATURE_QSH_EVENT_METRIC */

  qsh_client_reg(&mc_client_reg);
  
  M1X_MSG( DCP, LEGACY_HIGH, 
      "QSH: registered 1X-CP with QSH module" );

} /* mc_qsh_init */

/*===========================================================================

FUNCTION MC_QSH_CB

DESCRIPTION
  Callback function invoked by QSH module for 1X to process QSH requests.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mc_qsh_cb
(
  qsh_client_cb_params_s *cb_params_ptr
)
{

  if( cb_params_ptr->action == QSH_ACTION_METRIC_CFG ||
      cb_params_ptr->action == QSH_ACTION_METRIC_TIMER_EXPIRY ||
      cb_params_ptr->action == QSH_ACTION_CFG
    )
  {
    if(cb_params_ptr->action_mode == QSH_ACTION_MODE_SYNC_REQUIRED)
    {
      /* Called in QSH task context. Should have mutext protection here but
       * it's not feasible to add it for the data structures being 
       * accessed from within this API. SYNC mode should be rare as should
       * write accesses to CDMA structure within this API. So data corruption
       * chances are extremely rare. */
      mc_process_qsh_cmd(cb_params_ptr);
    }
    else
    { 
      if( (cdma.curr_state != CDMA_ENTER ) &&
          (cdma.curr_state != CDMA_EXIT )
        )
      {
        /* Queue the cmd on MC task */
        mc_cdma_qsh_command(cb_params_ptr);
      }
    } /* else */
  } /* if( cb_params_ptr->action == .. */
}/* mc_qsh_cb */

/*===========================================================================

FUNCTION MC_PROCESS_QSH_CMD

DESCRIPTION
  This API processes the cmds received from QSH module. This could either be 
  invoked in QSH context or in MC task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_process_qsh_cmd
(
  qsh_client_cb_params_s* qsh_cmd_ptr
)
{
  qsh_client_action_done_s cb_done_ptr;

  /* Below variables are used only if the feature FEATURE_QSH_EVENT_METRIC
   * is defined. Otherwise it will give compilation error that the variables 
   * are unused. Hence defining under the feature 
   */
#ifdef FEATURE_QSH_EVENT_METRIC 
  uint16 metric_id = ONEXL3_QSH_METRIC_MAX;
  qsh_client_metric_cfg_s *metric_cfg;
  int idx;
#endif /* FEATURE_QSH_EVENT_METRIC */

  switch(qsh_cmd_ptr->action)
  { 
    #ifdef FEATURE_QSH_EVENT_METRIC  
    case QSH_ACTION_METRIC_CFG:
    {
      metric_id = qsh_cmd_ptr->action_params.metric_cfg.id;

      M1X_MSG( DCP, LEGACY_HIGH, 
          "QSH: ACTION_METRIC_CFG metric_id: %d, action: %d", metric_id,
          qsh_cmd_ptr->action_params.metric_cfg.action);
      
      if(metric_id < ONEXL3_QSH_METRIC_MAX)
      {
        metric_cfg = &qsh_cmd_ptr->action_params.metric_cfg;
        
        if(metric_cfg->action == QSH_METRIC_ACTION_START)
        {
          if(metric_cfg->start_addr == NULL)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
                "QSH: Start address passed as NULL!" );
          }
          mc_qsh_data.metric_cfg[metric_id] = *metric_cfg;
          
          /* Handle the specific metric here */
          switch(metric_id)
          {   
            case ONEXL3_QSH_METRIC_CELL_INFO:
            {
              mc_send_cell_info_metrics();
              break;
            }
            case ONEXL3_QSH_METRIC_RF_PARAMS:
            {
              mc_send_rf_params_metrics(QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
              break;
            }
            default:
              break;
          }          
        }
        else if(metric_cfg->action == QSH_METRIC_ACTION_STOP)
        {
          mc_qsh_data.metric_cfg[metric_id].start_addr = NULL ;
          mc_qsh_data.metric_cfg[metric_id].action = QSH_METRIC_ACTION_STOP;
        }    
      }
      break;
    } /* QSH_ACTION_METRIC_CFG */

    case QSH_ACTION_METRIC_TIMER_EXPIRY:
    {                           
      for( idx = 0; idx < qsh_cmd_ptr->action_params.metric_timer_expiry.metric_id_count &&
                  idx < ONEXL3_QSH_METRIC_MAX ; idx++ )
      {
        metric_id = qsh_cmd_ptr->action_params.metric_timer_expiry.params[idx].metric_id;        
        switch(metric_id)
        {
          case ONEXL3_QSH_METRIC_RF_PARAMS:
          {
            mc_send_rf_params_metrics(QSH_CLIENT_METRIC_LOG_REASON_TIMER_EXPIRY);         
            break;
          }
            
          default:
            /*Everything else is event based */
            break;
        }
      }
      break;
    } /* QSH_ACTION_METRIC_TIMER_EXPIRY */
    #endif /* FEATURE_QSH_EVENT_METRIC */

    #ifdef FEATURE_QSH_EVENT_NOTIFY_TO_QSH 
    case QSH_ACTION_CFG:
    {
      /* Current configured ASID */
      sys_modem_as_id_e_type tmp_asid = mcc_get_asid();

      /* cmd_code format: <length,sub_id,event_id,CFG_action>
       * CFG_action: 1st byte from LSB */
      qsh_event_action_e action = 
           (byte)( qsh_cmd_ptr->action_params.cfg.cmd_code & 0x000000FF );

      /* event_id: 2nd byte from LSB */
      onexl3_qsh_event_e event_id = 
        (byte)( (qsh_cmd_ptr->action_params.cfg.cmd_code >> 8) & 0x000000FF );

      /* sub_id: 3rd byte from LSB */
      sys_modem_as_id_e_type sub_id = 
       (byte)( (qsh_cmd_ptr->action_params.cfg.cmd_code >> 16) & 0x000000FF );
      
      /*---------------------------------------------------------------------*/

      M1X_MSG( DCP, LEGACY_MED, 
               "QSH: Event id:%d, action:%d, sub_id:%d, curr_sub_id:%d",
               event_id, 
               action,
               sub_id,
               tmp_asid );

      if(tmp_asid == sub_id)
      {
        if(action == QSH_EVENT_ACTION_ENABLE)
        {
          /* this action controls to enable or disable the event */
          mc_qsh_data.event_action[event_id] = TRUE;
        }  
        else
        {
          mc_qsh_data.event_action[event_id] = FALSE;      
        } 
      } /* if(tmp_asid == sub_id) */
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR, 
                 "QSH: ASID mismatch, ignore QSH_ACTION_CFG cmd" );
      }

      break;
    } /* QSH_ACTION_CFG */
    #endif /* FEATURE_QSH_EVENT_NOTIFY_TO_QSH */
        
    default:
    {
      /* Other actions are not supported currently */ 
      M1X_MSG( DCP, LEGACY_ERROR, 
                "QSH: Unsupported action received: %d",
                qsh_cmd_ptr->action );
      break;
    } /* default */
  } /* switch(qsh_cmd_ptr->action) */

  /* For Metric Timer expiry and Analysis actions, qsh_client_action_done 
   * doesn't need to be called. For other actions it must be called */
  if( qsh_cmd_ptr->action != QSH_ACTION_METRIC_TIMER_EXPIRY &&
      qsh_cmd_ptr->action != QSH_ACTION_ANALYSIS )
  {
    /*  QSH should be intimated that our metric configuration is updated */
    qsh_client_action_done_init(&cb_done_ptr);
    cb_done_ptr.cb_params_ptr = qsh_cmd_ptr;
    if(qsh_cmd_ptr->action_mode == QSH_ACTION_MODE_SYNC_REQUIRED)
    {
      cb_done_ptr.action_mode_done = QSH_ACTION_MODE_DONE_SYNC;
    }
    else
    {
      cb_done_ptr.action_mode_done = QSH_ACTION_MODE_DONE_ASYNC;
    }
    qsh_client_action_done(&cb_done_ptr);
  }
} /* mc_process_qsh_cmd() */

/*===========================================================================

FUNCTION MC_SEND_QSH_EVENT

DESCRIPTION
  Common API Handler function for sending an event to QSH framework and this API is 
  called from CP, SRCH, MUX.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_qsh_event
(
  onexl3_qsh_event_e onex_event
)
{
  #ifdef FEATURE_QSH_EVENT_NOTIFY_TO_QSH
  qsh_client_event_notify_params_s event_notify_params;
  /*-----------------------------------------------------------------------*/


  /* Send event to QSH only if the event is enabled */
  if( (onex_event < ONEXL3_QSH_EVENT_MAX) &&
      mc_qsh_is_evt_enabled(onex_event)
    )
  {
    /* initialize event struct */  
    qsh_client_event_notify_init(&event_notify_params);
    
    event_notify_params.client = QSH_CLT_ONEXL3;  
    event_notify_params.subs_id = mc_qsh_get_asid();      
    event_notify_params.event_data = NULL;

    
    /* If Generic Event is enabled then the expectation is by default 
     * we need to send all the failure events to QSH and 
     * QSH event id should be Generic Event Id
     * Otherwise, send failure event to QSH with that particular Event Id */
    if(mc_qsh_data.event_action[ONEXL3_QSH_EVENT_FIELD_DEBUG_ANY_FAILURE] 
                                                                 == TRUE)
    {
      event_notify_params.id = ONEXL3_QSH_EVENT_FIELD_DEBUG_ANY_FAILURE;
      
      M1X_MSG( DCP, LEGACY_MED, 
               "QSH: Sending ONEXL3_QSH_EVENT_FIELD_DEBUG_ANY_FAILURE, "
               "QSH_Event_Id:%d, AS_ID:%d",
               onex_event,
               event_notify_params.subs_id );      
    }
    else
    {
      event_notify_params.id = onex_event;  

      M1X_MSG( DCP, LEGACY_MED, 
               "QSH: Sending QSH_Event_Id:%d, AS_ID:%d", 
               onex_event,
               event_notify_params.subs_id );
    }
           
    /* Log the Event through the QSH log */
    switch(onex_event)
    {
      /**************************************
       *** SRCH-L1 related QSH Event Types **
       **************************************/
      case ONEXL3_QSH_EVENT_L1_TIMING_ERR:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_TIMING_ERR, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_TIMING_ERR */

      case ONEXL3_QSH_EVENT_L1_TUNE_FREQ_ERR:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_TUNE_FREQ_ERR, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_TUNE_FREQ_ERR */
      
      case ONEXL3_QSH_EVENT_L1_PAGE_MISS:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_PAGE_MISS, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_PAGE_MISS */

      case ONEXL3_QSH_EVENT_L1_ASDIV_SWITCH:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_ASDIV_SWITCH, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_ASDIV_SWITCH */

      case ONEXL3_QSH_EVENT_L1_EMPTY_QTA:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_EMPTY_QTA, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_EMPTY_QTA */

      case ONEXL3_QSH_EVENT_L1_TRM_LOCK_WAIT_TIMEOUT:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L1_TRM_LOCK_WAIT_TIMEOUT, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L1_TRM_LOCK_WAIT_TIMEOUT */

      
      /**************************************
       *** MUX-L2 related QSH Event Types ***
       **************************************/   
      case ONEXL3_QSH_EVENT_L2_PAGE_DEC_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L2_PAGE_DEC_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L2_PAGE_DEC_FAIL */

      case ONEXL3_QSH_EVENT_L2_HIGH_RXAGC_DELTA:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L2_HIGH_RXAGC_DELTA, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L2_HIGH_RXAGC_DELTA */

      
      /**************************************
       *** CP-L3 related QSH Event Types ****
       **************************************/ 
      case ONEXL3_QSH_EVENT_L3_OOS:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_OOS, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_OOS */

      case ONEXL3_QSH_EVENT_L3_RLF:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_RLF, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_RLF */

      case ONEXL3_QSH_EVENT_L3_ACCESS_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_ACCESS_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_ACCESS_FAIL */   

      case ONEXL3_QSH_EVENT_L3_INTER_FREQ_HO_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_INTER_FREQ_HO_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_INTER_FREQ_HO_FAIL */

      case ONEXL3_QSH_EVENT_L3_ORIG_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_ORIG_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_ORIG_FAIL */

      case ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL */      

      case ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL */ 

      case ONEXL3_QSH_EVENT_L3_1X2L_RESEL_FAIL:
      {
        QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
                "QSH:1x ONEXL3_QSH_EVENT_L3_1X2L_RESEL_FAIL, Asid:%d",
                event_notify_params.subs_id);
        break;
      } /* ONEXL3_QSH_EVENT_L3_1X2L_RESEL_FAIL */ 
      
      default:
      {
        M1X_MSG( DCP, LEGACY_ERROR, 
                 "QSH: Unsupported event: %d",
                 onex_event );
      } /* default */
    } /* switch(onex_event) */

    /* Notify event to QSH */
    qsh_client_event_notify(&event_notify_params);
  } /* if(onex_event) */
  else
  {
    M1X_MSG( DCP, LEGACY_LOW, 
             "QSH: event:%d not Enabled", 
             onex_event );
  }
  #endif /* FEATURE_QSH_EVENT_NOTIFY_TO_QSH */
} /* mc_send_qsh_event() */

/*===========================================================================

FUNCTION MC_SEND_CELL_INFO_METRICS

DESCRIPTION
  This API would be used by 1X-CP to send cell info metrics to QSH module. 
  This metric is reported whenever there is a 1X cell change after SPM 
  is available on the new cell.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_cell_info_metrics
(
  void
)
{
  onexl3_qsh_metric_cell_info_s *metrics_ptr;
  
  qsh_client_metric_cfg_s *metric_cfg = &(mc_qsh_data.metric_cfg[ONEXL3_QSH_METRIC_CELL_INFO]);
    
  if(mc_qsh_is_metric_enabled(ONEXL3_QSH_METRIC_CELL_INFO) == FALSE)
  {
    M1X_MSG( DCP, LEGACY_MED, 
             "Cell info metrics disabled, skip sending to QSH");
    return;
  }
  
  metrics_ptr = (onexl3_qsh_metric_cell_info_s *)metric_cfg->start_addr;

  M1X_MSG( DCP, LEGACY_MED, 
           "QSH: Sending Cell info metrics, start addr: 0x%x, size %d, req size %d", 
           metrics_ptr, metric_cfg->size_bytes, 
           sizeof(onexl3_qsh_metric_cell_info_s));
    
  if ( metrics_ptr != NULL &&
       metric_cfg->size_bytes >= sizeof(onexl3_qsh_metric_cell_info_s))
  {

    if (mcc_update_cell_info_metrics(metrics_ptr))
    {
      M1X_MSG( DCP, LEGACY_HIGH, 
          "QSH: Sent Cell info to QSH, pilot %d, reg_zone %d", 
          metrics_ptr->pilot_pn, metrics_ptr->reg_zone);    
      
      mc_qsh_commit_data( ONEXL3_QSH_METRIC_CELL_INFO, 
                  QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL, metric_cfg );  
    }
  }
} /* mc_send_cell_info_metrics */

/*===========================================================================

FUNCTION MCC_UPDATE_CELL_INFO_METRICS

DESCRIPTION
  This API updates the cell_info metrics structure with the current BS info 
  to be reported to QSH module on BS change.

DEPENDENCIES
  cur_bs_ptr should not be Null and SPM should have been received, otherwise
  garbage values may be populated in the structure.

RETURN VALUE
  True if metrics are available, False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_update_cell_info_metrics
(
  onexl3_qsh_metric_cell_info_s *metrics_ptr
)
{
  switch(MAIN_STATE( cdma.curr_state ))
  {    

    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    /* Id we are in ENTER/EXIT state while this API is called then this API is 
     * called while processing sib8 message and use cell info from sib8 */
    case CDMA_CSFB:
    case CDMA_ENTER:
    case CDMA_EXIT:
    {
      return mcc_csfb_update_cell_info_metrics(metrics_ptr);
    } 
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    
    default:
    {
      if (cur_bs_ptr != NULL)
      {
        metrics_ptr->band_class = cdma.band_class;
        metrics_ptr->cfg_msg_seq = cur_bs_ptr->rx.config_msg_seq;
        metrics_ptr->freq = cdma.cdmach;
        metrics_ptr->pilot_pn = cur_bs_ptr->pilot_pn;
        metrics_ptr->base_id = cur_bs_ptr->csp.sp.base_id;
        metrics_ptr->reg_zone = cur_bs_ptr->csp.sp.reg_zone;
        metrics_ptr->sid = cur_bs_ptr->csp.sp.sid;
        metrics_ptr->nid = cur_bs_ptr->csp.sp.nid;
        return TRUE;
      }      
      break;
    }
  }

  return FALSE;  
  
} /* mcc_update_cell_info_metrics */

/*===========================================================================

FUNCTION MC_SEND_RF_PARAMS_METRICS

DESCRIPTION
  This API would be used by 1X-CP to send RF params metrics to QSH module. 
  This metric is reported periodically and the required parameters are fetched
  by 1X-CP from 1X-L1 (Searcher) module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_send_rf_params_metrics
(
  qsh_client_metric_log_reason_e reason
)
{
  onexl3_qsh_metric_rf_params_s *metrics_ptr;
  
  qsh_client_metric_cfg_s *metric_cfg = &(mc_qsh_data.metric_cfg[ONEXL3_QSH_METRIC_RF_PARAMS]);
  
  if(mc_qsh_is_metric_enabled(ONEXL3_QSH_METRIC_RF_PARAMS) == FALSE)
  {
    return;
  }
  
  metrics_ptr = (onexl3_qsh_metric_rf_params_s *)metric_cfg->start_addr;
  
  if ( metrics_ptr != NULL &&
       metric_cfg->size_bytes >= sizeof(onexl3_qsh_metric_rf_params_s))
  {
    if(mcc_update_rf_params_metrics(metrics_ptr))
    {          
      mc_qsh_commit_data( ONEXL3_QSH_METRIC_RF_PARAMS, 
                          reason, metric_cfg );
    }
  }
} /* mc_send_rf_params_metrics */

/*===========================================================================

FUNCTION MCC_UPDATE_RF_PARAMS_METRICS

DESCRIPTION
  This API updates the RF params metrics structure with the RF params 
  of the current BS to be reported to QSH module periodically.

DEPENDENCIES
  None.

RETURN VALUE
  True if metrics are available, False otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_update_rf_params_metrics
(
  onexl3_qsh_metric_rf_params_s *metrics_ptr
)
{   
  byte rssi0, rssi1, rssi_comb, ecio0, ecio1, ecio_comb;
  int16 tmp_tx_pwr;

  /* This API returns RSSI in db units, returns 125 if RSSI value is default */
  srch_get_cdma_rssi_all_chains( &rssi_comb, &rssi0, &rssi1 );
    
  /* The above SRCH APIs returned the absolute values in 1/2 dB units and we 
   * need to convert it to dB units before passing to QSH. Returns 63 if ECIO 
   * value is default */  
  srch_get_cdma_raw_ecio_all_chains( &ecio_comb, &ecio0, &ecio1 );  

  /* Tx API returns tx power in 1/10 dB units and returns -32768 for TX OFF */
  tmp_tx_pwr = txc_get_tx_agc_dbm10();  
  if (tmp_tx_pwr != -32768)
  {
    tmp_tx_pwr = tmp_tx_pwr/10;
    
    /* If we are in traffic state store the last known valid TX power value */
    if(MAIN_STATE(cdma.curr_state) == CDMA_TC)
    {
      valid_tx_power = tmp_tx_pwr;
    }
  }
  else
  {
    if(MAIN_STATE(cdma.curr_state) == CDMA_TC)
    {
      /* Report the last known valid TX power in traffic state */
      tmp_tx_pwr = valid_tx_power;
    }
    else
    {
      /* Since we are not in Traffic state any more reset 
       * the value of last known valid TX power in traffic state */
      valid_tx_power = tmp_tx_pwr;
    }
  }
  
  /* Store the values after doing required unit conversions. All the
   * below RF metrics are being provided as signed values in dB units.
   * For ECIO value of -63 and for RSSI value of -125 indicates 
   * no energy detected. For TX power -32768 indicates TX is off. */
  metrics_ptr->rssi0 = -rssi0;
  metrics_ptr->rssi1 = -rssi1;
  metrics_ptr->rssi_comb = -rssi_comb;
  metrics_ptr->ecio0 = (ecio0 == 63) ? -ecio0 : -ecio0/2;
  metrics_ptr->ecio1 = (ecio1 == 63) ? -ecio1 : -ecio1/2;
  metrics_ptr->ecio_comb = (ecio_comb == 63) ? -ecio_comb : -ecio_comb/2;
  metrics_ptr->tx_pwr = tmp_tx_pwr;  
  metrics_ptr->l1_state = (uint32) srch_get_state();
  
  return TRUE;

} /* mcc_update_rf_params_metrics */

/*===========================================================================

FUNCTION MC_QSH_COMMIT_DATA

DESCRIPTION
  This API is used by 1X-CP to send metrics info to QSH module. The metrics 
  could be sent due to metric timer expiry or due to 1X internal event such 
  as BS change.
  Metrics sent through this API will also ensure that write address for this
  metric would move to the next index in that metric's FIFO thus ensuring that
  next instance of this metric wouldn't overwrite the previous one.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_qsh_commit_data
(
  uint8 metric_id,
  qsh_client_metric_log_reason_e reason,
  qsh_client_metric_cfg_s *metric_cfg
)
{  
  qsh_client_metric_log_done_s cb_done;

  qsh_client_metric_log_done_init(&cb_done);
  
  cb_done.client = QSH_CLT_ONEXL3;
  cb_done.metric_id = metric_id;
  cb_done.metric_context_id = metric_cfg->metric_context_id;
  cb_done.log_reason = reason; 
  metric_cfg->start_addr = qsh_client_metric_log_done(&cb_done);
  
} /* mc_qsh_commit_data */

/*===========================================================================

FUNCTION MC_INIT_QSH_METRIC_CFG_ARRAY

DESCRIPTION
  To configure the 1X_CP array for metrics conllection before passing it to QSH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_init_qsh_metric_cfg_array
(
  void
)
{
  qsh_ext_metric_cfg_s *metric_cfg_ptr;
  sys_modem_as_id_e_type cur_asid = SYS_MODEM_AS_ID_1;

  metric_cfg_ptr = &(onexl3_qsh_metric_default_cfg_arr[ONEXL3_QSH_METRIC_CELL_INFO]);

  metric_cfg_ptr->id = ONEXL3_QSH_METRIC_CELL_INFO;
  metric_cfg_ptr->subs_id = cur_asid;
  metric_cfg_ptr->fifo.element_size_bytes = sizeof(onexl3_qsh_metric_cell_info_s);
  metric_cfg_ptr->fifo.element_count_total = MC_QSH_METRIC_CELL_INFO_COUNT;
  metric_cfg_ptr->sampling_period_ms = 0;  /* Indicates that this metric is not periodic */

  metric_cfg_ptr = &(onexl3_qsh_metric_default_cfg_arr[ONEXL3_QSH_METRIC_RF_PARAMS]);

  metric_cfg_ptr->id = ONEXL3_QSH_METRIC_RF_PARAMS;
  metric_cfg_ptr->subs_id = cur_asid;
  metric_cfg_ptr->fifo.element_size_bytes = sizeof(onexl3_qsh_metric_rf_params_s);
  metric_cfg_ptr->fifo.element_count_total = MC_QSH_METRIC_RF_PARAMS_COUNT;
  metric_cfg_ptr->sampling_period_ms = MC_QSH_METRIC_PERIODICITY;  /* This metric is periodic */  
  
} /* mc_init_qsh_metric_cfg_array */

/*===========================================================================

FUNCTION MC_QSH_IS_METRIC_ENABLED

DESCRIPTION
  Checks if a particular 1X_CP metric is enabled by QSH module or not.

DEPENDENCIES
  None.

RETURN VALUE
  True, if metric is enabled.
  False, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mc_qsh_is_metric_enabled
(
  onexl3_qsh_metric_e metric_id
)
{
  boolean ret = FALSE;

  if(mc_qsh_data.metric_cfg[metric_id].action == QSH_METRIC_ACTION_START)
  {
    ret = TRUE;
  }
  return ret;  
} /* mc_qsh_is_metric_enabled */

/*===========================================================================

FUNCTION MC_QSH_IS_EVT_ENABLED

DESCRIPTION
  Checks if a particular 1X event is enabled by QSH module or not.

DEPENDENCIES
  None.

RETURN VALUE
  True, if Particular Event/Generic Event is enabled.
  False, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mc_qsh_is_evt_enabled
(
  onexl3_qsh_event_e evt
)
{
  boolean ret = FALSE;

  /* Return TRUE if either Generic Event/Particular Event is enabled 
   * If Generic Evnet is enabled then the expectation is we need to report
   * all the failure events to QSH by default */
  if( (mc_qsh_data.event_action[ONEXL3_QSH_EVENT_FIELD_DEBUG_ANY_FAILURE] 
                                                                 == TRUE) ||
      (mc_qsh_data.event_action[evt] == TRUE)
    )
  {
    ret = TRUE;
  }
  return ret;
} /* mc_qsh_is_evt_enabled */

/*===========================================================================

FUNCTION MC_QSH_GET_ASID

DESCRIPTION

  This API returns the ASID value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the ASID value
  this API will default the ASID to SYS_MODEM_AS_ID_1

DEPENDENCIES
  None.

RETURN VALUE
  ASID value

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type mc_qsh_get_asid
(  
  void
)
{
  sys_modem_as_id_e_type cur_asid = mcc_get_asid();
  
  if (cur_asid == SYS_MODEM_AS_ID_NONE)
  {
    cur_asid = SYS_MODEM_AS_ID_1;
    M1X_MSG( DCP,  LEGACY_HIGH, 
             "1X ASID is None, defaulting to ASID 1");    
  }
  return cur_asid;
  
} /* mc_qsh_get_asid */
#endif /* FEATURE_1XCP_QSH_SUPPORT */
