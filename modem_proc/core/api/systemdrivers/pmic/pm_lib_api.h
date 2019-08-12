#ifndef PM_LIB_API_H
#define PM_LIB_API_H

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
 * @file pm_lib_api.h PMIC Library API header file.
 *
 * @brief This file is a master include file for PMIC library APIs.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_lib_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
08/26/13   rk      removing unused header files.
01/10/13   kt      Removed pm_vreg.h and included pm_smps.h and pm_ldo.h.
05/01/12   hs      Removed pm_vbatt.h.
04/16/12   hs      Removed irq files.
03/16/12   hs      Removed obsolete files.
03/14/12   hs      Replaced pm_mega_xo.h and pm_clocks.h with pm_xo_core.h
                   and pm_clk_buff.h
11/29/11   hs      Added pm_mega_xo.h
09/14/11   dy      Added pm_uicc.h
04/28/11   jtn     Added pm_smbc.h, pm_btm.h, pm_ovp.h
04/21/11   mpt     Added "pm_boot.h" and "pm_boot_chg.h"
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
11/05/10   hs      Added "pm_nfc.h" for NFC support.
07/05/10   wra     Modified Header includes to accomodate MSM8660
12/02/09   jtn     Add header file
11/19/09   jtn     Updated header files
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
/*
 * Include all of the header files describing the external interface to the PMIC driver.
 *
 */
#include "pm_err_flags.h"
#include "pm_lib_cmd.h"
#include "pm_external_resources.h"
#include "pm_version.h"  /* include first for definition of pm_model_type */

#include "pm_irq.h"
#include "pm_mpps.h"
#include "pm_rtc.h"
#include "pm_smps.h"
#include "pm_boost.h"
#include "pm_ldo.h"
#include "pm_vs.h"
#include "pm_gpio.h"
#include "pm_bua.h"
#include "pm_pbs_client.h"
#include "pm_uicc.h"
#include "pm_xo_core.h"
#include "pm_clk.h"

#endif /* _PM_LIB_API_H */
