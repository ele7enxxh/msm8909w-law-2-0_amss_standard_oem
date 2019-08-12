/*! \file 
 *  \n
 *  \brief  pm_misc.h ---- PMIC-MISC MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC MISC module.
 *
 *
 *  \n &copy; Copyright (c) 2009-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_misc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
05/21/09   wra     New file
===========================================================================*/

#ifndef PM_MISC_H
#define PM_MISC_H

/* The following MISC module type defined is not defined in the class because it needs to be accessible by other modules. */
typedef enum
{
  PM_MISC_LEGACY_MODE__DESSERT = 0,

  PM_MISC_LEGACY_MODE__ASSERT,

  PM_MISC_LEGACY_MODE__INVALID
}pm_misc_legacy_mode_type;

typedef enum {
  PM_MISC_RES_STATE__FOLLOW_BOOT = 0,

  PM_MISC_RES_STATE__OFF         = 1,

  PM_MISC_RES_STATE__LPM         = 2,

  PM_MISC_RES_STATE__HPM         = 3,

  PM_MISC_RES_STATE__INVALID
}pm_misc_res_state_type;


typedef enum
{
  PM_MISC_MODE_ACT__DESSERT = 0,

  PM_MISC_MODE_ACT__ASSERT,

  PM_MISC_MODE_ACT__INVALID
}pm_misc_mode_act_type;

typedef enum
{
  PM_MISC_MODE_CTRL_MASK__FOLLOW_MODE_ACT = 0,

  PM_MISC_MODE_CTRL_MASK__MODE_ACT_DONTCARE,

  PM_MISC_MODE_CTRL_MASK__INVALID
}pm_misc_mode_ctrl_mask_type;

/* Note, the enum also act as a "mask" for the register */
typedef enum
{
  PM_MISC_SEL_CTRL_PIN0 = 0x01,

  PM_MISC_SEL_CTRL_PIN1 = 0x02,

  PM_MISC_SEL_CTRL_PIN2 = 0x04,

  PM_MISC_SEL_CTRL_MODE = 0x08,

  PM_MISC_SEL_CTRL_PIN__INVALID
}pm_misc_sel_ctrl_pin_type;

typedef enum
{
  PM_MISC_CTRL_PIN_MASK__ALLOW_PIN_CTRL = 0,

  PM_MISC_CTRL_PIN_MASK__DISALLOW_PIN_CTRL,

  PM_MISC_CTRL_PIN_MASK__INVALID
}pm_misc_ctrl_pin_mask_type;

typedef enum
{
  PM_MISC_TERM__FORCE_OFF        = 0x00,

  PM_MISC_TERM__FORCE_PULLUP     = 0x01,

  PM_MISC_TERM__FORCE_PULLDOWN   = 0x10,

  PM_MISC_TERM__FOLLOW_MODE_CTRL = 0x11,

  PM_MISC_TERM__INVALID
}pm_misc_term_type;

typedef enum
{
  PM_MISC_DTEST__NORMAL = 0,

  PM_MISC_DTEST__MODE_CTRL_TO_DTEST3,

  PM_MISC_DTEST__INVALID
}pm_misc_dtest_type;

typedef enum
{
  /* For CTRLx pin, it means wire enable with active high OR wire enable with active low */
  /* For Mode_Ctrl pin, it means low power mode with active high OR low power mode with active low */
  PM_MISC_POLARITY__ACTIVE_H = 0,

  PM_MISC_POLARITY__ACTIVE_L,

  PM_MISC_POLARITY__INVALID
}pm_misc_polarity_type;

#endif

