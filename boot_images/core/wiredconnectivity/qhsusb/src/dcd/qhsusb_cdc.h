#ifndef QHSUSB_CDC_H
#define QHSUSB_CDC_H
/*=======================================================================*//**
  @file         qhsusb_cdc.h
 
  @brief        USB CDC
 
  @details      
                Defines data structures for 
                "Class Definition for Communications Devices"
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dcd/qhsusb_cdc.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"

#define USB_SETUP_REQ_CDC_SEND_ENCAPS_CMD          (0x00)
#define USB_SETUP_REQ_CDC_GET_ENCAPS_RESP          (0x00)
#define USB_SETUP_REQ_CDC_SET_LINE_CODING          (0x20)
#define USB_SETUP_REQ_CDC_GET_LINE_CODING          (0x21)
#define USB_SETUP_REQ_CDC_SET_CONTROL_LINE_STATE   (0x22)
#define USB_SETUP_REQ_CDC_SEND_BREAK               (0x23)

#define USB_DEV_CLASS_CDC                  (2)

#define USB_IFC_CLASS_CDC                  (2)
#define USB_IFC_CLASS_CDC_DATA            (10)

#define USB_IFC_SUBCLASS_CDC_ACM           (2)
#define USB_IFC_SUBCLASS_CDC_OBEX         (11)

#define USB_DESCTYPE_CS_INTERFACE       (0x24)
#define USB_DESCTYPE_CS_ENDPOINT        (0x25)

/* CDC common header*/
/**
 * Any CDC descriptor begins with this header
 */
typedef PACKED struct usb_desc_cdc {
   uint8  bLength;
   uint8  bDescriptorType;         /**< USB_DESCTYPE_CS_INTERFACE */
   uint8  bDescriptorSubType;
} PACKED_POST usb_desc_cdc_t;

#define USB_DESCSUBTYPE_CSIFC_HEADER        (0x00) /**< Header */
#define USB_DESCSUBTYPE_CSIFC_CM            (0x01) /**< Call Management */
#define USB_DESCSUBTYPE_CSIFC_ACM           (0x02) /**< Abstract Call Management */
#define USB_DESCSUBTYPE_CSIFC_UNION         (0x06) /**< Union */

typedef PACKED struct usb_desc_cdc_header {
   uint8  bLength;
   uint8  bDescriptorType;         /**< USB_DESCTYPE_CS_INTERFACE */
   uint8  bDescriptorSubType;      /**< USB_DESCSUBTYPE_CSIFC_HEADER */
   uint16 bcdCDC;                  /**< CDC spec version */
} PACKED_POST usb_desc_cdc_header_t;

typedef PACKED struct usb_desc_cdc_union {
   uint8  bLength;
   uint8  bDescriptorType;         /**< USB_DESCTYPE_CS_INTERFACE */
   uint8  bDescriptorSubType;      /**< USB_DESCSUBTYPE_CSIFC_UNION */
   uint8  bControlInterface;       /**< Ifc number for controlling interface */
   uint8  bSubordinateInterface[1];/**< Ifc numbers for subordinate interfaces */
} PACKED_POST usb_desc_cdc_union_t;

/**
 * From PSTN120
 */

typedef PACKED struct usb_desc_cdc_cm {
   uint8  bLength;
   uint8  bDescriptorType;         /**< USB_DESCTYPE_CS_INTERFACE */
   uint8  bDescriptorSubType;      /**< USB_DESCSUBTYPE_CSIFC_CM */
   /** 
    * The capabilities that this configuration supports:
    *  - D7..D2: RESERVED (Reset to zero)
    *  - D1: 0 - Device sends/receives call management information
    *  only over the Communications Class interface.
    *  1 - Device can send/receive call management information over
    *  a Data Class interface.
    *  - D0: 0 - Device does not handle call management itself.
    *  1 - Device handles call management itself.
    * 
    *  The previous bits, in combination, identify which call
    *  management scenario is used. If  bit D0 is reset to 0, then
    *  the value of bit D1 is ignored. In this case, bit D1 is
    *  reset to zero for future compatibility.
    */
   uint8  bmCapabilities;
   uint8  bDataInterface;          /**< Ifc number for data interface */
} PACKED_POST usb_desc_cdc_cm_t;

typedef PACKED struct usb_desc_cdc_acm {
   uint8  bLength;
   uint8  bDescriptorType;         /**< USB_DESCTYPE_CS_INTERFACE */
   uint8  bDescriptorSubType;      /**< USB_DESCSUBTYPE_CSIFC_ACM */
   /** 
    * The capabilities that this configuration supports. (A bit
    * value of zero means that the request is not supported.)
    * - D7..D4: RESERVED (Reset to zero)
    * - D3: 1 - Device supports the notification
    *   Network_Connection.
    * - D2: 1 - Device supports the request Send_Break
    * - D1: 1 - Device supports the request combination of
    * Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and
    * the notification Serial_State.
    * - D0: 1 - Device supports the request combination of
    * Set_Comm_Feature, Clear_Comm_Feature, and Get_Comm_Feature.
    * 
    * The previous bits, in combination, identify which
    * requests/notifications are supported by a CommunicationsClass
    * interface with the SubClass code of Abstract Control Model.
    */
   uint8  bmCapabilities;
} PACKED_POST usb_desc_cdc_acm_t;

typedef PACKED struct usb_cdc_line_coding {
   uint32 dwDTERate;       /**< Data Terminal Rate, bits per second */
   uint8  bCharFormat;     /**< Stop bits: 0 - 1 stop bit, 1 - 1.5, 2 - 2 */
   uint8  bParityType;     /**< 0 - none, 1 - odd, 2 - even, 3 - mark, 4 - space */
   uint8  bDataBits;       /**< one of {5, 6, 7, 8, 16} */
} PACKED_POST usb_cdc_line_coding_t;

typedef PACKED struct usb_cdc_notification {
   uint8  bmRequestType;
   uint8  bNotificationCode;
#define USB_CDC_NOTIFICATION_SERIAL_STATE             (0x20)
   uint16 wValue;
   uint16 wIndex;
   uint16 wLength;
} PACKED_POST usb_cdc_notification_t;

typedef PACKED struct usb_cdc_notify_serial_state {
   usb_cdc_notification_t notify;
   uint16 ctrl_in;
} PACKED_POST usb_cdc_notify_serial_state_t;

#endif /* QHSUSB_CDC_H */

