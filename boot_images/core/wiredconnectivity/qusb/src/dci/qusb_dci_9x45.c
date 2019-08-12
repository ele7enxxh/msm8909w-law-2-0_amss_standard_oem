//============================================================================
/**
 * @file        qusb_dci_9x45.c
 * @author      shreyasr
 * @date        19-June-2013
 *
 * @brief       QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) PHY specific handling.
 *
 * @details      There might be different PHY types for different targets.
 *               When using 3-wired Full-Speed PHY the USB Core (Link) cannot conntrol the D+/D-.
 * 
 * @note 
 * 
 * @ref         Design Ware Controller spec "DWC_usb3_databook.pdf".
 *
 *              Copyright (c) 2013 QUALCOMM Technologies Incorporated.
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
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qusb/src/dci/qusb_dci_9x45.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2013-06-19    shreyasr Added changes for MDM9x35
// 2012-04-14    tnk     Added flat file changes 
// 2010-04-14    yli     MSM8960 branch
// 2008-09-03   amirs    First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "qusb_hwio_dependency_9x45.h"
#include "ClockBoot.h"

#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "qusb_dci_api.h"         // DCI API Prototypes
#include "qusb_dci_private.h"     // dQH_t
#include "qusb_log.h" 

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
core_info_t dci_core_info[DCI_MAX_CORES];

#define QUSB_DLOAD_INFO_ADDR_IN_IMEM 	            (SHARED_IMEM_USB_BASE)
//defines for enabling the 1.5k pull up 
#define QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN     (HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN   (HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_BMSK)
#define QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS   (0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_SHFT)
#define QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE          (0x0 << HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE_SHFT)

#define QUSB_LINESTATE_CHECK_DELAY                (0x5)
#define QUSB_LINESTATE_CHECK_RETRY_CNT            (10000)   // 50msec is the total wait time to deal with a bad cable

// Assume a 300 ohm SMB charger resistor on D+/D- which corresponds to HSTX_TRIM offset of value 2
// Subtract QUSB_HSTX_TRIM_OFFSET to make amplitude overshoot by 400mV to account for longer cables and hubs.

#define QUSB_HSTX_TRIM_OFFSET                     (2)

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------
static boolean qusb_forced_download_feature_supported = FALSE;  //On MDM9x45 this feature shouldn't be supported 

//============================================================================
// QUSB Super-Speed PHY Configuration Array
//============================================================================
#define QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT    (72)

// Values according to Sec.3.2.2 of QMP_USB3_PHY_HPG (Feb28, 14)
static const uint32 qusb_dci_ss_phy_cfg_address_common[QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  QUSB_HWIO_ADDR_EMPTY,                                       // 0  : (0x00)
  QUSB_HWIO_ADDR_EMPTY,                                       // 1  : (0x00)
  QUSB_HWIO_ADDR_EMPTY,                                       // 2  : (0x00)
  QUSB_HWIO_ADDR_EMPTY,                                       // 3  : (0x00)
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_SYSCLK_EN_SEL_TXBAND_ADDR,         // 4  : (0x08) 19.2 Mhz Setup: Select refclk_in_cmos as sysclk_source
  HWIO_USB3PHY_QSERDES_COM_DEC_START1_ADDR,                   // 5  : (0x82) 19.2 Mhz Setup: Set decimal divide value to 0x82 (130d)
  HWIO_USB3PHY_QSERDES_COM_DEC_START2_ADDR,                   // 6  : (0x03) 
  QUSB_HWIO_ADDR_EMPTY,                                       // 7  : (0x00)
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START1_ADDR,              // 8  : (0xD5) Set fractional divide value to 0x35555 (218453d)
  HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START2_ADDR,              // 9  : (0xAA)
  HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START3_ADDR,              // 10 : (0x4D)
  HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP_EN_ADDR,               // 11 : (0x11) Increase compare range from +/-16 to +/- 256; Don't use lookup table for PLL lock count value
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP1_ADDR,                 // 12 : (0x2B) Set PLL lock compare value for 19.2Mhz to 5GHz
  HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP2_ADDR,                 // 13 : (0x68)
  HWIO_USB3PHY_QSERDES_COM_PLL_CRCTRL_ADDR,                   // 14 : (0x7C) Set proportional RC filter values
  HWIO_USB3PHY_QSERDES_COM_PLL_CP_SETI_ADDR,                  // 15 : (0x07) Set integral charge pump capacitance
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_PLL_IP_SETP_ADDR,                  // 16 : (0x1F) Set proportional charge pump current   
  HWIO_USB3PHY_QSERDES_COM_PLL_CP_SETP_ADDR,                  // 17 : (0x0F) Set proportional charge pump capacitance
  HWIO_USB3PHY_QSERDES_COM_PLL_IP_SETI_ADDR,                  // 18 : (0x01) Set integral charge pump current
  QUSB_HWIO_ADDR_EMPTY,                                       // 19 : (0x00)
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_IE_TRIM_ADDR,                      // 20 : (0x0F) Set external resistor bias current trim
  HWIO_USB3PHY_QSERDES_COM_IP_TRIM_ADDR,                      // 21 : (0x0F) Set internal resistor bias current trim  
  HWIO_USB3PHY_QSERDES_COM_PLL_CNTRL_ADDR,                    // 22 : (0x46) PLL IBOOST enabled, carry bit feed-forward; 3rd order MASH
  HWIO_USB3PHY_QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_ADDR, // 23 : (0x75) CDR Settings: Enable UCDR with default settings 
  //---------------------------------------------------------------------------
  HWIO_USB3PHY_QSERDES_COM_RESETSM_CNTRL_ADDR,                // 24 : (0x90) Calibration Settings: Use faster VCO calibration method
  HWIO_USB3PHY_QSERDES_COM_RESETSM_CNTRL2_ADDR,               // 25 : (0x07) Calibration Settings: Bypass restrim2 and background calibration
  HWIO_USB3PHY_QSERDES_COM_PLL_VCOTAIL_EN_ADDR,               // 26 : (0xE1) Calibration Settings: Force vco tail current as work-around for vco amp calibration
  QUSB_HWIO_ADDR_EMPTY,                                       // 27 : (0x00)
  //---------------------------------------------------------------------------  
  HWIO_USB3PHY_QSERDES_COM_RES_CODE_START_SEG1_ADDR,          // 28 : (0x20) Additional Writes from here: Resistor trim starts low and increases 
  HWIO_USB3PHY_QSERDES_COM_RES_CODE_CAL_CSR_ADDR,             // 29 : (0x77) Set sysclk and vco resistor value
  HWIO_USB3PHY_QSERDES_COM_RES_TRIM_CONTROL_ADDR,             // 30 : (0x15) Set restrim segment & method
  HWIO_USB3PHY_QSERDES_TX_RCV_DETECT_LVL_ADDR,                // 31 : (0x02) Set receiver detect compare level
  //---------------------------------------------------------------------------   
  HWIO_USB3PHY_QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,   // 32 : (0x67) Set Offsetcode adaptor mode and enable
  QUSB_HWIO_ADDR_EMPTY,                                       // 33 : (0x00)
  QUSB_HWIO_ADDR_EMPTY,                                       // 34 : (0x00)
  HWIO_USB3PHY_QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,      // 35 : (0x80) Prevent PCS from overriding calculated offset value
  //---------------------------------------------------------------------------  
  QUSB_HWIO_ADDR_EMPTY,                                       // 36 : (0x00)
  HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_ADDR,         // 37 : (0x06) Set equalizer de-gen cap value
  HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_ADDR,         // 38 : (0x6C) Configure equalization
  HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_ADDR,         // 39 : (0xA7) Configure equalization  
  //---------------------------------------------------------------------------  
  HWIO_USB3PHY_QSERDES_RX_SIGDET_ENABLES_ADDR,                // 40 : (0x40) Set so sigdet value from CSR
  HWIO_USB3PHY_QSERDES_RX_SIGDET_CNTRL_ADDR,                  // 41 : (0x73) Set sigdet level and bandwidth
  HWIO_USB3PHY_QSERDES_RX_SIGDET_DEGLITCH_CNTRL_ADDR,         // 42 : (0x06) Set sigdet deglitch bandwidth  
  HWIO_USB3PHY_PCIE_USB3_PHY_RX_IDLE_DTCT_CNTRL_ADDR,         // 43 : (0x48) Disable EIOS detection and using rx_sigdet for idle determination  
  //---------------------------------------------------------------------------  
  HWIO_USB3PHY_QSERDES_COM_SSC_EN_CENTER_ADDR,                // 44 : (0x01) Enable SSC downspread; write 00h to disable SSC
  HWIO_USB3PHY_QSERDES_COM_SSC_ADJ_PER1_ADDR,                 // 45 : (0x02) 3 sysclk cycles between divider changes 
  HWIO_USB3PHY_QSERDES_COM_SSC_PER1_ADDR,                     // 46 : (0x31) 31.5KHz
  HWIO_USB3PHY_QSERDES_COM_SSC_PER2_ADDR,                     // 47 : (0x01)
   //---------------------------------------------------------------------------  
  HWIO_USB3PHY_QSERDES_COM_SSC_STEP_SIZE1_ADDR,               // 48 : (0x19) SSC step = 6425 frac codes (-4800PPM)
  HWIO_USB3PHY_QSERDES_COM_SSC_STEP_SIZE2_ADDR,               // 49 : (0x19)
  HWIO_USB3PHY_PCIE_USB3_PHY_POWER_STATE_CONFIG2_ADDR,        // 50 : (0x08) sysclk off in P3
  QUSB_HWIO_ADDR_EMPTY,                                       // 51 : (0x00)
  //---------------------------------------------------------------------------   
  HWIO_USB3PHY_PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_L_ADDR,       // 52 : (0xE5) Set receiver detect delay to 4us when f(pipe_clk) = 125MHz
  HWIO_USB3PHY_PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_H_ADDR,       // 53 : (0x03) 
  HWIO_USB3PHY_PCIE_USB3_PHY_RXEQTRAINING_RUN_TIME_ADDR,      // 54 : (0x13) Reduce training trim from 2msec to ~300uSecs  
  HWIO_USB3PHY_PCIE_USB3_PHY_FLL_CNTRL2_ADDR,                 // 55 : (0x05) Divide FLL osc by 6 to create FLL clock
  //---------------------------------------------------------------------------     
  HWIO_USB3PHY_PCIE_USB3_PHY_LOCK_DETECT_CONFIG1_ADDR,        // 56 : (0xFF) Set LOCK_HOLDOFF_TIME_H/L to 3FFh = 4.092usec
  HWIO_USB3PHY_PCIE_USB3_PHY_LOCK_DETECT_CONFIG3_ADDR,        // 57 : (0x17) Set LOCK_COUNT_VAL_H/L to 102h =259 COMMAs (min of 2.072us of TS1/TS2) 
  QUSB_HWIO_ADDR_END,                                         // 58 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 59 : (0x00)
  //---------------------------------------------------------------------------
  QUSB_HWIO_ADDR_END,                                         // 60 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 61 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 62 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 63 : (0x00)
  //---------------------------------------------------------------------------
  QUSB_HWIO_ADDR_END,                                         // 64 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 65 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 66 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 67 : (0x00)
  //---------------------------------------------------------------------------
  QUSB_HWIO_ADDR_END,                                         // 68 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 69 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 70 : (0x00)
  QUSB_HWIO_ADDR_END,                                         // 71 : (0x00)
};

__align(4)
static const uint8 qusb_dci_ss_phy_cfg_value_common[QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT] =
{
  /* QUSB_HWIO_ADDR_EMPTY,                                       0 */ (0x00),
  /* QUSB_HWIO_ADDR_EMPTY,                                       1 */ (0x00),
  /* QUSB_HWIO_ADDR_EMPTY,                                       2 */ (0x00),
  /* QUSB_HWIO_ADDR_EMPTY,                                       3 */ (0x00),
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_SYSCLK_EN_SEL_TXBAND_ADDR,         4 */ (0x08),  // 19.2 Mhz Setup: Select refclk_in_cmos as sysclk_source
  /* HWIO_USB3PHY_QSERDES_COM_DEC_START1_ADDR,                   5 */ (0x82),  // 19.2 Mhz Setup: Set decimal divide value to 0x82 */ (130d),
  /* HWIO_USB3PHY_QSERDES_COM_DEC_START2_ADDR,                   6 */ (0x03),
  /* QUSB_HWIO_ADDR_EMPTY,                                       7 */ (0x00),
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START1_ADDR,              8 */ (0xD5),  // Set fractional divide value to 0x35555 */ (218453d),
  /* HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START2_ADDR,              9 */ (0xAA),
  /* HWIO_USB3PHY_QSERDES_COM_DIV_FRAC_START3_ADDR,              10*/ (0x4D),
  /* HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP_EN_ADDR,               11*/ (0x11),  // Increase compare range from +/-16 to +/- 256; Don't use lookup table for PLL lock count value
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP1_ADDR,                 12*/ (0x2B),  // Set PLL lock compare value for 19.2Mhz to 5GHz
  /* HWIO_USB3PHY_QSERDES_COM_PLLLOCK_CMP2_ADDR,                 13*/ (0x68),
  /* HWIO_USB3PHY_QSERDES_COM_PLL_CRCTRL_ADDR,                   14*/ (0x7C),  // Set proportional RC filter values
  /* HWIO_USB3PHY_QSERDES_COM_PLL_CP_SETI_ADDR,                  15*/ (0x07),  // Set integral charge pump capacitance
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_PLL_IP_SETP_ADDR,                  16*/ (0x1F),  // Set proportional charge pump current   
  /* HWIO_USB3PHY_QSERDES_COM_PLL_CP_SETP_ADDR,                  17*/ (0x0F),  // Set proportional charge pump capacitance
  /* HWIO_USB3PHY_QSERDES_COM_PLL_IP_SETI_ADDR,                  18*/ (0x01),  // Set integral charge pump current
  /* QUSB_HWIO_ADDR_EMPTY,                                       19*/ (0x00),
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_IE_TRIM_ADDR,                      20*/ (0x0F),  // Set external resistor bias current trim
  /* HWIO_USB3PHY_QSERDES_COM_IP_TRIM_ADDR,                      21*/ (0x0F),  // Set internal resistor bias current trim  
  /* HWIO_USB3PHY_QSERDES_COM_PLL_CNTRL_ADDR,                    22*/ (0x46),  // PLL IBOOST enabled, carry bit feed-forward; 3rd order MASH
  /* HWIO_USB3PHY_QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_ADDR, 23*/ (0x75),  // CDR Settings: Enable UCDR with default settings 
  //---------------------------------------------------------------------------
  /* HWIO_USB3PHY_QSERDES_COM_RESETSM_CNTRL_ADDR,                24*/ (0x90),  // Calibration Settings: Use faster VCO calibration method
  /* HWIO_USB3PHY_QSERDES_COM_RESETSM_CNTRL2_ADDR,               25*/ (0x07),  // Calibration Settings: Bypass restrim2 and background calibration
  /* HWIO_USB3PHY_QSERDES_COM_PLL_VCOTAIL_EN_ADDR,               26*/ (0xE1),  // Calibration Settings: Force vco tail current as work-around for vco amp calibration
  /* QUSB_HWIO_ADDR_EMPTY,                                       27*/ (0x00),
  //---------------------------------------------------------------------------  
  /* HWIO_USB3PHY_QSERDES_COM_RES_CODE_START_SEG1_ADDR,          28*/ (0x20),  // Additional Writes from here: Resistor trim starts low and increases 
  /* HWIO_USB3PHY_QSERDES_COM_RES_CODE_CAL_CSR_ADDR,             29*/ (0x77),  // Set sysclk and vco resistor value
  /* HWIO_USB3PHY_QSERDES_COM_RES_TRIM_CONTROL_ADDR,             30*/ (0x15),  // Set restrim segment & method
  /* HWIO_USB3PHY_QSERDES_TX_RCV_DETECT_LVL_ADDR,                31*/ (0x02),  // Set receiver detect compare level
  //---------------------------------------------------------------------------   
  /* HWIO_USB3PHY_QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_ADDR,   32*/ (0x67),  // Set Offsetcode adaptor mode and enable
  /* QUSB_HWIO_ADDR_EMPTY,                                       33*/ (0x00),
  /* QUSB_HWIO_ADDR_EMPTY,                                       34*/ (0x00),
  /* HWIO_USB3PHY_QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_ADDR,      35*/ (0x80),  // Prevent PCS from overriding calculated offset value
  //---------------------------------------------------------------------------  
  /* QUSB_HWIO_ADDR_EMPTY,                                       36*/ (0x00),
  /* HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_ADDR,         37*/ (0x06),  // Set equalizer de-gen cap value
  /* HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_ADDR,         38*/ (0x6C),  // Configure equalization
  /* HWIO_USB3PHY_QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_ADDR,         39*/ (0xA7),  // Configure equalization  
  //---------------------------------------------------------------------------  
  /* HWIO_USB3PHY_QSERDES_RX_SIGDET_ENABLES_ADDR,                40*/ (0x40),  // Set so sigdet value from CSR
  /* HWIO_USB3PHY_QSERDES_RX_SIGDET_CNTRL_ADDR,                  41*/ (0x73),  // Set sigdet level and bandwidth
  /* HWIO_USB3PHY_QSERDES_RX_SIGDET_DEGLITCH_CNTRL_ADDR,         42*/ (0x06),  // Set sigdet deglitch bandwidth  
  /* HWIO_USB3PHY_PCIE_USB3_PHY_RX_IDLE_DTCT_CNTRL_ADDR,         43*/ (0x48),  // Disable EIOS detection and using rx_sigdet for idle determination  
  //---------------------------------------------------------------------------  
  /* HWIO_USB3PHY_QSERDES_COM_SSC_EN_CENTER_ADDR,                44*/ (0x01),  // Enable SSC downspread; write 00h to disable SSC
  /* HWIO_USB3PHY_QSERDES_COM_SSC_ADJ_PER1_ADDR,                 45*/ (0x02),  // 3 sysclk cycles between divider changes 
  /* HWIO_USB3PHY_QSERDES_COM_SSC_PER1_ADDR,                     46*/ (0x31),  // 31.5KHz
  /* HWIO_USB3PHY_QSERDES_COM_SSC_PER2_ADDR,                     47*/ (0x01),
   //---------------------------------------------------------------------------  
  /* HWIO_USB3PHY_QSERDES_COM_SSC_STEP_SIZE1_ADDR,               48*/ (0x19),  // SSC step = 6425 frac codes */ (-4800PPM),
  /* HWIO_USB3PHY_QSERDES_COM_SSC_STEP_SIZE2_ADDR,               49*/ (0x19),
  /* HWIO_USB3PHY_PCIE_USB3_PHY_POWER_STATE_CONFIG2_ADDR,        50*/ (0x08),  // sysclk off in P3
  /* QUSB_HWIO_ADDR_EMPTY,                                       51*/ (0x00),
  //---------------------------------------------------------------------------   
  /* HWIO_USB3PHY_PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_L_ADDR,       52*/ (0xE5),  // Set receiver detect delay to 4us when f*/ (pipe_clk),  // = 125MHz
  /* HWIO_USB3PHY_PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_H_ADDR,       53*/ (0x03),
  /* HWIO_USB3PHY_PCIE_USB3_PHY_RXEQTRAINING_RUN_TIME_ADDR,      54*/ (0x13),  // Reduce training trim from 2msec to ~300uSecs  
  /* HWIO_USB3PHY_PCIE_USB3_PHY_FLL_CNTRL2_ADDR,                 55*/ (0x05),  // Divide FLL osc by 6 to create FLL clock
  //---------------------------------------------------------------------------     
  /* HWIO_USB3PHY_PCIE_USB3_PHY_LOCK_DETECT_CONFIG1_ADDR,        56*/ (0xFF),  // Set LOCK_HOLDOFF_TIME_H/L to 3FFh = 4.092usec
  /* HWIO_USB3PHY_PCIE_USB3_PHY_LOCK_DETECT_CONFIG3_ADDR,        57*/ (0x17),  // Set LOCK_COUNT_VAL_H/L to 102h =259 COMMAs */ (min of 2.072us of TS1/TS2),  // 
  /* QUSB_HWIO_ADDR_END,                                         58*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         59*/ (0x00),
  //---------------------------------------------------------------------------
  /* QUSB_HWIO_ADDR_END,                                         60*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         61*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         62*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         63*/ (0x00),
  //---------------------------------------------------------------------------
  /* QUSB_HWIO_ADDR_END,                                         64*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         65*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         66*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         67*/ (0x00),
  //---------------------------------------------------------------------------
  /* QUSB_HWIO_ADDR_END,                                         68*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         69*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         70*/ (0x00),
  /* QUSB_HWIO_ADDR_END,                                         71*/ (0x00),
};

#define QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT          (128)
#define QUSB_DCI_SS_PHY_FOUNDRY_CNT_MAX                       (8)

// initialization without reading the foundry information
static uint32 qusb_dci_ss_phy_foundry_id = 0;

// Array keeps tracking of starting index of override programming sequence
// per foundry
static const uint32 qusb_dci_ss_phy_cfg_override_start_index[QUSB_DCI_SS_PHY_FOUNDRY_CNT_MAX] =
{ 
    0,    // 0
    16,   // 1
    32,   // 2
    48,   // 3
    64,   // 4
    80,   // 5
    96,   // 6
    112,  // 7
};

// Between 2 foundries, there must be QUSB_HWIO_ADDR_END as sentinel value to
// prevent going over the boundry.
static const uint32 qusb_dci_ss_phy_cfg_address_override[QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT] = 
{
    //  0
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 16
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 32
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 48
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 64
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 80
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 96
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 112
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END, QUSB_HWIO_ADDR_END,
    // 128
};
__align(4)
static const uint8 qusb_dci_ss_phy_cfg_value_override[QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT]    = 
{
  //  0
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 16
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 32
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 48
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 64
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 80
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 96
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 112
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  // 128
};

//============================================================================
// QUSB High-Speed PHY Configuration Array
//============================================================================

#define QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT      (4)

// It is recommended to set the CSR bit "r_power_down = 1'b1", program any 
// hardware tuning (such as mentioned in #1 above)/config bits/etc., then 
// release "r_power_down = 1'b0" once again, in the QUSB2PHY_POWER_DOWN 
// register.

static const uint32 qusb_dci_hs_phy_cfg_address[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE1_ADDR,                     // 0  : (0xA0)
  HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE2_ADDR,                     // 1  : (0x63)
  HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE3_ADDR,                     // 2  : (0x93)
  HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE4_ADDR,                     // 3  : (0xD5)
};

static const uint8 qusb_dci_hs_phy_cfg_value[QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  /* HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE1_ADDR,                     0  */ (0xA0),
  /* HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE2_ADDR,                     1  */ (0x63),
  /* HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE3_ADDR,                     2  */ (0x93),
  /* HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE4_ADDR,                     3  */ (0xD5),
};


//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------
static void qusb_dci_phy_reg_array_process
(
  const uint32 *address_array, 
  const uint8  *value_array, 
  uint32        start_index, 
  uint32        array_entry_cnt
);

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------


//============================================================================
/**
 * @function    qusb_get_shared_imem_address
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
uint32 qusb_get_shared_imem_address()
{
  return QUSB_DLOAD_INFO_ADDR_IN_IMEM;
}

// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_clocks
 * 
 * @brief   This function will be used to turn ON the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_enable_usb30_clocks(void)
{
  boolean var = TRUE; 
  //The ClockInit doesn't actually configure the frequency and muxes. 
  //@ToDo: Add the clock frequencies 
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  var = Clock_InitUSB();
#endif
  return (var);
}

// ===========================================================================
/**
 * @function    qusb_dci_disable_usb30_clocks
 * 
 * @brief   This function will be used to turn OFF the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_disable_usb30_clocks(void)
{
  boolean var = TRUE; 
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL) && !defined(FEATURE_EMMCBLD)
  var = Clock_DisableUSB();
#endif
  return (var);
}

// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_power
 * 
 * @brief   This function enables the power domain for SNPS controller
 *
 * @param   TRUE: It enables the s/w Power Collapse 
 *          FALSE:  IT disables the s/w Power Collapse
 * 
 * @details  Used for enabling the power domain for SNPS controller
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
void qusb_dci_enable_usb30_power_collapse(boolean enable)
{
  Clock_Usb30EnableSWCollapse(enable); 
  return; 
}
// ===========================================================================
/**
 * @function    qusb_dci_is_usb30_sw_collapsed
 * 
 * @brief   This function reads back if  the power domain for SNPS 
            controller is power collapsed.
 * 
 * @details Used fr cheking if USB core was initialized in PBL.
 *          If not then single enumeration cannot continue, 
 *          USB core needs to be re-enumerated.
 * 
 * @return  TRUE or FALSE depending on status of SW collapse bit.
 * 
 */
// ===========================================================================
boolean qusb_dci_is_usb30_sw_collapsed(void)
{
  return Clock_Usb30GetSWCollapse();
}

// ===========================================================================
/**
 * @function    qusb_snps_switch_pipe_clock_src_to_usb3phy
 * 
 * @brief   This API switches the PIPE Clock from GCC XO source to USB PHY
 *          source
 * 
 * @details  During the system PON, PIPE clock is initially driven by XO. Once 
 *           the PHY reset sequence is completed, then the clock source needs 
 *           to be switched to PHY source.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_snps_switch_pipe_clock_src_to_usb3phy(void)
{
  //Call the clock API to do a pipe clock switch
  boolean var = FALSE; 
  var = Clock_Usb30SwitchPipeClk(); 
  if(FALSE == var)
  {
    qusb_error_log(QUSB_PIPE_CLK_SWITCH_ERROR_LOG, 0, __LINE__); 
  }
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_get_hardware_id
 * 
 * @brief   This function will be used for getting the SNPS h/w id.
 * 
 * @param   None
 * 
 * @return  uint32 - hardware id. 
 * 
 */
// ===========================================================================
uint32 qusb_dci_get_hardware_id(void)
{
  uint32 hw_id =0x0;
  hw_id = HWIO_USB30_GSNPSID_INM(HWIO_USB30_GSNPSID_SYNOPSYSID_BMSK);
  return hw_id;
}

// ===========================================================================
/**
 * @function    qusb_dci_hs_phy_update_hstx_trim
 * 
 * @brief   This function will update TUNE2 HSTX_TRIM register bits if feature is enabled.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qusb_dci_hs_phy_update_hstx_trim(void)
{

  boolean update_tune2 = 
    HWIO_FEATURE_CONFIG5_INM(HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK) 
    >> HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT;

  if(update_tune2)
  {
    // Subtract QUSB_HSTX_TRIM_OFFSET to overshoot amplitude by 400mV to account for longer cables and hubs.
    uint8 hstx_trim_val = 
        (HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_USB_PHY_TUNING_BMSK) 
        >> HWIO_QFPROM_CORR_CALIB_ROW0_MSB_USB_PHY_TUNING_SHFT) - QUSB_HSTX_TRIM_OFFSET;

    HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE2_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE2_UTM_HSTX_TRIM_BMSK, 
        hstx_trim_val << HWIO_QUSB2PHY_QUSB2PHY_PORT_TUNE2_UTM_HSTX_TRIM_SHFT);
  }
}

// ------------------------------------------------------------------------------------------------
// Core Initialization APIs
// ------------------------------------------------------------------------------------------------
// ===========================================================================
/**
 * @function    qusb_hs_phy_init
 * 
 * @brief   API used to initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_hs_phy_init(void)
{
    /* Keep the PHY in power down mode */
    HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
                                               (0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));
   
    qusb_dci_phy_reg_array_process(
          qusb_dci_hs_phy_cfg_address, 
          qusb_dci_hs_phy_cfg_value, 
          0, 
          QUSB_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT);

    // Overwrite HSTX TRIM calibration value
    qusb_dci_hs_phy_update_hstx_trim();

    /* Bring Out  the PHY from power down mode */
    HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
                                               (0x0 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));

}

// ===========================================================================
/**
 * @function    qusb_dci_configure_device_mode
 * 
 * @brief   This is used for configuring the Device config Register post reset.
 * 
 * @param   qusb_max_speed_required_t Maximum speed at which the device can operate
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_configure_device_mode(qusb_max_speed_required_t speed_required)
{
  // Configure the Minimum negotiatiated Speed to  HS: 0x0 , SS: 0x4
  if(speed_required == QUSB_MAX_SPEED_SUPER)
  {
    /* Maximum Speed Configured is Super Speed */
    HWIO_USB30_DCFG_OUTM(HWIO_USB30_DCFG_DEVSPD_BMSK, (QUSB_SUPER_SPEED << HWIO_USB30_DCFG_DEVSPD_SHFT));
  }
    else if (speed_required == QUSB_MAX_SPEED_HIGH)
  {
    /* Maximum Speed Configured is High Speed */
    HWIO_USB30_DCFG_OUTM(HWIO_USB30_DCFG_DEVSPD_BMSK, (QUSB_HIGH_SPEED << HWIO_USB30_DCFG_DEVSPD_SHFT));
  }

  /*set the device address to 0x0 after reset*/ 
  HWIO_USB30_DCFG_OUTM(HWIO_USB30_DCFG_DEVADDR_BMSK,(0x0 << HWIO_USB30_DCFG_DEVADDR_SHFT)); 

  /*LPM Capability EN: 0x1, DIS:0x0*/
  HWIO_USB30_DCFG_OUTM(HWIO_USB30_DCFG_LPMCAP_BMSK,(0x0 << HWIO_USB30_DCFG_LPMCAP_SHFT));

  /* Enable Device mode operation */
  HWIO_USB30_GCTL_OUTM(HWIO_USB30_GCTL_PRTCAPDIR_BMSK, (0x2 << HWIO_USB30_GCTL_PRTCAPDIR_SHFT )); 

  return;
}


// ===========================================================================
/**
 * @function    qusb_dci_configure_usb30
 * 
 * @brief   API used for PHY configuration in device Mode for USB3.0 
 *  
 * @details This API is used for configuring based on the HPG specifications 
 * 			Please follow the same steps for configuration 
 * 
 * @param   qusb_max_speed_required_t Maximum speed at which the device should
 *          operate. 
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_configure_usb30(qusb_max_speed_required_t speed_required)
{

  uint32 index = 0x0;

  //Assert Soft Reset on the core 
  HWIO_USB30_GCTL_OUTM(HWIO_USB30_GCTL_CORESOFTRESET_BMSK,(0x1 << HWIO_USB30_GCTL_CORESOFTRESET_SHFT));

  qusb_dci_delay_us(100);

  // Soft reset the USB 3.0 PHY PIPE3 interface  
  HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0x0,HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_PHYSOFTRST_BMSK,
                                                ((uint32)0x1 << HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_PHYSOFTRST_SHFT));
  qusb_dci_delay_us(100);

  HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0x0,HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_PHYSOFTRST_BMSK,
                                                ((uint32)0x0 << HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_PHYSOFTRST_SHFT));
  qusb_dci_delay_us(100);

  // Set DELAYP1TRANS to 0 
  HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0, HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK,
                               ((uint32) 0x0 << HWIO_USB30_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_SHFT));
  qusb_dci_delay_us(100);

  // Soft reset the USB 2.0 PHY interface 
  HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0,HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_BMSK,
                                               ((uint32)0x1 << HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_SHFT));
  qusb_dci_delay_us(100);

  HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0,HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_BMSK,
                                               ((uint32)0x0 << HWIO_USB30_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_SHFT));
  qusb_dci_delay_us(100);

  // De-assert Soft Reset on the core 
  HWIO_USB30_GCTL_OUTM(HWIO_USB30_GCTL_CORESOFTRESET_BMSK,(0x0 << HWIO_USB30_GCTL_CORESOFTRESET_SHFT));

  qusb_dci_delay_us(100);

  // Reset the controller
  HWIO_USB30_DCTL_OUTM(HWIO_USB30_DCTL_CSFTRST_BMSK,(0x1 << HWIO_USB30_DCTL_CSFTRST_SHFT));

  // Check to see if the core is reset successfully or not
  for(index = 0x0 ; index < QUSB_CSFTRST_TIME_OUT ; index++)
  {
    if(0x0 == HWIO_USB30_DCTL_INM(HWIO_USB30_DCTL_CSFTRST_BMSK))
    {
      break;
    }
    qusb_dci_delay_ms(1);
  }

  // Reset Device core  
  if(index == QUSB_CSFTRST_TIME_OUT)
  {
    DCI_ASSERT(DCI_RESET_DEVICE_ERROR_LOG);
  }

  // Controller Configuration to Peripheral mode 
  qusb_dci_configure_device_mode(speed_required);

  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_usb30_gcc_reset
 * 
 * @brief   API used for resetting the Link and PHYs using GCC control
 *  
 * @details This API is used for resetting the Link and PHYs using clock control 
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_usb30_gcc_reset(void)
{
  // Reset SNPS Link controller 
  HWIO_GCC_USB_30_BCR_OUTM(HWIO_GCC_USB_30_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB_30_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_USB_30_BCR_OUTM(HWIO_GCC_USB_30_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB_30_BCR_BLK_ARES_SHFT));

  // Reset QUSB (USB 2.0) and QMP (USB 3.0) PHYs 
  HWIO_GCC_QUSB2A_PHY_BCR_OUTM(HWIO_GCC_QUSB2A_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_QUSB2A_PHY_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x1 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x0 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));
  HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));
  HWIO_GCC_QUSB2A_PHY_BCR_OUTM(HWIO_GCC_QUSB2A_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_QUSB2A_PHY_BCR_BLK_ARES_SHFT));

  return; 
}

// ===========================================================================
/**
 * @function    qusb_dci_select_utmi_clk
 * 
 * @brief   This is used for configuring the core to UTMI clock instead of pipe
 *          clock.  This needs to be called when there is no SS USB PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_select_utmi_clk(void)
{
  qusb_error_log(QUSB_DCI_SELECT_UTMI_CLK_LOG, 0, 0);
  
  // If operating without SS PHY, follow this sequence to disable 
  // pipe clock requirement
  HWIO_USB30_GENERAL_CFG_OUTM(HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
    0x1 << HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);
  
  qusb_dci_delay_us(100);
  
  HWIO_USB30_GENERAL_CFG_OUTM(HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK,
    0x1 << HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_SEL_SHFT);
  
  qusb_dci_delay_us(100);
  
  HWIO_USB30_GENERAL_CFG_OUTM(HWIO_USB30_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK,
    0x1 << HWIO_USB30_GENERAL_CFG_PIPE3_PHYSTATUS_SW_SHFT);
  
  qusb_dci_delay_us(100);
  
  HWIO_USB30_GENERAL_CFG_OUTM(HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
    0x0 << HWIO_USB30_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);      
}

// ===========================================================================
/**
 * @function    qusb_ss_phy_init
 * 
 * @brief   API used for initializing the SS PHY 
 *  
 * @details This is used for initializing the SNPS controller PHY and QMP PHY 
 * 			Configure the QMP PHY as per Table 4-1 of the QMP PHY HPG 
 * 
 * @param   None.
 * 
 * @return  TRUE if successful else FALSE.
 * 
 */
// ===========================================================================
boolean qusb_ss_phy_init(void)
{
  uint32 qusb_timeout=0x0;

  qusb_error_log(QUSB_SS_PHY_INIT_LOG, 0, 0);

  /*Put the PHY and CSR blocks into RESET*/
  HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));

  /*Put the PHY block into RESET*/
  HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x1 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  /*Release the CSR block from RESET*/
  HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);
  
  // Write 0x11 to AHB2PHY bridge CSR PERIPH_SS_AHB2PHY_TOP_CFG so that 
  // writes and reads to/from the PHY use one wait state.
  HWIO_AHB2PHY_TOP_CFG_OUT(0x11);

  /* Release the PHY from Power Down mode */
  HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_OUTM(HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_BMSK, (0x1 << HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_SHFT));

  // Configure the QMP PHY as per Table 4-1 of the QMP PHY HPG with default entry
  qusb_dci_phy_reg_array_process(
          qusb_dci_ss_phy_cfg_address_common, 
          qusb_dci_ss_phy_cfg_value_common,
           0,
          QUSB_DCI_SS_PHY_COMMON_CFG_ARRAY_ENTRY_CNT);

  // Read foundry specific information and program addition overrides
  // qusb_dci_ss_phy_foundry_id = pbl_get_foundry_id();
  if (qusb_dci_ss_phy_foundry_id >= QUSB_DCI_SS_PHY_FOUNDRY_CNT_MAX)
  {
    qusb_error_log(INVALID_QUSB_DCI_SS_PHY_FOUNDRY_ID_LOG, 0, qusb_dci_ss_phy_foundry_id);
  }
  else
  {
    qusb_dci_phy_reg_array_process(
          qusb_dci_ss_phy_cfg_address_override, 
          qusb_dci_ss_phy_cfg_value_override, 
          qusb_dci_ss_phy_cfg_override_start_index[qusb_dci_ss_phy_foundry_id],
          QUSB_DCI_SS_PHY_OVERRIDE_CFG_ARRAY_ENTRY_CNT);
  }

  /* Release the PHY block from Reset */
  HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x0 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));

  qusb_dci_delay_us(100);

  /* Release the PHY from SW Reset state */
  HWIO_USB3PHY_PCIE_USB3_PHY_SW_RESET_OUTM(HWIO_USB3PHY_PCIE_USB3_PHY_SW_RESET_SW_RESET_BMSK, (0x0 << HWIO_USB3PHY_PCIE_USB3_PHY_SW_RESET_SW_RESET_SHFT));

  /* Start PHY operation (begin with internal power-up & calibration) */
  HWIO_USB3PHY_PCIE_USB3_PHY_START_OUT(0x03);

  /* Poll PHYSTATUS field of Register USB3_PHY_PCS_STATUS to Go LOW after reset*/
  while(HWIO_USB3PHY_PCIE_USB3_PHY_PCS_STATUS_INM(HWIO_USB3PHY_PCIE_USB3_PHY_PCS_STATUS_PHYSTATUS_BMSK))
  {
    qusb_dci_delay_us(100);
    if (qusb_timeout++ > 100)
    {
      DCI_ASSERT(DCI_SS_PHY_RESET_ERROR_LOG);
      return FALSE;
    }
  } 

  return TRUE;
}


// ===========================================================================
/**
 * @function    qusb_dci_deinit_hs_phy
 * 
 * @brief   API used to de-initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_deinit_hs_phy(void)
{
  //Clear the D+ Pull Up Assertion
  HWIO_USB30_HS_PHY_CTRL_OUTM(HWIO_USB30_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x0 << HWIO_USB30_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
  HWIO_USB30_HS_PHY_CTRL_OUTM(HWIO_USB30_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x0 << HWIO_USB30_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));
}
// ===========================================================================
/**
 * @function    qusb_dci_enable_vbus_valid
 * 
 * @brief       API used to enable VBUS using s/w control
 * 
 * @param   qusb_max_speed_required_t - Maximum Speed required to be configured
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_enable_vbus_valid(qusb_max_speed_required_t speed_required)
{

  HWIO_USB30_HS_PHY_CTRL_OUTM(HWIO_USB30_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK, (0x1 << HWIO_USB30_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT));
  HWIO_USB30_HS_PHY_CTRL_OUTM(HWIO_USB30_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK, (0x1 << HWIO_USB30_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT));

  /* If we want to enable SUPER SPEED then also set LANE PWR PRESENT bit */
  if(speed_required == QUSB_MAX_SPEED_SUPER)
  {
    HWIO_USB30_SS_PHY_CTRL_OUTM(HWIO_USB30_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK, (0x1 << HWIO_USB30_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT));
  }

  return;
}
// ===========================================================================
/**
 * @function    qusb_dci_phy_reg_array_process
 * 
 * @brief   This function reads from array which define list of hwio writes for
 *          USB PHY
 * 
 * @param   address_array   - array holding address of HW register
 *          value_array     - array holding values to be written to HW register
 *          start_index     - starting index for array processing
 *          array_entry_cnt - number of entries in the array
 * 
 * @return  None
 * 
 */
// ===========================================================================
static void qusb_dci_phy_reg_array_process
(
  const uint32 *address_array, 
  const uint8  *value_array, 
  uint32        start_index, 
  uint32        array_entry_cnt
)
{
  uint32 index = start_index;

  if ( (NULL == address_array)
      || (NULL == value_array)
      || (0 == array_entry_cnt) )
  {
    qusb_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__FAIL_LOG, 0, (uint32)address_array);
  }
  else
  {
    for (; index < array_entry_cnt; index++)
    {
      if (QUSB_HWIO_ADDR_END == address_array[index])
      {
        break;
      }

      if (QUSB_HWIO_ADDR_EMPTY == address_array[index])
      {
        continue;
      }

      out_dword(address_array[index], value_array[index]);
    }
  }
  qusb_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS__START_LOG, 0, start_index);
  qusb_error_log(QUSB_DCI_PHY_REG_ARRAY_PROCESS____END_LOG, 0, index);
}
// ===========================================================================
/**
 * @function    qusb_dci_check_for_pbl_dload
 * 
 * @brief   This function will check if D+ is grounded. And if it is connected
 *          to ground, then we go into EDL.
 * 
 * @param   None
 * 
 * @return  TRUE: if it is connected to GND
 *          FALSE: if it is not connected to GND. 
 * 
 */
// ===========================================================================
boolean qusb_dci_check_for_pbl_dload(void)
{
  uint32 linestate = 0x0; 
  int32 retries = 0x0;
  uint32 enable_fs_xvr = 0x0;
  boolean select_utmi_config = TRUE; // Fall back to HS Only UTMI config by default.
  
  if(FALSE == qusb_forced_download_feature_supported)
  {
    qusb_log(QUSB_SKIP_PBL_DLOAD_LOG, 0, 0xFF);
    return FALSE; 
  }
  
  // It is not guaranteed that the SNPS core is always power collapsed **/ 
  // Put it to power collapse and then bring out of power collapse
  qusb_dci_enable_usb30_power_collapse(TRUE); // Put it to power collapse explicitly 

  /** Bring the controller out of Power Collapse**/
  qusb_dci_enable_usb30_power_collapse(FALSE);
  
  //Disable the clocks 
  qusb_dci_disable_usb30_clocks(); 
  
  //hard reset the USB Link and PHY using the GCC reset
  qusb_dci_usb30_gcc_reset();
  
  //enable the clocks now 
  qusb_dci_enable_usb30_clocks();

  // Initialize SS PHY if present
  if(qusb_dci_ss_phy_present())
  {
    qusb_snps_switch_pipe_clock_src_to_usb3phy();
    
    // Fall back to HS only UTMI config if SS PHY initialization fails.
    select_utmi_config = (FALSE == qusb_ss_phy_init());
  }

  if(select_utmi_config)
  {
    qusb_dci_select_utmi_clk();
  }

  //Initialize the HS_PHY
  qusb_hs_phy_init(); 
  
  //Enable the VBUS
  qusb_dci_enable_vbus_valid(QUSB_MAX_SPEED_HIGH); 
  
  // Enable UTMI_TEST_MUX_SEL
  HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(
    HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_BMSK,
    0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_SHFT);

  
  //Disable the D+ Pull down 
  HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_DPPULLDOWN_BMSK, 
                                            (0x0 <<HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_DPPULLDOWN_SHFT )); 
  
  //Select the FS transceiver and enable the 1.5k pull-up. 
  enable_fs_xvr = (QUSB2PHY_PORT_UTMI_CTRL1_SUSPEND_N_EN | QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_EN | 
                   QUSB2PHY_PORT_UTMI_CTRL1_XCVR_SELECT_FS | QUSB2PHY_PORT_UTMI_CTRL1_OP_MODE); 
  HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_OUT(enable_fs_xvr);
  
  do
  {
    //wait for 5 microseconds 
    qusb_dci_delay_us(QUSB_LINESTATE_CHECK_DELAY); 
    
    //read the linestate
    linestate = HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_STATUS_INM(HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_STATUS_LINESTATE_BMSK); 
    
    //if D+ is still high, then the device should not enter the EDL Mode 
    if(linestate != 0x0)
    {
      break; 
    }
    
    retries++; 
   
  }while (retries < QUSB_LINESTATE_CHECK_RETRY_CNT);
  
  //disable the 1.5K Pull up resistor
  HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_OUT(HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL1_TERM_SELECT_BMSK);
  
  // Disable UTMI_TEST_MUX_SEL
  HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_OUTM(
    HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_BMSK,
    0x0 << HWIO_QUSB2PHY_QUSB2PHY_PORT_UTMI_CTRL2_UTMI_TEST_MUX_SEL_SHFT);

  if(linestate != 0x0)
  {
    qusb_log(QUSB_SKIP_PBL_DLOAD_LOG, 0, linestate);
    return FALSE; 
  }
  //D+ is connected to the GND
  qusb_log(QUSB_ENTER_PBL_DLOAD_LOG, 0, linestate); 
  return TRUE;
}

// ===========================================================================
/**
 * @function    qusb_dci_check_ss_phy_presence
 * 
 * @brief   This function will check if 0.9V analog supply to SS PHY is on.
 * 
 * @param   None
 * 
 * @return  TRUE:  If analog supply to SS PHY on.
 *          FALSE: If analog supply to SS PHY off.
 * 
 */
// ===========================================================================
boolean qusb_dci_ss_phy_present(void)
{
  uint32 ac_jtag_outp, ac_jtag_outn;
  boolean ss_phy_present = FALSE;
  
  //1.  Release pcieusb3phy_reset (pcieusb3phy_phy_reset can remain asserted).
  HWIO_GCC_USB3_PHY_BCR_OUTM(HWIO_GCC_USB3_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_USB3_PHY_BCR_BLK_ARES_SHFT));

  //2.  Write 0x01 to PCIE_USB3_PHY_POWER_DOWN_CONTROL to bring the PHY out of powerdown mode.
  HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_OUTM(HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_BMSK, (0x1 << HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_SHFT));

  //3.  Write 0x01 to QSERDES_RX_AC_JTAG_RESET to enable set/reset of the ac_jtag receiver circuit.
  HWIO_USB3PHY_QSERDES_RX_AC_JTAG_RESET_OUTM(HWIO_USB3PHY_QSERDES_RX_AC_JTAG_RESET_AC_JTAG_RESET_BMSK, (0x1 << HWIO_USB3PHY_QSERDES_RX_AC_JTAG_RESET_AC_JTAG_RESET_SHFT));
  
  //4.  Write 0x02 to QSERDES_RX_AC_JTAG_INITP and 0x03 to QSERDES_RX_AC_JTAG_INITN to set/reset ac_jtag_outp/n.
  HWIO_USB3PHY_QSERDES_RX_AC_JTAG_INITP_OUT(0x02);
  HWIO_USB3PHY_QSERDES_RX_AC_JTAG_INITN_OUT(0x03);
  
  //5.  Read QSERDES_RX_AC_JTAG_OUTP (expect value is 0x01) and  QSERDES_RX_AC_JTAG_OUTN (expect value is 0x00).
  ac_jtag_outp = HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_INM(HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_BMSK) >> HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_SHFT;
  ac_jtag_outn = HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_INM(HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_BMSK) >> HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_SHFT;
  if ((ac_jtag_outp == 0x01) && (ac_jtag_outn == 0x00))
  {
    //6.  Write 0x03 to QSERDES_RX_AC_JTAG_INITP and 0x02 to QSERDES_RX_AC_JTAG_INITN to reset/set ac_jtag_outp/n.
    HWIO_USB3PHY_QSERDES_RX_AC_JTAG_INITP_OUT(0x03);
    HWIO_USB3PHY_QSERDES_RX_AC_JTAG_INITN_OUT(0x02);
   
    //7.  Read QSERDES_RX_AC_JTAG_OUTP (expect value is 0x00) and  QSERDES_RX_AC_JTAG_OUTN (expect value is 0x01).
    ac_jtag_outp = HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_INM(HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_BMSK) >> HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_SHFT;
    ac_jtag_outn = HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_INM(HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_BMSK) >> HWIO_USB3PHY_QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_SHFT;
    if ((ac_jtag_outp == 0x00) && (ac_jtag_outn == 0x01))
    {
      ss_phy_present = TRUE;
    }
  }

  //If any of the reads at step 5 or step 7 do not match the expected values, then it can be assumed that the 0.9V analog supply is off.
  //If this is the case, then the PHY should be placed back in powerdown mode by writing 0x00 to PCIE_USB3_PHY_POWER_DOWN_CONTROL.
  if(FALSE == ss_phy_present)
  {
    HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_OUTM(HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_BMSK, (0x0 << HWIO_USB3PHY_PCIE_USB3_PHY_POWER_DOWN_CONTROL_SW_PWRDN_B_SHFT));
  }

  qusb_error_log(QUSB_SS_PHY_PRESENT, 0, ss_phy_present);
  return ss_phy_present;
}

