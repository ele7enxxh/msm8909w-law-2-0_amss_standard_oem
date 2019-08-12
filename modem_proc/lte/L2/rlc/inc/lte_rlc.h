/*!
  @file
  lte_rlc.h

  @brief
  kitchen sink RLC header file that includes/contains all RLC external
  interfaces.

  @author
  axiao
*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/inc/lte_rlc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/14/15   mg      CR 779901: Embms DSDS DPD feature - Phase 2
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1
04/17/12   ru      CR 342859: Added eMBMS logging related log masks
05/07/09   ax      added lte_XXX_log_set_mask() APIs
04/17/09   ax      update RLCDL logging according to the ICD
04/07/09   ar      updated the macro names as per ICD document
02/11/09   ax      initial version
==============================================================================*/

#ifndef LTE_RLC_H
#define LTE_RLC_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "lte_rlc_msg.h"
#include "lte_rlc_mac.h"
#include "lte_rlc_pdcp.h"
#include "lte_rlc_rrc.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief reserved RLCDL log mask
*/
#define LTE_RLCDL_RESERVED_LOG_MASK                             0x0001

/*! @brief RLCDL config log mask
*/
#define LTE_RLCDL_CONFIG_LOG_MASK                               0x0002

/*! @brief RLCDL AM all pdu log mask
*/
#define LTE_RLCDL_AM_ALL_PDU_LOG_MASK                           0x0004

/*! @brief RLCDL AM control pdu log mask
*/
#define LTE_RLCDL_AM_CONTROL_PDU_LOG_MASK                       0x0008

/*! @brief RLCDL AM polling pdu log mask
*/
#define LTE_RLCDL_AM_POLLING_PDU_LOG_MASK                       0x0010

/*! @brief RLCDL AM signaling pdu log mask
*/
#define LTE_RLCDL_AM_SIGNALING_PDU_LOG_MASK                     0x0020

/*! @brief RLCDL UM data pdu log mask
*/
#define LTE_RLCDL_UM_DATA_PDU_LOG_MASK                          0x0040

/*! @brief RLCDL statistics log mask
*/
#define LTE_RLCDL_STATISTICS_LOG_MASK                           0x0080

/*! @brief RLCDL Unified log packet 
*/
#define LTE_RLCDL_UNIFIED_LOG_MASK                              0x0100

/*! @brief RLCDL eMBMS statistics log mask 
*/
#define LTE_RLCDL_EMBMS_STATS_LOG_MASK                          0x0200

/*! @brief RLCDL eMBMS config log mask 
*/
#define LTE_RLCDL_EMBMS_CONFIG_LOG_MASK                         0x0400

/*! @brief RLCDL eMBMS dsds service info log mask 
*/
#define LTE_RLCDL_EMBMS_DSDS_SERV_LOG_MASK                      0x0800

/*! @brief RLCDL eMBMS dpi sdu log mask 
*/
#define LTE_RLCDL_EMBMS_DPI_SDU_LOG_MASK                        0x1000

/*! @brief RLCDL invalid pdu log mask
*/
#define LTE_RLCDL_INVALID_PDU_LOG_MASK                          0xFFFF

/*! @brief reserved RLCUL log mask
*/
#define LTE_RLCUL_RESERVED_LOG_MASK                             0x0001

/*! @brief RLCUL config log mask
*/
#define LTE_RLCUL_CONFIG_LOG_MASK                               0x0002

/*! @brief RLCUL AM all pdu log mask
*/
#define LTE_RLCUL_AM_ALL_PDU_LOG_MASK                           0x0004

/*! @brief RLCUL AM control pdu log mask
*/
#define LTE_RLCUL_AM_CONTROL_PDU_LOG_MASK                       0x0008

/*! @brief RLCUL AM polling pdu log mask
*/
#define LTE_RLCUL_AM_POLLING_PDU_LOG_MASK                       0x0010

/*! @brief RLCUL AM signaling pdu log mask
*/
#define LTE_RLCUL_AM_SIGNALING_PDU_LOG_MASK                     0x0020

/*! @brief RLCUL UM data pdu log mask
*/
#define LTE_RLCUL_UM_DATA_PDU_LOG_MASK                          0x0040

/*! @brief RLCUL statistics log mask
*/
#define LTE_RLCUL_STATISTICS_LOG_MASK                           0x0080


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*===========================================================================

  FUNCTION:  lte_rlcdl_task_init

===========================================================================*/
/*!
  @brief
  This function creates RLCDL task with the given priority.

  @return
  the RLCDL thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t lte_rlcdl_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_rlcul_task_init

===========================================================================*/
/*!
  @brief
  This function creates RLCUL task with the given priority.

  @return
  the RLCUL thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t lte_rlcul_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_rlcdl_log_set_mask

===========================================================================*/
/*!
  @brief
  This function overwrites the rlcdl log mask with the provided new mask.

  @return
  None.
*/
/*=========================================================================*/
extern void lte_rlcdl_log_set_mask
(
  uint16   new_log_mask/*!< new log mask */
);

/*===========================================================================

  FUNCTION:  lte_rlcul_log_set_mask

===========================================================================*/
/*!
  @brief
  This function overwrites the rlcul log mask with the provided new mask.

  @return
  None.
*/
/*=========================================================================*/
extern void lte_rlcul_log_set_mask
(
  uint16   new_log_mask/*!< new log mask */
);

/*===========================================================================

  FUNCTION:  lte_rlcul_pdcpul_log_set_mask

===========================================================================*/
/*!
  @brief
  This function overwrites the pdcpul log mask with the provided new mask.

  @return
  None.
*/
/*=========================================================================*/
extern void lte_rlcul_pdcpul_log_set_mask
(
  uint32   new_log_mask/*!< new log mask */
);

#endif /* LTE_RLC_H */
