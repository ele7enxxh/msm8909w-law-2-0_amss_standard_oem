/* stats_util.h
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __STATS_UTIL_H__
#define __STATS_UTIL_H__
#include "mct_stream.h"
#include "stats_event.h"
#include "modules.h"

/* Call this function before use the stats_ext_event, it shall add 1 to the ref_count
 * @return  the count of stats_ext_event
 */
int32_t circular_stats_data_use(mct_event_stats_ext_t* stats_ext_event);

/* Call this function after finish using the stats data, it shall decrease ref_count by 1
 * @return  the count of stats_ext_event after decreasing
 */
int32_t circular_stats_data_done(mct_event_stats_ext_t* stats_ext_event,
                                 mct_port_t *port, uint32_t identity, uint32_t frame_id);


void circular_stats_buf_divert_in_use(void *msg,isp_buf_divert_t *stats_buff);

void circular_stats_send_buf_divert_ack(mct_port_t *port,void *msg);

#endif //__STATS_UTIL_H__
