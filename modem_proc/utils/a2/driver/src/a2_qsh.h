#ifndef _A2_QSH_H_
#define _A2_QSH_H_
/*!
  @file a2_qsh.h

  @brief
  Interface for implementation of QSH client for A2.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/a2/driver/src/a2_qsh.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/26/15   sn      CR860622: QSH phase-II changes
12/05/14   sn      CR767131: QSH initial version

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <a2_util.h>
#include <a2_common.h>
#include <qsh.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/* DMA time to us: (t/20480) * 1000 */
#define A2_DMA_TIME_TO_MICROSEC(t)  (((t) * 100) >> 11)

/*! QSH major version */
#define A2_QSH_VER_MAJOR 1

/*! QSH minor version */
#define A2_QSH_VER_MINOR 1

/*! QSH modules */
#define A2_QSH_MOD_UL_PHY    0x0
#define A2_QSH_MOD_DL_PHY    0x1
#define A2_QSH_MOD_UL_SEC    0x2
#define A2_QSH_MOD_DL_PER    0x3
#define A2_QSH_MOD_UL_PER    0x4
#define A2_QSH_MOD_A2_HW     0x5
#define A2_QSH_MOD_FRAG      0x6

#define A2_DL_PER_NUM_TASKQ_FULL_THRESH       (100) 

#define A2_MAX_DMA_TIME_USEC   (1000)

#define A2_MAX_TASK_TIME_USEC   (1000)

#define A2_MAX_WAKEUP_TIME_USEC (1000)

#define A2_MAX_SHUTDOWN_TIME_USEC (1000)

#define A2_QSH_DL_PER_PDCP_CURR_PKT_THRESH     (10)

/*! Checking for enable status of UL_PERn */
#define A2_PERn_IS_ENABLED(n)  ((n & (HWIO_A2_PERn_UL_CFG_PER_EN_BMSK)) >> \
                                              (HWIO_A2_PERn_UL_CFG_PER_EN_SHFT))

/*! Checking for flowcontrolled status of UL_PERn */
#define A2_PERn_IS_FLOWCONTROLLED(n) ((n & (HWIO_A2_PERn_UL_CFG_PER_FLOW_CTL_BMSK)) >> \
                                                   (HWIO_A2_PERn_UL_CFG_PER_FLOW_CTL_SHFT))

typedef struct
{
  /*! Counter for reset trigger count */
  uint32 reset_count;

  /*! Counter for analysis trigger count */
  uint32 analysis_count;
} a2_qsh_stats;

typedef struct
{
  /*! Boolean to confirm the cause for QSH trigger */
  boolean       err_fatal;

  a2_qsh_stats  sync_mode;

  a2_qsh_stats  async_mode;

  /*! Time when QSH was triggered (can be used directly from a2_dbg_data) */
  a2_timetick_t qsh_invoke_time;  
} a2_qsh_s;

/*==============================================================================

                          A2 QSH LOG MACROS

==============================================================================*/

/* Need to enable this to get forced QSH report for A2 
#define A2_QSH_FORCE_REPORT_WO_DUMMY_VALUES
*/

#ifdef A2_QSH_FORCE_REPORT_WO_DUMMY_VALUES
#define EXPR(x) (TRUE)
#else
#define EXPR(x) (x)
#endif

#define A2_QSH_CHECK_LOG_HIGH_MSG(expr, category, summary, ...)  \
                                             QSH_CHECK_LOG(EXPR(expr),\
                                                           QSH_CLT_A2,\
                                                           category,\
                                                           QSH_MSG_TYPE_HIGH,\
                                                           summary,\
                                                           ##__VA_ARGS__)

#define A2_QSH_CHECK_LOG_MED_MSG(expr, category, summary, ...)  \
                                             QSH_CHECK_LOG(EXPR(expr),\
                                                           QSH_CLT_A2,\
                                                           category,\
                                                           QSH_MSG_TYPE_MEDIUM,\
                                                           summary,\
                                                           ##__VA_ARGS__)
                                                           
#define A2_QSH_CHECK_LOG_INFO_MSG(expr, category, summary, ...)  \
                                             QSH_CHECK_LOG(EXPR(expr),\
                                                           QSH_CLT_A2,\
                                                           category,\
                                                           QSH_MSG_TYPE_INFO,\
                                                           summary,\
                                                           ##__VA_ARGS__)

#define A2_QSH_LOG_INFO_STR(category, summary, ...)  \
                                             QSH_LOG(QSH_CLT_A2,\
                                                     category,\
                                                     QSH_MSG_TYPE_INFO,\
                                                     summary,\
                                                     ##__VA_ARGS__)
  

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_qsh_analysis_cb

==============================================================================*/
/*!
  @brief
  Callback registered with QSH.
*/
/*============================================================================*/
boolean a2_qsh_analysis_cb
(
  qsh_cb_params_s *   qsh_params_ptr
);

/*==============================================================================

  FUNCTION:  a2_qsh_run_action

==============================================================================*/
/*!
  @brief
  Performs top-level QSH action.
*/
/*============================================================================*/
void a2_qsh_run_action
(
  qsh_cb_params_s *   qsh_params_ptr
);

#endif /* _A2_QSH_H_ */
