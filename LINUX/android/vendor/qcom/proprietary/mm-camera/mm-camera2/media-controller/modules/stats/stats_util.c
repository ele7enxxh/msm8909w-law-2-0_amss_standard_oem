/* stats_util.c
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include "stats_util.h"
#include "q3a_thread.h"

static void circular_stats_data_ack(mct_port_t *port, mct_event_stats_ext_t* stats_ext_event,
                                    uint32_t identity, uint32_t frame_id);

/* Call this function before use the stats_ext_event, it shall add 1 to the ref_count
 * @return  the count of stats_ext_event
 */
int32_t circular_stats_data_use(mct_event_stats_ext_t* stats_ext_event)
{
  int32_t count = 0;
  pthread_mutex_lock(stats_ext_event->stats_mutex);
  count = ++stats_ext_event->ref_count;
  pthread_mutex_unlock(stats_ext_event->stats_mutex);

  CDBG_LOW("%s count %d stats_ext_event %p isp_event %p", __func__,
            count, stats_ext_event, stats_ext_event->stats_data);
  return count;
}

/* Call this function after finish using the stats data, it shall decrease ref_count by 1
 * @return  the count of stats_ext_event after decreasing
 */
int32_t circular_stats_data_done(mct_event_stats_ext_t* stats_ext_event, mct_port_t *port,
                                 uint32_t identity, uint32_t frame_id)
{
  if (!stats_ext_event) {
    CDBG_ERROR("%s empty parameter!!!", __func__);
    return 0;
  }

  int32_t count = 0;

  pthread_mutex_lock(stats_ext_event->stats_mutex);
  count = --stats_ext_event->ref_count;
  pthread_mutex_unlock(stats_ext_event->stats_mutex);

  CDBG_LOW("%s count %d stats_ext_event %p isp_event %p", __func__,
            count, stats_ext_event, stats_ext_event->stats_data);
  if (count == 0) {
    circular_stats_data_ack(port, stats_ext_event, identity, frame_id);
    free(stats_ext_event);
  }

  return count;
}

static void circular_stats_data_ack(mct_port_t *port, mct_event_stats_ext_t* stats_ext_event, uint32_t identity, uint32_t frame_id)
{
  if (!port || !stats_ext_event) {
    CDBG_ERROR("%s empty parameter!!!", __func__);
    return;
  }

  mct_event_t       ack_event;

  /* pack into an mct_event object*/
  ack_event.direction = MCT_EVENT_UPSTREAM;
  ack_event.identity = identity;
  ack_event.type = MCT_EVENT_MODULE_EVENT;
  ack_event.u.module_event.current_frame_id = frame_id;
  ack_event.u.module_event.type = MCT_EVENT_MODULE_STATS_DATA_ACK;
  ack_event.u.module_event.module_event_data = stats_ext_event->stats_data;

  CDBG_LOW("%s send MCT_EVENT_MODULE_STATS_DATA_ACK upstream,"
    "port =%p, stats_ext_event %p isp_event %p", __func__, port,
    ack_event.u.module_event.module_event_data, stats_ext_event->stats_data);
  MCT_PORT_EVENT_FUNC(port)(port, &ack_event);

  return;
}

void circular_stats_buf_divert_in_use(void *msg,isp_buf_divert_t *stats_buff)
{
  if(!msg || !stats_buff) {
    CDBG_ERROR("%s empty parameter!!!", __func__);
    return;
  }
  q3a_divertstats_pdaf_msg_t *pdaf_msg = (q3a_divertstats_pdaf_msg_t *)msg;
  pdaf_msg->ack_info.identity = pdaf_msg->identity;
  pdaf_msg->ack_info.buf_idx = stats_buff->buffer.index;
  pdaf_msg->ack_info.is_buf_dirty = TRUE;
  pdaf_msg->ack_info.channel_id = stats_buff->channel_id;
  pdaf_msg->ack_info.frame_id = stats_buff->buffer.sequence;
  pdaf_msg->ack_info.timestamp = stats_buff->buffer.timestamp;
  pdaf_msg->ack_info.stats_type = stats_buff->stats_type;
}

void circular_stats_send_buf_divert_ack(mct_port_t *port,void *msg)
{
  if (!port || !msg) {
    CDBG_ERROR("%s empty parameter!!!", __func__);
    return;
  }
  isp_buf_divert_ack_t        *ack_info = ( isp_buf_divert_ack_t*)msg;
  mct_event_t                  ack_event;
  /* pack into an mct_event object*/
  ack_event.type = MCT_EVENT_MODULE_EVENT;
  ack_event.direction = MCT_EVENT_UPSTREAM;
  ack_event.identity = ack_info->identity;
  ack_event.u.module_event.type = MCT_EVENT_MODULE_BUF_DIVERT_ACK;;
  ack_event.u.module_event.module_event_data = ack_info;//passing in stack variable address, expect this call to be synchronous

  CDBG_LOW("%s send MCT_EVENT_MODULE_BUF_DIVERT_ACK upstream,"
    "port =%p", __func__, port);
  MCT_PORT_EVENT_FUNC(port)(port, &ack_event);
  return;
}
