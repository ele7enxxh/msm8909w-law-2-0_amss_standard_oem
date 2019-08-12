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


/**@file usbdi_util.h

Contains the USB driver interface general utility functions and macros.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/usbdi_util.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
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

#ifndef _USBDI_UTIL_H_
#define _USBDI_UTIL_H_


/** @ingroup hsusb_jungo_api 
@{ */

#define usbd_get_desc(dev, type, index, len, desc) \
    usbd_get_desc_flags(dev, type, index, len, desc, 0, 0)

#include <uw_otg_app.h>

usbd_status     usbd_get_desc_flags(usbd_device_handle dev, juint8_t type, 
    juint8_t index, juint16_t len, void *desc, juint16_t flags, juint32_t *actlen);
jresult_t usbd_get_interface_power(usbd_device_handle dev, juint8_t *buffer);
jresult_t usbd_set_interface_power(usbd_device_handle dev, juint8_t *buffer);

usbd_status     usbd_get_config_desc(usbd_device_handle dev, juint8_t confidx,
    usb_config_descriptor_t *d);
usbd_status usbd_get_config_desc_full(usbd_device_handle dev, juint8_t index,
    usb_config_descriptor_t **cdp);
usbd_status     usbd_get_device_qualifier(usbd_device_handle dev, 
    usb_device_qualifier_t *d);
usbd_status     usbd_get_device_desc(usbd_device_handle dev,
    usb_device_descriptor_t *d);
usbd_status     usbd_set_address(usbd_device_handle dev, juint8_t addr);
usbd_status     usbd_reset_port(usbd_device_handle dev, juint8_t port,
    usb_port_status_t *ps);
usbd_status     usbd_get_port_status(usbd_device_handle dev, juint8_t port,
    usb_port_status_t *ps);
usbd_status     usbd_set_hub_feature(usbd_device_handle dev, juint16_t sel);
usbd_status     usbd_clear_hub_feature(usbd_device_handle, juint16_t sel);
usbd_status     usbd_set_port_feature(usbd_device_handle dev, juint8_t port,
    juint16_t sel);
usbd_status     usbd_clear_port_feature(usbd_device_handle dev, juint8_t port,
    juint16_t sel);
usbd_status     usbd_get_device_status(usbd_device_handle dev,
    usb_status_t *st);
usbd_status     usbd_get_hub_status(usbd_device_handle dev,
    usb_hub_status_t *st);
usbd_status     usbd_get_config(usbd_device_handle dev, juint8_t *conf);
usbd_status     usbd_get_string_desc(usbd_device_handle dev, juint8_t sindex,
    jint16_t langid,usb_string_descriptor_t *sdesc, juint32_t *sizep);
usbd_status     usbd_set_remote_wakeup(usbd_device_handle dev, jbool_t set);

usbd_status usbd_set_config_no(usbd_device_handle dev, juint8_t no, jint_t msg);
usbd_status usbd_set_config_index(usbd_device_handle dev, juint8_t index,
    jint_t msg);

usbd_status usbd_bulk_transfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    juint16_t flags, juint32_t timeout, void *buf, juint32_t *size);

usbd_status usbd_intr_transfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    juint16_t flags, juint32_t timeout, void *buf, juint32_t *size);

usbd_status usbd_set_otg_mode(usbd_device_handle dev, juint16_t value);
usbd_status usbd_check_iface_support(usbd_device_handle dev,
    usb_interface_descriptor_t *iface, otg_peripheral_state_t *support_state);
jresult_t usbd_get_port_interchip(usbd_device_handle dev, juint8_t port, 
    juint8_t *interchip);

/** @} */ /* end_group hsusb_jungo_api */

#endif
