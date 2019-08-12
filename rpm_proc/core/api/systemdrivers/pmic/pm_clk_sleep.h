#ifndef PM_CLK_SLEEP__H
#define PM_CLK_SLEEP__H

/** 
@file pm_clk_sleep.h

     This header file contains functions and variable declarations 
     to support the PMIC sleep clock peripheral.
*/
/*
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_clk_sleep.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
03/13/13   hw      Rearchitecting clk module driver to peripheral driver
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_clk.h"

/*===========================================================================

                 API PROTOTYPES

===========================================================================*/
/** @addtogroup pm_clk_sleep
@{ */

/** Returns the sleep status (enabled or disabled) of the
    clock (32 kHz crystal oscillator).

  @param[in] pmic_chip Selects the device in which the buffers 
                       being controlled are located. The device index
                       starts with zero.
  @param[in] clk_type  Selects which clock type to use.
                       See #pm_clk_type for more information.
  @param[out] on_off   Sleep clock status:
                       - PM_ON -- Clock is on
                       - PM_OFF -- Clock is off @tablebulletend
 
  @return
  Success or Error -- See #pm_err_flag_type.

  <b>Example</b>
  @code
  pm_clk_sleep_source_status(0, PM_CLK_SLEEP, *on_off);  @endcode
*/
pm_err_flag_type pm_clk_sleep_src_status(uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type* on_off);

/** @} */ /* end_addtogroup pm_clk_sleep */
#endif /* PM_CLK_SLEEP__H */

