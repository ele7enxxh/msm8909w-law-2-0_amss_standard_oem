#ifndef QHSUSB_FD_CHG_H
#define QHSUSB_FD_CHG_H
/*=======================================================================*//**
  Functional driver for dead battery charger function

  @file         qhsusb_fd_chg.h
 
  @brief        Skeleton documentation example
 
  @details      There is no standard class for download. 
                Instead, vendor-specific interface with 
                2 bulk EP (1-IN, 1-OUT) get used.
 
               Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_fd_chg.h#1 $ */
//
//  when        who       what, where, why
//  ----------  ---      -------------------------------------------------------
//  2010-08-30  dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
//
//*========================================================================
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_urb.h"
#include "qhsusb_ch9.h" /* for USB descriptor types */
#include "qhsusb_dci_api.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Init
 * 
 * Should be called prior to any other functions
 * 
 * @param rx_callback
 *               URB completion callback,
 *               will be called for every successfully received URB
 *  
 * @return device, NULL if error
 */
struct qhsusb_dcd_dsc_device* qhsusb_fd_chg_init(void);

/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qhsusb_fd_chg_shutdown(void);

/**
 * Detects charger port type
 * 
 * Should be called after USB stack initialization
 * 
 * @param 
 *        
 *        
 *  
 * @return Charger port type.
 */
qhsusb_chg_port_type qhsusb_fd_chg_detect_charger_type(void);

/**
 * @function    qhsusb_fd_chg_update_vid_pid
 * 
 * @brief       This function updates vid/pid if vid/pid values are stored
 *              in efuse. API performs change to use vid/pid only if value
 *              is non-zero.
 *
 * @note        This must be called before device enumerates.
 * 
 * @param   idVendor
 *          vendor-id value stored in efuse
 * 
 * @param   idProduct
 *          product-id value stored in efuse
 * 
 * @return  none
 * 
 * @see     qhsusb_al_chg_update_vid_pid
 * 
 */
void qhsusb_fd_chg_update_vid_pid(uint16 idVendor, uint16 idProduct);

/**
 * @function    qhsusb_fd_chg_get_vid_pid
 * 
 * @brief       This function calls device api to retrieve vid/pid from 
 *              chg_usb_device structure.
 *
 * @param   *idVendor
 *          vendor-id pointer which would be populated with vendor id.
 * 
 * @param   *idProduct
 *          product-id pointer which would be populated with product id.
 * 
 *
 * @return  none
 * 
 * @see     qhsusb_al_chg_get_vid_pid
 * 
 */
void qhsusb_fd_chg_get_vid_pid(uint16 *idVendor, uint16 *idProduct);

#endif /* #ifndef QHSUSB_FD_CHG_H */

