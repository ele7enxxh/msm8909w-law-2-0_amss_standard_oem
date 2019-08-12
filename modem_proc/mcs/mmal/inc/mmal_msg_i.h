#ifndef MMAL_MSG_I_H
#define MMAL_MSG_I_H
/*!
  @file mmal_msg.h

  @brief
   The messages sent to the mmal task.

  @ingroup mmal_task_messages 
*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/inc/mmal_msg_i.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/15/11   rj      Added SMD Implementation for SlaveBridge
06/23/11   sk      Support for the LTE RRC cal back fn
04/26/11   ars     Changed mmal_msg_send to return boolean
03/25/11   ars     Initial checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "mcs_variation.h"
#include <dsm.h>
#include <sys.h>
#ifdef FEATURE_WCDMA
#include <rrcdata.h>
#endif
#include <smd_lite.h>

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
#define MMAL_MSG_INIT_HDR(hdr, msg_id) (hdr)->id = (msg_id)

typedef enum
{
  MMAL_LOG_LISTENER_REQ,
  MMAL_CIQ_BKEND_EXECUTE_TIMER_EXP_REQ,
  MMAL_CIQ_PL_SIO_RX_CB_REQ,
  MMAL_CIQ_PL_SMD_RX_CB_REQ,
  MMAL_CIQ_PL_TIME_ZONE_CHG_REQ,
  MMAL_CIQ_FEND_WRRC_LOG_MSG_CB_REQ,
  MMAL_CIQ_FEND_LRRC_LOG_MSG_CB_REQ,
  MMAL_CIQ_METRICS_LT03_REQ,
  MMAL_CIQ_METRICS_LT04_REQ,
} mmal_msg_id_e;

typedef struct
{
  mmal_msg_id_e id;
} mmal_msg_hdr_s;

/*!
  @brief
  Notifies MMAL that there is a log packet coming in from
  log listener module.

*/
typedef struct
{
  mmal_msg_hdr_s   hdr;
/*! Can be event id or a unique log id */
  uint32           log_id; 
  dsm_item_type*   payload_dsm_ptr;
} mmal_log_listener_req_s;


typedef struct
{
  uint8 channel_type;                       /* Variable for channel type */
  uint8 msg_type;                       /* Variable for message type */
  uint8 direction;	
  dsm_item_type *raw_sdu;	
  uint32 raw_sdu_length;
  uint32 transactionid;                       /* Variable for transaction id */
}mmal_log_wrrc_info;

#ifdef FEATURE_WCDMA
typedef struct
{
  mmal_msg_hdr_s    hdr;
  log_message_type  *log_msg_ptr;
  dsm_item_type     *dsm_item_ptr;
  //mmal_log_wrrc_info  log_msg_ptr;
}mmal_wrrc_log_msg_cb_s;
#endif

typedef struct
{
  mmal_msg_hdr_s    hdr;
  uint16 pdu_num;
  uint16 encoded_msg_len;
  uint8 msg_type;
  uint8 lc_type;
  dsm_item_type     *dsm_item_ptr;
  //mmal_log_wrrc_info  log_msg_ptr;
}mmal_lrrc_log_msg_cb_s;

typedef struct
{
  mmal_msg_hdr_s    hdr;
  dsm_item_type*    dsm_item_ptr;
}mmal_pl_sio_rx_cb_s;

typedef struct
{
  mmal_msg_hdr_s    hdr;
  smdl_handle_type  port;
  smdl_event_type   event;
  void              *data;
}mmal_pl_smd_rx_cb_s;

/*!
  @brief
  Notifies MMAL of a change in time zone

*/
typedef struct
{
  mmal_msg_hdr_s                  hdr;
  sys_time_zone_type              time_zone;
  boolean                         daylight_saving_adj_avail;
  sys_daylight_saving_adj_e_type  daylight_saving_adj;
}mmal_time_zone_info_s;

/* timer callback struct */
typedef struct
{
  mmal_msg_hdr_s                  hdr;
}mmal_timer_cb_s;

/*=============================================================================

  FUNCTION:  mmal_msg_send

=============================================================================*/
/*!
  @brief
  Sends a message to the MMAL.
  Returns TRUE if the message was put in the circ_q
          FALSE otherwise
*/
/*===========================================================================*/
boolean mmal_msg_send
(
  /*! The message to be sent */
  mmal_msg_hdr_s* msg

);

#endif /* MMAL_MSG_I_H */
