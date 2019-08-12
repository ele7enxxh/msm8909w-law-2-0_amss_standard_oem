#ifndef PM_RPM_UTILITIES_H
#define PM_RPM_UTILITIES_H

 /*! \file pm_rpm_utilities.h
 *
 *  \brief This file contains RPM PMIC utility function prototypes.
 *
 *  &copy; Copyright 2012 - 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_rpm_utilities.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/14   rk      PM8916: SW workaround for LDO11 damage issue with faulty uSD cards (CR - 686702)
09/10/13   rk      Fixed the naming convention in \config.
09/10/13   rk      Code refactoring.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rpm_npa.h"
#include "npa.h"
#include "pm_npa.h"
#include "pm_pwr_alg.h"
#include "com_dtypes.h"
#include "pm_comm.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @brief This function is used to do memcpy and memcmp.
 * @param source Pointer to the source of data to be copied, type-casted to a pointer of type const void*.
 * @param destination Pointer to the destination array where the content is to be copied, type-casted to 
          a pointer of type void*.
 * @param num Number of bytes to copy.
 * 
 * @return Returns an integral value indicating the relationship between the content of the memory blocks:
   A zero value indicates that the contents of both memory blocks are equal.
   A value greater than zero indicates that the first byte that does not match in both memory blocks has a 
   greater value in ptr1 than in ptr2 as if evaluated as unsigned char values; And a value less than zero 
   indicates the opposite.
 */
int pm_rpm_int_copy(void *source, void *destination, size_t num);

/**
 * @brief This function is used to check if a regulator has settled by reading the real time status of VREG_OK register.
 * @param settle_start_time The timestamp at which the rail started to settle. 
 * @param estimated_settling_time_us The estimated time (in uS) that the rail is supposed to take to settle.
 * @param vreg_ok_status Function pointer for checking the VREG_OK status
 * @param pwr_res Pointer to the power resource. 
 * @param resource Pointer to the driver resource. 
 * @param resource_index Resource index.
 * @param settling error enabled - to ignore settling errors
 * 
 * @return returns the final VREG_OK status.
 * @note vreg_ok_status function pointer needs to be initialized when this function gets called.
 */
boolean pm_rpm_check_vreg_settle_status(uint64 settle_start_time, uint32 estimated_settling_time_us, 
                                        pm_pwr_data_type *pwr_res, pm_comm_info_type *comm_ptr, 
                                        uint8 resource_index, boolean settling_err_en);

/**
 * @brief This function is used to check if a regulator has settled by reading the real time status of STEPPER_DONE register.
 * @param settle_start_time The timestamp at which the rail started to settle. 
 * @param estimated_settling_time_us The estimated time (in uS) that the rail is supposed to take to settle.
 * @param stepper_done_status Function pointer for checking the STEPPER_DONE status
 * @param pwr_res Pointer to the power resource.
 * @param resource Pointer to the driver resource. 
 * @param resource_index Resource index.
 * @param settling error enabled - to ignore settling errors
 * 
 * @return returns the final STEPPER_DONE status..
 * @note stepper_done_status function pointer needs to be initialized when this function gets called.
 */
boolean pm_rpm_check_stepper_settle_status(uint64 settle_start_time, uint32 estimated_settling_time_us, 
                                           pm_pwr_data_type *pwr_res, pm_comm_info_type *comm_ptr, 
                                           uint8 resource_index, boolean settling_err_en);

/**
 * @brief This function is used to calculate the vset value from 
 *        voltage.
 * @param rsrc_type RPM resource type.
 * @param rsrc_index resource index.
 * @param volt_uv Voltage value in micro volts.
 * @param vset Pointer to calculated vset value to be returned.
 * 
 * @return return PM_ERR_FLAG__SUCCESS if successful else ERROR.
 */
pm_err_flag_type pm_rpm_calculate_vset(rpm_resource_type rsrc_type, uint32 rsrc_index, uint32 volt_uv, uint32* vset);

/**
 * @brief This function is used to return the current voltage 
 *        status of the rail.
 * @param rsrc_type RPM resource type.
 * @param rsrc_index resource index.
 * @param volt_uv Pointer to voltage value in micro volts to be 
 *                returned.
 * 
 * @return return PM_ERR_FLAG__SUCCESS if successful else ERROR.
 */
pm_err_flag_type pm_rpm_volt_level_status(rpm_resource_type rsrc_type, uint32 rsrc_index, uint32* volt_uv);

/**
 * @brief This function is used to get the raw sw mode status.
 * @param rsrc_type RPM resource type.
 * @param rsrc_index resource index.
 * @param mode Pointer to raw mode reg value to be returned.
 * 
 * @return return PM_ERR_FLAG__SUCCESS if successful else ERROR.
 */
pm_err_flag_type pm_rpm_sw_mode_status_raw(rpm_resource_type rsrc_type, uint32 rsrc_index, uint8* mode);

#endif //PM_RPM_UTILITIES_H


