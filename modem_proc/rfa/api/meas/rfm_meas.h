#ifndef RFM_MEAS_H
#define RFM_MEAS_H
/*
   @file
   rfm_meas.h

   @brief
   RF Driver's Inter Freq Meas external interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 2008 - 20012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
                   EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfm_meas.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/20/12   pl      remove obsolete interfaces
02/03/12   av      IRAT cleanup phase 1 
03/31/11   sar     Remove #def's to comply with CMI-4.
03/28/11   sar     Relocating file for rfa/api decoupling.
09/29/10   av/jyu  Created new API rfm_meas_common_get_irat_info_param  
08/18/10   pl      Added new APIs for script-based IRAT
03/23/10   can     Fixes.
03/19/10   can     Initial common measurement APIs and support.
03/01/10   tws     Add new measurement API prototypes.
10/14/08   sr      Initial version to separate meas specific external interface.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_gsm.h"
#include "rfmeas_lte.h"
#include "rfmeas_wcdma.h"
#include "rfmeas_1x.h"
#include "rfmeas_hdr.h"


/*----------------------------------------------------------------------------*/
/*

*/
/*----------------------------------------------------------------------------*/

rfm_meas_time_const_type rfm_meas_common_get_time_constant ( rfcom_mode_enum_type source,  
                                          rfcom_mode_enum_type target);

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfm_meas_common_script_enter( rfm_meas_enter_param_type *rfm_meas_enter_param, 
                                                   rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfm_meas_common_script_build_scripts( rfm_meas_setup_param_type *rfm_meas_setup_param, 
                                                           rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfm_meas_common_script_exit( rfm_meas_exit_param_type *rfm_meas_exit_param, 
                                                  rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfm_meas_common_get_irat_info_param( 
                              rfm_meas_irat_info_type *rfm_meas_irat_info_param, 
                              rfm_cb_handler_type cb_handler );
#endif /* RFM_MEAS_H */

