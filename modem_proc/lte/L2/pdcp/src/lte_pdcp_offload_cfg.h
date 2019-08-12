/*!
  @file
  lte_pdcp_offload_cfg.h

  @brief
  Compile time configuration for ROHC.

  @author
  sbathwal

  @detail
  PDCP can be configured at compile time to turn on certain features. A
  feature is turned ON by uncommenting a #define in this file. Compile 
  time flags defined in this file shall meet these requirements:
  -# All flags shall start with FEATURE_LTE_PDCP_.
  -# A Flag shall not cause another flag to be defined or undefined. 
  -# Flags used only in test code shall not be defined in this file.


*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_offload_cfg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
09/23/13   sb      CR393987: Remove duplicated macros
04/08/13   md      Reduced queue depth
===========================================================================*/

#ifndef LTE_PDCPOFFLOAD_CFG_H
#define LTE_PDCPOFFLOAD_CFG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <customer.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief maximum pending message router messages allowed
*/
#define LTE_PDCPOFFLOAD_CFG_TASK_MAX_MSG                                         50


/*! @brief maximum number of pkt enqueue to A2 without notifying A2
*/
#define LTE_PDCPOFFLOAD_MAX_ENQ_WO_NOTIFY                             15

/*---------------------------------------------------------------------------
  Following ROHC parameter values were recommended by system team.
---------------------------------------------------------------------------*/

#define LTE_PDCP_OFFLOAD_DEF_MAX_CID 15                 /* default value for max_cid as in
                                                       RRC spec 25.331 */

#define LTE_PDCP_OFFLOAD_DL_CIPH_BUF_EXP                            8

#endif /* LTE_PDCPOFFLOAD_CFG_H */
