/*!
  @file
  vstmr_tds.h

  @brief
  VSTMR TDSCDMA header file

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_tds.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_TDS_H
#define VSTMR_TDS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 
 
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal FCW value for TDS, in Q30.
    TDS:  Cx8 (10.24MHz) / 19.2e6 = (0.533..) * 2^30 = 0x22222222 (rounded)
*/
#define VSTMR_TDS_FCW                   (0x22222222)

/*! @brief Wraparound for TDS RTC.
    8192 subframes at Cx8 (8192*6400*8).
*/
#define VSTMR_TDS_RTC_MODULO            (0x19000000)

/*! @brief Subframe range for TDS in Cx8 = 6400 * 8 (=5msec).
*/
#define VSTMR_TDS_SUBFRAME_RANGE        (51200)

/*! @brief Subframe count range
*/
#define VSTMR_TDS_SUBFRAME_CNT_RANGE    (8192)


/*! Enum to identify TDS views 
*/
typedef enum  
{
  VSTMR_TDS_WALL_TIME_VIEW,  /*!< Wall time view */
  VSTMR_TDS_RX_VIEW,         /*!< RX view */
  VSTMR_TDS_NUM_VIEWS
} vstmr_tds_view_e;

/*! Enum to identify TDS frame phase 
*/
typedef enum  
{
  VSTMR_TDS_FRAME_EVEN = 0,  /*!< Even frame */
  VSTMR_TDS_FRAME_ODD        /*!< Odd frame */
} vstmr_tds_frame_phase_e;

/*! Enum to identify whether an event is periodic or single-shot. 
    Periodicity of 5ms and 10 ms are supported. 
*/
typedef enum  
{
  VSTMR_TDS_SINGLE,         /*!< Single-shot event */
  VSTMR_TDS_PERIODIC_5MS,   /*!< 5MS Periodic event */
  VSTMR_TDS_PERIODIC_10MS   /*!< 10MS Periodic event */
} vstmr_tds_periodicity_e;


/*! Structure to identify TDS segmentation format.
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
       Note all of these are in the MSW of the TDS stmr format. */
    union {

      uint32 full_seg_cx8;         /*!< Full segmented subframe_no+subframecx8 fields.
                                        NOT a flat cx8 count! */

      struct {
        uint32 subframecx8    : 16;  /*!< Subframe bits [0..VSTMR_TDS_SUBFRAME_RANGE - 1] */
        uint32 subframe_no    : 13;  /*!< Subframe number [0..VSTMR_TDS_SUBFRAME_CNT_RANGE - 1] */
        uint32                :  3;  
      };
      struct {
        uint32 subChipCx8     : 3;   /*!< Cx8 count within a chip [0..7] */
        uint32 subFrameCx1    : 13;  /*!< Cx1 count within a subframe (6400 chips) [0..6399] */
        uint32                : 16;
      };
      struct {
        uint32                : 16;  
        uint32 subframe_phase : 1;  /*!< Bit to indicate subframe phase (even or odd) */
        uint32 frame_phase    : 1;  /*!< Bit to indicate frame phase (even or odd) */
        uint32                : 14;
      };
      struct {
        uint32 frame_cx8      : 17; /*!< Frame bits [0..102399]  */
        uint32 frame_no       : 12; /*!< Frame number [0..4095] */
        uint32                : 3; 
      };
    };
  };
} vstmr_tdsstmr_t;


/*! Structure for providing TDS event's time
*/
typedef union {

  uint32 all;     /*!< For clearing entire word */

  struct {
    uint32   subFrameCx1     : 13;  /*!< Event's firing time relative to the start of the 
                                         subframe in cx1 [0..6399] */
    uint32   subframe_phase  : 1;   /*!< 0: Firing time is within even sub frame 
                                         1: Firing time is within odd sub frame */
    uint32   period_ind      : 1;   /*!< 0: Event occurs every 5ms sub frame. Bit 13 is ignored. 
                                         1: Event occurs every 10ms frame. Bit 13 is effective. */
    uint32                   : 17;  
  };
} vstmr_tds_event_time_t;


/*! @brief TDS view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s *rtc;       /*!< Pointer to the RTC struct */ 
  uint32              offset;    /*!< Offset for this view in TDS RTC units, 
                                      Note: Its a uint32, so no fractional part. */
} vstmr_tds_view_s;

/*! @brief TDS event table management structure.
*/
typedef struct vstmr_tds_event_tbl_s_
{
  vstmr_event_tbl_s         event_tbl_data; /*!< Internal event table data, stores 
                                                 event expiration counts in XO 
                                                 resolution. */

  /* TDS specific fields */
  vstmr_tds_view_s*         view;           /*!< Pointer to the associated view.
                                                 View is the source of time for this
                                                 event table */  

  uint32                    events_seg_cnt[VSTMR_MAX_EVENTS]; 
  /*!< Array to store event's expiration time in TDSSTMR resolution (no fractional 
       part). This is in the format of vstmr_tdsstmr_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */

  vstmr_tds_periodicity_e   periodicity_type[VSTMR_MAX_EVENTS];    
  /*!< Array to store periodicity type for the event */  
                                                               
  void (*isr_callback)(struct vstmr_tds_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< TDS callback routine which TDS VSTMR layer calls once an event expires */   

  void *isr_arg;
  /*!< Argument for the callback routine */  

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered TDS callback.
       Servicing the event table involves parsing the event table to find out what
       all events have expired and then reprogramming the comparator for the
       next event. */
       
  atomic_word_t             service_sync_flag;  /*!< Flag to synchronize event table 
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
           
  atomic_word_t             isr_sync_flag;  /*!< Flag to synchronize ISR handler and 
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
         
} vstmr_tds_event_tbl_s;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

vstmr_tdsstmr_t vstmr_tds_segment_cnt
(
  uint64 rtc                 
);

vstmr_tds_view_s * vstmr_tds_view_init
( 
  vstmr_rtc_id       rtc_id,       
  vstmr_tds_view_e   view_id,      
  vstmr_tdsstmr_t    offset        
);

vstmr_tds_view_s * vstmr_tds_get_view_handle
(
  vstmr_rtc_id       rtc_id,      
  vstmr_tds_view_e   view_id      
);

vstmr_tdsstmr_t vstmr_tds_get_current_stmr
(
  vstmr_tds_view_s  *t_view      
);

vstmr_tdsstmr_t vstmr_tds_xo_to_stmr
(
  vstmr_tds_view_s  *t_view,
  uint32             xo_cnt              
);

uint32 vstmr_tds_stmr_to_xo
(
  vstmr_tds_view_s  *t_view,           
  vstmr_tdsstmr_t    tstmr        
);

void vstmr_tds_stmr_sync
(
  vstmr_tds_view_s  *t_view,        
  vstmr_tdsstmr_t    tstmr,    
  uint32             xo_cnt    
);

void vstmr_tds_view_set_offset
(
  vstmr_tds_view_s  *t_view,      
  vstmr_tdsstmr_t    offset       
);

void vstmr_tds_view_adjust_offset
(
  vstmr_tds_view_s  *t_view,      
  int32              adjust_cx8   
);

void vstmr_tds_update_fcw
(
  vstmr_tds_view_s  *t_view,
  uint32             fcw
);

void vstmr_tds_event_tbl_register
(
  vstmr_tds_event_tbl_s    *event_tbl,        
  vstmr_tds_view_s         *t_view,            
  vstmr_int_types_e         int_type          
);

void vstmr_tds_event_tbl_deregister
(
  vstmr_tds_event_tbl_s  *event_tbl      
);

extern uint32 vstmr_tds_event_service
(
  vstmr_tds_event_tbl_s  *event_tbl      
);

void vstmr_tds_interrupt_register
(
  vstmr_tds_event_tbl_s  *event_tbl,
  void (*cb)(vstmr_tds_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_tds_interrupt_deregister
(
  vstmr_tds_event_tbl_s  *event_tbl      
);

void vstmr_tds_interrupt_deregister_from_isr
(
  vstmr_tds_event_tbl_s  *event_tbl      
);

void vstmr_tds_event_sched
(
  vstmr_tds_event_tbl_s*  id,
  uint32                  event_idx,
  vstmr_tdsstmr_t         evt_expire_cnt
);

void vstmr_tds_event_sched_single
(
  vstmr_tds_event_tbl_s*  id,
  uint32                  event_idx,
  uint32                  subframe_offset_cx1
);

void vstmr_tds_event_sched_periodic
(
  vstmr_tds_event_tbl_s*   id,
  uint32                   event_idx,
  vstmr_tds_event_time_t   event_time
);

void vstmr_tds_event_cancel
(
  vstmr_tds_event_tbl_s  *id,            
  uint32                  event_idx      
);

uint32 vstmr_tds_vfr_allocate
(
  vstmr_rtc_id       rtc_id,            
  vstmr_tds_view_e   view_id,      
  uint32             vfr_id   
);

void vstmr_tds_vfr_sync
(
  uint32                   vfr_id,
  vstmr_tds_frame_phase_e  frame_phase  
);

void vstmr_tds_vfr_release
(
  uint32             vfr_id,
  vstmr_rtc_id       rtc_id
);


/*===========================================================================

  FUNCTION:  vstmr_tds_seg_to_cx8

===========================================================================*/
/*!
    @brief
    Convert a segmented TDS stmr value to linear Cx8.
    The fractional portion is discarded.

    @return
    Linear Cx8 count in the range [0..VSTMR_TDS_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_tds_seg_to_cx8
(
  vstmr_tdsstmr_t  tstmr   /*!< Segmented TDS stmr count to convert */
)
{

  return ((tstmr.subframe_no * VSTMR_TDS_SUBFRAME_RANGE) + tstmr.subframecx8);  
  
} /* vstmr_tds_seg_to_cx8() */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_TDS_H*/

