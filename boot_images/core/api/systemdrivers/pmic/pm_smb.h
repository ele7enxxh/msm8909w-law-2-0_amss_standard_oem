#ifndef PM_SMB_H
#define PM_SMB_H

/**
 * Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
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
 * @file pm_smb.h  External charger SMB Module related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *   the SMB module.
 */

/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/14   pxm     Implement SMB driver
===========================================================================*/

#include "I2cError.h"
#include "pm_err_flags.h"

typedef enum 
{
    IL_USB100,
    IL_USB500,
    IL_AC,
} InputLimitMode;

/*
 * Initialization SMB charger chip to ensure 500ma input current. 
 *
 * @enable: Whether to enable charging function in SMB chip
 * @return PM_ERR_FLAG__I2C_OPT_ERR if I2C error happened
 *         PM_ERR_FLAG__SUCCESS if success
 */
//pm_err_flag_type pm_smb_enable_charging(boolean enable);

pm_err_flag_type pm_smb_enable_volatile_access(void);

pm_err_flag_type pm_smb_enable_aicl(boolean enable);

pm_err_flag_type pm_smb_enable_charging(boolean enable);

pm_err_flag_type pm_smb_set_icl_mode(InputLimitMode mode);

pm_err_flag_type pm_smb_set_icl(uint32 current);

pm_err_flag_type pm_smb_set_ibat_fast(uint32 current);

pm_err_flag_type pm_smb_init(void);

pm_err_flag_type pm_smb_deinit(void);

#endif /*PM_SMB_H*/

