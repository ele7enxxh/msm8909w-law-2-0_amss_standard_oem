#ifndef RFMEAS_MC_H
#define RFMEAS_MC_H

/*
   @file
   rfmeas_mc.h

   @brief

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_mc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/08/13   pl      rename cleanup_script_fp to reset_script_fp
04/08/13   pl      Add pre_config function pointer
01/31/13   sc      Added iRAT msgr register/deregsiter prototypes
11/06/12   pl      Add Cleanup script function pointer
10/04/12   sr      IRAT code cleanup for dime. 
02/03/12   av      IRAT cleanup phase 1 
01/17/12   av      Fix call flow for XtoG 
11/11/11   sr      removed unused IRAT code.
09/19/11   dw      Add get LtoW PLL timing support 
07/29/11   av      Populate GtoW start and cleanup times 
05/27/11   dw      Add rfmeas_mc_gtow_get_irat_info_param()
03/31/11   sar     Remove #def's to comply with CMI-4.
10/05/10   jyu     Fix compilation warnings 
05/20/10   tws     IRAT for MDM9K.
03/24/10   ka      Restore wtow
02/25/10   ka      Temporarily remove wtow
12/02/09   ckl     Added W2W measurement function.
10/14/08   sr      Initial version.

============================================================================*/


#include "rfcom.h"
#include "msgr_types.h"
#include "rfmeas_types.h"

extern boolean rfmeas_msgr_register( msgr_client_t *client_id, msgr_id_t id );

extern boolean rfmeas_msgr_deregister( msgr_client_t *client_id );

/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfm_meas_common_get_irat_info_param function will basically allow RFSW to pass to
    L1 how much the start-up and clean-up scripts take, and IRAT specific info if necessary
    for individual IRAT combination.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
  @param rfm_cb_handler_type: callback 

  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_mc_irat_info_get_fp_type)(rfm_meas_irat_info_type *irat_info_param);


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.

  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.

  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_mc_enter_fp_type)(rfm_meas_enter_param_type *meas_enter_param);


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the reset scripts to reset
    settings of previous tech
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_build_reset_scripts_fp_type)(rfm_meas_setup_param_type *meas_scripts_param);


/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the tune script to the specific technology,
    either target (tune-away script) or source (tune-back script)
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfmeas_build_reset_scripts function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_build_scripts_fp_type)(rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to perform pre-configuration of either an unused
    tranceiver device, or a partial configuration of a transceiver currently in used.
 
    Only preconfigure for target tech, and only preconfigure in such way that source tech
    operation is not disturbed.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_pre_config_fp_type)(rfm_meas_setup_param_type *meas_scripts_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.

  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.

  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
typedef rfm_meas_result_type (*rfmeas_mc_exit_fp_type)(rfm_meas_exit_param_type *meas_exit_param);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of IRAT interface function pointersfor each tech.
*/

typedef struct
{
  rfmeas_mc_irat_info_get_fp_type  rfmeas_mc_irat_info_get_fp;
  rfmeas_mc_enter_fp_type rfmeas_mc_enter_fp;
  rfmeas_pre_config_fp_type rfmeas_mc_pre_config_fp;
  rfmeas_build_reset_scripts_fp_type rfmeas_build_reset_scripts_fp;
  rfmeas_build_scripts_fp_type rfmeas_build_scripts_fp;
  rfmeas_mc_exit_fp_type rfmeas_mc_exit_fp;
} rfmeas_mc_func_tbl_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will register the IRAT API function ptrs from each tech.

  @param
  tech: Tech which is registering its IRAT APIs func_ptr table.
  func_tbl_ptr : tech's IRAT APIs table.
 
  @retval 
  Returns TRUE if the registration is successful, else FALSE.
*/

boolean rfmeas_mc_register(rfcom_mode_enum_type tech, rfmeas_mc_func_tbl_type *func_tbl_ptr);


/*----------------------------------------------------------------------------*/

rfm_meas_result_type rfmeas_mc_irat_info_get(rfm_meas_irat_info_type *irat_info_param, 
                                             rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfmeas_mc_enter( rfm_meas_enter_param_type *meas_enter_param, 
                                      rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfmeas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param, 
                                           rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/
rfm_meas_result_type rfmeas_mc_exit( rfm_meas_exit_param_type *meas_exit_param, 
                                     rfm_cb_handler_type cb_handler );

/*----------------------------------------------------------------------------*/

#endif /* RFMEAS_MC_H */
