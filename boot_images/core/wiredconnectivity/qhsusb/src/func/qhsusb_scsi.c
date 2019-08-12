/*=======================================================================*//**
  @file         qhsusb_scsi.c

  @brief        

  @details      
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: 

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  22/03/2011  sm        initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_scsi.h"
#include "qhsusb_fd_ms.h"
#include "qhsusb_fd_hdlc.h"
#include "qhsusb_dci_private.h"
#include "qhsusb_dcd.h"
#include "qhsusb_dci_api.h"
#include "qhsusb_cdc.h"

#include "qhsusb_ch9.h" /*for the descriptors definitions*/
#include "fs_hotplug.h"
#include "qhsusb_log.h"
#include <stringl/stringl.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
extern struct qhsusb_dcd_dsc_device sw_device;
extern device_info_type ms_devices;
#ifdef QHSUSB_MS_OPTIMIZATION
#define NO_OF_RX_BUF 2
#endif

typedef struct
{
  uint32   lba;
  uint16   blocks_num;
  uint8 lun;
}transfer_type;

static transfer_type current_xfer;
#ifdef QHSUSB_MS_OPTIMIZATION
static boolean qhsusb_is_first_handle_write = TRUE; /*Initially should be true*/
static qhsusb_emmc_write_status qhsusb_emmc_status = QHSUSB_EMMC_WRITE_DONE; 
static transfer_type qhsusb_prev_xfer;
static uint8 qhsusb_ms_tx_buffer[MS_TX_BUF_SIZE];
static uint8 qhsusb_ms_rx_buffer[NO_OF_RX_BUF][MS_RX_BUF_SIZE];
extern int8 qhsusb_curr_index;
#else
static uint8 qhsusb_ms_rx_buffer[MS_RX_BUF_SIZE]; 
static uint8 qhsusb_ms_tx_buffer[MS_TX_BUF_SIZE];
#endif 

extern xfer_status_enum cur_xfer_status;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static qhsusb_cbw_t cbw;
static qhsusb_csw_t csw;
static const char vendor_str[MAX_VENDOR_STR_SIZE] = "Qualcomm";
static const char product_str[MAX_PRODUCT_STR_SIZE] = "MMC Storage     "; 
static const char prod_rev_str[MAX_PROD_REV_STR_SIZE] = "1.00";

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static void handle_scsi_command(void);
static void handle_inquiry(void);
static void handle_read_format_capacity(void);
static void handle_read_capacity(void);
static void handle_mode_sense(void);
static void handle_mode_select(void);
static void handle_request_sense(void);
static void handle_test_unit_ready(void);
static void handle_write(void);
static void handle_read(void);
static void handle_unsupported_scsi_cmd(void);
//============================================================================
/**
 * @function  init_scsi_params
 *
 * @brief initialize csw and Tx/Rx buffers for SCSI
 *
 * @note
 *
 * @param ms_rx_buf_ptr, ms_tx_buf_ptr
 *
 * @return 
 *
 */
//============================================================================
void init_scsi_params(uint8 ** ms_rx_buf_ptr, uint8 ** ms_tx_buf_ptr)
{
  csw.dCSWDataResidue = 0; 
  csw.bCSWStatus = CSW_GOOD_STATUS; 
  csw.dCSWSignature = QHSUSB_CSW_SIGNATURE;
 
  qhsusb_log(INIT_SCSI_PARAMS_LOG,0,0);
#ifdef QHSUSB_MS_OPTIMIZATION
  if((qhsusb_curr_index == 1) || (qhsusb_curr_index == 0))
  {
       *ms_rx_buf_ptr = qhsusb_ms_rx_buffer[qhsusb_curr_index];
  }
  *ms_tx_buf_ptr = qhsusb_ms_tx_buffer;
#else
  *ms_rx_buf_ptr = qhsusb_ms_rx_buffer;
  *ms_tx_buf_ptr = qhsusb_ms_tx_buffer;
#endif
}

//============================================================================
/**
 * @function  stall_ms_eps
 *
 * @brief stall MS EPs 
 *
 * @note
 *
 * @param None.
 *
 * @return None. 
 *
 */
//============================================================================
static void stall_ms_eps(void)
{
/* STALL MS in & out EPs. The device shall maintain this state until a Reset Recovery */
  qhsusb_log(STALL_MS_EPS_LOG,0,0);
  qhsusb_dci_stall_endpoint(CORE_ID, MS_EP_IDX, QHSUSB_EP_DIR_RX);
  qhsusb_dci_stall_endpoint(CORE_ID, MS_EP_IDX, QHSUSB_EP_DIR_TX);
}

//============================================================================
/**
 * @function  is_cbw_valid
 *
 * @brief check if the CBW received from the host is valid.
 *
 * @note
 *
 * @param
 *
 * @return 
 *
 */
//============================================================================
static boolean is_cbw_valid(struct qhsusb_urb* urb)
{
  uint8 cbw_len;

  cbw_len = urb->actual_length;
  memscpy ((void*)&cbw,sizeof(qhsusb_cbw_t),(void*)urb->transfer_buffer_ptr, QHSUSB_CBW_LEN);
  urb->actual_length=0;

  /* check if CBW valid */
  if (cbw.bCBWCBLength && cbw_len == QHSUSB_CBW_LEN && 
    cbw.dCBWSignature == QHSUSB_CBW_SIGNATURE)
  { 
    /* check if CBW meaningful */
    if (cbw.bCBWLUN < MAX_NUM_LUNS)
    {
      current_xfer.lun = cbw.bCBWLUN;
      csw.dCSWTag = cbw.dCBWTag;
	  qhsusb_log(IS_CBW_VALID_LOG,0,TRUE);
      return TRUE;
    }
  }
  qhsusb_log(IS_CBW_VALID_LOG,0,FALSE);
  stall_ms_eps();  
  return FALSE;
}

//============================================================================
/**
 * @function  handle_new_cbw
 *
 * @brief in case the CBW is valid, handle it.
 *
 * @note
 *
 * @param urb
 *
 * @return None.
 *
 */
//============================================================================
void handle_new_cbw(struct qhsusb_urb* urb)
{
  qhsusb_log(HANDLE_NEW_CBW_LOG,0,0);
  if (is_cbw_valid(urb))
  {
    handle_scsi_command();
  }
}

#ifdef QHSUSB_MS_OPTIMIZATION
//============================================================================
/**
 * @function  qhsusb_write_last_data_to_emmc
 *
 * @brief used for writing the last request to emmc
 *
 * @note
 *
 * @param None
 *
 * @return None.
 *
 */
//============================================================================
void qhsusb_write_last_data_to_emmc()
{
  qhsusb_log(QHSUSB_WRITE_LAST_DATA_TO_EMMC_LOG,0,qhsusb_curr_index);
  qhsusb_fd_ms_hotplug_dev_write((void*)qhsusb_ms_rx_buffer[qhsusb_curr_index^1], qhsusb_prev_xfer.lun, qhsusb_prev_xfer.lba, qhsusb_prev_xfer.blocks_num);
}

//============================================================================
/**
 * @function  get_xfer_current_index
 *
 * @brief used for getting the current ping pong buffer number
 *
 * @note
 *
 * @param None
 *
 * @return The buffer index 0 or 1.
 *
 */
//============================================================================
int8 get_xfer_current_index()
{
	return qhsusb_curr_index;
}
#endif

//============================================================================
/**
 * @function  handle_scsi_command
 *
 * @brief Parse the CBW, and respond accordingly.
 *
 * @note
 *
 * @param None.
 *
 * @return None. 
 *
 */
//============================================================================
static void handle_scsi_command(void)
{
  uint8   scsi_cmd;

  scsi_cmd = cbw.CBWCB[0];

  qhsusb_log(HANDLE_SCSI_COMMAND_LOG,0,scsi_cmd);
  
  switch(scsi_cmd) 
  {
  case SCSI_INQUIRY:
    handle_inquiry();
    break;
  case SCSI_READ_FORMAT_CAPACITY:   
    handle_read_format_capacity();
    break;
  case SCSI_READ_CAPACITY10:   
    handle_read_capacity();
    break;
  case SCSI_READ10:
    handle_read();
    break;
  case SCSI_WRITE10:
    handle_write();
    break;
  case SCSI_MODE_SENSE6:  
    handle_mode_sense();
    break;
  case SCSI_REQUEST_SENSE:
    handle_request_sense();
    break;
  case SCSI_MODE_SELECT6:
    handle_mode_select();
    break;
  case SCSI_TEST_UNIT_READY:
    handle_test_unit_ready();
    break;
  /*handle unsupported SCSI commands gracefully*/
  case SCSI_BLANK:
  case SCSI_MAINTENANCE_IN:
	handle_unsupported_scsi_cmd();
	break;
  case SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL:
  case SCSI_SYNCHRONIZE_CACHE10:
    /*assuming not removable media, ignore*/
    sending_csw();
    break;
  default:
    /*For all the other SCSI commands handle gracefully*/
    handle_unsupported_scsi_cmd();
  }
}
//============================================================================
/**
 * @function  handle_unsupported_scsi_cmd
 *
 * @brief handles any unsupported SCSI command gracefully viz MAINTENANCE_IN
 *        BLANK etc.
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_unsupported_scsi_cmd(void)
{
	qhsusb_log(HANDLE_UNSUPPORTED_SCSI_CMD_LOG,0,0);
    qhsusb_dci_stall_endpoint(sw_device.core_id, MS_EP_IDX, QHSUSB_EP_DIR_TX);
    /* send scw with error code */
    csw.dCSWDataResidue = cbw.CBWCB[4];
    csw.bCSWStatus = CSW_ERROR_STATUS;
    sending_csw();	
}
//============================================================================
/**
 * @function  handle_inquiry
 *
 * @brief handle inquiry SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_inquiry(void)
{
  uint8 lun_num = current_xfer.lun; 
  uint8 *temp = qhsusb_ms_tx_buffer;
  qhsusb_log(HANDLE_INQUIRY_LOG,0,0);
  memset (qhsusb_ms_tx_buffer, 0 ,MAX_INQUERY_RES_SIZE);
  
  if (ms_devices.luns[lun_num].removable)
  {
    qhsusb_ms_tx_buffer[1] =  0x80; /* Removable Medium */
  }
  qhsusb_ms_tx_buffer[2] =  2;   /* isoEcmaAnsi */            
  qhsusb_ms_tx_buffer[3] =  2;   /* aenTioDataFormat */          
  qhsusb_ms_tx_buffer[4] =  31;  /* additionalLength */ 
  temp += 8; /* destination address for strings identification  */
  memscpy (temp,(MS_TX_BUF_SIZE -(8*sizeof(uint8))),vendor_str, MAX_VENDOR_STR_SIZE);
  temp += MAX_VENDOR_STR_SIZE;
  memscpy (temp,(MS_TX_BUF_SIZE - ((8*sizeof(uint8))+MAX_VENDOR_STR_SIZE)),product_str, MAX_PRODUCT_STR_SIZE);
  temp += MAX_PRODUCT_STR_SIZE;
  memscpy (temp,(MS_TX_BUF_SIZE - ((8*sizeof(uint8))+MAX_VENDOR_STR_SIZE+MAX_PRODUCT_STR_SIZE)),prod_rev_str, MAX_PROD_REV_STR_SIZE);

  qhsusb_fd_ms_send_request((void*) qhsusb_ms_tx_buffer,MAX_INQUERY_RES_SIZE, QHSUSB_EP_DIR_TX,SEND_TO_HOST );
}

//============================================================================
/**
 * @function  handle_read_format_capacity
 *
 * @brief handle read format capacity SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_read_format_capacity(void)
{
  uint8 lun_num = current_xfer.lun;
  uint32 num_blocks, block_size;
  
  memset (qhsusb_ms_tx_buffer, 0 ,MAX_READ_FORMAT_CAPACITY_SIZE);
  qhsusb_log(HANDLE_READ_FORMAT_CAPACITY_LOG,0,ms_devices.luns[lun_num].num_blocks);
  qhsusb_ms_tx_buffer[3] = 8;          /* capacity list length */
  num_blocks = ms_devices.luns[lun_num].num_blocks;
  block_size = ms_devices.luns[lun_num].block_size;
  num_blocks = SWAP32(num_blocks);
  block_size = SWAP32(block_size);
  memscpy(&qhsusb_ms_tx_buffer[4],(MS_TX_BUF_SIZE - (4*sizeof(uint8))),&num_blocks,sizeof(uint32)); 
  memscpy(&qhsusb_ms_tx_buffer[8],(MS_TX_BUF_SIZE - (8*sizeof(uint8))),&block_size,sizeof(uint32));  
  qhsusb_ms_tx_buffer[8] = 0x02;  /* Formatted Media - Current media capacity */ 
  csw.dCSWDataResidue = cbw.dCBWDataTransferLength - MAX_READ_FORMAT_CAPACITY_SIZE;
  qhsusb_fd_ms_send_request((void*) qhsusb_ms_tx_buffer,MAX_READ_FORMAT_CAPACITY_SIZE, QHSUSB_EP_DIR_TX, SEND_TO_HOST);
}

//============================================================================
/**
 * @function  handle_read_capacity
 *
 * @brief handle read capacity SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_read_capacity(void)
{
  uint8 lun_num = current_xfer.lun;
  uint32 num_blocks, block_size;

  qhsusb_log(HANDLE_READ_CAPACITY_LOG,0,ms_devices.luns[lun_num].num_blocks);
  num_blocks = (ms_devices.luns[lun_num].num_blocks)-1; /* for Last Logical Block Address */
  block_size = ms_devices.luns[lun_num].block_size;
  num_blocks = SWAP32(num_blocks);
  block_size = SWAP32(block_size);

  memset (qhsusb_ms_tx_buffer, 0 ,MAX_READ_CAPACITY_SIZE);
  memscpy(qhsusb_ms_tx_buffer, MS_TX_BUF_SIZE, &num_blocks,sizeof(uint32));
  memscpy(&qhsusb_ms_tx_buffer[4], (MS_TX_BUF_SIZE - (4*sizeof(uint8))), &block_size, sizeof(uint32) );  
  qhsusb_fd_ms_send_request((void*) qhsusb_ms_tx_buffer,MAX_READ_CAPACITY_SIZE, QHSUSB_EP_DIR_TX,SEND_TO_HOST);
}

//============================================================================
/**
 * @function  handle_mode_sense
 *
 * @brief handle mode sense SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_mode_sense(void)
{
  uint8 pc; /* page control */
  uint8 page_code;
  uint8 data[2]={0xff,0xff};
  uint32 len;

  pc = (cbw.CBWCB[2]) >> 6; /* page control */;
  page_code = (cbw.CBWCB[2]) & 0x3f;

  memset (qhsusb_ms_tx_buffer, 0 ,MAX_MODE_SENSE_SIZE);
  qhsusb_log(HANDLE_MODE_SENSE_LOG,0,0);

  if (page_code == 0x08 || page_code == 0x3F) 
  {
  
    qhsusb_ms_tx_buffer[4] = 0x08;     /* Page code */
    qhsusb_ms_tx_buffer[5] = 0x0A;    /* Page length */
   
    if (pc == 0) /* not changeable_values */ 
    { /* mode params */
      qhsusb_ms_tx_buffer[6] = 0x04;   
      memscpy(&qhsusb_ms_tx_buffer[8],(MS_TX_BUF_SIZE - (8*sizeof(uint8))), &data, 2);  
      memscpy(&qhsusb_ms_tx_buffer[12], (MS_TX_BUF_SIZE - (12*sizeof(uint8))), &data, 2);  
      memscpy(&qhsusb_ms_tx_buffer[14], (MS_TX_BUF_SIZE - (14*sizeof(uint8))), &data, 2); 
    }

    len = (cbw.CBWCB[4] == 0x0c ? 0x0c : 16);
    qhsusb_ms_tx_buffer [0]= len-1;
    qhsusb_ms_tx_buffer [1]=qhsusb_ms_tx_buffer [0]-1 ;
    if (cbw.CBWCB[4]-len>0)
    {
      csw.dCSWDataResidue = cbw.CBWCB[4]-len; 
    }
    qhsusb_fd_ms_send_request((void*) qhsusb_ms_tx_buffer, len, QHSUSB_EP_DIR_TX, SEND_TO_HOST);
  }
  else
  {
    /* stall EP */
    qhsusb_dci_stall_endpoint(sw_device.core_id, MS_EP_IDX, QHSUSB_EP_DIR_TX);
    /* send scw with error code */
    csw.dCSWDataResidue = cbw.CBWCB[4];
    csw.bCSWStatus = CSW_ERROR_STATUS;
    sending_csw();
  }
}
//============================================================================
/**
 * @function  handle_mode_select
 *
 * @brief handle mode select SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_mode_select(void)
{
  uint8 parameter_list_length = cbw.CBWCB[4];
  qhsusb_log(HANDLE_MODE_SELECT_LOG,0,0);
  /* the host send us OUT transfer in the length of  parameter_list_length*/
  csw.bCSWStatus = CSW_ERROR_STATUS;
#ifdef QHSUSB_MS_OPTIMIZATION
  qhsusb_fd_ms_send_request((void*) qhsusb_ms_rx_buffer[qhsusb_curr_index], parameter_list_length, QHSUSB_EP_DIR_RX, RCV_FROM_HOST);
  qhsusb_curr_index=qhsusb_curr_index^0x1;
#else
  qhsusb_fd_ms_send_request((void*) qhsusb_ms_rx_buffer, parameter_list_length, QHSUSB_EP_DIR_RX, RCV_FROM_HOST);
#endif 
}

//============================================================================
/**
 * @function  handle_request_sense
 *
 * @brief handle request sense SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_request_sense(void)
{ 
  memset (qhsusb_ms_tx_buffer, 0 ,MAX_REQUEST_SENSE_SIZE);
  qhsusb_log(HANDLE_REQUEST_SENSE_LOG,0,MAX_REQUEST_SENSE_SIZE);
  qhsusb_ms_tx_buffer[0] = 0xF0;  /* set  VALID bit and response code*/
  qhsusb_ms_tx_buffer[2] = 0x05;  /*illegal request*/
  qhsusb_ms_tx_buffer[7] = 0x0A;  /* additional sense length field*/     
  qhsusb_ms_tx_buffer[12] = 0x0; /* ADDITIONAL SENSE CODE */

  qhsusb_fd_ms_send_request((void*) qhsusb_ms_tx_buffer, MAX_REQUEST_SENSE_SIZE, QHSUSB_EP_DIR_TX, SEND_TO_HOST );
}

//============================================================================
/**
 * @function  handle_test_unit_ready
 *
 * @brief handle test unit ready SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_test_unit_ready(void)
{
  qhsusb_log(HANDLE_TEST_UNIT_READY_LOG,0,0);
  sending_csw();
}

//============================================================================
/**
 * @function  sending_csw
 *
 * @brief sending CSW to the host
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
void sending_csw(void)
{
  qhsusb_log(SENDING_CSW_LOG,0,0);
  cbw.bCBWCBLength = 0; /* we are ready for a new CBW*/
  memscpy((void*)qhsusb_ms_tx_buffer, MS_TX_BUF_SIZE, (void*)&csw,sizeof(qhsusb_csw_t));
  qhsusb_fd_ms_send_request((void*)qhsusb_ms_tx_buffer,QHSUSB_CSW_LEN, QHSUSB_EP_DIR_TX, SEND_CSW);
  csw.dCSWDataResidue = 0;
  csw.bCSWStatus = CSW_GOOD_STATUS;
}

//============================================================================
/**
 * @function  update_current_xfer_params
 *
 * @brief update current xfer params from CBW
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void update_current_xfer_params(void)
{
  uint32 temp;
  uint32 xfer_len;
  qhsusb_log(UPDATE_CURRENT_XFER_PARAMS_LOG,0,cbw.bCBWLUN);
  qhsusb_log(QHSUSB_TRANSFER_LENGTH_LOG,0,cbw.dCBWDataTransferLength);
  current_xfer.lun = cbw.bCBWLUN;
  xfer_len = cbw.dCBWDataTransferLength;
  memscpy (&temp, sizeof(temp), (void*)&cbw.CBWCB[2],sizeof(temp));
  current_xfer.lba = SWAP32(temp);
  current_xfer.blocks_num = xfer_len/(ms_devices.luns[current_xfer.lun].block_size);
}

//============================================================================
/**
 * @function  handle_read
 *
 * @brief handle read SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_read(void)
{
  qhsusb_log(HANDLE_READ_LOG,0,0);
  /*First check if any pending writes to emmc are there */
#ifdef QHSUSB_MS_OPTIMIZATION
  if(qhsusb_emmc_status ==QHSUSB_EMMC_WRITE_PENDING)
  {
    /*This means something has to be written to emmc still*/
    qhsusb_write_last_data_to_emmc();
    qhsusb_emmc_status=QHSUSB_EMMC_WRITE_DONE;
    /*By now the last data would have finished, so next write*/
    /*request should be the first one*/
    qhsusb_is_first_handle_write = TRUE; 
  }	
#endif
  /*Do the read operation*/
  update_current_xfer_params();
  if(qhsusb_fd_ms_hotplug_dev_read(current_xfer.lun, current_xfer.lba, current_xfer.blocks_num ))
  {
    if((current_xfer.blocks_num*512) > MS_TX_BUF_SIZE)
    {
      /** During SCSI Emulation Linux Host can request for more blocks**/
      /** Since we support a maximum of 64K only, we just send 64k maximum **/ 
      qhsusb_fd_ms_send_request((void*)qhsusb_ms_tx_buffer, MS_TX_BUF_SIZE, QHSUSB_EP_DIR_TX, SEND_TO_HOST); 
    }
    else
    {
      qhsusb_fd_ms_send_request((void*)qhsusb_ms_tx_buffer, current_xfer.blocks_num*512, QHSUSB_EP_DIR_TX, SEND_TO_HOST);
    }
    
  }
  else
  {
    qhsusb_log(QHSUSB_SCSI_FAIL_LOG, 0, __LINE__);
    /** Since you are not able to read from EMMC, this has to be a h/w bug **/
    DCI_ASSERT(QHSUSB_ERR_HW_FAILURE); 
  }
}

//============================================================================
/**
 * @function  handle_write
 *
 * @brief handle write SCSI command
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void handle_write(void)
{
#ifdef QHSUSB_MS_OPTIMIZATION
  qhsusb_log(HANDLE_WRITE_LOG,0,0);
  
  update_current_xfer_params();
  if(current_xfer.blocks_num*512 > MS_RX_BUF_SIZE )
  {
      qhsusb_log(QHSUSB_SCSI_FAIL_LOG, 0, __LINE__);
      DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); 
  }
  qhsusb_fd_ms_send_request((void*)qhsusb_ms_rx_buffer[qhsusb_curr_index], current_xfer.blocks_num*512, QHSUSB_EP_DIR_RX, HOST_READ);    
  /*For the first write request there is no need to do an eMMC write*/
  if(!qhsusb_is_first_handle_write)
  {
    qhsusb_fd_ms_hotplug_dev_write((void*)qhsusb_ms_rx_buffer[qhsusb_curr_index^1], qhsusb_prev_xfer.lun, qhsusb_prev_xfer.lba, qhsusb_prev_xfer.blocks_num);
    qhsusb_emmc_status = QHSUSB_EMMC_WRITE_DONE;
  }
  else
  {
    qhsusb_is_first_handle_write = FALSE;
  }	
  /*increment the index*/
  qhsusb_curr_index = qhsusb_curr_index^0x1; 
#else
  qhsusb_log(HANDLE_WRITE_LOG,0,0);
  update_current_xfer_params();
  if(current_xfer.blocks_num*512 > MS_RX_BUF_SIZE )
  {
      qhsusb_log(QHSUSB_SCSI_FAIL_LOG, 0, __LINE__);
      DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); 
  }
  qhsusb_fd_ms_send_request((void*)qhsusb_ms_rx_buffer, current_xfer.blocks_num*512, QHSUSB_EP_DIR_RX, HOST_READ);
#endif
}

//============================================================================
/**
 * @function  write_to_emmc_card
 *
 * @brief write data received from host to eMMC card.
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
void write_to_emmc_card(void)
{
  qhsusb_log(WRITE_TO_EMMC_CARD_LOG,0,0);
#ifdef QHSUSB_MS_OPTIMIZATION
  /*Basically storing Current transfer in the previous*/
  memscpy(&qhsusb_prev_xfer, sizeof(transfer_type), &current_xfer, sizeof(transfer_type));
  qhsusb_emmc_status =QHSUSB_EMMC_WRITE_PENDING; 
#else  
  qhsusb_fd_ms_hotplug_dev_write(current_xfer.lun, current_xfer.lba, current_xfer.blocks_num);
#endif
  sending_csw();
}
