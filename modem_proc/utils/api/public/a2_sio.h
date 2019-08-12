#ifndef A2_SIO_H
#define A2_SIO_H
/*!
  @file a2_sio.h

  @brief
   Interface to the A2 SIO module.

   This SIO interface is to make the A2 appear as a generic serial port
   to it's client (PS/DS).  This interface relates to data sent from PS
   out a USB port.  All data sent and received on this interface will not
   be sent the ipfiltering engine and is unciphered.

   One SIO port corresponds to one usb endpoint.

  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/a2_sio.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/08/14   sn      CR761039: Moved A2 SIO port registration from SIO to A2
09/26/11   yuw     CR 297255: Support back-to-back QMI call on 8960
12/01/09   ar      removed FEATURE_A2_SIO_20 dependency
                   removed a2_sio_set_hdr from header file
11/17/09   yuw     Removed FEATURE_A2_SIO_30 for on-target build compilation errors
08/03/09   yuw     Support PER loopback with SPS and USB
05/19/09   sah     Update SIO port numbers.
05/05/09   sah     Move ip header addition size to external interface.
04/30/09   sah     Add HDLC framing to sio interface.
04/22/09   sah     Add header addition to SIO interface.
04/17/09   sah     Update the code and prototypes to be compatible with
                   the SIO interface.  Add SPS registration code.
01/08/09   sah     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <sio.h>

/*==============================================================================

                               MACROS

==============================================================================*/

/*! @brief the maximum header size for the ipfilter header that the hardware
  can support in bytes*/
#define A2_HW_MAX_HDR_SIZE_IN_BYTES 16

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_sio_dev_init

==============================================================================*/
/*!
  @brief Initializes the A2 SIO device.

  Should be called just once.

*/
/*============================================================================*/
void a2_sio_dev_init( void );


/*==============================================================================

  FUNCTION:  a2_sio_open

==============================================================================*/
/*!
  @brief This function opens a shared memory stream.

  a2_sio_dev_init() must have been called prior to calling this function.

  @return A return code indicating the success of the function.

  @retval SIO_DONE_S     SIO stream as specified in open_ptr has been
                         successfully opened.
  @retval SIO_BADP_S     Bad Parameter
  @retval SIO_UNAVAIL_S  All available streams have been allocated.

*/
/*============================================================================*/
sio_status_type a2_sio_open
(
  /*! SIO open structure containing open params. */
  sio_open_type *open_ptr
);



/*==============================================================================

  FUNCTION:  a2_sio_close

==============================================================================*/
/*!
  @brief This function starts closing the port.

  It saves the client's extended callback function which will be called when
  the port is closed.

  close_cb will be called when all pending data has been received
  by the other processor.

*/
/*============================================================================*/
void a2_sio_close
(
  /*! SIO port to close */
  sio_stream_id_type stream_id,
  /*! callback to call when the stream is closed and all of the queued data
    has been received by the other processor.
    */
  sio_port_id_type port_id,
  sio_vv_func_ptr_type close_cb
);



/*==============================================================================

  FUNCTION:  a2_sio_flush_tx

==============================================================================*/
/*!
  @brief This function flushes the TX and then calls an extended callback.

  User should not queue more data until after the registered callback is
  executed.

*/
/*============================================================================*/
void a2_sio_flush_tx
(
  /*! Stream id */
  sio_stream_id_type stream_id,
  /*! SIO port to flush */
  sio_port_id_type      port_id,
  /*! extended callback to call when all of the queued data has been received
    by the other processor. */
  sio_vv_func_ptr_type flush_cb
);


/*==============================================================================

  FUNCTION:  a2_sio_flush_tx_ext

==============================================================================*/
/*!
  @brief This function flushes the TX and then calls an extended callback.


  User should not queue more data until after the registered callback is
  executed.

*/
/*============================================================================*/
void a2_sio_flush_tx_ext
(
  /*! SIO port to flush */
  sio_port_id_type      port_id,
  /*! extended callback to call when all of the queued data has been received
    by the other processor. */
  sio_vpu_func_ptr_type flush_cb,
  /*! data to use when calling the extended callback */
  void                  *cb_data
);
/*==============================================================================

  FUNCTION:  a2_sio_dev_transmit

==============================================================================*/
/*!
  @brief This function queues the dsm chain and signals the a2 task that data
  is pending.

*/
/*============================================================================*/
void a2_sio_dev_transmit
(
  /*! SIO stream id */
  sio_stream_id_type stream_id,
  /*! SIO port to send the data on */
  sio_port_id_type   port_id,
  /*! pointer to DSM item to send */
  dsm_item_type     *tx_ptr
);



/*==============================================================================

  FUNCTION:  a2_sio_ioctl

==============================================================================*/
/*!
  @brief This function is used to control various features of the A2 interface.

  The behaviour of this function depends on the "command" issued.

*/
/*============================================================================*/
void a2_sio_ioctl
(
  /*! Stream id */
  sio_stream_id_type    stream_id,
  /*! SIO port to control */
  sio_port_id_type      port_id,
  /*! SIO IOCTL command */
  sio_ioctl_cmd_type    cmd,
  /*! Pointer to union containing parameters or results for command - may be
    NULL if the command does not require any parameters
    */
  sio_ioctl_param_type *param
);

/*==============================================================================

  FUNCTION:  a2_sio_control_ioctl

==============================================================================*/
/*!
  @brief This function is used to bypass all control various features of the A2 interface onto SMD.

  The behaviour of this function depends on the "command" issued.

*/
/*============================================================================*/
void a2_sio_control_ioctl
(
  /*! Stream id */
  sio_stream_id_type    stream_id,
  /*! SIO port to control */
  sio_port_id_type      port_id,
  /*! SIO IOCTL command */
  sio_ioctl_cmd_type    cmd,
  /*! Pointer to union containing parameters or results for command - may be
    NULL if the command does not require any parameters
    */
  sio_ioctl_param_type *param
);

/*==============================================================================

  FUNCTION:  a2_sio_disable_device

==============================================================================*/
/*!
  @brief Disables the a2 sio device.

*/
/*============================================================================*/
void a2_sio_disable_device
(
  /*! The port id to disable */
  sio_port_id_type   port_id
);


/*==============================================================================

  FUNCTION:  a2_sio_control_transmit

==============================================================================*/
/*!
  @brief Transmits control information on the specified control port.

*/
/*============================================================================*/
void a2_sio_control_transmit
(
  /*! The stream to tranmit the control information on. */
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  /*! The pointer to the control data to transmit */
  dsm_item_type      *tx_ptr
);

/*==============================================================================

  FUNCTION:  a2_sio_control_close

==============================================================================*/
/*!
  @brief Close the specified control port.

*/
/*============================================================================*/
void a2_sio_control_close
(
  /*! The stream to close */
  sio_stream_id_type stream_id,
  /*! The function pointer to be called when the close is complete */
  sio_port_id_type   port_id,
  void               (*close_func_ptr)(void)
);

/*==============================================================================

  FUNCTION:  a2_sio_control_open

==============================================================================*/
/*!
  @brief Opens the specified control port

*/
/*============================================================================*/
sio_status_type a2_sio_control_open
(
  sio_open_type *open_ptr
);

/*==============================================================================

  FUNCTION:  a2_sio_configure_hdlc

==============================================================================*/
/*!
  @brief This function configures hdlc framing on a particular port.

  The port must be opened already (using sio_open()) prior to calling this
  api.

  Following configuration all data on this port, either direct or bridged
  will be PPP-in-HDLC framed.
*/
/*============================================================================*/
void a2_sio_configure_hdlc
(
  /*! SIO port to control */
  sio_port_id_type      port_id,
  /*! The accm value to apply while framing */
  uint32 accm
);

/*==============================================================================

  FUNCTION:  a2_sio_port_id_to_usb_index

==============================================================================*/
/*!
  @brief Converts a SIO Port id to a usb index.

  The core of the driver does not understand port ids, only usb indices.

*/
/*============================================================================*/
uint8 a2_sio_port_id_to_usb_index ( sio_port_id_type port_id );

/*==============================================================================

  FUNCTION:  a2_sio_driver_init

==============================================================================*/
/*!
  @brief Initializes the A2 SIO driver for SIO v3.0.

*/
/*============================================================================*/
void a2_sio_driver_init();

#endif /* A2_SIO_H */

