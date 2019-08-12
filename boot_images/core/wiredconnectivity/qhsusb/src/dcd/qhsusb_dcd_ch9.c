/*=======================================================================*//**
  @file         qhsusb_dcd_ch9.c

  @brief        Chapter 9 functionality

  @details      
 
                Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dcd/qhsusb_dcd_ch9.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2009-04-17  dhaval    Ported from VU_CORE_WIREDCONNECTIVITY_QHSUSB.00.00.08 
                        Disable ZLT for Rx endpoints. This way, long transfer 
                        will be split into several dTD's by hardware.	
  2009-03-17  dhaval    Removed compiler warnings.
  2007-08-14  vkondrat  check device state prior to transfer
  2007-08-10  vkondrat  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_dcd.h"
#include "qhsusb_dci_api.h"
#include <stringl/stringl.h>
#include "qhsusb_log.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#ifdef HAVE_DCD_EVENT_LOG
#pragma arm section rwdata = "USB_DEBUG_DATA", zidata = "USB_DEBUG_DATA"
struct dcd_log_s {
   enum qhsusb_log_event evt;
   uint8 addr;
   uint8 config;
   uint32 time;
   uint32 data0;
   uint32 data1;
};
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
struct dcd_log_s dcd_log[256] = {{evt_zero, 0, 0, 0, 0, 0}};
int dcd_log_index = 0;
#pragma arm section rwdata, zidata
#endif /*#ifdef HAVE_DCD_EVENT_LOG*/

static uint8 qhsusb_setup_buffer[10]={0};
static uint8 descriptor_buf[64];

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static boolean qhsusb_is_configured =FALSE;
#endif

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
 static uint32 QHSUSB_SHUTDOWN_LOOP_ITERATION = 500;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void dcd_callback_reset(struct qhsusb_dcd_dsc_device* dev);
static void dcd_callback_setup(struct qhsusb_dcd_dsc_device* dev, void* buffer_ptr);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static void dcd_callback_disconnect(struct qhsusb_dcd_dsc_device* dev);
#endif

static qhsusb_dci_callback_table_t dci_callbacks = {
   dcd_callback_reset,
   dcd_callback_setup,
   qhsusb_dcd_port_status_changed,
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
   dcd_callback_disconnect,
#endif
};

#ifdef HAVE_DCD_EVENT_LOG
void qhsusb_dcd_log_event(struct qhsusb_dcd_dsc_device* dev,
                          enum qhsusb_log_event evt,
                          uint32 data0,
                          uint32 data1)
{
   if (dcd_log_index < sizeof(dcd_log)/sizeof(dcd_log[0])) {
      struct dcd_log_s* t=&dcd_log[dcd_log_index];
      t->evt = evt;
      t->addr = dev->address;
      t->config = dev->config_value;
      t->time = qhsusb_dci_get_time_usec(dev->core_id,QHSUSB_TIMER_0_FREE_RUN);
      t->data0 = data0;
      t->data1 = data1;
      dcd_log_index++;
   }
}
#endif
/**
 * Find config by index
 * 
 * @param dev
 * @param index
 * 
 * @return 
 */
static struct qhsusb_dcd_dsc_configuration* find_config(
               const struct qhsusb_dcd_dsc_device* dev, uint16 index)
{
#ifdef QHSUSB_HAVE_MULTI_CONFIG
   struct qhsusb_dcd_dsc_configuration* conf;
   for ( conf = dev->configurations; 
         ( conf != NULL ) && ( index > 0 ); conf = conf->next, index-- ) {
      ;
   }
   return conf;
#else
   return dev->configurations;
#endif /* #ifdef QHSUSB_HAVE_MULTI_CONFIG */
}

/**
 * Find config by configuration value
 * 
 * @param dev
 * @param value
 * 
 * @return 
 */
static struct qhsusb_dcd_dsc_configuration* find_config_by_value(
               const struct qhsusb_dcd_dsc_device* dev, uint8 value)
{
#ifdef QHSUSB_HAVE_MULTI_CONFIG
   struct qhsusb_dcd_dsc_configuration* conf;
   for ( conf = dev->configurations; conf != NULL; conf = conf->next ) {
      if ( conf->descriptor->bConfigurationValue == value ) {
         return conf;
      }
   }
   return conf;
#else
   return dev->configurations;
#endif /* #ifdef QHSUSB_HAVE_MULTI_CONFIG */
}

static struct qhsusb_dcd_dsc_interface* find_ifc(
               const struct qhsusb_dcd_dsc_device* dev, uint16 index)
{
#ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
   struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
   if ( conf ) {
      struct qhsusb_dcd_dsc_interface* ifc;
      for ( ifc = conf->interfaces; ifc != NULL; ifc = ifc->next ) {
         if ( ( ifc->descriptor->bInterfaceNumber == index ) && 
              ( ifc->descriptor->bAlternateSetting == ifc->current_altsetting ) ) {
            return ifc;
         }
      }
   }
   return NULL;
#else
   return dev->active_configuration->interfaces;
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
}

/**
 * Find EP specified 
 *  
 * search strategy: 
 * - go to current configuration 
 * - iterate over interfaces 
 *   - look fo current altsetting
 *   - iterate over endpoints
 * 
 * @param dev
 * @param ep_address
 * 
 * @return EP software descriptor,
 *         NULL if not found
 */
static struct qhsusb_dcd_dsc_endpoint* find_ep(
               const struct qhsusb_dcd_dsc_device* dev, uint16 ep_address)
{
#ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
   struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
   if ( conf ) {
      struct qhsusb_dcd_dsc_interface* ifc;
      for ( ifc=conf->interfaces; ifc != NULL; ifc = ifc->next ) {
         if ( ifc->descriptor->bAlternateSetting == ifc->current_altsetting ) {
            struct qhsusb_dcd_dsc_endpoint* ep;
            for ( ep=ifc->endpoints; ep != NULL; ep = ep->next ) {
               if ( ep->descriptor && (ep->descriptor->bEndpointAddress == ep_address) ) {
                  return ep;
               }
            }
         }
      }
   }
#else
   struct qhsusb_dcd_dsc_endpoint* ep;

   if(dev->active_configuration && dev->active_configuration->interfaces)
   {
       for ( ep=dev->active_configuration->interfaces->endpoints;
             ep != NULL; ep = ep->next ) {
           if ( ep->descriptor && (ep->descriptor->bEndpointAddress == ep_address) ) {
             return ep;
           }
       }
   }        
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
   return NULL;
}

static struct qhsusb_dcd_dsc_string* find_string(
               const struct qhsusb_dcd_dsc_device* dev, uint16 lang, uint8 index)
{
   struct qhsusb_dcd_dsc_string* s;
   for ( s = dev->strings; s != NULL; s = s->next ) {
      if ( ( s->langid == lang ) && ( s->index == index ) ) {
         return s;
      }
   }
   return NULL;
}

static void dcd_notify(struct qhsusb_dcd_dsc_device* dev, uint32 event)
{
   qhsusb_dcd_log_event(dev, evt_notify, event, 0);
   if ( NULL != dev->notify ) {
      dev->notify(dev,event);
   }
}

#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
/**
 * Set maximum power to be drawn drom USB host.
 * 
 * Called every time when power allowed to be drawn changes.
 * 
 * Call charger driver with updated value
 * 
 * @param dev
 * @param max_current
 *               maximum current to draw from Vbus, ma
 */
static void dcd_set_max_current( struct qhsusb_dcd_dsc_device* dev, int max_current )
{
   qhsusb_dcd_log_event(dev, evt_set_power, (uint32)(max_current), 0);
   dev->max_current = max_current;
   dcd_notify(dev,QHSUSB_DCD_EVT_CURRENT);
}
#else
static inline void dcd_set_max_current( struct qhsusb_dcd_dsc_device* dev, int max_current )
{}
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */

/**
 * Set alternate setting for given interface
 * 
 * 2 path:
 * - 1-st to make sure correct ifc/alt exist;
 * - 2-nd to set correct alt for all ifc's with given index
 * 
 * @param conf
 * @param ifc_num
 * @param alt
 * 
 * @return error code:
 *         - 0 if OK
 *         - negative if error
 */
static int dcd_set_interface(
            struct qhsusb_dcd_dsc_configuration* conf, uint8 ifc_num, uint8 alt)
{
#ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
   struct qhsusb_dcd_dsc_interface* ifc;
   struct qhsusb_dcd_dsc_interface* ifc1;
   for ( ifc=conf->interfaces; ifc != NULL; ifc = ifc->next ) {
      if ( ( ifc->descriptor->bInterfaceNumber == ifc_num ) && 
           ( ifc->descriptor->bAlternateSetting == alt ) ) {
         /* found, 2-nd pass to write current alt */
         for ( ifc1 = conf->interfaces; ifc1 != NULL; ifc1 = ifc1->next ) {
            if ( ifc1->descriptor->bInterfaceNumber == ifc_num ) {
               ifc1->current_altsetting = alt;
            }
         }
         /* configure DCI */
         {
            struct qhsusb_dcd_dsc_endpoint* ep;
            for ( ep = ifc->endpoints; ep != NULL; ep = ep->next ) {
               usb_desc_endpoint_t* dsc = ep->descriptor;
               uint8 is_ep_in = (uint8)(!!(dsc->bEndpointAddress & 0x80));
               qhsusb_dci_config_endpoint(conf->device->core_id,
                                          (uint8)(dsc->bEndpointAddress & 0xf),
                                          is_ep_in,
                                          (uint8)(dsc->bmAttributes & 3),
                                          dsc->wMaxPacketSize,
                                          0); /* disable auto-zlp for TX */
            }
         }
         return 0;
      }
   }
   return -1;
#else
   struct qhsusb_dcd_dsc_interface* ifc = conf->interfaces;
   if ( ( ifc->descriptor->bInterfaceNumber == ifc_num ) && 
        ( ifc->descriptor->bAlternateSetting == alt ) ) {
      struct qhsusb_dcd_dsc_endpoint* ep;
      for ( ep = ifc->endpoints; ep != NULL; ep = ep->next ) {
         usb_desc_endpoint_t* dsc = ep->descriptor;
         uint8 is_ep_in = (uint8)(!!(dsc->bEndpointAddress & 0x80));
         qhsusb_dci_config_endpoint(conf->device->core_id,
                                    (uint8)(dsc->bEndpointAddress & 0xf),
                                    is_ep_in,
                                    (uint8)(dsc->bmAttributes & 3),
                                    dsc->wMaxPacketSize,
                                    0); /* disable auto-zlp for TX */
      }
      return 0;
   }
   return -1;
#endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
}

int qhsusb_dcd_set_config(struct qhsusb_dcd_dsc_device* dev, uint8 new_config)
{
   uint32 speed;
   uint32 max_packet_size = 0;
   qhsusb_dcd_log_event(dev, evt_set_config, new_config, dev->config_value);
   /**
    * It is illegal to change configuration from one non-0 to
    * another
    */
   if ( ( 0 != dev->config_value ) && 
        ( 0 != new_config ) && 
        ( new_config != dev->config_value )  ) {
      return -1;
   }
   if ( new_config != dev->config_value ) {
      if ( new_config != 0 ) {
         struct qhsusb_dcd_dsc_configuration* conf = 
            find_config_by_value(dev, new_config);
         if ( NULL == conf ) { /* requested configuration do not exist */
            return -1;
         }
         {
            /* all ifc's assigned altsetting 0 */
   #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS
            struct qhsusb_dcd_dsc_interface* ifc;
            for ( ifc = conf->interfaces; ifc != NULL; ifc = ifc->next ) {
               if ( dcd_set_interface(conf, ifc->descriptor->bInterfaceNumber, 0) < 0 ) {
                  return -1;
               }
            }
   #else
            if ( dcd_set_interface(conf, conf->interfaces->descriptor->bInterfaceNumber, 0) < 0 ) {
               return -1;
            }
   #endif /* #ifdef QHSUSB_HAVE_MULTI_FUNCTIONS */
         }
         /* apply changes */
         dev->config_value = new_config;
         dev->active_configuration = conf;
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
         dev->is_suspended = 0;
         /**
          * Accordingly to USB standard (#9.6.3), bMaxPower is in units 
          * of 2ma. To get value in ma, need to multiply it by 2 
          */
         dcd_set_max_current(dev, dev->active_configuration->descriptor->bMaxPower * 2);
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */
      } else {
         dev->config_value = 0;
         dev->active_configuration = NULL;
         dcd_set_max_current(dev, 0);
         /** 
          *  unconfigure all EP's
          *  
          *  Accordingly to USB standard, device may contain up to 16
          *  EP's in each direction. EP0 is control EP and is handled
          *  separately (by DCI) , thus not mentioned here
          */
         {
            uint8 ep;
            for(ep=1;ep<DCI_MAX_ENDPOINTS;ep++) {
               qhsusb_dci_cancel_transfer(dev->core_id,ep,QHSUSB_EP_DIR_RX);
               qhsusb_dci_unconfig_endpoint(dev->core_id,ep,QHSUSB_EP_DIR_RX);
               qhsusb_dci_cancel_transfer(dev->core_id,ep,QHSUSB_EP_DIR_TX);
               qhsusb_dci_unconfig_endpoint(dev->core_id,ep,QHSUSB_EP_DIR_TX);
            }
         }
      }
      dcd_notify(dev,QHSUSB_DCD_EVT_CONFIG);

      speed = qhsusb_dci_get_speed(dev->core_id);
      switch (speed)
      {
        case QHSUSB_DEVICE_SPEED_HIGH:
          max_packet_size = 512;
          break;
        case QHSUSB_DEVICE_SPEED_FULL:
          max_packet_size = 64;
          break;
        case QHSUSB_DEVICE_SPEED_LOW:
          max_packet_size = 8;
          break;
        default:
          break;
      }
      qhsusb_dci_set_max_packet_size(max_packet_size);
   }
   else
   {
      /* should also update controller speeds when configuration changes to 0 also */
      speed = qhsusb_dci_get_speed(dev->core_id);
      switch (speed)
      {
        case QHSUSB_DEVICE_SPEED_HIGH:
          max_packet_size = 512;
          break;
        case QHSUSB_DEVICE_SPEED_FULL:
          max_packet_size = 64;
          break;
        case QHSUSB_DEVICE_SPEED_LOW:
          max_packet_size = 8;
          break;
        default:
          break;
      }
      qhsusb_dci_set_max_packet_size(max_packet_size);
   }
   return 0;
}

int qhsusb_dcd_control_transfer(struct qhsusb_dcd_dsc_device* dev, int is_tx, void* buff, uint32 len)
{
   int rc = -1;
   struct qhsusb_urb* urb = is_tx ? &dev->setup_tx_urb : &dev->setup_rx_urb;
   if ( urb->transfer_status != QHSUSB_URB_STATUS_ACTIVE ) {
      urb->transfer_buffer_ptr = buff;
      urb->transfer_length = len;
      rc=qhsusb_submit_urb(urb);
   }
   return rc < 0 ? rc : (int)len;
}

/**
 * Initiate Tx transfer using dev->setup_buffer
 * 
 * @param dev
 * @param len    Transfer length
 * 
 * @return 
 */
static int dcd_control_reply(struct qhsusb_dcd_dsc_device* dev, uint32 len)
{
   return qhsusb_dcd_control_transfer(dev,1,dev->setup_buffer,len);
}


static int handle_get_descriptor(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
   PACKED struct dsc_struct {
      uint8 desc_index;
      uint8 desc_type;
   } PACKED_POST;
   struct dsc_struct* dsc = (struct dsc_struct*)(void*)(&req->wValue);
   void* buff = NULL;
   int len = -1;
   switch ( dsc->desc_type ) {
   case USB_DESCTYPE_DEVICE:
      buff = (void*)dev->descriptor;
      len = dev->descriptor->bLength;
      break;
   case USB_DESCTYPE_CONFIGURATION:
      {
         struct qhsusb_dcd_dsc_configuration* cfg = 
            find_config(dev, dsc->desc_index);
         if ( cfg != NULL ) {
            buff = (void*)cfg->descriptor;
            len = cfg->descriptor->wTotalLength;
         }
      }
      break;
   case USB_DESCTYPE_STRING:
      {
         struct qhsusb_dcd_dsc_string* s = 
            find_string(dev, req->wIndex, dsc->desc_index);
         if ( s != NULL ) {
            buff = (void*)s->descriptor;
            len = s->descriptor->bLength;
         }
      }
      break;
   default:
      ;
   }
   if ( buff != NULL ) {
      if ( len > req->wLength ) {
         len = req->wLength;
      }
      /* send it */
      memscpy((void*)descriptor_buf,sizeof(descriptor_buf),(void*)buff,len);
      return qhsusb_dcd_control_transfer(dev, 1, descriptor_buf, (uint32)len);
   }
   return len;
}

static int handle_setup_dev(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	int error =0x0;
#endif
   switch ( req->bRequest ) {
   case USB_SETUP_REQ_GET_STATUS:
      if ( req->wLength >= 2 ) {  /* USB standard expects 2 bytes here */
         /**
          * @todo report real data
          */
         *(uint16*)dev->setup_buffer = 0;
         /* bit 0 - is_self_powered */
         /* bit 1 - is_remote_wakeup */
         return dcd_control_reply(dev, 2);
      }
      break;
   case USB_SETUP_REQ_CLEAR_FEATURE:
   	  if (req->wValue == USB_FSEL_DEV_TEST_MODE)
   	  {
        dev->test_mode = USB_PORT_TEST_DISABLE;
        return 0;
   	  }
      break;
   case USB_SETUP_REQ_SET_FEATURE:
   	  if (req->wValue == USB_FSEL_DEV_TEST_MODE)
   	  {
   	    dev->qhsusb_test_mode_enabled = TRUE;
	      dev->test_mode = (qhsusb_usb_port_test_mode)((req->wIndex & 0x0f00) >> 8);
        if(dev->test_mode > USB_PORT_TEST_FORCE_ENABLE)
        {
          //If it is not in valid range move it to disable mode
          dev->test_mode = USB_PORT_TEST_DISABLE; 
        }
        return 0;
   	  }
      break;
   case USB_SETUP_REQ_SET_ADDRESS:
      qhsusb_dcd_log_event(dev, evt_set_addr, req->wValue, req->wIndex);
      if ( 0 == dev->config_value ) {
         uint8 addr = (uint8)(req->wValue & 0xff);
         dev->address = addr;
         qhsusb_dci_set_address(dev->core_id, dev->address);
         return 0;
      }
      break;
   case USB_SETUP_REQ_GET_DESCRIPTOR:
      return handle_get_descriptor(dev, req);
      //break;  // unreachable
   case USB_SETUP_REQ_SET_DESCRIPTOR:
      /* not supported (yet?) */
      break;
   case USB_SETUP_REQ_GET_CONFIGURATION:
      qhsusb_dcd_log_event(dev, evt_get_config, 0, 0);
      if ( req->wLength >= 1 ) { /* USB standard expects 1 byte here */
         *(uint8*)dev->setup_buffer = dev->config_value;
         return dcd_control_reply(dev, 1);
      }
      break;
   case USB_SETUP_REQ_SET_CONFIGURATION:
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	  error = qhsusb_dcd_set_config(dev, (uint8)(req->wValue & 0xff));
	  qhsusb_is_configured = TRUE;
	  return error;
#else
      return qhsusb_dcd_set_config(dev, (uint8)(req->wValue & 0xff));
#endif
   default:
      if ( NULL != dev->setup ) {
         return dev->setup(dev, req);
      }
      break;
   }
   return -1;
}

static int handle_setup_ifc(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
   struct qhsusb_dcd_dsc_interface* ifc = find_ifc(dev, req->wIndex);
   switch ( req->bRequest ) {
   case USB_SETUP_REQ_GET_STATUS:
      if ( ifc && req->wLength >= 2 ) { /* USB standard expects 2 bytes here */
         *(uint16*)dev->setup_buffer = 0;
         return dcd_control_reply(dev, 2);
      }
      break;
   case USB_SETUP_REQ_CLEAR_FEATURE:
      break;
   case USB_SETUP_REQ_SET_FEATURE:
      break;
   case USB_SETUP_REQ_GET_INTERFACE:
      if ( ( ifc != NULL ) && ( req->wLength >= 1 ) ) { /* USB standard expects 1 byte here */
         *(uint8*)dev->setup_buffer = ifc->current_altsetting;
         return dcd_control_reply(dev, 1);
      }
      break;
   case USB_SETUP_REQ_SET_INTERFACE:
      /* ifc as found above, is irrelevant */
      {
         struct qhsusb_dcd_dsc_configuration* conf = dev->active_configuration;
         if ( conf != NULL ) {
            return dcd_set_interface(conf, (uint8)(req->wIndex & 0xff), 
                                     (uint8)(req->wValue & 0xff));
         }
      }
      break;
   default:
      if ( ( NULL != ifc ) && ( NULL != ifc->setup ) ) {
         return ifc->setup(ifc, req);
      }
      break;
   }
   return -1;
}

static int handle_ep_halt(struct qhsusb_dcd_dsc_device* dev, 
                          struct qhsusb_dcd_dsc_endpoint* ep, int halt)
{
   uint8 ep_num = ep->descriptor->bEndpointAddress & 0x0f;
   boolean is_tx = !!(ep->descriptor->bEndpointAddress & 0x80);
   ep->is_halt = halt;
   if ( halt != 0 ) {
      qhsusb_dci_stall_endpoint(dev->core_id, ep_num, is_tx);
   } else {
      qhsusb_dci_unstall_endpoint(dev->core_id, ep_num, is_tx);
   }
   return 0;
}

static int handle_setup_ep(struct qhsusb_dcd_dsc_device* dev, struct usb_setup_packet* req)
{
   struct qhsusb_dcd_dsc_endpoint* ep = find_ep(dev, req->wIndex);
   if ( NULL == ep ) {
      return -1;
   }
   switch ( req->bRequest ) {
   case USB_SETUP_REQ_GET_STATUS:
      if ( req->wLength >= 2 ) {
         *(uint16*)dev->setup_buffer = ep->is_halt;
         return dcd_control_reply(dev, 2);
      }
      break;
   case USB_SETUP_REQ_CLEAR_FEATURE:
      switch ( req->wValue ) { /* feature selector */
      case USB_FSEL_EP_HALT:
         return handle_ep_halt(dev, ep, 0);
      default:
         ;
      }
      break;
   case USB_SETUP_REQ_SET_FEATURE:
      switch ( req->wValue ) { /* feature selector */
      case USB_FSEL_EP_HALT:
         return handle_ep_halt(dev, ep, 1);
      default:
         ;
      }
      break;
   case USB_SETUP_REQ_SYNCH_FRAME:
      break;
   default:
      if ( NULL != ep->setup ) {
         return ep->setup(ep, req);
      }
      break;
   }
   return -1;
}

/********callbacks for DCI*************/
/**
 * Handle reset notification from DCI
 * 
 * Reset moves USB device to 'default' state
 * 
 * It means not configured and address == 0
 * 
 * @param dev
 */
static void dcd_callback_reset(struct qhsusb_dcd_dsc_device* dev)
{
   /**
    * Prior to reset per se, perform port state analysis
    */
   qhsusb_dcd_port_status_changed(dev);
   qhsusb_dcd_log_event(dev,evt_reset,0,0);
   qhsusb_dcd_set_config(dev, 0);
   dev->address = 0;
   qhsusb_dci_set_address(dev->core_id, dev->address);
   /** 
    *  reset EP0 software state
    *  
    *  Hardware has reset EP0 transfers, reflect this in software.
    */
   dev->setup_rx_urb.transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
   dev->setup_tx_urb.transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
   qhsusb_is_configured = FALSE;
#endif
}


#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static void dcd_callback_disconnect(struct qhsusb_dcd_dsc_device* dev)
{
	qhsusb_is_configured = FALSE;
	dcd_notify(dev,QHSUSB_DCD_EVT_DISCONNECT);
}
#endif


static void dcd_callback_setup(struct qhsusb_dcd_dsc_device* dev, void* buffer_ptr)
{
   int data_out_len = -1; /**< Data phase length, or negative error code */
   struct usb_setup_packet* req;
   if((NULL == dev) || (NULL == buffer_ptr))
   {
      qhsusb_log(QHSUSB_NULL_PTR_LOG, 0, __LINE__);
      DCI_ASSERT(QHSUSB_ERR_BAD_PARAM);
      return;
   }
   req = (struct usb_setup_packet*)buffer_ptr;
#ifdef HAVE_DCD_EVENT_LOG
  {
    uint32* t = (uint32*)buffer_ptr;
    qhsusb_dcd_log_event(dev,evt_raw_setup,t[0],t[1]);
  }
#endif
   /* data phase */
   switch ( (req->bmRequestType >> USB_SETUP_RT_RCP_POS) & USB_SETUP_RT_RCP_SEED ) {
   case USB_SETUP_RT_RCP_DEV:
      data_out_len = handle_setup_dev(dev, req);
      break;
   case USB_SETUP_RT_RCP_IFC:
      data_out_len = handle_setup_ifc(dev, req);
      break;
   case USB_SETUP_RT_RCP_EP:
      data_out_len = handle_setup_ep(dev, req);
      break;
   default:
      ;
   }
   /* handshake phase */
   if ( data_out_len > 0 ) { /* have data phase, data was sent, use Rx for handshake */
      qhsusb_dcd_control_transfer(dev, 0, dev->setup_buffer, sizeof(qhsusb_setup_buffer));
   } else if ( data_out_len == 0 ) { /* no data phase, use Tx for handshake */
      dcd_control_reply(dev, 0);
   } else if ( data_out_len < 0 ) { /* error, stall EP0 */
      qhsusb_dci_stall_endpoint(dev->core_id, 0, 0);
   }
}

void qhsusb_dcd_port_status_changed(struct qhsusb_dcd_dsc_device* dev)
{
         qhsusb_dcd_log_event(dev,evt_portsc,0,0);
   /*********attachment********/
   {
      int is_attached = qhsusb_dci_is_attached(dev->core_id);
      if ( dev->is_attached != is_attached ) { /* it is attach/detach event */
         qhsusb_dcd_log_event(dev, evt_attach, (uint32)(is_attached), 0);
         dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
         /* @todo clean all internal states */
         if ( is_attached != 0 ) {             /* it is attach event */
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
            int wc;
            dev->is_suspended = 0;
            dcd_set_max_current(dev, 100);
            wc = qhsusb_dci_is_wall_charger(dev->core_id);
            if ( wc != 0 ) {
               dcd_set_max_current(dev, 1500);
            } else {
               dcd_set_max_current(dev, 0);
               qhsusb_dci_connect(dev->core_id);
            }
#else  /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */
            qhsusb_dci_connect(dev->core_id);
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */
         } else {                              /* it is detach event */
            dcd_set_max_current(dev, 0);
         }
         dev->is_attached = is_attached;
         dcd_notify(dev,QHSUSB_DCD_EVT_ATTACH);
      }
   }
   /*********speed********/
   {
      int speed;
      switch ( qhsusb_dci_get_speed(dev->core_id) ) {
      case QHSUSB_DEVICE_SPEED_FULL:
         speed = QHSUSB_DCD_SPEED_FULL;
         break;
      case QHSUSB_DEVICE_SPEED_LOW:
         speed = QHSUSB_DCD_SPEED_LOW;
         break;
      case QHSUSB_DEVICE_SPEED_HIGH:
         speed = QHSUSB_DCD_SPEED_HIGH;
         break;
      default:
         speed = QHSUSB_DCD_SPEED_UNKNOWN;
      }
      if ( dev->speed != speed ) { /* connection established */
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
         dev->is_suspended = 0;
#endif
         qhsusb_dcd_log_event(dev, evt_speed, (uint32)(speed), 0);
         dev->speed = speed;
         dcd_notify(dev,QHSUSB_DCD_EVT_SPEED);
      }
   }
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
   /*********suspend********/
   {
      int is_suspended = qhsusb_dci_is_suspended(dev->core_id);
      if ( dev->is_suspended != is_suspended ) {
         if ( is_suspended ) { // suspend
            qhsusb_dcd_log_event(dev,evt_suspend,0,0);
            dcd_set_max_current(dev, 0);
         } else {              // resume
            qhsusb_dcd_log_event(dev,evt_resume,0,0);
            if (dev->active_configuration) {
               /**
                * bMaxPower is in units of 2ma - see USB 2.0 #9.6.3
                */
               dcd_set_max_current(dev, dev->active_configuration->descriptor->bMaxPower * 2);
            } else {
               dcd_set_max_current(dev, 0);
            }
         }
         dev->is_suspended = is_suspended;
         dcd_notify(dev,QHSUSB_DCD_EVT_SUSPEND);
      }
   }
#endif /* #ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING */
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void qhsusb_dcd_init(struct qhsusb_dcd_dsc_device* dev)
{
   qhsusb_dcd_log_event(dev,evt_init,0,0);
   /* device is in "default" state - not configured, no address assigned */
   dev->is_attached = 0;
   dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
   dev->address = 0;
   dev->active_configuration = NULL;
   dev->config_value = 0;
   dcd_set_max_current(dev, 0);
   /* prepare URB */
   dev->setup_tx_urb.usb_device = dev;
   dev->setup_tx_urb.endpoint_address = 0x80;
   dev->setup_tx_urb.complete_callback = NULL;
   dev->setup_tx_urb.transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
   dev->setup_rx_urb.usb_device = dev;
   dev->setup_rx_urb.endpoint_address = 0x0;
   dev->setup_rx_urb.complete_callback = NULL;
   dev->setup_rx_urb.transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
   dev->setup_buffer = qhsusb_setup_buffer;
   dev->qhsusb_test_mode_enabled = FALSE;
   dev->test_mode = USB_PORT_TEST_DISABLE;

   /* init DCI */
   qhsusb_dci_init(dev->core_id, dev, &dci_callbacks);
   qhsusb_dci_start_timer(dev->core_id,QHSUSB_TIMER_0_FREE_RUN,1);
}

void qhsusb_dcd_shutdown(struct qhsusb_dcd_dsc_device* dev)
{
   uint32 i = 0; 
   for (i = 0; (QHSUSB_URB_STATUS_ACTIVE == dev->setup_tx_urb.transfer_status) && (i < QHSUSB_SHUTDOWN_LOOP_ITERATION); i++)
   {
		qhsusb_dcd_poll(dev);
   }
   qhsusb_dcd_set_config(dev, 0);
   qhsusb_dci_shutdown(dev->core_id);
   dev->is_attached = 0;
   dev->speed = QHSUSB_DCD_SPEED_UNKNOWN;
   dev->address = 0;
}

void qhsusb_dcd_poll(struct qhsusb_dcd_dsc_device* device)
{
   if (NULL != device) {
      qhsusb_dci_poll_events(device->core_id);
   }
}

int qhsusb_submit_urb(struct qhsusb_urb* urb)
{
   urb->signature = QHSUSB_URB_SIGNATURE;
   urb->transfer_status = 0;
   urb->actual_length = 0;
   if ( urb->usb_device != NULL ) {
      urb->core_id = urb->usb_device->core_id;
      /**
       * When device is not in configured state, valid transfer 
       * may be only for EP0
       *  
       * urb->endpoint_address have EP number in bits 3:0
       */
      if ( ( urb->usb_device->config_value != 0 ) || 
           ( ( urb->endpoint_address & 0xf ) == 0 ) ) {
         int rc = (int)qhsusb_dci_transfer_request(urb);
#ifdef HAVE_DCD_EVENT_LOG
         if (rc != 0 ) {
            qhsusb_dcd_log_event(urb->usb_device,evt_urb_err,urb->transfer_status,rc);
         }
#endif
         return rc;
      }
   }
   urb->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
   qhsusb_dcd_log_event(urb->usb_device, evt_urb_err, 
                        urb->transfer_status, (uint32)(-1));
   return -1;
}

/**
* Check if the USB device is enumerated.
* 
* 
* @param dev The device handle
*
* @return TRUE if the device is enumerated. FALSE otherwise.
*/
boolean qhsusb_dcd_is_enumerated(struct qhsusb_dcd_dsc_device* dev)
{
  qhsusb_dcd_poll(dev);

  return (dev->config_value != 0);
}

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
/**
* Check if the USB device is successfully configured
*
*
* @param None
*
* @return TRUE if configured else FALSE
*/
boolean qhsusb_is_device_configured(void)
{
	return qhsusb_is_configured;
}
#endif


