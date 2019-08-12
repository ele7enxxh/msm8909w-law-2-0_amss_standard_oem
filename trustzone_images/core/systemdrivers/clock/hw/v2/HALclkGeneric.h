#ifndef __HALCLKGENERIC_H__
#define __HALCLKGENERIC_H__
/*
==============================================================================

FILE:         HALclkGeneric.h

DESCRIPTION:
  Internal, chipset specific definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clock/hw/v2/HALclkGeneric.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/11/11   vs      Created.

==============================================================================
            Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/



/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * Common root clock configuration registers and their relative offset to
 * the first register (*_CMD_CGR)
 */
#define HAL_CLK_CMD_REG_OFFSET                                         0
#define HAL_CLK_CFG_REG_OFFSET                                         0x4
#define HAL_CLK_M_REG_OFFSET                                           0x8
#define HAL_CLK_N_REG_OFFSET                                           0xC
#define HAL_CLK_D_REG_OFFSET                                           0x10


/*
 * Common block fields/masks
 */
#define HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK                           0x00000001


/*
 * Common root clock command fields/masks (*_CMD_RCGR)
 */
#define HAL_CLK_CMD_CGR_ROOT_OFF_FMSK                                  0x80000000
#define HAL_CLK_CMD_CGR_ROOT_EN_FMSK                                   0x00000002
#define HAL_CLK_CMD_CFG_UPDATE_FMSK                                    0x00000001
#define HAL_CLK_CMD_CFG_UPDATE_SHFT                                    0


/*
 * Common root clock config fields/masks (*_CFG_RCGR)
 */
#define HAL_CLK_CFG_CGR_MODE_FMSK                                      0x00003000
#define HAL_CLK_CFG_CGR_MODE_SHFT                                      0xc
#define HAL_CLK_CFG_CGR_SRC_SEL_FMSK                                   0x00000700
#define HAL_CLK_CFG_CGR_SRC_SEL_SHFT                                   0x8
#define HAL_CLK_CFG_CGR_SRC_DIV_FMSK                                   0x0000001F
#define HAL_CLK_CFG_CGR_SRC_DIV_SHFT                                   0

#define HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL                             0x2
#define HAL_CLK_CFG_CFG_BYPASS_MODE_VAL                                0


/*
 * Common clock branch fields/masks (*_CBCR register)
 */
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK                           0x80000000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_SHFT                           31
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK                           0x01FF0000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT                           16
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK             0x00004000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK           0x00002000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK          0x00001000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK                        0x00000001
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_SHFT                        0


/*
 * Common distributed switch control fields (*_GDSCR register)
 */
#define HAL_CLK_GDSCR_PWR_ON_FMSK                                      0x80000000
#define HAL_CLK_GDSCR_PWR_ON_SHFT                                      31
#define HAL_CLK_GDSCR_EN_ALL_FMSK                                      0x000000C0
#define HAL_CLK_GDSCR_CLAMP_IO_FMSK                                    0x00000030
#define HAL_CLK_GDSCR_SW_COLLAPSE_FMSK                                 0x00000001
#define HAL_CLK_GDSCR_SW_COLLAPSE_SHFT                                 0


#endif  /* __HALCLKGENERIC_H__ */

