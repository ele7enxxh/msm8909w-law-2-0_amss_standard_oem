/** 
  @file pcie_core.h
  @brief
  Provides interfaces that provide platform specific support to PCIe 
  Device Driver

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/05/13   Weijunz      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_PLAT_H__
#define __PCIE_PLAT_H__

#include "comdef.h"
#include "pcie_core.h"

/** Target specific Config structure */
typedef struct _pcie_tgtcfg_type
{
    uint32 device_id;                           /**< -- Device ID                    */
    uint32 vendor_id;                           /**< -- Vendor ID                    */
    pcie_barcfg_type bar_config[PCIE_MAX_BARS]; /**< -- Bar Config info */
    uint32 needs_init;                          /**< -- Flag to indicate PCIe initialization */
    uint32 timeout_ms;                          /**< -- link enumeration Timeout in milliseconds (0 - no timeout) */
}pcie_tgtcfg_type;

/* ============================================================================
**  Function : pcie_plat_init
** ============================================================================
*/
/**
  Performs platform specific initialization. 
    
  Performs platform specific initialization like setting up hwio bases, interrupt
  info, osal layer etc.
    

  @return
  Status code.
  
  @dependencies
  None.
  
*/
pcie_status_type pcie_plat_init(pcie_devinfo_type **pcie_dev, pcie_tgtcfg_type **pcie_tgt_cfg);

/* ============================================================================
**  Function : pcie_plat_cfgcoreid
** ============================================================================
*/
/**
  Update the platform configuration for device id and vendor id at run time.
    
  Update the platform configuration for device id and vendor id at run time.
  This API must be called prior to PCIe initialization to ensure that the IDs
  are updated before the link gets enumerated.
    
  @param[in]  device_id    Device ID 
  @param[in]  vendor_id    Vendor ID

  @return
  Status code.
  
  @dependencies
  pcie_init must not have been called prior to this API being called.
  
  @sideeffects
  The vendor and device IDs will be updated when PCIe core initializes.
*/
pcie_status_type pcie_plat_cfgcoreid(uint32 device_id, uint32 vendor_id);

/* ============================================================================
**  Function : pcie_plat_cfgtimeout
** ============================================================================
*/
/**
  Configures the link enumeration timeout.
    
  Configures the link enumeration timeout.
    
  @param[in]  timeout_en    Enable timeout (1 - Enable ; 0 - Disable)

  @return
  Return_value_description.
*/
pcie_status_type pcie_plat_cfgtimeout(uint32 timeout_en);


#endif /* __PCIE_PLAT_H__ */
