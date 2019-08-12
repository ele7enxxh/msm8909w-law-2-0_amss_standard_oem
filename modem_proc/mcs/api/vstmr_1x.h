/*!
  @file
  vstmr_1x.h

  @brief
  VSTMR 1x header file

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_1x.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_1X_H
#define VSTMR_1X_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 
 
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal 1x FCW value in Q30.
    CDMA Cx8: (1.2288 MHz * 8) / 19.2e6 = 0.512 * 2^30 = 0x20c49ba6 (rounded) 
*/
#define VSTMR_1X_FCW               (0x20c49ba6)

/*! @brief Wraparound for 1x RTC (10.24 sec) 
           (2^6 (strobe160ms) * 2 (sync80) * VSTMR_1X_SYNC80_RANGE)  
*/
#define VSTMR_1X_RTC_MODULO        (0x06000000)

/*! @brief Wraparound for SYNC80 = (3 (26.6ms period) * 32768 (PN chips) * 8 (cx8))  
*/
#define VSTMR_1X_SYNC80_RANGE      (0xC0000)

/*! @brief Wraparound for SYNC80 count  
*/
#define VSTMR_1X_SYNC80_CNT_RANGE  128

/*! @brief Number of PN roll's within a SYNC80   
*/
#define VSTMR_1X_SYNC80_PNROLL_CNT 3

/*! @brief Wraparound for PCG (cx8) = (1536 (PCG chips) * 8 (cx8))  
*/
#define VSTMR_1X_PCG_RANGE         (0x3000)

/*! @brief Wraparound for PCG count  
*/
#define VSTMR_1X_PCG_CNT_RANGE     8192    

/*! @brief Number of PCG's within a frame   
*/
#define VSTMR_1X_FRAME_PCG_NUM     16

/*! @brief Wraparound for frame (cx1)   
*/
#define VSTMR_1X_FRAME_RANGE_CX1   ((VSTMR_1X_PCG_RANGE * VSTMR_1X_FRAME_PCG_NUM) >> 3)
 

/*! Enum to identify 1x views
*/
typedef enum  
{
  VSTMR_1X_RTC_VIEW,   /*!< 1x RTC view (analogous to RTC_CNT_1X in legacy HW) */
  VSTMR_1X_RX_VIEW,    /*!< 1x RX view (analogous to DEM1X_COMBINER_TIME in legacy HW) */
  VSTMR_1X_TX_VIEW,    /*!< 1x TX view (analogous to RTC_MOD_TX_SYS_TIME in legacy HW) */
  VSTMR_1X_NUM_VIEWS
} vstmr_1x_view_e;


/*! Structure to identify 1x segmentation (symbol combiner format).
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
       Note all of these are in the MSW of the 1xscomb format. */
    union{
      struct {
        uint32  cx8           : 3;  /*!< [0..7] */
        uint32  chips         : 15; /*!< 26.66 ms PN rollover [0..32767] */
        uint32  pnroll_num    : 2;  /*!< MOD3 count for three 26.66ms 
                                         period (SYNC26 strobe) [0..2] */
        uint32  sync80        : 1;  /*!< Indicates first or second 80 ms 
                                         period (SYNC80 strobe) */     
        uint32  strobe160ms   : 6;  /*!< 160ms periods count, 10.24 sec 
                                         range [0..63] */     
        uint32                : 5;       
      };
      struct {
        uint32  sync80_cx8    : 20; /*!< sync80 cx8 count 
                                         [0..VSTMR_1X_SYNC80_RANGE-1] */ 
        uint32  sync80_cnt    : 7;  /*!< [0..VSTMR_1X_SYNC80_CNT_RANGE - 1] */     
        uint32                : 5;       
      };
    };
  };
}vstmr_1xscomb_t;


/*! Structure to identify 1x segmentation (frame format).
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
       Note all of these are in the MSW of the 1xframe format. */
    union{
      struct {
        uint32  cx8          : 3;  /*!< [0..7] */
        uint32  chip_num     : 11; /*!< Chips within PCG [0..1535] */
        uint32  pcg_num      : 4;  /*!< PCG's within a frame [0..15] */
        uint32  frame_num    : 9;  /*!< Frame counter (10.24 sec range) [0..511] */     
        uint32               : 5;       
      };
      struct {
        uint32  pcg_cx8      : 14; /*!< [0..VSTMR_1X_PCG_RANGE-1] */ 
        uint32  pcg_cnt      : 13; /*!< [0..VSTMR_1X_PCG_CNT_RANGE - 1] */     
        uint32               : 5;       
      };
      struct {
        uint32               : 3; 
        uint32  frame_cx1    : 15; /*!< [0..VSTMR_1X_FRAME_RANGE_CX1-1] */     
        uint32               : 14;       
      };
    };
  };
}vstmr_1xframe_t;


/*! @brief 1x view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s *rtc;       /*!< Pointer to the RTC struct */ 
  uint32              offset;    /*!< Offset for this view in 1x RTC units (Cx8), 
                                      Note: Its a uint32, so no fractional part. */
} vstmr_1x_view_s;

/*! @brief 1x (symbol combiner format) event table management structure.
*/
typedef struct vstmr_1xscomb_event_tbl_s_
{
  vstmr_event_tbl_s   event_tbl_data;  /*!< Internal event table data, stores 
                                            event expiration counts in XO 
                                            resolution. */

  /* 1x specific fields */
  vstmr_1x_view_s    *view;            /*!< Pointer to the associated view
                                            View is the source of time for this
                                            event table */  

  uint32              events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in 1xscomb resolution (no fractional 
       part). This is in the format of vstmr_1xscomb_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */

  uint32              events_period[VSTMR_MAX_EVENTS];   
  /*!< Array to store periodic count for the events within the event table in 
       1xscomb resolution */ 

  void (*isr_callback)(struct vstmr_1xscomb_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< 1X callback routine which 1X VSTMR layer calls once an event expires */   

  void *isr_arg;
  /*!< Argument for the callback routine */  

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered 1X callback.
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
       < ... function body ...>  
                                                                                         */
} vstmr_1xscomb_event_tbl_s;

/*! @brief 1x (frame format) event table management structure.
*/
typedef struct vstmr_1xframe_event_tbl_s_
{
  vstmr_event_tbl_s   event_tbl_data;  /*!< Internal event table data, stores 
                                            event expiration counts in XO 
                                            resolution. */

  /* 1x specific fields */
  vstmr_1x_view_s    *view;            /*!< Pointer to the associated view
                                            View is the source of time for this
                                            event table */  

  uint32              events_seg_cnt[VSTMR_MAX_EVENTS];  
  /*!< Array to store event's expiration time in 1xframe resolution (no fractional 
       part). This is in the format of vstmr_1xframe_t. We only store the upper 32
       bits here as the lower 32 bits are fractional. There are in all
       VSTMR_MAX_EVENTS events for an event table. If a value is 0, that just
       means that event is not in use. */

  uint32              events_period[VSTMR_MAX_EVENTS];   
  /*!< Array to store periodic count for the events within the event table in 
       1xframe resolution */ 

  void (*isr_callback)(struct vstmr_1xframe_event_tbl_s_ *event_tbl, void *arg, uint32 pending);
  /*!< 1X callback routine which 1X VSTMR layer calls once an event expires */   

  void *isr_arg;
  /*!< Argument for the callback routine */  

  /*!< When the comparator fires, ISR calls the registered VSTMR callback function. 
       VSTMR function services the event table, and then calls registered 1X callback.
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
       < ... function body ...>                                                                */
                                                              
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
       < ... function body ...>                                                           */
                                                                                                                                                        
} vstmr_1xframe_event_tbl_s;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


vstmr_1xscomb_t vstmr_1x_scomb_segment_cnt
(
  uint64 rtc                    
);

vstmr_1xframe_t   vstmr_1x_frame_segment_cnt
(
  uint64 rtc                    
);

uint64 vstmr_1x_flatten_frame_cnt
(
  vstmr_1xframe_t    frame_seg             
);

uint64 vstmr_1x_flatten_scomb_cnt
(
  vstmr_1xscomb_t    scomb_seg             
);

vstmr_1x_view_s * vstmr_1x_get_view_handle
(
  vstmr_rtc_id       rtc_id,      
  vstmr_1x_view_e    view_id      
);

uint64 vstmr_1x_xo_to_rtc
(
  vstmr_1x_view_s   *view,
  uint32             xo_cnt      
);

uint64 vstmr_1x_get_rtc
(
  vstmr_1x_view_s   *view      
);

vstmr_1xscomb_t vstmr_1x_get_current_scomb
(
  vstmr_1x_view_s   *view      
);

vstmr_1xframe_t vstmr_1x_get_current_frame
(
  vstmr_1x_view_s   *view      
);

vstmr_1xscomb_t vstmr_1x_xo_to_scomb
(
  vstmr_1x_view_s   *view,
  uint32             xo_cnt           
);

vstmr_1xframe_t vstmr_1x_xo_to_frame
(
  vstmr_1x_view_s   *view,
  uint32             xo_cnt              
);

uint64 vstmr_1x_scomb_to_rtc
(
  vstmr_1x_view_s   *view,         
  vstmr_1xscomb_t    stmr     
);

uint64 vstmr_1x_frame_to_rtc
(
  vstmr_1x_view_s   *view,         
  vstmr_1xframe_t    stmr     
);

uint32 vstmr_1x_scomb_to_xo
(
  vstmr_1x_view_s   *view,        
  vstmr_1xscomb_t    scomb   
);

uint32 vstmr_1x_frame_to_xo
(
  vstmr_1x_view_s   *view,        
  vstmr_1xframe_t    frame   
);

void vstmr_1x_scomb_sync
(
  vstmr_1x_view_s   *view,        
  vstmr_1xscomb_t    stmr,   
  uint32             xo_cnt  
);

void vstmr_1x_frame_sync
(
  vstmr_1x_view_s   *view,        
  vstmr_1xframe_t    stmr,   
  uint32             xo_cnt  
);

uint32 vstmr_1x_view_get_offset
(
  vstmr_1x_view_s   *view
);

void vstmr_1x_view_set_offset
(
  vstmr_1x_view_s   *view,        
  uint32             offset       
);

void vstmr_1x_view_adjust_offset
(
  vstmr_1x_view_s   *view,        
  int32              adjust_cx8   
);

uint32 vstmr_1x_get_fcw
(
  vstmr_1x_view_s   *view
);

void vstmr_1x_update_fcw
(
  vstmr_1x_view_s   *view,
  uint32             fcw
);

void vstmr_1xscomb_event_tbl_register
(
  vstmr_1xscomb_event_tbl_s  *event_tbl,      
  vstmr_1x_view_s            *view,            
  vstmr_int_types_e           int_type        
);

void vstmr_1xframe_event_tbl_register
(
  vstmr_1xframe_event_tbl_s  *event_tbl,      
  vstmr_1x_view_s            *view,            
  vstmr_int_types_e           int_type        
);

void vstmr_1xscomb_event_tbl_deregister
(
  vstmr_1xscomb_event_tbl_s   *event_tbl      
);

void vstmr_1xframe_event_tbl_deregister
(
  vstmr_1xframe_event_tbl_s   *event_tbl      
);

uint32 vstmr_1xscomb_event_service_single
(
  vstmr_1xscomb_event_tbl_s  *event_tbl      
);

uint32 vstmr_1xscomb_event_service_periodic
(
  vstmr_1xscomb_event_tbl_s  *event_tbl      
);

uint32 vstmr_1xframe_event_service_single
(
  vstmr_1xframe_event_tbl_s  *event_tbl      
);

uint32 vstmr_1xframe_event_service_periodic
(
  vstmr_1xframe_event_tbl_s  *event_tbl      
);

void vstmr_1xscomb_interrupt_register
(
  vstmr_1xscomb_event_tbl_s  *event_tbl,
  void (*cb)(vstmr_1xscomb_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_1xframe_interrupt_register
(
  vstmr_1xframe_event_tbl_s  *event_tbl,
  void (*cb)(vstmr_1xframe_event_tbl_s *event_tbl, void *arg, uint32 pending),
  void *cb_arg
);

void vstmr_1xscomb_interrupt_deregister
(
  vstmr_1xscomb_event_tbl_s  *event_tbl      
);

void vstmr_1xframe_interrupt_deregister
(
  vstmr_1xframe_event_tbl_s  *event_tbl      
);

void vstmr_1xscomb_event_sched_single
(
  vstmr_1xscomb_event_tbl_s  *id,
  uint32                      event_idx,
  vstmr_1xscomb_t             evt_expire_cnt
);

void vstmr_1xscomb_event_sched_periodic
(
  vstmr_1xscomb_event_tbl_s  *id,
  uint32                      event_idx,
  vstmr_1xscomb_t             evt_expire_cnt,
  vstmr_1xscomb_t             period
);

void vstmr_1xframe_event_sched_single
(
  vstmr_1xframe_event_tbl_s  *id,
  uint32                      event_idx,
  vstmr_1xframe_t             evt_expire_cnt
);

void vstmr_1xframe_event_sched_periodic
(
  vstmr_1xframe_event_tbl_s  *id,
  uint32                      event_idx,
  vstmr_1xframe_t             evt_expire_cnt,
  vstmr_1xframe_t             period
);

void vstmr_1xscomb_event_cancel
(
  vstmr_1xscomb_event_tbl_s  *id,            
  uint32                      event_idx      
);

void vstmr_1xframe_event_cancel
(
  vstmr_1xframe_event_tbl_s  *id,            
  uint32                      event_idx      
);

uint32 vstmr_1x_vfr_allocate
(
  vstmr_rtc_id       rtc_id,                 
  vstmr_1x_view_e    view_id,           
  uint32             vfr_id,
  uint32             frame_offset_cx1
);

void vstmr_1x_vfr_release
(
  uint32             vfr_id,
  vstmr_rtc_id       rtc_id
);

/*===========================================================================

  FUNCTION:  vstmr_1x_scomb_to_cx8

===========================================================================*/
/*!
    @brief
    Convert a segmented 1XSTMR (symbol combiner format) to linear Cx8.
    The fractional portion is discarded.

    @return
    Linear Cx8 count in the range [0..VSTMR_1X_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_1x_scomb_to_cx8
(
  vstmr_1xscomb_t  scomb_stmr   /*!< Segmented 1XSTMR count (symbol combiner format)
                                     to convert */
)
{

  return ((scomb_stmr.sync80_cnt * VSTMR_1X_SYNC80_RANGE) + scomb_stmr.sync80_cx8);  
  
} /* vstmr_1x_scomb_to_cx8() */


/*===========================================================================

  FUNCTION:  vstmr_1x_frame_to_cx8

===========================================================================*/
/*!
    @brief
    Convert a segmented 1XSTMR (frame format) to linear Cx8.
    The fractional portion is discarded.

    @return
    Linear Cx8 count in the range [0..VSTMR_1X_RTC_MODULO-1].
*/
/*=========================================================================*/
INLINE uint32 vstmr_1x_frame_to_cx8
(
  vstmr_1xframe_t  frame_stmr   /*!< Segmented 1XSTMR count (frame format) 
                                    to convert */
)
{

  return ((frame_stmr.pcg_cnt * VSTMR_1X_PCG_RANGE) + frame_stmr.pcg_cx8);  
  
} /* vstmr_1x_seg_frame_to_cx8() */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_1X_H*/
