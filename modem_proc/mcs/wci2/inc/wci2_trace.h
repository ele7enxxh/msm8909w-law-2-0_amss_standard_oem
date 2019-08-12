#ifndef WCI2_TRACE_H
#define WCI2_TRACE_H
/*!
  @file
  wci2_trace.h

  @brief
  APIs for WCI2's debug trace logging

*/

/*===========================================================================

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_trace.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/14   ckk     Add support for type4 & type7[pwr state]
           ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <atomic_ops.h>
#include <IxErrno.h>
#include <comdef.h>

/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/
/* Number of events in circular buffer
 * Assume 4 events/ms * 1000 ms/s * 0.5 s = 2000 */
#define WCI2_TRACE_NUM_OF_EVENTS  1024

/*! @brief WCI2 trace events (last updated on 03/22/2013) */
typedef enum
{
  WCI2_TRC_INVALID,             /* [0, 0, 0] */
  WCI2_TRC_TSK_INIT,            /* [0, 0, 0] */
  WCI2_TRC_TSK_DEINIT,          /* [0, 0, 0] */
  WCI2_TRC_UART_RCV_MSG,        /* [BYTE, FIFO_INDX, RX_FIFO_PTR] */
  WCI2_TRC_UART_SND_MSG,        /* [SIZE, 0, 0] */
  WCI2_TRC_DIR_READ_CB,         /* [STATUS, ENABLE_MASK, INTR_STATUS_MASK] */
  WCI2_TRC_SIO_RX_CB,           /* [CXM_MODE, ADV_HW, 0] */
  WCI2_TRC_RX_WCI2,             /* [BYTE, TYPE, CXM_MODE] */
  WCI2_TRC_TX_WCI2,             /* [BYTE, TYPE, 0] */
  WCI2_TRC_T2_MISALIGNED,       /* [COUNT, BYTE, 0] */
  WCI2_TRC_SET_CXM_MODE,        /* [NEW_MODE, OLD_MODE, 0] */
  WCI2_TRC_SET_BAUD,            /* [NEW_BAUD, OLD_BAUD, 0] */
  WCI2_TRC_UART_POWER_STATE,    /* [NEW_STATE, OLD_STATE, 0] */
  WCI2_TRC_CLIENT_REG,          /* [RETVAL, MODE, HANDLE] */
  WCI2_TRC_CLIENT_DEREG,        /* [RETVAL, HANDLE, 0] */
  WCI2_TRC_CLNT_NTFY_T2_RX,     /* [REF_CNT, NUM_NOTIFIED, MSG_LEN] */
  WCI2_TRC_CLNT_NTFY_DIR_RX,    /* [NUM_NOTIFIED, STATUS, 0] */
  WCI2_TRC_CLNT_NTFY_BASIC_RX   /* [NUM_NOTIFIED, SIZE, 0] */

} wci2_trace_event_types_e;

/*! @brief WCI2 trace event record/structure 
    @note    Current structure size is 16 bytes & is packed tight */
typedef struct
{
  uint8  event; /*!< 8-bit event ID (wci2_trace_event_types_e) */
  uint8  a8;    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16 b16;   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32 c32;   /*!< 3rd of 3 opaque payloads (32-bit) */
  uint64 ts;    /*!< timestamp in slowclock ticks */
} wci2_trace_event_log_s;

/*! @brief WCI2 trace structure */
typedef struct
{
  /*!< Sequence number of the log entry in the table 
       Note: Last location written to in circular "events" trace buffer
             can be calculated by [seq_num % WCI2_TRACE_NUM_OF_EVENTS]
  */
  atomic_word_t         seq_num; 
  /*! Circular "events" trace buffer */
  wci2_trace_event_log_s events[ WCI2_TRACE_NUM_OF_EVENTS ];
  /*!< Version number to help keep script in step */
  uint8                 version_number;
} wci2_trace_event_s;

/* Macro for passing logging data to logging function */
#define WCI2_TRACE(event_id, a8, b16, c32) \
  wci2_trace_event((wci2_trace_event_types_e)event_id, (uint8)(a8), (uint16)(b16), (uint32)(c32))

/* WCI2 counters enum -- list of all counters.
 * NOTE: if more are added, add them at the end */
typedef enum
{
  WCI2_CNT_UART_RX,
  WCI2_CNT_UART_RX_WCI2_TYPE1,
  WCI2_CNT_UART_RX_WCI2_TYPE6,
  WCI2_CNT_UART_RX_WCI2_TYPE_UNSUPPORTED,
  WCI2_CNT_UART_TX,
  WCI2_CNT_UART_TX_WCI2_TYPE1,
  WCI2_CNT_UART_TX_WCI2_TYPE3,
  WCI2_CNT_UART_TX_WCI2_TYPE4,
  WCI2_CNT_UART_TX_WCI2_TYPE6,
  WCI2_CNT_UART_TX_WCI2_TYPE7,
  WCI2_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED,

  WCI2_CNT_MAX_COUNTERS /* placeholder - always last */
} wci2_counters_e;

/* WCI2 counter instance structure */
typedef struct
{
  uint32 count;
  uint32 last_value;
  uint32 last_tstamp;
} wci2_counter_type_s;


/*===========================================================================

  FUNCTION:  wci2_trace_init

===========================================================================*/
/*!
  @brief
    To initialize WCI2's debug trace/event logging interface

  @return
    None
*/
/*=========================================================================*/
void wci2_trace_init (
  void
);

/*===========================================================================

  FUNCTION:  wci2_trace_event

===========================================================================*/
/*!
  @brief
    To log WCI2 event into the trace buffer

  @return
    None
*/
/*=========================================================================*/
void wci2_trace_event
(
  wci2_trace_event_types_e event,   /*!< WCI2 event type */
  uint8                   a8,    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16                  b16,   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32                  c32    /*!< 3rd of 3 opaque payloads (32-bit) */
);

/*===========================================================================

  FUNCTION:  wci2_counter_event

===========================================================================*/
/*!
  @brief
    To increment a WCI2 counter corresponding to some event. Also log the 
    timestamp and value of that event

  @detail
    NOT threadsafe. please call only from within the wci2 task for now.

  @return
    None
*/
/*=========================================================================*/
void wci2_counter_event
(
  wci2_counters_e counter,    /*!< Unique counter ID */
  uint32         last_value  /*!< Specific to the event that the counter 
                                 refers to. This is meant to be the value of
                                 the event we are counting */
);

/*===========================================================================

  FUNCTION:  wci2_counter_reset

===========================================================================*/
/*!
  @brief
    Reset specified counter. Exceptions: If value passed is 
    WCI2_CNT_MAX_COUNTERS, will reset ALL counters. If WCI2_CNT_POLICY_RESET 
    is passed in, will reset all counters EXCEPT the policy counters.

  @return
    None
*/
/*=========================================================================*/
void wci2_counter_reset
(
  wci2_counters_e counter
);

/*===========================================================================

  FUNCTION:  wci2_get_counters

===========================================================================*/
/*!
  @brief
    Gets all counters

  @return
    None
*/
/*=========================================================================*/
void wci2_get_counters
(
  uint32 counters[]
);

#endif /* WCI2_TRACE_H */

