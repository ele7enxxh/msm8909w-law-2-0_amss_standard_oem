/*=============================================================================
  @file sns_rh_internal_svc.c

  This file implements the service provided by SMGR for internal clients.

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc. 
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_internal_svc.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-04-04  pn   Must deactivate MD-disabled streams when receiving MD enable signal
  2016-03-08  pn   Changed MD state change request method
  2016-03-03  pn   Changed order of report queue searches when replacing a report
  2015-09-24  pn   Code cleanup
  2015-09-18  pn   Sets report interval when processing Buffering report requests
  2015-08-24  pn   Renamed sensor_event_q to holding_q
  2015-05-27  bd   Indication flow control in SMGR
  2015-05-11  pn   Delays depot selection for report until after setting ODR
  2015-04-21  pn   Not using event timestamp as starting point for gated streams 
                   activated immediately when added
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2015-03-25  pn   Uses sensor event timestamp as ts_last_sent for event gated streams
  2014-12-30  pn   - When MD is disabled, activates event gated streams only 
                   if it is still necessary
                   - Frees sensor_status_monitor requests in cancel_internal_service() 
  2014-12-19  pn   Updated conditions when sensor events are enabled or disabled
  2014-12-16  pn   Fixed memory leak in sns_rh_cancel_internal_service()
  2014-12-05  pn   Treats active Occurred reports as independent reports
  2014-12-03  pn   Properly cleans up event gated reports
  2014-11-24  jtl  Add support for timestamp accurate clients in monitor ind
  2014-11-04  pn   sns_rh_enable_sensor_event() takes DDF sensor as input
  2014-10-23  pn   Checks queue in each loop after deleting a queue element
  2014-10-02  pn   Initializes report periodic_like field
  2014-09-25  pn   Selects appropriate depot for report item
  2014-09-16  pn   Event gated reports can be deactivated and reactivated
  2014-08-13  sc   Fixed compilation warnings (remove unused variable)
  2014-08-08  pn   Replaced SMGR_U_ALLOC_STRUCT with SMGR_ANY_ALLOC_STRUCT
  2014-08-09  pn   Fixed bug in traversing sensors's client DB
  2014-07-21  pn   Added support for event_gated_buffering
  2014-06-11  vy   Use SNS_OS_U_MALLOC to support uImage
  2014-05-07  pn   Added support for MD deregistration
  2014-04-29  pn   Refactored to support microImage
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sensor1.h"
#include "sns_memmgr.h"
#include "sns_rh_internal_svc.h"
#include "sns_rh_main.h"
#include "sns_rh_main_uimg.h"
#include "sns_rh_sol.h"
#include "sns_rh_util.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TICK_MOD_MASK                      0xFFFFFFFF
#define __SNS_MODULE__ SNS_RH

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_rh_process_internal_svc_cancel_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_CANCEL_REQ_V02.

  @param[in]  msg_ptr   the request message
 
  @return     device-specific test error code
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_internal_svc_cancel_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_common_cancel_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_common_cancel_resp_msg_v01);

  /* -------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  sns_rh_cancel_internal_service(msg_ptr->header.connection_handle);
  msg_ptr->header.msg_id      = SNS_SMGR_CANCEL_RESP_V02;
  msg_ptr->header.body_len    = sizeof(sns_common_cancel_resp_msg_v01);
  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
  sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_internal_version_request

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_VERSION_REQ_V02.

  @detail Builds and sends response message with version of the
          SMGR Internal interface in use.

  @param[i]  msg_ptr - the request message 

  @return  SNS_SUCCESS or SNS_ERR_NOMEM
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_internal_version_request(
  const sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_err_code_e err_code = SNS_ERR_NOMEM;
  sns_common_version_resp_msg_v01* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_common_version_resp_msg_v01);
  if ( resp_ptr != NULL )
  {
    sns_rh_mr_header_s resp_msg_header = msg_ptr->header;

    resp_ptr->max_message_id = SNS_SMGR_INTERNAL_SVC_V02_MAX_MESSAGE_ID;
    resp_ptr->interface_version_number = SNS_SMGR_INTERNAL_SVC_V02_IDL_MINOR_VERS;
    resp_msg_header.msg_id   = SNS_SMGR_VERSION_RESP_V02;
    resp_msg_header.body_len = sizeof(sns_common_version_resp_msg_v01);
    resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
    sns_rh_mr_send_resp(&resp_msg_header, resp_ptr);
    err_code = SNS_SUCCESS;
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_sensor_events_query_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_SENSOR_EVENTS_QUERY_REQ_V02

  @detail

  @param[i]  msg_ptr - the message
 
  @return  SNS_SUCCESS or SNS_ERR_NOMEM
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_sensor_events_query_request(
  const sns_rh_mr_req_q_item_s* msg_ptr)
{
  uint32_t e;
  sns_smgr_sensor_events_query_resp_msg_v02* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_sensor_events_query_resp_msg_v02);
  sns_rh_mr_header_s resp_msg_header = msg_ptr->header;
  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
  resp_ptr->sensor_events_valid = true;
  resp_ptr->sensor_events_len   = 0;
  for ( e=0; e<ARR_SIZE(sns_rh.sensor_events) && 
             sns_rh.sensor_events[e].sensor_event != 0; e++ )
  {
    SNS_SMGR_PRINTF1(
      LOW, "events_query_req - event(0x%x)", 
      (unsigned)sns_rh.sensor_events[e].sensor_event);

    resp_ptr->sensor_events[resp_ptr->sensor_events_len++] =
      sns_rh.sensor_events[e].sensor_event;
  }
  SNS_SMGR_PRINTF1(
    HIGH, "events_query_req - #events(%u)", (unsigned)resp_ptr->sensor_events_len);

  resp_msg_header.msg_id = SNS_SMGR_SENSOR_EVENTS_QUERY_RESP_V02;
  resp_msg_header.body_len = sizeof(sns_smgr_sensor_events_query_resp_msg_v02);
  sns_rh_mr_send_resp(&resp_msg_header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_generate_sensor_event_ind

===========================================================================*/
/*!
  @brief Sends sensor event indication to each registered client.

  @param[i/o] se_ptr - the event structure
 
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_generate_sensor_event_ind(const sns_rh_sensor_event_s* se_ptr)
{
  sns_smgr_sensor_event_ind_msg_v02* ind_ptr =
    &sns_rh.infrequent_ind.sensor_event;
  sns_rh_mr_req_q_item_s* q_item_ptr;

  ind_ptr->timestamp    = se_ptr->last_event_ts;
  ind_ptr->sensor_event = se_ptr->sensor_event;
  ind_ptr->event_status = se_ptr->status;
  ind_ptr->event_status_valid = true;
  SMGR_FOR_EACH_Q_ITEM( (sns_q_s*)&se_ptr->request_queue, q_item_ptr, q_link )
  {
    SNS_SMGR_PRINTF4(
      HIGH, "sensor_event_ind - conn_hndl=0x%x ev=x%08x ts=%u status=%u", 
      q_item_ptr->header.connection_handle, se_ptr->sensor_event, 
      se_ptr->last_event_ts, se_ptr->status);

    sns_rh_mr_send_indication(q_item_ptr->header.connection_handle,
                              ind_ptr,
                              SNS_SMGR_SENSOR_EVENT_IND_V02,
                              sizeof(*ind_ptr),
                              false); /* do not free ind msg */
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_find_sensor_event_request

===========================================================================*/
/*!
  @brief Searches in given queue for a particular request and
         removes it if necessary.

  @detail

  @param[i/o] se_ptr - the event whose request queue is to be searched
  @param[i]   in_msg_ptr - the request whose connection handle is key
  @param[i]   remove - if TRUE the found request shoudl be removed from queue
 
  @return  NULL, or the found request
 */
/*=========================================================================*/
SMGR_STATIC sns_rh_mr_req_q_item_s* sns_rh_find_sensor_event_request(
  sns_rh_sensor_event_s*        se_ptr,
  const sns_rh_mr_req_q_item_s* in_msg_ptr,
  bool                          remove)
{
  sns_rh_mr_req_q_item_s* found_q_item_ptr = NULL;
  if ( in_msg_ptr->request.sensor_event.sensor_event == se_ptr->sensor_event )
  {
    sns_rh_mr_req_q_item_s* q_item_ptr;

    SNS_SMGR_PRINTF1(
      MED, "find_sensor_ev_req - #queued_req=%u", sns_q_cnt(&se_ptr->request_queue));

    SMGR_FOR_EACH_Q_ITEM( &se_ptr->request_queue, q_item_ptr, q_link )
    {
      if ( q_item_ptr->header.connection_handle == 
           in_msg_ptr->header.connection_handle )
      {
        found_q_item_ptr = q_item_ptr;
        if ( remove )
        {
          sns_q_delete(&q_item_ptr->q_link);
        }
        break;
      }
    }
  }
  return found_q_item_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_add_sensor_event_request

===========================================================================*/
/*!
  @brief Queues the sensor event request and initiates the SMGR function that
         will trigger the event

  @param[i]   sensor_event - the event of interest
 
  @return SNS_SUCCESS or SNS_ERR_BAD_PARM
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_add_sensor_event_request(
  sns_rh_sensor_event_s*  se_ptr,
  sns_rh_mr_req_q_item_s* in_msg_ptr)
{
  sns_err_code_e err_code = SNS_SUCCESS;

  sns_q_put(&se_ptr->request_queue, &in_msg_ptr->q_link);
  if ( sns_q_cnt(&se_ptr->request_queue) == 1 )
  {
    se_ptr->last_event_ts = 0;
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_rh_find_sensor_event

===========================================================================*/
/*!
  @brief Searches for the given sensor event in the list of supported events

  @detail

  @param[i]   sensor_event - the event of interest
 
  @return  NULL, or the found event structure
 */
/*=========================================================================*/
SMGR_STATIC sns_rh_sensor_event_s* sns_rh_find_sensor_event(uint64_t sensor_event)
{
  sns_rh_sensor_event_s* se_ptr = NULL;
  uint32_t e = 0;
  while ( sns_rh.sensor_events[e].sensor_event != 0 )
  {
    if ( sensor_event == sns_rh.sensor_events[e].sensor_event )
    {
      se_ptr = &sns_rh.sensor_events[e];
      break;
    }
    e++;
  }
  return se_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_find_request

===========================================================================*/
/*!
  @brief Searches in given queue for a particular request and
         removes it if necessary.

  @detail

  @param[i/o] request_queue_ptr - the queue in which to search
  @param[i]   conn_handle       - the connection handle for which to search
  @param[i]   remove - if TRUE the found request shoudl be removed from queue
 
  @return  NULL, or the found request
 */
/*=========================================================================*/
SMGR_STATIC sns_rh_mr_req_q_item_s* sns_rh_find_request(
  sns_q_s*    request_queue_ptr,
  const void* conn_handle,
  bool        remove)
{
  sns_rh_mr_req_q_item_s* found_q_item_ptr = NULL;
  sns_rh_mr_req_q_item_s* q_item_ptr;

  SMGR_FOR_EACH_Q_ITEM( request_queue_ptr, q_item_ptr, q_link )
  {
    if ( q_item_ptr->header.connection_handle == conn_handle )
    {
      found_q_item_ptr = q_item_ptr;
      if ( remove )
      {
        sns_q_delete(&q_item_ptr->q_link);
      }
      break;
    }
  }
  return found_q_item_ptr;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_sensor_event_request

===========================================================================*/
/*!
  @brief Processes SNS_SMGR_SENSOR_EVENT_REQ_V02

  @detail

  @param[i]  msg_ptr - the message
 
  @return  SNS_SUCCESS or SNS_ERR_NOMEM
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_sensor_event_request(
  sns_rh_mr_req_q_item_s** msg_ptr_ptr)
{
  sns_rh_mr_req_q_item_s* msg_ptr = *msg_ptr_ptr;
  sns_smgr_sensor_event_req_msg_v02* req_ptr = &msg_ptr->request.sensor_event;
  sns_smgr_sensor_event_resp_msg_v02* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_sensor_event_resp_msg_v02);
  sns_rh_mr_header_s resp_msg_header = msg_ptr->header;
  sns_rh_sensor_event_s* se_ptr = NULL;

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  SNS_SMGR_PRINTF3(
    HIGH, "sensor_event_req - event(0x%08x%08x) reg(%d)", 
    req_ptr->sensor_event >> 32, req_ptr->sensor_event, req_ptr->registering);

  resp_ptr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_EBAD_PARAM;

  se_ptr = sns_rh_find_sensor_event(req_ptr->sensor_event);
  if ( se_ptr != NULL )
  {
    sns_rh_mr_req_q_item_s* existing_req_ptr = 
      sns_rh_find_sensor_event_request(se_ptr, msg_ptr, true);
    if ( req_ptr->registering )
    {
      sns_rh_add_sensor_event_request(se_ptr, msg_ptr);
      *msg_ptr_ptr = NULL;

      resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
    }
    else if ( existing_req_ptr != NULL )
    {
      resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
    }
    if ( existing_req_ptr != NULL )
    {
      SNS_OS_FREE(existing_req_ptr);
    }
  }

  resp_msg_header.msg_id = SNS_SMGR_SENSOR_EVENT_RESP_V02;
  resp_msg_header.body_len = sizeof(sns_smgr_sensor_event_resp_msg_v02);
  sns_rh_mr_send_resp(&resp_msg_header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_sensor_status_monitor_request

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_SENSOR_STATUS_MONITOR_REQ_V02.
 
  @details
 
  @param [i] msg_ptr_ptr - reference to the request message

  @return  SNS_ERR_NOMEM or SNS_SUCCESS
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_sensor_status_monitor_request(
  sns_rh_mr_req_q_item_s** msg_ptr_ptr)
{
  sns_rh_mr_req_q_item_s* msg_ptr = *msg_ptr_ptr;
  sns_smgr_sensor_status_monitor_req_msg_v02* req_ptr = 
    &msg_ptr->request.sensor_status_monitor;
  sns_smgr_sensor_status_monitor_resp_msg_v02* resp_ptr =
    SMGR_ALLOC_STRUCT(sns_smgr_sensor_status_monitor_resp_msg_v02);
  sns_rh_mr_header_s resp_msg_header = msg_ptr->header;

  /* HACKS */
  uint8_t sensor_id = (uint8_t)req_ptr->sensor_id;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = 
    sns_smgr_find_ddf_sensor(sensor_id, SNS_SMGR_DATA_TYPE_PRIMARY_V01);

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  SNS_SMGR_PRINTF2(
    HIGH, "status_monitor_req - sensor(0x%x) reg(%d)", 
    req_ptr->sensor_id, req_ptr->registering);

  resp_ptr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_EBAD_PARAM;
  resp_ptr->sensor_id_valid   = true;
  resp_ptr->sensor_id         = req_ptr->sensor_id;

  if ( ddf_sensor_ptr != NULL )
  {
    sns_q_s* q_ptr = &ddf_sensor_ptr->client_stat.sensor_status.request_queue;
    sns_rh_mr_req_q_item_s* existing_req_ptr = 
      sns_rh_find_request(q_ptr, msg_ptr->header.connection_handle, true);
    if ( msg_ptr->request.sensor_status_monitor.registering )
    {
      sns_q_put(q_ptr, &msg_ptr->q_link);
      *msg_ptr_ptr = NULL;
      resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
    }
    else if ( existing_req_ptr != NULL )
    {
      resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
    }
    if ( existing_req_ptr != NULL )
    {
      SNS_OS_FREE(existing_req_ptr);
    }
  }

  resp_msg_header.msg_id = SNS_SMGR_SENSOR_STATUS_MONITOR_RESP_V02;
  resp_msg_header.body_len = sizeof(sns_smgr_sensor_status_monitor_resp_msg_v02);
  sns_rh_mr_send_resp(&resp_msg_header, resp_ptr);

  if ( ddf_sensor_ptr != NULL )
  {
    sns_rh_generate_sensor_status_ind(ddf_sensor_ptr, msg_ptr);
  }
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_parse_report_request

===========================================================================*/
/*!
  @brief Validates the event_gated_buffering request
 
  @param [i] hdr_ptr - message header
  @param [i] req_ptr - the request
  @param [i] eg_req_ptr - an event_gated request item
  @param [i] q16_rpt_rate - the report rate for the event_gated item
  @param [o] rpt_ptr_ptr - the resulting report specs
  @param [o] resp_ptr - the response

  @return  SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 if all goes well,
           SNS_SMGR_RESPONSE_NAK_* if something goes wrong
 */
/*=========================================================================*/
SMGR_STATIC uint8_t sns_rh_parse_report_request(
  const sns_rh_mr_header_s*                         hdr_ptr,
  const sns_smgr_event_gated_buffering_req_msg_v02* req_ptr,
  const sns_smgr_buffering_req_item_s_v01*          eg_req_ptr,
  uint32_t                                          q16_rpt_rate,
  sns_rh_rpt_spec_s**                               rpt_ptr_ptr,
  sns_smgr_event_gated_buffering_resp_msg_v02*      resp_ptr,
  uint8_t                                           item_idx)
{
  sns_rh_rpt_spec_s* rpt_ptr = NULL;
  sns_rh_rpt_item_s* item_ptr = NULL;
  uint8_t ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  /* -------------------------------------------------------------------- */

  SNS_SMGR_PRINTF2(
    HIGH, "parse_report_request - rr=0x%x sr=%u", 
    q16_rpt_rate, eg_req_ptr->SamplingRate);

  if ( q16_rpt_rate == 0 ) /* client wants no reports in this event state */
  {
    return SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }
  if ( q16_rpt_rate > sns_smgr_rate_hz_to_q16(eg_req_ptr->SamplingRate) )
  {
    return SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01;
  }
  rpt_ptr  = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_spec_s);
  item_ptr = SMGR_ANY_ALLOC_STRUCT(sns_rh_rpt_item_s);

  if ( rpt_ptr == NULL || item_ptr == NULL )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
  }
  else if ( !sns_rh_sol_parse_buffering_item(
               item_idx, eg_req_ptr, item_ptr, 
               &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair) )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_INTERNAL_ERR_V01;
  }

  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    item_ptr->parent_report_ptr = rpt_ptr;
    rpt_ptr->item_list[rpt_ptr->num_items++] = item_ptr;

    rpt_ptr->rpt_id    = req_ptr->ReportId;
    rpt_ptr->proc_type = req_ptr->NotifySuspend.proc_type;
    rpt_ptr->send_ind_during_suspend = 
      req_ptr->NotifySuspend.send_indications_during_suspend;
    rpt_ptr->q16_rpt_rate = q16_rpt_rate;
    rpt_ptr->rpt_interval = sns_smgr_get_sched_intval(rpt_ptr->q16_rpt_rate);
    sns_rh_update_periodic_like_flag(rpt_ptr);

    rpt_ptr->msg_header = *hdr_ptr;
    sns_q_link(rpt_ptr, &rpt_ptr->rpt_link);
    *rpt_ptr_ptr = rpt_ptr;
  }
  else 
  {
    if ( rpt_ptr != NULL )
    {
      SNS_OS_ANY_FREE(rpt_ptr);
    }
    if ( item_ptr != NULL )
    {
      SNS_OS_ANY_FREE(item_ptr);
    }
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:  sns_rh_internal_svc_delete_report

===========================================================================*/
/*!
  @brief Searches given queue for report with given ID and connection handle
         and deletes it.

  @param[i]  hdr_ptr   - message header of new request
  @param[i]  report_id - report ID to match
  @param[i]  q_ptr     - the queue in which to search

  @return true if report was found and deleted
 */
/*=========================================================================*/
SMGR_STATIC bool sns_rh_internal_svc_delete_report(
  const void* conn_handle,
  uint8_t     report_id,
  sns_q_s*    q_ptr)
{
  bool found = false;
  sns_rh_rpt_spec_s* rpt_ptr;
  /* ---------------------------------------------------------------------- */

  SMGR_FOR_EACH_Q_ITEM(q_ptr, rpt_ptr, rpt_link)
  {
    if ( (conn_handle == rpt_ptr->msg_header.connection_handle) &&
         (report_id == rpt_ptr->rpt_id) )
    {
      sns_q_delete(&rpt_ptr->rpt_link);
      SNS_OS_ANY_FREE(rpt_ptr->item_list[0]);
      SNS_OS_ANY_FREE(rpt_ptr);
      found = true;
      break;
    }
  }
  return found;
}

/*===========================================================================

  FUNCTION:   sns_rh_add_event_gated_buffering_request

===========================================================================*/
SMGR_STATIC uint8_t sns_rh_add_event_gated_buffering_request(
  sns_rh_mr_header_s*                          hdr_ptr,
  sns_smgr_event_gated_buffering_req_msg_v02*  req_ptr,
  sns_rh_sensor_event_s*                       se_ptr,
  sns_smgr_event_gated_buffering_resp_msg_v02* resp_ptr )
{
  uint8_t ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  sns_rh_rpt_spec_s* e_rpt_ptr = NULL;
  sns_rh_rpt_spec_s* d_rpt_ptr = NULL;
  sns_rh_rpt_spec_s* o_rpt_ptr = NULL;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF4(
    HIGH, "add_event_gated_req - rr(e/d/o)=0x%x/0x%x/0x%x event_enabled=%u", 
    req_ptr->EventEnabledReportRate, req_ptr->EventDisabledReportRate,
    req_ptr->EventOccurredReportRate, se_ptr->enabled);

  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    ackNak = sns_rh_parse_report_request(
      hdr_ptr, req_ptr, &req_ptr->EventDisabledConfig, 
      req_ptr->EventDisabledReportRate, &d_rpt_ptr, resp_ptr, 0);
    SNS_SMGR_PRINTF1(
      HIGH, "add_event_gated_req - EventDisableConfig result=%u", ackNak);
  }
  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    ackNak = sns_rh_parse_report_request(
      hdr_ptr, req_ptr, &req_ptr->EventEnabledConfig, 
      req_ptr->EventEnabledReportRate, &e_rpt_ptr, resp_ptr, 1);
    SNS_SMGR_PRINTF1(
      HIGH, "add_event_gated_req - EventEnableConfig result=%u", ackNak);
  }
  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    ackNak = sns_rh_parse_report_request(
      hdr_ptr, req_ptr, &req_ptr->EventOccurredConfig, 
      req_ptr->EventOccurredReportRate, &o_rpt_ptr, resp_ptr, 2);
    SNS_SMGR_PRINTF1(
      HIGH, "add_event_gated_req - EventOccurredConfig result=%u", ackNak);
  }

  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    sns_rh_internal_svc_delete_report(
      hdr_ptr->connection_handle, req_ptr->ReportId, &se_ptr->enabled_state_rpt_q);
    sns_rh_internal_svc_delete_report(
      hdr_ptr->connection_handle, req_ptr->ReportId, &se_ptr->disabled_state_rpt_q);
    sns_rh_internal_svc_delete_report(
      hdr_ptr->connection_handle, req_ptr->ReportId, &se_ptr->occurred_state_rpt_q);
    sns_rh_sol_find_and_delete_report(
      hdr_ptr->connection_handle, req_ptr->ReportId, false);

    if ( o_rpt_ptr != NULL )
    {
      /* this report can only be restarted by client, no need to prepare for restart */
      sns_q_put(&se_ptr->occurred_state_rpt_q, &o_rpt_ptr->rpt_link);
    }
    if ( se_ptr->enabled )
    {
      if ( e_rpt_ptr != NULL )
      {
        e_rpt_ptr->holding_q = &se_ptr->enabled_state_rpt_q;
        sns_rh_sol_schedule_report(hdr_ptr, e_rpt_ptr);
      }
      if ( d_rpt_ptr != NULL )
      {
        d_rpt_ptr->holding_q = &se_ptr->disabled_state_rpt_q;
        sns_q_put(&se_ptr->disabled_state_rpt_q, &d_rpt_ptr->rpt_link);
      }
    }
    else /* event is disabled */
    {
      if ( e_rpt_ptr != NULL )
      {
        e_rpt_ptr->holding_q = &se_ptr->enabled_state_rpt_q;
        sns_q_put(&se_ptr->enabled_state_rpt_q, &e_rpt_ptr->rpt_link);
      }
      if ( d_rpt_ptr != NULL )
      {
        d_rpt_ptr->holding_q = &se_ptr->disabled_state_rpt_q;
        sns_q_put(&se_ptr->disabled_state_rpt_q, &d_rpt_ptr->rpt_link);
        sns_rh_update_sensor_event(se_ptr->ddf_sensor_ptr);
      }
    }
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:  sns_rh_disable_sensor_event

===========================================================================*/
/*!
  @brief Disables the given sensor event if the condition is right.

  @param[i]  se_ptr    - the sensor event to disable

  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_rh_disable_sensor_event(sns_rh_sensor_event_s* se_ptr)
{
  // PEND: Determine if/how MD-interrupt will work concurrently with an active DAF request
  if ( se_ptr->sensor_event == SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02 )
  {
    if ( (sns_q_cnt(&se_ptr->occurred_state_rpt_q) == 0) &&
         (sns_q_cnt(&se_ptr->disabled_state_rpt_q) == 0) &&
         (sns_q_cnt(&se_ptr->enabled_state_rpt_q) == 0) )
    {
      SNS_SMGR_PRINTF0(HIGH, "disable_sensor_event - disable MD");
      sns_smgr_set_md_state(false);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_delete_event_gated_buffering_request

===========================================================================*/
SMGR_STATIC uint8_t sns_rh_delete_event_gated_buffering_request(
  sns_rh_mr_header_s*                          hdr_ptr,
  sns_smgr_event_gated_buffering_req_msg_v02*  req_ptr,
  sns_rh_sensor_event_s*                       se_ptr)
{
  uint8_t ackNak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF3(
    MED, "delete_event_gated_req - ddf_sensor=%u #rpts=%u #e/d/o_rpts=0x%03x",
    SMGR_SENSOR_ID(se_ptr->ddf_sensor_ptr->sensor_ptr),
    se_ptr->ddf_sensor_ptr->client_stat.num_rpt_items,
    (sns_q_cnt(&se_ptr->enabled_state_rpt_q)  << 8) |
    (sns_q_cnt(&se_ptr->disabled_state_rpt_q) << 4) |
    sns_q_cnt(&se_ptr->occurred_state_rpt_q));

  sns_rh_sol_find_and_delete_report(hdr_ptr->connection_handle, 
                                    req_ptr->ReportId, false);

  if ( sns_rh_internal_svc_delete_report(hdr_ptr->connection_handle, 
                                         req_ptr->ReportId, 
                                         &se_ptr->enabled_state_rpt_q) )
  {
    SNS_SMGR_PRINTF0(MED, "delete_event_gated_req - enabled_rpt");
    ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }

  if ( sns_rh_internal_svc_delete_report(hdr_ptr->connection_handle, 
                                         req_ptr->ReportId, 
                                         &se_ptr->disabled_state_rpt_q) )
  {
    SNS_SMGR_PRINTF0(MED, "delete_event_gated_req - disabled_rpt");
    ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }

  if ( sns_rh_internal_svc_delete_report(hdr_ptr->connection_handle, 
                                         req_ptr->ReportId, 
                                         &se_ptr->occurred_state_rpt_q) )
  {
    SNS_SMGR_PRINTF0(MED, "delete_event_gated_req - occurred_rpt");
    ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  }

  if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    sns_rh_disable_sensor_event(se_ptr);
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_event_gated_buffering_request

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_EVENT_GATED_BUFFERING_REQ_V02.
 
  @details
 
  @param [i] msg_ptr_ptr - reference to the request message

  @return  SNS_ERR_NOMEM or SNS_SUCCESS
 */
/*=========================================================================*/
SMGR_STATIC sns_err_code_e sns_rh_process_event_gated_buffering_request(
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_smgr_event_gated_buffering_req_msg_v02*  req_ptr = 
    &msg_ptr->request.event_gated_buffering_report;
  sns_smgr_event_gated_buffering_resp_msg_v02* resp_ptr = 
    SMGR_ALLOC_STRUCT(sns_smgr_event_gated_buffering_resp_msg_v02);
  sns_rh_sensor_event_s* se_ptr = sns_rh_find_sensor_event(req_ptr->SensorEvent);
  /* ---------------------------------------------------------------------- */

  if ( resp_ptr == NULL )
  {
    return SNS_ERR_NOMEM;
  }

  SNS_SMGR_PRINTF3(
    HIGH, "event_gated_req - rpt_id=%u/0x%x action=%d",
    req_ptr->ReportId, msg_ptr->header.connection_handle, req_ptr->Action);
  SNS_SMGR_PRINTF2(
    HIGH, "event_gated_req - ev=0x%08x%08x",
    req_ptr->SensorEvent >> 32, req_ptr->SensorEvent);

  resp_ptr->ReportId_valid    = true;
  resp_ptr->ReportId          = req_ptr->ReportId;
  resp_ptr->AckNak_valid      = true;
  if ( se_ptr != NULL )
  {
    if ( req_ptr->Action == SNS_SMGR_EVENT_GATED_BUFFERING_ACTION_ADD_V02 )
    {
      resp_ptr->AckNak = sns_rh_add_event_gated_buffering_request(
        &msg_ptr->header, req_ptr, se_ptr, resp_ptr);
    }
    else
    {
      resp_ptr->AckNak = sns_rh_delete_event_gated_buffering_request(
        &msg_ptr->header, req_ptr, se_ptr);
    }
  }
  else
  {
    resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_SENSOR_ID_V01;
  }

  if ( resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    sns_rh_flush_reports();
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
    resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  }
  else
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_ENOTALLOWED;
  }

  msg_ptr->header.msg_id   = SNS_SMGR_EVENT_GATED_BUFFERING_RESP_V02;
  msg_ptr->header.body_len = sizeof(sns_smgr_event_gated_buffering_resp_msg_v02);
  sns_rh_mr_send_resp(&msg_ptr->header, resp_ptr);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_rh_process_md_int_enabled_sig

===========================================================================*/
/*!
  @brief Processes SNS_RH_MD_INT_ENABLED_SIG
  @param   none
  @return  none
 */
/*=========================================================================*/
void sns_rh_process_md_int_enabled_sig(void)
{
  sns_rh_sensor_event_s* se_ptr = 
    sns_rh_find_sensor_event(SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02);

  SNS_SMGR_PRINTF0(HIGH, "md_int_enabled_sig");

  if ( se_ptr != NULL )
  {
    sns_rh_rpt_spec_s* rpt_ptr;

    se_ptr->status = SNS_SMGR_SENSOR_EVENT_STATUS_ENABLED_V02;
    se_ptr->last_event_ts = sns_ddf_get_timestamp();
    se_ptr->enabled = false;
    sns_rh_generate_sensor_event_ind(se_ptr);

    SNS_SMGR_PRINTF2(HIGH, "md_int_enabled_sig - now=%u #en_rpt=%u", 
                     se_ptr->last_event_ts, sns_q_cnt(&se_ptr->enabled_state_rpt_q));

    if ( se_ptr->ddf_sensor_ptr->client_stat.num_rpt_items > 0 )
    {
      /* This is the scenario in which a new independent report prompts RH to activate
         MD-disabled reports while SMGR is in the process of enabling MD. RH will
         disable MD interrupt but MD interrupt could fire before it gets disabled.
         If MD interrupt fires now RH will try to activated the MD-occured reports which
         would collide with these MD-disabled reports as they would have the same
         report IDs.  To avoid the collision RH must deactivate MD-disabled reports now
         and re-activate them when SMGR informs RH that MD interrupt has been disabled. */

      sns_rh_rpt_item_s* item_ptr = se_ptr->ddf_sensor_ptr->client_stat.rpt_item_ptr;
      while ( item_ptr != NULL )
      {
        sns_rh_rpt_item_s* cur_item_ptr = item_ptr;
        item_ptr = item_ptr->next_item_ptr;
        if ( cur_item_ptr->parent_report_ptr->holding_q == &se_ptr->disabled_state_rpt_q )
        {
          sns_rh_sol_deactivate_report(cur_item_ptr->parent_report_ptr);
        }
      }
    }

    while ( (rpt_ptr = sns_q_check(&se_ptr->enabled_state_rpt_q)) != NULL )
    {
      uint8_t  i;
      RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
      {
        rpt_ptr->item_list[i]->ts_last_sent = se_ptr->last_event_ts;
      }
      sns_q_delete(&rpt_ptr->rpt_link);
      sns_rh_sol_schedule_report(&rpt_ptr->msg_header, rpt_ptr);
    }
    se_ptr->enabled = true;
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_process_md_int_disabled_sig

===========================================================================*/
/*!
  @brief Processes SNS_RH_MD_INT_DISABLED_SIG
  @param   none
  @return  none
 */
/*=========================================================================*/
void sns_rh_process_md_int_disabled_sig(void)
{
  sns_rh_sensor_event_s* se_ptr = 
    sns_rh_find_sensor_event(SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02);

  SNS_SMGR_PRINTF1(HIGH, "md_int_disabled_sig - se_ptr=%x", se_ptr);

  if ( se_ptr != NULL )
  {
    se_ptr->status = SNS_SMGR_SENSOR_EVENT_STATUS_DISABLED_V02;
    se_ptr->last_event_ts = sns_ddf_get_timestamp();
    se_ptr->enabled = false;
    sns_rh_generate_sensor_event_ind(se_ptr);

    SNS_SMGR_PRINTF2(HIGH, "md_int_disabled_sig - now=%u #dis_rpt=%u", 
                     se_ptr->last_event_ts, sns_q_cnt(&se_ptr->disabled_state_rpt_q));
    sns_rh_update_sensor_event(se_ptr->ddf_sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_process_md_int_sig

===========================================================================*/
/*!
  @brief Processes SNS_RH_MD_INT_SIG
  @param   none
  @return  none
 */
/*=========================================================================*/
void sns_rh_process_md_int_sig(void)
{
  sns_rh_sensor_event_s* se_ptr = 
    sns_rh_find_sensor_event(SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02);
  if ( se_ptr != NULL )
  {
    if ( !se_ptr->enabled )
    {
      SNS_SMGR_PRINTF0(ERROR, "MD int was enabled?!");
    }
    else
    {
      sns_rh_rpt_spec_s* rpt_ptr;

      se_ptr->status = SNS_SMGR_SENSOR_EVENT_STATUS_OCCURRED_V02;
      se_ptr->last_event_ts = sns_ddf_get_timestamp();
      se_ptr->enabled = false;
      sns_rh_generate_sensor_event_ind(se_ptr);

      SNS_SMGR_PRINTF2(HIGH, "md_int_sig - now=%u #occ_rpt=%u", 
                       se_ptr->last_event_ts, sns_q_cnt(&se_ptr->occurred_state_rpt_q));
      while ( (rpt_ptr = sns_q_check(&se_ptr->occurred_state_rpt_q)) != NULL )
      {
        uint8_t  i;
        RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
        {
          rpt_ptr->item_list[i]->ts_last_sent = se_ptr->last_event_ts;
        }
        sns_q_delete(&rpt_ptr->rpt_link);
        sns_rh_sol_schedule_report(&rpt_ptr->msg_header, rpt_ptr);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_process_internal_smgr_request

===========================================================================*/
/*!
  @brief Initializes Restricted Service 

  @param   none
  @return  none
 */
/*=========================================================================*/
sns_err_code_e sns_rh_process_internal_smgr_request(
  sns_rh_mr_req_q_item_s**  msg_ptr_ptr)
{
  sns_err_code_e  err_code = SNS_ERR_NOTSUPPORTED;
  switch ( (*msg_ptr_ptr)->header.msg_id )
  {
  case SNS_SMGR_CANCEL_REQ_V02:
    err_code = sns_rh_process_internal_svc_cancel_request(*msg_ptr_ptr);
    break;
  case SNS_SMGR_VERSION_REQ_V02:
    err_code = sns_rh_process_internal_version_request(*msg_ptr_ptr);
    break;
  case SNS_SMGR_SENSOR_EVENTS_QUERY_REQ_V02:
    err_code = sns_rh_process_sensor_events_query_request(*msg_ptr_ptr);
    break;
  case SNS_SMGR_SENSOR_EVENT_REQ_V02:
    err_code = sns_rh_process_sensor_event_request(msg_ptr_ptr);
    break;
  case SNS_SMGR_SENSOR_STATUS_MONITOR_REQ_V02:
    err_code = sns_rh_process_sensor_status_monitor_request(msg_ptr_ptr);
    break;
  case SNS_SMGR_EVENT_GATED_BUFFERING_REQ_V02:
    err_code = sns_rh_process_event_gated_buffering_request(*msg_ptr_ptr);
    break;
  default:
    SNS_SMGR_PRINTF1(
      ERROR, "internal_smgr_req - unknown msg_id(%u)", 
      (*msg_ptr_ptr)->header.msg_id);
    break;
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_rh_generate_sensor_status_ind

===========================================================================*/
/*!
  @brief Generates and sends SNS_SMGR_SENSOR_STATUS_MONITOR_IND_V02

  @param   ddf_sensor_ptr - the sensor whose status has changed
 
  @return  none
 */
/*=========================================================================*/
void sns_rh_generate_sensor_status_ind(
  sns_smgr_ddf_sensor_s*  ddf_sensor_ptr,
  sns_rh_mr_req_q_item_s* msg_ptr)
{
  sns_smgr_client_stat_s* cs_ptr = &ddf_sensor_ptr->client_stat;
  sns_smgr_sensor_status_monitor_ind_msg_v02* ind_ptr =
    &sns_rh.infrequent_ind.sensor_status_monitor;

  ind_ptr->sensor_id     = msg_ptr->request.sensor_status_monitor.sensor_id;
  ind_ptr->num_clients   = cs_ptr->num_rpt_items;
  ind_ptr->sampling_rate = ddf_sensor_ptr->current_odr;
  ind_ptr->timestamp     = cs_ptr->sensor_status.last_status_ts;
  ind_ptr->is_accurate_ts_client_present_valid
                         = true;
  ind_ptr->is_accurate_ts_client_present
                         = SMGR_BIT_TEST(cs_ptr->flags,
                                         RH_RPT_ITEM_FLAGS_ACCURATE_TS_B);

  SNS_SMGR_PRINTF4(
    HIGH, "sensor_status_ind - sensor=%u #client=%u ts=%u accurate_ts_present=%u", 
    ind_ptr->sensor_id, ind_ptr->num_clients, ind_ptr->timestamp,
    ind_ptr->is_accurate_ts_client_present);

  sns_rh_mr_send_indication(msg_ptr->header.connection_handle,
                            ind_ptr,
                            SNS_SMGR_SENSOR_STATUS_MONITOR_IND_V02,
                            sizeof(*ind_ptr),
                            false); /* do not free ind msg */
}

/*===========================================================================

  FUNCTION:   sns_rh_internal_svc_init

===========================================================================*/
/*!
  @brief Initializes SMGR internal service data structures

  @detail

  @param[i] none
  @return   none
 */
/*=========================================================================*/
void sns_rh_internal_svc_init(void)
{
  uint32_t s, d;
  sns_rh_sensor_event_s* se_ptr = sns_rh.sensor_events;

  SNS_OS_MEMZERO(sns_rh.sensor_events, sizeof(sns_rh.sensor_events));

  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    if ( sns_smgr.sensor[s].sensor_state <= SENSOR_STATE_FAILED )
    {
      continue;
    }
    for ( d=0; d<ARR_SIZE(sns_smgr.sensor[s].const_ptr->data_types); d++ )
    {
      if ( (sns_smgr.sensor[s].const_ptr->data_types[d] == SNS_DDF_SENSOR_ACCEL) &&
           SMGR_SENSOR_INTERRUPT_ENABLED(&sns_smgr.sensor[s]) )
      {
        se_ptr->sensor_event = SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02;
      }
      if ( se_ptr->sensor_event != 0 )
      {
        se_ptr->ddf_sensor_ptr = sns_smgr.sensor[s].ddf_sensor_ptr[d];
        se_ptr->status = SNS_SMGR_SENSOR_EVENT_STATUS_DISABLED_V02;
        sns_q_init(&se_ptr->request_queue);
        sns_q_init(&se_ptr->enabled_state_rpt_q);
        sns_q_init(&se_ptr->disabled_state_rpt_q);
        sns_q_init(&se_ptr->occurred_state_rpt_q);
        se_ptr++;
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_update_sensor_event

===========================================================================*/
/*!
  @brief Affects Motion Detect event
 
  @param [i] - ddf_sensor_ptr - the sensor that might be associated with an event

  @return none
 */
/*=========================================================================*/
void sns_rh_update_sensor_event(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_rh_sensor_event_s* se_ptr = 
    sns_rh_find_sensor_event(SNS_SMGR_SENSOR_EVENT_ID_MOTION_DETECT_1_V02);
  /* ---------------------------------------------------------------------- */

  if ( se_ptr != NULL && se_ptr->ddf_sensor_ptr == ddf_sensor_ptr && !se_ptr->enabled )
  {
    sns_rh_rpt_item_s*  item_ptr;
    sns_rh_rpt_item_s** item_ptr_ptr;
    bool independent_streams = false;
    bool occurred_streams = false;
    uint8_t en_rpts  = sns_q_cnt(&se_ptr->enabled_state_rpt_q);
    uint8_t dis_rpts = sns_q_cnt(&se_ptr->disabled_state_rpt_q);
    uint8_t occ_rpts = sns_q_cnt(&se_ptr->occurred_state_rpt_q);

    SMGR_FOR_EACH_ASSOC_ITEM( se_ptr->ddf_sensor_ptr, item_ptr, item_ptr_ptr )
    {
      if ( item_ptr->parent_report_ptr->msg_header.svc_num == SNS_SMGR_SVC_ID_V01 )
      {
        independent_streams = true;
      }
      else if ( item_ptr->parent_report_ptr->holding_q == NULL )
      {
        occurred_streams = true;
      }
    }

    SNS_SMGR_PRINTF4(
      HIGH, "enable_sensor_event - #e/d/o_rpts=0x%03x #streams=%u indep=%u occ=%u",
      (en_rpts << 8) | (dis_rpts << 4) | occ_rpts, 
      se_ptr->ddf_sensor_ptr->client_stat.num_rpt_items, 
      independent_streams, occurred_streams);

    if ( ((occ_rpts > 0) || (dis_rpts > 0)) && !occurred_streams )
    {
      if ( !independent_streams )
      {
        // PEND: Determine if/how MD-interrupt will work concurrently with an active DAF request
        /* stops eventDisabled streams now that event can be enabled */
        while ( se_ptr->ddf_sensor_ptr->client_stat.num_rpt_items > 0 )
        {
          sns_rh_sol_deactivate_report(
            se_ptr->ddf_sensor_ptr->client_stat.rpt_item_ptr->parent_report_ptr);
        }
        SNS_SMGR_PRINTF0(HIGH, "enable_sensor_event - forward MD request to SMGR");
        sns_smgr_set_md_state(true);
      }
      else
      {
        /* starts eventDisabled streams as independent streams would prevent 
           event from being enabled */
        sns_rh_rpt_spec_s* rpt_ptr;
        while ( (rpt_ptr = sns_q_check(&se_ptr->disabled_state_rpt_q)) != NULL )
        {
          uint8_t  i;
          RH_FOR_EACH_REPORT_ITEM( rpt_ptr, i )
          {
            rpt_ptr->item_list[i]->ts_last_sent = sns_ddf_get_timestamp();
          }
          sns_q_delete(&rpt_ptr->rpt_link);
          sns_rh_sol_schedule_report(&rpt_ptr->msg_header, rpt_ptr);
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_rh_cancel_internal_service

===========================================================================*/
/*!
  @details Cancels all requests to SMGR service by the client associated
         with given connection

  @param[i]  connection_handle - QCCI handle

  @return  none
 */
/*=========================================================================*/
void sns_rh_cancel_internal_service(const void* connection_handle)
{
  sns_rh_sensor_event_s* se_ptr = NULL;
  uint32_t e = 0;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF1(MED, "cancel_internal_service - conn=0x%x", connection_handle);

  while ( sns_rh.sensor_events[e].sensor_event != 0 )
  {
    uint32_t s, d;
    sns_rh_rpt_spec_s* rpt_ptr;
    sns_rh_rpt_spec_s* delete_rpt_ptr[SNS_SMGR_MAX_REPORT_CNT];
    uint8_t num_reports = 0;
    sns_rh_mr_req_q_item_s* q_item_ptr;
    se_ptr = &sns_rh.sensor_events[e];

    for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
    {
      sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[s];
      for ( d=0; d<ARR_SIZE(sns_smgr.sensor[s].ddf_sensor_ptr); d++ )
      {
        sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
        if ( ddf_sensor_ptr != NULL )
        {
          q_item_ptr = sns_rh_find_request(
                         &ddf_sensor_ptr->client_stat.sensor_status.request_queue, 
                         connection_handle, true);
          if ( q_item_ptr != NULL )
          {
            SNS_SMGR_PRINTF1(LOW, "cancel_internal_service - status_monitor ptr=0x%x", 
                             q_item_ptr);
            SNS_OS_FREE(q_item_ptr);
          }
        }
      }
    }
    SMGR_FOR_EACH_Q_ITEM( &se_ptr->request_queue, q_item_ptr, q_link )
    {
      if ( q_item_ptr->header.connection_handle == connection_handle )
      {
        SNS_SMGR_PRINTF1(LOW, "cancel_internal_service - ev_req ptr=0x%x", q_item_ptr);
        sns_q_delete(&q_item_ptr->q_link);
        SNS_OS_FREE(q_item_ptr);
        break;
      }
    }
    SMGR_FOR_EACH_Q_ITEM( &se_ptr->enabled_state_rpt_q, rpt_ptr, rpt_link )
    {
      if ( connection_handle == rpt_ptr->msg_header.connection_handle )
      {
        SNS_SMGR_PRINTF2(LOW, "cancel_internal_service - enabled_rpt id=%u/0x%x",
                         rpt_ptr->rpt_id, connection_handle);
        if ( num_reports < ARR_SIZE(delete_rpt_ptr) )
        {
          delete_rpt_ptr[num_reports++] = rpt_ptr;
        }
      }
    }
    SMGR_FOR_EACH_Q_ITEM( &se_ptr->disabled_state_rpt_q, rpt_ptr, rpt_link )
    {
      if ( connection_handle == rpt_ptr->msg_header.connection_handle )
      {
        SNS_SMGR_PRINTF2(LOW, "cancel_internal_service - disabled_rpt id=%u/0x%x",
                         rpt_ptr->rpt_id, connection_handle);
        if ( num_reports < ARR_SIZE(delete_rpt_ptr) )
        {
          delete_rpt_ptr[num_reports++] = rpt_ptr;
        }
      }
    }
    SMGR_FOR_EACH_Q_ITEM( &se_ptr->occurred_state_rpt_q, rpt_ptr, rpt_link )
    {
      if ( connection_handle == rpt_ptr->msg_header.connection_handle )
      {
        SNS_SMGR_PRINTF2(LOW, "cancel_internal_service - occurred_rpt id=%u/0x%x",
                         rpt_ptr->rpt_id, connection_handle);
        if ( num_reports < ARR_SIZE(delete_rpt_ptr) )
        {
          delete_rpt_ptr[num_reports++] = rpt_ptr;
        }
      }
    }
    if (num_reports > 0)
    {
      if ( num_reports >= ARR_SIZE(delete_rpt_ptr) )
      {
        SNS_SMGR_PRINTF0(ERROR, "cancel_internal_service - reached max reports!!!");
      }
      while ( num_reports-- > 0 )
      {
        sns_q_delete(&delete_rpt_ptr[num_reports]->rpt_link);
        SNS_OS_ANY_FREE(delete_rpt_ptr[num_reports]->item_list[0]);
        SNS_OS_ANY_FREE(delete_rpt_ptr[num_reports]);
      }
      sns_rh_disable_sensor_event(se_ptr);
    }
    e++;
  }
}

