#ifndef QHSUSB_FD_MS_H
#define QHSUSB_FD_MS_H
/*=======================================================================*//**
  @file qhsusb_fd_ms.h

  Functional driver for mass storage.

  @file         qhsusb_fd_ms.h
 
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header:  */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_urb.h"
#include "qhsusb_ch9.h" /* for USB descriptor types */
#include "fs_hotplug_config.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TO_HOST 1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define USB_SETUP_REQ_MS_RESET          0xff
#define USB_SETUP_REQ_MS_GET_MAX_LUN    0xfe


#ifndef  QHSUSB_MS_RX_BUFFER_SIZE
#define QHSUSB_MS_RX_BUFFER_SIZE (64*1024)  
#endif

#ifndef  QHSUSB_MS_TX_BUFFER_SIZE
#define QHSUSB_MS_TX_BUFFER_SIZE (64*1024) 
#endif

#define MS_RX_BUF_SIZE (QHSUSB_MS_RX_BUFFER_SIZE) /**< size of each (of 2) Rx buffers */
#define MS_TX_BUF_SIZE (QHSUSB_MS_TX_BUFFER_SIZE) /**< size of  Tx buffer */


typedef enum {
  LUN_STATUS_NOT_READY,
  LUN_STATUS_READY
}hsu_al_ms_lun_status_enum;

typedef enum {
  PREPARE_FOR_CBW,
  SEND_TO_HOST,
  RCV_FROM_HOST,
  HOST_READ,
  SEND_CSW
}xfer_status_enum;

typedef struct 
{
  /* LUN index */
  uint8                             index;
  /* LUN name */
  char                              hdev_name[FS_HOTPLUG_NAME_MAX]; 
  /* Is LUN enable/disable? */
  boolean                           enabled;
  /* Is LUn removable?*/
  boolean                           removable;
  /* LUN status */
  hsu_al_ms_lun_status_enum         status; 
  /* Argument to the block device functions */
  void*                             context;/* hotplug context */
  /* For the block device information */
  uint32                            block_size;
  uint32                            num_blocks;
  boolean                           read_only;
}lun_info_type;

#define MAX_NUM_LUNS 1
#define MS_EP_IDX 2

typedef struct 
{
  uint8           num_luns;
  lun_info_type   luns[MAX_NUM_LUNS];
}device_info_type;
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//============================================================================
/**
 * @function  qhsusb_fd_ms_is_tx_active
 *
 * @brief check if there is Tx transfer active.
 *
 * @note
 *
 * @param None.
 *
 * @return True if MS Tx is active, False otherwise..
 *
 */
//============================================================================
boolean qhsusb_fd_ms_is_tx_active(void);
//============================================================================
/**
 * @function  qhsusb_fd_ms_send_request
 *
 * @brief send data to host
 *
 * @note
 *
 * @param buff, len, dir, status.
 *
 * @return status (o is OK)
 *
 */
//============================================================================
uint32 qhsusb_fd_ms_send_request(void* buff, uint32 len, uint8 dir,  xfer_status_enum status);
//============================================================================
/**
 * @function  set_ms_packet_size
 *
 * @brief update MS packet size according to the USB spped (HS/LS)
 *
 * @note
 *
 * @param wMaxPacketSize
 *
 * @return None.
 *
 */
//============================================================================
void set_ms_packet_size(uint16 wMaxPacketSize);
//============================================================================
/**
 * @function  qhsusb_fd_ms_hotplug_dev_read
 *
 * @brief read from eMMC device.
 *
 * @note
 *
 * @param lun_num, start_address, num_blocks
 *
 * @return True on success, False otherwise.
 *
 */
//============================================================================
boolean qhsusb_fd_ms_hotplug_dev_read (uint8 lun_num, uint32  start_address, uint16  num_blocks);
//============================================================================
/**
 * @function  qhsusb_fd_ms_hotplug_dev_write
 *
 * @brief write to eMMC device.
 *
 * @note
 *
 * @param lun_num, start_address, num_blocks
 *
 * @return True on success, False otherwise.
 *
 */
//============================================================================
#ifdef QHSUSB_MS_OPTIMIZATION
boolean qhsusb_fd_ms_hotplug_dev_write(void * buff, uint8 lun_num, uint32  start_address, uint16  num_blocks);
#else
boolean qhsusb_fd_ms_hotplug_dev_write(uint8 lun_num, uint32  start_address, uint16  num_blocks);
#endif
//============================================================================
/**
 * @function  qhsusb_fd_ms_init
 *
 * @brief Initialize urb params for MS Tx/Rx .
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
void qhsusb_fd_ms_init(void);
//============================================================================
/**
 * @function  qhsusb_ms_init
 *
 * @brief init ms parameters after the enum process is over.
 *
 * @note
 *
 * @param device
 *
 * @return None.
 *
 */
//============================================================================
void qhsusb_ms_init(const struct qhsusb_dcd_dsc_device* dev);

#endif /* #ifndef QHSUSB_FD_MS_H */
