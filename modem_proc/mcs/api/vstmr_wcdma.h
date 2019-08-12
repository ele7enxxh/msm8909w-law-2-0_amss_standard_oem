/*!
  @file
  vstmr_wcdma.h

  @brief
  VSTMR WCDMA header file
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_wcdma.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/13   rj      Add "word" to WSTMR struct 
02/13/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_WCDMA_H
#define VSTMR_WCDMA_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal FCW value in Q30.
    WCDMA:  Cx8 (30.72MHz) / 19.2e6 = 1.6 * 2^30 = 0x66666666  (rounded)
*/
#define VSTMR_WCDMA_FCW                   (0x66666666)

/*! @brief Wraparound for WCDMA RTC.
    4096 frames at cx8 (4096*150*256*8).
*/
#define VSTMR_WCDMA_RTC_MODULO            (0x4b000000)

/*! @brief Subframe range for WSTMR in cx8 (=10msec @ 30.72MHz).
*/
#define VSTMR_WCDMA_SUBFRAME_RANGE        (256 * 150 * 8)

/*! @brief WSTMR frame count range 
*/
#define VSTMR_WCDMA_FRAME_CNT_RANGE       (4096)


/*! Enum to identify W views 
*/
typedef enum  
{
  VSTMR_WCDMA_REF_COUNT_VIEW,    /*!< Unmodified RTC reference count */
  VSTMR_WCDMA_TX_COUNT_VIEW,
  VSTMR_WCDMA_COMB_0_VIEW,       /*!< SW combiner references */
  VSTMR_WCDMA_COMB_1_VIEW,
  VSTMR_WCDMA_COMB_2_VIEW,
  VSTMR_WCDMA_NUM_VIEWS
} vstmr_wcdma_view_e;


/*! Structure to identify W segmentation format.
    Note that this is a 64-bit quantity. 
    The lower 32 bits are sub-cx8 fractional count.
*/
typedef union {

  /* Types for internal manipulation */
  uint64 d;
  uint32 w[2];
  
  uint64 all;     /*!< For clearing entire dword */

  struct {
    uint32 frac;                   /*!< Fractional bits, sub-cx8 */

    /* Multiple representations of the same segmented counts.
       Note all of these are in the MSW of the WSTMR format. */
    union {

      uint32 full_seg_cx8;         /*!< Full segmented frame+subframecx8 fields.
                                        NOT a flat cx8 count! */
      uint32 word;                 /*!< Legacy WFW code needs this */

      struct {
        uint32 subframecx8    : 19;  /*!< Subframe bits, range VSTMR_WCDMA_SUBFRAME_RANGE, 
                                          NOTE: Entire range for 19 bits are not used. */
        uint32 frame_no       : 12;  /*!< 12 Bits for counting frames, range VSTMR_WCDMA_FRAME_CNT_RANGE */
        uint32                : 1;  
      };
      struct {
        uint32 sub_bpg_cx8    : 11;  /*!< Bits within a bpg, range 256*8 */ 
        uint32 subframebpg    : 8;   /*!< BPG within a subframe, range 256*150 */
        uint32 conn_frame_no  : 8;   /*!< Connection frame number, [0..255] */
        uint32                : 5;  
      };

      /* Aliases for backward compatibility with WFW STMR format. */
      struct {
        uint32 subBpgCx8      : 11;  /*!< Cx8 count within a bpg [0..2047] */
        uint32 subFrameBpg    : 8;   /*!< BPG count within a frame [0..149] */
        uint32 frameNum       : 12;  /*!< Frame number [0..4095] */
        uint32                : 1; 
      };
      struct {
        uint32                : 11;
        uint32 bpgFrameStruct : 20; /*!< BPG count and frame count structure */
        uint32                : 1; 
      };
      struct {
        uint32 subChipCx8     : 3;   /*!< Cx8 count within a chip [0..7] */
        uint32 subBpgCx1      : 8;   /*!< Cx1 count within a bpg [0..255] */
        uint32                : 21;
      };
      struct {
        uint32 subFrameCx8    : 19;  /*!< Cx8 count within a frame [0..307199] */
        uint32                : 13;   
      };
      struct {
        uint32                : 3;
        uint32 subFrameCx1    : 16;  /*!< Cx1 count within a frame [0..38399] */
        uint32                : 13;
      };
      struct {
        uint32                : 2;
        uint32 subFrameCx2    : 17;  /*!< Cx2 count within a frame [0..38399] */
        uint32                : 13;
      };
    };
  };
} vstmr_wstmr_t;


/*! @brief WCDMA view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s  *rtc;       /*!< Pointer to the RTC struct */ 
  uint32               offset;    /*!< Offset for this view in WCDMA RTC units, 
                                       Note: Its a uint32, so no fractional part. */
} vstmr_wcdma_view_s;


/*! @brief WCDMA event table management structure.
*/
typedef struct vstmr_wcdma_event_tbl_s_
{
  vstmr_event_tbl_s     event_tbl_data;  /*!< Internal event table data, stores 
                                              event expiration counts in XO 
                                              resolution. */

  /* WCDMA specific fields */
  vstmr_wcdma_view_s*   view;            /*!< Pointer to the associated view. 
                                              View is the source of time for this
                                              event table */ 

  uint32                events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in WSTMR resolution (no fractional 
       part). This is in the format of vstmr_wstmr_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */
   
  void (*isr_callback)(struct vstmr_wcdma_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< W callback routine which W VSTMR layer calls once an event expires */    

  void *isr_arg;
  /*!< Argument for the callback routine */    

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered W callback.
       Servicing the event table involves parsing the event table to find out what
       all events have expired and then reprogramming the comparator for the
       next event. */

  atomic_word_t         service_sync_flag;     /*!< Flag to synchronize event table
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

  atomic_word_t         isr_sync_flag;         /*!< Flag to synchronize ISR handler and 
                                                    interrupt deregistration.
                                                    0: Interrupt is deregistered OR ISR
                                                       handler is active 
                                                    1: Interrupt is registered and is not 
                                                       being serviced by an ISR 

    Race condition between ISR handler and interrupt deregistration:
      1) ISR handler is active and interrupt deregistration is called from
         a parallel thread. Parallel thread will poll until ISR handler completes
         and will then proceed with deregistration.
      2) Interrupt deregistraion has already started and then ISR handler starts.
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

} vstmr_wcdma_event_tbl_s;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

vstmr_wstmr_t vstmr_wcdma_segment_cnt
(
  uint64 rtc              
);

vstmr_wcdma_view_s * vstmr_wcdma_view_init
( 
  vstmr_rtc_id         rtc_id,
  vstmr_wcdma_view_e   view_id,
  vstmr_wstmr_t        offset
);

vstmr_wcdma_view_s * vstmr_wcdma_get_view_handle
(
  vstmr_rtc_id         rtc_id,
  vstmr_wcdma_view_e   view_id
);

vstmr_wstmr_t vstmr_wcdma_get_current_wstmr
(
  vstmr_wcdma_view_s  *w_view
);

vstmr_wstmr_t vstmr_wcdma_xo_to_wstmr
(
  vstmr_wcdma_view_s  *w_view,
  uint32               xo_cnt  
);

uint32 vstmr_wcdma_wstmr_to_xo
(
  vstmr_wcdma_view_s  *w_view,
  vstmr_wstmr_t        wstmr
);

void vstmr_wcdma_wstmr_sync
(
  vstmr_wcdma_view_s  *w_view,        
  vstmr_wstmr_t        wstmr,    
  uint32               xo_cnt    
);

void vstmr_wcdma_view_set_offset
(
  vstmr_wcdma_view_s  *w_view,
  vstmr_wstmr_t        offset
);

void vstmr_wcdma_view_adjust_offset
(
  vstmr_wcdma_view_s  *w_view,
  int32                adjust_cx8
);

void vstmr_wcdma_update_fcw
(
  vstmr_wcdma_view_s  *w_view,
  uint32               fcw
);

void vstmr_wcdma_event_tbl_register
(
  vstmr_wcdma_event_tbl_s   *event_tbl,
  vstmr_wcdma_view_s        *view,
  vstmr_int_types_e         int_type
);

void vstmr_wcdma_event_tbl_deregister
(
  vstmr_wcdma_event_tbl_s   *event_tbl
);

void vstmr_wcdma_update_event_tbl_view
(
  vstmr_wcdma_event_tbl_s   *event_tbl,    
  vstmr_wcdma_view_s        *view          
);

extern uint32 vstmr_wcdma_event_service
(
  vstmr_wcdma_event_tbl_s   *event_tbl
);

void vstmr_wcdma_interrupt_register
(
  vstmr_wcdma_event_tbl_s   *event_tbl,
  void (*cb)(vstmr_wcdma_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_wcdma_interrupt_deregister
(
  vstmr_wcdma_event_tbl_s   *event_tbl
);

void vstmr_wcdma_event_sched_full
(
  vstmr_wcdma_event_tbl_s   *id,       
  uint32                     event_idx, 
  vstmr_wstmr_t              w_seg      
);

void vstmr_wcdma_event_sched
(
  vstmr_wcdma_event_tbl_s  *id,
  uint32                    event_idx,
  uint32                    subframe_offset_cx1
);

void vstmr_wcdma_event_sched_oneshot
(
  vstmr_wcdma_event_tbl_s  *id,
  uint32                    event_idx,
  uint32                    subframe_offset_cx1
);

void vstmr_wcdma_event_cancel
(
  vstmr_wcdma_event_tbl_s  *id,
  uint32                    event_idx
);

void vstmr_wcdma_event_cancel_mask
(
  vstmr_wcdma_event_tbl_s  *id,              
  uint16                    event_idx_mask   
);

uint32 vstmr_wcdma_vfr_allocate
(
  vstmr_rtc_id        rtc_id,            
  vstmr_wcdma_view_e  view_id,      
  uint32              vfr_id       
);

void vstmr_wcdma_vfr_release
(
  uint32              vfr_id,
  vstmr_rtc_id        rtc_id
);


/*===========================================================================

  FUNCTION:  vstmr_wcdma_seg_to_cx8

===========================================================================*/
/*!
    @brief
    Convert a segmented WSTMR value to linear cx8.
    The fractional portion is discarded.

    @return
    Linear cx8 count in the range [0..VSTMR_WCDMA_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_wcdma_seg_to_cx8
(
  vstmr_wstmr_t  wstmr   /*!< Segmented WSTMR count to convert */
)
{
  return ((wstmr.frame_no * VSTMR_WCDMA_SUBFRAME_RANGE) + wstmr.subframecx8);  
  
} /* vstmr_wcdma_seg_to_cx8() */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_WCDMA_H*/

