#ifndef LTE_A2_DL_PHYI_H
#define LTE_A2_DL_PHYI_H

/*!
  @file a2_dl_phyi.h

  @brief
   The internal interface to the downlink phy portion of the a2 driver for LTE.

  @ingroup a2_int_interface
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/a2/driver/src/a2_dl_phyi.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/08/14   sn      Initial Checkin


==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <a2_dl_phy.h>
#include <a2_common.h>
#include <a2_dbg.h>
#include <a2_taskq.h>
//#include <a2_poweri.h>
#include <a2_status_util.h>
#include <comdef.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#ifdef FEATURE_A2_SW_WORKAROUND_LTE_DL_PHY_CRC

/*! @brief crc size in bytes */
#define A2_DL_PHY_LTE_CRC_SIZE_IN_BYTES 3

#endif /* FEATURE_A2_SW_WORKAROUND_LTE_DL_PHY_CRC */

/* Max entries in DL PHY profiling table */
#define A2_DL_PHY_MAX_PRF_CNT 128

/*! @brief dl phy interrupts statistics */
typedef struct
{
  uint32 num_trblk_intr;        /*!< num transport block interrupts received */
  uint32 num_dma_irq_intr;      /*!< num DMA IRQ interrupts received */
  uint32 num_config_init_intr;  /*!< num DMA IRQ interrupts received after 
                                     switching the dl phy intr mode from timer 
                                     to immediate */
  uint32 num_intr_in_hw_off_state;  /*!< num of intr received when HW is off */
  uint32 num_intr_in_def_tech;  /*!< num of intr received when DL PHY is in 
                                     default state */
  a2_timetick_t last_intr_time; /*<! last time interrupt was generated */
} a2_dl_phy_intr_stats;

/*! @brief LTE dl phy statistics */
typedef struct
{
  uint32 num_get_data_calls;    /*!< num of times mac called
                                      a2_dl_phy_get_data() */
  uint32 num_get_data_calls_skipped; /*!< num of times get data is skipped
                                          because DL PHY tech is DEFAULT */
  uint32 num_trblk_rcvd;        /*!< num tranport blocks received */
  uint32 num_trblk_bytes_rcvd;  /*!< num tranport block bytes received */
  uint32 num_invalid_trblk_rcvd;       /*!< num invalid tbs received */
  uint32 num_invalid_trblk_bytes_rcvd; /*!< num invalid tb bytes received */
  uint32 trblk_error_cnt;
  uint32 num_intr_cfg_in_lte_tech;     /*!< num times LTE MAC called 
                                       a2_dl_phy_set_dec_interrupt_type() and A2
                                       was in LTE tech */
  uint32 num_intr_cfg_in_non_lte_tech; /*!< num times LTE MAC called 
                                       a2_dl_phy_set_dec_interrupt_type() but A2
                                       wasn't in LTE tech */
  uint32 num_get_data_calls_skipped_hw_off; /*!< num of times get data is 
                                       skipped because A2 HW is OFF */
#ifdef FEATURE_A2_SW_WORKAROUND_DL_PHY_INVALID_STATUS
  uint32 wrong_frag_dst_len_cnt;
  uint32 wrong_frag_dst_addr_cnt;
#endif
} a2_dl_phy_lte_stats;

/*! @brief dl phy profiling structure */
typedef struct
{
  uint32  tb_count;
  uint32  get_data_time;
  uint32  statusq_rd_ptr;
}a2_dl_phy_profiling_s;

/*! @brief dl phy statistics structure */
typedef struct
{
  a2_dl_phy_intr_stats intr;  /*!< interrupts related stats */
  a2_dl_phy_lte_stats  lte;   /*!< LTE related stats */
  a2_dl_phy_profiling_s  profiling[A2_DL_PHY_MAX_PRF_CNT];
  uint8                  profiling_index;
} a2_dl_phy_stats;


/*!
   @brief
    Structure to hold cached info of all the tasks written by this module
*/
typedef struct
{
  hwtask_a2_ts_mp_vbuf_cfg_t     vbuf_cfg_for_dob;
  hwtask_a2_ts_dma_irq_t         config_init_irq;
} a2_dl_phy_cached_tasks_s;


/*! @brief dl phy top level structure */
typedef struct
{
  a2_taskq_s                      taskq;
  a2_taskq_status_s               taskq_status;

  a2_dl_phy_tb_available_cb       tb_recd_cb;

  a2_status_util_dsm_chain_info_s partial_dsm_chain_info;
  uint32                          tb_len_in_bytes;
  void                            *tb_addr;
  uint32                          irq_en;

  /*! technology for which a2 dl phy has been configured to */
  a2_technology_e                 curr_technology;

  /*! statiscits for dl phy module */
  a2_dl_phy_stats                 stats;

  /* debug flag to control assert on error interrupts */
  boolean dbg_assert_err_intr;

  /*! flag to indicate the state of DL PHY data interrupt type*/
  boolean  imm_interrupt;

  /*! counter to keep track if we need to flush the status due
  to miss an interrupt */
  uint32  flush_status_q_due_to_missed_intr_cnt;

  /*! last statusq frag ptr processed by the DL PHY */
  dsm_item_type* last_frag_proc_by_dl_phy;

  /*! a2 power collapse related debug info */
  a2_pc_dbg_s    pc_dbg;

  a2_dl_phy_cached_tasks_s   cached_tasks;
/*! Critical section to protect decoder interrupt type (timer vs immediate) 
  config */
  a2_crit_sect_s dec_intr_cfg_crit_sect;

} a2_dl_phy_s;

/*! @brief define enumuration for DL PHY VBUF selection. Maps to the vbuf_sel 
   field in hwtask_a2_ts_a2_mp_vbuf_cfg_t. Can take atmost 8 values */
typedef enum
{
  /*! VBUF Undefined */
  A2_DL_PHY_VBUF_CFG_SEL_UNDEFINED,
  /*! LTE VBUF */
  A2_DL_PHY_VBUF_CFG_SEL_LTE,
  /*! W VBUF */
  A2_DL_PHY_VBUF_CFG_SEL_WCDMA,
  /*! DO VBUF */
  A2_DL_PHY_VBUF_CFG_SEL_DO,
  /*! TD VBUF */
  A2_DL_PHY_VBUF_CFG_SEL_TDSCDMA
} a2_dl_phy_vbuf_cfg_sel_e;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_dl_phy_get_top_lvl_ptr

==============================================================================*/
/*!
  @brief
  Returns a pointer to the top-level data structure.
*/
/*============================================================================*/
const a2_dl_phy_s * a2_dl_phy_get_top_lvl_ptr
(
  void
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_get_taskq

==============================================================================*/
/*!
    @brief
    This function returns the DL PHY task queue pointer to the caller.

    @caller
    a2_dl_phy_hspa.c

    @return
    pointer to a2_taskq_s
*/
/*============================================================================*/
a2_taskq_s *a2_dl_phy_get_taskq
(
  void
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_get_taskq_status

==============================================================================*/
/*!
    @brief
    This function returns the DL PHY status task queue pointer to the caller.

    @caller
    a2_dl_phy_hspa.c

    @return
    pointer to a2_taskq_status_s
*/
/*============================================================================*/
a2_taskq_status_s *a2_dl_phy_get_taskq_status
(
  void
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_set_technology

==============================================================================*/
/*!
  @brief
  Set the technology that is going to use A2 DL PHY HW and perform necessary
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
void a2_dl_phy_set_technology
(
  a2_technology_e technology
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_hw_init

==============================================================================*/
/*!
    @brief
    initializes the HW related registers for DL PHY block

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_hw_init
(
  a2_technology_e tech /* tech to boot a2 up in */
);

/*==============================================================================

  FUNCTION:  a2_dl_phy_init

==============================================================================*/
/*!
    @brief
    Initializes the SW structures of DL PHY block

    @return
    None
*/
/*============================================================================*/
void a2_dl_phy_init ( void );

/*==============================================================================

  FUNCTION:  a2_dl_phy_deinit

==============================================================================*/
/*!
    @brief
    De-initializes the downlink peripheral block

    @return
    None
*/
/*============================================================================*/
extern void a2_dl_phy_deinit(void);

/*==============================================================================

  FUNCTION:  a2_dl_phy_ist_cb

==============================================================================*/
/*!
  @brief
  Interrupt handler for A2 DL PHY module.

*/
/*============================================================================*/
int a2_dl_phy_ist_cb ( void* arg );

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_init

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
void a2_dl_phy_hspa_init ( void );

/*==============================================================================

  FUNCTION:  a2_dl_phy_hspa_get_status

==============================================================================*/
/*!
    @brief
    Reads the status from the status queue. Calls appropriate call back
    functions based on the status/tag received.

    @note
    Takes care of necessary cache invaldiation before processing status queue.

    @caller
    "HSPA Header Read ISR" (primary) and/or "MAC DL HS"

    @return
    boolean: TRUE if any status is read from the status queue. FALSE if status
             is not yet updated.
*/
/*============================================================================*/
boolean a2_dl_phy_hspa_get_status
(
  void
);

#endif /* LTE_A2_DL_PHYI_H */
