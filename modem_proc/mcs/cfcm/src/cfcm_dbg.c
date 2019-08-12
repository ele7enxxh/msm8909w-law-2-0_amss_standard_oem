/*!
  @file
  cfcm_dbg.c

  @brief
  CFCM related runtime tunable parameters.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_dbg.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/15/15   rj      Adding support for PDCP COMPRESSION feature for LTE
12/11/14   rj      New Monitors in DSM Large and Dup Pool
09/08/14   rj      Added support for DSM monitor for IPA client
04/07/14   rj      initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <customer.h>
#include "cfcm_cfg.h"
#include "cfcm_dbg.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/

STATIC cfcm_dbg_s cfcm_dbg =
{
#if defined(FEATURE_DIME_1_1) || defined(FEATURE_MDM9K_TGT_2_3)
  CFCM_CFG_NO_PA_MONITOR_FC_ENABLED_MASK,
#else
  CFCM_CFG_DEFAULT_MONITOR_FC_ENABLED_MASK,
#endif
  CFCM_CFG_DEFAULT_CLIENT_FC_ENABLED_MASK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_DNE_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_MANY_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_DNE_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_FEW_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_MANY_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_A2_UL_PER_FEW_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_A2_UL_PER_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_DNE_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_MANY_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_FEW_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_MANY_MARK,
  CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_DNE_MARK,
  CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_FEW_MARK,
  CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_IPA_DL_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_IPA_DL_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_MANY_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_FEW_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_FEW_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_MANY_MARK,
  CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_DNE_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_FEW_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_MANY_MARK,
  CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_DNE_MARK
};

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_dbg_get_data

==============================================================================*/
/*!
    @brief
    returns the pointer to the cfcm_dbg structure.

    @return
    the pointer to the cfcm_dbg structure

*/
/*============================================================================*/
EXTERN cfcm_dbg_s* cfcm_dbg_get_data( void )
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  return &cfcm_dbg;
} /* cfcm_dbg_get_data() */




/*==============================================================================

                                UNIT TEST

==============================================================================*/

