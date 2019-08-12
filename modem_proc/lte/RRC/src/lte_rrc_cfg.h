/*!
  @file
  lte_rrc_cfg.h

  @brief
  This file contains definitions for the features that LTE RRC may need.

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_cfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/17/10    mm     Commented out FEATURE_LTE_RRC_DISABLE_MIB_READING
04/16/10   amit    Removed FEATURE_LTE_RRC_STATIC_MEM
04/08/10   amit    Added FEATURE_LTE_RRC_USE_RRC_HEAP 
04/06/10   amit    Added FEATURE_LTE_RRC_STATIC_MEM
03/15/10   amit    Removed FEATURE_LTE_RRC_DELAY_SIB_READING
02/23/10   amit    Removed FEATURE_LTE_RRC_USE_CANNED_SIB1
02/17/10   amit    Added FEATURE_LTE_RRC_USE_CANNED_SIB1
02/01/10    mm      Removed FEATURE_LTE_RRC_SKIP_CQI_CFG_HACK
01/26/10    mm      Defined FEATURE_LTE_RRC_SKIP_CQI_CFG_HACK
01/24/10   amit    Added FEATURE_LTE_RRC_DELAY_SIB_READING
01/22/10    mm      Defined FEATURE_LTE_RRC_MIMO_HACK
01/11/10   amit    Added FEATURE_LTE_RRC_DISABLE_MIB_READING
12/11/09   vatsac  initial version
=============================================================================*/

#ifndef LTE_RRC_CFG_H
#define LTE_RRC_CFG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/


/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief LTE feature XXX supported by the UE
*/
#define FEATURE_LTE_RRC_XXX

#include "lte_variation.h"
#ifndef TEST_FRAMEWORK
   /*! @brief Feature to disable MIB reading by RRC SIB, remove this after ML1
    fix stabilizes
    */
  //#define FEATURE_LTE_RRC_DISABLE_MIB_READING 
#else
#error code not present
#endif /* TEST_FRAMEWORK */

/*! @brief Feature for using static memory for all private data
*/
//#define FEATURE_LTE_RRC_STATIC_MEM

/*! @brief Feature for using RRC heap and SIB heap. If feature is defined, a 
  chunk of memory is allocated and that is used as RRC heap managed using
  mem_heap. If feature is not defined, all memory allocations within RRC are
  done directly from modem heap (using modem_mem functions).
*/
//#define FEATURE_LTE_RRC_USE_RRC_HEAP
//#define FEATURE_LTE_RRC_MEM_DEBUG
#define FEATURE_SET_UEID_FOR_PG
#define FEATURE_LTE_RRC_CSP_BAND_REORDER_HACK
//#define FEATURE_LTE_RRC_FUZZ_TEST
//#define FEATURE_LTE_RRC_MEM_REPORT

#endif /* LTE_RRC_CFG_H */

