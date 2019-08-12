/*=======================================================================*//**
  @file         qhsusb_fd_ms.c

  @brief        MS FD implementation.

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
#include "qhsusb_fd_hdlc.h"
#include "qhsusb_fd_ms.h"
#include "qhsusb_scsi.h"
#include "qhsusb_dcd.h"
#include "qhsusb_dci_api.h"
#include "qhsusb_cdc.h"
#include "qhsusb_fd_common.h"
#include "qhsusb_ch9.h" /*for the descriptors definitions*/
#include "fs_hotplug.h"
#include "qhsusb_dcd.h"
#include <stringl/stringl.h>
#include "qhsusb_log.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define CUSTOMIZE NULL

#if defined (FEATURE_QHSUSB_HDLC_CDCACM) && defined (FEATURE_QHSUSB_MS) 
#error "MS and ACM couldn't be defined together"
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
extern PACKED struct hdlc_config_struct_obex PACKED_POST hdlc_usb_config_obex;
extern struct qhsusb_dcd_dsc_device sw_device;
extern struct qhsusb_dcd_dsc_configuration sw_config;
#ifdef QHSUSB_MS_OPTIMIZATION
int8 qhsusb_curr_index = 0x0;
#endif
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
xfer_status_enum cur_xfer_status;
 static uint8* ms_rx_buff_ptr; 
 static uint8* ms_tx_buff_ptr;

/* Info regarding all luns */
 device_info_type ms_devices;
/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static void init_lun (lun_info_type* lun_ptr, uint8 lun_index, boolean is_removable, const char *hdev_name);
static void update_new_lun_params(lun_info_type* lun_ptr);
static int ms_setup_packet_handler(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);
static void update_ms_devices_params(void);
static void hotplug_info_init (void);
static void poll_lun(lun_info_type* lun_ptr );
static void ms_init_rx(const struct qhsusb_dcd_dsc_device* dev);
static void qhsusb_iter_init_hotplug_lun
(
 hotplug_dev_type dev_type, enum hotplug_iter_type iter_type,
 enum hotplug_iter_device_location dev_location, boolean  removable
 );

/* MS interface */
struct qhsusb_dcd_dsc_interface ms_sw_data_ifc;
static struct qhsusb_dcd_dsc_endpoint ms_sw_data_eps[2] =
{
  {  /* IN */
    &ms_sw_data_eps[1],      //   struct qhsusb_dcd_dsc_endpoint* next;
    CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
    &ms_sw_data_ifc,         //   struct qhsusb_dcd_dsc_interface* interface;
    NULL,                    //   int (*setup)(...);
  },
  {  /* OUT */
    NULL,                    //   struct qhsusb_dcd_dsc_endpoint* next;
    CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
    &ms_sw_data_ifc,         //   struct qhsusb_dcd_dsc_interface* interface;
    NULL,                    //   int (*setup)(...);
  }
};
 struct qhsusb_dcd_dsc_interface ms_sw_data_ifc = {
  NULL,                      //  struct qhsusb_dcd_dsc_interface* next;
  CUSTOMIZE,                 //  struct usb_desc_interface* descriptor;/**/
  &sw_config,                //  struct qhsusb_dcd_dsc_configuration* configuration;
  ms_sw_data_eps,            //  struct qhsusb_dcd_dsc_endpoint* endpoints;
  ms_setup_packet_handler,   //  int (*setup)(...);
};

/************* Buffers for MS Tx/Rx *************/
static struct qhsusb_urb ms_tx_urb;
static struct qhsusb_urb ms_rx_urb;

//============================================================================
/**
 * @function  ms_setup_packet_handler
 *
 * @brief handle MS setup packets (EP0 packets)
 *
 * @note
 *
 * @param interface, request 
 *
 * @return transfer status
 *
 */
//============================================================================
static int ms_setup_packet_handler(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req)
{
  struct qhsusb_dcd_dsc_device* dev;
  int ep;
  uint32 core_id = sw_device.core_id;

  if (ifc != NULL && req != NULL)
  {

    qhsusb_log(MS_SETUP_PACKET_HANDLER_LOG,0,req->bRequest);
    dev = ifc->configuration->device;
    switch (req->bRequest) 
    {
      case USB_SETUP_REQ_MS_RESET:
        if (req->wLength == 0 && req->wValue == 0) 
        { 
          /* clear MS dQH and dTDs */
          ep = MS_EP_IDX;
          /* cancel active MS transfers */
          qhsusb_dci_cancel_transfer(core_id,ep,QHSUSB_EP_DIR_RX);
          qhsusb_dci_cancel_transfer(core_id,ep,QHSUSB_EP_DIR_TX);
          ms_init_rx(dev);    
        }
        /* respond to the command */ 
        return qhsusb_dcd_control_transfer (ifc->configuration->device, 1, dev->setup_buffer ,0);

      case USB_SETUP_REQ_MS_GET_MAX_LUN:
        if ( req->wLength == 1 && req->wValue == 0) 
        { 
          *(uint8*)dev->setup_buffer = ms_devices.num_luns - 1; /* zero based */
          return qhsusb_dcd_control_transfer (ifc->configuration->device, 1, dev->setup_buffer ,1);
        }
        break;
      default:  /* not supported */
        break;
    }
  }
  return -1;
}

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
void qhsusb_ms_init(const struct qhsusb_dcd_dsc_device* dev)
{
  qhsusb_log(QHSUSB_MS_INIT_LOG,0,0);
  update_ms_devices_params();
  init_scsi_params(&ms_rx_buff_ptr, &ms_tx_buff_ptr);
  ms_init_rx(dev);
}

//============================================================================
/**
 * @function  update_ms_devices_params
 *
 * @brief Calling poll_lun in order to get lun's info from hotplug.
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */  
//============================================================================
static void update_ms_devices_params(void)
{
  uint8 i;
  lun_info_type* lun_ptr;
  qhsusb_log(UPDATE_MS_DEVICES_PARAMS_LOG,0,0);
 for(i=0; i<ms_devices.num_luns; i++)
 {
  lun_ptr = &ms_devices.luns[i]; 

  if (lun_ptr == NULL)
  {
    /*ERROR*/
    return;
  }

  poll_lun(lun_ptr);
 }
}

//============================================================================
/**
 * @function  ms_init_rx
 *
 * @brief initialize RX MS EPs 
 *
 * @note
 *
 * @param dev
 *
 * @return None.
 *
 */
//============================================================================
static void ms_init_rx(const struct qhsusb_dcd_dsc_device* dev)
{
  qhsusb_log(MS_INIT_RX_LOG,0,ms_rx_urb.transfer_status);
  
  if (QHSUSB_URB_STATUS_ACTIVE != ms_rx_urb.transfer_status)
  {
    struct qhsusb_urb* urb = &ms_rx_urb;
    if ( 0 == urb->actual_length ) 
		{
			urb->transfer_length = QHSUSB_CBW_LEN;
      cur_xfer_status = PREPARE_FOR_CBW;
#ifdef QHSUSB_MS_OPTIMIZATION
	  if((qhsusb_curr_index == 1) || (qhsusb_curr_index == 0))
	  {
		urb->transfer_buffer_ptr = (ms_rx_buff_ptr+(qhsusb_curr_index*MS_RX_BUF_SIZE));
	  }
#else
      urb->transfer_buffer_ptr = ms_rx_buff_ptr;
#endif
      while ( 0 != qhsusb_submit_urb(urb) ) 
			{
        qhsusb_dcd_poll(&sw_device);
        if ( 0 == dev->config_value ) 
			  {
          return;
        }
      }
      return;
    }
  }
}

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
 void set_ms_packet_size(uint16 wMaxPacketSize)
{
  usb_desc_endpoint_t* temp_desc = ms_sw_data_eps[0].descriptor;

  temp_desc->wMaxPacketSize = wMaxPacketSize;
  temp_desc = ms_sw_data_eps[1].descriptor;
  temp_desc->wMaxPacketSize = wMaxPacketSize;
  qhsusb_log(SET_MS_PACKET_SIZE_LOG,0,wMaxPacketSize);
}

//============================================================================
/**
 * @function  ms_tx_cb
 *
 * @brief MS TX callback function
 *
 * @note
 *
 * @param urb
 *
 * @return None.
 *
 */
//============================================================================
static void ms_tx_cb(struct qhsusb_urb* urb)
{
  urb->actual_length = 0;
  qhsusb_log(MS_TX_CB_LOG,0,cur_xfer_status);
  switch (cur_xfer_status)
  {
    case SEND_TO_HOST:
      sending_csw(); 
      break;

    case SEND_CSW:
      ms_init_rx(urb->usb_device);
      break;
  }
}

//============================================================================
/**
 * @function  ms_rx_cb
 *
 * @brief MS RX callback function
 *
 * @note
 *
 * @param urb
 *
 * @return None.
 *
 */
//============================================================================
static void ms_rx_cb(struct qhsusb_urb* urb)
{
  qhsusb_log(MS_RX_CB_LOG,0,urb->transfer_status);
  
  if (urb->transfer_status != QHSUSB_URB_STATUS_COMPLETE_ERROR)
  {
    qhsusb_log(MS_RX_CB_CURR_STATUS_LOG,0,cur_xfer_status);
    switch (cur_xfer_status)
    {
      case PREPARE_FOR_CBW:
        handle_new_cbw(urb);
        break;

      case RCV_FROM_HOST:
        urb->actual_length = 0;
        sending_csw();
        break;

    case HOST_READ:
       urb->actual_length = 0;
       write_to_emmc_card();
       break;
	   
    default :
       /* recovers the MS state machine when extra RX interrupt is seen */
       urb->actual_length = 0;
       break;
    }
  }
}

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
void qhsusb_fd_ms_init(void)
{
  struct qhsusb_urb* urb;

  ms_sw_data_eps[0].descriptor = &(hdlc_usb_config_obex.ep_in_ms);
  ms_sw_data_eps[1].descriptor = &(hdlc_usb_config_obex.ep_out_ms);
  ms_sw_data_ifc.descriptor = &(hdlc_usb_config_obex.ifc_ms);
   
  qhsusb_log(QHSUSB_FD_MS_INIT_LOG,0,0);
  /* Tx */
  urb                      = &ms_tx_urb;
  urb->usb_device          = &sw_device;
  urb->endpoint_address    = ms_sw_data_eps[0].descriptor->bEndpointAddress;
  urb->transfer_length     = 0;
  urb->send_zlp            = FALSE;
  urb->complete_callback   = ms_tx_cb;

  /* Rx */
  urb                      = &ms_rx_urb;
  urb->usb_device          = &sw_device;
  urb->endpoint_address    = ms_sw_data_eps[1].descriptor->bEndpointAddress;
  urb->transfer_length     = MS_RX_BUF_SIZE;
  urb->send_zlp            = FALSE;
  urb->complete_callback   = ms_rx_cb;
  urb->actual_length       = 0;

  hotplug_info_init();
}

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
boolean qhsusb_fd_ms_is_tx_active(void)
{
  if (QHSUSB_URB_STATUS_ACTIVE == ms_tx_urb.transfer_status)
  {
    qhsusb_log(QHSUSB_FD_MS_IS_TX_ACTIVE_LOG,0,1);
    return 1;
  }
  qhsusb_log(QHSUSB_FD_MS_IS_TX_ACTIVE_LOG,0,0);
  return 0;
}

//============================================================================
/**
 * @function  hotplug_info_init
 *
 * @brief Init LUN params from hotplug.
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================
static void hotplug_info_init (void)
{
  qhsusb_log(HOTPLUG_INFO_INIT_LOG,0,0);
  ms_devices.num_luns = 0;
  /*Define eMMC as Non-Removable. Ensures proper enumeration in WP*/
  qhsusb_iter_init_hotplug_lun(
    HOTPLUG_TYPE_MMC, HOTPLUG_ITER_DEVICES, HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY, FALSE);
}

//============================================================================
/**
 * @function  qhsusb_iter_init_hotplug_lun
 *
 * @brief Retrieve LUN info from Hotplug. 
 *
 * @note
 *
 * @param dev_type, iter_type, dev_location, removable
 *
 * @return None.
 *
 */
//============================================================================
static void qhsusb_iter_init_hotplug_lun
(
 hotplug_dev_type dev_type, enum hotplug_iter_type iter_type,
 enum hotplug_iter_device_location dev_location, boolean  removable
 )
{
  struct hotplug_iter *hiter;
  struct hotplug_iter_record *hiter_rec;
  static uint8 initialized_mmc_luns = 0;

  qhsusb_log(QHSUSB_ITER_INIT_HOTPLUG_LUN_LOG,0,0);
  hiter = hotplug_iter_open (dev_type, iter_type, dev_location);
  if (!hiter)
  {
    /* ERROR */
    return;
  }

  hiter_rec = hotplug_iter_get_next (hiter);

  while (hiter_rec != NULL && ms_devices.num_luns < MAX_NUM_LUNS)
  {
    init_lun(&ms_devices.luns[ms_devices.num_luns],
      ms_devices.num_luns , removable, hiter_rec->hdev_name);

    ms_devices.num_luns++;

    if (dev_type == HOTPLUG_TYPE_MMC)
    {
      initialized_mmc_luns++;
    }
    hiter_rec = hotplug_iter_get_next (hiter);
  }
  hotplug_iter_close (hiter);
}

//============================================================================
/**
 * @function  init_lun
 *
 * @brief update lun_ptr info
 *
 * @note
 *
 * @param None.
 *
 * @return None.
 *
 */
//============================================================================

static void init_lun
(
 lun_info_type* lun_ptr, uint8 lun_index,
 boolean is_removable,const char *hdev_name
 )
{
  qhsusb_log(INIT_LUN_LOG,0,0);
  if (lun_ptr != NULL)
  {
    lun_ptr->index = lun_index;
    lun_ptr->enabled = TRUE;

    memscpy(lun_ptr->hdev_name,sizeof(lun_ptr->hdev_name), hdev_name, sizeof(lun_ptr->hdev_name));
    lun_ptr->status = LUN_STATUS_NOT_READY;  

    lun_ptr->removable = is_removable;

    lun_ptr->block_size = 0;
    lun_ptr->num_blocks = 0;
    lun_ptr->read_only = FALSE;
  }
}

//============================================================================
/**
 * @function  poll_lun
 *
 * @brief open device and check if it is present.
 *
 * @note
 *
 * @param lun_ptr.
 *
 * @return None.
 *
 */
//============================================================================
static void poll_lun(lun_info_type* lun_ptr)
{
  boolean ready = FALSE;
  int res;
  qhsusb_log(POLL_LUN_LOG,0,0);
  if (lun_ptr->context == NULL)
  {
      lun_ptr->context = hotplug_open_device (lun_ptr->hdev_name);
  }

  res = hotplug_dev_is_present( (struct hotplug_device *) lun_ptr->context);
  ready = (res == 1) ? TRUE:FALSE;
  
  if (!ready)
  {
    lun_ptr->status = LUN_STATUS_NOT_READY;
  }
  else
  {
    if (lun_ptr->status == LUN_STATUS_NOT_READY)
    {
      update_new_lun_params(lun_ptr);
    }
  }
} 

//============================================================================
/**
 * @function  update_new_lun_params
 *
 * @brief Update block-size, number of blocks and LUN status.
 *
 * @note
 *
 * @param lun_ptr.
 *
 * @return None.
 *
 */
//============================================================================
static void update_new_lun_params(lun_info_type* lun_ptr)
{
  uint16 block_size_tmp = 0;
  qhsusb_log(UPDATE_NEW_LUN_PARAMS_LOG,0,0);
  /* hotplug_dev_get_size returns 0 on success and -1 on error*/
  if (hotplug_dev_get_size ((struct hotplug_device *)lun_ptr->context,
    &lun_ptr->num_blocks, &block_size_tmp) == 0)
  {
    lun_ptr->block_size = block_size_tmp;
    lun_ptr->status = LUN_STATUS_READY;
  }
  else
  {
    /*error*/
    lun_ptr->block_size = 0;
    lun_ptr->num_blocks = 0;
  }
}
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
uint32 qhsusb_fd_ms_send_request(void* buff, uint32 len, uint8 dir, xfer_status_enum status)
{
  int rc = -1;
  struct qhsusb_urb* urb;
      
  cur_xfer_status  = status;
  
  qhsusb_log(QHSUSB_FD_MS_SEND_REQUEST_LOG,0,0);
  if (dir)/*TX*/
  {
    urb = &ms_tx_urb;
  }
  else
  {
    urb = &ms_rx_urb;
  }

  /* ensure previous URB is complete before creating another urb */
  while ( QHSUSB_URB_STATUS_ACTIVE == urb->transfer_status ) 
  {
      qhsusb_dcd_poll(&sw_device);
  }
  
  if ( urb->transfer_status != QHSUSB_URB_STATUS_ACTIVE )
  {
    urb->transfer_buffer_ptr = buff;
    urb->transfer_length = len;
    rc = qhsusb_submit_urb(urb);
  }
  return rc < 0 ? rc : (int)len;
}

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
boolean qhsusb_fd_ms_hotplug_dev_read (uint8 lun_num, uint32  start_address, uint16  num_blocks)
{
  void* context ;

  qhsusb_log(QHSUSB_FD_MS_HOTPLUG_DEV_READ_LOG,0,lun_num);
  if (lun_num < MAX_NUM_LUNS)
  {
    context= ms_devices.luns[lun_num].context; 
    /*hotplug_dev_read returns 0 on success and -1 on error*/
    if (hotplug_dev_read((struct hotplug_device*)context, start_address,
      (byte*)ms_tx_buff_ptr, num_blocks) == 0)
    {
      return TRUE;
    }
  }
  return FALSE;
}
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
boolean qhsusb_fd_ms_hotplug_dev_write(void * buffer, uint8 lun_num, uint32  start_address, uint16  num_blocks)
#else
boolean qhsusb_fd_ms_hotplug_dev_write(uint8 lun_num, uint32  start_address, uint16  num_blocks)
#endif
{
  void* context;
 
  qhsusb_log(QHSUSB_FD_MS_HOTPLUG_DEV_WRITE_LOG,0,lun_num);
  if (lun_num<MAX_NUM_LUNS)
  {
    context= ms_devices.luns[lun_num].context; /* The device handle needed for hotplug api functions */
    /*hotplug_dev_read returns 0 on success and -1 on error*/
#ifdef QHSUSB_MS_OPTIMIZATION
    if (hotplug_dev_write((struct hotplug_device*)context, start_address,
      (byte*)buffer,num_blocks) == 0)
#else
    if (hotplug_dev_write((struct hotplug_device*)context, start_address,
      (byte*)ms_rx_buff_ptr,num_blocks) == 0)
#endif
    {
      return TRUE;
    }
    return FALSE;
   }
  return FALSE;
}
