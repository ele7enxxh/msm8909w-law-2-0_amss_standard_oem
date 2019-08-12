/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file jusb_chp9.h

Holds definitions for implementation of Chapter 9 (USB Device Framework) of 
the USB 2.0 Specification.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jusb_chp9.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _JUSB_CHP9_H_
#define _JUSB_CHP9_H_

#include <jusb_common.h>


/** @ingroup hsusb_jungo_api 
@{ */

/* CONTROL REQUEST SUPPORT */
#define USB_DIR_OUT                     0       /* to device */
#define USB_DIR_IN                      0x80    /* to host */

/* USB types, the second of three bRequestType fields */
#define USB_TYPE_MASK                   (0x03 << 5)
#define USB_TYPE_STANDARD               (0x00 << 5)
#define USB_TYPE_CLASS                  (0x01 << 5)
#define USB_TYPE_VENDOR                 (0x02 << 5)
#define USB_TYPE_RESERVED               (0x03 << 5)

/* USB recipients, the third of three bRequestType fields */
#define USB_RECIP_MASK                  0x1f
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03

/* Retrive the type of the request recieved */
#define USB_REQ_TYPE                    (0x01 << 7)
#define USB_REQ_GET                     1
#define USB_REQ_SET                     0

/* Standard requests, for the bRequest field of a SETUP packet. */
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C

/* SuperSpeed standard requests. USB 3.0 spec table 9-4 */
#define USB_REQ_SET_SEL                 0x30
#define USB_REQ_SET_ISOCH_DELAY         0x31

/* USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE */
#define USB_DEVICE_SELF_POWERED         0       /* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP        1       /* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE            2       /* (high speed only) */
#define USB_DEVICE_B_HNP_ENABLE         3       /* dev may initiate HNP */
#define USB_DEVICE_A_HNP_SUPPORT        4       /* RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT    5       /* other RH port does */

#define USB_ENDPOINT_HALT               0       /* IN/OUT will STALL */

/** Retrieves the EP/Interface details from the Request structure.
*/
#define USB_REQ_ENDPOINT_NUMBER(wIndex)         (le16toh(wIndex) & 0x000f)
/** Retrieves the EP/Interface details from the Request structure.
*/
#define USB_REQ_ENDPOINT_DIRECTION(wIndex)      (le16toh(wIndex) & (0x01 << 7))
/** Retrieves the EP/Interface details from the Request structure.
*/
#define USB_REQ_INTERFACE_NUMBER(wIndex)        (le16toh(wIndex) & 0x00ff)

/** Setup data for a USB device control request.\ This is used by the 
usb_ctrlrequest struct.
*/
#define USB_CONTROL_REQUEST_SIZE        8

/** USB 2.0 specification control request structure.
*/
struct usb_ctrlrequest {
    juint8_t bRequestType;  /**< Direction, type, and recipient of the control 
                                 transfer. */
    juint8_t bRequest;      /**< Request type of the control transfer. */
    juint16_t wValue;       /**< Value parameter to the control transfer */
    juint16_t wIndex;       /**< Index parameter to the control transfer */
    juint16_t wLength;      /**< Length of the control transfer data phase. */
};

/* SuperSpeed Set SEL request data structure */
#define USB_SET_SEL_REQUEST_SIZE        6
struct usb_set_SEL_request
{
  uint8  U1SEL;
  uint8  U1PEL;
  uint16 U2SEL;
  uint16 U2PEL;
};
/* STANDARD DESCRIPTORS - returned by GET_DESCRIPTOR */

/* Standard descriptor type returned by GET_DESCRIPTOR (per Table 9.5 in the 
    USB 2.0 specification)*/

#define USB_DT_DEVICE                   0x01  /**< DEVICE descriptor. */
#define USB_DT_CONFIG                   0x02  /**< CONFIGURATION descriptor. */
#define USB_DT_STRING                   0x03  /**< STRING descriptor. */ 
#define USB_DT_INTERFACE                0x04  /**< INTERFACE descriptor. */
#define USB_DT_ENDPOINT                 0x05  /**< ENDPOINT descriptor. */
#define USB_DT_DEVICE_QUALIFIER         0x06  /**< DEVICE QULAIFIER descriptor. */
#define USB_DT_OTHER_SPEED_CONFIG       0x07  /**< OTHER SPEED descriptor. */
#define USB_DT_INTERFACE_POWER          0x08  /**< INTERFACE POWER descriptor. */

/* Standard descriptor type returned by GET_DESCRIPTOR (per a minor revision 
  to the USB 2.0 specification for ECN) */

#define USB_DT_OTG                      0x09  /**< OTG descriptor. */
#define USB_DT_DEBUG                    0x0a  /**< DEBUG descriptor. */
#define USB_DT_INTERFACE_ASSOCIATION    0x0b  /**< INTERFACE ASSOCIATION 
                                                   descriptor. */

/* Conventional codes for class-specific standard descriptors */

#define USB_DT_CS_DEVICE                0x21  /**< DEVICE code. */
#define USB_DT_CS_CONFIG                0x22  /**< CONFIG code. */
#define USB_DT_CS_STRING                0x23  /**< STRING code. */
#define USB_DT_CS_INTERFACE             0x24  /**< INTERFACE code. */
#define USB_DT_CS_ENDPOINT              0x25  /**< ENDPOINT code.*/

/** Provides two fields that all standard descriptors have at the beginning 
    of the descriptor.
*/
struct usb_descriptor_header {
    juint8_t  bLength;          /**< Length of the following descriptor. */
    juint8_t  bDescriptorType;  /**< Type of the following descriptor. */
};

#define USB_DT_DEVICE_SIZE              18

/** USB_DT_DEVICE device descriptor.
*/
struct usb_device_descriptor {
    juint8_t  bLength;             /**< Size of the descriptor in bytes. */
    juint8_t  bDescriptorType;     /**< Device descriptor. */
    juint16_t bcdUSB;              /**< USB specification number to which the 
                						device complies. */
    juint8_t  bDeviceClass;        /**< Class code (assigned by USB Org).\ If 
								        equal to zero, each interface specifies
                                        it's own class code.\ If equal to 0xFF, 
                    					the class code is vendor specified.\ 
                                        Otherwise, the field is a valid Class 
										code. */
    juint8_t  bDeviceSubClass;     /**< Subclass code (assigned by USB Org). */
    juint8_t  bDeviceProtocol;     /**< Protocol code (assigned by USB Org). */
    juint8_t  bMaxPacketSize0;     /**< Maximum packet size for a zero
								        endpoint.\ Valid sizes are 8, 16, 32,
										and 64. */
    juint16_t idVendor;            /**< Vendor ID (assigned by USB Org). */
    juint16_t idProduct;           /**< Product ID (assigned by the
								        Manufacturer). */
    juint16_t bcdDevice;           /**< Device release number. */
    juint8_t  iManufacturer;       /**< Index of the manufacturer string 
								        descriptor. */
    juint8_t  iProduct;            /**< Index of the product string
								        descriptor. */
    juint8_t  iSerialNumber;       /**< Index of the serial number string 
								        descriptor. */
    juint8_t  bNumConfigurations;  /**< Number of possible configurations. */
};

/* Device or interface class codes as found in bDeviceClass or bInterfaceClass 
    and defined by www.usb.org documents. */

#define USB_CLASS_PER_INTERFACE         0  /**< For Device class. */
#define USB_CLASS_AUDIO                 1  /**< Audio class. */
#define USB_CLASS_COMM                  2  /**< Communication class. */
#define USB_CLASS_HID                   3  /**< Human Interface Device class. */
#define USB_CLASS_PHYSICAL              5  /**< Physical class. */
#define USB_CLASS_STILL_IMAGE           6  /**< Still Image class. */
#define USB_CLASS_PRINTER               7  /**< Printer class. */
#define USB_CLASS_MASS_STORAGE          8  /**< Mass Storage class. */
#define USB_CLASS_HUB                   9  /**< Hub class. */
#define USB_CLASS_CDC_DATA              0x0a  /**< Communication Device class. */
#define USB_CLASS_CSCID                 0x0b  /**< Chip + Smart Card class. */
#define USB_CLASS_CONTENT_SEC           0x0d  /**< Content Security class. */
#define USB_CLASS_VIDEO                 0x0e  /**< Video class. */
#define USB_CLASS_MISC                  0xef  /**< MISC class. */
#define USB_CLASS_APP_SPEC              0xfe  /**< Application-specific class. */
#define USB_CLASS_VENDOR_SPEC           0xff  /**< Vendor-specific class. */

/** USB_CLASS_MISC subclass.
*/
#define USB_MISC_SUBCLASS_COMMON 0x02

/** USB_CLASS_MISC and USB_MISC_SUBCLASS_COMMON protocols.
*/
#define USB_MISC_SUBCLASS_COMMON_PRTCL_IAD 0x01

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */

#define USB_DT_CONFIG_SIZE      9

/** USB_DT_CONFIG configuration descriptor information.
*/
struct usb_config_descriptor {
    juint8_t  bLength;             /**< Size of the descriptor in bytes. */
    juint8_t  bDescriptorType;     /**< Configuration descriptor. */
    juint16_t wTotalLength;        /**< Total length in bytes of data
								        returned. */
    juint8_t  bNumInterfaces;      /**< Number of interfaces. */
    juint8_t  bConfigurationValue; /**< Value to use as an argument to select 
                                        this configuration. */
    juint8_t  iConfiguration;      /**< Index of the string descriptor
								        describing this configuration. */
    juint8_t  bmAttributes;        /**< Attributes: Self Powered, Remote 
                                        Wakeup. */
    juint8_t  bMaxPower;           /**< Maximum power consumption in 2 mA
								        units. */
};

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE              (1 << 7)        /* must be set */
#define USB_CONFIG_ATT_SELFPOWER        (1 << 6)        /* self powered */
#define USB_CONFIG_ATT_WAKEUP           (1 << 5)        /* can wakeup */

/** USB_DT_STRING string descriptor.
@note String zero is special; it holds language codes that the device supports,
      not Unicode characters.
*/
struct usb_string_descriptor {
    juint8_t  bLength;          /**< Length of the string descriptor. */
    juint8_t  bDescriptorType;  /**< Type of the string descriptor. */

    juint16_t wData[1];         /**< UTF-16LE encoded. */
};

#define USB_DT_INTERFACE_SIZE   9

/** USB_DT_INTERFACE interface descriptor.
*/
struct usb_interface_descriptor {
    juint8_t  bLength;             /**< Size of the descriptor in bytes. */
    juint8_t  bDescriptorType;     /**< Interface descriptor. */

    juint8_t  bInterfaceNumber;    /**< Number of the interface. */
    juint8_t  bAlternateSetting;   /**< Value used to select an alternative
								        setting. */
    juint8_t  bNumEndpoints;       /**< Number of endpoints used for this
								        interface. */
    juint8_t  bInterfaceClass;     /**< Class code (assigned by USB Org).*/
    juint8_t  bInterfaceSubClass;  /**< Subclass code (assigned by USB Org). */
    juint8_t  bInterfaceProtocol;  /**< Protocol code (assigned by USB Org). */
    juint8_t  iInterface;          /**< Index of the string descriptor
								        describing this interface. */
};

/* USB_DT_ENDPOINT: Endpoint descriptor */
#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_AUDIO_ENDPOINT_SIZE      9

/* Endpoints */
#define USB_ENDPOINT_NUMBER_MASK        0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3

#define USB_DT_QUALIFIER_SIZE   10

/** USB_DT_DEVICE_QUALIFIER device qualifier descriptor.
*/
struct usb_qualifier_descriptor {
    juint8_t  bLength;             /**< Length of the following descriptor. */
    juint8_t  bDescriptorType;     /**< Qualifier descriptor. */

    juint16_t bcdUSB;              /**< USB specification number to which the 
								        device complies. */
    juint8_t  bDeviceClass;        /**< Class code (assigned by USB Org). */
    juint8_t  bDeviceSubClass;     /**< Subclass code (assigned by USB Org). */
    juint8_t  bDeviceProtocol;     /**< Protocol code (assigned by USB Org). */
    juint8_t  bMaxPacketSize0;     /**< Maximum packet size for the zero
								        endpoint. */
    juint8_t  bNumConfigurations;  /**< Number of possible configurations. */
    juint8_t  bRESERVED;           /**< Reserved. */
};

#define USB_DT_OTG_SIZE 3

/** USB_DT_OTG (see "On-The-Go Supplement to the USB 2.0 Specification" 
    Revision 1.3).
*/
struct usb_otg_descriptor {
    juint8_t  bLength;           /**< Length of the following descriptor. */
    juint8_t  bDescriptorType;   /**< OTG descriptor. */

    juint8_t  bmAttributes;      /**< Support for HNP, SRP, etc. */
};

/* from usb_otg_descriptor.bmAttributes */
#define USB_OTG_SRP             (1 << 0)
#define USB_OTG_HNP             (1 << 1)        /* swap host/device roles */


#define USB_DT_INTERFACE_ASSOC_SIZE 8

/** USB_DT_INTERFACE_ASSOCIATION groups interfaces.
*/
struct usb_interface_assoc_descriptor {
    juint8_t  bLength;           /**< Length of the following descriptor. */
    juint8_t  bDescriptorType;   /**< Interface association descriptor. */

    juint8_t  bFirstInterface;   /**< Number of the first interface*/
    juint8_t  bInterfaceCount;   /**< Total number of interfaces. */
    juint8_t  bFunctionClass;    /**< Class code (assigned by USB Org).*/
    juint8_t  bFunctionSubClass; /**< Subclass code (assigned by USB Org). */
    juint8_t  bFunctionProtocol; /**< Protocol code (assigned by USB Org). */
    juint8_t  iFunction;         /**< Index of the string descriptor describing
                                      this interface. */
};

#define USB_GET_STATUS_SIZE        2
#define USB_MAX_PACKET_SIZE_SS_EP0 9

/** @} */ /* end_group hsusb_jungo_api */

#endif
