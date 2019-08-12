#ifndef __AHB2PHY_H__
#define __AHB2PHY_H__


/**
 * @file AHB2PHY.h
 * @brief
 * Header file for ABH2PHY.
 */


/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/phy/AHB2PHY.h#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/06/12   sl      Moved broadcast offsets to ddr_target.h.
12/06/11   tw      Initial version.
================================================================================
                   Copyright 2011-2012 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_target.h"
#include "ddr_common.h"


/*==============================================================================
                                  TYPES
==============================================================================*/
typedef struct
{
  boolean CA_PHY;
  boolean DQ00_PHY;
  boolean DQ01_PHY;
  boolean DQ02_PHY;
  boolean DQ03_PHY;

  boolean CA_SITE;
  boolean DQ00_SITE;
  boolean DQ01_SITE;
  boolean DQ02_SITE;
  boolean DQ03_SITE;
} ddr_mask;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ==========================================================================
**  Function : HAL_SDRAM_AHB2PHY_Broadcast_Enable
** ==========================================================================
*/
/*!
*   @brief
*   This function initializes the AHB2PHY
*
*   @param interface_name   -  [IN] The interface to enable broadcast mode
*   @param mask             -  [IN] The broadcast mask
*
*
*   @retval  None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/
void HAL_SDRAM_AHB2PHY_Broadcast_Enable(SDRAM_INTERFACE interface_name, ddr_mask mask);

#endif /* __AHB2PHY_H__ */
