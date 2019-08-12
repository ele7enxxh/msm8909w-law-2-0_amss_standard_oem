#ifndef SNS_RH_H
#define SNS_RH_H

/*============================================================================

  @file sns_rh.h

  @brief
  This file contains definition for Request Handler

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/inc/sns_rh.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-02-05  pn    Added SNS_RH_RESCHED_REPORTS_SIG
  2016-01-26  pn    Removed SNS_RH_SMGR_INIT_DONE_SIG
  2015-10-19  pn    Obsoleted SNS_RH_FIFO_SAMPLES_SIG
  2015-01-13  jtl   Renamed the decommissioned sig to SNS_RH_SENSOR_HEARTBEAT_SIG
  2015-09-30  pn    Added SNS_RH_SENSOR_DECOMMISSIONED_SIG
  2015-09-11  pn    Added SNS_RH_REPORT_IS_STREAMING() and SNS_RH_REPORT_IS_BATCHING()
  2015-07-30  BD    Added SNS_RH_BATCHBUFF_LOW_SIG 
  2015-05-27  BD    Added SNS_RH_SMR_RESUME_SIG and SMR resume event function
  2015-01-22  BD    Changed the way of putting functions in uImage to use sections
  2015-01-13  hw    Added SNS_RH_MODEM_WAKE_SIG and modem sync event function
  2015-01-16  vy    Moved uImage logging to debug module  
  2014-11-24  pn    Added SNS_RH_FIFO_SAMPLES_SIG
  2014-11-11  dk    Added signal for SNS_RH_LOGPKT_DROP_SIG
  2014-11-04  pn    Removed unused SNS_RH_SENSOR_EVENT_ENABLE_SIG
  2014-09-02  rt    Added signal for UPDATE_REPORT_TIMER
  2014-08-05  pn    Added ODR_CHANGED and FIFO_FLUSHED signals
  2014-07-21  pn    Added support for event_gated_buffering
  2014-06-16  pn    Removed references to unused circular buffer
  2014-05-07  pn    Added support for MD deregistration
  2014-05-02  aj    Replaced QMI with SMR
  2014-04-23  pn    Initial version
============================================================================*/
#include "sns_common.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/
#define SNS_RH_REPORT_IS_PERIODIC(rp)   \
  ((rp)->msg_header.msg_id == SNS_SMGR_REPORT_REQ_V01)

#define SNS_RH_REPORT_IS_BUFFERING(rp)                            \
  ((((rp)->msg_header.svc_num == SNS_SMGR_SVC_ID_V01) &&          \
    ((rp)->msg_header.msg_id == SNS_SMGR_BUFFERING_REQ_V01)) ||   \
   (((rp)->msg_header.svc_num == SNS_SMGR_INTERNAL_SVC_ID_V01) && \
    ((rp)->msg_header.msg_id == SNS_SMGR_EVENT_GATED_BUFFERING_REQ_V02)))

#define SNS_RH_REPORT_IS_STREAMING(rp) \
  (SNS_RH_REPORT_IS_PERIODIC(rp) || (rp)->periodic_like)

#define SNS_RH_REPORT_IS_BATCHING(rp) \
  (SNS_RH_REPORT_IS_BUFFERING(rp) && !(rp)->periodic_like && (rp)->rpt_tick != 0)

/*===========================================================================

                   DEFINITIONS AND TYPES

===========================================================================*/

#define SNS_RH_SELF_TEST_DONE_SIG         0x00000002
#define SNS_RH_RESCHED_REPORTS_SIG        0x00000004
#define SNS_RH_SAMPLE_READY_SIG           0x00000008
#define SNS_RH_REPORT_TIMER_SIG           0x00000010
#define SNS_RH_MD_INT_ENABLED_SIG         0x00000020
#define SNS_RH_MD_INT_DISABLED_SIG        0x00000040
#define SNS_RH_MD_INT_SIG                 0x00000080
#define SNS_RH_RESP_Q_SIG                 0x00000100
#define SNS_RH_IND_Q_SIG                  0x00000200
#define SNS_RH_ODR_CHANGED_SIG            0x00000400
#define SNS_RH_FIFO_FLUSHED_SIG           0x00000800
#define SNS_RH_BATCHBUFF_LOW_SIG          0x00001000
#define SNS_RH_SENSOR_HEARTBEAT_SIG       0x00002000
#define SNS_RH_SMR_MSG_SIG                0x00010000
#define SNS_RH_SMR_DISC_SIG               0x00020000
#define SNS_RH_SMR_RESUME_SIG             0x00040000
#define SNS_RH_APP_WAKE_SIG               0x00100000
#define SNS_RH_APP_SLEEP_SIG              0x00200000
#define SNS_RH_UPDATE_REPORT_TIMER_SIG    0x00400000
#define SNS_RH_MODEM_WAKE_SIG             0x02000000

#ifdef SNS_USES_ISLAND
#define SNS_RH_UIMAGE_CODE __attribute__((section (".text.SMGR")))
#define SNS_RH_UIMAGE_DATA __attribute__((section (".data.SMGR")))
#define SNS_RH_UIMAGE_BSS __attribute__((section (".bss.SMGR")))
#else
#define SNS_RH_UIMAGE_CODE
#define SNS_RH_UIMAGE_DATA
#define SNS_RH_UIMAGE_BSS
#endif /* USES_ISLAND */


/*===========================================================================

                   STRUCTURE DEFINITIONS

===========================================================================*/
typedef struct sns_rh_query_s             sns_rh_query_s;
typedef struct sns_rh_rpt_item_s          sns_rh_rpt_item_s;
typedef struct sns_rh_rpt_spec_s          sns_rh_rpt_spec_s;
typedef struct sns_rh_interpolator_obj_s  sns_rh_interpolator_obj_s;
typedef struct sns_rh_cic_obj_s           sns_rh_cic_obj_s;
typedef struct sns_rh_sensor_event_s      sns_rh_sensor_event_s;
typedef struct sns_rh_self_test_s         sns_rh_self_test_s;
typedef struct sns_rh_mr_req_q_item_s     sns_rh_mr_req_q_item_s;

/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern void sns_rh_uimg_modem_sync_event(void);

#endif /* SNS_RH_H */
