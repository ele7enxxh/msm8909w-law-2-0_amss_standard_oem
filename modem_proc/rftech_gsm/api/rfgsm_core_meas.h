/*!
   @file
   rfgsm_core_meas.h

   @brief

   @details

*/
/*=============================================================================
Copyright (c) 2008-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_meas.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/07/14   piy     Adding rfgsm_core_script_type enum 
05/14/14   cj/ck   Add Do2G support
03/26/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
04/23/13   cj      Added pre_config and cleanup APIs
12/20/12   sc      Use the RFGSM max SBI value to define the max num SBIs 
11/19/12   sc      RF-Only mDSP update shared variables cleanup
11/07/12   sc      Calculate the iRAT timings instead of explicitly defining
10/04/12   sr      Initial version

============================================================================*/

#ifndef RFGSM_CORE_MEAS_H
#define RFGSM_CORE_MEAS_H

#include "rfmeas_types.h"
#include "rfgsm_core_types.h"

/*----------------------------------------------------------------------------*/
/* @brief The following definitions are used to calculate the time offsets of */
/*        iRAT startup, power measurements, and cleanup                       */
/* @ TODO SC These numbers need to be reconfirmed on-target                   */
#define WTOG_STARTUP_SBI_MAX    38
#define LTOG_STARTUP_SBI_MAX    150
#define DOTOG_STARTUP_SBI_MAX   150
#define TTOG_STARTUP_SBI_MAX    65

#define XTOG_PWR_MEAS_SBI_MAX   RFGSM_MAX_RX_START_SBI_TRANSACTIONS

#define WTOG_CLEANUP_SBI_MAX    38
#define LTOG_CLEANUP_SBI_MAX    150
#define TTOG_CLEANUP_SBI_MAX    65

#define RFGSM_STARTUP_TIME_CALC_QS(num_sbis) RFGSM_SBI_TIME_CALC_QS(num_sbis)
#define RFGSM_PWR_MEAS_TIME_CALC_QS(num_sbis,pll_tune_time) RFGSM_RF_RX_SETUP_TIME_QS(num_sbis, pll_tune_time)
#define RFGSM_CLEANUP_TIME_CALC_QS(num_sbis) RFGSM_SBI_TIME_CALC_QS(num_sbis)

typedef enum
{
 RFGSM_CORE_IRAT_STARTUP,
 RFGSM_CORE_IRAT_CLEANUP,
 RFGSM_CORE_IRAT_ABORT
}rfgsm_core_script_type;

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_irat_info_get(rfm_meas_irat_info_type *irat_info_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_irat_update_rx_timings(rfm_meas_irat_info_type *irat_info_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_meas_enter( rfm_meas_enter_param_type *meas_enter_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_meas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_meas_exit( rfm_meas_exit_param_type *meas_exit_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_meas_pre_config( rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfgsm_core_meas_cleanup(rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/

void rfgsm_core_disable_rffe(rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/
#endif


