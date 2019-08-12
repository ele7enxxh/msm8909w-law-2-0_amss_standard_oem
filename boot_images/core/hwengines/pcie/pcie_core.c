/** 
@file pcie_core.c
@brief
Implements interfaces to access the PCIe Core HW. 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/11/14   Dilip   Voting for CNOC SVS frequency and changes for Core dump
05/21/14   Dilip   Enabled BAMDMA
05/20/14   Dilip   Disabled Q2A flush
05/11/14   MK      Remove the app_req_enter_l1 bit
05/06/14   Dilip   Enable/Disable pipe clk in enter/exit lpm
04/30/14   Dilip   Decreament the counter for req_enter_l1 only if it is greater than 0 
04/28/14   Dilip   Fixed Clk sequence in enable/disable clks and enter/exit lpm
04/23/14   Dilip   Core dump changes to save MHI registers
04/23/14   Dilip   Removed L1 exit/entry from pcie read and write registers while accessing DBI
04/21/14   Dilip   Check PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE to exit L1
04/08/14   Dilip   Added pcie_core_req_exit_l1 and  pcie_core_req_enter_l1
04/04/14   Dilip   Made 32-bit memcpy for pcie_core_dump and kick the dog after each region copy
03/25/14   Dilip   Change clock sequence in exit_lpm
03/19/14   Dilip   Made the NULL comparisons consistent
03/17/14   MK      Updated core settings for SnS
03/17/14   Dilip   Added NPA functions for configuring clocks and LDOs
03/17/14   Dilip   Replaced assert with pcie_osal_debug
08/05/13   Weijunz Created
===============================================================================
Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "pcie_core.h"
#include "pcie_hwio.h"
#include "pcieosal.h"
#include "pcie_clk_hwio.h"
#include "busywait.h"

#include "pcie_phy.h"

#ifndef  PCIE_BOOT
#include "dog.h"
#include "CoreTime.h"
#include "sleepi.h"
/* For NPA calls */
#include "pmapp_npa.h"
#endif

/* Frequency for SNOC nominal mode */
#define PCIE_SNOC_NOMINAL_MODE_SPEED_KHZ     (200000)

/* Frequency for CNOC SVS mode */
#define PCIE_CNOC_SVS_MODE_SPEED_KHZ         (37500)

/* Frequency for BIMC nominal mode */
#define PCIE_BIMC_NOMINAL_MODE_SPEED_KHZ     (400000)


/* DBI Base Address*/
const uint32 pcie_parf_dbi_base_addr = 0x3FFFE000;

/*Local Prototypes*/
static pcie_osal_clock_ctl_type pcie_ahb_clk_ctl ;
static pcie_osal_clock_ctl_type pcie_axi_clk_ctl ;
pcie_osal_clock_ctl_type pcie_pipe_clk_ctl ;
static pcie_osal_clock_ctl_type pcie_sleep_clk_ctl ;
static pcie_osal_clock_ctl_type pcie_axi_mstr_clk_ctl;
static pcie_osal_pwr_ctl_type pcie_core_pwr_ctl ;
static pcie_osal_clock_ctl_type pcie_phy_clk_ctl ;

#ifndef  PCIE_BOOT
static uint32 pcie_rw_glob_cnt=0;
/* NPA Handle for the client for LDOs */
static pcie_vregmgr_ctx_type pcie_vregmgr;
/* NPA Handle for the client for SNOC resource */
static  npa_client_handle           pcie_bus_snoc_npa_client;
/* NPA Handle for the client for CNOC resource */
static  npa_client_handle           pcie_bus_cnoc_npa_client;
/* NPA Handle for the client for BIMC resource */
static  npa_client_handle           pcie_bus_bimc_npa_client;
#endif
 
extern pcie_devinfo_type *pcie_dev;


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
void pcie_core_req_enter_l1(void)
{
#ifndef  PCIE_BOOT
    INTLOCK();
	if(pcie_rw_glob_cnt)
    {
        pcie_osal_atomic_dec(&pcie_rw_glob_cnt);
    }
	if(pcie_rw_glob_cnt==0)
	{
        pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,REQ_EXIT_L1),0X0);
        pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,REQ_ENTR_L1),0X1);
    }
    INTFREE();
#endif
}

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
void pcie_core_req_exit_l1(void)
{
#ifndef  PCIE_BOOT
   uint32 pm_stts_1;
   uint32 time_elapsed = 0;
   uint32 t1,t2;

   pcie_osal_atomic_inc(&pcie_rw_glob_cnt);
   if(pcie_rw_glob_cnt)
   {        
     pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,REQ_ENTR_L1),0X0);
     pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,REQ_EXIT_L1),0X1);
     __dmb(0xF);

     /* Sanity checking */
     pm_stts_1=pcie_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1); 


     /* Check were not out of substates */
     if(0 != (pm_stts_1&(PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)) )
     {
        t1 = CoreTimetick_Get();
   	    while((0 != (pm_stts_1&(PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)))&&(500 > TICKS_TO_US(time_elapsed))){
            busywait(20);
		    t2 = CoreTimetick_Get();
		    if(t2<t1)
		    {
		        time_elapsed = 0xFFFFFFFF-(t1-t2);
		    }
		    else
		    {
		        time_elapsed = t2-t1;
		    }
            pm_stts_1=pcie_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1);
   	    }

        if(0 != (pm_stts_1&(PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)) )
        {
           pcie_osal_debug(MSG_FATAL, "Link is in l1sub with time elapsed = %d",TICKS_TO_US(time_elapsed)); 
        }
     }

   }
#endif
}

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
void pcie_write_reg(uint32 base,uint32 offset,uint32 val)
{
    
    uint32 addr = base + offset;
    uint32 read_val;
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off())
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "Write fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true",addr);
        return  ;
    }
    #endif

   
    (*((volatile uint32 *)(addr))) = val; 
    if ((base == pcie_dev->dbi_base) || (base == pcie_dev->elbi_base))
    {
        read_val = (*((volatile uint32 *)(addr)));
        read_val = read_val;
        pcie_osal_debug(MSG_LOW,"DBI addr write : 0x%X", addr);
    }

}

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
void pcie_write_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift, uint32 field_val)
{
    uint32 regval;
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "Write fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", (uint32)(base+offset));
        return;
    }
    #endif
    regval = pcie_read_reg(base, offset);
    regval &= ~mask;
    regval = regval | (field_val << shift);
    pcie_write_reg(base,offset, regval);
}

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
uint32 pcie_read_reg(uint32 base,uint32 offset)
{
    uint32 addr = base + offset;
    uint32 value; 

    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "Read fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true",addr);
        return -1 ;
    }
    #endif


    value=(*((volatile uint32 *)(addr)));
    if ((base == pcie_dev->dbi_base) || (base == pcie_dev->elbi_base))
    {
        pcie_osal_debug(MSG_LOW,"DBI addr read : 0x%X", addr);
    }
  

    return value; 
    
}

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
uint32 pcie_read_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift)
{
    uint32 regval;
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "Read fail at 0x%x. pcie_core_is_ahb_or_axi_clk_off is true", (uint32)(base+offset));
        return -1 ;
    }
    #endif
    regval = pcie_read_reg(base, offset);
    regval &= mask;
    regval >>= shift;
    return regval;
}

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

void pcie_core_initerrata(pcie_devinfo_type *pcie_dev)
{
    uint32 mask = 0;

    /* Configure CLK_CGC_DIS to 1 */
    pcie_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, CORE_CLK_CGC_DIS), 1);

    /* Configure PCIe core to support 1GB aperture */
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_SLV_ADDR_SPACE_SIZE, 0x40000000);
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_DBI_BASE_ADDR, pcie_parf_dbi_base_addr);

    /* Enable the DBI Wakeup */
    pcie_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, SLV_DBI_WAKE_DISABLE), 0);

    /* Enable CS for RO(CS) register writes */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x1);
    pcie_osal_memorybarrier();

    /* Set the PMC Register - to support PME in D0, D3hot and D3cold */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_CAP_ID_NXT_PTR_REG_1, PME_SUPPORT), 0x19);

    /* Set the Endpoint L0s Acceptable Latency to 1us (max) */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_DEVICE_CAPABILITIES_REG_1, PCIE_CAP_EP_L0S_ACCPT_LATENCY), 0x7);

    /* Set the Endpoint L1 Acceptable Latency to 2 us (max) */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_DEVICE_CAPABILITIES_REG_1, PCIE_CAP_EP_L1_ACCPT_LATENCY), 0x7);

    /* Set the L0s Exit Latency to 2us-4us = 0x6 */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CAPABILITIES_REG, PCIE_CAP_L1_EXIT_LATENCY), 0x6);

    /* Set the L1 Exit Latency to be 32us-64 us = 0x6 */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CAPABILITIES_REG, PCIE_CAP_L0S_EXIT_LATENCY), 0x6);

    /* Enable Clock Power Management */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_LINK_CAPABILITIES_REG_1, PCIE_CAP_CLOCK_POWER_MAN), 0x1);

    /* Disable CS for RO(CS) register writes */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x0);
    pcie_osal_memorybarrier();

    /* Enable writes for RO(CS2) */
    pcie_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE,0x1);
    pcie_osal_memorybarrier();

    /* Set the Common Clock L0s Exit Latency to 2us-4us = 0x6 */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CAPABILITIES_REG, PCIE_CAP_L1_EXIT_LATENCY), 0x6);

    /* Set the Common Clock L1 Exit Latency to be 32us-64 us = 0x6 */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CAPABILITIES_REG, PCIE_CAP_L0S_EXIT_LATENCY), 0x6);

    /* Disable writes for RO(CS2) */
    pcie_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE,0x0);
    pcie_osal_memorybarrier();

    /* Common_Mode_Restore_Time recommended by the PHY team:
       70us (assuming 200nF ac coupling capacitance) for Tcommonmode in Torino.
       Required Tcommonmode will be shorter if ac cap is smaller than 200nF,                                           .
       and longer if ac cap is bigger than 200nF.
       This is a HwInit(CS*) and so doesn't require CS/CS2 enable switch.
    */ 
    pcie_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(L1SUB_CAPABILITY_REG, COMM_MODE_SUPPORT), 0x46);
    pcie_osal_memorybarrier();

    /* Set both the nFTS values to 0x80 to match the PHY settings - QCTDD00661401 */
    mask = PCIE_MASK(TYPE0_ACK_F_ASPM_CTRL_REG , ACK_N_FTS) | PCIE_MASK(TYPE0_ACK_F_ASPM_CTRL_REG , COMMON_CLK_N_FTS);
    pcie_write_reg_field(pcie_dev->dbi_base, TYPE0_ACK_F_ASPM_CTRL_REG, mask, PCIE_SHFT(TYPE0_ACK_F_ASPM_CTRL_REG , ACK_N_FTS) , 0x80);
    pcie_osal_memorybarrier();
}

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
pcie_status_type pcie_core_init(pcie_devinfo_type *pcie_dev, pcie_config_type *pcie_cfg)
{
    pcie_status_type status = PCIE_STATUS_SUCCESS;

    pcie_osal_debug(MSG_HIGH,"Begin");
    if ((NULL == pcie_dev) || (NULL == pcie_cfg))
    {
        return PCIE_STATUS_INVALIDPARAMS;
    }
    
#ifdef PCIE_BOOT
   #ifdef PCIE_VIRTIO
      HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK,0X1);
   #endif

    /*Disable PCIe phy ARES*/
    HWIO_GCC_PCIEPHY_PHY_BCR_OUTM(HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_BMSK,0X0) ;

    
 
    //TODO Hack to configure BIMC for the 1GB window for PCIe AXI space
  //  p=(uint32*)0x80182220; 
  //  *p=0x80000003;  
  HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTI(0,0x80000003) ;  
  //  p=(uint32*)0x801822A0; 
   // *p=0x80000001; 
  HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTI(1,0x80000001) ;
  //  p=(uint32*)0x80182228; 
  //  *p=0xC0000000; 
  //  p=(uint32*)0x801822A8; 
  //  *p=0xC0000000; 
  HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTI(0,0xC0000000) ;
  //  p=(uint32*)0x801822A8; 
   // *p=0xC0000000; 
  
  HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTI(1,0xC0000000) ;


    /* Initialize the PHY */
    status = pcie_phy_init(pcie_dev);       
    if(PCIE_STATUS_SUCCESS != status)
    {
        return status;
    }
    
  #ifdef FEATURE_PCIE_RUMI_BRINGUP
    
 
    //HACK set TLMM_GPIO_OE_0 (ENABLE ALL THE DCM CONTROL) 
   // p=(uint32*)0xc1813080; 
   // *p=0xFFFFFFFF; 
    HWIO_TLMM_GPIO_OE_0_OUT(0xFFFFFFFF) ;

    //HACK set TLMM_GPIO_OUT_0 (SET THE GPIOS (TX PHASE SHIFT) 
   // p=(uint32*)0xc1813000; 
  //  *p=0x00012000; 
   HWIO_TLMM_GPIO_OUT_0_OUT(0x00012000) ;
  

  #endif
#endif
    /* Settings Override */
    pcie_core_initerrata(pcie_dev);
    
    /* Configure the device id and vendor id */
    pcie_core_setcoreid(pcie_dev,pcie_cfg->device_id, pcie_cfg->vendor_id);

    /* Configure the Device Type */
    pcie_core_setdevicetype(pcie_dev, pcie_cfg->dev_type);

    /* Configure the device speed */
#ifdef PCIE_BOOT
    if (PCIE_CORE_TARGETSPEED_DEFAULT != pcie_cfg->tgt_speed)
    {
        pcie_core_settargetspeed(pcie_dev,pcie_cfg->tgt_speed);
    }
#else
    pcie_core_settargetspeed(pcie_dev,pcie_cfg->tgt_speed);
    pcie_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(PCIE20_PARF_AXI_MSTR_RD_HALT_NO_WRITES,EN),0X1);
#endif
	
    pcie_osal_debug(MSG_HIGH,"Exit");	

    return status;
}

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
uint32 pcie_core_getversion(pcie_devinfo_type *pcie_dev)
{
    if (NULL == pcie_dev)
    {
        return 0x0; 
    }
    /* Read the revision register */
    return pcie_read_reg(pcie_dev->elbi_base, PCIE20_ELBI_VERSION);    
}

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
void pcie_core_setcoreid(pcie_devinfo_type *pcie_dev, uint32 device_id, uint32 vendor_id)
{
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x1);
    pcie_osal_memorybarrier();

    pcie_write_reg(pcie_dev->dbi_base,TYPE0_DEVICE_ID_VENDOR_ID_REG_1, 
        PCIE_FVAL(TYPE0_DEVICE_ID_VENDOR_ID_REG_1, PCI_TYPE0_DEVICE_ID, device_id) |
        PCIE_FVAL(TYPE0_DEVICE_ID_VENDOR_ID_REG_1, PCI_TYPE0_VENDOR_ID, vendor_id)
        );
    pcie_osal_memorybarrier();

    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x0);
    pcie_osal_memorybarrier();
}

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
void pcie_core_enableltssm(pcie_devinfo_type *pcie_dev)
{
    pcie_osal_debug(MSG_HIGH,"Begin");
    if (NULL == pcie_dev)
    {
       /*Error will manifest in link not being up. */
        pcie_osal_debug(MSG_HIGH,"Error: pcie_dev is NULL");
        return; 
    }
    /*Enable LTSSM */
    pcie_osal_debug(MSG_HIGH,"Enabling PCIe Core LTSSM");
    pcie_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_LTSSM,LTSSM_EN),1);
    pcie_osal_debug(MSG_HIGH,"Exit");
}

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
pcie_linkstatus_type pcie_core_getlinkstate(pcie_devinfo_type *pcie_dev, pcie_core_linkstate_type *link_state)
{
    uint32 link_status;
    
    if ((NULL == pcie_dev) || (NULL == link_state))
    {
        return PCIE_LINKSTATUS_DOWN; 
    }

    *link_state = (pcie_core_linkstate_type) pcie_read_reg_field(pcie_dev->elbi_base,
                                                                 PCIE_FLD(PCIE20_ELBI_SYS_STTS,XMLH_LTSSM_STATE));

    link_status = pcie_read_reg_field(pcie_dev->elbi_base, 
                                      PCIE_FLD(PCIE20_ELBI_SYS_STTS,XMLH_LINK_UP));

    return (link_status)?PCIE_LINKSTATUS_UP:PCIE_LINKSTATUS_DOWN;
}

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
boolean pcie_core_isenumerated(pcie_devinfo_type *pcie_dev)
{
    if (NULL == pcie_dev)
    {
        return FALSE; 
    }

    return (pcie_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_STATUS_COMMAND_REG_1, PCI_TYPE0_BUS_MASTER_EN)) == 1)?TRUE:FALSE;
}

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
void pcie_core_setdevicetype(pcie_devinfo_type *pcie_dev, pcie_devmode_type devtype)
{
    if (NULL == pcie_dev)
    {
        return; 
    }

    pcie_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_DEVICE_TYPE, DEVICE_TYPE),(devtype==PCIE_DEV_EP)?0:4);
}

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
pcie_devmode_type pcie_core_getdevicetype(pcie_devinfo_type *pcie_dev)
{
    uint32 devtype = 0x0;
    if (NULL != pcie_dev)
    {
        devtype = pcie_read_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_DEVICE_TYPE, DEVICE_TYPE));
    }
    return (devtype==0x0)?PCIE_DEV_EP:PCIE_DEV_RC;
}

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
void pcie_core_settargetspeed(pcie_devinfo_type *pcie_dev, pcie_targetspeed_type targetspeed)
{
    if (NULL == pcie_dev)
    {
        return;
    }
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(LINK_CONTROL2_LINK_STATUS2_REG,PCIE_CAP_TARGET_LINK_SPEED), 
        targetspeed);
}

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
pcie_targetspeed_type pcie_core_gettargetspeed(pcie_devinfo_type *pcie_dev)
{
    if (NULL == pcie_dev)
    {
        return PCIE_CORE_TARGETSPEED_GEN1;
    }
    return  (pcie_targetspeed_type)pcie_read_reg_field(pcie_dev->dbi_base, 
        PCIE_FLD(LINK_CONTROL2_LINK_STATUS2_REG,
        PCIE_CAP_TARGET_LINK_SPEED));
}

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
void pcie_core_setatuconfig(pcie_devinfo_type *pcie_dev, boolean enable, pcie_atuconfig_type *atu_config)
{

    uint32 ctrl2_value = 0;

    pcie_osal_debug(MSG_HIGH,"Begin");
    if (NULL == pcie_dev)
    {
        pcie_osal_debug(MSG_HIGH,"Error: pcie_dev is NULL");
        return;
    }
    /* 3.11 iATU Programming */

    if (PCIE_ATUDIR_INBOUND == atu_config->dir)
    {
        atu_config->index |= 0x80000000;    
    }

    /* 1. Setup the Index Register */
    pcie_write_reg(pcie_dev->dbi_base,IATU_VIEWPORT_REG, atu_config->index);
    pcie_osal_memorybarrier();

    if( PCIE_ATUMODE_BARMATCHED == atu_config->mode )
    {
        pcie_osal_debug(MSG_HIGH,"PCIe in ATU Bar Matched Mode");
        /* 2. Setup the Target Address Registers */
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_low);
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_hi);

        /* 3. Configure the region through the Region Control 1 Register. 0x0 -- Region is MEM */
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0, 0x0);

        /* 4. Enable the region for BAR Match Mode. */
        if (TRUE == enable)
        {
            ctrl2_value = PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK;
        }
        ctrl2_value |= ( PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_MASK | 
            atu_config->bar << PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_SHFT);
#ifndef PCIE_BOOT
        if (PCIE_ATUDIR_OUTBOUND == atu_config->dir)
        {
           ctrl2_value |= PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_MASK ;    
        }
#endif
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, ctrl2_value) ;
        pcie_osal_memorybarrier();
    }
    else
    {
        pcie_osal_debug(MSG_HIGH,"PCIe in ATU Address Matched Mode");
        /* 2. Setup the Region Base and Limit Address Registers. */
        pcie_write_reg(pcie_dev->dbi_base, PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0, atu_config->base_addr_low);
        pcie_write_reg(pcie_dev->dbi_base, PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0, atu_config->base_addr_hi);
        pcie_write_reg(pcie_dev->dbi_base, PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0, (atu_config->base_addr_low + atu_config->size - 1));

        /* 3. Setup the Target Address Registers. */
        pcie_write_reg(pcie_dev->dbi_base, PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_low);  
        pcie_write_reg(pcie_dev->dbi_base, PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0, atu_config->target_addr_hi);

        /* 4. Configure the region through the Region Control 1 Register. */
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0, 0x0);

        /* 5. Enable the region. */
        if (TRUE == enable)
        {
            ctrl2_value = PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK;
#ifndef PCIE_BOOT
            if (PCIE_ATUDIR_OUTBOUND == atu_config->dir)
            {
               ctrl2_value |= PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_MASK ;    
            }
#endif
        }
        pcie_write_reg(pcie_dev->dbi_base,PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, ctrl2_value) ;
        pcie_osal_memorybarrier();
    }
    pcie_osal_debug(MSG_HIGH,"Exit");
}

/* ============================================================================
**  Function : pcie_core_setbarconfig
** ============================================================================
*/
/**
  Sets the BAR Config in the PCIe Config Space 
  
  Sets the BAR Config in the PCIe Config Space. This config determines how big the
  window into the MDM is for the host.
  
  @param[in]  pcie_dev      handle to the pcie device object
  @param[in]  bar_config    Bar configuration
  
  @return
  None.  
*/
void pcie_core_setbarconfig(pcie_devinfo_type *pcie_dev, pcie_barcfg_type *bar_config )
{
    uint32 bar_reg_offset, bar_type_val;

    pcie_osal_debug(MSG_HIGH,"Begin");
    if ((NULL == pcie_dev) || (NULL == bar_config))
    {
        pcie_osal_debug(MSG_HIGH,"Error: pcie_dev or bar_config is NULL");
        return;
    }

    bar_reg_offset = TYPE0_BAR0_REG_1 + (0x4 * bar_config->bar);     
    bar_type_val = ( PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_PREFETCH, bar_config->mode) |
          PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_TYPE, bar_config->type) |
          PCIE_FVAL(TYPE0_BAR0_REG_1, BAR0_MEM_IO, bar_config->memspace));

    /* 5.1.8.11 Base Address Registers configuration */

    /* Enable Config access via CS2 */
    pcie_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE,0x1);
    pcie_osal_memorybarrier();

    /* Configure the BAR Mask */
    pcie_write_reg(pcie_dev->dbi_base,bar_reg_offset,bar_config->mask); 
    pcie_osal_memorybarrier();

    /* Disable config access via CS2 */
    pcie_write_reg(pcie_dev->elbi_base, PCIE20_ELBI_CS2_ENABLE,0x0);
    pcie_osal_memorybarrier();


    /* Enable Config access via CS */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x1);
    pcie_osal_memorybarrier();

    /* Configure the BAR */
    pcie_write_reg(pcie_dev->dbi_base,bar_reg_offset,bar_type_val); 
    pcie_osal_memorybarrier();

    /* Disable config access via CS */
    pcie_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(MISC_CONTROL_1_REG,DBI_RO_WR_EN), 0x0);
    pcie_osal_memorybarrier();

    pcie_osal_debug(MSG_HIGH,"Exit");
}


/* ============================================================================
**  Function : pcie_core_getmsiconfig
** ============================================================================
*/
/**
  Gets the MSI Config from the PCIe Config Space programmed by the host.
  
  Gets the MSI Config from the PCIe Config Space programmed by the host.
  
  @param[in]  pcie_dev      handle to the pcie device object
  @param[out]  msi_config    MSI Config data to be returned
  
  @return
  None.  
*/
void pcie_core_getmsiconfig(pcie_devinfo_type *pcie_dev, pcie_msiconfig_type *msi_config)
{
    if ((NULL == pcie_dev) || (NULL == msi_config))
    {
        return;
    }
    msi_config->enabled        = pcie_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1 , PCI_MSI_ENABLE));
    msi_config->addr_hi        = pcie_read_reg(pcie_dev->dbi_base, MSI_UPPER_32);
    msi_config->addr_low       = pcie_read_reg(pcie_dev->dbi_base, MSI_LOWER_32);
    msi_config->data           = pcie_read_reg(pcie_dev->dbi_base, MSI_DATA_REG);
    msi_config->num_multi_msgs = pcie_read_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1, PCI_MSI_MULTIPLE_MSG_EN));
}

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
void pcie_core_sendparfmsi(pcie_devinfo_type *pcie_dev, uint32 msi_vector)
{
    if (NULL == pcie_dev)
    {
        return;
    }

    /* Use the PARF method to trigger the MSI */
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_MSI_GEN,msi_vector);

}


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
void pcie_core_configmhigate(pcie_devinfo_type *pcie_dev, pcie_barcfg_type *bar_config)
{

    uint32 mhi_ctrl_val = (PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_CLK_EN_MASK | 
                           PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_CLK_EN_MASK);

    pcie_osal_debug(MSG_HIGH,"Begin");

    if ((NULL == pcie_dev) || (NULL == bar_config))
    {
        pcie_osal_debug(MSG_HIGH,"Error: pcie_dev or bar_config is NULL");
        return;
    }

    /* Enable MHI clocks */
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_CLOCK_RESET_CTRL, 
                   mhi_ctrl_val);

    /* 
      Configure the MHI base address so that inbound transactions to the 
      BHI are routed appropriately to the BHI registers within the PCIe core
     */
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_BASE_ADDR_LOWER, 
                   bar_config->local_addr_low); 
    pcie_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_BASE_ADDR_UPPER, 
                   bar_config->local_addr_hi);

    /* Clear the MHI State register as well since we are in PBL */
    pcie_write_reg(pcie_dev->mhi_base, PCIE20_MHISTATUS, 0x0);
    pcie_osal_debug(MSG_HIGH,"Exit");

}

/* ============================================================================
**  Function : pcie_core_init_clk
** ============================================================================
*/
/**
  Initializes the PCIe Clocks.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_init_clk(void)
{
   pcie_osal_clock_ctl_init(&pcie_ahb_clk_ctl,"gcc_pcie_cfg_ahb_clk") ;
   pcie_osal_clock_ctl_init(&pcie_pipe_clk_ctl,"gcc_pcie_pipe_clk") ;
   pcie_osal_clock_ctl_init(&pcie_axi_clk_ctl,"gcc_pcie_axi_clk") ;
   pcie_osal_clock_ctl_init(&pcie_sleep_clk_ctl,"gcc_pcie_sleep_clk") ;
   pcie_osal_clock_ctl_init(&pcie_axi_mstr_clk_ctl,"gcc_pcie_axi_mstr_clk") ;
   pcie_osal_clock_ctl_init(&pcie_phy_clk_ctl,"gcc_pciephy_phy_clk") ;
   

}
/* ============================================================================
**  Function : pcie_core_enable_clk
** ============================================================================
*/
/**
  Enable PCIe Core Clocks.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_enable_clk(void)
{
   pcie_osal_debug(MSG_HIGH,"Begin");
   pcie_osal_clock_ctl_enable(&pcie_ahb_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_axi_mstr_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_axi_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_pipe_clk_ctl);
   pcie_osal_set_clock_freq(&pcie_sleep_clk_ctl,1,CLOCK_FREQUENCY_MHZ_EXACT );
   pcie_osal_clock_ctl_enable(&pcie_sleep_clk_ctl);
   HWIO_GCC_PCIEPHY_PHY_BCR_OUTM(HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_BMSK,0X0) ;
   pcie_osal_debug(MSG_HIGH,"Exit");
}


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
 void pcie_core_disable_clk(void)
{
   pcie_osal_debug(MSG_HIGH,"Begin");
   pcie_osal_clock_ctl_disable(&pcie_sleep_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_pipe_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_axi_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_axi_mstr_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_ahb_clk_ctl);
   HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK,0X0);
   pcie_osal_debug(MSG_HIGH,"Exit");
}



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
void pcie_core_init_pwr_domain(void)
{
   pcie_osal_pwr_domain_init(&pcie_core_pwr_ctl,"VDD_PCIE") ;

}

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
void pcie_core_enable_pwr_domain(void)
{
   pcie_osal_pwr_domain_enable(&pcie_core_pwr_ctl) ;
}

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
void pcie_core_disable_pwr_domain(void)
{
   pcie_osal_pwr_domain_disable(&pcie_core_pwr_ctl) ;
}

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
void pcie_core_enter_lpm(void)
{
   pcie_osal_debug(MSG_HIGH,"Begin");
   pcie_osal_clock_ctl_disable(&pcie_sleep_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_pipe_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_axi_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_axi_mstr_clk_ctl);
   pcie_osal_clock_ctl_disable(&pcie_ahb_clk_ctl);
   HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK,0X0);
   pcie_osal_debug(MSG_HIGH,"Exit");
}

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
void pcie_core_exit_lpm(void)
{
   pcie_osal_debug(MSG_HIGH,"Begin");
   pcie_osal_clock_ctl_enable(&pcie_ahb_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_axi_mstr_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_axi_clk_ctl);
   pcie_osal_clock_ctl_enable(&pcie_pipe_clk_ctl);
   pcie_osal_set_clock_freq(&pcie_sleep_clk_ctl,1,CLOCK_FREQUENCY_MHZ_EXACT );
   pcie_osal_clock_ctl_enable(&pcie_sleep_clk_ctl);
   pcie_osal_debug(MSG_HIGH,"Exit");
}

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
uint32 pcie_core_is_ahb_or_axi_clk_off(void)
{
    if( (!pcie_osal_is_clock_ctl_on(&pcie_ahb_clk_ctl)) || (!pcie_osal_is_clock_ctl_on(&pcie_axi_clk_ctl)) || (!pcie_osal_is_clock_ctl_on(&pcie_axi_mstr_clk_ctl)))
    {
        pcie_osal_debug(MSG_HIGH,"Either AHB or AXI clock is Off");
        return 1;
    }
    else
    {
        pcie_osal_debug(MSG_HIGH,"AHB and AXI clocks are On");
        return 0;
    }
}
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

void pcie_core_dump(pcie_devinfo_type *pcie_dev,pcie_coredump_type *pcie_coredump,pcie_core_dump_type pcie_dump_type)
{
#ifndef  PCIE_BOOT
   uint32 i; 
   /* Check if the clock is up */
   if(pcie_osal_is_clock_ctl_on(&pcie_ahb_clk_ctl))
   {
      pcie_osal_debug (MSG_HIGH, "Dumping PARF and PHY registers");
      pcie_osal_memcpy_reg ((uint32 *) pcie_coredump->parf_regs ,(uint32 *) pcie_dev->parf_base, PCIE_PARF_SIZE);
      /* Pet the watch dog */
      dog_force_kick();
      pcie_osal_memcpy_reg ((uint32 *) pcie_coredump->phy_regs,(uint32 *) pcie_dev-> phy_base,PCIE_CM_SIZE);
      /* Pet the watch dog */
      dog_force_kick();

      pcie_osal_memcpy_reg ((uint32 *) pcie_coredump->mhi_regs,(uint32 *) pcie_dev-> mhi_base,PCIE_MHI_SIZE);
      /* Pet the watch dog */
      dog_force_kick();
	  
      /* Back up testbus */
      for(i =0; i < sizeof(pcie_coredump->test_bus)/sizeof(pcie_coredump->test_bus[0]) ; i++)
      {
         pcie_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_SYS_CTRL, TESTBUS_SELECTOR), i); 
         pcie_coredump->test_bus[i]=pcie_read_reg(pcie_dev->parf_base, PCIE20_PARF_TEST_BUS);
         dog_force_kick();
      }
   }
   else
   {
      pcie_osal_debug (MSG_HIGH, "pcie_ahb_clk_ctl is OFF. PCIe Core Dump failed.");
   }

  if(pcie_dump_type == PCIE_FULL_DUMP)
  {
      if(pcie_osal_is_clock_ctl_on(&pcie_axi_clk_ctl))
      {
         pcie_osal_debug (MSG_HIGH, "Dumping DBI and ELBI registers");
         pcie_osal_memcpy_reg ((uint32 *) pcie_coredump->dbi_regs,(uint32 *) pcie_dev->dbi_base,PCIE_DBI_SIZE);
         /* Pet the watch dog */
         dog_force_kick();
         pcie_osal_memcpy_reg ((uint32 *) pcie_coredump->elbi_regs,(uint32 *) pcie_dev->elbi_base,PCIE_ELBI_SIZE);
         /* Pet the watch dog */
         dog_force_kick();
      }
      else
      {
         pcie_osal_debug (MSG_HIGH, "pcie_axi_clk_ctl is OFF. PCIe Core Dump failed.");
      }
   }
#endif
}


#ifndef  PCIE_BOOT
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
void pcie_core_pmic_npa_client_init(void)
{
	
    pcie_vregmgr_ctx_type *ctx = &pcie_vregmgr;
	
	/* Create a synchronous request client to the PCIe PMIC resource */
    ctx->pcie_handle = npa_create_sync_client(PMIC_NPA_GROUP_ID_PCIE, "pcie_pmic_npa_client", NPA_CLIENT_REQUIRED);

	/* Set the pcie_pmic_npa_mode_requested to PCIE_NO_MODE_REQUESTED to know that no mode has been voted for yet  */
    ctx->pcie_pmic_npa_mode_requested = PCIE_NO_MODE_REQUESTED;


    if(NULL == ctx->pcie_handle)
    {
        pcie_osal_debug(MSG_FATAL,"PMIC NPA client registration failed");
    }
}

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

void pcie_core_pmic_npa_mode_request(pcie_core_pm_modes_type pcie_core_pm_mode)
{
    pcie_vregmgr_ctx_type *ctx = &pcie_vregmgr;
	
    if(NULL == ctx->pcie_handle)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_vregmgr->pcie_handle is null");
    }
    if(PCIE_OFF == pcie_core_pm_mode)
    {
	    npa_set_request_attribute(ctx->pcie_handle,
                              NPA_REQUEST_FIRE_AND_FORGET);
    }
    else
    {
	    npa_set_request_attribute(ctx->pcie_handle,
                              NPA_REQUEST_DEFAULT);
    }

	if( PCIE_OFF == pcie_core_pm_mode)
    {
        /* vote for PCIE_OFF  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_OFF);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_OFF mode requested");
    }
	else if( PCIE_SUSPEND == pcie_core_pm_mode)
    {
        /* vote for PCIE_SUSPEND  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_SUSPEND);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_SUSPEND mode requested");
    }
	else if( PCIE_ACTIVE_NOMINAL == pcie_core_pm_mode)
    {
        /* vote for PCIE_ACTIVE_NOMINAL  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL mode requested");
    }
	else if( PCIE_ACTIVE_SVS == pcie_core_pm_mode)
    {
        /* vote for PCIE_ACTIVE_SVS  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS mode requested");
    }
	else if( PCIE_ACTIVE_TURBO == pcie_core_pm_mode)
    {
        /* vote for PCIE_ACTIVE_TURBO  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO mode requested");
    }
	else if( PCIE_MAX == pcie_core_pm_mode)
    {
        /* vote for PCIE_MAX  */
        npa_issue_required_request(ctx->pcie_handle, PMIC_NPA_MODE_ID_PCIE_MAX);
	    ctx->pcie_pmic_npa_mode_requested = pcie_core_pm_mode;
        pcie_osal_debug(MSG_HIGH,"PMIC_NPA_MODE_ID_PCIE_MAX mode requested");
    }
    else
	{
        pcie_osal_debug(MSG_ERR,"Unsupported PMIC NPA mode requested");
	}
}

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
void pcie_core_clk_npa_client_init(void)
{
	
	// Create a synchronous request client to the CNOC resource
    pcie_bus_cnoc_npa_client = npa_create_sync_client("/clk/cnoc", "pcie_bus_cnoc_npa_client", NPA_CLIENT_REQUIRED);
    if(NULL == pcie_bus_cnoc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_cnoc_npa_client registration failed");
    }
}

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


void pcie_core_clk_npa_mode_request(void)
{
    /* vote for CNOC @ SVS */
	
    if(NULL == pcie_bus_cnoc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_cnoc_npa_client is null");
    }

    npa_issue_required_request(pcie_bus_cnoc_npa_client, PCIE_CNOC_SVS_MODE_SPEED_KHZ);
    pcie_osal_debug(MSG_HIGH,"PCIE_CNOC_SVS_MODE_SPEED_KHZ requested");
}
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

void pcie_core_clk_npa_mode_release(void)
{
	/* Remove votes for the CNOC */
    npa_complete_request(pcie_bus_cnoc_npa_client);
    pcie_osal_debug(MSG_HIGH,"pcie_bus_cnoc_npa_client release complete");
}

/* ============================================================================
**  Function : pcie_core_npa_clkmgr_init
** ============================================================================
*/
/**
  Initialize the NPA client by registering it with the resources.
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_core_npa_clkmgr_init(void)
{
	// Create a synchronous request client to the SNOC resource
    pcie_bus_snoc_npa_client = npa_create_sync_client("/clk/snoc", "pcie_bus_snoc_npa_client", NPA_CLIENT_REQUIRED);
    if(NULL == pcie_bus_snoc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_snoc_npa_client registration failed");
    }

	// Create a synchronous request client to the BIMC resource
    pcie_bus_bimc_npa_client = npa_create_sync_client("/clk/bimc", "pcie_bus_bimc_npa_client", NPA_CLIENT_REQUIRED);
    if(NULL == pcie_bus_bimc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_bimc_npa_client registration failed");
    }
	
}

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

void pcie_core_npa_clkmgr_config_clocks(void)
{
    /* vote for SNOC and BIMC @ NOMINAL */
	
    if(NULL == pcie_bus_snoc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_snoc_npa_client is null");
    }

    npa_issue_required_request(pcie_bus_snoc_npa_client, PCIE_SNOC_NOMINAL_MODE_SPEED_KHZ);
    pcie_osal_debug(MSG_HIGH,"PCIE_BIMC_NOMINAL_MODE_SPEED_KHZ mode requested");

    if(NULL == pcie_bus_bimc_npa_client)
	{
        pcie_osal_debug(MSG_FATAL,"pcie_bus_bimc_npa_client is null");
    }
    npa_issue_required_request(pcie_bus_bimc_npa_client, PCIE_BIMC_NOMINAL_MODE_SPEED_KHZ);
    pcie_osal_debug(MSG_HIGH,"PCIE_BIMC_NOMINAL_MODE_SPEED_KHZ mode requested");
}

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

void pcie_core_npa_clkmgr_release_clocks(void)
{
	/* Remove votes for the various Clocks */
    npa_complete_request(pcie_bus_snoc_npa_client);
    pcie_osal_debug(MSG_HIGH,"pcie_bus_snoc_npa_client request complete");
    npa_complete_request(pcie_bus_bimc_npa_client);
    pcie_osal_debug(MSG_HIGH,"pcie_bus_bimc_npa_client request complete");
}
#endif

