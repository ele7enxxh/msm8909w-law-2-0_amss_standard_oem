//============================================================================
/**
 * @file        qhsusb_dci_8952.c
 * @author      shreyasr
 * @date        07-December-2012
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) PHY specific handling.
 *
 * @details      There might be different PHY types for different targets.
 *               When using 3-wired Full-Speed PHY the USB Core (Link) cannot conntrol the D+/D-.
 * 
 * @note 
 * 
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 *              Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_8952.c#2 $$DateTime: 2016/03/11 02:15:02 $$Author: pwbldsvc $
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2014-10-10   shreyasr  Initial draft for MSM8952
// 
// ===========================================================================

// #define DEBUG_ON_HAWK 1

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "qhsusb_hwio_dependency_8952.h"                             

#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "qhsusb_dci_api.h"         // DCI API Prototypes
#include "qhsusb_dci_private.h"     // dQH_t
#include "qhsusb_dci_hw_registers.h"    // For Multi-Core
#include "qhsusb_dci_hw_28nm_phy_registers.h"
#include "qhsusb_dci_8952_hw_apis.h"  // HW target specific access. 
#include "qhsusb_log.h"                                                     
//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
/* Total delay for Forced  EDL entry will come to (CNT * DELAY)  + ( CNT * 600us delay in ulpi read ) */
#define QHSUSB_LINESTATE_CHECK_DELAY (500)
#define QHSUSB_LINESTATE_CHECK_RETRY_CNT (25)

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
/* Enum of clock regimes that can be enabled and disabled. This is a subset
   of the complete set of hardware clock regimes. 
   The order of the items in this enum is not important. */
typedef enum
{
  CLK_RGM_USB_M,                /* Combined USB clock regimes, this resets the Link Only */
  CLK_RGM_USB_PHY_RESET_M,      /* USB PHY RESET regime, this resets the PHY, Wrapper and ULPI bridge */
  CLK_RGM_USB_PHY_POR_RESET_M   /* This resets the PHY only, typical usage is to do this after changing PHY tune params */ 
} clkrgm_msm_clk_type;


//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
core_info_t dci_core_info[DCI_MAX_CORES];
uint32 dci_core_base_address[DCI_MAX_CORES] = {USB2_HSIC_USB_OTG_HS_BASE_REG_BASE};

#define HS_USB_DLOAD_INFO_ADDR_IN_IMEM 	  (SHARED_IMEM_USB_BASE)

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------
static boolean dci_is_connected = FALSE;
static boolean dci_is_attached = FALSE;    // debug

static qhsusb_chg_port_type chg_port_type = QHSUSB_CHG_PORT_UNKNOWN;

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------
//============================================================================
/**
 * @function  qhsusb_dci_is_otg_cable_connected
 *
 * @brief     This API determines whether there is an OTG cable connected to the
 *            device
 *
 * @Note :    When OTG cable is connected the Forced EDL download mode shows
 *            an intermittent behaviour. 
 *            There is no on the fly ID detection supported in SBL
 *            This can detect the presence/absence of OTG cable only when it is 
 *            connected in HLOS and someone tries the device reboot. 
 *
 * @param     None   
 *
 * @return boolean ( TRUE if connected, else FALSE )
 *
 *
 */
//============================================================================
static boolean qhsusb_dci_is_otg_cable_connected(void)
{
    boolean id_grounded = HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_INM(HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_1_0_BMSK) >> HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_1_0_SHFT;
    boolean id_floating = HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_INM(HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_0_1_BMSK) >> HWIO_USB2_FEMTO_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_0_1_SHFT; 

    // This condition should not occur
    if(id_grounded == id_floating) 
    {
      qhsusb_log(QHSUSB_DETECT_ID_ERROR_LOG, 0, id_grounded);
    }

    return id_grounded;
}

//============================================================================
/**
 * @function  qhsusb_dci_is_attached
 *
 * @brief Report if a USB cable is attached to the device.
 *
 * @Note :  Although attached, the host communicates with the device only after 
 *          the device is *connected* to the bus (D+ pull-up).
 *
 * @Note : In Device Mode:
 *      1=Attached. 0=Not Attached. Default=0. 
 *      A one indicates that the device successfully attached
 *      and is operating in either high speed or full speed as indicated by the High Speed Port bit in
 *      this register. 
 *      A zero indicates that the device did not attach successfully or was forcibly
 *      disconnected by the software writing a zero to the Run bit in the USBCMD register. 
 *      It does not state the device being disconnected or suspended.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , CCS bit.
 *
 */
//============================================================================
boolean qhsusb_dci_is_attached(uint32 core_id)
{
    uint32 attached = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_CCS_BMSK);
    attached = VAL_TO_BOOL(attached);
    (void)dci_is_attached;
    dci_is_attached = (boolean)attached;

   return((boolean)attached);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_wall_charger
 *
 * @brief check if Wall-Charger detected.
 *
 * @Note :  USB-Charging is not supported in SCMM PBL, only software download!.
 *
 * @Note :  USB-Wall-Charger can be detected by the PHY. See ULPI-PHY spec.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "USB2ULPItpl_45nm-b - USB2.0 OTG ULPI":
 *      Table 5-10 Charger Detection Interface
 *
 */
//============================================================================
boolean qhsusb_dci_is_wall_charger(uint32 core_id)
{
    return(FALSE);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_connected
 *
 * @brief Report if the device is connected to the bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
boolean qhsusb_dci_is_connected(uint32 core_id)
{
    return(dci_is_connected);
}

//============================================================================
/**
 * @function  qhsusb_dci_connect
 *
 * @brief Connect the device to the USB bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
void qhsusb_dci_connect(uint32 core_id)
{
    dci_is_connected = TRUE; 

    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_RUN_FVAL); 
}

//============================================================================
/**
 * @function  qhsusb_dci_disconnect
 *
 * @brief Disconnect the device from the USB bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
void    qhsusb_dci_disconnect(uint32 core_id)
{
    dci_is_connected = FALSE;

    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_STOP_FVAL); 
}

// ===========================================================================
/**
 * @function    qhsusb_get_shared_imem_address
 * 
 * @brief   	Gets the shared imem address based on the hardware version
 * 
 * @note    	None          
 *  
 * @param   	None
 *
 * @return 		uint32 A 32-Bit Address
 *
 * @ref 
 *
 */
// ===========================================================================
uint32 qhsusb_get_shared_imem_address()
{
	return HS_USB_DLOAD_INFO_ADDR_IN_IMEM;
}


//============================================================================
/**
 * @function  dci_clk_regime_msm_reset_assert
 *
 * @brief     Assert CLK reset
 *
 * @Note : Local copy of CLK API to avoid build & Link of CLK in PBL (Boot team).
 *
 * @param regime    
 *
 * @return none
 *
 */
//============================================================================
void dci_clk_regime_msm_reset_assert(clkrgm_msm_clk_type regime)
{
  switch(regime)
  {
    case CLK_RGM_USB_M:
      /* Link reset also triggers PHY and ULPI bridge reset. */
      HWIO_OUTF(GCC_USB_HS_BCR, BLK_ARES, 1);
      break;

    case CLK_RGM_USB_PHY_RESET_M: 
      /* Resets the PHY, PHY Wrapper, ULPI Bridge */ 
      HWIO_OUTF(GCC_QUSB2_PHY_BCR, BLK_ARES, 1); 
      break;

    case CLK_RGM_USB_PHY_POR_RESET_M:
      /* Resets the PHY only. Basically achieving a PHY-POR */ 
      HWIO_OUTF(GCC_USB2_HS_PHY_ONLY_BCR, BLK_ARES, 1); 

    default:
      /* Reset not supported for this item. */
      break;
  }
}
//============================================================================

//============================================================================
/**
 * @function  dci_clk_regime_msm_reset_deassert
 *
 * @brief     De-Assert CLK reset
 *
 * @Note : Local copy of CLK API to avoid build & Link of CLK in PBL (Boot team).
 *
 * @param regime    
 *
 * @return none
 *
 */
//============================================================================
void dci_clk_regime_msm_reset_deassert(clkrgm_msm_clk_type regime)
{
  /* ------------------------------------------------------------------------
     Reset specified clock regime.
  ------------------------------------------------------------------------ */
  switch(regime)
  {
    case CLK_RGM_USB_M:
      /* Link reset also triggers PHY and ULPI bridge reset. */
      HWIO_OUTF(GCC_USB_HS_BCR, BLK_ARES, 0);
      break;

    case CLK_RGM_USB_PHY_RESET_M: 
      /* Resets the PHY, PHY Wrapper, ULPI Bridge */ 
      HWIO_OUTF(GCC_QUSB2_PHY_BCR, BLK_ARES, 0); 
      break;

    case CLK_RGM_USB_PHY_POR_RESET_M:
      /* Resets the PHY only. Basically achieving a PHY-POR */ 
      HWIO_OUTF(GCC_USB2_HS_PHY_ONLY_BCR, BLK_ARES, 0);    
    
    default:
      /* Reset not supported for this item. */
      break;
  }
}
//============================================================================
/**
 * @function  dci_enable_external_vbus_config
 *
 * Enables external VBUS configuration. This in turn manually drives D+ pull-up.
 * 
 * 
 * 
 * @param core id
 *        
 *        
 *  
 * @return none
 */
//=============================================================================
static void dci_enable_external_vbus_config(uint32 core_id)
{
	uint32 write_val = 0;

	write_val = USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_SET | USB_OTG_HS_MISC_A_VBUSVLDEXT_SET;

	// Select and enable external vbus configuration in the PHY.
	qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_MISC_A_REG_ADDR, write_val, 0xFF); 

	// Set chicken bit in USB configuration register for enabling session valid.
	DCI_HWIO_OUTM(core_id,USB_OTG_HS_GEN_CONFIG2, USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_BMSK, 1 << USB_OTG_HS_GEN_CONFIG2_SESS_VLD_CTRL_EN_SHFT); 
	
	// Enable external vbus configuration in the LINK.
	DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD, USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK, 1 << USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT);
}

#ifndef FEATURE_RUMI_BOOT
//============================================================================
/**
 * @function  qhsusb_dci_configure_phy_tuning_parameters
 *
 * This function tunes all the PHY tuning parameters using the ULPI Bridge.
 * Currently all the PHY tuning parameters writing happen only using the
 * ULPI Bridge. 
 * AHB2PHY mode for tuning them is currently disabled. 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none
 */
//=============================================================================
static void qhsusb_dci_configure_phy_tuning_parameters(uint32 core_id)
{
    // Skip PHY Tune since it is redundantly writing power-on default value
#if 0
    /* Change the PHY Tuning Parameters */ 
    qhsusb_dci_ulpi_write(core_id, USB_OTG_PARAMETER_OVERRIDE_A_ADDR, 0x33, 0xFF);
    qhsusb_dci_ulpi_write(core_id, USB_OTG_PARAMETER_OVERRIDE_B_ADDR, 0x33, 0xFF);
    qhsusb_dci_ulpi_write(core_id, USB_OTG_PARAMETER_OVERRIDE_C_ADDR, 0x07, 0xFF);
    qhsusb_dci_ulpi_write(core_id, USB_OTG_PARAMETER_OVERRIDE_D_ADDR, 0x13, 0xFF);

    qhsusb_dci_delay_us(75);
#endif
}
#endif
//============================================================================
/**
 * @function  qhsusb_dci_ulpi_read_with_err_check
 *
 * @brief write to the ulpi register
 *
 * @Note : For performing tasks that require ULPI reads eg. charger detection
 *          
 * @param ulpi_reg 
 *           ULPI register from which to read
 *        uint32 
 *           [OUT] value read
 *
 * @return TRUE if ULPI read was success, FALSE if ULPI timeout
 *
 * @ref 
 */
//============================================================================
static boolean qhsusb_dci_ulpi_read_with_err_check(uint32 core_id, uint32 ulpi_reg, uint32* value_ptr)
{
    uint32 ulpi_cmd = 0;
    uint32 read_val = 0;
    uint32 loop_count = 0;
    uint32 temp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    if (NULL == value_ptr)
    {
      return FALSE;
    }

    ulpi_cmd = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK +
               ( ulpi_reg << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT );   

    DCI_HWIO_OUT(core_id, USB_OTG_HS_ULPI_VIEWPORT, ulpi_cmd);

    /* poll until the run bit transitions to 0. */
    while((temp == USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK) && (loop_count++ < QHSUSB_UPLI_LOOP_COUNT))
    {
        read_val = DCI_HWIO_IN(core_id, USB_OTG_HS_ULPI_VIEWPORT) ;
        temp = read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;
        qhsusb_dci_delay_us(10); /* poll for 10us */
    }

    /* Check if ULPI Viewport timed out */
    if (loop_count >= QHSUSB_UPLI_LOOP_COUNT)
    {
      return FALSE;
    }
    
    *value_ptr = (read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK) >> USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT;

    return TRUE;
}

//============================================================================
/**
 * @function  qhsusb_dci_usb_hw_reset
 *
 * This function resets USB HW through GCC RESET and puts it into ULPI mode
 * 
 * @param none
 *  
 * @return none
 */
//=============================================================================
static void qhsusb_dci_usb_hw_reset(void)
{
  /*Add USB clock disable call*/
  qhsusb_dci_clk_disable();
  
  /* Force the Link Reset */ 
  dci_clk_regime_msm_reset_assert(CLK_RGM_USB_M);
  
  /* Force the PHY Reset (i.e PHY, PHY Wrapper and ULPI Bridge) */
  dci_clk_regime_msm_reset_assert(CLK_RGM_USB_PHY_RESET_M);
  
  /* Wait for 10usec for the Reset to propagate */
  qhsusb_dci_delay_us(10);
  
  dci_clk_regime_msm_reset_deassert(CLK_RGM_USB_PHY_RESET_M);
  
  qhsusb_dci_delay_us(75);
  
  dci_clk_regime_msm_reset_deassert(CLK_RGM_USB_M);
  
  /* Wait for phy/USB core to come out of reset */
  qhsusb_dci_delay_us(10);
  
  /* Enable USB clocks */    
  qhsusb_dci_clk_enable();
  
  /* set core to ULPI mode */
  DCI_HWIO_OUT(core_id,USB_OTG_HS_PORTSC, 0x80000000);

  /* Wait for ULPI mode the settle */
  qhsusb_dci_delay_us(10);
}

//=============================================================================
/**
 * @function  dci_init_phy
 *
 * @brief Initialize the PHY.
 *
 * @Note : Power and clock should be provided by the Boot Team or by the Hardware default.
 *
 * @Note : The PHY D+/D- IO pad strength needs to be calibrated.
 *
 * @Note : Internal Power (battery) should be used until calibartion is completed.
 *
 * @Note : For development , the code was tested on Hawk (before SCMM Tape-Out).
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , PHY type selection.
 *      Chapter 3.6.12 ULPI VIEWPORT , control the PHY from the USB Core.
 *
 */
//============================================================================

void dci_init_phy(uint32 core_id)
{
    dci_core_info[0].phy_type = DCI_PHY_TYPE_ULPI;  

    qhsusb_dci_usb_hw_reset();

#ifndef FEATURE_RUMI_BOOT
    /* Configure the PHY tuning parameters */ 
    qhsusb_dci_configure_phy_tuning_parameters(core_id);

    /* Configure D+ pull-up to be driven manually */
    dci_enable_external_vbus_config(core_id);
#endif
} // dci_init_phy

boolean qhsusb_dci_check_for_pbl_dload(uint32 core_id)
{
#ifdef FEATURE_FORCED_EDL_MODE_DISABLE
  //for Emulation and simulation this feature needs to be 
  //disabled
  qhsusb_log(QHSUSB_PBL_DLOAD_FEATURE_DISABLED_LOG, 0, 0);
  (void)qhsusb_dci_is_otg_cable_connected;
  (void)dci_enable_external_vbus_config; 
  return FALSE; 
#else
  uint32 linestate = 0;
  int32  retries   = 0;


  qhsusb_dci_usb_hw_reset();


  /* Check for OTG Cable's presence */
  qhsusb_dci_delay_us(10);
  if(qhsusb_dci_is_otg_cable_connected())
  {
    qhsusb_log(QHSUSB_SKIP_PBL_DLOAD_OTG_CABLE_CONNECTED_LOG,0,0);
    return FALSE;
  }
    
  /* Configure D+ pull-up to be driven manually */
  dci_enable_external_vbus_config(core_id);

  /* Disable the D+ pull-down */
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, 
  	USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_DISABLE,USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_MASK);

  /* Select FS transceiver and enable 1.5K pull-up. Refer to Table 41 of ULPI specification 1.1 for more details */
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_FNCTL_REG_ADDR, 
  	USB_OTG_HS_FNCTL_XCVRSELECT_FS_VAL | USB_OTG_HS_FNCTL_TERMSELECT_ENABLE_VAL | USB_OTG_HS_FNCTL_SUSPM_POWERED_VAL, 0xFF);

  /* Ensure that the D+ line is actually grounded (for EDL) by waiting ~50ms, otherwise it'll boot up immidiately */ 
  do
  {
    /* Wait for 500 microseconds */
    qhsusb_dci_delay_us(QHSUSB_LINESTATE_CHECK_DELAY);

    /* Read linestate from PHY debug register */

    if (FALSE == qhsusb_dci_ulpi_read_with_err_check(core_id, USB_OTG_HS_DEBUG_REG_ADDR, &linestate))
    {
      /* Perform GCC RESET to recover ULPI lock-up, and do not enter EDL */
      qhsusb_log(QHSUSB_SKIP_PBL_DLOAD_LOG,0,__LINE__);
      qhsusb_dci_usb_hw_reset();

      return FALSE;
    }

    /*D+ is still High, the Device should not enter DLOAD mode */
    if(linestate != 0)
    {
      break;
    }

    retries++;
  } while(retries < QHSUSB_LINESTATE_CHECK_RETRY_CNT);

  /* Disable the 1.5K pull-up resistor */
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_FNCTL_REG_ADDR, 
  	USB_OTG_HS_FNCTL_XCVRSELECT_FS_VAL | USB_OTG_HS_FNCTL_SUSPM_POWERED_VAL, 0xFF);

  /* Re-enable the D+ pull-down by setting OTG control register to defaults */
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_REG_RST_VAL, 0xFF);

  if(linestate != 0)
  {
    qhsusb_log(QHSUSB_SKIP_PBL_DLOAD_LOG,0,linestate);
    return FALSE;
  }
  else
  {
    qhsusb_log(QHSUSB_ENTER_PBL_DLOAD_LOG,0,linestate);
    return TRUE;
  }
#endif /*QHSUSB_PBL_DLOAD_FEATURE_DISABLED_LOG*/
}

//============================================================================
/**
 * @function  qhsusb_dci_soft_reset_core
 *
 * @brief This is used the Reset the USB
 *
 *
 * @param core_id    
 *
 * @return none
 *
 */
//============================================================================
void qhsusb_dci_soft_reset_core(uint32 core_id)
{
  /*Set Bit 1 of USBCMD Register for a Reset*/
  DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RST_BMSK,0x1<<USB_OTG_HS_USBCMD_RST_SHFT);
  /*Wait till the RESET bit is not zero*/
  while(0!=(DCI_HWIO_IN(core_id,USB_OTG_HS_USBCMD)&USB_OTG_HS_USBCMD_RST_BMSK));  
  return; 
}

//============================================================================
/**
 * @function  dci_dcache_inval
 *
 * @brief Invalidate Data-Cache
 *
 * @note: Data-Cache is disabled in SCMM ROM code (PBL) but used in Hawk.
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void dci_dcache_inval(uint32 *addr, uint32 length)
{
	qhsusb_dci_dcache_inval(addr, length);
}

//============================================================================
/**
 * @function  dci_dcache_flush
 *
 * @brief Flush Data-Cache
 *
 * @note: Data-Cache is disabled in SCMM ROM code (PBL) but used in Hawk.
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void dci_dcache_flush(uint32 *addr, uint32 length)
{
	qhsusb_dci_dcache_flush(addr, length);
}
//============================================================================
/**
 * @function  dci_reset_charger_detection_block
 *
 * Resets the charger detection block - control and output registers
 * 
 * 
 * 
 * @param core id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_reset_charger_detection_block(uint32 core_id)
{
    // Set the Charger Detection Control Register to Power-on Reset
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_REG_RST_VAL, 0xFF);
    
    // Set the Charger Detection Output Register to Power-on Reset
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_OUT_REG_ADDR, USB_OTG_HS_CHGDET_OUT_REG_RST_VAL, 0xFF);

    // Allow signals to settle
    qhsusb_dci_delay_us(100);
}

//============================================================================
/**
 * @function  dci_reset_alternate_interrupt_block
 *
 * Resets the alternate interrupt block - latch and enable registers
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_reset_alternate_interrupt_block(uint32 core_id)
{
    // Set the Interrupt Latch Register to Power-on Reset
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_ALTINTR_LATCH_REG_ADDR, USB_OTG_HS_ALTINTR_LATCH_REG_RST_VAL, 0xFF);
    
    // Set the Interrupt Enable Register to Power-on Reset
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_ALTINTR_ENABLE_REG_ADDR, USB_OTG_HS_ALTINTR_ENABLE_REG_RST_VAL, 0xFF);

    // Allow signals to settle
    qhsusb_dci_delay_us(100);
}

//============================================================================
/**
 * @function  dci_set_phy_mode
 *
 * Sets the PHY mode
 * 
 * 
 * 
 * @param core_id, phy mode
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_set_phy_mode(uint32 core_id, qhsusb_phy_mode_type phymode)
{
    if ( phymode == QHSUSB_CHG_PHY_MODE_NORMAL )
    {
       // Enable D+ and D- pull-down (default state).  Otherwise D+ will be pulled high during DRIVE MODE
       qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_ENABLE,USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_MASK);
       qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_ENABLE,USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_MASK);

       // Put back into DRIVE MODE
       qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_FNCTL_REG_ADDR, USB_OTG_HS_FNCTL_OPMODE_NORMAL_VAL, USB_OTG_HS_FNCTL_OPMODE_MASK);
    }
    else if ( phymode == QHSUSB_CHG_PHY_MODE_NONDRIVE )
    {
        qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_FNCTL_REG_ADDR, USB_OTG_HS_FNCTL_OPMODE_NONDRIVE_VAL, USB_OTG_HS_FNCTL_OPMODE_MASK);
    }

}

//============================================================================
/**
 * @function  dci_enable_ulpi_control_for_charger_block
 *
 * Enables ULPI control of the charger detection block in the PHY
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
void dci_enable_ulpi_control_for_charger_block(uint32 core_id)
{
    // Control Charger Block through ULPI 
}

//============================================================================
/**
 * @function  dci_enable_dcd_circuit
 *
 * Enables Data Contact Detect circuitry in the PHY
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_enable_dcd_circuit(uint32 core_id)
{
  /*  
  From the 28nm Charger Detection Spec :
  1 > Reset the Charger Detection and Alternate Interrupt blocks.
  2 > Clear DCDENB to ensure DCDOUT is cleared by hw.  
  3 > Disable Dp/Dm pull down. 
  4 > Initiate DCD by setting DCDENB to 1.
  5 > Initiate ACA detection : ACAENB = 0b1
  */

  // 1 > As a cleanup step reset the Charger Detection and Alternate Interrupt blocks to Power-On default values.
  // This will clear DCDENB and DCDOUT.
  dci_reset_charger_detection_block(core_id);
  dci_reset_alternate_interrupt_block(core_id);

  // 3 > Disable Dp pull down
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_DISABLE,USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_MASK);
  
  // For DCP detection, enable the D- pull down here (already enable by default)
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_ENABLE,USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_MASK);

  // 4 > Initiate DCD : DCDENB = 0b1
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_DCDENB_SET, USB_OTG_HS_CHGDET_CTL_DCDENB_MASK);

  // 5 > Initiate ACA detection : ACAENB = 0b1
  qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_ACAENB_SET, USB_OTG_HS_CHGDET_CTL_ACAENB_MASK);

  // Wait for 10ms
  qhsusb_dci_delay_ms(10);

}

//============================================================================
/**
 * @function  dci_disable_dcd_circuit
 *
 * Turns off Data Contact Detect circuitry in the PHY
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_disable_dcd_circuit(uint32 core_id)
{
    /*  
    1 > Clear DCDENB
    2 > Clear ACAENB
    3 > Disable the D - Pull down
    */

    // 1 > Disable DCD : DCDENB = 0b0
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_DCDENB_RST, USB_OTG_HS_CHGDET_CTL_DCDENB_MASK);

    // 2 > Disable ACA detection : ACAENB = 0b0
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_ACAENB_RST, USB_OTG_HS_CHGDET_CTL_ACAENB_MASK);

    // Wait for 10ms
    qhsusb_dci_delay_ms(10);

    // 3 > Disable the Dm Pulldown which was enabled
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_OTG_CONTROL_REG_ADDR, USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_DISABLE,USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_MASK);
}

//============================================================================
/**
 * @function  dci_enable_primary_charger_detection
 *
 * Enables Primary charger detection
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return none.
 */
//============================================================================
static void dci_enable_primary_charger_detection(uint32 core_id)
{
    // 1 > As a cleanup step reset the Charger Detection and Alternate Interrupt blocks to Power-On default values.
    // Sets CHRGSEL and VDATSRCAUTO to 0
    dci_reset_charger_detection_block(core_id);

    dci_reset_alternate_interrupt_block(core_id);
    
    // 2 > Set VDATSRCENB = 0b1
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_VDATSRCENB_SET, USB_OTG_HS_CHGDET_CTL_VDATSRCENB_MASK);
    
    // 3 > Set VDATDETENB = 0b1
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_VDATDETENB_SET, USB_OTG_HS_CHGDET_CTL_VDATDETENB_MASK);

    // 4 > Wait for 40ms(per spec) + 10ms: For CHGDET or ID bits to be ready
    qhsusb_dci_delay_ms(50);

    // Check CHGDET and ID bits reset the blocks

}
//============================================================================
/**
* @function  dci_disable_primary_charger_detection
*
* Disables Primary charger detection
*
*
*
* @param core_id
*        
*        
*  
* @return none.
*/
//============================================================================
static void dci_disable_primary_charger_detection(uint32 core_id)
{
    // 1 > As a cleanup step reset the Charger Detection and Alternate Interrupt blocks to Power-On default values.
    // Sets CHRGSEL and VDATSRCAUTO to 0
    dci_reset_charger_detection_block(core_id);

    dci_reset_alternate_interrupt_block(core_id);
    
    // 2 > Set VDATSRCENB = 0b0
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_VDATSRCENB_RST, USB_OTG_HS_CHGDET_CTL_VDATSRCENB_MASK);

    // 3 > Set VDATDETENB = 0b0
    qhsusb_dci_ulpi_write(core_id, USB_OTG_HS_CHGDET_CTL_REG_ADDR, USB_OTG_HS_CHGDET_CTL_VDATDETENB_RST, USB_OTG_HS_CHGDET_CTL_VDATDETENB_MASK);
  
    // 4 > Wait for 40ms(per spec) + 10ms: For CHGDET or ID bits to be ready
    qhsusb_dci_delay_ms(50);
 
    // Check CHGDET and ID bits reset the blocks
 
}
//=============================================================================
/**
 * @function  dci_read_dcdout_output
 *
 * Returns the value read from DCDOUT bit from the charger output register
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return value of DCDOUT bit.
 */
//=============================================================================
static uint32 dci_read_dcdout_output(uint32 core_id)
{
    /* Assumes that the CHGDET output is stable */
    uint32 read_val = 0;

    // Read the value of the Charger Detection Control Register.
    read_val = qhsusb_dci_ulpi_read(core_id,USB_OTG_HS_CHGDET_OUT_REG_ADDR);

    read_val = (read_val & USB_OTG_HS_CHGDET_OUT_DCDOUT_MASK) >> USB_OTG_HS_CHGDET_OUT_DCDOUT_POS;

    return read_val;
}

//=============================================================================
/**
 * @function  dci_read_chgdet_output
 *
 * Returns the value read from CHGDET bit from the charger output register
 * 
 * 
 * 
 * @param core_id
 *        
 *        
 *  
 * @return value of CHGDET bit
 */
//=============================================================================
static uint32 dci_read_chgdet_output(uint32 core_id)
{
    /* Assumes that the CHGDET output is stable */
    uint32 read_val = 0;

    // Read the value of the Charger Detection Control Register.
    read_val = qhsusb_dci_ulpi_read(core_id,USB_OTG_HS_CHGDET_OUT_REG_ADDR);
   
    read_val = (read_val & USB_OTG_HS_CHGDET_OUT_CHGDET_MASK) >> USB_OTG_HS_CHGDET_OUT_CHGDET_POS;

    return read_val;
}

//=============================================================================
/**
 * @function  dci_detect_charger_type
 *
 * Detects charger port type
 * 
 * Should be called after usb stack initialization
 * 
 * @param core_id
 *        
 *        
 *  
 * @return Charger port type.
 */
//=============================================================================
uint32 dci_detect_charger_type(uint32 core_id)
{
    boolean chg_det = 0;
    boolean dcd_out = 0;
    uint32 loop_count = 0;
    uint32 linestate =0;

    qhsusb_chg_state_type chg_sm_state = QHSUSB_CHG_ST_BSESS_VALID;
    chg_port_type = QHSUSB_CHG_PORT_UNKNOWN;

    while((chg_sm_state != QHSUSB_CHG_ST_END) && (loop_count++ < QHSUSB_CHG_DETECTION_LOOP_COUNT))
    {
        qhsusb_log(chg_sm_state,QHSUSB_CHG_STATE_TYPE_OFFSET,0x0);
        switch(chg_sm_state)
        {
            case QHSUSB_CHG_ST_BSESS_VALID :
            {
                dci_set_phy_mode(core_id, QHSUSB_CHG_PHY_MODE_NONDRIVE);
                dci_enable_dcd_circuit(core_id);
                chg_sm_state = QHSUSB_CHG_ST_WAIT_FOR_DCD;
            }
            break;

            case QHSUSB_CHG_ST_WAIT_FOR_DCD :
            {

                char pollCount = 0;
                dcd_out = 0;
                do{

                    dcd_out = dci_read_dcdout_output(core_id);

                    if(dcd_out == TRUE)
                    {
                        // Wait for TDCD_DBC
                        qhsusb_dci_delay_ms(10);
                        if( dcd_out == TRUE )
                        {
                            chg_sm_state = QHSUSB_CHG_ST_DCD_SUCCESS;
                            break;
                        }
                    }

                    qhsusb_dci_delay_ms(150); // Try again after 150ms
                    pollCount++;
                }while(pollCount != LINESTATE_POLL_COUNT);
                
                // If time-out occurs, go to the QHSUSB_CHG_ST_DCD_FAILURE state
                if(chg_sm_state == QHSUSB_CHG_ST_WAIT_FOR_DCD)
                {
                    chg_sm_state = QHSUSB_CHG_ST_DCD_FAILURE;
                }

                dci_disable_dcd_circuit(core_id);
            }
            break;

            case QHSUSB_CHG_ST_DCD_SUCCESS :
            {
                dci_enable_primary_charger_detection(core_id);
                chg_det = dci_read_chgdet_output(core_id);
                dci_disable_primary_charger_detection(core_id);
                linestate = qhsusb_dci_get_line_state(core_id);

                // If charger_vout is 0 we have detected SDP
                if (linestate)
                {
                    chg_port_type = QHSUSB_CHG_PORT_DCP_NON_COMPLIANT;
                    chg_sm_state = QHSUSB_CHG_ST_DONE;
                }
                else if(chg_det == FALSE)
                {
                    chg_sm_state = QHSUSB_CHG_ST_SDP;
                }
                else if(chg_det == TRUE)
                {
                    chg_sm_state = QHSUSB_CHG_ST_CHARGING_PORT_DETECTED;
                }
                else
                {
                    chg_sm_state = QHSUSB_CHG_ST_INVALID_CHARGER;
                }

            }
            break;

            case QHSUSB_CHG_ST_DCD_FAILURE :
            {
                dci_enable_primary_charger_detection(core_id);
                chg_det = dci_read_chgdet_output(core_id);
                dci_disable_primary_charger_detection(core_id);
                linestate = qhsusb_dci_get_line_state(core_id);
                if((FALSE == chg_det) || (linestate != 0x0))
                {
                  /** This is the case when DCD failure occured and charger
                  is detected as SDP, which means it is a floating charger **/ 
                  chg_port_type = QHSUSB_CHG_PORT_DCP_NON_COMPLIANT;
                    chg_sm_state = QHSUSB_CHG_ST_DONE;
                }
                else
                {
                    chg_sm_state = QHSUSB_CHG_ST_CHARGING_PORT_DETECTED;
                }
            }
            break;

            case QHSUSB_CHG_ST_CHARGING_PORT_DETECTED :
            {
                chg_port_type = QHSUSB_CHG_PORT_DCP;
                chg_sm_state = QHSUSB_CHG_ST_DONE;
            }
            break;

            case QHSUSB_CHG_ST_SDP :
            {
                chg_port_type = QHSUSB_CHG_PORT_SDP;
                chg_sm_state = QHSUSB_CHG_ST_DONE;
            }
            break;

            case QHSUSB_CHG_ST_INVALID_CHARGER :
            {
                chg_port_type = QHSUSB_CHG_PORT_INVALID;
                chg_sm_state = QHSUSB_CHG_ST_DONE;
            }
            break;

            case QHSUSB_CHG_ST_DONE :
            {
                /* Check if ULPI Viewport is locked-up */
                if (FALSE == qhsusb_dci_ulpi_read_with_err_check(core_id, USB_OTG_HS_DEBUG_REG_ADDR, &linestate))
                {
                    /* Perform GCC RESET to recover ULPI lock-up, and do not enter EDL */
                    qhsusb_log(QHSUSB_CHG_ST_SE1_ON_BSESS_VALID_LOG,0x0,__LINE__);
                    qhsusb_dci_usb_hw_reset();

                    // Override the charger to NON COMPLIANT since lock-up can be due to SE1
                    chg_port_type = QHSUSB_CHG_PORT_DCP_NON_COMPLIANT;
                }
                else
                {
                    dci_reset_charger_detection_block(core_id);
                    dci_reset_alternate_interrupt_block(core_id);
                    dci_set_phy_mode(core_id,QHSUSB_CHG_PHY_MODE_NORMAL);
                    if(chg_port_type == QHSUSB_CHG_PORT_SDP)
                    { 
                        qhsusb_dci_set_runstop_bit(core_id);
                    }
                }
                chg_sm_state = QHSUSB_CHG_ST_END;
            }
            break;

            case QHSUSB_CHG_ST_END :
            {
                // Nothing to do here
            }
            break;

            default :
            {
                chg_port_type = QHSUSB_CHG_PORT_INVALID;
                chg_sm_state = QHSUSB_CHG_ST_DONE;
            }
            break;
        }

    }

    if((loop_count >= QHSUSB_CHG_DETECTION_LOOP_COUNT) && (chg_sm_state != QHSUSB_CHG_ST_END))
    {
        /* Check if ULPI Viewport is locked-up */
        if (FALSE == qhsusb_dci_ulpi_read_with_err_check(core_id, USB_OTG_HS_DEBUG_REG_ADDR, &linestate))
        {
          /* Perform GCC RESET to recover ULPI lock-up, and do not enter EDL */
          qhsusb_log(QHSUSB_CHG_ST_SE1_ON_BSESS_VALID_LOG,0x0,__LINE__);
          qhsusb_dci_usb_hw_reset();
        }
        else
        {
          dci_reset_charger_detection_block(core_id);
          dci_reset_alternate_interrupt_block(core_id);
          dci_set_phy_mode(core_id,QHSUSB_CHG_PHY_MODE_NORMAL);
        }
        chg_port_type = QHSUSB_CHG_PORT_INVALID;
    }

    qhsusb_log(QHSUSB_CHARGER_TYPE_IS_LOG,0x0,chg_port_type);
    return chg_port_type;
}

boolean dci_is_streaming_mode_disable_required(void)
{
    //This target doesn't require disabling streaming mode.
    return FALSE;
}

