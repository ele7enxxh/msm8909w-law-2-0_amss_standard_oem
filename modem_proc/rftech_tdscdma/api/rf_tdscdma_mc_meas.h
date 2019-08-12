#ifndef RF_TDSCDMA_MC_MEAS_H
#define RF_TDSCDMA_MC_MEAS_H
/*!
  @file rf_tdscdma_mc_meas.h

  @brief
  This file contains the tdscdma meas functions.

  @details

*/

/*===========================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_mc_meas.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/14   ms      Disable tuner in X2T iRAT case (porting per CR600367)
12/10/13   zg      Mainlined the content under SGLTE feature;
                   Removed the unused legacy subroutine.           
07/19/13   ms      New header structure and pre-config under SGLTE feature
06/25/13   ms      Added div buff index for ifreq
04/04/13   jyu     Added support for IFREQ measurment on Rx1 chain
10/10/12   ms      Added TDSCDMA IRAT APIs
03/02/12   zg/jyu  Updated support for TDSCDMA<->GSM and added for 
                   TDSCDMA<->TDD LTE 
11/23/11   zg      Added prototype for rf_tdscdma_mc_ttog_enter_qdsp6.
11/09/11   zg      Updates on iRAT API for building scripts
10/21/11   zg      Ifreq upates. 
10/04/11   zg      Initial version.
===========================================================================*/

#include "rfmeas_tdscdma.h"
#include "rf_tdscdma_mc.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function registers the IRAT APIs for TDSCDMA with meas module.
 
  @param None
  @retval None
*/
void rf_tdscdma_mc_meas_init(void);

/*----------------------------------------------------------------------------*/

/*!
  @brief
    This function will populate all the timing information related IRAT measurements. This timing
    information will be used by L1 scheduling the measurements during IRAT gap.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rf_tdscdma_mc_irat_info_get(rfm_meas_irat_info_type *irat_info_param);
/*----------------------------------------------------------------------------*/

/*!
  @brief
  Enter measurement Mode for tdscdma.

  @details
  This function enters the measurement Mode for tdscdma.
 
  @param tds_meas_enter_param
  Parameters for tdscdma Measurement

  @return
  Status of function execution
*/

//#ifdef FEATURE_SGLTE
void rf_tdscdma_mc_meas_script_enter(rfm_meas_enter_param_type* 
                                                tds_meas_enter_param);
//#else
#if 0
void rf_tdscdma_mc_meas_script_enter 
(
  rfm_meas_tdscdma_enter_param_type* tdscdma_meas_enter_param
);

#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rf_tdscdma_mc_meas_enter( rfm_meas_enter_param_type *meas_enter_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit measurement Mode for tdscdma.

  @details
  This function exits the measurement Mode for tdscdma.
 
  @param tds_meas_exit_param
  Parameters for tdscdma Measurement

  @return
  Status of function execution
*/
void rf_tdscdma_mc_meas_script_exit 
(
  rfm_meas_tdscdma_exit_param_type* tdscdma_meas_exit_param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rf_tdscdma_mc_meas_exit( rfm_meas_exit_param_type *meas_exit_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup and Build tdscdma Script

  @details
  This functions initializes and start to build iRAT script

  @param tds_setup_script_param
  Build Script Parameter
 
  @return
  Status of function execution 
*/


//#ifdef FEATURE_SGLTE
rfm_meas_result_type rf_tdscdma_mc_meas_script_build_scripts 
(
  rfm_meas_setup_param_type *tds_setup_param
);
//#else
#if 0
rfm_meas_result_type rf_tdscdma_mc_meas_script_build_scripts 
(
  rfm_meas_tdscdma_setup_and_build_scripts_param_type* tdscdma_meas_param
);

#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rf_tdscdma_mc_meas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update tdscdma script buffers for the neighbour list
 
  @details
  This functions constructs scripts in SMEM for each channel in the neighbour list.

  @param num_tds_nbr
  Number of channels in the neighbour list.
 
  @param tds_nbr_list
  Neighbour list.
 
  @param carrier_freq_list
  Pointer of carrier_freq array, whose value will be returned by this function.
 
  @param ifreq_rxlm_buf_idx
  Index of ifreq RXLM buffer, which will be updated by this function.
 
  @param ifreq_rxlm_mode
  Index of ifreq RXLM mode, which will be updated by this function.
 
  @param rx_path
  Primary only, diversity only, or both 
 
  @param ifreq_rxlm_buf_idx_div
  Index of ifreq RXLM buffer for Rx1 chain, which will be updated by this function.
 
  @param ifreq_rxlm_mode_div
  Index of ifreq RXLM mode for Rx1 chain, which will be updated by this function.
 
  @return
  Status of function execution. 
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_meas_update_tds_nbr
( 
  uint8 num_tds_nbr,
  rfa_tdscdma_nbr_entry_t tds_nbr_list[],
  uint32 carrier_freq_list[],
  lm_handle_type ifreq_rxlm_buf_idx,
  rfa_tdscdma_rxlm_mode_t ifreq_rxlm_mode,
  rfa_tdscdma_rx_path_t    rx_path,
  lm_handle_type           ifreq_rxlm_buf_idx_div,
  rfa_tdscdma_rxlm_mode_t  ifreq_rxlm_mode_div,
  uint8 div_buf_idx
);  
  
//void rftdscdma_mc_ifreq_script_build_scripts (rfm_ifreq_setup_and_build_scripts_param_type *
//                                         ifreq_start_script_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get T-> G iRAT timing information
 
  @details
  This functions will return the setup time for Gap setup, Gap cleanup and
  burst measurement setup.

  @param irat_info
  pointer of the timing structure
 
  @return
  Content of timing structure. 
*/
void rf_tdscdma_mc_t2g_get_irat_info_param
( 
  rfm_meas_ttog_irat_info_param_type * irat_info 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get G-> T iRAT timing information
 
  @details
  This functions will return the setup time for Gap setup, Gap cleanup and
  burst measurement setup.

  @param irat_info
  pointer of the timing structure
 
  @return
  Content of timing structure. 
*/
void rf_tdscdma_mc_g2t_get_irat_info_param
( 
  rfm_meas_irat_info_type * irat_info 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Build T2G iRAT setup and cleanup Script

  @details
  This functions initializes and start to build the GSM script for T2G 
  and the cleanup script for returning to the serving technology (TDSCDMA)

  @param setup_param
  Build Script Parameter
   
  @return
  Status of function execution 
*/
#if 0
rfm_meas_result_type rf_tdscdma_mc_ttog_enter_qdsp6
( 
//  rfm_meas_tech_params_type * setup_params
  rfm_meas_params_channel_type* channel_params
);
#endif

rfm_meas_result_type rf_tdscdma_mc_meas_reset_scripts( rfm_meas_setup_param_type *meas_scripts_param);

//#ifdef FEATURE_SGLTE
rfm_meas_result_type rf_tdscdma_mc_meas_write_preconf_script( rfm_device_enum_type tdscdma_device );


rfm_meas_result_type rf_tdscdma_mc_meas_preconf_required( rfm_meas_tdscdma_setup_and_build_scripts_param_type tds_meas_param,
                                                     boolean* preconf_required );


rfm_meas_result_type rf_tdscdma_mc_meas_pre_config( rfm_meas_setup_param_type *meas_scripts_param);

//#endif
#endif /*RF_TDSCDMA_MC_MEAS_H*/
