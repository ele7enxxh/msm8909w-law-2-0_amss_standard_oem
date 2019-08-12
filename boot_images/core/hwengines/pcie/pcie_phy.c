/** 
  @file pcie_phy.c
  @brief
  Implements the interfaces to the PCIe PHY HW 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/18/14   Dilip        Added pcie_phy_config_update
05/06/14   Dilip        Removed pcie_phy_config_update
04/30/14   Dilip        Added function pcie_phy_config_update to configure params for Aux and Sys clk
04/25/14   Dilip        Workaround for torino version 2.0 in phy config
08/05/13   Weijunz      Created
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "comdef.h"
#include "pcie_phy.h"
#include "pcie_hwio.h"


static void pcie_phy_config(pcie_devinfo_type *pcie_dev) ;
/* ============================================================================
**  Function : pcie_phy_config
** ============================================================================
*/
/**
  Config  the PHY.
    
  Starts the PHY.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  None.
*/
static void pcie_phy_config(pcie_devinfo_type *pcie_dev)
{

    pcie_osal_debug(MSG_HIGH,"Begin"); 
    /*QMP PCIE 20nm PHY 100 MHz Power-up Sequence */

    pcie_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_PCIE_USB3_PHY_POWER_DOWN_CONTROL, 0x01); 

    /*  100MHz Setup */
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_SYS_CLK_CTRL,0x06);
    
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_CP_SETI,0x11);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_IP_SETP,0x3F);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_CP_SETP,0x00);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_IP_SETI,0x3F) ;

    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_IE_TRIM,0x0F) ;
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_IP_TRIM,0x0F);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_CNTRL,0x06);

    /* ; --- CDR Settings %LONG */
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_CDR_CONTROL1,0xF3);

    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_CDR_CONTROL_HALF, 0x2B);

    /*  --- Calibration Settings */
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RESETSM_CNTRL, 0x90);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RESETSM_CNTRL2,0x05);

    /*  Additional writes */
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RES_CODE_START_SEG1, 0x20);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RES_CODE_CAL_CSR, 0x77);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RES_TRIM_CONTROL, 0x15);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_TX_RCV_DETECT_LVL,0x03);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQ_GAIN1_LSB, 0xFF);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQ_GAIN1_MSB, 0x1F);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQ_GAIN2_LSB, 0xFF);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQ_GAIN2_MSB, 0x00);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2,0x1A);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1, 0x80);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_SIGDET_ENABLES, 0x40);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_SIGDET_CNTRL ,0x70);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_SIGDET_DEGLITCH_CNTRL, 0x06);
    pcie_write_reg(pcie_dev->phy_base,PCIE_USB3_PCS_PCIE_USB3_PHY_PWRUP_RESET_DLY_TIME_SYSCLK, 0xC8);
    pcie_write_reg(pcie_dev->phy_base,PCIE_USB3_PCS_PCIE_USB3_PHY_POWER_STATE_CONFIG1, 0xA3);
    pcie_write_reg(pcie_dev->phy_base,PCIE_USB3_PCS_PCIE_USB3_PHY_POWER_STATE_CONFIG2, 0x1B);
    pcie_write_reg(pcie_dev->phy_base,QSERDES_RX_RX_RCVR_IQ_EN,0x31);

    pcie_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_PCIE_USB3_PHY_PWRUP_RESET_DLY_TIME_AUXCLK, 0x80);
	
    if(pcie_dev->dev_version == DALCHIPINFO_VERSION(2,0))
    {
        /* Workaround for torino version 2.0 */
	    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_PLL_VCOTAIL_EN,0xE1);
	    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_RESETSM_CNTRL2,0x7);
    }

    pcie_osal_debug(MSG_HIGH,"Exit"); 
}

/* ============================================================================
**  Function : pcie_phy_start
** ============================================================================
*/
/**
  Starts the PHY.
    
  Starts the PHY.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  None.
*/

static void pcie_phy_start(pcie_devinfo_type *pcie_dev)
{
   pcie_osal_debug(MSG_HIGH,"Begin"); 
   pcie_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_PCIE_USB3_PHY_SW_RESET, 0x00);
   pcie_write_reg(pcie_dev->phy_base, PCIE_USB3_PCS_PCIE_USB3_PHY_START,
                   PCIE_MASK(PCIE_USB3_PCS_PCIE_USB3_PHY_START, PCS_START) | PCIE_MASK(PCIE_USB3_PCS_PCIE_USB3_PHY_START, SERDES_START));
   pcie_osal_debug(MSG_HIGH,"Exit"); 
   
}

/* ============================================================================
**  Function : pcie_phy_isready
** ============================================================================
*/
/**
  Returns whether the PHY is ready or not.
    
  Returns whether the PHY is ready or not.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  TRUE - PHY is ready; FALSE - otherwise.
  
*/
static int pcie_phy_isready(pcie_devinfo_type *pcie_dev)
{
    /* poll de-assertion(logic 0) of PHYSTATUS, which indicates PHY is ready*/
    return (0 == pcie_read_reg_field(pcie_dev->phy_base, PCIE_FLD(PCIE_USB3_PCS_PCIE_USB3_PHY_PCS_STATUS, PHYSTATUS)));
}

/* ============================================================================
**  Function : pcie_phy_waitforready
** ============================================================================
*/
/**
  Waits for Phy to stabilize and become ready for operation.
    
  Waits for Phy to stabilize and become ready for operation.
    
  @param[in]  pcie_dev    handle to the pcie device object

  @return
  TRUE iff Phy is ready before or as the timeout expired.
*/

static boolean pcie_phy_waitforready(pcie_devinfo_type *pcie_dev)
{
    pcie_osal_debug(MSG_HIGH,"Begin");
    while (!pcie_phy_isready(pcie_dev)); 
    pcie_osal_debug(MSG_HIGH,"Exit"); 
    return pcie_phy_isready(pcie_dev);
}


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
pcie_status_type pcie_phy_init(pcie_devinfo_type *pcie_dev)
{

   pcie_osal_debug(MSG_HIGH,"Begin"); 
   #ifdef PCIE_VIRTIO
   /*No PCIE PHY support in Virtio */
   return PCIE_STATUS_SUCCESS; 
   #endif  
   pcie_phy_config(pcie_dev);
   pcie_phy_start(pcie_dev);
   if(FALSE == pcie_phy_waitforready(pcie_dev))
   {
      pcie_osal_debug(MSG_HIGH,"Exit: PHY INIT FAILED"); 
      return PCIE_STATUS_PHYINITFAILED;
   }
   pcie_osal_debug(MSG_HIGH,"Exit"); 
   return PCIE_STATUS_SUCCESS;
}

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
void pcie_phy_config_update(pcie_devinfo_type *pcie_dev)
{
#ifdef PCIE_TN
    /* Update Phy additional settings */
    pcie_write_reg(pcie_dev->phy_base,QSERDES_COM_SYS_CLK_CTRL,0x06);
    pcie_write_reg(pcie_dev->phy_base, PCIE_USB3_PHY_PWRUP_RESET_DLY_TIME_AUXCLK, 0x80);
    pcie_osal_memorybarrier();
#endif
}

