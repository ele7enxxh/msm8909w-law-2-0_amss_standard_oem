/*!
  @file mmal_ciq_fend.c

  @brief
  Contains functions to initialize, shutdown, and misc function calls to the IQ agent

  @detail
  Contains the wrapper functions which makes the following calls to the IQ agent
  IQ_Init Initializes the instance of the IQ Agent Front-end library.
  IQ_Shutdown Shuts down the instance of IQ Agent Front-end library.
  IQ_GetAgentVersion Enables an integrator to programatically retrieve the IQ Agent's
   software version number.
   
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_fend.c#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
11/04/11   ejv    Register callbacks for ss25 and ss2b.
11/01/11   sk     Fix for cR 314255, Release the dsm item if the CFA pkt is generated
                  when the active RAT is not GSM
09/01/11   rj     Added Metrics LT03, LT04
08/18/11   sk     Picked a fix missed due to file resolve
08/18/11   sk     Listen to the CFA log packet for the GS31 metric
08/08/11   sk     Removed regn/dergn of the NAS_MESSAGE_SENT and 
                  NAS_MESSAGE_RECEIVED
08/04/11   sm/sk  CR299139: Added call back mmal_ciq_metrics_sniff_diag_logs for
                  diag response
08/04/11   sm     CR 299850 Fix: Released dsm item after processing LOG_GSM_RR_
                  CHANNEL_CONFIGURATION_C
08/02/11   sk     1.Fixed macros
                  2. If GS34/35 are not enabled then deregister the
                  call back with WCDMA RRC
07/20/11   sk     Fixed indentation
07/14/11   rj     Added Metrics LT10 and LT11
07/14/11   sm     Added code to avoid diag aggregation whenever any metrics is 
                  enabled
06/29/11   sk     Tap into ESM OTA IN and OUT msgs
06/28/11   pv     Compilation fix for non-LTE builds.
06/23/11   sk     Added listening to 0x5135 log packet
06/10/11   sk     Added listening to NAS OTA for GS30/31/34/35 
06/06/11   sk     Removed the undefined ref call to some functions
06/03/11   sk     GS34/35 related fixes
05/31/11   sk     Support for the LTE02 metric
05/19/11   sk     Reenable the GS01,02,03 metric
05/08/11   sk     Used the new mmal_ciq_metrics_free_item
05/05/11   sk     Modified the stats structure
05/04/11   sk     Featurize the code under FEATURE_MMAL_LTE
05/03/11   sk     Removed the registration/deregistration for the LOG_CM_CALL_EVENT_C 
                         log pkt as the GS01,02,03 metric is not needed anymore
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"


#ifdef FEATURE_MMAL_CIQ

#include <stdio.h>
#include <diag.h>
#ifdef FEATURE_MMAL_LTE
#include "mmal_msgr.h"
#include <lte_log_codes.h>
#include <lte_mac.h>
#include <lte_cphy_msg.h>
#include <lte_mac_msg.h>
#include <lte_rlc_msg.h>
#endif
#include "mmal_ciq_fend.h"
#include <log_codes.h>
#include <log_codes_gsm.h>
#include <log_codes_umts.h>
#include <log_codes_wcdma_int.h>
#include "mmal_ciq_metrics.h"
#include <naslog_v.h>
#ifdef FEATURE_WCDMA
#include <rrcdata.h>
#endif
#include "mmal_lte_metrics.h"
#include <diag.h>
#include <IQTypes.h>
#include <IQAgent.h>
#include <IQMetricsLT.h>
#include <IQMetricsGS.h>
#include <IQMetricsSS.h>
#include <mmal_util.h>
#include <log_codes_gsm_v.h>
#include <ps_ifacei_event.h>
#include <timer.h>
#include <cm.h>


/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef uint32 mmal_ciq_metric_mask_t;


#define MMAL_CIQ_SET_BMASK(bmask,value)           ((*bmask) |= (value))

#define MMAL_CIQ_IF_METRIC_ENABLED(_bmask, _mask) (((_bmask) & (_mask)) != 0)


/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/


typedef struct
{
  uint32                   mmal_ciq_fend_enable_metric_mask;
  mmal_ciq_rat_e           active_rat;

} mmal_ciq_fend_s;

mmal_ciq_fend_s            mmal_ciq_fend_info = {0,0};

#ifdef FEATURE_MMAL_LTE
static timer_type          lt03_timer;
static timer_type          lt04_timer;
static timer_type          lt07_timer;
lte_mac_stats_data_s       lt07_mac_stats;

static timer_type          lt09_timer;
lte_rlcdl_stats_s          lt09_dl_stats;
lte_rlcul_stats_s          lt09_ul_stats;
#endif // FEATURE_MMAL_LTE

static void               *gs15_iface_up_cb;
static void               *gs18_iface_down_cb;

#ifdef FEATURE_MMAL_LTE
iq_lte_eutra_meas_t        lt03_tMeas[32];

iq_lte_utra_meas_t         lt04_tMeasUtra[9];
iq_lte_geran_meas_t        lt04_tMeasGeran[9];
iq_lte_cdma2000_meas_t     lt04_tMeasCdma2000[9];
#endif // FEATURE_MMAL_LTE

static cm_client_id_type   mmal_cm_client_id;

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*===========================================================================

  FUNCTION:  mmal_ciq_fend_check_if_metric_enabled

===========================================================================*/
/*!
    @brief

    @detail

    @return

    @Note

*/
/*=========================================================================*/
boolean mmal_ciq_fend_check_if_metric_enabled
(
  mmal_ciq_metric_e   metric
)
{
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   metric ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

  FUNCTION:  mmal_ciq_fend_diag_cmd_rsp

===========================================================================*/
/*!
    @brief
    This function is provided as one of the parameters to diag_cmd_req
    when injecting the diag non-aggregation command. Then the response 
    generated would be routed to that callback function and not sent to 
    the apps side at all. callback function could just be a stub that just returns. 

    @detail
    

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_diag_cmd_rsp
(
  const byte *rsp, 
  unsigned int length, 
  void *param
)
{
  MMAL_MSG("MMAL Ignoring diag call back");
  mmal_ciq_metric_stats_info.metrics_info.num_diag_response++;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_set_active_rat

===========================================================================*/
/*!
    @brief
    This function sets the active RAT so that the log packet 0x713A
  can be appropriately routed

    @detail
    

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_set_active_rat(mmal_ciq_rat_e rat)
{
  mmal_ciq_fend_info.active_rat = rat;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_update_metric_bmask

===========================================================================*/
/*!
    @brief
    This function updates the bitmask

    @detail
    

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_update_metric_bmask
(
  const iq_metric_id_t          metric_id, 
  const uint32                      value
)
{
  iq_boolean_t  ret_val;

  /*----------------------------------------------------------------------
  First check if the IQ agent is interested in this metric
 -----------------------------------------------------------------------*/
  ret_val = IQ_ShouldSubmitMetric( metric_id );

  /*----------------------------------------------------------------------
  If the metric is ok to submit, add to the bitmask, otherwise dereisgter 
  the log event handler for this metric if it was registered with diag 
  previously
  -----------------------------------------------------------------------*/

  if (ret_val == IQ_TRUE)
  {
    MMAL_CIQ_SET_BMASK(
                      &mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                      value);
  }
  else
  {
    MMAL_MSG_ERROR("MMAL: CIQ not interested in metric ");
  }
}

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_register_diag_logs_events

===========================================================================*/
/*!
    @brief
    This function registers for the diag logs and events

    @detail
    Different metrics require diff log packets or events
    This function will register those log packets and events that are necessary 
    to produce these metrics

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_register_diag_logs_events
(
void
)
{
#ifdef FEATURE_MMAL_LTE
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      MMAL_CIQ_LT01_METRIC))
  {
    
  }
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      MMAL_CIQ_LT02_METRIC))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;

    if (!diag_add_log_listener(
       LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C,
        mmal_ciq_metrics_sniff_diag_logs,
        NULL //EVENT_LTE_RRC_UL_MSG
        ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;

    if (!diag_add_log_listener(
          LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,
          mmal_ciq_metrics_sniff_diag_logs,
          NULL //EMM Outgoing
        ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
    
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;

    if (!diag_add_log_listener(
            LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,
            mmal_ciq_metrics_sniff_diag_logs,
            NULL //ESM Outgoing
        ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;

    if (!diag_add_log_listener(
          LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C,
          mmal_ciq_metrics_sniff_diag_logs,
          NULL //EVENT_LTE_RRC_DL_MSG
        ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }
  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_LT06_METRIC)))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered++;

    /*! this is for extracting the NECI bit */
    if (!diag_add_event_listener(
      EVENT_LTE_RRC_STATE_CHANGE_TRIGGER,
      mmal_ciq_metrics_sniff_diag_events,
      NULL 
    ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
   
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered++;

    if (!diag_add_event_listener(
      EVENT_LTE_RRC_RADIO_LINK_FAILURE_STAT,
      mmal_ciq_metrics_sniff_diag_events,
      NULL 
    ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   MMAL_CIQ_LT10_METRIC ) )
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered++;
    if (!diag_add_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C,
                                mmal_ciq_metrics_sniff_diag_logs,
                                NULL ) )
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered++;
    if (!diag_add_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C,
                                mmal_ciq_metrics_sniff_diag_logs,
                                NULL ) )
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }

  /* LT11 uses one of the metrics from LT10, so only register here if we didn't
     register for LT10.  In this one case, we use an else if. */
  else if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                        MMAL_CIQ_LT11_METRIC ) )
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered++;
    if (!diag_add_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C,
                                mmal_ciq_metrics_sniff_diag_logs,
                                NULL ) )
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }
#endif
  /*------------------------------------------------------------------------*/
  /*! METRIC GS01, GS02, GS03 */
  /*------------------------------------------------------------------------*/
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      (MMAL_CIQ_GS01_METRIC|MMAL_CIQ_GS02_METRIC|MMAL_CIQ_GS03_METRIC)) 
  )
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    /*! Metric GS01, GS02, GS03 => Log code 0x12C1 */
    if (!diag_add_log_listener(
                                   LOG_CM_CALL_EVENT_C, /*0x12C1 */
                                   mmal_ciq_metrics_sniff_diag_logs,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

   
  /*if (!diag_add_event_listener(
                                   CM_CALL_EVENT_CALL_ON_HOLD, 
                                   mmal_ciq_metrics_sniff_diag_events,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  if (!diag_add_event_listener(
                                   CM_CALL_EVENT_CALL_RETRIEVED, 
                                   mmal_ciq_metrics_sniff_diag_events,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }*/
  }

  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      (MMAL_CIQ_GS03_METRIC)) 
  )
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    /*! Metric GS01, GS02, GS03 => Log code 0x12C1 */
    if (!diag_add_log_listener(
         LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C, /*0x12C1 */
         mmal_ciq_metrics_sniff_diag_logs,
         NULL //LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C
    ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }

  /*------------------------------------------------------------------------*/
  /*! METRIC GS34, GS 35 */
  /*------------------------------------------------------------------------*/

#if !defined(FEATURE_MMAL_DISABLE_GS3435) || !defined(FEATURE_MMAL_DISABLE_GS3031)
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (
#ifndef FEATURE_MMAL_DISABLE_GS3435
	  MMAL_CIQ_GS34_METRIC | MMAL_CIQ_GS35_METRIC
#else
          0
#endif /* !FEATURE_MMAL_DISABLE_GS3435 */
          |
#ifndef FEATURE_MMAL_DISABLE_GS3031
	  MMAL_CIQ_GS30_METRIC | MMAL_CIQ_GS31_METRIC
#else
          0
#endif /* !FEATURE_MMAL_DISABLE_GS3031 */
	 )))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    if (!diag_add_log_listener(
                                LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C,
                                mmal_ciq_metrics_sniff_diag_logs,
                                NULL//LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C
                                ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }
#endif /* !FEATURE_MMAL_DISABLE_GS3435 || !FEATURE_MMAL_DISABLE_GS3435 */

  /*------------------------------------------------------------------------*/
  /*! METRIC GS30, GS31 */
  /*------------------------------------------------------------------------*/
#ifndef FEATURE_MMAL_DISABLE_GS3031
  /*! Metric GS30, GS31 => Log code 0x512F */
  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_GS30_METRIC | MMAL_CIQ_GS31_METRIC)))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    if (!diag_add_log_listener(
                              LOG_GSM_RR_SIGNALING_MESSAGE_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_GSM_RR_SIGNALING_MESSAGE_C
                              ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    if (!diag_add_log_listener(
                              LOG_UMTS_CALL_FLOW_ANALYSIS_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_UMTS_CALL_FLOW_ANALYSIS_C
                              ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    if (!diag_add_log_listener(
                              LOG_GPRS_MAC_SIGNALLING_MESSAGE_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL 
                              ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
 
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    if (!diag_add_log_listener(
                              LOG_GSM_RR_CHANNEL_CONFIGURATION_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL 
                              ))
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  /*mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
  if (!diag_add_log_listener(
    LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C,
    mmal_ciq_metrics_sniff_diag_logs,
    NULL))
  {
    MMAL_CIQ_INCR_DIAG_ERR_CNT();
    MMAL_CIQ_INCR_ERR_CNT();
  }*/
  }
#endif /* !FEATURE_MMAL_DISABLE_GS3031 */

#ifndef FEATURE_MMAL_DISABLE_GS3031
  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_GS30_METRIC)))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered++;
    /*! this is for extracting the NECI bit */
    if (!diag_add_log_listener( LOG_GSM_RR_CELL_RESELECTION_PARAMS_C,
                                mmal_ciq_metrics_sniff_diag_logs,
                                NULL //LOG_GSM_RR_CELL_RESELECTION_PARAMS_C
                               ) )
    {
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
  }
#endif /* !FEATURE_MMAL_DISABLE_GS3031 */

  if ((mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered !=
      0) ||
    (mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered !=
      0))
  {
    uint8 request[] = {75, 18, 94, 0, 0, 0, 0, 0}; 
    /* disable diag aggregation */
    diag_cmd_req(
      request,sizeof(request)/sizeof(byte), mmal_ciq_fend_diag_cmd_rsp, NULL);
    mmal_ciq_metric_stats_info.metrics_info.num_diag_request++;
  }
}

/*===========================================================================

  FUNCTION:  mma_ciq_fend_deregister_diag_logs_events

===========================================================================*/
/*!
    @brief
    This function deregisters for the diag logs and events

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_deregister_diag_logs_events(void)
{
  boolean ret_val ;

#ifdef FEATURE_MMAL_LTE
  /*! Add the log packets for diag to intercept and pass it to the MMAL task */
  /*------------------------------------------------------------------------*/
  /*! METRIC LT01, LT02 */
  /*------------------------------------------------------------------------*/
   
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      MMAL_CIQ_LT02_METRIC))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    diag_remove_log_listener(
      LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C,
      mmal_ciq_metrics_sniff_diag_logs,
      NULL //LOG_LTE_RRC_OTA_MSG_LOG_C
    );
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    diag_remove_log_listener(
      LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C,
      mmal_ciq_metrics_sniff_diag_logs,
      NULL //LOG_LTE_RRC_OTA_MSG_LOG_C
    );   
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
  diag_remove_log_listener(
                              LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_LTE_RRC_OTA_MSG_LOG_C
                              );
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    diag_remove_log_listener(
      LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C,
      mmal_ciq_metrics_sniff_diag_logs,
      NULL //LOG_LTE_RRC_OTA_MSG_LOG_C
    ); 
  }
#endif

  /*------------------------------------------------------------------------*/
  /*! METRIC GS34, GS 35 */
  /*------------------------------------------------------------------------*/
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      (MMAL_CIQ_GS03_METRIC)) 
  )
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    /*! Metric GS01, GS02, GS03 => Log code 0x12C1 */
    diag_remove_log_listener(
       LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C, /*0x12C1 */
       mmal_ciq_metrics_sniff_diag_logs,
       NULL);
  }

  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
    (MMAL_CIQ_GS34_METRIC|MMAL_CIQ_GS35_METRIC|MMAL_CIQ_GS30_METRIC | 
    MMAL_CIQ_GS31_METRIC)))
  {

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
              LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C,
              mmal_ciq_metrics_sniff_diag_logs,
              NULL//LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C
             );


  }

  /*------------------------------------------------------------------------*/
  /*! METRIC GS30, GS31 */
  /*------------------------------------------------------------------------*/
  /*! Metric GS30, GS31 => Log code 0x512F */
  if (MMAL_CIQ_IF_METRIC_ENABLED(
                        mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                        (MMAL_CIQ_GS30_METRIC | MMAL_CIQ_GS31_METRIC)))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
              LOG_GSM_RR_SIGNALING_MESSAGE_C,
              mmal_ciq_metrics_sniff_diag_logs,
              NULL //LOG_GSM_RR_SIGNALING_MESSAGE_C
            );
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
                              LOG_GPRS_MAC_SIGNALLING_MESSAGE_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_GPRS_MAC_SIGNALLING_MESSAGE_C
                              );

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
                              LOG_GSM_RR_CHANNEL_CONFIGURATION_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_GSM_RR_CHANNEL_CONFIGURATION_C
                              );

    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
                              LOG_UMTS_CALL_FLOW_ANALYSIS_C,
                              mmal_ciq_metrics_sniff_diag_logs,
                              NULL //LOG_UMTS_CALL_FLOW_ANALYSIS_C
                              );
  }

  /*! Metric GS30, Extracting the NECI bit */
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      (MMAL_CIQ_GS30_METRIC  )))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener( LOG_GSM_RR_CELL_RESELECTION_PARAMS_C,
                                        mmal_ciq_metrics_sniff_diag_logs,
                                        NULL //LOG_GSM_RR_SIGNALING_MESSAGE_C
                                       );
    /*mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener( LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C,
                                        mmal_ciq_metrics_sniff_diag_logs,
                                        NULL );*/

  }

  
  /*------------------------------------------------------------------------*/
  /*! METRIC GS01, GS02, GS03 */
  /*------------------------------------------------------------------------*/
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      (MMAL_CIQ_GS01_METRIC|MMAL_CIQ_GS02_METRIC|MMAL_CIQ_GS03_METRIC)) 
  )
  {
    /*! Metric GS01, GS02, GS03 => Log code 0x12C1 */
    mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered--;
    ret_val = diag_remove_log_listener(
                                   LOG_CM_CALL_EVENT_C, /*0x12C1 */
                                   mmal_ciq_metrics_sniff_diag_logs,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   );

  /*ret_val = diag_remove_event_listener(
                                   CM_CALL_EVENT_CALL_ON_HOLD, 
                                   mmal_ciq_metrics_sniff_diag_events,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   );

    ret_val = diag_remove_event_listener(
                                   CM_CALL_EVENT_CALL_RETRIEVED, 
                                   mmal_ciq_metrics_sniff_diag_events,
                                   NULL //LOG_CM_CALL_EVENT_C
                                   );*/
 
  }
  /*---------------------------------------------------------------
  Removing the LT06 related events
  -----------------------------------------------------------------*/
#ifdef FEATURE_MMAL_LTE
  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_LT06_METRIC)))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered--;
    /*! this is for extracting the NECI bit */
    ret_val = diag_remove_event_listener( EVENT_LTE_RRC_STATE_CHANGE_TRIGGER,
                                          mmal_ciq_metrics_sniff_diag_events,
                                          NULL );
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered--;    
    ret_val = diag_remove_event_listener( EVENT_LTE_RRC_RADIO_LINK_FAILURE_STAT,
                                          mmal_ciq_metrics_sniff_diag_events,
                                          NULL );
  }

  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      MMAL_CIQ_LT10_METRIC))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered--;
    ret_val = diag_remove_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C,
                                        mmal_ciq_metrics_sniff_diag_logs,
                                        NULL );

    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered--;
    ret_val = diag_remove_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C,
                                        mmal_ciq_metrics_sniff_diag_logs,
                                        NULL );
  }


  if (MMAL_CIQ_IF_METRIC_ENABLED(
     mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
      MMAL_CIQ_LT11_METRIC))
  {
    mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered--;
    ret_val = diag_remove_log_listener( LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C,
                                        mmal_ciq_metrics_sniff_diag_logs,
                                        NULL );
  }

  if ((mmal_ciq_metric_stats_info.metrics_info.num_log_listener_cb_registered ==
      0) &&
      (mmal_ciq_metric_stats_info.metrics_info.num_event_listener_cb_registered ==
      0))
  {
    uint8 request[] = {75, 18, 94, 0, 1, 0, 0, 0}; 
    /* enable diag aggregation */
    diag_cmd_req(
      request,sizeof(request)/sizeof(byte), mmal_ciq_fend_diag_cmd_rsp, NULL);
    mmal_ciq_metric_stats_info.metrics_info.num_diag_request++;
  }
#endif
}


/*===========================================================================

  FUNCTION:  mmal_ciq_fend_update_profile

===========================================================================*/
/*!
    @brief
    This function updates the profiles needed

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_update_profile(void)
{
  cm_client_status_e_type   cm_result;
  int                       ps_retval;
  iq_result_t               iq_reg_retval = IQ_ERROR;

  /* deregister for the ones that are not needed any more */
  mmal_ciq_fend_deregister_diag_logs_events();

  /*--------------------------------------------------------------------*/
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT01,  
                               MMAL_CIQ_LT01_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT02,  
                               MMAL_CIQ_LT02_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT03,  
                               MMAL_CIQ_LT03_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT04,  
                               MMAL_CIQ_LT04_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT05,  
                               MMAL_CIQ_LT05_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT06,  
                               MMAL_CIQ_LT06_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT07,  
                               MMAL_CIQ_LT07_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT08,  
                               MMAL_CIQ_LT08_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT09,  
                               MMAL_CIQ_LT09_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT10,  
                               MMAL_CIQ_LT10_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT11,  
                               MMAL_CIQ_LT11_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT12,  
                               MMAL_CIQ_LT12_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_LT13,  
                               MMAL_CIQ_LT13_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS01, 
                               MMAL_CIQ_GS01_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS02,
                               MMAL_CIQ_GS02_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS03,
                               MMAL_CIQ_GS03_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS15,
                               MMAL_CIQ_GS15_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS18,
                               MMAL_CIQ_GS18_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS19,
                               MMAL_CIQ_GS19_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS30,
                               MMAL_CIQ_GS30_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS31,
                               MMAL_CIQ_GS31_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS34,
                               MMAL_CIQ_GS34_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS35,
                               MMAL_CIQ_GS35_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS46,
                               MMAL_CIQ_GS46_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS47,
                               MMAL_CIQ_GS47_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_GS6E,
                               MMAL_CIQ_GS6E_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_SS25,
                               MMAL_CIQ_SS25_METRIC);
  mmal_ciq_update_metric_bmask(IQ_METRIC_ID_SS2B,
                               MMAL_CIQ_SS2B_METRIC);


  /* Register for newly added ones */
  mmal_ciq_fend_register_diag_logs_events();

  /* Register for callbacks */

#ifndef FEATURE_MMAL_DISABLE_GS3435
  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_GS34_METRIC) | (MMAL_CIQ_GS35_METRIC)))
  {
#ifdef FEATURE_WCDMA
    /*! Register for the call back from W rrc for the W RRC related
    sig messages */
    rrc_register_mmal_cb(&mmal_ciq_metrics_wrrc_log_msg_process_cb);
#endif
  }
  else
  {
#ifdef FEATURE_WCDMA
    /*! De-Register for the call back from W rrc for the W RRC related
    sig messages */
    rrc_register_mmal_cb(NULL);
#endif
  }
#endif /* !FEATURE_MMAL_DISABLE_GS3435 */

  if (MMAL_CIQ_IF_METRIC_ENABLED(
      mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
     (MMAL_CIQ_LT01_METRIC)))
  {
    /*! Register for the call back from W rrc for the W RRC related
    sig messages */
#ifdef FEATURE_MMAL_LTE
    lte_rrc_log_register_mmal_cb(&mmal_ciq_metrics_lrrc_log_msg_process_cb);
#endif
  }
  else
  {
    /*! deregister for the call back from L rrc for the L RRC related
    sig messages */
#ifdef FEATURE_MMAL_LTE
    lte_rrc_log_register_mmal_cb(NULL);
#endif
  }

/* Set timers for selected messages */
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   MMAL_CIQ_LT03_METRIC ) )
  {
#ifdef FEATURE_MMAL_LTE
    timer_set( &lt03_timer, (timetick_type) 400, (timetick_type) 400, T_MSEC );
    MMAL_MSG( "MMAL: Set Timer for LT03" );
#endif
  }
  else 
  {
#ifdef FEATURE_MMAL_LTE
    timer_clr( &lt03_timer, T_NONE );
#endif
  }

  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   MMAL_CIQ_LT04_METRIC ) )
  {
#ifdef FEATURE_MMAL_LTE
    timer_set( &lt04_timer, (timetick_type) 400, (timetick_type) 400, T_MSEC );
    MMAL_MSG( "MMAL: Set Timer for LT04" );
#endif
  }
  else 
  {
#ifdef FEATURE_MMAL_LTE
    timer_clr( &lt04_timer, T_NONE );
#endif
  }

  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_LT05_METRIC ) )
  {
#ifdef FEATURE_MMAL_LTE
    MMAL_MSG( "MMAL: Set callbacks for LT05" );
    lte_rrc_log_register_mmal_cb_for_idle_meas( mmal_ciq_lte_metric_pack_and_submit_idle_msg_lt05 );
    lte_rrc_log_register_mmal_cb_for_conn_meas( mmal_ciq_lte_metric_pack_and_submit_conn_msg_lt05 );
#endif
  }
  else
  {
#ifdef FEATURE_MMAL_LTE
    MMAL_MSG( "MMAL: Clear callbacks for LT05" );
    lte_rrc_log_register_mmal_cb_for_idle_meas( NULL );
    lte_rrc_log_register_mmal_cb_for_conn_meas( NULL );
#endif
  }

  /* Set timers for selected messages */
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   MMAL_CIQ_LT07_METRIC ) )
  {
#ifdef FEATURE_MMAL_LTE
    timer_set( &lt07_timer, (timetick_type) 480, (timetick_type) 480, T_MSEC );
    MMAL_MSG( "MMAL: Set Timer for LT07" );
#endif
  }
  else 
  {
#ifdef FEATURE_MMAL_LTE
    timer_clr( &lt07_timer, T_NONE );
#endif
  }

  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                   MMAL_CIQ_LT09_METRIC ) )
  {
#ifdef FEATURE_MMAL_LTE
    timer_set( &lt09_timer, (timetick_type) 960, (timetick_type) 960, T_MSEC );
    MMAL_MSG( "MMAL: Set Timer for LT09" );
#endif
  }
  else
  {
#ifdef FEATURE_MMAL_LTE
    timer_clr( &lt09_timer, T_NONE );
#endif
  }

#ifndef FEATURE_MMAL_DISABLE_GS15
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_GS15_METRIC ) )
  {
    ps_retval = ps_iface_event_cback_reg( NULL, IFACE_UP_EV, gs15_iface_up_cb );
    MMAL_MSG_SPRINTF_1( "MMAL: GS15 CB registration attempt returned %d", ps_retval );
  }
  else
  {
    ps_iface_event_cback_dereg( NULL, IFACE_UP_EV, gs15_iface_up_cb );
    MMAL_MSG( "MMAL: GS15 CB deregistered" );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS15 */

#ifndef FEATURE_MMAL_DISABLE_GS18
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_GS18_METRIC ) )
  {
    ps_retval = ps_iface_event_cback_reg( NULL, IFACE_DOWN_EV, gs18_iface_down_cb );
    MMAL_MSG_SPRINTF_1( "MMAL: GS18 CB registration attempt returned %d", ps_retval );
  }
  else
  {
    ps_iface_event_cback_dereg( NULL, IFACE_DOWN_EV, gs18_iface_down_cb );
    MMAL_MSG( "MMAL: GS18 CB deregistered" );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS18 */

#if !defined(FEATURE_MMAL_DISABLE_GS46) || !defined(FEATURE_MMAL_DISABLE_GS47) || !defined(FEATURE_MMAL_DISABLE_GS6E)
  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_GS46_METRIC ) ||
       MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_GS47_METRIC ) ||
       MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_GS6E_METRIC ) )
  {
    cm_result = cm_mm_client_ss_reg( mmal_cm_client_id,
                                     mmal_ciq_cm_ss_event_srv_changed_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_SS_EVENT_SRV_CHANGED,
                                     CM_SS_EVENT_SRV_CHANGED,
                                     NULL );

    MMAL_ASSERT( CM_CLIENT_OK == cm_result );
    MMAL_MSG_ERROR( "MMAL: CM_SS_EVENT_SRV_CHANGED callback registered" );
  }
  else
  {
    cm_result = cm_mm_client_ss_reg( mmal_cm_client_id,
                                     mmal_ciq_cm_ss_event_srv_changed_cb,
                                     CM_CLIENT_EVENT_DEREG,
                                     CM_SS_EVENT_SRV_CHANGED,
                                     CM_SS_EVENT_SRV_CHANGED,
                                     NULL );

    MMAL_ASSERT( CM_CLIENT_OK == cm_result );
    MMAL_MSG_ERROR( "MMAL: CM_SS_EVENT_SRV_CHANGED callback deregistered" );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS46 || !FEATURE_MMAL_DISABLE_GS47 || !FEATURE_MMAL_DISABLE_GS6E */

  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_SS25_METRIC ) )
  {
    iq_reg_retval = IQ_RegisterQueriableMetric( IQ_METRIC_ID_SS25,
                                                mmal_ciq_ss25_query,
                                                NULL );

    if (iq_reg_retval == IQ_SUCCESS)
    {
      MMAL_MSG("MMAL: Registered for SS25");
    }
    else
    {
      MMAL_MSG("MMAL: Registration for SS25 failed!");
    }
  }

  if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask,
                                   MMAL_CIQ_SS2B_METRIC ) )
  {
    iq_reg_retval = IQ_RegisterQueriableMetric( IQ_METRIC_ID_SS2B,
                                                mmal_ciq_ss2b_query,
                                                NULL );

    if (iq_reg_retval == IQ_SUCCESS)
    {
      MMAL_MSG("MMAL: Registered for SS2B");
    }
    else
    {
      MMAL_MSG("MMAL: Registration for SS2B failed!");
    }
  }

}


/*===========================================================================

  FUNCTION:  mmal_ciq_fend_profile_changed_cb

===========================================================================*/
/*!
    @brief
    This function updates the profiles needed

    @detail

    @return

    @Note
   Event callback function prototype.  Any function that will be registered as 
   a callback  with  IQ_RegisterForEvent() or IQ_RegisterQueriableMetric()
   must conform to this  prototype.
*/
/*=========================================================================*/

void mmal_ciq_fend_profile_changed_cb
(
  void                          *callbackData,
  iq_event_type_t               event, 
  iq_event_param_t              eventParam,
  void                          *eventData, 
  unsigned long                 eventDataLen
)
{
  mmal_ciq_fend_update_profile();

  mmal_ciq_metric_stats_info.metrics_info.profile_update_cnt++;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_init

===========================================================================*/
/*!
    @brief
    This function initializes the IQ agent 

    @detail
    All the parameters that the metric submission code will use should also 
    be initialized here.

    @return
    TRUE indicates if the IQ agent was initialized successfully, FALSE
    otherwise

    @Note
    This function needs to be called prior to any metric submission related
    calls are made to the IQ agent.

*/
/*=========================================================================*/
boolean mmal_ciq_fend_init( void )
{
  iq_result_t               iq_res;
  boolean                   ret_val = FALSE;
  cm_client_status_e_type   cm_result;

  /*! Call the function to initialize the IQ agent */
  iq_res = IQ_Init();

  memset(&mmal_ciq_metric_stats_info, 0x0, sizeof(mmal_ciq_metric_stats_info));

  //mmal_ciq_wcdma_metric_init();

  /* Define Timers */
#ifdef FEATURE_MMAL_LTE
  timer_def( &lt03_timer, NULL, NULL, 0, mmal_ciq_lte_metric_lt03_timer_cb, NULL );
  timer_def( &lt04_timer, NULL, NULL, 0, mmal_ciq_lte_metric_lt04_timer_cb, NULL );
  timer_def( &lt07_timer, NULL, NULL, 0, mmal_ciq_lte_metric_request_data_lt07_cb, NULL );
  timer_def( &lt09_timer, NULL, NULL, 0, mmal_ciq_lte_metric_request_data_lt09_cb, NULL );
#endif
  
  /* Clear the context id map */
  mmal_ciq_metrics_wipe_context_id_map();

  /* Define Callbacks for PDP Context up and down */
  gs15_iface_up_cb = ps_iface_alloc_event_cback_buf( mmal_ciq_metrics_pdp_context_up_cb,
                                                     NULL );

  gs18_iface_down_cb = ps_iface_alloc_event_cback_buf( mmal_ciq_metrics_pdp_context_down_cb,
                                                       NULL );

  cm_result = cm_client_init( CM_CLIENT_TYPE_FLOATING1, &mmal_cm_client_id ); //CM_CLIENT_TYPE_MCS
  MMAL_MSG_SPRINTF_1( "cm_client_init returned %d\n", cm_result );
  MMAL_ASSERT( CM_CLIENT_OK == cm_result );

  cm_result = cm_client_act( mmal_cm_client_id );
  MMAL_MSG_SPRINTF_1( "cm_client_act returned %d\n", cm_result );
  MMAL_ASSERT( CM_CLIENT_OK == cm_result );

  /*! if the IQ agent initialized properly */
  if (iq_res == IQ_SUCCESS)
  {
    /*! @TODO - Add other initializations related to metric code here */
    mmal_ciq_fend_update_profile();

    if (IQ_RegisterForEvent(
                           IQ_EVENT_TYPE_PROFILE_CHANGED,
                           IQ_EVENT_PARAM_ANY,
                           mmal_ciq_fend_profile_changed_cb,
                           NULL) != IQ_SUCCESS)
    {
      MMAL_CIQ_INCR_CIQ_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    if (IQ_RegisterForEvent(
                           IQ_EVENT_TYPE_SERIAL_CONNECTION_STATE_CHANGED,
                           IQ_EVENT_PARAM_ANY,
                           mmal_ciq_fend_profile_changed_cb,
                           NULL)!= IQ_SUCCESS)
    {
      MMAL_CIQ_INCR_CIQ_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }

    ret_val = TRUE;
  }
  else
  {
    MMAL_CIQ_INCR_CIQ_ERR_CNT();
    MMAL_CIQ_INCR_ERR_CNT();
  }

  /*@TODO - What happens on a failure ? */
  return ret_val;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_shutdown

===========================================================================*/
/*!
    @brief
    This function shuts down the IQ agent 

    @detail
    
    @return
    VOID since the shutdown either completes or its a NOP for the IQ agent since
    the agent was not running earluier.
    
    @Note
    This function needs to be called after all the metric submission related
    calls are made to the IQ agent.

*/
/*=========================================================================*/
void mmal_ciq_fend_shutdown( void )
{
  iq_result_t     iq_res;

  /*! Call the function to shutdown the IQ agent */
  iq_res = IQ_Shutdown();

  /*! if the IQ agent initialized properly */
  if (iq_res == IQ_SUCCESS)
  {
    /*! @TODO - Add other cleanup related to metric code here */
    mmal_ciq_fend_deregister_diag_logs_events(); 
  }
  else
  {
    MMAL_CIQ_INCR_CIQ_ERR_CNT();
    MMAL_CIQ_INCR_ERR_CNT();
  }
}

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_process_log_listener_req

===========================================================================*/
/*!
    @brief
    Processes the events or logs that are intercepted
    and routed to appropriate metric handling code

    @detail
    
    @return
    
    
    @Note
    
*/
/*=========================================================================*/
void mmal_ciq_fend_process_log_listener_req
(
  const unsigned int              log_code,
  dsm_item_type              *log_info_ptr
)
{
//  uint16 data_size, status_byte;
  uint8  dir;
//  dsm_item_type *dsm_dup_ptr;
  
  MMAL_ASSERT (log_info_ptr);

  switch (log_code)
  {
#ifdef FEATURE_MMAL_LTE
  
    case LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C:
    case LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C:
    case LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C:
    case LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C:
      mmal_ciq_lte_metric_pack_msg_lt02(log_info_ptr, log_code);
      break;
    case EVENT_LTE_RRC_STATE_CHANGE_TRIGGER:
    case EVENT_LTE_RRC_RADIO_LINK_FAILURE_STAT:
      mmal_ciq_lte_metric_pack_and_submit_lt06(log_code,log_info_ptr);
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      break;
    case LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C:
      mmal_ciq_lte_metric_pack_and_submit_lt10(log_info_ptr, log_code);
      mmal_ciq_metrics_free_item( &log_info_ptr, __FILENAME__, __LINE__ );
      break;
    case LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C:
      if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                       MMAL_CIQ_LT10_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_lt10(log_info_ptr, log_code);
      }
      if ( MMAL_CIQ_IF_METRIC_ENABLED( mmal_ciq_fend_info.mmal_ciq_fend_enable_metric_mask, 
                                       MMAL_CIQ_LT11_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_lt11(log_info_ptr);
      }
      mmal_ciq_metrics_free_item( &log_info_ptr, __FILENAME__, __LINE__ );
      break;

#endif
    case LOG_GSM_RR_SIGNALING_MESSAGE_C:
      /*data_size = log_info_ptr->used;
      status_byte = 
              dsmi_dup_packet(&dsm_dup_ptr,
                             DSM_DS_LARGE_ITEM_POOL,
                             log_info_ptr,
                             0,
                             data_size, __FILENAME__, __LINE__);
      MMAL_ASSERT(status_byte == data_size); */

      mmal_ciq_gsm_metric_pack_and_submit_gs30(log_info_ptr);
      /*mmal_ciq_gsm_metric_pack_and_submit_gs31(dsm_dup_ptr); //log_info_ptr);*/
      mmal_ciq_fend_info.active_rat = GSM;
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      /*mmal_ciq_metrics_free_item(&dsm_dup_ptr, __FILENAME__, __LINE__);*/
      break;
    case LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C:
      dir = mmal_ciq_metric_extract_nas_ota_msg(&log_info_ptr);
      MSG_HIGH("MMAL NAS log ptr size %d", log_info_ptr->used,0,0);

      if (mmal_ciq_fend_info.active_rat == GSM)
      {
        MSG_HIGH("MMAL GSM log ptr size %d", log_info_ptr->used,0,0);
        mmal_ciq_gsm_metric_process_nas_msg(dir, &log_info_ptr);
      }
#ifdef FEATURE_WCDMA
      else if (mmal_ciq_fend_info.active_rat == WCDMA)
      {
        MSG_HIGH("MMAL WCDMA log ptr size %d", log_info_ptr->used,0,0);
        mmal_ciq_wcdma_metric_process_nas_msg(dir, &log_info_ptr);
      }
      break;
#endif
    case LOG_UMTS_CALL_FLOW_ANALYSIS_C:
      if (mmal_ciq_fend_info.active_rat == GSM)
      {
        mmal_ciq_gsm_metric_parse_cfa_pkt(log_info_ptr);
      }
      else
      {
        mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      }
      break;
    case LOG_GSM_RR_CELL_RESELECTION_PARAMS_C:
      mmal_ciq_gsm_metric_extract_neci_bit(log_info_ptr);
      break;
    case LOG_GPRS_MAC_SIGNALLING_MESSAGE_C:
      mmal_ciq_gsm_metric_pack_and_submit_gprs_gs30_gs31(log_info_ptr);
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      break;
  //case EVENT_RANDOM_ACCESS_REQUEST:
  /*case LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C:
    mmal_ciq_gsm_metric_pack_and_submit_rach_gs31(log_info_ptr);
    break;*/
    case LOG_GSM_RR_CHANNEL_CONFIGURATION_C:
      mmal_ciq_gsm_metric_get_channel_info(log_info_ptr);
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);    
      break;
    case LOG_CM_CALL_EVENT_C:
#ifndef FEATURE_MMAL_CIQ_DISABLE_GS01
      mmal_ciq_mmode_metric_pack_and_submit_gs01(log_info_ptr);
#endif
#ifndef FEATURE_MMAL_CIQ_DISABLE_GS02
      mmal_ciq_mmode_metric_pack_and_submit_gs02(log_info_ptr);
#endif    
#ifndef FEATURE_MMAL_CIQ_DISABLE_GS03
      mmal_ciq_mmode_metric_pack_and_submit_gs03(log_info_ptr);
#endif
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      break;
  /*case CM_CALL_EVENT_CALL_ON_HOLD:
  case CM_CALL_EVENT_CALL_ON_RETRIEVED:
    mmal_ciq_mmode_metric_pack_and_submit_gs02(log_code, log_info_ptr);
    mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
    break;   */
    case LOG_UMTS_CS_CALL_RELEASE_INFO_LOG_PACKET_C:
#ifndef FEATURE_MMAL_CIQ_DISABLE_GS03
      mmal_ciq_mmode_metric_save_release_cause_gs03(&log_info_ptr);
#endif
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      break;
  
    default:
      mmal_ciq_metrics_free_item(&log_info_ptr, __FILENAME__, __LINE__);
      MSG_HIGH("MMAL: Log Code 0x%x not handled", log_code,0,0);
      MMAL_CIQ_INCR_DIAG_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
      break;
  }
}

#endif /* FEATURE_MMAL*/
