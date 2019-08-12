#ifndef QHSUSB_SCSI_H
#define QHSUSB_SCSI_H
/*=======================================================================*//**
  @file qhsusb_scsi.h

  SCSI protocol implementation 

  @file         qhsusb_scsi.h
 
 
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_urb.h"
#include "qhsusb_ch9.h" /* for USB descriptor types */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QHSUSB_CBW_SIGNATURE (0x43425355) 
#define QHSUSB_CSW_SIGNATURE (0x53425355) 
#define QHSUSB_CBW_LEN 31
#define QHSUSB_CSW_LEN 13
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* Command block wrapper definitions */
typedef struct qhsusb_cbw_type {
  uint32  dCBWSignature;        /* QHSUSB_CBW_SIGNATURE */
  uint32  dCBWTag;              /* Command Block Tag */
  uint32  dCBWDataTransferLength;      /* number of data bytes */
  uint8   bmCBWFlags;           /* for direction  (bit #7) */
  uint8   bCBWLUN;              /* LUN number */
  uint8   bCBWCBLength;         /* Command block length (0x1-0x10)*/       
  uint8   CBWCB[16];            /* Command block */
}qhsusb_cbw_t;

/* Command Status Wrapper definitions */
typedef struct qhsusb_csw_type {
  uint32  dCSWSignature;        /* QHSUSB_CSW_SIGNATURE' */
  uint32  dCSWTag;              /* Command Block Tag (same as in cbw) */
  uint32  dCSWDataResidue;      /* the difference between the amount of data expected and the actual amount of data */
  uint8   bCSWStatus;           /* success/failure of the command */
}qhsusb_csw_t;

#ifdef QHSUSB_MS_OPTIMIZATION
typedef enum
{
	QHSUSB_EMMC_WRITE_DONE,
	QHSUSB_EMMC_WRITE_PENDING
}qhsusb_emmc_write_status;
#endif

#define CORE_ID 0


#define MAX_VENDOR_STR_SIZE 8
#define MAX_PRODUCT_STR_SIZE 16
#define MAX_PROD_REV_STR_SIZE 4
#define MAX_INQUERY_RES_SIZE 36

#define MAX_READ_FORMAT_CAPACITY_SIZE 12
#define MAX_READ_CAPACITY_SIZE 8
#define MAX_MODE_SENSE_SIZE 12
#define MAX_REQUEST_SENSE_SIZE 18

/* SCSI operation code */
#define SCSI_TEST_UNIT_READY       0x00
#define SCSI_REQUEST_SENSE         0x03
#define SCSI_INQUIRY               0x12
#define SCSI_MODE_SELECT6          0x15
#define SCSI_MODE_SENSE6           0x1A
#define SCSI_READ_FORMAT_CAPACITY  0x23
#define SCSI_READ_CAPACITY10       0x25
#define SCSI_READ10                0x28
#define SCSI_WRITE10               0x2A
#define SCSI_SYNCHRONIZE_CACHE10   0x35
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL        0x1E
#define SCSI_MAINTENANCE_IN		   0xA3
#define SCSI_BLANK		           0xA1

#define CSW_GOOD_STATUS 0
#define CSW_ERROR_STATUS 1

#define SWAP32(x) (((x&0x000000FF)<<(3*8)) | \
  ((x&0x0000FF00)<<(1*8)) | \
  ((x&0x00FF0000)>>(1*8)) | \
  ((x&0xFF000000)>>(3*8)))

/*----------------------------------------------------------------------------
 * Function Declarations 
 * -------------------------------------------------------------------------*/
void handle_new_cbw(struct qhsusb_urb* urb);
void init_scsi_params(uint8 ** ms_rx_buf_ptr, uint8 ** ms_tx_buf_ptr);
void sending_csw(void);
void write_to_emmc_card(void);
#ifdef QHSUSB_MS_OPTIMIZATION
int8 get_xfer_current_index(void);
#endif

#endif /* QHSUSB_SCSI_H */
