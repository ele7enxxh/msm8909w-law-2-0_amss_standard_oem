/*!
  @file
  lte_rlc_cfg.h

  @brief
  Compile time configuration for RLC.

  @detail
  RLC can be configured at compile time to turn on certain features.  
  Compile time flags defined in this file shall meet these requirements:
  -# All flags shall start with FEATURE_LTE_RLC_.
  -# A Flag shall not cause another flag to be defined or undefined. 
  -# Flags used only in test code shall not be defined in this file.

  @author
  axiao

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlc_cfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/31/16   cx      CR923654: fast nak optmization
04/15/15   ru      CR816655: RLC mitigation to prevent RLF due to FC Shutdown
01/10/15   pw      CR772564: RLC fast NAK for DTA
10/02/15   mg      CR904992: For L+G case use B factor to protect video TOI 
                   against BLER by trading off G page block rate.
09/15/15   mg      CR904992: EMBMS DPD tradeoff G and 1x paging
04/30/15   mg      CR832561: Adaptive b factor scaling
04/23/15   mg      CR824175:  Video threshold size determination as function of 
                   FEC and segment duration for DPD calculations
02/05/15   mg      CR788317: Invalid MSI and 0xB08C logging changes
01/27/15   mg      CR786757: EMBMS DPD not kicking in on service startup
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
01/14/15   mg      CR 779901: Embms DSDS DPD feature - Phase 2
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1 
10/03/14   mg      CR728123: RLC fast NAK for OPP CA SCELL tuneaway
09/19/14   ru      CR726194: Avoid UE crash when RLCDL header is greater than 512 bytes
05/27/14   mm      CR 670482: Changes for new QSH API
04/11/14   mg      CR647594: QSH Phase 1
04/03/14   mg      CR643306: DSDS RLC Fast NAK after receiving RF_AVAILABLE_IND 
01/23/14   mg      CR599494: RLC behavior to alleviate the low DSM count problem.
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features 
04/08/13   md      Reduced queue depth
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
12/16/11   ax      feature clean up 
10/25/11   ax      make RLC-A2 eMBMS watermark levels configruable
07/21/11   ax      fixed CR#297113: detect and release huge SDU 
04/05/10   ax      add #define for task mailbox size
03/26/10   ax      remvoed #define FEATURE_LTE_RLC_PROFILE
09/17/09   ax      moved rlcdl logging related timer value here
09/04/09   ax      moved UL logging related timer values from lte_rlcul_log.c
08/25/09   ax      merge lte_rlc_cfg.h files into a single file
06/08/09   ax      include lte_l2_profile.h so that it does not depend on tests
06/02/09   ax      center of RLC featurization
==============================================================================*/

#ifndef LTE_RLC_CFG_H
#define LTE_RLC_CFG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "lte_l2_profile.h"
#include "lte_as.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                   FEATURIZATION

==============================================================================*/

/*==============================================================================

                   RLCDL thresholds

==============================================================================*/

/*! @brief largest PDCP SDU size is 8188 per spec, assuming PDCP control PDU 
     will not be anywhere near this limit
*/
#define LTE_RLC_CFG_MAX_RLC_SDU_SZ                                    8200

/*==============================================================================

                   RLCDL memory related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief maximum pending message router messages allowed
*/
#define LTE_RLC_CFG_DL_TASK_MAX_MSG                                          50

/*! @brief number of RLCDL NAK elements dynamically allocated at one time
*/
#define LTE_RLC_CFG_DL_NUM_NAK_ELEMENTS                                      512 

/*! @brief pdu log buffer size for received PDUs
*/
#define LTE_RLC_CFG_DL_LOG_PDU_BUF_SZ                                       2048

/*! @brief pdu log buffer size for EMBMS DPI SDUs sent to upper layers
*/
#define LTE_RLC_CFG_DL_LOG_EMBMS_DPI_SDU_BUF_SZ                             2048

/*! @brief RLCDL->PDCPDL circular queue size (number of elements)
     10ms*1 entry per TTI per bearer*total number of active bearers
*/
#define LTE_RLC_CFG_DL_SDU_CIRC_Q_SIZE                    (LTE_MAX_ACTIVE_RB*10)

/*! @brief pdu log buffer size for transmitted PDUs
*/
#define LTE_RLC_CFG_UL_LOG_PDU_BUF_SIZE                                     2048

/*! @brief RLCDL task scratch pad size: 
    The scratch pad is used for the following, it is critical that the size is 
    big enough for the maximum required size for all the intended usage.
    1. extract the extended header (LI's)
*/
#define LTE_RLC_CFG_DL_SCRATCH_PAD_EXP                                      8

/*! @brief RLC-A2 eMBMS watermark low
*/
#define LTE_RLC_CFG_EMBMS_WM_LO                                     60000

/*! @brief RLC-A2 eMBMS watermark high
*/
#define LTE_RLC_CFG_EMBMS_WM_HI                                     75000

/*! @brief RLC-A2 eMBMS watermark dne
*/
#define LTE_RLC_CFG_EMBMS_WM_DNE                                    90000

/*==============================================================================

                   RLCUL memory related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief maximum pending message router messages allowed
*/
#define LTE_RLC_CFG_UL_TASK_MAX_MSG                                          100

/*! @brief Macro for maximum number of NACKs per status pdu. This number is a
 * restriction from the UE side. If there are any more NACKs present in the
 * status pdu, than this number, UE will ignore them.
*/
#define LTE_RLC_CFG_UL_MAX_NACKS_PER_STATUS_PDU                              512

/*! @brief Macro for maximum supported size (in bytes) of the RLCDL status pdu
*/
#define LTE_RLC_CFG_UL_AM_STATUS_PDU_MAX_SIZE                               2000

/*==============================================================================

                   RLCUL timing related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Log flush timer periodicity in ms
*/
#define LTE_RLC_CFG_UL_FLUSH_TIMER_PERIODICITY                                45

/*! @brief Log statistics timer periodicity in ms: 1.44 second
*/
#define LTE_RLC_CFG_UL_STATS_TIMER_PERIODICITY                 \
(LTE_RLC_CFG_UL_FLUSH_TIMER_PERIODICITY << 5)

/*! @brief Log status update timer periodicity in ms
*/
#define LTE_RLC_CFG_UL_LOG_STATUS_TIMER_PERIODICITY                          100

/*==============================================================================

                   RLCDL timing related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief flush every 40(8*5) ms (periodicity of timer )
*/
#define LTE_RLC_CFG_DL_PDU_LOG_FLUSH(counter)     (((counter) & 0x7) == 0)

/*! @brief update log status every 160(32*5) ms (periodicity of timer )
*/
#define LTE_RLC_CFG_DL_LOG_UPDATE(counter)    (((counter) & 0x1f) == 0)

/*! @brief update log status every 1280(256*5) ms (periodicity of timer )
*/
#define LTE_RLC_CFG_DL_LOG_STATISTICS(counter)    (((counter) & 0xff) == 0)

/*==============================================================================

                   RLCDL flow control related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief mask when rlc low dsm count behavior is not active
*/
#define LTE_RLC_CFG_DL_FC_DISABLE_MASK                                       0

/*! @brief mask when rlc low dsm count behavior is active
*/
#define LTE_RLC_CFG_DL_FC_LOW_MEMORY_MASK                                    0x00000001

/*! @brief mask when rlc receives RF tune back indication
*/
#define LTE_RLC_CFG_DL_DSDS_TUNEAWAY_MASK                                    0x00000001 << 1

/*! @brief mask when rlc receives SCELL unavailable indication
*/
#define LTE_RLC_CFG_DL_SCELL_TUNEAWAY_MASK                                   0x00000001 << 2

/*! @brief mask to indicate FAST NACK due to huge DL BLER
*/
#define LTE_RLC_CFG_DL_RX_WIN_FC_MASK                                        0x00000001 << 3

/*! @brief AM default reordering timer when rlc low dsm count behavior is active
*/
#define LTE_RLC_CFG_DL_FC_DEFAULT_AM_T_REORDERING                            20

/*! @brief AM default status prohibit timer when rlc low dsm count behavior is active
*/
#define LTE_RLC_CFG_DL_FC_DEFAULT_AM_T_STATUS_PROHIBIT                       20

/*! @brief supported AM minimal reordering timer 
*/
#define LTE_RLC_CFG_DL_MIN_AM_T_REORDERING                                   10

/*! @brief supported AM minimal status prohibit timer 
*/
#define LTE_RLC_CFG_DL_MIN_AM_T_STATUS_PROHIBIT                              10

/*! @brief the time that need to wait before sending another status PDU if the 
    current status PDU is suppressed. The value has to be less or equal to min
    supported T_STATUS_PROHIBIT, otherwise there will be overflow 
*/
#define LTE_RLC_CFG_DL_STATUS_REPORT_T_SUPPRESS                              10

/*! @brief UM default reordering timer when rlc low dsm count behavior is active
*/
#define LTE_RLC_CFG_DL_FC_DEFAULT_UM_T_REORDERING                            20

/*! @brief Threshold to start aggressive configuration. This is the difference between 
    VR_H and VR_R
*/
#define LTE_RLC_CFG_FC_RX_WIN_HIGH_THRESHOLD                                120

/*! @brief Threshold to stop aggressive configuration. This is the difference between 
    VR_H and VR_R
*/
#define LTE_RLC_CFG_FC_RX_WIN_LOW_THRESHOLD                                 60

/*! @brief Default EMBMS dynamic priority mask
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_DYN_PRI                                 0x01

/*! @brief Default EMBMS disable send burst priroity mask
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_DISABLE_SEND_BURST_PRI                  0x00

/*! @brief Default duration in ms of hi priority after a eMBMS service starts
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_HI_PRI_TIMER                           10000

/*! @brief Default Scaling factor A of eMBMS service
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_SCA_FACT_A                              103

/*! @brief Default Scaling factor B of eMBMS service
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_SCA_FACT_B_MAX                          10000

/*! @brief Default Adaptive B factor
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_SCA_FACT_B_MIN                          103


/*! @brief Default Scaling factor B of eMBMS service
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_SCA_FACT_DELTA                          0x00000001


/*! @brief Default EMBMS firs burst high priroity mask
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_FIRST_BURST_HI_PRI                     0x01

/*! @brief Default EMBMS MIN MSP time in ms to use LTE_TRM_BCAST priority instead of
                background priority for low 
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_MIN_MSP_BCAST                           600

/*! @brief Default avg minimum size per ms of a toi for it to be considered as
    video toi. video_seg_threshold_size = 12*seg_duration_in_ms
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_VID_SEG_THRES_SIZE_PER_MS              12

/*! @brief Default value of the flag to use background priority for eMBMS low
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_USE_BACKGROUND_LOW_PRI                  0x00

/*! @brief Default value of the flag to use background priority for eMBMS low
*/
#define LTE_RLC_CFG_DL_DEFAULT_EMBMS_USE_ADAPTIVE_B_FACT                     0x01


/*==============================================================================

                   RLC QSH related EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define LTE_RLC_CFG_QSH_MAJOR_VER                                            1
#define LTE_RLC_CFG_QSH_MINOR_VER                                            1
#define LTE_RLC_CFG_DL_QSH_HIGH_T_REORDERING                                 50
#define LTE_RLC_CFG_DL_QSH_HIGH_T_STATUS_PROHIBIT                            80
#define LTE_RLC_CFG_DL_QSH_HIGH_SEQ_DIFF                                     100


#define LTE_RLC_CFG_UL_QSH_HIGH_T_POLL                                       80
#define LTE_RLC_CFG_UL_QSH_HIGH_SEQ_DIFF                                     100

/*! @brief Time in milliseconds for which the DSDS fast nak behavior is active
*/
#define LTE_RLC_CFG_DL_DSDS_FAST_NAK_TIMER                                   45
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* LTE_RLC_CFG_H */
