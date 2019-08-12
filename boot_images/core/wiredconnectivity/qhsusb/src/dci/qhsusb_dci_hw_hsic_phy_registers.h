#ifndef QHSUSB_DCI_HW_HSIC_PHY_REGISTERS_H
#define QHSUSB_DCI_HW_HSIC_PHY_REGISTERS_H

//============================================================================
/**
 * @file        qhsusb_dci_hw_hsic_phy_registers.h
 * @author      dpatnaik
 * @date        8-Feb-2011
 *
 * @brief       HSIC configuration register interface
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
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_hw_hsic_phy_registers.h#1 $
// 
// when          who       what, where, why
// ----------   -----      ----------------------------------------------------------
// 2011-11-08   dpatnaik   First Draft
// 
// ===========================================================================
 
// HSIC PHY Registers for HSIC configuration and calibration

/* HSIC Config register - Read/Write */
/*-----------------------------------------------------------------------------------
 rw-0b0 |rw-0b0 |rw-0b0      |rw-0b0    |rw-0b0    |rw-0b0    |rw-0b0    |rw-0b0
 IDVLD  |ID     |IOCALPERENB |RSTDETENB |IOCALBGEN |IOCALERR  |IOCALDONE |HSICENB
-------------------------------------------------------------------------------------*/
#define USB_OTG_HSIC_CFG_REG_ADDR             (0x30)
#define USB_OTG_HSIC_CFG_REG_RST_VAL          (0x00)
#define USB_OTG_HSIC_CFG_REG_NULL_MASK_VAL    (0xFF)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_IDVLD_POS        (7)
#define USB_OTG_HSIC_CFG_REG_IDVLD_MASK       (0x1 << USB_OTG_HSIC_CFG_REG_IDVLD_POS)
#define USB_OTG_HSIC_CFG_REG_IDVLD_SET        (0x1 << USB_OTG_HSIC_CFG_REG_IDVLD_POS)
#define USB_OTG_HSIC_CFG_REG_IDVLD_RST        (0x0 << USB_OTG_HSIC_CFG_REG_IDVLD_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_ID_POS           (6)
#define USB_OTG_HSIC_CFG_REG_ID_MASK          (0x1 << USB_OTG_HSIC_CFG_REG_ID_POS)
#define USB_OTG_HSIC_CFG_REG_ID_SET           (0x1 << USB_OTG_HSIC_CFG_REG_ID_POS)
#define USB_OTG_HSIC_CFG_REG_ID_RST           (0x0 << USB_OTG_HSIC_CFG_REG_ID_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_IOCALPERENB_POS  (5)
#define USB_OTG_HSIC_CFG_REG_IOCALPERENB_MASK (0x1 << USB_OTG_HSIC_CFG_REG_IOCALPERENB_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALPERENB_SET  (0x1 << USB_OTG_HSIC_CFG_REG_IOCALPERENB_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALPERENB_RST  (0x0 << USB_OTG_HSIC_CFG_REG_IOCALPERENB_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_RSTDETENB_POS    (4)
#define USB_OTG_HSIC_CFG_REG_RSTDETENB_MASK   (0x1 << USB_OTG_HSIC_CFG_REG_RSTDETENB_POS)
#define USB_OTG_HSIC_CFG_REG_RSTDETENB_SET    (0x1 << USB_OTG_HSIC_CFG_REG_RSTDETENB_POS)
#define USB_OTG_HSIC_CFG_REG_RSTDETENB_RST    (0x0 << USB_OTG_HSIC_CFG_REG_RSTDETENB_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_IOCALBGEN_POS    (3)
#define USB_OTG_HSIC_CFG_REG_IOCALBGEN_MASK   (0x1 << USB_OTG_HSIC_CFG_REG_IOCALBGEN_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALBGEN_SET    (0x1 << USB_OTG_HSIC_CFG_REG_IOCALBGEN_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALBGEN_RST    (0x0 << USB_OTG_HSIC_CFG_REG_IOCALBGEN_POS)

/* 0-success (default), 1-error */
#define USB_OTG_HSIC_CFG_REG_IOCALERR_POS     (2)
#define USB_OTG_HSIC_CFG_REG_IOCALERR_MASK    (0x1 << USB_OTG_HSIC_CFG_REG_IOCALERR_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALERR_SET     (0x1 << USB_OTG_HSIC_CFG_REG_IOCALERR_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALERR_RST     (0x0 << USB_OTG_HSIC_CFG_REG_IOCALERR_POS)

/* 0-new sequence (default), 1-sequence success */
#define USB_OTG_HSIC_CFG_REG_IOCALDONE_POS    (1)
#define USB_OTG_HSIC_CFG_REG_IOCALDONE_MASK   (0x1 << USB_OTG_HSIC_CFG_REG_IOCALDONE_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALDONE_SET    (0x1 << USB_OTG_HSIC_CFG_REG_IOCALDONE_POS)
#define USB_OTG_HSIC_CFG_REG_IOCALDONE_RST    (0x0 << USB_OTG_HSIC_CFG_REG_IOCALDONE_POS)

/* 0-disable (default), 1-enable */
#define USB_OTG_HSIC_CFG_REG_HSICENB_POS      (0)
#define USB_OTG_HSIC_CFG_REG_HSICENB_MASK     (0x1 << USB_OTG_HSIC_CFG_REG_HSICENB_POS)
#define USB_OTG_HSIC_CFG_REG_HSICENB_SET      (0x1 << USB_OTG_HSIC_CFG_REG_HSICENB_POS)
#define USB_OTG_HSIC_CFG_REG_HSICENB_RST      (0x0 << USB_OTG_HSIC_CFG_REG_HSICENB_POS)

/* HSIC IO Calibration period register - Read/Write */
/*-----------------------------------------------------------------------------------*/
#define USB_OTG_HSIC_IOCALPER_REG_ADDR          (0x33)
#define USB_OTG_HSIC_IOCALPER_REG_MIN_VAL       (0x01)
#define USB_OTG_HSIC_IOCALPER_REG_MAX_VAL       (0xFF)
#define USB_OTG_HSIC_IOCALPER_REG_NULL_MASK_VAL (0xFF)

#endif // QHSUSB_DCI_HW_HSIC_PHY_REGISTERS_H
