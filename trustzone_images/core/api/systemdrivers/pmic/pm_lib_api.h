#ifndef _PM_LIB_API_H
#define _PM_LIB_API_H
/*! \file 
 *  \n
 *  \brief  pm_lib_api.h ---- PMIC library API header file 
 *  \n
 *  \n This file is a master include file for PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2009-2011 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_lib_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
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


/*****************************************************************
* include all of the header files describing the external interface
* to the PMIC driver
*****************************************************************/
#include "pm_lib_err.h"
#include "pm_lib_cmd.h"
#include "pm_resources_and_types.h"
#include "pm_version.h"  /* include first for definition of pm_model_type */
#include "pm_mpps.h"
#include "pm_rtc.h"
#include "pm_vreg.h"
#include "pm_vs.h"
#include "pm_ncp.h"
#include "pm_gpio.h"
#include "pm_coinchg.h"
#include "pm_itemp.h"
#include "pm_pwron.h"
#include "pm_misc.h"
#include "pm_boot.h"
#include "pm_boot_chg.h"
#include "pm_smbc.h"
#include "pm_btm.h"
#include "pm_arbiter_xoadc.h"
#include "pm_uicc.h"
#include "pm_xo_core.h"
#include "pm_clk_buffer.h"
#endif /* _PM_LIB_API_H */
