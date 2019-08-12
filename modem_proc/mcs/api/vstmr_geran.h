/*!
  @file
  vstmr_geran.h

  @brief
  VSTMR GERAN header file

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_geran.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/13   rj      Add frame extension support
04/08/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_GERAN_H
#define VSTMR_GERAN_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 
 
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal FCW value in Q30.
    (13 MHz) / 19.2e6 = (0.67708333...) * 2^30 = 0x2b555555 (rounded)
*/
#define VSTMR_GERAN_FCW                (0x2b555555)

/*! @brief Wraparound for GERAN RTC 
           4096 frames = (4096*60000)
*/
#define VSTMR_GERAN_RTC_MODULO         (0x0ea60000)

/*! @brief Frame range for GSTMR at 13MHz.
*/
#define VSTMR_GERAN_FRAME_RANGE        (60000)


/*! Enum to identify G views
*/
typedef enum  
{
  VSTMR_GERAN_GSTMR_VIEW,    /*!< GSTMR view */
  VSTMR_GERAN_NUM_VIEWS
} vstmr_geran_view_e;


/*! Structure to identify G Segmentation Format.
    Note that this is a 64-bit quantity. 
    The lower 32 bits are the sub-13M fractional count. 
*/
typedef union {

  /* Types for internal manipulation */
  uint64 d;
  uint32 w[2];

  uint64 all;     /*!< For clearing entire dword */

  struct {
    uint32 frac;                   /*!< Fractional Bits */

    /* Note all of these are in the MSW of the GSTMR format. */
    union {

      struct {
        uint32 raw_13M     : 18;  /*!< Raw 13 MHz count, range for 
                                       segmentation is [0..59999].  
                                       Has 2 extra bit's for:
                                       - Extending raw_13M count past
                                         a frame
                                       - Supporting frame extension 
                                         (upto 3 frames) */
        uint32 frame_num   : 12;  /*!< Frame no. [0..4095] */
        uint32             : 2;
      };
      struct {
        uint32             : 18;
        uint32 frame_mod4  : 2;   /*!< Frame no. mod 4  [0..3] */
        uint32             : 12; 
      };
    };
  };
} vstmr_gstmr_t;

/*! @brief GERAN Frame extension union
*/
typedef union {
  atomic64_word_t   value;

  struct{
    uint32 offset;          /*!< Offset for the view in GERAN RTC Units, 
                                 Note: Its a uint32, So NO fractional part */

    struct {
      uint32 ext_frame_tc      : 18;  /*!< Terminal count for the extended frame
                                           [0..239999] (extension upto 3 frames).
                                           Equals to (new frame length - 1).
                                           Valid values within an extended frame
                                           would be [0..ext_frame_tc] */  
      uint32 ext_frame_num     : 12;  /*!< Frame number to be extended [0..4095] */
      uint32                   : 2;
    };
  };
}vstmr_geran_ext_frame_u;


/*! @brief GERAN view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s        *rtc;              /*!< Pointer to the RTC Struct */ 

  vstmr_geran_ext_frame_u    ext_frame_data;   /*!< Extended frame data. NOTE: It also contains
                                                    view offset to support atomic read/write on
                                                    offset plus extended frame related fields */

} vstmr_geran_view_s;

/*! @brief GERAN event table management structure.
*/
typedef struct vstmr_geran_event_tbl_s_
{
  vstmr_event_tbl_s     event_tbl_data;   /*!< Internal event table data, stores 
                                               event expiration counts in XO 
                                               resolution. */

  /* GERAN specific fields */
  vstmr_geran_view_s   *view;             /*!< Pointer to the associated view.
                                               View is the source of time for this
                                               event table */  

  uint32                events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in GSTMR resolution (no fractional 
       part). This is in the format of vstmr_gstmr_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */
                                                               
  void (*isr_callback)(struct vstmr_geran_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< GERAN callback routine which G VSTMR layer calls once an event expires */   

  void *isr_arg;
  /*!< Argument for the callback routine */  

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered G callback.
       Servicing the event table involves parsing the event table to find out what
       all events have expired and then reprogramming the comparator for the
       next event. */

  atomic_word_t         service_sync_flag;  /*!< Flag to synchronize event table 
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
       < ... function body ...>                                                               */

  atomic_word_t         isr_sync_flag;      /*!< Flag to synchronize ISR handler and 
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
       < ... function body ...>                                                           */

} vstmr_geran_event_tbl_s;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


vstmr_gstmr_t vstmr_geran_segment_cnt
(
  uint64 rtc_cnt                 
);

vstmr_geran_view_s * vstmr_geran_view_init
( 
  vstmr_rtc_id         rtc_id,
  vstmr_geran_view_e   view_id,
  vstmr_gstmr_t        offset
);

vstmr_geran_view_s * vstmr_geran_get_view_handle
(
  vstmr_rtc_id         rtc_id,
  vstmr_geran_view_e   view_id
);

vstmr_gstmr_t vstmr_geran_get_current_gstmr
(
  vstmr_geran_view_s  *g_view
);

vstmr_gstmr_t vstmr_geran_xo_to_gstmr
(
  vstmr_geran_view_s  *g_view,
  uint32               xo_cnt              
);

uint32 vstmr_geran_gstmr_to_xo
(
  vstmr_geran_view_s  *g_view,
  vstmr_gstmr_t        gstmr
);

void vstmr_geran_gstmr_sync
(
  vstmr_geran_view_s  *g_view,        
  vstmr_gstmr_t        gstmr,    
  uint32               xo_cnt    
);

void vstmr_geran_update_fcw
(
  vstmr_geran_view_s  *g_view,
  uint32               fcw
);

void vstmr_geran_set_frame_tc
(
  vstmr_geran_view_s  *g_view,               
  uint32               new_terminal_cnt   
);

void vstmr_geran_complete_frame_ext
(
  vstmr_geran_view_s  *g_view
);

void vstmr_geran_view_set_offset
(
  vstmr_geran_view_s  *g_view,      
  uint32               offset       
);

void vstmr_geran_view_adjust_offset
(
  vstmr_geran_view_s  *g_view,    
  int32                adjust   
);

void vstmr_geran_event_tbl_register
(
  vstmr_geran_event_tbl_s    *event_tbl,
  vstmr_geran_view_s         *g_view,
  vstmr_int_types_e           int_type
);

void vstmr_geran_event_tbl_deregister
(
  vstmr_geran_event_tbl_s    *event_tbl
);

void vstmr_geran_interrupt_register
(
  vstmr_geran_event_tbl_s    *event_tbl,
  void (*cb)(vstmr_geran_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_geran_interrupt_deregister
(
  vstmr_geran_event_tbl_s    *event_tbl
);

void vstmr_geran_interrupt_deregister_from_isr
(
  vstmr_geran_event_tbl_s    *event_tbl
);

extern uint32 vstmr_geran_event_service
(
  vstmr_geran_event_tbl_s    *event_tbl
);

void vstmr_geran_event_sched
(
  vstmr_geran_event_tbl_s    *id,
  uint32                      event_idx,
  vstmr_gstmr_t               evt_expire_cnt
);

void vstmr_geran_event_cancel
(
  vstmr_geran_event_tbl_s    *id,
  uint32                      event_idx
);

uint32 vstmr_geran_vfr_allocate
(
  vstmr_rtc_id        rtc_id,            
  vstmr_geran_view_e  view_id,      
  uint32              vfr_id   
);

void vstmr_geran_vfr_sync
(
  uint32              vfr_id,
  vstmr_gstmr_t       sync_cnt
);

void vstmr_geran_vfr_release
(
  uint32              vfr_id,
  vstmr_rtc_id        rtc_id
);

/*===========================================================================

  FUNCTION:  vstmr_geran_seg_to_rtc

===========================================================================*/
/*!
    @brief
    Convert a segmented GERAN value to RTC.
    The fractional portion is discarded.

    @return
    RTC count in the range [0..VSTMR_GERAN_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_geran_seg_to_rtc
(
  vstmr_gstmr_t  gstmr   /*!< Segmented GSTMR count to convert */
)
{

  return ((gstmr.frame_num * VSTMR_GERAN_FRAME_RANGE) + gstmr.raw_13M);  
  
} /* vstmr_geran_seg_to_rtc() */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_GERAN_H*/

