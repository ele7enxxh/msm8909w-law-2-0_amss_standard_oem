#ifndef RF_1X_MEAS_H
#define RF_1X_MEAS_H

/*! 
  @file
  rf_1x_meas.h
 
  @brief
  This file includes the definitions and protoype used for 1x Measurement.
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_meas.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/03/13   spa     Remove script_build_scripts
07/01/13   APU     Call RX_STOP from 1 place and remove 
                   rf_1x_cleanup_post_measurement() because we use sleep when 
                   call meas_exit()
06/27/13   spa     Removed unused/no-op APIs 
10/17/12   sty     Added prototype for rf_1x_mc_meas_init() 
10/15/12   APU     IRAT. Added the new APIs.                 
03/21/12   Saul    IRAT. 1X to set second device to sleep in L21X. 
03/15/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
07/15/11   Saul    IRAT updates.
07/08/11   Saul    Preliminary IRAT changes.
07/08/11   Saul    Preliminary IRAT changes.
12/13/10   aro     Added 1x Measurement APIs
12/07/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfm_meas.h"
#include "rfmeas_1x.h"

/*----------------------------------------------------------------------------*/
rfm_meas_result_type
rf_1x_cleanup_for_measurement
(
  rfm_meas_1x_enter_param_type* irat_meas_param
);


/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_1x_mc_irat_info_get
( 
  rfm_meas_irat_info_type *rfm_meas_irat_info_param
) ;

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_1x_mc_meas_enter
( 
  rfm_meas_enter_param_type *rfm_meas_enter_param
) ;

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_1x_mc_meas_build_scripts
( 
  rfm_meas_setup_param_type *rfm_meas_setup_param
) ;

/*----------------------------------------------------------------------------*/
rfm_meas_result_type 
rf_1x_mc_meas_exit
( 
  rfm_meas_exit_param_type *rfm_meas_exit_param
) ;
   
/*----------------------------------------------------------------------------*/
boolean 
rf_1x_mc_meas_init
(
  void 
);
   
#endif /* RF_1X_MEAS_H */

