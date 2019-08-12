/*!
  @file
  vstmr.h

  @brief
  Virtual system timer interface.

  Generic functions used to emulate a tech-specific system timer using 
  a shared common timer source.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/vstmr.h#1 $

===========================================================================*/

#ifndef VSTMR_H
#define VSTMR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <atomic_ops.h>
#include <mcs_hwio.h>
#include <qurt.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief VSTMR API version. Compare this to vstmr_get_api() to ensure
    that VSTMR API is in sync.
*/
#define VSTMR_API_VERSION             (0x6)

/*! @brief Add alias for STMR register */
#ifndef HWIO_MSS_STMR_TIME_RD_IN
  #define MSS_STMR_TIME_RD            STMR_TIME_RD
#endif

/*! @brief RTC identifiers. 
*/
typedef enum
{
  VSTMR_RTC_WCDMA_SUB_START  = 0,
  VSTMR_RTC_WCDMA_SUB0       = VSTMR_RTC_WCDMA_SUB_START,  /*!< WCDMA subscription 0 */
  VSTMR_RTC_WCDMA_SUB1,                                    /*!< WCDMA subscription 1 */
  VSTMR_RTC_WCDMA_SUB_END    = VSTMR_RTC_WCDMA_SUB1,

  VSTMR_RTC_LTE_SUB_START,
  VSTMR_RTC_LTE_SUB0         = VSTMR_RTC_LTE_SUB_START,    /*!< LTE subscription 0 */
  VSTMR_RTC_LTE_SUB1,                                      /*!< LTE subscription 1 */
  VSTMR_RTC_LTE_SUB_END      = VSTMR_RTC_LTE_SUB1,

  VSTMR_RTC_GERAN_SUB_START,
  VSTMR_RTC_GERAN_SUB0       = VSTMR_RTC_GERAN_SUB_START,  /*!< GERAN subscription 0 */
  VSTMR_RTC_GERAN_SUB1,                                    /*!< GERAN subscription 1 */
  VSTMR_RTC_GERAN_SUB_END    = VSTMR_RTC_GERAN_SUB1,

  VSTMR_RTC_HDR_SUB_START,
  VSTMR_RTC_HDR_SUB0         = VSTMR_RTC_HDR_SUB_START,    /*!< HDR subscription 0 */
  VSTMR_RTC_HDR_SUB1,                                      /*!< HDR subscription 1 */
  VSTMR_RTC_HDR_SUB_END      = VSTMR_RTC_HDR_SUB1,

  VSTMR_RTC_1X_SUB_START,
  VSTMR_RTC_1X_SUB0          = VSTMR_RTC_1X_SUB_START,     /*!< 1x subscription 0 */
  VSTMR_RTC_1X_SUB1,                                       /*!< 1x subscription 1 */
  VSTMR_RTC_1X_SUB_END       = VSTMR_RTC_1X_SUB1,

  VSTMR_RTC_TDS_SUB_START,
  VSTMR_RTC_TDS_SUB0         = VSTMR_RTC_TDS_SUB_START,    /*!< TDS subscription 0 */
  VSTMR_RTC_TDS_SUB1,                                      /*!< TDS subscription 1 */
  VSTMR_RTC_TDS_SUB_END      = VSTMR_RTC_TDS_SUB1,

  VSTMR_NUM_RTC,
  VSTMR_RTC_INVALID          = VSTMR_NUM_RTC               /*!< Invalid RTC identifier */
} vstmr_rtc_id;

/*! @brief Bitmasks of comparators with different capabilities.
    STMR[5:0] support HIGH priority, connected to L1 also.
    STMR[31:30] support VFR strobes
    The remainder are NORMAL.
*/
#define VSTMR_INT_HIGH_MASK              (0x0000003F)
#define VSTMR_INT_VFR_MASK               (0xC0000000)
#define VSTMR_INT_VFR_PRI_MASK           (0x40000000)
#define VSTMR_INT_VFR_SEC_MASK           (0x80000000)
#define VSTMR_INT_NORMAL_MASK            (0xFFFFFFFF & ~(VSTMR_INT_HIGH_MASK | VSTMR_INT_VFR_MASK))

/*! @brief Comparator type identifiers.
    HW comparators have different capabilities. This enum allows selecting 
    comparators of a particular type.
    Note that this does not directly affect the priority of the interrupt 
    handler - it only selects comparators which could be used for the specified 
    purpose.
    NORMAL: Normal comparator. Clients with no special preference should use this.
    HIGH:   Interrupt is connected to Hexagon L1 interrupt input, as well as L2.
            These can be registered as either an L1 or an L2 interrupt.
    VFR:    The comparator generates a VFR strobe when it expires. 
            These should be used only when a VFR signal is needed, and only 
            a single event should be pending on them at any time.
*/
typedef enum
{
  VSTMR_INT_NORMAL   =  VSTMR_INT_NORMAL_MASK,  /*!< Normal Priority Interrupt */
  VSTMR_INT_HIGH     =  VSTMR_INT_HIGH_MASK,    /*!< High Priority Interrupt. */
  VSTMR_INT_VFR_PRI  =  VSTMR_INT_VFR_PRI_MASK, /*!< Primary VFR interrupt */
  VSTMR_INT_VFR_SEC  =  VSTMR_INT_VFR_SEC_MASK, /*!< Secondary VFR interrupt */
  VSTMR_INT_VFR_AUTO =  VSTMR_INT_VFR_MASK      /*!< For auto-allocation of VFR */
} vstmr_int_types_e;


/*! @brief Macro to read current XO count.
    This returns the 24-bit MSS STMR count.
    User needs to add the appropriate HWIO #includes - doing it from this
    file will slow down compile time everywhere.
*/
#ifdef TEST_FRAMEWORK
#error code not present
#else
#define VSTMR_XO_READ()                    HWIO_IN(MSS_STMR_TIME_RD)
#define VSTMR_XO_READ_FULL()               vstmr_xo_read_full()
#endif /* TEST_FRAMEWORK */

/*! @brief Number of bits in XO count.
*/
#define VSTMR_NUM_XO_BITS                  24
#define VSTMR_XO_MASK                      ((1<<VSTMR_NUM_XO_BITS)-1)

/*! @brief Duration of epoch in XO counts
*/
#define VSTMR_XO_EPOCH_DURATION            ((1<<VSTMR_NUM_XO_BITS) / 4)

/*! @brief XO rate 19.2 MHz
*/
#define VSTMR_XO_RATE_HZ                   (19200000)

/*! @brief Maximum number of events per event table.
*/
#define VSTMR_MAX_EVENTS                   16 



/*! @brief Extern for opaque RTC handle.
*/
struct vstmr_rtc_s;


/*! @brief Event parameters.
*/
typedef union
{
  /* Note these are overlayed */
  atomic_word_t atomic;      /*!< For atomic access to all parameters */

  uint32 all;                /*!< Access to entire structure */

  uint32 xo_msb_aligned;     /*!< Access to MSb-aligned XO count, for wraparound diff */

  struct {
    uint32 enabled : 1;                  /*!< 1 = event enabled */
    uint32         : (32 - VSTMR_NUM_XO_BITS - 1); 
    uint32 xo_cnt  : VSTMR_NUM_XO_BITS;  /*!< Expiration XO count */
  };

} vstmr_event_item_s;



/*! @brief Event table management structure.
*/
typedef struct 
{
  vstmr_event_item_s events[VSTMR_MAX_EVENTS];  /*!< Event information */ 
  uint16             irq_id;                    /*!< Interrupt ID for comparator.
                                                     When 0, also means this event table is deregistered. */
  uint8              comp_id;                   /*!< Comparator index associated with event table */

  qurt_mutex_t       *comp_mutex_ptr;           /*!< Mutex pointer to protect programming for the HW comparator 
                                                     associated for this event table. By default, its 
                                                     initialized for SW event tables and set to NULL for FW event
                                                     tables. FW has some fastint timelines which can't take 
                                                     a mutex. However, it can be enabled using an API call
                                                     if needed. */ 

  vstmr_event_item_s next_event;                /*!< Next event time stored in comparator */

} vstmr_event_tbl_s;




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern void vstmr_init(void);

extern uint32 vstmr_get_api(void);

extern uint64 vstmr_xo_read_full(void);


extern uint64 vstmr_get_current_rtc
(
  struct vstmr_rtc_s *rtc
);

extern uint64 vstmr_xo_to_rtc
(
  struct vstmr_rtc_s *rtc,   
  uint32 xo          
);

extern uint32 vstmr_rtc_to_xo
(
  struct vstmr_rtc_s *rtc,  
  uint64 rtc_cnt
);

extern uint32 vstmr_rtc_to_xo_ext
(
  struct vstmr_rtc_s *rtc, 
  uint64 rtc_cnt, 
  uint64 *xo_frac 
);

extern uint32 vstmr_get_rtc_fcw
(
  struct vstmr_rtc_s *rtc
);

extern void vstmr_update_rtc_fcw
(
  struct vstmr_rtc_s *rtc,
  uint32 xo_cnt,
  uint32 fcw 
);

extern void vstmr_rtc_sync
(
  struct vstmr_rtc_s *rtc,
  uint32 xo_cnt, 
  uint64 rtc_cnt 
);

extern void vstmr_rtc_adjust_epoch
(
  struct vstmr_rtc_s *rtc,
  uint32 epoch_inc, 
  int32 xo_adj  
);

extern struct vstmr_rtc_s * vstmr_get_rtc_handle
(
  vstmr_rtc_id id
);

extern void vstmr_get_rtc_epoch
(
  struct vstmr_rtc_s *rtc,
  uint32 *xo_epoch,
  uint64 *rtc_epoch,
  uint32 *fcw_epoch
);

extern void vstmr_set_rtc_epoch
(
  struct vstmr_rtc_s *rtc,
  uint32 xo_epoch, 
  uint64 rtc_epoch,
  uint32 fcw_epoch
);




extern void vstmr_event_tbl_register
(
  vstmr_event_tbl_s *event_tbl,
  vstmr_int_types_e int_type
);

extern void vstmr_event_tbl_deregister
(
  vstmr_event_tbl_s *event_tbl
);

extern void vstmr_event_sched
(
  vstmr_event_tbl_s *event_tbl, 
  uint32 event_idx,    
  uint32 xo_count 
);

extern void vstmr_event_cancel
(
  vstmr_event_tbl_s *event_tbl,
  uint32 event_idx   
);

extern uint32 vstmr_event_service
(
  vstmr_event_tbl_s *event_tbl  
);

extern void vstmr_event_tbl_set_mutex
(
  vstmr_event_tbl_s *event_tbl
);


extern uint32 vstmr_epoch_update
(
  int32 xo_adj  
);

extern void vstmr_suspend(void);
extern void vstmr_resume(void);


#ifdef __cplusplus
}  // extern "C"
#endif


#endif /* VSTMR_H */
