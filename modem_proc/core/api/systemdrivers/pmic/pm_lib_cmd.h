#ifndef PM_LIB_CMD_H
#define PM_LIB_CMD_H

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
 * @file pm_lib_cmd.h PMIC Library command parameters declaration.
 *
 * @brief This file is defines commands used in PMIC library APIs.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_lib_cmd.h#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_switch_cmd_type
 * @brief Use these commands to enable or disable the different PMIC devices
 */
typedef enum
{
   PM_OFF_CMD,    /*!< Command to turn off */
   PM_ON_CMD,    /*!< Command to turn on */
   PM_INVALID_CMD    /*!< Invalid selection, used for range checking in PMIC APIs */
}pm_switch_cmd_type;

#endif    /* PM_LIB_CMD_H */

