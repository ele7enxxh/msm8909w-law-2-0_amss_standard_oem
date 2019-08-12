/*!
  @file
  vstmr_hdr.h

  @brief
  VSTMR HDR header file

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_hdr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_HDR_H
#define VSTMR_HDR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 
 
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal HDR FCW value in Q30.
    CDMA cx8: (1.2288 MHz * 8) / 19.2e6 = 0.512 * 2^30 = 0x20c49ba6 (rounded) 
*/
#define VSTMR_HDR_FCW               (0x20c49ba6)

/*! @brief Wraparound for HDR RTC = 2^30 (~109 sec)  
*/
#define VSTMR_HDR_RTC_MODULO        (0x40000000)

/*! @brief Wraparound for HDR PN roll (cx8) = 2^18  
*/
#define VSTMR_HDR_PNROLL_MODULO     (0x00040000)

/*! @brief Wraparound for PN count = 2^12  
*/
#define VSTMR_HDR_PN_CNT_RANGE      4096

/*! Min/Max values for a periodic event's index in the HDR event table.
    0-7 events are assumed to be periodic. */
#define VSTMR_HDR_PERIODIC_EVT_MIN_INDEX  0
#define VSTMR_HDR_PERIODIC_EVT_MAX_INDEX  7

/*! Min/Max values for a one-shot event's index in the HDR event table.
    8-15 events are assumed to be one-shot. */
#define VSTMR_HDR_ONESHOT_EVT_MIN_INDEX   8
#define VSTMR_HDR_ONESHOT_EVT_MAX_INDEX   15


/*! Enum to identify HDR views
*/
typedef enum  
{
  VSTMR_HDR_RTC_VIEW,          /*!< HDR RTC view */
  VSTMR_HDR_TX_SYS_TIME_VIEW,  /*!< HDR TX system time view */
  VSTMR_HDR_RX_SYS_TIME_VIEW,  /*!< HDR RX system time view */
  VSTMR_HDR_NUM_VIEWS
} vstmr_hdr_view_e;


/*! Structure to identify HDR segmentation format.
    Note that this is a 64-bit quantity. 
    The lower 32 bits are sub-cx8 fractional count.
*/
typedef union {

  /* Types for internal manipulation */
  uint64 d;
  uint32 w[2];

  uint64 all;     /*!< For clearing entire dword */

  struct {
    uint32 frac;                /*!< Fractional bits, sub-cx8 */

    /* Multiple representations of the same segmented counts.
       Note all of these are in the MSW of the HSTMR format. */
    union{

      struct {
        uint32 cx8            : 30; /*!< Full cx8 count, Range 2^30 (~109 sec) */ 
        uint32                : 2;  
      };
      struct {
        uint32 pnroll_cx8     : 18; /*!< PN roll bits, Range 65536*4 */ 
        uint32 pn_cnt         : 12; /*!< PN count, Range VSTMR_HDR_PN_CNT_RANGE */
        uint32                : 2; 
      };
      struct {
        uint32                : 3;
        uint32 pnroll_cx1     : 15; /*!< Cx1 count within a PN roll */
        uint32                : 14;
      };
      struct {
        uint32                : 2;
        uint32 pnroll_cx2     : 16; /*!< Cx2 count within a PN roll, Range 65536 */
        uint32                : 14;
      };
      struct {
        uint32                : 3;
        uint32 slot_cx1       : 11; /*!< Cx1 count within 1 slot (2048 chips) */
        uint32 slot_cnt       : 16; /*!< Slot count */
        uint32                : 2; 
      };
      struct {
        uint32                : 14;
        uint32 slot_cnt_mod16 : 4;  /*!< Slot count mod 16 */
        uint32 frame_cnt      : 7;  /*!< 16 slots = 1 frame */
        uint32                : 7;
      };
    };
  };
}vstmr_hstmr_t;


/*! @brief HDR view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s *rtc;      /*!< Pointer to the RTC struct */ 
  uint32              offset;   /*!< Offset for this view in HDR RTC units
                                     (cx8), Note: Its a uint32, so no 
                                     fractional part. */
} vstmr_hdr_view_s;


/*! @brief HDR event table management structure.
*/
typedef struct vstmr_hdr_event_tbl_s_
{
  vstmr_event_tbl_s   event_tbl_data;  /*!< Internal event table data, stores 
                                            event expiration counts in XO 
                                            resolution. */

  /* HDR specific fields */
  vstmr_hdr_view_s   *view;            /*!< Pointer to the associated view
                                            View is the source of time for this
                                            event table */ 

  uint32              events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in HSTMR resolution (no fractional 
       part). This is in the format of vstmr_hstmr_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */

  uint32              events_period[VSTMR_MAX_EVENTS];   
  /*!< Array to store periodic count for the events within the event table. NOTE: 
       Events 8-15 are assumed to be one-shot, hence for them, period is stored 
       as 0 */ 
                                                               
  void (*isr_callback)(struct vstmr_hdr_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< HDR callback routine which HDR VSTMR layer calls once an event expires */   

  void *isr_arg;
  /*!< Argument for the callback routine */  

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered HDR callback.
       Servicing the event table involves parsing the event table to find out what
       all events have expired and then reprogramming the comparator for the
       next event. */

  atomic_word_t       service_sync_flag;    /*!< Flag to synchronize event table 
                                                 servicing and deregistration. 
                                                 0: Event table is deregistered OR 
                                                    is being serviced by the event 
                                                    table servicing API 
                                                 1: Event table is registered 
                                                    and is not being serviced
                                                    by the event table servicing API 

    Race condition between event table servicing and event table deregistration:
      1) Event table servicing is active and event table deregistration is called 
         from a parallel thread. Parallel thread will poll until servicing completes 
         and will then proceed with deregistration.
      2) Event table deregistration has already started and then event table servicing 
         starts. Event table servicing will exit immediately without doing any processing.

    Algorithm for handling the race:

      Event table service:
       if (!atomic_compare_and_set(&service_sync_flag, 1, 0))
         return; // deregister is active
       < ... function body ...>
       atomic_set(&service_sync_flag, 1); // Reset the flag

      Event table deregister:
       while (!atomic_compare_and_set(&service_sync_flag, 1, 0)){}  // Poll until event 
                                                                       servicing is NOT active
       < ... function body ...>                                                             */


  atomic_word_t       isr_sync_flag;        /*!< Flag to synchronize ISR handler and 
                                                 interrupt deregistration.
                                                 0: Interrupt is deregistered OR ISR
                                                    handler is active 
                                                 1: Interrupt is registered and is not 
                                                    being serviced by an ISR 

    Race condition between ISR handler and interrupt deregistration:
      1) ISR handler is active and interrupt deregistration is called from
         a parallel thread. Parallel thread will poll until ISR handler completes
         and will then proceed with deregistration.
      2) Interrupt deregistration has already started and then ISR handler starts.
         ISR handler will exit immediately without doing any processing.

    Algorithm for handling the race:

      ISR:
        if (!atomic_compare_and_set(&isr_sync_flag, 1, 0))
          return; // deregister is active
        < ... function body ...>
        atomic_set(&isr_sync_flag, 1); // Reset the flag

      Interrupt deregister:
        while (!atomic_compare_and_set(&isr_sync_flag, 1, 0)){}  // Poll until ISR
                                                                    handler is NOT active
       < ... function body ...>                                                         */

} vstmr_hdr_event_tbl_s;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


vstmr_hdr_view_s * vstmr_hdr_view_init
( 
  vstmr_rtc_id       rtc_id,
  vstmr_hdr_view_e   view_id,
  vstmr_hstmr_t      offset
);

vstmr_hdr_view_s * vstmr_hdr_get_view_handle
(
  vstmr_rtc_id       rtc_id,
  vstmr_hdr_view_e   view_id
);

vstmr_hstmr_t vstmr_hdr_get_current_hstmr
(
  vstmr_hdr_view_s  *h_view
);

vstmr_hstmr_t vstmr_hdr_xo_to_hstmr
(
  vstmr_hdr_view_s  *h_view,
  uint32 xo_cnt              
);

uint32 vstmr_hdr_hstmr_to_xo
(
  vstmr_hdr_view_s  *h_view,
  vstmr_hstmr_t      hstmr
);

void vstmr_hdr_hstmr_sync
(
  vstmr_hdr_view_s  *h_view,        
  vstmr_hstmr_t      hstmr,    
  uint32             xo_cnt    
);

void vstmr_hdr_view_set_offset
(
  vstmr_hdr_view_s  *h_view,
  vstmr_hstmr_t      offset
);

void vstmr_hdr_view_adjust_offset
(
  vstmr_hdr_view_s  *h_view,
  int32              adjust_cx8
);

void vstmr_hdr_update_fcw
(
  vstmr_hdr_view_s  *h_view,
  uint32 fcw
);

void vstmr_hdr_event_tbl_register
(
  vstmr_hdr_event_tbl_s    *event_tbl,
  vstmr_hdr_view_s         *h_view,
  vstmr_int_types_e         int_type
);

void vstmr_hdr_event_tbl_deregister
(
  vstmr_hdr_event_tbl_s    *event_tbl
);

void vstmr_hdr_interrupt_register
(
  vstmr_hdr_event_tbl_s    *event_tbl,
  void (*cb)(vstmr_hdr_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_hdr_interrupt_deregister
(
  vstmr_hdr_event_tbl_s    *event_tbl
);

extern uint32 vstmr_hdr_event_service
(
  vstmr_hdr_event_tbl_s    *event_tbl
);

uint32 vstmr_hdr_event_sched
(
  vstmr_hdr_event_tbl_s    *id,
  uint32                    event_idx,
  uint32                    pnroll_offset_cx8
);

void vstmr_hdr_event_sched_periodic
(
  vstmr_hdr_event_tbl_s    *id,
  uint32                    event_idx,
  uint32                    pnroll_offset_cx8,
  vstmr_hstmr_t             period       
);

void vstmr_hdr_event_cancel
(
  vstmr_hdr_event_tbl_s    *id,
  uint32                    event_idx
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_HDR_H*/
