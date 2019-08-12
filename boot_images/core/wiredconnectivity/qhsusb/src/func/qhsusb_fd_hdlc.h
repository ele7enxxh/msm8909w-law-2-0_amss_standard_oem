#ifndef QHSUSB_FD_HDLC_H
#define QHSUSB_FD_HDLC_H
/*=======================================================================*//**
  @file qhsusb_fd_hdlc.h

  Functional driver for download function

  @file         qhsusb_fd_hdlc.h
 
  @brief        Skeleton documentation example
 
  @details      There is no standard class for download. 
                Instead, vendor-specific interface with 
                2 bulk EP (1-IN, 1-OUT) get used.
 
               Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/*  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_fd_hdlc.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_urb.h"
#include "qhsusb_ch9.h" /* for USB descriptor types */
#include "qhsusb_al_bulk.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* Status of HDLC transfers */
enum qhsusb_fd_hdlc_error
{
  FD_HDLC_SUCCESS         = 0x0,       /* success */
  FD_HDLC_ERR_PERM        = 0x1,       /* operation not permitted */
  FD_HDLC_ERR_NO_MEM      = 0x2,       /* out of heap memory */
  FD_HDLC_ERR_CANCEL      = 0x3,       /* transfer cancelled */
  FD_HDLC_ERR_SUSPENDED   = 0x4,       /* device is suspended */
  FD_HDLC_ERR_ENUMERATION = 0x5,       /* enumeration failed */
  FD_HDLC_ERR_TIMEOUT     = 0x6,       /* operation timed out */
  FD_HDLC_ERR_BAD_PARAM   = 0x7,       /* bad parameter */
  FD_HDLC_ERR_UNKNOWN     = 0x8,       /* general error */
  FD_HDLC_ERR_READ        = 0x9,       /* read error */
  FD_HDLC_ERR_WRITE       = 0xA,       /* write error */
  FD_HDLC_LAST_ERROR      = 0x7FFFFFFF /* last error code */
};

typedef enum
{
  FD_CLIENT_SUPPLIED_BUFFER_MODE = 0x0, /* default mode */
  FD_USB_INTERNAL_BUFFER_MODE = 0x1 /* legacy mode */
} qhsusb_transfer_mode;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
/**
 * Should be set prior to call to \a qhsusb_fd_hdlc_init
 * 
 * Defines flavor for USB interface used:
 * 
 * @param use_cdc_acm
 *               - 1 - use CDC/ACM class
 *               - 0 - use "obex" like vendor specific class
 */
void qhsusb_hdlc_use_cdcacm(int use_cdc_acm);
/**
 * Query whether CDC/ACM used
 * 
 * @return 
 */
int qhsusb_hdlc_get_use_cdcacm(void);
#endif

/**
 * Set USB core to use
 *
 * Should be called prior to qhsusb_fd_hdlc_init
 *
 * @param core_id Core to use,
 *                - 0 - regular one (default)
 *                - 1 - HSIC
 */
void qhsusb_fd_hdlc_set_core(int core_id);

/**
 * Init
 * 
 * @note Should be called prior to any other functions
 * @param rx_callback
 *               URB completion callback,
 *               will be called for every successfully received URB
 * @param str_product
 *               Product name string
 * @param tx_callback
 *                   URB completion callback,
 *                   will be called for every successfully transmitted URB
 * @param enum_complete
 *                   Enumeration completion callback, will be
 *                   called after enumeration is complete
 * 
 * @return device software descriptor, NULL if error
 */
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product,
                                                  void (*tx_callback)(struct qhsusb_urb* urb),
                                                  void (*enum_complete)(void),
												  void (*error_handler)(void),
                                                  qhsusb_transfer_mode transfer_mode);
#else
struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product,
                                                  void (*tx_callback)(struct qhsusb_urb* urb),
                                                  void (*enum_complete)(void),
                                                  qhsusb_transfer_mode transfer_mode);
#endif
/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qhsusb_fd_hdlc_shutdown(void);

/**
 * schedule byte for Tx
 * 
 * @attention May block till USB finish data transfer.
 * 
 */
void qhsusb_fd_hdlc_tx_byte(unsigned char chr);


/**
 * schedule pkt for Tx
 * 
 * @attention May block till USB finish data transfer.
 * 
 */
void qhsusb_fd_hdlc_tx_pkt(unsigned char *pkt, uint32 len);


/**
 * Query whether USB is busy doing Tx.
 * 
 * @return - 0 : no transfer in progress
 *         - any other value : USB is not finished transfers
 */
int qhsusb_fd_hdlc_is_tx_active(void);

/**
 * Indicate Rx URB processing by upper layers is finished
 * and URB may be re-used.
 */

uint32 qhsusb_fd_hdlc_rx_pkt(void *buff_ptr, uint32 len);

void qhsusb_fd_hdlc_rx_completed(struct qhsusb_urb* urb);

/* USB internal buffer mode for clients that does not pass a buffer */
void qhsusb_fd_hdlc_rx_completed_int_buf_mode(struct qhsusb_urb* urb);

/*API to inform the lower layers that Bulk Download is in Progress*/
void qhsusb_fd_hdlc_bulk_in_progress(boolean is_bulk_progress);


/**
 * @function    qhsusb_fd_hdlc_update_vid_pid
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
 * @see     qhsusb_al_update_vid_pid
 * 
 */
void qhsusb_fd_hdlc_update_vid_pid(uint16 idVendor, uint16 idProduct);

/**
 * @function    qhsusb_fd_hdlc_get_vid_pid
 * 
 * @brief       This function returns vid/pid from hdlc_usb_device structure.
 *
 * @param   *idVendor
 *          vendor-id pointer which would be populated with vendor id by this function.
 * 
 * @param   *idProduct
 *          product-id pointer which would be populated with product id by this function.
 * 
 * @param   is_cdc_acm_device
 *          nonzero (1) value if standard CDC/ACM class and PID/VID is in use.
 *
 * @return  none
 * 
 * @see     qhsusb_al_get_vid_pid
 * 
 */
void qhsusb_fd_hdlc_get_vid_pid(uint16 *idVendor, uint16 *idProduct,
                                                  uint8   is_cdc_acm_dev);
/**
 * @function    qhsusb_fd_set_bulk_mode
 * 
 * @brief       This function sets the bulk mode as only serial 
 *              or serial and mass storage 
 *
 * @param   qhsusb_mode
 *          bulk mode enumeration which tells what BULK mode is
 *          to be used.
 * @return  none
 * 
 * @see     none
 * 
 */
void qhsusb_fd_set_bulk_mode(qhsusb_bulk_mode_type qhsusb_mode);												  

#endif /* #ifndef QHSUSB_FD_HDLC_H */

