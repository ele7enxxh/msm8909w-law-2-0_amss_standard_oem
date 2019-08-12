#ifndef A2_UL_PHYI_H
#define A2_UL_PHYI_H
/*!
  @file a2_ul_phyi.h

  A2 ul phy internal interface file

  @ingroup a2_int_interface
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/a2/driver/src/a2_ul_phyi.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/06/16   am      CR996933 Updated ULPHY macro and DS
12/05/14   sn      CR767131: QSH initial version

==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include <a2_dbg.h>
#include <a2_ul_phy.h>
#include <a2_taskq.h>
#include <a2_hw_constants.h>
#include <a2_hw_task_defi.h>
#include <a2_types.h>
#include <dsm.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*===========================================================================

                             MACROS

===========================================================================*/
#define A2_UL_PHY_CIPHER_INIT_NO_PARAM 0xFFFF

#define A2_UL_PHY_MAX_SRC_IOVEC        A2_MAX_SRC_IOVEC_COUNT_PER_DMA_CHAIN

/*! @brief Maximun number of bytes that can be dmaed out of ERAM in one DMA 
  operation on nikel */
#define A2_UL_PHY_NIKEL_DMA_FROM_ERAM_LIMIT     15

#define A2_UL_PHY_EXTRACT_BITS_11_to_17_USTMR(val)    \
                                              ( (val >> 0xB) & 0x7F )

#define A2_UL_PHY_DEBUG_INFO_MAX_SIZE    256

#define A2_UL_PHY_TAG_TIME_BMSK      0x3FFFFF

#define A2_UL_PHY_TAG_TIME_MAX_VAL   (A2_UL_PHY_TAG_TIME_BMSK + 1)

#define A2_UL_PHY_NOP                0x50000001  
#define A2_UL_PHY_CRC_INIT           0x40000012 

#ifdef FEATURE_A2_ON_TARGET
#define A2_UL_PHY_STMR_MAX_VAL       (A2_UL_PHY_OTA_TIME_BMSK + 1)
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef enum
{
  /*! Tag used for notifying that all tasks in the queue have been
    processed
    */
  A2_UL_PHY_TAG_ID_FINISHED = 0x0,
  A2_UL_PHY_TAG_HW_TAG_OTA,
  A2_UL_PHY_TAG_HW_PROCESS_START_TIME,
  A2_UL_PHY_TAG_HW_PROCESS_DONE_TIME,
  A2_UL_PHY_TAG_ID_MAX = 0x3
} a2_ul_phy_tag_id_e;

/*! @brief Structure storing the tasks to be filled
*/
typedef struct
{
  hwtask_a2_ts_dma_t                dma_to_specified_addr;
  hwtask_a2_ts_dma_t                dma_for_dsm;
  hwtask_a2_ts_dma_t                first_dma;
  hwtask_a2_ts_dma_t                dma_from_eram;
  hwtask_a2_ts_dma_header_t         dma_header;
  hwtask_a2_ts_dma_fill_t           dma_fill;
  hwtask_a2_ts_dma_fill_t           first_dma_fill;
  hwtask_a2_ts_dma_fill_t           dma_fill_ext;
  hwtask_a2_ts_dma_fill_t           dma_fill_harq_done;
  hwtask_a2_ts_crc_init_t           crc_init;
  hwtask_a2_ts_crc_report_t         crc_report;
  hwtask_a2_ts_umts_ciph_setup_t    ciph_setup;
  hwtask_a2_ts_umts_ciph_init_t     ciph_init;
  hwtask_a2_ul_phy_ts_umts_flag_t   umts_flag;
  hwtask_a2_ts_dma_tag_t            tag;
  hwtask_a2_ts_dma_tag_t            tag_time;
  hwtask_a2_ts_dma_irq_t            dma_irq;
} a2_ul_phy_task_cache_s;

/*! @brief Structure storing taskq programming info for harq retransmissions
*/

typedef struct
{
  uint32   ota_sched_time;
  uint32   delta_between_ota;
  uint32   hw_start_time;
  uint32   hw_done_time;
  uint32   hw_delta_from_sched;
  uint32   harq_id;
} a2_ul_phy_time_tag_s;

/*! @brief Structure storing crc corruption stats
*/
typedef struct
{
  uint32          num_enabled;
  uint32          num_disabled;
  a2_timetick_t   last_enabled_time;
  a2_timetick_t   last_disabled_time;
} a2_ul_phy_crc_corrupt_stats_s;

typedef struct
{
  /* addr of the memory inside UL PHY taskq from where the taskq programming for
   * this harq_id starts */
  uint32*         harq_taskq_start_addr;

  /* addr of the memory inside UL PHY taskq from where the taskq programming for
   * this harq_id ends */
  uint32*         harq_taskq_end_addr;

  /* offset to CRC address */
  uint32          crc_offset;

  /* flag to indicate if CRC needs to be corrupted or not for this harq id */
  boolean         corrupt_crc;

  /* dsm pointer pointing to the back up memory where taskq programming for this
   * harq is saved in case of discontinuous transmission (DTX) */
  dsm_item_type*  task_mem_dsm_ptr;

  /* The Universal stmr timer at which the current harq has to be transmitted */
  uint32          ota_sched_time;

  /* The Universal stmr timer at which SW commits the task to HW  */
  uint32          sw_commit_time;

  /* 11- 17 bits of ota_sched_time including the offset needed for A2 process */
  uint32          a2_sched_time;
  /* SW wrkaround min and max delta */
  uint32          swrk_min_delta;
  uint32          swrk_max_delta;

  /* End stats from Status Q*/
}a2_ul_phy_harq_taskq_info_s;

/*! @brief A2 UL PHY statistics structure
*/
typedef struct
{
  /* number of new harq transmissions since power up */
  uint32 num_new_harq_tx;
  /* number of new harq re-transmissions since power up */
  uint32 num_harq_retx;
  /* number of new harq discontinuous transmissions (DTX) since power up */
  uint32 num_harq_dtx;
    /*number of error case where A2 picks up HARQ retx request after conn release*/
  uint32 num_harq_retx_ignore;
  /*number of addr is NULL during harq re-transmissions since power up*/
  uint32 num_harq_retx_addr_null;
  /*number of addr is NULL during harq discontinuous trans since power up*/
  uint32 num_harq_dtx_addr_null;
  uint32 num_dma_irq_rcvd;
  /* no of time context switch happens after kernal protection*/
  uint32 num_cntx_switch_at_p0;
  /* sw delta scheduled slot histogram */
  uint32 delta_in_sched_histo[A2_UL_PHY_MAX_U_STMR_VALUE];
  /* no of times A2 HW notified the actual task process start time*/
  uint32 num_process_done_tag_time;
  /* counter to keep track UL PHY HW late processing */
  uint32 hw_late_processing_cnt;
  /* counter to keep track any abnormal in SW UL PHY time line */
  uint32 sw_abnormal_processing_cnt;
  /* debug UL PHY time tag */
  uint32 time_index;
  uint32 last_ota;
  a2_ul_phy_time_tag_s  time_tag[A2_UL_PHY_DEBUG_INFO_MAX_SIZE];

#ifdef FEATURE_A2_SW_WORKAROUND_UL_PHY_UNEXPECTED_STATUS
  uint32                      bogus_frag_dst_cnt;
  uint32                      bogus_frag_addr_cnt;
  uint32                      bogus_dst_dsm_free_cnt;
  uint32                      bogus_addr_dsm_free_cnt;
#endif
  /* for capturing crc corruption stats per harq id*/
  a2_ul_phy_crc_corrupt_stats_s crc_corrupt_stats[A2_UL_PHY_MAX_LTE_UL_HARQS];
}a2_ul_phy_stats_s;

/*! @brief Structure storing information on the ul phy
*/
typedef struct
{
  /* @todo May not be needed for lte */
  a2_cipher_init_param_u      last_cipher_params;

  /*! The callback to be called when the notification is triggered. */
  a2_ul_phy_notify_cb_t       notify_cb;

  /* Cached tasks where tasks are being prepared before copying to the
     uncached task queue */
  a2_ul_phy_task_cache_s      task;

  a2_taskq_status_s           status_taskq;

  /*! technology for which a2 ul phy has been configured to */
  a2_technology_e             curr_technology;

  uint32                      num_rlc_iovecs;
  a2_iovec_t                  rlc_iovec_list[A2_UL_PHY_MAX_SRC_IOVEC];

  /*! harq taskq programming related info */
  a2_ul_phy_harq_taskq_info_s harq_info[A2_UL_PHY_MAX_LTE_UL_HARQS];

  /*! harq id that was last initialized */
  uint8                       last_harq_id;

  a2_taskq_s                  taskq;

  /*! a2 ul phy statistics */
  a2_ul_phy_stats_s           stats;

  /*! a2 power collapse related debug info */
  a2_pc_dbg_s                 pc_dbg;
#ifdef FEATURE_A2_SW_WORKAROUND_UL_PHY_UNEXPECTED_STATUS
  dsm_item_type               *bogus_dsm_ptr;
#endif
} a2_ul_phy_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_ul_phy_init

==============================================================================*/
/*!
  @brief
  Initialize the SW structures for ul phy block

*/
/*============================================================================*/
void a2_ul_phy_init
(
  void
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_get_top_lvl_ptr

==============================================================================*/
/*!
  @brief
  Returns a pointer to the top-level data structure.
*/
/*============================================================================*/
a2_ul_phy_s * a2_ul_phy_get_top_lvl_ptr
(
  void
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_ist_cb

==============================================================================*/
/*!
  @brief
  Interrupt handler for A2 UL PHY module.

*/
/*============================================================================*/
int a2_ul_phy_ist_cb ( void* arg );


/*==============================================================================

  FUNCTION:  a2_ul_phy_set_technology

==============================================================================*/
/*!
  @brief
  Set the technology that is going to use A2 UL PHY HW and perform necessary
  HW initialization.

  @caller
  HSPA: whenever handover to HSPA is initiated and/or whenever WCDMA cell search
        starts
  LTE: whenever UE starts LTE cell search

  @todo - need to see from when and where to call this function. Critical from
   HSPA<->LTE hand-off. Need to make sure that task bring up doesn't happen in
   parallel. If yes then there is a need to sync up between LTE and HSPA.
   Probable mutex to guarantee mutual exclusion.
*/
/*============================================================================*/
void a2_ul_phy_set_technology
(
  a2_technology_e technology
);

/*==============================================================================

  FUNCTION:  a2_ul_phy_hw_init

==============================================================================*/
/*!
    @brief
    initializes the HW related registers for UL PHY block

    @return
    None
*/
/*============================================================================*/
void a2_ul_phy_hw_init
(
  void
);
/* a2_ul_phy_hw_init() */

/*==============================================================================

  FUNCTION:  a2_ul_phy_hspa_init

==============================================================================*/
/*!
    @brief
    Initializes variables associated with hspa init block

    @caller
    A2 Init (during A2 driver initialization)

    @return
    None
*/
/*============================================================================*/
void a2_ul_phy_hspa_init ( void );

/*==============================================================================

  FUNCTION:  a2_ul_phy_get_taskq

==============================================================================*/
/*!
    @brief
    This function returns the UL PHY task queue pointer to the caller.

    @caller
    a2_ul_phy_hspa.c

    @return
    pointer to a2_taskq_s
*/
/*============================================================================*/

a2_taskq_s *a2_ul_phy_get_taskq(void);


/*==============================================================================

  FUNCTION:  a2_ul_phy_get_taskq_status

==============================================================================*/
/*!
    @brief
    This function returns the UL PHY status task queue pointer to the caller.

    @caller
    a2_ul_phy_hspa.c

    @return
    pointer to a2_taskq_status_s
*/
/*============================================================================*/
a2_taskq_status_s *a2_ul_phy_get_taskq_status(void);
#endif /* A2_UL_PHYI_H */
