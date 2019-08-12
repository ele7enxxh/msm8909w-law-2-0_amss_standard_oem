/*!
  @file
  vstmr_lte.h

  @brief
  VSTMR LTE header file

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr_lte.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/13   rj      Initial rev

===========================================================================*/

#ifndef VSTMR_LTE_H
#define VSTMR_LTE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "vstmr.h"
 
 
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Nominal LTE FCW value in Q30.
    LTE OSTMR: (30.72 MHz) / 19.2e6 = 1.6 * 2^30 = 0x66666666 (rounded)
*/
#define VSTMR_LTE_FCW               (0x66666666)

/*! @brief Number of bits in OSTMR count 
*/
#define VSTMR_LTE_OSTMR_CNT_BITS    (30)

/*! @brief OSTMR count mask 
*/
#define VSTMR_LTE_OSTMR_CNT_MASK    ((1U << VSTMR_LTE_OSTMR_CNT_BITS) - 1)

/*! @brief Wraparound for LTE RTC (OSTMR)
*/
#define VSTMR_LTE_RTC_MODULO        (1U << VSTMR_LTE_OSTMR_CNT_BITS)


/*! Enum to identify LTE views
*/
typedef enum  
{
  VSTMR_LTE_OSTMR_VIEW,      /*!< LTE OSTMR view */
  VSTMR_LTE_NUM_VIEWS
} vstmr_lte_view_e;

/*! Structure to identify LTE segmentation format.
    Note that this is a 64-bit quantity. 
    The lower 32 bits are sub-OSTMR fractional count.
*/
typedef union {

  /* Types for internal manipulation */
  uint64 d;
  uint32 w[2];

  uint64 all;     /*!< For clearing entire dword */

  struct {
    uint32 frac;                /*!< Fractional bits, sub-OSTMR */

    struct {
      uint32 ostmr_cnt    : 30;  /*!< OSTMR count: 30 bits */
      uint32              :  2;  
    };
  };
} vstmr_ostmr_t;


/*! @brief LTE view management structure.
*/
typedef struct 
{
  struct vstmr_rtc_s *rtc;       /*!< Pointer to the RTC struct */ 
  uint32              offset;    /*!< Offset for this view in LTE RTC units (OSTMR), 
                                      Note: Its a uint32, so no fractional part. */
} vstmr_lte_view_s;


/*! @brief LTE event table management structure.
*/
typedef struct vstmr_lte_event_tbl_s_
{
  vstmr_event_tbl_s   event_tbl_data;  /*!< Internal event table data, stores 
                                            event expiration counts in XO 
                                            resolution. */ 

  /* LTE specific fields */
  vstmr_lte_view_s   *view;         /*!< Pointer to the associated view.
                                         View is the source of time for this
                                         event table */ 

  atomic_word_t       sync_flag;      /*!< Flag to synchronize event table 
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

} vstmr_lte_event_tbl_s;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


vstmr_lte_view_s * vstmr_lte_view_init
( 
  vstmr_rtc_id       rtc_id,
  vstmr_lte_view_e   view_id,
  vstmr_ostmr_t      offset
);

vstmr_lte_view_s * vstmr_lte_get_view_handle
(
  vstmr_rtc_id       rtc_id,
  vstmr_lte_view_e   view_id
);

vstmr_ostmr_t vstmr_lte_get_current_ostmr
(
  vstmr_lte_view_s  *l_view
);

vstmr_ostmr_t vstmr_lte_xo_to_ostmr
(
  vstmr_lte_view_s  *l_view,
  uint32 xo_cnt              
);

uint32 vstmr_lte_ostmr_to_xo
(
  vstmr_lte_view_s  *l_view,
  vstmr_ostmr_t      ostmr
);

uint64 vstmr_lte_ostmr_to_xo_frac
(
  vstmr_lte_view_s  *l_view,           
  vstmr_ostmr_t      ostmr        
);

void vstmr_lte_ostmr_sync
(
  vstmr_lte_view_s  *l_view,        
  vstmr_ostmr_t      ostmr,    
  uint32             xo_cnt    
);

void vstmr_lte_view_set_offset
(
  vstmr_lte_view_s  *l_view,
  vstmr_ostmr_t      offset
);

void vstmr_lte_view_adjust_offset
(
  vstmr_lte_view_s  *l_view,
  int32              adjust
);

void vstmr_lte_update_fcw
(
  vstmr_lte_view_s  *l_view,
  uint32 fcw
);

void vstmr_lte_event_tbl_register
(
  vstmr_lte_event_tbl_s    *event_tbl,
  vstmr_lte_view_s         *l_view,
  vstmr_int_types_e         int_type
);

void vstmr_lte_event_tbl_deregister
(
  vstmr_lte_event_tbl_s    *event_tbl
);

extern uint32 vstmr_lte_event_service
(
  vstmr_lte_event_tbl_s    *event_tbl
);

void vstmr_lte_event_sched
(
  vstmr_lte_event_tbl_s    *id,
  uint32                    event_idx,
  vstmr_ostmr_t             ostmr_cnt
);

void vstmr_lte_event_cancel
(
  vstmr_lte_event_tbl_s    *id,
  uint32                    event_idx
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VSTMR_LTE_H*/

