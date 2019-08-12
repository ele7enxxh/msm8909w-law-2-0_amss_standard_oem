#ifndef FTM_COMMON_RADIO_H
#define FTM_COMMON_RADIO_H

/*!
  @file
  ftm_common_radio.h

  @brief
  This module contains defintions and prototypes. FTM common Radio Control 
  code which is used to exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_radio.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/06/14   aro     Support to override ILPC mode of operation
08/19/13   spa     Added FTM_COMMON_RADIO_ENTER_MODE command
07/16/13   aro     Added ASDIV FTM mode testing support
04/02/13   aro     Command for enable debug level for calv3
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/01/13   aro     Added command to perform HW timer test
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
01/18/13   aro     Removed nikel-XPT specific code
10/12/12   Saul    Common XPT. Load DPD framework.
08/01/12   aro     Added EPT override Test function
06/21/12    vb     Added support for QFE1510 
06/08/12   aro     Added EPT IQ capture command
03/18/12   aro     Added function to set the SAR state
12/30/10   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_radio_set_sar_state
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_radio_test_hw_timer
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_radio_calv3_debug
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_radio_set_antenna
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_driver_verification_sandbox
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_radio_enter_mode
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_fbrx_iplc_override
(
  void  *ftm_req_data
);

#ifdef __cplusplus
}
#endif

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_COMMON_RADIO_H */

