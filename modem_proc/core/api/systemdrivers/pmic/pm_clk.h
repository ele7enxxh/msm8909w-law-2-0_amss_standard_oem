#ifndef PM_CLK_H
#define PM_CLK_H

/** @file pm_clk.h
*
* PMIC-MEGA XO CLK BUFFER RELATED DECLARATION
*
*  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC Clock module. 
*/
/*
*   Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
*   All Rights Reserved.
*   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_clk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/12   hs      Initial version.
========================================================================== */
#include "comdef.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/

/** @addtogroup pm_clk
@{ */

// \enum CLK resources available on PMIC component
 /** Definitions of the CLK resources in the target.
 */
typedef enum
{
    PM_RF_CLK1,       /**< RF Clock 1. */
    PM_RF_CLK2,       /**< RF Clock 2. */
    PM_RF_CLK3,       /**< RF Clock 3. */
    PM_SLEEP_CLK1,    /**< Sleep Clock 1. */ 
    PM_CLK_XO,        /**< XO Core clock. */
    PM_CLK_BB_1,      /**< BB Clock 1. */ 
    PM_CLK_BB_2,      /**< BB Clock 2. */
    PM_CLK_DIFF_1,    /**< DIFF Clock 1. */
    PM_CLK_DIV_1,     /**< DIV Clock 1. */
    PM_CLK_DIV_2,     /**< DIV Clock 2. */
    PM_CLK_DIV_3,     /**< DIV Clock 3. */
    PM_CLK_DIST,      /**< DIST Clock. */
    PM_CLK_LN_BB,     /**< LN_BB Clock. */
    PM_ALL_CLKS,      /**< All clocks. */ 
    PM_CLK_INVALID    /**< Invalid clock. */
}pm_clk_type;

/** Clock buffer output drive strength. */
typedef enum
{
    PM_CLK_BUFF_OUT_DRV__1X, /**< Output drive strength is normal. */
    PM_CLK_BUFF_OUT_DRV__2X, /**< Output drive strength is double. */
    PM_CLK_BUFF_OUT_DRV__3X, /**< Output drive strength is triple. */
    PM_CLK_BUFF_OUT_DRV__4X, /**< Output drive strength is quadruple. */
    PM_CLK_DRV_STRENGTH_INVALID /**< Output drive strength is invalid. */
}pm_clk_drv_strength_type;

#define pm_clk_buff_output_drive_strength_type     pm_clk_drv_strength_type

typedef struct
{
    pm_clk_drv_strength_type clk_drv_strength; 
    pm_on_off_type           clk_ok;
    pm_on_off_type           clk_sw_enable;
    pm_on_off_type           clk_pin_ctrled;
    pm_on_off_type           clk_pull_down;
    uint32                   clk_out_div;
}pm_clk_status_type;

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
                pm_dev_clk_buff_set_output_drive_strength
===========================================================================*/
/**
  Sets the output buffer drive strength.

 @param[in] pmic_chip   Selects the device in which the buffers being
                        controlled are located. Device index starts with
                        zero.
  @param[in] periph Clock type. See #pm_clk_type
 @param[in] drive_strength Output buffer drive strength setting. See
                          #pm_clk_drv_strength_type.

 @return
  SUCCESS or Error -- See #pm_err_flag_type.

 @dependencies
  None

 @sideeffects
 Interrupts are disabled while communicating with the PMIC.
*/
/*==========================================================================*/

pm_err_flag_type pm_dev_clk_buff_set_output_drive_strength(uint8 pmic_chip,
                                                           pm_clk_type periph,
                                                           pm_clk_drv_strength_type drive_strength);
pm_err_flag_type pm_clk_drv_strength(uint8 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type drive_strength );

/*===========================================================================
                pm_clk_sleep_src_status
===========================================================================*/
/**
Returns on/off (enable/disable) status of the sleep
        clock (external XTAL Oscillator).
 @param pmic_chip Selects the device in which the buffers
                   being controlled are located. Device index
                   starts with zero.
 @param clk_type Selects which clock type to use. See #pm_clk_type.
 @param on_off Returns on/off status of the sleep clock: \n
               - PM_ON = ON
               - PM_OFF = OFF @tablebulletend

 @note Usage Example:
            pm_clk_sleep_source_status(0, PM_CLK_SLEEP, *on_off);

 @return
 PM_ERR_FLAG__SUCCESS if successful. See #pm_err_flag_type
*/
pm_err_flag_type pm_clk_sleep_src_status(uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type* on_off);

/*===========================================================================
                pm_clk_get_status
===========================================================================*/
/** 
*  
* @brief Reads the status of PMIC clock. 
* 
* @param pmic_chip: Selects the device in which the buffers being 
*                   controlled are located. Device index starts
*                   with zero
* @param clk        : Selects which clock is being targeted by the API. Refer enum #pm_clk_type for more info
* @param clk_status : Gets clock status. Refer struct #pm_clk_status_type for more info
*                         
*  
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful 
*/ 
pm_err_flag_type pm_clk_get_status(uint8 pmic_chip, pm_clk_type clk, pm_clk_status_type *clk_status);

/** @} */ /* end_addtogroup pm_clk */

#endif /* PM_CLK_H */
