#ifndef PM_H
#define PM_H

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
 * @file pm.h PMIC related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *        the PMIC module.
 */

/*===========================================================================

                  P M    H E A D E R    F I L E

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/28/10   wra     Redirecting to main include pm_qc_pmic.h to avoid
                   HLOS file naming conflicts
                   - Also purged comments older than 2008
06/07/10   jtn     Add pmapp_vreg_pincntrl_vote.h 
05/06/10   jtn     Add pmapp_smps_mode_vote.h
02/10/10   jtn     Add header files for voltage scaling
01/21/10   jtn     Add pmapp_clk_vote.h
12/02/09   jtn     Add pmapp_pcil.h
07/29/09   jtn     Add pmapp_hs.h
03/24/09   jtn     SCMM integration
01/31/09   jtn     Added #include "pm_xoadc.h"
10/21/08   aab     Added #include "pmapp_usb.h"
09/30/08   jtn     Added #include "pmapp_hsed.h"
08/01/08   jtn     Added #include "pmapp_pcil.h"
06/26/08   jtn     Merged from Kip branch
02/27/08   jtn     Added PMIC_IS_KIP to list of PMICs
01/07/08   jtn     Merged from QSC6270 branch
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_qc_pmic.h"

#endif    /* PM_H */

