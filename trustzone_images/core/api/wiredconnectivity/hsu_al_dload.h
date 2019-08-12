#ifndef HSU_AL_DLOAD_H
#define HSU_AL_DLOAD_H

/*=============================================================================

          High Speed USB Download Adaptation Layer

=============================================================================*/
/**
@file hsu_al_dload.h

Defines externalized functions for the mass storage adaptation layer.

Externalized functions:
- hsu_al_dload_drain
- hsu_al_dload_init
- hsu_al_dload_receive_byte
- hsu_al_dload_transmit_byte
- hsu_al_dload_receive_pkt
- hsu_al_dload_transmit_pkt
- hsu_al_dload_shutdown
- hsu_al_dload_obex_init
*/

/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_dload.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who     what, where, why
  --------  ---     -----------------------------------------------------------
  06/24/10   lo     (Tech Pubs) Edited/added Doxygen comments and markup.      
    
=============================================================================*/

#include "comdef.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "jtypes.h"

/** @ingroup hsusb_qc_api
    @{
*/

/** Error return code from hsu_al_dload_receive_byte. This int value must not
    be a valid unsigned character value.
*/
#define  USB_NO_CHAR    (-1)

/** Defines the interface type.
*/
typedef enum
{
  INTERFACE_UNKNOWN, /**< &nbsp; */
  INTERFACE_UART1,   /**< &nbsp; */
  INTERFACE_USB      /**< &nbsp; */
} 
hsu_al_dload_interface_type;

/*=============================================================================
FUNCTION    hsu_al_dload_drain
=============================================================================*/
/**
Waits for the completion of any pending IN transfers.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_drain(void);

/*=============================================================================
FUNCTION    hsu_al_dload_init
=============================================================================*/
/**
Initializes HS-USB support for a download by calling hsu_al_dload_init_ex
with the right parameter.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_init(void);

/*=============================================================================
FUNCTION    hsu_al_dload_init_ex
=============================================================================*/
/**
Initializes HS-USB support for download. 

@param use_amss_product_id [IN] -- specifies whether to enumerate using
                                   the same product ID used when running 
                                   in AMSS (non-download mode).

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_init_ex(boolean use_amss_product_id);

/*=============================================================================
FUNCTION    hsu_al_dload_receive_byte
=============================================================================*/
/**
Receives incoming data from the USB bulk OUT EP and returns one character at a
time to the calling function. Although this function receives a bigger packet
at one time, it always returns one character to the calling function. This
approach was chosen for consistency between the UART and USB modules.

@dependencies
None.

@sideeffects
None.

@return
Returns the next character from the receive buffer, or USB_NO_CHAR if there is
no character to return.
*/
int hsu_al_dload_receive_byte(void);

/*=============================================================================
FUNCTION    hsu_al_dload_transmit_byte
=============================================================================*/
/**
Adds a byte to the transmit buffer. If the byte signifies the end of a packet,
the buffer is transmitted to the host.

@param chr [IN] -- Byte to be transmitted.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_transmit_byte(byte chr);

/*=============================================================================
FUNCTION    hsu_al_dload_receive_pkt
=============================================================================*/
/**
Receives incoming data from the USB bulk OUT EP and returns the contents of the
receive buffer to the calling function. It is up to the calling function to
parse the data. A pointer to the packet is stored in the argument passed
by the caller and the length in bytes is returned.

@param **buf [IN/OUT] -- Address to a buffer space for the incoming packet.

@dependencies
None.

@sideeffects
None.

@return
Returns a pointer to the received packet in the argument passed by the calling
function and returns the length of the packet in bytes. Returns NULL and 0 if
there is no data in the receive buffer.
*/
uint32 hsu_al_dload_receive_pkt(byte **buf);

/*=============================================================================
FUNCTION    hsu_al_dload_transmit_pkt
=============================================================================*/
/**
Buffers the packet into the USB Tx buffer and initiates the transmission of
the packet over the USB. This is not a generic routine and it only transmits
packets that are smaller than or equal to the USB Tx buffer. Packets that are
larger than the USB Tx buffer are discarded.

@param *pkt [IN] -- Pointer to the packet to be transmitted.
@param len  [IN] -- Number of bytes to transmit.

@dependencies
The USB device must be enumerated. The packet must fit in the Tx buffer.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_transmit_pkt
(
  byte *pkt, 
  uint32 len 
);

/*=============================================================================
FUNCTION    hsu_al_dload_shutdown
=============================================================================*/
/**
Shuts down the HS-USB stack.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_shutdown(void);

/*=============================================================================
FUNCTION    hsu_al_dload_mark_dload_used_hsu
=============================================================================*/
/**
Sets a marker in IRAM so that ARMPRG knows that a download over HS-USB was
successful.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_mark_dload_used_hsu(void);

/*=============================================================================
FUNCTION    hsu_al_dload_obex_init
=============================================================================*/
/**
The OBEX initialization function is called by hsu_al_oemsbl_stack_sel, which is 
called by the function driver on initialization. This function sets application
context and callbacks.

@param handle  [IN] -- CDC instance handle.
@param *cbs    [OUT] -- Callbacks array.
@param *info   [OUT] -- OBEX information.
@param *ctx    [OUT] -- CDC context.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success, an error number for failure.
*/
jresult_t hsu_al_dload_obex_init
(
 cdc_handle_t handle, 
 obex_callbacks_t* cbs, 
 obex_init_info_t* info, 
 cdc_appctx_t* ctx
);

/*=============================================================================
FUNCTION    hsu_al_dload_drain_timeout
=============================================================================*/
/**
Waits for the completion of any pending IN transfers or the specified time in
milliseconds, whichever occurs first.

@param timeout_ms [IN] -- The timeout (in ms) to allow for the completion of
                          the IN transfer.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_dload_drain_timeout(uint32 timeout_ms);

/** @} */ /* endgroup hsusb_qc_api */

#endif
