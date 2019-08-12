/** 
  @file pcie_phy.h
  @brief
  Provides interfaces to access the PCIe PHY HW. 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/18/14   Dilip        Added pcie_phy_config_update
05/06/14   Dilip        Removed pcie_phy_config_update function
04/30/14   Dilip        Added pcie_phy_config_update function
08/05/13   Weijunz      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_PHY_H__
#define __PCIE_PHY_H__

#include "pcie_core.h"

/** @addtogroup pcie_func_api
@{ */


/* ============================================================================
**  Function : pcie_phy_init
** ============================================================================
*/
/**
  Performs initialization of the PCIe PHY.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  Status of the initialization.  
*/
pcie_status_type pcie_phy_init(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_phy_config_update
** ============================================================================
*/
/**
  Update Phy additional settings.
    
  Update Phy additional settings.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  None.
*/
void pcie_phy_config_update(pcie_devinfo_type *pcie_dev);

/** @}  - pcie_func_api */


#endif /* __PCIE_PHY_H__ */

