#ifndef __LMTSMGR_TRANSLATION_H__
#define __LMTSMGR_TRANSLATION_H__
/*!
  @file lmtsmgr_translation.h

  @brief
   Translations functions exposed by the LMTSMGR Layer

*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/inc/lmtsmgr_translation.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/06/15   rj      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"
#include "comdef.h"
#include "tcxomgr.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

#if defined(FEATURE_MCS_STX_TO_CXM_CONVERT) || defined(FEATURE_LMTSMGR_SIMULATOR_SUPPORT)
/*=============================================================================

  FUNCTION:  lmtsmgr_trans_rf_bands_to_sys_bands

=============================================================================*/
uint32 lmtsmgr_trans_rf_bands_to_sys_bands 
(
  cxm_tech_type tech_id,
  uint32 tech_band
);
#endif

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_lmts_tech_to_xo_tech

=============================================================================*/
/*!
    @brief
    Get the XO tech ID corresponding to the limitsmgr tech ID
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_trans_lmts_tech_to_xo_tech
(
  cxm_tech_type   tech,
  tcxomgr_client_info_struct_type *xo_rgs_client
);

#ifdef FEATURE_LMTSMGR_SIMULATOR_SUPPORT
#error code not present
#endif /* FEATURE_LMTSMGR_SIMULATOR_SUPPORT */

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_to_sys_id

=============================================================================*/
/*!
    @brief
    Converts CXM tech IDs to MMCP tech IDs
 
    @return
    MMCP SYS tech ID
*/
/*=============================================================================*/
sys_sys_mode_e_type lmtsmgr_trans_cxm_to_sys_id 
(
  cxm_tech_type tech
);

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_trm_to_cxm_client

=============================================================================*/
/*!
    @brief
    This is a TRM to CXM client mapping 
 
    @return
    None
*/
/*===========================================================================*/
cxm_tech_type lmtsmgr_trans_trm_to_cxm_client 
(
  trm_client_enum_t client    /* Client's whose state update is sent */
);

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_to_trm_client

=============================================================================*/
/*!
    @brief
    Converts CXM tech IDs to TRM client IDs
 
    @return
    TRM client ID
*/
/*=============================================================================*/
trm_client_enum_t lmtsmgr_trans_cxm_to_trm_client 
(
  cxm_tech_type tech    
);
#endif /* Antenna Switch Div feature */

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_sys_bands_to_rf_bands

=============================================================================*/
/*!
    @brief
    Maps sys bands to RF bands 
 
    @return
    None
*/
/*===========================================================================*/
uint32 lmtsmgr_trans_sys_bands_to_rf_bands 
(
  uint32 tech_band,
  cxm_tech_type tech_id
);

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_cxm_tech_to_rf_mode

=============================================================================*/
/*!
    @brief
    Maps tech type to RF mode
 
    @return
    None
*/
rfm_mode_enum_type lmtsmgr_trans_cxm_tech_to_rf_mode
(
  cxm_tech_type tech_id
);

/*=============================================================================

  FUNCTION:  lmtsmgr_trans_sys_id_to_cxm_client

=============================================================================*/
/*!
    @brief
    Converts MMCP SYS Tech IDs to CXM client IDs
 
    @return
    CXM client ID
*/
/*=============================================================================*/
cxm_tech_type lmtsmgr_trans_sys_id_to_cxm_client
(
  sys_sys_mode_e_type  mode,
  sys_modem_as_id_e_type asid
);

#endif /* __LMTSMGR_TRANSLATION_H__ */
