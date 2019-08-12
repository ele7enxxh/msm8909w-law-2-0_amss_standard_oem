#ifndef PM_ARBITER_XOADC_H
#define PM_ARBITER_XOADC_H

/*! \file
*  \n
*  \brief  pm_arbiter_xoadc.h PMIC-ADC ARBITER MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC ARB_ADC (ADC Arbiter) module. 
*  \n &copy; Copyright 2011 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_arbiter_xoadc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/11   hs      Initial version. 
========================================================================== */
#include "comdef.h"
#include "pm_lib_err.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_adc_mode_type
   \brief The ADC mode
 */
typedef enum
{
   /*! ADC module OFF  */
   PM_ADC_MODE__MODULE_OFF,
    /*! ADC module ON  */
   PM_ADC_MODE__MODULE_ON
}pm_adc_mode_type;
/*! \enum pm_adc_conv_req_cmd_type
   \brief Use these commands to set or clear the conversion request in the
 *  ADC module.
 */
typedef enum
{
    /*! Command to stop a conversion.  */
   PM_ADC_CONVERSION_REQ_CMD__OFF,
    /*! Command to request a conversion.  */
   PM_ADC_CONVERSION_REQ_CMD__ON
}pm_adc_conv_req_cmd_type;
/*! \enum pm_adc_cmd_type
   \brief Use these commands to enable or disable certain functionalities in
 *  the ADC module.
 */
typedef enum
{
    /*! Disable the functionality.  */
   PM_ADC_CMD__DISABLE,
    /*! Enable the functionality.   */
   PM_ADC_CMD__ENABLE
}pm_adc_cmd_type;

/*! \enum pm_adc_conversion_status_type
   \brief The conversion status.
 */
typedef enum
{
    /*! Conversion complete.  */
    PM_ADC_CONVERSION_STATUS__COMPLETE,
    /*! Waiting for ADC to complete another process's conversion.  */
    PM_ADC_CONVERSION_STATUS__PENDING
} pm_adc_conversion_status_type;

/*! \enum pm_adc_conv_trig_type
   \brief Use these commands to set the conversion trigger condition that 
 *  starts ADC hold off timer.
 */
typedef enum
{
    /*! Failing Edge.  */
   PM_ADC_CONV_TRIG__FE,
    /*! Rising Edge.   */
   PM_ADC_CONV_TRIG__RE
}pm_adc_conv_trig_type;

/*! \enum pm_adc_mux_type
   \brief The pre-mux selection for ADC.
 */
typedef enum
{
   PM_ADC_AMUX_MAIN,
   PM_ADC_PREMUX_TO_CH6,
   PM_ADC_PREMUX_TO_CH7,
   PM_ADC_RSV_DISABLED
} pm_adc_mux_type; 

/*! \enum pm_adc_xoadc_input_select_type
   \brief The XOADC input.
 */
typedef enum
{
   PM_ADC_XOADC_INPUT_XO_IN_XOADC_GND,
   PM_ADC_XOADC_INPUT_PMIC_IN_XOADC_GND,
   PM_ADC_XOADC_INPUT_PMIC_IN_BMS_CSP,
   PM_ADC_XOADC_INPUT_RESERVED,
   PM_ADC_XOADC_INPUT_XOADC_GND_XOADC_GND,
   PM_ADC_XOADC_INPUT_XOADC_VDD_XOADC_GND,
   PM_ADC_XOADC_INPUT_VREFN_VREFN,
   PM_ADC_XOADC_INPUT_VREFP_VREFN
} pm_adc_xoadc_input_select_type;

/*! \enum pm_adc_xoadc_decimation_ratio_type
   \brief The XOADC decimation ratio.
 */
typedef enum
{
  PM_ADC_XOADC_DECIMATION_RATIO_512,
  PM_ADC_XOADC_DECIMATION_RATIO_1K,
  PM_ADC_XOADC_DECIMATION_RATIO_2K,
  PM_ADC_XOADC_DECIMATION_RATIO_4K,
} pm_adc_xoadc_decimation_ratio_type;

/*! \enum pm_adc_xoadc_conversion_rate_type
   \brief The XOADC convertion rate.
 */
typedef enum
{
  PM_ADC_XOADC_CONVERSION_RATE_TCXO_DIV_8,
  PM_ADC_XOADC_CONVERSION_RATE_TCXO_DIV_4,
  PM_ADC_XOADC_CONVERSION_RATE_TCXO_DIV_2,
  PM_ADC_XOADC_CONVERSION_RATE_TCXO
} pm_adc_xoadc_conversion_rate_type;


/*! \struct pm_adc_conv_seq_status_type
   \brief Use this structure to get the XOADC conversion sequencer status.
 */
typedef struct
{
    /*! Timeout flag.  */
   boolean   timeout;  
    /*! FIFO not empty flag   */
   boolean   fifo_not_empty;
}pm_adc_conv_seq_status_type;

/*===========================================================================

                 ARBITER-ADC DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @brief This function enables/disable the ADC Arbiter.
 * 
 * @details
 *  This function enables/disables the ADC Arbiter.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] cmd This parameter is used to enable or disasble the ADC Arbiter. 
 *           -- PM_ADC_MODE__MODULE_OFF    Disable ADC Arbiter.
 *           -- PM_ADC_MODE__MODULE_ON     Enable ADC Arbiter.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID  
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of 
 *         range.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_adc_set_mode(int externalResourceIndex, 
                                 pm_adc_mode_type mode);

/**
 * @brief This function sets the ADC conversion request.
 * 
 * @details
 *  This function sets the ADC conversion request. After the request was sent,  
 *  the request will be stored in the convversion request queue in the PMIC HW; 
 *  the request is cleared when the ADC conversion is completed.
 *   
 * @param[in] externalResourceIndex The external ADC ID.
 *
 * @param[in] cmd This parameter is used to set the conversion request. 
 *         -- PM_ADC_CONVERSION_REQ_CMD__OFF  Clear the ADC conversion request.
 *         -- PM_ADC_CONVERSION_REQ_CMD__ON   Set the ADC conversion request.
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED  = The internal resource ID for 
 *         the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 */
pm_err_flag_type pm_adc_set_conversion_request(int externalResourceIndex, 
                                               pm_adc_conv_req_cmd_type cmd);

/**
 * @brief This function gets the ADC conversion status.
 * 
 * @details
 *  This function gets the ADC conversion status based on the status of the  
 *  conversion status strobe and the end-of-conversion status flag. 
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[out] status The status of the conversion request. 
 *           -- PM_ADC_CONVERSION_STATUS__COMPLETE      -- Conversion completed 
 *           -- PM_ARB_ADC_CONVERSION_STATUS__PENDING   -- Waiting for ADC to 
 *              complete another process's conversion request or interval timer 
 *              to expire. 
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID   
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER       = Invalid pointer passed in.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_adc_get_conversion_status(int externalResourceIndex, 
                                  pm_adc_conversion_status_type *status);

/**
 * @brief This function enables/disable the conversion sequencer.
 * 
 * @details
 *  This function enables/disables the conversion sequencer.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] cmd This parameter is used to enable or disasble the conversion
 *            sequencer. 
 *           -- PM_ARB_ADC_CMD__ENABLE    Enable conversion sequencer.
 *           -- PM_ARB_ADC_CMD__DISABLE   Disable conversion sequencer.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID 
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 * @note This API is available only to modem processor.
 *
 */
pm_err_flag_type pm_adc_set_conv_sequencer(int externalResourceIndex, 
                                           pm_adc_cmd_type cmd);

/**
 * @brief This function sets the conversion trigger condition.
 * 
 * @details
 *  This function sets the conversion trigger condition that starts ADC 
 *  holdoff timer.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] trig This parameter is used to set the conversion trigger 
 *            condition. 
 *           -- PM_ADC_CONV_TRIG__FE    Failing edge.
 *           -- PM_ADC_CONV_TRIG__RE    Rising edge
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID  
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 * @note This API is available only to modem processor.
 */
pm_err_flag_type pm_adc_set_conv_trig_condition(int externalResourceIndex, 
                                                pm_adc_conv_trig_type trig);

/**
 * @brief This function sets the holdoff time.
 * 
 * @details
 *  This function sets the holdoff time which is the delay from conversion 
 *  trigger transition  to ADC enable.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] time_us The holdoff time in micro seconds. 
 * @note if the value of parameter time_us is out of the valid range that the
 *       PMIC HW can take,  the value will be limited to the boundary and the
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds time value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID  
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 * @note This API is available only to modem processor.
 */
pm_err_flag_type pm_adc_set_holdoff_time(int externalResourceIndex, 
                                         uint32 time_us);

/**
 * @brief This function sets the timeout time.
 * 
 * @details
 *  This function sets the timeout time which is the delay from SBI conversion 
 *  request to triggering conversion sequencer hold off timer.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] time_ms The timeout time in milli seconds. 
 * @note if the value of parameter time_ms is out of the valid range that the 
 *       PMIC HW can take, the value will be limited to the boundary and the 
 *       PMIC HW will be set to that value; at the same time, 
 *       PM_ERR_FLAG__PAR2_OUT_OF_RANGE will be returned to indicate that there 
 *       was an out-of-bounds time value parameter input.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID 
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 * @note This API is available only to modem processor.
 */
pm_err_flag_type pm_adc_set_timeout_time(int externalResourceIndex, 
                                         uint32 time_ms);

/**
 * @brief This function gets the status of conversion sequencer.
 * 
 * @details
 *  This function gets the status of conversion sequencer, including the SBI 
 *  conversion request timeout flag and status of FIFO. 
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[out] status The status of the conversion request. 
 *          -- PM_ARB_ADC_CONVERSION_STATUS__INVALID   -- Invalid status 
 *          -- PM_ARB_ADC_CONVERSION_STATUS__COMPLETE  -- Conversion completed 
 *          -- PM_ARB_ADC_CONVERSION_STATUS__OCCURRING -- Conversion in progress 
 *          -- PM_ARB_ADC_CONVERSION_STATUS__WAITING   -- Waiting for ADC to 
 *             complete another process's conversion request or interval timer 
 *             to expire. 
 *
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID
 *          for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER       = Invalid pointer passed in.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 * @note This API is available only to modem processor.
 */
pm_err_flag_type pm_adc_get_conv_sequencer_status(int externalResourceIndex, 
                                        pm_adc_conv_seq_status_type *status);

/**
 * @brief This function sets up the AMUX.
 * 
 * @details
 *  This function sets up the AMUX, such as channel and pre-mux selection.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] channel The analog channel.
 *     
 * @param[in] mux The MUX.
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID 
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__PAR3_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_adc_set_amux(int externalResourceIndex, 
                                 uint8 channel, 
                                 pm_adc_mux_type mux);

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
 *         PM_ERR_FLAG__INVALID_BTM_INDEXED   = The internal resource ID  
 *         for the BTM module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__INVALID_POINTER   = Invalud input pointer
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_adc_get_resource_prescalar(int externalResourceIndex, 
                                               uint8 channel, 
                                               uint8 *numerator, 
                                               uint8 *denominator);

/**
 * @brief This function sets the ADX input.
 * 
 * @details
 *  This function sets the XOADC input that is used for ARB_ADC.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] input This parameter is used to select ADC input; 
 *                  -- PM_ARB_ADC_ADC_INPUT_PMIC  -- PMIC_IN
 *                  -- PM_ARB_ADC_ADC_INPUT_XO    -- XO_IN
 *                  -- PM_ARB_ADC_ADC_INPUT_VREFP -- vrefp
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID 
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_adc_set_xoadc_input(int externalResourceIndex, 
                                        pm_adc_xoadc_input_select_type input);

/**
 * @brief This function sets the XOADC decimation ratio.
 * 
 * @details
 *  This function sets the XOADC decimation ratio that is used for ARB_ADC.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] ratio This parameter is used to select ADC decimation ratio; 
 *                  -- PM_ARB_ADC_ADC_DECIMATION_RATIO_512   -- 512
 *                  -- PM_ARB_ADC_ADC_DECIMATION_RATIO_1K    -- 1K
 *                  -- PM_ARB_ADC_ADC_DECIMATION_RATIO_2K    -- 2K
 *                  -- PM_ARB_ADC_ADC_DECIMATION_RATIO_4K    -- 4K
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID 
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is PM_ARB_ADC_CMD__DISABLE. 
 *
 */
pm_err_flag_type pm_adc_set_xoadc_decimation_ratio(int externalResourceIndex, 
                                    pm_adc_xoadc_decimation_ratio_type ratio);

/**
 * @brief This function sets the XOADC clock rate.
 * 
 * @details
 *  This function sets the XOADC clock rate that is used for ARB_ADC.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[in] rate This parameter is used to select ADC clock rate; 
 *                  -- PM_ARB_ADC_ADC_CONVERSION_RATE_TCXO_DIV_8    -- TCXO/8
 *                  -- PM_ARB_ADC_ADC_CONVERSION_RATE_TCXO_DIV_4    -- TCXO/4
 *                  -- PM_ARB_ADC_ADC_CONVERSION_RATE_TCXO_DIV_2    -- TCXO/2
 *                  -- PM_ARB_ADC_ADC_CONVERSION_RATE_TCXO          -- TCXO/1
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID  
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Input Parameter two is out of range.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 * @note The default setting in PMIC HW is device specific. 
 *
 */
pm_err_flag_type pm_adc_set_xoadc_conversion_rate(int externalResourceIndex, 
                                    pm_adc_xoadc_conversion_rate_type rate);

/**
 * @brief This function gets the XOADC conversion data.
 * 
 * @details
 *  This function gets the XOADC conversion data.
 *   
 * @param[in] externalResourceIndex The external ARB_ADC ID.
 *
 * @param[out] data This parameter is used to store the ADC conversion result
 *     
 * @return     pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE   = 
 *         The current processor doesn't have the access to the ARB_ADC module.
 *         PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED   = 
 *         The external resource ID for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_ARB_ADC_INDEXED   = The internal resource ID  
 *         for the ARB_ADC module is not correct.
 *         PM_ERR_FLAG__INVALID_POINTER     = Invalid input pointer
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_adc_get_xoadc_data(int externalResourceIndex,
                                       uint32* data);

#endif /* PM_ARBITER_XOADC_H */

