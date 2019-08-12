#ifndef PM_UICC_H
#define PM_UICC_H

/**
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_uicc.h PMIC UICC Feature related declaration.
 *
 * @brief This file contains functions prototypes and variable/type/constant
 *        declarations to support the HOTSWAP (UICC) feature inside the Qualcomm
 *        PMIC chips.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_uicc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
02/24/11   dy      Add API to set hotswap polarity
09/14/11   dy      Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_bua.h"


/*===========================================================================

                    UICC DRIVER FUNCTION PROTOTYPE

===========================================================================*/
/** 
 * @name pm_uicc_cntrl_enable 
 *  
 * @brief This function is used to enable/disable the UICC Hot-swap module.
 *
 * @param [in] alarm PM_BUA_BATT_ALARM for BATTERY and PM_BUA_UICCx_ALARM for
 *                   the corresponding UIM slots.
 * @param [in] enable TRUE to enable
 *					  FALSE to disable
 *
 * @return pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Failure in enabling/disabling
 *         the UICC feature or feature not supported.
 *         PM_ERR_FLAG__SUCCESS = Success.
 */
pm_err_flag_type pm_uicc_cntrl_enable(pm_bua_alarm_type alarm, boolean enable);

#endif    /* PM_UICC_H */
