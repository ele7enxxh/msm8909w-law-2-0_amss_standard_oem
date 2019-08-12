/*=======================================================================*//**
  @file         qhsusb_fd_hdlc.c

  @brief        Function Driver for HDLC stream function

  @details      
                This FD implements HDLC streaming protocol.
                It can use one of the following USB interfaces, depending on
                value of global variable qhsusb_hdlc_use_cdcacm:

                - 0 : use vendor specific class with 2 bulk EP's
                - 1 : use CDC/ACM class with 2 bulk and 1 interrupt EP's

                HDLC implementation: For H->D direction, double buffers used
                to maximize throughput. Upon packet receive, the user supplied
                callback executed. This callback should handle the data arrived.
                For D->H direction, byte oriented interface provided. Data
                accumulated in internal buffer till it is full or 
                HDLC terminator (0x7e) submitted. In this case, packet get 
                transmitted to the USB bus.

                CDC/ACM implementation: FD supports ACM class specific commands:

                - SET_LINE_CODING
                - GET_LINE_CODING
                - SET_CONTROL_LINE_STATE

                Immediately after configuration (SET_CONFIG), FD sends 
                SERIAL_STATE notification indicating DCD and DSR

                CDC/ACM uses 2 interfaces, thus it requires 
                QHSUSB_HAVE_MULTI_FUNCTIONS to be defined.

                To use CDC/ACM, set qhsusb_hdlc_use_cdcacm to 1 prior to
                QHSUSB stack initialization

                When the QHSUSB_SINGLE_BOOT_ENUM_MAGIC is passed via the 
                HS-USB scratch register HDLC will not issue a USB enumeration.
                Instead, it will rely on the existing USB enumeration. This is
                done for having a single USB enumeration during SW download.
 
                Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/func/qhsusb_fd_hdlc.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2011-07-22  dpatnaik  For EMMCBLD/EHOSTDL dont update serial number string
  2011-06-10  dpatnaik  Added support for reading PID, Serial string from IMEM
  2011-05-18  kimd      Added support for unframed RAM-dump (tx packet feature)
  2009-04-17  dhaval    Fixed vid/pid read for cdc/acm device.
  2009-04-16  dhaval    Implemented API for retrieving vid/pid
  2009-04-09  dhaval    Corrected iSerialNumber
  2009-04-06  dhaval    Added API qhsusb_fd_hdlc_rx_completed to fix defect
                        when usb transfer > 4K.
  2009-03-17  dhaval    Added API for updating vid/pid if vid/pid fuses blown 
  2009-03-17  dhaval    Removed compiler warnings.
  2008-12-17  vkondrat  CDC/ACM support
  2008-08-14  vkondrat  redesign - use simple 1 buffer mode, Rx callback
  2008-08-10  vkondrat  initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_fd_hdlc.h"

#include "qhsusb_dcd.h"
#include "qhsusb_dci_api.h"
#include "qhsusb_cdc.h"
#include "qhsusb_fd_common.h"
#include "qhsusb_boot_dload.h"
#include <string.h> // For memcpy
#ifdef FEATURE_QHSUSB_MS
#include "qhsusb_fd_ms.h"
#endif /* FEATURE_QHSUSB_MS */
#include "qhsusb_log.h"
#include <stringl/stringl.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TX_BUF_SIZE (512) /**< size of  Tx buffer */

#if defined(FEATURE_EHOSTDL) || defined(FEATURE_HOSTDL)
  #define RX_BUF_SIZE_FIXED (4096) /* for DMSS and Streaming Download Protocols Only */
  #define NUM_OF_INT_RX_BUF (2)
#else
  #define RX_BUF_SIZE_FIXED (512)  /* Other boot usecases */  
  #define NUM_OF_INT_RX_BUF (1)
#endif

#define USB_PRODUCT_CDCACM (0xf00a)
#define USB_PRODUCT_DIAG_ONLY (0x900E)
#define USB_PRODUCT_DIAG_MS (0x9006)

#define CUSTOMIZE NULL

#define QHSUSB_DLOAD_SIZE  35



// STRING DESCRIPTOR - USB 2.0 specification Table 9-16
// 1 byte:  bLength         - size of this desc in bytes
// 1 byte:  bDescriptorType - STRING Descriptor Type
// N bytes: bString         - UNICODE encoded string
// UNICODE string scriptor is not NULL-terminated.  The string length is
// substracting two from the value of the first type of the descriptor

// bLength's max value is 255.  2 bytes are used for bLength and 
// bDescriptorType fields
// This leaves 255-2 = 253 for UNICODE string size in bytes.
// Since UNICODE byte size has to be even number, max UNICODE ARRAY byte size 
// is 252 bytes.

#define QHSUSB_STRING_DESC_UNICODE_SIZE_MAX         (252)

// Size of bLength and bDescriptor field in STRING DESCRIPTOR in bytes
#define QHSUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE  (2)

// Maximum size for the STRING DESCRIPTOR, must be even number
#define QHSUSB_STRING_DESC_SIZE_MAX    \
  (QHSUSB_STRING_DESC_UNICODE_SIZE_MAX \
  + QHSUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE)

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
#if !defined(QHSUSB_HAVE_MULTI_FUNCTIONS)
   #error "CDC/ACM require multi-function support - define QHSUSB_HAVE_MULTI_FUNCTIONS"
#endif
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
#ifdef FEATURE_QHSUSB_MS
extern struct qhsusb_dcd_dsc_interface ms_sw_data_ifc;
extern struct qhsusb_dcd_dsc_endpoint  ms_sw_data_eps[2]; /* 2 BULK EPs*/
#endif /* FEATURE_QHSUSB_MS */
uint32 qhsusb_dload_info[QHSUSB_DLOAD_SIZE]; /*This is the static dload structure*/
/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static void hdlc_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event);

/* Functions for internal buffer mode */
static void hdlc_init_rx_int_buf_mode(const struct qhsusb_dcd_dsc_device* dev);
static void hdlc_rx_callback_int_buf_mode(struct qhsusb_urb* urb);


#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static int cdc_setup(struct qhsusb_dcd_dsc_interface* ifc, 
                     struct usb_setup_packet* req);
static int hdlc_use_cdcacm = 0;
static int hdlc_shutdown_in_progress = 0;
#endif

/*****hardware descriptors*****/
/*****OBEX ********************/
static usb_desc_device_t hdlc_usb_device_obex = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   0,                              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
   64,                             //  uint8  bMaxPacketSize0;
   USB_VENDOR_ID_QUALCOMM,         //  uint16 idVendor;
#ifndef FEATURE_QHSUSB_MS
   USB_PRODUCT_DIAG_ONLY,          //  uint16 idProduct;
#else /* FEATURE_QHSUSB_MS */
   USB_PRODUCT_DIAG_MS,
#endif /*FEATURE_QHSUSB_MS*/
   0x0,                            //  uint16 bcdDevice;
   1,                              //  uint8  iManufacturer;
   2,                              //  uint8  iProduct;
   0,                              //  uint8  iSerialNumber;
   1                               //  uint8  bNumConfigurations;
};

PACKED struct hdlc_config_struct_obex PACKED_POST hdlc_usb_config_obex = {
   {                                     // conf1
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct hdlc_config_struct_obex), // uint16 wTotalLength;
#ifdef FEATURE_QHSUSB_MS                   //      uint8  bNumInterfaces;
      2,
#else /* !FEATURE_QHSUSB_MS */
      1,                                 
#endif /*FEATURE_QHSUSB_MS */
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1,                                 //      uint8  bMaxPower;
   },
   {                                     // ifc_data
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      0xff,                              //      uint8  bInterfaceClass;
      0xff,                              //      uint8  bInterfaceSubClass;
      0xff,                              //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   }
#ifdef FEATURE_QHSUSB_MS
  ,{            /* if_ms */                           
      sizeof(usb_desc_interface_t),         //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,               //      uint8  bDescriptorType;
#ifdef FEATURE_RUMI_BOOT
      1,
#else
      20, /* should be changed */           //      uint8  bInterfaceNumber;
#endif
      0,                                    //      uint8  bAlternateSetting;
      2,                                    //      uint8  bNumEndpoints;
      0x8,                                  //      uint8  bInterfaceClass;
      0x6,                                  //      uint8  bInterfaceSubClass;
      0x50,                                 //      uint8  bInterfaceProtocol;
      0,                                    //      uint8  iInterface;
   },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    MS_EP_IDX | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    64,                                //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
   },
 {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    MS_EP_IDX | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    64,                             //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  }
#endif /*FEATURE_QHSUSB_MS*/
};
PACKED struct hdlc_config_struct_obex_only PACKED_POST hdlc_usb_config_obex_only = {
 {                                     // conf1
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct hdlc_config_struct_obex_only), // uint16 wTotalLength;
      1,
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1,                                 //      uint8  bMaxPower;
   },
   {                                     // ifc_data
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      0xff,                              //      uint8  bInterfaceClass;
      0xff,                              //      uint8  bInterfaceSubClass;
      0xff,                              //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   }
};

/*****CDC/ACM******************/
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static usb_desc_device_t hdlc_usb_device_cdcacm = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   USB_DEV_CLASS_CDC,              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
   64,                             //  uint8  bMaxPacketSize0;
   USB_VENDOR_ID_QUALCOMM,         //  uint16 idVendor;
   USB_PRODUCT_CDCACM,             //  uint16 idProduct;
   0x0,                            //  uint16 bcdDevice;
   1,                              //  uint8  iManufacturer;
   2,                              //  uint8  iProduct;
//   3,                              //  uint8  iSerialNumber;
   0,                              //  uint8  iSerialNumber;
   1                               //  uint8  bNumConfigurations;
};

static PACKED struct hdlc_config_struct_cdcacm {
   usb_desc_configuration_t conf1;
   usb_desc_interface_t ifc_cdc;       /**< CDC */
   usb_desc_cdc_header_t cdc_header;   /**< CDC */
   usb_desc_cdc_acm_t cdc_acm;         /**< CDC */
   usb_desc_cdc_union_t cdc_union;     /**< CDC */
   usb_desc_cdc_cm_t cdc_cm;           /**< CDC */
   usb_desc_endpoint_t ep_notify;      /**< CDC */
   usb_desc_interface_t ifc_data;
   usb_desc_endpoint_t ep_in;
   usb_desc_endpoint_t ep_out;
} PACKED_POST hdlc_usb_config_cdcacm = {
   {                                     // conf1
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct hdlc_config_struct_cdcacm),//uint16 wTotalLength;
      2,                                 //      uint8  bNumInterfaces;
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1,                                 //      uint8  bMaxPower;
   },
   {                                     // ifc_cdc
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      1,                                 //      uint8  bNumEndpoints;
      USB_IFC_CLASS_CDC,                 //      uint8  bInterfaceClass;
      USB_IFC_SUBCLASS_CDC_ACM,          //      uint8  bInterfaceSubClass;
      0x1,                               //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {                                     // cdc_header
      sizeof(usb_desc_cdc_header_t),     //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_HEADER,      //      uint8  bDescriptorSubType;
      0x0110,                            //      uint16 bcdCDC;
   },
   {                                     // cdc_acm
      sizeof(usb_desc_cdc_acm_t),        //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_ACM,         //      uint8  bDescriptorSubType;
      0x02,                              //      uint8  bmCapabilities;
   },
   {                                     // cdc_union
      sizeof(usb_desc_cdc_union_t)+0,    //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_UNION,       //      uint8  bDescriptorSubType;
      0,                                 //      uint8  bControlInterface;
      {1,},                              //      uint8  bSubordinateInterface[1];
   },
   {                                     // cdc_cm
      sizeof(usb_desc_cdc_cm_t),         //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_CM,          //      uint8  bDescriptorSubType;
      0x03,                              //      uint8  bmCapabilities;
      1,                                 //      uint8  bDataInterface;
   },
   {                                     // ep_notify
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      2 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_INTR,                  //      uint8  bmAttributes;
      16, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      32,                                //      uint8  bInterval;
   },
   {                                     // ifc_data
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      1,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      USB_IFC_CLASS_CDC_DATA,            //      uint8  bInterfaceClass;
      0,                                 //      uint8  bInterfaceSubClass;
      0,                                 //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },

};
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

static const uint8 str0[4] = {
   sizeof(str0),USB_DESCTYPE_STRING,
   0x09,0x04,                            // langid : US_EN
};

static const uint8 str_manuf[62] = {                       // iManufacturer
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

static const uint8 qhsusb_str_product[36] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'P',0,
   'r',0,
   'o',0,
   'd',0,
   'u',0,
   'c',0,
   't',0,
   ' ',0,
   'n',0,
   'a',0,
   'm',0,
   'e',0,
   ' ',0,
   'h',0,
   'e',0,
   'r',0,
   'e',0,
};

static const uint8 str_serial[34] = {                       // iSerialNumber
   sizeof(str_serial),USB_DESCTYPE_STRING,
   '1',0,
   '2',0,
   '3',0,
   '4',0,
   '5',0,
   '6',0,
   '7',0,
   '8',0,
   '9',0,
   '0',0,
   'A',0,
   'B',0,
   'C',0,
   'D',0,
   'E',0,
   'F',0,
};

#ifndef FEATURE_SKIP_SERIAL_STR_UPDATE
static uint8 serial_number_string[QHSUSB_STRING_DESC_SIZE_MAX] = {
  QHSUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE, USB_DESCTYPE_STRING, 0};
#endif /* FEATURE_SKIP_SERIAL_STR_UPDATE */

/*****software descriptors*****/
struct qhsusb_dcd_dsc_device sw_device;
struct qhsusb_dcd_dsc_configuration sw_config;
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_dcd_dsc_interface sw_cdc_control_ifc;
#endif
static struct qhsusb_dcd_dsc_interface sw_data_ifc;

 struct qhsusb_dcd_dsc_string sw_strings[] = { 
   {
      &sw_strings[1],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str0, //   struct usb_desc_header* descriptor;
      0,                             //   uint16 langid;
      0,                             //   uint8 index;
   },
   {
      &sw_strings[2],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str_manuf, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      1,                             //   uint8 index;
   },
   {
      &sw_strings[3],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)qhsusb_str_product, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      2,                             //   uint8 index;
   },
   {
      NULL,                          //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str_serial, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      3,                             //   uint8 index;
   },
};

static struct qhsusb_dcd_dsc_endpoint sw_data_eps[2] = {
   {  // IN
      &sw_data_eps[1],         //   struct qhsusb_dcd_dsc_endpoint* next;
      CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
      &sw_data_ifc,            //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
   {  // OUT
      NULL,                    //   struct qhsusb_dcd_dsc_endpoint* next;
      CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
      &sw_data_ifc,            //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
};

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_dcd_dsc_endpoint sw_cdc_eps[] = {
   {
      NULL,                    //   struct qhsusb_dcd_dsc_endpoint* next;
      &hdlc_usb_config_cdcacm.ep_notify, //   struct usb_desc_endpoint* descriptor;
      &sw_cdc_control_ifc,     //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
};

static struct qhsusb_dcd_dsc_interface sw_cdc_control_ifc = {
   &sw_data_ifc,                //  struct qhsusb_dcd_dsc_interface* next;
   &hdlc_usb_config_cdcacm.ifc_cdc,    //  struct usb_desc_interface* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configuration;
   sw_cdc_eps,                  //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   cdc_setup,                   //  int (*setup)(...);
};
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

static struct qhsusb_dcd_dsc_interface sw_data_ifc = {
#ifdef FEATURE_QHSUSB_MS  //  struct qhsusb_dcd_dsc_interface* next;
  &ms_sw_data_ifc,
#else /* !FEATURE_QHSUSB_MS */
   NULL,                        
#endif /* FEATURE_QHSUSB_MS */
   CUSTOMIZE,                   //  struct usb_desc_interface* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configuration;
   sw_data_eps,                 //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   NULL,                        //  int (*setup)(...);
};

static struct qhsusb_dcd_dsc_configuration sw_config = {
   NULL,                        //  struct qhsusb_dcd_dsc_configuration* next;
   CUSTOMIZE,                   //  struct usb_desc_configuration* descriptor;
   &sw_device,                  //  struct qhsusb_dcd_dsc_device* device;
   CUSTOMIZE,                   //  struct qhsusb_dcd_dsc_interface* interfaces;
};

static struct qhsusb_dcd_dsc_device sw_device = {
   CUSTOMIZE,                   //  struct usb_desc_device* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configurations;
   sw_strings,                  //  struct qhsusb_dcd_dsc_string* strings;
   0,                           //  uint32 core_id;
   hdlc_notify_cb,              //  void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
   NULL,                        //  int (*setup)(...);
};

/************* Buffers for Tx/Rx *************/

static uint8 hdlc_tx_buffer[TX_BUF_SIZE];
static struct qhsusb_urb hdlc_tx_urb;
static struct qhsusb_urb hdlc_rx_urb;
static void (*user_tx_callback)(struct qhsusb_urb* urb);
static void (*user_rx_callback)(struct qhsusb_urb* urb);
static void (*user_enum_complete)(void);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static void (*user_error_handler)(void);
static boolean qhsusb_cancel_transfer = FALSE;
#endif

static qhsusb_transfer_mode qhsusb_transfer_mode_g = FD_CLIENT_SUPPLIED_BUFFER_MODE;
static qhsusb_bulk_mode_type qhsusb_bulk_mode_g = QHSUSB_MODE__SER_MS;

/* for DMSS and DLOAD protocols fixed sized buffer from the USB layer */
static struct qhsusb_urb hdlc_rx_urb_fixed[NUM_OF_INT_RX_BUF];
static uint8 hdlc_rx_buffer_fixed[NUM_OF_INT_RX_BUF][RX_BUF_SIZE_FIXED];


#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_urb cdc_notify_urb;
static const usb_cdc_line_coding_t line_coding = {
   1200,    // uint32 dwDTERate;       /**< Data Terminal Rate, bits per second */
   0,       // uint8  bCharFormat;     /**< Stop bits: 0 - 1 stop bit, 1 - 1.5, 2 - 2 */
   0,       // uint8  bParityType;     /**< 0 - none, 1 - odd, 2 - even, 3 - mark, 4 - space */
   8,       // uint8  bDataBits;       /**< one of {5, 6, 7, 8, 16} */
};

/**
 * Control IN serial lines
 * 
 * [0] - bRxCarrier (DCD)
 * [1] - bTxCarrier (DSR)
 * [2] - bBreak
 * [3] - bRingSignal
 * [4] - bFraming (error)
 * [5] - bParity (error)
 * [6] - bOverRun (error)
 * [7..15] - reserved
 */
static uint16 uart_ctrl_in = 0;
/**
 * Control OUT serial lines
 * 
 * [0] - DTR
 * [1] - RTS
 * [2..15] - reserved
 */
static uint16 uart_ctrl_out = 0;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/** 
 * CDC/ACM command handler
 * 
 * @param ifc
 * @param req
 * 
 * @return int
 */
static int cdc_setup(struct qhsusb_dcd_dsc_interface* ifc, 
                     struct usb_setup_packet* req)
{
   int32 ret_value = -1;

   switch (req->bRequest) {
   case USB_SETUP_REQ_CDC_SET_LINE_CODING:
      {
         if ( req->wLength == sizeof(usb_cdc_line_coding_t) ) {
            int len = qhsusb_dcd_control_transfer(ifc->configuration->device, 0,
                                               (void*)&line_coding, 
                                               sizeof(usb_cdc_line_coding_t));
            if ( len == (int)(sizeof(usb_cdc_line_coding_t)) ) {
               ret_value = 0;
            }
         }
      }
      break;
   case USB_SETUP_REQ_CDC_GET_LINE_CODING:
      {
         uint32 len = sizeof(usb_cdc_line_coding_t);
         if ( req->wLength < len ) {
            len = req->wLength;
         }
         ret_value = qhsusb_dcd_control_transfer(ifc->configuration->device, 1, 
                                     (void*)&line_coding, len);
      }
      break;
   case USB_SETUP_REQ_CDC_SET_CONTROL_LINE_STATE:
      {
         if (uart_ctrl_out==0)
         {
           /* debug code */
           /* this was added to remove warning issued by compiler
              as this variable set but not used anywhere in code */
         }
         uart_ctrl_out = req->wValue;
         ret_value = 0;
      }
      break;
   default:
      ;
   }
   return ret_value;
}

static int cdc_set_ctrl_in(const struct qhsusb_dcd_dsc_device* dev, uint16 ctrl_in)
{
   int rc;
   /* send notification */
   static usb_cdc_notify_serial_state_t frame;
   frame.notify.bmRequestType = 0xA1;
   frame.notify.bNotificationCode = USB_CDC_NOTIFICATION_SERIAL_STATE;
   frame.notify.wValue = 0;
   frame.notify.wIndex = sw_cdc_control_ifc.descriptor->bInterfaceNumber;
   frame.notify.wLength = 2;
   frame.ctrl_in = ctrl_in;
   cdc_notify_urb.transfer_buffer_ptr = (void*)&frame;
   cdc_notify_urb.transfer_length = sizeof(frame);
   rc = qhsusb_submit_urb(&cdc_notify_urb);
   /* save value */
   if (0 == rc) {
      if (uart_ctrl_in==0)
      {
           /* debug code */
           /* this was added to remove warning issued by compiler
              as this variable set but not used anywhere in code */
      }
      uart_ctrl_in = ctrl_in;
   }
   return rc;
}

void qhsusb_hdlc_use_cdcacm(int use_cdc_acm)
{
   hdlc_use_cdcacm = use_cdc_acm;
}

int qhsusb_hdlc_get_use_cdcacm(void)
{
   return hdlc_use_cdcacm;
}

#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */


static void hdlc_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event)
{
   qhsusb_log(HDLC_NOTIFY_CB_LOG,0,0);
   switch (event) {
   case QHSUSB_DCD_EVT_CONFIG:
     if (qhsusb_transfer_mode_g == FD_USB_INTERNAL_BUFFER_MODE)
     {
       hdlc_init_rx_int_buf_mode(dev);
     }
#ifdef FEATURE_QHSUSB_MS
	if(qhsusb_bulk_mode_g == QHSUSB_MODE__SER_MS)
	{
      qhsusb_ms_init(dev);
	}
#endif /* FEATURE_QHSUSB_MS */

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
      if ( 0 != hdlc_use_cdcacm ) {
         cdc_set_ctrl_in(dev,0x3); /**< DCD | DSR */
      }
#endif
      if (user_enum_complete != NULL)
      {
         user_enum_complete();
      }
      break;
   case QHSUSB_DCD_EVT_SPEED:
      {
         uint16 wMaxPacketSize = 0;
         /**
          * Depending on speed, set bulk EP max. packet size 
          *  
          * USB 2.0 #5.8.3 specifies bulk max. packet size 
          *  - for low speed - none, bulk EP not supported 
          *  - for full speed one of {8,16,32,64}
          *  - for high speed - 512
          */
         switch (dev->speed) {
         case QHSUSB_DCD_SPEED_FULL:
            wMaxPacketSize = 64;
            break;
         case QHSUSB_DCD_SPEED_HIGH:
            wMaxPacketSize = 512;
            break;
         default:
            ; /* either low speed or not connected. can't function */
         }
         sw_data_eps[0].descriptor->wMaxPacketSize = wMaxPacketSize;
         sw_data_eps[1].descriptor->wMaxPacketSize = wMaxPacketSize;
#ifdef FEATURE_QHSUSB_MS
		if(qhsusb_bulk_mode_g == QHSUSB_MODE__SER_MS)
		{
         set_ms_packet_size(wMaxPacketSize);
		}
#endif /*FEATURE_QHSUSB_MS */
      }
      break;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	case QHSUSB_DCD_EVT_DISCONNECT: 
	{
		/*Now a disconnect event has occured during the transfer*/
		if(NULL != user_error_handler)
		{
			qhsusb_cancel_transfer = TRUE; 
			user_error_handler();
		}		
	}
	break;
#endif
   default:
      break;
   }
}

/**
 * At the end of Tx transfer, mark packet as done
 * 
 * @param urb
 */
static void hdlc_tx_callback(struct qhsusb_urb* urb)
{
   qhsusb_log(HDLC_TX_CALLBACK_LOG,0,0);
   urb->transfer_length = 0;
   if (user_tx_callback != NULL)
   {
      user_tx_callback(urb);
   }
   else
   {
     qhsusb_log(QHSUSB_NULL_PTR_LOG, 0, 0);
   }
}

/**
 * As soon as Rx completed,
 * Rx is idle and it is possible to submit another URB 
 *  
 * After this, (possibly time consuming process) user callback 
 * called. 
 *  
 * @note may block 
 *  
 * @param urb
 */
static void hdlc_rx_callback(struct qhsusb_urb* urb)
{
   qhsusb_log(HDLC_RX_CALLBACK_ACTUAL_LENGTH_LOG,0,urb->actual_length);
   qhsusb_log(HDLC_RX_CALLBACK_TRANSFER_LENGTH_LOG,0,urb->transfer_length);
   if(user_rx_callback != NULL)
   {
   user_rx_callback(urb);
}
   else
   {
     qhsusb_log(QHSUSB_NULL_PTR_LOG, 0, 0);
   } 
}

#ifndef FEATURE_SKIP_SERIAL_STR_UPDATE
static qhsusb_fd_hdlc_update_pid_and_serial_number_string()
{
	// Offset to serial number string in the String descriptor array.
	int i = QHSUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE; 
	int j = 0; // index to access serial number string from IMEM.
	int k = 0;
	uint8 * str_ptr = NULL;
	uint32 *info_ptr;

	qhsusb_dload_info_type* dload_info = qhsusb_boot_dload_get_dload_info_ptr();
	if (NULL != dload_info)
	{
		/*Typecast to 32 bit to read the dload_info*/
		info_ptr = (uint32 *) dload_info;
	}
	else
	{
		/*We will never hit this case as we always get the address*/
		/*If for some reason, it is NULL, then port hopping will happen*/
		return;
	}
	for(k= 0; k < QHSUSB_DLOAD_SIZE; k++)
	{
		*(qhsusb_dload_info+k) = *(info_ptr+k);
	}
	if(qhsusb_boot_dload_is_dload_info_pid_valid((qhsusb_dload_info_type*)qhsusb_dload_info))
	{
		/*First 32 Bit Number is the Product Id from Linux*/
		hdlc_usb_device_obex.idProduct   = (uint16) qhsusb_dload_info[0]; 
	#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
		hdlc_usb_device_cdcacm.idProduct = (uint16) dload_info->qhsusb_product_id;
	#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
	}
	
	if(qhsusb_boot_dload_is_dload_info_serial_num_valid((qhsusb_dload_info_type*)qhsusb_dload_info))
	{
		hdlc_usb_device_obex.iSerialNumber = 3;
	#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
		hdlc_usb_device_cdcacm.iSerialNumber = 3;
	#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
	
		// Read serial number from IMEM and convert to Unicode string.	
		str_ptr = (uint8 *)qhsusb_dload_info;
		/*Since the First 4 Bytes are the Product Id in the structure increment the pointer*/
		str_ptr = str_ptr + 4;	

		// "i" starts from index 2 (0 based) in STRING DESCRIPTOR where UNICODE 
		// string starts

		// while loop checks for following:
		// (1) NULL termination (2) source length (3) destination buffer length
		while(str_ptr[j] && (j < QHSUSB_SERIAL_NUMBER_LEN) && (i < QHSUSB_STRING_DESC_SIZE_MAX))
		{
			serial_number_string[i++] = str_ptr[j];
			serial_number_string[i++] = 0;
			j++;
		}
	
		// Set the bLength field to value of "i" which is the sum of follwing:
		// bLenght field + bDescriptorType field + UNICODE string length
		serial_number_string[0] = i;
		// Update the descriptor structure to point to non-default serial number string.
		sw_device.strings[3].descriptor = (struct usb_desc_header*)(void*)serial_number_string;
	}
	else
	{
	#ifdef FEATURE_QHSUSB_MS
		// If we are in MS compostion and info in IMEM is not valid, set string to default.
		hdlc_usb_device_obex.iSerialNumber = 3;
		sw_device.strings[3].descriptor = (struct usb_desc_header*)(void*)str_serial;
	#else /* FEATURE_QHSUSB_MS */	
	    // If we are not in MS composition then send the default serial.
		hdlc_usb_device_obex.iSerialNumber = 3;
		#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
		hdlc_usb_device_cdcacm.iSerialNumber = 0;
		#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
	#endif /* FEATURE_QHSUSB_MS */
	}
}
#endif /* FEATURE_SKIP_SERIAL_STR_UPDATE */
/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void qhsusb_fd_hdlc_set_core(int core_id)
{
    qhsusb_log(QHSUSB_FD_HDLC_SET_CORE_LOG,0,core_id);
    DCI_CHECK_CORE_ID(core_id);
	sw_device.core_id = core_id;
}

void qhsusb_fd_set_bulk_mode(qhsusb_bulk_mode_type qhsusb_mode)
{
	if((qhsusb_mode == QHSUSB_MODE__SER_MS) || (qhsusb_mode == QHSUSB_MODE__SER_ONLY)
		||(qhsusb_mode == QHSUSB_MODE__SER_ONLY__DL_MODE))
   {
		qhsusb_bulk_mode_g = qhsusb_mode;
   }
}

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product,
                                                  void (*tx_callback)(struct qhsusb_urb* urb),
                                                  void (*enum_complete)(void),
												  void (*error_handler)(void),
                                                  qhsusb_transfer_mode transfer_mode)
#else
struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product,
                                                  void (*tx_callback)(struct qhsusb_urb* urb),
                                                  void (*enum_complete)(void),
                                                  qhsusb_transfer_mode transfer_mode)
#endif
{
   boolean hdlc_enum_already_exists = FALSE;

   struct qhsusb_urb* urb;

   qhsusb_transfer_mode_g = transfer_mode;   
   qhsusb_log(QHSUSB_FD_HDLC_INIT_LOG,0,0);
#ifndef FEATURE_SKIP_SERIAL_STR_UPDATE
   qhsusb_fd_hdlc_update_pid_and_serial_number_string();
#endif 
   if ( NULL != str_product ) {
      sw_device.strings[2].descriptor = (struct usb_desc_header*)(void*)str_product;
   }
   /* set up sw -> hw links */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   hdlc_shutdown_in_progress = 0;
   if ( 0 != hdlc_use_cdcacm ) {
      sw_data_eps[0].descriptor = &hdlc_usb_config_cdcacm.ep_in;
      sw_data_eps[1].descriptor = &hdlc_usb_config_cdcacm.ep_out;
      sw_data_ifc.descriptor = &hdlc_usb_config_cdcacm.ifc_data;
      sw_config.descriptor = &hdlc_usb_config_cdcacm.conf1;
      sw_device.descriptor = &hdlc_usb_device_cdcacm;
      sw_config.interfaces = &sw_cdc_control_ifc;
   } else 
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
   {
	if(QHSUSB_MODE__SER_MS == qhsusb_bulk_mode_g)
	{
      sw_data_eps[0].descriptor = &hdlc_usb_config_obex.ep_in;
      sw_data_eps[1].descriptor = &hdlc_usb_config_obex.ep_out;
      sw_data_ifc.descriptor = &hdlc_usb_config_obex.ifc_data;
      sw_config.descriptor = &hdlc_usb_config_obex.conf1;
	}
	else if (QHSUSB_MODE__SER_ONLY == qhsusb_bulk_mode_g)
	{
		sw_data_ifc.next = NULL;
		sw_data_eps[0].descriptor = &hdlc_usb_config_obex_only.ep_in;
		sw_data_eps[1].descriptor = &hdlc_usb_config_obex_only.ep_out;
		sw_data_ifc.descriptor = &hdlc_usb_config_obex_only.ifc_data;
		sw_config.descriptor = &hdlc_usb_config_obex_only.conf1;
		if(hdlc_usb_device_obex.idProduct == USB_PRODUCT_DIAG_MS )
		{
			/*For serial only mode, change the product ID to 0x9008*/
			hdlc_usb_device_obex.idProduct = USB_PRODUCT_DIAG_ONLY; 
			hdlc_usb_device_obex.iSerialNumber = 0x3;
		}
	}
	else if (QHSUSB_MODE__SER_ONLY__DL_MODE == qhsusb_bulk_mode_g)
	{
		sw_data_ifc.next = NULL;
		sw_data_eps[0].descriptor = &hdlc_usb_config_obex_only.ep_in;
		sw_data_eps[1].descriptor = &hdlc_usb_config_obex_only.ep_out;
		sw_data_ifc.descriptor = &hdlc_usb_config_obex_only.ifc_data;
		sw_config.descriptor = &hdlc_usb_config_obex_only.conf1;
		if(hdlc_usb_device_obex.idProduct == USB_PRODUCT_DIAG_MS )
		{
			/*For serial only mode, change the product ID to 0x9008*/
			hdlc_usb_device_obex.idProduct = USB_PRODUCT_DIAG_ONLY; 
			hdlc_usb_device_obex.iSerialNumber = 0x3;
		}
	}
      sw_device.descriptor = &hdlc_usb_device_obex;
      sw_config.interfaces = &sw_data_ifc;
#ifdef FEATURE_QHSUSB_MS
	if( QHSUSB_MODE__SER_MS == qhsusb_bulk_mode_g )
	{
		/*Initialize the MS stack when you are in download mode only*/
      qhsusb_fd_ms_init();
	}
#endif /* FEATURE_QHSUSB_MS */
   }
   // Set enum flag for RS bit to be turned ON in qhsusb_dci_init();
   qhsusb_dci_set_enum_flag(TRUE);
   qhsusb_dcd_init(&sw_device);
   if(rx_callback != NULL)
   {
   user_rx_callback = rx_callback;
   }
   else
   {
      user_rx_callback = NULL;
   }
   if (tx_callback != NULL)
   {
      user_tx_callback = tx_callback;
   }
   else
   {
      user_tx_callback = NULL;
   }
   if (enum_complete != NULL)
   {
      user_enum_complete = enum_complete;
   }
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
   if( error_handler !=NULL)
   {
	  user_error_handler = error_handler;
   }
#endif
   hdlc_enum_already_exists = dci_skip_enumeration(sw_device.core_id);

   /* Data buffers & URB's */
   /* tx */
   urb                      = &hdlc_tx_urb;
   urb->usb_device          = &sw_device;
   urb->endpoint_address    = sw_data_eps[0].descriptor->bEndpointAddress;
   urb->transfer_buffer_ptr = hdlc_tx_buffer;
   urb->transfer_length     = 0;
   urb->transfer_status     = 0;
   urb->send_zlp            = TRUE;
   urb->complete_callback   = hdlc_tx_callback;
   urb->is_zero_address_chain_required     = FALSE;

   if (transfer_mode == FD_CLIENT_SUPPLIED_BUFFER_MODE) /* Client Supplied Buffer Mode */
   {
   /* rx */
   urb                      = &hdlc_rx_urb;
   urb->usb_device          = &sw_device;
   urb->endpoint_address    = sw_data_eps[1].descriptor->bEndpointAddress;
   urb->transfer_buffer_ptr = NULL; /* to be filled by al layer */
   urb->transfer_length     = 0; /* to be filled by al layer */
   urb->transfer_status     = 0;
   urb->send_zlp            = FALSE;
   urb->complete_callback   = hdlc_rx_callback;
   urb->actual_length       = 0;
   /*Initially set the is_zero_address_chain_required to FALSE*/
   urb->is_zero_address_chain_required     = FALSE;
   }
   else /* Internal Buffer Mode */
   {
     uint8 i = 0;
     for (i = 0; i < NUM_OF_INT_RX_BUF; i++) {
       urb                      = &hdlc_rx_urb_fixed[i];
       urb->usb_device          = &sw_device;
       urb->endpoint_address    = sw_data_eps[1].descriptor->bEndpointAddress;
       urb->transfer_buffer_ptr = hdlc_rx_buffer_fixed[i];
       urb->transfer_length     = RX_BUF_SIZE_FIXED;
       urb->send_zlp            = FALSE;
       urb->transfer_status     = 0;
       urb->complete_callback   = hdlc_rx_callback_int_buf_mode;
       urb->actual_length       = 0;
       /*Initially set the is_zero_address_chain_required to FALSE*/
       urb->is_zero_address_chain_required     = FALSE;
     }   
   }

   /* cdc_notify_urb */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   if ( 0 != hdlc_use_cdcacm ) {
      urb = &cdc_notify_urb;
      urb->usb_device          = &sw_device;
      urb->endpoint_address    = hdlc_usb_config_cdcacm.ep_notify.bEndpointAddress;
      urb->transfer_length     = 0;
      urb->complete_callback   = NULL;
   }
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

#ifndef FEATURE_QHSUSB_PBL
   if (hdlc_enum_already_exists)
   {
     /** 
     *  We got here without reconnect. enumeration already
     *  completed. Simulate connection and SET_CONFIG
     */
     qhsusb_dcd_port_status_changed(&sw_device); /* attachment, speed... */
     sw_device.address = 1;                      /* don't care, just not 0 */

     qhsusb_dci_cancel_transfer_wo_dqh_dtd(sw_device.core_id, 1, QHSUSB_EP_DIR_RX);
     qhsusb_dci_cancel_transfer_wo_dqh_dtd(sw_device.core_id, 1, QHSUSB_EP_DIR_TX);
     qhsusb_dci_cancel_transfer_wo_dqh_dtd(sw_device.core_id, 2, QHSUSB_EP_DIR_RX);
     qhsusb_dci_cancel_transfer_wo_dqh_dtd(sw_device.core_id, 2, QHSUSB_EP_DIR_TX);

     qhsusb_dcd_set_config(&sw_device, 1);

     if (transfer_mode == FD_USB_INTERNAL_BUFFER_MODE)
     {
       hdlc_init_rx_int_buf_mode(urb->usb_device);
     }
   }
#endif  /* !FEATURE_QHSUSB_PBL */

   return &sw_device;
}

void qhsusb_fd_hdlc_shutdown(void)
{
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   hdlc_shutdown_in_progress = 1;
   if ( 0 != hdlc_use_cdcacm ) {
      uint32 core_id = sw_device.core_id;
      qhsusb_dci_cancel_transfer(core_id, 1, QHSUSB_EP_DIR_RX);
      qhsusb_dci_cancel_transfer(core_id, 1, QHSUSB_EP_DIR_TX);
      qhsusb_dci_cancel_transfer(core_id, 2, QHSUSB_EP_DIR_RX);
      qhsusb_dci_cancel_transfer(core_id, 2, QHSUSB_EP_DIR_TX);
      return;
}
#endif
   qhsusb_log(QHSUSB_FD_HDLC_SHUTDOWN_LOG,0,0);
   qhsusb_dcd_shutdown(&sw_device);
}

void qhsusb_fd_hdlc_tx_byte(unsigned char chr)
{
   struct qhsusb_urb* urb = &hdlc_tx_urb;
   
   qhsusb_log(QHSUSB_FD_HDLC_TX_BYTE_LOG,0,chr);
   /**
    * @invariant current buffer not full 
    *  
    * Proof: After each Tx byte, buffer state checked and in case 
    * buffer full, it get transmitted and emptied. 
    */
   while ( QHSUSB_URB_STATUS_ACTIVE == urb->transfer_status ) {
      qhsusb_dcd_poll(&sw_device);
   }
   ((uint8*)(urb->transfer_buffer_ptr))[urb->transfer_length++] = chr;
   if ( ( ( 0x7e == chr ) && ( urb->transfer_length > 1 ) ) || 
        (urb->transfer_length >= TX_BUF_SIZE) ) { /* send */
      qhsusb_submit_urb(urb);
   }
}

void qhsusb_fd_hdlc_tx_pkt(unsigned char *pkt, uint32 len)
{
   struct qhsusb_urb* urb = &hdlc_tx_urb;
   void *internal_buff_pointer =NULL; 

   qhsusb_log(QHSUSB_FD_HDLC_TX_PKT_LOG,0,len);
   while ( QHSUSB_URB_STATUS_ACTIVE == urb->transfer_status ) 
   {
      qhsusb_dcd_poll(&sw_device);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	  if (TRUE == qhsusb_cancel_transfer)
      {
	  	  qhsusb_cancel_transfer= FALSE;
		 break;
      }
#endif
   }

   /* If pkt points to addr zero, we may need to copy the data to our own buffer. 
         This is needed for Ramdump collection usecase , where contents from  addr:0x0 are dumped*/
   if ((NULL == pkt) && (len > 0) && (len > TX_BUF_SIZE))
   {
    qhsusb_log(QHSUSB_FD_HDLC_TX_PKT_ZERO_ADDRESS_LOG,0,len);  
    /*point the pointer to the internal buffer*/
    internal_buff_pointer = hdlc_tx_buffer;
    /*copy to the internal Buffer first 512 */
    memscpy(internal_buff_pointer,TX_BUF_SIZE,pkt,TX_BUF_SIZE);
    /*set the zero address flag as TRUE*/
    urb->is_zero_address_chain_required = TRUE; 
    /*Submit the URB. This URB will not initiate a transfer*/
    urb->transfer_length = TX_BUF_SIZE;
    qhsusb_submit_urb(urb);

    /*Let the next URB take the client supplied Buffer*/
    urb->transfer_buffer_ptr = (pkt + TX_BUF_SIZE);
    urb->transfer_length = (len-TX_BUF_SIZE);
    urb->is_zero_address_chain_required = FALSE;
   }
   else if ((NULL == pkt) && (len > 0) && (len <=TX_BUF_SIZE))
   {
    /* for packets which are less than or equal to TX_BUF_SIZE  */
    /*point the pointer to the internal buffer*/
    qhsusb_log(QHSUSB_FD_HDLC_TX_PKT_ZERO_ADDRESS_LOG,0,len); 
    internal_buff_pointer = hdlc_tx_buffer;
    /*copy to the internal Buffer first 16k */
    memscpy(internal_buff_pointer,TX_BUF_SIZE,pkt,len);
    /*set the zero address flag as FALSE, here we want to prime*/
    /* the endpoint*/
    urb->is_zero_address_chain_required = FALSE; 
    urb->transfer_length = len;
   }
   else
   {
      /* in most case, we should be able avoid memcpy */
      urb->transfer_buffer_ptr = pkt;
      urb->transfer_length = len;
      urb->is_zero_address_chain_required = FALSE;
   }
   
   qhsusb_log(QHSUSB_TRANSFER_LENGTH_LOG,0,urb->transfer_length);
   /* send urb */
   qhsusb_submit_urb(urb);
   /* wait here until transfer is finished */
   while ( QHSUSB_URB_STATUS_ACTIVE == urb->transfer_status )
   {
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	  if (TRUE == qhsusb_cancel_transfer)
      {
	  	  qhsusb_cancel_transfer= FALSE;
		 break;
      }
#endif
      qhsusb_dcd_poll(&sw_device);
   }

   /* restore tx urb buffer */
   urb->transfer_buffer_ptr = hdlc_tx_buffer;
   
}

int qhsusb_fd_hdlc_is_tx_active(void)
{
   if ( ( QHSUSB_URB_STATUS_ACTIVE == hdlc_tx_urb.transfer_status ) || 
        ( QHSUSB_URB_STATUS_ACTIVE == sw_device.setup_tx_urb.transfer_status ) ) {
		qhsusb_log(QHSUSB_FD_HDLC_IS_TX_ACTIVE_LOG,0,1);
      return 1;
   }
#ifdef FEATURE_QHSUSB_HDLC_MS
   return (qhsusb_fd_ms_is_tx_active());
#endif /* FEATURE_QHSUSB_HDLC_MS */
   qhsusb_log(QHSUSB_FD_HDLC_IS_TX_ACTIVE_LOG,0,0);
   return 0;
}

uint32 qhsusb_fd_hdlc_rx_pkt(void *buff_ptr, uint32 len)
{
  struct qhsusb_urb* urb = &hdlc_rx_urb;
  qhsusb_log(QHSUSB_FD_HDLC_RX_PKT_LOG,0,len);
  /* handle previously active RX transfers */
  if (QHSUSB_URB_STATUS_ACTIVE == hdlc_rx_urb.transfer_status)
  {
    while (QHSUSB_URB_STATUS_ACTIVE == hdlc_rx_urb.transfer_status)
    {
      qhsusb_dcd_poll(&sw_device); /* finish the previous transfer internally before proceeding */
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	  if (TRUE == qhsusb_cancel_transfer)
	  {
	  	  qhsusb_cancel_transfer= FALSE;
	  	  break;
	  }
#endif
    }
  }

  /* avoid memcpy for all cases except destination address 0 */
  if ((NULL == buff_ptr) && (len > 0))
  {
    DCI_ASSERT(QHSUSB_ERR_BAD_PARAM);
  }
  else
  {
    urb->transfer_buffer_ptr = buff_ptr;
  }
  urb->transfer_length = len;
  qhsusb_log(QHSUSB_SUBMIT_RX_URB_LOG,0,len); 
  /* send urb */
  qhsusb_submit_urb(urb);

  return FD_HDLC_SUCCESS;
}

void qhsusb_fd_hdlc_rx_completed(struct qhsusb_urb* urb)
{
  qhsusb_log(QHSUSB_FD_HDLC_RX_COMPLETED_LOG,0,urb->actual_length);
  urb->actual_length = 0;
}

void qhsusb_fd_hdlc_update_vid_pid(uint16 idVendor, uint16 idProduct)
{
  qhsusb_log(QHSUSB_FD_HDLC_UPDATE_VID_PID_LOG,0,0);
  if (idVendor!=0)
  {
    hdlc_usb_device_obex.idVendor   = idVendor;
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
    hdlc_usb_device_cdcacm.idVendor = idVendor;
#endif
  }
  if (idProduct!=0)
  {
    hdlc_usb_device_obex.idProduct   = idProduct;
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
    hdlc_usb_device_cdcacm.idProduct = idProduct;
#endif
  }
}

void qhsusb_fd_hdlc_get_vid_pid(uint16 *idVendor, uint16 *idProduct,
                                                  uint8   is_cdc_acm_dev)
{
  qhsusb_log(QHSUSB_FD_HDLC_GET_VID_PID_LOG,0,0);
  if (is_cdc_acm_dev)
  {
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
    if (idVendor!=0)
    {
      *idVendor = hdlc_usb_device_cdcacm.idVendor;
    }
    if (idProduct!=0)
    {
      *idProduct = hdlc_usb_device_cdcacm.idProduct;
    }
#endif
  }
  else
  {
    if (idVendor!=0)
    {
      *idVendor = hdlc_usb_device_obex.idVendor;
    }
    if (idProduct!=0)
    {
      *idProduct = hdlc_usb_device_obex.idProduct;
    }
  }
}



/** 
  Legacy RX buffer priming related functions for DMSS and Streaming Download Protocols
  These protocols relies on USB internal buffers for transfers which the client later copies and process
  In the future, move to Sahara protocol for optimization
 */

/**
 * Internal Buffer Mode
 *
 * Try to submit Rx URB
 * 
 * No more then 1 URB may be active at a time, thus check for Rx 
 * idle. If no Rx is in progress, find 1-st available URB that 
 * is not currently used, i.e. actual_length==0. 
 *  
 * @note May block waiting while Rx URB is successfully 
 *       submitted
 *  
 */
static void hdlc_init_rx_int_buf_mode(const struct qhsusb_dcd_dsc_device* dev)
{
   /**
    * If we are in the middle of the shutdown process, attempt to 
    * re-init Rx path will lead to endless loop. Relevant for 
    * CDC/ACM only 
    */
  int i;
  for (i = 0; i < NUM_OF_INT_RX_BUF; i++)
  {
     if (QHSUSB_URB_STATUS_ACTIVE == hdlc_rx_urb_fixed[i].transfer_status)
        return;
  }

  for (i = 0; i < NUM_OF_INT_RX_BUF; i++) {
     struct qhsusb_urb* urb = &hdlc_rx_urb_fixed[i];
     if ( 0 == urb->actual_length ) {
         while ( 0 != qhsusb_submit_urb(urb) ) {
            qhsusb_dcd_poll(&sw_device);
            if ( 0 == dev->config_value ) {
                 return;
               }
         }
         return;
     }
  }
}
/**
 * RX complete for internal buffer mode
 */
static void hdlc_rx_callback_int_buf_mode(struct qhsusb_urb* urb)
{
  qhsusb_log(HDLC_RX_CALLBACK_INT_BUF_MODE_LOG,0,0);
  hdlc_init_rx_int_buf_mode(urb->usb_device);
  user_rx_callback(urb);
}

void qhsusb_fd_hdlc_rx_completed_int_buf_mode(struct qhsusb_urb* urb)
{
  qhsusb_log(QHSUSB_FD_HDLC_RX_COMPLETED_INT_BUF_MODE_LOG,0,0);
  urb->actual_length = 0;
  hdlc_init_rx_int_buf_mode(urb->usb_device);
}

/*API to inform the lower layers that Bulk Download is in Progress*/
void qhsusb_fd_hdlc_bulk_in_progress(boolean is_bulk_progress)
{
	dci_set_bulk_progress(is_bulk_progress);
}


