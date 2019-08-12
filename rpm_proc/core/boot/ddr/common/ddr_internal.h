#ifndef DDR_INTERNAL_H
#define DDR_INTERNAL_H

/**
 * @file ddr_internal.h
 * @brief
 * Function headers of DDR drivers internal private API
 */

/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/common/ddr_internal.h#1 $ 
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/06/12   tw      Added support for mx scaling
12/08/11   tw      Initial revision.
================================================================================
                  Copyright 2011 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
==============================================================================*/
#include "ddr_params.h"
#include "ddr_common.h"
#include "railway.h"

/* -----------------------------------------------------------------------
**                           DEFINES
** ----------------------------------------------------------------------- */

#define DDR_TRAINING_REQUIRED         0x6188709B
#define DDR_TRAINING_NOT_REQUIRED     0xACBE001F
#define DDR_TRAINING_DATA_UPDATED     0xDAAA11E1
#define DDR_TRAINING_DATA_NOT_UPDATED 0xDE8D526A
#define DDR_TEST_FRAMEWORK           0x3E3F2431


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ============================================================================
**  Function : ddr_target_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called at the end of ddr init to initialize any 
*   target specific configurations 
*   
*   @details
*   Target specific configurations such as override of timing registers and 
*   calling target specific init functions will be captured in this api
*   
*   @param 
*   None
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_target_init(void);

/* ============================================================================
**  Function : ddr_perform_training
** ============================================================================
*/
/*!
*   @brief
*   This function will train the ddr at every voltage and frequency level
*   supported
*
*   @details
*   This function will iterate through all voltage and frequency levels in order
*   to train for SITE. at the highest voltage and frequency level, it will train
*   for PHY.
*
*   @param
*   None
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean ddr_perform_training(void);

/* ============================================================================
**  Function : ddr_update_training_checksum
** ============================================================================
*/
/*!
*   @brief
*   This function call the crc32 api to calculate a checksum for the entire
*   training partition. The checksum is then saved as part of the partition
*   header.
*
*   @details
*   This function call the crc32 api to calculate a checksum for the entire
*   training partition. The checksum is then saved as part of the partition
*   header.
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  uint32 - crc32 calculated
*
*   @sa None
*/
uint32 ddr_update_training_header(void);

/* ============================================================================
**  Function : ddr_verify_checksum
** ============================================================================
*/
/*!
*   @brief
*   This function call the crc32 api to calculate a checksum for the entire
*   training partition. The checksum is then compared with the one saved in the
*   partition.
*
*   @details
*   This function call the crc32 api to calculate a checksum for the entire
*   training partition. The checksum is then compared with the one saved in the
*   partition. If they do not match, it indicates the partition has been corrupted
*   and dram stability is not guaranteed, a retraining will be required.
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  boolean - checksum match true/false
*
*   @sa None
*/
boolean ddr_verify_checksum(struct ddr_params_partition * ddr_training_partition);

/* ============================================================================
**  Function : ddr_phy_training
** ============================================================================
*/
/*!
*   @brief
*   This function will train the ddr phy at the highest frequency supported.
*
*   @details
*   This function will train the ddr phy at the highest frequency supported.
*   the rest of frequencies will be interpolated.
*
*   @param interface_name     -  [IN] the interface to train for
*   @param chip_select        -  [IN] the chip select to train for
*   @param training_params    -  [IN/OUT] pointer to training parameter to
*                                         populate trained results.
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void ddr_phy_training(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select,
    struct ddr_trained_params_lpddr3 *training_params);

/* ============================================================================
**  Function : ddr_get_cx_voltage_level
** ============================================================================
*/
/*!
*   @brief
*   This function will iterate through the voltage levels and find the appropriate
*   index. If voltage is not recorded in the structure, add it.
*
*   @details
*   This function will iterate through the voltage levels and find the appropriate
*   index. If voltage is not recorded in the structure, add it.
*
*   @param training_params    -  [IN/OUT] pointer to training parameter to
*                                         populate trained results.
*   @param vlevel             -  [IN] Voltage level to compare for
*
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  uint32 - index of voltage level passed in.
*
*   @sa None
*/
uint32 ddr_get_cx_voltage_level(struct ddr_trained_params_lpddr3 *ddr_param, uint32 vlevel);

/* ============================================================================
**  Function : ddr_get_mx_voltage_level
** ============================================================================
*/
/*!
*   @brief
*   This function will iterate through the voltage levels and find the appropriate
*   index. If voltage is not recorded in the structure, add it.
*
*   @details
*   This function will iterate through the voltage levels and find the appropriate
*   index. If voltage is not recorded in the structure, add it.
*
*   @param training_params    -  [IN/OUT] pointer to training parameter to
*                                         populate trained results.
*   @param vlevel             -  [IN] Voltage level to compare for
*
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  uint32 - index of voltage level passed in.
*
*   @sa None
*/
uint32 ddr_get_mx_voltage_level(struct ddr_trained_params_lpddr3 *ddr_param, uint32 vlevel);


/* ============================================================================
**  Function : ddr_pre_vddcx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before vddcx voltage switch occurs.
*
*   @param none
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_pre_vddcx_switch(const railway_settings *proposal, void * callback_cookie);

/* ============================================================================
**  Function : ddr_post_vddcx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after vddcx voltage switch occurs.
*
*   @param none
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_post_vddcx_switch(const railway_settings *proposal, void * callback_cookie);

/* ============================================================================
**  Function : ddr_pre_vddmx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before vddmx voltage switch occurs.
*
*   @param none
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_pre_vddmx_switch(const railway_settings *proposal, void * callback_cookie);

/* ============================================================================
**  Function : ddr_post_vddmx_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after vddmx voltage switch occurs.
*
*   @param none
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/

void ddr_post_vddmx_switch(const railway_settings *proposal, void * callback_cookie);

/* ============================================================================
**  Function : ddr_increase_clock_speed
** ============================================================================
*/
/*!
*   @brief
*   This function will bump up the clock speed for ddr to the maximum nominal
*   frequency
*
*   @details
*   This function will use clkrgm npa voting to vote for max nominal nominal
*   frequency
*
*   @param None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval
*   boolean - operation successful or not.
*
*   @sa None
*/

boolean ddr_increase_clock_speed(void);

void ddr_test_framework(void);
#endif
