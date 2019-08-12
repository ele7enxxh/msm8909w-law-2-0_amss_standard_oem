/*! \file 
 *  \n
 *  \brief  pm_itemp.h ---- PMIC internal temperature driver 
 *  \n
 *  \n This file is defines error codes returned by PMIC internal temperature driver 
 *  \n
 *  \n &copy; Copyright 2003-2010 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_itemp.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/09   dy      New API to configure overtemp threshold
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/

#ifndef PM_ITEMP_H
#define PM_ITEMP_H

/**
 * \enum pm_item_stage_type
 * TEMPERATURE PROTECTION type definitions 
 */
typedef enum
{
    PM_ITEMP_STAGE0,
    PM_ITEMP_STAGE1,
    PM_ITEMP_STAGE2,
    PM_ITEMP_STAGE3
}pm_item_stage_type;

/**
 * \enum pm_item_oride_type
 * TEMPERATURE PROTECTION type definitions 
 */
typedef enum
{
    PM_ITEMP_ORIDE_STAGE2,
    PM_ITEMP_ORIDE_OUT_OF_RANGE
}pm_item_oride_type;

/**
 * \enum pm_itemp_threshold_type
 * TEMPERATURE PROTECTION threshold type definitions 
 */
typedef enum
{
    PM_TEMP_THRESH_CTRL0,
    PM_TEMP_THRESH_CTRL1,
    PM_TEMP_THRESH_CTRL2,
    PM_TEMP_THRESH_CTRL3,
    PM_INVALID_INPUT
} pm_itemp_threshold_type;



/*===========================================================================

                 TEMPERATURE PROTECTION FUNCTION DEFINITIONS

===========================================================================*/
/**
 * \brief     This function returns the current over temperature protection stage
 * of the PMIC.
 *
 * \details This function returns the current over temperature protection stage
 * of the PMIC.  Note that a change of stage can be detected by enabling or pulling
 *  the Temperature Status Changed IRQ (refer to PM_T_STAT_CHANGED_IRQ_HDL
 *  and PM_T_STAT_CHANGED_RT_ST).
 *   Temperature Protection Stages:
 *       - STAGE0:
 *           - Temperature Range = T < 110C.
 *           - Normal Condition.
 *       - STAGE1:
 *           - Temperature Range = 110C > T < 130C.
 *           - Over Temperature Warning. No action.
 *       - STAGE2:
 *           - Temperature Range = 130C > T < 150C.
 *           - Shuts down high current drivers such as speaker and LED
 *             drivers automatically. Can be override through
 *             "pm_itemp_stage_override()".
 *       - STAGE3:
 *           - Temperature Range = T > 150C.
 *           - Powers down the PMIC.
 * 
 * \param[out] *itemp_stage Use this pointer to collect the current over temperature stage
 *     of the PMIC.
 *   - Type: pm_item_stage_type.
 *   - Valid Outputs:
 *       - PM_ITEMP_STAGE0
 *       - PM_ITEMP_STAGE1
 *       - PM_ITEMP_STAGE2
 *       - PM_ITEMP_STAGE3
 *
 * \return   Return value type: pm_err_flag_type.
 *   - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                        version of the PMIC. 
 *   - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
 *                                        with the PMIC.
 *   - PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *  
 * \sideeffects   Interrupts are disable while communicating with the PMIC.
*/
pm_err_flag_type pm_dev_itemp_get_stage(unsigned pmic_device_index, pm_item_stage_type *itemp_stage );
//#pragma message("Do not use pm_itemp_get_stage(), which is deprecated. Use pm_dev_itemp_get_stage() instead.")
pm_err_flag_type pm_itemp_get_stage( pm_item_stage_type *itemp_stage );

/**
 * \brief     This function overrides the automatic shut-down of the temperature
 * protection stage.
 *
 * \param  oride_cmd Enable/Disable automatic shut-down.
 *   - Type: pm_switch_cmd_type.
 *   - Valid Inputs:
 *       - PM_OFF_CMD (default) = Do not override automatic
 *       shut-down.
 *       - PM_ON_CMD            = Override automatic shut-down.
 *
 * \param oride_stage Which stage we want to configure.
 *    - Type: pm_switch_cmd_type.
 *    - Valid Inputs:
 *       - PM_ITEMP_ORIDE_STAGE2: Only stage 2 can be override.
 *
 * \return   Return value type: pm_err_flag_type.
 *   - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two
 *    is out of range.
 *   - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one
 *    is out of range.
 *   - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *                                        on this version of the
 *                                        PMIC.
 *   - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed
 *                                        to communicate with the
 *                                        PMIC.
 *   - PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 * \sideeffects   Interrupts are disable while communicating with the PMIC.
*/
pm_err_flag_type pm_dev_itemp_stage_override(unsigned pmic_device_index, pm_switch_cmd_type oride_cmd,
                                         pm_item_oride_type oride_stage);
//#pragma message("Do not use pm_itemp_stage_override(), which is deprecated. Use pm_dev_itemp_stage_override() instead.")
pm_err_flag_type pm_itemp_stage_override(   pm_switch_cmd_type oride_cmd,
                                         pm_item_oride_type oride_stage);

/**
 * \brief     This function controls the temperature threshold
 * 
 * \param  oride_cmd Enable/Disable automatic shut-down.
 *   - Type: pm_switch_cmd_type.
 *   - Valid Inputs:
 *       - PM_OFF_CMD (default) = Do not override automatic
 *       shut-down.
 *       - PM_ON_CMD            = Override automatic shut-down.
 * 
 * \param thresh_value Which stage we want to configure.
 *    - Type: pm_itemp_threshold_type
 *      - PM_TEMP_THRESH_CTRL0           = {105, 125, 145}
 *      - PM_TEMP_THRESH_CTRL1 (default) = {110, 130, 150}
 *      - PM_TEMP_THRESH_CTRL2           = {115, 135, 155}
 *      - PM_TEMP_THRESH_CTRL3           = {120, 140, 160}
 * 
 * 
 * \return   Return value type: pm_err_flag_type.
 *   - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one
 *    is out of range.
 *   - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available
 *                                        on this version of the
 *                                        PMIC.
 *   - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed
 *                                        to communicate with the
 *                                        PMIC.
 *   - PM_ERR_FLAG__SUCCESS               = SUCCESS.
 * 
 * \sideeffects   Interrupts are disable while communicating with the PMIC.
 * 
 */
pm_err_flag_type pm_dev_itemp_thresh_cntrl(unsigned pmic_device_index, pm_itemp_threshold_type  thresh_value );
//#pragma message("Do not use pm_itemp_thresh_cntrl(), which is deprecated. Use pm_dev_itemp_thresh_cntrl() instead.")
pm_err_flag_type pm_itemp_thresh_cntrl( pm_itemp_threshold_type  thresh_value );
                                            


#endif

