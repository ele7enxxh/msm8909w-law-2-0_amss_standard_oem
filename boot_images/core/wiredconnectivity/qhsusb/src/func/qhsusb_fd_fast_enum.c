/*=======================================================================*//**
  @file         qhsusb_fd_fast_enum.c

  @brief        Function Driver for fast enumeration

  @details      
                Simple data pipe device presented to the host.
                Device composition is similar to HDLC one.
                Application polls stack until enumeration completed.
                Then, application shut down the stack.
 
                Copyright (c) 2008 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_fd_fast_enum.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2008-09-28  vkondrat  initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_fd_fast_enum.h"

#include "qhsusb_dcd.h"

#include "qhsusb_dci_api.h"

#ifndef QHSUSB_FAST_ENUM_PID
#define QHSUSB_FAST_ENUM_PID (0x901C)
#endif

#ifndef QHSUSB_FAST_ENUM_VID
#define QHSUSB_FAST_ENUM_VID (USB_VENDOR_ID_QUALCOMM)
#endif



/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*****hardware descriptors*****/
static usb_desc_device_t fast_enum_usb_device = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   0,                              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
   64,                             //  uint8  bMaxPacketSize0;
   QHSUSB_FAST_ENUM_VID,           //  uint16 idVendor;
   QHSUSB_FAST_ENUM_PID,           //  uint16 idProduct;
   0x0,                            //  uint16 bcdDevice;
   1,                              //  uint8  iManufacturer;
   2,                              //  uint8  iProduct;
   0,                              //  uint8  iSerialNumber;
   1                               //  uint8  bNumConfigurations;
};

static PACKED struct fast_enum_config_struct {
   usb_desc_configuration_t conf1;
   usb_desc_interface_t ifc0;
   usb_desc_endpoint_t ep_out;
   usb_desc_endpoint_t ep_in;
} PACKED_POST fast_enum_usb_config = {
   {
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct fast_enum_config_struct), //      uint16 wTotalLength;
      1,                                 //      uint8  bNumInterfaces;
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1                                  //      uint8  bMaxPower;
   },
   {
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      0xff,                              //      uint8  bInterfaceClass;
      0xff,                              //      uint8  bInterfaceSubClass;
      0xff,                              //      uint8  bInterfaceProtocol;
      0                                  //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      32,                                //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      32,                                //      uint8  bInterval;
   },

};

static uint8 str0[4] = {
   sizeof(str0),USB_DESCTYPE_STRING,
   0x09,0x04,                            // langid : US_EN
};

static uint8 str_manuf[62] = {                       // iManufacturer
   sizeof(str_manuf),USB_DESCTYPE_STRING,
   'Q',0,
   'u',0,
   'a',0,
   'l',0,
   'c',0,
   'o',0,
   'm',0,
   'm',0,
   ' ',0,
   'C',0,
   'D',0,
   'M',0,
   'A',0,
   ' ',0,
   'T',0,
   'e',0,
   'c',0,
   'h',0,
   'n',0,
   'o',0,
   'l',0,
   'o',0,
   'g',0,
   'i',0,
   'e',0,
   's',0,
   ' ',0,
   'M',0,
   'S',0,
   'M',0,
};

static uint8 qhsusb_str_product[36] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'Q',0,
   'H',0,
   'S',0,
   'U',0,
   'S',0,
   'B',0,
   ' ',0,
   'F',0,
   'a',0,
   's',0,
   't',0,
   ' ',0,
   'E',0,
   'n',0,
   'u',0,
   'm',0,
   '.',0,
};

/*****software descriptors*****/
static struct qhsusb_dcd_dsc_device the_device;
static struct qhsusb_dcd_dsc_configuration the_config;
static struct qhsusb_dcd_dsc_interface the_ifc;

static struct qhsusb_dcd_dsc_string the_strings[] = {
   {
      &the_strings[1],               //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str0, //   struct usb_desc_header* descriptor;
      0,                             //   uint16 langid;
      0,                             //   uint8 index;
   },
   {
      &the_strings[2],               //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str_manuf, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      1,                             //   uint8 index;
   },
   {
      NULL,               //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)qhsusb_str_product, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      2,                             //   uint8 index;
   },
};

static struct qhsusb_dcd_dsc_endpoint the_eps[2] = {
   {
      &the_eps[1],                  //   struct qhsusb_dcd_dsc_endpoint* next;
      &fast_enum_usb_config.ep_out, //   struct usb_desc_endpoint* descriptor;
      &the_ifc,                     //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                         //   int (*setup)(...);
   },
   {
      NULL,                         //   struct qhsusb_dcd_dsc_endpoint* next;
      &fast_enum_usb_config.ep_in,  //   struct usb_desc_endpoint* descriptor;
      &the_ifc,                     //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                         //   int (*setup)(...);
   },
};

static struct qhsusb_dcd_dsc_interface the_ifc = {
   NULL,                        //  struct qhsusb_dcd_dsc_interface* next;
   &fast_enum_usb_config.ifc0,  //  struct usb_desc_interface* descriptor;
   &the_config,                 //  struct qhsusb_dcd_dsc_configuration* configuration;
   the_eps,                     //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   NULL,                        //  int (*setup)(...);
};

static struct qhsusb_dcd_dsc_configuration the_config = {
   NULL,                        //  struct qhsusb_dcd_dsc_configuration* next;
   &fast_enum_usb_config.conf1, //  struct usb_desc_configuration* descriptor;
   &the_device,                 //  struct qhsusb_dcd_dsc_device* device;
   &the_ifc                     //  struct qhsusb_dcd_dsc_interface* interfaces;
};

static struct qhsusb_dcd_dsc_device the_device = {
   &fast_enum_usb_device,       //  struct usb_desc_device* descriptor;
   &the_config,                 //  struct qhsusb_dcd_dsc_configuration* configurations;
   the_strings,                 //  struct qhsusb_dcd_dsc_string* strings;
   0,                           //  uint32 core_id;
   NULL,                        //  void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
   NULL,                        //  int (*setup)(...);
};

/************* Buffers for Tx/Rx *************/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

struct qhsusb_dcd_dsc_device* qhsusb_fd_fast_enum_init(void)
{
   qhsusb_dcd_init(&the_device);
   qhsusb_dci_set_runstop_bit(the_device.core_id);
   return &the_device;
}

void qhsusb_fd_fast_enum_shutdown(void)
{
   qhsusb_dcd_shutdown(&the_device);
}

