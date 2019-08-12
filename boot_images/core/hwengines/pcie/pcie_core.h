/** 
@file pcie_core.h
@brief
Provides interfaces to access the PCIe Core HW. 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/11/14   Dilip        Voting for CNOC SVS frequency and changes for Core dump
04/25/14   Dilip        Added dev_version in _pcie_devinfo_type
04/23/14   Dilip        Core dump changes to save MHI registers
04/08/14   Dilip        Added pcie_core_req_exit_l1 and  pcie_core_req_enter_l1
03/17/14   Dilip        Added NPA functions for configuring clocks and LDOs
08/05/13   Weijunz      Created
===============================================================================
Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_CORE_H__
#define __PCIE_CORE_H__


#include "comdef.h"
#include "pcieosal.h"
#include "pcie_hwio.h"
#include "DDIChipInfo.h"

#ifndef  PCIE_BOOT
#include "npa.h"
#endif

/** @addtogroup pcie_constants
@{ */

#define PCIE_MAX_BARS           (6) /**< -- Max Number of BARs per spec */
#define PCIE_LINK_TIMEOUT_MS (4000) /**< -- Link Training Timeout is 1 second per spec.  */

/** @}  - pcie_constants */

/** @addtogroup pcie_enum_api
@{ */


/** Link Status Type */
typedef enum {
         PCIE_LINKSTATE_DETECT_QUIET,      /**<-- Detect Quiet state */
         PCIE_LINKSTATE_DETECT_ACT,        /**<-- Detect Active State*/
         PCIE_LINKSTATE_POLL_ACTIVE,       /**<-- Polling Active State*/
         PCIE_LINKSTATE_POLL_COMPLIANCE,   /**<-- Polling Compliance State*/
         PCIE_LINKSTATE_POLL_CONFIG,       /**<-- Polling Config State */
         PCIE_LINKSTATE_PRE_DETECT_QUIET,  /**<-- Exit from polling config to Detect.Quiet*/
         PCIE_LINKSTATE_DETECT_WAIT,       /**<-- Wait before entering Detect.Quiet */
         PCIE_LINKSTATE_CFG_LINKWD_START,  /**<-- Config.LinkWidth.Start State */
         PCIE_LINKSTATE_CFG_LINKWD_ACEPT,  /**<-- Config.LinkWidth.Accept State */
         PCIE_LINKSTATE_CFG_LANENUM_WAIT,  /**<-- Config.LaneNumber.Wait State */
         PCIE_LINKSTATE_CFG_LANENUM_ACEPT, /**<-- Config.LaneNumber.Accept State */
         PCIE_LINKSTATE_CFG_COMPLETE,      /**<-- Config.Complete State */
         PCIE_LINKSTATE_CFG_IDLE,          /**<-- Config.Idle State */
         PCIE_LINKSTATE_RCVRY_LOCK,        /**<-- Recovery RcvrLock State */
         PCIE_LINKSTATE_RCVRY_SPEED,       /**<-- Recovery Speed State */
         PCIE_LINKSTATE_RCVRY_RCVRCFG,     /**<-- Recovery RcvrCfg State */
         PCIE_LINKSTATE_RCVRY_IDLE,        /**<-- Recovery Idle State */
         PCIE_LINKSTATE_L0,                /**<-- L0 State */
         PCIE_LINKSTATE_L0S,               /**<-- L0 Substate */
         PCIE_LINKSTATE_L123_SEND_EIDLE,   /**<-- L123 Send Enter IDLE State */
         PCIE_LINKSTATE_L1_IDLE,           /**<-- L1 Idle State */
         PCIE_LINKSTATE_L2_IDLE,           /**<-- L2 Idle State */
         PCIE_LINKSTATE_L2_WAKE,           /**<-- L2 Wake State */
         PCIE_LINKSTATE_DISABLED_ENTRY,    /**<-- Disabled State Entry */
         PCIE_LINKSTATE_DISABLED_IDLE,     /**<-- Disabled State Idle */
         PCIE_LINKSTATE_DISABLED,          /**<-- Disable State */
         PCIE_LINKSTATE_LPBK_ENTRY,        /**<-- Loopback Entry */
         PCIE_LINKSTATE_LPBK_ACTIVE,       /**<-- Loopback Active */
         PCIE_LINKSTATE_LPBK_EXIT,         /**<-- Loopback Exit */         
         PCIE_LINKSTATE_LPBK_EXIT_TIMEOUT, /**<-- Loopback Exit Timeout */ 
         PCIE_LINKSTATE_HOT_RESET_ENTRY,   /**<-- Hot Reset Entry */
         PCIE_LINKSTATE_HOT_RESET,         /**<-- Hot Reset */
         PCIE_LINKSTATE_RCVRY_EQ0,         /**<-- Recovery Equalization Phase 0*/
         PCIE_LINKSTATE_RCVRY_EQ1,         /**<-- Recovery Equalization Phase 1*/
         PCIE_LINKSTATE_RCVRY_EQ2,         /**<-- Recovery Equalization Phase 2*/
         PCIE_LINKSTATE_RCVRY_EQ3          /**<-- Recovery Equalization Phase 3*/
}pcie_core_linkstate_type;

/**< -- Prefetch mode of a BAR */
typedef enum {
    PCIE_BAR_NONPREFETCHABLE=0,  /**< -- BAR is non-prefetchable */
    PCIE_BAR_PREFETCHABLE=1      /**< -- BAR is pre-fetchable */
}pcie_core_barmode_type;

/** Address mode of a BAR */
typedef enum {
    PCIE_BARTYPE_32BIT=0,       /**< -- BAR is 32-bit addressable */ 
    PCIE_BARTYPE_64BIT=2        /**< -- BAR is 64-bit addressable */    
}pcie_core_bartype_type;

/** Memspace type of a bar */
typedef enum {
    PCIE_BARMEMSPACE_MEM=0,    /**< -- BAR is of memory type */ 
    PCIE_BARMEMSPACE_IO=1      /**< -- BAR is of IO type */    
}pcie_core_barmemspace_type;

/** @}  - pcie_enum_api */

/** @addtogroup pcie_struct_api
@{ */

/** Device Info structure */
typedef struct _pcie_devinfo_type
{
    uint32 parf_base;         /**< -- Base Address of the PARF */
    uint32 parf_base_phys;    /**< -- Physical Base Address of the PARF */
    uint32 phy_base;          /**< -- Base Address of the PHY */
    uint32 phy_base_phys;     /**< -- Physical Base Address of the PHY */
    uint32 memmap_base;       /**< -- Base Address of the PCIE AXI Region */
    uint32 memmap_base_phys;  /**< -- Physical Base Address of the PCIE AXI Region */
    uint32 dbi_base;          /**< -- Base Address of the DBI */
    uint32 dbi_base_phys;     /**< -- Physical Base Address of the DBI */
    uint32 elbi_base;         /**< -- Base Address of the ELBI */
    uint32 elbi_base_phys;    /**< -- Physical Base Address of the ELBI */
    uint32 mhi_base;          /**< -- Base Address of the MHI */
    uint32 mhi_base_phys;     /**< -- Physical Base Address of the MHI */
    /*PCIe IRQ handle with system driver*/
    pcie_osal_irq_type link_down_irq_h;
    pcie_osal_irq_type link_up_irq_h; 
    pcie_osal_irq_type d_state_irq_h ;
    pcie_osal_irq_type l1_inact_timeout_irq_h ;
    pcie_osal_irq_type bridge_flush_irq_h ;
    pcie_osal_irq_type pcie_dma_irq_h ;

    DalChipInfoVersionType dev_version;       /**< -- Holds the harware version */

}pcie_devinfo_type;

/** PCIe Core Dump Structure */
/* HW Context */
typedef struct _pcie_coredump_type
{
   uint32 parf_regs[PCIE_PARF_SIZE/sizeof(uint32)];    /* Register for storing PCIe PARF Registers */
   uint32 test_bus[0x33];                              /* Register for storing test bus Registers */
   uint32 mhi_regs[PCIE_MHI_SIZE/sizeof(uint32)];      /* Register for storing PCIe MHI registers */
   uint32 dbi_regs[PCIE_DBI_SIZE/sizeof(uint32)];      /* Register for storing PCIe DBI Registers */
   uint32 elbi_regs[PCIE_ELBI_SIZE/sizeof(uint32)];    /* Register for storing PCIe ELBI Registers */
   uint32 phy_regs[PCIE_CM_SIZE/sizeof(uint32)];       /* Register for storing PCIe PHY Registers */
}pcie_coredump_type;

/** PCIe coredump type*/
typedef enum {
    PCIE_FULL_DUMP=0,                  /**< Dumps all PCIe registers */
    PCIE_PARTIAL_DUMP=1                /**< Dumps only the PCIe AHB register */
}pcie_core_dump_type;

/** PM configuration of LDOs. 
         This enum entries from 0 till the last one should match the ones defined in pmapp_npa.h file. 
	     Created a copy for easier naming within PCIe.
*/
typedef enum
{
   PCIE_NO_MODE_REQUESTED = -1,        /**< -- For init purpose only  */
   PCIE_OFF = 0,                       /**< -- Completely powered down state */
   PCIE_SUSPEND,                       /**< -- Suspended state, but no VddMin */
   PCIE_ACTIVE_NOMINAL,                /**< -- Active mode with Nominal frequencies */ 
   PCIE_ACTIVE_SVS,                    /**< -- Active mode with SVS frequencies */ 
   PCIE_ACTIVE_TURBO,                  /**< -- Active mode with Turbo frequencies */ 
   PCIE_MAX,
}pcie_core_pm_modes_type;

#ifndef  PCIE_BOOT
/** Context for LDOs NPA modes */
typedef struct {
    npa_client_handle         pcie_handle;                         /**< --  PCIe handle */
    pcie_core_pm_modes_type   pcie_pmic_npa_mode_requested;        /**< --  PCIe handle */
} pcie_vregmgr_ctx_type;
#endif

/** PCIe Config structure */
typedef struct _pcie_config_type
{
    pcie_devmode_type dev_type;       /**< -- Device Type                  */
    pcie_targetspeed_type tgt_speed;  /**< -- Max speed to negotiate with  */
    uint32 device_id;                 /**< -- Device ID                    */
    uint32 vendor_id;                 /**< -- Vendor ID                    */
}pcie_config_type;

/** ATU Config structure */
typedef struct _pcie_atuconfig_type
{
    uint32 index;           /**< -- Index of the ATU region to be programmed */
    pcie_atudir_type dir;   /**< -- Direction of the ATU region*/
    pcie_atumode_type mode; /**< -- Mode of the ATU region (only address and bar matched modes supported)*/
    uint32 bar;             /**< -- Bar Index to match. Only valid for mode == PCIE_ATUMODE_BARMATCHED */
    uint32 base_addr_low;   /**< -- Untranslated lower 32-bits of the Source (untranslated) address. Valid for mode == PCIE_ATUMODE_ADDRESSMATCHED. Must be 4K aligned */
    uint32 base_addr_hi;    /**< -- Untranslated upper 32-bits of the Source (untranslated) address */
    uint32 size;            /**< -- Size of the region to be translated. Valid for mode == PCIE_ATUMODE_ADDRESSMATCHED. Must be 4K aligned */
    uint32 target_addr_low; /**< -- Translated lower 32-bits of the target (translated) address. Valid for both modes. Must be 4K aligned */
    uint32 target_addr_hi;  /**< -- Translated upper 32-bits of the target (translated) address. */  
}pcie_atuconfig_type;

/** Bar Config structure */
typedef struct _pcie_barconfig_type
{
    uint32 bar;                           /**< -- Bar Index */
    uint32 local_addr_low;                /**< -- Lower 32-bit of local address */
    uint32 local_addr_hi;                 /**< -- Upper 32-bit of local address */
    uint32 mask;                          /**< -- Mask of the local address range */
    pcie_core_barmode_type mode;          /**< -- Mode of the Bar */
    pcie_core_bartype_type type;          /**< -- Type of the Bar */
    pcie_core_barmemspace_type memspace;  /**< -- Access type of the Bar */ 
}pcie_barcfg_type;


extern pcie_osal_clock_ctl_type pcie_pipe_clk_ctl ;
/** @}  - pcie_struct_api */

/** @addtogroup pcie_constants
@{ */

/** Field Macro
*  @note Can be used only as a param to a pcie_read/write_reg_field!!
*/
#define PCIE_FLD(reg,field) reg, reg##_##field##_MASK, reg##_##field##_SHFT

/** Mask macro */
#define PCIE_MASK(reg,field) ( reg##_##field##_MASK )

/** Shift Macro */
#define PCIE_SHFT(reg,field) ( reg##_##field##_SHFT )

/** Field Value */
#define PCIE_FVAL(reg, field, val) (((uint32)(val) << ( reg##_##field##_SHFT )) & ( reg##_##field##_MASK ))

/** @}  - pcie_constants */

/** @addtogroup pcie_func_api
@{ */


/* ============================================================================
**  Function : pcie_core_req_enter_l1(void)
** ============================================================================
*/
/**
  Request to enter L1 link state.
 
  
  
  @param[in]      void
  
  
  @return
  pcie_link_state_type
*/
void pcie_core_req_enter_l1(void) ;

/* ============================================================================
**  Function : pcie_core_req_exit_l1(void)
** ============================================================================
*/
/**
  Request to exit L1 link state.
 
  
  
  @param[in]      void
  
  
  @return
  pcie_link_state_type
*/
void pcie_core_req_exit_l1(void) ;

/* ============================================================================
**  Function : pcie_write_reg
** ============================================================================
*/
/**
  Writes <c>val</c> to the register at <c>base+offset</c>.
  
  @param[in]  base      Base address of the module
  @param[in]  offset    Offset of the register within the module
  @param[in]  val       Value to be written
  
  @return
  None.
*/
void pcie_write_reg(uint32 base,uint32 offset,uint32 val);

/* ============================================================================
**  Function : pcie_write_reg_field
** ============================================================================
*/
/**
  Writes <c>field_val</c> to the field defined by <c>mask</c> and <c>shift</c>
  in the register at <c>base+offset</c>.
  
  @param[in]  base         Base address of the module               
  @param[in]  offset       Offset of the register within the module 
  @param[in]  mask         Mask for the field to be programmed 
  @param[in]  shift        Shift required to program the field 
  @param[in]  field_val    Value to be programmed in the field
  
  @return
  None.
*/
void pcie_write_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift, uint32 field_val);

/* ============================================================================
**  Function : pcie_read_reg
** ============================================================================
*/
/**
  Returns the value in the register at <c>base+offset</c>.
  
  @param[in]  base      Base address of the module               
  @param[in]  offset    Offset of the register within the module 
  
  @return
  Value of the register at <c>base+offset</c>.
  
*/
uint32 pcie_read_reg(uint32 base,uint32 offset);

/* ============================================================================
**  Function : pcie_read_reg_field
** ============================================================================
*/
/**
  Returns the <c>field_val</c> from the field defined by <c>mask</c> and
  <c>shift</c> in the register at <c>base+offset</c>.
  
  @param[in]  base      Base address of the module               
  @param[in]  offset    Offset of the register within the module 
  @param[in]  mask      Mask for the field to be read
  @param[in]  shift     Shift required to read the field
  
  @return
  Value of the field
*/
uint32 pcie_read_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift);

/* ============================================================================
**  Function : pcie_core_initerrata
** ============================================================================
*/
/**
  Additional PCIe Core HW configurations during initialization.
  
  
  @param[in,out]  pcie_dev    handle to the pcie device object
  
  
  @return
  None.  
*/

void pcie_core_initerrata(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_init
** ============================================================================
*/
/**
  Initializes the PCIe Core and the PHY.
  
  Initializes the PCIe Core and the PHY.
  
  @param[in,out]  pcie_dev    handle to the pcie device object
  @param[in,out]  pcie_cfg    Configuration to be programmed in the PCIe core
  
  @return
  None.  
*/
pcie_status_type pcie_core_init(pcie_devinfo_type *pcie_dev, pcie_config_type *pcie_cfg);

/* ============================================================================
**  Function : pcie_core_getversion
** ============================================================================
*/
/**
  Returns the revision of the core.
  
  Returns the revision of the core.
  
  @param[in]  pcie_dev    handle to the pcie device object
  
  @return
  The revision of the core.

*/
uint32 pcie_core_getversion(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_setcoreid
** ============================================================================
*/
/**
  Sets the device id and vendor id for the device.
    
  Sets the device id and vendor id for the device.
    
  @param[in,out]  pcie_dev     handle to the pcie device object
  @param[in]      device_id    Device ID
  @param[in]      vendor_id    Vendor ID

  @return
  None.
  
  @dependencies
  Link must not have been enumerated.  
*/
void pcie_core_setcoreid(pcie_devinfo_type *pcie_dev, uint32 device_id, uint32 vendor_id);

/* ============================================================================
**  Function : pcie_core_enableltssm
** ============================================================================
*/
/**
  Enables the Link Training and Status State Machine. 
  
  Enables the Link Training and Status State Machine. Once the state machine
  completes successfully, the link to the root complex should be complete. This
  can be confirmed by polling pcie_core_getlinkstate
  
  @param[in]  pcie_dev    handle to the pcie device object
  
  @return
  None.
  
  @sa
  pcie_core_getlinkstate.
*/
void pcie_core_enableltssm(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_getlinkstate
** ============================================================================
*/
/**
  Gets the state of the link.
  
  Returns the state of the link.
  
  @param[in]   pcie_dev      handle to the pcie device object
  @param[out]  link_state    State of the link
  
  @return
  State of the link (up or down).  
*/
pcie_linkstatus_type pcie_core_getlinkstate(pcie_devinfo_type *pcie_dev, 
                                            pcie_core_linkstate_type *link_state);

/* ============================================================================
**  Function : pcie_core_isenumerated
** ============================================================================
*/
/**
  Detects if the link has been enumerated by the root complex.
  
  After the link is up and trained, the root complex will have to assign
  resources to this end point and "enable" bus master access to it. Once that
  is done, the link is said to have been enumerated.
  
  @param[in]  pcie_dev    handle to the pcie device object
  
  @return
  TRUE - if link is enumerated
  FALSE - if link is NOT enumerated
*/
boolean pcie_core_isenumerated(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_setdevicetype
** ============================================================================
*/
/**
  Sets the operational mode of the pcie core.
  
  Sets the operational mode of the pcie core. There are two valid modes of
  operations: a. Root Complex mode b. End Point mode
  
  @param[in]  pcie_dev    handle to the pcie device object
  @param[in]  devtype     Device type to be configured to
  
  @return
  None.
*/
void pcie_core_setdevicetype(pcie_devinfo_type *pcie_dev, pcie_devmode_type devtype);

/* ============================================================================
**  Function : pcie_core_getdevicetype
** ============================================================================
*/
/**
  Returns the device type.
  
  Returns the device type.
  
  @param[in]  pcie_dev    handle to the pcie device object
  
  @return
  Type of the device (Root Complex or End Point).
*/
pcie_devmode_type pcie_core_getdevicetype(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_settargetspeed
** ============================================================================
*/
/**
  Sets the maximum speed supported by the link.
  
  Sets the maximum speed supported by the link. This value is used by the
  endpoint during link training sequence with the root complex.
  
  @param[in]  pcie_dev     handle to the pcie device object
  @param[in]  targetspeed  Maximum speed supported as per the spec 
  
  @return
  None.
*/
void pcie_core_settargetspeed(pcie_devinfo_type *pcie_dev, pcie_targetspeed_type targetspeed);

/* ============================================================================
**  Function : pcie_core_gettargetspeed
** ============================================================================
*/
/**
  Gets the maximum speed supported by the link.
  
  Gets the maximum speed supported by the link.
  
  @param[in]  pcie_dev    handle to the pcie device object
  
  @return
  Maximum speed supported by the link.
  
*/
pcie_targetspeed_type pcie_core_gettargetspeed(pcie_devinfo_type *pcie_dev);

/* ============================================================================
**  Function : pcie_core_setatuconfig
** ============================================================================
*/
/**
  Configures the internal Address Translation Unit (iATU).
  
  Configures the internal Address Translation Unit (iATU). This is required to
  map the inbound traffic to internal memory and outbound traffic to host memory
  space.
  
  @param[in]  pcie_dev     handle to the pcie device object
  @param[in]  enable       Enable the ATU by default
  @param[in]  atu_config   Configuration for the ATU
  
  @return
  None.  
*/
void pcie_core_setatuconfig(pcie_devinfo_type *pcie_dev, boolean enable, pcie_atuconfig_type *atu_config);

/* ============================================================================
**  Function : pcie_core_setbarconfig
** ============================================================================
*/
/**
  Sets the BAR Config in the PCIe Config Space 
  
  Sets the BAR Config in the PCIe Config Space. This config determines how big the
  window into the MDM is for the host.
  
  @param[in,out]  pcie_dev      handle to the pcie device object
  @param[in,out]  bar_config    Bar configuration
  
  @return
  None.  
*/
void pcie_core_setbarconfig(pcie_devinfo_type *pcie_dev, pcie_barcfg_type *bar_config );

/* ============================================================================
**  Function : pcie_core_getmsiconfig
** ============================================================================
*/
/**
  Gets the MSI Config from the PCIe Config Space programmed by the host.
  
  Gets the MSI Config from the PCIe Config Space programmed by the host.
  
  @param[in,out]  pcie_dev      handle to the pcie device object
  @param[in,out]  msi_config    MSI Config data to be returned
  
  @return
  None.  
*/
void pcie_core_getmsiconfig(pcie_devinfo_type *pcie_dev, pcie_msiconfig_type *msi_config);

/* ============================================================================
**  Function : pcie_core_sendparfmsi
** ============================================================================
*/
/**
  Sends an MSI via PARF mechanism.
    
  Sends an MSI via PARF mechanism. This approach is used for avoiding the need
  to allocate a 4K entry in the ATU for the MSI address in the host to which
  this vector is to be sent.
    
  @param[in]  pcie_dev      Handle to the pcie device object
  @param[in]  msi_vector    MSI Vector to be triggered at the host

  @return
  None.
  
  @dependencies
  MSI must have been enabled in the caps.
  
*/
void pcie_core_sendparfmsi(pcie_devinfo_type *pcie_dev, uint32 msi_vector);

/* ============================================================================
**  Function : pcie_core_configmhigate
** ============================================================================
*/
/**
  Configure the MHI Gate to allow host access to the MHI MMIO space. It is
  required since the BHI MMIO is located within the MHI block.
    
  Configure the MHI Gate to allow host access to the MHI MMIO space. It is
  required since the BHI MMIO is located within the MHI block.
    
  @param[in]  pcie_dev    handle to the pcie device object
  @param[in]  bar_config  Configuration to program the MHI gate

  @return
  None.
  
*/
void pcie_core_configmhigate(pcie_devinfo_type *pcie_dev, pcie_barcfg_type *bar_config);

/* ============================================================================
**  Function : pcie_core_init_clk
** ============================================================================
*/
/**
  Init the PCIe Core clock .
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_init_clk(void);

/* ============================================================================
**  Function : pcie_core_enable_clk
** ============================================================================
*/
/**
  Enable the PCIe Core clock
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_enable_clk(void);

/* ============================================================================
**  Function : pcie_core_disable_clk
** ============================================================================
*/
/**
  Disable the PCIe Core clock
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_disable_clk(void);


/* ============================================================================
**  Function : pcie_core_init_pwr_domain
** ============================================================================
*/
/**
  Init the PCIe Core power domain with system  driver.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_init_pwr_domain(void);

/* ============================================================================
**  Function : pcie_core_enable_pwr_domain
** ============================================================================
*/
/**
  Enable the PCIe Core power domain
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_enable_pwr_domain(void);

/* ============================================================================
**  Function : pcie_core_disable_pwr_domain
** ============================================================================
*/
/**
  Disable the PCIe Core power domain
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_disable_pwr_domain(void);

/* ============================================================================
**  Function : pcie_core_enter_lpm
** ============================================================================
*/
/**
  request PCIe core to enter low power mode
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_enter_lpm(void);

/* ============================================================================
**  Function : pcie_core_exit_lpm
** ============================================================================
*/
/**
  request PCIe core to exit low power mode
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_exit_lpm(void);

/* ============================================================================
**  Function : pcie_core_is_ahb_or_axi_clk_off
** ============================================================================
*/
/**
  Checks if either AHB or AXI clock is off
  
  
  @param[in]  void
  
  
  @return
	returns 1 if either of them is off else returns 0.  
*/
#ifdef PCIE_AHB_AXI_CLK_CHECK
uint32 pcie_core_is_ahb_or_axi_clk_off(void);
#endif 

/* ============================================================================
**  Function : pcie_core_dump
** ============================================================================
*/
/**
Dumps the PCIe registers.

 
@param[in,out]      pcie_devinfo_type             Device Info structure.
@param[in,out]      pcie_coredump_type            Coredump structure.
@param[in]          pcie_core_dump_type           Partial/Full dump

@return
Void
*/

void pcie_core_dump(pcie_devinfo_type *pcie_dev,pcie_coredump_type *pcie_coredump,pcie_core_dump_type pcie_dump_type);

/* ============================================================================
**  Function : pcie_core_pmic_npa_client_init
** ============================================================================
*/
/**
  Initialize the PMIC NPA client by registering it with the resources.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_pmic_npa_client_init(void);

/* ============================================================================
**  Function : pcie_core_pmic_npa_mode_request
** ============================================================================
*/
/**
  Votes for requested power mode configurations for various LDOs
  
  
  @param[in]  pcie_core_pm_modes_type      The requested power mode configuration
  
  
  @return
  None.  
*/

void pcie_core_pmic_npa_mode_request(pcie_core_pm_modes_type);

/* ============================================================================
**  Function : pcie_core_clk_npa_client_init
** ============================================================================
*/
/**
  Initialize the clk NPA client by registering it with the resources.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_clk_npa_client_init(void);

/* ============================================================================
**  Function : pcie_core_clk_npa_mode_request
** ============================================================================
*/
/**
  Vote for requested clk frequency for CNOC
  
  
  @param[in]  None
  
  
  @return
  None.  
*/


void pcie_core_clk_npa_mode_request(void);

/* ============================================================================
**  Function : pcie_core_clk_npa_mode_release
** ============================================================================
*/
/**
  Takes the votes back for CNOC
  
  
  @param[in]  void
  
  
  @return
  None.  
*/

void pcie_core_clk_npa_mode_release(void);

/* ============================================================================
**  Function : pcie_core_npa_clkmgr_init
** ============================================================================
*/
/**
  Initialize the NPA clients by registering it with the resources.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_npa_clkmgr_init(void);

/* ============================================================================
**  Function : pcie_core_npa_clkmgr_config_clocks
** ============================================================================
*/
/**
  Votes for various clocks for specific frequencies
  
  
  @param[in]  void
  
  
  @return
  None.  
*/

void pcie_core_npa_clkmgr_config_clocks(void);

/* ============================================================================
**  Function : pcie_core_npa_clkmgr_release_clocks
** ============================================================================
*/
/**
  Takes the votes back for various clocks
  
  
  @param[in]  void
  
  
  @return
  None.  
*/

void pcie_core_npa_clkmgr_release_clocks(void);



#endif /* __PCIE_CORE_H__ */
