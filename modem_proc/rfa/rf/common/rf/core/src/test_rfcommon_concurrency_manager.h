/*!
   @file
   test_rfcommon_concurrency_manager.h

   @brief
 
*/

/*===========================================================================

Copyright (c) 2012 - 2014 by QUALCOMM Technologies Inc. All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
-------------------------------------------------------------------------------
04/23/14   tks     Initial version.

============================================================================*/

#define RF_STDA_PATH_SEL_TBL_SIZE 8 
#define RF_IRAT_PATH_SEL_TBL_SIZE 13
#include "rfcommon_concurrency_manager.h"

/* Global definition for standalone path selection table */
extern rfc_alt_path_sel_type rf_oft_pathsel_tbl[RF_STDA_PATH_SEL_TBL_SIZE][RFM_MAX_WAN_DEVICES]; 

/* Global definition for irat path selection table */
extern rfc_alt_path_sel_type rf_oft_irat_pathsel_tbl[RF_IRAT_PATH_SEL_TBL_SIZE][RFM_MAX_WAN_DEVICES]; 

/* Interface function to retrieve the concurrency manager table */
rfcmn_state_mgr_type* rfcommon_get_concurrency_mgr_data(void);

