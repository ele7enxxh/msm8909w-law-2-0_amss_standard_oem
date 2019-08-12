/** 
  @file pcie_plat_pbl.c
  @brief
  Implements the interfaces that provide platform specific support to PCIe 
  Device Driver

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/27/15   Dilip        Fixed KW warnings
04/25/14   Dilip        added pcie_plat_init_chip_info function to initialize chip info during init
08/05/13   Weijunz      Created
===============================================================================
                   Copyright (c) 2013-15 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "pcie_plat.h"
#include "pcie_hwio.h"



/** Option to configure timeout */
const uint32 pcie_link_timeout=PCIE_LINK_TIMEOUT_MS;

/** Target specific pcie device info */
pcie_devinfo_type pcie_dev_info;

/** Target specific configuration data */
pcie_tgtcfg_type pcie_tgt_cfg_info = 
{
    /* Device ID */ 0x0300, 
    /*Vendor ID */  0x17CB, 

    /* bar_config */
    {
      /* bar loc_addr_lo          loc_addr_hi mask    mode                      type                memspace */
      {  0,  PCIE_MHI_BASE_PHYS,  0x00000000, 0xFFF,  PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_64BIT, PCIE_BARMEMSPACE_MEM },
      /*Disabled since BAR0 is 64bit*/
      {  1,  0x0,                 0x0,        0x0,    PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_32BIT, PCIE_BARMEMSPACE_MEM }, 
      {  2,  PCIE_MHI_BASE_PHYS,  0x00000000, 0xFFF,  PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_64BIT, PCIE_BARMEMSPACE_MEM },
      /*Disabled since BAR2 is 64bit*/
      {  3,  0x0,                 0x0,        0x0,    PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_32BIT, PCIE_BARMEMSPACE_MEM }, 
      /*Unused*/
      {  4,  0x0,                 0x0,        0x0,    PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_32BIT, PCIE_BARMEMSPACE_MEM }, 
      /*Unused*/
      {  5,  0x0,                 0x0,        0x0,    PCIE_BAR_NONPREFETCHABLE, PCIE_BARTYPE_32BIT, PCIE_BARMEMSPACE_MEM }, 
    },
    /* needs_init */ 1,
    /* timeout_ms */ 0
};

/* ============================================================================
**  Function : pcie_plat_init_chip_info
** ============================================================================
*/
/**
  Initializes the chip info. 
    
  Initializes the chip info.
    
  @return
  Status code.
  
  @dependencies
  None.
  
*/
static void pcie_plat_init_chip_info()
{
  DALResult dal_result;
  DalDeviceHandle *ChipInfo = NULL;
  static DalChipInfoVersionType nVer_info;

  dal_result = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &ChipInfo);
  if(dal_result != DAL_SUCCESS)
  {
      pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach failed");
  }
  
  if(NULL == ChipInfo)
  {
      pcie_osal_debug(MSG_FATAL, "ChipInfo is NULL");
      return;
  }
   
  dal_result = DalDevice_Open(ChipInfo, DAL_OPEN_SHARED);
  if(dal_result != DAL_SUCCESS)
  {
      pcie_osal_debug(MSG_FATAL, "DalDevice_Open failed");
  }

  dal_result = DalChipInfo_GetChipVersion(ChipInfo, &nVer_info);
  if(dal_result != DAL_SUCCESS)
  {
      pcie_osal_debug(MSG_FATAL, "DalChipInfo_GetChipVersion failed");
  }

  dal_result = DalDevice_Close(ChipInfo);
  if(dal_result != DAL_SUCCESS)
  {
      pcie_osal_debug(MSG_FATAL, "DalDevice_Close failed");
  }

  pcie_dev_info.dev_version = nVer_info;
}
 
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

pcie_status_type pcie_plat_init(pcie_devinfo_type **pcie_dev, pcie_tgtcfg_type **pcie_tgt_cfg)
{
    pcie_osal_meminfo pcie_ahb_mem,pcie_axi_mem ;

    if(pcie_dev == NULL || pcie_tgt_cfg == NULL) {
        return PCIE_STATUS_INVALIDPARAMS;
    }

    pcie_ahb_mem.pa = PCIE_PCIE20_WRAPPER_AHB_BASE_PHYS;
    pcie_ahb_mem.size = PCIE_PCIE20_WRAPPER_AHB_BASE_SIZE;


#ifndef PCIE_BOOT
    pcie_osal_mallocmemregion(&pcie_ahb_mem, pcie_ahb_mem.size);
	if (NULL == pcie_ahb_mem.handle)
    {
       pcie_osal_debug(MSG_FATAL, "pcie_plat_init: Could not allocate memory for pcie_ahb_mem"); 
       return PCIE_STATUS_INVALIDPARAMS;  
    }
#else
      pcie_ahb_mem.va = PCIE_PCIE20_WRAPPER_AHB_BASE_PHYS;
#endif 
	

    pcie_dev_info.parf_base = pcie_ahb_mem.va;
    pcie_dev_info.parf_base_phys = pcie_ahb_mem.pa;

    pcie_dev_info.mhi_base = pcie_ahb_mem.va + PCIE20_MHI_REG_BASE_OFFS ;
    pcie_dev_info.mhi_base_phys = pcie_ahb_mem.pa + PCIE20_MHI_REG_BASE_OFFS ;

    pcie_dev_info.phy_base = pcie_ahb_mem.va + PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS;
    pcie_dev_info.phy_base_phys = pcie_ahb_mem.pa + PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS;
    
    pcie_axi_mem.pa = PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS;
    pcie_axi_mem.size = PCIE_PCIE20_WRAPPER_AXI_BASE_SIZE;

#ifndef PCIE_BOOT
    pcie_osal_mallocmemregion(&pcie_axi_mem, pcie_axi_mem.size);
	if (NULL == pcie_axi_mem.handle)
    {
       pcie_osal_debug(MSG_FATAL, "pcie_plat_init: Could not allocate memory for pcie_axi_mem"); 
       return PCIE_STATUS_INVALIDPARAMS;  
    }
#else
      pcie_axi_mem.va = PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS;
#endif

    pcie_dev_info.memmap_base = pcie_axi_mem.va;
    pcie_dev_info.memmap_base_phys = pcie_axi_mem.pa;

    pcie_dev_info.dbi_base = pcie_axi_mem.va + DWC_PCIE_DM_REG_BASE_OFFS;
    pcie_dev_info.dbi_base_phys = pcie_axi_mem.pa + DWC_PCIE_DM_REG_BASE_OFFS ;
     
    pcie_dev_info.elbi_base = pcie_axi_mem.va + PCIE20_ELBI_REG_BASE_OFFS;
    pcie_dev_info.elbi_base_phys = pcie_axi_mem.pa + PCIE20_ELBI_REG_BASE_OFFS ;

	pcie_plat_init_chip_info();

    /* Provide configuration to the caller */
    *pcie_dev     = &pcie_dev_info;
    *pcie_tgt_cfg = &pcie_tgt_cfg_info;

    return PCIE_STATUS_SUCCESS;
}

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
pcie_status_type pcie_plat_cfgcoreid(uint32 device_id, uint32 vendor_id)
{
    pcie_tgt_cfg_info.device_id = device_id;
    pcie_tgt_cfg_info.vendor_id = vendor_id;

    return PCIE_STATUS_SUCCESS;
}

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
pcie_status_type pcie_plat_cfgtimeout(uint32 timeout_en)
{
    if(0x1 == timeout_en) 
    {
        pcie_tgt_cfg_info.timeout_ms = pcie_link_timeout;
    }
    return PCIE_STATUS_SUCCESS;
}
