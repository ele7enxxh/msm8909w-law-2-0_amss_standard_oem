#ifndef QHSUSB_DCI_HW_28NM_PHY_REGISTERS_H
#define QHSUSB_DCI_HW_28NM_PHY_REGISTERS_H

//============================================================================
/**
 * @file        qhsusb_dci_hw_28nm_phy_registers.h
 * @author      ddpatnaik
 * @date        7-Oct-2010
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) HW 
 *              interface. Implementation for the PBL image.
 *
 * 
 *
 *              Copyright (c) 2010 Qualcomm Technologies Incorporated.
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
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_hw_28nm_phy_registers.h#1 $
// 
// when          who       what, where, why
// ----------   -----      ----------------------------------------------------------
// 2010-10-07   dpatnaik   First Draft
// 
// ===========================================================================
 
// 28nm PHY Registers for Charger detection and related functions

/* Charger Detection Control - Read/Write */
/*------------------------------------------------------------------------
|rw-0b00  |rw-0b0 |rw-0b0 |rw-0b0  |rw-0b0      |rw-0b0     |rw-0b0
|RESERVED |ACAENB |DCDENB |CHRGSEL |VDATSRCAUTO |VDATSRCENB |VDATDETENB
-------------------------------------------------------------------------*/
#define USB_OTG_HS_CHGDET_CTL_REG_ADDR              (0x84)
#define USB_OTG_HS_CHGDET_CTL_REG_RST_VAL           (0x00)
#define USB_OTG_HS_CHGDET_CTL_REG_NULL_MASK         (0xFF)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_CHGDET_CTL_ACAENB_POS            (5)
#define USB_OTG_HS_CHGDET_CTL_ACAENB_MASK           (0x1 << USB_OTG_HS_CHGDET_CTL_ACAENB_POS)
#define USB_OTG_HS_CHGDET_CTL_ACAENB_SET            (0x1 << USB_OTG_HS_CHGDET_CTL_ACAENB_POS)
#define USB_OTG_HS_CHGDET_CTL_ACAENB_RST            (0x0 << USB_OTG_HS_CHGDET_CTL_ACAENB_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_CHGDET_CTL_DCDENB_POS            (4)
#define USB_OTG_HS_CHGDET_CTL_DCDENB_MASK           (0x1 << USB_OTG_HS_CHGDET_CTL_DCDENB_POS)
#define USB_OTG_HS_CHGDET_CTL_DCDENB_SET            (0x1 << USB_OTG_HS_CHGDET_CTL_DCDENB_POS)
#define USB_OTG_HS_CHGDET_CTL_DCDENB_RST            (0x0 << USB_OTG_HS_CHGDET_CTL_DCDENB_POS)

/* 0-Source on Dp (default), 1-Source on Dm */
#define USB_OTG_HS_CHGDET_CTL_CHRGSEL_POS           (3)
#define USB_OTG_HS_CHGDET_CTL_CHRGSEL_MASK          (0x1 << USB_OTG_HS_CHGDET_CTL_CHRGSEL_POS)
#define USB_OTG_HS_CHGDET_CTL_CHRGSEL_SET           (0x1 << USB_OTG_HS_CHGDET_CTL_CHRGSEL_POS)
#define USB_OTG_HS_CHGDET_CTL_CHRGSEL_RST           (0x0 << USB_OTG_HS_CHGDET_CTL_CHRGSEL_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_POS       (2)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_MASK      (0x1 << USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_SET       (0x1 << USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_RST       (0x0 << USB_OTG_HS_CHGDET_CTL_VDATSRCAUTO_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_CHGDET_CTL_VDATSRCENB_POS        (1)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCENB_MASK       (0x1 << USB_OTG_HS_CHGDET_CTL_VDATSRCENB_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCENB_SET        (0x1 << USB_OTG_HS_CHGDET_CTL_VDATSRCENB_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATSRCENB_RST        (0x0 << USB_OTG_HS_CHGDET_CTL_VDATSRCENB_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_CHGDET_CTL_VDATDETENB_POS        (0)
#define USB_OTG_HS_CHGDET_CTL_VDATDETENB_MASK       (0x1 << USB_OTG_HS_CHGDET_CTL_VDATDETENB_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATDETENB_SET        (0x1 << USB_OTG_HS_CHGDET_CTL_VDATDETENB_POS)
#define USB_OTG_HS_CHGDET_CTL_VDATDETENB_RST        (0x0 << USB_OTG_HS_CHGDET_CTL_VDATDETENB_POS)


/* Charger Detection Output - Read Only */
/*-------------------------------------------------------------------------
|r-0b000   |r-0b000 |r-0b0  |r-0b0  
|RESERVED  |RID     |DCDOUT |CHGDET 
-------------------------------------------------------------------------*/
#define USB_OTG_HS_CHGDET_OUT_REG_ADDR            (0x87)
#define USB_OTG_HS_CHGDET_OUT_REG_RST_VAL         (0x00)
#define USB_OTG_HS_CHGDET_OUT_REG_NULL_MASK       (0xFF)

#define USB_OTG_HS_CHGDET_OUT_RID_POS             (2)
#define USB_OTG_HS_CHGDET_OUT_RID_MASK            (0x7 << USB_OTG_HS_CHGDET_OUT_RID_POS)

#define USB_OTG_HS_CHGDET_OUT_DCDOUT_POS          (1)
#define USB_OTG_HS_CHGDET_OUT_DCDOUT_MASK         (0x1 << USB_OTG_HS_CHGDET_OUT_DCDOUT_POS)
#define USB_OTG_HS_CHGDET_OUT_DCDOUT_SET          (0x1 << USB_OTG_HS_CHGDET_OUT_DCDOUT_POS)

#define USB_OTG_HS_CHGDET_OUT_CHGDET_POS          (0)
#define USB_OTG_HS_CHGDET_OUT_CHGDET_MASK         (0x1 << USB_OTG_HS_CHGDET_OUT_CHGDET_POS)
#define USB_OTG_HS_CHGDET_OUT_CHGDET_SET          (0x1 << USB_OTG_HS_CHGDET_OUT_CHGDET_POS)

/* Alternate Interrupt Latch - Read/Write */
/*-----------------------------------------------------------------------
|rw-0b000 |rw-0b0   |rw-0b0   |rw-0b0    |rw-0b0       |rw-0b0     
|RESERVED |DMINTLCH |DPINTLCH |DCDINTLCH |CHGDETINTLCH |ACADETINTLCH 
------------------------------------------------------------------------*/
#define USB_OTG_HS_ALTINTR_LATCH_REG_ADDR              (0x91)
#define USB_OTG_HS_ALTINTR_LATCH_REG_RST_VAL           (0x00)
#define USB_OTG_HS_ALTINTR_LATCH_REG_NULL_MASK         (0xFF)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_LATCH_DMINT_POS             (4)
#define USB_OTG_HS_ALTINTR_LATCH_DMINT_MASK            (0x1 << USB_OTG_HS_ALTINTR_LATCH_DMINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DMINT_SET             (0x1 << USB_OTG_HS_ALTINTR_LATCH_DMINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DMINT_RST             (0x0 << USB_OTG_HS_ALTINTR_LATCH_DMINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_LATCH_DPINT_POS             (3)
#define USB_OTG_HS_ALTINTR_LATCH_DPINT_MASK            (0x1 << USB_OTG_HS_ALTINTR_LATCH_DPINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DPINT_SET             (0x1 << USB_OTG_HS_ALTINTR_LATCH_DPINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DPINT_RST             (0x0 << USB_OTG_HS_ALTINTR_LATCH_DPINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_LATCH_DCDINT_POS            (2)
#define USB_OTG_HS_ALTINTR_LATCH_DCDINT_MASK           (0x1 << USB_OTG_HS_ALTINTR_LATCH_DCDINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DCDINT_SET            (0x1 << USB_OTG_HS_ALTINTR_LATCH_DCDINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_DCDINT_RST            (0x0 << USB_OTG_HS_ALTINTR_LATCH_DCDINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_POS         (1)
#define USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_MASK        (0x1 << USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_SET         (0x1 << USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_RST         (0x0 << USB_OTG_HS_ALTINTR_LATCH_CHGDETINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_LATCH_ACADETINT_POS         (0)
#define USB_OTG_HS_ALTINTR_LATCH_ACADETINT_MASK        (0x1 << USB_OTG_HS_ALTINTR_LATCH_ACADETINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_ACADETINT_SET         (0x1 << USB_OTG_HS_ALTINTR_LATCH_ACADETINT_POS)
#define USB_OTG_HS_ALTINTR_LATCH_ACADETINT_RST         (0x0 << USB_OTG_HS_ALTINTR_LATCH_ACADETINT_POS)

/* Alternate Interrupt Enable - Read/Write */
/*-----------------------------------------------------------------------
|rw-0b000 |rw-0b0   |rw-0b0   |rw-0b0    |rw-0b0       |rw-0b0     
|RESERVED |DMINTENB |DPINTENB |DCDINTENB |CHGDETINTENB |ACADETINTENB 
------------------------------------------------------------------------*/
#define USB_OTG_HS_ALTINTR_ENABLE_REG_ADDR             (0x93)
#define USB_OTG_HS_ALTINTR_ENABLE_REG_RST_VAL          (0x00)
#define USB_OTG_HS_ALTINTR_ENABLE_REG_NULL_MASK        (0xFF)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_ENABLE_DMINT_POS             (4)
#define USB_OTG_HS_ALTINTR_ENABLE_DMINT_MASK            (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DMINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DMINT_SET             (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DMINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DMINT_RST             (0x0 << USB_OTG_HS_ALTINTR_ENABLE_DMINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_ENABLE_DPINT_POS             (3)
#define USB_OTG_HS_ALTINTR_ENABLE_DPINT_MASK            (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DPINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DPINT_SET             (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DPINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DPINT_RST             (0x0 << USB_OTG_HS_ALTINTR_ENABLE_DPINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_ENABLE_DCDINT_POS            (2)
#define USB_OTG_HS_ALTINTR_ENABLE_DCDINT_MASK           (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DCDINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DCDINT_SET            (0x1 << USB_OTG_HS_ALTINTR_ENABLE_DCDINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_DCDINT_RST            (0x0 << USB_OTG_HS_ALTINTR_ENABLE_DCDINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_POS         (1)
#define USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_MASK        (0x1 << USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_SET         (0x1 << USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_RST         (0x0 << USB_OTG_HS_ALTINTR_ENABLE_CHGDETINT_POS)

/* 0-No interrupt (default), 1-Interrupt occured */
#define USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_POS         (0)
#define USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_MASK        (0x1 << USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_SET         (0x1 << USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_POS)
#define USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_RST         (0x0 << USB_OTG_HS_ALTINTR_ENABLE_ACADETINT_POS)

// TODO for 28nm PHY
/* Function Control : 45nm 
--------------------------------------------------------------------------------
|r-0b0    |r-0b1    |rw-0b0 |rw-0b01 |rw-0b0     |rw-0b01
|reserved |suspendM |reset  |opmode  |termselect |xcvrselect
------------------------------------------------------------------------------*/
#define USB_OTG_HS_FNCTL_REG_ADDR             (0x04)
#define USB_OTG_HS_FNCTL_REG_RST_VAL          (0x49)

#define USB_OTG_HS_FNCTL_XCVRSELECT_POS           (0)
#define USB_OTG_HS_FNCTL_XCVRSELECT_MASK          (0x3 << USB_OTG_HS_FNCTL_XCVRSELECT_POS) 
#define USB_OTG_HS_FNCTL_XCVRSELECT_HS_VAL        (0x0 << USB_OTG_HS_FNCTL_XCVRSELECT_POS)
#define USB_OTG_HS_FNCTL_XCVRSELECT_FS_VAL        (0x1 << USB_OTG_HS_FNCTL_XCVRSELECT_POS)


#define USB_OTG_HS_FNCTL_TERMSELECT_POS           (2)
#define USB_OTG_HS_FNCTL_TERMSELECT_MASK          (0x1 << USB_OTG_HS_FNCTL_TERMSELECT_POS) 
#define USB_OTG_HS_FNCTL_TERMSELECT_ENABLE_VAL    (0x1 << USB_OTG_HS_FNCTL_TERMSELECT_POS)
#define USB_OTG_HS_FNCTL_TERMSELECT_DISABLE_VAL   (0x0 << USB_OTG_HS_FNCTL_TERMSELECT_POS)

/* Mode of operation:
 * 00-normal drive.
 * 01-non-drive.
 * 10-disable bit-stuff and NRZI encoding.
 * 11-no SYNC and EOP generation. */
#define USB_OTG_HS_FNCTL_OPMODE_POS           (3)
#define USB_OTG_HS_FNCTL_OPMODE_MASK          (0x3 << USB_OTG_HS_FNCTL_OPMODE_POS) 
#define USB_OTG_HS_FNCTL_OPMODE_NORMAL_VAL    (0x0 << USB_OTG_HS_FNCTL_OPMODE_POS)
#define USB_OTG_HS_FNCTL_OPMODE_NONDRIVE_VAL  (0x1 << USB_OTG_HS_FNCTL_OPMODE_POS)

/* 0 -  Low power mode for PHY
   1 -  Powered mode for PHY
*/
#define USB_OTG_HS_FNCTL_SUSPM_POS                (6)
#define USB_OTG_HS_FNCTL_SUSPM_MASK               (0x3 << USB_OTG_HS_FNCTL_SUSPM_POS) 
#define USB_OTG_HS_FNCTL_SUSPM_POWERED_VAL        (0x1 << USB_OTG_HS_FNCTL_SUSPM_POS)
#define USB_OTG_HS_FNCTL_SUSPM_SUSPEND_VAL        (0x0 << USB_OTG_HS_FNCTL_SUSPM_POS)

 //PHY Tuning Registers
 /* Parameter Override A 
 ----------------------------------------------------------------------------------
 |r-0b100    |r-0b0    |rw-0b100    |rw-0b0 
 |otgtune	 |reserved |compdistune |reserved
 ----------------------------------------------------------------------------------*/
 #define USB_OTG_PARAMETER_OVERRIDE_A_ADDR    (0x80)
 #define USB_OTG_HS_TUNE_POS				  (0)
 #define USB_OTG_HS_COMP_DISTUNE_POS		  (4)

 /* Parameter Override B 
 ----------------------------------------------------------------------------------
 |r-0b011    |rw-0b011    |r-0b0     
 |txvreftune |sqrxtune    |reserved 
 ----------------------------------------------------------------------------------*/
 #define USB_OTG_PARAMETER_OVERRIDE_B_ADDR    (0x81)
 #define USB_OTG_HS_TX_VREF_TUNE_POS		  (0)
 #define USB_OTG_HS_SQRX_TUNE_POS		      (4)
 #define USB_OTG_HS_SQRX_TUNE_MASK			  (0x7 << USB_OTG_HS_SQRX_TUNE_POS)
 #define USB_OTG_HS_SQRX_DEFAULT_VAL		  (0x6 << USB_OTG_HS_SQRX_TUNE_POS)
 

 /* Parameter Override C 
 ----------------------------------------------------------------------------------
 |rw-0b11    |rw-0b010     |rw-0b00 		|rw-0b0  		   |r-0b0     
 |txhsxvtune |txrisetune   |txpreempamptune |txpreemppulsetune |reserved
 ----------------------------------------------------------------------------------*/
 #define USB_OTG_PARAMETER_OVERRIDE_C_ADDR    (0x82)
 #define USB_OTG_HS_TX_XV_TUNE_POS 		      (0)
 #define USB_OTG_HS_TX_RISE_TUNE_POS		  (2)
 #define USB_OTG_HS_TX_PRE_EMP_AMP_TUNE_POS   (4)
 #define USB_OTG_HS_TX_PRE_EMP_PULSE_TUNE_POS (6)

 /* Parameter Override D 
 ----------------------------------------------------------------------------------
 |r-0b0011    |rw-0b01    |r-0b00     
 |txfslstune |txrestune    |reserved 
 ----------------------------------------------------------------------------------*/
 #define USB_OTG_PARAMETER_OVERRIDE_D_ADDR    (0x83)
 #define USB_OTG_HS_TX_FS_LS_TUNE_POS		  (0)
 #define USB_OTG_HS_TX_RES_TUNE_POS		      (4)

/* OTG Control : 28 nm 
------------------------------------------------------------------------------------------------------
|rw-0b0    				  |rw-0b0          |rw-0b0   |r-0b0     |r-0b0    |rw-0b1    |rw-0b1    |rw-0b0 
|UseExternalVBusIndicator |DrvVbusExternal |DrvVbus  |reserved  |reserved |DmPulldown|DpPullDown|IdPullUp
--------------------------------------------------------------------------------------------------------*/
#define USB_OTG_HS_OTG_CONTROL_REG_ADDR			(0x0A)
#define USB_OTG_HS_OTG_CONTROL_REG_RST_VAL		(0x07)


#define USB_OTG_HS_OTG_CONTROL_ID_PULLUP_POS		(0)

#define USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_POS		(1)
#define USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_MASK		(0x1 << USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_POS)
#define USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_DISABLE  (0x0 << USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_POS)
#define USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_ENABLE 	(0x1 << USB_OTG_HS_OTG_CONTROL_DP_PULLDOWN_POS)

#define USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_POS		(2)
#define USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_MASK		(0x1 << USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_POS)
#define USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_DISABLE	(0x0 << USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_POS)
#define USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_ENABLE	(0x1 << USB_OTG_HS_OTG_CONTROL_DM_PULLDOWN_POS)

#define USB_OTG_HS_OTG_CONTROL_DRV_VBUS				(5)
#define USB_OTG_HS_OTG_CONTROL_DRV_VBUS_EXTERNAL 	(6)
#define USB_OTG_HS_OTG_CONTROL_USE_EXT_VBUS_IND		(7)

/* Miscellaneous register for external VBUS control */
/*-------------------------------------------------------------------------
|r-0b000000  |rw-0b0        |rw-0b0  
|RESERVED    |VBUSVLDEXTSEL |VBUSVLDEXT
-------------------------------------------------------------------------*/
#define USB_OTG_HS_MISC_A_REG_ADDR              (0x96)
#define USB_OTG_HS_MISC_A_REG_RST_VAL           (0x00)
#define USB_OTG_HS_MISC_A_REG_NULL_MASK         (0xFF)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_POS     (1)
#define USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_MASK    (0x1 << USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_POS)
#define USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_SET     (0x1 << USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_POS)
#define USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_RST     (0x0 << USB_OTG_HS_MISC_A_VBUSVLDEXTSEL_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HS_MISC_A_VBUSVLDEXT_POS        (0)
#define USB_OTG_HS_MISC_A_VBUSVLDEXT_MASK       (0x1 << USB_OTG_HS_MISC_A_VBUSVLDEXT_POS)
#define USB_OTG_HS_MISC_A_VBUSVLDEXT_SET        (0x1 << USB_OTG_HS_MISC_A_VBUSVLDEXT_POS)
#define USB_OTG_HS_MISC_A_VBUSVLDEXT_RST        (0x0 << USB_OTG_HS_MISC_A_VBUSVLDEXT_POS)

/* Debug register that tells us line-state */
/*-------------------------------------------------------------------------
|r-0b000000  |rw-0b0        |rw-0b0  
|Reserved    |LineState1    |LineState0
-------------------------------------------------------------------------*/
#define USB_OTG_HS_DEBUG_REG_ADDR			    (0x15)


#endif // QHSUSB_DCI_HW_28NM_PHY_REGISTERS_H
