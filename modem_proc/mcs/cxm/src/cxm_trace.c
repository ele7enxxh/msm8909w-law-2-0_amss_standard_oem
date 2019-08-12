/*!
  @file
  cxm_trace.c

  @brief
  Implementation for CXM's debug trace logging

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_trace.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states
08/14/14   cdh     Diag log packet functionality added
09/30/13   tak     Counters for advanced TX toggling and LTE ML1 late message
12/19/12   btl     Initial Port to Dime
11/05/12   ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <timer.h>
#include <timetick.h>
#include <stringl/stringl.h> /* memeset */
#include "cxm_trace.h"
#include "cxm_utils.h"       /* CXM_ASSERT */
#include "cxm_intf_types.h"  /*FW counters struct*/

#include "modem_fw_memmap.h" /*FW counters addr*/
#include "coex_algos.h"      /* Access internal CXM data */
#include "log_codes.h"       /* codes for log packet system */
#include "cxm.h"
#include "cxm_smem.h"        /* Access internal CXM smem data */
#include "rex.h"
#include "wcn_coex_mgr.h"
#include "DDITimetick.h"
#include "cxm_antenna.h"     /* Access internal CXM antenna state */
#include "cxm_smem.h"

/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/
#define CXM_TRACE_VERSION_NUM   2

/* Instant of CXM's trace buffer */
STATIC cxm_trace_event_s cxm_trace_buffer;

/* CXM counters. Use cxm_counters_e enum as index into the array */
STATIC cxm_counter_type_s cxm_counters[ CXM_CNT_MAX_COUNTERS ];

/* Counter for FW events */
STATIC cxm_fw_smem_s *cxm_fw_counters;

/* Internal CXM logs */
STATIC cxm_low_prio_log_data_type  cxm_low_prio_log;
STATIC cxm_med_prio_log_data_type  cxm_med_prio_log;
STATIC cxm_high_prio_log_data_type cxm_high_prio_log;
STATIC cxm_smem_log_data_type      cxm_smem_log;

/* Timer to flush log packets periodically */
STATIC timer_type cxm_log_pkt_timer;

/* LUTs of pointers to CXM's internal STATIC data */
STATIC void *cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_MAX_LOW_PRIO)];
STATIC void *cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_MAX_MED_PRIO)];
STATIC void *cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_MAX_HIGH_PRIO)];
/* All smem logging uses these pointers -- no LUT required */
STATIC cxm_smem_data_s **cxm_log_smem_data_p = NULL;
STATIC cxm_smem_tech_fw_activity_s *cxm_log_fw_act_p;

/* booleans to track which log codes are enabled */
STATIC boolean cxm_low_prio_log_en = FALSE;
STATIC boolean cxm_med_prio_log_en = FALSE;
STATIC boolean cxm_high_prio_log_en = FALSE;
STATIC boolean cxm_smem_log_en = FALSE;
STATIC boolean cxm_one_or_more_logs_en = FALSE;

/* Access coex_params for log packets, externally declared in coex_algos.c */
extern coex_config_params_v8 coex_params;
/* Access coex_global_policy_params for log packets */
extern coex_plcy_parms_s coex_plcy_parms;

/*! @brief CXM task rex control block 
 * On target this declaration resides in service/task/task.c */
extern rex_tcb_type cxm_tcb;

/* Stores logging data that comes through msgr */
STATIC cxm_coex_log_async_ind_s  cxm_async_logs;

/* handle used for timetick services */
DalDeviceHandle *cxm_tt_handle;

/*===========================================================================

  FUNCTION:  cxm_trace_init

===========================================================================*/
/*!
  @brief
    To initialize CXM's debug trace/event logging interface

  @return
    None
*/
/*=========================================================================*/
void cxm_trace_init (
  void
)
{
  coex_state_info_type *coex_state_info_p;
  coex_wwan_tech_info *coex_wwan_tech_info_p;
  cxm_antenna_state_s *cxm_antenna_state_p;
  /*-----------------------------------------------------------------------*/

  /* Initialize the trace buffer */
  memset( &cxm_trace_buffer, 0, sizeof( cxm_trace_event_s ) );

  /* Set index to zero (begining of the trace buffer) */
  atomic_set( &(cxm_trace_buffer.seq_num), 0 );
  cxm_trace_buffer.version_number = CXM_TRACE_VERSION_NUM;

  /* Initialize the counter buffer */
  memset( &cxm_counters, 0, sizeof( cxm_counters ) );

  /*Initialize FW counters. We already memset the smem in algos_init*/
  cxm_fw_counters = (cxm_fw_smem_s*) FW_SMEM_COEX_ADDR; 

  /* Initialize cxm logs */
  memset( &cxm_low_prio_log, 0, sizeof(cxm_low_prio_log) );
  memset( &cxm_med_prio_log, 0, sizeof(cxm_med_prio_log) );
  memset( &cxm_high_prio_log, 0, sizeof(cxm_high_prio_log) );
  memset( &cxm_smem_log, 0, sizeof(cxm_smem_log) );

  /* Get pointers to CXM internal data */
  coex_state_info_p = coex_get_state_info_ptr();
  coex_wwan_tech_info_p = coex_get_wwan_state_info_ptr();
  cxm_antenna_state_p = cxm_get_antenna_state_ptr();
  cxm_log_smem_data_p = cxm_get_smem_data_ptr();
  cxm_log_fw_act_p =  cxm_get_smem_fw_act_ptr();
  
  /* Populate look up tables */
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)] = coex_state_info_p->state;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_CONTROLLER_PWR_LMT_STATE)] = 
    coex_state_info_p->cont_pwr_lmt_state;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCI2_PWR_LMT_STATE)] =
    coex_state_info_p->wci2_pwr_lmt_state;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_BLER_STARTED)] = 
    coex_state_info_p->metrics_bler_started;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_SINR_STARTED)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_LTE].metrics_started);
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM1_SINR_STARTED)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM1].metrics_started);
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM2_SINR_STARTED)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM2].metrics_started);
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM3_SINR_STARTED)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM3].metrics_started);
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED)] =
    &(coex_wwan_tech_info_p[CXM_TECH_TDSCDMA].metrics_started);
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)] = (void *)cxm_antenna_state_p;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)] = (void *)cxm_wcn_get_info_ptr();
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_LTE_TX_ADV)] = coex_state_info_p->log_lte_tx_adv;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_TECH_METRICS_READ)] = coex_state_info_p->log_tech_metrics_read;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_LTE_BLER_READ)] = coex_state_info_p->log_lte_bler_read;
  cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_HIGH_PRIO_INFO)] = coex_state_info_p->log_high_prio_info;
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_LTE]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_TDSCDMA]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM1]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_ONEX]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_HDR]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_WCDMA]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM2]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)] =
    &(coex_wwan_tech_info_p[CXM_TECH_GSM3]);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)] =
    (void *)(&coex_plcy_parms);
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_MDM_STATE)] = 
    coex_state_info_p->wci2_t7_info;
  cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCI2_DATA_RECV)] =
    coex_state_info_p->recv_wci2_data;
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_LTE]);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_GSM1]);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_GSM2]);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_GSM3]);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)] = 
    &(coex_wwan_tech_info_p[CXM_TECH_TDSCDMA]);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)] = 
    coex_state_info_p->band_filter_info;
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)] = (void *)(&coex_params);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)] = (void *)(&coex_params);
  cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)] = 
    (void *)(&(coex_params.port_table));

  /* Define log packet timer */
  timer_def( &cxm_log_pkt_timer, NULL, &cxm_tcb, CXM_LOG_PKT_FLUSH_SIG, 
             NULL, 0 );

  cxm_low_prio_log_en = log_status( LOG_CXM_LOW_PRIO_C );
  cxm_med_prio_log_en = log_status( LOG_CXM_MED_PRIO_C );
  cxm_high_prio_log_en = log_status( LOG_CXM_HIGH_PRIO_C );
  cxm_smem_log_en = log_status( LOG_CXM_SMEM_DATA_C );
  cxm_one_or_more_logs_en = cxm_low_prio_log_en || cxm_med_prio_log_en ||
    cxm_high_prio_log_en || cxm_smem_log_en;

  /* If at least one log is enabled, start the timer */
  if( cxm_one_or_more_logs_en )
  {
    /* Periodically expire every 20 seconds */
    timer_set( &cxm_log_pkt_timer, 20, 20, T_SEC );
  }

  /* handle to get 64-bit timetick for logging */
  if( ( DalTimetick_Attach("SystemTimer", &cxm_tt_handle) 
        != DAL_SUCCESS ) || ( cxm_tt_handle == NULL ) )
  {
    ERR_FATAL("Cannot attach to timetick driver!", 0, 0, 0); 
  }

  /*-----------------------------------------------------------------------*/

  return;
} /* cxm_trace_init */

/*===========================================================================

  FUNCTION:  cxm_log_send_msgr

===========================================================================*/
/*!
  @brief
    To send an async log event through msgr

  @return
    None
*/
/*=========================================================================*/
void cxm_log_send_msgr
(
  cxm_trace_event_types_e event,     /*!< CXM event type */
  uint64                  ts,        /*!< Time of event */
  va_list                 args_list, /*!< List of log codes */
  uint8                   nargs      /*!< Number of variable args*/
)
{
  uint8                    i;           /* loop counter */
  uint32                   pyld_e;      /* local for holding payload enum */
  cxm_coex_log_async_ind_s msgr_msg;    /* for sending logs to CXM context */
  cxm_wcn_tx_client_e      wcn_tx_clnt;

  /*--------------------------------------------------------------------------*/

  if ( !(cxm_low_prio_log_en || cxm_med_prio_log_en ||
       cxm_high_prio_log_en || cxm_smem_log_en) )
  {
    return;
  }
  memset( &msgr_msg, 0, sizeof( cxm_coex_log_async_ind_s ) );
  msgr_msg.event = event;
  msgr_msg.ts = ts;
  /* perform logging async */
  for( i = 0; i < nargs - 1 && i < COEX_MAX_ASYNC_LOGS; i++ )
  {
    /* Retrieve the payload enumerated value from variable args list */
    pyld_e = va_arg( args_list, int );

    msgr_msg.data[i].log_id = pyld_e;
    switch (pyld_e)
    {
      case CXM_LOG_SMEM_TDS_FW_DATA_V2:
        msgr_msg.data[i].payld.fw_activity.data = 
              (*((cxm_smem_data_s **)cxm_log_smem_data_p))-> \
              cxm_smem_tds_fw[cxm_log_fw_act_p->act[CXM_TECH_TDSCDMA].next_entry].data;
        break;
      case CXM_LOG_SMEM_GSM1_FW_DATA_V2:
        msgr_msg.data[i].payld.fw_activity.data = 
              (*((cxm_smem_data_s **)cxm_log_smem_data_p))-> \
              cxm_smem_gsm1_fw[cxm_log_fw_act_p->act[CXM_TECH_GSM1].next_entry].data; 
        break;
      case CXM_LOG_SMEM_GSM2_FW_DATA_V2:
        msgr_msg.data[i].payld.fw_activity.data = 
              (*((cxm_smem_data_s **)cxm_log_smem_data_p))-> \
              cxm_smem_gsm2_fw[cxm_log_fw_act_p->act[CXM_TECH_GSM2].next_entry].data;
        break;
      case CXM_LOG_SMEM_GSM3_FW_DATA_V2:
          msgr_msg.data[i].payld.fw_activity.data = 
            (*((cxm_smem_data_s **)cxm_log_smem_data_p))-> \
            cxm_smem_gsm3_fw[cxm_log_fw_act_p->act[CXM_TECH_GSM3].next_entry].data;
        break;
      case CXM_LOG_WCN_TX_STATE:
        wcn_tx_clnt = ((cxm_wcn_tx_info_s *)
           (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)]))->last_clnt;
        if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)] == NULL ||
            wcn_tx_clnt >= CXM_WCN_TX_CLIENT_MAX )
        {
          CXM_MSG_0( MED, "log lut null pointer or invalid wcn_tx_client" );
          return;
        }
        msgr_msg.data[i].payld.wcn_tx.clnt = wcn_tx_clnt;
        msgr_msg.data[i].payld.wcn_tx.status = 
          ((cxm_wcn_tx_info_s *)
           (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)]))->clnt[wcn_tx_clnt].state;
        msgr_msg.data[i].payld.wcn_tx.last_rep_tx = 
          ((cxm_wcn_tx_info_s *)
           (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)]))->clnt[wcn_tx_clnt].last_rep_tx;
        msgr_msg.data[i].payld.wcn_tx.last_rsp_code = 
          ((cxm_wcn_tx_info_s *)
           (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCN_TX_STATE)]))->clnt[wcn_tx_clnt].last_rsp_code;
        break;
      default:
        CXM_MSG_1( ERROR, "Unable to log pkt async log_id = %d",  pyld_e);
        return;
        break;
    }
  } 
  
  
  msgr_msg.num_logs = i;
  cxm_msgr_send_msg( &msgr_msg.msg_hdr, 
                 MCS_CXM_COEX_LOG_ASYNC_IND, 
                 sizeof(cxm_coex_log_async_ind_s) );
   
  return;
}

/*===========================================================================

  FUNCTION:  cxm_log_initiate_pkt

===========================================================================*/
/*!
  @brief
    Initiate the log packet entry

  @return
    None
*/
/*=========================================================================*/
void cxm_log_initiate_pkt
(
  cxm_trace_event_types_e event,     /*!< CXM event type */
  uint64                  ts,        /*!< Time of event */
  va_list                 args_list, /*!< List of log codes */
  uint8                   nargs      /*!< Number of variable args*/
)
{
  uint8                    i;           /* loop counters */
  uint32                   pyld_e;      /* local for holding payload enum */
  uint8                    n_pkt;       /* local packet count */

  /*--------------------------------------------------------------------------*/

  /* Warn user if log pkt enabled without specified structures */
  if( nargs == 1 )
  {
    CXM_MSG_0( ERROR, "Log pkt struct enum missing" );
  }
  
  /* Log as many structures as desired up to 15 */
  for( i = 0; i < nargs - 1; ++i )
  {
    /* Retrieve the payload enumerated value from variable args list */
    pyld_e = va_arg( args_list, int );

    if( CXM_LOG_LP( pyld_e ) )
    {
      if( !cxm_low_prio_log_en )
      {
        continue;
      }
      n_pkt = cxm_low_prio_log.log_struct.num_packets; 
      cxm_low_prio_log.log_struct.log_data[n_pkt].low_prio_type_id = pyld_e;
      cxm_low_prio_log.log_struct.log_data[n_pkt].ts = ts;
      cxm_low_prio_log.log_struct.log_data[n_pkt].event = (uint8)event;
    }
    else if( CXM_LOG_MP( pyld_e ) )
    {
      if( !cxm_med_prio_log_en )
      {
        continue;
      }
      n_pkt = cxm_med_prio_log.log_struct.num_packets;
      cxm_med_prio_log.log_struct.log_data[n_pkt].med_prio_type_id = pyld_e;
      cxm_med_prio_log.log_struct.log_data[n_pkt].ts = ts;
      cxm_med_prio_log.log_struct.log_data[n_pkt].event = (uint8)event;
    }
    else if( CXM_LOG_HP( pyld_e ) )
    {
      if( !cxm_high_prio_log_en )
      {
        continue;
      }
      n_pkt = cxm_high_prio_log.log_struct.num_packets;
      cxm_high_prio_log.log_struct.log_data[n_pkt].high_prio_type_id = pyld_e;
      cxm_high_prio_log.log_struct.log_data[n_pkt].ts = ts;
      cxm_high_prio_log.log_struct.log_data[n_pkt].event = (uint8)event;
    }
    else if( CXM_LOG_SM( pyld_e ) )
    {
      if( !cxm_smem_log_en )
      {
        continue;
      }
      n_pkt = cxm_smem_log.log_struct.num_packets;
      cxm_smem_log.log_struct.log_data[n_pkt].smem_data_type_id = pyld_e;
      cxm_smem_log.log_struct.log_data[n_pkt].ts = ts;
      cxm_smem_log.log_struct.log_data[n_pkt].event = (uint8)event;
    }
    else 
    {
      CXM_MSG_0( ERROR, "Unsupported log packet payload" );
      return;
    }

    /* Finally, log the data */
    cxm_log_new_data( pyld_e );
  }

  return;
}


/*===========================================================================

  FUNCTION:  cxm_trace_event

===========================================================================*/
/*!
  @brief
    To log CXM event into the trace buffer

  @return
    None
*/
/*=========================================================================*/
void cxm_trace_event
(
  cxm_trace_event_types_e event,   /*!< CXM event type */
  uint8                   a8,    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16                  b16,   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32                  c32,   /*!< 3rd of 3 opaque payloads (32-bit) */
  uint8                   nargs, /*!< Parameter for number of variable args*/
  ...  /*!< Variable argument list including MANDATORY "flag" parameter */
  /*!< uint32             flag,     log packetting and trace enable flag */
  /*!< uint32             pyld_e1   first optional log pkt payload enum */
  /*!< ...                ...       ...                                 */
  /*!< uint32             pyld_e15  final optional log pkt patload enum */
)
{
  cxm_trace_event_log_s   *log_p;       /* pointer to entry in table */ 
  atomic_plain_word_t      my_seq;      /* local seq # */
  uint32                   tbl_indx;    /* table index */
  uint32                   flag;        /* local trc and log pkt flag */
  va_list                  args_list;   /* holds list of variable args */
  uint64                   ts;          /* timestamp for the trace/log */

  /*--------------------------------------------------------------------------*/

  /* Check for proper number of arguments */
  if( nargs > 16 || nargs < 1 )
  {
    CXM_MSG_0( HIGH, "Improper usage of CXM_TRACE suspected" );
    return;
  }
  /* initialize variable argument list */
  va_start( args_list, nargs );

  /* first additional arg should be trc and log pkt flag */
  flag = va_arg( args_list, int );

  /* get the timestamp for this trace/log */
  if (DAL_SUCCESS != DalTimetick_GetTimetick64( 
                     cxm_tt_handle, (DalTimetickTime64Type *) &ts ))
  {
    CXM_MSG_0( ERROR, "unable to get timetick");
  }

  if( flag & CXM_TRC_EN )
  {
    /* do an atomic increment of the sequence # and get the result */
    my_seq = atomic_inc_return( &(cxm_trace_buffer.seq_num) );

    /* decrement the result since we get the update value -
       this is our reserved sequence # */
    my_seq -= 1;

    /* mod the index to fit in the size of the table - keep it a power of 2! */
    tbl_indx = my_seq % CXM_TRACE_NUM_OF_EVENTS;

    /* Get a pointer to the event structure, for convinence */
    log_p = &cxm_trace_buffer.events[ tbl_indx ];

    /* Record the event */
    log_p->event = (uint8)event;
    log_p->a8    = a8;
    log_p->b16   = b16;
    log_p->c32   = c32;
    log_p->ts    = ts;
  }
  
  if ( flag & CXM_LOG_PKT_EN )
  {
    /* Update log code enabled status */
    cxm_low_prio_log_en = log_status( LOG_CXM_LOW_PRIO_C );
    cxm_med_prio_log_en = log_status( LOG_CXM_MED_PRIO_C );
    cxm_high_prio_log_en = log_status( LOG_CXM_HIGH_PRIO_C );
    cxm_smem_log_en = log_status( LOG_CXM_SMEM_DATA_C );

    /* check what task is running so we know whether to handle logging async */
    if ( rex_self() != &cxm_tcb )
    {
      cxm_log_send_msgr( event, ts, args_list, nargs );
    }
    else
    {
      /* if we are logging from msgr msg, use the in-message timestamp instead */
      if ( flag & CXM_ASYNC_PKT_EN )
      {
        ts = cxm_async_logs.ts;
      }
      /* If none were enabled, and now at least one is enabled,
         start the periodic flush timer */
      if( !cxm_one_or_more_logs_en &&
          (cxm_low_prio_log_en || cxm_med_prio_log_en ||
           cxm_high_prio_log_en || cxm_smem_log_en) )
      {
        /* Periodically expire every 20 seconds */
        timer_set( &cxm_log_pkt_timer, 20, 20, T_SEC );
      }
      /* Else if one or more were enabled, and now none are enabled,
         stop the periodic flush timer */
      else if( cxm_one_or_more_logs_en &&
               !(cxm_low_prio_log_en || cxm_med_prio_log_en ||
                 cxm_high_prio_log_en || cxm_smem_log_en) )
      {
        timer_clr( &cxm_log_pkt_timer, T_SEC );
        va_end( args_list );
        return;
      }

      /* If none of the codes are enabled, exit.  Note purposeful
         assignment in conditional */
      if( !( cxm_one_or_more_logs_en = 
             (cxm_low_prio_log_en || cxm_med_prio_log_en ||
              cxm_high_prio_log_en || cxm_smem_log_en) ) )
      {
        va_end( args_list );
        return;
      }

      /* Initiate the log packet creation */
      cxm_log_initiate_pkt( event, ts, args_list, nargs );
    }
  }

  va_end( args_list );

  return;
} /* cxm_trace_event */

/*===========================================================================

  FUNCTION:  cxm_counter_event

===========================================================================*/
/*!
  @brief
    To increment a CXM counter corresponding to some event. Also log the 
    timestamp and value of that event

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_event
(
  cxm_counters_e counter,    /*!< Unique counter ID */
  uint32        last_value  /*!< Specific to the event that the counter 
                                 refers to. This is meant to be the value of
                                 the event we are counting */
)
{
  /* guard against buffer overflow */
  CXM_ASSERT( counter < CXM_CNT_MAX_COUNTERS );

  /* log the counter event passed in + timestamp */
  cxm_counters[counter].count++;
  cxm_counters[counter].last_value = last_value;
  cxm_counters[counter].last_tstamp = timetick_get_safe();

  return;
}

/*===========================================================================

  FUNCTION:  cxm_counter_reset

===========================================================================*/
/*!
  @brief
    Reset specified counter. If value passed is CXM_CNT_MAX_COUNTERS, will
    reset ALL counters.

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_reset
(
  cxm_counters_e counter
)
{
  int i;

  /*-----------------------------------------------------------------------*/

  /* guard against buffer overflow */
  CXM_ASSERT( counter <= CXM_CNT_MAX_COUNTERS );

  if( CXM_CNT_MAX_COUNTERS == counter )
  {
    /* reset all counters */
    memset( &cxm_counters, 0, sizeof( cxm_counters ) );
  }
  else if ( CXM_CNT_POLICY_RESET == counter )
  {
    /* reset all counters except POLICY_RESET and POLICY_UPDATES counters */
    for(i = CXM_CNT_UART_RX; i < CXM_CNT_MAX_COUNTERS; i++)
    {
      cxm_counters[i].count = 0;
    }
  }
  else
  {
    /* reset specific counter */
    cxm_counters[counter].count = 0;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_counter_log

===========================================================================*/
/*!
  @brief
    Logs all CXM counters with F3s

  @return
    None
*/
/*=========================================================================*/
void cxm_counter_log
(
  void
)
{
  CXM_MSG_8(HIGH, "cxm counters 1: CXM_CNT_POLICY_RESET: %d, CXM_CNT_POLICY_UPDATES: %d, "
                  "CXM_CNT_UART_RX: %d, CXM_CNT_UART_RX_WCI2_TYPE1: %d, CXM_CNT_UART_RX_WCI2_TYPE6: %d, "
                  "CXM_CNT_UART_RX_WCI2_TYPE_UNSUPPORTED: %d, CXM_CNT_UART_TX: %d, CXM_CNT_UART_TX_WCI2_TYPE1: %d",
            cxm_counters[CXM_CNT_POLICY_RESET].count, cxm_counters[CXM_CNT_POLICY_UPDATES].count,
            cxm_counters[CXM_CNT_UART_RX].count, cxm_counters[CXM_CNT_UART_RX_WCI2_TYPE1].count,
            cxm_counters[CXM_CNT_UART_RX_WCI2_TYPE6].count, cxm_counters[CXM_CNT_UART_RX_WCI2_TYPE_UNSUPPORTED].count,
            cxm_counters[CXM_CNT_UART_TX].count, cxm_counters[CXM_CNT_UART_TX_WCI2_TYPE1].count );

  CXM_MSG_8(HIGH, "cxm counters 2: CXM_CNT_UART_TX_WCI2_TYPE3: %d, CXM_CNT_UART_TX_WCI2_TYPE6: %d, "
                  "CXM_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED: %d, CXM_CNT_LTE_ML1_ADVANCE_NOTICE_RECVD: %d, CXM_CNT_LTE_ML1_SLEEP_DURN_RECVD: %d, "
                  "CXM_CNT_CONTR_PWR_LMT_REQ: %d, CXM_CNT_CONTR_PWR_LMT_COND_FAIL: %d, CXM_CNT_CONTR_PWR_LMT_HONORED: %d",
            cxm_counters[CXM_CNT_UART_TX_WCI2_TYPE3].count, cxm_counters[CXM_CNT_UART_TX_WCI2_TYPE6].count,
            cxm_counters[CXM_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED].count, cxm_counters[CXM_CNT_LTE_ML1_ADVANCE_NOTICE_RECVD].count,
            cxm_counters[CXM_CNT_LTE_ML1_SLEEP_DURN_RECVD].count, cxm_counters[CXM_CNT_CONTR_PWR_LMT_REQ].count,
            cxm_counters[CXM_CNT_CONTR_PWR_LMT_COND_FAIL].count, cxm_counters[CXM_CNT_CONTR_PWR_LMT_HONORED].count );

  CXM_MSG_5(HIGH, "cxm counters 3: CXM_CNT_WCI2_PWR_LMT_REQ: %d, CXM_CNT_WCI2_PWR_LMT_COND_FAIL: %d, "
                  "CXM_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED: %d, CXM_CNT_LATE_LTE_TX_MSG: %d, CXM_CNT_TX_ADV_TRANSMIT: %d",
            cxm_counters[CXM_CNT_WCI2_PWR_LMT_REQ].count, cxm_counters[CXM_CNT_WCI2_PWR_LMT_COND_FAIL].count,
            cxm_counters[CXM_CNT_WCI2_PWR_LMT_HONORED].count, cxm_counters[CXM_CNT_LATE_LTE_TX_MSG].count,
            cxm_counters[CXM_CNT_TX_ADV_TRANSMIT].count);

  CXM_MSG_5(HIGH, "fw counters: Frame Sync: %d, TX Active: %d, RX Prio: %d, WCN Prio Req: %d, WCN Prio Denied: %d",
            cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[CXM_CNT_FRAME_SYNC].count,
            cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[CXM_CNT_TX_ACTIVE].count,
            cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[CXM_CNT_RX_PRIO].count,
            cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[CXM_CNT_WCN_PRIO_BLANK_REQ].count,
            cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[CXM_CNT_WCN_PRIO_BLANK_DENIED].count );

  return;
}

/*===========================================================================

  FUNCTION:  cxm_get_lte_counters

===========================================================================*/
/*!
  @brief
    Gets all counters and LTE related FW counters

  @return
    None
*/
/*=========================================================================*/
void cxm_get_lte_counters
(
  uint32 fw_counters[],
  uint32 counters[]
)
{
  uint8 i;

  /*-----------------------------------------------------------------------*/

  /*get LTE FW counters*/
  for (i=0; i<CXM_CNT_MAX_FW_COUNTERS; i++)
  {
    fw_counters[i] = cxm_fw_counters->tech_data[CXM_TECH_LTE].counter_event[i].count;
  }
  /*get CXM counters*/
  for (i=0; i<CXM_CNT_MAX_COUNTERS; i++)
  {
    counters[i] = cxm_counters[i].count;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_get_async_log

===========================================================================*/
/*!
  @brief
    Get a particular log packet that came in through msgr

  @return
    cxm_async_log_paylds_u
*/
/*=========================================================================*/
cxm_async_log_paylds_u* cxm_get_async_log
(
  uint32 log_id
)
{
  uint8 i;
  cxm_async_log_paylds_u* log_pkt = NULL;

  /*-----------------------------------------------------------------------*/

  /* get log data */
  for ( i = 0; i < COEX_MAX_ASYNC_LOGS; i++ )
  {
    if ( cxm_async_logs.data[i].log_id == log_id )
    {
      log_pkt = &cxm_async_logs.data[i].payld;
    }
  }
  
  return log_pkt;
}

/*===========================================================================

  FUNCTION:  cxm_pack_low_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_low_prio_log_packet_payload
(
  cxm_low_prio_log_data_u *dest,
  cxm_low_prio_log_type_e pyld_e
)
{
  cxm_async_log_paylds_u* log_pkt;

  /*-----------------------------------------------------------------------*/

  if( dest == NULL )
  {
    CXM_MSG_0(MED, "Payload destination pointer is NULL");
    return;
  }

  switch( pyld_e )
  {
    case CXM_LOG_STATE:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_state.active_tech_mask = 
        ((coex_state_type *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)]))->
        active_tech_mask;
      dest->log_state.conn_tech_mask = 
        ((coex_state_type *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)]))->
        conn_tech_mask;
      dest->log_state.tx_pwr_tech_mask =
        ((coex_state_type *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)]))->
        tx_pwr_tech_mask;
      dest->log_state.uart_en_state =
        ((coex_state_type *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)]))->
        uart_en_state;
      dest->log_state.uart_handle =
        ((coex_state_type *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_STATE)]))->
        uart_handle;
      break;

    case CXM_LOG_CONTROLLER_PWR_LMT_STATE:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_CONTROLLER_PWR_LMT_STATE)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_controller_pwr_lmt_state =
        *((coex_power_limit_state_e *)
          (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_CONTROLLER_PWR_LMT_STATE)]));
      break;

    case CXM_LOG_WCI2_PWR_LMT_STATE:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCI2_PWR_LMT_STATE)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_wci2_pwr_lmt_state =
        *((coex_power_limit_state_e *)
          (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_WCI2_PWR_LMT_STATE)]));
      break;

    case CXM_LOG_METRICS_LTE_BLER_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_BLER_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_lte_bler_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_BLER_STARTED)]))->value;
      break;

    case CXM_LOG_METRICS_LTE_SINR_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_SINR_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_lte_sinr_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_LTE_SINR_STARTED)]))->value;
      break;

    case CXM_LOG_METRICS_GSM1_SINR_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM1_SINR_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_gsm1_sinr_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM1_SINR_STARTED)]))->value;
      break;

    case CXM_LOG_METRICS_GSM2_SINR_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM2_SINR_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_gsm2_sinr_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM2_SINR_STARTED)]))->value;
      break;

    case CXM_LOG_METRICS_GSM3_SINR_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM3_SINR_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_gsm3_sinr_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_GSM3_SINR_STARTED)]))->value;
      break;

    case CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_metrics_tds_nrrwbnd_noise_started.value =
        ((atomic_word_t *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_METRICS_TDS_NRRWBND_NOISE_STARTED)]))->value;
      break;

    case CXM_LOG_ANTENNA_STATE:
      if( cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_antenna_state.state = 
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->state;
      dest->log_antenna_state.info.adv_notice = 
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->info->adv_notice;
      dest->log_antenna_state.info.duration = 
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->info->duration;
      dest->log_antenna_state.sync.id = 
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->sync->id;
      dest->log_antenna_state.sync.sync_state_token = 
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->sync->sync_state_token;
      dest->log_antenna_state.token_wrap = 
        *(((cxm_antenna_state_s *)
           (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->token_wrap);
      dest->log_antenna_state.grant_dur =
        ((cxm_antenna_state_s *)
         (cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_ANTENNA_STATE)]))->grant_dur;
      break;
    case CXM_LOG_WCN_TX_STATE:
      log_pkt = cxm_get_async_log(pyld_e);
      if ( log_pkt != NULL )
      {
        dest->log_wcn_tx_state.clnt = log_pkt->wcn_tx.clnt;
        dest->log_wcn_tx_state.status = log_pkt->wcn_tx.status;
        dest->log_wcn_tx_state.last_rep_tx = log_pkt->wcn_tx.last_rep_tx;
        dest->log_wcn_tx_state.last_rsp_code = log_pkt->wcn_tx.last_rsp_code;
      }
      break;
    case CXM_LOG_LTE_TX_ADV:
      memscpy(&dest->log_lte_tx_adv, sizeof(cxm_lte_tx_adv_s_pack), 
              (cxm_lte_tx_adv_s_pack *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_LTE_TX_ADV)]), 
              sizeof(cxm_lte_tx_adv_s_pack));
      break;
    case CXM_LOG_TECH_METRICS_READ:
      memscpy(&dest->log_tech_metrics_read, sizeof(cxm_tech_metrics_read_s_pack), 
              (cxm_tech_metrics_read_s_pack *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_TECH_METRICS_READ)]), 
              sizeof(cxm_tech_metrics_read_s_pack));
      break;
    case CXM_LOG_LTE_BLER_READ:
      memscpy(&dest->log_lte_bler_read, sizeof(cxm_lte_bler_read_s_pack), 
              (cxm_lte_bler_read_s_pack *)(cxm_low_prio_log_lut[CXM_INDX(CXM_LOG_LTE_BLER_READ)]), 
              sizeof(cxm_lte_bler_read_s_pack));
      break;

    default:
      break;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_pack_med_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_med_prio_log_packet_payload
(
  cxm_med_prio_log_data_u *dest,
  cxm_med_prio_log_type_e pyld_e
)
{
  int i;

  /*-----------------------------------------------------------------------*/

  if( dest == NULL )
  {
    CXM_MSG_0(MED, "Payload destination pointer is NULL");
    return;
  }

  switch( pyld_e )
  {
    case CXM_LOG_LTE_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_lte_wwan_state_info.state_data.num_link_info_sets = 
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;
      for( i = 0; i < dest->
           log_lte_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_lte_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_lte_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_lte_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_lte_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_lte_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_lte_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_lte_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      dest->log_lte_wwan_state_info.state_data.params.lte_params.frame_structure = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.frame_structure;
      dest->log_lte_wwan_state_info.state_data.params.lte_params.tdd_config = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.tdd_config;
      dest->log_lte_wwan_state_info.state_data.params.lte_params.ssp = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.ssp;
      dest->log_lte_wwan_state_info.state_data.params.lte_params.dl_cp = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.dl_cp;
      dest->log_lte_wwan_state_info.state_data.params.lte_params.ul_cp = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.ul_cp;
      dest->log_lte_wwan_state_info.state_data.params.lte_params.frame_offset = 
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->
        state_data.params.lte_params.frame_offset;

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_lte_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_lte_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_lte_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_lte_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->conn_state;
      dest->log_lte_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_lte_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_LTE_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_TDS_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_tds_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_tds_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_tds_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_tds_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_tds_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_tds_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_tds_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_tds_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_tds_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_tds_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_tds_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_tds_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_tds_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->conn_state;
      dest->log_tds_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_tds_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_TDS_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_GSM1_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_gsm1_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_gsm1_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_gsm1_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_gsm1_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_gsm1_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_gsm1_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_gsm1_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->conn_state;
      dest->log_gsm1_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_gsm1_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_ONEX_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_onex_wwan_state_info.state_data.num_link_info_sets = 
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_onex_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_onex_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_onex_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_onex_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_onex_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_onex_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_onex_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_onex_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_onex_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_onex_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_onex_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_onex_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->conn_state;
      dest->log_onex_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_onex_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_ONEX_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_HDR_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_hdr_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_hdr_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_hdr_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_hdr_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_hdr_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_hdr_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_hdr_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_hdr_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->conn_state;
      dest->log_hdr_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_hdr_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_HDR_WWAN_STATE_INFO)]))->l3;
      
      break;

    case CXM_LOG_WCDMA_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_wcdma_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_wcdma_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_wcdma_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_wcdma_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_wcdma_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_wcdma_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_wcdma_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->conn_state;
      dest->log_wcdma_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_wcdma_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCDMA_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_GSM2_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_gsm2_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_gsm2_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_gsm2_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_gsm2_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_gsm2_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_gsm2_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_gsm2_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->conn_state;
      dest->log_gsm2_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_gsm2_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_GSM3_WWAN_STATE_INFO:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_gsm3_wwan_state_info.state_data.num_link_info_sets =
        ((coex_wwan_tech_info *)(cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
        state_data.num_link_info_sets;

      for( i = 0; i < dest->
           log_gsm3_wwan_state_info.state_data.num_link_info_sets; ++i )
      {
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].frequency =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].frequency;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].bandwidth =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].bandwidth;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].band =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].band;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].channel =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].channel;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].rf_device =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].rf_device;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].direction =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].direction;
        dest->log_gsm3_wwan_state_info.state_data.link_list[i].type =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->
          state_data.link_list[i].type;
      }

      for( i = 0; i < CXM_MAX_SUPPORTED_LINK_SETS; ++i )
      {
        dest->log_gsm3_wwan_state_info.tech_band_filter[i] =
          ((coex_wwan_tech_info *)
           (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->tech_band_filter[i];
      }

      dest->log_gsm3_wwan_state_info.sleep_thresh =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->sleep_thresh;
      dest->log_gsm3_wwan_state_info.wakeup_tick =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->wakeup_tick;
      dest->log_gsm3_wwan_state_info.conn_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->conn_state;
      dest->log_gsm3_wwan_state_info.sleep_state =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->sleep_state;
      dest->log_gsm3_wwan_state_info.l3 =
        ((coex_wwan_tech_info *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_WWAN_STATE_INFO)]))->l3;

      break;

    case CXM_LOG_GLOBAL_POLICY_PARAMS:
      if( cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      dest->log_global_policy_params.link_path_loss_threshold =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        link_path_loss_threshold;
      dest->log_global_policy_params.rb_filter_alpha =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        rb_filter_alpha;
      dest->log_global_policy_params.filtered_rb_threshold =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        filtered_rb_threshold;
      dest->log_global_policy_params.t7_con_holdoff =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        t7_con_holdoff;
      dest->log_global_policy_params.t7_pwr_alpha =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        t7_pwr_alpha;
      dest->log_global_policy_params.t7_pwr_thresh =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        t7_pwr_thresh;
      dest->log_global_policy_params.t7_pwr_holdoff =
        ((coex_plcy_parms_s *)
         (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_GLOBAL_POLICY_PARAMS)]))->
        t7_pwr_holdoff;
      break;

    case CXM_LOG_MDM_STATE:
      dest->log_mdm_state.wwan_tx_active = ((wci2_type7_type_s *)
             (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_MDM_STATE)]))->wwan_tx_active;
      dest->log_mdm_state.tx_ant_sel = ((wci2_type7_type_s *)
             (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_MDM_STATE)]))->tx_ant_sel;
      dest->log_mdm_state.wwan_tx_pwr_active = ((wci2_type7_type_s *)  
             (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_MDM_STATE)]))->wwan_tx_pwr_active;
      break;

    case CXM_LOG_WCI2_DATA_RECV:
      dest->log_wci2_data_recv = *((uint8 *)
             (cxm_med_prio_log_lut[CXM_INDX(CXM_LOG_WCI2_DATA_RECV)])) ;
      break;

    default:
      break;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_pack_victim_tbl

===========================================================================*/
/*!
  @brief
    Inline helper function to create a deep copy of victim_tbl, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_victim_tbl
(
  uint32 start_indx,
  cxm_high_prio_log_data_u *dest
)
{
  uint32 i, j, vict_seg_sz;

  /*-----------------------------------------------------------------------*/

  /* Throw an error here if victim table size is not 32 */
#if (COEX_MAX_VICTIM_TBL_ENTRIES != 32)
#error Fix victim table log packets
#endif

  /* For convenience, since victim table is divided in 4 */
  vict_seg_sz = COEX_MAX_VICTIM_TBL_ENTRIES/4;

  /* If start indx is not either 0, 8, 16, or 24, exit */
  if( start_indx % vict_seg_sz != 0 ||
      start_indx > 24 ||
      start_indx + vict_seg_sz > (uint32)COEX_MAX_VICTIM_TBL_ENTRIES )
  {
    CXM_MSG_0( MED, "victim tbl packing failed" );
    return;
  }

  for( i = start_indx; i < (start_indx + vict_seg_sz); ++i )
  {
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wwan.direction =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wwan.direction;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wwan.
      operating_dim =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wwan.operating_dim;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wwan.tech =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wwan.tech;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wwan.freq_start =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wwan.freq_start;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wwan.freq_stop =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wwan.freq_stop;

    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn.mode =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn.mode;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn.tech =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn.tech;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn.freq_start =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn.freq_start;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn.freq_stop =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn.freq_stop;

    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_policy =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_policy;

    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_policy =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn_policy;

    /* Copy wcn_params */
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_params.
      imd_protection_params.block_tx = 
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn_params.imd_protection_params.block_tx;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_params.
      imd_protection_params.static_backoff_tx_pwr =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn_params.imd_protection_params.static_backoff_tx_pwr;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_params.
      bt_afh_update_len =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].wcn_params.bt_afh_update_len;

    /* Copy all bt_afh_update fields */
    for( j = 0; 
         j < dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].
         wcn_params.bt_afh_update_len;
         ++ j)
    {
      dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_params.
        bt_afh_update[j].start_bt_channel =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
        victim_tbl[i].wcn_params.bt_afh_update[j].start_bt_channel;
      dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].wcn_params.
        bt_afh_update[j].end_bt_channel =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
        victim_tbl[i].wcn_params.bt_afh_update[j].end_bt_channel;
    }

    /* Copy mdm_params */
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      wcn_prio.tx_thlds.tx_pwr_thld =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.wcn_prio.tx_thlds.tx_pwr_thld;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      wcn_prio.tx_thlds.tx_bw_thld =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.wcn_prio.tx_thlds.tx_bw_thld;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      wcn_prio.frame_denials.duty_cycle_params.max_allowed_frame_denials =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.
      max_allowed_frame_denials;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      wcn_prio.frame_denials.duty_cycle_params.frame_denial_window =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.wcn_prio.frame_denials.duty_cycle_params.
      frame_denial_window;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      wcn_prio.frame_denials.max_cont_denials =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.wcn_prio.frame_denials.max_cont_denials;
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
      tx_adv.rb_thresh_len =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.tx_adv.rb_thresh_len;

    /* Copy rb_thresh fields */
    for( j = 0; 
         j < dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].
         mdm_params.tx_adv.rb_thresh_len;
         ++j )
    {
      dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
        tx_adv.rb_thresh[j].start =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
        victim_tbl[i].mdm_params.tx_adv.rb_thresh[j].start;
      dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].mdm_params.
        tx_adv.rb_thresh[j].end =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
        victim_tbl[i].mdm_params.tx_adv.rb_thresh[j].end;
    }
    
    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].
      mdm_params.tx_adv.tx_power_thresh =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.tx_adv.tx_power_thresh;

    dest->log_params_victim_tbl.victim_tbl[i%vict_seg_sz].
      mdm_params.pwr_lmt.power =
      ((coex_config_params_v8 *)
       (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)]))->
      victim_tbl[i].mdm_params.pwr_lmt.power;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_pack_high_prio_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_high_prio_log_packet_payload
(
  cxm_high_prio_log_data_u *dest,
  cxm_high_prio_log_type_e pyld_e
)
{
  int i, n_pkt;

  /*-----------------------------------------------------------------------*/

  if( dest == NULL )
  {
    CXM_MSG_0(MED, "Payload destination pointer is NULL");
    return;
  }

  /* switch to save code for tech policy */
  switch( pyld_e )
  {
    case CXM_LOG_LTE_POLICY_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_lte_policy_info.policy_state = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->policy_state;
      dest->log_lte_policy_info.policy = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->policy;
      dest->log_lte_policy_info.power_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.power_threshold;
      dest->log_lte_policy_info.rb_threshold= 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.rb_threshold;
      dest->log_lte_policy_info.tx_continuous_subframe_denials_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.tx_continuous_subframe_denials_threshold;
      dest->log_lte_policy_info.params.lte.apt_table = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.apt_table;
      dest->log_lte_policy_info.params.lte.controller_tx_power_limit = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.controller_tx_power_limit;
      dest->log_lte_policy_info.params.lte.wci2_power_limit = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.wci2_power_limit;
      dest->log_lte_policy_info.params.lte.wci2_tx_pwrlmt_timeout = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.wci2_tx_pwrlmt_timeout;
      dest->log_lte_policy_info.params.lte.controller_tx_pwrlmt_timeout = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.controller_tx_pwrlmt_timeout;
      dest->log_lte_policy_info.params.lte.tx_power_threshold_for_adv_tx_notice = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.tx_power_threshold_for_adv_tx_notice;
      dest->log_lte_policy_info.params.lte.rb_threshold_for_adv_tx_notice = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_LTE_POLICY_INFO)]))->
        plcy_parms.params.lte.rb_threshold_for_adv_tx_notice;
      break;

    case CXM_LOG_GSM1_POLICY_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_gsm1_policy_info.policy_state = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)]))->policy_state;
      dest->log_gsm1_policy_info.policy = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)]))->policy;
      dest->log_gsm1_policy_info.power_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)]))->
        plcy_parms.power_threshold;
      dest->log_gsm1_policy_info.rb_threshold= 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)]))->
        plcy_parms.rb_threshold;
      dest->log_gsm1_policy_info.tx_continuous_subframe_denials_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM1_POLICY_INFO)]))->
        plcy_parms.tx_continuous_subframe_denials_threshold;
      break;

    case CXM_LOG_GSM2_POLICY_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_gsm2_policy_info.policy_state = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)]))->policy_state;
      dest->log_gsm2_policy_info.policy = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)]))->policy;
      dest->log_gsm2_policy_info.power_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)]))->
        plcy_parms.power_threshold;
      dest->log_gsm2_policy_info.rb_threshold= 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)]))->
        plcy_parms.rb_threshold;
      dest->log_gsm2_policy_info.tx_continuous_subframe_denials_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM2_POLICY_INFO)]))->
        plcy_parms.tx_continuous_subframe_denials_threshold;
      break;

    case CXM_LOG_GSM3_POLICY_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_gsm3_policy_info.policy_state = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)]))->policy_state;
      dest->log_gsm3_policy_info.policy = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)]))->policy;
      dest->log_gsm3_policy_info.power_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)]))->
        plcy_parms.power_threshold;
      dest->log_gsm3_policy_info.rb_threshold= 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)]))->
        plcy_parms.rb_threshold;
      dest->log_gsm3_policy_info.tx_continuous_subframe_denials_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_GSM3_POLICY_INFO)]))->
        plcy_parms.tx_continuous_subframe_denials_threshold;
      break;

    case CXM_LOG_TDS_POLICY_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_tds_policy_info.policy_state = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)]))->policy_state;
      dest->log_tds_policy_info.policy = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)]))->policy;
      dest->log_tds_policy_info.power_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)]))->
        plcy_parms.power_threshold;
      dest->log_tds_policy_info.rb_threshold= 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)]))->
        plcy_parms.rb_threshold;
      dest->log_tds_policy_info.tx_continuous_subframe_denials_threshold = 
        ((coex_wwan_tech_info *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_TDS_POLICY_INFO)]))->
        plcy_parms.tx_continuous_subframe_denials_threshold;
      break;

    case CXM_LOG_BAND_FILTER_INFO:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_band_filter_info.filter_on =
        ((coex_band_filter_s *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
        filter_on;
      dest->log_band_filter_info.ul_len =
        ((coex_band_filter_s *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
        ul_len;
      dest->log_band_filter_info.dl_len =
        ((coex_band_filter_s *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
        dl_len;

      for( i = 0; i < dest->log_band_filter_info.ul_len; ++i )
      {
        dest->log_band_filter_info.ul_filter[i].freq_start =
          ((coex_band_filter_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
          ul_filter[i].freq_start;
        dest->log_band_filter_info.ul_filter[i].freq_stop =
          ((coex_band_filter_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
          ul_filter[i].freq_stop;
      }
      
      for( i = 0; i < dest->log_band_filter_info.dl_len; ++i )
      {
        dest->log_band_filter_info.dl_filter[i].freq_start =
          ((coex_band_filter_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
          dl_filter[i].freq_start;
        dest->log_band_filter_info.dl_filter[i].freq_stop =
          ((coex_band_filter_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_BAND_FILTER_INFO)]))->
          dl_filter[i].freq_stop;
      }

      break; 

    case CXM_LOG_PARAMS_NO_TABLES:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }
      dest->log_params_no_tables.version = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        version;
      dest->log_params_no_tables.cxm_coex_protocol = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        cxm_coex_protocol;
      dest->log_params_no_tables.num_conflicts = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        num_conflicts;
      dest->log_params_no_tables.baud = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        baud;
      dest->log_params_no_tables.link_path_loss_threshold = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        link_path_loss_threshold;
      dest->log_params_no_tables.filtered_rb_threshold = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        filtered_rb_threshold;
      dest->log_params_no_tables.rb_filter_alpha = 
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        rb_filter_alpha;

      dest->log_params_no_tables.cxm_tx_active_adv_notice =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        cxm_tx_active_adv_notice;
      dest->log_params_no_tables.mdm_conn_ind_holdoff_time =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        mdm_conn_ind_holdoff_time;
      dest->log_params_no_tables.cxm_lte_sleep_notifi_thres =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        cxm_lte_sleep_notifi_thres;
      dest->log_params_no_tables.tx_adv_late_toggle =
        ((coex_config_params_v8 *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_NO_TABLES)]))->
        tx_adv_late_toggle;

    case CXM_LOG_PARAMS_VICTIM_TBL:
/* Throw an error here if victim table size is not 32 */
#if (COEX_MAX_VICTIM_TBL_ENTRIES != 32)
#error Fix victim table log packets
#endif

      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_VICTIM_TBL)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      /* Special case: log all 4 parts of victim table, separately */
      for( i = 0; i < COEX_MAX_VICTIM_TBL_ENTRIES; 
           i += COEX_MAX_VICTIM_TBL_ENTRIES/4 )
      {
        n_pkt = cxm_high_prio_log.log_struct.num_packets;
        cxm_high_prio_log.log_struct.log_data[n_pkt].high_prio_type_id =
          CXM_LOG_PARAMS_VICTIM_TBL;
        cxm_pack_victim_tbl( i, &(cxm_high_prio_log.log_struct.
                                  log_data[n_pkt].payload) );
        cxm_high_prio_log.log_struct.num_packets++;
        if( cxm_high_prio_log.log_struct.num_packets == CXM_HIGH_PRIO_MAX_PACKETS )
        {
          /* Send the high priority log to diag */
          cxm_send_log( LOG_CXM_HIGH_PRIO_C );
        }
      }
      break;

    case CXM_LOG_PARAMS_PORT_TABLE:
      if( cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)] == NULL )
      {
        CXM_MSG_0( MED, "log lut null pointer" );
        return;
      }

      /* Copy port_table */
      dest->log_params_port_table.num_ports =
        ((cxm_port_table_v01_s *)
         (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
        num_ports;
      
      /* Copy port_info */
      for( i = 0; i < dest->log_params_port_table.num_ports; ++i )
      {
        dest->log_params_port_table.port_info[i].id =
          ((cxm_port_table_v01_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
          port_info[i].id;
        dest->log_params_port_table.port_info[i].type =
          ((cxm_port_table_v01_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
          port_info[i].type;
        dest->log_params_port_table.port_info[i].addr =
          ((cxm_port_table_v01_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
          port_info[i].addr;
        dest->log_params_port_table.port_info[i].assert_offset_us =
          ((cxm_port_table_v01_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
          port_info[i].assert_offset_us;
        dest->log_params_port_table.port_info[i].deassert_offset_us =
          ((cxm_port_table_v01_s *)
           (cxm_high_prio_log_lut[CXM_INDX(CXM_LOG_PARAMS_PORT_TABLE)]))->
          port_info[i].deassert_offset_us;
      }
      break;

    default:
      break;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_pack_gsm_timeline

===========================================================================*/
/*!
  @brief
    Inline helper function to create a deep copy of gsm timeline, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_gsm_timeline
(
  uint32 start_indx,
  uint32 end_idx,
  cxm_smem_log_data_u *dest,
  cxm_smem_data_log_type_e pyld_e
)
{
  uint32 i;

  /*-----------------------------------------------------------------------*/

  /* Throw an error here if gsm timeline size is not 32 */
#if (CXM_SMEM_MAX_TIMELINE_SLOTS != 32)
#error Fix gsm timeline log packets
#endif

  /* If start indx is not a multiple of 4 and less than max, exit */
  if( end_idx > (uint32)CXM_SMEM_MAX_TIMELINE_SLOTS - 1)
  {
    CXM_MSG_0( MED, "gsm timeline packing failed" );
    return;
  }

  switch( pyld_e )
  {
    case CXM_LOG_SMEM_GSM1_TIMELINE_V2:
      for( i = start_indx; i <= end_idx; ++i )
      {
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].frequency =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].frequency;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].bandwidth =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].bandwidth;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].slot_type =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].slot_type;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].start_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].start_time;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].end_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].end_time;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].prio =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].prio;
        dest->log_smem_gsm1_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].pwr =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm1.timeline.slot_list[i].pwr;
      }
      break;

    case CXM_LOG_SMEM_GSM2_TIMELINE_V2:
      for( i = start_indx; i <= end_idx; ++i )
      {
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].frequency =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].frequency;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].bandwidth =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].bandwidth;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].slot_type =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].slot_type;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].start_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].start_time;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].end_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].end_time;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].prio =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].prio;
        dest->log_smem_gsm2_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].pwr =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm2.timeline.slot_list[i].pwr;
      }
      break;

    case CXM_LOG_SMEM_GSM3_TIMELINE_V2:
      for( i = start_indx; i <= end_idx; ++i )
      {
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].frequency =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].frequency;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].bandwidth =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].bandwidth;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].slot_type =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].slot_type;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].start_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].start_time;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].end_time =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].end_time;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].prio =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].prio;
        dest->log_smem_gsm3_timeline.slot_list[i%CXM_LOG_GSM_TL_SEG_SIZE].pwr =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->
          gsm3.timeline.slot_list[i].pwr;
      }
      break;
    
    default:
      CXM_MSG_0( MED, "gsm timeline pack improper pyld_e" );
      break;         
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_pack_smem_log_packet_payload

===========================================================================*/
/*!
  @brief
    Helper function to create a deep copy of payload structures, in packed
    form, for ease of QXDM parsing

  @return
    None
*/
/*=========================================================================*/
void cxm_pack_smem_log_packet_payload
(
  cxm_smem_log_data_u *dest,
  cxm_smem_data_log_type_e pyld_e
)
{
  int i;
  uint8 n_pkt;
  cxm_async_log_paylds_u* log_pkt;
  uint8 gsm_tl_end_idx;
  uint64 gsm_tl_ts;

  /*-----------------------------------------------------------------------*/

  if( dest == NULL )
  {
    CXM_MSG_0(MED, "Payload destination pointer is NULL");
    return;
  }

  if( cxm_log_smem_data_p == NULL || *cxm_log_smem_data_p == NULL )
  {
    CXM_MSG_0( MED, "smem data null pointer" );
    return;
  }

  switch( pyld_e )
  {
    case CXM_LOG_SMEM_LTE_SLEEP_WAKEUP:
      dest->log_smem_lte_sleep_wakeup.is_waking_up = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.state.is_waking_up;
      dest->log_smem_lte_sleep_wakeup.duration = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.state.duration;
      break;

    case CXM_LOG_SMEM_LTE_FRAME_TIMING:
      dest->log_smem_lte_frame_timing.dl_frame_time = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.frame_timing.dl_frame_time;
      dest->log_smem_lte_frame_timing.ta = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.frame_timing.ta;
      break;

    case CXM_LOG_SMEM_LTE_TX_ADV:
      dest->log_smem_lte_tx_adv.sfn = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.sfn;
      dest->log_smem_lte_tx_adv.transmit = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.transmit;
      dest->log_smem_lte_tx_adv.channel_type = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.channel_type;
      dest->log_smem_lte_tx_adv.rbs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.rbs;
      dest->log_smem_lte_tx_adv.slot0_power = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot0_power;
      dest->log_smem_lte_tx_adv.slot0_first_rb = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot0_first_rb;
      dest->log_smem_lte_tx_adv.slot0_last_rb = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot0_last_rb;
      dest->log_smem_lte_tx_adv.slot1_power = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot1_power;
      dest->log_smem_lte_tx_adv.slot1_first_rb = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot1_first_rb;
      dest->log_smem_lte_tx_adv.slot1_last_rb = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.tx_adv.slot1_last_rb;
      break;

    case CXM_LOG_SMEM_LTE_HIGH_PRIO:
      dest->log_smem_lte_high_prio.start = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.high_prio.start;
      dest->log_smem_lte_high_prio.end = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.high_prio.end;
      dest->log_smem_lte_high_prio.num_freqs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.high_prio.num_freqs;

      for( i = 0; i < dest->log_smem_lte_high_prio.num_freqs &&
           i < CXM_SMEM_MAX_HIGH_PRIO_FREQ; ++i )
      {
        dest->log_smem_lte_high_prio.freq[i] =
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->lte.high_prio.freq[i];
      }
      break;

    case CXM_LOG_SMEM_TDS_SLEEP_WAKEUP:
      dest->log_smem_tds_sleep_wakeup.is_waking_up = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.state.is_waking_up;
      dest->log_smem_tds_sleep_wakeup.duration = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.state.duration;
      break;

    case CXM_LOG_SMEM_TDS_FRAME_TIMING:
      dest->log_smem_tds_frame_timing.sfn = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.frame_timing.sfn;
      dest->log_smem_tds_frame_timing.ustmr_val = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.frame_timing.ustmr_val;
      dest->log_smem_tds_frame_timing.ta = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.frame_timing.ta;
      break;

    case CXM_LOG_SMEM_TDS_ACT_ADV:
      dest->log_smem_tds_act_adv.sfn = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.act_adv.sfn;
      
      for( i = 0; i < CXM_SMEM_NUM_TDS_SLOTS; ++i )
      {
        dest->log_smem_tds_act_adv.slot[i].slot_type = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.act_adv.slot[i].slot_type;
        dest->log_smem_tds_act_adv.slot[i].tx_power = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.act_adv.slot[i].tx_power;
        dest->log_smem_tds_act_adv.slot[i].priority = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.act_adv.slot[i].priority;
        dest->log_smem_tds_act_adv.slot[i].monitor_freq = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.act_adv.slot[i].monitor_freq;
      }
      break;

    case CXM_LOG_SMEM_TDS_HIGH_PRIO:
      dest->log_smem_tds_high_prio.start = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.high_prio.start;
      dest->log_smem_tds_high_prio.end = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.high_prio.end;
      dest->log_smem_tds_high_prio.num_freqs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.high_prio.num_freqs;

      for( i = 0; i < dest->log_smem_tds_high_prio.num_freqs &&
           i < CXM_SMEM_MAX_HIGH_PRIO_FREQ; ++i )
      {
        dest->log_smem_tds_high_prio.freq[i] = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->tds.high_prio.freq[i];
      }
      break;

    case CXM_LOG_SMEM_GSM1_SLEEP_WAKEUP:
      dest->log_smem_gsm1_sleep_wakeup.is_waking_up = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.state.is_waking_up;
      dest->log_smem_gsm1_sleep_wakeup.duration = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.state.duration;
      break;

    case CXM_LOG_SMEM_GSM1_HIGH_PRIO:
      dest->log_smem_gsm1_high_prio.start = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.high_prio.start;
      dest->log_smem_gsm1_high_prio.end = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.high_prio.end;
      dest->log_smem_gsm1_high_prio.num_freqs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.high_prio.num_freqs;

      for( i = 0; i < dest->log_smem_gsm1_high_prio.num_freqs &&
           i < CXM_SMEM_MAX_HIGH_PRIO_FREQ; ++i )
      {
        dest->log_smem_gsm1_high_prio.freq[i] = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.high_prio.freq[i];
      }
      break;
      
    case CXM_LOG_SMEM_GSM1_TIMELINE_V2:
      /* Special case: log 4 entries at a time */
      /* get the timestamp that will be used for all the segments */
      gsm_tl_ts = cxm_smem_log.log_struct.log_data[cxm_smem_log.log_struct.num_packets].ts;
      for( i = 0; i < (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.timeline.num_entries; 
           i += CXM_LOG_GSM_TL_SEG_SIZE )
      {
        n_pkt = cxm_smem_log.log_struct.num_packets;
        cxm_smem_log.log_struct.log_data[n_pkt].smem_data_type_id 
          = CXM_LOG_SMEM_GSM1_TIMELINE_V2;
        cxm_smem_log.log_struct.log_data[n_pkt].ts = gsm_tl_ts;
        gsm_tl_end_idx = (i + CXM_LOG_GSM_TL_SEG_SIZE) > (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.timeline.num_entries ?
                        ((*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.timeline.num_entries - 1) : (i + CXM_LOG_GSM_TL_SEG_SIZE - 1);
        cxm_pack_gsm_timeline( i, gsm_tl_end_idx, &(cxm_smem_log.log_struct.log_data[n_pkt].payload),
                               CXM_LOG_SMEM_GSM1_TIMELINE_V2 );
        /* set the tot number and segment number */
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm1_timeline.msg_tot = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm1.timeline.num_entries;
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm1_timeline.segment = i/CXM_LOG_GSM_TL_SEG_SIZE;

        cxm_smem_log.log_struct.num_packets++;
        if( cxm_smem_log.log_struct.num_packets == CXM_SMEM_DATA_MAX_PACKETS )
        {
          /* Send the smem log to diag */
          cxm_send_log( LOG_CXM_SMEM_DATA_C );
        }
      }
      /* prevents an extra space in the buffer */
      cxm_smem_log.log_struct.num_packets--;
      break;

    case CXM_LOG_SMEM_GSM2_SLEEP_WAKEUP:
      dest->log_smem_gsm2_sleep_wakeup.is_waking_up = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.state.is_waking_up;
      dest->log_smem_gsm2_sleep_wakeup.duration = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.state.duration;
      break;

    case CXM_LOG_SMEM_GSM2_HIGH_PRIO:
      dest->log_smem_gsm2_high_prio.start = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.high_prio.start;
      dest->log_smem_gsm2_high_prio.end = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.high_prio.end;
      dest->log_smem_gsm2_high_prio.num_freqs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.high_prio.num_freqs;

      for( i = 0; i < dest->log_smem_gsm2_high_prio.num_freqs &&
           i < CXM_SMEM_MAX_HIGH_PRIO_FREQ; ++i )
      {
        dest->log_smem_gsm2_high_prio.freq[i] = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.high_prio.freq[i];
      }
      break;

    case CXM_LOG_SMEM_GSM2_TIMELINE_V2:
      /* Special case: log 4 entries at a time */
      /* get the timestamp that will be used for all the segments */
      gsm_tl_ts = cxm_smem_log.log_struct.log_data[cxm_smem_log.log_struct.num_packets].ts;
      for( i = 0; i < (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.timeline.num_entries; 
           i += CXM_LOG_GSM_TL_SEG_SIZE )
      {
        n_pkt = cxm_smem_log.log_struct.num_packets;
        cxm_smem_log.log_struct.log_data[n_pkt].smem_data_type_id 
          = CXM_LOG_SMEM_GSM2_TIMELINE_V2;
        cxm_smem_log.log_struct.log_data[n_pkt].ts = gsm_tl_ts;
        gsm_tl_end_idx = (i + CXM_LOG_GSM_TL_SEG_SIZE) > (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.timeline.num_entries ?
                        ((*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.timeline.num_entries - 1) : (i + CXM_LOG_GSM_TL_SEG_SIZE - 1);
        cxm_pack_gsm_timeline( i, gsm_tl_end_idx, &(cxm_smem_log.log_struct.log_data[n_pkt].payload),
                               CXM_LOG_SMEM_GSM2_TIMELINE_V2 );
        /* set the tot number and segment number */
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm2_timeline.msg_tot = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm2.timeline.num_entries;
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm2_timeline.segment = i/CXM_LOG_GSM_TL_SEG_SIZE;

        cxm_smem_log.log_struct.num_packets++;
        if( cxm_smem_log.log_struct.num_packets == CXM_SMEM_DATA_MAX_PACKETS )
        {
          /* Send the smem log to diag */
          cxm_send_log( LOG_CXM_SMEM_DATA_C );
        }
      }
      /* prevents an extra space in the buffer */
      cxm_smem_log.log_struct.num_packets--;
      break;

    case CXM_LOG_SMEM_GSM3_SLEEP_WAKEUP:
      dest->log_smem_gsm3_sleep_wakeup.is_waking_up = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.state.is_waking_up;
      dest->log_smem_gsm3_sleep_wakeup.duration = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.state.duration;
      break;

    case CXM_LOG_SMEM_GSM3_HIGH_PRIO:
      dest->log_smem_gsm3_high_prio.start = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.high_prio.start;
      dest->log_smem_gsm3_high_prio.end = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.high_prio.end;
      dest->log_smem_gsm3_high_prio.num_freqs = 
        (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.high_prio.num_freqs;

      for( i = 0; i < dest->log_smem_gsm3_high_prio.num_freqs &&
           i < CXM_SMEM_MAX_HIGH_PRIO_FREQ; ++i )
      {
        dest->log_smem_gsm3_high_prio.freq[i] = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.high_prio.freq[i];
      }
      break;

    case CXM_LOG_SMEM_GSM3_TIMELINE_V2:
      /* Special case: log 4 entries at a time */
      /* get the timestamp that will be used for all the segments */
      gsm_tl_ts = cxm_smem_log.log_struct.log_data[cxm_smem_log.log_struct.num_packets].ts;
      for( i = 0; i < (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.timeline.num_entries; 
           i += CXM_LOG_GSM_TL_SEG_SIZE )
      {
        n_pkt = cxm_smem_log.log_struct.num_packets;
        cxm_smem_log.log_struct.log_data[n_pkt].smem_data_type_id 
          = CXM_LOG_SMEM_GSM3_TIMELINE_V2;
        cxm_smem_log.log_struct.log_data[n_pkt].ts = gsm_tl_ts;
        gsm_tl_end_idx = (i + CXM_LOG_GSM_TL_SEG_SIZE) > (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.timeline.num_entries ?
                        ((*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.timeline.num_entries - 1) : (i + CXM_LOG_GSM_TL_SEG_SIZE - 1);
        cxm_pack_gsm_timeline( i, gsm_tl_end_idx, &(cxm_smem_log.log_struct.log_data[n_pkt].payload),
                               CXM_LOG_SMEM_GSM3_TIMELINE_V2 );
        /* set the tot number and segment number */
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm3_timeline.msg_tot = 
          (*((cxm_smem_data_s **)cxm_log_smem_data_p))->gsm3.timeline.num_entries;
        cxm_smem_log.log_struct.log_data[n_pkt].payload.log_smem_gsm3_timeline.segment = i/CXM_LOG_GSM_TL_SEG_SIZE;

        cxm_smem_log.log_struct.num_packets++;
        if( cxm_smem_log.log_struct.num_packets == CXM_SMEM_DATA_MAX_PACKETS )
        {
          /* Send the smem log to diag */
          cxm_send_log( LOG_CXM_SMEM_DATA_C );
        }
      }
      /* prevents an extra space in the buffer */
      cxm_smem_log.log_struct.num_packets--;
      break;

    case CXM_LOG_SMEM_TDS_FW_DATA_V2:
    case CXM_LOG_SMEM_GSM1_FW_DATA_V2:
    case CXM_LOG_SMEM_GSM2_FW_DATA_V2:
    case CXM_LOG_SMEM_GSM3_FW_DATA_V2:
      log_pkt = cxm_get_async_log(pyld_e);
      if ( log_pkt != NULL )
      {
        dest->log_smem_fw_data.cxm_smem_fw.data = 
          log_pkt->fw_activity.data;
      }
      break;
    default:
      break;
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_log_new_data

===========================================================================*/
/*!
  @brief
    Takes one of the enumerated log packet payload datatypes and creates a
    new entry in the appropriate log.

  @return
    None
*/
/*=========================================================================*/
void cxm_log_new_data
(
  uint32 pyld_e
)
{
  uint8 n_pkt;
  const void *data_ptr;

  /*-----------------------------------------------------------------------*/

  if( CXM_LOG_LP( pyld_e ) )
  {
    /* Check for out of bounds access */
    if( CXM_INDX(pyld_e) >= CXM_INDX(CXM_LOG_MAX_LOW_PRIO) )
    {
      return;
    }

    data_ptr = cxm_low_prio_log_lut[CXM_INDX(pyld_e)]; 
    /* Check for NULL payload */
    if( data_ptr == NULL )
    {
      CXM_MSG_0(LOW, "NULL payload not added to log packet");
      return;
    }

    n_pkt = cxm_low_prio_log.log_struct.num_packets;
    cxm_pack_low_prio_log_packet_payload(&(cxm_low_prio_log.log_struct.log_data[n_pkt].payload),
                                         (cxm_low_prio_log_type_e)pyld_e);
    cxm_low_prio_log.log_struct.num_packets++; 
    if( cxm_low_prio_log.log_struct.num_packets == CXM_LOW_PRIO_MAX_PACKETS )
    {
      /* Send the low priority log to diag */
      cxm_send_log( LOG_CXM_LOW_PRIO_C );
    }
  }
  else if( CXM_LOG_MP( pyld_e ) )
  {
    /* Check for out of bounds access */
    if( CXM_INDX(pyld_e) >= CXM_INDX(CXM_LOG_MAX_MED_PRIO) )
    {
      return;
    }

    data_ptr = cxm_med_prio_log_lut[CXM_INDX(pyld_e)];
    /* Check for NULL payload */
    if( data_ptr == NULL )
    {
      CXM_MSG_0(LOW, "NULL payload not added to log packet");
      return;
    }

    n_pkt = cxm_med_prio_log.log_struct.num_packets;
    cxm_pack_med_prio_log_packet_payload(&(cxm_med_prio_log.log_struct.log_data[n_pkt].payload),
                                         (cxm_med_prio_log_type_e)pyld_e);
    cxm_med_prio_log.log_struct.num_packets++;
    if( cxm_med_prio_log.log_struct.num_packets == CXM_MED_PRIO_MAX_PACKETS )
    {
      /* Send the med priority log to diag */
      cxm_send_log( LOG_CXM_MED_PRIO_C );
    }
  }
  else if( CXM_LOG_HP( pyld_e ) )
  {
    /* Check for out of bounds access */
    if( CXM_INDX(pyld_e) >= CXM_INDX(CXM_LOG_MAX_HIGH_PRIO) )
    {
      return;
    }

    data_ptr = cxm_high_prio_log_lut[CXM_INDX(pyld_e)];
    /* Check for NULL payload */
    if( data_ptr == NULL )
    {
      CXM_MSG_0(LOW, "NULL payload not added to log packet");
      return;
    }

    n_pkt = cxm_high_prio_log.log_struct.num_packets;
    cxm_pack_high_prio_log_packet_payload(&(cxm_high_prio_log.log_struct.log_data[n_pkt].payload),
                                          (cxm_high_prio_log_type_e)pyld_e);
    cxm_high_prio_log.log_struct.num_packets++;
    if( cxm_high_prio_log.log_struct.num_packets == CXM_HIGH_PRIO_MAX_PACKETS )
    {
      /* Send the high priority log to diag */
      cxm_send_log( LOG_CXM_HIGH_PRIO_C );
    }
  }
  else if( CXM_LOG_SM( pyld_e ) )
  {
    /* Check for out of bounds access */
    if( CXM_INDX(pyld_e) >= CXM_INDX(CXM_LOG_MAX_SMEM_DATA) )
    {
      return;
    }

    data_ptr = (void *)cxm_log_smem_data_p;
    /* Check for NULL payload */
    if( data_ptr == NULL )
    {
      CXM_MSG_0(LOW, "NULL payload not added to log packet");
      return;
    }

    n_pkt = cxm_smem_log.log_struct.num_packets;
    cxm_pack_smem_log_packet_payload(&(cxm_smem_log.log_struct.log_data[n_pkt].payload),
                                     (cxm_smem_data_log_type_e)pyld_e);
    cxm_smem_log.log_struct.num_packets++;
    if( cxm_smem_log.log_struct.num_packets == CXM_SMEM_DATA_MAX_PACKETS )
    {
      /* Send the smem log to diag */
      cxm_send_log( LOG_CXM_SMEM_DATA_C );
    }
  }
  else 
  {
    CXM_MSG_0(LOW, "Unsupported log packet payload type");
  }

  return;
}

/*===========================================================================

  FUNCTION:  cxm_send_log

===========================================================================*/
/*!
  @brief
    Sends the specified log to diag and resets the log.

  @return
    None
*/
/*=========================================================================*/
void cxm_send_log
( 
  uint16 log_code
)
{
  void *cxm_log_hdr_p;
  unsigned int log_size;

  /*-----------------------------------------------------------------------*/

  CXM_MSG_0( MED, "CXM entered cxm_send_log" );

  switch( log_code )
  {
    case LOG_CXM_LOW_PRIO_C:
      if( cxm_low_prio_log.log_struct.num_packets == 0 )
      {
        return;
      }
      cxm_log_hdr_p = (void *)(&cxm_low_prio_log);
      log_size = sizeof( cxm_low_prio_log );
      cxm_low_prio_log.version = CXM_LOG_PKT_VERSION;
      cxm_low_prio_log.subcode = CXM_LOG_PKT_SUBCODE;
      break;
    case LOG_CXM_MED_PRIO_C:
      if( cxm_med_prio_log.log_struct.num_packets == 0 )
      {
        return;
      }
      cxm_log_hdr_p = (void *)(&cxm_med_prio_log);
      log_size = sizeof( cxm_med_prio_log );
      cxm_med_prio_log.version = CXM_LOG_PKT_VERSION;
      cxm_med_prio_log.subcode = CXM_LOG_PKT_SUBCODE;
      break;
    case LOG_CXM_HIGH_PRIO_C:
      if( cxm_high_prio_log.log_struct.num_packets == 0 )
      {
        return;
      }
      cxm_log_hdr_p = (void *)(&cxm_high_prio_log);
      log_size = sizeof( cxm_high_prio_log );
      cxm_high_prio_log.version = CXM_LOG_PKT_VERSION;
      cxm_high_prio_log.subcode = CXM_LOG_PKT_SUBCODE;
      break;
    case LOG_CXM_SMEM_DATA_C:
      if( cxm_smem_log.log_struct.num_packets == 0 )
      {
        return;
      }
      cxm_log_hdr_p = (void *)(&cxm_smem_log);
      log_size = sizeof( cxm_smem_log );
      cxm_smem_log.version = CXM_LOG_PKT_VERSION;
      cxm_smem_log.subcode = CXM_LOG_PKT_SUBCODE;
      break;
    default:
      return;
  }

  /* Setup the log packet and submit to diag */
  log_set_code( cxm_log_hdr_p, log_code );
  log_set_length( cxm_log_hdr_p, log_size ); 
  log_set_timestamp( cxm_log_hdr_p );

  if( !log_submit( cxm_log_hdr_p ) )
  {
    CXM_MSG_0( MED, "Diag log packet submit failed" );
  }
  /* Reset the log */
  memset( cxm_log_hdr_p, 0, log_size );

  return;
}

/*=============================================================================

  FUNCTION:  coex_handle_log_async_ind

=============================================================================*/
/*!
    @brief
    Handle logs performed outside CXM context

    @return
    errno_enum_type
*/
/*===========================================================================*/
errno_enum_type coex_handle_log_async_ind (
  void *rcv_msgr_msg_ptr
)
{
  errno_enum_type                          retval = E_SUCCESS;
  cxm_coex_log_async_ind_s                 *msg_ptr;
  uint8                                    i;

  /*-----------------------------------------------------------------------*/

  /* De-mystify the received message pointer to the appropriate type */
  msg_ptr = (cxm_coex_log_async_ind_s *) rcv_msgr_msg_ptr;

  /* copy the incoming structs to the global struct */
  memscpy(&cxm_async_logs, sizeof(cxm_coex_log_async_ind_s), msg_ptr, sizeof(cxm_coex_log_async_ind_s));
  
  /* for each log code, call trace */
  for ( i = 0; i <  cxm_async_logs.num_logs; i++ )
  {
    CXM_TRACE( cxm_async_logs.event, 0, 0, 0, CXM_LOG_PKT_EN | CXM_ASYNC_PKT_EN, 
               cxm_async_logs.data[i].log_id );
  }

  return retval;
}


