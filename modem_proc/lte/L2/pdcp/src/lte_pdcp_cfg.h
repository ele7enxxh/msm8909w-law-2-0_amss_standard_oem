/*!
  @file
  lte_pdcp_cfg.h

  @brief
  Compile time configuration for PDCP.

  @author
  bqiu, axiao

  @detail
  PDCP can be configured at compile time to turn on certain features. A
  feature is turned ON by uncommenting a #define in this file. Compile 
  time flags defined in this file shall meet these requirements:
  -# All flags shall start with FEATURE_LTE_PDCP_.
  -# A Flag shall not cause another flag to be defined or undefined. 
  -# Flags used only in test code shall not be defined in this file.


*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_cfg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/15   ru      CR896720: PDCP Discard Timer Overwrite
09/25/14   sb      CR728056: Enable periodic refresh feature and reconfigure number of IRs
06/10/14   sb      CR672057: Revert timings of PDCP re-establishment to original and 
                   increase Num of IRs to huge number
05/13/14   sb      CR662435: Revert CR651692 and change number of IR packets
                   transmitted back to original value
04/22/14   sb      CR651692: Increase the number of IR packets transmitted in 
                   U-mode IR state before transitioning to FO state.
09/23/13   sb      CR393987: Change macros feeded to compressor engine
04/08/13   md      Reduced queue depth
12/16/11   ax      feature clean up LTE_PDCP_CFG_DYNAMIC_MEM_ALLOC
10/11/11   ax      reduce pdcp-rlc wm low and hi size for bidirectional tput 
09/06/11   ax      tweak RoHC related parameters
06/15/11   ax      RoHC support
05/20/10   ax      add #define LTE_PDCP_CFG_DYNAMIC_MEM_ALLOC
04/05/10   ax      add #define for maximum mailbox size
07/07/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_CFG_H
#define LTE_PDCP_CFG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <customer.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief maximum pending message router messages allowed
*/
#define LTE_PDCP_CFG_DL_TASK_MAX_MSG                                         50

/*! @brief maximum pending message router messages allowed
*/
#define LTE_PDCP_CFG_UL_TASK_MAX_MSG                                         50

/*! @brief maximum number of pkt enqueue to A2 without notifying A2
*/
#define LTE_PDCPDL_MAX_ENQ_WO_NOTIFY                             15

/*! @brief default assert mask
*/
#define LTE_PDCP_DBG_DEFAULT_ASSERT_MASK                         0

/*! @brief default discard timer to overwrite in ms unit
*/
#define LTE_PDCP_CFG_DEFAULT_OVERWRITE_DISCARD_TIMER             1500
/*---------------------------------------------------------------------------
  Following ROHC parameter values were recommended by system team.
---------------------------------------------------------------------------*/

#define LTE_PDCP_DEF_MAX_CID 15                 /* default value for max_cid as in
                                               RRC spec 25.331 */
#define LTE_PDCP_REV_DECOMP_DEPTH 0             /* default is 0, reverse decomp
                                               is not used */
#define LTE_PDCP_MRRU 0                         /* segmentation is turned off */
#define LTE_PDCP_MAX_JITTER_CD 60

/* compressor flow default parameters*/
#define LTE_PDCP_COMP_FLOW_WIN_WIDTH 6
#define LTE_PDCP_COMP_MAX_NUM_ALLOWED_PKT_SIZES 0
#define LTE_PDCP_COMP_FLOW_NUM_UPDATE_IR 15
#define LTE_PDCP_COMP_FLOW_NUM_NON_UPDATES 6
#define LTE_PDCP_COMP_FLOW_SO_IRT 256
#define LTE_PDCP_COMP_FLOW_SO_FOT 128
#define LTE_PDCP_COMP_FLOW_TIMER_COMP FALSE
#define LTE_PDCP_COMP_FLOW_PROF_HINT 0
#define LTE_PDCP_COMP_FLOW_F_MAX_TIME 500
#define LTE_PDCP_COMP_FLOW_EXP_BACKOFF 1
#define LTE_PDCP_COMP_FLOW_TIME_STRIDE 20
#define LTE_PDCP_COMP_FLOW_TS_RECALC TRUE
#define LTE_PDCP_COMP_FLOW_LOSS_OPTION FALSE
#define LTE_PDCP_COMP_FLOW_P_SN 5
#define LTE_PDCP_COMP_CONST_IP_ID_HANDLING TRUE

/* decompressor flow default parameters */
#define LTE_PDCP_DECOMP_FLOW_ACK_ENAB FALSE
#define LTE_PDCP_DECOMP_FLOW_RD_DEPTH 0
#define LTE_PDCP_DECOMP_FLOW_TIMER_BASED_DECOMP FALSE
#define LTE_PDCP_DECOMP_FLOW_LOSS_OPTION_THRES 0
#define LTE_PDCP_DECOMP_FLOW_P_SN 5

#endif /* LTE_PDCP_CFG_H */
