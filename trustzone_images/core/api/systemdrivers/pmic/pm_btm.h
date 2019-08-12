#ifndef PM_BTM_H
#define PM_BTM_H

/*! \file
*  \n
*  \brief  pm_btm.h PMIC-BTM MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC BTM (Battery Temperature Management) module. 
*  \n &copy; Copyright 2011 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_btm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/11   hs      Initial version. 
========================================================================== */
#include "comdef.h"
#include "pm_lib_err.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_btm_mode_type
   \brief The BTM mode
 */
typedef enum
{
   /*! BTM module OFF  */
   PM_BTM_MODE__MODULE_OFF,
    /*! BTM module ON  */
   PM_BTM_MODE__MODULE_ON
}pm_btm_mode_type;
/*! \enum pm_btm_conv_req_cmd_type
   \brief Use these commands to set or clear the conversion requestin in the
 *  the BTM module.
 */
typedef enum
{
    /*! Command to stop a conversion.  */
   PM_BTM_CONV_REQ_CMD__OFF,
    /*! Command to request a conversion.  */
   PM_BTM_CONV_REQ_CMD__ON
}pm_btm_conv_req_cmd_type;

/*! \enum pm_btm_xoadc_conversion_status_type
   \brief The XOADC conversion status.
 */
typedef enum
{
    /*! Conversion complete.  */
    PM_BTM_XOADC_CONVERSION_STATUS__COMPLETE,
    /*! Waiting for ADC to complete another process's conversion.  */
    PM_BTM_XOADC_CONVERSION_STATUS__PENDING
} pm_btm_xoadc_conversion_status_type;

/*! \enum pm_btm_meas_mode_type
   \brief The measurement mode.
 */
typedef enum
{
   PM_BTM_MEAS_MODE_SINGLE = 0,
   PM_BTM_MEAS_MODE_CONTINUOUS
}pm_btm_meas_mode_type;

/*! \enum pm_btm_mux_type
   \brief The pre-mux selection.
 */
typedef enum
{
   PM_BTM_AMUX_MAIN,
   PM_BTM_PREMUX_TO_CH6,
   PM_BTM_PREMUX_TO_CH7,
   PM_BTM_RSV_DISABLED
} pm_btm_mux_type; 

/*! \enum pm_btm_xoadc_input_select_type
   \brief The XOADC input.
 */
typedef enum
{
   PM_BTM_XOADC_INPUT_XO_IN_XOADC_GND,
   PM_BTM_XOADC_INPUT_PMIC_IN_XOADC_GND,
   PM_BTM_XOADC_INPUT_PMIC_IN_BMS_CSP,
   PM_BTM_XOADC_INPUT_RESERVED,
   PM_BTM_XOADC_INPUT_XOADC_GND_XOADC_GND,
   PM_BTM_XOADC_INPUT_XOADC_VDD_XOADC_GND,
   PM_BTM_XOADC_INPUT_VREFN_VREFN,
   PM_BTM_XOADC_INPUT_VREFP_VREFN
} pm_btm_xoadc_input_select_type;

/*! \enum pm_btm_xoadc_decimation_ratio_type
   \brief The XOADC decimation ratio.
 */
typedef enum
{
  PM_BTM_XOADC_DECIMATION_RATIO_512,
  PM_BTM_XOADC_DECIMATION_RATIO_1K,
  PM_BTM_XOADC_DECIMATION_RATIO_2K,
  PM_BTM_XOADC_DECIMATION_RATIO_4K,
} pm_btm_xoadc_decimation_ratio_type;

/*! \enum pm_btm_xoadc_conversion_rate_type
   \brief The XOADC convertion rate.
 */
typedef enum
{
  PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_8,
  PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_4,
  PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_2,
  PM_BTM_XOADC_CONVERSION_RATE_TCXO
} pm_btm_xoadc_conversion_rate_type;


/*===========================================================================

                 BTM DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @brief This function enables/disable the BTM module.
 * 
 * @details
 *  This function enables/disables the BTM timing interval battery tempeature
 *  measurements when the ADC aribiter module is enabled.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] cmd This parameter is used to enable or disasble the BTM module. 
 *           -- PM_BTM_CMD__ENABLE    Enable BTM module;
 *           -- PM_BTM_CMD__DISABLE   Disable BTM module.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED   = The internal resource ID for  
 *         the BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_mode(int externalResourceIndex, 
                                 pm_btm_mode_type mode); 

/**
 * @brief This function sets the conversion request.
 * 
 * @details
 *  This function sets the conversion request. After the request was sent, the
 *  ADC will start the battery tempeature measurement; the request is cleared
 *  when single measurement mode is selected and after the ADC conversion is 
 *  completed.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] cmd This parameter is used to request a ADC conversion. 
 *    -- PM_BTM_CMD__ENABLE    Request a conversion; the XOADC will be enabled.
 *    -- PM_BTM_CMD__DISABLE   Don't request a conversion; the XOADC will be 
 *                             disabled.
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED  = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_conversion_request(int externalResourceIndex, 
                                               pm_btm_conv_req_cmd_type cmd);

/**
 * @brief This function gets the conversion status.
 * 
 * @details
 *  This function gets the conversion status based on the status of the  
 *  conversion status strobe and the end-of-conversion status flag. 
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[status] The status of the conversion request. 
 *       -- PM_BTM_CONVERSION_STATUS__INVALID   -- Invalid status 
 *       -- PM_BTM_CONVERSION_STATUS__COMPLETE  -- Conversion completed 
 *       -- PM_BTM_CONVERSION_STATUS__OCCURRING -- Conversion in progress 
 *       -- PM_BTM_CONVERSION_STATUS__WAITING   -- Waiting for ADC to complete
 *          another process's conversion request or interval timer to expire. 
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED   = The internal resource ID for  
 *         the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER       = Invalid pointer passed in.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_btm_get_conversion_status(int externalResourceIndex, 
                            pm_btm_xoadc_conversion_status_type *status);

/**
 * @brief This function sets the measurement mode.
 * 
 * @details
 *  This function sets the battery temp measurement mode.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] mode This parameter is used to set the operating mode of the 
 *            measurement. 
 *    -- PM_XOADC_BTM_MEAS_MODE_SINGLE    Measure battery temp after a single 
 *                                        measurement interval time;
 *    -- PM_XOADC_BTM_MEAS_MODE_CONTINUOUS   Continously measure battery temp 
 *                                           at measurement interval times.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_measurement_mode(int externalResourceIndex, 
                                             pm_btm_meas_mode_type mode);

/**
 * @brief This function sets the measurement interval.
 * 
 * @details
 *  This function sets the battery temp measurement interval time.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] second The interval time
 * @note if the value of parameter second is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds second value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_measurement_interval(int externalResourceIndex, 
                                                 uint32 second);

/**
 * @brief This function sets the measurement interval.
 * 
 * @details
 *  This function sets the battery temp measurement interval time.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] channel The analog channel.
 *     
 * @param[in] mux The MUX.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE   = Input Parameter two is out of range.
 *         PM_ERR_FLAG__PAR3_OUT_OF_RANGE   = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS             = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_amux(int externalResourceIndex, 
                                 uint8 channel, 
                                 pm_btm_mux_type mux);

/**
 * @brief This function gets the prescaler value for the specific channel.
 * 
 * @details
 *  This function sets the battery temp measurement interval time.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] channel The analog channel.
 *     
 * @param[out] numerator The numerator of the prescalar.
 *     
 * @param[out] denominator The denominator of the prescalar.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__INVALID_POINTER   = Invalud input pointer
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_btm_get_prescalar(int externalResourceIndex, 
                                      uint8 channel, 
                                      uint8 *numerator, 
                                      uint8 *denominator);

/**
 * @brief This function sets the XOADX input.
 * 
 * @details
 *  This function sets the XOADX input that is used for BTM.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] input This parameter is used to select XOADC input; 
 *                  -- PM_BTM_XOADC_INPUT_PMIC  -- PMIC_IN
 *                  -- PM_BTM_XOADC_INPUT_XO    -- XO_IN
 *                  -- PM_BTM_XOADC_INPUT_VREFP -- vrefp
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_xoadc_input(int externalResourceIndex, 
                                        pm_btm_xoadc_input_select_type input);

/**
 * @brief This function sets the XOADX decimation ratio.
 * 
 * @details
 *  This function sets the XOADX decimation ratio that is used for BTM.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] ratio This parameter is used to select XOADC decimation ratio; 
 *                  -- PM_BTM_XOADC_DECIMATION_RATIO_512   -- 512
 *                  -- PM_BTM_XOADC_DECIMATION_RATIO_1K    -- 1K
 *                  -- PM_BTM_XOADC_DECIMATION_RATIO_2K    -- 2K
 *                  -- PM_BTM_XOADC_DECIMATION_RATIO_4K    -- 4K
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_xoadc_decimation_ratio(int externalResourceIndex, 
                                    pm_btm_xoadc_decimation_ratio_type ratio);

/**
 * @brief This function sets the XOADX clock rate.
 * 
 * @details
 *  This function sets the XOADX clock rate that is used for BTM.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] rate This parameter is used to select XOADC clock rate; 
 *                  -- PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_8    -- TCXO/8
 *                  -- PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_4    -- TCXO/4
 *                  -- PM_BTM_XOADC_CONVERSION_RATE_TCXO_DIV_2    -- TCXO/2
 *                  -- PM_BTM_XOADC_CONVERSION_RATE_TCXO          -- TCXO/1
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_set_xoadc_conversion_rate(int externalResourceIndex, 
                                    pm_btm_xoadc_conversion_rate_type rate);

/**
 * @brief This function gets the XOADC conversion result.
 * 
 * @details
 *  This function sets the XOADX cconversion result.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[out] data This parameter is used to store the XOADC conversion result
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER     = Invalud input pointer
 *         PM_ERR_FLAG__SUCCESS             = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 *
 */
pm_err_flag_type pm_btm_get_xoadc_data(int externalResourceIndex, 
                                       uint32* data);

/**
 * @brief This function sets the battery warm-temp threshold.
 * 
 * @details
 *  This function sets the threshold for battery to be considered as warm.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] warm_temp_thresh This parameter is used to set the warm battery 
 *            temp threshold.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 */
pm_err_flag_type pm_btm_set_batt_warm_threshold(int externalResourceIndex, 
                                                uint32 warm_temp_thresh);

/**
 * @brief This function sets the battery cool-temp threshold.
 * 
 * @details
 *  This function sets the threshold for battery to be considered as cool.
 *   
 * @param[in] externalResourceIndex The external BTM ID.
 *
 * @param[in] warm_temp_thresh This parameter is used to set the cool battery
 *            temp threshold.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the BTM module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the BTM module is not correct.
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED   = The internal resource ID for the 
 *         BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device spefic. 
 */
pm_err_flag_type pm_btm_set_batt_cool_threshold(int externalResourceIndex, 
                                                uint32 cool_temp_thresh);

#endif /* PM_BTM_H */

