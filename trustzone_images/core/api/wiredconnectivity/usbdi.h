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


/**@file usbdi.h

Contains the USB Driver layer's interface for use by the device/class 
drivers. 

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/usbdi.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.

    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
**************************************************************************/ 

#ifndef _USBDI_H_
#define _USBDI_H_


/** @ingroup hsusb_jungo_api 
@{ */

/** Pointer to the usbd_bus structure that contains USB driver bus 
    information.
*/
typedef struct usbd_bus         *usbd_bus_handle;

/** Pointer to the usbd_device structure that contains USB driver device 
    information.
*/
typedef struct usbd_device      *usbd_device_handle;

/** Pointer to the usbd_interface structure that contains USB driver 
    interface information.
*/
typedef struct usbd_interface   *usbd_interface_handle;

/** Pointer to the usb_pipe structure that contains USB driver pipe 
    information.
*/
typedef struct usbd_pipe        *usbd_pipe_handle;

/** Pointer to the usbd_xfer structure that contains USB driver transfer 
    information.
*/
typedef struct usbd_xfer        *usbd_xfer_handle;

typedef void                    *usbd_private_handle;

#include <usbdi_status.h>

typedef void (*usbd_callback)(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status);

/** Open flags. */
#define USBD_EXCLUSIVE_USE      0x01

/** Uses the default interval (specified by the endpoint descriptor) on 
    the interrupt pipe. */
#define USBD_DEFAULT_INTERVAL   (-1)

/** Request flag: do not copy data to the DMA buffer. */
#define USBD_NO_COPY            0x01

/** Request flag: wait for completion. */
#define USBD_SYNCHRONOUS        0x02

/** Request flag: allow short reads. */
#define USBD_SHORT_XFER_OK      0x04

/** Request flag: force a last short packet on a write operation. */
#define USBD_FORCE_SHORT_XFER   0x08

/** Request flag: the buffer can already can be accessed and used by the 
    DMA. */
#define USBD_DMA_BUF            0x10

#define USBD_NO_TIMEOUT 0
#define USBD_DEFAULT_TIMEOUT 5000 /* ms = 5 s */

usbd_status usbd_open_pipe(usbd_interface_handle iface, juint8_t address,
    juint8_t flags, usbd_pipe_handle *pipe);
void usbd_close_pipe(usbd_pipe_handle pipe);
usbd_status usbd_transfer(usbd_xfer_handle req);
usbd_xfer_handle usbd_alloc_xfer(usbd_device_handle dev);
void usbd_free_xfer(usbd_xfer_handle xfer);
void usbd_setup_xfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    usbd_private_handle priv, void *buffer, juint32_t length, juint16_t flags,
    juint32_t timeout, usbd_callback callback);
void usbd_setup_default_xfer(usbd_xfer_handle xfer, usbd_device_handle dev,
    usbd_private_handle priv, juint32_t timeout, usb_device_request_t *req, 
    void *buffer, juint32_t length, juint16_t flags, usbd_callback);
void usbd_setup_isoc_xfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    usbd_private_handle priv, juint16_t *frlengths, juint32_t nframes,
    juint16_t flags, usbd_callback);
void usbd_get_xfer_status(usbd_xfer_handle xfer, usbd_private_handle *priv,
    void **buffer, juint32_t *count, usbd_status *status);
usb_endpoint_descriptor_t 
*usbd_interface2endpoint_descriptor (usbd_interface_handle iface, 
    juint8_t address);
void usbd_abort_pipe(usbd_pipe_handle pipe);
void usbd_abort_request(usbd_xfer_handle xfer);
usbd_status usbd_clear_endpoint_stall(usbd_pipe_handle pipe);
usbd_status usbd_clear_endpoint_stall_async(usbd_pipe_handle pipe);
void usbd_clear_endpoint_toggle(usbd_pipe_handle pipe);
usbd_status usbd_endpoint_count(usbd_interface_handle iface, juint8_t *count);
usbd_status usbd_interface_count(usbd_device_handle dev, juint8_t *count);
void usbd_interface2device_handle(usbd_interface_handle iface,
    usbd_device_handle *dev);
usbd_status usbd_device2interface_handle(usbd_device_handle dev, 
    juint8_t ifaceno, usbd_interface_handle *iface);

usbd_device_handle usbd_pipe2device_handle(usbd_pipe_handle pipe);
void *usbd_alloc_buffer(usbd_xfer_handle xfer, juint32_t size);
void usbd_free_buffer(usbd_xfer_handle xfer);
void *usbd_get_buffer(usbd_xfer_handle xfer);
usbd_status usbd_sync_transfer(usbd_xfer_handle req);
usbd_status usbd_open_pipe_intr(usbd_interface_handle iface, juint8_t address,
    juint8_t flags, usbd_pipe_handle *pipe, usbd_private_handle priv,
    void *buffer, juint32_t length, usbd_callback cb, jint_t ival);
usbd_status usbd_do_request(usbd_device_handle dev, usb_device_request_t *req,
    void *data);
usbd_status usbd_do_request_async(usbd_device_handle dev, 
    usb_device_request_t *req, void *data);
usbd_status usbd_do_request_flags(usbd_device_handle dev,
    usb_device_request_t *req, void *data, juint16_t flags, juint32_t *actlen,
    juint32_t timeout);
usbd_status usbd_do_request_flags_pipe(usbd_device_handle dev,
    usbd_pipe_handle pipe, usb_device_request_t *req, void *data, 
    juint16_t flags, juint32_t *actlen, juint32_t);
usb_interface_descriptor_t 
*usbd_get_interface_descriptor (usbd_interface_handle iface);
usb_config_descriptor_t *usbd_get_config_descriptor(usbd_device_handle dev);
usb_device_descriptor_t *usbd_get_device_descriptor(usbd_device_handle dev);
usbd_status usbd_set_interface(usbd_interface_handle iface, jint_t altidx);
jint_t usbd_get_no_alts(usb_config_descriptor_t *cdesc, jint_t ifaceno);
usbd_status  usbd_get_interface(usbd_interface_handle iface, juint8_t *aiface);
jint_t usbd_get_interface_altindex(usbd_interface_handle iface);
usb_otg_descriptor_t *usbd_get_otg_descriptor(usbd_device_handle dev);

usb_interface_descriptor_t *usbd_find_idesc(usb_config_descriptor_t *cd,
    jint_t iindex, jint_t altidx);
usb_endpoint_descriptor_t *usbd_find_edesc(usb_config_descriptor_t *cd,
    jint_t ifaceidx, jint_t altidx, jint_t endptidx);

const char *usbd_errstr(usbd_status err);

void usbd_devinfo(usbd_device_handle dev, jint_t showclass, char *cp,
    juint32_t len);
const struct usbd_quirks *usbd_get_quirks(usbd_device_handle dev);
usb_endpoint_descriptor_t *usbd_get_endpoint_descriptor
(usbd_interface_handle iface, juint8_t address);

usbd_status usbd_reload_device_desc(usbd_device_handle dev);

/** USB device information.
*/
struct usb_devno {
    juint16_t ud_vendor;    /**< Vendor ID. */
    juint16_t ud_product;   /**< Product ID. */
};
const struct usb_devno *usb_match_device(const struct usb_devno *tbl,
    juint_t nentries, juint_t sz, juint16_t vendor, juint16_t product);

#define USB_PRODUCT_ANY         0xffff

/** Attached USB device's data.
 */
struct usb_attach_arg {
    juint8_t                     port;     /**< Port number. */
    jint_t                       configno; /**< Configuration number. */
    jint_t                       ifaceno;  /**< Interface number. */
    
    jint_t                       vendor;   /**< Vendor ID. */
    jint_t                       product;  /**< Product ID. */
    jint_t                       release;  /**< Release number. */
    
    usbd_device_handle          device;    /**< Current device. */
    usbd_interface_handle       iface;     /**< Current interface. */
    jint_t                       nifaces;  /**< Number of interfaces. */
    jdevice_t                   parent;    /**< Pointer to the parent 
                                                device. */
};

#define UMATCH_VENDOR_PRODUCT_REV                       (14)
#define UMATCH_VENDOR_PRODUCT                           (13)
#define UMATCH_VENDOR_DEVCLASS_DEVPROTO                 (12)
#define UMATCH_DEVCLASS_DEVSUBCLASS_DEVPROTO            (11)
#define UMATCH_DEVCLASS_DEVSUBCLASS                     (10)
#define UMATCH_VENDOR_PRODUCT_REV_CONF_IFACE            (9)
#define UMATCH_VENDOR_PRODUCT_CONF_IFACE                (8)
#define UMATCH_VENDOR_IFACESUBCLASS_IFACEPROTO          (7)
#define UMATCH_VENDOR_IFACESUBCLASS                     (6)
#define UMATCH_IFACECLASS_IFACESUBCLASS_IFACEPROTO      (5)
#define UMATCH_IFACECLASS_IFACESUBCLASS                 (4)
#define UMATCH_IFACECLASS                               (3)
#define UMATCH_IFACECLASS_GENERIC                       (2)
#define UMATCH_GENERIC                                  (1)
#define UMATCH_NONE                                     (0)

#define USBD_SHOW_DEVICE_CLASS          0x1
#define USBD_SHOW_INTERFACE_CLASS       0x2

jmsleep_h usbd_xfer_get_sleep_h(usbd_xfer_handle xfer);

#define IPL_USB IPL_BIO

/** @} */ /* end_group hsusb_jungo_api */

#endif
