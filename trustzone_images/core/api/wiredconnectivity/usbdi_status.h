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


/**@file usbdi_status.h

Contains the USB Driver Interface optional status definition.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/usbdi_status.h#1 $
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

#ifndef _USBDI_STATUS_H_
#define _USBDI_STATUS_H_


/** @ingroup hsusb_jungo_api 
@{ */

/** USB driver optional status. */
typedef enum {
        USBD_NORMAL_COMPLETION = 0, /* must be 0 */ /**< &nbsp; */
        USBD_IN_PROGRESS,       /* 1 */ /**< &nbsp; */
        /* errors */
        USBD_PENDING_REQUESTS,  /* 2 */ /**< &nbsp; */
        USBD_NOT_STARTED,       /* 3 */ /**< &nbsp; */
        USBD_INVAL,             /* 4 */ /**< &nbsp; */
        USBD_NOMEM,             /* 5 */ /**< &nbsp; */
        USBD_CANCELLED,         /* 6 */ /**< &nbsp; */
        USBD_BAD_ADDRESS,       /* 7 */ /**< &nbsp; */
        USBD_IN_USE,            /* 8 */ /**< &nbsp; */
        USBD_NO_ADDR,           /* 9 */ /**< &nbsp; */
        USBD_SET_ADDR_FAILED,   /* 10 */ /**< &nbsp; */
        USBD_NO_POWER,          /* 11 */ /**< &nbsp; */
        USBD_TOO_DEEP,          /* 12 */ /**< &nbsp; */
        USBD_IOERROR,           /* 13 */ /**< &nbsp; */
        USBD_NOT_CONFIGURED,    /* 14 */ /**< &nbsp; */
        USBD_TIMEOUT,           /* 15 */ /**< &nbsp; */
        USBD_SHORT_XFER,        /* 16 */ /**< &nbsp; */
        USBD_STALLED,           /* 17 */ /**< &nbsp; */
        USBD_INTERRUPTED,       /* 18 */ /**< &nbsp; */
        USBD_TEST_FAILURE,      /* 19 */ /**< &nbsp; */
        USBD_INVALID_STATE,     /* 20 */ /**< &nbsp; */

        USBD_ERROR_MAX          /* must be last */ /**< &nbsp; */
} usbd_status;

/** @} */ /* end_group hsusb_jungo_api */

#endif
