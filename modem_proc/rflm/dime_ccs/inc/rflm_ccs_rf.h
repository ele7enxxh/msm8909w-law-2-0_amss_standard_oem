#ifndef RFLM_CCS_RF_H
#define RFLM_CCS_RF_H

/*!
  @file
  rflm_ccs_rf.h

  @brief
  RFLM CCS RF driver

  @detail
  API for RFLM CCS Processor and Data Manager RF support. 

*/
/*===========================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dime_ccs/inc/rflm_ccs_rf.h#1 $

when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/04/13     sar     Fixed compiler warnings.
09/03/13     ra      Exported api and definitions to RFLM API CCS folder
07/16/13     ra      Initial port to Bolt RF SW
==============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_ccs_rf_intf.h"

// XXX Include deprecated code until techs can clean up
#ifndef FEATURE_RF_BOLT_PRE_SILICON
#define FEATURE_RF_BOLT_PRE_SILICON
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==========================================================================*/


#ifndef FEATURE_RF_BOLT_PRE_SILICON
typedef struct
{
  union
  {
    /*! @brief Make an array for generic copies */
    rflm_ccs_common_log_buffer_s rf_common_log_buf[FW_RF_COMMON_NUM_CAPTURES];
    /*! @brief Sample captures for DPD calibration */
    rflm_ccs_dpd_samp_buffer_s dpd_samp_buffer[FW_DPD_MAX_SAMP_BUF];
  };
} rflm_ccs_calibration_intf_s;
#endif /* FEATURE_RF_BOLT_PRE_SILICON */

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#ifndef FEATURE_RF_BOLT_PRE_SILICON

//To do: replace rflm_ccs_common_intf_s with Data Manager
/* Common RF shared memory interface */
extern rflm_ccs_common_intf_s *rflm_ccs_common_intf_ptr;

//To do: replace rflm_ccs_calibration_intf_s with Data Manager
/* RF calibration shared memory interface */
extern rflm_ccs_calibration_intf_s *rflm_ccs_calibration_intf_ptr;
#endif /* FEATURE_RF_BOLT_PRE_SILICON */





#endif /* RFLM_CCS_RF_H */
