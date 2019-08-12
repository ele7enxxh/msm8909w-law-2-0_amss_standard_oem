#ifndef RF_HDR_MEAS_H
#define RF_HDR_MEAS_H

/*! 
  @file
  rf_hdr_meas.h
 
  @brief
  This file includes the definitions and protoype used for HDR Measurement.
 
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_meas.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/03/13   spa     Remove script_build_scripts
06/27/13   spa     Removed unused/no-op APIs 
06/18/13   APU     Declared missing APIs so the test files can see the APIs.
10/10/12   APU     Added the init API.
03/21/12   Saul    IRAT. HDR to set second device to sleep in L2DO.
12/21/11   aro     Added a new function to perform HDR cleanup which is
                   called before enter LTE script enter in DO2L measurement.
07/15/11   Saul    IRAT updates.
07/08/11   Saul    Preliminary IRAT changes.
07/08/11   Saul    Preliminary IRAT changes.
12/13/10   aro     Added HDR Measurement APIs
12/07/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfm_meas.h"
#include "rfmeas_hdr.h"
#include "sys.h"

/*----------------------------------------------------------------------------*/
rfm_meas_result_type
rf_hdr_cleanup_for_measurement
(
  rfm_meas_hdr_enter_param_type* irat_meas_param
);

/*----------------------------------------------------------------------------*/
rfm_meas_result_type
rf_hdr_cleanup_post_measurement
(
  rfm_device_enum_type src_tech_device,
  rfm_meas_hdr_setup_and_build_scripts_param_type* irat_meas_param
);

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_hdr_mc_meas_enter 
( 
   rfm_meas_enter_param_type* rfm_meas_enter_param 
) ;

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_hdr_mc_meas_build_scripts
( 
    rfm_meas_setup_param_type* rfm_meas_setup_param 
) ;

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_hdr_mc_meas_exit 
( 
   rfm_meas_exit_param_type* rfm_meas_exit_param 
) ;


/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_hdr_mc_irat_info_get 
( 
   rfm_meas_irat_info_type* rfm_meas_irat_info_param 
) ;

/*----------------------------------------------------------------------------*/
boolean rf_hdr_mc_meas_init(void);

/*----------------------------------------------------------------------------*/

#endif /* RF_HDR_MEAS_H */

