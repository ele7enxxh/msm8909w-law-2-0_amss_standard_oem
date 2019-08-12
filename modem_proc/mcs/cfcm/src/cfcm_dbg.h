/*!
  @file
  cfcm_dbg.h

  @brief
  CFCM runtime tunable parameters related API and structure.

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_dbg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/15/15   rj      Adding support for PDCP COMPRESSION feature for LTE
12/11/14   rj      New Monitors in DSM Large and Dup Pool
09/08/14   rj      Added support for DSM monitor for IPA client
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_DBG_H
#define CFCM_DBG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>



/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
 
/*! @brief runtime tunable parameters
*/
typedef struct
{
  uint32  monitor_fc_enable_mask;/*!< default monitor flow control enable mask */
  uint32  client_fc_enable_mask; /*!< default client flow control enable mask */
  uint32  large_pool_lte_dne;    /*!< free count for LTE DNE */
  uint32  large_pool_lte_dl_few; /*!< free count for LTE DL FEW */
  uint32  large_pool_lte_dl_many;/*!< free count for LTE DL MANY */
  uint32  large_pool_a2_ul_per_few; /*!< free count for A2_UL PER FEW */
  uint32  large_pool_a2_ul_per_many; /*!< free count for A2_UL PER MANY */
  uint32  dup_pool_lte_dne;    /*!< free count for LTE DNE */
  uint32  dup_pool_lte_dl_few; /*!< free count for LTE DL FEW */
  uint32  dup_pool_lte_dl_many;/*!< free count for LTE DL MANY */
  uint32  dup_pool_a2_ul_per_few; /*!< free count for A2_UL PER FEW */
  uint32  dup_pool_a2_ul_per_many; /*!< free count for A2_UL PER MANY */
  uint32  large_pool_rlc_dne;    /*!< free count for RLC DNE */
  uint32  large_pool_rlc_dl_few; /*!< free count for RLC DL FEW */
  uint32  large_pool_rlc_dl_many;/*!< free count for RLC DL MANY */
  uint32  dup_pool_rlc_dl_few; /*!< free count for RLC DL FEW */
  uint32  dup_pool_rlc_dl_many;/*!< free count for RLC DL MANY */
  uint32  small_pool_rlc_dne;    /*!< free count for RLC DNE */
  uint32  small_pool_rlc_dl_few; /*!< free count for RLC DL FEW */
  uint32  small_pool_rlc_dl_many;/*!< free count for RLC DL MANY */
  uint32  large_pool_ipa_dl_many;   /*!<free count for IPA DL MANY*/
  uint32  large_pool_ipa_dl_few;   /*!<free count for IPA DL FEW*/
  uint32  large_pool_lte_dl_level1_few; /*!< free count for LTE DL FEW */
  uint32  large_pool_lte_dl_level1_many;/*!< free count for LTE DL MANY */
  uint32  dup_pool_lte_dl_level1_few;   /*!< free count for LTE DL MANY */
  uint32  dup_pool_lte_dl_level1_many;  /*!< free count for LTE DL MANY */
  uint32  large_pool_lte_pdcp_comp_few; /*!< free count for LTE COMP FEW */
  uint32  large_pool_lte_pdcp_comp_many;/*!< free count for LTE COMP MANY */
  uint32  large_pool_lte_pdcp_comp_dne;/*!< free count for LTE COMP DNE */
  uint32  dup_pool_lte_pdcp_comp_few; /*!< free count for LTE COMP FEW */
  uint32  dup_pool_lte_pdcp_comp_many;/*!< free count for LTE COMP MANY */
  uint32  dup_pool_lte_pdcp_comp_dne;/*!< free count for LTE COMP DNE */
} cfcm_dbg_s;
 
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern cfcm_dbg_s* cfcm_dbg_get_data( void );


#endif /* CFCM_DBG_H */
